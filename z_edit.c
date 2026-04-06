/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_edit.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/*----------------------------------------------------------------------------*/
/*     Includes & Defines                                                     */
/*----------------------------------------------------------------------------*/
#include "fig.h"
#include <libraries/mui.h>
#include <proto/muimaster.h>

#include "resources.h"
#include <exec/memory.h>
#include <proto/exec.h>
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "d_arc.h"
#include "d_box.h"
#include "d_ellipse.h"
#include "d_line.h"
#include "d_picobj.h"
#include "d_spline.h"
#include "d_text.h"
#include "d_regpoly.h"
#include "d_arcbox.h"
#include "e_align.h"
#include "e_arrow.h"
#include "e_addpt.h"
#include "e_break.h"
#include "e_compound.h"
#include "e_convert.h"
#include "e_copy.h"
#include "e_move.h"
#include "e_delete.h"
#include "e_deletept.h"
#include "e_flip.h"
#include "e_glue.h"
#include "e_movept.h"
#include "e_measure.h"
#include "e_rotate.h"
#include "e_scale.h"
#include "e_update.h"
#include "e_chop.h"
#include "e_tangent.h"
#include "e_editsfactor.h"
#include "e_changedepth.h"
#include "a_patterns.h"
#include "a_color.h"
#include "z_fig.h"
#include "u_markers.h"
#include "w_msgpanel.h"
#include "mcc_graddisp.h"
#include "mcc_fontdisp.h"
#include "mcc_arrowslider.h"
#include "z_color.h"
#include "mcc_imagedisp.h"

