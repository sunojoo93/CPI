#include "stdafx.h"
#include <ppl.h>

#include "NedLineCamera.h"
#include "VSCameraTask.h"
#include "VSNedLineCameraTaskDlg.h"


using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE g_hGrabEnd;

CNedLineCamera::CNedLineCamera()
{
	CString strPath;
	strPath.Format(theApp.GETDRV() + _T(":\\test"));

	m_nImageWidth =  0;
	m_nImageHeight = 0;
	m_nBandWidth = 0;
	m_nBitRate = 0;
	m_nCameraNum = 0;
	g_hGrabEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bLiveMode = FALSE;
	m_dStartTime = 0;
	m_dEndTime = 0;

	UserHookData.hGrabEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	// m_pImage = NULL;

	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX ; i++)
		m_srcImage[i] = NULL;
}

CNedLineCamera::~CNedLineCamera()
{
}

void CNedLineCamera::CloseApp()
{   
	
}

void CNedLineCamera::CameraExpose()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure Start"));
	StopGrab();
	m_GrabTime.Start();
	StartGrab(false);
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure End"));
}

BOOL CNedLineCamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;

	YskimLib *lib = &YskimLib::Instance();
	lib->SetData(_T("inifile"), strInitFilePath);
	CameraNumber = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Camera Number"), lib->GetData(_T("inifile"))));
	DcfFile = lib->GetDataFromINI(_T("Settings"), _T("Init File Path"), lib->GetData(_T("inifile")));
	ImageCount = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("ImageCount"), lib->GetData(_T("inifile"))));
	FrameGrabberNo = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Frame Grabber No"), lib->GetData(_T("inifile"))));
	LinkType = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Link Type"), lib->GetData(_T("inifile"))));
	DigitizerChannel = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Digitizer Channel"), lib->GetData(_T("inifile"))));
	CameraModel = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Camera Model"), lib->GetData(_T("inifile"))));
	CameraPort = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Serial Port"), lib->GetData(_T("inifile"))));
	CameraBaud = _ttoi(lib->GetDataFromINI(_T("Settings"), _T("Serial Baudrate"), lib->GetData(_T("inifile"))));
	TriggerPort = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("Port"), lib->GetData(_T("inifile"))));
	TriggerBaud = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("Baud"), lib->GetData(_T("inifile"))));
	TriggerStartF = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("StartF"), lib->GetData(_T("inifile"))));
	TriggerStartB = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("StartB"), lib->GetData(_T("inifile"))));
	TriggerStopF = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("StopF"), lib->GetData(_T("inifile"))));
	TriggerStopB = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("StopB"), lib->GetData(_T("inifile"))));
	TriggerPeriodF = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("PeriodF"), lib->GetData(_T("inifile"))));
	TriggerPeriodB = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("PeriodB"), lib->GetData(_T("inifile"))));
	TriggerCountF = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("CountF"), lib->GetData(_T("inifile"))));
	TriggerCountB = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("CountB"), lib->GetData(_T("inifile"))));

	m_Camera = new CNedCamControl;
	/*if (LinkType == eCameraLink)
	{
		bRet = OpenCameraComPort(CameraPort, CameraBaud, m_eCamModel);
	}
	else
	{*/
		if (m_Camera->ConnectCameraSerial(CameraPort, CameraBaud) == FALSE)
			AfxMessageBox(_T("Camera Serial Connection Error !!"));
	//}
	

	m_Trigger = new CTriggerControl;
	if (m_Trigger->ConnectTrigger(TriggerPort, TriggerBaud) == FALSE)
		AfxMessageBox(_T("Trigger Serial Connection Error !!"));

	if (!InitGrabber(FrameGrabberNo, DigitizerChannel, DcfFile))
		return FALSE;

	//BOOL bRet = FALSE;

	// Camera 정보
	m_nCameraNum	= GetPrivateProfileInt(_T("Settings"), _T("Camera Number"), 4, strInitFilePath);
	TCHAR szCamType[100] = {0, }, szDefaultGain[5] = {0, };
	GetPrivateProfileString(_T("Settings"), _T("Camera Type"), _T("Coaxial"), szCamType, sizeof(szCamType), strInitFilePath);
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("0.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));

	CString strCamType, strTemp;
	strCamType.Format(_T("%s"), szCamType);

	return bRet;
}

