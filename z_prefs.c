/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_prefs.c $
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
#include "w_msgpanel.h"
#include "z_fig.h"
#include "z_mode.h"
#include "z_color.h"
#include "z_edit.h"
#include "u_fonts.h"
#include "u_redraw.h"
#include "mcc_fontdisp.h"
#include "mcc_arrowslider.h"
#include <graphics/displayinfo.h>

static void GetScreenModeName(ULONG modeID, ULONG w, ULONG h, ULONG d, ULONG as, STRPTR buffer)
{
    struct NameInfo ni;
    BOOL hasName = FALSE;

    if (modeID == 0)
    {
        sprintf(buffer, "Workbench (%lux%lux%u)%s", w, h, d, as ? " [AS]" : "");
        return;
    }

    if (modeID != INVALID_ID)
    {
        if (GetDisplayInfoData(NULL, (UBYTE *)&ni, sizeof(ni), DTAG_NAME, modeID) > 0)
        {
            hasName = TRUE;
        }
    }

    if (hasName)
    {
        sprintf(buffer, "%s%s", ni.Name, as ? "  [AS]" : "");
    }
    else
    {
        sprintf(buffer, "%lux%lux%u%s [ID=%08lX]", w, h, d, as ? "  [AS]" : "", modeID);
    }
}

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFP3(void, UpdatePrefs,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, OpenPrefs,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, ApplyPrefs,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, ScreenModeStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SkinStop,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DisplayPageDim,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetFontPreview,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SelectNewFont,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void UpdatePrefs(struct Hook *h, APTR Object, ULONG *Arg);
void OpenPrefs(struct Hook *h, APTR Object, ULONG *Arg);
void ApplyPrefs(struct Hook *h, APTR Object, ULONG *Arg);
void ScreenModeStop(struct Hook *h, APTR Object, ULONG *Arg);
void SkinStop(struct Hook *h, APTR Object, ULONG *Arg);
void DisplayPageDim(struct Hook *h, APTR Object, ULONG *Arg);
void SetFontPreview(struct Hook *h, APTR Object, ULONG *Arg);
void SelectNewFont(struct Hook *h, APTR Object, ULONG *Arg);
#endif    

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/

/* overall gadgets */
APTR    BUT_OPT_OPEN, BUT_OPT_SAVE, BUT_OPT_USE, BUT_OPT_CANCEL;

/* Screen panel gadgets */
APTR	CHK_OPT_USEPUBSCREEN, STR_OPT_PUBSCREENNAME, CHK_OPT_BACKDROP;
APTR	POP_OPT_SCREENMODE;
APTR	POP_OPT_SKIN;
ULONG   prefs_ScreenMode;
ULONG   prefs_ScreenWidth, prefs_ScreenHeight, prefs_ScreenDepth, prefs_AutoScroll;
	
/* Page panel gadgets */
APTR	CYC_OPT_PFORMAT, CYC_OPT_PORIENT;
APTR	TXT_OPT_PWIDTH, TXT_OPT_PHEIGHT;

/* Display panel gadgets */
APTR	CYC_OPT_UNIT;
APTR	CHK_OPT_AUTOACTIVATE;
APTR	CHK_OPT_TOPRULER, CHK_OPT_SIDERULER;
APTR	CHK_OPT_AUTOSCROLLCANVAS, CHK_OPT_SOFTSCROLLCANVAS;
APTR	CHK_OPT_BESTQUALITY, CHK_OPT_ONSCREENAA;
APTR	NUM_OPT_USRSCALE, CHK_OPT_SHOWLENGTHS, CHK_OPT_SHOWNUMS;
APTR	NUM_OPT_FREEHANDRES, NUM_OPT_GRADSPREAD;

/* Font setting panel */
APTR	LST_OPT_FNT, FNT_OPT_PREVIEW;

/* I/O panel gadgets */
APTR	CHK_OPT_WRITEBACKUP, CHK_OPT_AUTOLOADLIB, CHK_OPT_AUTOSAVESETTINGS;
APTR	STR_OPT_DEFTOOL, CHK_OPT_CREATEICON, CHK_OPT_SAVEWHOLEPAL;
APTR	STR_OPT_F2DCMD, STR_OPT_F2DSCREEN, STR_OPT_F2DOUTPUT, STR_OPT_F2DINPUT, STR_OPT_F2DOTHER;
APTR	STR_OPT_R2VCMD, STR_OPT_R2VSCREEN, STR_OPT_R2VOUTPUT, STR_OPT_R2VOTHER;
APTR	NUM_OPT_SVGINTERP, NUM_OPT_SVGSCALE;

/* Misc panel gadgets */
APTR	STR_OPT_PRINTBUF, CHK_OPT_ALLOWNEGCOORD, CHK_OPT_DEBUG, CYC_OPT_GRIDUNIT;

/*----------------------------------------------------------------------------*/
/*     MakePrefsWindow - creates the preference window                        */
/*----------------------------------------------------------------------------*/

APTR MakePrefsWindow(void)
{
	int 		 i;
	static char *prefsregtitles[] = {NULL, NULL, NULL, NULL, NULL };
	static char *CYC_OPT_PORIENT_strings[] = {NULL, NULL, NULL};
	static char *CYC_OPT_UNIT_strings[] = {NULL, NULL, NULL};
	static char *CYC_OPT_GRIDUNIT_strings[] = {NULL, NULL, NULL};
	static char *CYC_OPT_PFORMAT_strings[NUMPAPERSIZES+1];
	char 		 page_width_str[80], page_height_str[80];
    static struct Hook ScreenModeStopHook;
    ScreenModeStopHook.h_Entry = (HOOKFUNC)ScreenModeStop;
    static struct Hook SkinStopHook;
    SkinStopHook.h_Entry = (HOOKFUNC)SkinStop;
    char        screenmode[256];

	prefsregtitles[0] = (char *)_(msg_Display);
    prefsregtitles[1] = (char *)_(msg_Page);
    prefsregtitles[2] = (char *)_(msg_IO);
    prefsregtitles[3] = (char *)_(msg_Misc);
	CYC_OPT_PORIENT_strings[0] = (char *)_(msg_Portrait);
    CYC_OPT_PORIENT_strings[1] = (char *)_(msg_Landscape);
	CYC_OPT_UNIT_strings[0] = (char *)_(msg_Imperial);
    CYC_OPT_UNIT_strings[1] = (char *)_(msg_Metric);
	CYC_OPT_GRIDUNIT_strings[0] = (char *)_(msg_Fractionnal);
    CYC_OPT_GRIDUNIT_strings[1] = (char *)_(msg_Tenth);
    
	/* Screen panel gadgets */
	CHK_OPT_USEPUBSCREEN	= MakeCheckmark((UBYTE *)_(msg_PrefsUsePubScr), appres.UsePubScreen);
    STR_OPT_PUBSCREENNAME	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 64,
        MUIA_String_Contents, appres.PubScreenName,
    End;
    CHK_OPT_BACKDROP		= MakeCheckmark((UBYTE *)_(msg_PrefsDrawInBackDrop), appres.Backdrop);
    GetScreenModeName(appres.ScreenMode, appres.ScreenWidth, appres.ScreenHeight, appres.ScreenDepth, appres.AutoScroll, screenmode);
    POP_OPT_SCREENMODE = PopaslObject,
		MUIA_Frame				, MUIV_Frame_Group,
		MUIA_FrameTitle			, (UBYTE *)_(msg_ScreenMode),
		MUIA_Popstring_String	,   StringObject,
										StringFrame,
										MUIA_String_MaxLen , 256,
										MUIA_String_Contents, screenmode,
									End,
		MUIA_Popstring_Button	, PopButton(MUII_PopFile),
		MUIA_Popasl_Type 		, ASL_ScreenModeRequest,
        MUIA_Popasl_StopHook    , &ScreenModeStopHook,
		ASLFR_TitleText			, (char *)_(msg_ScreenModeSelect),
	End;
    POP_OPT_SKIN = PopaslObject,
		MUIA_Frame				, MUIV_Frame_Group,
		MUIA_FrameTitle			, (UBYTE *)_(msg_Skin),
		MUIA_Popstring_String	,   StringObject,
										StringFrame,
										MUIA_String_MaxLen , 256,
										MUIA_String_Contents, appres.skin,
									End,
		MUIA_Popstring_Button	, PopButton(MUII_PopFile),
		MUIA_Popasl_Type 		, ASL_FileRequest,
        ASLFR_DrawersOnly       , TRUE,
		ASLFR_InitialDrawer		, "PROGDIR:Skins",
        ASLFR_InitialFile       , "saimon69",
		ASLFR_TitleText			, (char *)_(msg_SkinSelect),
        MUIA_Popasl_StopHook    , &SkinStopHook,
	End;
	/* Page panel gadgets */
	if (appres.landscape)
	{
		make_dimension_string(paper_sizes[appres.papersize].height, page_width_str, FALSE);
		make_dimension_string(paper_sizes[appres.papersize].width, page_height_str, FALSE);
	}
	else
	{
		make_dimension_string(paper_sizes[appres.papersize].width, page_width_str, FALSE);
		make_dimension_string(paper_sizes[appres.papersize].height, page_height_str, FALSE);
	}
	for(i=0; i<NUMPAPERSIZES; i++)
	{
		CYC_OPT_PFORMAT_strings[i] = (char *)paper_sizes[i].sname;
	}
	CYC_OPT_PFORMAT_strings[NUMPAPERSIZES] = NULL;	
	CYC_OPT_PFORMAT = CycleObject,
		MUIA_Cycle_Entries, CYC_OPT_PFORMAT_strings,
		MUIA_Cycle_Active, appres.papersize,
	End;
	CYC_OPT_PORIENT = CycleObject,
		MUIA_Cycle_Entries, CYC_OPT_PORIENT_strings,
		MUIA_Cycle_Active, (appres.landscape?1L:0L),
	End;

	/* Display panel gadgets */
	CYC_OPT_UNIT = CycleObject,
		MUIA_Cycle_Entries, CYC_OPT_UNIT_strings,
		MUIA_Cycle_Active, (appres.INCHES?0L:1L),
	End; 
	CHK_OPT_AUTOACTIVATE		= MakeCheckmark((UBYTE *)_(msg_AutoactivateWin), appres.AutoActivate);
	CHK_OPT_TOPRULER			= MakeCheckmark((UBYTE *)_(msg_TopRuler), appres.topruler_on);
	CHK_OPT_SIDERULER			= MakeCheckmark((UBYTE *)_(msg_SideRuler), appres.sideruler_on);
	CHK_OPT_AUTOSCROLLCANVAS	= MakeCheckmark((UBYTE *)_(msg_AutoScroll), appres.autoScrollCanvas);
	CHK_OPT_SOFTSCROLLCANVAS	= MakeCheckmark((UBYTE *)_(msg_SoftScroll), appres.softScrollCanvas);
	CHK_OPT_BESTQUALITY			= MakeCheckmark((UBYTE *)_(msg_HQDisplay), appres.viewBestQuality);
	CHK_OPT_ONSCREENAA			= MakeCheckmark((UBYTE *)_(msg_OnScreenAA), appres.OnScreenAA);
	NUM_OPT_USRSCALE			= NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 200L,
		MUIA_Numeric_Value, round(appres.userscale * 100),
        TAG_DONE);
	CHK_OPT_SHOWLENGTHS			= MakeCheckmark((UBYTE *)_(msg_ShowLength), appres.showlengths);
	CHK_OPT_SHOWNUMS			= MakeCheckmark((UBYTE *)_(msg_ShowNums), appres.shownums);
	NUM_OPT_FREEHANDRES			= NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 1L,
		MUIA_Numeric_Max  , 1000L,
		MUIA_Numeric_Value, appres.freehand_resolution,
        TAG_DONE);
    NUM_OPT_GRADSPREAD		= NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 0L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, appres.gradient_ditherspread,
        TAG_DONE);
        
	/* I/O panel gadgets */
	CHK_OPT_WRITEBACKUP			= MakeCheckmark((UBYTE *)_(msg_WriteBackup), appres.WriteBackup);
	CHK_OPT_AUTOLOADLIB			= MakeCheckmark((UBYTE *)_(msg_AutoloadLibrary), appres.AutoLoadLib);
    CHK_OPT_AUTOSAVESETTINGS	= MakeCheckmark((UBYTE *)_(msg_AutoSaveSettings), appres.autosave);
	STR_OPT_DEFTOOL	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 256,
        MUIA_String_Contents, appres.DefTool,
    End;
	CHK_OPT_CREATEICON			= MakeCheckmark((UBYTE *)_(msg_CreateIcon), appres.CreateIcons);
	CHK_OPT_SAVEWHOLEPAL		= MakeCheckmark((UBYTE *)_(msg_SavePalette), appres.save_whole_palette);
	STR_OPT_F2DCMD	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 256,
        MUIA_String_Contents, appres.f2d_cmd,
    End;
	STR_OPT_F2DSCREEN	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.f2d_screenopt,
    End;
	STR_OPT_F2DOUTPUT	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.f2d_outputopt,
    End;
	STR_OPT_F2DINPUT	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.f2d_inputopt,
    End;
	STR_OPT_F2DOTHER	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.f2d_otheropt,
    End;
	STR_OPT_R2VCMD	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 256,
        MUIA_String_Contents, appres.r2v_cmd,
    End;
	STR_OPT_R2VSCREEN	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.r2v_screenopt,
    End;
	STR_OPT_R2VOUTPUT	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.r2v_outputopt,
    End;
	STR_OPT_R2VOTHER	= StringObject,
        StringFrame,
        MUIA_String_MaxLen , 32,
        MUIA_String_Contents, appres.r2v_otheropt,
    End;
	NUM_OPT_SVGINTERP			= NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 1L,
		MUIA_Numeric_Max  , 100L,
		MUIA_Numeric_Value, appres.SVG_InterSteps,
        TAG_DONE);
	NUM_OPT_SVGSCALE			= NewObject(	mcc_asl->mcc_Class, NULL,
		MUIA_Group_Horiz  , TRUE,
		MUIA_Numeric_Min  , 10L,
		MUIA_Numeric_Max  , 400L,
		MUIA_Numeric_Value, appres.SVG_Scale,
        TAG_DONE);

	/* Font panel */
	LST_OPT_FNT = ListviewObject,
		MUIA_Listview_List , ListObject,
			ReadListFrame,
			MUIA_List_SourceArray, TXTFONT_strings,
			MUIA_List_Active, ((cur_textflags & PSFONT_TEXT)?cur_ps_font+1:cur_latex_font + NUM_FONTS + 1),
			MUIA_List_Format, "P=\33c",
		End,
	End;
	FNT_OPT_PREVIEW = NewObject(	mcc_font->mcc_Class, NULL,
									ImageButtonFrame,
									MYATTR_FONT, appres.DisplayFonts[((cur_textflags & PSFONT_TEXT)?cur_ps_font+1:cur_latex_font + NUM_FONTS + 1)],
									MYATTR_FONTSIZE, 24,
									MUIA_InputMode, MUIV_InputMode_RelVerify,
									MUIA_ShortHelp, (char *)_(msg_CircleNoEndPt),						
									TAG_DONE);
	/* Misc panel gadgets */
	STR_OPT_PRINTBUF			= StringObject,
        StringFrame,
		MUIA_String_Accept, "0123456789",
        MUIA_String_Format, MUIV_String_Format_Right,
        MUIA_String_MaxLen , 32,
        MUIA_String_Integer, appres.printbuf,
    End;
	CHK_OPT_ALLOWNEGCOORD		= MakeCheckmark((UBYTE *)_(msg_AllowNegCoords), appres.allownegcoords);
	CHK_OPT_DEBUG				= MakeCheckmark((UBYTE *)_(msg_Debug), appres.DEBUG);
	CYC_OPT_GRIDUNIT 			= CycleObject,
		MUIA_Cycle_Entries, CYC_OPT_GRIDUNIT_strings,
		MUIA_Cycle_Active, appres.gridunit,
	End; 

    return( WindowObject,
        MUIA_Window_Title, "AmiFig : Options",
        MUIA_Window_ID, MAKEID('Z','F','P','R'),
        MUIA_Window_Screen, Scr,
        MUIA_Window_ScreenTitle, "AmiFIG",

        WindowContents,	VGroup,
            Child, RegisterGroup(prefsregtitles),
                /* Display panel */
                Child, VGroup,
                    Child, HGroup,
                        MUIA_Frame	   , MUIV_Frame_Group,
                        MUIA_FrameTitle, (UBYTE *)_(msg_PubScreen),
                        Child, CHK_OPT_USEPUBSCREEN, 
                        Child, STR_OPT_PUBSCREENNAME, 
                    End,
                    Child, POP_OPT_SCREENMODE,
                    Child, POP_OPT_SKIN,
                    Child, GroupObject,
                        MUIA_Group_Columns, 2,
                        Child, CHK_OPT_BACKDROP,    										Child, CHK_OPT_AUTOACTIVATE,
                        Child, CHK_OPT_TOPRULER,											Child, CHK_OPT_SIDERULER,
                        Child, CHK_OPT_AUTOSCROLLCANVAS,									Child, CHK_OPT_SOFTSCROLLCANVAS,
                        Child, CHK_OPT_BESTQUALITY,											Child, CHK_OPT_ONSCREENAA,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_FreehandRes), End, 	Child, NUM_OPT_FREEHANDRES,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_GradDSpread), End, 	Child, NUM_OPT_GRADSPREAD,
                        Child, CHK_OPT_SHOWLENGTHS,											Child, CHK_OPT_SHOWNUMS,
                    End,
                    Child, RectangleObject, End,
                End,
                /* Page panel */
                Child, VGroup,
                    Child, GroupObject,
                        MUIA_Group_Columns, 2,
                        MUIA_Frame	   , MUIV_Frame_Group,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Format), End,               Child, CYC_OPT_PFORMAT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Orientation), End,          Child, CYC_OPT_PORIENT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Width), End,                Child, TXT_OPT_PWIDTH = TextObject, MUIA_Text_Contents, page_width_str, End,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Height), End,               Child, TXT_OPT_PHEIGHT = TextObject, MUIA_Text_Contents, page_height_str, End,
                    End,
                    Child, GroupObject,
                        MUIA_Group_Columns, 2,
                        MUIA_Frame	   , MUIV_Frame_Group,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_DisplayUnit), End,		Child, CYC_OPT_UNIT,
						Child, TextObject, MUIA_Text_Contents, (char *)_(msg_GridUnit), End,			Child, CYC_OPT_GRIDUNIT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_UserScale), End, 			Child, NUM_OPT_USRSCALE,
                    End,
                    Child, RectangleObject, End,
                End,
               /* I/O panel */
                Child, VGroup,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_StdIO),
                        MUIA_Group_Columns, 2,
                        Child, CHK_OPT_WRITEBACKUP,													Child, CHK_OPT_AUTOSAVESETTINGS,
                        Child, CHK_OPT_AUTOLOADLIB,                                                 Child, CHK_OPT_CREATEICON,
                        Child, CHK_OPT_SAVEWHOLEPAL,												Child, RectangleObject, End,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_DefaultTool), End,		Child, STR_OPT_DEFTOOL,
                    End,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_F2Dexport),
                        MUIA_Group_Columns, 2,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Command), End,			Child, STR_OPT_F2DCMD,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_ScreenOption), End, 		Child, STR_OPT_F2DSCREEN,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_OutputOption), End, 		Child, STR_OPT_F2DOUTPUT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_InputOption), End, 		Child, STR_OPT_F2DINPUT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_OtherOptions), End, 		Child, STR_OPT_F2DOTHER,
                    End,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_R2VImport),
                        MUIA_Group_Columns, 2,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Command), End,	 		Child, STR_OPT_R2VCMD,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_ScreenOption), End, 		Child, STR_OPT_R2VSCREEN,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_OutputOption), End,		Child, STR_OPT_R2VOUTPUT,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_OtherOptions), End, 		Child, STR_OPT_R2VOTHER,
                    End,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_SVGImport),
                        MUIA_Group_Columns, 2,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_StepNb), End,				Child, NUM_OPT_SVGINTERP,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Scaling), End, 			Child, NUM_OPT_SVGSCALE,
                    End,
                    Child, RectangleObject, End,
                End,
                /* Misc panel */
                Child, VGroup,
                    Child, VGroup,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_Fonts),
                        Child, LST_OPT_FNT,
                        Child, FNT_OPT_PREVIEW,
                    End,
                    Child, GroupObject,
                        MUIA_Frame, MUIV_Frame_Group,
                        MUIA_FrameTitle, (char *)_(msg_Misc),
                        MUIA_Group_Columns, 2,
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_PrintBuffer), End,	Child, STR_OPT_PRINTBUF,
                        Child, RectangleObject, End,											Child, CHK_OPT_ALLOWNEGCOORD,
                        Child, RectangleObject, End,											Child, CHK_OPT_DEBUG,
                    End,
                End,
            End,
            Child, HGroup,
                MUIA_Frame, MUIV_Frame_Group,
                Child, BUT_OPT_OPEN   = MakeButton((UBYTE *)_(msg_Open),   '\0', (UBYTE *)_(msg_OpenPrefFile)),
                Child, RectangleObject, End,
                Child, BUT_OPT_SAVE   = MakeButton((UBYTE *)_(msg_Save),   '\0', (UBYTE *)_(msg_SavePrefFile)),
                Child, BUT_OPT_USE    = MakeButton((UBYTE *)_(msg_Use),    '\0', (UBYTE *)_(msg_UsePref)),
                Child, BUT_OPT_CANCEL = MakeButton((UBYTE *)_(msg_Cancel), '\0', (UBYTE *)_(msg_DiscardModifications)),
            End,
        End,
    End);
}

