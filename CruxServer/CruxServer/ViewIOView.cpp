// ViewIOView.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ViewIOView.h"
#include "MainFrm.h"
#include "ViewSetup.h"
// CViewIOView


//DEFINE_PLC_ADDR_ENUM(testsdjdidm, 12)
IMPLEMENT_DYNCREATE(CViewIOView, CFormView)

CViewIOView::CViewIOView()
	: CFormView(CViewIOView::IDD)
{

}

CViewIOView::~CViewIOView()
{
}

void CViewIOView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewIOView, CFormView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CViewIOView 진단입니다.

#ifdef _DEBUG
void CViewIOView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewIOView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewIOView 메시지 처리기입니다.

void CViewIOView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	
	//wchar_t* testtest2[]  = //{L"test1",L"test2",L"test3"};
	//{
	//#define	PLDADRR(a,b,c) L#b,
	//#include "PLC_ADDR.inc"
	//#undef PLDADRR
	//};

	//wchar_t* testtest3[]  = //{L"test1",L"test2",L"test3"};
	//{
	//	#define	PLDADRR(a,b,c) L#c,
	//	#include "PLC_ADDR.inc"
	//	#undef PLDADRR
	//};

	m_fnInitGridPCSideIO();
	m_fnInitGridPLCSideIO();
	m_fnInitGridCIMSideIO();
}

void CViewIOView::m_fnInitGridPCSideIO()
{
	int nColumnCount;

	CRect rect;
	GetDlgItem(IDC_GRID_MAP_PC)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridPCSideIO.Create(rect, this, IDC_GRID_MAP_PC);
	m_GridPCSideIO.SetParent(this);
	m_GridPCSideIO.SetListMode(true);
	m_GridPCSideIO.SetEditable(FALSE);
//	m_GridPCSideIO.SetHeaderSort();
	m_GridPCSideIO.SetFixedBkColor(RGB(220, 240, 220));
	m_GridPCSideIO.SetBkColor(RGB(200, 200, 200));
	m_GridPCSideIO.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridPCSideIO.SetFixedRowCount(1);

	CString colNameMap[5] = { _T("No"), _T("Item"), _T("Addr"), _T("Value"), _T("Size") };
	nColumnCount = 5;
	m_GridPCSideIO.SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		if(k == 0)		m_GridPCSideIO.SetColumnWidth(k, 40);
		else if(k == 1) m_GridPCSideIO.SetColumnWidth(k, 130);
		else if(k == 2) m_GridPCSideIO.SetColumnWidth(k, 60);
		else if(k == 3)	m_GridPCSideIO.SetColumnWidth(k, 120);
		else			m_GridPCSideIO.SetColumnWidth(k, 40);
		m_GridPCSideIO.SetItemText(0, k, colNameMap[k]);
	}
}

void CViewIOView::m_fnInitGridPLCSideIO()
{
	int nColumnCount;

	CRect rect;
	GetDlgItem(IDC_GRID_MAP_PLC)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridPLCSideIO.Create(rect, this, IDC_GRID_MAP_PLC);
	m_GridPLCSideIO.SetParent(this);
	m_GridPLCSideIO.SetListMode(true);
	m_GridPLCSideIO.SetEditable(FALSE);
//	m_GridPLCSideIO.SetHeaderSort();
	m_GridPLCSideIO.SetFixedBkColor(RGB(220, 240, 220));
	m_GridPLCSideIO.SetBkColor(RGB(200, 200, 200));
	m_GridPLCSideIO.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridPLCSideIO.SetFixedRowCount(1);

	CString colNameMap[5] = { _T("No"), _T("Item"), _T("Addr"), _T("Value"), _T("Size") };
	nColumnCount = 5;
	m_GridPLCSideIO.SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		if(k == 0)		m_GridPLCSideIO.SetColumnWidth(k, 40);
		else if(k == 1) m_GridPLCSideIO.SetColumnWidth(k, 130);
		else if(k == 2) m_GridPLCSideIO.SetColumnWidth(k, 60);
		else if(k == 3)	m_GridPLCSideIO.SetColumnWidth(k, 120);
		else			m_GridPLCSideIO.SetColumnWidth(k, 40);
		m_GridPLCSideIO.SetItemText(0, k, colNameMap[k]);
	}
}



