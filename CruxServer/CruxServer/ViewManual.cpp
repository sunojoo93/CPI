// ViewManual.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ViewManual.h"
#include "MainFrm.h"

///////////////// 2015.01.12 ljk //////////////////
typedef enum enumDefectFile
{
	DEFECT_FILE_COL_NUM,
	DEFECT_FILE_COL_FILE_NAME,
	MAX_DEFECT_FILE_COL
};

typedef enum enumDefectSummary
{
	DEFECT_SUMMARY_COL_NUM,			// index	
	DEFECT_SUMMARY_LAYER_NO,
	DEFECT_SUMMARY_DEFECT_NO ,
	DEFECT_SUMMARY_DEFECT_CODE,	
	DEFECT_SUMMARY_DEFECT_FLAG, 
	DEFECT_SUMMARY_DEFECT_PIXEL_X,		
	DEFECT_SUMMARY_DEFECT_PIXEL_Y, 
	DEFECT_SUMMARY_DEFECT_GATE_START_NO,	
	DEFECT_SUMMARY_DEFECT_DATA_START_NO,
	DEFECT_SUMMARY_DEFECT_GATE_END_NO, 
	DEFECT_SUMMARY_DEFECT_DATA_END_NO,	
	DEFECT_SUMMARY_COL_COORD_START_X,		// xpos
	DEFECT_SUMMARY_COL_COORD_START_Y, 		// ypos
	DEFECT_SUMMARY_DEFECT_COORD_END_X, 	
	DEFECT_SUMMARY_DEFECT_COORD_END_Y, 
	DEFECT_SUMMARY_DEFECT_DEF_SIZE, 
	DEFECT_SUMMARY_DEFECT_DEF_IMG_NO,  
	DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, 
	DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y,  
	DEFECT_SUMMARY_DEFECT_DEF_TYPE,  
	DEFECT_SUMMARY_DEFECT_CAM_NO, 
	//DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH,
	MAX_DEFECT_SUMMARY_COL
};
///////////////////////////////////////////////////

// CViewManual

IMPLEMENT_DYNCREATE(CViewManual, CFormView)

CViewManual::CViewManual()
	: CFormView(CViewManual::IDD)
{
	m_pDlgMap = NULL;
	m_pDlgVisionState = NULL;
	m_pDlgStageState = NULL;

	m_pLoadingDlg			  = NULL;	// 2012.01.12 ljk
	m_hThreadUpdateDetect	  = NULL;	// 2012.01.12 ljk
	m_ngridDetectFileRow	  = -1;		// 2012.01.12 ljk
	m_ngridDetectFileCol	  = -1;		// 2012.01.12 ljk
	m_hThreadDetectFileSelect = NULL;	// 2012.01.12 ljk
}

CViewManual::~CViewManual()
{
	if(m_pDlgMap)
	{
		delete m_pDlgMap;
		m_pDlgMap = NULL;
	}

	if(m_pDlgVisionState)
	{
		delete m_pDlgVisionState;
		m_pDlgVisionState = NULL;
	}

	if(m_pDlgStageState)
	{
		delete m_pDlgStageState;
		m_pDlgStageState = NULL;
	}

	if (m_GridProcLog)
	{
		delete m_GridProcLog;
		m_GridProcLog= NULL;
	}

	for (int i = 0 ; i < GRID_DEFECT_STATS_MAX ; i ++)
	{
		
		if (m_GridDefect_Stats[i])
		{
			delete m_GridDefect_Stats[i];
			m_GridDefect_Stats[i] = NULL;
		}
	}

	DestroyLoadingDlg();	// 2015.01.12 ljk
	ClearDetectInfo();		// 2015.01.12 ljk
}

void CViewManual::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewManual, CFormView)
	ON_BN_CLICKED(IDC_RADIO_STATS, &CViewManual::OnBnClickedRadioStats)
	ON_BN_CLICKED(IDC_RADIO_DEFECT_SUMMARY, &CViewManual::OnBnClickedRadioDefectSummary)
	ON_BN_CLICKED(IDC_RADIO_DEFECT_1, &CViewManual::OnBnClickedRadioDefect1)
	ON_BN_CLICKED(IDC_RADIO_DEFECT_2, &CViewManual::OnBnClickedRadioDefect2)
	ON_BN_CLICKED(IDC_RADIO_DEFECT_3, &CViewManual::OnBnClickedRadioDefect3)
	ON_BN_CLICKED(IDC_RADIO_DEFECT_TOTAL, &CViewManual::OnBnClickedRadioDefectTotal)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_DETECT, &CViewManual::OnBnClickedButtonUpdateDetect)
	ON_NOTIFY(NM_CLICK, IDC_GRID_DEFECT_FILE_LIST, &CViewManual::OnGridClickDetectFileList)
	ON_NOTIFY(NM_CLICK, IDC_GRID_DEFECT_LIST, &CViewManual::OnGridClickDetectList)
