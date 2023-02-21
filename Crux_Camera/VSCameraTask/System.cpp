// System.cpp : implementation file
//

#include "stdafx.h"
#include "include.h"
#include "System.h"
#include "extern.h"

#include "DB.h"
//#include "..\main\MainFrm.h"
#include "..\CruxGUI\MainFrm.h"
#include "..\CruxGUI\CruxData.h"
//GxOCX
#include "..\CruxGUI\Lib\GxStaticEx.h"

#include "OGModelInfo.h"
#include "RamdiskFileCopyManager.h"

#include "define.h"

// 2015.09.24 cpp ���� include �� ������ ���� �߻� - System �� ���� �������� ���� - by CWH
#include "InspectAlgorithmInterface.h"
#include "VsMessageProcessor.h"																// For PLC Task
#include "..\VisualStation\CommonHeader\Class\Interserverinterface.h"		// For PLC Task


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ��żӵ� Table
int _nBaudRates[] = {	CBR_1200,	CBR_2400,	CBR_4800,
						CBR_9600,	CBR_14400,	CBR_19200,
						CBR_38400,	CBR_56000,	CBR_57600, 
						CBR_115200,  };

//!!!!
CSystem* pSys;				// �ܺο��� �����ϴ� CSystem ��������, ����ϴ� ���� �ʹ� �����Ƿ� ���߿� �����ϵ��� �Ѵ�.


// 2014-04-18, jhLee, �ʱ�ȭ�� ���� ���ο� Thrad �Լ�
static UINT ThreadInitFunc(LPVOID lParam);			
static UINT ThreadSetNetworkDrive(LPVOID pVoid); //��Ʈ��ũ ����̺� üũ��
ULARGE_INTEGER  lFreeBytesAvailable;
ULARGE_INTEGER  lTotalBytes;
ULARGE_INTEGER  lTotalFree;
const int ByteVariable = 1024;


/////////////////////////////////////////////////////////////////////////////
// CSystem

CSystem* CSystem::m_Instance	= NULL;					// Instance �ʱ�ȭ
//  Singleton Instance ��ȸ
CSystem* CSystem::GetInstance() 
{								
	if (m_Instance == NULL)				// 2014.12.15 ljk delete
	{
		m_Instance = new CSystem();
	}

   return m_Instance;
}

void CSystem::ReleaseInstance()
{
	if(m_Instance != NULL)
	{
		//delete m_Instance;
		m_Instance = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSystem::CSystem()
{	
//  AfxOleInit();
	m_nDirection = 0;
	m_pVsInterServerIF	= NULL;		// For PLC Task
	m_pVsMessage		= NULL;		// For PLC Task

	pSys = this;
	int i=0;

	m_bVisionState = false;
	OgNotUse = false;
	
	m_bNowInitializing = SYS_INIT_NOT_STARTED;

	m_iRejectDefectOption = 0;
	
	CheckInsp0.SetCameraID(0);
	CheckInsp1.SetCameraID(1);

	for(int i = 0; i < NUM_INSP_STAGE; i++)	// stage ID �ο�
		Stage[i].m_nStageID = i;

	PLCCellID = "";


	m_Parameter = new Parameter *[12];
	for (i=0 ; i<12 ; i++)
	{
		m_Parameter[i] = new Parameter[12];
		memset(m_Parameter[i], 0, sizeof(Parameter)*12 );
	}


	// 2014-04-18, jhLee
	m_pInitThread			= NULL;
	m_nInitSeq				= eINIT_StepNone;		// �ʱ�ȭ�� �������� ���� 
	m_nInitSeqSub			= 0;						// ���� �ʱ�ȭ ����

	m_bInitTerminateFlag	= false;					// �ʱ�ȭ Thread�� ���߿� �����ؾ��ϴ��� ���� 
	m_bInitPauseFlag		= false;					// �ʱ�ȭ Thread�� ���߿� �Ͻø����� �ؾ� �ϴ��� ����

	m_bSysInitComplete = FALSE;

	m_pInfoROI			= NULL;
	m_pInfoSEG			= NULL;
	m_pInfoPolMark		= NULL;
	m_ptPolMarkInfo		= NULL;
	m_pInfoMesMark		= NULL;
	m_ptMesMarkInfo		= NULL;

	// 2015.07.30 by CWH
	m_pInfoFilterROI	= NULL;	

	m_pInfoMoniImgROI	= NULL;	

	// CIH 15.06.06
	m_pInfoRefROI		= NULL;
	for (int iCamIndex = 0 ; iCamIndex < 8 ; iCamIndex++)
	{
		m_milRefBuffer[iCamIndex]		= NULL;
	}
	

	//2015.05.10 add by mby
	m_tdefectDisplyParam = new tdefectDisplyParam[2];

	m_pInfoUserParam=NULL;	// 2015.06.26 by CWH

	m_tdefectDisplyParam[0].initParam();
	m_tdefectDisplyParam[1].initParam();


	strArrayDrive = new CString[10];	

	m_defIndex = 1;
	m_nALLInspUse = 0;	
	m_nAutoRecipeFlag = 0;
	//2015.05.10 add by mby

	m_nMainUINo = 0;	//2021.02.08 add by mby
	m_nTeachUINo = 0;	//2021.02.08 add by mby
	m_bInterLockProceeFlag = FALSE;	//2015.10.19 add by mby

	m_nForwardScanPadPosition = 0;
	
	m_nRoiModePosition = 0;	//2015.05.20 add by mby

	for (i=0; i<4; i++)
		m_bFlgVacMessage[i] = FALSE;

	m_flgInterfaceAuto = FALSE;

	m_pPanel = NULL;	// 2014.
	m_pPanel = new CPanel();
	SetPanelCom(m_pPanel);	

	m_pLightOffset = new CLightOffset();
	SetLightOffsetCom(m_pLightOffset);

	// KSW 15.01.07
	m_nImagTestImageNum = 0;	
	m_hThreadFormatImg = NULL;	//2016.06.30 add by mby

	m_bUseDefectFilter = 0;		// 2015.08.05 by CWH

	// 16.03.16 KSW
	// ���� ��ġ �Ѿ�� ��ġ ����.
	m_MeasureCutFlag = false;

	// 16.04.04 KSW
	// ���� ����
	m_DeleteWaterDirtFlag = false;

	::InitializeCriticalSectionAndSpinCount(&m_csComLog, 2000);
	::InitializeCriticalSectionAndSpinCount(&m_csProgramLog, 2000);
	::InitializeCriticalSectionAndSpinCount(&m_csTactLog, 2000);
	::InitializeCriticalSectionAndSpinCount(&m_csMemoryLog, 2000);
	::InitializeCriticalSectionAndSpinCount(&m_csFormatImg, 2000);//2016.06.30 add by mby
	::InitializeCriticalSectionAndSpinCount(&m_csGuiBlocking, 2000);

	for (i = 0; i <= NUM_INSP_CAMERA; i++)
	{
		for (int j = 0; j <= MAX_INSP_THREAD; j++)
			::InitializeCriticalSectionAndSpinCount(&m_csTraceLog[i][j], 2000);
	}

	// 15.10.06 KSW
	// �ʱ�ȭ
	memset(m_bAlignCameraIndex, 1, sizeof(bool) * 8);
	memset(m_ptAlignMarkPos, 0, sizeof(CPoint) * 16);

	m_nManualLightValue = 0;
	m_bGrabLight        = FALSE;
	m_bManualLight      = FALSE;

	mCInspStop			= FALSE;
	m_nContineCnt		= 0;
	m_TTtime			= 0;
}

CSystem::~CSystem()
{
	if(m_Parameter)
	{
		for (int i=0 ; i<12 ; i++)
		{
			delete [] m_Parameter[i];
			m_Parameter[i] = NULL;
		}
		delete [] m_Parameter;
		m_Parameter = NULL;
	}

	FrontLight.m_SPort.Close();
	BackLight.m_SPort.Close();

	// Pattern Generator : �� stage���� ����
	for(int i=0; i<NUM_INSP_STAGE; i++)	// COM 2, 3
	{
		Stage[i].PatrnGen.ClosePort();
	}

	if (m_pPanel != NULL)
		delete m_pPanel;
	if (m_pLightOffset != NULL)
		delete m_pLightOffset;

	if (m_MilHostSystem) MsysFree(m_MilHostSystem);		
	m_MilHostSystem = M_NULL;

	if (m_MilApp) {
		MappFree(m_MilApp);
		m_MilApp = M_NULL;	
	}

	if(m_pInfoROI != NULL)	delete[] m_pInfoROI;
	if(m_pInfoSEG != NULL)	delete[] m_pInfoSEG;
	if(m_pInfoPolMark != NULL)	delete[] m_pInfoPolMark;
	if(m_ptPolMarkInfo != NULL)	delete[] m_ptPolMarkInfo;
	if(m_pInfoMesMark != NULL)	delete[] m_pInfoMesMark;
	if(m_ptMesMarkInfo != NULL)	delete[] m_ptMesMarkInfo;

	// 2015.07.30 by CWH
	if(m_pInfoFilterROI->pRectInfo != NULL)	delete[] m_pInfoFilterROI->pRectInfo;
	if(m_pInfoFilterROI != NULL)	delete[] m_pInfoFilterROI;
	
	// 2016.04.06 add by mby
	if(m_pInfoMoniImgROI->pRectInfo != NULL)	delete[] m_pInfoMoniImgROI->pRectInfo;
	if(m_pInfoMoniImgROI != NULL)	delete[] m_pInfoMoniImgROI;

	if(m_tdefectDisplyParam != NULL)	delete[] m_tdefectDisplyParam;

	if(strArrayDrive != NULL)	delete[] strArrayDrive;		//2015.06.07 add by mby 

	// 2015.09.01 ����̺꺰 Limit �뷮 �߰�
	//if(m_tDriveInfoParam != NULL)	delete[] m_tDriveInfoParam;	//2015.06.30 add by mby 
	if(m_pInfoUserParam != NULL)	delete[] m_pInfoUserParam;	// 2015.06.26 by CWH

	DeleteCriticalSection(&m_csComLog);
	//DeleteCriticalSection(&m_csTraceLog);
	DeleteCriticalSection(&m_csProgramLog);
	DeleteCriticalSection(&m_csTactLog);
	DeleteCriticalSection(&m_csMemoryLog);
	DeleteCriticalSection(&m_csFormatImg);	//2016.06.30 add by mby	
	DeleteCriticalSection(&m_csGuiBlocking);

	for (int i = 0; i <= NUM_INSP_CAMERA; i++)
	{
		for (int j = 0; j <= MAX_INSP_THREAD; j++)
			::DeleteCriticalSection(&m_csTraceLog[i][j]);
	}

	// CIH 2015.06.06
	if(m_pInfoRefROI != NULL)	delete[] m_pInfoRefROI;

	for (int iCamIndex = 0 ; iCamIndex < 8; iCamIndex++)
	{
		MbufFree(m_milRefBuffer[iCamIndex]);
		m_milRefBuffer[iCamIndex] = M_NULL;
	}
	
	
	SAFE_DELETE(m_pVsInterServerIF);		// For PLC Task
	SAFE_DELETE(m_pVsMessage)				// For PLC Task
  	TRACE("CSystem Bye~\n");
}

BOOL CSystem::InitInstance()
{
	
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CSystem::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
//	return CWinThread::ExitInstance();
	return 0;
}

//BEGIN_MESSAGE_MAP(CSystem, CWinThread)
BEGIN_MESSAGE_MAP(CSystem, CWnd)
	//{{AFX_MSG_MAP(CSystem)
		ON_WM_DESTROY()
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSystem message handlers


//
// �ʱ�ȭ ����, (ó������ ����)
void CSystem::StartInitialize()
{
	//AfxOleInit();
	
//?	if ( m_bNowInitializing != SYS_INIT_NOT_STARTED ) 	return;					// �ʱ�ȭ�� ���۵��� �ʾҴٴ� ������ �ƴ϶�� ���� ����

	m_bNowInitializing = SYS_INIT_ING;													// �ʱ�ȭ ���������� ǥ��

	//old	AfxBeginThread(StartHW,	THREAD_PRIORITY_NORMAL, 0, 0, NULL);			// �ʱ�ȭ Thread ����, ����


	m_nInitSeq					= eINIT_StepNone;		// �ʱ�ȭ�� �������� ���� 
	m_bInitTerminateFlag		= false;					// �ʱ�ȭ Thread�� ���߿� �����ؾ��ϴ��� ���� 
	m_bInitPauseFlag			= false;					// �ʱ�ȭ Thread�� ���߿� �Ͻø����� �ؾ� �ϴ��� ����
	SetSystemInitComplete(FALSE);

	m_pInitThread = AfxBeginThread( ThreadInitFunc, this );		// �ʱ�ȭ Thread ����, ����

	m_pVsMessage = new VsMessageProcessor(this);		// For PLC Task
	m_pVsMessage->m_fnStartVsMsgReceiver();				// For PLC Task

	// 2014-04-18, jhLee, ȣ�������� Block�� �ɸ��Ƿ� �Ʒ� Message pumping ������ �����Ѵ�.
	//MSG msg;
	//while(m_bNowInitializing == SYS_INIT_ING)
	//{
	//	if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}

	//	Sleep(100);
 //	}
		//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:StartInitialize() ,%s,%d"), pSys->GetPanelID(),EVT_SYS_START_INIT);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
}

void    CSystem::SetNetworkDrive()
{
	::AfxBeginThread(ThreadSetNetworkDrive, this);
}

// �ʱ�ȭ�� �Ͻ������� ������Ų��.
void	CSystem::PauseInitialize()
{
	m_bInitPauseFlag			= true;					// �ʱ�ȭ Thread�� ���߿� �Ͻø����� �ؾ� �ϴ��� ����
}

// �ʱ�ȭ �Ͻ������� Ǯ�� �簳�Ѵ�.
void	CSystem::ResumeInitialize()
{
	m_bInitPauseFlag			= false;					// �ʱ�ȭ Thread�� ���߿� �Ͻø����� �ؾ� �ϴ��� ����
}

// �ʱ�ȭ�� ���� ���� ��Ų��.
void	CSystem::AbortInitialize()
{
	m_bInitTerminateFlag		= true;					// �ʱ�ȭ Thread�� ���߿� �����ؾ��ϴ��� ���� 
}





bool CSystem::InitializeVision()
{

//	AfxOleInit();
// Initialize Host MIL (for OCR and Display)
 	MappAlloc(M_DEFAULT,&m_MilApp);
 	MsysAlloc(M_SYSTEM_HOST, M_DEFAULT,  M_DEFAULT, &m_MilHostSystem);
 	TRACE("InitializeVision() : m_MilHostSystem = %d", m_MilHostSystem);
	
//  	MappHookFunction(M_ERROR_CURRENT, HookHandler, this);
//  	MappControl(M_ERROR, M_PRINT_DISABLE);
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:InitializeVision() ,%s,%d"), pSys->GetPanelID(),EVT_SYS_INIT_VISION);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);

	for(int i=0; i<NUM_FRAME_GRABBER; i++)
	{
 		if(m_Config.IsFrameGrabber[i])
			Stage[i].InitVision(i);
	}

	return true;
}

void CSystem::ExitVision()
{
	for(int i=0; i<NUM_FRAME_GRABBER; i++) //NUM_INSP_STAGE
	{
		Stage[i].ExitVision();
 	}

	// Free the MIL 
	//if (m_MilHostSystem) MsysFree(m_MilHostSystem);		
 //	m_MilHostSystem = M_NULL;

	//if (m_MilApp) {
	//	MappFree(m_MilApp);
	//	m_MilApp = M_NULL;	
	//}

}

//////////////////////////////////////////////
// Trace

BOOL CSystem::SendCopyMsg(ULONG nMsgId, CString strMsg)
{
		COPYDATASTRUCT* CpStructData = new COPYDATASTRUCT;			// �޽��� ���ú� �޴� ������ �����Ǿ��Ѵ�.
		CpStructData->dwData = nMsgId;
		CpStructData->cbData = (DWORD)strlen(strMsg);				//���ڿ��� ����
		CpStructData->lpData = new TCHAR[CpStructData->cbData+2];
		memset(CpStructData->lpData, 0x00, CpStructData->cbData+2);
		memcpy(CpStructData->lpData, (LPSTR)(LPCTSTR)strMsg, CpStructData->cbData);

		//CpStructData->lpData = (LPSTR)(LPCTSTR)strMsg;		//���ڿ� ������
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE1))->SetBackColor(RGB(224, 224, 224));
		CMainFrame* pMainWnd = (CMainFrame*)pMasterWnd;

		BOOL bRet = FALSE;
		if (pMainWnd != NULL)
		{
			//::SendMessage(pMainWnd->m_hWnd, WM_COPYDATA, 0, (LPARAM)CpStructData);
			::PostMessage(pMainWnd->m_hWnd, WM_SHOW_LOG_DATA, 0, (LPARAM)CpStructData);
			bRet = TRUE;
		}
		return bRet;
}

