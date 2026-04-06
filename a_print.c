/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_print.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include "intui.h"
#include <devices/printer.h>
#include <devices/prtbase.h>
#include <unistd.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "a_coords.h"
#include "f_save.h"
#include "u_redraw.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_msgpanel.h"
#include "w_cursor.h"

int getPrinterDPI(int Density, int *xdpi, int *ydpi)
{
	extern struct IODRPReq *printerRequest;
	struct PrinterData *PD;
	struct PrinterExtendedData *PED;
	struct Preferences *pprefs;
	int err = 1;
	static int last_density=-1, last_xdpi=0, last_ydpi=0;

	*xdpi=0;
	*ydpi=0;

	if(Density == last_density)
	{
		*xdpi = last_xdpi;
		*ydpi = last_ydpi;
	}
	else
	{
		if(OpenDevice((CONST_STRPTR)"printer.device", 0, (struct IORequest *)printerRequest, 0) == 0)
		{
			printerRequest->io_RastPort		= rastport;
			printerRequest->io_ColorMap		= Scr->ViewPort.ColorMap;
			printerRequest->io_SrcX			= 0;
			printerRequest->io_SrcY			= 0;
			printerRequest->io_SrcWidth  	= 10;
			printerRequest->io_SrcHeight 	= 10;
			printerRequest->io_DestCols  	= 10;
			printerRequest->io_DestRows  	= 10;
			printerRequest->io_Command		= PRD_DUMPRPORT;
			printerRequest->io_Modes		= 0;

			PD							= (struct PrinterData *)printerRequest->io_Device; // get pointer to printer data
			pprefs						= &PD->pd_Preferences;
			pprefs->PrintDensity 		= Density;
			printerRequest->io_Special 	= SPECIAL_NOPRINT;

			if(( err = DoIO((struct IORequest *)printerRequest)) == 0)
			{
				PED 	= &PD->pd_SegmentData->ps_PED; // get pointer to printer extended data
				*xdpi 	= PED->ped_XDotsInch;
				*ydpi 	= PED->ped_YDotsInch;
				/* save data for next call */
				last_density = Density;
				last_xdpi=*xdpi;
				last_ydpi=*ydpi;
			}			
			CloseDevice((struct IORequest *)printerRequest);
		}
	}
    return(err);
}

