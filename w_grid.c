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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_grid.c $
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
#include "paintop.h"
#include "a_coords.h"
#include "w_canvas.h"

void redisplay_grid(void)
{
	float x, y, resolution = 0.0;
    int   gridmode;

	if(cur_gridmode != GRID_0)
	{
		for (gridmode = cur_gridmode-1; ((gridmode < GRID_4) && (resolution < 10.0)); gridmode++)
		{
			resolution = SCALE_DOWN_F(grid_spacing[cur_gridunit][gridmode]);
		}

		if (resolution >= 10.0)
		{
			SetDrPt(rastport,0x4444);
			SetABPenDrMd(rastport, 3, 2, JAM1);

			/*  vertical grid  */
			for(x=resolution-fmod((float)xshift, resolution); x <= canvas_win->GZZWidth; x += resolution)
			{
				Move(rastport, round(x), 0);
				Draw(rastport, round(x), canvas_win->Height);
			}

			/*  horizontal grid  */
			for(y=resolution-fmod((float)yshift, resolution); y <= canvas_win->GZZHeight; y += resolution)
			{
				Move(rastport, 0, round(y));
				Draw(rastport, canvas_win->Width, round(y));
			}
			SetDrPt(rastport,0xffff);
		}
	}
}

void round_coords(int *x, int *y)
{
	float txx,posrnd,poshlf,X,Y;

	X=*x; Y=*y;

	if(cur_pointposn != P_ANYONE)
	{
		posrnd=posn_rnd[cur_gridunit][cur_pointposn];
		poshlf=posn_hlf[cur_gridunit][cur_pointposn];

		*x = round( ((txx = fmod(X, posrnd)) < poshlf)? X-txx : X + posrnd - txx );
		*y = round( ((txx = fmod(Y, posrnd)) < poshlf)? Y-txx : Y + posrnd - txx );
	}
}
