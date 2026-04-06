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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_glue.c $
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
#include "u_bound.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_search.h"
#include "u_undo.h"
#include "w_canvas.h"

#ifndef AMIFIG
#include "d_line.h"
#include "f_read.h"
#include "u_draw.h"
#include "w_layers.h"
#include "w_mousefun.h"
#else
#include "e_glue.h"
#endif
#include "w_msgpanel.h"

#include "d_box.h"
#include "u_markers.h"
#include "w_cursor.h"

static void create_compoundobject(int x, int  y);
#ifndef AMIFIG
static void cancel_tag_region(void);
#endif
static void init_tag_region(int x, int y);
#ifndef AMIFIG
static void tag_region(int x, int y);
static void tag_object(F_line *p, int type, int x, int y, int px, int py);
#endif
static void get_arc(F_arc **list);
static void sel_arc(int xmin, int ymin, int xmax, int ymax);
static void get_compound(F_compound **list);
static void sel_compound(int xmin, int ymin, int xmax, int ymax);
static void get_ellipse(F_ellipse **list);
static void sel_ellipse(int xmin, int ymin, int xmax, int ymax);
static void get_line(F_line **list);
static void sel_line(int xmin, int ymin, int xmax, int ymax);
static void get_spline(F_spline **list);
static void sel_spline(int xmin, int ymin, int xmax, int ymax);
static void get_text(F_text **list);
static void sel_text(int xmin, int ymin, int xmax, int ymax);


void tag_obj_in_region(int xmin, int ymin, int xmax, int ymax);
int compose_compound (F_compound *c);

void
compound_selected(void)
{
    set_mousefun((char *)_(msg_tag_object), (char *)_(msg_tag_region), (char *)_(msg_compound_tagged), 
			(char *)_(msg_Locate_Object), "", "");
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(tag_object);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = init_tag_region;
    canvas_rightbut_proc = create_compoundobject;
    set_cursor(pick9_cursor);
    reset_action_on();
}


void
tag_object(F_line *p, int type, int x, int y, int px, int py)
{
    switch (type) {
    case O_COMPOUND:
        cur_c = (F_compound *) p;
        toggle_compoundhighlight(cur_c);
        cur_c->tagged = 1 - cur_c->tagged;
        break;
    case O_POLYLINE:
        cur_l = (F_line *) p;
        toggle_linehighlight(cur_l);
        cur_l->tagged = 1 - cur_l->tagged;
        break;
    case O_TXT:
        cur_t = (F_text *) p;
        toggle_texthighlight(cur_t);
        cur_t->tagged = 1 - cur_t->tagged;
        break;
    case O_ELLIPSE:
        cur_e = (F_ellipse *) p;
        toggle_ellipsehighlight(cur_e);
        cur_e->tagged = 1 - cur_e->tagged;
        break;
    case O_ARC:
        cur_a = (F_arc *) p;
        toggle_archighlight(cur_a);
        cur_a->tagged = 1 - cur_a->tagged;
        break;
    case O_SPLINE:
        cur_s = (F_spline *) p;
        toggle_splinehighlight(cur_s);
        cur_s->tagged = 1 - cur_s->tagged;
        break;
    default:
        return;
    }
}

void
init_tag_region(int x, int y)
{
	init_box_drawing(x, y);
    set_mousefun("", (char *)_(msg_final_corner), (char *)_(msg_Cancel), "", "", "");
    draw_mousefun_canvas();
    canvas_leftbut_proc = null_proc;
    canvas_middlebut_proc = tag_region;
    canvas_rightbut_proc    = cancel_tag_region;
}

void
cancel_tag_region(void)
{
	elastic_box(fix_x, fix_y, cur_x, cur_y);
    /* erase last lengths if appres.showlengths is true */
    erase_box_lengths();
	compound_selected();
    draw_mousefun_canvas();
}

void
tag_region(int x, int y)
{
	int		    xmin, ymin, xmax, ymax;

	elastic_box(fix_x, fix_y, cur_x, cur_y);
    /* erase last lengths if appres.showlengths is true */
    erase_box_lengths();
	xmin = min2(fix_x, x);
	ymin = min2(fix_y, y);
	xmax = max2(fix_x, x);
	ymax = max2(fix_y, y);
	tag_obj_in_region(xmin, ymin, xmax, ymax);
    compound_selected();
    draw_mousefun_canvas();
}

