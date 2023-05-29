
// VSCameraTask.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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


// CVSCameraTaskApp ����

CVSCameraTaskApp::CVSCameraTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_bCamState(FALSE),
	m_bSMemState(FALSE),
	m_pInternalServer(NULL)
{
	// �ٽ� ���� ������ ����
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;	// 20.12.29 yskim comment

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CVSCameraTaskApp::~CVSCameraTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// ������ CVSCameraTaskApp ��ü�Դϴ�.

CVSCameraTaskApp theApp;


// CVSCameraTaskApp �ʱ�ȭ

BOOL CVSCameraTaskApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	

	// Command Line ��ɾ� �޾ƿ���
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
	
	// [���ϸ�].exe [TaskNo] [Ini Path] �� �����ؾ� ��.
	// ���� : SimulationCamera.exe 1 SimulationCamera1.ini
	m_nTaskNo = _ttoi(argv[1]);
	strIniPath = argv[2];
	
	if (strIniPath.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("Please Check INI File Path !!"));
		return 1;
	}

	// Log ��ü ����

	///////////// ī�޶� ����� ��� �����ؾ� �ϴ� �κ� /////////////////////////////
	// CameraInterface ����ϴ� ���ο� Class ���� �� Interface �Լ��� �´� ��� ���� //
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


	// Visual Station ����
	m_bExec = TRUE;
	m_hEventThreadAlive = NULL;
	m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventThreadAlive);
	m_bCamState = m_pCamera->InitializeCamera(m_strInitFilePath);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
	
		
	// Shared Memory ����
	int nImageCount = /*GetPrivateProfileInt(_T("Settings"), _T("ImageCount"), 30, m_strInitFilePath)*/20;
	m_nPcNum = GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH);
	CString strDrv_CamNo = _T("");
	strDrv_CamNo.Format(_T("%s_%d"), theApp.GETDRV(), GetTaskNum());

	int nImageSizeX = m_pCamera->GetImageWidth();
#ifdef _NEDLINECAMERA
	int lineCount = ((CNedLineCamera*)m_pCamera)->TriggerCountF > ((CNedLineCamera*)m_pCamera)->TriggerCountB ? ((CNedLineCamera*)m_pCamera)->TriggerCountF : ((CNedLineCamera*)m_pCamera)->TriggerCountB;
	int nImageSizeY = m_pCamera->GetImageHeight() * (lineCount - 1);	// ���� ��ĵ �ƽ�������...
#elif _DALSALINECAMERA
	int lineCount   = ((CDalsaLineCamera*)m_pCamera)->TriggerCountF > ((CDalsaLineCamera*)m_pCamera)->TriggerCountB ? ((CDalsaLineCamera*)m_pCamera)->TriggerCountF : ((CDalsaLineCamera*)m_pCamera)->TriggerCountB;
	int imageY = ((CDalsaLineCamera*)m_pCamera)->imageY;// ���� ��ĵ �ƽ�������...
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

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.	
	SetRegistryKey(strRegKey);

	CVSCameraTaskDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)

	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// IPC ���� ���
	m_bExec = FALSE;
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(m_pSharedMemory);
	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pLogWriter);
	SAFE_DELETE(m_pLogLineScan);
	SAFE_DELETE(m_pLogMonitoring);

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

UINT CVSCameraTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSCameraTaskApp* pThis = (CVSCameraTaskApp*)pParam;

	// Visual Station ����
	int					nRet;
	VSMessageProcessor*	pInternalServer = NULL;

	//Internal Server ����.. �ΰ����� ��� Class ���� ���� �ȴ�.
	pThis->m_pInternalServer = NULL;
	pThis->m_pInternalServer = new VSMessageProcessor;
	pInternalServer = pThis->m_pInternalServer;

	//Internal Server ����
	do 
	{
		nRet = pThis->m_pInternalServer->m_fnStart(pThis->m_strInitFilePath, pThis->GetTaskNum());
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);
	
	printf("Internal Camera Start \n");

	// VS ���� üũ
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

// �α� ��� �߰�
void CVSCameraTaskApp::m_fnWriteTactLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogWriter->m_fnWriteLog(cBuffer);
}

void CVSCameraTaskApp::m_fnWriteLineScanLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = { 0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogLineScan->m_fnWriteLog(cBuffer);
}

// ����͸� �α� ��� �߰�
void CVSCameraTaskApp::m_fnWriteMonitoringLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogMonitoring->m_fnWriteLog(cBuffer);
}