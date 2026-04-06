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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_rulers.c $
 *       $Revision: 341 $
 *       $Date: 2017-02-18 16:41:43 +0000 (Sat, 18 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "paintop.h"
#include "object.h"

#include "w_canvas.h"
#include "a_coords.h"
#include "u_fonts.h"

#define TEXTSHIFT 12
#define TOPRULERSIZE	22
#define SIDERULERSIZE	22

int              last_toprul_pos=0, last_siderul_pos=0;
int              toprul_height=TOPRULERSIZE, siderul_width=SIDERULERSIZE; // always contains the height/width of the rulers and is set to 0 if ruler is invisible

void draw_topmark(int x);
void draw_sidemark(int x);

// top ruler

void print_number(int number, int x, int y)
{
	char buf[10];
	int len;

	sprintf(buf, "%d", number);
	len=TextLength(rastport, (CONST_STRPTR)buf, strlen(buf));
	Move(rastport, x-len/2, y);
	Text(rastport, (CONST_STRPTR)buf, strlen(buf));
}

BOOL setRulerFont(void)
{
	SetFont(rastport, GUIfont_Fixed);
	return(TRUE);
}

void redisplay_topruler(BOOL show)
{
	float x, x2, resolution, resolution10;
	int i, j, X, disp_text = 1;
	BOOL disp10 = FALSE, disp5 = FALSE, disp2 = FALSE, disp = FALSE, anfang = FALSE, text;

	if(appres.topruler_on)
    {
        text=setRulerFont();
        SetDrMd(rastport, JAM1);
        if(appres.INCHES)
        {
            resolution   = display_zoomscale*DISPLAY_PIX_PER_INCH;
            resolution10 = display_zoomscale*DISPLAY_PIX_PER_INCH / 10.0;
        }
        else
        {
            resolution   = display_zoomscale*DISPLAY_PIX_PER_CM;
            resolution10 = display_zoomscale*DISPLAY_PIX_PER_CM / 10.0;
        }

        // Define which markers to display
        if (resolution >=  4.0)     disp   = TRUE;
        if (resolution >=  8.0)     disp2  = TRUE;
        if (resolution >= 15.0)     disp5  = TRUE;
        if (resolution >= 30.0)     disp10 = TRUE;
        
        if (resolution < 18.0)		disp_text=2;
        if (resolution <  9.0)		disp_text=3;
        if (resolution <  6.0)		disp_text=5;
        if (resolution <  4.0)		disp_text=7;

        if(!disp)
            toprul_height=3;
        else
            toprul_height=TOPRULERSIZE;

        xshift=get_xshift();
        yshift=get_yshift();

        // Umriß zeichnen
        SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);
        Move(rastport, 0, toprul_height-1);
        Draw(rastport, canvas_win->GZZWidth-1, toprul_height-1);
		SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, 0x00AAAAAA, TAG_DONE);
		RectFill(rastport, 0, 0, canvas_win->GZZWidth-1, toprul_height-2);
        SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);

        i=fmod((float)xshift/resolution10, 10.0);
        j=((float)xshift/resolution10)/10.0;

        if(i != 0.0)
        {
            anfang=TRUE;
            j++; 	// die erste Zahl ist wegen xshift nicht sichtbar und wird beim Zeichnen übersprungen
        }

        if(show)
        {
            if (disp)
            {
                for(x=resolution10-fmod((float)xshift, resolution10) - resolution10; x <= canvas_win->GZZWidth;)
                {
                    for(x2=x; i < 10; i++) // Main Unit
                    {
                        X=round(x2);
                        Move(rastport, X, toprul_height-1);
                        switch(i)
                        {
                            case 0:     // Main Unit
                                Draw(rastport, X, toprul_height-1 - 10);
                                if(text && j%disp_text == 0)
                                    print_number(j, X, toprul_height-1 - 12);
                                j++;
                                break;
                            case 5:     // Half of Unit
                                if((disp2) && (!(disp5) || disp10))
                                    Draw(rastport, X, toprul_height-1 - 5);
                                break;
                            default:    // Tenth or Fifth of Unit
                                if((disp10) || (disp5 && i%2 == 0))
                                    Draw(rastport, X, toprul_height-1 - 3);
                        }
                        x2+=resolution10;
                    }
                    if(!anfang)			// das erste Stück ist u.U. nicht resolution breit
                        x+=resolution;
                    else
                        x=x2;
                    anfang=FALSE;
                    i=0;
                }
            }
            last_toprul_pos=0;
            draw_topmark(0);
        }
    }
}

