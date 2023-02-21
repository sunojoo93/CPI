
// ViewReport.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
#include "stdafx.h"
#include "CruxServer.h"
#include "ViewReport.h"
#include "ViewAuto.h"
#include "MainFrm.h"

//#include ".\Core\History.h"



// CViewReport

IMPLEMENT_DYNCREATE(CViewReport, CFormView)

CViewReport::CViewReport()
	: CFormView(CViewReport::IDD)
{
//	m_pDateTimeStart			 = NULL;
	m_peditStartTime			 = NULL;
	m_pspinStartTime			 = NULL;
//	m_pDateTimeEnd				 = NULL;
	m_peditEndTime				 = NULL;
	m_pspinEndTime				 = NULL;
	m_pGridCtrl					 = NULL;
								 
	m_hThreadFileLoad			 = NULL;
//	m_bThreadFileLoad			 = FALSE;
								 
	m_pLoadingDlg				 = NULL;
								 
	m_psReportList				 = NULL;
								 
//	m_pcomboSelectBar			 = NULL;

	m_pstaticGraphBkColor		 = NULL;
	m_pstaticGraphAxisColor		 = NULL;
	m_pstaticGraphRatioTextColor = NULL;
	m_pstaticGraphBarTextColor   = NULL;
	m_pstaticGraphBarListColor   = NULL;

	for (int i =0 ; i < 4; i ++)
	{			
		m_DlgRptDefData[i]	 	 = NULL;		
	}
	for (int i =0 ; i < 3; i ++)
	{
		m_peditStatsTime[i] 	 = NULL;
	}
	m_pDlgLogin					 = NULL;
	m_hThreadHandle				 = NULL;
	
}

CViewReport::~CViewReport()
{
	if(m_pLoadingDlg)
	{
	//	m_pLoadingDlg->DestroyWindow();
		delete m_pLoadingDlg;
		m_pLoadingDlg = NULL;
	}
	if(m_pGridCtrl)
	{
		m_pGridCtrl->DestroyWindow();
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}

	if(m_psReportList)
	{
		delete m_psReportList;
		m_psReportList = NULL;
	}

	if(m_pstaticGraphBkColor)
	{
		delete m_pstaticGraphBkColor;
		m_pstaticGraphBkColor = NULL;
	}
	if(m_pstaticGraphAxisColor)
	{
		delete m_pstaticGraphAxisColor;
		m_pstaticGraphAxisColor = NULL;
	}
	if(m_pstaticGraphRatioTextColor)
	{
		delete m_pstaticGraphRatioTextColor;
		m_pstaticGraphRatioTextColor = NULL;
	}
	if(m_pstaticGraphBarTextColor)
	{
		delete m_pstaticGraphBarTextColor;
		m_pstaticGraphBarTextColor = NULL;
	}
	if(m_pstaticGraphBarListColor)
	{
		delete m_pstaticGraphBarListColor;
		m_pstaticGraphBarListColor = NULL;
	}

	if (m_pDlgLogin)
	{
		delete m_pDlgLogin;
		m_pDlgLogin = NULL;
	}

	for (int i =0 ; i < 4; i ++)
	{			
		if (m_DlgRptDefData[i])
		{
			delete m_DlgRptDefData[i];
			m_DlgRptDefData[i]	 	 = NULL;		
		}
		
	}
	if (m_listReportPnlData.size() > 0) // 백터에 값이 있을 경우 초기화 진행
	{
		if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThreadHandle, INFINITE))
		//m_fnPanelDataClear(m_listReportPnlData);
		m_listReportPnlData.clear();
		m_listReportp[0]->clear();
		m_listReportp[1]->clear();
		m_listReportp[2]->clear();
	}	
	m_pDlgLogin					 = NULL;
}

void CViewReport::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRAPH, m_Graph);
	DDX_Control(pDX, IDC_COMBO_BAR, m_comboSelectBar);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_DateTimeStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_DateTimeEnd);
}

BEGIN_MESSAGE_MAP(CViewReport, CFormView)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_TIME, &CViewReport::OnDeltaposSpinStartTime)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_TIME, &CViewReport::OnDeltaposSpinEndTime)
	ON_EN_CHANGE(IDC_EDIT_START_TIME, &CViewReport::OnChangeEditStartTime)
	ON_EN_CHANGE(IDC_EDIT_END_TIME, &CViewReport::OnChangeEditEndTime)
	ON_CBN_SELCHANGE(IDC_COMBO_BAR, &CViewReport::OnSelchangeComboBar)	
	ON_MESSAGE(UM_MAINMENU_CHANGE,	OnMainMenuChange)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CViewReport::OnBnClickedSave)
END_MESSAGE_MAP()


// CViewReport 진단입니다.

#ifdef _DEBUG
void CViewReport::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewReport::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewReport 메시지 처리기입니다.
void CViewReport::m_fnStatsListUpdate(BOOL bRetryFlg /*= TRUE*/)
{
	CString cCountRoot, strTemp, strStatsTime ;	
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);	
	int nSectionFlg = 0;
	TCHAR szSectionCnt[3];


	for (int i =0 ; i < REPORT_LIST_MAX + 1 ; i ++)
	{
		strTemp.Format(_T("STATS_TIME%d"), i + 1);
		GetPrivateProfileString(EQP_STATE, strTemp, _T(""), szSectionCnt, sizeof(szSectionCnt), cCountRoot);
		strStatsTime = szSectionCnt;
		if( m_DlgRptDefData[i] && (i != 3 && strStatsTime.IsEmpty()))
		{ 
			if(m_DlgRptDefData[i])
			{
				delete m_DlgRptDefData[i];
				m_DlgRptDefData[i] = NULL;
			}
		}
		
		if(m_DlgRptDefData[i]== NULL && (i == 3 || !strStatsTime.IsEmpty()))
		{ 
			if(m_DlgRptDefData[i])
			{
				delete m_DlgRptDefData[i];
				m_DlgRptDefData[i] = NULL;
			}
			m_DlgRptDefData[i] = new CDlgReportDefData();
			m_DlgRptDefData[i]->Create(IDD_DLG_REPORT_DEF_INFO, this);			
			m_DlgRptDefData[i]->ShowWindow(SW_SHOW);			
			m_DlgRptDefData[i]->MoveWindow(103, 80 + (nSectionFlg * 195) , 1075 , 190);
		}
		nSectionFlg++;
		
	}

}

void CViewReport::OnInitialUpdate()
{	
	CFormView::OnInitialUpdate();
	m_fnStatsListUpdate();

	CString cCountRoot, strTemp, strStatsTime ;	
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);	
	TCHAR szSectionCnt[3];
	for (int i =0 ; i < REPORT_LIST_MAX; i++)
	{
		strTemp.Format(_T("STATS_TIME%d"), i + 1);
		GetPrivateProfileString(EQP_STATE, strTemp, _T(""), szSectionCnt, sizeof(szSectionCnt), cCountRoot);
		strStatsTime = szSectionCnt;

		if (m_peditStatsTime[i]	== NULL)
		{
			m_peditStatsTime[i]	 = (CEdit *)GetDlgItem(IDC_EDIT_RESET_TIME1 + i);
			m_peditStatsTime[i]->SetWindowText(strStatsTime);
			::SendMessage(m_peditStatsTime[i]->GetSafeHwnd(), EM_LIMITTEXT, 2, 0);
		}		

		m_listReportp[i] = new vector<CStringArray*>();
	}
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	SYSTEMTIME systime;
	::GetLocalTime(&systime);
	COleDateTime datetime;
	datetime.SetDateTime(systime.wYear, systime.wMonth, systime.wDay, 0, 0, 0);
	int nHour = systime.wHour;
	TCHAR tszTime[MAX_PATH] = {0,};
	_stprintf_s(tszTime, MAX_PATH, _T("%d"), nHour);

