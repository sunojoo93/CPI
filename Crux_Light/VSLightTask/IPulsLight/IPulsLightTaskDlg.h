
// VSLightTaskDlg.h : ��� ����
//

#pragma once

// for Tray Icon - User Message
#define	WM_TRAYICON_MSG	WM_USER + 1

// Timer
enum {
	eTIMER_UpdateTaskStatus	= 0,	// Task ���� UI ǥ�� Timer
	eTIMER_UpdateLightStatus = 1,
};

#define LIGHT_RED	RGB(255,153,153)
#define DARK_RED	RGB(102,0,0)
#define LIGHT_GREEN	RGB(204,255,153)
#define DARK_GREEN	RGB(0,51,0)
#define DARK_GRAY	RGB(51,51,51)
#define WHITE		RGB(255,255,255)
#define BLACK		RGB(0,0,0)

// CVSLightTaskDlg ��ȭ ����
class CVSLightTaskDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CVSLightTaskDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VSLightTask_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.	


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
	// Ʈ���� ������ �߰�
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
	CString AlarmSOP[9] = { _T("���� �µ� ȯ�濡�� ����ϰų� ����ȭ�� ���� �߿� �� PCB �µ��� ����� �� �ִ�. ������ ���̽��� �濭�� �ߵǴ� �������� �����Ͽ� �濭��Ų��."), _T("FPGA ��� ������ ���� FPGA Upgrade�Ŀ� �ش� ���� �߻� �� FPGA Upgrade�� ������Ѵ�."), _T("�ش� ���� �߻� �� �����翡 �����Ѵ�."), _T("LED ����� ������ ���к��� ���� ��� Alarm�� �߻��ȴ�."), _T("LED �輱�� Short �Ǿ��ų� LED+ ���������� 2V���� ���� ��� Alarm�� �߻��ϸ� LED �輱�� ���� �� ������ �����Ѵ�."), _T("LED ��¿� ���ϰ� ���� ��� Alarm�� ���۵ȴ�. \nLED �ؼ� �� ���� ���¸� �����Ѵ�. \nLED ������ ���� ���к��� ���� ��� �߱����� ������ ���������� �ִ�ġ�� ������ ���۽�Ų��."), _T("LED ��¿� ���ϰ� ���� ��� Alarm�� ���۵ȴ�. \nLED �ؼ� �� ���� ���¸� �����Ѵ�. \nLED ������ ���� ���к��� ���� ��� �߱����� ������ ���������� �ִ�ġ�� ������ ���۽�Ų��."), _T("LED ��¿� ���ϰ� ���� ��� Alarm�� ���۵ȴ�. \nLED �ؼ� �� ���� ���¸� �����Ѵ�. \nLED ������ ���� ���к��� ���� ��� �߱����� ������ ���������� �ִ�ġ�� ������ ���۽�Ų��."), _T("LED ��¿� ���ϰ� ���� ��� Alarm�� ���۵ȴ�. \nLED �ؼ� �� ���� ���¸� �����Ѵ�. \nLED ������ ���� ���к��� ���� ��� �߱����� ������ ���������� �ִ�ġ�� ������ ���۽�Ų��.") };
	CString AlarmCode[9] = { _T("0x0002"),_T("0x0008") ,_T("0x0010") ,_T("0x0020") ,_T("0x0040") ,_T("0x0100") ,_T("0x0200") ,_T("0x0400") ,_T("0x0800") };
	afx_msg void OnBnClickedBtnAlarmReset2();
};
