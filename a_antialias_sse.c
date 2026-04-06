/*
 *       AmiFIG
 *
 *       $HeadURL: https://svn.code.sf.net/p/amifig/code/trunk/src/a_antialias_sse.c $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 18:51:39 +0100 (sam., 30 janv. 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include "resources.h"
#include "a_antialias.h"

#ifdef __SSE3__
#include <pmmintrin.h>

void perform_antialising_sse3(struct pixelbuffer *u_redraw_pm, int width, int height)
{
	LONG i, j;
	LONG dst_index, src_index1, src_index2;
	ULONG *dst, *src1, *src2;
	    
    dst_index  = 0;
    src_index1 = 0;
    src_index2 = u_redraw_pm->pitch;
	j = height;
	do
    {
        dst  = &u_redraw_pm->data_aligned[dst_index];
        src1 = &u_redraw_pm->data_aligned[src_index1];
        src2 = &u_redraw_pm->data_aligned[src_index2];
		i = width/4;
		do
        {
            __m128i left     = _mm_avg_epu8(_mm_load_si128((__m128i const*)src1), _mm_load_si128((__m128i const*)src2));
            __m128i right    = _mm_avg_epu8(_mm_load_si128((__m128i const*)(src1+4)), _mm_load_si128((__m128i const*)(src2+4)));
            __m128i t0       = _mm_unpacklo_epi32( left, right ); 
            __m128i t1       = _mm_unpackhi_epi32( left, right ); 
            __m128i shuffle1 = _mm_unpacklo_epi32( t0, t1 );
            __m128i shuffle2 = _mm_unpackhi_epi32( t0, t1 );
            _mm_store_si128((__m128i *)dst, _mm_avg_epu8(shuffle1, shuffle2));
            dst  += 4; 
            src1 += 8;
            src2 += 8;
        }
		while(--i);
		dst_index  += u_redraw_pm->pitch;
        src_index1 += u_redraw_pm->pitch << 1;
        src_index2 += u_redraw_pm->pitch << 1;
    }
	while(--j);
}
#endif