//	if(m_pDateTimeStart == NULL)
	if(m_DateTimeStart.GetSafeHwnd())
	{
//		m_pDateTimeStart = (CDateTimeCtrl *)GetDlgItem(IDC_DATETIMEPICKER_START);		
//		m_pDateTimeStart->SetTime(datetime);
		m_DateTimeStart.SetTime(datetime);
	}
	if(m_peditStartTime == NULL)
	{
		m_peditStartTime = (CEdit *)GetDlgItem(IDC_EDIT_START_TIME);
		m_peditStartTime->SetWindowText(tszTime);
		::SendMessage(m_peditStartTime->GetSafeHwnd(), EM_LIMITTEXT, 2, 0);
	}
	if(m_pspinStartTime == NULL)
	{
		m_pspinStartTime = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_START_TIME);
		m_pspinStartTime->SetRange(23, 0);
	}

	if(m_DateTimeEnd.GetSafeHwnd())
	{
		m_DateTimeEnd.SetTime(datetime);
	}
	if(m_peditEndTime == NULL)
	{
		m_peditEndTime = (CEdit *)GetDlgItem(IDC_EDIT_END_TIME);
		m_peditEndTime->SetWindowText(tszTime);
		::SendMessage(m_peditEndTime->GetSafeHwnd(), EM_LIMITTEXT, 2, 0);
	}
	if(m_pspinEndTime == NULL)
	{
		m_pspinEndTime = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_END_TIME);
		m_pspinEndTime->SetRange(23, 0);
	}

	if(m_pLoadingDlg == NULL)
	{
		m_pLoadingDlg = new CLoadingDlg;
	//	m_pLoadingDlg->Create(CLoadingDlg::IDD, this);
	}

	if(m_psReportList == NULL)
	{
		m_psReportList = new stReportList;
		memset(m_psReportList, 0, sizeof(stReportList));
	}
	UpdateData(FALSE);

	if (m_listReportPnlData.size() > 0) // 백터에 값이 있을 경우 초기화 진행
	{
		if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThreadHandle, INFINITE))
			m_fnPanelDataClear(m_listReportPnlData);
		m_listReportPnlData.clear();
		m_listReportp[0]->clear();
		m_listReportp[1]->clear();
		m_listReportp[2]->clear();
	}	
	COleDateTime t1 = COleDateTime::GetCurrentTime();

	//2016.07.22		Mr.Chu 현재 s/w 실행 시 간헐적 검사Data Reading 중 프로그램 다운과 실행안됨으로 주석처리. 
	//사용 시 함수 내 m_fnStatsSearchData 함수 수정할것.
	//FindReportData(t1);		

	m_fnSortStatsData();

}

void CViewReport::OnDeltaposSpinStartTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	pNMUpDown->iDelta = -pNMUpDown->iDelta;

	*pResult = 0;
}


void CViewReport::OnDeltaposSpinEndTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	pNMUpDown->iDelta = -pNMUpDown->iDelta;

	*pResult = 0;
}

void CViewReport::OnChangeEditStartTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_peditStartTime == NULL)
	{
		return;
	}
	if(m_peditStartTime->GetSafeHwnd() == NULL)
	{
		return;
	}
	TCHAR tszValue[3] = {0,};
	UpdateData(FALSE);
//	m_peditStartTime->GetWindowText(tszValue, 3);
//	::GetWindowText(m_peditStartTime->GetSafeHwnd(), tszValue, 3);
	::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, tszValue, 3);
	int nlength = _tcslen(tszValue);
	if(nlength == 0)
	{
	//	m_peditStartTime->SetWindowText(_T("0"));
		::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, _T("0"));
	}
	else
	{
		int nValue = _ttoi(tszValue);
		if(nValue < 0)
		{
		//	m_peditStartTime->SetWindowText(_T("0"));
			::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, _T("0"));
		}
		else if(nValue > 23)
		{
		//	m_peditStartTime->SetWindowText(_T("23"));
			::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, _T("23"));
		}
		else
		{
			if(nlength == 2)
			{
				if(tszValue[0] == _T('0'))
				{
					nlength = _stprintf_s(tszValue, 3, _T("%d"), nValue);
					tszValue[nlength] = _T('\0');
				//	m_peditStartTime->SetWindowText(tszValue);
					::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, tszValue);
				}
			}
		}
	}
	UpdateData();
}


void CViewReport::OnChangeEditEndTime()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CFormView::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_peditEndTime == NULL)
	{
		return;
	}
	if(m_peditEndTime->GetSafeHwnd() == NULL)
	{
		return;
	}
	TCHAR tszValue[3] = {0,};
//	m_peditEndTime->GetWindowText(tszValue, 3);
	::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, tszValue, 3);
	int nlength = _tcslen(tszValue);
	if(nlength == 0)
	{
	//	m_peditEndTime->SetWindowText(_T("0"));		
		::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, _T("0"));
	}
	else
	{
		int nValue = _ttoi(tszValue);
		if(nValue < 0)
		{
		//	m_peditEndTime->SetWindowText(_T("0"));
			::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, _T("0"));
		}
		else if(nValue > 23)
		{
		//	m_peditEndTime->SetWindowText(_T("23"));
			::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, _T("23"));
		}
		else
		{
			if(nlength == 2)
			{
				if(tszValue[0] == _T('0'))
				{
					int nlength = 0;
					nlength = _stprintf_s(tszValue, 3, _T("%d"), nValue);
					tszValue[nlength] = _T('\0');
				//	m_peditEndTime->SetWindowText(tszValue);
					::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, tszValue);
				}
			}
		}
	}
}
BEGIN_EVENTSINK_MAP(CViewReport, CFormView)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SEARCH, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSearch, VTS_NONE)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SELECT_COLOR_BK, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSelectColorBk, VTS_NONE)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SELECT_COLOR_AXIS, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSelectColorAxis, VTS_NONE)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SELECT_COLOR_RATIO_TEXT, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSelectColorRatioText, VTS_NONE)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SELECT_COLOR_BAR_TEXT, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSelectColorBarText, VTS_NONE)
	ON_EVENT(CViewReport, IDC_GXBUTTONEXCTRL_SELECT_COLOR_BAR, DISPID_CLICK, CViewReport::ClickGxbuttonexctrlSelectColorBar, VTS_NONE)
END_EVENTSINK_MAP()


void CViewReport::ClickGxbuttonexctrlSearch()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 스레드를 기동한다.
	if(m_hThreadFileLoad == NULL)
	{
		UINT uiThreadFileLoad ;
		//	m_bThreadFileLoad = TRUE;
		m_hThreadFileLoad = (HANDLE)::_beginthreadex(NULL, 0, ThreadFileLoad, (void *)this, 0, &uiThreadFileLoad);
	}

	// 모달창을 띄운다.	
	
	if(m_pLoadingDlg)
	{
		delete m_pLoadingDlg;
		m_pLoadingDlg = NULL;
		m_pLoadingDlg = new CLoadingDlg;
		m_pLoadingDlg->DoModal();
	}
}

unsigned __stdcall CViewReport::ThreadFileLoad(void *pArg)
{
	CViewReport *pView = (CViewReport *)pArg;
	//while(pDlg->m_bThreadFileLoad)
	//{
	//	Sleep(10);
	//}

	pView->ReportFileLoad();

	if(pView->m_hThreadFileLoad)
	{
		CloseHandle(pView->m_hThreadFileLoad);
		pView->m_hThreadFileLoad = NULL;
	}

//	_endthreadex(0);

	return 0;
}

void CViewReport::ReportFileLoad()
{	
	Sleep(500);	// 로딩다이얼로그를 띄울 시간을 준다
	//int nlength = 0;
	TCHAR tszTempHour[MAX_PATH] = {0,};
	COleDateTimeSpan tempdatetimespan;
	int nTempDateTimeSpan = 0;

	int nTempYear, nTempMonth, nTempDay, nTempHour, nTempMin, nTempSec;
	
	nTempYear  = 0;
	nTempMonth = 0;
	nTempDay   = 0;
	nTempHour  = 0;
	nTempMin   = 0;
	nTempSec   = 0;

	memset(m_psReportList, 0, sizeof(stReportList));
	
	COleDateTimeSpan timeSpan;
	COleDateTime startDateTime, endDateTime;

	m_bFirRead = TRUE;
	m_bEndRead = FALSE;

// 	if (m_listReportPnlData.size() > 0) // 백터에 값이 있을 경우 초기화 진행
// 	{
		 //if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThreadHandle, INFINITE))
	WaitForSingleObject(m_hThreadHandle, INFINITE);
	m_fnPanelDataClear(m_listReportPnlData);
		
			
		//m_fnPanelDataClear(m_listReportPnlData);
		m_listReportPnlData.clear();		
		m_listReportp[0]->clear();
		m_listReportp[1]->clear();
		m_listReportp[2]->clear();
	//}	

	// DateTimePicker컨트롤의 데이터와 에디트 컨트롤의 값을 합친다.
//	m_pDateTimeStart->GetTime(startDateTime);
	m_DateTimeStart.GetTime(startDateTime);
//	m_peditStartTime->GetWindowText(tszTempHour, MAX_PATH);
	::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_START_TIME, tszTempHour, MAX_PATH);
	nTempDateTimeSpan = _ttoi(tszTempHour);
	tempdatetimespan.SetDateTimeSpan(0, nTempDateTimeSpan, 0, 0);
	startDateTime += tempdatetimespan;
		
