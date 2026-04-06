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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/u_redraw.c $
 *       $Revision: 338 $
 *       $Date: 2016-04-23 08:30:23 +0000 (Sat, 23 Apr 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/ 
#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"
#include "mode.h"
#include "e_rotate.h"
#include "u_draw.h"
#include "u_redraw.h"
#include "w_canvas.h"
#include "u_elastic.h"
#include "u_markers.h"
#include "u_bound.h"
#include "u_undo.h"
#include "w_rulers.h"
#ifndef AMIFIG
#include "d_arc.h"
#include "e_flip.h"
#include "w_drawprim.h"
#include "w_file.h"
#include "w_indpanel.h"
#include "w_layers.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"
#include "d_text.h"
#include "u_bound.h"
#include "w_cursor.h"
#else
#include "w_grid.h"
#include "a_antialias.h"
#include "a_coords.h"
#endif

/* EXPORTS */


/*
 * Support for rendering based on correct object depth.	 A simple depth based
 * caching scheme; anything more will require major surgery on the object
 * data structures that will percolate throughout program.
 *
 * One ``counts'' structure for each object type at each nesting depth from 0
 * to MAX_DEPTH - 1.  We track both the number of objects per type per depth,
 * as well as the number of objects drawn so far per type per depth to cut
 * down on search loop overhead.
 */

/*
 * The array of ``counts'' structures.	All objects at depth >= MAX_DEPTH are
 * accounted for in the counts[MAX_DEPTH] entry.
 */

struct counts	countss[MAX_DEPTH + 1];

/*
 * Function to clear the array of object counts with file load or new command.
 */

void redisplay_arcobject (F_arc *arcs, int depth, F_arc *skip);
void redisplay_compoundobject (F_compound *compounds, int depth, F_compound *);
void redisplay_ellipseobject (F_ellipse *ellipses, int depth, F_ellipse *skip);
void redisplay_lineobject (F_line *lines, int depth, F_line *skip);
void redisplay_splineobject (F_spline *splines, int depth, F_spline *skip);
void redisplay_textobject (F_text *texts, int depth, F_text *skip);

void arc_depths( F_arc *arcs, int *maxdepth, int *mindepth);
void line_depths( F_line *lines, int *maxdepth, int *mindepth);
void ellipse_depths( F_ellipse *ellipses, int *maxdepth, int *mindepth);
void spline_depths( F_spline *splines, int *maxdepth, int *mindepth);
void text_depths( F_text *texts, int *maxdepth, int *mindepth);

void clear_tags(void);

void select_redisplay_objects( APTR obj, BOOL ignore, int xmin_region, int ymin_region, int xmax_region, int ymax_region);

#ifndef AMIFIG
void
clearallcounts(void)
{
    register struct counts *cp;

    for (cp = &counts[0]; cp <= &counts[MAX_DEPTH]; ++cp) {
	cp->num_arcs = 0;
	cp->num_lines = 0;
	cp->num_ellipses = 0;
	cp->num_splines = 0;
	cp->num_texts = 0;
    }
    clearcounts();
}
#else
/*
 * Clear count of objects drawn at each depth
 */
void
clearcounts(void)
{
	register struct counts *cp;

	for (cp = &countss[0]; cp <= &countss[MAX_DEPTH]; cp++)	{
		cp->num_arcs	= 0;
		cp->num_lines	= 0;
		cp->num_ellipses= 0;
		cp->num_splines = 0;
		cp->num_texts	= 0;
		
		cp->cnt_arcs 	= 0;
		cp->cnt_lines	= 0;
		cp->cnt_ellipses= 0;
		cp->cnt_splines = 0;
		cp->cnt_texts	= 0;
	}
}
#endif

#ifdef AMIFIG
static void draw_pageborders(void)
{
	float pagex,pagey;

	get_pagesizes(&pagex, &pagey);

	SetDrPt(rastport,0xf0f0);
	SetDrMd(rastport,JAM1);
    SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[CYAN].RGB, TAG_DONE);

	Move(rastport, 0, toprul_height + CM_TO_PIX(pagey));
	Draw(rastport, CM_TO_PIX(pagex), toprul_height + CM_TO_PIX(pagey));
	Draw(rastport, CM_TO_PIX(pagex), toprul_height);

	SetDrPt(rastport,0xffff);
}
#endif

