// DlgManualJudge.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgManualJudge.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"


// CDlgManualJudge 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgManualJudge, CDialogEx)

typedef enum enumDefectSummary
{
	DEFECT_SUMMARY_COL_NUM,	
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
	DEFECT_SUMMARY_COL_COORD_START_X,
	DEFECT_SUMMARY_COL_COORD_START_Y,
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

CDlgManualJudge::CDlgManualJudge(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgManualJudge::IDD, pParent)
{
	mp_image_object = NULL;
	m_width_size = m_height_size = 0;
	m_strPanelID = _T("");
	m_strManualJudgeDefectFile = _T("");
	m_strQmsDefectFile = _T("");
}

CDlgManualJudge::~CDlgManualJudge()
{

	OnDestroy();

	if (m_GridManualJudgeDefectSummary)
	{
		delete m_GridManualJudgeDefectSummary;
		m_GridManualJudgeDefectSummary = NULL;
	}

}

void CDlgManualJudge::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgManualJudge, CDialogEx)
	ON_MESSAGE(WM_SOCK_CONTROL_MANUAL_JUDGE	, &CDlgManualJudge::m_fnManualJudgeSet)	
	ON_NOTIFY(NM_DBLCLK, IDC_GRID_MANUAL_JUDGE_DEFECT_SUMMARY, &CDlgManualJudge::OnGridDBClickManualJudgeDetectList)
	ON_BN_CLICKED(IDC_BTN_OK, &CDlgManualJudge::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_NG, &CDlgManualJudge::OnBnClickedBtnNg)
	ON_BN_CLICKED(IDC_IMAGE_PATH, &CDlgManualJudge::OnBnClickedImagePath)
	ON_BN_CLICKED(IDC_BTN_NGB, &CDlgManualJudge::OnBnClickedBtnNgb)
END_MESSAGE_MAP()

// CDlgManualJudge 메시지 처리기입니다.

LRESULT CDlgManualJudge::m_fnManualJudgeSet(WPARAM wParam, LPARAM lParam)
{
	CString strMsg;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	strMsg = (LPCTSTR)lParam;

	AfxExtractSubString(m_strPanelID, strMsg, 1, '#');
	AfxExtractSubString(m_strManualJudgeDefectFile,  strMsg, 2, '#');
	AfxExtractSubString(m_strQmsDefectFile,  strMsg, 3, '#');


	m_fnInitGridManualJudgeDefectSummary();
	if (pFrame->m_DataManager.FileExist(m_strManualJudgeDefectFile))
		UpdateGridManualJudgeDefectSummaryRow(m_strManualJudgeDefectFile);
	ShowWindow(SW_SHOW);
	MoveWindow(1270, 0, 648, 850);

	pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MANUAL_JUDGE_CHECK, 1);	//2018.06.01

	delete [] (wchar_t*)lParam;
	return 0;
}

void CDlgManualJudge::m_fnInitGridManualJudgeDefectSummary()
{
	CRect rect;
	GetDlgItem(IDC_GRID_MANUAL_JUDGE_DEFECT_SUMMARY)->GetWindowRect(rect);
	ScreenToClient(&rect);
	m_GridManualJudgeDefectSummary = new CGridCtrl();
	m_GridManualJudgeDefectSummary->Create(rect, this, IDC_GRID_MANUAL_JUDGE_DEFECT_SUMMARY);
	m_GridManualJudgeDefectSummary->SetParent(this);
	m_GridManualJudgeDefectSummary->SetListMode(true);
	m_GridManualJudgeDefectSummary->SetEditable(FALSE);
	//m_GridManualJudgeDefectSummary->SetHeaderSort();
	m_GridManualJudgeDefectSummary->SetFixedBkColor(RGB(220, 240, 220));
	m_GridManualJudgeDefectSummary->SetBkColor(RGB(200, 200, 200));
	m_GridManualJudgeDefectSummary->GetDefaultCell(FALSE, FALSE)->SetBackClr(COLOR_ENABLE);

	m_GridManualJudgeDefectSummary->SetFixedRowCount(1);


	TCHAR colNameInspSum[MAX_DEFECT_SUMMARY_COL][MAX_PATH] = { _T("NUM"),  _T("LAYER_NO"),  _T("DEFECT_NO"),  _T("DEFECT_CODE"),  _T("FLAG"),  _T("PIXEL_X"), 
		_T("PIXEL_Y"),  _T("GATE_START_NO"),  _T("DATA_START_NO"),  _T("GATE_END_NO"),  _T("DATA_END_NO"), 
		_T("COORD_START_X"),  _T("COORD_START_Y"),  _T("COORD_END_X"),  _T("COORD_END_Y"),  _T("DEF_SIZE"), 
		_T("IMG_NO"),  _T("IMG_SIZE_X"),  _T("IMG_SIZE_Y"),  _T("TYPE"),  _T("CAM_NO")  };

	m_GridManualJudgeDefectSummary->SetColumnCount(MAX_DEFECT_SUMMARY_COL);

	for(int k = 0; k < MAX_DEFECT_SUMMARY_COL; k++)
	{
		if(k == 0 || k == 1)
			m_GridManualJudgeDefectSummary->SetColumnWidth(k, 85);
		else if(k == 3)
			m_GridManualJudgeDefectSummary->SetColumnWidth(k, 110);
		else
			m_GridManualJudgeDefectSummary->SetColumnWidth(k, 70);

		m_GridManualJudgeDefectSummary->SetItemText(0, k, colNameInspSum[k]);
	}
}