//	m_pDateTimeEnd->GetTime(endDateTime);
	m_DateTimeEnd.GetTime(endDateTime);
//	m_peditEndTime->GetWindowText(tszTempHour, MAX_PATH);
	::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_END_TIME, tszTempHour, MAX_PATH);
	nTempDateTimeSpan = _ttoi(tszTempHour);
	tempdatetimespan.SetDateTimeSpan(0, nTempDateTimeSpan, 0, 0);
	endDateTime += tempdatetimespan;


	m_startDateTime = startDateTime;
	m_EndDateTime	= endDateTime;


	// 기간의 차를 구한다.
	timeSpan = endDateTime - startDateTime;
//	double dbTimeRange = timeSpan.GetTotalSeconds();
	double dbTimeRange	   = timeSpan.GetTotalDays();
	//double dbTimeRangeHour = timeSpan.GetTotalHours();	
	//double dbTimeRangeDay  = timeSpan.GetTotalDays();	
	//CString strYear		 = _T("");	CString strMonth	 = _T("");	CString strDay		 = _T("");
	//CString strHour		 = _T("");	CString strWriteTerm = _T("");	CString strSum		 = _T("");


	if(dbTimeRange < 0.)
	{
		// 기간이 나중이 빠를경우 Start와 End의 순서를 바꾼다.
		nTempYear  = startDateTime.GetYear();
		nTempMonth = startDateTime.GetMonth();
		nTempDay   = startDateTime.GetDay();
		nTempHour  = startDateTime.GetHour();
		nTempMin   = startDateTime.GetMinute();
		nTempSec   = startDateTime.GetSecond();

		startDateTime.SetDateTime(endDateTime.GetYear(), endDateTime.GetMonth(), endDateTime.GetDay(), endDateTime.GetHour(), endDateTime.GetMinute(), endDateTime.GetSecond());
		endDateTime.SetDateTime(nTempYear, nTempMonth, nTempDay, nTempHour, nTempMin, nTempSec);
	}
	
	COleDateTimeSpan timeSpan2;
	COleDateTimeSpan timeSpan3;
	if(m_pLoadingDlg)
	{
		timeSpan    = endDateTime - startDateTime;
		m_pLoadingDlg->SetTotalTime(timeSpan.GetTotalDays());	// 전체 시간을 구한다.
	}
	while(1)
	{		
		timeSpan2    = endDateTime - startDateTime;	// 증가되었을때 시간을 구한다.
		timeSpan3    = timeSpan	- timeSpan2;
		m_pLoadingDlg->SetCurTime(timeSpan3.GetTotalDays());
		
		dbTimeRange	    = timeSpan2.GetTotalDays();
		//dbTimeRangeDay  = timeSpan2.GetTotalDays();
		//dbTimeRangeHour = timeSpan2.GetTotalHours();
		//if (dbTimeRangeDay == 0 && dbTimeRange < 24)

		//strYear.Format( _T("%04d"),		startDateTime.GetYear() );
		//strMonth.Format(_T("%02d"),		startDateTime.GetMonth());
		//strDay.Format(  _T("%02d"),		startDateTime.GetDay()  );
		//strHour.Format( _T("%02d"),		startDateTime.GetHour() );

		//dbTimeRangeHour = dbTimeRangeDay * 100 + dbTimeRangeHour;
		if (dbTimeRange <= 0 /*dbTimeRangeHour <= 23*/)
		{
			//startDateTime = endDateTime;
			startDateTime.SetDateTime(startDateTime.GetYear(), startDateTime.GetMonth(), startDateTime.GetDay(), 
				endDateTime.GetHour(), endDateTime.GetMinute(), endDateTime.GetSecond());
			m_bEndRead = TRUE;
		}

		if(dbTimeRange < 0.)
		{					
			break;
		}

		else if(dbTimeRange == 0.)
		{
			m_pLoadingDlg->SetCurTime(timeSpan.GetTotalDays());
		}

	
		FindReportData(startDateTime);
		
		startDateTime.SetDateTime(startDateTime.GetYear(), startDateTime.GetMonth(), startDateTime.GetDay(), 
									0, endDateTime.GetMinute(), endDateTime.GetSecond());
		
		tempdatetimespan.SetDateTimeSpan(1, 0, 0, 0);	// 1시간씩 증가시켜 기간을 비교한다.
		startDateTime += tempdatetimespan;
		//m_bFirRead =FALSE;
		if(m_bEndRead == TRUE)
			break;
	}
	m_fnSortStatsData();
	
	//Sleep(700);
	if(m_pLoadingDlg)
	{
		m_pLoadingDlg->CloseDialog();
	}

	//UpdateGridCtrl();

	//m_Graph.SetValue(m_psReportList);
	//m_Graph.Refresh();
}

void CViewReport::FindReportData(COleDateTime &StartDateTime)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	STRU_REPORT_INFO stReportInfo;
	stReportInfo.init();
	CString strPath = pView->m_fnGetPanelSummaryDataPath();
	_tcscpy(stReportInfo.tDataPath, strPath);

	int nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);
	CString strYear		 = _T("");	CString strMonth	 = _T("");	CString strDay		 = _T("");
	CString strHour		 = _T("");	CString strWriteTerm = _T("");	CString strSum		 = _T("");

	strYear.Format( _T("%04d"),		StartDateTime.GetYear() );
	strMonth.Format(_T("%02d"),		StartDateTime.GetMonth());
	strDay.Format(  _T("%02d"),		StartDateTime.GetDay()  );
	strHour.Format( _T("%02d"),		StartDateTime.GetHour() );
	strSum.Format(_T("%s%s%s%s%s"),strYear, strMonth, strDay, strHour, strWriteTerm);
	_tcscpy(stReportInfo.tDataTime, strSum);
	
	stReportInfo.stDataTime.nYear			=   StartDateTime.GetYear() ;
	stReportInfo.stDataTime.nMonth			=   StartDateTime.GetMonth();
	stReportInfo.stDataTime.nDay			=   StartDateTime.GetDay()  ;
	stReportInfo.stDataTime.nHour			=   StartDateTime.GetHour() ;
	stReportInfo.stDataTime.nWriteTerm		=   nWriteTerm ;
	
	m_fnStatsSearchData(stReportInfo);
	m_stReportInfo = stReportInfo;
}



