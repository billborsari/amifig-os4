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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/d_ellipse.c $
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
#ifndef AMIFIG
#include "w_mousefun.h"
#endif
#include "u_markers.h"
#include "u_redraw.h"
#include "w_cursor.h"
#include "d_ellipse.h"
#include "w_msgpanel.h"

/*************************  local procedures  ********************/

void init_ellipsebyradius_drawing(int x, int y);
void init_ellipsebydiameter_drawing(int x, int y);
void init_circlebyradius_drawing(int x, int y);
void init_circlebydiameter_drawing(int x, int y);
void create_ellipsebydia(int x, int y);
void create_ellipsebyrad(int x, int y);
void create_circlebyrad(int x, int y);
void create_circlebydia(int x, int y);

void
ellipsebyradius_drawing_selected(void)
{
    canvas_locmove_proc = null_proc;
#ifndef AMIFIG
    canvas_leftbut_proc = init_circlebyradius_drawing;
    canvas_middlebut_proc = init_ellipsebyradius_drawing;
#else
    canvas_leftbut_proc     = init_ellipsebyradius_drawing;
    canvas_middlebut_proc   = null_proc;
#endif
    canvas_rightbut_proc = null_proc;
    set_cursor(crosshair_cursor);
#ifndef AMIFIG
    set_mousefun((char *)_(msg_Circle_center), (char *)_(msg_Ellipse_center), "", "", "", "");
#else
    set_mousefun((char *)_(msg_Circle_center), "", "", "", "", "");
#endif
	reset_action_on();
}


void
init_ellipsebyradius_drawing(int x, int y)
{
#ifndef AMIFIG
    cur_mode = F_ELLIPSE_BY_RAD;
#endif
	cur_x = fix_x = x;
	cur_y = fix_y = y;
	cur_angle = (float)cur_elltextangle/180.0*M_PI;
	center_marker(fix_x, fix_y);
#ifndef AMIFIG
    set_mousefun((char *)_(msg_Ellipse_corner), (char *)_(msg_Ellipse_corner), (char *)_(msg_Cancel), "", "", "");
#else
    set_mousefun((char *)_(msg_Ellipse_corner), "", "", "", "", "");
#endif
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_ebr;
    canvas_leftbut_proc = create_ellipsebyrad;
#ifndef AMIFIG
    canvas_middlebut_proc = create_ellipsebyrad;
#else
    canvas_middlebut_proc = null_proc;
#endif
    canvas_rightbut_proc = cancel_ellipsebyrad;
    set_cursor(null_cursor);
	elastic_ebr();
	set_action_on();
}

void
cancel_ellipsebyrad(void)
{
	elastic_ebr();
	center_marker(fix_x, fix_y);
#ifdef AMIFIG
	ellipsebyradius_drawing_selected();
#else
    circle_ellipse_byradius_drawing_selected();
#endif
    draw_mousefun_canvas();
}

void
create_ellipsebyrad(int x, int y)
{
	F_ellipse	*ellipse;

	elastic_ebr();
	center_marker(fix_x, fix_y);
	if((ellipse = create_ellipse()) == NULL)
		return;

	ellipse->type = T_ELLIPSE_BY_RAD;
	ellipse->style = cur_linestyle;
	ellipse->thickness = cur_linewidth;
	ellipse->style_val = cur_styleval * (cur_linewidth + 1) / 2;
	ellipse->angle = (float)cur_elltextangle/180.0*M_PI;
	ellipse->pen_color =cur_pencolor;
	ellipse->fill_color=cur_fillcolor;
	ellipse->depth = cur_depth;
	ellipse->pen_style = -1;
	ellipse->fill_style=cur_fillstyle;
	ellipse->direction = 1;
	ellipse->center.x = fix_x;
	ellipse->center.y = fix_y;
    ellipse->radiuses.x = abs(x - fix_x);
    ellipse->radiuses.y = abs(y - fix_y);
	ellipse->start.x = fix_x;
	ellipse->start.y = fix_y;
	ellipse->end.x = x;
	ellipse->end.y = y;
	ellipse->next = NULL;
	add_ellipse(ellipse);
    reset_action_on(); /* this signals redisplay_curobj() not to refresh */
    /* draw it and anything on top of it */
    redisplay_ellipse(ellipse);
#ifndef AMIFIG
    circle_ellipse_byradius_drawing_selected();
#else
    ellipsebyradius_drawing_selected();
#endif
    draw_mousefun_canvas();
}

