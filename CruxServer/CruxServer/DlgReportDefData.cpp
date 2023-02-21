
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// DlgDefectMap.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgReportDefData.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"
#include "ViewAuto.h"
#include "ViewReport.h"

// CDlgDefectInfo 대화 상자입니다.


IMPLEMENT_DYNAMIC(CDlgReportDefData, CDialogEx)

	CDlgReportDefData::CDlgReportDefData(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgReportDefData::IDD, pParent)
{
	m_cPanelID		= _T("-");	
	m_cGrade		= _T("-");	
	m_cCFResult		= _T("-");	
	m_cTftResult	= _T("-");
	m_cPenetResult	= _T("-");
	mp_edit			= NULL;
	mp_DefLbl		= NULL;
	mp_DefPer		= NULL;
	m_H_DefData		= NULL;
}

CDlgReportDefData::~CDlgReportDefData()
{
	//m_font.DeleteObject(); 
}

void CDlgReportDefData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	OnInitialUpdate();
}


BEGIN_MESSAGE_MAP(CDlgReportDefData, CDialogEx)	
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_UI_CONTROL_DIS_STATS_LIST, &CDlgReportDefData::m_fnUpdateStatsData)
	ON_MESSAGE(WM_UI_CONTROL_JUDGE_DIS, &CDlgReportDefData::m_fnInitState)
	ON_MESSAGE(WM_UI_CONTROL_DIS_STATS_NAME, &CDlgReportDefData::m_fnInitState)
//	ON_MESSAGE(WM_UI_CONTROL_JUDGE_INFO_DIS, &CDlgReportDefData::m_fnUpdateJudgeInfo)	 
END_MESSAGE_MAP()


// CDlgDefectMap 메시지 처리기입니다.


void CDlgReportDefData::OnPaint()
{
	CPaintDC dc(this); // device context for painting

}

BEGIN_EVENTSINK_MAP(CDlgReportDefData, CDialogEx)
//	ON_EVENT(CDlgReportDefData, IDC_GXBTN_RESET_ALARM2, DISPID_CLICK, CDlgReportDefData::ClickGxbtnResetAlarm, VTS_NONE)
	
END_EVENTSINK_MAP()

void CDlgReportDefData::OnInitialUpdate()
{
	m_H_DefData = CreateEvent( NULL, TRUE,  FALSE,  NULL ); 
	m_fnCreateFont();
	GetDlgItem(IDC_LB_DEF_CNT6)->SetFont(&m_Bigfont);
	//UpdateData(FALSE);
}

LRESULT CDlgReportDefData::m_fnInitState(WPARAM wParam, LPARAM lParam)
{
	
	CString strTemp = (LPCTSTR) lParam;
	
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT6, strTemp);
	return 1;
}

void CDlgReportDefData::OnDestroy() 
{
	if(mp_edit != NULL){
		for(int i = 0; i < m_row; i++){
			for(int sub_i = 0; sub_i < m_column; sub_i++){
				delete mp_edit[i*m_column + sub_i];
				delete mp_DefLbl[i*m_column + sub_i];
			}
		}
		delete[] mp_edit;
		delete[] mp_DefLbl;
	}

	CDialog::OnDestroy();
}

LRESULT CDlgReportDefData::m_fnUpdateStatsData(WPARAM wParam, LPARAM lParam)
{
	vector<CStringArray*>* listRptPnlData;
	CStringArray cArryLabel;
	listRptPnlData = (vector<CStringArray*>*)lParam;
	//if (listRptPnlData->size() > 0)
	//{
	//	for (int nRowCnt = 0 ; nRowCnt < listRptPnlData->size();nRowCnt++)
	//	{
	//		/*if ( nRowCnt == 0)
	//		{
	//		stRptPnlData = listRptPnlData->at(nRowCnt);

	//		for (int nColCnt = 0 ; nColCnt < MAX_PANEL_DATA ; nColCnt++)
	//		{
	//		cArryLabel.Add(stRptPnlData->tPnlData[nColCnt]);
	//		}				
	//		}*/
	//		
	//	}
	//}
	//m_fnUpdateLabel(&cArryLabel);

	//m_nfUpdateEdit(listRptPnlData);
	//if()
	//ResetEvent(m_H_DefData);
	
	//ResetEvent(m_H_DefData);
	return TRUE;
}

