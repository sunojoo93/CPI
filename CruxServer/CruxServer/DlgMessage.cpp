#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DlgMessage.cpp : ±¸Çö ÆÄÀÏÀÔ´Ï´Ù.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgMessage.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"
#include "ViewAuto.h"
#include "DlgRecipe.h"
// CDlgMessage ´ëÈ­ »óÀÚÀÔ´Ï´Ù.

IMPLEMENT_DYNAMIC(CDlgMessage, CDialogEx)


CDlgMessage::CDlgMessage(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMessage::IDD, pParent)
{
	m_pDlgPopUpImage = NULL;
	//m_strImagPath = NULL;
	afxAmbientActCtx = FALSE;

	//m_pDlgPopUpImage = new CDlgPopUpImage();
	//m_pDlgPopUpImage->Create(IDD_DLG_POP_DEF_IMG, this);
	//m_pDlgPopUpImage->ShowWindow(SW_HIDE);
}

CDlgMessage::~CDlgMessage()
{	
	DestroyWindow();
	/*if(m_strImagPath)
	delete[] m_strImagPath;*/

	if(m_pDlgPopUpImage)
	{
		delete m_pDlgPopUpImage;
		m_pDlgPopUpImage = NULL;
	}	
}

void CDlgMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMessage, CDialogEx)
	ON_MESSAGE(WM_SOCK_CONTROL_ALARM			, &CDlgMessage::m_fnAlarmSet)
	ON_MESSAGE(WM_SOCK_CONTROL_MEASURE_ALARM	, &CDlgMessage::m_fnMeasureAlarmSet)
	ON_MESSAGE(WM_SOCK_CONTROL_CONTACT_ALARM	, &CDlgMessage::m_fnContactAlarmSet)
	ON_MESSAGE(WM_SOCK_CONTROL_LIST_ALARM		, &CDlgMessage::m_fnListlAlarm)
	ON_MESSAGE(WM_SOCK_CONTROL_DEFTYPE_ALARM	, &CDlgMessage::m_fnOverlapDefTypeAlarmSet)	
	ON_MESSAGE(WM_SOCK_CONTROL_LUMINANCE_ALARM	, &CDlgMessage::m_fnLuminanceAlarmSet)	//2016.03.02
	ON_BN_CLICKED(IDOK							, &CDlgMessage::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_OVERLAP_DEFECT, &CDlgMessage::OnGridDBClickOverlapList)
	ON_WM_COPYDATA()  
END_MESSAGE_MAP()


LRESULT CDlgMessage::m_fnAlarmSet(WPARAM wParam, LPARAM lParam)
{
	CString strMsg = _T(""), strOverlapLine, strPanelId, strTemp;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	nOverlapCnt= wParam;
	
	strTemp = (LPCTSTR)lParam;
	AfxExtractSubString(strPanelId, strTemp, 1, '#');
	AfxExtractSubString(strOverlapLine,  strTemp, 0, '#');

	m_fnInitGridOverlapDefect(nOverlapCnt);
	m_fnUpdateOverlapDefect(strOverlapLine, nOverlapCnt);
	strMsg.Format(_T("Overlap Defect [%s]"),strPanelId);
	m_fnChangeCaption(strMsg);
	ShowWindow(SW_SHOW);
	pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
	
	G_AddLog(3, 2, _T("[ALARM] OVERLAP : %s"), strMsg);
	delete [] (wchar_t*)lParam;
	return 0;
}

LRESULT CDlgMessage::m_fnMeasureAlarmSet(WPARAM wParam, LPARAM lParam)
{

	CString strMsg = _T(""), strOverlapLine, strCaption, strTemp, strPanelId;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	strTemp = (LPCTSTR)lParam;

	AfxExtractSubString(strCaption, strTemp, 1, '#');
	AfxExtractSubString(strMsg,  strTemp, 2, '#');
	AfxExtractSubString(strPanelId,  strTemp, 3, '#');
	//m_pDlgMessage = new CDlgMessage();
	//m_pDlgMessage->Create(IDD_DLG_MESSAGE);

	m_fnInitGridOverlapMeasureDefect();
	m_fnUpdateOverlapMeasureDefect(strMsg);

	strMsg.Format(_T("%s [%s]"),strCaption, strPanelId);
	m_fnChangeCaption(strMsg);
	ShowWindow(SW_SHOW);
	if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
	{
		pSetup->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
	}
	else
		pSetup->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_MEASURE_COOR_OCCUR, 1);
	
	G_AddLog(3, 2, _T("[ALARM] OVERLAP MEASURE : %s"), strMsg);

	delete [] (wchar_t*)lParam;
	return 0;
}