bool CNedLineCamera::InitGrabber(int nGrabberNo, int nDigCh, CString strDcfFile)
{
	// MIL APP - SYSTEM
	MappAlloc(M_DEFAULT, &m_milApplication);
	if (m_milApplication == M_NULL)	return false;
	MsysAlloc(M_SYSTEM_SOLIOS, nGrabberNo, M_DEFAULT, &m_milSystem);
	if (m_milSystem == M_NULL)	return false;
	MdigAlloc(m_milSystem, nDigCh, strDcfFile, M_DEFAULT, &m_milDigitizer);
	if (m_milDigitizer == M_NULL)	return false;

	MappControl(M_ERROR, M_PRINT_DISABLE);

	m_lDigSizeX = (long)MdigInquire(m_milDigitizer, M_SIZE_X, NULL);
	m_lDigSizeY = (long)MdigInquire(m_milDigitizer, M_SIZE_Y, NULL);
	m_lDigBit = MdigInquire(m_milDigitizer, M_SIZE_BIT, NULL);
	m_lDigBand = MdigInquire(m_milDigitizer, M_SIZE_BAND, NULL);
	m_nImageWidth = (int)m_lDigSizeX;
	m_nImageHeight = (int)m_lDigSizeY;
	m_nBitRate = (int)m_lDigBit;
	m_nBandWidth = (int)m_lDigBand;

	//MdigHookFunction(m_milDigitizer, M_GRAB_START, (MIL_DIG_HOOK_FUNCTION_PTR)HookGrabStart, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_END, (MIL_DIG_HOOK_FUNCTION_PTR)HookGrabEnd, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_START, (MIL_DIG_HOOK_FUNCTION_PTR)HookFrameStart, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_END, (MIL_DIG_HOOK_FUNCTION_PTR)HookFrameEnd, this);

	//if (!m_fnInitializeImageBuffer())
	//	return FALSE;

	return true;
}

void CNedLineCamera::StartGrab(bool sync, bool fileSave)
{
	int count = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;
	/*count -= 1;*/
	// 버퍼할당
	AllocClearBuffer(count);
	// UserHookData 초기화
	UserHookData.obj = this;
	UserHookData.isGrabEnd = false;
	UserHookData.lastCount = 0;
	UserHookData.maxCount = count;
	UserHookData.isSaveImage = fileSave;
	ResetEvent(UserHookData.hGrabEnd);
	// 그랩 스타트
	m_GrabFlag = true;
	theApp.m_fnWriteLineScanLog(_T("Grab Start"));
	m_Camera->SetProperty("t", 2);	// 그랩할때 외부 트리거 모드로 변경
	MdigProcess(m_milDigitizer, m_milLineImage, count, M_SEQUENCE + M_COUNT(count), M_ASYNCHRONOUS, ProcessingFunction, &UserHookData);
	if (sync)
	{
		WaitForSingleObject(UserHookData.hGrabEnd, 10000);
	}
}

void CNedLineCamera::StopGrab()
{
	int count = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;
	//count -= 1;
	// 그랩 종료
	theApp.m_fnWriteLineScanLog(_T("Grab Stop"));
	MdigProcess(m_milDigitizer, m_milLineImage, count, M_STOP, M_ASYNCHRONOUS, ProcessingFunction, &UserHookData);
	SetEvent(UserHookData.hGrabEnd);
	m_GrabFlag = false;
}

void CNedLineCamera::AllocClearBuffer(int lineCount, bool onlyClear)
{
	theApp.m_fnWriteLineScanLog(_T("AllocClearBuffer Start"));

	for (int i = 0; i < lineCount; i++)
		BUFFREE(m_milLineImage[i]);
	BUFFREE(m_milMergeImage);

	MbufAllocColor(m_milSystem, m_lDigBand, m_lDigSizeX, m_lDigSizeY * lineCount, m_lDigBit, M_IMAGE + M_GRAB + M_PROC, &m_milMergeImage);
	MbufClear(m_milMergeImage, 0xFF);
	for (int i = 0; i < lineCount; i++)
	{
		MbufChild2d(m_milMergeImage, 0, i * m_lDigSizeY, m_lDigSizeX, m_lDigSizeY, &m_milLineImage[i]);
		MbufClear(m_milLineImage[i], 0xFF);
	}
	theApp.m_fnWriteLineScanLog(_T("AllocClearBuffer End"));
}

