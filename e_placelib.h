/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1998 by Stephane Mancini
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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/e_placelib.h $
 *       $Revision: 338 $
 *       $Date: 2016-04-23 08:30:23 +0000 (Sat, 23 Apr 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#ifndef AMIFIG
extern int	cur_library_object;
extern int	old_library_object;
#endif

extern void	cancel_place_lib_obj(int x, int y, int shift);
extern void	sel_place_lib_obj(void);
extern void	put_noobj_selected();
extern void put_selected (void);
