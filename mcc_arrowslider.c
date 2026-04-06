/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_arrowslider.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
/***************************************************************************/
#include "fig.h"
#include <proto/exec.h>                         
#include <proto/muimaster.h>
#include <proto/intuition.h>
#ifndef __amigaos4__
#include <proto/alib.h>
#endif
#include <libraries/mui.h>
#include <proto/utility.h>

#include "mcc_arrowslider.h"

#ifdef __amigaos4__
#include <stdarg.h>
Object *VARARGS68K DoSuperNew(struct IClass *cl, Object *obj,...)
{
	Object *rc;
	va_list args;

	va_startlinear(args,obj);
	rc = (Object *)DoSuperMethod(cl,obj,OM_NEW,va_getlinearva(args,ULONG),NULL);
	va_end(args);
	
	return rc;
}
#endif


IPTR ArrowSlider__OM_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ArrowSliderData *data;
    int horiz = GetTagData(MUIA_Group_Horiz, 0, msg->ops_AttrList);

    Object *slider = MUI_NewObject(MUIC_Slider, SliderFrame, MUIA_Prop_Horiz, horiz,
				 TAG_MORE, msg->ops_AttrList);

#ifdef __AROS__
    obj = (Object *)DoSuperNewTags(cl, obj, NULL,
			       MUIA_Group_Spacing, 0,
			       MUIA_Background, MUII_GroupBack,
			       TAG_MORE, (IPTR) msg->ops_AttrList);
#else
		obj = (Object *)DoSuperNew(cl, obj,
			       MUIA_Group_Spacing, 0,
			       MUIA_Background, MUII_GroupBack,
			       TAG_MORE, (IPTR) msg->ops_AttrList);			       			       
#endif		

    if (!obj)
	return FALSE;

    data = INST_DATA(cl, obj);
    data->slider = slider;

	data->up_arrow = ImageObject,
	    MUIA_Background, MUII_ButtonBack,
	    MUIA_Weight, 0,
	    ImageButtonFrame,
	    MUIA_InputMode, MUIV_InputMode_RelVerify,
	    MUIA_Image_Spec, horiz ? MUII_ArrowLeft : MUII_ArrowUp,
        MUIA_Image_FreeVert, TRUE,
	End;
	if (data->up_arrow)
	{
	    DoMethod(data->up_arrow, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
		     (IPTR)slider, 2, MUIM_Numeric_Decrease , 1);
    }

	data->down_arrow = ImageObject,
	    MUIA_Background, MUII_ButtonBack,
	    MUIA_Weight, 0,
	    ImageButtonFrame,
	    MUIA_InputMode, MUIV_InputMode_RelVerify,
	    MUIA_Image_Spec, horiz ? MUII_ArrowRight : MUII_ArrowDown,
        MUIA_Image_FreeVert, TRUE,
	End;
	if (data->down_arrow)
	{
	    DoMethod(data->down_arrow, MUIM_Notify, MUIA_Timer, MUIV_EveryTime,
		     (IPTR)slider, 2, MUIM_Numeric_Increase , 1);
    }

	DoMethod(obj, OM_ADDMEMBER, (IPTR)data->up_arrow);
	DoMethod(obj, OM_ADDMEMBER, (IPTR)data->slider);
	DoMethod(obj, OM_ADDMEMBER, (IPTR)data->down_arrow);

    /* Forward numeric value changes from slider to the group itself */
    DoMethod(data->slider, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
             (IPTR)obj, 3, MUIM_Set, MUIA_Numeric_Value, MUIV_TriggerValue);

    return (IPTR)obj;
}

IPTR ArrowSlider__OM_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ArrowSliderData *data = INST_DATA(cl, obj);
    struct TagItem *tag, *tstate;

    for (tstate = msg->ops_AttrList; (tag = NextTagItem(&tstate)); )
    {
        switch (tag->ti_Tag)
        {
            case MUIA_Numeric_Value:
            case MUIA_Numeric_Min:
            case MUIA_Numeric_Max:
                /* Forward these to the internal slider without triggering notifications to avoid loops */
                DoMethod(data->slider, MUIM_NoNotifySet, tag->ti_Tag, tag->ti_Data);
                break;
        }
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

IPTR ArrowSlider__OM_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct ArrowSliderData *data = INST_DATA(cl, obj);

    switch (msg->opg_AttrID)
    {
        case MUIA_Numeric_Value:
        case MUIA_Numeric_Min:
        case MUIA_Numeric_Max:
            /* Pull these from the internal slider */
            return get(data->slider, msg->opg_AttrID, msg->opg_Storage);
            
        default:
            return DoSuperMethodA(cl, obj, (Msg)msg);
    }
}

#ifdef __AROS__
AROS_UFH3(IPTR, ArrowSliderDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR ArrowSliderDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif    

    switch (msg->MethodID)
    {
		case OM_NEW:     return ArrowSlider__OM_NEW(cl, obj, (struct opSet *) msg);
        case OM_SET:     return ArrowSlider__OM_SET(cl, obj, (struct opSet *) msg);
        case OM_GET:     return ArrowSlider__OM_GET(cl, obj, (struct opGet *) msg);
        default:         return DoSuperMethodA(cl, obj, msg);
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
