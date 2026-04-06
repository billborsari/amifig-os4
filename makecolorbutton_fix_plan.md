# Implementation Plan: Correcting MakeColorButton & Universal Pen Updates

## 1. Problem Overview
The `MakeColorButton` instances across the AmiFIG GUI (`Plain` Fill swatches, `Pen` swatches, `User` palettes) are failing to adequately read initialized color states on screen due to improper `MUIA_Colorfield` parameter allocation or missing redraw notifications under Amiga MUI. When the user selects or changes a color, the surrounding dependent gadgets (like the active "Pen" or "Fill" previews) do not reliably synchronize to visually reflect the new RGB data.

## 2. Root Cause Analysis
- **Initialization Binding:** `MakeColorButton` constructs `ColorfieldObject` using `MUIA_Colorfield_Red`, `Green`, and `Blue` with 32-bit (0x01010101 expanded) integers inline. However, depending on MUI environment variables and macro expansions at boot, attributes configured during instantiation inside `MUI_NewObject` may not attach correctly or initialize into the display space. `MUIA_Colorfield_RGB` (which takes an array of three 32-bit values) is often more robust.
- **State Synchronization:** The `SetUsrColBut` function handles updating a subset of color rectangles manually (e.g., `BUT_USRCOL`, `BUT_PENCOL`, `BUT_FILCOL`), but it requires explicit external calls. Modifying shades or dynamically opening panels skips updating these pointers.
- **Redraw Events:** Scattered `SetAttrs` calls push the memory states but do not globally invalidate the window regions, causing stale black/blank swatches.

## 3. Step-by-Step Implementation Strategy

### Step 1: Standardize `MakeColorButton` Initialization
Instead of configuring the `MUIA_Colorfield_Red/Green/Blue` attributes directly inside the macro builder, we should:
1. Build the bare gadget struct: `ColorfieldObject, MUIA_Frame, MUIV_Frame_ImageButton, MUIA_InputMode... End`.
2. Explicitly cast a localized helper routine `SetColorButtonRGB(button, rgb)` to assign the colors immediately after object creation to ensure the MUI state machine receives it actively.
3. **CRITICAL:** `SetColorButtonRGB` must include an explicit `MUI_Redraw(button, MADF_DRAWUPDATE)` call. Without this, some Zune versions will update the internal attribute but fail to visually invalidate the gadget region.

### Step 2: Implement a Global Synchronization Routine
Create a definitive, single-source-of-truth function in `z_color.c` (e.g., `UpdateAllColorButtons(int index, ULONG rgb)`) which universally loops through and fires `SetAttrs` + `MUI_Redraw` for:
- `BUT_PENCOL[index]`
- `BUT_FILCOL[index]`
- `BUT_USRCOL[index]`
- `BUT_FSHADES[shade_index]` (if a base fill color is updated, force an automated trigger to `UpdateFShades()` rather than relying on GUI clicks).

### Step 3: Hooking the User Events
Locate all event hooks that physically alter colors:
- `DoColorPicker`
- `DoFilColorPal`
- `DoPenColorPal`
- Color adjust sliders.
Modify these hooks so that after they update their local `ColorPalette[i].RGB` array data, they universally dispatch a call to `UpdateAllColorButtons()`.

### Step 4: Validate Fill Pattern & Shade Visibility Updates
The `DoFMode` visibility manager guarantees the "Plain" shade gadget containers are visible, but we must ensure that `UpdateFShades()` is called unconditionally *every single time* the "Fill Type" cycle changes to 1 (Plain), forcing it to read the current system `cur_fillcolor` instead of remaining cached as black.

### Step 5: Fill Plain Chooser UI Synchronization
Specific attention is required for the `SG_FILPAL` scrollgroup:
1. When entering `Plain` mode in `DoFMode`, a bulk refresh of the 544 color swatches is needed to ensure that user-defined color changes made elsewhere (e.g., Pen tab or User Colors window) are physically pushed to the Fill tab's gadgets.
2. The `BUT_FILCOL[cur_fillcolor]` selection state must be explicitly set to `TRUE` to provide immediate visual feedback of the current selection.

## Conclusion
By standardizing color assignment into a single `SetAttr` + `MUI_Redraw` pipeline across all `ColorfieldObject`s and establishing a master loop (`RefreshAllColorButtons`) to dispatch redraw requests immediately, all GUI tabs will remain 100% physically synchronized with the active internal `ColorPalette` struct model.

---

# Part 2: Detailed Reference — Every GUI Palette/Swatch Rendering Location

This section provides an exhaustive, code-referenced inventory of every place the GUI renders a palette or color swatch. For each location, we describe **what data set it represents**, **how it should be updated**, and **specific implementation suggestions**.

---

## 4. Master Data Structures

