/*
 * TransFig: Facility for Translating Fig code
 * Copyright (c) 1989-2001 by Brian V. Smith
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons who receive
 * copies from any such party to do so, with the only requirement being
 * that this copyright notice remain intact.
 *
 */

#include "fig2dev.h"
#include "object.h"

extern "C"
{
extern int read_using_qimage(F_pic *pic);

/* return codes:  1 : success
		  0 : invalid file
*/

int read_png(FILE *file, int filetype, F_pic *pic, int *llx, int *lly, FILE *tfp)
{
    *llx = *lly = 0;

	if(read_using_qimage(pic) != -1)
		return 1;
	else
		return 0;
}

} // extern "C"
