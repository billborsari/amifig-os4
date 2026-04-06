/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 * This part Copyright (c) 1999-2002 Alexander Durner
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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_measure.c $
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
#include "u_elastic.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_msgpanel.h"
#include "u_geom.h"
#include "u_markers.h"
#include "w_cursor.h"
#include "w_drawprim.h"
#ifndef AMIFIG
#include "u_create.h"
#include "u_list.h"
#include "w_mousefun.h"
#include "w_setup.h"
#include "w_indpanel.h"
#endif

/* Measuring angles, lengths and areas */


static void init_anglemeas_object(char *p, int type, int x, int y, F_point *pp, F_point *pq);
static void init_anglemeas_object_m(char *p, int type, int x, int y, F_point *pp, F_point *pq);
static void init_anglemeas_object_r(char *p, int type, int x, int y, F_point *pp, F_point *pq);
static void init_anglemeas_threepoints(int px, int py);

static void anglemeas_second(int x, int y);
static void anglemeas_third(int x, int y);
static void anglemeas_third_l(int x, int y);
static void anglemeas_third_m(int x, int y);
static void cancel_anglemeas(void);
static void anglemeas_line(F_line *l, F_point *p);
static void anglemeas_arc(F_arc *a);
static void angle_msg(double value, char *msgtext);
static void angle_save(double value);

static void init_lenmeas_object(char *p, int type, int x, int y, int px, int py);
static void init_lenmeas_object_l(char *p, int type, int x, int y, int px, int py);
static void init_lenmeas_object_m(char *p, int type, int x, int y, int px, int py);
static void clear_lenmeas_memory(void);

static void init_areameas_object(char *p, int type, int x, int y, int px, int py);
static void init_areameas_object_l(char *p, int type, int x, int y, int px, int py);
static void init_areameas_object_m(char *p, int type, int x, int y, int px, int py);
static void clear_areameas_memory(int x, int y, int arg);

static void freehand_line_nomsg(int x, int y);

static F_point pa, pb, pc;
static int np;
static int save_objmask;

static int save_rotnangle;

static int save_len;
static float total_len = 0.0;

static int save_area;
static int signed_area = 0;
static float total_area = 0.0;

/***************************************************************************
 ANGLE MEASURING 
 ***************************************************************************/

#ifdef AMIFIG
#include "z_edit.h"

static void
set_and_show_rotnangle(float degr)
{
	cur_rotnangle = (int)degr;
	z_refreshAttributes();
}
#endif


