/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/intui.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"
#include <proto/gadtools.h>
#include <proto/muimaster.h>
#include <devices/printer.h>

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "w_msgpanel.h"
#include "w_canvas.h"
#include "u_redraw.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "f_picobj.h"
#include "w_rulers.h"
#include "a_antialias.h"
#include "a_rexxiface.h"
#include "a_color.h"
#include "w_cursor.h"
#include "intui.h"
#include "u_fonts.h"
#include "menu.h"
#include "z_fig.h"

/* Prototypes */
int 	NewScreen(void);
void	RemoveScreen(void);
int 	openWin(void);

/* Global Variables */
#ifdef __amigaos4__
extern struct Library *RexxSysBase;

struct Library			    *CyberGfxBase;
struct Library 			    *MUIMasterBase;

struct CyberGfxIFace	    *ICyberGfx;
struct ConsoleIFace		    *IConsole;
struct MUIMasterIFace	    *IMUIMaster;
#endif

struct Device				*InputBase        = NULL;


struct TextAttr 			GUIFont_Fixed_Attr    = {( STRPTR )"topaz.font", 8, 0x00, 0x01 };
struct TextFont 			*GUIfont_Fixed 	      = NULL;

struct MsgPort				*InputPort            = NULL;
struct IOStdReq				*InputRequest         = NULL;
struct MsgPort				*printerPort          = NULL;
struct IODRPReq				*printerRequest       = NULL;

BOOL						is_on_pubscreen = FALSE;

struct Screen				*Scr           = NULL;
UBYTE						*PubScreenName = NULL;
struct NewMenu  			*AmiFIGNM     = NULL;
struct Menu      			*ZFig_Menus    = NULL;

/* Functions */
BOOL ReopenScreen(void)
{
	int i;
	BOOL success = FALSE;
	BOOL winopen[MAXWINDOWS];

	/* Close all paint windows */
	saveContext(canvas_win);
	for(i=0; i < MAXWINDOWS; i++)
	{
		if(figContexts[i].paintWnd != NULL)
		{
			winopen[i]=TRUE;
			restoreContext((APTR)figContexts[i].paintWnd);
			close_canvaswin(FALSE);
		}
		else
		{
			winopen[i]=FALSE;
		}
	}
    
    CleanUp_AAbuffers();

	/* Close Screen */
	RemoveScreen();
	
	/* Open New Screen */ 
	if(NewScreen() == 0)
	{
		/* ReOpen paint Windows */
		for(i=0; i < MAXWINDOWS; i++)
		{
			if(winopen[i])
			{
				appres.CanvasLeft   = figContexts[i].CanvasLeft;
				appres.CanvasTop    = figContexts[i].CanvasTop;
				appres.CanvasWidth  = figContexts[i].CanvasWidth;
				appres.CanvasHeight = figContexts[i].CanvasHeight;

				setup_canvaswin(i);
				restoreContext(canvas_win);
				redisplay_canvas();
				reload_pictures();
			}
		}

		/* Initialise Temporary Raster contexts */
		initTmpRasContexts(&tmpras);
		success = TRUE;
	}

	return(success);
}

