#pragma once
#pragma pack(push)
#pragma pack(1)		// for Padding bit
//#include <windows.h>

#define MAX_IMAGE_COUNT	20		// 공유메모리 이미저 저장 최대 개수

#define SAFE_DELETE(p)	if(p){ delete (p); (p)=0; }			///< delete 문 재정의, 메모리가 할당 되었을 경우에만 동작됨
#define SAFE_DELETE_ARR(p) if(p){ delete [](p); (p)=0;}		///< delete[] 문 재정의, 메모리가 할당 되었을 경우에만 동작됨

struct SMemImageInfo
{
	SMemImageInfo();		
	SMemImageInfo &operator = (const SMemImageInfo & rhs);

	int		nImageWidth;
	int		nImageHeight;
	int		nImageBitrate;
	int		nImageBandWidth;
	int		nImageCount;
	BYTE	byteReserved[1048576];		// 예약 공간 - 위쪽에 변수 추가 시 크기 계산하여 삭제해주어야함.
};
	
class CSharedMemWrapper
{
public:
	CSharedMemWrapper();
	CSharedMemWrapper(int nImageSizeX, int nImageSizeY, int nImageBitrate, int nImageBandWidth, CString strDrv_CamNo, int nImageCount);
	virtual ~CSharedMemWrapper();

	// Global Function
	BOOL			CreateSharedMem(int nImageSizeX, int nImageSizeY, int nImageBitrate, int nImageBandWidth, CString strDrv_CamNo, int nImageCount);
	BOOL			OpenSharedMem(CString strDrv_CamNo);
	BOOL			IsInitial();

	BYTE*			GetImgAddress(int nImageCount);
	int				GetImgWidth();
	int				GetImgHeight();
	int				GetImgBitrate();
	int				GetImgBandWidth();
	double			GetImgBufSize();

	BOOL			SetImageInfo(SMemImageInfo *pData);
	SMemImageInfo*	GetImageInfo();

	void			DeleteMemory();

private:
	HANDLE			m_hAsignedMemory;
	BYTE*			m_pSharedMemory;

	SMemImageInfo*	m_pImageInfo;
	unsigned char*	m_pImageBuffer[MAX_IMAGE_COUNT];

	BYTE*			GetGlobalMemoryPtr(INT64 &nStartPos, INT64 nReadSize);
};

#pragma pack(pop)