//2015.10.12 add by IKM - Æú ¸¶Å© ºÒ·® °ü·Ã ¾Ë¶÷ Ã³¸® ±â´É S
LRESULT CDlgMessage::m_fnListlAlarm(WPARAM wParam, LPARAM lParam) //(CString strCaption, CString strMsg)
{
	CString strMsgSet = _T(""), strMsg = _T(""), strOverlapLine, strCaption, strTemp, strPanelId;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	strTemp = (LPCTSTR)lParam;

	AfxExtractSubString(strCaption,  strTemp, 0, '#');
	AfxExtractSubString(strMsg    ,  strTemp, 1, '#');

	strMsgSet.Format(_T("%s [ %s ]"),strMsg,strCaption);

	long lRet = 0;
	CStringArray strarray;

	/*if (!::IsWindow(this->GetSafeHwnd()))
	{	*/		
		//m_pDlgErrorMsg = NULL;
		//m_pDlgErrorMsg = new CDlgMessage();		
		//Create(IDD_DLG_MESSAGE);
		//SetWindowPos(NULL, 1000, 250, 230, 440, SWP_NOZORDER);
		SetWindowPos(NULL, 1000, 250, 330, 440, SWP_NOZORDER);
		strarray.Add(_T("PANEL_ID"));
		m_fnInitGridColData(strarray);
		strarray.RemoveAll();
	//}
	m_fnChangeCaption(_T("Message !"));	
	m_fnUpdatePanelListAlarm(strMsgSet);
	ShowWindow(SW_SHOW);

	lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_POL_MARK_ILEGAL_ALARM, 1);
	G_AddLog(3,2, _T("[ALARM] OVERLAP LIST : %s"), strMsg);
	delete [] (wchar_t*)lParam;
	return 0;
}
//2015.10.12 add by IKM - Æú ¸¶Å© ºÒ·® °ü·Ã ¾Ë¶÷ Ã³¸® ±â´É E

LRESULT CDlgMessage::m_fnContactAlarmSet(WPARAM wParam, LPARAM lParam)
{

	CString strMsg = _T(""), strOverlapLine, strCaption, strTemp, strPanelId;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	strTemp = (LPCTSTR)lParam;

	AfxExtractSubString(strCaption, strTemp, 0, '#');
	AfxExtractSubString(strMsg,  strTemp, 1, '#');
		
	//Create(IDD_DLG_MESSAGE);
	m_fnInitGridOverlapContactDefect();
	m_fnUpdateOverlapContactDefect(strMsg);
	m_fnChangeCaption(strCaption);
	//m_pDlgMessage->m_fnChangeCaption(strCaption);
	ShowWindow(SW_SHOW);
	pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
	G_AddLog(3, 2, _T("[ALARM] OVERLAP CONTACT : %s"), strMsg);

	delete [] (wchar_t*)lParam;
	return 0;
}

LRESULT CDlgMessage::m_fnOverlapDefTypeAlarmSet(WPARAM wParam, LPARAM lParam)
{
	CString strMsg = _T(""), strOverlapLine, strPanelId, strTemp;
	CString strDefectType = _T(""), strType = _T(""),strDefLine;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	nOverlapCnt= wParam;
	int nCurPos = 0;
	int strStopFlag = FALSE;
	int nDefType = 0;
	CString resToken, strStop;

	strTemp = (LPCTSTR)lParam;
	AfxExtractSubString(strPanelId, strTemp, 1, '#');
	AfxExtractSubString(strOverlapLine,  strTemp, 0, '#');

	AfxExtractSubString(strDefectType,  strOverlapLine, 0, ';');
	AfxExtractSubString(strDefLine,  strDefectType, 1, ',');
	AfxExtractSubString(strDefectType,  strDefLine, 2, '_');
	if(strDefectType == ""){
		AfxExtractSubString(strDefectType,  strDefLine, 1, '_');
	}
	if(strDefectType.TrimRight() == "SCRATCH"){
		strType = _T("划伤");
	}else if(strDefectType.TrimRight() == "BROKEN"){
		strType = _T("破损");
	}else if(strDefectType.TrimRight() == "BURR"){
		strType = _T("毛刺");
	}/*else{
		strType = _T("重复缺陷，请确认");
	}*/

	m_fnInitGridOverlapDefTypeCol(nOverlapCnt);
	m_fnUpdateOverlapDefType(strOverlapLine, nOverlapCnt);
	strMsg.Format(_T("%s 重复检出，请确认"),strType);	
	//strMsg.Format(_T("Detection Overlap Defect Type"));

	m_fnChangeCaption(strMsg);
	ShowWindow(SW_SHOW);

	do 
	{
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
		strStop = pFrame->m_DataManager.m_fnGetItemData(9, resToken);
		//strTemp.TrimRight(_T(";"));
		strStopFlag = _ttoi(strStop);
		if(strStopFlag){
			AfxExtractSubString(strDefLine,  resToken, 1, ',');
			AfxExtractSubString(strDefectType,  strDefLine, 2, '_');
			if(strDefectType == ""){
				AfxExtractSubString(strDefectType,  strDefLine, 1, '_');
			}
			if(strDefectType.TrimRight() == "SCRATCH"){
				nDefType = 3;
			}else if(strDefectType.TrimRight() == "BURR"){
				nDefType = 2;
			}else if(strDefectType.TrimRight() == "BUBBLE"){
				nDefType = 1;
			}else if(strDefectType.TrimRight() == "MEASURE"){
				nDefType = 4;
			}
			break;
		}
	}	while (resToken != "");
	
	if(strStopFlag)
	{
		if(nDefType == 1){
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_SCRATCH, 1);
		}else if(nDefType == 2){
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BURR, 1);
		}else if(nDefType == 3){
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BROKEN, 1);
		}else if(nDefType == 4){
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_MEASURE_FAIL, 1);
		}else{
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
		}
		G_AddLog(2, 0, _T("HeavyAlarm nDefType : %d"),nDefType);
	}
	else
	{
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_NON_STOP, 1);
		G_AddLog(2, 0, _T("LightAlarm"));
	}


	G_AddLog(2, 0, _T("[ALARM] OVERLAP : %s"), strMsg);
	delete [] (wchar_t*)lParam;
	return 0;
}

// CDlgMessage ¸Þ½ÃÁö Ã³¸®±âÀÔ´Ï´Ù.

