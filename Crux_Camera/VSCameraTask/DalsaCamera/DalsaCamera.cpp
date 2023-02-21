// Digitizer.cpp: implementation of the CDalsaCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ppl.h>

#include "DalsaCamera.h"
#include "VSCameraTask.h"

using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// 16.03.17 pixel shift tact
LONGLONG m_i64Freq;
LONGLONG m_i64Start;
LONGLONG m_i64End;

LONGLONG m_i64Start2;
LONGLONG m_i64End2;


HANDLE g_hExpEnd;

void CDalsaCamera::XferCallback(SapXferCallbackInfo *pInfo) {

	CDalsaCamera *pCam = (CDalsaCamera *)pInfo->GetContext();

	// PS
	if (pCam->m_nSequenceMode != 0)
	{
		if (pCam->Buffer_Grab->GetIndex() == 3)
		{
			if (pCam->ThreadControlValue[0].b_Proc == false)
			{
				pCam->ThreadControlValue[0].b_Proc = true;
				SetEvent(pCam->ThreadControlValue[0].Event);
			}
		}
		else if (pCam->Buffer_Grab->GetIndex() == 7)
		{
			if (pCam->ThreadControlValue[1].b_Proc == false)
			{
				pCam->ThreadControlValue[1].b_Proc = true;
				SetEvent(pCam->ThreadControlValue[1].Event);
			}
		}
	}
	// Non-PS
	else
	{
		if (pCam->Buffer_Grab->GetIndex() % 2 == 0)
		{
			if (pCam->ThreadControlValue[0].b_Proc == false)
			{
				pCam->ThreadControlValue[0].b_Proc = true;
				SetEvent(pCam->ThreadControlValue[0].Event);
			}
		}
		else if (pCam->Buffer_Grab->GetIndex() % 2 == 1)
		{
			if (pCam->ThreadControlValue[1].b_Proc == false)
			{
				pCam->ThreadControlValue[1].b_Proc = true;
				SetEvent(pCam->ThreadControlValue[1].Event);
			}
		}
	}
}

UINT ThreadControl(LPVOID lParam)
{
	ThreadControlStruct *pTCS = (ThreadControlStruct*)lParam;
	bool flag;

	WaitForSingleObject(pTCS->Event, INFINITE);
	while (pTCS->b_Thread)
	{
		int nProcessingCnt = pTCS->b_UsePS ? 4 : 1;

		for (int i = 0; i<nProcessingCnt; i++)
		{
			pTCS->ThreadValue[i]->b_Proc = true;
			SetEvent(pTCS->ThreadValue[i]->Event);
		}
		
		flag = true;
		while (flag)
		{
			flag = false;
			for (int i = 0; i<nProcessingCnt; i++)
			{
				if (pTCS->ThreadValue[i]->b_Proc)
					flag = true;
			}
		}

		*pTCS->LastDoneProcess = pTCS->SwitchNum;
		pTCS->b_Proc = false;
		SetEvent(g_hExpEnd);
		WaitForSingleObject(pTCS->Event, INFINITE);
	}
	return 0;
}

UINT ThreadProc(LPVOID lParam)
{
	ThreadStruct *pTS = (ThreadStruct*)lParam;

	WaitForSingleObject(pTS->Event, INFINITE);
	while (pTS->b_Thread)
	{
		if (pTS->b_UsePS)
		{
			for (int y = (pTS->SizeY / PROCESSING_NUM)*pTS->ThreadNum; y<(pTS->SizeY / PROCESSING_NUM)*(pTS->ThreadNum + 1); y++)
			{
				for (int x = 0; x<pTS->SizeX; x++)
				{
					pTS->Data_PS[(2 * x + 1) + 2 * pTS->SizeX*(2 * y + 0)] = pTS->Data_Grab[0][x + pTS->SizeX*y];
					pTS->Data_PS[(2 * x + 0) + 2 * pTS->SizeX*(2 * y + 0)] = pTS->Data_Grab[1][x + pTS->SizeX*y];
					pTS->Data_PS[(2 * x + 1) + 2 * pTS->SizeX*(2 * y + 1)] = pTS->Data_Grab[2][x + pTS->SizeX*y];
					pTS->Data_PS[(2 * x + 0) + 2 * pTS->SizeX*(2 * y + 1)] = pTS->Data_Grab[3][x + pTS->SizeX*y];
				}
			}
		}
		
		pTS->b_Proc = false;
		WaitForSingleObject(pTS->Event, INFINITE);
	}
	return 0;
}

