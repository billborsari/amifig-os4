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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/f_load.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "f_read.h"
#include "u_undo.h"
#include "w_msgpanel.h"
#include "u_list.h"
#include "u_redraw.h"
#include "w_cursor.h"

#ifndef AMIFIG
#include "figx.h"
#include "f_util.h"
#include "u_create.h"
#include "w_cmdpanel.h"
#include "w_export.h"
#include "w_file.h"
#include "w_indpanel.h"
#include "w_layers.h"
#include "w_rulers.h"
#include "w_print.h"
#include "w_util.h"
#include "w_setup.h"
#include "e_compound.h"
#include "u_bound.h"
#include "u_draw.h"
#include "w_grid.h"
#else
#include "f_save.h"
#endif


void	read_fail_message(char *file, int err);
extern int num_object;

void
load_file(const char *file)
{
	int		s;
	F_compound	c;

	if (*file == 0)
	{
		put_msg((char *)_(msg_EmptyName));
		return;
	}
	c.parent = NULL;
	c.GABPtr = NULL;
	c.arcs = NULL;
	c.compounds = NULL;
	c.ellipses = NULL;
	c.lines = NULL;
	c.splines = NULL;
	c.texts = NULL;
	c.next = NULL;
	beginWait();
	s = read_fig((char *)file, &c, FALSE, 0, 0);
	if ((s == 0) || (s == ENOENT))
	{		/* Successful read */
		clean_up();
		(void)strcpy(current_file, file);
		saved_objects = fobjects;
		fobjects = c;
		extern void RefreshAllColorButtons(void);
		RefreshAllColorButtons();
		redisplay_canvas();
		set_action(F_EDIT);
		put_msg((char *)_(msg_DeletingPrevFromMem));
		clean_up();
		put_msg((char *)_(msg_EditObjects), file, num_object);
	}
	else
	{
		put_msg((char *)_(msg_DeletingPrevFromMem));
		clean_up();
		read_fail_message((char *)file, s);
	}

	stcgfp(DirectoryName, current_file);
	stcgfn(FileName, current_file);

	figure_modified=0;
	endWait();
}

int save_file(const char *file)
{
	int err = write_file((char *)file);

	stcgfp(DirectoryName, file);
	stcgfn(FileName, file);

	return(err);
}

void merge_file(char *file)
{
	F_compound	c;
	int		s;

	if (*file == 0)
	{
		put_msg((char *)_(msg_EmptyName));
		return;
	}

	c.arcs = NULL;
	c.compounds = NULL;
	c.ellipses = NULL;
	c.lines = NULL;
	c.splines = NULL;
	c.texts = NULL;
	c.next = NULL;
	set_temp_cursor(wait_cursor);
	s = read_fig(file, &c, TRUE, 0, 0);
	if (s == 0)
	{		/* Successful read */
		clean_up();
		saved_objects = c;
		tail(&fobjects, &object_tails);
		append_objects(&fobjects, &saved_objects, &object_tails);
		extern void RefreshAllColorButtons(void);
		RefreshAllColorButtons();
		redisplay_canvas();
		put_msg((char *)_(msg_ObjectsInFile), file, num_object);
		set_action_object(F_ADD, O_ALL_OBJECT);
	}
	else if (s > 0)
		read_fail_message(file, s);
    reset_cursor();
}

int countObjects(F_compound *compound)
{
	F_arc		*a;
	F_ellipse	*e;
	F_compound	*c;
	F_spline	*s;
	F_line		*l;
	F_text		*t;
	int count=0;

	for(a = compound->arcs; a != NULL; a = a->next, count++);

	for(c = compound->compounds; c != NULL; c = c->next)
	{
		count += countObjects(c);
	}

	for(e = compound->ellipses; e != NULL; e = e->next, count++);

	for(l = compound->lines; l != NULL; l = l->next, count++);

	for(s = compound->splines; s != NULL; s = s->next, count++);

	for(t = compound->texts; t != NULL; t = t->next, count++);

	return(count);
}

void status_message(void)
{
	if (*current_file == '\0')
		put_msg((char *)_(msg_NoFile));
	else
		if (figure_modified)
			put_msg((char *)_(msg_ObjectsInModFile), current_file, countObjects(&fobjects));
		else
			put_msg((char *)_(msg_ObjectsInFile), current_file, countObjects(&fobjects));
}

void
read_fail_message(char *file, int err)
{
    if (err == 0)		/* Successful read */
	return;
#ifdef ENAMETOOLONG
    else if (err == ENAMETOOLONG)
	put_msg((char *)_(msg_FilenameIsTooLong), file);
#endif /* ENAMETOOLONG */
    else if (err == ENOENT)
	put_msg((char *)_(msg_FileNotExisting), file);
    else if (err == ENOTDIR)
	put_msg((char *)_(msg_NotADirectory), file);
    else if (err == EACCES)
	put_msg((char *)_(msg_ReadAccessBlocked), file);
    else if (err == EISDIR)
	put_msg((char *)_(msg_FileIsADirectory), file);
    else if (err == NO_VERSION)
	put_msg((char *)_(msg_FileHasNoVersionNumber), file);
    else if (err == EMPTY_FILE)
	put_msg((char *)_(msg_FileIsEmpty), file);
    else if (err == BAD_FORMAT)
	/* Format error; relevant error message is already delivered */
	;
    else
	put_msg((char *)_(msg_FileIsNotAccessible), file, strerror(err));
}