void CViewReport::UpdateGridCtrl()	// 그리드의 일반셀을 갱신하는 함수
{
	if(m_psReportList->nTotal == 0)
	{
		m_pGridCtrl->SetRedraw(FALSE);
		///////////////////////////////////// total //////////////////////////////////		
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOTAL, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOTAL, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////	

		///////////////////////////////////// Good //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_GOOD, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_GOOD, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Defect //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_DEFECT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_DEFECT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Inspect Fail //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_INSPECT_FAIL, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_INSPECT_FAIL, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 상부이름 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_NAME, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_NAME, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 상부찍힘 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_STABBED, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_STABBED, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 하부기포 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_BUBBLE, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_BUBBLE, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 하부이상 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_ERROR, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_ERROR, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 색얼룩 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_COLOR_SPOT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_COLOR_SPOT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 원형얼룩 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_CIRCLE_SPOT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_CIRCLE_SPOT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Monomer 뭉침 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_MONOMER_UNITE, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_MONOMER_UNITE, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Film 꺽임 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_FILM_ROLL_LINE, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_FILM_ROLL_LINE, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Wave //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_WAVE, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_WAVE, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 탈막 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_WASHING, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_WASHING, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Pad burnt //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Pad Scratch //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Pad 이상 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_ERROR, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_ERROR, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Pad 찍힘 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_STABBED, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_STABBED, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// 컷불량 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_CUT_ERROR, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_CUT_ERROR, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// Pad 뜯김 //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_PULLED_OUT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_PULLED_OUT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////

		///////////////////////////////////// C-Cut //////////////////////////////////
		//m_pGridCtrl->SetItemText(REPORT_ROW_C_CUT, REPORT_COUNT_COL, _T(""));
		//m_pGridCtrl->SetItemText(REPORT_ROW_C_CUT, REPORT_RATIO_COL, _T(""));
		///////////////////////////////////////////////////////////////////////////////
			
		/////////////////////////////////// DUST //////////////////////////////////		
		m_pGridCtrl->SetItemText(REPORT_ROW_DUST, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_DUST, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// WHITE TURBITY //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_TURBITY, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_TURBITY, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// ITO //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_ITO, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_ITO, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// EDGE BROKEN //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_EDGE_BROKEN, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_EDGE_BROKEN, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// PAD SCRATCH //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// DIMPLE POINT //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_POINT, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_POINT, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// ETCHING LINE //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_ETCHING_LINE, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_ETCHING_LINE, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// DIMPLE LINE //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_LINE, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_LINE, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// PAD BURNT //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// BURR //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_BURR, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_BURR, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// POL BUBBLE //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_BUBBLE, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_BUBBLE, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// POL SCRATCH //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_SCRATCH, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_SCRATCH, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// POL ALIEN SUB //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_ALIEN_SUB, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_ALIEN_SUB, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// POL MINOR //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_MINOR, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_POL_MINOR, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// BLACK SCRATCH //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_BLACK_SCRATCH, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_BLACK_SCRATCH, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// WHITE SCRATCH //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_SCRATCH, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_SCRATCH, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// CF MEASURE //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_CF_MEASURE, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_CF_MEASURE, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// TFT MEASURE //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_TFT_MEASURE, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_TFT_MEASURE, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// MURA //////////////////////////////////
		m_pGridCtrl->SetItemText(REPORT_ROW_MURA, REPORT_COUNT_COL, _T(""));
		m_pGridCtrl->SetItemText(REPORT_ROW_MURA, REPORT_RATIO_COL, _T(""));
		/////////////////////////////////////////////////////////////////////////////

		m_pGridCtrl->SetRedraw(TRUE);
		return;
	}

	m_pGridCtrl->SetRedraw(FALSE);

	///////////////////////////////////// total //////////////////////////////////
	//int nlength = 0;
	//TCHAR tszValue[MAX_PATH] = {0,};
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTotal);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOTAL, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%s"), _T("100.00"));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOTAL, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Good //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nGood);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_GOOD, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nGood/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_GOOD, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Defect //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nDefect);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_DEFECT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nDefect/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_DEFECT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Inspect Fail //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nInspectFail);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_INSPECT_FAIL, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nInspectFail/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_INSPECT_FAIL, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 상부이름 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTopName);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_NAME, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nTopName/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_NAME, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 상부찍힘 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTopStabbed);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_STABBED, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nTopStabbed/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_TOP_STABBED, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 하부기포 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nBottomBubble);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_BUBBLE, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nBottomBubble/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_BUBBLE, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 하부이상 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nBottomError);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_ERROR, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nBottomError/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_BOTTOM_ERROR, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 색얼룩 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nColorSpot);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_COLOR_SPOT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nColorSpot/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_COLOR_SPOT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 원형얼룩 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nCircleSpot);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_CIRCLE_SPOT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nCircleSpot/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_CIRCLE_SPOT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Monomer 뭉침 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nMonomerUnite);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_MONOMER_UNITE, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nMonomerUnite/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_MONOMER_UNITE, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Film 꺽임 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nFilmRollLine);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_FILM_ROLL_LINE, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nFilmRollLine/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_FILM_ROLL_LINE, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Wave //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nWave);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_WAVE, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nWave/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_WAVE, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 탈막 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nWashing);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_WASHING, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nWashing/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_WASHING, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Pad burnt //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadBurnt);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadBurnt/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Pad Scratch //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadScratch);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadScratch/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Pad 이상 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadError);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_ERROR, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadError/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_ERROR, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Pad 찍힘 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadStabbed);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_STABBED, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadStabbed/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_STABBED, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// 컷불량 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nCutError);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_CUT_ERROR, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nCutError/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_CUT_ERROR, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// Pad 뜯김 //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadPulledOut);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_PULLED_OUT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadPulledOut/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_PAD_PULLED_OUT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////// C-Cut //////////////////////////////////
	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nC_Cut);
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_C_CUT, REPORT_COUNT_COL, tszValue);

	//nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nC_Cut/m_psReportList->nTotal));
	//tszValue[nlength] = _T('\0');
	//m_pGridCtrl->SetItemText(REPORT_ROW_C_CUT, REPORT_RATIO_COL, tszValue);
	///////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// DUST //////////////////////////////////
	int nlength = 0;
	TCHAR tszValue[MAX_PATH] = {0,};
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTotal);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DUST, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%s"), _T("100.00"));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DUST, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// WHITE TURBITY //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nGood);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_TURBITY, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nGood/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_TURBITY, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// ITO //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nDefect);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_ITO, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nDefect/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_ITO, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// EDGE BROKEN //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nInspectFail);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_EDGE_BROKEN, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nInspectFail/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_EDGE_BROKEN, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////// PAD SCRATCH //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTopName);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nTopName/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_PAD_SCRATCH, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// DIMPLE POINT //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nTopStabbed);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_POINT, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nTopStabbed/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_POINT, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// ETCHING LINE //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nBottomBubble);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_ETCHING_LINE, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nBottomBubble/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_ETCHING_LINE, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////// DIMPLE LINE //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nBottomError);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_LINE, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nBottomError/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_DIMPLE_LINE, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// PAD BURNT //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nColorSpot);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nColorSpot/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_PAD_BURNT, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// BURR //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nCircleSpot);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_BURR, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nCircleSpot/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_BURR, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////// POL BUBBLE //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nMonomerUnite);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_BUBBLE, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nMonomerUnite/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_BUBBLE, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// POL SCRATCH //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nFilmRollLine);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_SCRATCH, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nFilmRollLine/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_SCRATCH, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////// POL ALIEN SUB //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nWave);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_ALIEN_SUB, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nWave/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_ALIEN_SUB, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// POL MINOR //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nWashing);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_MINOR, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nWashing/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_POL_MINOR, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// BLACK SCRATCH //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadBurnt);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_BLACK_SCRATCH, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadBurnt/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_BLACK_SCRATCH, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// WHITE SCRATCH //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadScratch);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_SCRATCH, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadScratch/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_WHITE_SCRATCH, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// CF MEASURE //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadError);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_CF_MEASURE, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadError/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_CF_MEASURE, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// TFT MEASURE //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nPadStabbed);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_TFT_MEASURE, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nPadStabbed/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_TFT_MEASURE, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// MURA //////////////////////////////////
	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%d"), m_psReportList->nCutError);
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_MURA, REPORT_COUNT_COL, tszValue);

	nlength = _stprintf_s(tszValue, MAX_PATH, _T("%.02lf"), (double)(100.*m_psReportList->nCutError/m_psReportList->nTotal));
	tszValue[nlength] = _T('\0');
	m_pGridCtrl->SetItemText(REPORT_ROW_MURA, REPORT_RATIO_COL, tszValue);
	/////////////////////////////////////////////////////////////////////////////

	m_pGridCtrl->SetRedraw(TRUE);
}

