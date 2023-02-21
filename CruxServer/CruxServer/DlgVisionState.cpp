
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgVisionState.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgVisionState.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CDlgVisionState 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgVisionState, CDialogEx)

CDlgVisionState::CDlgVisionState(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgVisionState::IDD, pParent)
{

}

CDlgVisionState::~CDlgVisionState()
{
}

void CDlgVisionState::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgVisionState, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CDlgVisionState 메시지 처리기입니다.

BOOL CDlgVisionState::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgVisionState::m_fnInitState()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V3))->EnableWindow(FALSE);
	//2015.05.12 add s-
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V4))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V5))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V6))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V7))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V8))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V9))->EnableWindow(FALSE);
	//2015.05.12 add e-

	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->EnableWindow(FALSE);
	//2015.05.12 add s-
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->EnableWindow(FALSE);
	//2015.05.12 add e-


	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(i == 0)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->EnableWindow(TRUE);
		}
		else if(i == 1)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V2))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->EnableWindow(TRUE);
		}
		else if(i == 2)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V3))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->EnableWindow(TRUE);
		}
		//else if(i == 3)	//2015.05.12 add s-
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V4))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->EnableWindow(TRUE);
		//}
		//else if(i == 4)
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V5))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->EnableWindow(TRUE);
		//}
		//else if(i == 5)
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V6))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->EnableWindow(TRUE);
		//}
		//else if(i == 6)
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V7))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->EnableWindow(TRUE);
		//}
		//else if(i == 7)
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V8))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->EnableWindow(TRUE);
		//}
		//else if(i == 8)
		//{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_V9))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->EnableWindow(TRUE);
		//}
	}

	//SetTimer(TIMER_UPDATE_STATE_VISION, TIMER_UPDATE_STATE_V_INTERVAL, NULL); //2020.10.13 comment by mby
}

void CDlgVisionState::m_fnUpdateState()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(i == 0)
		{
			if(pFrame->m_pVsSeverIF->m_nVisionState[0] == STATE_IDLE)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->SetCaption(_T("IDLE"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[0] == STATE_READY)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->SetCaption(_T("READY"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[0] == STATE_GRAB)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->SetCaption(_T("GRAB"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[0] == STATE_PROCESS)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->SetCaption(_T("PROCESS"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[0] == STATE_ERROR)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V1))->SetCaption(_T("ERROR"));
		}
		else if(i == 1)
		{
			if(pFrame->m_pVsSeverIF->m_nVisionState[1] == STATE_IDLE)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->SetCaption(_T("IDLE"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[1] == STATE_READY)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->SetCaption(_T("READY"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[1] == STATE_GRAB)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->SetCaption(_T("GRAB"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[1] == STATE_PROCESS)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->SetCaption(_T("PROCESS"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[1] == STATE_ERROR)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V2))->SetCaption(_T("ERROR"));
		}
		else if(i == 2)
		{
			if(pFrame->m_pVsSeverIF->m_nVisionState[2] == STATE_IDLE)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->SetCaption(_T("IDLE"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[2] == STATE_READY)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->SetCaption(_T("READY"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[2] == STATE_GRAB)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->SetCaption(_T("GRAB"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[2] == STATE_PROCESS)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->SetCaption(_T("PROCESS"));
			else if(pFrame->m_pVsSeverIF->m_nVisionState[2] == STATE_ERROR)
				((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V3))->SetCaption(_T("ERROR"));
		}
		//else if(i == 3)	//2015.05.12 add s-
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[3] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[3] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[3] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[3] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[3] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V4))->SetCaption(_T("ERROR"));
		//}
		//else if(i == 4)
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[4] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[4] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[4] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[4] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[4] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V5))->SetCaption(_T("ERROR"));
		//}
		//else if(i == 5)
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[5] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[5] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[5] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[5] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[5] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V6))->SetCaption(_T("ERROR"));
		//}
		//else if(i == 6)
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[6] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[6] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[6] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[6] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[6] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V7))->SetCaption(_T("ERROR"));
		//}
		//else if(i == 7)
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[7] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[7] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[7] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[7] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[7] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V8))->SetCaption(_T("ERROR"));
		//}
		//else if(i == 8)
		//{
		//	if(pFrame->m_pVsSeverIF->m_nVisionState[8] == STATE_IDLE)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->SetCaption(_T("IDLE"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[8] == STATE_READY)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->SetCaption(_T("READY"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[8] == STATE_GRAB)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->SetCaption(_T("GRAB"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[8] == STATE_PROCESS)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->SetCaption(_T("PROCESS"));
		//	else if(pFrame->m_pVsSeverIF->m_nVisionState[8] == STATE_ERROR)
		//		((CGxButtonEx*)GetDlgItem(IDC_GXBTN_STATE_V9))->SetCaption(_T("ERROR"));
		//}//2015.05.12 addd e-
	}
}

BEGIN_EVENTSINK_MAP(CDlgVisionState, CDialogEx)
	ON_EVENT(CDlgVisionState, IDC_GXBTN_RESET_ALARM, DISPID_CLICK, CDlgVisionState::ClickGxbtnResetAlarm, VTS_NONE)
END_EVENTSINK_MAP()



void CDlgVisionState::ClickGxbtnResetAlarm()
{	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	G_AddLog(3, 2, _T("Sequence Reset!!!"));

//	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	pFrame->m_bIsGrabStartRequest	= FALSE;	// 2015.07.02 ljk
	//for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)
	{
		pFrame->m_pThreadSeqPLC[i]->m_fnResetSEQ(TRUE, TRUE, TRUE);
		pFrame->m_bIsEngineerModeMsg[i] = FALSE;		// 2015.07.09 ljk
	}
	for(int i = 0; i < CIM_MAX; i++)
		pFrame->m_pThreadSeqCIM[i]->m_fnResetSEQ();
}

void CDlgVisionState::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_UPDATE_STATE_VISION)
	{
		m_fnUpdateState();
	}

	CDialogEx::OnTimer(nIDEvent);
}
