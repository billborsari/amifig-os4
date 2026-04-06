/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/menu.h $
 *       $Revision: 336 $
 *       $Date: 2016-01-30 17:51:39 +0000 (Sat, 30 Jan 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/

/* Menu definition */

/* Project */
#define CLEAR	            0
#define NEW		            10
#define OPEN                20
#define INSERT              30
#define IMPORTF	            40
	/* subitems of IMPORT */
	#define IMPPOTRACE          41
	#define IMPSVG	            42
#define SAVE                50
#define SAVEAS              60
#define EXPORT              70
	/* subitems of EXPORT */
	#define EXPORTPNG           71
	#define EXPORTF2D           72
#define PRINT	            80
#define PRINTPS	            90
#define STATUS              100
#define ABOUT               110
#define SAVEQUIT            120
#define QUIT                130
	
/* Edit */
#define TOGGLEZOOM          200
#define TOGGLECOLOR         210
#define TOGGLELIB           220
#define TOGGLEHELP			230
#define UNDO                270
#define REDISP              280

/* Libraries */

/* Settings */
#define EDITPREFS           400

