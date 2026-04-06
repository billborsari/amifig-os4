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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/u_draw.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_draw.h"
#include "u_fonts.h"
#include "u_geom.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_msgpanel.h"
#include "a_coords.h"
#include "f_picobj.h"
/************** ARRAY FOR ARROW SHAPES **************/ 

struct _fpnt { 
		double x,y;
	};

struct _arrow_shape {
		int	numpts;		/* number of points in arrowhead */
		int	tipno;		/* which point contains the tip */
		int	numfillpts;	/* number of points to fill */
		Boolean	simplefill;	/* if true, use points array to fill otherwise use fill_points array */
		Boolean	clip;		/* if false, no clip area needed (e.g. for reverse triangle arrowhead) */
		Boolean	half;		/* if true, arrowhead is half-wide and must be shifted to cover the line */
		double	tipmv;		/* acuteness of tip (smaller angle, larger tipmv) */
		struct	_fpnt points[6]; /* points in arrowhead */
		struct	_fpnt fillpoints[6]; /* points to fill if not "simple" */
	};

static struct _arrow_shape arrow_shapes[NUM_ARROW_TYPES] = {
		   /* number of points, index of tip, {datapairs} */
		   /* first point must be upper-left point of tail, then tip */

		   /* type 0 */
		   { 3, 1, 0, True, True, False, 2.15, {{-1,0.5}, {0,0}, {-1,-0.5}}},
		   /* place holder for what would be type 0 filled */
		   { 0 },
		   /* type 1a simple triangle */
		   { 4, 1, 0, True, True, False, 2.1, {{-1.0,0.5}, {0,0}, {-1.0,-0.5}, {-1.0,0.5}}},
		   /* type 1b filled simple triangle*/
		   { 4, 1, 0, True, True, False, 2.1, {{-1.0,0.5}, {0,0}, {-1.0,-0.5}, {-1.0,0.5}}},
		   /* type 2a concave spearhead */
		   { 5, 1, 0, True, True, False, 2.6, {{-1.25,0.5},{0,0},{-1.25,-0.5},{-1.0,0},{-1.25,0.5}}},
		   /* type 2b filled concave spearhead */
		   { 5, 1, 0, True, True, False, 2.6, {{-1.25,0.5},{0,0},{-1.25,-0.5},{-1.0,0},{-1.25,0.5}}},
		   /* type 3a convex spearhead */
		   { 5, 1, 0, True, True, False, 1.5, {{-0.75,0.5},{0,0},{-0.75,-0.5},{-1.0,0},{-0.75,0.5}}},
		   /* type 3b filled convex spearhead */
		   { 5, 1, 0, True, True, False, 1.5, {{-0.75,0.5},{0,0},{-0.75,-0.5},{-1.0,0},{-0.75,0.5}}},
		   /* type 4a diamond */
		   { 5, 1, 0, True, True, False, 1.15, {{-0.5,0.5},{0,0},{-0.5,-0.5},{-1.0,0},{-0.5,0.5}}},
		   /* type 4b filled diamond */
		   { 5, 1, 0, True, True, False, 1.15, {{-0.5,0.5},{0,0},{-0.5,-0.5},{-1.0,0},{-0.5,0.5}}},
		   /* type 5a/b circle - handled in code */
		   { 0, 0, 0, True, True, False, 0.0 },
		   { 0, 0, 0, True, True, False, 0.0 },
		   /* type 6a/b half circle - handled in code */
		   { 0, 0, 0, True, True, False, -1.0 },
		   { 0, 0, 0, True, True, False, -1.0 },
		   /* type 7a square */
		   { 5, 1, 0, True, True, False, 0.0, {{-1.0,0.5},{0,0.5},{0,-0.5},{-1.0,-0.5},{-1.0,0.5}}},
		   /* type 7b filled square */
		   { 5, 1, 0, True, True, False, 0.0, {{-1.0,0.5},{0,0.5},{0,-0.5},{-1.0,-0.5},{-1.0,0.5}}},
		   /* type 8a reverse triangle */
		   { 4, 1, 0, True, False, False, 0.0, {{-1.0,0},{0,0.5},{0,-0.5},{-1.0,0}}},
		   /* type 8b filled reverse triangle */
		   { 4, 1, 0, True, False, False, 0.0, {{-1.0,0},{0,0.5},{0,-0.5},{-1.0,0}}},

		   /* type 9a top-half filled concave spearhead */
		   { 5, 1, 3, False, True, False, 2.6, {{-1.25,0.5},{0,0},{-1.25,-0.5},{-1.0,0},{-1.25,0.5}},
			   			 {{-1.25,-0.5},{0,0},{-1,0}}},
		   /* type 9b bottom-half filled concave spearhead */
		   { 5, 1, 3, False, True, False, 2.6, {{-1.25,0.5},{0,0},{-1.25,-0.5},{-1.0,0},{-1.25,0.5}},
			   			 {{-1.25,0.5},{0,0},{-1,0}}},

		   /* type 10o top-half simple triangle */
		   { 4, 1, 0, True, True, True, 2.5, {{-1.0,0.5}, {0,0}, {-1,0.0}, {-1.0,0.5}}},
		   /* type 10f top-half filled simple triangle*/
		   { 4, 1, 0, True, True, True, 2.5, {{-1.0,0.5}, {0,0}, {-1,0.0}, {-1.0,0.5}}},
		   /* type 11o top-half concave spearhead */
		   { 4, 1, 0, True, True, True, 3.5, {{-1.25,0.5}, {0,0}, {-1,0}, {-1.25,0.5}}},
		   /* type 11f top-half filled concave spearhead */
		   { 4, 1, 0, True, True, True, 3.5, {{-1.25,0.5}, {0,0}, {-1,0}, {-1.25,0.5}}},
		   /* type 12o top-half convex spearhead */
		   { 4, 1, 0, True, True, True, 2.5, {{-0.75,0.5}, {0,0}, {-1,0}, {-0.75,0.5}}},
		   /* type 12f top-half filled convex spearhead */
		   { 4, 1, 0, True, True, True, 2.5, {{-0.75,0.5}, {0,0}, {-1,0}, {-0.75,0.5}}},

