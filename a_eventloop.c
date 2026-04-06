/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_eventloop.c $
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
#include <workbench/startup.h>
#ifndef __AROS__
#include <workbench/workbench.h>
#else
#include <proto/workbench.h>
#endif

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "a_coords.h"
#include "a_eventloop.h"
#include "a_rexxiface.h"
#include "d_arc.h"
#include "d_box.h"
#include "d_ellipse.h"
#include "d_line.h"
#include "d_picobj.h"
#include "d_spline.h"
#include "d_text.h"
#include "d_regpoly.h"
#include "d_arcbox.h"
#include "e_addpt.h"
#include "e_glue.h"
#include "e_movept.h"
#include "e_scale.h"
#include "e_delete.h"
#include "f_load.h"
#include "u_redraw.h"
#include "u_undo.h"
#include "w_msgpanel.h"
#include "w_canvas.h"
#include "z_fig.h"

extern void load_by_wbarg(struct WBArg *wbarg);

void clearFigure(void)
{
	stop_action(); // cancel current activity
	delete_all();
	put_msg((char *)_(msg_UndoFigureRestore));
	redisplay_canvas();
	figure_modified = 0;
}

void CloseAllFigs(void)
{
	int i = 0;
	BOOL ask = FALSE;

	stop_action();
	ask = figure_modified;
	while ((i < MAXWINDOWS) && !(ask))
	{
		if((figContexts[i].paintWnd != NULL) && (figContexts[i].figure_modified)) ask = TRUE;
		i++;
	}
	
	if ((ask) && (!doRequest((char *)_(msg_FigModCont))))
	{
		return;
	}

	for(i=0; i < MAXWINDOWS; i++)
	{
		if(figContexts[i].paintWnd != NULL)
		{
			restoreContext((APTR)figContexts[i].paintWnd);
			// Fenster schließen, ohne nochmal zu fragen, deshalb Bild hier löschen
			delete_all();
			put_msg((char *)_(msg_FigureDelete));
			clean_up();
			close_canvaswin(FALSE);
			deleteContext((APTR)figContexts[i].paintWnd);
		}
	}
}
                    
void HandleInput(void)
{
	struct AppMessage    *amsg;
	struct WBArg         *wbarg;
	extern struct 	MsgPort *appWinPort;
	ULONG	 		SignalSet, appsbit = 0, RexxSigBit = 0;
	int 			i;
    BOOL            running = TRUE;
    
	while(running)
	{
        if (DoMethod(App, MUIM_Application_Input, &SignalSet) == MUIV_Application_ReturnID_Quit)
		{
            CloseAllFigs();
		}

		/* Check if all windows were closed during MUI input (e.g. from the Quit menu) */
		if ((canvas_win == NULL) && ((restoreAnyContext()) == -1))
		{
			running = FALSE;
			break;
		}
			
		/* Add canvas window SigBit */
		for(i=0; i < MAXWINDOWS; i++)
		{
			if(figContexts[i].paintWnd != NULL)
			{
				SignalSet |= (1 << figContexts[i].paintWnd->UserPort->mp_SigBit);
			}
		}
		
		/* Add AppWindow SigBit */
		appsbit = 1L << appWinPort->mp_SigBit;
		SignalSet |= appsbit;

		/* Add Rexx SigBit */
        if (rexxport)
        {
		    RexxSigBit = 1L << getRexxSigBit();
		    SignalSet |= RexxSigBit;
        }
        else
        {
            RexxSigBit = 0;
        }
		
		/* Wait for Signals */
		SignalSet = Wait(SignalSet);
		
		/* Handle AppWindow Messages */
		if((appWinPort) && (SignalSet & appsbit))
		{
			/* Got an AppMessage */
			while((amsg = (struct AppMessage *) GetMsg(appWinPort)))
			{
				wbarg=(struct WBArg*)amsg->am_ArgList;
				for(i=0; i < (int)amsg->am_NumArgs; i++, wbarg++)
				{
					load_by_wbarg(wbarg);
					if(!figure_modified || doRequest((char *)_(msg_FigModCont)))
					{
						load_file(current_file);
						ActivateWindow(canvas_win);
					}
				}
			}
		}
		
		/* Handle Rexx Messages */
		if((rexxport) && (SignalSet & RexxSigBit))
		{		
			handleRexxMessage(SignalSet);
		}
		
		/* Handle canvas window messages */
		for(i=0; i < MAXWINDOWS; i++)
		{
			if ((figContexts[i].paintWnd) && (figContexts[i].paintWnd->UserPort) && (SignalSet & (1 << figContexts[i].paintWnd -> UserPort -> mp_SigBit)))
					process_win_paint_signals(i);
		}
		
		/* Has last window been closed? */
		if ((canvas_win == NULL) && ((restoreAnyContext()) == -1))	running = FALSE;
	}
}

void stop_action(void)
{
	if(action_on)
	{
		put_msg((char *)_(msg_ActionCancelled));
		switch(cur_mode)
		{
            case F_CIRCLE_BY_RAD:     		cancel_circlebyrad()       ; break;
            case F_CIRCLE_BY_DIA:     		cancel_circlebydia()       ; break;
            case F_ELLIPSE_BY_RAD:    		cancel_ellipsebyrad()      ; break;
            case F_ELLIPSE_BY_DIA:    		cancel_ellipsebydia()      ; break;
            case F_CLOSED_APPROX_SPLINE: 	cancel_line_drawing()      ; break;
            case F_APPROX_SPLINE:        	cancel_line_drawing()      ; break;
            case F_CLOSED_INTERP_SPLINE: 	cancel_line_drawing()      ; break;
            case F_INTERP_SPLINE:        	cancel_line_drawing()      ; break;
            case F_BOX:               		cancel_box()               ; break;
            case F_ARCBOX:             		cancel_arc_boxobject()     ; break;
            case F_POLYGON:           		cancel_line_drawing()      ; break;
            case F_POLYLINE:          		cancel_line_drawing()      ; break;
            case F_CIRCULAR_ARC:      		cancel_arc()               ; break;
            case F_GLUE:              		cancel_tag_region()        ; break;
            case F_SCALE:             		cancel_scale()             ; break;
            case F_ADD_POINT:         		cancel_line_pointadding()  ; break;
            case F_MOVE_POINT:        		cancel_move_point()        ; break;
            case F_REGPOLY:                 cancel_regpoly()           ; break;
            case F_DRAWOBJ:                 canvas_rightbut_proc(0,0,0); break;
            case F_COPY:	                canvas_rightbut_proc(0,0,0); break;
            case F_MOVE:              		canvas_rightbut_proc(0,0,0); break;
		}
	}
}

