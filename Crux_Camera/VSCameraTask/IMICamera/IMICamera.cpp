// Digitizer.cpp: implementation of the CIMICamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "IMICamera.h"
#include "VSCameraTask.h"


using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE g_hGrabEnd;		// Exposure End ����

CIMICamera::CIMICamera()
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
	// m_pImage = NULL;

	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX ; i++)
		m_srcImage[i] = NULL;

	ntcInit();
}

CIMICamera::~CIMICamera()
{
	if (m_pCameraHandle)
	{
		ntcClose(m_pCameraHandle);
		m_pCameraHandle = NULL;
	}

	ntcUninit();
}


void CIMICamera::CloseApp()
{   
	
}

////////////////////////////////////////////////////////////
// 19.04.15 - OneShot �Լ� �Ⱦ��� RunSWTrigger ����Ͽ� �׷�
//			  (����ð� �ȸԴ� ��������)
void CIMICamera::CameraExpose()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure Start"));
	ResetEvent(g_hGrabEnd);

	//if (ntcOneShot(m_pCameraHandle) != NEPTUNE_ERR_Success)
	if (ntcRunSWTrigger(m_pCameraHandle) != NEPTUNE_ERR_Success)
	{
		theApp.m_pLogWriter->m_fnWriteLog(_T("OneShot Failed"));
		SetEvent(g_hGrabEnd);
	}
	WaitForSingleObject(g_hGrabEnd, INFINITE);
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure End"));
}


///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.


BOOL CIMICamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;

	// Camera ����
	m_nCameraNum	= GetPrivateProfileInt(_T("Settings"), _T("Camera Number"), 4, strInitFilePath);
	TCHAR szCamType[100] = {0, }, szDefaultGain[5] = {0, };
	GetPrivateProfileString(_T("Settings"), _T("Camera Type"), _T("Coaxial"), szCamType, sizeof(szCamType), strInitFilePath);
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("0.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));

	CString strCamType, strTemp;
	strCamType.Format(_T("%s"), szCamType);

	NEPTUNE_TRIGGER  Trigger; 

	_uint32_t uiCount = 0;
	if (ntcGetCameraCount(&uiCount) == NEPTUNE_ERR_Success)
	{
		if (uiCount > 0)
		{
			PNEPTUNE_CAM_INFO pCameraInfo = new NEPTUNE_CAM_INFO[uiCount];
			if (pCameraInfo)
			{
				ZeroMemory(pCameraInfo, sizeof(NEPTUNE_CAM_INFO) * uiCount);
				ENeptuneError emErr = ntcGetCameraInfo(pCameraInfo, uiCount);
				if (emErr == NEPTUNE_ERR_Success)
				{
					for (_uint32_t i = 0; i < uiCount; i++)
					{
						PNEPTUNE_CAM_INFO pSetData = new NEPTUNE_CAM_INFO;
						if (pSetData)
						{
							memcpy(pSetData, &pCameraInfo[i], sizeof(NEPTUNE_CAM_INFO));
							strTemp = pSetData->strUserID;
							if (!strCamType.CompareNoCase(strTemp))
							{
								ENeptuneError emErr = ntcOpen(pSetData->strCamID, &m_pCameraHandle);
								if (emErr == NEPTUNE_ERR_Success)
								{
									// 19.04.15 - IMI ī�޶�� CONTINUOUS ��常 ��� (GRAB �̵�, LIVE ��)
									//emErr = ntcSetAcquisitionMode(m_pCameraHandle, NEPTUNE_ACQ_SINGLEFRAME);
									emErr = ntcSetAcquisitionMode(m_pCameraHandle, NEPTUNE_ACQ_CONTINUOUS);
									emErr = ntcSetAcquisition(m_pCameraHandle, NEPTUNE_BOOL_TRUE);

									Trigger.Source = NEPTUNE_TRIGGER_SOURCE_SW;
									Trigger.Mode = NEPTUNE_TRIGGER_MODE_15;
									Trigger.Polarity = NEPTUNE_POLARITY_RISINGEDGE;
									Trigger.OnOff = NEPTUNE_BOOL_TRUE;
									Trigger.nParam = 1;
									emErr = ntcSetTrigger(m_pCameraHandle, Trigger);

									if (emErr == NEPTUNE_ERR_Success)
									{
										SetImageCallBackState(0);
										ntcSetUnplugCallback(m_pCameraHandle, DeviceUnplugCallback, this);
										ntcSetFrameCallback(m_pCameraHandle, FrameCallback, this);
										bRet = TRUE;
									}
									else
									{
										theApp.m_pLogWriter->m_fnWriteLog(_T("SetAcquisition Failed"));
									}
								}
								else
								{
									theApp.m_pLogWriter->m_fnWriteLog(_T("Open Camera Failed"));
								}

								NEPTUNE_IMAGE_SIZE stImageSize;
								emErr = ntcGetImageSize(m_pCameraHandle, &stImageSize);
								if (emErr == NEPTUNE_ERR_Success)
								{
									m_nImageWidth = stImageSize.nSizeX;
									m_nImageHeight = stImageSize.nSizeY;
									m_nBandWidth = 3;
									m_nBitRate = 8;
								}
								else
								{
									theApp.m_pLogWriter->m_fnWriteLog(_T("GetImageSize Failed"));
								}

								break;
							}
						}
					}
				}
				else
				{
					theApp.m_pLogWriter->m_fnWriteLog(_T("Get Camera Device Failed"));
				}

				delete[] pCameraInfo;
				pCameraInfo = NULL;
			}
		}
	}

	return bRet;
}


