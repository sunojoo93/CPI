#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "StdAfx.h"
#include "CruxData.h"

#include "afxwin.h"		// MainFrame, View���� ���


CCruxData* CCruxData::m_Instance	= NULL;					// Instance �ʱ�ȭ

//  Singleton Instance ��ȸ
CCruxData* CCruxData::m_fnGetInstance() 
{								
	if (m_Instance == NULL)
	{
		m_Instance = new CCruxData();
	}

   return m_Instance;
}

void CCruxData::m_fnReleaseInstance()
{
   if (m_Instance != NULL) delete m_Instance;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

// ������
CCruxData::CCruxData(void)
{
	m_nCurrentViewID		= 0;				// ���� �������� �ִ� Form View ȭ��

	m_sRootPath				= _T("D:\\Crux\\");		// �⺻���� ���
	m_sConfigFile			= _T("Crux.ini");			// �������� �� �ִ� ����
	m_sRecipeFile			= _T("Model.ini");		// �𵨺� �����ǰ� ����� ����

}

// �Ҹ���
CCruxData::~CCruxData(void)
{
	// FormView ����
	m_fnRemoveView();
	m_listViewData.RemoveAll();			// ���� ������ ����
	m_listViewData.FreeExtra();			// �޸� ����

	// Dialog ����
	m_fnRemoveDialog();
	m_listDialogData.RemoveAll();
	m_listDialogData.FreeExtra();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data ��ȸ �� ���� �Լ���

//  �⺻��� ����
void CCruxData::m_fnSetRootPath(CString sPath)
{
	m_sRootPath = sPath;
}

// �⺻ ��� ��ȸ
CString	CCruxData::m_fnGetRootPath(void)
{
	return m_sRootPath;
}

// ������ ������ �����Ѵ�.
void	CCruxData::m_fnSetRecipeFile(CString sFile)
{
	m_sRecipeFile	= sFile;		// _T("Model.ini");		// �𵨺� �����ǰ� ����� ����

}

// ������ ������ ��ȸ�Ѵ�.
CString	CCruxData::m_fnGetRecipeFile(void)
{
	return m_sRecipeFile;
}

// ���������� ��ȸ�Ѵ�.
CString	CCruxData::m_fnGetConfigFile(void)
{
	return m_sConfigFile;				// �������� �� �ִ� ����

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From View ���� �Լ���

// View�� �߰��Ѵ�.
void	CCruxData::m_fnAddView(UINT nID, CView* pView, CString sCaption, BOOL bCreateFlag)
{
	stViewInfo	rData;

	rData.nID		= nID;
	rData.pView		= pView;
	rData.sCaption	= sCaption;
	rData.bCreate	= bCreateFlag;				// ���� Create���ٰ��ΰ� ?


	m_listViewData.Add( rData );
	
	// ���� View�� ������ �����϶�� �����Ǿ��ִٸ�, Create ����
	if ( bCreateFlag )
	{
		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame ȹ��
		CDocument* pCurrentDoc = pFrame->GetActiveDocument();
    
		CCreateContext newContext;
		newContext.m_pNewViewClass = NULL;
		newContext.m_pNewDocTemplate = NULL;
		newContext.m_pLastView = NULL;
		newContext.m_pCurrentFrame = NULL;
		newContext.m_pCurrentDoc = pCurrentDoc;
    
		CRect rect(0, 0, 0, 0); // resize later
    
		rData.pView->Create(NULL, NULL, (AFX_WS_DEFAULT_VIEW & ~WS_VISIBLE), rect, pFrame, AFX_IDW_PANE_FIRST + nID, &newContext);
		rData.pView->OnInitialUpdate();
	}
}


// ���� ���������ִ� View�� Caption
CString CCruxData::m_fnGetCurrentViewCaption(void)
{
	return m_fnGetViewCaption(m_nCurrentViewID);
}


// ���� View�� Caption�� ��ȸ�Ѵ�.
CString CCruxData::m_fnGetViewCaption(UINT nID)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		if ( m_listViewData[i].nID == nID ) return m_listViewData[i].sCaption;				// ���ϴ� ID�� View Caption�� �ǵ����ش�.
	}//of for i

	return _T("NONE");
}



// ���� View�� �����͸� ��ȸ�Ѵ�.
CView* CCruxData::m_fnGetViewPtr(UINT nID)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		if ( m_listViewData[i].nID == nID ) return m_listViewData[i].pView;					// ���ϴ� ID�� View Pointer�� �ǵ����ش�.
	}//of for i

	return NULL;
}


// ��� View�� �����Ѵ�. (�����)
void CCruxData::m_fnRemoveView(void)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		//if ( m_listViewData[i].bCreate )						// ���� Create���ذ��̶��,
		//	delete m_listViewData[i].pView;
	}//of for i
}


