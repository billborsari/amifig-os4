/**
 * SVG2FIG
 * (c) Yannick Erb
 * September 2010
 * 
 * Convert a SVG figure to a FIG figure
 * Supports SVG Tiny 
 * 
 * Usage SVG2FIG input.svg output.fig -s [scale]
 * 
 * Uses libxml2
 * Based on some parts of libsvgtiny
 */

#include <stdlib.h>
#include <string.h>
#include "svg2fig.h"

#define NUMBASECOLORS	32
#define NUMUSERCOLORS	512
#define NUMSHADES		41
#define BLACK			0x00000000
#define WHITE			0x00FFFFFF
 
ULONG BaseColors[NUMBASECOLORS]=
{
	0x00000000,
	0x000000FF,
	0x0000FF00,
	0x0000FFFF,
	0x00FF0000,
	0x00FF00FF,
	0x00FFFF00,
	0x00FFFFFF,
	0x00000090,
	0x000000B0,
	0x000000D0,
	0x0087CEFF,
	0x00009000,
	0x0000B000,
	0x0000D000,
	0x00009090,
	0x0000B0B0,
	0x0000D0D0,
	0x00900000,
	0x00B00000,
	0x00D00000,
	0x00900090,
	0x00B000B0,
	0x00D000D0,
	0x00803000,
	0x00A04000,
	0x00C06000,
	0x00FF8080,
	0x00FFA0A0,
	0x00FFC0C0,
	0x00FFE0E0,
	0x00FFD700
};
 
// calculate the distance between 2 rgb values
int calc_dist(ULONG a, ULONG b)
{
	int ra,ga,ba,rb,gb,bb;
	
	ra = (a >> 16) & 0xFF;
	ga = (a >> 8) & 0xFF;
	ba = (a) & 0xFF;

	rb = (b >> 16) & 0xFF;
	gb = (b >> 8) & 0xFF;
	bb = (b) & 0xFF;
	
	return (abs(ra-rb) + abs(ga-gb) + abs(ba-bb));
}

ULONG shade(ULONG BaseColor,int shade)
{
	UBYTE r,g,b;
	
	if ((shade == 20) || (shade < 0) || (shade > 40))
		return(BaseColor);
		

	if (BaseColor == BLACK)
	{
		if (shade > 20) return(BLACK);
		if (shade == 0) return(WHITE);
		shade = 40 - shade;
	}
	
	if (BaseColor == WHITE)
	{
		if (shade > 20) return(WHITE);
		if (shade == 0) return(BLACK);
	}
	
	if (shade == 0) 	return(BLACK);
	if (shade == 40) 	return(WHITE);
	
	r = (BaseColor >> 16) & 0xFF;
	g = (BaseColor >> 8) & 0xFF;
	b = BaseColor & 0xFF;
	
	if (shade < 20)
	{
		r = (UBYTE)((r * shade)/20);
		g = (UBYTE)((g * shade)/20);
		b = (UBYTE)((b * shade)/20);
	}
	
	if (shade > 20)
	{
		shade -= 20;
		r = (UBYTE)(r + (255 - r) * shade / 20);
		g = (UBYTE)(g + (255 - g) * shade / 20);
		b = (UBYTE)(b + (255 - b) * shade / 20);
	}

	return (RGB(r,g,b));
}
 
 // convert an RGB value to a pen number for fig
 // if exact is set, returns -1 if no exact match could be found
 // otherwise returns the closest match
int rgb_to_pen(ULONG rgb, BOOL exact)
{
	int i, pen;
	int distance, Min_distance;
 
	// get rid of alpha values
	rgb = rgb & 0x00FFFFFF;

	if (exact)
	{
		for(i = 0;i < NUMBASECOLORS; i++)
			if (rgb == BaseColors[i]) return(i);
		return(-1);
	}
	else
	{
		pen = 0;
		Min_distance = calc_dist(rgb, BaseColors[0]);
		for(i = 1;i < NUMBASECOLORS; i++)
		{
			if ((distance = calc_dist(rgb, BaseColors[i])) < Min_distance)
			{
				Min_distance = distance;
				pen = i;
			}
		}
		return(pen);
	}
}

int rgb_to_fill(struct SVG_Attr *attributes, BOOL exact)
{
	int i, s, pen;
	int distance, Min_distance;
	ULONG rgb = attributes->fill;
	
	if (strlen(attributes->gradient_id) > 0)
	{
		/* this is a gradient => look for number */
		i = 0;
		while((i<num_gradient) && (strcmp(attributes->gradient_id, gradient[i]->id) != 0)) i++;
		
		if (i<num_gradient)
		{
			/* found id */
			return((i+NUMUSERCOLORS+NUMBASECOLORS) * 100 + 20);
		}
		else
		{
			/* id not found => returns default value */
			return(0);
		}
		
	}
	else
	{
		// get rid of alpha values
		rgb = rgb & 0x00FFFFFF;
		
		if (exact)
		{
			for(i = 0;i < NUMBASECOLORS; i++)
				for (s = 1; s < NUMSHADES-1; s++)
					if (rgb == shade(BaseColors[i],s)) return(i*100+s);
			return(-1);
		}
		else
		{
			pen = 20;
			Min_distance = calc_dist(rgb, BaseColors[0]);
			for(i = 1;i < NUMBASECOLORS; i++)
			{
				for (s = 1; s < NUMSHADES-1; s++)
				{	
					if ((distance = calc_dist(rgb, shade(BaseColors[i],s))) < Min_distance)
					{
						Min_distance = distance;
						pen = i*100 + s;
					}
				}
			}
			return(pen);
		}
	}
}
