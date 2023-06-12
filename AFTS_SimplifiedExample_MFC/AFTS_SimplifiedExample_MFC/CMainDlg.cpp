// CMainDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "AFTS_SimplifiedExample_MFC.h"
#include "CMainDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAIN, pParent), m_radioTaskMode(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_AFTSSingleTone = NULL;
	for (int i = 0; i < AFTSDEVICE_NUMBER; ++i)
	{

		if (m_AFTS_Dlg_Class[i] != NULL)
		{
			m_AFTS_Dlg_Class[i] = NULL;
		}
	}
	nDeviceCount = 0;
	m_LogIndex = 0;
	m_bAFTSDiscovery = false;
	m_Dlginit_Flag = false;


}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ACSControllerIP, m_ACSIPAddressCtrl);
	//DDX_Control(pDX, IDC_AFTSTREE, m_AFTSTreeCtrl);
	DDX_Control(pDX, IDC_AFTSList, m_AFTSListControl);

	DDX_Control(pDX, IDC_RADIO3, m_TaskMode_Auto);
	DDX_Control(pDX, IDC_RADIO4, m_TaskMode_Setting);

	DDX_Radio(pDX, IDC_RADIO3, (int&)m_radioTaskMode);

	DDX_Control(pDX, IDC_PICTURE, m_Picture_Ctrl);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainDlg::OnTcnSelchangeTab1)
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO3, IDC_RADIO4, RadioCtrl)
	ON_BN_CLICKED(IDC_AFTSDiscovery, &CMainDlg::OnBnClickedAftsdiscovery)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AFTSList, &CMainDlg::OnLvnItemchangedAftslist)
	ON_BN_CLICKED(IDC_AFTSHandleDestroy, &CMainDlg::OnBnClickedAftshandledestroy)
	ON_BN_CLICKED(IDC_AFTSOpen, &CMainDlg::OnBnClickedAftsopen)
	ON_NOTIFY(NM_CLICK, IDC_AFTSList, &CMainDlg::OnNMClickAftslist)
	
	ON_BN_CLICKED(IDC_AFTSClose, &CMainDlg::OnBnClickedAftsclose)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	GetDlgItem(IDC_PICTURE)->GetWindowRect(m_Image_Rect);
	ScreenToClient(m_Image_Rect);

	if (m_Image.IsNull()) {
	
		m_Image.Load(L"6.png");
		InvalidateRect(m_Image_Rect);
	}

	InitControls();		// <-- 여기서 컨트롤을 초기화 한다.

	return TRUE;

}


// CMainDlg 메시지 처리기

void CMainDlg::InitControls()
{
	

	CRect rect;
	m_AFTSListControl.GetClientRect(&rect);
	m_AFTSListControl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_AFTSListControl.SetExtendedStyle(m_AFTSListControl.GetExtendedStyle() | LVS_EX_CHECKBOXES);
	m_AFTSListControl.InsertColumn(0, _T("AFTS Info"), LVCFMT_LEFT, 400);
	
	//m_AFTSListControl.InsertItem(0, _T("1"));
	//m_AFTSListControl.InsertItem(1, _T("2"));
	//m_AFTSListControl.InsertItem(2, _T("3"));
	//m_AFTSListControl.InsertItem(3, _T("4"));



	

	CListctr_clickindex = -1;

	m_ACSIPAddressCtrl.SetAddress(theApp.m_Module_IPadress[0], theApp.m_Module_IPadress[1], theApp.m_Module_IPadress[2], theApp.m_Module_IPadress[3]);

	OnBnClickedAftsdiscovery_Auto();

	//
	nDeviceCount = 4;

	initDevice(nDeviceCount);

	m_Dlginit_Flag = true;
	//

	
}

void CMainDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (IDC_TAB1 == pNMHDR->idFrom) {
		int select = m_ctlTab.GetCurSel();

		switch (select)
		{
		case 0:
			
			for (int i = 0; i < nDeviceCount; i++) {
				
				if (m_AFTS_Dlg_Class[i] != 0) {
					if (i == 0) {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_SHOW);
					}
					else {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_HIDE);
					}
				}
			}
			break;

		case 1:
			for (int i = 0; i < nDeviceCount; i++) {
				if (m_AFTS_Dlg_Class[i] != 0) {
					if (i == 1) {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_SHOW);
					}
					else {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_HIDE);
					}
				}
			}
			break;

		case 2:
			for (int i = 0; i < nDeviceCount; i++) {
				if (m_AFTS_Dlg_Class[i] != 0) {
					if (i == 2) {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_SHOW);
					}
					else {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_HIDE);
					}
				}
			}
			break;

		case 3:
			for (int i = 0; i < nDeviceCount; i++) {
				if (m_AFTS_Dlg_Class[i] != 0) {
					if (i == 3) {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_SHOW);
					}
					else {
						m_AFTS_Dlg_Class[i]->ShowWindow(SW_HIDE);
					}
				}
			}
			break;
		default:
			break;
		}
	}

	*pResult = 0;
}


void CMainDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	OnBnClickedAftshandledestroy();

	for (int i = 0; i < nDeviceCount; i++) {
		SAFE_DELETE_ARR(m_AFTS_Dlg_Class[i]);
	}

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CMainDlg::OnBnClickedAftsdiscovery()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bAFTSDiscovery == false) {
		if (m_AFTSSingleTone == NULL)
		{
			m_AFTSSingleTone = new AFTSSingleTone(EventCallback); //Register Eventcallback. listing the AFTS Devices. 
			m_AFTSSingleTone->Reg_EventCallback_ParentPointer(this);
			
		}
		
		BYTE ipFirst, ipSecond, ipThird, ipForth;
		CString strIPAddr;

		m_ACSIPAddressCtrl.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
		strIPAddr.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

		if (m_AFTSSingleTone->AFTSDiscovery(CString_to_uni_char_Convert(strIPAddr), _AFTSHandle, OUT nDeviceCount) == true)
		{
			m_bAFTSDiscovery = true;
			nDeviceCount = 4;
			initDevice(nDeviceCount);
			
		}
	}
}

void CMainDlg::OnBnClickedAftsdiscovery_Auto()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_bAFTSDiscovery == false) {
		if (m_AFTSSingleTone == NULL)
		{
			m_AFTSSingleTone = new AFTSSingleTone(EventCallback); //Register Eventcallback. listing the AFTS Devices. 
			m_AFTSSingleTone->Reg_EventCallback_ParentPointer(this);

		}

		BYTE ipFirst, ipSecond, ipThird, ipForth;
		CString strIPAddr;

		m_ACSIPAddressCtrl.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
		strIPAddr.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

		if (m_AFTSSingleTone->AFTSDiscovery(CString_to_uni_char_Convert(strIPAddr), _AFTSHandle, OUT nDeviceCount) == true)
		{
			m_bAFTSDiscovery = true;

			
		}
	}
}

char* CMainDlg::CString_to_uni_char_Convert(CString data)
{
	// Unicode CString -> char* 변환 과정 
	// CString -> wchar* -> char* 순서로 변환 되어야 함

	wchar_t *wchar_str;
	char *char_str;
	int char_str_len;

	// 1. CString to wchar * conversion
	wchar_str = data.GetBuffer(data.GetLength());
	char_str_len = WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, NULL, 0, NULL, NULL);
	char_str = new char[char_str_len];

	// 2. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wchar_str, -1, char_str, char_str_len, 0, 0);
	return char_str;
}

void CMainDlg::EventCallback(EVENTDATA* _Data)
{
	CMainDlg* pDlg = reinterpret_cast<CMainDlg*>(_Data->_pParent);
	pDlg->EventCallbackMain(_Data);
}

void CMainDlg::EventCallbackMain(EVENTDATA* _Data)
{
	switch (_Data->_LOGINDEX)
	{
	case MOTOR_AND_AFCAM_EVENT:
		//m_AFTS_Dlg_Class[_Data->_AFTSAxisIndex]->Addlog_ListCtrl(GetTimeInfo(), AddString(_Data->_MESSAGELOG));
		
		break;
	case DISCOVERY_DEVICEINFO:
		Addlog_ListCtrlAFTS(AddString(_Data->_MESSAGELOG));
		break;

	case AF_CAMERA_DISCONNECTED:
		//AddDevlog_ListCtrlAFTS(GetTimeInfo(), _Data->_AFTSAxisIndex ,AddString(_Data->_MESSAGELOG));
		break;
	}
}

