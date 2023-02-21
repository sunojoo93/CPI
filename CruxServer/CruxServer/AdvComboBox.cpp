#include "stdafx.h"  
#include "AdvComboBox.h"  

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CAdvComboBox, CComboBox)  

	CAdvComboBox::CAdvComboBox(int nItem, int nSubItem, CStringList* plstItems, int nSel)  
	: m_nItem(nItem)  
	, m_nSubItem(nSubItem)  
	, m_nSel(nSel)  
	, m_bESC(false)  
{  
	m_lstItems.AddTail(plstItems);  
}  

CAdvComboBox::~CAdvComboBox()  
{  
}  


BEGIN_MESSAGE_MAP(CAdvComboBox, CComboBox)  
	ON_WM_CREATE()  
	ON_WM_KILLFOCUS()  
	ON_WM_CHAR()  
	ON_WM_NCDESTROY()  
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)  
END_MESSAGE_MAP()  

BOOL CAdvComboBox::PreTranslateMessage(MSG* pMsg)  
{  
	if(pMsg->message == WM_KEYDOWN)  
	{  
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)  
		{  
			::TranslateMessage(pMsg);  
			::DispatchMessage(pMsg);  

			return TRUE;  
		}  
	}  

	return CComboBox::PreTranslateMessage(pMsg);  
}  

int CAdvComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)  
{  
	if (CComboBox::OnCreate(lpCreateStruct) == -1)  
		return -1;  

	CFont* pFont = GetParent()->GetFont();  
	SetFont(pFont);  

	for(POSITION pos = m_lstItems.GetHeadPosition(); pos != NULL; )  
	{  
		AddString((LPCTSTR)(m_lstItems.GetNext(pos)));  
	}  
	SetCurSel(m_nSel);  
	SetFocus();  

	return 0;  
}  

void CAdvComboBox::OnKillFocus(CWnd* pNewWnd)  
{  
	CComboBox::OnKillFocus(pNewWnd);  

	CString strTemp;  
	GetWindowText(strTemp);  

	LV_DISPINFO dispinfo;  
	dispinfo.hdr.hwndFrom   = GetParent()->m_hWnd;  
	dispinfo.hdr.idFrom     = GetDlgCtrlID();  
	dispinfo.hdr.code       = LVN_ENDLABELEDIT;  

	dispinfo.item.mask      = LVIF_TEXT;  
	dispinfo.item.iItem     = m_nItem;  
	dispinfo.item.iSubItem  = m_nSubItem;  
	dispinfo.item.pszText   = m_bESC ? NULL : LPTSTR((LPCTSTR)strTemp);  
	dispinfo.item.cchTextMax= strTemp.GetLength();  

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);  

	PostMessage(WM_CLOSE);  
}  

void CAdvComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)  
{  
	if(nChar == VK_ESCAPE || nChar == VK_RETURN)  
	{  
		if(nChar == VK_ESCAPE)  
		{  
			m_bESC = true;  
		}  
		GetParent()->SetFocus();  
		return;  
	}  

	CComboBox::OnChar(nChar, nRepCnt, nFlags);  
}  

void CAdvComboBox::OnNcDestroy()  
{  
	CComboBox::OnNcDestroy();  

	delete this;  
}  

void CAdvComboBox::OnCloseUp()  
{  
	GetParent()->SetFocus();  
} 