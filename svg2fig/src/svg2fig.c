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
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "svg2fig.h"

static void svg_parse(xmlNode *a_node, struct SVG_Attr attributes);
static void svg_parse_position(xmlNode *cur_node, struct SVG_Attr *attributes);
static void svg_parse_font(xmlNode *cur_node, struct SVG_Attr *attributes);
static void svg_parse_paint(xmlNode *cur_node, struct SVG_Attr *attributes);
static void svg_parse_transform(xmlNode *cur_node, struct SVG_Attr *attributes);
static void svg_parse_radialgradient(xmlNode *cur_node, struct SVG_Gradient **gradient);
static void svg_parse_lineargradient(xmlNode *cur_node, struct SVG_Gradient **gradient);
static void svg_parse_gradientstops(xmlNode *cur_node, struct SVG_Gradient *gradient);


// AROS libc is missing some functions that are handy for the application
#ifdef __AROS__
size_t strnlen (const char *string, size_t maxlen);
char *strndup (const char *s, size_t  n);
#endif

float scale = 1200/80;
int interp_steps = 50;
FILE *outfile = NULL;
struct SVG_Gradient *gradient[512] = {NULL};
int	   num_gradient = 0;

int main(int argc, char **argv)
{
	int i;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
	struct SVG_Attr attributes =
	{
		0.0,			// float	viewport_x;
		0.0,			// float	viewport_y;
		12.0,			// float	viewport_width;
		4.0,			// float	viewport_height;
		020,			// int		fill;
		"",				// char		gradient_id[128];
		0,				// int		stroke;
		0.0,			// float	stroke_width;
		{1,0,0,1,0,0},	// float	Mat[6];
		// font
		{
		"",				// char*	font_family;
		"",				// char*	font_style;
		"",				// char*	font_variant;
		400,			// int		font_weight;
		0,				// int		font_stretch;
		12,				// int		font_size;
		0}				// int		font_size_adjust;
	};

    if (argc < 2)
	{
		printf("SVG2FIG %s\n",VERSION);
		printf("(c)Yannick Erb, September 2010\n\n");
		printf("Usage:\n");
		printf("svg2fig infile [-o outfile] [-s scale] [-i interpolation_steps]\n");
		printf("infile                Input File\n");
		printf("outfile               Output File, default output to stdout\n");
		printf("scale                 Scales output (default 1.0)\n");
		printf("interpolation_steps   Number of steps used to interpolate Bezier Curves (default 50)\n");
		return(1);
	}
	
    // default output is to stdout
    outfile = stdout;	
	for (i=2;i<argc;i++)
	{
		if (strcmp(argv[i],"-o")==0)
		{
			i++;
			outfile = fopen(argv[i],"w");
		}
		else if (strcmp(argv[i],"-s")==0)
		{
			i++;
			scale = scale * atof(argv[i]);
			if (scale < 1.0) scale=1.0;
		}
		else if (strcmp(argv[i],"-i")==0)
		{
			i++;
			interp_steps = atoi(argv[i]);
			if (interp_steps < 5) interp_steps = 5;
		}
	}	
	
    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION



    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
        printf("error: could not parse file %s\n", argv[1]);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    svg_parse(root_element, attributes);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
	
	if (outfile!=stdout) fclose(outfile);
    
    for(i=0;i<512;i++)
        if (gradient[i]) free(gradient[i]);

    return 0;
}

