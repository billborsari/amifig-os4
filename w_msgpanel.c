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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_msgpanel.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/* This is for the message window below the command panel */
/* The popup message window is handled in the second part of this file */

#include "fig.h"
#ifndef AMIFIG
#include "figx.h"
#include <stdarg.h>
#else
#include "intui.h"
#include <proto/muimaster.h>
#include <libraries/mui.h>
#endif
#include "resources.h"
#include "object.h"
#include "mode.h"
#include "d_line.h"
#ifndef AMIFIG
#include "f_read.h"
#include "f_util.h"
#endif
#include "paintop.h"
#include "u_elastic.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#ifndef AMIFIG
#include "w_file.h"
#include "w_indpanel.h"
#endif
#include "w_msgpanel.h"
#ifndef AMIFIG
#include "w_util.h"
#include "w_setup.h"
#include "w_zoom.h"
#endif
#include "u_geom.h"
#ifndef AMIFIG
#include "w_color.h"
#else
#include "u_fonts.h"
#include "z_fig.h"
#include "z_edit.h"
#define roman_font GUIfont_Fixed
#endif



/********************* EXPORTS *******************/


BOOL		popup_up = FALSE;
BOOL		file_msg_is_popped=FALSE;
BOOL		first_file_msg;
BOOL		first_lenmsg = TRUE;

/************************  LOCAL ******************/

#ifndef AMIFIG
static BOOL update_figs = TRUE;
#endif

#ifdef AMIFIG
/* Message */
APTR	TEXT_MESSAGE;

/*----------------------------------------------------------------------------*/
/*     MakeMsgWindow - creates the help window                               */
/*----------------------------------------------------------------------------*/

#define MAX_MESSAGES       128
#define MAX_MESSAGE_LENGTH 128
static char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
static int  nb_messages = 0;

APTR MakeMsgWindow(void)
{	    
    return(WindowObject,
        MUIA_Window_Title, "AmiFig : Messages",
        MUIA_Window_ID, MAKEID('Z','F','M','S'),
		MUIA_Window_Screen, Scr,
		MUIA_Window_ScreenTitle, "AmiFIG",

        WindowContents,	VGroup,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (char *)_(msg_LastMsg),
            Child, TEXT_MESSAGE = ListviewObject,
                MUIA_Listview_Input, FALSE,
                MUIA_Listview_List , ListObject,
                    ReadListFrame,
                End,
            End,
        End,
	End);
}

void z_msg_methods(void)
{
    /* Close window */
    DoMethod(WinMsg, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinMsg, 3, MUIM_Set, MUIA_Window_Open, FALSE);
}

void beep(void)
{
    DisplayBeep(NULL);
}

void set_mousefun(char *LeftBut, char *MiddleBut, char *RightBut, char *S_LeftBut, char *S_MiddleBut, char*S_RightBut)
{
    Update_MouseAssigns(LeftBut, MiddleBut, RightBut, S_LeftBut, S_MiddleBut, S_RightBut);
}

void draw_mousefun_canvas()
{
    /* No need to do anything here */
}

/* put a message in the message window below the command button panel */
void 
put_msg(char *format, ...)
{
	va_list Arg;
    va_start(Arg, format);
    vsprintf(messages[nb_messages%MAX_MESSAGES], format, Arg);

    if (nb_messages > MAX_MESSAGES)
    {
        DoMethod(TEXT_MESSAGE, MUIM_List_Remove, MUIV_List_Remove_Last);
    }
    DoMethod(TEXT_MESSAGE, MUIM_List_InsertSingle, messages[nb_messages%MAX_MESSAGES], MUIV_List_Insert_Top);
    set(WinMode, MUIA_Window_Title, messages[nb_messages%MAX_MESSAGES]);
    nb_messages++;

    va_end(Arg);
}

BOOL doRequest(char *Msg)
{
	return((BOOL)MUI_Request(App, NULL, 0, NULL, (CONST_STRPTR)"Yes|No", (CONST_STRPTR)Msg));
}

