
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CDlgRecipe.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgRecipe.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewAuto.h"
#include "ViewSetup.h"


// CDlgRecipe 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgRecipe, CDialogEx)

CDlgRecipe::CDlgRecipe(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgRecipe::IDD, pParent)
	, m_nListRecipeCurSel(-1)
{
	m_strDefMapY = _T("");
	m_strDefMapX = _T("");
	m_strRcpAlignPcNo = _T("");		//2021.07.07 add by mby

	m_pDlgLogin = NULL;
}

CDlgRecipe::~CDlgRecipe()
{
	if (m_pDlgLogin)
	{
		delete m_pDlgLogin;
		m_pDlgLogin = NULL;
	}

}

void CDlgRecipe::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_RECIPE_TOTAL, m_ListRecipeTotal);
	DDX_Control(pDX, IDC_LIST_RECIPE, m_ListRecipe);
	DDX_Control(pDX, IDC_COMBO_PLC, m_PlcRecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC1, m_PC1RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC2, m_PC2RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC3, m_PC3RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC4, m_PC4RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC5, m_PC5RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC6, m_PC6RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC7, m_PC7RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC8, m_PC8RecipeListComboCtrl);
	DDX_Control(pDX, IDC_COMBO_PC9, m_PC9RecipeListComboCtrl);
	DDX_Text(pDX, IDC_EDIT_RECIPE_MAP_Y, m_strDefMapY);
	DDX_Text(pDX, IDC_EDIT_RECIPE_MAP_X, m_strDefMapX);
	DDX_Text(pDX, IDC_EDIT_ALIGNPC_NO, m_strRcpAlignPcNo);	//2021.07.07 add by mby
}


BEGIN_MESSAGE_MAP(CDlgRecipe, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RECIPE_TOTAL, &CDlgRecipe::OnLvnItemchangedListRecipeTotal)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgRecipe::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_COPY, &CDlgRecipe::OnBnClickedBtnCopy)
	ON_BN_CLICKED(IDC_BTN_DEL, &CDlgRecipe::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_BTN_SEL_RECIPE, &CDlgRecipe::OnBnClickedBtnSelRecipe)
	ON_CBN_SELCHANGE(IDC_COMBO_PLC, &CDlgRecipe::OnCbnSelchangeComboPlc)
	ON_CBN_SELCHANGE(IDC_COMBO_PC1, &CDlgRecipe::OnCbnSelchangeComboPc1)
	ON_CBN_SELCHANGE(IDC_COMBO_PC2, &CDlgRecipe::OnCbnSelchangeComboPc2)
	ON_CBN_SELCHANGE(IDC_COMBO_PC3, &CDlgRecipe::OnCbnSelchangeComboPc3)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CDlgRecipe::OnBnClickedBtnClose)
	ON_CBN_DROPDOWN(IDC_COMBO_PLC, &CDlgRecipe::OnCbnDropdownComboPlc)
	ON_CBN_SELCHANGE(IDC_COMBO_PC4, &CDlgRecipe::OnCbnSelchangeComboPc4)
	ON_CBN_SELCHANGE(IDC_COMBO_PC5, &CDlgRecipe::OnCbnSelchangeComboPc5)
	ON_CBN_SELCHANGE(IDC_COMBO_PC6, &CDlgRecipe::OnCbnSelchangeComboPc6)
	ON_CBN_SELCHANGE(IDC_COMBO_PC7, &CDlgRecipe::OnCbnSelchangeComboPc7)
	ON_CBN_SELCHANGE(IDC_COMBO_PC8, &CDlgRecipe::OnCbnSelchangeComboPc8)
	ON_CBN_SELCHANGE(IDC_COMBO_PC9, &CDlgRecipe::OnCbnSelchangeComboPc9)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_RECIPE_MAP_LT, IDC_RADIO_RECIPE_MAP_RB, OnBnClickedDefectMapOriginType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_RECIPE_MAP_AXIS_OFF, IDC_RADIO_RECIPE_MAP_AXIS_ON, OnBnClickedDefectMapMajorAxisType)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF, IDC_RADIO_RECIPE_MAP_CORD_QMS_ON, OnBnClickedDefectMapReportCoordinateQMSType)
	ON_BN_CLICKED(IDC_BUTTON_RECIPE_MAP_SIZE, &CDlgRecipe::OnBnClickedButtonRecipeMapSize)
	ON_MESSAGE(WM_AUTO_RECIPE, &CDlgRecipe::OnAutoRecipe)
	ON_BN_CLICKED(IDC_BTN_PLC_MODEL_READ, &CDlgRecipe::OnBnClickedBtnPlcModelRead)
	ON_BN_CLICKED(IDC_BTN_PLC_MODEL_NAME_CHANGE, &CDlgRecipe::OnBnClickedBtnPlcModelNameChange)
	ON_BN_CLICKED(IDC_BTN_PLC_MODEL_COPY, &CDlgRecipe::OnBnClickedBtnPlcModelCopy)
	ON_BN_CLICKED(IDC_BTN_PLC_MODEL_DELETE, &CDlgRecipe::OnBnClickedBtnPlcModelDelete)
	ON_BN_CLICKED(IDC_BUTTON_ALIGNPC_NO_SET, &CDlgRecipe::OnBnClickedButtonAlignpcNoSet)
END_MESSAGE_MAP()


// CDlgRecipe 메시지 처리기입니다.

