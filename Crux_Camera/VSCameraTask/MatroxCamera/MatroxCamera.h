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

// 16.03.16 pixel shift ��� �߰�
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
	void			CameraExpose();									// Exposure Time ���ȸ� ��� �� ��ȯ
	void			WaitGrabEnd();									// Wait Image Grab End	
	BOOL			DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle);
	void			GetGrabImage(byte* byteImgArr);
	void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	void			SaveGrabImage(CString strFilePath);	
	void			SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath);
	void			DisplayGrabImage(HWND hDispWnd = NULL);	
	void			DisplayLive(HWND hDispWnd = NULL);
	BOOL			StartLiveGrab();			// 16.06.30 Live ȭ�� ��� �߰� by CWH
	BOOL			StopLiveGrab();
	void			SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode);			// ���� Grab Image Shared Memory �� ����
	void			SaveFileCurBuffer(TCHAR* strSavePath);		// ���� Grab Image ���Ϸ� ����
	
	// Get
	int				GetImageWidth()									{ return (int)m_lDigSizeX;	}
	int				GetImageHeight()								{ return (int)m_lDigSizeY;	}
	int				GetImageBitrate()								{ return (int)m_lDigBit;	}
	int				GetImageBandwidth()								{ return (int)m_lDigBand;	}
	int				GetImageSize()									{ return (int)m_lDigSizeX * (int)m_lDigSizeY * (int)m_lDigBand * (int)(m_lDigBit==8?1:2);	}

	double			GetExposureTime();
	double			GetAnalogGain();								// 16.05.10 Camera Analog Gain �� ����	
	int				GetTriggerMode();								// Camera Trigger Mode ��ȯ - 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	int				GetSequenceMode();								// Camera Sequence Mode ��ȯ - 0:Disable 1:4-shot 2:9-shot 3~:Not used
	int				GetDataBit();									// Output Data Bit - 8, 10, 12
	int				GetExposureSource();							// Exposure Source - 1:Timed(Camera) 2:Pulse Width
	int				GetTriggerSource();								// Trigger Source - 1:CC1(Camera Link) 2:External

	// Camera ���� ����͸� ��� �߰�
	bool			GetFanStatus();									// Fan Status - 0:Off 1:On
	int				GetFanRPM();									// Fan RPM
	double			GetTemperature();								// Current Temperature
	double			GetCCDTemperature();							// Current CCD Temperature
	int				GetMonitoringInterval()							{	return m_nInterval			;};	
	CString			GetInitFilePath()								{	return m_strInitPath		;};	

	// Set
	BOOL			SetExposureTime(double nExpOnTime);				// Camera Exposure Time ���� 
	BOOL			SetStitchMode(int nCamMode);					// Cam Stitch Mode  180809 YSS
	BOOL			SetAnalogGain(double dGainValue);
	BOOL			SetTriggerMode(int nMode);
	BOOL			SetSequenceMode(int nMode);						// Pixel Shift ���	
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
	// Camera ���� ����͸� ��� �߰�
	void			SetMonitoringInterval(int nInterval)			{	m_nInterval = nInterval		;};
	void			SetInitFilePath(CString strPath)				{	m_strInitPath = strPath		;};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL			IsDigitigerConnected();							// Camera ���� ����
	BOOL			IsSystemConnected();							// Grab Board ���� ����	

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
	// 16.07		.19 Live ȭ�� UI �߰� by CWH
	MIL_ID			GetDisplayID()									{ return m_milDisplay;	}
	MIL_ID			GetLiveGrabImage();
	//MIL_ID 			GetManualImage(int nImgCnt, BOOL bIsGrayImg);
	int				GetLivePixelInfo(CPoint ptBuf);
	double			GetRegionMeanValue(MIL_ID milImage, const CRect rcROI);
	MIL_ID			ConvertYUVtoGRBG(MIL_ID SrcImg);

	// Set func...
	// Camera ��� Interface �߰�
	void			SetWBCoefficient(float* pCoefficients);		// WhiteBalance
	void			SetCameraModel(eCamModel eCamModel)	{ m_eCamModel = eCamModel; }	

	void			SetDisplayImage(MIL_ID milImage, HWND hDispWnd = NULL);
	void			ChangeDisplayMap(HWND hDispWnd, MIL_ID milImage = M_NULL);

	// ETC
	void			m_fnSnapAndTrigger();							// 16.07.04 Live Grab �Լ�	
	void			StartDisplayMap(HWND hDispWnd = NULL);	
	/// ���� - Camera�� ���� Gain �� �ʿ� - ������ ��û ����		181014 YSS
	void			SetDefaultGainOffset(double dGainOffset) { m_dDefaultGainOffset = dGainOffset; };
	double			GetDefaultGainOffset() { return m_dDefaultGainOffset; };


	// Variable
	BOOL			m_bIsMultiShot;
	UINT			m_nCurFrameNumber;	

	CCPUTimer		m_GrabTime;

	HANDLE			m_hExpEnd[MAX_BUFFER_NUMBER];		// Exposure End ���� Event Handle

	int				m_nHookCount;
	int				m_nHookGrabStart;
	int				m_nHookGrabEnd;
	
	// 16.07.04 Live ��� ����
	BOOL			m_bTriggerLive;
	BOOL			m_bFreeRunLive;

	MIL_ID			m_MilWBCoefficients;			// White Balance ����ġ

	MIL_ID			m_milCropImage;

	MIL_ID			m_LiveImage;
	MIL_ID			m_milLiveGrabBuffer;
	//17.01.09 GUI ȣ�� �̹��� MIL ����
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
	void			m_fnClearImageBuffer();		// 16.03.23 Grab �� ������ Buffer ���Ҵ��ϴ� ���ո� ����

	// 2017.02.20 MIL-Lite ���� Board Memory ���� ���� �� ���� �߻����� ���� Byte Buffer �����Ͽ� ���� �� ����ϵ��� ����
	//            Buffer �� Byte[] ��
	// Make total image - for pixel shift
	//void			m_fnPixelPosSetMono(unsigned short int *data,  unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	//void			m_fnPixelPosSetMono8Bit(BYTE *data, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset,UINT32 VOffset);	
	void			m_fnPixelPosSetMono(MIL_ID milSrc, unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	void			m_fnPixelPosSetMono8Bit(MIL_ID milSrc, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset,UINT32 VOffset);	
	// 16.03.16 Camera TASK �и� �غ� �۾� - End		<<
	void			m_fnFreeImageBuffer();	
	// MbufExport Wrapper - ���� ���� ����
	void			m_fnMbufExport(CString strFilePath, MIL_ID milBuffer);


	// ETC.
	// 16.05.16 Multishot Image Buffer �ʱ�ȭ for Tact
	static UINT		ThreadClearMultishotBuffers(LPVOID lParam);


	// Member Var.
	MIL_ID			m_milApplication;
	MIL_ID			m_milSystem; // MIL System
	CString			m_strDcfPath;

	MIL_INT			m_lDigSizeX;
	MIL_INT			m_lDigSizeY;
	MIL_INT			m_lDigBit;
	MIL_INT			m_lDigBand;

	// 16.03.16 Camera TASK �и� �غ� �۾� - Start		>>
	MIL_ID			m_milDigitizer;					// Solios MIL Digitizer used
	MIL_ID			m_OdImage[MAX_BUFFER_NUMBER];	// Solios MIL Image used	// Add [MAX_BUFFER_NUMBER] by CWH
	MIL_ID			m_ColorImage;
	// 2017.02.20 MIL-Lite ���� Board Memory ���� ���� �� ���� �߻����� ���� Byte Buffer �����Ͽ� ���� �� ����ϵ��� ����
	//Total image buffer and each image buffer
 	//unsigned short*	m_parFrameBuf[MAX_BUFFER_NUMBER];
	unsigned short*	m_totalImageBuf;
 	//BYTE*			m_parFrameBuf8Bit[MAX_BUFFER_NUMBER];
	BYTE*			m_totalImageBuf8Bit;

	// 16.03.16 Camera ���� ���� �߰� - Camera TASK ���� by CWH	
	CSerialCtrl		m_SerialCamera;	// camera serial communication
	int				m_nSequenceMode;
	BOOL			m_bUseHPS, m_bUseVPS, m_bUseColorBayer;
	
	// 2016.03.31 P/S ��� Ȯ�� - H, V ���� ���
	//UINT			m_nRatio;
	UINT			m_nHRatio;		// Horizontal
	UINT			m_nVRatio;		// Vertical
	UINT			m_nMaxBuf;	
	// 16.05.16 ī�޶� ���� ���� ����
	UINT			m_nCurHRatio;		// Horizontal
	UINT			m_nCurVRatio;		// Vertical
	UINT			m_nCurMaxBuf;	
	int				m_nCurSeqMode;

	UINT			m_stage_y_pos;
	UINT			m_stage_x_pos;
	UINT			m_PixelPitch;	

	// 16.03.16 ī�޶� TASK �и� �غ� �۾� by CWH		
	MIL_ID			m_milImageLarge[MAX_IMAGE_RATIO];					// pixel shift ���� buffer
	UINT32			m_BufLen;	

	// 16.05.16 Multishot Image Buffer �ʱ�ȭ for Tact
	CWinThread*     m_pThrClrMulBuf;
	HANDLE			m_hPreparePixelShift;	
	
	eLinkType		m_eLinkType;		// 0 : Serial, 1 : CoaXPress
	eCamModel		m_eCamModel;		// Camera Model ��

	// 16.06.30 Live ȭ�� ��� �߰� by CWH
	MIL_ID			m_milDisplay;	
	HWND			m_hLiveWnd;			// 16.07.04 Live Window Handle		
	MIL_ID			m_milDispImage;	
	MIL_ID			m_milGrabBuffer;		// ���� Grab Buffer
	MIL_ID			m_milTEST;

	cv::Mat			m_srcImage[NUM_LOAD_IMAGE_MAX];

	int				m_nInterval;
	CString			m_strInitPath;

	// 2020.11.13 MDJ 151M Camera ���� �ڵ� ����
	int				m_nInspPCType;

	
	double				m_dDefaultGainOffset;		// ���� ī�޶� ���� Gain ��		181014 YSS
};

#endif // !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
