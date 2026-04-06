/*
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_print.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

extern void print_bitmap(int left, int top, int width, int height, int Density);
extern int  getPrinterDPI(int Density, int *xdpi, int *ydpi);
extern int  PrintToPS( void );
