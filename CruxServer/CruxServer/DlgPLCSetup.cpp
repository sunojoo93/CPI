
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgPLCSetup.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgPLCSetup.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ActUtlType_i.c"
// CDlgPLCSetup 대화 상자입니다.
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

//2017.07.31		INSP_ETC_POINT ~ INSP_CF_PROTECTION 사이의 배열을 건너 뛰기 위함
typedef enum e_PARAM_ADDITION_OVERLAP{
	CNT_OVERLAP_CF_PROTECTION		= INSP_ETC_POINT + 1	,
	CNT_OVERLAP_TFT_PROTECTION								,
	MAX_COUNT_OVERLAP
};

extern CInterServerInterface* G_pVsServerIF;

IMPLEMENT_DYNAMIC(CDlgPLCSetup, CDialogEx)

CDlgPLCSetup::CDlgPLCSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgPLCSetup::IDD, pParent)
{
	m_bOpenPLC = FALSE;
	m_nEqpModel = 0;
	//2015.06.02
	m_bUseOverlap = FALSE;
	m_bAllOverlapDef = FALSE; //2016.03.23 add by ikm
	m_dOffX = 0;
	m_dOffY = 0;
	m_bUseContactFail = FALSE;
	m_nContactFailCount = 0;
	//2016.02.24
	m_bUsePolMiss = FALSE;
	m_nPolMissCount = 0;
	//2016.08.22
	m_bUseAlignFail = FALSE;
	m_nAlignFailCount = 0;
	//2019.03.21
	m_bUseBgrade = FALSE;
	m_nBgradeSetCount = 0;
	m_bUseCgrade = FALSE;
	m_nCgradeSetCount = 0;

	//2016.02.24
	m_strRoot = _T("");
	m_strRoot.Format(_T("%s%s"), ROOT_PATH,CONFIG_PATH );
	m_nEqpObject = GetPrivateProfileInt(EQP_STATE, KEY_EQP_OBJECT, 1, m_strRoot);	
	if(m_nEqpObject == P_PROJECT) m_nCmdType = CMD_TYPE_RES;
	else						  m_nCmdType = CMD_TYPE_NORES;
	//2015.06.02
	//InitializeCriticalSection(&cs); 

	//Critical Section 초기화, Multi CPU 를 고려한 Spin Count 를 둔다.
	//::InitializeCriticalSectionAndSpinCount(&csMXComponent, 2000);

	//hMutexPlcDevice = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("CRUX Vision System - Control PLC Device")); 

	//뮤텍스 열고 없으면 생성
	//if(hMutexPlcDevice == NULL)	hMutexPlcDevice = ::CreateMutex(NULL, FALSE, _T("CRUX Vision System - Control PLC Device")); 

	//FtpDstClient = new CFtpClient();
	//FtpImgClient = new CFtpClient();
	m_nPanelCount = 0;
	m_nAlignFailCount = 0;

	

}

CDlgPLCSetup::~CDlgPLCSetup()
{
	
	//DeleteCriticalSection(&csMXComponent); 
	//CloseHandle(hMutexPlcDevice);
	m_bOpenPLC = FALSE;
}

void CDlgPLCSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
#if PLC_HANDLE_TYPE
	DDX_Control(pDX, IDC_ACTUTLTYPE1, m_ActUtlType);
#else
	//	DDX_Control(pDX, IDC_ACTUTLTYPE1,(CActutltype &) m_ActUtlType);
