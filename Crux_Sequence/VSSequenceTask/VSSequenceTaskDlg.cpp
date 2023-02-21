
// VSSequenceTaskDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "VSSequenceTask.h"
#include "VSSequenceTaskDlg.h"
#include "afxdialogex.h"
#include "define.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVSSequenceTaskDlg ��ȭ ����

CVSSequenceTaskDlg::CVSSequenceTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSSequenceTaskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CVSSequenceTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListLog);
}

BEGIN_MESSAGE_MAP(CVSSequenceTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BTN_TEST, &CVSSequenceTaskDlg::OnBnClickedBtnTest)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_UNICODE,		m_fnPrintUiMsgUnicode)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_MULTI_BYTE,	m_fnPrintUiMsgMultiByte)
	ON_BN_CLICKED(IDC_BUTTON2, &CVSSequenceTaskDlg::OnBnClickedButton2)
//	ON_WM_WINDOWPOSCHANGING()
//	ON_WM_WINDOWPOSCHANGED()
//	ON_WM_MOVE()
END_MESSAGE_MAP()

BOOL CVSSequenceTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// Auto Mode �� �ּ�ȭ ����
	
	
	// ���� ǥ��
	CString title;
	title.Format(_T("Sequence %s"), Version);
	SetWindowText(title);
	//ShowWindow(SW_HIDE);
	// Icon ����
	
	//m_bTrayStatus = FALSE;
	//TraySetting();	
	ShowWindow(SW_SHOWMINIMIZED);
	PostMessage(WM_SHOWWINDOW, FALSE, SW_HIDE);
	//m_fnTrayIconRefresh();
	//m_bDlgShow = 0;
	//ShowWindowEx(0);
	//PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
	//ShowWindow(SW_HIDE);
	
	// Task ���� ���� Timer
	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);	

	// ����Ʈ �˶� ����
	theApp.m_fnInitializeRepeatAlarmInfo();

	// �ð� ����Ʈ �˶� ����
	theApp.m_fnInitializeTimeRepeatAlarmInfo();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CVSSequenceTaskDlg::OnPaint()
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
HCURSOR CVSSequenceTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVSSequenceTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	BOOL bHide = FALSE;
	BOOL bClose = FALSE;

	if(nID == SC_CLOSE	)
	{
		bHide = TRUE;
//#if defined(_DEBUG)
		if ( (GetKeyState(VK_SHIFT) < 0) && (AfxMessageBox(_T("���� �����Ͻðڽ��ϱ�?"), MB_YESNO|MB_ICONQUESTION)==IDYES) ) 
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
		AfxGetApp()->LoadIcon(IDI_ICON1);	

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

}

// Ʈ���� ������ �߰�
void CVSSequenceTaskDlg::TraySetting(void)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON_MSG;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	TCHAR strTitle[256];
	GetWindowText(strTitle, sizeof(strTitle));
	lstrcpy(nid.szTip, strTitle);
	Shell_NotifyIcon(NIM_ADD, &nid);
	SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
	SendMessage(WM_SETICON, (WPARAM)FALSE, (LPARAM)nid.hIcon);
	m_bTrayStatus = TRUE;
}


BOOL CVSSequenceTaskDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	//ShowWindowEx(0);
	if (pMsg->message == WM_COMMAND && pMsg->wParam == 2)
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(nid);
		nid.hWnd = m_hWnd;
		nid.uID = IDR_MAINFRAME;
		// Delete Icon in Status Bar
		Shell_NotifyIcon(NIM_DELETE, &nid);

		AfxGetApp()->LoadIcon(IDI_ICON1);
	}

	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;


	return CDialogEx::PreTranslateMessage(pMsg);

}

// Ʈ���� ������ �߰�
LRESULT CVSSequenceTaskDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		if(!IsWindowVisible())
			ShowWindow(SW_RESTORE);
		else
			ShowWindow(SW_HIDE);
	}
	return 0L;
}


void CVSSequenceTaskDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//���ҽ� ����
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_ICON1;

	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void CVSSequenceTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	HWND hButton = NULL;

	switch (nIDEvent)
	{
	case eTIMER_UpdateTaskStatus:
		if (theApp.GetIPCState())
			theApp.m_SequenceTask->SetParentWnd(m_hWnd);
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_IPC_CONNECT);
		::InvalidateRect(hButton, NULL, FALSE);
		break;
	}	

	CDialogEx::OnTimer(nIDEvent);
}


void CVSSequenceTaskDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// nIDCtl = IDC��
	switch(nIDCtl)
	{
		case IDC_BTN_IPC_CONNECT:
		{
			if (theApp.GetIPCState())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_RED, DARK_RED);
			}
		}
		break;
	}

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CVSSequenceTaskDlg::m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText)
{
	CDC dc;
	RECT rect;
	dc.Attach(lpDrawItemStruct->hDC);					//��ư�� dc���ϱ�
	rect = lpDrawItemStruct->rcItem;					//��ư���� ���ϱ�
	dc.Draw3dRect(&rect,WHITE,BLACK);					//��ư�� �ܰ��� �׸���

	dc.FillSolidRect(&rect, colorBtn);					//��ư����
	dc.SetBkColor(colorBtn);							//text�� ��׶��� ����
	dc.SetTextColor(colorText);							//texttort

	UINT state = lpDrawItemStruct->itemState;			//��ư���±��ϱ�
	if((state &ODS_SELECTED))
	{
		dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);
	}
	else
	{
		dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
	}

	TCHAR buffer[MAX_PATH];											//��ư�� text�� ������� �ӽù���
	ZeroMemory(buffer,MAX_PATH);									//�����ʱ�ȭ
	::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);	//��ư�� text���
	dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);	//��ư�� text�ֱ�
	dc.Detach();													//��ư�� dc Ǯ���ֱ�
}