void CMainDlg::Addlog_ListCtrlAFTS(CString Info)
{
	int nItemNum = m_AFTSListControl.GetItemCount();
	m_AFTSListControl.InsertItem(nItemNum, _T(""));
	//m_AFTSListControl.SetItemText(nItemNum, 0, AxisIndex);
	m_AFTSListControl.SetItemText(nItemNum, 0, Info);
}

void CMainDlg::AddDevlog_ListCtrlAFTS(CString Date, int Dev_Num, CString Info)
{

}

CString CMainDlg::AddString(char* msg)
{
	return Unicode_char_to_CString_Convert(msg);
}

CString CMainDlg::Unicode_char_to_CString_Convert(char *data)
{
	// Unicode char* -> CString 변환 과정 
	// char* -> wchar* -> CString 순서로 변환 되어야 함

	int len;
	CString str;
	BSTR buf;

	// 1. char* to wchar * conversion
	len = MultiByteToWideChar(CP_ACP, 0, data, strlen(data), NULL, NULL);
	buf = SysAllocStringLen(NULL, len);
	MultiByteToWideChar(CP_ACP, 0, data, strlen(data), buf, len);

	// 2. wchar_t* to CString conversion
	str.Format(_T("%s"), buf);

	return str;
}


void CMainDlg::RadioCtrl(UINT ID) {

	UpdateData(TRUE);

	if (nDeviceCount > 0) {
		switch (m_radioTaskMode)
		{
		case 0:


			GetDlgItem(IDC_ACSControllerIP)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSClose)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSOpen)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSDiscovery)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSHandleDestroy)->EnableWindow(FALSE);

			for (int i = 0; i < nDeviceCount; i++) {
				if (m_AFTS_Dlg_Class[i] != NULL) {
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZMotorMove)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFCAMEXPOSURE)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZOFFSETEDIT)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFSwitch)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamExposure)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamZOffset)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamExposure)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamZOffset)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisHoming)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisUp)->EnableWindow(FALSE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisDown)->EnableWindow(FALSE);
				}
			}

			m_nTaskMode = 0;

			break;

		case 1:


			GetDlgItem(IDC_ACSControllerIP)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSClose)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSOpen)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSDiscovery)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSHandleDestroy)->EnableWindow(TRUE);

			for (int i = 0; i < nDeviceCount; i++) {
				if (m_AFTS_Dlg_Class[i] != NULL) {
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZMotorMove)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFCAMEXPOSURE)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZOFFSETEDIT)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFSwitch)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamExposure)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamZOffset)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamExposure)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamZOffset)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisHoming)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisUp)->EnableWindow(TRUE);
					m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisDown)->EnableWindow(TRUE);
				}
			}

			m_nTaskMode = 1;

			break;

		default:
			break;
		}
	}
	UpdateData(FALSE);

}

void CMainDlg::OnLvnItemchangedAftslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_nTaskMode == 1) {
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		//m_AxisNum = m_AFTSListControl.GetItemCount() - 1;
		NM_LISTVIEW* pNMView = (NM_LISTVIEW*)pNMHDR;

		if (m_AFTSListControl.GetItemCount() == 0) {
			return;
		}
		else {
		
			//m_AxisNum = m_AFTSListControl.GetItemCount() - 1;
			
		}

		*pResult = 0;
	}
}


CString CMainDlg::GetTimeInfo()
{
	CString currentDate;
	CTime data = CTime::GetCurrentTime();
	currentDate.Format(_T("%d-%d-%d-%d-%d-%d"),
		data.GetYear(), data.GetMonth(), data.GetDay(),
		data.GetHour(), data.GetMinute(), data.GetSecond());
	return currentDate;
}

void CMainDlg::OnBnClickedAftshandledestroy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_bAFTSDiscovery == true)
	{
		for (int i = 0; i < nDeviceCount; i++) {
			if (m_AFTS_Dlg_Class[i] != NULL)
			{
				if (m_AFTS_Dlg_Class[i]->OnBnClickedAftsclose() == true) {
					
					
					SAFE_DELETE(m_AFTS_Dlg_Class[i]);
					
					
				}
				else {
					return;
				}
				
				
			}
		}
		m_ctlTab.DeleteAllItems();
		m_ctlTab.UnsubclassWindow();
		//m_ctlTab.m_hWnd == NULL;
		nDeviceCount = 0;

		if (m_AFTSSingleTone != NULL)
		{
			m_AFTSSingleTone->CloseAFTSHandle(false);
			delete m_AFTSSingleTone;
			m_AFTSSingleTone = NULL;

			m_bAFTSDiscovery = false;
			m_AFTSListControl.DeleteAllItems();
		}
	}
	else
	{
		return;
	}
}