void CSystem::TraceLogFile(CString *pmsg, int nCameraID, int nThreadID)
{

	// �Ķ���� ���� 
	if (nThreadID >= MAX_INSP_THREAD || nCameraID >= NUM_INSP_CAMERA )
	{
		//nThreadID ��  MAX_INSP_THREAD �� ������ �⺻ ���Ͽ� �����Ѵ�.
		nCameraID = NUM_INSP_CAMERA;
		nThreadID = MAX_INSP_THREAD;
	}

	// Critical Section - Thread Index �� ���� ���� ����
	EnterCriticalSection(&m_csTraceLog[nCameraID][nThreadID]);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);

	//jskim
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);

	// 2015.08.24 Thread ID �߰�
	CString strThreadID;
	strThreadID.Format(_T("%d"), nThreadID);

	//2015.08.26 Camera ID �߰�
	CString strCameraID;
	strCameraID.Format(_T("%d"), nCameraID);

	CString fName = m_Config.m_strLogFilePath;
	fName += CString("\\") + strToday;
	//CString fName = "D:\\CRUX\\LOG";
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "Process";
	CreateDirectory(fName, NULL);

	if (nThreadID < MAX_INSP_THREAD)
		fName += CString(_T("\\")) + strT + _T("_CAM") + strCameraID + _T("_T") + strThreadID + _T(".log");
	else
		fName += CString(_T("\\")) + strT + _T(".log");

	FILE* fp = NULL;

	// KSW 15.01.01
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}
	while( !fp );

	if (!fp)
	{
		LeaveCriticalSection(&m_csTraceLog[nCameraID][nThreadID]);
		::AfxMessageBox("System Trace Logging Failed - file open error 1");
		return;
	}

	CString logmsg = strTime + _T("> ") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;
	
	CString strGridLogMsg = "";
	strGridLogMsg.Format("%s\t%s",strTime, *pmsg);

	LeaveCriticalSection(&m_csTraceLog[nCameraID][nThreadID]);

	EnterCriticalSection(&m_csGuiBlocking);
	SendCopyMsg(SYS_MSG_LOG_SYSTEM, strGridLogMsg);
	LeaveCriticalSection(&m_csGuiBlocking);
}

void CSystem::MemoryLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level)
	{
		return;
	}
	EnterCriticalSection(&m_csMemoryLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("[Memory]%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);

	//jskim
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	
	CString fName = m_Config.m_strLogFilePath;
	CreateDirectory(fName, NULL);
	fName += CString("\\") + strToday;
	//CString fName = "D:\\CRUX\\LOG";
	CreateDirectory(fName, NULL);

	fName += CString("\\") + strT + ".log";

	FILE* fp = NULL;

	// KSW 15.01.01
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}while( !fp );

	//_tfopen_s(&fp, LPCTSTR(fName), "at");

	if (!fp) {
		LeaveCriticalSection(&m_csMemoryLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 2");
		return;
	}

	CString logmsg = strTime + _T("> ") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csMemoryLog);
}

void CSystem::CommLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}
	EnterCriticalSection(&m_csComLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("[ComLog]%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);
	
	//jskim
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	
	CString fName = m_Config.m_strLogFilePath;
	fName += CString("\\") + strToday;
	//CString fName = "D:\\CRUX\\LOG";
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "Common";
	CreateDirectory(fName, NULL);
		
	fName += CString("\\") + strT + ".log";

	FILE* fp = NULL;

	// KSW 15.01.01
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}
	while( !fp );
	//_tfopen_s(&fp, LPCTSTR(fName), "at");
	if (!fp) 
	{
		LeaveCriticalSection(&m_csComLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 3");
		return;
	}
	
	CString logmsg = strTime + _T("> ") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csComLog);
}

//2015.05.10 add by mby S-
void CSystem::InspectInfoLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}
	EnterCriticalSection(&m_csMemoryLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;
	int flag = 0;
	CFileFind	finder;
	COGModelInfo* m_OGModelInfo = NULL;

	CString strHeader;
	strHeader = _T("TIME,PANELID,TOTAL,NG,DUST,WHITE TURBITY,ITO,EDGE BROKEN,PAD SCRATCH,DIMPLE POINT,ETCHING LINE,DIMPLE LINE,PAD BURNT,BURR,POL BUBBLE,POL SCRATCH,POL ALIEN SUB,POL MINOR,BLACK SCRATCH,WHITE SCRATCH,CF MEASURE,TFT MEASURE,MURA");

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("[ComLog]%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);
	
	CString strToday;
	if(m_Config.m_nLogNew > time.wHour){
		strToday.Format("%04d%02d%02d_01", time.wYear, time.wMonth, time.wDay);
	}else{
		strToday.Format("%04d%02d%02d_02", time.wYear, time.wMonth, time.wDay);
	}	
	
	CString fName = m_Config.m_strLogFilePath;
	fName += CString("\\") + strToday;
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "InspectInfo";
	CreateDirectory(fName, NULL);	

	fName += CString("\\") + strToday + ".csv";	
	if ( m_OGModelInfo->IsFileExist(fName) )
	{
		flag = 0;
	}
	else
	{
		flag = 1;
	}

	FILE* fp = NULL;


	//���� ������ ���� �ִ� ��� ���� ��� �߻�.. ���� ���� �����̸� ���� �ʰ� ����.
	//do
	//{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	//}while( !fp );

	if (!fp) {
		LeaveCriticalSection(&m_csMemoryLog);
		pSys->TraceLogFile(&CString("InspectInfoLogFile Write fail"));
		//::AfxMessageBox("System Trace Logging Failed - file open error 4");
		return;
	}
	
	CString logmsg = strTime + _T(",") + *pmsg;
	if(flag == 1){
		fprintf(fp, "%s\n", strHeader);
	}
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csMemoryLog);
}

void CSystem::AlarmLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}
	//Message Date,PanelID,ErrorID,ErrorLevel,ErrorInfo
	EnterCriticalSection(&m_csComLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);	
	
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	
	CString fName = m_Config.m_strLogFilePath;
	fName += CString("\\") + strToday;
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "AlarmLog";
	CreateDirectory(fName, NULL);
		
	fName += CString("\\") + "AlarmList.log";

	FILE* fp = NULL;
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}while( !fp );

	if (!fp) {
		LeaveCriticalSection(&m_csComLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 3");
		return;
	}
	
	CString logmsg = strTime + _T(",") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csComLog);
}
void CSystem::EventLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}
	//Message Date,PanelID,ErrorID,ErrorLevel,ErrorInfo
	EnterCriticalSection(&m_csTactLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);	
	
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	
	CString fName = m_Config.m_strLogFilePath;
	fName += CString("\\") + strToday;
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "EventLog";
	CreateDirectory(fName, NULL);
		
	fName += CString("\\") + "EventList.log";

	FILE* fp = NULL;
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}while( !fp );

	if (!fp) {
		LeaveCriticalSection(&m_csTactLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 3");
		return;
	}
	
	CString logmsg = strTime + _T(",") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csTactLog);
}

//2015.05.10 add by chlee
//��ǻ���� �޷θ� ��뷮�� üũ�Ͽ� ����� �����Ѵ�. ũ��� byte
CString CSystem::GetMemoryInformation(){
	MEMORYSTATUS MemStat;
	MemStat.dwMemoryLoad=0;
	MemStat.dwTotalPhys=0;
	MemStat.dwAvailPhys=0;
	MemStat.dwTotalPageFile=0;
	MemStat.dwAvailPageFile=0;
	MemStat.dwTotalVirtual=0;
	MemStat.dwAvailVirtual=0;
	MemStat.dwLength=sizeof(MemStat);

	GlobalMemoryStatus(&MemStat);
	DWORD64 m_dwAvailPhys = (MemStat.dwAvailPhys /1024)/1024;
	DWORD64 m_dwTotalPhys = (MemStat.dwTotalPhys /1024)/1024;
	DWORD64 m_dwAvailPagePhys = (MemStat.dwAvailPageFile /1024)/1024;
	DWORD64 m_dwTotalPagePhys = (MemStat.dwTotalPageFile /1024)/1024;
	DWORD64 m_dwAvailVirPhys = (MemStat.dwTotalVirtual /1024)/1024;
	DWORD64 m_dwTotalVirPhys = (MemStat.dwAvailVirtual /1024)/1024;
	
	
	CString strSize;
	CString strSizeTotal;
	strSizeTotal.Append(_T("Available Memory Size : "));
	_ui64toa(m_dwAvailPhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("      Total Memory Size : "));
	_ui64toa(m_dwTotalPhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("MB\n"));
	strSizeTotal.Append(_T("      Available Page Size : "));
	_ui64toa(m_dwAvailPagePhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("MB\n"));
	strSizeTotal.Append(_T("      Total Page Size : "));
	_ui64toa(m_dwTotalPagePhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("MB\n"));
	strSizeTotal.Append(_T("      Available Vir Size : "));
	_ui64toa(m_dwAvailVirPhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("MB\n"));
	strSizeTotal.Append(_T("      Total Vir Size : "));
	_ui64toa(m_dwTotalVirPhys,strSize.GetBufferSetLength(1024),10);
	strSizeTotal.Append(strSize);
	strSizeTotal.Append(_T("MB\n"));

	return strSizeTotal;
}


//2015.05.10 add by mby E-
// �� ���濡 ���� ���� �Ǵ� ������ ���� �Ѵ�. 
// inspection �� �ε��ϱ� -> �ݵ�� ī��Ʈ ���� ���� ���� �� �ε带 ���� �Ѵ�.  
//BOOL CSystem::ChangeModelData(CString ModelName)
BOOL CSystem::ChangeModelData()
{
	CString	ReadData;
	
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:ChangeModelData Model(%s) ,%s,%d"), m_Config.m_strModelPath, pSys->GetPanelID(),EVT_SYS_CHANGE_MODEL);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);

	// load vision models : active
	ModelInfo.LoadModelInfo((LPCTSTR)(m_Config.m_strModelPath));

	////////////////////////////////////////
	// UF Param ����.
//  	((CUFInspThrd*)(((CActiveInspThrd*)Stage[0].Inspect.m_InspThrd)->m_pUFThrd))->SuspendThread();
//  	((CUFInspThrd*)(((CActiveInspThrd*)Stage[0].Inspect.m_InspThrd)->m_pUFThrd))->LoadUFParam();
//  	((CUFInspThrd*)(((CActiveInspThrd*)Stage[0].Inspect.m_InspThrd)->m_pUFThrd))->ResumeThread();	

	// load vision models : CRUX LINE
	// [����!] �ܰ� �˻� �� : ����(pattern) �����̹Ƿ�, �ݵ�� vision �ʱ�ȭ �� �ε��� ��
	// ����� �� 3���� : odyssey ���� ���� ��(3��, 1��), odyssey ���� ���� MIL8.0, MIL6.0
	/*/
#ifdef M_SYSTEM_ODYSSEY
	if(m_Config.IsFrameGrabber[2])		// ���� ���� �� (3��° ����)
		OGModelInfo.Load(m_Config.m_strOGParamPath, Stage[2].m_GdSystem.m_InspDig.GetOdysseyThreadPointer());	// ONL
	else if(m_Config.IsFrameGrabber[0])	// [TEST] test pc�� ���尡 1�� (1�� ���� ��� ���)
		OGModelInfo.Load(m_Config.m_strOGParamPath, Stage[0].m_GdSystem.m_InspDig.GetOdysseyThreadPointer());	// ONL
	else	// ���� ����
		OGModelInfo.LoadText(m_Config.m_strOGParamPath);
#else
		/*/
	OGModelInfo.Load(m_Config.m_strOGParamPath);
//#endif

	return TRUE;
}
//2015.10.29 Recipe list ���� ������ �۽� add by mby S-
void CSystem::SendRecipeListInfo()
{
	CString strParam;
	CString strFirstParam;
	strParam.Empty();
	strFirstParam.Empty();
	strFirstParam += "0.";

	// ���� ����
	CFileFind finder;

	BOOL bWork = finder.FindFile( m_Config.m_strModelFilePath + CString("\\*.*") );

	while(bWork)
	{
		bWork = finder.FindNextFile();

		if(finder.IsDots())	continue;
		else if(finder.IsDirectory())
		{
			CString strName, str = finder.GetFilePath();
			strName = str.Right( str.GetLength() -  str.ReverseFind( '\\' ) - 1 );
			strParam += strName + _T(",");			
		}
	}
	strParam.TrimRight(",");
	strParam.Replace(".","#");
	strFirstParam += strParam;
	//Recipe list ������ �۽�
	//2016.02.24 add by ikm - ������ ���� -> IPC ������� ����- S
	m_pVsMessage->SendVsMsg_ToServer_RecipeSet(strFirstParam);
	CommLogFile(&CString("SendVsMsg_ToServer_RecipeSet Send"), LOG_LEVEL_BASIC);
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:SendRecipeListInfo() ,%s,%d"), pSys->GetPanelID(),EVT_SYS_SEND_RCP_LIST);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	//Smith.HandlingSendMsg(VTM_INTERNAL_RECIPE_SEND,(LPSTR)(LPCTSTR)strFirstParam);
	//2016.02.24 add by ikm - ������ ���� -> IPC ������� ���� - E
}

