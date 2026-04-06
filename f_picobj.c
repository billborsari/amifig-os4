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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/f_picobj.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>
#include <sys/stat.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "a_coords.h"
#include "u_fonts.h"
#include "u_free.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_msgpanel.h"
#include "w_drawprim.h"
#include "w_cursor.h"

/* Prototypes */
int readpic_by_datatype( F_pic *pic);

// =================================================================================================================
// xs_Bilerp
// =================================================================================================================
static __inline ULONG xs_Bilerp(ULONG a, ULONG b, ULONG c, ULONG d, ULONG xerr, ULONG yerr)
{
    #define xs_rbmask    0x00ff00ff
    #define xs_agmask    0xff00ff00

    if (a==b && c==d && a==d)   return a;

    const ULONG arb        =   a & xs_rbmask;
    const ULONG crb        =   c & xs_rbmask;
    const ULONG aag        =   a & xs_agmask;
    const ULONG cag        =   c & xs_agmask;

    const ULONG rbdx1      =  (b & xs_rbmask) - arb;
    const ULONG rbdx2      =  (d & xs_rbmask) - crb;
    const ULONG agdx1      = ((b & xs_agmask)>>8) - (aag >> 8);
    const ULONG agdx2      = ((d & xs_agmask)>>8) - (cag >> 8);

    const ULONG rb1        = (arb      + ((rbdx1 * xerr) >> 8)) & xs_rbmask;
    const ULONG ag1        = (aag      + ((agdx1 * xerr)     )) & xs_agmask;
    const ULONG rbdy       = ((crb     + ((rbdx2 * xerr) >> 8)) & xs_rbmask)       - rb1;
    const ULONG agdy       = (((cag    + ((agdx2 * xerr)     )) & xs_agmask)>>8)   - (ag1 >> 8);

    const ULONG rb         = (rb1 + ((rbdy * yerr) >> 8)) & xs_rbmask;
    const ULONG ag         = (ag1 + ((agdy * yerr)     )) & xs_agmask;

    return ag | rb;
}

/**
 * Bilinear resize ARGB image.
 * pixels is an array of size w * h.
 * Target dimension is w2 * h2.
 * w2 * h2 cannot be zero.
 */
ULONG *resizeBilinear(ULONG *pixels, ULONG w, ULONG h, ULONG w2, ULONG h2) 
{
    ULONG *temp=NULL;
    ULONG a, b, c, d, x, y, linestart, xerr, yerr, index;
    double x_ratio = (double)(w)/(double)w2 ;
    double y_ratio = (double)(h)/(double)h2 ;
    double x_diff, y_diff;
    ULONG offset = 0 ;
	int i,j;
	
	temp = (ULONG *)AllocVec(w2*h2*4, MEMF_ANY);
	if (temp)
	{
            for (i=0;i<h2;i++) 
            {
			y_diff = (y_ratio * i);
			y = (ULONG)(y_diff) ;
			y_diff -= y;
			yerr = (ULONG)(255 * y_diff);
			
			linestart = y*w;
			
                for (j=0;j<w2;j++)
                {
				x_diff = (x_ratio * j);
				x = (ULONG)(x_diff);
				x_diff -= x;
				xerr = (ULONG)(255 * x_diff);
				
				index = (linestart + x) ;                
                a = pixels[index] ;
                b = pixels[index+1] ;
                c = pixels[index+w] ;
                d = pixels[index+w+1] ;
                      
				temp[offset++] = xs_Bilerp(a,b,c,d, xerr, yerr);
            }
        }
    }
    return temp ;
}

BOOL read_picobj( F_line *box, F_pic *pic, Color color)
{
	int err=0;
	char ext[FESIZE]="";

	free_bitmaps(pic);

	pic->bit_size.x 	= 0;
	pic->bit_size.y 	= 0;
	pic->pix_rotation 	= 0;
	pic->pix_width 		= 0;
	pic->pix_height 	= 0;
	pic->pix_flipped 	= 0;

	put_msg((char *)_(msg_ReadPicFile));

	stcgfe(ext, pic->file);

	beginWait();

    err=readpic_by_datatype( pic);
	if(err == FileInvalid)
	{
		/* none of the above */
		put_msg((char *)pic->file);//_(msg_UnknownPicFormat));
	}
	else
	if(err == -1)
	{
		put_msg((char *)_(msg_NoMemForBM));
	}
	else
	put_msg((char *)_(msg_OK));

	endWait();

	if(err == 0)
		return(TRUE);
	else
		return(FALSE);
}