CDalsaCamera::CDalsaCamera()
{
	for (int i = 0; i<MAX_BUFFER_NUMBER; i++)
		g_hExpEnd = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_lDigSizeX = 0;
	m_lDigSizeY = 0;
	m_lDigBit = 0;
	m_lDigBand = 0;

	QueryPerformanceFrequency((LARGE_INTEGER *)&m_i64Freq);
	m_bFreeRunLive = FALSE;
}

CDalsaCamera::~CDalsaCamera()
{
	Xfer->Destroy();
	Buffer_Grab->Destroy();
	Buffer_PS->Destroy();
	Acq->Destroy();
	delete[] Xfer;
	delete[] Buffer_Grab;
	delete[] Buffer_PS;
	delete[] Acq;
}

void CDalsaCamera::CameraExpose()
{
	m_GrabTime.Start();
	QueryPerformanceCounter((LARGE_INTEGER *)&m_i64Start);

	theApp.m_fnWriteTactLog(_T(">> Prepare Grab : %3.3f"), m_GrabTime.Stop(false));

	// PS
	if (m_nSequenceMode != 0)
	{
		Buffer_Grab->ResetIndex();
		Buffer_PS->ResetIndex();
		Xfer->Snap(4);
	}
	else
	{
		Buffer_Grab->ResetIndex();
		Buffer_PS->ResetIndex();
		Xfer->Snap(1);
	}

	WaitForSingleObject(g_hExpEnd, INFINITE);
}

///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// Camera Wrapper functions.