BOOL CDlgRecipe::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_BTN_CLOSE)->ShowWindow(SW_HIDE);

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	int nEqpSizeType = pFrame->m_fnEqpSizeType();		//2016.03.03 Mr.Chu		대형 Auto Recipe 
	int nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);	//2016.03.03 Mr.Chu		대형 Auto Recipe 

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 12;
	lf.lfCharSet = HANGUL_CHARSET;
	_tcscpy(lf.lfFaceName, _T("Arial"));
	HFONT hfont = ::CreateFontIndirect(&lf);

	CFont* pfont = CFont::FromHandle(hfont);

	DWORD EX_style = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE;

	// List Recipe Total
	m_ListRecipeTotal.SetFont(pfont);	
	m_ListRecipeTotal.ModifyStyle( LVS_TYPEMASK, LVS_REPORT );
	m_ListRecipeTotal.SetExtendedStyle( m_ListRecipeTotal.GetExtendedStyle() | EX_style );
	m_ListRecipeTotal.InsertColumn( 0, _T("Recipe ID"), LVCFMT_CENTER, 70 );
	m_ListRecipeTotal.InsertColumn( 1, _T("Comment"), LVCFMT_CENTER, 245 );

	m_ListRecipeTotal.SetColumnEditable(0);

	// List Recipe
	m_ListRecipe.SetFont(pfont);	
	m_ListRecipe.ModifyStyle( LVS_TYPEMASK, LVS_REPORT );
	m_ListRecipe.SetExtendedStyle( m_ListRecipe.GetExtendedStyle() | EX_style );
	m_ListRecipe.InsertColumn( 0, _T("PC Name"), LVCFMT_CENTER, 130 );
	m_ListRecipe.InsertColumn( 1, _T("Recipe ID"), LVCFMT_CENTER, 185 );

	//2016.03.14 Mr.Chu
	GetDlgItem(IDC_STATIC_SET_DEFECT_MAP_ORIGN)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_LT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_LB)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_RT)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_RB)->EnableWindow(FALSE);

	//2016.04.14 Mr.Chu		Defect Map Major Axis Setting
	GetDlgItem(IDC_STATIC_SET_DEFECT_MAP_AXIS)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_OFF)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_ON)->EnableWindow(FALSE);

	//2016.12.30
	GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_ON)->EnableWindow(FALSE);
	

	//2016.03.03 Mr.Chu				Client 관련 ComboBox Visible , InVisible 셋팅
	if(nEqpSizeType)					//대형
	{
		if (nEqpNo == 0 && (pFrame->m_stParamInfo.nCountVisionClient == 6))			//AVL
		{
			GetDlgItem(IDC_COMBO_PC7)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC7)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_COMBO_PC8)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC8)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_COMBO_PC9)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC9)->ShowWindow(SW_HIDE);
		}
	}
	else								//중소형
	{
		if (nEqpNo == 0)				//AVL
		{
			GetDlgItem(IDC_COMBO_PC3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC3)->ShowWindow(SW_HIDE);
		}
		GetDlgItem(IDC_COMBO_PC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC4)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC5)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC6)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC7)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC8)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC9)->ShowWindow(SW_HIDE);
	}

	//2017.07.07	Button Disable
	if( pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_PLC_MODEL_READ, 0) )
		GetDlgItem(IDC_BTN_PLC_MODEL_READ)->EnableWindow(FALSE);	

	for (int i = 0; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		m_fnSetRecipeComboBox(i);
	}

	m_fnRecipe_Init();

	//2016.02.16 Mr.Chu
	m_fnCheckContents();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgRecipe::m_fnRecipe_Init()
{
	CString strComment, strNo, strSectionNo, strKeyNo;
	wchar_t chRead[255] = {0};
	int nRecipeNo = 0;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	m_nListRecipeCurSel = -1;

	m_ListRecipeTotal.DeleteAllItems();

	m_stRecipeListInfo = new STRU_RECIPE_LIST_INFO[PLC_MAX_RECIPE];
	memset(m_stRecipeListInfo,0X00,sizeof(m_stRecipeListInfo));

	for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
	{
		strSectionNo.Format(_T("RECIPE_%d"), nCount + 1);

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW((LPCTSTR)strSectionNo, RECIPE_COMMENT, _T(""), (LPTSTR)chRead, 255);
		strComment = chRead;

		m_stRecipeListInfo[nCount].strRecipeDefeMapSizeX = _T("");
		m_stRecipeListInfo[nCount].strRecipeDefeMapSizeY = _T("");
		m_stRecipeListInfo[nCount].strRecipAlignPcNo = _T("");		//2021.07.07 add by mby

		if(strComment != _T(""))
		{
			nRecipeNo = pFrame->m_iniRecipe.GetProfileIntW((LPCTSTR)strSectionNo, RECIPE_NO, 0);
			strNo.Format(_T("%d"), nRecipeNo);
			m_ListRecipeTotal.InsertItem(nCount, strNo);
			m_stRecipeListInfo[nCount].nRecipeListNo = nRecipeNo;

			m_ListRecipeTotal.SetItemText(nCount, 1, chRead);
			_tcscpy(m_stRecipeListInfo[nCount].strRecipeComment, chRead);

			memset(chRead,0X00,sizeof(chRead));
			pFrame->m_iniRecipe.GetProfileStringW((LPCTSTR)strSectionNo, PLC_MODEL, _T(""), (LPTSTR)chRead, 255);
			_tcscpy(m_stRecipeListInfo[nCount].strRecipePLC, chRead);

			for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
			{
				strKeyNo.Format(_T("RECIPE_PC_%d"), i);

				memset(chRead,0X00,sizeof(chRead));
				pFrame->m_iniRecipe.GetProfileStringW((LPCTSTR)strSectionNo, (LPCTSTR)strKeyNo, _T(""), (LPTSTR)chRead, 255);

				_tcscpy(m_stRecipeListInfo[nCount].strRecipePC[i - 1], chRead);
			}

			//2016.03.14 Mr.Chu				Defect Map , Recipe 연동
			m_stRecipeListInfo[nCount].nRecipeDefMapType = pFrame->m_iniRecipe.GetProfileInt((LPCTSTR)strSectionNo,RECIPE_DEF_MAP_TYPE,0);

			pFrame->m_iniRecipe.GetProfileStringW((LPCTSTR)strSectionNo, RECIPE_DEF_MAP_SIZE, _T(""), (LPTSTR)chRead, 255);
			BOOL bCheck = false;
			wchar_t strMapX[255] = {0,},strMapY[255] = {0,};
			for(int i=0,j=0;i<255;i++)
			{
				if(chRead[i] == _T(',') )
				{
					bCheck = true;
					continue;
				}
				else if(chRead[i] == NULL || chRead[i] == _T('-') )
					break;

				if(bCheck == false)
					strMapX[i] = chRead[i];
				else
				{
					strMapY[j] = chRead[i];
					j++;
				}
			}

			m_stRecipeListInfo[nCount].strRecipeDefeMapSizeX.Format(_T("%s"),strMapX);
			m_stRecipeListInfo[nCount].strRecipeDefeMapSizeY.Format(_T("%s"),strMapY);
			//

			//2016.04.14 Mr.Chu		장축, 단축 레시피 셋팅 초기화
			m_stRecipeListInfo[nCount].nRecipeMajorAxisType = pFrame->m_iniRecipe.GetProfileInt((LPCTSTR)strSectionNo,RECIPE_DEF_MAP_MAJOR_AXIS,0);
			//

			//2016.04.14 Mr.Chu		Def Map 좌표 데이터  QMS에 연동
			m_stRecipeListInfo[nCount].nRecipeDefMapCoordQms = pFrame->m_iniRecipe.GetProfileInt((LPCTSTR)strSectionNo,RECIPE_DEF_MAP_COORD_QMS,0);
			//

			//2021.07.07 add by mby
			int RcpAlignPCNo = pFrame->m_iniRecipe.GetProfileInt((LPCTSTR)strSectionNo, RECIPE_ALIGNPC_NO, 0);
			m_stRecipeListInfo[nCount].strRecipAlignPcNo.Format(_T("%d"),RcpAlignPCNo);

		}
	}

	UpdateData(FALSE);
}

void CDlgRecipe::m_fnRefreshRecipeTotal()
{
	CString strRecipeComment, strNo;

	m_ListRecipeTotal.DeleteAllItems();

	for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
	{
		strRecipeComment = m_stRecipeListInfo[nCount].strRecipeComment;

		if (strRecipeComment != _T(""))
		{
			strNo.Format(_T("%d"), m_stRecipeListInfo[nCount].nRecipeListNo);
			m_ListRecipeTotal.InsertItem(nCount, strNo);
			m_ListRecipeTotal.SetItemText(nCount, 1, strRecipeComment);
		}
	}

	m_nListRecipeCurSel = -1;
}

void CDlgRecipe::m_fnSetRecipeComboBox(int nFlag)
{
	CString strRecipeTemp;
	int nCurPos = 0;
	wchar_t chRead[1000] = {0};		//20200212 add by mby
	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	if (nFlag == 0)	//PLC Recipe
	{
		// 콤보박스 아이템(Recipe Name) 초기화 by yskim 20170525
		for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
			pView->m_strPLCRecipe[nCount] = "";

		m_PlcRecipeListComboCtrl.ResetContent();

		// 콤보박스 아이템(Recipe Name) 가져옴 by yskim 02170525
		pView->m_fnReadRecipePLC();

		for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
		{
			strRecipeTemp.Format(_T("%d : %s"), nCount + 1, pView->m_strPLCRecipe[nCount]);
			m_PlcRecipeListComboCtrl.AddString(strRecipeTemp);
		}

		m_PlcRecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_1)		//PC1 Recipe
	{
		m_PC1RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_1"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC1RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC1RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_2)		//PC2 Recipe
	{
		m_PC2RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_2"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC2RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC2RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_3)		//PC3 Recipe
	{
		m_PC3RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_3"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC3RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC3RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_4)		//PC4 Recipe
	{
		m_PC4RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_4"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC4RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC4RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_5)		//PC5 Recipe
	{
		m_PC5RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_5"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC5RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC5RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_6)		//PC4 Recipe
	{
		m_PC6RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_6"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC6RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC6RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_7)		//PC4 Recipe
	{
		m_PC7RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_7"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC7RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC7RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_8)		//PC8 Recipe
	{
		m_PC8RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_8"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC8RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC8RecipeListComboCtrl.SetCurSel(0);
	}
	else if(nFlag == VISION_PC_9)		//PC9 Recipe
	{
		m_PC9RecipeListComboCtrl.ResetContent();

		memset(chRead,0X00,sizeof(chRead));
		pFrame->m_iniRecipe.GetProfileStringW(RECIPE_INFO, _T("RECIPE_LIST_PC_9"), _T(""), (LPTSTR)chRead, 1000);
		strRecipeTemp  = chRead;

		CString resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			m_PC9RecipeListComboCtrl.AddString(resToken);
			resToken= strRecipeTemp.Tokenize(EXT_COLON, nCurPos);
		}

		m_PC9RecipeListComboCtrl.SetCurSel(0);
	}

	UpdateData(FALSE);
}


void CDlgRecipe::OnLvnItemchangedListRecipeTotal(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNMHDR;
	m_CurRows = pItem->iRow;

	//2016.03.14 Mr.Chu
	GetDlgItem(IDC_STATIC_SET_DEFECT_MAP_ORIGN)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_LT)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_LB)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_RT)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_RB)->EnableWindow(TRUE);

	//2016.04.14 Mr.Chu		Defect Map Major Axis Setting
	GetDlgItem(IDC_STATIC_SET_DEFECT_MAP_AXIS)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_OFF)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_ON)->EnableWindow(TRUE);
	//
	//2016.12.30
	GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_ON)->EnableWindow(TRUE);

	UpdateData(TRUE);

	int nCurSel, nCount = m_ListRecipeTotal.GetItemCount();

	for ( m_nListRecipeCurSel=0 ; m_nListRecipeCurSel < nCount ; m_nListRecipeCurSel++ )
	{
		if ( (LVIS_SELECTED | LVIS_FOCUSED) == m_ListRecipeTotal.GetItemState( m_nListRecipeCurSel, LVIS_SELECTED | LVIS_FOCUSED ) )
		{
			nCurSel = m_nListRecipeCurSel;

			CString strName = m_ListRecipeTotal.GetItemText(m_nListRecipeCurSel, 1);

			GetDlgItem(IDC_EDIT_RECIPE)->SetWindowText(strName);

			m_fnRecipeSelView(nCurSel);

			UpdateData(FALSE);

			break;
		}
	}

	if (m_nListRecipeCurSel >= nCount)
	{
		m_nListRecipeCurSel = nCount - 1;
	}

	*pResult = 0;
}

