/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/const.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#define	PIX_PER_INCH		1200.0
#define	PIX_PER_CM			(PIX_PER_INCH / 2.54)

/* shorthand */
#define		PPI  PIX_PER_INCH
#define		PPCM PIX_PER_CM

#define	DISPLAY_PIX_PER_INCH 80.0
#define	DISPLAY_PIX_PER_CM	 (DISPLAY_PIX_PER_INCH/2.54)

#define PICT_PIX_PER_INCH	300 // original pictures dot per inch

#define	POINT_PER_INCH		72 // fig2dev Postscript resolution

#define ZOOM_FACTOR 		(float)(PIX_PER_INCH/DISPLAY_PIX_PER_INCH)

#define SCALE_DOWN(x)		round(zoomscale*display_zoomscale*(x))
#define SCALE_DOWN_F(x)		(float)(zoomscale*display_zoomscale*(x))

#define SCALE_DOWN_X(x)		(round(zoomscale*display_zoomscale*(x)) - xshift)
#define SCALE_DOWN_X_F(x)	(float)(zoomscale*display_zoomscale*(x) - xshift)

#define SCALE_DOWN_Y(y)		(round(zoomscale*display_zoomscale*(y)) - yshift)
#define SCALE_DOWN_Y_F(y)	(float)(zoomscale*display_zoomscale*(y) - yshift)

#define SCALE_UP(x)			round((ZOOM_FACTOR/display_zoomscale)*(x))
#define SCALE_UP_X(x)		round((ZOOM_FACTOR/display_zoomscale) * (x+xshift))
#define SCALE_UP_Y(y)		round((ZOOM_FACTOR/display_zoomscale) * (y+yshift))
#define SCALE_UP_F(x)		(float)((ZOOM_FACTOR/display_zoomscale)*(x))

#define ZOOMX(x)			round(zoomscale*((x)-zoomxoff)-display_zoomscale)
#define ZOOMY(y)			round(zoomscale*((y)-zoomyoff)-display_zoomscale)
#define BACKX(x)			round((x+display_zoomscale)/zoomscale+zoomxoff)
#define BACKY(y)			round((y+display_zoomscale)/zoomscale+zoomyoff)

#define CM_TO_PIX(x)		round((float)(x) * DISPLAY_PIX_PER_CM   * display_zoomscale)
#define INCH_TO_PIX(x)		round((float)(x) * DISPLAY_PIX_PER_INCH * display_zoomscale)
#define PIX_TO_INCH(x)		(float)((float)(x) / (DISPLAY_PIX_PER_INCH) / display_zoomscale)
#define PIX_TO_CM(x)		(float)((float)(x) / (DISPLAY_PIX_PER_CM) / display_zoomscale)

#define MINZOOM				0.05
#define MAXZOOM 			20
