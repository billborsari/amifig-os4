#pragma once

void stcgfp(char *directoryName, char *fullpath);
void stcgfn(char *fileName, char *fullpath);
void stcgfe(char *ext, char *fullpath);
void stcgf(char *path, char *fullpath);
void strmfp(char *fullpath, char *path, char *fileName);

#ifdef WIN32

struct passwd{
	char pw_name[256];
	char pw_gecos[256];
};

struct passwd *getpwuid();
void winSleep(unsigned long dwMilliseconds);
int winGethostname(char *name, unsigned long namelen);
void getGhostScriptPath(void);

#endif

#ifndef AMIGA
BOOL executeCommand(char *command);
#endif

#ifdef WIN32
extern char ghostScriptPath[1024];
#endif
