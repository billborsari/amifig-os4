/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_canvas.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
/*********************** IMPORTS ************************/

#include "fig.h"
#include "intui.h"
#include <proto/gadtools.h>
#include <proto/wb.h>
#if defined(__AROS__)
#include <devices/rawkeycodes.h>
#endif

#include "resources.h"
#include "object.h"
#include "mode.h"
#include "paintop.h"
#include "version.h"
#include "a_coords.h"
#include "w_canvas.h"
#include "e_compound.h"
#include "e_delete.h"
#include "w_msgpanel.h"
#include "u_undo.h"
#include "u_redraw.h"
#include "w_drawprim.h"
#include "w_rulers.h"
#include "w_grid.h"
#include "a_eventloop.h"
#include "w_cursor.h"
#include "z_zoom.h"
#include "z_mode.h"
#include "z_fig.h"

#define GADGETID(msg) ((msg->Class==IDCMP_GADGETUP) || (msg->Class==IDCMP_GADGETDOWN))?(((struct Gadget *)(msg->IAddress))->GadgetID):(-1)

struct Window    *canvas_win  = NULL;
struct MsgPort 	 *appWinPort  = NULL;
struct AppWindow *appWin      = NULL;

struct Gadget *horscroller=NULL, *vertscroller=NULL;

struct Rectangle paint_cliprect;
struct Region 	*clip_region	 = NULL;
struct Region 	*ref_clip_region = NULL;

struct RastPort				*rastport      = NULL;
struct TmpRas	 			tmpras;
struct AreaInfo	 			areainfo;
PLANEPTR 					tempraster     = NULL; 			/* for area fill functions */
int 						rastwidth=0,rastheight=0;		/* for area fill functions */
UBYTE 						polygondata[5*(MAXPOINTNUM+1)];	/* for low level graphic functions */

