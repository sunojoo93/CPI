#pragma once
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgVisionState ��ȭ �����Դϴ�.

class CDlgVisionState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVisionState)

public:
	CDlgVisionState(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgVisionState();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_VISION_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	void m_fnInitState();
	void m_fnUpdateState();

	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void ClickGxbtnResetAlarm();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
