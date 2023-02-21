
// VSPGControllerTask.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "VSPGControllerTask.h"

#include <conio.h>

#ifdef _PGController_EEC_C302B
#include "PGController_EEC_C302B/PGController_EEC_C302B.h"
#elif _PGController_FECP721
#include "PGController_FECP721/PGController_FECP721.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSPGControllerTaskApp

BEGIN_MESSAGE_MAP(CVSPGControllerTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSPGControllerTaskApp ����

CVSPGControllerTaskApp::CVSPGControllerTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_pInternalServer(NULL)
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_bSimulationMode = FALSE;
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CVSPGControllerTaskApp::~CVSPGControllerTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// ������ CVSPGControllerTaskApp ��ü�Դϴ�.

CVSPGControllerTaskApp theApp;


// CVSPGControllerTaskApp �ʱ�ȭ

BOOL CVSPGControllerTaskApp::InitInstance()
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
	//m_pLogWriter = new CLogWriter(LIGHT_LOG_PATH, _T("Tact"), 7, 0, true);

	// Command Line ��ɾ� �޾ƿ���
	int iCnt = 0;
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW(GetCommandLine(), &iCnt);

	CString strRegKey;

	///////////// ī�޶� ����� ��� �����ؾ� �ϴ� �κ� /////////////////////////////
	// CPGControllerInterface ����ϴ� ���ο� Class ���� �� Interface �Լ��� �´� ��� ���� //
#ifdef _PGController_EEC_C302B
	m_pPgController = new CPGController_EEC_C302B();
#elif _PGController_FECP721
	m_pPgController = new CPGController_FECP721();
#else
	m_pPgController = new CPGControllerInterface();
#endif
	////////////////////////////////////////////////////////////////////////////

// 	if (iCnt > 1)		// for test
// 	{
		m_bManual = FALSE;

		// for test
		//CString strInitFilePath;
#ifdef _PGController_EEC_C302B
		m_strInitFilePath.Format(_T("%s"), theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\PGController.ini"));
#elif _PGController_FECP721
		m_strInitFilePath.Format(_T("%s"), theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\PGController.ini"));
#endif

		// Visual Station ����
		m_bExec = TRUE;
		m_hEventThreadAlive = NULL;
		m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
		ResetEvent(m_hEventThreadAlive);
		m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
		

		TCHAR szSimulationFlag[6] = {0,};        //2017.01.02 sgkim H-Project
		GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
		if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
			SetSimulationMode(TRUE);
		else
			SetSimulationMode(FALSE);

		//m_nLightNo = GetPrivateProfileInt(_T("Settings"), _T("Light Number"), 30, m_strInitFilePath);

		//strRegKey.Format(_T("YWDSP Light Task%d"), m_nLightNo);
// 	}
// 	else
// 	{
// 		m_bManual = TRUE;
// 
// 		strRegKey = _T("YWDSP Light Task");
// 	}

	LocalFree(pStr);

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.	
	//SetRegistryKey(strRegKey);
	SetRegistryKey(strRegKey);

	//Command Prompt Quick Edit Mode Disable
	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwPreviousMode = 0;
	GetConsoleMode(hConsole, &dwPreviousMode);
	DWORD dwNewMode = dwPreviousMode | ENABLE_MOUSE_INPUT;
	//SetConsoleMode(hConsole, dwNewMode);
	dwNewMode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(hConsole, dwNewMode);

	// IPC ���� ���
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	m_bExec = FALSE;
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(m_pPgController);
//	SAFE_DELETE(m_pLogWriter);

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

UINT CVSPGControllerTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSPGControllerTaskApp* pThis = (CVSPGControllerTaskApp*)pParam;

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
		nRet = pThis->m_pInternalServer->m_fnStart(pThis->m_strInitFilePath);
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);

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

	return 0;
}

// �α� ��� �߰�
void CVSPGControllerTaskApp::m_fnWriteLog(const TCHAR* pszLog, ...)
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

	//m_pLogWriter->m_fnWriteLog(cBuffer);
}
	

ST_PG_DATA CVSPGControllerTaskApp::GetCurPatternVoltageData(int nPatternNo/*TCHAR* strCurStepName*/)	
{		
	if(nPatternNo < 0 || nPatternNo >= MAX_GRAB_STEP_COUNT) 
	{
		memset(&m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1], NULL, sizeof(m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1]));	
		return m_stPgInfo.stPgData[0];		
	}
	return  m_stPgInfo.stPgData[GetCurPgDataIndex(nPatternNo)];

// 	for(int i = 0 ; i < MAX_GRAB_STEP_COUNT  ; i++)
// 	{
// 		if(!_tcsicmp(m_stPgInfo.stPgData[i].strPatternName, strCurStepName))
// 			return m_stPgInfo.stPgData[i];	
// 	}
// 	if(nPatternNo >= MAX_GRAB_STEP_COUNT) 
// 	{
// 		memset(&m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1], NULL, sizeof(m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1]));	 
// 		return m_stPgInfo.stPgData[0];		
// 	}
// 	return m_stPgInfo.stPgData[nPatternNo];	
	

// 	memset(&m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1], NULL, sizeof(m_stPgInfo.stPgData[MAX_GRAB_STEP_COUNT - 1]));	 
// 	return m_stPgInfo.stPgData[0];
};