int CDlgReportDefData::m_nfUpdateEdit(vector<CStringArray*>* listList)
{

	//m_H_DefData = CreateEvent( NULL, TRUE,  FALSE,  NULL ); 
	SetEvent(m_H_DefData);

	CStringArray *stPnalItem;
	LONG	lTotalCnt = 0;
	CString strTemp = _T("0"), strPer = _T("0");
	CStringArray strAryPnlItem, strAryPnlData, strAryPnlCode;
	int nDefItemCnt = 0, nDefCnt = 0;// = listList->size();	
	int nNgCnt = 0, nOkCnt = 0 , nCfCnt = 0, nTftCnt = 0, nAddDefect[47] = {0,};	
	BOOL bDefItemFlg = FALSE, bRet = FALSE;
	////////////////////////////// TOTAL /////////////////////////////////////////////
	vector<CStringArray*>::iterator iVect; 
	//boost::unordered_map
	//CAtlMap
	strAryPnlData.SetSize(MAX_PANEL_DATA);
	strAryPnlItem.SetSize(MAX_PANEL_DATA);
	int nRowCnt =listList->size();
	if (nRowCnt == 0)
	{
		return FALSE;
	}
		//iVect = listList->end();	
		//iVect--;
	nRowCnt = 0;
	for(iVect=listList->begin(); iVect != listList->end(); iVect++)
	//while(iVect != listList->begin())
	{
		
		if ( nRowCnt != 0)
		{
			//stRptPnlData = listList->at(nRowCnt);
			//*iVect = listList->at(nRowCnt);	
			//stRptPnlData = *iVect;//
			strTemp = (*iVect)->GetAt(ENUM_TOTAL_GRADE);
			if (!strTemp.IsEmpty())
			{
				if(strTemp != JUDGE_A) nNgCnt++;
				else				   nOkCnt++;
			}
			
			strTemp =  (*iVect)->GetAt(ENUM_CF_DEFECT);
			if (!strTemp.IsEmpty())		nCfCnt+= _ttoi(strTemp);
			strTemp =  (*iVect)->GetAt(ENUM_TFT_DEFECT);
			if (!strTemp.IsEmpty())		nTftCnt+= _ttoi(strTemp);
						
			parallel_for(ENUM_DUST, ENUM_ETC_LINE, [&](int ncolCnt)
			//for (int ncolCnt = ENUM_DUST; ncolCnt <= ENUM_ETC_LINE ; ncolCnt++)
			{				
				if (_ttoi( (*iVect)->GetAt(ncolCnt)) > 0 )
				{					
					int nAddCnt = _ttoi(strAryPnlData[ncolCnt - ENUM_DUST]) + _ttoi(((*iVect)->GetAt(ncolCnt)));
					strAryPnlData[ncolCnt - ENUM_DUST].Format(_T("%d"), nAddCnt);							
					strAryPnlItem[ncolCnt - ENUM_DUST].Format(_T("%s"), stPnalItem->GetAt(ncolCnt));					
				}			
				//delete[] stRptPnlData->tPnlData[ncolCnt];
			});
			//delete stRptPnlData;
			//delete *iVect;			
			//listList->erase(iVect++);
		}
		if (nRowCnt == 0)
			stPnalItem = *iVect;//listList->at(0);
		nRowCnt++;
		//listList->pop_back();
		//listList.push_back(iVect);
		//iVect--;
	}
	//iVect = listList->erase(iVect);

	if (nNgCnt == 0)
		strPer.Format(_T("00.00"));
	else
	{
		if ((nNgCnt *100) / (nNgCnt + nOkCnt) < 10)
			strPer.Format(_T("0%.2f"),(float)(nNgCnt *100)/(nNgCnt + nOkCnt));
		else
			strPer.Format(_T("%.2f"),(float)(nNgCnt *100)/(nNgCnt + nOkCnt));
	}
	strTemp.Format(_T("%s%%  /  %d"), strPer, nNgCnt);
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT1, strTemp);

	if (nOkCnt == 0)
		strPer.Format(_T("00.00"));
	else
	{
		if ((nOkCnt *100) / (nNgCnt + nOkCnt) < 10)
			strPer.Format(_T("0%.2f"),(float)(nOkCnt *100)/(nNgCnt + nOkCnt));
		else
			strPer.Format(_T("%.2f"),(float)(nOkCnt *100)/(nNgCnt + nOkCnt));
	}
	strTemp.Format(_T("%s%%  /  %d"), strPer, nOkCnt);
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT2, strTemp);


	strTemp.Format(_T("%d"), (nNgCnt + nOkCnt));
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT3, strTemp);
	strTemp.Format(_T("%d"), nCfCnt);
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT4, strTemp);
	strTemp.Format(_T("%d"), nTftCnt);
	::SetDlgItemText(GetSafeHwnd(), IDC_LB_DEF_CNT5, strTemp);
	nDefItemCnt = 0;

	m_fnSortOrder(strAryPnlItem, strAryPnlData);
	//////////////////////////////////////////////////////////////////////////////////

	nDefItemCnt = strAryPnlData.GetCount();
	
	int nrow = nDefItemCnt / D_TEXT_DIS_LINE; if (nDefItemCnt % D_TEXT_DIS_LINE > 0)	nrow++;

	int ncol = nDefItemCnt / D_TEXT_DIS_LINE ? D_TEXT_DIS_LINE:nDefItemCnt;
	//m_row = nrow;
	int ncnt = 0;
	if(mp_edit != NULL){
		for(int i = 0; i < nrow; i++){
			for(int sub_i = 0; sub_i < ncol; sub_i++){
// 				if (mp_edit[i*ncol + sub_i])
// 				{
					delete mp_edit[i*ncol + sub_i];
					delete mp_DefLbl[i*ncol + sub_i];
					delete mp_DefPer[i*ncol + sub_i];

					mp_edit[i*ncol + sub_i] = NULL;
					mp_DefLbl[i*ncol + sub_i] = NULL;
					mp_DefPer[i*ncol + sub_i] = NULL;
				//}		
					ncnt++;
			}
		}
		delete[] mp_edit;
		delete[] mp_DefLbl;
		delete[] mp_DefPer;
		mp_edit = NULL;
	}
	m_row = nrow;
	m_column = ncol;
	int ngetCnt=0;

	mp_edit	  = new CEdit *[nrow*ncol];
	mp_DefLbl = new CEdit *[nrow*ncol];
	mp_DefPer = new CEdit *[nrow*ncol];
	CRect rect;	
	GetDlgItem(IDC_LABEL_IP)->GetWindowRect(rect);
	GetDlgItem(IDC_LB_DF_IP)->GetWindowRect(rect);	
	ScreenToClient(&rect);
	
	DWORD dwStyle = WS_EX_CLIENTEDGE | WS_CHILD | WS_VISIBLE |  
		ES_AUTOHSCROLL | WS_DISABLED ;  

	ncnt = 0;
	for(int i = 0; i < nrow; i++){
		for(int sub_i = 0; sub_i < ncol; sub_i++){
			
			if (ngetCnt >= nDefItemCnt)	break;

			mp_edit  [i*ncol + sub_i] = new CEdit(); 	
			mp_DefPer[i*ncol + sub_i] = new CEdit(); 	
			mp_DefLbl[i*ncol + sub_i] = new CEdit();

			ncnt++;
			for (int nchk = ngetCnt ; nchk < strAryPnlData.GetSize(); nchk++)
			{

				if (_ttoi(strAryPnlData[nchk]) > 0 )
				{					
					mp_DefLbl[i*ncol + sub_i]->Create(dwStyle, rect,this, IDC_LABEL_IP);
					mp_DefPer[i*ncol + sub_i]->Create(dwStyle, rect,this, IDC_LABEL_IP);
					mp_edit	 [i*ncol + sub_i]->Create(dwStyle, rect,this, IDC_LB_DF_IP);					

					mp_DefLbl[i*ncol + sub_i]->MoveWindow(10  + sub_i*250, 50 + i*22, 100, 20);	
					mp_DefPer[i*ncol + sub_i]->MoveWindow(110 + sub_i*250, 50 + i*22, 100 , 20);
					mp_edit	 [i*ncol + sub_i]->MoveWindow(170 + sub_i*250, 50 + i*22, 50 , 20);
					
					mp_DefLbl[i*ncol + sub_i]->SetFont(&m_font);
					mp_DefPer[i*ncol + sub_i]->SetFont(&m_font);
					mp_edit	 [i*ncol + sub_i]->SetFont(&m_font);
					
					strTemp.Format(_T("%d"), _ttoi(strAryPnlData[nchk]));					
					mp_edit[i*ncol + sub_i]->SetWindowText(strTemp);

					strTemp.Format(_T("%s"), strAryPnlItem[nchk]);
					mp_DefLbl[i*ncol + sub_i]->SetWindowText(strTemp);

					if ((int)((_ttoi(strAryPnlData[nchk]) *100) / (nCfCnt + nTftCnt)) < 10)
						strTemp.Format(_T(": 0%.2f%% / "),(float)((_ttof(strAryPnlData[nchk]) *100)/(nCfCnt + nTftCnt)));
					else
						strTemp.Format(_T(": %.2f%% / "),(float)((_ttof(strAryPnlData[nchk]) *100)/(nCfCnt + nTftCnt)));
					
					mp_DefPer[i*ncol + sub_i]->SetWindowText(strTemp);

					ngetCnt = nchk + 1;
					break;
				}
				
			}

		}
	}
	this->SetRedraw();
	strAryPnlData.RemoveAll();
	strAryPnlItem.RemoveAll();
	strAryPnlCode.RemoveAll();
	bRet = TRUE;
	//CloseHandle(m_H_DefData);
	ResetEvent(m_H_DefData);
	
	return bRet;
}