void CViewReport::FindPath(TCHAR *ptszPath, CStringArray &strElementAry)	// 해당 디렉토리나 파일을 검색한다.
{
	if(strElementAry.GetCount() == 0)	// 검색할 element가 존재하지 않을경우
	{
		return;
	}

	int nFileNameSize = 0;

	HANDLE hFindFile = NULL;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	BOOL bFileRead = FALSE;

	TCHAR tszPath[MAX_PATH]		= {0,};
	TCHAR tszElement[MAX_PATH]	= {0,};
	TCHAR tszFilePath[MAX_PATH] = {0,};
	_stprintf_s(tszElement, MAX_PATH, _T("%s")    , strElementAry.GetAt(0));
//	_stprintf_s(tszPath   , MAX_PATH, _T("%s\\%s"), ptszPath              , tszElement);
//	_stprintf_s(tszPath   , MAX_PATH, _T("%s"), ptszPath);
	_stprintf_s(tszFilePath,MAX_PATH, _T("%s\\*.*"), ptszPath);	// 반드시 검색 할 폴더 내에 *.*를 붙여야만 한다.
	strElementAry.RemoveAt(0);

//	hFindFile = ::FindFirstFile(tszPath, &wfd);
	hFindFile = ::FindFirstFile(tszFilePath, &wfd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	while(bResult)
	{
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)	// 디렉토리를 찾았다면
		{
			nFileNameSize = _tcslen(wfd.cFileName);
			if( !(nFileNameSize == 1 && _tcsncmp(wfd.cFileName, _T("."), nFileNameSize) == 0) 
				&& !(nFileNameSize == 2 && _tcsncmp(wfd.cFileName, _T(".."), nFileNameSize) == 0) )
			{	// '.'과 '..'은 제외
				nFileNameSize = nFileNameSize>(int)_tcslen(tszElement)?nFileNameSize:_tcslen(tszElement);
				if(_tcsncmp(wfd.cFileName, tszElement, nFileNameSize) == 0)	
				{
					_stprintf_s(tszPath, MAX_PATH, _T("%s\\%s"), ptszPath, tszElement);
					FindPath(tszPath, strElementAry);	// 하위디렉토리로 이동					
				}
			}
		}
		else if(wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)	// 파일을 찾았다면
		{
			nFileNameSize = nFileNameSize>(int)_tcslen(tszElement)?nFileNameSize:_tcslen(tszElement);
			if(_tcsncmp(wfd.cFileName, tszElement, nFileNameSize) == 0)
			{
				_stprintf_s(tszPath, MAX_PATH, _T("%s\\%s"), ptszPath, tszElement);
				bFileRead = ReadFileData(tszPath);
				break;
			}
		}
		bResult = ::FindNextFile(hFindFile, &wfd);
	}

	if(hFindFile)
	{
		::FindClose(hFindFile);
		hFindFile = NULL;
	}

}

BOOL CViewReport::ReadFileData(TCHAR *ptszPath)
{
	FILE *pFile;
	pFile = NULL;
	_tfopen_s(&pFile, ptszPath, _T("rb"));
	if(pFile == NULL)
	{
		return FALSE;
	}
	
	char szValue[MAX_PATH] = {0,};	
	char *pszTemp = NULL;	
	//////////////////////////////////////////// [RESULT COUNT] ///////////////////////////////////////////////////////
	char *pszPos = fgets(szValue, sizeof(szValue), pFile);	
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	int retval = strncmp(pszTemp, "[RESULT COUNT]", strlen("[RESULT COUNT]"));
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/////////////////////////////////////////////////////// TOTAL ////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "TOTAL", strlen("TOTAL"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	char *pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	int nValue = atoi(pchPos);	
	m_psReportList->nTotal += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////// GOOD //////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "GOOD", strlen("GOOD"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nGood += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// DEFECT ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "DEFECT", strlen("DEFECT"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nDefect += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// INSTPECT FAIL ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "INSPECT FAIL", strlen("INSPECT FAIL"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);
	m_psReportList->nInspectFail += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 상부이름 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "상부이름", strlen("상부이름"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nTopName += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 상부찍힘 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "상부찍힘", strlen("상부찍힘"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nTopStabbed += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 하부기포 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "하부기포", strlen("하부기포"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);
	m_psReportList->nBottomBubble += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 하부이상 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "하부이상", strlen("하부이상"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nBottomError += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 색얼룩 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "색얼룩", strlen("색얼룩"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nColorSpot += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 원형얼룩 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "원형얼룩", strlen("원형얼룩"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nCircleSpot += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// MONOMER 뭉침 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "MONOMER 뭉침", strlen("MONOMER 뭉침"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nMonomerUnite += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// FILM 꺽임 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "FILM 꺽임", strlen("FILM 꺽임"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nFilmRollLine += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// WAVE ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "WAVE", strlen("WAVE"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nWave += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 탈막 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "탈막", strlen("탈막"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nWashing += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// PAD BRUNT ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "PAD BRUNT", strlen("PAD BRUNT"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nPadBurnt += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// PAD SCRATCH ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "PAD SCRATCH", strlen("PAD SCRATCH"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);
	m_psReportList->nPadScratch += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// PAD 이상 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "PAD 이상", strlen("PAD 이상"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nPadError += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// PAD 찍힘 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "PAD 찍힘", strlen("PAD 찍힘"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nPadStabbed += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// 컷불량 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "컷불량", strlen("컷불량"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nCutError += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// PAD 뜯김 ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "PAD 뜯김", strlen("PAD 뜯김"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nPadPulledOut += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////// C-CUT ////////////////////////////////////////////////////////
	pszPos = fgets(szValue, sizeof(szValue), pFile);
	if(pszPos == NULL)
	{
		return FALSE;
	}

	pszTemp = trimAll(pszPos);
	retval = strncmp(pszTemp, "C-CUT", strlen("C-CUT"));	// 앞단어만 맞으면 됨
	if(retval < 0 || retval > 0)
	{
		return FALSE;
	}
	
	pchPos = strrchr(pszTemp, ' ');	// 뒤쪽에서 공백을 기준으로 값을 추출한다.
	if(pchPos == NULL)
	{
		return FALSE;
	}

	nValue = atoi(pchPos);	
	m_psReportList->nC_Cut += nValue;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}

	return TRUE;
}

char *CViewReport::trimAll(char *s)
{
	char *f = s;
	char *e = '\0';
	char *c = s;

	// 뒤쪽 공백 제거
	e = s + (strlen(s)) - 1;
	while(*e == ' ' && s <= e)
	{
		e--;
	}
	*(e+1) = '\0';

	// 앞쪽 공백 제거
	while(*f == ' ' && f <= e)
	{
		f++;
	}

	// 공백 없는 부분 복사
	if(s != f)
	{
		while(f <= e)
		{
			*(c++) = *(f++);
		}
		*c = '\0';
	}

	return s;
}

void CViewReport::OnSelchangeComboBar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	COLORREF colortemp;	
//	switch(m_pcomboSelectBar->GetCurSel())
	switch(m_comboSelectBar.GetCurSel())
	{
	case COMBO_BAR_TOTAL:
		colortemp = m_Graph.GetBarTotalColor();
		break;											
	case COMBO_BAR_GOOD:
		colortemp = m_Graph.GetBarGoodColor();				
		break;											
	case COMBO_BAR_DEFECT:
		colortemp = m_Graph.GetBarDefectColor();		
		break;												
	case COMBO_BAR_INSPECT_FAIL:
		colortemp = m_Graph.GetBarInspectFailColor();	
		break;											
	case COMBO_BAR_TOP_NAME:
		colortemp = m_Graph.GetBarTopNameColor();		
		break;											
	case COMBO_BAR_TOP_STABBED:	
		colortemp = m_Graph.GetBarTopStabbedColor();		
		break;											
	case COMBO_BAR_BOTTOM_BUBBLE:	
		colortemp = m_Graph.GetBarBottomBubbleColor();	
		break;											
	case COMBO_BAR_BOTTOM_ERROR:	
		colortemp = m_Graph.GetBarBottomErrorColor();	
		break;											
	case COMBO_BAR_COLOR_SPOT:	
		colortemp = m_Graph.GetBarColorSpotColor();			
		break;											
	case COMBO_BAR_CIRCLE_SPOT:		
		colortemp = m_Graph.GetBarCircleSpotColor();		
		break;											
	case COMBO_BAR_MONOMER_UNITE:	
		colortemp = m_Graph.GetBarMonomerUniteColor();	
		break;											
	case COMBO_BAR_FILM_ROLL_LINE:	
		colortemp = m_Graph.GetBarFilmRollLineColor();	
		break;											
	case COMBO_BAR_WAVE:		
		colortemp = m_Graph.GetBarWaveColor();			
		break;											
	case COMBO_BAR_WASHING:		
		colortemp = m_Graph.GetBarWashingColor();		
		break;											
	case COMBO_BAR_PAD_BURNT:	
		colortemp = m_Graph.GetBarPadBurntColor();		
		break;											
	case COMBO_BAR_PAD_SCRATCH:		
		colortemp = m_Graph.GetBarPadScratchColor();	
		break;											
	case COMBO_BAR_PAD_ERROR:	
		colortemp = m_Graph.GetBarPadErrorColor();		
		break;											
	case COMBO_BAR_PAD_STABBED:	
		colortemp = m_Graph.GetBarPadStabbedColor();	
		break;											
	case COMBO_BAR_CUT_ERROR:	
		colortemp = m_Graph.GetBarCutErrorColor();		
		break;											
	case COMBO_BAR_PAD_PULLED_OUT:	
		colortemp = m_Graph.GetBarPadPulledOutColor();	
		break;													
	case COMBO_BAR_C_CUT:	
		colortemp = m_Graph.GetBarC_CutColor();	
		break;
	}

	m_pstaticGraphBarListColor->SetBkColor(colortemp);
	m_pstaticGraphBarListColor->Refresh();
}


void CViewReport::ClickGxbuttonexctrlSelectColorBk()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	COLORREF color;

	CColorDialog *pDlg;
	pDlg = NULL;
	pDlg = new CColorDialog(RGB(255, 255, 255));
	if(pDlg->DoModal() == IDOK)
	{
		color = pDlg->GetColor();
		m_pstaticGraphBkColor->SetBkColor(color);
		m_pstaticGraphBkColor->Refresh();
		m_Graph.SetBkColor(color);
		m_Graph.Refresh();
	}

	if(pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}
}


void CViewReport::ClickGxbuttonexctrlSelectColorAxis()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	COLORREF color;

	CColorDialog *pDlg;
	pDlg = NULL;
	pDlg = new CColorDialog(RGB(255, 255, 255));
	if(pDlg->DoModal() == IDOK)
	{
		color = pDlg->GetColor();
		m_pstaticGraphAxisColor->SetBkColor(color);
		m_pstaticGraphAxisColor->Refresh();
		m_Graph.SetAxisColor(color);
		m_Graph.Refresh();
	}

	if(pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}
}


void CViewReport::ClickGxbuttonexctrlSelectColorRatioText()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	COLORREF color;

	CColorDialog *pDlg;
	pDlg = NULL;
	pDlg = new CColorDialog(RGB(255, 255, 255));
	if(pDlg->DoModal() == IDOK)
	{
		color = pDlg->GetColor();
		m_pstaticGraphRatioTextColor->SetBkColor(color);
		m_pstaticGraphRatioTextColor->Refresh();
		m_Graph.SetRatioTextColor(color);
		m_Graph.Refresh();
	}

	if(pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}
}


