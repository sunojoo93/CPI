
// VSCameraTask.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"

#include "VSCameraTask.h"

#include <conio.h>
#ifdef _MATROXCAMERA
#include "MatroxCamera/MatroxCamera.h"
#include "VSMatroxCameraTaskDlg.h"
#elif _SIMULATIONCAMERA
#include "SimulationCamera/SimulationCamera.h"
#include "VSSimulationCameraTaskDlg.h"
#elif _VISTEKCAMERA
#include "VistekCamera/VistekCamera.h"
#include "VSVistekCameraTaskDlg.h"
#elif _IMICAMERA
#include "IMICamera/IMICamera.h"
#include "VSIMICameraTaskDlg.h"
#elif _DALSACAMERA
#include "DalsaCamera/DalsaCamera.h"
#include "VSDalsaCameraTaskDlg.h"
#elif _HIKCAMERA
#include "HikCamera/HikCamera.h"
#include "VSHikCameraTaskDlg.h"
#elif _MATROXLINECAMERA
#include "MatroxLineCamera/MatroxLineCamera.h"
#include "VSMatroxLineCameraTaskDlg.h"
#elif _NEDLINECAMERA
#include "NedLineCamera/NedLineCamera.h"
#include "VSNedLineCameraTaskDlg.h"
#elif _DALSALINECAMERA
#include "DalsaLineCamera/DalsaLineCamera.h"
#include "VSDalsaLineCameraTaskDlg.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSCameraTaskApp

BEGIN_MESSAGE_MAP(CVSCameraTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSCameraTaskApp 생성

CVSCameraTaskApp::CVSCameraTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_bCamState(FALSE),
	m_bSMemState(FALSE),
	m_pInternalServer(NULL)
{
	// 다시 시작 관리자 지원
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;	// 20.12.29 yskim comment

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CVSCameraTaskApp::~CVSCameraTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// 유일한 CVSCameraTaskApp 개체입니다.

CVSCameraTaskApp theApp;


// CVSCameraTaskApp 초기화

BOOL CVSCameraTaskApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	

	// Command Line 명령어 받아오기
	int iCnt = 0;
	LPWSTR* argv = NULL;
	argv = CommandLineToArgvW(GetCommandLine(), &iCnt);

	if (argv == NULL || iCnt < 3)	
	{
		AfxMessageBox(_T("Please Check Command Line Argument !!"));
		return 1;
	}

	CString strRegKey = _T("");
	CString strIniPath = _T("");
	
	// [파일명].exe [TaskNo] [Ini Path] 로 실행해야 함.
	// 예시 : SimulationCamera.exe 1 SimulationCamera1.ini
	m_nTaskNo = _ttoi(argv[1]);
	strIniPath = argv[2];
	
	if (strIniPath.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("Please Check INI File Path !!"));
		return 1;
	}

	// Log 객체 생성

	///////////// 카메라가 변경될 경우 수정해야 하는 부분 /////////////////////////////
	// CameraInterface 상속하는 새로운 Class 생성 후 Interface 함수에 맞는 기능 구현 //
#ifdef _MATROXCAMERA
	m_pCamera = new CMatroxCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
	m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);
#elif _SIMULATIONCAMERA
	m_pCamera = new CSimulationCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
#elif _VISTEKCAMERA
	m_pCamera = new CVistekCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	if(GetTaskNum() == 1)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Coaxial"), 7, 0, true);
	else if(GetTaskNum() == 2)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Side"), 7, 0, true);
	else
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Debug"), 7, 0, true);
#elif _IMICAMERA
	m_pCamera = new CIMICamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	if (GetTaskNum() == 1)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Coaxial"), 7, 0, true);
	else if (GetTaskNum() == 2)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Side"), 7, 0, true);
	else
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Debug"), 7, 0, true);
#elif _DALSACAMERA
	m_pCamera = new CDalsaCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
	m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);
#elif _HIKCAMERA
	m_pCamera = new CHikCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	if (GetTaskNum() == 1)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Coaxial"), 7, 0, true);
	else if (GetTaskNum() == 2)
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Side"), 7, 0, true);
	else
		m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("CameraTact_Debug"), 7, 0, true);
#elif _MATROXLINECAMERA
	m_pCamera = new CMatroxLineCamera();
	m_pTrigger = new CTrigger();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
	m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);
#elif _NEDLINECAMERA
	m_pCamera = new CNedLineCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
	m_pLogLineScan = new CLogWriter((TSTRING)CAM_LINESCAN_LOG_PATH, _T("LineScan"), 7, 0, true);
	m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);
#elif _DALSALINECAMERA
	m_pCamera = new CDalsaLineCamera();
	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);
	m_pLogWriter = new CLogWriter((TSTRING)CAM_TACT_LOG_PATH, _T("Tact"), 7, 0, true);
	m_pLogLineScan = new CLogWriter((TSTRING)CAM_LINESCAN_LOG_PATH, _T("LineScan"), 7, 0, true);
	m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);
