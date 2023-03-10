#include "StdAfx.h"
#include "thread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	Class 생성자
*	Parameter	:	Thread 생성시 필요한 Stack Size, Thread 우선순위
*	Return		:	없음
*	Function	:	Class 생성자로서 Class 생성시 초기화 기능을 담당한다.
*	Create		:	2006.02.20
*	Author		:	김용태
*	Version		:	1.0
*/
CThread::CThread(int nStackSize, int nPriority)
{
	//Stack size 와 Priority 를 입력 받아 멤버 변수에 설정한다.
	m_nStackSize	= nStackSize;
	m_nPriority		= nPriority;

	//Thread ID 와 Thread handle 을 초기화 한다.
	m_dwThreadID	= 0;
	m_hThread		= NULL;
}

/*
*	Module name	:	Class 소멸자
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	Class 소멸자로서 Class 소멸시 호출 된다.
*	Create		:	2006.02.20
*	Author		:	김용태
*	Version		:	1.0
*/
CThread::~CThread(void)
{
	::CloseHandle(m_hThread);	//Thread를 소멸한다.
	m_hThread = NULL;			//Thread handle을 초기화 한다.
}

/*
*	Module name	:	m_fnStartThread
*	Parameter	:	없음
*	Return		:	생성된 Thread 의 Handle
*	Function	:	Thread를 생성한다.
*	Create		:	2006.02.20
*	Author		:	김용태
*	Version		:	1.0
*/
HANDLE CThread::m_fnStartThread()
{
	//내부 멤버 변수를 참고하여 Thread를 생성한다. 
	m_hThread = (HANDLE) _beginthreadex( 0, m_nStackSize, CThread::__THREAD_ENTRY_POINT, this, 0, (unsigned *)&m_dwThreadID);

	return m_hThread;	//Thread handle 을 반환한다.
}

/*
*	Module name	:	m_fnWaitForTermination
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	Thread의 종료 기능을 담당한다.
*	Create		:	2006.02.22
*	Author		:	김용태
*	Version		:	1.0
*/
bool CThread::m_fnWaitForTermination()
{
	//Thread handle 을 참고하여 Thread를 종료한다.
	if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, INFINITE))
	{
		::TerminateThread(m_hThread, 0);		
	}

	return true;
}

/*
*	Module name	:	__THREAD_ENTRY_POINT
*	Parameter	:	Thread 생성 Class 의 Pointer
*	Return		:	0
*	Function	:	실제 Thread fucntion 으로서, 실제 기능을 수행하는 m_fnRunThread를 호출한다.
*	Create		:	2006.02.20
*	Author		:	김용태
*	Version		:	1.0
*/
unsigned __stdcall CThread::__THREAD_ENTRY_POINT(void* pVoid)
{
	CThread* pThis = (CThread*) pVoid;		//파라메터로 넘겨 받는 값은 CThread 객체의 Pointer 이다.
	pThis->m_fnRunThread();					//실제 기능을 담당하는 함수 호출
	return 0;
}
