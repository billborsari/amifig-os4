/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_fig.c $
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

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "u_undo.h"
#include "a_print.h"
#include "u_redraw.h"
#include "f_load.h"
#include "f_save.h"
#include "a_eventloop.h"
#include "w_msgpanel.h"

#include "z_fig.h"
#include "z_mode.h"
#include "z_edit.h"
#include "z_color.h"
#include "z_prefs.h"
#include "z_pict.h"
#include "z_export.h"
#include "z_zoom.h"
#include "z_about.h"
#include "z_print.h"
#include "z_library.h"
#include "a_io.h"
#include "a_area.h"

#include "mcc_graddisp.h"
#include "mcc_fontdisp.h"
#include "mcc_zoomdisp.h"
#include "mcc_imagedisp.h"
#include "mcc_figdisp.h"
#include "mcc_arrowslider.h"
#include "mcc_colorswatch.h"


#include "menu.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

BOOL SaveAllFigs(void);

#ifdef __AROS__
AROS_UFP3(void, DoMenu,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoMenu(struct Hook *h, APTR Object, ULONG *Arg);
#endif

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/

/* MUI Application */
APTR App = NULL;
/* MUI Windows */
APTR WinMode = NULL, WinEdit = NULL, WinColor = NULL, WinPrefs = NULL, WinZoom = NULL;
APTR WinPict = NULL, WinExport = NULL, WinAbout = NULL;
APTR WinPrint = NULL, WinLib = NULL, WinMsg = NULL;
/* MUI Custom classes*/
struct MUI_CustomClass *mcc_grad = NULL;
struct MUI_CustomClass *mcc_font = NULL;
struct MUI_CustomClass *mcc_zoom = NULL;
struct MUI_CustomClass *mcc_imag = NULL;
struct MUI_CustomClass *mcc_fig  = NULL;
struct MUI_CustomClass *mcc_asl  = NULL;
struct MUI_CustomClass *mcc_csw  = NULL;

/*----------------------------------------------------------------------------*/
/*     MakeMUIApp - creates the user interface                                */
/*----------------------------------------------------------------------------*/
            
BOOL MakeMUIApp(void)
{
    BOOL    retvalue = FALSE;
    static struct Hook DoMenuHook;
    DoMenuHook.h_Entry = (HOOKFUNC)DoMenu;
	
	mcc_grad = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct GradData),GradDispatcher);
	mcc_font = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct FontData),FontDispatcher);
	mcc_zoom = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct ZoomData),ZoomDispatcher);
	mcc_imag = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct ImageData),ImageDispatcher);
	mcc_fig  = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct FigData),FigDispatcher);
	mcc_asl  = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Group,NULL,sizeof(struct ArrowSliderData),ArrowSliderDispatcher);
	mcc_csw  = MUI_CreateCustomClass(NULL,(ClassID)MUIC_Area,NULL,sizeof(struct ColorSwatchData),ColorSwatchDispatcher);
    
    
	if ((mcc_grad != NULL) && (mcc_font != NULL) && (mcc_zoom != NULL) && (mcc_imag != NULL) && (mcc_fig != NULL) && (mcc_asl != NULL) && (mcc_csw != NULL))
	{	
		App = ApplicationObject,
			MUIA_Application_Title,        "AmiFIG",
			MUIA_Application_Version,      "$VER: AmiFig " AMIFIG_VERSION "." AMIFIG_COMMIT " (" AMIFIG_VERDATE ")",
			MUIA_Application_Copyright,    "� 2010-2016 by Yannick Erb",
			MUIA_Application_Author,       "Yannick Erb",
			MUIA_Application_Description,  "AmiFIG conversion to Zune",
			MUIA_Application_Base,         "ZFIG",
			MUIA_Application_Menustrip,    MUI_MakeObject(MUIO_MenustripNM,AmiFIGNM,0),

			SubWindow, WinMode   = MakeModeWindow(),
			SubWindow, WinEdit   = MakeEditWindow(),
			SubWindow, WinColor  = MakeColorWindow(),
			SubWindow, WinPrefs  = MakePrefsWindow(),
			SubWindow, WinZoom   = MakeZoomWindow(),
			SubWindow, WinPict   = MakePictWindow(),
			SubWindow, WinExport = MakeExportWindow(),
			SubWindow, WinAbout	 = MakeAboutWindow(),
			SubWindow, WinPrint	 = MakePrintWindow(),
			SubWindow, WinLib	 = MakeLibraryWindow(),
			SubWindow, WinMsg	 = MakeMsgWindow(),
		End;
        
		if (App)
		{
			DoMethod(App, MUIM_Notify, MUIA_Application_MenuAction, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoMenuHook, MUIV_TriggerValue);
			z_mode_methods();
			z_edit_methods();
			z_color_methods();
			z_prefs_methods();
			z_zoom_methods();
			z_pict_methods();
			z_export_methods();
			z_about_methods();
			z_print_methods();
			z_library_methods();
			z_msg_methods();
			
			retvalue = TRUE;
			
			set(WinMode, MUIA_Window_Open, TRUE);
			set(WinEdit, MUIA_Window_Open, TRUE);
            
            /* Late refresh to ensure all swatches initialize correctly once attached to screen */
            extern void RefreshAllColorButtons(void);
            RefreshAllColorButtons();
		}
    } 
	
	if (retvalue == FALSE)
	{
		/* something went wrong, release whats allocated */
		CloseMUIApp();
	}
    return(retvalue);
}