// �ر򸱼� �־ �����
// ���÷��� �ɶ� ȣ��Ǵ°� �ƴϰ�,
// ���÷��� �Ǿ��ٰ� �ٽ� ����ɶ� ȣ���
void CALLBACK CIMICamera::DeviceUnplugCallback(void* pContext)
{
	CIMICamera* pCamera = (CIMICamera*)pContext;
	if (pCamera)
	{
		pCamera->InitializeCamera(theApp.GetInitFilePath());
		theApp.m_pLogWriter->m_fnWriteLog(_T("Selected camera was unplugged"));
	}
}


void CALLBACK CIMICamera::FrameCallback(PNEPTUNE_IMAGE pImage, void* pContext)
{
	CIMICamera* pCamera = (CIMICamera*)pContext;
	if (pCamera)
	{
		SetEvent(g_hGrabEnd);
	}
}


int CIMICamera::CloseCamera()
{
	if (m_pCameraHandle)
	{
		ntcClose(m_pCameraHandle);
		m_pCameraHandle = NULL;
	}

	return 1;
}

BOOL CIMICamera::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = FALSE;

	if (m_pCameraHandle)
	{
		_uint32_t microSecond = (_uint32_t)(dExpOnTime * 1000);
		if (ntcSetExposureTime(m_pCameraHandle, microSecond) == NEPTUNE_ERR_Success)
			bRet = TRUE;
	}

	return bRet;
}

// Stitch	180809 YSS
BOOL CIMICamera::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;
	return bRet;
}

double CIMICamera::GetExposureTime()
{
	_uint32_t curMicroSecond;
	_uint32_t minMicroSecond;
	_uint32_t maxMicroSecond;

	if (m_pCameraHandle)
	{
		if (ntcGetExposureTime(m_pCameraHandle, &curMicroSecond, &minMicroSecond, &maxMicroSecond) != NEPTUNE_ERR_Success)
			theApp.m_pLogWriter->m_fnWriteLog(_T("GetExposureTime Failed"));
	}

	return curMicroSecond / 1000.0;
}


int CIMICamera::GetTriggerMode()
{
	return 0;
}

BOOL CIMICamera::SetTriggerMode(int nMode)
{
	return 0;
}

double CIMICamera::GetAnalogGain()
{
	NEPTUNE_FEATURE stFeatureInfo;

	if (m_pCameraHandle)
	{
		if (ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GAIN, &stFeatureInfo) != NEPTUNE_ERR_Success)
			theApp.m_pLogWriter->m_fnWriteLog(_T("GetAnalogGain Failed"));
	}

	return (double) stFeatureInfo.Value;
}