void CDlgRecipe::m_fnRecipeSelView(int nCurSel)
{
	CString strInsNo,strTemp = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_ListRecipe.DeleteAllItems();

	m_ListRecipe.InsertItem(0, _T("PLC Model"));
	m_ListRecipe.SetItemText(0, 1, m_stRecipeListInfo[nCurSel].strRecipePLC);

	for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		strInsNo.Format(_T("Inspection PC %d"), i);
		m_ListRecipe.InsertItem(i, strInsNo);
		m_ListRecipe.SetItemText(i, 1, m_stRecipeListInfo[nCurSel].strRecipePC[i - 1]);
	}

	//2016.03.14 Mr.Chu	 Defect Map , Recipe Change 연동

	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_LT))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_LB))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_RT))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_RB))->SetCheck(FALSE);

	m_ListRecipe.InsertItem(pFrame->m_stParamInfo.nCountVisionClient+1, _T("Defect Map Type"));
	
	if(m_stRecipeListInfo[nCurSel].nRecipeDefMapType == 0)
	{
		AfxMessageBox(_T("Please Check Defect Map Type ! ( Default Setting )"));
		if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			m_stRecipeListInfo[nCurSel].nRecipeDefMapType = IDC_RADIO_RECIPE_MAP_RT;
		else
			m_stRecipeListInfo[nCurSel].nRecipeDefMapType = IDC_RADIO_RECIPE_MAP_LT;
	}

	if(m_stRecipeListInfo[nCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_LT)
	{
		strTemp.Format(_T("Left Top"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_LT))->SetCheck(TRUE);
	}
	else if(m_stRecipeListInfo[nCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_LB)
	{
		strTemp.Format(_T("Left Bottom"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_LB))->SetCheck(TRUE);
	}
	else if(m_stRecipeListInfo[nCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_RT)
	{
		strTemp.Format(_T("Right Top"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_RT))->SetCheck(TRUE);
	}
	else if(m_stRecipeListInfo[nCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_RB)
	{
		strTemp.Format(_T("Right Bottom"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_RB))->SetCheck(TRUE);
	}

	m_ListRecipe.SetItemText(pFrame->m_stParamInfo.nCountVisionClient+1, 1, strTemp);	//Origin Type
	strTemp = _T("");

	//	셋팅이 안되어 있을 경우에 대한 예외처리
	GetDlgItem(IDC_EDIT_RECIPE_MAP_X)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_RECIPE_MAP_Y)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_ALIGNPC_NO)->SetWindowText(_T(""));	//2021.07.07 add by mby

	if(m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX == _T("")
		|| m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY == _T(""))
	{
		AfxMessageBox(_T("Please Check Defect Map Size ! ( Default Setting )"));
		if ( pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
		{
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX.Format(_T("%d"),DEFAULT_PANEL_WIDTH_LARGE_SIZE);
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY.Format(_T("%d"),DEFAULT_PANEL_HEIGHT_LARGE_SIZE);
		}
		else if( pFrame->m_fnEqpSizeType() == EQP_SMALL_SIZE 
			|| pFrame->m_fnEqpSizeType() == EQP_MEDIUM_SIZE )
		{
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX.Format(_T("%d"),DEFAULT_PANEL_WIDTH_SIZE);
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY.Format(_T("%d"),DEFAULT_PANEL_HEIGHT_SIZE);
		}
		else if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE )
		{
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX.Format(_T("%d"),DEFAULT_PANEL_WIDTH_MONITOR_SIZE);
			m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY.Format(_T("%d"),DEFAULT_PANEL_HEIGHT_MONITOR_SIZE);
		}
	}
	//

	m_ListRecipe.InsertItem(pFrame->m_stParamInfo.nCountVisionClient+2, _T("Defect Map Size ( X, Y )"));
	strTemp.Format(_T("%s,%s"),m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX, m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY);
	m_ListRecipe.SetItemText(pFrame->m_stParamInfo.nCountVisionClient+2, 1, strTemp);	//Def Map Size Type

	m_strDefMapX = m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeX;
	m_strDefMapY = m_stRecipeListInfo[nCurSel].strRecipeDefeMapSizeY;
	m_strRcpAlignPcNo =	m_stRecipeListInfo[nCurSel].strRecipAlignPcNo;	//2021.07.07 add by mby

	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_ON))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_OFF))->SetCheck(FALSE);

	if(m_stRecipeListInfo[nCurSel].nRecipeMajorAxisType == 0)
	{
		AfxMessageBox(_T("Please Check Defect Map Major Axis ! ( Default Setting )"));
		if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			m_stRecipeListInfo[nCurSel].nRecipeMajorAxisType = IDC_RADIO_RECIPE_MAP_AXIS_ON;
		else
			m_stRecipeListInfo[nCurSel].nRecipeMajorAxisType = IDC_RADIO_RECIPE_MAP_AXIS_OFF;
	}

	if(m_stRecipeListInfo[nCurSel].nRecipeMajorAxisType == IDC_RADIO_RECIPE_MAP_AXIS_ON)
	{
		strTemp.Format(_T("X,Y Reversal Mode"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_ON))->SetCheck(TRUE);
	}
	else if(m_stRecipeListInfo[nCurSel].nRecipeMajorAxisType == IDC_RADIO_RECIPE_MAP_AXIS_OFF)
	{
		strTemp.Format(_T("Original Mode"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_AXIS_OFF))->SetCheck(TRUE);
	}

	m_ListRecipe.InsertItem(pFrame->m_stParamInfo.nCountVisionClient+3, _T("Defect Map Major Axis"));
	m_ListRecipe.SetItemText(pFrame->m_stParamInfo.nCountVisionClient+3, 1, strTemp);	//Origin Type

	//

	//2016.12.30 Mr.Chu
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_ON))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF))->SetCheck(FALSE);

	if(m_stRecipeListInfo[nCurSel].nRecipeDefMapCoordQms == 0)
	{
		AfxMessageBox(_T("Please Check Report Coordinate Data ! ( Default Setting )"));

		m_stRecipeListInfo[nCurSel].nRecipeDefMapCoordQms = IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF;
	}

	if(m_stRecipeListInfo[nCurSel].nRecipeDefMapCoordQms == IDC_RADIO_RECIPE_MAP_CORD_QMS_ON)
	{
		strTemp.Format(_T("Report QMS"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_ON))->SetCheck(TRUE);
	}
	else if(m_stRecipeListInfo[nCurSel].nRecipeDefMapCoordQms == IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF)
	{
		strTemp.Format(_T("Just View"));
		((CButton*)GetDlgItem(IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF))->SetCheck(TRUE);
	}

	m_ListRecipe.InsertItem(pFrame->m_stParamInfo.nCountVisionClient+4, _T("Defect Map Coordinate Data"));
	m_ListRecipe.SetItemText(pFrame->m_stParamInfo.nCountVisionClient+4, 1, strTemp);	//Origin Type

	//
	UpdateData(FALSE);

}

void CDlgRecipe::m_fnSelectRecipeVision(int nVision, CString strRecipe)
{
	CString strMsgTemp;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[nVision - 1])
	{
		pFrame->m_pVsSeverIF->m_fnSelectVisionRecipe(nVision, strRecipe);
	}
	else
	{
		strMsgTemp.Format(_T("Connection Check PC%d"), nVision);
		AfxMessageBox(strMsgTemp);
	}
}

