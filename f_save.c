/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/f_save.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "u_list.h"
#include "w_msgpanel.h"
#ifndef AMIFIG
#include "patchlevel.h"
#include "version.h"
#include "f_read.h"
#include "f_util.h"
#include "u_create.h"
#include "w_export.h"
#include "w_indpanel.h"
#include "w_setup.h"
#include "w_util.h"
#include "w_zoom.h"
#include "e_compound.h"
#include "f_load.h"
#include "u_bound.h"

static int	write_tmpfile = 0;
static char	save_cur_dir[PATH_MAX];

#else
#include <sys/errno.h>
#include "a_icon.h"          
#include "a_color.h"

extern int	num_object;
int 		num_obj_in_comp;
#endif

/* Prototypes */
#ifndef AMIFIG
static void write_arrows(FILE *fp, F_arrow *f, F_arrow *b);
#endif

int write_objects (FILE *fp);
void write_fig_header(FILE *fp);
void write_arc(FILE *fp, F_arc *a);
void write_compound(FILE *fp, F_compound *com);
void write_ellipse(FILE *fp, F_ellipse *e);
void write_line(FILE *fp, F_line *l);
void write_spline(FILE *fp, F_spline *s);
void write_text(FILE *fp, F_text *t);
void write_comments (FILE *fp, char *com);
void write_colordefs(FILE *fp);

#ifndef AMIFIG
void init_write_tmpfile(void)
{
  write_tmpfile=1;
  /* save current file directory */
  strcpy(save_cur_dir, cur_file_dir);
  /* and set to TMPDIR to preserver absolute paths of any imported pictures */
  strcpy(cur_file_dir, TMPDIR);
}

void end_write_tmpfile(void)
{
  write_tmpfile=0;
  /* restore current file directory */
  strcpy(cur_file_dir, save_cur_dir);
}
#endif

int write_file(char *file_name)
{
	FILE *fp;
	char backup_name[FCHARS+DSIZE+1]="";

	if((fp = fopen(file_name, "r")))
	{
		fclose(fp);
		if(appres.WriteBackup)
		{
			sprintf(backup_name, "%s.bak", file_name);
			remove(backup_name);
			rename(file_name, backup_name);
		}
	}

	if ((fp = fopen(file_name, "w")) == NULL) {
		put_msg((char *)_(msg_OpenFileErr), file_name);
		return (-1);
	}

	num_object = 0;
	num_obj_in_comp=0;

	if(write_objects(fp)) {
		doMessage((char *)_(msg_WriteFileErr), file_name);
		return (-1);
	}

	put_msg((char *)_(msg_ObjectsSavedIn), num_object, file_name);
	figure_modified = 0;
	strcpy(current_file, file_name);

	if(appres.CreateIcons)
		write_icon(file_name);

	return (0);
}

/* for fig2dev */
int 
write_objects(FILE *fp)
{
	F_arc	   *a;
	F_compound *c;
	F_ellipse  *e;
	F_line	   *l;
	F_spline   *s;
	F_text	   *t;

	/*
	 * A 2 for the orientation means that the origin (0,0) is at the upper 
	 * left corner of the screen (2nd quadrant).
	 */
#ifndef AMIFIG
    if (!update_figs)
	put_msg((char *)_(msg_Writing));
#ifdef I18N
    /* set the numeric locale to C so we get decimal points for numbers */
    setlocale(LC_NUMERIC, "C");
#endif  /* I18N */
#else
	put_msg((char *)_(msg_Writing));
#endif
	write_fig_header(fp);
	for (a = fobjects.arcs; a != NULL; a = a->next)	{
		num_object++;
		write_arc(fp, a);
	}
	for (c = fobjects.compounds; c != NULL; c = c->next) {
		num_object++;
		write_compound(fp, c);
	}
	for (e = fobjects.ellipses; e != NULL; e = e->next)	{
		num_object++;
		write_ellipse(fp, e);
	}
	for (l = fobjects.lines; l != NULL; l = l->next) {
		num_object++;
		write_line(fp, l);
	}
	for (s = fobjects.splines; s != NULL; s = s->next)	{
		num_object++;
		write_spline(fp, s);
	}
	for (t = fobjects.texts; t != NULL; t = t->next) {
		num_object++;
		write_text(fp, t);
	}
#ifndef AMIFIG
#ifdef I18N
    /* reset to original locale */
    setlocale(LC_NUMERIC, "");
#endif  /* I18N */
#endif
	if (ferror(fp))	{
		fclose(fp);
		return (-1);
	}
	if (fclose(fp) == EOF)
		return (-1);


	return (0);
}

