/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_io.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <unistd.h>
#include "intui.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "w_cursor.h"
#include "w_msgpanel.h"
#include "f_load.h"
#include "f_save.h"

extern BOOL is_on_pubscreen;

void ImportSVG(void)
{
    char SvgFile[256];
    char ext_cmd[256];
    FILE	   *fp;
    struct FileRequester *file_requester;
	
	file_requester=(struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
													(ASLFR_Screen), 			Scr,
													(TAG_DONE));

	if (file_requester != NULL)
	{
		// Open Drawer requester
		if(AslRequestTags(	file_requester, 
							(ASLFR_TitleText), (char *)_(msg_SelectSVGfile),
							(ASLFR_InitialDrawer), DirectoryName,
							(ASLFR_DoPatterns), TRUE,
							(ASLFR_RejectIcons), TRUE,
							(ASLFR_InitialPattern), "#?.svg",
							(TAG_DONE)) != 0)
		{
			strncpy(SvgFile, (const char *)file_requester->fr_Drawer, DSIZE);
			AddPart((STRPTR)SvgFile, file_requester->fr_File, 256);

			// build command line
			sprintf(ext_cmd,"svg2fig/svg2fig %s -o T:tmp.fig -s %5.1f -i %d", SvgFile, (float)appres.SVG_Scale/100.0, appres.SVG_InterSteps);
			Execute((CONST_STRPTR)ext_cmd, NULL, NULL);
			if ((fp = fopen("T:tmp.fig", "r")) != NULL)
			{
				fclose(fp);
				load_file("T:tmp.fig");
				remove("T:tmp.fig");
			}
		}
		FreeAslRequest(file_requester);
	}
}

void ImportR2V(void)
{
    char ext_cmd[256];
    FILE	   *fp;

	if (strcmp(appres.r2v_cmd,"not configured")!=0)
	{
		(*canvas_rightbut_proc)(0,0,0);
		if (appres.r2v_screenopt[0]!='\0')
			if(is_on_pubscreen)
				sprintf(ext_cmd,"%s %s T:tmp.fig %s %s",appres.r2v_cmd, appres.r2v_outputopt, appres.r2v_screenopt, PubScreenName);
			else
				sprintf(ext_cmd,"%s %s T:tmp.fig %s %s",appres.r2v_cmd, appres.r2v_outputopt, appres.r2v_screenopt, "AmiFIG");
		else
			sprintf(ext_cmd,"%s %s T:tmp.fig",appres.r2v_cmd, appres.r2v_outputopt);
		if (appres.r2v_otheropt[0]!='\0')
			sprintf(ext_cmd,"%s %s",ext_cmd,appres.r2v_otheropt);
		Execute((CONST_STRPTR)ext_cmd, NULL, NULL);
		if ((fp = fopen("T:tmp.fig", "r")) != NULL)
		{
			fclose(fp);
			load_file("T:tmp.fig");
			remove("T:tmp.fig");
		}
	}
	else
		doMessage((char *)_(msg_RAS2VECNotConfigured));
}

/* export file using fig2dev */
void ExportF2D(void)
{
	int modified;
	char ext_cmd[256], tempnam[L_tmpnam],tempdir[4+L_tmpnam],tempfilename[FILENAME_MAX]="";
	BPTR dirlock;
	
	if (strcmp(appres.f2d_cmd,"not configured")!=0)
	{
		beginWait();
		// write temporary file to be used as input for fig2dev later
		tmpnam(tempnam);
		sprintf(tempdir,"ram:%s",tempnam+2);
		dirlock = CreateDir((CONST_STRPTR)tempdir);
		if (!dirlock)
		{
			doMessage((char *)_(msg_WriteFileErr), tempdir);
			return;
		}
		UnLock(dirlock);
		sprintf(tempfilename, "%s/%s", tempdir, FileName);
		modified=figure_modified;
		write_file(tempfilename);
		if(modified)
			set_modifiedflag();

		// Prepare external command
		if (appres.f2d_screenopt[0]!='\0')
			if(is_on_pubscreen)
				sprintf(ext_cmd,"%s %s %s",appres.f2d_cmd, appres.f2d_screenopt, PubScreenName); 
			else
				sprintf(ext_cmd,"%s %s %s",appres.f2d_cmd, appres.f2d_screenopt, "AmiFIG");
		else
			sprintf(ext_cmd,"%s",appres.f2d_cmd);	
		sprintf(ext_cmd,"%s %s %s", ext_cmd, appres.f2d_inputopt, tempfilename);
		sprintf(ext_cmd,"%s %s \"%s\"", ext_cmd, appres.f2d_outputopt, DirectoryName);
		if (appres.f2d_otheropt[0]!='\0')
			sprintf(ext_cmd,"%s %s", ext_cmd, appres.f2d_otheropt);

		// Execute command
		Execute((CONST_STRPTR)ext_cmd, NULL, NULL);

		// Cleanup
		remove(tempfilename);
		strcat(tempfilename, ".info");
		remove(tempfilename);
		rmdir(tempdir);
		endWait();
	}
	else
		doMessage((char *)_(msg_FIG2DEVNotConfigured));
}