static int CreateGadgets(int context)
{
    static WORD img2which[] =
    {
        UPIMAGE,
        DOWNIMAGE,
        LEFTIMAGE,
        RIGHTIMAGE,
        SIZEIMAGE
    };
    IPTR imagew[NUM_IMAGES],imageh[NUM_IMAGES];
    WORD v_offset,h_offset, btop, i;
    struct 	DrawInfo *screen_drawinfo = NULL;
    int err = 0;

    screen_drawinfo = GetScreenDrawInfo(Scr);
    
    for(i = 0;i < NUM_IMAGES;i++)
    {
        figContexts[context].img[i] = NewObject( 0,(UBYTE *)SYSICLASS,SYSIA_DrawInfo,(IPTR)screen_drawinfo,
                                                SYSIA_Which,img2which[i],
                                                TAG_DONE);
        if (figContexts[context].img[i])
        {
            GetAttr(IA_Width,(Object *)figContexts[context].img[i],&imagew[i]);
            GetAttr(IA_Height,(Object *)figContexts[context].img[i],&imageh[i]);
        }
        else
        {
            err = 1;
        }
   }

   if (err == 0)
   {
        btop = Scr->WBorTop + screen_drawinfo->dri_Font->tf_YSize + 1;

        v_offset = imagew[IMG_DOWNARROW] / 4;
        h_offset = imageh[IMG_LEFTARROW] / 4;

        figContexts[context].firstgadget = figContexts[context].gad[GAD_HORIZSCROLL] = NewObject(0,(UBYTE *)PROPGCLASS,
            GA_Left,Scr->WBorLeft,
            GA_RelBottom,-imageh[IMG_LEFTARROW] + h_offset + 1,
            GA_RelWidth,-imagew[IMG_LEFTARROW] - imagew[IMG_RIGHTARROW] - imagew[IMG_SIZE] - Scr->WBorRight - 2,
            GA_Height,imageh[IMG_LEFTARROW] - (h_offset * 2),
            GA_ID,GAD_HORIZSCROLL,
            GA_BottomBorder,TRUE,
            GA_RelVerify,TRUE,
            GA_Immediate,TRUE,
            GA_GZZGadget,TRUE,
            PGA_NewLook,TRUE,
            PGA_Borderless,TRUE,
            PGA_Freedom,FREEHORIZ,
            TAG_DONE);
            horscroller = figContexts[context].gad[GAD_HORIZSCROLL];

        figContexts[context].gad[GAD_VERTSCROLL] = NewObject(0,(UBYTE *)PROPGCLASS,
            GA_Top,btop + 1,
            GA_RelRight,-imagew[IMG_DOWNARROW] + v_offset + 1,
            GA_Width,imagew[IMG_DOWNARROW] - v_offset * 2,
            GA_RelHeight,-imageh[IMG_DOWNARROW] - imageh[IMG_UPARROW] - imageh[IMG_SIZE] - btop -2,
            GA_ID,GAD_VERTSCROLL,
            GA_Previous,(IPTR)figContexts[context].gad[GAD_HORIZSCROLL],
            GA_RightBorder,TRUE,
            GA_RelVerify,TRUE,
            GA_Immediate,TRUE,
            GA_GZZGadget,TRUE,
            PGA_NewLook,TRUE,
            PGA_Borderless,TRUE,
            PGA_Freedom,FREEVERT,
            TAG_DONE);
        vertscroller = figContexts[context].gad[GAD_VERTSCROLL];

        figContexts[context].gad[GAD_UPARROW] = NewObject(0,(UBYTE *)BUTTONGCLASS,
            GA_Image,(IPTR)figContexts[context].img[IMG_UPARROW],
            GA_RelRight,-imagew[IMG_UPARROW] + 1,
            GA_RelBottom,-imageh[IMG_DOWNARROW] - imageh[IMG_UPARROW] - imageh[IMG_SIZE] + 1,
            GA_ID,GAD_UPARROW,
            GA_RightBorder,TRUE,
            GA_GZZGadget,TRUE,
            GA_Previous,(IPTR)figContexts[context].gad[GAD_VERTSCROLL],
            GA_Immediate,TRUE,
            TAG_DONE);

        figContexts[context].gad[GAD_DOWNARROW] = NewObject(0,(UBYTE *)BUTTONGCLASS,
            GA_Image,(IPTR)figContexts[context].img[IMG_DOWNARROW],
            GA_RelRight,-imagew[IMG_UPARROW] + 1,
            GA_RelBottom,-imageh[IMG_UPARROW] - imageh[IMG_SIZE] + 1,
            GA_ID,GAD_DOWNARROW,
            GA_RightBorder,TRUE,
            GA_GZZGadget,TRUE,
            GA_Previous,(IPTR)figContexts[context].gad[GAD_UPARROW],
            GA_Immediate,TRUE,
            TAG_DONE);

        figContexts[context].gad[GAD_RIGHTARROW] = NewObject(0,(UBYTE *)BUTTONGCLASS,
            GA_Image,(IPTR)figContexts[context].img[IMG_RIGHTARROW],
            GA_RelRight,-imagew[IMG_SIZE] - imagew[IMG_RIGHTARROW] + 1,
            GA_RelBottom,-imageh[IMG_RIGHTARROW] + 1,
            GA_ID,GAD_RIGHTARROW,
            GA_BottomBorder,TRUE,
            GA_GZZGadget,TRUE,
            GA_Previous,(IPTR)figContexts[context].gad[GAD_DOWNARROW],
            GA_Immediate,TRUE,
            TAG_DONE);

        figContexts[context].gad[GAD_LEFTARROW] = NewObject(0,(UBYTE *)BUTTONGCLASS,
            GA_Image,(IPTR)figContexts[context].img[IMG_LEFTARROW],
            GA_RelRight,-imagew[IMG_SIZE] - imagew[IMG_RIGHTARROW] - imagew[IMG_LEFTARROW] + 1,
            GA_RelBottom,-imageh[IMG_RIGHTARROW] + 1,
            GA_ID,GAD_LEFTARROW,
            GA_BottomBorder,TRUE,
            GA_GZZGadget,TRUE,
            GA_Previous,(IPTR)figContexts[context].gad[GAD_RIGHTARROW],
            GA_Immediate,TRUE,
            TAG_DONE);

        for(i = 0;i < NUM_GADGETS;i++)
        {
            if (!figContexts[context].gad[i]) err = 2;
        }
    }
    
    FreeScreenDrawInfo(Scr,screen_drawinfo);
    
    return(err);
}


