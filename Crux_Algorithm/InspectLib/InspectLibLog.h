
/************************************************************************/
// 알고리즘 로그 관련 헤더
// 수정일 : 18.03.28 - KSW
/************************************************************************/

#pragma once

#include <iostream>
#include <fstream>

#include "time.h"
#include <string>

#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>

#include <direct.h>

#pragma comment(lib, "pdh.lib")

using namespace std;

// Log 저장 경로
#define LOG_PATH_ALG		GETDRV() + L":\\CRUX\\DATA\\LOG\\Algorithm"

//#define LOG_PATH_ALG		L"D:\\CRUX\\DATA\\LOG\\Algorithm"
// 1GB = 1024 * 1024 * 1024
#define GIGA_BYTE			1073741824.0


// Tact Time Log 사용 유무
#define USE_LOG_COMMON_LIB		TRUE
#define USE_LOG_COMMON_ALIGN	TRUE
#define USE_LOG_AVI_POINT		TRUE
#define USE_LOG_AVI_LINE		TRUE
#define USE_LOG_AVI_MURA		TRUE
#define USE_LOG_AVI_MURA_NORMAL	TRUE
#define USE_LOG_SVI_MURA		FALSE
#define USE_LOG_AVI_MURA3		TRUE
#define USE_LOG_AVI_MURA4		TRUE
#define USE_LOG_AVI_MURA_CHOLE	TRUE
#define	USE_LOG_AVI_MURA_SCRATCH TRUE
#define USE_LOG_AVI_MEMORY		FALSE//사용시 Alg Tact 로그 오류 생김, Memory Test 확인시에만 사용 21.05.27

#define USE_LOG_APP				FALSE


// 로그 타입
enum ENUM_ALG_TYPE
{
	E_ALG_TYPE_COMMON_LIB	= 0	,
	E_ALG_TYPE_COMMON_ALIGN		,
	E_ALG_TYPE_AVI_POINT		,
	E_ALG_TYPE_AVI_LINE			,
	E_ALG_TYPE_AVI_MURA			,
	E_ALG_TYPE_SVI_MURA			,
	
	E_ALG_TYPE_AVI_MURA_NORMAL	,
	E_ALG_TYPE_AVI_MURA_3		,	// 21.01.29 최광일
	E_ALG_TYPE_AVI_MURA_4		,   //
	E_ALG_TYPE_AVI_MURA_CHOLE	,   //
	E_ALG_TYPE_AVI_MURA_SCRATCH	,   //
	E_ALG_TYPE_AVI_MEMORY		,
	E_ALG_TYPE_APP				,

	E_ALG_TYPE_CPU				,
	E_ALG_TYPE_TOTAL
};


//////////////////////////////////////////////////////////////////////////

class InspectLibLog
{
public:
	InspectLibLog(void)
	{
		for (int i=0 ; i<E_ALG_TYPE_TOTAL ; i++)
			InitializeCriticalSection(&safeFile[i]);

		m_bCPU = initCPU();
	}

	virtual ~InspectLibLog(void)
	{
		for (int i=0 ; i<E_ALG_TYPE_TOTAL ; i++)
			DeleteCriticalSection(&safeFile[i]);

		destroyCPU();
	}

	static const bool Use_AVI_Memory_Log = USE_LOG_AVI_MEMORY; //choi 21.05.27

protected:
	//	Algorithm Log 전용 critical section
	CRITICAL_SECTION	safeFile[E_ALG_TYPE_TOTAL];	

	// CPU
	PDH_HCOUNTER		m_hCounter;
	PDH_HQUERY			m_hQuery;
	bool				m_bCPU;

public:

