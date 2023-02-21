// DlgPanelCodeTest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgPanelCodeTest.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewAuto.h"

// CDlgPanelCodeTest 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgPanelCodeTest, CDialogEx)

CDlgPanelCodeTest::CDlgPanelCodeTest(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPanelCodeTest::IDD, pParent)
{

}

CDlgPanelCodeTest::~CDlgPanelCodeTest()
{
}

void CDlgPanelCodeTest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPanelCodeTest, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_INIT, &CDlgPanelCodeTest::OnBnClickedBtnInit)
	ON_BN_CLICKED(IDC_BTN_SET, &CDlgPanelCodeTest::OnBnClickedBtnSet)
	ON_BN_CLICKED(IDC_BTN_ACTION, &CDlgPanelCodeTest::OnBnClickedBtnAction)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgPanelCodeTest::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_EXIT, &CDlgPanelCodeTest::OnBnClickedBtnExit)
END_MESSAGE_MAP()


// CDlgPanelCodeTest 메시지 처리기입니다.


void CDlgPanelCodeTest::OnBnClickedBtnInit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetDlgItemInt(IDC_EDIT_TOTAL_PANEL,40);
	SetDlgItemInt(IDC_EDIT_JUDGE_CODE,3399);
	SetDlgItemInt(IDC_EDIT_TIME_LIMITE,3600);
	SetDlgItemInt(IDC_EDIT_FIRST_DAT,0);
	SetDlgItemInt(IDC_EDIT_SECOND_DAT,0);
	SetDlgItemInt(IDC_EDIT_THIRD_DAT,0);
	SetDlgItemInt(IDC_EDIT_FOURTH_DAT,0);
	
	SetDlgItemInt(IDC_STATIC_FIRST_DAT,0);
	SetDlgItemInt(IDC_STATIC_SECOND_DAT,0);
	SetDlgItemInt(IDC_STATIC_THIRD_DAT,0);
	SetDlgItemInt(IDC_STATIC_FOURTH_DAT,0);
}


void CDlgPanelCodeTest::OnBnClickedBtnSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//pFrame->m_stPanelCodeTest.init();

	pFrame->m_DataManager.m_stPanelCodeTest.nTotalPanel	= GetDlgItemInt(IDC_EDIT_TOTAL_PANEL);
	pFrame->m_DataManager.m_stPanelCodeTest.nJudgeCode	= GetDlgItemInt(IDC_EDIT_JUDGE_CODE);
	pFrame->m_DataManager.m_stPanelCodeTest.nTimeLimit	= GetDlgItemInt(IDC_EDIT_TIME_LIMITE);
	pFrame->m_DataManager.m_stPanelCodeTest.nFirstDat		= GetDlgItemInt(IDC_EDIT_FIRST_DAT);
	pFrame->m_DataManager.m_stPanelCodeTest.nSecondDat	= GetDlgItemInt(IDC_EDIT_SECOND_DAT);
	pFrame->m_DataManager.m_stPanelCodeTest.nThirdDat		= GetDlgItemInt(IDC_EDIT_THIRD_DAT);
	pFrame->m_DataManager.m_stPanelCodeTest.nFourthDat	= GetDlgItemInt(IDC_EDIT_FOURTH_DAT);

	GetDlgItem(IDC_BTN_ACTION)->EnableWindow(TRUE);
}


void CDlgPanelCodeTest::OnBnClickedBtnAction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strRoot = _T("") , strValue = _T("");
	
	//
	GetDlgItem(IDC_BTN_ACTION)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);

	GetDlgItem(IDC_BTN_INIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_SET)->EnableWindow(FALSE);
	//
	GetDlgItem(IDC_EDIT_TOTAL_PANEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_JUDGE_CODE)->EnableWindow(FALSE);
	//GetDlgItem(IDC_EDIT_TIME_LIMITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FIRST_DAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_SECOND_DAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_THIRD_DAT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FOURTH_DAT)->EnableWindow(FALSE);
	//

	pFrame->m_DataManager.m_stPanelCodeTest.bTestActive	= TRUE;
	pFrame->m_DataManager.m_stPanelCodeTest.nDatCnt		= NULL;
	pFrame->m_DataManager.m_stPanelCodeTest.tTimmer		= CTime::GetCurrentTime();

	// Mode Setting
	pFrame->m_DataManager.m_stPanelCodeTest.bJudgeInfo = TRUE;
	pFrame->m_DataManager.m_fnWritePanelCodeData(pFrame->m_DataManager.m_stPanelCodeTest);
	//
}