int CDlgReportDefData::m_fnUpdateLabel(CStringArray* cArryLabel)
{
	CString strTemp ;
	int nColcnt = cArryLabel->GetCount();
	for (int i= 0; i < nColcnt; i++)
	{
		strTemp = cArryLabel->GetAt(i);
	}	

	//CFormView::OnInitialUpdate();
	if(mp_edit != NULL){
		for(int i = 0; i < m_row; i++){
			for(int sub_i = 0; sub_i < m_column; sub_i++){
				delete mp_edit[i*m_column + sub_i];
			}
		}
		delete[] mp_edit;
	}

	mp_DefLbl = new CEdit *[m_row*m_column];
	for(int i = 0; i < nColcnt; i++){
		for(int sub_i = 0; sub_i < m_column; sub_i++){
			mp_DefLbl[i*m_column + sub_i] = new CEdit(); 
			mp_DefLbl[i*m_column + sub_i]->CreateEx(WS_EX_CLIENTEDGE, 
				_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | WS_DISABLED , 
				CRect(30 + sub_i*50, 100 + i*24, 30 + (sub_i+1)*50, 100 + (i+1)*24), 
				this, 20000 + i*m_column + sub_i);
		}
	}
	cArryLabel->RemoveAll();
	return TRUE;
}

int CDlgReportDefData::m_fnSortOrder(CStringArray &cArryLabel, CStringArray &cArryData)
{
	CStringArray *carryLblTemp;
	CStringArray *carryDatTemp;
	CString strTemp, strLbl;
	carryDatTemp = &cArryData;
	carryLblTemp = &cArryLabel;
	
	for (int i=0; i< cArryData.GetCount(); i++)
	{
		for (int j=0; j< cArryData.GetCount(); j++)
		{
			int a = _ttoi(cArryData.GetAt(i)) ;
			int b = _ttoi(carryDatTemp->GetAt(j)) ;
			if (a > 0 && a > b)
			{
				strTemp = cArryData[i];
				cArryData.SetAt(i, carryDatTemp->GetAt(j));
				cArryData[j] = strTemp;		
				
				strLbl = cArryLabel[i];
				cArryLabel.SetAt(i, carryLblTemp->GetAt(j));
				cArryLabel[j] = strLbl;					
			}
		}
	}
	return TRUE;
}

