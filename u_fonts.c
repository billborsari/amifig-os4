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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/u_fonts.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include "resources.h"
#include "u_fonts.h"
#include "object.h"
#include "w_msgpanel.h"
#ifdef AMIFIG
#include "paintop.h"
#include "mode.h"
#include "a_coords.h"
#include "w_canvas.h"
#endif

struct TextFont *canvas_font;
extern F_text *cur_text;

struct TextAttr fontattr=
{
	NULL, 			// name of the font
	DEF_FONTSIZE, 	// height of the font
	FS_NORMAL, 		// intrinsic font style
	0				// font preferences and flags
};

pr_size textsize(struct TextFont *fontstruct, int len, char *string)
{
    pr_size size;
    struct TextExtent te;
    
    SetFont(rastport, fontstruct);

    // Get some size information about the text
   	TextExtent(rastport, (CONST_STRPTR)string, len, &te);
    size.ascent  = - SCALE_UP(te.te_Extent.MinY);
    size.descent = SCALE_UP(te.te_Extent.MaxY);
    size.length  = SCALE_UP(te.te_Width);
    
    return size;
}

int SetCanvasFont(struct Window *win, F_text *t)
{
	int DisplayFontNum = 0;
    struct TextExtent te;

	if (t->flags & PSFONT_TEXT)
		DisplayFontNum = t->font + 1;
	else
		DisplayFontNum = t->font + NUM_FONTS + 1;

    // do not reopen already opened font!
    if ((!canvas_font) || 
        (fontattr.ta_Name == NULL) || 
        (strcmp((const char *)fontattr.ta_Name,appres.DisplayFonts[DisplayFontNum]) != 0) || 
        (fontattr.ta_YSize != round(display_zoomscale*t->size)))
    {
        if (canvas_font) CloseFont(canvas_font);
        
        fontattr.ta_Name	= (STRPTR)appres.DisplayFonts[DisplayFontNum];
        fontattr.ta_YSize	= round(display_zoomscale*t->size);

        if ((canvas_font = (struct TextFont *)OpenDiskFont(&fontattr)) == NULL)	
        {
            put_msg((char *)_(msg_FontNotAvailable),fontattr.ta_Name, round(display_zoomscale*t->size));
            // trying to open default font as a replacement
            fontattr.ta_Name	= (STRPTR)appres.DisplayFonts[0];
            if ((canvas_font = (struct TextFont *)OpenDiskFont(&fontattr)) == NULL)	
            {
                put_msg((char *)_(msg_FontNotAvailable),fontattr.ta_Name, round(display_zoomscale*t->size));		
                return(-1);
            }
        }
    }
    
    SetFont(rastport, canvas_font);

    // Get some size information about the text
   	TextExtent(rastport, (CONST_STRPTR)t->cstring, strlen(t->cstring), &te);
    t->ascent  = - SCALE_UP(te.te_Extent.MinY);
    t->descent = SCALE_UP(te.te_Extent.MaxY);
    t->length  = SCALE_UP(te.te_Width);
	return(0);
}

