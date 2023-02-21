#include "StdAfx.h"
//#include <stdio.h>
#include "SharedMemWrapper.h"
//#include "VSCameraTask.h"
#include "..\..\CommonHeaders\Structure.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SMemImageInfo::SMemImageInfo()
{
	memset( this, NULL , sizeof(SMemImageInfo) );
}

SMemImageInfo &SMemImageInfo::operator = (const SMemImageInfo &rhs)
{
	memcpy( this, &rhs, sizeof(SMemImageInfo) );

	return *this;
}

CSharedMemWrapper::CSharedMemWrapper()
	: m_hAsignedMemory(NULL)
	, m_pSharedMemory(NULL)
{
	for (int nCnt = 0; nCnt < MAX_IMAGE_COUNT; nCnt++)
		m_pImageBuffer[nCnt] = NULL;
}

CSharedMemWrapper::CSharedMemWrapper(int nImageSizeX, int nImageSizeY, int nImageBitrate, int nImageBandWidth, CString strDrv_CamNo, int nImageCount)
	: m_hAsignedMemory(NULL)
	, m_pSharedMemory(NULL)
{
	for (int nCnt = 0; nCnt < MAX_IMAGE_COUNT; nCnt++)
		m_pImageBuffer[nCnt] = NULL;

	BOOL bRet = CreateSharedMem(nImageSizeX, nImageSizeY, nImageBitrate, nImageBandWidth, strDrv_CamNo, nImageCount);
	//theApp.SetSMemState(bRet);
}

CSharedMemWrapper::~CSharedMemWrapper()
{
	DeleteMemory();
	//theApp.SetSMemState(FALSE);
}

BYTE* CSharedMemWrapper::GetGlobalMemoryPtr(INT64 &nStartPos, INT64 nReadSize)
{
	BYTE *pReturn = m_pSharedMemory+nStartPos;
	nStartPos+= nReadSize;

	return pReturn;
}

// Camera Task 에서만 Create - 다른 Task 는 Open 만 할 것.
// CRUX 실행 시 Alg. Task 보다 Camera Task 를 먼저 실행해야 함.
BOOL CSharedMemWrapper::CreateSharedMem(int nImageSizeX, int nImageSizeY, int nImageBitrate, int nImageBandWidth, CString strDrv_CamNo, int nImageCount)
{
	if(nImageSizeX <= 0 || nImageSizeY <= 0 || nImageBitrate <= 0 || nImageBandWidth <= 0 || nImageCount <= 0)
	{
		::AfxMessageBox(_T("Invalid Image Size Information!"));
		return FALSE;
	}

	DeleteMemory();

	int nBitrate;
	if (nImageBitrate == 8)		nBitrate = 1;
	else						nBitrate = 2;
	INT64 nImageSize = nImageSizeX * MAX_IMAGE_RATIO * nImageSizeY * MAX_IMAGE_RATIO * nImageBandWidth * nBitrate;	// * nImageCount;

	INT64 nTotalAsignedBuffSize = sizeof (SMemImageInfo) + (nImageSize * nImageCount);

	DWORD high = static_cast<DWORD>((nTotalAsignedBuffSize >> 32) & 0xFFFFFFFF);
	DWORD low  = static_cast<DWORD>( nTotalAsignedBuffSize        & 0xFFFFFFFF);

	CString strCamSMemName;
	strCamSMemName.Format(_T("PDC_SHARED_MEM_CAMERA_%s"), strDrv_CamNo);
	m_hAsignedMemory = (HANDLE) CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, high, low, strCamSMemName);

	if(m_hAsignedMemory == NULL)	
	{
		return FALSE;
	}

	m_pSharedMemory = (BYTE *)MapViewOfFile(m_hAsignedMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	INT64 nPos = 0;
	m_pImageInfo = (SMemImageInfo*)GetGlobalMemoryPtr(nPos, sizeof(SMemImageInfo));
	for (int nCnt = 0; nCnt < nImageCount; nCnt++)
		m_pImageBuffer[nCnt] = GetGlobalMemoryPtr(nPos, nImageSize);

	// Set ImageInfo
	//SMemImageInfo* pImageInfo = new SMemImageInfo();
	m_pImageInfo->nImageWidth		= nImageSizeX;
	m_pImageInfo->nImageHeight		= nImageSizeY;
	m_pImageInfo->nImageBitrate		= nImageBitrate;
	m_pImageInfo->nImageBandWidth	= nImageBandWidth;
	m_pImageInfo->nImageCount		= nImageCount;
	//memset(pImageInfo->byteReserved, 0, sizeof(pImageInfo->byteReserved));
	//SetImageInfo(pImageInfo);
	//SAFE_DELETE(pImageInfo);

	return TRUE;
}

