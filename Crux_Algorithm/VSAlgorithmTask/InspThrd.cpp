// InspThrd.cpp : implementation file
//

#include "stdafx.h"
#include "InspThrd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInspThrd

IMPLEMENT_DYNCREATE(CInspThrd, CWinThread)

CInspThrd::CInspThrd()
{
	m_bBusy = false;
}

CInspThrd::~CInspThrd()
{
	TRACE("CInspThrd Bye~\n");
}

BOOL CInspThrd::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CInspThrd::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CInspThrd, CWinThread)
	//{{AFX_MSG_MAP(CInspThrd)
	ON_THREAD_MESSAGE(WM_START_INSPECTION, (void(AFX_MSG_CALL CInspThrd::*)(WPARAM, LPARAM))OnStartInspection)
	ON_THREAD_MESSAGE(WM_START_SAVE_IMAGE, (void(AFX_MSG_CALL CInspThrd::*)(WPARAM, LPARAM))OnStartSaveImage)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInspThrd methods
void CInspThrd::Initialize(int nThreadCount)
{
	///////////////////////
	// Thread ID 초기화 : 가장 먼저 할 일
	m_nThrdID = nThreadCount;
	///////////////////////
}

/////////////////////////////////////////////////////////////////////////////
// CInspThrd message handlers

LRESULT CInspThrd::OnStartInspection(WPARAM wp, LPARAM lp)	//2016.10.17
{
	return 0;
}

LRESULT CInspThrd::OnStartSaveImage(WPARAM wp, LPARAM lp)	//2020.07.22
{
	return 0;
}

bool CInspThrd::IsThrdBusy()
{
	return m_bBusy;
}
