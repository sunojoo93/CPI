

//#include "..\EXTEND\CommonCtrl\GridCtrl.h"
#include ".\Grid\GridCtrl.h"
#include "DlgReportDefData.h"
#include "LoadingDlg.h"
#include ".\Graph\GraphCtrl.h"
#include ".\ColorStatic\SelectColorStatic.h"
#include "Login.h"
#include <list>	
// CViewReport �� ���Դϴ�.

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
	stReportList		*m_psReportList;	// �ڷḦ ������Ű�� ���� �ʿ�.
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
	CViewReport();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

	void m_fnStatsListUpdate(BOOL bRetryFlg = TRUE);
	void ReportFileLoad();	// �����忡�� ���� �Լ�
	void FindReportData(COleDateTime &StartDateTime);	// �ش� ��¥�� �����͸� ã�� �Լ�
	void UpdateGridCtrl();	// �׸����� �Ϲݼ��� �����ϴ� �Լ�
	void FindPath(TCHAR *ptszPath, CStringArray &strElementAry);					// �ش� ���丮 �� ������ �˻��Ѵ�.
	BOOL ReadFileData(TCHAR *ptszPath);		// ���Ͽ��� ������ �о���δ�.
	char *trimAll(char *s);	// ���ڿ��� �¿��� ������ ���� �ϴ� �Լ�

	enum { IDD = IDD_VIEW_REPORT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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