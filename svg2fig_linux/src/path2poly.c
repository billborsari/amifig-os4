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

#define MAXPOINTS	8192

#define NONE		0x00
#define MOVETO		0x01
#define LINETO		0x02
#define	HLINETO		0x03
#define	VLINETO     0x04
#define	CURVETO		0x05
#define SCURVETO	0x06
#define	QCURVETO	0x07
#define SQCURVETO	0x08
#define CLOSE		0x09

#define ABSOLUTE	0x00
#define RELATIVE	0x01

float *readpoints(char *s,int *n)
{
	static float output[6];
	int i=0;
	char *tmp;
	
	tmp = s;
	
	while(*tmp && i<*n)	output[i++] = strtod(tmp,&tmp);

	*n = tmp - s;
	return (output);
}

void curve2poly(float *points, float X0, float Y0, int steps, float *OutPoints)
{
	//the 4 bezier points
	float p[4];
	int loop;
	float f, fd, fdd, fddd, fdd_per_2, fddd_per_2, fddd_per_6;
	float t = 1.0 / (float)steps;
	float temp = t * t;
	
	// first pass fo X coordinates
	p[0] = X0;
	p[1] = points[0];
	p[2] = points[2];
	p[3] = points[4];	

	f = p[0];
	fd = 3 * (p[1] - p[0]) * t;
	fdd_per_2 = 3 * (p[0] - 2 * p[1] + p[2]) * temp;
	fddd_per_2 = 3 * (3 * (p[1] - p[2]) + p[3] - p[0]) * temp * t;
	fddd = fddd_per_2 + fddd_per_2;
	fdd = fdd_per_2 + fdd_per_2;
	fddd_per_6 = fddd_per_2 * (1.0 / 3);

	for (loop=0; loop <  (steps-1); loop++) 
	{
		f = f + fd + fdd_per_2 + fddd_per_6;
		fd = fd + fdd + fddd_per_2;
		fdd = fdd + fddd;
		fdd_per_2 = fdd_per_2 + fddd_per_2;

		OutPoints[loop * 2] = f;
	}

	OutPoints[(steps-1) * 2] = p[3];

	// second pass fo Y coordinates
	p[0] = Y0;
	p[1] = points[1];
	p[2] = points[3];
	p[3] = points[5];	

	f = p[0];
	fd = 3 * (p[1] - p[0]) * t;
	fdd_per_2 = 3 * (p[0] - 2 * p[1] + p[2]) * temp;
	fddd_per_2 = 3 * (3 * (p[1] - p[2]) + p[3] - p[0]) * temp * t;
	fddd = fddd_per_2 + fddd_per_2;
	fdd = fdd_per_2 + fdd_per_2;
	fddd_per_6 = fddd_per_2 * (1.0 / 3);

	for (loop=0; loop <  (steps-1); loop++) 
	{
		f = f + fd + fdd_per_2 + fddd_per_6;
		fd = fd + fdd + fddd_per_2;
		fdd = fdd + fddd;
		fdd_per_2 = fdd_per_2 + fddd_per_2;

		OutPoints[loop * 2 + 1] = f;
	}

	OutPoints[(steps-1) * 2 + 1] = p[3];
}

void qcurve2poly(float *points, float X0, float Y0, int steps, float *OutPoints)
{
    int i;
	float t,a,b,c;
    
	for (i=1; i < steps; ++i)
    {
        t = (float)i / (float)steps;
        a = (1.0 - t) * (1.0 - t);
        b = 2.0 * t * (1.0 - t);
        c = t * t;
        OutPoints[(i-1) * 2]	 = a * X0 + b * points[0] + c * points[2];
        OutPoints[(i-1) * 2 + 1] = a * Y0 + b * points[1] + c * points[3];
    }
	OutPoints[(steps-1) * 2]	  = points[2];
	OutPoints[(steps-1) * 2 + 1] = points[3];
}