END_MESSAGE_MAP()


// CViewManual 진단입니다.

#ifdef _DEBUG
void CViewManual::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewManual::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewManual 메시지 처리기입니다.

void CViewManual::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_fnInitGridLog();
	m_fnInitGridDefect_Stats();

	m_fnCreateDlgMap();
//	m_fnCreateDlgVisionState();
//	m_fnCreateDlgStageState();
}

void CViewManual::m_fnInitGridLog()
{
	// Log Grid 생성
	CRect rect;
//	GetDlgItem(IDC_GRID_LOG)->GetWindowRect(rect);
	GetDlgItem(IDC_GRID_DEFECT_LIST)->GetWindowRect(rect);	// 2015.01.12 ljk
	ScreenToClient(&rect);

//	m_GridProcLog->Create(rect, this, IDC_GRID_LOG);
	m_GridProcLog = new CGridCtrl();
	m_GridProcLog->Create(rect, this, IDC_GRID_DEFECT_LIST);	// 2015.01.12 ljk
	m_GridProcLog->SetParent(this);
	m_GridProcLog->SetListMode(true);
	m_GridProcLog->SetEditable(FALSE);
	//m_GridProcLog->SetHeaderSort();
	m_GridProcLog->SetFixedBkColor(RGB(220, 240, 220));
	m_GridProcLog->SetBkColor(RGB(200, 200, 200));
	m_GridProcLog->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridProcLog->SetFixedRowCount(1);

//	CString colNameSystem[2] = { _T("Time"), _T("System Event") };
	TCHAR tszColumn[MAX_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("50,NUM"),  _T("100,LAYER_NO"),  _T("50,DEFECT_NO"),  _T("100,DEFECT_CODE"),  _T("50,FLAG"),  _T("100,PIXEL_X"), 
														   _T("100,PIXEL_Y"),  _T("100,GATE_START_NO"),  _T("100,DATA_START_NO"),  _T("100,GATE_END_NO"),  _T("100,DATA_END_NO"), 
														    _T("100,COORD_START_X"),  _T("100,COORD_START_Y"),  _T("100,COORD_END_X"),  _T("100,COORD_END_Y"),  _T("100,DEF_SIZE"), 
															 _T("100,IMG_NO"),  _T("100,IMG_SIZE_X"),  _T("100,IMG_SIZE_Y"),  _T("100,TYPE"),  _T("100,CAM_NO")  };
	

	TCHAR *ptszValue = NULL;	// 2015.01.12 ljk
	TCHAR *ptszTemp  = NULL;	// 2015.01.12 ljk

	//nColumnCount = 2;
	//m_GridProcLog->SetColumnCount(nColumnCount);
	m_GridProcLog->SetColumnCount(MAX_DEFECT_SUMMARY_COL);	// 2015.01.12 ljk

//	for(int k = 0; k < nColumnCount; k++)
	for(int k = 0; k < MAX_DEFECT_SUMMARY_COL; k++)	// 2015.01.12 ljk
	{
		//if(k == 0)
		//	m_GridProcLog->SetColumnWidth(k, 100);
		//else
		//	m_GridProcLog->SetColumnWidth(k, 470);

		//m_GridProcLog->SetItemText(0, k, colNameSystem[k]);

		////////////////////// 2015.01.12 ljk //////////////////////
		ptszValue = _tcstok_s(tszColumn[k], EXT_COLON, &ptszTemp);
		if(ptszValue)
		{
			m_GridProcLog->SetColumnWidth(k, _ttoi(ptszValue));
		}
		else
		{
			break;
		}

		ptszValue = _tcstok_s(NULL, EXT_COLON, &ptszTemp);
		if(ptszValue)
		{
			m_GridProcLog->SetItemText(0, k, ptszValue);
		}
		else
		{
			break;
		}
		//////////////////////////////////////////////////////////////
	}
}

