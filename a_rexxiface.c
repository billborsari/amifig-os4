/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_rexxiface.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <proto/rexxsyslib.h>
#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_redraw.h"
#include "u_list.h"
#include "u_undo.h"
#include "a_savebitmap.h"
#include "a_color.h"
#include "a_eventloop.h"
#include "f_load.h"
#include "d_arc.h"
#include "d_box.h"
#include "d_ellipse.h"
#include "d_line.h"
#include "d_picobj.h"
#include "d_spline.h"
#include "d_text.h"
#include "d_regpoly.h"
#include "d_arcbox.h"
#include "a_coords.h"
#include "menu.h"
#include "u_fonts.h"
#include "z_mode.h"
#include "z_edit.h"
#include "a_io.h"
#include "w_msgpanel.h"

#ifdef __amigaos4__
struct Library      *RexxSysBase = NULL;
struct RexxSysIFace *IRexxSys    = NULL;
#else
struct RxsLib  *rexxlib      = NULL;
#endif

 struct MsgPort *rexxport     = NULL;
 char            portname[32] = "AMIFIG.0";
 STRPTR          mess1        = (STRPTR)"a result string";
 int curREXXBaseSystem = 0; // 0 = pixels in 1200 dpi, 1 = metric (cm), 2 = imperial (inch)
 
 #define TOFLOAT(i) atof((char *)args[i])
 #define TOINT(i) atoi((char *)args[i])
 #define GETLONGARG(i) (*((LONG *)args[i]))
 
 float makePixelCoordinate(IPTR *arg)
{
	float value = 0;

	switch(curREXXBaseSystem)
	{
	case 0: 
        value = SCALE_UP( atof((char *)arg)  * display_zoomscale);
		break;
	case 1: 
        value = SCALE_UP( CM_TO_PIX( atof((char *)arg) ) );
		break;
	case 2: 
        value = SCALE_UP( INCH_TO_PIX( atof((char *)arg) ) );
		break;
	}
	//printf("%f\n", value);
	return(value);
}


/* The functions that implement the exprted AREXX commands by wrapping
*  access to the several draw and status functions.
*/

void testfunc(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	printf("testfunc called!\n");
	strcpy((char *)resultstring, (const char *)mess1);
}

void rxSetZoomScale(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	float scale = TOFLOAT(0);

	if(scale < 1)
	{
		strcpy((char *)resultstring, "Zoom scale must be > 0!");
		*errorcode = RC_ERROR;
	}
	else if(scale > 999)
	{
		strcpy((char *)resultstring, "Zoom scale must be < 999!");
		*errorcode = RC_ERROR;
	}
	else	// change the zoom factor
	{
		display_zoomscale=(float)scale/100;

		update_scroller(FALSE);
		redisplay_canvas();
	}
}

void rxSetBaseSystem(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(stricmp((char *)args[0], "dpi") == 0)
	{
		curREXXBaseSystem = 0;
	}
	else
	{
		if(stricmp((char *)args[0], "metric") == 0)
		{
			curREXXBaseSystem = 1;
			appres.INCHES = FALSE;
			cur_gridunit = MM_UNIT;
			strcpy((char *)cur_fig_units, appres.INCHES ? "in" : "cm");
		}
		else
		{
			if(stricmp((char *)args[0], "imperial") == 0)
			{
				curREXXBaseSystem = 2;
				appres.INCHES = TRUE;
				cur_gridunit = appres.gridunit;
				strcpy((char *)cur_fig_units, appres.INCHES ? "in" : "cm");
			}
			else
			{
				sprintf((char *)resultstring, "Unknown unit: %s!", (char *)args[0]);
				*errorcode = RC_ERROR;
				return;
			}
		}
	}
}

void rxClear(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	clearFigure();
}

void rxLoadFile(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	load_file((char *)args[0]);
}

void rxSaveFile(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	save_file((char *)args[0]);
}

void rxExportFile(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int left, top, width, height;

	left   = TOINT(2);
	top    = TOINT(3);
	width  = TOINT(4);
	height = TOINT(5);

	if(stricmp((char *)args[1], "png") == 0)
	{
		write_bitmap((char *)TempName, left, top, width, height, width, height, 0);
		return;
	}
	
	if(stricmp((char *)args[1], "fig2dev") == 0)
	{
		ExportF2D();
		return;
	}
		
	sprintf((char *)resultstring, "Unknown export mode: %s!", (char *)args[1]);
	*errorcode = RC_ERROR;
}

