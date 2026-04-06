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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/u_list.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef U_LIST_H
#define U_LIST_H

extern void list_delete_arc(F_arc **arc_list, F_arc *arc);
extern void list_delete_ellipse(F_ellipse **ellipse_list, F_ellipse *ellipse);
extern void list_delete_line(F_line **line_list, F_line *line);
extern void list_delete_spline(F_spline **spline_list, F_spline *spline);
extern void list_delete_text(F_text **text_list, F_text *text);
extern void list_delete_compound(F_compound **list, F_compound *compound);
#ifdef MANAGE_DEPTH
void		remove_depth(int type, int depth);
void		remove_compound_depth(F_compound *comp);
#endif

extern void list_add_arc(F_arc **arc_list, F_arc *a);
extern void list_add_ellipse(F_ellipse **ellipse_list, F_ellipse *e);
extern void list_add_line(F_line **line_list, F_line *l);
extern void list_add_spline(F_spline **spline_list, F_spline *s);
extern void list_add_text(F_text **text_list, F_text *t);
extern void list_add_compound(F_compound **list, F_compound *c);
#ifdef MANAGE_DEPTH
void		add_depth(int type, int depth);
void		add_compound_depth(F_compound *comp);
#endif
F_line 		*last_line(F_line *list);
F_arc 		*last_arc(F_arc *list);
F_ellipse 	*last_ellipse(F_ellipse *list);
F_text 		*last_text(F_text *list);
F_spline 	*last_spline(F_spline *list);
F_compound 	*last_compound(F_compound *list);
extern F_point *last_point(F_point *list);
F_sfactor   *last_sfactor(F_sfactor *list);
extern BOOL first_spline_point(int x, int y, double s, F_spline *spline);
extern BOOL append_sfactor(double s, F_sfactor *cpoint);
extern F_point *search_spline_point(F_spline *spline, int x, int y);
F_point        *search_line_point(F_line *line, int x, int y);
extern F_sfactor *search_sfactor(F_spline *spline, F_point *selected_point);
extern BOOL insert_point(int x, int y, F_point *point);
extern int num_points(F_point *points);

F_line	       *prev_line(F_line *list, F_line *line);
F_arc	       *prev_arc(F_arc *list, F_arc *arc);
F_ellipse      *prev_ellipse(F_ellipse *list, F_ellipse *ellipse);
F_text	       *prev_text(F_text *list, F_text *text);
F_spline       *prev_spline(F_spline *list, F_spline *spline);
F_compound     *prev_compound(F_compound *list, F_compound *compound);
extern F_point	       *prev_point(F_point *list, F_point *point);

extern void delete_line(F_line *old_l);
extern void delete_arc(F_arc *old_a);
extern void delete_ellipse(F_ellipse *old_e);
extern void delete_text(F_text *old_t);
extern void delete_spline(F_spline *old_s);
extern void delete_compound(F_compound *old_c);

extern void add_line(F_line *new_l);
extern void add_arc(F_arc *new_a);
extern void add_ellipse(F_ellipse *new_e);
extern void add_text(F_text *new_t);
extern void add_spline(F_spline *new_s);
extern void add_compound(F_compound *new_c);

extern void change_line(F_line *old_l, F_line *new_l);
extern void change_arc(F_arc *old_a, F_arc *new_a);
extern void change_ellipse(F_ellipse *old_e, F_ellipse *new_e);
extern void change_text(F_text *old_t, F_text *new_t);
extern void change_spline(F_spline *old_s, F_spline *new_s);
extern void change_compound(F_compound *old_c, F_compound *new_c);


void		get_links(int llx, int lly, int urx, int ury);
void		adjust_links(int mode, F_linkinfo *links, int dx, int dy, int cx, int cy, float sx, float sy, BOOL copying);
extern void append_objects(F_compound *l1, F_compound *l2, F_compound *tails);
extern void cut_objects(F_compound *objects, F_compound *tails);
extern int object_count(F_compound *list);
extern void set_tags(F_compound *list, int tag);
extern void get_interior_links (int llx, int lly, int urx, int ury);
extern void append_point(int x, int y, F_point **point);
#ifdef MANAGE_DEPTH
extern void remove_arc_depths (F_arc *a);
extern void remove_ellipse_depths (F_ellipse *e);
extern void remove_line_depths (F_line *l);
extern void remove_spline_depths (F_spline *s);
extern void remove_text_depths (F_text *t);
#endif
extern void tail(F_compound *ob, F_compound *tails);
#endif /* U_LIST_H */