	// 로그 경로 가져오기
	void getLogPath(int nAlgType, wchar_t* strPath)
	{
		SYSTEMTIME time;
		::GetLocalTime(&time);
		CString strAlgLogPath = LOG_PATH_ALG;

		switch(nAlgType)
		{
		case E_ALG_TYPE_COMMON_LIB:
			if( USE_LOG_COMMON_LIB )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibInspect_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		case E_ALG_TYPE_COMMON_ALIGN:
			if( USE_LOG_COMMON_ALIGN )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAlign_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		case E_ALG_TYPE_AVI_POINT:
			if( USE_LOG_AVI_POINT )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIPoint_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		case E_ALG_TYPE_AVI_LINE:
			if( USE_LOG_AVI_LINE )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVILine_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		case E_ALG_TYPE_AVI_MURA:
			if( USE_LOG_AVI_MURA )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMura_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;
		case E_ALG_TYPE_AVI_MURA_NORMAL:
			if (USE_LOG_AVI_MURA_NORMAL)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMuraNormal_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;
		case E_ALG_TYPE_SVI_MURA:
			if( USE_LOG_SVI_MURA )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibSVIMura_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		////////////////////////////////////////////////////////////////////////// 21.01.29 최광일
		case E_ALG_TYPE_AVI_MURA_3:
			if (USE_LOG_AVI_MURA3)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMura3_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay);
			break;
		case E_ALG_TYPE_AVI_MURA_4:
			if (USE_LOG_AVI_MURA4)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMura4_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay);
			break;