BOOL CDalsaCamera::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = FALSE;
	TCHAR strDcfFilePath[256];
	//TCHAR strTemp[256];
	//int nGrabberNo, nDigCh;									// Digitizer 정보
	//int nPortNum, nBaudrate;								// Serial Port 정보
	int nTrigMode, nAnalogGain;								// Camera 정보

															//17.08.22 Initialize.ini에서 Simulation Mode를 TRUE로 해놓으면 Camera Task 프로그램이 종료됨 by LJM
	TCHAR szSimulationFlag[6] = { 0, };        //2017.01.02 sgkim H-Project
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode"), _T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if (!_tcscmp(szSimulationFlag, _T("TRUE")))
		PostQuitMessage(0);

	GetPrivateProfileString(_T("Settings"), _T("Init File Path"), _T("D:\\CRUX\\DATA\\DCF\\Cabernet_10720x8064_12bit_Trigger_85msPW.CCF"), strDcfFilePath, 100, strInitFilePath);

	// Camera 정보
	nTrigMode = GetPrivateProfileInt(_T("Settings"), _T("Trigger Mode"), 4, strInitFilePath);
	nAnalogGain = GetPrivateProfileInt(_T("Settings"), _T("Analog Gain"), 4, strInitFilePath);

	m_nSequenceMode = GetPrivateProfileInt(_T("Settings"), _T("Pixel Shift Mode"), 4, strInitFilePath);


	int nStrLengh = _tcslen(strDcfFilePath);
	char* pDcfFilePath_Multibyte = new char[nStrLengh + 1];
	memset(pDcfFilePath_Multibyte, 0, (nStrLengh + 1) * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, strDcfFilePath, -1, pDcfFilePath_Multibyte, nStrLengh, NULL, NULL);
	Acq = new SapAcquisition(SapLocation("Xtium-CLHS_PX8_1", 0), pDcfFilePath_Multibyte);//"D:\\CRUX\\DATA\\DCF\\Cabernet_10720x8064_12bit_Trigger_85msPW.CCF");
	m_AcqDevice = new SapAcqDevice(SapLocation("Xtium-CLHS_PX8_1", 0));
	m_AcqDevice->Create();
	Acq->Create();

	Acq->GetParameter(CORACQ_PRM_CROP_WIDTH, &SizeX);
	Acq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &SizeY);

	if (Acq != NULL)
	{
		Acq->GetParameter(CORACQ_PRM_CROP_WIDTH, &m_lDigSizeX);
		Acq->GetParameter(CORACQ_PRM_CROP_HEIGHT, &m_lDigSizeY);
		Acq->GetParameter(CORACQ_PRM_BIT_ORDERING, &m_lDigBit);
		m_lDigBit = 8;
		m_lDigBand = 1;

		bRet = TRUE;
	}
	else
	{
		m_lDigSizeX = 6576;
		m_lDigSizeY = 4384;
		m_lDigBit = 8;
		m_lDigBand = 1;
	}

	Buffer_Grab = new SapBuffer(GRAB_NUM * 2, SizeX, SizeY, SapFormatMono16, SapBuffer::TypeScatterGather);
	Buffer_Grab->Create();
	Buffer_PS = new SapBuffer(2, SizeX * 2, SizeY * 2, SapFormatMono16, SapBuffer::TypeScatterGather);
	Buffer_PS->Create();

	Xfer = new SapAcqToBuf(Acq, Buffer_Grab, XferCallback, this);
	Xfer->Create();

	LastDoneProcess = 0;
	for (int k = 0; k<2; k++)
	{
		for (int i = 0; i<PROCESSING_NUM; i++)
		{
			TreadValue[k][i].b_Proc = false;
			TreadValue[k][i].b_Thread = true;
			TreadValue[k][i].b_UsePS = m_nSequenceMode != 0;
			Buffer_Grab->GetParameter(0 + 4 * k, CORBUFFER_PRM_ADDRESS, &TreadValue[k][i].Data_Grab[0]);
			Buffer_Grab->GetParameter(1 + 4 * k, CORBUFFER_PRM_ADDRESS, &TreadValue[k][i].Data_Grab[1]);
			Buffer_Grab->GetParameter(2 + 4 * k, CORBUFFER_PRM_ADDRESS, &TreadValue[k][i].Data_Grab[2]);
			Buffer_Grab->GetParameter(3 + 4 * k, CORBUFFER_PRM_ADDRESS, &TreadValue[k][i].Data_Grab[3]);
			Buffer_PS->GetParameter(k, CORBUFFER_PRM_ADDRESS, &TreadValue[k][i].Data_PS);
			TreadValue[k][i].SizeX = SizeX;
			TreadValue[k][i].SizeY = SizeY;
			TreadValue[k][i].Event = (HANDLE)CreateEvent(0, FALSE, FALSE, 0);;
			TreadValue[k][i].SwitchNum = k;
			TreadValue[k][i].ThreadNum = i;
			m_ThreadProc[k][i] = AfxBeginThread(ThreadProc, &TreadValue[k][i], THREAD_PRIORITY_NORMAL, 0);
		}
		ThreadControlValue[k].b_Proc = false;
		ThreadControlValue[k].b_Thread = true;
		ThreadControlValue[k].b_UsePS = m_nSequenceMode != 0;
		ThreadControlValue[k].Event = (HANDLE)CreateEvent(0, FALSE, FALSE, 0);
		ThreadControlValue[k].SizeX = SizeX;
		ThreadControlValue[k].SizeY = SizeY;
		for (int i = 0; i<PROCESSING_NUM; i++)
		{
			ThreadControlValue[k].ThreadValue[i] = &TreadValue[k][i];
		}
		ThreadControlValue[k].SwitchNum = k;
		//TreadControlValue[k].View=View;
		//TreadControlValue[k].View_PS=View_PS;
		ThreadControlValue[k].LastDoneProcess = &LastDoneProcess;
		m_ThreadControl[k] = AfxBeginThread(ThreadControl, &ThreadControlValue[k], THREAD_PRIORITY_NORMAL, 0);
	}

	//SetTriggerMode(nTrigMode);
	SetTriggerMode(0);	// 무조건 프리런모드

	if (m_nSequenceMode != 0) {
		m_AcqDevice->SetFeatureValue("pixelShiftMode", "On");
	}
	else {
		m_AcqDevice->SetFeatureValue("pixelShiftMode", "Off");
	}

	return bRet;
}

BOOL CDalsaCamera::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = FALSE;
	if (dExpOnTime == 0) return FALSE;
	bRet = m_AcqDevice->SetFeatureValue("ExposureTime", (int) (dExpOnTime * 1000));

	return bRet;
}

