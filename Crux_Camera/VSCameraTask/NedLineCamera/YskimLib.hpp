#pragma once

#include "define.h"
#include <map>
#include <mutex>

class YskimLib
{
public:
	static YskimLib& Instance()
	{
		static YskimLib* _instance;
		static std::mutex _lock;

		if (_instance == NULL)
		{
			_lock.lock();

			if (_instance == NULL)
				_instance = new YskimLib();

			_lock.unlock();
		}

		return *_instance;
	}

#pragma region 프로그램 내 데이터 공유 기능
	void SetData(CString key, CString data)
	{
		dicAccessLock.lock();
		dic[key] = data;
		dicAccessLock.unlock();
	}
	CString GetData(CString key)
	{
		if (dic.count(key))
		{
			return dic[key];
		}
		else
		{
			return _T("");
		}
	}
#pragma endregion
	
#pragma region INI 접근 기능
	CString GetDataFromINI(CString section, CString key, CString file)
	{
		TCHAR sz[1000];
		GetPrivateProfileString(section, key, _T("0xDEADBEEF"), sz, sizeof(sz), file);
		return (LPCTSTR)sz;
	}
	bool GetBoolFromINI(CString section, CString key, CString file)
	{
		TCHAR sz[10];
		GetPrivateProfileString(section, key, _T("0xDEADBEEF"), sz, sizeof(sz), file);
		return _tcscmp(sz, _T("T")) == 0;
	}
	void SetDataToINI(CString section, CString key, CString data, CString file)
	{
		WritePrivateProfileString(section, key, data, file);
	}
	void SetDataToINI(CString section, CString key, double data, CString file)
	{
		CString str;
		str.Format(_T("%f"), data);
		WritePrivateProfileString(section, key, str, file);
	}
	void SetDataToINI(CString section, CString key, int data, CString file)
	{
		CString str;
		str.Format(_T("%d"), data);
		WritePrivateProfileString(section, key, str, file);
	}
	void SetDataToINI(CString section, CString key, bool data, CString file)
	{
		CString str = data ? _T("T") : _T("F");
		WritePrivateProfileString(section, key, str, file);
	}
#pragma endregion

private:
	YskimLib() {}
	std::mutex dicAccessLock;
	std::map<CString, CString> dic;
};