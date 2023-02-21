
// VSCameraTaskDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

// for Tray Icon - User Message
#define	WM_TRAYICON_MSG	WM_USER + 1

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

// CVSCameraTaskDlg ��ȭ ����
class CVSCameraTaskDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CVSCameraTaskDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	
// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_VSCameraTask_DIALOG };

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
private:
	// Ʈ���� ������ �߰�
	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();
	BOOL m_bTrayStatus;
	CString ToString(int value)
	{
		CString ret;
		ret.Format(_T("%d"), value);
		return ret;
	};
private:
	void m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText);	
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	CComboBox m_comboCamTap;
	CComboBox m_comboCamTest;
	CComboBox m_comboCamTriMode;
	CComboBox m_comboCamVdOut;
	CButton m_checkReverse;
	afx_msg void OnCbnSelchangeComboCam();
	afx_msg void OnBnClickedCheckCamReverse();
	afx_msg void OnBnClickedButtonCam();
	afx_msg void OnEnChangeEditCam();
	afx_msg void OnBnClickedButtonTrigger();
	afx_msg void OnEnChangeEditTrigger();
protected:
	afx_msg LRESULT OnUmUiControl(WPARAM wParam, LPARAM lParam);
};
