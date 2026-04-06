/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_graddisp.c $
 *       $Revision: 343 $
 *       $Date: 2017-02-23 20:01:02 +0000 (Thu, 23 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/***************************************************************************/
/* This class display a gradient preview area.                             */
/***************************************************************************/
#include <libraries/mui.h>
#include <proto/muimaster.h>

#include "fig.h"
#include "resources.h"
#include "w_drawprim.h"
#include "GradientFill.h"
#include "mcc_graddisp.h"

static const UBYTE  checkerboard[32] =
{
        /* Plane 0 */
        0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,
        0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF
};                                                                           

IPTR mNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct GradData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
		return(0);

	data = INST_DATA(cl,obj);

	/* parse initial taglist */
	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_GRAD:
				if (tag->ti_Data)
				{
					CopyMem((struct _Gradient *)tag->ti_Data, &(data->gradient), sizeof(struct _Gradient));
					data->Grad_OK = TRUE;
				}
				else
					data->Grad_OK = FALSE;
				break;
		}
	}

	return((IPTR)obj);
}



IPTR mDispose(struct IClass *cl,Object *obj,Msg msg)
{
	/* OM_NEW didnt allocates something, just do nothing here... */
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_SET method, we need to see if someone changed the gradient attribute.
*/

IPTR mSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct GradData *data = INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_GRAD:
				if (tag->ti_Data)
				{
					CopyMem((struct _Gradient *)tag->ti_Data, &(data->gradient), sizeof(struct _Gradient));
					data->Grad_OK = TRUE;
					MUI_Redraw(obj,MADF_DRAWOBJECT); /* redraw ourselves completely */
				}
				else
					data->Grad_OK = FALSE;
				break;
		}
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_GET method, see if someone wants to read the color.
*/

static IPTR mGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct GradData *data = INST_DATA(cl,obj);
	IPTR *store = (IPTR *)((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MYATTR_GRAD: *store = (IPTR)&data->gradient; return(TRUE);
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}

SAVEDS IPTR mAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
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

	msg->MinMaxInfo->MinWidth  += 50;
	msg->MinMaxInfo->DefWidth  += 100;
	msg->MinMaxInfo->MaxWidth  += 200;

	msg->MinMaxInfo->MinHeight += 50;
	msg->MinMaxInfo->DefHeight += 100;
	msg->MinMaxInfo->MaxHeight += 200;

	return((IPTR)0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS IPTR mDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct GradData *data = INST_DATA(cl,obj);
	WORD    width, height, radiusX, radiusY, x1, x2, y1, y2;
	zXPoint  Points[4];
	
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

        /* ok, everything ready to render... */
        Points[0].x =	_mleft(obj);  Points[0].y = _mtop(obj);
        Points[1].x =	_mright(obj); Points[1].y = _mtop(obj);
        Points[2].x =	_mright(obj); Points[2].y = _mbottom(obj);
        Points[3].x =	_mleft(obj);  Points[3].y = _mbottom(obj);
        
        /* Start by drawing the background */
        SetDrMd( _rp(obj), JAM2);
        SetAfPt( _rp(obj), (UWORD *)checkerboard, 4);
        SetRPAttrs( _rp(obj),
                    RPTAG_PenMode, FALSE,
                    RPTAG_FgColor, 0x00FFFFFF,
                    RPTAG_BgColor, 0x00000000,
                    TAG_DONE);
        RectFill ( _rp(obj), _mleft(obj)+1, _mtop(obj)+1, _mright(obj)-1, _mbottom(obj)-1);
        SetRPAttrs( _rp(obj),
                    RPTAG_PenMode, TRUE,
                    TAG_DONE);
        SetAfPt( _rp(obj), (UWORD *)NULL, 0);
        
        /* Now draw the gradient */
        if(data->Grad_OK == TRUE)
        {
            GradientPolygonFill(_rp(obj), Points, 4, &(data->gradient), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj));
            
            /* overlay the gradient information */
            width  = _mright(obj) - _mleft(obj);
            height = _mbottom(obj) - _mtop(obj);
            x1 = _mleft(obj) + width * data->gradient.x1 / 100;
            y1 = _mtop(obj) + height * data->gradient.y1 / 100;
            x2 = _mleft(obj) + width * data->gradient.x2 / 100;
            y2 = _mtop(obj) + height * data->gradient.y2 / 100;
            SetDrMd(_rp(obj),COMPLEMENT);
            SetAPen(_rp(obj), 1);
            /* first control point */
            Move(_rp(obj), x1 - 3, y1);
            Draw(_rp(obj), x1 + 3, y1);
            Move(_rp(obj), x1    , y1 - 3);
            Draw(_rp(obj), x1    , y1 + 3);
            /* second control point */
            if (!((x1 == x2) && (y1 == y2)))
            {
                Move(_rp(obj), x2 - 3, y2);
                Draw(_rp(obj), x2 + 3, y2);
                Move(_rp(obj), x2    , y2 - 3);
                Draw(_rp(obj), x2    , y2 + 3);
            }
            if (data->gradient.type == 0)
            {
                /* Draw line between both points */
                Move(_rp(obj), x1, y1);
                Draw(_rp(obj), x2, y2);				
            }
            else
            {
                /* Draw circle to show radius */
                radiusX = width * data->gradient.radius / 100;
                radiusY = height * data->gradient.radius / 100;
                DrawEllipse(_rp(obj),
                            x1,
                            y1,
                            radiusX,	radiusY);
            }
            SetDrMd(_rp(obj),JAM1);
        }
        
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
AROS_UFH3(IPTR, GradDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR GradDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

	switch (msg->MethodID)
	{
		case OM_NEW        : return(mNew      (cl,obj,(APTR)msg));
		case OM_DISPOSE    : return(mDispose  (cl,obj,(APTR)msg));
		case OM_SET        : return(mSet      (cl,obj,(APTR)msg));
		case OM_GET        : return(mGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(mAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(mDraw     (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
