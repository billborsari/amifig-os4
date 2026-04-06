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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_drawprim.c $
 *       $Revision: 352 $
 *       $Date: 2019-10-12 16:13:26 +0000 (Sat, 12 Oct 2019) $
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
#include "w_drawprim.h"
#include "a_color.h"
#include "a_patterns.h"
#include "u_draw.h"
#include "u_draw_spline.h"
#include "w_canvas.h"
#include "w_msgpanel.h"

#include "GradientFill.h"

/* clipping using Sutherland-Hodgeman algorithm */
#define CP_LEFT 	0
#define CP_RIGHT 	1
#define CP_TOP 		2
#define CP_BOTTOM 	3
BOOL   cp_inside( zXPoint p, int side, WORD Width, WORD Height);
zXPoint cp_intersect( zXPoint p, zXPoint q, int side, WORD Width, WORD Height );
void   cp_clipplane( int *nbpoints, zXPoint *in, int side, WORD Width, WORD Height );

void   calc_arcbox( int xmin, int ymin, int xmax, int ymax, int radius, int thickness, int style, float style_val);
void   lengthen_dash(int *x1, int *y1, int *x2, int *y2, double sina, double cosa, int add);
float  intersection_point(int l1x, int l1y, int l2x, int l2y, int m1x, int m1y, int m2x, int m2y, int *punktx, int *punkty, float *s, BOOL *err);
void   calc_outline(WORD x1, WORD y1, WORD x2, WORD y2, WORD points[5][2], int thickness);

/* Global variables for polygon drawing => should be removed! */
PointList GlobalPoints = {NULL, 0, 0};


/********************** EXTERN FUNCTION **********************************/

/* SetUp Temporary Raster for AreaDraw function */
int SetTempRaster( int width, int height, BOOL force)
{
    int requested_size;
    int current_size;
    int err_code = 1;

    width = width>MAXRASTSIZE?MAXRASTSIZE:width;
    height = height>MAXRASTSIZE?MAXRASTSIZE:height;
    
    requested_size  = width * height;
    current_size    = rastwidth * rastheight;
    
	/* requested size bigger than current one */
	if(((force==TRUE) && (requested_size != current_size)) || (requested_size > current_size))
	{
        if(tempraster)
        {
            FreeRaster(tempraster, rastwidth, rastheight);
            tempraster=NULL;
        }

        // New Raster
        if((tempraster=AllocRaster(width, height)) == NULL)
        {
            /* Not Enough memory, try with former size */
            err_code = -1;
            if((tempraster = AllocRaster(rastwidth, rastheight)) == NULL)
            {
                put_msg((char *)_(msg_OutOfMem));
                rastwidth = 0;
                rastheight = 0;
                initTmpRasContexts(NULL);
                
                err_code = -2;
            }
            else
            {
                InitTmpRas(&tmpras, tempraster, RASSIZE(rastwidth, rastheight));
                initTmpRasContexts(&tmpras);               
            }
        }
        else
        {
            err_code = 0;
            rastwidth   = width; 
            rastheight  = height;
            InitTmpRas(&tmpras, tempraster, RASSIZE(rastwidth, rastheight));
            initTmpRasContexts(&tmpras);
        }
	}

	return(err_code);
}

void elastic_ellipse( int center_x, int center_y, int radius_x, int radius_y, float angle, int op) // extern
{
	int rx, ry;

	if(angle == 0)
	{
		rx=SCALE_DOWN(radius_x);
		ry=SCALE_DOWN(radius_y);

		rx=abs(rx);
		ry=abs(ry);

		if(rx != 0 && ry != 0)
		{
		switch(op)
		{
		case INV_PAINT :
		        SetRPAttrs(rastport, RPTAG_PenMode, TRUE, TAG_DONE);
		                SetDrMd(rastport,COMPLEMENT);
		                break;
		        case PAINT :
		                SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);
		                SetDrMd(rastport,JAM2);
		        break;
		        case ERASE :
		                SetDrMd(rastport,JAM1);
		                SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[WHITE].RGB, TAG_DONE);
		break;
		default : 
		break;
		}
            DrawEllipse(rastport, SCALE_DOWN_X(center_x), SCALE_DOWN_Y(center_y), rx, ry);
        }
	}
	else
	{
		rx=abs(radius_x);
		ry=abs(radius_y);

		if(rx != 0 && ry != 0)
		{
            init_pointlist(&GlobalPoints,0);
            quick_angle_ellipse( center_x, center_y, rx, ry, angle, 20, &GlobalPoints);
            pw_line_array(&GlobalPoints, TRUE, SOLID_LINE, 0.0f, JOIN_MITER, CAP_BUTT, 1, BLACK, op);
        }
	}

	SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[BLACK].RGB, TAG_DONE);
	SetDrMd(rastport,JAM1);
}

