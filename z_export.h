/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_export.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef Z_EXPORTPANEL_H
#define Z_EXPORTPANEL_H

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

APTR MakeExportWindow(void);
void z_export_methods(void);
void z_updateexport(int left, int top, int width, int height);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

#endif /* Z_EXPORTPANEL_H */