int readpic_by_datatype( F_pic *pic)
{
	extern  char DirectoryName[DSIZE+1];
	Object *picture = NULL;
	struct BitMapHeader *bmhd = NULL;
	char file_name[FCHARS+DSIZE+1]="";
	struct stat status;
	struct pdtBlitPixelArray bpa;
	ULONG i;
    int err = 0;

	if(stat(pic->file, &status) == 0)
		strcpy(file_name, pic->file);
	else
		strmfp(file_name, DirectoryName, pic->file);

	picture=NewDTObject(file_name,
						DTA_GroupID, GID_PICTURE,
						OBP_Precision, PRECISION_EXACT,
						PDTA_Remap, FALSE,
						PDTA_DestMode, PMODE_V43,
						TAG_END);
						
	if (picture)
	{
		GetDTAttrs(	picture, 
					PDTA_BitMapHeader, &bmhd,
					TAG_END);
				
		if (bmhd) 
		{
			if ((pic->orig_data = AllocVec(	bmhd->bmh_Width * bmhd->bmh_Height * 4, MEMF_ANY)))
			{
				bpa.MethodID = PDTM_READPIXELARRAY;
				bpa.pbpa_PixelData = pic->orig_data;
				bpa.pbpa_PixelFormat = PBPAFMT_ARGB;
				bpa.pbpa_PixelArrayMod = bmhd->bmh_Width * 4;
				bpa.pbpa_Left = 0;
				bpa.pbpa_Top = 0;
				bpa.pbpa_Width = bmhd->bmh_Width;
				bpa.pbpa_Height = bmhd->bmh_Height;
						
				DoMethodA( picture, (Msg)&bpa );


#ifdef __AROS__
				if (bmhd->bmh_Depth<32)
                {
					if (AROS_BIG_ENDIAN)
						for (i=0; i < (bmhd->bmh_Height * bmhd->bmh_Width); i++)
							pic->orig_data[i] |= 0xFF000000;
					else
						for (i=0; i < (bmhd->bmh_Height * bmhd->bmh_Width); i++)
							pic->orig_data[i] |= 0x000000FF;
                }
#else
				if (bmhd->bmh_Depth<32)
                {
					for (i=0; i < (bmhd->bmh_Height * bmhd->bmh_Width); i++)
						pic->orig_data[i] |= 0xFF000000;
                }
#endif						
				pic->bit_size.x=bmhd->bmh_Width;
				pic->bit_size.y=bmhd->bmh_Height;
				pic->pix_width =bmhd->bmh_Width;
				pic->pix_height=bmhd->bmh_Height;
				pic->orig_width = (bmhd->bmh_Width * PIX_PER_INCH / PICT_PIX_PER_INCH);
				pic->orig_height= (bmhd->bmh_Height * PIX_PER_INCH / PICT_PIX_PER_INCH);
			}
            else
                err = -1;
		}
		DisposeDTObject(picture);
	}
    else
        err = FileInvalid;
    
	return(err);
}

void reload_pictures(void)
{
	F_line		*l;

	for(l = fobjects.lines; l != NULL; l = l->next)
        if(l->type == T_PICTURE)
            read_picobj( l, l->pic, 0);

	if(saved_objects.lines && saved_objects.lines->type == T_PICTURE)
        read_picobj( saved_objects.lines, saved_objects.lines->pic, 0);
}

// Bitmap über die Y-Achse flippen, also in X-Richtung
void flipx_bitmap(F_line *box)
{
	// No optimisation for speed at all!!!
	int i,j,half,width,height;
	ULONG temp;

	width =box->pic->bit_size.x;
	height=box->pic->bit_size.y;

	half=(width&1)? width/2+1 : width/2;

	for(j=0; j < half; j++)
	{
		for(i=0; i < height; i++)
		{
			temp = box->pic->orig_data[j + i*width];
			box->pic->orig_data[j + i*width] = box->pic->orig_data[width-j + i*width];
			box->pic->orig_data[width-j + i*width] = temp;
		}
	}
}

// Bitmap über die X-Achse flippen, also in Y-Richtung
void flipy_bitmap(F_line *box)
{
	// No optimisation for speed at all!!!
	int i,width,height;
	ULONG *temp;
	
	width =box->pic->bit_size.x;
	height=box->pic->bit_size.y;

	temp = AllocVec(width*4,MEMF_ANY);
	
	for(i=0; i < height/2; i++)
	{
		CopyMem(&(box->pic->orig_data[i*width]),temp,width*4);
		CopyMem(&(box->pic->orig_data[(height-1-i)*width]),&(box->pic->orig_data[i*width]),width*4);
		CopyMem(temp,&(box->pic->orig_data[(height-1-i)*width]),width*4);
	}
	
	FreeVec(temp);
}

int rotate_bitmap(F_line *box, int angle)
{
	int i,j,width,height;
	ULONG *temp_data = NULL;

	width  = box->pic->bit_size.x;
	height = box->pic->bit_size.y;

	if(!(temp_data=(ULONG *)AllocVec(height * width * 4, MEMF_ANY)))
	{
		put_msg((char *)_(msg_OutOfMem));
		return(-1);
	}

	if(angle == 90 || angle == -270)
	{
		for(i=0; i < height; i++)
			for(j=0; j < width; j++)
					temp_data[i + (width-1-j)*height] = box->pic->orig_data[ j + i*width];
	}
	else // (angle == -90 || angle == 270)
	{
		for(i=0; i < height; i++)
			for(j=0; j < width; j++)
					temp_data[height-1-i + j*height] = box->pic->orig_data[ j + i*width];
	}

	FreeVec(box->pic->orig_data);
	box->pic->orig_data=temp_data;

	box->pic->bit_size.x = height;
	box->pic->bit_size.y = width;

	return(0);
}