void doMessage(char *Msg, ...)
{
	static char message[MAX_MESSAGE_LENGTH];	
	va_list Arg;
	
	va_start(Arg, Msg);
	vsprintf(message, Msg, Arg);
	MUI_Request(App, NULL, 0, NULL, (CONST_STRPTR)_(msg_OK), (CONST_STRPTR)message);
	va_end(Arg);
}
#endif

static int	  ox = 0,
              oy = 0,
             ofx = 0,
             ofy = 0,
            ot1x = 0,
            ot1y = 0,
            ot2x = 0,
            ot2y = 0,
            ot3x = 0,
            ot3y = 0;

static char	bufx[20], bufy[20], bufhyp[20];

void
boxsize_msg(int fact)
{
    float	dx, dy;
    int		sdx, sdy;
    int		t1x, t1y, t2x, t2y;
    pr_size	sizex, sizey;
    float	sc_fact, old_dx, old_dy, new_dx, new_dy;
    float	udx, udy;
    char	dxstr[80],dystr[80];

    old_dx = (float)(abs(from_x - fix_x));
    old_dy = (float)(abs(from_y - fix_y));
    new_dx = (float)(abs(cur_x - fix_x));
    new_dy = (float)(abs(cur_y - fix_y));
    /* compute reasonable scale factor */
    if ((old_dx != 0.0) && (new_dx != 0.0))
        sc_fact = new_dx / old_dx;
    else if ((old_dy != 0.0) && (new_dy != 0.0))
        sc_fact = new_dy / old_dy;
    else
        sc_fact = 0.0;

    dx = (float) fact * (cur_x - fix_x);
    dy = (float) fact * (cur_y - fix_y);
    make_dimension_string(fabs(dx), dxstr, FALSE);
    make_dimension_string(fabs(dy), dystr, FALSE);
    put_msg((char *)_(msg_WidthHeightFactor), dxstr, dystr, sc_fact);

    /* if showing line lengths */
    if (appres.showlengths && !freehand_line) {
        if (dx < 0)
            sdx = -2.0/zoomscale;
        else
            sdx = 2.0/zoomscale;
        if (dy < 0)
            sdy = 2.0/zoomscale;
        else
            sdy = -2.0/zoomscale;

        /* erase old text */
        if (!first_lenmsg) {
            pw_text(canvas_win, ot1x, ot1y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufx,
                RED, COLOR_NONE);
            pw_text(canvas_win, ot2x, ot2y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufy,
                RED, COLOR_NONE);
        }
        first_lenmsg = FALSE;

        /* draw new text */

        /* in user units */
        udx = dx*appres.userscale /(double)(appres.INCHES? PIX_PER_INCH: PIX_PER_CM);
        udy = dy*appres.userscale /(double)(appres.INCHES? PIX_PER_INCH: PIX_PER_CM);
        sprintf(bufx,"%.3f", fabs(udx));
        sizex = textsize(roman_font, strlen(bufx), bufx);
        sprintf(bufy,"%.3f", fabs(udy));
        sizey = textsize(roman_font, strlen(bufy), bufy);
        /* dx first */
        t1x = (cur_x+fix_x)/2;
        if (dy < 0)
            t1y = cur_y + sdy - 5.0/zoomscale;			/* above the line */
        else
            t1y = cur_y + sdy + sizey.ascent + 5.0/zoomscale;	/* below the line */
            pw_text(canvas_win, t1x, t1y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufx,
                RED, COLOR_NONE);

        /* then dy */
        t2y = (cur_y+fix_y)/2+sdy;
        if (dx < 0)
            t2x = fix_x + sdx + 5.0/zoomscale;			/* right of the line */
        else
            t2x = fix_x + sdx - sizex.length - 4.0/zoomscale;	/* left of the line */
        pw_text(canvas_win, t2x, t2y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufy,
                RED, COLOR_NONE);

        /* now save new values */
        ot1x = t1x;
        ot1y = t1y;
        ot2x = t2x;
        ot2y = t2y;
        ox = cur_x+sdx;
        oy = cur_y+sdy;
    }
}