void CViewManual::m_fnInitGridDefect_Stats()
{
	int nRowCount;
	int nColumnCount;

	TCHAR *ptszValue = NULL;	 // 2015.01.12 ljk
	TCHAR *ptszTemp	 = NULL;	 // 2015.01.12 ljk

	// Log Grid 생성
	CRect rect;
//	GetDlgItem(IDC_GRID_DEFECT_STATS)->GetWindowRect(rect);
	GetDlgItem(IDC_GRID_DEFECT_FILE_LIST)->GetWindowRect(rect);	// 2015.01.12 ljk
	ScreenToClient(&rect);

	for (int i = 0; i < GRID_DEFECT_STATS_MAX; i++)
	{
	//	m_GridDefect_Stats[i]->Create(rect, this, IDC_GRID_LOG);
		m_GridDefect_Stats[i] = new CGridCtrl();
		m_GridDefect_Stats[i]->Create(rect, this, IDC_GRID_DEFECT_FILE_LIST);	// 2015.01.12 ljk
		m_GridDefect_Stats[i]->SetParent(this);
		m_GridDefect_Stats[i]->SetListMode(true);
		m_GridDefect_Stats[i]->SetEditable(FALSE);
		//m_GridDefect_Stats[i]->SetHeaderSort();
		m_GridDefect_Stats[i]->SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
		m_GridDefect_Stats[i]->SetFixedBkColor(RGB(220, 240, 220));
		m_GridDefect_Stats[i]->SetBkColor(RGB(200, 200, 200));
		m_GridDefect_Stats[i]->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

		m_GridDefect_Stats[i]->SetFixedRowCount(1);

		//if (i != GRID_STATS)
		//{
		//	nRowCount = 1;
		//	m_GridDefect_Stats[i]->SetRowCount(nRowCount);
		//	m_GridDefect_Stats[i]->SetRowHeight(0, 14);
		//}

		switch (i)
		{
		case GRID_DEFECT_TOTAL:
		case GRID_DEFECT_1:
		case GRID_DEFECT_2:
		case GRID_DEFECT_3:
			{
				/////////////////////////////
				// init Test Log Grid
				CString colNameDefect[8] = { _T("Num"), _T("Left"), _T("Top"), _T("Width"), _T("Height"), _T("Size"), _T("Code"), _T("Cam") };
				nColumnCount = 8;
				m_GridDefect_Stats[i]->SetColumnCount(nColumnCount);

				for(int k = 0; k < nColumnCount; k++)
				{
					m_GridDefect_Stats[i]->SetColumnWidth(k, 60);

					m_GridDefect_Stats[i]->SetItemText(0, k, colNameDefect[k]);
				}
			}
			break;

	//	case GRID_DEFECT_SUMMARY:
		case GRID_DEFECT_FILE_LIST:	// 2015.01.12 ljk
			{
				/////////////////////////////
				// init Insp Tot Log Grid
				//CString colNameInspSum[14] = { _T("S/N"), _T("Mode_T("), _T("User"), _T("Insp_Time"), 
				//	_T("All_Judge"), _T("Judge_01"), _T("Judge_02"), _T("Judge_03"), _T("Grab_T_01"), _T("Class_T_01"), 
				//	_T("Grab_T_02"), _T("Class_T_02"), _T("Grab_T_03"), _T("Class_T_03") };
				TCHAR tszColumn[MAX_DEFECT_FILE_COL][MAX_PATH]  = { _T("100,Num"), _T("450,File Name") };	// 2015.01.12 ljk

				//nColumnCount = 14;
				//m_GridDefect_Stats[i]->SetColumnCount(nColumnCount);
				m_GridDefect_Stats[i]->SetColumnCount(MAX_DEFECT_FILE_COL);	// 2015.01.12 ljk

			//	for(int k = 0; k < nColumnCount; k++)
				for(int k = 0; k < MAX_DEFECT_FILE_COL; k++)	// 2015.01.12 ljk
				{
					//if(k == 0 || k == 1)
					//	m_GridDefect_Stats[i]->SetColumnWidth(k, 85);
					//else if(k == 3)
					//	m_GridDefect_Stats[i]->SetColumnWidth(k, 110);
					//else
					//	m_GridDefect_Stats[i]->SetColumnWidth(k, 70);

					//m_GridDefect_Stats[i]->SetItemText(0, k, colNameInspSum[k]);

					ptszValue = _tcstok_s(tszColumn[k], EXT_COLON,  &ptszTemp);			// 2015.01.12 ljk
					if(ptszValue)														// 2015.01.12 ljk
					{
						m_GridDefect_Stats[i]->SetColumnWidth(k, _ttoi(ptszValue));		// 2015.01.12 ljk
					}
					else
					{
						break;
					}

					ptszValue = _tcstok_s(NULL, EXT_COLON,  &ptszTemp);					// 2015.01.12 ljk
					if(ptszValue)														// 2015.01.12 ljk
					{
						m_GridDefect_Stats[i]->SetItemText(0, k, ptszValue);				// 2015.01.12 ljk
					}
					else 
					{
						break;
					}
				}
			}
			break;

		case GRID_STATS:
			{
				CString colNameInspUser[3] = { _T("Defect Name"), _T("Count"), _T("Percent") };
				nColumnCount = 3;
				m_GridDefect_Stats[i]->SetColumnCount(nColumnCount);
				CString rownameInspUser[16] = {_T("Total Count"), _T("Good Count"), _T("Defect Count"), _T("Defect Detai_T("), _T("Fail Count"), _T("Crack Count"), _T("Chipping Count"), _T("Broken Count"), _T("PolDefect Count"), _T("SiUpperDefect1 Count"), _T("SiUpperDefect2 Count"), _T("SiLowerDefect1 Count"), _T("SiLowerDefect2 Count"), _T("TabICDefect Count"), _T("PCBDefect Count")};
				nRowCount = 16;
				m_GridDefect_Stats[i]->SetRowCount(nRowCount);
				for(int k = 0; k < nColumnCount; k++)
				{
					if(k == 0)
						m_GridDefect_Stats[i]->SetColumnWidth(k, 150);
					else
						m_GridDefect_Stats[i]->SetColumnWidth(k, 150);

					m_GridDefect_Stats[i]->SetItemText(0, k, colNameInspUser[k]);
				}
				m_GridDefect_Stats[i]->SetFixedBkColor(RGB(220, 240, 220));
				m_GridDefect_Stats[i]->SetBkColor(RGB(200, 200, 200));
				m_GridDefect_Stats[i]->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);
				for (int k = 0; k < nRowCount; k++)
				{
//					m_GridDefect_Stats[i]->SetRowHeight(k, 14);
					if (k != 0)
						m_GridDefect_Stats[i]->SetItemText(k, 0, rownameInspUser[k-1]);
					if (k == 4)
					{
						for (int j=0; j<nColumnCount; j++)
						{
							m_GridDefect_Stats[i]->SetItemBkColour(k, j, RGB(220, 240, 220));
						}
					}
				}
			}
			break;
		}
	}

