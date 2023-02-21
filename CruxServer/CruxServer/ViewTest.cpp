// ViewTest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "ViewTest.h"


// CViewTest

IMPLEMENT_DYNCREATE(CViewTest, CFormView)

CViewTest::CViewTest()
	: CFormView(CViewTest::IDD)
{

}

CViewTest::~CViewTest()
{
}

void CViewTest::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewTest, CFormView)
END_MESSAGE_MAP()

// CViewTest 진단입니다.

#ifdef _DEBUG
void CViewTest::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewTest::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CViewTest 메시지 처리기입니다.


void CViewTest::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


BOOL CViewTest::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFormView::PreCreateWindow(cs);
}
