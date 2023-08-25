#include "stdafx.h"
#include <ppl.h>

#include "TestCam.h"
#include "VSCameraTask.h"
#include "DalsaLineParam.hpp"


using namespace Concurrency;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


HANDLE g_hGrabEnd;
int ProcessGrabCnt = 0;
CTestCam::CTestCam()
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

CTestCam::~CTestCam()
{
}

void CTestCam::CloseApp()
{   
	
}

void CTestCam::CameraExpose(CString PanelID, CString VirID, CString Position, int nBufCnt)
{	

	//int nTriggerCountF = stLine.stLineData[stLine.stLineData[0].nCurrentGrab].nCOUNTF;
	//int nTriggerCountB = stLine.stLineData[stLine.stLineData[0].nCurrentGrab].nCountB;

	//CString strpos = stLine.stLineData[stLine.stLineData[0].nCurrentGrab].strDirection;
	//if (strpos == _T("FORWARD"))
	//{
	//	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("ScanDirection"), M_TYPE_STRING, MIL_TEXT("Reverse"));
	//}
	//else
	//{
	//	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("ScanDirection"), M_TYPE_STRING, MIL_TEXT("Forward"));
	//}


	//MimControl(m_milSystem, M_GRAB_DIRECTION_Y, M_REVERSE);
	//MIL_INT Orientation = MimInquire(m_milSystem, M_GRAB_DIRECTION_Y, M_NULL);	


	//CString TotalPath;
	//TotalPath.Format(_T("D:\\CamGrabTest\\%s"), VirID);
	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposrorcp ure Start, Max Count : %d"), nBufCnt);
	StopGrab(nBufCnt);
	m_GrabTime.Start();
	StartGrab(PanelID, VirID, Position, nBufCnt, false, true);
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera Exposure End, Max Count : %d"), nBufCnt);
}

BOOL CTestCam::InitializeCamera(CString strInitFilePath)
{
	BOOL bRet = TRUE;

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
	imageY = _ttoi(lib->GetDataFromINI(_T("Trigger"), _T("ImageY"), lib->GetData(_T("inifile"))));

	m_Camera = new CDalsaLineCamControl;
	/*if (LinkType == eCameraLink)
	{
		bRet = OpenCameraComPort(CameraPort, CameraBaud, m_eCamModel);
	//}
	//else
	//{*/
	//	if (m_Camera->ConnectCameraSerial(CameraPort, CameraBaud) == FALSE)
	//		AfxMessageBox(_T("Camera Serial Connection Error !!"));
	////}
	//

	//m_Trigger = new CTriggerControl;
	//if (m_Trigger->ConnectTrigger(TriggerPort, TriggerBaud) == FALSE)
	//	AfxMessageBox(_T("Trigger Serial Connection Error !!"));

	if (!InitGrabber(FrameGrabberNo, DigitizerChannel, DcfFile))
		return FALSE;

	//트리커 초기 세팅
	//m_Trigger->TriggerOrigin1();
	//m_Trigger->StopTriggerGen0();
	//m_Trigger->StopTriggerGen1();
	//m_Trigger->TriggerInit0();
	//m_Trigger->TriggerInit1();
	////////

	//BOOL bRet = FALSE;

	// Camera 정보
	m_nCameraNum	= GetPrivateProfileInt(_T("Settings"), _T("Camera Number"), 4, strInitFilePath);
	TCHAR szCamType[100] = {0, }, szDefaultGain[5] = {0, };
	GetPrivateProfileString(_T("Settings"), _T("Camera Type"), _T("Coaxial"), szCamType, sizeof(szCamType), strInitFilePath);
	GetPrivateProfileString(_T("Settings"), _T("Default Gain Offset"), _T("0.0"), szDefaultGain, sizeof(szDefaultGain), strInitFilePath);
	SetDefaultGainOffset(_ttof(szDefaultGain));

	CString strCamType, strTemp;
	strCamType.Format(_T("%s"), szCamType);

	//SetExposureTime(777);

	return bRet;
}
CString CTestCam::GetBoardName()
{
	CString Name;
	MIL_INT SystemDescriptorStringSize = 0;
	MsysInquire(m_milSystem, M_SYSTEM_DESCRIPTOR_SIZE, &SystemDescriptorStringSize);
	MIL_TEXT_PTR SystemDescriptorPtr = new MIL_TEXT_CHAR[SystemDescriptorStringSize];
	MsysInquire(m_milSystem, M_SYSTEM_DESCRIPTOR, SystemDescriptorPtr);
	Name.Format(_T("%s"), SystemDescriptorPtr);
	delete[] SystemDescriptorPtr;
	return Name;
}
CString CTestCam::GetCameraType()
{
	CString Name;
	MIL_INT64 DeviceScanTypeStringSize = 0;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("DeviceScanType"), M_TYPE_MIL_INT, &DeviceScanTypeStringSize);
	MIL_TEXT_PTR DeviceScanTypePtr = new MIL_TEXT_CHAR[DeviceScanTypeStringSize];
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("DeviceScanType"), M_TYPE_STRING, DeviceScanTypePtr);
	Name.Format(_T("%s"), DeviceScanTypePtr);
	delete[] DeviceScanTypePtr;

	return Name;
}
CString CTestCam::GetCameraName()
{
	//char Temp[100];
	//char* ptrTemp = Temp;

	CString Name;
	//Name = ptrTemp;


	MIL_INT DeviceModelNameStringSize = 0;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("DeviceModelName"), M_TYPE_MIL_INT, &DeviceModelNameStringSize);
	MIL_TEXT_PTR DeviceModelNamePtr = new MIL_TEXT_CHAR[DeviceModelNameStringSize];
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("DeviceModelName"), M_TYPE_STRING, DeviceModelNamePtr);
	Name.Format(_T("%s"), DeviceModelNamePtr);

	delete[] DeviceModelNamePtr;
	return Name;
}
int CTestCam::GetCameraWidth()
{
	return m_nImageWidth;
}
int CTestCam::GetCameraHeight()
{
	return m_nImageHeight;
}
int CTestCam::GetCameraDepth()
{
	return m_nBandWidth;
}
double CTestCam::GetCameraTemperature()
{
	MIL_DOUBLE Temp = 0;
	MsysInquire(m_milSystem, M_TEMPERATURE_FPGA, &Temp);

	return (double)Temp;
}