int create_pic_pixmap(F_line *box, int rotation, int width, int height, int flipped)
{
	F_pic *pic=box->pic;
	int err = 0;

	if(pic->orig_data == NULL)
	{
		err = -1;
	}
	else
	{
		if(pic->pix_flipped == flipped)
		{
			switch(rotation-pic->pix_rotation)
			{
				case   90:
				case -270:
					if(rotate_bitmap(box, rotation-pic->pix_rotation))
						err = -1;
					break;
				case  180:
				case -180:
					flipx_bitmap(box);
					flipy_bitmap(box);
					break;
				case  -90:
				case  270:
					if(rotate_bitmap(box, rotation-pic->pix_rotation))
						err = -1;
					break;
			}
		}
		else
		{
			if(rotation == 0 && pic->pix_rotation == 0)
			{
				if(rotate_bitmap(box, -90))
					err = -1;
				else
					flipx_bitmap(box);
			}
			else
				switch((rotation+pic->pix_rotation)%360)
				{
					case  90: flipy_bitmap(box); break;
					case 270: flipx_bitmap(box); break;
				}
		}

		if(err == 0)
		{
			pic->pix_rotation = rotation;
			pic->pix_flipped  = flipped;

			if(pic->scaled_data)
			{
				FreeVec(pic->scaled_data);
				pic->scaled_data = NULL;
			}
				
			if ((pic->bit_size.x==width) && (pic->bit_size.y=height))
			{
				/* Nothing to be done */
			}
			else
			{
			    pic->scaled_data = resizeBilinear(pic->orig_data, pic->bit_size.x, pic->bit_size.y, width, height);
			}
			pic->pix_width  = width;
			pic->pix_height = height;
		}
	}
	return(err);
}

void pw_picbox( F_line *box, int xmin, int ymin, int width, int height)
{
	int length;
	
	pw_polygon( box, PAINT);

	SetFont(rastport, GUIfont_Fixed);
	length = TextLength(rastport,(CONST_STRPTR)box->pic->file, strlen(box->pic->file));

	if(width > length && height > display_zoomscale*12)
	{
		Move(rastport, xmin+width/2-length/2, (long)(ymin+height/2+display_zoomscale*DEF_FONTSIZE/2));
		Text(rastport, (CONST_STRPTR)box->pic->file, strlen(box->pic->file));
	}
}

void get_box_dimension(F_line *box, int *width, int *height)
{
	F_pos origin,opposite;

	origin.x   = (box->points->x);
	origin.y   = (box->points->y);
	opposite.x = (box->points->next->next->x);
	opposite.y = (box->points->next->next->y);

	*width  = abs(origin.x - opposite.x);
	*height = abs(origin.y - opposite.y);
}

void pw_picture( F_line *box)
{
	int xmin, ymin;
	int width, height, rotation, err = 0;
	F_pos origin,opposite;
	F_pic *pic=box->pic;

	origin.x   = SCALE_DOWN_X(box->points->x);
	origin.y   = SCALE_DOWN_Y(box->points->y);
	opposite.x = SCALE_DOWN_X(box->points->next->next->x);
	opposite.y = SCALE_DOWN_Y(box->points->next->next->y);

	xmin = min2(origin.x, opposite.x);
	ymin = min2(origin.y, opposite.y);

	width  = abs(origin.x - opposite.x);
    height = abs(origin.y - opposite.y);
    
	rotation = 0;

	if(origin.x > opposite.x && origin.y > opposite.y)
		rotation = 180;
	if(origin.x > opposite.x && origin.y <= opposite.y)
		rotation = 270;
	if(origin.x <= opposite.x && origin.y > opposite.y)
		rotation = 90;

	/* if something has changed regenerate the pixmap */
	if(	box->pic->pix_rotation != rotation ||
		abs(box->pic->pix_width - width) > 1 ||		/* rounding makes diff of 1 bit */
		abs(box->pic->pix_height - height) > 1 ||
		box->pic->pix_flipped != box->pic->flipped)
	{
		err = create_pic_pixmap(box, rotation, width, height, box->pic->flipped);
	}

	if(err == 0)
	{
		if(pic->scaled_data)
		{

			WritePixelArrayAlpha(	(APTR) pic->scaled_data,
									0, 0, pic->pix_width * 4,
									rastport,
									xmin, ymin,
									pic->pix_width, pic->pix_height,
									0xFF000000 );

		}
		else if(pic->orig_data)
		{

			WritePixelArrayAlpha(	(APTR) pic->orig_data,
									0, 0, pic->bit_size.x * 4,
									rastport,
									xmin, ymin,
									pic->bit_size.x, pic->bit_size.y,
									0xFF000000 );

		}
		else
		{
			pw_picbox( box, xmin, ymin, width, height);
		}
	}
	else
	{
		pw_picbox( box, xmin, ymin, width, height);
	}
}
