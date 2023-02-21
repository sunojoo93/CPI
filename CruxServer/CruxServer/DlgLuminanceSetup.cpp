
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgLuminanceSetup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgLuminanceSetup.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewAuto.h"

// CDlgLuminanceSetup 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLuminanceSetup, CDialogEx)

CDlgLuminanceSetup::CDlgLuminanceSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLuminanceSetup::IDD, pParent)
{
	//2016.03.02
	m_bUseLuminanceAlarm = FALSE;
	m_nLuminanceAlarmSetCount = 0;
	m_nLuminanceAlarmSetPanelCount = 0;
	m_nLuminanceRange = 0;
	m_bUseTdiMin  = 0;
	for(int i = 0 ; i < DCC_COUNT ; i++)
	{
		m_nAryDccChkSizeX[i] = FALSE;
		m_nAryDccChkSizeY[i] = FALSE;
	}
	for(int i = 0 ; i < DCC_SIZE_CHECK ; i++)
	{
		m_strAryDccSizeX[i] = _T("");
		m_strAryDccSizeY[i] = _T("");
	}
	//2020.07.06 add by mby
	for(int i = 0 ; i < DCC_CHK_COUNT ; i++)
	{
		m_nAryDccCheck[i] = FALSE;
		m_nAryPaCntPD[i] = FALSE;
		m_nAryPaCntPDChk[i] = FALSE;
	}
	//2020.08.26 add by mby
	for(int i = 0 ; i < 16 ; i++)
	{
		m_nAryPaCntEdge[i] = FALSE;
		m_nAryPaCntEdgeChk[i] = FALSE;
	}
}

CDlgLuminanceSetup::~CDlgLuminanceSetup()
{
}

void CDlgLuminanceSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//2016.03.02
	DDX_Check(pDX, IDC_CHECK_LUMINANCE_ALARM_USE, m_bUseLuminanceAlarm);	//2015.06.02
	DDX_Text(pDX, IDC_EDIT_LUMINANCE_SET_ALARM_COUNT, m_nLuminanceAlarmSetCount);
	DDX_Text(pDX, IDC_EDIT_LUMINANCE_SET_PANEL_COUNT, m_nLuminanceAlarmSetPanelCount);
	DDX_Text(pDX, IDC_EDIT_LUMINANCE_SET_RANGE, m_nLuminanceRange);
	DDX_Text(pDX, IDC_EDIT_CAM1_MIN, m_nAryLuminanceLimit[CF_FORWARD_SIDE]);
	DDX_Text(pDX, IDC_EDIT_CAM1_MAX, m_nAryLuminanceLimit[CF_FORWARD_COAXIAL]);
	DDX_Text(pDX, IDC_EDIT_CAM2_MIN, m_nAryLuminanceLimit[CF_BACKWARD_SIDE]);
	DDX_Text(pDX, IDC_EDIT_CAM2_MAX, m_nAryLuminanceLimit[CF_BACKWARD_COAXIAL]);
	DDX_Text(pDX, IDC_EDIT_CAM3_MIN, m_nAryLuminanceLimit[TFT_FORWARD_SIDE]);
	DDX_Text(pDX, IDC_EDIT_CAM3_MAX, m_nAryLuminanceLimit[TFT_FORWARD_COAXIAL]);
	DDX_Text(pDX, IDC_EDIT_CAM4_MIN, m_nAryLuminanceLimit[TFT_BACKWARD_SIDE]);
	DDX_Text(pDX, IDC_EDIT_CAM4_MAX, m_nAryLuminanceLimit[TFT_BACKWARD_COAXIAL]);
	DDX_Text(pDX, IDC_EDIT_POL1_MAX, m_nAryLuminanceLimit[POL1]);

	//2020.04.10 add by mby
	DDX_Check(pDX, IDC_CHK_SIDE1, m_nAryDccDirect[INSP_DUST]);
	DDX_Check(pDX, IDC_CHK_SIDE2, m_nAryDccDirect[INSP_WHITE_TURBIDITY]);
	//DDX_Check(pDX, IDC_CHK_SIDE3, m_nAryDccDirect[INSP_ITO]);
	//DDX_Check(pDX, IDC_CHK_SIDE4, m_nAryDccDirect[INSP_EDGE_BROKEN]);
	//DDX_Check(pDX, IDC_CHK_SIDE5, m_nAryDccDirect[INSP_PAD_SCRATCH]);
	//DDX_Check(pDX, IDC_CHK_SIDE6, m_nAryDccDirect[INSP_DIMPLE_POINT]);	

	DDX_Check(pDX, IDC_CHECK_SIZEX1, m_nAryDccChkSizeX[INSP_DUST]);
	DDX_Check(pDX, IDC_CHECK_SIZEX2, m_nAryDccChkSizeX[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CHECK_SIZEX3, m_nAryDccChkSizeX[INSP_ITO]);
	DDX_Check(pDX, IDC_CHECK_SIZEX4, m_nAryDccChkSizeX[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_SIZEX5, m_nAryDccChkSizeX[INSP_PAD_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEX6, m_nAryDccChkSizeX[INSP_DIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_SIZEX7, m_nAryDccChkSizeX[INSP_ETCHING_LINE]);
	DDX_Check(pDX, IDC_CHECK_SIZEX8, m_nAryDccChkSizeX[INSP_DIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_SIZEX9, m_nAryDccChkSizeX[INSP_PAD_BURNT]);
	DDX_Check(pDX, IDC_CHECK_SIZEX10, m_nAryDccChkSizeX[INSP_BURR]);
	DDX_Check(pDX, IDC_CHECK_SIZEX11, m_nAryDccChkSizeX[INSP_POL_BUBBLE]);
	DDX_Check(pDX, IDC_CHECK_SIZEX12, m_nAryDccChkSizeX[INSP_POL_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEX13, m_nAryDccChkSizeX[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Check(pDX, IDC_CHECK_SIZEX14, m_nAryDccChkSizeX[INSP_POL_MINOR]);
	DDX_Check(pDX, IDC_CHECK_SIZEX15, m_nAryDccChkSizeX[INSP_BLACK_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEX16, m_nAryDccChkSizeX[INSP_WHITE_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEX17, m_nAryDccChkSizeX[16]);
	DDX_Check(pDX, IDC_CHECK_SIZEX18, m_nAryDccChkSizeX[17]);
	DDX_Check(pDX, IDC_CHECK_SIZEX19, m_nAryDccChkSizeX[18]);
	DDX_Check(pDX, IDC_CHECK_SIZEX20, m_nAryDccChkSizeX[19]);
	DDX_Check(pDX, IDC_CHECK_SIZEX21, m_nAryDccChkSizeX[20]);
	DDX_Check(pDX, IDC_CHECK_SIZEX22, m_nAryDccChkSizeX[21]);
	DDX_Check(pDX, IDC_CHECK_SIZEX23, m_nAryDccChkSizeX[22]);
	DDX_Check(pDX, IDC_CHECK_SIZEX24, m_nAryDccChkSizeX[23]);
	DDX_Check(pDX, IDC_CHECK_SIZEX25, m_nAryDccChkSizeX[24]);
	DDX_Check(pDX, IDC_CHECK_SIZEX26, m_nAryDccChkSizeX[25]);
	DDX_Check(pDX, IDC_CHECK_SIZEX27, m_nAryDccChkSizeX[26]);
	DDX_Check(pDX, IDC_CHECK_SIZEX28, m_nAryDccChkSizeX[27]);
	DDX_Check(pDX, IDC_CHECK_SIZEX29, m_nAryDccChkSizeX[28]);
	DDX_Check(pDX, IDC_CHECK_SIZEX30, m_nAryDccChkSizeX[29]);
	DDX_Check(pDX, IDC_CHECK_SIZEX31, m_nAryDccChkSizeX[30]);
	DDX_Check(pDX, IDC_CHECK_SIZEX32, m_nAryDccChkSizeX[31]);
	DDX_Check(pDX, IDC_CHECK_SIZEX33, m_nAryDccChkSizeX[32]);
	DDX_Check(pDX, IDC_CHECK_SIZEX34, m_nAryDccChkSizeX[33]);

	DDX_Check(pDX, IDC_CHECK_SIZEY1, m_nAryDccChkSizeY[INSP_DUST]);
	DDX_Check(pDX, IDC_CHECK_SIZEY2, m_nAryDccChkSizeY[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CHECK_SIZEY3, m_nAryDccChkSizeY[INSP_ITO]);
	DDX_Check(pDX, IDC_CHECK_SIZEY4, m_nAryDccChkSizeY[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_SIZEY5, m_nAryDccChkSizeY[INSP_PAD_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEY6, m_nAryDccChkSizeY[INSP_DIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_SIZEY7, m_nAryDccChkSizeY[INSP_ETCHING_LINE]);
	DDX_Check(pDX, IDC_CHECK_SIZEY8, m_nAryDccChkSizeY[INSP_DIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_SIZEY9, m_nAryDccChkSizeY[INSP_PAD_BURNT]);
	DDX_Check(pDX, IDC_CHECK_SIZEY10, m_nAryDccChkSizeY[INSP_BURR]);
	DDX_Check(pDX, IDC_CHECK_SIZEY11, m_nAryDccChkSizeY[INSP_POL_BUBBLE]);
	DDX_Check(pDX, IDC_CHECK_SIZEY12, m_nAryDccChkSizeY[INSP_POL_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEY13, m_nAryDccChkSizeY[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Check(pDX, IDC_CHECK_SIZEY14, m_nAryDccChkSizeY[INSP_POL_MINOR]);
	DDX_Check(pDX, IDC_CHECK_SIZEY15, m_nAryDccChkSizeY[INSP_BLACK_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEY16, m_nAryDccChkSizeY[INSP_WHITE_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_SIZEY17, m_nAryDccChkSizeY[16]);
	DDX_Check(pDX, IDC_CHECK_SIZEY18, m_nAryDccChkSizeY[17]);
	DDX_Check(pDX, IDC_CHECK_SIZEY19, m_nAryDccChkSizeY[18]);
	DDX_Check(pDX, IDC_CHECK_SIZEY20, m_nAryDccChkSizeY[19]);
	DDX_Check(pDX, IDC_CHECK_SIZEY21, m_nAryDccChkSizeY[20]);
	DDX_Check(pDX, IDC_CHECK_SIZEY22, m_nAryDccChkSizeY[21]);
	DDX_Check(pDX, IDC_CHECK_SIZEY23, m_nAryDccChkSizeY[22]);
	DDX_Check(pDX, IDC_CHECK_SIZEY24, m_nAryDccChkSizeY[23]);
	DDX_Check(pDX, IDC_CHECK_SIZEY25, m_nAryDccChkSizeY[24]);
	DDX_Check(pDX, IDC_CHECK_SIZEY26, m_nAryDccChkSizeY[25]);
	DDX_Check(pDX, IDC_CHECK_SIZEY27, m_nAryDccChkSizeY[26]);
	DDX_Check(pDX, IDC_CHECK_SIZEY28, m_nAryDccChkSizeY[27]);
	DDX_Check(pDX, IDC_CHECK_SIZEY29, m_nAryDccChkSizeY[28]);
	DDX_Check(pDX, IDC_CHECK_SIZEY30, m_nAryDccChkSizeY[29]);
	DDX_Check(pDX, IDC_CHECK_SIZEY31, m_nAryDccChkSizeY[30]);
	DDX_Check(pDX, IDC_CHECK_SIZEY32, m_nAryDccChkSizeY[31]);
	DDX_Check(pDX, IDC_CHECK_SIZEY33, m_nAryDccChkSizeY[32]);
	DDX_Check(pDX, IDC_CHECK_SIZEY34, m_nAryDccChkSizeY[33]);
	//2020.04.20 add by mby

	DDX_Check(pDX, IDC_CK_DCC1, m_nAryDccCheck[INSP_DUST]);
	DDX_Check(pDX, IDC_CK_DCC2, m_nAryDccCheck[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CK_DCC3, m_nAryDccCheck[INSP_ITO]);
	DDX_Check(pDX, IDC_CK_DCC4, m_nAryDccCheck[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CK_DCC5, m_nAryDccCheck[INSP_PAD_SCRATCH]);


	DDX_Text(pDX, IDC_EDIT_DCC_VALUE1, m_nAryDccInfoOne[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_DCC_VALUE2, m_nAryDccInfoOne[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_DCC_VALUE3, m_nAryDccInfoOne[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_DCC_VALUE4, m_nAryDccInfoOne[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_DCC_VALUE5, m_nAryDccInfoOne[INSP_PAD_SCRATCH]);


	DDX_Text(pDX, IDC_EDIT_DCC_TVALUE1, m_nAryDccInfoTwo[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_DCC_TVALUE2, m_nAryDccInfoTwo[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_DCC_TVALUE3, m_nAryDccInfoTwo[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_DCC_TVALUE4, m_nAryDccInfoTwo[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_DCC_TVALUE5, m_nAryDccInfoTwo[INSP_PAD_SCRATCH]);

	//01.02 add by mby
	DDX_Text(pDX, IDC_EDIT_SIZEX1, m_strAryDccSizeX[0]);
	DDX_Text(pDX, IDC_EDIT_SIZEX2, m_strAryDccSizeX[1]);
	DDX_Text(pDX, IDC_EDIT_SIZEX3, m_strAryDccSizeX[2]);
	DDX_Text(pDX, IDC_EDIT_SIZEX4, m_strAryDccSizeX[3]);
	DDX_Text(pDX, IDC_EDIT_SIZEX5, m_strAryDccSizeX[4]);
	DDX_Text(pDX, IDC_EDIT_SIZEX6, m_strAryDccSizeX[5]);
	DDX_Text(pDX, IDC_EDIT_SIZEX7, m_strAryDccSizeX[6]);
	DDX_Text(pDX, IDC_EDIT_SIZEX8, m_strAryDccSizeX[7]);
	DDX_Text(pDX, IDC_EDIT_SIZEX9, m_strAryDccSizeX[8]);
	DDX_Text(pDX, IDC_EDIT_SIZEX10, m_strAryDccSizeX[9]);
	DDX_Text(pDX, IDC_EDIT_SIZEX11, m_strAryDccSizeX[10]);
	DDX_Text(pDX, IDC_EDIT_SIZEX12, m_strAryDccSizeX[11]);
	DDX_Text(pDX, IDC_EDIT_SIZEX13, m_strAryDccSizeX[12]);
	DDX_Text(pDX, IDC_EDIT_SIZEX14, m_strAryDccSizeX[13]);
	DDX_Text(pDX, IDC_EDIT_SIZEX15, m_strAryDccSizeX[14]);
	DDX_Text(pDX, IDC_EDIT_SIZEX16, m_strAryDccSizeX[15]);
	DDX_Text(pDX, IDC_EDIT_SIZEX17, m_strAryDccSizeX[16]);
	DDX_Text(pDX, IDC_EDIT_SIZEX18, m_strAryDccSizeX[17]);
	DDX_Text(pDX, IDC_EDIT_SIZEX19, m_strAryDccSizeX[18]);
	DDX_Text(pDX, IDC_EDIT_SIZEX20, m_strAryDccSizeX[19]);
	DDX_Text(pDX, IDC_EDIT_SIZEX21, m_strAryDccSizeX[20]);
	DDX_Text(pDX, IDC_EDIT_SIZEX22, m_strAryDccSizeX[21]);
	DDX_Text(pDX, IDC_EDIT_SIZEX23, m_strAryDccSizeX[22]);
	DDX_Text(pDX, IDC_EDIT_SIZEX24, m_strAryDccSizeX[23]);
	DDX_Text(pDX, IDC_EDIT_SIZEX25, m_strAryDccSizeX[24]);
	DDX_Text(pDX, IDC_EDIT_SIZEX26, m_strAryDccSizeX[25]);
	DDX_Text(pDX, IDC_EDIT_SIZEX27, m_strAryDccSizeX[26]);
	DDX_Text(pDX, IDC_EDIT_SIZEX28, m_strAryDccSizeX[27]);
	DDX_Text(pDX, IDC_EDIT_SIZEX29, m_strAryDccSizeX[28]);
	DDX_Text(pDX, IDC_EDIT_SIZEX30, m_strAryDccSizeX[29]);
	DDX_Text(pDX, IDC_EDIT_SIZEX31, m_strAryDccSizeX[30]);
	DDX_Text(pDX, IDC_EDIT_SIZEX32, m_strAryDccSizeX[31]);
	DDX_Text(pDX, IDC_EDIT_SIZEX33, m_strAryDccSizeX[32]);
	DDX_Text(pDX, IDC_EDIT_SIZEX34, m_strAryDccSizeX[33]);
	DDX_Text(pDX, IDC_EDIT_SIZEX35, m_strAryDccSizeX[34]);
	DDX_Text(pDX, IDC_EDIT_SIZEX36, m_strAryDccSizeX[35]);
	DDX_Text(pDX, IDC_EDIT_SIZEX37, m_strAryDccSizeX[36]);
	DDX_Text(pDX, IDC_EDIT_SIZEX38, m_strAryDccSizeX[37]);
	DDX_Text(pDX, IDC_EDIT_SIZEX39, m_strAryDccSizeX[38]);
	DDX_Text(pDX, IDC_EDIT_SIZEX40, m_strAryDccSizeX[39]);
	DDX_Text(pDX, IDC_EDIT_SIZEX41, m_strAryDccSizeX[40]);
	DDX_Text(pDX, IDC_EDIT_SIZEX42, m_strAryDccSizeX[41]);
	DDX_Text(pDX, IDC_EDIT_SIZEX43, m_strAryDccSizeX[42]);
	DDX_Text(pDX, IDC_EDIT_SIZEX44, m_strAryDccSizeX[43]);
	DDX_Text(pDX, IDC_EDIT_SIZEX45, m_strAryDccSizeX[44]);
	DDX_Text(pDX, IDC_EDIT_SIZEX46, m_strAryDccSizeX[45]);
	DDX_Text(pDX, IDC_EDIT_SIZEX47, m_strAryDccSizeX[46]);
	DDX_Text(pDX, IDC_EDIT_SIZEX48, m_strAryDccSizeX[47]);
	DDX_Text(pDX, IDC_EDIT_SIZEX49, m_strAryDccSizeX[48]);
	DDX_Text(pDX, IDC_EDIT_SIZEX50, m_strAryDccSizeX[49]);
	DDX_Text(pDX, IDC_EDIT_SIZEX51, m_strAryDccSizeX[50]);
	DDX_Text(pDX, IDC_EDIT_SIZEX52, m_strAryDccSizeX[51]);
	DDX_Text(pDX, IDC_EDIT_SIZEX53, m_strAryDccSizeX[52]);
	DDX_Text(pDX, IDC_EDIT_SIZEX54, m_strAryDccSizeX[53]);
	DDX_Text(pDX, IDC_EDIT_SIZEX55, m_strAryDccSizeX[54]);
	DDX_Text(pDX, IDC_EDIT_SIZEX56, m_strAryDccSizeX[55]);
	DDX_Text(pDX, IDC_EDIT_SIZEX57, m_strAryDccSizeX[56]);
	DDX_Text(pDX, IDC_EDIT_SIZEX58, m_strAryDccSizeX[57]);
	DDX_Text(pDX, IDC_EDIT_SIZEX59, m_strAryDccSizeX[58]);
	DDX_Text(pDX, IDC_EDIT_SIZEX60, m_strAryDccSizeX[59]);
	DDX_Text(pDX, IDC_EDIT_SIZEX61, m_strAryDccSizeX[60]);
	DDX_Text(pDX, IDC_EDIT_SIZEX62, m_strAryDccSizeX[61]);
	DDX_Text(pDX, IDC_EDIT_SIZEX63, m_strAryDccSizeX[62]);
	DDX_Text(pDX, IDC_EDIT_SIZEX64, m_strAryDccSizeX[63]);
	DDX_Text(pDX, IDC_EDIT_SIZEX65, m_strAryDccSizeX[64]);
	DDX_Text(pDX, IDC_EDIT_SIZEX66, m_strAryDccSizeX[65]);
	DDX_Text(pDX, IDC_EDIT_SIZEX67, m_strAryDccSizeX[66]);
	DDX_Text(pDX, IDC_EDIT_SIZEX68, m_strAryDccSizeX[67]);

	DDX_Text(pDX, IDC_EDIT_SIZEY1, m_strAryDccSizeY[0]);
	DDX_Text(pDX, IDC_EDIT_SIZEY2, m_strAryDccSizeY[1]);
	DDX_Text(pDX, IDC_EDIT_SIZEY3, m_strAryDccSizeY[2]);
	DDX_Text(pDX, IDC_EDIT_SIZEY4, m_strAryDccSizeY[3]);
	DDX_Text(pDX, IDC_EDIT_SIZEY5, m_strAryDccSizeY[4]);
	DDX_Text(pDX, IDC_EDIT_SIZEY6, m_strAryDccSizeY[5]);
	DDX_Text(pDX, IDC_EDIT_SIZEY7, m_strAryDccSizeY[6]);
	DDX_Text(pDX, IDC_EDIT_SIZEY8, m_strAryDccSizeY[7]);
	DDX_Text(pDX, IDC_EDIT_SIZEY9, m_strAryDccSizeY[8]);
	DDX_Text(pDX, IDC_EDIT_SIZEY10, m_strAryDccSizeY[9]);
	DDX_Text(pDX, IDC_EDIT_SIZEY11, m_strAryDccSizeY[10]);
	DDX_Text(pDX, IDC_EDIT_SIZEY12, m_strAryDccSizeY[11]);
	DDX_Text(pDX, IDC_EDIT_SIZEY13, m_strAryDccSizeY[12]);
	DDX_Text(pDX, IDC_EDIT_SIZEY14, m_strAryDccSizeY[13]);
	DDX_Text(pDX, IDC_EDIT_SIZEY15, m_strAryDccSizeY[14]);
	DDX_Text(pDX, IDC_EDIT_SIZEY16, m_strAryDccSizeY[15]);
	DDX_Text(pDX, IDC_EDIT_SIZEY17, m_strAryDccSizeY[16]);
	DDX_Text(pDX, IDC_EDIT_SIZEY18, m_strAryDccSizeY[17]);
	DDX_Text(pDX, IDC_EDIT_SIZEY19, m_strAryDccSizeY[18]);
	DDX_Text(pDX, IDC_EDIT_SIZEY20, m_strAryDccSizeY[19]);
	DDX_Text(pDX, IDC_EDIT_SIZEY21, m_strAryDccSizeY[20]);
	DDX_Text(pDX, IDC_EDIT_SIZEY22, m_strAryDccSizeY[21]);
	DDX_Text(pDX, IDC_EDIT_SIZEY23, m_strAryDccSizeY[22]);
	DDX_Text(pDX, IDC_EDIT_SIZEY24, m_strAryDccSizeY[23]);
	DDX_Text(pDX, IDC_EDIT_SIZEY25, m_strAryDccSizeY[24]);
	DDX_Text(pDX, IDC_EDIT_SIZEY26, m_strAryDccSizeY[25]);
	DDX_Text(pDX, IDC_EDIT_SIZEY27, m_strAryDccSizeY[26]);
	DDX_Text(pDX, IDC_EDIT_SIZEY28, m_strAryDccSizeY[27]);
	DDX_Text(pDX, IDC_EDIT_SIZEY29, m_strAryDccSizeY[28]);
	DDX_Text(pDX, IDC_EDIT_SIZEY30, m_strAryDccSizeY[29]);
	DDX_Text(pDX, IDC_EDIT_SIZEY31, m_strAryDccSizeY[30]);
	DDX_Text(pDX, IDC_EDIT_SIZEY32, m_strAryDccSizeY[31]);
	DDX_Text(pDX, IDC_EDIT_SIZEY33, m_strAryDccSizeY[32]);
	DDX_Text(pDX, IDC_EDIT_SIZEY34, m_strAryDccSizeY[33]);
	DDX_Text(pDX, IDC_EDIT_SIZEY35, m_strAryDccSizeY[34]);
	DDX_Text(pDX, IDC_EDIT_SIZEY36, m_strAryDccSizeY[35]);
	DDX_Text(pDX, IDC_EDIT_SIZEY37, m_strAryDccSizeY[36]);
	DDX_Text(pDX, IDC_EDIT_SIZEY38, m_strAryDccSizeY[37]);
	DDX_Text(pDX, IDC_EDIT_SIZEY39, m_strAryDccSizeY[38]);
	DDX_Text(pDX, IDC_EDIT_SIZEY40, m_strAryDccSizeY[39]);
	DDX_Text(pDX, IDC_EDIT_SIZEY41, m_strAryDccSizeY[40]);
	DDX_Text(pDX, IDC_EDIT_SIZEY42, m_strAryDccSizeY[41]);
	DDX_Text(pDX, IDC_EDIT_SIZEY43, m_strAryDccSizeY[42]);
	DDX_Text(pDX, IDC_EDIT_SIZEY44, m_strAryDccSizeY[43]);
	DDX_Text(pDX, IDC_EDIT_SIZEY45, m_strAryDccSizeY[44]);
	DDX_Text(pDX, IDC_EDIT_SIZEY46, m_strAryDccSizeY[45]);
	DDX_Text(pDX, IDC_EDIT_SIZEY47, m_strAryDccSizeY[46]);
	DDX_Text(pDX, IDC_EDIT_SIZEY48, m_strAryDccSizeY[47]);
	DDX_Text(pDX, IDC_EDIT_SIZEY49, m_strAryDccSizeY[48]);
	DDX_Text(pDX, IDC_EDIT_SIZEY50, m_strAryDccSizeY[49]);
	DDX_Text(pDX, IDC_EDIT_SIZEY51, m_strAryDccSizeY[50]);
	DDX_Text(pDX, IDC_EDIT_SIZEY52, m_strAryDccSizeY[51]);
	DDX_Text(pDX, IDC_EDIT_SIZEY53, m_strAryDccSizeY[52]);
	DDX_Text(pDX, IDC_EDIT_SIZEY54, m_strAryDccSizeY[53]);
	DDX_Text(pDX, IDC_EDIT_SIZEY55, m_strAryDccSizeY[54]);
	DDX_Text(pDX, IDC_EDIT_SIZEY56, m_strAryDccSizeY[55]);
	DDX_Text(pDX, IDC_EDIT_SIZEY57, m_strAryDccSizeY[56]);
	DDX_Text(pDX, IDC_EDIT_SIZEY58, m_strAryDccSizeY[57]);
	DDX_Text(pDX, IDC_EDIT_SIZEY59, m_strAryDccSizeY[58]);
	DDX_Text(pDX, IDC_EDIT_SIZEY60, m_strAryDccSizeY[59]);
	DDX_Text(pDX, IDC_EDIT_SIZEY61, m_strAryDccSizeY[60]);
	DDX_Text(pDX, IDC_EDIT_SIZEY62, m_strAryDccSizeY[61]);
	DDX_Text(pDX, IDC_EDIT_SIZEY63, m_strAryDccSizeY[62]);
	DDX_Text(pDX, IDC_EDIT_SIZEY64, m_strAryDccSizeY[63]);
	DDX_Text(pDX, IDC_EDIT_SIZEY65, m_strAryDccSizeY[64]);
	DDX_Text(pDX, IDC_EDIT_SIZEY66, m_strAryDccSizeY[65]);
	DDX_Text(pDX, IDC_EDIT_SIZEY67, m_strAryDccSizeY[66]);
	DDX_Text(pDX, IDC_EDIT_SIZEY68, m_strAryDccSizeY[67]);

	DDX_Text(pDX, IDC_EDIT_DCC_RCP_CHANGE, m_strRepNo);

	//2020.07.06 add by mby
	DDX_Text(pDX, IDC_EDIT_PACNT_PD1, m_nAryPaCntPD[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_PACNT_PD2, m_nAryPaCntPD[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_PACNT_PD3, m_nAryPaCntPD[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_PACNT_PD4, m_nAryPaCntPD[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_PACNT_PD5, m_nAryPaCntPD[INSP_PAD_SCRATCH]);

	DDX_Check(pDX, IDC_CHK_PACOUNT_PD1, m_nAryPaCntPDChk[INSP_DUST]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_PD2, m_nAryPaCntPDChk[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_PD3, m_nAryPaCntPDChk[INSP_ITO]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_PD4, m_nAryPaCntPDChk[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_PD5, m_nAryPaCntPDChk[INSP_PAD_SCRATCH]);

	//2020.08.26 add by mby S-
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE1,	m_nAryPaCntEdgeChk[0]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE2,	m_nAryPaCntEdgeChk[1]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE3,	m_nAryPaCntEdgeChk[2]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE4,	m_nAryPaCntEdgeChk[3]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE5,	m_nAryPaCntEdgeChk[4]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE6,	m_nAryPaCntEdgeChk[5]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE7,	m_nAryPaCntEdgeChk[6]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE8,	m_nAryPaCntEdgeChk[7]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE9,	m_nAryPaCntEdgeChk[8]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE10,	m_nAryPaCntEdgeChk[9]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE11,	m_nAryPaCntEdgeChk[10]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE12,	m_nAryPaCntEdgeChk[11]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE13,	m_nAryPaCntEdgeChk[12]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE14,	m_nAryPaCntEdgeChk[13]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE15,	m_nAryPaCntEdgeChk[14]);
	DDX_Check(pDX, IDC_CHK_PACOUNT_EDGE16,	m_nAryPaCntEdgeChk[15]);

	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE1,		m_nAryPaCntEdge[0]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE2,		m_nAryPaCntEdge[1]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE3,		m_nAryPaCntEdge[2]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE4,		m_nAryPaCntEdge[3]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE5,		m_nAryPaCntEdge[4]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE6,		m_nAryPaCntEdge[5]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE7,		m_nAryPaCntEdge[6]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE8,		m_nAryPaCntEdge[7]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE9,		m_nAryPaCntEdge[8]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE10,	m_nAryPaCntEdge[9]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE11,	m_nAryPaCntEdge[10]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE12,	m_nAryPaCntEdge[11]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE13,	m_nAryPaCntEdge[12]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE14,	m_nAryPaCntEdge[13]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE15,	m_nAryPaCntEdge[14]);
	DDX_Text(pDX, IDC_EDIT_PACNT_EDGE16,	m_nAryPaCntEdge[15]);
	//2020.08.26 add by mby E-
}


BEGIN_MESSAGE_MAP(CDlgLuminanceSetup, CDialogEx)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM1_MIN, &CDlgLuminanceSetup::OnEnKillfocusEditCam1Min)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM1_MAX, &CDlgLuminanceSetup::OnEnKillfocusEditCam1Max)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM2_MIN, &CDlgLuminanceSetup::OnEnKillfocusEditCam2Min)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM2_MAX, &CDlgLuminanceSetup::OnEnKillfocusEditCam2Max)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM3_MIN, &CDlgLuminanceSetup::OnEnKillfocusEditCam3Min)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM3_MAX, &CDlgLuminanceSetup::OnEnKillfocusEditCam3Max)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM4_MIN, &CDlgLuminanceSetup::OnEnKillfocusEditCam4Min)
	ON_EN_KILLFOCUS(IDC_EDIT_CAM4_MAX, &CDlgLuminanceSetup::OnEnKillfocusEditCam4Max)
	ON_BN_CLICKED(IDC_CHECK_LUMINANCE_ALARM_USE, &CDlgLuminanceSetup::OnBnClickedCheckLuminanceAlarmUse)
	ON_EN_KILLFOCUS(IDC_EDIT_POL1_MAX, &CDlgLuminanceSetup::OnKillfocusEditPol1Max)
	ON_BN_CLICKED(IDC_BTN_DCC_SAVE, &CDlgLuminanceSetup::OnBnClickedBtnDccSave)
	ON_BN_CLICKED(IDC_BTN_DCC_RESET, &CDlgLuminanceSetup::OnBnClickedBtnDccReset)
	ON_BN_CLICKED(IDC_BTN_DCC_RCP_CHANGE, &CDlgLuminanceSetup::OnBnClickedBtnDccRcpChange)
END_MESSAGE_MAP()


// CDlgLuminanceSetup 메시지 처리기입니다.



// 함수 설명	: 휘도 리밋 값 설정 관련 파라메터를 로드하여 화면에 표시한다.
// Return		: 0 - OK, 그외 에러
int CDlgLuminanceSetup::m_fnLoadParameter()
{
	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strTemp,cCountRoot,strRecipeNo;
	int nRead = 0;
	TCHAR strBuffer[255];

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	m_strRepNo = strRecipeNo;

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MIN, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM1_MIN)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MAX, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM1_MAX)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MIN, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM2_MIN)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MAX, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM2_MAX)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MIN, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM3_MIN)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MAX, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM3_MAX)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MIN, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM4_MIN)->SetWindowText(strTemp);

	//nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MAX, 0);
	//strTemp.Format(_T("%d"), nRead);
	//GetDlgItem(IDC_EDIT_CAM4_MAX)->SetWindowText(strTemp);
	int nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	{
		GetDlgItem(IDC_STATIC_LUMI1)->SetWindowText(_T("CF COAXIAL #1"));
		GetDlgItem(IDC_STATIC_LUMI2)->SetWindowText(_T("CF COAXIAL #2"));
		GetDlgItem(IDC_STATIC_LUMI3)->SetWindowText(_T("CF SIDE #1"));
		GetDlgItem(IDC_STATIC_LUMI4)->SetWindowText(_T("CF SIDE #2"));
		GetDlgItem(IDC_STATIC_LUMI5)->SetWindowText(_T("TFT COAXIAL #1"));
		GetDlgItem(IDC_STATIC_LUMI6)->SetWindowText(_T("TFT COAXIAL #2"));
		GetDlgItem(IDC_STATIC_LUMI7)->SetWindowText(_T("PENETRATION #1"));
		GetDlgItem(IDC_STATIC_LUMI8)->SetWindowText(_T("PENETRATION #2"));

	}
	if(pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE && nEqpModel == EQP_TYPE_ABI)
		GetDlgItem(IDC_EDIT_POL1_MAX)->EnableWindow(TRUE);

	//2016.03.02
	m_bUseLuminanceAlarm = pFrame->m_stOverlapLuminanceInfo->bUseMode;
	m_nLuminanceAlarmSetCount = pFrame->m_stOverlapLuminanceInfo->nLuminanceAlarmSetCount;
	m_nLuminanceAlarmSetPanelCount = pFrame->m_stOverlapLuminanceInfo->nLuminanceAlarmSetPanelCount;
	m_nLuminanceRange = pFrame->m_stOverlapLuminanceInfo->nLuminanceRange;
	m_bUseTdiMin = pFrame->m_stOverlapLuminanceInfo->bUseTdiMin;


	((CButton*)GetDlgItem(IDC_CHECK_TDI_MIN))->SetCheck(m_bUseTdiMin);

	for (int i = 0; i < MAX_LUMINANCE_DATA; i++)
	{
		m_nAryLuminanceLimit[i] = pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[i];
	}
	//2019.12.16 add by mby
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("CHECK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccCheck[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccCheck[i] = nRead;
		strTemp.Format(_T("SET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoOne[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccInfoOne[i] = nRead;
		strTemp.Format(_T("TSET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoTwo[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccInfoTwo[i] = nRead;
		//2020.07.06 add by mby S-
		strTemp.Format(_T("PACNTPD%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPD[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTPDCHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDChk[i] = _ttoi(strTemp);
		//2020.07.06 add by mby E-
	}
	for (int i = 0; i < DCC_COUNT; i++)
	{
		strTemp.Format(_T("CHECKSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccChkSizeX[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccChkSizeX[i] = nRead;
		strTemp.Format(_T("CHECKSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccChkSizeY[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccChkSizeY[i] = nRead;
	}

	for (int i = 0; i < DCC_SIZE_CHECK; i++)
	{
		strTemp.Format(_T("SETSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		//pFrame->m_iniConfigSet.GetProfileStringW(_T("DCC_INFO"), strTemp, _T("0"), (LPTSTR)strBuffer, 255);
		m_strAryDccSizeX[i] = strBuffer;
		strTemp.Format(_T("SETSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		//pFrame->m_iniConfigSet.GetProfileStringW(_T("DCC_INFO"), strTemp, _T("0"), (LPTSTR)strBuffer, 255);
		m_strAryDccSizeY[i] = strBuffer;
	}

	//2020.08.26 add by mby S-
	for (int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("PACNTEDGE%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEdge[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTEDGECHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEdgeChk[i] = _ttoi(strTemp);
	}
	//2020.08.26 add by mby E-
	//2020.04.10 add by mby S-
	for (int i = 0; i < DCC_DIRECT_COUNT; i++)
	{
		strTemp.Format(_T("CHECKDIRECT%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccDirect[i] = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccDirect[i] = nRead;		
	}

	m_fnLuminanceAlarmUseCheck(m_bUseLuminanceAlarm); 
	
	UpdateData(FALSE);

	return nRet;
}

// 함수 설명	: 휘도 리밋 값 설정 관련 파라메터를 저장한다.
// Return		: 0 - OK, 그외 에러
int CDlgLuminanceSetup::m_fnSaveParameter()
{
	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strTemp, strLimit;

	UpdateData();

	//GetDlgItem(IDC_EDIT_CAM1_MIN)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MIN, strTemp);

	//GetDlgItem(IDC_EDIT_CAM1_MAX)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MAX, strTemp);

	//GetDlgItem(IDC_EDIT_CAM2_MIN)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MIN, strTemp);

	//GetDlgItem(IDC_EDIT_CAM2_MAX)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MAX, strTemp);

	//GetDlgItem(IDC_EDIT_CAM3_MIN)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MIN, strTemp);

	//GetDlgItem(IDC_EDIT_CAM3_MAX)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MAX, strTemp);

	//GetDlgItem(IDC_EDIT_CAM4_MIN)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MIN, strTemp);

	//GetDlgItem(IDC_EDIT_CAM4_MAX)->GetWindowText(strTemp);
	//pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MAX, strTemp);

	//2016.03.02
	strTemp.Format(_T("%d"), m_bUseLuminanceAlarm);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_USEMODE, strTemp);

	strTemp.Format(_T("%d"), m_nLuminanceAlarmSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_SET_COUNT, strTemp);

	strTemp.Format(_T("%d"), m_nLuminanceAlarmSetPanelCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_SET_COUNT_PANEL, strTemp);

	strTemp.Format(_T("%d"), m_nLuminanceRange);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_RANGE, strTemp);

	m_bUseTdiMin = ((CButton*)GetDlgItem(IDC_CHECK_TDI_MIN))->GetCheck();

	strTemp.Format(_T("%d"), m_bUseTdiMin);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_USE_TDI_MIN, strTemp);


	strTemp = _T("");
	for (int i = 0; i < MAX_LUMINANCE_DATA; i++)
	{
		strLimit.Format(_T("%d"), m_nAryLuminanceLimit[i]);

		if (i == MAX_LUMINANCE_DATA - 1)	strTemp.Append(strLimit);
		else	strTemp.Append(strLimit + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_LIMIT, strTemp);

	pFrame->m_fnSetOverlapLuminanceInfo();

	//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgLuminanceState->m_fnGetLimitValue();

	return nRet;
}

void CDlgLuminanceSetup::OnEnKillfocusEditCam1Min()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM1_MIN)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM1_MIN)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam1Max()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM1_MAX)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM1_MAX)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam2Min()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM2_MIN)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM2_MIN)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam2Max()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM2_MAX)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM2_MAX)->SetFocus(); 
	}
}



void CDlgLuminanceSetup::OnEnKillfocusEditCam3Min()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM3_MIN)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM3_MIN)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam3Max()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM3_MAX)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM3_MAX)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam4Min()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM4_MIN)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM4_MIN)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnEnKillfocusEditCam4Max()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_CAM4_MAX)->GetWindowText(strTemp);

	if( _tstoi((LPCTSTR)strTemp) < 0 || _tstoi((LPCTSTR)strTemp) > 255 )
	{
		//AfxMessageBox(_T("Out of range.(0 ~ 255)"));
		GetDlgItem(IDC_EDIT_CAM4_MAX)->SetFocus(); 
	}
}


void CDlgLuminanceSetup::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialogEx::OnOK();
}


void CDlgLuminanceSetup::OnBnClickedCheckLuminanceAlarmUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LUMINANCE_ALARM_USE))->GetCheck();
	m_fnLuminanceAlarmUseCheck(bCheck);
	//m_fnSaveParameter();

	//pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;
}

void CDlgLuminanceSetup::OnKillfocusEditPol1Max()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LUMINANCE_ALARM_USE))->GetCheck();

	m_fnLuminanceAlarmUseCheck(bCheck);

	pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;
}


void CDlgLuminanceSetup::OnKillfocusEditPol2Max()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LUMINANCE_ALARM_USE))->GetCheck();

	m_fnLuminanceAlarmUseCheck(bCheck);

	pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;
}


void CDlgLuminanceSetup::OnKillfocusEditPol3Max()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_LUMINANCE_ALARM_USE))->GetCheck();

	m_fnLuminanceAlarmUseCheck(bCheck);

	pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;
}


void CDlgLuminanceSetup::m_fnLuminanceAlarmUseCheck(BOOL bUseCheck)
{
	int nEqpModel;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	//0:AVL 1:ABI
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	GetDlgItem(IDC_EDIT_LUMINANCE_SET_ALARM_COUNT)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_LUMINANCE_SET_PANEL_COUNT)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_LUMINANCE_SET_RANGE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_TDI_MIN)->EnableWindow(bUseCheck);

	if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
	{
		if (nEqpModel == 0)	//AVL
		{
			GetDlgItem(IDC_EDIT_CAM1_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM1_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM4_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM4_MAX)->EnableWindow(bUseCheck);
		}
		else if(nEqpModel == 1)	//ABI
		{
			GetDlgItem(IDC_EDIT_CAM1_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM1_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_POL1_MAX)->EnableWindow(bUseCheck);
		}
	}
	else
	{
		if (nEqpModel == 0)
		{
			GetDlgItem(IDC_EDIT_CAM1_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM1_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MAX)->EnableWindow(bUseCheck);
		}
		else if(nEqpModel == 1)
		{
			GetDlgItem(IDC_EDIT_CAM1_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM1_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM2_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM3_MAX)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM4_MIN)->EnableWindow(bUseCheck);
			GetDlgItem(IDC_EDIT_CAM4_MAX)->EnableWindow(bUseCheck);
		}
	}
}

