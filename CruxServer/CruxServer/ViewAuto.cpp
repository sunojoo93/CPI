// ViewAuto.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ViewAuto.h"
#include "MainFrm.h"
#include <atlstr.h>
#include "ViewSetup.h"
#include "F_GRDAE_Dialog.h"
#include "GRADE_SET_Dialog.h"

// CViewAuto
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

extern CInterServerInterface* G_pVsServerIF;

IMPLEMENT_DYNCREATE(CViewAuto, CFormView)


	///////////////// 2015.01.12 ljk //////////////////
	typedef enum enumDefectFile
{
	DEFECT_FILE_COL_NUM,
	DEFECT_FILE_COL_FILE_NAME,
	MAX_DEFECT_FILE_COL
};

typedef enum enumDefectSummary
{
	DEFECT_SUMMARY_COL_NUM,			// index	
	DEFECT_SUMMARY_LAYER_NO,
	DEFECT_SUMMARY_DEFECT_NO ,
	DEFECT_SUMMARY_DEFECT_CODE,	
	DEFECT_SUMMARY_DEFECT_FLAG, 
	DEFECT_SUMMARY_DEFECT_PIXEL_X,		
	DEFECT_SUMMARY_DEFECT_PIXEL_Y, 
	DEFECT_SUMMARY_DEFECT_GATE_START_NO,	
	DEFECT_SUMMARY_DEFECT_DATA_START_NO,
	DEFECT_SUMMARY_DEFECT_GATE_END_NO, 
	DEFECT_SUMMARY_DEFECT_DATA_END_NO,	
	DEFECT_SUMMARY_COL_COORD_START_X,		// xpos
	DEFECT_SUMMARY_COL_COORD_START_Y, 		// ypos
	DEFECT_SUMMARY_DEFECT_COORD_END_X, 	
	DEFECT_SUMMARY_DEFECT_COORD_END_Y, 
	DEFECT_SUMMARY_DEFECT_DEF_SIZE, 
	DEFECT_SUMMARY_DEFECT_DEF_IMG_NO,  
	DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, 
	DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y,  
	DEFECT_SUMMARY_DEFECT_DEF_TYPE,  
	DEFECT_SUMMARY_DEFECT_CAM_NO, 
	//DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH,
	MAX_DEFECT_SUMMARY_COL
};

//2016.02.01 Packing Defect
typedef enum enumPackingDefectSummary
{
	P_DEFECT_SUMMARY_COL_NUM,	
	P_DEFECT_SUMMARY_TIME,
	P_DEFECT_SUMMARY_DEVICE,
	P_DEFECT_SUMMARY_CELL_ID,	
	P_DEFECT_SUMMARY_PAD, 
	P_DEFECT_SUMMARY_JUDGE,		 
	P_DEFECT_SUMMARY_COORD_START_X,
	P_DEFECT_SUMMARY_COORD_START_Y,
	P_DEFECT_SUMMARY_COORD_END_X, 	
	P_DEFECT_SUMMARY_COORD_END_Y,
	P_DEFECT_SUMMARY_DEFECT_TYPE,
	P_DEFECT_SUMMARY_AREA,
	P_DEFECT_SUMMARY_BOX_AREA,
	P_DEFECT_SUMMARY_BOX_X_MAX, 
	P_DEFECT_SUMMARY_BOX_X_MIN,	
	P_DEFECT_SUMMARY_BOX_Y_MAX,
	P_DEFECT_SUMMARY_BOX_Y_MIN,
	P_DEFECT_SUMMARY_CENTER_X, 	
	P_DEFECT_SUMMARY_CENTER_Y, 
	P_DEFECT_SUMMARY_LENGTH, 
	P_DEFECT_SUMMARY_BREADTH,  
	P_DEFECT_SUMMARY_ELONGATION, 
	P_DEFECT_SUMMARY_COMPACTNESS,  
	P_DEFECT_SUMMARY_HOLECOUNT,  
	P_DEFECT_SUMMARY_FERET_MIN_ANGLE, 
	P_DEFECT_SUMMARY_FERET_MAX_ANGLE,
	P_DEFECT_SUMMARY_FERET_MIN,
	P_DEFECT_SUMMARY_FERET_MAX,
	P_DEFECT_SUMMARY_FERET_MEAN,
	P_DEFECT_SUMMARY_FERET_X,
	P_DEFECT_SUMMARY_FERET_Y,
	P_DEFECT_SUMMARY_FERET_ELONGATION,
	P_DEFECT_SUMMARY_PIXEL_MAX,
	P_DEFECT_SUMMARY_PIXEL_MEAN,
	P_DEFECT_SUMMARY_PIXEL_MIN,
	P_DEFECT_SUMMARY_PIXEL_SIGMA,
	P_DEFECT_SUMMARY_PIXEL_SUM,
	MAX_P_DEFECT_SUMMARY_COL
};

typedef enum enumPanelSummary
{
	PANEL_SUMMARY_TIME,			// index	
	PANEL_SUMMARY_PNLID,
	PANEL_SUMMARY_GRADE ,
	PANEL_SUMMARY_DEFECT,
	PANEL_SUMMARY_DEFECT_NAME,
	PANEL_SUMMARY_CF, 
	PANEL_SUMMARY_EQ,
	PANEL_SUMMARY_TFT,
	PANEL_SUMMARY_STAGE,
	PANEL_SUMMARY_PENETRATE,
	PANEL_SUMMARY_MEA_FLAG,
	MAX_PANEL_SUMMARY_COL
};
///////////////////////////////////////////////////

CViewAuto::CViewAuto()
	: CFormView(CViewAuto::IDD)
{
	for(int i=0;i<9;i++)
		m_bLog_Sel[i] = FALSE;
	m_pDlgMap = NULL;
	m_pDlgDefInfo = NULL;
	m_pDlgVisionState = NULL;
	m_pDlgStageState = NULL;
	m_pDlgLuminanceState = NULL;
	m_pDlgPanelJudge = NULL;
	m_pLoadingDlg = NULL;
	nTestCnt = 0;
	nTestPnlCnt = 0;
	m_IdxPanelBtn = 1;
	m_hThreadDetectFileSelect = NULL;
	m_pDlgPopUpImage = NULL;
	m_pDlgMessage = NULL;
	m_pDlgMeasure = NULL;
	m_pDlgErrorMsg = NULL;
	m_pDlgDefTypeMsg = NULL;
	m_pDlgDspMsg = NULL; //2016.01.29 Mr.Chu
	m_pDlgTemperature = NULL;	//2016.04.04
	m_pDlgPanelCodeTest = NULL;	//2016.09.14 Mr.Chu
	m_pDlgManualJudge = NULL;	//2018.05.14
	m_pDlgGradeSet = NULL;	// 2018.12.03 RYU
	m_pDlgFgradeSet= NULL;	// 2019.01.28 RYU

	for (int i =0 ; i < 3; i ++)
	{
		m_bResetFlag[i]   = FALSE;
	}
}

CViewAuto::~CViewAuto()
{
	//2016.09.14 Mr.Chu
	if(m_pDlgPanelCodeTest)
	{
		delete m_pDlgPanelCodeTest;
		m_pDlgPanelCodeTest = NULL;
	}

	if(m_pDlgMap)
	{
		delete m_pDlgMap;
		m_pDlgMap = NULL;
	}
	if(m_pDlgDefInfo)
	{
		delete m_pDlgDefInfo;
		m_pDlgDefInfo = NULL;
	}
	if (m_pDlgPanelJudge)
	{
		delete m_pDlgPanelJudge;
		m_pDlgPanelJudge = NULL;
	}
	if(m_pDlgVisionState)
	{
		delete m_pDlgVisionState;
		m_pDlgVisionState = NULL;
	}

	if(m_pDlgStageState)
	{
		delete m_pDlgStageState;
		m_pDlgStageState = NULL;
	}

	if(m_pDlgLuminanceState)
	{
		delete m_pDlgLuminanceState;
		m_pDlgLuminanceState = NULL;
	}

	if(m_pDlgPopUpImage)
	{
		m_pDlgPopUpImage->OnDestroy();
		delete m_pDlgPopUpImage;
		m_pDlgPopUpImage = NULL;
	}
		

	if (m_GridProcLog)
	{
		//delete m_GridProcLog;
		m_GridProcLog = NULL;
	}
	if (m_GridDefectSummary)
	{
		delete m_GridDefectSummary;
		m_GridDefectSummary = NULL;
	}
	if (m_GridPanelSummary)
	{
		delete m_GridPanelSummary;
		m_GridPanelSummary = NULL;
	}
	if (m_GridReviewDefectSummary)
	{
		delete m_GridReviewDefectSummary;
		m_GridReviewDefectSummary = NULL;
	}

	//2016.04.04
	if(m_pDlgTemperature)
	{
		delete m_pDlgTemperature;
		m_pDlgTemperature = NULL;
	}

	//2018.05.14
	if(m_pDlgManualJudge)
	{
		m_pDlgManualJudge->OnDestroy();
		delete m_pDlgManualJudge;
		m_pDlgManualJudge = NULL;
	}
	if(m_pDlgGradeSet)	// 2018.12.03 ryu
	{
		delete m_pDlgGradeSet;
		m_pDlgGradeSet = NULL;
	}
	
	DeleteLoadingDlg();
	CloseLoadingDlg();
}

void CViewAuto::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST_LOG, m_ctrlLog);
	//DDX_Check(pDX, IDC_CHK_LOG_S1, m_bLog_S1);
	//DDX_Check(pDX, IDC_CHK_LOG_S2, m_bLog_S2);
	//DDX_Check(pDX, IDC_CHK_LOG_S3, m_bLog_S3);

	DDX_Radio(pDX, IDC_RADIO1, m_bDisChk);
	//	DDX_Radio(pDX, IDC_RADIO2, m_bDisChk2);
	DDX_Control(pDX, IDC_TAB_DEFECT_SUMMARY, m_TabDefectSummary);	//2015.06.22
	DDX_Control(pDX, IDC_TAB_DEFECT_LOG, m_ctrlLogTab);
	DDX_Control(pDX, IDC_LIST_LOG1, m_ctrlLogList1);
	DDX_Control(pDX, IDC_LIST_LOG2, m_ctrlLogList2);
	DDX_Control(pDX, IDC_LIST_LOG3, m_ctrlLogList3);
	DDX_Control(pDX, IDC_LIST_LOG4, m_ctrlLogList4);
	DDX_Control(pDX, IDC_LIST_LOG5, m_ctrlLogList5);
	DDX_Control(pDX, IDC_LIST_LOG6, m_ctrlLogList6);
	DDX_Control(pDX, IDC_LIST_LOG7, m_ctrlLogList7);
	DDX_Control(pDX, IDC_LIST_LOG8, m_ctrlLogList8);
	DDX_Control(pDX, IDC_LIST_LOG9, m_ctrlLogList9);
	DDX_Control(pDX, IDC_LIST_LOG10, m_ctrlLogList10);
}

BEGIN_MESSAGE_MAP(CViewAuto, CFormView)
	ON_BN_CLICKED(IDC_BTN_TEST, &CViewAuto::OnBnClickedBtnTest)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_TEST2, &CViewAuto::OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_TEST3, &CViewAuto::OnBnClickedBtnTest3)
	ON_BN_CLICKED(IDC_BTN_RESET, &CViewAuto::OnBnClickedBtnCntReset)
	//ON_BN_CLICKED(IDC_CHK_LOG_S1, &CViewAuto::OnBnClickedChkLogS1)
	//ON_BN_CLICKED(IDC_CHK_LOG_S2, &CViewAuto::OnBnClickedChkLogS2)
	//ON_BN_CLICKED(IDC_CHK_LOG_S3, &CViewAuto::OnBnClickedChkLogS3)
	ON_NOTIFY(NM_CLICK, IDC_GRID_SUMMARY, &CViewAuto::OnGridClickDetectList)
	ON_NOTIFY(NM_CLICK, IDC_GRID_DEFECT_LIST, &CViewAuto::OnGridClickDetectList)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_SUMMARY, &CViewAuto::OnGridDBClickDetectList)
	//ON_NOTIFY(NM_DBLCLK, IDC_GRID_DEFECT_LIST, &CViewAuto::OnGridDBClickDetectList)
	ON_BN_CLICKED(IDC_BTN_PANEL1, &CViewAuto::OnBnClickedBtnPanel1)
	ON_BN_CLICKED(IDC_BTN_PANEL2, &CViewAuto::OnBnClickedBtnPanel2)
	//ON_BN_CLICKED(IDC_RADIO1, &CViewAuto::OnBnClickedRadio1)
	//ON_BN_CLICKED(IDC_RADIO2, &CViewAuto::OnBnClickedRadio2)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO1, IDC_RADIO2, OnBnClickedRadio)
	//ON_STN_CLICKED(IDC_GRID_PNL_SURMMARY, &CViewAuto::OnStnClickedGridPnlSurmmary)
	ON_BN_CLICKED(IDC_BTN_PNL_CNT_RESET, &CViewAuto::OnBnClickedBtnCntReset)
	ON_NOTIFY(NM_CLICK, IDC_GRID_PNL_SURMMARY, &CViewAuto::OnGridClickPanelList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DEFECT_SUMMARY, &CViewAuto::OnTcnSelchangeTabDefectSummary)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_DEFECT_SUMMARY, &CViewAuto::OnTcnSelchangingTabDefectSummary)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DEFECT_LOG, &CViewAuto::OnTcnSelchangeTabDefectLog)
	ON_NOTIFY(NM_CLICK, IDC_GRID_REVIEW_SUMMARY, &CViewAuto::OnGridClickReviewDetectList)	//2015.07.06
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_REVIEW_SUMMARY, &CViewAuto::OnGridDBClickDetectList)
	ON_MESSAGE(WM_UI_CONTROL_PNL_SUMMARY_LIST, &CViewAuto::m_fnUpdatePanelSummary)
	ON_MESSAGE(WM_UI_CONTROL_DEF_SUMMARY_LIST, &CViewAuto::UpdateGridDetectSummaryRow)
	ON_MESSAGE(WM_UI_CONTROL_SAV_MEASURE_DAT, &CViewAuto::m_fnSaveFileMeasureInfo)
	ON_MESSAGE(WM_UI_NEW_MSG_DIALOG, &CViewAuto::m_fnAlarmSet)
	ON_MESSAGE(WM_UI_NEW_MEASURE_MSG_DIALOG, &CViewAuto::m_fnMeasureAlarmSet)
	ON_MESSAGE(WM_UI_NEW_CONTACT_MSG_DIALOG, &CViewAuto::m_fnContactFailAlarmSet)
	ON_MESSAGE(WM_UI_NEW_NG_JUDGE_MSG_DIALOG, &CViewAuto::m_fnNgJudgelAlarm)
	ON_MESSAGE(WM_UI_NEW_POL_MARK_MSG_DIALOG, &CViewAuto::m_fnPolMarkIlegalAlarm)
	ON_MESSAGE(WM_UI_NEW_DEF_TYPE_MSG_DIALOG, &CViewAuto::m_fnDefTypeAlarmSet)
	ON_MESSAGE(WM_UI_NEW_DISPLAY_MSG_DIALOG, &CViewAuto::m_fnDisplayMSGSet)
	ON_MESSAGE(WM_UI_ADD_LOG, &CViewAuto::m_fnAddLog)
	ON_MESSAGE(WM_UI_NEW_LUMINANCE_MSG_DIALOG, &CViewAuto::m_fnLuminanceAlarmSet)	//2016.03.02
	ON_MESSAGE(WM_UI_DISTROY_DIALOG, &CViewAuto::m_fnDistroyMSGDialog)
	ON_MESSAGE(WM_UI_NEW_MANUAL_JUDGE_MSG_DIALOG, &CViewAuto::m_fnManualJudgeSet)	//2018.05.14
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_TEMPERATURE, &CViewAuto::OnBnClickedBtnTemperature)
	ON_BN_CLICKED(IDC_BTN_PANEL_CODE_TEST, &CViewAuto::OnBnClickedBtnPanelCodeTest)
	ON_BN_CLICKED(F_GRDAE, &CViewAuto::OnBnClickedGrdae)
	ON_BN_CLICKED(IDC_GRADE_SET, &CViewAuto::OnBnClickedGradeSet)
END_MESSAGE_MAP()


// CViewAuto 진단입니다.

#ifdef _DEBUG
void CViewAuto::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewAuto::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



void CViewAuto::m_fnAlarmDialog(CDlgMessage*& DlgMsg)
{	
	DlgMsg = NULL;
	DlgMsg = new CDlgMessage();			
	DlgMsg->Create(IDD_DLG_MESSAGE);
	DlgMsg->ShowWindow( SW_HIDE );
	DlgMsg->UpdateWindow();

	//return DlgMsg;
}


LRESULT CViewAuto::m_fnAlarmSet(WPARAM wParam, LPARAM lParam)
{	
	CString strMsg = _T(""), strOverlapLine, strPanelId, strTemp;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	nOverlapCnt= wParam;
	strTemp = (LPCTSTR)lParam;
	AfxExtractSubString(strPanelId, strTemp, 1, '#');
	AfxExtractSubString(strOverlapLine,  strTemp, 0, '#');

	m_fnSaveFileOverlapDefectInfo(strPanelId, strOverlapLine, nOverlapCnt); //2015.10.07
	
	m_fnAlarmDialog(m_pDlgMessage);
	
	/*pFrame->m_DataManager.m_pDlgMessage = (CDlgMessage*)CWnd::
	FromHandle(pFrame->m_DataManager.m_fnAlarmDialog(pFrame->m_DataManager.m_pDlgMessage));*/

	m_pDlgMessage->PostMessage( WM_SOCK_CONTROL_ALARM, (WPARAM)wParam, (LPARAM)lParam );
	
	return 0;
}


LRESULT CViewAuto::m_fnMeasureAlarmSet(WPARAM wParam, LPARAM lParam)
{
	if(m_pDlgMessage)
	{
		delete m_pDlgMessage;
		m_pDlgMessage = NULL;
	}

	m_fnAlarmDialog(m_pDlgMeasure);
	m_pDlgMeasure->PostMessage(WM_SOCK_CONTROL_MEASURE_ALARM,wParam, (LPARAM)lParam);
	return 0;
}

LRESULT CViewAuto::m_fnContactFailAlarmSet(WPARAM wParam, LPARAM lParam)
{
	m_fnAlarmDialog(m_pDlgMessage);// (CDlgMessage*)CWnd::FromHandle( m_fnAlarmDialog(m_pDlgMessage) );		
	m_pDlgMessage->PostMessage(WM_SOCK_CONTROL_CONTACT_ALARM,wParam, (LPARAM)lParam);	
	return 0;
}
// 2015.10.15 add by ikm Packing 용 자동 레시피 변경 S
LRESULT CViewAuto::m_fnNgJudgelAlarm(WPARAM wParam, LPARAM lParam)
{	
	if (!::IsWindow(m_pDlgErrorMsg->GetSafeHwnd()))
	{			
		m_fnAlarmDialog(m_pDlgErrorMsg); //(CDlgMessage*)CWnd::FromHandle( m_fnAlarmDialog(m_pDlgErrorMsg) );
	}	
	m_pDlgErrorMsg->PostMessage(WM_SOCK_CONTROL_LIST_ALARM,wParam, (LPARAM)lParam);		
	return 0;
}
//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 S
LRESULT CViewAuto::m_fnPolMarkIlegalAlarm(WPARAM wParam, LPARAM lParam)
{	
	if (!::IsWindow(m_pDlgErrorMsg->GetSafeHwnd()))
	{
		m_fnAlarmDialog(m_pDlgErrorMsg);
		//(CDlgMessage*)CWnd::FromHandle( m_fnAlarmDialog(m_pDlgErrorMsg) );		
	}	
	m_pDlgErrorMsg->PostMessage(WM_SOCK_CONTROL_LIST_ALARM,wParam, (LPARAM)lParam);	
	return 0;
}
//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 E

