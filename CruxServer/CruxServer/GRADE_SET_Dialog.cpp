// GRADE_SET_Dialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "GRADE_SET_Dialog.h"
#include "afxdialogex.h"
#include "DataManager.h"
#include "DefineVal_BIG.h"
#include "MainFrm.h"
#include "ViewAuto.h"

// GRADE_SET_Dialog 대화 상자입니다.
IMPLEMENT_DYNAMIC(GRADE_SET_Dialog, CDialog)

GRADE_SET_Dialog::GRADE_SET_Dialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(GRADE_SET_Dialog::IDD, pParent)
{
	m_pDlgGradeRankSet = NULL;	//2020.07.22 add by mby
}
GRADE_SET_Dialog::~GRADE_SET_Dialog()
{
	if(m_pDlgGradeRankSet)
	{
		delete m_pDlgGradeRankSet;
		m_pDlgGradeRankSet = NULL;
	}
}
void GRADE_SET_Dialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO1, m_radio_button1);
	DDV_MinMaxInt(pDX, m_radio_button1, 0, 5);
	DDX_Radio(pDX, IDC_RADIO2, m_radio_button2);
	DDV_MinMaxInt(pDX, m_radio_button2, 0, 5);
	DDX_Radio(pDX, IDC_RADIO3, m_radio_button3);
	DDV_MinMaxInt(pDX, m_radio_button3, 0, 5);
	DDX_Radio(pDX, IDC_RADIO4, m_radio_button4);
	DDV_MinMaxInt(pDX, m_radio_button4, 0, 5);
	DDX_Radio(pDX, IDC_RADIO5, m_radio_button5);
	DDV_MinMaxInt(pDX, m_radio_button5, 0, 5);
	DDX_Radio(pDX, IDC_RADIO6, m_radio_button6);
	DDV_MinMaxInt(pDX, m_radio_button6, 0, 5);
	DDX_Radio(pDX, IDC_RADIO7, m_radio_button7);
	DDV_MinMaxInt(pDX, m_radio_button7, 0, 5);
	DDX_Radio(pDX, IDC_RADIO8, m_radio_button8);
	DDV_MinMaxInt(pDX, m_radio_button8, 0, 5);
	DDX_Radio(pDX, IDC_RADIO9, m_radio_button9);
	DDV_MinMaxInt(pDX, m_radio_button9, 0, 5);
	DDX_Radio(pDX, IDC_RADIO10, m_radio_button10);
	DDV_MinMaxInt(pDX, m_radio_button10, 0, 5);
	DDX_Radio(pDX, IDC_RADIO32, m_radio_button11);  //2019.01.12 by ryu 추가
	DDV_MinMaxInt(pDX, m_radio_button11, 0, 5);
	DDX_Radio(pDX, IDC_RADIO36, m_radio_button12);  //2019.01.12 by ryu Aß°¡
	DDV_MinMaxInt(pDX, m_radio_button12, 0, 5);
	DDX_Radio(pDX, IDC_RADIO39, m_radio_button13);  //2019.01.12 by ryu Aß°¡
	DDV_MinMaxInt(pDX, m_radio_button13, 0, 5);
	DDX_Radio(pDX, IDC_RADIO42, m_radio_button14);  //2019.01.12 by ryu Aß°¡
	DDV_MinMaxInt(pDX, m_radio_button14, 0, 5);
	DDX_Radio(pDX, IDC_RADIO45, m_radio_button15);  //2020.02.13 by mby
	DDV_MinMaxInt(pDX, m_radio_button15, 0, 5);

	DDX_Control(pDX, IDC_EDIT_TFTA, m_edit_tft_cf[0]);
	DDX_Control(pDX, IDC_EDIT_TFTB, m_edit_tft_cf[1]);
	DDX_Control(pDX, IDC_EDIT_TFTC, m_edit_tft_cf[2]);
	DDX_Control(pDX, IDC_EDIT_TFTD, m_edit_tft_cf[3]);

	DDX_Control(pDX, IDC_EDIT_CFA, m_edit_tft_cf[4]);
	DDX_Control(pDX, IDC_EDIT_CFB, m_edit_tft_cf[5]);
	DDX_Control(pDX, IDC_EDIT_CFC, m_edit_tft_cf[6]);
	DDX_Control(pDX, IDC_EDIT_CFD, m_edit_tft_cf[7]);

	DDX_Control(pDX, IDC_EDIT_CFCORBROKEN, m_edit_tft_cf[8]);	//2020.02.13 by mby

	DDX_Control(pDX, IDC_EDIT_RANK1, m_rank_edit[0]);  //2019.01.15 by ryu Aß°¡ S
	DDX_Control(pDX, IDC_EDIT_RANK2, m_rank_edit[1]);
	DDX_Control(pDX, IDC_EDIT_RANK3, m_rank_edit[2]);
	DDX_Control(pDX, IDC_EDIT_RANK4, m_rank_edit[3]);
	DDX_Control(pDX, IDC_EDIT_RANK5, m_rank_edit[4]);
	DDX_Control(pDX, IDC_EDIT_RANK6, m_rank_edit[5]);
	DDX_Control(pDX, IDC_EDIT_RANK7, m_rank_edit[6]);
	DDX_Control(pDX, IDC_EDIT_RANK8, m_rank_edit[7]);
	DDX_Control(pDX, IDC_EDIT_RANK9, m_rank_edit[8]);
	DDX_Control(pDX, IDC_EDIT_RANK10, m_rank_edit[9]);
	DDX_Control(pDX, IDC_EDIT_RANK11, m_rank_edit[10]);  //2019.01.15 by ryu Aß°¡ E
	DDX_Control(pDX, IDC_EDIT_RANK12, m_rank_edit[11]);
	
	DDX_Control(pDX, IDC_EDIT_RANK13, m_rank_edit[12]);  //2019.01.28 by ryu Aß°¡ S
	DDX_Control(pDX, IDC_EDIT_RANK14, m_rank_edit[13]);  //2019.01.28 by ryu Aß°¡ E

	DDX_Control(pDX, IDC_EDIT_RANK15, m_rank_edit[14]);  //2020.02.13 by mby

	DDX_Text(pDX, IDC_EDIT_GRADESET_RCP_CHANGE, m_strRepNo);	//2020.07.01 add by mby
}