Before diving into each GUI element, here are the core data structures everything depends on:

### 4.1. `ColorPalette[]` — The Single Source of Truth
- **File:** `resources.h` line 498, defined as:
  ```c
  extern Colors ColorPalette[NUM_STD_COLS + MAX_USR_COLS];
  // NUM_STD_COLS = 32  (resources.h:78)
  // MAX_USR_COLS = 512 (resources.h:80)
  ```
- **Type:** `Colors` struct (`resources.h` lines 458–464):
  ```c
  typedef struct _Colors {
      ULONG   RGB;      // 0xAARRGGBB packed color
      BOOL    Used;     // Is this color referenced by any object?
      BOOL    Defined;  // Has this color been explicitly set?
  } Colors;
  ```
- **Indices 0–31:** Standard (built-in) colors (e.g., BLACK=0, WHITE=7, etc.)
- **Indices 32–543:** User-defined colors (up to 512 user colors)

### 4.2. `GradientTable[]` — Gradient Fill Definitions
- **File:** `resources.h` line 496:
  ```c
  extern Gradient *GradientTable[MAX_GRD_COLS]; // MAX_GRD_COLS = 512
  ```
- Each `Gradient` has up to 16 `GradientStop` entries, each with a `.color` (0xAARRGGBB) and `.stopvalue` (0–100).

### 4.3. Key Global State Variables
- `cur_pencolor` — Index into `ColorPalette[]` for the active pen (outline) color.
- `cur_fillcolor` — Index into `ColorPalette[]` for the active fill color.
- `cur_fillstyle` — Active fill style shade index (0–40) or pattern offset.
- `CurrentSelectedUsrCol` — Index (0-based into user colors) of the selected cell in the User Colors window.
- `CurrentSelectedGrad` — Which gradient (0–511) is being edited.
- `CurrentSelectedGradStop` — Which stop (0–15) within a gradient is being edited.

---

## 5. GUI Palette/Swatch Locations — Complete Inventory

### 5.1. Pen Color Palette (`BUT_PENCOL[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_PENCOL[NUM_STD_COLS + MAX_USR_COLS]` (544 buttons) |
| **Declared** | `z_edit.c` line 263 |
| **Created** | `MakePenGroup()` — `z_edit.c` lines 411–498 |
| **Initial Data** | `ColorPalette[i].RGB` for `i = 0 .. (NUM_STD_COLS + MAX_USR_COLS - 1)` |
| **Creation Code** | `z_edit.c` line 418: `BUT_PENCOL[i] = MakeColorButton(ColorPalette[i].RGB, COLORCELLSIZE, COLORCELLSIZE);` |
| **Layout** | Arranged into rows of 16 (`PLINE_BUT[]`), split into "Standard Colors" (rows 0–1, indices 0–31) and "User Colors" (rows 2–33, indices 32–543) |
| **Selection Hook** | `DoPenColorPal` — `z_edit.c` lines 1531–1546. Sets `cur_pencolor` to the clicked index. |
| **Hook Binding** | `z_edit.c` line 1323: `DoMethod(BUT_PENCOL[i], MUIM_Notify, MUIA_Selected, TRUE, ..., &DoPenColorPalHook, 0);` |
| **Currently Updated By** | `SetUsrColBut()` — `z_color.c` lines 1525–1526 (only for user-color indices) |

**Data Set:** This palette mirrors the entire `ColorPalette[]` array. The first 32 entries are standard and never change at runtime. Entries 32+ correspond to user-defined colors that can be modified via the User Colors window, file loading, or the color picker.

**How It Should Be Updated:**
- Whenever `Add_UserColor(indx, RGB)` is called (which sets `ColorPalette[indx+NUM_STD_COLS].RGB`), the corresponding `BUT_PENCOL[indx+NUM_STD_COLS]` must have its `MUIA_Colorfield_Red/Green/Blue` attrs updated and be redrawn.
- Currently: `SetUsrColBut()` handles this correctly for user colors, but it is **only called from `Add_UserColor()`**.
- **Missing:** When a file is loaded via `f_read.c` and new user colors are imported (line 2001, 2045), `Add_UserColor` is called which chains to `SetUsrColBut()`. However, if `BUT_PENCOL[]` has not yet been attached to an MUI window (during startup initialization), the `MUI_Redraw` call silently fails or crashes. A guard is needed.

**Specific Implementation Suggestions:**
1. In `SetUsrColBut()` (`z_color.c` line 1510), add a NULL check before each `SetAttrs`/`MUI_Redraw` pair:
   ```c
   if (BUT_PENCOL[usrcol+NUM_STD_COLS]) {
       SetAttrs(BUT_PENCOL[usrcol+NUM_STD_COLS], MUIA_Colorfield_Red, (IPTR)r, ...);
       MUI_Redraw(BUT_PENCOL[usrcol+NUM_STD_COLS], MADF_DRAWUPDATE);
   }
   ```
