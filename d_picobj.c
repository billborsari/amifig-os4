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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/d_picobj.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "w_canvas.h"
#include "w_msgpanel.h"
#include "d_box.h"
#include "u_redraw.h"
#include "w_cursor.h"
#ifndef AMIFIG
#include "d_line.h"
#include "w_mousefun.h"
#include "e_edit.h"
#else
#include "f_picobj.h"
#include "u_search.h"
#include "z_pict.h"
#endif

/*************************** local declarations *********************/

static void init_picobj_drawing(int x, int y);
static void create_picobj(int x, int y);
static void cancel_picobj(void);



void
picobj_drawing_selected(void)
{
    set_mousefun((char *)_(msg_corner_point), "", "", "", "", "");
    canvas_locmove_proc = null_proc;
    canvas_leftbut_proc = init_picobj_drawing;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(crosshair_cursor);
	reset_action_on();
}

static void
init_picobj_drawing(int x, int y)
{
#ifdef AMIFIG

#define TOLERANCE ((int)((display_zoomscale < 20.0? 10: 14) * \
			PIX_PER_INCH/DISPLAY_PIX_PER_INCH/display_zoomscale))
            
	int px, py;
	F_line     *found_picobj = NULL;

	if (PictureEditWinOn)
	/* Picture Edit window is already open */
	{
		/* In this case, cur_l is already initialised */
		picobj_drawing_selected();
	}
	else if((found_picobj = picobj_search(x, y, TOLERANCE, &px, &py)) != NULL)
	/* Check if an existing Picture Object was selected */
	{
		cur_l=found_picobj;
		z_setup_picturewin();
		picobj_drawing_selected();
	}
	else
#endif
	{
		init_box_drawing(x, y);
        canvas_leftbut_proc     = create_picobj;
        canvas_rightbut_proc    = cancel_picobj;
	}
}

static void
cancel_picobj(void)
{
	elastic_box(fix_x, fix_y, cur_x, cur_y);
	picobj_drawing_selected();
    draw_mousefun_canvas();
}

static void
create_picobj(int x, int y)
{
	F_line	   *box;
	F_point	   *point;

    /* erase last lengths if appres.showlengths is true */
    erase_lengths();
	elastic_box(fix_x, fix_y, cur_x, cur_y);
	canvas_locmove_proc = null_proc;

	if ((point = create_point()) == NULL)
		return;

	point->x = fix_x;
	point->y = fix_y;
	point->next = NULL;

	if ((box = create_line()) == NULL) {
		free((char *) point);
		return;
	}
	box->type = T_PICTURE;
	box->style = SOLID_LINE;
	box->thickness = 1;
	box->pen_color = cur_pencolor;
	box->fill_color = DEFAULT;
	box->depth = cur_depth;
	box->pen_style = -1;
	box->join_style = 0;	/* not used */
	box->cap_style = 0;		/* not used */
	box->fill_style = UNFILLED;
	box->style_val = 0;

	if ((box->pic = create_pic()) == NULL) {
		free((char *) point);
		free((char *) box);
		return;
	}
#ifndef AMIFIG
    box->pic->new = True;		/* set new flag to delete if it user cancels edit operation */
    box->pic->pic_cache = NULL;
#endif
	box->pic->flipped = 0;
#ifndef AMIFIG
	box->pic->hw_ratio = 0.0;
	box->pic->pixmap = 0;
#endif
	box->pic->pix_width = 0;
	box->pic->pix_height = 0;
	box->pic->pix_rotation = 0;
	box->pic->pix_flipped = 0;
	box->points = point;
	append_point(fix_x, y, &point);
	append_point(x, y, &point);
	append_point(x, fix_y, &point);
	append_point(fix_x, fix_y, &point);
#ifdef AMIFIG
	box->pic->file[0] = '\0';
	box->pic->bit_size.x = 0;
	box->pic->bit_size.y = 0;
#endif
	add_line(box);
   /* draw it and anything on top of it */
    redisplay_line(box);
    put_msg((char *)_(msg_EnterPictName));  
#ifndef AMIFIG   
    edit_item(box, O_POLYLINE, 0, 0);
#else
	cur_l=box;
	z_setup_picturewin();
#endif
    picobj_drawing_selected();
    draw_mousefun_canvas();
}

#ifdef AMIFIG
void init_picobj_drawing2(int x, int y)
{
	init_box_drawing(x, y);
    canvas_leftbut_proc     = create_picobj;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = cancel_picobj;
}

int create_picobj2(int x, int y, char *filename)
{
	F_line	   *box;
	F_point	   *point;
	BOOL result;

	elastic_box(fix_x, fix_y, cur_x, cur_y);
	canvas_locmove_proc = null_proc;

	if ((point = create_point()) == NULL)
		return(-1);

	point->x = fix_x;
	point->y = fix_y;
	point->next = NULL;

	if ((box = create_line()) == NULL)
	{
		free((char *) point);
		return(-1);
	}
	box->type = T_PICTURE;
	box->style = SOLID_LINE;
	box->thickness = 1;
	box->pen_color = cur_pencolor;
	box->fill_color = DEFAULT;
	box->depth = cur_depth;
	box->pen_style = 0;
	box->join_style = 0;	/* not used */
	box->cap_style = 0;		/* not used */
	box->fill_style = UNFILLED;
	box->style_val = 0;
	box->radius = 0;

	if ((box->pic = create_pic()) == NULL)
	{
		free((char *) point);
		free((char *) box);
		return(-1);
	}
	box->pic->file[0] = '\0';
	box->pic->bit_size.x = 0;
	box->pic->bit_size.y = 0;
	box->pic->flipped = 0;
	box->pic->pix_width = 0;
	box->pic->pix_height = 0;
	box->pic->pix_rotation = 0;
	box->pic->pix_flipped = 0;

	box->points = point;
	append_point(fix_x, y, &point);
	append_point(x, y, &point);
	append_point(x, fix_y, &point);
	append_point(fix_x, fix_y, &point);

	strcpy(box->pic->file, filename);

	add_line(box);

	cur_l=box;

	result = read_picobj(cur_l, cur_l->pic, cur_l->pen_color);
	refresh_line_area(box, FALSE);

	picobj_drawing_selected();

	return(result? 0 : -2);
}
#endif
