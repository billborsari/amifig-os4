/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_pict.c $
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
#include <libraries/asl.h>

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "f_picobj.h"
#include "e_movept.h"
#include "u_list.h"
#include "u_redraw.h"
#include "u_undo.h"
#include "w_msgpanel.h"
#include "z_fig.h"
#include "mcc_arrowslider.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFP3(void, DoWinOpen,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoWinClose,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoOpenPic,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoResetSize,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoChangeWidth,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, DoChangeHeight,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
void DoWinOpen(struct Hook *h, APTR Object, ULONG *Arg);
void DoWinClose(struct Hook *h, APTR Object, ULONG *Arg);
void DoOpenPic(struct Hook *h, APTR Object, ULONG *Arg);
void DoResetSize(struct Hook *h, APTR Object, ULONG *Arg);
void DoChangeWidth(struct Hook *h, APTR Object, ULONG *Arg);
void DoChangeHeight(struct Hook *h, APTR Object, ULONG *Arg);
#endif

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    STR_PICT_FILENAME, BUT_PICT_CLOSE, GRP_PICT_EDIT;
APTR	SLI_PICT_HEIGHT, SLI_PICT_WIDTH, CHK_PICT_PROPORTIONAL, BUT_PICT_RESETSIZE;
APTR    STR_PICT_HEIGHT, STR_PICT_WIDTH;

BOOL	PictureEditWinOn = FALSE;
        
/*----------------------------------------------------------------------------*/
/*     MakePictWindow - creates the picture window                            */
/*----------------------------------------------------------------------------*/

APTR MakePictWindow(void)
{
    return(WindowObject,
            MUIA_Window_Title, "AmiFig : Picture Object",
            MUIA_Window_ID, MAKEID('Z','F','P','I'),
			MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",

            WindowContents,	VGroup,
				Child, STR_PICT_FILENAME = PopaslObject,
					MUIA_Frame				, MUIV_Frame_Group,
					MUIA_FrameTitle			, (UBYTE *)_(msg_PictureFile),
					MUIA_Popstring_String	,   StringObject,
                                                    StringFrame,
                                                    MUIA_ControlChar , 'f',
                                                    MUIA_String_MaxLen , 256,
                                                    MUIA_String_Contents, 0,
                                                End,
					MUIA_Popstring_Button	, PopButton(MUII_PopFile),
					MUIA_Popasl_Type 		, ASL_FileRequest,
					ASLFR_TitleText			, (char *)_(msg_PictureSelection),
					ASLFR_InitialDrawer		, "",
					ASLFR_InitialFile		, "",
				End,
				Child, GRP_PICT_EDIT = VGroup,
                    MUIA_Frame				, MUIV_Frame_Group,
                    MUIA_FrameTitle			, (UBYTE *)_(msg_PictureSize),
                    Child, HGroup,
                        Child, CHK_PICT_PROPORTIONAL = MakeCheckmark((UBYTE *)_(msg_Proportional), 0L),
                        Child, RectangleObject, End,
                        Child, BUT_PICT_RESETSIZE = MakeButton((UBYTE *)_(msg_Reset), '\0', (UBYTE *)_(msg_SetOriginalPicSize)),
                    End,
                    Child, GroupObject,
                        MUIA_Group_Columns, 4,
                        Child, TextObject,  MUIA_Text_Contents, (char *)_(msg_WidthPercent), End,
                        Child, SLI_PICT_WIDTH = NewObject(	mcc_asl->mcc_Class, NULL,
                            MUIA_Group_Horiz  , TRUE,
                            MUIA_Weight       , 300,
                            MUIA_Numeric_Min  , 1L,
                            MUIA_Numeric_Max  , 1000L,
                            MUIA_Numeric_Value, 100L,
                            TAG_DONE),
                        Child, STR_PICT_WIDTH = StringObject, 
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Contents, 0,
                        End,
                        Child, TextObject,
                            MUIA_Weight, 10,
                            MUIA_Text_Contents, "cm",
                        End, 
                        Child, TextObject, MUIA_Text_Contents, (char *)_(msg_HeightPercent), End,
                        Child, SLI_PICT_HEIGHT = NewObject(	mcc_asl->mcc_Class, NULL,
                            MUIA_Group_Horiz  , TRUE,
                            MUIA_Weight       , 300,
                            MUIA_Numeric_Min  , 1L,
                            MUIA_Numeric_Max  , 1000L,
                            MUIA_Numeric_Value, 100L,
                            TAG_DONE),
                        Child, STR_PICT_HEIGHT = StringObject, 
                            StringFrame,
                            MUIA_String_MaxLen , 8,
                            MUIA_String_Accept, "0123456789.",
                            MUIA_String_Format, MUIV_String_Format_Right,
                            MUIA_String_Contents, 0,
                        End,
                        Child, TextObject,
                            MUIA_Weight, 10,
                            MUIA_Text_Contents, "cm",
                        End, 
                    End,
                End,
				Child, HGroup,
					Child, RectangleObject, End,
					Child, BUT_PICT_CLOSE = MakeButton((UBYTE *)_(msg_Close), '\0', (UBYTE *)""),
				End,
            End,
        End);
}