void CDlgMessage::OnBnClickedOk()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
	CDialogEx::OnOK();	
	
	//delete this;	
	
	//CDlgMessage::OnClose();	
}

BOOL CDlgMessage::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	COPYDATASTRUCT *p = (COPYDATASTRUCT*)pCopyDataStruct;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strMsg = _T(""), strOverlapLine, strPanelId, strTemp;
	int nOverlapCnt= p->dwData;

	strTemp.Format( _T("%s"), (CString*)p->lpData );

	AfxExtractSubString(strPanelId, strTemp, 1, '#');
	AfxExtractSubString(strOverlapLine, strTemp, 0, '#');
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	

	m_fnInitGridOverlapDefect(nOverlapCnt);
	m_fnUpdateOverlapDefect(strOverlapLine, nOverlapCnt);
	strMsg.Format(_T("Overlap Defect [%s]"),strPanelId);
	m_fnChangeCaption(strMsg);
	ShowWindow(SW_SHOW);
	pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);

	G_AddLog(3, 2, _T("OVERLAP ALARM : %s"), strMsg);
	return 0;
} 

void CDlgMessage::m_fnInitGridOverlapDefTypeCol(int nOverlapCnt)
{
	CRect rect;
	int nRowCount = m_GridOverlapDefect.GetRowCount();

	if (nRowCount != 0)
	{
		return;
	}
	//((CGxStaticEx*)GetDlgItem(IDC_GRID_OVERLAP_DEFECT))->SetWindowPos(NULL, 10, 60, 200, 300, SWP_NOZORDER);

	CString strCol;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	//m_GridOverlapDefect.SetRowCount(nOverlapCnt + 1);
	
	//m_GridOverlapDefect.SetColumnCount(strAry.GetCount());

	TCHAR colNameInspSum[MSG_DEF_TYPE_MAX_COUNT][MAX_PATH] = { _T("PANEL ID"), _T("DEFECT TYPE"),  _T("DETECTION CNT"),  _T("DETECTiON TERM"), 
										  _T("COORD_START_X"),  _T("COORD_START_Y"),_T("COORD_END_X"),  _T("COORD_END_Y"),  _T("IMAGE_PATH"), _T("STOP_FLAG")}; 

	//for (int nCol= 0 ; nCol < strAry.GetCount() ; nCol++)
	//{
	//	strCol = strAry.GetAt(nCol) + _T("\n");
	//	_tcscpy(colNameInspSum[nCol], strCol.GetBuffer(0));		
	//	if(nCol == 0) m_GridOverlapDefect.SetColumnWidth(nCol, 150);
	//	else	   m_GridOverlapDefect.SetColumnWidth(nCol, 110);
	//	//m_GridOverlapDefect.SetColumnWidth(nCol, 200);
	//	m_GridOverlapDefect.SetItemText(0, nCol, colNameInspSum[nCol]);	
	//}
	m_GridOverlapDefect.SetColumnCount(MSG_DEF_TYPE_MAX_COUNT);

	for(int k = 0; k < MSG_DEF_TYPE_MAX_COUNT; k++)
	{
		if(k == 0) m_GridOverlapDefect.SetColumnWidth(k, 150);
		else	   m_GridOverlapDefect.SetColumnWidth(k, 110);
		m_GridOverlapDefect.SetItemText(0, k, colNameInspSum[k]);
	}

	m_GridOverlapDefect.SetColumnWidth(MSG_DEFTYPE_IMG_PATH, 0);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
}

void CDlgMessage::m_fnInitGridOverlapDefect(int nOverlapCnt)
{
	CRect rect;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	m_GridOverlapDefect.SetRowCount(nOverlapCnt + 1);

	TCHAR colNameInspSum[MSG_MAX_COUNT][MAX_PATH] = { _T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"), _T("DEFECT_TYPE"), _T("IMAGE_PATH")};

	m_GridOverlapDefect.SetColumnCount(MSG_MAX_COUNT);

	for(int k = 0; k < MSG_MAX_COUNT; k++)
	{
		m_GridOverlapDefect.SetColumnWidth(k, 110);
		m_GridOverlapDefect.SetItemText(0, k, colNameInspSum[k]);
	}
		m_GridOverlapDefect.SetColumnWidth(MSG_COORD_IMG_PATH, 0);
}
void CDlgMessage::m_fnInitGridOverlapContactDefect()
{
	CRect rect;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	m_GridOverlapDefect.SetRowCount(2);

	TCHAR colNameInspSum[MSG_MAX_COUNT][MAX_PATH] = { _T("PG NO"), _T("JUDGE CODE"),  _T("PG VALUE")}; 

	m_GridOverlapDefect.SetColumnCount(MSG_MAX_COUNT - 2);

	for(int k = 0; k < MSG_MAX_COUNT - 2; k++)
	{
		m_GridOverlapDefect.SetColumnWidth(k, 110);
		m_GridOverlapDefect.SetItemText(0, k, colNameInspSum[k]);
	}

}

void CDlgMessage::m_fnInitGridOverlapMeasureDefect()
{
	CRect rect;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	m_GridOverlapDefect.SetRowCount(2);

	TCHAR colNameInspSum[MSG_MAX_COUNT][MAX_PATH] = { _T("JUDGE CODE"),  _T("MEASURE DATA")}; 

	m_GridOverlapDefect.SetColumnCount(MSG_MAX_COUNT - 3);

	//for(int k = 0; k < MSG_MAX_COUNT- 3; k++)
	//{
		m_GridOverlapDefect.SetColumnWidth(0, 110);
		m_GridOverlapDefect.SetItemText(0, 0, colNameInspSum[0]);
		m_GridOverlapDefect.SetColumnWidth(1, 800);
		m_GridOverlapDefect.SetItemText(0, 1, colNameInspSum[1]);
	//}

}

void CDlgMessage::m_fnUpdateOverlapContactDefect(CString strOverlap)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	int nlength = 0, nCurPos = 0, nPositionCnt = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	

	m_GridOverlapDefect.SetRedraw(FALSE);
	m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);
	
	CString strTemp;
	strTemp = strOverlap;
	CStringArray strArrayData;
	
	nCurPos = 0;
	CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	while (resToken != "")
	{
		strArrayData.Add(resToken);
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	}

	nCurPos = 0;
	for(int i = 0; i < 1; i++)
	{
		CString strLine = strArrayData.GetAt(i);
		CString resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 0, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 1, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 2, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		}
		nCurPos = 0;
	}

	m_GridOverlapDefect.SetRedraw(TRUE);	
	
	m_GridOverlapDefect.Refresh();

}

