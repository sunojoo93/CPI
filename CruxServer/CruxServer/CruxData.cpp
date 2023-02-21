#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "StdAfx.h"
#include "CruxData.h"

#include "afxwin.h"		// MainFrame, View에서 사용


CCruxData* CCruxData::m_Instance	= NULL;					// Instance 초기화

//  Singleton Instance 조회
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

// 생성자
CCruxData::CCruxData(void)
{
	m_nCurrentViewID		= 0;				// 현재 보여지고 있는 Form View 화면

	m_sRootPath				= _T("D:\\Crux\\");		// 기본적인 경로
	m_sConfigFile			= _T("Crux.ini");			// 설정값이 들어가 있는 파일
	m_sRecipeFile			= _T("Model.ini");		// 모델별 레서피가 저장된 파일

}

// 소멸자
CCruxData::~CCruxData(void)
{
	// FormView 삭제
	m_fnRemoveView();
	m_listViewData.RemoveAll();			// 사용된 데이터 삭제
	m_listViewData.FreeExtra();			// 메모리 해제

	// Dialog 삭제
	m_fnRemoveDialog();
	m_listDialogData.RemoveAll();
	m_listDialogData.FreeExtra();
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data 조회 및 설정 함수들

//  기본경로 설정
void CCruxData::m_fnSetRootPath(CString sPath)
{
	m_sRootPath = sPath;
}

// 기본 경로 조회
CString	CCruxData::m_fnGetRootPath(void)
{
	return m_sRootPath;
}

// 레서피 파일을 지정한다.
void	CCruxData::m_fnSetRecipeFile(CString sFile)
{
	m_sRecipeFile	= sFile;		// _T("Model.ini");		// 모델별 레서피가 저장된 파일

}

// 레서피 파일을 조회한다.
CString	CCruxData::m_fnGetRecipeFile(void)
{
	return m_sRecipeFile;
}

// 설정파일을 조회한다.
CString	CCruxData::m_fnGetConfigFile(void)
{
	return m_sConfigFile;				// 설정값이 들어가 있는 파일

}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From View 관련 함수들

// View를 추가한다.
void	CCruxData::m_fnAddView(UINT nID, CView* pView, CString sCaption, BOOL bCreateFlag)
{
	stViewInfo	rData;

	rData.nID		= nID;
	rData.pView		= pView;
	rData.sCaption	= sCaption;
	rData.bCreate	= bCreateFlag;				// 직접 Create해줄것인가 ?


	m_listViewData.Add( rData );
	
	// 지정 View를 새로이 생성하라고 설정되어있다면, Create 수행
	if ( bCreateFlag )
	{
		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame 획득
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


// 현재 보여지고있는 View의 Caption
CString CCruxData::m_fnGetCurrentViewCaption(void)
{
	return m_fnGetViewCaption(m_nCurrentViewID);
}


// 지정 View의 Caption을 조회한다.
CString CCruxData::m_fnGetViewCaption(UINT nID)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		if ( m_listViewData[i].nID == nID ) return m_listViewData[i].sCaption;				// 원하는 ID의 View Caption을 되돌려준다.
	}//of for i

	return _T("NONE");
}



// 지정 View의 포인터를 조회한다.
CView* CCruxData::m_fnGetViewPtr(UINT nID)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		if ( m_listViewData[i].nID == nID ) return m_listViewData[i].pView;					// 원하는 ID의 View Pointer를 되돌려준다.
	}//of for i

	return NULL;
}


// 모든 View를 삭제한다. (종료시)
void CCruxData::m_fnRemoveView(void)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		//if ( m_listViewData[i].bCreate )						// 직접 Create해준것이라면,
		//	delete m_listViewData[i].pView;
	}//of for i
}


// View에서 Initial을 수행 할 수 있도록 조치한다.
void CCruxData::m_fnInitialView(void)
{
	int i;

	for (i = 0; i < m_listViewData.GetCount(); i++)
	{
		::SendMessage(m_listViewData[i].pView->m_hWnd, UM_INITIAL_DATAUPDATE, 0, 0);		// 화면 초기화 및 Update 시작 메세지 전송
	}//of for i
}