BOOL CSystem::ServerRecipeSelect(CString strRecipeName)
{
	// Load Model File
	BOOL bRetVal = FALSE;
	CMainFrame* pMainWnd = (CMainFrame*)pMasterWnd;
	CSystem *pSystem = CSystem::GetInstance();
	CString str;
	str.Format(_T("[APPLICATION]Auto Recipe Change (%s -> %s) UI Pos Main UI :%d,SubUI: %d "), m_strServerModelName, strRecipeName,pSystem->m_nMainUINo,pSystem->m_nTeachUINo);
	pSystem->StartLogFile(&str, LOG_LEVEL_BASIC);
	CString strFilePath = m_Config.m_strModelFilePath + _T("\\") + strRecipeName + _T("\\") + strRecipeName + _T(".mod");

	// model path ���� �� ��Ÿ param path(UF, OG)�� ���ÿ� ������
	pSystem->m_Config.UpdateModelPath(strFilePath);

	// Load
	pSystem->ChangeModelData();

	// Save Last Model *.ini
	pSystem->m_Config.Write("LastRecipe", "Latest_Model_Path", strFilePath);

	// Top Model Name Change
	pSystem->ModelInfo.SetModelName(strRecipeName);

	//2016.02.24 add by ikm - ������ ���� -> IPC ������� ���� - S
	//::SendMessage(((CMainFrame*)AfxGetMainWnd)->m_hWnd, UM_MODEL_CHANGE, 0, 0);
	::SendMessage(((CMainFrame*)AfxGetApp()->GetMainWnd())->m_hWnd, UM_MODEL_CHANGE, 0, 0);
	//2019.05.07 by ryu
	::SendMessage(((CMainFrame*)AfxGetApp()->GetMainWnd())->m_hWnd, UM_GRAB_TOPBAR, 0, 0);//UM_GRAB_TOPBAR
	//2016.02.24 add by ikm - ������ ���� -> IPC ������� ���� - E

	// Grab Env Check,Creat ������ inch����� GrabCount������� //Recipe���� �Ҽ��ְ� �ؾ���Todo
	if (!pSystem->m_Config.bSaparaMode)
	{
		//2016.03.25 add by mby
		for(int i = 0; i< NUM_INSP_CAMERA; i++ )
		{
			if(pSystem->m_Config.IsInspCamera[i])
				pSystem->Stage[i].Inspect.m_InspThrd[0]->CheckLineEnv(true);		
		}		
		//CheckLineEnv();
	}

	pSystem->Trigger.TriggerPositionChannelSet();

	//2016.02.11 add by mby S-
	CString strRoot=_T("");
	strRoot.Format(_T("%s\\%s\\%s_%s"),pSystem->m_Config.m_strModelFilePath, strRecipeName,strRecipeName,DEF_CODE_INI );
	pMainWnd->m_stDefCodeInfo.setInfoPath(PNL_JUDGE_ITEM, strRoot);
	pMainWnd->m_stDefCodeInfo.init_info(TRUE);
	pMainWnd->m_stDefCodeInfo.init_info(FALSE);
	//2016.02.11 add by mby E-

	m_strServerModelName = strRecipeName;

	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:ServerRecipeSelect Recipe(%s) ,%s,%d"), m_strServerModelName,pSys->GetPanelID(),EVT_SYS_SERVER_RCP_SELECT);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);

	m_nAutoRecipeFlag = 1;	//2016.02.01 add by mby

	return TRUE;
}

//2015.10.29 add by mby E-

//////////////////////////////////////////////
// System Init Thread

/* �ʱ�ȭ Procesure : [TODO] ������ ��
	1. �ʱ�ȭ ���� ���� : path validity

  // [TODO] ���� �ʱ�ȭ �߰��� �� : ���� �ʱ�ȭ�� ���� �ð��� �ɸ�
		
	3. Serial Port ���� : Light, Trigger, 
	4. ��� ���� ����
	5. Thread ����
	
	2. �� ���� Ȯ�� : path validity
	6. �� ����

	7. �ʱ�ȭ �Ϸ�
*/

//
// ���� StartHW() �Լ��� ��ü�ϴ� Thread �Լ�
UINT ThreadInitFunc(LPVOID lParam)
{
	CSystem* pSystem = (CSystem *)lParam;					// �Ѱܿ� �Ķ���͸� CSystem���� ��ȯ
	int i;
	BOOL bResult;			// Thead ���� step���� ���
	bool bPrevPause = pSystem->m_bInitPauseFlag;		// ������ Pause ���� ������

	CString strInitLog;
	strInitLog.Format("---------------Initialize Start!!-----------------");
	pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);

	pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_EVENT, CSystem::eEVT_InitStart, 0);			// �ʱ�ȭ ���� Event ����

 	while ( !(pSystem->m_bInitTerminateFlag) )			// ���� Flag�� true�� �� ������ �ݺ��Ѵ�.
 	{
 		if ( pSystem->m_bInitPauseFlag )						// �Ͻ������� �����Ǿ��ִٸ� 
 		{
 			if ( !bPrevPause )		// ó������ �Ͻ� ������ �����Ͽ��ٸ�, Event�� ������.
 			{
 				bPrevPause = pSystem->m_bInitPauseFlag; 
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_EVENT, CSystem::eEVT_InitPause, 0);		// �Ͻ�����
 			}
 
 			Sleep( 250 );											// ��� �ð� �ڿ� �� �ݺ� ���
 			continue;
 		}
 		else
 		{
 			if ( bPrevPause )		// ������ �Ͻ� ���� ���¿��ٸ�,
 			{
 				bPrevPause = pSystem->m_bInitPauseFlag; 
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_EVENT, CSystem::eEVT_InitResume, 0);		// �Ͻ����� ���� 
 			}
 		}
 
 		BOOL IsSapError = FALSE;
 
 		// ���� �ܰ躰�� �ʱ�ȭ�� �����Ѵ�.
 		switch ( pSystem->m_nInitSeq )		
 		{
 			case CSystem::eINIT_StepNone		:	// 0. ���� �ʱ�ȭ �������� �ʴ´�.
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);					// ���� ���� ������ ������.
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 				pSystem->m_nInitSeq		= CSystem::eINIT_StepFile;
 				pSystem->m_nInitSeqSub	= 0;						// ���� �ʱ�ȭ ����
 				break;
 
 
 			case CSystem::eINIT_StepFile		:	// 1. �ʱ�ȭ ���� ���� : path validity
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 				// �ʱ�ȭ ������ �����ϴ��� ����
 				if ( -1 == GetFileAttributes( pSystem->m_Config.GetPath() ))	// ������ �������� �ʴ´�.
 				{
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);					// ���� �ܰ迡�� �ʱ�ȭ ���и� �˸���.
 					pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_ConfigFile, 0);		// �ʱ�ȭ Alarm �߻��� �˸���, �������� ����
 					pSystem->m_bInitPauseFlag = true;																			// �ʱ�ȭ�� �Ͻ� �����.
 					break;
 				}
 
 				//2016.09.22 - Sapera Mode �˻�
 				if(!pSystem->m_Config.bSaparaMode)
 				{
 					switch(pSystem->m_Config.m_nEqpType)
 					{
 					case EQP_TYPE_SML:
 						break;
 					case EQP_TYPE_MID:
 						if(pSystem->m_Config.m_nModelType == EQP_TYPE_ABI && pSystem->m_Config.m_nInspactEqpNo == 2)
 							IsSapError = TRUE;
 						break;
 
 					case EQP_TYPE_BIG:
 						break;
 					case EQP_TYPE_MONITOR:
 						if(pSystem->m_Config.m_nModelType == EQP_TYPE_ABI && pSystem->m_Config.m_nInspactEqpNo > 6)
 							IsSapError = TRUE;
 						break;
 					}
 				}
 				else
 				{
 					switch(pSystem->m_Config.m_nEqpType)
 					{
 					case EQP_TYPE_SML:
 					case EQP_TYPE_MID:
 						if(pSystem->m_Config.m_nModelType == EQP_TYPE_ABI && pSystem->m_Config.m_nInspactEqpNo != 2)
 							IsSapError = TRUE;
 						break;
 
 					case EQP_TYPE_BIG:
 					case EQP_TYPE_MONITOR:
 						if(pSystem->m_Config.m_nModelType == EQP_TYPE_ABI && pSystem->m_Config.m_nInspactEqpNo <= 6)
 							IsSapError = TRUE;
 						break;
 					}
 				}
 
 				if(IsSapError)
 				{
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 					pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_ConfigFile, 0);
 					pSystem->m_bInitPauseFlag = true;
 					strInitLog.Format("[Initialize] Sapera Mode Checking Error!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					AfxMessageBox(strInitLog);
 					break;
 				}
 
 				// �ʱ�ȭ ������ ����
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 				pSystem->m_nInitSeq = CSystem::eINIT_StepModel;
 
 				strInitLog.Format("[Initialize] Init File Checking Complete!!");
 				pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 				break;
 
 	
 
 			case CSystem::eINIT_StepModel		:	// 2. �� ���� Ȯ��: path validity	
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 				// Model ���� ������ �����ϴ��� ����
 				if ( -1 == GetFileAttributes( pSystem->m_Config.m_strModelPath ))	// ������ �������� �ʴ´�.
 				{
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 					pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_ModelFile, 0);		// Model ���� ���� ����
 					pSystem->m_bInitPauseFlag = true;																		
 					break;
 				}
 
 				// �ʱ�ȭ ������ ����
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 				pSystem->m_nInitSeq = CSystem::eINIT_StepStage;
 
 				strInitLog.Format("[Initialize] Model File Checking Complete!!");
 				pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 				break;
 
 
 			case CSystem::eINIT_StepStage		:	// 3. Stage �ʱ�ȭ
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 				if( pSystem->m_Config.IsHW )
 				{
 					for( i = 0; i < (NUM_INSP_STAGE ); i++)	//�������� ������ 1�� :
 					{	
 						// Log("StartHW() : Stage(%d) �ʱ�ȭ - Start", i);
 			
 						if( !(pSystem->Stage[i].Init(i)) )		// stage ������ �ʱ�ȭ (inspThrd ����)
 						{
 							// Log("StartHW() : Stage(%d) �ʱ�ȭ - Fail", i);
 
 							pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 							pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_InitStage, i);	// Stage �ʱ�ȭ ����
 							pSystem->m_bInitPauseFlag = true;																		
 							break;
 						}
 			
 						// Log("StartHW() : Stage(%d) �ʱ�ȭ - End", i);
 						// pSys->StartLogFile(&strMsg);
 						if ( pSystem->m_bInitTerminateFlag ) return 0;			// thread ���� ���� Ȯ��
 					}//of for i
 				}//of if( pSystem->m_Config.IsHW )
 
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 				pSystem->m_nInitSeq = CSystem::eINIT_StepLight;
 
 				strInitLog.Format("[Initialize] Camera && Board Init Complete!!, Grab Thread Create!!");
 				pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 				break;
 
 
 
 			case CSystem::eINIT_StepLight		:	// 4. Serial, Light �ʱ�ȭ
 
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 				if( pSystem->m_Config.IsHW )
 				{
 					// Light Connetct --> COM1
 					//if( !pSystem->FrontLight.ConnectPg( (NUM_INSP_STAGE - 1), 19200) ){	//	COM1
 					if( !pSystem->FrontLight.ConnectPg( pSystem->m_Config.m_nSettingLightPort+1, _nBaudRates[pSystem->m_Config.m_nSettingLightBaud] ) ){	//	COM1
 							// Log strMsg.Format("StartHW() : Light Port �ʱ�ȭ - Fail");
 
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 						pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_InitLight, i);	// Light �ʱ�ȭ ����
 						pSystem->m_bInitPauseFlag = true;
 						strInitLog.Format("[Initialize] Light Connection fail!!");
 						pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 						break;
 					}
 
 					pSystem->FrontLight.TurnOffAll();
 
 					// log strMsg.Format("StartHW() :  Light Port �ʱ�ȭ - Sucess");
 				}//of if( pSystem->m_Config.IsHW )
 
 				pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, (NUM_INSP_STAGE - 2));
 				pSystem->m_nInitSeq		= CSystem::eINIT_StepTrigger;
 				pSystem->m_nInitSeqSub	= 0;						// ���� �ʱ�ȭ ����
 
 				strInitLog.Format("[Initialize] Light Init Complete!!");
 				pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 				break;
 
 
 
 			case CSystem::eINIT_StepTrigger	:	// 5. Serial, Trigger �ʱ�ȭ
 				// Trigger Connetc --> Com2
 				// og stage�� pattern generator�� ����.
 
 				// �� �ʱ�ȭ Step������ �ܰ谡 ���Ƽ� ���� step�� ����Ѵ�.
 				switch ( pSystem->m_nInitSeqSub )
 				{
 					case 0 : // �ʱ� ����
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 						if( pSystem->m_Config.IsHW )			// Hardware ��� ���� ?
 							pSystem->m_nInitSeqSub = 1;		// ����  Step���� ����.
 						else
 							pSystem->m_nInitSeqSub = 99;		// ����� �̵�
 
 						break;					
 
 					case 1 : // ��� ����
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 10);				// ������ ǥ��
						if((pSys->m_Config.m_nInspactEqpNo == 1) || (pSys->m_Config.m_nInspactEqpNo == 7)){
							
 							if( !pSystem->Trigger.ConnectTrigger(pSystem->m_Config.m_nTriggerPort,  _nBaudRates[pSystem->m_Config.m_nTriggerBaud])){ // COM2
 								// Log strMsg.Format("StartHW() : Trigger Port �ʱ�ȭ - Fail");
 
 								pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, pSystem->m_nInitSeqSub);				
 								pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_InitTrigger, pSystem->m_nInitSeqSub);	// Light �ʱ�ȭ ����
 								pSystem->m_bInitPauseFlag = true;
 								strInitLog.Format("[Initialize] Trigger Connection fail!!");
 								pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 								break;
 							}
						} 
 
 						Sleep(50);
 						if((pSys->m_Config.m_nEqpType != EQP_TYPE_BIG && pSys->m_Config.m_nEqpType != EQP_TYPE_BIG_D9)
 						|| ( (pSys->m_Config.m_nEqpType == EQP_TYPE_BIG && pSys->m_Config.m_nEqpType != EQP_TYPE_BIG_D9) 
 							 && ( (pSys->m_Config.m_nInspactEqpNo == 1) || (pSys->m_Config.m_nInspactEqpNo == 7) ) ) )
 							pSystem->m_nInitSeqSub = 2;		// ����  Step���� ����.
 						else pSystem->m_nInitSeqSub = 99;
 						break;
 
 					case 2 :
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 20);				// ������ ǥ��
 						pSys->Trigger.StopTriggerGen0();
 						Sleep(50);
 						pSystem->m_nInitSeqSub = 3;		// ����  Step���� ����.
 						break;
 
 					case 3 :
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 40);				// ������ ǥ��
 						pSys->Trigger.StopTriggerGen1();
 						Sleep(50);
 						pSystem->m_nInitSeqSub = 4;		// ����  Step���� ����.
 						break;
 
 					case 4 : // Trigger Borad �ʱ�ȭ
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 60);				// ������ ǥ��
  						pSys->Trigger.TriggerInit0();
 						Sleep(50);
 						pSystem->m_nInitSeqSub = 5;		// ����  Step���� ����.
 						break;
 
 					case 5 :
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 80);				// ������ ǥ��
 						pSys->Trigger.TriggerInit1();
 						pSystem->m_nInitSeqSub = 99;		// ����  Step���� ����.
 						break;
 
 					case 99 : // ���� �ܰ� ����
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 100);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 						pSystem->m_nInitSeq		= CSystem::eINIT_StepSocket1;
 						pSystem->m_nInitSeqSub	= 0;						// ���� �ʱ�ȭ ����
 						break;
 
 				}
 				break; //of case CSystem::eERR_SYSHW_InitTrigger	:	// Serial, Trigger �ʱ�ȭ
 
 
 
 			case CSystem::eINIT_StepSocket1 :				// 6. Socket, ù��° ���� �ʱ�ȭ
 					// Socket ��� ��Ʈ ���� : �� stage���� ����. ���⼭�� �ʱ�ȭ�� �Ҽ� �ֵ��� ��.
 					// Create�ϸ� ���� �߻���. client ���� Ȯ���� ��� �Ұ��ΰ�?
 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 					pSystem->m_nInitSeq		= CSystem::eINIT_StepSocket2;
 
 					strInitLog.Format("[Initialize] TCP/IP Socket1 Init Complet!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					break;
 
 			case CSystem::eINIT_StepSocket2 :				// 7. Socket, ���° ���� �ʱ�ȭ
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 					pSystem->m_nInitSeq		= CSystem::eINIT_StepSocket3;
 
 					strInitLog.Format("[Initialize] TCP/IP Socket2 Init Complet!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					break;
 
 			case CSystem::eINIT_StepSocket3 :				// 8. Socket, ����° ���� �ʱ�ȭ
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);					// ���� �ܰ��� �ʱ�ȭ ������ �˸���.
 					pSystem->m_nInitSeq		= CSystem::eINIT_StepBaseThread;
 					strInitLog.Format("[Initialize] TCP/IP Socket3 Init Complet!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					break;
 	
 			case CSystem::eINIT_StepBaseThread :				//  9. Base Thread ����
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					if ( !(pSystem->Smith.m_hThread) ) {									// Thread ������ �ʵǾ������� ������Ų��.
 						bResult = pSystem->Smith.CreateThread(0, 0, NULL);
 
 						if ( !bResult ) {			// ���� ����
 							pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 							pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_BaseThread, 0);	// Base thread ���� ����
 							pSystem->m_bInitPauseFlag = true;																		
 							break;
 						}
 					}
 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);
 					pSystem->m_nInitSeq		= CSystem::eINIT_StepModuleThread;
 
 					strInitLog.Format("[Initialize] Interface Thread Create Complet!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					break;
 
 
 			case CSystem::eINIT_StepModuleThread :				//  10. Module Thread ����
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					bResult = TRUE;
 					for ( i = 0; i < NUM_INSP_STAGE; i++)
 					{
 						if( !(pSystem->Stage[i].Inspect.m_hThread) )
 						{
 							bResult = pSystem->Stage[i].Inspect.CreateThread(0, 0, NULL);			// ������ Thread ���� �õ�
 							//??
 							//?? [TODO] inspect ���� inspThrd ������ stage.init() ��� ���⼭ �ϴ� ���� ���� �� 
 
 							if ( !bResult ) {			// ���� ����
 								pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, i);				
 								pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_ModuleThread, i);	// Module thread ���� ����
 								pSystem->m_bInitPauseFlag = true;																		
 								break; // for i loop�� �����⶧���� ���� ��ġ �ʿ�
 							}
 						}//of if create
 					}//of for i
 
 					if ( bResult ) { // �������� Thread ������ ���ƴٸ�,
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);
 						pSystem->m_nInitSeq		= CSystem::eINIT_StepModelSet;
 					}
 					break;
 
 
 			case CSystem::eINIT_StepModelSet :				//  11. Model ���� 
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 
 					//??	if(!pSys->ChangeModelData(pSys->m_Config.ModelName))	
 					if (! (pSys->ChangeModelData()) ) 
 					{
 						// Log strMsg.Format("StartHW() : ChangeModelData - Fail");
 
 						pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, pSystem->m_nInitSeq, 0);				
 						pSystem->pMasterWnd->PostMessage(WM_SYS_ALARM_SET, CSystem::eERR_SYSHW_ModelSet, 0);	// Model ���� ����
 						pSystem->m_bInitPauseFlag = true;																		
 						break;
 					}	
 					// Log strMsg.Format("StartHW() : ChangeModelData - sucess");
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);			
 					pSystem->m_nInitSeq		= CSystem::eINIT_StepFinish;		// �ʱ�ȭ �Ϸ�
 
 					strInitLog.Format("[Initialize] Model File Loading Complete!!");
 					pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
 					break;
 
 
 
 			case CSystem::eINIT_StepFinish :					// 12. �ʱ�ȭ ������	
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_PROCESS, pSystem->m_nInitSeq, 0);
 					pSystem->SystemReady.Set();
 
 					// log strMsg.Format("StartHW() : SystemReady - sucess");
 					// pSys->StartLogFile(&strMsg);
 					pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, pSystem->m_nInitSeq, 0);			
 					pSystem->m_bInitTerminateFlag = true;			//  Thread�� �ݺ��� �����Ѵ�.
 					break;
 		}//of switch ( m_nInitSeq )
 
 		if ( !(pSystem->m_bInitTerminateFlag) ) Sleep(100);		// CPU ������ ��� ����
 	}//of while


	// ����� ����/������ �������� Event�� �����ش�.
	if ( pSystem->m_nInitSeq == CSystem::eINIT_StepFinish )
	{
		//2015.04.26 ���缳�� ��ĵŸ���� ��Ȯ���� �޼��� ǥ��
		if(pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_POL || pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_13_POL_CF
			|| pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_13_POL_TFT || pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_546_POL_CF || pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_546_POL_TFT
			|| pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_Big_POL_TFT_Cam1 || pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_Big_POL_TFT_Cam2 
			|| pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_116_POL_CF || pSystem->m_Config.m_nCellType == E_MEASURE_TYPE_116_POL_TFT){
			if(pSystem->m_Config.m_nScanType != 1){
				AfxMessageBox(_T("Please Can you check the ScanType!!"));
			}
		}else{
			if(pSystem->m_Config.m_nScanType != 2){
				AfxMessageBox(_T("Please Can you check the ScanType!!"));
			}
		}
		pSystem->pMasterWnd->PostMessage(WM_SET_GRAB_TIME_VALUE, 1, 0);
		pSystem->pMasterWnd->PostMessage(WM_SET_INSPECT_INFO_VALUE, 1, 0);	//2015.05.10 add by mby 
		//pSystem->m_tdefectDisplyParam[0].Total_defect = 10;
		//pSystem->m_tdefectDisplyParam[0].Ng_defect = 7;
		//pSystem->m_tdefectDisplyParam[0].Defect_Type[0] = 2;
		//pSystem->m_tdefectDisplyParam[0].Defect_Type[1] = 5;
		//pSystem->pMasterWnd->PostMessage(WM_SET_INSPECT_INFO_VALUE, 0, 0);	//2015.05.10 add by mby 
		//CString strDefectTypeList,Temp ,tLog;
		//for(int i =0 ; i < MAX_DEFECT_COUNT ; i++){
		//	Temp.Format(_T("%d (%0.2f),"),pSystem->m_tdefectDisplyParam[0].Defect_Type[i],(double)(pSystem->m_tdefectDisplyParam[0].Defect_Type[i]*100)/ pSystem->m_tdefectDisplyParam[0].Ng_defect);
		//	strDefectTypeList += Temp;
		//}
		//tLog.Format(_T("%s , %d , %d , %s "), _T("PNL00001"), pSystem->m_tdefectDisplyParam[0].Total_defect,pSystem->m_tdefectDisplyParam[0].Ng_defect , strDefectTypeList);
		//pSystem->InspectInfoLogFile(&tLog);

		////2015.05.14 add by mby S-
		//CString strGridLogMsg = _T("1\t2\t3\t4\t5\t6\t6\t7");
		//pSystem->SendCopyMsg(SYS_MSG_LOG_TEST, strGridLogMsg);
		////2015.05.14 add by mby E-
		//2015.06.30 add by mby S-
		// 2015.09.01 ����̺꺰 Limit �뷮 �߰�
		pSystem->SearchDriveDisk();
		pSystem->GetUseDrivePath();
		pSystem->ProcessRamDiskFile();						//Ram Drive �� ����ϴ� ��� ������ ���� ���� ��ũ�� �̵� ��Ų��. ��׶��� ó��
		//2015.10.19 ���� �ʱ�ȭ�� Interlock ��ȣ OFF��Ŵ
		CString strParam;			
		strParam.Format(_T("OFF"));
		//2016.02.24 add by ikm - ������ ���� -> IPC ������� ����- S
		pSystem->m_pVsMessage->SendVsMsg_ToServer_Interlock(strParam);
		pSystem->CommLogFile(&CString("SendVsMsg_ToServer_Interlock Send"), LOG_LEVEL_BASIC);
		//pSystem->Smith.HandlingSendMsg(VTM_INTERNAL_PROCESS_INTERLOCK,(LPSTR)(LPCTSTR)strParam);
		//2016.02.24 add by ikm - ������ ���� -> IPC ������� ����- E
		pSystem->m_bInterLockProceeFlag = FALSE;
		//2015.10.19 add by mby

		//pSystem->m_Config.strImageDrviePath = _T("D:\\");
		CString strPathfolder;
		strPathfolder.Format(_T("%sARESULT"),  pSys->m_Config.strImageDrviePath);
		CreateDirectory(strPathfolder, NULL);
		strPathfolder.Format(_T("%sTemp"), pSys->m_Config.strImageDrviePath);		
		CreateDirectory(strPathfolder, NULL);
		//2015.06.30 add by mby E-

		//2015.04.26 ���缳�� ��ĵŸ���� ��Ȯ���� �޼��� ǥ��
		//ShellExecute(NULL, "open", "cmd", "/C DATE 2016-03-30 /C TIME 16:11:11.11", NULL, SW_HIDE);
		//ShellExecute(NULL,_T("runas"), "D:\\Shared\\EmptyStandbyList.exe", NULL, NULL, SW_HIDE);
		pSystem->m_pVsMessage->SendVsMsg_ToServer_TimeCheck();

		strInitLog.Format("[Initialize] Finish Complete!!");
		pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);		

		pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_EVENT, CSystem::eEVT_InitFinish, 0);	// �������� �ʱ�ȭ �Ϸ�
		pSystem->SetSystemInitComplete(TRUE);
	}
	else
	{
		strInitLog.Format("[Initialize] Fail!!");
		pSystem->StartLogFile(&strInitLog, LOG_LEVEL_BASIC);
		pSystem->pMasterWnd->PostMessage(WM_SYS_INIT_EVENT, CSystem::eEVT_InitAbort, 0);		// ���߿� �ʱ�ȭ ����
	}

	return 0;
}

