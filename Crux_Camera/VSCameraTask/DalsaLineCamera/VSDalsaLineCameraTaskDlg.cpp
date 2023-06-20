#include "stdafx.h"
#include "VSCameraTask.h"
#include "VSDalsaLineCameraTaskDlg.h"
#include "DalsaLineCamera.h"
#include "DalsaLineParam.hpp"

#include "version.h"
#include "afxdialogex.h"
#include "define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CVSCameraTaskDlg::CVSCameraTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSCameraTaskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVSCameraTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAM_TAP, m_comboCamTap);
	DDX_Control(pDX, IDC_COMBO_CAM_TEST, m_comboCamTest);
	DDX_Control(pDX, IDC_COMBO_CAM_TRIMODE, m_comboCamTriMode);
	DDX_Control(pDX, IDC_COMBO_CAM_VDOUT, m_comboCamVdOut);
	DDX_Control(pDX, IDC_CHECK_CAM_REVERSE, m_checkReverse);
}

BEGIN_MESSAGE_MAP(CVSCameraTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DRAWITEM()

	ON_EN_CHANGE(IDC_EDIT_CAM_GR, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GRFO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GRFE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GRRO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GRRE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GG, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GGFO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GGFE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GGRO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GGRE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GB, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GBFO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GBFE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GBRO, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_GBRE, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_WBR, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_WBG, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_WBB, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_EXP, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_EN_CHANGE(IDC_EDIT_CAM_LINEDELAY, &CVSCameraTaskDlg::OnEnChangeEditCam)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_TRIMODE, &CVSCameraTaskDlg::OnCbnSelchangeComboCam)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_VDOUT, &CVSCameraTaskDlg::OnCbnSelchangeComboCam)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_TAP, &CVSCameraTaskDlg::OnCbnSelchangeComboCam)
	ON_CBN_SELCHANGE(IDC_COMBO_CAM_TEST, &CVSCameraTaskDlg::OnCbnSelchangeComboCam)
	ON_BN_CLICKED(IDC_CHECK_CAM_REVERSE, &CVSCameraTaskDlg::OnBnClickedCheckCamReverse)
	ON_BN_CLICKED(IDC_BUTTON_CAM_LOAD, &CVSCameraTaskDlg::OnBnClickedButtonCam)
	ON_BN_CLICKED(IDC_BUTTON_CAM_SAVE, &CVSCameraTaskDlg::OnBnClickedButtonCam)
	ON_BN_CLICKED(IDC_BUTTON_CAM_STA, &CVSCameraTaskDlg::OnBnClickedButtonCam)
	ON_BN_CLICKED(IDC_BUTTON_CAM_GRABSTART, &CVSCameraTaskDlg::OnBnClickedButtonCam)
	ON_BN_CLICKED(IDC_BUTTON_CAM_GRABSTOP, &CVSCameraTaskDlg::OnBnClickedButtonCam)

	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_INIT_F, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_INIT_B, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_SET_F, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_SET_B, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_SCAN_F, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_SCAN_B, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_BN_CLICKED(IDC_BUTTON_TRIGGER_SAVE, &CVSCameraTaskDlg::OnBnClickedButtonTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_STARTPOS_F, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_STARTPOS_B, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_PERIOD_F, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_PERIOD_B, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_COUNT_F, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
	ON_EN_CHANGE(IDC_EDIT_TRIGGER_COUNT_B, &CVSCameraTaskDlg::OnEnChangeEditTrigger)
END_MESSAGE_MAP()


BOOL CVSCameraTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// 버전 표기
	CString title;
	title.Format(_T("DalsaLine Camera %s"), _Version);
	SetWindowText(title);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->GetAllProperty();
	DalsaLineParam* param = &DalsaLineParam::Instance();
	SetDlgItemText(IDC_EDIT_INI_FILE_PATH, theApp.GetInitFilePath());
	SetDlgItemText(IDC_EDIT_CAM_MODEL, param->GetParamStr(_T("Type")));
	SetDlgItemText(IDC_EDIT_CAM_GRB_NO, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->FrameGrabberNo));
	SetDlgItemText(IDC_EDIT_CAM_DIG_CH, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->DigitizerChannel));
	SetDlgItemText(IDC_EDIT_CAM_DCF, ((CDalsaLineCamera*)theApp.m_pCamera)->DcfFile);
	SetDlgItemText(IDC_EDIT_CAM_PORT, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->CameraPort));
	SetDlgItemText(IDC_EDIT_CAM_BAUD, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->CameraBaud));
	SetDlgItemText(IDC_EDIT_CAM_GR, param->GetParamStr(_T("r")));
	SetDlgItemText(IDC_EDIT_CAM_GRFO, param->GetParamStr(_T("grfo")));
	SetDlgItemText(IDC_EDIT_CAM_GRFE, param->GetParamStr(_T("grfe")));
	SetDlgItemText(IDC_EDIT_CAM_GRRO, param->GetParamStr(_T("grro")));
	SetDlgItemText(IDC_EDIT_CAM_GRRE, param->GetParamStr(_T("grre")));
	SetDlgItemText(IDC_EDIT_CAM_GG, param->GetParamStr(_T("g")));
	SetDlgItemText(IDC_EDIT_CAM_GGFO, param->GetParamStr(_T("ggfo")));
	SetDlgItemText(IDC_EDIT_CAM_GGFE, param->GetParamStr(_T("ggfe")));
	SetDlgItemText(IDC_EDIT_CAM_GGRO, param->GetParamStr(_T("ggro")));
	SetDlgItemText(IDC_EDIT_CAM_GGRE, param->GetParamStr(_T("ggre")));
	SetDlgItemText(IDC_EDIT_CAM_GB, param->GetParamStr(_T("b")));
	SetDlgItemText(IDC_EDIT_CAM_GBFO, param->GetParamStr(_T("gbfo")));
	SetDlgItemText(IDC_EDIT_CAM_GBFE, param->GetParamStr(_T("gbfe")));
	SetDlgItemText(IDC_EDIT_CAM_GBRO, param->GetParamStr(_T("gbro")));
	SetDlgItemText(IDC_EDIT_CAM_GBRE, param->GetParamStr(_T("gbre")));
	SetDlgItemText(IDC_EDIT_CAM_WBR, param->GetParamStr(_T("WBr")));
	SetDlgItemText(IDC_EDIT_CAM_WBG, param->GetParamStr(_T("WBg")));
	SetDlgItemText(IDC_EDIT_CAM_WBB, param->GetParamStr(_T("WBb")));
	SetDlgItemText(IDC_EDIT_CAM_EXP, param->GetParamStr(_T("i")));
	SetDlgItemText(IDC_EDIT_CAM_LINEDELAY, param->GetParamStr(_T("d")));
	m_comboCamTriMode.SetCurSel(param->GetParamInt(_T("t")));
	m_comboCamVdOut.SetCurSel(param->GetParamInt(_T("v")));
	m_comboCamTap.SetCurSel(param->GetParamInt(_T("tap")) - 1);
	m_comboCamTest.SetCurSel(param->GetParamInt(_T("T")));
	m_checkReverse.SetCheck(param->GetParamInt(_T("rev")));
	
	YskimLib *lib = &YskimLib::Instance();
	SetDlgItemText(IDC_EDIT_TRIGGER_COM, lib->GetDataFromINI(_T("Trigger"), _T("Port"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_BAUD, lib->GetDataFromINI(_T("Trigger"), _T("Baud"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_STARTPOS_F, lib->GetDataFromINI(_T("Trigger"), _T("StartF"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_STARTPOS_B, lib->GetDataFromINI(_T("Trigger"), _T("StartB"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_STOPPOS_F, lib->GetDataFromINI(_T("Trigger"), _T("StopF"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_STOPPOS_B, lib->GetDataFromINI(_T("Trigger"), _T("StopB"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_PERIOD_F, lib->GetDataFromINI(_T("Trigger"), _T("PeriodF"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_PERIOD_B, lib->GetDataFromINI(_T("Trigger"), _T("PeriodB"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_COUNT_F, lib->GetDataFromINI(_T("Trigger"), _T("CountF"), lib->GetData(_T("inifile"))));
	SetDlgItemText(IDC_EDIT_TRIGGER_COUNT_B, lib->GetDataFromINI(_T("Trigger"), _T("CountB"), lib->GetData(_T("inifile"))));

	// Auto Mode 시 최소화 시작
	if (!theApp.IsManualMode())
		ShowWindow(SW_MINIMIZE);

	// Icon 변경
	m_bTrayStatus = FALSE;
	TraySetting();	

	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);	

	return TRUE;
}

void CVSCameraTaskDlg::OnPaint()
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

HCURSOR CVSCameraTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVSCameraTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	BOOL bHide = FALSE;
	BOOL bClose = FALSE;

	if(nID == SC_CLOSE	)
	{
		bHide = TRUE;
//#if defined(_DEBUG)
		if ( (GetKeyState(VK_SHIFT) < 0) && (AfxMessageBox(_T("정말 종료하시겠습니까?"), MB_YESNO|MB_ICONQUESTION)==IDYES) ) 
		{
			bClose = TRUE;
		}
//#else
//		bClose = TRUE;
//#endif
	}
	else if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
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
		ShowWindow(SW_HIDE);
		return;

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}

}

void CVSCameraTaskDlg::TraySetting(void)
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

BOOL CVSCameraTaskDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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

	if (pMsg->message == WM_KEYDOWN)
	{
		/*if (pMsg->wParam == VK_RETURN)
		{
			int editboxID = GetFocus()->GetDlgCtrlID();
			GetDlgItem(editboxID)->SendMessage(WM_KILLFOCUS, NULL);
		}*/
	}

	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);

}

LRESULT CVSCameraTaskDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	return 0L;
}

void CVSCameraTaskDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//리소스 해제
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_ICON1;

	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void CVSCameraTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	HWND hButton = NULL;

	switch (nIDEvent)
	{
	case eTIMER_UpdateTaskStatus:
		if (!theApp.IsManualMode())
			SetDlgItemText(IDC_BTN_AUTO_MANUAL, _T("Auto"));
		else
			SetDlgItemText(IDC_BTN_AUTO_MANUAL, _T("Manual"));
		//hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_AUTO_MANUAL);
		//::InvalidateRect(hButton, NULL, FALSE);
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_IPC_CONNECT);
		::InvalidateRect(hButton, NULL, FALSE);
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_SHARED_MEM);
		::InvalidateRect(hButton, NULL, FALSE);
		hButton = ::GetDlgItem(m_hWnd, IDC_BTN_CAMERA);
		::InvalidateRect(hButton, NULL, FALSE);
		hButton = ::GetDlgItem(m_hWnd, IDC_BTN_TRIGGER);
		::InvalidateRect(hButton, NULL, FALSE);
		break;
	}	

	CDialogEx::OnTimer(nIDEvent);
}

void CVSCameraTaskDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// nIDCtl = IDC값
	switch(nIDCtl)
	{
		case IDC_BTN_AUTO_MANUAL:
		{
			if (!theApp.IsManualMode())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				m_fnChangeBtnColor(lpDrawItemStruct, DARK_GRAY, WHITE);
			}
		}
		break;
		case IDC_BTN_IPC_CONNECT:
		{
			if (theApp.GetIPCState())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				if (!theApp.IsManualMode())
					m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_RED, DARK_RED);
				else
					m_fnChangeBtnColor(lpDrawItemStruct, DARK_GRAY, WHITE);
			}
		}
		break;
		case IDC_BTN_SHARED_MEM:
		{
			if (theApp.GetSMemState())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				if (!theApp.IsManualMode())
					m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_RED, DARK_RED);
				else
					m_fnChangeBtnColor(lpDrawItemStruct, DARK_GRAY, WHITE);
			}
		}
		break;
		case IDC_BTN_CAMERA:
		{
			if (theApp.GetCameraState())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				if (!theApp.IsManualMode())
					m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_RED, DARK_RED);
				else
					m_fnChangeBtnColor(lpDrawItemStruct, DARK_GRAY, WHITE);
			}
		}
		case IDC_BTN_TRIGGER:
		{
			if (((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->IsConnected())
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

	if (((CDalsaLineCamera*)theApp.m_pCamera)->m_GrabFlag)
		GetDlgItem(IDC_BUTTON_CAM_GRABSTART)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BUTTON_CAM_GRABSTART)->EnableWindow(TRUE);

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CVSCameraTaskDlg::m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText)
{
	CDC dc;
	RECT rect;
	dc.Attach(lpDrawItemStruct->hDC);					//버튼의 dc구하기
	rect = lpDrawItemStruct->rcItem;					//버튼영역 구하기
	dc.Draw3dRect(&rect,WHITE,BLACK);					//버튼의 외곽선 그리기

	dc.FillSolidRect(&rect, colorBtn);					//버튼색상
	dc.SetBkColor(colorBtn);							//text의 백그라운드 색상
	dc.SetTextColor(colorText);							//texttort

	UINT state = lpDrawItemStruct->itemState;			//버튼상태구하기
	if((state &ODS_SELECTED))
	{
		dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);
	}
	else
	{
		dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
	}

	TCHAR buffer[MAX_PATH];											//버튼의 text를 얻기위한 임시버퍼
	ZeroMemory(buffer,MAX_PATH);									//버퍼초기화
	::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);	//버튼의 text얻기
	dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);	//버튼의 text넣기
	dc.Detach();													//버튼의 dc 풀어주기
}

