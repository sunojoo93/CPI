#pragma once


/*
*	Module name	:	CThread
*	Function	:	쓰레드의 생성 기능을 제공한다.
*					쓰레드 구동을 위해 CThread 를 상속받아서 사용한다. 
*					상속 받은 Class 는 m_fnRunThread 를 재정의해서 사용한다.
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.0
*/
class CThread
{
public:
	CThread(int nStackSize = 0, int nPriority = THREAD_PRIORITY_NORMAL);	//생성자
	~CThread(void);															//소멸자

	/*멤버 함수*/
public:
								//내부 Thread 로 정의된 m_fnRunThread를 가동 시킨다.
	HANDLE						m_fnStartThread();		

								//내부 Thread 종료 기능을 제공한다.
	bool						m_fnWaitForTermination();

protected:
								//Virtual Function으로 상속 받는 Class 에서 재정의 하여 사용한다.
	virtual void				m_fnRunThread() = 0;
								 
private:
								//내부 Thread로서 실제적인 기능을 하는 m_fnRunThread를 호출한다.
	static unsigned __stdcall	__THREAD_ENTRY_POINT(void* pVoid);					

	/*멤버 변수*/
private:
	DWORD	m_dwThreadID;		//Thread ID 로서 Thread 생성시 ID를 가지고 있는다.
	HANDLE	m_hThread;			//Thread Handle 로서 Thread 생성시 Handle 을 가지고 있는다.
	int		m_nStackSize;		//Thread 생성시 필요한 Thread Stack size 를 가지고 있는다.
	int		m_nPriority;		//Thread 생성시 필요한 Thread Priority 를 가지고 있는다.
};