int SetupCustomScreen(void)
{
	ULONG  			  screen_modeID;
	struct 	Screen   *pub_screen = NULL;
	struct 	DrawInfo *screen_drawinfo = NULL;
	UWORD			  DriPens[] = {0xffff};
	int 			  err = 1;

	if (appres.ScreenMode)
	{
		Scr = OpenScreenTags(  	NULL, 
								SA_Left,			0,
		                        SA_Top,				0,
		                        SA_Width,			appres.ScreenWidth,
		                        SA_Height,			appres.ScreenHeight,
		                        SA_Depth,			appres.ScreenDepth,
		                        SA_Type,			PUBLICSCREEN,
								SA_PubName,  		"AmiFIG",
								SA_LikeWorkbench, 	TRUE,
		                        SA_DisplayID,		appres.ScreenMode,
		                        SA_AutoScroll,		appres.AutoScroll,
		                        SA_Overscan,		OSCAN_TEXT,
		                        SA_Pens,			&DriPens[0],
								SA_SharePens,		TRUE,
								SA_ColorMapEntries, 256,
								SA_FullPalette, 	TRUE,
		                        SA_Title,			"AmiFIG",
		                        TAG_DONE );
	}
	
	if (Scr == NULL)
	{
		// trying to clone WB screen instead
		pub_screen = LockPubScreen((CONST_STRPTR)"Workbench");
		Scr = pub_screen;
		
		if(pub_screen != NULL)
		{
			doMessage((char *)_(msg_PreferredScreenErr));

			/* Get the DrawInfo structure from the locked screen
			** This returns pen, depth and font info.
			*/
			screen_drawinfo = GetScreenDrawInfo(pub_screen);

			if(screen_drawinfo != NULL)
			{
				screen_modeID = GetVPModeID(&(pub_screen->ViewPort));

				if( screen_modeID != INVALID_ID )
				{
                    {
						/* screen_modeID may now be used in a call to
						** OpenScreenTagList() with the tag SA_DisplayID.
						*/						
						Scr = OpenScreenTags(	NULL,
												SA_LikeWorkbench, 	TRUE,
												SA_Pens,			&DriPens[0],
												SA_SharePens,		TRUE,
												SA_ColorMapEntries, 256,
												SA_FullPalette, 	TRUE,
												SA_Type,			PUBLICSCREEN,
												SA_PubName,  		"AmiFIG",
												SA_Title,      		"AmiFIG",
												TAG_END);

						if (Scr != NULL)
						{
							appres.ScreenWidth  = pub_screen->Width;
							appres.ScreenHeight = pub_screen->Height;
							appres.ScreenDepth  = GetBitMapAttr(pub_screen->RastPort.BitMap, BMA_DEPTH);
							appres.ScreenMode   = screen_modeID;
							appres.AutoScroll   = TRUE;
						}

						/* Free the drawinfo and public screen as we don't
						** need them any more.  We now have our own screen.
						*/
						FreeScreenDrawInfo(pub_screen,screen_drawinfo);
						UnlockPubScreen((UBYTE *)"Workbench", pub_screen);
					}
				}
			}
		}
	}

	if(Scr != NULL)
	{
		PubScreenStatus( Scr, 0 );
		err = 0;
	}	
	return(err);
}

int NewScreen(void)
{
	int err = 1;
	
    is_on_pubscreen = FALSE;
	/* Try to lock PubScreen for display is requested */
	if(appres.UsePubScreen)
	{
		PubScreenName=appres.PubScreenName;
		if (( Scr = LockPubScreen( PubScreenName )))
		{
			/* Succeed to lock PubScreen */
			is_on_pubscreen=TRUE;
			err = 0;
		}
		else
		{
			/* PubScreen couldn't be locked */
			printf((const char *)_(msg_PublicScreenErr));
		}
	}
	
	/* If PubScreen not requested or locking failed, SetUp custom screen */
	if(is_on_pubscreen == FALSE)
	{
		err = SetupCustomScreen(); 
	}

	return(err);
}

void RemoveScreen(void)
{    	
	if(Scr)
	{
		if(is_on_pubscreen) 
			UnlockPubScreen( NULL, Scr );
		else
			CloseScreen( Scr );
		Scr = NULL;
	}
}

int openWin(void)
{
	int first;
	int err = 1;

	if(NewScreen() == 0)
	{
		if((first=firstAvailableContext()) != -1)
		{
			InitContext(first);
			if(setup_canvaswin(first) == 0)
			{
				reload_pictures();
				err = 0;
			}
		}
	}
	else
	{
		printf((const char *)_(msg_OpenScreenErr));
	}
	return(err);
}

