
/************************************************************************/
// Mat 메모리 관리 관련 소스
// 수정일 : 17.08.02 - KSW
/************************************************************************/


// MatBufferManager.cpp : 구현 파일입니다.
//

#include "stdafx.h"
//#include "InspectLib.h"
#include "AviInspection.h"
#include "MatBufferResultManager.h"


// 1GB = 1024 * 1024 * 1024
#define GIGA_BYTE			1073741824.0

// CMatBufferResultManager

IMPLEMENT_DYNAMIC(CMatBufferResultManager, CWnd)

CMatBufferResultManager::CMatBufferResultManager()
{
	//////////////////////////////////////////////////////////////////////////
	// KSW 17.09.29 - 중복실행 시 알람 발생
	//////////////////////////////////////////////////////////////////////////

	// 메모리 정보 가져오기
	MEMORYSTATUS memoryInfo;
	memoryInfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryInfo);

	// 남아있는(사용 가능한) 물리적 메모리
	double dEmptyMemory = memoryInfo.dwAvailPhys / GIGA_BYTE;

	// 물리적 메모리 전체 크기 ( GB )
	double dTotalMemory = memoryInfo.dwTotalPhys / GIGA_BYTE;

	// 사용 중인 메모리
	double dUseMemory = dTotalMemory - dEmptyMemory;

	// 100GB 이상인 경우 ( 장비컴퓨터 )
	if( dTotalMemory > 100 )
	{
		// 50GB 이상 메모리 사용 중인 경우, 중복 실행으로 파악 ( 알람 띄움 )
		if( dUseMemory > 160 )
		{
			AfxMessageBox(_T("Overlap VSAlgorithmTask !"));

			// 프로그램 강제 종료 구문
			DWORD dwExitCode;
			DWORD dwPID = GetCurrentProcessId();

			HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, 0, dwPID ); 

			if( NULL != hProcess )
			{
				GetExitCodeProcess( hProcess, &dwExitCode );
				TerminateProcess( hProcess, dwExitCode );
				WaitForSingleObject( hProcess, INFINITE );
				CloseHandle( hProcess );
			}
		}
		// 중복 실행 아닌 경우, 정상 메모리 할당
		else
		{
				AllocMem_Result(1 * 1024);
		}		
	}
	// 노트북 & 메모리 부족한 PC인 경우, 4M씩 할당
	else
	{
		// 4 MB
		AllocMem_Result(4);
	}
}

CMatBufferResultManager::~CMatBufferResultManager()
{
	DeleteMem_Result();
}


BEGIN_MESSAGE_MAP(CMatBufferResultManager, CWnd)
END_MESSAGE_MAP()



// CMatBufferResultManager 메시지 처리기입니다.


CMatResultBuf* CMatBufferResultManager::FindFreeBuf_Result()
{
	m_MemManager.Lock();

	int nIndex = -1;

	while( nIndex == -1 )
	{
		for (int i=0 ; i<MAX_MEM_COUNT ; i++)
		{
			if( !m_Data[i].GetUse_Result() )
			{
				m_Data[i].SetUse_Result(true);
				nIndex = i;
				break;
			}
		}

		Sleep(50);
	}

	m_MemManager.Unlock();

	return &m_Data[nIndex];
}

void CMatBufferResultManager::ReleaseFreeBuf_Result(CMatResultBuf* data)
{
	for (int i=0 ; i<MAX_MEM_COUNT ; i++)
	{
		if( data == &m_Data[i] )
		{
			m_Data[i].SetUse_Result(false);
		}
	}
}


void CMatBufferResultManager::AllocMem_Result(__int64 nSizeMB)
{
	for (int i=0 ; i<MAX_MEM_COUNT ; i++)
	{
		// MB
		m_Data[i].AllocMem_Result(nSizeMB * 1024 * 1024);
	}
}

void CMatBufferResultManager::DeleteMem_Result()
{
	for (int i=0 ; i<MAX_MEM_COUNT ; i++)
	{
		m_Data[i].DeleteMem_Result();
	}
}
