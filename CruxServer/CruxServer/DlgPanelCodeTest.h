#pragma once

// CDlgPanelCodeTest 대화 상자입니다.

class CDlgPanelCodeTest : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPanelCodeTest)

public:
	CDlgPanelCodeTest(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPanelCodeTest();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PANEL_CODE_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
//	int m_nTotalPanel;
//	int m_nJudgeCode;
//	int m_nTimeLimite;
//	int m_nFirstDat;
//	int m_nFourthDat;
//	int m_nSecondDat;
//	int m_nThirdDat;
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnBnClickedBtnAction();
	afx_msg void OnBnClickedBtnInit();

	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnExit();
	virtual BOOL OnInitDialog();
};
