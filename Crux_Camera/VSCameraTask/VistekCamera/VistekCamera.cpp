// Digitizer.cpp: implementation of the CVistekCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "VistekCamera.h"
#include "VSCameraTask.h"


using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE g_hExpEnd;
HANDLE g_hGrabEnd;		// Exposure End 시점
StreamingChannel_handle g_StreamingChannel;

SVGigE_RETURN __stdcall CamEventCallback(Event_handle EventID, void* Context)
{
	CVistekCamera *pData = (CVistekCamera*)Context;
	Message_handle MessageID;
	SVGigE_SIGNAL_TYPE MessageType =  SVGigE_SIGNAL_NONE;
	int nNum = 0;
	double dTransferTime = 0.0;
	//theApp.m_pCamera->
	Stream_getMessage(g_StreamingChannel,EventID,&MessageID,&MessageType);
	CString strLog;

	switch(MessageType)
	{
	case SVGigE_SIGNAL_END_OF_EXPOSURE:
		Stream_getMessageTimestamp(g_StreamingChannel,EventID,MessageID,&pData->m_dStartTime);
		if(!pData->m_bLiveMode)
			SetEvent(g_hExpEnd);
		strLog.Format(_T("Exposure Set Event"));
		theApp.m_pLogWriter->m_fnWriteLog(strLog);
		break;
	case SVGigE_SIGNAL_FRAME_COMPLETED:
		Stream_getMessageTimestamp(g_StreamingChannel,EventID,MessageID,&pData->m_dEndTime);

		dTransferTime = (float)(pData->m_dEndTime - pData->m_dStartTime)*1000;
		strLog.Format(_T("SVGigE_SIGNAL_FRAME_COMPLETED - %.3f"),dTransferTime);
		theApp.m_pLogWriter->m_fnWriteLog(strLog);
	default:
break;
	}
	Stream_releaseMessage(g_StreamingChannel, EventID, MessageID);
	return SVGigE_SUCCESS;
}

SVGigE_RETURN __stdcall ImageCallback(Image_handle Image, void* Context)
{

	CVistekCamera *pData = (CVistekCamera*)Context;
	CString strLog;
	if (pData->m_bLiveMode)
		Camera_softwareTrigger(pData->m_InspCamera);

	pData->SetImageCallBackState(0);

	unsigned char *ImageData = Image_getDataPointer(Image);
	if (ImageData == NULL)
	{
		SVGigE_SIGNAL_TYPE sgType = Image_getSignalType(Image);
		int nSgType = sgType;
		if (sgType == SVGigE_SIGNAL_CAMERA_CONNECTION_LOST)
			pData->SetImageCallBackState(nSgType);

		strLog.Format(_T("Image Callback Data Null, %d"), sgType);
		theApp.m_pLogWriter->m_fnWriteLog(strLog);
		return SVGigE_ERROR;
	}
	if (Image != 0)
		pData->m_Image = Image;
	if (!pData->m_bLiveMode)
		SetEvent(g_hGrabEnd);

	SVGigE_SIGNAL_TYPE sgType = Image_getSignalType(Image);

	strLog.Format(_T("Image Callback Complete"));
	theApp.m_pLogWriter->m_fnWriteLog(strLog);

	Image_release(Image);
	return SVGigE_SUCCESS;
}


CVistekCamera::CVistekCamera()
{
	CString strPath;
	strPath.Format(theApp.GETDRV() + _T(":\\test"));

	m_Image = 0;
	m_nImageWidth = 0;
	m_nImageHeight = 0;
	m_nBandWidth = 0;
	m_nBitRate = 0;
	m_nCameraNum = 0;
	g_hExpEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_hGrabEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bLiveMode = FALSE;
	m_dStartTime = 0;
	m_dEndTime = 0;

	for (int i = 0; i < NUM_LOAD_IMAGE_MAX; i++)
		m_srcImage[i] = NULL;

	// 	m_bCamStateThreadFlg = TRUE;
	// 	m_pCamStateThread = AfxBeginThread(ThreadConnectState, this);

}

