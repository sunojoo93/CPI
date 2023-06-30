
// VSLightTaskDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VSLightTask.h"
#include "IPulsLightTaskDlg.h"
#include "afxdialogex.h"
#include "define.h"
#include "version.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVSLightTaskDlg 대화 상자

CVSLightTaskDlg::CVSLightTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSLightTaskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVSLightTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVSLightTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BUTTON_ON, &CVSLightTaskDlg::OnBnClickedButtonOn)
	ON_BN_CLICKED(IDC_BUTTON_OFF, &CVSLightTaskDlg::OnBnClickedButtonOff)
	ON_BN_CLICKED(IDC_RADIO_OP_OFF, &CVSLightTaskDlg::OnBnClickedRadioOpOff)
	ON_BN_CLICKED(IDC_RADIO_OP_CON, &CVSLightTaskDlg::OnBnClickedRadioOpCon)
	ON_BN_CLICKED(IDC_RADIO_OP_PULSE, &CVSLightTaskDlg::OnBnClickedRadioOpPulse)

	ON_BN_CLICKED(IDC_RADIO_SEQMODE_ON, &CVSLightTaskDlg::OnBnClickedRadioSeqmodeOn)
	ON_BN_CLICKED(IDC_RADIO_SQEMODE_OFF, &CVSLightTaskDlg::OnBnClickedRadioSqemodeOff)
	ON_BN_CLICKED(IDC_BTN_GET_SQEIDX, &CVSLightTaskDlg::OnBnClickedBtnGetSqeidx)

	ON_BN_CLICKED(IDC_CHK_LED1, &CVSLightTaskDlg::OnBnClickedChkLed1)
	ON_BN_CLICKED(IDC_CHK_LED2, &CVSLightTaskDlg::OnBnClickedChkLed2)
	ON_BN_CLICKED(IDC_CHK_LED3, &CVSLightTaskDlg::OnBnClickedChkLed3)
	ON_BN_CLICKED(IDC_CHK_LED4, &CVSLightTaskDlg::OnBnClickedChkLed4)
	ON_BN_CLICKED(IDC_BTN_GET_TRGCNT, &CVSLightTaskDlg::OnBnClickedBtnGetTrgcnt)
	ON_BN_CLICKED(IDC_BTN_GET_ERRCNT, &CVSLightTaskDlg::OnBnClickedBtnGetErrcnt)
	ON_BN_CLICKED(IDC_BTN_GET_ALARMCNT, &CVSLightTaskDlg::OnBnClickedBtnGetAlarmcnt)
	ON_BN_CLICKED(IDC_BTN_TRGCNT_RESET, &CVSLightTaskDlg::OnBnClickedBtnTrgcntReset)
	ON_BN_CLICKED(IDC_BTN_ERRORCNT_RESET, &CVSLightTaskDlg::OnBnClickedBtnErrorcntReset)
	ON_BN_CLICKED(IDC_BTN_ALARM_RESET, &CVSLightTaskDlg::OnBnClickedBtnAlarmReset)
	ON_BN_CLICKED(IDC_BTN_SEQIDX_RESET, &CVSLightTaskDlg::OnBnClickedBtnSeqidxReset)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CVSLightTaskDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_LOAD, &CVSLightTaskDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_CHK_AUTOREFRESH, &CVSLightTaskDlg::OnBnClickedChkAutorefresh)
	ON_BN_CLICKED(IDC_RADIO_TRG_INTERNAL, &CVSLightTaskDlg::OnBnClickedRadioTrgInternal)
	ON_BN_CLICKED(IDC_BTN_ALARM_RESET2, &CVSLightTaskDlg::OnBnClickedBtnAlarmReset2)
END_MESSAGE_MAP()


// CVSLightTaskDlg 메시지 처리기