void CDlgManualJudge::m_fnClearManualJudgeDefectList()
{
	G_AddLog(3, 3, _T("m_fnClearManualJudgeDefectList() - Start"));
	STRU_DEFECT_INFO *pDelDefectInfo;
	while(m_vectorManualJudgeDefectInfo.size())
	{
		m_vectorManualJudgeDefectInfo.back();
		pDelDefectInfo = m_vectorManualJudgeDefectInfo[m_vectorManualJudgeDefectInfo.size()-1];
		if(pDelDefectInfo)
		{
			delete pDelDefectInfo;
			pDelDefectInfo = NULL;
		}
		m_vectorManualJudgeDefectInfo.pop_back(); 
	}
	G_AddLog(3, 3, _T("m_fnClearManualJudgeDefectList() - End"));
}

void CDlgManualJudge::m_fnUpdateSelManualJudgeDefectList(CString strPath)
{
	G_AddLog(3, 3, _T("m_fnUpdateSelManualJudgeDefectList() - Start"));
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	//if(m_nSelectSummary > (int)m_vectorDefectFile.size())
	//{
	//	return;
	//}

	//if(m_nSelectSummary < 0)
	//{
	//	return;
	//}

	TCHAR tszFindPath[MAX_PATH] = {0,};
	_stprintf_s(tszFindPath, MAX_PATH, _T("%s"), strPath);

	char szLine[1024] = {0,};
	char *pszLine	  = NULL;
	char *pszToken	  = NULL;
	char *pszTemp	  = NULL;
	FILE *pFile		  = NULL;

	int curPos = 0;
	CString strVal = _T("");

	try
	{
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

				if(m_vectorManualJudgeDefectInfo.size())
				{
					m_vectorManualJudgeDefectInfo.back();
				}

				strtok_s(szLine   , ",", &pszTemp);

				if(!strncmp(pszTemp, "DEFECT", strlen("DEFECT")))
				{

					pManualJudgeDetectInfo = NULL;
					pManualJudgeDetectInfo = new STRU_DEFECT_INFO;

					//strtok_s(szLine   , ",", &pszTemp);
					strtok_s(NULL   , ",", &pszTemp);
					/// DEFECT INFO START //////////////////////////////////	

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.layer_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Defect_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Defect_Code = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Flag = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Pixel_X = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Pixel_Y = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Gate_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Data_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Gate_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Data_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Coord_Start_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Coord_Start_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Coord_End_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Coord_End_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Defect_Size = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Defect_Img_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Img_Size_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Img_Size_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Defect_Type = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pManualJudgeDetectInfo->Dinfo.Camera_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);	//Light
					if(pszToken == NULL)break;

					pszToken = strtok_s(NULL   , ",", &pszTemp);	//Defect Image
					pManualJudgeDetectInfo->Dinfo.Defect_Img_Path = _T("");
					pManualJudgeDetectInfo->Dinfo.Defect_Img_Path.Empty();

					if(pszToken == NULL)

						pManualJudgeDetectInfo->Dinfo.Defect_Img_Path = _T("");
					else
						pManualJudgeDetectInfo->Dinfo.Defect_Img_Path = (pszToken);

					m_vectorManualJudgeDefectInfo.push_back(pManualJudgeDetectInfo);

				}
				nCnt++;
			}
			fclose(pFile);
			pFile = NULL;
		}
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelManualJudgeDefectList Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelManualJudgeDefectList Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelManualJudgeDefectList Failed !! - Exception "));
		e->Delete();
	}
	G_AddLog(3, 3, _T("m_fnUpdateSelManualJudgeDefectList() - End"));
}

