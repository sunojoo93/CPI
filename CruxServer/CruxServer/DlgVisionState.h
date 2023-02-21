#pragma once
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgVisionState 대화 상자입니다.

class CDlgVisionState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVisionState)

public:
	CDlgVisionState(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgVisionState();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_VISION_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void m_fnInitState();
	void m_fnUpdateState();

	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickGxbtnResetAlarm();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
