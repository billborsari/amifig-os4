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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_arrow.c $
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
#include "u_search.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "u_redraw.h"
#include "w_cursor.h"
#ifndef AMIFIG
#include "e_arrow.h"
#include "u_draw.h"
#include "w_mousefun.h"
#else
#include "w_msgpanel.h"
#endif

static void add_arrow_head(F_line *obj, int type, int x, int y, F_point *p, F_point *q);
#ifndef AMIFIG
static void	delete_arrow_head(F_line *obj, int type, int x, int y, F_point *p, F_point *q);
#endif
void add_linearrow(F_line *line, F_point *prev_point, F_point *selected_point);
void add_arcarrow(F_arc *arc, int point_num);
void add_splinearrow(F_spline *spline, F_point *prev_point, F_point *selected_point);



void 
arrow_head_selected(void)
{
    set_mousefun((char *)_(msg_add_arrow), (char *)_(msg_delete_arrow), "", (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(add_arrow_head);
    init_searchproc_middle(add_arrow_head);
    canvas_leftbut_proc = point_search_left;
    canvas_middlebut_proc = point_search_middle;
    canvas_rightbut_proc = null_proc;
    set_cursor(pick9_cursor);
    reset_action_on();
}

static void 
add_arrow_head(F_line *obj, int type, int x, int y, F_point *p, F_point *q)
{
	switch (type) {
	case O_POLYLINE:
		cur_l = (F_line *) obj;
		add_linearrow(cur_l, p, q);
		break;
	case O_SPLINE:
		cur_s = (F_spline *) obj;
		add_splinearrow(cur_s, p, q);
		break;
	case O_ARC:
		cur_a = (F_arc *) obj;
		/* dirty trick - arc point number is stored in p */
		add_arcarrow(cur_a, (int)((IPTR)p));
		break;
	}
}
#ifndef AMIFIG
static void
delete_arrow_head(F_line *obj, int type, int x, int y, F_point *p, F_point *q)
{
    switch (type) {
        case O_POLYLINE:
        	cur_l = (F_line *) obj;
        	delete_linearrow(cur_l, p, q);
        	break;
        case O_SPLINE:
        	cur_s = (F_spline *) obj;
        	delete_splinearrow(cur_s, p, q);
        	break;
        case O_ARC:
        	cur_a = (F_arc *) obj;
        	/* dirty trick - arc point number is stored in p */
        	delete_arcarrow(cur_a, (int) p);
        	break;
    }
}
#endif
void
add_linearrow(F_line *line, F_point *prev_point, F_point *selected_point)
{
	BOOL remove=FALSE;

	if(line->type == T_POLYGON || line->type == T_BOX || line->type == T_ARCBOX) return;
	if(line->points->next == NULL)
        return;	/* A single point line */

	refresh_line_area(line, TRUE);

	if(prev_point == NULL) {                /*  selected_point is the first point */
		if(line->back_arrow)
		{
			free(line->back_arrow);
			line->back_arrow=NULL;
			remove=TRUE;
		}
		else
			line->back_arrow = add_arrow(line->thickness);
	} else if(selected_point->next == NULL)  { /* forward arrow */
		if(line->for_arrow)
		{
			free(line->for_arrow);
			line->for_arrow=NULL;
			remove=TRUE;
		} else
			line->for_arrow = add_arrow(line->thickness);
	}

	clean_up();
	set_last_prevpoint(prev_point);
	set_last_selectedpoint(selected_point);
	set_latestline(line);

	if(remove)
		set_action_object(F_DELETE_ARROW_HEAD, O_POLYLINE);
	else
		set_action_object(F_ADD_ARROW_HEAD, O_POLYLINE);

	refresh_line_area(line, FALSE);
	set_modifiedflag();
}

void 
add_arcarrow(F_arc *arc, int point_num)
{
	BOOL remove=FALSE;

    /* only allow arrowheads on open arc */
    if (arc->type == T_PIE_WEDGE_ARC)
        return;

	refresh_arc_area(arc, TRUE);

	if(point_num == 0)  {                    /*  backward arrow  */
		if(arc->back_arrow)
		{
			free(arc->back_arrow);
			arc->back_arrow=NULL;
			remove=TRUE;
		} else
			arc->back_arrow = add_arrow(arc->thickness);

	} else if (point_num == 2)  {/*  for_arrow  */

		if(arc->for_arrow)
		{
			free(arc->for_arrow);
			arc->for_arrow=NULL;
			remove=TRUE;
		}
		else
			arc->for_arrow = add_arrow(arc->thickness);


	}

	clean_up();
	set_last_arcpointnum(point_num);
	set_latestarc(arc);

	if(remove)
		set_action_object(F_DELETE_ARROW_HEAD, O_ARC);
	else
		set_action_object(F_ADD_ARROW_HEAD, O_ARC);

	refresh_arc_area(arc, FALSE);
	set_modifiedflag();
}

void
add_splinearrow(F_spline *spline, F_point *prev_point, F_point *selected_point)
{
	BOOL remove=FALSE;

	if(closed_spline(spline)) return;

	refresh_spline_area(spline, TRUE);

	if(prev_point == NULL)                  /* add backward arrow */
	{
		if(spline->back_arrow)
		{
			free(spline->back_arrow);
			spline->back_arrow=NULL;
			remove=TRUE;
		}
		else
			spline->back_arrow = add_arrow(spline->thickness);
	}
	else if (selected_point->next == NULL)  /* add forward arrow */
	{
		if(spline->for_arrow)
		{
			free(spline->for_arrow);
			spline->for_arrow=NULL;
			remove=TRUE;
		}
		else
			spline->for_arrow = add_arrow(spline->thickness);
	}

	clean_up();
	set_last_prevpoint(prev_point);
	set_last_selectedpoint(selected_point);
	set_latestspline(spline);

	if(remove)
		set_action_object(F_DELETE_ARROW_HEAD, O_SPLINE);
	else
		set_action_object(F_ADD_ARROW_HEAD, O_SPLINE);

	refresh_spline_area(spline, FALSE);
	set_modifiedflag();
}