#endif

	DDX_Check(pDX, IDC_CHECK_OVERLAP_USE, m_bUseOverlap);	//2015.06.02
	DDX_Check(pDX, IDC_CHECK_ALL_OVERLAP_DEF, m_bAllOverlapDef);	//2016.03.23 add by ikm

	DDX_Text(pDX, IDC_EDIT_OVERLAP_POSITION_X, m_dOffX);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_POSITION_Y, m_dOffY);
	DDX_Check(pDX, IDC_CHECK_DEFECT1, m_nAryDefKind[INSP_DUST]);
	DDX_Check(pDX, IDC_CHECK_DEFECT2, m_nAryDefKind[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CHECK_DEFECT3, m_nAryDefKind[INSP_ITO]);
	DDX_Check(pDX, IDC_CHECK_DEFECT4, m_nAryDefKind[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT5, m_nAryDefKind[INSP_PAD_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT6, m_nAryDefKind[INSP_DIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT7, m_nAryDefKind[INSP_ETCHING_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT8, m_nAryDefKind[INSP_DIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT9, m_nAryDefKind[INSP_PAD_BURNT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT10, m_nAryDefKind[INSP_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT11, m_nAryDefKind[INSP_POL_BUBBLE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT12, m_nAryDefKind[INSP_POL_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT13, m_nAryDefKind[INSP_POL_ALIEN_SUBSTANCE ]);
	DDX_Check(pDX, IDC_CHECK_DEFECT14, m_nAryDefKind[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Check(pDX, IDC_CHECK_DEFECT15, m_nAryDefKind[INSP_BLACK_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT16, m_nAryDefKind[INSP_WHITE_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT17, m_nAryDefKind[INSP_CF_MEASURE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT18, m_nAryDefKind[INSP_TFT_MEASURE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT19, m_nAryDefKind[INSP_MURA]);
	DDX_Check(pDX, IDC_CHECK_DEFECT20, m_nAryDefKind[INSP_NO_USE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT21, m_nAryDefKind[INSP_CF_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT22, m_nAryDefKind[INSP_TFT_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT23, m_nAryDefKind[INSP_CF_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT24, m_nAryDefKind[INSP_TFT_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT25, m_nAryDefKind[INSP_CF_CORNER_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT26, m_nAryDefKind[INSP_TFT_CORNER_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT27, m_nAryDefKind[INSP_CF_CELL_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT28, m_nAryDefKind[INSP_TFT_CELL_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT29, m_nAryDefKind[INSP_PIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT30, m_nAryDefKind[INSP_PIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT31, m_nAryDefKind[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT32, m_nAryDefKind[INSP_CF_WAVE_PATTERN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT33, m_nAryDefKind[INSP_ETC_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT34, m_nAryDefKind[INSP_ETC_POINT]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT35, m_nAryDefKind[INSP_CF_PROTECTION]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT36, m_nAryDefKind[INSP_TFT_PROTECTION]);
	DDX_Check(pDX, IDC_CHECK_DEFECT35, m_nAryDefKind[CNT_OVERLAP_CF_PROTECTION]);
	DDX_Check(pDX, IDC_CHECK_DEFECT36, m_nAryDefKind[CNT_OVERLAP_TFT_PROTECTION]);


	DDX_Check(pDX, IDC_CHECK_CONTACT_FAIL, m_bUseContactFail);
	DDX_Check(pDX, IDC_CHECK_POL_MISS, m_bUsePolMiss);	//2016.02.24
	DDX_Check(pDX, IDC_CHECK_ALIGN_FAIL, m_bUseAlignFail);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT1, m_nAryDefCount[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT2, m_nAryDefCount[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT3, m_nAryDefCount[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT4, m_nAryDefCount[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT5, m_nAryDefCount[INSP_PAD_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT6, m_nAryDefCount[INSP_DIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT7, m_nAryDefCount[INSP_ETCHING_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT8, m_nAryDefCount[INSP_DIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT9, m_nAryDefCount[INSP_PAD_BURNT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT10, m_nAryDefCount[INSP_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT11, m_nAryDefCount[INSP_POL_BUBBLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT12, m_nAryDefCount[INSP_POL_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT13, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT14, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT15, m_nAryDefCount[INSP_BLACK_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT16, m_nAryDefCount[INSP_WHITE_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT17, m_nAryDefCount[INSP_CF_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT18, m_nAryDefCount[INSP_TFT_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT19, m_nAryDefCount[INSP_MURA]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT20, m_nAryDefCount[INSP_CONTACT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT21, m_nAryDefCount[INSP_CF_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT22, m_nAryDefCount[INSP_TFT_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT23, m_nAryDefCount[INSP_CF_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT24, m_nAryDefCount[INSP_TFT_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT25, m_nAryDefCount[INSP_CF_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT26, m_nAryDefCount[INSP_TFT_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT27, m_nAryDefCount[INSP_CF_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT28, m_nAryDefCount[INSP_TFT_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT29, m_nAryDefCount[INSP_PIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT30, m_nAryDefCount[INSP_PIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT31, m_nAryDefCount[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT32, m_nAryDefCount[INSP_CF_WAVE_PATTERN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT33, m_nAryDefCount[INSP_ETC_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT34, m_nAryDefCount[INSP_ETC_POINT]);
	//DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT35, m_nAryDefCount[INSP_CF_PROTECTION]);
	//DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT36, m_nAryDefCount[INSP_TFT_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT35, m_nAryDefCount[CNT_OVERLAP_CF_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT36, m_nAryDefCount[CNT_OVERLAP_TFT_PROTECTION]);


	DDX_Text(pDX, IDC_EDIT_CONTACT_FAIL_COUNT, m_nContactFailCount);
	DDX_Text(pDX, IDC_EDIT_POL_MISS_COUNT, m_nPolMissCount);	//2016.02.24
	DDX_Text(pDX, IDC_EDIT_POL_ALIGN_FAIL_COUNT, m_nAlignFailCount);

	DDX_Text(pDX, IDC_EDIT_PANEL_COUNT, m_nPanelCount);
	DDV_MinMaxInt(pDX, m_nPanelCount, 0, 1000);

	DDX_Check(pDX, IDC_CHECK_CFC_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_CBC_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_CBS_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_CFS_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_TBC_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_TBS_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_TDI_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_TDILUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_TFC_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_TFS_LUMINANCE, m_bArrLumiKind[LUMINANCE_TYPE_TFSLUMINANCE]);

	DDX_Text(pDX, IDC_EDIT_CBC_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_CBS_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_CFC_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_CFS_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_TBC_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_TBS_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_TDI_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_TDILUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_TFC_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_TFS_LUMINANCE_COUNT, m_nArrLumiCount[LUMINANCE_TYPE_TFSLUMINANCE]);
}
BEGIN_MESSAGE_MAP(CDlgPLCSetup, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_GETDEVICE, &CDlgPLCSetup::OnBnClickedBtnGetdevice)
	ON_BN_CLICKED(IDC_BTN_SETDEVICE, &CDlgPLCSetup::OnBnClickedBtnSetdevice)
	ON_BN_CLICKED(IDC_BTN_READBLOCK, &CDlgPLCSetup::OnBnClickedBtnReadblock)
	ON_BN_CLICKED(IDC_BTN_WRITEBLOCK, &CDlgPLCSetup::OnBnClickedBtnWriteblock)
	ON_BN_CLICKED(IDC_BTN_SET_PCALIVE, &CDlgPLCSetup::OnBnClickedBtnSetPcalive)
	ON_BN_CLICKED(IDC_BTN_SET_READY_S1, &CDlgPLCSetup::OnBnClickedBtnSetReadyS1)
	ON_BN_CLICKED(IDC_BTN_SET_READY_S2, &CDlgPLCSetup::OnBnClickedBtnSetReadyS2)
	ON_BN_CLICKED(IDC_BTN_SET_READY_S3, &CDlgPLCSetup::OnBnClickedBtnSetReadyS3)
	ON_BN_CLICKED(IDC_BTN_SET_FORWARD_S1, &CDlgPLCSetup::OnBnClickedBtnSetForwardS1)
	ON_BN_CLICKED(IDC_BTN_SET_FORWARD_S2, &CDlgPLCSetup::OnBnClickedBtnSetForwardS2)
	ON_BN_CLICKED(IDC_BTN_SET_FORWARD_S3, &CDlgPLCSetup::OnBnClickedBtnSetForwardS3)
	ON_BN_CLICKED(IDC_BTN_SET_ACK_S1, &CDlgPLCSetup::OnBnClickedBtnSetAckS1)
	ON_BN_CLICKED(IDC_BTN_SET_ACK_S2, &CDlgPLCSetup::OnBnClickedBtnSetAckS2)
	ON_BN_CLICKED(IDC_BTN_SET_ACK_S3, &CDlgPLCSetup::OnBnClickedBtnSetAckS3)
	ON_BN_CLICKED(IDC_BTN_SET_END_S1, &CDlgPLCSetup::OnBnClickedBtnSetEndS1)
	ON_BN_CLICKED(IDC_BTN_SET_END_S2, &CDlgPLCSetup::OnBnClickedBtnSetEndS2)
	ON_BN_CLICKED(IDC_BTN_SET_END_S3, &CDlgPLCSetup::OnBnClickedBtnSetEndS3)
	ON_BN_CLICKED(IDC_BTN_SET_ERROR_S1, &CDlgPLCSetup::OnBnClickedBtnSetErrorS1)
	ON_BN_CLICKED(IDC_BTN_SET_ERROR_S2, &CDlgPLCSetup::OnBnClickedBtnSetErrorS2)
	ON_BN_CLICKED(IDC_BTN_SET_ERROR_S3, &CDlgPLCSetup::OnBnClickedBtnSetErrorS3)
	ON_BN_CLICKED(IDC_BTN_SET_PPID_MIS1, &CDlgPLCSetup::OnBnClickedBtnSetPpidMis1)
	ON_BN_CLICKED(IDC_BTN_SET_PPID_MIS2, &CDlgPLCSetup::OnBnClickedBtnSetPpidMis2)
	ON_BN_CLICKED(IDC_BTN_SET_PPID_MIS3, &CDlgPLCSetup::OnBnClickedBtnSetPpidMis3)
	ON_BN_CLICKED(IDC_BTN_SET_RESET_S1, &CDlgPLCSetup::OnBnClickedBtnSetResetS1)
	ON_BN_CLICKED(IDC_BTN_SET_RESET_S2, &CDlgPLCSetup::OnBnClickedBtnSetResetS2)
	ON_BN_CLICKED(IDC_BTN_SET_RESET_S3, &CDlgPLCSetup::OnBnClickedBtnSetResetS3)
	ON_BN_CLICKED(IDC_BTN_SET_PCALARM_S1, &CDlgPLCSetup::OnBnClickedBtnSetPcalarmS1)
	ON_BN_CLICKED(IDC_BTN_SET_PCALARM_S2, &CDlgPLCSetup::OnBnClickedBtnSetPcalarmS2)
	ON_BN_CLICKED(IDC_BTN_SET_PCALARM_S3, &CDlgPLCSetup::OnBnClickedBtnSetPcalarmS3)
	ON_BN_CLICKED(IDC_BTN_SET_PCALIVE2, &CDlgPLCSetup::OnBnClickedBtnSetPcalive2)
	ON_BN_CLICKED(IDC_BTN_SET_PCALIVE3, &CDlgPLCSetup::OnBnClickedBtnSetPcalive3)
	ON_BN_CLICKED(IDC_BTN_SET_CYCLE_S1, &CDlgPLCSetup::OnBnClickedBtnSetCycleS1)
	ON_BN_CLICKED(IDC_BTN_SET_CYCLE_S2, &CDlgPLCSetup::OnBnClickedBtnSetCycleS2)
	ON_BN_CLICKED(IDC_BTN_SET_CYCLE_S3, &CDlgPLCSetup::OnBnClickedBtnSetCycleS3)

	ON_BN_CLICKED(IDC_BUTTON2, &CDlgPLCSetup::OnBnClickedButton2)

	ON_BN_CLICKED(IDC_BTN_RCP_EDIT, &CDlgPLCSetup::OnBnClickedBtnRcpEdit)
	ON_BN_CLICKED(IDC_BTN_SET_RCP, &CDlgPLCSetup::OnBnClickedBtnSetRcp)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_RCP_UP, &CDlgPLCSetup::OnBnClickedBtnRcpUp)
	ON_BN_CLICKED(IDC_BTN_RCP_DOWN, &CDlgPLCSetup::OnBnClickedBtnRcpDown)
	ON_BN_CLICKED(IDC_BTN_PROC_INFO, &CDlgPLCSetup::OnBnClickedBtnProcInfo)
	ON_BN_CLICKED(IDC_BTN_PROC_DATA, &CDlgPLCSetup::OnBnClickedBtnProcData)
	ON_BN_CLICKED(IDC_BTN_RCP_CHG, &CDlgPLCSetup::OnBnClickedBtnRcpChg)
	ON_BN_CLICKED(IDC_CHECK_OVERLAP_USE, &CDlgPLCSetup::OnBnClickedCheckOverlapUse)	//2015.06.02
	ON_BN_CLICKED(IDC_CHECK_ALL_OVERLAP_DEF, &CDlgPLCSetup::OnBnClickedCheckAllOverlapDef)	
END_MESSAGE_MAP()


// CDlgPLCSetup 메시지 처리기입니다.



BOOL CDlgPLCSetup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(FALSE);
		
	////..TEST  2015.02.03 add by ikm for FTP   - S	
	//m_strFileName = "U_KCS01_0001_20090701134555";
	////m_strFileName += EXTENSION_TMP;	
	//GetDlgItem(IDC_EDIT1)->SetWindowTextW(m_strFileName);	
	////..TEST 2015.02.03 add by ikm for FTP - E

	/*SetTimer(1000, 1000, NULL);
	SetTimer(1001, 1000, NULL);
	SetTimer(1002, 1000, NULL);
	SetTimer(1003, 1000, NULL);
	SetTimer(1004, 1000, NULL);
	SetTimer(1005, 1000, NULL);*/
#if PLC_HANDLE_TYPE
	
#else
	HRESULT hr = CoCreateInstance(CLSID_ActUtlType,	NULL, CLSCTX_INPROC_SERVER,	
									IID_IActUtlType,(LPVOID*)&m_ActUtlType);
	if(!SUCCEEDED(hr)){
		AfxMessageBox(_T("CoCrateInstance() Failed."));
		exit(0);
	}
#endif
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BEGIN_EVENTSINK_MAP(CDlgPLCSetup, CDialogEx)
	ON_EVENT(CDlgPLCSetup, IDC_GXBTN_PLC_OPEN, DISPID_CLICK, CDlgPLCSetup::ClickGxbtnPlcOpen, VTS_NONE)
	ON_EVENT(CDlgPLCSetup, IDC_GXBTN_PLC_CLOSE, DISPID_CLICK, CDlgPLCSetup::ClickGxbtnPlcClose, VTS_NONE)
END_EVENTSINK_MAP()


void CDlgPLCSetup::ClickGxbtnPlcOpen()
{
	CString strRet;
	long lRet = m_fnOpenPLC();

	if(lRet != 0)		// Open Error
	{
		strRet.Format(_T("0x%08x"),lRet);
		AfxMessageBox(strRet);

		return;
	}

	GetDlgItem(IDC_GXBTN_PLC_OPEN)->EnableWindow(FALSE);
	GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(TRUE);
}

void CDlgPLCSetup::ClickGxbtnPlcClose()
{
	CString strRet;
	long lRet = m_fnClosePLC();

	if(lRet != 0)		// Open Error
	{
		strRet.Format(_T("0x%08x"),lRet);
		AfxMessageBox(strRet);

		return;
	}

	GetDlgItem(IDC_GXBTN_PLC_OPEN)->EnableWindow(TRUE);
	GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(FALSE);
}

long CDlgPLCSetup::m_fnOpenPLC()
{
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	try{
		//EnterCriticalSection(&csMXComponent);
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		//Sleep(30);
		bIsEnterCS = true;
		// If you don't use default values, please set their properties before OPEN method call.
		// ---> Example: Change the Logical station number to "1" from default value.
#if PLC_HANDLE_TYPE
		//m_ActUtlType.put_ActLogicalStationNumber(1);	// Exec set-property method 
		//lRet = m_ActUtlType.Open();	// Open method exec
#else
		HRESULT	hr;
		BSTR szAdr = NULL;
		CString	MsgStr;
		
		hr = m_ActUtlType->put_ActLogicalStationNumber(1);	// Exec set-property method
		if(SUCCEEDED(hr)){	// Compornent Communication is succeeded?
			hr = m_ActUtlType->Open(&lRet);	// Exec Open Method
		}
#endif
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 
		//LeaveCriticalSection(&csMXComponent);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 
		AfxMessageBox(_T("PLC Open Error"), MB_ICONINFORMATION);
		Exception->Delete();
	}
	lRet = APP_OK;
	if(lRet == 0)	m_bOpenPLC = TRUE;
	return lRet;
}

long CDlgPLCSetup::m_fnClosePLC()
{
	m_bOpenPLC = false;

	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	try{
		//EnterCriticalSection(&csMXComponent);
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);

		bIsEnterCS = true;
#if PLC_HANDLE_TYPE
		//if (m_ActUtlType.m_hWnd != NULL)
			//lRet = m_ActUtlType.Close();	// Exec Close Method
#else
		HRESULT	hr;
		// ActUtlType Control (Custom Interface)
		hr = m_ActUtlType->Close(&lRet);	// Exec Close Method
#endif
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 
		////LeaveCriticalSection(&csMXComponent);
	}
	catch(COleDispatchException *Exception){
		////if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 
		// OLE IDispatch Interface Error
		AfxMessageBox(_T("PLC Close Error"), MB_ICONINFORMATION);
		Exception->Delete();
	}

	return lRet;
}



short CDlgPLCSetup::m_fnGetDevice2(TCHAR *  strAddr)
{
	int	nRet = APP_OK;
	short shRet = -1;
	short shValue = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함
	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return shRet;
	if (cTemp == "") 	return shRet;

	try{
		
		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//shRet = (short)m_ActUtlType.GetDevice2(strAddr, &shValue);// Exec GetDevice Method

		nRet = m_fnGetDeviceFromPlcTask(strAddr, &shValue);
		if (nRet != APP_OK)
		{
			//Write Log
			shValue = 0;
		}
#else
		HRESULT	hr;
		LONG lRet = -1;
		BSTR szDev = NULL;
		CString ttmp = strAddr;
		szDev = ttmp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->GetDevice2(szDev, &shValue,&lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);

#endif		
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//m_strRetCode.Format(_T("0x%08x"),shRet);
	//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);

		//		if(lRet == 0x00) // Success
		shRet = shValue;
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return shRet;
}

long CDlgPLCSetup::m_fnGetDevice(TCHAR * strAddr)
{
	int nRet = APP_OK;
	int nchk = 0;
	long lRet = -1;
	long lValue = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함'
	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{		

		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.GetDevice(/*(LPCTSTR)*/strAddr, &lValue);// Exec GetDevice Method
		//2018.12.17 add by mby
		for(nchk=0; nchk < 3; nchk++){
			nRet = m_fnGetDeviceDwordFromPlcTask(strAddr, &lValue);
			if (lRet == APP_OK) break;
			//G_AddLog(3, 0, _T("m_fnGetDeviceDwordFromPlcTask  strAddr=%s"),strAddr);
			Sleep(100);
		}//2018.12.17 add by mby
		if (nRet != APP_OK)
		{
			m_fnSetDevice(ADDR_PROCESS_CIM_NETWORK_FAIL, 1);  //2019.06.12 by ryu
			//Write Log
			G_AddLog(3, 0, _T("m_fnGetDeviceDwordFromPlcTask  strAddr=%s"),strAddr);
			lValue = 0;
		}
#else
		HRESULT	hr;
		BSTR szDev = NULL;
	//	tctemp = strAddr;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->GetDevice(szDev, &lValue,&lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);
#endif
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

	//	m_strRetCode.Format(_T("0x%08x"),lRet);
	//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);

//		if(lRet == 0x00) // Success
		lRet = lValue;
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return lRet;
}

long CDlgPLCSetup::m_fnGetDeviceModel(TCHAR * strAddr)
{
	int nRet = APP_OK;
	long lRet = -1;
	long lValue = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함'
	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{		

		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.GetDevice(/*(LPCTSTR)*/strAddr, &lValue);// Exec GetDevice Method

		nRet = m_fnGetDeviceDwordFromPlcTaskModel(strAddr, &lValue);
		if (nRet != APP_OK)
		{
			//Write Log
			lValue = 0;
		}
#else
		HRESULT	hr;
		BSTR szDev = NULL;
		//	tctemp = strAddr;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->GetDevice(szDev, &lValue,&lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);
#endif
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);

		//		if(lRet == 0x00) // Success
		lRet = lValue;
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return lRet;
}

long CDlgPLCSetup::m_fnGetDeviceBlock(TCHAR *  strAddr, int nSize, CString& strRet)
{
	int nRet = APP_OK;
	long lRet = -1;
	long lValue[50] = {0,};
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함
	
	byte* pGetData = NULL;

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;

	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->GetMainWnd();
	//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	//	nSize *= 2;
	try{
		
		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.ReadDeviceBlock(strAddr, nSize, lValue);// Exec ReadDeviceBlock Method

		nRet = m_fnGetDeviceBlockDwordFromPlcTask(strAddr, nSize, lValue);
		if (nRet != APP_OK)
		{
			//Write Log
			
		}
		lRet = nRet;
#else
		HRESULT	hr;		
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->ReadDeviceBlock(szDev, nSize, lValue, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);
		
#endif		
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

	//	m_strRetCode.Format(_T("0x%08x"),lRet);
	//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
		//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
		//	nSize /= 2;
		if(lRet == 0x00){	// Success
			char chValue[100];
			memset(chValue, 0x00, 100);
			int nn = 0;
			for (int i = 0; i < (nSize / 2); i++)
			{
				chValue[nn++] = lValue[i] & 0x00FF;
				chValue[nn++] = (lValue[i]) >> 8 & 0x00FF;
			}
			strRet = chValue;
		}
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}
	
	return nRet;
}

long CDlgPLCSetup::m_fnGetDeviceBlockModel(TCHAR *  strAddr, int nSize, CString& strRet)
{
	int nRet = APP_OK;
	long lRet = -1;
	long lValue[40000] = {0,};	//2016.12.30 50->40000
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	byte* pGetData = NULL;

	CString cTemp = strAddr;
	CString strData = _T(""), strTemp = _T("");

	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;

	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->GetMainWnd();
	//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	//	nSize *= 2;
	try{

		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.ReadDeviceBlock(strAddr, nSize, lValue);// Exec ReadDeviceBlock Method

		nRet = m_fnGetDeviceBlockDwordFromPlcTask(strAddr, nSize, lValue);
		if (nRet != APP_OK)
		{
			//Write Log

		}
		lRet = nRet;
#else
		HRESULT	hr;		
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->ReadDeviceBlock(szDev, nSize, lValue, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);

#endif		
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
		//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
		//	nSize /= 2;
		if(lRet == 0x00){	// Success
			//char chValue[10000];
			//memset(chValue, 0x00, 10000);
			int nn = 0;
			for (int i = 0; i < nSize / 4 ; i++)
			{
				//chValue[nn++] = lValue[i] & 0x00FF;
				//chValue[nn++] = (lValue[i]) >> 8 & 0x00FF;
				strTemp.Format(_T("%ld,"),lValue[i]);
				strData.Append(strTemp);
			}
			strRet = strData;
		}
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return nRet;
}

long CDlgPLCSetup::m_fnSetDevice(TCHAR *  strAddr, long lVal)
{
	int nRet = APP_OK;
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{
		
		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;		
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.SetDevice(strAddr, lVal);	// Exec SetDevice Method

		nRet = m_fnSetDeviceDwordFromPlcTask(strAddr, lVal);
		if (nRet != APP_OK)
		{
			//Write Log

		}
#else
		HRESULT	hr;
		LONG LVal = (LONG)lVal;
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area.  
		hr = m_ActUtlType->SetDevice(szDev, LVal, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		
#endif			
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}
	
	return nRet;
}

long CDlgPLCSetup::m_fnSetDeviceModel(TCHAR * strAddr, long lVal)
{
	int nRet = APP_OK;
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;

	try{
		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;		
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.SetDevice(strAddr, lVal);	// Exec SetDevice Method

		nRet = m_fnSetDeviceDwordFromPlcTaskModel(strAddr, lVal);
		if (nRet != APP_OK)
		{
			//Write Log
		}
#else
		HRESULT	hr;
		LONG LVal = (LONG)lVal;
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area.  
		hr = m_ActUtlType->SetDevice(szDev, LVal, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		
#endif	

		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 
		Exception->Delete();
	}
	return nRet;
}


long CDlgPLCSetup::m_fnSetDeviceAutoClear(TCHAR *  strAddr, long lVal, ULONG lWaitTime)
{
	int nRet = APP_OK;
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{

		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;		
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.SetDevice(strAddr, lVal);	// Exec SetDevice Method

		nRet = m_fnSetDeviceDwordAutoClearFromPlcTask(strAddr, lVal, lWaitTime);
		if (nRet != APP_OK)
		{
			//Write Log

		}
#else
		HRESULT	hr;
		LONG LVal = (LONG)lVal;
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area.  
		hr = m_ActUtlType->SetDevice(szDev, LVal, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		
#endif	

		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return nRet;
}

long CDlgPLCSetup::m_fnSetDevice2(TCHAR *  strAddr, short shVal)
{
	int nRet = APP_OK;
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{
		
		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;		
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.SetDevice2(strAddr, shVal);	// Exec SetDevice Method

		nRet = m_fnSetDeviceFromPlcTask(strAddr, shVal);
		if (nRet != APP_OK)
		{
			//Write Log

		}
#else
		HRESULT	hr;		
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->SetDevice2(szDev, shVal, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		
		
#endif			
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

	//	m_strRetCode.Format(_T("0x%08x"),lRet);
	//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}
	
	return nRet;
}

long CDlgPLCSetup::m_fnSetDevice2AutoClear(TCHAR *  strAddr, short shVal, ULONG lWaitTime)
{
	int nRet = APP_OK;
	long lRet = -1;
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;


	try{

		//EnterCriticalSection(&csMXComponent); 
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;		
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.SetDevice2(strAddr, shVal);	// Exec SetDevice Method

		nRet = m_fnSetDeviceAutoClearFromPlcTask(strAddr, shVal, lWaitTime);
		if (nRet != APP_OK)
		{
			//Write Log

		}
#else
		HRESULT	hr;		
		BSTR szDev = NULL;
		CString ttemp = strAddr;
		szDev = ttemp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->SetDevice2(szDev, shVal, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		

#endif			
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

		//	m_strRetCode.Format(_T("0x%08x"),lRet);
		//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return nRet;
}

long CDlgPLCSetup::m_fnSetDeviceBlock(TCHAR *  strAddr, int nSize, CString strVal)
{
	int	nRet = APP_OK;
	long lRet = -1;
	long lValue[100] = {0,};
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함

	CString cTemp = strAddr; 
	if (!m_bOpenPLC)	return lRet;
	if (cTemp == "") 	return lRet;

	int nLen = strVal.GetLength();

	if (nLen < nSize * 2)
	{
		for(int k = 0; k < (nSize * 2 - nLen); k++)
			strVal += " ";
	}

	try{
		for(int i = 0; i < nSize; i++)
			lValue[i] = strVal.GetAt(i * 2 + 1) << 8 | strVal.GetAt(i * 2);
		

		//EnterCriticalSection(&csMXComponent);
		//WaitForSingleObject(hMutexPlcDevice, INFINITE);
		bIsEnterCS = true;
		//Sleep(30);
#if PLC_HANDLE_TYPE
		//lRet = m_ActUtlType.WriteDeviceBlock(strAddr, nSize, lValue);// Exec ReadDeviceBlock Method

		nRet = m_fnSetDeviceBlockDwordFromPlcTask(strAddr, nSize, lValue);
		if (nRet != APP_OK)
		{
			//Write Log

		}
#else
		HRESULT	hr;		
		BSTR szDev = NULL;
		CString ttmp = strAddr;
		szDev = ttmp.AllocSysString();	// Allocate the BSTR-Type String area. 
		hr = m_ActUtlType->WriteDeviceBlock(szDev , nSize, lValue, &lRet);	// Exec GetDevice Method
		// Free the allocated area.
		//::SysFreeString(szDev);		
		
#endif			
		//LeaveCriticalSection(&csMXComponent);
		//if( hMutexPlcDevice ) ::ReleaseMutex(hMutexPlcDevice); 

	//	m_strRetCode.Format(_T("0x%08x"),lRet);
	//	GetDlgItem(IDC_EDIT_RETURN_CODE)->SetWindowText(m_strRetCode);
	} 
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&csMXComponent);
		//if( bIsEnterCS ) ::ReleaseMutex(hMutexPlcDevice); 

		Exception->Delete();
	}

	return nRet;
}

void CDlgPLCSetup::OnBnClickedBtnGetdevice()
{
	long lRet = -1;
	CString strAddr, strTemp;
	
	GetDlgItem(IDC_EDIT_DEVICE_NAME)->GetWindowText(strAddr);
	
	LPCTSTR xcttemp=  strAddr;
	
	lRet = m_fnGetDevice((TCHAR *)xcttemp);

	strTemp.Format(_T("%d"), lRet);

	GetDlgItem(IDC_EDIT_DEVICE_VALUE)->SetWindowText(strTemp);
}

void CDlgPLCSetup::OnBnClickedBtnSetdevice()
{
	long lRet = -1;
	long lValue = -1;
	CString strAddr, strValue;

	GetDlgItem(IDC_EDIT_DEVICE_NAME)->GetWindowText(strAddr);
	GetDlgItem(IDC_EDIT_DEVICE_VALUE)->GetWindowText(strValue);

	lValue = _ttol(strValue);
	LPCTSTR xcttemp=  strAddr;
	
	lRet = m_fnSetDevice((TCHAR *)xcttemp, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnReadblock()
{
	long lRet = -1;
	CString strAddr, strRet, strSize;

	GetDlgItem(IDC_EDIT_DEVICE_NAME)->GetWindowText(strAddr);
	GetDlgItem(IDC_EDIT_DEVICE_VAL_SIZE)->GetWindowText(strSize);

	int nSize = _ttoi(strSize);
	lRet = m_fnGetDeviceBlock((TCHAR *)(LPCTSTR) strAddr, nSize, strRet);

	GetDlgItem(IDC_EDIT_DEVICE_VAL_BLOCK)->SetWindowText(strRet);
}

void CDlgPLCSetup::OnBnClickedBtnWriteblock()
{
	long lRet = -1;
	CString strAddr, strSize, strValue;

	GetDlgItem(IDC_EDIT_DEVICE_NAME)->GetWindowText(strAddr);
	GetDlgItem(IDC_EDIT_DEVICE_VAL_SIZE)->GetWindowText(strSize);
	GetDlgItem(IDC_EDIT_DEVICE_VAL_BLOCK)->GetWindowText(strValue);

	int nSize = _ttoi(strSize);
	lRet = m_fnSetDeviceBlock((TCHAR *)(LPCTSTR)strAddr, nSize, strValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPcalive()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_ALIVE, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPcalive2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_ALIVE, lValue);
}


void CDlgPLCSetup::OnBnClickedBtnSetPcalive3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_ALIVE, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPcalarmS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_ALARM, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPcalarmS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_ALARM, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPcalarmS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_ALARM, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetReadyS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_READY, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetReadyS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_READY, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetReadyS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_READY, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetForwardS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_FORWARD, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetForwardS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_FORWARD, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetForwardS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_FORWARD, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetAckS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetAckS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetAckS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetEndS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_END, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetEndS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_END, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetEndS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_END, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetErrorS1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_ERROR, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetErrorS2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_ERROR, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetErrorS3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_ERROR, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPpidMis1()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S1_PPIDMISMATCH, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPpidMis2()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S2_PPIDMISMATCH, lValue);
}

void CDlgPLCSetup::OnBnClickedBtnSetPpidMis3()
{
	long lRet = -1;
	long lValue = -1;
	CString strValue;

	GetDlgItem(IDC_EDIT_SET_VALUE1)->GetWindowText(strValue);

	lValue = _ttol(strValue);

	lRet = m_fnSetDevice(ADDR_PC_S3_PPIDMISMATCH, lValue);
}


void CDlgPLCSetup::OnBnClickedBtnSetResetS1()
{
	long lRet = -1;

	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_FORWARD, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_GRAB_ERROR, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_PPIDMISMATCH, 0);
	lRet = m_fnSetDevice(ADDR_PC_S1_SEQ_ERROR, 0);
}


void CDlgPLCSetup::OnBnClickedBtnSetResetS2()
{
	long lRet = -1;

	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_FORWARD, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_END, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_GRAB_ERROR, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_PPIDMISMATCH, 0);
	lRet = m_fnSetDevice(ADDR_PC_S2_SEQ_ERROR, 0);
}


void CDlgPLCSetup::OnBnClickedBtnSetResetS3()
{
	long lRet = -1;

	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_FORWARD, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_END, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_GRAB_ERROR, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_PPIDMISMATCH, 0);
	lRet = m_fnSetDevice(ADDR_PC_S3_SEQ_ERROR, 0);
}

void CDlgPLCSetup::OnBnClickedBtnSetCycleS1()
{
	long lRet = -1;

	long shVal = m_fnGetDevice(ADDR_PC_S1_CYCLE);

	if(shVal == 0)
	{
		lRet = m_fnSetDevice(ADDR_PC_S1_CYCLE, 1);
		GetDlgItem(IDC_BTN_SET_CYCLE_S1)->SetWindowTextW(_T("Cycle S1 Off"));
	}
	else
	{
		lRet = m_fnSetDevice(ADDR_PC_S1_CYCLE, 0);
		GetDlgItem(IDC_BTN_SET_CYCLE_S1)->SetWindowTextW(_T("Cycle S1 On"));
	}
}

void CDlgPLCSetup::OnBnClickedBtnSetCycleS2()
{
	long lRet = -1;

	long shVal = m_fnGetDevice(ADDR_PC_S2_CYCLE);

	if(shVal == 0)
	{
		lRet = m_fnSetDevice(ADDR_PC_S2_CYCLE, 1);
		GetDlgItem(IDC_BTN_SET_CYCLE_S2)->SetWindowTextW(_T("Cycle S2 Off"));
	}
	else
	{
		lRet = m_fnSetDevice(ADDR_PC_S2_CYCLE, 0);
		GetDlgItem(IDC_BTN_SET_CYCLE_S2)->SetWindowTextW(_T("Cycle S2 On"));
	}
}

void CDlgPLCSetup::OnBnClickedBtnSetCycleS3()
{
	long lRet = -1;

	long shVal = m_fnGetDevice(ADDR_PC_S3_CYCLE);

	if(shVal == 0)
	{
		lRet = m_fnSetDevice(ADDR_PC_S3_CYCLE, 1);
		GetDlgItem(IDC_BTN_SET_CYCLE_S3)->SetWindowTextW(_T("Cycle S3 Off"));
	}
	else
	{
		lRet = m_fnSetDevice(ADDR_PC_S3_CYCLE, 0);
		GetDlgItem(IDC_BTN_SET_CYCLE_S3)->SetWindowTextW(_T("Cycle S3 On"));
	}
}


void CDlgPLCSetup::OnBnClickedButton2()
{
	
	//..TEST 2015.02.03 add by ikm for FTP - S	
	BOOL bTemp = false;
	int icheck=0;
	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());	//(CMainFrame*)AfxGetApp()->GetMainWnd();			
		

	CString strFileName;	

	/// 테스트용 결함 파일 생성 합니다. 텍스트 창에 바른 형식의 파일 명을 입력하십시오. 확장자는 입력하지 않습니다. - E
//	pFrame->m_DataManager.m_fnUpLoadImgFile(_T("PNL000001"), m_stPcClassifyEndFlag[nVisionCount].nTotalDefCnt);

	//CFtpClient FtpImgClient;
	//CFtpClient *FtpDstClient = new CFtpClient();
	//if (FtpDstClient == NULL)
	//{
	//	FtpDstClient = new CFtpClient();
	//}

	//if (FtpImgClient == NULL)
	//{
	//	FtpImgClient = new CFtpClient();
	//}
	//FtpDstClient->m_fnFTPdUploadData();
	//FtpImgClient->m_fnFTPdUploadImg();
	//rc =FtpDstClient->m_fnFTPdUploadImg();
	//rc = pFrame->m_FtpClient.m_fnFTPdUploadData();
	//..TEST 2015.02.03 add by ikm for FTP - E

	//pFrame->m_DataManager.m_fnMakeQmsData_AVL(_T(""),_T(""),bTemp);
	/// 테스트용 결함 파일 생성 합니다. 텍스트 창에 바른 형식의 파일 명을 입력하십시오. - S
	//strFileName = "U_KCS01_0001_20090701134555";
	//strFileName += EXTENSION_TMP;
	GetDlgItem(IDC_EDIT1)->GetWindowText(strFileName);
	//strFileName.MakeUpper();
	icheck = strFileName.Find(_T(".TMP")) ;
	if(icheck == -1)	strFileName += EXTENSION_TMP;
	
	icheck = strFileName.Find(_T(".tmp")) ;
	if(icheck == -1)	strFileName += EXTENSION_TMP;
	
	CFile file;
	if(!file.Open(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strFileName, CFile::modeRead))
	{
		file.Open(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait  + strFileName, CFile::modeCreate|CFile::modeWrite);
		file.Write(_T("OK"), 4);
	}	
	file.Close();

}

void CDlgPLCSetup::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//CDialogEx::OnOK();
}

void CDlgPLCSetup::OnBnClickedBtnSetRcp()
{
	long lRet = -1;
	CString strRecipeNo, strFSide, strFCoax;
	long lFSide, lFCoax, lRSide, lRCoax, lTrans1, lTrans2, lPADPos, lROICount;
	GetDlgItem(IDC_EDIT_RCP_NO)->GetWindowText(strRecipeNo);
	GetDlgItem(IDC_EDIT_F_SIDE)->GetWindowText(strFSide);
	GetDlgItem(IDC_EDIT_F_COAX)->GetWindowText(strFCoax);


	lFSide = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_0);
	lFCoax = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_1);
	lRSide = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_2);
	lRCoax = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_3);
	lTrans1 =m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_4);
	lTrans2 = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_5);
	lPADPos = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_6);
	lROICount =m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_7);


	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_CMD_NO, 101);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_RCP_NO, _ttoi(strRecipeNo));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_0, (lFSide));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_1, (lFCoax));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_2, lRSide);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_3, lRCoax);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_4, lTrans1);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_5, lTrans2);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_6, lPADPos);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_7, lROICount);
}

void CDlgPLCSetup::OnBnClickedBtnRcpEdit()
{
	long lRet = -1;
	int nPubNo = 0;
	CString strRecipeNo, strFSide, strFCoax;

	GetDlgItem(IDC_EDIT_RCP_NO)->GetWindowText(strRecipeNo);
	GetDlgItem(IDC_EDIT_F_SIDE)->GetWindowText(strFSide);
	GetDlgItem(IDC_EDIT_F_COAX)->GetWindowText(strFCoax);

	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_CMD_NO, 101);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_RCP_NO, _ttoi(strRecipeNo));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_0, _ttoi(strFSide));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_1, _ttoi(strFCoax));
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_2, 500);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_3, 300);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_4, 200);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_5, 200);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_6, 0);
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_7, 3);

	lRet = m_fnGetDevice(ADDR_RCP_EDIT_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
	nPubNo = lRet + 1;
	lRet = m_fnSetDevice(ADDR_RCP_EDIT_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

	// Receive No 와 ACK Code 는 Timer 에서 처리함
}

void CDlgPLCSetup::OnBnClickedBtnRcpUp()
{
	// Timer 에서 Check 중인 Publish No 를 확인하여 +1 이되면 하기 내용 적용한다.
	long lRet = -1;
	CString strRecipeNo, strFSide, strFCoax;
	int nPubNo = 0;
	GetDlgItem(IDC_EDIT_F_SIDE)->GetWindowText(strFSide);
	GetDlgItem(IDC_EDIT_F_COAX)->GetWindowText(strFCoax);

	// Recipe Body Upload 실시
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_CMD_NO, 100);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_0, _ttoi(strFSide));
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_1, _ttoi(strFCoax));
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_2, 500);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_3, 300);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_4, 200);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_5, 200);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_6, 0);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_7, 3);

	// ACK CODE 와 Receive No Set
	lRet = m_fnGetDevice(ADDR_RCP_UP_CIM_PUB_NO);
	nPubNo = lRet;
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_RECV_NO, nPubNo);
	lRet = m_fnSetDevice(ADDR_RCP_UP_EQ_ACK_CODE, 1);
}

void CDlgPLCSetup::OnBnClickedBtnRcpDown()
{
	// Timer 에서 Check 중인 Publish No 를 확인하여 +1 이되면 하기 내용 적용한다.
	long lRet = -1;
	CString strRecipeNo, strFSide, strFCoax, strTemp;
	long lFSide, lFCoax, lRSide, lRCoax, lTrans1, lTrans2, lPADPos, lROICount;


	// Recipe Body Download 실시
	lFSide = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_0);
	lFCoax = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_1);
	lRSide = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_2);
	lRCoax = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_3);
	lTrans1 = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_4);
	lTrans2 = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_5);
	lPADPos = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_6);
	lROICount = m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_7);

	// ACK CODE 와 Receive No Set
	lRet = m_fnSetDevice(ADDR_RCP_DOWN_EQ_CMD_NO, 105);
	lRet = m_fnGetDevice(ADDR_RCP_DOWN_CIM_PUB_NO);
	lRet = m_fnSetDevice(ADDR_RCP_DOWN_EQ_RECV_NO, lRet);
	lRet = m_fnSetDevice(ADDR_RCP_DOWN_EQ_ACK_CODE, 1);

	strTemp.Format(_T("%d"), lFSide);
	GetDlgItem(IDC_EDIT_F_SIDE)->SetWindowText(strTemp);
	strTemp.Format(_T("%d"), lFCoax);
	GetDlgItem(IDC_EDIT_F_COAX)->GetWindowText(strTemp);
}

void CDlgPLCSetup::OnBnClickedBtnProcInfo()
{
	long lRet = -1;
	int nPubNo = 0;
	CString strTemp;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPanelID1, strPanelID2;

	lRet = m_fnGetDevice(ADDR_PROC_INFO_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
	nPubNo = lRet + 1;
	lRet = m_fnSetDevice(ADDR_PROC_INFO_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

	lRet = m_fnSetDevice(ADDR_PROC_INFO_EQ_CMD_NO, 110);
	
	strPanelID1 = pFrame->m_fnGetPanelID_1(0);
	lRet = m_fnSetDeviceBlock(ADDR_PROC_INFO_EQ_PNL_ID_0, 20, strPanelID1);
	//lRet = m_fnSetDeviceBlock(ADDR_PROC_INFO_EQ_PNL_ID, 20, _T("PANELID_001"));

	// Receive No 와 Process Information은 Timer 에서 처리함
}

void CDlgPLCSetup::OnBnClickedBtnProcData()
{
	long lRet = -1;
	int nPubNo = 0;
	CString strTemp;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPanelID1, strPanelID2;

	lRet = m_fnSetDevice(ADDR_PROC_DATA_EQ_CMD_NO, 170);

	strPanelID1 = pFrame->m_fnGetPanelID_1(0);
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 20, strPanelID1);
	//lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 20, _T("PANELID_001"));

	//lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_RESULT, 2, _T("OK"));
	//lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_CODE_1, 4, _T("0001"));
	//lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_CODE_2, 4, _T("0002"));

	lRet = m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 2, _T("A"));
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, _T("AF1"));
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, _T("OK"));
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE2, 4, _T("0001"));
	lRet = m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE3, 4, _T("0002"));

	lRet = m_fnGetDevice(ADDR_PROC_DATA_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
	nPubNo = lRet + 1;
	lRet = m_fnSetDevice(ADDR_PROC_DATA_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

	// Receive No 와 ACK Code 는 Timer 에서 처리함
}

void CDlgPLCSetup::OnBnClickedBtnRcpChg()
{
	long lRet = -1;
	int RecipNo = 0;
	CString strTemp;
	
	GetDlgItem(IDC_RGCH_RCP_NO)->GetWindowText(strTemp);

	RecipNo = _ttoi(strTemp);

	lRet = m_fnSetDevice(ADDR_CUR_EQP_RCP_NO, RecipNo);
	lRet = 1;
	lRet = m_fnSetDevice(ADDR_CUR_RCP_CHG_ACK, lRet);	// Recieve No 에 +1 하여 Publish No Set
	
	while(lRet)
	{
		lRet = m_fnGetDevice(ADDR_CUR_RCP_CHG_REQ);
	}
	lRet = m_fnSetDevice(ADDR_CUR_RCP_CHG_ACK, lRet);
}

void CDlgPLCSetup::OnTimer(UINT_PTR nIDEvent)
{

	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());	

	m_nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	
	if(nIDEvent == 1000)	// Recipe Edit
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_RCP_EDIT_CIM_RECV_NO);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_EDIT_RECV_NO)->SetWindowText(strTemp);

			//lRet = m_fnGetDevice(ADDR_RCP_EDIT_CIM_ACK_CODE);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_EDIT_ACK_CODE)->SetWindowText(strTemp);
		}
	}
	else if(nIDEvent == 1001)	// Recipe UP
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_RCP_UP_CIM_PUB_NO);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_UP_PUB_NO)->SetWindowText(strTemp);
		}
	}
	else if(nIDEvent == 1002)	// Recipe Down
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_RCP_DOWN_CIM_PUB_NO);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_DOWN_PUB_NO)->SetWindowText(strTemp);
		}
	}
	else if(nIDEvent == 1003)	// Process Information
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_PROC_INFO_EQ_PUB_NO);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_PINFO_PUB_NO)->SetWindowText(strTemp);

			//lRet = -1;
			//CString strRecipeID, strStepID;
			//lRet = m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, strStepID);
		}
	}
	else if(nIDEvent == 1004)	// Process Data
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_PROC_DATA_CIM_RECV_NO);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_PDATA_RECV_NO)->SetWindowText(strTemp);

			//lRet = m_fnGetDevice(ADDR_PROC_DATA_CIM_ACK_CODE);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_PDATA_ACK_CODE)->SetWindowText(strTemp);
		}
	}
	else if(nIDEvent == 1005)	// Current Recipe Change REQ
	{
		if(m_bOpenPLC)
		{
			CString strTemp;

			//Chung Du Comment
			//long lRet = m_fnGetDevice(ADDR_CUR_RCP_CHG_REQ);

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_RGCH_REQ_FLAG)->SetWindowText(strTemp);
			////KEY_STATE_EQP_MODEL
			//
			//if(m_nEqpModel == 0 ) 
			//	lRet = m_fnGetDevice(_T("W8014"));
			//if(m_nEqpModel == 1)
			//	lRet = m_fnGetDevice(_T("W8016"));

			//strTemp.Format(_T("%d"), lRet);
			//GetDlgItem(IDC_RGCH_RCP_NO)->SetWindowText(strTemp);
		}
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