bool CTestCam::InitGrabber(int nGrabberNo, int nDigCh, CString strDcfFile)
{
	// MIL APP - SYSTEM
	// 시스템 할당
		MappAlloc(M_DEFAULT, &m_milApplication);
		if (m_milApplication == M_NULL)	return false;
		//1215
		//MsysAlloc(M_SYSTEM_SOLIOS, nGrabberNo, M_DEFAULT, &m_milSystem);
		//if (m_milSystem == M_NULL)	return false;

		// 프레임 보드 할당
		MsysAlloc(M_SYSTEM_RAPIXOCXP, nGrabberNo, M_DEFAULT, &m_milSystem);
		if (m_milSystem == M_NULL)	return false;

		// 디지타이저 할당
		MdigAlloc(m_milSystem, nDigCh, strDcfFile, M_DEFAULT, &m_milDigitizer);
		if (m_milDigitizer == M_NULL)	return false;

		MappControl(M_ERROR, M_PRINT_DISABLE);

		m_lDigSizeX = (long)MdigInquire(m_milDigitizer, M_SIZE_X,M_NULL);
		m_lDigSizeY = (long)MdigInquire(m_milDigitizer, M_SIZE_Y, M_NULL);
		m_lDigBit = MdigInquire(m_milDigitizer, M_SIZE_BIT, M_NULL);
		m_lDigBand = MdigInquire(m_milDigitizer, M_SIZE_BAND, M_NULL);
		m_nImageWidth = (int)m_lDigSizeX;
		m_nImageHeight = (int)m_lDigSizeY;
		m_nBitRate = (int)m_lDigBit;
		m_nBandWidth = (int)m_lDigBand;
		if ((m_lDigSizeX < 0)|| (m_lDigSizeY < 0) ||(m_lDigBit < 0 )||(m_lDigBand < 0))
			return FALSE;
	
		

	//MdigHookFunction(m_milDigitizer, M_GRAB_START, (MIL_DIG_HOOK_FUNCTION_PTR)HookGrabStart, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_END, (MIL_DIG_HOOK_FUNCTION_PTR)HookGrabEnd, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_START, (MIL_DIG_HOOK_FUNCTION_PTR)HookFrameStart, this);
	//MdigHookFunction(m_milDigitizer, M_GRAB_FRAME_END, (MIL_DIG_HOOK_FUNCTION_PTR)HookFrameEnd, this);

	//if (!m_fnInitializeImageBuffer())
	//	return FALSE;

	return true;
}