void pw_vector(struct Window *win, int x1, int y1, int x2, int y2, int op, int line_width, int line_style, float style_val, Color color)
{
	switch(op)
    {
        case INV_PAINT :
            SetDrMd(win->RPort,COMPLEMENT);
            SetRPAttrs(win->RPort, RPTAG_PenMode, TRUE, TAG_DONE);
            break;
        case PAINT :
            SetDrMd(win->RPort,JAM2);
            SetRPAttrs(win->RPort, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[color].RGB, TAG_DONE);
            break;
        case ERASE :
            SetDrMd(win->RPort,JAM1);
            SetRPAttrs(win->RPort, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[WHITE].RGB, TAG_DONE);
            break;
        default : 
            break;
    }
    
    switch (line_style)
    {
        case SOLID_LINE:
            SetDrPt(win->RPort,0xffff);
            break;
        case DASH_LINE:
            SetDrPt(win->RPort,0xff00);
            break;
        case DOTTED_LINE:
            SetDrPt(win->RPort,0xaaaa);
            break;
        case DASH_DOT_LINE:
            SetDrPt(win->RPort,0xfe02);
            break;
        case DASH_2_DOTS_LINE:
            SetDrPt(win->RPort,0xfc0a);
            break;
        case DASH_3_DOTS_LINE:
            SetDrPt(win->RPort,0xf82a);
            break;
        default:
            SetDrPt(win->RPort,0xffff);
            break;
    }
    
    if (line_style == PANEL_LINE)
    {
        Move(win->RPort, x1, y1);
        Draw(win->RPort, x2, y2);
    }
    else
    {
        Move(win->RPort, SCALE_DOWN_X(x1), SCALE_DOWN_Y(y1));
        Draw(win->RPort, SCALE_DOWN_X(x2), SCALE_DOWN_Y(y2));
    }
    
    SetDrPt(win->RPort,0xffff);
}

void pw_lines(struct Window *win, zXPoint *points, int npoints, int op, int depth, int thickness,
			int style, float style_val, int join_style, int cap_style,
			int fill_style, Color pen_color, Color fill_color)
{
	int i;
    PointList Points = {NULL, 0, 0};

    init_pointlist(&Points,npoints);

    for(i=0; i < npoints; i++)
        add_point_to_list( SCALE_DOWN_X(points[i].x), SCALE_DOWN_Y(points[i].y), &Points);

    if(fill_style != UNFILLED)         // fill_style
    {
        pw_area(&Points, pen_color, fill_color, fill_style);
    }
    
    pw_line_array(&Points, (fill_style != UNFILLED), style, style_val, join_style, cap_style, thickness, pen_color, op);

    free_pointlist(&Points);
}

void pw_arcbox( F_line *line) // extern
{
	F_point *point;
	int xmin, xmax, ymin, ymax;
	int scaledthick = round(line->thickness*display_zoomscale);
	float scaledstyleval = line->style_val*display_zoomscale;
    int scaledradius = round(line->radius*ZOOM_FACTOR);

	point=line->points;

	if(point != NULL)
    {
        xmin = xmax = point->x;
        ymin = ymax = point->y;
        while(point->next)   	/* find lower left (upper-left on screen)  */
        {						/* and upper right (lower right on screen) */
            point = point->next;

            if(point->x < xmin)
                xmin=point->x;
            else if(point->x > xmax)
                xmax=point->x;
                
            if(point->y < ymin)
                ymin=point->y;
            else if(point->y > ymax)
                ymax=point->y;
        }

        init_pointlist(&GlobalPoints,0);

        calc_arcbox( xmin, ymin, xmax, ymax, scaledradius, scaledthick, line->style, scaledstyleval);

        if(line->fill_style != UNFILLED)         // fill
        {
            pw_area(&GlobalPoints, line->pen_color, line->fill_color, line->fill_style);
        }

        pw_line_array(  &GlobalPoints, TRUE, 
                        line->style, line->style_val,
                        JOIN_BEVEL, line->cap_style, line->thickness, line->pen_color,PAINT);        
    }
}

void pw_polygon( F_line *line, int op) // extern
{
	F_point *point, *pointlist;
	
	pointlist=line->points;

	if(pointlist != NULL)
    {
        init_pointlist(&GlobalPoints,0);

        for(point=pointlist; point != NULL; point=point->next)
            add_point_to_list( SCALE_DOWN_X(point->x), SCALE_DOWN_Y(point->y), &GlobalPoints);
        
        if(line->fill_style != UNFILLED)         // fill
        {
            pw_area(&GlobalPoints, line->pen_color, line->fill_color, line->fill_style);
        }
      
        pw_line_array(  &GlobalPoints,
                        (line->type == T_POLYGON || line->type == T_BOX || line->type == T_ARCBOX), 
                        line->style, line->style_val,
                        line->join_style, line->cap_style, line->thickness, line->pen_color, op);
    }
}

BOOL pw_spline( F_spline *spline, int op) // extern
{
    BOOL  success = FALSE;
    float precision;
#define         HIGH_PRECISION    0.25
#define         LOW_PRECISION     0.5
#define         ZOOM_PRECISION    5.0

	precision = (float)( (display_zoomscale < ZOOM_PRECISION) ? LOW_PRECISION : HIGH_PRECISION );
    if (!appres.viewBestQuality) precision *=2;
    
	if (open_spline(spline))
    {
		success = compute_open_spline(spline, precision);
	}
    else
    {
		success = compute_closed_spline(spline, precision);
    }
    
    if ((success == TRUE) && (GlobalPoints.npoints > 0))
    {
        if(spline->fill_style != UNFILLED)         // fill
        {
            pw_area(&GlobalPoints, spline->pen_color, spline->fill_color, spline->fill_style);
        }

        pw_line_array(  &GlobalPoints, closed_spline(spline), spline->style, 
                        spline->style_val, JOIN_MITER, spline->cap_style,
                        spline->thickness, spline->pen_color, op);
        
    }
    return(success);
}

void pw_ellipse( F_ellipse *e, int op) // extern
{
	int a, b;

	a=e->radiuses.x;
	b=e->radiuses.y;

    init_pointlist(&GlobalPoints,0);
    if (appres.viewBestQuality)
        angle_ellipse( e->center.x, e->center.y, a, b, e->angle, e->style, &GlobalPoints);
    else
        quick_angle_ellipse( e->center.x, e->center.y, a, b, e->angle, 10, &GlobalPoints);

	if(e->fill_style != UNFILLED)         // fill
	{
		pw_area(&GlobalPoints, e->pen_color, e->fill_color, e->fill_style);
	}

    pw_line_array(  &GlobalPoints, TRUE, e->style, 
                    e->style_val, JOIN_MITER, CAP_ROUND,
                    e->thickness, e->pen_color,op);
}

