
// VSCameraTaskDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VSCameraTask.h"
#include "VSDalsaCameraTaskDlg.h"
#include "afxdialogex.h"
#include "define.h"
#include "version.h"
#include "CPUTimer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVSCameraTaskDlg 대화 상자

CVSCameraTaskDlg::CVSCameraTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSCameraTaskDlg::IDD, pParent)
	, m_nGV(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVSCameraTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_MODEL, m_cmbCamModel);
	DDX_Control(pDX, IDC_CMB_COLOR, m_cmbCamColor);
	DDX_Control(pDX, IDC_CMB_BAUDRATE, m_cmbBaudrate);
	DDX_Control(pDX, IDC_CMB_TRIG_MODE, m_cmbTrigMode);
	DDX_Control(pDX, IDC_CMB_PS_MODE, m_cmbPSMode);
	DDX_Control(pDX, IDC_CMB_GRABBER_NUMBER, m_cmbGrabberNum);
	DDX_Control(pDX, IDC_CMB_GRABBER_CHANNEL, m_cmbGrabberCH);
	DDX_Control(pDX, IDC_CHK_VERTICAL, m_chkVertical);
	DDX_Control(pDX, IDC_CHK_HORIZONTAL, m_chkHorizontal);
	DDX_Text(pDX, IDC_EDIT_GV, m_nGV);
	DDX_Control(pDX, IDC_EDIT_COND_INTERVAL, m_ctrlInterval);
	DDX_Control(pDX, IDC_EDIT_TEMP, m_ctrlTemperature);
	DDX_Control(pDX, IDC_EDIT_FAN_STATUS, m_ctrlFanRPM);
}

BEGIN_MESSAGE_MAP(CVSCameraTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BTN_SAVE_IMAGE, &CVSCameraTaskDlg::OnBnClickedBtnSaveImage)
	ON_BN_CLICKED(IDC_BTN_INI_FILE_PATH, &CVSCameraTaskDlg::OnBnClickedBtnIniFilePath)
	ON_BN_CLICKED(IDC_BTN_DCF_FILE_PATH, &CVSCameraTaskDlg::OnBnClickedBtnDcfFilePath)
	ON_BN_CLICKED(IDC_BTN_APPLY_ALL, &CVSCameraTaskDlg::OnBnClickedBtnApplyAll)
	ON_BN_CLICKED(IDC_BTN_OPEN_PORT, &CVSCameraTaskDlg::OnBnClickedBtnOpenPort)
	ON_BN_CLICKED(IDC_BTN_DCF_APPLY, &CVSCameraTaskDlg::OnBnClickedBtnDcfApply)
	ON_BN_CLICKED(IDC_BTN_SETTING_APPLY, &CVSCameraTaskDlg::OnBnClickedBtnSettingApply)
	ON_CBN_SELCHANGE(IDC_CMB_GRABBER_NUMBER, &CVSCameraTaskDlg::OnCbnSelchangeCmbGrabberNumber)
	ON_BN_CLICKED(IDC_BTN_LIVE, &CVSCameraTaskDlg::OnBnClickedBtnLive)
	ON_BN_CLICKED(IDC_BTN_GRAB, &CVSCameraTaskDlg::OnBnClickedBtnGrab)
	ON_BN_CLICKED(IDC_BUTTON1, &CVSCameraTaskDlg::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_CMB_MODEL, &CVSCameraTaskDlg::OnCbnSelchangeCmbModel)
	ON_BN_CLICKED(IDC_BTN_GET_CAM_COND, &CVSCameraTaskDlg::OnBnClickedGetCameraCondition)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_GRAB, &CVSCameraTaskDlg::OnBnClickedButtonAutoGrab)
	ON_BN_CLICKED(IDC_BTN_SETTING_APPLY2, &CVSCameraTaskDlg::OnBnClickedBtnSettingApply2)
END_MESSAGE_MAP()


// CVSCameraTaskDlg 메시지 처리기