void CTestCam::StartGrab(int nTriggerCountF, int nTriggerCountB, CString strpos ,bool sync, bool fileSave)
{
	int count = nTriggerCountF > nTriggerCountB ? nTriggerCountF : nTriggerCountB;
	//count -= 1;
	// 버퍼할당
	AllocClearBuffer(count);
	// UserHookData 초기화
	UserHookData.obj = this;
	UserHookData.isGrabEnd = false;
	UserHookData.ProcessedImageCount = 0;
	UserHookData.MaxCount = count;
	UserHookData.isSaveImage = true/*fileSave*/;
	ResetEvent(UserHookData.hGrabEnd);

	// 그랩 스타트
	m_GrabFlag = true;
	theApp.m_fnWriteLineScanLog(_T("Grab Start"));

	theApp.m_fnWriteLineScanLog(_T("MdigProcess Start"));
	// M_SEQUENCE + M_COUNT(Count) : 패킷의 총 길이
	MdigProcess(m_milDigitizer, m_milLineImage, count, M_SEQUENCE + M_COUNT(count), M_DEFAULT, ProcessingFunction, &UserHookData);
	theApp.m_fnWriteLineScanLog(_T("MdigProcess End"));
	if (sync)
	{
		WaitForSingleObject(UserHookData.hGrabEnd, 10000);
	}
}

void CTestCam::StartGrab(CString PanelID, CString VirID, CString Position, int nBufCnt, bool sync, bool fileSave)
{
	//count -= 1;
	// 버퍼할당
	//MdigControlFeature(m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("AcquisitionStart"), M_DEFAULT, M_NULL);
	theApp.m_fnWriteLineScanLog(_T("Grab Start, BufCnt : %d"), nBufCnt);

	MIL_BOOL AcquisitionStatus = M_FALSE;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("AcquisitionStatus"), M_TYPE_BOOLEAN, &AcquisitionStatus);
	theApp.m_fnWriteLineScanLog(_T("MdigProcess Acquisition Status : %d"), AcquisitionStatus);

	AllocClearBuffer(nBufCnt);
	CString TotalPath_;
	TotalPath_.Format(_T("D:\\CamGrabTest\\%s"), PanelID);
	// UserHookData 초기화
	CString TotalPath;
	TotalPath.Format(_T("D:\\CamGrabTest\\%s\\%s"), PanelID, Position);


	UserHookData.obj = this;
	UserHookData.isGrabEnd = false;
	UserHookData.ProcessedImageCount = 0;
	UserHookData.MaxCount = nBufCnt;
	UserHookData.isSaveImage = fileSave;
	UserHookData.PanelID = PanelID;
	UserHookData.VirID = VirID;
	UserHookData.SavePath = TotalPath;
	UserHookData.DirectSave = true;
	ResetEvent(UserHookData.hGrabEnd);
	if (GetFileAttributes((LPCTSTR)TotalPath_) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectory(TotalPath_, NULL);
	}
	if (GetFileAttributes((LPCTSTR)TotalPath) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectory(TotalPath, NULL);
	}
	ProcessGrabCnt = 0;
	// 그랩 스타트
	SetTriggerMode(eStandard);
	SetSequenceMode(1);
	SetSequenceIndex(0);

	m_GrabFlag = true;
	//MIL_INT64 SequencerModeStringSize = 0;
	//MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("SequencerMode"), M_TYPE_MIL_INT, &SequencerModeStringSize);
	//theApp.m_fnWriteLineScanLog(_T("Current Sequencer Prop Size : %d"), SequencerModeStringSize);


	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerConfigurationMode"), M_TYPE_STRING, MIL_TEXT("On"));

	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetSelector"), M_TYPE_STRING, MIL_TEXT("1"));

	//MIL_INT64 SequencerSetNext = 0;
	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetNext"), M_TYPE_INT64, &SequencerSetNext);

	//MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetNext"), M_TYPE_INT64, &SequencerSetNext);
	//theApp.m_fnWriteLineScanLog(_T("Current Sequencer Value : %d"), SequencerSetNext);


	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerConfigurationMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("On"));

	

	theApp.m_fnWriteLineScanLog(_T("MdigProcess Start"));

	MdigProcess(m_milDigitizer, m_milLineImage, nBufCnt, M_SEQUENCE + M_COUNT(nBufCnt), M_ASYNCHRONOUS, ProcessingFunction, &UserHookData);
	theApp.m_fnWriteLineScanLog(_T("MdigProcess End"));
	if (sync)
	{
		WaitForSingleObject(UserHookData.hGrabEnd, 10000);
	}
}