void z_prefs_methods(void)
{
    static struct Hook UpdatePrefsHook;
    UpdatePrefsHook.h_Entry = (HOOKFUNC)UpdatePrefs;
    static struct Hook OpenPrefsHook;
    OpenPrefsHook.h_Entry = (HOOKFUNC)OpenPrefs;
    static struct Hook ApplyPrefsHook;
    ApplyPrefsHook.h_Entry = (HOOKFUNC)ApplyPrefs;
    static struct Hook DisplayPageDimHook;
    DisplayPageDimHook.h_Entry = (HOOKFUNC)DisplayPageDim;
    static struct Hook SetFontPreviewHook;
    SetFontPreviewHook.h_Entry = (HOOKFUNC)SetFontPreview;
    static struct Hook SelectNewFontHook;
    SelectNewFontHook.h_Entry = (HOOKFUNC)SelectNewFont;

    /* Close window */
    DoMethod(WinPrefs, 			MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinPrefs, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod(BUT_OPT_CANCEL, 	MUIM_Notify, MUIA_Selected, FALSE, WinPrefs, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	
	/* Window opening notification => set all fields to current values */
    DoMethod(WinPrefs, 			MUIM_Notify, MUIA_Window_Open, TRUE, WinPrefs, 3, MUIM_CallHook   , (IPTR)&UpdatePrefsHook, 0L);

    DoMethod(BUT_OPT_OPEN, 	    MUIM_Notify, MUIA_Selected, FALSE, WinPrefs, 3, MUIM_CallHook   , (IPTR)&OpenPrefsHook, 0L);
    DoMethod(BUT_OPT_SAVE, 	    MUIM_Notify, MUIA_Selected, FALSE, WinPrefs, 3, MUIM_CallHook   , (IPTR)&ApplyPrefsHook, 1L);
    DoMethod(BUT_OPT_USE, 	    MUIM_Notify, MUIA_Selected, FALSE, WinPrefs, 3, MUIM_CallHook   , (IPTR)&ApplyPrefsHook, 0L);	
	
	/* Modification of page dimension display notification */
    DoMethod(CYC_OPT_PFORMAT, 	MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&DisplayPageDimHook, 0L);	
    DoMethod(CYC_OPT_PORIENT, 	MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&DisplayPageDimHook, 0L);	
    DoMethod(CYC_OPT_UNIT, 	    MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&DisplayPageDimHook, 0L);	
    DoMethod(NUM_OPT_USRSCALE, 	MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&DisplayPageDimHook, 0L);	
    DoMethod(CYC_OPT_GRIDUNIT, 	MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&DisplayPageDimHook, 0L);	
	
	/* Notifications for Font preference */
	DoMethod(LST_OPT_FNT, 		MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, WinPrefs, 3, MUIM_CallHook, (IPTR)&SetFontPreviewHook, MUIV_TriggerValue);
	DoMethod(FNT_OPT_PREVIEW,	MUIM_Notify, MUIA_Selected,	   FALSE,		   WinPrefs, 3, MUIM_CallHook, (IPTR)&SelectNewFontHook, 0L);
}

/*----------------------------------------------------------------------------*/
/*     UpdatePrefs                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, UpdatePrefs,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void UpdatePrefs(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	char 		 page_width_str[80], page_height_str[80], screenmode[256];
    
	if (appres.landscape)
	{
		make_dimension_string(paper_sizes[appres.papersize].height, page_width_str, FALSE);
		make_dimension_string(paper_sizes[appres.papersize].width, page_height_str, FALSE);
	}
	else
	{
		make_dimension_string(paper_sizes[appres.papersize].width, page_width_str, FALSE);
		make_dimension_string(paper_sizes[appres.papersize].height, page_height_str, FALSE);
	}
	
	/* Screen panel gadgets */
	set(CHK_OPT_USEPUBSCREEN, MUIA_Selected, appres.UsePubScreen);
	set(STR_OPT_PUBSCREENNAME, MUIA_String_Contents, appres.PubScreenName);
	set(CHK_OPT_BACKDROP, MUIA_Selected, appres.Backdrop);
    prefs_ScreenMode = appres.ScreenMode;
    prefs_ScreenWidth = appres.ScreenWidth;
    prefs_ScreenHeight = appres.ScreenHeight;
    prefs_ScreenDepth = appres.ScreenDepth;
    prefs_AutoScroll = appres.AutoScroll;
    GetScreenModeName(prefs_ScreenMode, prefs_ScreenWidth, prefs_ScreenHeight, prefs_ScreenDepth, prefs_AutoScroll, screenmode);
    set(POP_OPT_SCREENMODE, MUIA_String_Contents, screenmode);
	set(POP_OPT_SKIN, MUIA_String_Contents, appres.skin);
	
	/* Page panel gadgets */
	set(CYC_OPT_PFORMAT, MUIA_Cycle_Active, appres.papersize);
	set(CYC_OPT_PORIENT, MUIA_Cycle_Active, (appres.landscape?1L:0L));
	set(TXT_OPT_PWIDTH, MUIA_Text_Contents, page_width_str);
	set(TXT_OPT_PHEIGHT, MUIA_Text_Contents, page_height_str);

	/* Display panel gadgets */
	set(CYC_OPT_UNIT, MUIA_Cycle_Active, (appres.INCHES?0L:1L));
	set(CHK_OPT_AUTOACTIVATE, MUIA_Selected, appres.AutoActivate);
	set(CHK_OPT_TOPRULER, MUIA_Selected, appres.topruler_on);
	set(CHK_OPT_SIDERULER, MUIA_Selected, appres.sideruler_on);
	set(CHK_OPT_AUTOSCROLLCANVAS, MUIA_Selected, appres.autoScrollCanvas);
	set(CHK_OPT_SOFTSCROLLCANVAS, MUIA_Selected, appres.softScrollCanvas);
	set(CHK_OPT_BESTQUALITY, MUIA_Selected, appres.viewBestQuality);
	set(CHK_OPT_ONSCREENAA, MUIA_Selected, appres.OnScreenAA);
	set(NUM_OPT_USRSCALE, MUIA_Numeric_Value, round(appres.userscale * 100));
	set(CHK_OPT_SHOWLENGTHS, MUIA_Selected, appres.showlengths);
	set(CHK_OPT_SHOWNUMS, MUIA_Selected, appres.shownums);
	set(NUM_OPT_FREEHANDRES, MUIA_Numeric_Value, appres.freehand_resolution);
	set(NUM_OPT_GRADSPREAD,  MUIA_Numeric_Value, appres.gradient_ditherspread);

	/* Font setting panel */
	set(LST_OPT_FNT, MUIA_List_Active, 0);
	set(FNT_OPT_PREVIEW, MYATTR_FONT, appres.DisplayFonts[0]);

	/* I/O panel gadgets */
	set(CHK_OPT_WRITEBACKUP, MUIA_Selected, appres.WriteBackup);
	set(CHK_OPT_AUTOLOADLIB, MUIA_Selected, appres.AutoLoadLib);
    set(CHK_OPT_AUTOSAVESETTINGS, MUIA_Selected, appres.autosave);
	set(STR_OPT_DEFTOOL, MUIA_String_Contents, appres.DefTool);
	set(CHK_OPT_CREATEICON, MUIA_Selected, appres.CreateIcons);
	set(CHK_OPT_SAVEWHOLEPAL, MUIA_Selected, appres.save_whole_palette);
	set(STR_OPT_F2DCMD, MUIA_String_Contents, appres.f2d_cmd);
	set(STR_OPT_F2DSCREEN, MUIA_String_Contents, appres.f2d_screenopt);
	set(STR_OPT_F2DOUTPUT, MUIA_String_Contents, appres.f2d_outputopt);
	set(STR_OPT_F2DINPUT, MUIA_String_Contents, appres.f2d_inputopt);
	set(STR_OPT_F2DOTHER, MUIA_String_Contents, appres.f2d_otheropt);
	set(STR_OPT_R2VCMD, MUIA_String_Contents, appres.r2v_cmd);
	set(STR_OPT_R2VSCREEN, MUIA_String_Contents, appres.r2v_screenopt);
	set(STR_OPT_R2VOUTPUT, MUIA_String_Contents, appres.r2v_outputopt);
	set(STR_OPT_R2VOTHER, MUIA_String_Contents, appres.r2v_otheropt);
	set(NUM_OPT_SVGINTERP, MUIA_Numeric_Value, appres.SVG_InterSteps);
	set(NUM_OPT_SVGSCALE, MUIA_Numeric_Value, appres.SVG_Scale);

	/* Misc panel gadgets */
	set(STR_OPT_PRINTBUF, MUIA_String_Integer, appres.printbuf);
	set(CHK_OPT_ALLOWNEGCOORD, MUIA_Selected, appres.allownegcoords);
	set(CHK_OPT_DEBUG, MUIA_Selected, appres.DEBUG);
	set(CYC_OPT_GRIDUNIT, MUIA_Cycle_Active, appres.gridunit);

#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     OpenPrefs                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, OpenPrefs,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void OpenPrefs(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
    struct appresStruct *tmp_appres = NULL;
	char 		 page_width_str[80], page_height_str[80], screenmode[256];
    int i;
    
#ifdef __amigaos4__
    tmp_appres = (struct appresStruct*)AllocVecTags(sizeof(struct appresStruct), TAG_DONE);
#else
    tmp_appres = (struct appresStruct*)AllocVec(sizeof(struct appresStruct),MEMF_ANY);
#endif
    
    if (tmp_appres)
    {
        ReadSettings(TRUE, tmp_appres);
        
        if (tmp_appres->landscape)
        {
            make_dimension_string(paper_sizes[tmp_appres->papersize].height, page_width_str, FALSE);
            make_dimension_string(paper_sizes[tmp_appres->papersize].width, page_height_str, FALSE);
        }
        else
        {
            make_dimension_string(paper_sizes[tmp_appres->papersize].width, page_width_str, FALSE);
            make_dimension_string(paper_sizes[tmp_appres->papersize].height, page_height_str, FALSE);
        }
        
        /* Screen panel gadgets */
        set(CHK_OPT_USEPUBSCREEN, MUIA_Selected, tmp_appres->UsePubScreen);
        set(STR_OPT_PUBSCREENNAME, MUIA_String_Contents, tmp_appres->PubScreenName);
        set(CHK_OPT_BACKDROP, MUIA_Selected, tmp_appres->Backdrop);
        prefs_ScreenMode = tmp_appres->ScreenMode;
        prefs_ScreenWidth = tmp_appres->ScreenWidth;
        prefs_ScreenHeight = tmp_appres->ScreenHeight;
        prefs_ScreenDepth = tmp_appres->ScreenDepth;
        prefs_AutoScroll = tmp_appres->AutoScroll;
        GetScreenModeName(prefs_ScreenMode, prefs_ScreenWidth, prefs_ScreenHeight, prefs_ScreenDepth, prefs_AutoScroll, screenmode);
        set(POP_OPT_SCREENMODE, MUIA_String_Contents, screenmode);
        set(POP_OPT_SKIN, MUIA_String_Contents, tmp_appres->skin);
        
        /* Page panel gadgets */
        set(CYC_OPT_PFORMAT, MUIA_Cycle_Active, tmp_appres->papersize);
        set(CYC_OPT_PORIENT, MUIA_Cycle_Active, (tmp_appres->landscape?1L:0L));
        set(TXT_OPT_PWIDTH, MUIA_Text_Contents, page_width_str);
        set(TXT_OPT_PHEIGHT, MUIA_Text_Contents, page_height_str);

        /* Display panel gadgets */
        set(CYC_OPT_UNIT, MUIA_Cycle_Active, (tmp_appres->INCHES?0L:1L));
        set(CHK_OPT_AUTOACTIVATE, MUIA_Selected, tmp_appres->AutoActivate);
        set(CHK_OPT_TOPRULER, MUIA_Selected, tmp_appres->topruler_on);
        set(CHK_OPT_SIDERULER, MUIA_Selected, tmp_appres->sideruler_on);
        set(CHK_OPT_AUTOSCROLLCANVAS, MUIA_Selected, tmp_appres->autoScrollCanvas);
        set(CHK_OPT_SOFTSCROLLCANVAS, MUIA_Selected, tmp_appres->softScrollCanvas);
        set(CHK_OPT_BESTQUALITY, MUIA_Selected, tmp_appres->viewBestQuality);
        set(CHK_OPT_ONSCREENAA, MUIA_Selected, tmp_appres->OnScreenAA);
        set(NUM_OPT_USRSCALE, MUIA_Numeric_Value, round(tmp_appres->userscale * 100));
        set(CHK_OPT_SHOWLENGTHS, MUIA_Selected, tmp_appres->showlengths);
        set(CHK_OPT_SHOWNUMS, MUIA_Selected, tmp_appres->shownums);
        set(NUM_OPT_FREEHANDRES, MUIA_Numeric_Value, tmp_appres->freehand_resolution);
        set(NUM_OPT_GRADSPREAD,  MUIA_Numeric_Value, tmp_appres->gradient_ditherspread);
        
        /* Font setting panel => for font, update directly appres struct*/
        for (i=0; i<NUM_SYS_FONTS; i++)
        {
            strncpy(appres.DisplayFonts[i], tmp_appres->DisplayFonts[i],64);
        }
        

        /* I/O panel gadgets */
        set(CHK_OPT_WRITEBACKUP, MUIA_Selected, tmp_appres->WriteBackup);
        set(CHK_OPT_AUTOLOADLIB, MUIA_Selected, tmp_appres->AutoLoadLib);
        set(CHK_OPT_AUTOSAVESETTINGS, MUIA_Selected, tmp_appres->autosave);
        set(STR_OPT_DEFTOOL, MUIA_String_Contents, tmp_appres->DefTool);
        set(CHK_OPT_CREATEICON, MUIA_Selected, tmp_appres->CreateIcons);
		set(CHK_OPT_SAVEWHOLEPAL, MUIA_Selected, tmp_appres->save_whole_palette);
        set(STR_OPT_F2DCMD, MUIA_String_Contents, tmp_appres->f2d_cmd);
        set(STR_OPT_F2DSCREEN, MUIA_String_Contents, tmp_appres->f2d_screenopt);
        set(STR_OPT_F2DOUTPUT, MUIA_String_Contents, tmp_appres->f2d_outputopt);
        set(STR_OPT_F2DINPUT, MUIA_String_Contents, tmp_appres->f2d_inputopt);
        set(STR_OPT_F2DOTHER, MUIA_String_Contents, tmp_appres->f2d_otheropt);
        set(STR_OPT_R2VCMD, MUIA_String_Contents, tmp_appres->r2v_cmd);
        set(STR_OPT_R2VSCREEN, MUIA_String_Contents, tmp_appres->r2v_screenopt);
        set(STR_OPT_R2VOUTPUT, MUIA_String_Contents, tmp_appres->r2v_outputopt);
        set(STR_OPT_R2VOTHER, MUIA_String_Contents, tmp_appres->r2v_otheropt);
        set(NUM_OPT_SVGINTERP, MUIA_Numeric_Value, tmp_appres->SVG_InterSteps);
        set(NUM_OPT_SVGSCALE, MUIA_Numeric_Value, tmp_appres->SVG_Scale);

        /* Misc panel gadgets */
        set(STR_OPT_PRINTBUF, MUIA_String_Integer, tmp_appres->printbuf);
        set(CHK_OPT_ALLOWNEGCOORD, MUIA_Selected, tmp_appres->allownegcoords);
        set(CHK_OPT_DEBUG, MUIA_Selected, tmp_appres->DEBUG);
		set(CYC_OPT_GRIDUNIT, MUIA_Cycle_Active, tmp_appres->gridunit);

        FreeVec(tmp_appres);
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     UsePrefs                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, ApplyPrefs,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void ApplyPrefs(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    struct appresStruct *tmp_appres = NULL;
    int i;
    LONG tmp = 0;
    LONG ID, w, h1, d, as;
    STRPTR str = NULL;
    BOOL Reopenscreen = FALSE, SkinChange = FALSE;
	BOOL WindowOpenState[10] = {FALSE};
   
#ifdef __amigaos4__
    tmp_appres = (struct appresStruct*)AllocVecTags(sizeof(struct appresStruct), TAG_DONE);
#else
    tmp_appres = (struct appresStruct*)AllocVec(sizeof(struct appresStruct),MEMF_ANY);
#endif
    
    if (tmp_appres)
    {
         /* Screen panel gadgets */
        get(CHK_OPT_USEPUBSCREEN, MUIA_Selected, &tmp_appres->UsePubScreen);
        GET(STR_OPT_PUBSCREENNAME, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->PubScreenName, (char *)str, 48);
        get(CHK_OPT_BACKDROP, MUIA_Selected, &tmp_appres->Backdrop); 
        tmp_appres->ScreenMode    = prefs_ScreenMode;
        tmp_appres->ScreenWidth   = prefs_ScreenWidth;  
        tmp_appres->ScreenHeight  = prefs_ScreenHeight;   
        tmp_appres->ScreenDepth   = prefs_ScreenDepth;   
        tmp_appres->AutoScroll    = prefs_AutoScroll;   

        GET(POP_OPT_SKIN, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->skin, (char *)str, 32);
        
        /* Page panel gadgets */
        get(CYC_OPT_PFORMAT, MUIA_Cycle_Active, &tmp_appres->papersize);
        get(CYC_OPT_PORIENT, MUIA_Cycle_Active, &tmp);
        tmp_appres->landscape = ((tmp == 0)?FALSE:TRUE);

        /* Display panel gadgets */
        get(CYC_OPT_UNIT, MUIA_Cycle_Active, &tmp);
        tmp_appres->INCHES = ((tmp == 0)?TRUE:FALSE);
        get(CHK_OPT_AUTOACTIVATE, MUIA_Selected, &tmp_appres->AutoActivate);
        get(CHK_OPT_TOPRULER, MUIA_Selected, &tmp_appres->topruler_on);
        get(CHK_OPT_SIDERULER, MUIA_Selected, &tmp_appres->sideruler_on);
        get(CHK_OPT_AUTOSCROLLCANVAS, MUIA_Selected, &tmp_appres->autoScrollCanvas);
        get(CHK_OPT_SOFTSCROLLCANVAS, MUIA_Selected, &tmp_appres->softScrollCanvas);
        get(CHK_OPT_BESTQUALITY, MUIA_Selected, &tmp_appres->viewBestQuality);
        get(CHK_OPT_ONSCREENAA, MUIA_Selected, &tmp_appres->OnScreenAA);
        get(NUM_OPT_USRSCALE, MUIA_Numeric_Value, &tmp);
        tmp_appres->userscale = (float)tmp / 100.0;
        get(CHK_OPT_SHOWLENGTHS, MUIA_Selected, &tmp);
        tmp_appres->showlengths = (BOOL)tmp;
        get(CHK_OPT_SHOWNUMS, MUIA_Selected, &tmp);
        tmp_appres->shownums = (BOOL)tmp;
        get(NUM_OPT_FREEHANDRES, MUIA_Numeric_Value, &tmp_appres->freehand_resolution);
        get(NUM_OPT_GRADSPREAD, MUIA_Numeric_Value, &tmp_appres->gradient_ditherspread);

        /* Font setting panel => appres is updated directly, copy data to tmp_appres */
        for (i=0; i<NUM_SYS_FONTS; i++)
        {
            strncpy(tmp_appres->DisplayFonts[i], appres.DisplayFonts[i],64);
        }

        /* I/O panel gadgets */
        get(CHK_OPT_WRITEBACKUP, MUIA_Selected, &tmp_appres->WriteBackup);
        get(CHK_OPT_AUTOLOADLIB, MUIA_Selected, &tmp_appres->AutoLoadLib);
        get(CHK_OPT_AUTOSAVESETTINGS, MUIA_Selected, &tmp);
        tmp_appres->autosave = (BOOL)tmp;
        GET(STR_OPT_DEFTOOL, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->DefTool, (char *)str, FMSIZE);
        get(CHK_OPT_CREATEICON, MUIA_Selected, &tmp_appres->CreateIcons);
		get(CHK_OPT_SAVEWHOLEPAL, MUIA_Selected, &tmp_appres->save_whole_palette);
        GET(STR_OPT_F2DCMD, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->f2d_cmd, (char *)str, 256);
        GET(STR_OPT_F2DSCREEN, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->f2d_screenopt, (char *)str, 32);
        GET(STR_OPT_F2DOUTPUT, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->f2d_outputopt, (char *)str, 32);
        GET(STR_OPT_F2DINPUT, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->f2d_inputopt, (char *)str, 32);
        GET(STR_OPT_F2DOTHER, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->f2d_otheropt, (char *)str, 32);
        GET(STR_OPT_R2VCMD, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->r2v_cmd, (char *)str, 256);
        GET(STR_OPT_R2VSCREEN, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->r2v_screenopt, (char *)str, 32);
        GET(STR_OPT_R2VOUTPUT, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->r2v_outputopt, (char *)str, 32);
        GET(STR_OPT_R2VOTHER, MUIA_String_Contents, &str);
        strncpy((char *)tmp_appres->r2v_otheropt, (char *)str, 32);
        get(NUM_OPT_SVGINTERP, MUIA_Numeric_Value, &tmp_appres->SVG_InterSteps);
        get(NUM_OPT_SVGSCALE, MUIA_Numeric_Value, &tmp_appres->SVG_Scale);

        /* Misc panel gadgets */
        get(STR_OPT_PRINTBUF, MUIA_String_Integer, &tmp_appres->printbuf);
        get(CHK_OPT_ALLOWNEGCOORD, MUIA_Selected, &tmp);
        tmp_appres->allownegcoords = (BOOL)tmp;
        get(CHK_OPT_DEBUG, MUIA_Selected, &tmp);
        tmp_appres->DEBUG = (BOOL)tmp;
		get(CYC_OPT_GRIDUNIT, MUIA_Cycle_Active, &tmp_appres->gridunit);

        /* Copy also Canvas position to tmp_appres struct */
        tmp_appres->CanvasLeft   = appres.CanvasLeft;
        tmp_appres->CanvasTop    = appres.CanvasTop;
        tmp_appres->CanvasWidth  = appres.CanvasWidth;
        tmp_appres->CanvasHeight = appres.CanvasHeight;

        if (*Arg == 1)
        {
            /* Save settings */
            SaveSettings(TRUE, tmp_appres);
        }
        
        /* Check if screen shall be closed and reopenned */
        if (    (appres.UsePubScreen != tmp_appres->UsePubScreen)
           ||   ((tmp_appres->UsePubScreen != TRUE) && (tmp_appres->ScreenMode != appres.ScreenMode))
           ||   ((tmp_appres->UsePubScreen != TRUE) && (tmp_appres->Backdrop != appres.Backdrop)) )
        {
            Reopenscreen = TRUE;
        }
		
		if (strcmp(tmp_appres->skin, appres.skin) != 0)
		{
			SkinChange = TRUE;
			Reopenscreen = TRUE;
		}
		
        
        /* And apply settings */
        ApplySettings(tmp_appres);
        FreeVec(tmp_appres);

		/* Modify all picture icons if skin was modified */
		if (SkinChange)
		{
			Edit_ChangeSkin();
			Color_ChangeSkin();
			Mode_ChangeSkin();
		}

        if (Reopenscreen)
        {
			 /* Get open state of all MUI windows (not needed for prefs) */
			get(WinMode, 	MUIA_Window_Open, &WindowOpenState[0]);
			get(WinEdit, 	MUIA_Window_Open, &WindowOpenState[1]);
			get(WinColor, 	MUIA_Window_Open, &WindowOpenState[2]);
			get(WinZoom, 	MUIA_Window_Open, &WindowOpenState[3]);
			get(WinPict, 	MUIA_Window_Open, &WindowOpenState[4]);
			get(WinExport, 	MUIA_Window_Open, &WindowOpenState[5]);
			get(WinAbout, 	MUIA_Window_Open, &WindowOpenState[6]);
			get(WinPrint, 	MUIA_Window_Open, &WindowOpenState[7]);
			get(WinLib, 	MUIA_Window_Open, &WindowOpenState[8]);
			get(WinMsg, 	MUIA_Window_Open, &WindowOpenState[9]);
			 /* Close all MUI windows */
			set(WinMode, 	MUIA_Window_Open, FALSE);
			set(WinEdit, 	MUIA_Window_Open, FALSE);
			set(WinColor, 	MUIA_Window_Open, FALSE);
			set(WinPrefs, 	MUIA_Window_Open, FALSE);
			set(WinZoom, 	MUIA_Window_Open, FALSE);
			set(WinPict, 	MUIA_Window_Open, FALSE);
			set(WinExport, 	MUIA_Window_Open, FALSE);
			set(WinAbout, 	MUIA_Window_Open, FALSE);
			set(WinPrint, 	MUIA_Window_Open, FALSE);
			set(WinLib, 	MUIA_Window_Open, FALSE);
			set(WinMsg, 	MUIA_Window_Open, FALSE);
			/* apply new screen parameters */
			ReopenScreen();
			/* Set screen for windows */
			set(WinMode, 	MUIA_Window_Screen, Scr);
			set(WinEdit, 	MUIA_Window_Screen, Scr);
			set(WinColor, 	MUIA_Window_Screen, Scr);
			set(WinPrefs, 	MUIA_Window_Screen, Scr);
			set(WinZoom, 	MUIA_Window_Screen, Scr);
			set(WinPict, 	MUIA_Window_Screen, Scr);
			set(WinExport, 	MUIA_Window_Screen, Scr);
			set(WinAbout, 	MUIA_Window_Screen, Scr);
			set(WinPrint, 	MUIA_Window_Screen, Scr);
			set(WinLib, 	MUIA_Window_Screen, Scr);
			set(WinMsg, 	MUIA_Window_Screen, Scr);
			/* Reopen windows that were open (minus prefs window) */
			set(WinMode, 	MUIA_Window_Open, WindowOpenState[0]);
			set(WinEdit, 	MUIA_Window_Open, WindowOpenState[1]);
			set(WinColor, 	MUIA_Window_Open, WindowOpenState[2]);
			set(WinZoom, 	MUIA_Window_Open, WindowOpenState[3]);
			set(WinPict, 	MUIA_Window_Open, WindowOpenState[4]);
			set(WinExport, 	MUIA_Window_Open, WindowOpenState[5]);
			set(WinAbout, 	MUIA_Window_Open, WindowOpenState[6]);
			set(WinPrint, 	MUIA_Window_Open, WindowOpenState[7]);
			set(WinLib, 	MUIA_Window_Open, WindowOpenState[8]);
			set(WinMsg, 	MUIA_Window_Open, WindowOpenState[9]);			
        }
		else
		{
			/* Only close Prefs window */
			set(WinPrefs, MUIA_Window_Open, FALSE);
		}
		
		/* Refresh display */
		redisplay_canvas();
		z_mode_refresh();
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     ScreenModeStop                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, ScreenModeStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void ScreenModeStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    
    struct ScreenModeRequester	*mode_request = (struct ScreenModeRequester	*)Arg;
    char screenmode[256];
    
    prefs_ScreenMode   = mode_request->sm_DisplayID;
    prefs_ScreenWidth  = mode_request->sm_DisplayWidth;
    prefs_ScreenHeight = mode_request->sm_DisplayHeight;
    prefs_ScreenDepth  = mode_request->sm_DisplayDepth;
    prefs_AutoScroll   = (ULONG)mode_request->sm_AutoScroll;

    GetScreenModeName(prefs_ScreenMode, prefs_ScreenWidth, prefs_ScreenHeight, prefs_ScreenDepth, prefs_AutoScroll, screenmode);
    set(Object, MUIA_String_Contents, screenmode);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SkinStop                                                               */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SkinStop,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SkinStop(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    char skin[32];
    int i;
    
    struct FileRequester *file_requester = (struct FileRequester *)Arg;
    for (i=strlen((const char *)file_requester->fr_Drawer) ; ((file_requester->fr_Drawer[i] != '/') && (i>-1)); i--);
    i++;
    sprintf(skin, "%s", &file_requester->fr_Drawer[i]);
    set(Object, MUIA_String_Contents, skin);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DisplayPageDim                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DisplayPageDim,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DisplayPageDim(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	char 	page_width_str[80], page_height_str[80];
	float	sav_userscale;
	BOOL	sav_INCHES;
	int		sav_gridunit;
	LONG 	tmp = 0;
	BOOL	landscape = FALSE;
	int		papersize = 0;
	
	/* store current values */
	sav_userscale = appres.userscale;
	sav_INCHES    = appres.INCHES;
	sav_gridunit  = cur_gridunit;
	
	/* get tmp values */
	get(NUM_OPT_USRSCALE, MUIA_Numeric_Value, &tmp);
    appres.userscale = (float)tmp / 100.0;
	get(CYC_OPT_UNIT, MUIA_Cycle_Active, &tmp);
	appres.INCHES = ((tmp == 0)?TRUE:FALSE);
	get(CYC_OPT_GRIDUNIT, MUIA_Cycle_Active, &cur_gridunit);
	get(CYC_OPT_PORIENT, MUIA_Cycle_Active, &tmp);
	landscape = ((tmp == 0)?FALSE:TRUE);
	get(CYC_OPT_PFORMAT, MUIA_Cycle_Active, &papersize);
	
	if (landscape)
	{
		make_dimension_string(paper_sizes[papersize].height, page_width_str, FALSE);
		make_dimension_string(paper_sizes[papersize].width, page_height_str, FALSE);
	}
	else
	{
		make_dimension_string(paper_sizes[papersize].width, page_width_str, FALSE);
		make_dimension_string(paper_sizes[papersize].height, page_height_str, FALSE);
	}
    set(TXT_OPT_PWIDTH, MUIA_Text_Contents, page_width_str);
	set(TXT_OPT_PHEIGHT, MUIA_Text_Contents, page_height_str);
	
	/*revert to original values */
	appres.userscale = sav_userscale;
	appres.INCHES    = sav_INCHES;
	cur_gridunit     = sav_gridunit;
	
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SetFontPreview                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SetFontPreview,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetFontPreview(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	set(FNT_OPT_PREVIEW, MYATTR_FONT, appres.DisplayFonts[*Arg]);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SelectNewFont                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SelectNewFont,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SelectNewFont(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	struct FontRequester *font_request = NULL;
	LONG selfont = 0L;

	font_request=(struct FontRequester *)AllocAslRequestTags	(ASL_FontRequest,
																(ASLFO_Screen), 		Scr,
																(TAG_DONE));

	if (font_request != NULL)
	{
		if(AslRequestTags(font_request, (TAG_DONE)) != 0)
		{
			get(LST_OPT_FNT   , MUIA_List_Active  , &selfont);
			strncpy((char *)appres.DisplayFonts[selfont], (char *)font_request->fo_Attr.ta_Name,64);
			set(FNT_OPT_PREVIEW, MYATTR_FONT, appres.DisplayFonts[selfont]);
		}
		
		FreeAslRequest(font_request);
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