BOOL CIMICamera::SetAnalogGain(double dGainValue)
{
	BOOL bRet = FALSE;

	NEPTUNE_FEATURE stFeatureInfo;

	if (m_pCameraHandle)
	{
		if (ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GAIN, &stFeatureInfo) != NEPTUNE_ERR_Success)
			theApp.m_pLogWriter->m_fnWriteLog(_T("GetAnalogGain Failed"));

		stFeatureInfo.Value = (_int32_t) (dGainValue * GetDefaultGainOffset());

		if (ntcSetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GAIN, stFeatureInfo) == NEPTUNE_ERR_Success)
			bRet = TRUE;
	}

	return bRet;
}

void CIMICamera::DisplayLive(HWND hDispWnd)
{

}

void CIMICamera::WaitGrabEnd()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd Start"));
	WaitForSingleObject(g_hGrabEnd, 5000);
	ResetEvent(g_hGrabEnd);
	m_GrabTime.End();	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd End"));
}

void CIMICamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{	
	_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
	// ntcRunSWTrigger(m_pCameraHandle);
	ENeptuneError emErr = ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);
	if (emErr == NEPTUNE_ERR_Success)
	{
		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), pRGBBuffer, uiBufSize);
	}
	else
	{
		theApp.m_pLogWriter->m_fnWriteLog(_T("GetRGBData Failed"));
	}
	GlobalFree(pRGBBuffer);
}

void CIMICamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	// ���� ����
	CString strFilePath;
	strFilePath.Format(_T("%s"), strSavePath);
	SHCreateDirectoryEx(NULL, strFilePath.Left(strFilePath.GetLength() - (strFilePath.GetLength() - strFilePath.ReverseFind(_T('\\')))), NULL);

	// ����
	char* filename = CSTR2PCH(strSavePath);

	_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
	//ntcRunSWTrigger(m_pCameraHandle);
	ENeptuneError emErr = ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);

	Mat _mat = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
	cv::imwrite(filename, _mat);
}

// 16.07.04 Live ȭ�� �߰�
BOOL CIMICamera::StartLiveGrab()
{
	NEPTUNE_TRIGGER  Trigger;
	Trigger.Source = NEPTUNE_TRIGGER_SOURCE_SW;
	Trigger.Mode = NEPTUNE_TRIGGER_MODE_15;
	Trigger.Polarity = NEPTUNE_POLARITY_RISINGEDGE;
	Trigger.OnOff = NEPTUNE_BOOL_FALSE;
	Trigger.nParam = 1;
	if (ntcSetTrigger(m_pCameraHandle, Trigger) != NEPTUNE_ERR_Success)
	{
		theApp.m_pLogWriter->m_fnWriteLog(_T("StartLiveGrab Err : SetAcquisition Failed"));
		return FALSE;
	}

	m_bLiveMode = TRUE;
	theApp.m_fnWriteTactLog(_T("Start LiveGrab"));

	return TRUE;
}

BOOL CIMICamera::StopLiveGrab()
{
	NEPTUNE_TRIGGER  Trigger;
	Trigger.Source = NEPTUNE_TRIGGER_SOURCE_SW;
	Trigger.Mode = NEPTUNE_TRIGGER_MODE_15;
	Trigger.Polarity = NEPTUNE_POLARITY_RISINGEDGE;
	Trigger.OnOff = NEPTUNE_BOOL_TRUE;
	Trigger.nParam = 1;
	if (ntcSetTrigger(m_pCameraHandle, Trigger) != NEPTUNE_ERR_Success)
	{
		theApp.m_pLogWriter->m_fnWriteLog(_T("StopLiveGrab Err : SetAcquisition Failed"));
		return FALSE;
	}

	m_bLiveMode = FALSE;

	WaitForSingleObject(g_hGrabEnd, 1000);
	theApp.m_fnWriteTactLog(_T("WaitForSingleObject grab end"));
	ResetEvent(g_hGrabEnd);
	theApp.m_fnWriteTactLog(_T("StopLiveGrab"));

	return TRUE;
}

int CIMICamera::GetLivePixelInfo(CPoint ptBuf)
{
	//int nVal;

	return 0;
}