BOOL CVSLightTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	this->SetWindowText(theApp.m_pLight->GetControllerName());

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// Auto Mode 시 최소화 시작
	//if (!theApp.IsManualMode())
	//	ShowWindow(SW_MINIMIZE);

	// 버전 표기
	CString title;
	title.Format(_T("IPuls Light %s"), _Version);
	SetWindowText(title);

	// Icon 변경
	m_bTrayStatus = FALSE;
	TraySetting();	

	// Task 상태 갱신 Timer
	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);	
	SetTimer(eTIMER_UpdateLightStatus, 500, NULL);
	CString SlaveID;
	SlaveID.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0200, 1));
	GetDlgItem(IDC_TB_SLAVEID)->SetWindowTextW(SlaveID);

	((CButton*)GetDlgItem(IDC_CHK_LED1))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0320, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED2))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0321, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED3))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0322, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED4))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0323, 1));
	WORD OperationMode = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0300, 1);
	if (OperationMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_OP_OFF))->SetCheck(TRUE);
	if (OperationMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_OP_CON))->SetCheck(TRUE);
	if (OperationMode == 2)
		((CButton*)GetDlgItem(IDC_RADIO_OP_PULSE))->SetCheck(TRUE);

	WORD SeqMode = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0305, 1);
	if (SeqMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_SQEMODE_OFF))->SetCheck(TRUE);
	if (SeqMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_SEQMODE_ON))->SetCheck(TRUE);

	CString Conti1Rate;
	Conti1Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0330, 1));
	GetDlgItem(IDC_TB_CONTI1_RATE)->SetWindowTextW(Conti1Rate);

	CString Conti2Rate;
	Conti2Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0331, 1));
	GetDlgItem(IDC_TB_CONTI2_RATE)->SetWindowTextW(Conti2Rate);

	CString Conti3Rate;
	Conti3Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0332, 1));
	GetDlgItem(IDC_TB_CONTI3_RATE)->SetWindowTextW(Conti3Rate);

	CString Conti4Rate;
	Conti4Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0333, 1));
	GetDlgItem(IDC_TB_CONTI4_RATE)->SetWindowTextW(Conti4Rate);

	CString Pulse1Rate;
	Pulse1Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0340, 1));
	GetDlgItem(IDC_TB_PULSE1_RATE)->SetWindowTextW(Pulse1Rate);

	CString Pulse2Rate;
	Pulse2Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0341, 1));
	GetDlgItem(IDC_TB_PULSE2_RATE)->SetWindowTextW(Pulse2Rate);

	CString Pulse3Rate;
	Pulse3Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0342, 1));
	GetDlgItem(IDC_TB_PULSE3_RATE)->SetWindowTextW(Pulse3Rate);

	CString Pulse4Rate;
	Pulse4Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0343, 1));
	GetDlgItem(IDC_TB_PULSE4_RATE)->SetWindowTextW(Pulse4Rate);

	CString TriggerCount;
	TriggerCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0100, 2));
	GetDlgItem(IDC_TB_TRGCNT)->SetWindowTextW(TriggerCount);

	CString ErrorCount;
	ErrorCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0102, 2));
	GetDlgItem(IDC_TB_ERRCNT)->SetWindowTextW(ErrorCount);

	CString AlarmCount;
	AlarmCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0104, 2));
	GetDlgItem(IDC_TB_ALARMCNT)->SetWindowTextW(AlarmCount);

	CString SequenceIndex;
	SequenceIndex.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0106, 2));
	GetDlgItem(IDC_TB_SEQIDX)->SetWindowTextW(SequenceIndex);

	CString SeqStart;
	SeqStart.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0306, 1));
	GetDlgItem(IDC_TB_SEQ_START)->SetWindowTextW(SeqStart);

	CString SeqCount;
	SeqCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0307, 1));
	GetDlgItem(IDC_TB_SEQ_COUNT)->SetWindowTextW(SeqCount);

	CString SeqIdx0;
	SeqIdx0.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0380, 1));
	GetDlgItem(IDC_TB_SEQ_IDX0)->SetWindowTextW(SeqIdx0);

	CString SeqIdx1;
	SeqIdx1.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0381, 1));
	GetDlgItem(IDC_TB_SEQ_IDX1)->SetWindowTextW(SeqIdx1);

	CString SeqIdx2;
	SeqIdx2.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0382, 1));
	GetDlgItem(IDC_TB_SEQ_IDX2)->SetWindowTextW(SeqIdx2);

	CString SeqIdx3;
	SeqIdx3.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0383, 1));
	GetDlgItem(IDC_TB_SEQ_IDX3)->SetWindowTextW(SeqIdx3);

	CString SeqIdx4;
	SeqIdx4.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0384, 1));
	GetDlgItem(IDC_TB_SEQ_IDX4)->SetWindowTextW(SeqIdx4);

	CString SeqIdx5;
	SeqIdx5.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0385, 1));
	GetDlgItem(IDC_TB_SEQ_IDX5)->SetWindowTextW(SeqIdx5);

	CString SeqIdx6;
	SeqIdx6.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0386, 1));
	GetDlgItem(IDC_TB_SEQ_IDX6)->SetWindowTextW(SeqIdx6);

	CString SeqIdx7;
	SeqIdx7.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0387, 1));
	GetDlgItem(IDC_TB_SEQ_IDX7)->SetWindowTextW(SeqIdx7);

	CString Led_Rated1;
	Led_Rated1.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0350, 1));
	GetDlgItem(IDC_TB_RATED1)->SetWindowTextW(Led_Rated1);

	CString Led_Rated2;
	Led_Rated2.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0351, 1));
	GetDlgItem(IDC_TB_RATED2)->SetWindowTextW(Led_Rated2);

	CString Led_Rated3;
	Led_Rated3.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0352, 1));
	GetDlgItem(IDC_TB_RATED3)->SetWindowTextW(Led_Rated3);

	CString Led_Rated4;
	Led_Rated4.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0353, 1));
	GetDlgItem(IDC_TB_RATED4)->SetWindowTextW(Led_Rated4);

	CString Duration;
	Duration.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0310, 2));
	GetDlgItem(IDC_TB_DURATION)->SetWindowTextW(Duration);

	CString Period;
	Period.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0312, 2));
	GetDlgItem(IDC_TB_PERIOD)->SetWindowTextW(Period);

	CString TriggerDelay;
	TriggerDelay.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0314, 2));
	GetDlgItem(IDC_TB_TRGDELAY)->SetWindowTextW(TriggerDelay);

	CString Voltage;
	Voltage.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0316, 1));
	GetDlgItem(IDC_TB_MAXVOLT)->SetWindowTextW(Voltage);

	CString MultiTrigger;
	MultiTrigger.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0318, 2));
	GetDlgItem(IDC_TB_MULTITRG)->SetWindowTextW(MultiTrigger);

	WORD TrgInput = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0301, 1);
	if (TrgInput == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_INTERNAL))->SetCheck(TRUE);
	if (TrgInput == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_DIGIO))->SetCheck(TRUE);
	if (TrgInput == 2)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_RJ45))->SetCheck(TRUE);
	if (TrgInput == 3)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_SOFT))->SetCheck(TRUE);
	if (TrgInput == 4)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_CHAPORT))->SetCheck(TRUE);

	WORD TrgActivation = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0302, 1);
	if (TrgActivation == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_RISING))->SetCheck(TRUE);
	if (TrgActivation == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_FALLING))->SetCheck(TRUE);


	WORD TrgOutput = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0303, 1);
	if (SeqMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_LED))->SetCheck(TRUE);
	if (SeqMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_TRGINPUT))->SetCheck(TRUE);
	if (SeqMode == 2)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_ERREVTSGN))->SetCheck(TRUE);
	if (SeqMode == 3)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_LOW))->SetCheck(TRUE);
	if (SeqMode == 4)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_HIGH))->SetCheck(TRUE);
	if (SeqMode == 5)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_BYPASS))->SetCheck(TRUE);
	//theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0331, 1);
	//theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0332, 1);
	//theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0333, 1);
	//theApp.m_pLight->W
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CVSLightTaskDlg::OnPaint()
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
HCURSOR CVSLightTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVSLightTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
		//ShowWindow(SW_HIDE);
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
void CVSLightTaskDlg::TraySetting(void)
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


