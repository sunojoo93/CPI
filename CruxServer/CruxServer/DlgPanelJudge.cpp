
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgDefectMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgPanelJudge.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"
#include "ViewAuto.h"

// CDlgDefectInfo 대화 상자입니다.


IMPLEMENT_DYNAMIC(CDlgPanelJudge, CDialogEx)

	CDlgPanelJudge::CDlgPanelJudge(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPanelJudge::IDD, pParent)
{
	m_cPanelID		= _T("-");	
	m_cGrade		= _T("-");	
	m_cCFResult		= _T("-");	
	m_cTftResult	= _T("-");
	m_cPenetResult	= _T("-");
//	SetResultPanelId(_T(""));


}

CDlgPanelJudge::~CDlgPanelJudge()
{
}

void CDlgPanelJudge::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RESETTIME, m_edtResetTime);
}


BEGIN_MESSAGE_MAP(CDlgPanelJudge, CDialogEx)
	ON_WM_PAINT()
	 ON_MESSAGE(WM_UI_CONTROL_JUDGE_DIS, &CDlgPanelJudge::m_fnInitState)
	 ON_MESSAGE(WM_UI_CONTROL_JUDGE_INFO_DIS, &CDlgPanelJudge::m_fnUpdateJudgeInfo)
	 
END_MESSAGE_MAP()


// CDlgDefectMap 메시지 처리기입니다.


void CDlgPanelJudge::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}
BEGIN_EVENTSINK_MAP(CDlgPanelJudge, CDialogEx)
	ON_EVENT(CDlgPanelJudge, IDC_GXBTN_RESET_ALARM2, DISPID_CLICK, CDlgPanelJudge::ClickGxbtnResetAlarm, VTS_NONE)
	
END_EVENTSINK_MAP()

LRESULT CDlgPanelJudge::m_fnInitState(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS1))->SetCaption(_T("CF"));
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS2))->SetCaption(_T("TFT"));
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS3))->SetCaption(_T("RESET TIME"));//(_T("透过"));
	
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS3))->EnableWindow(FALSE);

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE3))->EnableWindow(FALSE);
	
	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(i == 0)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE1))->SetCaption(_T("OK"));
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE1))->EnableWindow(TRUE);
			SetResultCfJudge(_T("OK"));
		}
		else if(i == 1)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS2))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE2))->SetCaption(_T("OK"));
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE2))->EnableWindow(TRUE);
			SetResultTftJudge(_T("OK"));
		}
		else if(i == 2)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_VS3))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE3))->SetCaption(_T("OK"));
			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE3))->EnableWindow(TRUE);
			SetResultPenetJudge(_T("OK"));
		}
	}	

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetBackColor(GXCOLOR_LIME);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetTextColor(DEFAULT_GRAPH_AXIS_COLOR);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetCaption(_T("OK"));
	SetResultGrade(_T("OK"));
	return 0;
}

void CDlgPanelJudge::ClickGxbtnResetAlarm()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	
	if(pView->m_pDlgLogin)
	{
		delete pView->m_pDlgLogin;
		pView->m_pDlgLogin = NULL;
	}
	
	pView->m_pDlgLogin = new CLogin();

	if (pView->m_pDlgLogin->DoModal() == IDOK)
	{
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, FALSE); //16.05.03 sgkim
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, FALSE);

		G_AddLog(3, 2, _T("Server Sequence Reset!!!"));

		//2016.12.28 Monitor -> Merge 본 
		for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			pFrame->m_pVsSeverIF->m_bRecvRecipeRes[i - 1] = FALSE;

			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[i - 1])
			{
				pFrame->m_pVsSeverIF->m_fnVisionInitial(i);
			}				
		}

		for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			pFrame->m_pThreadSeqPLC[i]->m_fnResetSEQ(TRUE, TRUE, TRUE);		
			pFrame->m_bIsEngineerModeMsg[i] = FALSE;		// 2015.07.09 ljk
		}
		if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
		{
			for(int i = 0; i < CIM_MAX; i++)
				pFrame->m_pThreadSeqCIM[i]->m_fnResetSEQ();
		}
	}
}

