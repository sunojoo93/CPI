
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgDefectMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgDefectInfo.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewAuto.h"
#include <vector>
#include "Util\BIniFile.h"	
#include "DlgPanelJudge.h"

// CDlgDefectInfo 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgDefectInfo, CDialogEx)

	CDlgDefectInfo::CDlgDefectInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDefectInfo::IDD, pParent)
{
	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,CONFIG_COUNT_PATH );
	m_stTotalDefectType.initDefType();
	m_stTotalDefectType.setInfoPath(PNL_DEF_INFO, strRoot);
	m_pDlgLogin					 = NULL;
	m_bMaualFlg					 = FALSE;
}

CDlgDefectInfo::~CDlgDefectInfo()
{
	m_stTotalDefectType.initDefType();
	if (m_pDlgLogin)
	{
		delete m_pDlgLogin;
		m_pDlgLogin = NULL;
	}
}

void CDlgDefectInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgDefectInfo, CDialogEx)
	ON_WM_PAINT()
	ON_MESSAGE(WM_UI_CONTROL_DEF_TYPE_DIS, &CDlgDefectInfo::m_fnUpdateDefType)
	ON_BN_CLICKED(IDC_BTN_PNL_CNT_RESET, &CDlgDefectInfo::OnBnClickedBtnPnlCntReset)
END_MESSAGE_MAP()


// CDlgDefectMap 메시지 처리기입니다.


void CDlgDefectInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	m_fnChangeFont(m_strDef);

}