void z_pict_methods(void)
{
    static struct Hook DoWinOpenHook;
    DoWinOpenHook.h_Entry = (HOOKFUNC)DoWinOpen;
    static struct Hook DoWinCloseHook;
    DoWinCloseHook.h_Entry = (HOOKFUNC)DoWinClose;
    static struct Hook DoOpenPicHook;
    DoOpenPicHook.h_Entry = (HOOKFUNC)DoOpenPic;
    static struct Hook DoResetSizeHook;
    DoResetSizeHook.h_Entry = (HOOKFUNC)DoResetSize;
    static struct Hook DoChangeWidthHook;
    DoChangeWidthHook.h_Entry = (HOOKFUNC)DoChangeWidth;
    static struct Hook DoChangeHeightHook;
    DoChangeHeightHook.h_Entry = (HOOKFUNC)DoChangeHeight;

    /* Close window */
    DoMethod(WinPict, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinPict, 3, MUIM_Set, MUIA_Window_Open, FALSE);

	/* Close Button */
    DoMethod(BUT_PICT_CLOSE, MUIM_Notify, MUIA_Selected, FALSE, WinPict, 3, MUIM_Set, MUIA_Window_Open, FALSE);

    DoMethod(WinPict, MUIM_Notify, MUIA_Window_Open, TRUE, App, 3, MUIM_CallHook, (IPTR)&DoWinOpenHook, 0);
    DoMethod(WinPict, MUIM_Notify, MUIA_Window_Open, FALSE, App, 3, MUIM_CallHook, (IPTR)&DoWinCloseHook, 0);
    DoMethod(STR_PICT_FILENAME, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoOpenPicHook, 0);
    DoMethod(STR_PICT_FILENAME, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, App, 3, MUIM_CallHook, (IPTR)&DoOpenPicHook, 0);
    DoMethod(BUT_PICT_RESETSIZE, MUIM_Notify, MUIA_Selected, FALSE, App, 3, MUIM_CallHook, (IPTR)&DoResetSizeHook, 0);
    DoMethod(SLI_PICT_WIDTH, MUIM_Notify, MUIA_Numeric_Value,   MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeWidthHook, MUIV_TriggerValue);
    DoMethod(STR_PICT_WIDTH, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeWidthHook, MUIV_TriggerValue);
    DoMethod(SLI_PICT_HEIGHT, MUIM_Notify, MUIA_Numeric_Value,   MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeHeightHook, MUIV_TriggerValue);
    DoMethod(STR_PICT_HEIGHT, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, MUIV_Notify_Self, 3, MUIM_CallHook, (IPTR)&DoChangeHeightHook, MUIV_TriggerValue);
   
}

