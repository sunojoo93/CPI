// ViewSetup.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ViewSetup.h"
#include "MainFrm.h"
#include "ViewAuto.h"

// CViewSetup
#pragma comment(lib, "UxTheme.lib")

enum
{
	TAPCTRL_INSPITEM = 0,
	TAPCTRL_LUMINANCE,
	TAPCTRL_OVERLAP,
	TAPCTRL_TIME_OVERLAP,
	TAPCTRL_JUDGESET,
	TAPCTRL_RECIPE
};

IMPLEMENT_DYNCREATE(CViewSetup, CFormView)

CViewSetup::CViewSetup()
	: CFormView(CViewSetup::IDD)
	, m_nControlMode(FALSE)
	, m_nCtrlSockMode(FALSE)
	, m_nCIMSockMode(FALSE)
	, m_bUseCIM(FALSE)
	, m_bUseCIM2(FALSE)
	, m_bUseAutoDel(FALSE)
	,m_bUseSemaphore(FALSE)
	, m_nEqpType(0)
	,m_bRecipeChangeMode(FALSE)
{
	m_pDlgPLC = NULL;
	m_pDlgLumSetup = NULL;
	m_pDlgJudgeUI = NULL;
	m_pDlgLogin = NULL;
	m_pDlgRecipe = NULL;	//2015.11.05
	//m_pDlgMessage = NULL;
	//m_pDlgErrorMsg = NULL;
	m_pDlgSetJudgeItem = NULL; //2015.12.10 Mr.Chu
	m_pDlgDefSetup = NULL;

}

CViewSetup::~CViewSetup()
{
	if(m_pDlgPLC)
	{
		delete m_pDlgPLC;
		m_pDlgPLC = NULL;
	}

	if (m_pDlgLumSetup)
	{
		delete m_pDlgLumSetup;
		m_pDlgLumSetup = NULL;
	}

	if(m_pDlgRecipe)
	{
		delete m_pDlgRecipe;
		m_pDlgRecipe = NULL;
	}
	/*if(m_pDlgErrorMsg)
	{
	delete m_pDlgErrorMsg;
	m_pDlgErrorMsg = NULL;
	}
	if(m_pDlgMessage)
	{
	delete m_pDlgMessage;
	m_pDlgMessage = NULL;
	}*/
	//2015.12.10 Mr.Chu
	if(m_pDlgSetJudgeItem)
	{
		delete m_pDlgSetJudgeItem;
		m_pDlgSetJudgeItem = NULL;
	}
	if(m_pDlgDefSetup)
	{
		delete m_pDlgDefSetup;
		m_pDlgDefSetup = NULL;
	}
	if (m_GridIPList)
	{
	  delete m_GridIPList;
	  m_GridIPList=NULL;
	}

	if (m_pDlgJudgeUI)
	{
		delete m_pDlgJudgeUI;
		m_pDlgJudgeUI = NULL;
	}
}

void CViewSetup::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_PLC, m_nControlMode);
	DDX_Radio(pDX, IDC_RADIO_CTRL_SOCK_PASSIVE, m_nCtrlSockMode);
	DDX_Radio(pDX, IDC_RADIO_CIM_SOCK_PASSIVE, m_nCIMSockMode);
	DDX_Check(pDX, IDC_CHECK_CIM_USE, m_bUseCIM);
	DDX_Check(pDX, IDC_CHECK_CIM_USE2, m_bUseCIM2);	//2021.03.04 add by mby
	DDX_Check(pDX, IDC_CHECK_AUTODEL_USE, m_bUseAutoDel);
	DDX_Check(pDX, IDC_CHECK_SIGNAL_FLAG_USE, m_bUseSemaphore);
	DDX_Check(pDX, IDC_CHECK_NG_FLAG, m_bUseNgFlag);
	DDX_Radio(pDX, IDC_RADIO_ACC, m_nEqpType);
	DDX_Control(pDX, IDC_TAB_SETUP, m_ctrlSetupTab);
	DDX_Check(pDX, IDC_CHECK_MANUAL_JUDGE, m_bUseManualJudge);	//2018.05.10
	DDX_Check(pDX, IDC_CHECK_BUBBLE_AREA_ALARM, m_bUseBubbleAreaAlarm);	//2020.02.28 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC1, m_bUseInspPC[0]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC2, m_bUseInspPC[1]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC3, m_bUseInspPC[2]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC4, m_bUseInspPC[3]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC5, m_bUseInspPC[4]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC6, m_bUseInspPC[5]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC7, m_bUseInspPC[6]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC8, m_bUseInspPC[7]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_INSP_PC9, m_bUseInspPC[8]);		//2020.07.22 add by mby
	DDX_Check(pDX, IDC_CHK_ALIGN_PC, m_bUseAlignPC);		//2020.10.15 add by mby
	
}

BEGIN_MESSAGE_MAP(CViewSetup, CFormView)
	ON_MESSAGE(UM_MAINMENU_CHANGE,	OnMainMenuChange)
	ON_BN_CLICKED(IDC_BTN_LOAD_INFO, &CViewSetup::OnBnClickedBtnLoadInfo)
	ON_BN_CLICKED(IDC_BTN_SAVE_INFO, &CViewSetup::OnBnClickedBtnSaveInfo)
	ON_BN_CLICKED(IDC_BTN_SOCK_START, &CViewSetup::OnBnClickedBtnSockStart)
	ON_BN_CLICKED(IDC_BTN_SOCK_STOP, &CViewSetup::OnBnClickedBtnSockStop)
	ON_BN_CLICKED(IDC_BTN_SAVE_SOCK_SET, &CViewSetup::OnBnClickedBtnSaveSockSet)
	ON_BN_CLICKED(IDC_BTN_TEST_SETUP, &CViewSetup::OnBnClickedBtnTestSetup)
	ON_BN_CLICKED(IDC_RADIO_PLC, &CViewSetup::OnBnClickedRadioPlc)
	ON_BN_CLICKED(IDC_RADIO_PC, &CViewSetup::OnBnClickedRadioPc)
	ON_BN_CLICKED(IDC_CHECK_CIM_USE, &CViewSetup::OnBnClickedCheckCimUse)
	ON_BN_CLICKED(IDC_CHECK_CIM_USE2, &CViewSetup::OnBnClickedCheckCimUse2)	//2021.03.04 add by mby
	ON_BN_CLICKED(IDC_CHECK_AUTODEL_USE, &CViewSetup::OnBnClickedCheckAutodelUse)
	ON_BN_CLICKED(IDC_RADIO_CIM_SOCK_PASSIVE, &CViewSetup::OnBnClickedRadioCimSockPassive)
	
	/*ON_MESSAGE(WM_SOCK_CONTROL_MEASURE_ALARM, &CViewSetup::m_fnMeasureAlarmSet)
	ON_MESSAGE(WM_SOCK_CONTROL_CONTACT_ALARM, &CViewSetup::m_fnContactAlarmSet)
	ON_MESSAGE(WM_SOCK_CONTROL_LIST_ALARM, &CViewSetup::m_fnListlAlarm)*/
	//ON_BN_CLICKED(IDC_BTN_JUDGE, &CViewSetup::OnBnClickedBtnJudge)
	//ON_BN_CLICKED(IDC_BTN_RECIPE, &CViewSetup::OnBnClickedBtnRecipe)
	//ON_BN_CLICKED(IDC_BTN_SET_INSP_ITEM, &CViewSetup::OnBnClickedBtnSetInspItem)
	//ON_BN_CLICKED(IDC_BTN_OVERLAP_DEF, &CViewSetup::OnBnClickedBtnOverlapDef)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SETUP, &CViewSetup::OnTcnSelchangeTabSetup)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_MEASURE_SAVE, &CViewSetup::OnBnClickedBtnMeasureSave)
	ON_BN_CLICKED(IDC_BTN_INCH_CHANGE, &CViewSetup::OnBnClickedBtnInchChange)
	ON_BN_CLICKED(IDC_BTN_MSAVE, &CViewSetup::OnBnClickedBtnMsave)
	ON_BN_CLICKED(IDC_BTN_SET_LIMIT, &CViewSetup::OnBnClickedBtnSetLimit)
	ON_BN_CLICKED(IDC_BTN_LIFE_RESET1, &CViewSetup::OnBnClickedBtnLifeReset1)
	ON_BN_CLICKED(IDC_BTN_LIFE_RESET2, &CViewSetup::OnBnClickedBtnLifeReset2)
	ON_BN_CLICKED(IDC_BTN_LIFE_RESET3, &CViewSetup::OnBnClickedBtnLifeReset3)
	ON_BN_CLICKED(IDC_BTN_CHKALL, &CViewSetup::OnBnClickedBtnChkall)
