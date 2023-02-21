#pragma once  

class CAdvComboBox : public CComboBox  
{  
	DECLARE_DYNAMIC(CAdvComboBox)  

public:  
	CAdvComboBox(int nItem, int nSubItem, CStringList* plstItems, int nSel);  
	virtual ~CAdvComboBox();  

	virtual BOOL PreTranslateMessage(MSG* pMsg);  

protected:  
	DECLARE_MESSAGE_MAP()  

public:  
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);  
	afx_msg void OnKillFocus(CWnd* pNewWnd);  
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);  
	afx_msg void OnNcDestroy();  
	afx_msg void OnCloseUp();  

private:  
	int         m_nItem;  
	int         m_nSubItem;  
	CStringList m_lstItems;  
	int         m_nSel;  
	bool        m_bESC;  
};  