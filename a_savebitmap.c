/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_savebitmap.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <proto/datatypes.h>
#include <datatypes/pictureclass.h>

#ifdef __amigaos4__
#include <graphics/gfx.h>
#endif

#include "resources.h"
#include "a_coords.h"
#include "u_redraw.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_msgpanel.h"
#include "w_cursor.h"

BOOL SaveBitMap(struct BitMap *bm, long Width, long Height, ULONG AA, STRPTR Name)
{
	Object 						*DTImage = NULL;
	struct BitMapHeader 		*bmhd = NULL;
	struct RastPort 			*rp = NULL;
	struct dtWrite 				dtw;
	struct pdtBlitPixelArray 	dtb;
	BPTR 						file;
	UBYTE 						*dst = NULL;
    UBYTE 						*src = NULL;
	UBYTE 						*l1, *l2, *l3, *l4; // used for direct pixel line access
	int 						i,j,index,tmpindex;
	BOOL						success = FALSE;

	printf("SaveBitMap: starting for %s (W:%ld, H:%ld, AA:%lu)\n", Name, Width, Height, AA);

	if (AA != 0) // if we have to do Antialising the output resolution is divided!
	{
		Width  /= (AA + 1);
		Height /= (AA + 1);
	}
	
	DTImage = NewDTObject((APTR)NULL,
						  DTA_SourceType, DTST_RAM,
                          DTA_GroupID, GID_PICTURE,
						  DTA_BaseName, (IPTR)"png",
                          DTA_NominalHoriz, Width,
                          DTA_NominalVert, Height,
                          PDTA_SourceMode, PMODE_V43,
						  TAG_DONE);
	
	if (DTImage)
	{
		printf("SaveBitMap: NewDTObject created successfully.\n");
		if (GetDTAttrs(DTImage,PDTA_BitMapHeader,(IPTR)&bmhd,TAG_DONE))
		{
			bmhd->bmh_Width 		= Width;
			bmhd->bmh_Height 		= Height;
			bmhd->bmh_Depth 		= 32;
			bmhd->bmh_PageWidth 	= 320;
			bmhd->bmh_PageHeight 	= 240;
			
			dst = AllocVec(4*Width*Height, MEMF_ANY);
#ifdef __AROS__
			rp = CreateRastPort();
#else
			rp = AllocMem (sizeof (struct RastPort), MEMF_ANY);
			InitRastPort(rp);
#endif
			if (dst && rp)
			{
				rp->BitMap = bm;
				switch(AA)
				{
					case 0:
						/* No antialiasing */
						ReadPixelArray((APTR)dst, 0, 0, 4*Width, rp, 0, 0, Width, Height, RECTFMT_ARGB);
						break;
					case 1:
						/* do the actual antialiasing (each ouput pixel is the average of 4 pixels) */
						src = AllocVec(4 * Width*2 * Height*2, MEMF_ANY);
						if (src)
						{
							ReadPixelArray((APTR)src, 0, 0, 4 * Width * 2, rp, 0, 0, Width * 2, Height * 2, RECTFMT_ARGB);
							index = 0;
							for (j = 0; j < Height * 2 ; j+=2)
							{
								l1 = src + 4 *  j 	 * (Width * 2);
								l2 = src + 4 * (j+1) * (Width * 2);
								for (i = 0; i < Width * 2 * 4; i+=8)
								{
									tmpindex = index << 2;
									dst[tmpindex + 1] = (l1[i + 0 + 1] + l1[i + 4 + 1] + l2[i + 0 + 1] + l2[i + 4 + 1]) >> 2;
									dst[tmpindex + 2] = (l1[i + 0 + 2] + l1[i + 4 + 2] + l2[i + 0 + 2] + l2[i + 4 + 2]) >> 2;
									dst[tmpindex + 3] = (l1[i + 0 + 3] + l1[i + 4 + 3] + l2[i + 0 + 3] + l2[i + 4 + 3]) >> 2;
									index++;
								}
							}
							FreeVec(src);
						}
						else
							ReadPixelArray((APTR)dst, 0, 0, 4*Width, rp, 0, 0, Width, Height, RECTFMT_ARGB);
						break;
					case 2:
						/* do the actual antialiasing (each ouput pixel is the average of 9 pixels) */
						src = AllocVec(4 * Width*3 * Height*3, MEMF_ANY);
						if (src)
						{
							ReadPixelArray((APTR)src, 0, 0, 4 * Width * 3, rp, 0, 0, Width * 3, Height * 3, RECTFMT_ARGB);
							index = 0;
							for (j = 0; j < Height * 3; j+=3)
							{
								l1 = src + 4 *  j    * (Width * 3);
								l2 = src + 4 * (j+1) * (Width * 3);
								l3 = src + 4 * (j+2) * (Width * 3);
								for (i = 0; i<Width * 3 * 4; i+=12)
								{
									tmpindex = index << 2;
									dst[tmpindex + 1] = (l1[i + 0 + 1] + l1[i + 4 + 1] + l1[i + 8 + 1]
													   + l2[i + 0 + 1] + l2[i + 4 + 1] + l2[i + 8 + 1]
													   + l3[i + 0 + 1] + l3[i + 4 + 1] + l3[i + 8 + 1]) / 9;
									dst[tmpindex + 2] = (l1[i + 0 + 2] + l1[i + 4 + 2] + l1[i + 8 + 2]
													   + l2[i + 0 + 2] + l2[i + 4 + 2] + l2[i + 8 + 2]
													   + l3[i + 0 + 2] + l3[i + 4 + 2] + l3[i + 8 + 2]) / 9;
									dst[tmpindex + 3] = (l1[i + 0 + 3] + l1[i + 4 + 3] + l1[i + 8 + 3]
													   + l2[i + 0 + 3] + l2[i + 4 + 3] + l2[i + 8 + 3]
													   + l3[i + 0 + 3] + l3[i + 4 + 3] + l3[i + 8 + 3]) / 9;
									index++;
								}
							}
							FreeVec(src);
						}
						else
							ReadPixelArray((APTR)dst, 0, 0, 4*Width, rp, 0, 0, Width, Height, RECTFMT_ARGB);
						break;
					case 3:
						/* do the actual antialiasing (each ouput pixel is the average of 16 pixels) */
						src = AllocVec(4 * Width*4 * Height*4, MEMF_ANY);
						if (src)
						{
							ReadPixelArray((APTR)src, 0, 0, 4*Width*4, rp, 0, 0, Width*4, Height*4, RECTFMT_ARGB);
							index = 0;
							for (j = 0; j < Height * 4; j+=4)
							{
								l1 = src + 4 *  j    * (Width * 4);
								l2 = src + 4 * (j+1) * (Width * 4);
								l3 = src + 4 * (j+2) * (Width * 4);
								l4 = src + 4 * (j+3) * (Width * 4);
								for (i = 0; i<Width * 4 * 4 ; i+=16)
								{
									tmpindex = index << 2;
									dst[tmpindex + 1] = (l1[i + 0 + 1] + l1[i + 4 + 1] + l1[i + 8 + 1]+ l1[i + 12 + 1]
													   + l2[i + 0 + 1] + l2[i + 4 + 1] + l2[i + 8 + 1]+ l2[i + 12 + 1]
													   + l3[i + 0 + 1] + l3[i + 4 + 1] + l3[i + 8 + 1]+ l3[i + 12 + 1]
													   + l4[i + 0 + 1] + l4[i + 4 + 1] + l4[i + 8 + 1]+ l4[i + 12 + 1]) >> 4;
									dst[tmpindex + 2] = (l1[i + 0 + 2] + l1[i + 4 + 2] + l1[i + 8 + 2]+ l1[i + 12 + 2]
													   + l2[i + 0 + 2] + l2[i + 4 + 2] + l2[i + 8 + 2]+ l2[i + 12 + 2]
													   + l3[i + 0 + 2] + l3[i + 4 + 2] + l3[i + 8 + 2]+ l3[i + 12 + 2]
													   + l4[i + 0 + 2] + l4[i + 4 + 2] + l4[i + 8 + 2]+ l4[i + 12 + 2]) >> 4;
									dst[tmpindex + 3] = (l1[i + 0 + 3] + l1[i + 4 + 3] + l1[i + 8 + 3]+ l1[i + 12 + 3]
													   + l2[i + 0 + 3] + l2[i + 4 + 3] + l2[i + 8 + 3]+ l2[i + 12 + 3]
													   + l3[i + 0 + 3] + l3[i + 4 + 3] + l3[i + 8 + 3]+ l3[i + 12 + 3]
													   + l4[i + 0 + 3] + l4[i + 4 + 3] + l4[i + 8 + 3]+ l4[i + 12 + 3]) >> 4;
									index++;
								}
							}
							FreeVec(src);
						}
						else
							ReadPixelArray((APTR)dst, 0, 0, 4*Width, rp, 0, 0, Width, Height, RECTFMT_ARGB);
						break;
				}
				
#ifdef __amigaos4__
				{
					struct BitMap *save_bm = AllocBitMap(Width, Height, 32, BMF_MINPLANES | BMF_STANDARD, rp->BitMap);
					if (save_bm) {
						struct RastPort save_rp;
						InitRastPort(&save_rp);
						save_rp.BitMap = save_bm;
						
						// Blit the ARGB downmixed 'dst' array into 'save_bm' using CyberGraphics API signature
						WritePixelArray(dst, 0, 0, Width*4, &save_rp, 0, 0, Width, Height, RECTFMT_ARGB);
						
						// OS4 datatypes doesn't support writing pixels using PDTM_WRITEPIXELARRAY consistently on PNG subclasses.
						// The accepted technique is to re-create the Datatype instance providing the constructed BitMap directly!
						if (DTImage) DisposeDTObject(DTImage);
						DTImage = NewDTObject((APTR)NULL,
							DTA_SourceType, DTST_RAM,
							DTA_GroupID, GID_PICTURE,
							DTA_BaseName, (IPTR)"png",
							DTA_NominalHoriz, Width,
							DTA_NominalVert, Height,
							PDTA_BitMap, save_bm,
							TAG_DONE);
						
						if (DTImage) {
							struct BitMapHeader *bmh2 = NULL;
							if (GetDTAttrs(DTImage, PDTA_BitMapHeader, (IPTR)&bmh2, TAG_DONE) && bmh2) {
								bmh2->bmh_Width = Width;
								bmh2->bmh_Height = Height;
								bmh2->bmh_Depth = 32;
								bmh2->bmh_PageWidth = 320;
								bmh2->bmh_PageHeight = 240;
							}
							
							if ((file = Open(Name, MODE_NEWFILE))) {
								dtw.MethodID = DTM_WRITE;
								dtw.dtw_GInfo = NULL;
								dtw.dtw_FileHandle = file;
								dtw.dtw_Mode = DTWM_RAW;
								dtw.dtw_AttrList = NULL;

								if (DoMethodA(DTImage, (Msg)&dtw)) {
									success = TRUE;
									printf("SaveBitMap (OS4): Write DTM_WRITE successful.\n");
								} else {
									printf("SaveBitMap (OS4): DoMethodA DTM_WRITE failed.\n");
								}
								Close(file);
							} else {
								printf("SaveBitMap (OS4): Failed to open %s for writing\n", Name);
							}
						} else {
							printf("SaveBitMap (OS4): Failed to re-create DTImage with PDTA_BitMap.\n");
							FreeBitMap(save_bm);
						}
					} else {
						printf("SaveBitMap (OS4): Failed to allocate save_bm BitMap.\n");
					}
				}
#else
				dtb.MethodID 			= PDTM_WRITEPIXELARRAY;
				dtb.pbpa_PixelData 		= (APTR)dst;
				dtb.pbpa_PixelFormat 	= PBPAFMT_ARGB;
				dtb.pbpa_PixelArrayMod 	= Width*4;
				dtb.pbpa_Left 			= 0;
				dtb.pbpa_Top 			= 0;
				dtb.pbpa_Width 			= Width;
				dtb.pbpa_Height 		= Height;
				
				if (DoMethodA(DTImage, (Msg)&dtb))
				{
					/*write datatype object to file */
					if ((file = Open(Name,MODE_NEWFILE)))
					{
						dtw.MethodID 		= DTM_WRITE;
						dtw.dtw_GInfo 		= NULL;
						dtw.dtw_FileHandle 	= file;
						dtw.dtw_Mode 		= DTWM_RAW;
						dtw.dtw_AttrList 	= NULL;

						if (DoMethodA(DTImage, (Msg)&dtw)) {
							success = TRUE;
							printf("SaveBitMap: Write DTM_WRITE successful.\n");
						} else {
							printf("SaveBitMap: DoMethodA DTM_WRITE failed.\n");
						}
						Close(file);
					}
					else
					{
						printf("SaveBitMap: Failed to open %s for writing\n", Name);
					}
				}
				else
				{
					printf("SaveBitMap: DoMethodA PDTM_WRITEPIXELARRAY failed.\n");
				}
#endif
			}
			else
			{
				printf("SaveBitMap: Out of memory alloc Vec or RastPort.\n");
			}
		}
		else
		{
			printf("SaveBitMap: GetDTAttrs PDTA_BitMapHeader failed.\n");
		}
	}
	else
	{
		printf("SaveBitMap: NewDTObject returned NULL. Failed to create datatype.\n");
	}
	
	if (rp) 
	{
#ifdef __AROS__
		FreeRastPort(rp);
#else
		FreeMem (rp, sizeof (struct RastPort));
		rp = NULL;
#endif
	}
	if (dst) FreeVec(dst);
	if (DTImage) DisposeDTObject(DTImage);

	printf("SaveBitMap: Returning %d\n", success);
	return(success);
}

