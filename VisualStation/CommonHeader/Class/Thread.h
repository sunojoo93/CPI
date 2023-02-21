/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		Thread.h
@brief		Thread 를 Class 화 하여 사용할 수 있게 하는 가상 Class 를 제공한다.

@remark		
@author		김용태
@date		2006.
*/

#pragma once

#include <process.h>

/** 
@brief		쓰레드의 생성 기능을 제공한다.

@remark		
-			쓰레드 구동을 위해 CThread 를 상속받아서 사용해야 한다. 
-			상속 받은 Class 는 m_fnRunThread 를 재정의해서 사용한다.
@author		김용태
@date		2006.02.21
*/ 
class CThread
{
public:

	/** 
	@brief		생성자

	@return		없음
	@remark		Thread 생성시 필요한 인자를 받아서 초기화 한다.
	@author		김용태
	@date		2006.
	*/
	CThread(
		int nStackSize = 0,						///< Thread Stack Size
		int nPriority = THREAD_PRIORITY_NORMAL	///< Thread 의 우선순위
		)	
	{
		//Stack size 와 Priority 를 입력 받아 멤버 변수에 설정한다.
		m_nStackSize	= nStackSize;
		m_nPriority		= nPriority;

		//Thread ID 와 Thread handle 을 초기화 한다.
		m_dwThreadID	= 0;
		m_hThread		= NULL;
	}

	/** 
	@brief		소멸자

	@return		없음
	@remark		
	@author		김용태
	@date		2006.
	*/
	~CThread(void)
	{
		//Thread를 소멸한다.
		::CloseHandle(m_hThread);

		//Thread handle을 초기화 한다.
		m_hThread = NULL;						
	}

	/** 
	@brief		Thread 기능을 담당하는 m_fnRunThread를 가동 시킨다.

	@return		생성된 Thread 의 Handle
	@remark		
	@author		김용태
	@date		2006.
	*/
	HANDLE m_fnStartThread()
	{
		//내부 멤버 변수를 참고하여 Thread를 생성한다. 
		m_hThread = (HANDLE) _beginthreadex( 0, m_nStackSize, CThread::__THREAD_ENTRY_POINT, this, 0, (unsigned *)&m_dwThreadID);

		//Thread handle 을 반환한다.
		return m_hThread;						
	}

	/** 
	@brief		내부 Thread 종료 기능을 제공한다.

	@return		Thread 종료 후 Ture 반환
	@remark		
	@author		김용태
	@date		2006.
	*/
	bool m_fnWaitForTermination()
	{
		//Thread handle 을 참고하여 Thread를 종료한다.
		if (WAIT_OBJECT_0 != ::WaitForSingleObject(m_hThread, INFINITE))
		{
			::TerminateThread(m_hThread, 0);		
		}

		return true;
	}

protected:

	/** 
	@brief		Virtual Function으로 상속 받는 Class 에서 재정의 하여 사용한다.

	@return		없음
	@remark		상속 받는 Class 에서는 Thread 로 수행되는 기능을 재정의 하여 사용하여야 한다.
	@author		김용태
	@date		2006.
	*/
	virtual void				m_fnRunThread() = 0;

private:

	/** 
	@brief		내부 Thread로서 실제적인 기능을 하는 m_fnRunThread를 호출한다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006.
	*/
	static unsigned __stdcall __THREAD_ENTRY_POINT(
		void* pVoid									///< 생성된 CThread 객체의 Pointer
		)
	{
		//파라메터로 넘겨 받는 값은 CThread 객체의 Pointer 이다.
		CThread* pThis = (CThread*) pVoid;	

		//실제 기능을 담당하는 함수를 호출한다.
		pThis->m_fnRunThread();					
		return 0;
	}

	/*멤버 변수*/
private:
	DWORD	m_dwThreadID;		///< Thread ID 로서 Thread 생성시 ID를 가지고 있는다.
	HANDLE	m_hThread;			///< Thread Handle 로서 Thread 생성시 Handle 을 가지고 있는다.
	int		m_nStackSize;		///< Thread 생성시 필요한 Thread Stack size 를 가지고 있는다.
	int		m_nPriority;		///< Thread 생성시 필요한 Thread Priority 를 가지고 있는다.
};

/** 
@}
*/ 