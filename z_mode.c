/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_mode.c $
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
#include "u_undo.h"
#include "u_redraw.h"
#include "w_msgpanel.h"
#include "a_coords.h"
#include "z_fig.h"
#include "z_mode.h"
#include "mcc_imagedisp.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFP3(void, DoClick,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoToggle,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGridChange,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradChange,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoAAChange,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR, Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoModeMutex,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoClick(struct Hook *h, APTR Object, ULONG *Arg);
void DoToggle(struct Hook *h, APTR Object, ULONG *Arg);
void DoGridChange(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradChange(struct Hook *h, APTR Object, ULONG *Arg);
void DoAAChange(struct Hook *h, APTR Object, ULONG *Arg);
void DoModeMutex(struct Hook *h, APTR Object, ULONG *Arg);
#endif    
    

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    COORD;
APTR    BUT_UNDO;
APTR    BUT_UNCONSTRAINED, BUT_MOUNTHATTAN, BUT_MANHATTAN, BUT_MOUNTAIN, BUT_LATEXLINE, BUT_LATEXARROW;
APTR    BUT_BARROWS, BUT_FARROWS, BUT_FREEHAND;
APTR    GRP_MMODE, GRP_MODE, CYC_GRIDMODE, CYC_SNAPMODE;
APTR    CHK_DISPGRAD, CHK_DISPAA;
APTR    BUT_PLUS, BUT_MINUS;

int		displayed_gridunit = MM_UNIT;
BOOL    FreeHandBut = FALSE;

/*----------------------------------------------------------------------------*/
/*     MakeCheckmark1                                                         */
/*----------------------------------------------------------------------------*/

APTR MakeCheckmark1(UBYTE *label, LONG state)
{
    APTR CM;

    CM = GroupObject,                                  
        MUIA_Group_Horiz,    TRUE,                 
        MUIA_InputMode,      MUIV_InputMode_Toggle,
        MUIA_ShowSelState,   FALSE,                
        Child,               ImageObject,                 
            MUIA_Image_Spec,     MUII_RadioButton,    
            MUIA_ShowSelState,   FALSE,
            MUIA_Selected,       state,         
            MUIA_InnerLeft,      0,
            MUIA_InnerRight,     0,
            MUIA_InnerTop,       0,
            MUIA_InnerBottom,    0,     
            MUIA_Weight,         0,                 
        End,                                       
        Child,               TextObject,           
            MUIA_Text_Contents,  label,             
            MUIA_ShowSelState,   FALSE,             
        End,                                       
        MUIA_Weight,         0,                 
    End;

    return(CM);
}
        
/*----------------------------------------------------------------------------*/
/*     MakeModeWindow - creates the mode window                               */
/*----------------------------------------------------------------------------*/

APTR MakeModeWindow(void)
{
	APTR WHOLEMODEGROUP;

	/* Store grid unit used for display */
	displayed_gridunit = cur_gridunit;
	
    GRP_MODE = HGroup,
        MUIA_Weight, 0,
        Child, CYC_GRIDMODE = CycleObject,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (UBYTE *)_(msg_Grid),
            MUIA_Cycle_Entries, grid_name[cur_gridunit],
            MUIA_Cycle_Active, cur_gridmode,
        End,			
        Child, CYC_SNAPMODE = CycleObject,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (UBYTE *)_(msg_Snap),
            MUIA_Cycle_Entries, snap_name[cur_gridunit],
            MUIA_Cycle_Active, cur_pointposn,
        End,
    End;
	
    WHOLEMODEGROUP = VirtgroupObject,
        Child, HGroup,
            MUIA_Group_HorizSpacing, 0,
            Child, BUT_UNDO             = MakeImageButton((UBYTE *)"undo.png"            , 'U' , (UBYTE *)_(msg_Undo)),
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, BUT_UNCONSTRAINED    = MakeImageToggle((UBYTE *)"no_constrain.png"    , '\0', (UBYTE *)_(msg_Unconstrained)),
            Child, BUT_MOUNTHATTAN      = MakeImageToggle((UBYTE *)"mounthattan.png"     , '\0', (UBYTE *)_(msg_MountHattan)),
            Child, BUT_MANHATTAN        = MakeImageToggle((UBYTE *)"manhattan.png"       , '\0', (UBYTE *)_(msg_Manhattan)),
            Child, BUT_MOUNTAIN         = MakeImageToggle((UBYTE *)"mountain.png"        , '\0', (UBYTE *)_(msg_Mountain)),
            Child, BUT_LATEXLINE        = MakeImageToggle((UBYTE *)"latex_lines.png"     , '\0', (UBYTE *)_(msg_LatexLines)),
            Child, BUT_LATEXARROW       = MakeImageToggle((UBYTE *)"latex_arrows.png"    , '\0', (UBYTE *)_(msg_LatexArrows)),
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, BUT_BARROWS          = MakeImageToggle((UBYTE *)"arrow_start.png"     , '\0', (UBYTE *)_(msg_AutomaticStartArrow)),
            Child, BUT_FARROWS          = MakeImageToggle((UBYTE *)"arrow_end.png"       , '\0', (UBYTE *)_(msg_AutomaticEndArrow)),
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, BUT_FREEHAND         = MakeImageToggle((UBYTE *)"freehand.png"     , '\0', (UBYTE *)_(msg_FreeHandMode)),
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, BUT_PLUS             = MakeImageButton((UBYTE *)"zoom_in.png"         , '+' , (UBYTE *)_(msg_ZoomIn)),
            Child, BUT_MINUS            = MakeImageButton((UBYTE *)"zoom_out.png"        , '-' , (UBYTE *)_(msg_ZoomOut)),
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, GRP_MMODE = HGroup,
                Child, GRP_MODE,
            End,
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, VGroup,
                Child, CHK_DISPGRAD = MakeCheckmark1((UBYTE *)_(msg_Gradient), cur_DisplayGradients),
                Child, CHK_DISPAA   = MakeCheckmark1((UBYTE *)_(msg_AA), appres.OnScreenAA),
            End,
            Child, RectangleObject, MUIA_FixWidth, 8, End,
            Child, RectangleObject, End,
            Child, COORD = TextObject,  MUIA_Frame, MUIV_Frame_Text,
                                        MUIA_Font, MUIV_Font_Fixed, 
                                        MUIA_Text_SetVMax, FALSE, 
                                        MUIA_Text_SetMax, TRUE, 
                                        MUIA_Text_Contents, "\33rX 0000 00.00\nY 0000 00.00",
            End,
        End,
    End;

    return(WindowObject,
        MUIA_Window_Title, "AmiFig : Modes",
        MUIA_Window_ID, MAKEID('Z','F','M','O'),
        MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM,AmiFIGNM,0),
        MUIA_Window_Screen, Scr,
        MUIA_Window_ScreenTitle, "AmiFIG",
        MUIA_Window_TopEdge, MUIV_Window_TopEdge_Delta(0),
        MUIA_Window_LeftEdge, 336,
        MUIA_Window_UseBottomBorderScroller, TRUE,

        WindowContents,	ScrollgroupObject,
			MUIA_Scrollgroup_UseWinBorder, TRUE,
			MUIA_Scrollgroup_Contents, WHOLEMODEGROUP,
		End,
    End);
}