		   /* type 13a "wye" */
		   { 3, 0, 0, True, True, False, -1.0, {{0,0.5},{-1.0,0},{0,-0.5}}},
		   /* type 13b bar */
		   { 2, 1, 0, True, True, False, 0.0, {{0,0.5},{0,-0.5}}},
		   /* type 14a two-prong fork */
		   { 4, 0, 0, True, True, False, -1.0, {{0,0.5},{-1.0,0.5},{-1.0,-0.5},{0,-0.5}}},
		   /* type 14b backward two-prong fork */
		   { 4, 1, 0, True, True, False, 0.0, {{-1.0,0.5,},{0,0.5},{0,-0.5},{-1.0,-0.5}}},
		};

/* these are for the arrowheads */
static zXPoint	    farpts[50],barpts[50];
static zXPoint	    farfillpts[50], barfillpts[50];
static int	    	nfpts, nbpts, nffillpts, nbfillpts;

/************* Code begins here *************/
void clip_arrows (F_line *obj, int objtype, int op, int skip);
void draw_arrow (F_line *obj, F_arrow *arrow, zXPoint *points, int npoints, zXPoint *points2, int npoints2, int op);

/*********************** ARC ***************************/

void draw_arc( F_arc *a, int op)
{
	pw_arc( a, op);

	/* draw the arrowheads, if any */
	if (a->type != T_PIE_WEDGE_ARC)
	{
        draw_arrows((F_line *)a, O_ARC, op, 0);
	}
}

/*********************** ELLIPSE ***************************/

void draw_ellipse( F_ellipse *e, int op)
{
	pw_ellipse( e, op);
}
/*
 *  An Ellipse Generator.
 *  Written by James Tough   7th May 92
 *
 *  The following routines displays a filled ellipse on the screen from the
 *    semi-minor axis 'a', semi-major axis 'b' and angle of rotation
 *    'phi'.
 *
 *  It works along these principles .....
 *
 *        The standard ellipse equation is
 *
 *             x*x     y*y
 *             ---  +  ---
 *             a*a     b*b
 *
 *
 *        Rotation of a point (x,y) is well known through the use of
 *
 *            x' = x*COS(phi) - y*SIN(phi)
 *            y' = y*COS(phi) + y*COS(phi)
 *
 *        Taking these to together, this gives the equation for a rotated
 *      ellipse centered around the origin.
 *
 *           [x*COS(phi) - y*SIN(phi)]^2   [x*SIN(phi) + y*COS(phi)]^2
 *           --------------------------- + ---------------------------
 *                      a*a                           b*b
 *
 *        NOTE -  some of the above equation can be precomputed, eg,
 *
 *              i = COS(phi)/a        and        j = SIN(phi)/b
 *
 *        NOTE -  y is constant for each line so,
 *
 *              m = -yk*SIN(phi)/a    and     n = yk*COS(phi)/b
 *              where yk stands for the kth line ( y subscript k)
 *
 *        Where yk=y, we get a quadratic,
 *
 *              (i*x + m)^2 + (j*x + n)^2 = 1
 *
 *        Thus for any particular line, y, there is two corresponding x
 *      values. These are the roots of the quadratic. To get the roots,
 *      the above equation can be rearranged using the standard method,
 *
 *          -(i*m + j*n) +- sqrt[i^2 + j^2 - (i*n -j*m)^2]
 *      x = ----------------------------------------------
 *                           i^2 + j^2
 *
 *        NOTE -  again much of this equation can be precomputed.
 *
 *           c1 = i^2 + j^2
 *           c2 = [COS(phi)*SIN(phi)*(a-b)]
 *           c3 = [b*b*(COS(phi)^2) + a*a*(SIN(phi)^2)]
 *           c4 = a*b/c3
 *
 *      x = c2*y +- c4*sqrt(c3 - y*y),    where +- must be evaluated once
 *                                      for plus, and once for minus.
 *
 *        We also need to know how large the ellipse is. This condition
 *      arises when the sqrt of the above equation evaluates to zero.
 *      Thus the height of the ellipse is give by
 *
 *              sqrt[ b*b*(COS(phi)^2) + a*a*(SIN(phi)^2) ]
 *
 *       which just happens to be equal to sqrt(c3).
 *
 *         It is now possible to create a routine that will scan convert
 *       the ellipse on the screen.
 *
 *        NOTE -  c2 is the gradient of the new ellipse axis.
 *                c4 is the new semi-minor axis, 'a'.
 *           sqr(c3) is the new semi-major axis, 'b'.
 *
 *         These values could be used in a 4WS or 8WS ellipse generator
 *       that does not work on rotation, to give the feel of a rotated
 *       ellipse. These ellipses are not very accurate and give visable
 *       bumps along the edge of the ellipse. However, these routines
 *       are very quick, and give a good approximation to a rotated ellipse.
 *
 *       NOTES on the code given.
 *
 *           All the routines take there parameters as ( x, y, a, b, phi ),
 *           where x,y are the center of the ellipse ( relative to the
 *           origin ), a and b are the vertical and horizontal axis, and
 *           phi is the angle of rotation in RADIANS.
 *
 *           The 'moveto(x,y)' command moves the screen cursor to the
 *               (x,y) point.
 *           The 'lineto(x,y)' command draws a line from the cursor to
 *               the point (x,y).
 *
 */


/*
 *  QuickEllipse, uses the same method as Ellipse, but uses incremental
 *    methods to reduce the amount of work that has to be done inside
 *    the main loop. The speed increase is very noticeable.
 *
 *  Written by James Tough
 *  7th May 1992
 *
 */
 