END_MESSAGE_MAP()


// CViewSetup 진단입니다.

#ifdef _DEBUG
void CViewSetup::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewSetup::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewSetup 메시지 처리기입니다.
void CViewSetup::m_fnInitGridIPList()
{
	int nColumnCount;

	// Log Grid 생성
	CRect rect;
	GetDlgItem(IDC_GRID_IP)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_GridIPList = new CGridCtrl();
	m_GridIPList->Create(rect, this, IDC_GRID_IP);
	m_GridIPList->SetParent(this);
	m_GridIPList->SetListMode(true);
	//	m_GridIPList->EnableDragAndDrop(true);
	m_GridIPList->SetFixedBkColor(RGB(220, 240, 220));
	m_GridIPList->SetBkColor(RGB(200, 200, 200));
	m_GridIPList->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridIPList->SetFixedRowCount(1);

	CString colNameSystem[5] = { _T("No"), _T("Item"), _T("IP"), _T("Port"), _T("Status") };
	nColumnCount = 5;
	m_GridIPList->SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		if(k == 0)
			m_GridIPList->SetColumnWidth(k, 30);
		else if(k == 1)
			m_GridIPList->SetColumnWidth(k, 120);
		else if(k == 2)
			m_GridIPList->SetColumnWidth(k, 130);	//150
		else if(k == 3)
			m_GridIPList->SetColumnWidth(k, 80);
		else
			m_GridIPList->SetColumnWidth(k, 100);

		m_GridIPList->SetItemText(0, k, colNameSystem[k]);
	}
}

void CViewSetup::m_fnUpdateStatus()
{
	G_AddLog(3, 3, _T("m_fnUpdateStatus() - Start"));
	CString strTemp, strIPAddr, strPort;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if(m_pDlgPLC->m_bOpenPLC)
	{
		m_pDlgPLC->GetDlgItem(IDC_GXBTN_PLC_OPEN)->EnableWindow(FALSE);
		m_pDlgPLC->GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(TRUE);
	}
	else
	{
		m_pDlgPLC->GetDlgItem(IDC_GXBTN_PLC_OPEN)->EnableWindow(TRUE);
		m_pDlgPLC->GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(FALSE);
	}

	if(m_pDlgPLC)
		pFrame->m_pVsSeverIF->m_stSockStatus.bPLC = m_pDlgPLC->m_bOpenPLC;

	for(int i = 1; i < m_GridIPList->GetRowCount(); i++)
	{
		strTemp = m_GridIPList->GetItemText(i, 1);

		if(strTemp == KEY_VISION_SERVER)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionServer)
				m_GridIPList->SetItemText(i, 4, STATUS_LISTENING);
			else
				m_GridIPList->SetItemText(i, 4, STATUS_CLOSED);
		}
		else if(strTemp == KEY_VISION_PC_1)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[0])
			{			
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_2)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[1])
			{			
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_3)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[2])
			{			
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_4)	//2015.05.12 add s-
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[3])
			{			
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_5)	
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[4])
			{				
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_6)	
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[5])
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_7)	
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[6])
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_8)	
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[7])
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_VISION_PC_9)	
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[8])
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}	//2015.05.12 add e-
		else if(strTemp == KEY_SIGNAL_FLAG)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bSignalFlag)
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_PRE_ALIGN_PC)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign)
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}//2021.09.09
		else if(strTemp == KEY_PRE_ALIGN_PC2)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign)
			{
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			}
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_PLC)
		{
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bPLC)
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
		}
		else if(strTemp == KEY_CONTROL_PC)
		{
		
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bControlPC)
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);
			
		}
		else if(strTemp == KEY_CIM)
		{			
			if(pFrame->m_pVsSeverIF->m_stSockStatus.bCIM)
				m_GridIPList->SetItemText(i, 4, STATUS_CONNECTED);
			else
				m_GridIPList->SetItemText(i, 4, STATUS_DISCONNECTED);			
		}
	}

	m_GridIPList->Refresh();
	G_AddLog(3, 3, _T("m_fnUpdateStatus() - End"));
}

void CViewSetup::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_fnInitGridIPList();

	//m_fnCreateDlgPLC(); 

	//m_fnCreateDlgLuminanceSetup();

	m_fnCreateParamSetupTool();

	m_fnCreateTabControl();	//2016.10.10 Mr.Chu
	m_fnInitMeasureLimitInfo();		//2019.02.18 add by mby
}