/* set this as global as it is used also when changing gridunit */
static struct Hook DoGridChangeHook;

void z_mode_methods(void)
{
    static struct Hook DoClickHook;
    DoClickHook.h_Entry = (HOOKFUNC)DoClick;
    static struct Hook DoToggleHook;
    DoToggleHook.h_Entry = (HOOKFUNC)DoToggle;
    static struct Hook DoModeMutexHook;
    DoModeMutexHook.h_Entry = (HOOKFUNC)DoModeMutex;
    static struct Hook DoGradChangeHook;
    DoGradChangeHook.h_Entry = (HOOKFUNC)DoGradChange;
    static struct Hook DoAAChangeHook;
    DoAAChangeHook.h_Entry = (HOOKFUNC)DoAAChange;
    DoGridChangeHook.h_Entry = (HOOKFUNC)DoGridChange;
    
    DoMethod(WinMode, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    DoMethod(BUT_UNDO         , MUIM_Notify, MUIA_Pressed, FALSE, App, 3, MUIM_CallHook, (IPTR)&DoClickHook, F_UNDO);
    
    DoMethod(BUT_UNCONSTRAINED, MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_UNCONSTRAINED);
    DoMethod(BUT_MOUNTHATTAN  , MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_MOUNTHATTAN);
    DoMethod(BUT_MANHATTAN    , MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_MANHATTAN);
    DoMethod(BUT_MOUNTAIN     , MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_MOUNTAIN);
    DoMethod(BUT_LATEXLINE    , MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_LATEXLINE);
    DoMethod(BUT_LATEXARROW   , MUIM_Notify, MUIA_Selected, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoModeMutexHook, L_LATEXARROW);
    DoMethod(BUT_UNCONSTRAINED, MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_MOUNTHATTAN  , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_MANHATTAN    , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_MOUNTAIN     , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_LATEXLINE    , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    DoMethod(BUT_LATEXARROW   , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);

    DoMethod(BUT_BARROWS      , MUIM_Notify, MUIA_Selected, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoToggleHook, L_BARROWS);
    DoMethod(BUT_FARROWS      , MUIM_Notify, MUIA_Selected, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoToggleHook, L_FARROWS);

    DoMethod(BUT_FREEHAND     , MUIM_Notify, MUIA_Selected, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &(FreeHandBut));
    
    DoMethod(CYC_GRIDMODE     , MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoGridChangeHook, MUIV_TriggerValue);
    DoMethod(CYC_SNAPMODE     , MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, App, 3, MUIM_WriteLong,MUIV_TriggerValue,&(cur_pointposn));
    DoMethod(CHK_DISPGRAD     , MUIM_Notify, MUIA_Selected,     MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoGradChangeHook, MUIV_TriggerValue);
    DoMethod(CHK_DISPAA       , MUIM_Notify, MUIA_Selected,     MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoAAChangeHook,   MUIV_TriggerValue);

    DoMethod(BUT_PLUS         , MUIM_Notify, MUIA_Pressed, FALSE, App, 3, MUIM_CallHook, (IPTR)&DoClickHook, F_PLUS);
    DoMethod(BUT_MINUS        , MUIM_Notify, MUIA_Pressed, FALSE, App, 3, MUIM_CallHook, (IPTR)&DoClickHook, F_MINUS);
	
	z_mode_refresh();
}

/*----------------------------------------------------------------------------*/
/*     DoClick                                                                */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoClick,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoClick(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
    
    switch(*Arg)
    {
        case F_UNDO:
            undo();
            break;
        case F_PLUS:
            zoom_display(0,0,1.1);
            break;
        case F_MINUS:
            zoom_display(0,0,0.9);
            break;
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoToggle                                                               */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFH3(void, DoToggle,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoToggle(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif

	switch (*Arg)
	{
        case L_FARROWS:
            autoforwardarrow_mode = 1 - autoforwardarrow_mode;
            if (autoforwardarrow_mode == 1) put_msg((char *)_(msg_AutoEndArrow));
            break;
        case L_BARROWS:
            autobackwardarrow_mode = 1 - autobackwardarrow_mode;
            if (autobackwardarrow_mode == 1) put_msg((char *)_(msg_AutoStartArrow));
            break;
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoModeMutex                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoModeMutex,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoModeMutex(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
    
	extern int	manhattan_mode;
	extern int	mountain_mode;
	extern int	latexline_mode;
	extern int	latexarrow_mode;

    manhattan_mode  = 0;
    mountain_mode   = 0;
    latexline_mode  = 0;
    latexarrow_mode = 0;
    
	switch (*Arg)
	{
        case L_UNCONSTRAINED :
            DoMethod(BUT_MOUNTHATTAN,    MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MANHATTAN,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTAIN,       MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXLINE,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXARROW,     MUIM_NoNotifySet, MUIA_Selected, FALSE);
            put_msg((char *)_(msg_Uncontraint));
            break;
        case L_MOUNTHATTAN :
            DoMethod(BUT_UNCONSTRAINED,  MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MANHATTAN,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTAIN,       MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXLINE,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXARROW,     MUIM_NoNotifySet, MUIA_Selected, FALSE);
            mountain_mode   = 1;
            manhattan_mode  = 1;
            put_msg((char *)_(msg_MountHattan));
            break;
        case L_MANHATTAN :
            DoMethod(BUT_UNCONSTRAINED,  MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTHATTAN,    MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTAIN,       MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXLINE,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXARROW,     MUIM_NoNotifySet, MUIA_Selected, FALSE);
            manhattan_mode  = 1;
            put_msg((char *)_(msg_Manhattan));
            break;
        case L_MOUNTAIN :
            DoMethod(BUT_UNCONSTRAINED,  MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTHATTAN,    MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MANHATTAN,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXLINE,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXARROW,     MUIM_NoNotifySet, MUIA_Selected, FALSE);
            mountain_mode   = 1;
            put_msg((char *)_(msg_Mountain));
            break;
        case L_LATEXLINE :
            DoMethod(BUT_UNCONSTRAINED,  MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTHATTAN,    MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MANHATTAN,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTAIN,       MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXARROW,     MUIM_NoNotifySet, MUIA_Selected, FALSE);
            latexline_mode  = 1;
            put_msg((char *)_(msg_LatexLine));
            break;
        case L_LATEXARROW :
            DoMethod(BUT_UNCONSTRAINED,  MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTHATTAN,    MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MANHATTAN,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_MOUNTAIN,       MUIM_NoNotifySet, MUIA_Selected, FALSE);
            DoMethod(BUT_LATEXLINE,      MUIM_NoNotifySet, MUIA_Selected, FALSE);
            latexarrow_mode = 1;
            put_msg((char *)_(msg_LatexArrow));
            break;
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGridChange                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGridChange,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoGridChange(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif	
	cur_gridmode = *Arg;
	redisplay_canvas();
	
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradChange                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradChange,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoGradChange(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
	cur_DisplayGradients = *Arg;
	redisplay_canvas();
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoAAChange                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoAAChange,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoAAChange(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif	
	appres.OnScreenAA = *Arg;
	redisplay_canvas();
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

void z_mode_refresh(void)
{
	extern int	manhattan_mode;
	extern int	mountain_mode;
	extern int	latexline_mode;
	extern int	latexarrow_mode;
    
    if (latexarrow_mode)
        DoMethod(BUT_LATEXARROW,    MUIM_Set, MUIA_Selected, TRUE);
    else if (latexline_mode)
        DoMethod(BUT_LATEXLINE,    MUIM_Set, MUIA_Selected, TRUE);
    else if (mountain_mode)
        if (manhattan_mode)
            DoMethod(BUT_MOUNTHATTAN,    MUIM_Set, MUIA_Selected, TRUE);
        else
            DoMethod(BUT_MOUNTAIN,    MUIM_Set, MUIA_Selected, TRUE);
    else if (manhattan_mode)
        DoMethod(BUT_MANHATTAN,    MUIM_Set, MUIA_Selected, TRUE);
    else
        DoMethod(BUT_UNCONSTRAINED,    MUIM_Set, MUIA_Selected, TRUE);

	if (autoforwardarrow_mode)
        DoMethod(BUT_FARROWS,    MUIM_NoNotifySet, MUIA_Selected, TRUE);
    if (autobackwardarrow_mode)
        DoMethod(BUT_BARROWS,    MUIM_NoNotifySet, MUIA_Selected, TRUE);

	if (displayed_gridunit != cur_gridunit)
	{
		/* 	
			Grid Unit has changed, we need to change the displayed 
			values in cycle gadgets for grid and snap
		*/
        if (DoMethod(GRP_MMODE, MUIM_Group_InitChange))
        {
			/* First remove the Group from the display */
			DoMethod(GRP_MMODE,OM_REMMEMBER,GRP_MODE);
			/* Now dispose Group object as we want to recreate it */
			MUI_DisposeObject(GRP_MODE);
			/* Recreate the Group object with new values */
			GRP_MODE = HGroup,
                MUIA_Weight, 0,
				Child, CYC_GRIDMODE = CycleObject,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, (UBYTE *)_(msg_Grid),
					MUIA_Cycle_Entries, grid_name[cur_gridunit],
					MUIA_Cycle_Active, cur_gridmode,
				End,			
				Child, CYC_SNAPMODE = CycleObject,
                    MUIA_Frame, MUIV_Frame_Group,
                    MUIA_FrameTitle, (UBYTE *)_(msg_Snap),
					MUIA_Cycle_Entries, snap_name[cur_gridunit],
					MUIA_Cycle_Active, cur_pointposn,
				End,
			End;
			/* Add back the group to the display */
			DoMethod(GRP_MMODE, OM_ADDMEMBER,GRP_MODE);
			DoMethod(GRP_MMODE, MUIM_Group_ExitChange);
            /* And re-install notification methods */
            DoMethod(CYC_GRIDMODE     , MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoGridChangeHook, MUIV_TriggerValue);
            DoMethod(CYC_SNAPMODE     , MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, App, 3, MUIM_WriteLong,MUIV_TriggerValue,&(cur_pointposn));
			
			/* store new displayed units */
			displayed_gridunit = cur_gridunit;
        }
	}
	else
	{
		/* we can directly update the currently selected grid and snap mode */
		DoMethod(CYC_GRIDMODE,    MUIM_NoNotifySet, MUIA_Cycle_Active, cur_gridmode); 
		DoMethod(CYC_SNAPMODE,    MUIM_NoNotifySet, MUIA_Cycle_Active, cur_pointposn); 
	}
}

void z_print_coords(int X, int Y)
{
	LONG CentiX, CentiY;
	UBYTE PositionBuffer[30];
	double res;

    if(appres.INCHES)
    {
        res=(double)PIX_PER_INCH;
    }
    else
    {
        res=(double)PIX_PER_CM;
    }

    CentiX = (LONG)((double)(X * 100) / res);
    CentiY = (LONG)((double)(Y * 100) / res);

    sprintf((char *)PositionBuffer,"\33rX %4d %2d.%02d\nY %4d %2d.%02d",SCALE_DOWN(X),CentiX / 100,CentiX % 100,SCALE_DOWN(Y),CentiY / 100,CentiY % 100);
    set(COORD,MUIA_Text_Contents,PositionBuffer);
}

void Mode_ChangeSkin(void)
{
    UBYTE iconfile[128];
    
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"undo.png");
    set(BUT_UNDO          , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"no_constrain.png");
    set(BUT_UNCONSTRAINED , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"mounthattan.png");
    set(BUT_MOUNTHATTAN   , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"manhattan.png");
    set(BUT_MANHATTAN     , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"mountain.png");
    set(BUT_MOUNTAIN      , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"latex_lines.png");
    set(BUT_LATEXLINE     , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"latex_arrows.png");
    set(BUT_LATEXARROW    , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"arrow_start.png");
    set(BUT_BARROWS       , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"arrow_end.png");
    set(BUT_FARROWS       , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"freehand.png");
    set(BUT_FREEHAND      , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"zoom_in.png");
    set(BUT_PLUS          , MYATTR_IMAGEFILE, iconfile);
	sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,"zoom_out.png");
    set(BUT_MINUS         , MYATTR_IMAGEFILE, iconfile);
}