void write_fig_header(FILE *fp)
{
	fprintf(fp, "%s\n", file_header);
	fprintf(fp, appres.landscape? "Landscape\n": "Portrait\n");
	fprintf(fp, appres.flushleft? "Flush left\n": "Center\n");
	fprintf(fp, appres.INCHES? "Inches\n": "Metric\n");

	if(appres.papersize == CUSTOM)
		fprintf(fp, "%s\n", paper_sizes[PAPER_A4].sname);
	else
		fprintf(fp, "%s\n", paper_sizes[appres.papersize].sname);
	fprintf(fp, "%.2f\n", appres.magnification);
	fprintf(fp, "%s\n", appres.multiple? "Multiple": "Single");
	fprintf(fp, "%d\n", appres.transparent);
	fprintf(fp, "%d %d\n", (int)PIX_PER_INCH, 2);
	/* write the user color definitions (if any) */
	write_colordefs(fp);
}

/* write the user color definitions (if any) */
void write_colordefs(FILE *fp)
{
	int		    i,j;
    ULONG       alpha;

    /* Save single user colors */
	if (appres.save_whole_palette == FALSE)
		CheckUsedColors(&fobjects, FALSE);
		
	for (i=0; i<MAX_USR_COLS; i++)
	{
		if ((appres.save_whole_palette && ColorPalette[i+NUM_STD_COLS].Defined)|| ColorPalette[i+NUM_STD_COLS].Used)
        {
            alpha = (ColorPalette[i+NUM_STD_COLS].RGB & 0xFF000000) >> 24;
			if ((alpha == 0x00) || (alpha == 0xFF))
				/* No Alpha channel, keep compatibility with xfig */
				fprintf(fp, "0 %d #%06x\n", i+NUM_STD_COLS, (ColorPalette[i+NUM_STD_COLS].RGB & 0x00FFFFFF));
            else
				/* Color includes an alpha channel */
				fprintf(fp, "0 %d #%08x\n", i+NUM_STD_COLS, ColorPalette[i+NUM_STD_COLS].RGB);
        }
	}

    /* Save gradient definitions colors */
	for (i=0; i<MAX_GRD_COLS; i++)
	{   
        if (GradientTable[i] != NULL)
        {
            /* this color is a gradient definition */
            fprintf(fp, "0 %d",i+NUM_STD_COLS+MAX_USR_COLS);
            fprintf(fp, " %d %d %d %d %d %d %d %d %d\n",
					GradientTable[i]->type, GradientTable[i]->spread, GradientTable[i]->unit,
					GradientTable[i]->x1, GradientTable[i]->y1,
					GradientTable[i]->x2, GradientTable[i]->y2,
					GradientTable[i]->radius, GradientTable[i]->stops);

            for (j=0 ; j < GradientTable[i]->stops ; j++)
            {
                fprintf(fp," #%08x %d", GradientTable[i]->Stop[j].color, GradientTable[i]->Stop[j].stopvalue);
            }
            fprintf(fp,"\n");
        }
    }
}

void write_arc(FILE *fp, F_arc *a)
{
	F_arrow	   *f, *b;
    
	/* externally, type 1=open arc, 2=pie wedge */
	fprintf(fp, "%d %d %d %d %d %d %d %d %d %.3f %d %d %d %d %.3f %.3f %d %d %d %d %d %d\n",
	        O_ARC, a->type+1, a->style, a->thickness,
	        a->pen_color, a->fill_color, a->depth, a->pen_style, a->fill_style,
	        a->style_val, a->cap_style, a->direction,
	        ((f = a->for_arrow) ? 1 : 0), ((b = a->back_arrow) ? 1 : 0),
	        a->center.x, a->center.y,
	        a->point[0].x, a->point[0].y,
	        a->point[1].x, a->point[1].y,
	        a->point[2].x, a->point[2].y);
	if (f)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", f->type, f->style,
		        f->thickness, f->wd, f->ht);
	if (b)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", b->type, b->style,
		        b->thickness, b->wd, b->ht);
}