2. After file load completes and all windows are open, call a bulk refresh:
   ```c
   void RefreshAllPenButtons(void) {
       for (int i = 0; i < NUM_STD_COLS + MAX_USR_COLS; i++) {
           if (BUT_PENCOL[i]) SetColorButtonRGB(BUT_PENCOL[i], ColorPalette[i].RGB);
       }
   }
   ```
3. `SetColorButtonRGB()` is a new helper (see Section 6.1) that extracts R/G/B from packed RGB, calls `SetAttrs` + `MUI_Redraw`.

---

### 5.2. Fill Color Palette (`BUT_FILCOL[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_FILCOL[NUM_STD_COLS + MAX_USR_COLS]` (544 buttons) |
| **Declared** | `z_edit.c` line 266 |
| **Created** | `MakeFillGroup()` — `z_edit.c` lines 503–590 |
| **Initial Data** | `ColorPalette[i].RGB` for `i = 0 .. (NUM_STD_COLS + MAX_USR_COLS - 1)` |
| **Creation Code** | `z_edit.c` line 510: `BUT_FILCOL[i] = MakeColorButton(ColorPalette[i].RGB, COLORCELLSIZE, COLORCELLSIZE);` |
| **Selection Hook** | `DoFilColorPal` — `z_edit.c` lines 1560–1579. Sets `cur_fillcolor` and calls `UpdateFShades()`. |
| **Hook Binding** | `z_edit.c` line 1325: `DoMethod(BUT_FILCOL[i], MUIM_Notify, MUIA_Selected, TRUE, ..., &DoFilColorPalHook, 0);` |
| **Currently Updated By** | `SetUsrColBut()` — `z_color.c` lines 1528–1529 (only for user-color indices) |

**Data Set:** Identical mirroring of `ColorPalette[]`, just like the Pen palette but in the Fill tab. The fill palette is an independent set of 544 MUI `ColorfieldObject` widgets.

**How It Should Be Updated:**
- Same mechanism as Pen palette — `SetUsrColBut()` already pushes changes here.
- **Critical Bug:** When `DoFilColorPal` is invoked (user clicks a fill color), it calls `UpdateFShades()` then `set(BUT_FSHADES[20], MUIA_Selected, TRUE)`. But it does **not** verify that the newly selected `cur_fillcolor` maps to a valid/updated swatch color. If the user-defined color was recently changed via the color adjuster, the `BUT_FILCOL[]` swatch may be stale.

**Specific Implementation Suggestions:**
1. Same NULL-guard pattern as Pen buttons in `SetUsrColBut()`.
2. Add a `RefreshAllFillButtons()` companion to `RefreshAllPenButtons()`.
3. In `DoFilColorPal` (`z_edit.c` line 1571), after setting `cur_fillcolor = i`, explicitly call the new `SetColorButtonRGB(BUT_FILCOL[i], ColorPalette[i].RGB)` to force the swatch to re-read its color before `UpdateFShades()` is invoked with that color.

---

### 5.3. User Colors Palette (`BUT_USRCOL[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_USRCOL[MAX_USR_COLS]` (512 buttons) |
| **Declared** | `z_color.c` line 131 |
| **Created** | `MakeColorWindow()` — `z_color.c` lines 158–315 |
| **Initial Data** | `ColorPalette[i+NUM_STD_COLS].RGB` for `i = 0 .. MAX_USR_COLS-1` |
| **Creation Code** | `z_color.c` line 169: `BUT_USRCOL[i] = MakeColorButton(ColorPalette[i+NUM_STD_COLS].RGB, COLORCELLSIZE, COLORCELLSIZE);` |
| **Layout** | 32 rows of 16, arranged in two scroll-groups of 16 rows each (`LINE_USRPAL[0..15]` and `LINE_USRPAL[16..31]`) within `VG_USRPAL` |
| **Selection Hook** | `DoColorMutex` — `z_color.c` lines 607–676. Sets `CurrentSelectedUsrCol`, also handles gradient-build, copy, and swap operations. Updates `CAD_PENCOL` color adjuster wheel. |
| **Hook Binding** | `z_color.c` lines 361–362 |
| **Currently Updated By** | `SetUsrColBut()` — `z_color.c` lines 1522–1523 |

**Data Set:** This palette represents the **user-defined color slots** only (`ColorPalette[32..543]`). These can be edited via:
- The `CAD_PENCOL` `ColoradjustObject` (MUI color wheel/slider) → triggers `DoColorAdj` hook
- The color picker tool (`BUT_COL_CP`) → triggers `DoColorPicker` hook
- The ARGB hex string field (`STR_COL_ARGB`) → triggers `DoColorString` hook
- Copy/Swap/Gradient operations in `DoColorMutex`
- File load via `Add_UserColor()` in `f_read.c`

