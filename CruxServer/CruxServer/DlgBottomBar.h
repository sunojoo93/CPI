#pragma once

#include "resource.h"
#include "Lib\GxButtonEx.h"			// ��ư OCX 
#include "Lib\GxStaticEx.h"			// Static OCX
#include "Login.h"

// CDlgBottomBar ��ȭ �����Դϴ�.

class CDlgBottomBar : public CDialogBar
{
	DECLARE_DYNAMIC(CDlgBottomBar)

private:
	UINT	m_nIDSelectMenuButton;
	OLE_COLOR m_dwNormalColor;							// ���õ��� ���� ��ư ��
	OLE_COLOR m_dwSelectColor;							// ���õ� ��ư ��
	BOOL	m_bMenuLock;									// �޴��� ���/�̻�� ó���Ѵ�.
	
	void ChangeMenuColor(UINT nID, BOOL bFlag);				// ���� ���õ� �׸� �µ��� ��ư�� ���� �����Ѵ�.


public:
	CDlgBottomBar(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgBottomBar();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_BOTTOMBAR };
	CGxButtonEx	m_btnAuto;
	CGxButtonEx	m_btnManual;
	CGxButtonEx	m_btnReport;
	CGxButtonEx	m_btnSetup;
	CGxButtonEx	m_btnIOView;

	// ȭ���� �����ϵ��� �Ѵ�.
	void ChangeScreen(UINT nID, UINT nScreen, BOOL bFlag); 
	void Init(void);										// �ʱ�ȭ�� �����Ѵ�.
	void MenuLock(BOOL bFlag);							// �޴��� ��׾ �������� �ʵ��� ����/���� �����ش�.
	CLogin* m_pDlgLogin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	DECLARE_EVENTSINK_MAP()
	void ClickGxbtnBottomMenu1();
	void ClickGxbtnBottomMenu2();
	void ClickGxbtnBottomMenu3();
	void ClickGxbtnBottomMenu4();
	void ClickGxbtnBottomMenu5();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
