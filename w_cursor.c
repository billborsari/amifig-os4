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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_cursor.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <intuition/pointerclass.h>

#include "resources.h"
#include "w_canvas.h"

	
/*
static unsigned char arrow_cursor_Data[]=
{
 0xC0,0x00,0xE0,0x00,0xF0,0x00,0xF8,0x00,0xFC,0x00,0xF0,0x00,0x90,0x00,0x18,0x00,
 0x18,0x00,0x0C,0x00,0x0C,0x00,0x06,0x00,0x06,0x00,0x03,0x00,0x03,0x00,0x01,0x00,

 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
*/
static const unsigned char buster_cursor_Data[]=
{
 0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00,0xF8,0xF8,0x00,0x00,
 0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const unsigned char crosshair_cursor_Data[]=
{
 0x10,0x00,0x10,0x00,0x10,0x00,0xFE,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const unsigned char pick9_cursor_Data[]=
{
 0x3E,0x00,0x41,0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x41,0x00,
 0x3E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const unsigned char null_cursor_Data[]=
{
 0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

static const unsigned char pan_cursor_Data[]=
{
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x03, 0x70, 0x00,
	0x00, 0x64, 0x88, 0x00,
	0x00, 0x59, 0x1E, 0x00,
	0x00, 0x92, 0x22, 0x00,
	0x01, 0x24, 0x44, 0x00,
	0x02, 0x00, 0x88, 0x00,
	0x02, 0x01, 0x10, 0x00,
	0x04, 0x00, 0x20, 0x00,
	0x04, 0x00, 0x40, 0x00,
	0x08, 0x00, 0x80, 0x00,
	0x08, 0x01, 0x00, 0x00,
	0x08, 0x01, 0x00, 0x00,
	0x10, 0x01, 0x00, 0x00,
	0x10, 0x00, 0xF8, 0x00,
	0x20, 0x00, 0x04, 0x00,
	0x20, 0x00, 0x04, 0x00,
	0x40, 0x00, 0xF8, 0x00,
	0x00, 0x07, 0x00, 0x00,
	0x00, 0x38, 0x00, 0x00,
	0x00, 0x40, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

static const struct BitMap buster_cursor_BM =
{
 2,
 16,
 0,
 2,
 0,
 {(PLANEPTR)buster_cursor_Data,(PLANEPTR)(buster_cursor_Data+32)}
};
static const struct BitMap crosshair_cursor_BM=
{
 2,
 16,
 0,
 2,
 0,
 {(PLANEPTR)crosshair_cursor_Data,(PLANEPTR)(crosshair_cursor_Data+32)}
};
static const struct BitMap pick9_cursor_BM=
{
 2,
 16,
 0,
 2,
 0,
 {(PLANEPTR)pick9_cursor_Data,(PLANEPTR)(pick9_cursor_Data+32)}
};
static const struct BitMap null_cursor_BM=
{
 2,
 16,
 0,
 2,
 0,
 {(PLANEPTR)null_cursor_Data,(PLANEPTR)(null_cursor_Data+32)}
};
static const struct BitMap pan_cursor_BM=
{
 4,  // BytesPerRow
 24, // Rows
 0,  // Flags
 1,  // Depth
 0,  // pad
 {(PLANEPTR)pan_cursor_Data}
};

CURSOR null_cursor 		= NULL;
CURSOR arrow_cursor 	= NULL;
CURSOR buster_cursor 	= NULL;
CURSOR crosshair_cursor = NULL;
CURSOR pick9_cursor 	= NULL;
CURSOR pick15_cursor 	= NULL;
CURSOR cur_cursor 		= NULL;
CURSOR pan_cursor		= NULL;
CURSOR wait_cursor      = NULL;

void init_cursor(void)
{
#ifdef __amigaos4__
	arrow_cursor = (CURSOR)0; // POINTERTYPE_NORMAL
	null_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&null_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-4,
										  POINTERA_YOffset,     (LONG)0,
										  TAG_DONE);
	buster_cursor = (CURSOR)23; // POINTERTYPE_NOTALLOWED
	crosshair_cursor = (CURSOR)7; // POINTERTYPE_CROSS
	pick9_cursor = (CURSOR)38; // POINTERTYPE_SELECT
	pick15_cursor = (CURSOR)38; // POINTERTYPE_SELECT
	pan_cursor = (CURSOR)11; // POINTERTYPE_HAND
	wait_cursor = (CURSOR)1; // POINTERTYPE_BUSY
#else
	/* NULL pointer leads to normal pointer */
	arrow_cursor = NULL; 

	null_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&null_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-4,
										  POINTERA_YOffset,     (LONG)0,
										  TAG_DONE);
	buster_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&buster_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-6,
										  POINTERA_YOffset,     (LONG)-6,
										  TAG_DONE);
	crosshair_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&crosshair_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-3,
										  POINTERA_YOffset,     (LONG)-3,
										  TAG_DONE);
	pick9_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&pick9_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-5,
										  POINTERA_YOffset,     (LONG)-5,
										  TAG_DONE);
	pick15_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&pick9_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)1,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-5,
										  POINTERA_YOffset,     (LONG)-5,
										  TAG_DONE);
	pan_cursor = (Object *)NewObject(NULL, (STRPTR)"pointerclass",
										  POINTERA_BitMap,      (IPTR)&pan_cursor_BM,
										  POINTERA_WordWidth,   (ULONG)2,
										  POINTERA_XResolution, (ULONG)POINTERXRESN_SCREENRES,
										  POINTERA_YResolution, (ULONG)POINTERYRESN_SCREENRESASPECT,
										  POINTERA_XOffset,     (LONG)-19,
										  POINTERA_YOffset,     (LONG)-3,
										  TAG_DONE);