void anglemeas_selected(void)
{
    set_mousefun((char *)_(msg_first_point), (char *)_(msg_select_save), (char *)_(msg_select_object), "", (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_middle(init_anglemeas_object_m);
    init_searchproc_right(init_anglemeas_object_r);
    canvas_leftbut_proc = init_anglemeas_threepoints;
    canvas_middlebut_proc = point_search_middle;
    canvas_rightbut_proc = point_search_right;
    set_cursor(pick9_cursor);
    reset_action_on();
}

static void
angle_msg(double value, char *msgtext)
{
    put_msg((char *)_(msg_Degrees), msgtext, value*180.0/M_PI);
}

static void
angle_save(double value)
{
    float degr;
    if (!save_rotnangle)
      return;
    degr = (float)fabs(value*180.0/M_PI);
    if (degr > 180.0)
      degr = 360.0 - degr;
    set_and_show_rotnangle(degr);
}

/* OBJECT ANGLE MEASURING */

static void
init_anglemeas_object_m(char *p, int type, int x, int y, F_point *pp, F_point *pq)
{
    save_rotnangle = 1;
    init_anglemeas_object(p, type, x, y, pp, pq);
}

static void
init_anglemeas_object_r(char *p, int type, int x, int y, F_point *pp, F_point *pq)
{
    save_rotnangle = 0;
    init_anglemeas_object(p, type, x, y, pp, pq);
}

static void
init_anglemeas_object(char *p, int type, int x, int y, F_point *pp, F_point *pq)
{
    switch(type) {
    case O_POLYLINE:
        cur_l = (F_line*)p; 
        anglemeas_line(cur_l, pq); /* do_point_search returns `near' point in *q */
        break;
    case O_ARC:
        cur_a = (F_arc*)p;
        anglemeas_arc(cur_a); /* point doesn't matter */
        break;
    default:
        return;
    }
}

/* line angle */

static void 
anglemeas_line(F_line *l, F_point *p)
{
   double lineangle;
   if (compute_line_angle(l, p, &lineangle)) {
     /* lineangle in 0..2*PI */
     if (lineangle > M_PI)
       lineangle -= 2*M_PI;
     angle_msg(lineangle, "Angle at polyline corner: ");
     angle_save(lineangle);
   }
   else
     put_msg((char *)_(msg_AngleEndPtError));     
}

/* arc angle */

static void 
anglemeas_arc(F_arc *a)
{
   double dang;
   if (compute_arc_angle(a, &dang)) {
     angle_msg(dang, "Angle at arc center: ");
     angle_save(dang);
   }
   else
     put_msg((char *)_(msg_AngleError));
}

/* THREE-POINT ANGLE MEASURING */

static void
init_anglemeas_threepoints(int px, int py)
{
    set_cursor(arrow_cursor);
    set_mousefun((char *)_(msg_angle_tip), "", (char *)_(msg_Cancel), "", "", (char *)_(msg_Locate_Object));
    draw_mousefun_canvas();    
    canvas_rightbut_proc = cancel_anglemeas;
    pa.x = fix_x = cur_x = px;
    pa.y = fix_y = cur_y = py;
    np = 1;
    canvas_locmove_proc = freehand_line_nomsg;
    canvas_ref_proc = elastic_line;
    canvas_leftbut_proc = anglemeas_second;
    canvas_middlebut_proc = null_proc;
    elastic_line();
    set_temp_cursor(null_cursor);
    save_objmask = cur_objmask;
    update_markers(M_NONE);
    set_action_on();
}

static void
anglemeas_second(int x, int y)
{
   if (x == fix_x && y == fix_y)
       return;
   if (np == 1) { /* should always be! */
       set_mousefun((char *)_(msg_final_point),(char *)_(msg_save_angle),(char *)_(msg_Cancel), "", "", "");
       draw_mousefun_canvas();
       canvas_leftbut_proc = anglemeas_third_l;
       canvas_middlebut_proc = anglemeas_third_m;
   }
   elastic_line();
   cur_x = x;
   cur_y = y;
   elastic_line();
   pb.x = fix_x = x;
   pb.y = fix_y = y;
   np = 2;
   elastic_line();
}

static void
anglemeas_third_l(int x, int y)
{
   save_rotnangle = 0;
   anglemeas_third(x, y);
}

static void
anglemeas_third_m(int x, int y)
{
   save_rotnangle = 1;
   anglemeas_third(x, y);
}

static void
anglemeas_third(int x, int y)
{
   double uangle;
   if (x == fix_x && y == fix_y)
       return;
   elastic_line();
   pc.x = cur_x = x;
   pc.y = cur_y = y;
   elastic_line();
   /* erase? */
   pw_vector(canvas_win, pa.x, pa.y, pb.x, pb.y, INV_PAINT, 1, RUBBER_LINE, 0.0, DEFAULT);   
   pw_vector(canvas_win, pb.x, pb.y, pc.x, pc.y, INV_PAINT, 1, RUBBER_LINE, 0.0, DEFAULT);   
   if (compute_3p_angle(&pa, &pb, &pc, &uangle)) {
      if (uangle > M_PI)
        uangle -= 2*M_PI;
      angle_msg(uangle, "Angle defined by three points: ");
      angle_save(uangle);
   }
   else
       put_msg((char *)_(msg_AngleError));   
   update_markers(save_objmask);
   anglemeas_selected();
   draw_mousefun_canvas();
}

static void
cancel_anglemeas(void)
{
    elastic_line();
    if (np == 2) {
	/* erase initial part of line */
	cur_x = pa.x;
	cur_y = pa.y;
	elastic_line();
    }
    update_markers(save_objmask);
    anglemeas_selected();
    draw_mousefun_canvas();
}

static void
freehand_line_nomsg(int x, int y)
{
    elastic_line();
    cur_x = x;
    cur_y = y;
    elastic_line();
}

/***************************************************************************
 LENGTH MEASURING 
 ***************************************************************************/

void lenmeas_selected(void)
{
    set_mousefun((char *)_(msg_select_object), (char *)_(msg_select_add), (char *)_(msg_reset_to_0), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(init_lenmeas_object_l);
    init_searchproc_middle(init_lenmeas_object_m);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = clear_lenmeas_memory;
    set_cursor(pick9_cursor);
    /*    force_nopositioning(); */
    reset_action_on();
}

static void 
init_lenmeas_object(char *p, int type, int x, int y, int px, int py)
{
    float	    len;
    double	    a,b,z;
    int		    ok;
    char	   *msgtext;

    ok = 0;

    switch (type) {
      case O_POLYLINE:
	cur_l = (F_line*) p;
	(void) compute_poly_length(cur_l, &len);
	if (cur_l->type == T_BOX || cur_l->type == T_PICTURE)
	    msgtext = "box";
	else if (cur_l->type == T_ARCBOX) {
	    msgtext = "arcbox";
	    /* subtract radius corners */
	           len -= M_PI * cur_l->radius*2.0*ZOOM_FACTOR;
	} else if (cur_l->type == T_POLYGON)
	    msgtext = "polygon";
	else
	    msgtext = "polyline";
	ok = 1;
	break;

      case O_ARC:
	cur_a = (F_arc*) p;
	if (compute_arc_length(cur_a, &len)) {
	    msgtext = "arc";
	    ok = 1;
	}
	break;

      case O_ELLIPSE:
	cur_e = (F_ellipse*) p;
	/* ellipse or circle? */
	if (cur_e->radiuses.x == cur_e->radiuses.y) {
	    msgtext = "circle";
	    len = M_PI * cur_e->radiuses.x * 2.0;
	} else {
	    msgtext = "ellipse";
	    /* from The Math Forum (mathforum.org) */
	    a = cur_e->radiuses.x;
	    b = cur_e->radiuses.y;
	    z = (a-b)/(a+b);
	    len = M_PI * (a+b)*(1.0 + 3.0*z*z/(10.0 + sqrt(4.0-3.0*z*z)));
	}
	ok = 1;
	break;

      default:
	break;
    }
    if (ok) {
	if (save_len) {
	    total_len += len;
	    lenmeas_msg(msgtext, len, total_len);
	}
	else
	    lenmeas_msg(msgtext, len, -1.0);
    } else
	put_msg((char *)_(msg_LengthError));
}

static void 
init_lenmeas_object_l(char *p, int type, int x, int y, int px, int py)
{
  save_len = 0;
  init_lenmeas_object(p, type, x, y, px, py);
}

static void 
init_lenmeas_object_m(char *p, int type, int x, int y, int px, int py)
{
  save_len = 1;
  init_lenmeas_object(p, type, x, y, px, py);
}

static void
clear_lenmeas_memory(void)
{
   total_len = 0.0;
   put_msg((char *)_(msg_LengthReset));
}   

/***************************************************************************
 AREA MEASURING 
 ***************************************************************************/

void areameas_selected(void)
{
    set_mousefun((char *)_(msg_select_object), (char *)_(msg_select_add), (char *)_(msg_reset_to_0), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_reset_to_0));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(init_areameas_object_l);
    init_searchproc_middle(init_areameas_object_m);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = object_search_middle;
    canvas_rightbut_proc = clear_areameas_memory;
    set_cursor(pick9_cursor);
    /*    force_nopositioning(); */
    reset_action_on();
}


static void 
init_areameas_object(char *p, int type, int x, int y, int px, int py)
{
    float	    area;
    int		    ok;
    char	   *msgtext;

    ok = 0;

    switch (type) {
      case O_POLYLINE:
	cur_l = (F_line*) p;
	if (1) {
	    compute_poly_area(cur_l, &area);
	    if (cur_l->type == T_BOX)
		msgtext = "box";
	    else if (cur_l->type == T_ARCBOX) {
	        /* subtract radius corners from area */
		area -= M_PI*(cur_l->radius*cur_l->radius*ZOOM_FACTOR*ZOOM_FACTOR);
		msgtext = "arcbox";
	    } else {
		msgtext = "polygon";
	    }
	    ok = 1;
	}
	break;

      case O_ARC:
	cur_a = (F_arc*) p;
	if (compute_arc_area(cur_a, &area)) {
	    if (cur_a->type == T_OPEN_ARC)
		msgtext = "open arc";
	    else
		msgtext = "sector";
	    ok = 1;
	}
	break;

      case O_ELLIPSE:
	cur_e = (F_ellipse*) p;
	if (compute_ellipse_area(cur_e, &area)) {
	    msgtext = "ellipse";
	    ok = 1;
	}
	break;

      default:
	break;
    }
    if (ok) {
	if (!signed_area)
	    area = fabs(area);
	if (save_area) {
	    total_area += area;
	    areameas_msg(msgtext, area, total_area, 1);
	} else
      areameas_msg(msgtext, area, -1.0, 0);
    } else
	put_msg((char *)_(msg_AreaError));
}

static void 
init_areameas_object_l(char *p, int type, int x, int y, int px, int py)
{
  save_area = 0;
  init_areameas_object(p, type, x, y, px, py);
}

static void 
init_areameas_object_m(char *p, int type, int x, int y, int px, int py)
{
  save_area = 1;
  init_areameas_object(p, type, x, y, px, py);
}

static void
clear_areameas_memory(int x, int y, int arg)
{
   total_area = 0.0;
   signed_area = (arg != 0);
   if (signed_area)
     put_msg((char *)_(msg_SignedAreaReset));
   else
     put_msg((char *)_(msg_AreaReset));
}   