void redisplay_objects( F_compound *objects, APTR skip)
{
	int depth, maxcomp, maxtext, maxspline, maxarc, maxline, maxell, dum;
	unsigned int max_depth;
    BOOL OnScreenAA = appres.OnScreenAA;
    int width=0, height=0;
			
	if(objects == NULL) return;

	/* Clear object counts, and then get the max. depth of any object from
	   the max. depths of each object type in the top level compound. */

	clearcounts();

	compound_depths( objects->compounds, &maxcomp, &dum);
	text_depths( objects->texts, &maxtext, &dum);
	spline_depths( objects->splines, &maxspline, &dum);
	arc_depths( objects->arcs, &maxarc, &dum);
	line_depths( objects->lines, &maxline, &dum);
	ellipse_depths( objects->ellipses, &maxell, &dum);

	max_depth=max2(maxcomp, max2(maxtext, maxspline));
	max_depth=max2(maxarc, max2(maxline, max2(maxell, max_depth)));

	
	/*
        Initial try for an on screen antialising display
        The rendering is done in an off-screen bitmap at 
        2X the window resolution and then blitted back in the display
    */
    
	if (OnScreenAA)
	{
        /* Get current display size : must be a multiple of 2 */
        width  = ((canvas_win->GZZWidth  + 1)/2) * 2;
        height = ((canvas_win->GZZHeight + 1)/2) * 2;
        
        OnScreenAA = setup_aa_rendering(0, 0, width, height);
	}
	
	/*
	 * A new outer loop, executing once per depth level from max_depth down
	 * to 0 (negative depths are not supported).  The code inside the loop is
	 * the original code for redisplay_objects.
	 */
	for (depth = max_depth; depth >= 0; depth--)
	{
		redisplay_arcobject( objects->arcs, depth, skip);
		redisplay_compoundobject( objects->compounds, depth, skip);
		redisplay_ellipseobject( objects->ellipses, depth, skip);
		redisplay_lineobject( objects->lines, depth, skip);
		redisplay_splineobject( objects->splines, depth, skip);
		redisplay_textobject( objects->texts, depth, skip);
	}

    /*
        If AA is active and offscreen bitmap could be allocated
        perform AntiAliasing
    */
	if (OnScreenAA)
	{
        perform_antialising(0, 0, width, height);
	}
	
	/*
	 * Point markers and compounds, not being ``real objects'', are handled
	 * outside the depth loop.
	 */

    /* show the markers if they are on */
    toggle_markers_in_compound(objects);
    /* mark any center if requested */
    if (setcenter)
		center_marker(setcenter_x, setcenter_y);
    /* and any anchor */
#ifndef AMIFIG
    if (setanchor)
	center_marker(setanchor_x, setanchor_y);
#endif
}