UINT ThreadSetNetworkDrive(LPVOID pVoid)
{
	//20160818 - ��Ʈ��ũ ����̺긦 �׻� üũ�Ͽ�, �����Ǿ����� �ٽ� ����.
	CSystem* pSystem = CSystem::GetInstance();
	TCHAR buf[512];
	DWORD BufSize = 512;
	NETRESOURCE nr;

	DWORD	Result;
	CString	tLog;
	memset(&nr, NULL, sizeof(NETRESOURCE));
	CString	strPath;		//
	int nFailCount = 0;

	switch (pSystem->m_Config.m_nInspactEqpNo)
	{
		case EQP_PC_1: strPath.Format(_T("\\\\%s\\visionclient-1"),pSystem->m_Config.Serverip); break;
		case EQP_PC_2: strPath.Format(_T("\\\\%s\\visionclient-2"),pSystem->m_Config.Serverip); break;
		case EQP_PC_3: strPath.Format(_T("\\\\%s\\visionclient-3"),pSystem->m_Config.Serverip); break;
		case EQP_PC_4: strPath.Format(_T("\\\\%s\\visionclient-4"),pSystem->m_Config.Serverip); break;
		case EQP_PC_5: strPath.Format(_T("\\\\%s\\visionclient-5"),pSystem->m_Config.Serverip); break;
		case EQP_PC_6: strPath.Format(_T("\\\\%s\\visionclient-6"),pSystem->m_Config.Serverip); break;
		case EQP_PC_7: strPath.Format(_T("\\\\%s\\visionclient-7"),pSystem->m_Config.Serverip); break;
		case EQP_PC_8: strPath.Format(_T("\\\\%s\\visionclient-8"),pSystem->m_Config.Serverip); break;
		case EQP_PC_9: strPath.Format(_T("\\\\%s\\visionclient-9"),pSystem->m_Config.Serverip); break;			
	}

	_tcscpy(buf,strPath);

	nr.lpRemoteName = buf;
	nr.lpLocalName  = _T("M:");               
	nr.dwType = RESOURCETYPE_DISK;
	nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
	nr.dwScope = RESOURCE_REMEMBERED;
	nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
	nr.lpComment = NULL;
	//����Ȯ����Connection�� �������ʿ�� ����-������ �ٽÿ����Ҷ� ����������� ��Ʈ��ũ�� �����������������ִ�. ���������� ����ִ� ���¿����� ������ �����ʴ´�.
	//WNetCancelConnection2(nr.lpLocalName, CONNECT_UPDATE_PROFILE, TRUE);
	WNetCancelConnection2(nr.lpLocalName, CONNECT_UPDATE_PROFILE, FALSE);

	//2016.09.22 - Network Drive ���� ����
	//WNetCancelConnection2(nr.lpLocalName, CONNECT_UPDATE_PROFILE, FALSE);

	while(1)
	{
		if(WNetGetConnection(nr.lpLocalName, buf, &BufSize) == ERROR_NOT_CONNECTED){

			Result = WNetAddConnection2(&nr, pSystem->m_Config.Passward, pSystem->m_Config.Admin, CONNECT_UPDATE_PROFILE); 
			if(Result == NO_ERROR){
				nFailCount = 0;
				pSystem->CommLogFile(&CString("WNetAddConnection2 OK"), LOG_LEVEL_BASIC);
				break;
			}else{
				nFailCount++;
				pSystem->CommLogFile(&CString("WNetAddConnection2 NG"), LOG_LEVEL_BASIC);
			}
		}else{
			tLog.Format(_T("WNetGetConnection BufSize(%d)"), BufSize);
			pSystem->CommLogFile(&tLog, LOG_LEVEL_BASIC);			
			break;
		}

		if(nFailCount > 5)
		{
			AfxMessageBox(_T("Can Not Connect To Network Drive."));
			tLog.Format(_T("Can Not Connect To Network Drive."));
			pSystem->CommLogFile(&tLog, LOG_LEVEL_BASIC);
			break;
		}
		
		Sleep(20000);	//5000->20000
	}

	return 0;
}

