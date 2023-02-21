#pragma once

#include ".\Grid\GridCtrl.h"
#include "DlgDefectMap.h"
#include "DlgVisionState.h"
#include "DlgStageState.h"

#include "DataManager.h" // 2015.01.12 ljk						 
#include "LoadingDlg.h"	 // 2015.01.12 ljk
#include <vector>		 // 2015.01.12 ljk						 
#include <process.h>	 // 2015.01.12 ljk						 
using namespace std;	 // 2015.01.12 ljk

// CViewManual 폼 뷰입니다.

class CViewManual : public CFormView
{
	DECLARE_DYNCREATE(CViewManual)
private:
	std::vector<TCHAR *>		    m_vectorDetectFileList;		// 2015.01.12 ljk
	CLoadingDlg					   *m_pLoadingDlg;				// 2015.01.12 ljk
	HANDLE						    m_hThreadUpdateDetect;		// 2015.01.12 ljk
	int							    m_ngridDetectFileRow;		// 2015.01.12 ljk
	int							    m_ngridDetectFileCol;		// 2015.01.12 ljk
	HANDLE						    m_hThreadDetectFileSelect;	// 2015.01.12 ljk

	std::vector<STRU_DEFECT_INFO *> m_vectorDetectFileInfo;		// 2015.01.12 ljk
protected:
	virtual ~CViewManual();

public:
	CViewManual();           // 동적 만들기에 사용되는 protected 생성자입니다.

	enum { IDD = IDD_VIEW_MANUAL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg void OnBnClickedRadioStats();
	afx_msg void OnBnClickedRadioDefectSummary();
	afx_msg void OnBnClickedRadioDefect1();
	afx_msg void OnBnClickedRadioDefect2();
	afx_msg void OnBnClickedRadioDefect3();
	afx_msg void OnGridClickDetectFileList(NMHDR *pNotifyStruct, LRESULT *pResult);	// 그리드 클릭시 동작 // 2015.01.12 ljk
	afx_msg void OnGridClickDetectList(NMHDR *pNotifyStruct, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

	static unsigned __stdcall ThreadUpdateDetect(void *pArg);		// 2015.01.12 ljk	
	static unsigned __stdcall ThreadDetectFileSelect(void *pArg);	// 2015.01.12 ljk
public:
	virtual void OnInitialUpdate();


public:
	CGridCtrl* m_GridProcLog;
	CGridCtrl* m_GridDefect_Stats[GRID_DEFECT_STATS_MAX];

	void m_fnInitGridLog();
	void m_fnInitGridDefect_Stats();
	void m_fnChangeGrid(int nSelGrid);
	void m_fnUpdateGrid(int nGridIndex, CString strData, BOOL bClear);

	void m_fnAddLog(CString strLog);
	void m_fnClearLog();

	CDlgDefectMap* m_pDlgMap;
	CDlgVisionState* m_pDlgVisionState;
	CDlgStageState* m_pDlgStageState;

	void m_fnCreateDlgMap();
	void m_fnCreateDlgVisionState();
	void m_fnCreateDlgStageState();

	afx_msg void OnBnClickedRadioDefectTotal();
	afx_msg void OnBnClickedButtonUpdateDetect();

	/////////////////////////////////// Loading 다이얼로그 ///////////////////////////////////////////
	void CreateLoadingDlg();							// Loading 다이얼로그를 생성한다.
	void LoadingDlgOpenDelay();							// 스레드 내부에서 Loading 다이얼로그가 생성될때까지 대기하는 함수
	void OpenLoadingDlg(TCHAR *ptszText);				// Loading 다이얼로그를 띄우는 함수
	void CloseLoadingDlg();								// Loading 다이얼로그를 닫는 함수
	void DestroyLoadingDlg();							// Loading 다이얼로그를 해제하는 함수
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateGridDetectFileRow(int nModifyRow);		// DetectFileList 그리드의 라인수를 갱신하는 함수
	void UpdateGridDetectFileText();					// DetectFileList 그리드의 문자열을 갱신하는 함수

	void ClearDetectInfo();								// Detect 정보를 삭제하는 함수
	void ReadDetectFile();								// Detect File 내부의 내용을 읽어 구조체에 저장

	void UpdateGridDetectSummaryRow(int nModifyRow);	// DetectSummaryList 그리드의 라인수를 갱신하는 함수
	void UpdateGridDetectSummaryText();					// DetectSummaryList 그리드의 문자열을 갱신하는 함수
};