void boxsize_scale_msg(int fact)
{
    float	dx, dy;
    float	sc_fact, old_dx, old_dy, new_dx, new_dy;
    char	dxstr[80],dystr[80];

    old_dx = (float)(abs(from_x - fix_x));
    old_dy = (float)(abs(from_y - fix_y));
    new_dx = (float)(abs(cur_x - fix_x));
    new_dy = (float)(abs(cur_y - fix_y));
    dx = (float) fact * abs(cur_x - fix_x);
    dy = (float) fact * abs(cur_y - fix_y);
    make_dimension_string(dx, dxstr, FALSE);
    make_dimension_string(dy, dystr, FALSE);
    /* compute reasonable scale factor */
    if ((old_dx != 0.0) && (new_dx != 0.0))
        sc_fact = new_dx / old_dx;
    else if ((old_dy != 0.0) && (new_dy != 0.0))
        sc_fact = new_dy / old_dy;
    else
        sc_fact = 0.0;
    put_msg((char *)_(msg_WidthLengthFactor), dxstr, dystr, sc_fact);
}

void erase_box_lengths(void)
{
    if (!first_lenmsg && appres.showlengths && !freehand_line) {
	/* erase old text */
        pw_text(canvas_win, ot1x, ot1y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufx,
                RED, COLOR_NONE);
        pw_text(canvas_win, ot2x, ot2y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufy,
                RED, COLOR_NONE);
    }
    first_lenmsg = FALSE;
}

void erase_lengths(void)
{
    if (!first_lenmsg && appres.showlengths && !freehand_line) {
        /* erase old lines first */
        pw_vector(canvas_win,  ox, oy, ofx,  oy, INV_PAINT, 1, RUBBER_LINE, 0.0, RED);
        pw_vector(canvas_win, ofx, oy, ofx, ofy, INV_PAINT, 1, RUBBER_LINE, 0.0, RED);

        /* erase old text */
        pw_text(canvas_win, ot1x, ot1y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufx,
                RED, COLOR_NONE);
        pw_text(canvas_win, ot2x, ot2y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufy,
                RED, COLOR_NONE);
        pw_text(canvas_win, ot3x, ot3y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, bufhyp,
                RED, COLOR_NONE);
    }
    first_lenmsg = FALSE;
}

void
length_msg(int type)
{
    altlength_msg(type, fix_x, fix_y);
}

/*
** In typical usage, point fx,fy is the fixed point.
** Distance will be measured from it to cur_x,cur_y.
*/