//OLD
// UINT ThreadInitFunc(LPVOID lParam) �Լ��� ��ü
UINT StartHW( LPVOID pParam )
{
	//
 	//! AfxOleInit();
	// ���� ���� : http://msdn.microsoft.com/ko-kr/library/e91aseaz.aspx
	// AfxOleInit�� MFC DLL�κ��� ȣ��Ǵ� ���, ȣ���� �����մϴ�.  
	// �Լ��� DLL�κ��� ȣ��Ǹ�, OLE �ý����� ȣ���ϴ� �������α׷��� ���ؼ� �̹� �ʱ�ȭ�Ǿ��ٰ� �Լ��� �����ϱ� ������ ������ �߻��մϴ�. 
	//

	int i;
	int cnt = 0;	
	CString strMsg;
	CString	strReadData = "";

	// CSystem* pSys = CSystem::GetInstance();				// CSystem�� Instance�� ��ȸ�Ѵ�.

	//pSys->TraceLogFile(&CString("--------------------------------------------"));
	//pSys->CommLogFile(&CString("--------------------------------------------"));
	//pSys->StartLogFile(&CString("Initaial Start--------------------------------"));

	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	1. �ʱ�ȭ ���� ���� : path validity
	CFileFind fileFinder;
	if(fileFinder.FindFile(pSys->m_Config.GetPath()) == FALSE)	// fail
		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL,		1,	0);
	else
		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS,	1,	0);
	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2. �� ���� Ȯ��: path validity
	if(fileFinder.FindFile(pSys->m_Config.m_strModelPath) == 0)	// fail
		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL,		2,	0);
	else
		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS,	2,	0);	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// vision system -> Thread ���� 

	if( pSys->m_Config.IsHW )
	{
//		for(i=0; i<NUM_INSP_STAGE; i++)
		for(i=0; i<NUM_INSP_STAGE-1; i++)  //�������� ������ 1�� :
		{	
			strMsg.Format("StartHW() : Stage(%d) �ʱ�ȭ - Start", i);
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);
			
			if( !pSys->Stage[i].Init(i) )		// stage ������ �ʱ�ȭ (inspThrd ����)
			{
				strMsg.Format("StartHW() : Stage(%d) �ʱ�ȭ - Fail", i);
				//pSys->TraceLogFile(&strMsg);
				//pSys->StartLogFile(&strMsg);
				
				pSys->m_bNowInitializing = SYS_INIT_FAIL;
				
				return FALSE;
			}
			
			strMsg.Format("StartHW() : Stage(%d) �ʱ�ȭ - End", i);
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);
		}
	}


	// 3. Serial Port ���� - Light , Trigger
	if(pSys->m_Config.IsHW)
	{
		// Light Connetct --> COM1
		
		if( !pSys->FrontLight.ConnectPg( pSys->m_Config.m_nSettingLightPort+1, _nBaudRates[pSys->m_Config.m_nSettingLightBaud] ) ) //COM1
		{
			pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL,		INIT_STEP_3, (NUM_INSP_STAGE - 2) );

			strMsg.Format("StartHW() : Light Port �ʱ�ȭ - Fail");
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);
			pSys->m_bNowInitializing = SYS_INIT_FAIL;

			return FALSE;
		}
		else
		{
			pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, INIT_STEP_3, (NUM_INSP_STAGE - 2) );
			strMsg.Format("StartHW() :  Light Port �ʱ�ȭ - Sucess");
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);


		}

		// Trigger Connetc --> Com2
		// og stage�� pattern generator�� ����.
		if(!pSys->Trigger.ConnectTrigger(NUM_INSP_STAGE, 9600)){ // COM2
			pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, 3, NUM_INSP_STAGE-1);

			strMsg.Format("StartHW() : Trigger Port �ʱ�ȭ - Fail");
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);
			pSys->m_bNowInitializing = SYS_INIT_FAIL;
			return FALSE;
		}
		else
		{
			pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 3, NUM_INSP_STAGE-1);
			strMsg.Format("StartHW() :  Trigger Port �ʱ�ȭ - Sucess");
			//pSys->TraceLogFile(&strMsg);
			//pSys->StartLogFile(&strMsg);

			// Trigger Board init
			Sleep(100);


			pSys->Trigger.StopTriggerGen0();
			Sleep(100);
			pSys->Trigger.StopTriggerGen1();
			Sleep(100);
			// Trigger Borad �ʱ�ȭ
 			pSys->Trigger.TriggerInit0();
			Sleep(100);
			pSys->Trigger.TriggerInit1();
			Sleep(100);


//			pSys->Trigger.TriggerOrigin();	
// 			pSys->Trigger.TriggerStartPosition2K();
// 			Sleep(100);
// 
// 			pSys->Trigger.TriggerStartPosition8K();
// 
// 			pSys->Trigger.StartForwardScan();

// 			pSys->Trigger.TriggerStartPositionGet2K();
// 			pSys->Trigger.TriggerStartPositionGet8K();
// 
// 			pSys->Trigger.TriggerCurrentPosition();		
// 			pSys->Trigger.TriggerGenCount0();			
// 			pSys->Trigger.TriggerGenCount1();

 
// 			pSys->Trigger.StartForwardScan();

// 			pSys->Trigger.TriggerStartPosition();
// 			pSys->Trigger.StartForwardScan();

//			pSys->Trigger.StartBackwardScan();

		}

	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 4. Socket ��� ��Ʈ ���� : �� stage���� ����. ���⼭�� �ʱ�ȭ�� �Ҽ� �ֵ��� ��.
	// Create�ϸ� ���� �߻���. client ���� Ȯ���� ��� �Ұ��ΰ�?

	//if(!pSys->m_FirActSvSocket.InitSocket()){
	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, 4, 0);

	//		strMsg.Format("StartHW() : InitSocket �ʱ�ȭ - Fail");
	//		//pSys->TraceLogFile(&strMsg);
	//		//pSys->StartLogFile(&strMsg);

	//		pSys->m_bNowInitializing = SYS_INIT_FAIL;
	//		return FALSE;
	//}
	//else{
	//	strMsg.Format("StartHW() : InitSocket �ʱ�ȭ - sucess");
	//	//pSys->TraceLogFile(&strMsg);
	//	//pSys->StartLogFile(&strMsg);

	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 4, 0);
	//}
	//
	//if(!pSys->m_SndActSvSocket.InitSocket()){
	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, 4, 1);

	//		strMsg.Format("StartHW() : m_SndActSvSocket.InitSocket �ʱ�ȭ - Fail");
	//		//pSys->TraceLogFile(&strMsg);
	//		//pSys->StartLogFile(&strMsg);

	//		pSys->m_bNowInitializing = SYS_INIT_FAIL;
	//		return FALSE;
	//}
	//else{
	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 4, 1);
	//	strMsg.Format("StartHW() : m_SndActSvSocket.InitSocket �ʱ�ȭ - sucess");
	//	//pSys->TraceLogFile(&strMsg);
	//	//pSys->StartLogFile(&strMsg);

	//}
	//
	//if(!pSys->m_ExtSvSocket.InitSocket()){
	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, 4, 2);
	//	pSys->m_bNowInitializing = SYS_INIT_FAIL;
	//		return FALSE;
	//	}
	//else{
	//	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 4, 2);
	//}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	5. ���̽� �� ��� Thread ����
		BOOL Result = TRUE, _rst;
	
		if(!pSys->Smith.m_hThread){
			_rst = pSys->Smith.CreateThread(0, 0, NULL);
		}
		Result&=_rst;
		pSys->pMasterWnd->PostMessage((_rst)?WM_SYS_INIT_SUCCESS:WM_SYS_INIT_FAIL, 5, 0);
	
		for(i=0; i<NUM_INSP_STAGE; i++)
		{
			if(!pSys->Stage[i].Inspect.m_hThread)
			{
				_rst = pSys->Stage[i].Inspect.CreateThread(0, 0, NULL);
				Result&=_rst;
	
				// [TODO] inspect ���� inspThrd ������ stage.init() ��� ���⼭ �ϴ� ���� ���� ��
	
				pSys->pMasterWnd->PostMessage((_rst)?WM_SYS_INIT_SUCCESS:WM_SYS_INIT_FAIL, 5, i+1);	// 1, 2, 3
			}
		}
	
		if(!Result)
		{
			pSys->m_bNowInitializing = SYS_INIT_FAIL;
			return FALSE;
		}
 		
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 6. �� ����
//	if(!pSys->ChangeModelData(pSys->m_Config.ModelName))	
	if(!pSys->ChangeModelData())
	{
		pSys->m_bNowInitializing = SYS_INIT_FAIL;

		strMsg.Format("StartHW() : ChangeModelData - Fail");
		//pSys->TraceLogFile(&strMsg);
		//pSys->StartLogFile(&strMsg);

		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_FAIL, 6, 0);

		return FALSE;
	}
	else
	{
		pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 6, 0);
		strMsg.Format("StartHW() : ChangeModelData - sucess");
		//pSys->TraceLogFile(&strMsg);
		//pSys->StartLogFile(&strMsg);
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 6. �ʱ�ȭ �Ϸ�
 	pSys->pMasterWnd->PostMessage(WM_SYS_INIT_SUCCESS, 7, 0);
 
 	pSys->SystemReady.Set();
	strMsg.Format("StartHW() : SystemReady - sucess");
	//pSys->TraceLogFile(&strMsg);
	//pSys->StartLogFile(&strMsg);

	return TRUE;
}

/////////////////
// util functions
CString CSystem::GetStringFromFile(CString FileRootName, CString Section, CString Key)
{
	char		Buff[256];
 	CString		ReadData	= "";
	int			Cnt = 0;
	int			Length = 0;

	GetPrivateProfileString(
		Section,								// points to section name
		Key,									// points to key name
		"Error",								// points to default string
		Buff,									// points to destination buffer
		255,									// size of destination buffer
		FileRootName							// points to initialization filename
		);

	ReadData = CString(Buff);
	Length = ReadData.GetLength();
	Cnt = ReadData.Find('/');
	if(Cnt != -1)	ReadData.Delete(Cnt, Length - Cnt);
 	return	ReadData;

}

DWORD CSystem::SetStringToFile(CString FileRootName, CString Section, CString Key, CString strValue)
{
	WritePrivateProfileString(
		Section,								// points to section name
		Key,									// points to key name
		strValue,									// points to string to add
		FileRootName							// points to initialization filename
		);

 	return GetLastError();
	return 0;
}


CString CSystem::GetString(CString Source, int Index, char Divider)
{
 	CString Result	= "";
	int		End		= 0;
	int		Start	= 0;
	int		i		= 0;
	int		Length  = Source.GetLength();


	for(i = 0; i < Index; i++){
		Start = End + 1;
		End = Source.Find(Divider, Start);		
	}

	if(Index == 1 && End == -1){
		Result = Source;
		return Result;
	}
	else if(Index == 1){
		Result = Source.Mid(0, End);
		return Result;
	}
	else if(End != -1){
		Result = Source.Mid(Start, End - Start);
		return Result;
	}
	else if(End == -1){
		Result = Source.Mid(Start, Length - Start);
		return Result;
 	}

	return Result;
}

BOOL CSystem::SetString(CString FileRootName, CString Section, CString Key, CString Data)
{
//	return WritePrivateProfileString(Section, Key, Data, FileRootName);
	return 0;
}

BOOL CSystem::SetString(CString FileRootName, CString Section, CString Key, CString Data, int Index)
{
	CString ReadData;
	int		i;
	int		Start = 0;
	int		End = 0;
	
	ReadData = GetStringFromFile(FileRootName, Section, Key);

	for(i = 0; i < Index; i++){
		Start = End + 1;
		End = ReadData.Find(',', Start);		
 	}

	return TRUE;
}

// retrieve id string from socket message
// format : aaaaa.000
// message�� ���� id�� ���� ����, ���� ���� ����
CString CSystem::GetIdfromMsg(CString tmp)
{
	CString Strid;

	int idx = tmp.ReverseFind('.');
	Strid = tmp.Right(tmp.GetLength() - idx -1);

	int id = atoi(Strid.GetBuffer(0));	// ���ڰ� �ƴ� ��� 0�� ������ (panel ID : 001 ~ 999)

	if(id > 0)
		return Strid;

	return "";
}

//

UINT CSystem::GetCurrentMode()
{
	//jskim �߰� ����.

	//CCruxData *pCruxData = CCruxData::GetInstance();

	CMainFrame* pMainWnd = (CMainFrame*)pMasterWnd;

	::SendMessage(pMainWnd->m_hWnd, WM_GET_VIEW_POINT, 0, 0);
	/*/
	if(pMainWnd->GetActiveView() == pMainWnd->m_pViewTeach)
		m_nCurMode = VIEW_TEACH;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewManual)
		m_nCurMode = VIEW_MANUAL;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewAuto)
		m_nCurMode = VIEW_AUTO;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewSocket)
		m_nCurMode = VIEW_SOCKET;
	else 
		m_nCurMode = VIEW_INIT;
	/*/
	if(pMainWnd->GetActiveView() == pMainWnd->m_pViewTeaching)
		m_nCurMode = VIEW_TEACH;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewManual)
		m_nCurMode = VIEW_MANUAL;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewAuto)
		m_nCurMode = VIEW_AUTO;
	else if(pMainWnd->GetActiveView() == pMainWnd->m_pViewSetup)
		m_nCurMode = VIEW_SOCKET;
	else 
		m_nCurMode = VIEW_INIT;

 	return m_nCurMode;
	return 0;
}

// 
UINT CSystem::SetCurrentMode(UINT nMode)
{
	pMasterWnd->SendMessage(WM_SWITCH_VIEW, nMode, 0);
	m_nCurMode = nMode;

	return 0;
}

UINT CSystem::SetEquipInfo(LPCTSTR strPath)
{
	CString strTemp;

	GetPrivateProfileString(
		"General",							// points to section name
		"Equip",							// points to key name
		"",										// points to default string
		(char*)(LPCTSTR)strTemp,								// points to destination buffer
		255,									// size of destination buffer
		strPath									// points to initialization filename
		);

	strTemp.MakeUpper();

	if(strTemp == _T("AFT")) {
//		m_nEquipment = EQUI_AFT;
		m_Config.m_nEquipment = EQUI_AFT;
	} else {
//		m_nEquipment = EQUI_AVT;
		m_Config.m_nEquipment = EQUI_AVT;
	}

	return 0;
}

int CSystem::GetEquipInfo()
{
	return m_Config.m_nEquipment;
	return 0;
}

void CSystem::OnDestroy() 
{
	CWnd::OnDestroy();
	
//	CleanUpBuffers();
}

// DB ���� by kanto

void CSystem::TraceLogDB(int logID)
{
// 	CDB db;
// 	SYSTEMTIME curTime;
// 	::GetLocalTime(&curTime);
// 	CString strSql, DateTime;
// 	int MID;
// 	
// 	MID = pSys->m_Config.iEquipID;
// 	DateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, 
// 		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
// 
// 	if(db.Connect() && logID == 1400) {
// 		strSql.Format("INSERT INTO Log(DateTime, LCode, MID) VALUES('%s', '%d', '%d')", DateTime, logID, MID);
// 		db.ExecSql(strSql);
// 		db.Disconnect();
//  	}
}

//void CSystem::TraceResultDB(CPanel *pPanel)
//{
// 	CDB db;
// 	SYSTEMTIME curTime;
// 	CString strSql, DateTime, strTable;
// 	int MID;
// 
// 	::GetLocalTime(&curTime);
// 	DateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, 
// 		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
// 	MID = pSys->m_Config.iEquipID;
// 
// // 	if(db.Connect()) {
// // 		if(pPanel->GetStageID() == 2) {
// // 			strTable = "Panel_OG";
// // 		}
// // 		else {
// // 			strTable = "Panel";
// // 		}
// // 		strSql.Format("INSERT INTO %s(MID, ModelName, OCR, Candidate, Cst, DateTime, StageID) VALUES('%d', '%s', '%s', '%d', '%d', '%s', '%d')", 
// // 				strTable, MID, pSys->ModelInfo.GetModelName(), pPanel->GetOCR(), pPanel->GetInspResult(), pPanel->GetBinClass(), DateTime, pPanel->GetStageID());
// // 		db.ExecSql(strSql);
// // 		db.Disconnect();
// // 	}
//}

