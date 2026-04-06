/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_arrowslider.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
struct ArrowSliderData
{
    Object *slider;
    Object *up_arrow;
    Object *down_arrow;
};
#ifdef __AROS__
AROS_UFP3(IPTR, ArrowSliderDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR ArrowSliderDispatcher(Class *cl, Object *obj, Msg msg);
#endif
