/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_coords.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#ifndef COORDS_H_INCLUDED
#define COORDS_H_INCLUDED

extern int xshift, yshift;
extern int last_xshift, last_yshift;

extern void update_scroller(BOOL nullpos);
extern void get_pagesizes(float *pagex, float *pagey);
extern int  get_xshift(void);
extern int  get_yshift(void);
extern void syncScrollHorizontal(void);
extern void syncScrollVertical(void);
extern void jumpHorizontal(float factor);
extern void jumpVertical(float factor);
extern void zoom_display(int x, int y, float factor);
#endif // COORDS_H_INCLUDED
