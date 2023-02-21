#pragma once

#include ".\Grid\GridCtrl.h"
#include "afxwin.h"
#include "DlgDefectMap.h"
#include "DlgDefectInfo.h"
#include "DlgVisionState.h"
#include "DlgStageState.h"
#include "DlgLuminanceStatus.h"
#include "LoadingDlg.h"	
#include "DlgPanelJudge.h"
#include "afxcmn.h"
#include "DlgPopUpImage.h"
#include "DlgMessage.h"
#include "DlgTemperature.h"	//2016.04.04
#include "DlgPanelCodeTest.h"
#include "GRADE_SET_Dialog.h"
#include "F_GRDAE_Dialog.h"
#include "DlgManualJudge.h"
#include "DlgMessage2.h"
// CViewAuto 폼 뷰입니다.
//20150703 Chu
//#define  SAFE_DELETE(p) { if(p) { delete (p); (p) = NULL; } }

class CViewAuto : public CFormView
{
	DECLARE_DYNCREATE(CViewAuto)

protected:
	virtual ~CViewAuto();

private:

	CString   m_testQmsImgFile(CString strImgFileName);
	BOOL m_testImgFile(CString strPanelId);

	int m_ngridDetectFileRow;
	int m_ngridDetectFileCol;
	int m_IdxPanelBtn;
	CString m_strPanelPath[4];
	CLoadingDlg	   *m_pLoadingDlg;				
public:
	CViewAuto();           // 동적 만들기에 사용되는 protected 생성자입니다.

	enum { IDD = IDD_VIEW_AUTO };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CDlgMessage * m_pDlgMessage;
	CDlgMessage * m_pDlgDefTypeMsg;
	CDlgMessage * m_pDlgErrorMsg;
	CDlgMessage * m_pDlgMeasure;
	CDlgDisplayMessge * m_pDlgDspMsg;		//2016.01.29 Mr.Chu
	CDlgManualJudge * m_pDlgManualJudge;		//2018.05.14
	/////////////////////////////////// Loading 다이얼로그 ///////////////////////////////////////////
	void DeleteLoadingDlg();							// Loading 다이얼로그를 해제하는 함수
	void CreateLoadingDlg();							// Loading 다이얼로그를 생성한다.
	void LoadingDlgOpenDelay();							// 스레드 내부에서 Loading 다이얼로그가 생성될때까지 대기하는 함수
	void OpenLoadingDlg(TCHAR *ptszText);				// Loading 다이얼로그를 띄우는 함수
	void CloseLoadingDlg();								// Loading 다이얼로그를 닫는 함수
	void m_fnPopUpDefectImage();
	HANDLE m_hThreadDetectFileSelect;
	//////////////////////////////////////////////////////////////////////////////////////////////////
	CDlgPopUpImage* m_pDlgPopUpImage;
public:
	virtual void OnInitialUpdate();

	int nTestCnt;
	int nTestPnlCnt;
	CGridCtrl* m_GridProcLog;
	CGridCtrl* m_GridDefectSummary;
	CGridCtrl* m_GridPanelSummary;
	CGridCtrl* m_GridReviewDefectSummary;		//2015.06.22

	CDlgPanelCodeTest* m_pDlgPanelCodeTest;		//2016.09.14 Mr.Chu
	CDlgDefectMap* m_pDlgMap;
	CDlgDefectInfo* m_pDlgDefInfo;
	CDlgVisionState* m_pDlgVisionState;
	CDlgStageState* m_pDlgStageState;
	CDlgLuminanceState* m_pDlgLuminanceState;
	CDlgPanelJudge* m_pDlgPanelJudge;
	CDlgTemperature* m_pDlgTemperature;	//2016.04.04
	GRADE_SET_Dialog* m_pDlgGradeSet;	// 2018.12.03 ryu
	F_GRDAE_Dialog* m_pDlgFgradeSet; //2019.01.28 ryu

	void m_fnInitGridLog();
	void m_fnInitGridDefectSummary(int nEqpObject);
	void m_fnInitGridPanelSummary();
	void UpdateGridDetectSummaryText();
	//void UpdateGridDetectSummaryRow();
	LRESULT UpdateGridDetectSummaryRow(WPARAM wParam, LPARAM lParam);
	LRESULT m_fnUpdatePanelSummary(WPARAM wParam, LPARAM lParam);
	//void m_fnUpdatePanelSummary(CString strQmsFilePath);
	void m_fnUpdateSummary(CString strData, BOOL bClear);
	void m_fnInitGridReviewDefectSummary(int nEqpObject);					//2015.06.22
	void UpdateGridReviewDefectSummaryText();				//2015.06.22
	void UpdateGridReviewDefectSummaryRow(CString strPath);	//2015.06.22

	void m_fnInitGradeSet();	//2018.12.13 add by ryu

	//void m_fnAddLog(CString strLog);
	LRESULT m_fnAddLog(WPARAM wParam, LPARAM lParam);
	//void m_fnAddLog(CString strLog, BOOL bNoUseTotalLog = FALSE); // 2015.11.27 comment by ikm for Log Delay
	void m_fnClearLog();

