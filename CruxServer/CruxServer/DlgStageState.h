
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma once


// CDlgStageState ��ȭ �����Դϴ�.

class CDlgStageState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgStageState)

public:
	CDlgStageState(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgStageState();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_STAGE_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	void m_fnInitState();
	void m_fnUpdateState();
	void  SetChangePACodeMsg(CString strValue);	//2020.01.09 add by mby
	int m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
