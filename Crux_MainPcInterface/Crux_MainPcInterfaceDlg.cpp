
// Crux_MainPcInterfaceDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Crux_MainPcInterface.h"
#include "Crux_MainPcInterfaceDlg.h"
#include "afxdialogex.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrux_MainPcInterfaceDlg 대화 상자




CCrux_MainPcInterfaceDlg::CCrux_MainPcInterfaceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCrux_MainPcInterfaceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);
	MainPcInterface = NULL;
	m_pMainPcSimulator = NULL;
	m_bSimulationMode = FALSE;

	CString strTitle = _T("");
	int nPcNum = GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH);
	if (theApp.GetSimulationProgramMode())
	{
		strTitle.Format(_T("MainPcSimulator%d"), nPcNum);
		//m_hProcessLockMutex = CreateMutex(NULL, TRUE, _T("MainPcSimulator"));
		m_hProcessLockMutex = CreateMutex(NULL, TRUE, strTitle);
	}
	else
	{
		strTitle.Format(_T("MainPcInterface%d"), nPcNum);
		//m_hProcessLockMutex = CreateMutex(NULL, TRUE, _T("MainPcInterface"));
		m_hProcessLockMutex = CreateMutex(NULL, TRUE, strTitle);
	}

	// 	if (theApp.GetSimulationProgramMode())
	// 		m_hProcessLockMutex=CreateMutex(NULL,TRUE, _T("MainPcSimulator")); 
	// 	else
	// 		m_hProcessLockMutex=CreateMutex(NULL,TRUE, _T("MainPcInterface")); 

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CWnd *pWndPrev, *pWndChild;
		if (theApp.GetSimulationProgramMode())
			pWndPrev = CWnd::FindWindow(NULL, strTitle);
		else
			pWndPrev = CWnd::FindWindow(NULL, strTitle);

		if (pWndPrev)
		{
			pWndChild = pWndPrev->GetLastActivePopup();

			if (pWndChild->IsIconic())
				pWndPrev->ShowWindow(SW_RESTORE);

			pWndChild->SetForegroundWindow();
		}
		else
		{
			AfxMessageBox(_T("이미 프로그램이 실행 중입니다."));
			PostQuitMessage(0);
		}

	}
}

CCrux_MainPcInterfaceDlg::~CCrux_MainPcInterfaceDlg()
{
	if (m_hProcessLockMutex)
	{
		ReleaseMutex(m_hProcessLockMutex);
	}
	SAFE_DELETE(MainPcInterface);
	SAFE_DELETE(m_pMainPcSimulator);
}

void CCrux_MainPcInterfaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListLog);
}

BEGIN_MESSAGE_MAP(CCrux_MainPcInterfaceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAYICON_MSG, TrayIconMessage)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_UNICODE, m_fnPrintUiMsgUnicode)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_MULTI_BYTE, m_fnPrintUiMsgMultiByte)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_START_SIMULATOR, &CCrux_MainPcInterfaceDlg::OnBnClickedBtnStartSimulator)
	ON_BN_CLICKED(IDC_BUTTON1, &CCrux_MainPcInterfaceDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CCrux_MainPcInterfaceDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CCrux_MainPcInterfaceDlg 메시지 처리기

BOOL CCrux_MainPcInterfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.



	if (theApp.GetSimulationProgramMode())
	{
		this->SetWindowText(_T("Main Pc Simulator"));
		((CButton*)GetDlgItem(IDC_BTN_START_SIMULATOR))->ShowWindow(SW_SHOW);
	}
	else
	{
		// 버전 표기
		CString title;
		title.Format(_T("MainPcInterface %s"), _Version);
		SetWindowText(title);

		((CButton*)GetDlgItem(IDC_BTN_START_SIMULATOR))->ShowWindow(SW_SHOW);
	}
	
	//m_fnTrayIconRefresh();

	//m_bTrayStatus = FALSE;
	//TraySetting();
	ShowWindow(SW_SHOWMINIMIZED);
	PostMessage(WM_SHOWWINDOW, FALSE, SW_HIDE);

	SetTimer(eTIMER_VS_CONNECT, 1000, NULL);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCrux_MainPcInterfaceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	//{
	//	CAboutDlg dlgAbout;
	//	dlgAbout.DoModal();
	//}
	//else
	//{
	//	CDialogEx::OnSysCommand(nID, lParam);
	//}
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	BOOL bHide = FALSE;
	BOOL bClose = FALSE;

	if (nID == SC_CLOSE)
	{
		bHide = TRUE;
		//#if defined(_DEBUG)
		if ((GetKeyState(VK_SHIFT) < 0) && (AfxMessageBox(_T("Do you want to quit?"), MB_YESNO | MB_ICONQUESTION) == IDYES))
		{
			bClose = TRUE;
		}
		//#else
		//		bClose = TRUE;
		//#endif
	}
	else if (nID == SC_MINIMIZE)
	{
		bHide = TRUE;
	}

	if (bClose)
	{
		if (m_bTrayStatus)
		{
			NOTIFYICONDATA nid;
			nid.cbSize = sizeof(nid);
			nid.hWnd = m_hWnd;
			nid.uID = IDR_MAINFRAME;
			// Delete Icon in Status Bar
			Shell_NotifyIcon(NIM_DELETE, &nid);
		}
		AfxGetApp()->LoadIcon(IDI_TRAY);

		CDialog::OnSysCommand(nID, lParam);
	}
	else if (bHide)
	{
		ShowWindow(SW_MINIMIZE);
		return;

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
	//CDialogEx::OnSysCommand(nID, lParam);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCrux_MainPcInterfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCrux_MainPcInterfaceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCrux_MainPcInterfaceDlg::TraySetting(void)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON_MSG;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);

	TCHAR strTitle[256];
	GetWindowText(strTitle, sizeof(strTitle));
	lstrcpy(nid.szTip, strTitle);
	Shell_NotifyIcon(NIM_ADD, &nid);
	SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
	m_bTrayStatus = TRUE;
}

// 트레이 아이콘 추가
LRESULT CCrux_MainPcInterfaceDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		if (!IsWindowVisible())
			ShowWindow(SW_RESTORE);
		else
			ShowWindow(SW_HIDE);
	}
	return 0L;
}