LRESULT CViewAuto::m_fnDefTypeAlarmSet(WPARAM wParam, LPARAM lParam)
{		
	if (!::IsWindow(m_pDlgDefTypeMsg->GetSafeHwnd()))
	{
		m_fnAlarmDialog(m_pDlgDefTypeMsg);
		//(CDlgMessage*)CWnd::FromHandle( m_fnAlarmDialog(m_pDlgErrorMsg) );		
	}	
		
	m_pDlgDefTypeMsg->PostMessage( WM_SOCK_CONTROL_DEFTYPE_ALARM, (WPARAM)wParam, (LPARAM)lParam );
	
	return 0;
}
//2016.01.29 Mr.Chu
void CViewAuto::m_fnDisplayMSGDialog(CDlgDisplayMessge*& DlgDspMSG, WPARAM wParam, LPARAM lParam)
{

	DlgDspMSG = NULL;
	if(wParam==0 || wParam==1)
		DlgDspMSG = new CDlgDisplayMessge(TRUE);	
	else
		DlgDspMSG = new CDlgDisplayMessge();
	DlgDspMSG->Create(IDD_DLG_DISPLAY_MSG);
	
	
}

LRESULT CViewAuto::m_fnDisplayMSGSet(WPARAM wParam, LPARAM lParam)
{		

	if (wParam == 2)
	{
		if (m_pDlgDspMsg)
		{
			delete m_pDlgDspMsg;
			m_pDlgDspMsg = NULL;
		}
		
	}
	else
	{
		
		CString strTemp;
		CString strMsgA = _T(""), strMsgB = _T("");
		strTemp = (LPCTSTR)lParam;
		int nStringLength = 0;

		AfxExtractSubString(strMsgA,strTemp , 0, '#');
		nStringLength = strMsgA.GetLength();
		if(!wParam || wParam==1)
		{
			int nTemp = strTemp.GetLength();
			nStringLength = nTemp - nStringLength - 1;
		}
		else
			nStringLength = wParam - nStringLength - 2;
		
		strMsgB = strTemp.Right(nStringLength);

	
		if (!::IsWindow(m_pDlgDspMsg->GetSafeHwnd()))
			m_fnDisplayMSGDialog(m_pDlgDspMsg,wParam,lParam);
		else if(m_pDlgDspMsg->m_bAutoRecipeUse && (wParam && wParam!=1))
			return 0;
		else if(!m_pDlgDspMsg->m_bAutoRecipeUse && (!wParam || wParam==1))
		{
			SendMessage(WM_UI_DISTROY_DIALOG);
			m_fnDisplayMSGDialog(m_pDlgDspMsg,wParam,lParam);
		}

		m_pDlgDspMsg->m_btnRetry.ShowWindow(SW_HIDE);
		m_pDlgDspMsg->m_btnClose.ShowWindow(SW_HIDE);

		if(!wParam || wParam==1)
		{
			m_pDlgDspMsg->m_fnAutoRecipeDesign();
			if(!wParam)
			{
				m_pDlgDspMsg->m_btnRetry.ShowWindow(SW_SHOW);
				m_pDlgDspMsg->m_btnClose.ShowWindow(SW_SHOW);
			}
		}

		m_pDlgDspMsg->m_fnSetText(strMsgA,strMsgB);
		m_pDlgDspMsg->ShowWindow( SW_SHOW );
		m_pDlgDspMsg->UpdateWindow();
	}

	return 0;
}
// CViewAuto 메시지 처리기입니다.

void CViewAuto::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

//	m_fnInitGridLog();

	m_fnCreateDlgMap();
	m_fnCreateDlgPanelJudge();
	m_fnCreateDlgDefInfo();
	//m_fnCreateDlgVisionState();
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);
	//2016.02.01 Packing UI
	int nEqpObject	= GetPrivateProfileInt(EQP_STATE, KEY_EQP_OBJECT, 0, strConfigFile);
	int nEqpModel	 = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0, strConfigFile);	//0:AVL 1:ABI
	int nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile); //;중형 0 소형 = 1 대형 = 2

	if (nEqpObject != P_PROJECT)
	{
		m_fnCreateDlgStageState();
		GetDlgItem(IDC_STATIC_STAGE_STATUS)->ShowWindow(SW_SHOW);

		if( (nEqpModel == EQP_TYPE_ABI) && (nEqpSizeType == EQP_LARGE_SIZE) )
			GetDlgItem(IDC_BTN_PANEL_CODE_TEST)->ShowWindow(SW_SHOW);	//2016.09.14	Mr.Chu	고객사 요청 PanelCodeTest 기능 사용 조건

	}
	else
	{
		GetDlgItem(IDC_STATIC_PANEL_SUMMARY)->SetWindowPos(NULL, 630, 5, 620, 560, SWP_NOZORDER);
		GetDlgItem(IDC_GRID_PNL_SURMMARY)->SetWindowPos(NULL, 640, 25, 595, 530, SWP_NOZORDER);
	}
	//2016.02.01 Packing UI
	//m_fnCreateDlgLuminanceState();
	
	m_fnInitGridDefectSummary(nEqpObject);
	m_fnInitGridPanelSummary();
	m_fnLoadPanelSummary();
	m_fnInitGridReviewDefectSummary(nEqpObject);	//2015.06.22

	//m_fnInitGradeSet();	//2018.12.13 add by ryu
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	/*((CButton*)GetDlgItem(IDC_CHK_LOG_S1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_CHK_LOG_S2))->SetCheck(TRUE);

	if(pFrame->m_stParamInfo.nEqpType == EQP_TYPE_ACS)
		((CButton*)GetDlgItem(IDC_CHK_LOG_S3))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_CHK_LOG_S3))->EnableWindow(FALSE);*/

	for(int i=0;i<9;i++)
		m_bLog_Sel[i] = TRUE;

	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);

	//2015.06.22
	m_TabDefectSummary.InsertItem(0,_T("Current Defect Summary"));
	m_TabDefectSummary.InsertItem(1,_T("Review Defect Summary"));
	m_TabDefectSummary.SetCurFocus(0);
	((CGxStaticEx*)GetDlgItem(IDC_GRID_SUMMARY))->ShowWindow(SW_SHOW);
	((CGxStaticEx*)GetDlgItem(IDC_GRID_REVIEW_SUMMARY))->ShowWindow(SW_HIDE);
	//2015.06.22

	m_nViewDefectSummary = DEFECT_CURRENT;	//2015.07.06

	//2015.07.03 Chu
	m_ctrlLogTab.DeleteAllItems();
	m_ctrlLogTab.InsertItem(0,_T("# Total State"));
	m_ctrlLogTab.InsertItem(1,_T("# Stage1"));
	m_ctrlLogTab.InsertItem(2,_T("# Stage2"));

	GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);

	////2015.07.03 Chu
	//if(pFrame->m_stParamInfo.nEqpType == EQP_TYPE_ACS)


	if (nEqpSizeType == EQP_LARGE_SIZE || nEqpSizeType == EQP_MONITOR_SIZE) 
	{
	m_ctrlLogTab.InsertItem(3,_T("# Stage3"));
	m_ctrlLogTab.InsertItem(4,_T("# Stage4"));
	m_ctrlLogTab.InsertItem(5,_T("# Stage5"));
	m_ctrlLogTab.InsertItem(6,_T("# Stage6"));
	if(nEqpModel == EQP_TYPE_ABI)
	{
	m_ctrlLogTab.InsertItem(7,_T("# Stage7"));
	m_ctrlLogTab.InsertItem(8,_T("# Stage8"));
	m_ctrlLogTab.InsertItem(9,_T("# Stage9"));
	}
	}
	else if (nEqpSizeType != EQP_LARGE_SIZE && nEqpModel == EQP_TYPE_ABI) 
	{
		//m_ctrlLogTab.InsertItem(2,_T("# Stage2"));
		m_ctrlLogTab.InsertItem(3,_T("# Stage3"));
	}
	/*if (nEqpSizeType != EQP_LARGE_SIZE && nEqpModel == EQP_TYPE_AVL) 
	m_ctrlLogTab.InsertItem(2,_T("# Stage2"));

	if (nEqpSizeType && nEqpModel == EQP_TYPE_ABI) 
	{
	m_ctrlLogTab.InsertItem(2,_T("# Stage2"));
	}*/

	m_pDlgPopUpImage = new CDlgPopUpImage();
	m_pDlgPopUpImage->Create(IDD_DLG_POP_DEF_IMG, this);
	m_pDlgPopUpImage->ShowWindow(SW_HIDE);
	CString cCountRoot, strTemp, strStatsTime ;	
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);	
	TCHAR szSectionCnt[3];
	CTime time = CTime::GetCurrentTime();
	for (int i =0 ; i < 3 ; i ++)
	{
		strTemp.Format(_T("STATS_TIME%d"), i + 1);
		GetPrivateProfileString(EQP_STATE, strTemp, _T(""), szSectionCnt, sizeof(szSectionCnt), cCountRoot);
		strTemp = szSectionCnt;
		if (_ttoi(strTemp) == time.GetHour())
			m_bResetFlag[i] = TRUE;
		else
			m_bResetFlag[i] = TRUE;
	}

	if(m_pDlgTemperature)
	{
		delete m_pDlgTemperature;
		m_pDlgTemperature = NULL;
	}

	m_pDlgTemperature = new CDlgTemperature();
	m_pDlgTemperature->Create(IDD_DLG_TEMPERATURE, this);	
	m_pDlgTemperature->ShowWindow(SW_HIDE);

	
	SetTimer(1001, 10000, NULL);
}

void CViewAuto::m_fnInitGridLog()
{
	int nColumnCount;

	// Log Grid 생성
	CRect rect;
	GetDlgItem(IDC_GRID_LOG)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_GridProcLog = new CGridCtrl();
	m_GridProcLog->Create(rect, this, IDC_GRID_LOG);
	m_GridProcLog->SetParent(this);
	m_GridProcLog->SetListMode(true);
	m_GridProcLog->SetEditable(FALSE);
	//m_GridProcLog->SetHeaderSort();
	m_GridProcLog->SetFixedBkColor(RGB(220, 240, 220));
	m_GridProcLog->SetBkColor(RGB(200, 200, 200));
	m_GridProcLog->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridProcLog->SetFixedRowCount(1);

	CString colNameSystem[2] = { _T("Time"), _T("System Event") };
	nColumnCount = 2;
	m_GridProcLog->SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		if(k == 0)
			m_GridProcLog->SetColumnWidth(k, 150);	
		else 
			m_GridProcLog->SetColumnWidth(k, 470);

		m_GridProcLog->SetItemText(0, k, colNameSystem[k]);
	}
}

void CViewAuto::m_fnInitGridDefectSummary(int nEqpObject)
{
	// Log Grid 생성
	CRect rect;
	GetDlgItem(IDC_GRID_SUMMARY)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_GridDefectSummary = new CGridCtrl();
	m_GridDefectSummary->Create(rect, this, IDC_GRID_SUMMARY);
	m_GridDefectSummary->SetParent(this);
	m_GridDefectSummary->SetListMode(true);
	m_GridDefectSummary->SetEditable(FALSE);
	//m_GridDefectSummary->SetHeaderSort();
	m_GridDefectSummary->SetFixedBkColor(RGB(220, 240, 220));
	m_GridDefectSummary->SetBkColor(RGB(200, 200, 200));
	m_GridDefectSummary->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridDefectSummary->SetFixedRowCount(1);
#if 0
	CString colNameDefect[8] = { _T("Num"), _T("Left"), _T("Top"), _T("Width"), _T("Height"), _T("Size"), _T("Code"), _T("Cam") };
	nColumnCount = 8;
	m_GridDefectSummary->SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		m_GridDefectSummary->SetColumnWidth(k, 60);

		m_GridDefectSummary->SetItemText(0, k, colNameDefect[k]);
	}
#else
	if (nEqpObject != P_PROJECT)
	{
		//nColumnCount = MAX_DEFECT_SUMMARY_COL;

		TCHAR colNameInspSum[MAX_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("NUM"),  _T("LAYER_NO"),  _T("DEFECT_NO"),  _T("DEFECT_CODE"),  _T("FLAG"),  _T("PIXEL_X"), 
		_T("PIXEL_Y"),  _T("GATE_START_NO"),  _T("DATA_START_NO"),  _T("GATE_END_NO"),  _T("DATA_END_NO"), 
		_T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"),  _T("DEF_SIZE"), 
		_T("IMG_NO"),  _T("IMG_SIZE_X"),  _T("IMG_SIZE_Y"),  _T("TYPE"),  _T("CAM_NO")  };

		m_GridDefectSummary->SetColumnCount(MAX_DEFECT_SUMMARY_COL);

		for(int k = 0; k < MAX_DEFECT_SUMMARY_COL; k++)
		{
			if(k == 0 || k == 1)
				m_GridDefectSummary->SetColumnWidth(k, 85);
			else if(k == 3)
				m_GridDefectSummary->SetColumnWidth(k, 110);
			else
				m_GridDefectSummary->SetColumnWidth(k, 70);

			m_GridDefectSummary->SetItemText(0, k, colNameInspSum[k]);
		}
	}
	else
	{
		//nColumnCount = MAX_P_DEFECT_SUMMARY_COL;
		
		TCHAR colNameInspSum[MAX_P_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("NUM"),  _T("TIME"),  _T("DEVICE"),  _T("CELL_ID"),  _T("PAD"),  _T("JUDGE"), 
		_T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"), _T("DEFECT_TYPE"),
		_T("AREA"),  _T("BOX_AREA"),  _T("BOX_X_MAX"),  _T("BOX_X_MIN"), _T("BOX_Y_MAX"),  _T("BOX_Y_MIN"),  
		_T("CENTER_X"),  _T("CENTER_Y"),  _T("LENGTH"), _T("BREADTH"),  _T("ELONGATION"),  _T("COMPACTNESS"),  _T("HOLECOUNT"),  
		_T("FERET_MIN_ANGLE"), _T("FERET_MAX_ANGLE"),  _T("FERET_MIN"),  _T("FERET_MAX"),  _T("FERET_MEAN"),  _T("FERET_X"), _T("FERET_Y"),  
		_T("FERET_ELONGATION"),  _T("PIXEL_MAX"),  _T("PIXEL_MEAN"),  _T("PIXEL_MIN"),   _T("PIXEL_SIGMA"),  _T("PIXEL_SUM") };

		m_GridDefectSummary->SetColumnCount(MAX_P_DEFECT_SUMMARY_COL);

		for(int k = 0; k < MAX_P_DEFECT_SUMMARY_COL; k++)
		{
			if(k == 0 || k == 1)
				m_GridDefectSummary->SetColumnWidth(k, 85);
			else if(k == 3)
				m_GridDefectSummary->SetColumnWidth(k, 110);
			else
				m_GridDefectSummary->SetColumnWidth(k, 70);

			m_GridDefectSummary->SetItemText(0, k, colNameInspSum[k]);
		}
	}

#endif
}

