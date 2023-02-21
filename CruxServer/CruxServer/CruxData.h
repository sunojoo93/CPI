//
// Crux GUI�� ���� Data�� ����/�����ϴ� Class��, Singleton���� ���ȴ�.
//
//

#pragma once

#include "CruxDefine.h"


class CCruxData
{
private:
	CCruxData(void);								// �ߺ� ���� ����
	~CCruxData(void);
    static CCruxData* m_Instance;					// Singleton ��ü
public:
    static CCruxData* m_fnGetInstance();				// Singleton
	static void m_fnReleaseInstance();					// Singleton ����
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

private:

	// ȭ�鿡 ǥ�õǴ� View �� Dialog ����
	CstViewInfo			m_listViewData;				// ȭ�� FormView ������ ��Ƶΰ� �ִ� List
	CstDialogInfo		m_listDialogData;			// ȭ�� Dialog ������ ��Ƶΰ� �ִ� List
	UINT				m_nCurrentViewID;			// ���� ȭ�鿡 �������� �ִ� View�� ID

	CString	m_sRootPath;							// ����� �⺻�� �Ǵ� ���
	CString	m_sConfigFile;							// �������� �� �ִ� ����
	CString	m_sRecipeFile;							// �𵨺� �����ǰ� ����� ����

public :
	void	m_fnSetRootPath(CString sPath);				//  �⺻��� ����
	CString	m_fnGetRootPath(void);						// �⺻ ��� ��ȸ

	void	m_fnSetRecipeFile(CString sFile);			// ������ ������ �����Ѵ�.
	CString	m_fnGetRecipeFile(void);					// ������ ������ ��ȸ�Ѵ�.
	CString	m_fnGetConfigFile(void);					// ���������� ��ȸ�Ѵ�.

public:

	// Form View ���� �Լ� -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
	CString m_fnGetViewCaption(UINT nID);				// ���� View�� Caption�� ��ȸ�Ѵ�.
	CString m_fnGetCurrentViewCaption(void);			// ���� ���������ִ� View�� Caption
	CView* m_fnGetViewPtr(UINT nID);					// ���� View�� �����͸� ��ȸ�Ѵ�.

	void m_fnAddView(UINT nID, CView* pView, CString sCaption, BOOL bNewFlag = TRUE);			// View�� �߰��Ѵ�.
	void m_fnRemoveView(void);							// ��� View�� �����Ѵ�. (�����)
	void m_fnSetChangeView(UINT nID);					// ���� View�� ȭ���� �����Ѵ�.
	void m_fnSetChangeView(CView* pView);				// ���� View�� ȭ���� �����Ѵ�.
	void m_fnInitialView(void);							// View���� Initial�� ���� �� �� �ֵ��� ��ġ�Ѵ�.
	
	// Dialog ���� �Լ� -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
	void m_fnAddDialog(UINT nIndex, UINT nID, CDialog* pDialog, CString sCaption, BOOL bModal);			// Dialog�� �߰��Ѵ�.
	CDialog* m_fnGetDialogPtr(UINT nIndex);				// ���� Index�� Dialog �����͸� ��ȸ�Ѵ�.
	void m_fnRemoveDialog(void);						// ��� Dialog�� �����Ѵ�. (�����)
	int m_fnShowDialog(UINT nIndex);					// ���� Dialog�� ȭ�鿡 �����ش�.
	int m_fnGetDialogIndex(UINT nIndex);				// ���� Index�� �ش�Ǵ� Dialog�� �迭 ���ٿ� Index�� ��ȯ�Ѵ�.
};

