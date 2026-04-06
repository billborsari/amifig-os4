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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_arrow.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#ifdef AMIFIG
extern void add_linearrow(F_line *line, F_point *prev_point, F_point *selected_point);
extern void add_arcarrow(F_arc *arc, int point_num);
extern void add_splinearrow(F_spline *spline, F_point *prev_point, F_point *selected_point);
#else
extern void	delete_linearrow(F_line *line, F_point *prev_point, F_point *selected_point);
extern void	delete_arcarrow(F_arc *arc, int point_num);
extern void	delete_splinearrow(F_spline *spline, F_point *prev_point, F_point *selected_point);
#endif

extern void arrow_head_selected (void);
