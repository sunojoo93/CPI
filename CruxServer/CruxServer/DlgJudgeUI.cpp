
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgJudgeUI.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgJudgeUI.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CDlgJudgeUI 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgJudgeUI, CDialogEx)

CDlgJudgeUI::CDlgJudgeUI(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgJudgeUI::IDD, pParent)
{
	m_bUseJudgeCount = FALSE;
	m_DlgParam_One	= NULL;
	m_DlgParam_Two	= NULL;
}

CDlgJudgeUI::~CDlgJudgeUI()
{
	if(m_DlgParam_One)
	{
		delete m_DlgParam_One;
		m_DlgParam_One = NULL;
	}
	if(m_DlgParam_Two)
	{
		delete m_DlgParam_Two;
		m_DlgParam_Two = NULL;
	}
}

void CDlgJudgeUI::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//Count
	DDX_Check(pDX, IDC_CHECK_JUDGE_COUNT_USE, m_bUseJudgeCount);

	DDX_Control(pDX, IDC_TAB_PARAM, m_ctrlTabJudgeParam);
}


BEGIN_MESSAGE_MAP(CDlgJudgeUI, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_JUDGE_COUNT_USE, &CDlgJudgeUI::OnBnClickedCheckJudgeCountUse)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PARAM, &CDlgJudgeUI::OnTcnSelchangeTabParam)
END_MESSAGE_MAP()


// CDlgJudgeUI 메시지 처리기입니다.

void CDlgJudgeUI::m_fnDataTerminal()
{
	m_DlgParam_One->UpdateData();
	m_DlgParam_Two->UpdateData();
	for (int i = 0; i < INSP_TYPE_MAX - 1; i++)
	{
		if( i <= INSP_TFT_BURR )
		{
			m_nAryDefCount[i] = m_DlgParam_One->m_nAryDefCount[i];
			m_nAryDefLuminance[i] = m_DlgParam_One->m_nAryDefLuminance[i];
			m_dAryDistanceD1[i] = m_DlgParam_One->m_dAryDistanceD1[i];
			m_dAryDistanceD2[i] = m_DlgParam_One->m_dAryDistanceD2[i];
			m_dAryLength[i] = m_DlgParam_One->m_dAryLength[i];
			m_dAryThinknessW1[i] = m_DlgParam_One->m_dAryThinknessW1[i];
			m_dAryThinknessW2[i] = m_DlgParam_One->m_dAryThinknessW2[i];
		}
		else if( i > INSP_TFT_BURR )
		{
			m_nAryDefCount[i] = m_DlgParam_Two->m_nAryDefCount[i-INSP_CF_BROKEN];
			m_nAryDefLuminance[i] = m_DlgParam_Two->m_nAryDefLuminance[i-INSP_CF_BROKEN];
			m_dAryDistanceD1[i] = m_DlgParam_Two->m_dAryDistanceD1[i-INSP_CF_BROKEN];
			m_dAryDistanceD2[i] = m_DlgParam_Two->m_dAryDistanceD2[i-INSP_CF_BROKEN];
			m_dAryLength[i] = m_DlgParam_Two->m_dAryLength[i-INSP_CF_BROKEN];
			m_dAryThinknessW1[i] = m_DlgParam_Two->m_dAryThinknessW1[i-INSP_CF_BROKEN];
			m_dAryThinknessW2[i] = m_DlgParam_Two->m_dAryThinknessW2[i-INSP_CF_BROKEN];
		}
	}
}

