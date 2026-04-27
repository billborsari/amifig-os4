# AmiFIG ARexx Guide

AmiFIG features a robust ARexx interface that allows you to automate drawing tasks, control UI settings, and integrate with other AmigaOS applications.

## Quick Start

### The ARexx Port
By default, AmiFIG opens a public ARexx port named `AMIFIG.0`. If multiple instances are running, they will be named sequentially (`AMIFIG.1`, `AMIFIG.2`, etc.).

### Your First Script
Create a file named `test.rexx` and add the following:

```rexx
/* Simple AmiFIG Automation */
ADDRESS AMIFIG.0

'Clear'
'SetBaseSystem metric'
'SetPenColor 1'      /* Blue */
'DrawCircle 5 5 2'   /* Draw circular blue outline at 5cm, 5cm with 2cm radius */
'SetFillColor 4'     /* Red */
'SetFillShade 20'    /* Solid fill */
'DrawRectBox 10 2 15 7'
```

Run this script using `rx test.rexx` from the shell while AmiFIG is open.

---

## Coordinate Systems
AmiFIG supports three coordinate systems for ARexx commands. You can switch between them using the `SetBaseSystem` command.

| System | Mode | Description |
| :--- | :--- | :--- |
| **dpi** | `dpi` | Raw pixels at 1200 DPI internal resolution (Standard Fig format). |
| **metric** | `metric` | Coordinates are specified in centimeters (cm). |
| **imperial** | `imperial` | Coordinates are specified in inches (in). |

*Tip: Metric mode is usually the most intuitive for scripted drawing.*

---

## Command Reference

### Object Creation
| Command | Arguments | Description |
| :--- | :--- | :--- |
| **DrawCircle** | `X, Y, RADIUS` | Draws a circle. |
| **DrawEllipse** | `X, Y, R1, R2` | Draws an ellipse with two radii. |
| **DrawRectBox** | `X1, Y1, X2, Y2` | Draws a rectangular box. |
| **DrawText** | `X, Y, TEXT` | Places text at the location. |
| **DrawArc** | `X1, Y1, X2, Y2, X3, Y3` | Draws an open arc through 3 points. |
| **DrawPicture** | `X1, Y1, X2, Y2, FILE` | Places an external image. |

### Complex Shapes (Polylines/Splines)
Polylines and splines are created in a sequence:
1. `StartPolyline` (or `StartPolygon`, `StartSpline`, etc.) 
2. `AddLineSegment` (repeated as necessary)
3. `EndLine` (to finish the shape)

**Example:**
```rexx
'StartPolygon 0 0'
'AddLineSegment 5 0'
'AddLineSegment 5 5'
'EndLine 0 5'
```

### Global Settings & Attributes
| Command | Arguments | Description |
| :--- | :--- | :--- |
| **SetPenColor** | `COLOR_ID` | Standard indices (0:Black, 1:Blue, 2:Green, 3:Cyan, 4:Red, etc.) |
| **SetFillColor** | `COLOR_ID` | Sets color used for object fills. |
| **SetFillShade** | `SHADE` | 0=Black, 20=Solid, 40=White, 41-100=Patterns. |
| **SetLineWidth** | `WIDTH` | Line thickness (0-99). |
| **SetDepth** | `DEPTH` | Drawing depth (0:Front - 999:Back). |
| **SetDisplayFont** | `SIZE` | Sets the font size for text tools. |

---

## Comprehensive Scripting Examples

### 1. Generating a Simple Grid
This script demonstrates the use of loops to generate a coordinate-accurate grid using the metric system.

```rexx
/* Generate a 10x10 cm grid */
ADDRESS AMIFIG.0
'SetBaseSystem metric'
'SetLineWidth 1'
'SetPenColor 0' /* Black */

DO x = 0 TO 10
    'StartPolyline ' x ' 0'
    'EndLine ' x ' 10'
END

DO y = 0 TO 10
    'StartPolyline 0 ' y
    'EndLine 10 ' y
END
```

### 2. Creating a Styled "Flowchart" Box
Demonstrates setting multiple attributes before drawing a complex object.

```rexx
/* Draw a stylized flowchart process box */
ADDRESS AMIFIG.0
'SetBaseSystem metric'

/* Set attributes for the box */
'SetLineWidth 3'
'SetPenColor 1'      /* Blue border */
'SetFillColor 31'     /* Gold/Yellow fill */
'SetFillShade 20'    /* Solid fill */

/* Draw the box */
'DrawRectBox 2 2 8 5'

/* Add the text label */
'SetPenColor 0'      /* Black text */
'SetDisplayFont 18'
'SetTextAdjustment 1' /* Centered */
'DrawText 5 3.8 "PROCESS STEP"'
```

### 3. Automated Batch Export
If you have a large drawing and need multiple versions of it:

```rexx
/* Export the current work in multiple formats */
ADDRESS AMIFIG.0

/* Export as PNG for web */
'ExportFile "PROGDIR:Exports/diagram.png" png 0 0 1920 1080'

/* Export as FIG for backup/translation */
'SaveFile "PROGDIR:Exports/diagram_backup.fig"'

/* Trigger the internal high-quality fig2dev GUI */
'ExportFile "" fig2dev 0 0 0 0'
```

---

## Tips & Tricks

### Coordinate Precision
When using `metric` or `imperial` systems, AmiFIG handles the translation to internal DPI automatically. You can use floating point numbers (e.g., `DrawCircle 5.25 3.1 0.75`) for high precision.

### Captured Results
Some internal commands return information. You can access these using the ARexx `RESULT` variable after enabling results.
```rexx
OPTIONS RESULTS
ADDRESS AMIFIG.0
'testfunc'
IF RC = 0 THEN SAY "Success: " RESULT
ELSE SAY "Error code: " RC
```

### Scripting Complex Paths
Always remember the **Start/Add/End** pattern for lines and splines. If your script crashes or is aborted mid-sequence, you may need to send a `Clear` command or manually finish the object in the UI to reset the drawing state.

