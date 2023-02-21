#pragma once
#include "DlgMessage.h"
#include "afxwin.h"

// CDlgDefectMap ��ȭ �����Դϴ�.

class CDlgPanelJudge : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPanelJudge)

public:
	CDlgPanelJudge(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPanelJudge();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_PNL_JUDGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnPaint();
	DECLARE_EVENTSINK_MAP()

public: 
	CString m_cPanelID		;
	CString m_cGrade		;
	CString m_cCFResult		;
	CString m_cTftResult	;
	CString m_cPenetResult	;
	CDlgMessage					   *m_pDlgMessage;
	//2015.09.11 Mr.Chu
	CString m_cDefCode;
	//2018.04.16 YHS
	CString m_cDefCodeName;
public:

	LRESULT m_fnInitState(WPARAM wParam, LPARAM lParam);
	LRESULT m_fnUpdateJudgeInfo(WPARAM wParam, LPARAM lParam);
	//void m_fnUpdateJudgeInfo(CString cDefectVision, CString CPanelId);
	void ClickGxbtnResetAlarm();
	void ChangeResetTime();
	void  SetResultPanelId(CString strValue)	;
	void  SetResultGrade(CString strValue)		;
	void  SetResultCfJudge(CString strValue)	;
	void  SetResultTftJudge(CString strValue)	;
	void  SetResultPenetJudge(CString strValue);
	void  SetResultDefectCodeJudge(CString strValue);
	void  SetResultDefectCodeName(CString strValue);
	CString GetResultPanelId()		;
	CString GetResultGrade()		;
	CString GetResultCFJudge()		;
	CString GetResultTftJudge()	;
	CString GetResultPenetJudge()	;
	CString GetResultDefCode(void);		//2016.01.04 Mr.Chu
	CEdit m_edtResetTime;
	CString GetResultDefCodeName()	;	//2018.04.18 YHS

};