void CViewSetup::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{

	CFormView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

LRESULT CViewSetup::OnMainMenuChange(WPARAM wParam, LPARAM lParam)
{
	m_GridIPList->DeleteNonFixedRows();
	m_GridIPList->Refresh();

	m_fnLoadIPList(TRUE);
	m_fnLoadParam(TRUE);

	m_fnUpdateStatus();

	//2015.11.05
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd(); 
	if(wParam == 1 && pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
	{
		long lModelChangeInterlock = -1;
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

		lModelChangeInterlock = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_INTERLOCK);

		if (lModelChangeInterlock == OK)
		{
			m_fnRequestRecipeVision();
			m_fnReadRecipePLC();
		}
	}
	//2015.11.05
	
	return 0;
}

void CViewSetup::m_fnAddCommInfo(CString strIP, int nPort, CString strItem)
{
	CString strPort, strNo;
	int nRowCount = m_GridIPList->GetRowCount();

	strPort.Format(_T("%d"), nPort);
	strNo.Format(_T("%d"), nRowCount);

	m_GridIPList->SetRowCount(nRowCount + 1);

	m_GridIPList->SetItemText(nRowCount, 0, strNo);
	m_GridIPList->SetItemText(nRowCount, 1, strItem);
	m_GridIPList->SetItemText(nRowCount, 2, strIP);
	m_GridIPList->SetItemText(nRowCount, 3, strPort);
	m_GridIPList->SetItemText(nRowCount, 4, _T("None"));
	m_GridIPList->Refresh();
}

void CViewSetup::m_fnLoadIPList(BOOL bGridView)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strTemp;
	int nRead = 0;
	wchar_t chRead[255] = {0};

	// Get IP
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_SERVER, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_VisionServer = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_1, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_1 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_2, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_2 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_3, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_3 = chRead;	
	//2015.05.12 add s-
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_4, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_4 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_5, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_5 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_6, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_6 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_7, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_7 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_8, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_8 = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_9, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_Vision_9 = chRead;	
	//2015.05.12 add e-
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_SIGNAL_FLAG, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_SignalFlag = chRead;	

	//2021.09.07 add by mby S-
	int RcpAlignPCNo = pFrame->m_iniConfigSet.GetProfileInt(SECTION_PARAM, RECIPE_ALIGNPC_NO, 0);
	if(RcpAlignPCNo == 0){
		pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_PRE_ALIGN_PC, _T(""), (LPTSTR)chRead, 255);
	}else{
		pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_PRE_ALIGN_PC2, _T(""), (LPTSTR)chRead, 255);
	}	
	pFrame->m_stCommInfo.strIP_PreAlignPC = chRead;	
	//2021.09.07 add by mby E-

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_CIM, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_CIM = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_PLC, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_PLC = chRead;	
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_CONTROL_PC, _T(""), (LPTSTR)chRead, 255);
	pFrame->m_stCommInfo.strIP_ControlPC = chRead;	

	// Get Port
	pFrame->m_stCommInfo.nPort_VisionServer = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_SERVER, 0);
	pFrame->m_stCommInfo.nPort_Vision_1 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_1, 0);
	pFrame->m_stCommInfo.nPort_Vision_2 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_2, 0);
	pFrame->m_stCommInfo.nPort_Vision_3 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_3, 0);
	//2015.05.12 add s-
	pFrame->m_stCommInfo.nPort_Vision_4 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_4, 0);
	pFrame->m_stCommInfo.nPort_Vision_5 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_5, 0);
	pFrame->m_stCommInfo.nPort_Vision_6 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_6, 0);
	pFrame->m_stCommInfo.nPort_Vision_7 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_7, 0);
	pFrame->m_stCommInfo.nPort_Vision_8 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_8, 0);
	pFrame->m_stCommInfo.nPort_Vision_9 = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_VISION_PC_9, 0);
	//2015.05.12 add e-
	pFrame->m_stCommInfo.nPort_CIM = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_CIM, 0);
	pFrame->m_stCommInfo.nPort_PLC = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_PLC, 0);
	pFrame->m_stCommInfo.nPort_ControlPC = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_CONTROL_PC, 0);
	pFrame->m_stCommInfo.nPort_SignalFlag = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_SIGNAL_FLAG, 0);
	//2021.09.07 add by mby S-
	if(RcpAlignPCNo == 0){
		pFrame->m_stCommInfo.nPort_PreAlignPC = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_PRE_ALIGN_PC, 0);	//2015.08.19
	}else{
		pFrame->m_stCommInfo.nPort_PreAlignPC = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PORT, KEY_PRE_ALIGN_PC2, 0);	//2015.08.19
	}
	//2021.09.07 add by mby E-
	if(bGridView)
	{
		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_VisionServer, pFrame->m_stCommInfo.nPort_VisionServer, KEY_VISION_SERVER);
		////2015.05.12 comment s-
		//m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_1, pFrame->m_stCommInfo.nPort_Vision_1, KEY_VISION_PC_1);
		//m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_2, pFrame->m_stCommInfo.nPort_Vision_2, KEY_VISION_PC_2);
		//m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_3, pFrame->m_stCommInfo.nPort_Vision_3, KEY_VISION_PC_3);
		////2015.05.12 comment e-
		//2015.05.12 add s-
		for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			if(i == 0)		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_1, pFrame->m_stCommInfo.nPort_Vision_1, KEY_VISION_PC_1);
			else if(i == 1)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_2, pFrame->m_stCommInfo.nPort_Vision_2, KEY_VISION_PC_2);
			else if(i == 2)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_3, pFrame->m_stCommInfo.nPort_Vision_3, KEY_VISION_PC_3);
			else if(i == 3)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_4, pFrame->m_stCommInfo.nPort_Vision_4, KEY_VISION_PC_4);
			else if(i == 4)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_5, pFrame->m_stCommInfo.nPort_Vision_5, KEY_VISION_PC_5);
			else if(i == 5)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_6, pFrame->m_stCommInfo.nPort_Vision_6, KEY_VISION_PC_6);
			else if(i == 6)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_7, pFrame->m_stCommInfo.nPort_Vision_7, KEY_VISION_PC_7);
			else if(i == 7)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_8, pFrame->m_stCommInfo.nPort_Vision_8, KEY_VISION_PC_8);
			else if(i == 8)	m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_Vision_9, pFrame->m_stCommInfo.nPort_Vision_9, KEY_VISION_PC_9);
		}
		//2015.05.12 add e-

		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_SignalFlag, pFrame->m_stCommInfo.nPort_SignalFlag, KEY_SIGNAL_FLAG);
		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_PLC, pFrame->m_stCommInfo.nPort_PLC, KEY_PLC);
		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_CIM, pFrame->m_stCommInfo.nPort_CIM, KEY_CIM);
		m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_ControlPC, pFrame->m_stCommInfo.nPort_SignalFlag, KEY_CONTROL_PC);
		//2021.09.09 add by mby S-
		if(RcpAlignPCNo == 0){
			m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_PreAlignPC, pFrame->m_stCommInfo.nPort_PreAlignPC, KEY_PRE_ALIGN_PC);	//2015.08.19
		}else{
			m_fnAddCommInfo(pFrame->m_stCommInfo.strIP_PreAlignPC, pFrame->m_stCommInfo.nPort_PreAlignPC, KEY_PRE_ALIGN_PC2);	//2015.08.19
		}
		//2021.09.09 add by mby E-
		
	}
}

void CViewSetup::m_fnSaveIPList()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strIP, strPort, strItem;

	for(int i = 1; i < m_GridIPList->GetRowCount(); i++)
	{
		strItem = m_GridIPList->GetItemText(i, 1);
		strIP = m_GridIPList->GetItemText(i, 2);
		strPort = m_GridIPList->GetItemText(i, 3);

		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_IP, strItem, strIP);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PORT, strItem, strPort);
	}
}

