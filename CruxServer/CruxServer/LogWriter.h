﻿#pragma once

#include <string>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace std;

#pragma warning (disable: 4996)

class CLogWriterCruxServer
{

public:

	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const wchar_t* pszLogData,		///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > 4)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OK;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			wchar_t* cpLogBuffer = new wchar_t[nLogTextSize+50];
			memset(cpLogBuffer, 0, sizeof(cpLogBuffer));
			va_list vaList;
			va_start(vaList, pszLogData);
			vswprintf_s(cpLogBuffer, nLogTextSize+50, pszLogData, (va_list)vaList);
			va_end(vaList);
			nResultCode = m_fnWriteLog(cpLogBuffer);
			delete[] cpLogBuffer;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////

		

		return nResultCode;//OK;
	}
		
	int m_fnPrintLog(
		USHORT LogLevel,				///< 기록 할 Log 의 Log Level, 1차로 함수 내에서  Block을 결정하고, 2차로 Log Task 에서 Block 여부가 결정된다.
		int	nLogTextSize,				///< 로그를 기입할 내용의 사이즈
		const char* pszLogData,			///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		)
	{
		int				nResultCode = APP_OK;

		if(LogLevel > 4)//NetWork부하를 줄이기 위해서 Log Data자체를 보내지 않는다. 
		{
			return OK;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////
		{
			char* cpLogBuffer = new char[nLogTextSize+50];
			memset(cpLogBuffer, 0, sizeof(cpLogBuffer));
			va_list vaList;
			va_start(vaList, pszLogData);
			vsprintf_s(cpLogBuffer, nLogTextSize+50, pszLogData, (va_list)vaList);
			va_end(vaList);
			nResultCode = m_fnWriteLog(cpLogBuffer);
			delete[] cpLogBuffer;
		}
		/////////////////////////////////////////////////////////////////////////////////////////////
		
		//nResultCode = m_fnWriteLog(pszLogData);

		return nResultCode;//OK;
	}

	CLogWriterCruxServer(TSTRING szLogFilePath, TSTRING szLogFileName, int uPeriodDay, int uWriteTerm, bool bEnable = true)
	{
		m_szLogFilePath = szLogFilePath;
		m_szLogFileName = szLogFileName;
		m_uPeriodDay		= uPeriodDay;
		m_uWriteTerm		= uWriteTerm;
		m_bEnable			= bEnable;

		if(m_szLogFilePath[m_szLogFilePath.length()-1] != _T('\\'))
		{
			m_szLogFilePath += _T("\\");
		}

		m_szLastWriteFileName = _T("");
		m_ulFileNumber		= 0;
		m_hLastFileHandle	= NULL;


		::InitializeCriticalSection(&m_csLogWriter);


		if(APP_OK != m_fnMakePath())
		{

			_tprintf(_T("CLogWriterCruxServer::CLogWriterCruxServer -> Path make error\n"));
		}


		if(APP_OK != m_fnDeletePeriodOverFile())
		{
			_tprintf(_T("CLogWriterCruxServer::m_fnConnectFileHandle -> PeriodOverFile delete fail..\n"));
		}
	}
	~CLogWriterCruxServer(void)
	{

		if(NULL != m_hLastFileHandle)
		{
			fclose(m_hLastFileHandle);
		}
		::DeleteCriticalSection(&m_csLogWriter);
	}

public:

	int		m_fnWriteLog(const wchar_t* pszLog)
	{

		SYSTEMTIME systime;	

		::GetLocalTime(&systime);

		if(!m_bEnable)
		{
			wprintf(L"[%04d/%02d/%02d %02d:%02d:%02d] %s\n",systime.wYear, systime.wMonth, systime.wDay,
				systime.wHour, systime.wMinute, systime.wSecond, pszLog);
		}
		else
		{
			EnterCriticalSection(&m_csLogWriter);

			if(APP_OK != m_fnConnectFileHandle())
			{
				wprintf(L"CLogWriterCruxServer::m_fnWriteLog -> m_fnConnectFileHandle() process Fail..\n");
			}

			if(NULL != m_hLastFileHandle)
			{
				wprintf(L"[%04d/%02d/%02d %02d:%02d:%02d] %s\n",systime.wYear, systime.wMonth, systime.wDay,
					systime.wHour, systime.wMinute, systime.wSecond, pszLog);
				fwprintf(m_hLastFileHandle,L"[%04d/%02d/%02d %02d:%02d:%02d] %s\n",systime.wYear, systime.wMonth, systime.wDay,
					systime.wHour, systime.wMinute, systime.wSecond, pszLog);

				fclose(m_hLastFileHandle);
				m_hLastFileHandle = NULL;
			}
			else 
			{
				wprintf(L"CLogWriterCruxServer::m_fnWriteLog -> Log File Handle is NULL\n");
				wprintf(L"[%04d/%02d/%02d %02d:%02d:%02d] %s\n",systime.wYear, systime.wMonth, systime.wDay,
					systime.wHour, systime.wMinute, systime.wSecond, pszLog);
			}
			LeaveCriticalSection(&m_csLogWriter);
		}

		return	APP_OK;
	}

	int		m_fnWriteLog(const char* pszLog)
	{
		wchar_t* pStr;

		//멀티 바이트 크기 계산 길이 반환
		int strSize = MultiByteToWideChar(CP_ACP, 0,pszLog, -1, NULL, NULL);
		//wchar_t 메모리 할당

		pStr = new WCHAR[strSize];
		memset(pStr,0,sizeof(pStr));

		//형 변환

		MultiByteToWideChar(CP_ACP, 0,pszLog, strlen(pszLog)+1, pStr, strSize);

		m_fnWriteLog(pStr);
	
		delete[] pStr;

		return	APP_OK;
	}

	int		m_fnWriteLogVar(const wchar_t* pszLog, ...)
	{
		wchar_t* pStr;
		int strSize = wcslen(pszLog);

		pStr = new wchar_t[strSize];
		memset(pStr,0,sizeof(pStr));

		va_list vaList;
		va_start(vaList, pszLog);
		vswprintf(pStr, pszLog, (va_list)vaList);
		va_end(vaList);

		m_fnWriteLog(pStr);

		delete[] pStr;

		return APP_OK;
	}

	int		m_fnWriteLogVar(const char* pszLog, ...)
	{
		char* pStr;
		int strSize = strlen(pszLog);

		pStr = new char[strSize];
		memset(pStr,0,sizeof(pStr));

		va_list vaList;
		va_start(vaList, pszLog);
		vsprintf(pStr, pszLog, (va_list)vaList);
		va_end(vaList);

		m_fnWriteLog(pStr);

		delete[] pStr;

		return APP_OK;
	}

private:
	int		m_fnConnectFileHandle()
	{
		SYSTEMTIME	systime;
		TSTRING		szDIRFileName;
		int			nHour = -1;
		ULONG		ulNewFileNumber;
		TCHAR		tempString[8000] = {0,};

		::GetLocalTime(&systime);
		if(24 <= m_uWriteTerm || 0 >= m_uWriteTerm)
		{
			ulNewFileNumber	= systime.wYear*10000 + systime.wMonth*100 + systime.wDay;
		}
		else
		{
			nHour			= (systime.wHour / m_uWriteTerm) * m_uWriteTerm;
			ulNewFileNumber	= systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nHour;	
		}

		if(NULL != m_hLastFileHandle)
		{
			fclose(m_hLastFileHandle);
			m_hLastFileHandle = NULL;
		}

		if(0 <= nHour)
		{
			_stprintf(tempString, _T("%s_%04d%02d%02d%02d.Log"), m_szLogFileName.c_str(), systime.wYear, systime.wMonth, systime.wDay, nHour);
			m_szLastWriteFileName = tempString;
		}
		else
		{
			_stprintf(tempString, _T("%s_%04d%02d%02d.Log"), m_szLogFileName.c_str(), systime.wYear, systime.wMonth, systime.wDay);
			m_szLastWriteFileName = tempString;
		}

		szDIRFileName = m_szLogFilePath + m_szLastWriteFileName;

		//File 유무 체크
		m_hLastFileHandle = _tfopen(szDIRFileName.c_str(), _T("r"));
		//파일이 없으면
		if(NULL == m_hLastFileHandle)
		{
			//파일을 기록 모드로 연다.
			m_hLastFileHandle = _tfopen(szDIRFileName.c_str(), _T("wt, ccs=UNICODE"));
			if(NULL == m_hLastFileHandle)
			{
				_tprintf(_T("CLogWriterCruxServer::m_fnConnectFileHandle -> Logfile Create fail..\n"));
				return -1;
			}
		}
		//파일이 있으면
		else
		{
			fclose(m_hLastFileHandle);
			m_hLastFileHandle = NULL;
			//파일을 추가 기록 모드로 연다.
			m_hLastFileHandle = _tfopen(szDIRFileName.c_str(), _T("at, ccs=UNICODE"));

			if(NULL == m_hLastFileHandle)
			{
				_tprintf(_T("CLogWriterCruxServer::m_fnConnectFileHandle -> Logfile Create fail..\n"));
				return -1;
			}
		}

		m_ulFileNumber = ulNewFileNumber;

		return APP_OK;
	}
	int		m_fnMakePath()
	{
		int	nRet;

		if(m_szLogFilePath[m_szLogFilePath.length()-1] != _T('\\'))
		{
			m_szLogFilePath += _T("\\");
		}

		nRet = m_fnCreateDir(m_szLogFilePath.c_str());

		return nRet;
	}
	int		m_fnDeletePeriodOverFile()
	{
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		if(0 >= m_uPeriodDay)
		{
			return nErrorCode;
		}

		cLogFileName = m_szLogFilePath + m_szLogFileName + _T("*.log");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
			return nErrorCode;
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)m_uPeriodDay)
			{
				cDeleteLogFileName = m_szLogFilePath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)
				{

					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriterCruxServer::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);

		return nErrorCode;
	}
	int		m_fnCreateDir(const TCHAR* pszDir)
	{
		int nRet = APP_OK;
		BOOL bRet = true;
		int nLen = _tcslen(pszDir);
		TCHAR* pszSubDir = NULL;
		_tfinddata_t fdata;
		long hFind;

		for (int i = nLen - 1; i >= 0; i--)
		{
			if (pszDir[i] == _T('\\'))
			{
				pszSubDir = new TCHAR[i+1];
				memset(pszSubDir, 0, (i*sizeof(TCHAR))+sizeof(TCHAR));
				memcpy(pszSubDir, pszDir, i*sizeof(TCHAR));
				if (hFind = _tfindfirst(pszSubDir, &fdata) == -1L)
				{
					if (m_fnCreateDir(pszSubDir) == APP_NG)	
					{
						nRet = APP_NG;
						delete pszSubDir;	
						return nRet;
					}
				}
				delete pszSubDir;
				break;
			}
		}	
		bRet = ::CreateDirectory(pszDir, NULL);
		if(bRet == FALSE)
			nRet = APP_OK;
		else
			nRet = APP_OK;
		return nRet;
	}

private:
	TSTRING	m_szLogFilePath;
	TSTRING	m_szLogFileName;
	int		m_uPeriodDay;
	int		m_uWriteTerm;
	TSTRING	m_szLastWriteFileName;
	FILE*	m_hLastFileHandle;
	CRITICAL_SECTION m_csLogWriter;
	ULONG	m_ulFileNumber;
	bool	m_bEnable;
};