UINT CDlgManualJudge::GetManualJudgeDefectCount()
{
	return m_vectorManualJudgeDefectInfo.size();
}

STRU_DEFECT_INFO *CDlgManualJudge::GetManualJudgeDefectInfo(int nIndex)
{
	return m_vectorManualJudgeDefectInfo[nIndex];
}

void CDlgManualJudge::UpdateGridManualJudgeDefectSummaryRow(CString strPath)
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{	
		m_fnClearManualJudgeDefectList();
		m_fnUpdateSelManualJudgeDefectList(strPath);

		int nModifyRow = GetManualJudgeDefectCount();
		if(m_GridManualJudgeDefectSummary->GetSafeHwnd() == NULL)
		{
			return;
		}
		if(m_GridManualJudgeDefectSummary->GetRowCount() > nModifyRow + 1)
		{
			while(m_GridManualJudgeDefectSummary->GetRowCount() > nModifyRow + 1)
			{
				m_GridManualJudgeDefectSummary->DeleteRow(m_GridManualJudgeDefectSummary->GetRowCount() - 1);
			}
			m_GridManualJudgeDefectSummary->Invalidate();
		}
		else if(m_GridManualJudgeDefectSummary->GetRowCount() < nModifyRow + 1)
		{		
			UINT uiState = 0;
			while(m_GridManualJudgeDefectSummary->GetRowCount() < nModifyRow + 1)
			{
				m_GridManualJudgeDefectSummary->InsertRow(_T(""));
				m_GridManualJudgeDefectSummary->SetItemState(
					m_GridManualJudgeDefectSummary->GetRowCount()-1, 0,
					m_GridManualJudgeDefectSummary->GetItemState(m_GridManualJudgeDefectSummary->GetRowCount() - 1, 0)|GVIS_READONLY);			
				if((m_GridManualJudgeDefectSummary->GetRowCount() - 1)%2 == 1)
				{
					m_GridManualJudgeDefectSummary->SetItemBkColour(m_GridManualJudgeDefectSummary->GetRowCount() - 1, 0, RGB(220, 240, 220));
				}
				else
				{
					m_GridManualJudgeDefectSummary->SetItemBkColour(m_GridManualJudgeDefectSummary->GetRowCount() - 1, 0, RGB(255, 255, 255));
				}
			}
			m_GridManualJudgeDefectSummary->Invalidate();		
		}


		m_GridManualJudgeDefectSummary->SetRedraw(FALSE);

		UpdateGridManualJudgeDefectSummaryText();
		m_GridManualJudgeDefectSummary->SetRedraw(TRUE);
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryRow Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryRow Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryRow Failed !! - Exception "));
		e->Delete();
	}
}