#define COLORCELLSIZE 12

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFP3(void, DoEditMutex,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoAlignMutex,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetLineStyle,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetFillStyle,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetFillPattern,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetFontStyle,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, OpenFontConf,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoPenColorPal,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoFilColorPal,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoFMode,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoGradient,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, SetDLFont,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoEditMutex(struct Hook *h, APTR Object, ULONG *Arg);
void DoAlignMutex(struct Hook *h, APTR Object, ULONG *Arg);
void SetLineStyle(struct Hook *h, APTR Object, ULONG *Arg);
void SetFillStyle(struct Hook *h, APTR Object, ULONG *Arg);
void SetFillPattern(struct Hook *h, APTR Object, ULONG *Arg);
void SetFontStyle(struct Hook *h, APTR Object, ULONG *Arg);
void OpenFontConf(struct Hook *h, APTR Object, ULONG *Arg);
void DoPenColorPal(struct Hook *h, APTR Object, ULONG *Arg);
void DoFilColorPal(struct Hook *h, APTR Object, ULONG *Arg);
void DoFMode(struct Hook *h, APTR Object, ULONG *Arg);
void DoGradient(struct Hook *h, APTR Object, ULONG *Arg);
void SetDLFont(struct Hook *h, APTR Object, ULONG *Arg);
#endif    
    
void UpdateFShades(void);

typedef struct action_button
{
	APTR		obj;
	UBYTE*		image;
    UBYTE       key;
	UBYTE*		msg;
	LONG		action;
	LONG		mask;
	void		(*function)(void);
} Action_Button;

typedef struct align_button
{
	APTR		obj;
	UBYTE*		image;
    UBYTE       key;
	UBYTE*		msg;
	LONG		alignmode;
} Align_Button;


/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/

static char *regtitles[] = {NULL, NULL, NULL, NULL, NULL, NULL };
static char *CYC_LINESTYLE_strings[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static char *CYC_JOINSTYLE_strings[] = { NULL, NULL, NULL, NULL };
static char *CYC_CAPSTYLE_strings[] = { NULL, NULL, NULL, NULL };
static char *CYC_TXTJUST_strings[] = { NULL, NULL, NULL, NULL };
static char *CYC_ARCTYPE_strings[] = { NULL, NULL, NULL };
static char *CYC_ARROWTYPE_strings[NUM_ARROW_TYPES] = {	NULL, NULL, NULL, NULL, NULL, NULL,
														NULL, NULL, NULL, NULL, NULL, NULL,
														NULL, NULL, NULL, NULL, NULL, NULL,
														NULL, NULL, NULL, NULL, NULL, NULL,
														NULL, NULL, NULL, NULL, NULL, NULL};
static char *CYC_FMODE_strings[] = {NULL, NULL, NULL, NULL, NULL};
    

Action_Button Buttons[] =
{
    /* Draw primitives */
	{NULL, (UBYTE *)"circle_ray.png"          , '\0', (UBYTE *)NULL, F_CIRCLE_BY_RAD       , M_NONE                          , circlebyradius_drawing_selected   },
	{NULL, (UBYTE *)"circle_diameter.png"     , '\0', (UBYTE *)NULL, F_CIRCLE_BY_DIA       , M_NONE                          , circlebydiameter_drawing_selected },
	{NULL, (UBYTE *)"ellipse.png"             , '\0', (UBYTE *)NULL, F_ELLIPSE_BY_RAD      , M_NONE                          , ellipsebyradius_drawing_selected  },
	{NULL, (UBYTE *)"ellipse_diameter.png"    , '\0', (UBYTE *)NULL, F_ELLIPSE_BY_DIA      , M_NONE                          , ellipsebydiameter_drawing_selected},
	{NULL, (UBYTE *)"closed_spline.png"       , '\0', (UBYTE *)NULL, F_CLOSED_APPROX_SPLINE, M_NONE                          , spline_drawing_selected           },
	{NULL, (UBYTE *)"open_spline.png"         , '\0', (UBYTE *)NULL, F_APPROX_SPLINE       , M_NONE                          , spline_drawing_selected           },
	{NULL, (UBYTE *)"closed_spline_points.png", '\0', (UBYTE *)NULL, F_CLOSED_INTERP_SPLINE, M_NONE                          , spline_drawing_selected           },
	{NULL, (UBYTE *)"open_spline_points.png"  , '\0', (UBYTE *)NULL, F_INTERP_SPLINE       , M_NONE                          , spline_drawing_selected           },
	{NULL, (UBYTE *)"polyline.png"            , '\0', (UBYTE *)NULL, F_POLYLINE            , M_NONE                          , line_drawing_selected             },
	{NULL, (UBYTE *)"closed_poly.png"         , '\0', (UBYTE *)NULL, F_POLYGON             , M_NONE                          , line_drawing_selected             },
	{NULL, (UBYTE *)"rectangle.png"           , '\0', (UBYTE *)NULL, F_BOX                 , M_NONE                          , box_drawing_selected              },
	{NULL, (UBYTE *)"roundbox.png"            , '\0', (UBYTE *)NULL, F_ARCBOX              , M_NONE                          , arcbox_drawing_selected           },
	{NULL, (UBYTE *)"text.png"                , '\0', (UBYTE *)NULL, F_TEXT                , M_NONE                          , text_drawing_selected             },
	{NULL, (UBYTE *)"three_point_arc.png"     , '\0', (UBYTE *)NULL, F_CIRCULAR_ARC        , M_NONE                          , arc_drawing_selected              },
	{NULL, (UBYTE *)"picture.png"             , '\0', (UBYTE *)NULL, F_PICOBJ              , M_POLYLINE_BOX                  , picobj_drawing_selected           },
	{NULL, (UBYTE *)"regpoly.png"             , '\0', (UBYTE *)NULL, F_REGPOLY             , M_NONE                          , regpoly_drawing_selected          },
    /* Object edition */
	{NULL, (UBYTE *)"move.png"                , '\0', (UBYTE *)NULL, F_MOVE                , M_ALL                           , move_selected                     },
	{NULL, (UBYTE *)"duplicate.png"           , '\0', (UBYTE *)NULL, F_COPY                , M_ALL                           , copy_selected                     },
	{NULL, (UBYTE *)"delete.png"              , '\0', (UBYTE *)NULL, F_DELETE              , M_ALL                           , delete_selected                   },
	{NULL, (UBYTE *)"scale.png"               , '\0', (UBYTE *)NULL, F_SCALE               , M_NO_TXT                        , scale_selected                    },
	{NULL, (UBYTE *)"modify_point.png"        , '\0', (UBYTE *)NULL, F_MOVE_POINT          , M_NO_TXT                        , move_point_selected               },
	{NULL, (UBYTE *)"add_point.png"           , '\0', (UBYTE *)NULL, F_ADD_POINT           , M_VARPTS_OBJECT                 , point_adding_selected             },
	{NULL, (UBYTE *)"remove_point.png"        , '\0', (UBYTE *)NULL, F_DELETE_POINT        , M_VARPTS_OBJECT                 , delete_point_selected             },
	{NULL, (UBYTE *)"spline2poly.png"         , '\0', (UBYTE *)NULL, F_CONVERT             , M_VARPTS_OBJECT | M_POLYLINE_BOX, convert_selected                  },
	{NULL, (UBYTE *)"group.png"               , '\0', (UBYTE *)NULL, F_GLUE                , M_ALL                           , compound_selected                 },
	{NULL, (UBYTE *)"ungroup.png"             , '\0', (UBYTE *)NULL, F_BREAK               , M_COMPOUND                      , break_selected                    },
	{NULL, (UBYTE *)"group_open.png"          , '\0', (UBYTE *)NULL, F_ENTER_COMP          , M_COMPOUND                      , open_compound_selected            },
	{NULL, (UBYTE *)"group.png"               , '\0', (UBYTE *)NULL, F_EXIT_COMP           , M_COMPOUND                      , close_compound                    },
	{NULL, (UBYTE *)"flip_vert.png"           , '\0', (UBYTE *)NULL, F_FLIP                , M_NO_TXT                        , flip_ud_selected                  },
	{NULL, (UBYTE *)"flip_horiz.png"          , '\0', (UBYTE *)NULL, F_FLIP                , M_NO_TXT                        , flip_lr_selected                  },
	{NULL, (UBYTE *)"rotate_ccw.png"          , '\0', (UBYTE *)NULL, F_ROTATE              , M_ALL                           , rotate_ccw_selected               },
	{NULL, (UBYTE *)"rotate_cw.png"           , '\0', (UBYTE *)NULL, F_ROTATE              , M_ALL                           , rotate_cw_selected                },	
	{NULL, (UBYTE *)"align.png"               , '\0', (UBYTE *)NULL, F_ALIGN               , M_ALL                           , align_selected                    },
	{NULL, (UBYTE *)"arrow.png"               , '\0', (UBYTE *)NULL, F_ADD_ARROW_HEAD      , M_OPEN_OBJECT                   , arrow_head_selected               },
	{NULL, (UBYTE *)"measure_length.png"      , '\0', (UBYTE *)NULL, F_LENMEAS             , M_LENMEAS_OBJECT                , lenmeas_selected                  },	
	{NULL, (UBYTE *)"measure_angle.png"       , '\0', (UBYTE *)NULL, F_ANGLEMEAS           , M_ANGLEMEAS_OBJECT              , anglemeas_selected                },	
	{NULL, (UBYTE *)"measure_area.png"        , '\0', (UBYTE *)NULL, F_AREAMEAS            , M_AREAMEAS_OBJECT               , areameas_selected                 },	
	{NULL, (UBYTE *)"chop.png"                , '\0', (UBYTE *)NULL, F_CHOP                , M_POLYLINE | M_ARC |M_ELLIPSE   , chop_selected	                   },	
	{NULL, (UBYTE *)"tangent_normal.png"      , '\0', (UBYTE *)NULL, F_TANGENT             , M_TANGENT_OBJECT                , tangent_selected                  },
	{NULL, (UBYTE *)"edit_sfactor.png"        , '\0', (UBYTE *)NULL, F_EDIT                , M_SPLINE                        , editsfactor_selected              },	
	{NULL, (UBYTE *)"change_depth.png"        , '\0', (UBYTE *)NULL, F_CHANGE_DEPTH        , M_ALL                           , changedepth_selected              },	
	{NULL, (UBYTE *)"get_format.png"          , '\0', (UBYTE *)NULL, F_GET                 , M_OBJECT                        , get_selected                      },
	{NULL, (UBYTE *)"paste_format.png"        , '\0', (UBYTE *)NULL, F_UPDATE              , M_OBJECT                        , update_selected                   },	
};
#define			NUM_BUTTONS	(sizeof(Buttons) / sizeof(Action_Button))
APTR	ButtonGRP;

Align_Button HAlignButtons[] =
{
	{NULL, (UBYTE *)"align_none.png"   , '\0', (UBYTE *)"", ALIGN_NONE},
	{NULL, (UBYTE *)"align_left.png"   , '\0', (UBYTE *)"", ALIGN_LEFT},
	{NULL, (UBYTE *)"align_hcenter.png", '\0', (UBYTE *)"", ALIGN_CENTER},
	{NULL, (UBYTE *)"align_right.png"  , '\0', (UBYTE *)"", ALIGN_RIGHT},
};
Align_Button VAlignButtons[] =
{
	{NULL, (UBYTE *)"align_none.png"   , '\0', (UBYTE *)"", ALIGN_NONE},
	{NULL, (UBYTE *)"align_top.png"    , '\0', (UBYTE *)"", ALIGN_TOP},
	{NULL, (UBYTE *)"align_vcenter.png", '\0', (UBYTE *)"", ALIGN_CENTER},
	{NULL, (UBYTE *)"align_bottom.png" , '\0', (UBYTE *)"", ALIGN_BOTTOM},
};
APTR    AlignGRP;

/* rotation angle definition */
APTR    RotAngleGRP, NUM_ROTANGLE;

/* Number of regular polygon sides */
APTR	NumRegPolySidesGRP, NUM_NUMSIDES; 

/* Rounded box curve */
APTR	BoxCurveGRP, NUM_BOXRAD;

/* Ellipse initial Angle */
APTR	EllAngleGRP, NUM_ELLIANGLE;

/* ARC type */
APTR    ArcTypeGRP, CYC_ARCTYPE;

/* Depth group */
APTR    DepthGRP;

/* General Panel */
APTR    NUM_DEPTH;
APTR    NUM_LWIDTH, CYC_LINESTYLE, NUM_DOPGAP, NUM_DASHLEN, CYC_JOINSTYLE, CYC_CAPSTYLE, CYC_ARROWTYPE;
/* Pen Panel */
APTR    VG_PENPAL, BUT_PENCOL[NUM_STD_COLS + MAX_USR_COLS], PLINE_BUT[(NUM_STD_COLS + MAX_USR_COLS)/16];
APTR	BUT_PEN_EDITUSRCOL;
/* Fill Panel */
APTR    SG_FILPAL, VG_FILPAL, BUT_FILCOL[NUM_STD_COLS + MAX_USR_COLS], FLINE_BUT[(NUM_STD_COLS + MAX_USR_COLS)/16];
APTR	BUT_FSHADES[NUMSHADES], BUT_FPATTERNS[NUMPATTERNS], VG_SHADES, VG_PATTERNS;
APTR	CYC_FMODE, BUT_FIL_EDITUSRCOL, DUM_PALETTE;
APTR	VG_GRADIENT, NUM_GRADIENT, GRD_PREVIEW;
/* Text Panel */
APTR    LST_TXTFONT, NUM_FONTSIZE, NUM_FONTANGLE, BUT_FONTCONF, CYC_TXTJUST, CHK_TXTRIGID, CHK_TXTSPECIAL, FNT_PREVIEW;
/* Dimension lines Panel */
APTR	PAN_DIM;
APTR	NUM_DL_THICK, CYC_DL_STYLE, CYC_DL_COLOR, CYC_DL_LARROW, CYC_DL_RARROW;
APTR	CHK_DL_TICKS, NUM_DL_TICKTHICK, NUM_DL_BOXTHICK, CYC_DL_BOXCOL, CYC_DL_TXTCOL, CYC_DL_FONT, NUM_DL_FONTSIZE;
APTR	CHK_DL_FIXED, NUM_DL_PRECISION;
/* Update Panel */
APTR	CHK_UPD[UPD_Max];
APTR	BUT_UPD_ALL, BUT_UPD_NONE;
/* Mouse Button functions */
APTR    TEXT_L_MOUSE, TEXT_M_MOUSE, TEXT_R_MOUSE, TEXT_SL_MOUSE, TEXT_SM_MOUSE, TEXT_SR_MOUSE;
APTR    FilPanel;

/*----------------------------------------------------------------------------*/
/*     MakeButtonGrp - creates the main button group                          */
/*----------------------------------------------------------------------------*/
APTR MakeButtonGrp(void)
{
	int i;
    /* Create image buttons */
    i = 0;
    Buttons[i++].msg = (UBYTE *)_(msg_CircleRadius);
    Buttons[i++].msg = (UBYTE *)_(msg_CircleDiameter);
    Buttons[i++].msg = (UBYTE *)_(msg_EllipseRadius);
    Buttons[i++].msg = (UBYTE *)_(msg_EllipseDiameter);
    Buttons[i++].msg = (UBYTE *)_(msg_ClosedSpline);
    Buttons[i++].msg = (UBYTE *)_(msg_Spline);
    Buttons[i++].msg = (UBYTE *)_(msg_ClosedInterpSpline);
    Buttons[i++].msg = (UBYTE *)_(msg_InterpSpline);
    Buttons[i++].msg = (UBYTE *)_(msg_PolyLine);
    Buttons[i++].msg = (UBYTE *)_(msg_Polygon);
    Buttons[i++].msg = (UBYTE *)_(msg_RectBox);
    Buttons[i++].msg = (UBYTE *)_(msg_ArcBoxSelected);
    Buttons[i++].msg = (UBYTE *)_(msg_TextInput);
    Buttons[i++].msg = (UBYTE *)_(msg_Arc);
    Buttons[i++].msg = (UBYTE *)_(msg_Picture);
    Buttons[i++].msg = (UBYTE *)_(msg_RegpolySelected);
    Buttons[i++].msg = (UBYTE *)_(msg_moveobjects);
    Buttons[i++].msg = (UBYTE *)_(msg_CopyObjects);
    Buttons[i++].msg = (UBYTE *)_(msg_DeleteObjects);
    Buttons[i++].msg = (UBYTE *)_(msg_ScaleSelected);
    Buttons[i++].msg = (UBYTE *)_(msg_movepoints);
    Buttons[i++].msg = (UBYTE *)_(msg_AddPoints);
    Buttons[i++].msg = (UBYTE *)_(msg_DeletePoints);
    Buttons[i++].msg = (UBYTE *)_(msg_ConvertLine2Spline);
    Buttons[i++].msg = (UBYTE *)_(msg_CompoundCreate);
    Buttons[i++].msg = (UBYTE *)_(msg_CompoundBreak);
    Buttons[i++].msg = (UBYTE *)_(msg_CompoundOpen);
    Buttons[i++].msg = (UBYTE *)_(msg_CloseCompound);
    Buttons[i++].msg = (UBYTE *)_(msg_FlipHoriz);
    Buttons[i++].msg = (UBYTE *)_(msg_FlipVert);
    Buttons[i++].msg = (UBYTE *)_(msg_RotateObjectCCW);
    Buttons[i++].msg = (UBYTE *)_(msg_RotateObjectCW);
    Buttons[i++].msg = (UBYTE *)_(msg_Align);
    Buttons[i++].msg = (UBYTE *)_(msg_ArrowHead);
    Buttons[i++].msg = (UBYTE *)_(msg_MeasLength);
	Buttons[i++].msg = (UBYTE *)_(msg_MeasAngle);
	Buttons[i++].msg = (UBYTE *)_(msg_MeasArea); 
	Buttons[i++].msg = (UBYTE *)_(msg_ChopObject); 
	Buttons[i++].msg = (UBYTE *)_(msg_AddTangentOrNormal);
	Buttons[i++].msg = (UBYTE *)_(msg_SFactorEdit);    
	Buttons[i++].msg = (UBYTE *)_(msg_Front_Back);    
    Buttons[i++].msg = (UBYTE *)_(msg_GetAttributes);
    Buttons[i++].msg = (UBYTE *)_(msg_UpdateAttributes);
	
    for (i=0; i < NUM_BUTTONS; i++)
    {
        Buttons[i].obj = MakeImageToggle(Buttons[i].image , Buttons[i].key, Buttons[i].msg);
    }
	
	return(VGroup,
		Child, HGroup,
			Child, RectangleObject, MUIA_Weight, 0, End,
			Child, GroupObject,
				MUIA_Group_Columns, 8,
				MUIA_Group_Spacing, 0,
				Child, Buttons[ 0].obj,
				Child, Buttons[ 1].obj,
				Child, Buttons[ 2].obj,
				Child, Buttons[ 3].obj,
				Child, Buttons[ 4].obj,
				Child, Buttons[ 5].obj,
				Child, Buttons[ 6].obj,
				Child, Buttons[ 7].obj,
				Child, Buttons[ 8].obj,
				Child, Buttons[ 9].obj,
				Child, Buttons[10].obj,
				Child, Buttons[11].obj,
				Child, Buttons[12].obj,
				Child, Buttons[13].obj,
				Child, Buttons[14].obj,
				Child, Buttons[15].obj,
			End,
			Child, RectangleObject, MUIA_Weight, 0, End,
		End,
		Child, RectangleObject, MUIA_FixHeight, 2, End,
		Child, HGroup,
			Child, RectangleObject, MUIA_Weight, 0, End,
			Child, GroupObject,
				MUIA_Group_Columns, 8,
				MUIA_Group_Spacing, 0,
				Child, Buttons[16].obj,
				Child, Buttons[17].obj,
				Child, Buttons[18].obj,
				Child, Buttons[19].obj,
				Child, Buttons[20].obj,
				Child, Buttons[21].obj,
				Child, Buttons[22].obj,
				Child, Buttons[23].obj,
				Child, Buttons[24].obj,
				Child, Buttons[25].obj,
				Child, Buttons[26].obj,
				Child, Buttons[27].obj,
				Child, Buttons[28].obj,
				Child, Buttons[29].obj,
				Child, Buttons[30].obj,
				Child, Buttons[31].obj,
				Child, Buttons[32].obj,
				Child, Buttons[33].obj,
				Child, Buttons[34].obj,
				Child, Buttons[35].obj,
				Child, Buttons[36].obj,
				Child, Buttons[37].obj,
				Child, Buttons[38].obj,
				Child, Buttons[39].obj,
				Child, Buttons[40].obj,
				Child, RectangleObject, End,
				Child, RectangleObject, End,
				Child, RectangleObject, End,
				Child, RectangleObject, End,
				Child, RectangleObject, End,
				Child, Buttons[41].obj,
				Child, Buttons[42].obj,
			End,
			Child, RectangleObject, MUIA_Weight, 0, End,
		End,
	End);
}
/*----------------------------------------------------------------------------*/
/*     MakePenGroup - creates the Pen color Selection Group                   */
/*----------------------------------------------------------------------------*/
APTR MakePenGroup(void)
{
	int i;
	APTR VG_PenUser;

    /* Create color buttons */
    for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        BUT_PENCOL[i] = MakeColorButton(ColorPalette[i].RGB, COLORCELLSIZE, COLORCELLSIZE);
    }
    
    for (i=0; i < ((NUM_STD_COLS + MAX_USR_COLS)/16); i++)
    {
        PLINE_BUT[i] = HGroup,
            MUIA_Group_Spacing, 0,
            Child, BUT_PENCOL[i * 16 +  0],
            Child, BUT_PENCOL[i * 16 +  1],
            Child, BUT_PENCOL[i * 16 +  2],
            Child, BUT_PENCOL[i * 16 +  3],
            Child, BUT_PENCOL[i * 16 +  4],
            Child, BUT_PENCOL[i * 16 +  5],
            Child, BUT_PENCOL[i * 16 +  6],
            Child, BUT_PENCOL[i * 16 +  7],
            Child, BUT_PENCOL[i * 16 +  8],
            Child, BUT_PENCOL[i * 16 +  9],
            Child, BUT_PENCOL[i * 16 + 10],
            Child, BUT_PENCOL[i * 16 + 11],
            Child, BUT_PENCOL[i * 16 + 12],
            Child, BUT_PENCOL[i * 16 + 13],
            Child, BUT_PENCOL[i * 16 + 14],
            Child, BUT_PENCOL[i * 16 + 15],
        End;
    }

    /* Build User area dynamically to avoid hardcoded row limits */
    VG_PenUser = VGroup, MUIA_Group_Spacing, 0, End;
    for (i = 2; i < ((NUM_STD_COLS + MAX_USR_COLS)/16); i++) {
        DoMethod(VG_PenUser, OM_ADDMEMBER, PLINE_BUT[i]);
    }
    
    return (VirtgroupObject,
        MUIA_Group_Spacing, 2,
        MUIA_Group_Horiz, FALSE,
        Child, VGroup,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, "Standard Colors",
            MUIA_Group_Spacing, 0,
            Child, PLINE_BUT[0],
            Child, PLINE_BUT[1],
        End,
        Child, VGroup,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (UBYTE *)_(msg_UserColors),
            MUIA_Group_Spacing, 0,
            Child, VG_PenUser,
        End,
    End);
}

/*----------------------------------------------------------------------------*/
/*     MakeFillGroup - creates the Fill color Selection Group                 */
/*----------------------------------------------------------------------------*/
APTR MakeFillGroup(void)
{
	int i;
	APTR VG_FilUser;

    /* Create color buttons */
    for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        BUT_FILCOL[i] = MakeColorButton(ColorPalette[i].RGB, COLORCELLSIZE, COLORCELLSIZE);
    }
    
    for (i=0; i < ((NUM_STD_COLS + MAX_USR_COLS)/16); i++)
    {
        FLINE_BUT[i] = HGroup,
            MUIA_Group_Spacing, 0,
            Child, BUT_FILCOL[i * 16 +  0],
            Child, BUT_FILCOL[i * 16 +  1],
            Child, BUT_FILCOL[i * 16 +  2],
            Child, BUT_FILCOL[i * 16 +  3],
            Child, BUT_FILCOL[i * 16 +  4],
            Child, BUT_FILCOL[i * 16 +  5],
            Child, BUT_FILCOL[i * 16 +  6],
            Child, BUT_FILCOL[i * 16 +  7],
            Child, BUT_FILCOL[i * 16 +  8],
            Child, BUT_FILCOL[i * 16 +  9],
            Child, BUT_FILCOL[i * 16 + 10],
            Child, BUT_FILCOL[i * 16 + 11],
            Child, BUT_FILCOL[i * 16 + 12],
            Child, BUT_FILCOL[i * 16 + 13],
            Child, BUT_FILCOL[i * 16 + 14],
            Child, BUT_FILCOL[i * 16 + 15],
        End;
    }

    /* Build User area dynamically to avoid hardcoded row limits */
    VG_FilUser = VGroup, MUIA_Group_Spacing, 0, End;
    for (i = 2; i < ((NUM_STD_COLS + MAX_USR_COLS)/16); i++) {
        DoMethod(VG_FilUser, OM_ADDMEMBER, FLINE_BUT[i]);
    }
    
    return (VirtgroupObject,
        MUIA_Group_Spacing, 2,
        MUIA_Group_Horiz, FALSE,
        Child, VGroup,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, "Standard Colors",
            MUIA_Group_Spacing, 0,
            Child, FLINE_BUT[0],
            Child, FLINE_BUT[1],
        End,
        Child, VGroup,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (UBYTE *)_(msg_UserColors),
            MUIA_Group_Spacing, 0,
            Child, VG_FilUser,
        End,
    End);
}

