#pragma once
#include "afxwin.h"

// CGRADERANKSET 대화 상자입니다.

#pragma once

class CGRADERANKSET : public CDialogEx
{
	DECLARE_DYNAMIC(CGRADERANKSET)

public:
	CGRADERANKSET(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CGRADERANKSET();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_GRADERACNKSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	virtual BOOL OnInitDialog();

	CFileFind fileFind;			 // 파일 및 경로 찾는 객체
	CString strLogFolder;		 // 파일폴더
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