void CMainDlg::OnBnClickedAftsopen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	//m_AFTS_Dlg_Class[0]->ShowWindow(SW_SHOW);
	CRect rcCtrl;
	m_ctlTab.GetWindowRect(rcCtrl);				// 컨트롤 크기 확인
	ScreenToClient(rcCtrl);

	if (m_bAFTSDiscovery == true)
	{
		if (m_AFTS_Dlg_Class[m_AxisNum] == NULL)
		{
			m_AFTS_Dlg_Class[m_AxisNum] = new CAFTSSimplifiedExampleMFCDlg(_AFTSHandle, EventCallback, m_AxisNum);

	
			m_AFTS_Dlg_Class[m_AxisNum]->Create(IDD_AFTS_SIMPLIFIEDEXAMPLE_MFC_DIALOG, &m_ctlTab);								// 다이얼로그 생성 (탭에 종속되게 생성)
			m_AFTS_Dlg_Class[m_AxisNum]->MoveWindow(2, 40, rcCtrl.Width() - 6, rcCtrl.Height() - 28);	// 위치 이동
			m_AFTS_Dlg_Class[m_AxisNum]->ShowWindow(SW_HIDE);			// 초기 값은 HIDE

			if (m_AFTS_Dlg_Class[m_AxisNum]->OnBnClickedAftsopen() == true) {
				m_AFTSListControl.SetCheck(m_AxisNum, true);

				m_ctlTab.SetCurSel(m_AxisNum);
			}
			else {
				//ADDLOG
			}
		}
		else {
		
		}
	}
	else
	{
		return;
	}
}


void CMainDlg::OnNMClickAftslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_nTaskMode == 1) {
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		//m_AxisNum = m_AFTSListControl.GetItemCount() - 1;
		NM_LISTVIEW* pNMView = (NM_LISTVIEW*)pNMHDR;

		if (m_AFTSListControl.GetItemCount() == 0) {
			return;
		}
		else {
			CListctr_clickindex = pNMView->iItem;

			if (CListctr_clickindex == -1) {
				m_AxisNum = m_AFTSListControl.GetItemCount() - 1;
			}
			else {
				m_AxisNum = CListctr_clickindex;
			}
		}

		*pResult = 0;
	}
}


