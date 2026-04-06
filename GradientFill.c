/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/GradientFill.c $
 *       $Revision: 346 $
 *       $Date: 2017-09-10 13:15:04 +0000 (Sun, 10 Sep 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/******************************************************************

                    Polygon fill algorithm 
      based on public-domain code by Darel Rex Finley, 2007
      
*******************************************************************/
#include "fig.h"

#include "resources.h"
#include "object.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "GradientFill.h"

//#define TIMEMEAS1
#ifdef TIMEMEAS1
#include <proto/timer.h>
struct Device *TimerBase = NULL;
#endif

#define MAXNODES 256
static WORD nodeX[MAXNODES];
/* do not allocate slopes on stack and keep same buffer for both gradient and transparency fill routines */
static double slopes[MAXPOINTNUM];

static __inline LONG GetExtendIndex(int spread, double Extend)
{
	LONG Ext;
	Ext = (GRAD_PRECALC_DIM - 1) * Extend;

	if (appres.gradient_ditherspread > 0)
	{
		Ext += (rand() % (2*appres.gradient_ditherspread+1)) - appres.gradient_ditherspread;
	}
	
    switch(spread)
    {
        case 0:
            Ext = min2(max2(Ext, 0),(GRAD_PRECALC_DIM - 1));
            break;
        case 1:
            Ext &= (GRAD_PRECALC_DIM - 1);
            break;
        case 2:
        	Ext &= ((GRAD_PRECALC_DIM << 1) - 1);
        	if (Ext & GRAD_PRECALC_DIM) Ext ^= ((GRAD_PRECALC_DIM << 1) - 1);
            break;
    }
    return(Ext);
}

static __inline ULONG GetGradientColor(struct _Gradient *gradient, double ExtendPerCent)
{
    ULONG color = 0x00000000;
    ULONG C1, C2;
    ULONG B = 0x0,A = 0x0;
    int S1, S2;
    int i;
    ULONG   ratio1 = 0, ratio2 = 255;
    
    /* Look for the 2 stops to use */
    S1 = gradient->Stop[0].stopvalue;
    C1 = gradient->Stop[0].color;
    S2 = gradient->Stop[gradient->stops-1].stopvalue;
    C2 = gradient->Stop[gradient->stops-1].color;
    for (i=0;i<gradient->stops; i++)
    {
        if ((gradient->Stop[i].stopvalue <= (int)(ExtendPerCent)) && (gradient->Stop[i].stopvalue >= S1))
        {
            S1 = gradient->Stop[i].stopvalue;
            C1 = gradient->Stop[i].color;
        }
        if ((gradient->Stop[i].stopvalue >= (int)(ExtendPerCent)) && (gradient->Stop[i].stopvalue <= S2))
        {
            S2 = gradient->Stop[i].stopvalue;
            C2 = gradient->Stop[i].color;
        }
    }        
    if (S2 != S1)
    {
        ratio1 = round(255 * (ExtendPerCent - (double)S1)/(double)(S2 - S1));
        ratio2 = 255 - ratio1;
    }

    A = ((C2 & 0xFF00FF00) >> 8) * ratio1 + ((C1 & 0xFF00FF00) >> 8) * ratio2;
    B = ((C2 & 0x00FF00FF)       * ratio1 +  (C1 & 0x00FF00FF)       * ratio2) >> 8;

#ifdef __AROS__
    color = AROS_BE2LONG((A & 0xFF00FF00) | (B & 0x00FF00FF));
#else
    color = (A & 0xFF00FF00) | (B & 0x00FF00FF);
#endif


    return(color);
}

static inline void CalcSlopes(int NbPoints, zXPoint *Points, double *slopes)
{
	int i,j;
	WORD dx,dy;
	
	j=NbPoints-1;					
	for (i=0; i<NbPoints; i++) 
	{
		dy=Points[j].y-Points[i].y;
		dx=Points[j].x-Points[i].x;
		
		if(dy==0) 
			slopes[i]=1.0;
		else if(dx==0)
			slopes[i]=0.0;
		else
			slopes[i] = (double)dx / (double)dy;
			
		j=i; 
	}
}

static inline WORD BuildNodes(int NbPoints, zXPoint *Points, double *slopes, WORD Y, WORD *nodeX)
{
	WORD nodes = 0, swap;
	int i,j;
	
	j=NbPoints-1;					
	for (i=0; i<NbPoints; i++) 
	{
		if ((Points[i].y<Y && Points[j].y>=Y) ||  (Points[j].y<Y && Points[i].y>=Y)) 
		{
			// nodeX[nodes++] = Points[i].x + (((Points[j].x-Points[i].x) * (Y-Points[i].y)) / (Points[j].y-Points[i].y));
			nodeX[nodes++] = Points[i].x + (WORD)(slopes[i] * (Y-Points[i].y));
		}
		j=i; 
	}
	
	/*  Sort the nodes, via a simple �Bubble� sort. */
	i=0;
	while (i<nodes-1) 
	{
		if (nodeX[i] > nodeX[i+1]) 
		{
			swap       = nodeX[i];
			nodeX[i]   = nodeX[i+1];
			nodeX[i+1] = swap;
			if (i) i--; 
		}
		else
		{
			i++;
		}
	}
	
	return(nodes);
}

void GradientPolygonFill(struct RastPort *rastport, zXPoint *Points, int NbPoints, struct _Gradient *gradient, int xmin, int ymin, int xmax, int ymax)
{
    WORD    nodes, pixelX, pixelY, i;
    int     x1,y1,x2,y2,radius = 0;
    int     Width, Height, BMWidth, BMHeight;
    double  Theta = 0.0, cosTheta_GExtInv = 0.0, sinTheta_GExtInv = 0.0, GExtendInv = 1.0, sinTheta_GExtInv_dy = 0.0;
	double  DExtend = 0.0;
    ULONG   CExtend = 0;
    double  hyp = 0.0;
	double  inv_radius = 0.0;
	double	Xratio = 1.0, Yratio = 1.0; /* bounding box ratio */
	double  dx, dy, dy2, dy3;
	double  a = 0.0, b = 0.0;
	double  D = 0.0;	
	double	c = 0.0, dx1x2 = 0.0, dy1y2 = 0.0; 
    int     radius2 = 0;
    ULONG*  pixelbuf = NULL;
	ULONG*  precalcval = gradient->precalcval;
	int		spread = gradient->spread;
    int     pbw, pbh, index, baseindex;
#ifdef TIMEMEAS1
    struct EClockVal time1,time2,time3;
    ULONG E_Freq;
    LONG error;
    struct timerequest TimerIO;

    if (!(error = OpenDevice(TIMERNAME,UNIT_ECLOCK,
                  (struct IORequest *)&TimerIO,0L)) )
    {
        TimerBase = (struct Device *)TimerIO.tr_node.io_Device;

#endif                                           

    /* convert Gradient data to local coordinate system */
    Width   = xmax - xmin;
    Height  = ymax - ymin;
	if (Width > Height)
	{
		Yratio = (double)Width/(double)Height;
	}
	else if (Height > Width)
	{
		Xratio = (double)Height/(double)Width;
	}
    x1       = xmin + gradient->x1 * Width / 100;
    y1       = ymin + gradient->y1 * Height / 100;
    x2       = xmin + gradient->x2 * Width / 100;
    y2       = ymin + gradient->y2 * Height / 100;

	BMWidth	 = GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
	BMHeight = GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);
	
	/* 
		if gradient unit is figure, calculating the exact size of the object 
		allows to speed up things
		For gradient unit is object, ymin and ymax already have the right values
	*/
	if (gradient->unit == 1)
	{
		ymin = Points[0].y;
		ymax = Points[0].y;
        xmin = Points[0].x; 
        xmax = Points[0].x;
		for(i=1; i < NbPoints; i++)
		{
			ymin = min2(ymin,Points[i].y);
			ymax = max2(ymax,Points[i].y);
			xmin = min2(xmin,Points[i].x);
			xmax = max2(xmax,Points[i].x);
		}
	}
	ymin = max2(0, ymin);
	ymax = min2(BMHeight, ymax);
    pbh = (ymax - ymin);
	xmin = max2(0, xmin);
	xmax = min2(BMWidth, xmax);
    pbw = (xmax - xmin);

    /* Allocate temporary line buffer */
    pixelbuf = (ULONG *)AllocVec(pbh * pbw * 4,MEMF_ANY|MEMF_CLEAR);
    
    if (pixelbuf)
    {
		/* Pre-Calculate gradient if not already done */
		if (gradient->precalc == FALSE)
		{
			gradient->precalc = TRUE;
			for (i=0; i<GRAD_PRECALC_DIM; i++)
			{
				precalcval[i] = GetGradientColor(gradient, (double)(i)*100.0/(double)(GRAD_PRECALC_DIM - 1));
			}
		}
#ifdef TIMEMEAS1
		E_Freq =  ReadEClock((struct EClockVal *) &time1); /* Get initial reading */
#endif                                              		
		/* Pre-Calculate slopes */
		CalcSlopes(NbPoints, Points, slopes);		
		
		switch (gradient->type)
		{
			case 0:
				/* Linear Gradient */
				/* Calculate Gradient Extend invert */
				dx = (x2-x1);
				dy = (y2-y1);
				hyp = sqrt((double)(dx*dx + dy*dy));
				GExtendInv = 1.0/hyp;
				/* Calculate Gradient Angle */
				Theta = atan2(dy,dx);
				cosTheta_GExtInv = cos(Theta) * GExtendInv;
				sinTheta_GExtInv = sin(Theta) * GExtendInv;					
				/*  Loop through the rows of the image */
				for (pixelY=ymin; pixelY<ymax; pixelY++) 
				{
					/*  Build a list of nodes. */
					nodes = BuildNodes(NbPoints, Points, slopes, pixelY, nodeX);
					/*  Fill the pixels between node pairs. */
					dy = (double)(pixelY-y1);
					sinTheta_GExtInv_dy = sinTheta_GExtInv * dy;
					baseindex = (pixelY - ymin) * pbw - xmin;
					for (i=0; i<nodes; i+=2) 
					{
						if   (nodeX[i  ]>=BMWidth) break;
						if   (nodeX[i+1]> 0 ) 
						{              
							if (nodeX[i  ]< 0 ) nodeX[i  ]=0 ;
							if (nodeX[i+1]> BMWidth) nodeX[i+1]=BMWidth;
							index =  baseindex + nodeX[i];
							for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++)
							{
								dx = (double)(pixelX-x1);
								DExtend	= cosTheta_GExtInv * dx + sinTheta_GExtInv_dy;
								CExtend = GetExtendIndex(spread, DExtend);
								pixelbuf[index++] = precalcval[CExtend];
							}
						}
					}
				}
				break;
			case 1:             
				/* Radial Gradient */
				radius  = gradient->radius * max2(Width,Height) / 100;
				inv_radius = 1.0/(double)radius;
				radius2 = radius * radius;
				if ((x1 == x2) && (y1 == y2))
				{
					/* Nominal case : focal point equal center point */
					/*  Loop through the rows of the image */
					for (pixelY=ymin; pixelY<ymax; pixelY++) 
					{
						/*  Build a list of nodes. */
						nodes = BuildNodes(NbPoints, Points, slopes, pixelY, nodeX);
						/*  Fill the pixels between node pairs. */
						dy = (double)(pixelY-y1) * Yratio;
						dy2 = dy*dy;
						baseindex = (pixelY - ymin) * pbw - xmin;
						for (i=0; i<nodes; i+=2) 
						{
							if   (nodeX[i  ]>=BMWidth) break;
							if   (nodeX[i+1]> 0 ) 
							{              
								if (nodeX[i  ]< 0 ) nodeX[i  ]=0 ;
								if (nodeX[i+1]> BMWidth) nodeX[i+1]=BMWidth;
								index = baseindex + nodeX[i];
								for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++)
								{							
									dx = (double)(pixelX-x1) * Xratio;
									DExtend	= sqrt(dx*dx + dy2) * inv_radius;
									CExtend = GetExtendIndex(spread, DExtend);
									pixelbuf[index++] = precalcval[CExtend];
								}
							}
						}
					}
				}
				else
				{
					/* Special case : gradient with focal point */
					/* check that focal point in inside the circle, otherwise set focal point "on" circle */
					hyp = sqrt((double)((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
					if (1.05 * hyp > radius)
					{
						x2 = x1 + round(((double)(x2-x1) / hyp) * (double)radius * 0.95);
						y2 = y1 + round(((double)(y2-y1) / hyp) * (double)radius * 0.95);
					}
					/* precalc values for faster processing */
					dx1x2 = x2 - x1;
					dy1y2 = y2 - y1;
					c = (x1*x1 + y1*y1 + x2*x2 + y2*y2 - 2.0 * (x1*x2 + y1*y2) - radius2);
					/*  Loop through the rows of the image */
					for (pixelY=ymin; pixelY<ymax; pixelY++) 
					{
						/*  Build a list of nodes. */
						nodes = BuildNodes(NbPoints, Points, slopes, pixelY, nodeX);
						/*  Fill the pixels between node pairs. */
						dy = (double)(pixelY-y2) * Yratio;
						dy2 = dy*dy;
						dy3 = dy*dy1y2;
						baseindex = (pixelY - ymin) * pbw - xmin;
						for (i=0; i<nodes; i+=2) 
						{
							if   (nodeX[i  ]>=BMWidth) break;
							if   (nodeX[i+1]> 0 ) 
							{              
								if (nodeX[i  ]< 0 ) nodeX[i  ]=0 ;
								if (nodeX[i+1]> BMWidth) nodeX[i+1]=BMWidth;
								index = baseindex + nodeX[i];
								for (pixelX=nodeX[i]; pixelX<nodeX[i+1]; pixelX++)
								{
									/* 
										calculate the intersection point between 
										1. the max gradient circle and
										2. the line formed by focal point and current point 
									*/
									dx = (double)(pixelX-x2) * Xratio; 
									a = (dx*dx + dy2);
									b = (dx*dx1x2 + dy3);								
									D = sqrt((b*b) - (a*c));
									DExtend = a / (D - b);
									CExtend = GetExtendIndex(spread, DExtend);
									pixelbuf[index++] = precalcval[CExtend];
								}
							}
						}
					}
					/* for radial gradients with focal point, focal point is a singular point to be over-ridden */
					pixelbuf[(y2 - ymin) * pbw + (x2 - xmin)] = precalcval[0];
				}
				break;
			default:
				/* error, unknown gradient type */
				/* do nothing */
				break;
		}
    
#ifdef TIMEMEAS1
        E_Freq =  ReadEClock((struct EClockVal *) &time2); /* Get 2nd reading */
#endif                                           
        /* Plot the pixel Array (do it in steps so that Nouveau drivers doesn't get blocked) */
        i = 0;
        while (i < pbh)
        {
            WritePixelArrayAlpha(	(APTR)pixelbuf,
                                    0,
                                    i,
                                    pbw * 4,
                                    rastport,
                                    xmin,
                                    ymin + i,
                                    pbw,
                                    min2(pbh - i, 256),
                                    0xFFFFFFFF);
            i += 256;
        }
        /* Release temporary pixelbuf */
        FreeVec(pixelbuf);
#ifdef TIMEMEAS1
        E_Freq =  ReadEClock((struct EClockVal *) &time3); /* Get 3rd reading */
    	printf("\nPreparation : %f ms\tBlit : %f ms\n",
            	(time2.ev_lo-time1.ev_lo)/(double)E_Freq*1000.0,
            	(time3.ev_lo-time2.ev_lo)/(double)E_Freq*1000.0);
    }
    CloseDevice( (struct IORequest *) &TimerIO );
#endif
}
}

void PolygonFillAlpha(struct RastPort *rastport, zXPoint *Points, int NbPoints, ULONG color)
{
    WORD    nodes, line, i;
    int     BMWidth,BMHeight;
    ULONG*  pixelbuf = NULL;
    int     pix, index;
	WORD	ymin, ymax, xmin, xmax, pbh, pbw;
	
	BMWidth	= GetBitMapAttr(rastport->BitMap, BMA_WIDTH);
	BMHeight= GetBitMapAttr(rastport->BitMap, BMA_HEIGHT);

	/* look for first and last line */
    ymin = Points[0].y;
    ymax = Points[0].y;
    xmin = Points[0].x; 
    xmax = Points[0].x;
    for(i=1; i < NbPoints; i++)
    {
        ymin = min2(ymin,Points[i].y);
        ymax = max2(ymax,Points[i].y);
        xmin = min2(xmin,Points[i].x);
        xmax = max2(xmax,Points[i].x);
    }
	ymin = max2(0, ymin);
	ymax = min2(BMHeight, ymax);
    pbh = (ymax - ymin);
	xmin = max2(0, xmin);
	xmax = min2(BMWidth, xmax);
    pbw = (xmax - xmin);
	
	/* transform color to correct format in advance */
#ifdef __AROS__
	color = AROS_BE2LONG(color);
#endif
	
    /* Allocate temporary line buffer */
    pixelbuf = (ULONG *)AllocVec(pbh * pbw * 4,MEMF_ANY|MEMF_CLEAR);
    
    if (pixelbuf)
    {
		/* Pre-Calculate slopes */
		CalcSlopes(NbPoints, Points, slopes);		
		
        /*  Loop through the rows of the image */
        for (line=ymin; line<ymax; line++) 
        {
			/*  Build a list of nodes. */
			nodes = BuildNodes(NbPoints, Points, slopes, line, nodeX);          
            /*  Fill the pixels between node pairs. */
            for (i=0; i<nodes; i+=2) 
            {
                if   (nodeX[i  ]>=BMWidth) break;
                if   (nodeX[i+1]> 0 ) 
                {
                    if (nodeX[i  ]< 0 )      nodeX[i]   = 0;
                    if (nodeX[i+1]> BMWidth) nodeX[i+1] = BMWidth;
                    /* set pixelbuf to current fill color */
					index = (line - ymin) * pbw + (nodeX[i] - xmin);
                    for (pix=nodeX[i]; pix < nodeX[i+1]; pix++)
                    {
                        pixelbuf[index++] = color;
                    }
                }
            }
        }
        /* Plot the pixel Array (do it in steps so that Nouveau drivers doesn't get blocked) */
        i = 0;
        while (i < pbh)
        {
            WritePixelArrayAlpha(	(APTR)pixelbuf,
                                    0,
                                    i,
                                    pbw * 4,
                                    rastport,
                                    xmin,
                                    ymin + i,
                                    pbw,
                                    min2(pbh - i, 256),
                                    0xFFFFFFFF);
            i += 256;
        }
        FreeVec(pixelbuf);
    }
}
