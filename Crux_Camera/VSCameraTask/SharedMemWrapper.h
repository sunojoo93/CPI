#pragma once
#pragma pack(push)
#pragma pack(1)		// for Padding bit
//#include <windows.h>

#define MAX_IMAGE_COUNT	20		// �����޸� �̹��� ���� �ִ� ����

#define SAFE_DELETE(p)	if(p){ delete (p); (p)=0; }			///< delete �� ������, �޸𸮰� �Ҵ� �Ǿ��� ��쿡�� ���۵�
#define SAFE_DELETE_ARR(p) if(p){ delete [](p); (p)=0;}		///< delete[] �� ������, �޸𸮰� �Ҵ� �Ǿ��� ��쿡�� ���۵�

struct SMemImageInfo
{
	SMemImageInfo();		
	SMemImageInfo &operator = (const SMemImageInfo & rhs);

	int		nImageWidth;
	int		nImageHeight;
	int		nImageBitrate;
	int		nImageBandWidth;
	int		nImageCount;
	BYTE	byteReserved[1048576];		// ���� ���� - ���ʿ� ���� �߰� �� ũ�� ����Ͽ� �������־����.
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