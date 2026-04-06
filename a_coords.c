/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_coords.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "a_coords.h"
#include "intui.h"
#include "w_canvas.h"
#include "u_redraw.h"
#include "w_rulers.h"

int xshift=0, yshift=0;
int last_xshift=0, last_yshift=0;

/* returns page size in cm */
void get_pagesizes(float *pagex, float *pagey)
{
	float mul=PIX_PER_INCH/2.54;

	if(appres.landscape)
	{
		*pagex=(float)paper_sizes[appres.papersize].height/mul;
		*pagey=(float)paper_sizes[appres.papersize].width/mul;
	}
	else
	{
		*pagex=(float)paper_sizes[appres.papersize].width/mul;
		*pagey=(float)paper_sizes[appres.papersize].height/mul;
	}
}

int get_xshift(void)
{
	int potx,shift;
	float pagex,pagey;

	if(horscroller == NULL)
		return(0);

	get_pagesizes(&pagex, &pagey);

	potx=((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot;
	shift=(potx * (CM_TO_PIX(pagex) - (canvas_win->GZZWidth-siderul_width))) / 0xffff;
	return((shift < 0)? 0:shift);
}

int get_yshift(void)
{
	int poty,shift;
	float pagex,pagey;

	if(vertscroller == NULL)
		return(0);

	get_pagesizes(&pagex, &pagey);

	poty=((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot;
	shift=(poty * (CM_TO_PIX(pagey) - (canvas_win->GZZHeight-toprul_height))) / 0xffff;

	if(shift < 0)
		shift=0;

	shift-=toprul_height;

	return(shift);
}

static int get_body(int windsize, float pagesize)
{
	float windowsize;

	/* Window size in cm = size in Pixels / (Pixels per cm on screen  * Display-Zoom) */
	windowsize=(float)windsize / round(DISPLAY_PIX_PER_CM * display_zoomscale);
	windowsize=(windowsize < pagesize)? (windowsize/pagesize)*0xffff : 0xffff;

	return((int)windowsize);
}

/* nullpos: if TRUE => reset to initial position */
void update_scroller(BOOL nullpos)
{
	float pagex,pagey;

	if(horscroller == NULL || vertscroller == NULL)
		return;

	get_pagesizes(&pagex, &pagey);

	if(nullpos)
	{
		((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot=0;
		((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot=0;
	}

	if(horscroller)
	{
        xshift=get_xshift();
		((struct PropInfo *)(horscroller ->SpecialInfo))->HorizBody=get_body(canvas_win->GZZWidth-siderul_width, pagex);
	}

	if(vertscroller)
	{
        yshift=get_yshift();
		((struct PropInfo *)(vertscroller->SpecialInfo))->VertBody=get_body(canvas_win->GZZHeight-toprul_height, pagey);
	}

	RefreshGList(horscroller,canvas_win,0L,2);
}

/* scrolls the display to the current offset determined by the scrollbar value */
void syncScrollHorizontal(void)
{
	xshift=get_xshift();

	if(xshift != last_xshift)
	{
		int diff = xshift - last_xshift;
		ScrollRasterBF(rastport, diff, 0, 0, toprul_height, canvas_win->GZZWidth-siderul_width-1, canvas_win->GZZHeight);
		if(diff > 0)
		{
            /* scrolling left */
			refreshRectArea(canvas_win->GZZWidth-siderul_width-diff, toprul_height, canvas_win->GZZWidth-siderul_width-1, canvas_win->GZZHeight);
		}
		else 
		{
            /* scrolling right */
			refreshRectArea(0, toprul_height, -diff, canvas_win->GZZHeight);
		}
		last_xshift = xshift;
		redisplay_rulers(TOP_RULER);
	}
}

/* scroll the display to the current offset determined by the scrollbar value */
void syncScrollVertical(void)
{
	yshift=get_yshift();

	/*
        The vertical shift value contains the top ruler size. Correct this by removing
        the ruler size (shift values are negative, therefore add the ruler size)
    */
	if(yshift+toprul_height != last_yshift)
	{
		int diff = yshift+toprul_height - last_yshift;
		ScrollRasterBF(rastport, 0, diff, 0, toprul_height, canvas_win->GZZWidth-siderul_width-1, canvas_win->GZZHeight);
		if(diff > 0)
		{
            /* scrolling up */
			refreshRectArea(0, canvas_win->GZZHeight-diff, canvas_win->GZZWidth-siderul_width-1, canvas_win->GZZHeight);
		}
		else
		{
            /* scrolling down */
			refreshRectArea(0, toprul_height, canvas_win->GZZWidth-siderul_width-1, toprul_height-diff);
		}
		last_yshift = yshift+toprul_height;
		calc_max_rultextwidth(TRUE);
		redisplay_rulers(SIDE_RULER);
	}
}

/* 
    scroll by an amount relative to the page size
    if factor = 1 then scrolls one page
*/
void jumpHorizontal(float factor)
{
	LONG temp;

    if (horscroller == NULL) return;

    temp=((struct PropInfo *)horscroller->SpecialInfo)->HorizPot;
    temp+=((struct PropInfo *)horscroller->SpecialInfo)->HorizBody*factor;

	if (temp < 0x0000) temp=0x0000;
    if (temp > 0xffff) temp=0xffff;
    
    ((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot=temp;
    RefreshGList(horscroller, canvas_win, NULL, 1) ;
    syncScrollHorizontal();
}

void jumpVertical(float factor)
{
	LONG temp;

    if (vertscroller == NULL) return;

    temp=((struct PropInfo *)vertscroller->SpecialInfo)->VertPot;
    temp+=((struct PropInfo *)vertscroller->SpecialInfo)->VertBody*factor;

	if (temp < 0x0000) temp=0x0000;
    if (temp > 0xffff) temp=0xffff;
    
    ((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot=temp;
    RefreshGList(vertscroller, canvas_win, NULL, 1) ;
    syncScrollVertical();
}

void zoom_display(int X, int Y, float factor)
{
	if ((X != 0) || (Y != 0))
	{
		/* zoom in with focus on current mouse position */
		LONG NewX, NewY, DeltaX=0, DeltaY=0, x, y;
		float pagex,pagey;
        LONG tmp;
		
        x = SCALE_UP_X(X);
        y = SCALE_UP_Y(Y);
        
		display_zoomscale*=factor;
		if(display_zoomscale > MAXZOOM)
			display_zoomscale = MAXZOOM;
		if(display_zoomscale < MINZOOM)
			display_zoomscale = MINZOOM;
		update_scroller(FALSE);
		
		NewX = SCALE_DOWN_X(x);
		NewY = SCALE_DOWN_Y(y);
		get_pagesizes(&pagex, &pagey);
		
        tmp = CM_TO_PIX(pagex) - (canvas_win->GZZWidth-siderul_width);
        if (tmp != 0)
        {
            DeltaX = 	(X - NewX) * 0xFFFF	/ tmp;
            tmp = (long)((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot - DeltaX;
            tmp = min2(max2(tmp,0),0xFFFF);
            ((struct PropInfo *)(horscroller->SpecialInfo))->HorizPot = tmp;
        }
        
        tmp = CM_TO_PIX(pagey) - (canvas_win->GZZHeight-toprul_height);
		if (tmp != 0)
        {
            DeltaY = 	(Y - NewY) * 0xFFFF	/ tmp;		
            tmp = (long)((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot - DeltaY;
            tmp = min2(max2(tmp,0),0xFFFF);
            ((struct PropInfo *)(vertscroller->SpecialInfo))->VertPot = tmp;
		}
		RefreshGList(horscroller,canvas_win,0L,2);
		
		xshift = get_xshift();
		yshift = get_yshift();
	}
	else
	{
		display_zoomscale*=factor;
		if(display_zoomscale > MAXZOOM)
			display_zoomscale = MAXZOOM;
		if(display_zoomscale < MINZOOM)
			display_zoomscale = MINZOOM;
		update_scroller(FALSE);
	}
	calc_max_rultextwidth(TRUE);
	redisplay_canvas();
    follow_mouse(X,Y);
}
