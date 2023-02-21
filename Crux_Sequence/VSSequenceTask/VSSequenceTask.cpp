
// VSSequenceTask.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "VSSequenceTask.h"
#include "VSSequenceTaskDlg.h"
#include <conio.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSSequenceTaskApp

BEGIN_MESSAGE_MAP(CVSSequenceTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSSequenceTaskApp 생성

CVSSequenceTaskApp::CVSSequenceTaskApp()
	:m_bIPCConnect(FALSE)
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CVSSequenceTaskApp::~CVSSequenceTaskApp()
{
}

// 유일한 CVSSequenceTaskApp 개체입니다.

CVSSequenceTaskApp theApp;


// CVSSequenceTaskApp 초기화

BOOL CVSSequenceTaskApp::InitInstance()
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
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW(GetCommandLine(), &iCnt);

	CString strRegKey;

	// 설정 파일 Load
	m_Config.Load();

	// 19.12.03 yskim - Grab Limit Count
	m_nGrabLimitWaitTime = GetPrivateProfileInt(_T("Grab Limit"), _T("Grab Limit Wait Time"), 0, PATH_INIT_FILE);

	// Visual Station 연결
	m_bExec = TRUE;
	m_hEventThreadAlive = NULL;
	m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
		
	strRegKey.Format(_T("YWDSP Sequence Task"));

	LocalFree(pStr);

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.	
	SetRegistryKey(strRegKey);

	// Tact 측정을 위한 Proc 구조체 최대 개수 만큼 생성
	m_vPanelProcInfo.reserve(MAX_PROC_PANEL_COUNT);

	CVSSequenceTaskDlg dlg;
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

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

ST_REPEAT_ALARM_INFO* CVSSequenceTaskApp::m_fnGetRepeatAlarmInfo(CString strJudgeCode)
{
	/*
	알람 정보 벡터에서 strJudgeCode가 들어있는 알람 정보를 찾아 반환한다.
	순차적으로 찾다가 최초로 찾은 알람 정보만을 반환한다.
	*/
	ST_REPEAT_ALARM_INFO* info = NULL;

	for (int i = 0; i < (int) m_vRepeatAlarmInfo.size(); i++)
	{
		for (int k = 0; k < 10; k++)
		{
			if (m_vRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(_T("")) == 0)
				break;

			if (m_vRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(strJudgeCode) == 0)
			{
				info = m_vRepeatAlarmInfo[i];
				break;
			}
		}

		if (info != NULL)
			break;
	}

	return info;
}

ST_TIME_REPEAT_ALARM_INFO* CVSSequenceTaskApp::m_fnGetTimeRepeatAlarmInfo(CString strJudgeCode)
{
	/*
	알람 정보 벡터에서 strJudgeCode가 들어있는 알람 정보를 찾아 반환한다.
	순차적으로 찾다가 최초로 찾은 알람 정보만을 반환한다.
	*/
	ST_TIME_REPEAT_ALARM_INFO* info = NULL;

	for (int i = 0; i < (int) m_vTimeRepeatAlarmInfo.size(); i++)
	{
		for (int k = 0; k < 10; k++)
		{
			if (m_vTimeRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(_T("")) == 0)
				break;

			if (m_vTimeRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(strJudgeCode) == 0)
			{
				info = m_vTimeRepeatAlarmInfo[i];
				break;
			}
		}

		if (info != NULL)
			break;
	}

	return info;
}

void CVSSequenceTaskApp::m_fnInitializeRepeatAlarmInfo()
{
	/*
	Repeat Alarm Info 초기화
	각 PC 별 CRUX_Sequence.ini 내 존재하는 리피트 알람 정보를 가져온다.
	정보가 없을 경우 기본값을 셋팅해준다. (ini에도 기본값 넣어준다.)
	기본값
	[Repeat Alarm Info]
	item1 = 0, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS
	; 첫번째 인자 1(=사용), (0=미사용)
	; 두번째 인자 3(=Repeat Count, 즉, 3번 이상(>=) 연속 발생 시 알람)
	; 세번째 인자 DISPLAY_ABNORMAL(=카운트할 디펙명)

	(ini 작성 예제)
	[Repeat Alarm Info]
	item1 = 1, 3, ABNORMAL_AD1+ABNORMAL_AD3
	item2 = 1, 3, WHITE_BRIGHTNESS

	나중에 UI 필요할 경우
	UI 화면 만들고, UI 저장 시 위 포맷대로 ini 파일에 정보 저장한 뒤, Seq에서 이 함수를 호출해주면 됨
	*/

	TCHAR szBuf[500] = { 0, };
	CString strItemKey;
	CString strItemValue;
	CString strArgu1, strArgu2, strArgu3;
	CString temp;

	m_vRepeatAlarmInfo.clear();

	for (int i = 1; i <= MAX_PROC_PANEL_COUNT; i++)
	{
		strItemKey.Format(_T("item%d"), i);
		GetPrivateProfileString(_T("Repeat Alarm Info"), strItemKey, _T(""), szBuf, sizeof(szBuf), VS_SEQUENCE_TASK_INI_FILE);
		strItemValue.Format(_T("%s"), szBuf);

		// 빈 값이면 멈춘다.
		if (strItemValue.CompareNoCase(_T("")) == 0)
			break;

		ST_REPEAT_ALARM_INFO* stInfo = new ST_REPEAT_ALARM_INFO;

		// 첫번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu1, strItemValue, 0, ',');
		strArgu1.Trim();
		stInfo->bUse = strArgu1.CompareNoCase(_T("0")) != 0;	// 0이 아니면 true

		// 두번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu2, strItemValue, 1, ',');
		strArgu2.Trim();
		stInfo->nRepeatCount = _ttoi(strArgu2);

		// 세번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu3, strItemValue, 2, ',');
		strArgu3.Trim();
		for (int k = 0; k < 10; k++)
		{
			AfxExtractSubString(temp, strArgu3, k, '+');
			stInfo->strJudge[k] = temp;
		}

		// 현재 발생 횟수 초기화
		for (int stage_index = 0; stage_index < MAX_STAGE_COUNT; stage_index++)
		{
			stInfo->nCurrentCount[stage_index] = 0;
		}

		// 벡터에 넣는다.
		m_vRepeatAlarmInfo.push_back(stInfo);
	}

	// 벡터가 비어있다면 기본 값을 셋팅해준다.
	if (m_vRepeatAlarmInfo.empty())
	{
		WritePrivateProfileString(_T("Repeat Alarm Info"), _T("item1"), _T("0, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS"), VS_SEQUENCE_TASK_INI_FILE);
		m_fnInitializeRepeatAlarmInfo();
	}
}

void CVSSequenceTaskApp::m_fnInitializeTimeRepeatAlarmInfo()
{
	/*
	Time Repeat Alarm Info 초기화
	각 PC 별 CRUX_Sequence.ini 내 존재하는 리피트 알람 정보를 가져온다.
	정보가 없을 경우 기본값을 셋팅해준다. (ini에도 기본값 넣어준다.)
	기본값
	[Repeat Alarm Info]
	item1 = 0, 20, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS
	; 첫번째 인자 1(=사용), (0=미사용)
	; 두번째 인자 20(=시간 조건 (단위:분), 20분 미만에서 발생 횟수를 카운팅하기 위함)
	; 세번째 인자 3(=반복 조건, 정해진 시간 내 발생 횟수가 3회 이상이면 알람 발생)
	; 네번째 인자 DISPLAY_ABNORMAL(=카운트할 디펙명)

	(ini 작성 예제)
	[Repeat Alarm Info]
	item1 = 1, 10, 3, ABNORMAL_AD1+ABNORMAL_AD3
	item2 = 1, 20, 5, WHITE_BRIGHTNESS

	나중에 UI 필요할 경우
	UI 화면 만들고, UI 저장 시 위 포맷대로 ini 파일에 정보 저장한 뒤, Seq에서 이 함수를 호출해주면 됨
	*/

	TCHAR szBuf[500] = { 0, };
	CString strItemKey;
	CString strItemValue;
	CString strArgu1, strArgu2, strArgu3, strArgu4;
	CString temp;

	// 벡터 초기화
	for (int i = 0; i < (int) m_vTimeRepeatAlarmInfo.size(); i++)
		m_vTimeRepeatAlarmInfo[i]->vCells.clear();

	m_vTimeRepeatAlarmInfo.clear();

	for (int i = 1; i <= MAX_PROC_PANEL_COUNT; i++)
	{
		strItemKey.Format(_T("item%d"), i);
		GetPrivateProfileString(_T("Time Repeat Alarm Info"), strItemKey, _T(""), szBuf, sizeof(szBuf), VS_SEQUENCE_TASK_INI_FILE);
		strItemValue.Format(_T("%s"), szBuf);

		// 빈 값이면 멈춘다.
		if (strItemValue.CompareNoCase(_T("")) == 0)
			break;

		ST_TIME_REPEAT_ALARM_INFO* stInfo = new ST_TIME_REPEAT_ALARM_INFO;

		// 첫번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu1, strItemValue, 0, ',');
		strArgu1.Trim();
		stInfo->bUse = strArgu1.CompareNoCase(_T("0")) != 0;	// 0이 아니면 true

		// 두번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu2, strItemValue, 1, ',');
		strArgu2.Trim();
		stInfo->tsAlarmTime = CTimeSpan(0, 0, _ttoi(strArgu2), 0);

		// 세번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu3, strItemValue, 2, ',');
		strArgu3.Trim();
		stInfo->nAlarmCount = _ttoi(strArgu3);

		// 네번째 인자 파싱 및 셋팅
		AfxExtractSubString(strArgu4, strItemValue, 3, ',');
		strArgu4.Trim();
		for (int k = 0; k < 10; k++)
		{
			AfxExtractSubString(temp, strArgu4, k, '+');
			stInfo->strJudge[k] = temp;
		}

		// 벡터에 넣는다.
		m_vTimeRepeatAlarmInfo.push_back(stInfo);
	}

	// 벡터가 비어있다면 기본 값을 셋팅해준다.
	if (m_vTimeRepeatAlarmInfo.empty())
	{
		WritePrivateProfileString(_T("Time Repeat Alarm Info"), _T("item1"), _T("0, 20, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS"), VS_SEQUENCE_TASK_INI_FILE);
		m_fnInitializeTimeRepeatAlarmInfo();
	}
}

UINT CVSSequenceTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSSequenceTaskApp* pThis = (CVSSequenceTaskApp*)pParam;

	// Visual Station 연결
	int					nRet;

	do 
	{
		nRet = pThis->m_fnConectVisualStation();
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);

	printf("Internal Sequence Start \n");

	pThis->m_SequenceTask->SendNotifyInitial();

	// VS 상태 체크
	while(pThis->m_bExec)
	{
		// VS 연결 상태 Get
		if (pThis->m_SequenceTask != NULL)
			// VS 연결 상태 Get
			pThis->SetIPCState(pThis->m_SequenceTask->GetVSState());
		Sleep(1000);
	}
	
	pThis->SetIPCState(FALSE);
	//SAFE_DELETE(pInternalServer);

	SetEvent(pThis->m_hEventThreadAlive);
	printf("Internal Sequence End \n");

	return 0;
}