void CDlgPanelCodeTest::OnBnClickedBtnStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	pFrame->m_DataManager.m_stPanelCodeTest.bTestActive	= FALSE;

	//
	GetDlgItem(IDC_BTN_ACTION)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_INIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SET)->EnableWindow(TRUE);
	
	GetDlgItem(IDC_EDIT_TOTAL_PANEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_JUDGE_CODE)->EnableWindow(TRUE);
	//GetDlgItem(IDC_EDIT_TIME_LIMITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_FIRST_DAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_SECOND_DAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_THIRD_DAT)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_FOURTH_DAT)->EnableWindow(TRUE);
	//

	// Mode Setting
	pFrame->m_DataManager.m_stPanelCodeTest.bJudgeInfo = FALSE;
	pFrame->m_DataManager.m_fnWritePanelCodeData(pFrame->m_DataManager.m_stPanelCodeTest);
	//
}


void CDlgPanelCodeTest::OnBnClickedBtnExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	DestroyWindow();

	if(pAuto->m_pDlgPanelCodeTest)
	{
		delete pAuto->m_pDlgPanelCodeTest;
		pAuto->m_pDlgPanelCodeTest = NULL;
	}
}


BOOL CDlgPanelCodeTest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
 	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	pFrame->m_DataManager.m_fnReadPanelCodeData(&pFrame->m_DataManager.m_stPanelCodeTest);

	if(	pFrame->m_DataManager.m_stPanelCodeTest.bTestActive )
	{
		//
		GetDlgItem(IDC_BTN_ACTION)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);

		GetDlgItem(IDC_BTN_INIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SET)->EnableWindow(FALSE);
		//
		GetDlgItem(IDC_EDIT_TOTAL_PANEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_JUDGE_CODE)->EnableWindow(FALSE);
		//GetDlgItem(IDC_EDIT_TIME_LIMITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FIRST_DAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SECOND_DAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_THIRD_DAT)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FOURTH_DAT)->EnableWindow(FALSE);
		//

	}
	else
	{
		//
		GetDlgItem(IDC_BTN_ACTION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_INIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_SET)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_TOTAL_PANEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_JUDGE_CODE)->EnableWindow(TRUE);
		//GetDlgItem(IDC_EDIT_TIME_LIMITE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_FIRST_DAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_SECOND_DAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_THIRD_DAT)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_FOURTH_DAT)->EnableWindow(TRUE);
		//
	}

	SetDlgItemInt(IDC_EDIT_TOTAL_PANEL,pFrame->m_DataManager.m_stPanelCodeTest.nTotalPanel);
	SetDlgItemInt(IDC_EDIT_JUDGE_CODE,pFrame->m_DataManager.m_stPanelCodeTest.nJudgeCode);
	//SetDlgItemInt(IDC_EDIT_TIME_LIMITE,pFrame->m_stPanelCodeTest.nTimeLimit);
	SetDlgItemInt(IDC_EDIT_FIRST_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nFirstDat);
	SetDlgItemInt(IDC_EDIT_SECOND_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nSecondDat);
	SetDlgItemInt(IDC_EDIT_THIRD_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nThirdDat);
	SetDlgItemInt(IDC_EDIT_FOURTH_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nFourthDat);

	SetDlgItemInt(IDC_STATIC_FIRST_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nFirstDatJudge);
	SetDlgItemInt(IDC_STATIC_SECOND_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nSecondDatJudge);
	SetDlgItemInt(IDC_STATIC_THIRD_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nThirdDatJudge);
	SetDlgItemInt(IDC_STATIC_FOURTH_DAT,pFrame->m_DataManager.m_stPanelCodeTest.nFourthDatJudge);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
