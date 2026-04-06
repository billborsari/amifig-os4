/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_edit.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef Z_EDITPANEL_H
#define Z_EDITPANEL_H

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

APTR MakeEditWindow(void);
void z_edit_methods(void);
void z_refreshAttributes(void);
void TurnOffEditButtons(void);
void Edit_ChangeSkin(void);
void Update_MouseAssigns(char *LeftBut, char *MiddleBut, char *RightBut, char *S_LeftBut, char *S_MiddleBut, char*S_RightBut);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

#endif /* Z_EDITPANEL_H */