void CSystem::TraceErrorHistoryDB(int errorCode)
 {
// 	CDB db;
// 	SYSTEMTIME curTime;
// 	::GetLocalTime(&curTime);
// 	CString strSql, DateTime;
// 	int MID;
// 	
// 	MID = pSys->m_Config.iEquipID;
// 	DateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, 
// 		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
// 
// 	if(db.Connect()) {
// 		strSql.Format("INSERT INTO ErrorHistory(DateTime, ECode, MID) VALUES('%s', '%d', '%d')", DateTime, errorCode, MID);
// 		db.ExecSql(strSql);
// 		db.Disconnect();
// 	}
}

void CSystem::TraceSystemLogDB(int sCode)
{
// 	CDB db;
// 	SYSTEMTIME curTime;
// 	::GetLocalTime(&curTime);
// 	CString strSql, DateTime;
// 	int MID;
// 	
// 	MID = pSys->m_Config.iEquipID;
// 	DateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, 
// 		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
// 
// // 	if(db.Connect()) {
// // 		strSql.Format("INSERT INTO SystemLog(DateTime, SCode, MID) VALUES('%s', '%d', '%d')", DateTime, sCode, MID);
// // 		db.ExecSql(strSql);
// // 		strSql.Format("UPDATE MAVT SET CurrentState='%d', ModelName='%s' WHERE MID='%d'", sCode, pSys->ModelInfo.GetModelName(), MID);
// // 		db.ExecSql(strSql);
// // 		db.Disconnect();
//  // 	}
 }
 
 void CSystem::TraceRunTimeDB(BOOL rCode)
 {
// 	CDB db;
// 	SYSTEMTIME curTime;
// 	::GetLocalTime(&curTime);
// 	CString strSql, DateTime;
// 	int MID;
// 	
// 	MID = pSys->m_Config.iEquipID;
// 	DateTime.Format("%04d-%02d-%02d %02d:%02d:%02d.%03d", curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour, 
// 		curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
// 
// 	if(db.Connect()) {
// 		strSql.Format("INSERT INTO RunTime(RCode, DateTime, MID) VALUES('%d', '%s', '%d')", rCode, DateTime, MID);
// 		db.ExecSql(strSql);
// 		if(rCode < 200) {
// 			strSql.Format("INSERT INTO SystemLog(DateTime, SCode, MID) VALUES('%s', '%d', '%d')", DateTime, 3100, MID);
// 			db.ExecSql(strSql);
// 			strSql.Format("UPDATE MAVT SET CurrentState='%d' WHERE MID='%d'", 3100, MID);
// 			db.ExecSql(strSql);
// 		}
// 		else if(rCode == 201) {
// 			strSql.Format("INSERT INTO SystemLog(DateTime, SCode, MID) VALUES('%s', '%d', '%d')", DateTime, 3200, MID);
// 			db.ExecSql(strSql);
// 			strSql.Format("UPDATE MAVT SET CurrentState='%d' WHERE MID='%d'", 3200, MID);
// 			db.ExecSql(strSql);
// 		}
// 		else if(rCode == 205) {
// 			strSql.Format("INSERT INTO SystemLog(DateTime, SCode, MID) VALUES('%s', '%d', '%d')", DateTime, 3500, MID);
// 			db.ExecSql(strSql);
// 			strSql.Format("UPDATE MAVT SET CurrentState='%d' WHERE MID='%d'", 3500, MID);
// 			db.ExecSql(strSql);
// 		}
// 		db.Disconnect();
//  	}
}

BOOL CSystem::CheckManualOcr(CString strOcr)
{
	if(strOcr.Compare("ManualTest") == 0) {
		return FALSE;
	}
	else if(strlen((char*)(LPCTSTR)strOcr) == 4) {
		return FALSE;
 	}

	return TRUE;
}

void CSystem::AutoDeleteFile(CString Target, int Duration)
{
	BOOL	bWorking;

	CFileFind	finder;
	CFileStatus	rStatus;
	CString		strFileName;

	
	/*
	struct tm when;
	time_t	now, result;

	time(&now);
	when	= *localtime(&now);
	*/
	CTime CurTime = CTime::GetCurrentTime();
//	when.tm_mday	= when.tm_mday - Duration;
//	result	= mktime(&when);


	bWorking = finder.FindFile(Target);
	if (!bWorking)
		return;

	while (bWorking) 
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots() != TRUE )
		{
			strFileName	= finder.GetFilePath();
			CFile::GetStatus(strFileName, rStatus);
			CTime fileTime = rStatus.m_mtime;
			CTimeSpan ts = CurTime - fileTime;
			//if (rStatus.m_mtime.GetTime() < result)
			if (ts.GetDays() > Duration)
			{
				if( finder.IsDirectory() != TRUE )
					DeleteFile(strFileName);
				else
				{
					CString strtemp = strFileName;
					strtemp+="\\*.*";
					AutoDeleteFile(strtemp,Duration);
					RemoveDirectory(strFileName);
				}
			}
		}
	}
}



void CSystem::StartLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}
	EnterCriticalSection(&m_csProgramLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);

	//jskim
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);
	
	CString fName = m_Config.m_strLogFilePath;
	CreateDirectory(fName, NULL);
	fName += CString("\\") + strToday;
	//CString fName = "D:\\CRUX\\LOG";
	CreateDirectory(fName, NULL);

	fName += CString("\\") + "Program";
	CreateDirectory(fName, NULL);

	fName += CString("\\") + strT + ".log";

	FILE* fp = NULL;

	// KSW 15.01.01
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}while( !fp );

	//_tfopen_s(&fp, LPCTSTR(fName), "at");
	if (!fp) {
		LeaveCriticalSection(&m_csProgramLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 4");
		return;
	}
	
	CString logmsg = strTime + _T("> ") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csProgramLog);
}

void CSystem::TactLogFile(CString *pmsg, int Level)
{
	//16.08.01 Log level
	if( pSys->m_Config.m_nLogLevel > Level){
		return;
	}	
	EnterCriticalSection(&m_csTactLog);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	int msec = time.wMilliseconds;

	CString strTime;
	strTime.Format("%02d:%02d:%02d:%03d ", time.wHour, time.wMinute, time.wSecond, msec);

	CString strT;
	strT.Format("%d_%02d_%02d_%02d", time.wYear, time.wMonth, time.wDay, time.wHour);

	//jskim
	CString strToday;
	strToday.Format("%04d%02d%02d", time.wYear, time.wMonth, time.wDay);

	CString fName = m_Config.m_strLogFilePath;
	CreateDirectory(fName, NULL);
	fName += CString("\\") + strToday;
	//CString fName = "D:\\CRUX\\LOG";
	CreateDirectory(fName, NULL);
	fName += CString("\\") + CString("TactLog");
	CreateDirectory(fName, NULL);

	fName += CString("\\") + strT + ".log";

	FILE* fp = NULL;

	// KSW 15.01.01
	do
	{
		Sleep(10);
		_tfopen_s(&fp, LPCTSTR(fName), "at");
	}while( !fp );

	//_tfopen_s(&fp, LPCTSTR(fName), "at");
	if (!fp) {
		LeaveCriticalSection(&m_csTactLog);
		::AfxMessageBox("System Trace Logging Failed - file open error 5");
		return;
	}

	CString logmsg = strTime + _T("> ") + *pmsg;
	fprintf(fp, "%s\n", logmsg);
	fclose(fp);
	fp = NULL;

	LeaveCriticalSection(&m_csTactLog);
}

BOOL CSystem::GetVisionState()
{
	return m_bVisionState;
}
void CSystem::SetVisionState(BOOL bState)
{
	m_bVisionState = (bState)?true:false;
}
BOOL CSystem::GetSystemInitComplete()
{
	return m_bSysInitComplete;
}
void CSystem::SetSystemInitComplete(BOOL bSucess)
{
	m_bSysInitComplete = bSucess;
}
BOOL CSystem::GetVacMessage(int nID)
{
	return m_bFlgVacMessage[nID];
}
void CSystem::SetVacMessage(int nID, BOOL bFlg)
{
	m_bFlgVacMessage[nID] = bFlg;
}

int	CSystem::GetStageID()
{
	return m_nStageID;
}
void CSystem::SetStageID(int nID)
{
	m_nStageID = nID;
}
CString	CSystem::GetInnerID()
{
	return m_strInnerID;
}
void CSystem::SetInnerID(CString strID)
{
	m_strInnerID = strID;
}
CString	CSystem::GetOldInnerID()
{
	return m_strOldInnerID;
}
void CSystem::SetOldInnerID(CString strID)
{
	m_strOldInnerID = strID;
}

bool CSystem::GetCameraStatus()
{
	return m_bCameraStatus;
}

void CSystem::SetCameraStatus(bool bStatus)
{
	m_bCameraStatus = bStatus;
}

CString	CSystem::GetPanelID()
{
	return m_strPanelID;
}
CString	CSystem::GetPanelID1()
{
	return m_strPanelID1;
}
void CSystem::SetPanelID(CString strID)
{
	m_strPanelID = strID;
}
void CSystem::SetPanelID1(CString strID)
{
	m_strPanelID1 = strID;
}

CString	CSystem::GetModelName()
{
	return m_strModelName;
}

void CSystem::SetModelName(CString strName)
{
	m_strModelName = strName;
}

CString	CSystem::GetDefectPathName()
{
	return m_strDefectPathName;
}

void CSystem::SetDefectPathName(CString strName)
{
	m_strDefectPathName = strName;
}

BOOL CSystem::GetInterfaceAuto()
{
	return m_flgInterfaceAuto;
}

void CSystem::SetInterfaceAuto(BOOL bSet)
{
	m_flgInterfaceAuto = bSet;
}

int CSystem::GetVisionAlignT()
{
	if (m_iVisionAlignT < -5000 || m_iVisionAlignT > 5000)
		return 0;
	else
		return m_iVisionAlignT;
}

int CSystem::GetVisionAlignX()
{
	if (m_iVisionAlignX < -5000 || m_iVisionAlignX > 5000)
		return 0;
	else
		return m_iVisionAlignX;
}

int CSystem::GetVisionAlignY()
{
	if (m_iVisionAlignY < -5000 || m_iVisionAlignY > 5000)
		return 0;
	else
		return m_iVisionAlignY;
}

void CSystem::SetVisionAlignCoodi(int CoordX, int CoordY, int CoordT)
{
	m_iVisionAlignX = CoordX;
	m_iVisionAlignY = CoordY;
	m_iVisionAlignT = CoordT;
}

void CSystem::SetGrabTime(float dTime)
{
	m_fGrabTime = dTime;
}

float CSystem::GetGrabTime()
{
	return m_fGrabTime;
}

void CSystem::SetClassTime(float dTime)
{
	m_fClassTime = dTime;
}

float CSystem::GetClassTime()
{
	return m_fClassTime;
}

void CSystem::SetTotalTactTime(float dTime)
{
	m_fTotalTactTime = dTime;
}

float CSystem::GetTotalTactTime()
{
	return m_fTotalTactTime;
}

CPanel* CSystem::GetPanelCom()
{
	return m_pPanel;
}

void CSystem::SetPanelCom(CPanel* pPanel)
{
	m_pPanel = pPanel;
}

CLightOffset* CSystem::GetLightOffsetCom()
{
	return m_pLightOffset;
}

void CSystem::SetLightOffsetCom(CLightOffset* pLightOffset)
{
	m_pLightOffset = pLightOffset;
}

int CSystem::GetTeachingScanPadPos()
{
	return m_nForwardScanPadPosition;
}

int CSystem::SetTeachingScanPadPos(int nPos)
{
	m_nForwardScanPadPosition = nPos;
	return 0;
}
int CSystem::GetRoiMode()
{
	return m_nRoiModePosition;
}

int CSystem::SetRoiMode(int nPos)
{
	m_nRoiModePosition = nPos;
	return 0;
}





