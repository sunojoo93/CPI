#pragma once
#include "afxwin.h"

// CGRADERANKSET ��ȭ �����Դϴ�.

#pragma once

class CGRADERANKSET : public CDialogEx
{
	DECLARE_DYNAMIC(CGRADERANKSET)

public:
	CGRADERANKSET(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CGRADERANKSET();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_GRADERACNKSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();

	CFileFind fileFind;			 // ���� �� ��� ã�� ��ü
	CString strLogFolder;		 // ��������
	CString	m_strRepNo;			 // Recipe Number

	//Grade A
	int m_nAryGradeRankChkA_X[16];
	int m_nAryGradeRankChkA_Y[16];
	CString	m_strAryGradeRankA_X[16];
	CString	m_strAryGradeRankA_Y[16];

	//Grade B1
	int m_nAryGradeRankChkB_X[16];
	int m_nAryGradeRankChkB_Y[16];
	CString	m_strAryGradeRankB_X[32];
	CString	m_strAryGradeRankB_Y[32];

	CString	m_strAryGradeB_Rank[16];

	//Grade B2
	int m_nAryGradeRankChkB2_X[16];
	int m_nAryGradeRankChkB2_Y[16];
	CString	m_strAryGradeRankB2_X[32];
	CString	m_strAryGradeRankB2_Y[32];

	CString	m_strAryGradeB2_Rank[16];

	//Grade C
	int m_nAryGradeRankChkC_X[16];
	int m_nAryGradeRankChkC_Y[16];
	CString	m_strAryGradeRankC_X[16];
	CString	m_strAryGradeRankC_Y[16];


	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnGraderankRcpChange();
};
