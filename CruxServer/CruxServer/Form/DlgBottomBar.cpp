// DlgBottomBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgBottomBar.h"

#include "..\CruxDefine.h"					// 공통사용 선언부
#include "..\MainFrm.h"

// GxOCX 참조
#include "..\Lib\GxButtonEx.h"			// 버튼 OCX 
#include "..\Lib\GxStaticEx.h"			// Static OCX



// CDlgBottomBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgBottomBar, CDialogBar)

CDlgBottomBar::CDlgBottomBar(CWnd* pParent /*=NULL*/) : CDialogBar()
	// : CDialog(CDlgBottomBar::IDD, pParent)
	
{

	m_dwNormalColor		= GXCOLOR_SILVER;									// 선택되지 않은 버튼 색, 은색
	m_dwSelectColor		= GXCOLOR_LIME;									// 선택된 버튼 색, Lime

	//SetTimer(eTIMER_Startup, 1000, NULL);
}

CDlgBottomBar::~CDlgBottomBar()
{
}

void CDlgBottomBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GXBTN_BOTTOM_MENU1, m_btnAuto);
	DDX_Control(pDX, IDC_GXBTN_BOTTOM_MENU2, m_btnManual);
	DDX_Control(pDX, IDC_GXBTN_BOTTOM_MENU3, m_btnReport);
	DDX_Control(pDX, IDC_GXBTN_BOTTOM_MENU4, m_btnSetup);
	DDX_Control(pDX, IDC_GXBTN_BOTTOM_MENU5, m_btnIOView);
}


BEGIN_MESSAGE_MAP(CDlgBottomBar, CDialogBar)
	ON_WM_TIMER()

END_MESSAGE_MAP()


// CDlgBottomBar 메시지 처리기입니다.
BEGIN_EVENTSINK_MAP(CDlgBottomBar, CDialogBar)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU1, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu1, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU2, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu2, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU3, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu3, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU4, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu4, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU5, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu5, VTS_NONE)
END_EVENTSINK_MAP()

//
// 지정한 화면으로 변경한다.
//
void CDlgBottomBar::ChangeScreen(UINT nID, UINT nScreen, BOOL bFlag) 
{
	// 화면이 틀려졌거나 강제로 바꾸라는 설정이면,

	if ( m_bMenuLock && !bFlag) return;							// 메뉴가 잠겨있다면, 아무런 동작을 하지 않는다. 

	if ( (m_nIDSelectMenuButton != nID) || bFlag ) {


		ChangeMenuColor(m_nIDSelectMenuButton, FALSE);			// 이전 버튼은 보통 상태로 변경
		m_nIDSelectMenuButton = nID;							// 현재 선택된 화면은 지정한 값으로 대체한다.
		ChangeMenuColor(m_nIDSelectMenuButton, TRUE);			// 버튼 색 반전

		::SendMessage( ( (CMainFrame*)AfxGetMainWnd() )->m_hWnd, UM_MAINMENU_CHANGE, (WPARAM)nScreen, (LPARAM)bFlag);
	}
}


// 현재 선택된 항목에 맞도록 버튼의 색을 변경한다.
void  CDlgBottomBar::ChangeMenuColor(UINT nID, BOOL bFlag)
{
	CGxButtonEx* pBtn = (CGxButtonEx *)GetDlgItem(nID);
	
	if ( !pBtn ) return;				// 포인터 검증

	if ( bFlag ) {
		pBtn->SetBackColor(  m_dwSelectColor  );			// 보통 색
	}
	else {
		pBtn->SetBackColor( m_dwNormalColor );				// 반전
	}
}

// 초기화를 수행한다.
void CDlgBottomBar::Init(void)
{
	UpdateData(FALSE);
	ChangeMenuColor(IDC_GXBTN_BOTTOM_MENU1, TRUE);				// Auto 메뉴를 선택하였음을 표시	
	m_nIDSelectMenuButton = IDC_GXBTN_BOTTOM_MENU1;	
	MenuLock( FALSE );											// 메뉴를 풀어준다.

	m_btnAuto.SetCaption("Auto");
	m_btnAuto.SetFontSize(20);
	m_btnManual.SetCaption("Manual");
	m_btnManual.SetFontSize(20);
	m_btnReport.SetCaption("Report");
	m_btnReport.SetFontSize(20);
	m_btnSetup.SetCaption("Setup");
	m_btnSetup.SetFontSize(20);
	m_btnIOView.SetCaption("I/O View");
	m_btnIOView.SetFontSize(20);
}


// 메뉴를 잠그어서 반응하지 않도록 설정/해제 시켜준다.
void CDlgBottomBar::MenuLock(BOOL bFlag)
{
	CGxStaticEx* pLabel = (CGxStaticEx*)GetDlgItem(IDC_GXLBL_BOTTOM_LOCK);

	m_bMenuLock = bFlag;									// 메뉴를 사용/미사용 처리한다.

	if (pLabel) 
	{
		if ( bFlag )										// 메뉴를 잠그면 화면 좌측 상단에 열쇠 모양의 아이콘을 보이게한다.
			pLabel->ShowWindow(SW_SHOW); 
		else
			pLabel->ShowWindow(SW_HIDE); 
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


// 화면을 전환하는 버튼을 클릭하였다.
void CDlgBottomBar::ClickGxbtnBottomMenu1()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU1, eFORM_Auto, FALSE);					// Auto 화면
}


void CDlgBottomBar::ClickGxbtnBottomMenu2()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU2, eFORM_Manual, FALSE);					// Manual 화면
}


void CDlgBottomBar::ClickGxbtnBottomMenu3()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU3, eFORM_Report, FALSE);					// 통계 (Report) 화면
}

void CDlgBottomBar::ClickGxbtnBottomMenu4()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU4, eFORM_Setup, FALSE);					// Setup 화면
}


void CDlgBottomBar::ClickGxbtnBottomMenu5()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU5, eFORM_IOView, FALSE);					// I/O View 화면
}


// Timer 구동
void CDlgBottomBar::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == eTIMER_Startup )
	{
		KillTimer( nIDEvent );

		ChangeMenuColor(m_nIDSelectMenuButton, TRUE);							// 초기 선택된 버튼 색 반전
	}

	CDialogBar::OnTimer(nIDEvent);
}
