/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_export.c $
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
#include "a_savebitmap.h"
#include "z_fig.h"
#include "z_export.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFP3(void, DoExport,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoWholePage,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoAsSelection,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoChangeSize,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));

#else
void DoExport(struct Hook *h, APTR Object, ULONG *Arg);
void DoWholePage(struct Hook *h, APTR Object, ULONG *Arg);
void DoAsSelection(struct Hook *h, APTR Object, ULONG *Arg);
void DoChangeSize(struct Hook *h, APTR Object, ULONG *Arg);
#endif
/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    BUT_EXP_EXPORT, BUT_EXP_CLOSE;
APTR	BUT_EXP_WHOLEPAGE, STR_EXP_LEFT, STR_EXP_WDTH, STR_EXP_TOP, STR_EXP_HGHT;
APTR	STR_EXP_OUTWDTH, STR_EXP_OUTHGHT, BUT_EXP_ASSEL;
APTR	CYC_EXP_AA;
        
/*----------------------------------------------------------------------------*/
/*     MakeExportbmWindow - creates the Export Bitmap window                  */
/*----------------------------------------------------------------------------*/

APTR MakeExportWindow(void)
{
	float pagex = 0.0, pagey = 0.0;
	static char *CYC_EXP_AA_strings[] = {NULL, "2X2", "3X3", "4X4", NULL};

	CYC_EXP_AA_strings[0] = (char *)_(msg_None);

	
	get_pagesizes(&pagex, &pagey);
	
    return(WindowObject,
            MUIA_Window_Title, "AmiFig : Export Bitmap",
            MUIA_Window_ID, MAKEID('Z','F','E','X'),
			MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",

            WindowContents,	VGroup,
				Child, VGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_ExportSel),
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Left), End,
						Child, STR_EXP_LEFT = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, 0,
                        End, 
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Width), End,
						Child, STR_EXP_WDTH = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, CM_TO_PIX(pagex),
                        End, 
					End,
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Top), End,
						Child, STR_EXP_TOP = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, 0,
                        End,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Height), End,
						Child, STR_EXP_HGHT = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, CM_TO_PIX(pagey),
                        End,
					End,
					Child, HGroup,
						Child, RectangleObject, End,
						Child, BUT_EXP_WHOLEPAGE = MakeButton((UBYTE *)_(msg_ExportWhole), '\0', (UBYTE *)""),
					End,
				End,
				Child, VGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_ExportOutput),
					Child, HGroup,
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Width), End,
						Child, STR_EXP_OUTWDTH = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, CM_TO_PIX(pagex),
                        End, 
						Child, TextObject, MUIA_Text_Contents, (UBYTE *)_(msg_Height), End,
						Child, STR_EXP_OUTHGHT = StringObject,
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789",
                            MUIA_String_Integer, CM_TO_PIX(pagey),
                        End, 
					End,		
					Child, HGroup,
						Child, RectangleObject, End,
						Child, BUT_EXP_ASSEL = MakeButton((UBYTE *)_(msg_ExportAsSel), '\0', (UBYTE *)""),
					End,
				End,
				Child, HGroup,
					MUIA_Frame, MUIV_Frame_Group,
					MUIA_FrameTitle, (UBYTE *)_(msg_Antialising),
					Child, CYC_EXP_AA = CycleObject,
						MUIA_Cycle_Entries, CYC_EXP_AA_strings,
						MUIA_Cycle_Active, 0L,
					End,
				End,
				Child, HGroup,
					Child, BUT_EXP_EXPORT = MakeButton((UBYTE *)_(msg_Export), 'E', (UBYTE *)""),
					Child, RectangleObject, End,
					Child, BUT_EXP_CLOSE  = MakeButton((UBYTE *)_(msg_Cancel), 'C', (UBYTE *)""),
				End,
            End,
        End);
}