BOOL CDlgRecipe::m_fnSelectRecipePLC(int nPLCModelNo, int nCIMRecipeNo)
{
	long nModelChangeInterlock = -1, nCurModel = -1, lRet = -1, nChangeCheck = -1, nCnt = 0;
	CString strLocalData, strModelName, strPlcData, strTemp, strRecipeNo;
	BOOL bResult = FALSE;
	TCHAR tszParamAddr[255];

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	G_AddLog(2, 0, _T("m_fnSelectRecipePLC Start"));

	nModelChangeInterlock = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_INTERLOCK);

	strLocalData = _T("");
	strTemp = _T("");

	if (nModelChangeInterlock == OK)
	{
		//2016.12.13 Model Data Set
		G_AddLog(2, 0, _T("m_fnSelectRecipePLC nModelChangeInterlock = OK"));
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, TRUE);	//Model Change Start

		//Cheng Du Comment
		//현재 모델 저장
		//lRet = pView->m_pDlgPLC->m_fnGetDeviceBlockModel(ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_READ_PARAM_BUFFER_SIZE, strPlcData);
		//nCurModel = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_CURRENT_RECIPE_NO);
		//strModelName =  pFrame->m_DataManager.m_fnGetModelName(nCurModel);
		//strModelName.Trim();

		///////////////////
		int nTemp = 0;
		int nStartIdx = 0;

		//Buffer Clear
		for(int nParamCnt = 0; nParamCnt < PLC_MODEL_TOTAL_COUNT; nParamCnt++)
		{
			lRet -1;
			nTemp = 5376 + nParamCnt;
			memset(tszParamAddr, 0, sizeof(tszParamAddr));
			_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);
			lRet = pView->m_pDlgPLC->m_fnSetDeviceModel(tszParamAddr, 0);
		}

		for(int nParamCnt = 0; nParamCnt < PLC_MODEL_PARAM_COUNT; nParamCnt++)
		{
			nTemp = 6656 + nParamCnt;
			memset(tszParamAddr, 0, sizeof(tszParamAddr));
			_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);
			lRet = pView->m_pDlgPLC->m_fnGetDeviceModel(tszParamAddr);
			strTemp.Format(_T("%ld,"),lRet);
			strPlcData.Append(strTemp);
			if(nParamCnt == 979)
				nCurModel = lRet;
		}

		strModelName =  pFrame->m_DataManager.m_fnGetModelName(nCurModel);
		strModelName.Trim();
		///////////////////
		if(strModelName != _T("") && strPlcData != _T("") && nCurModel !=0)
		{
			pFrame->m_DataManager.m_fnWriteModel(nCurModel, strModelName, strPlcData);
			G_AddLog(2, 0, _T("m_fnSelectRecipePLC m_fnWriteModel"));
		}
		else
		{
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, TRUE);
			AfxMessageBox(_T("Model Data Save Fail"));
			return FALSE;
		}

		//요청 모델 세팅
		strModelName = pFrame->m_DataManager.m_fnGetModelName(nPLCModelNo);
		strLocalData = pFrame->m_DataManager.m_fnReadModel(nPLCModelNo, strModelName);
		if(strLocalData != _T(""))
		{
			int nCurPos = 0, nAddr = 0;
			CString resToken= strLocalData.Tokenize(EXT_COLON, nCurPos);
			while (resToken != "")
			{
				nTemp = 5376 + nAddr;
				memset(tszParamAddr, 0, sizeof(tszParamAddr));
				_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);

				nAddr++;

				if(nAddr == PLC_MODEL_PARAM_COUNT)	//중소형 : 9900 대형 : 980 모니터 : 990
					break;

				if(resToken != _T("0"))
				{
					lRet = pView->m_pDlgPLC->m_fnSetDeviceModel(tszParamAddr, _ttol(resToken));
				}
				resToken= strLocalData.Tokenize(EXT_COLON, nCurPos);
			}

			lRet = pView->m_pDlgPLC->m_fnSetDevice(_T("W18D3"), nPLCModelNo);

			Sleep(PLC_MODEL_CHANGE_SLEEP);	//중소형 35Sec

			bResult = pFrame->m_DataManager.m_fnCompareData(nPLCModelNo, strModelName, ADDR_PLC_MODEL_CHANGE_PARAM_BUFFER, PC_MODEL_BUFFER);
			G_AddLog(3, 2, _T("m_fnSelectRecipePLC() After Model No : %d Model Name : %s"), nPLCModelNo, strModelName);
		}
		//2016.12.13 Model Data Set

		if(bResult)
		{
			//pView->m_pDlgPLC->m_fnSetDevice(_T("W18D3"), nPLCModelNo);
			pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PLC_MODEL_CHANGE_NAME_BUFFER, PLC_MODEL_NAME_SIZE, strModelName);

			Sleep(500);

			nCurModel = pView->m_pDlgPLC->m_fnGetDevice(_T("W18D3"));

			if(nCurModel == nPLCModelNo)
			{
				//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_MODEL_CHANGE_REQ_NO_INFO, nPLCRecipeNo);

				pView->m_pDlgPLC->m_fnSetDevice(_T("W18E8"), nCIMRecipeNo);

				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, TRUE);	//OK

				while(TRUE)
				{
					nChangeCheck = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_CHECK);
					if(nCnt >= 3 || nChangeCheck == TRUE)
					{
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, FALSE);
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
						//Buffer Clear
						for(int nParamCnt = 0; nParamCnt < PLC_MODEL_TOTAL_COUNT; nParamCnt++)
						{
							lRet -1;
							nTemp = 5376 + nParamCnt;
							memset(tszParamAddr, 0, sizeof(tszParamAddr));
							_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);
							lRet = pView->m_pDlgPLC->m_fnSetDeviceModel(tszParamAddr, 0);
						}
						break;
					}
					nCnt++;
					Sleep(500);
				}

				return TRUE;
			}
			else
			{
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, TRUE);	//NG

				while(TRUE)
				{
					nChangeCheck = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_CHECK);
					if(nCnt >= 3 || nChangeCheck == TRUE)
					{
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, FALSE);
						pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
						//Buffer Clear
						for(int nParamCnt = 0; nParamCnt < PLC_MODEL_TOTAL_COUNT; nParamCnt++)
						{
							lRet -1;
							nTemp = 5376 + nParamCnt;
							memset(tszParamAddr, 0, sizeof(tszParamAddr));
							_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);
							lRet = pView->m_pDlgPLC->m_fnSetDeviceModel(tszParamAddr, 0);
						}
						break;
					}
					nCnt++;
					Sleep(500);
				}

				return FALSE;
			}
		}
		else
		{
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, TRUE);	//NG

			while(TRUE)
			{
				nChangeCheck = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_CHECK);
				if(nCnt >= 3 || nChangeCheck == TRUE)
				{
					pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
					pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, FALSE);
					pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
					//Buffer Clear
					for(int nParamCnt = 0; nParamCnt < PLC_MODEL_TOTAL_COUNT; nParamCnt++)
					{
						lRet -1;
						nTemp = 5376 + nParamCnt;
						memset(tszParamAddr, 0, sizeof(tszParamAddr));
						_stprintf_s(tszParamAddr, 255, _T("W%d"), nTemp);
						lRet = pView->m_pDlgPLC->m_fnSetDeviceModel(tszParamAddr, 0);
					}
					break;
				}
				nCnt++;
				Sleep(500);
			}

			return FALSE;
		}

	}
	else
		return FALSE;
}


void CDlgRecipe::OnBnClickedBtnSave()
{
	CString strSectionNo, strKeyNo, strRecipeComment, strRecipeNo, strTemp;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	int nRecipeCnt = m_ListRecipeTotal.GetItemCount();

	for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
	{
		strSectionNo.Format(_T("RECIPE_%d"), nCount + 1);

		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_COMMENT, m_stRecipeListInfo[nCount].strRecipeComment);

		if (nCount < nRecipeCnt)
		{
			strRecipeNo = m_ListRecipeTotal.GetItemText(nCount, 0);
			m_stRecipeListInfo[nCount].nRecipeListNo = _tstoi((LPCTSTR)strRecipeNo);
			pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_NO, strRecipeNo);
		}
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, PLC_MODEL, m_stRecipeListInfo[nCount].strRecipePLC);
		for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			strKeyNo.Format(_T("RECIPE_PC_%d"), i);
			pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, (LPCTSTR)strKeyNo, m_stRecipeListInfo[nCount].strRecipePC[i - 1]);
		}

		//2016.03.14 Mr.Chu			Defect map, Recipe 연동
		if(m_stRecipeListInfo[nCount].nRecipeDefMapType <=0 )
			strTemp = _T("");
		else
			strTemp.Format(_T("%d"),m_stRecipeListInfo[nCount].nRecipeDefMapType);
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_DEF_MAP_TYPE, strTemp);

		strTemp = _T("");
		strTemp.Format(_T("%s,%s"),m_stRecipeListInfo[nCount].strRecipeDefeMapSizeX, m_stRecipeListInfo[nCount].strRecipeDefeMapSizeY);
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_DEF_MAP_SIZE, strTemp);

		if(m_stRecipeListInfo[nCount].nRecipeMajorAxisType <=0 )
			strTemp = _T("");
		else
			strTemp.Format(_T("%d"),m_stRecipeListInfo[nCount].nRecipeMajorAxisType);
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_DEF_MAP_MAJOR_AXIS, strTemp);

		//2016.12.30
		if(m_stRecipeListInfo[nCount].nRecipeDefMapCoordQms <=0 )
			strTemp = _T("");
		else
			strTemp.Format(_T("%d"),m_stRecipeListInfo[nCount].nRecipeDefMapCoordQms);
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_DEF_MAP_COORD_QMS, strTemp);
		//
		//2021.07.07 add by mby for Align PC NO Select
		strTemp = _T("");
		strTemp.Format(_T("%s"),m_stRecipeListInfo[nCount].strRecipAlignPcNo);		
		pFrame->m_iniRecipe.WriteProfileStringW((LPCTSTR)strSectionNo, RECIPE_ALIGNPC_NO, strTemp);
	}
}


void CDlgRecipe::OnBnClickedBtnCopy()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strName, strNo, strRecipeComment;
	
	GetDlgItem(IDC_EDIT_RECIPE)->GetWindowText(strName);

	for (int nCount = 0 ; nCount < PLC_MAX_RECIPE ; nCount++)
	{
		strRecipeComment = m_stRecipeListInfo[nCount].strRecipeComment;

		if (strRecipeComment != _T("") && strRecipeComment == strName)
		{
			AfxMessageBox(_T("Recipe Comment 중복"));
			return;
		}
		else if(strRecipeComment == _T(""))
		{
			m_stRecipeListInfo[nCount] = m_stRecipeListInfo[m_nListRecipeCurSel];
			_tcscpy(m_stRecipeListInfo[nCount].strRecipeComment, strName);
			m_fnRefreshRecipeTotal();
			return;
		}
	}

}


