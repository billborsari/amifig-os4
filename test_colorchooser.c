/*
 *  test_colorchooser.c  —  v4: custom class for color swatches
 *
 *  Uses a custom MUI Area subclass that renders colors directly via
 *  RPTAG_APenColor (truecolor RectFill) instead of ColorfieldObject
 *  which requires pen allocation from the screen colormap.
 *
 *  Compile for AmigaOS 4:
 *      ppc-amigaos-gcc -gstabs -D__USE_INLINE__ -D__amigaos4__ -O2 -Wall \
 *          -Wno-parentheses -mcrt=newlib -o test_colorchooser test_colorchooser.c -lauto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <libraries/mui.h>

#ifdef __amigaos4__
#include <stdarg.h>
#include <proto/utility.h>
#endif

#ifndef __amigaos4__
#ifndef __AROS__
#include <proto/alib.h>
#endif
#endif

#ifdef __AROS__
#include <proto/alib.h>
#endif

#if defined(__amigaos4__) && !defined(IPTR)
typedef unsigned long IPTR;
#endif

#ifdef __amigaos4__
struct Library          *MUIMasterBase = NULL;
struct MUIMasterIFace   *IMUIMaster   = NULL;
#endif

#define MAKEID(a,b,c,d) ((ULONG)(a)<<24|(ULONG)(b)<<16|(ULONG)(c)<<8|(ULONG)(d))
#define EXPAND8(v)       ((ULONG)(v) * 0x01010101UL)

/* ═══════════════════════════════════════════════════════════════════════ */
/*  Custom Color Swatch Class                                             */
/*  Renders a colored rectangle directly via RPTAG_APenColor,             */
/*  bypassing MUI's pen allocation which is limited on 256-color screens. */
/* ═══════════════════════════════════════════════════════════════════════ */

/* Custom attribute tag IDs */
#define MUIA_ColorSwatch_RGB   0xFED40001UL

/* Instance data for our swatch class */
struct ColorSwatchData
{
    ULONG rgb;   /* 0x00RRGGBB format */
};

static struct MUI_CustomClass *CL_ColorSwatch = NULL;

/* --- OM_NEW --- */
static ULONG mColorSwatch_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ColorSwatchData *data;
    struct TagItem *tag;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj) return 0;

    data = INST_DATA(cl, obj);
    data->rgb = 0x00808080;  /* default grey */

    tag = FindTagItem(MUIA_ColorSwatch_RGB, msg->ops_AttrList);
    if (tag)
        data->rgb = (ULONG)(tag->ti_Data & 0x00FFFFFF);

    return (ULONG)obj;
}

/* --- OM_SET --- */
static ULONG mColorSwatch_Set(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);
    struct TagItem *tag;

    tag = FindTagItem(MUIA_ColorSwatch_RGB, msg->ops_AttrList);
    if (tag)
    {
        data->rgb = (ULONG)(tag->ti_Data & 0x00FFFFFF);
        MUI_Redraw(obj, MADF_DRAWUPDATE);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/* --- OM_GET --- */
static ULONG mColorSwatch_Get(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);

    if (msg->opg_AttrID == MUIA_ColorSwatch_RGB)
    {
        *(msg->opg_Storage) = data->rgb;
        return TRUE;
    }
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/* --- MUIM_AskMinMax --- */
static ULONG mColorSwatch_AskMinMax(struct IClass *cl, Object *obj,
                                     struct MUIP_AskMinMax *msg)
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth  += 8;
    msg->MinMaxInfo->MinHeight += 8;
    msg->MinMaxInfo->DefWidth  += 16;
    msg->MinMaxInfo->DefHeight += 16;
    msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;
    msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

    return 0;
}

/* --- MUIM_Draw --- */
static ULONG mColorSwatch_Draw(struct IClass *cl, Object *obj,
                                struct MUIP_Draw *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);
    struct RastPort *rp;
    LONG left, top, right, bottom;

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE)))
        return 0;

    rp     = _rp(obj);
    left   = _mleft(obj);
    top    = _mtop(obj);
    right  = _mright(obj);
    bottom = _mbottom(obj);

    /* Direct truecolor fill — no pen allocation needed */