// View���� Initial�� ���� �� �� �ֵ��� ��ġ�Ѵ�.
void CCruxData::m_fnInitialView(void)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		::SendMessage(m_listViewData[i].pView->m_hWnd, UM_INITIAL_DATAUPDATE, 0, 0);		// ȭ�� �ʱ�ȭ �� Update ���� �޼��� ����
	}//of for i
}


// ���� ID�� View�� ȭ���� �����Ѵ�.
void CCruxData::m_fnSetChangeView(UINT nID)
{
	// ������ ID�� �ش�Ǵ� View�� �˻��Ͽ� ȭ�� ��ȯ�� �����Ѵ�.
	m_fnSetChangeView(m_fnGetViewPtr(nID));
	m_nCurrentViewID = nID;						// ���� ȭ�鿡 �������� �ִ� Form�� ID ����
}


// ���� View�� ȭ���� �����Ѵ�.
void CCruxData::m_fnSetChangeView(CView* pView)
{

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame ȹ��
	CView*			pOldView		= pFrame->GetActiveView() ;				// ���� �������� View
	CView*			pNewView		= pView;

	if ( pNewView != NULL )
	{
		::SendMessage(pOldView->m_hWnd, UM_MAINMENU_CHANGE, 0, 0);			// ���� ȭ���� Update ���� ����

		 UINT temp = ::GetWindowLong(pOldView->m_hWnd, GWL_ID);
		 ::SetWindowLong(pOldView->m_hWnd, GWL_ID,	::GetWindowLong(pNewView->m_hWnd, GWL_ID));
		 ::SetWindowLong(pNewView->m_hWnd, GWL_ID,	temp);

		 // Display and update the new current view - hide the old one    
		 pOldView->ShowWindow(SW_HIDE);
		 pNewView->ShowWindow(SW_SHOW);

		 pFrame->SetActiveView(pNewView);
		 pNewView->Invalidate();
		 pFrame->RecalcLayout();

		::PostMessage(pNewView->m_hWnd, UM_MAINMENU_CHANGE, 1, 0);			// ���� ���ŵ� ȭ���� Update ���� ����
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Dialog ���� �Լ� -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

// Dialog�� �߰��Ѵ�.
//
void CCruxData::m_fnAddDialog(UINT nIndex, UINT nID, CDialog* pDialog, CString sCaption, BOOL bModal)
{
	stDialogInfo	rData;

	rData.nIndex	= nIndex;				// �ĺ��� Index, �˻��� ���
	rData.nID		= nID;					// Resource ID
	rData.pDialog	= pDialog;				// Dialog pointer
	rData.sCaption	= sCaption;				// �̸�
	rData.bModal	= bModal;				// Modal Dialog�ΰ� ?

	m_listDialogData.Add( rData );
	
	if ( !bModal )				// Modaless�ΰ�쿡�� Create�� ���ش�.
	{
		// ���� Dialog�� �����Ѵ�.
		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame ȹ��

		pDialog->Create(nID, (CWnd*)pFrame);
		pDialog->ShowWindow( SW_HIDE );
	}

}


// ���� Index�� DialogBar �����͸� ��ȸ�Ѵ�.
CDialog* CCruxData::m_fnGetDialogPtr(UINT nIndex)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		if ( m_listDialogData[i].nIndex == nIndex ) return m_listDialogData[i].pDialog;	// ���ϴ� Index�� Dialog Pointer�� �ǵ����ش�.
	}//of for i

	return NULL;
}


// ��� DialogBar�� �����Ѵ�. (�����)
void CCruxData::m_fnRemoveDialog(void)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		m_listDialogData[i].pDialog->DestroyWindow();
		delete m_listDialogData[i].pDialog;
	}//of for i
}

// ���ϴ� Index�� Dialog ���ٿ� Index�� �ǵ����ش�.
int CCruxData::m_fnGetDialogIndex(UINT nIndex)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		if ( m_listDialogData[i].nIndex == nIndex ) return i;	// ���ϴ� Index�� Dialog ���ٿ� Index�� �ǵ����ش�.
	}//of for i

	return -1;		// ã�� ����
}


// ���� Dialog�� ȭ�鿡 �����ش�.
int CCruxData::m_fnShowDialog(UINT nIndex)
{
	int nIdx = m_fnGetDialogIndex( nIndex );

	if ( nIdx >= 0 )			// �������� Index�ΰ� ?
	{
		if ( !m_listDialogData[nIdx].bModal )
		{
			CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame ȹ��

			m_listDialogData[nIdx].pDialog->ShowWindow( SW_SHOW );			// Modalness�� ȭ�鿡 �����ش�.
			m_listDialogData[nIdx].pDialog->SetWindowPos(&(pFrame->wndTopMost),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);		// �ֻ����� �����.
		}
		else
			return (int)m_listDialogData[nIdx].pDialog->DoModal();			// Modal ���·� �����ش�.

		return 0;		// Modaless�� ���� ����
	}

	return -1;			// �߸��� ���
}