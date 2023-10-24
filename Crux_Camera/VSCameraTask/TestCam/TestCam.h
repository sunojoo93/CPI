// Digitizer.h: interface for the CTestCam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
#define AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_

#include "CameraInterface.h"
#include "CPUTimer.h"
#include "define.h"
#include "..\..\CommonHeaders\Structure.h"
#include "afxwin.h"
#include <mil.h>
#include "DalsaLineCamControl.h"
#include "TriggerControl.h"
#include "DalsaLineParam.hpp"
#include "YskimLib.hpp"
#include "SerialCtrl.h"


#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//KYH
#define BUFFER_COUNT	200
#define BUFFREE(milBuffer)		if(milBuffer == 0) { MbufFree	(milBuffer);	milBuffer=M_NULL;	/*MbufClear(milBuffer,0);*/}
long MFTYPE HookFrameStart(long HookType, MIL_ID EventId, void *UserDataPtr);
long MFTYPE HookFrameEnd(long HookType, MIL_ID EventId, void *UserDataPtr);
class CTestCam : public CameraInterface
{
public:
	CTestCam();
   ~CTestCam();

	// virtual //////////////////////////////////////////////////////////////////////////////////
	// Init
	
	BOOL			InitializeCamera(CString strInitFilePath);
	BOOL			InitializeGrabber(int nGrabberNo, int nDigCh, CString strDcfFilePath);
	BOOL			OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel);	
	// Grab
	void			CameraExpose(CString PanelID, CString VirID, CString Position, int nBufCnt, BOOL BSpi, BOOL bDctS);									// Exposure Time 동안만 대기 후 반환
	int				WaitGrabEnd(int proc_num);									// Wait Image Grab End	
	BYTE*			GetGrabBuffer();
	BOOL			DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle);
	void			GetGrabImage(byte* byteImgArr);
	void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	void			SaveGrabImage(CString strFilePath);	
	void			SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath);
	void			DisplayGrabImage(HWND hDispWnd = NULL);	
	void			DisplayLive(HWND hDispWnd = NULL);
	BOOL			StartLiveGrab();			// 16.06.30 Live 화면 출력 추가 by CWH
	BOOL			StopLiveGrab();
	void			SetSMemCurBuffer(int nTriggerCountF, int nTriggerCountB, UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode);			// 현재 Grab Image Shared Memory 에 적재
	int				SetSMemCurBuffer(int nBufCnt,TCHAR* strPanelID);			// 현재 Grab Image Shared Memory 에 적재
	void			SaveFileCurBuffer(TCHAR* strSavePath);			// 현재 Grab Image 파일로 저장
	//int				StopGrab(int nBufCnt);
	// Get

	int				GetImageSize()									{ return 0  ;}

	double			GetExposureTime();
	double			GetAnalogGain();								// 16.05.10 Camera Analog Gain 값 설정	
	int				GetTriggerMode();								// Camera Trigger Mode 반환 - 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	BOOL			GetLiveMode()									{ return	m_bFreeRunLive	;}				
	//CSerialCtrl		GetSerialCtrl()                                 { return    m_SerialCamera  ;}
	int				GetSequenceMode()								{ return 0					;};
	int				GetDataBit()									{ return 8					;};

	// Set
	BOOL			SetExposureTime(double nExpOnTime);				// Camera Exposure Time 변경
	BOOL			SetStitchMode(int nCamMode);						// Cam Stitch Mode  180809 YSS
	BOOL			SetAnalogGain(double dGainValue);
	BOOL			SetTriggerMode(int nMode);
	void			SetImgaeBandWidth(int nBandWidth);
	BOOL			SetSequenceMode(int nSeqMode);
		BOOL			SetSequenceIndex(int nSeqIdx);

	BOOL			SetDataBit(int nDataBit)								{	return TRUE		;};

	void			TestFunc();
	void 			m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, int& nChCnt, int &nWidth, int &nHeight, double dAngle);

	double			GetMeanValue(cv::Mat &matBuf, CRect rect);
	void			GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain );

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int				CloseCamera();
	int			    freeCamera();

	void			SetImageCallBackState(int nState)			{m_ImgCallbackState = nState;}
	int				GetImageCallBackState()						{return m_ImgCallbackState;}	
	UINT			retryConnect();
	void			CloseApp();

	eCamModel		GetCameraModel()							{ return m_eCamModel;	   }
	void			GetWBCoefficient(float* pCoefficients);
	int				GetLivePixelInfo(CPoint ptBuf);

	// Set func...
	// Camera 통신 Interface 추가
	void			CalcWhiteBalance(float* WhiteBalance);		// WhiteBalance
	void			SetWhiteBalance(float* WhiteBalance);
	void			GetWhiteBalance(float* WhiteBalance);
	void			SetCameraModel(eCamModel eCamModel)	{ m_eCamModel = eCamModel; }

	int				GetImageWidth() { return	m_lDigSizeX; }
	int				GetImageHeight() { return	m_lDigSizeY; }
	int				GetImageBitrate() { return	m_lDigBit; }
	int				GetImageBandwidth() { return	m_lDigBand; }

	CString			GetCameraName();
	CString			GetBoardName();
	CString			GetCameraType();
	int				GetCameraWidth();
	int				GetCameraHeight();
	int				GetCameraDepth();
	double			GetCameraTemperature();

	int				SetCamSequencerProperty(ST_GRAB_AREA_INFO_AOT* data);
	int				ApplyProperty(ST_GRAB_CAMERA_VALUE_SET_AOT data, int seq_idx);
	//MIL_ID			GetMilGrabBuffer();
	MIL_ID			GetLiveGrabImage();

	//void			SetDisplayImage(MIL_ID milImage, HWND hDispWnd = NULL);
	//void			ChangeDisplayMap(HWND hDispWnd, MIL_ID milImage = M_NULL);

	// ETC
	void			StartDisplayMap(HWND hDispWnd = NULL);
	/// 설비별 - Camera별 별도 Gain 값 필요 - 광학팀 요청 사항
	void			SetDefaultGainOffset(double dGainOffset)		{	m_dDefaultGainOffset = dGainOffset	;};
	double			GetDefaultGainOffset()							{	return m_dDefaultGainOffset			;};

	CCPUTimer		m_GrabTime;

	
	// 16.07.04 Live 사용 여부
	BOOL			m_bFreeRunLive = FALSE;
	BOOL			m_bTriggerLive = FALSE;
	BOOL			m_bLiveMode;

	double m_dStartTime;
	double m_dEndTime;

	LONGLONG m_i64Freq;
	LONGLONG m_i64Start;
	LONGLONG m_i64End;

	LONGLONG m_i64Start2;
	LONGLONG m_i64End2;

	int g_nHookCount = 0;
	int g_nHookGrabStart = 0;
	int g_nHookGrabEnd = 0;

	//HANDLE g_hExpEnd[MAX_BUFFER_NUMBER] = { NULL, };		// Exposure End 시점