void CViewSetup::m_fnLoadParam(BOOL bUIUpdate)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strTemp;
	int nRead = 0;
	char chRead[255] = {0};

	int nEqpSizeType = pFrame->m_fnEqpSizeType();

	// Get Param
	pFrame->m_stParamInfo.bUseAutoDel			= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_AUTODEL, 0);
	pFrame->m_stParamInfo.nLogDelDay			= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LOGDEL, 14);
	pFrame->m_stParamInfo.nSummaryBackupDay		= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_SUMMARY_BACKUP, 30);
	pFrame->m_stParamInfo.nCountVisionClient	= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_VPC_CNT, 1);
	pFrame->m_stParamInfo.nCountStage			= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_STAGE_CNT, 1);	//2015.05.13 add
	pFrame->m_stParamInfo.nControlMode			= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_CTRL_MODE, 1);
	pFrame->m_stParamInfo.bControlSockMode		= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_CTRL_SOCK_MODE, 1);
	pFrame->m_stParamInfo.bUseCIM				= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_CIM, 0);
	pFrame->m_stParamInfo.bUseCIM2				= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_CIM2, 0);	//2021.03.04 add by mby
	pFrame->m_stParamInfo.bCIMSockMode			= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_CIM_SOCK_MODE, 1);
	pFrame->m_stParamInfo.nEqpType				= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	pFrame->m_stParamInfo.bUseSignalFlag		= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 1);
	pFrame->m_stParamInfo.nEqpObject			= pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_OBJECT, 1);
	pFrame->m_stParamInfo.bUseManualJudge		= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_MANUAL_JUDGE, 0);	//2018.05.10
	pFrame->m_stParamInfo.bUseBubbleAreaAlarm	= pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_BUBBLE_AREA_ALARM, 0);	//2020.02.28 add by mby
	
	//2015.10.30 Mr.Chu
	pFrame->m_stParamInfo.nEqpSize				= pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 1);
	m_bUseNgFlag								= pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0); //2015.10.29 add by ikm for ng flag - PACKING
	pFrame->m_LogMng.m_nCurrentLogPriority		=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LOG_PRIORITY, 0);

	//2020.07.22 add by mby S-
	for(int i = 0 ; i < 9 ; i++){
		strTemp.Format(_T("PC%d"), i+1);
		pFrame->m_stParamInfo.bUseInspPc[i] = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, strTemp, 0);
	}
	//2020.07.22 add by mby E-
		pFrame->m_stParamInfo.bUseAlignPc = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, _T("ALIGNPC"), 0);

	//2020.01.07 add by mby S-
	for(int i = 0 ; i<3 ; i++){
		strTemp.Format(_T("LIFE%d"), i+1);
		m_nLightLifeCnt[i] = pFrame->m_iniConfigSet.GetProfileIntW(_T("LIFE_COUNT"), strTemp, 0);
		strTemp.Format(_T("%d"), m_nLightLifeCnt[i]);
		if(i == 0 ){	
			GetDlgItem(IDC_EDIT_LIFE_SIDE1)->SetWindowText(strTemp);
		}else if( i == 1){
			GetDlgItem(IDC_EDIT_LIFE_SIDE2)->SetWindowText(strTemp);
		}else{
			GetDlgItem(IDC_EDIT_LIFE_SIDE3)->SetWindowText(strTemp);
		}
	}
	//2020.01.07 add by mby E-
	if(bUIUpdate)
	{
		strTemp.Format(_T("%d"), pFrame->m_stParamInfo.nCountVisionClient);
		GetDlgItem(IDC_EDIT_VISION_PC)->SetWindowText(strTemp);

		strTemp.Format(_T("%d"), pFrame->m_stParamInfo.nLogDelDay);
		GetDlgItem(IDC_EDIT_LOG_DEL_DAY)->SetWindowText(strTemp);

		strTemp.Format(_T("%d"), pFrame->m_stParamInfo.nSummaryBackupDay);
		GetDlgItem(IDC_EDIT_SUMMARY_B_DAY)->SetWindowText(strTemp);

		strTemp.Format(_T("%d"), pFrame->m_LogMng.m_nCurrentLogPriority);
		GetDlgItem(IDC_EDIT_LOG_PRIORITY)->SetWindowText(strTemp);

		m_bUseAutoDel = pFrame->m_stParamInfo.bUseAutoDel;
		m_nControlMode = pFrame->m_stParamInfo.nControlMode;
		m_nCtrlSockMode = pFrame->m_stParamInfo.bControlSockMode;
		m_bUseCIM = pFrame->m_stParamInfo.bUseCIM;
		m_bUseCIM2 = pFrame->m_stParamInfo.bUseCIM2;	//2021.03.04 add by mby
		m_nCIMSockMode = pFrame->m_stParamInfo.bCIMSockMode;
		m_nEqpType = pFrame->m_stParamInfo.nEqpType;
		m_bUseSemaphore = pFrame->m_stParamInfo.bUseSignalFlag;
		m_bUseManualJudge = pFrame->m_stParamInfo.bUseManualJudge;	//2018.05.10
		m_bUseBubbleAreaAlarm = pFrame->m_stParamInfo.bUseBubbleAreaAlarm;	//2020.02.28 add by mby
		GetDlgItem(IDC_STATIC_SOCK_MODE1)->EnableWindow(m_nControlMode);
		GetDlgItem(IDC_RADIO_CTRL_SOCK_PASSIVE)->EnableWindow(m_nControlMode);
		GetDlgItem(IDC_RADIO_CTRL_SOCK_ACTIVE)->EnableWindow(m_nControlMode);

		GetDlgItem(IDC_STATIC_SOCK_MODE2)->EnableWindow(m_bUseCIM);
		GetDlgItem(IDC_RADIO_CIM_SOCK_PASSIVE)->EnableWindow(m_bUseCIM);
		GetDlgItem(IDC_RADIO_CIM_SOCK_ACTIVE)->EnableWindow(m_bUseCIM);

		GetDlgItem(IDC_STATIC_LOG_DAY)->EnableWindow(m_bUseAutoDel);
		GetDlgItem(IDC_EDIT_LOG_DEL_DAY)->EnableWindow(m_bUseAutoDel);

		if( nEqpSizeType == EQP_MONITOR_SIZE )
		{
			//2017.04.29	모니터 측정 스펙 설정
			GetDlgItem(IDC_STATIC_MEASURE_SPEC_INFO)->ShowWindow(TRUE);
			GetDlgItem(IDC_EDIT_MEASURE_SPEC)->ShowWindow(TRUE);
			GetDlgItem(IDC_BTN_MEASURE_SAVE)->ShowWindow(TRUE);

			//int nMeasureSpec = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_MEASURE_SPEC, _T("Spec"), 0);
			pFrame->m_iniConfigSet.GetProfileStringW(SECTION_MEASURE_SPEC, _T("Spec"), _T(""), (LPTSTR)chRead, 255);
			strTemp.Format(_T("%s"), chRead);
			GetDlgItem(IDC_EDIT_MEASURE_SPEC)->SetWindowText( strTemp );
			//
		}

		//2020.08.27 add by mby
		for(int i = 0 ; i < 9; i++){
			GetDlgItem(1888 + i)->ShowWindow(FALSE);	//IDC_CHK_INSP_PC1 :1888 ~
			m_bUseInspPC[i] = pFrame->m_stParamInfo.bUseInspPc[i];
		}
		//2020.07.22 add by mby
		for(int i = 0 ; i< pFrame->m_stParamInfo.nCountVisionClient; i++){			
			GetDlgItem(1888 + i)->ShowWindow(TRUE);	//IDC_CHK_INSP_PC1 :1753 ~
		}
		m_bUseAlignPC = pFrame->m_stParamInfo.bUseAlignPc;	//2020.10.15 add by mby
		//strTemp.Format(_T("%s"),m_bUseNgFlag);
		CheckDlgButton(IDC_CHECK_NG_FLAG, m_bUseNgFlag);     //2015.10.29 add by ikm for ng flag - PACKING                      		
		UpdateData(FALSE);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		
		if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
		{	
			GetDlgItem(IDC_SIGNAL_FLAG2)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_NG_FLAG)->EnableWindow(FALSE);
		}
	}

	m_pDlgLumSetup->m_fnLoadParameter();
	m_pDlgPLC->m_fnLoadParameter();	//2015.06.02

	//2016.10.10 Mr.Chu
	m_pDlgDefSetup->m_fnLoadParameter();
	m_pDlgJudgeUI->m_fnLoadParameter();

	// Setup -> Auto 이동일 때도 LoadParam함수가 호출됨으로 인해 Recipe UI 호출 시 PLC Bit를 보고 표시 여부에 대한
	// 정보 꼬임방지를 위해 초기화면으로 이동한다
	if(m_ctrlSetupTab.GetCurSel() == TAPCTRL_RECIPE)
	{
		RECT rc;
		m_ctrlSetupTab.SetCurSel(TAPCTRL_OVERLAP);
		m_pDlgRecipe->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_BTN_SAVE_SOCK_SET)->EnableWindow(TRUE);

		m_pDlgSetJudgeItem->ShowWindow(SW_SHOW);

		m_pDlgSetJudgeItem->GetWindowRect(&rc);
		m_pDlgSetJudgeItem->GetClientRect(&rc);
		m_pDlgSetJudgeItem->MoveWindow(&rc);
		m_pDlgSetJudgeItem->MoveWindow(rc.left + 550, rc.top + 70, rc.right, rc.bottom);
	}
	//
	
}