void print_bitmap(int left, int top, int width, int height, int Density)
{
	struct BitMap 				*bm;
	int 						bwidth, bheight, depth=GetBitMapAttr(rastport->BitMap, BMA_DEPTH);
	struct Layer_Info 			*layer_info=NULL;
	struct Layer 				*layer=NULL;
	struct RastPort 			rport;
	struct RastPort 			*RastPortTemp=rastport;
	extern struct IODRPReq 		*printerRequest;
	struct PrinterData 			*PD;
	struct PrinterExtendedData 	*PED;
	struct Preferences 			*pprefs;
	float 						old_zoomscale=display_zoomscale, correctx, correcty;
	LONG 						err;
	int 						size, part, parts,lines_per_part,remainder;
	int 						pxdpi = 300, pydpi = 300;
	struct DrawContext 			DC;
		
	beginWait();

	DC = save_drawcontext();

	if(OpenDevice((CONST_STRPTR)"printer.device", 0, (struct IORequest *)printerRequest, 0) == 0)
	{
		PD = (struct PrinterData *)printerRequest->io_Device; // get pointer to printer data
		pprefs= &PD->pd_Preferences;
		pprefs->PrintDensity = Density;

		/* First get printer dpi information */
		printerRequest->io_RastPort		= rastport;
		printerRequest->io_ColorMap		= Scr->ViewPort.ColorMap;
		printerRequest->io_SrcX			= 0;
		printerRequest->io_SrcY			= 0;
		printerRequest->io_SrcWidth  	= 10;
		printerRequest->io_SrcHeight 	= 10;
		printerRequest->io_DestCols  	= 10;
		printerRequest->io_DestRows  	= 10;
		printerRequest->io_Command		= PRD_DUMPRPORT;
		printerRequest->io_Modes		= 0;
		printerRequest->io_Special 		= SPECIAL_NOPRINT;
		if(DoIO((struct IORequest *)printerRequest) == 0)
		{
			/* get pointer to printer extended data */
			PED 	= &PD->pd_SegmentData->ps_PED;
			pxdpi 	= PED->ped_XDotsInch;
			pydpi 	= PED->ped_YDotsInch;
		}
				
		/* Scaling of print bitmap to highest value between x and y dpi */
		if(pxdpi > pydpi)
			display_zoomscale = (float)pxdpi/DISPLAY_PIX_PER_INCH;
		else
			display_zoomscale = (float)pydpi/DISPLAY_PIX_PER_INCH;

		/* Correction factor for X and Y direction to apply if resolution is uneven */
		correctx=(float)pxdpi/(DISPLAY_PIX_PER_INCH * display_zoomscale);
		correcty=(float)pydpi/(DISPLAY_PIX_PER_INCH * display_zoomscale);

		/* to get the printout at the current display scale */
		display_zoomscale *= old_zoomscale;

		/* if function is called without explicit dimensions, whole page is printed */
		if(width == -1)
		{
			float pagex,pagey;

			get_pagesizes(&pagex, &pagey);

			width  = CM_TO_PIX(pagex);
			height = CM_TO_PIX(pagey);
			xshift = 0;
			yshift = 0;
		}
		else
		{
			/* Otherwise scale dimensions to new scale */
			float scale = display_zoomscale/old_zoomscale;

			width++;
			width  = round((float)width *scale);
			height = round((float)height*scale);
			xshift = round((float)left  *scale);
			yshift = round((float)top   *scale);
		}

		/* ???? */
		yshift--;
		
		/* divide the bitmap into parts smaller than printbuf */
		size  			= width*height*depth;
		parts 			= size / (appres.printbuf * 1024 * 8) + 1;
		lines_per_part 	= height / parts;
		remainder 		= height - (parts-1)*lines_per_part;
		if((bm=AllocBitMap(width, remainder+2, depth, BMF_MINPLANES | BMF_STANDARD | BMF_CLEAR, NULL)))
		{
			bwidth  = GetBitMapAttr(bm, BMA_WIDTH);
			bheight = GetBitMapAttr(bm, BMA_HEIGHT);

			if((layer_info=NewLayerInfo()))
			{
				if((layer=CreateBehindLayer(layer_info, bm, 0, 1, bwidth-1, bheight-1, LAYERSIMPLE | LAYERBACKDROP, NULL)))
				{
					
                    if (SetTempRaster(bwidth, bheight, FALSE) >= 0)
                    {
                        rastport=&rport;
                        InitRastPort(rastport);
                        rastport->Layer=layer;
                        rastport->BitMap=bm;

                        rastport->TmpRas=&tmpras;
                        rastport->AreaInfo=&areainfo;

                        for(part=0; part < parts; yshift+=lines_per_part, part++)
                        {
                            /* The last piece can be smaller than the others */
                            if(part == parts-1)
                            {
                                lines_per_part=remainder;
                            }

                            put_msg((char *)_(msg_PrintingProgress), part+1, parts);

                            ClearRect(0, 0, width, remainder+1);
                            
                            redisplay_all();

                            printerRequest->io_RastPort	 = rastport;
                            printerRequest->io_ColorMap	 = Scr->ViewPort.ColorMap;
                            printerRequest->io_SrcX		 = 0;
                            printerRequest->io_SrcY		 = 1;
                            printerRequest->io_SrcWidth  = width;
                            printerRequest->io_SrcHeight = lines_per_part;
                            printerRequest->io_DestCols  = width * correctx;
                            printerRequest->io_DestRows  = lines_per_part * correcty;
                            printerRequest->io_Command   = PRD_DUMPRPORT;
                            printerRequest->io_Modes     = 0;
                            printerRequest->io_Special   = ((part < parts-1)?SPECIAL_NOFORMFEED:0);

                            if((err=DoIO((struct IORequest *)printerRequest)) != 0)
                            {
                                put_msg((char *)_(msg_PrinterErr), err);
                            }
                        }
                        rastport->Layer		= NULL;
                        rastport->BitMap	= NULL;
                        rastport->TmpRas	= NULL;
                        rastport->AreaInfo	= NULL;
                        rastport=RastPortTemp;
                        if(layer)	DeleteLayer(0, layer);
                    }
                    else
                    {
                        put_msg((char *)_(msg_OutOfMem));
                    }
                }
				else
				{
					put_msg((char *)_(msg_LayerErr));
				}
			}
			else
			{
				put_msg((char *)_(msg_LayerInfoErr));
			}
			if(layer_info)	DisposeLayerInfo(layer_info);
			FreeBitMap(bm);
		}
		else
		{
			doMessage((char *)_(msg_PrintBMErr));
		}
		CloseDevice((struct IORequest *)printerRequest);
		display_zoomscale=old_zoomscale;
	}
	else
	{
		doMessage((char *)_(msg_PrinterDeviceErr));
	}

	restore_drawcontext(DC);
	put_msg((char *)_(msg_Done));
	endWait();
}

int PrintToPS()
{
	int modified;
	char dos_command[289], tempnam[L_tmpnam],tempdir[4+L_tmpnam],tempfigfilename[FILENAME_MAX]="", temppsfilename[FILENAME_MAX]="";
	BPTR dirlock;

	if(emptyfigure())
	{
		put_msg((char *)_(msg_PrintNoFigure));
		return(-1);
	}

	beginWait();

	// save current fig in temp directory
	tmpnam(tempnam);
	sprintf(tempdir,"ram:%s",tempnam+2);
	dirlock = CreateDir((CONST_STRPTR)tempdir);
	if (!dirlock)
	{
		doMessage((char *)_(msg_WriteFileErr), tempdir);
		return(-1);
	}
	UnLock(dirlock);
	sprintf(tempfigfilename, "%s/%s", tempdir, FileName);

	modified=figure_modified;

	write_file(tempfigfilename);

	if(modified)
		set_modifiedflag();

	// convert to PS using fig2dev
	sprintf(temppsfilename, "%s/%s", tempdir, "out.ps");
	sprintf(dos_command,"fig2dev -L ps %s %s", tempfigfilename, temppsfilename);
	Execute((CONST_STRPTR)dos_command, NULL, NULL);

	// send to USB port using CopyToPAR
	sprintf(dos_command,"CopyToPAR %s USB QUIET", temppsfilename);
	Execute((CONST_STRPTR)dos_command, NULL, NULL);
	
	// clear temporary files
	remove(tempfigfilename);
	remove(temppsfilename);
	strcat(tempfigfilename, ".info");
	remove(tempfigfilename);
	rmdir(tempdir);
	
	endWait();
	return(0);
}