CVistekCamera::~CVistekCamera()
{
	m_bCamStateThreadFlg = FALSE;
	WaitForSingleObject(m_pCamStateThread, INFINITE);

	Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_STOP);
	Stream_unregisterEventCallback(g_StreamingChannel, m_EventID, &CamEventCallback);
	Stream_closeEvent(g_StreamingChannel, m_EventID);
	m_EventID = SVGigE_NO_EVENT;

	StreamingChannel_delete(g_StreamingChannel);
	g_StreamingChannel = SVGigE_NO_STREAMING_CHANNEL;
	Camera_closeConnection(m_InspCamera);
}


void CVistekCamera::CloseApp()
{

}

////////////////////////////////////////////////////////////
// 16.03.16 Camera TASK 분리 준비 작업
// pixel shift 기능 추가
void CVistekCamera::CameraExpose()
{
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure Start"));
	ResetEvent(g_hExpEnd);
	Camera_softwareTrigger(m_InspCamera);

	WaitForSingleObject(g_hExpEnd, INFINITE);

	if (WaitForSingleObject(g_hGrabEnd, 500) == WAIT_OBJECT_0)
	{

	}
	ResetEvent(g_hGrabEnd);

	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure End"));
}


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.


BOOL CVistekCamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;
	SVGigE_RETURN result;

	// Camera 정보
	m_nCameraNum	= GetPrivateProfileInt(_T("Settings"), _T("Camera Number"), 4, strInitFilePath);
	TCHAR szCamType[100] = {0, }, szDefaultGain[5] = {0, };
	GetPrivateProfileString(_T("Settings"), _T("Camera Type"), _T("Coaxial"), szCamType, sizeof(szCamType), strInitFilePath);
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("0.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));

	CameraClient = CameraContainer_create(SVGigETL_TypeFilter); 
	CameraContainer_discovery(CameraClient);
	int nCameras = CameraContainer_getNumberOfCameras(CameraClient);
	CString strCamType,strTemp;
	strCamType.Format(_T("%s"),szCamType);

	for(int i = 0 ; i < nCameras ; i++)
	{
		m_InspCamera = CameraContainer_getCamera(CameraClient, i);
		strTemp = Camera_getUserDefinedName(m_InspCamera);
		
		if( !strCamType.CompareNoCase(strTemp) )
		{
			result = Camera_openConnection(m_InspCamera, 3.0);
			break;
		}

	}
	int nPacketSize = 0;
	 Camera_evaluateMaximalPacketSize(m_InspCamera, &nPacketSize);
	 Camera_setStreamingPacketSize(m_InspCamera,nPacketSize);
	 CString strLog;
	 strLog.Format(_T("Init Cam - Packet Size = %d"), nPacketSize);
	 theApp.m_fnWriteTactLog(strLog);

	result = StreamingChannel_create (&g_StreamingChannel, CameraClient, m_InspCamera, BUFFER_COUNT, &ImageCallback, this);
         
	result = Stream_createEvent(g_StreamingChannel,&m_EventID,100);    
	if( SVGigE_SUCCESS != Stream_addMessageType(g_StreamingChannel,m_EventID,SVGigE_SIGNAL_END_OF_EXPOSURE) )
	{
		Stream_closeEvent(g_StreamingChannel,m_EventID);
		return FALSE; 
	}
	if( SVGigE_SUCCESS != Stream_addMessageType(g_StreamingChannel,m_EventID,SVGigE_SIGNAL_FRAME_COMPLETED) )
	{
		Stream_closeEvent(g_StreamingChannel,m_EventID);
		return FALSE; 
	}
	
	result = Stream_registerEventCallback(g_StreamingChannel,m_EventID, &CamEventCallback, this);
	Camera_getImagerWidth(m_InspCamera, &m_nImageWidth);
	Camera_getImagerHeight(m_InspCamera, &m_nImageHeight);
	m_nBandWidth = 3;
	m_nBitRate = 8;


	float Framerate = 10;
	Camera_setFrameRate(m_InspCamera, Framerate);
	Camera_setAcquisitionMode(m_InspCamera, ACQUISITION_MODE_INT_TRIGGER);
	Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_START);
	if(result == SVGigE_SUCCESS) bRet = TRUE;
	else						bRet = FALSE;

	SetImageCallBackState(SVGigE_SUCCESS);
	return bRet;
}


