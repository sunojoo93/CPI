// Digitizer.cpp: implementation of the CSimulationCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "SimulationCamera.h"
#include "VSCameraTask.h"

using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimulationCamera::CSimulationCamera()
{

	GetPrivateProfileString(_T("SimulationInfo"), _T("AutoRunImageLoadPath") ,_T("0"), m_strSimulationImagePath, sizeof(m_strSimulationImagePath), INIT_FILE_PATH);
	GetPrivateProfileString(_T("SimulationInfo"), _T("LiveImageLoadFile") ,_T("0"), m_strSimulationLiveImagePath, sizeof(m_strSimulationLiveImagePath), INIT_FILE_PATH);

	m_nSimulationGrabTime=GetPrivateProfileInt(_T("Settings"), _T("GrabWaitTimeForSimulation"), 300, INIT_FILE_PATH);

	TCHAR strFilePath[200] = {0,};
	char strTempPath[200] = {0,};
	_stprintf(strFilePath,_T("%s"),m_strSimulationLiveImagePath);

	for(int i = 0 ; i < NUM_LOAD_IMAGE_MAX ; i++)
		m_srcImage[i] = NULL;
	WideCharToMultiByte(CP_ACP, 0, m_strSimulationLiveImagePath, 200, strTempPath, 200, NULL, NULL);

	m_liveImage = cv::imread(strTempPath, IMREAD_UNCHANGED);  
}

CSimulationCamera::~CSimulationCamera()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.

BOOL CSimulationCamera::InitializeCamera(CString strInitFilePath)
{
 	BOOL bRet = FALSE;
							// Digitizer ����
	TCHAR szSimulationFlag[6] = {0,};        //2017.01.02 sgkim H-Project
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("FALSE")) )
		PostQuitMessage(0);

	m_lDigSizeX		= GetPrivateProfileInt(_T("Settings"), _T("Image Width"), 4, strInitFilePath);
	m_lDigSizeY		= GetPrivateProfileInt(_T("Settings"), _T("Image Height"), 4, strInitFilePath);
	m_lDigBit		= GetPrivateProfileInt(_T("Settings"), _T("Image Bitrate"), 4, strInitFilePath);
	m_lDigBand		= GetPrivateProfileInt(_T("Settings"), _T("Image Bandwidth"), 4, strInitFilePath);	

	return bRet;
}


// 
// int CSimulationCamera::SetSharedMemForSimulation(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName)	
// {
// 	int nRet = FALSE;
// 	TCHAR strFilePath[200] = {0,};
// 	char strTempPath[200] = {0,};
// 	_stprintf(strFilePath,_T("%s\%s\\%d_%s.bmp"),m_strSimulationImagePath,strPanelID,nGrabNum,strGrabStepName);
// 
// 	WideCharToMultiByte(CP_ACP, 0, strFilePath, 200, strTempPath, 200, NULL, NULL);
// 
// 	if( fopen(strTempPath,"r") )
// 	{
// 		IplImage *cvImage = cvCreateImage( cvSize(theApp.m_pCamera->GetImageWidth(), theApp.m_pCamera->GetImageHeight()), 8, 1);
// 		cvImage = cvLoadImage(strTempPath, CV_LOAD_IMAGE_UNCHANGED);
// 		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), cvImage->imageData, cvImage->imageSize);
// 
// 		cvReleaseImage(&cvImage);
// 	}
// 	else
// 	{
// 		CString strMessage;
// 		strMessage.Format(_T("�̹��� ���� - %s"), strFilePath);
// 		AfxMessageBox(strMessage);
// 		return APP_NG;
// 
// 	}
// 
// 	return APP_OK;
// 	
// }

// 16.07.04 Live ȭ�� �߰�
BOOL CSimulationCamera::StartLiveGrab()
{
	m_bIsLive = TRUE;	

	TCHAR strFilePath[200] = {0,};
	char strTempPath[200] = {0,};
	_stprintf(strFilePath,_T("%s"),m_strSimulationLiveImagePath);
 	WideCharToMultiByte(CP_ACP, 0, m_strSimulationLiveImagePath, 200, strTempPath, 200, NULL, NULL);

	m_liveImage = cv::imread(strTempPath, IMREAD_UNCHANGED);  
	
	return TRUE;
}

BOOL CSimulationCamera::StopLiveGrab()
{
	m_bIsLive = FALSE;

	return TRUE;
}

BOOL CSimulationCamera::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
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


void CSimulationCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	Mat matResize;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(10);
	if(bIsLiveImage)
	{		
		cv::resize( m_liveImage(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY ), 0, 0, CV_INTER_AREA);
	}		
	else
	{		
		cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	}		
	
	if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		


 	memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());	
}

double CSimulationCamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
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
void CSimulationCamera::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
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
		cv::Mat matBuf = m_liveImage;// = GetMilGrabBuffer();

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
		cv::Mat matBuf = m_liveImage;//

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

void CSimulationCamera::WaitGrabEnd()
{
	Sleep(m_nSimulationGrabTime);
}
void CSimulationCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{
	int nRet = FALSE;
	TCHAR strFilePath[200] = {0,};
	char strTempPath[200] = {0,};
	_stprintf(strFilePath,_T("%s\\%s\\%02d_%s_CAM%02d.bmp"),m_strSimulationImagePath, strPanelID, nGrabNum, strGrabStepName,theApp.GetTaskNum()-1 );
	//strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), m_strSimulationImagePath, strPanelID, nGrabCnt, theApp.m_Config.GetCurStepName(nGrabCnt), 0);

	WideCharToMultiByte(CP_ACP, 0, strFilePath, 200, strTempPath, 200, NULL, NULL);

	if( fopen(strTempPath,"r") )
	{
		//cv::Mat cvImage = cvCreateImage( cvSize(theApp.m_pCamera->GetImageWidth(), theApp.m_pCamera->GetImageHeight()), 8, 1);
		cv::Mat cvImage = cv::imread(strTempPath, IMREAD_UNCHANGED);   
		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), cvImage.data, cvImage.total() * cvImage.elemSize());
		//memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), cvImage->imageData, cvImage->imageSize);

		//cvReleaseImage(&cvImage);
	}
	else
	{
		CString strMessage;
		strMessage.Format(_T("�̹��� ���� - %s"), strFilePath);
		AfxMessageBox(strMessage);

	}

}
void CSimulationCamera::SaveFileCurBuffer(TCHAR* strSavePath){}

void CSimulationCamera::m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, int &nChcnt, int &nWidth, int &nHeight, double dAngle)
{
 	char strTempPath[1000] = {0,};
 	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);	
	
	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void    CSimulationCamera::SaveGrabImage(CString strFilePath)
{

}

// 2017.05.11 add by ikm GUI -> Image SAVE �� - S
void CSimulationCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	Mat matSaveimage;
	Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	Sleep(30);
	char* filename = CSTR2PCH(strFilePath);
	matSaveimage = m_liveImage;
	
	if(bIsLiveImage)
	{		
		matSaveimage = m_liveImage;	

	}

	else
	{		
		matSaveimage = m_srcImage[nImgCnt];
	}

	if(matSaveimage.channels() == 1 && matSaveimage.depth() == CV_16U) matSaveimage.convertTo(matSaveimage, CV_8UC1, 1./16.);		
	cv::imwrite(filename, matSaveimage(rect));
	SAFE_DELETE_ARR(filename);
}
// 2017.05.11 add by ikm GUI -> Image SAVE �� - E