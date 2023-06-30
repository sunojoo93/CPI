
// VSLightTask.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CVSLightTaskApp 생성

CVSLightTaskApp::CVSLightTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_bLightState(FALSE),
	m_bSMemState(FALSE),
	m_pInternalServer(NULL)
{
	// 다시 시작 관리자 지원
	//m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CVSLightTaskApp::~CVSLightTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// 유일한 CVSLightTaskApp 개체입니다.

CVSLightTaskApp theApp;


// CVSLightTaskApp 초기화

BOOL CVSLightTaskApp::InitInstance()
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

	// Log 객체 생성

	
	// Command Line 명령어 받아오기
	int iCnt = 0;
	LPWSTR* argv = NULL;
	argv = CommandLineToArgvW(GetCommandLine(), &iCnt);

	// 실행파일 이름에서 조명 이름 받아오기
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
		if (iCnt == 2)	// 실행 시 [INI파일경로] 만 생략 했을 경우 Task No 는 입력 값으로 설정
			m_nTaskNum = _ttoi(argv[1]);
		else	// 실행 시 [TaskNo] [INI파일경로] 모두 생략 했을 경우 Default 값으로 Task No 는 1로 설정
			m_nTaskNum = 1;
		// INI 파일 경로는 [조명이름].ini 파일 경로 자동 생성
		strIniPath.Format(_T("%s.ini"), strLightName);
#endif		
	}
	else
	{
		// Eroom 컨트롤러(6ch, 24ch)에 따라서 명령 인수로 파일경로 설정 
		// [파일명].exe [TaskNo] [Ini Path] 로 실행해야 함.
		// 예시 : EroomLightTask.exe 1 EroomLight_6ch.ini
		// 예시 : EroomLightTask.exe 2 EroomLight_24ch.ini
		// 필요 시 argv[1] 은 Task No 로 활용 -> 18.05.30 활용 하도록 변경
		m_nTaskNum = _ttoi(argv[1]);
		strIniPath = argv[2];
	}

	if (strIniPath.Compare(_T("")) == 0)
	{
		AfxMessageBox(_T("Please Check INI File Path !!"));
		return 1;
	}

	///////////// 카메라가 변경될 경우 수정해야 하는 부분 /////////////////////////////
	// LightInterface 상속하는 새로운 Class 생성 후 Interface 함수에 맞는 기능 구현 //
#ifdef _NEEPSLIGHT
	m_pLight = new CNeepsLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("NeepsLightCheck"), 30, 0, true);
#elif _PLUSTEK
	m_pLight = new CPlustekLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("PlustekLightCheck"), 30, 0, true);
#elif _SUNINLIGHT
	m_pLight = new CSuninLight(); 
// 	m_pLogWriter = new CLogWriter((TSTRING)LIGHT_MONITORING_LOG_PATH, _T("SuninLightCheck"), 30, 0, true);
#elif _EROOMLIGHT	// Eroom Light 는 동시 실행 가능
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

// INI File 경로 생략했을 경우 자기 자신의 실행파일 이름에서 Task.exe 를 제외한 나머지 부분으로 INI 파일 경로 생성하도록 수정
// 예 : PlustekLightTask.exe -> PlustekLight.ini
// 생략하지 않았을 경우 실행파일 argv[2] 를 INI 파일 경로로 사용
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
		SetInspType((GetPrivateProfileInt(_T("Common"), _T("TYPE"), 1, INIT_FILE_PATH))); // 검사 타입 받아오기 
		int n = GetPCNum();
		// Visual Station 연결
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

			// 표준 초기화
			// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
			// 아래에서 필요 없는 특정 초기화
			// 루틴을 제거해야 합니다.
			// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
			// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
			// 적절한 내용으로 수정해야 합니다.	
			//SetRegistryKey(strRegKey);

			//CVSLightTaskDlg dlg;
			//m_pMainWnd = &dlg;
			//m_pDlg = new CVSLightTaskDlg();
			INT_PTR nResponse = m_pDlg->DoModal();

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
		m_pLogWriter->m_fnWriteLog(_T("Light task EXIT"));
		SAFE_DELETE(m_pLight);
		SAFE_DELETE(m_pLogWriter);
		SAFE_DELETE(m_pDlg);
	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

UINT CVSLightTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSLightTaskApp* pThis = (CVSLightTaskApp*)pParam;

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
	
	printf("Internal Light Start \n");

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
	printf("Internal Light End \n");

	return 0;
}

// 로그 기록 추가
void CVSLightTaskApp::m_fnWriteLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	if (m_pInternalServer)
	{
		m_pInternalServer->m_fnPrintLog(cBuffer);
	}
}

// 로그 함수 Alg 테스크와 같은 방식으로 변경	180417 YSS
// bTraceList	: Light Task UI에 로그 출력 여부
// pszLog		: 로그 텍스트
void CVSLightTaskApp::WriteLog(const TCHAR* pszLog, ...)
{
	//TCHAR cBuffer[1024] = {0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
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

	// Log 파일에 기록
	if (m_pInternalServer)
	{
		m_pInternalServer->m_fnPrintLog(cBuffer);
	}

	// Light Task UI에 Trace 로그 표시
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