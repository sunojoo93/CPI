
/************************************************************************/
// Mat �޸� ���� ���� �ҽ�
// ������ : 17.08.02 - KSW
/************************************************************************/


// MatBufferManager.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "InspectLib.h"
#include "MatBufferManager.h"

// 1GB = 1024 * 1024 * 1024
#define GIGA_BYTE			1073741824.0

// CMatBufferManager

IMPLEMENT_DYNAMIC(CMatBufferManager, CWnd)

CMatBufferManager::CMatBufferManager()
{
	//////////////////////////////////////////////////////////////////////////
	// KSW 17.09.29 - �ߺ����� �� �˶� �߻�
	//////////////////////////////////////////////////////////////////////////

	// CRUX_Algorithm.ini���� Data �о� ���� �ӽ��߰� 190218 YWS
	SetLodeData();
	SetLodeData_Low();
	SetLodeData_High();

	m_Data = new CMatBuf[m_nMax_Men_Count_ini];
	m_Data_Low = new CMatBuf[m_nMax_Men_Count_ini_Low];
	m_Data_High = new CMatBuf[m_nMax_Men_Count_ini_High];

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
		if( dUseMemory > 50 )
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
				AllocMem(m_nMaxAllocCount * 1024);
				AllocMem_Low(m_nMaxAllocCount_Low * 1024);
				AllocMem_High(m_nMaxAllocCount_High * 1024);
		}		
	}
	// ��Ʈ�� & �޸� ������ PC�� ���, 4M�� �Ҵ�
	else
	{
		// 4 MB
		AllocMem(m_nMenAllocCount);
		AllocMem_Low(m_nMenAllocCount_Low);
		AllocMem_High(m_nMenAllocCount_High);
	}
}

CMatBufferManager::~CMatBufferManager()
{
	DeleteMem();
	delete(m_Data);
	DeleteMem_Low();
	delete(m_Data_Low);
	DeleteMem_High();
	delete(m_Data_High);

}


BEGIN_MESSAGE_MAP(CMatBufferManager, CWnd)
END_MESSAGE_MAP()



// CMatBufferManager �޽��� ó�����Դϴ�.


CMatBuf* CMatBufferManager::FindFreeBuf()
{
	m_MemManager.Lock();

	int nIndex = -1;

	while( nIndex == -1 )
	{
		for (int i=0 ; i<m_nMax_Men_Count_ini ; i++)
		{
			if( !m_Data[i].GetUse() )
			{
				m_Data[i].SetUse(true);
				nIndex = i;
				break;
			}
		}

		Sleep(50);
	}

	m_MemManager.Unlock();

	return &m_Data[nIndex];
}

void CMatBufferManager::FindFreeBuf_Multi(const int nCount, CMatBuf **cMatBuf_Multi)
{
	m_MemManager.Lock();

	int *nIndex = new int[nCount];
	int nCnt = 0;
	while (true)
	{
		for (int i = 0; i < m_nMax_Men_Count_ini; i++)
		{
			if (!m_Data[i].GetUse())
			{
				m_Data[i].SetUse(true);
				nIndex[nCnt++] = i;
				break;
			}
		}
		Sleep(50);
		if (nCnt == nCount) break;
	}
	m_MemManager.Unlock();

	for (int i = 0; i < nCount; i++)
	{
		cMatBuf_Multi[i] = &m_Data[nIndex[i]];
	}
}

void CMatBufferManager::ReleaseFreeBuf(CMatBuf* data)
{
	for (int i=0 ; i<m_nMax_Men_Count_ini ; i++)
	{
		if( data == &m_Data[i] )
		{
			m_Data[i].SetUse(false);
		}
	}
}

void CMatBufferManager::ReleaseFreeBuf_Multi(const int nCount, CMatBuf** data)
{
	for (int i = 0; i < m_nMax_Men_Count_ini; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			if (data[j] == &m_Data[i])
			{
				m_Data[i].SetUse(false);
			}
		}
	}
}

void CMatBufferManager::AllocMem(__int64 nSizeMB)
{
	for (int i=0 ; i<m_nMax_Men_Count_ini ; i++)
	{
		// MB
		m_Data[i].AllocMem(nSizeMB * 1024 * 1024);
	}
}

void CMatBufferManager::DeleteMem()
{
	for (int i=0 ; i<m_nMax_Men_Count_ini ; i++)
	{
		m_Data[i].DeleteMem();
	}
}