/* store the points across (row-wise in) the matrix */
#define newpoint( xp, yp, i, j) \
	if(elltotpts >= MAXPOINTNUM/4) \
	{ \
		if(elltotpts == MAXPOINTNUM/4) \
		{ \
			put_msg((char *)_(msg_TooManyEllPoint), MAXPOINTNUM); \
			elltotpts++; \
		} \
	} \
    else \
    { \
        ellxpoints[i][j]=round(xp); \
        ellypoints[i][j]=round(yp); \
        nump[j]++; \
        elltotpts++; \
        if(++(j) > 3) \
        { \
            j=0; \
            i++; \
        } \
    }

void angle_ellipse( int center_x, int center_y, int radius_x, int radius_y, float angle, int style, PointList *PolyPoints)
{
	int order[4]={0,1,3,2};
	int i,j;
	double	xcen, ycen, a, b;
	double	c1, c2, c3, c4, c5, c6, v1, cphi, sphi, cphisqr, sphisqr;
	double	xleft, xright, d, asqr, bsqr;
	int	ymax, yy=0, k, m, dir;
    
    int	ellxpoints[MAXPOINTNUM/4][4], ellypoints[MAXPOINTNUM/4][4];
    int	nump[4];
    int	elltotpts;

	if(radius_x == 0 || radius_y == 0)
		return;

	xcen=SCALE_DOWN_X_F(center_x);
	ycen=SCALE_DOWN_Y_F(center_y);

	a = SCALE_DOWN_F(radius_x);
	b = SCALE_DOWN_F(radius_y);
	cphi = cos(angle);
	sphi = sin(angle);
	cphisqr = cphi*cphi;
	sphisqr = sphi*sphi;
	asqr = a*a;
	bsqr = b*b;

	c1 = (cphisqr/asqr)+(sphisqr/bsqr);
	c2 = ((cphi*sphi/asqr)-(cphi*sphi/bsqr))/c1;
	c3 = (bsqr*cphisqr) + (asqr*sphisqr);
	ymax = round(sqrt(c3));
	c4 = (a*b)/(double)c3;
	c5 = 0;
	v1 = c4*c4;
	c6 = 2*v1;
	c3 = c3*v1-v1;

	elltotpts = 0;

	for(i=0; i <= 3; i++)
		nump[i]=0;

	i=0; j=0;

	/* add first points */
	if(ymax % 2)
	{
		d = sqrt(c3);
		newpoint( (double)(xcen-d),ycen, i, j);
		newpoint( (double)(xcen+d),ycen, i, j);
		c5 = c2;
		yy=1;
	}

	while(c3 >= 0)
	{
		d = sqrt(c3);
		xleft = c5-d;
		xright = c5+d;
		newpoint( (double)(xcen+xleft),(double)(ycen+yy), i, j);
		newpoint( (double)(xcen+xright),(double)(ycen+yy), i, j);
		newpoint( (double)(xcen-xright),(double)(ycen-yy), i, j);
		newpoint( (double)(xcen-xleft),(double)(ycen-yy), i, j);
		c5+=c2;
		v1+=c6;
		c3-=v1;
		yy=yy+1;
	}

	if(elltotpts >= MAXPOINTNUM/4)
	{
		/* Max number of points exceeded, plot ellipse using quick method */
		quick_angle_ellipse(center_x, center_y, radius_x, radius_y, angle, 20, PolyPoints);
	}
	else
	{
		dir=0;
		/* now go down the 1st column, up the 2nd, down the 4th
		  and up the 3rd to get the points in the correct order */
		for(k=0; k <= 3; k++) {
			if(dir == 0)
				for(m=0; m < nump[k]; m++) {
					add_point_to_list( ellxpoints[m][order[k]], ellypoints[m][order[k]], PolyPoints);
				}
			else
				for(m=nump[k]-1; m >= 0; m--) {
					add_point_to_list( ellxpoints[m][order[k]], ellypoints[m][order[k]], PolyPoints);
				}

			dir = 1-dir;
		} /* next k */

		/* add another point to join with first */
		add_point_to_list( ellxpoints[0][order[0]], ellypoints[0][order[0]], PolyPoints);
	}
}

void quick_angle_ellipse( int center_x, int center_y, int radius_x, int radius_y, float angle, int precision, PointList *PolyPoints)
{
	double	xcen, ycen, a, b, z, X, Y, Xfirst=0.0, Yfirst=0.0;
	double	cphi, sphi, calpha, salpha, alpha;
    double  circ;
    int     npts;
    BOOL    first = TRUE;

	if(radius_x == 0 || radius_y == 0)
		return;

	xcen=SCALE_DOWN_X_F(center_x);
	ycen=SCALE_DOWN_Y_F(center_y);

	a = SCALE_DOWN_F(radius_x);
	b = SCALE_DOWN_F(radius_y);
	cphi = cos(-angle);
	sphi = sin(-angle);
    
    z = (a-b)/(a+b);
    circ = M_PI * (a+b)*(1.0 + 3.0*z*z/(10.0 + sqrt(4.0-3.0*z*z)));
    npts = max2(8,round(circ / (double)precision));
    
    for(alpha=0.0; alpha < M_2PI; alpha += M_2PI/npts)
    {
        calpha = cos(alpha);
        salpha = sin(alpha);
        X = xcen + a * calpha * cphi - b * salpha * sphi;
        Y = ycen + a * calpha * sphi + b * salpha * cphi;
        if (first)
        {
            Xfirst = X;
            Yfirst = Y;
            first = FALSE;
        }
        add_point_to_list( round(X), round(Y), PolyPoints);
    }
    add_point_to_list( round(Xfirst), round(Yfirst), PolyPoints);
}

/*********************** LINE ***************************/

