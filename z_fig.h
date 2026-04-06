/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_fig.h $
 *       $Revision: 343 $
 *       $Date: 2017-02-23 20:01:02 +0000 (Thu, 23 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef ZFIG_H
#define ZFIG_H

/*----------------------------------------------------------------------------*/
/*    Macros                                                                  */
/*----------------------------------------------------------------------------*/

#define MAKEID(a,b,c,d) ((ULONG)(a)<<24|(ULONG)(b)<<16|(ULONG)(c)<<8|(ULONG)(d))

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

BOOL MakeMUIApp(void);
void CloseMUIApp(void);
void DoMenuItem(IPTR item);
APTR MakeCheckmark(UBYTE *label, LONG state);
APTR MakeButton(UBYTE *, UBYTE, UBYTE *);
APTR MakeToggle(UBYTE *, UBYTE, UBYTE *);
APTR MakeColorButton(ULONG rgb, LONG W, LONG H);
APTR MakeImageButton(UBYTE *, UBYTE, UBYTE *);
APTR MakeImageToggle(UBYTE *, UBYTE, UBYTE *);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

/* MUI Application */
extern APTR App;
/* MUI Windows */
extern APTR WinMode, WinEdit, WinColor, WinPrefs, WinZoom, WinPict, WinExport, WinAbout;
extern APTR WinPrint, WinLib, WinMsg;
/* MUI custom classes */
extern struct MUI_CustomClass *mcc_grad;
extern struct MUI_CustomClass *mcc_font;
extern struct MUI_CustomClass *mcc_zoom;
extern struct MUI_CustomClass *mcc_imag;
extern struct MUI_CustomClass *mcc_fig;
extern struct MUI_CustomClass *mcc_asl;
extern struct MUI_CustomClass *mcc_csw;

ULONG GetStringFromUser(STRPTR buffer, ULONG maxlen, STRPTR title);

#endif /* ZFIG_H */