//2015.06.02 s-
void CDlgPLCSetup::OnBnClickedCheckOverlapUse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	//
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_OVERLAP_USE))->GetCheck();
	m_fnOverlapUseCheck(bCheck);

	//pFrame->m_stOverlapCoordInfo->bUseMode = bCheck;
}

//2016.03.23 add by ikm - S
void CDlgPLCSetup::OnBnClickedCheckAllOverlapDef() 
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_ALL_OVERLAP_DEF))->GetCheck();

	pFrame->m_stOverlapCoordInfo->bAllOverlapDef = bCheck;
}
//2016.03.23 add by ikm - E

void CDlgPLCSetup::m_fnLoadParameter()
{
	CString strTemp;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	m_bUseOverlap = pFrame->m_stOverlapCoordInfo->bUseMode;
	m_bAllOverlapDef = pFrame->m_stOverlapCoordInfo->bAllOverlapDef; //2016.03.23 add by ikm
	m_dOffX = pFrame->m_stOverlapCoordInfo->dOffsetX;
	m_dOffY = pFrame->m_stOverlapCoordInfo->dOffsetY;
	for (int i = 0; i < MAX_DEFECT_KIND; i++)
	{
		m_nAryDefKind[i] = pFrame->m_stOverlapCoordInfo->nDefectCheck[i];
		m_nAryDefCount[i] = pFrame->m_stOverlapCoordInfo->nDefectCount[i];
	}

	strTemp.Format(_T("%0.3f"), m_dOffY);
	GetDlgItem(IDC_EDIT_OVERLAP_POSITION_X)->SetWindowText(strTemp);
	strTemp.Format(_T("%0.3f"), m_dOffY);
	GetDlgItem(IDC_EDIT_OVERLAP_POSITION_Y)->SetWindowText(strTemp);

	m_bUseContactFail = pFrame->m_stOverlapCoordInfo->bUseContactFail;
	m_nContactFailCount =  pFrame->m_stOverlapCoordInfo->nContactFailDefectCount;

	//2016.02.24
	m_bUsePolMiss = pFrame->m_stOverlapCoordInfo->bUsePolMiss;
	m_nPolMissCount =  pFrame->m_stOverlapCoordInfo->nPolMissDefectCount;

	//2016.08.22
	m_bUseAlignFail = pFrame->m_stOverlapCoordInfo->bUseAlignFail;
	m_nAlignFailCount =  pFrame->m_stOverlapCoordInfo->nAlignFaliDefectCount;

	//2019.03.21
	m_bUseBgrade = pFrame->m_stOverlapCoordInfo->bUseBgradecnt;
	m_nBgradeSetCount =  pFrame->m_stOverlapCoordInfo->nBgradecntDefectCount;

	m_bUseCgrade = pFrame->m_stOverlapCoordInfo->bUseCgradecnt;
	m_nCgradeSetCount=  pFrame->m_stOverlapCoordInfo->nCgradecntDefectCount;

	//2016.11.21
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		m_bArrLumiKind[i] = pFrame->m_stOverlapCoordInfo->bArrLuminanceChk[i];
		m_nArrLumiCount[i] = pFrame->m_stOverlapCoordInfo->nArrLuminanceCnt[i];
	}
	//2020.02.28 add by mby
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		m_bArrBubbleStationKind[i] = pFrame->m_stOverlapCoordInfo->bArrBubbleStationChk[i];
		m_nArrBubbleStationCount[i] = pFrame->m_stOverlapCoordInfo->nArrBubbleStationCnt[i];
	}
	//

	//2015.09.16 Mr.Chu
	m_nPanelCount =  pFrame->m_stOverlapCoordInfo->nPanelCnt;

	m_fnOverlapUseCheck(m_bUseOverlap);

	//2017.06.28	Update Value to Ui
	UpdateData(FALSE);
	//UpdateData 갱신 시 해당 변수 두개(nDefectCount) 초기화되는 버그 예외처리
	SetDlgItemInt(IDC_EDIT_OVERLAP_COUNT11,pFrame->m_stOverlapCoordInfo->nDefectCount[INSP_POL_BUBBLE]);	//UpdateData(FALSE) 시 0으로 초기화 되는 현상 예외처리
	SetDlgItemInt(IDC_EDIT_OVERLAP_COUNT12,pFrame->m_stOverlapCoordInfo->nDefectCount[INSP_POL_SCRATCH]);	//UpdateData(FALSE) 시 0으로 초기화 되는 현상 예외처리
	//
}