// redraw rectangular area, ignoring (or not ignoring) the object pointed at by obj
// coordinates are at FIG resolution
void refresh_area( APTR obj, int xmin, int ymin, int xmax, int ymax, BOOL ignore)
{
	struct Region *old_region = NULL;
	struct Rectangle cliprect;
	F_compound *objects = &fobjects;
	int depth, maxcomp, maxtext, maxspline, maxarc, maxline, maxell;
	unsigned int max_depth;
	// Needed for AA on screen output
    BOOL OnScreenAA = appres.OnScreenAA;
	int width = 0, height = 0, dum = 0;

	xmin-=SCALE_UP(4);
	xmax+=SCALE_UP(4);
	ymin-=SCALE_UP(4);
	ymax+=SCALE_UP(4);

	select_redisplay_objects( obj, ignore, xmin, ymin, xmax, ymax);
	
	xmin=SCALE_DOWN_X(xmin); ymin=SCALE_DOWN_Y(ymin);
	xmax=SCALE_DOWN_X(xmax); ymax=SCALE_DOWN_Y(ymax);	

	// area is completely outside of painting area => invisible
	if (    (ymax < paint_cliprect.MinY) 
		 || (ymin > paint_cliprect.MaxY)
		 || (xmax < paint_cliprect.MinX)
		 || (xmin > paint_cliprect.MaxX))
		return;

	// reduce area to visible painting area
	if(ymin < paint_cliprect.MinY)	ymin=paint_cliprect.MinY;
	if(ymax > paint_cliprect.MaxY)	ymax=paint_cliprect.MaxY;
	if(xmin < paint_cliprect.MinX)	xmin=paint_cliprect.MinX;
	if(xmax > paint_cliprect.MaxX)	xmax=paint_cliprect.MaxX;

    /* turn off the markers if they are on */
    toggle_markers_in_compound(objects);
    /* turn off any center maker if existing */
    if (setcenter)
		center_marker(setcenter_x, setcenter_y);

    if (OnScreenAA)
	{
        /* Get current display size : sould be a multiple of 2 */
		width  = ((xmax-xmin+2) / 2) * 2;
		height = ((ymax-ymin+2) / 2) * 2;
        OnScreenAA = setup_aa_rendering(xmin, ymin, width, height);
	}
	
	if (!OnScreenAA)
	{
		// clear region
		ClearRect( xmin, ymin, xmax, ymax);
	}
        
    // paint_cliprect definiert den nutzbaren Bereich des Zeichenfensters
    cliprect.MinX=xmin;
    cliprect.MinY=ymin;
    cliprect.MaxX=xmax;
    cliprect.MaxY=ymax;

    OrRectRegion(ref_clip_region, &cliprect);
    LockLayer(0, canvas_win->WLayer);

    old_region=InstallClipRegion(canvas_win->WLayer, ref_clip_region);
	
	/* Clear object counts, and then get the max. depth of any object from
	   the max. depths of each object type in the top level compound. */
	clearcounts();

	compound_depths( objects->compounds, &maxcomp, &dum);
	text_depths( objects->texts, &maxtext, &dum);
	spline_depths( objects->splines, &maxspline, &dum);
	arc_depths( objects->arcs, &maxarc, &dum);
	line_depths( objects->lines, &maxline, &dum);
	ellipse_depths( objects->ellipses, &maxell, &dum);

	max_depth=max2(maxcomp, max2(maxtext, maxspline));
	max_depth=max2(maxarc, max2(maxline, max2(maxell, max_depth)));

	/*
	 * A new outer loop, executing once per depth level from max_depth down
	 * to 0 (negative depths are not supported).  The code inside the loop is
	 * the original code for redisplay_objects.
	 */
	for (depth = max_depth; depth >= 0; depth--)
	{
		redisplay_arcobject( objects->arcs, depth, (ignore)? obj:NULL);
		redisplay_compoundobject( objects->compounds, depth, (ignore)? obj:NULL);
		redisplay_ellipseobject( objects->ellipses, depth, (ignore)? obj:NULL);
		redisplay_lineobject( objects->lines, depth, (ignore)? obj:NULL);
		redisplay_splineobject( objects->splines, depth, (ignore)? obj:NULL);
		redisplay_textobject( objects->texts, depth, (ignore)? obj:NULL);
	}

    /*
        If AA is active and offscreen bitmap could be allocated
        perform AntiAliasing
    */
	if (OnScreenAA)
	{
        perform_antialising(xmin, ymin, width, height);
 	}
    	
	redisplay_grid();
	draw_pageborders();

    InstallClipRegion(canvas_win->WLayer, old_region);

    UnlockLayer(canvas_win->WLayer);
    ClearRectRegion(ref_clip_region, &cliprect);
    
    /*
     * Point markers and compounds, not being ``real objects'', are handled
     * outside the depth loop.
     */

    /* show the markers if they are on */
    toggle_markers_in_compound(objects);
    /* mark any center if requested */
    if (setcenter)
	center_marker(setcenter_x, setcenter_y);
    /* and any anchor */
#ifndef AMIFIG
    if (setanchor)
	center_marker(setanchor_x, setanchor_y);
#endif
}


/*
 * Find the maximum and minimum depth of any arc, recording the number of arcs per each
 * level along the way.
 */
void arc_depths( F_arc *arcs, int *maxdepth, int *mindepth)
{
	F_arc	   *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for (fp = arcs; fp != NULL; fp = fp->next)
	{
		if (*maxdepth < fp->depth)
			*maxdepth = fp->depth;
		if (*mindepth > fp->depth)
			*mindepth = fp->depth;

		++countss[min2(fp->depth, MAX_DEPTH)].num_arcs;
	}
}

/*
 * Find the maximum and minimum depth of any line, recording the number of lines per each
 * level along the way.
 */