void CloseAll(void)
{
    int i;

    if (appres.autosave == TRUE)
    {
        /* Save settings */    
        SaveSettings(FALSE, &appres);
	}
    
	closeRexxIFace();
    
    /* Free Gradient table */
    for (i=0; i<MAX_USR_COLS;i++)
    {
        if (GradientTable[i] != NULL)
        {
            FreeVec(GradientTable[i]);
        }
    }
    
    free_pointlist(&GlobalPoints);

    CleanUp_AAbuffers();
    
	if(tempraster)
	{
		FreeRaster(tempraster, rastwidth, rastheight);
		tempraster=NULL;
		initTmpRasContexts(NULL);
	}

	if(InputRequest)
	{
		if(InputRequest -> io_Device)
		{
			CloseDevice((struct IORequest *)InputRequest);
		}
#ifdef __amigaos4__
		FreeSysObject(ASOT_IOREQUEST, InputRequest);
#else
		DeleteStdIO(InputRequest);
#endif
		InputRequest = NULL;
	}

	if(InputPort)
	{
#ifdef __amigaos4__
		FreeSysObject(ASOT_PORT, InputPort);
#else
		DeletePort(InputPort);
#endif
		InputPort = NULL;
	}

	if(printerPort)
	{
#ifdef __amigaos4__
		FreeSysObject(ASOT_PORT, printerPort);
#else
		DeletePort(printerPort);
#endif
		printerPort = NULL;
	}

	if(printerRequest)
	{
#ifdef __amigaos4__
		FreeSysObject(ASOT_IOREQUEST, printerRequest);
#else
		DeleteExtIO((struct IORequest *)printerRequest);
#endif
		printerRequest=NULL;
	}

	/* Close all paint windows */
	for(i=0; i < MAXWINDOWS; i++)
	{
		if(figContexts[i].paintWnd != NULL)
		{
			restoreContext((APTR)figContexts[i].paintWnd);
			close_canvaswin(FALSE);
			deleteContext((APTR)figContexts[i].paintWnd);
		}
	}
		
	if(ZFig_Menus)
	{
		FreeMenus( ZFig_Menus );
		ZFig_Menus = NULL;
	}
	if(AmiFIGNM)
	{
	    FreeVec(AmiFIGNM);
	}
	
	/* Close Screen */
	RemoveScreen();

	if(clip_region)
	{
		DisposeRegion(clip_region);
	}

	if(ref_clip_region)
	{
		DisposeRegion(ref_clip_region);
	}
		
	if(GUIfont_Fixed)
	{
		CloseFont(GUIfont_Fixed);
		GUIfont_Fixed = NULL;
	}
		
	dispose_cursor();
#ifdef __amigaos4__
    if(MUIMasterBase)   CloseLibrary(MUIMasterBase);
    if(CyberGfxBase)    CloseLibrary(CyberGfxBase);
#endif
}

BOOL OpenAll(void)
{
	/* NewMenu structure */
    struct NewMenu AmiFIGNM_def[] = {
        {NM_TITLE,	(STRPTR)_(msg_MenuProject)				,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuNew) 			,(STRPTR)"N", 0, 0L, (APTR)NEW},
        {NM_ITEM,		(STRPTR)_(msg_MenuOpen)			,(STRPTR)"O", 0, 0L, (APTR)OPEN},
        {NM_ITEM,		(STRPTR)_(msg_MenuInsert)			,(STRPTR)"I", 0, 0L, (APTR)INSERT},
        {NM_ITEM,		(STRPTR)_(msg_MenuImport)			,NULL		, 0, 0L, (APTR)0},
        {NM_SUB,			(STRPTR)"SVG"				,NULL		, 0, 0L, (APTR)IMPSVG},
        {NM_SUB,			(STRPTR)_(msg_MenuR2V)		    ,NULL		, 0, 0L, (APTR)IMPPOTRACE},
        {NM_ITEM,		(STRPTR)_(msg_MenuSave)			,(STRPTR)"S", 0, 0L, (APTR)SAVE},
        {NM_ITEM,		(STRPTR)_(msg_MenuSaveAs)			,(STRPTR)"A", 0, 0L, (APTR)SAVEAS},
        {NM_ITEM,		(STRPTR)_(msg_MenuExport)			,NULL		, 0, 0L, (APTR)0},
        {NM_SUB,			(STRPTR)_(msg_MenuAsPNG)	    ,NULL		, 0, 0L, (APTR)EXPORTPNG},
        {NM_SUB,			(STRPTR)_(msg_MenuViaF2D)	    ,NULL		, 0, 0L, (APTR)EXPORTF2D},
        {NM_ITEM,		(STRPTR)_(msg_MenuPrint)			,NULL		, 0, 0L, (APTR)PRINT},
        {NM_ITEM,		(STRPTR)_(msg_MenuPrintPS)			,NULL		, 0, 0L, (APTR)PRINTPS},
        {NM_ITEM,		(STRPTR)NM_BARLABEL				,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuSandQ)			,NULL		, 0, 0L, (APTR)SAVEQUIT},
        {NM_ITEM,		(STRPTR)_(msg_MenuQuit)			,(STRPTR)"Q", 0, 0L, (APTR)QUIT},
        {NM_TITLE,	(STRPTR)_(msg_MenuEdit)				,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuZoom)			,(STRPTR)"Z", 0, 0L, (APTR)TOGGLEZOOM},
        {NM_ITEM,		(STRPTR)_(msg_MenuColor)			,(STRPTR)"C", 0, 0L, (APTR)TOGGLECOLOR},
        {NM_ITEM,		(STRPTR)_(msg_MenuOpenLibWnd)		,(STRPTR)"L", 0, 0L, (APTR)TOGGLELIB},
		{NM_ITEM,		(STRPTR)_(msg_HelpWnd) 			    ,(STRPTR)"H", 0, 0L, (APTR)TOGGLEHELP},
        {NM_ITEM,		(STRPTR)NM_BARLABEL				,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuUndo)			,(STRPTR)"U", 0, 0L, (APTR)UNDO},
        {NM_ITEM,		(STRPTR)_(msg_MenuRedisplay)		,(STRPTR)"R", 0, 0L, (APTR)REDISP},
        {NM_ITEM,		(STRPTR)_(msg_MenuClear)			,NULL		, 0, 0L, (APTR)CLEAR},
        {NM_TITLE,	(STRPTR)_(msg_MenuSettings)			,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuEditSettings)	,NULL		, 0, 0L, (APTR)EDITPREFS},
        {NM_ITEM,		(STRPTR)NM_BARLABEL				,NULL		, 0, 0L, (APTR)0},
        {NM_ITEM,		(STRPTR)_(msg_MenuStatus)			,(STRPTR)"?", 0, 0L, (APTR)STATUS},
        {NM_ITEM,		(STRPTR)_(msg_MenuAbout)			,NULL		, 0, 0L, (APTR)ABOUT},
        {NM_END, 	NULL								,NULL		, 0, 0L, (APTR)0}
    };
    
	ClearContexts();