static void svg_parse(xmlNode * a_node, struct SVG_Attr attributes)
{
    xmlNode *cur_node = NULL;
	char *s = NULL;
	struct SVG_Attr object_attributes;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((char *)cur_node->name,"svg")==0)
			{
				// main svg entry
				// look for global attributes
				svg_parse_position(cur_node, &attributes);
				svg_parse_transform(cur_node, &attributes);
				svg_parse_paint(cur_node, &attributes);
				svg_parse_font(cur_node, &attributes);
				
				// write fig header
				write_header(outfile);
				
				// look for svg children
				svg_parse(cur_node->children, attributes);
			}
			else if (strcmp((char *)cur_node->name,"defs")==0)
			{
				// defs entry = > look for gradient childrens
				svg_parse(cur_node->children, attributes);
			} 
			else if (strcmp((char *)cur_node->name,"g")==0)
			{
				// sub group entry
				struct SVG_Attr group_attributes;
				
				memcpy(&group_attributes, &attributes, sizeof(struct SVG_Attr));
				
				// look for group attributes
				svg_parse_position(cur_node, &group_attributes);
				svg_parse_transform(cur_node, &group_attributes);
				svg_parse_paint(cur_node, &group_attributes);
				svg_parse_font(cur_node, &group_attributes);				
				
				// look for group children
				svg_parse(cur_node->children, group_attributes);
			} 
			else
			{
				memcpy(&object_attributes, &attributes, sizeof(struct SVG_Attr));
				// look for object attributes
				svg_parse_position(cur_node, &object_attributes);
				svg_parse_transform(cur_node, &object_attributes);
				svg_parse_paint(cur_node, &object_attributes);
				svg_parse_font(cur_node, &object_attributes);	
			}
			
			/* Check for gradient definitions */
			if (strcmp((char *)cur_node->name,"radialGradient")==0)
			{
				if (num_gradient < 512)
				{
					svg_parse_radialgradient(cur_node, &gradient[num_gradient]);
					write_gradient(gradient[num_gradient], num_gradient, outfile);
					num_gradient++;
				}
			}
			else if (strcmp((char *)cur_node->name,"linearGradient")==0)
			{
				if (num_gradient < 512)
				{
					svg_parse_lineargradient(cur_node, &gradient[num_gradient]);
					write_gradient(gradient[num_gradient], num_gradient, outfile);
					num_gradient++;
				}
			}
			/* Check for objects */
			else if (strcmp((char *)cur_node->name,"rect")==0)
			{
				// Rectangle, read attributes
				float x,y,w,h,rx,ry;

				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x");
				if (s) x = string_to_length(s, &object_attributes, HORIZONTAL);
				else x = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y");
				if (s) y = string_to_length(s, &object_attributes, VERTICAL);
				else y = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "width");
				if (s) w = string_to_length(s, &object_attributes, HORIZONTAL);
				else w = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "height");
				if (s) h = string_to_length(s, &object_attributes, VERTICAL);
				else h = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "rx");
				if (s) rx = string_to_length(s, &object_attributes, HORIZONTAL);
				else rx = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "ry");
				if (s) ry = string_to_length(s, &object_attributes, VERTICAL);
				else ry = rx;
				xmlFree(s);

				write_rect(x,y,w,h,rx,ry,&object_attributes,scale,outfile);				
			}
			else if (strcmp((char *)cur_node->name,"circle")==0)
			{
				// Circle, read attributes
				float cx,cy,r;

				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cx");
				if (s) cx = string_to_length(s, &object_attributes, HORIZONTAL);
				else cx = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cy");
				if (s) cy = string_to_length(s, &object_attributes, VERTICAL);
				else cy = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "r");
				if (s) r = string_to_length(s, &object_attributes, HORIZONTAL);
				else r = 0.0;
				xmlFree(s);

				write_circle(cx,cy,r, &object_attributes, scale, outfile);
			}
			else if (strcmp((char *)cur_node->name,"ellipse")==0)
			{
				// Ellipse, read attributes
				float cx,cy,rx,ry;

				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cx");
				if (s) cx = string_to_length(s, &object_attributes, HORIZONTAL);
				else cx = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cy");
				if (s) cy = string_to_length(s, &object_attributes, VERTICAL);
				else cy = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "rx");
				if (s) rx = string_to_length(s, &object_attributes, HORIZONTAL);
				else rx = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "ry");
				if (s) ry = string_to_length(s, &object_attributes, VERTICAL);
				else ry = rx;
				xmlFree(s);

				write_ellipse(cx,cy,rx,ry, &object_attributes, scale, outfile);
			}
 			else if (strcmp((char *)cur_node->name,"line")==0)
			{
				// Line, read attributes
				float x1,y1,x2,y2;

				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x1");
				if (s) x1 = string_to_length(s, &object_attributes, HORIZONTAL);
				else x1 = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y1");
				if (s) y1 = string_to_length(s, &object_attributes, VERTICAL);
				else y1 = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x2");
				if (s) x2 = string_to_length(s, &object_attributes, HORIZONTAL);
				else x2 = 0.0;
				xmlFree(s);
				s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y2");
				if (s) y2 = string_to_length(s, &object_attributes, VERTICAL);
				else y2 = 0.0;
				xmlFree(s);

				write_line(x1, y1, x2, y2, &object_attributes, scale, outfile);
			}
 			else if ((strcmp((char *)cur_node->name,"polyline")==0) ||
					 (strcmp((char *)cur_node->name,"polygon")==0))
			{
				// Polyline or Polygon, read attributes
				float *points,dx,dy;
				int nbpoints = 0,i = 0,n;
				char *s_save;
				
				s = s_save = (char *) xmlGetProp(cur_node, (const xmlChar *) "points");

				for (i = 0; s[i]; i++)
					if (s[i] == ',') s[i] = ' ';
				i = 0;
				
				// first scan to count number of points
				while (*s)
				{
					if (sscanf(s, "%f %f %n", &dx, &dy, &n) == 2)
					{
						nbpoints++;
						s += n;
					}
					else
						break;
				}

				// now we can allocate the memory
				points = (float *)malloc(nbpoints * 2 * sizeof(float));
				
				s = s_save;
				
				// second scan to read points
				while (*s)
				{
					if (sscanf(s, "%f %f %n", &points[i*2], &points[i*2+1], &n) == 2)
					{
						i++;
						s += n;
					}
					else
						break;
				}

				if (strcmp((char *)cur_node->name,"polyline")==0)
					write_polyline(points, nbpoints, &object_attributes, scale, outfile);
				else
					write_polygon(points, nbpoints, &object_attributes, scale, outfile);
					
				free(points);
				xmlFree(s_save);
			}
 			else if (strcmp((char *)cur_node->name,"path")==0)
			{
				// Path, read attributes
				float 	*points;
				int		nbpoints = 0;
				int		nbchar = 0;
				int		type = 0;
				char	*s_save;
				
				s = s_save = (char *) xmlGetProp(cur_node, (const xmlChar *) "d");

				while (*s)
				{
					nbpoints = 0;
					points = path2poly(s, &nbpoints, &nbchar, &type, interp_steps);
					s += nbchar;
					if (points == NULL) // error while reading path.
					{
						printf("#Error while reading path\n");
						break;
					}
					if (type == 1)
						write_polygon(points, nbpoints, &object_attributes, scale, outfile);
					else
						write_polyline(points, nbpoints, &object_attributes, scale, outfile);
					free(points);
				}
				xmlFree(s_save);
			}
 			else if (strcmp((char *)cur_node->name,"text")==0)
			{
				// Text, read attributes
				char *text;
				xmlNode *child;
				
				text = "";

				for (child = cur_node->children; child; child = child->next)
				{
					if (child->type == XML_TEXT_NODE)
						text = strdup((const char *) child->content);
					else if (child->type == XML_ELEMENT_NODE &&
							strcmp((const char *) child->name,"tspan") == 0)
					{
						// parse_text(child, state);
					}
				}
		
				write_text(text, &object_attributes, scale, outfile);
			}
		}
    }
}