int CVistekCamera::CloseCamera()
{
	Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_STOP);
	Stream_unregisterEventCallback(g_StreamingChannel,m_EventID, &CamEventCallback);
	Stream_closeEvent(g_StreamingChannel, m_EventID);
	m_EventID = SVGigE_NO_EVENT;

	StreamingChannel_delete(g_StreamingChannel);
	g_StreamingChannel = SVGigE_NO_STREAMING_CHANNEL;
	Camera_closeConnection(m_InspCamera);

	return 1;
}

BOOL CVistekCamera::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = FALSE;
	SVGigE_RETURN result;

	result = Camera_setExposureTime(m_InspCamera, (float)dExpOnTime * 1000);
	
	if(result == SVGigE_SUCCESS) bRet = TRUE;
	else						bRet = FALSE;
	return bRet;
}
// Stitch	180809 YSS
BOOL CVistekCamera::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;

	return bRet;
}

double CVistekCamera::GetExposureTime()
{
	float ExposeTime;
	Camera_getExposureTime(m_InspCamera,&ExposeTime);
	return ExposeTime/1000;
}


int CVistekCamera::GetTriggerMode()
{
	return 0;
}

BOOL CVistekCamera::SetTriggerMode(int nMode)
{

	return 0;
}

double CVistekCamera::GetAnalogGain()
{
	float gain;
	Camera_getGain(m_InspCamera,&gain);
	return (double)gain;
}

BOOL CVistekCamera::SetAnalogGain(double dGainValue)
{
	BOOL bRet;
	SVGigE_RETURN result;
	// 2019.10.28 MDJ CSOT Site 광학팀 요청에 의하여 패턴 관계 없이 Gain값 OffSet 값으로 대체 
	// 기본 Gain값 더해서 적용 - 설비별 Camera별 편차 보정 - 광학팀 요청 기능
	//result = Camera_setGain(m_InspCamera, (float)(dGainValue + GetDefaultGainOffset()));
	result = Camera_setGain(m_InspCamera, GetDefaultGainOffset());
	if(result == SVGigE_SUCCESS) bRet = TRUE;
	else						bRet = FALSE;
	return bRet;
}

void CVistekCamera::DisplayLive(HWND hDispWnd)
{

}
void CVistekCamera::WaitGrabEnd()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd Start"));
	/*WaitForSingleObject(g_hGrabEnd, 5000);	
	ResetEvent(g_hGrabEnd);*/
	m_GrabTime.End();	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd End"));
}

void CVistekCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{	
	int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();

	unsigned char* rgbImage = (unsigned char *)GlobalAlloc(GMEM_FIXED, nBufferSize);
	SVGigE_RETURN result = Image_getImageRGB(m_Image, rgbImage, nBufferSize, BAYER_METHOD_HQLINEAR);
	memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), rgbImage, nBufferSize);
	GlobalFree(rgbImage);

}

void CVistekCamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	char filename[200] = {0,};
	//strcpy(filename, (char*)strSavePath);
	int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();

	int nLen = WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, NULL, 0, NULL, NULL);       

	WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, filename, nLen, NULL, NULL);

	CString strFilePath;
	strFilePath.Format(_T("%s"),strSavePath);
	SHCreateDirectoryEx(NULL, strFilePath.Left(strFilePath.GetLength() - (strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')))), NULL);
	unsigned char* rgbImage = (unsigned char *)GlobalAlloc(GMEM_FIXED, nBufferSize);
	SVGigE_RETURN result = Image_getImageRGB(m_Image, rgbImage, nBufferSize, BAYER_METHOD_HQLINEAR);
	SVGigE_writeImageToBitmapFile(filename, rgbImage, GetImageWidth(), GetImageHeight(), GVSP_PIX_RGB24);
	GlobalFree(rgbImage);
}

// 16.07.04 Live 화면 추가
BOOL CVistekCamera::StartLiveGrab()
{
	BOOL bRet = FALSE;
	bRet = Camera_softwareTrigger(m_InspCamera) == SVGigE_SUCCESS ? TRUE : FALSE;
	m_bLiveMode = TRUE;
	theApp.m_fnWriteTactLog(_T("Start LiveGrab"));

	return bRet;
}

BOOL CVistekCamera::StopLiveGrab()
{
	theApp.m_fnWriteTactLog(_T("StopLiveGrab"));
	m_bLiveMode = FALSE;
	WaitForSingleObject(g_hExpEnd, 1000);
	theApp.m_fnWriteTactLog(_T("WaitForSingleObject exp"));
	WaitForSingleObject(g_hGrabEnd, 1000);
	theApp.m_fnWriteTactLog(_T("WaitForSingleObject grab end"));

	
	ResetEvent(g_hExpEnd);
	ResetEvent(g_hGrabEnd);
	theApp.m_fnWriteTactLog(_T("StopLiveGrab"));

	return TRUE;
}


int CVistekCamera::GetLivePixelInfo(CPoint ptBuf)
{
	//int nVal;

	return 0;
}

void CVistekCamera::SaveGrabImage(CString strFilePath)
{
	int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* rgbImage = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);

	SVGigE_RETURN result = Image_getImageRGB(m_Image, rgbImage, nBufferSize, BAYER_METHOD_HQLINEAR);
	char* filename = CSTR2PCH(strFilePath);
	SVGigE_writeImageToBitmapFile(filename, rgbImage, GetImageWidth(), GetImageHeight(), GVSP_PIX_RGB24);
	SAFE_DELETE_ARR(filename);	
	
	GlobalFree(rgbImage);

}

// 2017.05.11 add by ikm GUI -> Image SAVE 용 - S
void CVistekCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	
	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	char* filename = CSTR2PCH(strFilePath);

	if(bIsLiveImage)
	{		
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* rgbImage = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);

		SVGigE_RETURN result = Image_getImageRGB(m_Image, rgbImage, nBufferSize, BAYER_METHOD_HQLINEAR);
		//char* filename = CSTR2PCH(strFilePath);
		SVGigE_writeImageToBitmapFile(filename, rgbImage, GetImageWidth(), GetImageHeight(), GVSP_PIX_RGB24);
		//SAFE_DELETE_ARR(filename);	

		GlobalFree(rgbImage);
	}

	else
	{			
		cv::imwrite(filename, m_srcImage[nImgCnt](rect));
	}
	SAFE_DELETE_ARR(filename);
}
// 2017.05.11 add by ikm GUI -> Image SAVE 용 - E

Image_handle CVistekCamera::GetGrabBuffer()
{
	//ResetEvent(g_hGrabEnd);
	CameraExpose();

	WaitGrabEnd();

	return m_Image;
}

BOOL CVistekCamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	if( matSrcBuffer.empty() )
		return FALSE;

	cv::Mat matTempBuffer = cv::Mat::zeros(matSrcBuffer.size(), CV_8UC1);
	cv::Mat matRotation = cv::getRotationMatrix2D( Point(matSrcBuffer.cols/2, matSrcBuffer.rows/2), dAngle, 1.0 );

	cv::warpAffine( matSrcBuffer, matTempBuffer, matRotation, matTempBuffer.size() );

	if( !matDstBuffer.empty() )
		matDstBuffer.release();

	matDstBuffer = matTempBuffer.clone();

	matTempBuffer.release();
	matRotation.release();

	return TRUE;
}

// 원본 이미지
void CVistekCamera::GetGrabImage(byte* byteImgArr)
{

	int nSizeX = Image_getSizeX(m_Image);
	int nSizeY = Image_getSizeY(m_Image);
	int nImageID = Image_getImageID(m_Image);


	SVGigE_RETURN result = Image_getImageRGB(m_Image, byteImgArr, nSizeX * nSizeY * 3, BAYER_METHOD_HQLINEAR);
}

