/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/main.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#include "fig.h"
#include "intui.h"
#include <workbench/startup.h>
#ifndef __AROS__
#include <workbench/workbench.h>
#define WBenchMsg wbstartup
#else
#include <resources/processor.h>
#include <proto/processor.h>
#include <aros/startup.h>
#endif

#include "resources.h"
#include "mode.h"
#include "version.h"
#include "e_delete.h"
#include "a_eventloop.h"
#include "f_load.h"
#include "u_fonts.h"
#include "z_fig.h"
#include "z_mode.h"
#include "z_edit.h"

// just to be sure , that noone will have stack problems.
#ifndef __AROS__
long __stack=1000000;
#endif
#ifdef __amigaos4__
static const char * __attribute__((used)) stackcookie = "$STACK: 1000000";
#endif

/*************************  Flags  *******************************/

char version[]="$VER: AmiFig " AMIFIG_VERSION "." AMIFIG_COMMIT " (" AMIFIG_VERDATE ")";
static char		*file = NULL;
BOOL            SSE3_Support    = FALSE;
#ifdef __AROS__
APTR            ProcessorBase = NULL;
#endif

void load_by_wbarg(struct WBArg *wbarg)
{
	struct FileInfoBlock fib;
	BPTR lock, oldlock;
	char pathonly[FCHARS+DSIZE+1]="";
	BOOL first=TRUE;

	if(wbarg->wa_Lock && *wbarg->wa_Name)
	{
		oldlock=wbarg->wa_Lock;

		while((lock=ParentDir(oldlock)))
		{
			Examine(oldlock, &fib);
			strcpy(current_file, pathonly);
			sprintf(pathonly, "%s/%s", fib.fib_FileName, current_file);

			if(!first)
				UnLock(oldlock);
			else
				first=FALSE;

			oldlock=lock;
		}

		Examine(oldlock, &fib);
		strcpy(current_file, pathonly);
		sprintf(pathonly, "%s:%s", fib.fib_FileName, current_file);
		sprintf(current_file, "%s%s", pathonly, wbarg->wa_Name);

		if(!first)
			UnLock(oldlock);

		file=current_file;
	}
}

int get_args(int argc, char **argv)
{
	char *option;
	struct WBStartup *wbstartup;
	struct WBArg *wbarg;
	int i;
	int ErrCode = 0;

	if(argc == 0)  // Icon-Start
	{
		wbstartup=(struct WBStartup *)argv;

		for(i=0, wbarg=WBenchMsg->sm_ArgList; i < wbstartup->sm_NumArgs; i++, wbarg++)
		{
			if(i == 1)
				load_by_wbarg(wbarg);
		}
	}
	else           // Shell-Start
	{
		strcpy(homedir, "progdir:");

		while(--argc > 0)
		{
			option = *++argv;
			if(*option == '-')
			{
				while(*++option != '\0')
				{
					switch(*option)
					{
						case 'd':	// turn on debugging
							appres.DEBUG = TRUE;
							break;

						default:
							printf("%s: Flag <%c> unrecognized\n", argv[0], *option);
							ErrCode = 1;
							break;
					}
				}
			}
			else
			{
				file = option;
			}
		}
	}
	return(ErrCode);
}

int main(int argc, char **argv)
{
	long err_code = 1;
	
    printf("%s\n", version + 6);

	delete_all();

	if (get_args(argc, argv) == 0)
	{
		strcat(file_header, PROTOCOL_VERSION);

#ifdef __AROS__
        /* Under AROS check for SSE3 support */
        if ((ProcessorBase = OpenResource((CONST_STRPTR)PROCESSORNAME)) != NULL)
        {
            ULONG index = 0;
            struct TagItem ftags [] =
            {
                { GCIT_SelectedProcessor,   index },
                { GCIT_SupportsSSE3,        (IPTR)&SSE3_Support },
                { TAG_DONE, TAG_DONE }
            };

            GetCPUInfo(ftags);
        }
#endif
        
#ifdef __amigaos4__
extern BOOL Locale_Initialize(VOID);
extern VOID Locale_Deinitialize(VOID);
#endif

// ...

		if(OpenAll())
		{
#ifdef __amigaos4__
			Locale_Initialize();
#endif
			if (MakeMUIApp())
			{
				/* Application started correctly */
				err_code = 0;
				
				if(file != NULL)
				{
					load_file(file);
				}
				
				/* Start main loop */
				HandleInput();
				
				/* Main loop stopped, MUI application can be closed */
				CloseMUIApp();
			}
		}
		CloseAll();
	}
	exit(err_code);
}

/**********************  canvas variables  ************************/

void	(*canvas_locmove_proc)();
void	(*canvas_leftbut_proc)();
void	(*canvas_middlebut_proc)();
void	(*canvas_middlebut_save)();
void	(*canvas_rightbut_proc)();
void	(*canvas_rightbut_save)();
void	(*canvas_ref_proc) ();
int		fix_x, fix_y, cur_x, cur_y;

/************************  Routines  ****************************/

void null_proc(int a, int b)
{
    /* this is the null process, it does nothing as expected */
}
