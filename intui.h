/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/intui.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifdef __amigaos4__
	#define CreateStdIO(p) ((struct IOStdReq *) CreateIORequest((p),sizeof(struct IOStdReq)))
	#define DeleteStdIO(io) DeleteIORequest((struct IORequest *)(io))

	#define CreateExtIO(port,size) CreateIORequest((port),(size))
	#define DeleteExtIO(ior)	DeleteIORequest((struct IORequest *)(ior))

	#define RPTAG_FgColor RPTAG_APenColor
	#define RPTAG_BgColor RPTAG_BPenColor
	#define RPTAG_PenMode TAG_IGNORE

	#define GET(obj,attr,store) get(obj,attr,store)
#endif

extern struct Screen  *Scr;
extern UBYTE          *PubScreenName;

extern struct NewMenu *AmiFIGNM;
extern struct Menu    *ZFig_Menus;

extern BOOL ReopenScreen(void);
extern void CloseAll(void);
extern BOOL OpenAll(void);

/* DoFileRequest  		int mode */
#define FR_FIGDATA  	0x00000001
#define FR_FIGPREFS 	0x00000002
#define FR_EXPORT  		0x00000004

#define FR_DOLOAD 		0x00001000
#define FR_DOSAVE 		0x00002000

#define FR_MODEMASK		(FR_FIGDATA | FR_FIGPREFS | FR_EXPORT)
#define FR_ACTMASK		(FR_DOLOAD | FR_DOSAVE)
extern BOOL DoFileRequest(UBYTE *Title, UBYTE *Buffer, int mode);
