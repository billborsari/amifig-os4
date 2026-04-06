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
 *	genbitmaps.c : bitmap driver for fig2dev
 *
 *	Author: Brian V. Smith
 *		Handles AutoCad Slide, GIF, JPEG, TIFF, PCX, PNG, XBM and XPM.
 *		Uses genps functions to generate PostScript output then
 *		calls ghostscript to convert it to the output language
 *		if ghostscript has a driver for that language, or to ppm
 *		if otherwise. If the latter, ppmtoxxx is then called to make
 *		the final XXX file.
 */

#include "fig2dev.h"

#ifdef WIN32
#include <direct.h>
#endif

#if defined(WINFIG)
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
extern void genps_start(F_compound	*objects);
extern int genps_end(void);
// end includes added by AKS

static	char	 *gsdev,tmpname[PATH_MAX];
static	Boolean	 direct;
static	FILE	*saveofile;
static	char	*ofile;
static	int	 width,height;
static	int	 jpeg_quality=75;
static	int	 border_margin = 0;
static	int	 smooth = 0;

void genbitmaps_option(char opt, char *optarg)
{
    switch (opt) {

	case 'b':			/* border margin around bitmap */
	    sscanf(optarg,"%d",&border_margin);
	    break;

	case 'g':			/* background color (handled in postscript gen) */
	    if (lookup_X_color(optarg,&background) >= 0) {
		bgspec = True;
	    } else {
		fprintf(stderr,"Can't parse color '%s', ignoring background option\n",
				optarg);
	    }
	    break;

	case 'N':			/* convert colors to grayscale */
	    grayonly = 1;
	    break;

	case 'q':			/* jpeg image quality */
	    if (strcmp(lang,"jpeg") != 0)
		fprintf(stderr,"-q option only allowed for jpeg quality; ignored\n");
	    sscanf(optarg,"%d",&jpeg_quality);
	    break;

	case 't':			/* GIF transparent color */
	    if (strcmp(lang,"gif") != 0)
		fprintf(stderr,"-t option only allowed for GIF transparent color; ignored\n");
	    (void) strcpy(gif_transparent,optarg);
	    transspec = True;
	    break;

	case 'S':			/* smoothing factor */
	    sscanf(optarg,"%d",&smooth);
	    if (smooth != 0 && smooth != 1 && smooth != 2 && smooth != 4) {
		fprintf(stderr,
			"fig2dev: bad value for -S option: %s, should be 0, 2 or 4\n",
			optarg);
		exit(1);
	    }
	    break;

	case 'F':	/* ignore magnification, font sizes and lang here */
	case 'f':
	case 'm':
	case 's':
      	case 'G':	/* grid */
	case 'L':
			/* these are all handled in fig2dev.c */
	    break;

	default:
	    put_msg(Err_badarg, opt, lang);
	    break;
    }
}

//HANDLE readEnd;
//PHANDLE writeEnd;

