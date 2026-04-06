/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_icon.c $
 *       $Revision: 341 $
 *       $Date: 2017-02-18 16:41:43 +0000 (Sat, 18 Feb 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include <proto/icon.h>
#include <workbench/icon.h>
#include <workbench/workbench.h>

#include "resources.h"
#include "a_savebitmap.h"
#include "a_coords.h"

void write_icon(char *name)
{
	struct DiskObject *diskobj;
	STRPTR olddeftool;
	char iconname[256];
	int  iconwidth = 64, iconheight = 64;
	int  width, height;
	float pagex,pagey;

	/* Generate thumbnail icon */
	sprintf(iconname,"%s.info", name);
	get_pagesizes(&pagex, &pagey);
	width  = CM_TO_PIX(pagex);
	height = CM_TO_PIX(pagey);
	if (width > height)
	{
		iconheight = iconwidth * height / width;
	}
	else
	{
		iconwidth = iconheight * width / height;
	}
	write_bitmap(iconname, 0, 0, width, height, iconwidth, iconheight, 2);
	
	/* Load DiskObject and modify parameters if necessary */
	if((diskobj=GetDiskObject((CONST_STRPTR)name)) == NULL)
	{
		diskobj=GetDefDiskObject(WBPROJECT);
	}
	
	if (diskobj != NULL)
	{
		if(diskobj->do_StackSize < 262144)
		{
			diskobj->do_StackSize = 262144;
		}
		olddeftool   = diskobj->do_DefaultTool;
		diskobj->do_DefaultTool=(STRPTR)appres.DefTool;
		PutDiskObject((CONST_STRPTR)name, diskobj);
		diskobj->do_DefaultTool = olddeftool;
		FreeDiskObject(diskobj);
	}
}

void read_icon(const char *name)
{
	struct DiskObject *diskobj;
	STRPTR *toolarray;
	char *s;
	int i;

	if((diskobj=GetDiskObject((CONST_STRPTR)name)))
	{
		toolarray = diskobj->do_ToolTypes;

		if((s=(char *)FindToolType(toolarray, (STRPTR)"PAGE")))
		{
			for(i=0; i < NUMPAPERSIZES; i++)
				if(strcmp(s, paper_sizes[i].sname) == 0)
					appres.papersize=i;
		}

		if((s=(char *)FindToolType(toolarray, (STRPTR)"EXPORTSCALE")))
		{
			if(atof(s) > 0)
				appres.magnification=100.0*atof(s);
		}
		FreeDiskObject(diskobj);
	}
}
