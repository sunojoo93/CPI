#include "StdAfx.h"
#include "logwriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	Class 생성자
*	Parameter	:	Log 파일 경로
*					파일 이름
*					보관일
*					기록시간단위
*	Return		:	없음
*	Function	:	Class 생성자로서 Class 생성시 초기화 기능을 담당한다.
*	Create		:	2006.02.27
*	Author		:	김용태
*	Version		:	1.0
*/
CLogWriter::CLogWriter(char* pszLogFilePath, char* pszLogFileName, UINT uPeriodDay, UINT uWriteTerm, bool bEnable)
{
	//입력 받는 값으로 멤버 변수를 초기화 한다.
	strcpy(m_szLogFilePath, pszLogFilePath);
	strcpy(m_szLogFileName, pszLogFileName);
	m_uPeriodDay		= uPeriodDay;
	m_uWriteTerm		= uWriteTerm;
	m_bEnable			= bEnable;

	//로그파일 경로의 마지막문자가 '\' 인지를 확인한다. 
	if(m_szLogFilePath[strlen(m_szLogFilePath)-1] != '\\')
	{
		sprintf(m_szLogFilePath, "%s\\", m_szLogFilePath);
	}

	//마지막 기록된 파일 이름을 초기화 한다.
	memset(m_szLastWriteFileName,0,MAX_LOG_FILE_NAME);
	m_ulFileNumber		= 0;
	m_hLastFileHandle	= NULL;
	
	//Critical Section 초기화, Multi CPU 를 고려한 Spin Count 를 둔다.
	::InitializeCriticalSectionAndSpinCount(&m_csLogWriter, 2000);
	
	//파일 경로를 생성한다.
	if(OKAY != m_fnMakePath())
	{
		//Error 처리
		printf("CLogWriter::CLogWriter -> Path make error\n");
	}

	//보관 기간이 지난 파일을 지우도록 요청한다.
	if(OKAY != m_fnDeletePeriodOverFile())
	{
		printf("CLogWriter::m_fnConnectFileHandle -> PeriodOverFile delete fail..\n");
	}
}

/*
*	Module name	:	Class 소멸자
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸자
*	Create		:	2006.02.27
*	Author		:	김용태
*	Version		:	1.0
*/
CLogWriter::~CLogWriter(void)
{
	//마지막으로 기록한 파일의 핸들을 Close 한다.
	if(NULL != m_hLastFileHandle)
	{
		fclose(m_hLastFileHandle);
	}

	//Critical Section 삭제
	::DeleteCriticalSection(&m_csLogWriter);
}

/*
*	Module name	:	m_fnWriteLog
*	Parameter	:	Null로 끝나는 문자열 Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	Log를 기록한다.
*	Create		:	2006.02.27
*	Author		:	김용태
*	Version		:	1.1
*	-Version 1.1 (김용태), 2006.11.10
*		- Logwriter 동작 Flag 에 따른 기능 추가
*/
int CLogWriter::m_fnWriteLog(const char* pszLog)
{

	SYSTEMTIME systime;						//System 시간을 알기 위한 변수

	::GetLocalTime(&systime);

	//만일 Log 기록 기능 동작 flag 가 false 이면
	if(!m_bEnable)
	{
		//현재 Log 를 화면에 표시하고 끝낸다.
		printf("[%02d:%02d:%02d.%03d] %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
			systime.wMilliseconds, pszLog);
	}
	//그렇지 않고 만일 Log 기록 Flag 가 true 이면
	else
	{
		EnterCriticalSection(&m_csLogWriter);	//Critical Section 진입

		//현재 시간을 기준으로 Log 파일 핸들을 연결 요청한다. 함수 내에서는 m_hLastFileHandle 에 값을 설정한다.
		if(OKAY != m_fnConnectFileHandle())
		{
			//Error 처리
			printf("CLogWriter::m_fnWriteLog -> m_fnConnectFileHandle() process Fail..\n");
		}

		//문자열의 처음에 현재 시간,분,초(1/1000)을 삽입한다.
		//현재 문자열을 연결된 파일에 기록한다.

		if(NULL != m_hLastFileHandle)
		{
			printf("[%02d:%02d:%02d.%03d] %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
				systime.wMilliseconds, pszLog);
			fprintf(m_hLastFileHandle,"[%02d:%02d:%02d.%03d] %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
				systime.wMilliseconds, pszLog);

			fclose(m_hLastFileHandle);
			m_hLastFileHandle = NULL;
		}
		else 
		{
			printf("CLogWriter::m_fnWriteLog -> Log File Handle is NULL\n");
			printf("[%2d:%2d:%2d.%3d] %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
				systime.wMilliseconds, pszLog);
		}
		LeaveCriticalSection(&m_csLogWriter);	//Critical Section 탈출
	}
	
	//정상 종료
	return	1;
}