void line_depths( F_line *lines, int *maxdepth, int *mindepth)
{
	F_line	   *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for (fp = lines; fp != NULL; fp = fp->next)
	{
		if (*maxdepth < fp->depth)
			*maxdepth = fp->depth;
		if (*mindepth > fp->depth)
			*mindepth = fp->depth;

		++countss[min2(fp->depth, MAX_DEPTH)].num_lines;
	}
}

/*
 * Find the maximum and minimum depth of any ellipse, recording the number of ellipses
 * per each level along the way.
 */
void ellipse_depths( F_ellipse *ellipses, int *maxdepth, int *mindepth)
{
	F_ellipse	   *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for (fp = ellipses; fp != NULL; fp = fp->next)
	{
		if (*maxdepth < fp->depth)
			*maxdepth = fp->depth;
		if (*mindepth > fp->depth)
			*mindepth = fp->depth;

		++countss[min2(fp->depth, MAX_DEPTH)].num_ellipses;
	}
}

/*
 * Find the maximum and minimum depth of any spline, recording the number of splines per
 * each level along the way.
 */
void spline_depths( F_spline *splines, int *maxdepth, int *mindepth)
{
	F_spline	   *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for (fp = splines; fp != NULL; fp = fp->next)
	{
		if (*maxdepth < fp->depth)
			*maxdepth = fp->depth;
		if (*mindepth > fp->depth)
			*mindepth = fp->depth;

		++countss[min2(fp->depth, MAX_DEPTH)].num_splines;
	}
}

/*
 * Find the maximum and minimum depth of any text, recording the number of texts per each
 * level along the way.
 */
void text_depths( F_text *texts, int *maxdepth, int *mindepth)
{
	F_text	   *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for (fp = texts; fp != NULL; fp = fp->next)
	{
		if (*maxdepth < fp->depth)
			*maxdepth = fp->depth;
		if (*mindepth > fp->depth)
			*mindepth = fp->depth;
			
		++countss[min2(fp->depth, MAX_DEPTH)].num_texts;
	}
}

/*
 * Find the maximum and minimum depth of any of the objects contained in the compound.
 */
void compound_depths( F_compound *compounds, int *maxdepth, int *mindepth)
{
	int maxcomp, maxtext, maxspline, maxarc, maxline, maxell;
	int mincomp, mintext, minspline, minarc, minline, minell;
	F_compound *fp;

	*maxdepth = MIN_DEPTH;
	*mindepth = MAX_DEPTH;

	for(fp = compounds; fp != NULL; fp = fp->next)
	{
		compound_depths( fp->compounds, &maxcomp, &mincomp);
		text_depths( fp->texts, &maxtext, &mintext);
		spline_depths( fp->splines, &maxspline, &minspline);
		arc_depths( fp->arcs, &maxarc, &minarc);
		line_depths( fp->lines, &maxline, &minline);
		ellipse_depths( fp->ellipses, &maxell, &minell);

		*maxdepth=max2(maxcomp, max2(maxtext, max2(maxspline, *maxdepth)));
		*maxdepth=max2(maxarc, max2(maxline, max2(maxell, *maxdepth)));
		
		*mindepth=min2(mincomp, min2(mintext, min2(minspline, *mindepth)));
		*mindepth=min2(minarc, min2(minline, min2(minell, *mindepth)));
	}
}

/*
 * Redisplay a list of arcs.  Only display arcs of the correct depth.
 * For each arc drawn, update the count for the appropriate depth in
 * the counts array.
 */
void redisplay_arcobject( F_arc *arcs, int depth, F_arc *skip)
{
	F_arc	   *arc;
	struct counts  *cp = &countss[min2(depth, MAX_DEPTH)];

	arc = arcs;
	while (arc != NULL && cp->cnt_arcs < cp->num_arcs)
	{
		if(depth == arc->depth)
		{
			if(!arc->ignore && skip != arc && skip != (F_arc *)-1)
				draw_arc( arc, PAINT);
			++cp->cnt_arcs;
		}
		arc = arc->next;
	}
}

/*
 * Redisplay a list of ellipses.  Only display ellipses of the correct depth
 * For each ellipse drawn, update the count for the
 * appropriate depth in the counts array.
 */

void redisplay_ellipseobject( F_ellipse *ellipses, int depth, F_ellipse *skip)
{
	F_ellipse	   *ep;
	struct counts  *cp = &countss[min2(depth, MAX_DEPTH)];


	ep = ellipses;
	while (ep != NULL && cp->cnt_ellipses < cp->num_ellipses)
	{
		if(depth == ep->depth)
		{
			if(!ep->ignore && skip != ep && skip != (F_ellipse *)-1)
				draw_ellipse( ep, PAINT);
			++cp->cnt_ellipses;
		}
		ep = ep->next;
	}
}

