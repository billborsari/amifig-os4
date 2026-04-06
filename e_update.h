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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_update.h $
 *       $Revision: 338 $
 *       $Date: 2016-04-23 08:30:23 +0000 (Sat, 23 Apr 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/* ! following defines must be in same order than GD_UPD_xxxx defines in w_update.c */
#define UPD_Depth             	0
#define UPD_Pencol            	1
#define UPD_Fillcolor         	2
#define UPD_Fillstyle         	3
#define UPD_Linewid           	4
#define UPD_Linestyle         	5
#define UPD_JoinStyle         	6
#define UPD_CapStyle          	7
#define UPD_ETAngle           	8
#define UPD_Arctype           	9
#define UPD_Font              	10
#define UPD_Fontsize          	11
#define UPD_Adjust            	12
#define UPD_Rigid             	13
#define UPD_SpecialText       	14
#define UPD_TextAngle         	15
#define UPD_Max                 16

extern ULONG updatelist[UPD_Max];

extern void update_selected(void);
extern void update_line(F_line *l);
extern void update_text(F_text *t);
extern void update_ellipse(F_ellipse *e);
extern void update_arc(F_arc *a);
extern void update_spline(F_spline *s);
extern void update_arcs(F_arc *arcs);
extern void update_compounds(F_compound *compounds);
extern void update_ellipses(F_ellipse *ellipses);
extern void update_lines(F_line *lines);
extern void update_splines(F_spline *splines);
extern void update_texts(F_text *texts);
extern void update_compound(F_compound *compound);
extern void get_selected(void);
extern void get_line_attrs(F_line *l);
extern void get_text_attrs(F_text *t);
extern void get_ellipse_attrs(F_ellipse *e);
extern void get_arc_attrs(F_arc *a);
extern void get_spline_attrs(F_spline *s);
extern void fix_fillstyle(F_line *object);