void CViewReport::ClickGxbuttonexctrlSelectColorBarText()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	COLORREF color;

	CColorDialog *pDlg;
	pDlg = NULL;
	pDlg = new CColorDialog(RGB(255, 255, 255));
	if(pDlg->DoModal() == IDOK)
	{
		color = pDlg->GetColor();
		m_pstaticGraphBarTextColor->SetBkColor(color);
		m_pstaticGraphBarTextColor->Refresh();
		m_Graph.SetBarTextColor(color);
		m_Graph.Refresh();
	}

	if(pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}
}


void CViewReport::ClickGxbuttonexctrlSelectColorBar()
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	COLORREF color;

	CColorDialog *pDlg;
	pDlg = NULL;
	pDlg = new CColorDialog(RGB(255, 255, 255));
	if(pDlg->DoModal() == IDOK)
	{
		color = pDlg->GetColor();
	//	switch(m_pcomboSelectBar->GetCurSel())
		switch(m_comboSelectBar.GetCurSel())
		{
		case COMBO_BAR_TOTAL:
			m_Graph.SetBarTotalColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_GOOD:
			m_Graph.SetBarGoodColor(color);		
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_DEFECT:
			m_Graph.SetBarDefectColor(color);	
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;												
		case COMBO_BAR_INSPECT_FAIL:
			m_Graph.SetBarInspectFailColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_TOP_NAME:
			m_Graph.SetBarTopNameColor(color);	
			m_Graph.Refresh();	m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_TOP_STABBED:	
			m_Graph.SetBarTopStabbedColor(color);
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_BOTTOM_BUBBLE:	
			m_Graph.SetBarBottomBubbleColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_BOTTOM_ERROR:	
			m_Graph.SetBarBottomErrorColor(color);
			m_Graph.Refresh();m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_COLOR_SPOT:	
			m_Graph.SetBarColorSpotColor(color);
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();	
			break;											
		case COMBO_BAR_CIRCLE_SPOT:		
			m_Graph.SetBarCircleSpotColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_MONOMER_UNITE:	
			m_Graph.SetBarMonomerUniteColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_FILM_ROLL_LINE:	
			m_Graph.SetBarFilmRollLineColor(color);
			m_Graph.Refresh();m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_WAVE:		
			m_Graph.SetBarWaveColor(color);	
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_WASHING:		
			m_Graph.SetBarWashingColor(color);	
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_PAD_BURNT:	
			m_Graph.SetBarPadBurntColor(color);	
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_PAD_SCRATCH:		
			m_Graph.SetBarPadScratchColor(color);
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_PAD_ERROR:	
			m_Graph.SetBarPadErrorColor(color);	
			m_Graph.Refresh();	
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_PAD_STABBED:	
			m_Graph.SetBarPadStabbedColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_CUT_ERROR:	
			m_Graph.SetBarCutErrorColor(color);		
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;											
		case COMBO_BAR_PAD_PULLED_OUT:	
			m_Graph.SetBarPadPulledOutColor(color);
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;													
		case COMBO_BAR_C_CUT:	
			m_Graph.SetBarC_CutColor(color);	
			m_Graph.Refresh();
			m_pstaticGraphBarListColor->SetBkColor(color);
			m_pstaticGraphBarListColor->Refresh();
			break;
		}
	}

	if(pDlg)
	{
		delete pDlg;
		pDlg = NULL;
	}
}

void CViewReport::OnBnClickedSave()
{

	TCHAR tszValue[3] = {0,};
	CString cCountVal;
	CString strTemp;
	CString cCountRoot;
	BOOL bCmpChar = TRUE, bCmpOK = FALSE;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);	
	

	for (int i = 0 ; i< REPORT_LIST_MAX ; i++)
	{		
		::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i, tszValue, 3);	
		cCountVal = tszValue;
		char tCompareAscii;

		if (!cCountVal.IsEmpty() && i == 1 || i == 2)
		{
			::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1, tszValue, 3);	
			strTemp = tszValue;
			if (strTemp.IsEmpty())
			{
				AfxMessageBox(_T("please, insert First Time"));
				m_peditStatsTime[i]=(CEdit *)GetDlgItem(IDC_EDIT_RESET_TIME1); 
				int len = m_peditStatsTime[i]->GetWindowTextLength(); 
				m_peditStatsTime[i]->SetSel(len, len); 			
				m_peditStatsTime[i]->SetFocus(); 
				return;
			}
		}
		if (!cCountVal.IsEmpty() && i == 2)
		{
			::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME2, tszValue, 3);	
			strTemp = tszValue;
			if (strTemp.IsEmpty())
			{
				AfxMessageBox(_T("please, insert Second Time"));
				m_peditStatsTime[i]=(CEdit *)GetDlgItem(IDC_EDIT_RESET_TIME2); 
				int len = m_peditStatsTime[i]->GetWindowTextLength(); 
				m_peditStatsTime[i]->SetSel(len, len); 			
				m_peditStatsTime[i]->SetFocus(); 
				return;
			}
		}

		for (int j = 0 ; j < cCountVal.GetAllocLength() ; j++)
		{
			tCompareAscii = (char)cCountVal.GetAt(j);
			int nCmpAscii = tCompareAscii;

			if (cCountVal.GetAt(j) != NULL && (nCmpAscii < 47 || nCmpAscii > 57))
			{
				bCmpChar = FALSE; 
			}
			cCountVal.ReleaseBuffer();
		}

		if (_ttoi(cCountVal) < 0 || _ttoi(cCountVal) >24 || bCmpChar == FALSE)
		{
			AfxMessageBox(_T("please, input 0 ~ 24 hour"));
			::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i, _T(""));
			m_peditStatsTime[i]=(CEdit *)GetDlgItem(IDC_EDIT_RESET_TIME1 + i); 
			int len = m_peditStatsTime[i]->GetWindowTextLength(); 
			m_peditStatsTime[i]->SetSel(len, len); 			
			m_peditStatsTime[i]->SetFocus(); 
			return;
		}
		bCmpOK = TRUE;						
	}	
	if(m_pDlgLogin)
	{
		delete m_pDlgLogin;
		m_pDlgLogin = NULL;
	}	
	m_pDlgLogin = new CLogin();			
	if (bCmpOK)
	{
		if (m_pDlgLogin->DoModal() == IDOK)
		{
			for (int i = 0 ; i< REPORT_LIST_MAX ; i++)
			{
				::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i, tszValue, 3);	
				cCountVal = tszValue;
				if (cCountVal.IsEmpty()) { cCountVal = _T("");	::SetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i, _T(""));	}
				strTemp.Format(_T("STATS_TIME%d"), i + 1);
				WritePrivateProfileString(EQP_STATE, strTemp , cCountVal, cCountRoot);				
			}
		}
	}

	m_fnStatsListUpdate();
}