void CDlgDefectInfo::m_fnInitDefectInfo()
{

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->SetCaption(_T(""));
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->EnableWindow(FALSE);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->SetCaption(_T(""));
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->EnableWindow(FALSE);
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->SetCaption(_T(""));

	int nEqpModel= 0;

	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	//2016.02.01 Packing UI
	if (pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
	{
		if (!nEqpModel)
		{
			// AVL 
			// POL BUBBLE
			GetDlgItem(IDC_TXT_PB)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PB2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL3)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL7)->ShowWindow(FALSE);
			// POL SCRACH
			GetDlgItem(IDC_LABEL5)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PS)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PS2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL9)->ShowWindow(FALSE);
			// CF POL SUB
			GetDlgItem(IDC_LABEL11)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PAS)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PAS2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL13)->ShowWindow(FALSE);
			// TFT POL SUB
			GetDlgItem(IDC_LABEL15)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PM)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_PM2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL17)->ShowWindow(FALSE);
			// MURA
			GetDlgItem(IDC_LABEL35)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_MURA)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_MURA2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL37)->ShowWindow(FALSE);

			// 2018.05.30 add by yhs -s Auto View Defect Info 수정
			// BURR
			GetDlgItem(IDC_LABEL38)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BURR)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BURR2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL39)->ShowWindow(FALSE);
			// BROKEN
			GetDlgItem(IDC_LABEL14)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EB)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EB2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL16)->ShowWindow(FALSE);
			// WHITE TURBITY
			GetDlgItem(IDC_LABEL4)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WT)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WT2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL8)->ShowWindow(SW_HIDE);
			// ITO
			GetDlgItem(IDC_LABEL10)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_ITO)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_ITO2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL12)->ShowWindow(FALSE);
			// DUST TYPE
			GetDlgItem(IDC_LABEL2)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DUST)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DUST2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL6)->ShowWindow(FALSE);
			// PAD SCRACH
			//GetDlgItem(IDC_LABEL18)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADSC)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADSC2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL20)->ShowWindow(SW_HIDE);
			// DIMPLE POINT
			GetDlgItem(IDC_LABEL22)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DP)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DP2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL24)->ShowWindow(SW_HIDE);
			// ETCHING LINE
			GetDlgItem(IDC_LABEL26)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_EL)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_EL2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL28)->ShowWindow(SW_HIDE);
			// DIMPLE LINE
			GetDlgItem(IDC_LABEL30)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DIMPLINE)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DIMPLINE2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL32)->ShowWindow(SW_HIDE);
			// BLALCK SCRACH
			GetDlgItem(IDC_LABEL19)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL21)->ShowWindow(SW_HIDE);
			// WHITE SCRACH
			GetDlgItem(IDC_LABEL23)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL25)->ShowWindow(SW_HIDE);
			// CF MEASURE
			//GetDlgItem(IDC_LABEL27)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_CM)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_CM2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL29)->ShowWindow(SW_HIDE);
			// TFT MEASURE
			//GetDlgItem(IDC_LABEL31)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_TM)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_TM2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL33)->ShowWindow(SW_HIDE);
			// 2018.05.30 add by yhs -e
		}
		else
		{
			// ABI
			// DUST TYPE
			GetDlgItem(IDC_LABEL2)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DUST)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DUST2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL6)->ShowWindow(FALSE);
			// WHITE TURBITY
			GetDlgItem(IDC_LABEL4)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_WT)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_WT2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL8)->ShowWindow(FALSE);
			// ITO
			GetDlgItem(IDC_LABEL10)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_ITO)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_ITO2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL12)->ShowWindow(FALSE);
			// DIMPLE POINT
			GetDlgItem(IDC_LABEL22)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DP)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DP2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL24)->ShowWindow(FALSE);
			// ETCHING LINE
			GetDlgItem(IDC_LABEL26)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EL)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EL2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL28)->ShowWindow(FALSE);
			// DIMPLE LINE
			GetDlgItem(IDC_LABEL30)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DIMPLINE)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_DIMPLINE2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL32)->ShowWindow(FALSE);
			// BLACK SCRACH
			GetDlgItem(IDC_LABEL19)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BS)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BS2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL21)->ShowWindow(FALSE);
			// WHITE SCRACH
			GetDlgItem(IDC_LABEL23)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_WS)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_WS2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL25)->ShowWindow(FALSE);
			
			// 2018.05.30 add by yhs -s Auto View Defect Info 수정
			// BURR
			GetDlgItem(IDC_LABEL38)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BURR)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_BURR2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL39)->ShowWindow(FALSE);
			// BROKEN
			GetDlgItem(IDC_LABEL14)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EB)->ShowWindow(FALSE);GetDlgItem(IDC_TXT_EB2)->ShowWindow(FALSE);GetDlgItem(IDC_LABEL16)->ShowWindow(FALSE);
			// 2018.05.30 add by yhs -e
		}
	}
	else
	{
		// DUST TYPE
		GetDlgItem(IDC_LABEL2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DUST)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DUST2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL6)->ShowWindow(SW_HIDE);
		// WHITE TURBITY
		GetDlgItem(IDC_LABEL4)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WT)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WT2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL8)->ShowWindow(SW_HIDE);
		// ITO
		GetDlgItem(IDC_LABEL10)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_ITO)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_ITO2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL12)->ShowWindow(SW_HIDE);
		// DIMPLE POINT
		GetDlgItem(IDC_LABEL22)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DP)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DP2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL24)->ShowWindow(SW_HIDE);
		// ETCHING LINE
		GetDlgItem(IDC_LABEL26)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_EL)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_EL2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL28)->ShowWindow(SW_HIDE);
		// DIMPLE LINE
		GetDlgItem(IDC_LABEL30)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DIMPLINE)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_DIMPLINE2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL32)->ShowWindow(SW_HIDE);
		// BLALCK SCRACH
		GetDlgItem(IDC_LABEL19)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL21)->ShowWindow(SW_HIDE);
		// WHITE SCRACH
		GetDlgItem(IDC_LABEL23)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_WS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL25)->ShowWindow(SW_HIDE);
		// POL BUBBLE
		GetDlgItem(IDC_TXT_PB)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PB2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL3)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL7)->ShowWindow(SW_HIDE);
		// POL SCRACH
		GetDlgItem(IDC_LABEL5)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL9)->ShowWindow(SW_HIDE);
		// CF POL SUB 
		GetDlgItem(IDC_LABEL11)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PAS)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PAS2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL13)->ShowWindow(SW_HIDE);
		// TFT POL SUB
		GetDlgItem(IDC_LABEL15)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PM)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PM2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL17)->ShowWindow(SW_HIDE);
		// MURA
		GetDlgItem(IDC_LABEL35)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_MURA)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_MURA2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL37)->ShowWindow(SW_HIDE);
		// PAD SCRACH
		GetDlgItem(IDC_LABEL18)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADSC)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADSC2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL20)->ShowWindow(SW_HIDE);
		// PAD BURN
		GetDlgItem(IDC_LABEL34)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADBURN)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_PADBURN2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL36)->ShowWindow(SW_HIDE);
		// BURR
		GetDlgItem(IDC_LABEL38)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BURR)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_BURR2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL39)->ShowWindow(SW_HIDE);
		// CF MEASURE
		GetDlgItem(IDC_LABEL27)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_CM)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_CM2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL29)->ShowWindow(SW_HIDE);
		// TFT MEASURE
		GetDlgItem(IDC_LABEL31)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_TM)->ShowWindow(SW_HIDE);GetDlgItem(IDC_TXT_TM2)->ShowWindow(SW_HIDE);GetDlgItem(IDC_LABEL33)->ShowWindow(SW_HIDE);
	}


	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(i == 0)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->SetCaption(_T("0"));
		}
		else if(i == 1)
		{
			((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->EnableWindow(TRUE);
			((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->SetCaption(_T("0"));
		}
		else if(i == 2)
		{
		//	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->EnableWindow(TRUE);
		//	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->SetCaption(_T("0"));
		}
	}	

	CString cCountRoot, cCountVal;

	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);

	int nGradeOK	= GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
	int nGradeNG	= GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
	int nGradeTotal	= nGradeOK + nGradeNG;

	if(nGradeTotal)
		cCountVal.Format(_T("%d	(%.1f%%)"), nGradeOK, (float)nGradeOK / (float)nGradeTotal * 100);
	else
		cCountVal.Format(_T("%d (%d%%)"), nGradeOK, 0);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->SetCaption(cCountVal);

	if(nGradeTotal)
		cCountVal.Format(_T("%d	(%.1f%%)"), nGradeNG, (float)nGradeNG / (float)nGradeTotal * 100);
	else
		cCountVal.Format(_T("%d (%d%%)"), nGradeNG, 0);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->SetCaption(cCountVal);

	cCountVal.Format(_T("%d"), nGradeTotal);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_TOTALCELL))->SetCaption(cCountVal);

	SetDlgDefTypeVal();
}