void
altlength_msg(int type, int fx, int fy)
{
    double	dx,dy;
    float		len, ulen;
    float		udx, udy;
    float		ang;
    int		sdx, sdy;
    int		t1x=0, t1y=0, t2x=0, t2y=0, t3x=0, t3y=0;
    pr_size	sizex, sizey, sizehyp;
    char		lenstr[80],dxstr[80],dystr[80];

    dx = (cur_x - fx);
    dy = (cur_y - fy);
    len = (float) sqrt(dx*dx + dy*dy);
    make_dimension_string(dx, dxstr, FALSE);
    make_dimension_string(dy, dystr, FALSE);
    make_dimension_string(len, lenstr, FALSE);

    /* in user units */
    udx = dx*appres.userscale /(double)(appres.INCHES? PIX_PER_INCH: PIX_PER_CM);
    udy = dy*appres.userscale /(double)(appres.INCHES? PIX_PER_INCH: PIX_PER_CM);
    ulen = len*appres.userscale /(double)(appres.INCHES? PIX_PER_INCH: PIX_PER_CM);
    if (dy != 0.0 || dx != 0.0) {
        ang = (float) - atan2(dy,dx) * 180.0/M_PI;
    } else {
        ang = 0.0;
    }
    switch (type) {
        case MSG_RADIUS:
        case MSG_DIAM:
            put_msg("%s = %s, dx = %s, dy = %s",
                        (type==MSG_RADIUS? (char *)_(msg_Radius):(char *)_(msg_Diameter)),
                lenstr, dxstr, dystr);
            break;
        case MSG_RADIUS2:
            put_msg((char *)_(msg_Radiuses),
                dxstr, dystr);
            break;
        case MSG_PNTS_LENGTH:
            put_msg((char *)_(msg_LengthMeas),
                num_point, ((num_point != 1)? "s": ""),
                lenstr, dxstr, dystr, ang );
            break;
        case MSG_RADIUS_ANGLE:
        case MSG_DIAM_ANGLE:
            if (num_point == 0)
              put_msg((char *)_(msg_RadOrDimAndAngle),
                  (type==MSG_RADIUS_ANGLE? (char *)_(msg_Radius):(char *)_(msg_Diameter)),
                  lenstr, ang );
            else
              put_msg("%d point%s, Angle = %.1f deg",
                  num_point, ((num_point != 1)? "s": ""), ang );
            break;
        default:
            put_msg("%s = %s, dx = %s, dy = %s (%.1f) deg",
                (type==MSG_LENGTH? (char *)_(msg_Length): (char *)_(msg_Distance)),
                lenstr, dxstr, dystr, ang);
            break;
    }

    /* now draw two lines to complete the triangle and label the two sides
     with the lengths e.g.:
                  |\
                  | \
                  |  \
            2.531 |   \ 2.864
                  |    \
                  |     \
                  -------
                1.341
    */

    if (dx < 0)
    sdx = 2.0/zoomscale;
    else
    sdx = -2.0/zoomscale;
    if (dy < 0)
    sdy = -2.0/zoomscale;
    else
    sdy = 2.0/zoomscale;

    if (appres.showlengths && !freehand_line) {
        switch (type) {
            case MSG_PNTS_LENGTH:
            case MSG_LENGTH:
            case MSG_DIST:
                if (!first_lenmsg) {
                    /* erase old lines first */
                    pw_vector(canvas_win,ox,oy,ofx,oy,INV_PAINT,1,RUBBER_LINE,0.0,RED);
                    pw_vector(canvas_win,ofx,oy,ofx,ofy,INV_PAINT,1,RUBBER_LINE,0.0,RED);

                    /* erase old text */
                    pw_text(canvas_win, ot1x, ot1y, INV_PAINT, MAX_DEPTH+1, roman_font,
                            0.0, bufx, RED, COLOR_NONE);
                    pw_text(canvas_win, ot2x, ot2y, INV_PAINT, MAX_DEPTH+1, roman_font,
                            0.0, bufy, RED, COLOR_NONE);
                    pw_text(canvas_win, ot3x, ot3y, INV_PAINT, MAX_DEPTH+1, roman_font,
                            0.0, bufhyp, RED, COLOR_NONE);
                }

                /* draw new lines */
                /* horizontal (dx) */
                pw_vector(canvas_win, cur_x+sdx, cur_y+sdy, fx+sdx, cur_y+sdy, INV_PAINT, 1, 
                        RUBBER_LINE, 0.0, RED);
                /* vertical (dy) */
                pw_vector(canvas_win, fx+sdx, cur_y+sdy, fx+sdx, fy+sdy, INV_PAINT, 1, 
                        RUBBER_LINE, 0.0, RED);

                /* draw new text */

                /* put the lengths in strings and get their sizes for positioning */
                sprintf(bufx,"%.3f", fabs(udx));
                sizex = textsize(roman_font, strlen(bufx), bufx);
                sprintf(bufy,"%.3f", fabs(udy));
                sizey = textsize(roman_font, strlen(bufy), bufy);
                sprintf(bufhyp,"%.3f", ulen);
                sizehyp = textsize(roman_font, strlen(bufhyp), bufhyp);

                /* dx first */
                t1x = (cur_x+fx)/2;
                if (dy < 0)
                    t1y = cur_y + sdy - 3.0/zoomscale;			/* above the line */
                else
                    t1y = cur_y + sdy + sizey.ascent + 3.0/zoomscale;	/* below the line */
                    pw_text(canvas_win, t1x, t1y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0, 
                        bufx, RED, COLOR_NONE);

                t2y = (cur_y+fy)/2+sdy;
                /* now dy */
                if (dx < 0)
                    t2x = fx + sdx + 4.0/zoomscale;			/* right of the line */
                else
                    t2x = fx + sdx - sizex.length - 4.0/zoomscale;	/* left of the line */
                pw_text(canvas_win, t2x, t2y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0,
                        bufy, RED, COLOR_NONE);

                /* finally, the hypotenuse */
                if (dx > 0)
                    t3x = t1x + 4.0/zoomscale;			/* right of the hyp */
                else
                    t3x = t1x - sizehyp.length - 4.0/zoomscale;	/* left of the hyp */
                if (dy < 0)
                    t3y = t2y + sizehyp.ascent + 3.0/zoomscale;	/* below the hyp */
                else
                    t3y = t2y - 3.0/zoomscale;			/* above the hyp */
                pw_text(canvas_win, t3x, t3y, INV_PAINT, MAX_DEPTH+1, roman_font, 0.0,
                        bufhyp, RED, COLOR_NONE);

                break;

            default:
                break;
        }
        first_lenmsg = FALSE;
    }

    ot1x = t1x;
    ot1y = t1y;
    ot2x = t2x;
    ot2y = t2y;
    ot3x = t3x;
    ot3y = t3y;
    ox = cur_x+sdx;
    oy = cur_y+sdy;
    ofx = fx+sdx;
    ofy = fy+sdy;
}