**How It Should Be Updated:**
- All modification paths call `Add_UserColor()` which calls `SetUsrColBut()`, which already updates `BUT_USRCOL[]`, `BUT_PENCOL[]`, and `BUT_FILCOL[]` plus conditionally calls `UpdateFShades()`.
- **Bug:** The `DoColorAdj` hook (`z_color.c` lines 688–712) calls `Add_UserColor(CurrentSelectedUsrCol, value)` — but the `value` calculation has incorrect operator precedence:
  ```c
  value = ((a << 24) | (r & 0xFF000000) >> 8 | (g & 0xFF000000) >> 16 | (b & 0xFF000000) >> 24);
  ```
  The `>>` operator has lower precedence than `|`, so this should be:
  ```c
  value = (a << 24) | ((r & 0xFF000000) >> 8) | ((g & 0xFF000000) >> 16) | ((b & 0xFF000000) >> 24);
  ```
  This same bug exists in `DoGradColorAdj` (`z_color.c` line 856).

**Specific Implementation Suggestions:**
1. **Fix operator precedence** in `DoColorAdj` (line 704) and `DoGradColorAdj` (line 856) by adding parentheses around each `>> N` operation.
2. In `SetUsrColBut()`, the function currently uses `extern` declarations for `BUT_PENCOL` and `BUT_FILCOL` — these should be moved to a shared header (e.g. add to `z_edit.h` or create a new `z_color_internal.h`).
3. Add NULL guards as described in 5.1.

---

### 5.4. Fill Shade Swatches (`BUT_FSHADES[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_FSHADES[NUMSHADES]` — 41 buttons (NUMSHADES = NUMSHADEPATS + NUMTINTPATS = 21 + 20 = 41) |
| **Declared** | `z_edit.c` line 267 |
| **Created** | `MakeShadesGroup()` — `z_edit.c` lines 595–657 |
| **Initial Data** | `getfillcolor(cur_fillcolor, i)` for i ≠ 20; index 20 = `ColorPalette[cur_fillcolor].RGB` (the "pure" base color, displayed larger at 4×COLORCELLSIZE) |
| **Creation Code** | `z_edit.c` lines 602–604 |
| **Layout** | Index 20 is the large center swatch. Indices 0–19 are shades (dark→base), 21–40 are tints (base→light). Arranged in a group of 10 columns × 4 rows. |
| **Selection Hook** | `SetFillStyle` — `z_edit.c` lines 1644–1659. Sets `cur_fillstyle`. |
| **Hook Binding** | `z_edit.c` lines 1332–1333 |
| **Currently Updated By** | `UpdateFShades()` — `z_edit.c` lines 1890–1905 |

**Data Set:** These buttons display the **shade/tint ramp** derived from the currently selected fill color (`cur_fillcolor`). The `getfillcolor(BaseColor, shade)` function (`a_color.c` lines 139–191) computes:
- shade 0 = pure black
- shade 1–19 = progressively lighter toward the base color
- shade 20 = the base color itself
- shade 21–39 = progressively lighter toward white
- shade 40 = pure white

**How It Should Be Updated:**
- `UpdateFShades()` is the correct mechanism — it iterates all 41 shades, calls `getfillcolor()` to recompute the RGB, then `SetAttrs` + `MUI_Redraw` each `BUT_FSHADES[i]`.
- **Bug 1:** `UpdateFShades()` uses `NUMSHADES` (41) as the loop bound. However, if `cur_fillcolor` refers to a gradient (index ≥ `NUM_STD_COLS + MAX_USR_COLS`), `getfillcolor()` returns `0x00000000` because `BaseColor >= NUM_STD_COLS + MAX_USR_COLS` falls through to the end of the function with `OutPen = 0`. This means all shades turn black when a gradient is selected.
  - **Fix:** In `getfillcolor()` (`a_color.c` line 147), the guard `if (BaseColor < NUM_STD_COLS + MAX_USR_COLS)` should return early with a sensible default (e.g., `0xFF000000`) when BaseColor is out of range.

- **Bug 2:** `DoFMode` (`z_edit.c` lines 1706–1752) switches fill mode. When switching to "Plain" (case 1, line 1718), it sets `cur_fillcolor = WHITE` then calls `UpdateFShades()`. However, `cur_fillcolor` might have been a user color before, and the shades panel should show the color corresponding to whichever fill palette cell is selected, not always white.
  - **Fix:** In `DoFMode` case 1, instead of hardcoding `cur_fillcolor = WHITE`, read the currently selected `BUT_FILCOL[]` state, or just call `UpdateFShades()` without resetting `cur_fillcolor`.