/*
 * Redisplay a list of lines.  Only display lines of the correct depth.
 * For each line drawn, update the count for the appropriate
 * depth in the counts array.
 */

void redisplay_lineobject( F_line *lines, int depth, F_line *skip)
{
	F_line	   *lp;
	struct counts  *cp = &countss[min2(depth, MAX_DEPTH)];


	lp = lines;
	while (lp != NULL && cp->cnt_lines < cp->num_lines)
	{
		if(depth == lp->depth)
		{
			if(!lp->ignore && skip != lp && skip != (F_line *)-1)
				draw_line( lp, PAINT);
			++cp->cnt_lines;
		}
		lp = lp->next;
	}
}

/*
 * Redisplay a list of splines.	 Only display splines of the correct depth
 * For each spline drawn, update the count for the
 * appropriate depth in the counts array.
 */

void redisplay_splineobject( F_spline *splines, int depth, F_spline *skip)
{
	F_spline	   *spline;
	struct counts  *cp = &countss[min2(depth, MAX_DEPTH)];

	spline = splines;
	while (spline != NULL && cp->cnt_splines < cp->num_splines)
	{
		if(depth == spline->depth)
		{
			if(!spline->ignore && skip != spline && skip != (F_spline *)-1)
				draw_spline( spline, PAINT);
			++cp->cnt_splines;
		}
		spline = spline->next;
	}
}

/*
 * Redisplay a list of texts.  Only display texts of the correct depth.	 For
 * each text drawn, update the count for the appropriate depth in the counts
 * array.
 */

void redisplay_textobject( F_text *texts, int depth, F_text *skip)
{
	F_text	   *text;
	struct counts  *cp = &countss[min2(depth, MAX_DEPTH)];

	text = texts;
	while (text != NULL && cp->cnt_texts < cp->num_texts)
	{
		if(depth == text->depth)
		{
			if(!text->ignore && skip != text && skip != (F_text *)-1)
				draw_text( text, PAINT);
			++cp->cnt_texts;
		}
		text = text->next;
	}
}

/*
 * Redisplay a list of compounds at a current depth.  Basically just farm the
 * work out to the objects contained in the compound.
 */

void redisplay_compoundobject( F_compound *compounds, int depth, F_compound *skip)
{
	F_compound	   *c;

	for(c = compounds; c != NULL; c = c->next) {
		if(!c->ignore && c != skip)	{
			redisplay_arcobject( c->arcs, depth, (APTR)skip);
			redisplay_compoundobject( c->compounds, depth, skip);
			redisplay_ellipseobject( c->ellipses, depth, (APTR)skip);
			redisplay_lineobject( c->lines, depth, (APTR)skip);
			redisplay_splineobject( c->splines, depth, (APTR)skip);
			redisplay_textobject( c->texts, depth, (APTR)skip);
		}
	}
}



/*
 * Redisplay the entire drawing.
 */
void
redisplay_canvas(void)
{
	APTR skip=NULL;

	UWORD old_pattern;
	old_pattern=rastport->LinePtrn;

	if(canvas_win == NULL)
		return;

	if (!appres.OnScreenAA)
    {
        ClearRect(0, toprul_height, canvas_win->GZZWidth-siderul_width, canvas_win->GZZHeight);
    }

	redisplay_rulers(BOTH_RULER);

	clear_tags();

	if(action_on && (cur_mode == F_MOVE || cur_mode == F_COPY || cur_mode == F_DRAWOBJ))
	{
		switch(cur_object)
		{
            case O_ELLIPSE:	    skip=new_e; elastic_moveellipse(); break;
            case O_POLYLINE:	skip=new_l; elastic_moveline(new_l->points); break;
            case O_SPLINE:	    skip=new_s; elastic_moveline(new_s->points); break;
            case O_TXT:		skip=new_t; elastic_movetext(); break;
            case O_ARC:		    skip=new_a; elastic_movearc(new_a); break;
            case O_COMPOUND:	skip=new_c; elastic_movebox(); break;
		}
	}

	if(action_on && cur_mode == F_SCALE)
		elastic_box(fix_x, fix_y, cur_x, cur_y);

	if(action_on && canvas_ref_proc)
		canvas_ref_proc(fix_x, fix_y, cur_x, cur_y);

	redisplay_objects(&fobjects, skip);
	draw_pageborders();
	redisplay_grid();

	SetDrPt(rastport, old_pattern);
	last_xshift = xshift;
	last_yshift = yshift+toprul_height; // remove ruler height (add, because shift values are negative)
}

