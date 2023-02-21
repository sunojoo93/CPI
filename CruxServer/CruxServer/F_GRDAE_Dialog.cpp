// F_GRDAE_Dialog.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "CruxServer.h"
#include "F_GRDAE_Dialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ThreadProcPLC.h"
#include "ViewSetup.h"

// F_GRDAE_Dialog 대화 상자입니다.
IMPLEMENT_DYNAMIC(F_GRDAE_Dialog, CDialogEx)


F_GRDAE_Dialog::F_GRDAE_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(F_GRDAE_Dialog::IDD, pParent)
{
	opid_strValue = _T("");
}

F_GRDAE_Dialog::~F_GRDAE_Dialog()
{
}

void F_GRDAE_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(F_GRDAE_Dialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &F_GRDAE_Dialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &F_GRDAE_Dialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &F_GRDAE_Dialog::OnBnClickedCancel)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// SET 버튼 클릭 시.
void F_GRDAE_Dialog::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strOpidValue =_T("");
	CString strValue = _T("");
	
	char chCompulsionJudge = 0;

	GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);	
	GetDlgItem(IDC_EDIT_GRADE_OPID)->GetWindowText(opid_strValue);	
	
	if( opid_strValue.IsEmpty()  == TRUE)
	{
		AfxMessageBox(_T("you check Value(OPID NULL)"));
	}
	else
	{
		GetDlgItem(IDC_EDIT_GRADE_OPID)->GetWindowText(strOpidValue);
		int nValue = strOpidValue.GetLength();	
		if(nValue > 5)
		{
			AfxMessageBox(_T("Please Input the Operator ID under 5 Number"), MB_ICONINFORMATION);
		}
		else
		{
			opid_set_flag = TRUE;
		}		
	}

	if(strValue.IsEmpty() == TRUE)
	{
		AfxMessageBox(_T("you check Value(GRADE NULL)"));
	}
	else
	{	
		strValue.MakeUpper();	// 대문자로 변경
		chCompulsionJudge = strValue.GetAt(0);	// 첫번째 char 획득

		if (isalpha(chCompulsionJudge))
		{
			if (chCompulsionJudge >= 'A' && chCompulsionJudge <= 'C')
			{
				// A~D의 정상값이 입력되었다면 입력란 다시 셋팅
				strValue.Format(_T("%c"), chCompulsionJudge);
				GetDlgItem(IDC_EDIT1)->SetWindowText(strValue);
				// 확인 메시지 출력
				AfxMessageBox(_T("[")+strValue+_T("]")+_T(" Set Click"), MB_OK);
				juduge_set_flag = TRUE;
			}
			else
			{
				// 비정상값 메시지 출력
				AfxMessageBox(_T("Not Allowed([A~C]Allow)"));
			}
		}
	}	
}

// OK 클릭 시 강제 판정 적용_ ryu
void F_GRDAE_Dialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	G_AddLog(3, 3, _T("OnBnClickedOk() - Start"));
	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());	
	// PLC 경알람 해제.
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	CString strValue = _T("");
	CString strRankValue = _T("");	//2020.08.17 add by mby
	BOOL bNgFlag = FALSE;	//2020.08.17 add by mby
	char chCompulsionJudge = 0;

	GetDlgItem(IDC_EDIT_FO_RANK)->GetWindowText(strRankValue);	//2020.08.17 add by mby
	GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
	chCompulsionJudge = strValue.GetAt(0);	// 첫번째 char 획득

		// 예외상황 리턴 -> Text 입력에 대문자, 숫자기입 관련 속성에서 처리.
		// Set 안누르고 OK 누를 때, 예외 적용.
		//if (chCompulsionJudge == '\0') return;	
	if ( opid_set_flag==TRUE && juduge_set_flag == TRUE)
		{		
			STRU_NG_FLAG_INFO stNgFlagInfo_Panel1;
			stNgFlagInfo_Panel1.init_info();
	
		int nGradeCode = 0;

		// 판넬 ID 획득
		CString strPanelID1 = _T("");
		pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_ABI_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, strPanelID1);

		// A ~ D등급 판정 정보
		if (chCompulsionJudge == 'A')
		{
			nGradeCode = NG_FLAG_A_GRADE;
		}
		else if (chCompulsionJudge == 'B')
		{
			nGradeCode = NG_FLAG_AVL_B_GRADE;
		}
		else //if (chCompulsionJudge == 'C')
		{
			nGradeCode = NG_FLAG_C_GRADE;
		}

		// 등급 기록 강제 판정 B로 됨. //2015.09.17 change 1 -> 5 -> 1
		// NG_FLAG_A_GRADE;		//0 : A Grade    NG_FLAG_AVL_B_GRADE;	//5 : B Grade
		// NG_FLAG_C_GRADE;		//6 : C Grade    NG_FLAG_D_GRADE;		//7 : D Grade	
		G_AddLog(2, 3, _T("NG FLAG [COMPULSION] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_ONE PanelID1 : %s || NG Flag = %d")
				, strPanelID1, nGradeCode);	// NG_FLAG_AVL_B_GRADE

		// TEST LOG
		G_AddLog(3, 0, _T("OnBnClickedOk() PanelID1 : %s || NG Flag = %d"), strPanelID1, nGradeCode);
		strPanelID1.TrimRight();
		
		// 강제 판정값 Write 해주기
		stNgFlagInfo_Panel1.ForceDefaultData(strPanelID1, nGradeCode);
		m_pDataManager->m_fnMakeQMSByPassData_AVL_Force(strPanelID1, strValue, opid_strValue, strRankValue);	//2020.08.17 add by mby
		bNgFlag = pFrame->m_DataManager.m_fnNgFlagInfoCheck(nGradeCode);	//2020.08.17 add by mby
		stNgFlagInfo_Panel1.nNgFlag = bNgFlag;								//2020.08.17 add by mby
		pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
		
		//PLC 경알람 해제.
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_JUDGE_SIGNAL, 0);
		juduge_set_flag =0;
	}
	else
	{
		// 비정상값 메시지 출력
		AfxMessageBox(_T("Set Button Click Please~"));
		return;
	}	

	G_AddLog(3, 3, _T("OnBnClickedOk() - End"));
	CDialogEx::OnOK();
}