void rxUndo(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	undo();
}

void rxSetDepth(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
    LONG depth;
    
    depth = GETLONGARG(0);
    if(depth < 0 || depth > 999)
	{
		put_msg((char *)_(msg_DepthLimits));
		sprintf((char *)resultstring, "Wrong value for depth: %ld. Value must be from 0 to 999!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
	}
	else
	{
		cur_depth = depth;
        z_refreshAttributes();
	}
}

void rxSetLineWidth(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) > 99)
	{
		sprintf((char *)resultstring, "Wrong value for line width: %ld. Value must be from 0 to 99!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	cur_linewidth = (GETLONGARG(0) == -1)? 1 : GETLONGARG(0);

    z_refreshAttributes();
}

void rxSetLineStyle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
    LONG linestyle;
    
    linestyle = *((LONG *)args[0]);
    
    switch(linestyle)
	{
        case SOLID_LINE:
            cur_linestyle = SOLID_LINE;
            cur_styleval = 0.0;
            put_msg((char *)_(msg_Solid));
            break;
        case DASH_LINE:
            cur_linestyle = DASH_LINE;
            cur_styleval = cur_dashlength;
            put_msg((char *)_(msg_Dashed));
            break;
        case DOTTED_LINE:
            cur_linestyle = DOTTED_LINE;
            cur_styleval = cur_dotgap;
            put_msg((char *)_(msg_Dotted));
            break;
        case DASH_DOT_LINE:
            cur_linestyle = DASH_DOT_LINE;
            cur_styleval = cur_dashlength;
            put_msg((char *)_(msg_Dash1Dot));
            break;
        case DASH_2_DOTS_LINE:
            cur_linestyle = DASH_2_DOTS_LINE;
            cur_styleval = cur_dashlength;
            put_msg((char *)_(msg_Dash2Dot));
            break;
        case DASH_3_DOTS_LINE:
            cur_linestyle = DASH_3_DOTS_LINE;
            cur_styleval = cur_dashlength;
            put_msg((char *)_(msg_Dash3Dot));
            break;
        default:
            sprintf((char *)resultstring, "Unknown line style: %ld!", (long int)*((LONG *)args[0]));
            *errorcode = RC_ERROR;
            break;
	}
    z_refreshAttributes();
}

void rxSetJoinStyle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(*((LONG *)args[0]) > 2)
	{
		sprintf((char *)resultstring, "Unknown join style: %ld!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
	}
	else
	{
		cur_joinstyle = *((LONG *)args[0]);
        z_refreshAttributes();
	}
}

void rxSetCapStyle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(*((LONG *)args[0]) > 2)
	{
		sprintf((char *)resultstring, "Unknown cap style: %ld!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
	}
	else
	{
		cur_capstyle = *((LONG *)args[0]);
        z_refreshAttributes();
	}
}

void rxSetArrowStyle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(*((LONG *)args[0]) > 3)
	{
		sprintf((char *)resultstring, "Unknown arrow type: %ld!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
	}
	else
	{
		BOOL solid;

		if((solid = *((LONG *)args[1])) > 1)
		{
			sprintf((char *)resultstring, "Arrow style can only be 0 or 1!");
			*errorcode = RC_ERROR;
			return;
		}

		if(*((LONG *)args[0]) == 0)
			// simple arrows cannot be filled
			cur_arrowtype=0;
		else
        {
            cur_arrowtype = (*((LONG *)args[0])) * 2 - 1;
            cur_arrowtype += solid;
        }

        z_refreshAttributes();
	}
}

void rxSetAutoArrows(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(*((LONG *)args[0]) > 1)
	{
		sprintf((char *)resultstring, "Argument can only be 0 or 1: %ld!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
		return;
	}

	if(*((LONG *)args[1]) > 1)
	{
		sprintf((char *)resultstring, "Argument can only be 0 or 1: %ld!", (long int)*((LONG *)args[0]));
		*errorcode = RC_ERROR;
		return;
	}

	autoforwardarrow_mode = *((LONG *)args[0]);
	autobackwardarrow_mode = *((LONG *)args[1]);

	// refreshModesGadgets();
    z_mode_refresh();
}

void rxSetEllipseAngle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	cur_elltextangle = TOINT(0)%360;

    z_refreshAttributes();
}