BOOL CVSLightTaskDlg::PreTranslateMessage(MSG* pMsg)
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
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_CONTI1_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI1_RATE)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0330, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_CONTI2_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI2_RATE)->GetWindowTextW(Data);		
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0331, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_CONTI3_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI3_RATE)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0332, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_CONTI4_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI4_RATE)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0333, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_PULSE1_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI1_RATE)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0340, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_PULSE2_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI1_RATE)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0341, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_PULSE3_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI1_RATE)->GetWindowTextW(Data);	
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0342, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_PULSE4_RATE)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_CONTI1_RATE)->GetWindowTextW(Data);	
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0343, Value);
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_START)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_START)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0306, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_COUNT)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_COUNT)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0307, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX0)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX0)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0380, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX1)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX1)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0381, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX2)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX2)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0382, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX3)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX3)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0383, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX4)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX4)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0384, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX5)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX5)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0385, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX6)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX6)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0386, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_SEQ_IDX7)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX7)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0387, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_RATED1)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX7)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0350, 0);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_RATED2)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX7)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0351, 0);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_RATED3)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX7)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0352, 0);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_RATED4)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_SEQ_IDX7)->GetWindowTextW(Data);
		WORD Value = _ttoi(Data);
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0353, 0);
	}


	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_DURATION)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		CString Data;
		GetDlgItem(IDC_TB_DURATION)->GetWindowTextW(Data);
		FLOAT Value = _tstof(Data);
		theApp.m_pLight->Write_Func6_FLOAT((BYTE)0, (BYTE)0x06, (WORD)0x0310, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_PERIOD)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		//CString Data;
		//GetDlgItem(IDC_TB_PERIOD)->GetWindowTextW(Data);
		//FLOAT Value = _tstof(Data);
		//theApp.m_pLight->Write_Func6_FLOAT((BYTE)0, (BYTE)0x06, (WORD)0x0312, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_TRGDELAY)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		//CString Data;
		//GetDlgItem(IDC_TB_TRGDELAY)->GetWindowTextW(Data);
		//FLOAT Value = _tstof(Data);
		//theApp.m_pLight->Write_Func6_FLOAT((BYTE)0, (BYTE)0x06, (WORD)0x0314, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_MAXVOLT)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		//CString Data;
		//GetDlgItem(IDC_TB_MAXVOLT)->GetWindowTextW(Data);
		//WORD Value = _ttoi(Data);
		//theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0316, Value);
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == GetDlgItem(IDC_TB_MULTITRG)->m_hWnd && pMsg->wParam == VK_RETURN)
	{
		//CString Data;
		//GetDlgItem(IDC_TB_MULTITRG)->GetWindowTextW(Data);
		//WORD Value = _ttoi(Data);
		//theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0318, Value);
	}

	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);

}