float *path2poly(char *s, int *nbpoints, int *nbchar, int *Type, int interp_steps)
{
	unsigned int i;
	static float OrigX = -1, OrigY = -1;
	float PCtrlX = 0, PCtrlY = 0;
	int Comm = NONE;
	int Mode = ABSOLUTE;
	float *Poly = NULL;
	float *points;
	int n;
	char *s_save;
	
	// Allocate a buffer big enough, if nbPoints get bigger than this, return NULL (error)
	Poly = (float *)malloc(MAXPOINTS * 2 * sizeof(float));
	*nbpoints = 0;
	*Type = 0;
	
	// keep the original pointer to be able to return number of read characters
	s_save = s;
	
	// start by cleaning up the string, so we don't have to mess up with different separators later
	for (i = 0; s[i]; i++)
		if (s[i] == ',' || s[i] == '\t' || s[i] == '\n')
			s[i] = ' ';
			
	while (*s)
	{
		// go to next non empty character
		while ((*s) && (s[0] == ' ')) s++;
		
		if (!(*s))
		{
			if (Comm == NONE) goto error;	// empty string passed
			else break;						// end of the path	
		}
		
		if (s[0]=='M')
		{
			if (Comm==NONE)
			{
				Comm = MOVETO;
				Mode = ABSOLUTE;
			}
			else
			{
				// we are starting a new path, return a polyline
				s--;
				*nbchar = s - s_save;
				if (*nbpoints == 2) *Type = 2;
				else *Type = 0;
				return(Poly);
			}
		}
		else if (s[0]=='m')
		{
			Comm = MOVETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='L')
		{
			Comm = LINETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='l')
		{
			Comm = LINETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='H')
		{
			Comm = HLINETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='h')
		{
			Comm = HLINETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='V')
		{
			Comm = VLINETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='v')
		{
			Comm = VLINETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='C')
		{
			Comm = CURVETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='c')
		{
			Comm = CURVETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='S')
		{
			Comm = SCURVETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='s')
		{
			Comm = SCURVETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='Q')
		{
			Comm = QCURVETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='q')
		{
			Comm = QCURVETO;
			Mode = RELATIVE;
		}
		else if (s[0]=='T')
		{
			Comm = SQCURVETO;
			Mode = ABSOLUTE;
		}
		else if (s[0]=='t')
		{
			Comm = SQCURVETO;
			Mode = RELATIVE;
		}
		else if ((s[0]=='Z') || (s[0]=='z'))
		{
			Comm = CLOSE;
			Mode = ABSOLUTE;
		}
		else if ((Comm!=NONE) 
				 && ((s[0]=='0') || (s[0]=='1') || (s[0]=='2') 
				 || (s[0]=='3') || (s[0]=='4') || (s[0]=='5') 
				 || (s[0]=='6') || (s[0]=='7') || (s[0]=='8') 
				 || (s[0]=='9') || (s[0]=='.') || (s[0]=='-')
				 || (s[0]=='+')))
		{
			//we are in a following loop with the same command just go on
			s--;
		}
		else
			goto error;
		
		// move to next character
		s++;
		
		// check if MAXPOINTS is reached if yes return incomplete polynome; 
		if (*nbpoints > (MAXPOINTS - interp_steps)) Comm = CLOSE; 
		
		if (Comm == CLOSE)
		{
			// we shall return a closed path, i.e. a polygon
			*nbchar = s - s_save;
			if (*nbpoints == 2) *Type = 2;
			else *Type = 1;
			return(Poly);
		}
			
		// go to next non empty character
		while ((*s) && (s[0] == ' ')) s++;
		if (!(*s)) goto error;

		//check if we are in a second call
		if ((Comm != MOVETO) && (OrigX != -1) && (OrigY != -1) && (*nbpoints==0))
		{
			Poly[0] = OrigX;
			Poly[1] = OrigY;
			*nbpoints = 1;
			// reset control points
			PCtrlX = OrigX;
			PCtrlY = OrigY;			
		}
		
		switch(Comm)
		{
		case MOVETO:
			// read the new points
			n = 2;
			points = readpoints(s,&n);
			// This is the origin point and the first point of the Poly
			OrigX = Poly[0] = points[0];
			OrigY = Poly[1] = points[1];
			*nbpoints = 1;
			s += n;
			// subsequent commands are LINETO
			Comm = LINETO;
			// reset control points
			PCtrlX = OrigX;
			PCtrlY = OrigY;
			break;
        case LINETO:
 			// read the new points
			n = 2;
			points = readpoints(s,&n);
			if (Mode == ABSOLUTE)
			{
				Poly[*nbpoints * 2] 	= points[0];
				Poly[*nbpoints * 2 + 1] = points[1];
			}
			else
			{
				Poly[*nbpoints * 2] 	= points[0] + OrigX;
				Poly[*nbpoints * 2 + 1] = points[1] + OrigY;
			}
			// update origin
			OrigX = Poly[*nbpoints * 2];
			OrigY = Poly[*nbpoints * 2 + 1];
			// reset control points
			PCtrlX = Poly[*nbpoints * 2];
			PCtrlY = Poly[*nbpoints * 2 + 1];
			// Increase counters
			(*nbpoints)++;
			s += n;
			break;
		case HLINETO:
			// read the new points
			n = 1;
			points = readpoints(s,&n);
			if (Mode == ABSOLUTE)
			{
				Poly[*nbpoints * 2] 	= points[0];
				Poly[*nbpoints * 2 + 1] = Poly[(*nbpoints-1) * 2 + 1];
			}
			else
			{
				Poly[*nbpoints * 2] 	= points[0] + OrigX;
				Poly[*nbpoints * 2 + 1] = Poly[(*nbpoints-1) * 2 + 1];
			}
			// update origin
			OrigX = Poly[*nbpoints * 2];
			OrigY = Poly[*nbpoints * 2 + 1];
			// reset control points
			PCtrlX = Poly[*nbpoints * 2];
			PCtrlY = Poly[*nbpoints * 2 + 1];
			// Increase counters
			(*nbpoints)++;
			s += n;
			break;
        case VLINETO:
			// read the new points
			n = 1;
			points = readpoints(s,&n);
			if (Mode == ABSOLUTE)
			{
				Poly[*nbpoints * 2] 	= Poly[(*nbpoints-1) * 2];
				Poly[*nbpoints * 2 + 1] = points[0];
			}
			else
			{
				Poly[*nbpoints * 2] 	= Poly[(*nbpoints-1) * 2];
				Poly[*nbpoints * 2 + 1] = points[0] + OrigX;
			}
			// update origin
			OrigX = Poly[*nbpoints * 2];
			OrigY = Poly[*nbpoints * 2 + 1];
			// reset control points
			PCtrlX = Poly[*nbpoints * 2];
			PCtrlY = Poly[*nbpoints * 2 + 1];
			// Increase counters
			(*nbpoints)++;
			s += n;
			break;
        case CURVETO:
			// read the new points
			n = 6;
			points = readpoints(s,&n);
			if (Mode == RELATIVE)
			{
				// translate to absolute mode
				points[0]+=OrigX;
				points[1]+=OrigY;
				points[2]+=OrigX;
				points[3]+=OrigY;
				points[4]+=OrigX;
				points[5]+=OrigY;
			}
			// update origin
			OrigX = points[4];
			OrigY = points[5];
			//calculate the estimated poly
			curve2poly(points, Poly[(*nbpoints-1) * 2], Poly[(*nbpoints-1) * 2 + 1], interp_steps, &Poly[*nbpoints * 2]);

			// reset control points
			PCtrlX = points[2];
			PCtrlY = points[3];
			// Increase counters
			(*nbpoints)+=interp_steps;
			s += n;
			break;
        case SCURVETO:
			// read the new points
			n = 4;
			points = readpoints(s,&n);
			if (Mode == RELATIVE)
			{
				// translate to absolute mode
				points[0]+=OrigX;
				points[1]+=OrigY;
				points[2]+=OrigX;
				points[3]+=OrigY;
			}
			// build complete CURVETO
			points[5] = points[3];
			points[4] = points[2];
			points[3] = points[1];
			points[2] = points[0];
			// calculate first control point form current point and previous ctrl point
			points[0] = Poly[(*nbpoints-1) * 2]		+ (Poly[(*nbpoints-1) * 2]		- PCtrlX);
			points[1] = Poly[(*nbpoints-1) * 2 + 1]	+ (Poly[(*nbpoints-1) * 2 + 1]	- PCtrlY);
			// update origin
			OrigX = points[4];
			OrigY = points[5];
			
			//calculate the estimated poly
			curve2poly(points, Poly[(*nbpoints-1) * 2], Poly[(*nbpoints-1) * 2 + 1], interp_steps, &Poly[*nbpoints * 2]);
			
			// reset control points
			PCtrlX = points[2];
			PCtrlY = points[3];
			// Increase counters
			(*nbpoints)+=interp_steps;
			s += n;
			break;
        case QCURVETO:	
			// read the new points
			n = 4;
			points = readpoints(s,&n);
			if (Mode == RELATIVE)
			{
				// translate to absolute mode
				points[0]+=OrigX;
				points[1]+=OrigY;
				points[2]+=OrigX;
				points[3]+=OrigY;
			}
			// update origin
			OrigX = points[2];
			OrigY = points[3];

			//calculate the estimated poly
			qcurve2poly(points, Poly[(*nbpoints-1) * 2], Poly[(*nbpoints-1) * 2 + 1], interp_steps, &Poly[*nbpoints * 2]);
			
			// reset control points
			PCtrlX = points[0];
			PCtrlY = points[1];
			// Increase counters
			(*nbpoints)+=interp_steps;
			s += n;
			break;
        case SQCURVETO:
			// read the new points
			n = 2;
			points = readpoints(s,&n);
			if (Mode == RELATIVE)
			{
				// translate to absolute mode
				points[0]+=OrigX;
				points[1]+=OrigY;
			}
			// build complete QCURVETO
			points[3] = points[1];
			points[2] = points[0];
			// calculate first control point form current point and previous ctrl point
			points[0] = Poly[(*nbpoints-1) * 2]		+ (Poly[(*nbpoints-1) * 2]		- PCtrlX);
			points[1] = Poly[(*nbpoints-1) * 2 + 1]	+ (Poly[(*nbpoints-1) * 2 + 1]	- PCtrlY);
			// update origin
			OrigX = points[2];
			OrigY = points[3];

			//calculate the estimated poly
			qcurve2poly(points, Poly[(*nbpoints-1) * 2], Poly[(*nbpoints-1) * 2 + 1], interp_steps, &Poly[*nbpoints * 2]);
			
			// reset control points
			PCtrlX = points[0];
			PCtrlY = points[1];
			// Increase counters
			(*nbpoints)+=interp_steps;
			s += n;
			break;
		}
	}

	// we end without closing the path, we shall return a polyline
	*nbchar = s - s_save;
	if (*nbpoints == 2) *Type = 2;
	else *Type = 0;
	return(Poly);
	
error :
	//error, return NULL path
	if (Poly) free(Poly);
	*nbpoints = 0;
	*nbchar	  = 0;
	return(NULL);
}