static void
create_compoundobject(int x, int y)
{
	F_compound	*c;

	if((c = create_compound()) == NULL)
		return;

	if(compose_compound(c) == 0) {
		free((char*)c);
		compound_selected();
	draw_mousefun_canvas();
	put_msg((char *)_(msg_EmptyCompoundIgnored));
		return;
	}
    /*
     * Make the bounding box exactly match the dimensions of the compound.
     */
    compound_bound(c, &c->nwcorner.x, &c->nwcorner.y,
		   &c->secorner.x, &c->secorner.y);

	/* if zero width or height in the compound, adjust to next positioning
	   grid point or a few pixels if positioning grid is "ANY" */
	if(c->nwcorner.x == c->secorner.x) {
#ifndef AMIFIG
    	if (cur_pointposn != P_ANY) {
#else
		if(cur_pointposn != P_ANYONE) {
#endif
    	    c->secorner.x += posn_rnd[cur_gridunit][cur_pointposn];
    	    c->secorner.x = ceil_coords_x(c->secorner.x,c->secorner.y);
		}
	}
	if(c->nwcorner.y == c->secorner.y)	{
#ifndef AMIFIG
    	if (cur_pointposn != P_ANY) {
#else
		if(cur_pointposn != P_ANYONE) {
#endif
    	    c->secorner.y += posn_rnd[cur_gridunit][cur_pointposn];
    	    c->secorner.y = ceil_coords_y(c->secorner.x,c->secorner.y);
		}
	}
	c->next = NULL;
	clean_up();
	set_action(F_GLUE);
    toggle_markers_in_compound(c);
    list_add_compound(&fobjects.compounds, c);
    mask_toggle_compoundmarker(c);
	set_latestcompound(c);
	set_modifiedflag();
	compound_selected();
    draw_mousefun_canvas();
}

void tag_obj_in_region(int xmin, int ymin, int xmax, int ymax)
{
	sel_ellipse(xmin, ymin, xmax, ymax);
	sel_line(xmin, ymin, xmax, ymax);
	sel_spline(xmin, ymin, xmax, ymax);
	sel_text(xmin, ymin, xmax, ymax);
	sel_arc(xmin, ymin, xmax, ymax);
	sel_compound(xmin, ymin, xmax, ymax);
}


int compose_compound(F_compound *c)
{
	c->ellipses = NULL;
	c->lines = NULL;
	c->texts = NULL;
	c->splines = NULL;
	c->arcs = NULL;
    c->comments = NULL;
	c->compounds = NULL;
#ifndef AMIFIG
    /* defer updating of layer buttons until we've composed the entire compound */
    defer_update_layers = True;
#endif
	get_ellipse(&c->ellipses);
	get_line(&c->lines);
	get_spline(&c->splines);
	get_text(&c->texts);
	get_arc(&c->arcs);
	get_compound(&c->compounds);
#ifndef AMIFIG
    /* now update the layer buttons */
    defer_update_layers = False;
    update_layers();
#endif
	if(c->ellipses != NULL)
		return (1);
	if(c->splines != NULL)
		return (1);
	if(c->lines != NULL)
		return (1);
	if(c->texts != NULL)
		return (1);
	if(c->arcs != NULL)
		return (1);
	if(c->compounds != NULL)
		return (1);
	return (0);
}

static void
sel_ellipse(int xmin, int ymin, int xmax, int ymax)
{
	F_ellipse	   *e;

	for(e = fobjects.ellipses; e != NULL; e = e->next) {
	    if (!active_layer(e->depth))
	       continue;
		if(xmin > e->center.x - e->radiuses.x)
			continue;
		if(xmax < e->center.x + e->radiuses.x)
			continue;
		if(ymin > e->center.y - e->radiuses.y)
			continue;
		if(ymax < e->center.y + e->radiuses.y)
			continue;
		e->tagged = 1 - e->tagged;
		toggle_ellipsehighlight(e);
	}
}

static void
get_ellipse(F_ellipse **list)
{
    F_ellipse	   *e=NULL, *ee=NULL, *ellipse=NULL;

	for(e = fobjects.ellipses; e != NULL;) {
		if(!e->tagged) {
			ee = e;
			e = e->next;
			continue;
		}
#ifndef AMIFIG
	    remove_depth(O_ELLIPSE, e->depth);
#endif
		if(*list == NULL)
			*list = e;
		else
			ellipse->next = e;
		ellipse = e;
		if (e == fobjects.ellipses)
			e = fobjects.ellipses = fobjects.ellipses->next;
		else {
			e = ee->next = e->next;
		}
		ellipse->next = NULL;
	}
}

static void
sel_arc(int xmin, int ymin, int xmax, int ymax)
{
	F_arc	   *a;
	int		    urx, ury, llx, lly;

	for(a = fobjects.arcs; a != NULL; a = a->next) {
	    if (!active_layer(a->depth))
	       continue;
		arc_bound(a, &llx, &lly, &urx, &ury);
		if(xmin > llx)
			continue;
		if(xmax < urx)
			continue;
		if(ymin > lly)
			continue;
		if(ymax < ury)
			continue;
		a->tagged = 1 - a->tagged;
		toggle_archighlight(a);
	}
}

static void
get_arc(F_arc **list)
{
    F_arc	   *a=NULL, *arc=NULL, *aa=NULL;

	for(a = fobjects.arcs; a != NULL;) {
		if(!a->tagged) {
			aa = a;
			a = a->next;
			continue;
		}
#ifndef AMIFIG
	    remove_depth(O_ARC, a->depth);
#endif
		if(*list == NULL)
			*list = a;
		else
			arc->next = a;
		arc = a;
		if(a == fobjects.arcs)
			a = fobjects.arcs = fobjects.arcs->next;
		else
			a = aa->next = a->next;
		arc->next = NULL;
	}
}

static void
sel_line(int xmin, int ymin, int xmax, int ymax)
{
	F_line	   *l;
	F_point	   *p;
	int		    inbound;

	for(l = fobjects.lines; l != NULL; l = l->next)	{
        if (!active_layer(l->depth))
            continue;
		for(inbound = 1, p = l->points; p != NULL && inbound;
		        p = p->next) {
			inbound = 0;
			if(xmin > p->x)
				continue;
			if(xmax < p->x)
				continue;
			if(ymin > p->y)
				continue;
			if(ymax < p->y)
				continue;
			inbound = 1;
		}
		if(!inbound)
			continue;
		l->tagged = 1 - l->tagged;
		toggle_linehighlight(l);
	}
}

static void
get_line(F_line **list)
{
    F_line	   *line=NULL, *l=NULL, *ll=NULL;

	for(l = fobjects.lines; l != NULL;) {
		if(!l->tagged) {
			ll = l;
			l = l->next;
			continue;
		}
#ifndef AMIFIG
	    remove_depth(O_POLYLINE, l->depth);
#endif
		if(*list == NULL)
			*list = l;
		else
			line->next = l;
		line = l;
		if(l == fobjects.lines)
			l = fobjects.lines = fobjects.lines->next;
		else
			l = ll->next = l->next;
		line->next = NULL;
	}
}

static void
sel_spline(int xmin, int ymin, int xmax, int ymax)
{
	F_spline	   *s;
	int		    urx, ury, llx, lly;

	for(s = fobjects.splines; s != NULL; s = s->next) {
	    if (!active_layer(s->depth))
	       continue;
		spline_bound(s, &llx, &lly, &urx, &ury);
		if(xmin > llx)
			continue;
		if(xmax < urx)
			continue;
		if(ymin > lly)
			continue;
		if(ymax < ury)
			continue;
		s->tagged = 1 - s->tagged;
		toggle_splinehighlight(s);
	}
}

static void
get_spline(F_spline **list)
{
    F_spline	   *spline=NULL, *s=NULL, *ss=NULL;

	for(s = fobjects.splines; s != NULL;) {
		if(!s->tagged) {
			ss = s;
			s = s->next;
			continue;
		}
#ifndef AMIFIG
	    remove_depth(O_SPLINE, s->depth);
#endif
		if(*list == NULL)
			*list = s;
		else
			spline->next = s;
		spline = s;
		if(s == fobjects.splines)
			s = fobjects.splines = fobjects.splines->next;
		else
			s = ss->next = s->next;
		spline->next = NULL;
	}
}

static void
sel_text(int xmin, int ymin, int xmax, int ymax)
{
	F_text	   *t;
	int		    txmin, txmax, tymin, tymax;
    int         dum;

	for(t = fobjects.texts; t != NULL; t = t->next)	{
	    if (!active_layer(t->depth))
	       continue;
    	text_bound(t, &txmin, &tymin, &txmax, &tymax,
    			&dum,&dum,&dum,&dum,&dum,&dum,&dum,&dum);
		if(xmin > txmin || xmax < txmax ||
		        ymin > tymin || ymax < tymax)
			continue;
		t->tagged = 1 - t->tagged;
		toggle_texthighlight(t);
	}
}

static void
get_text(F_text **list)
{
    F_text	   *text=NULL, *t=NULL, *tt=NULL;

	for(t = fobjects.texts; t != NULL;)	{
		if(!t->tagged) {
			tt = t;
			t = t->next;
			continue;
		}
#ifndef AMIFIG
	    remove_depth(O_TXT, t->depth);
#endif
		if(*list == NULL)
			*list = t;
		else
			text->next = t;
		text = t;
		if(t == fobjects.texts)
			t = fobjects.texts = fobjects.texts->next;
		else
			t = tt->next = t->next;
		text->next = NULL;
	}
}

static void
sel_compound(int xmin, int ymin, int xmax, int ymax)
{
	F_compound	   *c;

	for(c = fobjects.compounds; c != NULL; c = c->next)	{
#ifndef AMIFIG
	    if (!any_active_in_compound(c))
	       continue;
#endif
		if(xmin > c->nwcorner.x)
			continue;
		if(xmax < c->secorner.x)
			continue;
		if(ymin > c->nwcorner.y)
			continue;
		if(ymax < c->secorner.y)
			continue;
		c->tagged = 1 - c->tagged;
		toggle_compoundhighlight(c);
	}
}

static void
get_compound(F_compound **list)
{
    F_compound	   *compd=NULL, *c=NULL, *cc=NULL;

	for(c = fobjects.compounds; c != NULL;)	{
		if(!c->tagged) {
			cc = c;
			c = c->next;
			continue;
		}
#ifndef AMIFIG
	    remove_compound_depth(c);
#endif
		if(*list == NULL)
			*list = c;
		else
			compd->next = c;
		compd = c;
		if(c == fobjects.compounds)
			c = fobjects.compounds = fobjects.compounds->next;
		else
			c = cc->next = c->next;
		compd->next = NULL;
	}
}
