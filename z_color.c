/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_color.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/*----------------------------------------------------------------------------*/
/*     Includes & Defines                                                     */
/*----------------------------------------------------------------------------*/

#include "fig.h"
#include <libraries/mui.h>
#include <proto/muimaster.h>

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "a_color.h"
#include "z_fig.h"
#include "z_color.h"
#include "mcc_graddisp.h"
#include "mcc_arrowslider.h"
#include "mcc_imagedisp.h"
#include "mcc_colorswatch.h"
#include "w_msgpanel.h"

#define COLORCELLSIZE 12
#define GetA(x) ((x & 0xFF000000) >> 24)
#define GetR(x) ((x & 0x00FF0000) >> 16)
#define GetG(x) ((x & 0x0000FF00) >> 8)
#define GetB(x)  (x & 0x000000FF)


/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFP3(void, DoColorMutex,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoColorAdj,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoColorButtons,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradAdd,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradAdj,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradColorMutex,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradColorAdj,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradAddStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradSwapStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradRemStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradType,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradChg,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoColorPicker,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoColorString,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoColorTabChange,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoColorMutex(struct Hook *h, APTR Object, ULONG *Arg);
void DoColorAdj(struct Hook *h, APTR Object, ULONG *Arg);
void DoColorButtons(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradStop(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradAdd(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradAdj(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradColorMutex(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradColorAdj(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradAddStop(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradSwapStop(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradRemStop(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradType(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradChg(struct Hook *h, APTR Object, ULONG *Arg);
void DoColorPicker(struct Hook *h, APTR Object, ULONG *Arg);
void DoColorString(struct Hook *h, APTR Object, ULONG *Arg);
void DoColorTabChange(struct Hook *h, APTR Object, ULONG *Arg);
#endif    
    
    
APTR MakeGradGroup(void);
void updategradcolors(void);


/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR	BUT_USRCOL[MAX_USR_COLS], LINE_USRPAL[MAX_USR_COLS/16], VG_USRPAL;
APTR	CAD_PENCOL, BUT_GRADIENT, BUT_COPY, BUT_SWAP, NUM_USRALPHA;
APTR    BUT_COL_CP, STR_COL_ARGB;

APTR	VG_GRADEDIT, VG_GRADEDITNULL, NUM_GRADNUM, BUT_ADDGRAD, BUT_COPYGRAD, BUT_DELGRAD, CHK_FIXFOCUS, GRP_RADIAL;
APTR	CYC_GRADTYPE, CYC_GRADSPREAD, CYC_GRADUNIT, NUM_GRADX1, NUM_GRADY1, NUM_GRADX2, NUM_GRADY2, NUM_GRADRADIUS;
APTR    GRP_GRADSTOP[16];
APTR	BUT_GRADCOLOR[16], NUM_GRADSTOP[16], BUT_GRADSTOPADD, BUT_GRADSTOPREM, BUT_GRADSTOPSWAP;
APTR    CAD_GRADCOL, NUM_GRADALPHA, GRD_PREV, TXT_P1, TXT_P2;
APTR    BUT_GRAD_CP, STR_GRAD_ARGB;

LONG	CurrentSelectedUsrCol = 0;
BOOL    IgnoreColorAdj = FALSE;
BOOL    IgnoreGradColorAdj = FALSE;
LONG	FirstGradientColor = -1;
LONG    FirstSwapColor = -1;
LONG    FirstCopyColor = -1;
LONG    FirstSwapStop = -1;
LONG    CurrentSelectedGradStop = 0;
LONG    CurrentSelectedGrad = 0;
LONG    ActiveColorTab = 0;
APTR    RG_COLORTABS;

extern APTR GRD_PREVIEW;
        
/*----------------------------------------------------------------------------*/
/*     MakeColorWindow - creates the color window                             */
/*----------------------------------------------------------------------------*/

APTR MakeColorWindow(void)
{
    static char *colorregtitles[] = {NULL, NULL, NULL };
	int i;
	
    colorregtitles[0] = (char *)_(msg_UserColors);
    colorregtitles[1] = (char *)_(msg_Gradient);
    
	/* Create color buttons */
    for (i=0; i < MAX_USR_COLS; i++)
    {
        BUT_USRCOL[i] = MakeColorButton(ColorPalette[i+NUM_STD_COLS].RGB, COLORCELLSIZE, 32);
    }
    
    for (i=0; i < (MAX_USR_COLS/16); i++)
    {
        LINE_USRPAL[i] = HGroup,
            MUIA_Group_Spacing, 0,
            Child, BUT_USRCOL[i * 16 +  0],
            Child, BUT_USRCOL[i * 16 +  1],
            Child, BUT_USRCOL[i * 16 +  2],
            Child, BUT_USRCOL[i * 16 +  3],
            Child, BUT_USRCOL[i * 16 +  4],
            Child, BUT_USRCOL[i * 16 +  5],
            Child, BUT_USRCOL[i * 16 +  6],
            Child, BUT_USRCOL[i * 16 +  7],
            Child, BUT_USRCOL[i * 16 +  8],
            Child, BUT_USRCOL[i * 16 +  9],
            Child, BUT_USRCOL[i * 16 + 10],
            Child, BUT_USRCOL[i * 16 + 11],
            Child, BUT_USRCOL[i * 16 + 12],
            Child, BUT_USRCOL[i * 16 + 13],
            Child, BUT_USRCOL[i * 16 + 14],
            Child, BUT_USRCOL[i * 16 + 15],
        End;
	}
    VG_USRPAL = VirtgroupObject,
        MUIA_Group_Spacing, 0,
        MUIA_Group_Horiz, FALSE,
    End;

    for (i = 0; i < (MAX_USR_COLS/16); i++) {
        DoMethod(VG_USRPAL, OM_ADDMEMBER, LINE_USRPAL[i]);
    }


	NUM_GRADNUM  = NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Weight       , 400,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 511L,
		MUIA_Numeric_Value, 0L,
		TAG_DONE);
    
    BUT_ADDGRAD  = MakeButton((UBYTE *)_(msg_New), '\0', (UBYTE *)_(msg_CreateNewGradient));
    BUT_COPYGRAD = MakeButton((UBYTE *)_(msg_Copy),   '\0', (UBYTE *)_(msg_CopyNewGradient));
    BUT_DELGRAD  = MakeButton((UBYTE *)_(msg_Delete), '\0', (UBYTE *)_(msg_DeleteGradient));

	NUM_USRALPHA = SliderObject,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 255L,
		MUIA_Numeric_Value, 0L,
	End;

    return(WindowObject,
            MUIA_Window_Title, "AmiFig : User Colors",
            MUIA_Window_ID, MAKEID('Z','F','C','O'),
			MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",
            MUIA_Window_Width, 800,
            MUIA_Window_Height, 700,

            WindowContents, RG_COLORTABS = RegisterGroup(colorregtitles),
				Child, HGroup,
                    Child, VGroup,
						MUIA_Frame, MUIV_Frame_Group,
                        MUIA_Weight, 100,
                        Child, ScrollgroupObject,
                            MUIA_FrameTitle, (UBYTE *)_(msg_UserColors),
                            MUIA_Scrollgroup_FreeHoriz, FALSE,
                            MUIA_Scrollgroup_Contents, VG_USRPAL,
                            MUIA_Weight, 1000,
                        End,
                        Child, HGroup,
                            MUIA_Weight, 0,
                            Child, BUT_COPY     = MakeButton((UBYTE *)_(msg_Copy), '\0', (UBYTE *)_(msg_CopyColor)),
                            Child, RectangleObject, End,
                            Child, BUT_SWAP     = MakeButton((UBYTE *)_(msg_Swap), '\0', (UBYTE *)_(msg_SwapColor)),
                            Child, RectangleObject, End,
                            Child, BUT_GRADIENT = MakeButton((UBYTE *)_(msg_Gradient), '\0', (UBYTE *)_(msg_CreateGradient)),
                        End,
                    End,
 					Child, VGroup,
						MUIA_Frame, MUIV_Frame_Group,
						MUIA_Weight, 500,
						Child, HGroup,
							Child, TextObject, MUIA_Text_Contents, "ALPHA:", MUIA_Text_SetMax, TRUE, End,
							Child, NUM_USRALPHA,
						End,
 						Child, CAD_PENCOL = ColoradjustObject, 
							MUIA_MinHeight, 350,
							MUIA_MinWidth, 350,
							MUIA_Weight, 1000,
						End,
                        Child, HGroup,
                            Child, BUT_COL_CP = MakeImageToggle((UBYTE *)"color_picker.png", '\0' , (UBYTE *)_(msg_ColorPicker)),
                            Child, RectangleObject, End,
                            Child, TextObject, MUIA_Text_Contents, "ARGB:", MUIA_Text_SetMax, TRUE, End,
                            Child, STR_COL_ARGB = StringObject,
                                StringFrame,
                                MUIA_String_Contents, "FFFFFFFF",
                                MUIA_String_MaxLen , 9,
                                MUIA_String_Format, MUIV_String_Format_Right,
                                MUIA_String_Accept, "0123456789ABCDEFabcdef",
                            End, 
                        End,
					End,
				End,
				Child, VGroup,
					Child, HGroup,
						MUIA_Frame, MUIV_Frame_Group,
						Child, NUM_GRADNUM,
						Child, BUT_ADDGRAD,
						Child, BUT_COPYGRAD,
                        Child, BUT_DELGRAD,
					End,
					Child, VG_GRADEDIT = MakeGradGroup(),
					Child, VG_GRADEDITNULL = RectangleObject, End,
				End,
            End,
        End);
}

void z_color_methods(void)
{
	int i;
	
    static struct Hook DoColorMutexHook;
    static struct Hook DoColorAdjHook;
    static struct Hook DoColorButtonsHook;
    static struct Hook DoGradColorMutexHook;
    static struct Hook DoGradColorAdjHook;
    static struct Hook DoGradStopHook;
    static struct Hook DoGradAdjHook;
    static struct Hook DoGradAddHook;
    static struct Hook DoGradRemStopHook;
    static struct Hook DoGradAddStopHook;
    static struct Hook DoGradSwapStopHook;
    static struct Hook DoGradTypeHook;
    static struct Hook DoGradChgHook;
    static struct Hook DoColorPickerHook;
    static struct Hook DoColorStringHook;
    DoColorMutexHook.h_Entry = (HOOKFUNC)DoColorMutex;
    DoColorAdjHook.h_Entry = (HOOKFUNC)DoColorAdj;
    DoColorButtonsHook.h_Entry = (HOOKFUNC)DoColorButtons;
    DoGradColorMutexHook.h_Entry = (HOOKFUNC)DoGradColorMutex;
    DoGradColorAdjHook.h_Entry = (HOOKFUNC)DoGradColorAdj;
    DoGradStopHook.h_Entry = (HOOKFUNC)DoGradStop;
    DoGradAdjHook.h_Entry = (HOOKFUNC)DoGradAdj;
    DoGradAddHook.h_Entry = (HOOKFUNC)DoGradAdd;
    DoGradRemStopHook.h_Entry = (HOOKFUNC)DoGradRemStop;
    DoGradAddStopHook.h_Entry = (HOOKFUNC)DoGradAddStop;
    DoGradSwapStopHook.h_Entry = (HOOKFUNC)DoGradSwapStop;
    DoGradTypeHook.h_Entry = (HOOKFUNC)DoGradType;
    DoGradChgHook.h_Entry = (HOOKFUNC)DoGradChg;
    DoColorPickerHook.h_Entry = (HOOKFUNC)DoColorPicker;
    DoColorStringHook.h_Entry = (HOOKFUNC)DoColorString;
    
    /* Close window */
    DoMethod(WinColor, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        WinColor, 3, MUIM_Set, MUIA_Window_Open, FALSE);
		
    /* User color panel methods */
    DoMethod(CAD_PENCOL, MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,  App, 3, MUIM_CallHook, (IPTR)&DoColorAdjHook, 0);
    DoMethod(NUM_USRALPHA, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,  App, 3, MUIM_CallHook, (IPTR)&DoColorAdjHook, 0);
    for (i=0; i < MAX_USR_COLS; i++)
    {
        DoMethod(BUT_USRCOL[i], MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorMutexHook, 0);
        DoMethod(BUT_USRCOL[i], MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }
    set(BUT_USRCOL[CurrentSelectedUsrCol], MUIA_Selected, TRUE);
    DoMethod(BUT_GRADIENT, MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorButtonsHook, 0);
    DoMethod(BUT_COPY,     MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorButtonsHook, 1);
    DoMethod(BUT_SWAP,     MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorButtonsHook, 2);
    
    /* Gradient panel methods */
    DoMethod(CYC_GRADTYPE,    MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradTypeHook, 0);
    DoMethod(CYC_GRADSPREAD,  MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 0);
    DoMethod(CYC_GRADUNIT,    MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 1);
    DoMethod(NUM_GRADX1,      MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 2);
    DoMethod(NUM_GRADY1,      MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 3);
    DoMethod(NUM_GRADX2,      MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 4);
    DoMethod(NUM_GRADY2,      MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 5);
    DoMethod(NUM_GRADRADIUS,  MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAdjHook, 6);
    DoMethod(BUT_GRADSTOPREM, MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradRemStopHook, 0);
    DoMethod(BUT_GRADSTOPADD, MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAddStopHook, 0);
    DoMethod(BUT_GRADSTOPSWAP,MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradSwapStopHook, 0);
    DoMethod(BUT_ADDGRAD,     MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAddHook, 0);
    DoMethod(BUT_COPYGRAD,    MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAddHook, 1);
    DoMethod(BUT_DELGRAD,     MUIM_Notify, MUIA_Selected     , FALSE         , MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradAddHook, 2);
    for (i=0; i < 16; i++)
    {
        DoMethod(BUT_GRADCOLOR[i], MUIM_Notify, MUIA_Selected,      TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradColorMutexHook, 0);
        DoMethod(BUT_GRADCOLOR[i], MUIM_Notify, MUIA_Selected,      FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
        DoMethod(NUM_GRADSTOP[i],  MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradStopHook, 0);
    }
    set(BUT_GRADCOLOR[0], MUIA_Selected, TRUE);
    DoMethod(CAD_GRADCOL, MUIM_Notify, MUIA_Coloradjust_RGB, MUIV_EveryTime,  App, 3, MUIM_CallHook, (IPTR)&DoGradColorAdjHook, 0);
    DoMethod(NUM_GRADALPHA, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,  App, 3, MUIM_CallHook, (IPTR)&DoGradColorAdjHook, 0);    
    DoMethod(NUM_GRADNUM, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoGradChgHook, 0);
    set(NUM_GRADNUM, MUIA_Numeric_Value, 0);
    if (GradientTable[CurrentSelectedGrad] == NULL)
    {
        set(VG_GRADEDIT, MUIA_ShowMe, FALSE);
        set(VG_GRADEDITNULL, MUIA_ShowMe, TRUE);
    }

    DoMethod(BUT_COL_CP, MUIM_Notify, MUIA_Selected, TRUE , MUIV_Notify_Self, 3, MUIM_CallHook   , (IPTR)&DoColorPickerHook, 0);
    DoMethod(BUT_COL_CP, MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_GRAD_CP, MUIM_Notify, MUIA_Selected, TRUE , MUIV_Notify_Self, 3, MUIM_CallHook   , (IPTR)&DoColorPickerHook, 0);
    DoMethod(BUT_GRAD_CP, MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(STR_COL_ARGB,  MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorStringHook, MUIV_TriggerValue);
    DoMethod(STR_GRAD_ARGB, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoColorStringHook, MUIV_TriggerValue);
    if (GradientTable[0] != NULL) {
        updategradcolors();
    }
}

/*----------------------------------------------------------------------------*/
/*     MakeGradGroup                                                          */
/*----------------------------------------------------------------------------*/
APTR MakeGradGroup(void)
{
	int i;
    static char *CYC_GRADTYPE_strings[] = {NULL, NULL, NULL };
    static char *CYC_GRADSPREAD_strings[] = {NULL, NULL, NULL, NULL };
    static char *CYC_GRADUNIT_strings[] = { NULL, NULL, NULL };
	APTR LocGroup;

    CYC_GRADTYPE_strings[0] = (char *)_(msg_Linear);
    CYC_GRADTYPE_strings[1] = (char *)_(msg_Radial);
    CYC_GRADSPREAD_strings[0] = (char *)_(msg_Pad);
    CYC_GRADSPREAD_strings[1] = (char *)_(msg_Repeat);
    CYC_GRADSPREAD_strings[2] = (char *)_(msg_Symetric);
    CYC_GRADUNIT_strings[0] = (char *)_(msg_Object);
    CYC_GRADUNIT_strings[1] = (char *)_(msg_Figure);

    
	CYC_GRADTYPE = CycleObject,
		MUIA_Cycle_Entries, CYC_GRADTYPE_strings,
		MUIA_Cycle_Active, 0L,
	End;

	CYC_GRADSPREAD = CycleObject,
		MUIA_Cycle_Entries, CYC_GRADSPREAD_strings,
		MUIA_Cycle_Active, 0L,
	End;

	CYC_GRADUNIT = CycleObject,
		MUIA_Cycle_Entries, CYC_GRADUNIT_strings,
		MUIA_Cycle_Active, 0L,
	End;

	NUM_GRADX1 = SliderObject,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, 50L,
	End;

	NUM_GRADY1 = SliderObject,
		MUIA_Slider_Horiz, FALSE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, 50L,
	End;

	NUM_GRADX2 = SliderObject,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, 50L,
	End;

	NUM_GRADY2 = SliderObject,
		MUIA_Slider_Horiz, FALSE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, 50L,
	End;

	NUM_GRADRADIUS = SliderObject,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, 50L,
	End;

	NUM_GRADALPHA = SliderObject,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 255L,
		MUIA_Numeric_Value, 255L,
	End;
    
    CHK_FIXFOCUS = MakeCheckmark((UBYTE *)_(msg_FixedFocalPt), 0L);

	BUT_GRADSTOPADD = MakeButton((UBYTE *)_(msg_Add), '\0', (UBYTE *)_(msg_AddOneColorStop));
	BUT_GRADSTOPREM = MakeButton((UBYTE *)_(msg_Del), '\0', (UBYTE *)_(msg_RemoveLastColorStop));
	BUT_GRADSTOPSWAP = MakeButton((UBYTE *)_(msg_Swap), '\0', (UBYTE *)_(msg_SwapColorStop));
	
	for (i=0; i<16; i++)
	{
		BUT_GRADCOLOR[i] = MakeColorButton(0x000F0F0F*i, COLORCELLSIZE, COLORCELLSIZE);
		NUM_GRADSTOP[i]  = NumericbuttonObject,
			MUIA_Group_Horiz  , FALSE,
			MUIA_Numeric_Min  , 0L,
			MUIA_Numeric_Max  , 100L,
			MUIA_Numeric_Value, (LONG)(100*i/16),
		End;
		GRP_GRADSTOP[i] = VGroup,
			Child, BUT_GRADCOLOR[i],
			Child, NUM_GRADSTOP[i],
		End;
	}
	
	LocGroup = VGroup,
        Child, HGroup,
            MUIA_Frame, MUIV_Frame_Group,
			Child, CYC_GRADTYPE,
			Child, CYC_GRADSPREAD,
			Child, CYC_GRADUNIT,
		End,
		Child, VGroup,
            Child, HGroup,
                MUIA_Frame, MUIV_Frame_Group,
                Child, GRP_GRADSTOP[ 0],
                Child, GRP_GRADSTOP[ 1],
                Child, GRP_GRADSTOP[ 2],
                Child, GRP_GRADSTOP[ 3],
                Child, GRP_GRADSTOP[ 4],
                Child, GRP_GRADSTOP[ 5],
                Child, GRP_GRADSTOP[ 6],
                Child, GRP_GRADSTOP[ 7],
                Child, GRP_GRADSTOP[ 8],
                Child, GRP_GRADSTOP[ 9],
                Child, GRP_GRADSTOP[10],
                Child, GRP_GRADSTOP[11],
                Child, GRP_GRADSTOP[12],
                Child, GRP_GRADSTOP[13],
                Child, GRP_GRADSTOP[14],
                Child, GRP_GRADSTOP[15],
                Child, RectangleObject, MUIA_Weight, 1000, End,
            End,
            Child, HGroup,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    Child, HGroup,
                        Child, RectangleObject, MUIA_Weight, 25, End,
                        Child, BUT_GRADSTOPADD,
                        Child, BUT_GRADSTOPREM,			
                        Child, BUT_GRADSTOPSWAP,
                        Child, RectangleObject, MUIA_Weight, 25, End,
                    End,
                    Child, GroupObject,
                        MUIA_Group_Columns, 3,
                        MUIA_Group_Spacing, 5,
                        Child, TXT_P1 = TextObject,  MUIA_Text_Contents, "\33cP1", End,
                        Child, RectangleObject, End,
                        Child, NUM_GRADX1,
                        Child, RectangleObject, End,
                        Child, TXT_P2 = TextObject,  MUIA_Text_Contents, "\33cP2", End,
                        Child, NUM_GRADX2,
                        Child, NUM_GRADY1,
                        Child, NUM_GRADY2,
                        Child, GRD_PREV = NewObject(mcc_grad->mcc_Class, NULL,
                                                    MUIA_Background, MUII_BACKGROUND,
                                                    TAG_DONE),
                    End,
                    Child, RectangleObject, MUIA_Weight, 1, End,
                    Child, GRP_RADIAL = GroupObject,
                        MUIA_Group_Columns, 2,
                        MUIA_Group_Spacing, 5,
                        Child, RectangleObject, End,
                        Child, CHK_FIXFOCUS,
                        Child, TextObject,  MUIA_Text_Contents, (char *)_(msg_Radius), End,
                        Child, NUM_GRADRADIUS,
                    End,
                End,
                Child, VGroup,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_Weight, 500,
                    Child, HGroup,
                        Child, TextObject, MUIA_Text_Contents, "ALPHA:", MUIA_Text_SetMax, TRUE, End,
                        Child, NUM_GRADALPHA,
                    End,
                     Child, CAD_GRADCOL = ColoradjustObject, 
							MUIA_MinHeight, 350,
							MUIA_MinWidth, 350,
							MUIA_Weight, 1000,
						End,
                    Child, HGroup,
                        Child, BUT_GRAD_CP = MakeImageToggle((UBYTE *)"color_picker.png", '\0' , (UBYTE *)_(msg_ColorPicker)),
                        Child, RectangleObject, End,
                        Child, TextObject, MUIA_Text_Contents, "ARGB:", MUIA_Text_SetMax, TRUE, End,
						Child, STR_GRAD_ARGB = StringObject,
                            StringFrame,
                            MUIA_String_Contents, "FFFFFFFF",
                            MUIA_String_MaxLen , 9,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789ABCDEFabcdef",
                        End, 
                    End,
                End,
            End,
		End,
	End;

	return(LocGroup);			
}

/*----------------------------------------------------------------------------*/
/*     DoColorMutex                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorMutex,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorMutex(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    char argb_str[9];
    
    for (i=0; i < MAX_USR_COLS; i++)
    {
        if (Object != BUT_USRCOL[i])
            DoMethod(BUT_USRCOL[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
        {
			CurrentSelectedUsrCol = i;
            /* Synchronize with Pen and Fill palettes in main window */
            cur_pencolor = i + NUM_STD_COLS;
            cur_fillcolor = i + NUM_STD_COLS;
            DispatchColorUIUpdate(UPDATE_PALETTES | UPDATE_SHADES, cur_pencolor);
        }
    }
    
    /* Highlight the selected color in all palettes */
    {
        extern APTR BUT_PENCOL[], BUT_FILCOL[];
        for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++) {
            if (i != (CurrentSelectedUsrCol + NUM_STD_COLS)) {
                DoMethod(BUT_PENCOL[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
                DoMethod(BUT_FILCOL[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
            } else {
                DoMethod(BUT_PENCOL[i], MUIM_NoNotifySet, MUIA_Selected, TRUE);
                DoMethod(BUT_FILCOL[i], MUIM_NoNotifySet, MUIA_Selected, TRUE);
            }
        }
    }
	
	if (FirstGradientColor != -1)
	{
        LONG G1, G2, GSize;
        ULONG red, green, blue, alpha;

        /* Second color selected => build gradient */
		G2 = max2(FirstGradientColor, CurrentSelectedUsrCol);
		G1 = min2(FirstGradientColor, CurrentSelectedUsrCol);	
		GSize = G2 - G1;
		for (i=G1+1;i<G2;i++)
		{
			alpha	= 	(int)GetA(ColorPalette[G1+NUM_STD_COLS].RGB) + ((int)GetA(ColorPalette[G2+NUM_STD_COLS].RGB) - (int)GetA(ColorPalette[G1+NUM_STD_COLS].RGB)) * (i - G1) / GSize;
			red		= 	(int)GetR(ColorPalette[G1+NUM_STD_COLS].RGB) + ((int)GetR(ColorPalette[G2+NUM_STD_COLS].RGB) - (int)GetR(ColorPalette[G1+NUM_STD_COLS].RGB)) * (i - G1) / GSize;
			green	= 	(int)GetG(ColorPalette[G1+NUM_STD_COLS].RGB) + ((int)GetG(ColorPalette[G2+NUM_STD_COLS].RGB) - (int)GetG(ColorPalette[G1+NUM_STD_COLS].RGB)) * (i - G1) / GSize;
			blue	= 	(int)GetB(ColorPalette[G1+NUM_STD_COLS].RGB) + ((int)GetB(ColorPalette[G2+NUM_STD_COLS].RGB) - (int)GetB(ColorPalette[G1+NUM_STD_COLS].RGB)) * (i - G1) / GSize;
			Add_UserColor(i, ((ULONG)alpha << 24) | ((ULONG)red << 16) | ((ULONG)green << 8) | ((ULONG)blue));
		}
		/* reset gradient build function */
		FirstGradientColor = -1;
        put_msg((char *)_(msg_StartColor));
	}

	if (FirstCopyColor != -1)
	{
        /* Copy Color to newly selected cell */
        Add_UserColor(CurrentSelectedUsrCol, ColorPalette[FirstCopyColor+NUM_STD_COLS].RGB);
        /* reset color copy function */
        FirstCopyColor = -1;
    }

	if (FirstSwapColor != -1)
	{
        ULONG tmp_col = ColorPalette[FirstSwapColor+NUM_STD_COLS].RGB;
        /* swap Colors to newly semected cell */
        Add_UserColor(FirstSwapColor, ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB);
        Add_UserColor(CurrentSelectedUsrCol, tmp_col);
        /* reset color swap function */
        FirstSwapColor = -1;
    }
    
    /* update colorwheel display */
    IgnoreColorAdj = TRUE;
    nnset(NUM_USRALPHA, MUIA_Numeric_Value    , (ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0xFF000000) >> 24);
    nnset(CAD_PENCOL,   MUIA_Coloradjust_Red  ,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x00FF0000) >> 16) * 0x01010101);
    nnset(CAD_PENCOL,   MUIA_Coloradjust_Green,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x0000FF00) >>  8) * 0x01010101);
    nnset(CAD_PENCOL,   MUIA_Coloradjust_Blue ,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x000000FF)      ) * 0x01010101);
    sprintf(argb_str, "%08lX", ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB);
    nnset(STR_COL_ARGB, MUIA_String_Contents, argb_str);
    IgnoreColorAdj = FALSE;
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoColorAdj                                                             */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorAdj,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorAdj(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    ULONG a=0,r=0,g=0,b=0;
	ULONG value;
    char argb_str[9];
    LONG activePage = 0;
    
    if (IgnoreColorAdj) return;
    get(RG_COLORTABS, MUIA_Group_ActivePage, &activePage);
    if (activePage != 0) return; /* Only process on User Colors tab */
    
	get(NUM_USRALPHA, MUIA_Numeric_Value  , &a);
	get(CAD_PENCOL, MUIA_Coloradjust_Red  , &r);
	get(CAD_PENCOL, MUIA_Coloradjust_Green, &g);
	get(CAD_PENCOL, MUIA_Coloradjust_Blue , &b);
	value = ((a << 24) | ((r & 0xFF000000) >> 8) | ((g & 0xFF000000) >> 16) | ((b & 0xFF000000) >> 24));
    sprintf(argb_str, "%08lX", value);
    nnset(STR_COL_ARGB, MUIA_String_Contents, argb_str);
	Add_UserColor(CurrentSelectedUsrCol, value);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoColorButtons                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorButtons,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorButtons(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    switch ((int)*Arg)
    {
        case 0:
            if (FirstGradientColor == -1)
            {
                /* First button selection => store first color */
                FirstGradientColor = CurrentSelectedUsrCol;
                put_msg((char *)_(msg_SelectSecondColor));
            }
            break;
        case 1:
            if (FirstCopyColor == -1)
            {
                /* First button selection => store first color */
                FirstCopyColor = CurrentSelectedUsrCol;
                put_msg((char *)_(msg_SelectCopyColor));
            }
            break;
        case 2:
            if (FirstSwapColor == -1)
            {
                /* First button selection => store first color */
                FirstSwapColor = CurrentSelectedUsrCol;
                put_msg((char *)_(msg_SelectSwapColor));
            }
            break;
        default:
            break;
    }
#ifdef __AROS__	
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradColorMutex                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradColorMutex,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradColorMutex(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    char argb_str[9];
    
    for (i=0; i < 16; i++)
    {
        if (Object != BUT_GRADCOLOR[i])
            DoMethod(BUT_GRADCOLOR[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
        {
			if (GradientTable[CurrentSelectedGrad]!=NULL)
			{
				CurrentSelectedGradStop = i;
                IgnoreGradColorAdj = TRUE;
				nnset(NUM_GRADALPHA, MUIA_Numeric_Value    ,((GradientTable[CurrentSelectedGrad]->Stop[i].color & 0xFF000000) >> 24));
				nnset(CAD_GRADCOL,   MUIA_Coloradjust_Red  ,((GradientTable[CurrentSelectedGrad]->Stop[i].color & 0x00FF0000) >> 16) * 0x01010101);
				nnset(CAD_GRADCOL,   MUIA_Coloradjust_Green,((GradientTable[CurrentSelectedGrad]->Stop[i].color & 0x0000FF00) >>  8) * 0x01010101);
				nnset(CAD_GRADCOL,   MUIA_Coloradjust_Blue ,((GradientTable[CurrentSelectedGrad]->Stop[i].color & 0x000000FF)      ) * 0x01010101);
                sprintf(argb_str, "%08lX", GradientTable[CurrentSelectedGrad]->Stop[i].color);
                nnset(STR_GRAD_ARGB, MUIA_String_Contents, argb_str);
                IgnoreGradColorAdj = FALSE;
				set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
				if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
					set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
			}
        }
    }
    
	if (FirstSwapStop != -1)
	{
        /* Second Stop selected => Swap colors */
        ULONG TmpColor;
        
        TmpColor = GradientTable[CurrentSelectedGrad]->Stop[FirstSwapStop].color;
        GradientTable[CurrentSelectedGrad]->Stop[FirstSwapStop].color = GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].color;
        GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].color = TmpColor;
        
        /* Recalc gradient previews */
        GradientTable[CurrentSelectedGrad]->precalc = FALSE;
        updategradcolors();
        set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
        if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
            set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
 
		/* reset gradient build function */
		put_msg((char *)_(msg_SwapDone), FirstSwapStop, CurrentSelectedGradStop);
		FirstSwapStop = -1;
	}
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradColorAdj                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradColorAdj,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradColorAdj(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    ULONG r=0,g=0,b=0,a=0;
    ULONG argb = 0;
    char argb_str[9];

    if (IgnoreGradColorAdj) return;

    get(NUM_GRADALPHA, MUIA_Numeric_Value  , &a);
	get(CAD_GRADCOL, MUIA_Coloradjust_Red  , &r);
	get(CAD_GRADCOL, MUIA_Coloradjust_Green, &g);
	get(CAD_GRADCOL, MUIA_Coloradjust_Blue , &b);
    argb = ((a << 24) | ((r & 0xFF000000) >> 8) | ((g & 0xFF000000) >> 16) | ((b & 0xFF000000) >> 24));
    sprintf(argb_str, "%08lX",argb);
    nnset(STR_GRAD_ARGB, MUIA_String_Contents, argb_str);
    if (GradientTable[CurrentSelectedGrad]!=NULL)
    {
        GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].color = argb;
        /* Update previews via dispatcher */
        DispatchColorUIUpdate(UPDATE_GRADIENTS, -1);
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradAddStop                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradAddStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradAddStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    
    if (GradientTable[CurrentSelectedGrad]->stops < 16)
    {
        LONG prevStop;
        LONG newStopVal;
        int newIdx;

        /* Calculate a sensible default position for the new stop */
        prevStop = GradientTable[CurrentSelectedGrad]->Stop[GradientTable[CurrentSelectedGrad]->stops-1].stopvalue;
        newStopVal = prevStop + (100 - prevStop) / 2;
        if (newStopVal <= prevStop) newStopVal = prevStop + 1;
        if (newStopVal > 100) newStopVal = 100;

        GradientTable[CurrentSelectedGrad]->stops++;
        newIdx = GradientTable[CurrentSelectedGrad]->stops - 1;
        GradientTable[CurrentSelectedGrad]->Stop[newIdx].color = 0xFF000000;
        GradientTable[CurrentSelectedGrad]->Stop[newIdx].stopvalue = newStopVal;
        GradientTable[CurrentSelectedGrad]->precalc = FALSE;
        updategradcolors();
        nnset(STR_GRAD_ARGB, MUIA_String_Contents, "FF000000");
        set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
		if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
			set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    }
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradRemStop                                                          */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFH3(void, DoGradRemStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradRemStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    

    if (GradientTable[CurrentSelectedGrad]->stops > 2)
    {
        GradientTable[CurrentSelectedGrad]->stops--;
        GradientTable[CurrentSelectedGrad]->precalc = FALSE;
        updategradcolors();
		set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
		if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
			set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    }
#ifdef __AROS__    
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradSwapStop                                                          */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFH3(void, DoGradSwapStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradSwapStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	if (FirstSwapStop == -1)
	{
		/* First button selection => store first stop */
		FirstSwapStop = CurrentSelectedGradStop;
        put_msg((char *)_(msg_SecondSwapStop), FirstSwapStop);
	}
#ifdef __AROS__    
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradType                                                             */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradType,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradType(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    

    get(CYC_GRADTYPE,   MUIA_Cycle_Active,  &(GradientTable[CurrentSelectedGrad]->type));
    if (GradientTable[CurrentSelectedGrad]->type == 0)
    {
        set(GRP_RADIAL, MUIA_ShowMe, FALSE);
        set(CHK_FIXFOCUS, MUIA_Selected, FALSE);
		set(TXT_P1, MUIA_Text_Contents, "\33cP1");
		set(TXT_P2, MUIA_Text_Contents, "\33cP2");
        GradientTable[CurrentSelectedGrad]->x1 = 0;
        GradientTable[CurrentSelectedGrad]->y1 = 0; 
        GradientTable[CurrentSelectedGrad]->x2 = 100;
        GradientTable[CurrentSelectedGrad]->y2 = 100;
        DoMethod(NUM_GRADX1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x1);
        DoMethod(NUM_GRADY1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y1); 
        DoMethod(NUM_GRADX2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x2);
        DoMethod(NUM_GRADY2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y2);
    }
    else
    {
        set(GRP_RADIAL, MUIA_ShowMe, TRUE);
		set(TXT_P1, MUIA_Text_Contents, "\33cC");
		set(TXT_P2, MUIA_Text_Contents, "\33cF");
        GradientTable[CurrentSelectedGrad]->x1 = 50;
        GradientTable[CurrentSelectedGrad]->y1 = 50; 
        GradientTable[CurrentSelectedGrad]->x2 = 50;
        GradientTable[CurrentSelectedGrad]->y2 = 50;
        GradientTable[CurrentSelectedGrad]->radius = 50;        
        DoMethod(NUM_GRADX1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x1);
        DoMethod(NUM_GRADY1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y1); 
        DoMethod(NUM_GRADX2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x2);
        DoMethod(NUM_GRADY2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y2);
        DoMethod(NUM_GRADRADIUS, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->radius);
        set(CHK_FIXFOCUS, MUIA_Selected, TRUE);
    }
 	set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
	if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
		set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
   
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradChg                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradChg,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradChg(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    

    /* get new gradient number */
    get(NUM_GRADNUM, MUIA_Numeric_Value, &CurrentSelectedGrad);
    
    if (GradientTable[CurrentSelectedGrad] != NULL)
    {
        /* read gradient value if defined and update display*/
        DoMethod(CYC_GRADTYPE,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->type);
        DoMethod(CYC_GRADSPREAD, MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->spread);
        DoMethod(CYC_GRADUNIT,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->unit);
        DoMethod(NUM_GRADX1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x1);
        DoMethod(NUM_GRADY1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y1); 
        DoMethod(NUM_GRADY2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y2);
        
        /* turn on display */
        set(BUT_ADDGRAD, MUIA_Disabled, TRUE);
        set(BUT_COPYGRAD, MUIA_Disabled, FALSE);
        set(BUT_DELGRAD, MUIA_Disabled, FALSE);
        set(VG_GRADEDIT, MUIA_ShowMe, TRUE);
        set(VG_GRADEDITNULL, MUIA_ShowMe, FALSE);
        
        updategradcolors();
        
        if (GradientTable[CurrentSelectedGrad]->type == 0)
        {
            set(GRP_RADIAL, MUIA_ShowMe, FALSE);
        }
        else
        {
            set(GRP_RADIAL, MUIA_ShowMe, TRUE);
            DoMethod(NUM_GRADRADIUS, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->radius);
        }
    }
    else
    {
        /* turn off display */
		set(BUT_ADDGRAD, MUIA_Disabled, FALSE);
		set(BUT_COPYGRAD, MUIA_Disabled, TRUE);
		set(BUT_DELGRAD, MUIA_Disabled, TRUE);
        set(VG_GRADEDIT, MUIA_ShowMe, FALSE);
        set(VG_GRADEDITNULL, MUIA_ShowMe, TRUE);
    }
    
    /* Update gradient preview */
    set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
        set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    
#ifdef __AROS__   
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradAdj                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradAdj,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradAdj(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    BOOL link = FALSE;
    
    get(CHK_FIXFOCUS, MUIA_Selected, &link);

    switch (*Arg)
    {
        case 0:
            get(CYC_GRADSPREAD, MUIA_Cycle_Active,  &(GradientTable[CurrentSelectedGrad]->spread));
            break;
        case 1:
            get(CYC_GRADUNIT,   MUIA_Cycle_Active,  &(GradientTable[CurrentSelectedGrad]->unit));
            break;
        case 2:
            get(NUM_GRADX1,     MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->x1));
            if(link)
            {
                DoMethod(NUM_GRADX2, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x1);
                GradientTable[CurrentSelectedGrad]->x2 = GradientTable[CurrentSelectedGrad]->x1;
            }
            break;
        case 3:
            get(NUM_GRADY1,     MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->y1));
            GradientTable[CurrentSelectedGrad]->y1 = GradientTable[CurrentSelectedGrad]->y1;
            if(link)
            {
                DoMethod(NUM_GRADY2, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y1);
                GradientTable[CurrentSelectedGrad]->y2 = GradientTable[CurrentSelectedGrad]->y1;
            }
            break;
        case 4:
            get(NUM_GRADX2,     MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->x2));
            if(link)
            {
                DoMethod(NUM_GRADX1, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x2);
                GradientTable[CurrentSelectedGrad]->x1 = GradientTable[CurrentSelectedGrad]->x2;
            }
            break;
        case 5:
            get(NUM_GRADY2,     MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->y2));
            GradientTable[CurrentSelectedGrad]->y2 = GradientTable[CurrentSelectedGrad]->y2;
            if(link)
            {
                DoMethod(NUM_GRADY1, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y2);
                GradientTable[CurrentSelectedGrad]->y1 = GradientTable[CurrentSelectedGrad]->y2;
            }
            break;
        case 6:
            get(NUM_GRADRADIUS, MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->radius));
            break;
    }
    /* Update Display */
    set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
	if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
		set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradStop                                                             */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    
    for (i=0; i < 16; i++)
    {
        if (Object == NUM_GRADSTOP[i])
        {
			CurrentSelectedGradStop = i;
            get(NUM_GRADSTOP[CurrentSelectedGradStop], MUIA_Numeric_Value, &(GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].stopvalue));
			if (CurrentSelectedGradStop > 0)
				set(NUM_GRADSTOP[CurrentSelectedGradStop-1], MUIA_Numeric_Max, GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].stopvalue);
			if (CurrentSelectedGradStop < 15)
				set(NUM_GRADSTOP[CurrentSelectedGradStop+1], MUIA_Numeric_Min, GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].stopvalue);
            set(BUT_GRADCOLOR[CurrentSelectedGradStop], MUIA_Selected, TRUE);
            GradientTable[CurrentSelectedGrad]->precalc = FALSE;
			set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
			if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
				set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
        }
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradAdd                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradAdd,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoGradAdd(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	int grad = 0;
    
    switch (*Arg)
    {
        case 0:
            /* create new gradient */
            get(NUM_GRADNUM,  MUIA_Numeric_Value, &CurrentSelectedGrad);
            if (GradientTable[CurrentSelectedGrad] == NULL)
            {
#ifdef __amigaos4__
                GradientTable[CurrentSelectedGrad] = AllocVecTags(sizeof(Gradient), TAG_DONE);
#else
                GradientTable[CurrentSelectedGrad] = AllocVec(sizeof(Gradient),MEMF_ANY);
#endif
                if (GradientTable[CurrentSelectedGrad] != NULL)
                {
                    set(BUT_ADDGRAD, MUIA_Disabled, TRUE);
                    set(BUT_COPYGRAD, MUIA_Disabled, FALSE);
                    set(BUT_DELGRAD, MUIA_Disabled, FALSE);
                    /* create a default gradient */
                    GradientTable[CurrentSelectedGrad]->type = 0;
                    GradientTable[CurrentSelectedGrad]->spread = 0;
                    GradientTable[CurrentSelectedGrad]->unit = 0;
                    GradientTable[CurrentSelectedGrad]->x1 = 0;
                    GradientTable[CurrentSelectedGrad]->y1 = 0; 
                    GradientTable[CurrentSelectedGrad]->x2 = 100;
                    GradientTable[CurrentSelectedGrad]->y2 = 100;
                    GradientTable[CurrentSelectedGrad]->radius = 100;
                    GradientTable[CurrentSelectedGrad]->stops = 2;
                    GradientTable[CurrentSelectedGrad]->Stop[0].color = 0xFF000000;
                    GradientTable[CurrentSelectedGrad]->Stop[0].stopvalue = 0;
                    GradientTable[CurrentSelectedGrad]->Stop[1].color = 0xFFFFFFFF;
                    GradientTable[CurrentSelectedGrad]->Stop[1].stopvalue = 100;
                    GradientTable[CurrentSelectedGrad]->precalc = FALSE;
                    DoMethod(CYC_GRADTYPE,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->type);
                    DoMethod(CYC_GRADSPREAD, MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->spread);
                    DoMethod(CYC_GRADUNIT,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[CurrentSelectedGrad]->unit);
                    DoMethod(NUM_GRADX1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x1);
                    DoMethod(NUM_GRADY1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y1); 
                    DoMethod(NUM_GRADX2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->x2);
                    DoMethod(NUM_GRADY2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->y2);
                    if (GradientTable[CurrentSelectedGrad]->type == 0)
                    {
                        set(GRP_RADIAL, MUIA_ShowMe, FALSE);
                    }
                    else
                    {
                        set(GRP_RADIAL, MUIA_ShowMe, TRUE);
                        DoMethod(NUM_GRADRADIUS, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->radius);
                    }
                    set(VG_GRADEDIT, MUIA_ShowMe, TRUE);
                    set(VG_GRADEDITNULL, MUIA_ShowMe, FALSE);
                    updategradcolors();
                    set(BUT_GRADCOLOR[0], MUIA_Selected, TRUE);
                    set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
                    if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
                        set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
                }
            }
            break;
        case 1:
            /* copy current gradient */
            get(NUM_GRADNUM,  MUIA_Numeric_Value, &CurrentSelectedGrad);
            if (GradientTable[CurrentSelectedGrad] != NULL)
            {
                /* search for first available gradient */
                while(GradientTable[grad] != NULL) grad++;
                
                if (grad < 512)
                {
#ifdef __amigaos4__
                    GradientTable[grad] = AllocVecTags(sizeof(Gradient), TAG_DONE);
#else
                    GradientTable[grad] = AllocVec(sizeof(Gradient),MEMF_ANY);
#endif
                    if (GradientTable[grad] != NULL)
                    {
                        set(BUT_ADDGRAD, MUIA_Disabled, TRUE);
                        set(BUT_COPYGRAD, MUIA_Disabled, FALSE);
                        set(BUT_DELGRAD, MUIA_Disabled, FALSE);
                        /* copy gradient */
                        CopyMem(GradientTable[CurrentSelectedGrad], GradientTable[grad], sizeof(Gradient));
                        
                        /* set current gradient to newly created gradient */
                        CurrentSelectedGrad = grad;
                        DoMethod(NUM_GRADNUM,    MUIM_NoNotifySet, MUIA_Numeric_Value, grad);
                        DoMethod(CYC_GRADTYPE,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[grad]->type);
                        DoMethod(CYC_GRADSPREAD, MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[grad]->spread);
                        DoMethod(CYC_GRADUNIT,   MUIM_NoNotifySet, MUIA_Cycle_Active,  GradientTable[grad]->unit);
                        DoMethod(NUM_GRADX1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[grad]->x1);
                        DoMethod(NUM_GRADY1,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[grad]->y1); 
                        DoMethod(NUM_GRADX2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[grad]->x2);
                        DoMethod(NUM_GRADY2,     MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[grad]->y2);
                        if (GradientTable[grad]->type == 0)
                        {
                            set(GRP_RADIAL, MUIA_ShowMe, FALSE);
                        }
                        else
                        {
                            set(GRP_RADIAL, MUIA_ShowMe, TRUE);
                            DoMethod(NUM_GRADRADIUS, MUIM_NoNotifySet, MUIA_Numeric_Value, GradientTable[grad]->radius);
                        }
                        set(VG_GRADEDIT, MUIA_ShowMe, TRUE);
                        set(VG_GRADEDITNULL, MUIA_ShowMe, FALSE);
                        updategradcolors();
                        set(BUT_GRADCOLOR[0], MUIA_Selected, TRUE);
                        set(GRD_PREV, MYATTR_GRAD, GradientTable[grad]);
                        if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == grad)
                            set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[grad]);
                    }
                }
            }
            break;
        case 2:
            /* delete current gradient */ 
            get(NUM_GRADNUM,  MUIA_Numeric_Value, &CurrentSelectedGrad);
            if (GradientTable[CurrentSelectedGrad] != NULL)
            {
                FreeVec(GradientTable[CurrentSelectedGrad]);
                GradientTable[CurrentSelectedGrad] = NULL;
                set(BUT_ADDGRAD, MUIA_Disabled, FALSE);
                set(BUT_COPYGRAD, MUIA_Disabled, TRUE);
                set(BUT_DELGRAD, MUIA_Disabled, TRUE);
                set(VG_GRADEDIT, MUIA_ShowMe, FALSE);
                set(VG_GRADEDITNULL, MUIA_ShowMe, TRUE);
                /* Update gradient preview */
                set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
                if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
                    set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
            }
            break;
        default:
            break;
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoColorTabChange                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorTabChange,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorTabChange(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    ActiveColorTab = (LONG)*Arg;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoColorPicker                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorPicker,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorPicker(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    ULONG PickedColor = 0xFFFFFFFF, OldPickedColor = 0xFFFFFFFF;
    UWORD X,Y, OldX = 0, OldY = 0;
    char argb_str[9];
        
    put_msg((char *)_(msg_ColorPicker));    
    
    Delay(10);
    while ((PeekQualifier() & IEQUALIFIER_LEFTBUTTON) != IEQUALIFIER_LEFTBUTTON) 
    {
        X = Scr->MouseX;
        Y = Scr->MouseY;
        if ((X != OldX) || (Y != OldY))
        {
            OldX = X;
            OldY = Y;
            PickedColor =  ReadRGBPixel(&(Scr->RastPort), X, Y);
            PickedColor |= 0xFF000000;
            if (PickedColor != OldPickedColor)
            {
                OldPickedColor = PickedColor;
                if (Object == BUT_COL_CP)
                {
                    Add_UserColor(CurrentSelectedUsrCol, PickedColor);
                    IgnoreColorAdj = TRUE;
                    nnset(NUM_USRALPHA, MUIA_Numeric_Value    , (ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0xFF000000) >> 24);
                    nnset(CAD_PENCOL,   MUIA_Coloradjust_Red  ,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x00FF0000) >> 16) * 0x01010101);
                    nnset(CAD_PENCOL,   MUIA_Coloradjust_Green,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x0000FF00) >>  8) * 0x01010101);
                    nnset(CAD_PENCOL,   MUIA_Coloradjust_Blue ,((ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB & 0x000000FF)      ) * 0x01010101);
                    sprintf(argb_str, "%08lX", ColorPalette[CurrentSelectedUsrCol+NUM_STD_COLS].RGB);
                    nnset(STR_COL_ARGB, MUIA_String_Contents, argb_str);
                    IgnoreColorAdj = FALSE;
                }
                else if (Object == BUT_GRAD_CP)
                {
                    GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].color = PickedColor;
                    updategradcolors();
                    IgnoreGradColorAdj = TRUE;
                    nnset(NUM_GRADALPHA, MUIA_Numeric_Value    ,((PickedColor & 0xFF000000) >> 24));
                    nnset(CAD_GRADCOL,   MUIA_Coloradjust_Red  ,((PickedColor & 0x00FF0000) >> 16) * 0x01010101);
                    nnset(CAD_GRADCOL,   MUIA_Coloradjust_Green,((PickedColor & 0x0000FF00) >>  8) * 0x01010101);
                    nnset(CAD_GRADCOL,   MUIA_Coloradjust_Blue ,((PickedColor & 0x000000FF)      ) * 0x01010101);
                    sprintf(argb_str, "%08lX", PickedColor);
                    nnset(STR_GRAD_ARGB, MUIA_String_Contents, argb_str);
                    IgnoreGradColorAdj = FALSE;
                    set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
                    if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
                        set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
                }
            }
        }
        Delay(1);
    }   
    DoMethod(Object, MUIM_NoNotifySet, MUIA_Selected, FALSE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoColorString                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoColorString,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1))
{
    AROS_USERFUNC_INIT
#else
void DoColorString(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    ULONG argb,a;
    STRPTR argb_str = (STRPTR)*Arg;
    
    sscanf((char *)argb_str, "%08lX", &argb);
    
    if (Object == STR_COL_ARGB)
    {
        Add_UserColor(CurrentSelectedUsrCol, argb);
        /* update colorwheel display */
        a = (argb & 0xFF000000) >> 24;
        IgnoreColorAdj = TRUE;
        nnset(NUM_USRALPHA, MUIA_Numeric_Value    ,a==255?0:a);
        nnset(CAD_PENCOL,   MUIA_Coloradjust_Red  ,((argb & 0x00FF0000) >> 16) * 0x01010101);
        nnset(CAD_PENCOL,   MUIA_Coloradjust_Green,((argb & 0x0000FF00) >>  8) * 0x01010101);
        nnset(CAD_PENCOL,   MUIA_Coloradjust_Blue ,((argb & 0x000000FF)      ) * 0x01010101);
        IgnoreColorAdj = FALSE;
    }
    else if (Object == STR_GRAD_ARGB)
    {
        GradientTable[CurrentSelectedGrad]->Stop[CurrentSelectedGradStop].color = argb;
        updategradcolors();
        /* update colorwheel display */
        IgnoreGradColorAdj = TRUE;
        nnset(NUM_GRADALPHA, MUIA_Numeric_Value    ,((argb & 0xFF000000) >> 24));
        nnset(CAD_GRADCOL,   MUIA_Coloradjust_Red  ,((argb & 0x00FF0000) >> 16) * 0x01010101);
        nnset(CAD_GRADCOL,   MUIA_Coloradjust_Green,((argb & 0x0000FF00) >>  8) * 0x01010101);
        nnset(CAD_GRADCOL,   MUIA_Coloradjust_Blue ,((argb & 0x000000FF)      ) * 0x01010101);
        IgnoreGradColorAdj = FALSE;
        set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
        if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
            set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    }
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}


void updategradcolors(void)
{
    LONG i;
    
    if (GradientTable[CurrentSelectedGrad] != NULL)
    {
        for (i=0; i<16; i++)
        {
            if (i<GradientTable[CurrentSelectedGrad]->stops)
            {
                SetColorButtonRGB(BUT_GRADCOLOR[i], GradientTable[CurrentSelectedGrad]->Stop[i].color);
                set(NUM_GRADSTOP[i], MUIA_Numeric_Value, GradientTable[CurrentSelectedGrad]->Stop[i].stopvalue);
                set(GRP_GRADSTOP[i], MUIA_ShowMe, TRUE);
            }
            else
            {
                set(GRP_GRADSTOP[i], MUIA_ShowMe, FALSE);
            }
        }
    }
}

void SetUsrColBut(int usrcol, ULONG value)
{
	extern APTR BUT_PENCOL[];
    extern APTR BUT_FILCOL[];
    extern int cur_fillcolor;
    extern void UpdateFShades(void);

    SetColorButtonRGB(BUT_USRCOL[usrcol], value);
    SetColorButtonRGB(BUT_PENCOL[usrcol + NUM_STD_COLS], value);
    SetColorButtonRGB(BUT_FILCOL[usrcol + NUM_STD_COLS], value);
    
    if ((usrcol+NUM_STD_COLS) == cur_fillcolor)
    {
        UpdateFShades();
    }
}

void Color_ChangeSkin(void)
{
    UBYTE iconfile[128];
    
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"color_picker.png");
	set(BUT_COL_CP, MYATTR_IMAGEFILE, iconfile);
	set(BUT_GRAD_CP, MYATTR_IMAGEFILE, iconfile);
}

void SetColorButtonRGB(APTR button, ULONG rgb)
{
    if (!button) return;

    /* Use custom ColorSwatch class attribute for direct truecolor update.
       Strip alpha — MUIA_ColorSwatch_RGB uses 0x00RRGGBB format. */
    set(button, MUIA_ColorSwatch_RGB, (rgb & 0x00FFFFFF));
}

void RefreshAllColorButtons(void)
{
    extern APTR BUT_PENCOL[];
    extern APTR BUT_FILCOL[];
    extern void UpdateFShades(void);
    int i;

    /* Refresh all pen and fill buttons from ColorPalette */
    for (i = 0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        if (i < NUM_STD_COLS || ColorPalette[i].Defined) {
            SetColorButtonRGB(BUT_PENCOL[i], ColorPalette[i].RGB);
            SetColorButtonRGB(BUT_FILCOL[i], ColorPalette[i].RGB);
        }
    }

    /* Refresh user color buttons */
    for (i = 0; i < MAX_USR_COLS; i++)
    {
        if (ColorPalette[i + NUM_STD_COLS].Defined) {
            SetColorButtonRGB(BUT_USRCOL[i], ColorPalette[i + NUM_STD_COLS].RGB);
        }
    }

    /* Refresh shade ramp */
    UpdateFShades();
}

/* --- Standardized Sync Wrappers --- */

void SyncPenPalette(int index) {
    extern APTR BUT_PENCOL[];
    if (index >= 0 && index < (NUM_STD_COLS + MAX_USR_COLS)) {
        SetColorButtonRGB(BUT_PENCOL[index], ColorPalette[index].RGB);
    }
}

void SyncFillPalette(int index) {
    extern APTR BUT_FILCOL[];
    if (index >= 0 && index < (NUM_STD_COLS + MAX_USR_COLS)) {
        SetColorButtonRGB(BUT_FILCOL[index], ColorPalette[index].RGB);
    }
}

void SyncUserPalette(int index) {
    if (index >= 0 && index < MAX_USR_COLS) {
        SetColorButtonRGB(BUT_USRCOL[index], ColorPalette[index + NUM_STD_COLS].RGB);
    }
}

void SyncShadeRamp(void) {
    extern void UpdateFShades(void);
    UpdateFShades();
}

void SyncGradientEditor(void) {
    extern void updategradcolors(void);
    extern APTR GRD_PREV, GRD_PREVIEW;
    extern int cur_fillcolor;
    updategradcolors();
    if (GradientTable[CurrentSelectedGrad] != NULL) {
        GradientTable[CurrentSelectedGrad]->precalc = FALSE;
        set(GRD_PREV, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
        if ((cur_fillcolor - (NUM_STD_COLS + MAX_USR_COLS)) == CurrentSelectedGrad)
            set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[CurrentSelectedGrad]);
    }
}

/* Master Dispatcher */
void DispatchColorUIUpdate(ULONG area_mask, int index) {
    if (area_mask & UPDATE_PALETTES) {
        if (index < 0) {
            RefreshAllColorButtons();
        } else {
            SyncPenPalette(index);
            SyncFillPalette(index);
            if (index >= NUM_STD_COLS) {
                SyncUserPalette(index - NUM_STD_COLS);
            }
        }
    }
    
    if (area_mask & UPDATE_SHADES) {
        SyncShadeRamp();
    }
    
    if (area_mask & UPDATE_GRADIENTS) {
        SyncGradientEditor();
    }
}