int CMainDlg::mfn_AutoFocus(int index , double zPos) {

	int ref = APP_OK;
	CString strLogTemp;

	if (m_bAFTSDiscovery == true && m_AFTS_Dlg_Class[index] != NULL)
	{
		
		
		ref = m_AFTS_Dlg_Class[index]->mfn_MoveZAxis(zPos);

		if (ref != APP_OK) {
			strLogTemp.Format(_T("[%s][%s] AF Move Z Fail!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

			theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

			return ref;
		}

		ref = m_AFTS_Dlg_Class[index]->OnAfswitch();

		if (ref != APP_OK) {
			strLogTemp.Format(_T("[%s][%s] AF OnAF Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

			theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

			return ref;
		}

		ref = m_AFTS_Dlg_Class[index]->OffAfswitch();

		if (ref != APP_OK) {
			strLogTemp.Format(_T("[%s][%s] AF OffAF Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

			theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

			return ref;
		}
		
		
		
		return ref;
	}
	else {

		ref = APP_NG;

		strLogTemp.Format(_T("[%s][%s] AF Fail [Discovery Fail || Dlg Null] - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

		theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

		return ref;
	}
}


void CMainDlg::OnBnClickedAftsclose()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	

	if (m_bAFTSDiscovery == true)
	{

		if (m_AFTS_Dlg_Class[m_AxisNum] != NULL)
		{
			if (m_AFTS_Dlg_Class[m_AxisNum]->OnBnClickedAftsclose() == true) {
				m_AFTSListControl.SetCheck(m_AxisNum, false);
				SAFE_DELETE(m_AFTS_Dlg_Class[m_AxisNum]);

				m_ctlTab.SetCurSel(m_AxisNum);
			}
			else {
			
			}


		}
		else {
		
		}
		
	}
	else
	{
		return;
	}
}



bool CMainDlg::initDevice(int nDev_Count) {

	if (m_bAFTSDiscovery == true) {
		for (int i = 0; i < nDeviceCount; i++) {
			m_AFTS_Dlg_Class[i] = new CAFTSSimplifiedExampleMFCDlg(_AFTSHandle, EventCallback, i);
			if (m_AFTS_Dlg_Class[i] != NULL) {
				
				m_AFTSListControl.SetCheck(i, true);
			}
		}
	}
	else {
		AfxMessageBox(_T("AF 장치 연결 실패"));

		return false;
	}

	

	/////////////////////////////////////////////////////////////
	CRect rcCtrl;

	// 메인 Tab Control
	if (m_ctlTab.m_hWnd == NULL)
	{

		m_ctlTab.SubclassDlgItem(IDC_TAB1, this);	// 컨트롤 연결
		m_ctlTab.GetWindowRect(rcCtrl);				// 컨트롤 크기 확인
		ScreenToClient(rcCtrl);						// 컨트롤 크기를 상대 좌표로 변환 (다이얼로그 위의 크기)

		
	}
	
	for (int i = 0; i < nDeviceCount; i++) {
		m_ctlTab.InsertItem(i, strTab_Name[i]);		// 탭 생성

	}
	// Tab Page 1
	for (int i = 0; i < nDeviceCount; i++)
	{

		m_AFTS_Dlg_Class[i]->Create(IDD_AFTS_SIMPLIFIEDEXAMPLE_MFC_DIALOG, &m_ctlTab);								// 다이얼로그 생성 (탭에 종속되게 생성)
		m_AFTS_Dlg_Class[i]->MoveWindow(2, 40, rcCtrl.Width() - 6, rcCtrl.Height() - 28);	// 위치 이동
		m_AFTS_Dlg_Class[i]->ShowWindow(SW_HIDE);			// 초기 값은 HIDE
	}

	

	// 초기 선택 탭 설정
	if (nDeviceCount > 0) {
		m_ctlTab.SetCurSel(0);
		m_AFTS_Dlg_Class[0]->ShowWindow(SW_SHOW);	// 첫 페이지는 SHOW

		if (m_TaskMode_Auto.GetCheck() == true) {

			GetDlgItem(IDC_ACSControllerIP)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSClose)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSOpen)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSDiscovery)->EnableWindow(FALSE);
			GetDlgItem(IDC_AFTSHandleDestroy)->EnableWindow(FALSE);

			m_nTaskMode = 0;
		}
		else {

			GetDlgItem(IDC_ACSControllerIP)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSClose)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSOpen)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSDiscovery)->EnableWindow(TRUE);
			GetDlgItem(IDC_AFTSHandleDestroy)->EnableWindow(TRUE);

			m_nTaskMode = 1;
		}
	}

	for (int i = 0; i < nDeviceCount; i++) {


		/////
		if (m_TaskMode_Auto.GetCheck() == true) {

			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZMotorMove)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFCAMEXPOSURE)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZOFFSETEDIT)->SendMessage(EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFSwitch)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamExposure)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamZOffset)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamExposure)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamZOffset)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisHoming)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisUp)->EnableWindow(FALSE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisDown)->EnableWindow(FALSE);

			m_nTaskMode = 0;
		}
		else {

			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZMotorMove)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFCAMEXPOSURE)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZOFFSETEDIT)->SendMessage(EM_SETREADONLY, (WPARAM)FALSE, (LPARAM)0);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_AFSwitch)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamExposure)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_GetAFCamZOffset)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamExposure)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_SetAFCamZOffset)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisHoming)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisUp)->EnableWindow(TRUE);
			m_AFTS_Dlg_Class[i]->GetDlgItem(IDC_ZAxisDown)->EnableWindow(TRUE);

			m_nTaskMode = 1;
		}

		/////
	}



	return true;
}

void CMainDlg::PingTest(int index) {
	Sleep(1000);
	m_AFTS_Dlg_Class[index]->PingTest(index);
}

void CMainDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	if (!m_Image.IsNull()) {
		dc.SetStretchBltMode(COLORONCOLOR);
		m_Image.Draw(dc, m_Image_Rect);
	}
}
