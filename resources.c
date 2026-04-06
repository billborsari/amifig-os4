/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/resources.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
 
#include "fig.h"
#include "intui.h"
#include <sys/stat.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "version.h"
#include "a_coords.h"
#include "a_eventloop.h"
#include "u_redraw.h"
#include "u_create.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_cursor.h"
#include "w_drawprim.h"
#include "w_msgpanel.h"
#include "z_mode.h"


Colors ColorPalette[NUM_STD_COLS + MAX_USR_COLS] =
{
    /* Preset only Standard colors */
	{0x00000000, TRUE, TRUE}, /* BLACK	  "Black",     */
	{0x000000FF, TRUE, TRUE}, /* BLUE	  "Blue",      */
	{0x0000FF00, TRUE, TRUE}, /* GREEN	  "Green",     */
	{0x0000FFFF, TRUE, TRUE}, /* CYAN	  "Cyan",      */
	{0x00FF0000, TRUE, TRUE}, /* RED	  "Red",       */
	{0x00FF00FF, TRUE, TRUE}, /* MAGENTA  "Magenta",   */
	{0x00FFFF00, TRUE, TRUE}, /* YELLOW	  "Yellow",    */
	{0x00FFFFFF, TRUE, TRUE}, /* WHITE	  "White",     */
	{0x00000090, TRUE, TRUE}, /*          "Blue4",     */
	{0x000000B0, TRUE, TRUE}, /*          "Blue3",     */
	{0x000000D0, TRUE, TRUE}, /*          "Blue2",     */
	{0x0087CEFF, TRUE, TRUE}, /*          "LtBlue",    */
	{0x00009000, TRUE, TRUE}, /*          "Green4",    */
	{0x0000B000, TRUE, TRUE}, /*          "Green3",    */
	{0x0000D000, TRUE, TRUE}, /*          "Green2",    */
	{0x00009090, TRUE, TRUE}, /*          "Cyan4",     */
	{0x0000B0B0, TRUE, TRUE}, /*          "Cyan3",     */
	{0x0000D0D0, TRUE, TRUE}, /*          "Cyan2",     */
	{0x00900000, TRUE, TRUE}, /*          "Red4",      */
	{0x00B00000, TRUE, TRUE}, /*          "Red3",      */
	{0x00D00000, TRUE, TRUE}, /*          "Red2",      */
	{0x00900090, TRUE, TRUE}, /*          "Magenta4",  */
	{0x00B000B0, TRUE, TRUE}, /*          "Magenta3",  */
	{0x00D000D0, TRUE, TRUE}, /*          "Magenta2",  */
	{0x00803000, TRUE, TRUE}, /*          "Brown4",    */
	{0x00A04000, TRUE, TRUE}, /*          "Brown3",    */
	{0x00C06000, TRUE, TRUE}, /*          "Brown2",    */
	{0x00FF8080, TRUE, TRUE}, /*          "Pink4",     */
	{0x00FFA0A0, TRUE, TRUE}, /*          "Pink3",     */
	{0x00FFC0C0, TRUE, TRUE}, /*          "Pink2",     */
	{0x00FFE0E0, TRUE, TRUE}, /*          "Pink",      */
	{0x00FFD700, TRUE, TRUE}  /*          "Gold",      */
};


/* Declare Gradient Table */
Gradient *GradientTable[MAX_GRD_COLS] = {NULL};

/* Declare paper_sizes structure defining names and dimensions */
struct	paper_def paper_sizes[NUMPAPERSIZES] = {
    {"Letter  ", "Letter  (8.5\" x 11\" / 216 x 279 mm)", LETTER_WIDTH, LETTER_HEIGHT}, 
    {"Legal   ", "Legal   (8.5\" x 14\" / 216 x 356 mm)",   10200, 16800}, 
    {"Tabloid ", "Tabloid ( 11\" x 17\" / 279 x 432 mm)",   13200, 20400}, 
    {"A       ", "ANSI A  (8.5\" x 11\" / 216 x 279 mm)",   10200, 13200}, 
    {"B       ", "ANSI B  ( 11\" x 17\" / 279 x 432 mm)",   13200, 20400}, 
    {"C       ", "ANSI C  ( 17\" x 22\" / 432 x 559 mm)",   20400, 26400}, 
    {"D       ", "ANSI D  ( 22\" x 34\" / 559 x 864 mm)",   26400, 40800}, 
    {"E       ", "ANSI E  ( 34\" x 44\" / 864 x 1118 mm)",   40800, 52800}, 
    {"A9      ", "ISO A9  (  37mm x   52mm)",  1748,  2467},
    {"A8      ", "ISO A8  (  52mm x   74mm)",  2457,  3500},
    {"A7      ", "ISO A7  (  74mm x  105mm)",  3496,  4960},
    {"A6      ", "ISO A6  ( 105mm x  148mm)",  4960,  6992}, 
    {"A5      ", "ISO A5  ( 148mm x  210mm)",  6992,  9921},
    {"A4      ", "ISO A4  ( 210mm x  297mm)",  A4_WIDTH, A4_HEIGHT}, 
    {"A3      ", "ISO A3  ( 297mm x  420mm)", 14031, 19843}, 
    {"A2      ", "ISO A2  ( 420mm x  594mm)", 19843, 28063}, 
    {"A1      ", "ISO A1  ( 594mm x  841mm)", 28063, 39732}, 
    {"A0      ", "ISO A0  ( 841mm x 1189mm)", 39732, 56173}, 
    {"B10     ", "JIS B10 (  32mm x   45mm)",  1516,  2117},
    {"B9      ", "JIS B9  (  45mm x   64mm)",  2117,  3017},
    {"B8      ", "JIS B8  (  64mm x   91mm)",  3017,  4300},
    {"B7      ", "JIS B7  (  91mm x  128mm)",  4300,  6050},
    {"B6      ", "JIS B6  ( 128mm x  182mm)",  6050,  8598},
    {"B5      ", "JIS B5  ( 182mm x  257mm)",  8598, 12150},
    {"B4      ", "JIS B4  ( 257mm x  364mm)", 12150, 17200},
    {"B3      ", "JIS B3  ( 364mm x  515mm)", 17200, 24333},
    {"B2      ", "JIS B2  ( 515mm x  728mm)", 24333, 34400},
    {"B1      ", "JIS B1  ( 728mm x 1030mm)", 34400, 48666},
    {"B0      ", "JIS B0  (1030mm x 1456mm)", 48666, 68783},
};