void CloseMUIApp(void)
{
    if (WinMode != NULL) set(WinMode, MUIA_Window_Open, FALSE);
    if (WinEdit != NULL) set(WinEdit, MUIA_Window_Open, FALSE);
    if (WinMsg != NULL) set(WinMsg, MUIA_Window_Open, FALSE);
    if (App != NULL) MUI_DisposeObject(App);
	if (mcc_grad != NULL) MUI_DeleteCustomClass(mcc_grad);
	if (mcc_font != NULL) MUI_DeleteCustomClass(mcc_font);
	if (mcc_zoom != NULL) MUI_DeleteCustomClass(mcc_zoom);
	if (mcc_imag != NULL) MUI_DeleteCustomClass(mcc_imag);
	if (mcc_fig != NULL) MUI_DeleteCustomClass(mcc_fig);
	if (mcc_asl != NULL) MUI_DeleteCustomClass(mcc_asl);
	if (mcc_csw != NULL) MUI_DeleteCustomClass(mcc_csw);
	App 	  = NULL;
    WinMode   = NULL;
    WinEdit   = NULL;
    WinColor  = NULL;
    WinPrefs  = NULL;
    WinZoom   = NULL;
    WinPict   = NULL;
    WinExport = NULL;
    WinAbout  = NULL;
    WinPrint  = NULL;
    WinLib	  = NULL;
    WinMsg	  = NULL;
	mcc_grad  = NULL;
	mcc_font  = NULL;
	mcc_zoom  = NULL;
	mcc_imag  = NULL;
	mcc_fig   = NULL;
	mcc_asl   = NULL;
}

/*----------------------------------------------------------------------------*/
/*     DoMenu                                                                */
/*----------------------------------------------------------------------------*/

void DoMenuItem(IPTR item)
{
    switch(item)
    {
		case CLEAR :
			if(figure_modified)
				if(!doRequest((char *)_(msg_FigModCont)))
					break;
			clearFigure();
			break;
		case NEW :
			CreateNewContext();
			break;
		case OPEN :
			if(figure_modified)
				if(!doRequest((char *)_(msg_FigModCont)))
					break;
			if(DoFileRequest((UBYTE *)_(msg_OpenFile),(UBYTE *)TempName,FR_FIGDATA | FR_DOLOAD))
				load_file(TempName);
			break;
		case INSERT :
			if(DoFileRequest((UBYTE *)_(msg_InsertFile),(UBYTE *)TempName,FR_FIGDATA | FR_DOLOAD))
				merge_file(TempName);
			break;
		case IMPPOTRACE :
            ImportR2V();
			break;
		case IMPSVG :
            ImportSVG();
			break;
		case SAVE :
			if(emptyfigure())
				put_msg((char *)_(msg_NothingToSave));
			else
				save_file(current_file);
			break;
		case SAVEAS :
			if(emptyfigure())
				put_msg((char *)_(msg_NothingToSave));
			else
				if(DoFileRequest((UBYTE *)_(msg_SaveFile),(UBYTE *)TempName,FR_FIGDATA | FR_DOSAVE))
					save_file(TempName);
			break;
		case EXPORTPNG :
			if(emptyfigure())
				put_msg((char *)_(msg_NothingToSave));
			else
			{
				set(WinExport, MUIA_Window_Open, TRUE);
				selectarea_selected();
			}
			break;
		case EXPORTF2D :
			if(emptyfigure())
				put_msg((char *)_(msg_NothingToSave));
			else
			{
				ExportF2D();
			}
			break;
		case PRINT :
			set(WinPrint, MUIA_Window_Open, TRUE);
			selectarea_selected();
			break;
		case PRINTPS :
			PrintToPS();
			break;
		case STATUS :
			status_message();
			break;
		case ABOUT :
			set(WinAbout, MUIA_Window_Open, TRUE);
            set(WinAbout, MUIA_Window_Activate, TRUE);
			break;
		case SAVEQUIT :
			if (SaveAllFigs())
			{
				CloseAllFigs();
			}
			break;
		case QUIT :
			CloseAllFigs();
			break;
		case TOGGLEZOOM :
			set(WinZoom, MUIA_Window_Open, TRUE);
            set(WinZoom, MUIA_Window_Activate, TRUE);
			break;
		case TOGGLECOLOR :
			set(WinColor, MUIA_Window_Open, TRUE);
            set(WinColor, MUIA_Window_Activate, TRUE);
            break;
		case TOGGLELIB :
			set(WinLib, MUIA_Window_Open, TRUE);
            set(WinLib, MUIA_Window_Activate, TRUE);
			break;
		case TOGGLEHELP :
			set(WinMsg, MUIA_Window_Open, TRUE);
            set(WinMsg, MUIA_Window_Activate, TRUE);
			break;
		case UNDO :
			undo();
			break;
		case REDISP :
			redisplay_canvas();
			break;
		case EDITPREFS :  
			set(WinPrefs, MUIA_Window_Open, TRUE);
            set(WinPrefs, MUIA_Window_Activate, TRUE);
			break;
    }
}