void draw_line(F_line *line, int op)
{
	if(line->type == T_PICTURE)
	{
		pw_picture( line);
	}
	else
    {
		if(line->type == T_ARCBOX && round(line->radius*display_zoomscale) >= 3)
		{
			pw_arcbox( line);
		}
		else
        {
			pw_polygon( line, op);
            draw_arrows((F_line *)line, O_POLYLINE, op, 0);
        }
    }
}


/*********************** TEXT ***************************/

void draw_text( F_text *text, int op)
{
    int x,y;
    double cost, sint;
    
    if (SetCanvasFont(canvas_win, text) != 0)
        return;
    
    switch (text->type)
    {
        case T_CENTER_JUSTIFIED:
            cost = cos(text->angle);
            sint = sin(text->angle);
            x = round(text->base_x-cost*text->length/2);
            y = round(text->base_y+sint*text->length/2);
            break;
        case T_RIGHT_JUSTIFIED :
            cost = cos(text->angle);
            sint = sin(text->angle);
            x = round(text->base_x-cost*text->length);
            y = round(text->base_y+sint*text->length);
            break;
        default :
            x = text->base_x;
            y = text->base_y;
            break;
    }
	pw_text(canvas_win, x, y, op, text->depth, canvas_font,
             text->angle, text->cstring, text->color, COLOR_NONE);
}

/*********************** COMPOUND ***************************/

void
draw_compoundelements( F_compound *c, int op)
{
	F_line		*l;
	F_spline	*s;
	F_ellipse	*e;
	F_text		*t;
	F_arc		*a;
	F_compound	*c1;

	for(l = c->lines; l != NULL; l = l->next) {
		draw_line(l, op);
	}
	for(s = c->splines; s != NULL; s = s->next)	{
		draw_spline(s, op);
	}
	for(a = c->arcs; a != NULL; a = a->next) {
		draw_arc(a, op);
	}
	for(e = c->ellipses; e != NULL; e = e->next) {
		draw_ellipse(e, op);
	}
	for(t = c->texts; t != NULL; t = t->next) {
		draw_text( t, op);
	}
	for(c1 = c->compounds; c1 != NULL; c1 = c1->next) {
		draw_compoundelements( c1, op);
	}
}

/*************************** ARROWS *****************************

 compute_arcarrow_angle - Computes a point on a line which is a chord
	to the arc specified by center (x1,y1) and endpoint (x2,y2),
	where the chord intersects the arc arrow->ht from the endpoint.

 May give strange values if the arrow.ht is larger than about 1/4 of
 the circumference of a circle on which the arc lies.

****************************************************************/

void compute_arcarrow_angle(float x1, float y1, int x2, int y2, int direction, F_arrow *arrow, int *x, int *y)
{
    double	r, alpha, beta, dy, dx;
    double	lpt,h;

    dy=y2-y1;
    dx=x2-x1;
    r=sqrt(dx*dx+dy*dy);

    h = (double) arrow->ht*ZOOM_FACTOR;
    /* lpt is the amount the arrowhead extends beyond the end of the line */
    lpt = arrow->thickness/2.0/(arrow->wd/h/2.0);
    /* add this to the length */
    h += lpt;

    /* radius too small for this method, use normal method */
    if (h > 2.0*r) {
	compute_normal(x1,y1,x2,y2,direction,x,y);
	return;
    }

    beta=atan2(dy,dx);
    if (direction) {
	alpha=2*asin(h/2.0/r);
    } else {
	alpha=-2*asin(h/2.0/r);
    }

    *x=round(x1+r*cos(beta+alpha));
    *y=round(y1+r*sin(beta+alpha));
}
#ifndef AMIFIG
/* temporary error handler - see call to XSetRegion in clip_arrows below */

int tempXErrorHandler (Display *display, XErrorEvent *event)
{
	return 0;
}
#endif

/****************************************************************

 clip_arrows - calculate a clipping region which is the current 
	clipping area minus the polygons at the arrowheads.

 This will prevent the object (line, spline etc.) from protruding
 on either side of the arrowhead Also calculate the arrowheads
 themselves and put the outline polygons in farpts[nfpts] for forward
 arrow and barpts[nbpts] for backward arrow, and the fill areas in
 farfillpts[nffillpts] for forward arrow and barfillpts[nbfillpts]
 for backward arrow.

 The points[] array must already have the points for the object
 being drawn (spline, line etc), and npoints, the number of points.

 "skip" points are skipped from each end of the points[] array (for splines)
****************************************************************/

