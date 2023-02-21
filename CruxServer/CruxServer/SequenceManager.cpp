// SequenceManager.cpp: implementation of the CSequenceManagerManager2 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SeqTimeMng.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define AFXBEGINTHREAD(pfnThreadProc,pParam) AfxBeginThread(pfnThreadProc, pParam, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL)


CSequenceManager::CSequenceManager()
{
	m_hThread		= NULL;
	m_bFlagThread	= FALSE;

	m_nCurStep		= 0;
	m_nOldStep		= 0;	
}

CSequenceManager::~CSequenceManager()
{
	m_fnDeInit();
}

int	CSequenceManager::m_fnInit()
{
	return m_fnStartScenario(TRUE);
}

int	CSequenceManager::m_fnDeInit()
{
	if(m_bFlagThread) return m_fnStartScenario(FALSE);
	else return 0;
}

void CSequenceManager::m_fnInitStep(int nStepValue/*=SM_STEP_INIT*/)
{
	m_nCurStep	= nStepValue;
	m_nOldStep	= SM_STEP_INIT;
}

void CSequenceManager::m_fnClearScenario()
{
	m_nCurStep		= 0;
	m_nOldStep		= 0;	
}

void CSequenceManager::m_fnSetStep(int nStep)
{
	m_nCurStep = nStep;
}

void CSequenceManager::m_fnSetOldStep(int nOldStep)
{
	m_nOldStep = nOldStep;
}

int	CSequenceManager::m_fnGetStep()
{
	return m_nCurStep;
}

int	CSequenceManager::m_fnGetOldStep()
{
	return m_nOldStep;
}

BOOL CSequenceManager::m_fnCheckStep()
{
	if(m_nCurStep == m_nOldStep)
		return FALSE;
	else
	{
		m_nOldStep = m_nCurStep;		
		return TRUE;
	}
}

int	CSequenceManager::m_fnRetryStep()
{
	m_fnSetOldStep(SM_STEP_INIT);

	return m_nCurStep;
}

int CSequenceManager::m_fnStartScenario(BOOL bStart, BOOL nScenarioMode/*TRUE*/)
{
	CString strLog;
	int nRes = 0;

	if(bStart)
	{
		m_csExitThread.Lock();
		m_bFlagThread = true;
		m_csExitThread.Unlock();

		if(nScenarioMode == TRUE)
			m_pThread = AFXBEGINTHREAD(m_fnCheckScenario, (LPVOID)this);
		else
			m_pThread = AFXBEGINTHREAD(m_fnCheckInput, (LPVOID)this);
		
		if(NULL == m_pThread)
		{
			// error...
		}

		m_pThread->m_bAutoDelete = FALSE;
		m_pThread->ResumeThread(); 	
	}
	else
	{
		m_csExitThread.Lock();
		m_bFlagThread = false;
		m_csExitThread.Unlock();

		TerminateThread(m_pThread->m_hThread, 0);
		if(NULL != m_pThread)
		{
			::WaitForSingleObject(m_pThread->m_hThread,INFINITE);
			delete m_pThread;
			m_pThread = NULL;
		}
	}

	return nRes;
}

UINT CSequenceManager::m_fnCheckScenario(LPVOID lpThread)
{
	CSequenceManager*	pCtrlWnd = (CSequenceManager*)lpThread;

	while(1)
	{
		Sleep(SLEEP_COUNT);

		if(pCtrlWnd == NULL)			continue;
		if(pCtrlWnd->m_bFlagThread != TRUE)	break;

		if(pCtrlWnd->m_fnGetStep() && pCtrlWnd->m_fnCheckStep())
			pCtrlWnd->m_fnCheck();
	}

	return 0;
}

UINT CSequenceManager::m_fnCheckInput(LPVOID lpThread)
{
	CSequenceManager*	pCtrlWnd = (CSequenceManager*)lpThread;

	while(1)
	{
		Sleep(SLEEP_COUNT);

		if(pCtrlWnd == NULL)			continue;
		if(pCtrlWnd->m_bFlagThread != TRUE)	break;
			pCtrlWnd->m_fnCheck();
	}

	return 0;
}

int	CSequenceManager::m_fnGetCurrentTime()
{
	CTime time, getTime;

	time = getTime.GetCurrentTime();

	int nHSecond = time.GetHour() * 3600;
	int nMSecond = time.GetMinute() * 60;
	int nSecond  = time.GetSecond();

	return nHSecond + nMSecond + nSecond;
}


int CSequenceManager::m_fnGetCurrentTime2()
{
	SYSTEMTIME time;

	::GetLocalTime(&time);

	return time.wHour * 3600 * 1000 + time.wMinute * 60 * 1000 + time.wSecond * 1000 + time.wMilliseconds;
}
