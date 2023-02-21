#pragma once

#include "define.h"
#include "stdafx.h"
#include <mutex>

#define GET_NAME(n) #n

class DalsaLineParam
{
public:
	static DalsaLineParam& Instance()
	{
		static DalsaLineParam* _instance;
		static std::mutex _lock;

		if (_instance == NULL)
		{
			_lock.lock();

			if (_instance == NULL)
				_instance = new DalsaLineParam();

			_lock.unlock();
		}

		return *_instance;
	}

	void SetParams(CString message)
	{
		for (int i = 0; ; i++)
		{
			CString line;
			CString name, value;
			if (AfxExtractSubString(line, message, i, '\r'))
			{
				line.Replace('>', ' ');
				line = line.Trim();
				if (line.Find('=', 0) >= 0) 
				{
					AfxExtractSubString(name, line, 0, '=');
					AfxExtractSubString(value, line, 1, '=');
					params[name.Trim()] = value.Trim();
				}
				else
				{
					for (int i = 0; i < line.GetLength(); i++) 
					{
						if (isdigit(line[i]) || '-' == line[i])
						{
							name = line.Left(i);
							value = line.Right(line.GetLength() - i);
							params[name.Trim()] = value.Trim();
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	CString GetParamStr(CString key) 
	{
		if (params.count(key))
		{
			return params[key];
		}
		else
		{
			return _T("");
		}
	}

	int GetParamInt(CString key) 
	{
		if (params.count(key))
		{
			return _ttoi(params[key]);
		}
		else
		{
			return -1;
		}
	}

private:
	DalsaLineParam() {}
	std::map<CString, CString> params;
};