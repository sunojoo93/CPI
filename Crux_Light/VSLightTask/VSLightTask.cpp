
// VSLightTask.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "VSLightTask.h"

#include <conio.h>

// #ifdef _NEEPSLIGHT
// #include "NeepsLight/NeepsLightControl.h"
// #include "NeepsLightTaskDlg.h"
// #elif _PLUSTEK
// #include "Plustek/PlustekLightControl.h"
// #include "PlustekLightTaskDlg.h"
// #elif _SUNINLIGHT
// #include "SuninLight/SuninLightControl.h"
// #include "SuninLightTaskDlg.h"
// #elif _EROOMLIGHT
// #include "EroomLight/EroomLightControl.h"
// #include "EroomLightTaskDlg.h"
// #elif _LFINELIGHT
// #include "LFineLight/LFineLightControl.h"
// #include "LFineLightTaskDlg.h"
// #elif _LLIGHTLIGHT
// #include "LLightLight/LLightLightControl.h"
// #include "LLightLightTaskDlg.h"
// #else
// #include "VSLightTaskDlg.h"
// #endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSLightTaskApp

BEGIN_MESSAGE_MAP(CVSLightTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSLightTaskApp ����

CVSLightTaskApp::CVSLightTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_bLightState(FALSE),
	m_bSMemState(FALSE),
	m_pInternalServer(NULL)
{
	// �ٽ� ���� ������ ����
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CVSLightTaskApp::~CVSLightTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// ������ CVSLightTaskApp ��ü�Դϴ�.

CVSLightTaskApp theApp;


// CVSLightTaskApp �ʱ�ȭ

BOOL CVSLightTaskApp::InitInstance()
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

	// Log ��ü ����

	
	// Command Line ��ɾ� �޾ƿ���
	int iCnt = 0;
	LPWSTR* argv = NULL;
	argv = CommandLineToArgvW(GetCommandLine(), &iCnt);

	// �������� �̸����� ���� �̸� �޾ƿ���
	TCHAR szFilename[MAX_PATH + 1] = {0};
	GetModuleFileName(NULL, szFilename, MAX_PATH);
	CString strLightName = PathFindFileName(szFilename);
	strLightName.Replace(_T("Task.exe"), _T(""));

	CString strRegKey = _T("");
	CString strIniPath = _T("");

	if (argv == NULL || iCnt < 3)
	{
#if _EROOMLIGHT
		AfxMessageBox(_T("Please Check Command Line Argument !!"));
		return 1;
#else
		if (iCnt == 2)	// ���� �� [INI���ϰ��] �� ���� ���� ��� Task No �� �Է� ������ ����
			m_nTaskNum = _ttoi(argv[1]);
		else	// ���� �� [TaskNo] [INI���ϰ��] ��� ���� ���� ��� Default ������ Task No �� 1�� ����
			m_nTaskNum = 1;
		// INI ���� ��δ� [�����̸�].ini ���� ��� �ڵ� ����
		strIniPath.Format(_T("%s.ini"), strLightName);
#endif		
	}
	else
	{
		// Eroom ��Ʈ�ѷ�(6ch, 24ch)�� ���� ��� �μ��� ���ϰ�� ���� 
		// [���ϸ�].exe [TaskNo] [Ini Path] �� �����ؾ� ��.
		// ���� : EroomLightTask.exe 1 EroomLight_6ch.ini
		// ���� : EroomLightTask.exe 2 EroomLight_24ch.ini
		// �ʿ� �� argv[1] �� Task No �� Ȱ�� -> 18.05.30 Ȱ�� �ϵ��� ����
		m_nTaskNum = _ttoi(argv[1]);
		strIniPath = argv[2];
	}

	if (strIniPath.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("Please Check INI File Path !!"));
		return 1;
	}

	///////////// ī�޶� ����� ��� �����ؾ� �ϴ� �κ� /////////////////////////////
	// LightInterface ����ϴ� ���ο� Class ���� �� Interface �Լ��� �´� ��� ���� //
#ifdef _NEEPSLIGHT
	m_pLight = new CNeepsLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("NeepsLightCheck"), 30, 0, true);
#elif _PLUSTEK
	m_pLight = new CPlustekLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("PlustekLightCheck"), 30, 0, true);
#elif _SUNINLIGHT
	m_pLight = new CSuninLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("SuninLightCheck"), 30, 0, true);
#elif _EROOMLIGHT	// Eroom Light �� ���� ���� ����
	m_pLight = new CEroomLight();
//	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("EroomLightCheck"), 30, 0, true);
#elif _LFINELIGHT
	m_pLight = new CLFineLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("LFineLightCheck"), 30, 0, true);
#elif _IPULSLIGHT
	m_pLight = new CIPulsLight();
	// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("LLightLightCheck"), 30, 0, true);
#elif _LLIGHTLIGHT
	m_pLight = new CLLightLight();
	// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("LLightLightCheck"), 30, 0, true);
#elif _TRIVISIONLIGHT
	m_pLight = new CTrivisionLight();
	// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("TrivisionLightCheck"), 30, 0, true);
#else
	m_pLight = new LightInterface();