void CCrux_MainPcInterfaceDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//리소스 해제
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_TRAY;

	Shell_NotifyIcon(NIM_DELETE, &nid);

}


void CCrux_MainPcInterfaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int nRet = APP_OK;
	switch (nIDEvent)
	{
	case eTIMER_VS_CONNECT:
		nRet = m_fnConectVisualStation();
		if (nRet == APP_OK)
		{
			KillTimer(eTIMER_VS_CONNECT);
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}

int CCrux_MainPcInterfaceDlg::m_fnConectVisualStation()
{
	if (MainPcInterface != NULL)
	{
		return APP_OK;
	}
	TCHAR szSeqLogPath[100] = { 0, }, szSeqFileName[100] = { 0, }, szServerIP[100] = { 0, };
	int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;
	bool bSimulationMode = false;
	CString strIniFile;

	if (theApp.GetSimulationProgramMode())
		strIniFile = VS_MAIN_PC_SIMULATOR_TASK_INI_FILE;
	else
		strIniFile = VS_MAIN_PC_INTERFACE_TASK_INI_FILE;


	//	nTaskNo				= GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 21, VS_SEQUENCE_TASK_INI_FILE);
	nTaskNo = (theApp.GetPCNum() * 100)
		+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 60, strIniFile));
	nServerPort = GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, strIniFile);
	uPeriodDay = GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, strIniFile);
	uWriteTerm = GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, strIniFile);
	uWriteLogLevel = GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, strIniFile);

	GetPrivateProfileString(_T("LogInfo"), _T("Path"), theApp.GETDRV() + _T(":\\VisualStation\\VSLOG\\VSPLC"), szSeqLogPath, sizeof(szSeqLogPath), strIniFile);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName"), _T("CRUX"), szSeqFileName, sizeof(szSeqFileName), strIniFile);
	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP"), _T("127.0.0.1"), szServerIP, sizeof(szServerIP), strIniFile);

	MainPcInterface = new TaskInterfacer(nTaskNo, nServerPort, szServerIP, szSeqLogPath,
		szSeqFileName, false, uPeriodDay, uWriteTerm, uWriteLogLevel, m_hWnd);
	int nRet = MainPcInterface->Start();
	if (nRet != APP_OK)
	{
		SAFE_DELETE(MainPcInterface);
		return nRet;
	}
	if (theApp.GetSimulationProgramMode())
		nRet = MainPcInterface->CmdEditSend(START_MAIN_PC_IF_SOCKET, 0, 0, VS_MAIN_PC_SIMULATOR_TASK, NULL, CMD_TYPE_NORES);
	else
		nRet = MainPcInterface->CmdEditSend(START_MAIN_PC_IF_SOCKET, 0, 0, VS_MAIN_PC_TASK, NULL, CMD_TYPE_NORES);

	if (nRet != APP_OK)
	{
		AfxMessageBox(_T("Program Start fail!!"));
		SAFE_DELETE(MainPcInterface);
	}
	return nRet;
}

LRESULT CCrux_MainPcInterfaceDlg::m_fnPrintUiMsgUnicode(WPARAM wParam, LPARAM lParam)
{
	CString strMsg = _T("");
	SYSTEMTIME systime;

	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond,
		systime.wMilliseconds, (LPCTSTR)lParam);

	if (m_ctrlListLog.GetCount() > MAX_GRID_LOG)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0, strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[](wchar_t*)lParam;

	return 0;
}

LRESULT CCrux_MainPcInterfaceDlg::m_fnPrintUiMsgMultiByte(WPARAM wParam, LPARAM lParam)
{
	wchar_t* pStr;

	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, -1, NULL, NULL);
	//wchar_t 메모리 할당

	pStr = new WCHAR[strSize];
	memset(pStr, 0, sizeof(WCHAR) * strSize);

	//형 변환
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, (int)strlen((LPCSTR)lParam) + 1, pStr, strSize);


	CString strMsg = _T("");
	SYSTEMTIME systime;

	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond,
		systime.wMilliseconds, pStr);

	if (m_ctrlListLog.GetCount() > MAX_GRID_LOG)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0, strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[](char*)lParam;
	delete[] pStr;

	return 0;
}