/*
*	Module name	:	m_fnConnectFileHandle
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	현재 시간을 기준으로 Log 파일을 생성하고 파일 핸들을 연결한다.
*	Create		:	2006.02.27
*	Author		:	김용태
*	Version		:	1.0
*/
int CLogWriter::m_fnConnectFileHandle()
{
	SYSTEMTIME	systime;
	char		szDIRFileName[MAX_LOG_FILE_PATH];
	int			nHour = -1;
	ULONG		ulNewFileNumber;

	//기본 파일 이름, 현재 년,월,일,시간을 조합하여 실제 기록될 파일 이름을 결정한다. 
	//시간 선택 기준은 0 시를 기준으로 Write term 만큼 더하게 되고, 해당 시간대를 포함하는 것을 선택한다.
	//WriteTerm 이 0이나 24 일경우 파일 이름에서 시간은 넣지 않는다.
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

	//만일 마지막으로 기록한 파일의 핸들이 존재하면 해당 파일을 닫는다.
	if(NULL != m_hLastFileHandle)
	{
		fclose(m_hLastFileHandle);
		m_hLastFileHandle = NULL;
	}
		
	////기록될 파일 이름이 마지막으로 성공한 파일 이름과 동일하지 않다면
	//if(m_ulFileNumber != ulNewFileNumber)
	//{
	//	//보관 기간이 지난 파일을 지우도록 요청한다.
	//	if(OKAY != m_fnDeletePeriodOverFile())
	//	{
	//		printf("CLogWriter::m_fnConnectFileHandle -> PeriodOverFile delete fail..\n");
	//	}
	//}

	//기록될 파일 이름을 결정한다. nHour 값이 결정 되었다면 파일 이름에 시간을 포함시킨다.
	if(0 <= nHour)
	{
		sprintf(m_szLastWriteFileName, "%s_%04d%02d%02d%02d.Log", m_szLogFileName, systime.wYear, systime.wMonth, systime.wDay, nHour);
	}
	else
	{
		sprintf(m_szLastWriteFileName, "%s_%04d%02d%02d.Log", m_szLogFileName, systime.wYear, systime.wMonth, systime.wDay);
	}

	//Path 를 포함한 FULL name 을 결정한다.
	sprintf(szDIRFileName, "%s%s", m_szLogFilePath, m_szLastWriteFileName);

	//결정된 File을 추가 권한으로 열고 Handle 을 저장한다.
	m_hLastFileHandle = fopen(szDIRFileName, "a");
	if(NULL == m_hLastFileHandle)
	{
		printf("CLogWriter::m_fnConnectFileHandle -> Logfile Create fail..\n");
		return -1;
	}

	//최근 기록된 File name에 대한 날자추출 파일을 Update 한다.
	m_ulFileNumber = ulNewFileNumber;


	//정상 종료
	return OKAY;
}

