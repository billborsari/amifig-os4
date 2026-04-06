/*
 *       AmiFIG - Custom Color Swatch MUI class
 *
 *       Renders a colored rectangle directly via RPTAG_APenColor (truecolor),
 *       bypassing MUI's ColorfieldObject which allocates pens from the screen
 *       colormap and runs out when many instances are created.
 *
 *       AROS Public License
 */

#include "fig.h"

#include <proto/muimaster.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <libraries/mui.h>

#include "mcc_colorswatch.h"

/* ─── OM_NEW ───────────────────────────────────────────────────────────── */

static ULONG mColorSwatch_New(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ColorSwatchData *data;
    struct TagItem *tag;

    obj = (Object *)DoSuperMethodA(cl, obj, (Msg)msg);
    if (!obj) return 0;

    data = INST_DATA(cl, obj);
    data->rgb = 0x00808080;  /* default grey */

    tag = FindTagItem(MUIA_ColorSwatch_RGB, msg->ops_AttrList);
    if (tag)
        data->rgb = (ULONG)(tag->ti_Data & 0x00FFFFFF);

    return (ULONG)obj;
}

/* ─── OM_SET ───────────────────────────────────────────────────────────── */

static ULONG mColorSwatch_Set(struct IClass *cl, Object *obj, struct opSet *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);
    struct TagItem *tag;

    tag = FindTagItem(MUIA_ColorSwatch_RGB, msg->ops_AttrList);
    if (tag)
    {
        data->rgb = (ULONG)(tag->ti_Data & 0x00FFFFFF);
        MUI_Redraw(obj, MADF_DRAWUPDATE);
    }

    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/* ─── OM_GET ───────────────────────────────────────────────────────────── */

static ULONG mColorSwatch_Get(struct IClass *cl, Object *obj, struct opGet *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);

    if (msg->opg_AttrID == MUIA_ColorSwatch_RGB)
    {
        *(msg->opg_Storage) = data->rgb;
        return TRUE;
    }
    return DoSuperMethodA(cl, obj, (Msg)msg);
}

/* ─── MUIM_AskMinMax ───────────────────────────────────────────────────── */

static ULONG mColorSwatch_AskMinMax(struct IClass *cl, Object *obj,
                                     struct MUIP_AskMinMax *msg)
{
    DoSuperMethodA(cl, obj, (Msg)msg);

    msg->MinMaxInfo->MinWidth  += 8;
    msg->MinMaxInfo->MinHeight += 8;
    msg->MinMaxInfo->DefWidth  += 16;
    msg->MinMaxInfo->DefHeight += 16;
    msg->MinMaxInfo->MaxWidth  += MUI_MAXMAX;
    msg->MinMaxInfo->MaxHeight += MUI_MAXMAX;

    return 0;
}

/* ─── MUIM_Draw ────────────────────────────────────────────────────────── */

static ULONG mColorSwatch_Draw(struct IClass *cl, Object *obj,
                                struct MUIP_Draw *msg)
{
    struct ColorSwatchData *data = INST_DATA(cl, obj);
    struct RastPort *rp;
    LONG left, top, right, bottom;

    DoSuperMethodA(cl, obj, (Msg)msg);

    if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE)))
        return 0;

    rp     = _rp(obj);
    left   = _mleft(obj);
    top    = _mtop(obj);
    right  = _mright(obj);
    bottom = _mbottom(obj);

    /* Direct truecolor fill — no pen allocation needed */
    SetRPAttrs(rp, RPTAG_APenColor, data->rgb, TAG_DONE);
    RectFill(rp, left, top, right, bottom);

    return 0;
}

/* ─── Dispatcher ───────────────────────────────────────────────────────── */

#ifdef __AROS__
AROS_UFH3(IPTR, ColorSwatchDispatcher,
    AROS_UFHA(Class  *, cl,  A0),
    AROS_UFHA(Object *, obj, A2),
    AROS_UFHA(Msg     , msg, A1))
{
    AROS_USERFUNC_INIT
#else
IPTR ColorSwatchDispatcher(Class *cl, Object *obj, Msg msg)
{
#endif
    switch (msg->MethodID)
    {
        case OM_NEW:         return mColorSwatch_New(cl, obj, (struct opSet *)msg);
        case OM_SET:         return mColorSwatch_Set(cl, obj, (struct opSet *)msg);
        case OM_GET:         return mColorSwatch_Get(cl, obj, (struct opGet *)msg);
        case MUIM_AskMinMax: return mColorSwatch_AskMinMax(cl, obj, (struct MUIP_AskMinMax *)msg);
        case MUIM_Draw:      return mColorSwatch_Draw(cl, obj, (struct MUIP_Draw *)msg);
    }
    return DoSuperMethodA(cl, obj, msg);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
