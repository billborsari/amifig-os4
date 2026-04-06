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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/f_read.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/* errors from read_figc in addition to those in errno.h (e.g. ENOENT) */
#define	BAD_FORMAT		-1
#define	EMPTY_FILE		-2
#define NO_VERSION		-3

#define MERGE			TRUE
#define DONT_MERGE		FALSE

#define REMAP_IMAGES		TRUE
#define DONT_REMAP_IMAGES	FALSE

/* min, max font size (points) */
#define MIN_FONT_SIZE	1
#define MAX_FONT_SIZE	500

/* input buffer length */
#define BUF_SIZE 1024

extern int  read_fig(char *file_name, F_compound *obj, BOOL merge, int xoff, int yoff);