- **Bug 3:** When a user color that is currently `cur_fillcolor` is changed (via color wheel or picker), `SetUsrColBut()` correctly calls `UpdateFShades()`. However, this update happens **before** `ColorPalette[].RGB` has been fully committed by `Add_UserColor`, creating a race. In `Add_UserColor` (`a_color.c` lines 98–123), the RGB is written at line 115 and `SetUsrColBut` is called at line 117 — this order is correct, so this is NOT actually a race. The real issue is that `SetUsrColBut` only checks `if ((usrcol+NUM_STD_COLS) == cur_fillcolor)` — this comparison is correct. **No bug here**, but verify this logic path is being exercised.

**Specific Implementation Suggestions:**
1. Add a safety clamp in `getfillcolor()`:
   ```c
   if (BaseColor < 0 || BaseColor >= NUM_STD_COLS + MAX_USR_COLS)
       return 0xFF000000; // opaque black fallback
   ```
2. In `DoFMode` case 1 (`z_edit.c` line 1720), change:
   ```c
   // BEFORE (always resets to white):
   cur_fillcolor = WHITE;
   // AFTER (preserve current selection or default properly):
   if (cur_fillcolor >= NUM_STD_COLS + MAX_USR_COLS)
       cur_fillcolor = WHITE; // only reset if was a gradient
   ```
3. In `z_refreshAttributes()` (`z_edit.c` line 1939), `UpdateFShades()` is already called. Verify this function is called after every file load. (It is — via `setPenColor()`/`setFillColor()`/`setFillStyle()` in `a_color.c` which call `z_refreshAttributes()`.)

---

### 5.5. Fill Pattern Buttons (`BUT_FPATTERNS[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_FPATTERNS[NUMPATTERNS]` — 22 buttons |
| **Declared** | `z_edit.c` line 267 |
| **Created** | `MakePatternsGroup()` — `z_edit.c` lines 662–721 |
| **Type** | `ImageObject` (NOT `ColorfieldObject`) — these display 16×16 bitmap patterns, not colors |
| **Selection Hook** | `SetFillPattern` — `z_edit.c` lines 1673–1688 |

**Data Set:** These are **not** color swatches. They are monochrome 16×16 bitmap pattern images from the `patternPointers[]` array. They represent crosshatch/fill patterns (hatching, dots, bricks, etc.).

**How They Should Be Updated:** These do not display color data and do not need color refresh. They are static images. **No changes needed** for the color synchronization problem.

---

### 5.6. Gradient Start & Stop Color Swatches (`BUT_GRADCOLOR[]`)

| Item | Detail |
|------|--------|
| **Widget Array** | `BUT_GRADCOLOR[16]` (up to 16 gradient stops) |
| **Declared** | `z_color.c` line 138 |
| **Created** | `MakeGradGroup()` — `z_color.c` lines 412–595 |
| **Initial Data** | `0x000F0F0F * i` — a simple grey ramp placeholder |
| **Creation Code** | `z_color.c` line 494: `BUT_GRADCOLOR[i] = MakeColorButton(0x000F0F0F*i, COLORCELLSIZE, COLORCELLSIZE);` |
| **Selection Hook** | `DoGradColorMutex` — `z_color.c` lines 773–829. Sets `CurrentSelectedGradStop`. |
| **Currently Updated By** | `updategradcolors()` — `z_color.c` — uses `SetColorButtonRGB()` ✅ |

**Data Set:** These represent the **color stops of the currently selected gradient** (`GradientTable[CurrentSelectedGrad]->Stop[i].color`). Only `stops` entries are visible; the rest are hidden via `set(GRP_GRADSTOP[i], MUIA_ShowMe, FALSE)`.

**Gradient Start & Stop Initialization:**
When a new gradient is created via `DoGradAdd` (case 0), two default stops are set up:
- **Start (Stop[0]):** color = `0xFF000000` (opaque black), stopvalue = `0`
- **End (Stop[1]):** color = `0xFFFFFFFF` (opaque white), stopvalue = `100`

When additional stops are added via `DoGradAddStop`, the new stop is placed at the **midpoint** between the previous last stop's position and 100, ensuring stops don't pile up at the end. The new stop's color defaults to opaque black (`0xFF000000`).

**How It Should Be Updated:**
- `updategradcolors()` is the correct mechanism. It reads each active stop's `.color` field and calls `SetColorButtonRGB()` on `BUT_GRADCOLOR[i]`. ✅ Already uses `SetColorButtonRGB`.
- ~~**Bug:** The `-gradcolor` buttons are initialized with placeholder colors during `MakeGradGroup`, but `updategradcolors()` is NOT called during initialization.~~ ✅ **Fixed:** `z_color_methods()` now calls `updategradcolors()` at init if `GradientTable[0] != NULL`.
- ~~**Bug:** `DoGradColorAdj` had operator precedence issues.~~ ✅ **Fixed:** Parentheses are now correct.
- `DoGradColorAdj` only updates the selected swatch (`BUT_GRADCOLOR[CurrentSelectedGradStop]`) during live dragging; the gradient preview (`GRD_PREV`) is NOT updated during live slider movement to avoid performance issues. The preview updates on discrete actions (ARGB string entry, color picker commit, stop add/remove/swap, or gradient switching).