//	m_fnChangeGrid(GRID_DEFECT_SUMMARY);
	m_fnChangeGrid(GRID_DEFECT_FILE_LIST);	// 2015.01.12 ljk
	CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_DEFECT_SUMMARY);
	if(pBtn)	pBtn->SetCheck(1);
}

void CViewManual::m_fnChangeGrid(int nSelGrid)
{
	for (int k = 0; k < GRID_DEFECT_STATS_MAX; k++)
	{
		m_GridDefect_Stats[k]->ShowWindow(SW_HIDE);
	}

	m_GridDefect_Stats[nSelGrid]->ShowWindow(SW_SHOW);

	CString strTitle;
	if(nSelGrid == GRID_STATS)					strTitle = "Statistics";
//	else if(nSelGrid == GRID_DEFECT_SUMMARY)	strTitle = "Defect Summary";
	else if(nSelGrid == GRID_DEFECT_FILE_LIST)	strTitle = "Defect File List";	// 2015.01.12 ljk
	else if(nSelGrid == GRID_DEFECT_TOTAL)		strTitle = "Total Defect";
	else if(nSelGrid == GRID_DEFECT_1)			strTitle = "Defect Vision #1";
	else if(nSelGrid == GRID_DEFECT_2)			strTitle = "Defect Vision #2";
	else if(nSelGrid == GRID_DEFECT_3)			strTitle = "Defect Vision #3";

	GetDlgItem(IDC_STATIC_GRID)->SetWindowText(strTitle);

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strData, strItem;
	BOOL bFirst = TRUE;

	switch(nSelGrid)
	{
	case GRID_STATS:
		break;
//	case GRID_DEFECT_SUMMARY:
	case GRID_DEFECT_FILE_LIST:	// 2015.01.12 ljk
		{
		//	for(int i = 0; i < pFrame->m_DataManager.m_strArrSummary.GetCount(); i++)
			for(int i = 0; i < (int)pFrame->m_DataManager.GetSummaryCount(); i++)	// 2015.01.12 ljk
			{
			//	strData = pFrame->m_DataManager.m_strArrSummary.GetAt(i);
				strData = pFrame->m_DataManager.GetSummaryText(i);	// 2015.01.12 ljk

				m_fnUpdateGrid(nSelGrid, strData, bFirst);	// 첫번째이면 이전 Grid 데이터 삭제
				bFirst = FALSE;
			}
		}
		break;
	case GRID_DEFECT_TOTAL:
		{
		//	if(pFrame->m_DataManager.m_strArrMerge.GetCount()  <= INDEX_PANELDATA)	return;
			if(pFrame->m_DataManager.GetMergeCount()  <= INDEX_PANELDATA)	return;	// 2015.01.12 ljk

		//	for(int i = 0; i < pFrame->m_DataManager.m_strArrMerge.GetCount(); i++)
			for(int i = 0; i < (int)pFrame->m_DataManager.GetMergeCount(); i++)	// 2015.01.12 ljk
			{
			//	strData = pFrame->m_DataManager.m_strArrMerge.GetAt(i);
				strData = pFrame->m_DataManager.GetMergeText(i);	// 2015.01.12 ljk

				strItem = pFrame->m_DataManager.m_fnGetItemData(1, strData);

				if(strItem == "DEFECT")
				{
					strData.Replace(_T("DATA      ,DEFECT    ,"), _T(""));
					m_fnUpdateGrid(nSelGrid, strData, bFirst);	// 첫번째이면 이전 Grid 데이터 삭제
					bFirst = FALSE;
				}
			}
		}
		break;
	case GRID_DEFECT_1:
	case GRID_DEFECT_2:
	case GRID_DEFECT_3:
		{
		//	if(pFrame->m_DataManager.m_strArrMerge.GetCount()  <= INDEX_PANELDATA)	return;
			if(pFrame->m_DataManager.GetMergeCount()  <= INDEX_PANELDATA)	return;	// 2015.01.12 ljk

			int nGridIndex = nSelGrid - 3;

		//	for(int i = 0; i < pFrame->m_DataManager.m_strArrDefect[nGridIndex].GetCount(); i++)
			for(int i = 0; i < (int)pFrame->m_DataManager.GetGridDefectCount(nGridIndex); i++)	// 2015.01.12 ljk
			{
			//	strData = pFrame->m_DataManager.m_strArrDefect[nGridIndex].GetAt(i);
				strData = pFrame->m_DataManager.GetGridDefectText(nGridIndex, i);	// 2015.01.12 ljk

				strItem = pFrame->m_DataManager.m_fnGetItemData(1, strData);

				if(strItem == _T("DEFECT"))
				{
					strData.Replace(_T("DATA      ,DEFECT    ,"), _T(""));
					m_fnUpdateGrid(nSelGrid, strData, bFirst);	// 첫번째이면 이전 Grid 데이터 삭제
					bFirst = FALSE;
				}
			}
		}
		break;
	default:
		break;
	}
	
}

