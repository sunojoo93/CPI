
// AFTS_SimplifiedExample_MFCDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "AFTS_SimplifiedExample_MFC.h"
#include "AFTS_SimplifiedExample_MFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAFTSSimplifiedExampleMFCDlg 대화 상자



CAFTSSimplifiedExampleMFCDlg::CAFTSSimplifiedExampleMFCDlg(void* p_AFTSHandle, AFTSEventCallback Callback,  int p_DeviceIndex,  CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AFTS_SIMPLIFIEDEXAMPLE_MFC_DIALOG, pParent )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_AFTSSingleTone = NULL;

	m_pAFTSClass = NULL; //Autorun 시 주석제거

	nDeviceCount = 0;
	m_LogIndex = 0;
	m_bAFTSDiscovery = true;
	m_bAFCAMOpened = false;

	_AFTSHandle = p_AFTSHandle;
	m_AxisNum = p_DeviceIndex;

	pEventCallback = Callback;
	m_Dlginit_Flag = false;
	
	if (m_bAFTSDiscovery == true)
	{
		if (m_pAFTSClass == NULL)
		{
			m_pAFTSClass = new AFTSControlLibrary(m_AxisNum, _AFTSHandle, ImageCallback, EventCallback, AFTSParamCallback, this); //인자의 경우, 모터 축 번호,AFTS 핸들, 영상콜백함수, 이벤트 콜백 함수, 현재 Dlg의 포인터 순으로 입력.			
			//m_pAFTSClass->OpenAFCAM();
			//m_bAFCAMOpened = true;

			// MFC UI Control Only
			//m_AFTSListControl.SetCheck(m_AxisNum, true);
			//SetTimer(1, 500, NULL);
		}
	}
	
	
}

void CAFTSSimplifiedExampleMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EventLog, m_EventLogListctrl);
	//DDX_Control(pDX, IDC_ACSControllerIP, m_ACSIPAddressCtrl);
	//DDX_Control(pDX, IDC_AFTSTREE, m_AFTSTreeCtrl);
	//DDX_Control(pDX, IDC_AFTSList, m_AFTSListControl);
	DDX_Control(pDX, IDC_AFMODE_1, m_AFModeBtn);
	DDX_Control(pDX, IDC_AFMODE_2, m_MeasureModeBtn);
	DDX_Control(pDX, IDC_AFSwitchLED, m_AFSwitchLED);
	DDX_Control(pDX, IDC_HWLimitLeftLED, m_HWLeftLimitLED);
	DDX_Control(pDX, IDC_HWLimitRightLED, m_HWRightLimitLED);

	DDX_Control(pDX, IDC_AFTSList2, mCameraCtrl_Log);
}

BEGIN_MESSAGE_MAP(CAFTSSimplifiedExampleMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_AFTSDiscovery, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsdiscovery)
	ON_BN_CLICKED(IDC_AFTSHandleDestroy, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftshandledestroy)
	//ON_BN_CLICKED(IDC_AFTSOpen, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsopen)
	//ON_BN_CLICKED(IDC_AFTSClose, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsclose)
	ON_BN_CLICKED(IDC_AFSwitch, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedAfswitch)
	ON_BN_CLICKED(IDC_ZAxisHoming, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxishoming)
	ON_BN_CLICKED(IDC_ZAxisUp, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxisup)
	ON_BN_CLICKED(IDC_ZAxisDown, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxisdown)
	ON_BN_CLICKED(IDC_GetAFCamExposure, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedGetafcamexposure)
	ON_BN_CLICKED(IDC_GetAFCamZOffset, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedGetafcamzoffset)
	ON_BN_CLICKED(IDC_SetAFCamExposure, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedSetafcamexposure)
	ON_BN_CLICKED(IDC_SetAFCamZOffset, &CAFTSSimplifiedExampleMFCDlg::OnBnClickedSetafcamzoffset)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AFTSList, &CAFTSSimplifiedExampleMFCDlg::OnLvnItemchangedAftslist)
	ON_WM_TIMER()
	ON_WM_DESTROY()
//	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// CAFTSSimplifiedExampleMFCDlg 메시지 처리기

BOOL CAFTSSimplifiedExampleMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	
	//((CButton*)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
	
	

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	((CButton*)GetDlgItem(IDC_AFMODE_1))->SetCheck(TRUE);


	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	mfn_DlgInit();

	m_Dlginit_Flag = true;
	//OnBnClickedAftsdiscovery();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CAFTSSimplifiedExampleMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CAFTSSimplifiedExampleMFCDlg::OnPaint()
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
HCURSOR CAFTSSimplifiedExampleMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsdiscovery()
{
	/*if (m_AFTSSingleTone == NULL)
	{
		m_AFTSSingleTone = new AFTSSingleTone(EventCallback); //Register Eventcallback. listing the AFTS Devices. 
		m_AFTSSingleTone->Reg_EventCallback_ParentPointer(this);
	}

	BYTE ipFirst, ipSecond, ipThird, ipForth;
	CString strIPAddr;

	m_ACSIPAddressCtrl.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	strIPAddr.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

	if (m_AFTSSingleTone->AFTSDiscovery  (CString_to_uni_char_Convert(strIPAddr), _AFTSHandle, OUT nDeviceCount) == true)
	{
		m_bAFTSDiscovery = true;
	}*/
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftshandledestroy()
{
	/*if (m_bAFTSDiscovery == true)
	{

		if (m_AFTSClass[m_AxisNum] != NULL)
		{
			OnBnClickedAftsclose();// Before call CloseAFTSHandle, Please close AF Cam first.
		}
		if (m_AFTSSingleTone != NULL)
		{
			m_AFTSSingleTone->CloseAFTSHandle(false);
			delete m_AFTSSingleTone;
			m_AFTSSingleTone = NULL;

			m_bAFCAMOpened = false;
			m_bAFTSDiscovery = false;
			m_AFTSListControl.DeleteAllItems();
		}
	}
	else
	{
		return;
	}*/
	
}


bool CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsopen()
{
		if (m_bAFCAMOpened == false)
		{
			try{
				m_pAFTSClass->OpenAFCAM();
				m_bAFCAMOpened = true;

				// MFC UI Control Only
				//m_AFTSListControl.SetCheck(m_AxisNum, true);
				SetTimer(1, 500, NULL);
			}
			catch (const std::exception&) {
				return false;
			}
		}
		else if(m_bAFCAMOpened == true)
		{
			return true;
		}

}


bool CAFTSSimplifiedExampleMFCDlg::OnBnClickedAftsclose()
{
	
		if (m_bAFCAMOpened == true)
		{
			try {
				m_pAFTSClass->AFSwitchOff();
				m_pAFTSClass->CloseAFCAM();
				m_bAFCAMOpened = false;

				delete m_pAFTSClass;
				m_pAFTSClass = NULL;

				m_bAFCAMOpened = false;

				// MFC UI Control Only
				//m_AFTSListControl.SetCheck(m_AxisNum, false);
				KillTimer(1);

				CString text = _T("");
				SetDlgItemText(IDC_ZMotorPoSEdit, text);
				SetDlgItemText(IDC_AFDIFF, text);
				SetDlgItemText(IDC_AFCAMEXPOSURE, text);
				SetDlgItemText(IDC_ZOFFSETEDIT, text);

				return true;
			}
			catch (const std::exception&) {
				return false;
			}
	
		}
		else
		{
			return true;
		}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedAfswitch()
{
	bool _AFSwitch = false;
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			m_pAFTSClass->Get_AFSwitch(_AFSwitch);
			if (m_AFModeBtn.GetCheck() == true)
			{
				if (_AFSwitch == false)
				{
					m_pAFTSClass->AFSwitchOn();
					SetDlgItemText(IDC_AFSwitch, _T("AF OFF"));
					GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
					GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);
				}
				else if (_AFSwitch == true)
				{
					m_pAFTSClass->AFSwitchOff();
					SetDlgItemText(IDC_AFSwitch, _T("AF ON"));
					GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
					GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);
				}
			}
			else if (m_MeasureModeBtn.GetCheck() == true)
			{
				CString Status;
				GetDlgItemText(IDC_AFSwitch, Status);
				if (Status == "AF ON")
				{
					m_pAFTSClass->MeasureModeOn();
					SetDlgItemText(IDC_AFSwitch, _T("AF OFF"));
					GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
					GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);
				}
				else if (Status == "AF OFF")
				{
					m_pAFTSClass->MeasureModeOff();
					SetDlgItemText(IDC_AFSwitch, _T("AF ON"));
					GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
					GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);
				}
			}
			//else if (_AFSwitch == true)
			//{
			//	if (m_AFModeBtn.GetCheck() == true)
			//	{
			//		m_AFTSClass[m_AxisNum]->AFSwitchOff();
			//	}
			//	else if (m_MeasureModeBtn.GetCheck() == true)
			//	{
			//		m_AFTSClass[m_AxisNum]->MeasureModeOff();
			//	}
			//	SetDlgItemText(IDC_AFSwitch, _T("AF ON"));
			//}
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}





}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxishoming()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			m_pAFTSClass->HomingZAxis();
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxisup()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			CString _StrValue;
			GetDlgItemText(IDC_ZMotorMove, _StrValue);
			double Pos = _wtof(_StrValue);
			m_pAFTSClass->MoveZAxis(Pos,true);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedZaxisdown()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			CString _StrValue;
			GetDlgItemText(IDC_ZMotorMove, _StrValue);
			double Pos = _wtof(_StrValue);
			m_pAFTSClass->MoveZAxis(-1*Pos, true);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedGetafcamexposure()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			float Value = 0;
			m_pAFTSClass->Get_AFCamExposure(Value);

			CString text;
			text.Format(L"%.2f", Value); //Unicode
			SetDlgItemTextW(IDC_AFCAMEXPOSURE, text);

		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedGetafcamzoffset()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			float Value = 0;
			m_pAFTSClass->Get_AFCamZOffset(Value);

			CString text;

			text.Format(L"%.4f", Value); //Unicode
			SetDlgItemTextW(IDC_ZOFFSETEDIT, text);

		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedSetafcamexposure()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{

			CString _StrValue;
			GetDlgItemText(IDC_AFCAMEXPOSURE, _StrValue);
			float Value = _wtof(_StrValue);

			m_pAFTSClass->SetCamExposure(Value);

		
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::OnBnClickedSetafcamzoffset()
{
	if (m_bAFTSDiscovery == true)
	{
		if (m_bAFCAMOpened == true)
		{
			CString _StrValue;
			GetDlgItemText(IDC_ZOFFSETEDIT, _StrValue);
			float Value = _wtof(_StrValue);

			m_pAFTSClass->SetZOffset(Value);
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
}


void CAFTSSimplifiedExampleMFCDlg::ImageCallback(IMAGEDATA* _Data)
{
	CAFTSSimplifiedExampleMFCDlg* pDlg = reinterpret_cast<CAFTSSimplifiedExampleMFCDlg*>(_Data->_pParent);
	pDlg->ImageProcessing(_Data);
}

void CAFTSSimplifiedExampleMFCDlg::ImageProcessing(IMAGEDATA* _Data)
{
	m_MainImageData._WIDTH = _Data->_WIDTH;
	m_MainImageData._HEIGHT = _Data->_HEIGHT;
	m_MainImageData._BUFFER = _Data->_BUFFER;
	m_MainImageData._BUFFERINDEX = _Data->_BUFFERINDEX;
}

void CAFTSSimplifiedExampleMFCDlg::EventCallback(EVENTDATA* _Data)
{
	CAFTSSimplifiedExampleMFCDlg* pDlg = reinterpret_cast<CAFTSSimplifiedExampleMFCDlg*>(_Data->_pParent);
	pDlg->EventCallbackMain(_Data);
}

void CAFTSSimplifiedExampleMFCDlg::EventCallbackMain(EVENTDATA* _Data)
{
	switch (_Data->_LOGINDEX)
	{
	case MOTOR_AND_AFCAM_EVENT:
		Addlog_ListCtrl(GetTimeInfo(), AddString(_Data->_MESSAGELOG));
		break;
	case DISCOVERY_DEVICEINFO:
		//Addlog_ListCtrlAFTS(AddString(_Data->_MESSAGELOG));
		break;
	case AF_CAMERA_DISCONNECTED:
		Addlog_ListCtrlAFTS(GetTimeInfo() , AddString(_Data->_MESSAGELOG));
		break;
	}
}

void CAFTSSimplifiedExampleMFCDlg::AFTSParamCallback(AFTSParam* _Data)
{
	CAFTSSimplifiedExampleMFCDlg* pDlg = reinterpret_cast<CAFTSSimplifiedExampleMFCDlg*>(_Data->_pParent);
	pDlg->AFTSParamCallbackMain(_Data);
}

void CAFTSSimplifiedExampleMFCDlg::AFTSParamCallbackMain(AFTSParam* _Data)
{

	CString text;
	text.Format(L"%.2f", _Data->_CAMExposure); //Unicode
	SetDlgItemTextW(IDC_AFCAMEXPOSURE, text);

	text.Format(L"%.4f", _Data->_ZOffset); //Unicode
	SetDlgItemTextW(IDC_ZOFFSETEDIT, text);
}



void CAFTSSimplifiedExampleMFCDlg::Addlog_ListCtrl(CString Date, CString Info)
{
	int nItemNum = m_EventLogListctrl.GetItemCount();
	m_EventLogListctrl.InsertItem(nItemNum, _T(""));
	m_EventLogListctrl.SetItemText(nItemNum, 0, Date);
	m_EventLogListctrl.SetItemText(nItemNum, 1, Info);
}

void CAFTSSimplifiedExampleMFCDlg::Addlog_ListCtrlAFTS(CString Date , CString Info)
{
	int nItemNum = mCameraCtrl_Log.GetItemCount();
	mCameraCtrl_Log.InsertItem(nItemNum, _T(""));
	mCameraCtrl_Log.SetItemText(nItemNum, 0, Date);
	mCameraCtrl_Log.SetItemText(nItemNum, 1, Info);
}


CString CAFTSSimplifiedExampleMFCDlg::UpLogIndex()
{
	CString str;
	str.Format(_T("%d", ++m_LogIndex));
	return str;
}

CString CAFTSSimplifiedExampleMFCDlg::AddString(char* msg)
{
	return Unicode_char_to_CString_Convert(msg);
}

char* CAFTSSimplifiedExampleMFCDlg::AddChar(CString msg)
{
	return CString_to_uni_char_Convert(msg);
}



CString CAFTSSimplifiedExampleMFCDlg::GetTimeInfo()
{
	CString currentDate;
	CTime data = CTime::GetCurrentTime();
	currentDate.Format(_T("%d-%d-%d-%d-%d-%d"),
		data.GetYear(), data.GetMonth(), data.GetDay(),
		data.GetHour(), data.GetMinute(), data.GetSecond());
	return currentDate;
}

CString CAFTSSimplifiedExampleMFCDlg::Unicode_char_to_CString_Convert(char *data)
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

char* CAFTSSimplifiedExampleMFCDlg::CString_to_uni_char_Convert(CString data)
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



void CAFTSSimplifiedExampleMFCDlg::OnLvnItemchangedAftslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*if (m_nTaskMode == 1) {
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		m_AxisNum = m_AFTSListControl.GetItemCount() - 1;
		*pResult = 0;
	}*/
}


void CAFTSSimplifiedExampleMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case 1:
		RefreshAFParams();
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CAFTSSimplifiedExampleMFCDlg::RefreshAFParams()
{
	if (m_pAFTSClass != NULL)
	{
		double _ZPoS = 0;
		double _XPoS = 0;
		double AFDiff = 0;
		int  AFDiff_Offset = 0;
		bool AFSWitch = false;
		bool CAMStreaming = false;
		bool XAxisEnabled = false;
		bool ZAxisEnabled = false;
		bool SWLimitLeft = false;
		bool SWLimitRight = false;
		bool HWLimitLeft = false;
		bool HWLimitRight = false;

		
		m_pAFTSClass->GetAFDiffValue(AFDiff);
		m_pAFTSClass->GetAxisPoSValue(_ZPoS);
		m_pAFTSClass->Get_AFSwitch(AFSWitch);
		m_pAFTSClass->GetAFDiff_OffsetValue(AFDiff_Offset);
		m_pAFTSClass->Get_AFCamStreaming(CAMStreaming);
		m_pAFTSClass->Get_MotorEnabledStatus(ZAxisEnabled);
		m_pAFTSClass->Get_SWLimitLeftStatus(SWLimitLeft);
		m_pAFTSClass->Get_SWLimitRightStatus(SWLimitRight);
		m_pAFTSClass->Get_HWLimitLeftStatus(HWLimitLeft);
		m_pAFTSClass->Get_HWLimitRightStatus(HWLimitRight);

		CString text;
		text.Format(_T("%.4f"), _ZPoS);

		SetDlgItemText(IDC_ZMotorPoSEdit, text);

		text.Format(_T("%.2f"), AFDiff);
		SetDlgItemText(IDC_AFDIFF, text);

		if (AFSWitch == true)
		{
			m_AFSwitchLED.SetFaceColor(RGB(30, 255, 20)); //배경색 변경
			SetDlgItemText(IDC_AFSwitchLED,_T("ON"));
		}
		else
		{
			m_AFSwitchLED.SetFaceColor(RGB(250, 100, 10)); //배경색 변경
			SetDlgItemText(IDC_AFSwitchLED,_T("OFF"));
		}

		if (HWLimitLeft == true)
		{
			m_HWLeftLimitLED.SetFaceColor(RGB(255, 0, 50)); //배경색 변경
			SetDlgItemText(IDC_HWLimitLeftLED, _T("ON"));
		}
		else
		{
			m_HWLeftLimitLED.SetFaceColor(RGB(30, 255, 20)); //배경색 변경
			SetDlgItemText(IDC_HWLimitLeftLED, _T("OFF"));
		}

		if (HWLimitRight == true)
		{
			m_HWRightLimitLED.SetFaceColor(RGB(255, 0, 50)); //배경색 변경
			SetDlgItemText(IDC_HWLimitRightLED, _T("ON"));
		}
		else
		{
			m_HWRightLimitLED.SetFaceColor(RGB(30, 255, 20)); //배경색 변경
			SetDlgItemText(IDC_HWLimitRightLED, _T("OFF"));
		}

	}
}

void CAFTSSimplifiedExampleMFCDlg::OnDestroy()
{
	OnBnClickedAftshandledestroy();
	CDialogEx::OnDestroy();
}


int CAFTSSimplifiedExampleMFCDlg::mfn_MoveZAxis(double zPos)
{
	try
	{
		return m_pAFTSClass->MoveZAxis(zPos, false);
	}
	catch (CException* e)
	{
		CString strLogTemp;

		strLogTemp.Format(_T("[%s][%s] AF Move Z Fail!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

		theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

		return APP_NG;
	}
}

void CAFTSSimplifiedExampleMFCDlg::mfn_DlgInit()
{
	//m_ACSIPAddressCtrl.SetAddress(theApp.m_Module_IPadress[0], theApp.m_Module_IPadress[1], theApp.m_Module_IPadress[2], theApp.m_Module_IPadress[3]);
	

	// MFC UI Control Only
	//m_AFTSListControl.SetCheck(m_AxisNum, true);

	CRect rect;
	m_EventLogListctrl.GetClientRect(&rect);
	m_EventLogListctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_EventLogListctrl.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 130);
	m_EventLogListctrl.InsertColumn(1, _T("LogInfo"), LVCFMT_LEFT, 1000);

	mCameraCtrl_Log.GetClientRect(&rect);
	mCameraCtrl_Log.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	mCameraCtrl_Log.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 130);
	mCameraCtrl_Log.InsertColumn(1, _T("LogInfo"), LVCFMT_LEFT, 1000);
	

	m_AFSwitchLED.EnableWindowsTheming(false);
	m_AFSwitchLED.SetTextColor(RGB(0, 0, 0)); //글자색 변경


	m_HWLeftLimitLED.EnableWindowsTheming(false);
	m_HWLeftLimitLED.SetTextColor(RGB(0, 0, 0)); //글자색 변경


	m_HWRightLimitLED.EnableWindowsTheming(false);
	m_HWRightLimitLED.SetTextColor(RGB(0, 0, 0)); //글자색 변경


	m_pAFTSClass->OpenAFCAM(); //Autorun 시 주석제거
	m_bAFCAMOpened = true;	//Autorun 시 주석제거
	SetTimer(1, 500, NULL);	//Autorun 시 주석제거
	
	
}

int CAFTSSimplifiedExampleMFCDlg::OnAfswitch() {
	try
	{
		bool _AFSwitch = false;
		if (m_bAFTSDiscovery == true)
		{
			if (m_bAFCAMOpened == true)
			{
				m_pAFTSClass->Get_AFSwitch(_AFSwitch);
				if (m_AFModeBtn.GetCheck() == true)
				{
					if (_AFSwitch == false)
					{
						int nRet = APP_OK;

						nRet = m_pAFTSClass->AFSwitchOn();
						SetDlgItemText(IDC_AFSwitch, _T("AF OFF"));
						GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
						GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);

						return nRet;
					}
					else {

						CString strLogTemp;

						strLogTemp.Format(_T("[%s][%s] AF On [AF Swich On State] Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

						theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

						return APP_NG;
					}
				
				}
				else {

					CString strLogTemp;

					strLogTemp.Format(_T("[%s][%s] AF On [AF Mode Measure State] Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

					theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

					return APP_NG;
				
				}
				/*else if (m_MeasureModeBtn.GetCheck() == true)
				{
					CString Status;
					GetDlgItemText(IDC_AFSwitch, Status);
					if (Status == "AF OFF")
					{
						m_pAFTSClass->MeasureModeOff();
						SetDlgItemText(IDC_AFSwitch, _T("AF ON"));
						GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
						GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);
					}
					
				}*/
			}
			else
			{
				CString strLogTemp;

				strLogTemp.Format(_T("[%s][%s] AF On [AFCAM OFF State] Fail!!!- %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

				theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

				return APP_NG;
			}
		}
		else
		{
			CString strLogTemp;

			strLogTemp.Format(_T("[%s][%s] AF On [Discovery OFF State] Fail!!!- %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

			theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

			return APP_NG;
		}
	}
	catch (CException* e)
	{
		CString strLogTemp;

		strLogTemp.Format(_T("[%s][%s] AF On Error!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

		theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

		return APP_NG;
	}

}

int CAFTSSimplifiedExampleMFCDlg::OffAfswitch() {

	try
	{
		bool _AFSwitch = false;

		if (m_bAFTSDiscovery == true)
		{
			if (m_bAFCAMOpened == true)
			{
				m_pAFTSClass->Get_AFSwitch(_AFSwitch);
				if (m_AFModeBtn.GetCheck() == true)
				{
					if (_AFSwitch == true)
					{
						int nRet = APP_OK;

						nRet = m_pAFTSClass->AFSwitchOff();
						SetDlgItemText(IDC_AFSwitch, _T("AF ON"));
						GetDlgItem(IDC_AFMODE_1)->EnableWindow(TRUE);
						GetDlgItem(IDC_AFMODE_2)->EnableWindow(TRUE);

						return nRet;
					}
					else {

						CString strLogTemp;

						strLogTemp.Format(_T("[%s][%s] AF Off [AF Swich Off State] Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

						theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

						return APP_NG;
					}

				}
				else {

					CString strLogTemp;

					strLogTemp.Format(_T("[%s][%s] AF Off [AF Mode Measure State] Fail!!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

					theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

					return APP_NG;

				}
				/*else if (m_MeasureModeBtn.GetCheck() == true)
				{
					CString Status;
					GetDlgItemText(IDC_AFSwitch, Status);
					if (Status == "AF ON")
					{
						m_pAFTSClass->MeasureModeOn();
						SetDlgItemText(IDC_AFSwitch, _T("AF OFF"));
						GetDlgItem(IDC_AFMODE_1)->EnableWindow(FALSE);
						GetDlgItem(IDC_AFMODE_2)->EnableWindow(FALSE);
					}

				}*/

			}
			else
			{
				CString strLogTemp;

				strLogTemp.Format(_T("[%s][%s] AF Off [AFCAM OFF State] Fail!!!- %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

				theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

				return APP_NG;
			}
		}
		else
		{
			CString strLogTemp;

			strLogTemp.Format(_T("[%s][%s] AF Off [Discovery OFF State] Fail!!!- %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

			theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

			return APP_NG;
		}
	}
	catch (CException* e)
	{
		CString strLogTemp;

		strLogTemp.Format(_T("[%s][%s] AF Off Error!! - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

		theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);

		return APP_NG;
	}
}

void CAFTSSimplifiedExampleMFCDlg::PingTest(int index) {

	Sleep(1000);

	CString strLogTemp;
	strLogTemp.Format(_T("[%s][%s] AF ping test - %3.3f"), theApp.m_strLog_AF_Position, theApp.m_strLog_AF_Stage, theApp.m_AFTactTime.Stop(true));

	theApp.m_pLogWriter->m_fnWriteLog(strLogTemp);
}
