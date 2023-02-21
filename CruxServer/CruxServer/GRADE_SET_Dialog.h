#pragma once
#include "afxwin.h"
#include "GRADERANKSET.h"

// GRADE_SET_Dialog 대화 상자입니다.

class GRADE_SET_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(GRADE_SET_Dialog)

public:
	GRADE_SET_Dialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~GRADE_SET_Dialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IDC_GRADE_SET};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedSetBtn1();

	CGRADERANKSET* m_pDlgGradeRankSet;	//2020.07.22 add by mby

	int m_radio_button1;  // 1
	int m_radio_button2;  // 2
	int m_radio_button3;  // 3
	int m_radio_button4;  // 4
	int m_radio_button5;  // 5
	int m_radio_button6;  // 6
	int m_radio_button7;  // 7
	int m_radio_button8;  // 8
	int m_radio_button9;  // 9
	int m_radio_button10;  // 10
	int m_radio_button11;  // 11  2019.01.12 by ryu 
	int m_radio_button12;  // 12  2019.01.12 by ryu 
	int m_radio_button13;  // 13  2019.01.30 by ryu 
	int m_radio_button14;  // 13  2019.01.30 by ryu
	int m_radio_button15;  // 14  2020.02.13 by mby 

	afx_msg void OnBnClickedOk();

	int tft_cornerbroken;   //1
	int cf_cornerbroken;	//2
	int tft_broken;			//3
	int cf_broken;			//4
	int tft_burr;			//5
	int cf_burr;			//6
	int tft_scarch;			//7
	int cf_scarch;			//8
	int tft_cell_broken;	//9
	int cf_cell_broken;		//10

	// grade_setting()추가 -- S by ryu 2018.11.23
	CFileFind fileFind;			 // 파일 및 경로 찾는 객체
	CString strLogFolder;		 // 파일폴더
	// grade_setting()추가 -- E 
	CString strKey, strSection;  // 섹션

	virtual BOOL OnInitDialog();

	//---------------- S ------------- _2018.11.29 by ryu
	
	CString m_strValue_Grade[MAX_COUNT_GRADE];		 // 13 set
	CString m_strValue_Num[VALUE_COUNT];		 // 
	CString m_strValue_Rank[MAX_COUNT_RANK];
	CEdit m_edit_tft_cf[9];
	//---------------- E -------------
	afx_msg void OnBnClickedCancel();
	
	CEdit m_rank_edit[MAX_COUNT_RANK];

	CString	m_strRepNo;	//2020.07.01 add by mby
	void m_GradeRadioInfoSet(int Array, CString Grade );	//2020.07.01 add by mby
	afx_msg void OnBnClickedBtnGradesetRcpChange();		//2020.07.01 add by mby
	afx_msg void OnBnClickedBtnGraderankOpen();
};