void CDlgJudgeUI::m_fnLoadParameter()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_bUseJudgeCount = pFrame->m_stJudgeCount->bUseMode;

	for (int i = 0; i < INSP_TYPE_MAX - 1; i++)
	{
		m_nAryDefCount[i] = pFrame->m_stJudgeCount->nJudgeCount[i];
		m_nAryDefLuminance[i] = pFrame->m_stJudgeCount->nJudgeLuminance[i];
		m_dAryDistanceD1[i] = pFrame->m_stJudgeCount->nJudgeDistanceD1[i];
		m_dAryDistanceD2[i] = pFrame->m_stJudgeCount->nJudgeDistanceD2[i];
		m_dAryLength[i] = pFrame->m_stJudgeCount->nJudgeLength[i];
		m_dAryThinknessW1[i] = pFrame->m_stJudgeCount->nJudgeThinknessW1[i];
		m_dAryThinknessW2[i] = pFrame->m_stJudgeCount->nJudgeThinknessW2[i];

		if( i <= INSP_TFT_BURR )
		{
			m_DlgParam_One->m_nAryDefCount[i] = pFrame->m_stJudgeCount->nJudgeCount[i];
			m_DlgParam_One->m_nAryDefLuminance[i] = pFrame->m_stJudgeCount->nJudgeLuminance[i];
			m_DlgParam_One->m_dAryDistanceD1[i] = pFrame->m_stJudgeCount->nJudgeDistanceD1[i];
			m_DlgParam_One->m_dAryDistanceD2[i] = pFrame->m_stJudgeCount->nJudgeDistanceD2[i];
			m_DlgParam_One->m_dAryLength[i] = pFrame->m_stJudgeCount->nJudgeLength[i];
			m_DlgParam_One->m_dAryThinknessW1[i] = pFrame->m_stJudgeCount->nJudgeThinknessW1[i];
			m_DlgParam_One->m_dAryThinknessW2[i] = pFrame->m_stJudgeCount->nJudgeThinknessW2[i];
		}
		else if( i > INSP_TFT_BURR )
		{
			m_DlgParam_Two->m_nAryDefCount[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeCount[i];
			m_DlgParam_Two->m_nAryDefLuminance[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeLuminance[i];
			m_DlgParam_Two->m_dAryDistanceD1[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeDistanceD1[i];
			m_DlgParam_Two->m_dAryDistanceD2[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeDistanceD2[i];
			m_DlgParam_Two->m_dAryLength[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeLength[i];
			m_DlgParam_Two->m_dAryThinknessW1[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeThinknessW1[i];
			m_DlgParam_Two->m_dAryThinknessW2[i-INSP_CF_BROKEN] = pFrame->m_stJudgeCount->nJudgeThinknessW2[i];
		}
	}

	m_fnJudgeCountUseCheck(m_bUseJudgeCount);

	m_DlgParam_One->UpdateData(FALSE);
	m_DlgParam_Two->UpdateData(FALSE);
	UpdateData(FALSE);
}

void CDlgJudgeUI::m_fnSaveJudgeCount()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strJudgeCountPath, strValue;
	strJudgeCountPath.Format(_T("%s%s"), ROOT_PATH,CONFIG_COUNT_PATH );

	m_fnDataTerminal(); //2016.10.10 Tab Page 내 데이터를 원본 변수에 저장한다.

	//m_bUseJudgeCount = GetDlgItem(IDC_CHECK_JUDGE_COUNT_USE).GetDlgItemInt()
	m_bUseJudgeCount = ((CButton*)GetDlgItem(IDC_CHECK_JUDGE_COUNT_USE))->GetCheck();

	strValue.Format(_T("%d"), m_bUseJudgeCount);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%d"), m_nAryDefCount[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_COUNT, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);


	strValue.Format(_T("%d"), m_nAryDefCount[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefCount[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_COUNT, _T("ETC_POINT"), strValue, strJudgeCountPath);

	
	//Luminance
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);

	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%d"), m_nAryDefLuminance[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_LUMINANCE, _T("ETC_POINT"), strValue, strJudgeCountPath);


	//Distance_D1
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);

	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD1[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D1, _T("ETC_POINT"), strValue, strJudgeCountPath);

	//Distance_D2
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);

	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryDistanceD2[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_DISTANCE_D2, _T("ETC_POINT"), strValue, strJudgeCountPath);

	//LENGTH
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%0.2f"), m_dAryLength[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);

	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryLength[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_LENGTH, _T("ETC_POINT"), strValue, strJudgeCountPath);

	//Width_W1
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_ALIEN_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_ALIEN_SUBSTANCE"), strValue, strJudgeCountPath);
	//strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_POL_MINOR]);
	//WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("POL_MINOR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);
	
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW1[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W1, _T("ETC_POINT"), strValue, strJudgeCountPath);

	//WIDTH_W2
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_DUST]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("DUST"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_WHITE_TURBIDITY]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("WHITE_TURBIDITY"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_ITO]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("ITO"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_EDGE_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("EDGE_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_PAD_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("PAD_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_DIMPLE_POINT]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("DIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_ETCHING_LINE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("ETCHING_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_DIMPLE_LINE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("DIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_PAD_BURNT]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("PAD_BURNT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_POL_BUBBLE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("POL_BUBBLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_POL_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("POL_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_POL_ALIEN_SUBSTANCE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_POL_ALIEN_SUBSTANCE+1]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_POL_SUBSTANCE"), strValue, strJudgeCountPath);
	/*strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_POL_MINOR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("POL_MINOR"), strValue, strJudgeCountPath);*/
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_BLACK_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("BLACK_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_WHITE_SCRATCH]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("WHITE_SCRATCH"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_TFT_MEASURE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_MEASURE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_MURA]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("MURA"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_NO_USE]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("NO_USE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_TFT_BURR]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_BURR"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_TFT_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_TFT_CORNER_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_CORNER_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("CF_CELL_BROKEN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_TFT_CELL_BROKEN]);
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("TFT_CELL_BROKEN"), strValue, strJudgeCountPath);

	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_PIMPLE_POINT] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_POINT"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_PIMPLE_LINE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_PIMPLE_CIRCLE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_CIRCLE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_CF_WAVE_PATTERN] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("WAVE_PATTERN"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_ETC_LINE] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("ETC_LINE"), strValue, strJudgeCountPath);
	strValue.Format(_T("%0.2f"), m_dAryThinknessW2[INSP_ETC_POINT] );
	WritePrivateProfileString(DEF_JUDGE_WIDTH_W2, _T("ETC_POINT"), strValue, strJudgeCountPath);
	
	pFrame->m_fnLoadJudgeCount();
}

void CDlgJudgeUI::m_fnJudgeCountUseCheck(BOOL bUseCheck)
{

	int nEqpModel;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	//0:AVL 1:ABI
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	if (nEqpModel == 0)
	{
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT28)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT34)->EnableWindow(bUseCheck);

		//Luminance
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE28)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE34)->EnableWindow(bUseCheck);


		//Distance_D1
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_28)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_34)->EnableWindow(bUseCheck);

		//Distance_D2
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_28)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_34)->EnableWindow(bUseCheck);

		//LENGTH
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH28)->EnableWindow(bUseCheck);

		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH34)->EnableWindow(bUseCheck);

		//WIDTH_W1
		//page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_28)->EnableWindow(bUseCheck);


		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_34)->EnableWindow(bUseCheck);
		
		//WIDTH_W2
		//Page1
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_1)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_2)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_5)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_6)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_7)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_15)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_28)->EnableWindow(bUseCheck);


		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_29)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_30)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_31)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_32)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_33)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_34)->EnableWindow(bUseCheck);
	}
	else if(nEqpModel == 1)
	{
		//COUNT
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_COUNT28)->EnableWindow(bUseCheck);

		//LUMINANCE
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LUMINANCE22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LUMINANCE28)->EnableWindow(bUseCheck);

		//Distance_D1
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D1_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D1_28)->EnableWindow(bUseCheck);

		//Distance_D1
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_DISTANCE_D2_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_DISTANCE_D2_28)->EnableWindow(bUseCheck);

		//LENGTH
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_LENGTH22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_LENGTH28)->EnableWindow(bUseCheck);

		//WIDTH_D1
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W1_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W1_28)->EnableWindow(bUseCheck);

		//WIDTH_D1
		//Page1
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT1)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT2)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT3)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT4)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_5)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT6)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT7)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT8)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_9)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT10)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_11)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_12)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_13)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_14)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT15)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT16)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_17)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_18)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_19)->EnableWindow(bUseCheck);
		//m_DlgParam_One->GetDlgItem(IDC_JUDGE_COUNT20)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_21)->EnableWindow(bUseCheck);
		m_DlgParam_One->GetDlgItem(IDC_JUDGE_WIDTH_W2_22)->EnableWindow(bUseCheck);

		//Page2
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_23)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_24)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_25)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_26)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_27)->EnableWindow(bUseCheck);
		m_DlgParam_Two->GetDlgItem(IDC_JUDGE_WIDTH_W2_28)->EnableWindow(bUseCheck);
	}

}


