// RegistryMgr.cpp: implementation of the CRegistryMgr class.
//
//////////////////////////////////////////////////////////////////////

#ifdef VSTUDIO

#include "StdAfx.h"

// documented out for VisualStudio 2005
//#include <iostream.h>
#include <QStringList>
#include "RegistryMgr.h"
//#include "setDebugNew.h"

CRegistryMgr::CRegistryMgr() : valid(false)
{
}

CRegistryMgr::~CRegistryMgr()
{
	this->hRootKey = HKEY_LOCAL_MACHINE;
}

CRegistryMgr::CRegistryMgr(const HKEY hKey, const QString& subkey) : valid(false)
{
	this->hRootKey = hKey;
	this->subkey = subkey;
}

bool CRegistryMgr::addEntry(CRegistryMgrEntry entry)
{
	keylist.append(entry);

	return(true);
}

bool CRegistryMgr::saveAll(void)
{
	HKEY hKey;
	DWORD disposition;
	CRegistryMgrEntry keyEntry;
	DWORD lData;
	void *binData;

	for(int i=0; i < keylist.size(); i++)
	{
		keyEntry = keylist[i];

		if(RegCreateKeyEx(hRootKey, subkey.toLatin1().data(), NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &disposition) != ERROR_SUCCESS)
		{
			return(false);
		}

		switch(keyEntry.type)
		{
		case REG_SZ:
			{
				QString strData = keyEntry.getStringValue();

				if(RegSetValueEx(hKey, keyEntry.name.toLatin1().data(), NULL, keyEntry.type, 
					(unsigned char *)strData.toLatin1().data(), keyEntry.getSize()) != ERROR_SUCCESS)
				{
					return(false);
				}
				break;
			}
		case REG_DWORD:
			lData = keyEntry.getLongValue();

			if(RegSetValueEx(hKey, keyEntry.name.toLatin1().data(), NULL, keyEntry.type, 
					(unsigned char *)&lData, keyEntry.getSize()) != ERROR_SUCCESS)
			{
				return(false);
			}
			break;
		case REG_BINARY:
			binData = keyEntry.getBinaryValue();

			if(RegSetValueEx(hKey, keyEntry.name.toLatin1().data(), NULL, keyEntry.type, 
					(unsigned char *)binData, keyEntry.getSize()) != ERROR_SUCCESS)
			{
				return(false);
			}
			break;
		}
	}

	return(true);
}

bool CRegistryMgr::queryKey(const QString& key, QString *value)
{
	HKEY hkResult;
	DWORD type;
	BYTE valueBuffer[256];
//	BYTE valueBuffer[256];
	DWORD len = sizeof(valueBuffer);

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey.toLatin1().data(), 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
		return(false);

	long result;

	if((result = RegQueryValueExA(hkResult, key.toLatin1().data(), NULL, &type, valueBuffer, &len)) != ERROR_SUCCESS)
	{
//		TRACE("RegQueryValueEx returned %d", result);
		return(false);
	}

	*value = QString::fromLatin1((char *)valueBuffer);

	return(true);
}

bool CRegistryMgr::queryKey(const QString& key, DWORD *value)
{
	HKEY hkResult;
	DWORD type;
	DWORD len = sizeof(DWORD);

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey.toLatin1().data(), 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
		return(false);

	long result;

	if((result = RegQueryValueEx(hkResult, key.toLatin1().data(), NULL, &type, (BYTE *)value, &len)) != ERROR_SUCCESS)
	{
//		TRACE("RegQueryValueEx returned %d", result);
		return(false);
	}

	return(true);
}

void CRegistryMgr::readAll(void)
{
	QString value; 
	DWORD dwordValue;

	for(int i=0; i < keylist.size(); i++)
	{
		switch(keylist[i].type)
		{
		case REG_SZ:
			if(queryKey(keylist[i].name.toLatin1().data(), &value))
				keylist[i].setValue(keylist[i].name, value);
			break;
		case REG_DWORD:
			if(queryKey(keylist[i].name.toLatin1().data(), &dwordValue))
				keylist[i].setValue(keylist[i].name, dwordValue);
			break;
		case REG_BINARY:
			if(queryKey(keylist[i].name.toLatin1().data(), &value))
				keylist[i].setValue(keylist[i].name, value);
			break;
		}
	}

	valid = true;
}

QStringList *CRegistryMgr::enumerateKeys(const QString& key)
{
	HKEY hkResult;
	char valueBuffer[256];
	DWORD len = sizeof(valueBuffer);
	char classBuffer[256];
	DWORD classlen = sizeof(classBuffer);
	FILETIME filetime;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, subkey.toLatin1().data(), 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
		return NULL;

	DWORD index = 0;
	LONG result = 0;

	QStringList *keylist = new QStringList();

	for(index =0 ; /*index == 0 || */result != ERROR_NO_MORE_ITEMS; index++)
	{
		len = sizeof(valueBuffer);
		classlen = sizeof(classBuffer);

		if((result = RegEnumKeyEx(hkResult, index, valueBuffer, &len, NULL, classBuffer, &classlen, &filetime)) != ERROR_SUCCESS)
			return keylist;

		keylist->append(valueBuffer);
//		TRACE("%s %s %d\n", valueBuffer, classBuffer, index);
	}

	return keylist;
}

void CRegistryMgr::refresh(void)
{
	QString value; 

	for(int i=0; i < keylist.size(); i++)
	{
		if(queryKey(keylist[i].name, &value))
			keylist[i].setValue(keylist[i].name, value);
	}

	valid = true;
}

QString CRegistryMgr::getStringValue(const QString& key)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
	{
//			TRACE("%s %s\n", keylist[i].name, key);
		if(keylist[i].name == key)
		{
			return(keylist[i].getStringValue());
		}
	}

	return("");
}

DWORD CRegistryMgr::getLongValue(const QString& key)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
		if(keylist[i].name == key)
			return(keylist[i].getLongValue());

	return 0;
}

void *CRegistryMgr::getBinaryValue(const QString& key)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
		if(keylist[i].name == key)
			return(keylist[i].getBinaryValue());

	return NULL;
}

void CRegistryMgr::setValue(const QString& key, const QString& value)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
		if(keylist[i].name == key)
			keylist[i].setValue(key, value);
}

void CRegistryMgr::setValue(const QString& key, const long value)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
		if(keylist[i].name == key)
			keylist[i].setValue(key, value);
}

void CRegistryMgr::setValue(const QString& key, const void *buffer, const int size)
{
	if(!valid)
		readAll();

	for(int i=0; i < keylist.size(); i++)
		if(keylist[i].name == key)
			keylist[i].setValue(key, buffer, size);
}

void CRegistryMgr::Dump()
{
//	TRACE("Registry dump:\n");

	for(int i=0; i < keylist.size(); i++)
	{
//		TRACE("%s %s\n", keylist[i].name, keylist[i].getStringValue());
	}
}

#endif // VSTUDIO
