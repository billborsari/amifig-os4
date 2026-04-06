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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_placelib.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <libraries/mui.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "e_placelib.h"
#include "u_create.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_markers.h"
#include "u_search.h"
#include "u_translate.h"
#include "u_redraw.h"
#include "u_undo.h"
#include "w_msgpanel.h"
#include "w_cursor.h"
#include "w_canvas.h"
#ifndef AMIFIG
#include "figx.h"
#include "e_edit.h"
#include "e_rotate.h"
#include "u_draw.h"
#include "w_indpanel.h"
#include "w_library.h"
#include "w_mousefun.h"
#include "w_drawprim.h"		/* for max_char_height */
#include "w_dir.h"
#include "w_util.h"
#include "w_setup.h"
#include "w_zoom.h"
#include "e_flip.h"
#include "e_scale.h"
#include "w_modepanel.h"
#else
#include "a_coords.h"
#include "z_fig.h"
#include "z_library.h"
#endif

/* EXPORTS */
#ifndef AMIFIG
int	cur_library_object = -1;
int	old_library_object = -1;
#endif
/* STATICS */

static int	off_library_x,off_library_y;

static void	init_move_object(int x, int y),move_object(int x, int y),change_draw_mode(int x, int y);
#ifndef AMIFIG
static void	transform_lib_obj(XKeyEvent *kpe, unsigned char c, KeySym keysym);
#endif
static void place_lib_object(int x, int y, unsigned int shift);
static void	put_draw(int paint_mode);
static void	sel_place_lib_obj_proc(int x, int y, int shift);
#ifndef AMIFIG
static int	orig_put_x, orig_put_y;
#endif

static Boolean	draw_box = False;


void
put_selected(void)
{
#ifndef AMIFIG
	int	 i, x = 0,y = 0;
	char	*com;
    
	set_mousefun((char *)_(msg_place_object),(char *)_(msg_new_object),(char *)_(msg_cancel_library),
			(char *)_(msg_place_and_edit),(char *)_(msg_change_draw_mode), (char *)_(msg_place_at_orig));
#else
	int	 x = 0,y = 0;
	
	set_mousefun((char *)_(msg_place_object),(char *)_(msg_new_object),(char *)_(msg_cancel_library),
				 "",(char *)_(msg_change_draw_mode), "");
#endif
	set_action_on();
#ifndef AMIFIG
	cur_c = lib_compounds[cur_library_object]->compound;
#endif
	new_c = copy_compound(cur_c);
#ifdef MANAGE_DEPTH
	/* add it to the depths so it is displayed */
	add_compound_depth(new_c);
#endif
	/* find lower-right corner for draw_box() */
	off_library_x = new_c->secorner.x;
	off_library_y = new_c->secorner.y;
#ifndef AMIFIG
	/* and upper-left in case the user wants to place it at its original position */
	/* this is saved here because the compound was shifted to 0,0 when reading in */
	orig_put_x = lib_compounds[cur_library_object]->corner.x;
	orig_put_y = lib_compounds[cur_library_object]->corner.y;
#endif
	canvas_locmove_proc = init_move_object;
	canvas_ref_proc = null_proc;
	canvas_leftbut_proc = place_lib_object;
	canvas_middlebut_proc = sel_place_lib_obj_proc;
	canvas_rightbut_proc = cancel_place_lib_obj;
	set_cursor(null_cursor);

#ifndef AMIFIG
	/* get the pointer position */
	get_pointer_win_xy(&x, &y);
	/* draw the first image */
	init_move_object(BACKX(x), BACKY(y));
#else
	x = SCALE_UP_X(canvas_win->GZZMouseX);
	y = SCALE_UP_Y(canvas_win->GZZMouseY);
	init_move_object(x,y);
#endif
#ifndef AMIFIG
	/* message that we're placing object so and so */
	com = strdup(lib_compounds[cur_library_object]->compound->comments);
	if (strlen(com)) {
	    /* change newlines to blanks */
	    for (i=strlen(com); i>=0; i--)
		if (com[i] == '\n')
		    com[i] = ' ';
	    put_msg("Placing library object \"%s\" (%s.fig)",
		com, library_objects_texts[cur_library_object]);
	} else {
	    put_msg((char *)_(msg_PlaceLibObj), library_objects_texts[cur_library_object]);
	}
#endif
}

#ifndef AMIFIG
/* allow rotation or flipping of library object before placing on canvas */
static void
transform_lib_obj(XKeyEvent *kpe, unsigned char c, KeySym keysym)
{
    int x,y;

    x = cur_x;
    y = cur_y;

    /* first erase the existing image */
    put_draw(ERASE);
    if (c == 'r') {
	rotn_dirn = 1;
	act_rotnangle = 90;
	rotate_compound(new_c, x, y);
    } else if (c == 'l') {
	rotn_dirn = -1;
	act_rotnangle = 90;
	rotate_compound(new_c, x, y);
    } else if (c == 'h') {
	flip_compound(new_c, x, y, LR_FLIP);
    } else if (c == 'v') {
	flip_compound(new_c, x, y, UD_FLIP);
    } else if (c == 's') {
	scale_compound(new_c, 0.9, 0.9, x, y);
    } else if (c == 'S') {
	scale_compound(new_c, 1.1, 1.1, x, y);
    } /* if not any of the above characters, ignore it */
    /* and draw the new image */
    put_draw(PAINT);
}
#endif