void ellipsebydiameter_drawing_selected(void)
{
#ifndef AMIFIG
    set_mousefun((char *)_(msg_Circle_diameter), (char *)_(msg_Ellipse_corner), "", "", "", "");
#else
    set_mousefun((char *)_(msg_Circle_diameter), "", "", "", "", "");
#endif
    canvas_locmove_proc = null_proc;
#ifndef AMIFIG
    canvas_leftbut_proc = init_circlebydiameter_drawing;
    canvas_middlebut_proc = init_ellipsebydiameter_drawing;
#else
    canvas_leftbut_proc     = init_ellipsebydiameter_drawing;
    canvas_middlebut_proc   = null_proc;
#endif
    canvas_rightbut_proc = null_proc;
    set_cursor(crosshair_cursor);
	reset_action_on();
}

void
init_ellipsebydiameter_drawing(int x, int y)
{
	cur_x = fix_x = x;
	cur_y = fix_y = y;
	cur_angle = (float)cur_elltextangle/180.0*M_PI;
	center_marker(fix_x, fix_y);
#ifndef AMIFIG
    set_mousefun((char *)_(msg_final_corner), (char *)_(msg_final_corner), (char *)_(msg_Cancel), "", "", "");
#else
    set_mousefun((char *)_(msg_final_corner), "", (char *)_(msg_Cancel), "", "", "");
#endif
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_ebd;
    canvas_leftbut_proc = create_ellipsebydia;
#ifndef AMIFIG
    canvas_middlebut_proc = create_ellipsebydia;
#else
    canvas_middlebut_proc   = null_proc;
#endif
    canvas_rightbut_proc = cancel_ellipsebydia;
    set_cursor(null_cursor);
	elastic_ebd();
	set_action_on();
}

void
cancel_ellipsebydia(void)
{
	elastic_ebd();
	center_marker(fix_x, fix_y);
#ifdef AMIFIG
	ellipsebydiameter_drawing_selected();
#else
    circle_ellipse_bydiameter_drawing_selected();
#endif
    draw_mousefun_canvas();
}


void 
create_ellipsebydia(int x, int y)
{
	F_ellipse	*ellipse;

	elastic_ebd();
	center_marker(fix_x, fix_y);
	if((ellipse = create_ellipse()) == NULL)
		return;

	ellipse->type = T_ELLIPSE_BY_DIA;
	ellipse->style = cur_linestyle;
	ellipse->thickness = cur_linewidth;
	ellipse->style_val = cur_styleval * (cur_linewidth + 1) / 2;
	ellipse->angle = (float)cur_elltextangle/180.0*M_PI;
	ellipse->pen_color =cur_pencolor;
	ellipse->fill_color=cur_fillcolor;
	ellipse->depth = cur_depth;
	ellipse->pen_style = -1;
	ellipse->fill_style=cur_fillstyle;
	ellipse->direction = 1;
	ellipse->center.x = (fix_x + x) / 2;
	ellipse->center.y = (fix_y + y) / 2;
	ellipse->radiuses.x = abs(ellipse->center.x - fix_x);
	ellipse->radiuses.y = abs(ellipse->center.y - fix_y);
	ellipse->start.x = fix_x;
	ellipse->start.y = fix_y;
	ellipse->end.x = x;
	ellipse->end.y = y;
	ellipse->next = NULL;
	add_ellipse(ellipse);
    reset_action_on(); /* this signals redisplay_curobj() not to refresh */
    /* draw it and anything on top of it */
    redisplay_ellipse(ellipse);
#ifdef AMIFIG
	ellipsebydiameter_drawing_selected();
#else
    circle_ellipse_bydiameter_drawing_selected();
#endif
    draw_mousefun_canvas();
}

/***************************  circle  section  ************************/

void circlebyradius_drawing_selected(void)
{
    canvas_locmove_proc     = null_proc;
    canvas_leftbut_proc     = init_circlebyradius_drawing;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = null_proc;
	set_cursor(arrow_cursor);
	reset_action_on();
    set_mousefun((char *)_(msg_Circle_center), "", "", "", "", "");
    draw_mousefun_canvas();
}


void
init_circlebyradius_drawing(int x, int y)
{
#ifndef AMIFIG
    cur_mode = F_CIRCLE_BY_RAD;
#endif
	cur_x = fix_x = x;
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
    set_mousefun((char *)_(msg_set_radius), "", (char *)_(msg_Cancel), "", "", "");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_cbr;
    canvas_leftbut_proc = create_circlebyrad;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = cancel_circlebyrad;
	set_cursor(null_cursor);
	elastic_cbr();
	set_action_on();
}

