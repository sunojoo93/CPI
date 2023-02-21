// DlgBottomBar.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "DlgBottomBar.h"

#include "..\CruxDefine.h"					// ������ �����
#include "..\MainFrm.h"

// GxOCX ����
#include "..\Lib\GxButtonEx.h"			// ��ư OCX 
#include "..\Lib\GxStaticEx.h"			// Static OCX



// CDlgBottomBar ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgBottomBar, CDialogBar)

CDlgBottomBar::CDlgBottomBar(CWnd* pParent /*=NULL*/) : CDialogBar()
	// : CDialog(CDlgBottomBar::IDD, pParent)
	
{

	m_dwNormalColor		= GXCOLOR_SILVER;									// ���õ��� ���� ��ư ��, ����
	m_dwSelectColor		= GXCOLOR_LIME;									// ���õ� ��ư ��, Lime

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


// CDlgBottomBar �޽��� ó�����Դϴ�.
BEGIN_EVENTSINK_MAP(CDlgBottomBar, CDialogBar)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU1, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu1, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU2, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu2, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU3, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu3, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU4, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu4, VTS_NONE)
	ON_EVENT(CDlgBottomBar, IDC_GXBTN_BOTTOM_MENU5, DISPID_CLICK, CDlgBottomBar::ClickGxbtnBottomMenu5, VTS_NONE)
END_EVENTSINK_MAP()

//
// ������ ȭ������ �����Ѵ�.
//
void CDlgBottomBar::ChangeScreen(UINT nID, UINT nScreen, BOOL bFlag) 
{
	// ȭ���� Ʋ�����ų� ������ �ٲٶ�� �����̸�,

	if ( m_bMenuLock && !bFlag) return;							// �޴��� ����ִٸ�, �ƹ��� ������ ���� �ʴ´�. 

	if ( (m_nIDSelectMenuButton != nID) || bFlag ) {


		ChangeMenuColor(m_nIDSelectMenuButton, FALSE);			// ���� ��ư�� ���� ���·� ����
		m_nIDSelectMenuButton = nID;							// ���� ���õ� ȭ���� ������ ������ ��ü�Ѵ�.
		ChangeMenuColor(m_nIDSelectMenuButton, TRUE);			// ��ư �� ����

		::SendMessage( ( (CMainFrame*)AfxGetMainWnd() )->m_hWnd, UM_MAINMENU_CHANGE, (WPARAM)nScreen, (LPARAM)bFlag);
	}
}


// ���� ���õ� �׸� �µ��� ��ư�� ���� �����Ѵ�.
void  CDlgBottomBar::ChangeMenuColor(UINT nID, BOOL bFlag)
{
	CGxButtonEx* pBtn = (CGxButtonEx *)GetDlgItem(nID);
	
	if ( !pBtn ) return;				// ������ ����

	if ( bFlag ) {
		pBtn->SetBackColor(  m_dwSelectColor  );			// ���� ��
	}
	else {
		pBtn->SetBackColor( m_dwNormalColor );				// ����
	}
}

// �ʱ�ȭ�� �����Ѵ�.
void CDlgBottomBar::Init(void)
{
	UpdateData(FALSE);
	ChangeMenuColor(IDC_GXBTN_BOTTOM_MENU1, TRUE);				// Auto �޴��� �����Ͽ����� ǥ��	
	m_nIDSelectMenuButton = IDC_GXBTN_BOTTOM_MENU1;	
	MenuLock( FALSE );											// �޴��� Ǯ���ش�.

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


// �޴��� ��׾ �������� �ʵ��� ����/���� �����ش�.
void CDlgBottomBar::MenuLock(BOOL bFlag)
{
	CGxStaticEx* pLabel = (CGxStaticEx*)GetDlgItem(IDC_GXLBL_BOTTOM_LOCK);

	m_bMenuLock = bFlag;									// �޴��� ���/�̻�� ó���Ѵ�.

	if (pLabel) 
	{
		if ( bFlag )										// �޴��� ��׸� ȭ�� ���� ��ܿ� ���� ����� �������� ���̰��Ѵ�.
			pLabel->ShowWindow(SW_SHOW); 
		else
			pLabel->ShowWindow(SW_HIDE); 
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////


// ȭ���� ��ȯ�ϴ� ��ư�� Ŭ���Ͽ���.
void CDlgBottomBar::ClickGxbtnBottomMenu1()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU1, eFORM_Auto, FALSE);					// Auto ȭ��
}


void CDlgBottomBar::ClickGxbtnBottomMenu2()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU2, eFORM_Manual, FALSE);					// Manual ȭ��
}


void CDlgBottomBar::ClickGxbtnBottomMenu3()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU3, eFORM_Report, FALSE);					// ��� (Report) ȭ��
}

void CDlgBottomBar::ClickGxbtnBottomMenu4()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU4, eFORM_Setup, FALSE);					// Setup ȭ��
}


void CDlgBottomBar::ClickGxbtnBottomMenu5()
{
	ChangeScreen(IDC_GXBTN_BOTTOM_MENU5, eFORM_IOView, FALSE);					// I/O View ȭ��
}


// Timer ����
void CDlgBottomBar::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == eTIMER_Startup )
	{
		KillTimer( nIDEvent );

		ChangeMenuColor(m_nIDSelectMenuButton, TRUE);							// �ʱ� ���õ� ��ư �� ����
	}

	CDialogBar::OnTimer(nIDEvent);
}
