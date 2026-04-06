/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_print.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/*----------------------------------------------------------------------------*/
/*     Includes & Defines                                                     */
/*----------------------------------------------------------------------------*/
#include "fig.h"

#include <libraries/mui.h>
#include <proto/muimaster.h>

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "a_coords.h"
#include "a_print.h"
#include "z_fig.h"
#include "z_print.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFP3(void, DoPrint,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoPrintWholePage,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoPrint(struct Hook *h, APTR Object, ULONG *Arg);
void DoPrintWholePage(struct Hook *h, APTR Object, ULONG *Arg);
#endif    

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    BUT_PRI_EXPORT, BUT_PRI_CLOSE;
APTR	BUT_PRI_WHOLEPAGE, STR_PRI_LEFT, STR_PRI_WDTH, STR_PRI_TOP, STR_PRI_HGHT;
APTR	STR_PRI_BUFFER, NUM_PRI_DENSITY;
        
/*----------------------------------------------------------------------------*/
/*     MakePrintWindow - creates the Print window                             */
/*----------------------------------------------------------------------------*/

APTR MakePrintWindow(void)
{
	float pagex = 0.0, pagey = 0.0;
	char wdth_txt[10], hght_txt[10];
	
	get_pagesizes(&pagex, &pagey);
	sprintf(wdth_txt, "%8.2f", pagex);
	sprintf(hght_txt, "%8.2f", pagey);
	
    return(WindowObject,
            MUIA_Window_Title, "AmiFig : Print Figure",
            MUIA_Window_ID, MAKEID('Z','F','P','R'),
			MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",

            WindowContents,	VGroup,
				Child, VGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_PrintSel),
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Left), End,
						Child, STR_PRI_LEFT = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Contents, "0.0",
                        End, 
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Width), End,
						Child, STR_PRI_WDTH = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Contents, wdth_txt,
                        End,
						Child, TextObject, MUIA_Text_Contents, "cm", End,
					End,
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Top), End,
						Child, STR_PRI_TOP = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Contents, "0.0",
                        End,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Height), End,
						Child, STR_PRI_HGHT = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Contents, hght_txt,
                        End,
						Child, TextObject, MUIA_Text_Contents, "cm", End,
					End,
					Child, HGroup,
						Child, RectangleObject, End,
						Child, BUT_PRI_WHOLEPAGE = MakeButton((UBYTE *)_(msg_ExportWhole), '\0', (UBYTE *)""),
					End,
				End,
				Child, VGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_PrinterPrefs),
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (char *)_(msg_Density), End,
						Child, NUM_PRI_DENSITY = SliderObject,
                            MUIA_Group_Horiz  , TRUE,
                            MUIA_Numeric_Min  , 1,
                            MUIA_Numeric_Max  , 7,
                            MUIA_Numeric_Value, 2,
                        End,
                    End,
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_PrintBuffer), End,
						Child, STR_PRI_BUFFER = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 10,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, appres.printbuf,
                        End,
					End,
				End,
				Child, HGroup,
					Child, BUT_PRI_EXPORT = MakeButton((UBYTE *)_(msg_PRINT), 'P', (UBYTE *)""),
					Child, RectangleObject, End,
					Child, BUT_PRI_CLOSE  = MakeButton((UBYTE *)_(msg_Cancel), 'C', (UBYTE *)""),
				End,
            End,
        End);
}

