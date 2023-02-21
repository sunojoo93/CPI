// Digitizer.cpp: implementation of the CHikCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "HikCamera.h"
#include "VSCameraTask.h"
#include "MvCamera.h"


using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE g_hExpEnd;
HANDLE g_hGrabEnd;		// Exposure End 시점


//StreamingChannel_handle g_StreamingChannel;

/*Get Image buffer function, you can get the chunk infomation from frame infomation*/
void __stdcall ImageCallBack(unsigned char * pSrcBuf, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	CHikCamera *pData = (CHikCamera*)pUser;
	CString strLog;

	if (pData->m_bLiveMode)
		pData->m_pcMyCamera->CommandExecute("TriggerSoftware");

	pData->SetImageCallBackState(0);

	if (pFrameInfo)
	{
		pData->m_pSaveImageBuf = (unsigned char *)malloc(sizeof(unsigned char) * pFrameInfo->nFrameLen);
		
		pData->m_nSaveImageBufSize = pFrameInfo->nFrameLen;

		memcpy(pData->m_pSaveImageBuf, pSrcBuf, pFrameInfo->nFrameLen);
		memcpy(&pData->m_stImageInfo, pFrameInfo, sizeof(MV_FRAME_OUT_INFO_EX));

		pData->SetImageCallBackState(0);

		if (!pData->m_bLiveMode)
		{
			SetEvent(g_hExpEnd);
			SetEvent(g_hGrabEnd);
		}
	}
	else
	{
		pData->SetImageCallBackState(1);
	}

	strLog.Format(_T("Image Callback Complete"));
	theApp.m_pLogWriter->m_fnWriteLog(strLog);
}

// 이벤트 콜백 처리 필요해 보임
void __stdcall EventCallBack(MV_EVENT_OUT_INFO * pEventInfo, void* pUser)
{
	if (pEventInfo)
	{
		__int64 nBlockId = pEventInfo->nBlockIdHigh;
		nBlockId = (nBlockId << 32) + pEventInfo->nBlockIdLow;

		__int64 nTimestamp = pEventInfo->nTimestampHigh;
		nTimestamp = (nTimestamp << 32) + pEventInfo->nTimestampLow;

		//printf("EventName[%s], EventID[%u], BlockId[%I64d], Timestamp[%I64d]\n",
		//	pEventInfo->EventName, pEventInfo->nEventID, nBlockId, nTimestamp);
	}
}

CHikCamera::CHikCamera()
{
	CString strPath;
	strPath.Format(theApp.GETDRV() + _T(":\\test"));

	m_nImageWidth =  0;
	m_nImageHeight = 0;
	m_nBandWidth = 0;
	m_nBitRate = 0;
	m_nCameraNum = 0;
	g_hExpEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_hGrabEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bLiveMode = FALSE;
	m_dStartTime = 0;
	m_dEndTime = 0;

	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX ; i++)
		m_srcImage[i] = NULL;

	m_pcMyCamera = new CMvCamera();

}

CHikCamera::~CHikCamera()
{
	m_bCamStateThreadFlg = FALSE;
	WaitForSingleObject(m_pCamStateThread, INFINITE);
	CloseCamera();
}


void CHikCamera::CloseApp()
{   
	CloseCamera();
}

////////////////////////////////////////////////////////////
// 16.03.16 Camera TASK 분리 준비 작업
// pixel shift 기능 추가
void CHikCamera::CameraExpose()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure Start"));
	ResetEvent(g_hExpEnd);
	
	m_pcMyCamera->CommandExecute("TriggerSoftware");
	
	WaitForSingleObject(g_hExpEnd,INFINITE);
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure End"));
}


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.


