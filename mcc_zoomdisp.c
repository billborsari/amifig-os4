/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_zoomdisp.c $
 *       $Revision: 343 $
 *       $Date: 2017-02-23 20:01:02 +0000 (Thu, 23 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
/***************************************************************************/
/* This class display a ZOOM area.                                         */
/***************************************************************************/
#include <libraries/mui.h>
#include <proto/intuition.h>
#ifndef __amigaos4__
#include <proto/alib.h>
#endif
#include <proto/graphics.h>
#include <proto/muimaster.h>
#include <proto/utility.h>
#ifdef __AROS__
#include <proto/arossupport.h>
#endif

#include "fig.h"

#include "mcc_zoomdisp.h"

#define		min2(a, b)	(((a) < (b)) ? (a) : (b))
#define		max2(a, b)	(((a) > (b)) ? (a) : (b))

void  ReAllocateData(struct ZoomData *data, LONG W, LONG H)
{
    LONG depth;
    
    if (data->srcwin && data->srcwin->RPort)
    {
        if (data->tmprp == NULL) {
        	#ifdef __AROS__
        	data->tmprp = CreateRastPort();
        	#else
        	data->tmprp = AllocMem(sizeof (struct RastPort), MEMF_ANY);
        	InitRastPort(data->tmprp);
        	#endif
        }	
        data->dstW = W;
        data->dstH = H;
        data->srcW = W * 100 / data->ratio;
        data->srcH = H * 100 / data->ratio;
        depth     = GetBitMapAttr(data->srcwin->RPort->BitMap, BMA_DEPTH);

        data->ScaleX = ScalerDiv(data->srcW , (W + data->srcW - 1)  / data->srcW, 1);
        data->ScaleY = ScalerDiv(data->srcH , (H + data->srcH - 1)  / data->srcH, 1);

        if (data->tmpbm)        FreeBitMap(data->tmpbm);
        if (data->scaledbm)     FreeBitMap(data->scaledbm);

        data->tmpbm     = AllocBitMap(data->srcW , data->srcH, depth, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, data->srcwin->RPort->BitMap);
        data->scaledbm  = AllocBitMap(data->ScaleX, data->ScaleY, depth, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, data->srcwin->RPort->BitMap);
        /* Assign BitMap to RastPort */
        data->tmprp->BitMap = data->tmpbm;

        /* Build ScaleArgs struct */
        data->ScaleArgs.bsa_SrcWidth	= data->srcW ;
        data->ScaleArgs.bsa_SrcHeight	= data->srcH;
        data->ScaleArgs.bsa_XSrcFactor	= 1;
        data->ScaleArgs.bsa_YSrcFactor	= 1;
        data->ScaleArgs.bsa_XDestFactor	= (W + data->srcW  - 1) / data->srcW;
        data->ScaleArgs.bsa_YDestFactor	= (H + data->srcH - 1)  / data->srcH;
        data->ScaleArgs.bsa_SrcBitMap	= data->tmpbm;
        data->ScaleArgs.bsa_DestBitMap 	= data->scaledbm;
    }
}
        
IPTR ZoomNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct ZoomData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
		return(0);

	data = INST_DATA(cl,obj);
	
	data->srcwin   = NULL;
	data->tmprp    = NULL;
	data->tmpbm    = NULL;
	data->scaledbm = NULL;
	data->srcx     = 0;
	data->srcy     = 0;
	data->ratio    = 400;
    data->ScaleX   = 0;
    data->ScaleY   = 0;
    data->dstW     = 100;
    data->dstH     = 100;
    
	/* parse initial taglist */
	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_SRCWIN:
				data->srcwin = (struct Window*)(tag->ti_Data);
				break;
			case MYATTR_SRCX:
				data->srcx = (LONG)(tag->ti_Data);
				break;
			case MYATTR_SRCY:
				data->srcy = (LONG)(tag->ti_Data);
				break;
			case MYATTR_RATIO:
				data->ratio = (LONG)(tag->ti_Data);
				break;
		}
	}

	return((IPTR)obj);
}



IPTR ZoomDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct ZoomData *data = INST_DATA(cl,obj);

	if (data->tmpbm) 	FreeBitMap(data->tmpbm);
	if (data->scaledbm) FreeBitMap(data->scaledbm);
	if (data->tmprp) {
			#ifdef __AROS__
			FreeRastPort(data->tmprp);
			#else
			FreeMem (data->tmprp, sizeof (struct RastPort));
			data->tmprp = NULL;
			#endif
	}		
	data->tmprp 	= NULL;	
	data->tmpbm 	= NULL;
	data->scaledbm 	= NULL;
	data->srcwin 	= NULL;
    
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_SET method, we need to see if someone changed the gradient attribute.
*/

