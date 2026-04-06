/*
 * TransFig: Facility for Translating Fig code
 * Parts Copyright (c) 1989-2002 by Brian V. Smith
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and
 * documentation files (the "Software"), including without limitation the
 * rights to use, copy, modify, merge, publish and/or distribute copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that this copyright
 * notice remain intact.
 *
 */

/*
 *	genpdf.c : pdf driver for fig2dev
 *
 *	Author: Brian V. Smith
 *		Uses genps functions to generate PostScript output then
 *		calls ghostscript (device pdfwrite) to convert it to pdf.
 */

#include "fig2dev.h"

#ifdef WIN32
#include <direct.h>
#endif

#ifdef WIN32
#include "genps.oldpatterns.h"
#else
#include "genps.h"
#endif

#include "object.h"
#include "texfonts.h"

#ifdef WIN32
#include "win32supp.h"
#endif

// begin includes added by AKS
extern void gen_ps_eps_option(char opt, char *optarg);
extern void genps_start(F_compound	*objects);
extern int genps_end(void);
// end includes added by AKS

static	FILE	*saveofile;
static	char	*ofile;

void genpdf_option(char opt, char *optarg)
{
	/* just use the eps options */
	pdfflag = True;
	epsflag = True;
	gen_ps_eps_option(opt, optarg);
}

void
genpdf_start(objects)
F_compound	*objects;
{
#ifdef WIN32
	char outputDirAndFile[1024] = "";
	char curdir[1024] = "";
	int retval = 0;
#endif

    /* divert output from ps driver to the pipe into ghostscript */
    /* but first close the output file that main() opened */
    saveofile = tfp;
    if (tfp != stdout)
	fclose(tfp);

    /* make up the command for gs */
    ofile = (to == NULL? "-": to);

#ifdef WIN32
	if(strlen(ghostScriptPath) > 0)
	{
		_getcwd(curdir, 1024);
		retval = _chdir(ghostScriptPath);

		if(strstr(ofile, ":") == NULL)
		{
			sprintf(outputDirAndFile, "%s\\%s", curdir, ofile);
		} else
		{
			sprintf(outputDirAndFile, "%s", ofile);
		}
	}

	sprintf(gscom,
	 "gswin32c.exe -q -dNOPAUSE -sAutoRotatePages=None -dAutoFilterColorImages=false -dColorImageFilter=/FlateEncode -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress -sOutputFile=\"%s\" - -c quit",
		outputDirAndFile);
#else
    sprintf(gscom,
	 "gs -q -dNOPAUSE -dBATCH -sAutoRotatePages=None -dAutoFilterColorImages=false -dColorImageFilter=/FlateEncode -sDEVICE=pdfwrite -dPDFSETTINGS=/prepress \"-sOutputFile=%s\" - -c quit",
		ofile);
#endif

#ifndef WIN32
    (void) signal(SIGPIPE, gs_broken_pipe);
#endif

	if ((tfp = popen(gscom,"w" )) == 0)
	{
		fprintf(stderr,"fig2dev: Can't open pipe to ghostscript\n");
		fprintf(stderr,"command was: %s\n", gscom);

#ifdef WIN32
		_chdir(curdir);
#endif
		exit(1);
	}

#ifdef WIN32
	if(strlen(ghostScriptPath) > 0)
		_chdir(curdir);
#endif

    genps_start(objects);
}

int
genpdf_end()
{
	/* wrap up the postscript output */
	if (genps_end() != 0)
	    return -1;		/* error, return now */

	if (pclose(tfp) != 0) {
	    fprintf(stderr,"Error in ghostcript command\n");
	    fprintf(stderr,"command was: %s\n", gscom);
	    return -1;
	}

#ifndef WIN32
	(void) signal(SIGPIPE, SIG_DFL);
#endif

	/* we've already closed the original output file */
	tfp = 0;

	/* all ok so far */

	return 0;
}

struct driver dev_pdf = {
  	genpdf_option,
	genpdf_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genps_text,
	genpdf_end,
	INCLUDE_TEXT
};