/*
 * In typical usage, point x3,y3 is the one that is moving,
 * the other two are fixed.  Distances will be measured from
 * points 1 -> 3 and 2 -> 3.
 * Delta X and Y are measured from global from_x and from_y.
 */

void
length_msg2(int x1, int y1, int x2, int y2, int x3, int y3)
{
    float	len1, len2;
    double	dx1=0.0, dy1=0.0, dx2=0.0, dy2=0.0;
    char	len1str[80], len2str[80];
    char	dx1str[80], dy1str[80], dx2str[80], dy2str[80];

    len1=len2=0.0;
    if (x1 != -999) {
	    dx1 = x3 - x1;
	    dy1 = y3 - y1;
	    len1 = (float)(sqrt(dx1*dx1 + dy1*dy1));
    }
    if (x2 != -999) {
	    dx2 = x3 - x2;
	    dy2 = y3 - y2;
	    len2 = (float)(sqrt(dx2*dx2 + dy2*dy2));
    }
    make_dimension_string(len1, len1str, FALSE);
    make_dimension_string(len2, len2str, FALSE);
    make_dimension_string(dx1, dx1str, FALSE);
    make_dimension_string(dy1, dy1str, FALSE);
    make_dimension_string(dx2, dx2str, FALSE);
    make_dimension_string(dy2, dy2str, FALSE);
    put_msg("Len 1 = %s, Len 2 = %s, dx1 = %s, dy1 = %s, dx2 = %s, dy2 = %s",
		len1str, len2str, dx1str, dy1str, dx2str, dy2str);
}

/* x2,y2 moving middle point */