#ifdef __AROS__
AROS_UFH3(void, DoMenu,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
    
    DoMenuItem(*Arg);
    
    AROS_USERFUNC_EXIT
}
#else
void DoMenu(struct Hook *h, APTR Object, ULONG *Arg)
{
		DoMenuItem(*Arg);
}
#endif

/*----------------------------------------------------------------------------*/
/*     MakeCheckmark                                                          */
/*----------------------------------------------------------------------------*/

APTR MakeCheckmark(UBYTE *label, LONG state)
{
    APTR CM;

    CM = GroupObject,                                  
        MUIA_Group_Horiz,    TRUE,                 
        MUIA_InputMode,      MUIV_InputMode_Toggle,
        MUIA_ShowSelState,   FALSE,                
        Child,               ImageObject,          
            ImageButtonFrame,                       
            MUIA_Image_Spec,     MUII_CheckMark,    
            MUIA_Background,     MUII_ButtonBack,   
            MUIA_Image_FreeVert, TRUE,              
            MUIA_ShowSelState,   FALSE,
            MUIA_Selected,       state,         
            MUIA_Weight,         0,                 
        End,                                       
        Child,               TextObject,           
            MUIA_Text_Contents,  label,             
            MUIA_ShowSelState,   FALSE,             
        End,                                       
    End;

    return(CM);
}

/*----------------------------------------------------------------------------*/
/*     MakeButton                                                             */
/*----------------------------------------------------------------------------*/

APTR MakeButton(UBYTE *Label, UBYTE Key, UBYTE *Help)
{
    return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
                                    MUIA_Text_PreParse, "\33c",
                                    MUIA_ControlChar, Key,
                                    MUIA_InputMode, MUIV_InputMode_RelVerify,
                                    MUIA_Background, MUII_ButtonBack,
                                    End);
}

/*----------------------------------------------------------------------------*/
/*     MakeToggle                                                             */
/*----------------------------------------------------------------------------*/

APTR MakeToggle(UBYTE *Label, UBYTE Key, UBYTE *Help)
{
    return(TextObject, ButtonFrame, MUIA_Text_Contents, Label,
                                    MUIA_Text_PreParse, "\33c",
                                    MUIA_ControlChar, Key,
                                    MUIA_InputMode, MUIV_InputMode_Toggle,
                                    MUIA_Background, MUII_ButtonBack,
                                    End);
}

/*----------------------------------------------------------------------------*/
/*     MakeColorToggle                                                        */
/*----------------------------------------------------------------------------*/

APTR MakeColorButton(ULONG rgb, LONG W, LONG H)
{
    APTR  ColorToggle;

    /* Use custom ColorSwatch class that renders via RPTAG_APenColor
       (direct truecolor) instead of ColorfieldObject which allocates
       pens from the screen colormap and runs out with many instances. */
    ColorToggle = NewObject(mcc_csw->mcc_Class, NULL,
        MUIA_Frame,            MUIV_Frame_ImageButton,
        MUIA_InputMode,        MUIV_InputMode_Toggle,
        MUIA_ColorSwatch_RGB,  (rgb & 0x00FFFFFF),
        MUIA_FixWidth,         W,
        MUIA_FixHeight,        H,
        MUIA_InnerLeft,        0,
        MUIA_InnerRight,       0,
        MUIA_InnerTop,         0,
        MUIA_InnerBottom,      0,
        TAG_DONE);

    return (ColorToggle);
}


/*----------------------------------------------------------------------------*/
/*     MakeImageButton                                                        */
/*----------------------------------------------------------------------------*/