void CDlgMessage::m_fnUpdateOverlapMeasureDefect(CString strOverlap)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	int nlength = 0, nCurPos = 0, nPositionCnt = 0;
	TCHAR tszIndex[700] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	

	m_GridOverlapDefect.SetRedraw(FALSE);
	m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);

	CString strTemp;
	strTemp = strOverlap;
	CStringArray strArrayData;

	nCurPos = 0;
	CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	while (resToken != "")
	{
		strArrayData.Add(resToken);
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	}

	nCurPos = 0;
	for(int i = 0; i < 1; i++)
	{
		CString strLine = strArrayData.GetAt(i);
		CString resToken= strLine.Tokenize(_T("."), nCurPos); //2016.07.07 È®ÀÎ Measure ¾Ë¶÷Ã¢ Code, data °°ÀÌ ³ª¿Ã¶§ ºÁ¾ßÇÒ ºÎºÐ.
		while (resToken != "")
		{
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 0, tszIndex);
			resToken= strLine.Tokenize(_T(":"), nCurPos);

			if (resToken == _T("")) break;
			nlength = _stprintf_s(tszIndex, 700, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 1, tszIndex);
			resToken= strLine.Tokenize(_T(":"), nCurPos);

		}
		nCurPos = 0;
	}

	m_GridOverlapDefect.SetRedraw(TRUE);	

	m_GridOverlapDefect.Refresh();

}

void CDlgMessage::m_fnUpdateOverlapDefect(CString strOverlap, int nOverlapCnt)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	int nlength = 0, nCurPos = 0, nPositionCnt = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	

	m_GridOverlapDefect.SetRedraw(FALSE);
	m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);

	CString strTemp;
	strTemp = strOverlap;
	CStringArray strArrayData;

	nCurPos = 0;

	//m_strImagPath = new CString[nOverlapCnt + 1];

	CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	while (resToken != "")
	{
		strArrayData.Add(resToken);
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	}

	nCurPos = 0;
	for(int i = 0; i < nOverlapCnt; i++)
	{
		CString strLine = strArrayData.GetAt(i);
		CString resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_START_X, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_START_Y, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_END_X, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_END_Y, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_TYPE, tszIndex);
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');		
			//if (nlength == 0)
			//	//m_strImagPath[i] = _T("");
			//else
			//	//m_strImagPath[i] = tszIndex;
			m_GridOverlapDefect.SetItemText(i + 1, MSG_COORD_IMG_PATH, tszIndex);

			if (nCurPos == -1)
			{
				break;
			}
			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
		}
		nCurPos = 0;
	}

	m_GridOverlapDefect.SetRedraw(TRUE);	

	m_GridOverlapDefect.Refresh();

}

void CDlgMessage::m_fnUpdateOverlapDefType(CString strOverlap, int nOverlapCnt)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	int nRowCount = m_GridOverlapDefect.GetRowCount();
	int nlength = 0, nCurPos = 0, nCustCurPos = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	
	
	//m_GridOverlapDefect.SetRedraw(FALSE);
	m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);

	CString strTemp;
	strTemp = strOverlap;
	CStringArray strArrayData;

	nCurPos = 0;

	//m_strImagPath = new CString[nRowCount + nOverlapCnt + 1];