void CViewAuto::m_fnInitGridPanelSummary()
{
 	int nColumnCount;
	CString cCountVal = _T(""), cCountRoot = _T("");
	// Log Grid 생성
	CRect rect;

	//cCountRoot.Append(ROOT_PATH);
	//cCountRoot.Append(CONFIG_COUNT_PATH);

	//nGradeOK = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
	//nGradeNG = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);

	/*cCountVal.Format(_T("%d"), nGradeOK);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->SetCaption(cCountVal);
	cCountVal.Format(_T("%d"), nGradeNG);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->SetCaption(cCountVal);
	cCountVal.Format(_T("%d"), nGradeNG + nGradeOK);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_TOTALCELL))->SetCaption(cCountVal);*/

	GetDlgItem(IDC_GRID_PNL_SURMMARY)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_GridPanelSummary = new CGridCtrl();
	m_GridPanelSummary->Create(rect, this, IDC_GRID_PNL_SURMMARY);
	m_GridPanelSummary->SetParent(this);
	m_GridPanelSummary->SetListMode(true);
	m_GridPanelSummary->SetEditable(FALSE);
	//m_GridDefectSummary->SetHeaderSort();
	m_GridPanelSummary->SetFixedBkColor(RGB(220, 240, 220));
	m_GridPanelSummary->SetBkColor(RGB(200, 200, 200));
	m_GridPanelSummary->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridPanelSummary->SetFixedRowCount(1);
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	//2015.09.11 Mr.Chu
	TCHAR colNameInspSum[MAX_PANEL_SUMMARY_COL][MAX_PATH]= { _T("INSP_TIME"),  _T("PANEL_ID"),  _T("GRADE"),  _T("CODE"),  _T("DEF_NAME") ,  _T("CF"),_T("EQ"), _T("TFT"), _T("ST"),_T("PA+"),_T("MEA_FLAG")/*,  _T("PENETRATE")*/};
	
	nColumnCount = 6;
	m_GridPanelSummary->SetColumnCount(MAX_PANEL_SUMMARY_COL);

	for(int k = 0; k < MAX_PANEL_SUMMARY_COL; k++)
	{
		if(k == 0 )
			m_GridPanelSummary->SetColumnWidth(k, 140);
		else if(k == 1)
			m_GridPanelSummary->SetColumnWidth(k, 150);
		else if (k == 4 )
			m_GridPanelSummary->SetColumnWidth(k, 130);
		else if (k == 5 || k == 7)
			m_GridPanelSummary->SetColumnWidth(k, 30);
		else if (k == 10 )
			m_GridPanelSummary->SetColumnWidth(k, 70);	//2021.04.16 add by mby
		else
			m_GridPanelSummary->SetColumnWidth(k, 50);
		

		m_GridPanelSummary->SetItemText(0, k, colNameInspSum[k]);
	}

	//test file save
	//m_fnUpdatePanelSummary();
}
//2018.12.13 add by ryu
void CViewAuto::m_fnInitGradeSet()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	int nRet = 0;
	
	CString strTemp,cCountRoot,strRecipeNo;
	int nRead = 0;
	TCHAR strBuffer[255];

	//2020.08.25 add by mby S-
	//Recipe No check
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);
	strRecipeNo = strBuffer;

	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	//2020.08.25 add by mby E-

	CString arrKey[] = {
		_T("TFT_CORNERBROKEN"),
		_T("CF_CORNERBROKEN"),
		_T("TFT_RBROKEN"),
		_T("CF_RBROKEN"),
		_T("TFT_BURR"),
		_T("CF_BURR"),
		_T("TFT_SCARCH"),
		_T("CF_SCARCH"),
		_T("TFT_CELL_BROKEN"),
		_T("CF_CELL_BROKEN"),
		_T("ETC_POINT"),
		_T("GRADE_ALIGN_FAIL"),
		_T("TFT_BROKEN_VAL"),
		_T("CF_BROKEN_VAL"),
		_T("CF_COR_BROKEN_VAL")	// 2020.02.13 by mby
	};

	TCHAR szBuf[MAX_PATH]={0,};
	for(int i = 0; i < (MAX_COUNT_GRADE) ; i++)
	{
		GetPrivateProfileString(_T("GRADE_INPUT"), arrKey[i], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + strRecipeNo + _T(".ini"));
		m_strValue_GradeAuto[i].Format(_T("%s"), szBuf);
	}
	//2019.01.15 여기 문제
	CString arrKey_Rank[] = {
		_T("TFT_CORNERBROKEN"),
		_T("CF_CORNERBROKEN"),
		_T("TFT_RBROKEN"),
		_T("CF_RBROKEN"),
		_T("TFT_BURR"),
		_T("CF_BURR"),
		_T("TFT_SCARCH"),
		_T("CF_SCARCH"),
		_T("TFT_CELL_BROKEN"),
		_T("CF_CELL_BROKEN"),
		_T("ETC_POINT"),
		_T("GRADE_ALIGN_FAIL"),
		_T("TFT_RBROKEN_VAL"),
		_T("CF_RBROKEN_VAL"),
		_T("CF_COR_RBROKEN_VAL")
	};
	//2019.01.15 rank 등급 표시
	for(int k = 0; k < MAX_COUNT_RANK; k++)
	{
		GetPrivateProfileString(_T("RANK_VALUE"), arrKey_Rank[k], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + strRecipeNo + _T(".ini"));
		m_strValue_RankAuto[k].Format(_T("%s"), szBuf);
	}
	CString strkey[]=
	{
		_T("TFT_A"),
		_T("TFT_B"),
		_T("TFT_C"),  
		_T("TFT_D"),
		_T("CF_A"),
		_T("CF_B"),
		_T("CF_C"),
		_T("CF_D"),
		_T("CF_COR_BROKEN")	//2020.02.13 by mby
	};

	for(int j =0; j < VALUE_COUNT ; j++)
	{
		GetPrivateProfileString(_T("GRADE_VALUE"), strkey[j], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + strRecipeNo + _T(".ini"));
		m_strValue_NumAuto[j].Format(_T("%s"), szBuf);
	}
}
//2018.12.13 add by ryu
LRESULT CViewAuto::m_fnUpdatePanelSummary(WPARAM wParam, LPARAM lParam)
{
	CString strQmsFilePath;
	strQmsFilePath = (LPCTSTR)lParam;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		G_AddLog(3, 2, _T("m_fnUpdatePanelSummary null"));
		return 0;
	}
	
	int nlength = 0, nGradeCnt = 0, nGradeNG = 0, nGradeOK = 0;
	CString cCountVal = _T(""), cCountRoot = _T("") ;
	int i = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	
	 CString strJudgeCode;
	int nRowCount = m_GridPanelSummary->GetRowCount();
	int nColCount = m_GridPanelSummary->GetColumnCount();

	if(nRowCount >= MAX_PANEL_LIST + 1)
	{
		//m_GridPanelSummary->DeleteRow(1);
		m_GridPanelSummary->DeleteRow(MAX_PANEL_LIST);
	}
	//m_GridPanelSummary->InsertRow(_T(""));
	m_GridPanelSummary->InsertRow(_T(""), nRowCount > 1 ? 1 : -1);

	//nRowCount = m_GridPanelSummary->GetRowCount() - 1;
	nRowCount = 1;

	m_GridPanelSummary->SetRedraw(FALSE);
	m_GridPanelSummary->SetItemState(m_GridPanelSummary->GetRowCount()-1, 0,m_GridPanelSummary->GetItemState(m_GridPanelSummary->GetRowCount() - 1, 0)|GVIS_READONLY);		
	
	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime);
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_TIME, tszIndex);

	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultPanelId());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_PNLID, tszIndex);

	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultGrade());
	tszIndex[nlength] = _T('\0');
	strJudgeCode = tszIndex;
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_GRADE, tszIndex);

	//2015.09.11 Mr.Chu
	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultDefCode());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_DEFECT, tszIndex);

	// 2018.04.16 YHS
	m_pDlgPanelJudge->SetResultDefectCodeName(m_fnSetDefectName(m_pDlgPanelJudge->GetResultDefCode())); 
	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultDefCodeName());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_DEFECT_NAME, tszIndex);
	

	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),m_pDlgPanelJudge->GetResultCFJudge());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_CF, tszIndex);

	//2019.06.20 by ryu
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_EQ , pFrame->m_DataManager.m_strPolPositionEQ_CV2);
	nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultTftJudge());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_TFT, tszIndex);
	//2019.06.20 by ryu
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_STAGE , pFrame->m_DataManager.m_strPolPositionStage_CV2);


	pFrame->m_stParamInfo.nEqpObject = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_OBJECT, 1);
	for (int i = 0; i < m_GridPanelSummary->GetColumnCount(); i ++)
	{
		if (strJudgeCode != _T("A") && pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
		{
			m_GridPanelSummary->SetItemBkColour(nRowCount, i , RGB(255, 66, 0));
			m_GridPanelSummary->SetItemFgColour(nRowCount, i , RGB(255, 255, 255));
		}
	}

	/*nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_pDlgPanelJudge->GetResultPenetJudge());
	tszIndex[nlength] = _T('\0');
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_PENETRATE, tszIndex);*/
	//2020.04.13 add by mby 
	CString PaCodeFlag = _T("0");	
	if(pFrame->m_DataManager.m_bDcc_Check1_flag){
		PaCodeFlag = _T("1");
	}else if(pFrame->m_DataManager.m_bDcc_Check2_flag){
		PaCodeFlag = _T("2");
	}
	m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_PENETRATE, PaCodeFlag);
	//2020.04.13 add by mby
	//2021.04.22 add by mby
	if(pFrame->m_DataManager.m_bMea_Fail_flag == 1){
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_MEA_FLAG, _T("CF_NG"));	//2021.04.16 add by mby
	}else if( pFrame->m_DataManager.m_bMea_Fail_flag == 2){
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_MEA_FLAG, _T("TFT_NG"));	//2021.04.16 add by mby
	}else{
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_MEA_FLAG, _T("_"));	//2021.04.16 add by mby
	}
	m_GridPanelSummary->SetRedraw(TRUE);	
	
	m_GridPanelSummary->Refresh();
	G_AddLog(3, 2, _T("Refresh"));
	m_fnSaveFilePanelInfo(strQmsFilePath);
	m_fnSaveFilePanelStats();		//2019.01.17 add by mby

	//cCountVal = ((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->GetDlgItemText() + ((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->GetDlgItemText();
}
void CViewAuto::m_fnUpdateSummary(CString strData, BOOL bClear)
{
	if(bClear)
		m_GridDefectSummary->DeleteNonFixedRows();

	int nRowCount = m_GridDefectSummary->GetRowCount();
	int nColCount = m_GridDefectSummary->GetColumnCount();

	m_GridDefectSummary->InsertRow(_T(""), nRowCount > 1 ? 1 : -1);

	int nCol = 0;
	int curPos= 0;
	CString resToken= strData.Tokenize(EXT_COLON,curPos);
	while (resToken != "")
	{
		m_GridDefectSummary->SetItemText(1, nCol++, resToken);
		if (nCol+1 > nColCount)
			break;
		resToken= strData.Tokenize(EXT_COLON,curPos);
	};

	if (nRowCount > MAX_GRID_LOG)
	{
		m_GridDefectSummary->DeleteRow(nRowCount);
	}

	m_GridDefectSummary->Refresh();
}
#if 0
void CViewAuto::m_fnAddLog(CString strLog)
{
	CTime time = CTime::GetCurrentTime();
	CString strTime;

	strTime = time.Format(_T("%Y/%m/%d %H:%M:%S"));
	
	int nRowCount = m_GridProcLog->GetRowCount();

	m_GridProcLog->InsertRow(_T(""));

	m_GridProcLog->SetItemText(nRowCount, 0, strTime);
	m_GridProcLog->SetItemText(nRowCount, 1, strLog);

	if (nRowCount > MAX_GRID_LOG)
		m_fnClearLog();

	m_GridProcLog->Refresh();
}
#else
LRESULT/*void*/ CViewAuto::m_fnAddLog(WPARAM wParam, LPARAM lParam)//(CString strLog, BOOL bNoUseTotalLog /*= FALSE*/)
{
	CString strCurrentTime, strLog;
	CTime ctCurrentTime;
	ctCurrentTime = CTime::GetCurrentTime();
	strCurrentTime = ctCurrentTime.Format("%H시%M분%S초 : ");
	strLog = (LPCTSTR)lParam;
	strLog = strCurrentTime + strLog; //strLog;
	
	// 2015.11.27 comment by ikm for Log Delay
	if (!wParam/*bNoUseTotalLog*/)
	{
		if(m_ctrlLogList1.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList1.DeleteString(MAX_GRID_LOG - 1);
		m_ctrlLogList1.InsertString(0, strLog);
		m_ctrlLogList1.SetCurSel(0);
	}
	
	if(m_bLog_Sel[0] == TRUE)
	{
		if(m_ctrlLogList2.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList2.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList2.InsertString(0, strLog);
		m_ctrlLogList2.SetCurSel(0);
	}
	else if(m_bLog_Sel[1] == TRUE)
	{
		if(m_ctrlLogList3.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList3.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList3.InsertString(0, strLog);
		m_ctrlLogList3.SetCurSel(0);
	}
	else if(m_bLog_Sel[2] == TRUE)
	{
		if(m_ctrlLogList4.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList4.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList4.InsertString(0, strLog);
		m_ctrlLogList4.SetCurSel(0);
	}
	else if(m_bLog_Sel[3] == TRUE)
	{
		if(m_ctrlLogList5.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList5.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList5.InsertString(0, strLog);
		m_ctrlLogList5.SetCurSel(0);
	}
	else if(m_bLog_Sel[4] == TRUE)
	{
		if(m_ctrlLogList6.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList6.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList6.InsertString(0, strLog);
		m_ctrlLogList6.SetCurSel(0);
	}
	else if(m_bLog_Sel[5] == TRUE)
	{
		if(m_ctrlLogList7.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList7.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList7.InsertString(0, strLog);
		m_ctrlLogList7.SetCurSel(0);
	}
	else if(m_bLog_Sel[6] == TRUE)
	{
		if(m_ctrlLogList8.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList8.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList8.InsertString(0, strLog);
		m_ctrlLogList8.SetCurSel(0);
		
	}
	else if(m_bLog_Sel[7] == TRUE)
	{
		if(m_ctrlLogList9.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList9.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList9.InsertString(0, strLog);
		m_ctrlLogList9.SetCurSel(0);
	}
	else if(m_bLog_Sel[8] == TRUE)
	{
		if(m_ctrlLogList10.GetCount() == MAX_GRID_LOG)
			m_ctrlLogList10.DeleteString(MAX_GRID_LOG - 1);

		m_ctrlLogList10.InsertString(0, strLog);
		m_ctrlLogList10.SetCurSel(0);
	}
	return TRUE;
}
#endif
void CViewAuto::m_fnClearLog()
{
	m_GridProcLog->DeleteNonFixedRows();
	m_GridProcLog->Refresh();
}

void CViewAuto::m_fnCreateDlgMap()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	wchar_t chRead[255] = {0};
	//2016.02.24 Mr.Chu
	CString strTemp = _T(""), strConfigFile = _T(""), strRecipePath = _T("");
	CString strCurrentlyRecipe = _T("");
	int nDefMapAxis = 0, nIndex = 0, nReportQMSDefMapCoord = 0;
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	int nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile); //;중형 0 소형 = 1 대형 = 2
	//

	GetDlgItem(IDC_STATIC)->SetWindowText(_T("Defect Map"));

	if(m_pDlgMap == NULL)	
	{
		m_pDlgMap = new CDlgDefectMap();
		m_pDlgMap->Create(IDD_DLG_MAP, this);
	}
	m_pDlgMap->ShowWindow(SW_SHOW);
	
	m_pDlgMap->MoveWindow(680, 140, 540, 415-100);

	strRecipePath.Append(ROOT_PATH);
	strRecipePath.Append(RECIPE_INFO_PATH);

	GetPrivateProfileString(RECIPE_INFO,RECIPE_CURRENT,_T(""),(LPTSTR)chRead,255,strRecipePath);
	strCurrentlyRecipe = chRead;

	GetPrivateProfileString(strCurrentlyRecipe,RECIPE_DEF_MAP_TYPE,_T(""),(LPTSTR)chRead,255,strRecipePath);
	nIndex = _ttoi(chRead);

	switch(nIndex)
	{
	case IDC_RADIO_RECIPE_MAP_LT:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
		break;
	case IDC_RADIO_RECIPE_MAP_LB:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LB;
		break;
	case IDC_RADIO_RECIPE_MAP_RT:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RT;
		break;
	case IDC_RADIO_RECIPE_MAP_RB:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RB;
		break;
	default:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
		break;
	}

	GetPrivateProfileString(strCurrentlyRecipe,RECIPE_DEF_MAP_SIZE,_T(""),(LPTSTR)chRead,255,strRecipePath);
	strTemp = _T("");
	nIndex	= NULL;

	while (TRUE)
	{
		if(chRead[nIndex] == _T(',') )
		{
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth = _wtof(strTemp);
			strTemp = _T("");
			nIndex++;
			continue;
		}
		else if(chRead[nIndex] == _T('') )
		{
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight = _wtof(strTemp);
			break;
		}

		strTemp += chRead[nIndex];
		nIndex++;
	}

	//	셋팅이 안되어 있을 경우에 대한 예외처리
	if(pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth == NULL
		|| pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight == NULL)
	{
		if ( nEqpSizeType == EQP_LARGE_SIZE )
		{
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth  = DEFAULT_PANEL_WIDTH_LARGE_SIZE;
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight = DEFAULT_PANEL_HEIGHT_LARGE_SIZE;
		}
		else if( nEqpSizeType == EQP_SMALL_SIZE ||
			nEqpSizeType == EQP_MEDIUM_SIZE )
		{
		pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth  = DEFAULT_PANEL_WIDTH_SIZE;
		pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight = DEFAULT_PANEL_HEIGHT_SIZE;
		}
		else if( nEqpSizeType == EQP_MONITOR_SIZE )
		{
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth  = DEFAULT_PANEL_WIDTH_MONITOR_SIZE;
			pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight = DEFAULT_PANEL_HEIGHT_MONITOR_SIZE;
		}
	}
	//

	nDefMapAxis = GetPrivateProfileInt(strCurrentlyRecipe, RECIPE_DEF_MAP_MAJOR_AXIS, 0, strRecipePath);
		
	switch(nDefMapAxis)
	{
	case RECIPE_DEF_MAP_X_Y_REVERSAL_OFF:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = FALSE;
		break;
	case RECIPE_DEF_MAP_X_Y_REVERSAL_ON:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = TRUE;
		break;
	default:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = FALSE;
		break;
	}

	nReportQMSDefMapCoord = GetPrivateProfileInt(strCurrentlyRecipe, RECIPE_DEF_MAP_COORD_QMS, 0, strRecipePath);

	switch(nReportQMSDefMapCoord)
	{
	case RECIPE_DEF_MAP_COORD_QMS_OFF:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = FALSE;
		break;
	case RECIPE_DEF_MAP_COORD_QMS_ON:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = TRUE;
		break;
	default:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = FALSE;
		break;
	}
	pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo = GetPrivateProfileInt(strCurrentlyRecipe, RECIPE_ALIGNPC_NO, 0, strRecipePath);	//2021.07.07 add by mby
	UpdateData(FALSE);
}

void CViewAuto::m_fnCreateDlgDefInfo()
{

	GetDlgItem(IDC_STATIC)->SetWindowText(_T("Defect Info"));
		
	if(m_pDlgDefInfo == NULL)	
	{
		m_pDlgDefInfo = new CDlgDefectInfo();
		m_pDlgDefInfo->Create(IDD_DLG_DEF_INFO, this);
	}
	//SendMessage(WM_SOCK_VISION_ACCEPT, WPARAM(nErrorCode), LPARAM(pSockClient));
	
	//m_pDlgMap->ShowWindow(SW_HIDE);
	m_pDlgDefInfo->ShowWindow(SW_SHOW); 
	//m_pDlgDefInfo->MoveWindow(23, 30, 565, 415);
	m_pDlgDefInfo->MoveWindow(23, 130, 585, 415);
	
	//m_pDlgMap->ShowWindow(SW_HIDE);
	//m_pDlgDefInfo->ShowWindow(SW_SHOW);
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	
}

void CViewAuto::m_fnCreateDlgVisionState()
{
	if(m_pDlgVisionState == NULL)	// 2015.01.12 ljk
	{
		m_pDlgVisionState = new CDlgVisionState();
		m_pDlgVisionState->Create(IDD_DLG_VISION_STATE, this);
	}
	m_pDlgVisionState->ShowWindow(SW_SHOW);
	m_pDlgVisionState->MoveWindow(640, 28, 585, 120);
}

void CViewAuto::m_fnCreateDlgStageState()
{
	if(m_pDlgStageState == NULL)	// 2015.01.12 ljk
	{
		m_pDlgStageState = new CDlgStageState();
		m_pDlgStageState->Create(IDD_DLG_STAGE_STATE, this);
	}
	m_pDlgStageState->ShowWindow(SW_SHOW);
	m_pDlgStageState->MoveWindow(640, 20, 600, 100);
	//m_pDlgStageState->MoveWindow(640, 180, 1250, 240);	//2015.05.11 edit
}

void CViewAuto::m_fnCreateDlgPanelJudge()
{
	if(m_pDlgPanelJudge == NULL)
	{
		m_pDlgPanelJudge = new CDlgPanelJudge();
		m_pDlgPanelJudge->Create(IDD_DLG_PNL_JUDGE, this);
	}
	m_pDlgPanelJudge->ShowWindow(SW_SHOW);
	//m_pDlgPanelJudge->MoveWindow(640, 28, 585, 140);
	m_pDlgPanelJudge->MoveWindow(23, 20, 585, 80);
}

void CViewAuto::m_fnCreateDlgLuminanceState()
{
	m_pDlgLuminanceState = new CDlgLuminanceState();
	m_pDlgLuminanceState->Create(IDD_DLG_LUMINANCE, this);
	m_pDlgLuminanceState->ShowWindow(SW_SHOW);
	m_pDlgLuminanceState->MoveWindow(640, 360, 585, 130);
}


BOOL CViewAuto::m_testImgFile(CString strPanelId)
{
	CString strPathName, strCmpFileName, strFileName;
	CString strImgFilePath = _T(""), strRootPath = _T(""), strPath = _T(""), strtestPath = _T("");
	BOOL bWorking;
	int nVisionCnt = 1, nCnt = 1;
	CFileFind finder;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	CString strtemp, resToken;	
	int curPos;
	strtemp = strPanelId;
	curPos = 0;

	strRootPath.Format(_T("%s%s\\"), ROOT_PATH, RESULT_PATH);

	strImgFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iWait;

	while( nVisionCnt <= pFrame->m_stParamInfo.nCountVisionClient )
	{
		strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, nVisionCnt);
		nCnt  = 1;
		bWorking = finder.FindFile(strPath + _T("*.*"));	// 모든 파일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

			if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

			strFileName = finder.GetFileName();	// 파일 이름 얻기			

			if (strFileName.Find(EXTENSION_JPG) != -1)
			{
				if ( strFileName.Find(strPanelId) != -1)
				{	
					//strImgFilePath = m_testQmsImgFile(strFileName);
					strtestPath.Format(_T("%00004d"),nCnt );
					strImgFilePath.Format(_T("%s\\%s%s.jpg"), strPath, _T("PANELID00000000001_10_"), strtestPath);

					//CopyFile(strPath + strFileName, strImgFilePath, 0);
					MoveFile(strPath + strFileName , strImgFilePath);

					nCnt ++;
				}
			}
			if (bWorking == FALSE) break;
		}
		nVisionCnt ++;
	}
	finder.Close();
	return TRUE;
}

void CViewAuto::OnBnClickedBtnTest()
{
	try
	{

		long lRet = -1;
		int nPubNo = 0;

		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	
		//pFrame->m_DataManager.m_fnDelLastClsyfyInfo(_T("B21"), 0);
		//pFrame->m_DataManager.m_fnDelLastClsyfyInfo(_T("B21"), 1);

		//Recipe Edit	2018.01.20
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_CMD_NO, 101);
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_RCP_NO, 2);
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_0, 700);
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_1, 700);
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_2, 700);
		//Recipe Edit

		lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_RCP_EDIT_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
		nPubNo = lRet + 1;
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_RCP_EDIT_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

		//	pFrame->m_DataManager.m_fnMeasureAlarmSet(_T("adsfadsfasdfsad"), _T("sadfasdfasfdafd"), strPanelID);
		//pView->m_pDlgPLC->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, TRUE);
		//pView->m_pDlgPLC->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, TRUE);
		//Sleep(1000);
		//pView->m_pDlgPLC->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, FALSE);
		//pView->m_pDlgPLC->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, FALSE);

		//pFrame->m_DataManager.m_fnftnRun();

		//testResultProc();

		/*while(true)
		{*/
		//	if (nTestCnt == 3)
		//{
		//nTestCnt = 0;

		//}
		/*for(int i = 0; i<=6; i++)
		{
		pFrame->m_DataManager.m_fnClassifyProcInfo(_T("PNL000000000000000"),_T(""), i);
		}

		Sleep(500);*/
		//pFrame->m_DataManager.m_fnClassifyProcInfo(_T("PNL000000000000000"),_T(""), 1);
		//pFrame->m_DataManager.m_fnClassifyProcInfo(_T("N1A00M59004434AH09"),_T(""), 1);
		//}
	}
	catch (CMemoryException* e)
	{
		e->Delete();
	}
	catch (CFileException* e)
	{
		e->Delete();
	}
	catch (CException* e)
	{
		e->Delete();
	}	
}