#ifdef __amigaos4__
    MUIMasterBase = OpenLibrary((CONST STRPTR)"muimaster.library", 0);
	CyberGfxBase = OpenLibrary("cybergraphics.library", 40L);
    IMUIMaster = (struct MUIMasterIFace *)GetInterface(MUIMasterBase, "main", 1, NULL);
	ICyberGfx = (struct CyberGfxIFace *) GetInterface( CyberGfxBase, "main", 1, NULL );
#endif


#ifdef __amigaos4__
	if(!(InputPort = (struct MsgPort *)AllocSysObjectTags(ASOT_PORT, TAG_DONE)))
#else
	if(!(InputPort = (struct MsgPort *)CreatePort(NULL, 0)))
#endif
		return(FALSE);

#ifdef __amigaos4__
	if(!(InputRequest = (struct IOStdReq *)AllocSysObjectTags(ASOT_IOREQUEST,
		ASOIOR_ReplyPort, InputPort,
		ASOIOR_Size, sizeof(struct IOStdReq),
		TAG_DONE)))
#else
	if(!(InputRequest = (struct IOStdReq *)CreateStdIO(InputPort)))
#endif
		return(FALSE);

	if(OpenDevice((CONST STRPTR)"input.device", 0, (struct IORequest *)InputRequest, 0))
		return(FALSE);

#ifdef __MORPHOS__
	InputBase=(struct Library *)&InputRequest->io_Device->dd_Library;
#else
	InputBase=(struct Device *)&InputRequest->io_Device->dd_Library;
#endif

#ifdef __amigaos4__
	if(!(printerPort= (struct MsgPort *)AllocSysObjectTags(ASOT_PORT, TAG_DONE)))
#else
	if(!(printerPort= (struct MsgPort *)CreatePort(NULL, 0)))
#endif
		return(FALSE);

#ifdef __amigaos4__
	if((printerRequest=(struct IODRPReq *)AllocSysObjectTags(ASOT_IOREQUEST,
		ASOIOR_ReplyPort, printerPort,
		ASOIOR_Size, sizeof(struct IODRPReq),
		TAG_DONE)) == NULL)
#else
	if((printerRequest=(struct IODRPReq *)CreateExtIO(printerPort, sizeof(struct IODRPReq))) == NULL)
#endif
		return(FALSE);

	init_cursor();
	Init_UserColors();
	Init_Gradients();

	if(ReadSettings(FALSE, &appres) == FALSE)
	{
		/* Unable to read setting file, save the default values to setting file */
		SaveSettings(FALSE, &appres);
	}

	/* For the moment FIXED font can't be selected, it's fixed to TOPAZ8 */
	GUIfont_Fixed = OpenFont(&GUIFont_Fixed_Attr);
	
	PubScreenName	= appres.PubScreenName;

	/* Copy NewMenu Structure to global memory chunk for usage in Application */
#ifdef __amigaos4__
    AmiFIGNM = AllocVecTags(sizeof(AmiFIGNM_def), TAG_DONE);
#else
    AmiFIGNM = AllocVec(sizeof(AmiFIGNM_def), MEMF_ANY);
