#pragma once  

#include "EditableList.h"

class CAdvListCtrl : public CEditableList  
{  
	DECLARE_DYNAMIC(CAdvListCtrl)  

public:  
	CAdvListCtrl();  
	virtual ~CAdvListCtrl();  

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);  
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);  
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);  
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);  
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);  

	int         HitTestEx(CPoint& point, int* nCol) const;  
	void        SetColumnCombo(int nColumn, CStringList& lstItems);  
	
protected:  
	DECLARE_MESSAGE_MAP()  



	CComboBox*  ShowAdvComboBox(int nItem, int nCol, CStringList& lstItems, int nSel);  

private:
	int				m_nComboColumn;  
	CStringList		m_lstItems;
};  

