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

// CViewManual �� ���Դϴ�.

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
	CViewManual();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

	enum { IDD = IDD_VIEW_MANUAL };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	afx_msg void OnBnClickedRadioStats();
	afx_msg void OnBnClickedRadioDefectSummary();
	afx_msg void OnBnClickedRadioDefect1();
	afx_msg void OnBnClickedRadioDefect2();
	afx_msg void OnBnClickedRadioDefect3();
	afx_msg void OnGridClickDetectFileList(NMHDR *pNotifyStruct, LRESULT *pResult);	// �׸��� Ŭ���� ���� // 2015.01.12 ljk
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

	/////////////////////////////////// Loading ���̾�α� ///////////////////////////////////////////
	void CreateLoadingDlg();							// Loading ���̾�α׸� �����Ѵ�.
	void LoadingDlgOpenDelay();							// ������ ���ο��� Loading ���̾�αװ� �����ɶ����� ����ϴ� �Լ�
	void OpenLoadingDlg(TCHAR *ptszText);				// Loading ���̾�α׸� ���� �Լ�
	void CloseLoadingDlg();								// Loading ���̾�α׸� �ݴ� �Լ�
	void DestroyLoadingDlg();							// Loading ���̾�α׸� �����ϴ� �Լ�
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateGridDetectFileRow(int nModifyRow);		// DetectFileList �׸����� ���μ��� �����ϴ� �Լ�
	void UpdateGridDetectFileText();					// DetectFileList �׸����� ���ڿ��� �����ϴ� �Լ�

	void ClearDetectInfo();								// Detect ������ �����ϴ� �Լ�
	void ReadDetectFile();								// Detect File ������ ������ �о� ����ü�� ����

	void UpdateGridDetectSummaryRow(int nModifyRow);	// DetectSummaryList �׸����� ���μ��� �����ϴ� �Լ�
	void UpdateGridDetectSummaryText();					// DetectSummaryList �׸����� ���ڿ��� �����ϴ� �Լ�
};