void CTestCam::StopGrab(int nTriggerCountF, int nTriggerCountB)
{
	int count = nTriggerCountF > nTriggerCountB ? nTriggerCountF : nTriggerCountB;
	//count -= 1;
	// 그랩 종료

	theApp.m_fnWriteLineScanLog(_T("Grab Stop"));
	MdigProcess(m_milDigitizer, m_milLineImage, count, M_STOP, M_DEFAULT, ProcessingFunction, &UserHookData);
	

	SetEvent(UserHookData.hGrabEnd);
	m_GrabFlag = false;
}
int CTestCam::StopGrab(int nBufCnt)
{	
	//count -= 1;
	// 그랩 종료
	//MdigControlFeature(m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("AcquisitionStop"), M_DEFAULT, M_NULL);


	theApp.m_fnWriteLineScanLog(_T("Grab Stop Enter, BufCnt : %d, Proc Cnt : %d"), nBufCnt, ProcessGrabCnt);
	MIL_BOOL AcquisitionStatus = M_FALSE;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("AcquisitionStatus"), M_TYPE_BOOLEAN, &AcquisitionStatus);
	theApp.m_fnWriteLineScanLog(_T("MdigProcess Acquisition Status : %d"), AcquisitionStatus);

	MdigProcess(m_milDigitizer, m_milLineImage, nBufCnt, M_STOP, M_DEFAULT, ProcessingFunction, &UserHookData);	


	theApp.m_fnWriteLineScanLog(_T("Grab Stop Done, BufCnt : %d, Proc Cnt : %d"), nBufCnt, ProcessGrabCnt);
	SetEvent(UserHookData.hGrabEnd);
	m_GrabFlag = false;
	return ProcessGrabCnt;
}


void CTestCam::AllocClearBuffer(int nBufCnt, bool onlyClear)
{
	try
	{


		theApp.m_fnWriteLineScanLog(_T("AllocClearBuffer Start"));

		for (int i = 0; i < nBufCnt; i++)
		{
			theApp.m_fnWriteLineScanLog(_T("Buf Free Count : %d"), i);
			if (m_milLineImage[i] > 0)
			{
				MbufFree(m_milLineImage[i]);
				m_milLineImage[i] = M_NULL;
			}
		}
		theApp.m_fnWriteLineScanLog(_T("Buf Free End"));
		//if (m_milMergeImage > 0)
		//{
		//	MbufFree(m_milMergeImage);
		//	m_milMergeImage = M_NULL;
		//}

		int Band = GetImageBandwidth();
		int DigBit = GetImageBitrate();
		int y = GetImageHeight();
		int x = GetImageWidth();
		theApp.m_fnWriteLineScanLog(_T("Get Size"));
		// 총 이미지 개수
		// ex.. BrightField, DarkField 20장씩 찍는다면 Buf Cnt는 40개가 되어야 함
		for (int i = 0; i < nBufCnt; ++i)
		{
			MbufAlloc2d(m_milSystem, x, y, DigBit, M_IMAGE + M_GRAB + M_PROC, &m_milLineImage[i]);

			MbufClear(m_milLineImage[i], 0xFF);
		}
		theApp.m_fnWriteLineScanLog(_T("Buf Alloc"));
		//MbufAlloc2d(m_milSystem, x, y * nBufCnt, DigBit, M_IMAGE + M_GRAB + M_PROC, &m_milMergeImage);

		//MbufClear(m_milMergeImage, 0xFF);


		//for (int i = 0; i < nBufCnt; i++)
		//{

		//	MbufChild2d(m_milMergeImage, 0, i * y, x, y, &m_milLineImage[i]);
		//	MbufClear(m_milLineImage[i], 0xFF);

		//}

		// 촬영 타임아웃이 없음 지속촬영(Stop하기 전까지)
		MdigControl(m_milDigitizer, M_GRAB_TIMEOUT, M_INFINITE);
		theApp.m_fnWriteLineScanLog(_T("AllocClearBuffer End"));
	}
	catch (Exception ex)
	{
		theApp.m_fnWriteLineScanLog((LPCTSTR)ex.msg.c_str());
	}
}

