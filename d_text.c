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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/d_text.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
#include "fig.h"
#include "z_fig.h"

#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "d_text.h"
#include "u_fonts.h"
#include "u_create.h"
#include "u_list.h"
#include "u_redraw.h"
#include "u_search.h"
#include "w_canvas.h"
#include "w_cursor.h"
#include "w_msgpanel.h"

void
text_drawing_selected(void)
{
    canvas_locmove_proc     = null_proc;
    canvas_middlebut_proc   = null_proc;
    canvas_leftbut_proc     = init_text_input;
    canvas_rightbut_proc    = null_proc;
    set_mousefun((char *)_(msg_position_cursor), "", "", "", "", "");
	set_cursor(arrow_cursor);
}

void
init_text_input(int x, int y)
{
	int dummy1;
	char buffer[256];
    buffer[0] = '\0';
	ULONG ret;
	int base_x = 0, base_y = 0;

	cur_x = x;
	cur_y = y;

	beginWait();
	
	if((cur_t = text_search(cur_x, cur_y, &dummy1)) == NULL)
	{
		base_x = cur_x;
		base_y = cur_y;
	}
	else
	{
		base_x = cur_t->base_x;
		base_y = cur_t->base_y;
		strcpy(buffer, cur_t->cstring);
	}	

	ret = GetStringFromUser(
			(UBYTE *)buffer,
			256,
			(char *)"Enter text");
			
	if (ret)
	{	
		int len;

		len=strlen(buffer);

		if((cur_t == NULL) && (len !=0))  	/* a brand new text */
		{
			cur_t = new_text(base_x, base_y, buffer);
			add_text(cur_t);
		}
		else if (cur_t != NULL)				/* existing text modified */
		{
			if(strcmp(cur_t->cstring, buffer) != 0)
			{
				delete_text(cur_t);

				if(len != 0)
				{	
					cur_t = new_text(base_x, base_y, buffer);
					add_text(cur_t);
				}
			}
		}

		cur_t = NULL;
        redisplay_canvas();
	}

	endWait();
}

F_text *
new_text(int x, int y, char *thetext)
{
	F_text		*text;

	if((text=create_text()) == NULL)
		return(NULL);

	text->cstring = malloc(strlen(thetext)+1);
	text->type = cur_textjust;
	text->font = (cur_textflags & PSFONT_TEXT)? cur_ps_font:cur_latex_font;
	text->angle = (float)cur_textangle/180.0*M_PI;
	text->color = cur_pencolor;
	text->depth = cur_depth;
	text->size = cur_fontsize;
	strcpy(text->cstring, thetext);
	text->base_x = x;
	text->base_y = y;
	text->next = NULL;
	text->flags = cur_textflags;
	text->pen_style = 0;
    
    /* Set size information */
	SetCanvasFont(canvas_win, text);
	return(text);
}
