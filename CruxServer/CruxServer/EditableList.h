//#if !defined(AFX_EDITABLELIST_H__18109D08_09CE_4DC6_B23D_42F9728E30C9__INCLUDED_)
//#define AFX_EDITABLELIST_H__18109D08_09CE_4DC6_B23D_42F9728E30C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditableList.h : header file
//

// InPlaceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit_ window

class CInPlaceEdit_ : public CEdit
{
// Construction
public:
	CInPlaceEdit_(int iItem, int iSubItem, CString sInitText);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit_)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInPlaceEdit_();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit_)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iItem;
	int m_iSubItem;
	CString m_sInitText;
	BOOL    m_bESC;	 	// To indicate whether ESC key was pressed
};

/////////////////////////////////////////////////////////////////////////////
// CEditableList window

class CEditableList : public CListCtrl
{
// Construction
public:
	CEditableList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditableList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditableList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditableList)
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SetColumnEditable(int nColumn); 

private:
	int HitTestEx(CPoint &point, int *col) const;
	CEdit* EditSubLabel( int nItem, int nCol );

	int	m_nEditColum;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_EDITABLELIST_H__18109D08_09CE_4DC6_B23D_42F9728E30C9__INCLUDED_)
