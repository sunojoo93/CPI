// MainView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "MainView.h"


// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

CMainView::CMainView()
	: CFormView(CMainView::IDD)
{
}

CMainView::~CMainView()
{
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainView, CFormView)
END_MESSAGE_MAP()


// CMainView �����Դϴ�.

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMainView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMainView �޽��� ó�����Դϴ�.


void CMainView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	
	//GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();
}


BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::PreCreateWindow(cs);
}