void CViewManual::m_fnAddLog(CString strLog)
{
	CTime time = CTime::GetCurrentTime();
	CString strTime;

	strTime = time.Format(_T("%Y/%m/%d %H:%M:%S"));
	
	int nRowCount = m_GridProcLog->GetRowCount();
	int nColCount = m_GridProcLog->GetColumnCount();

	m_GridProcLog->InsertRow(_T(""), nRowCount > 1 ? 1 : -1);

	m_GridProcLog->SetItemText(1, 0, strTime);
	m_GridProcLog->SetItemText(1, 1, strLog);

	if (nRowCount >= MAX_GRID_LOG + 1)
		m_GridProcLog->DeleteRow(MAX_GRID_LOG);

	m_GridProcLog->Refresh();
}

void CViewManual::m_fnClearLog()
{
	m_GridProcLog->DeleteNonFixedRows();
	m_GridProcLog->Refresh();
}

void CViewManual::m_fnUpdateGrid(int nGridIndex, CString strData, BOOL bClear)
{
	if(bClear)
		m_GridDefect_Stats[nGridIndex]->DeleteNonFixedRows();

	int nRowCount = m_GridDefect_Stats[nGridIndex]->GetRowCount();
	int nColCount = m_GridDefect_Stats[nGridIndex]->GetColumnCount();

	m_GridDefect_Stats[nGridIndex]->InsertRow(_T(""), nRowCount > 1 ? 1 : -1);

	int nCol = 0;
	int curPos= 0;
	CString resToken= strData.Tokenize(EXT_COLON,curPos);
	while (resToken != "")
	{
		m_GridDefect_Stats[nGridIndex]->SetItemText(1, nCol++, resToken);
		if (nCol+1 > nColCount)
			break;
		resToken= strData.Tokenize(EXT_COLON,curPos);
	};

	if (nRowCount > MAX_GRID_LOG)
	{
		m_GridDefect_Stats[nGridIndex]->DeleteRow(nRowCount);
	}

	m_GridDefect_Stats[nGridIndex]->Refresh();
}

void CViewManual::m_fnCreateDlgMap()
{
	if(m_pDlgMap == NULL)	// 2015.01.12 ljk
	{
		m_pDlgMap = new CDlgDefectMap();
		m_pDlgMap->Create(IDD_DLG_MAP, this);
	}    
	m_pDlgMap->ShowWindow(SW_SHOW);
	m_pDlgMap->MoveWindow(23, 30, 540, 415);

//	m_pDlgMap->m_fnSetPanelSize(585, 415);
	m_pDlgMap->m_fnSetPanelSize(1920, 1080);
}

void CViewManual::m_fnCreateDlgVisionState()
{
	if(m_pDlgVisionState == NULL)	// 2015.01.12 ljk
	{
		m_pDlgVisionState = new CDlgVisionState();
		m_pDlgVisionState->Create(IDD_DLG_VISION_STATE, this);
	}    
	m_pDlgVisionState->ShowWindow(SW_SHOW);
	m_pDlgVisionState->MoveWindow(640, 28, 585, 120);
}

void CViewManual::m_fnCreateDlgStageState()
{
	if(m_pDlgStageState == NULL)	// 2015.01.12 ljk
	{
		m_pDlgStageState = new CDlgStageState();
		m_pDlgStageState->Create(IDD_DLG_STAGE_STATE, this);
	}    
	m_pDlgStageState->ShowWindow(SW_SHOW);
	m_pDlgStageState->MoveWindow(640, 180, 585, 20);
}

void CViewManual::OnBnClickedRadioStats()
{
	m_fnChangeGrid(GRID_STATS);
}

void CViewManual::OnBnClickedRadioDefectSummary()
{
//	m_fnChangeGrid(GRID_DEFECT_SUMMARY);
	m_fnChangeGrid(GRID_DEFECT_FILE_LIST);	// 2015.01.12 ljk
}