void 
arc_msg(int x1, int y1, int x2, int y2, int x3, int y3)
{
    float	len1, len2, r;
    double	dx1, dy1, dx2, dy2;
    char	len1str[80], len2str[80], radstr[80];
    char	dx1str[80], dy1str[80], dx2str[80], dy2str[80];

    if (!compute_arcradius(x1, y1, x2, y2, x3, y3, &r))
        length_msg2(x1, y1, x2, y2, x3, y3);
    else {
        dx1 = x3 - x1;
        dy1 = y3 - y1;
        len1 = (float)(sqrt(dx1*dx1 + dy1*dy1));
        dx2 = x3 - x2;
        dy2 = y3 - y2;
        len2 = (float)(sqrt(dx2*dx2 + dy2*dy2));
        make_dimension_string(len1, len1str, FALSE);
        make_dimension_string(len2, len2str, FALSE);
        make_dimension_string(r, radstr, FALSE);
        make_dimension_string(dx1, dx1str, FALSE);
        make_dimension_string(dy1, dy1str, FALSE);
        make_dimension_string(dx2, dx2str, FALSE);
        make_dimension_string(dy2, dy2str, FALSE);
        put_msg("Len 1 = %s, Len 2 = %s, Rad = %s, dx1 = %s, dy1 = %s, dx2 = %s, dy2 = %s",
            len1str, len2str, radstr, dx1str, dy1str, dx2str, dy2str);
    }
}

void
lenmeas_msg(char *msgtext, float len, float totlen)
{
    char	lenstr[80],totlenstr[80];

    make_dimension_string(len, lenstr, FALSE);
    make_dimension_string(totlen, totlenstr, FALSE);

    if (totlen >= 0.0) 
        put_msg((char *)_(msg_LengthOfIsAcc),
                 msgtext, lenstr, totlenstr);
    else
        put_msg((char *)_(msg_LengthOfIs),
                 msgtext, lenstr);
}

void
areameas_msg(char *msgtext, float area, float totarea, int flag)
{
    char	areastr[80],totareastr[80];

    make_dimension_string(area, areastr, TRUE);
    make_dimension_string(totarea, totareastr, TRUE);
    if (flag) 
        put_msg((char *)_(msg_AreaOfIsAcc),
                 msgtext, areastr, totareastr);
    else
        put_msg((char *)_(msg_AreaOfIs),
                 msgtext, areastr);
}


/* This is the section for the popup message window (file_msg) */
/* if global update_figs is true, do a fprintf(stderr,msg) instead of in the window */

/* VARARGS1 */
void
file_msg(char *format,...)
{
#ifndef AMIFIG
    XawTextBlock block;
    va_list ap;

    if (!update_figs) {
        popup_file_msg();
        if (first_file_msg) {
	    first_file_msg = False;
            file_msg("---------------------");
            file_msg("File %s:",read_file_name);
        }
    }

    va_start(ap, format);
    /* format the string (but leave room for \n and \0) */
    vsnprintf(tmpstr, sizeof(tmpstr)-2, format, ap);
    va_end(ap);

    strcat(tmpstr,"\n");
    if (update_figs) {
        fprintf(stderr,tmpstr);
    } else {
        /* append this message to the file message widget string */
        block.firstPos = 0;
        block.ptr = tmpstr;
        block.length = strlen(tmpstr);
        block.format = FMT8BIT;
        /* make editable to add new message */
        FirstArg(XtNeditType, XawtextEdit);
        SetValues(file__(msg_win));
        /* insert the new message after the end */
        (void) XawTextReplace(file__(msg_win),file__(msg_length),file__(msg_length),&block);
        (void) XawTextSetInsertionPoint(file__(msg_win),file__(msg_length));

        /* make read-only again */
        FirstArg(XtNeditType, XawtextRead);
        SetValues(file__(msg_win));
        file__(msg_length) += block.length;
    }
#endif
}

#ifndef AMIFIG
static void
clear_file_message(Widget w, XButtonEvent *ev)
{
    XawTextBlock	block;
    int			replcode;

    if (!file__(msg_popup))
	return;

    tmpstr[0]=' ';
    block.firstPos = 0;
    block.ptr = tmpstr;
    block.length = 1;
    block.format = FMT8BIT;

    /* make editable to clear message */
    FirstArg(XtNeditType, XawtextEdit);
    NextArg(XtNdisplayPosition, 0);
    SetValues(file__(msg_win));

    /* replace all messages with one blank */
    replcode = XawTextReplace(file__(msg_win),0,file__(msg_length),&block);
    if (replcode == XawPositionError)
        fprintf(stderr,"XawTextReplace XawPositionError\n");
    else if (replcode == XawEditError)
        fprintf(stderr,"XawTextReplace XawEditError\n");

    /* make read-only again */
    FirstArg(XtNeditType, XawtextRead);
    SetValues(file__(msg_win));
    file__(msg_length) = 0;
}