// 콤보박스 선택했을때 Cam Parameter 적용되도록 하는 함수
void CVSCameraTaskDlg::OnCbnSelchangeComboCam()
{
	int comboID = GetFocus()->GetDlgCtrlID();
	switch (comboID)
	{
	case IDC_COMBO_CAM_TRIMODE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("t", m_comboCamTriMode.GetCurSel());
		break;
	case IDC_COMBO_CAM_VDOUT:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("v", m_comboCamVdOut.GetCurSel());
		break;
	case IDC_COMBO_CAM_TAP:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("tap", m_comboCamTap.GetCurSel() + 1);
		break;
	case IDC_COMBO_CAM_TEST:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("T", m_comboCamTest.GetCurSel());
		break;
	}
}

// 체크박스 체크/해제했을때 Cam Parameter 적용되도록 하는 함수
void CVSCameraTaskDlg::OnBnClickedCheckCamReverse()
{
	int checkID = GetFocus()->GetDlgCtrlID();
	switch (checkID)
	{
	case IDC_CHECK_CAM_REVERSE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("rev", m_checkReverse.GetCheck());
		break;
	}
}

// 버튼 클릭했을때 Cam Parameter 적용되도록 하는 함수
void CVSCameraTaskDlg::OnBnClickedButtonCam()
{
	DalsaLineParam* param = &DalsaLineParam::Instance();

	CString VirID;
	struct tm curr_tm;
	time_t curr_time = time(nullptr);
	_localtime64_s(&curr_tm, &curr_time);
	VirID.Format(_T("%4d%02d%02d-%02d%02d%02d"), curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday, curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
	
	int buttonID = GetFocus()->GetDlgCtrlID();
	switch (buttonID)
	{
	case IDC_BUTTON_CAM_LOAD:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->LoadAllProperty();
		break;
	case IDC_BUTTON_CAM_SAVE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SaveAllProperty();
		break;
	case IDC_BUTTON_CAM_STA:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->GetAllProperty();
		SetDlgItemText(IDC_EDIT_INI_FILE_PATH, theApp.GetInitFilePath());
		SetDlgItemText(IDC_EDIT_CAM_MODEL, param->GetParamStr(_T("Type")));
		SetDlgItemText(IDC_EDIT_CAM_GRB_NO, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->FrameGrabberNo));
		SetDlgItemText(IDC_EDIT_CAM_DIG_CH, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->DigitizerChannel));
		SetDlgItemText(IDC_EDIT_CAM_DCF, ((CDalsaLineCamera*)theApp.m_pCamera)->DcfFile);
		SetDlgItemText(IDC_EDIT_CAM_PORT, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->CameraPort));
		SetDlgItemText(IDC_EDIT_CAM_BAUD, ToString(((CDalsaLineCamera*)theApp.m_pCamera)->CameraBaud));
		SetDlgItemText(IDC_EDIT_CAM_GR, param->GetParamStr(_T("r")));
		SetDlgItemText(IDC_EDIT_CAM_GRFO, param->GetParamStr(_T("grfo")));
		SetDlgItemText(IDC_EDIT_CAM_GRFE, param->GetParamStr(_T("grfe")));
		SetDlgItemText(IDC_EDIT_CAM_GRRO, param->GetParamStr(_T("grro")));
		SetDlgItemText(IDC_EDIT_CAM_GRRE, param->GetParamStr(_T("grre")));
		SetDlgItemText(IDC_EDIT_CAM_GG, param->GetParamStr(_T("g")));
		SetDlgItemText(IDC_EDIT_CAM_GGFO, param->GetParamStr(_T("ggfo")));
		SetDlgItemText(IDC_EDIT_CAM_GGFE, param->GetParamStr(_T("ggfe")));
		SetDlgItemText(IDC_EDIT_CAM_GGRO, param->GetParamStr(_T("ggro")));
		SetDlgItemText(IDC_EDIT_CAM_GGRE, param->GetParamStr(_T("ggre")));
		SetDlgItemText(IDC_EDIT_CAM_GB, param->GetParamStr(_T("b")));
		SetDlgItemText(IDC_EDIT_CAM_GBFO, param->GetParamStr(_T("gbfo")));
		SetDlgItemText(IDC_EDIT_CAM_GBFE, param->GetParamStr(_T("gbfe")));
		SetDlgItemText(IDC_EDIT_CAM_GBRO, param->GetParamStr(_T("gbro")));
		SetDlgItemText(IDC_EDIT_CAM_GBRE, param->GetParamStr(_T("gbre")));
		SetDlgItemText(IDC_EDIT_CAM_WBR, param->GetParamStr(_T("WBr")));
		SetDlgItemText(IDC_EDIT_CAM_WBG, param->GetParamStr(_T("WBg")));
		SetDlgItemText(IDC_EDIT_CAM_WBB, param->GetParamStr(_T("WBb")));
		SetDlgItemText(IDC_EDIT_CAM_EXP, param->GetParamStr(_T("i")));
		SetDlgItemText(IDC_EDIT_CAM_LINEDELAY, param->GetParamStr(_T("d")));
		m_comboCamTriMode.SetCurSel(param->GetParamInt(_T("t")));
		m_comboCamVdOut.SetCurSel(param->GetParamInt(_T("v")));
		m_comboCamTap.SetCurSel(param->GetParamInt(_T("tap")) - 1);
		m_comboCamTest.SetCurSel(param->GetParamInt(_T("T")));
		m_checkReverse.SetCheck(param->GetParamInt(_T("rev")));
		break;
	case IDC_BUTTON_CAM_GRABSTART:
		((CDalsaLineCamera*) theApp.m_pCamera)->StartGrab(VirID, VirID, _T("Test"), 65, false, true);
		//((CDalsaLineCamera*)theApp.m_pCamera)->StartGrab(0,52,false, true);
		break;
	case IDC_BUTTON_CAM_GRABSTOP:
		((CDalsaLineCamera*)theApp.m_pCamera)->StopGrab(65);
		break;
	}
}

