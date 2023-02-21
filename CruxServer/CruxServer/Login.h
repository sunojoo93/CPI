#pragma once

#include "Util\BIniFile.h"			// INI 파일 엑세스

// CLogin 대화 상자입니다.

class CLogin : public CDialog
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogin();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