void write_compound(FILE *fp, F_compound *com)
{
	F_arc	   *a;
	F_compound	   *c;
	F_ellipse	   *e;
	F_line	   *l;
	F_spline	   *s;
	F_text	   *t;

	fprintf(fp, "%d %d %d %d %d\n", O_COMPOUND, com->nwcorner.x,
	        com->nwcorner.y, com->secorner.x, com->secorner.y);
	for (a = com->arcs; a != NULL; a = a->next, num_obj_in_comp++)
		write_arc(fp, a);
	for (c = com->compounds; c != NULL; c = c->next)
	{
		write_compound(fp, c);
		num_obj_in_comp++;
	}
	for (e = com->ellipses; e != NULL; e = e->next, num_obj_in_comp++)
		write_ellipse(fp, e);
	for (l = com->lines; l != NULL; l = l->next, num_obj_in_comp++)
		write_line(fp, l);
	for (s = com->splines; s != NULL; s = s->next, num_obj_in_comp++)
		write_spline(fp, s);
	for (t = com->texts; t != NULL; t = t->next, num_obj_in_comp++)
		write_text(fp, t);
	fprintf(fp, "%d\n", O_END_COMPOUND);

	num_obj_in_comp--;
}

void write_ellipse(FILE *fp, F_ellipse *e)
{
    /* get rid of any evil ellipses which have either radius = 0 */
	if (e->radiuses.x == 0 || e->radiuses.y == 0)
		return;

	fprintf(fp, "%d %d %d %d %d %d %d %d %d %.3f %d %.4f %d %d %d %d %d %d %d %d\n",
	        O_ELLIPSE, e->type, e->style, e->thickness,
	        e->pen_color, e->fill_color, e->depth, e->pen_style, e->fill_style,
	        e->style_val, e->direction, e->angle,
	        e->center.x, e->center.y,
	        e->radiuses.x, e->radiuses.y,
	        e->start.x, e->start.y,
	        e->end.x, e->end.y);
}

void write_line(FILE *fp, F_line *l)
{
	F_point	   *p;
	F_arrow	   *f, *b;
	int		   npts;

	if (l->points == NULL)
		return;

	/* count number of points and put it in the object */
#ifndef AMIFIG
	for (npts=0, p = l->points; p != NULL; p = p->next)
	npts++;
#else
	npts=num_points(l->points);
#endif

	fprintf(fp, "%d %d %d %d %d %d %d %d %d %.3f %d %d %d %d %d %d\n",
	        O_POLYLINE, l->type, l->style, l->thickness,
	        l->pen_color, l->fill_color, l->depth, l->pen_style,
	        l->fill_style, l->style_val, l->join_style, l->cap_style,
	        l->radius,
	        ((f = l->for_arrow) ? 1 : 0), ((b = l->back_arrow) ? 1 : 0), npts);
	if (f)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", f->type, f->style,
		        f->thickness, f->wd, f->ht);
	if (b)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", b->type, b->style,
		        b->thickness, b->wd, b->ht);

	/* handle picture stuff */
	if (l->type == T_PICTURE) {
	    char *s1;
#ifndef AMIFIG
	    picfile = NULL;
	    if (l->pic->pic_cache)
	    	picfile = l->pic->pic_cache->file;
	    if (picfile == NULL || strlen(picfile) == 0) {
			s1 = "<empty>";
	    } else if (strncmp(picfile, cur_file_dir, strlen(cur_file_dir))==0 && 
			strlen(picfile) > strlen(cur_file_dir) && picfile[strlen(cur_file_dir)] == '/') {
			/* use relative path if the file is under current directory */
		    s1 = &picfile[strlen(cur_file_dir)+1];
	    } else {
			/* use full path */
			s1 = picfile;
	    }
#else
		s1 = (char *)((l->pic->file && *l->pic->file)? l->pic->file: "<empty>");
#endif
		fprintf(fp, "\t%d %s\n", l->pic->flipped, s1);
	}

	fprintf(fp, "\t");
	npts=0;
	for (p = l->points; p != NULL; p = p->next)	{
		fprintf(fp, " %d %d", p->x, p->y);
		if (++npts >= 6 && p->next != NULL)	{
			fprintf(fp,"\n\t");
			npts=0;
		}
	}
	fprintf(fp, "\n");
}



