/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_rexxiface.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
extern BOOL initRexxIFace(void);
extern void closeRexxIFace(void);
extern ULONG getRexxSigBit(void);
extern void handleRexxMessage(ULONG SignalSet);

extern struct MsgPort *rexxport;
extern char portname[32];