void CViewSetup::m_fnSaveParam()
{
	G_AddLog(3, 3, _T("m_fnSaveParam() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strTemp,strTemp1;

	GetDlgItem(IDC_EDIT_VISION_PC)->GetWindowTextW(strTemp);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_VPC_CNT, strTemp);
	
	GetDlgItem(IDC_EDIT_LOG_DEL_DAY)->GetWindowTextW(strTemp);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LOGDEL, strTemp);

	GetDlgItem(IDC_EDIT_SUMMARY_B_DAY)->GetWindowTextW(strTemp);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_SUMMARY_BACKUP, strTemp);

	UpdateData();

	strTemp.Format(_T("%d"), m_nControlMode);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_CTRL_MODE, strTemp);

	strTemp.Format(_T("%d"), m_nCtrlSockMode);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_CTRL_SOCK_MODE, strTemp);

	strTemp.Format(_T("%d"), m_bUseCIM);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_CIM, strTemp);

	//2021.03.04 add by mby
	strTemp.Format(_T("%d"), m_bUseCIM2);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_CIM2, strTemp);
	pFrame->m_stParamInfo.bUseCIM2 = m_bUseCIM2;

	strTemp.Format(_T("%d"), m_nEqpType);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, strTemp);

	if(m_bUseCIM)
	{
		strTemp.Format(_T("%d"), m_nCIMSockMode);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_CIM_SOCK_MODE, strTemp);
	}

	strTemp.Format(_T("%d"), m_bUseAutoDel);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_AUTODEL, strTemp);

	strTemp.Format(_T("%d"), m_bUseSemaphore);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, strTemp);

	//2018.05.10
	strTemp.Format(_T("%d"), m_bUseManualJudge);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_MANUAL_JUDGE, strTemp);

	//2020.02.28 add by mby
	strTemp.Format(_T("%d"), m_bUseBubbleAreaAlarm);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_BUBBLE_AREA_ALARM, strTemp);
	if(pFrame->m_stParamInfo.bUseAutoDel)
		pFrame->m_LogMng.m_fnSetAutoDeleteInfo(strTemp, pFrame->m_stParamInfo.nLogDelDay);

	strTemp.Format(_T("%d"), m_bUseNgFlag);
	pFrame->m_iniConfigSet.WriteProfileStringW(EQP_STATE, KEY_EQP_USE_NG_FLAG, strTemp); //2015.10.29 add by ikm for ng flag - PACKING 

	GetDlgItem(IDC_EDIT_LOG_PRIORITY)->GetWindowText(strTemp);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LOG_PRIORITY, strTemp);

	//2020.07.22 add by mby S-
	for(int i = 0 ; i < 9 ; i++){
		strTemp.Format(_T("%d"), m_bUseInspPC[i]);
		strTemp1.Format(_T("PC%d"), i+1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, strTemp1, strTemp);
	}
	//2020.07.22 add by mby E-

	strTemp.Format(_T("%d"), m_bUseAlignPC);	//2020.10.15 add by mby
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, _T("ALIGNPC"), strTemp); //2020.10.15 add by mby
	//BOOL bCheck = ()->GetCheck();

	CButton* btnOverlapCheck = (CButton*)m_pDlgPLC->GetDlgItem(IDC_CHECK_OVERLAP_USE);
	BOOL bCheck = btnOverlapCheck->GetCheck();
	m_pDlgPLC->m_fnOverlapUseCheck(bCheck);
	pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;


	CButton* btnLuminanceCheck = (CButton*)m_pDlgPLC->GetDlgItem(IDC_CHECK_LUMINANCE_ALARM_USE);
	m_pDlgLumSetup->m_fnLuminanceAlarmUseCheck(bCheck);
	
	//
	m_pDlgLumSetup->m_fnSaveParameter();
	m_pDlgPLC->m_fnSaveParameter();	//2015.06.02
	m_pDlgSetJudgeItem->OnBnClickedButtonSave();
	m_pDlgDefSetup->m_fnSaveParameter();
	m_pDlgJudgeUI->m_fnSaveJudgeCount();
	//

	G_AddLog(3, 3, _T("m_fnSaveParam() - End"));
}

void CViewSetup::m_fnCreateDlgPLC()
{
	m_pDlgPLC = new CDlgPLCSetup();
    m_pDlgPLC->Create(IDD_DLG_PLC_SETUP, this);
// 	m_pDlgPLC->ShowWindow(SW_SHOW);
// 
// 	RECT rc;
// 	m_pDlgPLC->GetWindowRect(&rc);
// 	m_pDlgPLC->GetClientRect(&rc);
// 	m_pDlgPLC->MoveWindow(&rc);
// 	m_pDlgPLC->MoveWindow(rc.left + 550, rc.top + 30, rc.right, rc.bottom);
}

//2016.10.10 Mr.Chu
//Parameter 셋업을 위한 도구들의 생성
void CViewSetup::m_fnCreateParamSetupTool()
{
	if(m_pDlgPLC)
	{
		delete m_pDlgPLC;
		m_pDlgPLC = NULL;
	}
	m_pDlgPLC = new CDlgPLCSetup();
	m_pDlgPLC->Create(IDD_DLG_PLC_SETUP, this);

	if(m_pDlgLumSetup)
	{
		delete m_pDlgLumSetup;
		m_pDlgLumSetup = NULL;
	}
	m_pDlgLumSetup = new CDlgLuminanceSetup();
	m_pDlgLumSetup->Create(IDD_DLG_LUMINANCE_SETUP, this);

	if(m_pDlgSetJudgeItem)
	{
		delete m_pDlgSetJudgeItem;
		m_pDlgSetJudgeItem = NULL;
	}
	m_pDlgSetJudgeItem = new CDlgSetJudgeItem();
	m_pDlgSetJudgeItem->Create(IDD_DLG_SET_ITEM, this);

	if(m_pDlgDefSetup)
	{
		delete m_pDlgDefSetup;
		m_pDlgDefSetup = NULL;
	}
	m_pDlgDefSetup = new CDlgDEFSetup();
	m_pDlgDefSetup->Create(IDD_DLG_DEF_SETUP, this);

	if(m_pDlgRecipe)
	{
		delete m_pDlgRecipe;
		m_pDlgRecipe = NULL;
	}
	m_pDlgRecipe = new CDlgRecipe();
	m_pDlgRecipe->Create(IDD_DLG_RECIPE, this);

	if(m_pDlgJudgeUI)
	{
		delete m_pDlgJudgeUI;
		m_pDlgJudgeUI = NULL;
	}

	m_pDlgJudgeUI = new CDlgJudgeUI();
	m_pDlgJudgeUI->Create(IDD_DLG_JUDGE_UI, this);

}