// 트레이 아이콘 추가
LRESULT CVSLightTaskDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	return 0L;
}


void CVSLightTaskDlg::OnDestroy()
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

void CVSLightTaskDlg::OnTimer(UINT_PTR nIDEvent)
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
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_LIGHT);
		::InvalidateRect(hButton, NULL, FALSE);
		break;
	case eTIMER_UpdateLightStatus:
		if (AutoRefresh)
		{
			CString TriggerCount;
			TriggerCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0100, 2));
			GetDlgItem(IDC_TB_TRGCNT)->SetWindowTextW(TriggerCount);

			CString ErrorCount;
			ErrorCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0102,2));
			GetDlgItem(IDC_TB_ERRCNT)->SetWindowTextW(ErrorCount);

			CString AlarmCount;
			AlarmCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0104, 2));
			GetDlgItem(IDC_TB_ALARMCNT)->SetWindowTextW(AlarmCount);

			CString SequenceIndex;
			SequenceIndex.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0106, 2));
			GetDlgItem(IDC_TB_SEQIDX)->SetWindowTextW(SequenceIndex);
		}
	}	

	CDialogEx::OnTimer(nIDEvent);
}


void CVSLightTaskDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
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
		case IDC_BTN_LIGHT:
		{
			if (theApp.GetLightState())
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

void CVSLightTaskDlg::m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText)
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

void CVSLightTaskDlg::OnBnClickedButtonOn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//STRU_LIGHT_INFO stTemp;
	//stTemp.nControllerCount = 1;
	//stTemp.stLight[0].nChCnt = 8;
	//for (int i = 0; i < stTemp.stLight[0].nChCnt; i++)
	//	stTemp.stLight[0].nLightVal[i] = 500;
	//theApp.m_pLight->MultiTurnOn(stTemp);
	theApp.m_pLight->GetTriggerCount(1);
}