char *COL_strings[] =
{
	"Black",   
	"Blue",    
	"Green",   
	"Cyan",    
	"Red",     
	"Magenta", 
	"Yellow",  
	"White",   
	"Blue4",   
	"Blue3",   
	"Blue2",   
	"LtBlue",  
	"Green4",  
	"Green3",  
	"Green2",  
	"Cyan4",   
	"Cyan3",   
	"Cyan2",   
	"Red4",    
	"Red3",    
	"Red2",    
	"Magenta4",
	"Magenta3",
	"Magenta2",
	"Brown4",  
	"Brown3",  
	"Brown2",  
	"Pink4",   
	"Pink3",   
	"Pink2",   
	"Pink",    
	"Gold",
	NULL
};

char *TXTFONT_strings[] =
{
	"Default",
	"Times Roman",
	"Times Italic",
	"Times Bold",
	"Times Bold Italic",
	"Avantgarde Book",
	"Avantgarde Book Oblique",
	"Avantgarde Demi",
	"Avantgarde Demi Oblique",
	"Bookman Light",
	"Bookman Light Italic",
	"Bookman Demi",
	"Bookman Demi Italic",
	"Courier",
	"Courier Oblique",
	"Courier Bold",
	"Courier Bold Oblique",
	"Helvetica",
	"Helvetica Oblique",
	"Helvetica Bold",
	"Helvetica Bold Oblique",
	"Helvetica Narrow",
	"Helvetica Narrow Oblique",
	"Helvetica Narrow Bold",
	"Helvetica Narrow Bold Oblique",
	"NewCentury Schoolbook Roman",
	"NewCentury Schoolbook Italic",
	"NewCentury Schoolbook Bold",
	"NewCentury Schoolbook Bold Italic",
	"Palatino Roman",
	"Palatino Italic",
	"Palatino Bold",
	"Palatino Bold Italic",
	"Symbol",
	"Zapf Chancery Medium Italic",
	"Zapf Dingbats",
	"Latex : Default",
	"Latex : Roman",
	"Latex : Bold",
	"Latex : Italic",
	"Latex : Modern",
	"Latex : Typewriter",
	NULL
};

float	zoomscale 			= 1.0F/ZOOM_FACTOR;
int		zoomxoff  			= 0;
int		zoomyoff  			= 0;
float	display_zoomscale 	= 1.0F;
BOOL    display_fractions	= TRUE;

char settingsVersion[]="AMIFIG_" SETTING_VERSION;

struct appresStruct appres =
{
	// Screen Mode
	TRUE,       			    // ULONG			UsePubScreen;
	"Workbench", 			    // UBYTE			PubScreenName[48];
	0,						    // ULONG			ScreenMode;
	0,         				    // WORD				ScreenWidth;
	0,         				    // WORD				ScreenHeight;
	0,          			    // WORD				ScreenDepth;
	FALSE,        			    // ULONG			AutoScroll;
	FALSE,       			    // ULONG			Backdrop;
        
	// Skins
	"Manu's Theme",		        // char				skin[32];
	        
	// Windows positions    
	336,           			    // UWORD			CanvasLeft;
	130,          			    // UWORD			CanvasTop;
	690,         			    // UWORD			CanvasWidth;
	610,         			    // UWORD			CanvasHeight;
        