void z_export_methods(void)
{
    static struct Hook DoExportHook;
    DoExportHook.h_Entry = (HOOKFUNC)DoExport;
    static struct Hook DoWholePageHook;
    DoWholePageHook.h_Entry = (HOOKFUNC)DoWholePage;
    static struct Hook DoAsSelectionHook;
    DoAsSelectionHook.h_Entry = (HOOKFUNC)DoAsSelection;
    static struct Hook DoChangeSizeHook;
    DoChangeSizeHook.h_Entry = (HOOKFUNC)DoChangeSize;

    /* Close window */
    DoMethod(WinExport, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinExport, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	
	/* Close Button */
    DoMethod(BUT_EXP_CLOSE, MUIM_Notify, MUIA_Selected, FALSE, WinExport, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	/* Export Button */
    DoMethod(BUT_EXP_EXPORT, MUIM_Notify, MUIA_Selected, FALSE, WinExport, 3, MUIM_CallHook, (IPTR)&DoExportHook, 0L);
	
	/* Whole page button */
    DoMethod(BUT_EXP_WHOLEPAGE, MUIM_Notify, MUIA_Selected, FALSE, WinExport, 3, MUIM_CallHook, (IPTR)&DoWholePageHook, 0L);

	/* As Selection button */
    DoMethod(BUT_EXP_ASSEL, MUIM_Notify, MUIA_Selected, FALSE, WinExport, 3, MUIM_CallHook, (IPTR)&DoAsSelectionHook, 0L);

	/* Size change */
    DoMethod(STR_EXP_WDTH,    MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeSizeHook, MUIV_TriggerValue);
    DoMethod(STR_EXP_HGHT,    MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeSizeHook, MUIV_TriggerValue);
    DoMethod(STR_EXP_OUTWDTH, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeSizeHook, MUIV_TriggerValue);
    DoMethod(STR_EXP_OUTHGHT, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeSizeHook, MUIV_TriggerValue);
}

/*----------------------------------------------------------------------------*/
/*     DoExport                                                               */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoExport,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoExport(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	LONG left = 0, top = 0, width = 0, height = 0;
	LONG expwidth = 0, expheight = 0;
	ULONG AA = 0;

	get(STR_EXP_LEFT, MUIA_String_Integer, &left);
	get(STR_EXP_WDTH, MUIA_String_Integer, &width);
	get(STR_EXP_TOP, MUIA_String_Integer, &top);
	get(STR_EXP_HGHT, MUIA_String_Integer, &height);
	get(STR_EXP_OUTWDTH, MUIA_String_Integer, &expwidth);
	get(STR_EXP_OUTHGHT, MUIA_String_Integer, &expheight);
	get(CYC_EXP_AA, MUIA_Cycle_Active, &AA);

	if(DoFileRequest((UBYTE *)"Export as...", (UBYTE *)TempName, FR_EXPORT) != FALSE)
	{
		write_bitmap((char *)TempName, left, top, width, height, expwidth, expheight, AA);
	}
	
    set(WinExport, MUIA_Window_Open, FALSE);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoWholePage                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoWholePage,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoWholePage(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
	
	float pagex = 0.0, pagey = 0.0;
	LONG  width = 0,height = 0,outwidth = 0, outheight = 0;
	
	get_pagesizes(&pagex, &pagey);
	
	get(STR_EXP_WDTH, MUIA_String_Integer, &width);
	get(STR_EXP_HGHT, MUIA_String_Integer, &height);
	get(STR_EXP_OUTWDTH, MUIA_String_Integer, &outwidth);
	get(STR_EXP_OUTHGHT, MUIA_String_Integer, &outheight);
	
	outwidth  = round(outwidth * CM_TO_PIX(pagex) / width);
	outheight = round(outheight * CM_TO_PIX(pagey) / height);
	
	DoMethod(STR_EXP_OUTWDTH,MUIM_NoNotifySet, MUIA_String_Integer, outwidth);
	DoMethod(STR_EXP_OUTHGHT,MUIM_NoNotifySet, MUIA_String_Integer, outheight);

	DoMethod(STR_EXP_LEFT, MUIM_NoNotifySet, MUIA_String_Integer, 0);
	DoMethod(STR_EXP_WDTH, MUIM_NoNotifySet, MUIA_String_Integer, CM_TO_PIX(pagex));
	DoMethod(STR_EXP_TOP,  MUIM_NoNotifySet, MUIA_String_Integer, 0);
	DoMethod(STR_EXP_HGHT, MUIM_NoNotifySet, MUIA_String_Integer, CM_TO_PIX(pagey));
	
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoAsSelection                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoAsSelection,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoAsSelection(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	LONG width = 0,height = 0;
    
	get(STR_EXP_WDTH, MUIA_String_Integer, &width);
	get(STR_EXP_HGHT, MUIA_String_Integer, &height);
	DoMethod(STR_EXP_OUTWDTH, MUIM_NoNotifySet, MUIA_String_Integer, width);
	DoMethod(STR_EXP_OUTHGHT, MUIM_NoNotifySet, MUIA_String_Integer, height);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoChangeSize                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoChangeSize,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoChangeSize(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	LONG 	width = 0, height = 0, outwidth = 0, outheight = 0;
	
	get(STR_EXP_WDTH, MUIA_String_Integer, &width);
	get(STR_EXP_HGHT, MUIA_String_Integer, &height);
	get(STR_EXP_OUTWDTH, MUIA_String_Integer, &outwidth);
	get(STR_EXP_OUTHGHT, MUIA_String_Integer, &outheight);
	
	if (Object == STR_EXP_WDTH)
	{
		outwidth = round(width * outheight / height);
		DoMethod(STR_EXP_OUTWDTH, MUIM_NoNotifySet, MUIA_String_Integer, outwidth);
	}
	else if (Object == STR_EXP_HGHT)
	{
		outheight = round(height * outwidth / width);
		DoMethod(STR_EXP_OUTHGHT, MUIM_NoNotifySet, MUIA_String_Integer, outheight);
	}
	else if (Object == STR_EXP_OUTWDTH)
	{
		outheight = round(outwidth * height / width);
		DoMethod(STR_EXP_OUTHGHT, MUIM_NoNotifySet, MUIA_String_Integer, outheight);
	}
	else if (Object == STR_EXP_OUTHGHT)
	{
		outheight = round(outheight * width / height);
		DoMethod(STR_EXP_OUTHGHT, MUIM_NoNotifySet, MUIA_String_Integer, outheight);
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

void z_updateexport(int left, int top, int width, int height)
{
	LONG 	oldwidth = 0, oldheight = 0, outwidth = 0, outheight = 0;
		
	get(STR_EXP_WDTH, MUIA_String_Integer, &oldwidth);
	get(STR_EXP_HGHT, MUIA_String_Integer, &oldheight);
	get(STR_EXP_OUTWDTH, MUIA_String_Integer, &outwidth);
	get(STR_EXP_OUTHGHT, MUIA_String_Integer, &outheight);

	outwidth  = round(outwidth * width / oldwidth);
	outheight = round(outheight * height / oldheight);
	
	DoMethod(STR_EXP_OUTWDTH,MUIM_NoNotifySet, MUIA_String_Integer, outwidth);
	DoMethod(STR_EXP_OUTHGHT,MUIM_NoNotifySet, MUIA_String_Integer, outheight);
	
	DoMethod(STR_EXP_LEFT, MUIM_NoNotifySet, MUIA_String_Integer, left);
	DoMethod(STR_EXP_WDTH, MUIM_NoNotifySet, MUIA_String_Integer, width);
	DoMethod(STR_EXP_TOP,  MUIM_NoNotifySet, MUIA_String_Integer, top);
	DoMethod(STR_EXP_HGHT, MUIM_NoNotifySet, MUIA_String_Integer, height);
}
