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
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "svg2fig.h"

//Line styles
#define Default				-1
#define Solid	 			0
#define Dashed	 			1
#define Dotted	 			2
#define Dashdotted	 		3
#define Dashdoubledotted	4
#define Dashtripledotted	5

// join style
#define Miter				0
#define Round				1
#define Bevel				2

// cap style
#define Butt				0
#define Round				1
#define Projecting			2


void write_PolyType(FILE *fp,
					int   sub_type,
					int   line_style,
					int   thickness,
					int   pen_color,
					int   fill_color,
					int   depth,	
					int   area_fill,
					float style_val,
					int   join_style,
					int   cap_style,
					int   radius,
					int   forward_arrow,
					int   backward_arrow,
					int   nbpoints,
					int   *points);
					


void write_header(FILE *fp)
{
	fprintf(fp, "#FIG 3.2  Produced by SVG2FIG %s\n",VERSION);
	fprintf(fp, "Portrait\n");
	fprintf(fp, "Flush left\n");
	fprintf(fp, "Metric\n");
	fprintf(fp, "A4\n");
	fprintf(fp, "100.00\n");
	fprintf(fp, "Single\n");
	fprintf(fp, "-2\n");
	fprintf(fp, "1200 2\n");
}

void write_polygon(float *points, int nbpoints, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int *int_points;
	int i;
	int sub_type		= 3;
	int line_style		= Solid;
	float style_val		= 1.0;
	int join_style		= Miter;
	int cap_style		= Butt;
	int radius			= 0;
	int forward_arrow	= 0;
	int backward_arrow	= 0;
	int pen_color		= rgb_to_pen(attributes->stroke, FALSE);
	int fill_color		= rgb_to_fill(attributes, TRUE);
	int thickness		= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int depth			= 0;
	int area_fill		= 20;
	
	if(fill_color == -1)
	{
		/* couldn't find matching standard color */
		fill_color = write_usercolor(attributes->fill, fp);
	}
	else
	{
		area_fill = (attributes->fill > 0x00FFFFFF)?-1:fill_color % 100;
		fill_color /= 100;
	}

	int_points = malloc((nbpoints + 1) * 2 * sizeof(int));
	for (i=0; i<nbpoints; i++)
	{
		transform_point(&points[i*2], &points[i*2+1], attributes);
		int_points[i*2] = round(points[i*2] * scale);
		int_points[i*2+1] = round(points[i*2+1] * scale);
	}
	int_points[nbpoints*2] = int_points[0];
	int_points[nbpoints*2+1] = int_points[1];
	
	
	
	write_PolyType(	fp,
					sub_type,
					line_style,
					thickness,
					pen_color,
					fill_color,
					depth,	
					area_fill,
					style_val,
					join_style,
					cap_style,
					radius,
					forward_arrow,
					backward_arrow,
					nbpoints+1,
					int_points);
	free(int_points);
}

void write_polyline(float *points, int nbpoints, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int *int_points;
	int i;
	int sub_type		= 1;
	int line_style		= Solid;
	float style_val		= 1.0;
	int join_style		= Miter;
	int cap_style		= Butt;
	int radius			= 0;
	int forward_arrow	= 0;
	int backward_arrow	= 0;
	int pen_color		= rgb_to_pen(attributes->stroke, FALSE);
	int fill_color		= rgb_to_fill(attributes, TRUE);
	int thickness		= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int depth			= 0;
	int area_fill		= 20;
	
	if(fill_color == -1)
	{
		/* couldn't find matching standard color */
		fill_color = write_usercolor(attributes->fill, fp);
	}
	else
	{
		area_fill = (attributes->fill > 0x00FFFFFF)?-1:fill_color % 100;
		fill_color /= 100;
	}

	int_points = malloc(nbpoints * 2 * sizeof(int));
	for (i=0; i<nbpoints; i++)
	{
		transform_point(&points[i*2], &points[i*2+1], attributes);
		int_points[i*2] = round(points[i*2] * scale);
		int_points[i*2+1] = round(points[i*2+1] * scale);
	}
	
	write_PolyType(	fp,
					sub_type,
					line_style,
					thickness,
					pen_color,
					fill_color,
					depth,	
					area_fill,
					style_val,
					join_style,
					cap_style,
					radius,
					forward_arrow,
					backward_arrow,
					nbpoints,
					int_points);
	free(int_points);
}

