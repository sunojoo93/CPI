// ViewTest.cpp : ���� �����Դϴ�.
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

// CViewTest �����Դϴ�.

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


// CViewTest �޽��� ó�����Դϴ�.


void CViewTest::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


BOOL CViewTest::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	return CFormView::PreCreateWindow(cs);
}