static void svg_parse_radialgradient(xmlNode *cur_node, struct SVG_Gradient **grad)
{
	char *s = NULL;
	struct SVG_Gradient * gradient = (struct SVG_Gradient *)malloc(sizeof(struct SVG_Gradient));
	
	/* set default values */
	strncpy(gradient->id, "", 128);
    gradient->type = 1;
    gradient->spread = 0;
    gradient->unit = 0;
    gradient->x1 = 50;
    gradient->y1 = 50;
    gradient->x2 = 50;
    gradient->y2 = 50;
    gradient->radius = 50;
    gradient->stops = 0;
	
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "id");
	if (s) strncpy(gradient->id, s, 128);
    xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "gradientUnits");
	if ((s) && (strcmp((char *)s, "userSpaceOnUse")==0)) gradient->unit = 1;
	xmlFree(s);	

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "spreadMethod");
	if (s)
	{	
		if (strcmp((char *)s, " repeat") == 0)
			gradient->spread = 1;
		else if (strcmp((char *)s, "reflect") == 0)
			gradient->spread = 2;
	}
	xmlFree(s);	

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cx");
	if (s) 
	{
		gradient->x1 = round(string_to_percent(s));
		gradient->x2 = gradient->x1;
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "cy");
	if (s)
	{
		gradient->y1 = round(string_to_percent(s));
		gradient->y2 = gradient->y1;
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "r");
	if (s)
	{
		gradient->radius = round(string_to_percent(s));
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "fx");
	if (s)
	{
		gradient->x2 = round(string_to_percent(s));
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "fy");
	if (s)
	{
		gradient->y2 = round(string_to_percent(s));
	}
	xmlFree(s);
	
	/* parse stops */ 
	svg_parse_gradientstops(cur_node->children, gradient);
    *grad = gradient;
}