void write_rect(float x, float y, float w, float h, float rx, float ry, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int *int_points;
	int i;
	float points[10];
	int sub_type		= 2; // 4 is rounded, rotated => 3(polygon), can't be rounded
	int line_style		= Solid;
	float style_val		= 1.0;
	int join_style		= Miter;
	int cap_style		= Butt;
	int radius			= (rx * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI;
	int forward_arrow	= 0;
	int backward_arrow	= 0;
	int pen_color		= rgb_to_pen(attributes->stroke, FALSE);
	int fill_color		= rgb_to_fill(attributes, TRUE);
	int thickness		= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int depth			= 0;
	int area_fill		= 20;
	
	if(fill_color == -1)
	{
		/* couldn't find matching standard color */
		fill_color = write_usercolor(attributes->fill, fp);
	}
	else
	{
		area_fill = (attributes->fill > 0x00FFFFFF)?-1:fill_color % 100;
		fill_color /= 100;
	}
	
	if((radius!=0) && (attributes->Mat[1]==0) && (attributes->Mat[2]==0))
		sub_type = 4;	// rounded rectangle only supported without rotation
	else if ((attributes->Mat[1]!=0) || (attributes->Mat[2]!=0))
		sub_type = 3;	// if the rectangle is rotated, it has to be converted to a polygon
	
	points[0] = x;
	points[1] = y;
	points[2] = x+w;
	points[3] = y;
	points[4] = x+w;
	points[5] = y+h;
	points[6] = x;
	points[7] = y+h;
	points[8] = x;
	points[9] = y;
	
	
	int_points = malloc(5 * 2 * sizeof(int));
	for (i=0; i<5; i++)
	{
		transform_point(&points[i*2], &points[i*2+1], attributes);
		int_points[i*2] = round(points[i*2] * scale);
		int_points[i*2+1] = round(points[i*2+1] * scale);
	}

	write_PolyType(	fp, 
					sub_type,
					line_style,
					thickness,
					pen_color,
					fill_color,
					depth,	
					area_fill,
					style_val,
					join_style,
					cap_style,
					radius,
					forward_arrow,
					backward_arrow,
					5,
					int_points);
	
	free(int_points);
}


void write_circle(float cx, float cy, float r, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int		object_code	= 1;
	int		sub_type	= 3;
	int		line_style	= Solid;
	float	style_val	= 1.0;
	int		direction	= 1;
	float	angle		= 0;
	int		center_x	= cx;
	int		center_y	= cy;
	int		radius_x	= (r * attributes->Mat[0] * scale);
	int 	radius_y	= radius_x;
	int		start_x		= 0;
	int 	start_y		= 0;
	int		end_x		= 0;
	int		end_y		= 0;
	int		pen_color	= rgb_to_pen(attributes->stroke, FALSE);
	int		fill_color	= rgb_to_fill(attributes, TRUE);
	int		thickness	= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int		depth		= 0;
	int		pen_style	= 0;
	int		area_fill	= 20;
	
	if(fill_color == -1)
	{
		/* couldn't find matching standard color */
		fill_color = write_usercolor(attributes->fill, fp);
	}
	else
	{
		area_fill = (attributes->fill > 0x00FFFFFF)?-1:fill_color % 100;
		fill_color /= 100;
	}
	
	transform_point(&cx, &cy, attributes);
	center_x = round(cx * scale);
	center_y = round(cy * scale);
	
	fprintf(fp, "%d %d %d %d %d %d %d %d %d %f %d %f %d %d %d %d %d %d %d %d\n\n",
				object_code,
				sub_type,
				line_style,
				thickness,
				pen_color,
				fill_color,
				depth,
				pen_style,
				area_fill,
				style_val,
				direction,
				angle,
				center_x,
				center_y,
				radius_x,
				radius_y,
				start_x,
				start_y,	
				end_x,	
				end_y);				
}

void write_ellipse(float cx, float cy, float rx, float ry, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int		object_code	= 1;
	int		sub_type	= 1;
	int		line_style	= Solid;
	float	style_val	= 1.0;
	int		direction	= 1;
	float	angle		= -atan2f(attributes->Mat[1], attributes->Mat[0]);	// to be calculated from matrix
	int		center_x	= cx;
	int		center_y	= cy;
	int		radius_x	= (rx * attributes->Mat[0] * scale);
	int 	radius_y	= (ry * attributes->Mat[3] * scale);
	int		start_x		= 0;
	int 	start_y		= 0;
	int		end_x		= 0;
	int		end_y		= 0;
	int		pen_color	= rgb_to_pen(attributes->stroke, FALSE);
	int		fill_color	= rgb_to_fill(attributes, TRUE);
	int		thickness	= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int		depth		= 0;
	int		pen_style	= 0;
	int		area_fill	= 20;
	
	if(fill_color == -1)
	{
		/* couldn't find matching standard color */
		fill_color = write_usercolor(attributes->fill, fp);
	}
	else
	{
		area_fill = (attributes->fill > 0x00FFFFFF)?-1:fill_color % 100;
		fill_color /= 100;
	}
	
	transform_point(&cx, &cy, attributes);
	center_x = round(cx * scale);
	center_y = round(cy * scale);
	
	fprintf(fp, "%d %d %d %d %d %d %d %d %d %f %d %f %d %d %d %d %d %d %d %d\n\n",
				object_code,
				sub_type,
				line_style,
				thickness,
				pen_color,
				fill_color,
				depth,
				pen_style,
				area_fill,
				style_val,
				direction,
				angle,
				center_x,
				center_y,
				radius_x,
				radius_y,
				start_x,
				start_y,	
				end_x,	
				end_y);
}



void write_line(float x1, float y1, float x2, float y2, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int *int_points;
	int sub_type		= 1;
	int line_style		= Solid;
	int pen_color		= rgb_to_pen(attributes->stroke, FALSE);
	int fill_color		= 0;
	int thickness		= 	((attributes->stroke_width == 0) || (pen_color > 0x00FFFFFF))?
							0:max((attributes->stroke_width * attributes->Mat[0] * scale) / FIG_DPI * FIG_LINE_DPI,1);
	int depth			= 0;
	int area_fill		= -1;
	float style_val		= 1.0;
	int join_style		= Miter;
	int cap_style		= Butt;
	int radius			= 0;
	int forward_arrow	= 0;
	int backward_arrow	= 0;

	int_points = malloc(2 * 2 * sizeof(int));
	transform_point(&x1, &y1, attributes);
	transform_point(&x2, &y2, attributes);
	int_points[0] = round(x1 * scale);
	int_points[1] = round(y1 * scale);
	int_points[2] = round(x2 * scale);
	int_points[3] = round(y2 * scale);
		
	write_PolyType(	fp,
					sub_type,
					line_style,
					thickness,
					pen_color,
					fill_color,
					depth,	
					area_fill,
					style_val,
					join_style,
					cap_style,
					radius,
					forward_arrow,
					backward_arrow,
					2,
					int_points);
	free(int_points);
}



void write_text(char *text, struct SVG_Attr *attributes, float scale, FILE *fp)
{
	int		    l, len, lx, n;
	char	    c;
	char	    buf[4];
	
	int		object		= 4;
	int		sub_type	= 0;
	int		color		= rgb_to_pen(attributes->fill, FALSE);
	int		depth		= 0;
	int		pen_style	= 0;
	// using first PS font at the moment, correspondance table to be found
	int		font 		= 0;
	int		font_size 	= max((attributes->font.font_size * attributes->Mat[0] * scale) / FIG_DPI * 72,1);
	float	angle		= -atan2(attributes->Mat[2], attributes->Mat[0]);
	// bit 2 set => PS font
	int		font_flags	= 0x04; 
	int		height		= (attributes->viewport_height * attributes->Mat[3] * scale);
	int		length		= (attributes->viewport_width * attributes->Mat[0] * scale);
	int		x			= attributes->viewport_x;
	int		y			= attributes->viewport_y;
	float	tmpx		= attributes->viewport_x;
	float	tmpy		= attributes->viewport_y;
	
	transform_point(&tmpx, &tmpy, attributes);
	x = round(tmpx * scale);
	y = round(tmpy * scale);
	
	if (length == 0)
		return;
	fprintf(fp, "%d %d %d %d %d %d %d %.4f %d %d %d %d %d ",
	        object, sub_type, color, depth, pen_style,
	        font, font_size, angle,
	        font_flags, height, length,
	        x, y);
			
	len = strlen(text);
	for (l=0; l<len; l++)
	{
		c = text[l];
		if (c == '\\')
			fprintf(fp,"\\\\");		/* escape a '\' with another one */
		else if ((unsigned int) c <= 255)
			putc(c,fp);				/* normal 7-bit ASCII */
		else
		{
			n = ((int) c)&255;		/* 8-bit, make \xxx (octal) */
			buf[3]='\0';
			for (lx = 2; lx>=0; lx--)
			{
				buf[lx] = '0'+(n%8);
				n /= 8;
			}
			fprintf(fp,"\\%s",buf);
		}
	}
	fprintf(fp,"\\001\n");			/* finish off with '\001' string */
}


void write_PolyType(FILE *fp,
					int   sub_type,
					int   line_style,
					int   thickness,
					int   pen_color,
					int   fill_color,
					int   depth,	
					int   area_fill,
					float style_val,
					int   join_style,
					int   cap_style,
					int   radius,
					int   forward_arrow,
					int   backward_arrow,
					int   nbpoints,
					int   *points)

{
	int i;

	fprintf(fp, "2 %d %d %d %d %d %d 0 %d %f %d %d %d %d %d %d\n",
				sub_type,
				line_style,
				thickness,
				pen_color,
				fill_color,
				depth,	
				area_fill,
				style_val,
				join_style,
				cap_style,
				radius,
				forward_arrow,
				backward_arrow,
				nbpoints);
				
	for(i=0; i<nbpoints * 2;i++)
	{
		if ((i+1)%20 == 1) fprintf(fp, "\t");
		fprintf(fp, "%d ",points[i]);
		if ((i+1)%20 == 0) fprintf(fp, "\n");
	}
	
	fprintf(fp,"\n\n");
}

void write_gradient(struct SVG_Gradient *gradient, int grad_number, FILE *fp)
{
	int j;
	
	fprintf(fp, "0 %d", grad_number+32+512);
	fprintf(fp, " %d %d %d %d %d %d %d %d %d\n",
			gradient->type, gradient->spread, gradient->unit,
			gradient->x1, gradient->y1,
			gradient->x2, gradient->y2,
			gradient->radius, gradient->stops);

	for (j=0 ; j < gradient->stops ; j++)
	{
		fprintf(fp," #%08x %d", gradient->Stop[j].color, gradient->Stop[j].stopvalue);
	}
	fprintf(fp,"\n");
}

int write_usercolor(ULONG rgb, FILE *fp)
{
	static int 		numusercolor = 0;
	static ULONG 	usercolor[512];
	int				i=0;
	int				fill;
	
	/* Check for already saved user colors */
	while((i<numusercolor) && (usercolor[i] != rgb)) i++;
	
	if (i<numusercolor)
	{
		fill = numusercolor + 32;
	}	
	else if (numusercolor < 512)
	{
		fprintf(fp, "0 %d #%06x\n", numusercolor+32, rgb);
		usercolor[numusercolor] = rgb;
		fill = numusercolor + 32;
		numusercolor++;
	}
	else
	{
		/* if no more room for user colors use black */
		fill = 0;
	}
	return(fill);
}