LRESULT CDlgDefectInfo::m_fnUpdateDefType(WPARAM wParam, LPARAM lParam) //(STRU_DEFECT_TYPE stDefType)
{
	STRU_DEFECT_TYPE* stDefType;		
	 stDefType = (STRU_DEFECT_TYPE*)lParam;
	 //delete stDefType;
	//pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//pFrame->m_iniConfigSet.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);

	try
	{		
		m_stDefectType.initDefType();		

		m_stDefectType = *stDefType;
		
		m_stTotalDefectType.AddDefCnt(m_stDefectType);
		m_stTotalDefectType.WriteDefInfo();

		//SetDlgDefTypeVal();	//2020.10.13 comment by mby

	
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
	
	return 0;
}
void CDlgDefectInfo::SetDlgDefTypeVal()
{
	CString cDataValue = _T("");
	float dPersentCnt = 0;
	m_stTotalDefectType.GetDefInfo();

	m_stTotalDefectType.TotalDefType();
//	cDataValue.Format(_T("%d"),m_stTotalDefectType.TotalCnt );

	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfNgCnt + m_stTotalDefectType.CfMeasureFail + m_stTotalDefectType.TftNgCnt + m_stTotalDefectType.TftMeasureFail);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATICEXCTRL1))->SetCaption(cDataValue);
	// Dust Type
	cDataValue.Format(_T("%d"),m_stTotalDefectType.DustType );
	GetDlgItem(IDC_TXT_DUST)->SetWindowText(cDataValue); 
	if (!(m_stTotalDefectType.DustType))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)(m_stTotalDefectType.DustType *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_DUST2)->SetWindowText(cDataValue);

	// WHITE TURBITY
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfWhiteBlur + m_stTotalDefectType.TftWhiteBlur);
	GetDlgItem(IDC_TXT_WT)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfWhiteBlur))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)(m_stTotalDefectType.CfWhiteBlur *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_WT2)->SetWindowText(cDataValue);

	// ITO
	cDataValue.Format(_T("%d"),m_stTotalDefectType.ItoPeel );
	GetDlgItem(IDC_TXT_ITO)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.ItoPeel))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)(m_stTotalDefectType.ItoPeel *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_ITO2)->SetWindowText(cDataValue);

	// Broken
	cDataValue.Format(_T("%d"),m_stTotalDefectType.WanesBroken + m_stTotalDefectType.CfCornerBroken + m_stTotalDefectType.CfBroken + m_stTotalDefectType.TftBroken + m_stTotalDefectType.TftCornerBroken + m_stTotalDefectType.CfCellBroken + m_stTotalDefectType.TftCellBroken);
	GetDlgItem(IDC_TXT_EB)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.WanesBroken+ m_stTotalDefectType.CfCornerBroken + m_stTotalDefectType.CfBroken + m_stTotalDefectType.TftBroken + m_stTotalDefectType.TftCornerBroken + m_stTotalDefectType.CfCellBroken + m_stTotalDefectType.TftCellBroken))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.WanesBroken + m_stTotalDefectType.CfCornerBroken + m_stTotalDefectType.CfBroken + m_stTotalDefectType.TftBroken
												+ m_stTotalDefectType.TftCornerBroken + m_stTotalDefectType.CfCellBroken + m_stTotalDefectType.TftCellBroken) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_EB2)->SetWindowText(cDataValue);

	// Socket Scrach
	cDataValue.Format(_T("%d"),m_stTotalDefectType.SocketScrach );
	GetDlgItem(IDC_TXT_PADSC)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.SocketScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"), (float)(m_stTotalDefectType.SocketScrach *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_PADSC2)->SetWindowText(cDataValue);

	// Dimple point
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfDimplePoint + m_stTotalDefectType.TftDimplePoint);
	GetDlgItem(IDC_TXT_DP)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfDimplePoint + m_stTotalDefectType.TftDimplePoint))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfDimplePoint + m_stTotalDefectType.TftDimplePoint) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_DP2)->SetWindowText(cDataValue);

	//Etching line
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfEdgeilegal + m_stTotalDefectType.TftEdgeilegal) ;
	GetDlgItem(IDC_TXT_EL)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfEdgeilegal + m_stTotalDefectType.TftEdgeilegal))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfEdgeilegal + m_stTotalDefectType.TftEdgeilegal) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_EL2)->SetWindowText(cDataValue);

	// Dimple line
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfDimpleLine + m_stTotalDefectType.TftDimpleLine );
	GetDlgItem(IDC_TXT_DIMPLINE)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfDimpleLine + m_stTotalDefectType.TftDimpleLine))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfDimpleLine + m_stTotalDefectType.TftDimpleLine) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_DIMPLINE2)->SetWindowText(cDataValue);

	// Pad Burn
	cDataValue.Format(_T("%d"),m_stTotalDefectType.SocketBurn );
	GetDlgItem(IDC_TXT_PADBURN)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.SocketBurn))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.SocketBurn) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_PADBURN2)->SetWindowText(cDataValue);

	// BURR
	cDataValue.Format(_T("%d"),m_stTotalDefectType.Burr + m_stTotalDefectType.CfBurr + m_stTotalDefectType.TftBurr);
	GetDlgItem(IDC_TXT_BURR)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.Burr + m_stTotalDefectType.CfBurr + m_stTotalDefectType.TftBurr))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.Burr + m_stTotalDefectType.CfBurr + m_stTotalDefectType.TftBurr) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_BURR2)->SetWindowText(cDataValue);

	// Pol Bubble
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfPolBubble + m_stTotalDefectType.TftPolBubble );
	GetDlgItem(IDC_TXT_PB)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfPolBubble + m_stTotalDefectType.TftPolBubble))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfPolBubble + m_stTotalDefectType.TftPolBubble) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_PB2)->SetWindowText(cDataValue);

	// Pol Scrach
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfPolScrach + m_stTotalDefectType.TftPolScrach );
	GetDlgItem(IDC_TXT_PS)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfPolScrach + m_stTotalDefectType.TftPolScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfPolScrach + m_stTotalDefectType.TftPolScrach) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_PS2)->SetWindowText(cDataValue);

	// CF Pol Sub
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CFpolSubstance );				
	GetDlgItem(IDC_TXT_PAS)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CFpolSubstance))								
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CFpolSubstance) *100)/m_stTotalDefectType.TotalCnt);			
	GetDlgItem(IDC_TXT_PAS2)->SetWindowText(cDataValue);

	//cDataValue.Format(_T("%d"),m_stTotalDefectType.PolMinor);			// Deleted by KYH01 폴 마이너 미사용 삭제
	//GetDlgItem(IDC_TXT_PM)->SetWindowText(cDataValue);
	//if (!(m_stTotalDefectType.PolMinor))
	//	cDataValue = _T("0.00");
	//else
	//	cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.PolMinor) *100)/m_stTotalDefectType.TotalCnt);
	//GetDlgItem(IDC_TXT_PM2)->SetWindowText(cDataValue);

	// // TFT POL SUB
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TFTpolSubstance);	// Modified by KYH01 POL Minor -> TFT Pol Substance 로 변경
	GetDlgItem(IDC_TXT_PM)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TFTpolSubstance))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TFTpolSubstance) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_PM2)->SetWindowText(cDataValue);

	// Black Scrach
	//cDataValue.Format(_T("%d"),m_stTotalDefectType.BlackScrach);
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftScrach + m_stTotalDefectType.CfScrach);
	GetDlgItem(IDC_TXT_BS)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftScrach + m_stTotalDefectType.CfScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"), (float)((m_stTotalDefectType.TftScrach + m_stTotalDefectType.CfScrach) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_BS2)->SetWindowText(cDataValue);

	// White Scrace
	cDataValue.Format(_T("%d"),m_stTotalDefectType.WhiteScrach);
	GetDlgItem(IDC_TXT_WS)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.WhiteScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.WhiteScrach) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_WS2)->SetWindowText(cDataValue);

	// CF Measure
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfMeasureFail);
	GetDlgItem(IDC_TXT_CM)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfMeasureFail))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfMeasureFail) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CM2)->SetWindowText(cDataValue);

	// TFT Measure
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftMeasureFail);
	GetDlgItem(IDC_TXT_TM)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftMeasureFail))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftMeasureFail) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TM2)->SetWindowText(cDataValue);

	// Mura
	cDataValue.Format(_T("%d"),m_stTotalDefectType.mura );
	GetDlgItem(IDC_TXT_MURA)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.mura))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.mura) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_MURA2)->SetWindowText(cDataValue);


	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfNgCnt + m_stTotalDefectType.CfMeasureFail);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->SetCaption(cDataValue);

	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftNgCnt + m_stTotalDefectType.TftMeasureFail);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->SetCaption(cDataValue);

	//cDataValue.Format(_T("%d"),m_stTotalDefectType.PenetNgCnt);
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->SetCaption(cDataValue);

	//cDataValue.Format(_T("%d"),pFrame->m_DataManager.m_fnGetCfNgCnt());
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC1))->SetCaption(cDataValue);

	//cDataValue.Format(_T("%d"),pFrame->m_DataManager.m_fnGetTftNgCnt());
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC2))->SetCaption(cDataValue);

	//cDataValue.Format(_T("%d"),pFrame->m_DataManager.m_fnGetPenetNgCnt());
	//((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_PC3))->SetCaption(cDataValue);

	// 2018.05.30 add by yhs -s Auto View Defect Info 수정
	// CF BURR A
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBurrA);
	GetDlgItem(IDC_TXT_CF_BURR_A)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBurrA))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBurrA) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BURR_A2)->SetWindowText(cDataValue);

	// CF BURR B
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBurrB);
	GetDlgItem(IDC_TXT_CF_BURR_B)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBurrB))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBurrB) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BURR_B2)->SetWindowText(cDataValue);

	// CF BURR C
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBurrC);
	GetDlgItem(IDC_TXT_CF_BURR_C)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBurrC))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBurrC) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BURR_C2)->SetWindowText(cDataValue);

	// CF BURR D
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBurrD);
	GetDlgItem(IDC_TXT_CF_BURR_D)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBurrD))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBurrD) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BURR_D2)->SetWindowText(cDataValue);

	// TFT BURR A
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBurrA);
	GetDlgItem(IDC_TXT_TFT_BURR_A)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBurrA))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBurrA) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BURR_A2)->SetWindowText(cDataValue);

	// TFT BURR B
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBurrB);
	GetDlgItem(IDC_TXT_TFT_BURR_B)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBurrB))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBurrB) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BURR_B2)->SetWindowText(cDataValue);

	// TFT BURR C
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBurrC);
	GetDlgItem(IDC_TXT_TFT_BURR_C)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBurrC))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBurrC) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BURR_C2)->SetWindowText(cDataValue);

	// TFT BURR D
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBurrD);
	GetDlgItem(IDC_TXT_TFT_BURR_D)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBurrD))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBurrD) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BURR_D2)->SetWindowText(cDataValue);

	// CF BORKEN A + CF CORNERBROKEN A  (Edge + Side)
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBrokenA + m_stTotalDefectType.CfCornerBrokenA);
	GetDlgItem(IDC_TXT_CF_BROKEN_A)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBrokenA))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBrokenA + m_stTotalDefectType.CfCornerBrokenA) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BROKEN_A2)->SetWindowText(cDataValue);

	// CF BORKEN B
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBrokenB + m_stTotalDefectType.CfCornerBrokenB);
	GetDlgItem(IDC_TXT_CF_BROKEN_B)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBrokenB))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBrokenB + m_stTotalDefectType.CfCornerBrokenB) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BROKEN_B2)->SetWindowText(cDataValue);

	// CF BORKEN C
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBrokenC + m_stTotalDefectType.CfCornerBrokenC);
	GetDlgItem(IDC_TXT_CF_BROKEN_C)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBrokenC))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBrokenC + m_stTotalDefectType.CfCornerBrokenC) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BROKEN_C2)->SetWindowText(cDataValue);

	// CF BORKEN D
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfBrokenD + m_stTotalDefectType.CfCornerBrokenD);
	GetDlgItem(IDC_TXT_CF_BROKEN_D)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfBrokenD))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfBrokenD + m_stTotalDefectType.CfCornerBrokenD) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_BROKEN_D2)->SetWindowText(cDataValue);

	// TFT BORKEN A
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBrokenA + m_stTotalDefectType.TftCornerBrokenA);
	GetDlgItem(IDC_TXT_TFT_BROKEN_A)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBrokenA))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBrokenA + m_stTotalDefectType.TftCornerBrokenA) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BROKEN_A2)->SetWindowText(cDataValue);

	// TFT BORKEN B
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBrokenB + m_stTotalDefectType.TftCornerBrokenB);
	GetDlgItem(IDC_TXT_TFT_BROKEN_B)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBrokenB))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBrokenB + m_stTotalDefectType.TftCornerBrokenB) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BROKEN_B2)->SetWindowText(cDataValue);

	// TFT BORKEN C
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBrokenC + m_stTotalDefectType.TftCornerBrokenC);
	GetDlgItem(IDC_TXT_TFT_BROKEN_C)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBrokenC))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBrokenC + m_stTotalDefectType.TftCornerBrokenC) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BROKEN_C2)->SetWindowText(cDataValue);

	// TFT BORKEN D
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftBrokenD + m_stTotalDefectType.TftCornerBrokenD);
	GetDlgItem(IDC_TXT_TFT_BROKEN_D)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftBrokenD))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftBrokenD + m_stTotalDefectType.TftCornerBrokenD) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_BROKEN_D2)->SetWindowText(cDataValue);

	// CF SCRACH
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfScrach);
	GetDlgItem(IDC_TXT_CF_SCRATCH)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfScrach) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_SCRATCH2)->SetWindowText(cDataValue);

	// TFT SCRACH
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftScrach);
	GetDlgItem(IDC_TXT_TFT_SCRATCH)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftScrach))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftScrach) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_SCRATCH2)->SetWindowText(cDataValue);

	// CF CELL BROKEN
	cDataValue.Format(_T("%d"),m_stTotalDefectType.CfCellBroken);
	GetDlgItem(IDC_TXT_CF_CELLBROKEN)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.CfCellBroken))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.CfCellBroken) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_CF_CELLBROKEN2)->SetWindowText(cDataValue);

	// TFT CELl BROKEN
	cDataValue.Format(_T("%d"),m_stTotalDefectType.TftCellBroken);
	GetDlgItem(IDC_TXT_TFT_CELLBROKEN)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.TftCellBroken))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.TftCellBroken) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_TFT_CELLBROKEN2)->SetWindowText(cDataValue);

	// ALIGN FAIL
	cDataValue.Format(_T("%d"),m_stTotalDefectType.AlignFail);
	GetDlgItem(IDC_TXT_ALIGNFAIL)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.AlignFail))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"),(float)((m_stTotalDefectType.AlignFail) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_ALIGNFAIL2)->SetWindowText(cDataValue);

	// Other
	cDataValue.Format(_T("%d"),m_stTotalDefectType.EtcLine + m_stTotalDefectType.EtcPoint);
	GetDlgItem(IDC_TXT_OTHER)->SetWindowText(cDataValue);
	if (!(m_stTotalDefectType.EtcLine + m_stTotalDefectType.EtcPoint))
		cDataValue = _T("0.00");
	else
		cDataValue.Format(_T("%0.2f"), (float)((m_stTotalDefectType.EtcLine + m_stTotalDefectType.EtcPoint) *100)/m_stTotalDefectType.TotalCnt);
	GetDlgItem(IDC_TXT_OTHER2)->SetWindowText(cDataValue);

	// 2018.05.30 add by yhs -e


	CString cCountRoot, cCountVal;
	cCountRoot.Format(_T("%s%s"),ROOT_PATH, CONFIG_COUNT_PATH);

	CDlgPanelJudge * pDlgPnlJudge = (CDlgPanelJudge*)AfxGetMainWnd();	

	int nGradeOK = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
	int nGradeNG = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
	int nGradeTotal	= nGradeOK + nGradeNG;

	if(nGradeTotal)
		cCountVal.Format(_T("%d	(%.1f%%)"), nGradeOK, (float)nGradeOK / (float)nGradeTotal * 100);
	else
		cCountVal.Format(_T("%d (%d%%)"), nGradeOK, 0);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->SetCaption(cCountVal);

	if(nGradeTotal)
		cCountVal.Format(_T("%d	(%.1f%%)"), nGradeNG, (float)nGradeNG / (float)nGradeTotal * 100);
	else
		cCountVal.Format(_T("%d (%d%%)"), nGradeNG, 0);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->SetCaption(cCountVal);

	cCountVal.Format(_T("%d"), nGradeTotal);
	((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_TOTALCELL))->SetCaption(cCountVal);
}
#