#ifdef __amigaos4__
    SetRPAttrs(rp, RPTAG_APenColor, data->rgb, TAG_DONE);
#else
    /* AROS/MorphOS: use SetRPAttrs or fall back to closest pen */
    SetRPAttrs(rp, RPTAG_APenColor, data->rgb, TAG_DONE);
#endif
    RectFill(rp, left, top, right, bottom);

    return 0;
}

/* --- Dispatcher --- */
#ifdef __amigaos4__
static ULONG VARARGS68K mColorSwatch_Dispatcher(struct IClass *cl, Object *obj, Msg msg)
#else
AROS_UFH3(static ULONG, mColorSwatch_Dispatcher,
    AROS_UFHA(struct IClass *, cl, A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg, msg, A1))
{
    AROS_USERFUNC_INIT
#endif
#ifdef __amigaos4__
{
#endif
    switch (msg->MethodID)
    {
        case OM_NEW:         return mColorSwatch_New(cl, obj, (struct opSet *)msg);
        case OM_SET:         return mColorSwatch_Set(cl, obj, (struct opSet *)msg);
        case OM_GET:         return mColorSwatch_Get(cl, obj, (struct opGet *)msg);
        case MUIM_AskMinMax: return mColorSwatch_AskMinMax(cl, obj, (struct MUIP_AskMinMax *)msg);
        case MUIM_Draw:      return mColorSwatch_Draw(cl, obj, (struct MUIP_Draw *)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/* --- Create / Destroy the custom class --- */

static BOOL CreateColorSwatchClass(void)
{
    CL_ColorSwatch = MUI_CreateCustomClass(NULL, MUIC_Area, NULL,
                                            sizeof(struct ColorSwatchData),
                                            (APTR)mColorSwatch_Dispatcher);
    if (!CL_ColorSwatch) {
        printf("ERROR: Failed to create ColorSwatch custom class!\n");
        return FALSE;
    }
    printf("ColorSwatch class created OK: %p\n", CL_ColorSwatch);
    return TRUE;
}

static void DeleteColorSwatchClass(void)
{
    if (CL_ColorSwatch)
        MUI_DeleteCustomClass(CL_ColorSwatch);
}

/* Helper: create a swatch object */
static APTR MakeSwatchBtn(ULONG rgb, LONG w, LONG h)
{
    APTR obj;

    obj = NewObject(CL_ColorSwatch->mcc_Class, NULL,
        MUIA_Frame,            MUIV_Frame_ImageButton,
        MUIA_InputMode,        MUIV_InputMode_Toggle,
        MUIA_ColorSwatch_RGB,  (rgb & 0x00FFFFFF),
        MUIA_FixWidth,         w,
        MUIA_FixHeight,        h,
        MUIA_InnerLeft,        0,
        MUIA_InnerRight,       0,
        MUIA_InnerTop,         0,
        MUIA_InnerBottom,      0,
        TAG_DONE);

    if (!obj) printf("[MakeSwatchBtn] FAILED rgb=%08lX\n", (unsigned long)rgb);
    return obj;
}

/* Helper: update swatch color */
static void SetSwatchColor(APTR btn, ULONG rgb)
{
    if (!btn) return;
    set(btn, MUIA_ColorSwatch_RGB, (rgb & 0x00FFFFFF));
}

/* ═══════════════════════════════════════════════════════════════════════ */
/*  Application logic                                                     */
/* ═══════════════════════════════════════════════════════════════════════ */

#define NUM_STD_COLS    32
#define NUM_USR_COLS    16
#define CELLSIZE        16

static ULONG StdColRGB[NUM_STD_COLS] = {
    0x00000000, 0x000000FF, 0x0000FF00, 0x0000FFFF,
    0x00FF0000, 0x00FF00FF, 0x00FFFF00, 0x00FFFFFF,
    0x00000090, 0x000000B0, 0x000000D0, 0x0087CEFF,
    0x00009000, 0x0000B000, 0x0000D000, 0x00009090,
    0x0000B0B0, 0x0000D0D0, 0x00900000, 0x00B00000,
    0x00D00000, 0x00900090, 0x00B000B0, 0x00D000D0,
    0x00803000, 0x00A04000, 0x00C06000, 0x00FF8080,
    0x00FFA0A0, 0x00FFC0C0, 0x00FFE0E0, 0x00FFD700,
};

static ULONG UsrColRGB[NUM_USR_COLS] = {
    0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFF00,
    0xFFFF00FF, 0xFF00FFFF, 0xFFFF8000, 0xFF8000FF,
    0xFF804000, 0xFF008040, 0xFF400080, 0xFFFF4040,
    0xFF40FF40, 0xFF4040FF, 0xFF808080, 0xFFFFFFFF,
};

static LONG  CurSel = 0;
static BOOL  IgnoreAdj = FALSE;

static APTR App;
static APTR WinPalette, WinEditor;
static APTR BUT_SYS[NUM_STD_COLS];
static APTR BUT_USR_W1[NUM_USR_COLS];
static APTR BUT_USR_W2[NUM_USR_COLS];
static APTR CAD_COLOR, SLD_ALPHA, STR_ARGB, TXT_STATUS;

/* ─── Editor sync ──────────────────────────────────────────────────────── */

static void SyncEditorToSelection(void)
{
    ULONG rgb = UsrColRGB[CurSel];
    ULONG a   = (rgb & 0xFF000000) >> 24;
    char  buf[9];

    IgnoreAdj = TRUE;
    nnset(SLD_ALPHA, MUIA_Numeric_Value, a);
    nnset(CAD_COLOR, MUIA_Coloradjust_Red,   EXPAND8((rgb & 0x00FF0000) >> 16));
    nnset(CAD_COLOR, MUIA_Coloradjust_Green, EXPAND8((rgb & 0x0000FF00) >>  8));
    nnset(CAD_COLOR, MUIA_Coloradjust_Blue,  EXPAND8((rgb & 0x000000FF)      ));
    sprintf(buf, "%08lX", (unsigned long)rgb);
    nnset(STR_ARGB, MUIA_String_Contents, buf);
    IgnoreAdj = FALSE;
}

static void UpdateStatus(const char *action)
{
    static char status[128];
    sprintf(status, "Sel: %ld   ARGB: %08lX   [%s]",
            (long)CurSel, (unsigned long)UsrColRGB[CurSel], action);
    set(TXT_STATUS, MUIA_Text_Contents, status);
}

/* ─── Hooks ────────────────────────────────────────────────────────────── */

#ifdef __AROS__
AROS_UFH3(void, HookSwatchClicked,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(APTR, obj, A2),
    AROS_UFHA(IPTR *, arg, A1))
{
    AROS_USERFUNC_INIT
#else
void HookSwatchClicked(struct Hook *h, APTR obj, ULONG *arg)
{
#endif
    int i;

    for (i = 0; i < NUM_USR_COLS; i++)
        if (obj == BUT_USR_W1[i] || obj == BUT_USR_W2[i])
        {   CurSel = i; break; }

    for (i = 0; i < NUM_USR_COLS; i++)
    {
        if (i != CurSel) {
            DoMethod(BUT_USR_W1[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_USR_W2[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        } else {
            if (obj == BUT_USR_W1[i])
                DoMethod(BUT_USR_W2[i], MUIM_NoNotifySet, MUIA_Selected, TRUE);
            else
                DoMethod(BUT_USR_W1[i], MUIM_NoNotifySet, MUIA_Selected, TRUE);
        }
    }

    SyncEditorToSelection();
    UpdateStatus("selected");
    printf("[SWATCH] %ld sel  ARGB=%08lX\n", (long)CurSel, (unsigned long)UsrColRGB[CurSel]);

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __AROS__
AROS_UFH3(void, HookColorAdj,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(APTR, obj, A2),
    AROS_UFHA(IPTR *, arg, A1))
{
    AROS_USERFUNC_INIT
#else
void HookColorAdj(struct Hook *h, APTR obj, ULONG *arg)
{
#endif
    ULONG a = 0, r = 0, g = 0, b = 0, value;
    char  buf[9];

    if (IgnoreAdj) goto done;

    get(SLD_ALPHA,  MUIA_Numeric_Value,     &a);
    get(CAD_COLOR,  MUIA_Coloradjust_Red,   &r);
    get(CAD_COLOR,  MUIA_Coloradjust_Green, &g);
    get(CAD_COLOR,  MUIA_Coloradjust_Blue,  &b);

    value = ((a << 24) |
             ((r & 0xFF000000) >>  8) |
             ((g & 0xFF000000) >> 16) |
             ((b & 0xFF000000) >> 24));

    UsrColRGB[CurSel] = value;
    sprintf(buf, "%08lX", (unsigned long)value);
    nnset(STR_ARGB, MUIA_String_Contents, buf);

    SetSwatchColor(BUT_USR_W1[CurSel], value);
    SetSwatchColor(BUT_USR_W2[CurSel], value);
    UpdateStatus("color adjusted");

    printf("[COLORADJ] sel=%ld => %08lX\n", (long)CurSel, (unsigned long)value);

done: ;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

#ifdef __AROS__
AROS_UFH3(void, HookColorString,
    AROS_UFHA(struct Hook *, h, A0),
    AROS_UFHA(APTR, obj, A2),
    AROS_UFHA(IPTR *, arg, A1))
{
    AROS_USERFUNC_INIT
#else
void HookColorString(struct Hook *h, APTR obj, ULONG *arg)
{
#endif
    STRPTR str = (STRPTR)*arg;
    ULONG  argb = 0;

    sscanf((char *)str, "%08lX", &argb);
    UsrColRGB[CurSel] = argb;
    SetSwatchColor(BUT_USR_W1[CurSel], argb);
    SetSwatchColor(BUT_USR_W2[CurSel], argb);
    SyncEditorToSelection();
    UpdateStatus("string entered");

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/* ─── Main ─────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[])
{
    int  i;
    ULONG sigs;
    BOOL  running = TRUE;

    static struct Hook swatchHook;
    static struct Hook colorAdjHook;
    static struct Hook colorStrHook;

    APTR GRP_SYSCOL, GRP_USR1, GRP_USR2;

    printf("\n=== ColorChooser Test v4 — Custom Swatch Class ===\n");
    printf("Uses RPTAG_APenColor (truecolor) instead of ColorfieldObject pens.\n\n");

#ifdef __amigaos4__
    MUIMasterBase = OpenLibrary((CONST STRPTR)"muimaster.library", 0);
    if (!MUIMasterBase) { printf("ERROR: muimaster.library\n"); return 20; }
    IMUIMaster = (struct MUIMasterIFace *)GetInterface(MUIMasterBase, "main", 1, NULL);
    if (!IMUIMaster) { CloseLibrary(MUIMasterBase); return 20; }
#endif

    if (!CreateColorSwatchClass()) {
#ifdef __amigaos4__
        if (IMUIMaster) DropInterface((struct Interface *)IMUIMaster);
        if (MUIMasterBase) CloseLibrary(MUIMasterBase);
#endif
        return 20;
    }

    /* ─── Create swatch buttons ────────────────────────────────────── */

    printf("Creating %d system swatches...\n", NUM_STD_COLS);
    for (i = 0; i < NUM_STD_COLS; i++)
        BUT_SYS[i] = MakeSwatchBtn(StdColRGB[i], CELLSIZE, CELLSIZE);

    printf("Creating %d user swatches (W1)...\n", NUM_USR_COLS);
    for (i = 0; i < NUM_USR_COLS; i++)
        BUT_USR_W1[i] = MakeSwatchBtn(UsrColRGB[i], CELLSIZE, CELLSIZE);

    printf("Creating %d user swatches (W2)...\n", NUM_USR_COLS);
    for (i = 0; i < NUM_USR_COLS; i++)
        BUT_USR_W2[i] = MakeSwatchBtn(UsrColRGB[i], CELLSIZE, 28);

    /* ─── Layout ───────────────────────────────────────────────────── */

    GRP_SYSCOL = VGroup,
        MUIA_Group_Spacing, 0,
        Child, HGroup, MUIA_Group_Spacing, 0,
            Child, BUT_SYS[ 0], Child, BUT_SYS[ 1], Child, BUT_SYS[ 2], Child, BUT_SYS[ 3],
            Child, BUT_SYS[ 4], Child, BUT_SYS[ 5], Child, BUT_SYS[ 6], Child, BUT_SYS[ 7],
            Child, BUT_SYS[ 8], Child, BUT_SYS[ 9], Child, BUT_SYS[10], Child, BUT_SYS[11],
            Child, BUT_SYS[12], Child, BUT_SYS[13], Child, BUT_SYS[14], Child, BUT_SYS[15],
        End,
        Child, HGroup, MUIA_Group_Spacing, 0,
            Child, BUT_SYS[16], Child, BUT_SYS[17], Child, BUT_SYS[18], Child, BUT_SYS[19],
            Child, BUT_SYS[20], Child, BUT_SYS[21], Child, BUT_SYS[22], Child, BUT_SYS[23],
            Child, BUT_SYS[24], Child, BUT_SYS[25], Child, BUT_SYS[26], Child, BUT_SYS[27],
            Child, BUT_SYS[28], Child, BUT_SYS[29], Child, BUT_SYS[30], Child, BUT_SYS[31],
        End,
    End;

    GRP_USR1 = HGroup, MUIA_Group_Spacing, 0,
        Child, BUT_USR_W1[ 0], Child, BUT_USR_W1[ 1], Child, BUT_USR_W1[ 2], Child, BUT_USR_W1[ 3],
        Child, BUT_USR_W1[ 4], Child, BUT_USR_W1[ 5], Child, BUT_USR_W1[ 6], Child, BUT_USR_W1[ 7],
        Child, BUT_USR_W1[ 8], Child, BUT_USR_W1[ 9], Child, BUT_USR_W1[10], Child, BUT_USR_W1[11],
        Child, BUT_USR_W1[12], Child, BUT_USR_W1[13], Child, BUT_USR_W1[14], Child, BUT_USR_W1[15],
    End;

    GRP_USR2 = HGroup, MUIA_Group_Spacing, 0,
        Child, BUT_USR_W2[ 0], Child, BUT_USR_W2[ 1], Child, BUT_USR_W2[ 2], Child, BUT_USR_W2[ 3],
        Child, BUT_USR_W2[ 4], Child, BUT_USR_W2[ 5], Child, BUT_USR_W2[ 6], Child, BUT_USR_W2[ 7],
        Child, BUT_USR_W2[ 8], Child, BUT_USR_W2[ 9], Child, BUT_USR_W2[10], Child, BUT_USR_W2[11],
        Child, BUT_USR_W2[12], Child, BUT_USR_W2[13], Child, BUT_USR_W2[14], Child, BUT_USR_W2[15],
    End;

    SLD_ALPHA = SliderObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Numeric_Min, 0L, MUIA_Numeric_Max, 255L, MUIA_Numeric_Value, 255L,
    End;

    TXT_STATUS = TextObject,
        MUIA_Text_Contents, "Ready.",
        MUIA_Text_SetMax, FALSE,
    End;

    /* ─── Application ──────────────────────────────────────────────── */

    App = ApplicationObject,
        MUIA_Application_Title, "ColorChooser Test v4",
        MUIA_Application_Version, "$VER: ColorChooserTest 4.0 (03.04.2026)",
        MUIA_Application_Base, "COLTEST",

        SubWindow, WinPalette = WindowObject,
            MUIA_Window_Title, "Window 1 : Palette View",
            MUIA_Window_ID, MAKEID('C','P','A','L'),
            WindowContents, VGroup,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, "System Colors",
                    Child, GRP_SYSCOL,
                End,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, "User Colors (R,G,B,Y,M,C...)",
                    Child, GRP_USR1,
                End,
                Child, TXT_STATUS,
            End,
        End,

        SubWindow, WinEditor = WindowObject,
            MUIA_Window_Title, "Window 2 : Color Editor",
            MUIA_Window_ID, MAKEID('C','E','D','T'),
            WindowContents, VGroup,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, "User Colors",
                    Child, GRP_USR2,
                End,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, "Color Adjust",
                    MUIA_Weight, 1000,
                    Child, HGroup,
                        Child, TextObject, MUIA_Text_Contents, "ALPHA:", MUIA_Text_SetMax, TRUE, End,
                        Child, SLD_ALPHA,
                    End,
                    Child, CAD_COLOR = ColoradjustObject, MUIA_Weight, 1000, End,
                    Child, HGroup,
                        Child, TextObject, MUIA_Text_Contents, "ARGB:", MUIA_Text_SetMax, TRUE, End,
                        Child, STR_ARGB = StringObject,
                            StringFrame,
                            MUIA_String_Contents, "FFFF0000",
                            MUIA_String_MaxLen, 9,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789ABCDEFabcdef",
                        End,
                    End,
                End,
            End,
        End,
    End;

    if (!App) {
        printf("ERROR: MUI App creation failed.\n");
        DeleteColorSwatchClass();
#ifdef __amigaos4__
        if (IMUIMaster) DropInterface((struct Interface *)IMUIMaster);
        if (MUIMasterBase) CloseLibrary(MUIMasterBase);
#endif
        return 20;
    }

    /* ─── Hooks & Notifications ────────────────────────────────────── */

    swatchHook.h_Entry   = (HOOKFUNC)HookSwatchClicked;
    colorAdjHook.h_Entry = (HOOKFUNC)HookColorAdj;
    colorStrHook.h_Entry = (HOOKFUNC)HookColorString;

    DoMethod(WinPalette, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    DoMethod(WinEditor, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    for (i = 0; i < NUM_USR_COLS; i++)
    {
        DoMethod(BUT_USR_W1[i], MUIM_Notify, MUIA_Selected, TRUE,
                 MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&swatchHook, 0);
        DoMethod(BUT_USR_W1[i], MUIM_Notify, MUIA_Selected, FALSE,
                 MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);

        DoMethod(BUT_USR_W2[i], MUIM_Notify, MUIA_Selected, TRUE,
                 MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&swatchHook, 0);
        DoMethod(BUT_USR_W2[i], MUIM_Notify, MUIA_Selected, FALSE,
                 MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }

    DoMethod(CAD_COLOR, MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,
             App, 3, MUIM_CallHook, (IPTR)&colorAdjHook, 0);
    DoMethod(SLD_ALPHA, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
             App, 3, MUIM_CallHook, (IPTR)&colorAdjHook, 0);
    DoMethod(STR_ARGB, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&colorStrHook, MUIV_TriggerValue);

    set(BUT_USR_W1[0], MUIA_Selected, TRUE);

    /* ─── Open & Run ───────────────────────────────────────────────── */

    set(WinPalette, MUIA_Window_Open, TRUE);
    set(WinEditor,  MUIA_Window_Open, TRUE);

    SyncEditorToSelection();
    UpdateStatus("ready");

    printf("\nRunning. All swatches should display solid colors.\n");
    printf("Close either window to quit.\n\n");

    while (running)
    {
        ULONG id = DoMethod(App, MUIM_Application_NewInput, &sigs);
        if (id == MUIV_Application_ReturnID_Quit) running = FALSE;
        if (running && sigs) {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if (sigs & SIGBREAKF_CTRL_C) running = FALSE;
        }
    }

    /* ─── Cleanup ──────────────────────────────────────────────────── */

    set(WinPalette, MUIA_Window_Open, FALSE);
    set(WinEditor,  MUIA_Window_Open, FALSE);
    MUI_DisposeObject(App);
    DeleteColorSwatchClass();

#ifdef __amigaos4__
    if (IMUIMaster)   DropInterface((struct Interface *)IMUIMaster);
    if (MUIMasterBase) CloseLibrary(MUIMasterBase);
#endif

    printf("\n=== Done ===\n");
    return 0;
}