void CViewIOView::m_fnInitGridCIMSideIO(void)
{
	int nColumnCount;

	CRect rect;
	GetDlgItem(IDC_GRID_MAP_CIM)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridCIMSideIO.Create(rect, this, IDC_GRID_MAP_CIM);
	m_GridCIMSideIO.SetParent(this);
	m_GridCIMSideIO.SetListMode(true);
	m_GridCIMSideIO.SetEditable(FALSE);
	//m_GridCIMSideIOdeIO.SetHeaderSort();
	m_GridCIMSideIO.SetFixedBkColor(RGB(220, 240, 220));
	m_GridCIMSideIO.SetBkColor(RGB(200, 200, 200));
	m_GridCIMSideIO.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridCIMSideIO.SetFixedRowCount(1);

	CString colNameMap[5] = { _T("No"), _T("Item"), _T("Addr"), _T("Value"), _T("Size") };
	nColumnCount = 5;
	m_GridCIMSideIO.SetColumnCount(nColumnCount);

	for(int k = 0; k < nColumnCount; k++)
	{
		if(k == 0)		m_GridCIMSideIO.SetColumnWidth(k, 40);
		else if(k == 1) m_GridCIMSideIO.SetColumnWidth(k, 130);
		else if(k == 2) m_GridCIMSideIO.SetColumnWidth(k, 60);
		else if(k == 3)	m_GridCIMSideIO.SetColumnWidth(k, 120);
		else			m_GridCIMSideIO.SetColumnWidth(k, 40);
		m_GridCIMSideIO.SetItemText(0, k, colNameMap[k]);
	}
}


void CViewIOView::m_fnUpdateGrid(CGridCtrl* pGridCtrl, int nIndex, CString strData)
{
	int nCol = 1;
	int curPos = 0;

	int nColCount = pGridCtrl->GetColumnCount();

	CString resToken= strData.Tokenize(EXT_COLON, curPos);
	while (resToken != "")
	{
		resToken.Replace(_T("ADDR_"), _T(""));
		pGridCtrl->SetItemText(nIndex, nCol++, resToken);
		if (nCol+1 > nColCount)
			break;
		resToken= strData.Tokenize(EXT_COLON, curPos);
	}

	pGridCtrl->Refresh();
}

void CViewIOView::m_fnLoadMapConfig()
{
	CString strTemp, strKey, strData;
	wchar_t chRead[255] = {0};
	int nRowCount = 0;
	int nIndex = 0;
	int plcUnitCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	wchar_t strUnit[2] = {0};
	m_nAddrStartPC	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_START_PC, 0);
	m_nAddrEndPC	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_END_PC, 0);
	m_nAddrStartPLC = pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_START_PLC, 0);
	m_nAddrEndPLC	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_END_PLC, 0);
	m_nItemCntPC	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ITEM_COUNT_PC, 0);
	m_nItemCntPLC	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ITEM_COUNT_PLC, 0);
	m_nAddrStartCim	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_START_CIM, 0);
	m_nAddrEndCim	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ADDR_END_CIM, 0);
	m_nItemCntCim	= pFrame->m_iniConfigPLCMap.GetProfileIntW(SECTION_CONFIG, KEY_ITEM_COUNT_CIM, 0);

	nRowCount = m_nAddrEndPC - m_nAddrStartPC + 1;
	m_GridPCSideIO.SetRowCount(nRowCount + 1);

	
	for(int i = 0; i < nRowCount + 1; i++)
	{
		strTemp.Format(_T("%d"), i + 1);
		m_GridPCSideIO.SetItemText(i + 1, 0, strTemp);

		strTemp.Format(_T("%d"), m_nAddrStartPC + i);
		m_GridPCSideIO.SetItemText(i + 1, 2, strTemp);
	}

	nRowCount = m_nAddrEndPLC - m_nAddrStartPLC + 1;
	m_GridPLCSideIO.SetRowCount(nRowCount + 1);

	for(int i = 0; i < nRowCount + 1; i++)
	{
		strTemp.Format(_T("%d"), i + 1);
		m_GridPLCSideIO.SetItemText(i + 1, 0, strTemp);
		strTemp.Format(_T("%d"), m_nAddrStartPLC + i);
		m_GridPLCSideIO.SetItemText(i + 1, 2, strTemp);
	}

	nRowCount = m_nAddrEndCim - m_nAddrStartCim + 1;
	m_GridCIMSideIO.SetRowCount(nRowCount + 1);

	for(int i = 0; i < nRowCount + 1; i++)
	{
		strTemp.Format(_T("%d"), i + 1);
		m_GridCIMSideIO.SetItemText(i + 1, 0, strTemp);
		strTemp.Format(_T("%d"), m_nAddrStartCim + i);
		m_GridCIMSideIO.SetItemText(i + 1, 2, strTemp);
	}