void clip_arrows(F_line *obj, int objtype, int op, int skip)
{
#ifndef AMIFIG
    Region	    mainregion, newregion;
    Region	    region;
    XPoint	    xpts[50];
    int		    x, y;
    zXPoint	    clippts[50];
    int		    i, j, n, nclippts;
#else
    int		    x, y;
	zXPoint 	*points;
	int			npoints;
#endif

#ifndef AMIFIG
    if (obj->for_arrow || obj->back_arrow) {
	/* start with current clipping area - maybe we won't have to draw anything */
	xpts[0].x = clip_xmin;
	xpts[0].y = clip_ymin;
	xpts[1].x = clip_xmax;
	xpts[1].y = clip_ymin;
	xpts[2].x = clip_xmax;
	xpts[2].y = clip_ymax;
	xpts[3].x = clip_xmin;
	xpts[3].y = clip_ymax;
	mainregion = XPolygonRegion(xpts, 4, WindingRule);
    }
#else
	points=(zXPoint *)GlobalPoints.Points;
	npoints = GlobalPoints.npoints;
#endif
    if (skip > npoints-2)
	skip = 0;

    /* get points for any forward arrowhead */
    if (obj->for_arrow) {
		x = SCALE_UP_X(points[npoints-skip-2].x);
		y = SCALE_UP_Y(points[npoints-skip-2].y);
		if (objtype == O_ARC) {
			F_arc  *a = (F_arc *) obj;
			compute_arcarrow_angle(a->center.x, a->center.y, a->point[2].x,
					a->point[2].y, a->direction,
					a->for_arrow, &x, &y);
		}
#ifndef AMIFIG
		calc_arrow(x, y, points[npoints-1].x, points[npoints-1].y, obj->thickness,
			   obj->for_arrow, farpts, &nfpts, farfillpts, &nffillpts, clippts, &nclippts);
		if (nclippts) {
			/* set clipping in scaled space */
			for (i=0; i < nclippts; i++) {
				xpts[i].x = ZOOMX(clippts[i].x);
				xpts[i].y = ZOOMY(clippts[i].y);
			}
			n = i;
			/* draw the clipping area for debugging */
			if (appres.DEBUG) {
			  for (i=0; i<n; i++) {
				if (i==n-1)
				j=0;
				else
				j=i+1;
				pw_vector(canvas_win,xpts[i].x,xpts[i].y,xpts[j].x,xpts[j].y,op,1,
				PANEL_LINE,0.0,RED);
			  }
			}
			region = XPolygonRegion(xpts, n, WindingRule);
			newregion = XCreateRegion();
			XSubtractRegion(mainregion, region, newregion);
			XDestroyRegion(region);
			XDestroyRegion(mainregion);
			mainregion=newregion;
		}
#else
		calc_arrow(x, y, SCALE_UP_X(points[npoints-1].x), SCALE_UP_Y(points[npoints-1].y), obj->thickness,
			   obj->for_arrow, farpts, &nfpts, farfillpts, &nffillpts);
#endif
    }
	
    /* get points for any backward arrowhead */
    if (obj->back_arrow) {
		x = SCALE_UP_X(points[skip+1].x);
		y = SCALE_UP_Y(points[skip+1].y);
		if (objtype == O_ARC) {
			F_arc  *a = (F_arc *) obj;
			compute_arcarrow_angle(a->center.x, a->center.y, a->point[0].x,
					   a->point[0].y, a->direction ^ 1,
					   a->back_arrow, &x, &y);
		}
#ifndef AMIFIG
		calc_arrow(x, y, points[0].x, points[0].y, obj->thickness,
				obj->back_arrow, barpts, &nbpts, barfillpts, &nbfillpts, clippts, &nclippts);
		if (nclippts) {
			/* set clipping in scaled space */
			for (i=0; i < nclippts; i++) {
				xpts[i].x = ZOOMX(clippts[i].x);
				xpts[i].y = ZOOMY(clippts[i].y);
			}
			n = i;
			/* draw the clipping area for debugging */
			if (appres.DEBUG) {
			  int j;
			  for (i=0; i<n; i++) {
				if (i==n-1)
				j=0;
				else
				j=i+1;
				pw_vector(canvas_win,xpts[i].x,xpts[i].y,xpts[j].x,xpts[j].y,op,1,
				PANEL_LINE,0.0,RED);
			  }
			}
			region = XPolygonRegion(xpts, n, WindingRule);
			newregion = XCreateRegion();
			XSubtractRegion(mainregion, region, newregion);
			XDestroyRegion(region);
			XDestroyRegion(mainregion);
			mainregion=newregion;
		}
#else
		calc_arrow(x, y, SCALE_UP_X(points[0].x), SCALE_UP_Y(points[0].y), obj->thickness,
				obj->back_arrow, barpts, &nbpts, barfillpts, &nbfillpts);
#endif
    }
#ifndef AMIFIG
    /* now set the clipping region for the subsequent drawing of the object */
	if (obj->for_arrow || obj->back_arrow) {
		/* install a temporary error handler to ignore any BadMatch error
		   from the buggy R5 Xlib XSetRegion() */
		XSetErrorHandler (tempXErrorHandler);
		XSetRegion(tool_d, gccache[op], mainregion);
		/* restore original error handler */
		if (!appres.DEBUG)
			XSetErrorHandler(X_error_handler);
		XDestroyRegion(mainregion);
    }
#endif
}

/****************************************************************

 calc_arrow - calculate arrowhead points heading from (x1, y1) to (x2, y2)

		        |\
		        |  \
		        |    \
(x1,y1) +---------------|      \+ (x2, y2)
		        |      /
		        |    /
		        |  /
		        |/ 

 Fills points[] array with npoints arrowhead *outline* coordinates and
 fillpoints[] array with nfillpoints points for the part to be filled *IF*
 it is a special arrowhead that has a different fill area than the outline.

 Otherwise, the points[] array is also used to fill the arrowhead in draw_arrow()
 The linethick param is the thickness of the *main line/spline/arc*,
 not the arrowhead.

 The clippts[] array is filled with the clip area so that the line won't
 protrude through the arrowhead.

****************************************************************/

#define ROTX(x,y)  (x)*cosa + (y)*sina + xa
#define ROTY(x,y) -(x)*sina + (y)*cosa + ya

#define ROTX2(x,y)  (x)*cosa + (y)*sina + x2
#define ROTY2(x,y) -(x)*sina + (y)*cosa + y2

#define ROTXC(x,y)  (x)*cosa + (y)*sina + fix_x
#define ROTYC(x,y) -(x)*sina + (y)*cosa + fix_y

