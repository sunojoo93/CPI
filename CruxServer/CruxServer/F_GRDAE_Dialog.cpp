// F_GRDAE_Dialog.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "CruxServer.h"
#include "F_GRDAE_Dialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ThreadProcPLC.h"
#include "ViewSetup.h"

// F_GRDAE_Dialog ��ȭ �����Դϴ�.
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


// SET ��ư Ŭ�� ��.
void F_GRDAE_Dialog::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		strValue.MakeUpper();	// �빮�ڷ� ����
		chCompulsionJudge = strValue.GetAt(0);	// ù��° char ȹ��

		if (isalpha(chCompulsionJudge))
		{
			if (chCompulsionJudge >= 'A' && chCompulsionJudge <= 'C')
			{
				// A~D�� ������ �ԷµǾ��ٸ� �Է¶� �ٽ� ����
				strValue.Format(_T("%c"), chCompulsionJudge);
				GetDlgItem(IDC_EDIT1)->SetWindowText(strValue);
				// Ȯ�� �޽��� ���
				AfxMessageBox(_T("[")+strValue+_T("]")+_T(" Set Click"), MB_OK);
				juduge_set_flag = TRUE;
			}
			else
			{
				// ������ �޽��� ���
				AfxMessageBox(_T("Not Allowed([A~C]Allow)"));
			}
		}
	}	
}

// OK Ŭ�� �� ���� ���� ����_ ryu
void F_GRDAE_Dialog::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	G_AddLog(3, 3, _T("OnBnClickedOk() - Start"));
	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());	
	// PLC ��˶� ����.
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	CString strValue = _T("");
	CString strRankValue = _T("");	//2020.08.17 add by mby
	BOOL bNgFlag = FALSE;	//2020.08.17 add by mby
	char chCompulsionJudge = 0;

	GetDlgItem(IDC_EDIT_FO_RANK)->GetWindowText(strRankValue);	//2020.08.17 add by mby
	GetDlgItem(IDC_EDIT1)->GetWindowText(strValue);
	chCompulsionJudge = strValue.GetAt(0);	// ù��° char ȹ��

		// ���ܻ�Ȳ ���� -> Text �Է¿� �빮��, ���ڱ��� ���� �Ӽ����� ó��.
		// Set �ȴ����� OK ���� ��, ���� ����.
		//if (chCompulsionJudge == '\0') return;	
	if ( opid_set_flag==TRUE && juduge_set_flag == TRUE)
		{		
			STRU_NG_FLAG_INFO stNgFlagInfo_Panel1;
			stNgFlagInfo_Panel1.init_info();
	
		int nGradeCode = 0;

		// �ǳ� ID ȹ��
		CString strPanelID1 = _T("");
		pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_ABI_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, strPanelID1);

		// A ~ D��� ���� ����
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

		// ��� ��� ���� ���� B�� ��. //2015.09.17 change 1 -> 5 -> 1
		// NG_FLAG_A_GRADE;		//0 : A Grade    NG_FLAG_AVL_B_GRADE;	//5 : B Grade
		// NG_FLAG_C_GRADE;		//6 : C Grade    NG_FLAG_D_GRADE;		//7 : D Grade	
		G_AddLog(2, 3, _T("NG FLAG [COMPULSION] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_ONE PanelID1 : %s || NG Flag = %d")
				, strPanelID1, nGradeCode);	// NG_FLAG_AVL_B_GRADE

		// TEST LOG
		G_AddLog(3, 0, _T("OnBnClickedOk() PanelID1 : %s || NG Flag = %d"), strPanelID1, nGradeCode);
		strPanelID1.TrimRight();
		
		// ���� ������ Write ���ֱ�
		stNgFlagInfo_Panel1.ForceDefaultData(strPanelID1, nGradeCode);
		m_pDataManager->m_fnMakeQMSByPassData_AVL_Force(strPanelID1, strValue, opid_strValue, strRankValue);	//2020.08.17 add by mby
		bNgFlag = pFrame->m_DataManager.m_fnNgFlagInfoCheck(nGradeCode);	//2020.08.17 add by mby
		stNgFlagInfo_Panel1.nNgFlag = bNgFlag;								//2020.08.17 add by mby
		pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
		
		//PLC ��˶� ����.
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_JUDGE_SIGNAL, 0);
		juduge_set_flag =0;
	}
	else
	{
		// ������ �޽��� ���
		AfxMessageBox(_T("Set Button Click Please~"));
		return;
	}	

	G_AddLog(3, 3, _T("OnBnClickedOk() - End"));
	CDialogEx::OnOK();
}

void F_GRDAE_Dialog::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}

HBRUSH F_GRDAE_Dialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{ 
	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
		
	CRect rect;
	UINT nID = pWnd->GetDlgCtrlID();
	if(nID == IDC_STATIC_COLOR_BAR)
    {		
        pDC->SetTextColor(RGB(0, 0, 255));  // ���� ��      
		// Static ���� ũ�� ����
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
		pDC->SetTextColor(RGB(255, 0, 0));  // ���� ��      
		// Static ���� ũ�� ����
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
	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}

BOOL F_GRDAE_Dialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	((CEdit *) GetDlgItem(IDC_EDIT1))->SetLimitText(1); // �Է� ���� 1	
	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	((CEdit *) GetDlgItem(IDC_EDIT_GRADE_OPID))->SetLimitText(6); // input : 6	
	g_editFont.CreatePointFont( 250, TEXT( "Fixedsys" ) );
	g_edit_opid.CreatePointFont( 250, TEXT( "Fixedsys" ) );
	GetDlgItem( IDC_EDIT1 )->SetFont( &g_editFont );
	GetDlgItem( IDC_EDIT_GRADE_OPID )->SetFont( &g_edit_opid );
	GetDlgItem( IDC_EDIT_FO_RANK )->SetFont( &g_edit_opid );	//2020.08.17 add by mby

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	m_pDataManager = &pFrame->m_DataManager;	

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}
