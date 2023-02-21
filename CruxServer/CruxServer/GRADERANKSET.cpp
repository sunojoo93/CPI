// GRADERANKSET.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "GRADERANKSET.h"
#include "afxdialogex.h"
#include "DataManager.h"
#include "DefineVal_BIG.h"
#include "MainFrm.h"
#include "ViewAuto.h"


// CGRADERANKSET 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGRADERANKSET, CDialogEx)

CGRADERANKSET::CGRADERANKSET(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGRADERANKSET::IDD, pParent)
{
	for(int i = 0 ; i < 16 ; i++)
	{
		m_nAryGradeRankChkA_X[i] = FALSE;
		m_nAryGradeRankChkA_Y[i] = FALSE;
		m_nAryGradeRankChkB_X [i] = FALSE;
		m_nAryGradeRankChkB_Y[i] = FALSE;
		m_nAryGradeRankChkB2_X [i] = FALSE;
		m_nAryGradeRankChkB2_Y[i] = FALSE;
		m_nAryGradeRankChkC_X [i] = FALSE;
		m_nAryGradeRankChkC_Y[i] = FALSE;

		m_strAryGradeRankA_X[i] = _T("");
		m_strAryGradeRankA_Y[i] = _T("");
		m_strAryGradeRankC_X[i] = _T("");
		m_strAryGradeRankC_Y[i] = _T("");

		m_strAryGradeB_Rank[i] = _T("");
		m_strAryGradeB2_Rank[i] = _T("");
	}
	for(int i = 0 ; i < 32 ; i++)
	{
		m_strAryGradeRankB_X[i] = _T("");
		m_strAryGradeRankB_Y[i] = _T("");
		m_strAryGradeRankB2_X[i] = _T("");
		m_strAryGradeRankB2_Y[i] = _T("");
	}
}

CGRADERANKSET::~CGRADERANKSET()
{
}