BOOL CVSCameraTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// Auto Mode 시 최소화 시작
	if (!theApp.IsManualMode())
		ShowWindow(SW_MINIMIZE);

	// 버전 표기
	CString title;
	title.Format(_T("Dalsa Camera %s"), _Version);
	SetWindowText(title);

	// Icon 변경
	m_bTrayStatus = FALSE;
	TraySetting();	

	m_fnInitUIControls();

	if (theApp.IsManualMode())
		m_fnSetDefaultUIControls();
	else
		m_fnSetAutoUIControls(theApp.GetInitFilePath());

	// Task 상태 갱신 Timer
	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);

	// Camera Monitoring Thread 생성
	//m_bMonitoring = true;
	//m_pThreadMonitoring = AfxBeginThread(ThreadMonitoring, this);

	//m_pDlgAutoGrab = NULL;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

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

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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

// 트레이 아이콘 추가
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

	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;


	return CDialogEx::PreTranslateMessage(pMsg);

}

// 트레이 아이콘 추가
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

	m_bMonitoring = false;
	Sleep(100);
	m_pThreadMonitoring = NULL;
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
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_CAMERA);
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
		break;
	}

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

void CVSCameraTaskDlg::OnBnClickedBtnSaveImage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog* pDlg = NULL;
	if (theApp.m_pCamera->GetImageBitrate() == 8)
		pDlg = new CFileDialog( TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("BMP FILES (*.BMP)|*.bmp|All Files (*.*)|*.*||") );
	else
		pDlg = new CFileDialog( TRUE, _T("tiff"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("TIFF FILES (*.TIFF)|*.tiff|All Files (*.*)|*.*||") );

	if(pDlg->DoModal() == IDOK)
	{
		CString strPath;
		strPath.Format(_T("%s"), pDlg->GetPathName());
		//strPath.Format(_T("%s"), pDlg->GetFileName() );
		theApp.m_pCamera->CameraExpose();
		theApp.m_pCamera->WaitGrabEnd();
		theApp.m_pCamera->SaveGrabImage(strPath);
	}
	SAFE_DELETE(pDlg);
}


void CVSCameraTaskDlg::OnBnClickedBtnIniFilePath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	CFileDialog dlg( TRUE, _T("ini"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("INI FILES (*.INI)|*.ini|All Files (*.*)|*.*||") );

	if(dlg.DoModal() == IDOK)
	{
		CString strPath;
		strPath.Format(_T("%s"), dlg.GetPathName() );
		SetDlgItemText(IDC_EDIT_INI_FILE_PATH, strPath);
	}
}


void CVSCameraTaskDlg::OnBnClickedBtnDcfFilePath()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg( TRUE, _T("dcf"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("DCF FILES (*.DCF)|*.dcf|All Files (*.*)|*.*||") );

	if(dlg.DoModal() == IDOK)
	{
		CString strPath;
		strPath.Format(_T("%s"), dlg.GetPathName() );
		SetDlgItemText(IDC_EDIT_DCF_FILE_PATH, strPath);
	}
}

void CVSCameraTaskDlg::m_fnAddCmbString(CComboBox& ComboBox, const CString* strItemArr, int nItemCount)
{
	for (int nCnt=0; nCnt < nItemCount; nCnt++)
	{
		ComboBox.AddString(strItemArr[nCnt]);
	}
}

void CVSCameraTaskDlg::OnBnClickedBtnApplyAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strIniFilePath;
	GetDlgItemText(IDC_EDIT_INI_FILE_PATH, strIniFilePath);
	
	BOOL bRet = theApp.m_pCamera->InitializeCamera(strIniFilePath);
	theApp.SetCameraState(bRet);
	m_fnSetAutoUIControls(strIniFilePath);
}


void CVSCameraTaskDlg::OnBnClickedBtnOpenPort()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strPortNum;
	GetDlgItemText(IDC_EDIT_COM_PORT, strPortNum);

	BOOL bRet = theApp.m_pCamera->
		OpenCameraComPort(_ttoi(strPortNum), _ttoi(strBaudrate[m_cmbBaudrate.GetCurSel()]), (eCamModel)m_cmbCamModel.GetCurSel());

	if (bRet)
	{
		CString strVal;
		strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
		SetDlgItemText(IDC_EDIT_EXP_ON, strVal);
		strVal.Format(_T("%.3lf"), theApp.m_pCamera->GetAnalogGain());
		SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
		m_cmbPSMode.SetCurSel(theApp.m_pCamera->GetSequenceMode());
		m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
	}
}