void rxSetFontSize(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) > BIGGESTFONT)
	{
		sprintf((char *)resultstring, "Maximum font size is %d!", BIGGESTFONT);
		*errorcode = RC_ERROR;
		return;
	}

	cur_fontsize = GETLONGARG(0);

    z_refreshAttributes();
}

void rxSetTextAdjustment(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) > 2)
	{
		sprintf((char *)resultstring, "Unknown text adjustment: %ld!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	cur_textjust = GETLONGARG(0);

    z_refreshAttributes();
}

void rxSetRigidTextFlag(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) > 1)
	{
		sprintf((char *)resultstring, "Rigid text flag can only be 0 or 1!");
		*errorcode = RC_ERROR;
		return;
	}

	if(GETLONGARG(0) == 0)
		cur_textflags &= ~RIGID_TEXT;
	else
		cur_textflags |= RIGID_TEXT;

    z_refreshAttributes();
}

void rxSetSpecialTextFlag(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) > 1)
	{
		sprintf((char *)resultstring, "Special text flag can only be 0 or 1!");
		*errorcode = RC_ERROR;
		return;
	}

	if(GETLONGARG(0) == 0)
		cur_textflags &= ~SPECIAL_TEXT;
	else
		cur_textflags |= SPECIAL_TEXT;

    z_refreshAttributes();
}

void rxSetTextAngle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	cur_textangle = TOINT(0)%360;

    z_refreshAttributes();
}

void rxSetPenColor(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) >= NUM_STD_COLS)
	{
		sprintf((char *)resultstring, "Unknown pen color: %ld!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	setPenColor(GETLONGARG(0));
}

void rxSetFillColor(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) >= NUM_STD_COLS)
	{
		sprintf((char *)resultstring, "Unknown fill color: %ld!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	setFillColor(GETLONGARG(0));
}

void rxSetFillShade(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) >= (NUMSHADEPATS+NUMTINTPATS))
	{
		sprintf((char *)resultstring, "Unknown fill shade: %ld!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	setFillStyle(GETLONGARG(0));
}


void rxSetUnfilled(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	setFillStyle(UNFILLED);
}

void rxSetPattern(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	if(GETLONGARG(0) >= NUMPATTERNS)
	{
		sprintf((char *)resultstring, "Unknown pattern: %ld!", (long int)GETLONGARG(0));
		*errorcode = RC_ERROR;
		return;
	}

	setFillStyle((NUMSHADEPATS+NUMTINTPATS) + GETLONGARG(0));
}

void rxDrawCircle(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );
	int radius = makePixelCoordinate( (IPTR *)args[2] );

	circlebyradius_drawing_selected();
	init_circlebyradius_drawing(x, y);
	create_circlebyrad(x+radius, y);
    redisplay_canvas();
}

void rxDrawEllipse(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );
	int radius1 = makePixelCoordinate( (IPTR *)args[2] );
	int radius2 = makePixelCoordinate( (IPTR *)args[3] );

	ellipsebyradius_drawing_selected();
	init_ellipsebyradius_drawing(x, y);
	create_ellipsebyrad(x+radius1, y+radius2);
    redisplay_canvas();
}

void rxStartClosedSpline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_CLOSED_APPROX_SPLINE;

	spline_drawing_selected();
	init_spline_drawing(x, y);

//	refreshToolsGadgets();
}

void rxStartSpline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_APPROX_SPLINE;

	spline_drawing_selected();
	init_spline_drawing(x, y);

//	refreshToolsGadgets();
}

void rxStartClosedInterpSpline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_CLOSED_INTERP_SPLINE;

	spline_drawing_selected();
	init_spline_drawing(x, y);

//	refreshToolsGadgets();
}

void rxStartInterpSpline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_INTERP_SPLINE;

	spline_drawing_selected();
	init_spline_drawing(x, y);

//	refreshToolsGadgets();
}