	// I/O 
	FALSE,       			    // ULONG			WriteBackup;	
	FALSE,       			    // ULONG			AutoLoadLib;	
	"AmiFIG",    			    // char				DefTool[FMSIZE];
	TRUE,        			    // ULONG			CreateIcons;
	TRUE,						// ULONG			save_whole_palette      /* Save also unused user colors */

#if defined(__amigaos4__) || defined(__AROS__) || defined(__linux__)
	// Export via Fig2Dev Command
#if defined(__amigaos4__)
	"python FIG2DEV.py",		// char				f2d_cmd[256];
#else
	"LUA:amilua FIG2DEV.lua",	// char				f2d_cmd[256];
#endif
	"-screen",  				// char				f2d_screenopt[32];
	"-o",      					// char				f2d_outputopt[32];
    "",                     	// char				f2d_inputopt[32];
	"",                     	// char				f2d_otheropt[32];
	
	// Import via RAS2VEC
#if defined(__amigaos4__)
	"python RAS2VEC.py",		// char				r2v_cmd[256];
#else
	"LUA:amilua RAS2VEC.lua",	// char				r2v_cmd[256];
#endif
	"-screen",  				// char				r2v_screenopt[32];
	"-o",      					// char				r2v_outputopt[32];
    "",                     	// char				r2v_otheropt[32];
#else
	// Export via Fig2Dev Command
	"fig2dev -L pdf",			// char				f2d_cmd[256];
	"",  						// char				f2d_screenopt[32];
	">",      					// char				f2d_outputopt[32];
    "",                     	// char				f2d_inputopt[32];
	"",                     	// char				f2d_otheropt[32];
	
	// Import via RAS2VEC
	"not configured",			// char				r2v_cmd[256];
	"",  						// char				r2v_screenopt[32];
	"",      					// char				r2v_outputopt[32];
    "",                     	// char				r2v_otheropt[32];
#endif
	// SVG import options
	50,							// ULONG			SVG_InterSteps;
	100,						// ULONG			SVG_Scale;

	// Paint window settings
	TRUE,        			    // ULONG			AutoActivate;		
	TRUE,        			    // ULONG			topruler_on;
	TRUE,        			    // ULONG			sideruler_on;
	TRUE,					    // ULONG			autoScrollCanvas;
	TRUE,					    // ULONG			softScrollCanvas;
	TRUE,					    // ULONG			viewBestQuality;	
	FALSE,						// ULONG		    OnScreenAA;		Display Antialising
    1.0,                    	// float			userscale;		scale screen units to user units
    TRUE,                   	// BOOL     		showlengths;    length/width lines when drawing or moving
    TRUE,                   	// BOOL     		shownums;       print vertex numbers above polyline points
	200,						// ULONG		    freehand_resolution;
    10,                         // ULONG            gradient_ditherspread

	
	// Default page settings
    FALSE,                  	// BOOL	    			INCHES; 
    PAPER_A4,               	// ULONG				papersize;
    TRUE,     	             	// BOOL	    			landscape;  
    	
