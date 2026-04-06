/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_mode.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef Z_MODEPANEL_H
#define Z_MODEPANEL_H

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

APTR MakeModeWindow(void);
void z_mode_methods(void);
void z_mode_refresh(void);
void z_print_coords(int X, int Y);
void Mode_ChangeSkin(void);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

extern BOOL FreeHandBut;

#endif /* Z_MODEPANEL_H */