void CViewManual::OnBnClickedRadioDefectTotal()
{
	m_fnChangeGrid(GRID_DEFECT_TOTAL);
}

void CViewManual::OnBnClickedRadioDefect1()
{
	m_fnChangeGrid(GRID_DEFECT_1);
}

void CViewManual::OnBnClickedRadioDefect2()
{
	m_fnChangeGrid(GRID_DEFECT_2);
}

void CViewManual::OnBnClickedRadioDefect3()
{
	m_fnChangeGrid(GRID_DEFECT_3);
}


void CViewManual::OnBnClickedButtonUpdateDetect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_hThreadUpdateDetect == NULL)
	{
		DestroyLoadingDlg();
		CreateLoadingDlg();

		UINT uiThreadID;
		m_hThreadUpdateDetect = (HANDLE)::_beginthreadex(NULL, 0, ThreadUpdateDetect, this, 0, &uiThreadID);
		OpenLoadingDlg(_T("Loading..."));
	}
}

void CViewManual::OnGridClickDetectFileList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = pItem->iColumn;

	m_pDlgMap->m_nDefectIndex = -1; // 결함 리스트 선택 영역 해제

	if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
	{
		return;
	}

	if(m_hThreadDetectFileSelect == NULL)
	{
		DestroyLoadingDlg();
		CreateLoadingDlg();

		UINT uiThreadID;
		m_hThreadDetectFileSelect = (HANDLE)::_beginthreadex(NULL, 0, ThreadDetectFileSelect, this, 0, &uiThreadID);
		OpenLoadingDlg(_T("Detect File 선택 중..."));
	}
}

void CViewManual::OnGridClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult)
{
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_ngridDetectFileRow = pItem->iRow;
	m_ngridDetectFileCol = pItem->iColumn;

	if(m_ngridDetectFileRow < 1 || m_ngridDetectFileCol < 0)
	{
		return;
	}

	m_pDlgMap->m_nDefectIndex = m_ngridDetectFileRow;
	m_pDlgMap->Invalidate();
}

unsigned __stdcall CViewManual::ThreadUpdateDetect(void *pArg)
{
 	CViewManual *pView = (CViewManual *)pArg;
	pView->LoadingDlgOpenDelay();	
	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	
	if(pFrame)
	{	
		/////////////////////////////// Summary 초기화 /////////////////////////////
		pFrame->m_DataManager.m_fnClearDefectList();
		pView->UpdateGridDetectSummaryRow(pFrame->m_DataManager.GetDefectCount());
		////////////////////////////////////////////////////////////////////////////

		///////////////////////////////// Map 초기화 ///////////////////////////////
		pView->m_pDlgMap->Invalidate();
		////////////////////////////////////////////////////////////////////////////

		////////////////////////////// Defect File 리스트 갱신 /////////////////////////////////
		pFrame->m_DataManager.m_fnClearSummary();	// Data Manager에서는 Summary가 파일리스트임
		pFrame->m_DataManager.m_fnLoadSummary();		// Data Manager에서는 Summary가 파일리스트임

		// 벡터의 내용을 그리드에 출력한다.
		pView->UpdateGridDetectFileRow(pFrame->m_DataManager.GetSummaryCount());
		/////////////////////////////////////////////////////////////////////////////////////////
	}
	pView->CloseLoadingDlg();

	if(pView->m_hThreadUpdateDetect)
	{
		CloseHandle(pView->m_hThreadUpdateDetect);
		pView->m_hThreadUpdateDetect = NULL;
	}

	return 0;
}

unsigned __stdcall CViewManual::ThreadDetectFileSelect(void *pArg)
{
	CViewManual *pView = (CViewManual *)pArg;
	pView->LoadingDlgOpenDelay();

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame)
	{		
		////////////////////// Summary 데이터 갱신 /////////////////////////
		pFrame->m_DataManager.m_fnClearDefectList();
		pFrame->m_DataManager.m_fnSetSummary(pView->m_ngridDetectFileRow);
		pFrame->m_DataManager.m_fnUpdateDefectList();

		pView->UpdateGridDetectSummaryRow(pFrame->m_DataManager.GetDefectCount());
		////////////////////////////////////////////////////////////////////

		///////////// 맵 갱신 //////////////
		// 9인치 기준 해상도(6760 x 5200)
		pView->m_pDlgMap->Invalidate();
		////////////////////////////////////		
	}	

	pView->CloseLoadingDlg();
	if(pView->m_hThreadDetectFileSelect)
	{
		CloseHandle(pView->m_hThreadDetectFileSelect);
		pView->m_hThreadDetectFileSelect = NULL;
	}

	return 0;
}

void CViewManual::CreateLoadingDlg()
{
	if(m_pLoadingDlg == NULL)
	{
		m_pLoadingDlg = new CLoadingDlg;
	}
}		

