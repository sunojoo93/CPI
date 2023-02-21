#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CruxServerView.cpp : CCruxServerView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "CruxServer.h"
#endif

#include "CruxServerDoc.h"
#include "CruxServerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCruxServerView

IMPLEMENT_DYNCREATE(CCruxServerView, CView)

BEGIN_MESSAGE_MAP(CCruxServerView, CView)
END_MESSAGE_MAP()

// CCruxServerView ����/�Ҹ�

CCruxServerView::CCruxServerView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CCruxServerView::~CCruxServerView()
{
}

BOOL CCruxServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CCruxServerView �׸���

void CCruxServerView::OnDraw(CDC* /*pDC*/)
{
	CCruxServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CCruxServerView ����

#ifdef _DEBUG
void CCruxServerView::AssertValid() const
{
	CView::AssertValid();
}

void CCruxServerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCruxServerDoc* CCruxServerView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCruxServerDoc)));
	return (CCruxServerDoc*)m_pDocument;
}
#endif //_DEBUG


// CCruxServerView �޽��� ó����
