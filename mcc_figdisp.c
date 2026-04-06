/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_figdisp.c $
 *       $Revision: 343 $
 *       $Date: 2017-02-23 20:01:02 +0000 (Thu, 23 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/***************************************************************************/
/* This class display an xfig figure                                       */
/***************************************************************************/
#include <libraries/mui.h>
#include <proto/muimaster.h>

#include "fig.h"
#include "resources.h"
#include "a_coords.h"
#include "u_create.h"
#include "u_redraw.h"
#include "u_list.h"
#include "w_canvas.h"
#include "mcc_figdisp.h"

/* Local prototype */
void DisplayCompound(F_compound	*compound, struct RastPort *rp, int x, int y, int w, int h);


IPTR FigNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct FigData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
		return(0);

	data = INST_DATA(cl,obj);
	data->compound = NULL;
		
	/* parse initial taglist */
	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_COMPOUND:
				if (tag->ti_Data)
				{
                    data->compound = copy_compound((F_compound *)(tag->ti_Data));
				}
				break;
		}
	}

	return((IPTR)obj);
}



IPTR FigDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct FigData *data = INST_DATA(cl,obj);

    if (data->compound)
        delete_compound((F_compound *)data->compound);
        
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_SET method, change the picture.
*/

IPTR FigSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct FigData *data = INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_COMPOUND:
				if (tag->ti_Data)
				{
                    if (data->compound)
                        delete_compound(data->compound);
					data->compound = copy_compound((F_compound *)(tag->ti_Data));
					MUI_Redraw(obj,MADF_DRAWOBJECT); /* redraw ourselves completely */;
				}
				break;
		}
	}
	
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_GET method, see if someone wants to read the compound.
*/

static IPTR FigGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct FigData *data = INST_DATA(cl,obj);
	IPTR *store = (IPTR *)((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MYATTR_COMPOUND:
			*store = (IPTR)(data->compound);
			return(TRUE);
			break;
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}

SAVEDS IPTR FigAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	/*
	** let our superclass first fill in what it thinks about sizes.
	** this will e.g. add the size of frame and inner spacing.
	*/

	DoSuperMethodA(cl,obj,(APTR)msg);

	/*
	** now add the values specific to our object. note that we
	** indeed need to *add* these values, not just set them!
	*/

	msg->MinMaxInfo->MinWidth  += 100;
	msg->MinMaxInfo->DefWidth  += 150;
	msg->MinMaxInfo->MaxWidth  += 400;

	msg->MinMaxInfo->MinHeight += 100;
	msg->MinMaxInfo->DefHeight += 150;
	msg->MinMaxInfo->MaxHeight += 400;

	return((IPTR)0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS IPTR FigDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct FigData *data = INST_DATA(cl,obj);
	
	/*
	** let our superclass draw itself first, area class would
	** e.g. draw the frame and clear the whole region. What
	** it does exactly depends on msg->flags.
	*/

	DoSuperMethodA(cl,obj,(APTR)msg);

	/*
	** if MADF_DRAWOBJECT isn't set, we shouldn't draw anything.
	** MUI just wanted to update the frame or something like that.
	*/
	if (msg->flags & MADF_DRAWOBJECT)
	{
        APTR ClipHandler = NULL;
        
        ClipHandler = MUI_AddClipping( muiRenderInfo(obj), _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj));
		
		/* Clear area */
		SetDrMd( _rp(obj), JAM1);
		SetRPAttrs( _rp(obj),
					RPTAG_PenMode, FALSE,
					RPTAG_FgColor, 0x00FFFFFF,
					TAG_DONE);
		RectFill ( _rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj) );
		SetRPAttrs(_rp(obj), RPTAG_PenMode, TRUE, TAG_DONE);
		
		/* Display the compound */
		DisplayCompound(data->compound, _rp(obj), _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj));
		
		/* Restore clip region */
        MUI_RemoveClipping(muiRenderInfo(obj), ClipHandler);
    }
    return((IPTR)0);
}


/*
** Here comes the dispatcher for our custom class. We only need to
** care about MUIM_AskMinMax and MUIM_Draw in this simple case.
** Unknown/unused methods are passed to the superclass immediately.
*/
#ifdef __AROS__
AROS_UFH3(IPTR, FigDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR FigDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

	switch (msg->MethodID)
	{
		case OM_NEW        : return(FigNew      (cl,obj,(APTR)msg));
		case OM_DISPOSE    : return(FigDispose  (cl,obj,(APTR)msg));
		case OM_SET        : return(FigSet      (cl,obj,(APTR)msg));
		case OM_GET        : return(FigGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(FigAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(FigDraw     (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}


void DisplayCompound(F_compound	*compound, struct RastPort *rp, int x, int y, int w, int h)
{
    int				xmin, xmax, ymin, ymax;
    float			width, height, size;
    int				save_xshift, save_yshift;
    float			save_zoomscale;
    Boolean			save_shownums, save_OnScreenAA;
	struct RastPort *save_rp = rastport;
	int				pixsize = min2(w-10,h-10);
    		
	if (compound)
	{
		/* switch to preview rastport */
		rastport = rp;
        rastport->TmpRas   = &tmpras;
        rastport->AreaInfo = &areainfo;	

		/* first, save current zoom settings */
		save_zoomscale	= display_zoomscale;
		save_xshift	= xshift;
		save_yshift	= yshift;
		save_OnScreenAA = appres.OnScreenAA;
		appres.OnScreenAA = True;

		/* save and turn off showing vertex numbers */
		save_shownums	= appres.shownums;
		appres.shownums	= False;

		xmin = compound->nwcorner.x;
		ymin = compound->nwcorner.y;
		xmax = compound->secorner.x;
		ymax = compound->secorner.y;
		width  = xmax - xmin;
		height = ymax - ymin;
		size = max2(width,height)/ZOOM_FACTOR;

		/* scale to fit the preview canvas */
		display_zoomscale = min2(2.0,(float) (pixsize) / size);
		/* center the figure in the canvas */
		xshift = -x-5;
		yshift = -y-5;

		/* draw the object into the pixmap */
		redisplay_objects(compound, NULL);
		
		/* now restore settings for main canvas/figure */
		display_zoomscale	= save_zoomscale;
		xshift				= save_xshift;
		yshift				= save_yshift;
		appres.OnScreenAA 	= save_OnScreenAA;

		/* restore shownums */
		appres.shownums	= save_shownums;
		/* restore initial rastport */
		rastport = save_rp;
	}
}

