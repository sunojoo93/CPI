// Digitizer.h: interface for the CDalsaCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
#define AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_

#include "CameraInterface.h"
#include "CPUTimer.h"
#include "define.h"
#include "..\..\CommonHeaders\Structure.h"
#include "SapClassBasic.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Dalsa 4-SHOT
#define GRAB_NUM 4
#define PROCESSING_NUM 4
#define MAX_BUFFER_NUMBER	MAX_IMAGE_RATIO * MAX_IMAGE_RATIO

//Thread STRUCT
typedef struct
{
	unsigned short *Data_Grab[4]; //4���� ���� point
	unsigned short *Data_PS;
	HANDLE Event;
	int ThreadNum;
	int SwitchNum;
	bool b_Thread;
	bool b_Proc;
	bool b_UsePS;
	int SizeX;
	int SizeY;
}da;

typedef struct
{
	//	unsigned short *Data_Grab[4]; //4���� ���� point
	//	unsigned short *Data_PS;
	HANDLE Event;
	//	int ThreadNum;
	int SwitchNum;
	bool b_Thread;
	bool b_Proc;
	bool b_UsePS;
	SapView	*View;
	SapView	*View_PS;
	int SizeX;
	int SizeY;
	ThreadStruct *ThreadValue[PROCESSING_NUM];
	int *LastDoneProcess;
	//	bool b_Save;
}ThreadControlStruct;

class CDalsaCamera : public CameraInterface
{
public:
	CDalsaCamera();
	virtual ~CDalsaCamera();

	// virtual //////////////////////////////////////////////////////////////////////////////////
	// Init
	BOOL			InitializeCamera(CString strInitFilePath);
	BOOL			InitializeGrabber(int nGrabberNo, int nDigCh, CString strDcfFilePath);
	BOOL			OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel);
	// Grab
	void			CameraExpose();									// Exposure Time ���ȸ� ��� �� ��ȯ
	void			WaitGrabEnd();									// Wait Image Grab End
	void			GetGrabImage(byte* byteImgArr);
	void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	//void			GetGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY);
	void			SaveGrabImage(CString strFilePath);
	void			SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath);
	void			DisplayGrabImage(HWND hDispWnd = NULL);
	void			DisplayLive(HWND hDispWnd = NULL);
	BOOL			StartLiveGrab();			// 16.06.30 Live ȭ�� ��� �߰� by CWH
	BOOL			StopLiveGrab();
	void			SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode);			// ���� Grab Image Shared Memory �� ����
	void			SaveFileCurBuffer(TCHAR* strSavePath);		// ���� Grab Image ���Ϸ� ����

																// Get
	int				GetImageWidth() { return (int)m_lDigSizeX*(int)(m_nSequenceMode == 0 ? 1 : 2); }
	int				GetImageHeight() { return (int)m_lDigSizeY*(int)(m_nSequenceMode == 0 ? 1 : 2); }
	int				GetImageBitrate() { return (int)m_lDigBit; }
	int				GetImageBandwidth() { return (int)m_lDigBand; }
	int				GetImageSize() { return (int)m_lDigSizeX * (int)m_lDigSizeY * (int)m_lDigBand * (int)(m_lDigBit == 8 ? 1 : 2); }

	double			GetExposureTime();
	double			GetAnalogGain();								// 16.05.10 Camera Analog Gain �� ����	
	int				GetTriggerMode();								// Camera Trigger Mode ��ȯ - 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
	int				GetSequenceMode();								// Camera Sequence Mode ��ȯ - 0:Disable 1:4-shot 2:9-shot 3~:Not used
	int				GetDataBit();									// Output Data Bit - 8, 10, 12
	int				GetExposureSource();							// Exposure Source - 0:Timed(Camera) 1:Pulse Width

																	// Set
	BOOL			SetExposureTime(double nExpOnTime);				// Camera Exposure Time ����
	BOOL			SetStitchMode(int nCamMode);					// Cam Stitch Mode  180809 YSS
	int				SetAnalogGain(double nGainValue);
	BOOL			SetTriggerMode(int nMode);
	BOOL			SetSequenceMode(int nMode);						// Pixel Shift ���	
	BOOL			SetDataBit(int nMode);							// Output Data Bit
	BOOL			SetExposureSource(int nMode);					// Exposure Source

	void			TestFunc(int nGV);
	void 			m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt/*, byte* imageBub*/, CRect crect, BOOL bIsGrayImg/*, int nBufSize*/);
	double			GetMeanValue(cv::Mat &matBuf, CRect rect);
	void			GetBrightnessVal(int nWantMean, CRect rect, int& nOutExp, int& nOutGain);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int				GetBufHoriRatio() { return m_nHRatio; }
	int				GetBufVertRatio() { return m_nVRatio; }
	int				GetLivePixelInfo(CPoint ptBuf);

	// Variable
	BOOL			m_bFreeRunLive;

	CCPUTimer		m_GrabTime;

	SapAcquisition	*Acq;
	SapFeature		*feature;
	SapBuffer		*Buffer_Grab; //0~3 First 4~7 Second
	SapBuffer		*Buffer_PS; //0 First 1 Second
	SapView			*View, *View_PS;
	SapTransfer		*Xfer;
	SapManager		*m_SapManager;								 //SapManager����
	static void XferCallback(SapXferCallbackInfo *pInfo);
	static void AcqCallback(SapAcqCallbackInfo *pInfo);
	int SizeX, SizeY;
	int LastDoneProcess;
	ThreadStruct TreadValue[2][PROCESSING_NUM];
	ThreadControlStruct ThreadControlValue[2];
	CWinThread *m_ThreadProc[2][PROCESSING_NUM];
	CWinThread *m_ThreadControl[2];
		*m_AcqDevice;