void CViewAuto::OnBnClickedBtnTest2()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	pFrame->m_pSockManager->m_fnRequestVisionState(VISION_PC_1);
}

void CViewAuto::OnBnClickedBtnTest3()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	pFrame->m_pSockManager->m_fnRequestGrabReady(VISION_PC_1, SCAN_DIR_FORWARD, _T("RCP001"), _T("PANELID001"), _T("PANELID002"));
}

void CViewAuto::OnBnClickedBtnReset()
{
	//G_AddLog(3, 2, _T("Sequence Reset!!!"));

	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//
	//for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	//{
	//	pFrame->m_pThreadSeqPLC[i]->m_fnResetSEQ(TRUE, TRUE, TRUE);
	//}
}


void CViewAuto::OnBnClickedChkLogS1()
{
	UpdateData();
}


void CViewAuto::OnBnClickedChkLogS2()
{
	UpdateData();
}



void CViewAuto::OnBnClickedChkLogS3()
{
	UpdateData();
}
void CViewAuto::UpdateGridDetectSummaryText()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{
		if(pFrame == NULL)
		{
			return;
		}

		STRU_DEFECT_INFO *pTempDefectInfo;
		if(pFrame->m_DataManager.GetDefectCount())
		{
			int nlength = 0;
			TCHAR tszIndex[MAX_PATH] = {0,};
			TCHAR tszTemp[MAX_PATH]  = {0,};
		//	STRU_DEFECT_INFO *ptszNewDetectInfo;
			int i = 0;

			while(i < (int)pFrame->m_DataManager.GetDefectCount())
			{
				pTempDefectInfo = pFrame->m_DataManager.GetDefectInfo(i);

				if(pTempDefectInfo)
				{		
					//2016.02.01 Packing Defect
					if (pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
					{	
						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_NUM, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.layer_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_LAYER_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Defect_Code);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CODE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Flag);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_FLAG, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_Start_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_START_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_Start_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_START_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_End_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_END_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_End_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_END_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.2f"), pTempDefectInfo->Dinfo.Defect_Size);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_SIZE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_No);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Type);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_TYPE, tszIndex);
		
						nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Camera_No);
						tszTemp[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CAM_NO, tszTemp);

						/*		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_Path);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH, tszIndex);*/
					}
					else
					{
						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COL_NUM, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.InspTime);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_TIME, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Device);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_DEVICE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Cell_ID);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CELL_ID, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Pad);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PAD, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Judge);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_JUDGE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.P_Defect_Type);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_DEFECT_TYPE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_START_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_START_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_END_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_END_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Area);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_AREA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Area);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_AREA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_X_Max);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_X_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_X_Min);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_X_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Y_Max);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_Y_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Y_Min);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_Y_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Center_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CENTER_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Center_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CENTER_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Length);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_LENGTH, tszIndex);

						nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Breadth);
						tszTemp[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BREADTH, tszTemp);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Elongation);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_ELONGATION, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Compactness);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COMPACTNESS, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Holecount);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_HOLECOUNT, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Min_Angle);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MIN_ANGLE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Max_Angle);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MAX_ANGLE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Min);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Max);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Mean);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MEAN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_X);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Y);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Elongation);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_ELONGATION, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Max);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Mean);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MEAN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Min);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Sigma);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_SIGMA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Sum);
						tszIndex[nlength] = _T('\0');
						m_GridDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_SUM, tszIndex);

					}
				}
				i++;
			}
		}
		pTempDefectInfo = NULL;

	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryText Failed !! - Memory Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryText Failed !! - File Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryText Failed !! - Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
	}

}

void CViewAuto::m_fnPopUpDefectImage()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strPath;
	try
	{
		if(pFrame == NULL)
		{
			return;
		}

		if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
		{
			return;
		}

		STRU_DEFECT_INFO *pTempDefectInfo;

		int nTab = m_TabDefectSummary.GetCurSel();

		switch(nTab)
		{
		case 0:
			if(pFrame->m_DataManager.GetDefectCount())
			{
				pTempDefectInfo = pFrame->m_DataManager.GetDefectInfo(m_ngridDetectFileRow - 1);

				strPath = pTempDefectInfo->Dinfo.Defect_Img_Path;
			}

			break;
		case 1:
			if(pFrame->m_DataManager.GetReviewDefectCount())
			{
				pTempDefectInfo = pFrame->m_DataManager.GetReviewDefectInfo(m_ngridDetectFileRow - 1);

				strPath = pTempDefectInfo->Dinfo.Defect_Img_Path;

			}
			break;
		}
		
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

		//strPath = _T("D:\\QMS\\i_sent\\20150515\\51400\\PNL000000000000000\\155315\\KV04-004-1.jpg");
		
		//m_pDlgPopUpImage->MoveWindow(1260, 500, 648, 510);
		//m_pDlgPopUpImage->ShowWindow(SW_SHOW);		
		if(strPath.GetLength() > 0)
		{
			m_pDlgPopUpImage->OnDestroy();
			m_pDlgPopUpImage->SendMessage(WM_UI_CONTROL_POPUP_IMAGE, (WPARAM)0, (LPARAM)(LPCTSTR)strPath);
		}
		/*m_pDlgPopUpImage->SetWindowText(strPath.TrimRight());
		if (m_pDlgPopUpImage->LoadImage(strPath.TrimRight()))
		{
		m_pDlgPopUpImage->ShowImage();
		}	
		*/
		pTempDefectInfo = NULL;
	}
	catch(CException* e)
	{
		e->Delete();
	}
}

LRESULT CViewAuto::UpdateGridDetectSummaryRow(WPARAM wParam, LPARAM lParam) //(STRU_DEFECT_TYPE stDefType)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CString strPath = (LPCTSTR)lParam;
	try
	{	
		/*if (m_IdxPanelBtn  - 1 == PANEL_ONE)
		{*/
			pFrame->m_DataManager.m_fnClearDefectList();
			pFrame->m_DataManager.m_fnUpdateSelDefectList(strPath);
		//}
	/*	if (m_IdxPanelBtn - 1 == PANEL_TWO)
		{
			pFrame->m_DataManager.m_fnClearDefectList();
			pFrame->m_DataManager.m_fnUpdateSelDefectList(pFrame->m_DataManager.m_strPanelPath[PANEL_TWO]);
		}*/

		int nModifyRow = pFrame->m_DataManager.GetDefectCount();
		if(m_GridDefectSummary->GetSafeHwnd() == NULL)
		{
			return APP_NG;
		}
		if(m_GridDefectSummary->GetRowCount() > nModifyRow + 1)
		{
			while(m_GridDefectSummary->GetRowCount() > nModifyRow + 1)
			{
				m_GridDefectSummary->DeleteRow(m_GridDefectSummary->GetRowCount() - 1);
			}
			m_pDlgMap->m_nDefectIndex = m_GridDefectSummary->GetRowCount() - 1;
			m_GridDefectSummary->Invalidate();
		}
		else if(m_GridDefectSummary->GetRowCount() < nModifyRow + 1)
		{		
			UINT uiState = 0;
			while(m_GridDefectSummary->GetRowCount() < nModifyRow + 1)
			{
				m_GridDefectSummary->InsertRow(_T(""));
				m_GridDefectSummary->SetItemState(
					m_GridDefectSummary->GetRowCount()-1, 0,
					m_GridDefectSummary->GetItemState(m_GridDefectSummary->GetRowCount() - 1, 0)|GVIS_READONLY);			
				if((m_GridDefectSummary->GetRowCount() - 1)%2 == 1)
				{
					m_GridDefectSummary->SetItemBkColour(m_GridDefectSummary->GetRowCount() - 1, 0, RGB(220, 240, 220));
				}
				else
				{
					m_GridDefectSummary->SetItemBkColour(m_GridDefectSummary->GetRowCount() - 1, 0, RGB(255, 255, 255));
				}
			}
			m_GridDefectSummary->Invalidate();		
		}	
		m_GridDefectSummary->SetRedraw(FALSE);
	
		m_pDlgMap->Invalidate();

		UpdateGridDetectSummaryText();
		m_GridDefectSummary->SetRedraw(TRUE);

		return APP_OK;
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryRow Failed !! - Memory Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
		return APP_NG;
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryRow Failed !! - File Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
		return APP_NG;
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] UpdateGridDetectSummaryRow Failed !! - Exception "), pFrame->m_DataManager.GetPanelId());
		e->Delete();
		return APP_NG;
	}
}

void CViewAuto::OnGridClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = pItem->iColumn;

	if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
	{
		return;
	}
	//m_fnPopUpDefectImage();
	m_pDlgMap->m_nDefectIndex = m_ngridDetectFileRow;
	m_pDlgMap->Invalidate();
}

void CViewAuto::OnGridDBClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = pItem->iColumn;

	m_fnPopUpDefectImage();
}

void CViewAuto::OnGridClickReviewDetectList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = pItem->iColumn;

	if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
	{
		return;
	}
	//m_fnPopUpDefectImage();
	m_pDlgMap->m_nDefectIndex = m_ngridDetectFileRow;
	m_pDlgMap->Invalidate();
}

unsigned __stdcall CViewAuto::ThreadDetectFileSelect(void *pArg)
{
	CViewAuto *pView = (CViewAuto *)pArg;
	pView->LoadingDlgOpenDelay();
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame)
	{	
		pView->PostMessage(WM_UI_CONTROL_DEF_SUMMARY_LIST, (WPARAM)0, (LPARAM)0);
		//pView->UpdateGridDetectSummaryRow();
	}	

	pView->CloseLoadingDlg();
	if(pView->m_hThreadDetectFileSelect)
	{
		CloseHandle(pView->m_hThreadDetectFileSelect);
		pView->m_hThreadDetectFileSelect = NULL;
	}

	return 0;
}


void CViewAuto::OnBnClickedBtnPanel1()
{
	m_IdxPanelBtn = PANEL_ONE + 1;
	if(m_hThreadDetectFileSelect == NULL)
	{
		DeleteLoadingDlg();
		CreateLoadingDlg();

		UINT uiThreadID;
		m_hThreadDetectFileSelect = (HANDLE)::_beginthreadex(NULL, 0, ThreadDetectFileSelect, this, 0, &uiThreadID);

		OpenLoadingDlg(_T("LOADING..."));
	}

}


void CViewAuto::OnBnClickedBtnPanel2()
{
	m_IdxPanelBtn = PANEL_TWO + 1;
	if(m_hThreadDetectFileSelect == NULL)
	{
		DeleteLoadingDlg();
		CreateLoadingDlg();

		UINT uiThreadID;
		m_hThreadDetectFileSelect = (HANDLE)::_beginthreadex(NULL, 0, ThreadDetectFileSelect, this, 0, &uiThreadID);

		OpenLoadingDlg(_T("LOADING..."));
	}
}



void CViewAuto::CreateLoadingDlg()
{
	if(m_pLoadingDlg == NULL)
	{
		m_pLoadingDlg = new CLoadingDlg;
		
	}
}		

void CViewAuto::LoadingDlgOpenDelay()
{
	while(1)
	{
		if(m_pLoadingDlg)
		{
			if(m_pLoadingDlg->GetSafeHwnd())
			{
				break;
			}
		}
		Sleep(0);
	}
}	

void CViewAuto::OpenLoadingDlg(TCHAR *ptszText)
{
	if(m_pLoadingDlg)
	{	//m_pLoadingDlg->SetDlgItemTextW(0,ptszText);
		m_pLoadingDlg->DoModal();
	}
}	

void CViewAuto::CloseLoadingDlg()
{
	if(m_pLoadingDlg)
	{
		if(m_pLoadingDlg->GetSafeHwnd())
		{
			m_pLoadingDlg->CloseDialog();
		}
	}
}		


void CViewAuto::DeleteLoadingDlg()
{
	if(m_pLoadingDlg)
	{
		delete m_pLoadingDlg;
		m_pLoadingDlg = NULL;
	}
}		