/*----------------------------------------------------------------------------*/
/*     MakeShadesGroup - creates the Shades color Selection Group             */
/*----------------------------------------------------------------------------*/
APTR MakeShadesGroup(void)
{
	int i;
	
	/* Create Shades buttons */    
    for(i=0; i<NUMSHADES; i++)
    {
        if (i != 20) BUT_FSHADES[i] = MakeColorButton(getfillcolor(cur_fillcolor, i), COLORCELLSIZE, COLORCELLSIZE);		
    }
    BUT_FSHADES[20] = MakeColorButton(ColorPalette[cur_fillcolor].RGB, 4*COLORCELLSIZE, 4*COLORCELLSIZE);
    
    return(HGroup,
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_FrameTitle, (UBYTE *)_(msg_SelectShadeTint),
        Child, RectangleObject, End,
        Child, BUT_FSHADES[20],
        Child, RectangleObject, End,
        Child, GroupObject,
            MUIA_Group_Columns, 10,
            MUIA_Group_Spacing, 0,
            Child, BUT_FSHADES[40],
            Child, BUT_FSHADES[39],
            Child, BUT_FSHADES[38],
            Child, BUT_FSHADES[37],
            Child, BUT_FSHADES[36],
            Child, BUT_FSHADES[35],
            Child, BUT_FSHADES[34],
            Child, BUT_FSHADES[33],
            Child, BUT_FSHADES[32],
            Child, BUT_FSHADES[31],
            Child, BUT_FSHADES[30],
            Child, BUT_FSHADES[29],
            Child, BUT_FSHADES[28],
            Child, BUT_FSHADES[27],
            Child, BUT_FSHADES[26],
            Child, BUT_FSHADES[25],
            Child, BUT_FSHADES[24],
            Child, BUT_FSHADES[23],
            Child, BUT_FSHADES[22],
            Child, BUT_FSHADES[21],
            Child, BUT_FSHADES[19],
            Child, BUT_FSHADES[18],
            Child, BUT_FSHADES[17],
            Child, BUT_FSHADES[16],
            Child, BUT_FSHADES[15],
            Child, BUT_FSHADES[14],
            Child, BUT_FSHADES[13],
            Child, BUT_FSHADES[12],
            Child, BUT_FSHADES[11],
            Child, BUT_FSHADES[10],
            Child, BUT_FSHADES[ 9],
            Child, BUT_FSHADES[ 8],
            Child, BUT_FSHADES[ 7],
            Child, BUT_FSHADES[ 6],
            Child, BUT_FSHADES[ 5],
            Child, BUT_FSHADES[ 4],
            Child, BUT_FSHADES[ 3],
            Child, BUT_FSHADES[ 2],
            Child, BUT_FSHADES[ 1],
            Child, BUT_FSHADES[ 0],
        End,
    End);
}

/*----------------------------------------------------------------------------*/
/*     MakePatternsGroup - creates the Patterns color Selection Group         */
/*----------------------------------------------------------------------------*/
APTR MakePatternsGroup(void)
{
	static struct Image PatternImages[NUMPATTERNS];
	int i;

	/* Create patterns buttons */
	for (i=0; i<NUMPATTERNS; i++)
	{
		PatternImages[i].LeftEdge	= 0;
		PatternImages[i].TopEdge	= 0;
		PatternImages[i].Width		= 16;
		PatternImages[i].Height		= 16;
		PatternImages[i].Depth		= 1;
		PatternImages[i].ImageData	= (UWORD *)patternPointers[i];
		PatternImages[i].PlanePick	= 0x0001;
		PatternImages[i].PlaneOnOff	= 0x0000;
		PatternImages[i].NextImage	= NULL;
		BUT_FPATTERNS[i] = ImageObject,
			MUIA_Image_OldImage,  &PatternImages[i],
			MUIA_Frame,           MUIV_Frame_ImageButton,
			MUIA_InputMode,       MUIV_InputMode_Toggle,
			MUIA_InnerLeft,       0,
			MUIA_InnerRight,      0,
			MUIA_InnerTop,        0,
			MUIA_InnerBottom,     0,     
		End;							
	}
    return(HGroup,
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_FrameTitle, (UBYTE *)_(msg_SelectPattern),
        Child, RectangleObject, End,
        Child, GroupObject,
            MUIA_Group_Columns, 11,
            MUIA_Group_Spacing, 0,
            Child, BUT_FPATTERNS[ 0],
            Child, BUT_FPATTERNS[ 1],
            Child, BUT_FPATTERNS[ 2],
            Child, BUT_FPATTERNS[ 3],
            Child, BUT_FPATTERNS[ 4],
            Child, BUT_FPATTERNS[ 5],
            Child, BUT_FPATTERNS[ 6],
            Child, BUT_FPATTERNS[ 7],
            Child, BUT_FPATTERNS[ 8],
            Child, BUT_FPATTERNS[ 9],
            Child, BUT_FPATTERNS[10],
            Child, BUT_FPATTERNS[11],
            Child, BUT_FPATTERNS[12],
            Child, BUT_FPATTERNS[13],
            Child, BUT_FPATTERNS[14],
            Child, BUT_FPATTERNS[15],
            Child, BUT_FPATTERNS[16],
            Child, BUT_FPATTERNS[17],
            Child, BUT_FPATTERNS[18],
            Child, BUT_FPATTERNS[19],
            Child, BUT_FPATTERNS[20],
            Child, BUT_FPATTERNS[21],
		End,
		Child, RectangleObject, End,
	End);
}

