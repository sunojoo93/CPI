
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgTopBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DlgTopBar.h"

// Crux Project
#include "CruxDefine.h"					// 공통 선언

// Data Class
#include "CruxData.h"				// 각종 정보를 담고있는 Data Class


// CDlgTopBar 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgTopBar, CDialogBar)

CDlgTopBar::CDlgTopBar(CWnd* pParent /*=NULL*/) : CDialogBar()
	// : CDialog(CDlgTopBar::IDD, pParent)
{

}

CDlgTopBar::~CDlgTopBar()
{
}

void CDlgTopBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgTopBar, CDialogBar)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_CLOSE()

	
END_MESSAGE_MAP()


// CDlgTopBar 메시지 처리기입니다.

// 일정 시간마다 이벤트 발생
void CDlgTopBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == eTIMER_Clock)			// 시계 표시 Timer 인가 ?
	{
		m_timeClock = COleDateTime::GetCurrentTime();		//  현재 시각
		// m_sDateTime = m_timeClock.Format(_T("%Y.%m.%d\r\n%H:%M:%S"));
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOP_CLOCK))->SetCaption( m_timeClock.Format(_T("%Y.%m.%d\r\n%H:%M:%S")) );		
	}
	else if ( nIDEvent == eTIMER_Startup )
	{
		KillTimer( eTIMER_Startup );
		SetTimer(eTIMER_Clock, 1000, NULL);					// 시계표시
	}

	CDialogBar::OnTimer(nIDEvent);
}

int CDlgTopBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetTimer(eTIMER_Startup, 500, NULL);				// 시작처리 타이머 호출

	return 0;
}

void CDlgTopBar::OnClose()
{
	KillTimer( eTIMER_Clock );							// 시계표시 타이머 정지

	CDialogBar::OnClose();
}

void CDlgTopBar::m_fnSetProcState(CString strProcState)
{
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_PROC_STATE))->SetCaption(strProcState);
}

void CDlgTopBar::m_fnSetPPID(CString strPPID)
{
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_PPID))->SetCaption(strPPID);
}

void CDlgTopBar::m_fnSetPanelID(CString strPanelID, int nIndex)
{
	if(nIndex == 1)			// Panel ID #1
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_PANELID_1))->SetCaption(strPanelID);
	else if(nIndex == 2)	// Panel ID #2
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_PANELID_2))->SetCaption(strPanelID);
}

void CDlgTopBar::m_fnSetState(int nIndex, BOOL bEnable)
{
	if(nIndex == 1)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_1))->SetIconIndex(bEnable);
	else if(nIndex == 2)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_2))->SetIconIndex(bEnable);
	else if(nIndex == 3)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_3))->SetIconIndex(bEnable);
	else if(nIndex == 4)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_4))->SetIconIndex(bEnable);
	else if(nIndex == 5)	//2015.05.12 add s-
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_5))->SetIconIndex(bEnable);
	else if(nIndex == 6)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_6))->SetIconIndex(bEnable);
	else if(nIndex == 7)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_7))->SetIconIndex(bEnable);
	else if(nIndex == 8)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_8))->SetIconIndex(bEnable);
	else if(nIndex == 9)	//2015.05.12 add e-
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_9))->SetIconIndex(bEnable);
	else if(nIndex == 10)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_PLC))->SetIconIndex(bEnable);
	else if(nIndex == 11)
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_CIM))->SetIconIndex(bEnable);
	else if(nIndex == 12)	//2015.11.20
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_QMS))->SetIconIndex(bEnable);
	else if(nIndex == 30)	//2015.12.18 Mr.Chu
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TOPBAR_STATE_ALIGN))->SetIconIndex(bEnable);
}