void CVSSequenceTaskDlg::OnBnClickedBtnTest()
{
	//theApp.m_Config.Load();

	//CString strOrgFilePath = theApp.m_Config.GETDRV() +  _T(":\\CRUX\\DATA\\INI\\PGVoltInfo_Org.ini");

	//theApp.m_Config.UpdatePGVoltInfo(strOrgFilePath);\\


	TCHAR tMtpDrv[50] = { 0, };
	memset(tMtpDrv, 0, sizeof(tMtpDrv));
	CString strMtpDrv = _T("");

	CString strOriginPGInfoPath = _T("");

	//KYH 2021.10.21 MTP DISPLAY_ABNORMAL
	bool bAD = true;

	
	GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
	strMtpDrv.Format(_T("%s"), tMtpDrv);

	GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
	strMtpDrv.Format(_T("%s"), tMtpDrv);


	strOriginPGInfoPath.Format(_T("%s"),_T("abcde.ini"));

	//theApp.m_Config.UpdatePGVoltInfo(strMtpDrv, strOriginPGInfoPath);

	int nRet = 0;
	ST_PG_INFO stPgInfo;

	if (theApp.m_Config.UpdatePGVoltInfo(_T("D:"), strOriginPGInfoPath))
	{
		stPgInfo = theApp.m_Config.GetPgInfo();

		

		for (int nCount = 0; nCount < MAX_GRAB_STEP_COUNT; nCount++)
		{		
			if (theApp.m_Config.GetType() == TYPE_AVI)
			{
				if ((0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName,_T("WHITE"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("R"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("G"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("B"))))
				{
					if (stPgInfo.stPgData[nCount].wVoltage[0] == 7500 && stPgInfo.stPgData[nCount].wVoltage[1] == 7500 && stPgInfo.stPgData[nCount].wVoltage[2] == 7500)
					{
						bAD = false;
						CString strTemp = _T("");
						strTemp.Format(_T("%d"), bAD);
				
						break;
					}
				}
			}
			else if (theApp.m_Config.GetType() == TYPE_SVI)
			{
				if (stPgInfo.stPgData[nCount].wVoltage[0] == 7500 && stPgInfo.stPgData[nCount].wVoltage[1] == 7500 && stPgInfo.stPgData[nCount].wVoltage[2] == 7500)
				{
					bAD = false;
					break;
				}
			}
	
		}
		
		nRet = theApp.m_SequenceTask->CmdEditSend_TEST(SEND_SEND_VOLTAGEINFO, 0, (ULONG)sizeof(ST_PG_INFO), VS_PG_TASK, (byte *)&stPgInfo, CMD_TYPE_RES);
		if(nRet != APP_OK)
			throw 999;
	}
	else
	{	
		AfxMessageBox(_T("Fail"));
// 		m_fnPrintLog(FALSE, _T("SetPgInfo fail - %s"), strOriginPGInfoPath);
// 		VS_Send_Alarm_To_MainPC(eALARM_READ_FAIL_MTP_RESULT,eALARMTYPE_HEAVY);
	}
	return;

	TCHAR strCurStepName[50] = {0,};
	int nPatternIndex = 0;

	for (int i=0; i<10; i++)
	{		
		nPatternIndex = theApp.m_Config.GetCurPgIndex(i);
		nRet = theApp.m_SequenceTask->CmdEditSend_TEST(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK , (byte *)&nPatternIndex, CMD_TYPE_RES);
	}
	//theApp.m_Config.SetPgInfo(_T("E:\\mtp\\754B78E022A5AAK05.ini"));

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
 //	int nRet = 0;
 	byte  byteParam[1000]		= {0,};
 	byte* pSendParam			= byteParam;
 
 	
 	*(int*)pSendParam = 0;	pSendParam += sizeof(int);
 	*(BOOL*)pSendParam = TRUE;	pSendParam += sizeof(BOOL);
 	_tcscpy((TCHAR *)pSendParam, _T("123456"));	pSendParam += (100);
 	_tcscpy((TCHAR *)pSendParam, _T("123456"));	pSendParam += (100);
 	_tcscpy((TCHAR *)pSendParam, _T("AVI_5.5QHD_17.09.28"));	pSendParam += (100);
 	*(BOOL*)pSendParam = TRUE;	pSendParam += sizeof(BOOL);
 	*(int*)pSendParam = 0;	pSendParam += sizeof(int);
 	
 	int nParamSize = sizeof(int) + sizeof(BOOL) + (100 * 3) + sizeof(BOOL) + sizeof(int);
// 
 	nRet = theApp.m_SequenceTask->CmdEditSend_TEST(21,20, 0, nParamSize, 121, (byte *)byteParam, CMD_TYPE_NORES);
// 	
// 	return;
// 	
// 	_tcscpy((TCHAR *)pSendParam, _T("KIM SANG GOO"));		pSendParam += 100;
// 	_tcscpy((TCHAR *)pSendParam, _T("TEST"));				pSendParam += 100;
// 	_tcscpy((TCHAR *)pSendParam, theApp.m_Config.GETDRV() + _T(":\\Doc\\Image\\TEST11"));			pSendParam += 400;
// 	nRet = theApp.m_SequenceTask->CmdEditSend_TEST(21, 12, 0, (ULONG)600, 121, (byte *)byteParam, CMD_TYPE_NORES);
// 	return;
// 
// 	CString strTemp = _T("");
// 	strTemp.Format(_T("%s\\%d_%s.bmp"), theApp.m_Config.GETDRV() + _T(":\\TEST"),/*strFileDirectory,*/ 2, _T("TEST")/*theApp.m_Config.GetCurStepName(nGrabCnt)*/);
// 
// 	PARAM_INSPECT_START* prmInspStart = new PARAM_INSPECT_START;
// 	prmInspStart->nInspType = 1;
// 	memcpy((TCHAR *)prmInspStart->strPanelID, _T("TEST"), sizeof(prmInspStart->strPanelID));
// 	for (int nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
// 	{
// 		prmInspStart->bUseCamera[nCamIndex] = theApp.m_Config.IsUseCamera(2, nCamIndex);
// 		_tcscpy(prmInspStart->strImagePath[nCamIndex], strTemp);		// CString To TCHAR *
// 	}
// 	prmInspStart->nImageNum = 2;	
// 
// 	nRet = theApp.m_SequenceTask->CmdEditSend_TEST(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
// 	return;
// 
// 	PARAM_CLASSIFY_END* prmClassifyEnd = new PARAM_CLASSIFY_END;
// 
// 	_tcscpy(prmClassifyEnd->strPanelID, _T("KIM SANG GOO"));
// 	//prmClassifyEnd->bIsOK = TRUE;
// 	prmClassifyEnd->nDefectCount = 1234;
// 	//_tcscpy(prmClassifyEnd->strStartTime, _T("IM KYUNG MIN"));
// 	//_tcscpy(prmClassifyEnd->strTactTime, _T("KIM SANG GOO"));
// 
// 	nRet = theApp.m_SequenceTask->CmdEditSend_TEST(21, 30, 0, sizeof(PARAM_CLASSIFY_END), 121, (byte *)prmClassifyEnd, CMD_TYPE_RES);
// 	SAFE_DELETE(prmClassifyEnd);
	return;
}




int CVSSequenceTaskDlg::m_fnTrayIconRefresh()
{
	try
	{
		HWND  hWnd;
		CRect re;
		DWORD dwWidth, dwHeight, x, y;

		// find a handle of a tray
		hWnd = ::FindWindow( _T("Shell_TrayWnd"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("TrayNotifyWnd"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("SysPager"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("ToolbarWindow32"), NULL );
		// get client rectangle (needed for width and height of tray)
		if( hWnd!=NULL )
		{
			::GetClientRect( hWnd, &re );

			// get size of small icons
			dwWidth = (DWORD)GetSystemMetrics(SM_CXSMICON);   //  sm_cxsmicon);
			dwHeight = (DWORD)GetSystemMetrics(SM_CYSMICON);   //  sm_cysmicon);

			// initial y position of mouse - half of height of icon
			y = dwHeight >> 1;
			while( y < (DWORD)re.bottom )  // while y < height of tray
			{
				x = dwHeight>>1;              // initial x position of mouse - half of width of icon
				while( x < (DWORD)re.right ) // while x < width of tray
				{
					::SendMessage( hWnd, WM_MOUSEMOVE, 0, (y<<16)|x); // simulate moving mouse over an icon
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



LRESULT CVSSequenceTaskDlg::m_fnPrintUiMsgUnicode(WPARAM wParam, LPARAM lParam)
{
	CString strMsg = _T("");
	SYSTEMTIME systime;	

	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
		systime.wMilliseconds, (LPCTSTR)lParam);

	if(m_ctrlListLog.GetCount() > MAX_GRID_LOG	)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0,strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[] (wchar_t*)lParam;

	return 0;
}

LRESULT CVSSequenceTaskDlg::m_fnPrintUiMsgMultiByte(WPARAM wParam, LPARAM lParam)
{
	wchar_t* pStr;

	//��Ƽ ����Ʈ ũ�� ��� ���� ��ȯ
	int strSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, -1, NULL, NULL);
	//wchar_t �޸� �Ҵ�

	pStr = new WCHAR[strSize];
	memset(pStr,0,sizeof(WCHAR) * strSize);

	//�� ��ȯ
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, (int)strlen((LPCSTR)lParam)+1, pStr, strSize);


	CString strMsg = _T("");
	SYSTEMTIME systime;	

	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
		systime.wMilliseconds, pStr);

	if(m_ctrlListLog.GetCount() > MAX_GRID_LOG	)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0,strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[] (char*)lParam;
	delete[] pStr;

	return 0;
}

void CVSSequenceTaskDlg::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	PARAM_MOVE_MOTION*	prmMoveMotion = new PARAM_MOVE_MOTION;

	byte* pReceiveParam[100];

	int nPCno = 1;

	int nRet = 0;

	_tcscpy(prmMoveMotion->strMotor, _T("MOTOR_T"));
	prmMoveMotion->nMovePoint = nPCno;

	nRet = theApp.m_SequenceTask->CmdEditSend_TEST(REQ_MOVE_MOTION, 0, (ULONG)sizeof(PARAM_MOVE_MOTION), VS_MAIN_PC_TASK, (byte *)prmMoveMotion, CMD_TYPE_RES);

}
BOOL CVSSequenceTaskDlg::ShowWindowEx(int nCmdShow)
{
	m_bDlgShow = (nCmdShow == SW_SHOW);
	return (GetSafeHwnd()) ? ShowWindow(nCmdShow) : TRUE;
}

//void CVSSequenceTaskDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
//{
//	CDialogEx::OnWindowPosChanging(lpwndpos);
//		if (m_bDlgShow)
//			lpwndpos->flags |= SWP_SHOWWINDOW;
//		else
//			lpwndpos->flags &= ~SWP_SHOWWINDOW;
//
//
//	
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//}


//void CVSSequenceTaskDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
//{
//
//	if (m_bDlgShow)
//		lpwndpos->flags |= SWP_SHOWWINDOW;
//	else
//		lpwndpos->flags &= ~SWP_SHOWWINDOW;
//
//
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//}