void CDlgRecipe::OnBnClickedBtnDel()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	m_stRecipeListInfo[m_nListRecipeCurSel].init_info();

	STRU_RECIPE_LIST_INFO stRecipeTemp;

	for (int i = m_nListRecipeCurSel; i < PLC_MAX_RECIPE - 1; i++)
	{
		if (i >= PLC_MAX_RECIPE) break;
		stRecipeTemp.init_info();
		stRecipeTemp = m_stRecipeListInfo[i];
		m_stRecipeListInfo[i] = m_stRecipeListInfo[i + 1];
		m_stRecipeListInfo[i + 1] = stRecipeTemp;
	}

	m_ListRecipe.DeleteAllItems();
	
	GetDlgItem(IDC_EDIT_RECIPE)->SetWindowText(_T(""));

	m_fnRefreshRecipeTotal();
}


void CDlgRecipe::OnBnClickedBtnSelRecipe()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	SendMessage(WM_AUTO_RECIPE);
}

BOOL CDlgRecipe::m_fnChangePLCRecipe()
{
	CString strRecipePLC, strRecipePC[MAX_VISION_PC];
	CString strCimRecipe, strCimRecipeName;	//2016.02.26 Mr.Chu			PLC Cim관련 Recipe 변경
	int nPlcModelNo, nCimModelNo;
	BOOL bPlcRetVal = FALSE, bPcRetVal = FALSE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	int nEqpType = pFrame->m_stParamInfo.nEqpType;
	int nEqpSizeType = pFrame->m_fnEqpSizeType();		//2016.03.03 Mr.Chu

	strRecipePLC = m_ListRecipe.GetItemText(0, 1);
	strCimRecipe = m_ListRecipeTotal.GetItemText(m_nListRecipeCurSel, 0);
	strCimRecipeName = m_ListRecipeTotal.GetItemText(m_nListRecipeCurSel, 1);//2016.02.26 Mr.Chu			PLC Cim관련 Recipe 변경
	nPlcModelNo = _tstoi((LPCTSTR)strRecipePLC);
	nCimModelNo = _tstoi((LPCTSTR)strCimRecipe);
	bPlcRetVal = m_fnSelectRecipePLC(nPlcModelNo, nCimModelNo);

	if (bPlcRetVal)
	{
		int nRecpNum = _tstoi(strCimRecipe);

		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_CHANGE_RECIPE_NO, nRecpNum);
		//2018.03.09
		pFrame->m_iniRecipe.WriteProfileStringW(RECIPE_INFO,RECIPE_CURRENT_NO, strCimRecipe);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, strCimRecipe);			//ADD BY KYH01

		//20161212 - 기존 적용에 중소형 AVL에도 적용으로 변경(주석처리)
		//if (nEqpSizeType == EQP_LARGE_SIZE || (nEqpSizeType != EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS) || nEqpSizeType == EQP_MONITOR_SIZE)
		//if( nEqpSizeType == EQP_MONITOR_SIZE )
		//	pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PLC_CHANGE_RECIPE_NAME,20,strCimRecipeName);	//100->20
		//else
		//	pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PLC_CHANGE_RECIPE_NAME,100,strCimRecipeName);
		pView->m_fnInitMeasureLimitInfo();
	}

	return bPlcRetVal;	
	//
}

void CDlgRecipe::m_fnSetDefectMap()
{	
	CString strTemp = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//2017.07.10 Origin Type 정보는 Client에서 수신하도록 함.  //2016.03.14 Mr.Chu		Defect Map, Recipe 연동
	//Defect Map Type
// 	if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_LT)
// 		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
// 	else if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_LB)
// 		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_LB;
// 	else if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_RT)
// 		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RT;
// 	else if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType == IDC_RADIO_RECIPE_MAP_RB)
// 		pFrame->m_DataManager.m_stDefCoordDatRcp.nOriginType = RECIPE_DEF_MAP_ORIGIN_RB;
	//

	//Defect Map Major Axis Type
	if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeMajorAxisType == IDC_RADIO_RECIPE_MAP_AXIS_OFF)
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = FALSE;
	else if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeMajorAxisType == IDC_RADIO_RECIPE_MAP_AXIS_ON)
		pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType = TRUE;
	//

	//2016.12.30 Report QMS Coordinate From Defect Map
	if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapCoordQms == IDC_RADIO_RECIPE_MAP_CORD_QMS_OFF)
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = FALSE;
	else if(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapCoordQms == IDC_RADIO_RECIPE_MAP_CORD_QMS_ON)
		pFrame->m_DataManager.m_stDefCoordDatRcp.bReportQMSCoordDat = TRUE;
	//
	//2021.07.07 add by mby S-
	if(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipAlignPcNo == _T("0")){
		pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo = 0;
	}else{
		pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo = 1;
	}
	//2021.07.07 add by mby E-

	if(!(CViewAuto*)pFrame->m_pViewAuto)
		pFrame->m_pViewAuto = (CViewAuto*)pFrame->GetActiveView();
	((CViewAuto*)pFrame->m_pViewAuto)->UpdateData(FALSE);

	pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth 
		= _wtof(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipeDefeMapSizeX);
	pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight
		= _wtof(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipeDefeMapSizeY);


	if(((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap == NULL
		|| ((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap->GetSafeHwnd() == NULL)	
	{
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap = new CDlgDefectMap();
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap->Create(IDD_DLG_MAP, this);
	}

	// Map Data Initialize		//20160708 Mr.Chu
	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap->m_stDefMapData.strBefPanelID = _T("");
	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap->m_stDefMapData.strBefInspDate = _T("");
	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgMap->OnPaint();		
	//

	((CViewAuto*)pFrame->m_pViewAuto)->m_fnSelOriginType(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType);
	((CViewAuto*)pFrame->m_pViewAuto)->m_fnActiveXYReversal(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeMajorAxisType);	
	((CViewAuto*)pFrame->m_pViewAuto)->m_fnActiveReportCoordQMS(m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapCoordQms);	
	strTemp.Format(_T("RECIPE_%d"),m_nListRecipeCurSel+1);
	pFrame->m_iniRecipe.WriteProfileStringW(RECIPE_INFO,RECIPE_CURRENT,strTemp);
}

void CDlgRecipe::OnCbnSelchangeComboPlc()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe, strNo;

	int nIndex = m_PlcRecipeListComboCtrl.GetCurSel();
	//m_PlcRecipeListComboCtrl.GetLBText(nIndex, strRecipe);
	strNo.Format(_T("%d"), nIndex + 1);
	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePLC, strNo);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc1()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC1RecipeListComboCtrl.GetCurSel();
	m_PC1RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[0], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc2()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC2RecipeListComboCtrl.GetCurSel();
	m_PC2RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[1], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc3()
{
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC3RecipeListComboCtrl.GetCurSel();
	m_PC3RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[2], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnBnClickedBtnClose()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	pView->m_bRecipeChangeMode = FALSE;
	delete [] m_stRecipeListInfo;
	DestroyWindow();
}


void CDlgRecipe::OnCbnDropdownComboPlc()
{
	m_fnSetRecipeComboBox(0);
}

//2016.02.16 Mr.Chu
//기능 : Client Recipe 미등록되어있는 것에 관한 불합리 검사
void CDlgRecipe::m_fnCheckContents()
{
	int i,j;
	int nFlag=0,nData=0;

	int* nIndexPC;

	CString **strRecipeTempPC;
	CString strRecipe, strSectionNo, strSectionPCNo;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	//Load Data
	strRecipeTempPC = new CString*[pFrame->m_stParamInfo.nCountVisionClient];
	nIndexPC = new int[pFrame->m_stParamInfo.nCountVisionClient];
	for (nFlag = 0; nFlag < pFrame->m_stParamInfo.nCountVisionClient; nFlag++)
	{
		//Load PC1
		if(nFlag+1 == VISION_PC_1)
		{
			nIndexPC[nFlag] = m_PC1RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0;nData<nIndexPC[nFlag];nData++)
				m_PC1RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC2
		if(nFlag+1 == VISION_PC_2)
		{
			nIndexPC[nFlag] = m_PC2RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC2RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC3
		if(nFlag+1 == VISION_PC_3)
		{
			nIndexPC[nFlag] = m_PC3RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC3RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}

		//Load PC4
		if(nFlag+1 == VISION_PC_4)
		{
			nIndexPC[nFlag] = m_PC4RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC4RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC5
		if(nFlag+1 == VISION_PC_5)
		{
			nIndexPC[nFlag] = m_PC5RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC5RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC6
		if(nFlag+1 == VISION_PC_6)
		{
			nIndexPC[nFlag] = m_PC6RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC6RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC7
		if(nFlag+1 == VISION_PC_7)
		{
			nIndexPC[nFlag] = m_PC7RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC7RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC8
		if(nFlag+1 == VISION_PC_8)
		{
			nIndexPC[nFlag] = m_PC8RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC8RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
		//Load PC9
		if(nFlag+1 == VISION_PC_9)
		{
			nIndexPC[nFlag] = m_PC9RecipeListComboCtrl.GetCount();
			strRecipeTempPC[nFlag] = new CString[nIndexPC[nFlag]];

			for(nData=0; nData<nIndexPC[nFlag]; nData++)
				m_PC9RecipeListComboCtrl.GetLBText(nData, strRecipeTempPC[nFlag][nData]);
		}
	}
	
	//Compare Data
	bool bCompareOK = false;
	for(int nCount = 0; nCount<PLC_MAX_RECIPE; nCount++)
	{
		bCompareOK = false;

		for (i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			for(j=0;j<nIndexPC[i];j++)
				if( m_stRecipeListInfo[nCount].strRecipePC[i] == strRecipeTempPC[i][j])
				{
					bCompareOK = true;
					break;
				}
			if(bCompareOK == false)
			{
				strSectionNo.Format(_T("RECIPE_%d"), nCount + 1);
				strSectionPCNo.Format(_T("%s%d"),RECIPE_PC_,i+1);

				memset(m_stRecipeListInfo[nCount].strRecipePC[i],NULL,sizeof(m_stRecipeListInfo[nCount].strRecipePC[i]));
				pFrame->m_iniRecipe.WriteProfileStringW(strSectionNo, (LPCTSTR)strSectionPCNo, _T(""));
			}
		}
		
	}
	
	
	for (nFlag = 0; nFlag < pFrame->m_stParamInfo.nCountVisionClient; nFlag++)
	{
		delete [] strRecipeTempPC[nFlag];
	}
	delete [] strRecipeTempPC;
	delete [] nIndexPC;

}

void CDlgRecipe::OnCbnSelchangeComboPc4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC4RecipeListComboCtrl.GetCurSel();
	m_PC4RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[3], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC5RecipeListComboCtrl.GetCurSel();
	m_PC5RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[4], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC6RecipeListComboCtrl.GetCurSel();
	m_PC6RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[5], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc7()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC7RecipeListComboCtrl.GetCurSel();
	m_PC7RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[6], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc8()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC8RecipeListComboCtrl.GetCurSel();
	m_PC8RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[7], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}


void CDlgRecipe::OnCbnSelchangeComboPc9()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_nListRecipeCurSel < 0){
		AfxMessageBox(_T("Recipe를 선택해 주세요."));
		return;
	}

	CString strRecipe;

	int nIndex = m_PC9RecipeListComboCtrl.GetCurSel();
	m_PC9RecipeListComboCtrl.GetLBText(nIndex, strRecipe);

	_tcscpy(m_stRecipeListInfo[m_nListRecipeCurSel].strRecipePC[8], strRecipe);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}

//2016.03.03 Mr.Chu
//기능 : UI 상 Client 관련 Combo Box 와 Static Box Visible 및 Invisible 셋팅
//nEqpNo 0:AVL 1:ABI				nEqpSizeType 0:중소형 1: 대형
void CDlgRecipe::m_fnInitializeClientUi(int nEqpNo,int nEqpSizeType)
{
	if(nEqpSizeType)		//대형
	{
		if (nEqpNo == 0)
		{
			GetDlgItem(IDC_COMBO_PC7)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC7)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_COMBO_PC8)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC8)->ShowWindow(SW_HIDE);

			GetDlgItem(IDC_COMBO_PC9)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC9)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		if (nEqpNo == 0)
		{
			GetDlgItem(IDC_COMBO_PC3)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_PC3)->ShowWindow(SW_HIDE);
		}
		GetDlgItem(IDC_COMBO_PC4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC4)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC5)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC5)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC6)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC6)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC7)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC7)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC8)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_COMBO_PC9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_PC9)->ShowWindow(SW_HIDE);
	}
	//nEqpNo 0:AVL 1:ABI

}

