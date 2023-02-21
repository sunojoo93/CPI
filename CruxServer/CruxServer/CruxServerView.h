
// CruxServerView.h : CCruxServerView 클래스의 인터페이스
//

#pragma once


class CCruxServerView : public CView
{
protected: // serialization에서만 만들어집니다.
	CCruxServerView();
	DECLARE_DYNCREATE(CCruxServerView)

// 특성입니다.
public:
	CCruxServerDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CCruxServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // CruxServerView.cpp의 디버그 버전
inline CCruxServerDoc* CCruxServerView::GetDocument() const
   { return reinterpret_cast<CCruxServerDoc*>(m_pDocument); }
#endif