	// System to XFig font mapping			char 			DisplayFonts[42][64];
#ifdef __AROS__
	{
		"Vera Sans.font",					// -1	"default"
		"Vera Sans.font",					//  0	"Times Roman"
		"Vera Sans Italic.font",			//  1	"Times Italic"
		"Vera Sans Bold.font",				//  2	"Times Bold"
		"Vera Sans Bold Italic.font",		//  3	"Times Bold Italic"
		"dejavuserifbook.font",				//  4	"Avantgarde Book"
		"dejavuserifoblique.font",			//  5	"Avantgarde Book Oblique"
		"dejavusansbold.font",				//  6	"Avantgarde Demi"
		"dejavusansboldoblique.font",		//  7	"Avantgarde Demi Oblique"
		"Vera Sans.font",					//  8	"Bookman Light"
		"Vera Sans Italic.font",			//  9	"Bookman Light Italic"
		"Vera Sans.font",					// 10	"Bookman Demi"
		"Vera Sans Italic.font",			// 11	"Bookman Demi Italic"
		"ttcourier.font",					// 12	"Courier"
		"ttcourier.font",					// 13	"Courier Oblique"
		"ttcourier.font",					// 14	"Courier Bold"
		"ttcourier.font",					// 15	"Courier Bold Oblique"
		"arial.font",						// 16	"Helvetica"
		"arial.font",						// 17	"Helvetica Oblique"
		"arial.font",						// 18	"Helvetica Bold"
		"arial.font",						// 19	"Helvetica Bold Oblique"
		"dejavusanscondensedcondens.font",	// 20	"Helvetica Narrow"
		"dejavusanscondensedoblique.font",	// 21	"Helvetica Narrow Oblique"
		"dejavusanscondensedbold.font",		// 22	"Helvetica Narrow Bold"
		"dejavusanscondensedboldobl.font",	// 23	"Helvetica Narrow Bold Oblique"
		"Vera Sans.font",					// 24	"NewCentury Schoolbook Roman"
		"Vera Sans Italic.font",			// 25	"NewCentury Schoolbook Italic"
		"Vera Sans Bold.font",				// 26	"NewCentury Schoolbook Bold"
		"Vera Sans Bold Italic.font",		// 27	"NewCentury Schoolbook Bold Italic"
		"Vera Sans.font",					// 28	"Palatino Roman"
		"Vera Sans Italic.font",			// 29	"Palatino Italic"
		"Vera Sans Bold.font",				// 30	"Palatino Bold"
		"Vera Sans Bold Italic.font",		// 31	"Palatino Bold Italic"
		"Vera Sans.font",					// 32	"Symbol"
		"Vera Sans.font",					// 33	"Zapf Chancery Medium Italic"
		"Vera Sans.font",					// 34	"Zapf Dingbats"
		"Vera Sans.font",					//  0	Latex "Default"
		"Vera Sans.font",					//  1	Latex "Roman"
		"Vera Sans Bold.font",				//  2	Latex "Bold"
		"Vera Sans Italic.font",			//  3	Latex "Italic"
		"dejavuserifbook.font",				//  4	Latex "Modern"
		"fixed.font"						//  5	Latex "Typewriter"
	},
#else
	{
		"CGTimes.font",									// -1	"default"
		"Bitstream Vera Sans.font",						// 0	"Times Roman"
		"Bitstream Vera Sans Oblique.font",				// 1	"Times Italic"
		"Bitstream Vera Sans Bold.font",				// 2	"Times Bold"
		"Bitstream Vera Sans Bold Oblique.font",		// 3	"Times Bold Italic"
		"DejaVu Sans.font",								// 4	"Avantgarde Book"
		"DejaVu Sans Oblique.font",						// 5	"Avantgarde Book Oblique"
		"DejaVu Sans Bold.font",						// 6	"Avantgarde Demi"
		"DejaVu Sans Bold Oblique.font",				// 7	"Avantgarde Demi Oblique"
		"URW Bookman L Light.font",						// 8	"Bookman Light"
		"URW Bookman L Light Italic.font",				// 9	"Bookman Light Italic"
		"URW Bookman L Demi Bold.font",					// 10	"Bookman Demi"
		"URW Bookman L Demi Bold Italic.font",			// 11	"Bookman Demi Italic"
		"Nimbus Mono L Regular.font",					// 12	"Courier"
		"Nimbus Mono L Regular Oblique.font",			// 13	"Courier Oblique"
		"Nimbus Mono L Regular Oblique.font",			// 14	"Courier Bold"
		"Nimbus Mono L Bold Oblique.font",				// 15	"Courier Bold Oblique"
		"Nimbus Sans L Regular.font",					// 16	"Helvetica"
		"Nimbus Sans L Regular Italic.font",			// 17	"Helvetica Oblique"
		"Nimbus Sans L Bold.font",						// 18	"Helvetica Bold"
		"Nimbus Sans L Bold Italic.font",				// 19	"Helvetica Bold Oblique"
		"Nimbus Sans L Regular Condensed.font",			// 20	"Helvetica Narrow"
		"Nimbus Sans L Regular Condensed Italic.font",	// 21	"Helvetica Narrow Oblique"
		"Nimbus Sans L Bold Condensed.font",			// 22	"Helvetica Narrow Bold"
		"Nimbus Sans L Bold Condensed Italic.font",		// 23	"Helvetica Narrow Bold Oblique"
		"Century Schoolbook L Roman.font",				// 24	"NewCentury Schoolbook Roman"
		"Century Schoolbook L Italic.font",				// 25	"NewCentury Schoolbook Italic"
		"Century Schoolbook L Bold.font",				// 26	"NewCentury Schoolbook Bold"
		"Century Schoolbook L Bold Italic.font",		// 27	"NewCentury Schoolbook Bold Italic"
		"URW Palladio L Roman.font",					// 28	"Palatino Roman"
		"URW Palladio L Italic.font",					// 29	"Palatino Italic"
		"URW Palladio L Bold.font",						// 30	"Palatino Bold"
		"URW Palladio L Bold Italic.font",				// 31	"Palatino Bold Italic"
		"Standard Symbols L.font",						// 32	"Symbol"
		"URW Chancery L Medium Italic.font",			// 33	"Zapf Chancery Medium Italic"
		"Dingbats.font",								// 34	"Zapf Dingbats"
		"CGTimes.font",									// 0	Latex "Default"
		"Bitstream Vera Sans.font",						// 1	Latex "Roman"
		"Bitstream Vera Sans Oblique.font",				// 2	Latex "Bold"
		"Bitstream Vera Sans Bold.font",				// 3	Latex "Italic"
		"Bitstream Vera Sans Bold Oblique.font",		// 4	Latex "Modern"
		"Fixed.font"									// 5	Latex "Typewriter"
	},
#endif

	// Misc parameters
	1024,					// ULONG	printbuf;		Printing buffer size 
    FALSE,                  // BOOL     DEBUG;      
    TRUE,                   // BOOL	    flushleft;		center/flush-left printing
    FALSE,                  // BOOL		multiple;		multiple/single page for export/print
    -2,                     // LONG		transparent;	transparent color for GIF export (-2=none, -1=background)
    100.0,                  // float	magnification;	export/print magnification
    FALSE,                  // BOOL     allownegcoords;
	FRACT_UNIT,				// LONG		gridunit;		grid unit to be used in INCHES mode
    TRUE,                   // BOOL     autosave;       Automatically saves the settings at exit
};