**Specific Implementation Suggestions:**
1. ✅ `updategradcolors()` called at end of `z_color_methods()` for initial display.
2. ✅ Operator precedence fixed in `DoGradColorAdj`.
3. ✅ `DoGradAddStop` now computes midpoint stop position instead of always 100.

---

### 5.7. Gradient Preview in Edit Window (`GRD_PREVIEW`)

| Item | Detail |
|------|--------|
| **Widget** | `GRD_PREVIEW` (single custom MCC_GRADDISP object) |
| **Declared** | `z_edit.c` line 269 |
| **Set via** | `set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[...]);` |

**Data Set:** A custom MUI class (`mcc_graddisp.c`) that renders a visual preview of the **currently selected gradient** as applied to the fill.

**How It Should Be Updated:** Updated via `MYATTR_GRAD` whenever gradient parameters change. This is done in many places throughout `z_color.c` (approximately 15 call sites). All these call sites already check `if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)` before updating `GRD_PREVIEW`. This logic is correct.

**No additional changes needed** for `GRD_PREVIEW`.

---

### 5.8. Gradient Editor Preview (`GRD_PREV`)

| Item | Detail |
|------|--------|
| **Widget** | `GRD_PREV` (single custom MCC_GRADDISP object in the color window) |
| **Declared** | `z_color.c` line 139: implicit in `MakeGradGroup()` |
| **Set via** | `set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);` |

**Data Set:** Same as `GRD_PREVIEW` but in the gradient editor tab of the User Colors window. Shows real-time gradient preview while editing stops.

**How It Should Be Updated:** All the call sites that update `GRD_PREVIEW` also update `GRD_PREV`. This appears to be handled correctly.

**No additional changes needed** for `GRD_PREV`.

---

### 5.9. Color Adjust Widget (`CAD_PENCOL`)

| Item | Detail |
|------|--------|
| **Widget** | `CAD_PENCOL` — a `ColoradjustObject` (MUI built-in color wheel + RGB sliders) |
| **Declared** | In `MakeColorWindow()`, `z_color.c` (approximately line 300) |
| **Data Source** | Reflects `ColorPalette[CurrentSelectedUsrCol + NUM_STD_COLS].RGB` |
| **Hook** | `DoColorAdj` — fires on every slider movement. Reads R/G/B from the widget, packs to ARGB, and calls `Add_UserColor()`. |

**Data Set:** This widget is the **editing interface** for modifying a single user color. It reads/writes `ColorPalette[CurrentSelectedUsrCol + NUM_STD_COLS]`.

**How It Should Be Updated:**
- When a new user color cell is clicked (`DoColorMutex`, `z_color.c` lines 663–672), the color adjust widget is updated via `nnset(CAD_PENCOL, MUIA_Coloradjust_Red/Green/Blue, ...)`.
- The `IgnoreColorAdj` flag prevents re-entrant notification loops.
- **Known Issue:** The hex string field (`STR_COL_ARGB`) and the alpha slider (`NUM_USRALPHA`) are updated too, but only in `DoColorMutex`. If `DoColorPicker` modifies the color, it updates `CAD_PENCOL` but the alpha+hex string also need updating — and they ARE updated in `DoColorPicker` (lines 1394–1400). This path looks correct.

**No additional changes needed** for `CAD_PENCOL` itself.

---

## 6. Proposed New Functions

### 6.1. `SetColorButtonRGB()` — Centralized Color Button Updater

**File:** Add to `z_color.c` (or a new shared utility file).

```c
/*
 * SetColorButtonRGB - update a ColorfieldObject's RGB and force redraw
 * Safely handles NULL objects (e.g., before window is opened).
 */
void SetColorButtonRGB(APTR button, ULONG rgb)
{
    ULONG r, g, b;

    if (!button) return;

    r = ((rgb & 0x00FF0000) >> 16) * 0x01010101;
    g = ((rgb & 0x0000FF00) >>  8) * 0x01010101;
    b = ((rgb & 0x000000FF)      ) * 0x01010101;

    SetAttrs(button,
        MUIA_Colorfield_Red,   (IPTR)r,
        MUIA_Colorfield_Green, (IPTR)g,
        MUIA_Colorfield_Blue,  (IPTR)b,
        TAG_DONE);
    MUI_Redraw(button, MADF_DRAWUPDATE);
}
```