//	int nRowCount = m_GridOverlapDefect.GetRowCount();
	CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	while (resToken != "")
	{
		strArrayData.Add(resToken);
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	}

	nCurPos = 0;
	CString strCurDefType = _T("");
	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		nCurPos			= 0;	//2017.08.04
		nCustCurPos		= 0;	//2017.08.04

		CString strLine = strArrayData.GetAt(i);
		CString resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		//	2016.08.31 Áßº¹ ±â·Ï ¹æÁö
		nCustCurPos = nCurPos;
		CString strCompDefType = strLine.Tokenize(EXT_COLON, nCustCurPos);

		if(strCurDefType != strCompDefType)
			strCurDefType = strCompDefType;
		else
			continue;
		//

		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
		tszIndex[nlength] = _T('\0');

		while (resToken != "")
		{
		//	resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			//nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			//tszIndex[nlength] = _T('\0');

			//////////////////////////////////////////////////////////////////////////////////
			if(m_GridOverlapDefect.GetRowCount() == MAX_GRID_LOG * 10 + 1)
				m_GridOverlapDefect.DeleteRow(MAX_GRID_LOG * 10);//.DeleteString(MAX_GRID_LOG - 1);				
			int nInputRow = m_GridOverlapDefect.InsertRow(_T(""), nRowCount > 1 ? 1 : -1);//.InsertString(0, strLog);		

			GV_ITEM item;  // ±¸Á¶Ã¼ ¼±¾ð
			for (int nItemcnt =0 ; nItemcnt < MSG_DEF_TYPE_MAX_COUNT ; nItemcnt ++)
			{
				/*if (nItemcnt == MSG_DEFTYPE_IMG_PATH)
				{
				if (nlength == 0)
				m_strImagPath[i] = _T("");
				else
				m_strImagPath[i] = tszIndex;
				}	*/
				
				item.mask    = GVIF_TEXT|GVIF_FORMAT; // strText, nFormatÀ» »ç¿ëÇÏ°Ô ¼³Á¤
				item.row     = nInputRow; // row  À§Ä¡ ¼³Á¤
				item.col     =  nItemcnt;            // ³ÖÀ» columnÀÇ À§Ä¡ ¼³Á¤
				item.nFormat = DT_VCENTER|DT_SINGLELINE|DT_RIGHT; // Á¤·Ä ¿É¼Ç: ¼öÁ÷°¡¿îµ¥, ½Ì±Û¶óÀÎ, ¼öÆò¿À¸¥ÂÊ
				item.strText.Format(_T("%s"), tszIndex); // text ¼³Á¤
				m_GridOverlapDefect.SetItem(&item); // ÇØ´ç Cell À§Ä¡¿¡ »ðÀÔ
				if (nCurPos == - 1)
				{
					break;
				}
				resToken= strLine.Tokenize(EXT_COLON, nCurPos);
				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
				tszIndex[nlength] = _T('\0');		
			}
			
			//////////////////////////////////////////////////////////////////////////////////
			//resToken= strLine.Tokenize(EXT_COLON, nCurPos);

			/*nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_PANELID, tszIndex);
			*/
			/*resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_TYPE, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_COUNT, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_TERM, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_COORD_START_X, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_COORD_START_Y, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_COORD_END_X, tszIndex);

			resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, MSG_DEFTYPE_COORD_END_Y, tszIndex);
			*/
			//resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			//nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			//tszIndex[nlength] = _T('\0');		
			/*	if (nlength == 0)
			m_strImagPath[i] = _T("");
			else
			m_strImagPath[i] = tszIndex;*/
			//resToken= strLine.Tokenize(EXT_COLON, nCurPos);

		}
// 		nCurPos		 = 0;		//2017.08.04
// 		nCustCurPos = 0;		//2017.08.04
	}

	m_GridOverlapDefect.SetRedraw(TRUE);	

	m_GridOverlapDefect.Refresh();

}
void CDlgMessage::m_fnChangeCaption(CString strCaption)
{
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_DEFECT))->SetCaption(strCaption);
}

  
void CDlgMessage::OnGridDBClickOverlapList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = m_GridOverlapDefect.GetColumnCount() - 1; //pItem->iColumn;

	m_fnPopUpDefectImage();
}

void CDlgMessage::m_fnPopUpDefectImage()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strPath;
	try
	{
		if(pFrame == NULL)	
		{
			return;
		}

		if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
		{
			return;
		}
			
		if(0 < m_GridOverlapDefect.GetRowCount() - 1)
		{			
			strPath = m_GridOverlapDefect.GetItemText(m_ngridDetectFileRow, m_ngridDetectFileCol);
			//strPath = m_strImagPath[m_ngridDetectFileRow - 1];
		}
		if (strPath != _T(""))
		{
			if(m_pDlgPopUpImage)
			{
				m_pDlgPopUpImage->OnDestroy();
				delete m_pDlgPopUpImage;	
				m_pDlgPopUpImage = NULL;
			}

			m_pDlgPopUpImage = NULL;
			m_pDlgPopUpImage = new CDlgPopUpImage();
			m_pDlgPopUpImage->Create(IDD_DLG_POP_DEF_IMG, this);
			//m_pDlgPopUpImage->ShowWindow(SW_HIDE);	
			if(strPath.GetLength() > 0)
			{
				m_pDlgPopUpImage->OnDestroy();
				m_pDlgPopUpImage->SendMessage(WM_UI_CONTROL_POPUP_IMAGE, (WPARAM)0, (LPARAM)(LPCTSTR)strPath);
			}
			//// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.
			//if(m_pDlgPopUpImage)
			//{
			//	m_pDlgPopUpImage->OnDestroy();
			//	delete m_pDlgPopUpImage;	
			//	m_pDlgPopUpImage = NULL;

			//}
			//m_pDlgPopUpImage = new CDlgPopUpImage();
			//m_pDlgPopUpImage->Create(IDD_DLG_POP_DEF_IMG, this);
			//m_pDlgPopUpImage->SetWindowText(strPath.TrimRight());
			//m_pDlgPopUpImage->ShowWindow(SW_SHOW);
			//m_pDlgPopUpImage->MoveWindow(1260, 500, 648, 510);
			////strPath = _T("D:\\QMS\\i_sent\\20150515\\51400\\PNL000000000000000\\155315\\KV04-004-1.jpg");
			//if (m_pDlgPopUpImage->LoadImage(strPath.TrimRight()))
			//{
			//	m_pDlgPopUpImage->ShowImage();
			//}	

		}
	
	}
	catch(CException* e)
	{
		e->Delete();
	}
}