void CViewSetup::m_fnCreateDlgLuminanceSetup()
{
	m_pDlgLumSetup = new CDlgLuminanceSetup();
	m_pDlgLumSetup->Create(IDD_DLG_LUMINANCE_SETUP, this);
// 	m_pDlgLumSetup->ShowWindow(SW_SHOW);
// 
// 	RECT rc;
// 	m_pDlgLumSetup->GetWindowRect(&rc);
// 	m_pDlgLumSetup->GetClientRect(&rc);
// 	m_pDlgLumSetup->MoveWindow(&rc);
// 	m_pDlgLumSetup->MoveWindow(rc.left + 560, rc.top + 720, rc.right, rc.bottom);
}

//2016.10.10 Mr.Chu		Tab Control
void CViewSetup::m_fnCreateTabControl()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_ctrlSetupTab.BringWindowToTop();
	SetWindowTheme(GetDlgItem(IDC_TAB_SETUP)->GetSafeHwnd(), L"", L"");
	m_ctrlSetupTab.InsertItem(TAPCTRL_INSPITEM+1, _T("Set Insp Item"));
	m_ctrlSetupTab.InsertItem(TAPCTRL_LUMINANCE+1, _T("Luminance Setting"));
	m_ctrlSetupTab.InsertItem(TAPCTRL_OVERLAP+1, _T("Overlap Setting && PLC Control"));
	m_ctrlSetupTab.InsertItem(TAPCTRL_TIME_OVERLAP+1, _T("Time Overlap Setting"));
	m_ctrlSetupTab.InsertItem(TAPCTRL_JUDGESET+1, _T("Judge Set"));
	pFrame->m_stParamInfo.nEqpObject = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_OBJECT, 1);//2018.11.03 ADD BY MBY
	if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
		m_ctrlSetupTab.InsertItem(TAPCTRL_RECIPE+1, _T("Recipe Setting"));

	// 처음 실행시 보이는 화면은 PLC Set Up 이다.
	RECT rc;
	m_ctrlSetupTab.SetCurSel(TAPCTRL_INSPITEM);

	m_pDlgPLC->ShowWindow(SW_HIDE);
	m_pDlgLumSetup->ShowWindow(SW_HIDE);
	m_pDlgSetJudgeItem->ShowWindow(SW_HIDE);
	m_pDlgDefSetup->ShowWindow(SW_HIDE);
	m_pDlgRecipe->ShowWindow(SW_HIDE);
	m_pDlgJudgeUI->ShowWindow(SW_HIDE);

	m_pDlgSetJudgeItem->ShowWindow(SW_SHOW);
	m_pDlgSetJudgeItem->GetWindowRect(&rc);
	m_pDlgSetJudgeItem->GetClientRect(&rc);
	m_pDlgSetJudgeItem->MoveWindow(&rc);
	m_pDlgSetJudgeItem->MoveWindow(rc.left + 550, rc.top + 70, rc.right, rc.bottom);


}

void CViewSetup::OnBnClickedBtnLoadInfo()
{
	m_GridIPList->DeleteNonFixedRows();
	m_GridIPList->Refresh();

	m_fnLoadIPList(TRUE);
}

void CViewSetup::OnBnClickedBtnSaveInfo()
{
	m_fnSaveIPList();
}

void CViewSetup::OnBnClickedBtnSockStart()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_fnLoadParam(FALSE);
	//pFrame->m_pVsSeverIF->m_fnInitSock();
}

void CViewSetup::OnBnClickedBtnSockStop()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	//pFrame->m_pVsSeverIF->m_fnSockCloseAll();
}

void CViewSetup::OnBnClickedBtnSaveSockSet()
{
	//Chu
	m_fnLogin();
}

void CViewSetup::OnBnClickedBtnTestSetup()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

//	pFrame->m_pVsSeverIF->m_fnConnectToCIM();
}

void CViewSetup::OnBnClickedRadioPlc()
{
	GetDlgItem(IDC_STATIC_SOCK_MODE1)->EnableWindow(0);
	GetDlgItem(IDC_RADIO_CTRL_SOCK_PASSIVE)->EnableWindow(0);
	GetDlgItem(IDC_RADIO_CTRL_SOCK_ACTIVE)->EnableWindow(0);
}

void CViewSetup::OnBnClickedRadioPc()
{
	GetDlgItem(IDC_STATIC_SOCK_MODE1)->EnableWindow(1);
	GetDlgItem(IDC_RADIO_CTRL_SOCK_PASSIVE)->EnableWindow(1);
	GetDlgItem(IDC_RADIO_CTRL_SOCK_ACTIVE)->EnableWindow(1);
}

void CViewSetup::OnBnClickedCheckCimUse()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_CIM_USE))->GetCheck();

	GetDlgItem(IDC_STATIC_SOCK_MODE2)->EnableWindow(bCheck);
	GetDlgItem(IDC_RADIO_CIM_SOCK_PASSIVE)->EnableWindow(bCheck);
	GetDlgItem(IDC_RADIO_CIM_SOCK_ACTIVE)->EnableWindow(bCheck);
}
//2021.03.04 add by mby
void CViewSetup::OnBnClickedCheckCimUse2()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_CIM_USE2))->GetCheck();
}

void CViewSetup::OnBnClickedCheckAutodelUse()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_AUTODEL_USE))->GetCheck();
	GetDlgItem(IDC_STATIC_LOG_DAY)->EnableWindow(bCheck);
	GetDlgItem(IDC_EDIT_LOG_DEL_DAY)->EnableWindow(bCheck);
}

void CViewSetup::OnBnClickedRadioCimSockPassive()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


//Chu
void CViewSetup::m_fnLogin(void)
{
	
	if(m_pDlgLogin)
	{
	delete m_pDlgLogin;
	m_pDlgLogin = NULL;
	}
	
	/*m_pDlgLogin->Create(IDD_DLG_LOGIN, this);	
	m_pDlgLogin->ShowWindow(SW_SHOW);
	m_pDlgLogin->MoveWindow(640, 260, 230, 150);*/
	m_pDlgLogin = new CLogin();
	//m_pDlgLogin->MoveWindow(640, 260, 230, 150);
	if (m_pDlgLogin->DoModal() == IDOK)
	{
		m_fnSaveParam();
		G_AddLog(3, 2, _T("m_fnSaveParam"));	//2017.03.24
	}
}

//2015.11.05
void CViewSetup::m_fnRequestRecipeVision()
{
	G_AddLog(3, 3, _T("m_fnRequestRecipeVision() - Start"));
	CString strMsgTemp;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		pFrame->m_pVsSeverIF->m_bRecvRecipeRes[i - 1] = FALSE;
		
		if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[i - 1])
		{
			pFrame->m_pVsSeverIF->m_fnRequestVisionRecipe(i);
		}
		else
		{
			strMsgTemp.Format(_T("Connection Check PC%d"), i);
			AfxMessageBox(strMsgTemp);
		}
	}
	G_AddLog(3, 3, _T("m_fnRequestRecipeVision() - End"));
}