**Prototype:** Add to `z_color.h`:
```c
void SetColorButtonRGB(APTR button, ULONG rgb);
```

**Usage:** Replace every `SetAttrs(..., MUIA_Colorfield_Red, ...)` + `MUI_Redraw(...)` pair in:
- `SetUsrColBut()` (3 pairs, `z_color.c` lines 1522–1529)
- `UpdateFShades()` (1 pair in a loop, `z_edit.c` lines 1901–1902)
- `updategradcolors()` (1 pair in a loop, `z_color.c` lines 1497–1498)
- `DoGradColorAdj` (1 pair, `z_color.c` lines 862–863)
- `MakeColorButton()` itself (1 pair, `z_fig.c` lines 406–410)

### 6.2. `RefreshAllColorButtons()` — Post-Load Full Sync

**File:** Add to `z_color.c`.

```c
void RefreshAllColorButtons(void)
{
    extern APTR BUT_PENCOL[];
    extern APTR BUT_FILCOL[];
    int i;

    /* Refresh all pen and fill buttons from ColorPalette */
    for (i = 0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        SetColorButtonRGB(BUT_PENCOL[i], ColorPalette[i].RGB);
        SetColorButtonRGB(BUT_FILCOL[i], ColorPalette[i].RGB);
    }

    /* Refresh user color buttons */
    for (i = 0; i < MAX_USR_COLS; i++)
    {
        SetColorButtonRGB(BUT_USRCOL[i], ColorPalette[i + NUM_STD_COLS].RGB);
    }

    /* Refresh shade ramp */
    UpdateFShades();
}
```

**Prototype:** Add to `z_color.h`:
```c
void RefreshAllColorButtons(void);
```

**Call Sites:** Invoke this:
1. After file load completes in `f_load.c` (after `read_figc()` returns successfully)
2. After `Init_UserColors()` if the window is already open
3. After any bulk color import (e.g., SVG import via `svg2fig`)

### 6.3. Revised `SetUsrColBut()` — Using the New Helper

```c
void SetUsrColBut(int usrcol, ULONG value)
{
    extern APTR BUT_PENCOL[];
    extern APTR BUT_FILCOL[];
    extern int cur_fillcolor;
    extern void UpdateFShades(void);

    SetColorButtonRGB(BUT_USRCOL[usrcol], value);
    SetColorButtonRGB(BUT_PENCOL[usrcol + NUM_STD_COLS], value);
    SetColorButtonRGB(BUT_FILCOL[usrcol + NUM_STD_COLS], value);

    if ((usrcol + NUM_STD_COLS) == cur_fillcolor)
    {
        UpdateFShades();
    }
}
```

## 6. Standardized UI Area to Function Mapping

To ensure there is no ambiguity across the codebase, the following table defines which discrete UI area is managed by which creation and refresh functions.

### 6.1. Current Function Mapping

| UI Area | Logical Name | Widget Array | Creation Scope | Refresh / Sync Function |
|:---|:---|:---|:---|:---|
| **Pen Tab** | Pen Palette | `BUT_PENCOL[]` | `MakePenGroup()` | `RefreshAllColorButtons()` |
| **Fill Tab (Plain)** | Fill Palette | `BUT_FILCOL[]` | `MakeFillGroup()` | `RefreshAllColorButtons()` |
| **Fill Tab (Shades)** | Shade Ramp | `BUT_FSHADES[]` | `MakeShadesGroup()` | `UpdateFShades()` |
| **User Colors Win** | Color Grid | `BUT_USRCOL[]` | `MakeColorWindow()` | `SetUsrColBut()` |
| **Gradient Editor** | Stop Swatches | `BUT_GRADCOLOR[]` | `MakeGradGroup()` | `updategradcolors()` |
| **Gradient Editor** | Local Preview | `GRD_PREV` | `MakeGradGroup()` | `set(..., MYATTR_GRAD, ...)` |
| **Fill Tab (Main)** | Global Preview| `GRD_PREVIEW` | `z_edit.c` | `set(..., MYATTR_GRAD, ...)` |

### 6.2. Proposed Functional Refactor (Clarity Plan)

The current naming convention (`updategradcolors` lowercase vs `UpdateFShades` CamelCase vs `SetUsrColBut` abbreviation) is inconsistent. We plan to introduce clear, area-specific wrapper functions:

1.  `SyncPenPalette(int index)` - Specifically updates one swatch in the Pen tab.
2.  `SyncFillPalette(int index)` - Specifically updates one swatch in the Fill tab.
3.  `SyncUserPalette(int index)` - Specifically updates one swatch in the User Colors window.
4.  `SyncShadeRamp()` - Forces a re-calculation and redraw of all 41 shade/tint swatches.
5.  `SyncGradientEditor()` - Refreshes the color stop icons and position numbers in the gradient window.