//
//	wchar_t* tettmp;
//	
//	tettmp = PLC_ADDR_READ()  ;
//
//
//	TCHAR * tTemp[] = 
//	{
//#define	PLDADRR(a,b,c) L#a,L#b,L#c,
//#include "PLC_ADDR.inc"
//#undef PLDADRR
//
//	};
//
	//G_PLC_READ();
	for(int i = 0; i < m_nItemCntPC; i++)
	{
		strKey.Format(_T("%d"), i + 1);

		pFrame->m_iniConfigPLCMap.GetProfileStringW(SECTION_PC_SIDE, strKey, _T(""), chRead, 255);
		strData = chRead;

		AfxExtractSubString(strTemp, strData, 1, ',');
		if(strTemp == "")	continue;
		strTemp.Replace(_T("L"), _T(""));
		strTemp.Replace(_T("D"), _T(""));
		strTemp.Replace(_T("W"), _T(""));
		strTemp.Replace(_T("B"), _T(""));
		nIndex = _ttoi(strTemp) - m_nAddrStartPC + 1;
		if(nIndex < 1 || nIndex > nRowCount) continue;

		m_fnUpdateGrid(&m_GridPCSideIO, nIndex, strData);
	}

	for(int i = 0; i < m_nItemCntPLC; i++)
	{
		strKey.Format(_T("%d"), i + 1);

		pFrame->m_iniConfigPLCMap.GetProfileStringW(SECTION_PLC_SIDE, strKey, _T(""), chRead, 255);
		strData = chRead;

		AfxExtractSubString(strTemp, strData, 1, ',');
		if(strTemp == "")	continue;
		strTemp.Replace(_T("L"), _T(""));
		strTemp.Replace(_T("D"), _T(""));
		strTemp.Replace(_T("W"), _T(""));
		strTemp.Replace(_T("B"), _T(""));
		nIndex = _ttoi(strTemp) - m_nAddrStartPLC + 1;
		if(nIndex < 1 || nIndex > nRowCount) continue;

		m_fnUpdateGrid(&m_GridPLCSideIO, nIndex, strData);
	}

	for(int i = 0; i < m_nItemCntCim; i++)
	{
		strKey.Format(_T("%d"), i + 1);

		pFrame->m_iniConfigPLCMap.GetProfileStringW(SECTION_CIM_SIDE, strKey, _T(""), chRead, 255);
		strData = chRead;

		AfxExtractSubString(strTemp, strData, 1, ',');
		if(strTemp == "")	continue;
		strTemp.Replace(_T("L"), _T(""));
		strTemp.Replace(_T("D"), _T(""));
		strTemp.Replace(_T("W"), _T(""));
		strTemp.Replace(_T("B"), _T(""));
		nIndex = _ttoi(strTemp) - m_nAddrStartCim + 1;
		if(nIndex < 1 || nIndex > nRowCount) continue;

		m_fnUpdateGrid(&m_GridCIMSideIO, nIndex, strData);
	}
}