void CDlgManualJudge::UpdateGridManualJudgeDefectSummaryText()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{
		if(pFrame == NULL)
		{
			return;
		}

		STRU_DEFECT_INFO *pTempDefectInfo;
		if(GetManualJudgeDefectCount())
		{
			int nlength = 0;
			TCHAR tszIndex[MAX_PATH] = {0,};
			TCHAR tszTemp[MAX_PATH]  = {0,};
			//	STRU_DEFECT_INFO *ptszNewDetectInfo;
			int i = 0;
			while(i < (int)GetManualJudgeDefectCount())
			{
				pTempDefectInfo = GetManualJudgeDefectInfo(i);

				if(pTempDefectInfo)
				{			
					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), i+1);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_NUM, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.layer_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_LAYER_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"),pTempDefectInfo->Dinfo.Defect_Code);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CODE, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Flag);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_FLAG, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_X);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_X, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Pixel_Y);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_PIXEL_Y, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_Start_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_START_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_Start_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_START_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Gate_End_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_GATE_END_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Data_End_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DATA_END_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_X);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_X, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_Start_Y);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_COL_COORD_START_Y, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_X);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_X, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Coord_End_Y);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_COORD_END_Y, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Size);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_SIZE, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_No);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_NO, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_X);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_X, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%0.3f"), pTempDefectInfo->Dinfo.Img_Size_Y);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_IMG_SIZE_Y, tszIndex);

					nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Defect_Type);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_TYPE, tszIndex);

					nlength = _stprintf_s(tszTemp, MAX_PATH, _T("%d"), pTempDefectInfo->Dinfo.Camera_No);
					tszTemp[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_CAM_NO, tszTemp);

					/*		nlength = _stprintf_s(tszIndex, MAX_PATH, _T("%s"), pTempDefectInfo->Dinfo.Defect_Img_Path);
					tszIndex[nlength] = _T('\0');
					m_GridManualJudgeDefectSummary->SetItemText(i+1, DEFECT_SUMMARY_DEFECT_DEF_IMG_PATH, tszIndex);*/
				}
				i++;
			}
		}

		pTempDefectInfo = NULL;
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryText Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryText Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("UpdateGridManualJudgeDefectSummaryText Failed !! - Exception "));
		e->Delete();
	}

}

void CDlgManualJudge::OnGridDBClickManualJudgeDetectList(NMHDR *pNotifyStruct, LRESULT *pResult)
{	
	NM_GRIDVIEW *pItem = (NM_GRIDVIEW *)pNotifyStruct;
	m_nGridManualJudgeDetectFileRow = pItem->iRow;
	m_nGridManualJudgeDetectFileCol = pItem->iColumn;
	CString strPath;

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	try
	{
		if((pFrame == NULL) || (m_nGridManualJudgeDetectFileRow < 1 || m_nGridManualJudgeDetectFileCol < 0))
		{
			return;
		}

		STRU_DEFECT_INFO *pTempDefectInfo;

		if(GetManualJudgeDefectCount())
		{
			pTempDefectInfo = GetManualJudgeDefectInfo(m_nGridManualJudgeDetectFileRow - 1);
			strPath = pTempDefectInfo->Dinfo.Defect_Img_Path;
			strPath =pFrame->m_DataManager.m_fnSetItemMutiData(3 , strPath, _T("i_temp") , 6, 8 - 1, _T("\\"));
		}
				
		if(strPath.GetLength() > 0)
		{
			if (LoadImage(strPath.TrimRight()))
			{
				ShowImage();
			}
			else
				G_AddLog(2, 0, _T("OnGridDBClickManualJudgeDetectList() Loading Fail : %s"), strPath);
		}

		pTempDefectInfo = NULL;
	}
	catch(CException* e)
	{
		e->Delete();
	}
}

