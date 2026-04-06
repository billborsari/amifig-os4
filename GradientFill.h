/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/GradientFill.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
void GradientPolygonFill(struct RastPort *rastport, zXPoint *Points, int NbPoints, struct _Gradient *gradient, int xmin, int ymin, int xmax, int ymax);
void PolygonFillAlpha(struct RastPort *rastport, zXPoint *Points, int NbPoints, ULONG color);