#ifndef AMIFIG
void calc_arrow(int x1, int y1, int x2, int y2, int linethick, F_arrow *arrow, zXPoint *points, int *npoints, zXPoint *fillpoints, int *nfillpoints, zXPoint *clippts, int *nclippts)
#else
void calc_arrow(int x1, int y1, int x2, int y2, int linethick, F_arrow *arrow, zXPoint *points, int *npoints, zXPoint *fillpoints, int *nfillpoints)
#endif
{
    double	    x, y, xb, yb, dx, dy, l, sina, cosa;
    double	    mx, my;
    double	    ddx, ddy, lpt, tipmv;
    double	    alpha;
    double	    miny, maxy;
    int		    xa, ya, xs, ys;
    double	    wd  = (double) arrow->wd*ZOOM_FACTOR;
    double	    len = (double) arrow->ht*ZOOM_FACTOR;
    double	    th  = arrow->thickness*ZOOM_FACTOR;
    double	    radius;
    double	    angle, init_angle, rads;
    double	    fix_x, fix_y;
    int		    type, style, indx;
    int		    i, np;
#ifndef AMIFIG
    int		    offset, halfthick;
    /* to enlarge the clip area in case the line is thick */
    halfthick = linethick * ZOOM_FACTOR/2 + 1;
#else
    int		    offset;
#endif
    /* types = 0...10 */
    type = arrow->type;
    /* style = 0 (unfilled) or 1 (filled) */
    style = arrow->style;
    /* index into shape array */
    indx = 2*type + style;

    *npoints = *nfillpoints = 0;
#ifndef AMIFIG
    *nclippts = 0;
#endif
    dx = x2 - x1;
    dy = y1 - y2;
    /* return now if arrowhead width or length is 0 or line has zero length */
    if (wd == 0 || len == 0 || (dx==0 && dy==0))
	return;

    /* lpt is the amount the arrowhead extends beyond the end of the
       line because of the sharp point (miter join) */
    tipmv = arrow_shapes[indx].tipmv;
    lpt = 0.0;
    if (tipmv > 0.0)
	lpt = th / (2.0 * sin(atan(wd / (tipmv * len))));
    else if (tipmv == 0.0)
	lpt = th / 2.0;	/* types which have blunt end */
			/* (Don't adjust those with tipmv < 0) */

    /* alpha is the angle the line is relative to horizontal */
    alpha = atan2(dy,-dx);

    /* ddx, ddy is amount to move end of line back so that arrowhead point
       ends where line used to */
    ddx = lpt * cos(alpha);
    ddy = lpt * sin(alpha);

    /* move endpoint of line back */
    mx = x2 + ddx;
    my = y2 + ddy;

    l = sqrt(dx * dx + dy * dy);
    sina = dy / l;
    cosa = dx / l;
    xb = mx * cosa - my * sina;
    yb = mx * sina + my * cosa;

    /* (xa,ya) is the rotated endpoint (used in ROTX and ROTY macros) */
    xa =  xb * cosa + yb * sina + 0.5;
    ya = -xb * sina + yb * cosa + 0.5;

    miny =  100000.0;
    maxy = -100000.0;

    if (type == 5 || type == 6) {
	/*
	 * CIRCLE and HALF-CIRCLE arrowheads
	 *
	 * We approximate circles with (40+zoom)/4 points
	 */

	/* use original dx, dy to get starting angle */
	init_angle = compute_angle(dx, dy);

	/* (xs,ys) is a point the length of the arrowhead BACK from
	   the end of the shaft */
	/* for the half circle, use 0.0 */
	xs =  (xb-(type==5? len: 0.0)) * cosa + yb * sina + 0.5;
	ys = -(xb-(type==5? len: 0.0)) * sina + yb * cosa + 0.5;

	/* calc new (dx, dy) from moved endpoint to (xs, ys) */
	dx = mx - xs;
	dy = my - ys;
	/* radius */
	radius = len/2.0;
	fix_x = xs + (dx / (double) 2.0);
	fix_y = ys + (dy / (double) 2.0);
	/* choose number of points for circle - 40+zoom/4 points */
	np = round(display_zoomscale/4.0) + 40;

	if (type == 5) {
	    /* full circle */
	    init_angle = 5.0*M_PI_2 - init_angle;
	    rads = M_2PI;
	} else {
	    /* half circle */
	    init_angle = 3.0*M_PI_2 - init_angle;
	    rads = M_PI;
	}

	/* draw the half or full circle */
	for (i = 0; i < np; i++) {
	    angle = init_angle - (rads * (double) i / (double) (np-1));
	    x = fix_x + round(radius * cos(angle));
	    points[*npoints].x = x;
	    y = fix_y + round(radius * sin(angle));
	    points[*npoints].y = y;
	    (*npoints)++;
	}
#ifndef AMIFIG
	/* set clipping to a box at least as large as the line thickness
	   or diameter of the circle, whichever is larger */
	/* 4 points in clip box */
	miny = min2(-halfthick, -radius-th/2.0);
	maxy = max2( halfthick,  radius+th/2.0);

	i=0;
	/* start at new endpoint of line */
	clippts[i].x = ROTXC(0,            -radius-th/2.0);
	clippts[i].y = ROTYC(0,            -radius-th/2.0);
	i++;
	clippts[i].x = ROTXC(0,             miny);
	clippts[i].y = ROTYC(0,             miny);
	i++;
	/* add halfthick in case the line cap style is Round or Projecting */
	clippts[i].x = ROTXC(radius+th/2.0+halfthick, miny);
	clippts[i].y = ROTYC(radius+th/2.0+halfthick, miny);
	i++;
	/* add halfthick in case the line cap style is Round or Projecting */
	clippts[i].x = ROTXC(radius+th/2.0+halfthick, maxy);
	clippts[i].y = ROTYC(radius+th/2.0+halfthick, maxy);
	i++;
	clippts[i].x = ROTXC(0,             maxy);
	clippts[i].y = ROTYC(0,             maxy);
	i++;
	*nclippts = i;
#endif
    } else {
	/*
	 * ALL OTHER HEADS
	 */

	*npoints = arrow_shapes[indx].numpts;
	/* we'll shift the half arrowheads down by the difference of the main line thickness 
	   and the arrowhead thickness to make it flush with the main line */
	if (arrow_shapes[indx].half)
	    offset = ZOOM_FACTOR * (linethick - arrow->thickness)/2;
	else
	    offset = 0;

	/* fill the points array with the outline */
	for (i=0; i<*npoints; i++) {
	    x = arrow_shapes[indx].points[i].x * len;
	    y = arrow_shapes[indx].points[i].y * wd - offset;
	    miny = min2(y, miny);
	    maxy = max2(y, maxy);
	    points[i].x = ROTX(x,y);
	    points[i].y = ROTY(x,y);
	}

	/* and the fill points array if there are fill points different from the outline */
	*nfillpoints = arrow_shapes[indx].numfillpts;
	for (i=0; i<*nfillpoints; i++) {
	    x = arrow_shapes[indx].fillpoints[i].x * len;
	    y = arrow_shapes[indx].fillpoints[i].y * wd - offset;
	    miny = min2(y, miny);
	    maxy = max2(y, maxy);
	    fillpoints[i].x = ROTX(x,y);
	    fillpoints[i].y = ROTY(x,y);
	}
#ifndef AMIFIG
	/* to include thick lines in clip area */
	miny = min2(miny, -halfthick);
	maxy = max2(maxy, halfthick);

	/* set clipping to the first three points of the arrowhead and
	   the (enlarged) box surrounding it */
	*nclippts = 0;
	if (arrow_shapes[indx].clip) {
		for (i=0; i < 3; i++) {
		    x = arrow_shapes[indx].points[i].x * len;
		    y = arrow_shapes[indx].points[i].y * wd - offset;
		    clippts[i].x = ROTX(x,y);
		    clippts[i].y = ROTY(x,y);
		}

		/* locate the tip of the head */
		tip = arrow_shapes[indx].tipno;

		/* now make the box around it at least as large as the line thickness */
		/* start with last x, lower y */

		clippts[i].x = ROTX(x,miny);
		clippts[i].y = ROTY(x,miny);
		i++;
		/* x tip, same y (note different offset in ROTX/Y2 rotation) */
		/* add halfthick in case the line cap style is Round or Projecting */
		clippts[i].x = ROTX2(arrow_shapes[indx].points[tip].x*len+halfthick + ZOOM_FACTOR, miny);
		clippts[i].y = ROTY2(arrow_shapes[indx].points[tip].x*len+halfthick + ZOOM_FACTOR, miny);
		i++;
		/* x tip, upper y (note different offset in ROTX/Y2 rotation) */
		/* add halfthick in case the line cap style is Round or Projecting */
		clippts[i].x = ROTX2(arrow_shapes[indx].points[tip].x*len+halfthick + ZOOM_FACTOR, maxy);
		clippts[i].y = ROTY2(arrow_shapes[indx].points[tip].x*len+halfthick + ZOOM_FACTOR, maxy);
		i++;
		/* first x of arrowhead, upper y */
		clippts[i].x = ROTX(arrow_shapes[indx].points[0].x*len, maxy);
		clippts[i].y = ROTY(arrow_shapes[indx].points[0].x*len, maxy);
		i++;
	}
	/* set the number of points in the clip or bounds */
	*nclippts = i;
#endif
    }
}