BOOL CSharedMemWrapper::OpenSharedMem(CString strDrv_CamNo)
{
	DeleteMemory();
	
	CString strCamSMemName;
	strCamSMemName.Format(_T("PDC_SHARED_MEM_CAMERA_%s"), strDrv_CamNo);
	//m_hAsignedMemory = (HANDLE) CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)nTotalAsignedBuffSize, strCamSMemName);
	m_hAsignedMemory = (HANDLE) OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, strCamSMemName);

	if(m_hAsignedMemory == NULL)	
	{
		return FALSE;
	}

	m_pSharedMemory = (BYTE *)MapViewOfFile(m_hAsignedMemory, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	INT64 nPos = 0;
	m_pImageInfo = (SMemImageInfo*)GetGlobalMemoryPtr(nPos, sizeof(SMemImageInfo));

	long nImageSizeX, nImageSizeY, nImageBitrate, nImageBandWidth, nImageCount;

	nImageSizeX		= m_pImageInfo->nImageWidth;
	nImageSizeY		= m_pImageInfo->nImageHeight;
	nImageBitrate	= m_pImageInfo->nImageBitrate;
	nImageBandWidth	= m_pImageInfo->nImageBandWidth;
	nImageCount		= m_pImageInfo->nImageCount;
	
	int nBitrate;
	if (nImageBitrate == 8)		nBitrate = 1;
	else						nBitrate = 2;
	INT64 nImageSize = nImageSizeX * MAX_IMAGE_RATIO * nImageSizeY * MAX_IMAGE_RATIO * nImageBandWidth * nBitrate;
	
	for (int nCnt = 0; nCnt < nImageCount; nCnt++)
		m_pImageBuffer[nCnt] = GetGlobalMemoryPtr(nPos, nImageSize);

	return TRUE;
}


BOOL CSharedMemWrapper::IsInitial()
{
	return m_hAsignedMemory != NULL;
}

/////// Global Function
unsigned char *CSharedMemWrapper::GetImgAddress(int nImgCnt)
{
	if(!IsInitial())
		return NULL;

	return m_pImageBuffer[nImgCnt];
}

int CSharedMemWrapper::GetImgWidth()
{
  	return m_pImageInfo->nImageWidth;
}

int CSharedMemWrapper::GetImgHeight()
{
	return m_pImageInfo->nImageHeight;
}

int CSharedMemWrapper::GetImgBitrate()
{
	return m_pImageInfo->nImageBitrate;
}

int CSharedMemWrapper::GetImgBandWidth()
{
	return m_pImageInfo->nImageBandWidth;
}

double CSharedMemWrapper::GetImgBufSize()
{
	int nBitrate;	// 10, 12bit Image 대응
	if (m_pImageInfo->nImageBitrate == 8)		nBitrate = 1;
	else										nBitrate = 2;
	return m_pImageInfo->nImageWidth * m_pImageInfo->nImageHeight * m_pImageInfo->nImageBandWidth * nBitrate;
}

// Helper Function
void CSharedMemWrapper::DeleteMemory()
{
	if(m_pSharedMemory)
	{
		UnmapViewOfFile(m_pSharedMemory); 

		m_pSharedMemory = NULL;
		for (int nCnt = 0; nCnt < MAX_IMAGE_COUNT; nCnt++)
			m_pImageBuffer[nCnt] = NULL;
	}

	if(m_hAsignedMemory)
	{
		CloseHandle(m_hAsignedMemory);
		m_hAsignedMemory = NULL;
	}
}

BOOL CSharedMemWrapper::SetImageInfo(SMemImageInfo *pData)
{
	if(!IsInitial())
		return FALSE;

	if( !pData )
	{
		return FALSE;
		//*m_pImageInfo = SMemImageInfo();
	}
	else
	{
		*m_pImageInfo = *pData;
	}	

	return TRUE;	
}

SMemImageInfo* CSharedMemWrapper::GetImageInfo()
{
	if(!IsInitial())
		return NULL;

	return m_pImageInfo;
}