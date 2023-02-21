// Digitizer.h: interface for the CMatroxCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
#define AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_

#include <mil.h>
#include "SerialCtrl.h"
#include "CameraInterface.h"
#include "CPUTimer.h"
#include "define.h"
#include "..\..\CommonHeaders\Structure.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define BUFFREE(milBuffer)		if(milBuffer)	{ MbufFree	(milBuffer);	milBuffer=M_NULL;	}

// 16.03.16 pixel shift 기능 추가
#define MAX_BUFFER_NUMBER	MAX_IMAGE_RATIO * MAX_IMAGE_RATIO
#define MAX_CAM_PULSE_TIME	2000

long MFTYPE HookGrabStart(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookGrabEnd(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookFrameStart(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookFrameEnd(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookTimerStart(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookTimerEnd(long HookType, MIL_ID EventId, void *UserDataPtr);
MIL_INT MFTYPE ProcessingFunction(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr);

// for test0627
const int TESTOFFSET = 100;
const int TESTCROPSIZE = 100;


class CMatroxCamera : public CameraInterface
{
public:
	CMatroxCamera();
	virtual ~CMatroxCamera();

	// virtual //////////////////////////////////////////////////////////////////////////////////
	// Init
	BOOL			InitializeCamera(CString strInitFilePath);
	BOOL			InitializeGrabber(int nGrabberNo, eBoardModel eGrabberModel, int nDigCh, CString strDcfFilePath);
	BOOL			OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel);	
	// Grab
	void			CameraExpose();									// Exposure Time 동안만 대기 후 반환
	void			WaitGrabEnd();									// Wait Image Grab End	
	BOOL			DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle);
	void			GetGrabImage(byte* byteImgArr);
	void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	void			SaveGrabImage(CString strFilePath);	
	void			SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath);
	void			DisplayGrabImage(HWND hDispWnd = NULL);	
	void			DisplayLive(HWND hDispWnd = NULL);
	BOOL			StartLiveGrab();			// 16.06.30 Live 화면 출력 추가 by CWH
	BOOL			StopLiveGrab();
	void			SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode);			// 현재 Grab Image Shared Memory 에 적재
	void			SaveFileCurBuffer(TCHAR* strSavePath);		// 현재 Grab Image 파일로 저장
	
	// Get
	int				GetImageWidth()									{ return (int)m_lDigSizeX;	}
	int				GetImageHeight()								{ return (int)m_lDigSizeY;	}
	int				GetImageBitrate()								{ return (int)m_lDigBit;	}
	int				GetImageBandwidth()								{ return (int)m_lDigBand;	}
	int				GetImageSize()									{ return (int)m_lDigSizeX * (int)m_lDigSizeY * (int)m_lDigBand * (int)(m_lDigBit==8?1:2);	}

	double			GetExposureTime();
	double			GetAnalogGain();								// 16.05.10 Camera Analog Gain 값 설정	
	int				GetTriggerMode();								// Camera Trigger Mode 반환 - 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	int				GetSequenceMode();								// Camera Sequence Mode 반환 - 0:Disable 1:4-shot 2:9-shot 3~:Not used
	int				GetDataBit();									// Output Data Bit - 8, 10, 12
	int				GetExposureSource();							// Exposure Source - 1:Timed(Camera) 2:Pulse Width
	int				GetTriggerSource();								// Trigger Source - 1:CC1(Camera Link) 2:External

	// Camera 상태 모니터링 기능 추가
	bool			GetFanStatus();									// Fan Status - 0:Off 1:On
	int				GetFanRPM();									// Fan RPM
	double			GetTemperature();								// Current Temperature
	double			GetCCDTemperature();							// Current CCD Temperature
	int				GetMonitoringInterval()							{	return m_nInterval			;};	
	CString			GetInitFilePath()								{	return m_strInitPath		;};	

	// Set
	BOOL			SetExposureTime(double nExpOnTime);				// Camera Exposure Time 변경 
	BOOL			SetStitchMode(int nCamMode);					// Cam Stitch Mode  180809 YSS
	BOOL			SetAnalogGain(double dGainValue);
	BOOL			SetTriggerMode(int nMode);
	BOOL			SetSequenceMode(int nMode);						// Pixel Shift 기능	
	BOOL			SetDataBit(int nMode);							// Output Data Bit
	BOOL			SetExposureSource(int nMode);					// Exposure Source
	BOOL			SetTriggerSource(int nMode);					// Trigger Source

	void			TestFunc( int nGV );
	void 			m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, int& nChCnt, int &nWidth, int &nHeight, double dAngle);
	double			GetMeanValue(MIL_ID milBuf, CRect rect);
	void			GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain );

	int				GetImageCallBackState()							{return 0;}
	void			SetImageCallBackState(int nState);
	UINT			retryConnect()									{return 0;}
	// Camera 상태 모니터링 기능 추가
	void			SetMonitoringInterval(int nInterval)			{	m_nInterval = nInterval		;};
	void			SetInitFilePath(CString strPath)				{	m_strInitPath = strPath		;};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL			IsDigitigerConnected();							// Camera 연결 여부
	BOOL			IsSystemConnected();							// Grab Board 연결 여부	

	void			CloseApp();
	void			CloseGrabber();									// Mil System
	void			CloseDigitizer();
	void			OpenFeatureDialog();
	
	// Get func...
	MIL_ID			GetSystem()										{ return m_milSystem;		}
	MIL_ID			GetDigitizer()									{ return m_milDigitizer;	}
	int				GetBufHoriRatio()								{ return m_nHRatio;			}
	int				GetBufVertRatio()								{ return m_nVRatio;			}
	CSerialCtrl		GetSerialCtrl()									{ return m_SerialCamera;		}
	eCamModel		GetCameraModel()								{ return m_eCamModel;		}	
	BOOL			GetUseColorBayer()								{ return m_bUseColorBayer;		}	
	MIL_ID			GetMilGrabBuffer();
	void			GetBufferBGR24(void* DestImg, MIL_ID SrcImg);
	void			GetWBCoefficient(float* pCoefficients);
	// 16.07		.19 Live 화면 UI 추가 by CWH
	MIL_ID			GetDisplayID()									{ return m_milDisplay;	}
	MIL_ID			GetLiveGrabImage();
	//MIL_ID 			GetManualImage(int nImgCnt, BOOL bIsGrayImg);
	int				GetLivePixelInfo(CPoint ptBuf);
	double			GetRegionMeanValue(MIL_ID milImage, const CRect rcROI);
	MIL_ID			ConvertYUVtoGRBG(MIL_ID SrcImg);

	// Set func...
	// Camera 통신 Interface 추가
	void			SetWBCoefficient(float* pCoefficients);		// WhiteBalance
	void			SetCameraModel(eCamModel eCamModel)	{ m_eCamModel = eCamModel; }	

	void			SetDisplayImage(MIL_ID milImage, HWND hDispWnd = NULL);
	void			ChangeDisplayMap(HWND hDispWnd, MIL_ID milImage = M_NULL);

	// ETC
	void			m_fnSnapAndTrigger();							// 16.07.04 Live Grab 함수	
	void			StartDisplayMap(HWND hDispWnd = NULL);	
	/// 설비별 - Camera별 별도 Gain 값 필요 - 광학팀 요청 사항		181014 YSS
	void			SetDefaultGainOffset(double dGainOffset) { m_dDefaultGainOffset = dGainOffset; };
	double			GetDefaultGainOffset() { return m_dDefaultGainOffset; };


	// Variable
	BOOL			m_bIsMultiShot;
	UINT			m_nCurFrameNumber;	

	CCPUTimer		m_GrabTime;

	HANDLE			m_hExpEnd[MAX_BUFFER_NUMBER];		// Exposure End 시점 Event Handle

	int				m_nHookCount;
	int				m_nHookGrabStart;
	int				m_nHookGrabEnd;
	
	// 16.07.04 Live 사용 여부
	BOOL			m_bTriggerLive;
	BOOL			m_bFreeRunLive;

	MIL_ID			m_MilWBCoefficients;			// White Balance 보정치

	MIL_ID			m_milCropImage;

	MIL_ID			m_LiveImage;
	MIL_ID			m_milLiveGrabBuffer;
	//17.01.09 GUI 호출 이미지 MIL 버퍼