BEGIN_MESSAGE_MAP(GRADE_SET_Dialog, CDialog)
	ON_BN_CLICKED(IDC_SET_BTN1, &GRADE_SET_Dialog::OnBnClickedSetBtn1)
	ON_BN_CLICKED(IDOK, &GRADE_SET_Dialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &GRADE_SET_Dialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_GRADESET_RCP_CHANGE, &GRADE_SET_Dialog::OnBnClickedBtnGradesetRcpChange)	//2020.07.01 add by mby
	ON_BN_CLICKED(IDC_BTN_GRADERANK_OPEN, &GRADE_SET_Dialog::OnBnClickedBtnGraderankOpen)
END_MESSAGE_MAP()

//Setting button_1
void GRADE_SET_Dialog::OnBnClickedSetBtn1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(true);
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CString strTemp,strTemp1,strRcpNo;

	GetDlgItem(IDC_EDIT_GRADESET_RCP_CHANGE)->GetWindowText(strRcpNo);
	if(strRcpNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}

	int m_radio_button_arr[]=
	{
		m_radio_button1,
		m_radio_button2,
		m_radio_button3,
		m_radio_button4,
		m_radio_button5,
		m_radio_button6,
		m_radio_button7,
		m_radio_button8,		
		m_radio_button9,
		m_radio_button10,
		m_radio_button11, // 2019.01.12 by ryu
		m_radio_button12,
		m_radio_button13,  // 2019.01.30 by ryu 
		m_radio_button14,
		m_radio_button15	// 2020.02.13 by mby 
	};

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
	_T("ETC_POINT"),  // 2019.01.12 by ryu
	_T("GRADE_ALIGN_FAIL"),  // 2019.01.18 by ryu
	_T("TFT_BROKEN_VAL"),
	_T("CF_BROKEN_VAL"),
	_T("CF_COR_BROKEN_VAL")	// 2020.02.13 by mby 
	};

	for(int i = 0; i < (MAX_COUNT_GRADE); i++)
	{
		switch(m_radio_button_arr[i])  
		{
			case 0 :			
				//AfxMessageBox(_T("1번째 A등급 표시"));			
				m_strValue_Grade[i] = JUDGE_A;	
				break;
			case 1 :
				//AfxMessageBox(_T("1번째 B등급 표시"));
				m_strValue_Grade[i] = JUDGE_B;			
				break;
			case 2 :
				//AfxMessageBox(_T("1번째 C등급 표시"));
				m_strValue_Grade[i] = JUDGE_C;			
				break;
		}	  
		//strKey = arrKey[i];     // 키
		WritePrivateProfileString( _T("GRADE_INPUT"), arrKey[i], m_strValue_Grade[i], GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		pAuto->m_strValue_GradeAuto[i]= m_strValue_Grade[i];
	}	
	AfxMessageBox(_T("Check SET, GRADE OK"));	
	

	CString strkey_Rank[]=
	{
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

	for(int j = 0; j < (MAX_COUNT_RANK); j++)
	{
		m_rank_edit[j].GetWindowText(m_strValue_Rank[j]);
		WritePrivateProfileString(_T("RANK_VALUE"), strkey_Rank[j], m_strValue_Rank[j], GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		pAuto->m_strValue_RankAuto[j] = m_strValue_Rank[j];
	}
}

void GRADE_SET_Dialog::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 수치 저장.	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CString strkey_arr[]=
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
	for(int i = 0; i < VALUE_COUNT; i++)
	{
		m_edit_tft_cf[i].GetWindowText(m_strValue_Num[i]);
		WritePrivateProfileString(_T("GRADE_VALUE"), strkey_arr[i], m_strValue_Num[i], GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		pAuto->m_strValue_NumAuto[i] = m_strValue_Num[i];
	}

	CString arrKey_rank[] = 
	{
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

	for(int j = 0; j < (MAX_COUNT_RANK) /*rank 2°³ Aß°¡*/; j++)
	{
		m_rank_edit[j].GetWindowText(m_strValue_Rank[j]);
		WritePrivateProfileString(_T("RANK_VALUE"), arrKey_rank[j], m_strValue_Rank[j], GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		pAuto->m_strValue_RankAuto[j] = m_strValue_Rank[j];
	}

	AfxMessageBox(_T("Check OK"));	
	CDialog::OnOK();
}

void GRADE_SET_Dialog::OnBnClickedCancel()
{
	CDialogEx::OnCancel();	//20181220 by mby
}

BOOL GRADE_SET_Dialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strTemp,cCountRoot,strRecipeNo;
	int nRead = 0;
	TCHAR strBuffer[255];

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	//Recipe No check
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);
	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	m_strRepNo = strRecipeNo;
	GetDlgItem(IDC_EDIT_GRADESET_RCP_CHANGE)->SetWindowText(m_strRepNo);

	// TODO:  ¿ⓒ±a¿¡ Aß°¡ AE±aE­ AU¾÷A≫ Aß°¡CO´I´U.
	((CEdit *) GetDlgItem(IDC_EDIT_RANK1))->SetLimitText(3);			  // input : 3		
	((CEdit *) GetDlgItem(IDC_EDIT_RANK2))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK3))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK4))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK5))->SetLimitText(3);

	((CEdit *) GetDlgItem(IDC_EDIT_RANK6))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK7))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK8))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK9))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK10))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK11))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK12))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK13))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK14))->SetLimitText(3);
	((CEdit *) GetDlgItem(IDC_EDIT_RANK15))->SetLimitText(3);	//2020.02.13 add by mby

	strLogFolder = GRADE_SETTING_FOLDER_PATH;	
	if( !fileFind.FindFile(strLogFolder) )	// 폴더가 없을 시,
	{
		CreateDirectory(strLogFolder,NULL);	// 폴더 생성
	}	

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
		_T("TFT_BROKEN_VAL"),  //13
		_T("CF_BROKEN_VAL"),  //14
		_T("CF_COR_BROKEN_VAL")	// 2020.02.13 by mby 
	};

		CString arrKey_rank[] = {
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
		_T("CF_RBROKEN_VAL"),  //14
		_T("CF_COR_RBROKEN_VAL")	//2020.02.13 add by mby
	};

	int arrRadioA[] = {
		IDC_RADIO1,
		IDC_RADIO2,
		IDC_RADIO3,
		IDC_RADIO4,
		IDC_RADIO5,
		IDC_RADIO6,
		IDC_RADIO7,
		IDC_RADIO8,
		IDC_RADIO9,
		IDC_RADIO10,
		IDC_RADIO32,
		IDC_RADIO36,
		IDC_RADIO39,
		IDC_RADIO42,
		IDC_RADIO45
	};

	int arrRadioB[] = {
		IDC_RADIO11,
		IDC_RADIO12,
		IDC_RADIO13,
		IDC_RADIO14,
		IDC_RADIO15,
		IDC_RADIO16,
		IDC_RADIO17,
		IDC_RADIO18,
		IDC_RADIO19,
		IDC_RADIO20,
		IDC_RADIO34,
		IDC_RADIO37,
		IDC_RADIO40,
		IDC_RADIO43,
		IDC_RADIO46
	};

	int arrRadioC[] = {
		IDC_RADIO21,
		IDC_RADIO22,
		IDC_RADIO23,
		IDC_RADIO24,
		IDC_RADIO25,
		IDC_RADIO26,
		IDC_RADIO27,
		IDC_RADIO28,
		IDC_RADIO29,
		IDC_RADIO30,
		IDC_RADIO35,
		IDC_RADIO38,
		IDC_RADIO41,
		IDC_RADIO44,
		IDC_RADIO47
	};

	TCHAR szBuf[MAX_PATH]={0,};

	for(int i = 0; i < (MAX_COUNT_GRADE); i++)  // BROKEN_VAL 추가
	{
		GetPrivateProfileString(_T("GRADE_INPUT"), arrKey[i], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Grade[i].Format(_T("%s"), szBuf); // 값

		if(m_strValue_Grade[i] ==  JUDGE_A)
		{
			((CButton*)GetDlgItem(arrRadioA[i]))->SetCheck(1);
		}
		else if(m_strValue_Grade[i] ==  JUDGE_B)
		{
			((CButton*)GetDlgItem(arrRadioB[i]))->SetCheck(1);
		}
		else
		{
			((CButton*)GetDlgItem(arrRadioC[i]))->SetCheck(1);
		}
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
		GetPrivateProfileString(_T("GRADE_VALUE"), strkey[j], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Num[j].Format(_T("%s"), szBuf);
		m_edit_tft_cf[j].SetWindowTextW(m_strValue_Num[j]);
	}
	
	for(int k =0; k < (MAX_COUNT_RANK);/*RANK 2°³ Aß°¡*/ k++)
	{
		GetPrivateProfileString(_T("RANK_VALUE"), arrKey_rank[k], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Rank[k].Format(_T("%s"), szBuf);
		m_rank_edit[k].SetWindowTextW(m_strValue_Rank[k]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void GRADE_SET_Dialog::OnBnClickedBtnGradesetRcpChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strRecipeNo;
	UpdateData();
	//Recipe 정보 유무 Check
	GetDlgItem(IDC_EDIT_GRADESET_RCP_CHANGE)->GetWindowText(strRecipeNo);
	if(strRecipeNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	m_strRepNo = strRecipeNo;

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
		_T("TFT_BROKEN_VAL"),  //13
		_T("CF_BROKEN_VAL"),  //14
		_T("CF_COR_BROKEN_VAL")
	};

		CString arrKey_rank[] = {
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
		_T("CF_RBROKEN_VAL"),  //14
		_T("CF_COR_RBROKEN_VAL")
	};	

	TCHAR szBuf[MAX_PATH]={0,};

	for(int i = 0; i < (MAX_COUNT_GRADE); i++)  // BROKEN_VAL 추가
	{
		GetPrivateProfileString(_T("GRADE_INPUT"), arrKey[i], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Grade[i].Format(_T("%s"), szBuf); // 값

		m_GradeRadioInfoSet(i,m_strValue_Grade[i]);
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
		_T("CF_COR_BROKEN")
	};

	for(int j =0; j < VALUE_COUNT ; j++)
	{
		GetPrivateProfileString(_T("GRADE_VALUE"), strkey[j], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Num[j].Format(_T("%s"), szBuf);
		m_edit_tft_cf[j].SetWindowTextW(m_strValue_Num[j]);
	}
	
	for(int k =0; k < (MAX_COUNT_RANK);/*RANK 2°³ Aß°¡*/ k++)
	{
		GetPrivateProfileString(_T("RANK_VALUE"), arrKey_rank[k], _T(""), szBuf, MAX_PATH, GRADE_SETTING_PATH + m_strRepNo + _T(".ini"));
		m_strValue_Rank[k].Format(_T("%s"), szBuf);
		m_rank_edit[k].SetWindowTextW(m_strValue_Rank[k]);
	}
	UpdateData(FALSE);
}
//2020.07.01 add by mby s-
void GRADE_SET_Dialog::m_GradeRadioInfoSet(int Array, CString Grade )
{
	switch(Array)
	{
	case 0: 
		if(Grade ==  JUDGE_A)
		{
			m_radio_button1 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button1 = 1;
		}
		else
		{
			m_radio_button1 = 2;
		}
		break;
	case 1: 
		if(Grade ==  JUDGE_A)
		{
			m_radio_button2 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button2 = 1;
		}
		else
		{
			m_radio_button2 = 2;
		}
		break;
	case 2: 
		if(Grade ==  JUDGE_A)
		{
			m_radio_button3 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button3 = 1;
		}
		else
		{
			m_radio_button3 = 2;
		}
		break;
	case 3:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button4 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button4 = 1;
		}
		else
		{
			m_radio_button4 = 2;
		}
		break;
	case 4:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button5 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button5 = 1;
		}
		else
		{
			m_radio_button5 = 2;
		}
		break;
	case 5:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button6 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button6 = 1;
		}
		else
		{
			m_radio_button6 = 2;
		}
		break;
	case 6:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button7 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button7 = 1;
		}
		else
		{
			m_radio_button7 = 2;
		}
		break;
	case 7:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button8 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button8 = 1;
		}
		else
		{
			m_radio_button8 = 2;
		}
		break;
	case 8:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button9 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button9 = 1;
		}
		else
		{
			m_radio_button9 = 2;
		}
		break;
	case 9:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button10 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button10 = 1;
		}
		else
		{
			m_radio_button10 = 2;
		}
		break;
	case 10:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button11 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button11 = 1;
		}
		else
		{
			m_radio_button11 = 2;
		}
		break;
	case 11:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button12 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button12 = 1;
		}
		else
		{
			m_radio_button12 = 2;
		}
		break;
	case 12:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button13 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button13 = 1;
		}
		else
		{
			m_radio_button13 = 2;
		}
		break;
	case 13:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button14 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button14 = 1;
		}
		else
		{
			m_radio_button14 = 2;
		}
		break;
	case 14:
		if(Grade ==  JUDGE_A)
		{
			m_radio_button15 = 0;
		}
		else if(Grade ==  JUDGE_B)
		{
			m_radio_button15 = 1;
		}
		else
		{
			m_radio_button15 = 2;
		}
		break;
	}
}
//2020.07.01 add by mby E-

void GRADE_SET_Dialog::OnBnClickedBtnGraderankOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if( m_pDlgGradeRankSet )
	{
		delete m_pDlgGradeRankSet;
		m_pDlgGradeRankSet = NULL;
	}
	m_pDlgGradeRankSet = new CGRADERANKSET();
	m_pDlgGradeRankSet->Create(IDD_DLG_GRADERACNKSET, this);
	m_pDlgGradeRankSet->ShowWindow(SW_SHOW);
}