void
cancel_circlebyrad(void)
{
	elastic_cbr();
	center_marker(fix_x, fix_y);
#ifdef AMIFIG
	circlebyradius_drawing_selected();
#else
    circle_ellipse_byradius_drawing_selected();
#endif
    draw_mousefun_canvas();
}

void
create_circlebyrad(int x, int y)
{
	F_ellipse	*c;
	double		rx, ry;

	elastic_cbr();
	center_marker(fix_x, fix_y);
	if((c = create_ellipse()) == NULL)
		return;

	c->type = T_CIRCLE_BY_RAD;
	c->style = cur_linestyle;
	c->thickness = cur_linewidth;
	c->style_val = cur_styleval * (cur_linewidth + 1) / 2;
	c->angle = 0.0;
	c->pen_color =cur_pencolor;
	c->fill_color=cur_fillcolor;
	c->depth = cur_depth;
	c->pen_style = -1;
	c->fill_style=cur_fillstyle;
	c->direction = 1;
	c->center.x = fix_x;
	c->center.y = fix_y;
	rx = fix_x - x;
    ry = fix_y - y;
	c->radiuses.x = c->radiuses.y = round(sqrt(rx * rx + ry * ry));
	c->start.x = fix_x;
	c->start.y = fix_y;
	c->end.x = x;
	c->end.y = y;
	c->next = NULL;
	add_ellipse(c);
    reset_action_on(); /* this signals redisplay_curobj() not to refresh */
    /* draw it and anything on top of it */
    redisplay_ellipse(c);
#ifdef AMIFIG
	circlebyradius_drawing_selected();
#else
    circle_ellipse_byradius_drawing_selected();
#endif
    draw_mousefun_canvas();
}

void circlebydiameter_drawing_selected(void)
{
    canvas_locmove_proc     = null_proc;
    canvas_leftbut_proc     = init_circlebydiameter_drawing;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = null_proc;
	set_cursor(arrow_cursor);
	reset_action_on();
    set_mousefun((char *)_(msg_Circle_diameter), "", "", "", "", "");
    draw_mousefun_canvas();
}


void
init_circlebydiameter_drawing(int x, int y)
{
    cur_mode = F_CIRCLE_BY_DIA;
	cur_x = fix_x = x;
	cur_y = fix_y = y;
	center_marker(fix_x, fix_y);
    set_mousefun((char *)_(msg_final_point), "", (char *)_(msg_Cancel), "", "", "");
    draw_mousefun_canvas();
    canvas_locmove_proc = resizing_cbd;
    canvas_leftbut_proc = create_circlebydia;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = cancel_circlebydia;
    set_cursor(null_cursor);
	elastic_cbd();
	set_action_on();
}

void 
cancel_circlebydia(void)
{
	elastic_cbd();
	center_marker(fix_x, fix_y);
#ifdef AMIFIG
	circlebydiameter_drawing_selected();
#else
    circle_ellipse_bydiameter_drawing_selected();
#endif
    draw_mousefun_canvas();
}

void
create_circlebydia(int x, int y)
{
	F_ellipse	*c;
	double		rx, ry;

	elastic_cbd();
	center_marker(fix_x, fix_y);
	if((c = create_ellipse()) == NULL)
		return;

	c->type = T_CIRCLE_BY_DIA;
	c->style = cur_linestyle;
	c->thickness = cur_linewidth;
	c->style_val = cur_styleval * (cur_linewidth + 1) / 2;
	c->angle = 0.0;
	c->pen_color =cur_pencolor;
	c->fill_color=cur_fillcolor;
	c->depth = cur_depth;
	c->pen_style = -1;
	c->fill_style=cur_fillstyle;
	c->direction = 1;
    c->center.x = round((fix_x + x) / 2);
    c->center.y = round((fix_y + y) / 2);
    rx = x - c->center.x;
    ry = y - c->center.y;
    c->radiuses.x = c->radiuses.y = round(sqrt(rx * rx + ry * ry));
	c->start.x = fix_x;
	c->start.y = fix_y;
	c->end.x = x;
	c->end.y = y;
	c->next = NULL;
    add_ellipse(c);
    reset_action_on(); /* this signals redisplay_curobj() not to refresh */
    /* draw it and anything on top of it */
    redisplay_ellipse(c);
#ifdef AMIFIG
	circlebydiameter_drawing_selected();
#else
    circle_ellipse_bydiameter_drawing_selected();
#endif
    draw_mousefun_canvas();
}
