/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_figdisp.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#define MYATTR_COMPOUND 	0x22220040

struct FigData
{
	F_compound *compound;
};
#ifdef __AROS__
AROS_UFP3(IPTR, FigDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR FigDispatcher(Class *cl, Object *obj, Msg msg);
#endif