/*----------------------------------------------------------------------------*/
/*     MakeAlignGroup - creates the Align button Group				          */
/*----------------------------------------------------------------------------*/
APTR MakeAlignGroup(void)
{
	int i;
	
    /* Create Align image buttons */
    for (i=0; i < 4; i++)
    {
        HAlignButtons[i].obj = MakeImageToggle(HAlignButtons[i].image , HAlignButtons[i].key, HAlignButtons[i].msg);
        VAlignButtons[i].obj = MakeImageToggle(VAlignButtons[i].image , VAlignButtons[i].key, VAlignButtons[i].msg);
    }
    return(HGroup,
		MUIA_Group_Spacing, 0,
		
		Child, RectangleObject, End,
		Child, HAlignButtons[0].obj,
		Child, HAlignButtons[1].obj,
		Child, HAlignButtons[2].obj,
		Child, HAlignButtons[3].obj,
		Child, RectangleObject, End,
		Child, VAlignButtons[0].obj,
		Child, VAlignButtons[1].obj,
		Child, VAlignButtons[2].obj,
		Child, VAlignButtons[3].obj,
		Child, RectangleObject, End,
	End);  
}

/*----------------------------------------------------------------------------*/
/*     MakeDimLinesPanel - creates the Dim Lines panel                        */
/*----------------------------------------------------------------------------*/

APTR MakeDimLinesPanel(void)
{
    static char *dimregtitles[] = {NULL, NULL, NULL};

    dimregtitles[0] = (char *)_(msg_Line);
	dimregtitles[1] = (char *)_(msg_TextBox);
	
    NUM_DL_THICK = NewObject(	mcc_asl->mcc_Class, NULL,
        MUIA_Group_Horiz , TRUE,
		MUIA_Numeric_Min  , 0,
		MUIA_Numeric_Max  , 20,
		MUIA_Numeric_Value, cur_dimline_thick,
        TAG_DONE);
	
	CYC_DL_COLOR = CycleObject,
		MUIA_Cycle_Entries, COL_strings,
		MUIA_Cycle_Active, cur_dimline_color,
	End;

	CYC_DL_STYLE = CycleObject,
		MUIA_Cycle_Entries, CYC_LINESTYLE_strings,
		MUIA_Cycle_Active, cur_dimline_style,
	End;

	CHK_DL_TICKS = MakeCheckmark((UBYTE *)"", cur_dimline_ticks);

	NUM_DL_TICKTHICK = NewObject(	mcc_asl->mcc_Class, NULL,
        MUIA_Group_Horiz , TRUE,
		MUIA_Numeric_Min  , 0,
		MUIA_Numeric_Max  , 20,
		MUIA_Numeric_Value, cur_dimline_tickthick,
        TAG_DONE);

	CYC_DL_LARROW = CycleObject,
		MUIA_Cycle_Entries, CYC_ARROWTYPE_strings,
		MUIA_Cycle_Active, cur_dimline_leftarrow,
	End;
	
	CYC_DL_RARROW = CycleObject,
		MUIA_Cycle_Entries, CYC_ARROWTYPE_strings,
		MUIA_Cycle_Active, cur_dimline_rightarrow,
	End;
	
	/* Text Box */
	CYC_DL_FONT = CycleObject,
		MUIA_Cycle_Entries, TXTFONT_strings,
		MUIA_Cycle_Active, ((cur_dimline_psflag & PSFONT_TEXT)?cur_dimline_font+1:cur_dimline_font + NUM_FONTS + 1),
	End;

	CYC_DL_TXTCOL = CycleObject,
		MUIA_Cycle_Entries, COL_strings,
		MUIA_Cycle_Active, cur_dimline_textcolor,
	End;
	
	CYC_DL_BOXCOL = CycleObject,
		MUIA_Cycle_Entries, COL_strings,
		MUIA_Cycle_Active, cur_dimline_boxcolor,
	End;
	
	NUM_DL_FONTSIZE = NewObject(	mcc_asl->mcc_Class, NULL,
        MUIA_Group_Horiz , TRUE,
		MUIA_Numeric_Min  , 1,
		MUIA_Numeric_Max  , 72,
		MUIA_Numeric_Value, cur_dimline_fontsize,
        TAG_DONE);
	
	NUM_DL_BOXTHICK = NewObject(	mcc_asl->mcc_Class, NULL,
        MUIA_Group_Horiz , TRUE,
		MUIA_Numeric_Min  , 0,
		MUIA_Numeric_Max  , 20,
		MUIA_Numeric_Value, cur_dimline_boxthick,
        TAG_DONE);
	
	NUM_DL_PRECISION = NewObject(	mcc_asl->mcc_Class, NULL,
        MUIA_Group_Horiz , TRUE,
		MUIA_Numeric_Min  , 0,
		MUIA_Numeric_Max  , 4,
		MUIA_Numeric_Value, cur_dimline_prec,
        TAG_DONE);
	
	CHK_DL_FIXED = MakeCheckmark((UBYTE *)"", cur_dimline_fixed);
		
	return( RegisterGroup(dimregtitles),
		Child, VGroup,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Thickness), End,
				Child, NUM_DL_THICK,
			End,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Color), End,
				Child, CYC_DL_COLOR,
			End,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Style), End,
				Child, CYC_DL_STYLE,
			End,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_UseTicks), End,
                Child, CHK_DL_TICKS,
            End,
            Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_TickThickness), End,
				Child, NUM_DL_TICKTHICK,
			End,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_LeftArrow), End,
				Child, CYC_DL_LARROW,
			End,
			Child, HGroup,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_RightArrow), End,
				Child, CYC_DL_RARROW,
			End,
            Child, RectangleObject, End,
		End,
		Child, VGroup,
			Child, CYC_DL_FONT,
			Child, GroupObject,
                MUIA_Group_Columns, 2,
                MUIA_Group_Spacing, 5,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_TextSize), End,
				Child, NUM_DL_FONTSIZE,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_TextColor), End,
				Child, CYC_DL_TXTCOL,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Precision), End,
				Child, NUM_DL_PRECISION,
                Child, TextObject, MUIA_Text_Contents, (char *)_(msg_FixedText), End,
				Child, CHK_DL_FIXED,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_BoxThickness), End,
				Child, NUM_DL_BOXTHICK,
				Child, TextObject, MUIA_Text_Contents, (char *)_(msg_BoxColor), End,
				Child, CYC_DL_BOXCOL,
			End,
            Child, RectangleObject, End,
		End,
    End);
}

/*----------------------------------------------------------------------------*/
/*     MakeEditWindow - creates the edit window                               */
/*----------------------------------------------------------------------------*/

