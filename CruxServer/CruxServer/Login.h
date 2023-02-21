#pragma once

#include "Util\BIniFile.h"			// INI ���� ������

// CLogin ��ȭ �����Դϴ�.

class CLogin : public CDialog
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLogin();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	BOOL		m_bLoginCheck;
public:
	CBIniFile   m_iniLoginInfo;
	afx_msg void OnBnClickedButtonAccept();
	BOOL m_fnGetLoginCheck();
	void m_fnSetLoginCheck(BOOL bCheck);
	void m_fnCheckPassword(void);
	CString m_strGetPW;
	afx_msg void OnBnClickedButtonExit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool m_mod;	
};