void CViewIOView::m_fnUpdateMapData()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	if(pFrame->m_nViewID != eFORM_IOView)	return;

	// 작업중...
	// PC Side
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_ALIVE,				pView->m_pDlgPLC->m_stPlcMapPC[0].bAlive);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_ALARM,			pView->m_pDlgPLC->m_stPlcMapPC[0].bAlarm);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_GRAB_READY,		pView->m_pDlgPLC->m_stPlcMapPC[0].bGrabReady);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_FORWARD,		pView->m_pDlgPLC->m_stPlcMapPC[0].bForward);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_GRAB_ACK,		pView->m_pDlgPLC->m_stPlcMapPC[0].bGrabAck);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_GRAB_END,		pView->m_pDlgPLC->m_stPlcMapPC[0].bGrabEnd);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_GRAB_ERROR,		pView->m_pDlgPLC->m_stPlcMapPC[0].bGrabError);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_PPIDMISMATCH,	pView->m_pDlgPLC->m_stPlcMapPC[0].bPPIDMismatch);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S1_SEQ_ERROR,		pView->m_pDlgPLC->m_stPlcMapPC[0].bSeqError);

	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_ALIVE,			pView->m_pDlgPLC->m_stPlcMapPC[1].bAlive);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_ALARM,			pView->m_pDlgPLC->m_stPlcMapPC[1].bAlarm);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_GRAB_READY,		pView->m_pDlgPLC->m_stPlcMapPC[1].bGrabReady);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_FORWARD,		pView->m_pDlgPLC->m_stPlcMapPC[1].bForward);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_GRAB_ACK,		pView->m_pDlgPLC->m_stPlcMapPC[1].bGrabAck);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_GRAB_END,		pView->m_pDlgPLC->m_stPlcMapPC[1].bGrabEnd);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_GRAB_ERROR,		pView->m_pDlgPLC->m_stPlcMapPC[1].bGrabError);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_PPIDMISMATCH,	pView->m_pDlgPLC->m_stPlcMapPC[1].bPPIDMismatch);
	m_fnSetData(&m_GridPCSideIO, m_nAddrStartPC, ADDR_PC_S2_SEQ_ERROR,		pView->m_pDlgPLC->m_stPlcMapPC[1].bSeqError);

	// PLC Side
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_ALIVE,			pView->m_pDlgPLC->m_stPlcMapPLC[0].bAlive);

	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S1_REQ,			pView->m_pDlgPLC->m_stPlcMapPLC[0].bGrabReq);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S1_FORWARD,		pView->m_pDlgPLC->m_stPlcMapPLC[0].bScanForward);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S1_GRAB_END_OK, pView->m_pDlgPLC->m_stPlcMapPLC[0].bGrabEndOK);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S1_RESET,		pView->m_pDlgPLC->m_stPlcMapPLC[0].bReset);

	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S2_REQ,			pView->m_pDlgPLC->m_stPlcMapPLC[1].bGrabReq);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S2_FORWARD,		pView->m_pDlgPLC->m_stPlcMapPLC[1].bScanForward);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S2_GRAB_END_OK, pView->m_pDlgPLC->m_stPlcMapPLC[1].bGrabEndOK);
	m_fnSetData(&m_GridPLCSideIO, m_nAddrStartPLC, ADDR_PLC_S2_RESET,		pView->m_pDlgPLC->m_stPlcMapPLC[1].bReset);
}

void CViewIOView::m_fnSetData(CGridCtrl* pGridCtrl, int nStartAddr, CString strAddr, int nValue)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	CString strData;
	int nIndex = 0;

	// 작업중...
	strData.Format(_T("%d"), nValue);
	strAddr.Replace(_T("L"), _T(""));
	nIndex = _ttoi(strAddr) - nStartAddr + 1;

	pGridCtrl->SetItemText(nIndex, 3, strData);

	pGridCtrl->Refresh();
}

void CViewIOView::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_UPDATE_MAP_DATA)
	{

		m_fnUpdateMapData();
	}

	CFormView::OnTimer(nIDEvent);
}