BOOL CDlgManualJudge::LoadImage(CString strImgPath)
{
	try
	{
		if (mp_image_object != NULL) mp_image_object->Release();
		BOOL ok_flag = 0;

		HANDLE h_file = CreateFileW(strImgPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (INVALID_HANDLE_VALUE != h_file)
		{
			int file_size = GetFileSize(h_file, NULL);
			if (-1 != file_size)
			{
				HGLOBAL h_global = GlobalAlloc(GMEM_MOVEABLE, file_size);
				if (NULL != h_global)
				{
					void *p_data = GlobalLock(h_global);

					if (NULL != p_data)
					{
						unsigned long read_size = 0 ;

						ReadFile(h_file, p_data, file_size, &read_size, NULL);
						GlobalUnlock(h_global);
						LPSTREAM p_stream = NULL;
						HRESULT h_result = CreateStreamOnHGlobal(h_global, TRUE, &p_stream);
						if (SUCCEEDED(h_result) && p_stream)
						{
							::OleLoadPicture(p_stream, file_size, FALSE, IID_IPicture, (LPVOID*)&mp_image_object);
							if (SUCCEEDED(h_result) && mp_image_object)
							{
								mp_image_object->get_Width(&m_width_size);
								mp_image_object->get_Height(&m_height_size);
							}
							ok_flag = 1;
						}					
					}

				}
				GlobalFree(h_global);
			}
			CloseHandle(h_file);
		}
		return ok_flag;
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

	return TRUE;

}

void CDlgManualJudge::ShowImage()
{
	CClientDC dc(this);

	if (m_width_size && m_height_size )
	{
		int x_size = MulDiv(m_width_size, dc.GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int y_size = MulDiv(m_height_size, dc.GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);
		mp_image_object->Render(dc.m_hDC, 1, 1, x_size, y_size, 0, 
			m_height_size, m_width_size, -m_height_size, NULL);
	}
}

void CDlgManualJudge::OnDestroy()
{
	if (mp_image_object != NULL)
	{
		mp_image_object->Release();
		mp_image_object = NULL;
	}

}

void CDlgManualJudge::OnBnClickedBtnOk()
{
	CString strTemp =_T("");
	int nRet = AfxMessageBox(_T("Are you sure Grade A?"), MB_OKCANCEL);
	if ( nRet != IDOK ) return;	
	GetDlgItem(IDC_EDIT_OPID)->GetWindowText(strTemp);
	int nValue = strTemp.GetLength();	
	
	if(nValue > 5)
	{
		AfxMessageBox(_T("Please Input the Operator ID under 6 Number"), MB_ICONINFORMATION);
	}
	else if(strTemp == _T(""))
	{
		AfxMessageBox(_T("Please Input the Operator ID"), MB_ICONINFORMATION);
	}
	else
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

		pFrame->m_DataManager.m_fnSetManualJudge(1, m_strPanelID, m_strManualJudgeDefectFile, m_strQmsDefectFile, strTemp);
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MANUAL_JUDGE_CHECK, 0);	//2018.06.01

		SendMessage(WM_CLOSE);
	}
	
}


void CDlgManualJudge::OnBnClickedBtnNg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp =_T("");
	int nRet = AfxMessageBox(_T("Are you sure Grade NG?"), MB_OKCANCEL);
	if ( nRet != IDOK ) return;
	GetDlgItem(IDC_EDIT_OPID)->GetWindowText(strTemp);
	int nValue = strTemp.GetLength();
	if(nValue > 5){
		AfxMessageBox(_T("Please Input the Operator ID under 6 Number"), MB_ICONINFORMATION);
	}else if(strTemp == _T(""))
	{
		AfxMessageBox(_T("Please Input the Operator ID"), MB_ICONINFORMATION);
	}else{
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

		pFrame->m_DataManager.m_fnSetManualJudge(0, m_strPanelID, m_strManualJudgeDefectFile, m_strQmsDefectFile, strTemp);
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MANUAL_JUDGE_CHECK, 0);	//2018.06.01

		SendMessage(WM_CLOSE);
	}	
}


void CDlgManualJudge::OnBnClickedImagePath()
{
	// image PATH
	//CString strPath;
	//STRU_DEFECT_INFO *pTempDefectInfo;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	
	if(pFrame->m_DataManager.m_strReJudgeImgPath ==_T(""))
	{
		AfxMessageBox(_T("IMAGE PATH NULL"));
	}
	else
	{		
		ShellExecute(NULL, _T("open"), pFrame->m_DataManager.m_strReJudgeImgPath , NULL, NULL, SW_SHOW);		 
	}
}

void CDlgManualJudge::OnBnClickedBtnNgb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTemp =_T("");
	int nRet = AfxMessageBox(_T("Are you sure Grade B(FB3)?"), MB_OKCANCEL);
	if ( nRet != IDOK ) return;
	GetDlgItem(IDC_EDIT_OPID)->GetWindowText(strTemp);
	int nValue = strTemp.GetLength();
	if(nValue > 5){
		AfxMessageBox(_T("Please Input the Operator ID under 6 Number"), MB_ICONINFORMATION);
	}else if(strTemp == _T(""))
	{
		AfxMessageBox(_T("Please Input the Operator ID"), MB_ICONINFORMATION);
	}else{
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

		pFrame->m_DataManager.m_fnSetManualJudge(2, m_strPanelID, m_strManualJudgeDefectFile, m_strQmsDefectFile, strTemp);
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_MANUAL_JUDGE_CHECK, 0);	//2018.06.01

		SendMessage(WM_CLOSE);
	}	
}