BOOL CDalsaCamera::SetStitchMode(int nMode)



{
	BOOL bRet = FALSE;
	return bRet;
}

double CDalsaCamera::GetExposureTime()
{
	BOOL bRet = FALSE;
	int nExposTime = 0;

	bRet = m_AcqDevice->GetFeatureValue("ExposureTime", &nExposTime);
	return (double)nExposTime / 1000;
}

BOOL CDalsaCamera::SetSequenceMode(int nMode)
{
	BOOL bRet = FALSE;

	WaitForSingleObject(g_hExpEnd, 1000);
	m_nSequenceMode = nMode;

	for (int k = 0; k<2; k++)
	{
		for (int i = 0; i<PROCESSING_NUM; i++)
			TreadValue[k][i].b_UsePS = m_nSequenceMode != 0;

		ThreadControlValue[k].b_UsePS = m_nSequenceMode != 0;
	}

	if (m_nSequenceMode != 0) {
		m_Ac("pixelShiftMode", "On");
	}
	else {
		m_AcqDevice->SetFeatureValue("pixelShiftMode", "Off");
	}

	return bRet;
}

int CDalsaCamera::GetSequenceMode()
{
	BOOL bRet = FALSE;
	int	n_mode;
	bRet = m_AcqDevice->GetFeatureValue("pixelShiftMode", &n_mode);
	return n_mode;
}

int CDalsaCamera::GetTriggerMode()
{
	BOOL bRet = FALSE;
	m_AcqDevice->GetFeatureValue("TriggerMode", &bRet);

	return bRet;
}

BOOL CDalsaCamera::SetTriggerMode(int nMode)
{
	if (m_bFreeRunLive)
	{
		AfxMessageBox(_T("Please Live Stop !!!"));
		return FALSE;
	}
	BOOL bRet = FALSE;

	if (nMode)
		bRet = TRUE;

	bRet = m_AcqDevice->SetFeatureValue("TriggerMode", bRet);


	return bRet;
}

double CDalsaCamera::GetAnalogGain()
{
	double dGainValue = 0;

	m_AcqDevice->GetFeatureValue("Gain", &dGainValue);

	return dGainValue;
}

BOOL CDalsaCamera::SetAnalogGain(double nGainValue)
{
	BOOL bRet = FALSE;
	if (nGainValue == 0) return FALSE;
	bRet = m_AcqDevice->SetFeatureValue("Gain", nGainValue);
	return bRet;
}

// 2017.02.06 Data Bit 설정 (8, 10, 12 bit 중 하나)
int CDalsaCamera::GetDataBit()
{
	return 0; /*m_InspCamera.GetCameraDataBit(m_eCamModel);*/
}

BOOL CDalsaCamera::SetDataBit(int nBit)
{
	return 0; /*m_InspCamera.SetCameraDataBit(m_eCamModel, nBit);*/
}

// 2017.02.06 Exposure Source 설
int CDalsaCamera::GetExposureSource()
{
	int	n_Exposmode;
	m_AcqDevice->GetFeatureValue("ExposureMode", &n_Exposmode);
	return n_Exposmode;
}

BOOL CDalsaCamera::SetExposureSource(int nMode)
{
	BOOL bRet = FALSE;

	bRet = m_AcqDevice->SetFeatureValue("ExposureMode", "Timed");
	return bRet;
}

void CDalsaCamera::WaitGrabEnd()
{
	WaitForSingleObject(g_hExpEnd, 5000);
	ResetEvent(g_hExpEnd);
	Xfer->Freeze();
	m_GrabTime.End();
}

void CDalsaCamera::SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{
	int nHeight = GetImageHeight();
	int nWidth = GetImageWidth();
	Mat mtData = cv::Mat(nHeight, nWidth, CV_16UC1);
	Mat mtData2;

	if (m_nSequenceMode != 0)
	{
		Buffer_PS->GetAddress(0, 0, (void**)&mtData.data);
	}
	// Non-PS
	else
	{
		Buffer_Grab->GetAddress(0, 0, (void**)&mtData.data);
	}

	mtData.convertTo(mtData2, CV_8UC1);

	memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), mtData2.data, mtData2.cols * mtData2.rows);
}