void redisplay_all(void)
{
	APTR skip=NULL;

	clear_tags();

	redisplay_objects( &fobjects, skip);
}

void clear_arc_tags(F_arc *arclist)
{
	F_arc *a;

	for(a = arclist; a != NULL; a=a->next)
		a->ignore=FALSE;
}

void clear_ellipse_tags(F_ellipse *ellipselist)
{
	F_ellipse *e;

	for(e = ellipselist; e != NULL; e=e->next)
		e->ignore=FALSE;
}

void clear_line_tags(F_line *linelist)
{
	F_line *l;

	for(l = linelist; l != NULL; l=l->next)
		l->ignore=FALSE;
}

void clear_spline_tags(F_spline *splinelist)
{
	F_spline *s;

	for(s = splinelist; s != NULL; s=s->next)
		s->ignore=FALSE;
}

void clear_text_tags(F_text *textlist)
{
	F_text *t;

	for(t = textlist; t != NULL; t=t->next)
		t->ignore=FALSE;
}

void clear_compound_tags(F_compound *compoundlist)
{
	F_compound *c;

	for(c = compoundlist; c != NULL; c=c->next)
	{
		c->ignore=FALSE;
		clear_arc_tags(c->arcs);
		clear_ellipse_tags(c->ellipses);
		clear_line_tags(c->lines);
		clear_spline_tags(c->splines);
		clear_text_tags(c->texts);
		clear_compound_tags(c->compounds);
	}
}

void clear_tags(void)
{
	clear_arc_tags(fobjects.arcs);
	clear_ellipse_tags(fobjects.ellipses);
	clear_line_tags(fobjects.lines);
	clear_spline_tags(fobjects.splines);
	clear_text_tags(fobjects.texts);
	clear_compound_tags(fobjects.compounds);
}

BOOL check_overlap(int xmin1, int ymin1, int xmax1, int ymax1, int xmin2, int ymin2, int xmax2, int ymax2);
BOOL check_overlap_arc(F_arc *a, int xmin_region, int ymin_region, int xmax_region, int ymax_region);
BOOL check_overlap_ellipse(F_ellipse *e, int xmin_region, int ymin_region, int xmax_region, int ymax_region);
BOOL check_overlap_line(F_line *l, int xmin_region, int ymin_region, int xmax_region, int ymax_region);
BOOL check_overlap_spline(F_spline *s, int xmin_region, int ymin_region, int xmax_region, int ymax_region);
BOOL check_overlap_text( F_text *t, int xmin_region, int ymin_region, int xmax_region, int ymax_region);
BOOL check_overlap_compound( F_compound *c, int xmin_region, int ymin_region, int xmax_region, int ymax_region);

BOOL check_overlap(int xmin1, int ymin1, int xmax1, int ymax1, int xmin2, int ymin2, int xmax2, int ymax2)
{
	if(xmin1 < xmin2)
		if(ymin1 < ymin2)
			return((BOOL)(xmax1 >= xmin2 && ymax1 >= ymin2));
		else
			return((BOOL)(xmax1 >= xmin2 && ymin1 <= ymax2));
	else
		if(ymin1 < ymin2)
			return((BOOL)(xmin1 <= xmax2 && ymax1 >= ymin2));
		else
			return((BOOL)(xmin1 <= xmax2 && ymin1 <= ymax2));
}