static void
file__(msg_panel_dismiss)(Widget w, XButtonEvent *ev)
{
	XtPopdown(file__(msg_popup));
	file_msg_is_popped=False;
}
#endif

void
popup_file_msg(void)
{
#ifndef AMIFIG
	if (file__(msg_popup)) {
	    if (!file_msg_is_popped) {
            XtPopup(file__(msg_popup), XtGrabNone);
            XSetWMProtocols(tool_d, XtWindow(file__(msg_popup)), &wm_delete_window, 1);
	    }
	    /* ensure that the most recent colormap is installed */
	    set_cmap(XtWindow(file__(msg_popup)));
	    file_msg_is_popped = True;
	    return;
	}

	file_msg_is_popped = True;
	FirstArg(XtNx, 0);
	NextArg(XtNy, 0);
	NextArg(XtNcolormap, tool_cm);
	NextArg(XtNtitle, "Xfig: Error messages");
	file__(msg_popup) = XtCreatePopupShell("file_msg",
					transientShellWidgetClass,
					tool, Args, ArgCount);
	XtOverrideTranslations(file__(msg_popup),
			XtParseTranslationTable(file__(msg_translations)));
	XtAppAddActions(tool_app, file__(msg_actions), XtNumber(file__(msg_actions)));

	file__(msg_panel) = XtCreateManagedWidget("file__(msg_panel)", formWidgetClass,
					   file__(msg_popup), NULL, ZERO);
	XtOverrideTranslations(file__(msg_panel),
			   XtParseTranslationTable(file_msg2_translations));

	FirstArg(XtNwidth, 500);
	NextArg(XtNheight, 200);
	NextArg(XtNeditType, XawtextRead);
	NextArg(XtNdisplayCaret, False);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
	NextArg(XtNscrollVertical, XawtextScrollAlways);
	NextArg(XtNright, XtChainRight);
	NextArg(XtNbottom, XtChainBottom);
	file__(msg_win) = XtCreateManagedWidget("file__(msg_win)", asciiTextWidgetClass,
					     file__(msg_panel), Args, ArgCount);
	XtOverrideTranslations(file__(msg_win),
			   XtParseTranslationTable(file_msg2_translations));

	FirstArg(XtNlabel, "Dismiss");
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNfromVert, file__(msg_win));
	NextArg(XtNtop, XtChainBottom);
	NextArg(XtNbottom, XtChainBottom);
	NextArg(XtNleft, XtChainLeft);
	NextArg(XtNright, XtChainLeft);
	file__(msg_dismiss) = XtCreateManagedWidget("dismiss", commandWidgetClass,
				       file__(msg_panel), Args, ArgCount);
	XtAddEventHandler(file__(msg_dismiss), ButtonReleaseMask, False,
			  (XtEventHandler)file__(msg_panel_dismiss), (XtPointer) NULL);

	FirstArg(XtNlabel, "Clear");
	NextArg(XtNheight, 25);
	NextArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNfromVert, file__(msg_win));
	NextArg(XtNfromHoriz, file__(msg_dismiss));
	NextArg(XtNtop, XtChainBottom);
	NextArg(XtNbottom, XtChainBottom);
	NextArg(XtNleft, XtChainLeft);
	NextArg(XtNright, XtChainLeft);
	file__(msg_dismiss) = XtCreateManagedWidget("clear", commandWidgetClass,
				       file__(msg_panel), Args, ArgCount);
	XtAddEventHandler(file__(msg_dismiss), ButtonReleaseMask, False,
			  (XtEventHandler)clear_file_message, (XtPointer) NULL);

	XtPopup(file__(msg_popup), XtGrabNone);
	XSetWMProtocols(tool_d, XtWindow(file__(msg_popup)), &wm_delete_window, 1);
	/* insure that the most recent colormap is installed */
	set_cmap(XtWindow(file__(msg_popup)));
