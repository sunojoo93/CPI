#pragma once

#include "resource.h"
// GxOCX 참조
#include "Lib\GxButtonEx.h"			// 버튼 OCX 
#include "Lib\GxStaticEx.h"			// Static OCX

// CDlgTopBar 대화 상자입니다.

class CDlgTopBar : public CDialogBar
{
	DECLARE_DYNAMIC(CDlgTopBar)

public:
	CDlgTopBar(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTopBar();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_TOPBAR };

	void m_fnSetProcState(CString strProcState);
	void m_fnSetPPID(CString strPPID);
	void m_fnSetPanelID(CString strPanelID, int nIndex);
	void m_fnSetState(int nIndex, BOOL bEnable);


private:
	enum {
		eTIMER_Startup	= 0,			// 초기 실행시 구동 타이머
		eTIMER_Clock	= 1,			// 시계표시 타이머
		eTIMER_Status	= 2,			// 각종 상태를 표시해주는 타이머
	};//of enum


	COleDateTime	m_timeClock;		//  현재 시각
	CString			m_sDateTime;		// 현재 시각 문자열

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	
};