int OpenCanvasWindow(int context)
{
	APTR VisualInfo = NULL;

    if (context != -1)
    {
        figContexts[context].zoomrect.MinX=80;
        figContexts[context].zoomrect.MinY=10;
        figContexts[context].zoomrect.MaxX=100;
        figContexts[context].zoomrect.MaxY=100;
        
        if (CreateGadgets(context) == 0)
        {
            if ( ! ( canvas_win = OpenWindowTags( NULL,
                                                  WA_Left,			appres.CanvasLeft,
                                                  WA_Top,			appres.CanvasTop,
                                                  WA_InnerWidth,	appres.CanvasWidth,
                                                  WA_InnerHeight,	appres.CanvasHeight,
                                                  WA_IDCMP,	IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_INTUITICKS|
                                                            IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|IDCMP_NEWSIZE|
#ifdef __amigaos4__
                                                            IDCMP_EXTENDEDMOUSE|
#endif
                                                            IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_MENUPICK|
                                                            IDCMP_CHANGEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_ACTIVEWINDOW,
                                                  WA_Flags,	WFLG_SIZEGADGET|WFLG_DRAGBAR|WFLG_DEPTHGADGET|
                                                            WFLG_CLOSEGADGET|WFLG_SIZEBRIGHT|WFLG_SIZEBBOTTOM|
                                                            WFLG_SMART_REFRESH|WFLG_REPORTMOUSE|WFLG_GIMMEZEROZERO|
                                                            WFLG_NEWLOOKMENUS|WFLG_NOCAREREFRESH,
                                                  WA_Title,	(UBYTE *)"AmiFig",
                                                  WA_Gadgets,(IPTR)figContexts[context].firstgadget,
                                                  WA_ScreenTitle,	"AmiFIG",
                                                  WA_CustomScreen,	Scr,
                                                  WA_MinWidth,	200,
                                                  WA_MinHeight,	100,
                                                  WA_MaxWidth,	-1,
                                                  WA_MaxHeight,	-1,
                                                  WA_Zoom, &figContexts[context].zoomrect,
                                                  TAG_DONE )))

            {
                printf("%s\n",(UBYTE *)_(msg_PaintWndErr));
                return( 4L );
            }
        }
        else
        {
            return(2L);
        }
    }
    else
    {
        UWORD   WTop;
        UWORD   WWidth;
        UWORD   WHeight;

        WTop = (Scr->WBorTop + (Scr->Font->ta_YSize + 1));
        WWidth 	= Scr->Width;
        WHeight	= Scr->Height - WTop;

        if ( ! ( canvas_win = OpenWindowTags( NULL,
                                              WA_Left,			0,
                                              WA_Top,			WTop,
                                              WA_InnerWidth,	WWidth,
                                              WA_InnerHeight,	WHeight,
                                              WA_IDCMP,			IDCMP_GADGETDOWN|IDCMP_INTUITICKS|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|
#ifdef __amigaos4__
                                                                IDCMP_EXTENDEDMOUSE|
#endif
                                                                IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_REFRESHWINDOW|IDCMP_MENUPICK,
                                              WA_Flags,			WFLG_SMART_REFRESH|WFLG_BACKDROP|WFLG_REPORTMOUSE|WFLG_GIMMEZEROZERO|
                                                                WFLG_BORDERLESS | WFLG_NEWLOOKMENUS,
                                              WA_ScreenTitle,	"AmiFIG",
                                              WA_CustomScreen,	Scr,
                                              TAG_DONE )))
        {
            printf("%s\n",(UBYTE *)_(msg_PaintWndErr));
            return( 4L );
        }
    }
    	
    if(ZFig_Menus)
    {
		if ((VisualInfo = GetVisualInfo( Scr, TAG_DONE )))
		{
			LayoutMenus(ZFig_Menus, VisualInfo, GTMN_NewLookMenus, TRUE, TAG_DONE);
			SetMenuStrip(canvas_win, ZFig_Menus);
			FreeVisualInfo( VisualInfo );
		}
	}
    
    GT_RefreshWindow( canvas_win, NULL );

    return( 0L );
}

int setup_canvaswin(int context)
{
	int err = 0;;

	if(appres.Backdrop)
	{
        err=OpenCanvasWindow(-1);
	}
	else
	{
        err=OpenCanvasWindow(context);
	}

    if (err == 0)
    {
        /* set up AppWindow */
        appWinPort = CreateMsgPort();
        appWin     = AddAppWindow(0, 0, canvas_win, appWinPort, TAG_DONE) ;

        rastport=canvas_win->RPort;
        SetRPAttrs(rastport, RPTAG_PenMode,TRUE, (RPTAG_BPen), 2, TAG_DONE);
        renewContext(canvas_win, context);

        if(SetTempRaster(canvas_win->Width, canvas_win->Height, TRUE) < 0)
        {
            err = 2;
        }
        else
        {
            canvas_win->RPort->AreaInfo=&areainfo;
            ClearRect( 0, toprul_height-1, canvas_win->GZZWidth-siderul_width, canvas_win->GZZHeight-toprul_height);
            redisplay_rulers(BOTH_RULER);
        }
    }

	return(err);
}


