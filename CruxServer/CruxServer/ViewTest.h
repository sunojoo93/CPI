#pragma once



// CViewTest �� ���Դϴ�.

class CViewTest : public CFormView
{
	DECLARE_DYNCREATE(CViewTest)

protected:
	virtual ~CViewTest();

public:
	CViewTest();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.

	enum { IDD = IDD_VIEW_TEST };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