afx_msg void CDlgDefectInfo::OnBnClickedBtnPnlCntReset()
{
	/*if (AfxMessageBox(_T("Do you want Defect Information Reset? "), MB_OKCANCEL) != IDOK)
	return;*/

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	
	if (!m_bMaualFlg)
	{
		if(m_pDlgLogin)
		{
			delete m_pDlgLogin;
			m_pDlgLogin = NULL;
		}	
		m_pDlgLogin = new CLogin();			
	}
	if (m_bMaualFlg || m_pDlgLogin->DoModal() == IDOK)
	{
		pAuto->m_pDlgPanelJudge->ChangeResetTime();

		

		CString cCountRoot= _T("");
		cCountRoot.Append(ROOT_PATH);
		cCountRoot.Append(CONFIG_COUNT_PATH);

		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, _T("0"), cCountRoot);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, _T("0"), cCountRoot);

		((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_OK))->SetCaption(_T("0"));	
		((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_GRADE_NG))->SetCaption(_T("0"));	
		((CGxStaticEx*)GetDlgItem(IDC_GXSTATIC_TOTALCELL))->SetCaption(_T("0"));

		m_stTotalDefectType.initDefType();
		m_stTotalDefectType.WriteDefInfo();

		SetDlgDefTypeVal();
	}
	
}


