/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_zoom.c $
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

#include "intui.h"
#include "z_fig.h"
#include "mcc_zoomdisp.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    CYC_ZOOMFACTOR, ZOO_DISPLAY;
        
/*----------------------------------------------------------------------------*/
/*     MakeZoomWindow - creates the zoom window                               */
/*----------------------------------------------------------------------------*/

APTR MakeZoomWindow(void)
{
	static char *CYC_ZOOMFACTOR_strings[] = {"200%", "400%", "800%", "1600%", NULL };
	
    return(WindowObject,
            MUIA_Window_Title, "AmiFig : Zoom",
            MUIA_Window_ID, MAKEID('Z','F','Z','O'),
			MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",

            WindowContents,	VGroup,
				Child, HGroup,
					MUIA_Frame, MUIV_Frame_Group,
					Child, RectangleObject, End,
					Child, TextObject, MUIA_Text_Contents , (char *)_(msg_ZoomFactor), End,
					Child, CYC_ZOOMFACTOR = CycleObject,
                           MUIA_Cycle_Entries, CYC_ZOOMFACTOR_strings,
						   MUIA_Cycle_Active, 1,
					End,
				End,
				Child, ZOO_DISPLAY = NewObject(	mcc_zoom->mcc_Class, NULL,
												MUIA_Background	, MUII_BACKGROUND,
												MYATTR_SRCWIN	, canvas_win,
												MYATTR_SRCX 	, 0, 
												MYATTR_SRCY 	, 0,
												MYATTR_RATIO	, 400,
												TAG_DONE),
            End,
        End);
}

void z_zoom_methods(void)
{
    /* Close window */
    DoMethod(WinZoom, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        WinZoom, 3, MUIM_Set, MUIA_Window_Open, FALSE);
}

void z_dozoom(void)
{
    ULONG ZoomOpen = FALSE;
    LONG  ratio = 400;
        
    get(WinZoom, MUIA_Window_Open, &ZoomOpen);
    
    if (ZoomOpen)
    {
        get(CYC_ZOOMFACTOR, MUIA_Cycle_Active, &ratio);
        ratio = (ratio + 1) * 200;
    
        set(ZOO_DISPLAY, MYATTR_SRCWIN, canvas_win);
        set(ZOO_DISPLAY, MYATTR_RATIO, ratio);
        set(ZOO_DISPLAY, MYATTR_SRCX, canvas_win->GZZMouseX);
        set(ZOO_DISPLAY, MYATTR_SRCY, canvas_win->GZZMouseY);
        MUI_Redraw(ZOO_DISPLAY,MADF_DRAWOBJECT);
    }
}
