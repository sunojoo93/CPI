#pragma once
#include "Login.h"

// CDlgDefectMap 대화 상자입니다.

class CDlgDefectInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectInfo)

public:
	STRU_DEFECT_TYPE m_stDefectType;
	CDlgDefectInfo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDefectInfo();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DEF_INFO };
private:

	CLogin* m_pDlgLogin;
	STRU_DEFECT_TYPE m_stTotalDefectType;
	void m_fnGetTotalDefType(STRU_DEFECT_TYPE stDefType);
	CToolTipCtrl m_tip_ctrl;
	CString m_strDef;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bMaualFlg;
	void  SetDlgDefTypeVal();
	LRESULT m_fnUpdateDefType(WPARAM wParam, LPARAM lParam);
	//void m_fnUpdateDefType(STRU_DEFECT_TYPE stDefType);
	void m_fnInitDefectInfo();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtnPnlCntReset();
	CFont m_boldFont, *m_oriFont;
	virtual BOOL OnInitDialog();
	void m_fnChangeFont(CString strDef);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
