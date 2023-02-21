#include "stdafx.h"  
#include "AdvListCtrl.h"  
#include "AdvComboBox.h"  

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CAdvListCtrl, CEditableList)  

CAdvListCtrl::CAdvListCtrl()  
{  
	m_nComboColumn = -1;  
}  

CAdvListCtrl::~CAdvListCtrl()  
{  
}  


BEGIN_MESSAGE_MAP(CAdvListCtrl, CEditableList)  
	ON_WM_HSCROLL()  
	ON_WM_VSCROLL()  
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CAdvListCtrl::OnLvnEndlabeledit)  
	ON_WM_LBUTTONDOWN()  
	ON_WM_LBUTTONUP()  
END_MESSAGE_MAP()  

void CAdvListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  
{  
	if(GetFocus() != this) SetFocus();  
	CEditableList::OnHScroll(nSBCode, nPos, pScrollBar);  
}  

void CAdvListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)  
{  
	if(GetFocus() != this) SetFocus();  
	CEditableList::OnVScroll(nSBCode, nPos, pScrollBar);  
}  

void CAdvListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)  
{  
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);  
	LVITEM* pItem = &pDispInfo->item;  

	if(pItem->pszText != NULL)  
	{  
		SetItemText(pItem->iItem, pItem->iSubItem, pItem->pszText);  
	}  

	*pResult = 0;  
}  

void CAdvListCtrl::OnLButtonDown(UINT nFlags, CPoint point)  
{  
	int nIndex;  
	

	int nColumn;  
	if((nIndex = HitTestEx(point, &nColumn)) != -1)  
	{  
		UINT flag = LVIS_FOCUSED;  
		if(/*(GetItemState(nIndex, flag) & flag) == flag &&*/ m_nComboColumn == nColumn)  
		{  
			//if(!(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS))  
			{  
				/*CStringList lstItems;  
				lstItems.AddTail(L"Item 1");  
				lstItems.AddTail(L"Item 2");  */

				CString strGet = this->GetItemText(nIndex, m_nComboColumn);  
				ShowAdvComboBox(nIndex, m_nComboColumn, m_lstItems, 0);  
				SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);  
			}  
		}  
		else  
		{  
			SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);  
			CEditableList::OnLButtonDown(nFlags, point);  
		}  
	}  

	//  CEditableList::OnLButtonDown(nFlags, point);  
}  

void CAdvListCtrl::OnLButtonUp(UINT nFlags, CPoint point)  
{  
	//  CEditableList::OnLButtonUp(nFlags, point);  
}  

int CAdvListCtrl::HitTestEx(CPoint& point, int* nCol) const  
{  
	int nColumn = 0;  
	int nRow = HitTest(point, NULL);  

	if(nCol) *nCol = 0;  

	// LVS_REPORT Check  
	if((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)  
		return nRow;  

	nRow = GetTopIndex();  
	int nBottom = nRow + GetCountPerPage();  

	if(nBottom > GetItemCount())  
		nBottom = GetItemCount();  

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);  
	int nColumnCount = pHeader->GetItemCount();  

	for(; nRow <= nBottom; ++nRow)  
	{  
		CRect rect;  
		GetItemRect(nRow, &rect, LVIR_BOUNDS);  

		if(rect.PtInRect(point))  
		{  
			for(nColumn = 0; nColumn < nColumnCount; ++nColumn)  
			{  
				int nColWidth = GetColumnWidth(nColumn);  
				if(point.x >= rect.left && point.x <= (rect.left + nColWidth))  
				{  
					if(nCol) *nCol = nColumn;  
					return nRow;  
				}  

				rect.left += nColWidth;  
			}  
		}  
	}  

	return -1;  
}  

void CAdvListCtrl::SetColumnCombo(int nColumn, CStringList& lstItems)  
{  
	POSITION ps;
	m_nComboColumn = nColumn;  

	for (ps = lstItems.GetHeadPosition(); ps != NULL;)
	{
		m_lstItems.AddTail(lstItems.GetNext(ps));
	}
}  



CComboBox* CAdvListCtrl::ShowAdvComboBox(int nItem, int nCol, CStringList& lstItems, int nSel)  
{  
	if(!EnsureVisible(nItem, TRUE)) return NULL;  

	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);  
	int nColumnCount = pHeader->GetItemCount();  
	if(nCol >= nColumnCount || GetColumnWidth(nCol) < 10)  
		return NULL;  

	int offset = 0;  
	for(int i = 0; i < nCol; ++i)  
	{  
		offset += GetColumnWidth(i);  
	}  

	CRect rect;  
	GetItemRect(nItem, &rect, LVIR_BOUNDS);  

	CRect rcClient;  
	GetClientRect(&rcClient);  
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)  
	{  
		CSize size;  
		size.cx = offset + rect.left;  
		size.cy = 0;  
		Scroll(size);  
		rect.left -= size.cx;  
	}  

	rect.left += offset + 4;  
	rect.right = rect.left + GetColumnWidth(nCol) - 3;  
	int nHeight = rect.bottom - rect.top;  
	rect.bottom += 5 * nHeight;  
	if(rect.right > rcClient.right) rect.right = rcClient.right;  

	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE |  
		CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;  
	CComboBox* pList = new CAdvComboBox(nItem, nCol, &lstItems, nSel);  
	pList->Create(dwStyle, rect, this, GetDlgCtrlID() );  
	pList->SetItemHeight(-1, nHeight);  
	pList->SetHorizontalExtent(GetColumnWidth(nCol));  

	return pList;  
}