/*----------------------------------------------------------------------------*/
/*     DoWinOpen                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoWinOpen,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoWinOpen(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    int  width, height, perw, perh;
    char tmp[16];
    
    DebugPrintF("[PICT] DoWinOpen: enter\n");
    
	PictureEditWinOn = TRUE;
	if (cur_l)
	{
	    DebugPrintF("[PICT] DoWinOpen: cur_l=%p pic=%p\n", (void *)cur_l, (void *)cur_l->pic);
	    if (cur_l->pic && strlen(cur_l->pic->file) != 0)
        {
            DebugPrintF("[PICT] DoWinOpen: file='%s'\n", cur_l->pic->file);
            refresh_line_area(cur_l, TRUE);
            DoMethod(STR_PICT_FILENAME, MUIM_NoNotifySet, MUIA_String_Contents, cur_l->pic->file);
            DebugPrintF("[PICT] DoWinOpen: get_box_dimension\n");
            get_box_dimension(cur_l, &width, &height);
            DebugPrintF("[PICT] DoWinOpen: w=%ld h=%ld orig_w=%ld orig_h=%ld\n",
                   width, height, cur_l->pic->orig_width, cur_l->pic->orig_height);
            if (cur_l->pic->orig_width > 0 && cur_l->pic->orig_height > 0)
            {
                perw = 100 * width / cur_l->pic->orig_width;
                perh = 100 * height / cur_l->pic->orig_height;
                DoMethod(SLI_PICT_WIDTH, MUIM_NoNotifySet , MUIA_Numeric_Value,  perw);
                sprintf(tmp, "%8.3f", (double)width / PIX_PER_CM);
                DoMethod(STR_PICT_WIDTH, MUIM_NoNotifySet , MUIA_String_Contents, tmp);
                DoMethod(SLI_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_Numeric_Value,  perh);
                sprintf(tmp, "%8.3f", (double)height / PIX_PER_CM);
                DoMethod(STR_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
                DoMethod(CHK_PICT_PROPORTIONAL, MUIM_NoNotifySet, MUIA_Selected, (perw == perh));
            }
            refresh_line_area(cur_l, FALSE);
            DebugPrintF("[PICT] DoWinOpen: done updating gadgets\n");
        }
        else
        {
            DebugPrintF("[PICT] DoWinOpen: no file set (new picture)\n");
        }
	}
	else
	{
	    DebugPrintF("[PICT] DoWinOpen: WARNING cur_l is NULL!\n");
	}
	DebugPrintF("[PICT] DoWinOpen: exit\n");
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoWinClose                                                             */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoWinClose,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoWinClose(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif
    DebugPrintF("[PICT] DoWinClose: enter cur_l=%p\n", (void *)cur_l);
	
    if(cur_l)
	{
	    if(cur_l->pic && strlen(cur_l->pic->file) == 0)
		{
		    DebugPrintF("[PICT] DoWinClose: empty file, deleting line\n");
			delete_line(cur_l);
			clean_up();
		}
	}
	
	PictureEditWinOn = FALSE;
    DebugPrintF("[PICT] DoWinClose: exit\n");
    
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoOpenPic                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoOpenPic,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoOpenPic(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    STRPTR newpic=(STRPTR)"";
    int  width, height, perw, perh;
    char tmp[16];
    
    DebugPrintF("[PICT] DoOpenPic: enter\n");
    
    if (!cur_l || !cur_l->pic)
    {
        DebugPrintF("[PICT] DoOpenPic: ERROR cur_l=%p or pic is NULL, aborting\n", (void *)cur_l);
        goto done;
    }
    
    { IPTR _t; get(STR_PICT_FILENAME, MUIA_String_Contents, &_t); newpic = (STRPTR)_t; }
    DebugPrintF("[PICT] DoOpenPic: newpic='%s' cur_file='%s'\n",
           newpic ? (char *)newpic : "(null)", cur_l->pic->file);
    
    redisplay_canvas(); // Clear old area
    DebugPrintF("[PICT] DoOpenPic: after redisplay_canvas\n");
    
	if(strcmp((const char *)newpic,cur_l->pic->file)!=0)
	{
        strcpy(cur_l->pic->file, (const char *)newpic);
        DebugPrintF("[PICT] DoOpenPic: calling read_picobj\n");
        if (read_picobj(cur_l, cur_l->pic, cur_l->pen_color) && 
            cur_l->pic->orig_width > 0 && cur_l->pic->orig_height > 0)
        {
            DebugPrintF("[PICT] DoOpenPic: read OK, orig_w=%ld orig_h=%ld\n",
                   cur_l->pic->orig_width, cur_l->pic->orig_height);
            redisplay_canvas(); // Redraw with image
            get_box_dimension(cur_l, &width, &height);
            DebugPrintF("[PICT] DoOpenPic: box w=%ld h=%ld\n", width, height);
            perw = 100 * width / cur_l->pic->orig_width;
            perh = 100 * height / cur_l->pic->orig_height;
            DoMethod(SLI_PICT_WIDTH, MUIM_NoNotifySet , MUIA_Numeric_Value,  perw);
            sprintf(tmp, "%8.3f", (double)width / PIX_PER_CM);
            DoMethod(STR_PICT_WIDTH, MUIM_NoNotifySet , MUIA_String_Contents, tmp);
            DoMethod(SLI_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_Numeric_Value,  perh);
            sprintf(tmp, "%8.3f", (double)height / PIX_PER_CM);
            DoMethod(STR_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
            DoMethod(CHK_PICT_PROPORTIONAL, MUIM_NoNotifySet, MUIA_Selected, (perw == perh));
            DebugPrintF("[PICT] DoOpenPic: gadgets updated\n");
         }
        else
        {
            DebugPrintF("[PICT] DoOpenPic: read_picobj failed or zero size\n");
            put_msg("Not a valid picture file");
            cur_l->pic->file[0] = 0;
            set(STR_PICT_FILENAME, MUIA_String_Contents, 0);
        }
	}
	else
	{
	    DebugPrintF("[PICT] DoOpenPic: same file, skipped\n");
	}
	
done:
    DebugPrintF("[PICT] DoOpenPic: exit\n");
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoResetSize                                                            */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoResetSize,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoResetSize(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    

    char tmp[16];
    
    DebugPrintF("[PICT] DoResetSize: enter cur_l=%p\n", (void *)cur_l);
    
    if (!cur_l || !cur_l->pic)
    {
        DebugPrintF("[PICT] DoResetSize: ERROR cur_l or pic is NULL\n");
        goto done;
    }
    
    DebugPrintF("[PICT] DoResetSize: orig_w=%ld orig_h=%ld\n",
           cur_l->pic->orig_width, cur_l->pic->orig_height);
	
    refresh_line_area(cur_l, TRUE);
    stretch_box(cur_l, cur_l->pic->orig_width, cur_l->pic->orig_height);
    
    DoMethod(SLI_PICT_WIDTH, MUIM_NoNotifySet , MUIA_Numeric_Value,  100);
    sprintf(tmp, "%8.3f", (double)cur_l->pic->orig_width / PIX_PER_CM);
    DoMethod(STR_PICT_WIDTH, MUIM_NoNotifySet , MUIA_String_Contents, tmp);
    
    DoMethod(SLI_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_Numeric_Value,  100);
    sprintf(tmp, "%8.3f", (double)cur_l->pic->orig_height / PIX_PER_CM);
    DoMethod(STR_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
    
    DoMethod(CHK_PICT_PROPORTIONAL, MUIM_NoNotifySet, MUIA_Selected, TRUE);
    refresh_line_area(cur_l, FALSE);
    
    DebugPrintF("[PICT] DoResetSize: done\n");
    
done: ;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoChangeWidth                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoChangeWidth,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoChangeWidth(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
    
    char tmp[16];
    int  width = 0, height = 0, per = 100;
    float widthcm; 
    ULONG prop = FALSE;
    
    DebugPrintF("[PICT] DoChangeWidth: enter cur_l=%p\n", (void *)cur_l);
    
    if (!cur_l || !cur_l->pic)
    {
        DebugPrintF("[PICT] DoChangeWidth: ERROR cur_l or pic is NULL\n");
        goto done;
    }
    
    if (cur_l->pic->orig_width <= 0)
    {
        DebugPrintF("[PICT] DoChangeWidth: ERROR orig_width=%ld\n", cur_l->pic->orig_width);
        goto done;
    }
 
    get_box_dimension(cur_l, &width, &height);
    if (Object == STR_PICT_WIDTH)
    {
        strcpy(tmp, (char *)(*Arg));
        sscanf(tmp, "%f", &widthcm);
        width = widthcm * PIX_PER_CM;
        per = 100 * width / cur_l->pic->orig_width;
        DoMethod(SLI_PICT_WIDTH, MUIM_NoNotifySet, MUIA_Numeric_Value,  per);
    }
    else
    {
        per = (int)(*Arg);
        width = cur_l->pic->orig_width * per / 100;
        sprintf(tmp, "%8.3f", (double)width / PIX_PER_CM);
        DoMethod(STR_PICT_WIDTH, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
    }
    
    get(CHK_PICT_PROPORTIONAL, MUIA_Selected, &prop);
    if (prop)
    {
        DoMethod(SLI_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_Numeric_Value, per);
        height = cur_l->pic->orig_height * per / 100;
        sprintf(tmp, "%8.3f", (double)height / PIX_PER_CM);
        DoMethod(STR_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
    }
    
  	refresh_line_area(cur_l, TRUE);
	stretch_box(cur_l, width, height);
	refresh_line_area(cur_l, FALSE);  
    
    DebugPrintF("[PICT] DoChangeWidth: done w=%ld h=%ld per=%ld\n", width, height, per);
    
done: ;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     DoChangeHeight                                                         */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, DoChangeHeight,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void DoChangeHeight(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	    
    char tmp[16];
    int  width = 0, height = 0, per = 100;
    float heightcm; 
    ULONG prop = FALSE;
    
    DebugPrintF("[PICT] DoChangeHeight: enter cur_l=%p\n", (void *)cur_l);
    
    if (!cur_l || !cur_l->pic)
    {
        DebugPrintF("[PICT] DoChangeHeight: ERROR cur_l or pic is NULL\n");
        goto done;
    }
    
    if (cur_l->pic->orig_height <= 0)
    {
        DebugPrintF("[PICT] DoChangeHeight: ERROR orig_height=%ld\n", cur_l->pic->orig_height);
        goto done;
    }
    
    get_box_dimension(cur_l, &width, &height);
    
    if (Object == STR_PICT_HEIGHT)
    {
        strcpy(tmp, (char *)(*Arg));
        sscanf(tmp, "%f", &heightcm);
        height = heightcm * PIX_PER_CM;
        per = 100 * height / cur_l->pic->orig_height;
        DoMethod(SLI_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_Numeric_Value,  per);
    }
    else
    {
        per = (int)(*Arg);
        height = cur_l->pic->orig_height * per / 100;
        sprintf(tmp, "%8.3f", (double)height / PIX_PER_CM);
        DoMethod(STR_PICT_HEIGHT, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
    }
    
    get(CHK_PICT_PROPORTIONAL, MUIA_Selected, &prop);
    if (prop)
    {
        DoMethod(SLI_PICT_WIDTH, MUIM_NoNotifySet, MUIA_Numeric_Value, per);
        width = cur_l->pic->orig_width * per / 100;
        sprintf(tmp, "%8.3f", (double)width / PIX_PER_CM);
        DoMethod(STR_PICT_WIDTH, MUIM_NoNotifySet, MUIA_String_Contents, tmp);
    }
    
  	refresh_line_area(cur_l, TRUE);
	stretch_box(cur_l, width, height);
	refresh_line_area(cur_l, FALSE);  
	
	DebugPrintF("[PICT] DoChangeHeight: done w=%ld h=%ld per=%ld\n", width, height, per);
	
done: ;
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

void z_setup_picturewin(void)
{
    DebugPrintF("[PICT] z_setup_picturewin: opening window\n");
    set(WinPict, MUIA_Window_Open, TRUE);
}