void pw_arc( F_arc *a, int op) // extern
{
	double rx, ry;
	int radius;

    rx = a->point[0].x - a->center.x;
    ry = a->center.y - a->point[0].y;
    radius = round(sqrt(rx * rx + ry * ry));

    init_pointlist(&GlobalPoints,0);
    curve( round(rx), round(ry), round(a->point[2].x - a->center.x), round(a->center.y - a->point[2].y),
          (a->type == T_PIE_WEDGE_ARC), a->direction, radius, radius, round(a->center.x), round(a->center.y), a->style, &GlobalPoints);

	if(a->fill_style != UNFILLED)         // fill
	{
		pw_area(&GlobalPoints, a->pen_color, a->fill_color, a->fill_style);
	}
    
    pw_line_array(  &GlobalPoints, (a->type == T_PIE_WEDGE_ARC), a->style, 
                    a->style_val, JOIN_MITER, a->cap_style,
                    a->thickness, a->pen_color,op);
}

/******************************* INTERN FUNCTIONS ***************************/

/* point list management functions */
void init_pointlist(PointList *PolyPoints, int NPoints)
{
    if (PolyPoints->Points == NULL)
    {
        if (NPoints == 0) NPoints = MAXPOINTNUM;
        PolyPoints->Points = (zXPoint*)AllocVec(sizeof(zXPoint) * NPoints,MEMF_ANY); 
        PolyPoints->MaxPoints = NPoints;
    }
	PolyPoints->npoints=0;
}

void free_pointlist(PointList* PolyPoints)
{
	if (PolyPoints->Points != NULL)
	{
		FreeVec(PolyPoints->Points);
	}
	PolyPoints->npoints=0;
}

BOOL add_point_to_list( int x, int y, PointList *PolyPoints)
{
	if(PolyPoints->npoints >= PolyPoints->MaxPoints) return(FALSE);

	if ((PolyPoints->npoints == 0) || (x != PolyPoints->Points[PolyPoints->npoints-1].x || y != PolyPoints->Points[PolyPoints->npoints-1].y))
	{
		PolyPoints->Points[PolyPoints->npoints].x = x;
		PolyPoints->Points[PolyPoints->npoints].y = y;
		PolyPoints->npoints++;
	}

	return(TRUE);
}

/* Define fill colors for final drawing */
int DefineFillColor( Color pen_color, Color fill_color, int fill_style)
{
    ULONG fg_color, bg_color;
    ULONG alpha;
	/*
		Special color fill return :
		-1 		=> none, standard fill function in use
		-2		=> User Color fill with alpha channel
		0 - 512	=> gradient fill in use
	*/
	int Special = -1;

	pen_color   = (pen_color == DEFAULT)? BLACK : pen_color;
	fill_color  = (fill_color == DEFAULT)? BLACK : fill_color;

	if(fill_style != UNFILLED)                              // fill
	{
		if (fill_color > (NUM_STD_COLS + MAX_USR_COLS - 1))
		{
			/* gradient fill */
            if (cur_DisplayGradients)
            {
                Special = fill_color - (NUM_STD_COLS + MAX_USR_COLS);
                if (GradientTable[Special] == NULL)
                {
                    /* if gradient isn't defined, fill with white */
                    Special = -1;
                    SetDrMd(rastport, JAM2);
                    SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[WHITE].RGB, TAG_DONE);
                }
            }
            else
            {
                fg_color = ColorPalette[WHITE].RGB;
                if (GradientTable[fill_color - (NUM_STD_COLS + MAX_USR_COLS)] != NULL)
                {
                    fg_color = GradientTable[fill_color - (NUM_STD_COLS + MAX_USR_COLS)]->Stop[0].color;
                }
                Special = -1;
                SetDrMd(rastport, JAM2);
                SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, fg_color, TAG_DONE);              
            }
		}
		else
		{
			SetDrMd(rastport, JAM2);		
			if(fill_style < (NUMSHADEPATS+NUMTINTPATS))                     // Plain color
			{
				if (cur_DisplayGradients)
                {
                    alpha = (ColorPalette[fill_color].RGB & 0xFF000000) >> 24;
                    if ((alpha > 0x00) && (alpha < 0xFF))
                    {
                        /* Alpha channel is defined */
                        Special = -2;
                    }
                }
				/* A-Pen is fill color */
				fg_color = getfillcolor(fill_color,fill_style);
			}
			else															// pattern
			{
				/* A-Pen is pen color */
				if(pen_color < (NUM_STD_COLS + MAX_USR_COLS))
					fg_color = ColorPalette[pen_color].RGB;
				else
					fg_color = ColorPalette[BLACK].RGB;
				/* B-Pen is fill color */
				bg_color = getfillcolor(fill_color,fill_style);
				
				if (appres.OnScreenAA)
					SetAfPt(rastport, patternPointers_AA[fill_style-(NUMSHADEPATS+NUMTINTPATS)], 4);
				else
					SetAfPt(rastport, patternPointers[fill_style-(NUMSHADEPATS+NUMTINTPATS)], 4);
					
				SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_BgColor, bg_color, TAG_DONE);
			}		
			SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, fg_color, TAG_DONE);
		}
	}

	return(Special);
}