void CGRADERANKSET::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//GRADE A - X
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X1,  m_nAryGradeRankChkA_X[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X2,  m_nAryGradeRankChkA_X[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X3,  m_nAryGradeRankChkA_X[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X4,  m_nAryGradeRankChkA_X[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X5,  m_nAryGradeRankChkA_X[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X6,  m_nAryGradeRankChkA_X[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X7,  m_nAryGradeRankChkA_X[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X8,  m_nAryGradeRankChkA_X[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X9,  m_nAryGradeRankChkA_X[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X10, m_nAryGradeRankChkA_X[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X11, m_nAryGradeRankChkA_X[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X12, m_nAryGradeRankChkA_X[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X13, m_nAryGradeRankChkA_X[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X14, m_nAryGradeRankChkA_X[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X15, m_nAryGradeRankChkA_X[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_X16, m_nAryGradeRankChkA_X[15]);
	//GRADE A - Y
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y1,  m_nAryGradeRankChkA_Y[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y2,  m_nAryGradeRankChkA_Y[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y3,  m_nAryGradeRankChkA_Y[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y4,  m_nAryGradeRankChkA_Y[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y5,  m_nAryGradeRankChkA_Y[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y6,  m_nAryGradeRankChkA_Y[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y7,  m_nAryGradeRankChkA_Y[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y8,  m_nAryGradeRankChkA_Y[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y9,  m_nAryGradeRankChkA_Y[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y10, m_nAryGradeRankChkA_Y[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y11, m_nAryGradeRankChkA_Y[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y12, m_nAryGradeRankChkA_Y[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y13, m_nAryGradeRankChkA_Y[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y14, m_nAryGradeRankChkA_Y[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y15, m_nAryGradeRankChkA_Y[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_A_Y16, m_nAryGradeRankChkA_Y[15]);
	//GRADE A - X Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X1,  m_strAryGradeRankA_X[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X2,  m_strAryGradeRankA_X[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X3,  m_strAryGradeRankA_X[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X4,  m_strAryGradeRankA_X[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X5,  m_strAryGradeRankA_X[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X6,  m_strAryGradeRankA_X[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X7,  m_strAryGradeRankA_X[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X8,  m_strAryGradeRankA_X[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X9,  m_strAryGradeRankA_X[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X10, m_strAryGradeRankA_X[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X11, m_strAryGradeRankA_X[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X12, m_strAryGradeRankA_X[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X13, m_strAryGradeRankA_X[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X14, m_strAryGradeRankA_X[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X15, m_strAryGradeRankA_X[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_X16, m_strAryGradeRankA_X[15]);
	//GRADE A - Y Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y1,  m_strAryGradeRankA_Y[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y2,  m_strAryGradeRankA_Y[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y3,  m_strAryGradeRankA_Y[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y4,  m_strAryGradeRankA_Y[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y5,  m_strAryGradeRankA_Y[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y6,  m_strAryGradeRankA_Y[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y7,  m_strAryGradeRankA_Y[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y8,  m_strAryGradeRankA_Y[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y9,  m_strAryGradeRankA_Y[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y10, m_strAryGradeRankA_Y[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y11, m_strAryGradeRankA_Y[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y12, m_strAryGradeRankA_Y[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y13, m_strAryGradeRankA_Y[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y14, m_strAryGradeRankA_Y[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y15, m_strAryGradeRankA_Y[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_A_Y16, m_strAryGradeRankA_Y[15]);

	//GRADE B1 - X
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X1,  m_nAryGradeRankChkB_X[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X2,  m_nAryGradeRankChkB_X[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X3,  m_nAryGradeRankChkB_X[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X4,  m_nAryGradeRankChkB_X[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X5,  m_nAryGradeRankChkB_X[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X6,  m_nAryGradeRankChkB_X[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X7,  m_nAryGradeRankChkB_X[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X8,  m_nAryGradeRankChkB_X[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X9,  m_nAryGradeRankChkB_X[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X10, m_nAryGradeRankChkB_X[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X11, m_nAryGradeRankChkB_X[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X12, m_nAryGradeRankChkB_X[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X13, m_nAryGradeRankChkB_X[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X14, m_nAryGradeRankChkB_X[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X15, m_nAryGradeRankChkB_X[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_X16, m_nAryGradeRankChkB_X[15]);
	//GRADE B1 - Y
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y1,  m_nAryGradeRankChkB_Y[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y2,  m_nAryGradeRankChkB_Y[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y3,  m_nAryGradeRankChkB_Y[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y4,  m_nAryGradeRankChkB_Y[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y5,  m_nAryGradeRankChkB_Y[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y6,  m_nAryGradeRankChkB_Y[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y7,  m_nAryGradeRankChkB_Y[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y8,  m_nAryGradeRankChkB_Y[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y9,  m_nAryGradeRankChkB_Y[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y10, m_nAryGradeRankChkB_Y[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y11, m_nAryGradeRankChkB_Y[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y12, m_nAryGradeRankChkB_Y[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y13, m_nAryGradeRankChkB_Y[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y14, m_nAryGradeRankChkB_Y[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y15, m_nAryGradeRankChkB_Y[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B_Y16, m_nAryGradeRankChkB_Y[15]);
	//GRADE B1 - X Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X1,  m_strAryGradeRankB_X[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X2,  m_strAryGradeRankB_X[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X3,  m_strAryGradeRankB_X[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X4,  m_strAryGradeRankB_X[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X5,  m_strAryGradeRankB_X[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X6,  m_strAryGradeRankB_X[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X7,  m_strAryGradeRankB_X[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X8,  m_strAryGradeRankB_X[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X9,  m_strAryGradeRankB_X[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X10, m_strAryGradeRankB_X[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X11, m_strAryGradeRankB_X[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X12, m_strAryGradeRankB_X[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X13, m_strAryGradeRankB_X[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X14, m_strAryGradeRankB_X[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X15, m_strAryGradeRankB_X[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X16, m_strAryGradeRankB_X[15]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X17, m_strAryGradeRankB_X[16]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X18, m_strAryGradeRankB_X[17]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X19, m_strAryGradeRankB_X[18]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X20, m_strAryGradeRankB_X[19]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X21, m_strAryGradeRankB_X[20]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X22, m_strAryGradeRankB_X[21]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X23, m_strAryGradeRankB_X[22]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X24, m_strAryGradeRankB_X[23]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X25, m_strAryGradeRankB_X[24]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X26, m_strAryGradeRankB_X[25]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X27, m_strAryGradeRankB_X[26]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X28, m_strAryGradeRankB_X[27]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X29, m_strAryGradeRankB_X[28]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X30, m_strAryGradeRankB_X[29]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X31, m_strAryGradeRankB_X[30]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_X32, m_strAryGradeRankB_X[31]);
	//GRADE B1 - Y Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y1,  m_strAryGradeRankB_Y[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y2,  m_strAryGradeRankB_Y[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y3,  m_strAryGradeRankB_Y[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y4,  m_strAryGradeRankB_Y[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y5,  m_strAryGradeRankB_Y[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y6,  m_strAryGradeRankB_Y[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y7,  m_strAryGradeRankB_Y[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y8,  m_strAryGradeRankB_Y[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y9,  m_strAryGradeRankB_Y[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y10, m_strAryGradeRankB_Y[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y11, m_strAryGradeRankB_Y[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y12, m_strAryGradeRankB_Y[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y13, m_strAryGradeRankB_Y[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y14, m_strAryGradeRankB_Y[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y15, m_strAryGradeRankB_Y[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y16, m_strAryGradeRankB_Y[15]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y17, m_strAryGradeRankB_Y[16]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y18, m_strAryGradeRankB_Y[17]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y19, m_strAryGradeRankB_Y[18]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y20, m_strAryGradeRankB_Y[19]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y21, m_strAryGradeRankB_Y[20]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y22, m_strAryGradeRankB_Y[21]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y23, m_strAryGradeRankB_Y[22]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y24, m_strAryGradeRankB_Y[23]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y25, m_strAryGradeRankB_Y[24]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y26, m_strAryGradeRankB_Y[25]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y27, m_strAryGradeRankB_Y[26]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y28, m_strAryGradeRankB_Y[27]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y29, m_strAryGradeRankB_Y[28]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y30, m_strAryGradeRankB_Y[29]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y31, m_strAryGradeRankB_Y[30]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_Y32, m_strAryGradeRankB_Y[31]);

	//GRADE B1 - RANK
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK1,  m_strAryGradeB_Rank[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK2,  m_strAryGradeB_Rank[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK3,  m_strAryGradeB_Rank[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK4,  m_strAryGradeB_Rank[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK5,  m_strAryGradeB_Rank[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK6,  m_strAryGradeB_Rank[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK7,  m_strAryGradeB_Rank[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK8,  m_strAryGradeB_Rank[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK9,  m_strAryGradeB_Rank[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK10, m_strAryGradeB_Rank[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK11, m_strAryGradeB_Rank[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK12, m_strAryGradeB_Rank[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK13, m_strAryGradeB_Rank[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK14, m_strAryGradeB_Rank[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK15, m_strAryGradeB_Rank[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B_RANK16, m_strAryGradeB_Rank[15]);

	//GRADE B2 - X
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X1,  m_nAryGradeRankChkB2_X[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X2,  m_nAryGradeRankChkB2_X[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X3,  m_nAryGradeRankChkB2_X[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X4,  m_nAryGradeRankChkB2_X[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X5,  m_nAryGradeRankChkB2_X[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X6,  m_nAryGradeRankChkB2_X[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X7,  m_nAryGradeRankChkB2_X[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X8,  m_nAryGradeRankChkB2_X[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X9,  m_nAryGradeRankChkB2_X[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X10, m_nAryGradeRankChkB2_X[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X11, m_nAryGradeRankChkB2_X[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X12, m_nAryGradeRankChkB2_X[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X13, m_nAryGradeRankChkB2_X[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X14, m_nAryGradeRankChkB2_X[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X15, m_nAryGradeRankChkB2_X[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_X16, m_nAryGradeRankChkB2_X[15]);
	//GRADE B2 - Y
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y1,  m_nAryGradeRankChkB2_Y[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y2,  m_nAryGradeRankChkB2_Y[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y3,  m_nAryGradeRankChkB2_Y[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y4,  m_nAryGradeRankChkB2_Y[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y5,  m_nAryGradeRankChkB2_Y[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y6,  m_nAryGradeRankChkB2_Y[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y7,  m_nAryGradeRankChkB2_Y[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y8,  m_nAryGradeRankChkB2_Y[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y9,  m_nAryGradeRankChkB2_Y[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y10, m_nAryGradeRankChkB2_Y[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y11, m_nAryGradeRankChkB2_Y[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y12, m_nAryGradeRankChkB2_Y[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y13, m_nAryGradeRankChkB2_Y[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y14, m_nAryGradeRankChkB2_Y[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y15, m_nAryGradeRankChkB2_Y[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_B2_Y16, m_nAryGradeRankChkB2_Y[15]);
	//GRADE B2 - X Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X1,  m_strAryGradeRankB2_X[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X2,  m_strAryGradeRankB2_X[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X3,  m_strAryGradeRankB2_X[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X4,  m_strAryGradeRankB2_X[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X5,  m_strAryGradeRankB2_X[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X6,  m_strAryGradeRankB2_X[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X7,  m_strAryGradeRankB2_X[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X8,  m_strAryGradeRankB2_X[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X9,  m_strAryGradeRankB2_X[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X10, m_strAryGradeRankB2_X[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X11, m_strAryGradeRankB2_X[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X12, m_strAryGradeRankB2_X[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X13, m_strAryGradeRankB2_X[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X14, m_strAryGradeRankB2_X[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X15, m_strAryGradeRankB2_X[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X16, m_strAryGradeRankB2_X[15]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X17, m_strAryGradeRankB2_X[16]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X18, m_strAryGradeRankB2_X[17]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X19, m_strAryGradeRankB2_X[18]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X20, m_strAryGradeRankB2_X[19]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X21, m_strAryGradeRankB2_X[20]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X22, m_strAryGradeRankB2_X[21]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X23, m_strAryGradeRankB2_X[22]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X24, m_strAryGradeRankB2_X[23]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X25, m_strAryGradeRankB2_X[24]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X26, m_strAryGradeRankB2_X[25]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X27, m_strAryGradeRankB2_X[26]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X28, m_strAryGradeRankB2_X[27]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X29, m_strAryGradeRankB2_X[28]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X30, m_strAryGradeRankB2_X[29]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X31, m_strAryGradeRankB2_X[30]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_X32, m_strAryGradeRankB2_X[31]);
	//GRADE B2 - Y Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y1,  m_strAryGradeRankB2_Y[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y2,  m_strAryGradeRankB2_Y[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y3,  m_strAryGradeRankB2_Y[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y4,  m_strAryGradeRankB2_Y[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y5,  m_strAryGradeRankB2_Y[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y6,  m_strAryGradeRankB2_Y[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y7,  m_strAryGradeRankB2_Y[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y8,  m_strAryGradeRankB2_Y[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y9,  m_strAryGradeRankB2_Y[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y10, m_strAryGradeRankB2_Y[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y11, m_strAryGradeRankB2_Y[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y12, m_strAryGradeRankB2_Y[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y13, m_strAryGradeRankB2_Y[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y14, m_strAryGradeRankB2_Y[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y15, m_strAryGradeRankB2_Y[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y16, m_strAryGradeRankB2_Y[15]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y17, m_strAryGradeRankB2_Y[16]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y18, m_strAryGradeRankB2_Y[17]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y19, m_strAryGradeRankB2_Y[18]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y20, m_strAryGradeRankB2_Y[19]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y21, m_strAryGradeRankB2_Y[20]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y22, m_strAryGradeRankB2_Y[21]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y23, m_strAryGradeRankB2_Y[22]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y24, m_strAryGradeRankB2_Y[23]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y25, m_strAryGradeRankB2_Y[24]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y26, m_strAryGradeRankB2_Y[25]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y27, m_strAryGradeRankB2_Y[26]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y28, m_strAryGradeRankB2_Y[27]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y29, m_strAryGradeRankB2_Y[28]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y30, m_strAryGradeRankB2_Y[29]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y31, m_strAryGradeRankB2_Y[30]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_Y32, m_strAryGradeRankB2_Y[31]);

	//GRADE B2 - RANK
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK1,  m_strAryGradeB2_Rank[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK2,  m_strAryGradeB2_Rank[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK3,  m_strAryGradeB2_Rank[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK4,  m_strAryGradeB2_Rank[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK5,  m_strAryGradeB2_Rank[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK6,  m_strAryGradeB2_Rank[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK7,  m_strAryGradeB2_Rank[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK8,  m_strAryGradeB2_Rank[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK9,  m_strAryGradeB2_Rank[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK10, m_strAryGradeB2_Rank[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK11, m_strAryGradeB2_Rank[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK12, m_strAryGradeB2_Rank[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK13, m_strAryGradeB2_Rank[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK14, m_strAryGradeB2_Rank[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK15, m_strAryGradeB2_Rank[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_B2_RANK16, m_strAryGradeB2_Rank[15]);

	//GRADE C - X
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X1,  m_nAryGradeRankChkC_X[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X2,  m_nAryGradeRankChkC_X[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X3,  m_nAryGradeRankChkC_X[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X4,  m_nAryGradeRankChkC_X[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X5,  m_nAryGradeRankChkC_X[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X6,  m_nAryGradeRankChkC_X[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X7,  m_nAryGradeRankChkC_X[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X8,  m_nAryGradeRankChkC_X[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X9,  m_nAryGradeRankChkC_X[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X10, m_nAryGradeRankChkC_X[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X11, m_nAryGradeRankChkC_X[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X12, m_nAryGradeRankChkC_X[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X13, m_nAryGradeRankChkC_X[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X14, m_nAryGradeRankChkC_X[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X15, m_nAryGradeRankChkC_X[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_X16, m_nAryGradeRankChkC_X[15]);
	//GRADE C - Y
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y1,  m_nAryGradeRankChkC_Y[0]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y2,  m_nAryGradeRankChkC_Y[1]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y3,  m_nAryGradeRankChkC_Y[2]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y4,  m_nAryGradeRankChkC_Y[3]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y5,  m_nAryGradeRankChkC_Y[4]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y6,  m_nAryGradeRankChkC_Y[5]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y7,  m_nAryGradeRankChkC_Y[6]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y8,  m_nAryGradeRankChkC_Y[7]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y9,  m_nAryGradeRankChkC_Y[8]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y10, m_nAryGradeRankChkC_Y[9]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y11, m_nAryGradeRankChkC_Y[10]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y12, m_nAryGradeRankChkC_Y[11]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y13, m_nAryGradeRankChkC_Y[12]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y14, m_nAryGradeRankChkC_Y[13]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y15, m_nAryGradeRankChkC_Y[14]);
	DDX_Check(pDX, IDC_CHK_G_RANK_C_Y16, m_nAryGradeRankChkC_Y[15]);
	//GRADE C - X Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X1,  m_strAryGradeRankC_X[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X2,  m_strAryGradeRankC_X[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X3,  m_strAryGradeRankC_X[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X4,  m_strAryGradeRankC_X[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X5,  m_strAryGradeRankC_X[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X6,  m_strAryGradeRankC_X[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X7,  m_strAryGradeRankC_X[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X8,  m_strAryGradeRankC_X[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X9,  m_strAryGradeRankC_X[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X10, m_strAryGradeRankC_X[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X11, m_strAryGradeRankC_X[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X12, m_strAryGradeRankC_X[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X13, m_strAryGradeRankC_X[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X14, m_strAryGradeRankC_X[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X15, m_strAryGradeRankC_X[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_X16, m_strAryGradeRankC_X[15]);
	//GRADE C - Y Value
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y1,  m_strAryGradeRankC_Y[0]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y2,  m_strAryGradeRankC_Y[1]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y3,  m_strAryGradeRankC_Y[2]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y4,  m_strAryGradeRankC_Y[3]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y5,  m_strAryGradeRankC_Y[4]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y6,  m_strAryGradeRankC_Y[5]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y7,  m_strAryGradeRankC_Y[6]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y8,  m_strAryGradeRankC_Y[7]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y9,  m_strAryGradeRankC_Y[8]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y10, m_strAryGradeRankC_Y[9]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y11, m_strAryGradeRankC_Y[10]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y12, m_strAryGradeRankC_Y[11]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y13, m_strAryGradeRankC_Y[12]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y14, m_strAryGradeRankC_Y[13]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y15, m_strAryGradeRankC_Y[14]);
	DDX_Text(pDX, IDC_EDIT_G_RANK_C_Y16, m_strAryGradeRankC_Y[15]);


}


BEGIN_MESSAGE_MAP(CGRADERANKSET, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGRADERANKSET::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CGRADERANKSET::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_GRADERANK_RCP_CHANGE, &CGRADERANKSET::OnBnClickedBtnGraderankRcpChange)
END_MESSAGE_MAP()


// CGRADERANKSET 메시지 처리기입니다.
BOOL CGRADERANKSET::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strTemp,cCountRoot,strRecipeNo,strFilePath;
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
	GetDlgItem(IDC_EDIT_GRADERANK_RCP)->SetWindowText(m_strRepNo);
	strLogFolder = GRADE_SETTING_FOLDER_PATH;	
	if( !fileFind.FindFile(strLogFolder) )	// 폴더가 없을 시,
	{
		CreateDirectory(strLogFolder,NULL);	// 폴더 생성
	}
	strFilePath = GRADE_SETTING_PATH + m_strRepNo + _T(".ini");
	//Grade Rank A 
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank A  - Check X
		strTemp.Format(_T("CHKA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkA_X[i] = _ttoi(strTemp);
		//Grade Rank A  - Check Y
		strTemp.Format(_T("CHKA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkA_Y[i] = _ttoi(strTemp);

		//Grade Rank A  - Size X
		strTemp.Format(_T("SIZEA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankA_X[i] = strTemp;
		//Grade Rank A  - Size Y
		strTemp.Format(_T("SIZEA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankA_Y[i] = strTemp;
	}
	//Grade Rank C
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank C  - Check X
		strTemp.Format(_T("CHKC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkC_X[i] = _ttoi(strTemp);
		//Grade Rank C  - Check Y
		strTemp.Format(_T("CHKC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkC_Y[i] = _ttoi(strTemp);

		//Grade Rank C  - Size X
		strTemp.Format(_T("SIZEC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankC_X[i] = strTemp;
		//Grade Rank C  - Size Y
		strTemp.Format(_T("SIZEC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankC_Y[i] = strTemp;
	}
	//Grade Rank B,B2
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank B  - Check X
		strTemp.Format(_T("CHKB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB_X[i] = _ttoi(strTemp);
		//Grade Rank B  - Check Y
		strTemp.Format(_T("CHKB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB_Y[i] = _ttoi(strTemp);
		//Grade Rank B  - RANK
		strTemp.Format(_T("RANKB%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB_Rank[i] = strTemp;

		//Grade Rank B2  - Check X
		strTemp.Format(_T("CHKB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB2_X [i] = _ttoi(strTemp);
		//Grade Rank B2  - Check Y
		strTemp.Format(_T("CHKB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB2_Y[i] = _ttoi(strTemp);
		//Grade Rank B2  - RANK
		strTemp.Format(_T("RANKB2%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB2_Rank[i] = strTemp;

	}
	//Grade Rank B,B2 Size
	for (int i = 0; i < 32; i++)
	{
		//Grade Rank B  - Size X
		strTemp.Format(_T("SIZEB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB_X[i] = strTemp;
		//Grade Rank B  - Size Y
		strTemp.Format(_T("SIZEB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB_Y[i] = strTemp;

		//Grade Rank B2  - Size X
		strTemp.Format(_T("SIZEB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB2_X[i] = strTemp;
		//Grade Rank B2  - Size Y
		strTemp.Format(_T("SIZEB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB2_Y[i] = strTemp;
	}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGRADERANKSET::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 수치 저장.	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strTemp,strTemp1,cCountRoot,strRecipeNo,strFilePath;
	int nRead = 0;
	TCHAR strBuffer[255];
	UpdateData();
	//Recipe 정보 유무 Check
	GetDlgItem(IDC_EDIT_GRADERANK_RCP)->GetWindowText(strRecipeNo);
	if(strRecipeNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	m_strRepNo = strRecipeNo;
	strFilePath = GRADE_SETTING_PATH + m_strRepNo + _T(".ini");
	
	//Grade Rank A 
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank A  - Check X
		strTemp.Format(_T("CHKA_X%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkA_X[i]);
		WritePrivateProfileString(_T("GRADERANK_A"), strTemp, strTemp1, strFilePath);
		//Grade Rank A  - Check Y
		strTemp.Format(_T("CHKA_Y%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkA_Y[i]);
		WritePrivateProfileString(_T("GRADERANK_A"), strTemp, strTemp1, strFilePath);

		//Grade Rank A  - Size X
		strTemp.Format(_T("SIZEA_X%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_A"), strTemp, m_strAryGradeRankA_X[i], strFilePath);

		//Grade Rank A  - Size Y
		strTemp.Format(_T("SIZEA_Y%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_A"), strTemp, m_strAryGradeRankA_Y[i], strFilePath);
	}
	//Grade Rank C
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank C  - Check X
		strTemp.Format(_T("CHKC_X%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkC_X[i]);
		WritePrivateProfileString(_T("GRADERANK_C"), strTemp, strTemp1, strFilePath);
		//Grade Rank C  - Check Y
		strTemp.Format(_T("CHKC_Y%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkC_Y[i]);
		WritePrivateProfileString(_T("GRADERANK_C"), strTemp, strTemp1, strFilePath);

		//Grade Rank C  - Size X
		strTemp.Format(_T("SIZEC_X%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_C"), strTemp, m_strAryGradeRankC_X[i], strFilePath);

		//Grade Rank C  - Size Y
		strTemp.Format(_T("SIZEC_Y%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_C"), strTemp, m_strAryGradeRankC_Y[i], strFilePath);

	}
	//Grade Rank B,B2
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank B  - Check X
		strTemp.Format(_T("CHKB_X%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkB_X[i]);
		WritePrivateProfileString(_T("GRADERANK_B"), strTemp, strTemp1, strFilePath);
		//Grade Rank B  - Check Y
		strTemp.Format(_T("CHKB_Y%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkB_Y[i]);
		WritePrivateProfileString(_T("GRADERANK_B"), strTemp, strTemp1, strFilePath);
		//Grade Rank B  - RANK
		strTemp.Format(_T("RANKB%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B"), strTemp, m_strAryGradeB_Rank[i], strFilePath);

		//Grade Rank B2  - Check X
		strTemp.Format(_T("CHKB2_X%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkB2_X[i]);
		WritePrivateProfileString(_T("GRADERANK_B2"), strTemp, strTemp1, strFilePath);
		//Grade Rank B2  - Check Y
		strTemp.Format(_T("CHKB2_Y%d"), i+1);
		strTemp1.Format(_T("%d"), m_nAryGradeRankChkB2_Y[i]);
		WritePrivateProfileString(_T("GRADERANK_B2"), strTemp, strTemp1, strFilePath);
		//Grade Rank B2  - RANK
		strTemp.Format(_T("RANKB2%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B2"), strTemp, m_strAryGradeB2_Rank[i], strFilePath);

	}
	//Grade Rank B,B2 Size
	for (int i = 0; i < 32; i++)
	{
		//Grade Rank B  - Size X
		strTemp.Format(_T("SIZEB_X%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B"), strTemp, m_strAryGradeRankB_X[i], strFilePath);
		//Grade Rank B  - Size Y
		strTemp.Format(_T("SIZEB_Y%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B"), strTemp, m_strAryGradeRankB_Y[i], strFilePath);

		//Grade Rank B2  - Size X
		strTemp.Format(_T("SIZEB2_X%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B2"), strTemp, m_strAryGradeRankB2_X[i], strFilePath);
		//Grade Rank B2  - Size Y
		strTemp.Format(_T("SIZEB2_Y%d"), i+1);
		WritePrivateProfileString(_T("GRADERANK_B2"), strTemp, m_strAryGradeRankB2_Y[i], strFilePath);
	}	

	CDialogEx::OnOK();
}

void CGRADERANKSET::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CGRADERANKSET::OnBnClickedBtnGraderankRcpChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strRecipeNo,strFilePath,strTemp;
	TCHAR strBuffer[255];
	UpdateData();
	//Recipe 정보 유무 Check
	GetDlgItem(IDC_EDIT_GRADERANK_RCP)->GetWindowText(strRecipeNo);
	if(strRecipeNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	m_strRepNo = strRecipeNo;
	
	strFilePath = GRADE_SETTING_PATH + m_strRepNo + _T(".ini");
	//Grade Rank A 
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank A  - Check X
		strTemp.Format(_T("CHKA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkA_X[i] = _ttoi(strTemp);
		//Grade Rank A  - Check Y
		strTemp.Format(_T("CHKA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkA_Y[i] = _ttoi(strTemp);

		//Grade Rank A  - Size X
		strTemp.Format(_T("SIZEA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankA_X[i] = strTemp;
		//Grade Rank A  - Size Y
		strTemp.Format(_T("SIZEA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankA_Y[i] = strTemp;
	}
	//Grade Rank C
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank C  - Check X
		strTemp.Format(_T("CHKC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkC_X[i] = _ttoi(strTemp);
		//Grade Rank C  - Check Y
		strTemp.Format(_T("CHKC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkC_Y[i] = _ttoi(strTemp);

		//Grade Rank C  - Size X
		strTemp.Format(_T("SIZEC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankC_X[i] = strTemp;
		//Grade Rank C  - Size Y
		strTemp.Format(_T("SIZEC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankC_Y[i] = strTemp;
	}
	//Grade Rank B,B2
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank B  - Check X
		strTemp.Format(_T("CHKB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB_X[i] = _ttoi(strTemp);
		//Grade Rank B  - Check Y
		strTemp.Format(_T("CHKB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB_Y[i] = _ttoi(strTemp);
		//Grade Rank B  - RANK
		strTemp.Format(_T("RANKB%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB_Rank[i] = strTemp;

		//Grade Rank B2  - Check X
		strTemp.Format(_T("CHKB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB2_X [i] = _ttoi(strTemp);
		//Grade Rank B2  - Check Y
		strTemp.Format(_T("CHKB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB2_Y[i] = _ttoi(strTemp);
		//Grade Rank B2  - RANK
		strTemp.Format(_T("RANKB2%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB2_Rank[i] = strTemp;

	}
	//Grade Rank B,B2 Size
	for (int i = 0; i < 32; i++)
	{
		//Grade Rank B  - Size X
		strTemp.Format(_T("SIZEB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB_X[i] = strTemp;
		//Grade Rank B  - Size Y
		strTemp.Format(_T("SIZEB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB_Y[i] = strTemp;

		//Grade Rank B2  - Size X
		strTemp.Format(_T("SIZEB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB2_X[i] = strTemp;
		//Grade Rank B2  - Size Y
		strTemp.Format(_T("SIZEB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB2_Y[i] = strTemp;
	}	
	UpdateData(FALSE);
}