//2019.12.16 add by mby
void CDlgLuminanceSetup::OnBnClickedBtnDccSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	DOUBLE dCfBurr, dTftBurr, dCfBroken, dTftBroken, dCfCorBroken, dTftCorBroken, dCfPolBubble, dTftPolBubble;	//2020.04.14 add by mby
	CString strCheck1,strCheck2,strCheck3,strCheck4;	//2020.04.14 add by mby
	CString strTemp,strTemp1,strRcpNo;
	CString cCountRoot;
	CString cChkLog1,cChkLog2,cChkLog3;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	UpdateData();

	//2020.04.14 add by mby for judgeset Size over Check
	dCfBurr			= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BURR];
	dTftBurr		= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BURR];
	dCfBroken		= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BROKEN];
	dTftBroken		= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BROKEN];
	dCfCorBroken	= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_CORNER_BROKEN];
	dTftCorBroken	= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_CORNER_BROKEN];
	dCfPolBubble	= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_BUBBLE + 24]; //tft
	dTftPolBubble	= pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_BUBBLE];	//cf

	//2020.04.10 add by mby for Flag Info Check
	if(m_nAryDccDirect[0] && m_nAryDccDirect[1]){
		AfxMessageBox(_T("Please Check Burr Side Flag. Only One Check "), MB_ICONERROR | MB_OK);
		return;
	}	
	if(!m_nAryDccDirect[0] && !m_nAryDccDirect[1]){
		AfxMessageBox(_T("Please make sure to check one.[Burr Side Flag]  "), MB_ICONERROR | MB_OK);
		return;
	}
	//2020.04.10 add by mby for Flag Info Check

	//2020.04.14 add by mby for judge set over check
	if(dCfBurr != NULL){
		GetDlgItem(IDC_EDIT_SIZEX2)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX4)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY2)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY4)->GetWindowText(strCheck4);
		if(dCfBurr < _ttof(strCheck1) || dCfBurr < _ttof(strCheck2) || dCfBurr < _ttof(strCheck3) || dCfBurr < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[Cf/Tft Burr ]  "), MB_ICONERROR | MB_OK);
			return;
		}		
	}
	if(dCfBroken != NULL){
		GetDlgItem(IDC_EDIT_SIZEX6)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX8)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY6)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY8)->GetWindowText(strCheck4);
		if(dTftBurr < _ttof(strCheck1) || dTftBurr < _ttof(strCheck2) || dTftBurr < _ttof(strCheck3) || dTftBurr < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfBroken A ]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX10)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX12)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY10)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY12)->GetWindowText(strCheck4);
		if(dCfBroken < _ttof(strCheck1) || dCfBroken < _ttof(strCheck2) || dCfBroken < _ttof(strCheck3) || dCfBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfBroken B]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX14)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX16)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY14)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY16)->GetWindowText(strCheck4);
		if(dCfBroken < _ttof(strCheck1) || dCfBroken < _ttof(strCheck2) || dCfBroken < _ttof(strCheck3) || dCfBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfBroken C]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX18)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX20)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY18)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY20)->GetWindowText(strCheck4);
		if(dCfBroken < _ttof(strCheck1) || dCfBroken < _ttof(strCheck2) || dCfBroken < _ttof(strCheck3) || dCfBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfBroken D]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}
	if(dTftBroken != NULL){
		GetDlgItem(IDC_EDIT_SIZEX22)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX24)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY22)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY24)->GetWindowText(strCheck4);
		if(dTftBroken < _ttof(strCheck1) || dTftBroken < _ttof(strCheck2) || dTftBroken < _ttof(strCheck3) || dTftBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftBroken A]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX26)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX28)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY26)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY28)->GetWindowText(strCheck4);
		if(dTftBroken < _ttof(strCheck1) || dTftBroken < _ttof(strCheck2) || dTftBroken < _ttof(strCheck3) || dTftBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftBroken B]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX30)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX32)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY30)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY32)->GetWindowText(strCheck4);
		if(dTftBroken < _ttof(strCheck1) || dTftBroken < _ttof(strCheck2) || dTftBroken < _ttof(strCheck3) || dTftBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftBroken C]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX34)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX36)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY34)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY36)->GetWindowText(strCheck4);
		if(dTftBroken < _ttof(strCheck1) || dTftBroken < _ttof(strCheck2) || dTftBroken < _ttof(strCheck3) || dTftBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftBroken D]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}
	if(dCfCorBroken != NULL){
		GetDlgItem(IDC_EDIT_SIZEX38)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX40)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY38)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY40)->GetWindowText(strCheck4);
		if(dCfCorBroken < _ttof(strCheck1) || dCfCorBroken < _ttof(strCheck2) || dCfCorBroken < _ttof(strCheck3) || dCfCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfCorBroken A]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX42)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX44)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY42)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY44)->GetWindowText(strCheck4);
		if(dCfCorBroken < _ttof(strCheck1) || dCfCorBroken < _ttof(strCheck2) || dCfCorBroken < _ttof(strCheck3) || dCfCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfCorBroken B]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX46)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX48)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY46)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY48)->GetWindowText(strCheck4);
		if(dCfCorBroken < _ttof(strCheck1) || dCfCorBroken < _ttof(strCheck2) || dCfCorBroken < _ttof(strCheck3) || dCfCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfCorBroken C]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX50)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX52)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY50)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY52)->GetWindowText(strCheck4);
		if(dCfCorBroken < _ttof(strCheck1) || dCfCorBroken < _ttof(strCheck2) || dCfCorBroken < _ttof(strCheck3) || dCfCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfCorBroken D]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}
	if(dTftCorBroken != NULL){
		GetDlgItem(IDC_EDIT_SIZEX54)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX56)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY54)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY56)->GetWindowText(strCheck4);
		if(dTftCorBroken < _ttof(strCheck1) || dTftCorBroken < _ttof(strCheck2) || dTftCorBroken < _ttof(strCheck3) || dTftCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftCorBroken A]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX58)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX60)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY58)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY60)->GetWindowText(strCheck4);
		if(dTftCorBroken < _ttof(strCheck1) || dTftCorBroken < _ttof(strCheck2) || dTftCorBroken < _ttof(strCheck3) || dTftCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftCorBroken B]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX62)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX64)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY62)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY64)->GetWindowText(strCheck4);
		if(dTftCorBroken < _ttof(strCheck1) || dTftCorBroken < _ttof(strCheck2) || dTftCorBroken < _ttof(strCheck3) || dTftCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftCorBroken C]  "), MB_ICONERROR | MB_OK);
			return;
		}
		GetDlgItem(IDC_EDIT_SIZEX66)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX68)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY66)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY68)->GetWindowText(strCheck4);
		if(dTftCorBroken < _ttof(strCheck1) || dTftCorBroken < _ttof(strCheck2) || dTftCorBroken < _ttof(strCheck3) || dTftCorBroken < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftCorBroken D]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}
	/*if(dCfPolBubble != NULL){
		GetDlgItem(IDC_EDIT_SIZEX26)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX28)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY26)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY28)->GetWindowText(strCheck4);
		if(dCfPolBubble < _ttof(strCheck1) || dCfPolBubble < _ttof(strCheck2) || dCfPolBubble < _ttof(strCheck3) || dCfPolBubble < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[CfPolBubble ]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}
	if(dTftPolBubble != NULL){
		GetDlgItem(IDC_EDIT_SIZEX30)->GetWindowText(strCheck1);
		GetDlgItem(IDC_EDIT_SIZEX32)->GetWindowText(strCheck2);
		GetDlgItem(IDC_EDIT_SIZEY30)->GetWindowText(strCheck3);
		GetDlgItem(IDC_EDIT_SIZEY32)->GetWindowText(strCheck4);
		if(dTftPolBubble < _ttof(strCheck1) || dTftPolBubble < _ttof(strCheck2) || dTftPolBubble < _ttof(strCheck3) || dTftPolBubble < _ttof(strCheck4)){
			AfxMessageBox(_T("Please Check Size Value. is this over then Judge Set Value[TftPolBubble ]  "), MB_ICONERROR | MB_OK);
			return;
		}
	}*/
	//2020.04.14 add by mby for judge set over check

	GetDlgItem(IDC_EDIT_DCC_RCP_CHANGE)->GetWindowText(strRcpNo);
	if(strRcpNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	//2020.04.10 add by mby
	for (int i = 0; i < DCC_DIRECT_COUNT; i++)
	{
		strTemp.Format(_T("%d"), m_nAryDccDirect[i]);
		strTemp1.Format(_T("CHECKDIRECT%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), strTemp1, strTemp);
	}
	
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("%d"), m_nAryDccCheck[i]);
		strTemp1.Format(_T("CHECK%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), strTemp1, strTemp);

		strTemp.Format(_T("%d"), m_nAryPaCntPDChk[i]);
		strTemp1.Format(_T("PACNTPDCHK%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);		
	}
	cChkLog1.Format(_T("DCCCHECK : %d,%d,%d,%d,%d,%d"), m_nAryDccCheck[0], m_nAryDccCheck[1], m_nAryDccCheck[2], m_nAryDccCheck[3], m_nAryDccCheck[4], m_nAryDccCheck[5]);	//2020.04.20 add by mby
	//2020.08.26 ad by mby S-
	for (int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("%d"), m_nAryPaCntEdgeChk[i]);
		strTemp1.Format(_T("PACNTEDGECHK%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);		
	}
	//2020.08.26 ad by mby E-
	for (int i = 0; i < DCC_COUNT; i++)
	{
		strTemp.Format(_T("%d"), m_nAryDccChkSizeX[i]);
		strTemp1.Format(_T("CHECKSIZEX%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), strTemp1, strTemp);
	}
	cChkLog2.Format(_T("CHECKSIZEX : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"), m_nAryDccChkSizeX[0], m_nAryDccChkSizeX[1], m_nAryDccChkSizeX[2], m_nAryDccChkSizeX[3], m_nAryDccChkSizeX[4], m_nAryDccChkSizeX[5]
		, m_nAryDccChkSizeX[6], m_nAryDccChkSizeX[7], m_nAryDccChkSizeX[8], m_nAryDccChkSizeX[9], m_nAryDccChkSizeX[10], m_nAryDccChkSizeX[11]
		, m_nAryDccChkSizeX[12], m_nAryDccChkSizeX[13], m_nAryDccChkSizeX[14], m_nAryDccChkSizeX[15]);	//2020.04.20 add by mby
	for (int i = 0; i < DCC_COUNT; i++)
	{
		strTemp.Format(_T("%d"), m_nAryDccChkSizeY[i]);
		strTemp1.Format(_T("CHECKSIZEY%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), strTemp1, strTemp);
	}
	cChkLog3.Format(_T("CHECKSIZEY : %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"), m_nAryDccChkSizeY[0], m_nAryDccChkSizeY[1], m_nAryDccChkSizeY[2], m_nAryDccChkSizeY[3], m_nAryDccChkSizeY[4], m_nAryDccChkSizeY[5]
		, m_nAryDccChkSizeY[6], m_nAryDccChkSizeY[7], m_nAryDccChkSizeY[8], m_nAryDccChkSizeY[9], m_nAryDccChkSizeY[10], m_nAryDccChkSizeY[11]
		, m_nAryDccChkSizeY[12], m_nAryDccChkSizeY[13], m_nAryDccChkSizeY[14], m_nAryDccChkSizeY[15]);	//2020.04.20 add by mby

	GetDlgItem(IDC_EDIT_DCC_VALUE1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SET1"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SET1"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_VALUE2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SET2"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SET2"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_VALUE3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SET3"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SET3"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_VALUE4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SET4"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SET4"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_VALUE5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SET5"), strTemp, cCountRoot);


	GetDlgItem(IDC_EDIT_DCC_TVALUE1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("TSET1"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("TSET1"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_TVALUE2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("TSET2"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("TSET2"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_TVALUE3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("TSET3"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("TSET3"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_TVALUE4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("TSET4"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("TSET4"), strTemp);
	GetDlgItem(IDC_EDIT_DCC_TVALUE5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("TSET5"), strTemp, cCountRoot);

	GetDlgItem(IDC_EDIT_SIZEX1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX1"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX1"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX2"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX2"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX3"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX3"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX4"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX4"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX5"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX5"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX6)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX6"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX6"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX7)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX7"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX7"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX8)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX8"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX8"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX9)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX9"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX9"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX10)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX10"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX10"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX11)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX11"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX11"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX12)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX12"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX12"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX13)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX13"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX13"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX14)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX14"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX14"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX15)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX15"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX15"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX16)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX16"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX16"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX17)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX17"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX17"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX18)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX18"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX18"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX19)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX19"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX19"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX20)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX20"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX20"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX21)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX21"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX21"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX22)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX22"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX22"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX23)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX23"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX23"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX24)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX24"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX24"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX25)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX25"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX25"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX26)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX26"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX26"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX27)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX27"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX27"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX28)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX28"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX28"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX29)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX29"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX29"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX30)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX30"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX30"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX31)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX31"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX31"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX32)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX32"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX32"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX33)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX33"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX33"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX34)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX34"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX34"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX35)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX35"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX35"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX36)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX36"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX36"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX37)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX37"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX37"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX38)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX38"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX38"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX39)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX39"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX39"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX40)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX40"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX40"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX41)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX41"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX41"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX42)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX42"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX42"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX43)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX43"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX43"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX44)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX44"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX44"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX45)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX45"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX45"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX46)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX46"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX46"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX47)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX47"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX47"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX48)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX48"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX48"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX49)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX49"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX49"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX50)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX50"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX50"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX51)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX51"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX51"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX52)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX52"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX52"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX53)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX53"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX53"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX54)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX54"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX54"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX55)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX55"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX55"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX56)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX56"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX56"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX57)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX57"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX57"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX58)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX58"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX58"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX59)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX59"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX59"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX60)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX60"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX60"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX61)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX61"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX61"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX62)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX62"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX62"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX63)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX63"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX63"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX64)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX64"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX64"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX65)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX65"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX65"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX66)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX66"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX66"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX67)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX67"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX67"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEX68)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEX68"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEX68"), strTemp);


	GetDlgItem(IDC_EDIT_SIZEY1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY1"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY1"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY2"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY2"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY3"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY3"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY4"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY4"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY5"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY5"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY6)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY6"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY6"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY7)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY7"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY7"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY8)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY8"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY8"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY9)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY9"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY9"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY10)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY10"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY10"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY11)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY11"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY11"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY12)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY12"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY12"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY13)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY13"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY13"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY14)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY14"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY14"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY15)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY15"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY15"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY16)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY16"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY16"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY17)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY17"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY17"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY18)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY18"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY18"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY19)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY19"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY19"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY20)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY20"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY20"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY21)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY21"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY21"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY22)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY22"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY22"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY23)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY23"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY23"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY24)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY24"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY24"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY25)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY25"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY25"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY26)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY26"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY26"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY27)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY27"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY27"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY28)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY28"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY28"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY29)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY29"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY29"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY30)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY30"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY30"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY31)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY31"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY31"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY32)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY32"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY32"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY33)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY33"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY33"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY34)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY34"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY34"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY35)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY35"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY35"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY36)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY36"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY36"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY37)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY37"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY37"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY38)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY38"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY38"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY39)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY39"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY39"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY40)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY40"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY40"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY41)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY41"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY41"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY42)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY42"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY42"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY43)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY43"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY43"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY44)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY44"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY44"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY45)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY45"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY45"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY46)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY46"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY46"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY47)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY47"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY47"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY48)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY48"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY48"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY49)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY49"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY49"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY50)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY50"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY50"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY51)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY51"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY51"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY52)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY52"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY52"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY53)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY53"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY53"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY54)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY54"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY54"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY55)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY55"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY55"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY56)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY56"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY56"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY57)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY57"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY57"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY58)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY58"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY58"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY59)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY59"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY59"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY60)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY60"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY60"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY61)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY61"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY61"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY62)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY62"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY62"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY63)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY63"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY63"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY64)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY64"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY64"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY65)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY65"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY65"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY66)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY66"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY66"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY67)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY67"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY67"), strTemp);
	GetDlgItem(IDC_EDIT_SIZEY68)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("SETSIZEY68"), strTemp, cCountRoot);
	//pFrame->m_iniConfigSet.WriteProfileStringW(_T("DCC_INFO"), _T("SETSIZEY68"), strTemp);

	WritePrivateProfileString(_T("LAST"), _T("INCH"), strRcpNo, cCountRoot);

	//2020.07.06 add by mby
	GetDlgItem(IDC_EDIT_PACNT_PD1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTPD1"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_PD2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTPD2"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_PD3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTPD3"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_PD4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTPD4"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_PD5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTPD5"), strTemp, cCountRoot);

	//2020.08.26 add by mby S-
	GetDlgItem(IDC_EDIT_PACNT_EDGE1)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE1"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE2)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE2"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE3)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE3"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE4)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE4"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE5)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE5"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE6)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE6"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE7)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE7"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE8)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE8"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE9)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE9"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE10)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE10"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE11)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE11"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE12)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE12"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE13)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE13"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE14)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE14"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE15)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE15"), strTemp, cCountRoot);
	GetDlgItem(IDC_EDIT_PACNT_EDGE16)->GetWindowText(strTemp);
	WritePrivateProfileString(strRcpNo, _T("PACNTEDGE16"), strTemp, cCountRoot);
	//2020.08.26 add by mby E-	

	G_AddLog(3, 0, _T(" %s -- %s -- %s "), cChkLog1,cChkLog2,cChkLog3 );	//2020.04.20 add by mby
}


