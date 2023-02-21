#pragma once



// CViewTest 폼 뷰입니다.

class CViewTest : public CFormView
{
	DECLARE_DYNCREATE(CViewTest)

protected:
	virtual ~CViewTest();

public:
	CViewTest();           // 동적 만들기에 사용되는 protected 생성자입니다.

	enum { IDD = IDD_VIEW_TEST };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


