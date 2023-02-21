// SeqTimeMng.cpp: implementation of the CSeqTimeMng class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "SeqTimeMng.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define CV_1NEXT_HANDLING_TIMEOUT	30
#define CV_CUR_HANDLING_TIMEOUT	30


CSeqTimeMng::CSeqTimeMng()
{
	m_nStartLog = 0;
	m_nStart1NextHandlingTimeout = 0;
	m_nStartCurHandlingTimeout = 0;
	m_nStartDSensorTimeout = 0;
	m_nStartSSensorTimeout = 0;

}

CSeqTimeMng::~CSeqTimeMng()
{

}

int	CSeqTimeMng::m_fnShowInformDlg(int nSwType, char* szInform)
{
// 	CInformDlg dlg;
// 
// 	dlg.m_fnSetInform(nSwType, szInform);
// 
// 	return (int)dlg.DoModal();
	return 0;
}

BOOL CSeqTimeMng::m_fnIs1NextHandlignTimeout()
{
	int nTimeOut = m_fnGetCurrentTime() - m_nStart1NextHandlingTimeout;
	if( nTimeOut < 0 ) nTimeOut += 24 * 3600;
 
	if(CV_1NEXT_HANDLING_TIMEOUT <= nTimeOut)	return TRUE;
	else										return FALSE;
}

void CSeqTimeMng::m_fnReset1NextHandlingTimeout()
{
	m_nStart1NextHandlingTimeout = m_fnGetCurrentTime();
}

BOOL CSeqTimeMng::m_fnIsCurHandlignTimeout()
{
	int nTimeOut = m_fnGetCurrentTime() - m_nStartCurHandlingTimeout;
	if( nTimeOut < 0 ) nTimeOut += 24 * 3600;
 
	if(CV_CUR_HANDLING_TIMEOUT <= nTimeOut)	return TRUE;
	else										return FALSE;
}

void CSeqTimeMng::m_fnResetCurHandlingTimeout()
{
	m_nStartCurHandlingTimeout = m_fnGetCurrentTime();
}