void CDlgJudgeUI::OnBnClickedCheckJudgeCountUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_JUDGE_COUNT_USE))->GetCheck();
	m_fnJudgeCountUseCheck(bCheck);
	pFrame->m_stJudgeCount->bUseMode = bCheck;
}


BOOL CDlgJudgeUI::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	RECT rc;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_DlgParam_One = new CDlgJudgeUIParam1();
	m_DlgParam_One->Create(IDD_DLG_JUDGE_UI_PARAM_1, this);

	m_DlgParam_Two = new CDlgJudgeUIParam2();
	m_DlgParam_Two->Create(IDD_DLG_JUDGE_UI_PARAM_2, this);

	//Setting Tab control 2016.10.10 Mr.Chu
	SetWindowTheme(GetDlgItem(IDC_TAB_PARAM)->GetSafeHwnd(), L"", L"");

	m_ctrlTabJudgeParam.InsertItem(1, _T("Page1"));
	m_ctrlTabJudgeParam.InsertItem(2, _T("Page2"));

	m_DlgParam_One->ShowWindow(SW_SHOW);

	m_DlgParam_One->GetWindowRect(&rc);
	m_DlgParam_One->GetClientRect(&rc);
	m_DlgParam_One->MoveWindow(&rc);
	m_DlgParam_One->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
	//

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgJudgeUI::OnTcnSelchangeTabParam(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RECT rc;
	int nIndex = m_ctrlTabJudgeParam.GetCurSel();

	m_DlgParam_One->ShowWindow(SW_HIDE);
	m_DlgParam_Two->ShowWindow(SW_HIDE);

	switch(nIndex)
	{
	case TAPCTRL_PAGE1:
		m_DlgParam_One->ShowWindow(SW_SHOW);

		m_DlgParam_One->GetWindowRect(&rc);
		m_DlgParam_One->GetClientRect(&rc);
		m_DlgParam_One->MoveWindow(&rc);
		m_DlgParam_One->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
		break;
	case TAPCTRL_PAGE2:
		m_DlgParam_Two->ShowWindow(SW_SHOW);

		m_DlgParam_Two->GetWindowRect(&rc);
		m_DlgParam_Two->GetClientRect(&rc);
		m_DlgParam_Two->MoveWindow(&rc);
		m_DlgParam_Two->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
		break;
	}

	*pResult = 0;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgJudgeUIParam1, CDialogEx)