int CVSSequenceTaskApp::m_fnConectVisualStation()
{
	if (m_SequenceTask != NULL)
	{
		return APP_OK;
	}
	TCHAR szSeqLogPath[100] = {0,}, szSeqFileName[100] = {0,}, szServerIP[100] = {0,};
	int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;	

	// Sequence Task 는 PC 당 하나만 있어야 함	
	nTaskNo				= (m_Config.GetPCNum() * 100)
						+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 21, VS_SEQUENCE_TASK_INI_FILE));
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, VS_SEQUENCE_TASK_INI_FILE);
	uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, VS_SEQUENCE_TASK_INI_FILE);
	uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, VS_SEQUENCE_TASK_INI_FILE);
	uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, VS_SEQUENCE_TASK_INI_FILE);

	GetPrivateProfileString(_T("LogInfo"), _T("Path") ,theApp.m_Config.GETDRV() + _T(":\\VisualStation\\VSLOG\\VSPLC"), szSeqLogPath, sizeof(szSeqLogPath), VS_SEQUENCE_TASK_INI_FILE);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("CRUX"), szSeqFileName, sizeof(szSeqFileName), VS_SEQUENCE_TASK_INI_FILE);
	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), VS_SEQUENCE_TASK_INI_FILE);

	m_SequenceTask = new TaskInterfacer(nTaskNo, nServerPort, szServerIP, szSeqLogPath,
		szSeqFileName, false, uPeriodDay, uWriteTerm, uWriteLogLevel);
	int nRet = m_SequenceTask->Start();
	if (nRet != APP_OK)
	{
		SAFE_DELETE(m_SequenceTask);
	}
	return nRet;
}
