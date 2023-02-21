
// VSPGControllerTask.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CVSPGControllerTaskApp 생성

CVSPGControllerTaskApp::CVSPGControllerTaskApp()
:	m_bManual(TRUE),
	m_bIPCConnect(FALSE),
	m_pInternalServer(NULL)
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
	m_bSimulationMode = FALSE;
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CVSPGControllerTaskApp::~CVSPGControllerTaskApp()
{
	SAFE_DELETE(m_pInternalServer);
}

// 유일한 CVSPGControllerTaskApp 개체입니다.

CVSPGControllerTaskApp theApp;


// CVSPGControllerTaskApp 초기화

BOOL CVSPGControllerTaskApp::InitInstance()
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
	//m_pLogWriter = new CLogWriter(LIGHT_LOG_PATH, _T("Tact"), 7, 0, true);

	// Command Line 명령어 받아오기
	int iCnt = 0;
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW(GetCommandLine(), &iCnt);

	CString strRegKey;

	///////////// 카메라가 변경될 경우 수정해야 하는 부분 /////////////////////////////
	// CPGControllerInterface 상속하는 새로운 Class 생성 후 Interface 함수에 맞는 기능 구현 //
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

		// Visual Station 연결
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

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.	
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

	// IPC 종료 대기
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	m_bExec = FALSE;
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	SAFE_DELETE(m_pPgController);
//	SAFE_DELETE(m_pLogWriter);

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

UINT CVSPGControllerTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSPGControllerTaskApp* pThis = (CVSPGControllerTaskApp*)pParam;

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
		nRet = pThis->m_pInternalServer->m_fnStart(pThis->m_strInitFilePath);
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);

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

	return 0;
}

// 로그 기록 추가
void CVSPGControllerTaskApp::m_fnWriteLog(const TCHAR* pszLog, ...)
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