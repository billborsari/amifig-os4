/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_fontdisp.c $
 *       $Revision: 343 $
 *       $Date: 2017-02-23 20:01:02 +0000 (Thu, 23 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
/***************************************************************************/
/* This class display a Font preview area.                                 */
/***************************************************************************/
#include "fig.h"
#include <libraries/mui.h>
#include <proto/muimaster.h>
#ifndef __amigaos4__
#include <proto/alib.h>
#endif

#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/diskfont.h>
#include <graphics/rpattr.h>

#include <stdio.h>
#include <string.h>

#include "mcc_fontdisp.h"

IPTR FontNew(struct IClass *cl,Object *obj,Msg msg)
{
	struct FontData *data;
	struct TagItem *tags,*tag;

	if (!(obj = (Object *)DoSuperMethodA(cl,obj,(APTR)msg)))
		return(0);

	data = INST_DATA(cl,obj);
	
	data->fontattr.ta_Name  = NULL;
	data->fontattr.ta_YSize = 12;
	data->fontattr.ta_Style = FS_NORMAL;
	data->fontattr.ta_Flags = 0;
	
	
	/* parse initial taglist */
	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_FONT:
				if (tag->ti_Data)
				{
					data->fontattr.ta_Name = (STRPTR)(tag->ti_Data);
				}
				break;
			case MYATTR_FONTSIZE:
				if (tag->ti_Data)
				{
					data->fontattr.ta_YSize = (LONG)(tag->ti_Data);
				}
				break;
		}
	}

	return((IPTR)obj);
}



IPTR FontDispose(struct IClass *cl,Object *obj,Msg msg)
{
	/* OM_NEW didnt allocates something, just do nothing here... */
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_SET method, we need to see if someone changed the gradient attribute.
*/

IPTR FontSet(struct IClass *cl,Object *obj,Msg msg)
{
	struct FontData *data = INST_DATA(cl,obj);
	struct TagItem *tags,*tag;
	BOOL   redraw = FALSE;

	for (tags=((struct opSet *)msg)->ops_AttrList;(tag=NextTagItem((APTR)&tags));)
	{
		switch (tag->ti_Tag)
		{
			case MYATTR_FONT:
				if (tag->ti_Data)
				{
					data->fontattr.ta_Name = (STRPTR)(tag->ti_Data);
					redraw = TRUE;
				}
				break;
			case MYATTR_FONTSIZE:
				if (tag->ti_Data)
				{
					data->fontattr.ta_YSize = (LONG)(tag->ti_Data);
					redraw = TRUE;
				}
				break;
		}
	}
	if (redraw) MUI_Redraw(obj,MADF_DRAWOBJECT); /* redraw ourselves completely */;
	return(DoSuperMethodA(cl,obj,(APTR)msg));
}


/*
** OM_GET method, see if someone wants to read the color.
*/

static IPTR FontGet(struct IClass *cl,Object *obj,Msg msg)
{
	struct FontData *data = INST_DATA(cl,obj);
	IPTR *store = (IPTR *)((struct opGet *)msg)->opg_Storage;

	switch (((struct opGet *)msg)->opg_AttrID)
	{
		case MYATTR_FONT:
			*store = (IPTR)&(data->fontattr.ta_Name);
			return(TRUE);
			break;
		case MYATTR_FONTSIZE:
			*store = (IPTR)data->fontattr.ta_YSize;
			return(TRUE);
			break;			
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
}

SAVEDS IPTR FontAskMinMax(struct IClass *cl,Object *obj,struct MUIP_AskMinMax *msg)
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

	msg->MinMaxInfo->MinHeight += 72 + 20;
	msg->MinMaxInfo->DefHeight += 72 + 20;
	msg->MinMaxInfo->MaxHeight += 72 + 20;

	return((IPTR)0);
}


/*
** Draw method is called whenever MUI feels we should render
** our object. This usually happens after layout is finished
** or when we need to refresh in a simplerefresh window.
** Note: You may only render within the rectangle
**       _mleft(obj), _mtop(obj), _mwidth(obj), _mheight(obj).
*/

SAVEDS IPTR FontDraw(struct IClass *cl,Object *obj,struct MUIP_Draw *msg)
{
	struct FontData *data = INST_DATA(cl,obj);
	APTR preview_font;
	char previewtext[128];
	int  tlength;
	
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
		/* Clear area */
		SetDrMd( _rp(obj), JAM1);
		SetRPAttrs( _rp(obj),
					RPTAG_PenMode, FALSE,
					RPTAG_FgColor, 0x00FFFFFF,
					TAG_DONE);
		RectFill ( _rp(obj), _mleft(obj), _mtop(obj), _mright(obj), _mbottom(obj) );
		
		/* Write preview text */
		if ((preview_font = OpenDiskFont(&(data->fontattr))))	
		{
			SetFont(_rp(obj), preview_font);
			sprintf(previewtext, "%s", data->fontattr.ta_Name);
			tlength = strlen(previewtext);
						
			SetRPAttrs( _rp(obj),
						RPTAG_PenMode, FALSE,
						RPTAG_FgColor, 0x00000000,
						TAG_DONE);
			Move( _rp(obj), _mleft(obj) + 2, _mbottom(obj) - 20);
			while (TextLength(_rp(obj), (CONST_STRPTR)previewtext, tlength) > (_mright(obj) - _mleft(obj) - 4)) previewtext[--tlength] = 0;
			Text( _rp(obj), (CONST_STRPTR)previewtext, tlength);
			CloseFont(preview_font);
		}
		SetRPAttrs(_rp(obj), RPTAG_PenMode, TRUE, TAG_DONE);
	}
	return((IPTR)0);
}


/*
** Here comes the dispatcher for our custom class. We only need to
** care about MUIM_AskMinMax and MUIM_Draw in this simple case.
** Unknown/unused methods are passed to the superclass immediately.
*/
#ifdef __AROS__
AROS_UFH3(IPTR, FontDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR FontDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

	switch (msg->MethodID)
	{
		case OM_NEW        : return(FontNew      (cl,obj,(APTR)msg));
		case OM_DISPOSE    : return(FontDispose  (cl,obj,(APTR)msg));
		case OM_SET        : return(FontSet      (cl,obj,(APTR)msg));
		case OM_GET        : return(FontGet      (cl,obj,(APTR)msg));
		case MUIM_AskMinMax: return(FontAskMinMax(cl,obj,(APTR)msg));
		case MUIM_Draw     : return(FontDraw     (cl,obj,(APTR)msg));
	}

	return(DoSuperMethodA(cl,obj,(APTR)msg));
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