// Crop Image
void CVistekCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	IplImage *reSizeImage = NULL;
	IplImage *cropImage = NULL;

	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	if(bIsLiveImage)
	{
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
		SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
		Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
		cv::resize( matLive(rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
		GlobalFree(Image);
	}	
	else
	{		
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	}
		
	if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

	memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
}

double CVistekCamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
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

// 입력값 : 원하는 평균 밝기 & 밝기를 맞추고자 하는 영역
// 출력값 : 노출시간 & 게인
void CVistekCamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
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

		// Grab OpenCV 인 경우 라이브 영상
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
		SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
		
		GlobalFree(Image);
		//cv::Mat matBuf = m_liveImage;// = GetMilGrabBuffer();

		// 평균 GV값 계산
		double dMean = GetMeanValue(matBuf, rect);

		// 현재 평균GV가 원하는 평균 GV보다 낮은 경우, Gain 올려서 다시 Loop ( Exp는 최대 상태 )
		// GV가 딱 맞는 경우도 있을수 있음 ( 2GV 정도 더 남겨 둠 )
		if (nWantMean + 2 > dMean)
			nGain += nGainStep;
		// 만족 하는 경우 Loop 나가기
		else
			break;
	}

	// 최대치보다 게인값이 높은 경우, 만족하는 셋팅 없음 ( Exp는 최대 상태 )
	if (nGain > nMaxGain)	return;

	SetAnalogGain(nGain);

	// Exp 조절 Loop ( 0보다 큰 경우만 )
	while (nExpStep > 0)
	{
		SetExposureTime(nExp + nExpStep);

		// Grab OpenCV 인 경우 라이브 영상
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
		SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);

		// 평균 GV값 계산
		double dMean = GetMeanValue(matBuf, rect);

		// 원하는 평균 밝기값과 현재 평균 밝기값 비교
		if (nWantMean > dMean)
			nExp += nExpStep;
		// Step 반으로 해서 다시 시도
		else 
			nExpStep /= 2;
	}

	// 결과
	nOutExp = nExp;
	nOutGain = nGain;
}

void CVistekCamera::DisplayGrabImage(HWND hDispWnd)
{
	Image_handle Image;
	Image = GetGrabBuffer();

}

void CVistekCamera::TestFunc()
{

}

// 2017.01.09 add by ikm GUI 이미지 로드 시 MIL 버퍼 저장 (color, gray image 구분) - S  
void CVistekCamera::m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);

	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void CVistekCamera::CalcWhiteBalance(float* WhiteBalance)
{

	Camera_setAcquisitionMode(m_InspCamera, ACQUISITION_MODE_INT_TRIGGER);
	Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_START);
	Camera_softwareTrigger(m_InspCamera);
	//ResetEvent(g_hExpEnd);
	WaitForSingleObject(g_hGrabEnd,INFINITE);
	Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_STOP);
	int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
	SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 

	float R;
	float G;
	float B;

	if( SVGigE_SUCCESS == SVGigE_estimateWhiteBalance(Image, nBufferSize, &R, &G, &B) )
	{
		WhiteBalance[0] = R;
		WhiteBalance[1] = G;
		WhiteBalance[2] = B;
	}
	
	
	GlobalFree(Image);

}

void CVistekCamera::SetWhiteBalance(float* WhiteBalance)
{	
	Camera_setWhiteBalance(m_InspCamera,WhiteBalance[0], WhiteBalance[1], WhiteBalance[2]);
}

void CVistekCamera::GetWhiteBalance(float* WhiteBalance)
{
	Camera_getWhiteBalance(m_InspCamera,&WhiteBalance[0],&WhiteBalance[1],&WhiteBalance[2]);
}

UINT CVistekCamera::retryConnect()
{
	BOOL bRet = FALSE;

	CString strLog;
	//Internal Server 시작		
	
	CloseCamera();
	Sleep(3000);
	InitializeCamera(theApp.GetInitFilePath());

	strLog.Format(_T("Camrea reconnect, %d"),SVGigE_SIGNAL_CAMERA_CONNECTION_LOST);
	theApp.m_pLogWriter->m_fnWriteLog(strLog);	
	printf("ThreadConnectState End \n");

	return 0;
}