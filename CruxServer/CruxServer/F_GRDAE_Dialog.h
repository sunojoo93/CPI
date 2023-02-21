#pragma once
#include "DataManager.h"

// F_GRDAE_Dialog ��ȭ �����Դϴ�.

class F_GRDAE_Dialog : public CDialogEx
{
	DECLARE_DYNAMIC(F_GRDAE_Dialog)

public:
	F_GRDAE_Dialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~F_GRDAE_Dialog();
	CDataManager* m_pDataManager;
	

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()
public:
	CFont g_editFont;			// ryu 18.10.11   
	CFont g_edit_opid;			// ryu 19.01.01 
	CFont m_static_font;	    // ryu 18.09.29 
	CFont m_static_opid;		// ryu 19.01.01 
	CString opid_strValue;     //19.01.10 by ryu

	int opid_set_flag, juduge_set_flag;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
};