/*Defines outline color for drawing */
void DefinePenColor(Color pen_color, int op)
{
    ULONG rgb;
    
    pen_color   = (pen_color == DEFAULT)? BLACK : pen_color;
    
	if(pen_color < (NUM_STD_COLS + MAX_USR_COLS))
		rgb = ColorPalette[pen_color].RGB;
	else
		rgb = ColorPalette[BLACK].RGB;
        
	switch(op)
    {
        case INV_PAINT :
            SetDrMd(rastport,COMPLEMENT);
            break;
        case PAINT :
            SetDrMd(rastport,JAM1);
            break;
        case ERASE :
            SetDrMd(rastport,JAM1);
            rgb = ColorPalette[WHITE].RGB;
            break;
        default : 
            break;
    }
    
    SetAfPt(rastport, NULL, 0);
	SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, rgb, TAG_DONE);
}

/* simple dot drawing */
void pw_dot(int x, int y, int thickness)
{
    ULONG SaveFgCol;
    WORD    Width, Height;
	static const UBYTE pat3[]= 
	{
		0x40, 0x00,
		0xE0, 0x00,
		0x40, 0x00
	};
	static const UBYTE pat4[]= 
	{	
		0x60, 0x00,
		0xF0, 0x00,
		0xF0, 0x00,
		0x60, 0x00
	};	

	Width  = GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
    Height = GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);
    
	if ((x >= 0) && (x < Width) && (y >= 0) && (y < Height))
	{
		GetRPAttrs(rastport, RPTAG_FgColor, &SaveFgCol, TAG_DONE);
		if (thickness == 1)
		{
			WriteRGBPixel(rastport, x, y, SaveFgCol);
		}
		else if (thickness == 2)
		{
			BltPattern(rastport, NULL, x, y, x+1, y+1, 0);
		}
		else if (thickness == 3)
		{
			BltPattern(rastport, (PLANEPTR)pat3, x-1, y-1, x+1, y+1, 2);
		}
		else if (thickness == 4)
		{
			BltPattern(rastport, (PLANEPTR)pat4, x-1, y-1, x+2, y+2, 2);
		}
		else
		{
			thickness = (thickness&1)? thickness/2 : thickness/2-1;
			AreaCircle(rastport, x, y, thickness);
			AreaEnd(rastport);
		}
		SetRPAttrs(rastport, RPTAG_PenMode, FALSE,  RPTAG_FgColor, SaveFgCol, TAG_DONE);
	}
}

/* Area drawing */
void pw_area(PointList* PolyPoints, Color pen_color, Color fill_color, int fill_style)
{
	int     i, Specialfill;
    ULONG   SaveFgCol;
    WORD    Width, Height;
    WORD    xmin = 0,ymin = 0,xmax = 100,ymax = 100;
    
	Specialfill = DefineFillColor( pen_color, fill_color, fill_style);
	
    GetRPAttrs(rastport, RPTAG_FgColor, &SaveFgCol, TAG_DONE);
    
    if (Specialfill > -1)
    {
		if (GradientTable[Specialfill] != NULL)
		{
			/* Use self made polygon gradient fill function */
			if (GradientTable[Specialfill]->unit == 0) 
			{
				/* the following computation are only needed for gradient defined in bounding box units */
				xmin = PolyPoints->Points[0].x;
				xmax = PolyPoints->Points[0].x;
				ymin = PolyPoints->Points[0].y;
				ymax = PolyPoints->Points[0].y;
				for(i=1; i < PolyPoints->npoints; i++)
				{
					xmin = min2(xmin,PolyPoints->Points[i].x);
					xmax = max2(xmax,PolyPoints->Points[i].x);
					ymin = min2(ymin,PolyPoints->Points[i].y);
					ymax = max2(ymax,PolyPoints->Points[i].y);
				}
			}
			else
			{
				/* Gradient defined in figure units */
				xmin = SCALE_DOWN_X(0);
				ymin = SCALE_DOWN_Y(0);
				if (appres.landscape)
				{
					xmax = paper_sizes[appres.papersize].height;
					ymax = paper_sizes[appres.papersize].width;
				}
				else
				{
					xmax = paper_sizes[appres.papersize].width;
					ymax = paper_sizes[appres.papersize].height;
				}
				if (!appres.INCHES)
				{
					xmax = (int)(xmax*2.54*PIX_PER_CM/PIX_PER_INCH);
					ymax = (int)(ymax*2.54*PIX_PER_CM/PIX_PER_INCH);
				}
				xmax = SCALE_DOWN_X(xmax);
				ymax = SCALE_DOWN_Y(ymax);
			}			
			GradientPolygonFill(rastport, PolyPoints->Points, PolyPoints->npoints, GradientTable[Specialfill], xmin, ymin, xmax, ymax);
		}
    }
	else if (Specialfill == -2)
	{
		/* Polygon fill with transparence */
		PolygonFillAlpha(rastport, PolyPoints->Points, PolyPoints->npoints, ColorPalette[fill_color].RGB);
	}
    else
    {
        static zXPoint   Local_Points[MAXPOINTNUM];
        int             Local_NbPoints;

		/* We need to clip to visible area due to a bug in graphic.library */
        CopyMem(PolyPoints->Points, Local_Points, PolyPoints->npoints * sizeof(zXPoint));
        Local_NbPoints = PolyPoints->npoints;
        /* first pass (clip on left side) */
        Width  = GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
        Height = GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);
        cp_clipplane( &Local_NbPoints, Local_Points, CP_LEFT, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_RIGHT, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_TOP, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_BOTTOM, Width, Height );
		
        /* Use standard drawing functions */
        AreaMove(rastport, Local_Points[0].x, Local_Points[0].y);
        for( i = 1 ; i < Local_NbPoints ; i++ )
            AreaDraw(rastport, Local_Points[i].x, Local_Points[i].y);
        AreaEnd(rastport);
    }
    SetRPAttrs(rastport, RPTAG_PenMode, FALSE,  RPTAG_FgColor, SaveFgCol, TAG_DONE);
}

