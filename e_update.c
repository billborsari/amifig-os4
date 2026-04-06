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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_update.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"
#include "resources.h"
#include "object.h"
#include "mode.h"
#include "paintop.h"
#include "u_create.h"
#include "u_list.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_msgpanel.h"

#include "e_scale.h"
#include "u_bound.h"
#include "u_fonts.h"
#include "u_free.h"
#include "u_redraw.h"
#include "w_cursor.h"
#ifndef AMIFIG
#include "d_text.h"
#include "u_draw.h"
#include "w_drawprim.h"
#include "w_indpanel.h"
#include "w_mousefun.h"
#include "w_setup.h"
#include "w_util.h"
#include "f_util.h"
#else
#include "e_update.h"
#include "a_color.h"
#include "z_edit.h"
#endif

static void init_update_object(F_line *p, int type, int x, int y, int px, int py);
void update_line(F_line *l);
void update_text(F_text *t);
void update_ellipse(F_ellipse *e);
void update_arc(F_arc *a);
void update_spline(F_spline *s);
void update_compound(F_compound *compound);

static void init_get(F_line *p, int type, int x, int y, int px, int py);
void get_line_attrs(F_line *l);
void get_text_attrs(F_text *t);
void get_ellipse_attrs(F_ellipse *e);
void get_arc_attrs(F_arc *a);
void get_spline_attrs(F_spline *s);

ULONG updatelist[UPD_Max]={	TRUE, TRUE,	TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
							FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};