//2015.10.12 add by IKM - Æú ¸¶Å© ºÒ·® °ü·Ã ¾Ë¶÷ Ã³¸® ±â´É S
void CDlgMessage::m_fnInitGridColData(CStringArray &strAry)
{
	CRect rect;
	int nRowCount = m_GridOverlapDefect.GetRowCount();

	if (nRowCount != 0)
	{
		return;
	}
	//((CGxStaticEx*)GetDlgItem(IDC_GRID_OVERLAP_DEFECT))->SetWindowPos(NULL, 10, 60, 200, 300, SWP_NOZORDER);
	((CGxStaticEx*)GetDlgItem(IDC_GRID_OVERLAP_DEFECT))->SetWindowPos(NULL, 10, 60, 300, 300, SWP_NOZORDER);

	CString strCol;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);	
	
	//((CGxStaticEx*)GetDlgItem(IDC_GXLBL_DEFECT))->SetWindowPos(NULL, 10 , 10, 200, 50, SWP_NOZORDER);
	((CGxStaticEx*)GetDlgItem(IDC_GXLBL_DEFECT))->SetWindowPos(NULL, 10 , 10, 300, 50, SWP_NOZORDER);
	//((CGxStaticEx*)GetDlgItem(IDOK))->SetWindowPos(NULL, 165 , 370, 50, 30, SWP_NOZORDER);
	((CGxStaticEx*)GetDlgItem(IDOK))->SetWindowPos(NULL, 165 , 370, 100, 30, SWP_NOZORDER);
	//m_GridOverlapDefect.SetWindowPos(NULL, 10 , 60, 200, 300, SWP_NOZORDER);
		
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	m_GridOverlapDefect.SetColumnCount(strAry.GetCount());
	
	TCHAR colNameInspSum[MSG_MAX_COUNT][MAX_PATH];
	
	for (int nCol= 0 ; nCol < strAry.GetCount() ; nCol++)
	{
		strCol = strAry.GetAt(nCol) + _T("\n");		
		_tcscpy(colNameInspSum[nCol], strCol.GetBuffer(0));						
		m_GridOverlapDefect.SetColumnWidth(nCol, 290);
		m_GridOverlapDefect.SetItemText(0, nCol, colNameInspSum[nCol]);
	}		
	strCol.ReleaseBuffer();
}

void CDlgMessage::m_fnUpdatePanelListAlarm(CString strMsg)
{
	//////////////////////////////////////////////////////////////////////

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}
	int nRowCount = m_GridOverlapDefect.GetRowCount();
	//m_GridOverlapDefect.InsertRow(_T(""), nRowCount > 1 ? (nRowCount + 1) : -1);
	int nlength = 0, nCurPos = 0, nPositionCnt = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	

	try
	{
 		//m_GridOverlapDefect.SetRowCount(nRowCount + 1 );
 		//nRowCount = m_GridOverlapDefect.GetRowCount();
		//m_GridOverlapDefect.SetRedraw(FALSE);
		m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);

		CString strTemp;
		strTemp = strMsg;
		CStringArray strArrayData;

		nCurPos = 0;
		CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
		while (resToken != "")
		{
			strArrayData.Add(resToken);
			resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
		}

		nCurPos = 0;

		for(int i = 0; i < strArrayData.GetCount(); i++)
		{		
			CString strLine = strArrayData.GetAt(i);
			CString resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			while (resToken != "")
			{
				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
				tszIndex[nlength] = _T('\0');

				//////////////////////////////////////////////////////////////////////////////////
				if(m_GridOverlapDefect.GetRowCount() == MAX_GRID_LOG * 10 + 1)
					m_GridOverlapDefect.DeleteRow(MAX_GRID_LOG * 10);//.DeleteString(MAX_GRID_LOG - 1);				
				m_GridOverlapDefect.InsertRow(tszIndex, nRowCount > 1 ? 1 : -1);//.InsertString(0, strLog);				
				//////////////////////////////////////////////////////////////////////////////////
				resToken= strLine.Tokenize(EXT_COLON, nCurPos);
			}
			nCurPos = 0;
		}

		//m_GridOverlapDefect.SetRedraw(TRUE);
		//m_GridOverlapDefect.SetRedraw(TRUE, FALSE);
		m_GridOverlapDefect.ShowScrollBar(SB_HORZ,FALSE);

		m_GridOverlapDefect.Refresh();
		
	}
	catch(CException* e)
	{
		e->Delete();
	}
}

//2015.10.12 add by IKM - Æú ¸¶Å© ºÒ·® °ü·Ã ¾Ë¶÷ Ã³¸® ±â´É E


//2016.01.29 Mr.Chu       Msg Display Class
IMPLEMENT_DYNAMIC(CDlgDisplayMessge, CDialogEx)

CDlgDisplayMessge::CDlgDisplayMessge(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgDisplayMessge::IDD, pParent)
{

	//  m_strStaticText = _T("");
	m_strText = _T("");
	m_bAutoRecipeUse = FALSE;
	m_edtBgColor.CreateSolidBrush(NULL_BRUSH);
	//m_font.CreatePointFont(120,_T("±¼¸²"))
}
CDlgDisplayMessge::CDlgDisplayMessge(BOOL bDlgAutoRecipe)
{
	m_strText = _T("");
	m_bAutoRecipeUse = bDlgAutoRecipe;
	m_edtBgColor.CreateSolidBrush(RGB(200, 200, 200));
}

CDlgDisplayMessge::~CDlgDisplayMessge()
{
	//DestroyWindow();
	m_edtBgColor.DeleteObject();
}

void CDlgDisplayMessge::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);
	DDX_Control(pDX, IDC_BUTTON_RETRY, m_btnRetry);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
}