CDlgJudgeUIParam1::CDlgJudgeUIParam1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgJudgeUIParam1::IDD, pParent)
{


}

CDlgJudgeUIParam1::~CDlgJudgeUIParam1()
{

}

BEGIN_MESSAGE_MAP(CDlgJudgeUIParam1, CDialogEx)

END_MESSAGE_MAP()

void CDlgJudgeUIParam1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//Count
 	DDX_Text(pDX, IDC_JUDGE_COUNT1, m_nAryDefCount[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT2, m_nAryDefCount[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT3, m_nAryDefCount[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT4, m_nAryDefCount[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT5, m_nAryDefCount[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT6, m_nAryDefCount[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT7, m_nAryDefCount[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT8, m_nAryDefCount[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT9, m_nAryDefCount[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT10, m_nAryDefCount[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT11, m_nAryDefCount[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT12, m_nAryDefCount[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT13, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT14, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT15, m_nAryDefCount[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT16, m_nAryDefCount[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT17, m_nAryDefCount[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT18, m_nAryDefCount[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT19, m_nAryDefCount[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT20, m_nAryDefCount[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT21, m_nAryDefCount[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT22, m_nAryDefCount[INSP_TFT_BURR]);
 
 
 
 
 	//Luminance
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE1, m_nAryDefLuminance[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE2, m_nAryDefLuminance[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE3, m_nAryDefLuminance[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE4, m_nAryDefLuminance[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE5, m_nAryDefLuminance[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE6, m_nAryDefLuminance[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE7, m_nAryDefLuminance[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE8, m_nAryDefLuminance[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE9, m_nAryDefLuminance[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE10, m_nAryDefLuminance[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE11, m_nAryDefLuminance[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE12, m_nAryDefLuminance[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE13, m_nAryDefLuminance[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE14, m_nAryDefLuminance[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE15, m_nAryDefLuminance[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE16, m_nAryDefLuminance[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE17, m_nAryDefLuminance[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE18, m_nAryDefLuminance[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE19, m_nAryDefLuminance[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE20, m_nAryDefLuminance[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE21, m_nAryDefLuminance[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_LUMINANCE22, m_nAryDefLuminance[INSP_TFT_BURR]);
 
 	//Distance_D1
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_1, m_dAryDistanceD1[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_2, m_dAryDistanceD1[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_3, m_dAryDistanceD1[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_4, m_dAryDistanceD1[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_5, m_dAryDistanceD1[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_6, m_dAryDistanceD1[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_7, m_dAryDistanceD1[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_8, m_dAryDistanceD1[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_9, m_dAryDistanceD1[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_10, m_dAryDistanceD1[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_11, m_dAryDistanceD1[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_12, m_dAryDistanceD1[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_13, m_dAryDistanceD1[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_14, m_dAryDistanceD1[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_15, m_dAryDistanceD1[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_16, m_dAryDistanceD1[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_17, m_dAryDistanceD1[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_18, m_dAryDistanceD1[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_19, m_dAryDistanceD1[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_20, m_dAryDistanceD1[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_21, m_dAryDistanceD1[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_22, m_dAryDistanceD1[INSP_TFT_BURR]);
 
 	//Distance_D2
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_1, m_dAryDistanceD2[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_2, m_dAryDistanceD2[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_3, m_dAryDistanceD2[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_4, m_dAryDistanceD2[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_5, m_dAryDistanceD2[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_6, m_dAryDistanceD2[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_7, m_dAryDistanceD2[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_8, m_dAryDistanceD2[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_9, m_dAryDistanceD2[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_10, m_dAryDistanceD2[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_11, m_dAryDistanceD2[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_12, m_dAryDistanceD2[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_13, m_dAryDistanceD2[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_14, m_dAryDistanceD2[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_15, m_dAryDistanceD2[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_16, m_dAryDistanceD2[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_17, m_dAryDistanceD2[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_18, m_dAryDistanceD2[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_19, m_dAryDistanceD2[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_20, m_dAryDistanceD2[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_21, m_dAryDistanceD2[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_22, m_dAryDistanceD2[INSP_TFT_BURR]);
 
 
 	//LENGTH
 	DDX_Text(pDX, IDC_JUDGE_LENGTH1, m_dAryLength[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH2, m_dAryLength[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH3, m_dAryLength[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH4, m_dAryLength[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH5, m_dAryLength[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH6, m_dAryLength[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH7, m_dAryLength[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH8, m_dAryLength[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH9, m_dAryLength[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH10, m_dAryLength[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH11, m_dAryLength[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH12, m_dAryLength[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH13, m_dAryLength[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH14, m_dAryLength[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH15, m_dAryLength[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH16, m_dAryLength[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH17, m_dAryLength[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH18, m_dAryLength[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH19, m_dAryLength[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH20, m_dAryLength[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH21, m_dAryLength[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_LENGTH22, m_dAryLength[INSP_TFT_BURR]);
 
 	//WIDTH_W1
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_1, m_dAryThinknessW1[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_2, m_dAryThinknessW1[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_3, m_dAryThinknessW1[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_4, m_dAryThinknessW1[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_5, m_dAryThinknessW1[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_6, m_dAryThinknessW1[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_7, m_dAryThinknessW1[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_8, m_dAryThinknessW1[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_9, m_dAryThinknessW1[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_10, m_dAryThinknessW1[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_11, m_dAryThinknessW1[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_12, m_dAryThinknessW1[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_13, m_dAryThinknessW1[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_14, m_dAryThinknessW1[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_15, m_dAryThinknessW1[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_16, m_dAryThinknessW1[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_17, m_dAryThinknessW1[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_18, m_dAryThinknessW1[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_19, m_dAryThinknessW1[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_20, m_dAryThinknessW1[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_21, m_dAryThinknessW1[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_22, m_dAryThinknessW1[INSP_TFT_BURR]);
 
 	//WIDTH_W2
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_1, m_dAryThinknessW2[INSP_DUST]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_2, m_dAryThinknessW2[INSP_WHITE_TURBIDITY]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_3, m_dAryThinknessW2[INSP_ITO]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_4, m_dAryThinknessW2[INSP_EDGE_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_5, m_dAryThinknessW2[INSP_PAD_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_6, m_dAryThinknessW2[INSP_DIMPLE_POINT]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_7, m_dAryThinknessW2[INSP_ETCHING_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_8, m_dAryThinknessW2[INSP_DIMPLE_LINE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_9, m_dAryThinknessW2[INSP_PAD_BURNT]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_10, m_dAryThinknessW2[INSP_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_11, m_dAryThinknessW2[INSP_POL_BUBBLE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_12, m_dAryThinknessW2[INSP_POL_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_13, m_dAryThinknessW2[INSP_POL_ALIEN_SUBSTANCE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_14, m_dAryThinknessW2[INSP_POL_ALIEN_SUBSTANCE+1]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_15, m_dAryThinknessW2[INSP_BLACK_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_16, m_dAryThinknessW2[INSP_WHITE_SCRATCH]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_17, m_dAryThinknessW2[INSP_CF_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_18, m_dAryThinknessW2[INSP_TFT_MEASURE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_19, m_dAryThinknessW2[INSP_MURA]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_20, m_dAryThinknessW2[INSP_NO_USE]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_21, m_dAryThinknessW2[INSP_CF_BURR]);
 	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_22, m_dAryThinknessW2[INSP_TFT_BURR]);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgJudgeUIParam2, CDialogEx)
CDlgJudgeUIParam2::CDlgJudgeUIParam2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgJudgeUIParam2::IDD, pParent)
{


}

CDlgJudgeUIParam2::~CDlgJudgeUIParam2()
{

}

void CDlgJudgeUIParam2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//Count
 	DDX_Text(pDX, IDC_JUDGE_COUNT23, m_nAryDefCount[INSP_CF_BROKEN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT24, m_nAryDefCount[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT25, m_nAryDefCount[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT26, m_nAryDefCount[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT27, m_nAryDefCount[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT28, m_nAryDefCount[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
 
 	DDX_Text(pDX, IDC_JUDGE_COUNT29, m_nAryDefCount[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT30, m_nAryDefCount[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT31, m_nAryDefCount[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT32, m_nAryDefCount[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT33, m_nAryDefCount[INSP_ETC_LINE - INSP_CF_BROKEN]);
 	DDX_Text(pDX, IDC_JUDGE_COUNT34, m_nAryDefCount[INSP_ETC_POINT - INSP_CF_BROKEN]);
 
 
 
 
  	//Luminance
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE23, m_nAryDefLuminance[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE24, m_nAryDefLuminance[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE25, m_nAryDefLuminance[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE26, m_nAryDefLuminance[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE27, m_nAryDefLuminance[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE28, m_nAryDefLuminance[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE29, m_nAryDefLuminance[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE30, m_nAryDefLuminance[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE31, m_nAryDefLuminance[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE32, m_nAryDefLuminance[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE33, m_nAryDefLuminance[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LUMINANCE34, m_nAryDefLuminance[INSP_ETC_POINT - INSP_CF_BROKEN]);
  
  	//Distance_D1
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_23, m_dAryDistanceD1[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_24, m_dAryDistanceD1[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_25, m_dAryDistanceD1[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_26, m_dAryDistanceD1[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_27, m_dAryDistanceD1[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_28, m_dAryDistanceD1[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_29, m_dAryDistanceD1[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_30, m_dAryDistanceD1[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_31, m_dAryDistanceD1[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_32, m_dAryDistanceD1[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_33, m_dAryDistanceD1[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D1_34, m_dAryDistanceD1[INSP_ETC_POINT - INSP_CF_BROKEN]);
  
  	//Distance_D2
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_23, m_dAryDistanceD2[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_24, m_dAryDistanceD2[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_25, m_dAryDistanceD2[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_26, m_dAryDistanceD2[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_27, m_dAryDistanceD2[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_28, m_dAryDistanceD2[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_29, m_dAryDistanceD2[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_30, m_dAryDistanceD2[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_31, m_dAryDistanceD2[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_32, m_dAryDistanceD2[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_33, m_dAryDistanceD2[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_DISTANCE_D2_34, m_dAryDistanceD2[INSP_ETC_POINT - INSP_CF_BROKEN]);
  
  
  	//LENGTH
  	DDX_Text(pDX, IDC_JUDGE_LENGTH23, m_dAryLength[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH24, m_dAryLength[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH25, m_dAryLength[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH26, m_dAryLength[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH27, m_dAryLength[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH28, m_dAryLength[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_LENGTH29, m_dAryLength[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH30, m_dAryLength[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH31, m_dAryLength[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH32, m_dAryLength[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH33, m_dAryLength[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_LENGTH34, m_dAryLength[INSP_ETC_POINT - INSP_CF_BROKEN]);
  
  
  	//WIDTH_W1
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_23, m_dAryThinknessW1[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_24, m_dAryThinknessW1[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_25, m_dAryThinknessW1[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_26, m_dAryThinknessW1[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_27, m_dAryThinknessW1[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_28, m_dAryThinknessW1[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_29, m_dAryThinknessW1[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_30, m_dAryThinknessW1[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_31, m_dAryThinknessW1[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_32, m_dAryThinknessW1[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_33, m_dAryThinknessW1[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W1_34, m_dAryThinknessW1[INSP_ETC_POINT - INSP_CF_BROKEN]);
  
  	//WIDTH_W2
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_23, m_dAryThinknessW2[INSP_CF_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_24, m_dAryThinknessW2[INSP_TFT_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_25, m_dAryThinknessW2[INSP_CF_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_26, m_dAryThinknessW2[INSP_TFT_CORNER_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_27, m_dAryThinknessW2[INSP_CF_CELL_BROKEN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_28, m_dAryThinknessW2[INSP_TFT_CELL_BROKEN - INSP_CF_BROKEN]);
  
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_29, m_dAryThinknessW2[INSP_PIMPLE_POINT - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_30, m_dAryThinknessW2[INSP_PIMPLE_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_31, m_dAryThinknessW2[INSP_CF_PIMPLE_CIRCLE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_32, m_dAryThinknessW2[INSP_CF_WAVE_PATTERN - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_33, m_dAryThinknessW2[INSP_ETC_LINE - INSP_CF_BROKEN]);
  	DDX_Text(pDX, IDC_JUDGE_WIDTH_W2_34, m_dAryThinknessW2[INSP_ETC_POINT - INSP_CF_BROKEN]);
}

BEGIN_MESSAGE_MAP(CDlgJudgeUIParam2, CDialogEx)

END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////