void pw_text(struct Window *win, int x, int y, int op, int depth, struct TextFont *fontstruct,
             float angle, char *string, Color color, Color background)
{
	Color pen_color;
	int length, ascent, descent;
    struct TextExtent te;

#ifndef AMIFIG
    /* if this depth is inactive, draw the text in gray */
    /* if depth == MAX_DEPTH+1 then the caller wants the original color no matter what */
    if (draw_parent_gray || (depth < MAX_DEPTH+1 && !active_layer(depth)))
	color = MED_GRAY;
#endif

    SetFont(rastport, fontstruct);

	x=SCALE_DOWN_X(x);
	y=SCALE_DOWN_Y(y);

    /* Get some size information */
    TextExtent(rastport, (CONST_STRPTR)string, strlen(string), &te);
    ascent  = - te.te_Extent.MinY;
    descent = te.te_Extent.MaxY;
    length  = te.te_Width;

	pen_color = (color == DEFAULT)? BLACK : color;
	if (pen_color >= (NUM_STD_COLS + MAX_USR_COLS)) pen_color = BLACK;

	if(op == INV_PAINT)
	{
		SetDrMd(rastport,COMPLEMENT);
		SetRPAttrs(rastport, RPTAG_PenMode, TRUE, TAG_DONE);
	}
	else
    {
        SetDrMd(rastport, JAM1);
        SetRPAttrs( rastport,
                    RPTAG_PenMode, FALSE,
                    RPTAG_FgColor, ((op == ERASE)? ColorPalette[WHITE].RGB : ColorPalette[pen_color].RGB) | 0xFF000000,
                    TAG_DONE);                    
    }

	if(angle == 0)
	{
		/* No rotation : simply draw text at current position */
		Move(rastport, x, y);
		Text(rastport, (CONST_STRPTR)string, strlen(string));
	}
	else
	{
		/* Text with rotation isn't handled directly by libraries do the work ourselves */
		int x0, y0, x1, y1, x2, y2, x3, y3;
		int minx, miny, maxx, maxy;
		int x_rel,y_rel,sourcex,sourcey;
		float cosine = (float)cos(angle);
		float sine   = (float)sin(angle);	
	
		/* First get the coordinates of the 4 corner points after rotation */
        x0 = (int)roundf(+ descent * sine);
        y0 = (int)roundf(- descent * cosine);
		x1 = (int)roundf(- ascent  * sine);
		y1 = (int)roundf(+ ascent  * cosine);
		x2 = (int)roundf(- ascent  * sine    + length * cosine);
		y2 = (int)roundf(+ ascent  * cosine  + length * sine);
		x3 = (int)roundf(+ descent * sine    + length * cosine);
		y3 = (int)roundf(- descent * cosine  + length * sine);

        /* Compute dimensions of the resulting bitmap */
		minx = min2(x0,min2(x1, min2(x2,x3)));
		miny = min2(y0,min2(y1, min2(y2,y3)));
		maxx = max2(x0,max2(x1, max2(x2,x3)));
		maxy = max2(y0,max2(y1, max2(y2,y3)));

		if (op != INV_PAINT)
		{
			/* if not inverse paint we draw the actual text */
			UBYTE  			*pixmap = NULL;
			struct BitMap 	*TempBM = NULL;
			struct RastPort *TempRP = NULL;
            ULONG  			 Lpixel;

			WORD BMW = GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
			WORD BMH = GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);
			WORD PMH, PMW;
			
			if (strcmp(string," ")!=0)
			{				

                PMH = (ascent + descent + 1);
                PMW = length;
                // Do stuff here so that we end up with a pixmap (256 gray levels)
                // create Temp BitMap and RastPort
                if ((TempBM = AllocBitMap(PMW, PMH, 32, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, rastport->BitMap)))
                {
#ifdef __AROS__
                    if ((TempRP = CreateRastPort()))
#else
                    TempRP = AllocMem (sizeof (struct RastPort), MEMF_ANY);
                    InitRastPort(TempRP);
#endif            
                    if (TempRP)
                    {
                        TempRP->BitMap = TempBM;
            
                        // clear rastport
                        SetDrMd(TempRP,JAM1);
                        SetRPAttrs(TempRP, RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xFFFFFFFF, TAG_DONE);
                        RectFill(TempRP, 0, 0, PMW, PMH);
                    
                        // write text to TempBM
                        SetDrMd(TempRP,JAM1);
                        SetRPAttrs(TempRP, RPTAG_PenMode, FALSE, RPTAG_FgColor, 0xFF000000, TAG_DONE);
                        Move(TempRP, 0,(int)(ascent + 1));
                        SetFont(TempRP, fontstruct);
                        Text(TempRP, (CONST_STRPTR)string, strlen(string));
                        
                        // copy data to the pixmap
                        pixmap = AllocVec(PMW * PMH * 4, MEMF_ANY);
                        
                        ReadPixelArray( pixmap, 0, 0, PMW * 4,
                                        TempRP, 0, 0, PMW, PMH,
                                        RECTFMT_ARGB);

                        // free temporary BitMap and RastPort
#ifdef __AROS__
                        FreeRastPort(TempRP);
#else
                        if (TempRP) 
                        {
                            FreeMem (TempRP, sizeof (struct RastPort));
                            TempRP = NULL;
                        }
#endif
                    }
                    FreeBitMap(TempBM);
                }
				
                Lpixel   = ColorPalette[pen_color].RGB;
				
				for( y_rel = miny; y_rel < maxy; y_rel++ )
				{
					for( x_rel = minx; x_rel < maxx; x_rel++ )
					{
						sourcex = (int)roundf(x_rel*cosine + y_rel*sine  );
						
						sourcey = (int)roundf(x_rel*sine   - y_rel*cosine) + (ascent+1);
						
						if( sourcex >= 0 && sourcex < PMW && sourcey >= 0 && sourcey < PMH )
						{
							Lpixel = (Lpixel & 0x00FFFFFF) | ((255 - pixmap[(sourcey * PMW + sourcex)*4 + 1]) << 24);
							if ((Lpixel & 0xFF000000) != 0)
                            {
								if (((x + x_rel) > 0) && ((y - y_rel)>0) && ((x + x_rel) < BMW) && ((y - y_rel) < BMH))
								{
                                    WriteRGBPixel(rastport, x + x_rel, y - y_rel, Lpixel);
								}
                            }
						}
					}
				}
				FreeVec(pixmap);
			}
		}
		else
		{
			/* for inverse paint we are just drawing the bounding box for moving angled text */
			WORD points[5][2];
		
			points[0][0]=x+x0;
			points[0][1]=y-y0;

			points[1][0]=x+x1;
			points[1][1]=y-y1;

			points[2][0]=x+x2;
			points[2][1]=y-y2;

			points[3][0]=x+x3;
			points[3][1]=y-y3;

			points[4][0]=x+x0;
			points[4][1]=y-y0;

			Move(rastport, points[0][0], points[0][1]);
			PolyDraw(rastport, 5, (WORD *)points);			
		}
	}
		
	if(op == INV_PAINT)
    {
        SetDrMd(rastport,JAM1);
    }
}
