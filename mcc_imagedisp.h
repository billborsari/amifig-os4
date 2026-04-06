/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/mcc_imagedisp.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#define MYATTR_IMAGEFILE 	0x22220030
#define MYATTR_IMAGEWIDTH 	0x22220031
#define MYATTR_IMAGEHEIGHT 	0x22220032

struct ImageData
{
	STRPTR	 sourcefile;
	UBYTE	*positive_image;
	UBYTE	*negative_image;
	WORD	 Width;
	WORD	 Height;
};
#ifdef __AROS__
AROS_UFP3(IPTR, ImageDispatcher,
    AROS_UFPA(Class  *, cl,  A0),
    AROS_UFPA(Object *, obj, A2),
    AROS_UFPA(Msg     , msg, A1));
#else
IPTR ImageDispatcher(Class *cl, Object *obj, Msg msg);
#endif