LRESULT CViewReport::OnMainMenuChange(WPARAM wParam, LPARAM lParam)
{
	

	return 0;
}

int CViewReport::m_fnStatsSearchData(STRU_REPORT_INFO stReportInfo)
{
	int nRet = 0 , nDataCnt = 0;
	//CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strFileName;
	CString strRootPath = _T("");
	CString strStartPath, strEndPath;
	BOOL bWorking, bFirRead = TRUE;		
	CFileFind finder;	
	
	strRootPath = stReportInfo.tDataPath;

	strStartPath.Format(_T("paneldata_%04d%02d%02d"), 
						stReportInfo.stDataTime.nYear, stReportInfo.stDataTime.nMonth, stReportInfo.stDataTime.nDay);
	

	nDataCnt = stReportInfo.stDataTime.nDay;
	try
	{
		bWorking = finder.FindFile(strRootPath + _T("*.*"));	// 모든 파일
		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

			if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

			strFileName = finder.GetFileName();	// 파일 이름 얻기			

			if ( strFileName.Find(strStartPath) != -1 && strFileName.Find(_T(".csv")) != -1)				
			{
				//ReadFileData((TCHAR *)(LPCTSTR)(strRootPath + strFileName));
				m_fnReadFileData((TCHAR *)(LPCTSTR)(strRootPath + strFileName), stReportInfo,  m_bFirRead,  m_bEndRead);
				m_bFirRead = FALSE;
				break;
			}
			
			if (bWorking == FALSE) break;
		}
		finder.Close();
	}

	catch (CFileException* e)
	{		
		G_AddLog(2, 3, _T("m_fnStatsSearchData Failed !! - CFileException "));
		e->Delete();
	}
	catch (CException* e)
	{		
		G_AddLog(2, 3, _T("m_fnStatsSearchData Failed !! - CException "));
		e->Delete();
	}

	return nRet;
}

int CViewReport::m_fnReadFileData(TCHAR* ptszPath, STRU_REPORT_INFO stReportInfo, BOOL bFirRead, BOOL bEndRead)
{
 	int nRetVal = 0, nRowCnt = 0, nlength = 0, nColCnt = 0, nFirFlag = 0;
 	CStdioFile cstFile;
 	CString strLine, strItem, strNewLine ,strTemp, strCmpTime;
 	CString strLineHeader,strDate, strTime;
 	CString strtemp, resToken;	
 	CString check_date= _T("");	
 	//CStringArray *strArrayData; 	
 	CStringArray *stTmpPnlData;	
 	 int nAddData = 1;
 	if (!cstFile.Open(ptszPath, CFile::modeRead | CFile::typeText, NULL))
 	{
 		G_AddLog(3, 2, _T("m_fnReadFileData File Open Error %s : %d"), ptszPath , GetLastError());
 		return FALSE;
 	}
 
 	if(m_listReportPnlData.size())		
 		m_listReportPnlData.back();	
 
 	while(cstFile.ReadString(strLine))
 	{
 		if(strLine == "")	continue; 
 
 		stTmpPnlData = new CStringArray;
 		//parallel_for(0 ,MAX_PANEL_DATA , [&](int i){	
 		for (int i = 0 ; i < MAX_PANEL_DATA ; i++)
 		{
 			// 추가 .. 시간대 검색하여 할당할 필요 있음
 			if ((!bFirRead && nFirFlag != 0) || (bFirRead)  )
 			{
 				AfxExtractSubString(strTemp, strLine, i, ','); 	
 				if (!bEndRead && bFirRead && nFirFlag != 0)
 				{
 					strNewLine = strLine;
 					AfxExtractSubString(strCmpTime, strNewLine, ENUM_INSPECT_TIME, ','); 						
 					AfxExtractSubString(strNewLine, strCmpTime, 1, ' '); 						
 					AfxExtractSubString(strNewLine, strNewLine, 0, ':'); 	
 				
 					if (_ttoi(strNewLine) < stReportInfo.stDataTime.nHour)
 					{
 						nAddData = FALSE;
 					   break;
 					}
 				}
 				if (bEndRead && nFirFlag != 0)
 				{
 					strNewLine = strLine;
 					AfxExtractSubString(strCmpTime, strNewLine, ENUM_INSPECT_TIME, ','); 	
 					AfxExtractSubString(strNewLine, strCmpTime, 1, ' '); 						
 					AfxExtractSubString(strNewLine, strNewLine, 0, ':'); 	
 
 					if (_ttoi(strNewLine) >= stReportInfo.stDataTime.nHour)
 					{
 						nAddData = FALSE;
 						break;
 					}
 				}
 				strTemp.Remove(_T('\"')); strTemp.Trim();			
 				//stTmpPnlData->tPnlData[i] = new TCHAR[MAX_PANEL_DATA_LEN];	
 				stTmpPnlData->Add(strTemp);
 				//memset(stTmpPnlData->tPnlData[i] , 0, sizeof(char) * strTemp.GetLength());				
 				//nlength = _stprintf_s(&stTmpPnlData->tPnlData[i][0], MAX_PANEL_DATA_LEN, _T("%s"), strTemp);
 				//stTmpPnlData->tPnlData[i][nlength] = _T('\0');		
 				nAddData = TRUE;
 			}
 
 			else
 				nAddData = FALSE;
 									
 		}
		//});
 		nFirFlag++;
 		if (nAddData)
 		{
 			m_listReportPnlData.push_back(stTmpPnlData); 					
 		}	
		else
			delete stTmpPnlData;
 	}	
 	cstFile.Close();

	return TRUE;

}

int CViewReport::m_fnSortStatsData()
{
	int nResetCnt[REPORT_LIST_MAX] = {0, };
	int nTemp = 0;
	TCHAR tEndTime[3] = {0,};
	TCHAR tStartTime[3] = {0,};
	int ncmpTime = 0;
	int nStrTime = 0;
	int nEndTime = 0;
	int nEmptyCnt = 0;
	////////////////////////////// TOTAL /////////////////////////////////////////////
	LONG	lTotalCnt = 0;
	CString strTemp = _T("0"), strCmpTime = _T("0"), strChkTime;
	CStringArray strAryPnlItem, strAryPnlData, strAryPnlCode;
	int nDefItemCnt = 0, nDefCnt = 0;// = listList->size();	
	
	BOOL bDefItemFlg = FALSE;
	//stRptPnlData->SetSize(MAX_PANEL_DATA);
	//stPnalItem->SetSize(MAX_PANEL_DATA);
	////////////////////////////// TOTAL /////////////////////////////////////////////

	//CRITICAL_SECTION cs;
	//::InitializeCriticalSection(&cs);
	
	for (int i = 0 ; i < REPORT_LIST_MAX ; i ++)
	{
		::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i, tStartTime  , 3);	
		strChkTime = tStartTime;
		if (!strChkTime.IsEmpty())
		{
			nEmptyCnt++;
			
		}

	}