void  pw_area_simple(zXPoint *Points, int nbpoints)
{
	int     i;
    ULONG   SaveFgCol;
    WORD    Width, Height;
	BOOL	Draw = FALSE;
        
	Width  = GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
    Height = GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);
    	
	for( i = 0; i < nbpoints; i++)
	{
		if ((Points[i].x >= 0) && (Points[i].x < Width) && (Points[i].y >= 0) && (Points[i].y < Height))
		{
			Draw = TRUE;
			break;
		}
	}
	
	if (Draw)
	{
        zXPoint   Local_Points[12];
        int      Local_NbPoints;
 
        /* perform drawing */
		GetRPAttrs(rastport, RPTAG_FgColor, &SaveFgCol, TAG_DONE);
        CopyMem(Points, Local_Points, nbpoints * sizeof(zXPoint));
        Local_NbPoints = nbpoints;
        /* first pass (clip on left side) */
        cp_clipplane( &Local_NbPoints, Local_Points, CP_LEFT, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_RIGHT, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_TOP, Width, Height );
        cp_clipplane( &Local_NbPoints, Local_Points, CP_BOTTOM, Width, Height );
		AreaMove(rastport, Local_Points[0].x, Local_Points[0].y);
		for( i = 1 ; i < Local_NbPoints ; i++ )
			AreaDraw(rastport, Local_Points[i].x, Local_Points[i].y);
		AreaEnd(rastport);
		SetRPAttrs(rastport, RPTAG_PenMode, FALSE,  RPTAG_FgColor, SaveFgCol, TAG_DONE);
	}
}

/* Polygon drawing */

/* berechnet den intersection_point der Vektoren a und b
	*     (l1x)   (lx)     (m1x)   (mx)
	*  a: (   )+s*(  )  b: (   )+t*(  )
	*     (l1y)   (ly)     (m1y)   (my)
	*  Der R�ckgabewert ist t, der Teil, um den b bis zum intersection_point
	*  verl�ngert werden mu�.
	*  0, falls keine Werte f�r s oder t ermittelt werden konnten
	*  In diesem Fall wird err auf TRUE gesetzt
*/
float intersection_point(int l1x, int l1y, int l2x, int l2y, int m1x, int m1y, int m2x, int m2y, int *punktx, int *punkty, float *s, BOOL *err)
{
	float t;         /* Faktor */
	int lx,ly,mx,my;

	*err=0;

	lx=l2x-l1x;
	ly=l2y-l1y;

	// Linie ist ein Punkt
	if(lx == 0 && ly == 0)
	// if(lx+ly == 0)
	{
		*err=1;
		return(0);
	}

	mx=m2x-m1x;
	my=m2y-m1y;

	// Linie ist ein Punkt
	if(mx == 0 && my == 0)
	{
		*err=1;
		return(0);
	}

	if(lx*my-ly*mx)
    {
        t=(float)((l1y-m1y)*lx+(m1x-l1x)*ly) / (lx*my-ly*mx);
    }
	else    // parallel
	{
		*s=0;
		*err=2;
		return(0);
	}

	if(lx)
        *s=((float)m1x + t*mx - l1x) / lx;
	else
        *s=((float)m1y + t*my - l1y) / ly;

	*punktx=round((float)m1x+t*mx);
	*punkty=round((float)m1y+t*my);

	return(t);
}


// aus 2 Punkten und thickness ein rechteckiges Segment berechnen
void calc_outline(WORD x1, WORD y1, WORD x2, WORD y2, WORD points[5][2], int thickness)
{
	int normx, normy, nx, ny, nx2, ny2, thick2;
	float inv_len;

	thick2=thickness/2;

	// Normale ausrechnen
	normx=y2 - y1;
	normy=x1 - x2;

	// Normale auf halbe Linienst�rke normieren
	inv_len=1/sqrt((float)(normx*normx + normy*normy));

	nx=round((float)normx*inv_len * thick2);
	ny=round((float)normy*inv_len * thick2);

	nx2=round((float)normx*inv_len * (thickness-thick2 - 1));
	ny2=round((float)normy*inv_len * (thickness-thick2 - 1));

	// das Rechteck berechnen
	points[0][0]=x1+nx;
	points[0][1]=y1+ny;

	points[1][0]=x1-nx2;
	points[1][1]=y1-ny2;

	points[2][0]=x2-nx2;
	points[2][1]=y2-ny2;

	points[3][0]=x2+nx;
	points[3][1]=y2+ny;

	points[4][0]=points[0][0];
	points[4][1]=points[0][1];
}

void lengthen_dash(int *x1, int *y1, int *x2, int *y2, double sina, double cosa, int add)
{
	double xa,ya,xb,yb;

	// parallel zur X-Achse drehen
	xb = (double)*x2 * cosa - (double)*y2 * sina;
	yb = (double)*x2 * sina + (double)*y2 * cosa;

	// parallel zur X-Achse drehen
	xa = (double)*x1 * cosa - (double)*y1 * sina;
	ya = (double)*x1 * sina + (double)*y1 * cosa;

	// verl�ngern
	xb+=add;
	xa-=add;

	*x2 = round( xb * cosa + yb * sina);
	*y2 = round(-xb * sina + yb * cosa);

	*x1 = round( xa * cosa + ya * sina);
	*y1 = round(-xa * sina + ya * cosa);
}