static void svg_parse_lineargradient(xmlNode *cur_node, struct SVG_Gradient **grad)
{
	char *s = NULL;
	struct SVG_Gradient *gradient = (struct SVG_Gradient *)malloc(sizeof(struct SVG_Gradient));

	/* set default values */
	strncpy(gradient->id, "", 128);
    gradient->type = 0;
    gradient->spread = 0;
    gradient->unit = 0;
    gradient->x1 = 0;
    gradient->y1 = 0;
    gradient->x2 = 100;
    gradient->y2 = 0;
    gradient->radius = 0;
    gradient->stops = 0;
	
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "id");
	if (s) strncpy(gradient->id, s, 128);
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "gradientUnits");
	if ((s) && (strcmp((char *)s, "userSpaceOnUse")==0)) gradient->unit = 1;
	xmlFree(s);	

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "spreadMethod");
	if (s)
	{	
		if (strcmp((char *)s, " repeat") == 0)
			gradient->spread = 1;
		else if (strcmp((char *)s, "reflect") == 0)
			gradient->spread = 2;
	}
	xmlFree(s);	
	
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x1");
	if (s) 
	{
		gradient->x1 = round(string_to_percent(s));
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x2");
	if (s)
	{
		gradient->x2 = round(string_to_percent(s));
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y1");
	if (s)
	{
		gradient->y1 = round(string_to_percent(s));
	}
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y2");
	if (s)
	{
		gradient->y2 = round(string_to_percent(s));
	}
	xmlFree(s);

	/* parse stops */ 
	svg_parse_gradientstops(cur_node->children, gradient);
    *grad = gradient;
}

static void svg_parse_gradientstops(xmlNode * a_node, struct SVG_Gradient *gradient)
{
    xmlNode *cur_node = NULL;
	char *s = NULL;
    char *substr = NULL;
    char *sep = NULL;
	float tmp;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
			if (strcmp((char *)cur_node->name,"stop")==0)
			{
				if (gradient->stops<16)
				{
					gradient->stops++;
					
					/* set default values */
					gradient->Stop[gradient->stops-1].stopvalue = 0;
					gradient->Stop[gradient->stops-1].color = 0x00000000;
					
					s = (char *) xmlGetProp(cur_node, (const xmlChar *) "offset");
					if (s)
					{
						gradient->Stop[gradient->stops-1].stopvalue = round(string_to_percent(s));
					}
					xmlFree(s);
					
					s = (char *) xmlGetProp(cur_node, (const xmlChar *) "style");
					if (s)
					{
                        /* stop-color */
                        substr = strstr (s,"stop-color") + 11;
                        sep    = strstr (substr,";");
                        if (sep) *sep   = 0;
						svg_parse_color(substr, &gradient->Stop[gradient->stops-1].color, NULL);
                        if (sep) *sep   = ';';
                        /* stop-opacity */
                        substr = strstr (s,"stop-opacity") + 13;
                        sep    = strstr (substr,";");
                        if (sep) *sep   = 0;
						if (sscanf(substr, "%f", &tmp) == 1)
						{
							gradient->Stop[gradient->stops-1].color |= (ULONG)(round(255 * tmp)) << 24;
						}
					}
					xmlFree(s);
				}
			}
		}
	}
}