// 2019.05.31 yskim - ���� �׽�Ʈ �Ϸ� (Cam Task���� Image save ��ư ���)
void CIMICamera::SaveGrabImage(CString strFilePath)
{
	char* filename = CSTR2PCH(strFilePath);
	CameraExpose();
	uint uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	Mat mat = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3);
	if (ntcGetRGBData(m_pCameraHandle, mat.data, uiBufSize) != NEPTUNE_ERR_Success)
		return;
	cv::imwrite(filename, mat);
}

// 2017.05.11 add by ikm GUI -> Image SAVE �� - S
void CIMICamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	ResetEvent(g_hGrabEnd);

	char* filename = CSTR2PCH(strFilePath);

	if(bIsLiveImage)
	{
		_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
		WaitGrabEnd();

		if (ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize) != NEPTUNE_ERR_Success)
			return;

		matResize = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer)(rect);
		cv::imwrite(filename, matResize);
		GlobalFree(pRGBBuffer);
	}
	else
	{			
		cv::imwrite(filename, m_srcImage[nImgCnt](rect));
	}
}
// 2017.05.11 add by ikm GUI -> Image SAVE �� - E

BYTE* CIMICamera::GetGrabBuffer()
{
	CameraExpose();
	//WaitGrabEnd();
	// return m_pImage;

	_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
	//ntcRunSWTrigger(m_pCameraHandle);
	ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);
	return pRGBBuffer;
}

BOOL CIMICamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
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

// ���� �̹���
void CIMICamera::GetGrabImage(byte* byteImgArr)
{
	_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
	//ntcRunSWTrigger(m_pCameraHandle);
	ENeptuneError emErr = ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);
	if (emErr == NEPTUNE_ERR_Success)
	{
		memcpy(byteImgArr, pRGBBuffer, uiBufSize);
	}
	else
	{
		theApp.m_pLogWriter->m_fnWriteLog(_T("GetRGBData Failed"));
	}
	GlobalFree(pRGBBuffer);
}

// Crop Image
void CIMICamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	ResetEvent(g_hGrabEnd);
	if(bIsLiveImage)
	{
		_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
		WaitGrabEnd();

		if (ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize) != NEPTUNE_ERR_Success)
			return;

		Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
		cv::resize(matLive(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);

		GlobalFree(pRGBBuffer);
	}	
	else
	{		
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	}
		
	if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

	memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
}

double CIMICamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
{
	if( matBuf.empty() )	return 0;

	cv::Rect rectROI( rect.left, rect.top, rect.Width(), rect.Height() );

	// ���� �ڸ���
	cv::Mat matTempBuf = matBuf( rectROI );

	// ��� ���ϱ�
	cv::Scalar m;
	m = cv::mean( matTempBuf );

	// �޸� �Ҵ� ����
	matTempBuf.release();

	return (double)m[0];
}

// �Է°� : ���ϴ� ��� ��� & ��⸦ ���߰��� �ϴ� ����
// ��°� : ����ð� & ����
void CIMICamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
{	
	// ���� �ð� �ʱⰪ
	int nExp = 5;
	int nExpStep = 200;
	const int nMaxExp = 601;

	// ī�޶� ���� �ʱⰪ
	int nGain = 30;
	int nGainStep = 30;
	const int nMaxGain = 300;	

	SetExposureTime(nMaxExp);

	// Gain ���� Loop
	while (nGain <= nMaxGain)
	{
		SetAnalogGain(nGain);

		// Grab OpenCV �� ��� ���̺� ����
		_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
		ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);
		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
		GlobalFree(pRGBBuffer);
		//cv::Mat matBuf = m_liveImage;// = GetMilGrabBuffer();

		// ��� GV�� ���
		double dMean = GetMeanValue(matBuf, rect);

		// ���� ���GV�� ���ϴ� ��� GV���� ���� ���, Gain �÷��� �ٽ� Loop ( Exp�� �ִ� ���� )
		// GV�� �� �´� ��쵵 ������ ���� ( 2GV ���� �� ���� �� )
		if (nWantMean + 2 > dMean)
			nGain += nGainStep;
		// ���� �ϴ� ��� Loop ������
		else
			break;
	}

	// �ִ�ġ���� ���ΰ��� ���� ���, �����ϴ� ���� ���� ( Exp�� �ִ� ���� )
	if (nGain > nMaxGain)	return;

	SetAnalogGain(nGain);

	// Exp ���� Loop ( 0���� ū ��츸 )
	while (nExpStep > 0)
	{
		SetExposureTime(nExp + nExpStep);

		// Grab OpenCV �� ��� ���̺� ����
		_uint32_t uiBufSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* pRGBBuffer = (byte *)GlobalAlloc(GMEM_FIXED, uiBufSize);
		ntcGetRGBData(m_pCameraHandle, pRGBBuffer, uiBufSize);
		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
		GlobalFree(pRGBBuffer);

		// ��� GV�� ���
		double dMean = GetMeanValue(matBuf, rect);

		// ���ϴ� ��� ��Ⱚ�� ���� ��� ��Ⱚ ��
		if (nWantMean > dMean)
			nExp += nExpStep;
		// Step ������ �ؼ� �ٽ� �õ�
		else 
			nExpStep /= 2;
	}

	// ���
	nOutExp = nExp;
	nOutGain = nGain;
}