void rxAddSplineSegment(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	get_intermediatepoint(x, y, 0);
}

void rxEndSpline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	if(num_point < min_num_points)
	{
		sprintf((char *)resultstring, "Not enough points for this type of spline!\n\
		        (only %d points given where %d points are needed)", num_point, min_num_points);
		*errorcode = RC_ERROR;
		cancel_line_drawing();
	}
	else
		create_splineobject(x, y);
    redisplay_canvas();
}

void rxDrawRectBox(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x1 = makePixelCoordinate( (IPTR *)args[0] );
	int y1 = makePixelCoordinate( (IPTR *)args[1] );
	int x2 = makePixelCoordinate( (IPTR *)args[2] );
	int y2 = makePixelCoordinate( (IPTR *)args[3] );
	/*
		int x1 = SCALE_UP( TOFLOAT(0) );
		int y1 = SCALE_UP( TOFLOAT(1) );
		int x2 = SCALE_UP( TOFLOAT(2) );
		int y2 = SCALE_UP( TOFLOAT(3) );
	*/
	box_drawing_selected();
	init_box_drawing(x1, y1);
	create_boxobject(x2, y2);
    redisplay_canvas();
}

void rxStartPolyline(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_POLYLINE;

	line_drawing_selected();
	init_trace_drawing(x, y);

//	refreshToolsGadgets();
}

void rxStartPolygon(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	cur_mode = F_POLYGON;

	line_drawing_selected();
	init_trace_drawing(x, y);

//	refreshToolsGadgets();
}

void rxAddLineSegment(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	//	printf("rxAddLineSegment(%d, %d) called!\n", x, y);

	get_intermediatepoint(x, y, 0);
}

void rxEndLine(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );

	//	printf("rxEndLine called!\n");

	create_lineobject(x, y);
    redisplay_canvas();
}

void rxDrawText(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x = makePixelCoordinate( (IPTR *)args[0] );
	int y = makePixelCoordinate( (IPTR *)args[1] );
	F_text *newtext;

	text_drawing_selected();
	newtext = new_text(x, y, (char *)args[2]);
	add_text(newtext);
	redisplay_canvas();

//	refreshToolsGadgets();
}

void rxDrawArc(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x1 = makePixelCoordinate( (IPTR *)args[0] );
	int y1 = makePixelCoordinate( (IPTR *)args[1] );
	int x2 = makePixelCoordinate( (IPTR *)args[2] );
	int y2 = makePixelCoordinate( (IPTR *)args[3] );
	int x3 = makePixelCoordinate( (IPTR *)args[4] );
	int y3 = makePixelCoordinate( (IPTR *)args[5] );

	cur_arctype = T_OPEN_ARC;

	arc_drawing_selected();
	init_arc_drawing(x1, y1);
	get_arcpoint(x2, y2);
	create_arcobject(x3, y3);
    redisplay_canvas();
}

void rxDrawClosedArc(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x1 = makePixelCoordinate( (IPTR *)args[0] );
	int y1 = makePixelCoordinate( (IPTR *)args[1] );
	int x2 = makePixelCoordinate( (IPTR *)args[2] );
	int y2 = makePixelCoordinate( (IPTR *)args[3] );
	int x3 = makePixelCoordinate( (IPTR *)args[4] );
	int y3 = makePixelCoordinate( (IPTR *)args[5] );

	cur_arctype = T_PIE_WEDGE_ARC;

	arc_drawing_selected();
	init_arc_drawing(x1, y1);
	get_arcpoint(x2, y2);
	create_arcobject(x3, y3);
    redisplay_canvas();
}

void rxDrawPicture(IPTR *args, LONG *errorcode, STRPTR resultstring)
{
	int x1 = makePixelCoordinate( (IPTR *)args[0] );
	int y1 = makePixelCoordinate( (IPTR *)args[1] );
	int x2 = makePixelCoordinate( (IPTR *)args[2] );
	int y2 = makePixelCoordinate( (IPTR *)args[3] );

	picobj_drawing_selected();
	init_picobj_drawing2(x1, y1);
	if(create_picobj2(x2, y2, (char *)args[4]) == -2)
	{
		sprintf((char *)resultstring, "File %s does not exist!", (STRPTR)args[4]);
		*errorcode = RC_ERROR;
	}
    redisplay_canvas();
}