void CViewManual::LoadingDlgOpenDelay()
{
	while(1)
	{
		if(m_pLoadingDlg)
		{
			if(m_pLoadingDlg->GetSafeHwnd())
			{
				break;
			}
		}
		Sleep(0);
	}
}	

void CViewManual::OpenLoadingDlg(TCHAR *ptszText)
{
	if(m_pLoadingDlg)
	{
	//	m_pLoadingDlg->SetText(ptszText);
		m_pLoadingDlg->DoModal();
	}
}	

void CViewManual::CloseLoadingDlg()
{
	if(m_pLoadingDlg)
	{
		if(m_pLoadingDlg->GetSafeHwnd())
		{
			m_pLoadingDlg->CloseDialog();
		}
	}
}		

void CViewManual::DestroyLoadingDlg()
{
	if(m_pLoadingDlg)
	{
		delete m_pLoadingDlg;
		m_pLoadingDlg = NULL;
	}
}				

void CViewManual::UpdateGridDetectFileRow(int nModifyRow)
{
	if(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetSafeHwnd() == NULL)
	{
		return;
	}
	if(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() > nModifyRow + 1)
	{
		while(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() > nModifyRow + 1)
		{
			m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->DeleteRow(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() - 1);
		}
		m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->Invalidate();
	}
	else if(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() < nModifyRow + 1)
	{		
		UINT uiState = 0;
		while(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() < nModifyRow + 1)
		{
			m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->InsertRow(_T(""));
			m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetItemState(
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount()-1,
				0,
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetItemState(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() - 1, 0)|GVIS_READONLY);			
			if((m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() - 1)%2 == 1)
			{
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetItemBkColour(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() - 1, 0, RGB(220, 240, 220));
			}
			else
			{
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetItemBkColour(m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->GetRowCount() - 1, 0, RGB(255, 255, 255));
			}
		}
		m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->Invalidate();		
	}
	m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetRedraw(FALSE);
	UpdateGridDetectFileText();
	m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetRedraw(TRUE);
}

void CViewManual::UpdateGridDetectFileText()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	if(pFrame->m_DataManager.GetSummaryCount()>0)	
	{
		int nlength = 0;
		TCHAR tszIndex[MAX_PATH] = {0,};
		TCHAR *ptszNewDetectFile;
		int i = 0;
		while(i < (int)pFrame->m_DataManager.GetSummaryCount())
		{
			ptszNewDetectFile = pFrame->m_DataManager.GetSummaryText(i);
			if(ptszNewDetectFile)
			{
				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
				tszIndex[nlength] = _T('\0');
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetItemText(i+1, DEFECT_FILE_COL_NUM, tszIndex);
				m_GridDefect_Stats[GRID_DEFECT_FILE_LIST]->SetItemText(i+1, DEFECT_FILE_COL_FILE_NAME, ptszNewDetectFile);
			}
			i++;
		}
	}
}

void CViewManual::ClearDetectInfo()
{
	//STRU_DEFECT_INFO *pDelDefectInfo;
	while(m_vectorDetectFileInfo.size())
	{
		m_vectorDetectFileInfo.back();
		/*pDelDefectInfo = m_vectorDetectFileInfo[m_vectorDetectFileInfo.size() - 1];
		delete pDelDefectInfo;
		pDelDefectInfo = NULL;*/
		m_vectorDetectFileInfo.pop_back();
	}
}

void CViewManual::ReadDetectFile()
{
	TCHAR tszFindPath[MAX_PATH] = {0,};
	//int nlength = _stprintf_s(tszFindPath, MAX_PATH, _T("%s\\%s\\"), ROOT_PATH, RESULT_PATH);
	_stprintf_s(tszFindPath, MAX_PATH, _T("%s\\%s"), DATA_PATH, RESULT_PATH,//_T("D:\\이제규\\업무\\20150107\\CruxServer\\Result"),
		m_vectorDetectFileList[m_ngridDetectFileRow-1]);

	STRU_DEFECT_INFO *pNewDetectInfo;
	pNewDetectInfo = new STRU_DEFECT_INFO;
	char szLine[1024] = {0,};
	char *pszLine	  = NULL;
	char *pszToken	  = NULL;
	char *pszTemp	  = NULL;
	FILE *pFile		  = NULL;
	_tfopen_s(&pFile, tszFindPath, _T("rb"));
	if(pFile)
	{
		int nCnt = 0;
		while(1)
		{
			pszLine = fgets(szLine, 1024, pFile);
			if(pszLine == NULL)
			{
				break;
			}

			strtok_s(szLine   , ",", &pszTemp);
			if(!strncmp(pszTemp, "DEFECT", strlen("DEFECT")))
			{
				strtok_s(NULL   , ",", &pszTemp);
				/// DEFECT INFO START //////////////////////////////////			
				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.layer_No = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Code = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Flag = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Pixel_X = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Pixel_Y = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Gate_Start_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Data_Start_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Gate_End_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Data_End_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_Start_X = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_Start_Y = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_End_X = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_End_Y = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Size = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Img_No = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Img_Size_X = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Img_Size_Y = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Type = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Camera_No = atoi(pszToken);
				

				pNewDetectInfo->Dinfo.Defect_Img_Path = _T("");
				pNewDetectInfo->Dinfo.Defect_Img_Path.Empty();
				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Img_Path = (pszToken);

				//m_vectorDefectInfo.push_back(pNewDetectInfo);
				m_vectorDetectFileInfo.push_back(pNewDetectInfo);
			}
			nCnt++;
		}
		fclose(pFile);
		pFile = NULL;
	}

	delete pNewDetectInfo;
	pNewDetectInfo = NULL;
}

