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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/fig.h $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#ifndef FIG_H
#define FIG_H

#include <proto/exec.h>
#include <proto/dos.h>
#ifdef __amigaos4__
#include <proto/rexxsyslib.h>
/* Add legacy DOS macros if not defined */
#undef Execute
/* SystemTags is a macro in OS4 SDK that handles IDOS->SystemTags call */
#define Execute(c,i,o) SystemTags(c, TAG_DONE)
#undef Examine
#define Examine(l,f) OBSOLETEExamine(l,f)
#undef ExNext
#define ExNext(l,f) OBSOLETEExNext(l,f)

extern struct DOSIFace *IDOS;

/* Make sure Rexx is covered */
#ifndef SetRexxVar
/* Map SetRexxVar(msg, name, value, len) to SetRexxVarFromMsg(name, value, msg) */
#define SetRexxVar(r,v,val,len) SetRexxVarFromMsg(v,val,r)
#endif
#endif

#ifndef __amigaos4__
#include <proto/alib.h>
#endif
#include <proto/utility.h>
#include <proto/intuition.h>
#include <intuition/gadgetclass.h>
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
#include <proto/diskfont.h>
#include <proto/asl.h>
#include <proto/input.h>
#ifdef __AROS__
#include <proto/arossupport.h>
#endif

#include <proto/graphics.h>
#include <graphics/gfx.h>
#include <graphics/rpattr.h>
#include <graphics/gfxmacros.h>

#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/layers.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>

/* Make sure booleans are known */
#undef Boolean
#undef True
#undef False
#define Boolean BOOL
#define True    TRUE
#define False   FALSE

#ifdef __amigaos4__
#include <stdarg.h>
#endif

#ifndef RPTAG_FgColor
#define RPTAG_FgColor RPTAG_APenColor
#endif
#ifndef RPTAG_BgColor
#define RPTAG_BgColor RPTAG_BPenColor
#endif
#ifndef RPTAG_PenMode
#define RPTAG_PenMode TAG_IGNORE
#endif

#include "const.h"

#include "locale.h"

#ifndef INLINE
#define INLINE __inline
#endif

/* IPTR is defined in exec/types.h on AmigaOS 4 */
#if defined(__amigaos4__) && !defined(IPTR)
typedef unsigned long IPTR;
#endif

extern struct Window *canvas_win;

extern void (*canvas_locmove_proc)();
extern void (*canvas_leftbut_proc)();
extern void (*canvas_middlebut_proc)();
extern void (*canvas_middlebut_save)();
extern void (*canvas_rightbut_proc)();
extern void (*canvas_rightbut_save)();
extern void	(*canvas_ref_proc) ();
extern void null_proc(int a, int b);
extern void (*return_proc)();

extern BOOL SSE3_Support;
extern int fix_x, fix_y, cur_x, cur_y;

extern struct TextFont *GUIfont_Fixed;

#ifdef __MORPHOS__
typedef		void *		CURSOR;
#else
typedef		Object *	CURSOR;
#endif

typedef struct
{
	WORD x,y;
}
zXPoint ;

typedef struct
{
    zXPoint* Points;
    int      MaxPoints;
    int      npoints;
}
PointList;

/* use my own PI because GNUC has a long double and others have something else */
#undef 	M_PI
#undef 	M_PI_2
#undef 	M_2PI
#define M_PI	3.14159265358979323846
#define M_PI_2	1.57079632679489661923
#define M_2PI	6.28318530717958647692

/* Define some common macros */
#define		min2(a, b)	(((a) < (b)) ? (a) : (b))
#define		max2(a, b)	(((a) > (b)) ? (a) : (b))
#define		min3(a,b,c)	((((a<b)?a:b)<c)?((a<b)?a:b):c)
#define		max3(a,b,c)	((((a>b)?a:b)>c)?((a>b)?a:b):c)
#define		round(a)	(int)(((a)<0.0)?(a)-.5:(a)+.5)
#define		signof(a)	(((a) < 0) ? -1 : 1)

/* QUICK HACK : all layers are active */
#define active_layer(x) TRUE
#define any_active_in_compound(x) TRUE

#endif /* FIG_H */
