
// CruxServerView.h : CCruxServerView Ŭ������ �������̽�
//

#pragma once


class CCruxServerView : public CView
{
protected: // serialization������ ��������ϴ�.
	CCruxServerView();
	DECLARE_DYNCREATE(CCruxServerView)

// Ư���Դϴ�.
public:
	CCruxServerDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// �����Դϴ�.
public:
	virtual ~CCruxServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CruxServerView.cpp�� ����� ����
inline CCruxServerDoc* CCruxServerView::GetDocument() const
   { return reinterpret_cast<CCruxServerDoc*>(m_pDocument); }
#endif