void CVSCameraTaskDlg::OnBnClickedBtnDcfApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strIniFilePath;
	GetDlgItemText(IDC_EDIT_DCF_FILE_PATH, strIniFilePath);
	BOOL bRet = theApp.m_pCamera->
		InitializeGrabber(m_cmbGrabberNum.GetCurSel(), eSolios, m_cmbGrabberCH.GetCurSel(), strIniFilePath);		// Cam.Task UI Board Model 추가 작업 필요

	if (bRet)
		theApp.SetCameraState(TRUE);
	else
		theApp.SetCameraState(FALSE);
}


void CVSCameraTaskDlg::OnBnClickedBtnSettingApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strVal;
	theApp.m_pCamera->SetTriggerMode(m_cmbTrigMode.GetCurSel());
	theApp.m_pCamera->SetSequenceMode(m_cmbPSMode.GetCurSel());
	GetDlgItemText(IDC_EDIT_EXP_ON, strVal);
	theApp.m_pCamera->SetExposureTime(_ttof(strVal));
	GetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
	theApp.m_pCamera->SetAnalogGain(_ttoi(strVal));
}

void CVSCameraTaskDlg::m_fnInitUIControls()
{
	TCHAR strVal[2];
	CString strKey;

	m_fnAddCmbString(m_cmbCamModel,	strCamModel,	(int)eModel_Count);
	m_fnAddCmbString(m_cmbCamColor,	strCamColor,	(int)eColor_Count);
	m_fnAddCmbString(m_cmbBaudrate,	strBaudrate,	(int)eBaudrate_Count);
	if (m_cmbCamModel.GetCurSel() == (int)eVA47M5_Series || m_cmbCamModel.GetCurSel() == (int)eVNP_MX_Series)
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	2);
	else
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	(int)eTrigMode_Count);
	m_fnAddCmbString(m_cmbPSMode,	strPSMode,		(int)ePSMode_Count);

	for (int nCnt=0;nCnt<NUM_FRAME_GRABBER; nCnt++)
	{
		strKey.Format(_T("Frame Grabber_%d"), nCnt);
		GetPrivateProfileString(_T("Grabber_Board"), strKey ,_T("F"), strVal, 2, DEVICE_FILE_PATH);
		if (!_tcscmp(strVal, _T("T")))
			m_cmbGrabberNum.AddString(strKey);
	}	
}

void CVSCameraTaskDlg::OnCbnSelchangeCmbGrabberNumber()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fnRefreshCmbChannel();
}

void CVSCameraTaskDlg::m_fnSetDefaultUIControls()
{
	SetDlgItemText(IDC_STATIC_CAM_NUM, _T("Manual Camera"));
// 	m_cmbCamModel.SetCurSel(0);
// 	m_cmbCamColor.SetCurSel(0);
// 	SetDlgItemText(IDC_EDIT_COM_PORT, _T("1"));
// 	m_cmbBaudrate.SetCurSel(0);
// 	m_cmbGrabberNum.SetCurSel(0);
// 	m_fnRefreshCmbChannel();
// 	m_cmbGrabberCH.SetCurSel(0);
// 	SetDlgItemText(IDC_EDIT_EXP_ON, _T("100.000"));
// 	SetDlgItemText(IDC_EDIT_EXP_OFF, _T("0"));
}