#endif
    CopyMem(AmiFIGNM_def, AmiFIGNM, sizeof(AmiFIGNM_def));
	/* Create Menu structure for intuition (canvas) window */
	ZFig_Menus = CreateMenus(AmiFIGNM, GTMN_FrontPen, 0L, TAG_DONE);

	if((clip_region=NewRegion()) == NULL)
		return(FALSE);

	if((ref_clip_region=NewRegion()) == NULL)
		return(FALSE);

	InitArea(&areainfo, polygondata, MAXPOINTNUM);
	
	if(openWin())
		return(FALSE);

	calc_max_rultextwidth(TRUE);
	redisplay_canvas();

	initRexxIFace();

	canvas_locmove_proc		= (void (*)(int, int, int))null_proc;
	canvas_leftbut_proc		= (void (*)(int, int, int))null_proc;
	canvas_middlebut_proc	= (void (*)(int, int, int))null_proc;
	canvas_rightbut_proc	= (void (*)(int, int, int))null_proc;
    canvas_ref_proc         = NULL;
	return(TRUE);
}

// Index des Suffix feststellen
int get_suffix_pos(char *name)
{
	int i;

	for(i=strlen(name)-1; name[i] != '.' && i >= 0; i--);

	return(i);
}

#define SUFFIXLEN 8
#define FR_PATTLEN 21

BOOL DoFileRequest(UBYTE *Title, UBYTE *Buffer, int mode)
{
	char suffix[SUFFIXLEN+1]="";
	int len;
	BOOL success = FALSE;
	struct FileRequester		*file_request = NULL;
	static char ExportName[FCHARS+1] 	= "";
	static char PrefsDirectory[DSIZE+1]	= "";
	static char ExportDir[DSIZE+1]		= "";
	
	file_request=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
														(ASLFR_Screen), 			Scr,
														(TAG_DONE));

	if (file_request != NULL)
	{
		beginWait();

		switch(mode & FR_MODEMASK)
		{
			case FR_FIGDATA:
				if(AslRequestTags(	file_request,
									(ASLFR_TitleText), 		Title,
									(ASLFR_InitialDrawer),	DirectoryName,
									(ASLFR_InitialFile),	FileName,
									(ASLFR_DoPatterns), 	TRUE,
									(ASLFR_InitialPattern), "#?.(fig|fig.bak)",
									(TAG_DONE)) != 0)
				{
					if((mode & FR_ACTMASK) == FR_DOSAVE)
					{
						/* if suffix isn't .fig, append suffix */
						if(strstr((const char *)file_request->fr_File, ".fig") == NULL)
						{
							strncpy(suffix, ".fig", SUFFIXLEN);
						}
					}
					success = TRUE;
				}
				break;
				
			case FR_FIGPREFS:
				if(AslRequestTags(	file_request,
									(ASLFR_TitleText), 		Title,
									(ASLFR_InitialDrawer),	PrefsDirectory,
									(ASLFR_InitialFile), 	PrefsName,
									(ASLFR_DoPatterns), 	FALSE,
									(ASLFR_InitialPattern), "",
									(TAG_DONE)) != 0)
				{
					strncpy(PrefsDirectory, (const char *)file_request->fr_Drawer, DSIZE);
					strncpy(PrefsName,      (const char *)file_request->fr_File, FCHARS);
					success = TRUE;
				}
				break;
				
			case FR_EXPORT:
				// make the export name from the file name, e.g.: new.fig => new.ps
				if(strcmp(ExportName, "") == 0)
				{
					if((len=get_suffix_pos(FileName)) == -1)
						strncpy(ExportName, FileName, FCHARS);
					else
						strncpy(ExportName, FileName, len);
				}
				else
				{
					if((len=get_suffix_pos(ExportName)) == -1)
						strncpy(ExportName, ExportName, FCHARS);
					else
						strncpy(ExportName, ExportName, len);
				}
				ExportName[len]='\0';

				// append the correct suffix
				strcat(ExportName, ".png");

				if(AslRequestTags(	file_request,
									(ASLFR_TitleText), 		Title,
									(ASLFR_InitialDrawer),	ExportDir,
									(ASLFR_InitialFile), 	ExportName,
									(ASLFR_DoPatterns), 	FALSE,
									(ASLFR_InitialPattern), "",
									(TAG_DONE)) != 0)
				{
					strncpy(ExportDir,  (const char *)file_request->fr_Drawer, DSIZE);
					strncpy(ExportName, (const char *)file_request->fr_File, FCHARS);
					success = TRUE;
				}
				break;
		}

		if (success == TRUE)
		{
			strmfp((char *)Buffer, (const char *)file_request->fr_Drawer, (const char *)file_request->fr_File);
			strcat((char *)Buffer, suffix);
		}

		FreeAslRequest(file_request);
		endWait();
	}

	return(success);
}