BOOL CHikCamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;
	int nRet = MV_OK;

	// Camera 정보
	// m_nCameraNum	= GetPrivateProfileInt(_T("Settings"), _T("Camera Number"), 4, strInitFilePath);
	TCHAR szCamType[100] = {0, }, szDefaultGain[5] = {0, };
	GetPrivateProfileString(_T("Settings"), _T("Camera Type"), _T("Coaxial"), szCamType, sizeof(szCamType), strInitFilePath);
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("0.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));

	CString strCamType;
	strCamType.Format(_T("%s"), szCamType);

	// ini 이름 가져와서 카메라 UserName이랑 매칭되는 카메라 찾아서 연결
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
	nRet = CMvCamera::EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);

	for (unsigned int i = 0; i < m_stDevList.nDeviceNum; i++)
	{
		MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
		CString strUserDefinedName(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
		if (!strCamType.CompareNoCase(strUserDefinedName))	// 같으면
		{
			nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[i]);

			if (MV_OK == nRet)
				m_bOpenDevice = TRUE;
		}
	}

	// SV 카메라에만 필요한 동작, PacketSize를 Max로 잡는 코드랑 이니셜 코드들
	unsigned int nPacketSize = 0;
	m_pcMyCamera->GetOptimalPacketSize(&nPacketSize);
	nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);

	CString strLog;
	strLog.Format(_T("Init Cam - Packet Size = %d"), nPacketSize);
	theApp.m_fnWriteTactLog(strLog);
	
	// 이미지 콜백함수 등록
	nRet = m_pcMyCamera->RegisterImageCallBack(ImageCallBack, this);
	//nRet = m_pcMyCamera->RegisterEventCallBack("ExposureEnd", EventCallBack, NULL);

	// 해상도 가져오는 
	MVCC_INTVALUE_EX stParam;
	memset(&stParam, 0, sizeof(MVCC_INTVALUE_EX));
	m_pcMyCamera->GetIntValue("Width", &stParam);
	m_nImageWidth = (int)stParam.nCurValue;
	m_pcMyCamera->GetIntValue("Height", &stParam);
	m_nImageHeight = (int)stParam.nCurValue;
	m_nBandWidth = 1;
	//m_nBandWidth = 3;
	m_nBitRate = 8;

	// 프레임 속도 10
	float Framerate = 10;
	m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", Framerate);
	// 트리거모드로 설정 (소프트웨어 트리거써서 그랩할거기때문)
	m_pcMyCamera->SetEnumValue("TriggerMode", MV_TRIGGER_MODE_ON);
	m_pcMyCamera->SetEnumValue("TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);

	// ExposureAuto off
	nRet = m_pcMyCamera->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
	m_pcMyCamera->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);

	// GainAuto off
	m_pcMyCamera->SetEnumValue("GainAuto", MV_GAIN_MODE_OFF);

	// camGrabStart
	m_pcMyCamera->StartGrabbing();

	if (nRet == MV_OK)	bRet = TRUE;
	else				bRet = FALSE;

	return bRet;
}


int CHikCamera::CloseCamera()
{
	// Stop grab image
	m_pcMyCamera->StopGrabbing();

	// Unregister image callback
	m_pcMyCamera->RegisterImageCallBack(NULL, NULL);

	// 채널 닫기
	// - - - RGB 추가되면 필요

	// Close device & Destroy handle
	if (m_pcMyCamera)
	{
		m_pcMyCamera->Close();
		delete m_pcMyCamera;
		m_pcMyCamera = NULL;
	}

	m_bOpenDevice = FALSE;

	if (m_pGrabBuf)
	{
		free(m_pGrabBuf);
		m_pGrabBuf = NULL;
	}
	m_nGrabBufSize = 0;

	if (m_pSaveImageBuf)
	{
		free(m_pSaveImageBuf);
		m_pSaveImageBuf = NULL;
	}
	m_nSaveImageBufSize = 0;

	return 1;
}

BOOL CHikCamera::SetExposureTime(double dExpOnTime)
{
	if (FALSE == m_bOpenDevice)
		return STATUS_ERROR;

	if (dExpOnTime < 0)
		dExpOnTime = 0;

	return m_pcMyCamera->SetFloatValue("ExposureTime", (float)dExpOnTime * 1000) == MV_OK ? TRUE : FALSE;
}
// Stitch	180809 YSS
BOOL CHikCamera::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;

	return bRet;
}

double CHikCamera::GetExposureTime()
{
	float ExposeTime;

	MVCC_FLOATVALUE stFloatValue = { 0 };
	m_pcMyCamera->GetFloatValue("ExposureTime", &stFloatValue);
	ExposeTime = stFloatValue.fCurValue;

	return ExposeTime/1000;
}


int CHikCamera::GetTriggerMode()
{
	return 0;
}

BOOL CHikCamera::SetTriggerMode(int nMode)
{

	return 0;
}

double CHikCamera::GetAnalogGain()
{
	MVCC_FLOATVALUE stFloatValue = { 0 };
	m_pcMyCamera->GetFloatValue("Gain", &stFloatValue);
	return (double)stFloatValue.fCurValue;
}