#endif
	CString strLogFileName = _T("");
	strLogFileName.Format(_T("%sCheck%d"), strLightName, theApp.GetVsTaskNum());
	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, strLogFileName.GetBuffer(0), 30, 0, true);
	////////////////////////////////////////////////////////////////////////////

// 	if (iCnt > 1)		// for test
// 	{
		m_bManual = FALSE;
		m_pLogWriter->m_fnWriteLog(_T("Light task Start"));
		m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);

// INI File ��� �������� ��� �ڱ� �ڽ��� �������� �̸����� Task.exe �� ������ ������ �κ����� INI ���� ��� �����ϵ��� ����
// �� : PlustekLightTask.exe -> PlustekLight.ini
// �������� �ʾ��� ��� �������� argv[2] �� INI ���� ��η� ���
// #ifdef _NEEPSLIGHT
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\NeepsLight.ini"));		
// #elif _PLUSTEK
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\PlustekLight.ini"));
// #elif _SUNINLIGHT
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\SuninLight.ini"));
// #elif _EROOMLIGHT
// 		m_strInitFilePath.Format(_T("D:\\CRUX\\DATA\\INI\\%s"), strIniPath);
// #elif _LFINELIGHT
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\LFineLight.ini"));
// #elif _LLIGHTLIGHT
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\LLightLight.ini"));
// #else
// 		m_strInitFilePath.Format(_T("%s"), _T("D:\\CRUX\\DATA\\INI\\Light.ini"));	
// #endif

		m_pDlg = new CVSLightTaskDlg();

		SetPCNum((GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH)));
		SetSiteTypeNum( (GetPrivateProfileInt(_T("Common"), _T("SITE TYPE"), 0, INIT_FILE_PATH)) );
		SetInspType((GetPrivateProfileInt(_T("Common"), _T("TYPE"), 1, INIT_FILE_PATH))); // �˻� Ÿ�� �޾ƿ��� 
		int n = GetPCNum();
		// Visual Station ����
		m_bExec = TRUE;
		m_hEventThreadAlive = NULL;
		m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
		ResetEvent(m_hEventThreadAlive);
		m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
		
		m_bLightState = m_pLight->InitializeLight(m_strInitFilePath);

		//m_nLightNo = GetPrivateProfileInt(_T("Settings"), _T("Light Number"), 30, m_strInitFilePath);

		//strRegKey.Format(_T("YWDSP Light Task%d"), m_nLightNo);
// 	}
// 	else
// 	{
// 		m_bManual = TRUE;
// 
// 		strRegKey = _T("YWDSP Light Task");
// 	}
		if (m_bLightState)
		{
			LocalFree(argv);

			// ǥ�� �ʱ�ȭ
			// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
			// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
			// ��ƾ�� �����ؾ� �մϴ�.
			// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
			// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
			// ������ �������� �����ؾ� �մϴ�.	
			//SetRegistryKey(strRegKey);

			//CVSLightTaskDlg dlg;
			//m_pMainWnd = &dlg;
			//m_pDlg = new CVSLightTaskDlg();
			INT_PTR nResponse = m_pDlg->DoModal();

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
		m_pLogWriter->m_fnWriteLog(_T("Light task EXIT"));
		SAFE_DELETE(m_pLight);
		SAFE_DELETE(m_pLogWriter);
		SAFE_DELETE(m_pDlg);
	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

UINT CVSLightTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSLightTaskApp* pThis = (CVSLightTaskApp*)pParam;

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
	
	printf("Internal Light Start \n");

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
	printf("Internal Light End \n");

	return 0;
}

// �α� ��� �߰�
void CVSLightTaskApp::m_fnWriteLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	if (m_pInternalServer)
	{
		m_pInternalServer->m_fnPrintLog(cBuffer);
	}
}

// �α� �Լ� Alg �׽�ũ�� ���� ������� ����	180417 YSS
// bTraceList	: Light Task UI�� �α� ��� ����
// pszLog		: �α� �ؽ�Ʈ
void CVSLightTaskApp::WriteLog(const TCHAR* pszLog, ...)
{
	//TCHAR cBuffer[1024] = {0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	TCHAR* cBuffer = NULL;
	TCHAR* cBufferTemp = NULL;		// for UI
	int len = 0;
	int nBufSize = 0;

	if (pszLog != NULL)
	{
		len = _vscwprintf( pszLog, vaList ) + 1;
		nBufSize = len * sizeof(TCHAR);

		cBuffer = new TCHAR[nBufSize];		
		memset(cBuffer,0,nBufSize);

		if (cBuffer)
			vswprintf(cBuffer, pszLog, (va_list)vaList);
	}
	va_end(vaList);

	// Log ���Ͽ� ���
	if (m_pInternalServer)
	{
		m_pInternalServer->m_fnPrintLog(cBuffer);
	}

	// Light Task UI�� Trace �α� ǥ��
	//if(bTraceList)
	//{
		if(m_pDlg != NULL)
		{
			cBufferTemp	= new TCHAR[nBufSize];
			memset(cBufferTemp,0,nBufSize);
			memcpy(cBufferTemp, cBuffer, nBufSize);
			::PostMessage(m_pDlg->m_hWnd, WM_PRINT_UI_LOG_MSG_UNICODE, nBufSize, (LPARAM)cBufferTemp);	// Print Log GUI
		}
	//}
}