/* draw the arrowhead resulting from the call to calc_arrow() */
/* points[npoints] contains the outline and points2[npoints2] the points to be filled */
void draw_arrow(F_line *obj, F_arrow *arrow, zXPoint *points, int npoints, zXPoint *points2, int npoints2, int op)
{
    int		    fill = NUMSHADEPATS-1; /* Plain fill */
	int			fill_color = obj->pen_color;

    if (obj->thickness == 0)
		return;
    if (arrow->type == 0 || arrow->type >= 13)
		fill = UNFILLED;			/* old arrow head or new unfilled types */
    else if (arrow->style == 0)
		fill_color = WHITE;
	else
		fill_color = obj->pen_color;
	
    if (npoints2==0)
		/* no special fill, use outline points to fill too */
		pw_lines(canvas_win, points, npoints, op, obj->depth, round(arrow->thickness),
			SOLID_LINE, 0.0, JOIN_MITER, CAP_BUTT,
			fill, obj->pen_color, fill_color);
    else {
		/* fill whole (outline) with white to obscure the line inside */
		pw_lines(canvas_win, points, npoints, op, obj->depth, 0,
			SOLID_LINE, 0.0, JOIN_MITER, CAP_BUTT,
			NUMSHADEPATS-1, obj->pen_color, WHITE);
		/* draw outline */
		pw_lines(canvas_win, points, npoints, op, obj->depth, round(arrow->thickness),
			SOLID_LINE, 0.0, JOIN_MITER, CAP_BUTT,
			UNFILLED, obj->pen_color, obj->pen_color);
		/* fill special part with pen color */
		pw_lines(canvas_win, points2, npoints2, op, obj->depth, 0,
			SOLID_LINE, 0.0, JOIN_MITER, CAP_BUTT,
			NUMSHADEPATS-1, obj->pen_color, obj->pen_color);
    }
}

void draw_arrows(F_line *obj, int objtype, int op, int skip)
{
	clip_arrows(obj,objtype,op,skip);
	if (obj->for_arrow) {
		draw_arrow(obj, obj->for_arrow, farpts, nfpts, farfillpts, nffillpts, op);
	}
	if (obj->back_arrow) {
		draw_arrow(obj, obj->back_arrow, barpts, nbpts, barfillpts, nbfillpts, op);
	}
}

#ifdef AMIFIG
static void too_many_points(void)
{
	put_msg((char *)_(msg_TooManyPts));
}
#endif
/********************* CURVES FOR ARCS AND ELLIPSES ***************

 This routine plot two dimensional curve defined by a second degree
 polynomial of the form : 2    2 f(x, y) = ax + by + g = 0

 (x0,y0) is the starting point as well as ending point of the curve. The curve
 will translate with the offset xoff and yoff.

 This algorithm is derived from the eight point algorithm in : "An Improved
 Algorithm for the generation of Nonparametric Curves" by Bernard W.
 Jordan, William J. Lennon and Barry D. Holm, IEEE Transaction on Computers
 Vol C-22, No. 12 December 1973.

 This routine is only called for ellipses when the angle is 0 and the line type
 is not solid.  For angles of 0 with solid lines, pw_curve() is called.
 For all other angles angle_ellipse() is called.

 Will fill the curve if fill_style is != UNFILLED (-1)
 Call with draw_points = True to display the points using draw_point_array
	Otherwise global points array is filled with npoints values but
	not displayed.
 Call with draw_center = True and center_x, center_y set to draw endpoints
	to center point (xoff,yoff) (arc type 2, i.e. pie wedge)

****************************************************************/