void CViewAuto::OnBnClickedRadio(UINT ID)
{
	UpdateData(TRUE);
	switch(m_bDisChk)
	{
	case 0 :
		GetDlgItem(IDC_STATIC)->SetWindowText(_T("Defect Map"));
		m_pDlgMap->ShowWindow(SW_SHOW);
		/*m_pDlgMap->MoveWindow(23, 30, 540, 415);
		m_pDlgMap->m_fnSetPanelSize(301, 174);*/
		m_pDlgDefInfo->ShowWindow(SW_HIDE);
		((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);

		break;
	case 1 :
		GetDlgItem(IDC_STATIC)->SetWindowText(_T("Defect Info"));
		m_pDlgMap->ShowWindow(SW_HIDE);
		m_pDlgDefInfo->ShowWindow(SW_SHOW);
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
		break;
	}
	
}

void CViewAuto::m_fnSaveFilePanelInfo(CString strQmsFilePath)
{
	CStringArray strArrayData;
	wchar_t chRead[255] = {0}, lpszPath[255] = {0};	
	CString strResultFile, cPath, strLine, cPanelinfo, strPanelSummaryDataPath;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	//cPath =_T("D:\\Defect Info\\PanelData\\paneldata1.csv");
	//strResultFile.Format(_T("%s_%d.csv"), strPath_, nThreadID_);

	int nWriteTerm = 0, nDeleteTerm = 0;
	CStdioFile cstFile;
	CFileFind finder;

	try
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CTime tTime;
		CString cDataRoot, cCountRoot = _T("");
		CString cDataPath, strFileName;
		ULONG nDateCnt = 0, nDateCntTemp = 0, lDataUntilDay = 0;
		SYSTEMTIME	systime;
		BOOL bWorking;

		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);
		cDataRoot = chRead;
		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255);
		cDataPath = chRead;

		nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);
		
		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		strPanelSummaryDataPath = m_fnGetPanelSummaryDataPath();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		//cLogFileName = cDataRoot + cDataPath + _T("*.csv");	//2015.06.26 edit by sjw for File Path Change
		cLogFileName = strPanelSummaryDataPath + _T("*.csv");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
		//	return nErrorCode;
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)nDeleteTerm)
			{
				//cDeleteLogFileName = cDataRoot + cDataPath + FileData.cFileName;	//2015.06.26 edit by sjw for File Path Change
				cDeleteLogFileName = strPanelSummaryDataPath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)	
				{
					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		cCountRoot.Append(ROOT_PATH);
		cCountRoot.Append(CONFIG_COUNT_PATH);

		// 매 장당 결함 ini 파일에 결과를 카운팅 한다.
		//GetPrivateProfileString(PNL_COUNT_SET, KEY_SAVE_DEFECT_PATH, _T(""), (LPTSTR)chRead, sizeof((LPTSTR)chRead), cCountRoot);
		//GetPrivateProfileInt(lpszSection, lpKeyName, nDefault, m_szFileName);

		//pFrame->m_fnCreateFolder(cDataRoot + cDataPath);
		pFrame->m_fnCreateFolder(strPanelSummaryDataPath);
		
		::GetLocalTime(&systime);
		
		//bWorking = finder.FindFile(cDataRoot + cDataPath +_T("*.*"));	// 모든화일
		bWorking = finder.FindFile(strPanelSummaryDataPath +_T("*.*"));	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			
			strFileName = finder.GetFileName();

			//if(finder.IsDots()) continue;

			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				nDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + nWriteTerm; // + tTime.GetHour();

				if (nDateCnt < nDateCntTemp)
				{
					nDateCnt = nDateCntTemp;
					lDataUntilDay = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100;
				}	

				/*	if ( nDeleteTerm != 0 &&   abs((systime.wYear*10000 + systime.wMonth*100 + systime.wDay) - 
				(tTime.GetYear()*10000 + tTime.GetMonth()*100 + tTime.GetDay())) > nDeleteTerm)
				{
				DeleteFile(cDataRoot + cDataPath + strFileName);
				}*/
				
			}
			
		}    

		nDateCntTemp = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + systime.wHour;

		if (nDateCnt == 0)
		{
			nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;//systime.wHour;
		}
		else if (lDataUntilDay < systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100)
		{
			int ntDay = nDateCnt - nDateCntTemp;
			if ( abs(ntDay) >= 100)
			{
				nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;				
			}
			else
			{
				if (nWriteTerm < systime.wHour)
				{
					nDateCnt = nDateCntTemp;
				}
				if (nWriteTerm == 0)
				{
					nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;		
				}
			}
		}
		else
		{			
			if (nDateCnt < nDateCntTemp)
			{
				//int ntHour = nDateCnt - nDateCntTemp;
				if (nWriteTerm <= systime.wHour)
				{
					nDateCntTemp = (nDateCntTemp - systime.wHour) + nWriteTerm;
					nDateCnt = nDateCntTemp;
				}
				/*	int ntHour = nDateCnt - nDateCntTemp;
				if (nWriteTerm < abs(ntHour))
				{
				nDateCnt = nDateCntTemp;
				}*/
			}
		}
		
		finder.Close();
		
		//cPath.Format(_T("%s%spaneldata_%d.csv"), cDataRoot ,cDataPath ,nDateCnt);
		cPath.Format(_T("%spaneldata_%d.csv"), strPanelSummaryDataPath ,nDateCnt);

		if (!pFrame->m_DataManager.FileExist(cPath))
		{
			strLine = _T("PANEL ID					,\
						 INSPECT TIME				,\
						 TOTAL GRADE				,\
						 TOTAL DEFECT				,\
						 CF	GRADE					,\
						 CF DEFECT					,\
						 TFT GRADE					,\
						 TFT DEFECT					,\
						 POL GRADE					,\
						 POL DEFECT					,\
						 DUST						,\
						 WHITE TURBITY				,\
						 ITO						,\
						 EDGE BROKEN				,\
						 PAD SCRATCH				,\
						 DIMPLE POINT				,\
						 ETCHING LINE				,\
						 DIMPLE LINE				,\
						 PAD BURNT					,\
						 BURR						,\
						 POL BUBBLE					,\
						 POL SCRATCH				,\
						 POL ALIEN SUB				,\
						 POL MINOR					,\
						 BLACK SCRATCH				,\
						 WHITE SCRATCH				,\
						 CF MEASURE					,\
						 TFT MEASURE				,\
						 MURA				    	,\
						 CF BURR				    ,\
						 TFT BURR				    ,\
						 CF BROKEN				    ,\
						 TFT BROKEN				    ,\
						 CF CORNER BROKEN			,\
						 TFT CORNER BROKEN			,\
						 CF CELL BROKEN				,\
						 TFT CELL BROKEN			,\
						 CF PIMPLE POINT			,\
						 TFT PIMPLE POINT			,\
						 CF PIMPLE LINE				,\
						 TFT PIMPLE LINE			,\
						 CF PIMPLE CIRCLE			,\
						 CF WAVE PATTERN			,\
						 ETC POINT					,\
						 ETC LINE					,\
						 POL MISS					,\
						 QMS FILE PATH				,\
						 DEFECT CODE NUM			,\
						 EQ							,\
						 STAGE						,\
						 PA+						,\
						 MEA_FLAG					");//2021.04.13 add by mby 
		}

		if (!cstFile.Open(cPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
		{
			G_AddLog(2, 3, _T("m_fnSaveFileDefectInfo File Write Error %s : %d"), cPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();
		m_pDlgDefInfo->m_stDefectType.TotalDefType();
		//2020.04.13 add by mby S-
		CString PaCodeFlag = _T("0");	
		if(pFrame->m_DataManager.m_bDcc_Check1_flag){
			PaCodeFlag = _T("1");
		}else if(pFrame->m_DataManager.m_bDcc_Check2_flag){
			PaCodeFlag = _T("2");
		}
		//2020.04.13 add by mby S-
		//2021.04.20 add by mby S-
		CString MeaFailFlag = _T("_");	
		if(m_pDlgDefInfo->m_stDefectType.CfMeasureFail){
			MeaFailFlag = _T("CF_NG");
		}else if(m_pDlgDefInfo->m_stDefectType.TftMeasureFail){
			MeaFailFlag = _T("TFT_NG");
		}
		//2020.04.13 add by mby S-
		
		cPanelinfo.Format(_T("\n%s,%s,%s,%d,%s,%d,%s,%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%s,%s,%s,%s"),	//2015.07.13
			m_pDlgPanelJudge->GetResultPanelId(),  pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime , m_pDlgPanelJudge->GetResultGrade() ,m_pDlgDefInfo->m_stDefectType.TotalCnt,
			m_pDlgPanelJudge->GetResultCFJudge() , m_pDlgDefInfo->m_stDefectType.CfNgCnt ,m_pDlgPanelJudge->GetResultTftJudge(), m_pDlgDefInfo->m_stDefectType.TftNgCnt ,
			m_pDlgPanelJudge->GetResultPenetJudge(),   m_pDlgDefInfo->m_stDefectType.PenetNgCnt ,m_pDlgDefInfo->m_stDefectType.DustType, m_pDlgDefInfo->m_stDefectType.CfWhiteBlur + m_pDlgDefInfo->m_stDefectType.TftWhiteBlur, 
			m_pDlgDefInfo->m_stDefectType.ItoPeel, 	m_pDlgDefInfo->m_stDefectType.WanesBroken,	m_pDlgDefInfo->m_stDefectType.SocketScrach, m_pDlgDefInfo->m_stDefectType.CfDimplePoint + m_pDlgDefInfo->m_stDefectType.TftDimplePoint, 
			m_pDlgDefInfo->m_stDefectType.CfEdgeilegal + m_pDlgDefInfo->m_stDefectType.TftEdgeilegal,m_pDlgDefInfo->m_stDefectType.CfDimpleLine + m_pDlgDefInfo->m_stDefectType.TftDimpleLine,
			m_pDlgDefInfo->m_stDefectType.SocketBurn, m_pDlgDefInfo->m_stDefectType.Burr, m_pDlgDefInfo->m_stDefectType.CfPolBubble + m_pDlgDefInfo->m_stDefectType.TftPolBubble,
			// -- 21
			m_pDlgDefInfo->m_stDefectType.CfPolScrach + m_pDlgDefInfo->m_stDefectType.TftPolScrach, m_pDlgDefInfo->m_stDefectType.CFpolSubstance + m_pDlgDefInfo->m_stDefectType.TFTpolSubstance, 
			m_pDlgDefInfo->m_stDefectType.PolMinor, m_pDlgDefInfo->m_stDefectType.TftScrach + m_pDlgDefInfo->m_stDefectType.CfScrach, m_pDlgDefInfo->m_stDefectType.WhiteScrach,
			m_pDlgDefInfo->m_stDefectType.CfMeasureFail, m_pDlgDefInfo->m_stDefectType.TftMeasureFail, m_pDlgDefInfo->m_stDefectType.mura,
			m_pDlgDefInfo->m_stDefectType.CfBurr, m_pDlgDefInfo->m_stDefectType.TftBurr, m_pDlgDefInfo->m_stDefectType.CfBroken, m_pDlgDefInfo->m_stDefectType.TftBroken,	//2015.07.13
			m_pDlgDefInfo->m_stDefectType.CfCornerBroken, m_pDlgDefInfo->m_stDefectType.TftCornerBroken, m_pDlgDefInfo->m_stDefectType.CfCellBroken, m_pDlgDefInfo->m_stDefectType.TftCellBroken, 

			m_pDlgDefInfo->m_stDefectType.CfPimplePoint, m_pDlgDefInfo->m_stDefectType.TftPimplePoint, m_pDlgDefInfo->m_stDefectType.CfPimpleLine, m_pDlgDefInfo->m_stDefectType.TftPimpleLine, 
			m_pDlgDefInfo->m_stDefectType.CfPimpleCircle, m_pDlgDefInfo->m_stDefectType.CfWavePattern, m_pDlgDefInfo->m_stDefectType.EtcPoint, m_pDlgDefInfo->m_stDefectType.EtcLine, m_pDlgDefInfo->m_stDefectType.PolMiss, 
			
			strQmsFilePath,	//2015.08.26
			m_pDlgPanelJudge->GetResultDefCode(), 

			pFrame->m_DataManager.m_strPolPositionEQ_CV2,	//2019.06.20 by ryu
			pFrame->m_DataManager.m_strPolPositionStage_CV2,	//2019.06.20 by ryu
			PaCodeFlag,MeaFailFlag	//2021.04.20 add by mby
			); //2015.09.17 Mr.Chu	
		strLine.Append(cPanelinfo);
		/*for(int i = 0; i < strArrayData.GetCount(); i++)
		{
		strLine = strArrayData.GetAt(i);*/
		
		cstFile.WriteString(strLine);
	//	}

		cstFile.Close();
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	//fclose(out);
}
//2019.01.17 add by mby S-
void CViewAuto::m_fnSaveFilePanelStats()
{
	CStringArray strArrayData;
	wchar_t chRead[255] = {0}, lpszPath[255] = {0};	
	CString strResultFile, cPath, strLine, cPanelinfo, strPanelSummaryDataPath;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	int nWriteTerm = 0, nDeleteTerm = 0;
	CStdioFile cstFile;
	CFileFind finder;

	try
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CTime tTime;
		CString cDataRoot, cCountRoot = _T("");
		CString cDataPath, strFileName;
		ULONG nDateCnt = 0, nDateCntTemp = 0, lDataUntilDay = 0;
		SYSTEMTIME	systime;
		BOOL bWorking;

		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);
		cDataRoot = chRead;
		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255);
		cDataPath = chRead;

		nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);
		
		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		::GetLocalTime(&systime);
		strPanelSummaryDataPath.Format(_T("D:\\DEFECT INFO\\PANEL PD INFO\\%4d%02d%02d\\"),systime.wYear,systime.wMonth,systime.wDay);
		//strPanelSummaryDataPath = _T("D:\\DEFECT INFO\\PANEL PD INFO\\");
		//strPanelSummaryDataPath = m_fnGetPanelSummaryDataPath();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		cLogFileName = strPanelSummaryDataPath + _T("*.csv");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)nDeleteTerm)
			{
				cDeleteLogFileName = strPanelSummaryDataPath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)	
				{
					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		cCountRoot.Append(ROOT_PATH);
		cCountRoot.Append(CONFIG_COUNT_PATH);

		pFrame->m_fnCreateFolder(strPanelSummaryDataPath);
		
		::GetLocalTime(&systime);
		
		bWorking = finder.FindFile(strPanelSummaryDataPath +_T("*.*"));	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			
			strFileName = finder.GetFileName();

			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				nDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + nWriteTerm; // + tTime.GetHour();

				if (nDateCnt < nDateCntTemp)
				{
					nDateCnt = nDateCntTemp;
					lDataUntilDay = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100;
				}				
			}
			
		}    

		nDateCntTemp = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + systime.wHour;

		if (nDateCnt == 0)
		{
			nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;//systime.wHour;
		}
		else if (lDataUntilDay < systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100)
		{
			int ntDay = nDateCnt - nDateCntTemp;
			if ( abs(ntDay) >= 100)
			{
				nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;				
			}
			else
			{
				if (nWriteTerm < systime.wHour)
				{
					nDateCnt = nDateCntTemp;
				}
				if (nWriteTerm == 0)
				{
					nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;		
				}
			}
		}
		else
		{			
			if (nDateCnt < nDateCntTemp)
			{
				if (nWriteTerm <= systime.wHour)
				{
					nDateCntTemp = (nDateCntTemp - systime.wHour) + nWriteTerm;
					nDateCnt = nDateCntTemp;
				}
			}
		}
		
		finder.Close();		

		//cPath.Format(_T("%sPanelStats_%d.csv"), strPanelSummaryDataPath ,nDateCnt);
		cPath.Format(_T("%sPanelStats.csv"), strPanelSummaryDataPath);

		if (!pFrame->m_DataManager.FileExist(cPath))
		{
			strLine = _T("PANEL ID					,\
						 INSPECT TIME				,\
						 CF Broken A				,\
						 CF Broken B				,\
						 CF Broken C				,\
						 CF Broken D				,\
						 TFT Broken A				,\
						 TFT Broken B				,\
						 TFT Broken C				,\
						 TFT Broken D				,\
						 CF Corner A				,\
						 CF Corner B				,\
						 CF Corner C				,\
						 CF Corner D				,\
						 TFT Corner A				,\
						 TFT Corner B				,\
						 TFT Corner C				,\
						 TFT Corner D				,\
						 DEFECT CODE NUM			,\
						 TOTAL GRADE				");
		}

		if (!cstFile.Open(cPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
		{
			G_AddLog(2, 3, _T("m_fnSaveFileDefectInfo File Write Error %s : %d"), cPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();

		cPanelinfo.Format(_T("\n%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s,%s"),	//2019.01.17 add by mby
			m_pDlgPanelJudge->GetResultPanelId(),  pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime , 
			pFrame->m_DataManager.m_stDefType_UI.CfBrokenA,pFrame->m_DataManager.m_stDefType_UI.CfBrokenB,pFrame->m_DataManager.m_stDefType_UI.CfBrokenC,pFrame->m_DataManager.m_stDefType_UI.CfBrokenD,
			pFrame->m_DataManager.m_stDefType_UI.TftBrokenA,pFrame->m_DataManager.m_stDefType_UI.TftBrokenB,pFrame->m_DataManager.m_stDefType_UI.TftBrokenC,pFrame->m_DataManager.m_stDefType_UI.TftBrokenD,
		
			pFrame->m_DataManager.m_stDefType_UI.CfCornerBrokenA,pFrame->m_DataManager.m_stDefType_UI.CfCornerBrokenB,pFrame->m_DataManager.m_stDefType_UI.CfCornerBrokenC,pFrame->m_DataManager.m_stDefType_UI.CfCornerBrokenD,
			pFrame->m_DataManager.m_stDefType_UI.TftCornerBrokenA,pFrame->m_DataManager.m_stDefType_UI.TftCornerBrokenB,pFrame->m_DataManager.m_stDefType_UI.TftCornerBrokenC,pFrame->m_DataManager.m_stDefType_UI.TftCornerBrokenD,
				
			m_pDlgPanelJudge->GetResultDefCode(),m_pDlgPanelJudge->GetResultGrade());	

		strLine.Append(cPanelinfo);		
		cstFile.WriteString(strLine);


		cstFile.Close();
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CException* e)
	{
		e->Delete();
		cstFile.Close();
	}
}
//2019.01.17 add by mby E-
//2015.10.06
LRESULT CViewAuto::m_fnSaveFileMeasureInfo(WPARAM wParam, LPARAM lParam)//(CString strMeasureData, int nEQType)
{
	G_AddLog(3, 3, _T("m_fnSaveFileMeasureInfo() - Start"));
	CStringArray strArrayData;
	CString strMeasureData = (LPCTSTR)lParam;
	int nEQType = wParam;
	wchar_t chRead[255] = {0}, lpszPath[255] = {0};	
	CString strResultFile, cPath, strLine, cPanelinfo, strMeasureSummaryDataPath;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	int nWriteTerm = 0, nDeleteTerm = 0;
	CStdioFile cstFile;
	CFileFind finder;
	STRU_MEASURE_DATA stMeasureData;

	try
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CTime tTime;
		CString cDataRoot, cCountRoot = _T("");
		CString cDataPath, strFileName;
		ULONG nDateCnt = 0, nDateCntTemp = 0, lDataUntilDay = 0;
		SYSTEMTIME	systime;
		BOOL bWorking;

		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);
		cDataRoot = chRead;
		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255);
		cDataPath = chRead;

		nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);
		
		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		strMeasureSummaryDataPath = m_fnGetMeasureSummaryDataPath();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		cLogFileName = strMeasureSummaryDataPath + _T("*.csv");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime  - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)nDeleteTerm)
			{
				cDeleteLogFileName = strMeasureSummaryDataPath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)	
				{
					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);

		cCountRoot.Append(ROOT_PATH);
		cCountRoot.Append(CONFIG_COUNT_PATH);

		pFrame->m_fnCreateFolder(strMeasureSummaryDataPath);
		
		::GetLocalTime(&systime);
		
		bWorking = finder.FindFile(strMeasureSummaryDataPath +_T("*.*"));	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			strFileName = finder.GetFileName();

			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				nDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + nWriteTerm; // + tTime.GetHour();

				if (nDateCnt < nDateCntTemp)
				{
					nDateCnt = nDateCntTemp;
					lDataUntilDay = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100;
				}				
			}		
		}    

		nDateCntTemp = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + systime.wHour;

		if (nDateCnt == 0)
		{
			nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;//systime.wHour;
		}
		else if (lDataUntilDay < systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100)
		{
			int ntDay = nDateCnt - nDateCntTemp;
			if ( abs(ntDay) >= 100)
			{
				nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;				
			}
			else
			{
				if (nWriteTerm < systime.wHour)
				{
					nDateCnt = nDateCntTemp;
				}
				if (nWriteTerm == 0)
				{
					nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;		
				}
			}
		}
		else
		{			
			if (nDateCnt < nDateCntTemp)
			{
				if (nWriteTerm <= systime.wHour)
				{
					nDateCntTemp = (nDateCntTemp - systime.wHour) + nWriteTerm;
					nDateCnt = nDateCntTemp;
				}
			}
		}
		
		finder.Close();
		
		cPath.Format(_T("%sMeasuredata_%d.csv"), strMeasureSummaryDataPath ,nDateCnt);

		if (!pFrame->m_DataManager.FileExist(cPath))
		{
			if (nEQType == EQP_TYPE_ACC)
			{
				// 2018.03.12 LGU AX1...AY4 <-> EX1...EX4
				strLine = _T("PANEL ID					,\
							 INSPECT TIME				,\
							 EX1				        ,\
							 EY1				        ,\
							 EX2				        ,\
							 EY2				        ,\
							 EX3				        ,\
							 EY3				        ,\
							 EX4				        ,\
							 EY4				        ,\
							 AX1				        ,\
							 AY1				        ,\
							 AX2			        	,\
							 AY2				        ,\
							 AX3			        	,\
							 AY3			        	,\
							 AX4	        			,\
							 AY4	        			,\
							 DISTANCE L1				,\
							 DISTANCE L2				,\
							 DISTANCE W1				,\
							 DISTANCE W2				,\
							 ANGLE T1					,\
							 ANGLE T2					,\
							 ANGLE T3					,\
							 ANGLE T4					,\
							 DISTANCE L3				,\
							 DISTANCE L4				,\
							 DISTANCE W3	    		,\
							 DISTANCE W4				,\
							 ANGLE T5					,\
							 ANGLE T6					,\
							 ANGLE T7					,\
							 ANGLE T8					");
			}
			else if(nEQType == EQP_TYPE_ACS)
			{
				if(pFrame->m_fnEqpSizeType() != EQP_MONITOR_SIZE){
					strLine = _T("PANEL ID					,\
								 INSPECT TIME				,\
								 DISTANCE X1				,\
								 DISTANCE Y1				,\
								 DISTANCE X2				,\
								 DISTANCE Y2				,\
								 DISTANCE X3				,\
								 DISTANCE Y3				,\
								 DISTANCE E1				,\
								 DISTANCE E2				,\
								 DISTANCE X4				,\
								 DISTANCE Y4				,\
								 DISTANCE X5				,\
								 DISTANCE Y5				,\
								 DISTANCE X6				,\
								 DISTANCE Y6				,\
								 DISTANCE E3				,\
								 DISTANCE E4				,\
								 DISTANCE L1				,\
								 DISTANCE L2				,\
								 DISTANCE W1				,\
								 DISTANCE W2				,\
								 ANGLE T1					,\
								 ANGLE T2					,\
								 ANGLE T3					,\
								 ANGLE T4					,\
								 DISTANCE L3				,\
								 DISTANCE L4				,\
								 DISTANCE W3	    		,\
								 DISTANCE W4				,\
								 ANGLE T5					,\
								 ANGLE T6					,\
								 ANGLE T7					,\
								 ANGLE T8					");
				}else{				
					strLine = _T("PANEL ID					,\
								 INSP TIME					,\
								 DISTAN X1					,\
								 DISTAN Y1					,\
								 DISTAN X2					,\
								 DISTAN Y2					,\
								 DISTAN X3					,\
								 DISTAN Y3					,\
								 DISTAN X4					,\
								 DISTAN Y4					,\
								 DISTAN L1					,\
								 DISTAN L2					,\
								 DISTAN W1					,\
								 DISTAN W2					,\
								 ANGLE T1					,\
								 ANGLE T2					,\
								 ANGLE T3					,\
								 ANGLE T4					,\
								 DISTAN X5					,\
								 DISTAN Y5					,\
								 DISTAN X6					,\
								 DISTAN Y6					,\
								 DISTAN X7					,\
								 DISTAN Y7					,\
								 DISTAN X8					,\
								 DISTAN Y8					,\
								 DISTAN L3					,\
								 DISTAN L4					,\
								 DISTAN W3	    			,\
								 DISTAN W4					,\
								 ANGLE T5					,\
								 ANGLE T6					,\
								 ANGLE T7					,\
								 ANGLE T8					");
				}
			}
		}

		if (!cstFile.Open(cPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
		{
			G_AddLog(2, 3, _T("m_fnSaveFileMeasureInfo File Write Error %s : %d"), cPath, GetLastError());
			return APP_NG;
		}
		cstFile.SeekToEnd();

		stMeasureData = m_fnSetMeasureInfo(strMeasureData, nEQType);
		if(pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE && nEQType == EQP_TYPE_ACS){
			cPanelinfo.Format(_T("\n%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f,%0.2f"),	//2015.07.13
				m_pDlgPanelJudge->GetResultPanelId(),  pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime,
				stMeasureData.nDistance_X1, stMeasureData.nDistance_Y1, stMeasureData.nDistance_X2, stMeasureData.nDistance_Y2,
				stMeasureData.nDistance_X3, stMeasureData.nDistance_Y3, stMeasureData.nDistance_E1, stMeasureData.nDistance_E2,
				stMeasureData.nDistance_L1, stMeasureData.nDistance_L2, stMeasureData.nDistance_W1, stMeasureData.nDistance_W2,
				stMeasureData.dAngle_T1, stMeasureData.dAngle_T2, stMeasureData.dAngle_T3, stMeasureData.dAngle_T4,
				stMeasureData.nDistance_X4, stMeasureData.nDistance_Y4, stMeasureData.nDistance_X5, stMeasureData.nDistance_Y5,
				stMeasureData.nDistance_X6, stMeasureData.nDistance_Y6, stMeasureData.nDistance_E3, stMeasureData.nDistance_E4,				
				stMeasureData.nDistance_L3, stMeasureData.nDistance_L4, stMeasureData.nDistance_W3, stMeasureData.nDistance_W4,
				stMeasureData.dAngle_T5, stMeasureData.dAngle_T6, stMeasureData.dAngle_T7, stMeasureData.dAngle_T8);
		}else{
			cPanelinfo.Format(_T("\n%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f,%0.2f,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f,%0.2f"),	//2015.07.13
				m_pDlgPanelJudge->GetResultPanelId(),  pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime,
				stMeasureData.nDistance_X1, stMeasureData.nDistance_Y1, stMeasureData.nDistance_X2, stMeasureData.nDistance_Y2,
				stMeasureData.nDistance_X3, stMeasureData.nDistance_Y3, stMeasureData.nDistance_E1, stMeasureData.nDistance_E2,
				stMeasureData.nDistance_X4, stMeasureData.nDistance_Y4, stMeasureData.nDistance_X5, stMeasureData.nDistance_Y5,
				stMeasureData.nDistance_X6, stMeasureData.nDistance_Y6, stMeasureData.nDistance_E3, stMeasureData.nDistance_E4,
				stMeasureData.nDistance_L1, stMeasureData.nDistance_L2, stMeasureData.nDistance_W1, stMeasureData.nDistance_W2,
				stMeasureData.dAngle_T1, stMeasureData.dAngle_T2, stMeasureData.dAngle_T3, stMeasureData.dAngle_T4,
				stMeasureData.nDistance_L3, stMeasureData.nDistance_L4, stMeasureData.nDistance_W3, stMeasureData.nDistance_W4,
				stMeasureData.dAngle_T5, stMeasureData.dAngle_T6, stMeasureData.dAngle_T7, stMeasureData.dAngle_T8);
		}		

		strLine.Append(cPanelinfo);
		
		cstFile.WriteString(strLine);

		cstFile.Close();

		return APP_OK;
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		cstFile.Close();
		return APP_NG;
	}
	catch (CFileException* e)
	{
		e->Delete();
		cstFile.Close();
		return APP_NG;
	}
	catch (CException* e)
	{
		e->Delete();
		cstFile.Close();
		return APP_NG;
	}
	G_AddLog(3, 3, _T("m_fnSaveFileMeasureInfo() - End"));
}

