
// VSCameraTaskDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "VSCameraTask.h"
#include "VSIMICameraTaskDlg.h"

#include "version.h"
#include "afxdialogex.h"
#include "define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVSCameraTaskDlg ��ȭ ����

CVSCameraTaskDlg::CVSCameraTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSCameraTaskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVSCameraTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_MODEL, m_cmbCamModel);
	DDX_Control(pDX, IDC_CMB_COLOR, m_cmbCamColor);
	DDX_Control(pDX, IDC_CMB_TRIG_MODE, m_cmbTrigMode);

	DDX_Control(pDX, IDC_EDIT_RED_COEFF, m_RedCoeff);
	DDX_Control(pDX, IDC_EDIT_BLUE_COEFF, m_BlueCoeff);
	DDX_Control(pDX, IDC_EDIT_GREEN_COEFF, m_GreenCoeff);
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
	ON_BN_CLICKED(IDC_BTN_CALC_RGB, &CVSCameraTaskDlg::OnBnClickedBtnCalcRgb)
	ON_BN_CLICKED(IDC_BTN_SAVE_WB_VALUE, &CVSCameraTaskDlg::OnBnClickedBtnSaveWbValue)
	ON_BN_CLICKED(IDC_BTN_GET_WB, &CVSCameraTaskDlg::OnBnClickedBtnGetWb)
END_MESSAGE_MAP()


// CVSCameraTaskDlg �޽��� ó����

BOOL CVSCameraTaskDlg::OnInitDialog()
{


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	TCHAR strVal[11];
	//	CString strTemp;
	float WhiteBalance[3] = {0,};

	GetPrivateProfileString(_T("White Balance"), _T("R") ,_T("0"), strVal, sizeof(strVal), theApp.GetInitFilePath());
	//strTemp.Format(_T("Camera#%s"), strVal);			
	WhiteBalance[0] = (float)_ttof(strVal);
	GetPrivateProfileString(_T("White Balance"), _T("G") ,_T("0"), strVal, sizeof(strVal), theApp.GetInitFilePath());
	WhiteBalance[1] = (float)_ttof(strVal);
	GetPrivateProfileString(_T("White Balance"), _T("B") ,_T("0"), strVal, sizeof(strVal), theApp.GetInitFilePath());
	WhiteBalance[2] = (float)_ttof(strVal);

	theApp.m_pCamera->SetWhiteBalance(WhiteBalance);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// ���� ǥ��
	CString title;
	title.Format(_T("IMI Camera %s"), _Version);
	SetWindowText(title);

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	// Auto Mode �� �ּ�ȭ ����
	if (!theApp.IsManualMode())
		ShowWindow(SW_MINIMIZE);

	// Icon ����
	m_bTrayStatus = FALSE;
	TraySetting();	

	m_fnInitUIControls();

	if (theApp.IsManualMode())
		m_fnSetDefaultUIControls();
	else
		m_fnSetAutoUIControls(theApp.GetInitFilePath());

	//m_dispLive = new DisplayLive;
	// Task ���� ���� Timer
	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);	

	

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CVSCameraTaskDlg::OnPaint()
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
HCURSOR CVSCameraTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVSCameraTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// Ʈ���� ������ �߰�
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//���ҽ� ����
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_ICON1;

	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void CVSCameraTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// nIDCtl = IDC��
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

void CVSCameraTaskDlg::OnBnClickedBtnSaveImage()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CFileDialog* pDlg = NULL;
	if (theApp.m_pCamera->GetImageBitrate() == 8)
		pDlg = new CFileDialog( TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("BMP FILES (*.BMP)|*.bmp|All Files (*.*)|*.*||") );
	else
		pDlg = new CFileDialog( TRUE, _T("tiff"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("TIFF FILES (*.TIFF)|*.tiff|All Files (*.*)|*.*||") );

	if(pDlg->DoModal() == IDOK)
	{
		CString strPath;
		strPath.Format(_T("%s"), pDlg->GetPathName() );
		theApp.m_pCamera->SaveGrabImage(strPath);
	}
	SAFE_DELETE(pDlg);
}


void CVSCameraTaskDlg::OnBnClickedBtnIniFilePath()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strIniFilePath;
	GetDlgItemText(IDC_EDIT_INI_FILE_PATH, strIniFilePath);
	
	BOOL bRet = theApp.m_pCamera->InitializeCamera(strIniFilePath);
	theApp.SetCameraState(bRet);
	m_fnSetAutoUIControls(strIniFilePath);
}