void CViewSetup::m_fnReadRecipePLC()
{
	G_AddLog(3, 3, _T("m_fnReadRecipePLC() - Start"));
	CString strPlcModelDataPath;
	CString strFileName, strFileFullNameTmp, strFileNameTmp, strModelNo, strModelName;
	CFileFind finder;
	BOOL bWorking;
	int nCurPos, nIdx;
	TCHAR szTmp[256];

	ZeroMemory(szTmp, 256);
	//Local PLC Model File Read

	strPlcModelDataPath.Format(_T("%s%s"),ROOT_PATH , PLC_MODEL_PATH);
	bWorking = finder.FindFile(strPlcModelDataPath + _T("*.*"));

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots()) continue;

		strFileName = finder.GetFileName();
		if (strFileName.Find(EXTENSION_DAT))
		{
			strFileFullNameTmp = strFileName;

			//ZeroMemory(szTmp, 256);
			StrCpy(szTmp, strFileFullNameTmp);
			PathRemoveExtension(szTmp); 
			strFileNameTmp = szTmp;

			AfxExtractSubString(strModelNo, strFileNameTmp, 0, '_');
			AfxExtractSubString(strModelName, strFileNameTmp, 1, '_');

			nIdx = _tstoi(strModelNo) - 1;
			m_strPLCRecipe[nIdx] = strModelName;
		}

	}
	finder.Close();
	G_AddLog(3, 3, _T("m_fnReadRecipePLC() - End"));
}

BOOL CViewSetup::m_fnRecipeResCheck()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for (int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(!pFrame->m_pVsSeverIF->m_bRecvRecipeRes[i])
			return FALSE;
	}

	return TRUE;
}


//void CViewSetup::OnBnClickedBtnRecipe()
void CViewSetup::m_fnOpenRecipe()		//2016.10.10 
{
	RECT rc;
	long nModelChangeInterlock = -1;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	nModelChangeInterlock = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_INTERLOCK);

	if (m_fnRecipeResCheck() && nModelChangeInterlock == OK)
	{
		if(m_pDlgRecipe)
		{
			delete m_pDlgRecipe;
			m_pDlgRecipe = NULL;
		}

		m_pDlgRecipe = new CDlgRecipe();
		m_pDlgRecipe->Create(IDD_DLG_RECIPE, this);
		m_pDlgRecipe->ShowWindow(SW_SHOW);

		m_pDlgRecipe->GetWindowRect(&rc);
		m_pDlgRecipe->GetClientRect(&rc);
		m_pDlgRecipe->MoveWindow(&rc);
		m_pDlgRecipe->MoveWindow(rc.left + 550, rc.top + 110, rc.right, rc.bottom);
		m_bRecipeChangeMode = TRUE;
	}
	else if(!m_fnRecipeResCheck())
	{
		AfxMessageBox(_T("Recipe Download Fail!"), MB_ICONINFORMATION | MB_OK);
	}
	else if(nModelChangeInterlock != OK)
	{
		AfxMessageBox(_T("PLC AutoRun 상태 체크"), MB_ICONINFORMATION | MB_OK);
		G_AddLog_ALARM(_T("OnAutoRecipe - PLC AutoRun Check : %d"), ERROR_PLC_SELECT_RECIPE);
	}
}


// void CViewSetup::OnBnClickedBtnSetInspItem()
// {
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 
// 	if(m_pDlgSetJudgeItem)
// 	{
// 		delete m_pDlgSetJudgeItem;
// 		m_pDlgSetJudgeItem = NULL;
// 	}
// 	m_pDlgSetJudgeItem = new CDlgSetJudgeItem();
// 	m_pDlgSetJudgeItem->Create(IDD_DLG_SET_ITEM, this);
// 	m_pDlgSetJudgeItem->ShowWindow(SW_SHOW);
// 	m_pDlgSetJudgeItem->MoveWindow(550, 50, 700, 750);	//(550, 50, 530, 750);
// }


// void CViewSetup::OnBnClickedBtnOverlapDef()
// {
// 	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
// 
// 	if(m_pDlgDefSetup)
// 	{
// 		delete m_pDlgDefSetup;
// 		m_pDlgDefSetup = NULL;
// 	}
// 	m_pDlgDefSetup = new CDlgDEFSetup();
// 	m_pDlgDefSetup->Create(IDD_DLG_DEF_SETUP, this);
// 	m_pDlgDefSetup->m_fnLoadParameter();
// 	m_pDlgDefSetup->ShowWindow(SW_SHOW);
// 	m_pDlgDefSetup->MoveWindow(550, 50, 900, 600);
// }

//2016.10.10 Mr.Chu
//Tab Cnt 제어 문
void CViewSetup::OnTcnSelchangeTabSetup(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex = m_ctrlSetupTab.GetCurSel();
	RECT rc;

	m_pDlgPLC->ShowWindow(SW_HIDE);
	m_pDlgLumSetup->ShowWindow(SW_HIDE);
	m_pDlgSetJudgeItem->ShowWindow(SW_HIDE);
	m_pDlgDefSetup->ShowWindow(SW_HIDE);
	m_pDlgRecipe->ShowWindow(SW_HIDE);
	m_pDlgJudgeUI->ShowWindow(SW_HIDE);

	if( nIndex != TAPCTRL_RECIPE )
		GetDlgItem(IDC_BTN_SAVE_SOCK_SET)->EnableWindow(TRUE);

	switch(nIndex)
	{
	case TAPCTRL_INSPITEM:
		m_pDlgSetJudgeItem->ShowWindow(SW_SHOW);

		m_pDlgSetJudgeItem->GetWindowRect(&rc);
		m_pDlgSetJudgeItem->GetClientRect(&rc);
		m_pDlgSetJudgeItem->MoveWindow(&rc);
		m_pDlgSetJudgeItem->MoveWindow(rc.left + 550, rc.top + 70, rc.right, rc.bottom);

		break;
	case TAPCTRL_LUMINANCE:
		m_pDlgLumSetup->ShowWindow(SW_SHOW);

		m_pDlgLumSetup->GetWindowRect(&rc);
		m_pDlgLumSetup->GetClientRect(&rc);
		m_pDlgLumSetup->MoveWindow(&rc);
		m_pDlgLumSetup->MoveWindow(rc.left + 570, rc.top + 100, rc.right, rc.bottom);
		break;
	case TAPCTRL_OVERLAP:
		m_pDlgPLC->ShowWindow(SW_SHOW);

		m_pDlgPLC->GetWindowRect(&rc);
		m_pDlgPLC->GetClientRect(&rc);
		m_pDlgPLC->MoveWindow(&rc);
		m_pDlgPLC->MoveWindow(rc.left + 550, rc.top + 70, rc.right, rc.bottom);
		//m_pDlgSetJudgeItem->MoveWindow(550, 50, 700, 750);	//(550, 50, 530, 750); 
		break;
	case TAPCTRL_TIME_OVERLAP:
		//m_pDlgDefSetup->m_fnLoadParameter();
		m_pDlgDefSetup->ShowWindow(SW_SHOW);

		m_pDlgDefSetup->GetWindowRect(&rc);
		m_pDlgDefSetup->GetClientRect(&rc);
		m_pDlgDefSetup->MoveWindow(&rc);
		m_pDlgDefSetup->MoveWindow(rc.left + 570, rc.top + 80, rc.right, rc.bottom);
		//m_pDlgDefSetup->MoveWindow(550, 50, 900, 600);
		break;
	case TAPCTRL_JUDGESET:
		m_pDlgJudgeUI->ShowWindow(SW_SHOW);

		m_pDlgJudgeUI->GetWindowRect(&rc);
		m_pDlgJudgeUI->GetClientRect(&rc);
		m_pDlgJudgeUI->MoveWindow(&rc);
		m_pDlgJudgeUI->MoveWindow(rc.left + 560, rc.top + 70, rc.right, rc.bottom);
		break;
	case TAPCTRL_RECIPE:
		GetDlgItem(IDC_BTN_SAVE_SOCK_SET)->EnableWindow(FALSE);
		m_fnOpenRecipe();
		break;
	}

	*pResult = 0;
}