MIL_INT CTestCam::ProcessingFunction(MIL_INT HookType, MIL_ID HookId, void *HookDataPtr)
{
	//CTestCam* temp = (CTestCam*)theApp.m_pCamera;
	//temp->m_Trigger->TriggerGenCount0();
	//temp->m_Trigger->TriggerCurrentPosition();
	HookDataStruct *UserHookDataPtr = (HookDataStruct*)HookDataPtr;
	
	//MIL_ID ModifiedBufferId;
	MIL_INT ModifiedBufferIndex;
	// 해당 훅의 정보를 가져온다.
	// 수정된 버퍼 아이디를 가져온다.
	//MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_ID, &ModifiedBufferId);
	// 수정된 버퍼 인덱스를 가져온다.
	MdigGetHookInfo(HookId, M_MODIFIED_BUFFER + M_BUFFER_INDEX, &ModifiedBufferIndex);
	// 다음 훅이 들어올 때 증가된 프로세스 카운트
	
	UserHookDataPtr->ProcessedImageCount = ModifiedBufferIndex + 1;
	ProcessGrabCnt = UserHookDataPtr->ProcessedImageCount;
	theApp.m_fnWriteLineScanLog(_T("ProcessingFunction - %d/%d"), UserHookDataPtr->ProcessedImageCount, UserHookDataPtr->MaxCount);

	if (UserHookDataPtr->MaxCount == UserHookDataPtr->ProcessedImageCount)
	{
		//if(!UserHookDataPtr->isGrabEnd)
		//{
		UserHookDataPtr->obj->SetImageCallBackState(0);

		//MdigControlFeature(UserHookDataPtr->obj->m_milDigitizer, M_FEATURE_EXECUTE, MIL_TEXT("AcquisitionStop"), M_DEFAULT, M_NULL);
		MdigProcess(UserHookDataPtr->obj->m_milDigitizer, UserHookDataPtr->obj->m_milLineImage, UserHookDataPtr->MaxCount, M_STOP, M_DEFAULT, ProcessingFunction, &UserHookDataPtr);
		SetEvent(UserHookDataPtr->hGrabEnd);
		//MdigControlFeature(UserHookDataPtr->obj->m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("Off"));
		theApp.m_fnWriteLineScanLog(_T("Grab End, MaxCount : %d"), UserHookDataPtr->MaxCount);
		UserHookDataPtr->obj->m_GrabFlag = false;
		if (UserHookDataPtr->isSaveImage)
		{
			if (!UserHookDataPtr->DirectSave)
			{
				for (int idx = 0; idx < ModifiedBufferIndex; ++idx)
				{
					struct tm curr_tm;
					time_t curr_time = time(nullptr);
					_localtime64_s(&curr_tm, &curr_time);
					CString filePath;
					filePath.Format(_T("%s\\%d.bmp"), UserHookDataPtr->SavePath, idx);

					theApp.m_fnWriteLineScanLog(_T("image save Start."));
					//temp->m_Trigger->TriggerGenCount0();
					MbufExport(filePath, M_BMP, UserHookDataPtr->obj->m_milLineImage[idx]);
					theApp.m_fnWriteLineScanLog(_T("image save End."));
				}
			}
		}

	}
	if (UserHookDataPtr->isSaveImage)
	{
		if (UserHookDataPtr->DirectSave)
		{
			struct tm curr_tm;
			time_t curr_time = time(nullptr);
			_localtime64_s(&curr_tm, &curr_time);
			CString filePath;
			filePath.Format(_T("%s\\%d.bmp"), UserHookDataPtr->SavePath, UserHookDataPtr->ProcessedImageCount);

			theApp.m_fnWriteLineScanLog(_T("image save Start."));
			//temp->m_Trigger->TriggerGenCount0();
			MbufExport(filePath, M_BMP, UserHookDataPtr->obj->m_milLineImage[ModifiedBufferIndex]);
			theApp.m_fnWriteLineScanLog(_T("image save End."));
		}

	}
	return 0;
}

int CTestCam::CloseCamera()
{
	//m_Trigger->ClosePort();
	m_Camera->m_CamSerial->CloseComPort();
	BUFFREE(m_milMergeImage);
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		BUFFREE(m_milLineImage[i]);
		BUFFREE(m_milLineImageBack[i]);
	}
		
	MdigFree(m_milDigitizer);
	MsysFree(m_milSystem);
	MappFree(m_milApplication);
	return 1;
}

int CTestCam::freeCamera()
{
	/*m_Trigger->ClosePort();
	m_Camera->m_CamSerial->CloseComPort();*/
	BUFFREE(m_milMergeImage);
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		BUFFREE(m_milLineImage[i]);
		BUFFREE(m_milLineImageBack[i]);
	}
		
	MdigFree(m_milDigitizer);
	MsysFree(m_milSystem);
	MappFree(m_milApplication);
	return 1;
}

BOOL CTestCam::SetExposureTime(double dExpOnTime)
{
	BOOL bRet = TRUE;
	m_Camera->SetProperty("set", (int)(dExpOnTime * 1000));
	
	return bRet;
}

BOOL CTestCam::SetStitchMode(int nMode)
{
	BOOL bRet = FALSE;
	return bRet;
}

double CTestCam::GetExposureTime()
{
	return 0 / 1000.0;
}

int CTestCam::GetTriggerMode()
{
	return 0;
}

