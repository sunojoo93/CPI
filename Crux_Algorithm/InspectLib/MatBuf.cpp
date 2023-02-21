
/************************************************************************/
// Mat 메모리 관련 소스
// 수정일 : 17.08.02 - KSW
/************************************************************************/

// MatBuf.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectLib.h"
#include "MatBuf.h"

// CMatBuf

IMPLEMENT_DYNAMIC(CMatBuf, CWnd)

CMatBuf::CMatBuf()
{
	m_bUse			= false;
	m_bMemAlloc		= false;
	m_nSizeIndex	= 0;
	m_nMaxSizeToTal	= 0;
	m_bNULL			= false;

	m_nMem_Fix		= 0;
	m_nMem_Auto		= 0;
	//m_nMemoryIndex	= 0;
}

CMatBuf::~CMatBuf()
{
	DeleteMem();

	m_bUse			= false;
	m_bMemAlloc		= false;
	m_nSizeIndex	= 0;
	m_nMaxSizeToTal	= 0;

	m_nMem_Fix		= 0;
	m_nMem_Auto		= 0;
	//m_nMemoryIndex	= 0;
}


BEGIN_MESSAGE_MAP(CMatBuf, CWnd)
END_MESSAGE_MAP()


// CMatBuf 메시지 처리기입니다.


void CMatBuf::AllocMem(__int64 nSize)
{
	if( !m_bMemAlloc )
	{
		m_nMaxSizeToTal = sizeof(BYTE) * nSize;
		m_Data = (BYTE *)malloc(m_nMaxSizeToTal);
		memset(m_Data, 0, m_nMaxSizeToTal);	
		m_bMemAlloc = true;
	}	
}

void CMatBuf::DeleteMem()
{
	if( m_bMemAlloc )
	{
		free(m_Data);
		m_Data = NULL;
	}	
}

cv::Mat CMatBuf::GetMat(int rows, int cols, int type, bool bSetFlag, ENUM_MEMORY_HALF memory)
{
	// Lock
	m_MemLock.Lock();

	cv::Mat matTemp;

	__int64 nNewSize = rows * cols * GetTypeSize(type);

	if( m_bNULL )
	{
		// 새로 할당하여 넘겨주기
		matTemp = cv::Mat::zeros(rows, cols, type);
	}
	// 풀로 사용하는 경우
	else
	{
		// Max 넘어가는 경우, 새로 할당하여 넘겨주기
		if( m_nSizeIndex + nNewSize >= m_nMaxSizeToTal )
		{
			if (bSetFlag) {
				matTemp = cv::Mat::zeros(rows, cols, type);
				m_nMem_Auto += nNewSize;
			}
			else {
				matTemp.create(rows, cols, type);
				m_nMem_Auto += nNewSize;
			}
			//if(false)
			//{
			//	CString strTemp;
			//	strTemp.Format(_T("D:\\newMemory.csv"));
			//	// File Open
			//	std::ofstream outFile(strTemp, std::ofstream::ios_base::app);
			//	// write
			//	outFile << nNewSize << "," << m_nSizeIndex << "," << m_nMaxSizeToTal << "," << m_nMemoryIndex++ << "\n";
			//	// File Close
			//	outFile.close();
			//}    
		}
		else
		{
			matTemp = cv::Mat(rows, cols, type, &m_Data[ m_nSizeIndex ]);
			if(bSetFlag)
				matTemp.setTo(0);
			//if (false)
			//{
			//	CString strTemp;
			//	strTemp.Format(_T("D:\\oldMemory.csv"));
			//	// File Open
			//	std::ofstream outFile(strTemp, std::ofstream::ios_base::app);
			//	// write
			//	outFile << nNewSize << "," << m_nSizeIndex << "," << m_nMaxSizeToTal << "," << m_nMemoryIndex++ << "\n";
			//	// File Close
			//	outFile.close();
			//}
			m_nSizeIndex += nNewSize;
			m_nMem_Fix = m_nSizeIndex;
		}
	}

	// Lock
	m_MemLock.Unlock();

	// 메모리 넘겨주기
	return matTemp;
}

cv::Mat CMatBuf::GetMat(cv::Size size, int type, bool bSetFlag, ENUM_MEMORY_HALF memory)
{
	return GetMat(size.height, size.width, type, bSetFlag, memory);
}

void CMatBuf::SetMem(CMatBuf* cMatBuf)
{
	if( cMatBuf == NULL )
	{
		m_bNULL = true;
		return;
	}

	// 데이터 공유
	m_Data				= cMatBuf->m_Data;
	m_bUse				= cMatBuf->m_bUse;
	m_nSizeIndex		= cMatBuf->m_nSizeIndex;
	m_nMaxSizeToTal		= cMatBuf->m_nMaxSizeToTal;

	// 시작 위치 저장하기
	m_nStartIndex = m_nSizeIndex;
}

bool CMatBuf::GetUse()
{
	return m_bUse;
}

void CMatBuf::SetUse(bool bUse)
{
	m_bUse = bUse;

	// 해제할 경우, Size 초기화
	if( m_bUse == false )
	{
		// 메모리 Index 초기화
		m_nSizeIndex = 0;
	}
}

__int64 CMatBuf::Get_AutoMemory()
{
	return m_nMem_Auto;
}

__int64 CMatBuf::Get_FixMemory()
{
	return m_nMem_Fix;
}

int CMatBuf::GetTypeSize(int type)
{
	int nRet = -1;

	switch(type)
	{
	case CV_8U :
	case CV_8S :
		nRet = 1;
		break;

	case CV_16U :
	case CV_16S :
		nRet = 2;
		break;

	case CV_32S :
	case CV_32F :
		nRet = 4;
		break;

	case CV_64F :
		nRet = 8;
		break;

	default:
		nRet = -1;
		break;
	}

	return nRet;
}