BOOL CDlgDefectInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CRect LabelRect, WinRect;
	CPoint point;
	m_tip_ctrl.Create(this);
	m_tip_ctrl.SetMaxTipWidth(300);
	m_tip_ctrl.SetDelayTime(TTDT_AUTOPOP, 20000);


	GetDlgItem(IDC_LABEL14)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3383 : WANES BROKEN \n3378 : CF CORNER BROKEN \n3376 : CF BROKEN \n3377 : TFT BROKEN \n3379 : TFT CORNER BROKEN \n3372 : CF CELL BROKEN \n3373 : TFT CELL BROKEN  "), WinRect, 1);
	GetDlgItem(IDC_LABEL4)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3381 : CF WHITE BLUR \n3380 : TFT WHITE BLUR"), WinRect, 1);
	GetDlgItem(IDC_LABEL10)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3382 : ITOPEEL"), WinRect, 1);
	GetDlgItem(IDC_LABEL18)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3384 : SOCKET SCRACH"), WinRect, 1);




	GetDlgItem(IDC_LABEL22)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3386 : CF DIMPLE POINT \n3385 : TFT DIMPLE POINT"), WinRect, 1);
	GetDlgItem(IDC_LABEL26)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3388 : CF EDGE ILEGAL \n3387 : TFT EDGE ILEGAL"), WinRect, 1);
	GetDlgItem(IDC_LABEL30)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3390 : CF DIMPLE LINE \n3389 : TFT DIMPLE LINE"), WinRect, 1);
	GetDlgItem(IDC_LABEL34)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3391 : SOCKET BURN"), WinRect, 1);
	GetDlgItem(IDC_LABEL38)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3374 : CF BURR \n3375 : TFT BURR \n3392 : BURR"), WinRect, 1);

	GetDlgItem(IDC_LABEL19)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3393 : CF SCRATCH \n3394 : TFT SCRATCH"), WinRect, 1);
	GetDlgItem(IDC_LABEL27)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3395 : CF MEASURE FAIL"), WinRect, 1);


	GetDlgItem(IDC_LABEL27)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3395 : CF MEASURE FAIL"), WinRect, 1);
	GetDlgItem(IDC_LABEL31)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3396 : TFT MEASURE FAIL"), WinRect, 1);
	GetDlgItem(IDC_LABEL35)->GetWindowRect(WinRect);	ScreenToClient(&WinRect);
	m_tip_ctrl.AddTool(this, _T("3791 : MURA"), WinRect, 1);

	m_tip_ctrl.Activate(TRUE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgDefectInfo::m_fnChangeFont(CString strDef)
{
	
	CFont boldFont, *oldFont;
	LOGFONT lf;
	oldFont = GetFont();
	oldFont->GetObject( sizeof(LOGFONT), &lf);
	m_strDef = strDef;
	int nDef;
	//boldFont.CreatePointFont(80,_T("Fixedsys"));
	
	lf.lfWeight = 700;
	boldFont.CreateFontIndirect( &lf ); 

	GetDlgItem(IDC_LABEL14)->SetFont(oldFont); GetDlgItem(IDC_LABEL4)->SetFont(oldFont); GetDlgItem(IDC_LABEL10)->SetFont(oldFont);
	GetDlgItem(IDC_LABEL19)->SetFont(oldFont); GetDlgItem(IDC_LABEL18)->SetFont(oldFont); GetDlgItem(IDC_LABEL22)->SetFont(oldFont);
	GetDlgItem(IDC_LABEL26)->SetFont(oldFont); GetDlgItem(IDC_LABEL30)->SetFont(oldFont);  GetDlgItem(IDC_LABEL27)->SetFont(oldFont);
	GetDlgItem(IDC_LABEL34)->SetFont(oldFont); GetDlgItem(IDC_LABEL38)->SetFont(oldFont);  GetDlgItem(IDC_LABEL31)->SetFont(oldFont);
	nDef = _ttoi(strDef);
	switch(nDef)
	{
	case WANESBROKEN: case CFCORNERBROKEN: case CFBROKEN: case TFTBROKEN: case TFTCORNERBROKEN:
	case CFCELLBROKEN: case TFTCELLBROKEN:
		GetDlgItem(IDC_LABEL14)->SetFont(&boldFont);
		break;
	case CFWHITEBLUR: case TFTWHITEBLUR:
		GetDlgItem(IDC_LABEL4)->SetFont(&boldFont);
		break;
	case ITOPEEL:
		GetDlgItem(IDC_LABEL10)->SetFont(&boldFont);
		break;
	case SOCKETSCRACH:
		GetDlgItem(IDC_LABEL18)->SetFont(&boldFont);
		break;
	case CFDIMPLEPOINT: case TFTDIMPLEPOINT:
		GetDlgItem(IDC_LABEL22)->SetFont(&boldFont);
		break;
	case CFEDGEILEGAL: case TFTEDGEILEGAL:
		GetDlgItem(IDC_LABEL26)->SetFont(&boldFont);
		break;
	case CFDIMPLELINE: case TFTDIMPLELINE:
		GetDlgItem(IDC_LABEL30)->SetFont(&boldFont);
		break;
	case SOCKETBURN:
		GetDlgItem(IDC_LABEL34)->SetFont(&boldFont);
		break;
	case CFBURR: case TFTBURR: case BURR:
		GetDlgItem(IDC_LABEL38)->SetFont(&boldFont);
		break;
	case CFPOLBUBBLE: case TFTPOLBUBBLE:
		GetDlgItem(IDC_LABEL3)->SetFont(&boldFont);
		break;
	case CFPOLSCRATCH: case TFTPOLSCRATCH:
		GetDlgItem(IDC_LABEL5)->SetFont(&boldFont);
		break;
	case CFPOLSUBSTANCE: case TFTPOLSUBSTANCE:
		GetDlgItem(IDC_LABEL11)->SetFont(&boldFont);
		break;
	case POLMINOR:
		GetDlgItem(IDC_LABEL15)->SetFont(&boldFont);
		break;
	case CFSCRATCH: case TFTSCRATCH:
		GetDlgItem(IDC_LABEL19)->SetFont(&boldFont);
		break;
	case CFMEASUREFAIL:
		GetDlgItem(IDC_LABEL27)->SetFont(&boldFont);
		break;
	case TFTMEASUREFAIL:
		GetDlgItem(IDC_LABEL31)->SetFont(&boldFont);
		break;
	case MURA:
		GetDlgItem(IDC_LABEL35)->SetFont(&boldFont);
		break;
	}
	
}


BOOL CDlgDefectInfo::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	m_tip_ctrl.RelayEvent(pMsg);

	return CDialogEx::PreTranslateMessage(pMsg);
}