BOOL CTestCam::SetTriggerMode(int nMode)
{
	CString Temp;

	
	MIL_INT64 TriggerModeStringSize = 0;
	//MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("TriggerSelector"), M_TYPE_STRING, MIL_TEXT("ExposureStart"));
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("TriggerMode"), M_TYPE_MIL_INT, &TriggerModeStringSize);
	MIL_TEXT_PTR TriggerModePtr = new MIL_TEXT_CHAR[TriggerModeStringSize];
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("TriggerMode"), M_TYPE_STRING, TriggerModePtr);
	Temp.Format(_T("%s"), TriggerModePtr);
	theApp.m_pLogWriter->m_fnWriteLog(_T("Current Trigger Mode : %s"), *Temp);

	if (nMode == 0)
	{
		MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("TriggerMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	}
	else
	{
		MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("TriggerMode"), M_TYPE_STRING, MIL_TEXT("On"));
	}
	delete[] TriggerModePtr;
	return TRUE;
}

double CTestCam::GetAnalogGain()
{
	return (double) 0;
}

BOOL CTestCam::SetAnalogGain(double dGainValue)
{
	BOOL bRet = FALSE;
	return bRet;
}

void CTestCam::DisplayLive(HWND hDispWnd)
{

}

void CTestCam::WaitGrabEnd()
{	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd Start"));
	//WaitForSingleObject(UserHookData.hGrabEnd, 10000);
	//ResetEvent(UserHookData.hGrabEnd);
	m_GrabTime.End();	
	theApp.m_pLogWriter->m_fnWriteLog(_T("Camera WaitGrabEnd End"));
}

void CTestCam::SetSMemCurBuffer(int TriggerCountF, int TriggerCountB, UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode)
{
	if (m_lDigBand == 1)
	{
		int line = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;		
		int width = theApp.m_pSharedMemory->GetImgWidth();
		int height = theApp.m_pSharedMemory->GetImgHeight();
		MbufGet2d(m_milMergeImage, 0, 0, /*GetImageWidth()*/m_lDigSizeX/*width*/, /*GetImageHeight()*/m_lDigSizeY * line/*height*/, theApp.m_pSharedMemory->GetImgAddress(nGrabNum));

		
		//MbufGet2d(m_milMergeImage, 0, 0, GetImageWidth()/*m_lDigSizeX*/,GetImageHeight()/*m_lDigSizeY*/ * line/*m_lDigSizeY*/, theApp.m_pSharedMemory->GetImgAddress(nGrabNum));
	}
	else if (m_lDigBand == 3)
	{
		int line = TriggerCountF > TriggerCountB ? TriggerCountF : TriggerCountB;
		/*line -= 1;*/
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth() * line;
		byte* Image = new byte[nBufferSize];
		MbufGetColor(m_milMergeImage, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
		memcpy(theApp.m_pSharedMemory->GetImgAddress(nGrabNum), Image, nBufferSize);
		SAFE_DELETE_ARR(Image);
	}
}
/// Sequencer 모드 대응
int CTestCam::SetSMemCurBuffer(int nBufCnt , TCHAR* strPanelID)
{
	if (m_lDigBand == 1)
	{
		int line = nBufCnt;
		int width = theApp.m_pSharedMemory->GetImgWidth();
		int height = theApp.m_pSharedMemory->GetImgHeight();

		for (int i = 0; i < nBufCnt; ++i)
		{			
			MbufGet2d(m_milLineImage[i], 0, 0, width, height, theApp.m_pSharedMemory->GetImgAddress(i));
		}



		//MbufGet2d(m_milMergeImage, 0, 0, GetImageWidth()/*m_lDigSizeX*/,GetImageHeight()/*m_lDigSizeY*/ * line/*m_lDigSizeY*/, theApp.m_pSharedMemory->GetImgAddress(nGrabNum));
	}
	else if (m_lDigBand == 3)
	{
		int line = nBufCnt;
		/*line -= 1;*/
		int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
		byte* Image = new byte[nBufferSize];
		for (int i = 0; i < nBufCnt; ++i)
		{
			MbufGetColor(m_milLineImage[i], M_PACKED + M_BGR24, M_ALL_BANDS, Image);
			memcpy(theApp.m_pSharedMemory->GetImgAddress(i), Image, nBufferSize);
		}
		

		SAFE_DELETE_ARR(Image);
	}
	return ProcessGrabCnt;
}

void CTestCam::SaveFileCurBuffer(TCHAR* strSavePath)
{
	m_fnMbufExport(strSavePath, m_milMergeImage);
}

BOOL CTestCam::StartLiveGrab()
{
	m_bLiveMode = TRUE;
	theApp.m_fnWriteTactLog(_T("Start LiveGrab"));


	SetSequenceMode(eNone);
	SetTriggerMode(eFreeRun);
	Sleep(10);

	m_GrabTime.Start();

	// Serial 통신 오류로 인해 Camera Mode 변경되지 않았을 경우 연속 Grab 호출 -> 사용 안함
	if (GetTriggerMode() != eFreeRun)
	{
		AfxMessageBox(_T("Trigger Mode Error !!"));
		//bRet = FALSE;
		// 		m_bTriggerLive = TRUE;
		// 		m_fnSnapAndTrigger();
	}
	else
	{
		m_bFreeRunLive = TRUE;
		MdigGrabContinuous(m_milDigitizer, m_LiveImage);
		//bRet = TRUE;
	}


	return TRUE;
}
BOOL CTestCam::SetSequenceMode(int nSeqMode)
{	
	MIL_INT64 SequencerModeStringSize = 0;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("SequencerMode"), M_TYPE_MIL_INT, &SequencerModeStringSize);
	theApp.m_fnWriteLineScanLog(_T("Current Sequencer Prop Size : %d"), SequencerModeStringSize);

	if(nSeqMode == 0)
		MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	if (nSeqMode == 1)
		MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("On"));
	return TRUE;
}
BOOL CTestCam::SetSequenceIndex(int nSeqIdx)
{
	theApp.m_fnWriteLineScanLog(_T("Start Set Sequence Index"));
	MIL_INT64 SequencerModeStringSize = 0;
	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE + M_STRING_SIZE, MIL_TEXT("SequencerMode"), M_TYPE_MIL_INT, &SequencerModeStringSize);
	theApp.m_fnWriteLineScanLog(_T("Current Sequencer Prop Size : %d"), SequencerModeStringSize);

	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerConfigurationMode"), M_TYPE_STRING, MIL_TEXT("On"));

	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetSelector"), M_TYPE_STRING, MIL_TEXT("1"));

	MIL_INT64 SequencerSetNext = nSeqIdx;
	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetNext"), M_TYPE_INT64, &SequencerSetNext);

	MdigInquireFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerSetNext"), M_TYPE_INT64, &SequencerSetNext);
	theApp.m_fnWriteLineScanLog(_T("Current Sequencer Value : %d"), SequencerSetNext);

	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerConfigurationMode"), M_TYPE_STRING, MIL_TEXT("Off"));
	MdigControlFeature(m_milDigitizer, M_FEATURE_VALUE, MIL_TEXT("SequencerMode"), M_TYPE_STRING, MIL_TEXT("On"));

	return TRUE;
}
BOOL CTestCam::StopLiveGrab()
{
	m_bLiveMode = FALSE;
	theApp.m_fnWriteTactLog(_T("StopLiveGrab"));
	return TRUE;
}