void CDalsaCamera::SaveFileCurBuffer(TCHAR* strSavePath)
{
	BOOL bRet = FALSE;
	char filename[200] = { 0, };
	int nBufferSize = GetImageWidth() * GetImageHeight() * 2;
	CString strFolderPath = _T("");

	strFolderPath = strSavePath;
	strFolderPath = strFolderPath.Left(strFolderPath.ReverseFind('\\'));
	if (GetFileAttributes(strFolderPath) == -1)
		bRet = CreateDirectory(strFolderPath, NULL);

	Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_16UC1);

	byte* pData = 0;
	Buffer_Grab->GetAddress(0, 0, (void**)&pData);
	memcpy(matLive.data, pData, nBufferSize);

	int nLen = WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strSavePath, -1, filename, nLen, NULL, NULL);

	bRet = imwrite(filename, matLive);
}

void CDalsaCamera::DisplayLive(HWND hDispWnd)
{
	assert(true, _T("Not Implemented (DisplayLive)"));
}

BOOL CDalsaCamera::StartLiveGrab()
{
	if (!m_bFreeRunLive)
	{
		theApp.m_fnWriteTactLog(_T("Start Live Grab"));

		m_bFreeRunLive = TRUE;

		Buffer_Grab->ResetIndex();
		Buffer_PS->ResetIndex();
		Xfer->Grab();

		return TRUE;
	}

	return FALSE;
}

BOOL CDalsaCamera::StopLiveGrab()
{
	if (m_bFreeRunLive)
	{
		Xfer->Freeze();

		WaitForSingleObject(g_hExpEnd, 1000);
		theApp.m_fnWriteTactLog(_T("WaitForSingleObject grab end"));
		ResetEvent(g_hExpEnd);
		theApp.m_fnWriteTactLog(_T("Stop Live Grab"));

		m_bFreeRunLive = FALSE;

		return TRUE;
	}

	return FALSE;
}

void CDalsaCamera::SaveGrabImage(CString strFilePath)
{
	char* filename = CSTR2PCH(strFilePath);
	int nHeight = GetImageHeight();
	int nWidth = GetImageWidth();
	Mat mtData = cv::Mat(nHeight, nWidth, CV_16UC1);

	if (m_nSequenceMode != 0)
	{
		Buffer_PS->GetAddress(0, 0, (void**)&mtData.data);
		// Buffer_PS->Save 는 무슨이유인지 모르겠는데 저장이 안됨
		//Buffer_PS->Save(strFilePath, "-format bmp", LastDoneProcess);
	}
	else
	{
		Buffer_Grab->GetAddress(0, 0, (void**)&mtData.data);
		//Buffer_Grab->Save(strFilePath, "-format bmp", LastDoneProcess);
	}

	cv::imwrite(filename, mtData);
}

void CDalsaCamera::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
	assert(true, _T("Not Implemented (SaveGrabImage)"));
}

// 원본 이미지
void CDalsaCamera::GetGrabImage(byte* byteImgArr)
{
	// PS
	if (m_nSequenceMode != 0)
	{
		Buffer_PS->GetAddress(0, 0, (void**)&byteImgArr);
	}
	// Non-PS
	else
	{
		Buffer_Grab->GetAddress(0, 0, (void**)&byteImgArr);
	}
}

// Crop Image
void CDalsaCamera::GetGrabImage(int nImgCnt, BOOL bIsLiveImage/*, BOOL bIsGrayImg*/, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	Mat matResize;
	Rect rect(lOffsetX, lOffsetY, lCropSizeX, lCropSizeY);

	if (bIsLiveImage)
	{
		static Mat matLive;
		Mat matLive2;/* = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC1);*/

		// matLive 사이즈가 달라지면 (PS 모드 변경으로 인해)
		if (matLive.rows != GetImageHeight())
		{
			matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_16UC1);
		}

		ResetEvent(g_hExpEnd);
		WaitForSingleObject(g_hExpEnd, INFINITE);

		// PS
		if (m_nSequenceMode != 0)
		{
			Buffer_PS->GetAddress(0, 0, (void**)&matLive.data);
		}
		// Non-PS
		else
		{
			Buffer_Grab->GetAddress(0, 0, (void**)&matLive.data);
		}

		matLive.convertTo(matLive2, CV_8UC1);
		cv::resize(matLive2(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);
		memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
	}
}


