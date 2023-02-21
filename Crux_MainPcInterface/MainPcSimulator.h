#pragma once
#include "afxwin.h"


// CMainPcSimulator 대화 상자입니다.

class CMainPcSimulator : public CDialogEx
{
	DECLARE_DYNAMIC(CMainPcSimulator)

public:
	CMainPcSimulator(VOID* pTaskInterface = NULL, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMainPcSimulator();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SIMULATION_MODE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	 
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
