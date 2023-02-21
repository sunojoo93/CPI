#pragma once

#include "resource.h"
// GxOCX ����
#include "Lib\GxButtonEx.h"			// ��ư OCX 
#include "Lib\GxStaticEx.h"			// Static OCX

// CDlgTopBar ��ȭ �����Դϴ�.

class CDlgTopBar : public CDialogBar
{
	DECLARE_DYNAMIC(CDlgTopBar)

public:
	CDlgTopBar(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgTopBar();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_TOPBAR };

	void m_fnSetProcState(CString strProcState);
	void m_fnSetPPID(CString strPPID);
	void m_fnSetPanelID(CString strPanelID, int nIndex);
	void m_fnSetState(int nIndex, BOOL bEnable);


private:
	enum {
		eTIMER_Startup	= 0,			// �ʱ� ����� ���� Ÿ�̸�
		eTIMER_Clock	= 1,			// �ð�ǥ�� Ÿ�̸�
		eTIMER_Status	= 2,			// ���� ���¸� ǥ�����ִ� Ÿ�̸�
	};//of enum


	COleDateTime	m_timeClock;		//  ���� �ð�
	CString			m_sDateTime;		// ���� �ð� ���ڿ�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	
};