/* Setting file functions */
void ApplySettings(struct appresStruct *tmp_appres)
{
	memcpy((void *)&appres, (void *)tmp_appres, sizeof(struct appresStruct));
	if (appres.INCHES)
	{
		cur_gridunit = appres.gridunit;
	}
	else
	{
		cur_gridunit = MM_UNIT;
	}
	strcpy(cur_fig_units, appres.INCHES ? "in" : "cm");
}

static BOOL ValidateSettings(struct appresStruct *res)
{
	/* Basic ranges check to handle corrupt files */
	if (res->papersize < 0 || res->papersize >= NUMPAPERSIZES) return FALSE;
	if (res->magnification < 0.01 || res->magnification > 2000.0) return FALSE;
	if (res->ScreenWidth < 0 || res->ScreenWidth > 16384) return FALSE;
	if (res->ScreenHeight < 0 || res->ScreenHeight > 16384) return FALSE;
	if (res->ScreenDepth < 0 || res->ScreenDepth > 32) return FALSE;
	if (res->CanvasWidth < 100 || res->CanvasWidth > 16384) return FALSE;
	if (res->CanvasHeight < 100 || res->CanvasHeight > 16384) return FALSE;
	if (res->printbuf < 128 || res->printbuf > 1024*1024*10) return FALSE;
	if (res->userscale <= 0.0) return FALSE;
	if (res->gridunit < 0 || res->gridunit > 2) return FALSE;

	/* Ensure critical strings are null-terminated */
	if (memchr(res->PubScreenName, 0, sizeof(res->PubScreenName)) == NULL) return FALSE;
	if (memchr(res->skin, 0, sizeof(res->skin)) == NULL) return FALSE;
	if (memchr(res->DefTool, 0, sizeof(res->DefTool)) == NULL) return FALSE;
	if (memchr(res->f2d_cmd, 0, sizeof(res->f2d_cmd)) == NULL) return FALSE;

	return TRUE;
}

BOOL ReadSettings(BOOL AskForFile, struct appresStruct *tmp_appres)
{
	FILE *file = NULL;
	struct stat status;
	char version[32];
    BOOL success = FALSE;

	if(AskForFile)
	{
		if(!DoFileRequest((UBYTE *)"Open Settings...", (UBYTE *)TempName, FR_FIGPREFS | FR_DOLOAD))
			return(FALSE);
	}
	else
		strmfp(TempName, homedir, PrefsName); // make filename from homedir and PrefsName

	if((file=fopen(TempName, "rb")) != NULL)
	{
		if (fgets(version, sizeof(version), file))
		{
			char *p = strchr(version, '\n');
			if (p) *p = '\0';
			p = strchr(version, '\r');
			if (p) *p = '\0';

			if(strcmp(settingsVersion, version) == 0)
			{
				if(stat(TempName, &status) == 0)
				{
					if(status.st_size == strlen(settingsVersion) + 1 + sizeof(struct appresStruct))
					{
						if(fread(tmp_appres, sizeof(struct appresStruct), 1, file) == 1)
						{
							if (ValidateSettings(tmp_appres))
							{
								success = TRUE;
							}
						}
					}
				}
			}
		}
		fclose(file);
	}
	return(success);
}

BOOL SaveSettings(BOOL AskForFile, struct appresStruct *tmp_appres)
{
	FILE *file;
	int blocknumber;
    BOOL success = FALSE;

	if(AskForFile)
	{
		if(!DoFileRequest((UBYTE *)"Save Settings...", (UBYTE *)TempName, FR_FIGPREFS | FR_DOSAVE))
			return(FALSE);
	}
	else
	{
		strmfp(TempName,homedir,PrefsName);
	}
	
	if((file=fopen(TempName,"wb")) != NULL)
	{
		fprintf(file, "%s\n", settingsVersion);

		if((blocknumber=fwrite(tmp_appres, sizeof(struct appresStruct), 1, file)) == 1)
		{
			success = TRUE;
		}
		fclose(file);
	}
	return(success);
}

/*
	The following functions support change of context, i.e. having several fig files
	openned at same time in different canvas windows.
	This is a hack currently and should be reworked in the future.
	Also these function shall be copied in their own file.
*/
#ifdef AMIFIG
struct FigContext figContexts[MAXWINDOWS];
struct FigContext *curContext=NULL;
extern int  last_toprul_pos, last_siderul_pos;
extern int  wait_count;		// number of wait-calls
extern BOOL waiting;		// waitstate
extern struct MsgPort	*appWinPort;
extern struct AppWindow	*appWin;

void copy_objects(F_compound *src, F_compound *dst)
{
	dst->tagged		= src->tagged;
	dst->distrib	= src->distrib;
	dst->nwcorner.x	= src->nwcorner.x;
	dst->nwcorner.y	= src->nwcorner.y;
	dst->secorner.x	= src->secorner.x;
	dst->secorner.y	= src->secorner.y;
	dst->lines		= src->lines;
	dst->ellipses	= src->ellipses;
	dst->splines	= src->splines;
	dst->texts		= src->texts;
	dst->arcs		= src->arcs;
	dst->compounds	= src->compounds;
	dst->ignore		= src->ignore;
	dst->next		= src->next;
}

