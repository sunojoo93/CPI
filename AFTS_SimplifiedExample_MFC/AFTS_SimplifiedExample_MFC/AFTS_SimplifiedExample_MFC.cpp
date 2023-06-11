
// AFTS_SimplifiedExample_MFC.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "AFTS_SimplifiedExample_MFC.h"
#include "AFTS_SimplifiedExample_MFCDlg.h"
#include "CMainDlg.h"
#include "AFWorkManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAFTSSimplifiedExampleMFCApp

BEGIN_MESSAGE_MAP(CAFTSSimplifiedExampleMFCApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAFTSSimplifiedExampleMFCApp 생성

CAFTSSimplifiedExampleMFCApp::CAFTSSimplifiedExampleMFCApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CAFTSSimplifiedExampleMFCApp 개체입니다.

CAFTSSimplifiedExampleMFCApp theApp;


// CAFTSSimplifiedExampleMFCApp 초기화

BOOL CAFTSSimplifiedExampleMFCApp::InitInstance()
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

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

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

	/*if (m_nTaskNo == 1) {
		m_strLog_AF_Position = _T("상면");
	}
	else {
		m_strLog_AF_Position = _T("후면");
	}*/

	m_strLog_AF_Position = _T("");

	if (strIniPath.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("Please Check INI File Path !!"));
		return 1;
	}

	m_strInitFilePath.Format(theApp.GETDRV() + _T(":\\CRUX\\DATA\\INI\\%s"), strIniPath);

	TCHAR strTemp[256];
	GetPrivateProfileString(_T("AFModule"), _T("ModuleIP"), _T("T"), strTemp, 20, m_strInitFilePath);
	
	CString adress_Temp = _T("");

	for (int i = 0; i < 4; i++) {

		AfxExtractSubString(adress_Temp, strTemp, i, '.');
		m_Module_IPadress[i] = _ttoi(adress_Temp);
		
	}


	m_pLogWriter = new CLogWriter((TSTRING)AF_LOG_PATH, _T("AFLog"), 7, 0, true);
	
	
	//m_pLogMonitoring = new CLogWriter((TSTRING)CAM_MONITORING_LOG_PATH, _T("Monitoring"), 7, 0, true);


	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
		// Visual Station 연결
	
	m_bExec = TRUE;
	m_hEventThreadAlive = NULL;
	m_hEventThreadAlive = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
	
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	LocalFree(argv);

	//CAFTSSimplifiedExampleMFCDlg dlg;
	CMainDlg Mdlg;

	m_pMainWnd = &Mdlg;
	INT_PTR nResponse = Mdlg.DoModal();
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
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}


	// IPC 종료 대기
	m_bExec = FALSE;
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	SAFE_DELETE(m_pLogWriter);

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

UINT CAFTSSimplifiedExampleMFCApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CAFTSSimplifiedExampleMFCApp* pThis = (CAFTSSimplifiedExampleMFCApp*)pParam;

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
	while (pThis->m_bExec)
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