static void svg_parse_position(xmlNode *cur_node, struct SVG_Attr *attributes)
{
	char *s = NULL;
	
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "x");
	if (s) attributes->viewport_x = string_to_length(s, attributes, HORIZONTAL);
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "y");
	if (s) attributes->viewport_y = string_to_length(s, attributes, VERTICAL);
	xmlFree(s);
	
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "width");
	if (s) attributes->viewport_width = string_to_length(s, attributes, HORIZONTAL);
	xmlFree(s);

	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "height");
	if (s) attributes->viewport_height = string_to_length(s, attributes, VERTICAL);
	xmlFree(s);
	
	/* parse viewBox */
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "viewBox");
	if (s) {
		float min_x, min_y, vwidth, vheight;
		int i;

		for (i = 0; s[i]; i++)
			if (s[i] == ',') s[i] = ' ';
		i = 0;
		
		if(sscanf(s, "%f %f %f %f", &min_x, &min_y, &vwidth, &vheight) == 4)
		{
			attributes->Mat[0] = (float) attributes->viewport_width / vwidth;
			attributes->Mat[3] = (float) attributes->viewport_height / vheight;
			attributes->Mat[4] += -min_x * attributes->Mat[0];
			attributes->Mat[5] += -min_y * attributes->Mat[3];
		}
	}
}

static void svg_parse_transform(xmlNode *cur_node, struct SVG_Attr *attributes)
{
	char *s = NULL, *add_s = NULL;
	
	// transformation
	s = add_s = (char *) xmlGetProp(cur_node, (const xmlChar *) "transform");
	if (s)
	{
		float a, b, c, d, e, f;
		float za, zb, zc, zd, ze, zf;
		float angle, x, y;
		int n;
		unsigned int i;

		for (i = 0; s[i]; i++)
			if (s[i] == ',')
				s[i] = ' ';

		while (*s) {
			a = d = 1;
			b = c = 0;
			e = f = 0;
			if (sscanf(s, "matrix (%f %f %f %f %f %f) %n", &a, &b, &c, &d, &e, &f, &n) == 6)
				;
			else if (sscanf(s, "translate (%f %f) %n", &e, &f, &n) == 2)
				;
			else if (sscanf(s, "translate (%f) %n", &e, &n) == 1)
				;
			else if (sscanf(s, "scale (%f %f) %n", &a, &d, &n) == 2)
				;
			else if (sscanf(s, "scale (%f) %n", &a, &n) == 1) 
				d = a;
			else if (sscanf(s, "rotate (%f %f %f) %n", &angle, &x, &y, &n) == 3)
			{
				angle = angle / 180 * M_PI;
				a = cos(angle);
				b = sin(angle);
				c = -sin(angle);
				d = cos(angle);
				e = -x * cos(angle) + y * sin(angle) + x;
				f = -x * sin(angle) - y * cos(angle) + y;
			} 
			else if (sscanf(s, "rotate (%f) %n", &angle, &n) == 1)
			{
				angle = angle / 180 * M_PI;
				a = cos(angle);
				b = sin(angle);
				c = -sin(angle);
				d = cos(angle);
			} 
			else if (sscanf(s, "skewX (%f) %n", &angle, &n) == 1)
			{
				angle = angle / 180 * M_PI;
				c = tan(angle);
			} 
			else if (sscanf(s, "skewY (%f) %n", &angle, &n) == 1)
			{
				angle = angle / 180 * M_PI;
				b = tan(angle);
			} 
			else
				break;
			za = attributes->Mat[0] * a + attributes->Mat[2] * b;
			zb = attributes->Mat[1] * a + attributes->Mat[3] * b;
			zc = attributes->Mat[0] * c + attributes->Mat[2] * d;
			zd = attributes->Mat[1] * c + attributes->Mat[3] * d;
			ze = attributes->Mat[0] * e + attributes->Mat[2] * f + attributes->Mat[4];
			zf = attributes->Mat[1] * e + attributes->Mat[3] * f + attributes->Mat[5];
			attributes->Mat[0] = za;
			attributes->Mat[1] = zb;
			attributes->Mat[2] = zc;
			attributes->Mat[3] = zd;
			attributes->Mat[4] = ze;
			attributes->Mat[5] = zf;
			s += n;
		}
	}
	
	xmlFree(add_s);
}