void write_spline(FILE *fp, F_spline *s)
{
	F_sfactor	   *cp;
	F_point	   *p;
	F_arrow	   *f, *b;
	int		   npts;

	if (s->points == NULL)
		return;
	/* count number of points and put it in the object */
	for (npts=0, p = s->points; p != NULL; p = p->next)
		npts++;
        
	fprintf(fp, "%d %d %d %d %d %d %d %d %d %.3f %d %d %d %d\n",
	        O_SPLINE, s->type, s->style, s->thickness,
	        s->pen_color, s->fill_color, s->depth, s->pen_style,
	        s->fill_style, s->style_val, s->cap_style,
	        ((f = s->for_arrow) ? 1 : 0), ((b = s->back_arrow) ? 1 : 0), npts);
	if (f)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", f->type, f->style,
		        f->thickness, f->wd, f->ht);
	if (b)
		fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", b->type, b->style,
		        b->thickness, b->wd, b->ht);
	fprintf(fp, "\t");
	npts=0;
	for (p = s->points; p != NULL; p = p->next)
	{
		fprintf(fp, " %d %d", p->x, p->y);
		if (++npts >= 6 && p->next != NULL)
		{
			fprintf(fp,"\n\t");
			npts=0;
		}
	};
	fprintf(fp, "\n");

	if (s->sfactors == NULL)
		return;



	/* save new shape factor */

	fprintf(fp, "\t");
	npts=0;
	for (cp = s->sfactors; cp != NULL; cp = cp->next)
	{
		fprintf(fp, " %.3f",cp->s);
		if (++npts >= 8 && cp->next != NULL)
		{
			fprintf(fp,"\n\t");
			npts=0;
		}
	};
	fprintf(fp, "\n");
}

void write_text(FILE *fp, F_text *t)
{
	int		    l, len, lx, n;
	char	    c;
	char	    buf[4];

	if (t->length == 0)
		return;
	fprintf(fp, "%d %d %d %d %d %d %d %.4f %d %d %d %d %d ",
	        O_TXT, t->type, t->color, t->depth, t->pen_style,
	        t->font, t->size, t->angle,
	        t->flags, t->ascent+t->descent, t->length,
	        t->base_x, t->base_y);
	len = strlen(t->cstring);
	for (l=0; l<len; l++)
	{
		c = t->cstring[l];
		if (c == '\\')
			fprintf(fp,"\\\\");		/* escape a '\' with another one */
		else if ((unsigned int) c <= 255)
			putc(c,fp);			/* normal 7-bit ASCII */
		else
		{
			n = ((int) c)&255;		/* 8-bit, make \xxx (octal) */
			buf[3]='\0';
			for (lx = 2; lx>=0; lx--)
			{
				buf[lx] = '0'+(n%8);
				n /= 8;
			}
			fprintf(fp,"\\%s",buf);
		}
	}
	fprintf(fp,"\\001\n");		/* finish off with '\001' string */
}

/* write any arrow heads */
#ifndef AMIFIG
static void
write_arrows(FILE *fp, F_arrow *f, F_arrow *b)
{
    if (appres.write_v40) {
	if (f)
	    fprintf(fp, "  ForwardArrow { %d %d %.2f %.2f %.2f }\n", f->type, f->style,
		f->thickness, f->wd*15.0, f->ht*15.0);
	if (b)
	    fprintf(fp, "  BackwardArrow { %d %d %.2f %.2f %.2f }\n", b->type, b->style,
		b->thickness, b->wd*15.0, b->ht*15.0);
    } else {
	/* V3.2 */
	if (f)
	    fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", f->type, f->style,
		f->thickness, f->wd*15.0, f->ht*15.0);
	if (b)
	    fprintf(fp, "\t%d %d %.2f %.2f %.2f\n", b->type, b->style,
		b->thickness, b->wd*15.0, b->ht*15.0);
    } /* V4.0/V3.2 */
}
#endif
