#ifndef W_MSGPANEL_H
#define W_MSGPANEL_H
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
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/w_msgpanel.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/ 
extern void beep(void);
extern void set_mousefun(char *LeftBut, char *MiddleBut, char *RightBut, char *S_LeftBut, char *S_MiddleBut, char*S_RightBut);
extern void draw_mousefun_canvas();
extern void doMessage(char *Msg, ...);
extern void put_msg(char *format, ...);
extern BOOL doRequest(char* message);

#ifndef AMIFIG
extern void	init_msg(void);
#endif
extern void	file_msg(char *format, ...);
extern void	boxsize_msg(int fact);
extern void	length_msg(int type);
extern void	altlength_msg(int type, int fx, int fy);
extern void	length_msg2(int x1, int y1, int x2, int y2, int x3, int y3);
extern void	popup_file_msg(void);
extern void	make_dimension_string(float length, char *str, BOOL square);

extern BOOL	popup_up;
extern BOOL	first_file_msg;
extern BOOL	file_msg_is_popped;
extern BOOL	first_lenmsg;

extern void boxsize_scale_msg (int fact);
extern void erase_box_lengths (void);
extern void erase_lengths (void);
extern void arc_msg (int x1, int y1, int x2, int y2, int x3, int y3);
extern void areameas_msg (char *msgtext, float area, float totarea, int flag);
extern void lenmeas_msg (char *msgtext, float len, float totlen);
#ifndef AMIFIG
extern void setup_msg(void);
#endif
#ifdef AMIFIG
APTR MakeMsgWindow(void);
void z_msg_methods(void);
#endif

#endif /* W_MSGPANEL_H */