BOOL check_overlap_arc(F_arc *a, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax;

	arc_bound(a, &xmin, &ymin, &xmax, &ymax);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

BOOL check_overlap_ellipse(F_ellipse *e, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax;

	ellipse_bound(e, &xmin, &ymin, &xmax, &ymax);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

BOOL check_overlap_line(F_line *l, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax;

	line_bound(l, &xmin, &ymin, &xmax, &ymax);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

BOOL check_overlap_spline(F_spline *s, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax;

	spline_bound(s, &xmin, &ymin, &xmax, &ymax);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

BOOL check_overlap_text( F_text *t, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax,dum;

	text_bound( t, &xmin, &ymin, &xmax, &ymax,&dum,&dum,&dum,&dum,&dum,&dum,&dum,&dum);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

BOOL check_overlap_compound( F_compound *c, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	int xmin,ymin,xmax,ymax;

	compound_bound( c, &xmin, &ymin, &xmax, &ymax);

	if(check_overlap(xmin, ymin, xmax, ymax, xmin_region, ymin_region, xmax_region, ymax_region))
		return(TRUE);
	else
		return(FALSE);
}

// repaint a rectangular area and ignore (or not ignore) the object pointed at by obj
void select_redisplay_objects( APTR obj, BOOL ignore, int xmin_region, int ymin_region, int xmax_region, int ymax_region)
{
	F_arc *work_a; F_compound *work_c; F_ellipse *work_e; F_line *work_l; F_spline *work_s; F_text *work_t;

	for(work_a = fobjects.arcs; work_a != NULL; work_a=work_a->next)
		if(work_a != obj)
			work_a->ignore=!check_overlap_arc(work_a, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_a->ignore=ignore;

	for(work_e = fobjects.ellipses; work_e != NULL; work_e=work_e->next)
		if(work_e != obj)
			work_e->ignore=!check_overlap_ellipse(work_e, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_e->ignore=ignore;

	for(work_l = fobjects.lines; work_l != NULL; work_l=work_l->next)
		if(work_l != obj)
			work_l->ignore=!check_overlap_line(work_l, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_l->ignore=ignore;

	for(work_s = fobjects.splines; work_s != NULL; work_s=work_s->next)
		if(work_s != obj)
			work_s->ignore=!check_overlap_spline(work_s, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_s->ignore=ignore;

	for(work_t = fobjects.texts; work_t != NULL; work_t=work_t->next)
		if(work_t != obj)
			work_t->ignore=!check_overlap_text( work_t, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_t->ignore=ignore;

	for(work_c = fobjects.compounds; work_c != NULL; work_c=work_c->next)
	{
		if(work_c != obj)
			work_c->ignore=!check_overlap_compound( work_c, xmin_region, ymin_region, xmax_region, ymax_region);
		else
			work_c->ignore=ignore;

		if(!work_c->ignore)
			clear_compound_tags(work_c);
	}
}

// coordinates are at display resolution
void refreshRectArea(int xmin, int ymin, int xmax, int ymax)
{
	refresh_area(NULL, SCALE_UP_X(xmin), SCALE_UP_Y(ymin), SCALE_UP_X(xmax), SCALE_UP_Y(ymax), FALSE);
}


void refresh_arc_area( F_arc *arc, BOOL ignore)
{
	int xmin,ymin,xmax,ymax;

	arc_bound(arc, &xmin, &ymin, &xmax, &ymax);
	refresh_area( arc, xmin, ymin, xmax, ymax, ignore);
}

void refresh_ellipse_area( F_ellipse *ellipse, BOOL ignore)
{
	int xmin,ymin,xmax,ymax;

	ellipse_bound(ellipse, &xmin, &ymin, &xmax, &ymax);
	refresh_area( ellipse, xmin, ymin, xmax, ymax, ignore);
}

void refresh_line_area( F_line *line, BOOL ignore)
{
	int xmin,ymin,xmax,ymax;

	line_bound(line, &xmin, &ymin, &xmax, &ymax);
	refresh_area( line, xmin, ymin, xmax, ymax, ignore);
}

void refresh_spline_area( F_spline *spline, BOOL ignore)
{
	int xmin,ymin,xmax,ymax;

	spline_bound(spline, &xmin, &ymin, &xmax, &ymax);
	refresh_area( spline, xmin, ymin, xmax, ymax, ignore);
}

void refresh_text_area( F_text *text, BOOL ignore)
{
	int xmin,ymin,xmax,ymax,dum;

	text_bound( text, &xmin, &ymin, &xmax, &ymax,&dum,&dum,&dum,&dum,&dum,&dum,&dum,&dum);
	refresh_area( text, xmin, ymin, xmax, ymax, ignore);
}

void refresh_compound_area( F_compound *compound, BOOL ignore)
{
	int xmin,ymin,xmax,ymax;

	compound_bound( compound, &xmin, &ymin, &xmax, &ymax);
	refresh_area( compound, xmin, ymin, xmax, ymax, ignore);
}