int CCrux_MainPcInterfaceDlg::m_fnTrayIconRefresh()
{
	try
	{
		HWND  hWnd;
		CRect re;
		DWORD dwWidth, dwHeight, x, y;

		// find a handle of a tray
		hWnd = ::FindWindow(_T("Shell_TrayWnd"), NULL);

		if (hWnd != NULL)
			hWnd = ::FindWindowEx(hWnd, 0, _T("TrayNotifyWnd"), NULL);

		if (hWnd != NULL)
			hWnd = ::FindWindowEx(hWnd, 0, _T("SysPager"), NULL);

		if (hWnd != NULL)
			hWnd = ::FindWindowEx(hWnd, 0, _T("ToolbarWindow32"), NULL);
		// get client rectangle (needed for width and height of tray)
		if (hWnd != NULL)
		{
			::GetClientRect(hWnd, &re);

			// get size of small icons
			dwWidth = (DWORD)GetSystemMetrics(SM_CXSMICON);   //  sm_cxsmicon);
			dwHeight = (DWORD)GetSystemMetrics(SM_CYSMICON);   //  sm_cysmicon);

			// initial y position of mouse - half of height of icon
			y = dwHeight >> 1;
			while (y < (DWORD)re.bottom)  // while y < height of tray
			{
				x = dwHeight >> 1;              // initial x position of mouse - half of width of icon
				while (x < (DWORD)re.right) // while x < width of tray
				{
					::SendMessage(hWnd, WM_MOUSEMOVE, 0, (y << 16) | x); // simulate moving mouse over an icon
					x = x + dwWidth; // add width of icon to x position
				}
				y = y + dwHeight; // add height of icon to y position
			}
		}



		return APP_OK;
	}
	catch (...)
	{
		return APP_NG;
	}
}

void CCrux_MainPcInterfaceDlg::OnBnClickedBtnStartSimulator()
{
	if (m_pMainPcSimulator)
	{
		m_pMainPcSimulator->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pMainPcSimulator = new CMainPcSimulator(MainPcInterface);
		m_pMainPcSimulator->Create(IDD_SIMULATION_MODE_DIALOG);
		m_pMainPcSimulator->ShowWindow(SW_SHOW);
	}
}


BOOL CCrux_MainPcInterfaceDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCrux_MainPcInterfaceDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*int nRet = 0;
	byte bPCNO[8] = { 0, };
	int OK = 1;
	byte* pPCNoTemp = bPCNO;

	*(int*)pPCNoTemp = theApp.GetPCNum() -1;
	pPCNoTemp += sizeof(int);
	*(int*)pPCNoTemp = OK;
	pPCNoTemp += sizeof(int);

	nRet = MainPcInterface->CmdEditSend(SEND_THETA_ON_OFF, 0, sizeof(bPCNO), VS_UI_TASK, bPCNO, CMD_TYPE_RES, 500);*/

	//2021.02.24 Grab Time 수동조절
	clock_t start, end;
	int nResult;
	double dTime;
	CString strIniFile;
	TCHAR chrTime[10] = { 0, };
	LPTSTR fstrResult;

	CString strMsg = _T("");
	start = clock();
	Sleep(3000);
	end = clock();
	nResult = (int)(end - start);

	strIniFile = VS_MAIN_PC_INTERFACE_TASK_INI_FILE;

	// double dTime = GetPrivateProfileInt(_T("MainGrabTime"), _T("Time"), 0, strIniFile) * 1000;
	GetPrivateProfileString(_T("MainGrabTime"), _T("Time"), _T("0"), chrTime, sizeof(chrTime), strIniFile);
	int nUse = GetPrivateProfileInt(_T("MainGrabTime"), _T("Use"), 0, strIniFile);

	dTime = _tcstod(chrTime, &fstrResult);

	int nSleepTime = (dTime * 1000) - nResult;

	if (nUse == 1 && nResult < (int)chrTime)
	{
		strMsg.Format(L"Grab Time %.1f Delay START\n", dTime);
		Sleep(nSleepTime);
		strMsg.Format(L"Grab Time %.1f Delay END\n", dTime);
	}

}



void CCrux_MainPcInterfaceDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nRet = 0;
	byte bPCNO[8] = { 0, };
	int NG = 0;
	byte* pPCNoTemp = bPCNO;

	*(int*)pPCNoTemp = theApp.GetPCNum() - 1;
	pPCNoTemp += sizeof(int);
	*(int*)pPCNoTemp = NG;
	pPCNoTemp += sizeof(int);

	//nRet = MainPcInterface->CmdEditSend(SEND_THETA_OFF, 0, sizeof(bPCNO), VS_UI_TASK, bPCNO, CMD_TYPE_RES, 500);
}