void F_GRDAE_Dialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}

HBRUSH F_GRDAE_Dialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ 
	// TODO:  여기서 DC의 특성을 변경합니다.
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
		
	CRect rect;
	UINT nID = pWnd->GetDlgCtrlID();
	if(nID == IDC_STATIC_COLOR_BAR)
    {		
        pDC->SetTextColor(RGB(0, 0, 255));  // 글자 색      
		// Static 글자 크기 조절
		m_static_font.CreateFont(40, // nHeight 
                               11, // nWidth 
                               0, // nEscapement 
                               0, // nOrientation 
                               1, // nWeight 
                               0, // bItalic 
                               0, // bUnderline 
                               0, // cStrikeOut 
                               0, // nCharSet 
                               OUT_DEFAULT_PRECIS, // nOutPrecision 
                               0,                              // nClipPrecision 
                               DEFAULT_QUALITY,       // nQuality
                               DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily 
                               _T("Fixedsys")); // lpszFacename 

		CFont *Odfont = (CFont*)pDC->SelectObject(&m_static_font);

		return (HBRUSH)::GetStockObject(NULL_BRUSH);
    }
	else if ( nID == IDC_STATIC_OPID)
	{
		pDC->SetTextColor(RGB(255, 0, 0));  // 글자 색      
		// Static 글자 크기 조절
		m_static_opid.CreateFont(40, // nHeight 
                               11, // nWidth 
                               0, // nEscapement 
                               0, // nOrientation 
                               1, // nWeight 
                               0, // bItalic 
                               0, // bUnderline 
                               0, // cStrikeOut 
                               0, // nCharSet 
                               OUT_DEFAULT_PRECIS,				// nOutPrecision 
                               0,                               // nClipPrecision 
                               DEFAULT_QUALITY,					// nQuality
                               DEFAULT_PITCH | FF_DONTCARE,		// nPitchAndFamily 
                               _T("Fixedsys")); // lpszFacename 

		CFont *Odfont = (CFont*)pDC->SelectObject(&m_static_opid);

		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

BOOL F_GRDAE_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CEdit *) GetDlgItem(IDC_EDIT1))->SetLimitText(1); // 입력 갯수 1	
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	((CEdit *) GetDlgItem(IDC_EDIT_GRADE_OPID))->SetLimitText(6); // input : 6	
	g_editFont.CreatePointFont( 250, TEXT( "Fixedsys" ) );
	g_edit_opid.CreatePointFont( 250, TEXT( "Fixedsys" ) );
	GetDlgItem( IDC_EDIT1 )->SetFont( &g_editFont );
	GetDlgItem( IDC_EDIT_GRADE_OPID )->SetFont( &g_edit_opid );
	GetDlgItem( IDC_EDIT_FO_RANK )->SetFont( &g_edit_opid );	//2020.08.17 add by mby

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	m_pDataManager = &pFrame->m_DataManager;	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