// 	if (m_listReportPnlData.size() > 0)
// 	{
		//parallel_for(0,  nEmptyCnt , [&](int i)
		for (int i = 0; i< nEmptyCnt; i++)
		{
			if(m_listReportp[i]->size())		
				m_listReportp[i]->back();	
			//iList = m_listReportPnlData.begin();
			int n = m_listReportPnlData.size();
			int nRowCnt = 0;
			if (n)
			{
				std::vector<CStringArray*>::iterator iList;
				//parallel_for(m_listReportPnlData.begin() , m_listReportPnlData.end() , iList, m_DataSet//{	
				for (iList = m_listReportPnlData.begin() ; iList != m_listReportPnlData.end() ; iList++)
				{			
 					if ( nRowCnt != 0)
 					{
 						strTemp = (*iList)->GetAt(ENUM_INSPECT_TIME);
 						AfxExtractSubString(strTemp, strTemp, 1, ' '); 	
 						//strCmpTime = strCmpTime.Right(5);
 						AfxExtractSubString(strTemp, strTemp, 0, ':'); 	
 						//strTemp = strTemp.Right(5);
 						//AfxExtractSubString(strCmpTime, strTemp, 0, ':'); 	
 						::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + i		 , tEndTime  , 3);	
 						::GetDlgItemText(GetSafeHwnd(), IDC_EDIT_RESET_TIME1 + abs((i * (nEmptyCnt -1) > (nEmptyCnt) ? (nEmptyCnt) : (i * (nEmptyCnt - 1))) - (nEmptyCnt - 1)), tStartTime, 3);	
 					
 						strChkTime = tEndTime;
 						if (!strChkTime.IsEmpty())
 						{
 							ncmpTime = _ttoi(strTemp);nEndTime = _ttoi(tEndTime);nStrTime = _ttoi(tStartTime);
 							if (nStrTime >= nEndTime)
 							{
 								nEndTime += 24;						
 								if (nStrTime > ncmpTime)
 									ncmpTime += 24;
 							}
 							if (ncmpTime >= nStrTime && ncmpTime < nEndTime ) 							
 								m_listReportp[i]->push_back((*iList));
 						}					
 					}
 					if ( nRowCnt == 0)				
 						m_listReportp[i]->push_back((*iList));// =  m_listReportPnlData.at(nRowCnt);
 
 					//iList++;
					nRowCnt++;
				}
			}
			int a = 0; 
			a = m_listReportp[i]->size();
			int b = 0;
			b = m_listReportPnlData.size();
		}
		//);
		size_t t = nEmptyCnt;
		//parallel_for<size_t>(0,  nEmptyCnt , [&](size_t i){
		for (int i = 0; i< nEmptyCnt; i++){
		CString strCaption;
		::PostMessage(m_DlgRptDefData[i]->GetSafeHwnd(), WM_UI_CONTROL_DIS_STATS_LIST, (WPARAM)0, (LPARAM)(vector<CStringArray>*)m_listReportp[i]);
		if (i == 0)		strCaption.Format(_T("FIRST DATA"));
		if (i == 1)		strCaption.Format(_T("SECOND DATA"));
		if (i == 2)		strCaption.Format(_T("THIRD DATA"));
		::SendMessage(m_DlgRptDefData[i]->GetSafeHwnd(), WM_UI_CONTROL_DIS_STATS_NAME, (WPARAM)0, (LPARAM)(LPCTSTR)strCaption);				
		}
		//);
		strTemp.Format(_T("TOTAL DATA"));		
		::SendMessage(m_DlgRptDefData[3]->GetSafeHwnd(), WM_UI_CONTROL_DIS_STATS_NAME, (WPARAM)0, (LPARAM)(LPCTSTR)strTemp);
		::PostMessage(m_DlgRptDefData[3]->GetSafeHwnd(), WM_UI_CONTROL_DIS_STATS_LIST, (WPARAM)0, (LPARAM)(vector<CStringArray>*)&m_listReportPnlData);
	//}
		
		if (m_hThreadHandle == NULL)
		{
			UINT uiThreadID;
			m_hThreadHandle = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadDelListData, (LPVOID)this, 0, &uiThreadID);
		}
		
	return TRUE;
}


unsigned __stdcall  CViewReport::m_fnThreadDelListData(LPVOID pArg )
{	
	CViewReport *pFrame = (CViewReport *)pArg;		

	Sleep(2000);
	for (int i = 0 ; i< REPORT_LIST_MAX + 1 ; i++)
	{
		if(pFrame->m_DlgRptDefData[i] != NULL) pFrame->H_EVENT[i] =  pFrame->m_DlgRptDefData[0]->m_H_DefData;
	}
	
	//if (WAIT_OBJECT_0 != WaitForMultipleObjects(4, H_EVENT, TRUE,INFINITE))//(WAIT_OBJECT_0 != WaitForSingleObject(m_DlgRptDefData[3]->m_H_DefData, INFINITE))//::WaitForMultipleObjects(4, H_EVENT, TRUE, INFINITE))
	{
		WaitForMultipleObjects(REPORT_LIST_MAX + 1, pFrame->H_EVENT, TRUE,INFINITE);
		

	}

	BOOL rc = FALSE;
	
		//vector<CStringArray*>::iterator ilist;
		//ilist = pFrame->m_listReportPnlData.begin();
		CStringArray* ilist;

		//CStringArray* pDelDefectFile;
		if(pFrame->m_listReportPnlData.size() > 0)
		{
		while (pFrame->m_listReportPnlData.size() > 0)//for(ilist = listReportListInfo.begin();ilist != listReportListInfo.end(); ilist++)
		{
			pFrame->m_listReportPnlData.back();	
	
			ilist = pFrame->m_listReportPnlData.at(pFrame->m_listReportPnlData.size() -1);
			//pFrame->m_listReportPnlData.erase(--ilist);
		//	pDelDefectFile = *--ilist;// (*ilist);//listReportListInfo.at( listReportListInfo.size() -1);		
		//	delete pDelDefectFile;
			delete ilist;	
			//
			pFrame->m_listReportPnlData.pop_back();
		}	
		}

//  	parallel_for_each(pFrame->m_listReportPnlData.begin(),  pFrame->m_listReportPnlData.end() , [&](CStringArray* ilist)	
//  	{	
//  		delete ilist; 	 					
//  	});	
		
	CloseHandle(pFrame->m_hThreadHandle);
	pFrame->m_hThreadHandle = NULL;

	return rc;
}

int CViewReport::m_fnPanelDataClear(vector<CStringArray*> &listReportListInfo)
{

//	vector<CStringArray*>::iterator ilist;
//	ilist = listReportListInfo.begin();
//	CStringArray* pDelDefectFile;
//	if(listReportListInfo.size() > 0)
//	{
//	while (listReportListInfo.size() > 0)//for(ilist = listReportListInfo.begin();ilist != listReportListInfo.end(); ilist++)
//	{
//		listReportListInfo.back();	
//
//		ilist = listReportListInfo.end();
//		pDelDefectFile = *--ilist;// (*ilist);//listReportListInfo.at( listReportListInfo.size() -1);		
//		delete pDelDefectFile;
////		delete ilist;	
//		//
//		listReportListInfo.pop_back();
//	}	
//	}
////	

	
 	parallel_for_each(listReportListInfo.begin(),  listReportListInfo.end() , [&](CStringArray* ilist)	
 	{
 		//listReportListInfo.back();	
 // 		if (ilist->GetSize() > 0)
 // 		{
 			//vector<CStringArray*>::iterator iList  = ilist->;
 			//if(listReportListInfo.size() < 0) {}
 			//ilist->RemoveAll();
  			if(ilist)
  			{			
 				//ilist->RemoveAll();
 				
 				delete ilist;
 				//listReportListInfo.erase(ilist);
 				//ilist->
 			}		
 			ilist->RemoveAll();
 //		}	
 		//listReportListInfo.pop_back();
 	});	
	
	return TRUE;
}

