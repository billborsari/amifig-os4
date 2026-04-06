/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_area.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/


/*
	Area selection functions used to set zone to export or to print out.
*/
#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "a_coords.h"
#include "u_elastic.h"
#include "w_cursor.h"
#include "z_export.h"
#include "z_print.h"

void initselectarea(int x, int y);
void set_area(int x, int y);
void stop_selectarea(void);

void selectarea_selected(void)
{
    canvas_locmove_proc     = null_proc;
    canvas_leftbut_proc     = initselectarea;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = null_proc;
	set_cursor(arrow_cursor);
	reset_action_on();
}

void stop_selectarea(void)
{
	elastic_box(fix_x, fix_y, cur_x, cur_y);
	selectarea_selected() ;
}

void initselectarea(int x, int y)
{
	cur_x = fix_x = x;
	cur_y = fix_y = y;
    canvas_locmove_proc     = resizing_box;
    canvas_leftbut_proc     = set_area;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = stop_selectarea;
	elastic_box(fix_x, fix_y, cur_x, cur_y);
	set_temp_cursor(null_cursor);
	set_action_on();
}

void set_area(int x, int y)
{
	int area_x,area_y,area_w,area_h;
	int xa, xb, ya, yb;

	/* erase current displayed area*/
	elastic_box(fix_x, fix_y, cur_x, cur_y);
	
	/* get the screen coordinates and substract the shift of the visible part */
	xa = SCALE_DOWN_X(fix_x)+xshift;
	ya = SCALE_DOWN_Y(fix_y)+yshift;
	xb = SCALE_DOWN_X(x)+xshift;
	yb = SCALE_DOWN_Y(y)+yshift;
		
	/* Export results */
	area_x = min2(xa, xb);
	area_y = min2(ya, yb);
	area_w = abs(xb-xa);
	area_h = abs(yb-ya);
    z_updateprint(area_x,area_y,area_w,area_h);
    z_updateexport(area_x,area_y,area_w,area_h);
	
	/* restart action */
	selectarea_selected();
}