void CViewManual::UpdateGridDetectSummaryRow(int nModifyRow)
{
	if(m_GridProcLog->GetSafeHwnd() == NULL)
	{
		return;
	}
	if(m_GridProcLog->GetRowCount() > nModifyRow + 1)
	{
		while(m_GridProcLog->GetRowCount() > nModifyRow + 1)
		{
			m_GridProcLog->DeleteRow(m_GridProcLog->GetRowCount() - 1);
		}
		m_GridProcLog->Invalidate();
	}
	else if(m_GridProcLog->GetRowCount() < nModifyRow + 1)
	{		
		UINT uiState = 0;
		while(m_GridProcLog->GetRowCount() < nModifyRow + 1)
		{
			m_GridProcLog->InsertRow(_T(""));
			m_GridProcLog->SetItemState(
				m_GridProcLog->GetRowCount()-1,
				0,
				m_GridProcLog->GetItemState(m_GridProcLog->GetRowCount() - 1, 0)|GVIS_READONLY);			
			if((m_GridProcLog->GetRowCount() - 1)%2 == 1)
			{
				m_GridProcLog->SetItemBkColour(m_GridProcLog->GetRowCount() - 1, 0, RGB(220, 240, 220));
			}
			else
			{
				m_GridProcLog->SetItemBkColour(m_GridProcLog->GetRowCount() - 1, 0, RGB(255, 255, 255));
			}
		}
		m_GridProcLog->Invalidate();		
	}
	m_GridProcLog->SetRedraw(FALSE);
	UpdateGridDetectSummaryText();
	m_GridProcLog->SetRedraw(TRUE);
}

void CViewManual::UpdateGridDetectSummaryText()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	if(pFrame == NULL)
	{
		return;
	}

	STRU_DEFECT_INFO *pTempDefectInfo;
	if(pFrame->m_DataManager.GetDefectCount())
	{
		int nlength = 0;
		TCHAR tszIndex[MAX_PATH] = {0,};
		TCHAR tszTemp[MAX_PATH]  = {0,};
	//	STRU_DEFECT_INFO *ptszNewDetectInfo;
		int i = 0;
		while(i < (int)pFrame->m_DataManager.GetDefectCount())
		{
			pTempDefectInfo = pFrame->m_DataManager.GetDefectInfo(i);

			if(pTempDefectInfo)
			{
				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_NUM, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.layer_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_LAYER_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Defect_Code);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CODE, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Flag);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_FLAG, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_X);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_X, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_Y);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_Y, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_Start_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_START_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_Start_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_START_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_End_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_END_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_End_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_END_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_X, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_Y, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_X, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_Y, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Size);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_SIZE, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_No);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_NO, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_X);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_Y);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y, tszIndex);

				nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Type);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_TYPE, tszIndex);

				/*nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_Path);
				tszIndex[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH, tszIndex);*/
				//nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
				//tszTemp[nlength] = _T('\0');
				//m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_X, tszTemp);

				//nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
				//tszTemp[nlength] = _T('\0');
				//m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_Y, tszTemp);

				//nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), (pTempDefectInfo->Dinfo.Coord_End_X));
				//tszTemp[nlength] = _T('\0');
				//m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_X, tszTemp);

				//nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%0.3f"), (pTempDefectInfo->Dinfo.Coord_End_Y));
				//tszTemp[nlength] = _T('\0');
				//m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_Y, tszTemp);
				/*nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), (int)(pTempDefectInfo->Dinfo.Coord_End_X - pTempDefectInfo->Dinfo.Coord_Start_X));
				tszTemp[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_SIZE_DX, tszTemp);

				nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), (int)(pTempDefectInfo->Dinfo.Coord_End_Y - pTempDefectInfo->Dinfo.Coord_Start_Y));
				tszTemp[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_SIZE_DY, tszTemp);*/

				/*nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Size);
				tszTemp[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_SIZE, tszTemp);

				nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Type);
				tszTemp[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_COL_CODE, tszTemp);
*/
				nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Camera_No);
				tszTemp[nlength] = _T('\0');
				m_GridProcLog->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CAM_NO, tszTemp);
			}
			i++;
		}
	}

	//delete pTempDefectInfo;
	pTempDefectInfo = NULL;
}