BOOL close_canvaswin(BOOL request)
{
	struct AppMessage *amsg;
    int context = -1, i;

	if(canvas_win)
	{
        /* retrieve corresponding context */
        for(i=0; i < MAXWINDOWS; i++)
        {
            if(figContexts[i].paintWnd == canvas_win)
            {
                context = i;
            }
        }

        if(request)
		{
			if(figure_modified)
			{
				if(!doRequest((char *)_(msg_FigModCont)))
					return(FALSE);
			}
				
			/* close all compounds being edited */
			close_all_compounds();
			delete_all();
			put_msg((char *)_(msg_FigureDelete));
			clean_up();
		}

		canvas_win->RPort->AreaInfo = NULL;
		canvas_win->RPort->TmpRas   = NULL;

		RemoveAppWindow(appWin);
        if (ZFig_Menus)
        {
            ClearMenuStrip( canvas_win );
        }
        CloseWindow( canvas_win );

       
        if (context != -1)
        {
            for(i = 0; i < NUM_GADGETS;i++)
            {
                if ((IPTR)figContexts[context].gad[i]) DisposeObject((Object *)figContexts[context].gad[i]);
            }
            for(i = 0; i < NUM_IMAGES;i++)
            {
                if ((IPTR)figContexts[context].img[i]) DisposeObject((Object *)figContexts[context].img[i]);
            }
        }

		while((amsg = (struct AppMessage *)GetMsg(appWinPort)))
			ReplyMsg((struct Message *)amsg) ;

		DeleteMsgPort(appWinPort) ;

		canvas_win = NULL;
	}

	return(TRUE);
}