//2017.04.29	모니터 측정 스펙 설정
//요청 담당자	: 왕빙
//사유			: 대형과 측정 처리구조가 달라 스펙을 따로 User가 설정하도록 한다.
void CViewSetup::OnBnClickedBtnMeasureSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strMeasureSpec		=_T("");
	int		nMeasureSpec		= 0;

	GetDlgItem(IDC_EDIT_MEASURE_SPEC)->GetWindowText(strMeasureSpec);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_MEASURE_SPEC, _T("Spec"), strMeasureSpec);
}


void CViewSetup::OnBnClickedBtnLifeReset1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	GetDlgItem(IDC_EDIT_LIFE_SIDE1)->SetWindowText(_T("0"));
	pFrame->m_iniConfigSet.WriteProfileStringW(_T("LIFE_COUNT"), _T("LIFE1"), _T("0"));	
}


void CViewSetup::OnBnClickedBtnLifeReset2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	GetDlgItem(IDC_EDIT_LIFE_SIDE2)->SetWindowText(_T("0"));
	pFrame->m_iniConfigSet.WriteProfileStringW(_T("LIFE_COUNT"), _T("LIFE2"),  _T("0"));	
}


void CViewSetup::OnBnClickedBtnInchChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp,strRcpNo,strName;
	TCHAR strBuffer[255];
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString cCountRoot;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, MEASURE_LIMIT_INFO_PATH);

	GetDlgItem(IDC_EDIT_INCH)->GetWindowText(strRcpNo);
	if(strRcpNo ==_T("")) return;

	for(int i=0;i<32;i++){
		if(i > 15) strName.Format(_T("TF%d"),i+1);
		else	   strName.Format(_T("CF%d"),i+1);
		GetPrivateProfileString(strRcpNo, strName, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		GetDlgItem(IDC_EDIT_MLIMIT1+i)->SetWindowText(strTemp);
	}
		
	for(int i=0;i<16;i++){		
		if(i > 7) strName.Format(_T("TF-R%d"),i+1);
		else	   strName.Format(_T("CF-R%d"),i+1);
		GetPrivateProfileString(strRcpNo, strName, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		GetDlgItem(IDC_EDIT_RANGE1+i)->SetWindowText(strTemp);
	}
	GetPrivateProfileString(strRcpNo, _T("RVLAUE"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
	strTemp = strBuffer;
	GetDlgItem(IDC_EDIT_R_VLAUE)->SetWindowText(strTemp);
	GetPrivateProfileString(strRcpNo, _T("PSIZE"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
	strTemp = strBuffer;
	GetDlgItem(IDC_EDIT_PSIZE)->SetWindowText(strTemp);
}


void CViewSetup::OnBnClickedBtnMsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp,strRcpNo,strName;
	TCHAR strBuffer[255];
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString cCountRoot;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, MEASURE_LIMIT_INFO_PATH);
	GetDlgItem(IDC_EDIT_INCH)->GetWindowText(strRcpNo);
	if(strRcpNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	for(int i=0;i<32;i++){
		GetDlgItem(IDC_EDIT_MLIMIT1+i)->GetWindowText(strTemp);
		if(i > 15) strName.Format(_T("TF%d"),i+1);
		else	   strName.Format(_T("CF%d"),i+1);
		WritePrivateProfileString(strRcpNo, strName, strTemp, cCountRoot);		
	}
	for(int i=0;i<16;i++){
		GetDlgItem(IDC_EDIT_RANGE1+i)->GetWindowText(strTemp);
		if(i > 7) strName.Format(_T("TF-R%d"),i+1);
		else	   strName.Format(_T("CF-R%d"),i+1);
		WritePrivateProfileString(strRcpNo, strName, strTemp, cCountRoot);		
	}
	GetDlgItem(IDC_EDIT_R_VLAUE)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("RVLAUE"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PSIZE)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PSIZE"), strTemp, cCountRoot);

	WritePrivateProfileString(_T("LAST"), _T("INCH"), strRcpNo, cCountRoot);		
}
void CViewSetup::m_fnInitMeasureLimitInfo()
{
	CString strTemp,strConfigPath,strRecipeNo,strName;
	TCHAR strBuffer[255];
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString cCountRoot;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, MEASURE_LIMIT_INFO_PATH);

	strConfigPath.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);

	GetPrivateProfileString(SECTION_RECIPE_NO, KEY_LAST_NO, _T(""), strBuffer, sizeof(strBuffer), strConfigPath);
	//pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);
	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	GetDlgItem(IDC_EDIT_INCH)->SetWindowText(strRecipeNo);
	pFrame->m_DataManager.m_stMeasureLimitInfo.strInch = strRecipeNo;

	GetPrivateProfileString(strRecipeNo, _T("RVLAUE"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
	strTemp = strBuffer;
	//if(strTemp==_T("")) AfxMessageBox(_T("Please Check Value!!"));
	GetDlgItem(IDC_EDIT_R_VLAUE)->SetWindowText(strTemp);
	pFrame->m_DataManager.m_stMeasureLimitInfo.nRValue = _ttoi(strTemp);
	GetPrivateProfileString(strRecipeNo, _T("PSIZE"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
	strTemp = strBuffer;
	GetDlgItem(IDC_EDIT_PSIZE)->SetWindowText(strTemp);
	pFrame->m_DataManager.m_stMeasureLimitInfo.nPanelValue = _ttoi(strTemp);

	for(int i=0;i<32;i++){
		if(i > 15) strName.Format(_T("TF%d"),i+1);
		else	   strName.Format(_T("CF%d"),i+1);
		GetPrivateProfileString(strRecipeNo, strName, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		GetDlgItem(IDC_EDIT_MLIMIT1+i)->SetWindowText(strTemp);
		if(i > 15)	pFrame->m_DataManager.m_stMeasureLimitInfo.nTFValue[i-16] = _ttoi(strTemp);
		else		pFrame->m_DataManager.m_stMeasureLimitInfo.nCFValue[i] = _ttoi(strTemp);
	}
		
	for(int i=0;i<16;i++){		
		if(i > 7) strName.Format(_T("TF-R%d"),i+1);
		else	   strName.Format(_T("CF-R%d"),i+1);
		GetPrivateProfileString(strRecipeNo, strName, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		GetDlgItem(IDC_EDIT_RANGE1+i)->SetWindowText(strTemp);
		if(i > 7)	pFrame->m_DataManager.m_stMeasureLimitInfo.nTFRStValue[i-8] = _ttoi(strTemp);
		else		pFrame->m_DataManager.m_stMeasureLimitInfo.nCFRStValue[i] = _ttoi(strTemp);
	}
	
}

void CViewSetup::OnBnClickedBtnSetLimit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fnInitMeasureLimitInfo();
}


void CViewSetup::OnBnClickedBtnLifeReset3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	GetDlgItem(IDC_EDIT_LIFE_SIDE3)->SetWindowText(_T("0"));
	pFrame->m_iniConfigSet.WriteProfileStringW(_T("LIFE_COUNT"), _T("LIFE3"),  _T("0"));	
}

void CViewSetup::OnBnClickedBtnChkall()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//PC6개 : PC1,PC3,PC5  PC9개 : PC1,PC4,PC7 // Light, Trigger 설정하는 PC는 변경시 유의가 필요하다.
	for(int i = 0 ; i < pFrame->m_stParamInfo.nCountVisionClient; i++){		
		m_bUseInspPC[i] = true;
	}
	UpdateData(FALSE);
}