int CTestCam::GetLivePixelInfo(CPoint ptBuf)
{
	return 0;
}

void CTestCam::SaveGrabImage(CString strFilePath)
{
}

void CTestCam::SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
{
}

BYTE* CTestCam::GetGrabBuffer()
{
	return 0;
}

BOOL CTestCam::DoRotateImage(cv::Mat matSrcBuffer, cv::Mat& matDstBuffer, double dAngle)
{
	return TRUE;
}

void CTestCam::GetGrabImage(byte* byteImgArr)
{
}

//void CTestCam::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
//{
	//Mat matResize;
	//Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);
	//Sleep(30);
	//ResetEvent(g_hGrabEnd);
	//if(bIsLiveImage)
	//{
	//	

	//	//Mat matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, pRGBBuffer);
	//	//cv::resize(matLive(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);
	//	//
	//	//GlobalFree(pRGBBuffer);
	//}	
	//else
	//{		
	//	cv::resize( m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX ,lWindowSizeY), 0, 0, CV_INTER_AREA);
	//}
	//	
	//if(matResize.channels() == 1 && matResize.depth() == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

	//memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
//}
//MIL_ID CTestCam::GetMilGrabBuffer()
//{
	//CameraExpose();
	//WaitGrabEnd();
	//ResetEvent(g_hExpEnd[0]);

	//if (m_lDigBand == 1)
	//{
	//	return m_milGrabBuffer;
	//}
	//else if (m_lDigBand == 3)
	//{
	//	// YSKIM
	//	MbufBayer(m_milGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
	//	return m_ColorImage;
	//}

	//return m_milGrabBuffer;