BEGIN_MESSAGE_MAP(CDlgDisplayMessge, CDialogEx)

	ON_BN_CLICKED(IDOK, &CDlgDisplayMessge::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_RETRY, &CDlgDisplayMessge::OnBnClickedButtonRetry)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CDlgDisplayMessge::OnBnClickedButtonClose)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CDlgDisplayMessge::OnBnClickedOk()
{
	// TODO: ¿©±â¿¡ ÄÁÆ®·Ñ ¾Ë¸² Ã³¸®±â ÄÚµå¸¦ Ãß°¡ÇÕ´Ï´Ù.

	DestroyWindow();
	delete this;		
	//CDialogEx::OnOK();
}

void CDlgDisplayMessge::m_fnSetText(CString strMsgA, CString strMsgB)
{
	/*static bool    bInitFont = false;
	static CFont   font ;
	LOGFONT        LogFont; 
	*/

	GetDlgItem( IDC_STATIC_TEXT)->SetFont(&m_font);
	GetDlgItem( IDC_EDIT_TEXT)->SetFont(&m_font);
	///GetDlgItem( IDOK)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_TEXT		)->SetWindowText(strMsgA);
	GetDlgItem(IDC_EDIT_TEXT		)->SetWindowText(strMsgB);

	//if(bInitFont == false)
	//{
	//	GetDlgItem(IDC_STATIC_TEXT	)->GetFont()->GetLogFont(&LogFont);
	//	LogFont.lfWeight = 200;      // ±½±â¼³Á¤( MAX : 1000 )
	//	LogFont.lfHeight = 70 ;      // Font Size ¼³Á¤

	//	font.CreateFontIndirect( &LogFont); 

	//	bInitFont = true;
	//}

	
}


BOOL CDlgDisplayMessge::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if(m_bAutoRecipeUse)
	{
		m_font.CreateFont(
			30,                     // ±ÛÀÚ³ôÀÌ
			16,                     // ±ÛÀÚ³Êºñ
			0,                      // Ãâ·Â°¢µµ
			0,                      // ±âÁØ ¼±¿¡¼­ÀÇ°¢µµ
			FW_HEAVY,              // ±ÛÀÚ±½±â
			FALSE,                  // Italic Àû¿ë¿©ºÎ
			FALSE,                  // ¹ØÁÙÀû¿ë¿©ºÎ
			FALSE,                  // Ãë¼Ò¼±Àû¿ë¿©ºÎ
			DEFAULT_CHARSET,       // ¹®ÀÚ¼ÂÁ¾·ù
			OUT_DEFAULT_PRECIS,    // Ãâ·ÂÁ¤¹Ðµµ
			CLIP_DEFAULT_PRECIS,   // Å¬¸®ÇÎÁ¤¹Ðµµ
			DEFAULT_QUALITY,       // Ãâ·Â¹®ÀÚÇ°Áú
			DEFAULT_PITCH,         // ±Û²ÃPitch
			_T("±¼¸²Ã¼")           // ±Û²Ã
			);
	}
	else
	{
		m_font.CreateFont(
			40,                     // ±ÛÀÚ³ôÀÌ
			23,                     // ±ÛÀÚ³Êºñ
			0,                      // Ãâ·Â°¢µµ
			0,                      // ±âÁØ ¼±¿¡¼­ÀÇ°¢µµ
			FW_HEAVY,              // ±ÛÀÚ±½±â
			FALSE,                  // Italic Àû¿ë¿©ºÎ
			FALSE,                  // ¹ØÁÙÀû¿ë¿©ºÎ
			FALSE,                  // Ãë¼Ò¼±Àû¿ë¿©ºÎ
			DEFAULT_CHARSET,       // ¹®ÀÚ¼ÂÁ¾·ù
			OUT_DEFAULT_PRECIS,    // Ãâ·ÂÁ¤¹Ðµµ
			CLIP_DEFAULT_PRECIS,   // Å¬¸®ÇÎÁ¤¹Ðµµ
			DEFAULT_QUALITY,       // Ãâ·Â¹®ÀÚÇ°Áú
			DEFAULT_PITCH,         // ±Û²ÃPitch
			_T("±¼¸²Ã¼")           // ±Û²Ã
			);
	}
	SetWindowPos(NULL, 0 , 300, 1280, 300, SWP_NOZORDER);

	if(m_bAutoRecipeUse)
	{
		ModifyStyle(WS_CAPTION, NULL);
		SetMenu(NULL);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ¿¹¿Ü: OCX ¼Ó¼º ÆäÀÌÁö´Â FALSE¸¦ ¹ÝÈ¯ÇØ¾ß ÇÕ´Ï´Ù.
}


//2016.03.02

void CDlgMessage::m_fnInitGridOverlapLuminanceDefect()
{
	CRect rect;
	GetDlgItem(IDC_GRID_OVERLAP_DEFECT)->GetWindowRect(rect);
	ScreenToClient(&rect);

	m_GridOverlapDefect.Create(rect, this, IDC_GRID_OVERLAP_DEFECT);
	m_GridOverlapDefect.SetParent(this);
	m_GridOverlapDefect.SetListMode(true);
	m_GridOverlapDefect.SetEditable(FALSE);
	m_GridOverlapDefect.SetFixedBkColor(RGB(220, 240, 220));
	m_GridOverlapDefect.SetBkColor(RGB(200, 200, 200));
	m_GridOverlapDefect.GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
	m_GridOverlapDefect.SetFixedRowCount(1);
	m_GridOverlapDefect.SetRowCount(2);

	TCHAR colNameInspSum[MSG_MAX_COUNT][MAX_PATH] = {_T("LUMINANCE DATA")}; 

	m_GridOverlapDefect.SetColumnCount(MSG_MAX_COUNT - 4);

	m_GridOverlapDefect.SetColumnWidth(0, 800);
	m_GridOverlapDefect.SetItemText(0, 0, colNameInspSum[0]);

}