void
#ifndef AMIFIG
curve(Window window, int depth, int xstart, int ystart, int xend, int yend, 
	Boolean draw_points, Boolean draw_center, int direction,
	int a, int b, int xoff, int yoff, int op, int thick,
	int style, float style_val, int fill_style, 
	Color pen_color, Color fill_color, int cap_style)
#else
curve( int xstart, int ystart, int xend, int yend, BOOL draw_center,
           int direction, int a, int b, int xoff, int yoff, int style, PointList *PolyPoints)
#endif
{
    register int    x, y;
    register double deltax, deltay, dfx, dfy;
    double	    dfxx, dfyy;
    double	    falpha, fx, fy, fxy, absfx, absfy, absfxy;
    int		    margin, test_succeed, inc, dec;
    float	    zoom;
#ifndef AMIFIG
    /* if this depth is inactive, draw the curve in gray */
    if (depth < MAX_DEPTH+1 && !active_layer(depth)) {
	pen_color = MED_GRAY;
	fill_color = LT_GRAY;
    }

    zoom = 1.0;
    /* if drawing on canvas (not in indicator button) adjust values by zoomscale */
    if (style != PANEL_LINE) {
		zoom = zoomscale;
		xstart = round(xstart * zoom);
		ystart = round(ystart * zoom);
		xend = round(xend * zoom);
		yend = round(yend * zoom);
		a = round(a * zoom);
		b = round(b * zoom);
		xoff = round(xoff * zoom);
		yoff = round(yoff * zoom);
    }

    init_point_array();
#else
	/* Drawing on canvas (not in indicator button) adjust values by zoomscale */
    zoom = zoomscale*display_zoomscale;
    xstart = round(xstart * zoom);
    ystart = round(ystart * zoom);
    xend = round(xend * zoom);
    yend = round(yend * zoom);
    a = round(a * zoom);
    b = round(b * zoom);
    xoff = round(xoff * zoom)-xshift;
    yoff = round(yoff * zoom)-yshift;
#endif

    /* this must be AFTER init_point_array() */
    if (a == 0 || b == 0)
	return;

    x = xstart;
    y = ystart;
    dfx = 2 * (double) a * (double) xstart;
    dfy = 2 * (double) b * (double) ystart;
    dfxx = 2 * (double) a;
    dfyy = 2 * (double) b;

    falpha = 0;
    if (direction) {
		inc = 1;
		dec = -1;
    } else {
		inc = -1;
		dec = 1;
    }
    if (xstart == xend && ystart == yend) {
		test_succeed = margin = 2;
    } else {
		test_succeed = margin = 3;
    }

#ifndef AMIFIG
    if (!add_point(round((xoff + x)/zoom), round((yoff - y)/zoom))) {
#else
	if (!add_point_to_list( xoff+x, yoff-y, PolyPoints)) {
#endif
		return;
    } else {
		while (test_succeed) {
			deltax = (dfy < 0) ? inc : dec;
			deltay = (dfx < 0) ? dec : inc;
			fx = falpha + dfx * deltax + a;
			fy = falpha + dfy * deltay + b;
			fxy = fx + fy - falpha;
			absfx = fabs(fx);
			absfy = fabs(fy);
			absfxy = fabs(fxy);

			if ((absfxy <= absfx) && (absfxy <= absfy))
				falpha = fxy;
			else if (absfy <= absfx) {
				deltax = 0;
				falpha = fy;
			} else {
				deltay = 0;
				falpha = fx;
			}
			x += deltax;
			y += deltay;
			dfx += (dfxx * deltax);
			dfy += (dfyy * deltay);
#ifndef AMIFIG
			if (!add_point(round((xoff + x)/zoom), round((yoff - y)/zoom))) {
#else
			if (!add_point_to_list(xoff + x, yoff - y, PolyPoints)) {
#endif
				break;
			}

			if ((abs(x - xend) < margin && abs(y - yend) < margin) &&
				(x != xend || y != yend))
				test_succeed--;
		}
    }

    if (xstart == xend && ystart == yend)	/* end points should touch */
#ifndef AMIFIG
	if (!add_point(round((xoff + xstart)/zoom),
			round((yoff - ystart)/zoom)))
#else
	if (!add_point_to_list(xoff + xstart, yoff - ystart, PolyPoints))
#endif
		too_many_points();

    /* if this is arc type 2 then connect end points to center */
    if (draw_center) {
#ifndef AMIFIG
		if (!add_point(round(xoff/zoom),round(yoff/zoom)))
#else
		if (!add_point_to_list(xoff, yoff, PolyPoints))
#endif			
			too_many_points();
#ifndef AMIFIG
		if (!add_point(round((xoff + xstart)/zoom),round((yoff - ystart)/zoom)))
#else
		if (!add_point_to_list(xoff + xstart, yoff - ystart, PolyPoints))
#endif			
			too_many_points();

    }
#ifndef AMIFIG
    if (draw_points) {
	draw_point_array(window, op, depth, thick, style, style_val, JOIN_BEVEL,
			cap_style, fill_style, pen_color, fill_color);
    }
#endif
}

/*********************** SPLINE ***************************/

/**********************************/
/* include common spline routines */
/**********************************/

void
draw_spline( F_spline *spline, int op)
{
	BOOL         success;
	
	success = pw_spline( spline, op);

    if (success) draw_arrows((F_line *)spline, O_POLYLINE, op, 4);
}