#pragma region Trigger 관련
	CTriggerControl* m_Trigger;
	int CameraNumber;
	CString DcfFile;
	int ImageCount;
	int FrameGrabberNo;
	int LinkType;
	int DigitizerChannel;
	int CameraModel;
	int CameraPort;
	int CameraBaud;
	int TriggerPort;
	int TriggerBaud;
	int TriggerStartF;
	int TriggerStartB;
	int TriggerStopF;
	int TriggerStopB;
	int TriggerPeriodF;
	int TriggerPeriodB;
	int TriggerCountF;
	int TriggerCountB;
	int imageY;
#pragma endregion

#pragma region DalsaLine Camera 접근용
	typedef struct
	{
		// Mil System, Mil Digitizer 접근 가능
		CTestCam*	obj;
		CString VirID;
		CString PanelID;
		int		MaxCount;
		int		ProcessedImageCount;
		bool	isGrabEnd;
		bool	isSaveParticleImage;
		HANDLE	hGrabEnd;
		CString SavePath;
		CString Position;
		bool DirectSave;

	} HookDataStruct;

	CDalsaLineCamControl* m_Camera;
	MIL_ID			m_milApplication;
	MIL_ID			m_milSystem;
	MIL_ID			m_milDigitizer;
	MIL_ID			m_milDispImage;
	MIL_ID			m_milMergeImage;
	MIL_ID			m_milLineImage[BUFFER_COUNT];
	MIL_ID			m_milLineImageBack[BUFFER_COUNT];
	MIL_INT			m_lDigSizeX;
	MIL_INT			m_lDigSizeY;
	MIL_INT			m_lDigBit;
	MIL_INT			m_lDigBand;
	HookDataStruct	UserHookData;
	bool	m_GrabFlag = false;
	bool InitGrabber(int nGrabberNo, int nDigCh, CString strDcfFile);
	void StartGrab(int nTriggerCountF, int nTriggerCountB, CString strpos ,bool sync, bool fileSave = false);
	void StartGrab(CString PanelID, CString VirID, CString Position, int nBufCnt, bool sync, bool fileSave = false, bool directSave = false);
	void StopGrab(int nTriggerCountF, int nTriggerCountB);
	int StopGrab(int nBufCnt);
	void AllocClearBuffer(int nBufCnt, bool onlyClear = false);
	void m_fnMbufExport(CString strFilePath, MIL_ID milBuffer);
	static MIL_INT ProcessingFunction(MIL_INT HookType, MIL_ID HookId, void *HookDataPtr);
	MIL_ID			m_LiveImage;
	MIL_ID			m_milLiveGrabBuffer;
	MIL_ID			m_milCropImage;
	MIL_ID			m_milDisplay;
	MIL_ID			m_ColorImage;
	MIL_ID			m_milCropLoadClrImg;
	MIL_ID			m_MilWBCoefficients;			// White Balance 보정치


	BOOL			m_fnInitializeImageBuffer();
	BOOL			m_fnPrepareGrabBuffer();


	//bool m_bTriggerLive;
#pragma endregion

private:

	unsigned short*	m_totalImageBuf;
	BYTE*			m_totalImageBuf8Bit;
	cv::Mat m_srcImage[NUM_LOAD_IMAGE_MAX];

	eCamModel	m_eCamModel;		// Camera Model 명

	//CSerialCtrl		m_SerialCamera;	// camera serial communication
	int m_nImageWidth;
	int m_nImageHeight;
	int m_nBandWidth;
	int m_nBitRate;
	
	int							m_nCameraNum;
	
	int m_ImgCallbackState;
	CWinThread*			m_pCamStateThread;
	BOOL				m_bCamStateThreadFlg;		

	double				m_dDefaultGainOffset;
};

#endif // !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