void CDlgPLCSetup::m_fnSaveParameter()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strTemp, strDefectCheck;

	UpdateData(TRUE);

	strTemp.Format(_T("%d"), m_bUseOverlap);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_USEMODE, strTemp);

	//2016.03.23 add by ikm - S
	strTemp.Format(_T("%d"), m_bAllOverlapDef);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALL_OVERLAP_DEF_MODE, strTemp);
	//2016.03.23 add by ikm - E

	strTemp.Format(_T("%0.3f"), m_dOffX);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_OFFSET_X, strTemp);

	strTemp.Format(_T("%0.3f"), m_dOffY);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_OFFSET_Y, strTemp);

	strTemp = _T("");
	for (int i = 0; i < MAX_DEFECT_KIND; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefKind[i]);

		if (i == MAX_DEFECT_KIND - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_DEFECT_CHECK, strTemp);

	strTemp = _T("");
	for (int i = 0; i < MAX_DEFECT_KIND; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefCount[i]);

		if (i == MAX_DEFECT_KIND - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_DEFECT_COUNT, strTemp);
	
	strTemp.Format(_T("%d"), m_bUseContactFail);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nContactFailCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_SETTING_COUNT, strTemp);

	//2016.02.24
	strTemp.Format(_T("%d"), m_bUsePolMiss);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nPolMissCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_SETTING_COUNT, strTemp);

	//2016.08.22	Mr.Chu
	strTemp.Format(_T("%d"), m_bUseAlignFail);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nAlignFailCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_SETTING_COUNT, strTemp);
	//2019.03.20	ryu
	///////////////////////////////////////// KEY_OVERLAP_B_GRADE_CHECK /////////////////////////////////////////////////////////
	strTemp.Format(_T("%d"), m_bUseBgrade);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_B_GRADE_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nBgradeSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_B_GRADE_SETTING_COUNT, strTemp);

	strTemp.Format(_T("%d"), m_bUseCgrade);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_C_GRADE_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nCgradeSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_C_GRADE_SETTING_COUNT, strTemp);

	//2016.11.21		
	///////////////////////////////////////// KEY_OVERLAP_LUMINANCE_CHECK /////////////////////////////////////////////////////////
	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_bArrLumiKind[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_CHECK, strTemp);
	///////////////////////////////////////// KEY_OVERLAP_LUMINANCE_COUNT /////////////////////////////////////////////////////////
	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nArrLumiCount[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_SETTING_COUNT, strTemp);

	//2020.02.28 add by mby		
	///////////////////////////////////////// KEY_OVERLAP_BUBBLE_CHECK /////////////////////////////////////////////////////////
	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_bArrBubbleStationKind[i]);

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_BUBBLESTATION_CHECK, strTemp);
	///////////////////////////////////////// KEY_OVERLAP_BUBBLE_COUNT /////////////////////////////////////////////////////////
	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nArrBubbleStationCount[i]);

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_BUBBLESTATION_SETTING_COUNT, strTemp);

	//2015.09.16 Mr.Chu
	strTemp.Format(_T("%d"), m_nPanelCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_PANEL_COUNT, strTemp);

	pFrame->m_fnSetOverlapCoordInfo();

}

void CDlgPLCSetup::m_fnOverlapUseCheck(BOOL bUseCheck)
{
	int nEqpModel;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	//0:AVL 1:ABI
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	GetDlgItem(IDC_EDIT_OVERLAP_POSITION_X)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_POSITION_Y)->EnableWindow(bUseCheck);

	if (nEqpModel == 0)
	{
		GetDlgItem(IDC_CHECK_DEFECT1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_COUNT1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT34)->EnableWindow(bUseCheck);
	}
	else if(nEqpModel == 1)
	{
		GetDlgItem(IDC_CHECK_DEFECT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT34)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT35)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT36)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT34)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT35)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT36)->EnableWindow(bUseCheck);

		//2016.11.21	Mr.Chu
		GetDlgItem(IDC_CHECK_TDI_LUMINANCE)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_TDI_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_CHECK_CF_PROTECTION)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_TFT_PROTECTION)->EnableWindow(bUseCheck);
	}

	GetDlgItem(IDC_CHECK_CONTACT_FAIL)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_CONTACT_FAIL_COUNT)->EnableWindow(bUseCheck);

	//2016.02.24
	GetDlgItem(IDC_CHECK_POL_MISS)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_POL_MISS_COUNT)->EnableWindow(bUseCheck);

	//2015.09.16 Mr.Chu
	GetDlgItem(IDC_STATIC_PANEL_COUNT)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_PANEL_COUNT)->EnableWindow(bUseCheck);

	//2016.08.22	Mr.Chu
	GetDlgItem(IDC_CHECK_ALIGN_FAIL)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_POL_ALIGN_FAIL_COUNT)->EnableWindow(bUseCheck);

	//2016.11.21	Mr.Chu
	GetDlgItem(IDC_CHECK_CFC_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_CFC_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_CBC_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_CBC_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_CFS_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_CFS_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_CBS_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_CBS_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_TFC_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_TFC_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_TBC_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_TBC_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_TFS_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_TFS_LUMINANCE_COUNT)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_TBS_LUMINANCE)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_TBS_LUMINANCE_COUNT)->EnableWindow(bUseCheck);
	//

	//2016.03.23 add by ikm - S
	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
		GetDlgItem(IDC_CHECK_ALL_OVERLAP_DEF)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_CHECK_ALL_OVERLAP_DEF)->EnableWindow(FALSE);
	//2016.03.23 add by ikm - E
}