// 입력값 : 원하는 평균 밝기 & 밝기를 맞추고자 하는 영역
// 출력값 : 노출시간 & 게인
void CDalsaCamera::GetBrightnessVal(int nWantMean, CRect rect, int& nOutExp, int& nOutGain)
{
//	// 노출 시간 초기값
//	int nExp = 5;
//	int nExpStep = 200;
//	const int nMaxExp = 601;
//
//	// 카메라 게인 초기값
//	int nGain = 30;
//	int nGainStep = 30;
//	const int nMaxGain = 300;
//
///*	if (m_eCamModel == eVA47M5_Series)
//		SetTriggerMode(eStandard);
//	else
//		SetTriggerMode(eOverlap)*/;
//
//	SetSequenceMode(eNone);
//
//	SetExposureTime(nMaxExp);
//
//	// Gain 조절 Loop
//	while (nGain <= nMaxGain)
//	{
//		SetAnalogGain(nGain);
//
//		// Grab OpenCV 인 경우 라이브 영상
//		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
//		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
//		//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
//		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
//
//		GlobalFree(Image);
//
//		//MIL_ID milBuf = GetMilGrabBuffer();
//
//		// 평균 GV값 계산
//		double dMean = GetMeanValue(milBuf, rect);
//
//		// 현재 평균GV가 원하는 평균 GV보다 낮은 경우, Gain 올려서 다시 Loop ( Exp는 최대 상태 )
//		// GV가 딱 맞는 경우도 있을수 있음 ( 2GV 정도 더 남겨 둠 )
//		if (nWantMean + 2 > dMean)
//			nGain += nGainStep;
//		// 만족 하는 경우 Loop 나가기
//		else
//			break;
//	}
//
//	// 최대치보다 게인값이 높은 경우, 만족하는 셋팅 없음 ( Exp는 최대 상태 )
//	if (nGain > nMaxGain)	return;
//
//	SetAnalogGain(nGain);
//
//	// Exp 조절 Loop ( 0보다 큰 경우만 )
//	while (nExpStep > 0)
//	{
//		SetExposureTime(nExp + nExpStep);
//
//		// 라이브 영상
//		// Grab OpenCV 인 경우 라이브 영상
//		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
//		byte* Image = (byte *)GlobalAlloc(GMEM_FIXED, nBufferSize);
//		//SVGigE_RETURN result = Image_getImageRGB(m_Image, Image, nBufferSize,BAYER_METHOD_HQLINEAR); 
//		cv::Mat matBuf = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
//		//MIL_ID milBuf = GetMilGrabBuffer();
//
//		// 평균 GV값 계산
//		double dMean = GetMeanValue(milBuf, rect);
//
//		// 원하는 평균 밝기값과 현재 평균 밝기값 비교
//		if (nWantMean > dMean)
//			nExp += nExpStep;
//		// Step 반으로 해서 다시 시도
//		else
//			nExpStep /= 2;
//	}
//
//	// 결과
//	nOutExp = nExp;
//	nOutGain = nGain;
}


BOOL CDalsaCamera::OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
{
	return TRUE;
}

BOOL CDalsaCamera::InitializeGrabber(int nGrabberNo, int nDigCh, CString strDcfFilePath)
{
	return TRUE;
}

void CDalsaCamera::DisplayGrabImage(HWND hDispWnd)
{

}

double CDalsaCamera::GetMeanValue(cv::Mat &matBuf, CRect rect)
{
	if (matBuf.empty())	return 0;

	cv::Rect rectROI(rect.left, rect.top, rect.Width(), rect.Height());

	// 영역 자르기
	cv::Mat matTempBuf = matBuf(rectROI);

	// 평균 구하기
	cv::Scalar m;
	m = cv::mean(matTempBuf);

	// 메모리 할당 해제
	matTempBuf.release();

	return (double)m[0];
}

void CDalsaCamera::TestFunc(int nGV)
{

}

void CDalsaCamera::m_fnSetBufImage(TCHAR* strImgPath, int nImgCnt, /*byte* imageBub,*/ CRect crect, BOOL bIsGrayImg/*, int nBufSize*/)
{
	
}