void CVSCameraTaskDlg::OnEnChangeEditCam()
{
	if (GetFocus() == NULL)
		return;
	int editboxID = GetFocus()->GetDlgCtrlID();
	int value = GetDlgItemInt(editboxID);
	switch (editboxID)
	{
	case IDC_EDIT_CAM_GR:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("r", value);
		break;
	case IDC_EDIT_CAM_GRFO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("grfo", value);
		break;
	case IDC_EDIT_CAM_GRFE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("grfe", value);
		break;
	case IDC_EDIT_CAM_GRRO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("grro", value);
		break;
	case IDC_EDIT_CAM_GRRE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("grre", value);
		break;
	case IDC_EDIT_CAM_GG:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("g", value);
		break;
	case IDC_EDIT_CAM_GGFO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("ggfo", value);
		break;
	case IDC_EDIT_CAM_GGFE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("ggfe", value);
		break;
	case IDC_EDIT_CAM_GGRO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("ggro", value);
		break;
	case IDC_EDIT_CAM_GGRE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("ggre", value);
		break;
	case IDC_EDIT_CAM_GB:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("b", value);
		break;
	case IDC_EDIT_CAM_GBFO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("gbfo", value);
		break;
	case IDC_EDIT_CAM_GBFE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("gbfe", value);
		break;
	case IDC_EDIT_CAM_GBRO:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("gbro", value);
		break;
	case IDC_EDIT_CAM_GBRE:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("gbre", value);
		break;
	case IDC_EDIT_CAM_WBR:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("WBr", value);
		break;
	case IDC_EDIT_CAM_WBG:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("WBg", value);
		break;
	case IDC_EDIT_CAM_WBB:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("WBb", value);
		break;
	case IDC_EDIT_CAM_EXP:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("i", value);
		break;
	case IDC_EDIT_CAM_LINEDELAY:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Camera->SetProperty("d", value);
		break;
	}
}