private:
	// Member Func.
	//BOOL			m_fnInitializeDigitizer();
	//BOOL			m_fnPrepareGrabBuffer();
	//BOOL			m_fnPrepareGrabBuffer(int nImgWidth, int nImgHeight, int nDigBit, int nDigBand);
	//void			m_fnGetPixelShiftData();
	//BOOL			m_fnInitializeImageBuffer();
	//void			m_fnClearImageBuffer();		// 16.03.23 Grab �� ������ Buffer ���Ҵ��ϴ� ���ո� ����

	//											// 2017.02.20 MIL-Lite ���� Board Memory ���� ���� �� ���� �߻����� ���� Byte Buffer �����Ͽ� ���� �� ����ϵ��� ����
	//											//            Buffer �� Byte[] ��
	//											// Make total image - for pixel shift
	//											//void			m_fnPixelPosSetMono(unsigned short int *data,  unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	//											//void			m_fnPixelPosSetMono8Bit(BYTE *data, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset,UINT32 VOffset);	
	//void			m_fnPixelPosSetMono(MIL_ID milSrc, unsigned short int *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	//void			m_fnPixelPosSetMono8Bit(MIL_ID milSrc, BYTE *Odata, UINT32 HLen, UINT32 VLen, BYTE HRatio, BYTE VRatio, UINT32 HOffset, UINT32 VOffset);
	//// 16.03.16 Camera TASK �и� �غ� �۾� - End		<<
	//void			m_fnFreeImageBuffer();
	//// MbufExport Wrapper - ���� ���� ����
	//void			m_fnMbufExport(CString strFilePath, MIL_ID milBuffer);
	//void			Clear(); //Sapera Buff Clear()	

	//						 // ETC.
	//						 // 16.05.16 Multishot Image Buffer �ʱ�ȭ for Tact
	//static UINT		ThreadClearMultishotBuffers(LPVOID lParam);


	//// Member Var.
	//MIL_ID			m_milApplication;
	//MIL_ID			m_milSystem; // MIL System
	//CString			m_strDcfPath;

	int			m_lDigSizeX;
	int			m_lDigSizeY;
	int			m_lDigBit;
	int			m_lDigBand;

	//// 16.03.16 Camera TASK �и� �غ� �۾� - Start		>>
	//MIL_ID			m_milDigitizer;					// Solios MIL Digitizer used
	//MIL_ID			m_OdImage[MAX_BUFFER_NUMBER];	// Solios MIL Image used	// Add [MAX_BUFFER_NUMBER] by CWH
	//MIL_ID			m_ColorImage;
	//MIL_ID			m_MilWBCoefficients;			// White Balance ����ġ
	//												// 2017.02.20 MIL-Lite ���� Board Memory ���� ���� �� ���� �߻����� ���� Byte Buffer �����Ͽ� ���� �� ����ϵ��� ����
	//												//Total image buffer and each image buffer
	//												//unsigned short*	m_parFrameBuf[MAX_BUFFER_NUMBER];
	//unsigned short*	m_totalImageBuf;
	////BYTE*			m_parFrameBuf8Bit[MAX_BUFFER_NUMBER];
	//BYTE*			m_totalImageBuf8Bit;

	//// 16.03.16 Camera ���� ���� �߰� - Camera TASK ���� by CWH	
	////CCamCtrl		m_InspCamera;	// camera serial communication
	int				m_nSequenceMode;
	BOOL			m_bUseHPS, m_bUseVPS;

	//// 2016.03.31 P/S ��� Ȯ�� - H, V ���� ���
	////UINT			m_nRatio;
	UINT			m_nHRatio;		// Horizontal
	UINT			m_nVRatio;		// Vertical
	//UINT			m_nMaxBuf;
	//// 16.05.16 ī�޶� ���� ���� ����
	//UINT			m_nCurHRatio;		// Horizontal
	//UINT			m_nCurVRatio;		// Vertical
	//UINT			m_nCurMaxBuf;
	//int				m_nCurSeqMode;

	//UINT			m_stage_y_pos;
	//UINT			m_stage_x_pos;
	//UINT			m_PixelPitch;

	//// 16.03.16 ī�޶� TASK �и� �غ� �۾� by CWH		
	//MIL_ID			m_milImageLarge[MAX_IMAGE_RATIO];					// pixel shift ���� buffer
	//UINT32			m_BufLen;

	//// 16.05.16 Multishot Image Buffer �ʱ�ȭ for Tact
	//CWinThread*     m_pThrClrMulBuf;
	//HANDLE			m_hPreparePixelShift;

	//eCamModel	m_eCamModel;		// Camera Model ��

	//									// 16.06.30 Live ȭ�� ��� �߰� by CWH
	//MIL_ID			m_milDisplay;

	//HWND			m_hLiveWnd;			// 16.07.04 Live Window Handle	

	//MIL_ID			m_milDispImage;

	//MIL_ID			m_milGrabBuffer;		// ���� Grab Buffer

	//MIL_ID			m_milTEST;

	//cv::Mat m_srcImage[NUM_LOAD_IMAGE_MAX];
};
UINT ThreadProc(LPVOID lParam);
UINT ThreadControl(LPVOID lParam);
#endif // !defined(AFX_DIGITIZER_H__EFEFFA6C_DB06_4177_9826_24EBE108BAF7__INCLUDED_)