LRESULT CDlgPanelJudge::m_fnUpdateJudgeInfo(WPARAM wParam, LPARAM lParam)
//void CDlgPanelJudge::m_fnUpdateJudgeInfo(CString cDefectVision, CString CPanelId)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPanelID, cCountRoot = _T("") , cCountVal = _T("");
	int nDefectVision = 0;
	BOOL bTotalJudge = FALSE, bJudgeVal = FALSE;
	int nIdcNo = -1, nGradeCnt;
	
	nDefectVision = wParam;
	strPanelID = (LPCTSTR)lParam;

	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_PANELID))->SetCaption(strPanelID);
	SetResultPanelId(strPanelID);
	
	bTotalJudge = FALSE;	

	if(nDefectVision == 0)
	{		
		nIdcNo = IDC_GXLBL_JUDGE1;		
		SetResultCfJudge(_T("NG"));
	}
	if(nDefectVision == 1)
	{
		nIdcNo = IDC_GXLBL_JUDGE2;	
		SetResultTftJudge(_T("NG"));
	}
	if(nDefectVision == 2)
	{
		nIdcNo = IDC_GXLBL_JUDGE3;	
		SetResultPenetJudge(_T("NG"));
	}
	/*if(_ttoi(cDefectVision) == -1)
	{
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE1))->SetCaption(_T("GOOD"));
	SetResultCfJudge(_T("GOOD"));
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE2))->SetCaption(_T("GOOD"));
	SetResultTftJudge(_T("GOOD"));
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_JUDGE3))->SetCaption(_T("GOOD"));
	SetResultPenetJudge(_T("GOOD"));
	}*/
	
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);

	if (nIdcNo != -1)
	{
		((CGxStaticEx*)GetDlgItem(nIdcNo))->SetCaption(_T("NG"));
		bTotalJudge = TRUE;	
	}
		
	if (bTotalJudge)
	{
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetBackColor(GXCOLOR_RED);
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetTextColor(REPORT_NORMAL_CELL_BK_COLOR);
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetCaption(_T("NG"));
		SetResultGrade(_T("NG"));
		//nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
		//cCountVal.Format(_T("%d"), ++nGradeCnt);
		//WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, cCountVal, cCountRoot);
	}
	else
	{
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetBackColor(GXCOLOR_LIME);
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetTextColor(DEFAULT_GRAPH_AXIS_COLOR);
		((CGxStaticEx*)GetDlgItem(IDC_GXLBL_STATE1))->SetCaption(_T("OK"));
		SetResultGrade(_T("OK"));
		//nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
		//cCountVal.Format(_T("%d"), ++nGradeCnt);
		//WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, cCountVal, cCountRoot);
	}
	return 0;
}

void  CDlgPanelJudge::SetResultPanelId(CString strValue)
{
	m_cPanelID = strValue;
}
void  CDlgPanelJudge::SetResultGrade(CString strValue)	
{
	m_cGrade = strValue;
}
void  CDlgPanelJudge::SetResultCfJudge(CString strValue)	
{
	m_cCFResult = strValue;
}
void  CDlgPanelJudge::SetResultTftJudge(CString strValue)	
{
	m_cTftResult = strValue;
}
void  CDlgPanelJudge::SetResultPenetJudge(CString strValue)
{
	m_cPenetResult = strValue;
}

void  CDlgPanelJudge::SetResultDefectCodeJudge(CString strValue)
{
	if( strValue == _T("") )
		strValue =  _T("         -");
	m_cDefCode = strValue;
}

void  CDlgPanelJudge::SetResultDefectCodeName(CString strValue)
{
	m_cDefCodeName = strValue;
}

CString CDlgPanelJudge::GetResultPanelId()	
{
	return m_cPanelID ;
}
CString CDlgPanelJudge::GetResultGrade()	
{
	return m_cGrade ;
}
CString CDlgPanelJudge::GetResultCFJudge()		
{
	return m_cCFResult ;
}
CString CDlgPanelJudge::GetResultTftJudge()	
{
	return m_cTftResult ;
}
CString CDlgPanelJudge::GetResultPenetJudge()	
{
	return m_cPenetResult ;
}

//2015.09.11 Mr.Chu
CString CDlgPanelJudge::GetResultDefCode()
{
	return m_cDefCode;
}
CString CDlgPanelJudge::GetResultDefCodeName()
{
	return m_cDefCodeName;
}

void CDlgPanelJudge::ChangeResetTime()
{
	CString strTime;
	CTime time = CTime::GetCurrentTime(&time);
	strTime = time.Format(_T("Reset Time : %Y:%m:%d_%H:%M:%S"));
	GetDlgItem(IDC_EDIT_RESETTIME)->SetWindowText(strTime);
	G_AddLog_RESET(strTime);
}
