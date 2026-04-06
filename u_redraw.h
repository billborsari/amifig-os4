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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/u_redraw.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/


extern void redisplay_canvas(void);
extern void	clearcounts(void);		/* clear object counters for each depth */
extern void	clearallcounts(void);	/* clear all object counters for each depth */
extern void redisplay_all(void);
extern void refresh_area( APTR obj, int xmin, int ymin, int xmax, int ymax, BOOL ignore);
extern void clear_compound_tags(F_compound *compoundlist);
extern void redisplay_objects( F_compound *objects, APTR skip);
extern void compound_depths( F_compound *compounds, int *maxdepth, int *mindepth);

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

struct counts {
	unsigned int 	    num_arcs;	  /* # arcs at this depth */
	unsigned int 	    num_lines;	  /* # lines at this depth */
	unsigned int 	    num_ellipses; /* # ellipses at this depth */
	unsigned int 	    num_splines;  /* # splines at this depth */
	unsigned int 	    num_texts;	  /* # texts at this depth */
	unsigned int 	    cnt_arcs;	  /* count of arcs drawn at this depth */
	unsigned int 	    cnt_lines;	  /* count of lines drawn at this depth */
	unsigned int 	    cnt_ellipses; /* count of ellipses drawn at this depth */
	unsigned int 	    cnt_splines;  /* count of splines drawn at this depth */
	unsigned int 	    cnt_texts;	  /* count of texts drawn at this depth */
};

extern void refreshRectArea(int xmin, int ymin, int xmax, int ymax);
extern void refresh_arc_area( F_arc *arc, BOOL ignore);
extern void refresh_ellipse_area( F_ellipse *ellipse, BOOL ignore);
extern void refresh_line_area( F_line *line, BOOL ignore);
extern void refresh_spline_area( F_spline *spline, BOOL ignore);
extern void refresh_text_area( F_text *text, BOOL ignore);
extern void refresh_compound_area( F_compound *compound, BOOL ignore);

#define redisplay_arc(obj)      refresh_arc_area( obj, FALSE)
#define redisplay_ellipse(obj)  refresh_ellipse_area( obj, FALSE)
#define redisplay_line(obj)     refresh_line_area( obj, FALSE)
#define redisplay_spline(obj)   refresh_spline_area( obj, FALSE)
#define redisplay_text(obj)     refresh_text_area( obj, FALSE)
#define redisplay_compound(obj) refresh_compound_area( obj, FALSE)