STRU_MEASURE_DATA CViewAuto::m_fnSetMeasureInfo(CString strMeasureData, int nEQType)
{
	G_AddLog(3, 3, _T("m_fnSetMeasureInfo() - Start"));
	CString strVal = _T(""), check_data = _T("");
	int curPos = 0;	int nDefCnt = 0;
	STRU_MEASURE_DATA stTempMeasureData;
	check_data = strMeasureData;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	while (TRUE)
	{	
		if (nEQType == EQP_TYPE_ACC)
		{
			if (nDefCnt > 15) break;
		}
		else if(nEQType == EQP_TYPE_ACS)
		{
			if (nDefCnt > 31) break;
		}

		int ntokenCnt = pFrame->m_DataManager.m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt > ntokenCnt) break;	//2016.02.23 Mr.Chu		측정값 누락 조건문 변경 >=   ->  >

		strVal = check_data.Tokenize(EXT_COLON, curPos);
		
		if(pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE){
			switch (nDefCnt)
			{
			case 0:
				stTempMeasureData.nDistance_X1 = _tstoi((LPCTSTR)strVal);
				break;
			case 1:
				stTempMeasureData.nDistance_Y1 = _tstoi((LPCTSTR)strVal);
				break;
			case 2:
				stTempMeasureData.nDistance_X2 = _tstoi((LPCTSTR)strVal);
				break;
			case 3:
				stTempMeasureData.nDistance_Y2 = _tstoi((LPCTSTR)strVal);
				break;
			case 4:
				stTempMeasureData.nDistance_X3 = _tstoi((LPCTSTR)strVal);;
				break;
			case 5:
				stTempMeasureData.nDistance_Y3 = _tstoi((LPCTSTR)strVal);
				break;
			case 6:
				stTempMeasureData.nDistance_E1 = _tstoi((LPCTSTR)strVal);
				break;
			case 7:
				stTempMeasureData.nDistance_E2 = _tstoi((LPCTSTR)strVal);
				break;
			case 8:
				stTempMeasureData.nDistance_L1 = _tstoi((LPCTSTR)strVal);
				break;
			case 9:
				stTempMeasureData.nDistance_L2 = _tstoi((LPCTSTR)strVal);
				break;
			case 10:
				stTempMeasureData.nDistance_W1 = _tstoi((LPCTSTR)strVal);
				break;
			case 11:
				stTempMeasureData.nDistance_W2 = _tstoi((LPCTSTR)strVal);
				break;
			case 12:
				stTempMeasureData.dAngle_T1 = _tstof((LPCTSTR)strVal);
				break;
			case 13:
				stTempMeasureData.dAngle_T2 = _tstof((LPCTSTR)strVal);
				break;
			case 14:
				stTempMeasureData.dAngle_T3 = _tstof((LPCTSTR)strVal);
				break;
			case 15:
				stTempMeasureData.dAngle_T4 = _tstof((LPCTSTR)strVal);
				break;
			case 16:
				stTempMeasureData.nDistance_X4 = _tstoi((LPCTSTR)strVal);
				break;
			case 17:
				stTempMeasureData.nDistance_Y4 = _tstoi((LPCTSTR)strVal);
				break;
			case 18:
				stTempMeasureData.nDistance_X5 = _tstoi((LPCTSTR)strVal);
				break;
			case 19:
				stTempMeasureData.nDistance_Y5 = _tstoi((LPCTSTR)strVal);
				break;
			case 20:
				stTempMeasureData.nDistance_X6 = _tstoi((LPCTSTR)strVal);
				break;
			case 21:
				stTempMeasureData.nDistance_Y6 = _tstoi((LPCTSTR)strVal);;
				break;
			case 22:
				stTempMeasureData.nDistance_E3 = _tstoi((LPCTSTR)strVal);
				break;
			case 23:
				stTempMeasureData.nDistance_E4 = _tstoi((LPCTSTR)strVal);
				break;			
			case 24:
				stTempMeasureData.nDistance_L3 = _tstoi((LPCTSTR)strVal);
				break;
			case 25:
				stTempMeasureData.nDistance_L4 = _tstoi((LPCTSTR)strVal);
				break;
			case 26:
				stTempMeasureData.nDistance_W3 = _tstoi((LPCTSTR)strVal);
				break;
			case 27:
				stTempMeasureData.nDistance_W4 = _tstoi((LPCTSTR)strVal);
				break;
			case 28:
				stTempMeasureData.dAngle_T5 = _tstof((LPCTSTR)strVal);
				break;
			case 29:
				stTempMeasureData.dAngle_T6 = _tstof((LPCTSTR)strVal);
				break;
			case 30:
				stTempMeasureData.dAngle_T7 = _tstof((LPCTSTR)strVal);
				break;
			case 31:
				stTempMeasureData.dAngle_T8 = _tstof((LPCTSTR)strVal);
				break;
			default:
				break;
			}
			nDefCnt ++;	
		}else{
			switch (nDefCnt)
			{
			case 0:
				stTempMeasureData.nDistance_X1 = _tstoi((LPCTSTR)strVal);
				break;
			case 1:
				stTempMeasureData.nDistance_Y1 = _tstoi((LPCTSTR)strVal);
				break;
			case 2:
				stTempMeasureData.nDistance_X2 = _tstoi((LPCTSTR)strVal);
				break;
			case 3:
				stTempMeasureData.nDistance_Y2 = _tstoi((LPCTSTR)strVal);
				break;
			case 4:
				stTempMeasureData.nDistance_X3 = _tstoi((LPCTSTR)strVal);
				break;
			case 5:
				stTempMeasureData.nDistance_Y3 = _tstoi((LPCTSTR)strVal);
				break;
			case 6:
				stTempMeasureData.nDistance_E1 = _tstoi((LPCTSTR)strVal);
				break;
			case 7:
				stTempMeasureData.nDistance_E2 = _tstoi((LPCTSTR)strVal);
				break;
			case 8:
				stTempMeasureData.nDistance_X4 = _tstoi((LPCTSTR)strVal);
				break;
			case 9:
				stTempMeasureData.nDistance_Y4 = _tstoi((LPCTSTR)strVal);
				break;
			case 10:
				stTempMeasureData.nDistance_X5 = _tstoi((LPCTSTR)strVal);
				break;
			case 11:
				stTempMeasureData.nDistance_Y5 = _tstoi((LPCTSTR)strVal);
				break;
			case 12:
				stTempMeasureData.nDistance_X6 = _tstoi((LPCTSTR)strVal);
				break;
			case 13:
				stTempMeasureData.nDistance_Y6 = _tstoi((LPCTSTR)strVal);;
				break;
			case 14:
				stTempMeasureData.nDistance_E3 = _tstoi((LPCTSTR)strVal);
				break;
			case 15:
				stTempMeasureData.nDistance_E4 = _tstoi((LPCTSTR)strVal);
				break;
			case 16:
				stTempMeasureData.nDistance_L1 = _tstoi((LPCTSTR)strVal);
				break;
			case 17:
				stTempMeasureData.nDistance_L2 = _tstoi((LPCTSTR)strVal);
				break;
			case 18:
				stTempMeasureData.nDistance_W1 = _tstoi((LPCTSTR)strVal);
				break;
			case 19:
				stTempMeasureData.nDistance_W2 = _tstoi((LPCTSTR)strVal);
				break;
			case 20:
				stTempMeasureData.dAngle_T1 = _tstof((LPCTSTR)strVal);
				break;
			case 21:
				stTempMeasureData.dAngle_T2 = _tstof((LPCTSTR)strVal);
				break;
			case 22:
				stTempMeasureData.dAngle_T3 = _tstof((LPCTSTR)strVal);
				break;
			case 23:
				stTempMeasureData.dAngle_T4 = _tstof((LPCTSTR)strVal);
				break;
			case 24:
				stTempMeasureData.nDistance_L3 = _tstoi((LPCTSTR)strVal);
				break;
			case 25:
				stTempMeasureData.nDistance_L4 = _tstoi((LPCTSTR)strVal);
				break;
			case 26:
				stTempMeasureData.nDistance_W3 = _tstoi((LPCTSTR)strVal);
				break;
			case 27:
				stTempMeasureData.nDistance_W4 = _tstoi((LPCTSTR)strVal);
				break;
			case 28:
				stTempMeasureData.dAngle_T5 = _tstof((LPCTSTR)strVal);
				break;
			case 29:
				stTempMeasureData.dAngle_T6 = _tstof((LPCTSTR)strVal);
				break;
			case 30:
				stTempMeasureData.dAngle_T7 = _tstof((LPCTSTR)strVal);
				break;
			case 31:
				stTempMeasureData.dAngle_T8 = _tstof((LPCTSTR)strVal);
				break;
			default:
				break;
			}
			nDefCnt ++;		
		}
				
	};
	G_AddLog(3, 3, _T("m_fnSetMeasureInfo() - Start"));
	return stTempMeasureData;
}


//2015.10.07
void CViewAuto::m_fnSaveFileOverlapDefectInfo(CString strPanelID, CString strOverlapDefectData, int nOverlapCnt)
{
	G_AddLog(3, 3, _T("m_fnSaveFileOverlapDefectInfo() - Start"));
	CStringArray strArrayData;
	wchar_t chRead[255] = {0}, lpszPath[255] = {0};	
	CString strResultFile, cPath, strLine, cPanelinfo, strOverlapDefectSummaryDataPath, strTemp, strVal = _T(""), check_data = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	int nWriteTerm = 0, nDeleteTerm = 0, nCurPos = 0, nDefCnt = 0;
	double dStartX, dStartY, dEndX, dEndY;
	CString strType;
	CStdioFile cstFile;
	CFileFind finder;
	strTemp = strOverlapDefectData;

	try
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CTime tTime;
		CString cDataRoot, cCountRoot = _T("");
		CString cDataPath, strFileName;
		ULONG nDateCnt = 0, nDateCntTemp = 0, lDataUntilDay = 0;
		SYSTEMTIME	systime;
		BOOL bWorking;

		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);
		cDataRoot = chRead;
		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255);
		cDataPath = chRead;

		nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);
		
		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		strOverlapDefectSummaryDataPath = m_fnGetOverlapDefectSummaryDataPath();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		cLogFileName = strOverlapDefectSummaryDataPath + _T("*.csv");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)nDeleteTerm)
			{
				cDeleteLogFileName = strOverlapDefectSummaryDataPath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)	
				{
					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);

		cCountRoot.Append(ROOT_PATH);
		cCountRoot.Append(CONFIG_COUNT_PATH);

		pFrame->m_fnCreateFolder(strOverlapDefectSummaryDataPath);
		
		::GetLocalTime(&systime);
		
		bWorking = finder.FindFile(strOverlapDefectSummaryDataPath +_T("*.*"));	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			strFileName = finder.GetFileName();

			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				nDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + nWriteTerm; // + tTime.GetHour();

				if (nDateCnt < nDateCntTemp)
				{
					nDateCnt = nDateCntTemp;
					lDataUntilDay = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100;
				}				
			}		
		}    

		nDateCntTemp = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + systime.wHour;

		if (nDateCnt == 0)
		{
			nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;//systime.wHour;
		}
		else if (lDataUntilDay < systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100)
		{
			int ntDay = nDateCnt - nDateCntTemp;
			if ( abs(ntDay) >= 100)
			{
				nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;				
			}
			else
			{
				if (nWriteTerm < systime.wHour)
				{
					nDateCnt = nDateCntTemp;
				}
				if (nWriteTerm == 0)
				{
					nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;		
				}
			}
		}
		else
		{			
			if (nDateCnt < nDateCntTemp)
			{
				if (nWriteTerm <= systime.wHour)
				{
					nDateCntTemp = (nDateCntTemp - systime.wHour) + nWriteTerm;
					nDateCnt = nDateCntTemp;
				}
			}
		}
		
		finder.Close();
		
		cPath.Format(_T("%sOverlapDefectdata_%d.csv"), strOverlapDefectSummaryDataPath ,nDateCnt);

		if (!pFrame->m_DataManager.FileExist(cPath))
		{
			strLine = _T("PANEL ID					,\
						 INSPECT TIME				,\
						 START X					,\
						 START Y					,\
						 END X						,\
						 END Y						,\
						 TYPE						");
		}

		if (!cstFile.Open(cPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
		{
			G_AddLog(2, 3, _T("m_fnSaveFileOverlapDefectInfo File Write Error %s : %d"), cPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();

		nCurPos = 0;
		CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
		while (resToken != "")
		{
			strArrayData.Add(resToken);
			resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
		}

		nCurPos = 0;
		for(int i = 0; i < nOverlapCnt; i++)
		{
			CString strMeasureLine = strArrayData.GetAt(i);
			check_data = strMeasureLine;
			cPanelinfo = _T("");
			while (TRUE)
			{	
				if (nDefCnt > 4) break;

				strVal = check_data.Tokenize(EXT_COLON, nCurPos);

				switch (nDefCnt)
				{
				case 0:
					dStartX = _tstof((LPCTSTR)strVal);
					break;
				case 1:
					dStartY = _tstof((LPCTSTR)strVal);
					break;
				case 2:
					dEndX = _tstof((LPCTSTR)strVal);
					break;
				case 3:
					dEndY = _tstof((LPCTSTR)strVal);
					break;
				case 4:
					strType = strVal;
					break;
				default:
					break;
				}
				nDefCnt ++;				
			};
			cPanelinfo.Format(_T("\n%s,%s,%0.3f,%0.3f,%0.3f,%0.3f,%s"),
				strPanelID,  pFrame->m_DataManager.m_strInspDate + _T(" " ) + pFrame->m_DataManager.m_strInspTime,
				dStartX, dStartY, dEndX, dEndY, strType);

			strLine.Append(cPanelinfo);
			nDefCnt = 0;
			nCurPos = 0;
		}
		
		cstFile.WriteString(strLine);

		cstFile.Close();
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	G_AddLog(3, 3, _T("m_fnSaveFileOverlapDefectInfo() - End"));
}

void CViewAuto::OnBnClickedBtnCntReset()
{
	CString cCountRoot= _T("");
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);

	WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, _T("0"), cCountRoot);
	WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, _T("0"), cCountRoot);

	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->SetCaption(_T("0"));	
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->SetCaption(_T("0"));	
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_TOTALCELL))->SetCaption(_T("0"));
}

//2015.06.22
void CViewAuto::OnGridClickPanelList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	CString strPanelID, strDefectFile, strDefectSummaryDataPath;
	CString strDate, strYear, strMonth, strDay, strTemp, strCode;
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	int nRow = pItem->iRow;
	int nCol = pItem->iColumn;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();


	if(nRow < 1 || nCol < 0)
	{
		return;
	}

	strPanelID = m_GridPanelSummary->GetItemText(nRow, 1);
	m_pDlgMap->m_stDefMapData.strCurPanelID = strPanelID;

	strDate = m_GridPanelSummary->GetItemText(nRow, 0);
	m_pDlgMap->m_stDefMapData.strCurInspDate = strDate;
	strDate = strDate.Left(strDate.Find(_T(" ")));

	strCode = m_GridPanelSummary->GetItemText(nRow,3); //16.05.02 sgkim

	m_pDlgDefInfo->m_fnChangeFont(strCode);

	int i = 0;
	while(AfxExtractSubString(strTemp, strDate, i, '/'))
	{
		if(i == 0)		strYear = strTemp;
		else if(i == 1)	strMonth = strTemp;		
		else if(i == 2)	strDay  = strTemp;		
		i++;
	}
	strDefectSummaryDataPath = m_fnGetDefectSummaryDataPath();
	strDefectFile.Format(_T("%s%s\\%s\\%s\\%s.txt"), strDefectSummaryDataPath, strYear, strMonth, strDay, strPanelID);

	if (pFrame->m_DataManager.FileExist(strDefectFile))
	{
		UpdateGridReviewDefectSummaryRow(strDefectFile);

		m_TabDefectSummary.SetCurFocus(1);
	}

}

CString CViewAuto::m_fnGetDefectSummaryDataPath()
{
	wchar_t chRead[255] = {0};
	CString strDataRoot, strDataPath, strDataDefectPath;
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataRoot = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPath = chRead;
	memset(chRead,0X00,sizeof(chRead));
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_DEFECT_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataDefectPath = chRead;

	return strDataRoot + strDataPath + strDataDefectPath;

}

CString CViewAuto::m_fnGetPanelSummaryDataPath()
{
	wchar_t chRead[255] = {0};
	CString strDataRoot, strDataPath, strDataPanelPath;
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataRoot = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPath = chRead;
	memset(chRead,0X00,sizeof(chRead));
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PANEL_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPanelPath = chRead;

	return strDataRoot + strDataPath + strDataPanelPath;

}

//2015.10.06
CString CViewAuto::m_fnGetMeasureSummaryDataPath()
{
	wchar_t chRead[255] = {0};
	CString strDataRoot, strDataPath, strDataMeasurePath;
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataRoot = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPath = chRead;
	memset(chRead,0X00,sizeof(chRead));
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_MEASURE_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataMeasurePath = chRead;

	return strDataRoot + strDataPath + strDataMeasurePath;

}

//2015.10.07
CString CViewAuto::m_fnGetOverlapDefectSummaryDataPath()
{
	wchar_t chRead[255] = {0};
	CString strDataRoot, strDataPath, strDataOverlapDefectPath;
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataRoot = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPath = chRead;
	memset(chRead,0X00,sizeof(chRead));
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_OVERLAP_DEFECT_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataOverlapDefectPath = chRead;

	return strDataRoot + strDataPath + strDataOverlapDefectPath;

}

//2015.10.08
CString CViewAuto::m_fnGetQmsBackUpDataPath()
{
	wchar_t chRead[255] = {0};
	CString strDataRoot, strDataPath, strDataQmsBackUpPath;
	CString strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataRoot = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataPath = chRead;
	memset(chRead,0X00,sizeof(chRead));
	GetPrivateProfileString(PNL_DATA_PATH, KEY_DEFINFO_QMS_BACKUP_PATH, _T(""), (LPTSTR)chRead, 255, strConfigFile);
	strDataQmsBackUpPath = chRead;

	return strDataRoot + strDataPath + strDataQmsBackUpPath;

}