struct RexxCommand
{
	STRPTR rc_Name;
	STRPTR rc_Template;
	void (*rc_Function)(IPTR *args, LONG *errorcode, STRPTR resultstring);
};

//#define NUM_REXX_COMMANDS 2

struct RexxCommand rexxCommands[] =
    {
	    {(STRPTR)"testfunc",		        (STRPTR)"",			                            testfunc},
	    {(STRPTR)"SetZoomScale", 	        (STRPTR)"ARG/A",	                            rxSetZoomScale},
	    {(STRPTR)"SetBaseSystem",	        (STRPTR)"ARG/A",	                            rxSetBaseSystem},
//  	{(STRPTR)"SetBaseUnit",	            (STRPTR)"ARG/A",		                        rxSetBaseUnit},
	    {(STRPTR)"Clear",                   (STRPTR)"NAME",	                                rxClear},
	    {(STRPTR)"LoadFile",                (STRPTR)"NAME/A",	                            rxLoadFile},
	    {(STRPTR)"SaveFile",                (STRPTR)"NAME/A",	                            rxSaveFile},
	    {(STRPTR)"ExportFile",              (STRPTR)"NAME/A, MODE/A, LEFT/A, TOP/A, WIDTH/A, HEIGHT/A",	rxExportFile},
	    {(STRPTR)"Undo",                    (STRPTR)"",                                     rxUndo},
	    {(STRPTR)"SetDepth", 		        (STRPTR)"DEPTH/N/A",	                        rxSetDepth},
	    {(STRPTR)"SetLineWidth", 	        (STRPTR)"WIDTH/N/A",	                        rxSetLineWidth},
	    {(STRPTR)"SetLineStyle", 	        (STRPTR)"STYLE/N/A",	                        rxSetLineStyle},
	    {(STRPTR)"SetJoinStyle", 	        (STRPTR)"STYLE/N/A",	                        rxSetJoinStyle},
	    {(STRPTR)"SetCapStyle", 	        (STRPTR)"STYLE/N/A",	                        rxSetCapStyle},
	    {(STRPTR)"SetArrowStyle", 	        (STRPTR)"TYPE/N/A, FILLED/N/A",	                rxSetArrowStyle},
//	    {(STRPTR)"SetArrowStyle", 	        (STRPTR)"ARG/N/A, SOLID/S/F",	                rxSetArrowStyle},
	    {(STRPTR)"SetAutoArrows", 	        (STRPTR)"FORWARD/N/A, BACKWARD/N/A",            rxSetAutoArrows},
	    {(STRPTR)"SetEllipseAngle",         (STRPTR)"ANGLE/A",	                            rxSetEllipseAngle},
	    {(STRPTR)"SetDisplayFont",          (STRPTR)"SIZE/N/A",		                        rxSetFontSize},
	    {(STRPTR)"SetTextAdjustment",       (STRPTR)"ARG/N/A",                              rxSetTextAdjustment},
	    {(STRPTR)"SetRigidTextFlag",        (STRPTR)"ARG/N/A",	                            rxSetRigidTextFlag},
	    {(STRPTR)"SetSpecialTextFlag",      (STRPTR)"ARG/N/A",	                            rxSetSpecialTextFlag},
	    {(STRPTR)"SetTextAngle",            (STRPTR)"ANGLE/A",		                        rxSetTextAngle},
	    {(STRPTR)"SetPenColor",             (STRPTR)"COLOR/N/A",	                        rxSetPenColor},
	    {(STRPTR)"SetFillColor",            (STRPTR)"COLOR/N/A",	                        rxSetFillColor},
	    {(STRPTR)"SetFillShade",            (STRPTR)"SHADE/N/A",	                        rxSetFillShade},
	    {(STRPTR)"SetUnfilled",             (STRPTR)"",	                                    rxSetUnfilled},
	    {(STRPTR)"SetPattern",              (STRPTR)"PATTERN/N/A",	                        rxSetPattern},
	    {(STRPTR)"DrawCircle",		        (STRPTR)"X/A, Y/A, RADIUS/A",	                rxDrawCircle},
	    {(STRPTR)"DrawEllipse",		        (STRPTR)"X/A, Y/A, RADIUS1/A, RADIUS2/A",	    rxDrawEllipse},
	    {(STRPTR)"StartClosedSpline",		(STRPTR)"X/A, Y/A",	                            rxStartClosedSpline},
	    {(STRPTR)"StartSpline",				(STRPTR)"X/A, Y/A",	                            rxStartSpline},
	    {(STRPTR)"StartClosedInterpSpline",	(STRPTR)"X/A, Y/A",	                            rxStartClosedInterpSpline},
	    {(STRPTR)"StartInterpSpline",		(STRPTR)"X/A, Y/A",	                            rxStartInterpSpline},
	    {(STRPTR)"AddSplineSegment",		(STRPTR)"X/A, Y/A",	                            rxAddSplineSegment},
	    {(STRPTR)"EndSpline",				(STRPTR)"X/A, Y/A",	                            rxEndSpline},
	    {(STRPTR)"DrawRectBox", 	        (STRPTR)"X1/A, Y1/A, X2/A, Y2/A",	            rxDrawRectBox},
	    {(STRPTR)"StartPolyline",	        (STRPTR)"X/A, Y/A",	                            rxStartPolyline},
	    {(STRPTR)"StartPolygon",	        (STRPTR)"X/A, Y/A",	                            rxStartPolygon},
	    {(STRPTR)"AddLineSegment",	        (STRPTR)"X/A, Y/A",	                            rxAddLineSegment},
	    {(STRPTR)"EndLine",			        (STRPTR)"X/A, Y/A",	                            rxEndLine},
	    {(STRPTR)"DrawText",		        (STRPTR)"X/A, Y/A, TEXT/A",	                    rxDrawText},
	    {(STRPTR)"DrawArc",			        (STRPTR)"X1/A, Y1/A, X2/A, Y2/A, X3/A, Y3/A",	rxDrawArc},
	    {(STRPTR)"DrawClosedArc",	        (STRPTR)"X1/A, Y1/A, X2/A, Y2/A, X3/A, Y3/A",	rxDrawClosedArc},
	    {(STRPTR)"DrawPicture",	            (STRPTR)"X1/A, Y1/A, X2/A, Y2/A, FILE/A",	    rxDrawPicture},
	    {(STRPTR)NULL,                      (STRPTR)NULL,                                   NULL}
    };

