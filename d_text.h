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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/d_text.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef AMIFIG
extern void	char_handler(XKeyEvent *kpe, unsigned char c, KeySym keysym);
extern void	draw_char_string(void);
extern void	erase_char_string(void);
extern void	finish_text_input(int x, int y, int shift);
extern void	reload_text_fstruct(F_text *t);
extern void	reload_text_fstructs(void);
extern BOOL	text_selection_active;
extern BOOL	ConvertSelection();
extern void	LoseSelection(), TransferSelectionDone();
extern int	work_font;

#ifdef I18N
extern XIC	xim_ic;
extern BOOL	xim_active;
extern		i18n_char_handler();
extern		prefix_append_char();
#ifdef I18N_USE_PREEDIT
extern		kill_preedit();
#endif  /* I18N_USE_PREEDIT */
#endif  /* I18N */
#endif
extern void text_drawing_selected (void);


#ifdef AMIFIG
extern void init_text_input(int x, int y);
extern F_text *new_text(int x, int y, char *thetext);
#endif