// 	MIL_ID			m_milLoadImgBuff[NUM_LOAD_IMAGE_MAX];
// 	MIL_ID			m_milLoadClrImgBuff[NUM_LOAD_IMAGE_MAX];
 	MIL_ID			m_milCropLoadClrImg;
// 	MIL_ID			m_milCropLoadImg;
private:
	// Member Func.
	BOOL			m_fnInitializeDigitizer(const CString& strDCF, int nDigChannel);		
	BOOL			m_fnPrepareGrabBuffer();
	BOOL			m_fnPrepareGrabBuffer(int nImgWidth, int nImgHeight, int nDigBit, int nDigBand);
	void			m_fnGetPixelShiftData();	
	BOOL			m_fnInitializeImageBuffer();
	void			m_fnClearImageBuffer();		// 16.03.23 Grab 할 때마다 Buffer 재할당하는 불합리 수정

	// 2017.02.20 MIL-Lite 에서 Board Memory 직접 접근 시 문제 발생으로 인해 Byte Buffer 생성하여 복사 후 사용하도록 수정
	//            Buffer 를 Byte[] 에
	// Make total image - for pixel shift
	//void			m_fnPixelPosSetMono(unsigned short int *data,  unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	//void			m_fnPixelPosSetMono8Bit(BYTE *data, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset,UINT32 VOffset);	
	void			m_fnPixelPosSetMono(MIL_ID milSrc, unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	void			m_fnPixelPosSetMono8Bit(MIL_ID milSrc, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset,UINT32 VOffset);	
	// 16.03.16 Camera TASK 분리 준비 작업 - End		<<
	void			m_fnFreeImageBuffer();	
	// MbufExport Wrapper - 상위 폴더 생성
	void			m_fnMbufExport(CString strFilePath, MIL_ID milBuffer);


	// ETC.
	// 16.05.16 Multishot Image Buffer 초기화 for Tact
	static UINT		ThreadClearMultishotBuffers(LPVOID lParam);


	// Member Var.
	MIL_ID			m_milApplication;
	MIL_ID			m_milSystem; // MIL System
	CString			m_strDcfPath;

	MIL_INT			m_lDigSizeX;
	MIL_INT			m_lDigSizeY;
	MIL_INT			m_lDigBit;
	MIL_INT			m_lDigBand;

	// 16.03.16 Camera TASK 분리 준비 작업 - Start		>>
	MIL_ID			m_milDigitizer;					// Solios MIL Digitizer used
	MIL_ID			m_OdImage[MAX_BUFFER_NUMBER];	// Solios MIL Image used	// Add [MAX_BUFFER_NUMBER] by CWH
	MIL_ID			m_ColorImage;
	// 2017.02.20 MIL-Lite 에서 Board Memory 직접 접근 시 문제 발생으로 인해 Byte Buffer 생성하여 복사 후 사용하도록 수정
	//Total image buffer and each image buffer
 	//unsigned short*	m_parFrameBuf[MAX_BUFFER_NUMBER];
	unsigned short*	m_totalImageBuf;
 	//BYTE*			m_parFrameBuf8Bit[MAX_BUFFER_NUMBER];
	BYTE*			m_totalImageBuf8Bit;

	// 16.03.16 Camera 개수 관련 추가 - Camera TASK 적용 by CWH	
	CSerialCtrl		m_SerialCamera;	// camera serial communication
	int				m_nSequenceMode;
	BOOL			m_bUseHPS, m_bUseVPS, m_bUseColorBayer;
	
	// 2016.03.31 P/S 기능 확장 - H, V 선택 기능
	//UINT			m_nRatio;
	UINT			m_nHRatio;		// Horizontal
	UINT			m_nVRatio;		// Vertical
	UINT			m_nMaxBuf;	
	// 16.05.16 카메라 현재 정보 저장
	UINT			m_nCurHRatio;		// Horizontal
	UINT			m_nCurVRatio;		// Vertical
	UINT			m_nCurMaxBuf;	
	int				m_nCurSeqMode;

	UINT			m_stage_y_pos;
	UINT			m_stage_x_pos;
	UINT			m_PixelPitch;	

	// 16.03.16 카메라 TASK 분리 준비 작업 by CWH		
	MIL_ID			m_milImageLarge[MAX_IMAGE_RATIO];					// pixel shift 최종 buffer
	UINT32			m_BufLen;	

	// 16.05.16 Multishot Image Buffer 초기화 for Tact
	CWinThread*     m_pThrClrMulBuf;
	HANDLE			m_hPreparePixelShift;	
	
	eLinkType		m_eLinkType;		// 0 : Serial, 1 : CoaXPress
	eCamModel		m_eCamModel;		// Camera Model 명

	// 16.06.30 Live 화면 출력 추가 by CWH
	MIL_ID			m_milDisplay;	
	HWND			m_hLiveWnd;			// 16.07.04 Live Window Handle		
	MIL_ID			m_milDispImage;	
	MIL_ID			m_milGrabBuffer;		// 현재 Grab Buffer
	MIL_ID			m_milTEST;

	cv::Mat			m_srcImage[NUM_LOAD_IMAGE_MAX];

	int				m_nInterval;
	CString			m_strInitPath;

	// 2020.11.13 MDJ 151M Camera 대응 코드 수정
	int				m_nInspPCType;

	
	double				m_dDefaultGainOffset;		// 설비별 카메라 별도 Gain 값		181014 YSS
};

#endif // !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