#define RESULTLEN 512
#define BASENAME "AmiFIG"

static char resultstring[RESULTLEN];

BOOL initRexxIFace(void)
{
	int number=0;
#ifdef __amigaos4__
	if(!(RexxSysBase = OpenLibrary("rexxsyslib.library", 0)))
    {
		printf("Could not open rexxsyslib.library\n");
		return(FALSE);
    }
    if(!(IRexxSys = (struct RexxSysIFace *)GetInterface(RexxSysBase, "main", 1, NULL)))
    {
        printf("Could not get RexxSys interface\n");
        CloseLibrary(RexxSysBase);
        RexxSysBase = NULL;
        return(FALSE);
    }
#else
	if(!(rexxlib = (struct RxsLib *)OpenLibrary((CONST_STRPTR)RXSNAME, 0)))
	{
		printf("Could not open \"%s\" version %d or higher\n",RXSNAME,0);
		return(FALSE);
	}
#endif
	
	Forbid();

	// search for other AmiFIG instances
	while(FindPort((CONST_STRPTR)portname))
		sprintf(portname, "AMIFIG.%d", ++number);

#ifdef __amigaos4__
    rexxport = (struct MsgPort *)AllocSysObjectTags(ASOT_PORT,
        ASOPORT_Name,   portname,
        ASOPORT_Public, TRUE,
        ASOPORT_Pri,    0,
        TAG_DONE);
#else
	rexxport = (struct MsgPort*)CreatePort((STRPTR)portname, 0);
#endif

	Permit();
    
	return(rexxport != NULL);
}