void genbitmaps_start(F_compound *objects)
{
    char extra_options[200];
    float bd;

#ifdef WIN32
	char curdir[1024] = "";
	char outputDirAndFile[1024] = "";
	int retval = 0;
//	char gswinCommand[1024];
#endif

    bd = border_margin * THICK_SCALE;

    llx -= bd;
    lly -= bd;
    urx += bd;
    ury += bd;

    /* make command for ghostscript */

    width=round(mag*(urx-llx)/THICK_SCALE);
    height=round(mag*(ury-lly)/THICK_SCALE);

    /* Add conditionals here if gs has a driver built-in */
    /* gs has a driver for png, ppm, pcx, jpeg and tiff */

    direct = True;
    ofile = (to == NULL? "-": to);
    extra_options[0]='\0';

    gsdev = NULL;
    /* if we're smoothing, we'll generate ppm and tell gs to
		use TextAlphaBits and GraphicsAlphaBits */
    if (strcmp(lang,"pcx")==0) {
	gsdev="pcx256";
    } else if (strcmp(lang,"ppm")==0) {
	gsdev="ppmraw";
    } else if (strcmp(lang,"png")==0) {
	gsdev="png16m";
    } else if (strcmp(lang,"tiff")==0) {
	/* use the 24-bit - unfortunately, it doesn't use compression */
	gsdev="tiff24nc";
    } else if (strcmp(lang,"jpeg")==0) {
	gsdev="jpeg";
	/* set quality for JPEG */
	sprintf(extra_options," -dJPEGQ=%d",jpeg_quality);
    }
    if (smooth > 1) {
      sprintf(extra_options+strlen(extra_options),
	      " -dTextAlphaBits=%d -dGraphicsAlphaBits=%d",smooth,smooth);
    }
    /* no driver in gs or we're smoothing, use ppm output then use ppmtoxxx later */
    if (gsdev == NULL) {
	gsdev="ppmraw";
	if (smooth > 1 || strcmp(lang,"ppm")) {
	    /* make a unique name for the temporary ppm file */
	    sprintf(tmpname,"%s/f2d%d.ppm",TMPDIR,getpid());
	    ofile = tmpname;
	    direct = False;
	}
    }

    /* make up the command for gs */
#ifdef WIN32
	_getcwd(curdir, 1024);

	if(strlen(ghostScriptPath) > 0)
	{
		retval = _chdir(ghostScriptPath);

		if(retval == -1)
		{
			fprintf(stderr,"fig2dev: Could not change to: %s\n", ghostScriptPath);
		} else
		{
			fprintf(stderr,"fig2dev: changed dir to %s\n", ghostScriptPath);
		}
	}

//SetStdHandle(STD_INPUT_HANDLE, NULL);
//SetStdHandle(STD_OUTPUT_HANDLE, NULL);
	if(strstr(ofile, ":") == NULL)
	{
		sprintf(outputDirAndFile, "%s\\%s", curdir, ofile);
	} else
	{
		sprintf(outputDirAndFile, "%s", ofile);
	}

    sprintf(gscom, "gswin32c.exe -q -dSAFER -sDEVICE=%s -r80 -g%dx%d -sOutputFile=\"%s\" %s -",
		   gsdev, width, height, outputDirAndFile, extra_options);
#else
     sprintf(gscom, "gs -q -dSAFER -sDEVICE=%s -r80 -g%dx%d \"-sOutputFile=%s\" %s -",
		   gsdev, width, height, ofile, extra_options);
#endif

   /* divert output from ps driver to the pipe into ghostscript */
    /* but first close the output file that main() opened */
	saveofile = tfp;
	if (tfp != stdout)
		fclose(tfp);

#ifndef WIN32
    (void) signal(SIGPIPE, gs_broken_pipe);
#endif
/*
	CreatePipe(&readEnd, &writeEnd, NULL, 100000);

	BOOL success = executeCommand(gscom);
*/

	if ((tfp = popen(gscom,"w" )) == NULL)
	{
		fprintf(stderr,"fig2dev: Can't open pipe to ghostscript\n");
		fprintf(stderr,"error was: %s\n", strerror( errno ));
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

    /* generate eps and not ps */
    epsflag = True;
    genps_start(objects);
}

int
genbitmaps_end()
{
	char	 com[PATH_MAX+200],com1[200];
	char	 errfname[PATH_MAX];
	char	*tmpname1;
	int	 status;

	/* wrap up the postscript output */
	if (genps_end() != 0)
	    return -1;		/* error, return now */

	/* add a showpage so ghostscript will produce output */
	fprintf(tfp, "showpage\n");

	status = pclose(tfp);
	/* we've already closed the original output file */
	tfp = 0;
	if (status != 0) {
	    fprintf(stderr,"Error in ghostcript command\n");
	    fprintf(stderr,"command was: %s\n", gscom);
	    return -1;
	}

#ifndef WIN32
	(void) signal(SIGPIPE, SIG_DFL);
#endif

	/* all ok so far */
	status = 0;

	/* for the formats that are only 8-bits, reduce the colors to 256 */
	/* and pipe through the ppm converter for that format */
#ifdef AMIGA
	if (!direct) {
	    char pipe_tmp[PATH_MAX];
	    char *lang_tool = "";
	    sprintf(pipe_tmp, "T:f2d_pipe%d.ppm", getpid());
	    
	    if (strcmp(lang, "gif")==0) lang_tool = "ppmtogif";
	    else if (strcmp(lang, "jpeg")==0) lang_tool = "ppmtojpeg";
	    else if (strcmp(lang, "xbm")==0) lang_tool = "pbmtoxbm";
	    else if (strcmp(lang, "xpm")==0) lang_tool = "ppmtoxpm";
	    else if (strcmp(lang, "sld")==0) lang_tool = "ppmtoacad";
	    else if (strcmp(lang, "pcx")==0) lang_tool = "ppmtopcx";
	    else if (strcmp(lang, "png")==0) lang_tool = "pnmtopng";
	    else if (strcmp(lang, "tiff")==0) lang_tool = "pnmtotiff";
	    
	    /* Step 1: Quantize/Process to intermediate file */
	    if (strcmp(lang, "gif")==0 || strcmp(lang, "xpm")==0) {
		sprintf(com, "ppmquant 256 %s > %s", tmpname, pipe_tmp);
	    } else if (strcmp(lang, "xbm")==0) {
		sprintf(com, "ppmtopgm %s | pgmtopbm > %s", tmpname, pipe_tmp);
	    } else {
                /* For others, just copy or link if needed, but let's just use tmpname directly in step 2 if no quant needed */
                strcpy(pipe_tmp, tmpname);
                com[0] = '\0';
            }
            
            if (com[0]) {
                status = system(com);
                if (status != 0) {
                    fprintf(stderr, "fig2dev: ppmquant failed\n");
                    _unlink(pipe_tmp);
                    return -1;
                }
            }
            
            /* Step 2: Final conversion to output file */
            if (saveofile != stdout) {
                if (strcmp(lang, "gif")==0 && gif_transparent[0]) {
                    sprintf(com, "%s -transparent %s %s > \"%s\"", lang_tool, gif_transparent, pipe_tmp, to);
                } else {
                    sprintf(com, "%s %s > \"%s\"", lang_tool, pipe_tmp, to);
                }
            } else {
                sprintf(com, "%s %s", lang_tool, pipe_tmp);
            }
            
            status = system(com);
            if (pipe_tmp != tmpname) _unlink(pipe_tmp);
	}
#else
	if (!direct) {
	    tmpname1 = tmpname;
	    strcpy(com, "(");
	    if (strcmp(lang, "gif")==0) {
		if (gif_transparent[0]) {
		    /* escape the first char of the transparent color (#) for the shell */
		    sprintf(com1,"ppmquant 256 %s | ppmtogif -transparent \\%s",
			tmpname1, gif_transparent);
		} else {
		    sprintf(com1,"ppmquant 256 %s | ppmtogif",tmpname1);
		}
	    } else if (strcmp(lang, "jpeg")==0) {
		sprintf(com1, "ppmtojpeg --quality=%d %s", jpeg_quality, tmpname1);
	    } else if (strcmp(lang, "xbm")==0) {
		sprintf(com1,"ppmtopgm %s | pgmtopbm | pbmtoxbm",tmpname1);
	    } else if (strcmp(lang, "xpm")==0) {
		sprintf(com1,"ppmquant 256 %s | ppmtoxpm",tmpname1);
	    } else if (strcmp(lang, "sld")==0) {
		sprintf(com1,"ppmtoacad %s",tmpname1);
	    } else if (strcmp(lang, "pcx")==0) {
		sprintf(com1, "ppmtopcx %s", tmpname1);
	    } else if (strcmp(lang, "ppm")==0) {
		com1[0] = '\0';				/* nothing to do for ppm */
	    } else if (strcmp(lang, "png")==0) {
		sprintf(com1, "pnmtopng %s", tmpname1);
	    } else if (strcmp(lang, "tiff")==0) {
		sprintf(com1, "pnmtotiff %s", tmpname1);
	    } else {
		fprintf(stderr, "fig2dev: unsupported image format: %s\n", lang);
		exit(1);
	    }
	    strcat(com, com1);

	    if (saveofile != stdout) {
		/* finally, route output from ppmtoxxx to final output file, if
		   not going to stdout */
		strcat(com," > \"");
		strcat(com,to);
		strcat(com,"\"");
	    }
	    /* close off parenthesized command stream */
	    strcat(com,")");

	    /* make a unique name for an error file */
	    sprintf(errfname,"%s/f2d%d.err",TMPDIR,getpid());

	    /* send all messages to error file */
	    strcat(com," 2> ");
	    strcat(com,errfname);

	    /* execute the ppm program */
	    if ((status=system(com)) != 0) {
		FILE *errfile;

		/* force to -1 */
		status = -1;

		/* seems to be a race condition where not all of the messages
		   make it into the error file before we open it, so we'll wait a tad */
		sleep(1);
		errfile = fopen(errfname,"r");
		fprintf(stderr,"fig2dev: error while converting image.\n");
		fprintf(stderr,"Command used:\n  %s\n",com);
		fprintf(stderr,"Messages resulting:\n");
		if (errfile == 0)
		    fprintf(stderr,"can't open error file %s\n",errfname);
		else {
		    while (!feof(errfile)) {
			if (fgets(com, sizeof(com)-1, errfile) == NULL)
			    break;
			fprintf(stderr,"  %s",com);
		    }
		    fclose(errfile);
		}
	    }

	    /* finally, remove the temporary file and the error file */
	    _unlink(tmpname);
	    _unlink(errfname);
	}
#endif

	return status;
}

struct driver dev_bitmaps = {
  	genbitmaps_option,
	genbitmaps_start,
	genps_grid,
	genps_arc,
	genps_ellipse,
	genps_line,
	genps_spline,
	genps_text,
	genbitmaps_end,
	INCLUDE_TEXT
};


