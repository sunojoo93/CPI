#pragma once

#include "resource.h"
#include "Lib\GxButtonEx.h"			// 버튼 OCX 
#include "Lib\GxStaticEx.h"			// Static OCX
#include "Login.h"

// CDlgBottomBar 대화 상자입니다.

class CDlgBottomBar : public CDialogBar
{
	DECLARE_DYNAMIC(CDlgBottomBar)

private:
	UINT	m_nIDSelectMenuButton;
	OLE_COLOR m_dwNormalColor;							// 선택되지 않은 버튼 색
	OLE_COLOR m_dwSelectColor;							// 선택된 버튼 색
	BOOL	m_bMenuLock;									// 메뉴를 사용/미사용 처리한다.
	
	void ChangeMenuColor(UINT nID, BOOL bFlag);				// 현재 선택된 항목에 맞도록 버튼의 색을 변경한다.


public:
	CDlgBottomBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgBottomBar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_BOTTOMBAR };
	CGxButtonEx	m_btnAuto;
	CGxButtonEx	m_btnManual;
	CGxButtonEx	m_btnReport;
	CGxButtonEx	m_btnSetup;
	CGxButtonEx	m_btnIOView;

	// 화면을 변경하도록 한다.
	void ChangeScreen(UINT nID, UINT nScreen, BOOL bFlag); 
	void Init(void);										// 초기화를 수행한다.
	void MenuLock(BOOL bFlag);							// 메뉴를 잠그어서 반응하지 않도록 설정/해제 시켜준다.
	CLogin* m_pDlgLogin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
