#if !defined(AFX_STAGE_H__E8D3E32F_6DAD_4C46_A743_6707FF28740F__INCLUDED_)
#define AFX_STAGE_H__E8D3E32F_6DAD_4C46_A743_6707FF28740F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InspectionCamera.h : header file
//
#include "InspThrd.h"

/////////////////////////////////////////////////////////////////////////////
// CInspPanel thread

class CInspPanel : public CObject
{
public:
	CInspPanel();           // protected constructor used by dynamic creation

// Attributes
public:
// Operations
public:
	bool ExitVision();
	bool InitVision();

	// ����ִ� ������ ���� ��� �� �����尡 ���涧���� ����ϴٰ� �˻� ����
	int StartInspection(WPARAM wParam, LPARAM lParam);
	int StartSaveImage(WPARAM wParam, LPARAM lParam);	// yskim 2020.07.22 �����������嵵 Thread�� ����

	virtual ~CInspPanel();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInspPanel)
	public:
	//}}AFX_VIRTUAL

// Implementation
// public:
// 	DECLARE_MESSAGE_MAP()

private:
	CInspThrd*	m_pInspThrd[MAX_THREAD_COUNT];
	int			m_nCurThrdID;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STAGE_H__E8D3E32F_6DAD_4C46_A743_6707FF28740F__INCLUDED_)
