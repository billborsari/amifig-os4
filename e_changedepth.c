/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 * Parts Copyright (c) 1995 by C. Blanc and C. Schlick
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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_changedepth.c $
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
#include "w_msgpanel.h"
#include "w_cursor.h"
#include "u_search.h"
#include "u_redraw.h"

static void init_depthfront(F_line *obj, int type, int x, int y, F_point *p, F_point *q);
static void init_depthback(F_line *obj, int type, int x, int y, F_point *p, F_point *q);
void set_depth(int x, int y);


void changedepth_selected(void)
{
    set_mousefun((char *)_(msg_Put_Front), "", (char *)_(msg_Put_Back), (char *)_(msg_Locate_Object), "", (char *)_(msg_Locate_Object));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(init_depthfront);
    init_searchproc_right(init_depthback);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = object_search_right;
    set_cursor(pick15_cursor);
    reset_action_on();
}

static void line_changedepth(F_line *line, int deltadepth)
{
	line->depth += deltadepth;
	line->depth = min2(line->depth, MAX_DEPTH);
	line->depth = max2(line->depth, MIN_DEPTH);			
}

static void text_changedepth(F_text *text, int deltadepth)
{
	text->depth += deltadepth;
	text->depth = min2(text->depth, MAX_DEPTH);
	text->depth = max2(text->depth, MIN_DEPTH);
}

static void ellipse_changedepth(F_ellipse *ell, int deltadepth)
{
	ell->depth += deltadepth;
	ell->depth = min2(ell->depth, MAX_DEPTH);
	ell->depth = max2(ell->depth, MIN_DEPTH);			
}

static void arc_changedepth(F_arc *arc, int deltadepth)
{
	arc->depth += deltadepth;
	arc->depth = min2(arc->depth, MAX_DEPTH);
	arc->depth = max2(arc->depth, MIN_DEPTH);			
}

static void spline_changedepth(F_spline *spline, int deltadepth)
{
	spline->depth += deltadepth;
	spline->depth = min2(spline->depth, MAX_DEPTH);
	spline->depth = max2(spline->depth, MIN_DEPTH);
}

static void compound_changedepth(F_compound *compound, int deltadepth)
{
	F_line		*l;
	F_arc		*a;
	F_ellipse	*e;
	F_spline	*s;
	F_text		*t;
	F_compound	*c1;

	for (l = compound->lines; l != NULL; l = l->next)
		line_changedepth(l, deltadepth);
	for (a = compound->arcs; a != NULL; a = a->next)
		arc_changedepth(a, deltadepth);
	for (e = compound->ellipses; e != NULL; e = e->next)
		ellipse_changedepth(e, deltadepth);
	for (s = compound->splines; s != NULL; s = s->next)
		spline_changedepth(s, deltadepth);
	for (t = compound->texts; t != NULL; t = t->next)
		text_changedepth(t, deltadepth);
	for (c1 = compound->compounds; c1 != NULL; c1 = c1->next)
		compound_changedepth(c1, deltadepth);
}

static void
change_depth(F_line *obj, int type, int deltadepth)
{
	switch (type)
	{
		case O_COMPOUND:
			compound_changedepth((F_compound *)obj, deltadepth);
			redisplay_compound((F_compound *)obj);
			break;
		case O_POLYLINE:
			line_changedepth((F_line *)obj, deltadepth);
			redisplay_line((F_line *)obj);
			break;
		case O_TXT:
			text_changedepth((F_text *)obj, deltadepth);
			redisplay_text((F_text *)obj);
			break;
		case O_ELLIPSE:
			ellipse_changedepth((F_ellipse *)obj, deltadepth);
			redisplay_ellipse((F_ellipse *)obj);
			break;
		case O_ARC:
			arc_changedepth((F_arc *)obj, deltadepth);
			redisplay_arc((F_arc *)obj);
			break;
		case O_SPLINE:
			spline_changedepth((F_spline *)obj, deltadepth);
			redisplay_spline((F_spline *)obj);
			break;
		default:
			return;
	}
}

static void
init_depthfront(F_line *obj, int type, int x, int y, F_point *p, F_point *q)
{
	int mindepth = MIN_DEPTH, maxdepth = MAX_DEPTH, cmindepth = MIN_DEPTH, cmaxdepth = MAX_DEPTH, deltadepth = -1;

	if ((PeekQualifier() & IEQUALIFIER_CONTROL))
	{
		compound_depths((F_compound *)&fobjects, &maxdepth, &mindepth);
		if (type == O_COMPOUND)
		{
			compound_depths((F_compound *)obj, &cmaxdepth, &cmindepth);
			deltadepth = mindepth - cmaxdepth - 1;
		}	
		else
			deltadepth = mindepth - obj->depth - 1;
    }
	
    change_depth(obj, type, deltadepth);
	changedepth_selected();
}

static void
init_depthback(F_line *obj, int type, int x, int y, F_point *p, F_point *q)
{
	int mindepth = MIN_DEPTH, maxdepth = MAX_DEPTH, cmindepth = MIN_DEPTH, cmaxdepth = MAX_DEPTH, deltadepth = 1;

	if ((PeekQualifier() & IEQUALIFIER_CONTROL))
	{
		compound_depths((F_compound *)&fobjects, &maxdepth, &mindepth);
		if (type == O_COMPOUND)
		{
			compound_depths((F_compound *)obj, &cmaxdepth, &cmindepth);
			deltadepth = maxdepth - cmindepth + 1;
		}	
		else
			deltadepth = maxdepth - obj->depth + 1;
    }
	
    change_depth(obj, type, deltadepth);
	changedepth_selected();
}
