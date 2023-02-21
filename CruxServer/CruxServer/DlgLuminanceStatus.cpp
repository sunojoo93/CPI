#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgLuminanceStatus.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgLuminanceStatus.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"

// CDlgLuminanceStatus 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLuminanceState, CDialogEx)

CDlgLuminanceState::CDlgLuminanceState(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLuminanceState::IDD, pParent)
{
	memset(m_nMin, 0, sizeof(m_nMin));
	memset(m_nMax, 0, sizeof(m_nMax));
}

CDlgLuminanceState::~CDlgLuminanceState()
{
}

void CDlgLuminanceState::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CDlgLuminanceState, CDialogEx)

END_MESSAGE_MAP()


// CDlgLuminanceStatus 메시지 처리기입니다.


BOOL CDlgLuminanceState::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	int nColumnCount;
	int nRowCount;

	// Log Grid 생성
	CRect rect;
	GetDlgItem(IDC_GRID_LUM_STATE)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridLuminanceState.Create(rect, this, IDC_GRID_LUM_STATE);
	m_GridLuminanceState.SetParent(this);
	m_GridLuminanceState.SetListMode(true);
	m_GridLuminanceState.SetEditable(FALSE);
	m_GridLuminanceState.EnableSelection(FALSE);
	//m_GridLuminanceState.SetHeaderSort();
	m_GridLuminanceState.SetFixedBkColor(RGB(220, 240, 220));
	m_GridLuminanceState.SetBkColor(RGB(200, 200, 200));
	m_GridLuminanceState.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridLuminanceState.SetFixedRowCount(1);

	//CString colText[5][5] = { _T(" "), _T("Cam"), _T("Value"), _T("Min"), _T("Max") ,
	//	_T("CF"), _T("Cam1"), _T("0"), _T("80"), _T("200") ,
	//	_T("CF"), _T("Cam2"), _T("0"), _T("80"), _T("200") ,
	//	_T("TFT"), _T("Cam1"), _T("0"), _T("80"), _T("200") ,
	//	_T("TFT"), _T("Cam2"), _T("0"), _T("80"), _T("200") };
	CString colText[11][5] = { _T(" "), _T("Cam"), _T("Value"), _T("Min"), _T("Max") ,
							_T("TFT"), _T("Cam1"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam2"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam3"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam4"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam5"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam6"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam7"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam8"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam9"), _T("0"), _T("80"), _T("200") ,
							_T("TFT"), _T("Cam10"), _T("0"), _T("80"), _T("200") };

	nColumnCount = 5;
	nRowCount = 11;	//2015.05.11 edit 5 -> 11
	m_GridLuminanceState.SetColumnCount(nColumnCount);
	m_GridLuminanceState.SetRowCount(nRowCount);

	DWORD dwTextStyle = DT_CENTER|DT_VCENTER|DT_SINGLELINE;

	for(int row = 0; row < nRowCount; row++)
	{
		for(int Col = 0; Col < nColumnCount; Col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = Col;
			Item.nFormat = dwTextStyle;
			Item.strText.Format(_T("%s"),colText[row][Col]);
			m_GridLuminanceState.SetItem(&Item);  
		}
	}

	

	m_GridLuminanceState.ExpandColumnsToFit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 함수 설명	: 파라메터로 저장된 휘도 리밋 값을 화면에 표시
// Return		: 0 - OK, 그외 에러
int CDlgLuminanceState::m_fnGetLimitValue()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strTemp[20];	//2015.05.11 edit 8 -> 20
	int nRead = 0, cnt = 0, camCnt = 0, nRet = 0;

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_1_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_2_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_3_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_4_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	//2015.05.11 add s-
	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_5_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_5_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_6_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_6_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_7_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_7_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_8_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_8_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_9_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_9_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_10_MIN, 0);
	m_nMin[camCnt] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);

	nRead = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LUMINANCE_CAM_10_MAX, 0);
	m_nMax[camCnt++] = nRead;
	strTemp[cnt++].Format(_T("%d"), nRead);
	//2015.05.11 add e-

	cnt = 0;

	DWORD dwTextStyle = DT_CENTER|DT_VCENTER|DT_SINGLELINE;

	for(int row = 1; row < 11; row++)	//2015.05.11 edit 5 -> 11
	{
		for(int Col = 3; Col < 5; Col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = Col;
			Item.nFormat = dwTextStyle;
			Item.strText = strTemp[cnt++];
			m_GridLuminanceState.SetItem(&Item);  
		}
	}
	return nRet;
}

// 함수 설명	: 보고된 휘도 값을 화면에 표시, 알람 발생
// Return		: 0 - OK, 그외 에러
int CDlgLuminanceState::m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue)
{
	int nRet = 0;

	//GV_ITEM Item;
	//DWORD dwTextStyle = DT_CENTER|DT_VCENTER|DT_SINGLELINE;

	//Item.mask = GVIF_TEXT|GVIF_FORMAT;
	//Item.row = nCamNo;
	//Item.col = 2;
	//Item.nFormat = dwTextStyle;
	//Item.strText.Format(_T("%d"), nValue);
	//m_GridLuminanceState.SetItem(&Item);

	//if(nValue < m_nMin[nCamNo-1] || nValue > m_nMax[nCamNo-1])
	//{
	//	m_GridLuminanceState.SetItemBkColour(nCamNo, 2, RGB(255,0,0));
	//	m_fnSendAlarmToPLC(nVisionID, nCamNo);
	//}
	//else
	//{
	//	m_GridLuminanceState.SetItemBkColour(nCamNo, 2, COLOR_ENABLE);
	//}

	return nRet;
}

// 함수 설명	: 휘도 관련 알람을 PLC 전달. 추후 구현 필요. Todo
// Return		: 0 - OK, 그외 에러
int CDlgLuminanceState::m_fnSendAlarmToPLC(int nVisionID, int nCamNo)
{
	int nRet = 0;

	/*CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if(nVisionID == VISION_PC_1)
	{
		if(nCamNo == 0)
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S1_SIDE, 1);
		else
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S1_COAX, 1);
	}
	else if(nVisionID == VISION_PC_2)
	{
		if(nCamNo == 0)
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S2_SIDE, 1);
		else
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S2_COAX, 1);
	}
	else
	{
		if(nCamNo == 0)
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S3_TR1, 1);
		else
			((CViewSetup*)(pFrame->m_pViewSetup))->m_pDlgPLC->m_fnSetDevice(ADDR_ALARM_S3_TR2, 1);
	}*/

	return nRet;
}