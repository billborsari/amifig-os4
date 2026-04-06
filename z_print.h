/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_print.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef Z_PRINTPANEL_H
#define Z_PRINTPANEL_H

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

APTR MakePrintWindow(void);
void z_print_methods(void);
void z_updateprint(int left, int top, int width, int height);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

#endif /* Z_PRINTPANEL_H */
