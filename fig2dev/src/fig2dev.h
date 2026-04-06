/*
 * TransFig: Facility for Translating Fig code
 * Copyright (c) 1991 by Micah Beck
 * Parts Copyright (c) 1985-1988 by Supoj Sutanthavibul
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

#pragma once

#ifdef WIN32
#include <io.h>
#include <process.h>
#endif

typedef unsigned char byte;
typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef int BOOL;

#include <stdlib.h>
#include <stdio.h>
#include "patchlevel.h"
#include <math.h>

#ifndef WIN32
#include <sys/file.h>
#include <pwd.h>
#include <unistd.h>

#define _unlink unlink
#define _fileno fileno
#define _read read
#define _write write
#define _strdup strdup
#endif

#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>

#include "pi.h"

/* location for temporary files */
#ifdef WIN32
extern char TMPDIR[1024];
#else
#ifdef AMIGA
#define TMPDIR "T:"
#else
#define TMPDIR "/tmp"
#endif
#endif

typedef char Boolean;
#define	NO	2
#define	False	0
#define	True	1

#define DEFAULT_FONT_SIZE 11

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifdef USE_INLINE
#define INLINE inline
#else
#define INLINE
#endif /* USE_INLINE */

/* include ctype.h for isascii() and isxdigit() macros */
#include <ctype.h>

#ifndef X_NOT_STDC_ENV
#  include <string.h>
#else
#  ifdef SYSV
#    include <string.h>
#  else
#    include <strings.h>
#    ifndef strchr
#      define strchr index
#    endif
#    ifndef strrchr
#      define strrchr rindex
#    endif
#  endif  /* SYSV else */
#endif  /* !X_NOT_STDC_ENV else */

#if defined(hpux) || defined(SYSV) || defined(SVR4)
#include <sys/types.h>
#define bzero(s,n) memset((s),'\0',(n))
#endif

#define round(x)	((int) ((x) + ((x >= 0)? 0.5: -0.5)))

#define	NUM_STD_COLS	32
#define	MAX_USR_COLS	512

#define NUMSHADES	21
#define NUMTINTS	20
#define NUMPATTERNS     22

#ifndef RGB_H
#define RGB_H
typedef struct _RGB {
	unsigned short red, green, blue;
	} RGB;
#endif /* RGB_H */

float rgb2luminance (float r, float g, float b);

#if defined(AMIGA) && !defined(__AROS__) && !defined(__amigaos4__)
void __stdargs put_msg(UBYTE *format, ...);
#else
void put_msg(char *format, ...);
#endif

extern void	unpsfont();
extern int lookup_X_color(char *name, RGB  *rgb);

extern char	Err_badarg[];
extern char	Err_mem[];

extern char	*PSfontnames[];
extern int	PSisomap[];

extern char	*prog, *from, *to;
extern char	*name;
extern double	font_size;
extern Boolean	correct_font_size;	/* use correct font size */
extern double	mag, fontmag;
extern FILE	*tfp;

extern double	ppi;		/* Fig file resolution (e.g. 1200) */
extern int	llx, lly, urx, ury;
extern Boolean	landscape;
extern Boolean	center;
extern Boolean	multi_page;	/* multiple page option for PostScript */
extern Boolean	overlap;	/* overlap pages in multiple page output */
extern Boolean	orientspec;	/* true if the command-line args specified land or port */
extern Boolean	centerspec;	/* true if the command-line args specified -c or -e */
extern Boolean	magspec;	/* true if the command-line args specified -m */
extern Boolean	transspec;	/* set if the user specs. the GIF transparent color */
extern Boolean	paperspec;	/* true if the command-line args specified -z */
extern Boolean  boundingboxspec;/* true if the command-line args specified -B or -R */
extern Boolean	multispec;	/* true if the command-line args specified -M */
extern Boolean	metric;		/* true if the file contains Metric specifier */
extern char	gif_transparent[]; /* GIF transp color hex name (e.g. #ff00dd) */
extern char	papersize[];	/* paper size */
extern char     boundingbox[];  /* boundingbox */
extern float	THICK_SCALE;	/* convert line thickness from screen res. */
extern char	lang[];		/* selected output language */
extern char	*Fig_color_names[]; /* hex names for Fig colors */
extern RGB	background;	/* background (if specified by -g) */
extern Boolean	bgspec;		/* flag to say -g was specified */
extern float	grid_minor_spacing; /* grid minor spacing (if any) */
extern float	grid_major_spacing; /* grid major spacing (if any) */
extern char	gscom[];	/* to build up a command for ghostscript */
extern Boolean	psencode_header_done; /* if we have already emitted PSencode header */
extern Boolean	transp_header_done;   /* if we have already emitted transparent image header */
extern Boolean	grayonly;	/* convert colors to grayscale (-N option) */