/* Die Verbindung zwischen zwei rechteckigen Segmenten herstellen
	*
	* points		das letzte Segment
	* (px, py)		der gemeinsame Verbindungspunkt
	* last_points	das vorletzte Segment
	* join_style	die Art der Verbindung
	* thickness		Linienst�rke
*/
void make_join( WORD points[5][2], WORD px, WORD py, WORD last_points[4][2], int join_style, int thickness)
{
	float s=0.0f,t,cosa;
	int punktx=0,punkty=0,ax,ay,bx,by;
	BOOL err;
	WORD AreaPoints[4][2];

	if(last_points[0][0] != -1)
    {
        switch(join_style)
        {
            case JOIN_MITER:
                ax=last_points[1][0] - last_points[2][0];
                ay=last_points[1][1] - last_points[2][1];

                bx=points[2][0] - points[1][0];
                by=points[2][1] - points[1][1];

                // Angle between the 2 segments
                cosa=(float)(ax*bx+ay*by)/(sqrt((float)(ax*ax+ay*ay)) * sqrt((float)(bx*bx+by*by)));

                // Segments should not be almost parallel
                if(cosa < 0.98 && cosa > -0.995)
                {
                    // First intersection point between the 2 segments
                    t = intersection_point( last_points[1][0], last_points[1][1], last_points[2][0], last_points[2][1],
                                            points[2][0], points[2][1], points[1][0], points[1][1], &punktx, &punkty, &s, &err);

                    // if t >= 1 => We found the right one corner
                    if(s >= 1 && t >= 1)
                    {
                        AreaPoints[0][0] = punktx;
                        AreaPoints[0][1] = punkty;
                        AreaPoints[1][0] = points[1][0];
                        AreaPoints[1][1] = points[1][1];
                        AreaPoints[2][0] = px;
                        AreaPoints[2][1] = py;
                        AreaPoints[3][0] = last_points[2][0];
                        AreaPoints[3][1] = last_points[2][1];
                        pw_area_simple((zXPoint *)AreaPoints, 4);
                    }
                    else
                    {
                        // Second intersection point between the 2 segments
                        t = intersection_point( last_points[0][0], last_points[0][1], last_points[3][0], last_points[3][1],
                                                points[3][0], points[3][1], points[0][0], points[0][1], &punktx, &punkty, &s, &err);

                        // t >= 1 => This is the right corner
                        if(s >= 1 && t >= 1)
                        {
                            AreaPoints[0][0] = punktx;
                            AreaPoints[0][1] = punkty;
                            AreaPoints[1][0] = points[3][0];
                            AreaPoints[1][1] = points[3][1];
                            AreaPoints[2][0] = px;
                            AreaPoints[2][1] = py;
                            AreaPoints[3][0] = last_points[0][0];
                            AreaPoints[3][1] = last_points[0][1];
                            pw_area_simple((zXPoint *)AreaPoints, 4);
                        }
                    }
                }
                break;

            case JOIN_ROUND:
                pw_dot((LONG)px, (LONG)py, thickness);
                break;

            case JOIN_BEVEL:
                t = intersection_point( last_points[1][0], last_points[1][1], last_points[2][0], last_points[2][1],
                                        points[2][0], points[2][1], points[1][0], points[1][1], &punktx, &punkty, &s, &err);

                if(t > 1.001)
                {
                    AreaPoints[0][0] = last_points[2][0];
                    AreaPoints[0][1] = last_points[2][1];
                    AreaPoints[1][0] = points[1][0];
                    AreaPoints[1][1] = points[1][1];
                    AreaPoints[2][0] = px;
                    AreaPoints[2][1] = py;
                    pw_area_simple((zXPoint *)AreaPoints, 3);
                }
                else
                {
                    t=intersection_point(last_points[0][0], last_points[0][1], last_points[3][0], last_points[3][1],
                    points[3][0], points[3][1], points[0][0], points[0][1], &punktx, &punkty, &s, &err);

                    if(t > 1.001)
                    {
                        AreaPoints[0][0] = last_points[3][0];
                        AreaPoints[0][1] = last_points[3][1];
                        AreaPoints[1][0] = points[0][0];
                        AreaPoints[1][1] = points[0][1];
                        AreaPoints[2][0] = px;
                        AreaPoints[2][1] = py;
                        pw_area_simple((zXPoint *)AreaPoints, 3);
                    }
                }
                break;
                
            default :
                // Unknown JOIN STYLE
                break;
        }
    }
}

