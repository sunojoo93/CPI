#pragma once


// CDlgLuminanceSetup 대화 상자입니다.

class CDlgLuminanceSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLuminanceSetup)

public:
	CDlgLuminanceSetup(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLuminanceSetup();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_LUMINANCE_SETUP };

	int m_fnLoadParameter();
	int m_fnSaveParameter();
	void m_fnLuminanceAlarmUseCheck(BOOL bUseCheck);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//2016.03.02
	BOOL m_bUseLuminanceAlarm;
	int m_nLuminanceAlarmSetCount;
	int m_nLuminanceAlarmSetPanelCount;
	int m_nAryLuminanceLimit[MAX_LUMINANCE_DATA];
	int m_nLuminanceRange;
	BOOL m_bUseTdiMin;

	//2019.12.16 add by mby
	int m_nAryDccCheck[DCC_CHK_COUNT];
	int m_nAryDccInfoOne[DCC_CHK_COUNT];
	int m_nAryDccInfoTwo[DCC_CHK_COUNT];

	int m_nAryDccChkSizeX[DCC_COUNT];
	int m_nAryDccChkSizeY[DCC_COUNT];
	CString	m_strAryDccSizeX[DCC_SIZE_CHECK];
	CString	m_strAryDccSizeY[DCC_SIZE_CHECK];

	//2020.04.10 add by mby
	int m_nAryDccDirect[DCC_DIRECT_COUNT];

	CString	m_strRepNo;	//2020.04 16 add by mby

	int m_nAryPaCntPD[DCC_CHK_COUNT];	//2020.07.06 add by mby
	int m_nAryPaCntPDChk[DCC_CHK_COUNT];	//2020.07.06 add by mby

	int m_nAryPaCntEdge[16];	//2020.08.26 add by mby
	int m_nAryPaCntEdgeChk[16];	//2020.08.26 add by mby

	afx_msg void OnEnKillfocusEditCam1Min();
	afx_msg void OnEnKillfocusEditCam1Max();
	afx_msg void OnEnKillfocusEditCam2Min();
	afx_msg void OnEnKillfocusEditCam2Max();
	afx_msg void OnEnKillfocusEditCam3Min();
	afx_msg void OnEnKillfocusEditCam3Max();
	afx_msg void OnEnKillfocusEditCam4Min();
	afx_msg void OnEnKillfocusEditCam4Max();
	virtual void OnOK();
	afx_msg void OnBnClickedCheckLuminanceAlarmUse();
	afx_msg void OnKillfocusEditPol1Max();
	afx_msg void OnKillfocusEditPol2Max();
	afx_msg void OnKillfocusEditPol3Max();

	afx_msg void OnBnClickedBtnDccSave();
	afx_msg void OnBnClickedBtnDccReset();
	afx_msg void OnBnClickedBtnDccRcpChange();
};