void CVSCameraTaskDlg::OnBnClickedButtonTrigger()
{
	YskimLib *lib = &YskimLib::Instance();
	int startPosF = GetDlgItemInt(IDC_EDIT_TRIGGER_STARTPOS_F);
	int startPosB = GetDlgItemInt(IDC_EDIT_TRIGGER_STARTPOS_B);
	int stopPosF = GetDlgItemInt(IDC_EDIT_TRIGGER_STOPPOS_F);
	int stopPosB = GetDlgItemInt(IDC_EDIT_TRIGGER_STOPPOS_B);
	int periodF = GetDlgItemInt(IDC_EDIT_TRIGGER_PERIOD_F);
	int periodB = GetDlgItemInt(IDC_EDIT_TRIGGER_PERIOD_B);
	int countF = GetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_F);
	int countB = GetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_B);
	int buttonID = GetFocus()->GetDlgCtrlID();
	switch (buttonID)
	{
	case IDC_BUTTON_TRIGGER_INIT_F:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->StopTriggerGen0();
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->TriggerOrigin0();
		break;
	case IDC_BUTTON_TRIGGER_INIT_B:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->StopTriggerGen0();
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->TriggerOrigin0();
		break;
	case IDC_BUTTON_TRIGGER_SET_F:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->TriggerStartPositionChannel0(
			((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartF, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopF, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodF);
		break;
	case IDC_BUTTON_TRIGGER_SET_B:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->TriggerStartPositionChannel0(
			((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartB, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopB, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodB);
		break;
	case IDC_BUTTON_TRIGGER_SCAN_F:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->StartForwardScan();
		break;
	case IDC_BUTTON_TRIGGER_SCAN_B:
		((CDalsaLineCamera*)theApp.m_pCamera)->m_Trigger->StartBackwardScan();
		break;
	case IDC_BUTTON_TRIGGER_SAVE:
		lib->SetDataToINI(_T("Trigger"), _T("StartF"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartF, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("StartB"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartB, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("StopF"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopF, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("StopB"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopB, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("PeriodF"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodF, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("PeriodB"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodB, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("CountF"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountF, lib->GetData(_T("inifile")));
		lib->SetDataToINI(_T("Trigger"), _T("CountB"), ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountB, lib->GetData(_T("inifile")));
		break;
	}
}

void CVSCameraTaskDlg::OnEnChangeEditTrigger()
{
	int heightPixel = ((CDalsaLineCamera*)theApp.m_pCamera)->m_lDigSizeY;
	if (heightPixel < 0 || heightPixel > 999999)
		heightPixel = 1000;
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartF = GetDlgItemInt(IDC_EDIT_TRIGGER_STARTPOS_F);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartB = GetDlgItemInt(IDC_EDIT_TRIGGER_STARTPOS_B);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodF = GetDlgItemInt(IDC_EDIT_TRIGGER_PERIOD_F);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodB = GetDlgItemInt(IDC_EDIT_TRIGGER_PERIOD_B);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountF = GetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_F);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountB = GetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_B);
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopF =
		((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartF + ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodF * ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountF * heightPixel;
	((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopB =
		((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStartB + ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerPeriodB * ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountB * heightPixel;
	SetDlgItemInt(IDC_EDIT_TRIGGER_STOPPOS_F, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopF);
	SetDlgItemInt(IDC_EDIT_TRIGGER_STOPPOS_B, ((CDalsaLineCamera*)theApp.m_pCamera)->TriggerStopB);

	if (((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountF > BUFFER_COUNT)
		SetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_F, BUFFER_COUNT);
	if (((CDalsaLineCamera*)theApp.m_pCamera)->TriggerCountB > BUFFER_COUNT)
		SetDlgItemInt(IDC_EDIT_TRIGGER_COUNT_B, BUFFER_COUNT);
}
