
// Crux_MainPcInterfaceDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Crux_MainPcInterface.h"
#include "Crux_MainPcInterfaceDlg.h"
#include "afxdialogex.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrux_MainPcInterfaceDlg ��ȭ ����




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
			AfxMessageBox(_T("�̹� ���α׷��� ���� ���Դϴ�."));
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


// CCrux_MainPcInterfaceDlg �޽��� ó����

BOOL CCrux_MainPcInterfaceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.



	if (theApp.GetSimulationProgramMode())
	{
		this->SetWindowText(_T("Main Pc Simulator"));
		((CButton*)GetDlgItem(IDC_BTN_START_SIMULATOR))->ShowWindow(SW_SHOW);
	}
	else
	{
		// ���� ǥ��
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


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CCrux_MainPcInterfaceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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

// Ʈ���� ������ �߰�
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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//���ҽ� ����
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_TRAY;

	Shell_NotifyIcon(NIM_DELETE, &nid);

}


void CCrux_MainPcInterfaceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

	//��Ƽ ����Ʈ ũ�� ��� ���� ��ȯ
	int strSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, -1, NULL, NULL);
	//wchar_t �޸� �Ҵ�

	pStr = new WCHAR[strSize];
	memset(pStr, 0, sizeof(WCHAR) * strSize);

	//�� ��ȯ
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CCrux_MainPcInterfaceDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	/*int nRet = 0;
	byte bPCNO[8] = { 0, };
	int OK = 1;
	byte* pPCNoTemp = bPCNO;

	*(int*)pPCNoTemp = theApp.GetPCNum() -1;
	pPCNoTemp += sizeof(int);
	*(int*)pPCNoTemp = OK;
	pPCNoTemp += sizeof(int);

	nRet = MainPcInterface->CmdEditSend(SEND_THETA_ON_OFF, 0, sizeof(bPCNO), VS_UI_TASK, bPCNO, CMD_TYPE_RES, 500);*/

	//2021.02.24 Grab Time ��������
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