APTR MakeImageButton(UBYTE *pictfile, UBYTE Key, UBYTE *Help)
{
    UBYTE iconfile[128];
    
    sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,pictfile);
    return( NewObject(	mcc_imag->mcc_Class, NULL,
                        MYATTR_IMAGEFILE, iconfile,
                        MUIA_InputMode, MUIV_InputMode_RelVerify,
                        MUIA_ControlChar, Key,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_ShortHelp, Help,
                        MUIA_InnerLeft, 0,
                        MUIA_InnerRight, 0,
                        MUIA_InnerTop, 0,
                        MUIA_InnerBottom, 0,
                        ImageButtonFrame,
                        TAG_DONE));
}

/*----------------------------------------------------------------------------*/
/*     MakeImageToggle                                                        */
/*----------------------------------------------------------------------------*/

APTR MakeImageToggle(UBYTE *pictfile, UBYTE Key, UBYTE *Help)
{
    UBYTE iconfile[128];
    
    sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,pictfile);
    return( NewObject(	mcc_imag->mcc_Class, NULL,
                        MYATTR_IMAGEFILE, iconfile,
                        MUIA_InputMode, MUIV_InputMode_Toggle,
                        MUIA_ControlChar, Key,
                        MUIA_Background, MUII_ButtonBack,
                        MUIA_ShortHelp, Help,
                        MUIA_InnerLeft, 0,
                        MUIA_InnerRight, 0,
                        MUIA_InnerTop, 0,
                        MUIA_InnerBottom, 0,
                        ImageButtonFrame,
                        TAG_DONE));
}

BOOL SaveAllFigs(void)
{
	int  i;
	BOOL quitok = TRUE;

	saveContext((APTR)canvas_win);
	for(i = 0; i < MAXWINDOWS; i++)
	{
		if(figContexts[i].paintWnd != NULL)
		{
			restoreContext((APTR)figContexts[i].paintWnd);
			ActivateWindow(canvas_win);
			WindowToFront(canvas_win);

			if(DoFileRequest((UBYTE *)"Save & Quit...",(UBYTE *)TempName,FR_FIGDATA | FR_DOSAVE))
			{
				if(write_file((char *)TempName))
				{
					quitok=FALSE;
				}
            }
		}
	}
	
	return(quitok);
}

/*----------------------------------------------------------------------------*/
/*     GetStringFromUser - Simple string requester (Modal)                    */
/*----------------------------------------------------------------------------*/
ULONG GetStringFromUser(STRPTR buffer, ULONG maxlen, STRPTR title)
{
    APTR WinReq, StrReq, ButOk, ButCancel;
    ULONG sigs = 0, id;
    BOOL running = TRUE;
    ULONG result = 0;
    
    if (!buffer) return 0;
    
    WinReq = WindowObject,
        MUIA_Window_Title, title,
        MUIA_Window_RefWindow, WinMode,
        MUIA_Window_ID, MAKEID('S','T','R','Q'),
        
        WindowContents, VGroup,
            Child, StrReq = StringObject,
                MUIA_String_Contents, (IPTR)buffer,
                MUIA_String_MaxLen, maxlen,
                MUIA_Frame, MUIV_Frame_String,
            End,
            Child, HGroup,
                Child, ButOk = SimpleButton("_Ok"),
                Child, ButCancel = SimpleButton("_Cancel"),
            End,
        End,
    End;

    if (!WinReq) return 0;
    
    DoMethod(WinReq, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
             App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
             
    DoMethod(ButOk, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, 1001);
             
    DoMethod(ButCancel, MUIM_Notify, MUIA_Pressed, FALSE,
             App, 2, MUIM_Application_ReturnID, 1002);
             
    DoMethod(StrReq, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
             App, 2, MUIM_Application_ReturnID, 1001);

    DoMethod(App, OM_ADDMEMBER, WinReq);
    set(WinReq, MUIA_Window_Open, TRUE);
    set(WinReq, MUIA_Window_ActiveObject, StrReq);

    while(running)
    {
        id = DoMethod(App, MUIM_Application_Input, &sigs);
        
        if (id == MUIV_Application_ReturnID_Quit || id == 1002)
        {
            running = FALSE;
            result = 0;
        }
        else if (id == 1001)
        {
            IPTR temp_val = 0;
            get(StrReq, MUIA_String_Contents, &temp_val);
            if (temp_val) {
                if (buffer != (char *)temp_val) {
                    strncpy(buffer, (char *)temp_val, maxlen-1);
                }
                buffer[maxlen-1] = '\0';
				printf("buffer inside z_fig: %s\n", buffer);
            }
            running = FALSE;
            result = 1;
        }
        
        if (running && sigs)
        {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if (sigs & SIGBREAKF_CTRL_C) running = FALSE;
        }
    }
    
    set(WinReq, MUIA_Window_Open, FALSE);
    DoMethod(App, OM_REMMEMBER, WinReq);
    MUI_DisposeObject(WinReq);
    
    return result;
}

/*----------------------------------------------------------------------------*/
/*     GetStringFromUser - Simple string requester (Modal)                    */
/*----------------------------------------------------------------------------*/