void clear_objectVars(F_compound *objPointers)
{
	objPointers->tagged		= 0;
	objPointers->distrib	= 0;
	objPointers->nwcorner.x	= 1200;
	objPointers->nwcorner.y	= 0;
	objPointers->secorner.x	= 0;
	objPointers->secorner.y	= 0;
	objPointers->lines		= 0;
	objPointers->ellipses	= 0;
	objPointers->splines	= 0;
	objPointers->texts		= 0;
	objPointers->arcs		= 0;
	objPointers->compounds	= 0;
	objPointers->ignore		= 0;
	objPointers->next		= 0;
}

void CreateNewContext(void)
{
	int first;
	struct FigContext *SavedContext = NULL;
	
	stop_action();
	saveContext(canvas_win);

	SavedContext = curContext;

	if((first=firstAvailableContext()) == -1)
		return;

	InitContext(first);

	if(setup_canvaswin(first))
	{
		restoreContext(SavedContext);
		return;
	}
	ActivateWindow(canvas_win);
	initTmpRasContexts(&tmpras);
	redisplay_canvas();
}


int firstAvailableContext(void)
{
	int i;

	for(i=0; i < MAXWINDOWS; i++)
		if(figContexts[i].paintWnd == NULL)
			return(i);

	return(-1);
}

void ClearContexts(void)
{
	int i;

	for(i=0; i < MAXWINDOWS; i++)
		figContexts[i].paintWnd=NULL;
}

void InitContext(int i)
{
	if(i < MAXWINDOWS)
	{
		clear_objectVars(&fobjects);
		clear_objectVars(&saved_objects);
		clear_objectVars(&object_tails);
		
		figContexts[i].last_action			= 0;
		figContexts[i].last_object			= 0;
		figContexts[i].figure_modified		= 0;
		figContexts[i].CanvasLeft			= appres.CanvasLeft + i * 20;
		figContexts[i].CanvasTop 			= appres.CanvasTop + i * 20;
		figContexts[i].CanvasWidth 			= appres.CanvasWidth;
		figContexts[i].CanvasHeight			= appres.CanvasHeight;
		figContexts[i].waiting 				= FALSE;
		figContexts[i].wait_count 			= 0;
		figContexts[i].cur_cursor 			= arrow_cursor;
		figContexts[i].display_zoomscale	= 1.0;
		figContexts[i].landscape	 		= appres.landscape;
		figContexts[i].cur_gridmode  		= cur_gridmode;
		figContexts[i].cur_pointposn 		= cur_pointposn;
		figContexts[i].topruler_on	 		= appres.topruler_on;
		figContexts[i].sideruler_on  		= appres.sideruler_on;
		figContexts[i].last_toprul_pos 		= last_toprul_pos ;
		figContexts[i].last_siderul_pos		= last_siderul_pos;
		figContexts[i].Unit			 		= appres.INCHES;
		figContexts[i].PageFormat			= appres.papersize;
		strcpy(figContexts[i].current_file			, "New.fig");
		strcpy(figContexts[i].latest_current_file	, "");
		strcpy(figContexts[i].FileName				, "New.fig");
		strcpy(figContexts[i].DirectoryName			, "");
		strcpy(figContexts[i].LatestFileName		, "");
		strcpy(figContexts[i].LatestDirectoryName	, "");
		
		last_action 		= figContexts[i].last_action;
		last_object 		= figContexts[i].last_object;
		figure_modified 	= figContexts[i].figure_modified;
		display_zoomscale 	= figContexts[i].display_zoomscale;
		strcpy(DirectoryName		, figContexts[i].DirectoryName);
		strcpy(LatestFileName		, figContexts[i].LatestFileName);
		strcpy(LatestDirectoryName	, figContexts[i].LatestDirectoryName);
				
		xshift		= 0; 
		yshift		= 0;
		last_xshift	= 0; 
		last_yshift	= 0;
	}
}

void renewContext(struct Window *wnd, int i)
{
	if(wnd == NULL)
		return;

	figContexts[i].paintWnd		= wnd;
	figContexts[i].rastport		= rastport;
	figContexts[i].appWinPort	= appWinPort;
	figContexts[i].appWin		= appWin;
	figContexts[i].horscroller  = horscroller;
	figContexts[i].vertscroller = vertscroller ;
	
	curContext=&figContexts[i];
}