void CVSCameraTaskDlg::OnBnClickedBtnOpenPort()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	CString strPortNum;
// 	GetDlgItemText(IDC_EDIT_COM_PORT, strPortNum);
// 
// 	BOOL bRet = theApp.m_pCamera->
// 		OpenCameraComPort(_ttoi(strPortNum), _ttoi(strBaudrate[m_cmbBaudrate.GetCurSel()]), (eCamModel)m_cmbCamModel.GetCurSel());
// 
// 	if (bRet)
// 	{
// 		CString strVal;
// 		strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
// 		SetDlgItemText(IDC_EDIT_EXP_ON, strVal);
// 		strVal.Format(_T("%d"), theApp.m_pCamera->GetAnalogGain());
// 		SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
// 		m_cmbPSMode.SetCurSel(theApp.m_pCamera->GetSequenceMode());
// 		m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
// 	}
}


void CVSCameraTaskDlg::OnBnClickedBtnDcfApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	CString strIniFilePath;
// 	GetDlgItemText(IDC_EDIT_DCF_FILE_PATH, strIniFilePath);
// 	BOOL bRet = theApp.m_pCamera->
// 		InitializeGrabber(m_cmbGrabberNum.GetCurSel(), m_cmbGrabberCH.GetCurSel(), strIniFilePath);
// 
// 	if (bRet)
// 		theApp.SetCameraState(TRUE);
// 	else
// 		theApp.SetCameraState(FALSE);
}


void CVSCameraTaskDlg::OnBnClickedBtnSettingApply()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strVal;
	theApp.m_pCamera->SetTriggerMode(m_cmbTrigMode.GetCurSel());
	GetDlgItemText(IDC_EDIT_EXP_ON, strVal);
	theApp.m_pCamera->SetExposureTime(_ttof(strVal));
	GetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
	theApp.m_pCamera->SetAnalogGain(_ttoi(strVal));
}

void CVSCameraTaskDlg::m_fnInitUIControls()
{
	//m_fnAddCmbString(m_cmbTrigMode,	, 2);
}

void CVSCameraTaskDlg::OnCbnSelchangeCmbGrabberNumber()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnRefreshCmbChannel();
}

void CVSCameraTaskDlg::m_fnSetDefaultUIControls()
{
	SetDlgItemText(IDC_STATIC_CAM_NUM, _T("Manual Camera"));
}

void CVSCameraTaskDlg::m_fnSetAutoUIControls(CString strInitFilePath)
{
	TCHAR strVal[256];
	CString strTemp;

	SetDlgItemText(IDC_EDIT_INI_FILE_PATH, strInitFilePath);
	
	GetPrivateProfileString(_T("Settings"), _T("Camera Number") ,_T("1"), strVal, 100, strInitFilePath);
	strTemp.Format(_T("Camera#%s"), strVal);
	SetDlgItemText(IDC_STATIC_CAM_NUM, strTemp);
	GetPrivateProfileString(_T("Settings"), _T("Init File Path") ,_T("1"), strVal, 100, strInitFilePath);
	SetDlgItemText(IDC_EDIT_DCF_FILE_PATH, strVal);
	m_fnRefreshCmbChannel();
	m_cmbCamModel.SetCurSel(GetPrivateProfileInt(_T("Settings"), _T("Camera Model"), 100, strInitFilePath));
	OnCbnSelchangeCmbModel();
	if (theApp.m_pCamera->GetImageBandwidth() == 1)
		m_cmbCamColor.SetCurSel(0);
	else
		m_cmbCamColor.SetCurSel(1);
	GetPrivateProfileString(_T("Settings"), _T("Serial Port") ,_T("1"), strVal, 100, strInitFilePath);
	SetDlgItemText(IDC_EDIT_COM_PORT, strVal);
	GetPrivateProfileString(_T("Settings"), _T("Serial Baudrate") ,_T("115200"), strVal, 100, strInitFilePath);

	m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
	
	strTemp.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
	SetDlgItemText(IDC_EDIT_EXP_ON, strTemp);
	SetDlgItemText(IDC_EDIT_EXP_OFF, _T("0"));
	
	strTemp.Format(_T("%.2lf"), theApp.m_pCamera->GetAnalogGain());
	SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strTemp);

	OnBnClickedBtnGetWb();

	// 2016.02.06 Output Databit 8Bit ����
	theApp.m_pCamera->SetDataBit(8);
	// 2016.02.06 Exposure Source - Pulse Width �� ����
	theApp.m_pCamera->SetExposureSource(1);



}

