#pragma once
#include "afxwin.h"


// CMainPcSimulator ��ȭ �����Դϴ�.

class CMainPcSimulator : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPcSimulator)

public:
	CMainPcSimulator(VOID* pTaskInterface = NULL, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMainPcSimulator();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SIMULATION_MODE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	 
	DECLARE_MESSAGE_MAP()
private:
	CStringArray		m_strCellID;
	CListBox			m_ctrlListAutoRunCell;
	VOID*				m_pTaskInterface;
	int					m_nStartCellNo, m_nEndCellNo, m_nRepeat, m_nDelay;

	int m_fnLoadCellIdList();
	int m_fnShowCellIdList();
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnReloadCellList();
	afx_msg void OnBnClickedBtnSimulStart();
	afx_msg void OnBnClickedBtnSimulStop();
};