//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingToatalROICount
//	 2. �Լ���� :	Teaching���� ������ ROI �� ���� ���
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	ROI �� ����.
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingToatalROICount(void)
{
	int nCount = 0;

	// ����ó�� �߰� KSW 15.12.04
	if( m_pInfoROI != NULL )
		nCount = m_pInfoROI->nROICnt;

	return nCount;
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingToatalMoniROICount
//	 2. �Լ���� :	Teaching���� ������ MONITOR IMG ROI �� ���� ���
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	ROI �� ����.
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingToatalMoniROICount(void)
{
	int nCount = 0;
	if( m_pInfoMoniImgROI != NULL )
		nCount = m_pInfoMoniImgROI->nROICnt;
	return nCount;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetUsePanelMoniROI
//	 2. �Լ���� :	Teaching���� ������ ROI Panel ��
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	ROI ������ Panel ��ȣ 
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetUsePanelMoniROI(int nCnt)
{
	return m_pInfoMoniImgROI->pRectInfo[nCnt].nUsePanel;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMoniROI
//	 2. �Լ���� :	Teaching���� ������ ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI ���� / CRect
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingMoniROI(int nROINum)
{
	return m_pInfoMoniImgROI->pRectInfo[nROINum].rectROI;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMoniROIName
//	 2. �Լ���� :	Teaching���� ������ ROI �̸� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI Name / LPCTSTR
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
LPCTSTR CSystem::GetTeachingMoniROIName(int nROINum)
{
	return m_pInfoMoniImgROI->pRectInfo[nROINum].strROIName;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkMoni1
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkMoni1(int nROINum)
{
	return m_pInfoMoniImgROI->pRectInfo[nROINum].ptPoint[0];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkMoni2
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkMoni2(int nROINum)
{
	return m_pInfoMoniImgROI->pRectInfo[nROINum].ptPoint[1];
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingToatalFilterROICount
//	 2. �Լ���� :	Teaching���� ������ Filter ROI �� ���� ���
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	ROI �� ����.
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingToatalFilterROICount(void)
{
	int nCount = 0;
	if( m_pInfoFilterROI != NULL )
		nCount = m_pInfoFilterROI->nROICnt;
	return nCount;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetUsePanelFilterROI
//	 2. �Լ���� :	Teaching���� ������ ROI Panel �� ���� ���� - ���� ���
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	ROI ������ Panel ��ȣ - 0 : ���� ����, 1 : Panel1, 2 : Panel2, 3 : Both
//	 5. �� �� �� :	2015/09/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetUsePanelFilterROI(int nCnt)
{
	return m_pInfoFilterROI->pRectInfo[nCnt].nUsePanel;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingToatalSegCount
//	 2. �Լ���� :	Teaching���� ������ Segmentation �� ���� ���
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	Segmentation �� ����.
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingToatalSEGCount(void)
{
	return m_pInfoSEG->nSEGCnt;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingSEG
//	 2. �Լ���� :	Teaching���� ������ Segmenatation ���� ���.
//	 3. ��    �� :	nSEGNum		: SEG ��ȣ
//	 4. �� ȯ �� :	SEG ���� / int
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingSEG(int nSEGNum)
{
	return m_pInfoSEG[nSEGNum].nYCoordSEGLine;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingROI
//	 2. �Լ���� :	Teaching���� ������ ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI ���� / CRect
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingROI(int nROINum)
{
	return m_pInfoROI[nROINum].rectROI;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingROIName
//	 2. �Լ���� :	Teaching���� ������ ROI �̸� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI �̸� : LPCTSTR
//	 5. �� �� �� :	2015/09/16
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
LPCTSTR CSystem::GetTeachingROIName(int nROINum)
{
	return m_pInfoROI[nROINum].strROIName;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingROIRef
//	 2. �Լ���� :	Teaching���� ������ ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI ���� / CRect
//	 5. �� �� �� :	2015/06/17
//	 6. �� �� �� :	CIH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingRefROI(int nROINum)
{
	return m_pInfoRefROI[nROINum].rectROI;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingFilterROI
//	 2. �Լ���� :	Teaching���� ������ ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI ���� / CRect
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingFilterROI(int nROINum)
{
	return m_pInfoFilterROI->pRectInfo[nROINum].rectROI;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingFilterROIName
//	 2. �Լ���� :	Teaching���� ������ ROI �̸� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	ROI Name / LPCTSTR
//	 5. �� �� �� :	2015/09/16
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
LPCTSTR CSystem::GetTeachingFilterROIName(int nROINum)
{
	return m_pInfoFilterROI->pRectInfo[nROINum].strROIName;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMark1
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark 1 ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMark1(int nROINum)
{
	return m_pInfoROI[nROINum].ptPoint[0];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkRef1
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark 2 ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/06/17
//	 6. �� �� �� :	CIH	
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkRef1(int nROINum)
{
	return m_pInfoRefROI[nROINum].ptPoint[1];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkFilter1
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkFilter1(int nROINum)
{
	return m_pInfoFilterROI->pRectInfo[nROINum].ptPoint[0];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMark2
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark 1 ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMark2(int nROINum)
{
	return m_pInfoROI[nROINum].ptPoint[1];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkRef2
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark 2 ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/06/17
//	 6. �� �� �� :	CIH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkRef2(int nROINum)
{
	return m_pInfoRefROI[nROINum].ptPoint[1];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingMarkFilter2
//	 2. �Լ���� :	Teaching���� ROI ���� ��� Align Mark ��ǥ ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	CPoint
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CPoint CSystem::GetTeachingMarkFilter2(int nROINum)
{
	return m_pInfoFilterROI->pRectInfo[nROINum].ptPoint[1];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingToatalDefectCount
//	 2. �Լ���� :	Teaching���� ������ Algorithm List �� ���� ���
//	 3. ��    �� :	����.
//	 4. �� ȯ �� :	Algorithm List �� ����.
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int CSystem::GetTeachingToatalDefectCount(void)
{
	return E_ALGORITHM_NUMBER;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingDefectUse
//	 2. �Լ���� :	Teaching���� ������ ROI -> Defect T/F Ȯ��
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					nAlgorithmNum	: Algorithm List ��ȣ
//	 4. �� ȯ �� :	True / False
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
bool CSystem::GetTeachingDefectUse(int nROINum, int nAlgorithmNum, int nInspSectionNum)
{
	return m_pInfoROI[nROINum].bAlgorithmUse[nInspSectionNum][nAlgorithmNum];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingParameter
//	 2. �Լ���� :	Teaching���� ������ ROI -> Defect -> �Ķ���� ���
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					nAlgorithmNum	: Defect ��ȣ
//					nParaNum	: Parameter ��ȣ
//	 4. �� ȯ �� :	Teaching���� ������ �� / double��
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
double CSystem::GetTeachingParameter(int nROINum, int nAlgorithmNum, int nParaNum, int nInspSectionNum)
{
	return m_pInfoROI[nROINum].dPara[nInspSectionNum][nAlgorithmNum][nParaNum];
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	CopyTeachingParameter
//	 2. �Լ���� :	Teaching���� ������ ROI -> Defect -> �Ķ���� ���
//	 3. ��    �� :	dDstArr		: ��� ���� �迭 ���� �ּ�
//					nROINum		: ROI ��ȣ
//					nAlgorithmNum	: Defect ��ȣ
//					nParaNum	: Parameter ��ȣ
//	 4. �� ȯ �� :	void
//	 5. �� �� �� :	2015/03/18
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	Teaching���� ������ ROI -> Defect -> �Ķ���� �뤊�� ����
//----------------------------------------------------------------------------------------------------
void CSystem::CopyTeachingParameter(double *dDstArr, int nROINum, int nAlgorithmNum, int nInspSectionNum , int Mode)
{
	//2015.07.20 change by mby divide Parameter of Mode
	if(Mode == 0){
		memcpy(dDstArr, m_pInfoROI[nROINum].dPara[nInspSectionNum][nAlgorithmNum], sizeof(double) * MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT );
	}else{
		memcpy(dDstArr, m_pInfoUserParam[nROINum].dPara[nInspSectionNum][nAlgorithmNum], sizeof(double) * MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT );
	}	
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetDefectFilteringParam
//	 2. �Լ���� :	Teaching���� ������ Defect Filtering Parameter �� ������
//	 3. ��    �� :	dDstArr		: ��� ���� �迭 ���� �ּ�
//					nROINum		: ROI ��ȣ
//					nAlgorithmNum	: Defect ��ȣ
//					nParaNum	: Parameter ��ȣ
//	 4. �� ȯ �� :	void
//	 5. �� �� �� :	2015/03/24
//	 6. �� �� �� :	
//	 7. ������� :
//	 8. ��    �� :	Teaching���� ������ ROI -> Defect -> �Ķ���� �뤊�� ����
//----------------------------------------------------------------------------------------------------
void CSystem::GetDefectFilteringParam(tDefectJudgment* dDstArr, int nCnt, int nROINum, int nAlgorithmNum, int nInspSectionNum , int Mode)
{
	//2015.07.20 change by mby divide Parameter of Mode
	if(Mode == 0)
	{
		memcpy(dDstArr, m_pInfoROI[nROINum].stDefectFilteringParam[nInspSectionNum][nAlgorithmNum], sizeof(tDefectJudgment) * nCnt );
	}
	else
	{
		memcpy(dDstArr, m_pInfoUserParam[nROINum].stDefectFilteringParam[nInspSectionNum][nAlgorithmNum], sizeof(tDefectJudgment) * nCnt );
	}
	
	//mm -> Pixel, ���� ���� mm�� ����. 2018.05.22 Lyh.
	for(int nDefectName=0; nDefectName < E_DEFECT_NAME_COUNT; nDefectName++)
	{	
		if(dDstArr[nDefectName].bUse) 
		{
			for(int i=0; i<4; i++)
			{
				for(int nJudgeName=0; nJudgeName < E_DEFECT_JUDGMENT_COUNT; nJudgeName++)
				{
					if(dDstArr[nDefectName].Judgment[i][nJudgeName].bUse && 
					  (nJudgeName == E_DEFECT_JUDGMENT_LENGTH || nJudgeName == E_DEFECT_JUDGMENT_BREADTH || nJudgeName == E_DEFECT_JUDGMENT_FERET_MIN ||
					   nJudgeName == E_DEFECT_JUDGMENT_FERET_MAX || nJudgeName == E_DEFECT_JUDGMENT_FERET_MEAN || nJudgeName == E_DEFECT_JUDGMENT_FERET_X || 
					   nJudgeName == E_DEFECT_JUDGMENT_FERET_Y || nJudgeName == E_DEFECT_JUDGMENT_LENGTH_SUB_FERET_MAX || nJudgeName == E_DEFECT_JUDGMENT_BREADTH_SUB_FERET_MIN || 
					   nJudgeName == E_DEFECT_JUDGMENT_EDGE_RANGE_X || nJudgeName == E_DEFECT_JUDGMENT_EDGE_RANGE_Y) ) 
					   dDstArr[nDefectName].Judgment[i][nJudgeName].dValue = floor(dDstArr[nDefectName].Judgment[i][nJudgeName].dValue/m_Config.m_dbCameraResolution[0]);
				}

			}
		}
	}
			
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingAlignROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	���� ROI ���� / CRect
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingAlignROI(int nROINum, int nThreadID)
{
	CRect rectRet(0,0,0,0);

	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoROI[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID )
		{
			return m_pInfoROI[nROINum].rectROITemp[ThreadCnt].rectROI;
		}

	}

	return rectRet;
	//return m_pInfoROI[nROINum].rectROITemp;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingAlignRefROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	���� ROI ���� / CRect
//	 5. �� �� �� :	2015/06/06
//	 6. �� �� �� :	CIH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingAlignRefROI(int nROINum, int nThreadID)
{
	//CRect rectRet(0,0,0,0);
	
	return m_pInfoRefROI[nROINum].rectROITemp[nThreadID].rectROI;

// 	for (int iCamIndex = 0; iCamIndex < MAX_INSPECTION_SECTION; iCamIndex++)
// 	{
// 		if (m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].nThreadID == nThreadID + 1 )
// 		{
// 			return m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].rectROI;
// 		}
// 	}
// 	return rectRet;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingAlignFilterROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	���� ROI ���� / CRect
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingAlignFilterROI(int nROINum, int nThreadID)
{
	CRect rcCurCorrectROI(0, 0, 0, 0);

	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID )
		{
			rcCurCorrectROI = m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].rectROI;
			break;
		}
	}
	return rcCurCorrectROI;
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	GetTeachingAlignMoniROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ���.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//	 4. �� ȯ �� :	���� ROI ���� / CRect
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
CRect CSystem::GetTeachingAlignMoniROI(int nROINum, int nThreadID)
{
	CRect rcCurCorrectROI(0, 0, 0, 0);

	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID )
		{
			rcCurCorrectROI = m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].rectROI;
			break;
		}
	}
	return rcCurCorrectROI;
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SetTeachingAlignROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ����.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					rect		: ���� ����	
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CSystem::SetTeachingAlignROI(int nROINum, CRect rect, int nThreadID)
{	
	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoROI[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID || 
			m_pInfoROI[nROINum].rectROITemp[ThreadCnt].nThreadID == 0)
		{
			m_pInfoROI[nROINum].rectROITemp[ThreadCnt].rectROI = rect;
			m_pInfoROI[nROINum].rectROITemp[ThreadCnt].nThreadID = nThreadID;
			return;
		}
	}

	//m_pInfoROI[nROINum].rectROITemp = rect;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SetTeachingAlignRefROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ����.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					rect		: ���� ����	
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/02/05
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CSystem::SetTeachingAlignRefROI(int nROINum, CRect rect, int nThreadID)
{
	
	m_pInfoRefROI[nROINum].rectROITemp[nThreadID].rectROI = rect;

// 	for (int iCamIndex = 0; iCamIndex < MAX_INSPECTION_SECTION; iCamIndex++)
// 	{
// 		if (m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].nThreadID == nThreadID + 1 || 
// 			m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].nThreadID == 0)
// 		{
// 			m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].rectROI = rect;
// 			m_pInfoRefROI[nROINum].rectROITemp[iCamIndex].nThreadID = nThreadID + 1;
// 			return;
// 		}
// 	}
	//m_pInfoROI[nROINum].rectROITemp = rect;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SetTeachingAlignFilterROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ����.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					rect		: ���� ����	
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/08/04
//	 6. �� �� �� :	CWH
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CSystem::SetTeachingAlignFilterROI(int nROINum, CRect rect, int nThreadID)
{	
	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID || 
			m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == 0)
		{
			m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].rectROI = rect;
			m_pInfoFilterROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID = nThreadID;
			return;
		}
	}

	//m_pInfoROI[nROINum].rectROITemp = rect;
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SetTeachingAlignMoniROI
//	 2. �Լ���� :	Align Mark ã�� ��, ���� ROI ���� ����.
//	 3. ��    �� :	nROINum		: ROI ��ȣ
//					rect		: ���� ����	
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2016/04/06
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CSystem::SetTeachingAlignMoniROI(int nROINum, CRect rect, int nThreadID)
{	
	for (int ThreadCnt = 0; ThreadCnt < MAX_INSPACT_THREAD_CNT; ThreadCnt++)
	{
		if (m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == nThreadID || 
			m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID == 0)
		{
			m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].rectROI = rect;
			m_pInfoMoniImgROI->pRectInfo[nROINum].rectROITemp[ThreadCnt].nThreadID = nThreadID;
			return;
		}
	}
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SearchDriveDisk
//	 2. �Լ���� :	Local Drvie Disk Search
//	 3. ��    �� :	//					
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/06/07
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CSystem::SearchDriveDisk()
{
	// 2015.09.01 Comment by CWH �Ź� ���� �ʿ� ����
	//DWORD   dwDiskInfo;
	//int    nCount = 0;
	//CString strDrive;	
	//char pTmp[256];      // local drive �̸��� ���� ����
	//memset(pTmp, '\0', 256);
	//CString	ExDrive1 = _T("C:\\");
	//CString	ExDrive2 = _T("D:\\");
	//CString	ExDrive3 = _T("E:\\");
	//
 //   dwDiskInfo = GetLogicalDrives();

 //   char  szDrive[MAX_PATH];
 //   GetLogicalDriveStrings(MAX_PATH, szDrive);
 //   // szDrive�� �Ľ��Ͽ� ����̺� ����� ��� �� �� �ִ�.
	//for(int i=0 ; i<256 ; i++) {
	//	if(szDrive[i] == '\0' && szDrive[i+1] == '\0')
	//	break;
 //    
	//	strDrive.Empty();
	//	while(szDrive[i] != '\0')
	//	strDrive += szDrive[i++];

	//	switch(GetDriveType(strDrive)) {
	//		case DRIVE_CDROM:
	//		break;
	//		case DRIVE_REMOVABLE:
	//		break;
	//		case DRIVE_FIXED:
	//			if(strDrive != ExDrive1 && strDrive != ExDrive2 && strDrive != ExDrive3){
	//				strArrayDrive[nCount] = strDrive;
	//				nCount++;
	//			}
	//		break;
	//		default :
	//		break;
	//	}
	//}
	//for(int i=0 ; i< nCount ; i++) {
	//	SearchDiskInfo(strArrayDrive[i],i);
	//}
	for(int i=0 ; i< m_Config.m_tDriveInfoParam.size() ; i++)
		SearchDiskInfo(m_Config.m_tDriveInfoParam[i].Drive_Name,i);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	SearchDiskInfo
//	 2. �Լ���� :	���� Drive Disk�뷮 Ȯ��
//	 3. ��    �� :	LPCTSTR lpDriver
//					Count
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/06/07
//	 6. �� �� �� :	MBY
//	 7. ������� :
//	 8. ��    �� :	
//---------------------------------------------------------------------------------------------------
void CSystem::SearchDiskInfo(LPCTSTR lpDriver, int count)
{
	float  TolalSize, TotalFree;
    UINT   DiskType;
	float  dPrecent = 0.0;
	CString	Drive_name = _T("");
	CString	ExDrive1 = _T("C:\\");
	CString	ExDrive2 = _T("D:\\");
	CString	ExDrive3 = _T("E:\\");

    DiskType = GetDriveType(lpDriver);
	CMainFrame* pMainFrame = (CMainFrame*)(pSys->pMasterWnd);
 
    if( DiskType == DRIVE_FIXED ) // ���� ��ũ(Hard Disk)�� ��츸 ��ũ ũ�⸦ ���Ѵ�.
    {
        GetDiskFreeSpaceEx(lpDriver, &lFreeBytesAvailable, &lTotalBytes, &lTotalFree);
		TolalSize = GetByte(lTotalBytes);
		TotalFree = GetByte(lTotalFree);
		dPrecent = (float)(TotalFree/TolalSize) *100;
		Drive_name.Format(_T("%s"),lpDriver);
		// 2015.09.01 ����̺꺰 ��� / ���� ���� ���� ����
		//if(lpDriver != ExDrive1 && lpDriver != ExDrive2 && lpDriver != ExDrive3){			
		// 2015.09.21 Comment by CWH - �뷮 üũ ���ո� ����
		//if(m_Config.m_tDriveInfoParam[count].FormatFlag != 99){				
			//m_Config.m_tDriveInfoParam[count].Drive_Name = Drive_name;	// �ߺ� ����
			m_Config.m_tDriveInfoParam[count].Free_PerCent = dPrecent;
			//if(m_dPrecent < DRIVE_LIMIT_SIZE){
			if(dPrecent < 100 - m_Config.m_tDriveInfoParam[count].nDriveLimitSize){
				m_Config.m_tDriveInfoParam[count].FormatFlag = 99;		//��� �Ұ� ���� �� ���� �ʿ�
				//m_Config.m_tDriveInfoParam[count].Total_Drive = 1; 
				//m_Config.m_tDriveInfoParam[count].bUseFlag = false;		//��밡�ɺҰ�
			}else{
				m_Config.m_tDriveInfoParam[count].FormatFlag = 0;		
				//m_Config.m_tDriveInfoParam[count].bUseFlag = true;		//��밡��
			}																		
		//}
		//}

		pMainFrame->PostMessage(WM_SET_PROGRESS_VALUE, count+1,(LPARAM)dPrecent);
		switch(count) {
			case 0:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK1)->SetWindowText(lpDriver);			
				break;
			case 1:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK2)->SetWindowText(lpDriver);			
				break;
			case 2:			
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK3)->SetWindowText(lpDriver);			
				break;
			case 3:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK4)->SetWindowText(lpDriver);				
				break;
			case 4:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK5)->SetWindowText(lpDriver);
				break;
			case 5:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK6)->SetWindowText(lpDriver);
				break;
			case 6:
				pMainFrame->m_pViewAuto->GetDlgItem(IDC_LBL_DISK7)->SetWindowText(lpDriver);
				break;			
			default :
				break;
		}
        //lpData->lDiskFreeSize = lTotalFree;
        //lpData->lDiskSize = lTotalBytes;
    }
    else       // �÷���, �õ�ҵ��� �뷮�� 0���� �Ѵ�.
    {
       // lpData->lDiskFreeSize = 0;
        //lpData->lDiskSize = 0;
    }
}
float CSystem::GetByte(ULARGE_INTEGER dwParam)
{
	//�����ִ� ���� ���� ��������
	//ByteVariable 1 : Kilo
	//ByteVariable 2 : Mega
	//ByteVariable 3 : Giga

	return ((float)dwParam.u.HighPart *4 + ((float)dwParam.u.LowPart/(ByteVariable*ByteVariable*ByteVariable)));
}

//Ram Drive ���� ������ ����
int	CSystem::ProcessRamDiskFile()
{
	static CRamDiskFileCopyManager RamDiskManager(&m_Config.m_bUseRamDisk, &m_Config.m_strRamDrivePath, &m_Config.m_strLastUsedPhysicalDrive);

	if(RamDiskManager.m_fnGetThreadHandle() == NULL)
		RamDiskManager.m_fnStartRamDiskFileCopyManager();

	return 0;
}

void CSystem::GetUseDrivePath()
{	
	int	n_dCheck = 0;	

	// 2015.09.01 ����̺꺰 �뷮 Limit ��� �߰� �� ��ü Sequence ����	
	int nLastDriveIndex;

	// ���� ��� ���� ����̺� �ִ� �뷮���� ���
	for (int nDriveIndex=0 ; nDriveIndex< m_Config.m_tDriveInfoParam.size(); nDriveIndex++)
	{
		if (m_Config.m_tDriveInfoParam[nDriveIndex].Drive_Name == m_Config.strImageDrviePath)
		{			
			if(m_Config.m_tDriveInfoParam[nDriveIndex].bUseFlag == true && m_Config.m_tDriveInfoParam[nDriveIndex].FormatFlag != 99)
			{
				// Ram Drive �߰� ���� 
				if (m_Config.m_bUseRamDisk)
				{
					m_Config.m_strLastUsedPhysicalDrive	= m_Config.strImageDrviePath;
					m_Config.strImageDrviePath			= m_Config.m_strRamDrivePath;
				}
				else
				{
					m_Config.m_strLastUsedPhysicalDrive	= m_Config.strImageDrviePath;
				}
				return;
			}
			nLastDriveIndex = nDriveIndex;
		}
	}

	// ���� ��� �� ����̺긦 �����ϰ� ���������� ���� ����̺� �˻�
	//for (int i=0 ; i< DRIVE_PATH_COUNT ; i++)
	for (int i=0 ; i< m_Config.m_tDriveInfoParam.size() - 1; i++)
	{
		// ����̺� ���� ����
		if (++nLastDriveIndex > m_Config.m_tDriveInfoParam.size() -1)	nLastDriveIndex = 0;

		if(m_Config.m_tDriveInfoParam[nLastDriveIndex].bUseFlag == true && m_Config.m_tDriveInfoParam[nLastDriveIndex].FormatFlag != 99)
		{
			// Ram Drive �߰� ���� 
			if (m_Config.m_bUseRamDisk)
			{
				m_Config.strImageDrviePath			= m_Config.m_strRamDrivePath;
				m_Config.m_strLastUsedPhysicalDrive	= m_Config.m_tDriveInfoParam[nLastDriveIndex].Drive_Name;
			}
			else
			{
				m_Config.strImageDrviePath			= m_Config.m_tDriveInfoParam[nLastDriveIndex].Drive_Name;	
				m_Config.m_strLastUsedPhysicalDrive	= m_Config.m_tDriveInfoParam[nLastDriveIndex].Drive_Name;
			}
			
			//2015.11.09 add by mby S-
			//space Manager ������ �����߻����� ���� Client ���α׷����� �ϳ��� ����̺갡 ������ ��������̺�� �Ѿ�� 
			//����� �����ִ� �κ� �߰�
			//ShellExecute(NULL,_T("open"), "D:\\NeoCrux\\RestartSpaceM.bat", NULL, NULL, SW_HIDE);
			n_dCheck = 1;		
			break;
		}		
	}

	// Default
	if(n_dCheck == 0)
	{
		// //Ram Drive �߰� ���� 
		if (m_Config.m_bUseRamDisk)
		{
			m_Config.strImageDrviePath			= m_Config.m_strRamDrivePath;
			m_Config.m_strLastUsedPhysicalDrive	= _T("D:\\");
			AfxMessageBox(_T("Please Check Free Size on Hard Disk!!!.D����̺�� �����մϴ�."));
		}
		else
		{
			m_Config.strImageDrviePath			= _T("D:\\");
			m_Config.m_strLastUsedPhysicalDrive	= _T("D:\\");
			AfxMessageBox(_T("Please Check Free Size on Hard Disk!!!.D����̺�� �����մϴ�."));
		}

	}
	m_Config.SaveDriveParam();
}

// 2015.09.01 Comment by CWH
//Drive Disk Format or Delete 
//BOOL CSystem::m_fnFormatImg() 
//{
//	//BOOL rc;	
//	EnterCriticalSection(&m_csFormatImg);  //�ǹ� ���� �Ӱ迵��.. 2015.08.25 Comment by KYT
//	if(m_hThreadFormatImg == NULL)
//	{
//		UINT uiThreadID;
//		//m_hThreadFormatImg = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadFormatlmgProc, (LPVOID)this, 0, &uiThreadID);		
//	}
//	m_hThreadFormatImg = NULL;
//	LeaveCriticalSection(&m_csFormatImg);
//	return 1;
//}

// 2015.09.01 Comment by CWH
//unsigned __stdcall  CSystem::m_fnThreadFormatlmgProc(LPVOID pArg )
//{
//	CString	fTempPath;	
//	m_Config.TraceLogFile(&CString("m_fnThreadFormatlmgProc start"));
//	for (int i=0 ; i< DRIVE_PATH_COUNT ; i++)
//	{
//		if(m_Config.m_tDriveInfoParam[i].bUseFlag == false && m_Config.m_tDriveInfoParam[i].FormatFlag == 99){
//			fTempPath.Format(_T("%s%s"),m_Config.m_tDriveInfoParam[i].Drive_Name,_T("Temp"));
//			m_Config.DeleteDirectoryFile(fTempPath);
//			fTempPath.Format(_T("%s%s"),m_Config.m_tDriveInfoParam[i].Drive_Name,_T("ARESULT"));
//			m_Config.DeleteDirectoryFile(fTempPath);
//			m_Config.m_tDriveInfoParam[i].Total_Drive = 0;
//			m_Config.m_tDriveInfoParam[i].FormatFlag = 0;
//			m_Config.m_tDriveInfoParam[i].bUseFlag = true;		//��밡��
//			break;
//		}
//	}	
//	pSys->TraceLogFile(&CString("m_fnThreadFormatlmgProc end"));
//	
//	return 0;
//}
////----------------------------------------------------------------------------------------------------
////	 1. �� �� �� :	DeleteDirectoryFile
////	 2. �Լ���� :	���� ���� ����
////	 3. ��    �� :	RootDir : ���� ���
////	 4. �� ȯ �� :	TRUE	: ���� ���� ���� ���.
////					FALSE	: ������ ��ã�� ���.
////	 5. �� �� �� :	
////	 6. �� �� �� :	
////	 7. ������� :
////	 8. ��    �� :	
////----------------------------------------------------------------------------------------------------
//
//BOOL CSystem::DeleteDirectoryFile(LPCTSTR RootDir)
//{
//	if( RootDir == NULL )	return FALSE;
//
//	BOOL bRval = FALSE; 
//	int nRval = 0; 
//
//	CString szNextDirPath   = _T(""); 
//	CString szRoot = _T("");
//
//	// �ش� ���丮�� ��� ������ �˻��Ѵ�.
//	szRoot.Format(_T("%s\\*.*"), RootDir);
//
//	CFileFind find; 
//
//	bRval = find.FindFile( szRoot );
//
//	if( bRval == FALSE )	return bRval;
//
//	while( bRval )  
//	{  
//		bRval = find.FindNextFile();
//
//		// . or .. �� ��� ���� �Ѵ�.  
//		if( find.IsDots() == FALSE )   
//		{   
//			// Directory �� ��� ���ȣ�� �Ѵ�.
//			if( find.IsDirectory() )   
//			{   
//				DeleteDirectoryFile(find.GetFilePath());   
//			}   
//			// file�� ��� ���� 
//			else
//			{    
//				bRval = DeleteFile(find.GetFilePath()); 
//			}  
//		}
//	} 
//	find.Close();
//	bRval = RemoveDirectory( RootDir );   
//
//	return bRval;
//}
// 2016.04.07 Defect Moni ROI ��� ����
void CSystem::SetUseDefectMoniROI(BOOL bUseMoni)
{
	m_bUseDefectMoni = bUseMoni;
}

BOOL CSystem::GetUseDefectMoniROI()
{
	return m_bUseDefectMoni;
}

// 2015.08.06 Defect Filter ROI ��� ���� - UI �� Mode OG Param ����
void CSystem::SetUseDefectFilterROI(BOOL bUseFilter)
{
	m_bUseDefectFilter = bUseFilter;
}

BOOL CSystem::GetUseDefectFilterROI()
{
	return m_bUseDefectFilter;
}

// 2015.09.24 cpp ���� include �� ������ ���� �߻� - System �� ���� �������� ���� - by CWH
void CSystem::CallCalculate_OffSetRef(MIL_ID milDrawBuffer, int nPosition, bool bClockWise, tInfoAlign InfoAlign, int nThreadID)
{
	InspectAlgorithmInterface Algorithminterface;
	Algorithminterface.Calculate_OffSetRef(milDrawBuffer,  nPosition, bClockWise, InfoAlign, nThreadID);
}


// For PLC Task  
//unsigned __stdcall CSystem::m_fnThreadVsMsgReceiver(LPVOID pArg)
//{
//	CSystem *pSystem = (CSystem*)pArg;
//
//	while(1)
//	{
//		BYTE* bpPeekMsg = NULL;
//
//		if (pSystem->m_pVsInterServerIF == NULL)
//		{
//			Sleep(1000);
//
//			TCHAR szPlcLogPath[100],szPlcFileName[100], szServerIP[100];
//			int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;
//			CString szVsIniFilePath;
//			szVsIniFilePath.Format(_T("%s"), VS_CRUX_CLIENT_TASK_INI_FILE);
//
//			nTaskNo				= GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 50, szVsIniFilePath);
//			nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, szVsIniFilePath);
//			uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, szVsIniFilePath);
//			uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, szVsIniFilePath);
//			uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, szVsIniFilePath);
//
//			nTaskNo += pSystem->m_Config.m_nInspactEqpNo - 1;
//
//			GetPrivateProfileString(_T("LogInfo"), _T("Path") ,_T("D:\\Crux_Data\\LOG_VS"), szPlcLogPath, sizeof(szPlcLogPath), szVsIniFilePath);
//			GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("VS_CRUX"), szPlcFileName, sizeof(szPlcFileName), szVsIniFilePath);
//			GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), szVsIniFilePath);
//
//			pSystem->m_pVsInterServerIF = new CInterServerInterface(szPlcLogPath, szPlcFileName, NULL, NULL, false, uPeriodDay, uWriteTerm, true, uWriteLogLevel);
//			int nRet = pSystem->m_pVsInterServerIF->m_fnRegisterTask(nTaskNo, szServerIP, nServerPort);
//			if (nRet != APP_OK)
//			{
//				SAFE_DELETE(pSystem->m_pVsInterServerIF);
//			}
//
//			SAFE_DELETE(pSystem->m_pVsMessage);
//			pSystem->m_pVsMessage = new VsServerInterface(pSystem->m_pVsInterServerIF, pSystem);
//		}
//		else
//		{
//			if (pSystem->m_pVsInterServerIF)
//			{
//				bpPeekMsg = pSystem->m_pVsInterServerIF->m_fnPeekMessage();
//
//				if (bpPeekMsg)
//				{
//					// �޼��� ó���� �ʿ�.
//					pSystem->m_pVsInterServerIF->m_fnFreeMemory(bpPeekMsg);
//				}
//			}
//
//		}
//	}
//}

void CSystem::InitGrabCountforCacheMem()
{
	m_nInitMemGrabAccumCount = 0;
}

int CSystem::AccumulateIGrabCountforCacheMem()
{
	return ++m_nInitMemGrabAccumCount;
}

void CSystem::SendContollerError(BOOL bShow, CString strMsg)
{
	::SendMessage(((CMainFrame*)AfxGetApp()->GetMainWnd())->m_hWnd, UM_CONTROLLER_ERROR, (WPARAM)bShow, (LPARAM)(LPCTSTR)strMsg);
}

int CSystem::GetLineFittingTh()
{
	return m_LineFittingTh;
}

void CSystem::SetLineFittingTh(int Value)
{
	m_LineFittingTh = Value;
}
//2021.06.07 add by mby
int CSystem::GetTTIndfo()
{
	return m_TTtime;
}

void CSystem::SetTTIndfo(int Value)
{
	m_TTtime = Value;
}