void z_print_methods(void)
{
    static struct Hook DoPrintHook;
    DoPrintHook.h_Entry = (HOOKFUNC)DoPrint;
    static struct Hook DoPrintWholePageHook;
    DoPrintWholePageHook.h_Entry = (HOOKFUNC)DoPrintWholePage;


    /* Close window */
    DoMethod(WinPrint, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinPrint, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	
	/* Close Button */
    DoMethod(BUT_PRI_CLOSE, MUIM_Notify, MUIA_Selected, FALSE, WinPrint, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	/* Export Button */
    DoMethod(BUT_PRI_EXPORT, MUIM_Notify, MUIA_Selected, FALSE, WinPrint, 3, MUIM_CallHook, (IPTR)&DoPrintHook, 0L);
	
	/* Whole page button */
    DoMethod(BUT_PRI_WHOLEPAGE, MUIM_Notify, MUIA_Selected, FALSE, WinPrint, 3, MUIM_CallHook, (IPTR)&DoPrintWholePageHook, 0L);
}

/*----------------------------------------------------------------------------*/
/*     DoPrint                                                                */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoPrint,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoPrint(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	CONST_STRPTR tmp = NULL;
	float tmpfl;
	LONG left = 0, top = 0, width = 0, height = 0;
	LONG Density = 0;

	/* Gather data from interface display */
	{ IPTR _t; get(STR_PRI_LEFT, MUIA_String_Contents, &_t); tmp = (STRPTR)_t; }
	sscanf((const char *)tmp, "%f", &tmpfl);
	left   = CM_TO_PIX(tmpfl);
	{ IPTR _t; get(STR_PRI_WDTH, MUIA_String_Contents, &_t); tmp = (STRPTR)_t; }
	sscanf((const char *)tmp, "%f", &tmpfl);
	width   = CM_TO_PIX(tmpfl);
	{ IPTR _t; get(STR_PRI_TOP,  MUIA_String_Contents, &_t); tmp = (STRPTR)_t; }
	sscanf((const char *)tmp, "%f", &tmpfl);
	top   = CM_TO_PIX(tmpfl);
	{ IPTR _t; get(STR_PRI_HGHT, MUIA_String_Contents, &_t); tmp = (STRPTR)_t; }
	sscanf((const char *)tmp, "%f", &tmpfl);
	height   = CM_TO_PIX(tmpfl);
	{ IPTR _t; get(STR_PRI_BUFFER, MUIA_String_Contents, &_t); tmp = (STRPTR)_t; }
	sscanf((const char *)tmp, "%d", &appres.printbuf);
	get(NUM_PRI_DENSITY,  MUIA_Numeric_Value,   &Density);

	/* print the figure with collected data and close the window */
	print_bitmap(left,top,width,height,Density);
    set(WinPrint, MUIA_Window_Open, FALSE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoPrintWholePage                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoPrintWholePage,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoPrintWholePage(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	float pagex = 0.0, pagey = 0.0;
	char wdth_txt[10], hght_txt[10];
	
	get_pagesizes(&pagex, &pagey);
	sprintf(wdth_txt, "%8.2f", pagex);
	sprintf(hght_txt, "%8.2f", pagey);
	
	DoMethod(STR_PRI_LEFT, MUIM_NoNotifySet, MUIA_String_Contents, "0.0");
	DoMethod(STR_PRI_WDTH, MUIM_NoNotifySet, MUIA_String_Contents, wdth_txt);
	DoMethod(STR_PRI_TOP,  MUIM_NoNotifySet, MUIA_String_Contents, "0.0");
	DoMethod(STR_PRI_HGHT, MUIM_NoNotifySet, MUIA_String_Contents, hght_txt);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

void z_updateprint(int left, int top, int width, int height)
{
	char wdth_txt[10], hght_txt[10], top_txt[10], left_txt[10];

	sprintf(wdth_txt, "%8.2f", PIX_TO_CM(width));
	sprintf(hght_txt, "%8.2f", PIX_TO_CM(height));
	sprintf(top_txt , "%8.2f", PIX_TO_CM(top));
	sprintf(left_txt, "%8.2f", PIX_TO_CM(left));

	DoMethod(STR_PRI_LEFT, MUIM_NoNotifySet, MUIA_String_Contents, left_txt);
	DoMethod(STR_PRI_WDTH, MUIM_NoNotifySet, MUIA_String_Contents, wdth_txt);
	DoMethod(STR_PRI_TOP,  MUIM_NoNotifySet, MUIA_String_Contents, top_txt);
	DoMethod(STR_PRI_HGHT, MUIM_NoNotifySet, MUIA_String_Contents, hght_txt);
}
