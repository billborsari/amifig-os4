/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_imagedisp.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
/***************************************************************************/
#include "fig.h"
#define StrDup strdup
#include <proto/exec.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#ifndef __amigaos4__
#include <proto/alib.h>
#endif
#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/utility.h>
#include <libraries/mui.h>
#include <datatypes/pictureclass.h>
#include <string.h>

#include "mcc_imagedisp.h"

static void LoadImageFile(struct ImageData *data)
{
	Object *o;
	struct BitMapHeader *bmhd;
	struct pdtBlitPixelArray bpa;
	ULONG i;

	if ((o = NewDTObject((char *) data->sourcefile,
				DTA_GroupID, GID_PICTURE,
				OBP_Precision, PRECISION_IMAGE,
				PDTA_Remap, FALSE,
				PDTA_DestMode, PMODE_V43,
				TAG_END)))
	{

		GetDTAttrs(	o, PDTA_BitMapHeader, &bmhd, TAG_END);

		if (bmhd) 
		{
			data->positive_image = AllocVec(bmhd->bmh_Width * bmhd->bmh_Height * 4, MEMF_ANY);
			data->negative_image = AllocVec(bmhd->bmh_Width * bmhd->bmh_Height * 4, MEMF_ANY);
			
			if (data->positive_image && data->negative_image)
			{
				bpa.MethodID = PDTM_READPIXELARRAY;
				bpa.pbpa_PixelData = data->positive_image;
				bpa.pbpa_PixelFormat = PBPAFMT_ARGB;
				bpa.pbpa_PixelArrayMod = bmhd->bmh_Width * 4;
				bpa.pbpa_Left = 0;
				bpa.pbpa_Top = 0;
				bpa.pbpa_Width = bmhd->bmh_Width;
				bpa.pbpa_Height = bmhd->bmh_Height;
						
				DoMethodA( o, (Msg)&bpa );
				
				for (i=0; i < (bmhd->bmh_Width * bmhd->bmh_Height); i++)
				{
					data->negative_image[i * 4 + 1] = ~data->positive_image[i * 4 + 1];
					data->negative_image[i * 4 + 2] = ~data->positive_image[i * 4 + 2];
					data->negative_image[i * 4 + 3] = ~data->positive_image[i * 4 + 3];					
				}

                if (bmhd->bmh_Depth<24)
                    for (i=0; i < (bmhd->bmh_Height * bmhd->bmh_Width); i++)
					{
                        data->positive_image[i * 4] = 255;
                        data->negative_image[i * 4] = 255;
					}
                else
                    for (i=0; i < (bmhd->bmh_Height * bmhd->bmh_Width); i++)
					{
                        data->negative_image[i * 4] = data->positive_image[i * 4];
					}
               
                    
				data->Width = bmhd->bmh_Width;
				data->Height = bmhd->bmh_Height;
			}
			else
			{
				if (data->positive_image) FreeVec(data->positive_image);
                data->positive_image = NULL;
				if (data->negative_image) FreeVec(data->negative_image);
                data->negative_image = NULL;
			}
		}
		DisposeDTObject(o);
	}	
}


IPTR ImageNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct ImageData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
		return(0);

	data = INST_DATA(cl,obj);
	
	data->sourcefile		= NULL;
	data->positive_image	= NULL;
	data->negative_image	= NULL;
	data->Width				= 0;
	data->Height			= 0;
	
	/* parse initial taglist */
	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_IMAGEFILE:
				if (tag->ti_Data)
				{
                    data->sourcefile = StrDup((CONST_STRPTR)tag->ti_Data);;
					LoadImageFile(data);
				}
				break;
		}
	}

	return((IPTR)obj);
}



IPTR ImageDispose(struct IClass *cl,Object *obj,Msg msg)
{
	struct ImageData *data = INST_DATA(cl,obj);
	
	if (data->positive_image)	FreeVec(data->positive_image);
	if (data->negative_image)	FreeVec(data->negative_image);
	if (data->sourcefile)		free(data->sourcefile);
	
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_SET method, change the picture.
*/

IPTR ImageSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct ImageData *data = INST_DATA(cl,obj);
	struct TagItem *tags,*tag;

	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_IMAGEFILE:
				if (tag->ti_Data)
				{
					/* We need to free up the memory and reload the new picture */
					if (data->positive_image)	FreeVec(data->positive_image);
					if (data->negative_image)	FreeVec(data->negative_image);
					if (data->sourcefile)		free(data->sourcefile);
					
					/* Copy new source file name */
					data->sourcefile = StrDup((CONST_STRPTR)tag->ti_Data);
					
					/* And load the image */
					LoadImageFile(data);

					/*Finally redraw ourselves completely */
					MUI_Redraw(obj,MADF_DRAWOBJECT);
				}
				break;
		}
	}
	
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_GET method, see if someone wants to read the color.
*/

static IPTR ImageGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct ImageData *data = INST_DATA(cl,obj);
	IPTR *store = (IPTR *)((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MYATTR_IMAGEFILE:
			*store = (IPTR)(data->sourcefile);
			return(TRUE);
			break;
		case MYATTR_IMAGEWIDTH:
			*store = (IPTR)(data->Width);
			return(TRUE);
			break;
		case MYATTR_IMAGEHEIGHT:
			*store = (IPTR)(data->Height);
			return(TRUE);
			break;
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}

SAVEDS IPTR ImageAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
{
	struct ImageData *data = INST_DATA(cl,obj);
	/*
	** let our superclass first fill in what it thinks about sizes.
	** this will e.g. add the size of frame and inner spacing.
	*/

	DoSuperMethodA(cl,obj,(APTR)msg);

	/*
	** now add the values specific to our object. note that we
	** indeed need to *add* these values, not just set them!
	*/

	msg->MinMaxInfo->MinWidth  += data->Width;
	msg->MinMaxInfo->DefWidth  += data->Width;
	msg->MinMaxInfo->MaxWidth  += data->Width;

	msg->MinMaxInfo->MinHeight += data->Height;
	msg->MinMaxInfo->DefHeight += data->Height;
	msg->MinMaxInfo->MaxHeight += data->Height;

	return((IPTR)0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS IPTR ImageDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct ImageData *data = INST_DATA(cl,obj);
    ULONG                   selected = 0;
	
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
        get(obj, MUIA_Selected, &selected);
        
        if (selected)
        {
            if (data->negative_image != NULL)
            {
                WritePixelArrayAlpha(	(APTR) data->negative_image,
                                        0, 0, data->Width * 4,
                                        _rp(obj),
                                        _mleft(obj), _mtop(obj),
                                        data->Width, data->Height,
                                        0xFFFFFFFF);
            }
        }
        else
        {
            if (data->positive_image != NULL)
            {
                WritePixelArrayAlpha(	(APTR) data->positive_image,
                                        0, 0, data->Width * 4,
                                        _rp(obj),
                                        _mleft(obj), _mtop(obj),
                                        data->Width, data->Height,
                                        0xFFFFFFFF);
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
AROS_UFH3(IPTR, ImageDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR ImageDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

	switch (msg->MethodID)
	{
		case OM_NEW        : return(ImageNew      (cl,obj,(APTR)msg));
		case OM_DISPOSE    : return(ImageDispose  (cl,obj,(APTR)msg));
		case OM_SET        : return(ImageSet      (cl,obj,(APTR)msg));
		case OM_GET        : return(ImageGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(ImageAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(ImageDraw     (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
