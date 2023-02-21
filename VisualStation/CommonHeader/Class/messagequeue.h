/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		MessageQueue.h
@brief		메시지 큐의 기능을 제공한다.

@remark		
@author		김용태
@date		2006.03.07
*/
 
#pragma once


#include <deque>										//STL deque 를 사용하기 위함
#include "cslock.h"

/** 
@brief		메시지 큐의 기능을 제공한다.

@remark		
@author		김용태
@date		2006.06.21
*/ 
template <class T>
class CMessageQueue : private std::deque<T>, private cslock
{
public:
	CMessageQueue(bool bWaitPopMessage = false);					
	~CMessageQueue(void);

	/*멤버 함수*/
public:
	bool	m_fnPushMessage(T pItem);					///< 메시지 큐에 메시지를 삽입 한다.
	T		m_fnPopMessage();							///< 메시지 큐의 첫번째 내용을 꺼낸다.
	bool	m_fnIsEmpty();								///< 큐의 내용이 존재하는지를 체크한다.
private:

	/*멤버 변수*/
public:
	HANDLE				m_hEventMessageExist;			///< 본 이벤트를 통해 메시지 유무를 확인 하고, 메시지를 처리한다.
private:	
	HANDLE				m_hEventMessagePop;				///< Message Pop 이 완료 되면 발생되는 이벤트.
	bool				m_bWaitPopMsg;					///< Message가 Push 되면 Pop될떄까지 대기 하는지 여부, 기본 False
	long				m_lQueueCount;					///< Queue 안의 메시지 개수
};

/** 
@}
*/ 


/** 
@brief		생성자

@return		없음
@remark		
@author		김용태
@date		2006.03.08
*/
template <class T>
CMessageQueue<T>::CMessageQueue(bool bWaitPopMessage) : std::deque<T>(), cslock(), m_bWaitPopMsg(bWaitPopMessage)
{
	//대기 쓰레드(메시지 처리자)를 깨우는 이벤트를 생성한다.
	m_hEventMessageExist = CreateEvent( NULL, TRUE,  FALSE,  NULL ); 

	// 메시지 Pop 에 대한 이벤트 헨들, Push 후 Pop 이 될때 까지 대기할 필요가 있을 경우 사용한다.
	m_hEventMessagePop = CreateEvent( NULL, TRUE,  FALSE,  NULL ); 

	//이벤트를 Reset 한다.
	ResetEvent(m_hEventMessageExist);
	//Queue 개수를 초기화한다.
	m_lQueueCount = 0;
}

/** 
@brief		소멸자

@return		없음
@remark		
@author		김용태
@date		2006.03.08
*/
template <class T>
CMessageQueue<T>::~CMessageQueue()

{
	lock();
	//대기 쓰레드(메시지 처리자)를 깨우는 이벤트를 종료한다.
	CloseHandle(m_hEventMessageExist);

	CloseHandle(m_hEventMessagePop);
	unlock();
}

/** 
@brief		메시지 큐에 메시지를 삽입 한다.

@return		없음
@remark		
@author		김용태
@date		2006.03.08
*/
template <class T>
bool CMessageQueue<T>::m_fnPushMessage(T pItem)
{
	bool bRet = true;

	lock();

	//Message 를 큐에 삽입한다.
	push_back(pItem);

	//큐 Count 를 중가한다.
	m_lQueueCount++;
	
	//
	ResetEvent(m_hEventMessagePop);

	//이벤트를 발생 시켜서 대기 중인 Thread를 깨운다.
	SetEvent(m_hEventMessageExist);

	unlock();

#ifdef _DEBUG
	//만일 메시지가 바로 Pop되어서 처리가 되어야 한다면, Pop event 를 최대 10 ms 대기한다.
	if(m_bWaitPopMsg)
		::WaitForSingleObject(m_hEventMessagePop, 10);
#endif

	return bRet;
}

/** 
@brief		메시지 큐의 첫번째 내용을 꺼낸다.

@return		없음
@remark		
@author		김용태
@date		2006.03.08
*/
template <class T>
T CMessageQueue<T>::m_fnPopMessage()
{
	T	pMessage;

	lock();

	//큐가 비었다면 Event Reset 후 NULL 반환 한다.
	if(0 == m_lQueueCount)
	{
		ResetEvent(m_hEventMessageExist);
		unlock();
		return NULL;
	}

	//큐의 처음 내용을 꺼내온다
	pMessage = front();
	pop_front();

	//큐 Count 를 감소한다.
	m_lQueueCount--;

	//이벤트를 발생 시켜서 Pop이 될때 까지  대기 중인 Thread를 깨운다.
	SetEvent(m_hEventMessagePop);

	unlock();
	return pMessage;
}

/** 
@brief		큐의 내용이 존재하는지를 체크한다.

@return		없음
@remark		
@author		김용태
@date		2006.03.08
*/
template <class T>
bool CMessageQueue<T>::m_fnIsEmpty()
{
	bool bReturn;

	//Queue 의 내용이 비었는지 대한 값을 가져 온다.
	bReturn = empty();

	//큐가 비었는지 반환한다.
	return bReturn;
}

