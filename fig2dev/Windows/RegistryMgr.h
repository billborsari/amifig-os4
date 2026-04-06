#pragma once

#ifdef VSTUDIO

#include <QString>
#include <QList>

//#include <afxwin.h>         // MFC core and standard components
#include <Windows.h>
//typedef unsigned long       DWORD;

class CRegistryMgrEntry
{
public:
	CRegistryMgrEntry() : binValue(NULL), size(0)
	{
	}

	virtual ~CRegistryMgrEntry()
	{
//		if(binValue != NULL)
//			free(binValue);
	}

	CRegistryMgrEntry(const QString& name, const QString& value) : binValue(NULL), size(0)
	{
		setValue(name, value);
	}

	CRegistryMgrEntry(const QString& name, const long value) : binValue(NULL), size(0)
	{
		setValue(name, value);
	}

	CRegistryMgrEntry(const QString& name, const void *buffer, const int size) : binValue(NULL), size(0)
	{
		setValue(name, buffer, size);
	}

	QString getStringValue()
	{
		return(strValue);
	}

	DWORD getLongValue()
	{
		return(dwValue);
	}

	void *getBinaryValue()
	{
		return(binValue);
	}

	int getSize()
	{
		return(size);
	}

	void setValue(const QString& name, const QString& value)
	{
		this->type = REG_SZ;
		this->name = name;
		this->strValue = value;
		this->size = value.length();
	}

	void setValue(const QString& name, const long value)
	{
		this->type = REG_DWORD;
		this->name = name;
		this->dwValue = value;
		this->size = 4;
	}

	void setValue(const QString& name, const void *buffer, const int size)
	{
		this->type = REG_BINARY;
		this->name = name;
		if((this->binValue = malloc(size)) != NULL)
		{
			this->size = size;
			memcpy(this->binValue, buffer, size);
		}
	}

	QString name;
	DWORD type;

private:
	QString strValue;
	DWORD dwValue;
	void *binValue;
	int size;
};


class CRegistryMgr  
{
public:
	QString subkey;
	QList<CRegistryMgrEntry> keylist;
	bool valid;
	HKEY hRootKey;

	CRegistryMgr();
	virtual ~CRegistryMgr();
	CRegistryMgr(const HKEY hKey, const QString& subkey);

	void invalidate()
	{
		valid = false;
	}

	bool addEntry(CRegistryMgrEntry entry);

	bool saveAll(void);

	bool queryKey(const QString& key, QString *value);

	bool queryKey(const QString& key, DWORD *value);

	void refresh(void);

	void readAll(void);

	QStringList *enumerateKeys(const QString& key);

	QString getStringValue(const QString& key);

	DWORD getLongValue(const QString& key);

	void *getBinaryValue(const QString& key);

	void setValue(const QString& key, const QString& value);

	void setValue(const QString& key, const long value);

	void setValue(const QString& key, const void *buffer, const int size);

	void Dump();

	bool hasValue(const QString& name)
	{
		for(int i=0; i < keylist.size(); i++)
		{
			if(keylist[i].name == name)
				return true;
		}
		return false;
	}
};

#endif // VSTUDIO
