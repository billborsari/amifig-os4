/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_antialias.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "a_antialias.h"
#include "a_antialias_sse.h"
#include "a_coords.h"
#include "w_canvas.h"
#include "w_msgpanel.h"
#include "w_drawprim.h"

#define AVG(a,b)    (((((a)^(b)) & 0xfefefefeUL) >> 1) + ((a)&(b)))

/*
    In order to improve rendering speed offscreen buffers are made global
    and allocated once and resized if necessary
*/

struct BitMap     *u_redraw_bm = NULL;
struct pixelbuffer u_redraw_pm = {(LONG)0, (LONG)0, (ULONG *)NULL, (ULONG *)NULL};
struct RastPort   *aa_tmp_rastport = NULL;

void CleanUp_AAbuffers()
{
    if(u_redraw_bm)
		{
        FreeBitMap(u_redraw_bm);
        u_redraw_bm = NULL;
	}
    FreePixBuf_aligned(&u_redraw_pm);
}

BOOL setup_aa_rendering(int x, int y, int width, int height)
{
    BOOL success = FALSE;
	
    width*=2;
    height*=2;
    
    if ((u_redraw_bm != NULL)
       && ((GetBitMapAttr(u_redraw_bm, BMA_WIDTH)  < width) 
        || (GetBitMapAttr(u_redraw_bm, BMA_HEIGHT) < height)))
    {
        // offscreen bitmap isn't of correct size => force re-creation
        FreeBitMap(u_redraw_bm);
        u_redraw_bm = NULL;
    }
    
    if (u_redraw_bm == NULL)
    {
        // offscreen bitmap isn't existing => create one
        u_redraw_bm = AllocBitMap(	width,
                                    height, 
                                    GetBitMapAttr(rastport->BitMap, BMA_DEPTH),
                                    BMF_MINPLANES | BMF_STANDARD,
                                    rastport->BitMap);
    }
    
    if(u_redraw_bm)
    {        
        // Save actual display rastport
        aa_tmp_rastport = rastport;

#ifdef __AROS__
        rastport = CreateRastPort();
#else
        rastport = AllocMem (sizeof (struct RastPort), MEMF_ANY);
        InitRastPort(rastport);
#endif
        if (rastport)
        {
            if (SetTempRaster( width, height, FALSE) >= 0)
            {
                rastport->BitMap   = u_redraw_bm;
                rastport->TmpRas   = &tmpras;
                rastport->AreaInfo = &areainfo;

                SetDrMd(rastport, JAM1);
                SetRPAttrs(rastport, RPTAG_PenMode, FALSE, RPTAG_FgColor, ColorPalette[WHITE].RGB, TAG_DONE);
                RectFill(rastport, 0, 0, width, height);
                                            
                success = AllocPixBuf_aligned(&u_redraw_pm, width, height, 16);
            }
		}
    }
	
	if (success == TRUE)
	{
		/*
			We have all necessary to perform AntiAliasing display
			Let's adapt zooming factor and shift values for this
		*/
		display_zoomscale *= 2;
		xshift = (xshift + x) * 2;
		yshift = (yshift + y) * 2;
	}
	else
	{
        put_msg((char *)_(msg_OffScreenFailed));
		/*
			Couldn't init rastport or Pixel Buffer
			Use display rastport for rendering without AA
		*/
		if (u_redraw_bm)
        {
            FreeBitMap(u_redraw_bm);
            u_redraw_bm = NULL;
        }
		if ((rastport != NULL) && (aa_tmp_rastport != rastport))
		{
#ifdef __AROS__
			FreeRastPort(rastport);
#else
			FreeMem (rastport, sizeof (struct RastPort));
			rastport = NULL;
#endif
		}
		rastport = aa_tmp_rastport;
	}
    
    return(success);
}

void perform_antialising(int x, int y, int width, int height)
{
	LONG rectfmt;

#ifdef __AROS
    LONG pixfmt = 0;
    
    pixfmt = GetCyberMapAttr(rastport->BitMap, CYBRMATTR_PIXFMT);
    switch(pixfmt)
    {
		case PIXFMT_BGR032:
        case PIXFMT_BGRA32:
        case PIXFMT_RGBA32:
		case PIXFMT_RGB032:
		case PIXFMT_ABGR32:
		case PIXFMT_0BGR32:
		case PIXFMT_ARGB32:
		case PIXFMT_0RGB32:
            rectfmt = RECTFMT_RAW;
            break;
        default:
            rectfmt = RECTFMT_ARGB;
            break;
    }
#else
	rectfmt = RECTFMT_ARGB;
#endif
    
    /*
        Copy bitmap to CPU RAM in one time
        speeds up rendering as video mem access is very slow, especially for small amount of data.
    */
    ReadPixelArray((APTR)u_redraw_pm.data_aligned, 0, 0, (u_redraw_pm.pitch << 2), rastport, 0, 0, width * 2, height * 2, rectfmt);
    
#ifdef __AROS__ 
    if (SSE3_Support == TRUE)
    {
        perform_antialising_sse3(&u_redraw_pm, width, height);
    }
    else
#endif
    {
    	LONG i, j;
        LONG dst_index, src_index1, src_index2;
        ULONG *dst, *src1, *src2;

        dst_index  = 0;
        src_index1 = 0;
        src_index2 = u_redraw_pm.pitch;
        j = height;
        do
        {
            dst  = &u_redraw_pm.data_aligned[dst_index];
            src1 = &u_redraw_pm.data_aligned[src_index1];
            src2 = &u_redraw_pm.data_aligned[src_index2];
            i = width/4;
            do
            {
                dst[0] =  AVG(AVG(src1[0], src1[1]), AVG(src2[0], src2[1]));
                dst[1] =  AVG(AVG(src1[2], src1[3]), AVG(src2[2], src2[3]));
                dst[2] =  AVG(AVG(src1[4], src1[5]), AVG(src2[4], src2[5]));
                dst[3] =  AVG(AVG(src1[6], src1[7]), AVG(src2[6], src2[7]));
                dst  += 4; 
                src1 += 8;
                src2 += 8;
            }
            while(--i);
            dst_index  += u_redraw_pm.pitch;
            src_index1 += u_redraw_pm.pitch << 1;
            src_index2 += u_redraw_pm.pitch << 1;
        }
        while(--j);
    }
   
    SetDrMd(aa_tmp_rastport, JAM1);
    WritePixelArray(	(APTR)u_redraw_pm.data_aligned,
                        0,
                        0,
                        u_redraw_pm.pitch << 2,
                        aa_tmp_rastport,
                        x,
                        y,
                        width,
                        height,
                        rectfmt);
                        
                            
#ifdef __AROS__
    FreeRastPort(rastport);
#else
    if (rastport) 
    {
        FreeMem (rastport, sizeof (struct RastPort));
        rastport = NULL;
    }
#endif

    /* Set back initial values */
    rastport = aa_tmp_rastport;
    display_zoomscale /= 2;
    xshift = (xshift / 2) - x;
    yshift = (yshift / 2) - y;	
}
