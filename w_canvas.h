/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_canvas.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#define MAXPOINTNUM 	32000 /* maximum number of points for polygons */
extern struct RastPort	*rastport;
extern struct Region	*clip_region;
extern struct Region 	*ref_clip_region;
extern struct Rectangle  paint_cliprect;
extern struct AreaInfo	 areainfo;
extern UBYTE 			 polygondata[5*(MAXPOINTNUM+1)];	/* for low level graphic functions */
extern struct Gadget    *horscroller, *vertscroller;


#define MAXRASTSIZE 	11560 /* 16MB equivalent to roughly 11560 x 11560 pixels */
extern struct TmpRas  	tmpras;
extern PLANEPTR 		tempraster;
extern int 				rastwidth,rastheight;

extern struct Window  *canvas_win;

extern int  setup_canvaswin(int context);
extern BOOL close_canvaswin(BOOL request);
extern void process_win_paint_signals(int i);

extern void ClearRect(WORD x, WORD y, WORD w, WORD h);

extern void setup_paint_region(void);
extern void delete_paint_region(void);
