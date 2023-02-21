
// VSSequenceTaskDlg.h : ��� ����
//

#pragma once



// Timer
enum {
	eTIMER_UpdateTaskStatus	= 0,	// Task ���� UI ǥ�� Timer
};

#define LIGHT_RED	RGB(255,153,153)
#define DARK_RED	RGB(102,0,0)
#define LIGHT_GREEN	RGB(204,255,153)
#define DARK_GREEN	RGB(0,51,0)
#define DARK_GRAY	RGB(51,51,51)
#define WHITE		RGB(255,255,255)
#define BLACK		RGB(0,0,0)

// CVSSequenceTaskDlg ��ȭ ����
class CVSSequenceTaskDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CVSSequenceTaskDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VSSequenceTask_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.	


// �����Դϴ�.
protected:
	HICON m_hIcon;
//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);


	// ������ �޽��� �� �Լ�
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
	// Ʈ���� ������ �߰�
	LRESULT TrayIconMessage			(WPARAM wParam, LPARAM lParam);

	void TraySetting();
	int m_fnTrayIconRefresh();
	BOOL m_bTrayStatus;
	CListBox m_ctrlListLog;
	

private:
	void m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText);	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedButton2();
	BOOL m_bDlgShow = FALSE;
	BOOL ShowWindowEx(int nCmdShow);


//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
//	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
//	afx_msg void OnMove(int x, int y);
};