// side ruler
int calc_max_rultextwidth(BOOL recalc)
{
	float y, resolution;
	int j, max_textwidth=0, wid;
	char buf[10];
	static int maxwidth=0;

	if(recalc)
	{
		if(appres.INCHES)
			resolution = display_zoomscale*DISPLAY_PIX_PER_INCH / 10.0;
		else
			resolution = display_zoomscale*DISPLAY_PIX_PER_CM / 10.0;

		if(resolution*10.0 < 4.0)
		{
			maxwidth=0;
			return(maxwidth);
		}
		else
		{
			j=((float)yshift/resolution)/10.0 + 1;

			setRulerFont();
			for(y=resolution-fmod((float)yshift, resolution); y <= canvas_win->GZZHeight; y += 10*resolution)
			{
				sprintf(buf, "%d", j);
				if((wid=TextLength(rastport, (CONST_STRPTR)buf, strlen(buf))) > max_textwidth) max_textwidth=wid;
				j++;
			}
			maxwidth = max_textwidth + TEXTSHIFT;
		}
	}
	return(maxwidth);
}

void redisplay_sideruler(BOOL show)
{
	float y, y2, resolution, resolution10;
	int i, j, X, Y, max_textwidth, shift, disp_text = 1;
    BOOL disp10 = FALSE, disp5 = FALSE, disp2 = FALSE, disp = FALSE, anfang = FALSE, text;
    
	if(appres.sideruler_on)
    {
        text=setRulerFont();
        SetDrMd(rastport, JAM1);
        if(appres.INCHES)
        {
            resolution   = display_zoomscale*DISPLAY_PIX_PER_INCH;
            resolution10 = display_zoomscale*DISPLAY_PIX_PER_INCH / 10.0;
        }
        else
        {
            resolution   = display_zoomscale*DISPLAY_PIX_PER_CM;
            resolution10 = display_zoomscale*DISPLAY_PIX_PER_CM / 10.0;
        }
        // Define which markers to display
        if (resolution >=  4.0)     disp   = TRUE;
        if (resolution >=  8.0)     disp2  = TRUE;
        if (resolution >= 15.0)     disp5  = TRUE;
        if (resolution >= 30.0)     disp10 = TRUE;
        
        if (resolution < 18.0)		disp_text=2;
        if (resolution <  9.0)		disp_text=3;
        if (resolution <  6.0)		disp_text=5;
        if (resolution <  4.0)		disp_text=7;

        max_textwidth = calc_max_rultextwidth(FALSE);
        siderul_width = max_textwidth + 3;

        xshift=get_xshift();
        yshift=get_yshift();

        // Umriß zeichnen
        SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);
        Move(rastport, canvas_win->GZZWidth-siderul_width, 0);
        Draw(rastport, canvas_win->GZZWidth-siderul_width, canvas_win->GZZHeight-1);
		SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, 0x00AAAAAA, TAG_DONE);
		RectFill(rastport, canvas_win->GZZWidth-siderul_width + 1, 0, canvas_win->GZZWidth, canvas_win->GZZHeight-1);
        SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);
        Move(rastport, canvas_win->GZZWidth-siderul_width, toprul_height-1);
        Draw(rastport, canvas_win->GZZWidth-1, toprul_height-1);

        shift=yshift+toprul_height;
        i=fmod((float)shift/resolution10, 10.0);
        j=((float)shift/resolution10)/10.0;

        if(i != 0)
        {
            anfang=TRUE;
            j++; 	// die erste Zahl ist wegen xshift nicht sichtbar und wird beim Zeichnen übersprungen
        }

        if(show)
        {
            X=canvas_win->GZZWidth-siderul_width;
            for(y=toprul_height + resolution10-fmod((float)shift, resolution10) - resolution10; y <= canvas_win->GZZHeight;)
            {
                for(y2=y; i < 10; i++) // Main Unit
                {
                    Y=round(y2);
                    Move(rastport, X, Y);
                    switch(i)
                    {
                        case 0:             // Main Unit
                            if(disp)
                            {
                                Draw(rastport, X+10, Y);
                                if(disp && text && j%disp_text == 0)
                                    print_number(j, canvas_win->GZZWidth-max_textwidth+TEXTSHIFT+3, Y+3);
                            }
                            j++;
                            break;
                        case 5:             // Half of Unit
                            if((disp2) && (!(disp5) || disp10))
                                Draw(rastport, X+5, Y);
                            break;
                        default:            // Tenth or Fifth of Unit
                            if((disp10) || (disp5 && i%2 == 0))
                                Draw(rastport, X+3, Y);
                    }
                    y2+=resolution10;
                }

                if(!anfang)			// das erste Stück ist u.U. nicht resolution breit
                    y+=resolution;
                else
                    y=y2;

                anfang=FALSE;
                i=0;
            }

            last_siderul_pos=toprul_height;
            draw_sidemark(toprul_height);
        }
    }
}

