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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/d_ellipse.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

extern void ellipsebyradius_drawing_selected(void);
extern void cancel_ellipsebyrad(void);
extern void init_ellipsebyradius_drawing(int x, int y);
extern void create_ellipsebyrad(int x, int y);

extern void ellipsebydiameter_drawing_selected(void);
extern void cancel_ellipsebydia(void);
extern void init_ellipsebydiameter_drawing(int x, int y);
extern void create_ellipsebydia(int x, int y);

extern void circlebyradius_drawing_selected(void);
extern void cancel_circlebyrad(void);
extern void init_circlebyradius_drawing(int x, int y);
extern void create_circlebyrad(int x, int y);

extern void circlebydiameter_drawing_selected(void);
extern void cancel_circlebydia(void);
extern void init_circlebydiameter_drawing(int x, int y);
extern void create_circlebydia(int x, int y);
