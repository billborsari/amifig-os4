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

#define VERSION "v1.1"

#define LONG  int 
#define ULONG unsigned int
#define BOOL  unsigned int
#define UBYTE unsigned char
#define FALSE 0
#define TRUE  1

#define FIG_DPI 		1200
#define FIG_LINE_DPI	80

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#ifndef max
#define max(x,y) ((x)>(y))?(x):(y)
#endif

#include <stdlib.h>
#include <stdio.h>

struct SVG_font
{
	char*	font_family;
	char*	font_style;
	char*	font_variant;
	int		font_weight;
	int		font_stretch;
	int		font_size;
	int		font_size_adjust;
};

struct SVG_Attr
{
	float				viewport_x;
	float				viewport_y;	
	float				viewport_width;
	float				viewport_height;	
	ULONG				fill;
	char				gradient_id[128];
	ULONG				stroke;
	float				stroke_width;
	float				Mat[6];
	struct SVG_font		font;
};

struct svg_named_color {
	const char 	*name;
	ULONG 		color;
};

struct GradientStop
{
    ULONG   color;
    LONG    stopvalue;
};

struct SVG_Gradient
{
	char		 		id[128];
    LONG         		type;
    LONG         		spread;
    LONG         		unit;
    LONG         		x1;
    LONG         		y1;
    LONG         		x2;
    LONG         		y2;
    LONG         		radius;
    LONG         		stops;
    struct GradientStop Stop[16];
};


/* colors.gperf */
const struct svg_named_color *
		svg_color_lookup(register const char *str,
				register unsigned int len);


#define HORIZONTAL	0x00
#define VERTICAL 	0x01
#define RGB(r, g, b) ((r) << 16 | (g) << 8 | (b))


extern struct SVG_Gradient *gradient[512];
extern int	   				num_gradient;


// proto
void svg_parse_color(const char *s, ULONG *c, struct SVG_Attr *attributes);
float string_to_length(const char *s, struct SVG_Attr *attributes, int axes);
float string_to_percent(const char *s);
void transform_point(float *x, float *y, struct SVG_Attr *attributes);

// path2poly.c
float * path2poly(char *s, int *nbpoints, int *nbchar, int *type, int interp_steps);

// fig_colors.c
int rgb_to_pen(ULONG rgb, BOOL exact);
int rgb_to_fill(struct SVG_Attr *attributes, BOOL exact);

//fig_save.c
void write_header(FILE *fp);
void write_rect(float x, float y, float w, float h, float rx, float ry, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_circle(float cx, float cy, float r, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_ellipse(float cx, float cy, float rx, float ry, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_line(float x1, float y1, float x2, float y2, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_polygon(float *points, int nbpoints, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_polyline(float *points, int nbpoints, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_text(char *text, struct SVG_Attr *attributes, float scale, FILE *fp);
void write_gradient(struct SVG_Gradient *gradient, int grad_number, FILE *fp);
int  write_usercolor(ULONG rgb, FILE *fp);


