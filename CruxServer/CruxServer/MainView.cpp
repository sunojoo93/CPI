// MainView.cpp : 구현 파일입니다.
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


// CMainView 진단입니다.

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


// CMainView 메시지 처리기입니다.


void CMainView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	
	//GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();
}


BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}