void
update_selected(void)
{
    set_mousefun((char *)_(msg_update_object), "", "",
			(char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc     = null_proc;
    canvas_ref_proc = null_proc;
	init_searchproc_left(init_update_object);
    canvas_leftbut_proc     = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc    = null_proc;
	set_cursor(pick9_cursor);

	return_proc = update_selected;
}


void
get_selected(void)
{
    set_mousefun((char *)_(msg_Get_settings), "", "",
			(char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object), (char *)_(msg_Locate_Object));
    canvas_locmove_proc     = null_proc;
	init_searchproc_left(init_get);
    canvas_leftbut_proc     = object_search_left;
    canvas_middlebut_proc   = null_proc;
    canvas_rightbut_proc    = null_proc;
	return_proc = get_selected;
	set_cursor(pick9_cursor);
}

static void
init_get(F_line *p, int type, int x, int y, int px, int py)
{
	switch(type)
	{
        case O_COMPOUND:
            put_msg((char *)_(msg_CompoundGetAttrErr));
            return;
            break;
        case O_POLYLINE:
            get_line_attrs(p);
            break;
        case O_TXT:
            get_text_attrs((F_text *)p);
            break;
        case O_ELLIPSE:
            get_ellipse_attrs((F_ellipse *)p);
            break;
        case O_ARC:
            get_arc_attrs((F_arc *)p);
            break;
        case O_SPLINE:
            get_spline_attrs((F_spline *)p);
            break;
        default:
            break;
	}

    z_refreshAttributes();
}

/* update the selected object FROM the indicator buttons */

static void
init_update_object(F_line *p, int type, int x, int y, int px, int py)
{
#ifndef AMIFIG
    int		    largest;
#endif
    Boolean	    dontupdate;

    dontupdate = False;

	switch (type) {
	case O_COMPOUND:
		set_temp_cursor(wait_cursor);
		cur_c = (F_compound *) p;
		new_c = copy_compound(cur_c);
#ifndef AMIFIG
	    /* keep the depths of the objects inside the compound the same
    	    relative to each other, setting the depth of the *most shallow*
    	    to the desired depth */
		keep_depth = True;
	    largest = find_largest_depth(cur_c);
	    /* find delta */
	    delta_depth = cur_depth - find_smallest_depth(cur_c);
	    /* if renumbering would make depths too large don't allow it */
	    if ((delta_depth + largest > MAX_DEPTH) && (cur_updatemask & I_DEPTH)) {
     	    if (popup_query(QUERY_YESNO,
         		"Some depths would exceed maximum - those objects\nwill be set to maximum depth. Update anyway?") != RESULT_YES) {
         	    delta_depth = 0;
         	    dontupdate = True;
    	    }
    	}
#endif
    	update_compound(new_c);
#ifndef AMIFIG
    	keep_depth = False;
#endif  
    	change_compound(cur_c, new_c);
    	/* redraw anything near the old comound */
    	redisplay_compound(cur_c);
    	/* draw the new compound */
    	redisplay_compound(new_c);
		break;
	case O_POLYLINE:
		set_temp_cursor(wait_cursor);
		cur_l = (F_line *) p;
		new_l = copy_line(cur_l);
		update_line(new_l);
		change_line(cur_l, new_l);
    	/* redraw anything near the old line */
    	redisplay_line(cur_l);
    	/* draw the new line */
    	redisplay_line(new_l);
		break;
	case O_TXT:
		set_temp_cursor(wait_cursor);
		cur_t = (F_text *) p;
		new_t = copy_text(cur_t);
		update_text(new_t);
		change_text(cur_t, new_t);
    	/* redraw anything near the old text */
    	redisplay_text(cur_t);
    	/* draw the new text */
    	redisplay_text(new_t);
		break;
	case O_ELLIPSE:
		set_temp_cursor(wait_cursor);
		cur_e = (F_ellipse *) p;
		new_e = copy_ellipse(cur_e);
		update_ellipse(new_e);
		change_ellipse(cur_e, new_e);
    	/* redraw anything near the old ellipse */
    	redisplay_ellipse(cur_e);
    	/* draw the new ellipse */
    	redisplay_ellipse(new_e);
		break;
	case O_ARC:
		set_temp_cursor(wait_cursor);
		cur_a = (F_arc *) p;
		new_a = copy_arc(cur_a);
		update_arc(new_a);
		change_arc(cur_a, new_a);
    	/* redraw anything near the old arc */
    	redisplay_arc(cur_a);
    	/* draw the new arc */
    	redisplay_arc(new_a);
		break;
	case O_SPLINE:
		set_temp_cursor(wait_cursor);
		cur_s = (F_spline *) p;
		new_s = copy_spline(cur_s);
		update_spline(new_s);
		change_spline(cur_s, new_s);
    	/* redraw anything near the old spline */
    	redisplay_spline(cur_s);
    	/* draw the new spline */
    	redisplay_spline(new_s);
		break;
	default:
		return;
	}
	reset_cursor();
    if (!dontupdate)
	   put_msg((char *)_(msg_ObjectUpdated));
}

void update_ellipse(F_ellipse *ellipse)
{
	if(updatelist[UPD_Depth])
		ellipse->depth=cur_depth;

	if(updatelist[UPD_Pencol])
		ellipse->pen_color=cur_pencolor;

	if(updatelist[UPD_Fillcolor])
		ellipse->fill_color=cur_fillcolor;

	if(updatelist[UPD_Fillstyle])
	{
		ellipse->fill_style=cur_fillstyle;
	}

	if(updatelist[UPD_Linewid])
	{
		ellipse->style_val=2.0*ellipse->style_val/(ellipse->thickness+1);
		ellipse->thickness=cur_linewidth;
		ellipse->style_val=ellipse->style_val * (ellipse->thickness+1) / 2;
	}

	if(updatelist[UPD_Linestyle])
	{
		ellipse->style=cur_linestyle;
		ellipse->style_val=cur_styleval * (ellipse->thickness+1) / 2;
	}

	if(updatelist[UPD_ETAngle])
		ellipse->angle=(float)cur_elltextangle/180.0*M_PI;
}

void update_arc(F_arc *arc)
{
	if(updatelist[UPD_Depth])
		arc->depth=cur_depth;

	if(updatelist[UPD_Pencol])
		arc->pen_color=cur_pencolor;

	if(updatelist[UPD_Fillcolor])
		arc->fill_color=cur_fillcolor;

	if(updatelist[UPD_Fillstyle])
	{
		arc->fill_style=cur_fillstyle;
	}

	if(updatelist[UPD_Linewid])
	{
		arc->style_val=2.0*arc->style_val/(arc->thickness+1);
		arc->thickness=cur_linewidth;
		arc->style_val=arc->style_val * (arc->thickness+1) / 2;
		update_arrows((F_line *)arc, cur_linewidth);
	}

	if(updatelist[UPD_Linestyle])
	{
		arc->style=cur_linestyle;
		arc->style_val=cur_styleval * (arc->thickness+1) / 2;
	}

	if(updatelist[UPD_Arctype])
		arc->type=cur_arctype;

	if(updatelist[UPD_CapStyle])
		arc->cap_style=cur_capstyle;
}

void update_line(F_line *line)
{
	if(updatelist[UPD_Depth])
		line->depth=cur_depth;

	if(updatelist[UPD_Pencol])
		line->pen_color=cur_pencolor;

	if(updatelist[UPD_Fillcolor])
		line->fill_color=cur_fillcolor;

	if(updatelist[UPD_Fillstyle])
	{
        line->fill_style=cur_fillstyle;
	}

	if(updatelist[UPD_Linewid])
	{
		line->style_val=2.0*line->style_val/(line->thickness+1);
		line->thickness=cur_linewidth;
		line->style_val=line->style_val * (line->thickness+1) / 2;
		update_arrows(line, cur_linewidth);
	}

	if(updatelist[UPD_Linestyle])
	{
		line->style=cur_linestyle;
		line->style_val=cur_styleval * (line->thickness+1) / 2;
	}

	if(updatelist[UPD_JoinStyle])
		line->join_style=cur_joinstyle;

	if(updatelist[UPD_CapStyle])
		line->cap_style=cur_capstyle;
}

void update_text(F_text *text)
{
	if(updatelist[UPD_Depth])
		text->depth=cur_depth;

	if(updatelist[UPD_Pencol])
		text->color=cur_pencolor;

	if(updatelist[UPD_Font])
	{
		text->font =(cur_textflags & PSFONT_TEXT)? cur_ps_font:cur_latex_font;

		if(cur_textflags & PSFONT_TEXT)
			text->flags|=PSFONT_TEXT;
		else
			text->flags&=~PSFONT_TEXT;

		SetCanvasFont(canvas_win, text);
	}

	if(updatelist[UPD_Fontsize])
	{
		SetCanvasFont(canvas_win, text);
	}

	if(updatelist[UPD_Adjust])
    {
		text->type=cur_textjust;
    }
    
	if(updatelist[UPD_Rigid])
    {
		if(cur_textflags & RIGID_TEXT)
        {
			text->flags|=RIGID_TEXT;
		}
        else
        {
			text->flags&=~RIGID_TEXT;
        }
    }
    
	if(updatelist[UPD_SpecialText])
	{
        if(cur_textflags & SPECIAL_TEXT)
        {
			text->flags|=SPECIAL_TEXT;
		}
        else
		{
            text->flags&=~SPECIAL_TEXT;
        }
    }
    
	if(updatelist[UPD_TextAngle])
	{
        text->angle=(float)cur_textangle/180.0*M_PI;
    }
}

void update_spline(F_spline *spline)
{
	if(updatelist[UPD_Depth])
		spline->depth=cur_depth;

	if(updatelist[UPD_Pencol])
		spline->pen_color=cur_pencolor;

	if(updatelist[UPD_Fillcolor])
		spline->fill_color=cur_fillcolor;

	if(updatelist[UPD_Fillstyle])
	{
		spline->fill_style=cur_fillstyle;
	}

	if(updatelist[UPD_Linewid])
	{
		spline->style_val=2.0*spline->style_val/(spline->thickness+1);
		spline->thickness=cur_linewidth;
		spline->style_val=spline->style_val * (spline->thickness+1) / 2;
		update_arrows((F_line *)spline, cur_linewidth);
	}

	if(updatelist[UPD_Linestyle])
	{
		spline->style=cur_linestyle;
		spline->style_val=cur_styleval * (spline->thickness+1) / 2;
	}

	if(updatelist[UPD_CapStyle])
		spline->cap_style=cur_capstyle;
}

/* check that the fill style is legal for the color in the object */
/* WARNING: this procedure assumes that splines, lines, arcs and ellipses
	    all have the same structure up to the fill_style and color */

void fix_fillstyle(F_line  *object)
{
	if (object->fill_color == BLACK || object->fill_color == DEFAULT)
	{
		if (object->fill_style >= NUMSHADEPATS &&
		        object->fill_style < NUMSHADEPATS+NUMTINTPATS)
			object->fill_style = UNFILLED;
	}
	/* a little sanity check */
#ifndef AMIFIG
    if (object->fill_color < DEFAULT)
		object->fill_color = DEFAULT;
	if (object->fill_color >= NUMFILLPATS)
		object->fill_color = NUMFILLPATS;
#endif
}

void update_compound(F_compound *compound)
{
    F_line	   *dline, *dtick1, *dtick2, *dbox;
    F_text	   *dtext;

    /* if this is a dimension line, update its settings from the dimline settings */
    if (dimline_components(compound, &dline, &dtick1, &dtick2, &dbox)) {
    	if (dline) {
    	    dline->thickness = cur_dimline_thick;
    	    dline->style = cur_dimline_style;
    	    dline->pen_color = cur_dimline_color;
    
    	    /* free old left arrow */
    	    if (dline->back_arrow) {
                free((char *) dline->back_arrow);
                dline->back_arrow = NULL;
    	    }
    	    /* create new one if setting says so */
    	    if (cur_dimline_leftarrow != -1)
            {
                dline->back_arrow = backward_dim_arrow();
            }
    	    /* free old right arrow */
    	    if (dline->for_arrow) {
                free((char *) dline->for_arrow);
                dline->for_arrow = NULL;
    	    }
    	    /* create new one if setting says so */
    	    if (cur_dimline_rightarrow != -1)
            {
                dline->for_arrow = forward_dim_arrow();
            }
    	    /* update text box */
    	    if (dbox) {
                /* attach the polygon after the main line */
                dline->next = dbox;
                dbox->thickness = cur_dimline_boxthick;
                dbox->fill_color = cur_dimline_boxcolor;
    	    }
    	} /* if (dline) */
    
    	/* free any old ticks */
    	if (dtick1)
    	    free_linestorage(dtick1);
    	if (dtick2)
    	    free_linestorage(dtick2);
    
    	/* create new ones if user wants */
    	if (cur_dimline_ticks) {
    	    create_dimline_ticks(dline, &dtick1, &dtick2);
    	    /* attach it to the previous object in the compound */
    	    if (dbox)
            {
                dbox->next = dtick1;
    	    }
            else
    		{
                dline->next = dtick1;
                dtick1->next = dtick2;
            }
    	} else {
    	    /* no ticks, terminate list of lines after box */
    	    if (dbox)
            {
                dbox->next = (F_line *) NULL;
            }
    	}
        /* now put the new line list into the compound */
        if (dline)
        {
            compound->lines = dline;
        }
        else
        {
            compound->lines = dbox;
        }
    	/* finally, the text */
    	if ((dtext = compound->texts)) {
    	    dtext->color = cur_dimline_textcolor;
    	    dtext->font = cur_dimline_font;
    	    dtext->size = cur_dimline_fontsize;
    	    dtext->flags = cur_dimline_psflag? PSFONT_TEXT: 0;
    	    /* free any comments in the text */
    	    if (dtext->comments)
            {
                free((char *) dtext->comments);
            }
    	    if (cur_dimline_fixed)
            {
                dtext->comments = my_strdup("fixed text");
            }
    	}
#ifndef AMIFIG
    	/* now update the depths of the components */
    	up_depth_part(dline->depth, cur_depth);
    	if (dbox)
    	    up_depth_part(dbox->depth,  cur_depth);
    	if (dtick1)
    	    up_depth_part(dtick1->depth, cur_depth);
    	if (dtick2)
    	    up_depth_part(dtick2->depth, cur_depth);
    	if (dtext)
    	    up_depth_part(dtext->depth, cur_depth);
#endif    
    	/* finally, rescale if necessary */
    	rescale_dimension_line(compound, 1.0, 1.0, 0, 0);
    
    	/* end of dimension line update */

    } else {
    	/* ordinary compound */
    	update_lines(compound->lines);
    	update_splines(compound->splines);
    	update_ellipses(compound->ellipses);
    	update_arcs(compound->arcs);
    	update_texts(compound->texts);
    	update_compounds(compound->compounds);
    }
    compound_bound(compound, &compound->nwcorner.x, &compound->nwcorner.y,
		   &compound->secorner.x, &compound->secorner.y);
}

void update_arcs(F_arc *arcs)
{
	F_arc	   *a;

	for (a = arcs; a != NULL; a = a->next)
		update_arc(a);
}

void update_compounds(F_compound *compounds)
{
	F_compound	   *c;

	for (c = compounds; c != NULL; c = c->next)
		update_compound(c);
}

void update_ellipses(F_ellipse *ellipses)
{
	F_ellipse	   *e;

	for (e = ellipses; e != NULL; e = e->next)
		update_ellipse(e);
}

void update_lines(F_line *lines)
{
	F_line	   *l;

	for (l = lines; l != NULL; l = l->next)
		update_line(l);
}

void update_splines(F_spline *splines)
{
	F_spline	   *s;

	for (s = splines; s != NULL; s = s->next)
		update_spline(s);
}

void update_texts(F_text *texts)
{
	F_text	   *t;

	for (t = texts; t != NULL; t = t->next)
		update_text(t);
}



void get_line_attrs(F_line *l)
{
	if(updatelist[UPD_Depth])
		cur_depth=l->depth;

	if(updatelist[UPD_Pencol])
        setPenColor(l->pen_color);

	if(updatelist[UPD_Fillcolor])
        setFillColor(l->fill_color);

	if(updatelist[UPD_Fillstyle])
        setFillStyle(l->fill_style);

	if(updatelist[UPD_Linewid])
		cur_linewidth=l->thickness;

	if(updatelist[UPD_Linestyle])
	{
		cur_linestyle=l->style;
		cur_styleval=2.0*l->style_val/(l->thickness+1);
	}

	if(updatelist[UPD_JoinStyle])
		cur_joinstyle=l->join_style;

	if(updatelist[UPD_CapStyle])
		cur_capstyle=l->cap_style;
}

void get_text_attrs(F_text *t)
{
	if(updatelist[UPD_Depth])
		cur_depth=t->depth;

	if(updatelist[UPD_Pencol])
        setPenColor(t->color);

	if(updatelist[UPD_Font])
    {
		if(t->flags & PSFONT_TEXT)
		{
			cur_ps_font=t->font;
			cur_textflags|=PSFONT_TEXT;
		}
		else
		{
			cur_latex_font=t->font;
			cur_textflags&=~PSFONT_TEXT;
		}
    }
	if(updatelist[UPD_Fontsize])
		cur_fontsize=t->size;

	if(updatelist[UPD_Adjust])
		cur_textjust=t->type;

	if(updatelist[UPD_Rigid])
    {
		if(t->flags & RIGID_TEXT)
			cur_textflags|=RIGID_TEXT;
		else
			cur_textflags&=~RIGID_TEXT;
    }
    
	if(updatelist[UPD_SpecialText])
	{
        if(t->flags & SPECIAL_TEXT)
			cur_textflags|=SPECIAL_TEXT;
		else
			cur_textflags&=~SPECIAL_TEXT;
    }
    
	if(updatelist[UPD_TextAngle])
		cur_textangle=round(t->angle / M_PI * 180.0);
}

void get_ellipse_attrs(F_ellipse *e)
{
	if(updatelist[UPD_Depth])
		cur_depth=e->depth;

	if(updatelist[UPD_Pencol])
        setPenColor(e->pen_color);

	if(updatelist[UPD_Fillcolor])
        setFillColor(e->fill_color);

	if(updatelist[UPD_Fillstyle])
        setFillStyle(e->fill_style);

	if(updatelist[UPD_Linewid])
		cur_linewidth=e->thickness;

	if(updatelist[UPD_Linestyle])
	{
		cur_linestyle=e->style;
		cur_styleval=2.0*e->style_val/(e->thickness+1);
	}

	if(updatelist[UPD_ETAngle])
		cur_elltextangle=round(e->angle / M_PI * 180.0);
}

void get_arc_attrs(F_arc *a)
{
	if(updatelist[UPD_Depth])
		cur_depth=a->depth;

	if(updatelist[UPD_Pencol])
        setPenColor(a->pen_color);

	if(updatelist[UPD_Fillcolor])
        setFillColor(a->fill_color);

	if(updatelist[UPD_Fillstyle])
        setFillStyle(a->fill_style);

	if(updatelist[UPD_Linewid])
		cur_linewidth=a->thickness;

	if(updatelist[UPD_Linestyle])
	{
		cur_linestyle=a->style;
		cur_styleval=2.0*a->style_val/(a->thickness+1);
	}

	if(updatelist[UPD_Arctype])
		cur_arctype=a->type;

	if(updatelist[UPD_CapStyle])
		cur_capstyle=a->cap_style;
}

void get_spline_attrs(F_spline *s)
{
	if(updatelist[UPD_Depth])
		cur_depth=s->depth;

	if(updatelist[UPD_Pencol])
        setPenColor(s->pen_color);

	if(updatelist[UPD_Fillcolor])
        setFillColor(s->fill_color);

	if(updatelist[UPD_Fillstyle])
        setFillStyle(s->fill_style);

	if(updatelist[UPD_Linewid])
		cur_linewidth=s->thickness;

	if(updatelist[UPD_Linestyle])
	{
		cur_linestyle=s->style;
		cur_styleval=2.0*s->style_val/(s->thickness+1);
	}

	if(updatelist[UPD_CapStyle])
		cur_capstyle=s->cap_style;
}