int CDlgPLCSetup::m_fnGetDeviceFromPlcTask(TCHAR * strAddr, short* psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte bParam[60] = {0,};

	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bParam, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_GET_DEVICE, 0, sizeof(bParam), bParam, 0, 0, 0, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}

	*psData = *(short*)&bParam[0];

	::SysFreeString(UnicodeStr);

	return nRet;
}

int CDlgPLCSetup::m_fnGetDeviceDwordFromPlcTask(TCHAR * strAddr, long* psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte bParam[60] = {0,};
	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bParam, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_GET_DEVICE_DWORD, 0, sizeof(bParam), bParam, 0, 0, 0, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}

	*psData = *(long*)&bParam[0];
	::SysFreeString(UnicodeStr);

	return nRet;
}

int CDlgPLCSetup::m_fnGetDeviceDwordFromPlcTaskModel(TCHAR * strAddr, long* psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;

	int nRet = APP_OK;
	byte bParam[60] = {0,};
	BSTR UnicodeStr = NULL;
	CString MultiByteStr = strAddr;
	//CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bParam, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_GET_DEVICE_DWORD, 0, sizeof(bParam), bParam, 0, 0, 0, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}

	*psData = *(long*)&bParam[0];
	::SysFreeString(UnicodeStr);

	return nRet;
}

int CDlgPLCSetup::m_fnGetDeviceBlockDwordFromPlcTask(TCHAR * strAddr, int nSize, LONG *lplData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte* bpParam = new byte[sizeof(DWORD)*nSize+50];
	byte* bpTemp = bpParam;

	memset(bpParam,0,sizeof(DWORD)*nSize+50);

	*(long*)bpTemp = (long)nSize;
	bpTemp += sizeof(long);

	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_GET_DEVICE_BLOCK_DWORD, 0, sizeof(DWORD)*nSize+50, bpParam, 0, 0, 0, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}

	memcpy(lplData, bpParam, nSize * sizeof(DWORD));
	SAFE_DELETE_ARR(bpParam);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceFromPlcTask(TCHAR * strAddr, short psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte bParam[60]={0,};
	byte* bpTemp = &bParam[0];

	*(short*)bpTemp = psData;
	bpTemp += sizeof(short);

	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);
		
	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE, 0, sizeof(bParam), bParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceAutoClearFromPlcTask(TCHAR * strAddr, short psData, ULONG lWaitTime)
{
	if (G_pVsServerIF==NULL)
		return APP_NG;

	int nRet = APP_OK;
	byte bParam[60]= {0,};
	byte* bpTemp = &bParam[0];

	*(short*)bpTemp = psData;
	bpTemp += sizeof(short);

	*(ULONG*)bpTemp = lWaitTime;
	bpTemp += sizeof(ULONG);

	BSTR UnicodeStr = NULL;
	CString MultyByteStr = strAddr;
	UnicodeStr = MultyByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE_AUTOCLEAR,
		0, sizeof(bParam), bParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceDwordFromPlcTask(TCHAR * strAddr, long psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte bParam[60]={0,};
	byte* bpTemp = &bParam[0];

	*(long*)bpTemp = psData;
	bpTemp += sizeof(long);

	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);


	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE_DWORD, 0, sizeof(bParam), bParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceDwordFromPlcTaskModel(TCHAR * strAddr, long psData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;

	int nRet = APP_OK;
	byte bParam[60]={0,};
	byte* bpTemp = &bParam[0];

	*(long*)bpTemp = psData;
	bpTemp += sizeof(long);

	BSTR UnicodeStr = NULL;
	CString MultiByteStr = strAddr;
	//CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);


	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE_DWORD, 0, sizeof(bParam), bParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceDwordAutoClearFromPlcTask(TCHAR * strAddr, long psData, ULONG lWaitTime)
{
	if (G_pVsServerIF==NULL)
		return APP_NG;

	int nRet = APP_OK;
	byte bParam[60] = {0,};
	byte* bpTemp = &bParam[0];

	*(long*)bpTemp = psData;
	bpTemp += sizeof(long);

	*(ULONG*)bpTemp = lWaitTime;
	bpTemp += sizeof(ULONG);

	BSTR UnicodeStr = NULL;
	CString MultyByteStr = strAddr;
	UnicodeStr = MultyByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);

	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE_DWORD_AUTOCLEAR,
		0, sizeof(bParam), bParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	::SysFreeString(UnicodeStr);
	return nRet;
}

int CDlgPLCSetup::m_fnSetDeviceBlockDwordFromPlcTask(TCHAR * strAddr, int nSize, LONG *lplData)
{
	if (G_pVsServerIF == NULL )
		return APP_NG;
		
	int nRet = APP_OK;
	byte* bpParam = new byte[sizeof(DWORD)*nSize+60];
	byte* bpTemp = bpParam;
	memset(bpParam,0,sizeof(DWORD)*nSize+60);

	*(long*)bpTemp = (long)nSize;
	bpTemp += sizeof(long);
	memcpy(bpTemp, lplData, sizeof(long) * nSize);
	bpTemp += sizeof(long) * nSize;

	BSTR UnicodeStr = NULL;
	//CString MultiByteStr = strAddr;
	CString MultiByteStr = m_fnConvertHexToDec(strAddr);
	UnicodeStr = MultiByteStr.AllocSysString();
	wcscpy((WCHAR*)bpTemp, UnicodeStr);


	nRet = G_pVsServerIF->m_fnSendCommand(VS_PLC_TASK, VS_PLC_SEQUENCE_SET_DEVICE_BLOCK_DWORD, 0, sizeof(DWORD)*nSize+60, bpParam, 0, 0, 0, m_nCmdType);
	if (nRet != APP_OK)
	{
		// Write Error Log
	}
	else
		Sleep(0);
	SAFE_DELETE_ARR(bpParam);
	::SysFreeString(UnicodeStr);
	return nRet;
}

CString CDlgPLCSetup::m_fnConvertHexToDec(TCHAR * strAddr)
{
	CString strHex, strDec, strArea, strTemp;
	int	nResult = 0;
	
	strTemp = strAddr;

	strArea = strTemp.Left(1);
	//strHex.Replace(strArea, _T(""));
	strHex = strTemp.Right(4);

	int nLen = strHex.GetLength();
	TCHAR * tchHex = (TCHAR *)(LPCTSTR)strHex;			

	for(int i=0;i<nLen;i++)
	{
		TCHAR tchCh = 0;
		
		if(tchHex[i] >= L'0' && tchHex[i] <= L'9')
			tchCh = (tchHex[i] - 0x0030) & 0x000F;
		else if(tchHex [i] >= L'A' && tchHex[i] <= L'F')
			tchCh = (tchHex[i] - 0x0037) & 0x000F;
		else
			return 0;

		nResult |= (tchCh << ((nLen-1) - i) * 4);
	}

	strDec.Format(_T("%s%d"), strArea, nResult);

	return strDec;
}






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CDlgDEFSetupParam1, CDialogEx)
	CDlgDEFSetupParam1::CDlgDEFSetupParam1(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDEFSetupParam1::IDD, pParent)
{


}

CDlgDEFSetupParam1::~CDlgDEFSetupParam1()
{

}

BEGIN_MESSAGE_MAP(CDlgDEFSetupParam1, CDialogEx)

END_MESSAGE_MAP()

