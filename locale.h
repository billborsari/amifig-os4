/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/locale.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

#ifndef _LOCALE_H_
#define _LOCALE_H_

#include <exec/types.h>

#ifndef IPTR
typedef unsigned long IPTR;
#endif

#define CATCOMP_NUMBERS
#include "strings.h"

/*** Prototypes *************************************************************/
/* Main *********************************************************************/
CONST_STRPTR _(ULONG ID);       /* Get a message, as a STRPTR */
#define __(id) ((IPTR) _(id))   /* Get a message, as an IPTR */

/* Setup ********************************************************************/
BOOL Locale_Initialize(VOID);
VOID Locale_Deinitialize(VOID);

#endif /* _LOCALE_H_ */
