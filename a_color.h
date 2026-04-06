/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_color.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
extern void setPenColor(int value);
extern void setFillColor(int value);
extern void setFillStyle(int value);
extern void CheckUsedColors(F_compound *objects, BOOL Compound);
extern int  Add_UserColor(int indx, ULONG RGB);
extern void Init_UserColors(void);
extern ULONG getfillcolor(Color BaseColor, int shade);
