
/************************************************************************/
// Mat �޸� ���� ���� �ҽ�
// ������ : 17.08.02 - KSW
/************************************************************************/


// MatBufferManager.cpp : ���� �����Դϴ�.
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
	// KSW 17.09.29 - �ߺ����� �� �˶� �߻�
	//////////////////////////////////////////////////////////////////////////

	// �޸� ���� ��������
	MEMORYSTATUS memoryInfo;
	memoryInfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memoryInfo);

	// �����ִ�(��� ������) ������ �޸�
	double dEmptyMemory = memoryInfo.dwAvailPhys / GIGA_BYTE;

	// ������ �޸� ��ü ũ�� ( GB )
	double dTotalMemory = memoryInfo.dwTotalPhys / GIGA_BYTE;

	// ��� ���� �޸�
	double dUseMemory = dTotalMemory - dEmptyMemory;

	// 100GB �̻��� ��� ( �����ǻ�� )
	if( dTotalMemory > 100 )
	{
		// 50GB �̻� �޸� ��� ���� ���, �ߺ� �������� �ľ� ( �˶� ��� )
		if( dUseMemory > 160 )
		{
			AfxMessageBox(_T("Overlap VSAlgorithmTask !"));

			// ���α׷� ���� ���� ����
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
		// �ߺ� ���� �ƴ� ���, ���� �޸� �Ҵ�
		else
		{
				AllocMem_Result(1 * 1024);
		}		
	}
	// ��Ʈ�� & �޸� ������ PC�� ���, 4M�� �Ҵ�
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



// CMatBufferResultManager �޽��� ó�����Դϴ�.


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
