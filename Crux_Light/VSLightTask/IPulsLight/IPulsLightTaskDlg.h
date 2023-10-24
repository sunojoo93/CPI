
// VSLightTaskDlg.h : 헤더 파일
//

#pragma once

// for Tray Icon - User Message
#define	WM_TRAYICON_MSG	WM_USER + 1

// Timer
enum {
	eTIMER_UpdateTaskStatus	= 0,	// Task 상태 UI 표시 Timer
	eTIMER_UpdateLightStatus = 1,
};

#define LIGHT_RED	RGB(255,153,153)
#define DARK_RED	RGB(102,0,0)
#define LIGHT_GREEN	RGB(204,255,153)
#define DARK_GREEN	RGB(0,51,0)
#define DARK_GRAY	RGB(51,51,51)
#define WHITE		RGB(255,255,255)
#define BLACK		RGB(0,0,0)

// CVSLightTaskDlg 대화 상자
class CVSLightTaskDlg : public CDialogEx
{
// 생성입니다.
public:
	CVSLightTaskDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VSLightTask_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.	


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	UINT OcuuredAlarmCode = 0;
private:
	// 트레이 아이콘 추가
	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();
	BOOL m_bTrayStatus;
private:
	void m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText);	
	bool AutoRefresh = false;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButtonOn();
	afx_msg void OnBnClickedButtonOff();
	afx_msg void OnBnClickedRadioOpOff();
	afx_msg void OnBnClickedRadioOpCon();
	afx_msg void OnBnClickedRadioOpPulse();
	afx_msg void OnBnClickedRadioSeqmodeOn();
	afx_msg void OnBnClickedRadioSqemodeOff();
	afx_msg void OnBnClickedBtnGetSqeidx();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedChkLed1();
	afx_msg void OnBnClickedChkLed2();
	afx_msg void OnBnClickedChkLed3();
	afx_msg void OnBnClickedChkLed4();
	afx_msg void OnBnClickedBtnGetTrgcnt();
	afx_msg void OnBnClickedBtnGetErrcnt();
	afx_msg void OnBnClickedBtnGetAlarmcnt();
	afx_msg void OnBnClickedBtnTrgcntReset();
	afx_msg void OnBnClickedBtnErrorcntReset();
	afx_msg void OnBnClickedBtnAlarmReset();
	afx_msg void OnBnClickedBtnSeqidxReset();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedChkAutorefresh();
	afx_msg void OnEnChangeTbConti1Rate();
	afx_msg void OnBnClickedRadioTrgInternal();

	CString AlarmDesc[9] = { _T("PCB Temperature over 70 degree"), _T("FPGA Register Check Error"), _T("Flash Memory Check Error"), _T("Power Range Over"), _T("LED Short Circuit"), _T("LED1 Not Connected"), _T("LED2 Not Connected"), _T("LED3 Not Connected"), _T("LED4 Not Connected") };
	CString AlarmSOP[9] = { _T("높은 온도 환경에서 사용하거나 과부화에 의한 발열 시 PCB 온도가 상승할 수 있다. 가급적 케이스를 방열이 잘되는 구조물에 부착하여 방열시킨다."), _T("FPGA 통신 에러로 만일 FPGA Upgrade후에 해당 증상 발생 시 FPGA Upgrade를 재수행한다."), _T("해당 증상 발생 시 제조사에 문의한다."), _T("LED 출력이 설정된 전압보다 높을 경우 Alarm이 발생된다."), _T("LED 배선이 Short 되었거나 LED+ 구동전압이 2V보다 낮을 경우 Alarm이 발생하며 LED 배선을 제거 후 증상을 점검한다."), _T("LED 출력에 부하가 없을 경우 Alarm이 동작된다. \nLED 극성 및 연결 상태를 점검한다. \nLED 전압이 구동 전압보다 높은 경우 발광되지 않으며 구동전압을 최대치로 높여서 동작시킨다."), _T("LED 출력에 부하가 없을 경우 Alarm이 동작된다. \nLED 극성 및 연결 상태를 점검한다. \nLED 전압이 구동 전압보다 높은 경우 발광되지 않으며 구동전압을 최대치로 높여서 동작시킨다."), _T("LED 출력에 부하가 없을 경우 Alarm이 동작된다. \nLED 극성 및 연결 상태를 점검한다. \nLED 전압이 구동 전압보다 높은 경우 발광되지 않으며 구동전압을 최대치로 높여서 동작시킨다."), _T("LED 출력에 부하가 없을 경우 Alarm이 동작된다. \nLED 극성 및 연결 상태를 점검한다. \nLED 전압이 구동 전압보다 높은 경우 발광되지 않으며 구동전압을 최대치로 높여서 동작시킨다.") };
	CString AlarmCode[9] = { _T("0x0002"),_T("0x0008") ,_T("0x0010") ,_T("0x0020") ,_T("0x0040") ,_T("0x0100") ,_T("0x0200") ,_T("0x0400") ,_T("0x0800") };
	afx_msg void OnBnClickedBtnAlarmReset2();
};
