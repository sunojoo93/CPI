
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgStageState.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgStageState.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CDlgStageState 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgStageState, CDialogEx)

CDlgStageState::CDlgStageState(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgStageState::IDD, pParent)
{

}

CDlgStageState::~CDlgStageState()
{
}

void CDlgStageState::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStageState, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgStageState 메시지 처리기입니다.


BOOL CDlgStageState::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgStageState::m_fnInitState()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S3))->EnableWindow(FALSE);

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P2))->EnableWindow(FALSE);

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE3))->EnableWindow(FALSE);
	//2015.05.19 add s-
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE4))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE5))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE6))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE7))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE8))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE9))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE10))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE11))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE12))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE13))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE14))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE15))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE16))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE17))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE18))->EnableWindow(FALSE);
	//2015.05.19 add e-

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH3))->EnableWindow(FALSE);
	//2015.05.19 add s-
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH4))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH5))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH6))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH7))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH8))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH9))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH10))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH11))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH12))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH13))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH14))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH15))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH16))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH17))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH18))->EnableWindow(FALSE);
	//2015.05.19 add e-

	//for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)	//2015.05.15 edit
	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)
	{
		if(i == 0)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P2))->EnableWindow(TRUE);

			//Cheng Du
			for(int nVCnt = 0; nVCnt < pFrame->m_stStageInfo[i].nVisionCnt; nVCnt++) 
			{
				if(nVCnt < 6){
					//PC1~6 State Display
					((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->EnableWindow(TRUE);	//Status Stage1 -> 3101
					((CGxStaticEx*)GetDlgItem(4101 + nVCnt))->EnableWindow(TRUE);	//Light  Stage1 -> 4101
				}else if(nVCnt == 6){
					//PC7 State Display
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S2))->EnableWindow(TRUE);
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S2))->SetCaption(_T("INSPECTION STAGE #1"));
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P1))->EnableWindow(TRUE);
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P2))->EnableWindow(TRUE);

					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE7))->EnableWindow(TRUE);
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH7))->EnableWindow(TRUE);
				}else if(nVCnt == 7){
					//PC8 State Display
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE8))->EnableWindow(TRUE);
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH8))->EnableWindow(TRUE);
				}else if(nVCnt == 8){
					//PC9 State Display
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE9))->EnableWindow(TRUE);
					((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH9))->EnableWindow(TRUE);
				}
			}
		}
		else if(i == 1)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S2))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P2))->EnableWindow(TRUE);

			for(int nVCnt = 0; nVCnt < pFrame->m_stStageInfo[i].nVisionCnt; nVCnt++) 
			{
				((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->EnableWindow(TRUE);	//Status Stage2 -> 3201
				((CGxStaticEx*)GetDlgItem(4201 + nVCnt))->EnableWindow(TRUE);	//Light  Stage2 -> 4201
			}
		}
		else if(i == 2)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_S3))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P2))->EnableWindow(TRUE);

			for(int nVCnt = 0; nVCnt < pFrame->m_stStageInfo[i].nVisionCnt; nVCnt++) 
			{
				((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->EnableWindow(TRUE);	//Status Stage3 -> 3301
				((CGxStaticEx*)GetDlgItem(4301 + nVCnt))->EnableWindow(TRUE);	//Light  Stage3 -> 4301
			}
		}
	}	

	
	//SetTimer(TIMER_UPDATE_STATE_STAGE, TIMER_UPDATE_STATE_S_INTERVAL, NULL);	//2020.10.13 comment by mby
}