void CVSLightTaskDlg::OnBnClickedButtonOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->SequenceIndexReset(2, 1);
}




void CVSLightTaskDlg::OnBnClickedRadioOpOff()
{
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0300, 0);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CVSLightTaskDlg::OnBnClickedRadioOpCon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0300, 1);
}


void CVSLightTaskDlg::OnBnClickedRadioOpPulse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0300, 2);
}



void CVSLightTaskDlg::OnBnClickedRadioSeqmodeOn()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0305, 1);
}


void CVSLightTaskDlg::OnBnClickedRadioSqemodeOff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0305, 0);
}




void CVSLightTaskDlg::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CVSLightTaskDlg::OnBnClickedChkLed1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = ((CButton*)GetDlgItem(IDC_CHK_LED1))->GetCheck();
	if (Checked)	
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0320, 1);
	
	else
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0320, 0);
}


void CVSLightTaskDlg::OnBnClickedChkLed2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = ((CButton*)GetDlgItem(IDC_CHK_LED2))->GetCheck();
	if (Checked)
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0321, 1);

	else
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0321, 0);
}


void CVSLightTaskDlg::OnBnClickedChkLed3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = ((CButton*)GetDlgItem(IDC_CHK_LED3))->GetCheck();
	if (Checked)
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0322, 1);

	else
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0322, 0);
}


void CVSLightTaskDlg::OnBnClickedChkLed4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = ((CButton*)GetDlgItem(IDC_CHK_LED4))->GetCheck();
	if (Checked)
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0323, 1);

	else
		theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0323, 0);
}


void CVSLightTaskDlg::OnBnClickedBtnGetTrgcnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString TriggerCount;
	TriggerCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0100, 2));
	GetDlgItem(IDC_TB_TRGCNT)->SetWindowTextW(TriggerCount);
}


void CVSLightTaskDlg::OnBnClickedBtnGetErrcnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString ErrorCount;
	ErrorCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0102, 2));
	GetDlgItem(IDC_TB_ERRCNT)->SetWindowTextW(ErrorCount);
}


void CVSLightTaskDlg::OnBnClickedBtnGetAlarmcnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CString AlarmCount;
	AlarmCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0104, 2));
	GetDlgItem(IDC_TB_ALARMCNT)->SetWindowTextW(AlarmCount);
}

void CVSLightTaskDlg::OnBnClickedBtnGetSqeidx()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString SequenceIndex;
	SequenceIndex.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0106, 2));
	GetDlgItem(IDC_TB_SEQIDX)->SetWindowTextW(SequenceIndex);
}

void CVSLightTaskDlg::OnBnClickedBtnTrgcntReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0006, 1);
}





void CVSLightTaskDlg::OnBnClickedBtnAlarmReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0004, 1);
}
void CVSLightTaskDlg::OnBnClickedBtnErrorcntReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0007, 1);
}

void CVSLightTaskDlg::OnBnClickedBtnSeqidxReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0008, 1);
}


void CVSLightTaskDlg::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0001, 1);
}