#endif
}

/*
   Make a string from the length.
   If the units are inches and there is a fractional value *and* 
   cur_gridunit == FRACT_UNIT, try to make a fraction out of it, e.g. 3/64
   If the units are area measure, pass square = True
*/

void
make_dimension_string(float length, char *str, BOOL square)
{
    float	 ulen;
    int		 ilen, ifeet=0, ifract, iexp;
    char	 tmpstr[100], *units, format[20];

    ulen = length/PIX_PER_INCH * appres.userscale;

    if (!appres.INCHES) {
		/* Metric */
		if (square)
			sprintf(str, "%.3f square %s", length / 
				(float)(PIX_PER_CM*PIX_PER_CM)*appres.userscale*appres.userscale,
				cur_fig_units);
		else {
			/* make a %.xf format where x is the precision the user wants */
			  sprintf(format, "%%.%df %%s", cur_dimline_prec);
			  sprintf(str, format, length / PIX_PER_CM*appres.userscale, cur_fig_units);
		}

    /* Inches */
    } else if (!square && (!display_fractions || (cur_gridunit != FRACT_UNIT))) {
			/* user doesn't want fractions or is in decimal units */
			/* make a %.xf format where x is the precision the user wants */
			sprintf(format, "%%.%df %%s", cur_dimline_prec);
			sprintf(str, format, ulen, cur_fig_units);
    } else if (!square) {
        ilen = (int) ulen;
        units = cur_fig_units;
        if (ilen == ulen) {
            /* integral */
            sprintf(str, "%d %s", ilen, cur_fig_units);
        } else {
            tmpstr[0] = '\0';
            /* first see if the user units are f, ft or feet */
            if (strcmp(cur_fig_units,"f") == 0 ||
                    strcmp(cur_fig_units,"ft") == 0 ||
                    strcmp(cur_fig_units,"feet") == 0) {
                /* yes, start with the feet part */
                ifeet = (int) ulen;
                if (abs(ifeet) >= 1)
                    sprintf(tmpstr,"%d %s ", ifeet, cur_fig_units);
                ulen = (ulen - ifeet) * (square? 144.0: 12.0);
                ilen = (int) ulen;
                units = "in";	/* next part is inches */
            }
            /* see if it is a multiple of 1/64 */
            ifract = (ulen - ilen) * 64.0;
            if (ifract != 0) {
                if (ilen + ifract/64.0 == ulen) {
                    for (iexp=64; iexp >= 2; ) {
                        if (ifract%2 == 0) {
                            ifract /= 2;
                            iexp /= 2;
                        } else
                            break;
                    }
                    sprintf(str,"%s%d-%d/%d %s", tmpstr, abs(ilen), abs(ifract), iexp, units);
                } else {
                    /* decimal not near any fraction */
                    /* make a %.xf format where x is the precision the user wants */
                    sprintf(format, "%%s%%.%df %%s", cur_dimline_prec);
                    sprintf(str, format, tmpstr, ulen, units);
                }
            } else {
                /* no fraction, whole inches */
                if (ifeet < 0)
                    ilen = abs(ilen);	/* if feet < 0, no need to report negative inches */
                sprintf(str,"%s%d %s", tmpstr, ilen, units);
            }
        }
    } else {
		/* square IP units */
		/* make a %.xf format where x is the precision the user wants */
		sprintf(format, "%%.%df square %%s", cur_dimline_prec);
		sprintf(str, format, length / 
				(float)(PIX_PER_INCH*PIX_PER_INCH)*appres.userscale*appres.userscale,
				cur_fig_units);
    }
}