void CDlgReportDefData::m_fnCreateFont()
{
	m_font.CreateFont(
		10,                     // 글자높이
		5,                     // 글자너비
		0,                      // 출력각도
		0,                      // 기준 선에서의각도
		FW_HEAVY,              // 글자굵기
		FALSE,                  // Italic 적용여부
		FALSE,                  // 밑줄적용여부
		FALSE,                  // 취소선적용여부
		DEFAULT_CHARSET,       // 문자셋종류
		OUT_DEFAULT_PRECIS,    // 출력정밀도
		CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
		DEFAULT_QUALITY,       // 출력문자품질
		DEFAULT_PITCH,         // 글꼴Pitch
		_T("굴림체")           // 글꼴
		);

	m_Bigfont.CreateFont(
		22,                     // 글자높이
		15,                     // 글자너비
		0,                      // 출력각도
		0,                      // 기준 선에서의각도
		FW_HEAVY,              // 글자굵기
		FALSE,                  // Italic 적용여부
		FALSE,                  // 밑줄적용여부
		FALSE,                  // 취소선적용여부
		DEFAULT_CHARSET,       // 문자셋종류
		OUT_DEFAULT_PRECIS,    // 출력정밀도
		CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
		DEFAULT_QUALITY,       // 출력문자품질
		DEFAULT_PITCH,         // 글꼴Pitch
		_T("굴림체")           // 글꼴
		);
}
