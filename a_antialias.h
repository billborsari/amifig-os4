/*
 *       AmiFIG
 *
 *       $HeadURL: file:///svn/p/amifig/code/trunk/src/a_antialias.h $
 *       $Revision: 338 $
 *       $Date: 2016-04-23 08:30:23 +0000 (Sat, 23 Apr 2016) $
 *       $Author: yannickaros $
 *
 *       AROS Public License
*/
void CleanUp_AAbuffers();
extern BOOL setup_aa_rendering(int x, int y, int width, int height);
extern void perform_antialising(int x, int y, int width, int height);