BOOL write_bitmap(char *file_name, int left, int top, int width, int height, int expwidth, int expheight, ULONG AA)
{
	struct BitMap *bm = NULL;
	int depth;
	struct RastPort *RastPortTemp=rastport;
	ULONG SavedAAsetting = appres.OnScreenAA;
	struct DrawContext DC;
	float old_zoomscale = display_zoomscale;

	beginWait();

	appres.OnScreenAA = FALSE;	// we don't want double Antialising...
	depth = GetBitMapAttr(RastPortTemp->BitMap, BMA_DEPTH);

	DC = save_drawcontext();
	
	display_zoomscale = min2((float)expwidth / (float)width,(float)expheight / (float)height); // 1.0F;
	
	if (AA > 3) AA = 0; //maximum AA level is 3 (4X antialising)
	
	if (AA != 0)
	{
		// For AntiAliased output we generate a bigger bitmap that is afterwards reduced again in SaveBitmap
		display_zoomscale *= (AA + 1);
		expwidth *= (AA + 1);
		expheight *= (AA + 1);
	}

	xshift = left * display_zoomscale; 
	yshift = top * display_zoomscale;
	display_zoomscale *= old_zoomscale; // to be equivalent to current display
	
	bm = AllocBitMap(expwidth, expheight, depth, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, RastPortTemp->BitMap);

	if(bm)
	{
#ifdef __AROS__
 		rastport = CreateRastPort();
#else
		rastport = AllocMem (sizeof (struct RastPort), MEMF_ANY);
		InitRastPort(rastport);
#endif
		if (rastport)
		{
            if(SetTempRaster(expwidth, expheight, FALSE) >= 0)
            {
                rastport->BitMap   = bm;
                rastport->TmpRas   = &tmpras;
                rastport->AreaInfo = &areainfo;

                ClearRect(0, 0, expwidth, expheight);
        
                redisplay_all();

                put_msg((char *)_(msg_SavingBitmap),file_name);

                if (SaveBitMap(bm, expwidth, expheight, AA, (STRPTR)file_name))
                    put_msg((char *)_(msg_Done));
                else
                    put_msg((char *)_(msg_DatatypeError));
            }

#ifdef __AROS__
            FreeRastPort(rastport);
#else
            FreeMem (rastport, sizeof (struct RastPort));
            rastport = NULL;
#endif
                rastport=RastPortTemp;

            FreeBitMap(bm);
            display_zoomscale = old_zoomscale;
            restore_drawcontext(DC);
            endWait();
            return(TRUE);
		}

		put_msg((char *)_(msg_RastportInitErr));
		FreeBitMap(bm);
		rastport=RastPortTemp;
	}
	else
	{
		put_msg((char *)_(msg_BMAllocateErr));
	}

	appres.OnScreenAA = SavedAAsetting; 
	display_zoomscale = old_zoomscale;
	restore_drawcontext(DC);
	endWait();
	return(FALSE);
}
