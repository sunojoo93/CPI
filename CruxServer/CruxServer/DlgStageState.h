
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma once


// CDlgStageState 대화 상자입니다.

class CDlgStageState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStageState)

public:
	CDlgStageState(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgStageState();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_STAGE_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	void m_fnInitState();
	void m_fnUpdateState();
	void  SetChangePACodeMsg(CString strValue);	//2020.01.09 add by mby
	int m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