		case E_ALG_TYPE_AVI_MURA_CHOLE:
			if (USE_LOG_AVI_MURA_CHOLE)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMuraChole_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay);
			break;

		case E_ALG_TYPE_AVI_MURA_SCRATCH:
			if (USE_LOG_AVI_MURA_CHOLE)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMuraScratch_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay);
			break;

		case E_ALG_TYPE_AVI_MEMORY:
			if (USE_LOG_AVI_MEMORY)
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAVIMemory_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay);
		//////////////////////////////////////////////////////////////////////////

		case E_ALG_TYPE_APP:
			if( USE_LOG_APP )
				swprintf_s(strPath, MAX_PATH, L"%s\\LibAPP_%04d%02d%02d.Log", (LPCWSTR)strAlgLogPath, time.wYear, time.wMonth, time.wDay );
			break;

		default:
			break;
		}
	};

	void get_SubType(int nAlgType, string& sub_AlgType)
	{
		SYSTEMTIME time;
		::GetLocalTime(&time);
		CString strAlgLogPath = LOG_PATH_ALG;

		switch (nAlgType)
		{
		case E_ALG_TYPE_COMMON_LIB:
			if (USE_LOG_COMMON_LIB)
				sub_AlgType = "COMMON_LIB";
				
			break;

		case E_ALG_TYPE_COMMON_ALIGN:
			if (USE_LOG_COMMON_ALIGN)
				sub_AlgType = "LibAlign";
				
			break;

		case E_ALG_TYPE_AVI_POINT:
			if (USE_LOG_AVI_POINT)
				sub_AlgType = "LibAVIPoint";
				
			break;

		case E_ALG_TYPE_AVI_LINE:
			if (USE_LOG_AVI_LINE)
				sub_AlgType = "LibAVILine";
				
			break;

		case E_ALG_TYPE_AVI_MURA:
			if (USE_LOG_AVI_MURA)
				sub_AlgType = "LibAVIMura";
				
			break;
		case E_ALG_TYPE_AVI_MURA_NORMAL:
			if (USE_LOG_AVI_MURA_NORMAL)
				sub_AlgType = "LibAVIMuraNormal";
				
			break;
		case E_ALG_TYPE_SVI_MURA:
			if (USE_LOG_SVI_MURA)
				sub_AlgType = "LibSVIMura";
				
			break;

			////////////////////////////////////////////////////////////////////////// 21.01.29 최광일
		case E_ALG_TYPE_AVI_MURA_3:
			if (USE_LOG_AVI_MURA3)
				sub_AlgType = "LibAVIMura3";
				
			break;
		case E_ALG_TYPE_AVI_MURA_4:
			if (USE_LOG_AVI_MURA4)
				sub_AlgType = "LibAVIMura4";
				
			break;

		case E_ALG_TYPE_AVI_MURA_CHOLE:
			if (USE_LOG_AVI_MURA_CHOLE)
				sub_AlgType = "LibAVIMuraChole";

			break;
		
		case E_ALG_TYPE_AVI_MURA_SCRATCH:
			if (USE_LOG_AVI_MURA_SCRATCH)
				sub_AlgType = "LibAVIMuraScratch";

			break;

		case E_ALG_TYPE_AVI_MEMORY:
			if (USE_LOG_AVI_MEMORY)
				sub_AlgType = "LibAVIMemory";

			break;
			//////////////////////////////////////////////////////////////////////////

		case E_ALG_TYPE_APP:
			if (USE_LOG_APP)
				sub_AlgType = "LibAPP";
				
			break;

		default:
			break;
		}
	};

	// 초기화 CPU
	bool initCPU()
	{
		PDH_STATUS status = PdhOpenQuery(NULL, NULL, &m_hQuery);
		if( status != ERROR_SUCCESS )	return false;

		status = PdhAddCounter( m_hQuery, _T("\\Processor(_TOTAL)\\% Processor Time"), NULL, &m_hCounter );
		if( status != ERROR_SUCCESS )	return false;

		status = PdhCollectQueryData( m_hQuery );
		if( status != ERROR_SUCCESS )	return false;

		return true;
	}

	// 해제 CPU
	void destroyCPU()
	{
		if( m_hQuery )
			PdhCloseQuery( m_hQuery );

		m_hQuery = NULL;
	}

	// CPU 메모리 가져오기
	double getUseMemoryCPU()
	{
		// 초기화 안된 경우
		if( !m_bCPU )	return 0.0;

		//EnterCriticalSection(&safeFile[E_ALG_TYPE_CPU]);

		PDH_STATUS status = PdhCollectQueryData( m_hQuery );
		if( status != ERROR_SUCCESS )
		{
			//LeaveCriticalSection(&safeFile[E_ALG_TYPE_CPU]);
			return 0.0;
		}

		PDH_FMT_COUNTERVALUE    value;
		status = PdhGetFormattedCounterValue( m_hCounter, PDH_FMT_DOUBLE, 0, &value );
		if( status != ERROR_SUCCESS )
		{
			//LeaveCriticalSection(&safeFile[E_ALG_TYPE_CPU]);
			return 0.0;
		}

		//LeaveCriticalSection(&safeFile[E_ALG_TYPE_CPU]);

		return (double)value.doubleValue;
	}

	// RAM 메모리 가져오기
	double getUseMemoryRAM()
	{
		// 메모리 정보 가져오기
		MEMORYSTATUS memoryInfo;
		memoryInfo.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&memoryInfo);

		// 남아있는(사용 가능한) 물리적 메모리
		double dEmptyMemory = memoryInfo.dwAvailPhys / GIGA_BYTE;

		// 물리적 메모리 전체 크기 ( GB )
		double dTotalMemory = memoryInfo.dwTotalPhys / GIGA_BYTE;

		// 사용 중인 메모리
		return dTotalMemory - dEmptyMemory;
	};

	// 로그만 쓰는 경우
	clock_t writeInspectLog(int nAlgType, char* strFunc, wchar_t* strTxt, wchar_t* strPat=NULL)
	{
		// 로그 경로 가져오기
		wchar_t strPath[MAX_PATH] = {0, };
		getLogPath(nAlgType, strPath);

		// 로그 경로 없는 경우 나가기
		if( strPath[0] == NULL )		return 0;

		clock_t tBeforeTime = clock();
		int		nClock = (int)tBeforeTime;

		SYSTEMTIME lst;
		GetLocalTime(&lst);
		EnterCriticalSection(&safeFile[nAlgType]);
		
		// File Open
		std::ofstream outFile(strPath, std::ofstream::ios_base::app);

		// Function
		string swsFunc(strFunc);

		CString strTime;
		if( strPat != NULL )
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]\t%s"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, nClock, getUseMemoryRAM(), strPat );
		else
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, nClock, getUseMemoryRAM() );

		wstring wsTimeTxt(strTime);
		string	swsTimeTxt(wsTimeTxt.begin(), wsTimeTxt.end());

		// Txt
		if( strTxt == NULL )
		{
			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\n";
		}
		else
		{
			wstring wsTxt(strTxt);
			string swsTxt(wsTxt.begin(), wsTxt.end());

			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\t" << swsTxt << "\n";
		}		

		// File Close
		outFile.close();

		LeaveCriticalSection(&safeFile[nAlgType]);

		return tBeforeTime;
	};

	// 시간 측정 로그
	clock_t writeInspectLogTime(int nAlgType, clock_t tBeforeTime, char* strFunc, wchar_t* strTxt, wchar_t* strPat=NULL)
	{
		// 로그 경로 가져오기
		wchar_t strPath[MAX_PATH] = {0, };
		getLogPath(nAlgType, strPath);

		// 로그 경로 없는 경우 나가기
		if( strPath[0] == NULL )		return 0;

		int		nClock = (int)tBeforeTime;

		SYSTEMTIME lst;
		GetLocalTime(&lst);
		EnterCriticalSection(&safeFile[nAlgType]);

		// File Open
		std::ofstream outFile(strPath, std::ofstream::ios_base::app);

		// Function
		string swsFunc(strFunc);

		// Time
		clock_t tAfterTime = clock();
		double dAfterTime = (double)(tAfterTime - tBeforeTime) / CLOCKS_PER_SEC;

		CString strTime;
		if( strPat != NULL )
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]\t%s"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tBeforeTime, getUseMemoryRAM(), strPat );
		else
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tBeforeTime, getUseMemoryRAM() );

		wstring wsTimeTxt(strTime);
		string	swsTimeTxt(wsTimeTxt.begin(), wsTimeTxt.end());

		if( strTxt == NULL )
		{
			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\t" << dAfterTime << "\n";
		}
		else
		{
			wstring wsTxt(strTxt);
			string swsTxt(wsTxt.begin(), wsTxt.end());

			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\t" << swsTxt << "\t" << dAfterTime << "\n";
		}

		// File Close
		outFile.close();

		LeaveCriticalSection(&safeFile[nAlgType]);

		return tAfterTime;
	};

	// 시간 측정 로그
	clock_t writeInspectLogTime(int nAlgType, clock_t tInitTime, clock_t tBeforeTime, char* strFunc, wchar_t* strTxt, wchar_t* strPat=NULL)
	{
		// 로그 경로 가져오기
		wchar_t strPath[MAX_PATH] = {0, };
		getLogPath(nAlgType, strPath);

		// 로그 경로 없는 경우 나가기
		if( strPath[0] == NULL )		return 0;

		int		nClock = (int)tBeforeTime;

		SYSTEMTIME lst;
		GetLocalTime(&lst);
		EnterCriticalSection(&safeFile[nAlgType]);

		// File Open
		std::ofstream outFile(strPath, std::ofstream::ios_base::app);

		// Function
		string swsFunc(strFunc);

		// Time
		clock_t tAfterTime = clock();
		double dAfterTime = (double)(tAfterTime - tBeforeTime) / CLOCKS_PER_SEC;
		double dTotalTime = (double)(tAfterTime - tInitTime) / CLOCKS_PER_SEC;

		CString strTime;
		if( strPat != NULL )
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]\t%s"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM(), strPat );
		else
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM() );

		wstring wsTimeTxt(strTime);
		string	swsTimeTxt(wsTimeTxt.begin(), wsTimeTxt.end());

		if( strTxt == NULL )
		{
			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\t" << dAfterTime << "\t(" << dTotalTime << ")" << "\n";
		}
		else
		{
			wstring wsTxt(strTxt);
			string swsTxt(wsTxt.begin(), wsTxt.end());

			// write
			outFile << swsTimeTxt << "\t" << swsFunc << "\t" << swsTxt << "\t" << dAfterTime << "\t(" << dTotalTime << ")" <<"\t"<<"\n";
		}

		// File Close
		outFile.close();

		LeaveCriticalSection(&safeFile[nAlgType]);

		return tAfterTime;
	};
	//////////////////////////////////////////////////////////////////////////choi
	clock_t writeInspectLogTime(int nAlgType, clock_t tInitTime, clock_t tBeforeTime, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0, wchar_t* strPat = NULL)
	{
		// 로그 경로 가져오기
		wchar_t strPath[MAX_PATH] = { 0, };
		wchar_t AlgType_Sub[MAX_PATH] = { 0, };

		getLogPath(nAlgType, strPath);

		// 로그 경로 없는 경우 나가기
		if (strPath[0] == NULL)		return 0;

		int		nClock = (int)tBeforeTime;

		SYSTEMTIME lst;
		GetLocalTime(&lst);
		EnterCriticalSection(&safeFile[nAlgType]);

		// File Open
		std::ofstream outFile(strPath, std::ofstream::ios_base::app);

		// Function
		string swsFunc(strFunc);

		// Time
		clock_t tAfterTime = clock();
		double dAfterTime = (double)(tAfterTime - tBeforeTime) / CLOCKS_PER_SEC;
		double dTotalTime = (double)(tAfterTime - tInitTime) / CLOCKS_PER_SEC;

		CString strTime;
		if (strPat != NULL)
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]\t%s"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM(), strPat);
		else
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM());

		wstring wsTimeTxt(strTime);
		string	swsTimeTxt(wsTimeTxt.begin(), wsTimeTxt.end());

	
		
			if (strTxt == NULL)
			{
				// write
				outFile << swsTimeTxt << "\t" << swsFunc << "\t" << nMemory_Use_Value << "\n";
			}
			else
			{
				wstring wsTxt(strTxt);
				string swsTxt(wsTxt.begin(), wsTxt.end());

				// write
				outFile << swsTimeTxt << "\t" << swsFunc << "\t" << swsTxt << "\t" << nMemory_Use_Value << "\n";
			}
	

		// File Close
		outFile.close();

		LeaveCriticalSection(&safeFile[nAlgType]);

		return tAfterTime;
	};

	clock_t writeInspectLogTime(int nAlgType, clock_t tInitTime, clock_t tBeforeTime, char* strFunc, wchar_t* strTxt, __int64 nMemory_Use_Value = 0, int nSub_AlgType = NULL ,wchar_t* strPat = NULL)
	{
		// 로그 경로 가져오기
		wchar_t strPath[MAX_PATH] = { 0, };
		string AlgType_Sub ="";

		getLogPath(nAlgType, strPath);

		if (nSub_AlgType != NULL) {
			get_SubType(nSub_AlgType, AlgType_Sub);
		}
		// 로그 경로 없는 경우 나가기
		if (strPath[0] == NULL)		return 0;

		int		nClock = (int)tBeforeTime;

		SYSTEMTIME lst;
		GetLocalTime(&lst);
		EnterCriticalSection(&safeFile[nAlgType]);

		// File Open
		std::ofstream outFile(strPath, std::ofstream::ios_base::app);

		// Function
		string swsFunc(strFunc);

		// Time
		clock_t tAfterTime = clock();
		double dAfterTime = (double)(tAfterTime - tBeforeTime) / CLOCKS_PER_SEC;
		double dTotalTime = (double)(tAfterTime - tInitTime) / CLOCKS_PER_SEC;

		CString strTime;
		if (strPat != NULL)
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]\t%s"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM(), strPat);
		else
			strTime.Format(_T("[%02d:%02d:%02d:%03d]\t%09d\t[%.4fGB]"), lst.wHour, lst.wMinute, lst.wSecond, lst.wMilliseconds, tInitTime, getUseMemoryRAM());

		wstring wsTimeTxt(strTime);
		string	swsTimeTxt(wsTimeTxt.begin(), wsTimeTxt.end());

		if (nSub_AlgType != NULL) {
			if (strTxt == NULL)
			{
				// write
				outFile << swsTimeTxt << "\t" << AlgType_Sub << "::" << swsFunc << "\t" << nMemory_Use_Value << "\n";
			}
			else
			{
				wstring wsTxt(strTxt);
				string swsTxt(wsTxt.begin(), wsTxt.end());

				// write
				outFile << swsTimeTxt << "\t" << AlgType_Sub << "::" << swsFunc << "\t" << swsTxt << "\t" << nMemory_Use_Value << "\n";
			}
		}
		else {
			if (strTxt == NULL)
			{
				// write
				outFile << swsTimeTxt << "\t" << swsFunc << "\t" << nMemory_Use_Value << "\n";
			}
			else
			{
				wstring wsTxt(strTxt);
				string swsTxt(wsTxt.begin(), wsTxt.end());

				// write
				outFile << swsTimeTxt << "\t"  << swsFunc << "\t" << swsTxt << "\t" << nMemory_Use_Value << "\n";
			}
		}

		// File Close
		outFile.close();

		LeaveCriticalSection(&safeFile[nAlgType]);

		return tAfterTime;
	};

	//////////////////////////////////////////////////////////////////////////
	// 현재 솔루션 실행 드라이브 판단 추가
	CString GETDRV() 
	{ 
		TCHAR buff[MAX_PATH];
		memset(buff, 0, MAX_PATH);
		::GetModuleFileName(NULL, buff, sizeof(buff));
		CString strFolder = buff;
		CString strRet = strFolder.Left(1);
		strRet.MakeUpper();
		return strRet;
	};
};