
// SpaceManager.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "SpaceManager.h"
#include "SpaceManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceManagerApp

BEGIN_MESSAGE_MAP(CSpaceManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSpaceManagerApp 생성

CSpaceManagerApp::CSpaceManagerApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CSpaceManagerApp 개체입니다.

CSpaceManagerApp theApp;

// CSpaceManagerApp 초기화

BOOL CSpaceManagerApp::InitInstance()
{

	// 중복 실행 방지
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("SpaceManager"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ReleaseMutex(hMutex);

		CWnd *pWndPrev, *pWndChild;
		pWndPrev = CWnd::FindWindow(NULL, _T("SpaceManager"));
		if(pWndPrev)
		{
			pWndChild = pWndPrev->GetLastActivePopup();

			if(pWndChild->IsIconic())
				pWndPrev->ShowWindow(SW_RESTORE);

			pWndChild->SetForegroundWindow();
		}
		return FALSE;
	}
	ReleaseMutex(hMutex);

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

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	m_pLogWriter = new CLogWriter(SM_LOG_PATH, _T("SpaceManager"), 7, 0, true);
	//2016.03.15 비밀폴더 사용으로 인해 언어변환안됨문제 설정파일 D:\Crux_Data\INI\Language\SpaceManager폴더로 이동시킴
	//CreateDirectory(ORG_FILE_PATH, NULL);
	if(!PathIsDirectory(ORG_FILE_PATH))
		SHCreateDirectoryEx(NULL, ORG_FILE_PATH, NULL);
	CopyFile(ORG_RESOURCE_PATH, RESOURCE_PATH, 0);		        // 데이터 파일 복사
	CopyFile(ORG_LANG_KOREAN_PATH, LANG_KOREAN_PATH, 0);		// 데이터 파일 복사
	CopyFile(ORG_LANG_CHINESE_PATH, LANG_CHINESE_PATH, 0);		// 데이터 파일 복사
	//2016.03.15 비밀폴더 사용으로 인해 언어변환안됨문제 설정파일 D:\Crux_Data\INI\Language\SpaceManager폴더로 이동시킴

	CSpaceManagerDlg dlg;
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

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 로그 객체 삭제
	if(m_pLogWriter){ delete (m_pLogWriter); (m_pLogWriter)=0; }

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

// 로그 기록 추가
void CSpaceManagerApp::m_fnWriteLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogWriter->m_fnWriteLog(cBuffer);
}