void CVSLightTaskDlg::OnBnClickedBtnLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	theApp.m_pLight->Write_Func6_UINT16((BYTE)0, (BYTE)0x06, (WORD)0x0002, 1);

	CString SlaveID;
	SlaveID.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0200, 1));
	GetDlgItem(IDC_TB_SLAVEID)->SetWindowTextW(SlaveID);

	((CButton*)GetDlgItem(IDC_CHK_LED1))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0320, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED2))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0321, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED3))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0322, 1));
	((CButton*)GetDlgItem(IDC_CHK_LED4))->SetCheck(theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0323, 1));
	WORD OperationMode = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0300, 1);
	if (OperationMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_OP_OFF))->SetCheck(TRUE);
	if (OperationMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_OP_CON))->SetCheck(TRUE);
	if (OperationMode == 2)
		((CButton*)GetDlgItem(IDC_RADIO_OP_PULSE))->SetCheck(TRUE);

	WORD SeqMode = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0305, 1);
	if (SeqMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_SQEMODE_OFF))->SetCheck(TRUE);
	if (SeqMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_SEQMODE_ON))->SetCheck(TRUE);

	CString Conti1Rate;
	Conti1Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0330, 1));
	GetDlgItem(IDC_TB_CONTI1_RATE)->SetWindowTextW(Conti1Rate);

	CString Conti2Rate;
	Conti2Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0331, 1));
	GetDlgItem(IDC_TB_CONTI2_RATE)->SetWindowTextW(Conti2Rate);

	CString Conti3Rate;
	Conti3Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0332, 1));
	GetDlgItem(IDC_TB_CONTI3_RATE)->SetWindowTextW(Conti3Rate);

	CString Conti4Rate;
	Conti4Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0333, 1));
	GetDlgItem(IDC_TB_CONTI4_RATE)->SetWindowTextW(Conti4Rate);

	CString Pulse1Rate;
	Pulse1Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0340, 1));
	GetDlgItem(IDC_TB_PULSE1_RATE)->SetWindowTextW(Pulse1Rate);

	CString Pulse2Rate;
	Pulse2Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0341, 1));
	GetDlgItem(IDC_TB_PULSE2_RATE)->SetWindowTextW(Pulse2Rate);

	CString Pulse3Rate;
	Pulse3Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0342, 1));
	GetDlgItem(IDC_TB_PULSE3_RATE)->SetWindowTextW(Pulse3Rate);

	CString Pulse4Rate;
	Pulse4Rate.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0343, 1));
	GetDlgItem(IDC_TB_PULSE4_RATE)->SetWindowTextW(Pulse4Rate);

	CString TriggerCount;
	TriggerCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0100, 2));
	GetDlgItem(IDC_TB_TRGCNT)->SetWindowTextW(TriggerCount);

	CString ErrorCount;
	ErrorCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0102, 2));
	GetDlgItem(IDC_TB_ERRCNT)->SetWindowTextW(ErrorCount);

	CString AlarmCount;
	AlarmCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0104, 2));
	GetDlgItem(IDC_TB_ALARMCNT)->SetWindowTextW(AlarmCount);

	CString SequenceIndex;
	SequenceIndex.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0106, 2));
	GetDlgItem(IDC_TB_SEQIDX)->SetWindowTextW(SequenceIndex);

	CString SeqStart;
	SeqStart.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0306, 1));
	GetDlgItem(IDC_TB_SEQ_START)->SetWindowTextW(SeqStart);

	CString SeqCount;
	SeqCount.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0307, 1));
	GetDlgItem(IDC_TB_SEQ_COUNT)->SetWindowTextW(SeqCount);

	CString SeqIdx0;
	SeqIdx0.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0380, 1));
	GetDlgItem(IDC_TB_SEQ_IDX0)->SetWindowTextW(SeqIdx0);

	CString SeqIdx1;
	SeqIdx1.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0381, 1));
	GetDlgItem(IDC_TB_SEQ_IDX1)->SetWindowTextW(SeqIdx1);

	CString SeqIdx2;
	SeqIdx2.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0382, 1));
	GetDlgItem(IDC_TB_SEQ_IDX2)->SetWindowTextW(SeqIdx2);

	CString SeqIdx3;
	SeqIdx3.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0383, 1));
	GetDlgItem(IDC_TB_SEQ_IDX3)->SetWindowTextW(SeqIdx3);

	CString SeqIdx4;
	SeqIdx4.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0384, 1));
	GetDlgItem(IDC_TB_SEQ_IDX4)->SetWindowTextW(SeqIdx4);

	CString SeqIdx5;
	SeqIdx5.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0385, 1));
	GetDlgItem(IDC_TB_SEQ_IDX5)->SetWindowTextW(SeqIdx5);

	CString SeqIdx6;
	SeqIdx6.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0386, 1));
	GetDlgItem(IDC_TB_SEQ_IDX6)->SetWindowTextW(SeqIdx6);

	CString SeqIdx7;
	SeqIdx7.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0387, 1));
	GetDlgItem(IDC_TB_SEQ_IDX7)->SetWindowTextW(SeqIdx7);

	CString Led_Rated1;
	Led_Rated1.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0350, 1));
	GetDlgItem(IDC_TB_RATED1)->SetWindowTextW(Led_Rated1);

	CString Led_Rated2;
	Led_Rated2.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0351, 1));
	GetDlgItem(IDC_TB_RATED2)->SetWindowTextW(Led_Rated2);

	CString Led_Rated3;
	Led_Rated3.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0352, 1));
	GetDlgItem(IDC_TB_RATED3)->SetWindowTextW(Led_Rated3);

	CString Led_Rated4;
	Led_Rated4.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0353, 1));
	GetDlgItem(IDC_TB_RATED4)->SetWindowTextW(Led_Rated4);

	CString Duration;
	Duration.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0310, 2));
	GetDlgItem(IDC_TB_DURATION)->SetWindowTextW(Duration);

	CString Period;
	Period.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0312, 2));
	GetDlgItem(IDC_TB_PERIOD)->SetWindowTextW(Period);

	CString TriggerDelay;
	TriggerDelay.Format(_T("%d"), theApp.m_pLight->Read_Func3_FLOAT((BYTE)0, (BYTE)0x03, (WORD)0x0314, 2));
	GetDlgItem(IDC_TB_TRGDELAY)->SetWindowTextW(TriggerDelay);

	CString Voltage;
	Voltage.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0316, 1));
	GetDlgItem(IDC_TB_MAXVOLT)->SetWindowTextW(Voltage);

	CString MultiTrigger;
	MultiTrigger.Format(_T("%d"), theApp.m_pLight->Read_Func3_UINT32((BYTE)0, (BYTE)0x03, (WORD)0x0318, 2));
	GetDlgItem(IDC_TB_MULTITRG)->SetWindowTextW(MultiTrigger);

	WORD TrgInput = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0301, 1);
	if (TrgInput == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_INTERNAL))->SetCheck(TRUE);
	if (TrgInput == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_DIGIO))->SetCheck(TRUE);
	if (TrgInput == 2)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_RJ45))->SetCheck(TRUE);
	if (TrgInput == 3)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_SOFT))->SetCheck(TRUE);
	if (TrgInput == 4)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_CHAPORT))->SetCheck(TRUE);

	WORD TrgActivation = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0302, 1);
	if (TrgActivation == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_RISING))->SetCheck(TRUE);
	if (TrgActivation == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_FALLING))->SetCheck(TRUE);


	WORD TrgOutput = theApp.m_pLight->Read_Func3_UINT16((BYTE)0, (BYTE)0x03, (WORD)0x0303, 1);
	if (SeqMode == 0)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_LED))->SetCheck(TRUE);
	if (SeqMode == 1)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_TRGINPUT))->SetCheck(TRUE);
	if (SeqMode == 2)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_ERREVTSGN))->SetCheck(TRUE);
	if (SeqMode == 3)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_LOW))->SetCheck(TRUE);
	if (SeqMode == 4)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_HIGH))->SetCheck(TRUE);
	if (SeqMode == 5)
		((CButton*)GetDlgItem(IDC_RADIO_TRG_BYPASS))->SetCheck(TRUE);
}


void CVSLightTaskDlg::OnBnClickedChkAutorefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = ((CButton*)GetDlgItem(IDC_CHK_AUTOREFRESH))->GetCheck();
	if (Checked)	
		AutoRefresh = true;	
	else
		AutoRefresh = false;
}


void CVSLightTaskDlg::OnEnChangeTbConti1Rate()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CVSLightTaskDlg::OnBnClickedRadioTrgInternal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CVSLightTaskDlg::OnBnClickedBtnAlarmReset2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString Code, Desc, SOP;

	GetDlgItem(IDC_TB_ALARMCNT)->GetWindowTextW(Code);

	int ArrayCount = (sizeof(AlarmDesc) / sizeof(CString));
	for (int i = 0; i < ArrayCount; ++i)
	{
		if (AlarmCode[i].MakeUpper() == Code.MakeUpper())
		{
			CString Anal;
			Anal.Format(_T("알람 설명 : %s\nSOP : %s"), AlarmDesc[i], AlarmSOP[i]);
			AfxMessageBox(Anal);
		}
	}
}