struct paperdef
{
    char *name;			/* name for paper size */
    int width;			/* paper width in points */
    int height;			/* paper height in points */
};

#define NUMPAPERSIZES 29
extern struct paperdef paperdef[];

#ifdef GRADIENTS
typedef struct _GradientStop
{
    unsigned long   color;
    long		    stopvalue;
}
GradientStop;

typedef struct _Gradient
{
    long         type;
    long         spread;
    long         unit;
    long         x1;
    long         y1;
    long         x2;
    long         y2;
    long         radius;
    long         stops;
    GradientStop Stop[16];
}
Gradient;

#define MAX_GRD_COLS    512
extern Gradient *GradientTable[MAX_GRD_COLS];
#endif

/* user-defined colors */
typedef		struct{
			int c,r,g,b;
#ifdef GRADIENTS
			int a, grad;
#endif
			}
		User_color;

extern User_color	user_colors[MAX_USR_COLS + MAX_GRD_COLS];
extern int		user_col_indx[MAX_USR_COLS + MAX_GRD_COLS];
extern int		num_usr_cols;
extern Boolean		pats_used, pattern_used[NUMPATTERNS];

extern void	gendev_null();
extern void gendev_null2(float p1, float p2);
void gs_broken_pipe(int sig);

/* for GIF files */
#define	MAXCOLORMAPSIZE 256

struct Cmap {
	unsigned short red, green, blue;
	unsigned long pixel;
};

/* define PATH_MAX if not already defined */
/* taken from the X11R5 server/os/osfonts.c file */
#ifndef X_NOT_POSIX
#ifdef _POSIX_SOURCE
#include <limits.h>
#else
#if !defined(sun) || defined(sparc)
#define _POSIX_SOURCE
#include <limits.h>
#undef _POSIX_SOURCE
#endif /* !defined(sun) || defined(sparc) */
#endif /* _POSIX_SOURCE */
#endif /* X_NOT_POSIX */

#ifndef PATH_MAX
#ifndef WIN32
#include <sys/param.h>
#endif
#ifdef MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 1024
#endif /* MAXPATHLEN */
#endif /* PATH_MAX */

#if ( !defined(__NetBSD__) && !defined(__DARWIN__) && !defined(WIN32) && !defined(__FreeBSD) )
extern int		sys_nerr, errno;
#endif

#if ( !(defined(BSD) && (BSD >= 199306)) && !defined(__NetBSD__) && \
	!defined(__GNU_LIBRARY__) && !defined(__FreeBSD__) && \
	!defined(__GLIBC__) && !defined(__CYGWIN__) && !defined(__DARWIN__) && !defined(WIN32))
	    extern char *sys_errlist[];
#endif

typedef struct _point
{
    int x,y;
} Point;

// includes added by AKS

#if defined(WIN32) // || defined(AMIGA)
#define cfree(x) free(x);
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#define getpid _getpid
#define pclose _pclose
#define popen _popen
#define sleep(x) winSleep(x)
#define gethostname winGethostname
#endif

#include "object.h"

/* 
 * Device driver interface structure
 */
struct driver {
 	void (*option)(char, char *);	/* interpret driver-specific options */
  	void (*start)(F_compound *);	/* output file header */
  	void (*grid)(float major, float minor);		/* draw grid */
	void (*arc)(F_arc *);		/* object generators */
	void (*ellipse)(F_ellipse *);
	void (*line)(F_line *);
	void (*spline)(F_spline *);
	void (*text)(F_text *);
	int (*end)();		/* output file trailer (returns status) */
  	int text_include;	/* include text length in bounding box */
#define INCLUDE_TEXT 1
#define EXCLUDE_TEXT 0
};

extern void print_comments(char	*string1, F_comment	*comment, char	*string2);
extern int ReadFromBitmapFile(FILE *file, unsigned int *width, unsigned int *height, unsigned char **data_ret);
//extern void calc_arrow(int x1, int y1, int x2, int y2, int *c1x, int *c1y, int *c2x, int *c2y, F_arrow *arrow, Point points[], int *npoints, int *nboundpts);
extern void calc_arrow(int x1, int y1, int x2, int y2, int linethick, F_arrow *arrow, Point points[], int *npoints, Point  fillpoints[], int *nfillpoints, Point  clippts[], int *nclippts);
extern void compute_arcarrow_angle(double x1, double y1, double x2, double y2, int direction, F_arrow *arrow, int *x, int *y);
