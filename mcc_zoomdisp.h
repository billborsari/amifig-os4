/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_zoomdisp.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#define MYATTR_SRCWIN 	0x22220020
#define MYATTR_SRCX     0x22220021
#define MYATTR_SRCY     0x22220022
#define MYATTR_RATIO    0x22220023

struct ZoomData
{
	struct Window       *srcwin;
	struct RastPort     *tmprp;
	struct BitMap	    *tmpbm;
	struct BitMap	    *scaledbm;
    struct BitScaleArgs	ScaleArgs;
	LONG		        srcx;
	LONG			    srcy;
    LONG                srcW;
    LONG                srcH;
	LONG			    ratio;
    WORD                ScaleX;
    WORD                ScaleY;
    LONG                dstW;
    LONG                dstH;
};
#ifdef __AROS__
AROS_UFP3(IPTR, ZoomDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR ZoomDispatcher(Class *cl, Object *obj, Msg msg);
#endif