// ini���� Data �б� �ӽ� �߰� 190218 YWS
void CMatBufferManager::SetLodeData()
{
	CString ALGORITHM_TASK_INI_FILE = GETDRV() + _T(":\\Crux\\DATA\\INI\\CRUX_Algorithm.ini");
	m_nMax_Men_Count_ini = GetPrivateProfileInt(_T("InspInfo"), _T("MaxMen Count"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMaxAllocCount = GetPrivateProfileInt(_T("InspInfo"), _T("MaxAlloc Count"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMenAllocCount = GetPrivateProfileInt(_T("InspInfo"), _T("MenAlloc Count"), 1, ALGORITHM_TASK_INI_FILE);
}

// ini���� Data �б� �ӽ� �߰� 190218 YWS
CString CMatBufferManager::GETDRV()
{
	TCHAR buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileName(NULL, buff, sizeof(buff));
	CString strFolder = buff;
	CString strRet = strFolder.Left(1);
	strRet.MakeUpper();
	return strRet;
}

//////////////////////////////////////////////////////////////////////////Low

CMatBuf* CMatBufferManager::FindFreeBuf_Low()
{
	m_MemManager_Low.Lock();

	int nIndex = -1;

	while (nIndex == -1)
	{
		for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
		{
			if (!m_Data_Low[i].GetUse())
			{
				m_Data_Low[i].SetUse(true);
				nIndex = i;
				break;
			}
		}

		Sleep(50);
	}

	m_MemManager_Low.Unlock();

	return &m_Data_Low[nIndex];
}

void CMatBufferManager::FindFreeBuf_Multi_Low(const int nCount, CMatBuf **cMatBuf_Multi)
{
	m_MemManager_Low.Lock();

	int *nIndex = new int[nCount];
	int nCnt = 0;
	while (true)
	{
		for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
		{
			if (!m_Data_Low[i].GetUse())
			{
				m_Data_Low[i].SetUse(true);
				nIndex[nCnt++] = i;
				break;
			}
		}
		Sleep(50);
		if (nCnt == nCount) break;
	}
	m_MemManager_Low.Unlock();

	for (int i = 0; i < nCount; i++)
	{
		cMatBuf_Multi[i] = &m_Data_Low[nIndex[i]];
	}
}

void CMatBufferManager::ReleaseFreeBuf_Low(CMatBuf* data)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
	{
		if (data == &m_Data_Low[i])
		{
			m_Data_Low[i].SetUse(false);
		}
	}
}

void CMatBufferManager::ReleaseFreeBuf_Multi_Low(const int nCount, CMatBuf** data)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			if (data[j] == &m_Data_Low[i])
			{
				m_Data_Low[i].SetUse(false);
			}
		}
	}
}

void CMatBufferManager::AllocMem_Low(__int64 nSizeMB)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
	{
		// MB
		m_Data_Low[i].AllocMem(nSizeMB * 1024 * 1024);
	}
}

void CMatBufferManager::DeleteMem_Low()
{
	for (int i = 0; i < m_nMax_Men_Count_ini_Low; i++)
	{
		m_Data_Low[i].DeleteMem();
	}
}

// ini���� Data �б� �ӽ� �߰� 190218 YWS
void CMatBufferManager::SetLodeData_Low()
{
	CString ALGORITHM_TASK_INI_FILE = GETDRV() + _T(":\\Crux\\DATA\\INI\\CRUX_Algorithm.ini");
	m_nMax_Men_Count_ini_Low = GetPrivateProfileInt(_T("InspInfo"), _T("MaxMen Count Low"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMaxAllocCount_Low = GetPrivateProfileInt(_T("InspInfo"), _T("MaxAlloc Count Low"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMenAllocCount_Low = GetPrivateProfileInt(_T("InspInfo"), _T("MenAlloc Count Low"), 1, ALGORITHM_TASK_INI_FILE);
}

//////////////////////////////////////////////////////////////////////////HIgh

CMatBuf* CMatBufferManager::FindFreeBuf_High()
{
	m_MemManager_High.Lock();

	int nIndex = -1;

	while (nIndex == -1)
	{
		for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
		{
			if (!m_Data_High[i].GetUse())
			{
				m_Data_High[i].SetUse(true);
				nIndex = i;
				break;
			}
		}

		Sleep(50);
	}

	m_MemManager_High.Unlock();

	return &m_Data_High[nIndex];
}

void CMatBufferManager::FindFreeBuf_Multi_High(const int nCount, CMatBuf **cMatBuf_Multi)
{
	m_MemManager_High.Lock();

	int *nIndex = new int[nCount];
	int nCnt = 0;
	while (true)
	{
		for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
		{
			if (!m_Data_High[i].GetUse())
			{
				m_Data_High[i].SetUse(true);
				nIndex[nCnt++] = i;
				break;
			}
		}
		Sleep(50);
		if (nCnt == nCount) break;
	}
	m_MemManager_High.Unlock();

	for (int i = 0; i < nCount; i++)
	{
		cMatBuf_Multi[i] = &m_Data_High[nIndex[i]];
	}
}

void CMatBufferManager::ReleaseFreeBuf_High(CMatBuf* data)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
	{
		if (data == &m_Data_High[i])
		{
			m_Data_High[i].SetUse(false);
		}
	}
}

void CMatBufferManager::ReleaseFreeBuf_Multi_High(const int nCount, CMatBuf** data)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
	{
		for (int j = 0; j < nCount; j++)
		{
			if (data[j] == &m_Data_High[i])
			{
				m_Data_High[i].SetUse(false);
			}
		}
	}
}

void CMatBufferManager::AllocMem_High(__int64 nSizeMB)
{
	for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
	{
		// MB
		m_Data_High[i].AllocMem(nSizeMB * 1024 * 1024);
	}
}

void CMatBufferManager::DeleteMem_High()
{
	for (int i = 0; i < m_nMax_Men_Count_ini_High; i++)
	{
		m_Data_High[i].DeleteMem();
	}
}

// ini���� Data �б� �ӽ� �߰� 190218 YWS
void CMatBufferManager::SetLodeData_High()
{
	CString ALGORITHM_TASK_INI_FILE = GETDRV() + _T(":\\Crux\\DATA\\INI\\CRUX_Algorithm.ini");
	m_nMax_Men_Count_ini_High = GetPrivateProfileInt(_T("InspInfo"), _T("MaxMen Count High"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMaxAllocCount_High = GetPrivateProfileInt(_T("InspInfo"), _T("MaxAlloc Count High"), 1, ALGORITHM_TASK_INI_FILE);
	m_nMenAllocCount_High = GetPrivateProfileInt(_T("InspInfo"), _T("MenAlloc Count High"), 1, ALGORITHM_TASK_INI_FILE);
}