#endif
	cur_cursor = arrow_cursor; /* current cursor */

}

void dispose_cursor(void)
{
#ifndef __amigaos4__
	if (null_cursor) 		DisposeObject((Object *)null_cursor);
	if (arrow_cursor) 		DisposeObject((Object *)arrow_cursor);
	if (buster_cursor) 		DisposeObject((Object *)buster_cursor);
	if (crosshair_cursor) 	DisposeObject((Object *)crosshair_cursor);
	if (pick9_cursor) 		DisposeObject((Object *)pick9_cursor);
	if (pick15_cursor) 		DisposeObject((Object *)pick15_cursor);
	if (pan_cursor) 		DisposeObject((Object *)pan_cursor);	
#else
	if (null_cursor) 		DisposeObject((Object *)null_cursor);
#endif
}

void set_cursor(CURSOR cursor)
{
	cur_cursor = cursor;
#ifdef __amigaos4__
	if ((IPTR)cursor < 100) {
		SetWindowPointer( canvas_win,
						  WA_PointerType, (ULONG)(IPTR)cursor,
						  TAG_DONE );
	} else {
		SetWindowPointer( canvas_win,
						  WA_Pointer, (APTR)cursor,
						  TAG_DONE );
	}
#else
	SetWindowPointer( canvas_win,
					  WA_Pointer, (APTR)cursor,
					  TAG_DONE );
#endif
}

BOOL waiting   = FALSE;  // waitstate
int wait_count = 0;    // number of wait-calls

/* enter busy state: set mouse-pointer to wait status */
void beginWait(void)
{
	int i;

	if(wait_count == 0)
	{
		for(i=0; i < MAXWINDOWS; i++)
			if(figContexts[i].paintWnd)
				SetWindowPointer( figContexts[i].paintWnd,
								  WA_BusyPointer, TRUE,
								  WA_PointerDelay, TRUE,
								  TAG_DONE );

		waiting=TRUE;
	}

	wait_count++;
}

/* end busy state */
void endWait(void)
{
	int i;

	if(wait_count == 1)
	{
		for(i=0; i < MAXWINDOWS; i++)
			if(figContexts[i].paintWnd) {
#ifdef __amigaos4__
                if ((IPTR)figContexts[i].cur_cursor < 100) {
                    SetWindowPointer( figContexts[i].paintWnd,
					  WA_PointerType, (ULONG)(IPTR)figContexts[i].cur_cursor,
					  TAG_DONE );
                } else {
                    SetWindowPointer( figContexts[i].paintWnd,
					  WA_Pointer, (APTR)figContexts[i].cur_cursor,
					  TAG_DONE );
                }
#else
                SetWindowPointer( figContexts[i].paintWnd,
					  WA_Pointer, (APTR)figContexts[i].cur_cursor,
					  TAG_DONE );
#endif
            }

		waiting=FALSE;
	}

	wait_count--;
}

void set_temp_cursor(CURSOR cursor)
{
	beginWait();
}

void reset_cursor(void)
{
	endWait();
}