void CDlgMessage::m_fnUpdateOverlapLuminanceDefect(CString strOverlap)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	int nlength = 0, nCurPos = 0, nPositionCnt = 0;
	TCHAR tszIndex[MAX_PATH] = {0,};
	TCHAR tszTemp[MAX_PATH]  = {0,};	

	m_GridOverlapDefect.SetRedraw(FALSE);
	m_GridOverlapDefect.SetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0,m_GridOverlapDefect.GetItemState(m_GridOverlapDefect.GetRowCount() - 1, 0)|GVIS_READONLY);

	CString strTemp;
	strTemp = strOverlap;
	CStringArray strArrayData;

	nCurPos = 0;
	CString resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	while (resToken != "")
	{
		strArrayData.Add(resToken);
		resToken= strTemp.Tokenize(EXT_SEMICOL, nCurPos);
	}

	nCurPos = 0;
	for(int i = 0; i < 1; i++)
	{
		CString strLine = strArrayData.GetAt(i);
		CString resToken= strLine.Tokenize(_T(":"), nCurPos);
		while (resToken != "")
		{
			nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			tszIndex[nlength] = _T('\0');
			m_GridOverlapDefect.SetItemText(i + 1, 0, tszIndex);
			resToken= strLine.Tokenize(_T(":"), nCurPos);

			//if (resToken == _T("")) break;
			//nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), resToken);
			//tszIndex[nlength] = _T('\0');
			//m_GridOverlapDefect.SetItemText(i + 1, 1, tszIndex);
			//resToken= strLine.Tokenize(_T(":"), nCurPos);

		}
		nCurPos = 0;
	}

	m_GridOverlapDefect.SetRedraw(TRUE);	

	m_GridOverlapDefect.Refresh();

}

LRESULT CDlgMessage::m_fnLuminanceAlarmSet(WPARAM wParam, LPARAM lParam)
{

	CString strMsg = _T(""), strOverlapLine, strCaption, strTemp, strPanelId;
	int nOverlapCnt= 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	strTemp = (LPCTSTR)lParam;

	AfxExtractSubString(strCaption, strTemp, 1, '#');
	AfxExtractSubString(strMsg,  strTemp, 2, '#');
	AfxExtractSubString(strPanelId,  strTemp, 3, '#');

	m_fnInitGridOverlapLuminanceDefect();
	m_fnUpdateOverlapLuminanceDefect(strMsg);

	strMsg.Format(_T("%s [%s]"),strCaption, strPanelId);
	m_fnChangeCaption(strMsg);
	ShowWindow(SW_SHOW);

	G_AddLog(3, 2, _T("[ALARM] OVERLAP LUMINANCE : %s"), strMsg);

	delete [] (wchar_t*)lParam;
	return 0;
}

void CDlgDisplayMessge::OnBnClickedButtonRetry()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pViewSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	pViewSetup->m_pDlgRecipe->SendMessage(WM_AUTO_RECIPE);
}

void CDlgDisplayMessge::OnBnClickedButtonClose()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pViewSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	pViewSetup->m_pDlgRecipe->EnableWindow(TRUE);
	pViewSetup->EnableWindow(TRUE);
	SendMessage(WM_CLOSE);
}

void CDlgDisplayMessge::m_fnAutoRecipeDesign()
{
	int centerX = GetSystemMetrics(SM_CXSCREEN)/2;
	int centerY = GetSystemMetrics(SM_CYSCREEN)/2;

	MoveWindow(centerX-300, centerY-300, 600, 600);

	CRect rect;
	GetClientRect(&rect);

	GetDlgItem(IDC_STATIC_TEXT)->MoveWindow(1, 1, rect.right-2, rect.bottom/12);
	GetDlgItem(IDC_EDIT_TEXT)->MoveWindow(1, rect.bottom/12, rect.right-2, rect.bottom/12*8);

	m_btnRetry.MoveWindow(rect.right/5, (rect.bottom/5)*4, rect.right/6, rect.right/6);
	m_btnClose.MoveWindow(rect.right/5*3, (rect.bottom/5)*4, rect.right/6, rect.right/6);
}


BOOL CDlgDisplayMessge::OnEraseBkgnd(CDC* pDC)
{
	if(m_bAutoRecipeUse)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillSolidRect(rect, RGB(200,200,200));

		return TRUE;
	}
	return CDialogEx::OnEraseBkgnd(pDC);
}

HBRUSH CDlgDisplayMessge::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if(m_bAutoRecipeUse)
	{
		switch(nCtlColor)
		{
		case CTLCOLOR_STATIC:
			if(pWnd->GetDlgCtrlID() == IDC_STATIC_TEXT)
			{
				pDC->SetTextColor(RGB(255,0,0));
				pDC->SetBkMode(TRANSPARENT);
				return (HBRUSH)GetStockObject(NULL_BRUSH);
			}	

		case CTLCOLOR_EDIT:
			if(pWnd->GetDlgCtrlID() == IDC_EDIT_TEXT)
			{
				pDC->SetBkMode(TRANSPARENT);
				return (HBRUSH)m_edtBgColor;
			}	
		}
	}
	
	return hbr;
}