//2016.12.30 Defect Map 좌표 QMS에 연동
void CDlgRecipe::OnBnClickedDefectMapReportCoordinateQMSType(UINT ID)
{
	m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapCoordQms =  ID;
	Invalidate(FALSE);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}

//2016.03.14 Mr.Chu				Defect Map과 Recipe Change 연동		
void CDlgRecipe::OnBnClickedDefectMapOriginType(UINT ID)
{
	m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeDefMapType =  ID;
	Invalidate(FALSE);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}
void CDlgRecipe::OnBnClickedDefectMapMajorAxisType(UINT ID)
{
	m_stRecipeListInfo[m_nListRecipeCurSel].nRecipeMajorAxisType =  ID;
	Invalidate(FALSE);
	m_fnRecipeSelView(m_nListRecipeCurSel);
}

//2016.03.14 Mr.Chu	 Defect Map , Recipe Change 연동
void CDlgRecipe::OnBnClickedButtonRecipeMapSize()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	m_stRecipeListInfo[m_nListRecipeCurSel].strRecipeDefeMapSizeX = m_strDefMapX;
	m_stRecipeListInfo[m_nListRecipeCurSel].strRecipeDefeMapSizeY = m_strDefMapY;

	m_fnRecipeSelView(m_nListRecipeCurSel);
}