APTR MakeEditWindow(void)
{
	APTR WHOLEGROUP;
    
    /* initialise strings */
    regtitles[0] = (char *)_(msg_Stroke);
    regtitles[1] = (char *)_(msg_Fill);
    regtitles[2] = (char *)_(msg_Text);
    regtitles[3] = (char *)_(msg_Dim);
    regtitles[4] = (char *)_(msg_Updates);
    CYC_LINESTYLE_strings[0] = (char *)_(msg_Solid);
    CYC_LINESTYLE_strings[1] = (char *)_(msg_Dashed); 
    CYC_LINESTYLE_strings[2] = (char *)_(msg_Dotted); 
    CYC_LINESTYLE_strings[3] = (char *)_(msg_Dash1Dot); 
    CYC_LINESTYLE_strings[4] = (char *)_(msg_Dash2Dot); 
    CYC_LINESTYLE_strings[5] = (char *)_(msg_Dash3Dot);
    CYC_JOINSTYLE_strings[0] = (char *)_(msg_Miter);
    CYC_JOINSTYLE_strings[1] = (char *)_(msg_Round);
    CYC_JOINSTYLE_strings[2] = (char *)_(msg_Bevel);
    CYC_CAPSTYLE_strings[0] = (char *)_(msg_Butt);
    CYC_CAPSTYLE_strings[1] = (char *)_(msg_Cap);
    CYC_CAPSTYLE_strings[2] = (char *)_(msg_Project);
    CYC_TXTJUST_strings[0] = (char *)_(msg_Left);
    CYC_TXTJUST_strings[1] = (char *)_(msg_Center);
    CYC_TXTJUST_strings[2] = (char *)_(msg_Right);
    CYC_ARCTYPE_strings[0] = (char *)_(msg_Open);
    CYC_ARCTYPE_strings[1] = (char *)_(msg_Closed);
    CYC_ARROWTYPE_strings[0] = (char *)_(msg_Boring); 
    CYC_ARROWTYPE_strings[1] = (char *)_(msg_HollowTriangle);
    CYC_ARROWTYPE_strings[2] = (char *)_(msg_PlainTriangle);
    CYC_ARROWTYPE_strings[3] = (char *)_(msg_HollowNice);
    CYC_ARROWTYPE_strings[4] = (char *)_(msg_PlainNice);
    CYC_ARROWTYPE_strings[5] = (char *)_(msg_HollowSpear);
    CYC_ARROWTYPE_strings[6] = (char *)_(msg_PlainSpear);
    CYC_ARROWTYPE_strings[ 7] = (char *)"Diamond H";
    CYC_ARROWTYPE_strings[ 8] = (char *)"Diamond P";
    CYC_ARROWTYPE_strings[ 9] = (char *)"Circle H";
    CYC_ARROWTYPE_strings[10] = (char *)"Circle P";
    CYC_ARROWTYPE_strings[11] = (char *)"Half circle H";
    CYC_ARROWTYPE_strings[12] = (char *)"Half circle P";
    CYC_ARROWTYPE_strings[13] = (char *)"Square H";
    CYC_ARROWTYPE_strings[14] = (char *)"Square P";
    CYC_ARROWTYPE_strings[15] = (char *)"Rev. triangle H";
    CYC_ARROWTYPE_strings[16] = (char *)"Rev. triangle P";
    CYC_ARROWTYPE_strings[17] = (char *)"Top Half F Spearhead";
    CYC_ARROWTYPE_strings[18] = (char *)"Bottom Half F Spearhead";
    CYC_ARROWTYPE_strings[19] = (char *)"Top Triangle H";
    CYC_ARROWTYPE_strings[20] = (char *)"Top Triangle P";
    CYC_ARROWTYPE_strings[21] = (char *)"top-half concave spearhead H";
    CYC_ARROWTYPE_strings[22] = (char *)"top-half concave spearhead P";
    CYC_ARROWTYPE_strings[23] = (char *)"top-half convex spearhead H";
    CYC_ARROWTYPE_strings[24] = (char *)"top-half convex spearhead P";
    CYC_ARROWTYPE_strings[25] = (char *)"wye";
    CYC_ARROWTYPE_strings[26] = (char *)"bar";
    CYC_ARROWTYPE_strings[27] = (char *)"two-prong fork";
    CYC_ARROWTYPE_strings[28] = (char *)"backward two-prong fork";
    CYC_ARROWTYPE_strings[29] = NULL;
    CYC_FMODE_strings[0] = (char *)_(msg_NoFill);
    CYC_FMODE_strings[1] = (char *)_(msg_Plain);
    CYC_FMODE_strings[2] = (char *)_(msg_Pattern);
    CYC_FMODE_strings[3] = (char *)_(msg_Gradient);

	
	WHOLEGROUP = VirtgroupObject,
        /* The main functions buttons */
		Child, ButtonGRP = MakeButtonGrp(),
        
		/* Specific parameters that are only visible when corresponding functions are selected */
		Child, AlignGRP = MakeAlignGroup(),        

        /* Mouse button assign reminder */
        Child, GroupObject,
            MUIA_Frame, MUIV_Frame_Group,
            MUIA_FrameTitle, (char *)_(msg_MouseFunction),
            MUIA_Group_Columns, 3,
            MUIA_Group_Spacing, 2,
            Child, VGroup,
                Child, TextObject, MUIA_Text_PreParse, "\033c", MUIA_Text_Contents, "Left", End,
                Child, TEXT_L_MOUSE  = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
            Child, VGroup,
                Child, TextObject, MUIA_Text_PreParse, "\033c", MUIA_Text_Contents, "Middle", End,
                Child, TEXT_M_MOUSE  = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
            Child, VGroup,
                Child, TextObject, MUIA_Text_PreParse, "\033c", MUIA_Text_Contents, "Right", End,
                Child, TEXT_R_MOUSE  = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
            Child, VGroup,
                Child, HGroup,
                    MUIA_Weight, 0,
                    Child, ImageObject, MUIA_Image_Spec, MUII_ArrowUp, MUIA_FixWidth, 12, MUIA_FixHeight, 12, End,
                    Child, TextObject, MUIA_Text_Contents, " + Left", End,
                End,
                Child, TEXT_SL_MOUSE = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
            Child, VGroup,
                Child, HGroup,
                    MUIA_Weight, 0,
                    Child, ImageObject, MUIA_Image_Spec, MUII_ArrowUp, MUIA_FixWidth, 12, MUIA_FixHeight, 12, End,
                    Child, TextObject, MUIA_Text_Contents, " + Middle", End,
                End,
                Child, TEXT_SM_MOUSE = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
            Child, VGroup,
                Child, HGroup,
                    MUIA_Weight, 0,
                    Child, ImageObject, MUIA_Image_Spec, MUII_ArrowUp, MUIA_FixWidth, 12, MUIA_FixHeight, 12, End,
                    Child, TextObject, MUIA_Text_Contents, " + Right", End,
                End,
                Child, TEXT_SR_MOUSE = TextObject,  MUIA_Frame, MUIV_Frame_Text, End,
            End,
        End,

        Child, HGroup,
            /* Depth setting */
            Child, DepthGRP = HGroup,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, (char *)_(msg_Depth),
                Child, NUM_DEPTH = NewObject(	mcc_asl->mcc_Class, NULL,
                    MUIA_Group_Horiz  , TRUE,
                    MUIA_Numeric_Min  , MIN_DEPTH,
                    MUIA_Numeric_Max  , MAX_DEPTH,
                    MUIA_Numeric_Value, cur_depth,
                    TAG_DONE),
            End,
            Child, RotAngleGRP = GroupObject,
                MUIA_ShowMe, FALSE,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, (UBYTE *)_(msg_RotAngle),
                Child, NUM_ROTANGLE = NewObject(	mcc_asl->mcc_Class, NULL,
                    MUIA_Group_Horiz , TRUE,
                    MUIA_Numeric_Min  , 0,
                    MUIA_Numeric_Max  , 359,
                    MUIA_Numeric_Value, cur_rotnangle,
                    TAG_DONE),
            End,
            Child, NumRegPolySidesGRP = GroupObject,
                MUIA_ShowMe, FALSE,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, "RegPoly sides",
                Child, NUM_NUMSIDES = NewObject(	mcc_asl->mcc_Class, NULL,
                    MUIA_Group_Horiz  , TRUE,
                    MUIA_Numeric_Min  , 3,
                    MUIA_Numeric_Max  , 20,
                    MUIA_Numeric_Value, cur_numsides,
                    TAG_DONE),
            End,
            Child, BoxCurveGRP = GroupObject,
                MUIA_ShowMe, FALSE,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, "Box Curve",
                Child, NUM_BOXRAD = NewObject(	mcc_asl->mcc_Class, NULL,
                    MUIA_Group_Horiz  , TRUE,
                    MUIA_Numeric_Min  , 1,
                    MUIA_Numeric_Max  , 20,
                    MUIA_Numeric_Value, cur_boxradius,
                    TAG_DONE),
            End,
            Child, EllAngleGRP = GroupObject,
                MUIA_ShowMe, FALSE,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, (char *)_(msg_EllipseAngle),
                Child, NUM_ELLIANGLE = NewObject(	mcc_asl->mcc_Class, NULL,
                    MUIA_Group_Horiz  , TRUE,
                    MUIA_Numeric_Min  , 0,
                    MUIA_Numeric_Max  , 359,
                    MUIA_Numeric_Value, cur_elltextangle,
                    TAG_DONE),
            End,
            Child, ArcTypeGRP = GroupObject,
                MUIA_ShowMe, FALSE,
                MUIA_Frame, MUIV_Frame_Group,
                MUIA_FrameTitle, (char *)_(msg_ArcType),
                Child, CYC_ARCTYPE = CycleObject,
                        MUIA_Cycle_Entries, CYC_ARCTYPE_strings,
                        MUIA_Cycle_Active, cur_arctype,
                End,
            End,
        End,
        
		/* Panels */
		Child, RegisterGroup(regtitles),
			/* Stroke panel */
			Child, VGroup,
				Child, HGroup,
					Child, TextObject, MUIA_Text_Contents , (char *)_(msg_Width), End,
					Child, NUM_LWIDTH = NewObject(	mcc_asl->mcc_Class, NULL,
						MUIA_Group_Horiz  , TRUE,
						MUIA_Numeric_Min  , 0,
						MUIA_Numeric_Max  , 99,
						MUIA_Numeric_Value, cur_linewidth,
                        TAG_DONE),
				End,
                Child, HGroup,
                    Child, TextObject, MUIA_Text_Contents , (char *)_(msg_Style), End,
                    Child, CYC_LINESTYLE = CycleObject,
                        MUIA_Cycle_Entries, CYC_LINESTYLE_strings,
                    End,
                End,
                Child, HGroup,
                    Child, TextObject, MUIA_Text_Contents , (char *)_(msg_GapLength), End,
                    Child, NUM_DOPGAP = NewObject(	mcc_asl->mcc_Class, NULL,
                        MUIA_Group_Horiz  , TRUE,
                        MUIA_Numeric_Min  , 1,
                        MUIA_Numeric_Max  , 10,
                        MUIA_Numeric_Value, round(cur_dotgap),
                        TAG_DONE),                  
                    Child, TextObject, MUIA_Text_Contents , (char *)_(msg_DashLength), End,
                    Child, NUM_DASHLEN = NewObject(	mcc_asl->mcc_Class, NULL,
                        MUIA_Group_Horiz  , TRUE,
                        MUIA_Numeric_Min  , 1,
                        MUIA_Numeric_Max  , 10,
                        MUIA_Numeric_Value, round(cur_dashlength),
                        TAG_DONE),                  
                End,
                Child, HGroup,
                    Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Arrow), End,          
                    Child, CYC_ARROWTYPE = CycleObject,
                        MUIA_Cycle_Entries, CYC_ARROWTYPE_strings,
                        MUIA_Cycle_Active, cur_arrowtype,
                    End,
                End,
                Child, HGroup,                    
                    Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Join), End,
                    Child, CYC_JOINSTYLE = CycleObject,
                        MUIA_Cycle_Entries, CYC_JOINSTYLE_strings,
                    End,
                    Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Cap), End,
                    Child, CYC_CAPSTYLE = CycleObject,
                        MUIA_Cycle_Entries, CYC_CAPSTYLE_strings,
                    End,
                End,
				Child, ScrollgroupObject,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_PenColor),
					MUIA_Weight, 800,
					MUIA_Scrollgroup_FreeHoriz, FALSE,
					MUIA_Scrollgroup_Contents, VG_PENPAL = MakePenGroup(),
				End,
				Child, BUT_PEN_EDITUSRCOL = MakeButton((UBYTE *)_(msg_EditColors), '\0', (UBYTE *)_(msg_OpenColorWindow)),
			End,
			/* Fill Panel */
			Child, FilPanel = VGroup,
				MUIA_Background, MUII_WindowBack,
				Child, HGroup,
					MUIA_Frame, MUIV_Frame_Group,
					Child, TextObject, MUIA_Text_Contents , (UBYTE *)_(msg_FillType), End,
					Child, CYC_FMODE = CycleObject,
							MUIA_Cycle_Entries, CYC_FMODE_strings,
							MUIA_Cycle_Active, 0L,
					End,
				End,
				Child, DUM_PALETTE = RectangleObject, End,
				Child, SG_FILPAL = ScrollgroupObject,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_FillColor),
					MUIA_Weight, 800,
					MUIA_Scrollgroup_FreeHoriz, FALSE,
					MUIA_Scrollgroup_Contents, VG_FILPAL = MakeFillGroup(),
					
				End,
				Child, VG_SHADES = MakeShadesGroup(),
				Child, VG_PATTERNS = MakePatternsGroup(),
				Child, VG_GRADIENT = VGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (char *)_(msg_Gradient),
					MUIA_ShowMe, FALSE,
					MUIA_Weight, 800,
					
					Child, HGroup,
						Child, NUM_GRADIENT = NewObject(	mcc_asl->mcc_Class, NULL,
							MUIA_Group_Horiz  , TRUE,
							MUIA_Numeric_Min  , 0,
							MUIA_Numeric_Max  , 511,
							MUIA_Numeric_Value, 0,
                            TAG_DONE),
					End,
					Child, HGroup,
						Child, RectangleObject, MUIA_Weight, 1, End,
						Child, GRD_PREVIEW = NewObject(	mcc_grad->mcc_Class, NULL,
                                                        MUIA_Background, MUII_BACKGROUND,
                                                        MUIA_Weight, 100,
														TAG_DONE),
						Child, RectangleObject, MUIA_Weight, 1, End,
					End,
				End,
				Child, BUT_FIL_EDITUSRCOL = MakeButton((UBYTE *)_(msg_EditColors), '\0', (UBYTE *)_(msg_OpenColorWindow)),
			End,                        
			/* Text panel */
			Child, VGroup,
				Child, LST_TXTFONT = ListviewObject,
					MUIA_Listview_List , ListObject,
						ReadListFrame,
						MUIA_List_SourceArray, TXTFONT_strings,
						MUIA_List_Active, ((cur_textflags & PSFONT_TEXT)?cur_ps_font+1:cur_latex_font + NUM_FONTS + 1),
						MUIA_List_Format, "P=\33c",
					End,
				End,
				Child, GroupObject,
					MUIA_Group_Columns, 2,
					MUIA_Group_Spacing, 5,
					Child, TextObject, MUIA_Text_Contents , (char *)_(msg_Size), End,
					Child, NUM_FONTSIZE = NewObject(	mcc_asl->mcc_Class, NULL,
						MUIA_Group_Horiz  , TRUE,
						MUIA_Numeric_Min  , SMALLESTFONT,
						MUIA_Numeric_Max  , BIGGESTFONT,
						MUIA_Numeric_Value, cur_fontsize,
                        TAG_DONE),
					Child, TextObject, MUIA_Text_Contents , (char *)_(msg_TextAngle), End,
					Child, NUM_FONTANGLE = NewObject(	mcc_asl->mcc_Class, NULL,
						MUIA_Group_Horiz  , TRUE,
						MUIA_Numeric_Min  , 0,
						MUIA_Numeric_Max  , 359,
						MUIA_Numeric_Value, cur_textangle,
                        TAG_DONE),
					Child, TextObject, MUIA_Text_Contents , (char *)_(msg_Justification), End,
					Child, CYC_TXTJUST = CycleObject,
						MUIA_Cycle_Entries, CYC_TXTJUST_strings,
						MUIA_Cycle_Active, cur_textjust,
					End,
					Child, CHK_TXTRIGID  = MakeCheckmark((UBYTE *)_(msg_RigidText)  , 0L),
					Child, CHK_TXTSPECIAL = MakeCheckmark((UBYTE *)_(msg_SpecialText), 0L),
				End,
				Child, FNT_PREVIEW = NewObject(	mcc_font->mcc_Class, NULL,
												MUIA_Frame, MUIV_Frame_String,
												MYATTR_FONT, appres.DisplayFonts[0],
												MYATTR_FONTSIZE, cur_fontsize,
												TAG_DONE),
				Child, BUT_FONTCONF = MakeButton((UBYTE *)_(msg_ChangeDispFont), '\0', (UBYTE *)_(msg_DefineSystemFonts)),
			End,
			/* Dimension Lines Panel */
			Child, PAN_DIM = MakeDimLinesPanel(),
			/* Update Panel */
			Child, VGroup,
				Child, GroupObject,
					MUIA_Group_Columns, 2,
					MUIA_Group_Spacing, 2,
					Child, CHK_UPD[UPD_Depth      ] = MakeCheckmark((UBYTE *)_(msg_Depth), 0L),
					Child, CHK_UPD[UPD_Pencol     ] = MakeCheckmark((UBYTE *)_(msg_PenColor), 0L),
					Child, CHK_UPD[UPD_Fillcolor  ] = MakeCheckmark((UBYTE *)_(msg_FillColor), 0L),
					Child, CHK_UPD[UPD_Fillstyle  ] = MakeCheckmark((UBYTE *)_(msg_FillStyle), 0L),
					Child, CHK_UPD[UPD_Linewid    ] = MakeCheckmark((UBYTE *)_(msg_LineWidth), 0L),
					Child, CHK_UPD[UPD_Linestyle  ] = MakeCheckmark((UBYTE *)_(msg_LineStyle), 0L),
					Child, CHK_UPD[UPD_JoinStyle  ] = MakeCheckmark((UBYTE *)_(msg_JoinStyle), 0L),
					Child, CHK_UPD[UPD_CapStyle   ] = MakeCheckmark((UBYTE *)_(msg_CapStyle), 0L),
					Child, CHK_UPD[UPD_ETAngle    ] = MakeCheckmark((UBYTE *)_(msg_EllipseAngle), 0L),
					Child, CHK_UPD[UPD_Arctype    ] = MakeCheckmark((UBYTE *)_(msg_ArcType), 0L),
					Child, CHK_UPD[UPD_Font       ] = MakeCheckmark((UBYTE *)_(msg_TextFont), 0L),
					Child, CHK_UPD[UPD_Fontsize   ] = MakeCheckmark((UBYTE *)_(msg_TextSize), 0L),
					Child, CHK_UPD[UPD_Adjust     ] = MakeCheckmark((UBYTE *)_(msg_TextAdjust), 0L),
					Child, CHK_UPD[UPD_Rigid      ] = MakeCheckmark((UBYTE *)_(msg_TextRigid), 0L),
					Child, CHK_UPD[UPD_SpecialText] = MakeCheckmark((UBYTE *)_(msg_TextSpecial), 0L),
					Child, CHK_UPD[UPD_TextAngle  ] = MakeCheckmark((UBYTE *)_(msg_TextAngle), 0L),
				End,
				Child, HGroup,
					Child, BUT_UPD_ALL  = MakeButton((UBYTE *)_(msg_All), '\0', (UBYTE *)_(msg_SelectAllFields)),
					Child, RectangleObject, End,
					Child, BUT_UPD_NONE = MakeButton((UBYTE *)_(msg_None), '\0', (UBYTE *)_(msg_UnselectAll)),
				End,
				Child, RectangleObject, End,
			End,
		End,
	End;

    return(WindowObject,
        MUIA_Window_Title, "AmiFig : Edit",
        MUIA_Window_ID, MAKEID('Z','F','E','D'),
        MUIA_Window_Menustrip, MUI_MakeObject(MUIO_MenustripNM,AmiFIGNM,0),
		MUIA_Window_Screen, Scr,
		MUIA_Window_ScreenTitle, "AmiFIG",
        MUIA_Window_TopEdge, MUIV_Window_TopEdge_Delta(0),
        MUIA_Window_LeftEdge, 0,
        MUIA_Window_Width, MUIV_Window_Width_Visible(100),
        MUIA_Window_Height, MUIV_Window_Height_Screen(90),
        MUIA_Window_SizeGadget, FALSE,
        MUIA_Window_UseRightBorderScroller, TRUE,
        //MUIA_Window_UseBottomBorderScroller, TRUE,

        WindowContents,	ScrollgroupObject,
			MUIA_Scrollgroup_UseWinBorder, TRUE,
			MUIA_Scrollgroup_Contents, WHOLEGROUP,
		End,
    End);
}