void saveContext(APTR wnd)
{
	int i;

	if(wnd == NULL)
		return;

	for(i=0; i < MAXWINDOWS; i++)
	{
		if((APTR)figContexts[i].paintWnd == wnd)
		{
			copy_objects(&fobjects,			&figContexts[i].fobjects);
			copy_objects(&saved_objects,	&figContexts[i].saved_objects);
			copy_objects(&object_tails,		&figContexts[i].object_tails);
			
			figContexts[i].last_action		= last_action;
			figContexts[i].last_object		= last_object;
			figContexts[i].figure_modified	= figure_modified;
			figContexts[i].CanvasLeft		= appres.CanvasLeft;
			figContexts[i].CanvasTop 		= appres.CanvasTop;
			figContexts[i].CanvasWidth 		= appres.CanvasWidth;
			figContexts[i].CanvasHeight		= appres.CanvasHeight;
			figContexts[i].waiting 			= waiting;
			figContexts[i].wait_count 		= wait_count;
			figContexts[i].cur_cursor 		= cur_cursor;
			figContexts[i].xshift			= xshift;
			figContexts[i].yshift			= yshift;
			figContexts[i].last_xshift		= last_xshift;
			figContexts[i].last_yshift		= last_yshift;
			figContexts[i].display_zoomscale= display_zoomscale;
			figContexts[i].landscape	 	= appres.landscape;
			figContexts[i].cur_gridmode  	= cur_gridmode;
			figContexts[i].cur_pointposn 	= cur_pointposn;
			figContexts[i].topruler_on	 	= appres.topruler_on;
			figContexts[i].sideruler_on  	= appres.sideruler_on;
			figContexts[i].last_toprul_pos 	= last_toprul_pos ;
			figContexts[i].last_siderul_pos	= last_siderul_pos;
			figContexts[i].Unit			 	= appres.INCHES;
			figContexts[i].PageFormat	 	= appres.papersize;

			strcpy(figContexts[i].current_file, current_file);
			strcpy(figContexts[i].latest_current_file, latest_current_file);
			strcpy(figContexts[i].FileName, FileName);
			strcpy(figContexts[i].DirectoryName, DirectoryName);
			strcpy(figContexts[i].LatestFileName, LatestFileName);
			strcpy(figContexts[i].LatestDirectoryName, LatestDirectoryName);
		}
	}
}

void restoreContext(APTR wnd)
{
	int i;

	if(wnd == NULL)
		return;

	for(i=0; i < MAXWINDOWS; i++)
	{
		if((APTR)figContexts[i].paintWnd == wnd)
		{
			copy_objects(&figContexts[i].fobjects, &fobjects);
			copy_objects(&figContexts[i].saved_objects, &saved_objects);
			copy_objects(&figContexts[i].object_tails, &object_tails);

			last_action				= figContexts[i].last_action;
			last_object				= figContexts[i].last_object;
			figure_modified			= figContexts[i].figure_modified;
			canvas_win			    = figContexts[i].paintWnd;
			rastport				= figContexts[i].rastport;
			appWinPort				= figContexts[i].appWinPort;
			appWin					= figContexts[i].appWin;
			appres.CanvasLeft		= figContexts[i].CanvasLeft;
			appres.CanvasTop 		= figContexts[i].CanvasTop;
			appres.CanvasWidth 		= figContexts[i].CanvasWidth;
			appres.CanvasHeight		= figContexts[i].CanvasHeight;
			horscroller  			= figContexts[i].horscroller;
			vertscroller 			= figContexts[i].vertscroller;
			waiting 				= figContexts[i].waiting;
			wait_count 				= figContexts[i].wait_count;
			cur_cursor 				= figContexts[i].cur_cursor;
			xshift					= figContexts[i].xshift;
			yshift					= figContexts[i].yshift;
			last_xshift				= figContexts[i].last_xshift;
			last_yshift				= figContexts[i].last_yshift;
			display_zoomscale		= figContexts[i].display_zoomscale;
			appres.landscape		= figContexts[i].landscape;
			cur_gridmode			= figContexts[i].cur_gridmode;
			cur_pointposn			= figContexts[i].cur_pointposn;
			appres.topruler_on		= figContexts[i].topruler_on;
			appres.sideruler_on		= figContexts[i].sideruler_on;
			last_toprul_pos			= figContexts[i].last_toprul_pos;
			last_siderul_pos		= figContexts[i].last_siderul_pos;
			appres.INCHES			= figContexts[i].Unit;
			appres.papersize		= figContexts[i].PageFormat;

			strcpy(current_file, figContexts[i].current_file);
			strcpy(latest_current_file, figContexts[i].latest_current_file);
			strcpy(FileName, figContexts[i].FileName);
			strcpy(DirectoryName, figContexts[i].DirectoryName);
			strcpy(LatestFileName, figContexts[i].LatestFileName);
			strcpy(LatestDirectoryName, figContexts[i].LatestDirectoryName);

			reset_cursor();
			curContext=&figContexts[i];
		}
	}
}

int restoreAnyContext(void)
{
	int i;

	for(i=0; ((i < MAXWINDOWS) && (figContexts[i].paintWnd == NULL)); i++);

	if(i < MAXWINDOWS)
	{
		restoreContext((APTR)figContexts[i].paintWnd);
		return(i);
	}

	return(-1);
}

void deleteContext(APTR paintWnd)
{
	int i;

	for(i=0; i < MAXWINDOWS; i++)
	{
		if((APTR)figContexts[i].paintWnd == paintWnd)
		{
			figContexts[i].paintWnd=NULL;
			if((struct Window *)paintWnd == canvas_win || canvas_win == NULL)
			{
				canvas_win=NULL;
				rastport=NULL;
				curContext=NULL;
			}
		}
	}
}