static void svg_parse_paint(xmlNode *cur_node, struct SVG_Attr *attributes)
{
	char *s = NULL;
	BOOL str = FALSE;
	BOOL strwdt = FALSE;
	
	// fill color
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "fill");
	if (s) svg_parse_color(s, &attributes->fill, attributes);
	xmlFree(s);
	
	// Stroke color
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "stroke");
	if (s)
	{
		svg_parse_color(s, &attributes->stroke, attributes);
		attributes->stroke_width = 1.0;
		str = TRUE;
	}
	xmlFree(s);
	
	// Stroke Width
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "stroke-width");
	if (s) 
	{
		attributes->stroke_width = strtod(s,NULL);
		if (!str) attributes->stroke = 0x00000000;
	}
	xmlFree(s);
	
	// Style
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "style");
	if (s)
	{
		const char *s1;
		char *value;
		if ((s1 = strstr(s, "fill:"))) {
			s1 += 5;
			while (*s1 == ' ') s1++;
			value = strndup(s1, strcspn(s1, "; "));
			svg_parse_color(value, &attributes->fill, attributes);
			free(value);
		}
		if ((s1 = strstr(s, "stroke:"))) {
			str = TRUE;
			s1 += 7;
			while (*s1 == ' ') s1++;
			value = strndup(s1, strcspn(s1, "; "));
			svg_parse_color(value, &attributes->stroke, attributes);
			if (!strwdt) attributes->stroke_width = 1.0;
			free(value);
		}
		if ((s1 = strstr(s, "stroke-width:"))) {
			strwdt = TRUE;
			s1 += 13;
			while (*s1 == ' ') s1++;
			attributes->stroke_width = strtod(s1,NULL);
			if (!str) attributes->stroke = 0x00000000;
		}
	}
	xmlFree(s);
}

static void svg_parse_font(xmlNode *cur_node, struct SVG_Attr *attributes)
{
	char *s = NULL;
	
	// fonts
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-family");
	// if (s) printf("%s\n",s);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-style");
	// if (s) printf("%s\n",s);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-variant");
	// if (s) printf("%s\n",s);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-weight");
	// if (s) printf("%s\n",s);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-stretch");
	// if (s) printf("%s\n",s);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-size");
	if (s) attributes->font.font_size = strtod(s,NULL);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font-size-adjust");
	if (s) attributes->font.font_size_adjust = strtod(s,NULL);
	xmlFree(s);
	s = (char *) xmlGetProp(cur_node, (const xmlChar *) "font");
	// if (s) printf("%s\n",s);
	xmlFree(s);
}