void closeRexxIFace(void)
{
	if(rexxport)
    {
#ifdef __amigaos4__
        FreeSysObject(ASOT_PORT, rexxport);
#else
		DeletePort(rexxport);
#endif
    }
    rexxport = NULL;

#ifdef __amigaos4__
    if (IRexxSys) DropInterface((struct Interface *)IRexxSys);
    if (RexxSysBase) CloseLibrary(RexxSysBase);
    IRexxSys = NULL;
    RexxSysBase = NULL;
#else
	if(rexxlib)
		CloseLibrary((struct Library *)rexxlib);
#endif
}

ULONG getRexxSigBit(void)
{
	if(rexxport == NULL)
		return(0);

	return(rexxport->mp_SigBit);
}

/* This function handles incoming requests and calls the above wrapper functions.
*/
void handleRexxMessage(ULONG SignalSet)
{
	struct RexxMsg *message;

	while((message = (struct RexxMsg *)GetMsg(rexxport)))
	{
		// is this an AREXX-message?
		if(IsRexxMsg(message))
		{
			STRPTR arg;
			LONG errorcode = RC_OK;
			IPTR opts[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
			struct RDArgs *argsptr;
			struct RexxCommand *command;
            STRPTR cmdstr = (STRPTR)ARG0(message);

            resultstring[0] = '\0';
            if (!cmdstr || *cmdstr == '\0') 
            {
                errorcode = RC_ERROR;
                goto finish_msg;
            }

			//printf("arg0=%s\n", ARG0(message));
			// look for the command to execute
			for(command = rexxCommands; command->rc_Name != NULL; command++)
			{
                size_t cmdlen = strlen((const char *)command->rc_Name);
				// Check for exact match or match followed by space/newline
				if(strnicmp((const char *)command->rc_Name, (const char *)cmdstr, cmdlen) == 0)
				{
                    if (cmdstr[cmdlen] != '\0' && cmdstr[cmdlen] != ' ' && cmdstr[cmdlen] != '\n')
                        continue;

					// check parameters
					if((argsptr = AllocDosObjectTags(DOS_RDARGS, TAG_DONE)))
					{
						if((arg = AllocVec(strlen((const char *)cmdstr) + 2, MEMF_CLEAR)))
						{
							strcpy((char *)arg, (const char *)ARG0(message));
							strcat((char *)arg, "\n");
							argsptr->RDA_Source.CS_Buffer = arg + strlen((const char *)command->rc_Name);
							argsptr->RDA_Source.CS_Length = strlen((const char *)argsptr->RDA_Source.CS_Buffer);
							argsptr->RDA_Source.CS_CurChr = 0;

							if(ReadArgs(command->rc_Template, (IPTR *)opts, argsptr) != NULL)
								(*command->rc_Function)(opts, &errorcode, (STRPTR)resultstring);
							else
							{
								errorcode = RC_ERROR;
								Fault(IoErr(), (CONST_STRPTR)BASENAME, (STRPTR)resultstring, RESULTLEN);
							}

							FreeArgs(argsptr);
							FreeVec(arg);
						}
						else
							errorcode = RC_FATAL;
						FreeDosObject(DOS_RDARGS, argsptr);
					}
					else
					{
						printf("AllocDosObjectTags failed!\n");
						errorcode = RC_FATAL;
					}
					break;
				}
			}

			// command not found?
			if(command->rc_Name == NULL)
			{
				snprintf(resultstring, RESULTLEN, "Unknown command %.256s\n", cmdstr);
				errorcode = RC_FATAL;
			}

finish_msg:
			message->rm_Result1 = errorcode;
			message->rm_Result2 = 0;

			if(errorcode == RC_OK)
			{
				if(message->rm_Action & RXFF_RESULT)
				{
					message->rm_Result2 = (IPTR)CreateArgstring(resultstring, strlen(resultstring));
				}
			}
			else
			{
				//printf("setting error code\n");
				message->rm_Result2 = errorcode;
#ifdef __AROS__
				SetRexxVar((struct RexxMsg *)message, (CONST_STRPTR)"AMIFIG.LASTERROR", resultstring, strlen(resultstring));
#elif defined(__amigaos4__)
                SetRexxVar((struct RexxMsg *)message, (CONST_STRPTR)"AMIFIG.LASTERROR", resultstring, strlen(resultstring));
#else
                #warning !! need to deal with SetRexxvar() for os4!
#endif
			}
		}

		ReplyMsg((struct Message *)message);
	}
}
