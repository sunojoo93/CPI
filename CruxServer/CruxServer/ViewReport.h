

//#include "..\EXTEND\CommonCtrl\GridCtrl.h"
#include ".\Grid\GridCtrl.h"
#include "DlgReportDefData.h"
#include "LoadingDlg.h"
#include ".\Graph\GraphCtrl.h"
#include ".\ColorStatic\SelectColorStatic.h"
#include "Login.h"
#include <list>	
// CViewReport 폼 뷰입니다.

struct function_object
{
	function_object( std::vector<CStringArray*>& lList_ ) : lList(lList_) {}

	std::vector<CStringArray*>& lList;

	void operator() (long i) const
	{
		//lList.push_back[i] = i;		
	}
};

#define REPORT_LIST_MAX  3
class CViewReport : public CFormView
{
	DECLARE_DYNCREATE(CViewReport)
private:
	CEdit               *m_peditStartTime;
	CSpinButtonCtrl		*m_pspinStartTime;
	CEdit				*m_peditEndTime;
	CSpinButtonCtrl		*m_pspinEndTime;
	CEdit               *m_peditStatsTime[3];	
	CGridCtrl			*m_pGridCtrl;
	HANDLE				 m_hThreadFileLoad;
	CLoadingDlg			*m_pLoadingDlg;
	stReportList		*m_psReportList;	// 자료를 누적시키기 위해 필요.
	CGraphCtrl			 m_Graph;
	BOOL				 m_bFirRead;
	BOOL				 m_bEndRead;
	CSelectColorStatic  *m_pstaticGraphBkColor;
	CSelectColorStatic  *m_pstaticGraphAxisColor;
	CSelectColorStatic  *m_pstaticGraphRatioTextColor;
	CSelectColorStatic  *m_pstaticGraphBarTextColor;
	CSelectColorStatic  *m_pstaticGraphBarListColor;

	COleDateTime		m_startDateTime;
	COleDateTime		m_EndDateTime;
	CLogin*				m_pDlgLogin;
	HANDLE				m_hThreadHandle;
	
public:
	HANDLE H_EVENT[4];			
	STRU_REPORT_INFO m_stReportInfo;
	//CStringArray m_strArrayData;
	std::vector<CStringArray*> m_listReportPnlData;	
	std::vector<CStringArray*> *m_listReportp[3];	
		
protected:
	virtual ~CViewReport();

public:
	CViewReport();           // 동적 만들기에 사용되는 protected 생성자입니다.

	void m_fnStatsListUpdate(BOOL bRetryFlg = TRUE);
	void ReportFileLoad();	// 스레드에서 돌릴 함수
	void FindReportData(COleDateTime &StartDateTime);	// 해당 날짜로 데이터를 찾는 함수
	void UpdateGridCtrl();	// 그리드의 일반셀을 갱신하는 함수
	void FindPath(TCHAR *ptszPath, CStringArray &strElementAry);					// 해당 디렉토리 및 파일을 검색한다.
	BOOL ReadFileData(TCHAR *ptszPath);		// 파일에서 내용을 읽어들인다.
	char *trimAll(char *s);	// 문자열의 좌우측 공백을 제거 하는 함수

	enum { IDD = IDD_VIEW_REPORT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	static unsigned __stdcall ThreadFileLoad(void *pArg);

	DECLARE_MESSAGE_MAP()
public:

	CDlgReportDefData* m_DlgRptDefData[4];
	static unsigned __stdcall  m_fnThreadDelListData(LPVOID pArg );
	virtual void OnInitialUpdate();
	afx_msg void OnDeltaposSpinStartTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinEndTime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChangeEditStartTime();
	afx_msg void OnChangeEditEndTime();
	afx_msg LRESULT OnMainMenuChange(WPARAM wParam, LPARAM lParam);	
	DECLARE_EVENTSINK_MAP()
	void ClickGxbuttonexctrlSearch();
	afx_msg void OnSelchangeComboBar();
	CComboBox m_comboSelectBar;
	void ClickGxbuttonexctrlSelectColorBk();
	void ClickGxbuttonexctrlSelectColorAxis();
	void ClickGxbuttonexctrlSelectColorRatioText();
	void ClickGxbuttonexctrlSelectColorBarText();
	void ClickGxbuttonexctrlSelectColorBar();
	CDateTimeCtrl m_DateTimeStart;
	CDateTimeCtrl m_DateTimeEnd;
	afx_msg void OnBnClickedSave();
	int m_fnStatsSearchData(STRU_REPORT_INFO stReportInfo);
	int m_fnReadFileData(TCHAR* ptszPath, STRU_REPORT_INFO stReportInfo, BOOL bFirRead, BOOL bEndRead);
	int m_fnPanelDataClear(vector<CStringArray*> &listReportListInfo);
	int m_fnSortStatsData();
	void m_DataSet(CStringArray* listReportListInfo);


};