MIL_INT CNedLineCamera::ProcessingFunction(MIL_INT HookType, MIL_ID HookId, void MPTYPE *HookDataPtr)
{
	HookDataStruct *UserHookDataPtr = (HookDataStruct*)HookDataPtr;
	MIL_ID ModifiedBufferId;
	MIL_INT ModifiedBufferIndex;
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_ID, &ModifiedBufferId);
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &ModifiedBufferIndex);
	UserHookDataPtr->lastCount = ModifiedBufferIndex + 1;
	theApp.m_fnWriteLineScanLog(_T("ProcessingFunction - %d/%d"), UserHookDataPtr->lastCount, UserHookDataPtr->maxCount);
	if (UserHookDataPtr->maxCount == UserHookDataPtr->lastCount)
	{
		UserHookDataPtr->obj->SetImageCallBackState(0);
		UserHookDataPtr->isGrabEnd = true;
		MdigProcess(UserHookDataPtr->obj->m_milDigitizer, UserHookDataPtr->obj->m_milLineImage, UserHookDataPtr->maxCount, M_STOP, M_ASYNCHRONOUS, ProcessingFunction, &UserHookDataPtr);
		SetEvent(UserHookDataPtr->hGrabEnd);
		theApp.m_fnWriteLineScanLog(_T("Grab End"));
		UserHookDataPtr->obj->m_GrabFlag = false;

		UserHookDataPtr->obj->m_Camera->SetProperty("t", 0);	// 그랩 완료 시 프리런상태로 변경

		// 저장
		if (UserHookDataPtr->isSaveImage)
		{
			struct tm curr_tm;
			time_t curr_time = time(nullptr);
			_localtime64_s(&curr_tm, &curr_time);
			CString filePath;
			filePath.Format(_T("D:\\GrabTest-%4d%02d%02d-%02d%02d%02d.bmp"), curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
			MbufExport(filePath, M_BMP, UserHookDataPtr->obj->m_milMergeImage);
		}
	}
	return 0;
}

int CNedLineCamera::CloseCamera()
{
	m_Trigger->ClosePort();
	m_Camera->m_CamSerial->CloseComPort();
	BUFFREE(m_milMergeImage);
	for (int i = 0; i < BUFFER_COUNT; i++)
		BUFFREE(m_milLineImage[i]);
	MdigFree(m_milDigitizer);
	MsysFree(m_milSystem);
	MappFree(m_milApplication);
	return 1;
}

BOOL CNedLineCamera::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = TRUE;
	m_Camera->SetProperty("i", (int)(dExpOnTime * 1000));
	return bRet;
}

BOOL CNedLineCamera::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;
	return bRet;
}

double CNedLineCamera::GetExposureTime()
{
	return 0 / 1000.0;
}

int CNedLineCamera::GetTriggerMode()
{
	return 0;
}

BOOL CNedLineCamera::SetTriggerMode(int nMode)
{
	return 0;
}

double CNedLineCamera::GetAnalogGain()
{
	return (double) 0;
}

BOOL CNedLineCamera::SetAnalogGain(double dGainValue)
{
	BOOL bRet = FALSE;
	return bRet;
}

void CNedLineCamera::DisplayLive(HWND hDispWnd)
{

}

void CNedLineCamera::WaitGrabEnd()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd Start"));
	WaitForSingleObject(UserHookData.hGrabEnd, 10000);
	ResetEvent(UserHookData.hGrabEnd);
	m_GrabTime.End();	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd End"));
}

void CNedLineCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{
	if (m_lDigBand == 1)
	{
		int line = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;
		line -= 1;
		MbufGet2d(m_milMergeImage, 0, 0, m_lDigSizeX, m_lDigSizeY * line, theApp.m_pSharedMemory->GetImgAddress(nGrabNum));
	}
	else if (m_lDigBand == 3)
	{
		int line = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;
		line -= 1;
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth() * line;
		byte* Image = new byte[nBufferSize];
		MbufGetColor(m_milMergeImage, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), Image, nBufferSize);
		SAFE_DELETE_ARR(Image);
	}
}

void CNedLineCamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	m_fnMbufExport(strSavePath, m_milMergeImage);
}

