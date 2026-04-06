/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_color.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef Z_COLORPANEL_H
#define Z_COLORPANEL_H

/*----------------------------------------------------------------------------*/
/*    Protos                                                                  */
/*----------------------------------------------------------------------------*/

APTR MakeColorWindow(void);
void z_color_methods(void);
void SetUsrColBut(int usrcol, ULONG value);
void Color_ChangeSkin(void);
void SetColorButtonRGB(APTR button, ULONG rgb);
void Init_UserColors(void);
void Init_Gradients(void);
void RefreshAllColorButtons(void);

/* UI synchronization areas */
#define UPDATE_PALETTES  (1L << 0)
#define UPDATE_SHADES    (1L << 1)
#define UPDATE_GRADIENTS (1L << 2)
#define UPDATE_ALL_COLOR_UI (UPDATE_PALETTES | UPDATE_SHADES | UPDATE_GRADIENTS)

/* Standardized sync functions */
void SyncPenPalette(int index);
void SyncFillPalette(int index);
void SyncUserPalette(int index);
void SyncShadeRamp(void);
void SyncGradientEditor(void);

void DispatchColorUIUpdate(ULONG area_mask, int index);

/*----------------------------------------------------------------------------*/
/*    Global Var                                                              */
/*----------------------------------------------------------------------------*/

#endif /* Z_COLORPANEL_H */