void initTmpRasContexts(struct TmpRas *tmpras)
{
	int i=0;

	for(; i < MAXWINDOWS; i++)
	{
		if(figContexts[i].paintWnd != NULL)
		{
			figContexts[i].rastport->TmpRas=tmpras;
		}
	}
}

struct DrawContext save_drawcontext(void)
{
	struct DrawContext DC = {0,0,0,0,0};
	
	DC.ViewBestQuality = appres.viewBestQuality;
	appres.viewBestQuality = TRUE;

	DC.xshift=xshift;
	DC.yshift=yshift;

	// Remember old size of tempraster, so we can set it back to the previous
	// size, because printing needs a far bigger temporary raster than screen
	// painting, and we don't want to waste chip memory.
	DC.rastwidth  = rastwidth;
	DC.rastheight = rastheight;

	xshift=0; yshift=0;
	
	return(DC);
}

void restore_drawcontext(struct DrawContext DC)
{
	appres.viewBestQuality = DC.ViewBestQuality;
	xshift=DC.xshift; 
	yshift=DC.yshift;

	if(SetTempRaster(DC.rastwidth, DC.rastheight, TRUE) < 0)
    {
        put_msg((char *)_(msg_OutOfMem));
    }
}
#endif

#ifdef __GNUC__
/* Get the filename extension. */
int stcgfe(char *ext, const char *name)
{
   const char *p = name + strlen(name);
   const char *q = p;
   while (p > name && *--p != '.' && *p != '/' && *p != ':');
   if (*p++ == '.' && q - p < FESIZE)
   {
      memcpy(ext, p, q - p + 1);
      return q - p;
   }
   *ext = '\0';
   return 0;
}

/* Get the path. */
int stcgfp(char *path, const char *name)
{
   const char *p = name + strlen(name);
   while (p > name && *--p != '/' && *p != ':');
   if (p - name > 0)
   {
      memcpy(path, name, p - name);
      path[p-name+1] = '\0';
      return p - name;
   }
   *path = '\0';
   return 0;
}

/* Get the base name. */
int stcgfn(char *node, const char *name)
{
   const char *p = name + strlen(name);
   const char *q = p;
   while (p > name && p[-1] != '/' && p[-1] != ':')
      --p;
   if (q - p < FNSIZE)
   {
      memcpy(node, p, q - p + 1);
      return q - p;
   }
   *node = '\0';
   return 0;
}

/* Append a file name to a path. */
void strmfp(char *name, const char *path, const char *node)
{
  size_t len = path ? strlen(path) : 0;
  if (len)
  {
    memcpy(name, path, len);
    if (name[len-1] != '/' && name[len-1] != ':')
      name[len++] = '/';
  }
  strcpy(name + len, node);
}

/* Convert a signed int to a string */
int stci_d(char *out, int in)
{
	sprintf(out, "%d", in);
	return strlen(out);
}
#endif

char
*my_strdup(char *str)
{
    char *s;
    
    if (str==NULL)
        return NULL;
    
    s = new_string(strlen(str)+1);
    if (s!=NULL) 
        strcpy(s, str);
    
    return s;
}

void FreePixBuf_aligned(struct pixelbuffer *pb)
{
	if (pb->data != NULL)
    {
		FreeVec(pb->data);
    }
	pb->data = (ULONG *)NULL;
	pb->data_aligned = (ULONG *)NULL;
	pb->size   = 0;
	pb->pitch  = 0;
}

BOOL AllocPixBuf_aligned(struct pixelbuffer *pb, int width, int height, int alignement)
{
	/*
		width and height in pixel : 32 bits(4 Bytes) per pixel
		alignement in Bytes
	*/
	LONG size, pitch;
	BOOL success = FALSE;
	
	/* ensure that each line starts at an aligned adress */
    if (alignement > 3)
        pitch = (width%(alignement >> 2)==0)?width:(width/(alignement >> 2) + 1) * (alignement >> 2);
	else
		pitch = width;

	/* ensure height is even */
	height += height%2;
	
	/* calculate needed buffer size */
	size    = (4 * pitch * height) + (alignement - 1);	
	
	if (pb->size < size)
	{
		/* if buffer size isn't big enough (or not existing) create a new one */
		FreePixBuf_aligned(pb);
		pb->data   = (ULONG *)AllocVec(size, MEMF_ANY);
		if (pb->data != NULL)
		{
			pb->size   = size;
			pb->pitch  = pitch;
		}
	}
	else
	{
		/* only update pitch for current need, size is kept */
		pb->pitch  = pitch;
	}
	
	if (pb->data != NULL)
	{
		/* finally calculate start adress of aligned data */
		pb->data_aligned = pb->data;
		while ((IPTR)pb->data_aligned & (alignement-1))
		{
			pb->data_aligned++;
		}
		success = TRUE;
	}
	
	return(success);
}
