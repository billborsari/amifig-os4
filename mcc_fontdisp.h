/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_fontdisp.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#define MYATTR_FONT 	0x22220000
#define MYATTR_FONTSIZE 0x22220001

struct FontData
{
	struct TextAttr fontattr;
};
#ifdef __AROS__
AROS_UFP3(IPTR, FontDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR FontDispatcher(Class *cl, Object *obj, Msg msg);
#endif