void z_edit_methods(void)
{
    int i;
    
    static struct Hook SetLineStyleHook;
    static struct Hook SetFillStyleHook;
    static struct Hook SetFillPatternHook;
    static struct Hook SetFontStyleHook;
    static struct Hook DoEditMutexHook;
    static struct Hook DoAlignMutexHook;
    static struct Hook OpenFontConfHook;
    static struct Hook DoPenColorPalHook;
    static struct Hook DoFilColorPalHook;
    static struct Hook DoFModeHook;
    static struct Hook DoGradientHook;
    static struct Hook SetDLFontHook;
    SetLineStyleHook.h_Entry = (HOOKFUNC)SetLineStyle;
    SetFillStyleHook.h_Entry = (HOOKFUNC)SetFillStyle;
    SetFillPatternHook.h_Entry = (HOOKFUNC)SetFillPattern;
    SetFontStyleHook.h_Entry = (HOOKFUNC)SetFontStyle;
    DoEditMutexHook.h_Entry = (HOOKFUNC)DoEditMutex;
    DoAlignMutexHook.h_Entry = (HOOKFUNC)DoAlignMutex;
    OpenFontConfHook.h_Entry = (HOOKFUNC)OpenFontConf;
    DoPenColorPalHook.h_Entry = (HOOKFUNC)DoPenColorPal;
    DoFilColorPalHook.h_Entry = (HOOKFUNC)DoFilColorPal;
    DoFModeHook.h_Entry = (HOOKFUNC)DoFMode;
    DoGradientHook.h_Entry = (HOOKFUNC)DoGradient;
    SetDLFontHook.h_Entry = (HOOKFUNC)SetDLFont;
    
    /* Window exit */
    DoMethod(WinEdit, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
 
    /* Mutex buttons */
    for (i=0; i<NUM_BUTTONS; i++)
    {
        DoMethod(Buttons[i].obj, MUIM_Notify, MUIA_Selected, TRUE , MUIV_Notify_Self, 2, MUIM_CallHook   , (IPTR)&DoEditMutexHook);
        DoMethod(Buttons[i].obj, MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }

	/* Align mode buttons */
    for (i=0; i<4; i++)
    {
        DoMethod(HAlignButtons[i].obj , MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 4, MUIM_CallHook, (IPTR)&DoAlignMutexHook, 1, HAlignButtons[i].alignmode);
        DoMethod(VAlignButtons[i].obj , MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 4, MUIM_CallHook, (IPTR)&DoAlignMutexHook, 2, VAlignButtons[i].alignmode);
        DoMethod(HAlignButtons[i].obj , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
        DoMethod(VAlignButtons[i].obj , MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }
    for (i=0;i<4;i++)
    {
        if (cur_halign == HAlignButtons[i].alignmode)
            set(HAlignButtons[i].obj, MUIA_Selected, TRUE);
        if (cur_valign == VAlignButtons[i].alignmode)
            set(VAlignButtons[i].obj, MUIA_Selected, TRUE);
    }   

    /* cur_rotnangle */
    DoMethod(NUM_ROTANGLE  , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_rotnangle);
    
    /* General */
    DoMethod(NUM_DEPTH     , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_depth);
    DoMethod(NUM_LWIDTH    , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_linewidth);
    DoMethod(NUM_DOPGAP    , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetLineStyleHook, 0);
    DoMethod(NUM_DASHLEN   , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetLineStyleHook, 0);
    DoMethod(CYC_LINESTYLE , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetLineStyleHook, 0);
    DoMethod(CYC_JOINSTYLE , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_joinstyle);
    DoMethod(CYC_CAPSTYLE  , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_capstyle);
    DoMethod(CYC_ARCTYPE   , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_arctype);
    DoMethod(NUM_ELLIANGLE , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_elltextangle);
    DoMethod(CYC_ARROWTYPE , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_arrowtype);
    DoMethod(NUM_BOXRAD    , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_boxradius);
    DoMethod(NUM_NUMSIDES  , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&cur_numsides);

    /* Colors */
    for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        DoMethod(BUT_PENCOL[i], MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoPenColorPalHook, 0);
        DoMethod(BUT_PENCOL[i], MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
        DoMethod(BUT_FILCOL[i], MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoFilColorPalHook, 0);
        DoMethod(BUT_FILCOL[i], MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }
    set(BUT_PENCOL[cur_pencolor], MUIA_Selected, TRUE);
    set(BUT_FILCOL[cur_fillcolor], MUIA_Selected, TRUE);
    for (i=0; i < NUMSHADES; i++)
    {
        DoMethod(BUT_FSHADES[i], MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&SetFillStyleHook, 0);
        DoMethod(BUT_FSHADES[i], MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }
	if ((cur_fillstyle > -1) && (cur_fillstyle < NUMSHADES))
	{
    	set(BUT_FSHADES[cur_fillstyle], MUIA_Selected, TRUE);
	}
    for (i=0; i < NUMPATTERNS; i++)
    {
        DoMethod(BUT_FPATTERNS[i], MUIM_Notify, MUIA_Selected, TRUE,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&SetFillPatternHook, 0);
        DoMethod(BUT_FPATTERNS[i], MUIM_Notify, MUIA_Selected, FALSE, MUIV_Notify_Self, 3, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    }
	if ((cur_fillstyle > 40) && (cur_fillstyle < NUMFILLPATS))
	{
    	set(BUT_FPATTERNS[cur_fillstyle - (NUMSHADEPATS+NUMTINTPATS)], MUIA_Selected, TRUE);
	}
    DoMethod(CYC_FMODE, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,  MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoFModeHook, MUIV_TriggerValue);
    set(CYC_FMODE, MUIA_Cycle_Active, 1L);
	set(CYC_FMODE, MUIA_Cycle_Active, 0L);
    
    /* Ensure gradient 0 is displayed in preview if it exists */
    if (GradientTable[0] != NULL) 
        set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[0]);
    DoMethod(BUT_PEN_EDITUSRCOL, MUIM_Notify, MUIA_Selected, FALSE , WinColor, 3, MUIM_Set      , MUIA_Window_Open, TRUE);
    DoMethod(BUT_FIL_EDITUSRCOL, MUIM_Notify, MUIA_Selected, FALSE , WinColor, 3, MUIM_Set      , MUIA_Window_Open, TRUE);
    DoMethod(NUM_GRADIENT, MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&DoGradientHook, 0);
    set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[0]);
    
    /* Text */
    DoMethod(NUM_FONTSIZE  , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 2);
    DoMethod(CYC_TXTJUST   , MUIM_Notify, MUIA_Cycle_Active ,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 0);
    DoMethod(NUM_FONTANGLE , MUIM_Notify, MUIA_Numeric_Value,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 0);
    DoMethod(LST_TXTFONT   , MUIM_Notify, MUIA_List_Active  ,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 1);
    DoMethod(CHK_TXTRIGID  , MUIM_Notify, MUIA_Selected     ,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 0);
    DoMethod(CHK_TXTSPECIAL, MUIM_Notify, MUIA_Selected     ,MUIV_EveryTime, App,3, MUIM_CallHook, (IPTR)&SetFontStyleHook, 0);
    DoMethod(BUT_FONTCONF  , MUIM_Notify, MUIA_Selected     ,TRUE, App, 3, MUIM_CallHook, (IPTR)&OpenFontConfHook, 0);

	/* Dimension lines */
	DoMethod(NUM_DL_THICK	  , MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_thick);
	DoMethod(CYC_DL_COLOR	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_color);
	DoMethod(CYC_DL_STYLE	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_style);
	DoMethod(CHK_DL_TICKS	  , MUIM_Notify, MUIA_Selected, 	 MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_ticks);
	DoMethod(NUM_DL_TICKTHICK , MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_tickthick);
	DoMethod(CYC_DL_LARROW	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_leftarrow);
	DoMethod(CYC_DL_RARROW	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_rightarrow);
	DoMethod(CYC_DL_TXTCOL	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_textcolor);
	DoMethod(CYC_DL_BOXCOL	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_boxcolor);
	DoMethod(NUM_DL_FONTSIZE  , MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_fontsize);
	DoMethod(NUM_DL_BOXTHICK  , MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_boxthick);
	DoMethod(NUM_DL_PRECISION , MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_prec);
	DoMethod(CHK_DL_FIXED	  , MUIM_Notify, MUIA_Selected, 	 MUIV_EveryTime, App, 3, MUIM_WriteLong, MUIV_TriggerValue, &cur_dimline_fixed);

	DoMethod(CYC_DL_FONT	  , MUIM_Notify, MUIA_Cycle_Active,  MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&SetDLFontHook , MUIV_TriggerValue);

	/* Update */
	for (i=0; i<UPD_Max; i++)
	{
		DoMethod(CHK_UPD[i]  , MUIM_Notify, MUIA_Selected     ,MUIV_EveryTime, App,3, MUIM_WriteLong,MUIV_TriggerValue,&updatelist[i]);
		set(CHK_UPD[i], MUIA_Selected, updatelist[i]);
	}

	DoMethod(BUT_UPD_ALL,  MUIM_Notify, MUIA_Selected, FALSE, BUT_UPD_ALL, 21, MUIM_MultiSet, MUIA_Selected, TRUE,
			CHK_UPD[0], CHK_UPD[ 1], CHK_UPD[ 2], CHK_UPD[ 3], CHK_UPD[ 4], CHK_UPD[ 5], CHK_UPD[ 6], CHK_UPD[ 7], CHK_UPD[8], 
			CHK_UPD[9], CHK_UPD[10], CHK_UPD[11], CHK_UPD[12], CHK_UPD[13], CHK_UPD[14], CHK_UPD[15], NULL);
	DoMethod(BUT_UPD_NONE, MUIM_Notify, MUIA_Selected, FALSE, BUT_UPD_NONE, 21, MUIM_MultiSet, MUIA_Selected, FALSE,
			CHK_UPD[0], CHK_UPD[ 1], CHK_UPD[ 2], CHK_UPD[ 3], CHK_UPD[ 4], CHK_UPD[ 5], CHK_UPD[ 6], CHK_UPD[ 7], CHK_UPD[8], 
			CHK_UPD[9], CHK_UPD[10], CHK_UPD[11], CHK_UPD[12], CHK_UPD[13], CHK_UPD[14], CHK_UPD[15], NULL);
}

/*----------------------------------------------------------------------------*/
/*     SetDLFont                                                              */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFH3(void, SetDLFont,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetDLFont(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
    if (*Arg < NUM_FONTS)
    {
        cur_dimline_font = *Arg - 1;
        cur_dimline_psflag|=PSFONT_TEXT;
    }
    else
    {
        cur_dimline_font = *Arg - NUM_FONTS - 1;
        cur_dimline_psflag&=~PSFONT_TEXT;
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SetFontStyle                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SetFontStyle,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetFontStyle(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    
    LONG selection = 0L;
    LONG rigid     = 0L;
    LONG special   = 0L;
	
    get(LST_TXTFONT   , MUIA_List_Active   , &selection);
    get(CHK_TXTRIGID  , MUIA_Selected      , &rigid);
    get(CHK_TXTSPECIAL, MUIA_Selected      , &special);
	get(NUM_FONTSIZE  , MUIA_Numeric_Value , &cur_fontsize);
	get(CYC_TXTJUST   , MUIA_Cycle_Active  , &cur_textjust);
	get(NUM_FONTANGLE , MUIA_Numeric_Value , &cur_textangle);
    
    if (rigid)      cur_textflags|=RIGID_TEXT;
    else            cur_textflags&=~RIGID_TEXT;
    
    if (special)    cur_textflags|=SPECIAL_TEXT;
    else            cur_textflags&=~SPECIAL_TEXT;
    
    if (selection < NUM_FONTS)
    {
        cur_ps_font = selection - 1;
        cur_textflags|=PSFONT_TEXT;
    }
    else
    {
        cur_latex_font = selection - NUM_FONTS - 1;
        cur_textflags&=~PSFONT_TEXT;
    }
	
	if (*Arg == 1)
		set(FNT_PREVIEW, MYATTR_FONT, appres.DisplayFonts[selection]);
	if (*Arg == 2)
		set(FNT_PREVIEW, MYATTR_FONTSIZE, cur_fontsize);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     OpenFontConf                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, OpenFontConf,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void OpenFontConf(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
	
	struct FontRequester *font_request = NULL;
	LONG selfont = 0L;

	font_request=(struct FontRequester *)AllocAslRequestTags	(ASL_FontRequest,
																(ASLFO_Screen), 		Scr,
																(TAG_DONE));

	if (font_request != NULL)
	{
		if(AslRequestTags(font_request, (TAG_DONE)) != 0)
		{
			get(LST_TXTFONT   , MUIA_List_Active  , &selfont);
			strncpy(appres.DisplayFonts[selfont], (const char *)font_request->fo_Attr.ta_Name,64);
			set(FNT_PREVIEW, MYATTR_FONT, appres.DisplayFonts[selfont]);
		}
		
		FreeAslRequest(font_request);
	}
#ifdef __AROS__	
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoPenColorPal                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoPenColorPal,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoPenColorPal(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    
    for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        if (Object != BUT_PENCOL[i])
            DoMethod(BUT_PENCOL[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
        {
            cur_pencolor = i;           
            DispatchColorUIUpdate(UPDATE_PALETTES, i);
        }
    }
#ifdef __AROS__    
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoFilColorPal                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoFilColorPal,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoFilColorPal(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    
    for (i=0; i < NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        if (Object != BUT_FILCOL[i])
            DoMethod(BUT_FILCOL[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
        {
            cur_fillcolor = i;
            DispatchColorUIUpdate(UPDATE_PALETTES | UPDATE_SHADES, i);
            set(BUT_FSHADES[20], MUIA_Selected, TRUE);
        }
    }
#ifdef __AROS__    
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SetLineStyle                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SetLineStyle,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetLineStyle(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    LONG dotgap = 0L;
    LONG dashlength = 0L;
    
    get(CYC_LINESTYLE   , MUIA_Cycle_Active , &cur_linestyle);
    get(NUM_DOPGAP      , MUIA_Numeric_Value, &dotgap);
    get(NUM_DASHLEN     , MUIA_Numeric_Value, &dashlength);
    
    cur_dotgap = (float)dotgap;
    cur_dashlength = (float)dashlength;
    
	switch(cur_linestyle)
	{
        case SOLID_LINE:
            cur_styleval = 0.0;
            break;
        case DASH_LINE:
            cur_styleval = cur_dashlength;
            break;
        case DOTTED_LINE:
            cur_styleval = cur_dotgap;
            break;
        case DASH_DOT_LINE:
            cur_styleval = cur_dashlength;
            break;
        case DASH_2_DOTS_LINE:
            cur_styleval = cur_dashlength;
            break;
        case DASH_3_DOTS_LINE:
            cur_styleval = cur_dashlength;
            break;
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SetFillStyle                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SetFillStyle,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetFillStyle(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    
    for (i=0; i < NUMSHADES; i++)
    {
        if (Object != BUT_FSHADES[i])
            DoMethod(BUT_FSHADES[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
           	cur_fillstyle = i;
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     SetFillPattern                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, SetFillPattern,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void SetFillPattern(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int i;
    
    for (i=0; i < NUMPATTERNS; i++)
    {
        if (Object != BUT_FPATTERNS[i])
            DoMethod(BUT_FPATTERNS[i], MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
           	cur_fillstyle = i + (NUMSHADEPATS+NUMTINTPATS);
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoFMode                                                                */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoFMode,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoFMode(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    DoMethod(FilPanel, MUIM_Group_InitChange);

    switch(*Arg)
    {
        case 0:
            /* No Fill style selected */
            cur_fillcolor = WHITE;
            cur_fillstyle = UNFILLED;
            set(SG_FILPAL   , MUIA_ShowMe, FALSE);
            set(VG_GRADIENT , MUIA_ShowMe, FALSE);
            set(VG_SHADES   , MUIA_ShowMe, FALSE);
            set(VG_PATTERNS , MUIA_ShowMe, FALSE);
            set(DUM_PALETTE , MUIA_ShowMe, TRUE);
            break;
        case 1:
            /* Plain Fill style selected */
            extern void RefreshAllColorButtons(void);
            if (cur_fillcolor >= NUM_STD_COLS + MAX_USR_COLS)
                cur_fillcolor = WHITE;
            cur_fillstyle = 20;
            set(SG_FILPAL   , MUIA_ShowMe, TRUE);
            set(VG_GRADIENT , MUIA_ShowMe, FALSE);
            set(VG_SHADES   , MUIA_ShowMe, TRUE);
            set(VG_PATTERNS , MUIA_ShowMe, FALSE);
            set(DUM_PALETTE , MUIA_ShowMe, FALSE);
            RefreshAllColorButtons();
            set(BUT_FILCOL[cur_fillcolor], MUIA_Selected, TRUE);
            UpdateFShades();
            set(BUT_FSHADES[cur_fillstyle], MUIA_Selected, TRUE);
            break;
        case 2:
            /* Pattern Fill style selected */
            cur_fillcolor = WHITE;
            cur_fillstyle = NUMSHADES;
            set(SG_FILPAL   , MUIA_ShowMe, TRUE);
            set(VG_GRADIENT , MUIA_ShowMe, FALSE);
            set(VG_SHADES   , MUIA_ShowMe, FALSE);
            set(VG_PATTERNS , MUIA_ShowMe, TRUE);
            set(DUM_PALETTE , MUIA_ShowMe, FALSE);
            set(BUT_FILCOL[cur_fillcolor], MUIA_Selected, TRUE);
            set(BUT_FPATTERNS[0], MUIA_Selected, TRUE);
            break;
        case 3:
            /* Gradient Fill style selected */
            cur_fillcolor = (NUM_STD_COLS + MAX_USR_COLS);
            cur_fillstyle = 20;
			if (GradientTable[0] == NULL) Init_Gradients();
			set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[0]);
            set(SG_FILPAL   , MUIA_ShowMe, FALSE);
            set(VG_GRADIENT , MUIA_ShowMe, TRUE);
            set(VG_SHADES   , MUIA_ShowMe, FALSE);
            set(VG_PATTERNS , MUIA_ShowMe, FALSE);
            set(DUM_PALETTE , MUIA_ShowMe, FALSE);
            break;
    }
    DoMethod(FilPanel, MUIM_Group_ExitChange);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoAlignMutex                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoAlignMutex,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoAlignMutex(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	int i;

    if (Arg[0] == 1)    /* Horizontal align mode*/
    {
        for (i=0; i < 4; i++)
        {
            if (Object != HAlignButtons[i].obj)
                DoMethod(HAlignButtons[i].obj, MUIM_NoNotifySet, MUIA_Selected, FALSE);
            else
                cur_halign = Arg[1];
        }
    }
    else                /* Vertical align mode*/
    {
        for (i=0; i < 4; i++)
        {
            if (Object != VAlignButtons[i].obj)
                DoMethod(VAlignButtons[i].obj, MUIM_NoNotifySet, MUIA_Selected, FALSE);
            else
                cur_valign = Arg[1];
        }
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
    
/*----------------------------------------------------------------------------*/
/*     DoEditMutex                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoEditMutex,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoEditMutex(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	int i;

    for (i=0; i < NUM_BUTTONS; i++)
    {
        if (Object != Buttons[i].obj)
            DoMethod(Buttons[i].obj, MUIM_NoNotifySet, MUIA_Selected, FALSE);
        else
        {
            /* update markers mask */
            new_objmask = Buttons[i].mask;
            if(setcenter)
            {
                center_marker(setcenter_x, setcenter_y);
                setcenter = 0;
            }
            
            cur_mode = Buttons[i].action;
			
			/* Turn ON/OFF specific parameters setting */
			set(AlignGRP, 			MUIA_ShowMe, (cur_mode == F_ALIGN));
            set(DepthGRP,           MUIA_ShowMe, (cur_mode != F_ALIGN)); /* turn off Depth GRP when align selected */
			set(RotAngleGRP, 		MUIA_ShowMe, (cur_mode == F_ROTATE));
			set(NumRegPolySidesGRP, MUIA_ShowMe, (cur_mode == F_REGPOLY));
			set(BoxCurveGRP, 		MUIA_ShowMe, (cur_mode == F_ARCBOX));
			set(EllAngleGRP, 		MUIA_ShowMe,((cur_mode == F_ELLIPSE_BY_RAD) || (cur_mode ==  F_ELLIPSE_BY_DIA)));
			set(ArcTypeGRP, 		MUIA_ShowMe, (cur_mode == F_CIRCULAR_ARC));

			/* Start Action */
			(*Buttons[i].function)();
            put_msg((char *)Buttons[i].msg);
            ActivateWindow(canvas_win);
        }
    }
    update_markers(new_objmask);
#ifdef __AROS__   
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoGradient                                                             */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoGradient,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoGradient(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
    LONG gradient=0;
    
    get(NUM_GRADIENT, MUIA_Numeric_Value, &gradient);
    cur_fillcolor =  gradient + (NUM_STD_COLS + MAX_USR_COLS);
    if (GradientTable[gradient]!=NULL)
    {
        set(GRD_PREVIEW, MUIA_ShowMe, TRUE);
        set(GRD_PREVIEW, MYATTR_GRAD, GradientTable[gradient]);
    }
    else
    {
        set(GRD_PREVIEW, MUIA_ShowMe, FALSE);
    }
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

void UpdateFShades(void)
{
	int i;
    ULONG rgb;
    extern void SetColorButtonRGB(APTR button, ULONG rgb);

	for (i=0; i<NUMSHADES; i++)
	{
	    rgb = getfillcolor(cur_fillcolor, i); 
        SetColorButtonRGB(BUT_FSHADES[i], rgb);
    }
    
}

void z_refreshAttributes(void)
{
    int i;
    
    /* Set Align mode */
    for (i=0;i<4;i++)
    {
        if (cur_halign == HAlignButtons[i].alignmode)
            set(HAlignButtons[i].obj, MUIA_Selected, TRUE);
        if (cur_valign == VAlignButtons[i].alignmode)
            set(VAlignButtons[i].obj, MUIA_Selected, TRUE);
    }   

    /* cur_rotnangle */
    set(NUM_ROTANGLE    , MUIA_Numeric_Value, cur_rotnangle);

    /* Depth */
    set(NUM_DEPTH       , MUIA_Numeric_Value, cur_depth);

    /* Line */
    set(NUM_LWIDTH      , MUIA_Numeric_Value, cur_linewidth);
    set(CYC_LINESTYLE   , MUIA_Cycle_Active , cur_linestyle);
    set(NUM_DOPGAP      , MUIA_Numeric_Value, cur_dotgap);
    set(NUM_DASHLEN     , MUIA_Numeric_Value, cur_dashlength);
    set(CYC_JOINSTYLE   , MUIA_Cycle_Active , cur_joinstyle);
    set(CYC_CAPSTYLE    , MUIA_Cycle_Active , cur_capstyle);

    /* Colors */
    set(BUT_PENCOL[cur_pencolor], MUIA_Selected, TRUE);
    set(BUT_FILCOL[cur_fillcolor], MUIA_Selected, TRUE);
    if ((cur_fillstyle < NUMSHADES) && (cur_fillstyle > -1))
    	set(BUT_FSHADES[cur_fillstyle], MUIA_Selected, TRUE);
    UpdateFShades();
   
    /* Text */
    set(NUM_FONTSIZE    , MUIA_Numeric_Value, cur_fontsize);
    set(NUM_FONTANGLE   , MUIA_Numeric_Value, cur_textangle);
    if(cur_textflags & RIGID_TEXT)
        set(CHK_TXTRIGID  , MUIA_Selected, 1L);
    else
        set(CHK_TXTRIGID  , MUIA_Selected, 0L);
    if(cur_textflags & SPECIAL_TEXT)
        set(CHK_TXTSPECIAL  , MUIA_Selected, 1L);
    else
        set(CHK_TXTSPECIAL  , MUIA_Selected, 0L);  
    if (cur_textflags & PSFONT_TEXT)
        set(LST_TXTFONT   , MUIA_List_Active, cur_ps_font+1);
    else
        set(LST_TXTFONT   , MUIA_List_Active, cur_latex_font + NUM_FONTS + 1);
        
    /* Misc */
    set(CYC_ARCTYPE     , MUIA_Cycle_Active , cur_arctype);
    set(NUM_ELLIANGLE   , MUIA_Numeric_Value, cur_elltextangle);
    set(CYC_ARROWTYPE   , MUIA_Cycle_Active , cur_arrowtype);
    set(NUM_BOXRAD      , MUIA_Numeric_Value, cur_boxradius);
}

void TurnOffEditButtons(void)
{
	int i;

    for (i=0; i < NUM_BUTTONS; i++)
    {
        DoMethod(Buttons[i].obj, MUIM_NoNotifySet, MUIA_Selected, FALSE);
    }
}

void Edit_ChangeSkin(void)
{
	int i;
    UBYTE iconfile[128];
    
    for (i=0; i < NUM_BUTTONS; i++)
    {
		sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,Buttons[i].image);
		set(Buttons[i].obj, MYATTR_IMAGEFILE, iconfile);
		
    }
    for (i=0; i < 4; i++)
    {
		sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,HAlignButtons[i].image);
		set(HAlignButtons[i].obj, MYATTR_IMAGEFILE, iconfile);
		sprintf((char *)iconfile,"PROGDIR:Skins/%s/%s",appres.skin,VAlignButtons[i].image);
		set(VAlignButtons[i].obj, MYATTR_IMAGEFILE, iconfile);
    }
}

void Update_MouseAssigns(char *LeftBut, char *MiddleBut, char *RightBut, char *S_LeftBut, char *S_MiddleBut, char*S_RightBut)
{
    #define MOUSE_ASSIGN_MAX	32
    static char	mousefun_l[MOUSE_ASSIGN_MAX];
    static char	mousefun_m[MOUSE_ASSIGN_MAX];
    static char	mousefun_r[MOUSE_ASSIGN_MAX];
    static char	mousefun_sh_l[MOUSE_ASSIGN_MAX];
    static char	mousefun_sh_m[MOUSE_ASSIGN_MAX];
    static char	mousefun_sh_r[MOUSE_ASSIGN_MAX];

    strncpy(mousefun_l   , LeftBut    , MOUSE_ASSIGN_MAX);
    strncpy(mousefun_m   , MiddleBut  , MOUSE_ASSIGN_MAX);
    strncpy(mousefun_r   , RightBut   , MOUSE_ASSIGN_MAX);
    strncpy(mousefun_sh_l, S_LeftBut  , MOUSE_ASSIGN_MAX);
    strncpy(mousefun_sh_m, S_MiddleBut, MOUSE_ASSIGN_MAX);
    strncpy(mousefun_sh_r, S_RightBut , MOUSE_ASSIGN_MAX);

    set(TEXT_L_MOUSE , MUIA_Text_Contents, mousefun_l);
    set(TEXT_M_MOUSE , MUIA_Text_Contents, mousefun_m);
    set(TEXT_R_MOUSE , MUIA_Text_Contents, mousefun_r);
    set(TEXT_SL_MOUSE, MUIA_Text_Contents, mousefun_sh_l);
    set(TEXT_SM_MOUSE, MUIA_Text_Contents, mousefun_sh_m);
    set(TEXT_SR_MOUSE, MUIA_Text_Contents, mousefun_sh_r);
}