void CVSCameraTaskDlg::m_fnSetAutoUIControls(CString strInitFilePath)
{
	TCHAR strVal[256];
	CString strTemp;

	SetDlgItemText(IDC_EDIT_INI_FILE_PATH, strInitFilePath);
	
	strTemp.Format(_T("Camera#%d"), theApp.GetTaskNum());
	SetDlgItemText(IDC_STATIC_CAM_NUM, strTemp);
	GetPrivateProfileString(_T("Settings"), _T("Init File Path") ,_T("1"), strVal, 100, strInitFilePath);
	SetDlgItemText(IDC_EDIT_DCF_FILE_PATH, strVal);
	m_cmbGrabberNum.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Frame Grabber No"), 100, strInitFilePath));	
	m_fnRefreshCmbChannel();
	m_cmbGrabberCH.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Digitizer Channel"), 100, strInitFilePath));
	m_cmbCamModel.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Camera Model"), 100, strInitFilePath));
	OnCbnSelchangeCmbModel();
	if (theApp.m_pCamera->GetImageBandwidth() == 1)
		m_cmbCamColor.SetCurSel(0);
	else
		m_cmbCamColor.SetCurSel(1);
	GetPrivateProfileString(_T("Settings"), _T("Serial Port") ,_T("1"), strVal, 100, strInitFilePath);
	SetDlgItemText(IDC_EDIT_COM_PORT, strVal);
	GetPrivateProfileString(_T("Settings"), _T("Serial Baudrate") ,_T("115200"), strVal, 100, strInitFilePath);
	for (int nCnt=0; nCnt<eBaudrate_Count; nCnt++)
	{
		if (strBaudrate[nCnt].Compare(strVal) == 0)
		{
			m_cmbBaudrate.SetCurSel(nCnt);
			break;
		}
	}
	// INI File 값 사용 안함
 	//m_cmbTrigMode.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Trigger Mode"), 100, strInitFilePath));
	//GetPrivateProfileString(_T("Settings"), _T("Exposure On Time") ,_T("1"), strVal, 100, strInitFilePath);
	//SetDlgItemText(IDC_EDIT_EXP_ON, strVal);
	//GetPrivateProfileString(_T("Settings"), _T("Analog Gain") ,_T("1"), strVal, 100, strInitFilePath);
	//SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
	//m_cmbPSMode.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Pixel Shift Mode"), 100, strInitFilePath));

	m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
	
	strTemp.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
	SetDlgItemText(IDC_EDIT_EXP_ON, strTemp);
	SetDlgItemText(IDC_EDIT_EXP_OFF, _T("0"));
	
	strTemp.Format(_T("%.3lf"), theApp.m_pCamera->GetAnalogGain());
	SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strTemp);
	
	m_cmbPSMode.SetCurSel(theApp.m_pCamera->GetSequenceMode());
	GetPrivateProfileString(_T("Settings"), _T("Use Horizon Pixel Shift") ,_T("T"), strVal, 100, DEVICE_FILE_PATH);
	if (!_tcscmp(strVal, _T("T")))
		m_chkHorizontal.SetCheck(1);
	else
		m_chkHorizontal.SetCheck(0);
	GetPrivateProfileString(_T("Settings"), _T("Use Vertical Pixel Shift") ,_T("T"), strVal, 100, DEVICE_FILE_PATH);
	if (!_tcscmp(strVal, _T("T")))
		m_chkVertical.SetCheck(1);
	else
		m_chkVertical.SetCheck(0);	
}

void CVSCameraTaskDlg::m_fnRefreshCmbChannel()
{
	TCHAR strVal[2];
	CString strSection, strKey;

	m_cmbGrabberCH.Clear();

	strSection.Format(_T("Frame Grabber_%d"), m_cmbGrabberNum.GetCurSel());

	for (int nCnt=0;nCnt<NUM_INSP_CAMERA; nCnt++)
	{		
		strKey.Format(_T("Insp Camera_%d"), nCnt);
		GetPrivateProfileString(strSection, strKey ,_T("F"), strVal, 2, DEVICE_FILE_PATH);
		if (!_tcscmp(strVal, _T("T")))
			m_cmbGrabberCH.AddString(strKey);
	}
}

void CVSCameraTaskDlg::OnBnClickedBtnLive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strBtnCaption;
	GetDlgItemText(IDC_BTN_LIVE, strBtnCaption);

	if (strBtnCaption.Compare(_T("Live")))
	{
		SetDlgItemText(IDC_BTN_LIVE, _T("Live"));
		theApp.m_pCamera->StopLiveGrab();		
		GetDlgItem(IDC_BTN_GRAB)->EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_BTN_LIVE, _T("Stop"));
		theApp.m_pCamera->DisplayLive();
		GetDlgItem(IDC_BTN_GRAB)->EnableWindow(FALSE);
	}
}