//Auto Recipe UI 변경. 기존 Sequence는 그대로...    2016.04.19 Add By KTB
LRESULT CDlgRecipe::OnAutoRecipe(WPARAM wParam, LPARAM lParam)
{
	CString strRecipePC[MAX_VISION_PC], strTempRecipe = _T(""), strPLCModelBackUpPath = _T("");
	CString strMsgTemp = _T("");
	BOOL bContinue = TRUE, bComplete[MAX_VISION_PC];
	memset(bComplete, FALSE, sizeof(BOOL)*MAX_VISION_PC);
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pViewSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	pViewSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE); //2017.07.11 add by mby

	//PLC, Client 단 하나라도 문제가 있을 시 Auto Recipe 안함.
	pViewSetup->m_pDlgRecipe->EnableWindow(FALSE);
	pViewSetup->EnableWindow(FALSE);
	pFrame->m_pVsSeverIF->m_AutoRecipeMSG.Format(_T("Setting..\r\n"));
	pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0);

	long nModelChangeInterlock = -1;
	nModelChangeInterlock = pViewSetup->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_INTERLOCK);
	if(nModelChangeInterlock != OK)
	{
		strMsgTemp.Format(_T("No Ready - PLC\r\n"));
		pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
		G_AddLog_ALARM(_T("OnAutoRecipe - No Ready PLC : %d"), ERROR_INSP_AUTO_RECIPE);
		bContinue = FALSE;
	}

	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(!pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[i])
		{
			strMsgTemp.Format(_T("No Ready - Client[%d]\r\n"), i+1);
			G_AddLog_ALARM(_T("OnAutoRecipe - No Ready Client[%d] : %d"),i+1, ERROR_INSP_AUTO_RECIPE);
			pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			bContinue = FALSE;
			continue;
		}

		strRecipePC[i] = m_ListRecipe.GetItemText(i+1, 1);
		strTempRecipe = strRecipePC[i];
		strTempRecipe.Replace(_T("."), _T("#"));

		if(strTempRecipe == _T(""))
		{
			strMsgTemp.Format(_T("Recipe ID Error - Client[%d]\r\n"), i+1);
			pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			bContinue = FALSE;
		}
	}

	if(!bContinue)
	{
		strMsgTemp.Format(_T("Fail"));
		pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
		pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0, 0);
		memset(pFrame->m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
		return 0;
	}

	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		strTempRecipe = strRecipePC[i];
		strTempRecipe.Replace(_T("."), _T("#"));
		if(!pFrame->m_pVsSeverIF->m_fnSelectVisionRecipe(i+1, strTempRecipe))
		{
			strMsgTemp.Format(_T("Can Not Send - Client[%d]\r\n"), i+1);
			pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			bContinue = FALSE;
			G_AddLog_ALARM(_T("%d : OnAutoRecipe - Recipe Can not send Client"), ERROR_VISOION_SELECT_RECIPE);
		}
	}

	//약 10초 정도 클라이언트의 Model Change 완료 응답을 기다림.
	if(bContinue)
	{
		int nCnt = 0;
		while(nCnt < 100)
		{
			bContinue = TRUE;
			for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
			{
				if(pFrame->m_bAutoRecipe[i])
				{
					if(!bComplete[i])
					{
						bComplete[i] = TRUE;
						strMsgTemp.Format(_T("Complete - Client[%d]\r\n"), i+1);
						pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
						pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0);
					}
				}
				else
					bContinue = FALSE;
			}

			if(bContinue)	
				break;
			
			nCnt++;
			Sleep(100);
		}
	}
	else
	{
		strMsgTemp.Format(_T("Fail"));
		pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
		pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0, 0);
		memset(pFrame->m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
		return 0;
	}

	//클라이언트에서 모든 응답이 왔을 시 PLC 작업시작.
	if(bContinue)
	{
		if(pViewSetup->m_pDlgRecipe->m_fnChangePLCRecipe())
		{
			strMsgTemp.Format(_T("Complete - [PLC]\r\nFinish\r\n"));
			pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0);
			
			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE )
				pFrame->m_pSockManager->m_bRcpEditFlag = TRUE;			// 2017.06.30 Recipe Edit Cim에 보고

			//2020.07.10 add by mby for Recipe Backup부 Comment S-
			//	Back Up PLC Teaching Model
			//strMsgTemp.Format(_T("Back Up PLC Teaching Model...PC1 \r\n"));
			//pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			//pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0);

			//strPLCModelBackUpPath.Format(_T("%s%s"),ROOT_PATH,PLC_MODEL_PATH);
			//if( pFrame->m_DataManager.m_fnBackUpNetworkDrive(strPLCModelBackUpPath,TRUE) ) {		//Folder Copy
			//	strMsgTemp.Format(_T("Back Up Finish \r\n"));
			//	pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			//	pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0);
			//}
			////2020.07.10 add by mby for Recipe Backup부 Comment E-

			Sleep(2000);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, RECIPE_ALIGNPC_NO, m_strRcpAlignPcNo); //2021.07.07 add by mby
			pViewSetup->m_pDlgRecipe->EnableWindow(TRUE);
			pViewSetup->EnableWindow(TRUE);
			pViewSetup->m_pDlgRecipe->m_fnSetDefectMap();
			//모든 작업이 완료하면 다이얼로그 파괴
			pFrame->m_DataManager.m_fnDistroyMSGDialog();
		}
		else
		{
			strMsgTemp.Format(_T("Can Not Change - [PLC]\r\nFail"));
			pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0, 0);
			memset(pFrame->m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
			G_AddLog_ALARM(_T("%d : OnAutoRecipe - Recipe Can not change PLC"), ERROR_PLC_SELECT_RECIPE);
		}
	}
	else
	{
		for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
		{
			if(!bComplete[i])
			{	
				strMsgTemp.Format(_T("No Response - Client[%d]\r\n"), i+1);
				pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
			}
		}
		strMsgTemp.Format(_T("Fail"));
		pFrame->m_pVsSeverIF->m_AutoRecipeMSG += strMsgTemp;
		pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Auto Recipe"), pFrame->m_pVsSeverIF->m_AutoRecipeMSG, 0, 0);
		memset(pFrame->m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
	}

	memset(pFrame->m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
	return 0;
}


void CDlgRecipe::OnBnClickedBtnPlcModelRead()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( !m_fnLogin() ) 		// 최초 사용 외 사용 금지
	{
		return ;
	}

	TCHAR tszParamAddr[255], tszModelAddr[255];
	CString strParam, strModelName, strTemp;
	long lRet;
	BOOL bWRet, bCRet;
	int nEqpSize, nEqpType, nModelCnt;
	int nCount, nNo;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pViewSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	nEqpSize = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);		//중형:0 소형:1 대형:2 모니터:3
	nEqpType = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);		//0:AVL 1:ABI

	if( (nEqpSize == EQP_MEDIUM_SIZE && nEqpType == EQP_TYPE_ABI) 
		|| nEqpSize == EQP_MONITOR_SIZE
		|| (nEqpSize == EQP_SMALL_SIZE && nEqpType == EQP_TYPE_AVL) )
		nModelCnt = 20;
	else
		nModelCnt = 30;

	nModelCnt = 1;	//Cheng Du
	//for ( nNo = 1; nNo <= nModelCnt; nNo++ )	//Cheng Du //타입마다 갯수 다름
	//for ( nNo = 0; nNo < nModelCnt; nNo++ )
	//{
		
		//Cheng Du Comment
		//if(nEqpSize == EQP_LARGE_SIZE)
		//{
		//	//TV
		//	memset(tszModelAddr, 0, sizeof(tszModelAddr));	// D7000
		//	if( nNo - 1 >= 10 )
		//		_stprintf_s(tszModelAddr, 255, _T("D7%d0"), nNo - 1);
		//	else
		//		_stprintf_s(tszModelAddr, 255, _T("D70%d0"), nNo - 1);

		//	memset(tszParamAddr, 0, sizeof(tszParamAddr));
		//	_stprintf_s(tszParamAddr, 255, _T("ZR%d"), nNo * 1000);
		//}
		//else if(nEqpSize == EQP_MONITOR_SIZE)
		//{
		//	//모니터
		//	if(nEqpType == EQP_TYPE_AVL)
		//	{
		//		memset(tszModelAddr, 0, sizeof(tszModelAddr));
		//		_stprintf_s(tszModelAddr, 255, _T("D7%d0"), nNo + 49 ); // D7500
		//	}
		//	else if(nEqpType == EQP_TYPE_ABI)
		//	{
		//		memset(tszModelAddr, 0, sizeof(tszModelAddr));
		//		_stprintf_s(tszModelAddr, 255, _T("D5%d0"), nNo + 67 ); // D5680
		//	}

		//	memset(tszParamAddr, 0, sizeof(tszParamAddr));
		//	_stprintf_s(tszParamAddr, 255, _T("ZR%d"), nNo * 1000);
		//}
		//else
		//{
		//	//중소형
		//	memset(tszModelAddr, 0, sizeof(tszModelAddr));
		//	_stprintf_s(tszModelAddr, 255, _T("ZR%d0000"), nNo + 1);	//ZR20000

		//	memset(tszParamAddr, 0, sizeof(tszParamAddr));
		//	_stprintf_s(tszParamAddr, 255, _T("ZR%d0100"), nNo + 1);
		//}
		//nCount = 0;
		//lRet = -1;	bWRet = FALSE;	bCRet = FALSE;	strParam = _T(""), strModelName = _T("");
		//lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_CURRENT_MODEL_NAME, LEN_RECIPE_MODEL, strModelName);
		//lRet = pViewSetup->m_pDlgPLC->m_fnGetDevice(_T("W1A0E"));
		//lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceBlockModel(ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_READ_PARAM_SIZE, strParam);
		//while(TRUE){
		//Sleep(500);
		//if(strParam == _T("")){
		//lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceBlockModel(ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_READ_PARAM_SIZE, strParam);
		//nCount++;
		//}else{
		//break;
		//}
		//if(nCount>10){
		//break;
		//}
		//}
		//strModelName.Trim();

		//if (lRet == -1)
		//{
		//AfxMessageBox(_T("PLC Recipe Download Fail"));
		//break;
		//}
		//else
		//{
		//if(strModelName != _T(""))
		//{
		//bWRet = pFrame->m_DataManager.m_fnWriteModel(nNo, strModelName, strParam);
		//if(bWRet)
		//{
		//bCRet = pFrame->m_DataManager.m_fnCompareData(nNo, strModelName, ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_READ_PARAM_SIZE);
		//if(!bCRet)
		//{
		//AfxMessageBox(_T("PLC Recipe Compare Fail"));
		//break;
		//}
		//}
		//else
		//{
		//lRet = pViewSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
		//lRet = pViewSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
		//AfxMessageBox(_T("PLC Recipe Write Fail"));
		//break;
		//}
		//}
		//}
		//}

	nCount = 0;
	lRet = -1;	bWRet = FALSE;	bCRet = FALSE;	strParam = _T(""), strModelName = _T(""), strTemp = _T("");
	lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_CURRENT_MODEL_NAME, LEN_RECIPE_MODEL, strModelName);
	//lRet = pViewSetup->m_pDlgPLC->m_fnGetDevice(_T("W1A0E"));
	//lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceBlockModel(ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_READ_PARAM_SIZE, strParam);

	int nTemp = 0;
	int nStartIdx = 0;
	for(int nParamCnt = 0; nParamCnt < PLC_MODEL_PARAM_COUNT; nParamCnt++)
	{

		nTemp = 6656 + nParamCnt;
		memset(tszModelAddr, 0, sizeof(tszModelAddr));
		_stprintf_s(tszModelAddr, 255, _T("W%d"), nTemp);
		lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceModel(tszModelAddr);
		strTemp.Format(_T("%ld,"),lRet);
		strParam.Append(strTemp);
		if(nParamCnt == 979)
			nNo = lRet;
	}
	while(TRUE){
		Sleep(500);
		if(strParam == _T("")){
			int nTemp = 0;
			strParam = _T("");
			for(int nParamCnt = 0; nParamCnt < PLC_MODEL_PARAM_COUNT; nParamCnt++)
			{
				nTemp = 6656 + nParamCnt;
				memset(tszModelAddr, 0, sizeof(tszModelAddr));
				_stprintf_s(tszModelAddr, 255, _T("W%d"), nTemp);
				lRet = pViewSetup->m_pDlgPLC->m_fnGetDeviceModel(tszModelAddr);
				strTemp.Format(_T("%ld,"),lRet);
				strParam.Append(strTemp);
				if(nParamCnt == 979)
					nNo = lRet;
			}
			nCount++;
		}else{
			break;
		}
		if(nCount>10){
			break;
		}
	}
	strModelName.Trim();

	if (lRet == -1)
	{
		AfxMessageBox(_T("PLC Recipe Download Fail"));
		//break;
	}
	else
	{
		if(strModelName != _T(""))
		{
			bWRet = pFrame->m_DataManager.m_fnWriteModel(nNo, strModelName, strParam);
			if(bWRet)
			{
				bCRet = pFrame->m_DataManager.m_fnCompareData(nNo, strModelName, ADDR_PLC_CURRENT_MODEL_PARAM, PLC_MODEL_BUFFER);
				if(!bCRet)
				{
					AfxMessageBox(_T("PLC Recipe Compare Fail"));
					//break;
				}
			}
			else
			{
				lRet = pViewSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_REQ, FALSE);
				lRet = pViewSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
				AfxMessageBox(_T("PLC Recipe Write Fail"));
				//break;
			}
		}
	}

	//2017.07.07	Button Disable
	if( nNo >= nModelCnt )
	{
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_PLC_MODEL_READ, _T("1"));
		GetDlgItem(IDC_BTN_PLC_MODEL_READ)->EnableWindow(FALSE);	
	}

	m_fnSetRecipeComboBox(0);
}