void CVSCameraTaskDlg::m_fnRefreshCmbChannel()
{

	CString strSection, strKey;

}

void CVSCameraTaskDlg::OnBnClickedBtnLive()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
// 	CString strBtnCaption;
// 	GetDlgItemText(IDC_BTN_LIVE, strBtnCaption);
// 
// 	if (strBtnCaption.Compare(_T("Live")))
// 	{
// 		SetDlgItemText(IDC_BTN_LIVE, _T("Live"));
// 		theApp.m_pCamera->StopLiveGrab();		
// 		GetDlgItem(IDC_BTN_GRAB)->EnableWindow(TRUE);
// 	}
// 	else
// 	{
// 		SetDlgItemText(IDC_BTN_LIVE, _T("Stop"));
// 		theApp.m_pCamera->DisplayLive();
// 		GetDlgItem(IDC_BTN_GRAB)->EnableWindow(FALSE);
// 	}


}


void CVSCameraTaskDlg::OnBnClickedBtnGrab()
{
	theApp.m_pCamera->DisplayGrabImage();
}


// TEST �� ����� ��ư
void CVSCameraTaskDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	theApp.m_pCamera->TestFunc(0);
}


void CVSCameraTaskDlg::OnCbnSelchangeCmbModel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_cmbTrigMode.ResetContent();
	if (m_cmbCamModel.GetCurSel() == (int)eVA47M5_Series)
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	2);
	else
		m_fnAddCmbString(m_cmbTrigMode,	strTrigMode,	(int)eTrigMode_Count);
}


void CVSCameraTaskDlg::OnBnClickedGetCameraCondition()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strVal;
	strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetExposureTime());
	SetDlgItemText(IDC_EDIT_EXP_ON, strVal);
	strVal.Format(_T("%.2lf"), theApp.m_pCamera->GetAnalogGain());
	SetDlgItemText(IDC_EDIT_ANALOG_GAIN, strVal);
	m_cmbTrigMode.SetCurSel(theApp.m_pCamera->GetTriggerMode());
}



void CVSCameraTaskDlg::OnBnClickedBtnCalcRgb()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.	
	float WhiteBalance[3] = {0,};
	theApp.m_pCamera->CalcWhiteBalance(WhiteBalance);

	CString strTemp;
	strTemp.Format(_T("%lf"), WhiteBalance[0]);
	m_RedCoeff.SetWindowTextW(strTemp);

	strTemp.Format(_T("%lf"), WhiteBalance[1]);
	m_GreenCoeff.SetWindowTextW(strTemp);

	strTemp.Format(_T("%lf"), WhiteBalance[2]);
	m_BlueCoeff.SetWindowTextW(strTemp);


	
}


void CVSCameraTaskDlg::OnBnClickedBtnSaveWbValue()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strTemp;
	float WhiteBalance[3] = {0,};
	m_RedCoeff.GetWindowTextW(strTemp);
	WhiteBalance[0] = (float)_ttof(strTemp);
	m_GreenCoeff.GetWindowTextW(strTemp);
	WhiteBalance[1] = (float)_ttof(strTemp);
	m_BlueCoeff.GetWindowTextW(strTemp);
	WhiteBalance[2] = (float)_ttof(strTemp);

	theApp.m_pCamera->SetWhiteBalance(WhiteBalance);
		
	CString strVal;

	strVal.Format(_T("%f"), WhiteBalance[0]);
	WritePrivateProfileString(_T("White Balance"), _T("R"), strVal, theApp.GetInitFilePath());
	strVal.Format(_T("%f"), WhiteBalance[1]);
	WritePrivateProfileString(_T("White Balance"), _T("G"), strVal, theApp.GetInitFilePath());
	strVal.Format(_T("%f"), WhiteBalance[2]);
	WritePrivateProfileString(_T("White Balance"), _T("B"), strVal, theApp.GetInitFilePath());
}


void CVSCameraTaskDlg::OnBnClickedBtnGetWb()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	float WhiteBalance[3] = {0,};
	theApp.m_pCamera->GetWhiteBalance(WhiteBalance);

	CString strTemp;
	strTemp.Format(_T("%lf"),WhiteBalance[0]);
	m_RedCoeff.SetWindowTextW(strTemp);

	strTemp.Format(_T("%lf"),WhiteBalance[1]);
	m_GreenCoeff.SetWindowTextW(strTemp);

	strTemp.Format(_T("%lf"),WhiteBalance[2]);
	m_BlueCoeff.SetWindowTextW(strTemp);
}