### 6.3. The Master Dispatcher

The goal is to replace direct calls to multiple internal sync functions with a single **Master Dispatcher** pattern:

```c
// Proposed entry point for all UI color state changes
void DispatchColorUIUpdate(ULONG area_mask, int index) {
    if (area_mask & UPDATE_PALETTES) {
        RefreshAllColorButtons(); // Updates Pen, Fill, and User palettes
    }
    if (area_mask & UPDATE_SHADES) {
        UpdateFShades();
    }
    if (area_mask & UPDATE_GRADIENTS) {
        updategradcolors();
    }
}
```

---

## 7. Summary of All Required Changes

| # | File | Line(s) | Change | Severity | Status |
|---|------|---------|--------|----------|--------|
| 1 | `z_color.c` | New | Add `SetColorButtonRGB()` function with `MUI_Redraw` | Foundation | ✅ Done |
| 2 | `z_color.c` | New | Add `RefreshAllColorButtons()` function | Foundation | ✅ Done |
| 3 | `z_color.h` | Append | Add prototypes for above two functions | Foundation | ✅ Done |
| 4 | `z_color.c` | 1504–1519 | Rewrite `SetUsrColBut()` using `SetColorButtonRGB()` with NULL guards | High | ✅ Done |
| 5 | `z_color.c` | 706 | Fix operator precedence in `DoColorAdj` value calculation | Critical | ✅ Already correct |
| 6 | `z_color.c` | 859 | Fix operator precedence in `DoGradColorAdj` argb calculation | Critical | ✅ Already correct |
| 7 | `z_edit.c` | 1898–1910 | Rewrite `UpdateFShades()` to use `SetColorButtonRGB()` | Medium | ✅ Done |
| 8 | `z_edit.c` | 1727–1728 | Fix `DoFMode` case 1 to not blindly reset `cur_fillcolor = WHITE` when coming from a gradient fill | Medium | ✅ Done |
| 9 | `z_color.c` | 1482–1502 | Rewrite `updategradcolors()` to use `SetColorButtonRGB()` | Low | ✅ Done |
| 10 | `z_fig.c` | 384–405 | Rewrite `MakeColorButton()` to use `SetColorButtonRGB()` | Low | ✅ Done |
| 11 | `a_color.c` | 145–146 | Add out-of-bounds guard in `getfillcolor()` | Medium | ✅ Done |
| 12 | `f_load.c` | After load | Call `RefreshAllColorButtons()` after file load | High | ✅ Done |
| 13 | `z_color.c` | ~407 | Call `updategradcolors()` at end of `z_color_methods()` | Low | ✅ Done |
| 14 | `z_color.c` | DoColorAdj | Remove duplicate computation and dead checks | Low | ✅ Done |
| 15 | `z_color.c` | DoGradAddStop | New stops placed at midpoint | Medium | ✅ Done |
| 16 | `z_edit.c` | DoPenColorPal | Explicitly refresh selected pen color swatch on click | Low | ✅ Done |
| 17 | `z_edit.c` | DoFilColorPal | Explicitly refresh selected fill color swatch on click | Low | ✅ Done |
| 18 | `z_edit.c` | DoFMode | Add `RefreshAllFillButtons()` call in Case 1 (Plain) | Medium | ✅ Done |
| 19 | `mcc_arrowslider.c` | All | Implement `OM_SET` and `OM_GET` for numeric forwarding | Critical | ✅ Done |
| 20 | `z_color.c` | DoGradColorAdj | Trigger real-time gradient preview updates | High | ✅ Done |

---

## 8. Testing Checklist

After implementing the above changes, verify the following scenarios:

1. **Startup:** All 32 standard color swatches display correct colors in both Pen and Fill tabs.
2. **Open User Colors window:** All 512 user color cells display their colors (grey default).
3. **Edit a user color via color wheel:** The corresponding swatch in User Colors, Pen palette, AND Fill palette all update in real-time.
4. **Edit a user color via color picker:** Same as above.
5. **Edit a user color via hex string:** Same as above.
6. **Change fill mode to Plain:** Shade ramp updates to reflect the currently selected fill color (not always white).
7. **Switch fill color in Plain mode:** Shade ramp updates immediately.
8. **Load a .fig file with custom colors:** All palettes reflect the loaded colors after file open.
9. **Import an SVG with custom colors:** Same as above.
10. **Gradient editing:** Stop color swatches update when slider is moved. Preview reflects changes.
11. **Gradient creation:** New gradient stop swatches show correct initial colors.
12. **Copy/Swap colors in User Colors:** Both swapped cells update everywhere.
13. **Create gradient ramp in User Colors:** All intermediate cells update correctly.