void process_win_paint_signals(int i)
{	
	struct IntuiMessage	*Message;
	ULONG	 			class,Code;
    struct MenuItem     *item;
	ULONG				GadID;
#ifdef __amigaos4__
	LONG				WheelY = 0;
#endif
	int					X=0, Y=0;
	int					x,y;
	long				DeltaX=0, DeltaY=0;
	static ULONG 		current_gadget=NUM_GADGETS;
	static int			OldX = 0, OldY = 0;
	static BOOL			DoPan = FALSE;
    static BOOL         DoPanCrtl = FALSE;
	static CURSOR		old_cursor;
	static CURSOR		save_cursor=NULL;
	BOOL 				UpdateCanvasDisplay = FALSE;
	int					NewWidth, NewHeight;
	
	while((Message = (struct IntuiMessage *)GT_GetIMsg(figContexts[i].paintWnd -> UserPort)))
	{
		class		= Message -> Class;
		Code		= Message -> Code;
		GadID		= GADGETID(Message);
#ifdef __amigaos4__
		WheelY		= ((class==IDCMP_EXTENDEDMOUSE) && (Code == IMSGCODE_INTUIWHEELDATA))?(((struct IntuiWheelData *)(Message->IAddress))->WheelY):(0);
#endif
		GT_ReplyIMsg(Message);
		
		X = max2(0,figContexts[i].paintWnd->GZZMouseX);
		Y = max2(0,figContexts[i].paintWnd->GZZMouseY);
        
        shift = (PeekQualifier() & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT));

		x=SCALE_UP_X(X);
		y=SCALE_UP_Y(Y);

		if(cur_pointposn != P_ANYONE)
		{
			if(!action_on)
			{
				if( cur_mode != F_BREAK      && cur_mode != F_COPY              && cur_mode != F_MOVE &&
				    cur_mode != F_DELETE     && cur_mode != F_MOVE_POINT        && cur_mode != F_DELETE_POINT &&
				    cur_mode != F_ADD_POINT  && cur_mode != F_DELETE_ARROW_HEAD && cur_mode != F_ADD_ARROW_HEAD &&
				    cur_mode != F_FLIP       && cur_mode != F_ROTATE            && cur_mode != F_ALIGN && 
                    cur_mode != F_CONVERT    && cur_mode != F_UPDATE            && cur_mode != F_GET && 
                    cur_mode != F_SCALE)
						
					round_coords(&x, &y);
			}
			else
				round_coords(&x, &y);
		}

		switch(class)
		{
#ifdef __amigaos4__
			case IDCMP_EXTENDEDMOUSE:
				if(Code == IMSGCODE_INTUIWHEELDATA)
				{
					if(WheelY > 0) {
						zoom_display(X,Y,0.9);
					}

					if(WheelY < 0) {
						zoom_display(X,Y,1.1);
					}
				}
				break;
#endif
			case IDCMP_ACTIVEWINDOW:
				/* In all cases stop current action */
				stop_action();
				if (canvas_win != figContexts[i].paintWnd)
				{
					saveContext(canvas_win);
					restoreContext(figContexts[i].paintWnd);
					redisplay_rulers(BOTH_RULER);
					/* Update Display */
					z_mode_refresh();
				}
				break;
			case IDCMP_NEWSIZE:
				NewWidth = figContexts[i].paintWnd->GZZWidth;
				NewHeight = figContexts[i].paintWnd->GZZHeight;

				if (appres.CanvasWidth != NewWidth || appres.CanvasHeight != NewHeight)
				{
					appres.CanvasWidth  = NewWidth;
					appres.CanvasHeight = NewHeight;
                    figContexts[i].CanvasWidth  = NewWidth;
                    figContexts[i].CanvasHeight = NewHeight;
					update_scroller(FALSE);
					UpdateCanvasDisplay = TRUE;
                    if (SetTempRaster( appres.CanvasWidth, appres.CanvasHeight, FALSE) < 0)
                    {
                        put_msg((char *)_(msg_OutOfMem));
                    }
				}
				break;
			case IDCMP_CHANGEWINDOW:
				if(!appres.Backdrop)
				{
					appres.CanvasLeft = figContexts[i].paintWnd->LeftEdge;
					appres.CanvasTop  = figContexts[i].paintWnd->TopEdge;
                    figContexts[i].CanvasLeft = figContexts[i].paintWnd->LeftEdge;
                    figContexts[i].CanvasTop  = figContexts[i].paintWnd->TopEdge;                    
				}
				break;
			case IDCMP_GADGETUP:
				if(appres.softScrollCanvas)
				{
					if(current_gadget == GAD_HORIZSCROLL)	syncScrollHorizontal();
					if(current_gadget == GAD_VERTSCROLL)	syncScrollVertical();
				}
				else
				{
					if(current_gadget == GAD_HORIZSCROLL)
					{
						xshift=get_xshift();
						UpdateCanvasDisplay = TRUE;
					}

					if(current_gadget == GAD_VERTSCROLL)
					{
						yshift=get_yshift();
						UpdateCanvasDisplay = TRUE;
					}
				}
				current_gadget=NUM_GADGETS;
				break;
			case IDCMP_GADGETDOWN:
				current_gadget=GadID;
				switch(current_gadget)
				{
					case GAD_LEFTARROW:
						jumpHorizontal(-0.1F);
						break;
					case GAD_RIGHTARROW:
						jumpHorizontal(+0.1F);
						break;
					case GAD_UPARROW:
						jumpVertical(-0.1F);
						break;
					case GAD_DOWNARROW:
						jumpVertical(+0.1F);
						break;
                }
				break;
			case IDCMP_INTUITICKS:
				if(appres.softScrollCanvas)
				{
					if(current_gadget == GAD_HORIZSCROLL)	syncScrollHorizontal();
					if(current_gadget == GAD_VERTSCROLL)	syncScrollVertical();
				}
				break;
			case IDCMP_RAWKEY:
				switch(Code)
				{
					case CURSORLEFT:
						jumpHorizontal(-0.1F);
						break;
					case CURSORRIGHT:
						jumpHorizontal(+0.1F);
						break;
					case CURSORUP:
						jumpVertical(-0.1F);
						break;
					case CURSORDOWN:
						jumpVertical(+0.1F);
						break;
#ifndef __amigaos4__
					case RAWKEY_NM_WHEEL_UP: //wheel up
						zoom_display(X,Y,1.1);
						break;
					case RAWKEY_NM_WHEEL_DOWN: //wheel down
						zoom_display(X,Y,0.9);
						break;
					case RAWKEY_ESCAPE:
						stop_action();
						break;
#endif						
				}
				break;
			case IDCMP_MOUSEMOVE:
				if( Scr->MouseY < figContexts[i].paintWnd->TopEdge+figContexts[i].paintWnd->BorderTop ||
					Scr->MouseY > figContexts[i].paintWnd->TopEdge+figContexts[i].paintWnd->Height-figContexts[i].paintWnd->BorderBottom ||
					Scr->MouseX < figContexts[i].paintWnd->LeftEdge+figContexts[i].paintWnd->BorderLeft ||
					Scr->MouseX >= figContexts[i].paintWnd->LeftEdge+figContexts[i].paintWnd->Width-figContexts[i].paintWnd->BorderRight)
				{
					/* We're out of the drawing area */
					figContexts[i].paintWnd -> Flags &= ~WFLG_RMBTRAP;
					if (save_cursor == NULL)
					{
						save_cursor = cur_cursor;
						set_cursor(arrow_cursor);
					}
				}
				else
				{
					figContexts[i].paintWnd -> Flags |= WFLG_RMBTRAP;
					z_print_coords(x,y);
					if (DoPan)
					{
						long tmp;
						float pagex,pagey;
						
						if (DoPanCrtl == TRUE && (!(PeekQualifier() & IEQUALIFIER_CONTROL)))
						{
                            DoPanCrtl = FALSE;
							DoPan = FALSE;
							set_cursor(old_cursor);
							figContexts[i].cur_cursor = old_cursor;
						}
						else
						{	
							get_pagesizes(&pagex, &pagey);
							
                            tmp = CM_TO_PIX(pagey) - (canvas_win->GZZHeight-toprul_height);
                            if (tmp != 0)
                            {
                                DeltaY = 	(Y - OldY) * 0xFFFF	/ (tmp);		
                                tmp = (long)((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot - DeltaY;
                                tmp = min2(max2(tmp,0),0xFFFF);
                                ((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot = tmp;
                            }
                            
                            tmp = CM_TO_PIX(pagex) - (canvas_win->GZZWidth-siderul_width);
							if (tmp != 0)
                            {
                                DeltaX = 	(X - OldX) * 0xFFFF	/ (tmp);
                                tmp = (long)((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot - DeltaX;
                                tmp = min2(max2(tmp,0),0xFFFF);
                                ((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot = tmp;
                            }
							RefreshGList(horscroller,figContexts[i].paintWnd,0L,2);
							
							if(DeltaX != 0) syncScrollHorizontal();
							if(DeltaY != 0) syncScrollVertical();
							
							OldX = X;
							OldY = Y;
						}
					}
					else    
					{
						if(save_cursor != NULL)
						{
							if (cur_cursor != arrow_cursor) set_cursor(cur_cursor);
							else set_cursor(save_cursor);
							save_cursor = NULL;
						}
						if(!(action_on && cur_mode == F_SCALE))
						{
							follow_mouse(SCALE_DOWN_X(x), SCALE_DOWN_Y(y));
						}
						(*canvas_locmove_proc)(x, y);
					}
					z_dozoom();
				}
								
				if(action_on && appres.autoScrollCanvas)
				{
					/* Auto Scroll if pointer leaves the paint window */
					if(Scr->MouseX < figContexts[i].paintWnd->LeftEdge)
						jumpHorizontal(-0.1F);
					if(Scr->MouseX >= figContexts[i].paintWnd->LeftEdge+figContexts[i].paintWnd->Width-figContexts[i].paintWnd->BorderRight)
						jumpHorizontal(+0.1F);
					if(Scr->MouseY < figContexts[i].paintWnd->TopEdge+figContexts[i].paintWnd->BorderTop)
						jumpVertical(-0.1F);
					if(Scr->MouseY > figContexts[i].paintWnd->TopEdge+figContexts[i].paintWnd->Height-figContexts[i].paintWnd->BorderBottom)
						jumpVertical(+0.1F);
				}
				break;
			case IDCMP_MOUSEBUTTONS:
				if (	X <  figContexts[i].paintWnd->GZZWidth-siderul_width &&
						Y >= toprul_height &&
						Y <  figContexts[i].paintWnd->GZZHeight)
				{
					switch(Code)
					{
                        case SELECTDOWN:
                            if ((cur_mode != F_CHANGE_DEPTH) && (PeekQualifier() & IEQUALIFIER_CONTROL))
                            {
                                DoPanCrtl = TRUE;
                                DoPan = TRUE;
                                old_cursor = cur_cursor;
                                set_cursor(pan_cursor);
                                figContexts[i].cur_cursor = pan_cursor;
                                OldX = X;
                                OldY = Y;
                            }
                            else if (PeekQualifier() & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))
                            {
                                (*canvas_middlebut_proc)(x,y,shift);
                            }
                            else
                            {
                                (*canvas_leftbut_proc)(x,y,shift);
                            }
                            break;
                        case SELECTUP:
                            if ((FreeHandBut == TRUE) &&                         
                                ((cur_mode == F_CLOSED_APPROX_SPLINE) ||
                                 (cur_mode == F_APPROX_SPLINE       ) ||
                                 (cur_mode == F_CLOSED_INTERP_SPLINE) ||
                                 (cur_mode == F_INTERP_SPLINE       ) ||
                                 (cur_mode == F_POLYLINE            ) ||
                                 (cur_mode == F_POLYGON             )))

                            {
                                (*canvas_middlebut_proc)(x,y,shift);
                            }
                            else if (DoPan)
                            {
                                DoPanCrtl = FALSE;
                                DoPan = FALSE;
                                set_cursor(old_cursor);
                                figContexts[i].cur_cursor = old_cursor;
                            }
                            break;
                        case MENUDOWN:
                            (*canvas_rightbut_proc)(x,y,shift);
                            break;
                        case MIDDLEDOWN:
                            if (canvas_middlebut_proc == null_proc)
                            {
                                DoPan = TRUE;
                                old_cursor = cur_cursor;
                                set_cursor(pan_cursor);
                                figContexts[i].cur_cursor = pan_cursor;
                                OldX = X;
                                OldY = Y;
                            }
                            else
                            {
                                (*canvas_middlebut_proc)(x,y,shift);
                            }
                            break;
                        case MIDDLEUP:
                            if (DoPan)
                            {
                                DoPan = FALSE;
                                set_cursor(old_cursor);
                                figContexts[i].cur_cursor = old_cursor;
                            }
                            break;
					}
                    z_dozoom();
				}

				break;
			case IDCMP_CLOSEWINDOW:
				{
					struct FigContext *SavedContext = curContext;
					saveContext(canvas_win);
					restoreContext((APTR)figContexts[i].paintWnd);
					if(close_canvaswin(TRUE))
					{
						deleteContext((APTR)figContexts[i].paintWnd);
					}
					restoreContext(SavedContext);
					break;
				}
            case IDCMP_MENUPICK:
                while(Code != MENUNULL)
                {
                    if ((item = ItemAddress(ZFig_Menus, Code)))
                    {
                        DoMenuItem((IPTR)GTMENUITEM_USERDATA(item));
                        Code = item->NextSelect;
                    }
                    else
                    {
                        Code = MENUNULL;
                    }
                } /* while(Code != MENUNULL) */
                break;
		}
		
		if(figContexts[i].paintWnd == NULL) break; /* If Window has been closed */
	}
	
	if (figContexts[i].paintWnd && UpdateCanvasDisplay)
	{
		calc_max_rultextwidth(TRUE);
		redisplay_canvas();
	}
}

void ClearRect(WORD x, WORD y, WORD w, WORD h)
{
    SetDrMd(rastport, JAM1);
    SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[WHITE].RGB, TAG_DONE);
    RectFill(rastport, x, y, w, h);
}

/************************************
  paint regions clipping functions 
************************************/
void setup_paint_region(void)
{
	LockLayer(0, canvas_win->WLayer);
	paint_cliprect.MinX = 0;
	paint_cliprect.MaxX = canvas_win->GZZWidth-(siderul_width+1);
	paint_cliprect.MinY = toprul_height;
	paint_cliprect.MaxY = canvas_win->GZZHeight-1;
	OrRectRegion(clip_region, &paint_cliprect);
	InstallClipRegion(canvas_win->WLayer, clip_region);
	UnlockLayer(canvas_win->WLayer);
}

void delete_paint_region(void)
{
	LockLayer(0, canvas_win->WLayer);
	InstallClipRegion(canvas_win->WLayer, NULL);
	UnlockLayer(canvas_win->WLayer);
	ClearRectRegion(clip_region, &paint_cliprect);
}