void pw_line_array(PointList* PolyPoints, BOOL closed, int style, float style_val, int join_style, int cap_style, int thickness, Color color, int op)
{
	int thick2;
	double l,sina=0.0,cosa=0.0;    
	int point, lastpx, lastpy, xa=0, ya=0, xb=0, yb=0;
	WORD cur_points[5][2],first_points[5][2];
	WORD last_points[4][2]={{-1,-1}, {-1,-1}, {-1,-1}, {-1,-1}};
	BOOL first=TRUE,edge=FALSE;
	int dash_num[4], gap_num, dashes = 0, cur_dash = 0;
	BOOL paint;
	int x1, y1, x2, y2, count;
	register int d,dx,dy;
	register int s1,s2,t,i1,i2,i;

    if (thickness == 0)
        return;
		
	DefinePenColor(color,op);

    /* convert to display units */
    thickness = round((float)thickness * display_zoomscale);
    style_val = style_val * display_zoomscale + (float)thickness;
    
    thick2 = (thickness&1)? thickness/2 : thickness/2-1;
    
    switch(style)
    {
        case SOLID_LINE:
            gap_num = 0;
            dash_num[0] = 0;
            dashes = 0;
            cap_style = CAP_BUTT;
            break;
        case DASH_LINE:
            gap_num = round(style_val);
            dash_num[0] = round(style_val);
            dashes = 1;
            break;
        case DOTTED_LINE:
            gap_num = round(style_val);
            dash_num[0] = 1;
            dashes = 1;
            break;
        case DASH_DOT_LINE:
            gap_num = round(style_val);
            dash_num[0] = round(style_val);
            dash_num[1] = 1;
            dashes = 2;
            break;
        case DASH_2_DOTS_LINE:
            gap_num = round(style_val);
            dash_num[0] = round(style_val);
            dash_num[1] = 1;
            dash_num[2] = 1;
            dashes = 3;
            break;
        case DASH_3_DOTS_LINE:
            gap_num = round(style_val);
            dash_num[0] = round(style_val);
            dash_num[1] = 1;
            dash_num[2] = 1;
            dash_num[3] = 1;
            dashes = 4;
            break;
        default:                    // Same as SOLID_LINE
            gap_num = 0;
            dash_num[0] = 0;
            dashes = 0;
            cap_style = CAP_BUTT;
            break;
    }
        
	lastpx=PolyPoints->Points[0].x, lastpy=PolyPoints->Points[0].y;

	for(point=1,paint=TRUE,count=0; point < PolyPoints->npoints; point++)
	{
		x1=PolyPoints->Points[point-1].x;
		y1=PolyPoints->Points[point-1].y;
		x2=PolyPoints->Points[point].x;
		y2=PolyPoints->Points[point].y;
	
		if (dashes == 0)
		{
			// SOLID LINE
			if (thickness > 1)
			{
				calc_outline(x1, y1, x2, y2, cur_points, thickness);
				pw_area_simple( (zXPoint *)cur_points, 4);

				if(first)
				{
					CopyMem(cur_points, first_points, 5*4);
					first=FALSE;
				}
				else
				{
					make_join( cur_points, x1, y1, last_points, join_style, thickness);
				}
				CopyMem(cur_points, last_points, 4*4);
			}
			else
			{
				Move(rastport, x1, y1);
				Draw(rastport, x2, y2);
			}
		}
		else
		{
			// start Bresenham            
			dx=abs(x2-x1);
			dy=abs(y2-y1);

			if((l = sqrt((double)dx * (double)dx + (double)dy * (double)dy)) != 0)
			{
				// a ist Winkel zwischen Linie und X-Achse
				sina = ((double)y2-(double)y1) / l;
				cosa = ((double)x2-(double)x1) / l;
			}

			s1=(x2-x1 >= 0) ? 1 : -1;
			s2=(y2-y1 >= 0) ? 1 : -1;

			if(dx < dy)
			{
				d=dx;
				dx=dy;
				dy=d;
				t=TRUE;
			}
			else
				t=0;

			d=2*dy-dx;

			i1=2*dy;
			i2=2*dx;
						
			for(i=1; i <= dx; i++, count++)
			{
				if(paint)
				{
					if(count >= dash_num[cur_dash]-1)
					{
						count=0;
						paint=FALSE;
						
						xa=lastpx; ya=lastpy; xb=x1; yb=y1;
						
						if (thickness > 1)
						{
							if((dash_num[cur_dash]==1) && cap_style != CAP_ROUND) 
							{
								lengthen_dash(&xa, &ya, &xb, &yb, cosa, sina, thick2);
							}
							else if(cap_style == CAP_PROJECT)
							{
								lengthen_dash(&xa, &ya, &xb, &yb, sina, cosa, thick2);
							}

							calc_outline(xa, ya, xb, yb, cur_points, thickness);
							
							if(first)
							{
								CopyMem(cur_points, first_points, 5*4);
								first=FALSE;
							}
							
							if ((dash_num[cur_dash]==1) && cap_style == CAP_ROUND)
							{
								pw_dot(x1, y1, thickness);
							}
							else
							{
								pw_area_simple( (zXPoint *)cur_points, 4);
								if (cap_style == CAP_ROUND)
								{
									if(!edge)       // not an edge, draw also linecap
									{
										pw_dot(lastpx, lastpy, thickness);
									}

									pw_dot(x1, y1, thickness);
								}
							}
							if(edge)
							{
								make_join( cur_points, lastpx, lastpy, last_points, join_style, thickness);
								edge=FALSE;
							}
						}
						else
						{
							Move(rastport, xa, ya);
							Draw(rastport, xb, yb);
						}
					}
				}
				else
				{
					if(count >= gap_num)
					{
						count=0;
						paint=TRUE;
						cur_dash=(cur_dash+1)%dashes;
						lastpx=x1, lastpy=y1;
					}
				}

				if(d >= 0)
				{
					if(t) x1+=s1;
					else  y1+=s2;
					d-=i2;
				}

				if(t) y1+=s2;
				else  x1+=s1;

				d+=i1;
				// end Bresenham
			}

			if(paint && !(dash_num[cur_dash]==1 && cap_style == CAP_ROUND))
			{
				xa=lastpx; ya=lastpy; xb=x1; yb=y1;
				if (thickness > 1)
				{
					if((dash_num[cur_dash]==1) && cap_style != CAP_ROUND) 
					{
						lengthen_dash(&xa, &ya, &xb, &yb, cosa, sina, thick2);
					}
					else if(cap_style == CAP_PROJECT)
					{
						lengthen_dash(&xa, &ya, &xb, &yb, sina, cosa, thick2);
					}
					
					calc_outline(xa, ya, xb, yb, cur_points, thickness);
 
					if ((dash_num[cur_dash]==1) && cap_style == CAP_ROUND)
					{
						pw_dot(x1, y1, thickness);
					}
					else
					{
						pw_area_simple( (zXPoint *)cur_points, 4);
					}
					
					if(edge)
					{
						make_join( cur_points, lastpx, lastpy, last_points, join_style, thickness);
					}
					else 
					{
						if(cap_style == CAP_ROUND)
						{
							pw_dot(lastpx, lastpy, thickness);
						}
					}

					lastpx=x1, lastpy=y1;

					CopyMem(cur_points, last_points, 4*4);
					edge=TRUE;
				}
				else
				{
					Move(rastport, xa, ya);
					Draw(rastport, xb, yb);
				}
			}     
		}
    }

    if (closed)
	{
        if (paint && !(dash_num[cur_dash]==1 && cap_style == CAP_ROUND))
		{
            if (thickness > 1)
            {
                make_join( first_points, lastpx, lastpy, last_points, join_style, thickness);
            }
		}
	}
}