void CDlgStageState::m_fnUpdateState()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPanelID1, strPanelID2;

	//for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)	//2015.05.15 edit
	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)
	{
		strPanelID1 = pFrame->m_fnGetPanelID_1(i);
		strPanelID2 = pFrame->m_fnGetPanelID_2(i);

		if(strPanelID1 == _T(""))	strPanelID1 = _T("NONE");
		if(strPanelID2 == _T(""))	strPanelID2 = _T("NONE");

		if(i == 0)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P1))->SetCaption(strPanelID1);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG1_P2))->SetCaption(strPanelID2);

			//Cheng Du ToDo...
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P1))->SetCaption(strPanelID1);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P2))->SetCaption(strPanelID2);
		}
		else if(i == 1)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P1))->SetCaption(strPanelID1);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG2_P2))->SetCaption(strPanelID2);
		}
		else if(i == 2)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P1))->SetCaption(strPanelID1);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_STG3_P2))->SetCaption(strPanelID2);
		}

	}

	//2015.05.19 add s-
	int nCnt = 0;

	for(int nSCnt = 0; nSCnt < pFrame->m_stParamInfo.nCountStage; nSCnt++)
	{
		for(int nVCnt = 0; nVCnt < pFrame->m_stStageInfo[nSCnt].nVisionCnt; nVCnt++)
		{
			if(nSCnt == 0)
			{
				if(nVCnt < 6){
					//Status Stage1 -> 3101
					if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_IDLE)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("IDLE"));			
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_READY)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("READY"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_GRAB)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("GRAB"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_PROCESS)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("PROCESS"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_ERROR)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("ERROR"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_START)		//2015.07.05
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("INSP..."));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_END)
						((CGxStaticEx*)GetDlgItem(3101 + nVCnt))->SetCaption(_T("INSP END"));
				}else{
					if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_IDLE)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("IDLE"));			
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_READY)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("READY"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_GRAB)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("GRAB"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_PROCESS)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("PROCESS"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_ERROR)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("ERROR"));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_START)		//2015.07.05
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("INSP..."));
					else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_END)
						((CGxStaticEx*)GetDlgItem(3195 + nVCnt))->SetCaption(_T("INSP END"));
				}
			}
			else if(nSCnt == 1)
			{
				//Status Stage2 -> 3201
				if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_IDLE)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("IDLE"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_READY)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("READY"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_GRAB)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("GRAB"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_PROCESS)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("PROCESS"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_ERROR)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("ERROR"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_START)		//2015.07.05
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("INSP..."));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_END)
					((CGxStaticEx*)GetDlgItem(3201 + nVCnt))->SetCaption(_T("INSP END"));
			}
			else if(nSCnt == 2)
			{
				//Status Stage3 -> 3301
				if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_IDLE)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("IDLE"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_READY)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("READY"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_GRAB)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("GRAB"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_PROCESS)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("PROCESS"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_ERROR)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("ERROR"));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_START)		//2015.07.05
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("INSP..."));
				else if(pFrame->m_pVsSeverIF->m_nVisionState[nCnt] == STATE_INSP_END)
					((CGxStaticEx*)GetDlgItem(3301 + nVCnt))->SetCaption(_T("INSP END"));
			}
			nCnt++;
		}
	}
	//2015.05.19 add e-

}

// 함수 설명	: 보고된 휘도 값을 화면에 표시, 알람 발생
// Return		: 0 - OK, 그외 에러
int CDlgStageState::m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue)
{
	int nRet = 0;
	CString cCaption = _T("");

	cCaption.Format(_T("%d"), nValue);
	//if (nVisionID == 1)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH1))->SetCaption(cCaption);
	//}
	//if (nVisionID == 2)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH2))->SetCaption(cCaption);
	//}
	//if (nVisionID == 3)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH3))->SetCaption(cCaption);
	//}
	////2015.05.19 add s-
	//if (nVisionID == 4)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH4))->SetCaption(cCaption);
	//}
	//if (nVisionID == 5)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH5))->SetCaption(cCaption);
	//}
	//if (nVisionID == 6)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH6))->SetCaption(cCaption);
	//}
	//if (nVisionID == 7)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH7))->SetCaption(cCaption);
	//}
	//if (nVisionID == 8)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH8))->SetCaption(cCaption);
	//}
	//if (nVisionID == 9)
	//{
	//	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE_LIGTH9))->SetCaption(cCaption);
	//}
	////2015.05.19 add e-
	//----------------------------
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for(int nSCnt = 0; nSCnt < pFrame->m_stParamInfo.nCountStage; nSCnt++)
	{
		for(int nVCnt = 0; nVCnt < pFrame->m_stStageInfo[nSCnt].nVisionCnt; nVCnt++)
		{
			if(nVisionID == pFrame->m_stStageInfo[nSCnt].nVisionNo[nVCnt])
			{
				((CGxStaticEx*)GetDlgItem(4000 + ((nSCnt + 1) * 100) + nVCnt + 1))->SetCaption(cCaption);
			}
		}
	}
	//----------------------------

	return nRet;
}

void CDlgStageState::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_UPDATE_STATE_STAGE)
	{
		m_fnUpdateState();
	}

	CDialogEx::OnTimer(nIDEvent);
}
void CDlgStageState::SetChangePACodeMsg(CString str)
{
	GetDlgItem(IDC_STAT_PACODE_MSG)->SetWindowText(str);
}