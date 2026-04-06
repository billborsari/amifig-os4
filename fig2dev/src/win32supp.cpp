//#include <afx.h>

// I have no idea what this "dev" is and where it is defined, but it conflicts with definitions in qdatastream.h
#if defined(dev)
#undef dev
#endif

#include <stdlib.h>
#include <string.h>
#ifndef AMIGA
#include <QString>
#include <QStringList>
#include <QProcess>
#endif

#ifdef WIN32
#include "RegistryMgr.h"
#endif

typedef int BOOL;

extern "C" {

#include "win32supp.h"

void stcgfp(char *directoryName, char *fullpath)
{
	int i;

	if(directoryName == NULL)
		return;

	if(fullpath == NULL || strlen(fullpath) == 0)
	{
		directoryName[0] = 0x00;
		return;
	}

	// find the last '\'
	for(i=strlen(fullpath)-1; i >= 0; i--)
	{
		if(fullpath[i] == '\\')
		{
			break;
		}
	}

	if(i == -1)
	{
		directoryName[0] = 0x00;
		return;
	}

	strncpy(directoryName, fullpath, i);
	directoryName[i] = 0x00;
}

void stcgfn(char *fileName, char *fullpath)
{
	int i, j;

	if(fileName == NULL)
		return;

	if(fullpath == NULL || strlen(fullpath) == 0)
	{
		fileName[0] = 0x00;
		return;
	}

	// find the last '\'
	for(i=strlen(fullpath)-1; i >= 0; i--)
	{
		if(fullpath[i] == '\\')
		{
			break;
		}
	}

	// no \ found => take full path as filename
	if(i == -1)
	{
		strcpy(fileName, fullpath);
		return;
	}

	i++;

	for(j=0; i < (signed int) strlen(fullpath); i++, j++)
	{
		fileName[j] = fullpath[i];
	}

	fileName[j] = 0x00;
}

void stcgfe(char *ext, char *fullpath)
{
	int i, j;

	if(ext == NULL)
		return;

	if(fullpath == NULL || strlen(fullpath) == 0)
	{
		ext[0] = 0x00;
		return;
	}

	// find the last '.'
	for(i=strlen(fullpath)-1; i >= 0; i--)
	{
		if(fullpath[i] == '.')
		{
			break;
		}
	}

	// not found
	if(i == -1)
	{
		ext[0] = 0x00;
		return;
	}

	i++;

	for(j=0; i < (signed int) strlen(fullpath); i++, j++)
	{
		ext[j] = fullpath[i];
	}

	ext[j] = 0x00;
}

// returns full path without extension
void stcgf(char *path, char *fullpath)
{
	int i;

	if(path == NULL)
		return;

	if(fullpath == NULL || strlen(fullpath) == 0)
	{
		path[0] = 0x00;
		return;
	}

	// find the last '.'
	for(i=strlen(fullpath)-1; i >= 0; i--)
	{
		if(fullpath[i] == '.')
		{
			break;
		}
	}

	if(i == -1)
	{
		path[0] = 0x00;
		return;
	}

	strncpy(path, fullpath, i);
	path[i] = 0x00;
}

void strmfp(char *fullpath, char *path, char *fileName)
{
	int i, j;
	int pathlen = strlen(path);

	if(fullpath == NULL || path == NULL || fileName == NULL)
	{
		fullpath[0] = 0x00;
		return;
	}

	if(pathlen > 0)
	{
		if(path[pathlen-1] == '\\')
			strncpy(fullpath, path, pathlen-1);
		else
			strncpy(fullpath, path, pathlen);
	}

	for(i=0, j=pathlen; i < strlen(fileName); i++, j++)
	{
		fullpath[j] = fileName[i];
	}

	fullpath[j] = 0x00;
}

#ifndef AMIGA
BOOL executeCommand(char *command)
{
	QProcess process;
	bool success = false;
	QString message;
	QString completeCommand = command;

	process.start(completeCommand);

	success = process.waitForStarted();

	if(!success)
	{
		int error = process.error();
//		qDebug("doExport: error=%d\n", error);
		message = QString("%1 failed with error code %2").arg(completeCommand).arg(error);
	}

	if(success)
	{
		success = process.waitForFinished();
	}

	return success;
}
#endif

#ifdef WIN32
static struct passwd who;

struct passwd *getpwuid()
{
	DWORD userIDbufferSize = 256;
	who.pw_name[0] = 0;
	who.pw_gecos[0] = 0;

	if(!GetUserName(who.pw_name, &userIDbufferSize))
	{
		who.pw_name[0] = 0;
		return NULL;
	}

	return(&who);
}

void winSleep(unsigned long dwMilliseconds)
{
	Sleep(dwMilliseconds);
}

int winGethostname(char *name, unsigned long namelen)
{
	BOOL result = GetComputerName(name, &namelen);
	return(result? 0 : -1);
/*
	int err = gethostname(name, namelen);

	if(err == SOCKET_ERROR)
		printf("gethostname %d\n", WSAGetLastError());

	return(err == 0? 0 : -1);
*/}

char ghostScriptPath[1024] = "";

void getGhostScriptPath(void)
{
	QString ghostScriptKey = "SOFTWARE\\AFPL Ghostscript";
	CRegistryMgr gsRegistryMgr(HKEY_LOCAL_MACHINE, ghostScriptKey);
	QStringList *keylist = gsRegistryMgr.enumerateKeys(NULL);

	if(keylist == NULL)
	{
		ghostScriptKey = "SOFTWARE\\GPL Ghostscript";
		gsRegistryMgr.subkey = ghostScriptKey;
		keylist = gsRegistryMgr.enumerateKeys(NULL);

		if(keylist == NULL)
			return;
	}

	if(keylist == NULL)
		return;

	QString gsVersion, path;

	if(keylist->size() > 0)
	{
		gsVersion = keylist->last();

		CRegistryMgr gsRegistryMgr2(HKEY_LOCAL_MACHINE, ghostScriptKey+"\\"+gsVersion);
		CRegistryMgrEntry strGSDLL("GS_DLL", "");
		gsRegistryMgr2.addEntry(strGSDLL);
		gsRegistryMgr2.readAll();

		path = gsRegistryMgr2.getStringValue("GS_DLL");
		int index = path.indexOf("\\bin\\");

		if(index != -1)
		{
			path = path.left(index+5);
		}
	}

//	path.Replace("\\\\", "\\");

	strcpy(ghostScriptPath, path.toLatin1().data());

	delete keylist;
}

#endif

}