#else
	m_pCamera = new CameraInterface();
#endif
	
	////////////////////////////////////////////////////////////////////////////

	m_bManual = FALSE;	

	int nGrabWaitTimeForSimulation;
	nGrabWaitTimeForSimulation		= GetPrivateProfileInt(_T("Settings"), _T("GrabWaitTimeForSimulation"), 30, m_strInitFilePath);
	TCHAR szSimulationFlag[6] = {0,};        //2017.01.02 sgkim H-Project
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		SetSimulationMode(TRUE);
	else
		SetSimulationMode(FALSE);


	// Visual Station 연결
	m_bExec = TRUE;
	m_hEventThreadAlive = NULL;
	m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventThreadAlive);
	m_bCamState = m_pCamera->InitializeCamera(m_strInitFilePath);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
	
		
	// Shared Memory 연결
	int nImageCount = /*GetPrivateProfileInt(_T("Settings"), _T("ImageCount"), 30, m_strInitFilePath)*/20;
	m_nPcNum = GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH);
	CString strDrv_CamNo = _T("");
	strDrv_CamNo.Format(_T("%s_%d"), theApp.GETDRV(), GetTaskNum());

	int nImageSizeX = m_pCamera->GetImageWidth();
#ifdef _NEDLINECAMERA
	int lineCount = ((CNedLineCamera*)m_pCamera)->TriggerCountF > ((CNedLineCamera*)m_pCamera)->TriggerCountB ? ((CNedLineCamera*)m_pCamera)->TriggerCountF : ((CNedLineCamera*)m_pCamera)->TriggerCountB;
	int nImageSizeY = m_pCamera->GetImageHeight() * (lineCount - 1);	// 라인 스캔 맥스값으로...
#elif _DALSALINECAMERA
	int lineCount   = ((CDalsaLineCamera*)m_pCamera)->TriggerCountF > ((CDalsaLineCamera*)m_pCamera)->TriggerCountB ? ((CDalsaLineCamera*)m_pCamera)->TriggerCountF : ((CDalsaLineCamera*)m_pCamera)->TriggerCountB;
	int imageY = ((CDalsaLineCamera*)m_pCamera)->imageY;// 라인 스캔 맥스값으로...
	//int imageY      = 0;
	int nImageSizeY = m_pCamera->GetImageHeight() * (imageY);

#else
	int nImageSizeY = m_pCamera->GetImageHeight();
#endif
	int nImageBitrate = m_pCamera->GetImageBitrate();
	int nImageBandWidth = m_pCamera->GetImageBandwidth();
	m_pSharedMemory = new CSharedMemWrapper(nImageSizeX, nImageSizeY, nImageBitrate, nImageBandWidth, strDrv_CamNo, nImageCount);

	
	strRegKey.Format(_T("YWDSP Camera Task%d"), GetTaskNum());


	LocalFree(argv);

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.	
	SetRegistryKey(strRegKey);

	CVSCameraTaskDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)

	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	// IPC 종료 대기
	m_bExec = FALSE;
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(m_pSharedMemory);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pLogWriter);
	SAFE_DELETE(m_pLogLineScan);
	SAFE_DELETE(m_pLogMonitoring);

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

UINT CVSCameraTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSCameraTaskApp* pThis = (CVSCameraTaskApp*)pParam;

	// Visual Station 연결
	int					nRet;
	VSMessageProcessor*	pInternalServer = NULL;

	//Internal Server 생성.. 부가적인 모든 Class 들이 생성 된다.
	pThis->m_pInternalServer = NULL;
	pThis->m_pInternalServer = new VSMessageProcessor;
	pInternalServer = pThis->m_pInternalServer;

	//Internal Server 시작
	do 
	{
		nRet = pThis->m_pInternalServer->m_fnStart(pThis->m_strInitFilePath, pThis->GetTaskNum());
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);
	
	printf("Internal Camera Start \n");

	// VS 상태 체크
	while(pThis->m_bExec)
	{
		if (pThis->m_pInternalServer != NULL)
			pThis->SetIPCState(pThis->m_pInternalServer->GetVSState());
		Sleep(1000);
	}
	
	pThis->SetIPCState(FALSE);
	//SAFE_DELETE(pInternalServer);

	SetEvent(pThis->m_hEventThreadAlive);
	printf("Internal Camera End \n");

	return 0;
}

// 로그 기록 추가
void CVSCameraTaskApp::m_fnWriteTactLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogWriter->m_fnWriteLog(cBuffer);
}

void CVSCameraTaskApp::m_fnWriteLineScanLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = { 0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogLineScan->m_fnWriteLog(cBuffer);
}

// 모니터링 로그 기록 추가
void CVSCameraTaskApp::m_fnWriteMonitoringLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogMonitoring->m_fnWriteLog(cBuffer);
}