BOOL CHikCamera::SetAnalogGain(double dGainValue)
{
	BOOL bRet = TRUE;

	// 기본 Gain값 더해서 적용 - 설비별 Camera별 편차 보정 - 광학팀 요청 기능
	int nRet = m_pcMyCamera->SetFloatValue("Gain", (float)(dGainValue + GetDefaultGainOffset()));

	return bRet;
}

void CHikCamera::DisplayLive(HWND hDispWnd)
{

}
void CHikCamera::WaitGrabEnd()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd Start"));
	WaitForSingleObject(g_hGrabEnd, 5000);	
	ResetEvent(g_hGrabEnd);
	m_GrabTime.End();	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd End"));
}

void CHikCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{	
	int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	unsigned char* rgbImage = (unsigned char *)GlobalAlloc(GMEM_FIXED, nBufferSize);

	memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), rgbImage, nBufferSize);
	GlobalFree(rgbImage);
}

void CHikCamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	char filename[200] = {0,};
	int nRet = MV_OK;
	MV_SAVE_IMG_TO_FILE_PARAM stSaveFileParam;
	memset(&stSaveFileParam, 0, sizeof(MV_SAVE_IMG_TO_FILE_PARAM));

	if (nRet == MV_OK)
	{
		m_nSaveImageType = MV_Image_Bmp;

		stSaveFileParam.enImageType = m_nSaveImageType;				// Image format to save
		stSaveFileParam.enPixelType = m_stImageInfo.enPixelType;    // Camera pixel type
		stSaveFileParam.nWidth = m_stImageInfo.nWidth;				// Width
		stSaveFileParam.nHeight = m_stImageInfo.nHeight;			// Height
		stSaveFileParam.nDataLen = m_stImageInfo.nFrameLen;
		stSaveFileParam.pData = m_pSaveImageBuf;
		stSaveFileParam.iMethodValue = 0;
	}
	int nLen = WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, NULL, 0, NULL, NULL);       
	WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, filename, nLen, NULL, NULL);

	CString strFilePath;
	strFilePath.Format(_T("%s"),strSavePath);
	SHCreateDirectoryEx(NULL, strFilePath.Left(strFilePath.GetLength() - (strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')))), NULL);

	char* pfilename = CSTR2PCH(strFilePath);
	memcpy(stSaveFileParam.pImagePath, pfilename, strlen(pfilename));

	nRet = m_pcMyCamera->SaveImageToFile(&stSaveFileParam);
}

// 16.07.04 Live 화면 추가
BOOL CHikCamera::StartLiveGrab()
{
	BOOL bRet = FALSE;

	m_bLiveMode = TRUE;
	bRet = m_pcMyCamera->CommandExecute("TriggerSoftware") == MV_OK ? TRUE : FALSE;

	if (!bRet)
	{
		theApp.m_fnWriteTactLog(_T("Start LiveGrab fail !!"));
	}
	theApp.m_fnWriteTactLog(_T("Start LiveGrab"));

	return bRet;
}

BOOL CHikCamera::StopLiveGrab()
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


int CHikCamera::GetLivePixelInfo(CPoint ptBuf)
{
	//int nVal;

	return 0;
}

void CHikCamera::SaveGrabImage(CString strFilePath)
{
	MV_FRAME_OUT stImageInfo = { 0 };

	char* filename = CSTR2PCH(strFilePath);

	MV_SAVE_IMG_TO_FILE_PARAM stSaveFileParam;
	memset(&stSaveFileParam, 0, sizeof(MV_SAVE_IMG_TO_FILE_PARAM));

	m_nSaveImageType = MV_Image_Bmp;

	stSaveFileParam.enImageType		= m_nSaveImageType;				// Image format to save
	stSaveFileParam.enPixelType		= m_stImageInfo.enPixelType;    // Camera pixel type
	stSaveFileParam.nWidth			= m_stImageInfo.nWidth;			// Width
	stSaveFileParam.nHeight			= m_stImageInfo.nHeight;		// Height
	stSaveFileParam.nDataLen		= m_stImageInfo.nFrameLen;
	stSaveFileParam.pData			= m_pSaveImageBuf;
	stSaveFileParam.iMethodValue	= 0;

	memcpy(stSaveFileParam.pImagePath, filename, strlen(filename));

	int nRet = m_pcMyCamera->SaveImageToFile(&stSaveFileParam);

	SAFE_DELETE_ARR(filename);	
}

// 2017.05.11 add by ikm GUI -> Image SAVE 용 - S
void CHikCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	char* filename = CSTR2PCH(strFilePath);

	if(bIsLiveImage)
	{		
		MV_SAVE_IMG_TO_FILE_PARAM stSaveFileParam;
		memset(&stSaveFileParam, 0, sizeof(MV_SAVE_IMG_TO_FILE_PARAM));

		m_nSaveImageType = MV_Image_Bmp;

		stSaveFileParam.enImageType		= m_nSaveImageType;				// Image format to save
		stSaveFileParam.enPixelType		= m_stImageInfo.enPixelType;    // Camera pixel type
		stSaveFileParam.nWidth			= m_stImageInfo.nWidth;			// Width
		stSaveFileParam.nHeight			= m_stImageInfo.nHeight;		// Height
		stSaveFileParam.nDataLen		= m_stImageInfo.nFrameLen;
		stSaveFileParam.pData			= m_pSaveImageBuf;
		stSaveFileParam.iMethodValue	= 0;

		memcpy(stSaveFileParam.pImagePath, filename, strlen(filename));

		int nRet = m_pcMyCamera->SaveImageToFile(&stSaveFileParam);
	}

	else
	{			
		cv::imwrite(filename, m_srcImage[nImgCnt](rect));
	}
	SAFE_DELETE_ARR(filename);
}
// 2017.05.11 add by ikm GUI -> Image SAVE 용 - E


void CHikCamera::GetGrabBuffer()
{
	CameraExpose();

	WaitGrabEnd();
}

BOOL CHikCamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
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
void CHikCamera::GetGrabImage(byte* byteImgArr)
{

	//int nSizeX = Image_getSizeX(m_Image);
	//int nSizeY = Image_getSizeY(m_Image);
	//int nImageID = Image_getImageID(m_Image);


	//SVGigE_RETURN result = Image_getImageRGB(m_Image, byteImgArr, nSizeX * nSizeY * 3, BAYER_METHOD_HQLINEAR);
}

// Crop Image
void CHikCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	IplImage *reSizeImage = NULL;
	IplImage *cropImage = NULL;

	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	if(bIsLiveImage)
	{
		//int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		//byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
		//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
		//memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, nBufferSize);
		//memcpy(m_pSaveImageBuf, stImageInfo.pBufAddr, m_nSaveImageBufSize);

		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
		memcpy(Image, m_pSaveImageBuf, nBufferSize);
		Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC1, Image);
		//Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, m_pSaveImageBuf);
		cv::resize( matLive(rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
		//GlobalFree(Image);
	}	
	else
	{		
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	}
		
	if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

 	memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
}

double CHikCamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
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
void CHikCamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
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
		//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
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
		//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
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

void CHikCamera::DisplayGrabImage(HWND hDispWnd)
{
	//Image_handle Image;
	//Image = GetGrabBuffer();
	GetGrabBuffer();
}

void CHikCamera::TestFunc()
{

}

// 2017.01.09 add by ikm GUI 이미지 로드 시 MIL 버퍼 저장 (color, gray image 구분) - S  
void CHikCamera::m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);

	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void CHikCamera::CalcWhiteBalance(float* WhiteBalance)
{
	/*
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
	*/
}

void CHikCamera::SetWhiteBalance(float* WhiteBalance)
{	
	//Camera_setWhiteBalance(m_InspCamera,WhiteBalance[0], WhiteBalance[1], WhiteBalance[2]);
}

void CHikCamera::GetWhiteBalance(float* WhiteBalance)
{
	//Camera_getWhiteBalance(m_InspCamera,&WhiteBalance[0],&WhiteBalance[1],&WhiteBalance[2]);
}

UINT CHikCamera::retryConnect()
{
	BOOL bRet = FALSE;

	CString strLog;
	//Internal Server 시작		
	
	CloseCamera();
	Sleep(3000);
	InitializeCamera(theApp.GetInitFilePath());

	//strLog.Format(_T("Camrea reconnect, %d"),SVGigE_SIGNAL_CAMERA_CONNECTION_LOST);
	theApp.m_pLogWriter->m_fnWriteLog(strLog);	
	printf("ThreadConnectState End \n");

	return 0;
}