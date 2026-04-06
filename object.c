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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/object.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "u_fonts.h"

/************************  Objects  **********************/

F_compound fobjects = {0, 0, { 0, 0 }, { 0, 0 },
                       NULL, NULL, NULL, NULL, NULL,
					   (char*) NULL, NULL, NULL, FALSE, NULL, 
					   FALSE, FALSE, 0, 0, NULL};

/************  global object pointers ************/

F_line	       *cur_l, *new_l, *old_l;
F_arc	       *cur_a, *new_a, *old_a;
F_ellipse      *cur_e, *new_e, *old_e;
F_text	       *cur_t, *new_t, *old_t;
F_spline       *cur_s, *new_s, *old_s;
F_compound     *cur_c, *new_c, *old_c;
F_point	       *first_point=NULL, *cur_point=NULL;
F_linkinfo     *cur_links=NULL;

#ifndef AMIFIG
struct _pics   *pictures=NULL;		/* common repository to share imported pictures */
#endif

int
emptyfigure(void)
{
    if (fobjects.texts != NULL)
	return (0);
    if (fobjects.lines != NULL)
	return (0);
    if (fobjects.ellipses != NULL)
	return (0);
    if (fobjects.splines != NULL)
	return (0);
    if (fobjects.arcs != NULL)
	return (0);
    if (fobjects.compounds != NULL)
	return (0);
    return (1);
}