static void
sel_place_lib_obj_proc(int x, int y, int shift)
{
    /* if shift-left button, change drawing mode */
    if (shift) {
		change_draw_mode(x, y);
		return;
    }
    /* else popup the library panel again */
    sel_place_lib_obj();
}

void
sel_place_lib_obj(void)
{
#ifndef AMIFIG
    canvas_kbd_proc = (void (*)())transform_lib_obj;
#endif
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    canvas_leftbut_proc = null_proc;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc; 
  
    /* erase any object currently being dragged around the canvas */
#ifndef AMIFIG
    if (lib_compounds && action_on && new_c)
		put_draw(ERASE);
    popup_library_panel();
#else
	if (action_on && new_c)
		put_draw(ERASE);
	DoMethod(WinLib,MUIM_Window_ToFront);
#endif
}

static void
put_draw(int paint_mode)
{
  register int    x1, y1, x2, y2;
  
  if (draw_box) {
      x1=cur_x;
      y1=cur_y;
      x2=cur_x+off_library_x;
      y2=cur_y+off_library_y;
      elastic_box(x1, y1, x2, y2);
  } else {
      if (paint_mode==ERASE)
	redisplay_compound(new_c);
      else
#ifndef AMIFIG
	redisplay_objects(new_c);
#else
	redisplay_objects(new_c, NULL);
#endif    
  }
}

static void
change_draw_mode(int x, int y)
{
    put_draw(ERASE);
    draw_box = !draw_box;
    translate_compound(new_c,-new_c->nwcorner.x,-new_c->nwcorner.y);
    if (!draw_box)
		translate_compound(new_c,cur_x,cur_y);
    
    put_draw(PAINT);
}

#ifndef AMIFIG
/* place library object at original position in its file */
static void
place_lib_object_orig(int x, int y, unsigned int shift)
{
    int dx,dy;

    canvas_ref_proc = null_proc;
    put_draw(ERASE);
    clean_up();
    /* move back to original position */
    dx = orig_put_x-x;
    dy = orig_put_y-y;
    translate_compound(new_c,dx,dy);
    /* remove it from the depths because it will be added when it is put in the main list */
    remove_compound_depth(new_c);
    add_compound(new_c);
    set_modifiedflag();
    redisplay_compound(new_c);
    put_selected();
}
#endif

static void
place_lib_object(int x, int y, unsigned int shift)
{
#ifndef AMIFIG
    F_compound *this_c;
#endif
    canvas_leftbut_proc = null_proc;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    put_draw(ERASE);
    clean_up();
    if (draw_box) 
    	translate_compound(new_c,cur_x,cur_y);
#ifdef MANAGE_DEPTH
    /* remove it from the depths because it will be added when it is put in the main list */
    remove_compound_depth(new_c);
#endif
    add_compound(new_c);
    set_modifiedflag();
    redisplay_compound(new_c);
    if (shift) {
#ifndef AMIFIG
	/* temporarily turn off library place mode so we can edit the object just placed */
	canvas_kbd_proc = null_proc;
	clear_mousefun();
	set_mousefun("","","", "", "", "");
	turn_off_current();
	set_cursor(arrow_cursor);
	edit_remember_lib_mode = True;
	this_c = new_c;
	edit_item(this_c, O_COMPOUND, 0, 0);
#endif
    } else {
        put_selected();
    }
}


static void
move_object(int x, int y)
{
    int dx,dy;
    void  (*save_canvas_locmove_proc) ();
    void  (*save_canvas_ref_proc) ();
  
    save_canvas_locmove_proc = canvas_locmove_proc;
    save_canvas_ref_proc = canvas_ref_proc;
    /* so we don't recurse infinitely */
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
    put_draw(ERASE);  
    if (!draw_box) {
	dx=x-cur_x;
	dy=y-cur_y;
	translate_compound(new_c,dx,dy);
    }
    cur_x=x;cur_y=y;
    put_draw(PAINT);
    canvas_locmove_proc = save_canvas_locmove_proc;
    canvas_ref_proc = save_canvas_ref_proc;
}

static void
init_move_object(int x, int y)
{	
    cur_x=x;
    cur_y=y;
    if (!draw_box)    
	translate_compound(new_c,x,y);
    
    put_draw(PAINT);
    canvas_locmove_proc = move_object;
    canvas_ref_proc = move_object;
}

/* cancel placing a library object */

void
cancel_place_lib_obj(int x, int y, int shift)
{
#ifndef AMIFIG
    /* if shift right-button, actually do a place in original position */
    if (shift) {
	place_lib_object_orig(x, y, shift);
	return;
    }
#endif
    reset_action_on();
    canvas_leftbut_proc = null_proc;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    canvas_locmove_proc = null_proc;
    canvas_ref_proc = null_proc;
#ifndef AMIFIG
    canvas_kbd_proc = null_proc;
    clear_mousefun();
    set_mousefun("","","", "", "", "");
    turn_off_current();
#endif
    set_cursor(arrow_cursor);
    put_draw(ERASE);
#ifdef MANAGE_DEPTH
    /* remove it from the depths */
    remove_compound_depth(new_c);
#endif
}

