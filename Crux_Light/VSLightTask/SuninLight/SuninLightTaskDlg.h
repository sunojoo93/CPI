
// SuninLightTaskDlg.h : 헤더 파일
//

#pragma once

// for Tray Icon - User Message
#define	WM_TRAYICON_MSG					WM_USER + 1
#define WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3

// Timer
enum {
	eTIMER_UpdateTaskStatus	= 0,	// Task 상태 UI 표시 Timer
};

#define LIGHT_RED	RGB(255,153,153)
#define DARK_RED	RGB(102,0,0)
#define LIGHT_GREEN	RGB(204,255,153)
#define DARK_GREEN	RGB(0,51,0)
#define DARK_GRAY	RGB(51,51,51)
#define WHITE		RGB(255,255,255)
#define BLACK		RGB(0,0,0)

#define MAX_GRID_LOG					1000

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
private:
	LRESULT m_fnPrintUiMsgUnicode	(WPARAM wParam, LPARAM lParam);	
	LRESULT m_fnPrintUiMsgMultiByte	(WPARAM wParam, LPARAM lParam);
	// 트레이 아이콘 추가
	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();
	BOOL m_bTrayStatus;
	CListBox m_ctrlListLog;		// ListBox 컨트롤 추가
private:
	void m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText);	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedButtonTest();
	afx_msg void OnBnClickedButtonMultiOn();
	afx_msg void OnBnClickedButtonMultiOff();
};