void CViewAuto::m_fnLoadPanelSummary()
{
	CString strPanelSummaryDataPath, strTempFileName, strFileName, strFilePath, strLine, strItem;
	CStringArray strArrayData;
	ULONG lDateCnt = 0, lDateCntTemp = 0;
	BOOL bWorking;
	CStdioFile cstFile;
	CFileFind finder;
	CTime tTime;

	strPanelSummaryDataPath = m_fnGetPanelSummaryDataPath();
	bWorking = finder.FindFile(strPanelSummaryDataPath +_T("*.*"));

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();

		if(finder.IsDots()) continue;
		
		strTempFileName = finder.GetFileName();

		if (strTempFileName != _T(".") )
		{
			finder.GetLastWriteTime(tTime); 
			lDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + tTime.GetHour();

			if (lDateCnt < lDateCntTemp)
			{
				lDateCnt = lDateCntTemp;
				strFileName = strTempFileName;
			}				
		}
			
	}    
	finder.Close();

	strFilePath.Format(_T("%s%s"), strPanelSummaryDataPath ,strFileName);

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnLoadPanelSummary File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}
	while(cstFile.ReadString(strLine))
	{
		if(strLine == _T(""))	continue;

		strItem = strLine;
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem != _T("PANEL ID"))
		{
			strArrayData.Add(strLine);
		}

	}
	cstFile.Close();

	int nPanelTotalCnt = strArrayData.GetCount();
	int nPanelViewStart = 0;

	if (nPanelTotalCnt >= MAX_PANEL_LIST)
		nPanelViewStart = nPanelTotalCnt - MAX_PANEL_LIST;
	else
		nPanelViewStart = 0;
	
	int nCnt = 0;
	for(int i = nPanelTotalCnt - 1; i >= nPanelViewStart; i--)
	{
		strLine = strArrayData.GetAt(i);
		m_fnViewPanelSummary(strLine);
	}

}

void CViewAuto::m_fnViewPanelSummary(CString strLine)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}
	
	int nlength = 0, nGradeCnt = 0, nGradeNG = 0, nGradeOK = 0;
	CString cCountVal = _T(""), cCountRoot = _T("") ;
	int i = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	
	CString strJudgeCode = _T(""), strDefCode = _T(""); 
	int nRowCount = m_GridPanelSummary->GetRowCount();
	int nColCount = m_GridPanelSummary->GetColumnCount();

	if (nRowCount > MAX_PANEL_LIST)
	{
		m_GridPanelSummary->DeleteRow(1);
	}
	m_GridPanelSummary->InsertRow(_T(""));

	nRowCount = m_GridPanelSummary->GetRowCount() - 1;

	m_GridPanelSummary->SetRedraw(FALSE);
	m_GridPanelSummary->SetItemState(m_GridPanelSummary->GetRowCount()-1, 0,m_GridPanelSummary->GetItemState(m_GridPanelSummary->GetRowCount() - 1, 0)|GVIS_READONLY);
	
	int nCurPos = 0;
	CString resToken= strLine.Tokenize(EXT_COLON, nCurPos);
	while (resToken != _T(""))
	{
		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_PNLID, tszIndex);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_TIME, tszIndex);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		strJudgeCode = tszIndex;
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_GRADE, tszIndex);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_CF, tszIndex);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_TFT, tszIndex);
		resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		//2015.09.11 Mr.Chu
		for(int i=0;i< MAX_DEFECT_KIND +3 ; i++)  //38   +6 ->  +3 
		{
			if(nCurPos == -1)
				break;	
			else
				strDefCode = resToken;

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		}
			
		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), strDefCode);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_DEFECT, tszIndex);
		//2018.04.18 YHS
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), m_fnSetDefectName(strDefCode));
			tszIndex[nlength] = _T('\0');
 			m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_DEFECT_NAME, tszIndex);

		//2019.06.20 by ryu
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_EQ, tszIndex);  // _T("") //2019.05.06 by ryu	

		if(nCurPos != -1)resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_STAGE, tszIndex);  // _T("") //2019.05.06 by ryu	
		//2020.04.13 add by mby S-
		if(nCurPos != -1) resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');
		m_GridPanelSummary->SetItemText(nRowCount, PANEL_SUMMARY_PENETRATE, tszIndex);  // _T("")
		//2020.04.13 add by mby E-
		pFrame->m_stParamInfo.nEqpObject = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_OBJECT, 1);
		for (int i = 0; i < m_GridPanelSummary->GetColumnCount(); i ++)
		{
			if (strJudgeCode != _T("A") && pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
			{
				m_GridPanelSummary->SetItemBkColour(nRowCount, i , RGB(255, 66, 0));
				m_GridPanelSummary->SetItemFgColour(nRowCount, i , RGB(255, 255, 255));
			}
		}
		
		break;
	}

	m_GridPanelSummary->SetRedraw(TRUE);	
	
	m_GridPanelSummary->Refresh();
}


void CViewAuto::m_fnInitGridReviewDefectSummary(int nEqpObject)
{
	CRect rect;
	GetDlgItem(IDC_GRID_REVIEW_SUMMARY)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridReviewDefectSummary = new CGridCtrl();
	m_GridReviewDefectSummary->Create(rect, this, IDC_GRID_REVIEW_SUMMARY);
	m_GridReviewDefectSummary->SetParent(this);
	m_GridReviewDefectSummary->SetListMode(true);
	m_GridReviewDefectSummary->SetEditable(FALSE);
	m_GridReviewDefectSummary->SetFixedBkColor(RGB(220, 240, 220));
	m_GridReviewDefectSummary->SetBkColor(RGB(200, 200, 200));
	m_GridReviewDefectSummary->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridReviewDefectSummary->SetFixedRowCount(1);

	if (nEqpObject != P_PROJECT)
	{
		//nColumnCount = MAX_DEFECT_SUMMARY_COL;

		TCHAR colNameInspSum[MAX_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("NUM"),  _T("LAYER_NO"),  _T("DEFECT_NO"),  _T("DEFECT_CODE"),  _T("FLAG"),  _T("PIXEL_X"), 
		_T("PIXEL_Y"),  _T("GATE_START_NO"),  _T("DATA_START_NO"),  _T("GATE_END_NO"),  _T("DATA_END_NO"), 
		_T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"),  _T("DEF_SIZE"), 
		_T("IMG_NO"),  _T("IMG_SIZE_X"),  _T("IMG_SIZE_Y"),  _T("TYPE"),  _T("CAM_NO")  };

		m_GridReviewDefectSummary->SetColumnCount(MAX_DEFECT_SUMMARY_COL);

		for(int k = 0; k < MAX_DEFECT_SUMMARY_COL; k++)
		{
			if(k == 0 || k == 1)
				m_GridReviewDefectSummary->SetColumnWidth(k, 85);
			else if(k == 3)
				m_GridReviewDefectSummary->SetColumnWidth(k, 110);
			else
				m_GridReviewDefectSummary->SetColumnWidth(k, 70);

			m_GridReviewDefectSummary->SetItemText(0, k, colNameInspSum[k]);
		}
	}
	else
	{
		//nColumnCount = MAX_P_DEFECT_SUMMARY_COL;
		
		TCHAR colNameInspSum[MAX_P_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("NUM"),  _T("TIME"),  _T("DEVICE"),  _T("CELL_ID"),  _T("PAD"),  _T("JUDGE"), 
		 _T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"), _T("DEFECT_TYPE"),
		_T("AREA"),  _T("BOX_AREA"),  _T("BOX_X_MAX"),  _T("BOX_X_MIN"), _T("BOX_Y_MAX"),  _T("BOX_Y_MIN"),  
		_T("CENTER_X"),  _T("CENTER_Y"),  _T("LENGTH"), _T("BREADTH"),  _T("ELONGATION"),  _T("COMPACTNESS"),  _T("HOLECOUNT"),  
		_T("FERET_MIN_ANGLE"), _T("FERET_MAX_ANGLE"),  _T("FERET_MIN"),  _T("FERET_MAX"),  _T("FERET_MEAN"),  _T("FERET_X"), _T("FERET_Y"),  
		_T("FERET_ELONGATION"),  _T("PIXEL_MAX"),  _T("PIXEL_MEAN"),  _T("PIXEL_MIN"),   _T("PIXEL_SIGMA"),  _T("PIXEL_SUM") };

		m_GridReviewDefectSummary->SetColumnCount(MAX_P_DEFECT_SUMMARY_COL);

		for(int k = 0; k < MAX_P_DEFECT_SUMMARY_COL; k++)
		{
			if(k == 0 || k == 1)
				m_GridReviewDefectSummary->SetColumnWidth(k, 85);
			else if(k == 3)
				m_GridReviewDefectSummary->SetColumnWidth(k, 110);
			else
				m_GridReviewDefectSummary->SetColumnWidth(k, 70);

			m_GridReviewDefectSummary->SetItemText(0, k, colNameInspSum[k]);
		}
	}
}

void CViewAuto::UpdateGridReviewDefectSummaryRow(CString strPath)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{	
		pFrame->m_DataManager.m_fnClearReviewDefectList();
		pFrame->m_DataManager.m_fnUpdateSelReviewDefectList(strPath);

		int nModifyRow = pFrame->m_DataManager.GetReviewDefectCount();
		if(m_GridReviewDefectSummary->GetSafeHwnd() == NULL)
		{
			return;
		}
		if(m_GridReviewDefectSummary->GetRowCount() > nModifyRow + 1)
		{
			while(m_GridReviewDefectSummary->GetRowCount() > nModifyRow + 1)
			{
				m_GridReviewDefectSummary->DeleteRow(m_GridReviewDefectSummary->GetRowCount() - 1);
			}
			m_pDlgMap->m_nDefectIndex = m_GridReviewDefectSummary->GetRowCount() - 1;	//2015.07.06
			m_GridReviewDefectSummary->Invalidate();
		}
		else if(m_GridReviewDefectSummary->GetRowCount() < nModifyRow + 1)
		{		
			UINT uiState = 0;
			while(m_GridReviewDefectSummary->GetRowCount() < nModifyRow + 1)
			{
				m_GridReviewDefectSummary->InsertRow(_T(""));
				m_GridReviewDefectSummary->SetItemState(
					m_GridReviewDefectSummary->GetRowCount()-1, 0,
					m_GridReviewDefectSummary->GetItemState(m_GridReviewDefectSummary->GetRowCount() - 1, 0)|GVIS_READONLY);			
				if((m_GridReviewDefectSummary->GetRowCount() - 1)%2 == 1)
				{
					m_GridReviewDefectSummary->SetItemBkColour(m_GridReviewDefectSummary->GetRowCount() - 1, 0, RGB(220, 240, 220));
				}
				else
				{
					m_GridReviewDefectSummary->SetItemBkColour(m_GridReviewDefectSummary->GetRowCount() - 1, 0, RGB(255, 255, 255));
				}
			}
			m_GridReviewDefectSummary->Invalidate();		
		}


		m_GridReviewDefectSummary->SetRedraw(FALSE);

		m_pDlgMap->Invalidate();	//2015.07.06

		UpdateGridReviewDefectSummaryText();
		m_GridReviewDefectSummary->SetRedraw(TRUE);
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryRow Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryRow Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryRow Failed !! - Exception "));
		e->Delete();
	}
}

void CViewAuto::UpdateGridReviewDefectSummaryText()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{
		if(pFrame == NULL)
		{
			return;
		}

		STRU_DEFECT_INFO *pTempDefectInfo;
		if(pFrame->m_DataManager.GetReviewDefectCount())
		{
			int nlength = 0;
			TCHAR tszIndex[MAX_PATH] = {0,};
			TCHAR tszTemp[MAX_PATH]  = {0,};
			//	STRU_DEFECT_INFO *ptszNewDetectInfo;
			int i = 0;
			while(i < (int)pFrame->m_DataManager.GetReviewDefectCount())
			{
				pTempDefectInfo = pFrame->m_DataManager.GetReviewDefectInfo(i);

				if(pTempDefectInfo)
				{	
					//2016.02.01 Packing Defect
					if (pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
					{			
						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_NUM, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.layer_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_LAYER_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Defect_Code);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CODE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Flag);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_FLAG, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_Start_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_START_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_Start_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_START_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_End_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_END_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_End_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_END_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.2f"), pTempDefectInfo->Dinfo.Defect_Size);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_SIZE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_No);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_NO, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Type);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_TYPE, tszIndex);

						nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Camera_No);
						tszTemp[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CAM_NO, tszTemp);

						/*		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_Path);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH, tszIndex);*/
					}
					else
					{
						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COL_NUM, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.InspTime);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_TIME, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Device);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_DEVICE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Cell_ID);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CELL_ID, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Pad);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PAD, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Judge);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_JUDGE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.P_Defect_Type);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_DEFECT_TYPE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_START_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_START_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_END_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COORD_END_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Area);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_AREA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Area);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_AREA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_X_Max);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_X_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_X_Min);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_X_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Y_Max);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_Y_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Box_Y_Min);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BOX_Y_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Center_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CENTER_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Center_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_CENTER_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Length);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_LENGTH, tszIndex);

						nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Breadth);
						tszTemp[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_BREADTH, tszTemp);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Elongation);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_ELONGATION, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Compactness);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_COMPACTNESS, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Holecount);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_HOLECOUNT, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Min_Angle);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MIN_ANGLE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Max_Angle);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MAX_ANGLE, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Min);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Max);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Mean);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_MEAN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_X);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_X, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Y);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_Y, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Feret_Elongation);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_FERET_ELONGATION, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Max);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MAX, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Mean);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MEAN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Min);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_MIN, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Sigma);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_SIGMA, tszIndex);

						nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Pixel_Sum);
						tszIndex[nlength] = _T('\0');
						m_GridReviewDefectSummary->SetItemText(i+1, P_DEFECT_SUMMARY_PIXEL_SUM, tszIndex);

					}
				}
				i++;
			}
		}

		pTempDefectInfo = NULL;
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryText Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryText Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridReviewDefectSummaryText Failed !! - Exception "));
		e->Delete();
	}

}

void CViewAuto::OnTcnSelchangeTabDefectSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nTab = m_TabDefectSummary.GetCurSel();

	switch(nTab)
	{
	case 0:
		((CGxStaticEx*)GetDlgItem(IDC_GRID_SUMMARY))->ShowWindow(SW_SHOW);
		m_nViewDefectSummary = DEFECT_CURRENT;	//2015.07.06
		break;
	case 1:
		((CGxStaticEx*)GetDlgItem(IDC_GRID_REVIEW_SUMMARY))->ShowWindow(SW_SHOW);
		m_nViewDefectSummary = DEFECT_REVIEW;	//2015.07.06
		break;
	}
	m_pDlgMap->m_nDefectIndex = -1; 
	m_pDlgMap->Invalidate();

	*pResult = 0;
}


void CViewAuto::OnTcnSelchangingTabDefectSummary(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nTab = m_TabDefectSummary.GetCurSel();

	switch(nTab)
	{
	case 0:
		((CGxStaticEx*)GetDlgItem(IDC_GRID_SUMMARY))->ShowWindow(SW_HIDE);
		break;
	case 1:
		((CGxStaticEx*)GetDlgItem(IDC_GRID_REVIEW_SUMMARY))->ShowWindow(SW_HIDE);
		break;
	}

	*pResult = 0;
}

//2015.07.03 Chu
//기능 : Tab control Log Disply Controllor
void CViewAuto::OnTcnSelchangeTabDefectLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int select = m_ctrlLogTab.GetCurSel();

	switch( select )
	{
	case 0:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 1:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 2:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 3:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 4:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 5:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 6:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 7:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 8:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_HIDE);
		break;
	case 9:
		GetDlgItem(IDC_LIST_LOG1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG3)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_LOG10)->ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}

//2015.07.03 Chu
//기능 : Stage 별로 로그 표시 및 저장 여부 체크
void CViewAuto::m_fnCheakLogFunc(int nStageID)
{
	int select = m_ctrlLogTab.GetCurSel();

	for(int i=0;i<9;i++)
		m_bLog_Sel[i] = FALSE;

	switch( select )
	{
	case 0:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 1:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 2:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 3:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 4:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 5:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 6:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 7:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 8:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	case 9:
		if(nStageID == 0)
			m_bLog_Sel[0] = TRUE;
		else if(nStageID == 1)
			m_bLog_Sel[1] = TRUE;
		else if(nStageID == 2)
			m_bLog_Sel[2] = TRUE;
		else if(nStageID == 3)
			m_bLog_Sel[3] = TRUE;
		else if(nStageID == 4)
			m_bLog_Sel[4] = TRUE;
		else if(nStageID == 5)
			m_bLog_Sel[5] = TRUE;
		else if(nStageID == 6)
			m_bLog_Sel[6] = TRUE;
		else if(nStageID == 7)
			m_bLog_Sel[7] = TRUE;
		else if(nStageID == 8)
			m_bLog_Sel[8] = TRUE;
		break;
	}
}


void CViewAuto::testResultProc()
{
		/////////////////////////////////////////////
		/*Result Data 용 Panel 정보 처리 부*/
		int nVisionCnt = 0;
		BOOL bPanelUseChk = FALSE;		
		FILE *in = NULL, *out = NULL;
		STRU_CLASSIFY_INFO m_stPcClassifyEndFlag[MAX_REV_PANEL][MAX_PANEL];	//2015.05.19 edit 3 -> MAX_VISION_PC
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
		CString strtemp = _T(""), strClsEndInfoPath = _T("");
		CString cCompareVal ;
		size_t read_size = 0;
		errno_t errFileOpen;		
		int nEqpSizeType = 0;
		SYSTEMTIME	systime ;
		LONGLONG llStartTime;
		LONGLONG llCmpTime;
		::GetLocalTime(&systime);
		int nCnt = 0, ntimeInx = 0;
		llStartTime = 0;
		
		llStartTime  = (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;
		nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
		CString cPnlCntInfo;
		cPnlCntInfo.Append(ROOT_PATH);
		cPnlCntInfo.Append(CONFIG_PNLCNT_INFO_PATH);
		
		CString cTestVal;		

		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
		int nVisionID = 0;
		CString strPanelID_1, strPanelID_2;
		
		/////////////////////////////////////////////

		int i = 0;
		
		
		
		if (nVisionID >= pFrame->m_stParamInfo.nCountVisionClient+1)
		{
			nTestCnt= 0;
			nTestPnlCnt++;
		}
		nVisionID = ++nTestCnt;
		//strPanelID_1= (_T("PNL000000000000000"));
		//strPanelID_2= (_T("PNL000000000000002"));
	//	strPanelID_1.Format(_T("A000000%d"), ++nTestCnt);
	//	strPanelID_2.Format(_T("B000000%d"), ++nTestCnt);
		strPanelID_1.TrimRight();
		strPanelID_2.TrimRight();
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		//2015.06.12 add by IKM - S		
		strClsEndInfoPath.Format(_T("%s%s"), ROOT_PATH , CLASSIFY_INFO_PATH);
		errFileOpen = _tfopen_s(&in, strClsEndInfoPath , _T("rb+")); // 구조체 바이너리 파일을 읽음

		if(errFileOpen == 0)
		{		
			read_size = fread(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag), 1, in);			
			fclose(in);
		}

		bPanelUseChk = FALSE;
		
		// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
		while(nVisionCnt < MAX_REV_PANEL)
		{		
			cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();
			
			if (!cCompareVal.IsEmpty() && cCompareVal == strPanelID_1)				
			{				
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nTotalDefCnt[nVisionID] = 0;
				}
				if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
				{
					
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID;
				}
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인						
			
				if (nEqpSizeType == EQP_SMALL_SIZE)
				{
					if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
					{
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nTotalDefCnt[nVisionID] = 0;
					}
					if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
					{
						
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID;
					}				
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE;	
				}
				bPanelUseChk = TRUE;	
				break;
				
			}
			nVisionCnt++;
		}
		
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			while(nVisionCnt < MAX_REV_PANEL)
			{
				
				cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();

				if (cCompareVal.IsEmpty() && !strPanelID_1.IsEmpty())
				{
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID , strPanelID_1);
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime = llStartTime;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID;
				
					if (nEqpSizeType == EQP_SMALL_SIZE)
					{
						_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID , strPanelID_2);
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].llStartTime = llStartTime;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID ;
					}
					bPanelUseChk = TRUE;	
					break;
				}
				nVisionCnt++;
			}
		}				

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			llCmpTime  = m_stPcClassifyEndFlag[0][PANEL_ONE].llStartTime ;
			while(nVisionCnt < MAX_REV_PANEL)
			{			
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime < llCmpTime )
				{
					llCmpTime = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime ;
					ntimeInx = nVisionCnt;
				}			
				nVisionCnt++;
			}

			_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID , strPanelID_1);
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].llStartTime = llStartTime;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID , strPanelID_2);
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].llStartTime = llStartTime;
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
			}

			nVisionCnt = ntimeInx;
		}	
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(pFrame->m_stParamInfo.nEqpObject == B_PROJECT)
			pFrame->m_DataManager.m_fnResultProcess( m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE],m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO], nVisionID);
		else if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
		{
			pFrame->m_DataManager.m_fnResultProcessPacking( m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE],m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO], nVisionID);
		}
		
		read_size = 0;	
		errFileOpen = _tfopen_s(&out, strClsEndInfoPath, _T("wb"));
		if(errFileOpen == 0)
		{
			fwrite(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag) , 1, out);		
			fclose(out);
		}
		///////////////////////////////////////////////////////////////////////////////////////////////
		//2015.06.12 add by IKM - E

		TRACE(_T("Classify End"));
}
//2016.02.24 Mr.Chu	
//기능 : 상하좌우 Radio Button 입력시 실시간 화면갱신
void CViewAuto::m_fnSelOriginType(UINT ID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	switch(ID)
	{
	case RECIPE_DEF_MAP_ORIGIN_LT:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
		break;
	case RECIPE_DEF_MAP_ORIGIN_LB:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LB;
		break;
	case RECIPE_DEF_MAP_ORIGIN_RT:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RT;
		break;
	case RECIPE_DEF_MAP_ORIGIN_RB:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RB;
		break;
	default:
		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
		break;
	}
	m_pDlgMap->OnPaint();
	m_pDlgMap->Invalidate();
}


