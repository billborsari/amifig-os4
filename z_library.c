/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_library.c $
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

#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <mui/NListtree_mcc.h>					/* MUI mcc's */
#include <mui/NListview_mcc.h>

#ifdef ALIGN_LEFT								/* Needed for MUI mcc's ? */
#undef ALIGN_LEFT
#endif
#ifdef ALIGN_RIGHT
#undef ALIGN_RIGHT
#endif
#ifdef ALIGN_CENTER
#undef ALIGN_CENTER
#endif

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "intui.h"
#include "f_read.h"
#include "u_create.h"
#include "u_list.h"
#include "u_translate.h"
#include "u_free.h"
#include "f_save.h"
#include "z_fig.h"

#include "u_markers.h"
#include "e_placelib.h"
#include "mcc_figdisp.h"
#include "a_eventloop.h"
#include "z_edit.h"

/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/
#ifdef __AROS__
AROS_UFP3(APTR, NListTreeConstructor,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(APTR, NListTreeDestructor,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, NListTreeAct,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, AddLibObj,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
AROS_UFP3(void, AddLibraryDir,
    AROS_UFPA(struct Hook *, h, A0),
    AROS_UFPA(APTR , Object, A2),
    AROS_UFPA(IPTR *, Arg, A1));
#else
APTR NListTreeConstructor(struct Hook *h, APTR Object, ULONG *Arg);
APTR NListTreeDestructor(struct Hook *h, APTR Object, ULONG *Arg);
void NListTreeAct(struct Hook *h, APTR Object, ULONG *Arg);
void AddLibObj(struct Hook *h, APTR Object, ULONG *Arg);
void AddLibraryDir(struct Hook *h, APTR Object, ULONG *Arg);
#endif

static ULONG readdir(Object *list,STRPTR dir,struct MUI_NListtree_TreeNode *node);

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
APTR    LTR_LIB_LIBRARY, FIG_LIB_PREVIEW, BUT_LIB_ADD, TXT_LIB_COMMENT;
        
/*----------------------------------------------------------------------------*/
/*     MakeLibraryWindow - creates the Library window                         */
/*----------------------------------------------------------------------------*/

APTR MakeLibraryWindow(void)
{
    static struct Hook NListTreeConstructorHook;
    NListTreeConstructorHook.h_Entry = (HOOKFUNC)NListTreeConstructor;
    static struct Hook NListTreeDestructorHook;
    NListTreeDestructorHook.h_Entry = (HOOKFUNC)NListTreeDestructor;
	
    return(WindowObject,
		MUIA_Window_Title, "AmiFig : Library",
		MUIA_Window_ID, MAKEID('Z','F','L','I'),
		MUIA_Window_Screen, Scr,
		MUIA_Window_ScreenTitle, "AmiFIG",

		WindowContents,	HGroup,
            Child, VGroup,
				Child, NListviewObject,
					MUIA_NListview_NList, LTR_LIB_LIBRARY = NListtreeObject,
						InputListFrame,
						MUIA_NList_Format, "BAR,",
						MUIA_NList_Title, TRUE,
						MUIA_NListtree_DoubleClick, MUIV_NListtree_DoubleClick_All,
						MUIA_NListtree_ShowTree, TRUE,
						MUIA_NListtree_TreeColumn, 0,
						MUIA_NListtree_ConstructHook, &NListTreeConstructorHook,
						MUIA_NListtree_DestructHook,  &NListTreeDestructorHook,
                        MUIA_NListtree_CompareHook, MUIV_NListtree_CompareHook_LeavesBottom,
						MUIA_NListtree_EmptyNodes, TRUE,
					End,
				End,
				Child, BUT_LIB_ADD = MakeButton((UBYTE *)_(msg_AddLibrary), '\0', (UBYTE *)_(msg_AddNewLibraryObjectsToList)),
			End,
            Child, VGroup,
				Child, FIG_LIB_PREVIEW = NewObject(	mcc_fig->mcc_Class, NULL,
													MUIA_Background, MUII_BACKGROUND,
													MUIA_Frame, MUIV_Frame_ImageButton,
													MUIA_InputMode, MUIV_InputMode_RelVerify,
													TAG_DONE),
				Child, TXT_LIB_COMMENT = TextObject, 
					MUIA_Frame, MUIV_Frame_Text,
					MUIA_Text_SetVMax, FALSE, 
				End,
			End,
        End,
    End);
}

void z_library_methods(void)
{
    static struct Hook NListTreeActHook;
    NListTreeActHook.h_Entry = (HOOKFUNC)NListTreeAct;
    static struct Hook AddLibObjHook;
    AddLibObjHook.h_Entry = (HOOKFUNC)AddLibObj;
    static struct Hook AddLibraryDirHook;
    AddLibraryDirHook.h_Entry = (HOOKFUNC)AddLibraryDir;

	 /* Close window */
    DoMethod(WinLib, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, WinLib, 3, MUIM_Set, MUIA_Window_Open, FALSE);
			
	DoMethod(LTR_LIB_LIBRARY, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime, WinLib, 3, MUIM_CallHook, (IPTR)&NListTreeActHook, MUIV_TriggerValue);
    DoMethod(FIG_LIB_PREVIEW, MUIM_Notify, MUIA_Pressed,  FALSE, WinLib, 3, MUIM_CallHook, (IPTR)&AddLibObjHook, 0L);
    DoMethod(BUT_LIB_ADD    , MUIM_Notify, MUIA_Pressed,  FALSE, WinLib, 3, MUIM_CallHook, (IPTR)&AddLibraryDirHook, 0L);
	
	if (appres.AutoLoadLib)
	{
		/* Automatically load Libraries in default directory */
		readdir(LTR_LIB_LIBRARY,(STRPTR)"Libraries",NULL);
	}
}

STRPTR get_ext(STRPTR filename)
{
	static char extension[32];
	int position=strlen((char *)filename)-1;

	strcpy(extension,"");
	
	while(position > -1 && filename[position] != '.') position--;

	if (position > -1)
	{
		strncpy(extension,(const char *)&filename[position+1],32);
	}

	return (STRPTR)extension;
}

static ULONG readdir(Object *list,STRPTR dir,struct MUI_NListtree_TreeNode *node)
{  
	BPTR lock;
	struct MUI_NListtree_TreeNode *temp=NULL;
	struct FileInfoBlock fib;
	ULONG res=TNF_NOSIGN;

	put_msg("Scanning directory: %s", dir);

	if((lock=Lock(dir,ACCESS_READ)))
	{
		if(Examine(lock,&fib))
		{
			while(ExNext(lock,&fib))
			{
				if(fib.fib_DirEntryType>0)
				{
					char subdir[1024];
					
					/* Directory : Store as TNF_LIST */
					temp=(struct MUI_NListtree_TreeNode *)DoMethod(list,MUIM_NListtree_Insert,fib.fib_FileName,dir,node,MUIV_NListtree_Insert_PrevNode_Sorted,TNF_LIST);
					/* And Parse the sub directory */
					strcpy(subdir,(const char *)dir);
					AddPart((STRPTR)subdir, fib.fib_FileName, 1024);
					readdir(list,(STRPTR)subdir,temp);
				}
				else
				{
					/* File : only keep if it is a .fig file and store it */
					if (strnicmp((const char *)get_ext(fib.fib_FileName),"fig",3) == 0)
					{
						DoMethod(list,MUIM_NListtree_Insert,fib.fib_FileName,dir,node,MUIV_NListtree_Insert_PrevNode_Sorted,0);
					}
				}
			}
		}
		UnLock(lock);
	}
	return(res);
}

/* read the library object file and put into a compound */

F_compound * load_lib_obj(char *fname)
{
	F_compound	*c, *c2;
    int save_pointposn = cur_pointposn;
    BOOL save_allownegcoords = appres.allownegcoords;
    
    appres.allownegcoords = TRUE;
    cur_pointposn = P_ANYONE;
    
	c = create_compound();

	if (read_fig(fname, c, MERGE, 0, 0)==0)
    {
	    /* if there are no other objects than one compound, don't encapsulate
		it in another compound */
	    if (c->arcs == NULL && c->ellipses == NULL && c->lines == NULL && 
            c->splines == NULL && c->texts == NULL) 
        {
            /* but first check that there is a compound */
            if (c->compounds == NULL)
            {
                delete_compound(c);
                c = (F_compound *) NULL;
                appres.allownegcoords = save_allownegcoords;
                return(c);
            }
            /* now check if the compound is the only one */
            if (c->compounds->next == NULL)
            {
                /* yes, save ptr to embedded compound */
                c2 = c->compounds;
                /* move the top comment down after freeing any existing in that compound */
                if (c2->comments)
                    free((char *) c2->comments);
                c2->comments = c->comments;
                /* free the toplevel */
                free((char *) c);
                /* make the embedded compound the toplevel */
                c = c2;
            }
	    }
	    /* now translate compound to 0,0 */
	    translate_compound(c, -c->nwcorner.x, -c->nwcorner.y);
	} 
    else 
    {
		/* delete the compound we've just created */
		delete_compound(c);
		c = (F_compound *) NULL;
	}
    appres.allownegcoords = save_allownegcoords;
    cur_pointposn = save_pointposn;
	return(c);
}

/*----------------------------------------------------------------------------*/
/*     NListTreeConstructor                                                   */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(APTR, NListTreeConstructor,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
APTR NListTreeConstructor(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	
	struct MUIP_NListtree_ConstructMessage *msg = (struct MUIP_NListtree_ConstructMessage *)Arg;
    F_compound *figobject = NULL;
    char fname[1024];
	
    if (!(msg->Flags&TNF_LIST))
    {
        strcpy(fname,(const char *)msg->UserData);
		AddPart((STRPTR)fname, msg->Name, 1024);
		put_msg("Loading library object: %s", msg->Name);
        figobject = load_lib_obj(fname);
		if (figobject) put_msg("Successfully loaded: %s", msg->Name);
		else put_msg("Failed to load: %s", msg->Name);
    }

	return(figobject);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     NListTreeDestructor                                                    */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(APTR, NListTreeDestructor,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
APTR NListTreeDestructor(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    

	struct MUIP_NListtree_DestructMessage *msg = (struct MUIP_NListtree_DestructMessage *)Arg;
	F_compound *obj = (F_compound *)msg->UserData;

	if (obj)
	{
		free_compound(&obj);
	}
	
	return(NULL);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     NListTreeAct                                                           */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, NListTreeAct,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void NListTreeAct(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	struct MUI_NListtree_TreeNode *sel  = (struct MUI_NListtree_TreeNode *)*Arg;
	F_compound * object = (F_compound *)sel->tn_User;
	
    if (sel->tn_User)
    {
        set(FIG_LIB_PREVIEW, MYATTR_COMPOUND, object);
		if (object->comments)
		{
			set(TXT_LIB_COMMENT, MUIA_Text_Contents, object->comments);
		}
		else
		{
			set(TXT_LIB_COMMENT, MUIA_Text_Contents, "");
		}
    }
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     AddLibObj                                                              */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, AddLibObj,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void AddLibObj(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	struct MUI_NListtree_TreeNode *sel = NULL;

	/* Get current entry */
    get(LTR_LIB_LIBRARY, MUIA_NListtree_Active, &sel);
	
	if (sel != NULL)
	{
		if ((F_compound *)sel->tn_User != NULL)
		{
			/* Stop current action if any */
			stop_action();
			cur_mode = F_NULL;
			TurnOffEditButtons();
			
			/* Turn off markers */
			new_objmask = M_NONE;
			update_markers(new_objmask);
				
			/* Set Current Compound to library object */
			cur_c = (F_compound *)sel->tn_User;
			
			/* Activate Canvas window */
			ActivateWindow(canvas_win);
			
			/* Call e_placelib function */
			put_selected();
		}
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

/*----------------------------------------------------------------------------*/
/*     AddLibraryDir                                                          */
/*----------------------------------------------------------------------------*/

#ifdef __AROS__
AROS_UFH3(void, AddLibraryDir,
AROS_UFHA(struct Hook *, h, A0),
AROS_UFHA(APTR, Object, A2),
AROS_UFHA(IPTR *, Arg, A1));
{
    AROS_USERFUNC_INIT
#else
void AddLibraryDir(struct Hook *h, APTR Object, ULONG *Arg)
{
#endif    
	char libdirname[1024];
    struct MUI_NListtree_TreeNode *node=NULL;
	struct FileRequester	*file_requester = NULL;
	
	file_requester=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
														(ASLFR_Screen), 			Scr,
														(TAG_DONE));

	if (file_requester != NULL)
	{
		if(AslRequestTags(	file_requester, 
							(ASLFR_TitleText), 		(char *)_(msg_SelectDirectory),
							(ASLFR_InitialDrawer), 	"PROGDIR:",
							(ASLFR_DrawersOnly), 	TRUE,
							(ASLFR_RejectIcons), 	TRUE,
							(TAG_DONE)) != 0)
		{
			char shortname[1024];
			strmfp(libdirname, (const char *)file_requester->fr_Drawer, (const char *)file_requester->fr_File);
			
			/* Use only folder name for display if possible */
			if (stcgfn(shortname, libdirname) == 0) strcpy(shortname, libdirname);

			/* Insert category at root (parent=NULL) */
			node=(struct MUI_NListtree_TreeNode *)DoMethod(LTR_LIB_LIBRARY,MUIM_NListtree_Insert,shortname,libdirname,NULL,MUIV_NListtree_Insert_PrevNode_Sorted,TNF_LIST);
			
			if (node)
			{
				readdir(LTR_LIB_LIBRARY,(STRPTR)libdirname,node);
			}
		}
		FreeAslRequest(file_requester);
	}
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}
