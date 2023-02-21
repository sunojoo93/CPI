#pragma once

#include ".\Grid\GridCtrl.h"
#include "DlgPLCSetup.h"
#include "DlgLuminanceSetup.h"
#include "DlgJudgeUI.h"
#include "Login.h"
#include "DlgMessage.h"
#include "DlgRecipe.h"
#include "DlgSetJudgeItem.h"
#include "afxcmn.h"

// CViewSetup 폼 뷰입니다.

class CViewSetup : public CFormView
{
	DECLARE_DYNCREATE(CViewSetup)

protected:
	virtual ~CViewSetup();

public:
	CViewSetup();           // 동적 만들기에 사용되는 protected 생성자입니다.

	enum { IDD = IDD_VIEW_SETUP };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	//CDlgMessage *m_pDlgMessage[1000];
	//CDlgMessage *m_pDlgMessage;
	//CDlgMessage	*m_pDlgErrorMsg;
	CGridCtrl* m_GridIPList;
	CDlgPLCSetup* m_pDlgPLC;
	CDlgLuminanceSetup* m_pDlgLumSetup;
	CDlgJudgeUI* m_pDlgJudgeUI;
	BOOL m_nControlMode;
	BOOL m_nCtrlSockMode;
	BOOL m_nCIMSockMode;
	BOOL m_bUseCIM;
	BOOL m_bUseCIM2;	//2021.03.04 add by mby
	BOOL m_bUseAutoDel;
	BOOL m_bUseSemaphore;
	BOOL m_bUseNgFlag;
	int m_nEqpType;
	BOOL m_bUseManualJudge;	//2018.05.10
	BOOL m_bUseBubbleAreaAlarm;	//2020.02.28 add by mby
	BOOL m_bUseInspPC[9];		//2020.07.22 add by mby
	BOOL m_bUseAlignPC;		//2020.10.15 add by mby
	int m_nLightLifeCnt[3];	//2020.01.08 add by mby

	void m_fnInitGridIPList();
	void m_fnLoadIPList(BOOL bGridView);
	void m_fnSaveIPList();
	void m_fnLoadParam(BOOL bUIUpdate);
	void m_fnSaveParam();

	void m_fnAddCommInfo(CString strIP, int nPort, CString strItem);

	void m_fnUpdateStatus();

	void m_fnCreateDlgPLC();
	void m_fnCreateDlgLuminanceSetup();
	void m_fnCreateTabControl();	//2016.10.10 Mr.Chu
	void m_fnCreateParamSetupTool();	//2016.10.10 Mr.Chu

	//2015.11.05
	void m_fnRequestRecipeVision();
	void m_fnReadRecipePLC();
	void m_fnOpenRecipe();		//2016.10.10	OnBnClickedBtnRecipe  =>	m_fnOpenRecipe로 변경
	BOOL m_fnRecipeResCheck();
	BOOL m_bRecipeChangeMode;

	void m_fnInitMeasureLimitInfo();						//2019.02.18 add by mby
	//2015.11.05

public:
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//afx_msg LRESULT m_fnAlarmSet(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT m_fnMeasureAlarmSet(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT m_fnContactAlarmSet(WPARAM wParam, LPARAM lParam);
	//afx_msg LRESULT m_fnListlAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMainMenuChange(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnBnClickedBtnLoadInfo();
	afx_msg void OnBnClickedBtnSaveInfo();
	afx_msg void OnBnClickedBtnSockStart();
	afx_msg void OnBnClickedBtnSockStop();
	afx_msg void OnBnClickedBtnSaveSockSet();
	afx_msg void OnBnClickedBtnTestSetup();
	afx_msg void OnBnClickedRadioPlc();
	afx_msg void OnBnClickedRadioPc();
	afx_msg void OnBnClickedCheckCimUse();
	afx_msg void OnBnClickedCheckCimUse2();
	afx_msg void OnBnClickedCheckAutodelUse();
	afx_msg void OnBnClickedRadioCimSockPassive();
	afx_msg void OnBnClickedRadioAcc();
	//Chu
	void m_fnLogin(void);
	CLogin* m_pDlgLogin;
	//afx_msg void OnBnClickedBtnRecipe();			//2016.10.10	OnBnClickedBtnRecipe  =>	m_fnOpenRecipe로 변경

	CDlgRecipe* m_pDlgRecipe;	//2015.11.05
	CString				m_strPLCRecipe[PLC_MAX_RECIPE];	//2015.11.05
	//afx_msg void OnBnClickedBtnSetInspItem();

	//2015.12.10 Mr.Chu
	CDlgSetJudgeItem* m_pDlgSetJudgeItem;

	CDlgDEFSetup*	  m_pDlgDefSetup;
	afx_msg void OnBnClickedBtnOverlapDef();
	CTabCtrl m_ctrlSetupTab;
	afx_msg void OnTcnSelchangeTabSetup(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnMeasureSave();
	afx_msg void OnBnClickedBtnInchChange();
	afx_msg void OnBnClickedBtnMsave();
	afx_msg void OnBnClickedBtnSetLimit();
	afx_msg void OnBnClickedCheckManualJudge();
	afx_msg void OnBnClickedBtnLifeReset1();
	afx_msg void OnBnClickedBtnLifeReset2();
	afx_msg void OnBnClickedBtnLifeReset3();
	afx_msg void OnBnClickedBtnChkall();
};