void CDlgRecipe::OnBnClickedBtnPlcModelNameChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strModelNo, strModelName, strOldModelName, strNewModelName, strBufferModelName;
	CString strPlcModelDataPath, strOldModelNamePath, strNewModelNamePath;
	int nModelNo, nCurModelNo = -1, nCnt = 0;
	BOOL bExist = FALSE;
	long lRet = -1, nChangeCheck = -1;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	GetDlgItem(IDC_EDIT_PLC_MODEL_CHANGE_NO)->GetWindowText(strModelNo);
	GetDlgItem(IDC_EDIT_PLC_MODEL_CHANGE_NAME)->GetWindowText(strModelName);

	strModelNo.Trim();	
	strModelName.Trim();

	if(strModelNo == _T(""))
	{
		AfxMessageBox(_T("Input Model No"));
		return;
	}

	if(strModelName == _T(""))
	{
		AfxMessageBox(_T("Input Model Name"));
		return;
	}

	nModelNo = _ttoi(strModelNo);

	bExist = pFrame->m_DataManager.m_fnModelExist(nModelNo);

	if(bExist)
	{
		nCurModelNo = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_CURRENT_RECIPE_NO);
		if(nCurModelNo != -1)
		{
			//모델 번호가 현재 사용하고 있는 모델이면 현재 모델명 영역에 입력한 모델명으로 업데이트
			//모델 번호가 현재 사용하고 있는 모델이 아니면 파일명만 변경
			if(nModelNo == nCurModelNo)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_NAME_CHANGE_REQ, TRUE);
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PLC_MODEL_CHANGE_NAME_BUFFER, PLC_MODEL_NAME_SIZE, strModelName);
				lRet = pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_MODEL_CHANGE_NAME_BUFFER, LEN_RECIPE_MODEL, strBufferModelName);
				strBufferModelName.Trim();
				if(strModelName = strBufferModelName)
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, TRUE);
				else
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, TRUE);
				
				while(TRUE)
				{
					nChangeCheck = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_CHECK);
					if(nCnt >= 3 || nChangeCheck == TRUE)
					{
						lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_NAME_CHANGE_REQ, FALSE);
						lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_OK, FALSE);
						lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);
						break;
					}
					nCnt++;
					Sleep(500);
				}

			}
			strPlcModelDataPath.Format(_T("%s%s"),ROOT_PATH , PLC_MODEL_PATH);
			strOldModelName = pFrame->m_DataManager.m_fnGetModelName(nModelNo);
			strNewModelName = strModelName;

			strOldModelNamePath.Format(_T("%s%d_%s%s"), strPlcModelDataPath, nModelNo, strOldModelName, EXTENSION_DAT);
			strNewModelNamePath.Format(_T("%s%d_%s%s"), strPlcModelDataPath, nModelNo, strNewModelName, EXTENSION_DAT);

			CFile::Rename(strOldModelNamePath , strNewModelNamePath);

			pView->m_strPLCRecipe[nModelNo -1] = strModelName;

		}
	}
	else
		AfxMessageBox(_T("Not Found Model : %d"), nModelNo);
}


void CDlgRecipe::OnBnClickedBtnPlcModelCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strCurrnetModelNo, strTargetModelNo;
	int nCurrnetModelNo, nTargetModelNo;
	BOOL bExist = FALSE, bRetVal = FALSE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	GetDlgItem(IDC_EDIT_PLC_MODEL_COPY_CURRENT_NO)->GetWindowText(strCurrnetModelNo);
	GetDlgItem(IDC_EDIT_PLC_MODEL_COPY_TARGET_NO)->GetWindowText(strTargetModelNo);

	strCurrnetModelNo.Trim();	
	strTargetModelNo.Trim();

	if(strCurrnetModelNo == _T("") || strTargetModelNo == _T(""))
	{
		AfxMessageBox(_T("Input Model No"));
		return;
	}

	nCurrnetModelNo = _ttoi(strCurrnetModelNo);	
	nTargetModelNo = _ttoi(strTargetModelNo);

	bExist = pFrame->m_DataManager.m_fnModelExist(nCurrnetModelNo);

	if(bExist)
	{
		bRetVal = pFrame->m_DataManager.m_fnCopyModel(nCurrnetModelNo, nTargetModelNo);

		if(bRetVal)
		{
			AfxMessageBox(_T("Model Copy Success"));
			pView->m_strPLCRecipe[nTargetModelNo - 1] = pView->m_strPLCRecipe[nCurrnetModelNo - 1];

			//2017.07.07	PC1번 Shared 폴더 내 PLCTeachingData Recipe Back Up
			CString strPLCModelBackUpPath = _T("");
			strPLCModelBackUpPath.Format(_T("%s%s"),ROOT_PATH,PLC_MODEL_PATH);
			pFrame->m_DataManager.m_fnBackUpNetworkDrive(strPLCModelBackUpPath,TRUE); 
			//
		}
		else
			AfxMessageBox(_T("Model Copy Fail"));
	}
	else
		AfxMessageBox(_T("Not Found Model : %d"), nCurrnetModelNo);
}


void CDlgRecipe::OnBnClickedBtnPlcModelDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strDeleteModelNo;
	int nDeleteModelNo;
	int nCurModelNo;
	BOOL bExist = FALSE, bRetVal = FALSE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	GetDlgItem(IDC_EDIT_PLC_MODEL_DELETE_NO)->GetWindowText(strDeleteModelNo);

	strDeleteModelNo.Trim();

	if(strDeleteModelNo == _T(""))
	{
		AfxMessageBox(_T("Input Model No"));
		return;
	}

	nDeleteModelNo = _ttoi(strDeleteModelNo);

	bExist = pFrame->m_DataManager.m_fnModelExist(nDeleteModelNo);

	if(bExist)
	{
		// add by yskim 2017-07-14, 삭제할 모델 번호가 현재 사용하고 있는 모델이면 삭제되지 않음
		nCurModelNo = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_CURRENT_RECIPE_NO);
		if(nCurModelNo != -1)
		{
			if(nDeleteModelNo == nCurModelNo)
			{
				AfxMessageBox(_T("Current model in used is not deleted..."));
				return;
			}
		}
		
		// 지우기 전에 백업을 한다.. modify by yskim
		//2017.07.07	PC1번 Shared 폴더 내 PLCTeachingData Recipe Back Up
		CString strPLCModelBackUpPath = _T("");
		strPLCModelBackUpPath.Format(_T("%s%s"),ROOT_PATH,PLC_MODEL_PATH);
		pFrame->m_DataManager.m_fnBackUpNetworkDrive(strPLCModelBackUpPath,TRUE); 
		//
		
		bRetVal = pFrame->m_DataManager.m_fnDeleteModel(nDeleteModelNo);

		if(bRetVal)
		{
			AfxMessageBox(_T("Model Delete Success"));
			pView->m_strPLCRecipe[nDeleteModelNo -1] = _T("");
		}
		else
			AfxMessageBox(_T("Model Delete Fail"));
	}
	else
		AfxMessageBox(_T("Not Found Model : %d"), nDeleteModelNo);
}

BOOL CDlgRecipe::m_fnLogin()
{
	if(m_pDlgLogin)
	{
	delete m_pDlgLogin;
	m_pDlgLogin = NULL;
	}
	
	m_pDlgLogin = new CLogin();

	if (m_pDlgLogin->DoModal() == IDOK)
	{
		return m_pDlgLogin->m_fnGetLoginCheck();
	}

	return FALSE;
}
//2021.07.07 add by mby
void CDlgRecipe::OnBnClickedButtonAlignpcNoSet()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	if(_ttoi(m_strRcpAlignPcNo) > 1){
		AfxMessageBox(_T("Check Align PC NO (0~1)"));
		return;
	}
	m_stRecipeListInfo[m_nListRecipeCurSel].strRecipAlignPcNo = m_strRcpAlignPcNo;

	m_fnRecipeSelView(m_nListRecipeCurSel);
}