/*
*	Module name	:	m_fnMakePath
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	파일의 경로를 만든다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CLogWriter::m_fnMakePath()
{
	int	nRet;
//	int nSize;
//	WCHAR *wszLogPath;

	//로그파일 경로의 마지막문자가 '\' 인지를 확인한다.
	if(m_szLogFilePath[strlen(m_szLogFilePath)-1] != '\\')
	{
		sprintf(m_szLogFilePath, "%s\\", m_szLogFilePath);
	}
	
	//로그파일 경로를 가지는 문자열을 유니코드 방식으로 변경 한다. 함수 파라메터로 쓰기 위함
	//nSize		= MultiByteToWideChar(CP_ACP, 0, m_szLogFilePath, -1, NULL, NULL);
	//wszLogPath	= new WCHAR[nSize];
	//MultiByteToWideChar(CP_ACP, 0, m_szLogFilePath, (int)strlen(m_szLogFilePath) + 1, wszLogPath, nSize);

	//유니코드로 변경된 로그파일 경로를 참고하여 Path 를 생성한다.
	nRet = SHCreateDirectoryEx(NULL, m_szLogFilePath, NULL);
	
	if(ERROR_SUCCESS != nRet)
	{
		//printf("CLogWriter::m_fnMakePath -> Path make fail..\n");
		nRet = OKAY;
	}
	else
	{
		nRet = OKAY;
	}
//	delete []wszLogPath;

	//결과 값을 반환한다.
	return nRet;
}

/*
*	Module name	:	m_fnDeletePeriodOverFile
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	보관 기간이 지난 파일들을 삭제한다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CLogWriter::m_fnDeletePeriodOverFile()
{
	HANDLE				hSearch;
	WIN32_FIND_DATA		FileData;
	char				cLogFileName[MAX_LOG_FILE_NAME];
	char				cDeleteLogFileName[MAX_LOG_FILE_NAME];
	bool				bExistFIle;
	SYSTEMTIME			SystemTime;
	FILETIME			TempFileTime, SystemFileTime;
	UINT				uDifferenceDate;
	int					nErrorCode = OKAY;

	//만일 보관기간이 0 이라면 파일을 모두 가지고 있어야 하므로 정상 종료한다.
	if(0 >= m_uPeriodDay)
	{
		//printf("CLogWriter::m_fnDeletePeriodOverFile -> Period is 0.\n"); 
		return nErrorCode;
	}

	//저장 파일 경로 내의 파일 명 중에 로그 기본 파일 이름을 포함하는 첫 번째 파일의 속성을 가지고 온다.
	sprintf(cLogFileName, "%s%s*.log", m_szLogFilePath, m_szLogFileName);
	hSearch = FindFirstFile(cLogFileName, &FileData);

	//만일 파일 찾기 결과가 파일 없음을 나타내면 정상 종료
	if (INVALID_HANDLE_VALUE == hSearch) 
	{ 
		//printf("CLogWriter::m_fnDeletePeriodOverFile -> No files found.\n"); 
		//파일 찾기 종료
		FindClose(hSearch);
		return nErrorCode;
	}
	//파일 있음 플래그를 true로 설정 한다.
	bExistFIle = true;

	//현재 시스템 시간을 가지고 온다.
	::GetLocalTime(&SystemTime);
	SystemTimeToFileTime(&SystemTime,&SystemFileTime);

	//반복한다. 파일 있음 플래그가 false 일 때 까지
	while(bExistFIle)
	{
		//현재 시간에서 파일 생성 날자의 차이를 구한다.
		TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
		TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;
		
		uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);
		
		//만일 날자 차이가 보관 기간을 넘겼다면 파일을 삭제한다.
		if(uDifferenceDate >= m_uPeriodDay)
		{
			sprintf(cDeleteLogFileName, "%s%s", m_szLogFilePath, FileData.cFileName);
			DeleteFile(cDeleteLogFileName);
		}

		//만일 다음 파일을 찾을 때 오류 라면
		if(!FindNextFile(hSearch, &FileData))
		{
			nErrorCode = GetLastError();
			//만약 다음 파일이 없다면
			if(ERROR_NO_MORE_FILES == nErrorCode)
			{
				//정상 종료
				bExistFIle = false;
				nErrorCode = OKAY;
			}
			else
			{
				//다음 파일을 가지고 오지 못함
				printf("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n");
				bExistFIle = false;
			}
		}
	}
	
	//파일 찾기 종료
	FindClose(hSearch);

	return nErrorCode;
}