IPTR ZoomSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct ZoomData *data = INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_SRCWIN:
				data->srcwin = (struct Window*)(tag->ti_Data);
                ReAllocateData(data, data->dstW, data->dstH);
				break;
			case MYATTR_SRCX:
				data->srcx = (LONG)(tag->ti_Data);
				break;
			case MYATTR_SRCY:
				data->srcy = (LONG)(tag->ti_Data);
				break;
			case MYATTR_RATIO:
				data->ratio = (LONG)(tag->ti_Data);
                ReAllocateData(data, data->dstW, data->dstH);
				break;
		}
	}
    
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_GET method, see if someone wants to read the color.
*/

static IPTR ZoomGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct ZoomData *data = INST_DATA(cl,obj);
	IPTR *store = (IPTR *)((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MYATTR_SRCWIN:
			*store = (IPTR)(data->srcwin);
			return(TRUE);
			break;
		case MYATTR_SRCX:
			*store = (IPTR)data->srcx;
			return(TRUE);
			break;
		case MYATTR_SRCY:
			*store = (IPTR)data->srcy;
			return(TRUE);
			break;
		case MYATTR_RATIO:
			*store = (IPTR)data->ratio;
			return(TRUE);
			break;
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}

SAVEDS IPTR ZoomAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
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
	msg->MinMaxInfo->DefWidth  += 200;
	msg->MinMaxInfo->MaxWidth  += 600;

	msg->MinMaxInfo->MinHeight += 100;
	msg->MinMaxInfo->DefHeight += 200;
	msg->MinMaxInfo->MaxHeight += 600;

	return((IPTR)0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS IPTR ZoomDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct ZoomData 	   *data = INST_DATA(cl,obj);

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
		if (data->srcwin && data->srcwin->RPort)
		{
			if ((data->dstW != _mwidth(obj)) || (data->dstH != _mheight(obj)))
            {
                ReAllocateData(data, _mwidth(obj), _mheight(obj));
            }
            						
			if (data->tmpbm && data->tmprp && data->scaledbm)
			{
                LONG srcx, srcy, offsetX, offsetY;

                srcx = data->srcx;
                srcy = data->srcy;        
                
                if(srcx - data->srcW / 2 > 0)
				{
					srcx -= data->srcW / 2;
				}
                else
				{
					srcx=0;
				}
                if(srcx+data->srcW  > data->srcwin->GZZWidth)
				{
					srcx = data->srcwin->GZZWidth - data->srcW;
                }
				
                if(srcy - data->srcH / 2 > 0)
				{
					srcy -= data->srcH / 2;
                }
				else
				{
					srcy=0;
				}
                if(srcy+data->srcH > data->srcwin->GZZHeight)
				{
					srcy = data->srcwin->GZZHeight - data->srcH;
				}
                
                offsetX = min2(max2(0,(data->srcx - srcx)),data->srcW);
                offsetY = min2(max2(0,(data->srcy - srcy)),data->srcH);


                /* copy source to local bitmap */
				ClipBlit(data->srcwin->RPort, srcx, srcy, data->tmprp, 0, 0, data->srcW, data->srcH, 0xC0);
				/* Draw lines showing current pointed point */
				SetDrMd(data->tmprp, COMPLEMENT);
				SetAPen(data->tmprp, 1);
				Move(data->tmprp, offsetX, 0);
				Draw(data->tmprp, offsetX, data->srcH - 1);
				Move(data->tmprp, 0,  offsetY);
				Draw(data->tmprp, data->srcW - 1, offsetY);
				SetDrMd(data->tmprp, JAM1);
				/* scale */
				BitMapScale(&data->ScaleArgs);
				/* And finally blit to destination */
				BltBitMapRastPort(  data->scaledbm, 0, 0, _rp(obj), _mleft(obj), _mtop(obj), 
                                    _mwidth(obj), _mheight(obj), 0xC0);
			}
		}
	}
	return((IPTR)0);
}


/*
** Here comes the dispatcher for our custom class. We only need to
** care about MUIM_AskMinMax and MUIM_Draw in this simple case.
** Unknown/unused methods are passed to the superclass immediately.
*/

#ifdef __AROS__
AROS_UFH3(IPTR, ZoomDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR ZoomDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

	switch (msg->MethodID)
	{
		case OM_NEW        : return(ZoomNew      (cl,obj,(APTR)msg));
		case OM_DISPOSE    : return(ZoomDispose  (cl,obj,(APTR)msg));
		case OM_SET        : return(ZoomSet      (cl,obj,(APTR)msg));
		case OM_GET        : return(ZoomGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(ZoomAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(ZoomDraw     (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