void CIMICamera::DisplayGrabImage(HWND hDispWnd)
{
	GetGrabBuffer();
}

void CIMICamera::TestFunc()
{

}

// 2017.01.09 add by ikm GUI �̹��� �ε� �� MIL ���� ���� (color, gray image ����) - S  
void CIMICamera::m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);

	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void CIMICamera::CalcWhiteBalance(float* WhiteBalance)
{

	//Camera_setAcquisitionMode(m_InspCamera, ACQUISITION_MODE_INT_TRIGGER);
	//Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_START);
	//Camera_softwareTrigger(m_InspCamera);
	////ResetEvent(g_hExpEnd);
	//WaitForSingleObject(g_hGrabEnd,INFINITE);
	//Camera_setAcquisitionControl(m_InspCamera, ACQUISITION_CONTROL_STOP);
	//int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
	//byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
	//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 

	//float R;
	//float G;
	//float B;

	//if( SVGigE_SUCCESS == SVGigE_estimateWhiteBalance(Image, nBufferSize, &R, &G, &B) )
	//{
	//	WhiteBalance[0] = R;
	//	WhiteBalance[1] = G;
	//	WhiteBalance[2] = B;
	//}
	
	//GlobalFree(Image);
}

void CIMICamera::SetWhiteBalance(float* WhiteBalance)
{	
	NEPTUNE_FEATURE fGainRed, fGainGreen, fGainBlue;

	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_RGAIN, &fGainRed);
	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GGAIN, &fGainGreen);
	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_BGAIN, &fGainBlue);

	fGainRed.Value = (int)WhiteBalance[0];
	fGainGreen.Value = (int)WhiteBalance[1];
	fGainBlue.Value = (int) WhiteBalance[2];

	ntcSetFeature(m_pCameraHandle, NEPTUNE_FEATURE_RGAIN, fGainRed);
	ntcSetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GGAIN, fGainGreen);
	ntcSetFeature(m_pCameraHandle, NEPTUNE_FEATURE_BGAIN, fGainBlue);
}

void CIMICamera::GetWhiteBalance(float* WhiteBalance)
{
	NEPTUNE_FEATURE fGainRed, fGainGreen, fGainBlue;

	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_RGAIN, &fGainRed);
	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_GGAIN, &fGainGreen);
	ntcGetFeature(m_pCameraHandle, NEPTUNE_FEATURE_BGAIN, &fGainBlue);

	WhiteBalance[0] = (float)fGainRed.Value;
	WhiteBalance[1] = (float)fGainGreen.Value;
	WhiteBalance[2] = (float)fGainBlue.Value;
}

UINT CIMICamera::retryConnect()
{
	BOOL bRet = FALSE;

	CString strLog;
	//Internal Server ����		
	
	CloseCamera();
	Sleep(3000);
	InitializeCamera(theApp.GetInitFilePath());

	strLog.Format(_T("Camrea reconnect"));
	theApp.m_pLogWriter->m_fnWriteLog(strLog);	
	printf("ThreadConnectState End \n");

	return 0;
}