void svg_parse_color(const char *s, ULONG *c, struct SVG_Attr *attributes)
{
	ULONG r, g, b;
	float rf, gf, bf;
	size_t len = strlen(s);

	if (len == 4 && s[0] == '#')
	{
		if (sscanf(s + 1, "%1x%1x%1x", &r, &g, &b) == 3)
			*c = RGB(r | r << 4, g | g << 4, b | b << 4);

	} 
	else if (len == 7 && s[0] == '#')
	{
		if (sscanf(s + 1, "%2x%2x%2x", &r, &g, &b) == 3)
			*c = RGB(r, g, b);

	} 
	else if (10 <= len && s[0] == 'r' && s[1] == 'g' && s[2] == 'b' && s[3] == '(' && s[len - 1] == ')') 
	{
		if (sscanf(s + 4, "%u,%u,%u", &r, &g, &b) == 3)
			*c = RGB(r, g, b);
		else if (sscanf(s + 4, "%f%%,%f%%,%f%%", &rf, &gf, &bf) == 3) {
			b = bf * 255 / 100;
			g = gf * 255 / 100;
			r = rf * 255 / 100;
			*c = RGB(r, g, b);
		}

	} 
	else if (len == 4 && strcmp(s, "none") == 0) 
	{
		*c = 0xFF000000; //transparent
	} 
	else if (5 < len && s[0] == 'u' && s[1] == 'r' && s[2] == 'l' && s[3] == '(') 
	{
		if (attributes)
		{
			strncpy(attributes->gradient_id, &s[5], 128);
			attributes->gradient_id[strlen(attributes->gradient_id)-1] = '\0';
		}
		*c = 0x00000000;
	}
	else
	{
		// named color
		const struct svg_named_color *named_color;
		named_color = svg_color_lookup(s, strlen(s));
		if (named_color)
			*c = named_color->color;
	}
}

float string_to_percent(const char *s)
{
	int num_length = strspn(s, "0123456789+-.");
	const char *unit = s + num_length;
	float n = atof((const char *) s);
	
	if (unit[0] == 0) {
		n *= 100;
	} 
	
	return(n);
}

float string_to_length(const char *s, struct SVG_Attr *attributes, int axes)
{
	int num_length = strspn(s, "0123456789+-.");
	const char *unit = s + num_length;
	float n = atof((const char *) s);

	if (unit[0] == 0) {
		return n;
	} else if (unit[0] == '%') {
		return n / 100.0 * ((axes==VERTICAL)?attributes->viewport_height:attributes->viewport_width);
	} else if (unit[0] == 'e' && unit[1] == 'm') {
		return n * attributes->font.font_size;
	} else if (unit[0] == 'e' && unit[1] == 'x') {
		return n / 2.0 * attributes->font.font_size;
	} else if (unit[0] == 'p' && unit[1] == 'x') {
		return n;
	} else if (unit[0] == 'p' && unit[1] == 't') {
		return n * 1.25;
	} else if (unit[0] == 'p' && unit[1] == 'c') {
		return n * 15.0;
	} else if (unit[0] == 'm' && unit[1] == 'm') {
		return n * 3.543307;
	} else if (unit[0] == 'c' && unit[1] == 'm') {
		return n * 35.43307;
	} else if (unit[0] == 'i' && unit[1] == 'n') {
		return n * 90;
	}

	return 0;
}

void transform_point(float *x, float *y, struct SVG_Attr *attributes)
{
	float xO, yO;
	
	xO = *x;
	yO = *y;
			
	*x = attributes->Mat[0] * xO + attributes->Mat[2] * yO + attributes->Mat[4];
	*y = attributes->Mat[1] * xO + attributes->Mat[3] * yO + attributes->Mat[5];
}

// AROS libc is missing some functions that are handy for the application
#ifdef __AROS__
size_t strnlen (const char *string, size_t maxlen)
{
  const char *end = memchr (string, '\0', maxlen);
  return end ? (size_t) (end - string) : maxlen;
}

char *strndup (const char *s, size_t  n)
{
  size_t len = strnlen (s, n);
  char *new = malloc (len + 1);

  if (new == NULL)
    return NULL;

  new[len] = '\0';
  return memcpy (new, s, len);
}
#endif