// which: 1 top only, 2 side only, 3 both
void redisplay_rulers(int which)
{
	UWORD old_pattern;

	old_pattern=rastport->LinePtrn;
	SetDrPt(rastport, 0xffff);
	
	delete_paint_region();

	if(which&1)
	{
		toprul_height = 0;
		if(appres.topruler_on) redisplay_topruler(TRUE);
	}

	if(which&2)
	{
		siderul_width = 0;
		if(appres.sideruler_on) redisplay_sideruler(TRUE);
	}

	setup_paint_region();
	
	SetDrPt(rastport, old_pattern);
}

void draw_topmark(int x)
{
    SetDrMd(rastport, COMPLEMENT);
	Move(rastport, x  , toprul_height-3);
    Draw(rastport, x  , toprul_height-4);
    Draw(rastport, x-8, 0);
    Draw(rastport, x+8, 0);
    Draw(rastport, x  , toprul_height-4);
}

void move_topmark(int x)
{
	UWORD old_pattern;

	if(x > canvas_win->GZZWidth-siderul_width-1)
		x=canvas_win->GZZWidth-siderul_width-1;

	if(appres.topruler_on)
	{
		old_pattern=rastport->LinePtrn;
		SetDrPt(rastport, 0xffff);
		draw_topmark(last_toprul_pos);
		draw_topmark(x);
		last_toprul_pos=x;
		SetDrPt(rastport, old_pattern);
	}
}

void draw_sidemark(int y)
{    
    SetDrMd(rastport, COMPLEMENT);
	Move(rastport, canvas_win->GZZWidth-siderul_width+2 , y);
    Draw(rastport, canvas_win->GZZWidth-siderul_width+3 , y);
    Draw(rastport, canvas_win->GZZWidth-1               , y-8);
    Draw(rastport, canvas_win->GZZWidth-1               , y+8);
    Draw(rastport, canvas_win->GZZWidth-siderul_width+3 , y);
}

void move_sidemark(int y)
{
	UWORD old_pattern;

	if(y < toprul_height)
		y=toprul_height;

	if(appres.sideruler_on)
	{
		old_pattern=rastport->LinePtrn;
		SetDrPt(rastport, 0xffff);
		draw_sidemark(last_siderul_pos);
		draw_sidemark(y);
		last_siderul_pos=y;
		SetDrPt(rastport, old_pattern);
	}
}

void follow_mouse(int x, int y)
{
	delete_paint_region();
	move_topmark(x);
	move_sidemark(y);
	setup_paint_region();
}
