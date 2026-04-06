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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_editsfactor.c $
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
#include "u_list.h"
#include "u_markers.h"
#include "u_search.h"
#include "u_redraw.h"
#include "w_msgpanel.h"
#include "w_cursor.h"

static void init_modify_sfactor(F_line *obj, int type, int x, int y, F_point *p, F_point *q);
void moving_sfactor(int x, int y);
void set_sfactor(int x, int y);
void cancel_sfactor(void);
double saved_sfactor;

double calcsfact(int x, int y)
{
	double sfact;

	sfact = (double)(max2(-1200, min2(1200, x - fix_x))) / 1200.0;
	// ydif = (double)(max2(-1200, min2(1200, y - fix_y))) / 1200.0;
    
    return(sfact);
}

void editsfactor_selected(void)
{
    set_mousefun((char *)_(msg_Modify_SFactor), "", "", (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    init_searchproc_left(init_modify_sfactor);
    canvas_leftbut_proc = point_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(pick15_cursor);
    reset_action_on();
}

static void
init_modify_sfactor(F_line *obj, int type, int x, int y, F_point *p, F_point *q)
{
	F_sfactor *sfactor;
	
	if (type == O_SPLINE)
	{
		cur_x = fix_x = x;
		cur_y = fix_y = y;
		cur_s = (F_spline *)obj;
		cur_point = (F_point *)q;
		set_mousefun((char *)_(msg_Set), "", (char *)_(msg_Cancel), "", "", "");
		draw_mousefun_canvas();
		
		/* save initial sfactor */
		sfactor = search_sfactor(cur_s, cur_point);
		saved_sfactor = sfactor->s;
		
		canvas_locmove_proc = moving_sfactor;
		canvas_leftbut_proc = set_sfactor;
		canvas_middlebut_proc = null_proc;
		canvas_rightbut_proc = cancel_sfactor;
		set_cursor(pick15_cursor);
		set_action_on();
	}
    else
    {
        editsfactor_selected();
    }
}

void
moving_sfactor(int x, int y)
{
    double sfact;
    F_sfactor *sfactor;
 
    sfact = calcsfact(x,y);
    put_msg((char *)_(msg_SFactorValue), sfact);
    /* draw new spline */
	sfactor = search_sfactor(cur_s, cur_point);
	sfactor->s = sfact;
	redisplay_spline(cur_s);
}

void
set_sfactor(int x, int y)
{
	F_sfactor *sfactor;
	double sfact;

    sfact = calcsfact(x,y); 
	sfactor = search_sfactor(cur_s, cur_point);
	sfactor->s = sfact;
	redisplay_spline(cur_s);
	
	editsfactor_selected();
    draw_mousefun_canvas();	
}

void
cancel_sfactor(void)
{
	F_sfactor *sfactor;
	/* put back initial spline */
	mask_toggle_splinemarker(cur_s);
	sfactor = search_sfactor(cur_s, cur_point);
	sfactor->s = saved_sfactor;
	redisplay_spline(cur_s);
	
	editsfactor_selected();
    draw_mousefun_canvas();
}
