
// Crux_MainPcInterfaceDlg.h : ��� ����
//

#pragma once

#include "TaskInterfacer.h"
#include "MainPcSimulator.h"

#define MAX_GRID_LOG					1000

// CCrux_MainPcInterfaceDlg ��ȭ ����
class CCrux_MainPcInterfaceDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CCrux_MainPcInterfaceDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CCrux_MainPcInterfaceDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_CRUX_MAINPCINTERFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

private:
	int	m_fnConectVisualStation();
	int m_fnTrayIconRefresh();
	double ReadFloat(char* szSection, char* szKey, float fltDefaultValue);
	HANDLE m_hProcessLockMutex;		// �ߺ� ���� ���� ���ؽ�
	TaskInterfacer* MainPcInterface;
	CListBox m_ctrlListLog;
	CMainPcSimulator* m_pMainPcSimulator;

	//BOOL m_bSimulationProgramMode;		//Program �� Simultor�� ���� �Ǿ�� �ϴ��� Flag 
	BOOL m_bSimulationMode;				//Crux �� Simulation Mode �� �����Ǿ�� �ϴ��� Flag

// �����Դϴ�.
protected:
	HICON m_hIcon;
	BOOL m_bTrayStatus;

	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();

	LRESULT m_fnPrintUiMsgUnicode(WPARAM wParam, LPARAM lParam);	
	LRESULT m_fnPrintUiMsgMultiByte(WPARAM wParam, LPARAM lParam);

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnStartSimulator();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