// 지정 ID의 View로 화면을 변경한다.
void CCruxData::m_fnSetChangeView(UINT nID)
{
	// 지정한 ID에 해당되는 View를 검색하여 화면 전환을 지령한다.
	m_fnSetChangeView(m_fnGetViewPtr(nID));
	m_nCurrentViewID = nID;						// 현재 화면에 보여지고 있는 Form의 ID 갱신
}


// 지정 View로 화면을 변경한다.
void CCruxData::m_fnSetChangeView(CView* pView)
{

	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame 획득
	CView*			pOldView		= pFrame->GetActiveView() ;				// 현재 보여지는 View
	CView*			pNewView		= pView;

	if ( pNewView != NULL )
	{
		::SendMessage(pOldView->m_hWnd, UM_MAINMENU_CHANGE, 0, 0);			// 이전 화면의 Update 중지 지령

		 UINT temp = ::GetWindowLong(pOldView->m_hWnd, GWL_ID);
		 ::SetWindowLong(pOldView->m_hWnd, GWL_ID,	::GetWindowLong(pNewView->m_hWnd, GWL_ID));
		 ::SetWindowLong(pNewView->m_hWnd, GWL_ID,	temp);

		 // Display and update the new current view - hide the old one    
		 pOldView->ShowWindow(SW_HIDE);
		 pNewView->ShowWindow(SW_SHOW);

		 pFrame->SetActiveView(pNewView);
		 pNewView->Invalidate();
		 pFrame->RecalcLayout();

		::PostMessage(pNewView->m_hWnd, UM_MAINMENU_CHANGE, 1, 0);			// 새로 갱신된 화면의 Update 시작 지령
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Dialog 관련 함수 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

// Dialog를 추가한다.
//
void CCruxData::m_fnAddDialog(UINT nIndex, UINT nID, CDialog* pDialog, CString sCaption, BOOL bModal)
{
	stDialogInfo	rData;

	rData.nIndex	= nIndex;				// 식별용 Index, 검색시 사용
	rData.nID		= nID;					// Resource ID
	rData.pDialog	= pDialog;				// Dialog pointer
	rData.sCaption	= sCaption;				// 이름
	rData.bModal	= bModal;				// Modal Dialog인가 ?

	m_listDialogData.Add( rData );
	
	if ( !bModal )				// Modaless인경우에는 Create를 해준다.
	{
		// 지정 Dialog를 생성한다.
		CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame 획득

		pDialog->Create(nID, (CWnd*)pFrame);
		pDialog->ShowWindow( SW_HIDE );
	}

}


// 지정 Index의 DialogBar 포인터를 조회한다.
CDialog* CCruxData::m_fnGetDialogPtr(UINT nIndex)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		if ( m_listDialogData[i].nIndex == nIndex ) return m_listDialogData[i].pDialog;	// 원하는 Index의 Dialog Pointer를 되돌려준다.
	}//of for i

	return NULL;
}


// 모든 DialogBar를 삭제한다. (종료시)
void CCruxData::m_fnRemoveDialog(void)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		m_listDialogData[i].pDialog->DestroyWindow();
		delete m_listDialogData[i].pDialog;
	}//of for i
}

// 원하는 Index의 Dialog 접근용 Index를 되돌려준다.
int CCruxData::m_fnGetDialogIndex(UINT nIndex)
{
	int i;

	for (i = 0; i < m_listDialogData.GetCount(); i++)
	{
		if ( m_listDialogData[i].nIndex == nIndex ) return i;	// 원하는 Index의 Dialog 접근용 Index를 되돌려준다.
	}//of for i

	return -1;		// 찾지 못함
}


// 지정 Dialog를 화면에 보여준다.
int CCruxData::m_fnShowDialog(UINT nIndex)
{
	int nIdx = m_fnGetDialogIndex( nIndex );

	if ( nIdx >= 0 )			// 정상적인 Index인가 ?
	{
		if ( !m_listDialogData[nIdx].bModal )
		{
			CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);			// MainFrame 획득

			m_listDialogData[nIdx].pDialog->ShowWindow( SW_SHOW );			// Modalness로 화면에 보여준다.
			m_listDialogData[nIdx].pDialog->SetWindowPos(&(pFrame->wndTopMost),0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);		// 최상위로 만든다.
		}
		else
			return (int)m_listDialogData[nIdx].pDialog->DoModal();			// Modal 형태로 보여준다.

		return 0;		// Modaless의 정상 종료
	}

	return -1;			// 잘못된 경우
}