BOOL CNedLineCamera::StartLiveGrab()
{
	m_bLiveMode = TRUE;
	theApp.m_fnWriteTactLog(_T("Start LiveGrab"));


	//SetSequenceMode(eNone);
	//SetTriggerMode(eFreeRun);
	//Sleep(10);

	//m_GrabTime.Start();

	//// Serial 통신 오류로 인해 Camera Mode 변경되지 않았을 경우 연속 Grab 호출 -> 사용 안함
	//if (GetTriggerMode() != eFreeRun)
	//{
	//	AfxMessageBox(_T("Trigger Mode Error !!"));
	//	bRet = FALSE;
	//	// 		m_bTriggerLive = TRUE;
	//	// 		m_fnSnapAndTrigger();
	//}
	//else
	//{
	//	m_bFreeRunLive = TRUE;
	//	MdigGrabContinuous(m_milDigitizer, m_LiveImage);
	//	bRet = TRUE;
	//}


	return TRUE;
}

BOOL CNedLineCamera::StopLiveGrab()
{
	m_bLiveMode = FALSE;
	theApp.m_fnWriteTactLog(_T("StopLiveGrab"));
	return TRUE;
}

int CNedLineCamera::GetLivePixelInfo(CPoint ptBuf)
{
	return 0;
}

void CNedLineCamera::SaveGrabImage(CString strFilePath)
{
}

void CNedLineCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
}

BYTE* CNedLineCamera::GetGrabBuffer()
{
	return 0;
}

BOOL CNedLineCamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	return TRUE;
}

void CNedLineCamera::GetGrabImage(byte* byteImgArr)
{
}

void CNedLineCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	ResetEvent(g_hGrabEnd);
	if(bIsLiveImage)
	{
		

		//Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
		//cv::resize(matLive(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);
		//
		//GlobalFree(pRGBBuffer);
	}	
	else
	{		
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	}
		
	if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

	memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
}

double CNedLineCamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
{
	if( matBuf.empty() )	return 0;

	cv::Rect rectROI( rect.left, rect.top, rect.Width(), rect.Height() );

	// 영역 자르기
	cv::Mat matTempBuf = matBuf( rectROI );

	// 평균 구하기
	cv::Scalar m;
	m = cv::mean( matTempBuf );

	// 메모리 할당 해제
	matTempBuf.release();

	return (double)m[0];
}

void CNedLineCamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
{	
	// 노출 시간 초기값
	int nExp = 5;
	int nExpStep = 200;
	const int nMaxExp = 601;

	// 카메라 게인 초기값
	int nGain = 30;
	int nGainStep = 30;
	const int nMaxGain = 300;	

	SetExposureTime(nMaxExp);

	// Gain 조절 Loop
	while (nGain <= nMaxGain)
	{
		SetAnalogGain(nGain);

	}

	// 최대치보다 게인값이 높은 경우, 만족하는 셋팅 없음 ( Exp는 최대 상태 )
	if (nGain > nMaxGain)	return;

	SetAnalogGain(nGain);

	// Exp 조절 Loop ( 0보다 큰 경우만 )
	while (nExpStep > 0)
	{
		SetExposureTime(nExp + nExpStep);

	}

	// 결과
	nOutExp = nExp;
	nOutGain = nGain;
}

void CNedLineCamera::DisplayGrabImage(HWND hDispWnd)
{
	GetGrabBuffer();
}

void CNedLineCamera::TestFunc()
{

}

void CNedLineCamera::m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);

	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void CNedLineCamera::CalcWhiteBalance(float* WhiteBalance)
{

}

void CNedLineCamera::SetWhiteBalance(float* WhiteBalance)
{	
	
}

void CNedLineCamera::GetWhiteBalance(float* WhiteBalance)
{
	
}

UINT CNedLineCamera::retryConnect()
{
	BOOL bRet = FALSE;

	CString strLog;
	//Internal Server 시작		
	
	CloseCamera();
	Sleep(3000);
	InitializeCamera(theApp.GetInitFilePath());

	strLog.Format(_T("Camrea reconnect"));
	theApp.m_pLogWriter->m_fnWriteLog(strLog);	
	printf("ThreadConnectState End \n");

	return 0;
}

BOOL CNedLineCamera::OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
{
	BOOL bRet = FALSE;
	m_eCamModel = eModel;
	//bRet = m_SerialCamera.ConnectCameraSerial(nComPort, nBaudrate, eModel);

	return bRet;
}

void CNedLineCamera::m_fnMbufExport(CString strFilePath, MIL_ID milBuffer)
{
	// 파일 경로 생성
	SHCreateDirectoryEx(NULL, strFilePath.Left(strFilePath.GetLength() - (strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')))), NULL);

	if (m_lDigBit == 8)
	{
		MbufExport(strFilePath, M_BMP, milBuffer);
	}
	else
	{
		MbufExport(strFilePath, M_TIFF, milBuffer);
	}
}