//}
MIL_ID CTestCam::GetLiveGrabImage()
{
	//MbufCopy(m_LiveImage, m_milLiveGrabBuffer);

	//2019.04.11
	if (m_lDigBand == 1)
	{
		return m_LiveImage;
	}
	else if (m_lDigBand == 3)
	{
		//MbufBayer(m_milLiveGrabBuffer, m_ColorImage, m_MilWBCoefficients, M_BAYER_GR);		// YUV to GRBG
		//return m_ColorImage;
	}

	return m_milLiveGrabBuffer;
}
void CTestCam::GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)
{
	MIL_ID milGrab, milCrop;
	Mat matResize, matLive;

	Rect rect(lOffsetX, lOffsetY, lCropSizeX, lCropSizeY);
	//Sleep(10);
	if (bIsLiveImage) // LIVE 영상 호출인지 확인
	{
		if (m_bTriggerLive || m_bFreeRunLive)
		{
			milGrab = GetLiveGrabImage();
		}
		else
		{
			//milGrab = GetMilGrabBuffer();
		}

		if (m_lDigBand == 1 && m_lDigBit == 12)
		{
			MbufTransfer(milGrab, m_milCropImage,
				lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, M_ALL_BANDS,
				0, 0, lWindowSizeX, lWindowSizeY, M_ALL_BANDS,
				M_COPY + M_SCALE, M_DEFAULT, M_NULL, M_NULL);

			byte* pByteInput = new byte[lWindowSizeX * lWindowSizeY * CV_16U];
			MbufGet2d(m_milCropImage, 0, 0, lWindowSizeX, lWindowSizeY, pByteInput);
			matResize = cv::Mat(lWindowSizeX, lWindowSizeY, CV_16UC1, pByteInput);
			matResize.convertTo(matResize, CV_8UC1, 1. / 16.);
			memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
			SAFE_DELETE_ARR(pByteInput);

		}
		else if (m_lDigBand == 3 && m_lDigBit == 8)
		{
			// YSKIM
			int nBufferSize = GetImageWidth() * GetImageHeight() * GetImageBandwidth();
			byte* Image = new byte[nBufferSize];
			MbufGetColor(milGrab, M_PACKED + M_BGR24, M_ALL_BANDS, Image);
			matLive = cv::Mat(GetImageHeight(), GetImageWidth(), CV_8UC3, Image);
			cv::resize(matLive(rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);
			memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
			SAFE_DELETE_ARR(Image);
		}
		else
		{
			MbufTransfer(milGrab, m_milCropImage,
				lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, M_ALL_BANDS,
				0, 0, lWindowSizeX, lWindowSizeY, M_ALL_BANDS,
				M_COPY + M_SCALE, M_DEFAULT, M_NULL, M_NULL);

			MbufGet2d(m_milCropImage, 0, 0, lWindowSizeX, lWindowSizeY, byteImgArr);
		}
	}
	// 2017.01.09 add by ikm GUI Image Load 용 - S
	else // GUI 이미지 호출
	{
		//Rect rect(lOffsetX,lOffsetY,lCropSizeX,lCropSizeY);	
		cv::resize(m_srcImage[nImgCnt](rect), matResize, cv::Size(lWindowSizeX, lWindowSizeY), 0, 0, CV_INTER_AREA);

		int nChCnt = matResize.channels();  int imgDepth = matResize.depth();
		if (nChCnt == 1 && imgDepth == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1. / 16.);
		memcpy(byteImgArr, matResize.data, matResize.total() * matResize.elemSize());
	}
	// 	int nChCnt = matResize.channels();  int imgDepth = matResize.depth();
	// 	if(nChCnt == 1 && imgDepth == CV_16U) matResize.convertTo(matResize, CV_8UC1, 1./16.);		

}

double CTestCam::GetMeanValue(cv::Mat &matBuf, CRect rect)
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

void CTestCam::GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
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

void CTestCam::DisplayGrabImage(HWND hDispWnd)
{
	GetGrabBuffer();
}

void CTestCam::TestFunc()
{

}

void CTestCam::m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)
{
	char strTempPath[1000] = {0,};
	WideCharToMultiByte(CP_ACP, 0, strImgPath, 1000, strTempPath, 1000, NULL, NULL);

	m_srcImage[nImgCnt] = cv::imread(strTempPath, IMREAD_UNCHANGED); 
	if(dAngle != 0.0F)DoRotateImage( m_srcImage[nImgCnt],  m_srcImage[nImgCnt], dAngle);
	nChcnt = m_srcImage[nImgCnt].channels();
	nWidth = m_srcImage[nImgCnt].size().width;
	nHeight = m_srcImage[nImgCnt].size().height;
}

void CTestCam::CalcWhiteBalance(float* WhiteBalance)
{

}

void CTestCam::SetWhiteBalance(float* WhiteBalance)
{	
	
}

void CTestCam::GetWhiteBalance(float* WhiteBalance)
{
	
}

UINT CTestCam::retryConnect()
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

BOOL CTestCam::OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
{
	BOOL bRet = FALSE;
	m_eCamModel = eModel;
	//bRet = m_SerialCamera.ConnectCameraSerial(nComPort, nBaudrate, eModel);

	return bRet;
}

void CTestCam::m_fnMbufExport(CString strFilePath, MIL_ID milBuffer)
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