// clipping using Sutherland-Hodgeman algorithm
BOOL cp_inside( zXPoint p, int side, WORD Width, WORD Height  )
{
	switch( side )
	{
		case CP_LEFT:
			return (p.x >= 0);
		case CP_RIGHT:
			return (p.x < Width);
		case CP_TOP:
			return (p.y >= 0);
		case CP_BOTTOM:
			return (p.y < Height);
		default:
			return (FALSE);
	}
}

zXPoint cp_intersect( zXPoint p, zXPoint q, int side, WORD Width, WORD Height  )
{
	zXPoint t = {0,0};
	double a, b;

	/* find slope and intercept of segment pq */
	a = (double)( q.y - p.y ) / (double)( q.x - p.x );
	b = (double)(p.y - p.x * a);

	switch( side )
	{
		case CP_LEFT:
			t.x = 0;
			t.y = (WORD)(t.x * a + b);
			break;
		case CP_RIGHT:
			t.x = Width - 1;
			t.y = (WORD)(t.x * a + b);
			break;
		case CP_TOP:
			t.y = 0;
			if( isfinite(a) )
				t.x = (WORD)(( t.y - b ) / a);
			else
				t.x = p.x;
			break;
		case CP_BOTTOM:
			t.y = Height - 1;
			if( isfinite(a) )
				t.x = (WORD)(( t.y - b ) / a);
			else
				t.x = p.x;
			break;
	}
	return(t);
}

void cp_clipplane( int *nbpoints, zXPoint *in, int side, WORD Width, WORD Height )
{
	int i, j=0;
	zXPoint cur_point, prev_point;
	static zXPoint out[MAXPOINTNUM];

	prev_point = in[*nbpoints-1];
	for( i = 0 ; i < *nbpoints ; i++ )
	{
		cur_point = in[i];
		
		if( cp_inside( cur_point, side, Width, Height ) )
		{
			/* cur_point is "inside" */
			if( !cp_inside( prev_point, side, Width, Height ) )
			{
				/* cur_point is "inside" and prev_point is "outside" */
				out[j] = cp_intersect( cur_point, prev_point, side, Width, Height );
				j++;
			}
			out[j] = cur_point; 
			j++;
		}
		else if( cp_inside( prev_point, side, Width, Height ) )
		{
			/* prev_point is "inside" and cur_point is "outside" */
			out[j] = cp_intersect( prev_point, cur_point, side, Width, Height );
			j++;
		}

		prev_point = cur_point;
	}
	
	*nbpoints = j;
    CopyMem(out, in, *nbpoints * sizeof(zXPoint));
}

void calc_arcbox( int xmin, int ymin, int xmax, int ymax, int radius, int thickness, int style, float style_val)
{
	int arc_radius;

    arc_radius=round(sqrt((float)(radius*radius + radius*radius)));
      
    /* upper left */
    add_point_to_list( SCALE_DOWN_X(xmin), SCALE_DOWN_Y(ymin+radius), &GlobalPoints);
    curve( -radius, 0, 0, radius, FALSE, 0, arc_radius, arc_radius, xmin+radius, ymin+radius, style, &GlobalPoints);
    add_point_to_list( SCALE_DOWN_X(xmin+radius), SCALE_DOWN_Y(ymin), &GlobalPoints);

    /* upper right */
    add_point_to_list( SCALE_DOWN_X(xmax-radius), SCALE_DOWN_Y(ymin), &GlobalPoints);
    curve( 0, radius, radius, 0, FALSE, 0, arc_radius, arc_radius, xmax-radius, ymin+radius, style, &GlobalPoints);
    add_point_to_list( SCALE_DOWN_X(xmax), SCALE_DOWN_Y(ymin+radius), &GlobalPoints);

    /* lower right */
    add_point_to_list( SCALE_DOWN_X(xmax), SCALE_DOWN_Y(ymax-radius), &GlobalPoints);
    curve( radius, 0, 0, -radius, FALSE, 0, arc_radius, arc_radius, xmax-radius, ymax-radius, style, &GlobalPoints);
    add_point_to_list( SCALE_DOWN_X(xmax-radius), SCALE_DOWN_Y(ymax), &GlobalPoints);

    /* lower left */
    add_point_to_list( SCALE_DOWN_X(xmin+radius), SCALE_DOWN_Y(ymax), &GlobalPoints);
    curve( 0, -radius, -radius, 0, FALSE, 0, arc_radius, arc_radius, xmin+radius, ymax-radius, style, &GlobalPoints);
    add_point_to_list( SCALE_DOWN_X(xmin), SCALE_DOWN_Y(ymax-radius), &GlobalPoints);
    add_point_to_list( SCALE_DOWN_X(xmin), SCALE_DOWN_Y(ymin+radius), &GlobalPoints);
}