	void m_fnCreateDlgDefInfo();
	void m_fnCreateDlgMap();
	void m_fnCreateDlgVisionState();
	void m_fnCreateDlgStageState();
	void m_fnCreateDlgPanelJudge();
	void m_fnCreateDlgLuminanceState();
	void m_fnSaveFilePanelInfo(CString strQmsFilePath);
	void m_fnSaveFilePanelStats();		//2019.01.17 add by mby
	LRESULT m_fnSaveFileMeasureInfo(WPARAM wParam, LPARAM lParam);
	//void m_fnSaveFileMeasureInfo(CString strMeasureData, int nEQType);	//2015.10.06
	STRU_MEASURE_DATA m_fnSetMeasureInfo(CString strMeasureData, int nEQType);		//2015.10.06
	void m_fnSaveFileOverlapDefectInfo(CString strPanelID, CString strOverlapDefectData, int nOverlapCnt);	//2015.10.07
	CString m_fnGetDefectSummaryDataPath();		//2015.06.22
	CString m_fnGetPanelSummaryDataPath();		//2015.06.22
	void m_fnLoadPanelSummary();				//2015.06.22
	void m_fnViewPanelSummary(CString strLine);	//2015.06.22

	CString m_fnGetMeasureSummaryDataPath();		//2015.10.06
	CString m_fnGetOverlapDefectSummaryDataPath();	//2015.10.07
	CString m_fnGetQmsBackUpDataPath();				//2015.10.08
	void    m_fnAlarmDialog(CDlgMessage*& DlgMsg);
	void	m_fnDisplayMSGDialog(CDlgDisplayMessge*& DlgDspMSG, WPARAM wParam, LPARAM lParam);	//2016.01.29 Mr.Chu
	void    m_fnManualJudgeDialog(CDlgManualJudge*& DlgMsg);	//2018.05.14
	afx_msg LRESULT m_fnDisplayMSGSet(WPARAM wParam, LPARAM lParam);	//2016.01.29 Mr.Chu
	afx_msg LRESULT m_fnAlarmSet(WPARAM wParam, LPARAM lParam);	
	afx_msg LRESULT m_fnPolMarkIlegalAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnNgJudgelAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnMeasureAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnContactFailAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnDefTypeAlarmSet(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT m_fnLuminanceAlarmSet(WPARAM wParam, LPARAM lParam);	//2016.03.02
	afx_msg LRESULT m_fnManualJudgeSet(WPARAM wParam, LPARAM lParam);		//2018.05.14
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnTest2();
	afx_msg void OnBnClickedBtnTest3();
	afx_msg void OnBnClickedBtnReset();
	afx_msg LRESULT m_fnDistroyMSGDialog(WPARAM wParam, LPARAM lParam);

	BOOL m_bLog_Sel[9];
	BOOL m_bLog_S1;
	BOOL m_bLog_S2;
	BOOL m_bLog_S3;
	BOOL m_bDisChk;
	BOOL m_bDisChk2;

	afx_msg void OnBnClickedChkLogS1();
	afx_msg void OnBnClickedChkLogS2();
	afx_msg void OnBnClickedChkLogS3();
	afx_msg void OnGridClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult);
	afx_msg void OnGridClickReviewDetectList(NMHDR *pNotifyStruct, LRESULT *pResult);	//2015.07.06
	afx_msg void OnGridDBClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult);

	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedBtnPanel1();
	afx_msg void OnBnClickedBtnPanel2();

	static unsigned __stdcall ThreadDetectFileSelect(void *pArg);
	afx_msg void OnBnClickedRadio(UINT ID);
	afx_msg void OnStnClickedGridPnlSurmmary();
	afx_msg void OnBnClickedBtnCntReset();
	afx_msg void OnGridClickPanelList(NMHDR *pNotifyStruct, LRESULT *pResult);	//2015.06.22

	//2015.06.22
	CTabCtrl m_TabDefectSummary;
	afx_msg void OnTcnSelchangeTabDefectSummary(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTabDefectSummary(NMHDR *pNMHDR, LRESULT *pResult);

	//2015.06.22
	CTabCtrl m_ctrlLogTab;
	CListBox m_ctrlLogList1;
	CListBox m_ctrlLogList2;
	CListBox m_ctrlLogList3;
	CListBox m_ctrlLogList4;
	CListBox m_ctrlLogList5;
	CListBox m_ctrlLogList6;
	CListBox m_ctrlLogList7;
	CListBox m_ctrlLogList8;
	CListBox m_ctrlLogList9;
	CListBox m_ctrlLogList10;

	afx_msg void OnTcnSelchangeTabDefectLog(NMHDR *pNMHDR, LRESULT *pResult);
	void m_fnCheakLogFunc(int nStageID);

	int m_nViewDefectSummary;	//2015.07.06

	void testResultProc();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	BOOL m_bResetFlag[3];

	CString m_strValue_GradeAuto[MAX_COUNT_GRADE];	// 13
	CString m_strValue_RankAuto[ MAX_COUNT_RANK ];   // 14
	CDlgMessage2* m_pDlgMeaErrorMsg;
	CString m_strValue_NumAuto[VALUE_COUNT];
	
	//int m_nOriginType;
	void m_fnSelOriginType(UINT ID);
	void m_fnActiveXYReversal(UINT ID);
	void m_fnActiveReportCoordQMS(UINT ID);
	afx_msg void OnBnClickedBtnTemperature();
	//BOOL m_bMajorAxisType;	//2016.04.14 Mr.Chu		장축,단축 셋팅 변수
	afx_msg void OnBnClickedBtnPanelCodeTest();
	CString m_fnSetDefectName(CString strValue);
	afx_msg void OnBnClickedGrdae();

	afx_msg void OnBnClickedGradeSet();
};


