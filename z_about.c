/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/z_about.c $
 *       $Revision: 345 $
 *       $Date: 2017-04-06 19:17:34 +0000 (Thu, 06 Apr 2017) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/*----------------------------------------------------------------------------*/
/*     Includes & Defines                                                     */
/*----------------------------------------------------------------------------*/
#include "fig.h"
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include "intui.h"
#include "z_fig.h"
#include "z_about.h"
#include "version.h"


/*----------------------------------------------------------------------------*/
/*     Local prototypes                                                       */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*     Global Variables                                                       */
/*----------------------------------------------------------------------------*/
    
#include "a_rexxiface.h"

APTR    BUT_ABOUT_CLOSE, LV_ABOUT;
static char about_text[4096];
static const char text[] = \
"AmiFIG " AMIFIG_VERSION "." AMIFIG_COMMIT " (" AMIFIG_DATE ") \n\
(C)2010-2016 Yannick Erb\n\
\n\
Build date: %s %s\n\
\n\
ARexx Port: %s\n\
\n\
Open Source version (Aros Public License)\n\
\n\
visit us @ http://sourceforge.net/projects/amifig/\n\
\n\
Based on AmiFIG 3.1 (C) Andreas Schmidt\n\
\n\
This source is copyright the original authors and based on Zunefig.\n\
\n\
Contributors :\n\
Saimon69 (button images)\n\
Frank Ruthe (button images)\n\
Mathias Rustler\n\
Kas1e (AmigaOS4 port)\n\
Xenic (AmigaOS4 port)\n\
Polluks (MorphOS port)\n\
Samir Hawamdeh (Italian Catalog)\n\
Manu (button images, icons, testing)\n\
Tomasz Paul (Polish Catalog)\n\
\n\
Polygon fill algorithm used for gradient fill based on\n\
public-domain code by Darel Rex Finley, 2007\n\
\n\
Based on XFIG\n\
Original Copyright (c) 1985 by Supoj Sutanthavibul\n\
Parts Copyright (c) 1994-1999 by Brian V. Smith\n\
Parts Copyright (c) 1991 by Paul King\n\
Other Copyrights may be found in various files\n\
\n\
Original Xfig copyright notice:\n\
Any party obtaining a copy of these files is granted, free of charge,\n\
a full and unrestricted irrevocable, world-wide, paid up, royalty-free,\n\
nonexclusive right and license to deal in this software and documentation\n\
files (the \"Software\"), including without limitation the rights to use,\n\
copy, modify, merge, publish and/or distribute copies of the Software,\n\
and to permit persons who receive copies from any such party to do so,\n\
with the only requirement being that this copyright notice remain intact.\n\
\n\
No representations are made about the suitability\n\
of this software for any purpose.\n\
It is provided \"as is\" without express or implied warranty.\n\
\n\
# Parts Copyright (C) 1993 by Alan Richardson (mppa3@uk.ac.sussex.syma)\n\
The text rotation code in w_rottext.c was written by Alan Richardson.\n\
The above copyright notice holds for this work as well.\n\
\n\
# Parts Copyright (c) 1994 by Anthony Dekker\n\
The Kohonen neural network code for color optimization\n\
was written by Anthony Dekker.\n\
\n\
# Parts Copyright (c) 1995 by C. Blanc and C. Schlick\n\
The X-Spline code was written Carole Blanc (blanc@labri.u-bordeaux.fr)\n\
and Christophe Schlick (schlick@labri.u-bordeaux.fr) starting from an\n\
initial implementation done by\n\
C. Feuille, S. Grosbois, L. Maziere and L. Minihot\n\
as a student practice (Universite Bordeaux, France).\n\
For additional information about X-splines, see:\n\
X-Splines: A Spline Model Designed for the End User\n\
by C. Blanc and C. Schlick, Proceedings of SIGGRAPH'95\n\
http://dept-info.labri.u-bordeaux.fr/~schlick/DOC/sig1.html\n";
     
/*----------------------------------------------------------------------------*/
/*     MakeAboutWindow - creates the about window                             */
/*----------------------------------------------------------------------------*/

APTR MakeAboutWindow(void)
{
    sprintf(about_text, text, __DATE__, __TIME__, portname);

    return(WindowObject,
            MUIA_Window_Title, "AmiFig : About",
            MUIA_Window_ID, MAKEID('Z','F','A','B'),
            MUIA_Window_Activate, TRUE,
#ifdef __AROS__
            MUIA_Window_ActiveObject, LV_ABOUT,
#endif
            MUIA_Window_Screen, Scr,
			MUIA_Window_ScreenTitle, "AmiFIG",

            WindowContents,	VGroup,
				Child, HGroup,
					Child, RectangleObject, End,
					Child, MUI_NewObject(   "Dtpic.mui",
											MUIA_Dtpic_Name,"logo.png",
											TAG_DONE),
					Child, RectangleObject, End,					
				End,
				Child, LV_ABOUT = ListviewObject, 
					MUIA_Listview_Input, TRUE, 
                    MUIA_Listview_ScrollerPos, MUIV_Listview_ScrollerPos_Right,
					MUIA_Listview_List, FloattextObject, 
						MUIA_Frame, MUIV_Frame_ReadList,
						MUIA_Background, MUII_ReadListBack,
						MUIA_List_Format, "P=\33c",
						MUIA_Floattext_Text, about_text,
						MUIA_Floattext_TabSize, 4,
						MUIA_Floattext_Justify, TRUE,
					End,
				End,
				Child, HGroup,
					Child, RectangleObject, End,
					Child, BUT_ABOUT_CLOSE = MakeButton((UBYTE *)_(msg_Close), '\0', (UBYTE *)""),					
				End,
            End,
        End);
}

void z_about_methods(void)
{
    /* Close window */
    DoMethod(WinAbout, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
        WinAbout, 3, MUIM_Set, MUIA_Window_Open, FALSE);
	
	/* Close Button */
    DoMethod(BUT_ABOUT_CLOSE, MUIM_Notify, MUIA_Selected, FALSE,
        WinAbout, 3, MUIM_Set, MUIA_Window_Open, FALSE);
}