void CDlgDEFSetupParam1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_DEFECT1,       m_nAryDefKind[INSP_DUST]);
	DDX_Check(pDX, IDC_CHECK_DEFECT2,       m_nAryDefKind[INSP_WHITE_TURBIDITY]);
	DDX_Check(pDX, IDC_CHECK_DEFECT3,       m_nAryDefKind[INSP_ITO]);
	DDX_Check(pDX, IDC_CHECK_DEFECT4,       m_nAryDefKind[INSP_EDGE_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT5,       m_nAryDefKind[INSP_PAD_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT6,       m_nAryDefKind[INSP_DIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT7,       m_nAryDefKind[INSP_ETCHING_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT8,       m_nAryDefKind[INSP_DIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT9,       m_nAryDefKind[INSP_PAD_BURNT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT10,      m_nAryDefKind[INSP_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT11,      m_nAryDefKind[INSP_POL_BUBBLE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT12,      m_nAryDefKind[INSP_POL_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT13,      m_nAryDefKind[INSP_POL_ALIEN_SUBSTANCE ]);
	DDX_Check(pDX, IDC_CHECK_DEFECT14,      m_nAryDefKind[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Check(pDX, IDC_CHECK_DEFECT15,      m_nAryDefKind[INSP_BLACK_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT16,      m_nAryDefKind[INSP_WHITE_SCRATCH]);
	DDX_Check(pDX, IDC_CHECK_DEFECT17,      m_nAryDefKind[INSP_CF_MEASURE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT18,      m_nAryDefKind[INSP_TFT_MEASURE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT19,      m_nAryDefKind[INSP_MURA]);
	DDX_Check(pDX, IDC_CHECK_DEFECT20,      m_nAryDefKind[INSP_NO_USE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT21,      m_nAryDefKind[INSP_CF_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT22,      m_nAryDefKind[INSP_TFT_BURR]);
	DDX_Check(pDX, IDC_CHECK_DEFECT23,      m_nAryDefKind[INSP_CF_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT24,      m_nAryDefKind[INSP_TFT_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT25,      m_nAryDefKind[INSP_CF_CORNER_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT26,      m_nAryDefKind[INSP_TFT_CORNER_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT27,      m_nAryDefKind[INSP_CF_CELL_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT28,      m_nAryDefKind[INSP_TFT_CELL_BROKEN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT29,      m_nAryDefKind[INSP_PIMPLE_POINT]);
	DDX_Check(pDX, IDC_CHECK_DEFECT30,      m_nAryDefKind[INSP_PIMPLE_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT31,      m_nAryDefKind[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT32,      m_nAryDefKind[INSP_CF_WAVE_PATTERN]);
	DDX_Check(pDX, IDC_CHECK_DEFECT33,      m_nAryDefKind[INSP_ETC_LINE]);
	DDX_Check(pDX, IDC_CHECK_DEFECT34,      m_nAryDefKind[INSP_ETC_POINT]);
	DDX_Check(pDX, IDC_CHECK_CONTACT_FAIL,  m_bUseContactFail);
	DDX_Check(pDX, IDC_CHECK_POL_MISS,		m_bUsePolMiss);	//2016.02.24
	DDX_Check(pDX, IDC_CHECK_ALIGN_FAIL,	m_bUseAlignFail);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT1,  m_nAryDefCount[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT2,  m_nAryDefCount[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT3,  m_nAryDefCount[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT4,  m_nAryDefCount[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT5,  m_nAryDefCount[INSP_PAD_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT6,  m_nAryDefCount[INSP_DIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT7,  m_nAryDefCount[INSP_ETCHING_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT8,  m_nAryDefCount[INSP_DIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT9,  m_nAryDefCount[INSP_PAD_BURNT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT10, m_nAryDefCount[INSP_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT11, m_nAryDefCount[INSP_POL_BUBBLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT12, m_nAryDefCount[INSP_POL_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT13, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT14, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT15, m_nAryDefCount[INSP_BLACK_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT16, m_nAryDefCount[INSP_WHITE_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT17, m_nAryDefCount[INSP_CF_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT18, m_nAryDefCount[INSP_TFT_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT19, m_nAryDefCount[INSP_MURA]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT20, m_nAryDefCount[INSP_CONTACT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT21, m_nAryDefCount[INSP_CF_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT22, m_nAryDefCount[INSP_TFT_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT23, m_nAryDefCount[INSP_CF_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT24, m_nAryDefCount[INSP_TFT_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT25, m_nAryDefCount[INSP_CF_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT26, m_nAryDefCount[INSP_TFT_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT27, m_nAryDefCount[INSP_CF_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT28, m_nAryDefCount[INSP_TFT_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT29,m_nAryDefCount[INSP_PIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT30,m_nAryDefCount[INSP_PIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT31,m_nAryDefCount[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT32,m_nAryDefCount[INSP_CF_WAVE_PATTERN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT33,m_nAryDefCount[INSP_ETC_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT34,m_nAryDefCount[INSP_ETC_POINT]);
	DDX_Text(pDX, IDC_EDIT_CONTACT_FAIL_COUNT, m_nContactFailStopCount);
	DDX_Text(pDX, IDC_EDIT_POL_MISS_COUNT, m_nPolMissStopCount);	//2016.02.24
	DDX_Text(pDX, IDC_EDIT_ALIGN_FAIL_COUNT, m_nAlignFailStopCount);


	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT1,  m_nAryDefAlarmCount[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT2,  m_nAryDefAlarmCount[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT3,  m_nAryDefAlarmCount[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT4,  m_nAryDefAlarmCount[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT5,  m_nAryDefAlarmCount[INSP_PAD_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT6,  m_nAryDefAlarmCount[INSP_DIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT7,  m_nAryDefAlarmCount[INSP_ETCHING_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT8,  m_nAryDefAlarmCount[INSP_DIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT9,  m_nAryDefAlarmCount[INSP_PAD_BURNT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT10, m_nAryDefAlarmCount[INSP_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT11, m_nAryDefAlarmCount[INSP_POL_BUBBLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT12, m_nAryDefAlarmCount[INSP_POL_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT13, m_nAryDefAlarmCount[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT14, m_nAryDefAlarmCount[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT15, m_nAryDefAlarmCount[INSP_BLACK_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT16, m_nAryDefAlarmCount[INSP_WHITE_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT17, m_nAryDefAlarmCount[INSP_CF_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT18, m_nAryDefAlarmCount[INSP_TFT_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT19, m_nAryDefAlarmCount[INSP_MURA]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT20, m_nAryDefAlarmCount[INSP_NO_USE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT21, m_nAryDefAlarmCount[INSP_CF_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT22, m_nAryDefAlarmCount[INSP_TFT_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT23, m_nAryDefAlarmCount[INSP_CF_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT24, m_nAryDefAlarmCount[INSP_TFT_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT25, m_nAryDefAlarmCount[INSP_CF_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT26, m_nAryDefAlarmCount[INSP_TFT_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT27, m_nAryDefAlarmCount[INSP_CF_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT28, m_nAryDefAlarmCount[INSP_TFT_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT29,m_nAryDefAlarmCount[INSP_PIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT30,m_nAryDefAlarmCount[INSP_PIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT31,m_nAryDefAlarmCount[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT32,m_nAryDefAlarmCount[INSP_CF_WAVE_PATTERN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT33,m_nAryDefAlarmCount[INSP_ETC_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);


	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME1,   m_nAryDefTerm[INSP_DUST]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME2,   m_nAryDefTerm[INSP_WHITE_TURBIDITY]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME3,   m_nAryDefTerm[INSP_ITO]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME4,   m_nAryDefTerm[INSP_EDGE_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME5,   m_nAryDefTerm[INSP_PAD_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME6,   m_nAryDefTerm[INSP_DIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME7,   m_nAryDefTerm[INSP_ETCHING_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME8,   m_nAryDefTerm[INSP_DIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME9,   m_nAryDefTerm[INSP_PAD_BURNT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME10,  m_nAryDefTerm[INSP_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME11,  m_nAryDefTerm[INSP_POL_BUBBLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME12,  m_nAryDefTerm[INSP_POL_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME13,  m_nAryDefTerm[INSP_POL_ALIEN_SUBSTANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME14,  m_nAryDefTerm[INSP_POL_ALIEN_SUBSTANCE+1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME15,  m_nAryDefTerm[INSP_BLACK_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME16,  m_nAryDefTerm[INSP_WHITE_SCRATCH]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME17,  m_nAryDefTerm[INSP_CF_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME18,  m_nAryDefTerm[INSP_TFT_MEASURE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME19,  m_nAryDefTerm[INSP_MURA]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME20,  m_nAryDefTerm[INSP_NO_USE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME21,  m_nAryDefTerm[INSP_CF_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME22,  m_nAryDefTerm[INSP_TFT_BURR]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME23,  m_nAryDefTerm[INSP_CF_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME24,  m_nAryDefTerm[INSP_TFT_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME25,  m_nAryDefTerm[INSP_CF_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME26,  m_nAryDefTerm[INSP_TFT_CORNER_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME27,  m_nAryDefTerm[INSP_CF_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME28,  m_nAryDefTerm[INSP_TFT_CELL_BROKEN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME29,  m_nAryDefTerm[INSP_PIMPLE_POINT]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME30,  m_nAryDefTerm[INSP_PIMPLE_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME31,  m_nAryDefTerm[INSP_CF_PIMPLE_CIRCLE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME32,  m_nAryDefTerm[INSP_CF_WAVE_PATTERN]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME33,  m_nAryDefTerm[INSP_ETC_LINE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME34,  m_nAryDefTerm[INSP_ETC_POINT]);
	DDX_Text(pDX, IDC_EDIT_CONTACT_FAIL_TIME, m_fContactFailTerm);
	DDX_Text(pDX, IDC_EDIT_POL_MISS_TIME, m_fPolMissTerm);	//2016.02.24

	//DDV_MinMaxInt(pDX, m_nPanelCount, 0, 1000);

	DDX_Text(pDX, IDC_EDIT_ALIGN_FAIL_TIME, m_fAlignFailTerm);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT37, m_nAlignFailSetCount);
	//  DDX_Control(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT36, m_nPolMissDefectCount);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT36, m_nPolMissSetCount);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT35, m_nContactFailSetCount);

}

//Time OverLap Parameter 저장 .  Def 관련 항목들은 Param2 Tab에서 저장한다.
void CDlgDEFSetupParam1::m_fnSaveParameter(CString *strDefArr)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int i = 0;
	CString strTemp = _T(""), strDefectCheck = _T("");

	UpdateData(TRUE);

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefKind[i]);
		strDefArr[TIME_OVERLAP_CHECK].Append(strDefectCheck + EXT_COLON);
	}

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefCount[i]);
		strDefArr[TIME_OVERLAP_STOP_COUNT].Append(strDefectCheck + EXT_COLON);
	}

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryDefTerm[i]));
		strDefArr[TIME_OVERLAP_TERM].Append(strDefectCheck + EXT_COLON);
	}

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryDefAlarmCount[i]));
		strDefArr[TIME_OVERLAP_ALARM_COUNT].Append(strDefectCheck + EXT_COLON);
	}


	strTemp.Format(_T("%d"), m_bUseContactFail);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nContactFailStopCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_STOP_COUNT, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_fContactFailTerm));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_TERM, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_nContactFailSetCount));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_SETTING_COUNT, strTemp);

	//2016.02.24
	strTemp.Format(_T("%d"), m_bUsePolMiss);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nPolMissStopCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_STOP_COUNT, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_fPolMissTerm));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_TERM, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_nPolMissSetCount));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE,  KEY_OVERLAP_POL_MISS_SETTING_COUNT, strTemp);
	//

	//2016.08.22
	strTemp.Format(_T("%d"), m_bUseAlignFail);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_CHECK, strTemp);

	strTemp.Format(_T("%d"), m_nAlignFailSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_SETTING_COUNT, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_fAlignFailTerm));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_TERM, strTemp);

	strTemp.Format(_T("%0.1f"), float(m_nAlignFailStopCount));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_STOP_COUNT, strTemp);
}

//Time OverLap Parameter 저장 .  Def 관련 항목들은 Param1 Tab에서 로드한다.
void CDlgDEFSetupParam1::m_fnLoadParameter( int *nDefArrCurPos )
{
	CString strTemp;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	TCHAR strBuffer[255];
	CString strLine = _T(""), strDefectCheck = _T("");
	int i = 0;
	int nDefectCnt = 0;

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	CString resTokenKind = _T("");
	if( nDefArrCurPos[TIME_OVERLAP_CHECK] >= 0)
		resTokenKind = strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_CHECK]);

	for(i=0; i<MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{		
		if( resTokenKind == _T("") )
			break;

		m_nAryDefKind[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_CHECK]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	CString resTokenCount = _T("");
	if( nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] >= 0)
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT]);
	for(i=0; i<MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{		
		if( resTokenCount == _T("") )
			break;

		m_nAryDefCount[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_TERM, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	CString resTokenTime = _T("");
	if( nDefArrCurPos[TIME_OVERLAP_TERM] >= 0)
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_TERM]);
	for(i=0; i<MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{		
		if( resTokenTime == _T("") )
			break;

		m_nAryDefTerm[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_TERM]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_STOP_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	CString resTokenAlarmCount = _T("");
	if( nDefArrCurPos[TIME_OVERLAP_STOP_COUNT] >= 0)
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_STOP_COUNT]);
	for(i=0; i<MAX_DEFECT_KIND_PARAM_UI_ONE; i++)
	{		
		if( resTokenAlarmCount == _T("") )
			break;

		m_nAryDefAlarmCount[nDefectCnt] = _ttof(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_STOP_COUNT]);
		nDefectCnt++;
	}

	m_bUseContactFail		=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_CHECK, 0);
	m_nContactFailStopCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_STOP_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_TERM, _T(""), strBuffer,255);
	m_fContactFailTerm			= _ttof(strBuffer);
	m_nContactFailSetCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_SETTING_COUNT, 0);

	//2016.02.24
	m_bUsePolMiss			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_CHECK, 0);
	m_nPolMissStopCount		=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_STOP_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_TERM, _T(""), strBuffer,255);
	m_fPolMissTerm		= _ttof(strBuffer);
	m_nPolMissSetCount		=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_SETTING_COUNT, 0);

	//2016.08.22
	m_bUseAlignFail			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_CHECK, 0);
	m_nAlignFailSetCount    =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_SETTING_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_TERM, _T(""), strBuffer,255);
	m_fAlignFailTerm		= _ttof(strBuffer);
	m_nAlignFailStopCount	=	pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_STOP_COUNT, 0);

	UpdateData(FALSE);
}

void CDlgDEFSetupParam1::m_fnOverlapUseCheck(BOOL bUseCheck, int nEqpModel)
{
	if (nEqpModel == EQP_TYPE_AVL)
	{
		GetDlgItem(IDC_CHECK_DEFECT1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_COUNT1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_TIME1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT1)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT2)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT3)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT7)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT8)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT15)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT16)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT29)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT30)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT31)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT32)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT34)->EnableWindow(bUseCheck);
	}
	else if(nEqpModel == EQP_TYPE_ABI)
	{
		GetDlgItem(IDC_CHECK_DEFECT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_CHECK_DEFECT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_COUNT4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_COUNT34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_TIME4)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME5)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME6)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME9)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME10)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME11)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME12)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME13)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME14)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME17)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME18)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME19)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME21)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME22)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME23)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME24)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME25)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME26)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME27)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME28)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME33)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_TIME34)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_CHECK_CONTACT_FAIL)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_CONTACT_FAIL_COUNT)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_CONTACT_FAIL_TIME)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT35)->EnableWindow(bUseCheck);


		//2016.02.24
		GetDlgItem(IDC_CHECK_POL_MISS)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_POL_MISS_COUNT)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_POL_MISS_TIME)->EnableWindow(bUseCheck);
		GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT36)->EnableWindow(bUseCheck);
	}
	//2016.08.22
	GetDlgItem(IDC_CHECK_ALIGN_FAIL)->EnableWindow(bUseCheck);	
	GetDlgItem(IDC_EDIT_ALIGN_FAIL_COUNT)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_ALIGN_FAIL_TIME)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT37)->EnableWindow(bUseCheck);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//2017.07.31
typedef enum e_DefSetUpParam2
{
	PARAM_CF_PROTECTION = 0,
	PARAM_TFT_PROTECTION,
	PARAM_B_GRADE_CHECK,
	PARAM_C_GRADE_CHECK,
	MAX_SET_PARAM
};


IMPLEMENT_DYNAMIC(CDlgDEFSetupParam2, CDialogEx)
	CDlgDEFSetupParam2::CDlgDEFSetupParam2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDEFSetupParam2::IDD, pParent)
{


}

CDlgDEFSetupParam2::~CDlgDEFSetupParam2()
{

}

BEGIN_MESSAGE_MAP(CDlgDEFSetupParam2, CDialogEx)

END_MESSAGE_MAP()

void CDlgDEFSetupParam2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	//2019.03.25 by ryu
	DDX_Check(pDX, IDC_CHECK_DEFECT37,	m_bUseBgrade);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME37, m_fBgradeTerm);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_B_GRADE_COUNT, m_nBgradeSetCount);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT37, m_nBgradeStopCount);

	DDX_Check(pDX, IDC_CHECK_DEFECT38,	m_bUseCgrade);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME38, m_fCgradeTerm);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT38, m_nCgradeSetCount);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT38, m_nCgradeStopCount);
	//2019.03.25 by ryu

	// Defect Arr
	DDX_Check(pDX, IDC_CHECK_DEFECT35,  m_nAryDefKind[PARAM_CF_PROTECTION]);
	DDX_Check(pDX, IDC_CHECK_DEFECT36,  m_nAryDefKind[PARAM_TFT_PROTECTION]);
	//2020.02.28 add by mby
	DDX_Check(pDX, IDC_CHECK_DEFECT39,  m_bAryBubbleStationKind[BUBBLESTATION_1]);
	DDX_Check(pDX, IDC_CHECK_DEFECT40,  m_bAryBubbleStationKind[BUBBLESTATION_2]);
	DDX_Check(pDX, IDC_CHECK_DEFECT41,  m_bAryBubbleStationKind[BUBBLESTATION_3]);
	DDX_Check(pDX, IDC_CHECK_DEFECT42,  m_bAryBubbleStationKind[BUBBLESTATION_4]);
	DDX_Check(pDX, IDC_CHECK_DEFECT43,  m_bAryBubbleStationKind[BUBBLESTATION_5]);
	DDX_Check(pDX, IDC_CHECK_DEFECT44,  m_bAryBubbleStationKind[BUBBLESTATION_6]);
	DDX_Check(pDX, IDC_CHECK_DEFECT45,  m_bAryBubbleStationKind[BUBBLESTATION_7]);
	DDX_Check(pDX, IDC_CHECK_DEFECT46,  m_bAryBubbleStationKind[BUBBLESTATION_8]);
	DDX_Check(pDX, IDC_CHECK_DEFECT47,  m_bAryBubbleStationKind[BUBBLESTATION_9]);
	DDX_Check(pDX, IDC_CHECK_DEFECT48,  m_bAryBubbleStationKind[BUBBLESTATION_10]);
	DDX_Check(pDX, IDC_CHECK_DEFECT49,  m_bAryBubbleStationKind[BUBBLESTATION_11]);
	DDX_Check(pDX, IDC_CHECK_DEFECT50,  m_bAryBubbleStationKind[BUBBLESTATION_12]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME35,   m_nAryDefTerm[PARAM_CF_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME36,   m_nAryDefTerm[PARAM_TFT_PROTECTION]);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME39,   m_nAryBubbleStationTerm[BUBBLESTATION_1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME40,   m_nAryBubbleStationTerm[BUBBLESTATION_2]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME41,   m_nAryBubbleStationTerm[BUBBLESTATION_3]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME42,   m_nAryBubbleStationTerm[BUBBLESTATION_4]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME43,   m_nAryBubbleStationTerm[BUBBLESTATION_5]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME44,   m_nAryBubbleStationTerm[BUBBLESTATION_6]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME45,   m_nAryBubbleStationTerm[BUBBLESTATION_7]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME46,   m_nAryBubbleStationTerm[BUBBLESTATION_8]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME47,   m_nAryBubbleStationTerm[BUBBLESTATION_9]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME48,   m_nAryBubbleStationTerm[BUBBLESTATION_10]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME49,   m_nAryBubbleStationTerm[BUBBLESTATION_11]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME50,   m_nAryBubbleStationTerm[BUBBLESTATION_12]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT35,  m_nAryDefAlarmCount[PARAM_CF_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT36,  m_nAryDefAlarmCount[PARAM_TFT_PROTECTION]);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT39,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT40,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_2]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT41,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_3]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT42,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_4]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT43,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_5]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT44,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_6]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT45,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_7]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT46,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_8]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT47,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_9]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT48,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_10]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT49,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_11]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT50,  m_nAryBubbleStationAlarmCount[BUBBLESTATION_12]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT35, m_nAryDefCount[PARAM_CF_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT36, m_nAryDefCount[PARAM_TFT_PROTECTION]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT39, m_nAryBubbleStationDefCount[BUBBLESTATION_1]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT40, m_nAryBubbleStationDefCount[BUBBLESTATION_2]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT41, m_nAryBubbleStationDefCount[BUBBLESTATION_3]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT42, m_nAryBubbleStationDefCount[BUBBLESTATION_4]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT43, m_nAryBubbleStationDefCount[BUBBLESTATION_5]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT44, m_nAryBubbleStationDefCount[BUBBLESTATION_6]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT45, m_nAryBubbleStationDefCount[BUBBLESTATION_7]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT46, m_nAryBubbleStationDefCount[BUBBLESTATION_8]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT47, m_nAryBubbleStationDefCount[BUBBLESTATION_9]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT48, m_nAryBubbleStationDefCount[BUBBLESTATION_10]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT49, m_nAryBubbleStationDefCount[BUBBLESTATION_11]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT50, m_nAryBubbleStationDefCount[BUBBLESTATION_12]);
	//2020.02.28 add by mby

	DDX_Check(pDX, IDC_CHECK_LUMINANCE1,       m_bAryLumiKind[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE2,       m_bAryLumiKind[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE3,       m_bAryLumiKind[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE4,       m_bAryLumiKind[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE5,       m_bAryLumiKind[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE6,       m_bAryLumiKind[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE7,       m_bAryLumiKind[LUMINANCE_TYPE_TFSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE8,       m_bAryLumiKind[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Check(pDX, IDC_CHECK_LUMINANCE9,       m_bAryLumiKind[LUMINANCE_TYPE_TDILUMINANCE]);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME1, m_nAryLumiDefCount[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME2, m_nAryLumiDefCount[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME3, m_nAryLumiDefCount[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME4, m_nAryLumiDefCount[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME5, m_nAryLumiDefCount[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME6, m_nAryLumiDefCount[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME7, m_nAryLumiDefCount[LUMINANCE_TYPE_TFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME8, m_nAryLumiDefCount[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_TIME9, m_nAryLumiDefCount[LUMINANCE_TYPE_TDILUMINANCE]);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT1, m_nAryLumiAlarmCount[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT2, m_nAryLumiAlarmCount[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT3, m_nAryLumiAlarmCount[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT4, m_nAryLumiAlarmCount[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT5, m_nAryLumiAlarmCount[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT6, m_nAryLumiAlarmCount[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT7, m_nAryLumiAlarmCount[LUMINANCE_TYPE_TFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT8, m_nAryLumiAlarmCount[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT9, m_nAryLumiAlarmCount[LUMINANCE_TYPE_TDILUMINANCE]);

	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT1, m_nAryLumiTerm[LUMINANCE_TYPE_CFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT2, m_nAryLumiTerm[LUMINANCE_TYPE_CBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT3, m_nAryLumiTerm[LUMINANCE_TYPE_CFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT4, m_nAryLumiTerm[LUMINANCE_TYPE_CBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT5, m_nAryLumiTerm[LUMINANCE_TYPE_TFCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT6, m_nAryLumiTerm[LUMINANCE_TYPE_TBCLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT7, m_nAryLumiTerm[LUMINANCE_TYPE_TFSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT8, m_nAryLumiTerm[LUMINANCE_TYPE_TBSLUMINANCE]);
	DDX_Text(pDX, IDC_EDIT_OVERLAP_LUMINANCE_COUNT9, m_nAryLumiTerm[LUMINANCE_TYPE_TDILUMINANCE]);
}

//Time OverLap Parameter 저장 .  Def 관련 항목들은 Param2 Tab에서 저장한다.
void CDlgDEFSetupParam2::m_fnSaveParameter( CString *strDefArr )
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int i = 0;
	CString strTemp = _T(""), strDefectCheck = _T("");

	UpdateData(TRUE);

	//2019.03.21 saveparam2
	strTemp.Format(_T("%d"), m_bUseBgrade);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_CHECK, strTemp);
	strTemp.Format(_T("%d"), m_nBgradeSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_SETTING_COUNT, strTemp);
	strTemp.Format(_T("%0.1f"), float(m_fBgradeTerm));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_TERM, strTemp);
	strTemp.Format(_T("%0.1f"), float(m_nBgradeStopCount));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_STOP_COUNT	, strTemp);

	strTemp.Format(_T("%d"), m_bUseCgrade);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_CHECK, strTemp);
	strTemp.Format(_T("%d"), m_nCgradeSetCount);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_SETTING_COUNT, strTemp);
	strTemp.Format(_T("%0.1f"), float(m_fCgradeTerm));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_TERM, strTemp);
	strTemp.Format(_T("%0.1f"), float(m_nCgradeStopCount));
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_STOP_COUNT	, strTemp);

	// QMS REPORT NOMAL DEFECT
	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_TWO; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefKind[i]);

		if (i == MAX_DEFECT_KIND_PARAM_UI_TWO - 1)	strDefArr[TIME_OVERLAP_CHECK].Append(strDefectCheck);
		else										strDefArr[TIME_OVERLAP_CHECK].Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_CHECK, strDefArr[TIME_OVERLAP_CHECK]);
	
	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_TWO; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryDefCount[i]);

		if (i == MAX_DEFECT_KIND_PARAM_UI_TWO - 1)	strDefArr[TIME_OVERLAP_STOP_COUNT].Append(strDefectCheck);
		else										strDefArr[TIME_OVERLAP_STOP_COUNT].Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_COUNT, strDefArr[TIME_OVERLAP_STOP_COUNT]);

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_TWO; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryDefTerm[i]));

		if (i == MAX_DEFECT_KIND_PARAM_UI_TWO - 1)	strDefArr[TIME_OVERLAP_TERM].Append(strDefectCheck);
		else										strDefArr[TIME_OVERLAP_TERM].Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_TERM, strDefArr[TIME_OVERLAP_TERM]);

	for (i = 0; i < MAX_DEFECT_KIND_PARAM_UI_TWO; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryDefAlarmCount[i]));

		if (i == MAX_DEFECT_KIND_PARAM_UI_TWO - 1)	strDefArr[TIME_OVERLAP_ALARM_COUNT].Append(strDefectCheck);
		else										strDefArr[TIME_OVERLAP_ALARM_COUNT].Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_STOP_COUNT, strDefArr[TIME_OVERLAP_ALARM_COUNT]);

	// LUMINANCE
	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_bAryLumiKind[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_CHECK, strTemp);

	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryLumiDefCount[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_DEFECT_COUNT, strTemp);

	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryLumiTerm[i]));

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_TERM, strTemp);

	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryLumiAlarmCount[i]));

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_STOP_COUNT, strTemp);


	// Bubble Station //2020.02.28 add by mby
	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_bAryBubbleStationKind[i]);

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_CHECK, strTemp);

	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), m_nAryBubbleStationDefCount[i]);

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_DEFECT_COUNT, strTemp);

	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%0.2f"), float(m_nAryBubbleStationTerm[i]));  //0.1f -> 0.2f 2018.12.28 BY RYU

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_TERM, strTemp);

	strTemp = _T("");
	for (int i = 0; i < BUBBLESTATION_MAX; i++)
	{
		strDefectCheck.Format(_T("%0.1f"), float(m_nAryBubbleStationAlarmCount[i]));

		if (i == BUBBLESTATION_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_STOP_COUNT, strTemp);
}

//Time OverLap Parameter 저장 .  Def 관련 항목들은 Param1 Tab에서 로드한다.
void CDlgDEFSetupParam2::m_fnLoadParameter( int *nDefArrCurPos )
{
	CString strTemp = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	TCHAR strBuffer[255];
	CString strLine = _T(""), strDefectCheck = _T("");
	int nCurPos = 0, i = 0;
	int nDefectCnt = 0;
	CString resTokenKind = _T(""), resTokenCount = _T(""), resTokenTime = _T(""), resTokenAlarmCount = _T("");

	//2019.03.21
	m_bUseBgrade			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_CHECK, 0);
	m_nBgradeSetCount    =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_SETTING_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_TERM, _T(""), strBuffer,255);
	m_fBgradeTerm		= _ttof(strBuffer);
	m_nBgradeStopCount	=	pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_STOP_COUNT, 0);

	m_bUseCgrade			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_CHECK, 0);
	m_nCgradeSetCount    =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_SETTING_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_TERM, _T(""), strBuffer,255);
	m_fCgradeTerm		= _ttof(strBuffer);
	m_nCgradeStopCount	=	pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_STOP_COUNT, 0);


	// QMS REPORT , NOMAL DATA
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	//nDefArrCurPos[TIME_OVERLAP_CHECK] = nDefArrCurPos[TIME_OVERLAP_CHECK] - 4;
	nDefArrCurPos[TIME_OVERLAP_CHECK] = nDefArrCurPos[TIME_OVERLAP_CHECK] - 3;
	if( nDefArrCurPos[TIME_OVERLAP_CHECK] >= 0)
		resTokenKind = strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_CHECK]);

	for(i=MAX_DEFECT_KIND_PARAM_UI_ONE; i<MAX_DEFECT_KIND; i++)
	{		
		if( resTokenKind == _T("") )
			break;

		m_nAryDefKind[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_CHECK]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	//nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] = nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] - 4;
	nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] = nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] - 3;
	if( nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT] >= 0)
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT]);
	for(i=MAX_DEFECT_KIND_PARAM_UI_ONE; i<MAX_DEFECT_KIND; i++)
	{		
		if( resTokenCount == _T("") )
			break;

		m_nAryDefCount[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON,  nDefArrCurPos[TIME_OVERLAP_ALARM_COUNT]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_TERM, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	nDefArrCurPos[TIME_OVERLAP_TERM] = nDefArrCurPos[TIME_OVERLAP_TERM] - 4;
	if( nDefArrCurPos[TIME_OVERLAP_TERM] >= 0)
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_TERM]);
	for(i=MAX_DEFECT_KIND_PARAM_UI_ONE; i<MAX_DEFECT_KIND; i++)
	{		
		if( resTokenTime == _T("") )
			break;

		m_nAryDefTerm[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_TERM]);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_STOP_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nDefectCnt = 0;
	nDefArrCurPos[TIME_OVERLAP_STOP_COUNT] = nDefArrCurPos[TIME_OVERLAP_STOP_COUNT] - 4;
	if( nDefArrCurPos[TIME_OVERLAP_STOP_COUNT] >= 0)
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_STOP_COUNT]);
	for(i=MAX_DEFECT_KIND_PARAM_UI_ONE; i<MAX_DEFECT_KIND; i++)
	{		
		if( resTokenAlarmCount == _T("") )
			break;

		m_nAryDefAlarmCount[nDefectCnt] = _ttof(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nDefArrCurPos[TIME_OVERLAP_STOP_COUNT]);
		nDefectCnt++;
	}



	//////////////////////////////////////////////////////////////////////////
	//LUMINANCE DATA
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_CHECK, _T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	resTokenKind = _T("");
	resTokenKind = strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{		
		m_bAryLumiKind[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_DEFECT_COUNT,_T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount = _T("");
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{		
		m_nAryLumiDefCount[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_TERM, _T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenTime = _T("");
	resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenTime != "")
	{		
		m_nAryLumiTerm[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_STOP_COUNT, _T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenAlarmCount = _T("");
	resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenAlarmCount != "")
	{		
		m_nAryLumiAlarmCount[nDefectCnt] = _ttof(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

#if 1
	//////////////////////////////////////////////////////////////////////////
	//BUBBLESTATION //2020.02.28 add by mby
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_CHECK, _T("0,0,0,0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenKind = _T("");
	resTokenKind = strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{		
		m_bAryBubbleStationKind[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}


	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_DEFECT_COUNT,_T("0,0,0,0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount = _T("");
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{		
		m_nAryBubbleStationDefCount[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_TERM, _T("0,0,0,0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenTime = _T("");
	resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenTime != "")
	{		
		m_nAryBubbleStationTerm[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_STOP_COUNT, _T("0,0,0,0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenAlarmCount = _T("");
	resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenAlarmCount != "")
	{		
		m_nAryBubbleStationAlarmCount[nDefectCnt] = _ttof(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
#endif
	UpdateData(FALSE);
}

void CDlgDEFSetupParam2::m_fnOverlapUseCheck(BOOL bUseCheck, int nEqpModel)
{
	GetDlgItem(IDC_CHECK_LUMINANCE1)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE2)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE3)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE4)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE5)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE6)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE7)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_LUMINANCE8)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_CHECK_DEFECT35)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT36)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT37)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT38)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT39)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT40)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT41)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT42)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT43)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT44)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT45)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT46)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT47)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT48)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT49)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_CHECK_DEFECT50)->EnableWindow(bUseCheck);
	//2019.05.28 --E
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT1)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT2)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT3)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT4)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT5)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT6)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT7)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT8)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_EDIT_OVERLAP_TIME35)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME36)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME37)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME38)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME39)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME40)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME41)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME42)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME43)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME44)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME45)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME46)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME47)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME48)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME49)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_TIME50)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME1)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME2)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME3)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME4)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME5)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME6)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME7)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME8)->EnableWindow(bUseCheck);
	
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT35)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT36)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_B_GRADE_COUNT)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT38)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT39)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT40)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT41)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT42)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT43)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT44)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT45)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT46)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT47)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT48)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT49)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_ALARM_COUNT50)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT1)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT2)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT3)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT4)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT5)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT6)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT7)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT8)->EnableWindow(bUseCheck);

	GetDlgItem(IDC_EDIT_OVERLAP_COUNT35)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT36)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT37)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT38)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT39)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT40)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT41)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT42)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT43)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT44)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT45)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT46)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT47)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT48)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT49)->EnableWindow(bUseCheck);
	GetDlgItem(IDC_EDIT_OVERLAP_COUNT50)->EnableWindow(bUseCheck);
	if (nEqpModel == EQP_TYPE_ABI)
	{
		GetDlgItem(IDC_CHECK_LUMINANCE9)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_COUNT9)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_TIME9)->EnableWindow(bUseCheck);

		GetDlgItem(IDC_EDIT_OVERLAP_LUMINANCE_ALARM_COUNT9)->EnableWindow(bUseCheck);
	}
}




