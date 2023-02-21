
/************************************************************************/
// Mat �޸� ���� �ҽ�
// ������ : 17.08.02 - KSW
/************************************************************************/

// MatBuf.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
/*#include "InspectLib.h"*/
#include "MatResultBuf.h"
#include "AviInspection.h"

// CMatBuf

IMPLEMENT_DYNAMIC(CMatResultBuf, CWnd)

CMatResultBuf::CMatResultBuf()
{
	m_bUse			= false;
	m_bMemAlloc		= false;
	m_nSizeIndex	= 0;
	m_nMaxSizeToTal	= 0;
	m_bNULL			= false;

	//m_nMemoryIndex	= 0;
}

CMatResultBuf::~CMatResultBuf()
{
	DeleteMem_Result();

	m_bUse			= false;
	m_bMemAlloc		= false;
	m_nSizeIndex	= 0;
	m_nMaxSizeToTal	= 0;

	//m_nMemoryIndex	= 0;
}


BEGIN_MESSAGE_MAP(CMatResultBuf, CWnd)
END_MESSAGE_MAP()


// CMatBuf �޽��� ó�����Դϴ�.


void CMatResultBuf::AllocMem_Result(__int64 nSize)
{
	if( !m_bMemAlloc )
	{
		m_nMaxSizeToTal = sizeof(BYTE) * nSize;
		m_Data = (BYTE *)malloc(m_nMaxSizeToTal);
		memset(m_Data, 0, m_nMaxSizeToTal);	
		m_bMemAlloc = true;
	}	
}

void CMatResultBuf::DeleteMem_Result()
{
	if( m_bMemAlloc )
	{
		free(m_Data);
		m_Data = NULL;
	}	
}

cv::Mat CMatResultBuf::GetMat_Result(int rows, int cols, int type, bool bSetFlag, ENUM_MEMORY_HALF memory)
{
	// Lock
	m_MemLock.Lock();

	cv::Mat matTemp;

	__int64 nNewSize = rows * cols * GetTypeSize_Result(type);

	if( m_bNULL )
	{
		// ���� �Ҵ��Ͽ� �Ѱ��ֱ�
		matTemp = cv::Mat::zeros(rows, cols, type);
	}
	// Ǯ�� ����ϴ� ���
	else
	{
		// Max �Ѿ�� ���, ���� �Ҵ��Ͽ� �Ѱ��ֱ�
		if( m_nSizeIndex + nNewSize >= m_nMaxSizeToTal )
		{
			matTemp = cv::Mat::zeros(rows, cols, type); 
		}
		else
		{
			matTemp = cv::Mat(rows, cols, type, &m_Data[ m_nSizeIndex ]);
			if(bSetFlag)
				matTemp.setTo(0);

			m_nSizeIndex += nNewSize;
		}
	}

	// Lock
	m_MemLock.Unlock();

	// �޸� �Ѱ��ֱ�
	return matTemp;
}

cv::Mat CMatResultBuf::GetMat_Result(cv::Size size, int type, bool bSetFlag, ENUM_MEMORY_HALF memory)
{
	return GetMat_Result(size.height, size.width, type, bSetFlag, memory);
}

void CMatResultBuf::SetMem_Result(CMatResultBuf* cMatResultBuf)
{
	if( cMatResultBuf == NULL )
	{
		m_bNULL = true;
		return;
	}

	// ������ ����
	m_Data			= cMatResultBuf->m_Data;
	m_bUse			= cMatResultBuf->m_bUse;
	m_nSizeIndex	= cMatResultBuf->m_nSizeIndex;
	m_nMaxSizeToTal = cMatResultBuf->m_nMaxSizeToTal;

	// ���� ��ġ �����ϱ�
	m_nStartIndex = m_nSizeIndex;
}

bool CMatResultBuf::GetUse_Result()
{
	return m_bUse;
}

void CMatResultBuf::SetUse_Result(bool bUse)
{
	m_bUse = bUse;

	// ������ ���, Size �ʱ�ȭ
	if( m_bUse == false )
	{
		// �޸� Index �ʱ�ȭ
		m_nSizeIndex = 0;
	}
}

int CMatResultBuf::GetTypeSize_Result(int type)
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