void CVSCameraTaskDlg::OnBnClickedBtnGrab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pCamera->DisplayGrabImage();
}


// TEST 에 사용할 버튼
void CVSCameraTaskDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 	UpdateData( TRUE );
// 
// 	theApp.m_pCamera->TestFunc( m_nGV );
	theApp.m_pCamera->GetTemperature();
}


void CVSCameraTaskDlg::OnCbnSelchangeCmbModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_cmbTrigMode.ResetContent();
	if (m_cmbCamModel.GetCurSel() == (int)eVA47M5_Series || m_cmbCamModel.GetCurSel() == (int)eVNP_MX_Series)
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	2);
	else
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	(int)eTrigMode_Count);
}


void CVSCameraTaskDlg::OnBnClickedGetCameraCondition()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strVal;
	strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
	SetDlgItemText(IDC_EDIT_EXP_ON, strVal);
	strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetAnalogGain());
	SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
	m_cmbPSMode.SetCurSel(theApp.m_pCamera->GetSequenceMode());
	m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
}


void CVSCameraTaskDlg::OnBnClickedButtonAutoGrab()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//m_pDlgAutoGrab = new CAutoGrab();

	//m_pDlgAutoGrab->DoModal();

	//SAFE_DELETE(m_pDlgAutoGrab);
}

UINT CVSCameraTaskDlg::ThreadMonitoring(LPVOID pParam)
{
	CVSCameraTaskDlg* pThis = (CVSCameraTaskDlg*)pParam;
	const int DELAY = 10;
	const int INTERVAL = theApp.m_pCamera->GetMonitoringInterval();

	TCHAR strInterval[10] = {0, };
	_itot_s(INTERVAL, strInterval, 10);
	pThis->m_ctrlInterval.SetWindowText(strInterval);

	// Check Interval 이 1초 미만으로 설정되어 있으면 동작 안함
 	if (INTERVAL < 1000)
 		return 0;

	int nCount = 0;		// Thread 종료 즉각 반응하기 위함
	while(pThis->m_bMonitoring)
	{
		if (nCount % (INTERVAL / DELAY) == 0)
		{
			CCPUTimer tactMonitoring;
			tactMonitoring.Start();
			nCount = 0;
			CString strTemperature = _T(""), strCCDTemperature = _T(""), strFanStatus = _T(""), strFanRPM = _T("");
			strTemperature.Format(_T("%.2f"), theApp.m_pCamera->GetTemperature());
			strCCDTemperature.Format(_T("%.2f"), theApp.m_pCamera->GetCCDTemperature());
			strFanStatus.Format(_T("%s"), theApp.m_pCamera->GetFanStatus()?_T("ON"):_T("OFF"));
			strFanRPM.Format(_T("%d"), theApp.m_pCamera->GetFanRPM());			
			pThis->m_ctrlTemperature.SetWindowText(strTemperature);
			pThis->m_ctrlFanRPM.SetWindowText(strFanStatus);
			theApp.m_fnWriteMonitoringLog(_T("Cam Temperature : %s℃ (CCD : %s℃) / Fan : %s (RPM : %s) <TACT : %.3f>"), strTemperature, strCCDTemperature, strFanStatus, strFanRPM, tactMonitoring.Stop(false));
		}
		nCount++;
		Sleep(DELAY);		
	}
	return 0;
}

void CVSCameraTaskDlg::OnBnClickedBtnSettingApply2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// Camera Monitoring Interval
	TCHAR strInterval[10] = {0, };
	m_ctrlInterval.GetWindowText(strInterval, 10);
	int nInterval = _ttoi(strInterval);
	WritePrivateProfileString(_T("Settings"), _T("Monitoring Interval"), strInterval, theApp.m_pCamera->GetInitFilePath());
	theApp.m_pCamera->SetMonitoringInterval(nInterval);
	m_bMonitoring = false;
	Sleep(100);
	m_bMonitoring = true;
	m_pThreadMonitoring = NULL;
	m_pThreadMonitoring = AfxBeginThread(ThreadMonitoring, this);
}