//2016.04.14 Mr.Chu	
//기능 : QMS Coord Report Radio Button 입력시 실시간 화면갱신
void CViewAuto::m_fnActiveReportCoordQMS(UINT ID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	switch(ID)
	{
	case RECIPE_DEF_MAP_COORD_QMS_OFF:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = FALSE;
		break;
	case RECIPE_DEF_MAP_COORD_QMS_ON:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = TRUE;
		break;
	}
	m_pDlgMap->OnPaint();
	m_pDlgMap->Invalidate();
}

//2016.04.14 Mr.Chu	
//기능 : MajorAxis Radio Button 입력시 실시간 화면갱신
void CViewAuto::m_fnActiveXYReversal(UINT ID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	switch(ID)
	{
	case RECIPE_DEF_MAP_X_Y_REVERSAL_OFF:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = FALSE;
		break;
	case RECIPE_DEF_MAP_X_Y_REVERSAL_ON:
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = TRUE;
		break;
	}
	m_pDlgMap->OnPaint();
	m_pDlgMap->Invalidate();
}

//2016.03.02
LRESULT CViewAuto::m_fnLuminanceAlarmSet(WPARAM wParam, LPARAM lParam)
{

	if(m_pDlgMessage)
	{
		delete m_pDlgMessage;
		m_pDlgMessage = NULL;
	}
	
	m_fnAlarmDialog(m_pDlgMessage);
	m_pDlgMessage->PostMessage(WM_SOCK_CONTROL_LUMINANCE_ALARM,wParam, (LPARAM)lParam);
	return 0;
}

void CViewAuto::OnTimer(UINT_PTR nIDEvent)
{
	CString cCountRoot, strTemp, strStatsTime ;	
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);	
	
	TCHAR szSectionCnt[3];

	switch(nIDEvent)
	{
	case 1001:
		CTime time = CTime::GetCurrentTime();
		

		for (int i =0 ; i < 3 ; i ++)
		{
			strTemp.Format(_T("STATS_TIME%d"), i + 1);
			GetPrivateProfileString(EQP_STATE, strTemp, _T(""), szSectionCnt, sizeof(szSectionCnt), cCountRoot);
			strTemp = szSectionCnt;
			if (m_bResetFlag[i] && _ttoi(strTemp) == time.GetHour())
			{
				if(m_pDlgDefInfo != NULL)	
				{
					m_pDlgDefInfo->m_bMaualFlg = TRUE;
					//m_pDlgDefInfo->OnBnClickedBtnPnlCntReset();	//2017.01.09 일정 시간에 Reset 되는 기능  // Report 기능 사용 시 주석 해제할 것
					m_pDlgDefInfo->m_bMaualFlg = FALSE;
				}
				m_bResetFlag[i] = FALSE;
			}
			if (!m_bResetFlag[i] && _ttoi(strTemp) != time.GetHour())
				m_bResetFlag[i] = TRUE;
		}
		//KillTimer(1001);

		break;
	}

	CWnd::OnTimer(nIDEvent);
}

void CViewAuto::OnBnClickedBtnTemperature()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	m_pDlgTemperature->ShowWindow(SW_SHOW);
}

LRESULT CViewAuto::m_fnDistroyMSGDialog(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgDspMsg)
	{
		delete m_pDlgDspMsg;
		m_pDlgDspMsg = NULL;
	}
	return 0;
}

// 2016.09.14	Mr.Chu
// 기능 : 고객사 요청으로 인한 대형 ABI 설비일 경우 Panel N개 중 일정한 X번째 Panel을 사용자 설정 Judge ( 보통 3399 )로 처리한다.
void CViewAuto::OnBnClickedBtnPanelCodeTest()		//2016.09.14	Mr.Chu		왕빙 요청기능
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
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		if( m_pDlgPanelCodeTest )
		{
			delete m_pDlgPanelCodeTest;
			m_pDlgPanelCodeTest = NULL;
		}
		m_pDlgPanelCodeTest = new CDlgPanelCodeTest();
		m_pDlgPanelCodeTest->Create(IDD_DLG_PANEL_CODE_TEST, this);
		m_pDlgPanelCodeTest->ShowWindow(SW_SHOW);
		m_pDlgPanelCodeTest->MoveWindow(550, 350, 330, 310);
			
	}	

}


CString CViewAuto::m_fnSetDefectName(CString strValue)
{
	CString tempSub = _T("");
	int DefCode = _ttoi(strValue);
	//2021.03.29 add by mby S-
	if(strValue == _T("AO82")) DefCode = 82;
	else if(strValue == _T("AO83")) DefCode = 83;
	else if(strValue == _T("AO84")) DefCode = 84;
	else if(strValue == _T("AO85")) DefCode = 85;		
	else if(strValue == _T("AO86")) DefCode = 86;
	else if(strValue == _T("AO87")) DefCode = 87;
	else if(strValue == _T("AO88")) DefCode = 88;
	else if(strValue == _T("AO89")) DefCode = 89;
	else if(strValue == _T("AO90")) DefCode = 90;
	else if(strValue == _T("AO91")) DefCode = 91;
	else if(strValue == _T("AO50")) DefCode = 50;
	else if(strValue == _T("AO51")) DefCode = 51;
	else if(strValue == _T("AO52")) DefCode = 52;
	else if(strValue == _T("AO53")) DefCode = 53;
	else if(strValue == _T("AO54")) DefCode = 54;
	else if(strValue == _T("AO55")) DefCode = 55;
	else if(strValue == _T("AO56")) DefCode = 56;
	else if(strValue == _T("AO57")) DefCode = 57;
	else if(strValue == _T("AO66")) DefCode = 66;
	else if(strValue == _T("AO67")) DefCode = 67;
	else if(strValue == _T("AO68")) DefCode = 68;
	else if(strValue == _T("AO69")) DefCode = 69;
	else if(strValue == _T("AO70")) DefCode = 70;
	else if(strValue == _T("AO71")) DefCode = 71;
	else if(strValue == _T("AO72")) DefCode = 72;
	else if(strValue == _T("AO73")) DefCode = 73;
	else if(strValue == _T("AO74")) DefCode = 74;
	else if(strValue == _T("AO75")) DefCode = 75;
	else if(strValue == _T("AO76")) DefCode = 76;
	else if(strValue == _T("AO77")) DefCode = 77;
	else if(strValue == _T("AO78")) DefCode = 78;
	else if(strValue == _T("AO79")) DefCode = 79;
	else if(strValue == _T("AO80")) DefCode = 80;
	else if(strValue == _T("AO81")) DefCode = 81;
	//2021.03.29 add by mby E-

	CString temp = _T("");
	switch(DefCode)
	{
	case TFTCORNERBROKEN:
		temp = E_DEFECT_NAME_EDGE_TFT_CORNER;
		break;
	case TFTCORNERBROKEN_B:
		temp = E_DEFECT_NAME_EDGE_TFT_CORNER_B;
		break;
	case TFTCORNERBROKEN_C:
		temp = E_DEFECT_NAME_EDGE_TFT_CORNER_C;
		break;
	case TFTCORNERBROKEN_D:
		temp = E_DEFECT_NAME_EDGE_TFT_CORNER_D;
		break;
	case CFCORNERBROKEN:
		temp = E_DEFECT_NAME_EDGE_CF_CORNER;
		break;
	case CFCORNERBROKEN_B:
		temp = E_DEFECT_NAME_EDGE_CF_CORNER_B;
		break;
	case CFCORNERBROKEN_C:
		temp = E_DEFECT_NAME_EDGE_CF_CORNER_C;
		break;
	case CFCORNERBROKEN_D:
		temp = E_DEFECT_NAME_EDGE_CF_CORNER_D;
		break;
	case TFTBROKEN:
		temp = E_DEFECT_NAME_EDGE_TFT_BROKEN;
		break;
	case TFTBROKEN_B:
		temp = E_DEFECT_NAME_EDGE_TFT_BROKEN_B;
		break;
	case TFTBROKEN_C:
		temp = E_DEFECT_NAME_EDGE_TFT_BROKEN_C;
		break;
	case TFTBROKEN_D:
		temp = E_DEFECT_NAME_EDGE_TFT_BROKEN_D;
		break;
	case CFBROKEN:
		temp = E_DEFECT_NAME_EDGE_CF_BROKEN;
		break;
	case CFBROKEN_B:
		temp = E_DEFECT_NAME_EDGE_CF_BROKEN_B;
		break;
	case CFBROKEN_C:
		temp = E_DEFECT_NAME_EDGE_CF_BROKEN_C;
		break;
	case CFBROKEN_D:
		temp = E_DEFECT_NAME_EDGE_CF_BROKEN_D;
		break;
	case TFTBURR:
		temp = E_DEFECT_NAME_EDGE_TFT_BURR;
		break;
	case TFTBURR_B:
		temp = E_DEFECT_NAME_EDGE_TFT_BURR_B;
		break;
	case TFTBURR_C:
		temp = E_DEFECT_NAME_EDGE_TFT_BURR_C;
		break;
	case TFTBURR_D:
		temp = E_DEFECT_NAME_EDGE_TFT_BURR_D;
		break;
	case CFBURR:
		temp = E_DEFECT_NAME_EDGE_CF_BURR;
		break;
	case CFBURR_B:
		temp = E_DEFECT_NAME_EDGE_CF_BURR_B;
		break;
	case CFBURR_C:
		temp = E_DEFECT_NAME_EDGE_CF_BURR_C;
		break;
	case CFBURR_D:
		temp = E_DEFECT_NAME_EDGE_CF_BURR_D;
		break;
	case SOCKETSCRACH:
		temp = E_DEFECT_NAME_PAD_SCRATCH;
		break;
	case SOCKETBURN:
		temp = E_DEFECT_NAME_PAD_BURNT;
		break;
	case CFPOLMEASUREFAIL:		// EQP TYPE ABI
	case CFMEASUREFAIL:			// EQP TYPE AVL
		temp = E_DEFECT_NAME_CF_MEASURE;
		break;
	case TFTPOLMEASUREFAIL:		// EQP TYPE ABI
	case TFTMEASUREFAIL:		// EQP TYPE AVL
		temp = E_DEFECT_NAME_TFT_MEASURE;
		break;
	case CFDIMPLEPOINT:
		temp = E_DEFECT_NAME_CF_DIMPLE_POINT;
		break;
	case TFTDIMPLEPOINT:
		temp = E_DEFECT_NAME_TFT_DIMPLE_POINT;
	break;
	case CFCELLBROKEN:
		temp = E_DEFECT_NAME_CF_ACTIVE_BROKEN;
	break;
	case TFTCELLBROKEN:
		temp = E_DEFECT_NAME_TFT_ACTIVE_BROKEN;
		break;
// 	case ETCLINE:			// 1802 중복
// 		temp = E_DEFECT_NAME_OTHER_LINE;
// 		break;
	case ETCPOINT:			// 1802 중복
		temp = E_DEFECT_NAME_OTHER_POINT;
		break;
	case ALIGNFAIL:
		temp = E_DEFECT_NAME_ALIGN_FAIL;
		break;
	case CFCLUMINANCE:
		temp = E_DEFECT_NAME_CFC_LUMINANCE;
		break;
	case CBCLUMINANCE:
		temp = E_DEFECT_NAME_CBC_LUMINANCE;
		break;
	case CFSLUMINANCE:
		temp = E_DEFECT_NAME_CFS_LUMINANCE;
		break;
	case CBSLUMINANCE:
		temp = E_DEFECT_NAME_CBS_LUMINANCE;
		break;
	case TFCLUMINANCE:
		temp =  E_DEFECT_NAME_TFC_LUMINANCE;
		break;
	case TBCLUMINANCE:
		temp = E_DEFECT_NAME_TBC_LUMINANCE;
		break;
	case TFSLUMINANCE:
		temp = E_DEFECT_NAME_TFS_LUMINANCE;
		break;
	case TBSLUMINANCE:
		temp = E_DEFECT_NAME_TBS_LUMINANCE;
		break;
	case CFPROTECTION:
		temp = E_DEFECT_NAME_CF_PROTECTION;
		break;
	case TFTPROTECTION:
		temp = E_DEFECT_NAME_TFT_PROTECTION;
		break;
	case BGRADECHECK:							// 2019.03.15 by ryu
		temp = E_DEFECT_NAME_B_GRADE_CHECK;
		break;
	case CGRADECHECK:							// 2019.03.15 by ryu
		temp = E_DEFECT_NAME_C_GRADE_CHECK;	
		break;
// 	case 1470:
// 		temp = E_DEFECT_NAME_PANEL_CODE_TEST;
// 		break;
	// EQP_TYPE_ABI
	case MURA:
		temp = E_DEFECT_NAME_TDI_MURA;
		break;
	case CFPOLSUBSTANCE:
		temp = E_DEFECT_NAME_CF_SUBSTANCE;
		break;
	case TFTPOLSUBSTANCE:
		temp = E_DEFECT_NAME_TFT_SUBSTANCE;
		break;
	case CFPOLBUBBLE:
		temp = E_DEFECT_NAME_CF_POL_BUBBLE;
		break;
	case CFPOLSCRATCH:
		temp = E_DEFECT_NAME_CF_POL_SCRATCH;
		break;
	case TFTPOLBUBBLE:
		temp = E_DEFECT_NAME_TFT_POL_BUBBLE;
		break;
	case TFTPOLSCRATCH:
		temp = E_DEFECT_NAME_TFT_POL_SCRATCH;
		break;
	case POLMINOR:
		temp = E_DEFECT_NAME_TDI_MINOR;
		break;
	case POLMISS:
		temp = E_DEFECT_NAME_POL_MISS;
		break;
	case TDILUMINANCE:
		temp = E_DEFECT_NAME_TDI_LUMINANCE;
		break;
	// EQP_TYPE_AVL
	case CFSCRATCH:
		temp = E_DEFECT_NAME_CF_SCRATCH;
		break;
	case TFTSCRATCH:
		temp = E_DEFECT_NAME_TFT_SCRATCH;
		break;
	case CFWHITEBLUR:
		temp = E_DEFECT_NAME_CF_WHITE_TURBIDITY;
		break;
	case ITOPEEL:
		temp = E_DEFECT_NAME_CF_ITO;
		break;
	case CFEDGEILEGAL:
		temp = E_DEFECT_NAME_CF_ETCHING_LINE;
		break;
	case CFDIMPLELINE:
		temp = E_DEFECT_NAME_CF_DIMPLE_LINE;
		break;
	case TFTWHITEBLUR:
		temp = E_DEFECT_NAME_TFT_WHITE_TURBIDITY;
		break;
	case TFTEDGEILEGAL:
		temp = E_DEFECT_NAME_TFT_ETCHING_LINE;
		break;
	case TFTDIMPLELINE:
		temp = E_DEFECT_NAME_TFT_DIMPLE_LINE;
		break;
	case CFPIMPLEPOINT:
		temp = E_DEFECT_NAME_CF_PIMPLE_POINT;
		break;
	case TFTPIMPLEPOINT:
		temp = E_DEFECT_NAME_TFT_PIMPLE_POINT;
		break;
	case CFPIMPLELINE:
		temp = E_DEFECT_NAME_CF_PIMPLE_LINE;
		break;
	case TFTPIMPLELINE:
		temp = E_DEFECT_NAME_TFT_PIMPLE_LINE;
		break;
	case CFPIMPLECIRCLE:
		temp = E_DEFECT_NAME_PIMPLE_CIRCLE ;
		break;
	case CFWAVEPATTERN:
		temp = E_DEFECT_NAME_DIRT;
		break;
	default:
		temp = _T(""); 
		break;
	}
	return temp;
	
}

// 강제 판정 관련 추가
// 2018.10.04_ryu 
void CViewAuto::OnBnClickedGrdae()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//F_GRDAE_Dialog dig;
	//dig.DoModal(); 
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	if(pView->m_pDlgLogin)
	{
		delete pView->m_pDlgLogin;
		pView->m_pDlgLogin = NULL;
	}

	pView->m_pDlgLogin = new CLogin();	
	pView->m_pDlgLogin->m_mod = TRUE; // 다른 비밀번호를 사용하기 위한 플래그
	if (pView->m_pDlgLogin->DoModal() == IDOK)
	{		
		if( m_pDlgFgradeSet )
		{
			delete m_pDlgFgradeSet;
			m_pDlgFgradeSet = NULL;
		}
		m_pDlgFgradeSet = new F_GRDAE_Dialog();
		m_pDlgFgradeSet->Create(IDD_DIALOG2, this);
		m_pDlgFgradeSet->ShowWindow(SW_SHOW);
		//m_pDlgFgradeSet->MoveWindow(550, 350, 330, 310);
	}
}

//등급 변경 기능추가 
// 2018.12.03 ryu
void CViewAuto::OnBnClickedGradeSet()
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
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		if( m_pDlgGradeSet )
		{
			delete m_pDlgGradeSet;
			m_pDlgGradeSet = NULL;
		}
		m_pDlgGradeSet = new GRADE_SET_Dialog();
		m_pDlgGradeSet->Create(IDD_IDC_GRADE_SET, this);
		m_pDlgGradeSet->ShowWindow(SW_SHOW);
		//m_pDlgGradeSet->MoveWindow(550, 350, 330, 310);
	}
}
LRESULT CViewAuto::m_fnManualJudgeSet(WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgManualJudge)
	{
		m_pDlgManualJudge->OnDestroy();
		delete m_pDlgManualJudge;
		m_pDlgManualJudge = NULL;
	}

	if (!::IsWindow(m_pDlgManualJudge->GetSafeHwnd()))
	{
		m_fnManualJudgeDialog(m_pDlgManualJudge);		
	}	

	m_pDlgManualJudge->PostMessage( WM_SOCK_CONTROL_MANUAL_JUDGE, (WPARAM)wParam, (LPARAM)lParam );

	return 0;
}
void CViewAuto::m_fnManualJudgeDialog(CDlgManualJudge*& DlgMsg)
{	
	DlgMsg = NULL;
	DlgMsg = new CDlgManualJudge();			
	DlgMsg->Create(IDD_DLG_MANUAL_JUDGE);
	DlgMsg->ShowWindow( SW_HIDE );
	DlgMsg->UpdateWindow();

	//return DlgMsg;
}