///////////////////////////////////////////////////////////////////////////
//2015.06.02 e-


IMPLEMENT_DYNAMIC(CDlgDEFSetup, CDialogEx)

CDlgDEFSetup::CDlgDEFSetup(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDEFSetup::IDD, pParent)
{
	m_bUseOverlap = FALSE;
}

CDlgDEFSetup::~CDlgDEFSetup()
{
	if(m_DlgTimeParam_One)
	{
		delete m_DlgTimeParam_One;
		m_DlgTimeParam_One = NULL;
	}
	if(m_DlgTimeParam_Two)
	{
		delete m_DlgTimeParam_Two;
		m_DlgTimeParam_Two = NULL;
	}
	if(m_strArrDefItem)
	{
		delete m_strArrDefItem;
		m_strArrDefItem = NULL;
	}

}

void CDlgDEFSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_OVERLAP_USE,   m_bUseOverlap);	
// 	DDX_Check(pDX, IDC_CHECK_DEFECT1,       m_nAryDefKind[INSP_DUST]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT2,       m_nAryDefKind[INSP_WHITE_TURBIDITY]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT3,       m_nAryDefKind[INSP_ITO]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT4,       m_nAryDefKind[INSP_EDGE_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT5,       m_nAryDefKind[INSP_PAD_SCRATCH]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT6,       m_nAryDefKind[INSP_DIMPLE_POINT]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT7,       m_nAryDefKind[INSP_ETCHING_LINE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT8,       m_nAryDefKind[INSP_DIMPLE_LINE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT9,       m_nAryDefKind[INSP_PAD_BURNT]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT10,      m_nAryDefKind[INSP_BURR]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT11,      m_nAryDefKind[INSP_POL_BUBBLE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT12,      m_nAryDefKind[INSP_POL_SCRATCH]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT13,      m_nAryDefKind[INSP_POL_ALIEN_SUBSTANCE ]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT14,      m_nAryDefKind[INSP_POL_MINOR]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT15,      m_nAryDefKind[INSP_BLACK_SCRATCH]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT16,      m_nAryDefKind[INSP_WHITE_SCRATCH]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT17,      m_nAryDefKind[INSP_CF_MEASURE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT18,      m_nAryDefKind[INSP_TFT_MEASURE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT19,      m_nAryDefKind[INSP_MURA]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT20,      m_nAryDefKind[INSP_NO_USE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT21,      m_nAryDefKind[INSP_CF_BURR]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT22,      m_nAryDefKind[INSP_TFT_BURR]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT23,      m_nAryDefKind[INSP_CF_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT24,      m_nAryDefKind[INSP_TFT_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT25,      m_nAryDefKind[INSP_CF_CORNER_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT26,      m_nAryDefKind[INSP_TFT_CORNER_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT27,      m_nAryDefKind[INSP_CF_CELL_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT28,      m_nAryDefKind[INSP_TFT_CELL_BROKEN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT29,      m_nAryDefKind[INSP_PIMPLE_POINT]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT30,      m_nAryDefKind[INSP_PIMPLE_LINE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT31,      m_nAryDefKind[INSP_CF_PIMPLE_CIRCLE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT32,      m_nAryDefKind[INSP_CF_WAVE_PATTERN]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT33,      m_nAryDefKind[INSP_ETC_LINE]);
// 	DDX_Check(pDX, IDC_CHECK_DEFECT34,      m_nAryDefKind[INSP_ETC_POINT]);
// 	DDX_Check(pDX, IDC_CHECK_CONTACT_FAIL,  m_bUseContactFail);
// 	DDX_Check(pDX, IDC_CHECK_POL_MISS,		m_bUsePolMiss);	//2016.02.24
// 	DDX_Check(pDX, IDC_CHECK_ALIGN_FAIL,	m_bUseAlignFail);
// 
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT1,  m_nAryDefCount[INSP_DUST]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT2,  m_nAryDefCount[INSP_WHITE_TURBIDITY]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT3,  m_nAryDefCount[INSP_ITO]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT4,  m_nAryDefCount[INSP_EDGE_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT5,  m_nAryDefCount[INSP_PAD_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT6,  m_nAryDefCount[INSP_DIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT7,  m_nAryDefCount[INSP_ETCHING_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT8,  m_nAryDefCount[INSP_DIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT9,  m_nAryDefCount[INSP_PAD_BURNT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT10, m_nAryDefCount[INSP_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT11, m_nAryDefCount[INSP_POL_BUBBLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT12, m_nAryDefCount[INSP_POL_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT13, m_nAryDefCount[INSP_POL_ALIEN_SUBSTANCE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT14, m_nAryDefCount[INSP_POL_MINOR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT15, m_nAryDefCount[INSP_BLACK_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT16, m_nAryDefCount[INSP_WHITE_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT17, m_nAryDefCount[INSP_CF_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT18, m_nAryDefCount[INSP_TFT_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT19, m_nAryDefCount[INSP_MURA]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT20, m_nAryDefCount[INSP_CONTACT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT21, m_nAryDefCount[INSP_CF_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT22, m_nAryDefCount[INSP_TFT_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT23, m_nAryDefCount[INSP_CF_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT24, m_nAryDefCount[INSP_TFT_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT25, m_nAryDefCount[INSP_CF_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT26, m_nAryDefCount[INSP_TFT_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT27, m_nAryDefCount[INSP_CF_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT28, m_nAryDefCount[INSP_TFT_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT29,m_nAryDefCount[INSP_PIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT30,m_nAryDefCount[INSP_PIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT31,m_nAryDefCount[INSP_CF_PIMPLE_CIRCLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT32,m_nAryDefCount[INSP_CF_WAVE_PATTERN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT33,m_nAryDefCount[INSP_ETC_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_COUNT34,m_nAryDefCount[INSP_ETC_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_CONTACT_FAIL_COUNT, m_nContactFailStopCount);
// 	DDX_Text(pDX, IDC_EDIT_POL_MISS_COUNT, m_nPolMissStopCount);	//2016.02.24
// 	DDX_Text(pDX, IDC_EDIT_ALIGN_FAIL_COUNT, m_nAlignFailStopCount);
// 
// 
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT1,  m_nAryDefAlarmCount[INSP_DUST]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT2,  m_nAryDefAlarmCount[INSP_WHITE_TURBIDITY]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT3,  m_nAryDefAlarmCount[INSP_ITO]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT4,  m_nAryDefAlarmCount[INSP_EDGE_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT5,  m_nAryDefAlarmCount[INSP_PAD_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT6,  m_nAryDefAlarmCount[INSP_DIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT7,  m_nAryDefAlarmCount[INSP_ETCHING_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT8,  m_nAryDefAlarmCount[INSP_DIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT9,  m_nAryDefAlarmCount[INSP_PAD_BURNT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT10, m_nAryDefAlarmCount[INSP_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT11, m_nAryDefAlarmCount[INSP_POL_BUBBLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT12, m_nAryDefAlarmCount[INSP_POL_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT13, m_nAryDefAlarmCount[INSP_POL_ALIEN_SUBSTANCE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT14, m_nAryDefAlarmCount[INSP_POL_MINOR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT15, m_nAryDefAlarmCount[INSP_BLACK_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT16, m_nAryDefAlarmCount[INSP_WHITE_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT17, m_nAryDefAlarmCount[INSP_CF_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT18, m_nAryDefAlarmCount[INSP_TFT_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT19, m_nAryDefAlarmCount[INSP_MURA]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT20, m_nAryDefAlarmCount[INSP_NO_USE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT21, m_nAryDefAlarmCount[INSP_CF_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT22, m_nAryDefAlarmCount[INSP_TFT_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT23, m_nAryDefAlarmCount[INSP_CF_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT24, m_nAryDefAlarmCount[INSP_TFT_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT25, m_nAryDefAlarmCount[INSP_CF_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT26, m_nAryDefAlarmCount[INSP_TFT_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT27, m_nAryDefAlarmCount[INSP_CF_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT28, m_nAryDefAlarmCount[INSP_TFT_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT29,m_nAryDefAlarmCount[INSP_PIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT30,m_nAryDefAlarmCount[INSP_PIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT31,m_nAryDefAlarmCount[INSP_CF_PIMPLE_CIRCLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT32,m_nAryDefAlarmCount[INSP_CF_WAVE_PATTERN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT33,m_nAryDefAlarmCount[INSP_ETC_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT34,m_nAryDefAlarmCount[INSP_ETC_POINT]);
// 
// 
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME1,   m_nAryDefTerm[INSP_DUST]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME2,   m_nAryDefTerm[INSP_WHITE_TURBIDITY]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME3,   m_nAryDefTerm[INSP_ITO]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME4,   m_nAryDefTerm[INSP_EDGE_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME5,   m_nAryDefTerm[INSP_PAD_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME6,   m_nAryDefTerm[INSP_DIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME7,   m_nAryDefTerm[INSP_ETCHING_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME8,   m_nAryDefTerm[INSP_DIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME9,   m_nAryDefTerm[INSP_PAD_BURNT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME10,  m_nAryDefTerm[INSP_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME11,  m_nAryDefTerm[INSP_POL_BUBBLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME12,  m_nAryDefTerm[INSP_POL_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME13,  m_nAryDefTerm[INSP_POL_ALIEN_SUBSTANCE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME14,  m_nAryDefTerm[INSP_POL_MINOR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME15,  m_nAryDefTerm[INSP_BLACK_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME16,  m_nAryDefTerm[INSP_WHITE_SCRATCH]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME17,  m_nAryDefTerm[INSP_CF_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME18,  m_nAryDefTerm[INSP_TFT_MEASURE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME19,  m_nAryDefTerm[INSP_MURA]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME20,  m_nAryDefTerm[INSP_NO_USE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME21,  m_nAryDefTerm[INSP_CF_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME22,  m_nAryDefTerm[INSP_TFT_BURR]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME23,  m_nAryDefTerm[INSP_CF_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME24,  m_nAryDefTerm[INSP_TFT_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME25,  m_nAryDefTerm[INSP_CF_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME26,  m_nAryDefTerm[INSP_TFT_CORNER_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME27,  m_nAryDefTerm[INSP_CF_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME28,  m_nAryDefTerm[INSP_TFT_CELL_BROKEN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME29,  m_nAryDefTerm[INSP_PIMPLE_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME30,  m_nAryDefTerm[INSP_PIMPLE_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME31,  m_nAryDefTerm[INSP_CF_PIMPLE_CIRCLE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME32,  m_nAryDefTerm[INSP_CF_WAVE_PATTERN]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME33,  m_nAryDefTerm[INSP_ETC_LINE]);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_TIME34,  m_nAryDefTerm[INSP_ETC_POINT]);
// 	DDX_Text(pDX, IDC_EDIT_CONTACT_FAIL_TIME, m_nContactFailTerm);
// 	DDX_Text(pDX, IDC_EDIT_POL_MISS_TIME, m_nPolMissTerm);	//2016.02.24
// 
// 	//DDV_MinMaxInt(pDX, m_nPanelCount, 0, 1000);
// 
// 	DDX_Text(pDX, IDC_EDIT_ALIGN_FAIL_TIME, m_fAlignFailTerm);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT37, m_nAlignFailDefectCount);
// 	//  DDX_Control(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT36, m_nPolMissDefectCount);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT36, m_nPolMissDefectCount);
// 	DDX_Text(pDX, IDC_EDIT_OVERLAP_ALARM_COUNT35, m_nContactFailDefectCount);
	DDX_Control(pDX, IDC_TAB_PARAM, m_ctrlTabTimeOverlab);
}
BEGIN_MESSAGE_MAP(CDlgDEFSetup, CDialogEx)
	
	ON_BN_CLICKED(IDC_CHECK_OVERLAP_USE, &CDlgDEFSetup::OnBnClickedCheckOverlapUse)	//2015.06.02
	ON_BN_CLICKED(IDC_BTN_SAVE, &CDlgDEFSetup::OnBnClickedBtnSave)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PARAM, &CDlgDEFSetup::OnTcnSelchangeTabParam)
END_MESSAGE_MAP()


// CDlgPLCSetup 메시지 처리기입니다.



BOOL CDlgDEFSetup::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//Setting Tab control 2016.11.21 Mr.Chu
	RECT rc;
	m_DlgTimeParam_One = new CDlgDEFSetupParam1();
	m_DlgTimeParam_One->Create(IDD_DLG_DEF_SETUP_PARAM_1, this);

	m_DlgTimeParam_Two = new CDlgDEFSetupParam2();
	m_DlgTimeParam_Two->Create(IDD_DLG_DEF_SETUP_PARAM_2, this);

	m_strArrDefItem = new CString[TIME_OVERLAP_ITEM_MAX];
	//

	SetWindowTheme(GetDlgItem(IDC_TAB_PARAM)->GetSafeHwnd(), L"", L"");

	m_ctrlTabTimeOverlab.InsertItem(1, _T("Page1"));
	m_ctrlTabTimeOverlab.InsertItem(2, _T("Page2"));

	m_DlgTimeParam_One->ShowWindow(SW_SHOW);

	m_DlgTimeParam_One->GetWindowRect(&rc);
	m_DlgTimeParam_One->GetClientRect(&rc);
	m_DlgTimeParam_One->MoveWindow(&rc);
	m_DlgTimeParam_One->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
	//

	GetDlgItem(IDC_BTN_SAVE)->ShowWindow(SW_HIDE);
	//GetDlgItem(IDC_GXBTN_PLC_CLOSE)->EnableWindow(FALSE);

	return TRUE;  
}

BEGIN_EVENTSINK_MAP(CDlgDEFSetup, CDialogEx)
//	ON_EVENT(CDlgDEFSetup, IDC_GXBTN_PLC_OPEN, DISPID_CLICK, CDlgDEFSetup::ClickGxbtnPlcOpen, VTS_NONE)
//	ON_EVENT(CDlgDEFSetup, IDC_GXBTN_PLC_CLOSE, DISPID_CLICK, CDlgDEFSetup::ClickGxbtnPlcClose, VTS_NONE)
END_EVENTSINK_MAP()


//Time OverLap Parameter 저장 .  Def 관련 ( m_nAryDefKind )은 Param2 Tab에서 로드한다.
void CDlgDEFSetup::m_fnLoadParameter()
{
	CString strTemp;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strLine, strDefectCheck;
	int nCurPos[TIME_OVERLAP_ITEM_MAX] = {0,};
	int nDefectCnt = 0;

	m_bUseOverlap = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_USEMODE, 0);

	for(int i=0;i<TIME_OVERLAP_ITEM_MAX;i++)
		m_strArrDefItem[i].Format(_T(""));

	// Data merge in 'm_strArrDefItem' and Defect Parameter Load at 'm_DlgTimeParam_One'
	m_DlgTimeParam_One->m_fnLoadParameter( nCurPos );
	m_DlgTimeParam_Two->m_fnLoadParameter( nCurPos );

	m_fnOverlapUseCheck(m_bUseOverlap);

	UpdateData(FALSE);
}


void CDlgDEFSetup::m_fnSaveParameter()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strTemp, strDefectCheck;

	UpdateData(TRUE);

	strTemp.Format(_T("%d"), m_bUseOverlap);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_USEMODE, strTemp);

	for(int i=0;i<TIME_OVERLAP_ITEM_MAX;i++)
		m_strArrDefItem[i].Format(_T(""));

	// Data merge in 'm_strArrDefItem' and Defect Parameter Save at 'm_DlgTimeParam_Two'
	m_DlgTimeParam_One->m_fnSaveParameter( m_strArrDefItem );
	m_DlgTimeParam_Two->m_fnSaveParameter( m_strArrDefItem );

}

void CDlgDEFSetup::m_fnOverlapUseCheck(BOOL bUseCheck)
{
	int nEqpModel;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	
	//0:AVL 1:ABI
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	m_DlgTimeParam_One->m_fnOverlapUseCheck(bUseCheck,nEqpModel);
	m_DlgTimeParam_Two->m_fnOverlapUseCheck(bUseCheck,nEqpModel);
}

void CDlgDEFSetup::OnBnClickedCheckOverlapUse()
{
	BOOL bCheck = ((CButton*)GetDlgItem(IDC_CHECK_OVERLAP_USE))->GetCheck();
	m_fnOverlapUseCheck(bCheck);
}


void CDlgDEFSetup::OnBnClickedBtnSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_fnSaveParameter();
}


//2016.11.21	Mr.Chu
void CDlgDEFSetup::OnTcnSelchangeTabParam(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RECT rc;
	int nIndex = m_ctrlTabTimeOverlab.GetCurSel();

	m_DlgTimeParam_One->ShowWindow(SW_HIDE);
	m_DlgTimeParam_Two->ShowWindow(SW_HIDE);

	switch(nIndex)
	{
	case TAPCTRL_PAGE1:
		m_DlgTimeParam_One->ShowWindow(SW_SHOW);

		m_DlgTimeParam_One->GetWindowRect(&rc);
		m_DlgTimeParam_One->GetClientRect(&rc);
		m_DlgTimeParam_One->MoveWindow(&rc);
		m_DlgTimeParam_One->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
		break;
	case TAPCTRL_PAGE2:
		m_DlgTimeParam_Two->ShowWindow(SW_SHOW);

		m_DlgTimeParam_Two->GetWindowRect(&rc);
		m_DlgTimeParam_Two->GetClientRect(&rc);
		m_DlgTimeParam_Two->MoveWindow(&rc);
		m_DlgTimeParam_Two->MoveWindow(rc.left, rc.top + 60, rc.right, rc.bottom);
		break;
	}

	*pResult = 0;
}
