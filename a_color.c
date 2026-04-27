/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_color.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"

#include "resources.h"
#include <exec/memory.h>
#include <proto/exec.h>
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "z_color.h"
#include "z_edit.h"

#define GetR(x) ((x & 0x00FF0000) >> 16)
#define GetG(x) ((x & 0x0000FF00) >> 8)
#define GetB(x)  (x & 0x000000FF)

void setPenColor(int value)
{
	cur_pencolor = value;
    z_refreshAttributes();
}

void setFillColor(int value)
{
	cur_fillcolor = value;
    z_refreshAttributes();
}

void setFillStyle(int value)
{
	cur_fillstyle = value;
    z_refreshAttributes();
}

/*
	Checks for used colors, updates Used field in ColorPalette table
*/
void CheckUsedColors(F_compound *objects, BOOL compound)
{
	F_arc	   *a;
	F_compound *c;
	F_ellipse  *e;
	F_line	   *l;
	F_spline   *s;
	F_text	   *t;
	int i;
	
	if (compound == FALSE)
	{
		/* initial call => initialise UsedColors table */
		for (i=0; i<MAX_USR_COLS; i++) ColorPalette[i+NUM_STD_COLS].Used = FALSE;
	}
	
	for (c = objects->compounds; c != NULL; c = c->next)
	{
		CheckUsedColors(c, TRUE);
	}
	for (a = objects->arcs; a != NULL; a = a->next)
	{
		if (a->pen_color  > NUM_STD_COLS - 1) ColorPalette[a->pen_color].Used = TRUE;
		if (a->fill_color > NUM_STD_COLS - 1) ColorPalette[a->fill_color].Used = TRUE;
	}
	for (e = objects->ellipses; e != NULL; e = e->next)
	{
		if (e->pen_color  > NUM_STD_COLS - 1) ColorPalette[e->pen_color].Used = TRUE;
		if (e->fill_color > NUM_STD_COLS - 1) ColorPalette[e->fill_color].Used = TRUE;
	}
	for (l = objects->lines; l != NULL; l = l->next)
	{
		if (l->pen_color  > NUM_STD_COLS - 1) ColorPalette[l->pen_color].Used = TRUE;
		if (l->fill_color > NUM_STD_COLS - 1) ColorPalette[l->fill_color].Used = TRUE;
	}
	for (s = objects->splines; s != NULL; s = s->next)
	{
		if (s->pen_color  > NUM_STD_COLS - 1) ColorPalette[s->pen_color].Used = TRUE;
		if (s->fill_color > NUM_STD_COLS - 1) ColorPalette[s->fill_color].Used = TRUE;
	}
	for (t = objects->texts; t != NULL; t = t->next)
	{
		if (t->color  > NUM_STD_COLS - 1) ColorPalette[t->color].Used = TRUE;
	}
}

/*
    Add or Overwrite a User Color definition
    call with indx > -1 if you want to allocate UserColor(indx) explicitly
    if indx == -1, uses the first non used UserColor
    return the colorcell number (0..MAX_USR_COLS-1)
    return -1 if MAX_USR_COLS already used
*/
int Add_UserColor(int indx, ULONG RGB)
{
	int	i;
	
	if (indx == -1)
	{
		/* look for color cell available in the middle */
		for (i=0; i < MAX_USR_COLS; i++)
			if (ColorPalette[i+NUM_STD_COLS].Defined == FALSE)
			{
				indx = i;
				break;
			}
	}

	if ((indx > -1) && (indx < NUM_STD_COLS + MAX_USR_COLS))
	{
        ColorPalette[indx+NUM_STD_COLS].RGB     = RGB;
		ColorPalette[indx+NUM_STD_COLS].Defined = TRUE;
        DispatchColorUIUpdate(UPDATE_PALETTES | UPDATE_SHADES, indx + NUM_STD_COLS);
    }
	else
		indx = -1;
		
	return indx;
}

void Init_UserColors(void)
{
	int i;

	/* init color palette for user defined colors */
    for(i=NUM_STD_COLS; i<NUM_STD_COLS + MAX_USR_COLS; i++)
    {
        ColorPalette[i].RGB     = 0xFFAAAAAA;
        ColorPalette[i].Used    = FALSE;
		ColorPalette[i].Defined = TRUE;
    }
}
void Init_Gradients(void)
{
    if (GradientTable[0] == NULL)
    {
#ifdef __amigaos4__
        GradientTable[0] = AllocVecTags(sizeof(Gradient), TAG_DONE);
#else
        GradientTable[0] = AllocVec(sizeof(Gradient), MEMF_ANY);
#endif
        if (GradientTable[0] != NULL)
        {
            GradientTable[0]->type = 0;
            GradientTable[0]->spread = 0;
            GradientTable[0]->unit = 0;
            GradientTable[0]->x1 = 0;
            GradientTable[0]->y1 = 0; 
            GradientTable[0]->x2 = 100;
            GradientTable[0]->y2 = 100;
            GradientTable[0]->radius = 100;
            GradientTable[0]->stops = 2;
            GradientTable[0]->Stop[0].color = 0xFF333333;
            GradientTable[0]->Stop[0].stopvalue = 0;
            GradientTable[0]->Stop[1].color = 0xFFFFFFFF;
            GradientTable[0]->Stop[1].stopvalue = 100;
            GradientTable[0]->precalc = FALSE;
        }
    }
}

/* Returns RGB value of fill_color and shade */
ULONG getfillcolor(int BaseColor, int shade)
{
	UBYTE R,G,B,oR=0,oG=0,oB=0;
	ULONG OutPen=0x00000000;
    ULONG alpha;
    
    if (BaseColor < 0 || BaseColor >= NUM_STD_COLS + MAX_USR_COLS)
        return 0xFF000000; // opaque black fallback

    alpha = (ColorPalette[BaseColor].RGB & 0xFF000000);
	
    if (BaseColor < NUM_STD_COLS + MAX_USR_COLS)
    {
        if (BaseColor == BLACK) shade = 40 - shade;
        
        if ((shade == 20) || (shade < 0) || (shade > 40))
        {
            OutPen = ColorPalette[BaseColor].RGB;
        }
        else if (shade == 0)
        {
            OutPen = ColorPalette[BLACK].RGB;
        }
        else if (  (shade == 40)
                || ((shade > 20) && ((BaseColor == WHITE) || (BaseColor == BLACK))))
        {
            OutPen = ColorPalette[WHITE].RGB;
        }
        else
        {
            R = (UBYTE)GetR(ColorPalette[BaseColor].RGB);
            G = (UBYTE)GetG(ColorPalette[BaseColor].RGB);
            B = (UBYTE)GetB(ColorPalette[BaseColor].RGB);

            if (shade < 20)
            {
                
                oR = (UBYTE)((R * shade)/20);
                oG = (UBYTE)((G * shade)/20);
                oB = (UBYTE)((B * shade)/20);
            }
            
            if (shade > 20)
            {
                shade -= 20;
                oR = (UBYTE)(R + (255 - R) * shade / 20);
                oG = (UBYTE)(G + (255 - G) * shade / 20);
                oB = (UBYTE)(B + (255 - B) * shade / 20);
            }
            OutPen = (ULONG)(oR << 16 | oG << 8 | oB);	
        }
    }

    OutPen |= alpha;
	return(OutPen);
}