void CDlgLuminanceSetup::OnBnClickedBtnDccReset()
{
	// TODO: ??? ??? ?? ??? ??? ?????.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	//if (AfxMessageBox(_T("Do you want checked Panelcount data reset? "), MB_OKCANCEL) != IDOK) return;
	
	int nRead = 0;
	CString cCountRoot;
	CString strTemp,strTemp1,strRcpNo;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	GetDlgItem(IDC_EDIT_DCC_RCP_CHANGE)->GetWindowText(strRcpNo);
	if(strRcpNo==_T("")){
		AfxMessageBox(_T("Input Recp No"));
		return;
	}
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("%d"), 0);
		strTemp1.Format(_T("TCHECK%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//WritePrivateProfileString(_T("DCC_COUNT"), strTemp1, strTemp, cCountRoot);
		
		strTemp.Format(_T("%d"),0);	
		strTemp1.Format(_T("TOTAL%d"), i+1);
		WritePrivateProfileString(strRcpNo, strTemp1, strTemp, cCountRoot);
		//WritePrivateProfileString(_T("DCC_COUNT"), strTemp1, strTemp, cCountRoot);
	}
}


void CDlgLuminanceSetup::OnBnClickedBtnDccRcpChange()
{
	int nRet = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString strTemp,cCountRoot,strRecipeNo;
	int nRead = 0;
	TCHAR strBuffer[255];
	UpdateData();

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	GetDlgItem(IDC_EDIT_DCC_RCP_CHANGE)->GetWindowText(strRecipeNo);
	if(strRecipeNo ==_T("")) return;

	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("CHECK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccCheck[i] = _ttoi(strTemp);
		
		strTemp.Format(_T("SET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoOne[i] = _ttoi(strTemp);
	
		strTemp.Format(_T("TSET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoTwo[i] = _ttoi(strTemp);

		//2020.07.06 add by mby S-
		strTemp.Format(_T("PACNTPD%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPD[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTPDCHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDChk[i] = _ttoi(strTemp);
		//2020.07.06 add by mby E-
	}
	for (int i = 0; i < DCC_COUNT; i++)
	{
		strTemp.Format(_T("CHECKSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccChkSizeX[i] = _ttoi(strTemp);
	
		strTemp.Format(_T("CHECKSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccChkSizeY[i] = _ttoi(strTemp);		
	}

	for (int i = 0; i < DCC_SIZE_CHECK; i++)
	{
		strTemp.Format(_T("SETSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);			
		m_strAryDccSizeX[i] = strBuffer;
		strTemp.Format(_T("SETSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		m_strAryDccSizeY[i] = strBuffer;
	}
	for (int i = 0; i < DCC_DIRECT_COUNT; i++)
	{
		strTemp.Format(_T("CHECKDIRECT%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccDirect[i] = _ttoi(strTemp);	
	}
	//2020.08.26 add by mby S-
	for (int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("PACNTEDGE%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEdge[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTEDGECHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEdgeChk[i] = _ttoi(strTemp);
	}
	//2020.08.26 add by mby E-
	UpdateData(FALSE);
}
