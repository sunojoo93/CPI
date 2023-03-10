#pragma once


#include <deque>										//STL deque 를 사용하기 위함

/*
*	Module name	:	CItemList
*	Function	:	Item 리스트를 가진다. 기본적인 큐의 기능을 제공한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
class CItemList : public std::deque<T>
{
public:
	CItemList();					
	~CItemList(void);

	/*멤버 함수*/
public:
	int		m_fnPushItem(T pItem);					//메시지 큐에 메시지를 삽입 한다.
	T		m_fnPopItem();							//메시지 큐의 첫번째 내용을 꺼낸다.
	bool	m_fnIsEmpty();							//메시지 큐가 비었는지 확인 한다.
	int		m_fnDeleteItem(T pItem);				//메시지 큐안의 내용중 파라메터와 일치하는 것을 삭제한다.
	bool	m_fnIsItem(T pItem);					//메시지 큐안에 파라메터와 일치하는 내용이 존재하는지 체크한다.
private:
	int		m_fnResetItem();						//메시지 큐의 내용을 삭제한다.

	/*멤버 변수*/
private:
	CRITICAL_SECTION	m_csItemQueue;				//메시지 큐 접근을 위한 Critical Section
};


/*
*	Module name	:	CItemList
*	Parameter	:	큐 기능 구분자
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	생성자
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
CItemList<T>::CItemList() : std::deque<T>()
{
	//Critical Section 을 초기화 한다. Multi CPU 를 고려한 Spin Count 를 둔다.
	InitializeCriticalSectionAndSpinCount(&m_csItemQueue, 2000);
}

/*
*	Module name	:	~CItemList
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	소멸자
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
CItemList<T>::~CItemList(void)
{
	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	//메시지큐의 내용을 삭제한다.
	m_fnResetItem();
	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

	//Critical Section 을 종료한다.
	DeleteCriticalSection(&m_csItemQueue);
}

/*
*	Module name	:	m_fnPushItem
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐에 메시지를 삽입 한다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
int CItemList<T>::m_fnPushItem(T pItem)
{
	int		nErrorCode = OKAY;

	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

	//Item 를 큐에 삽입한다.
	push_back(pItem);

	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

	return nErrorCode;
}

/*
*	Module name	:	m_fnPopItem
*	Parameter	:	없음
*	Return		:	정상 종료 : 메시지의 시작 Pointer
*					이상 종료 : NULL
*	Function	:	메시지 큐의 첫번째 내용을 꺼낸다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
T CItemList<T>::m_fnPopItem()
{
	T	pItem;

	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

	//큐가 비었다면 NULL 반환 한다.
	if(empty())
	{
		LeaveCriticalSection(&m_csItemQueue);		//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
		return NULL;
	}

	//큐의 처음 내용을 꺼내온다
	pItem = front();
	pop_front();

	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

	return pItem;
}

/*
*	Module name	:	m_fnDeleteItem
*	Parameter	:	Item Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐안의 내용중 파라메터와 일치하는 것을 삭제한다.
*	Create		:	2006.03.17
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
int CItemList<T>::m_fnDeleteItem(T pItem)
{
	int nErrorCode = NG;
	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽
	//큐안의 내용중 처음 내용을 가져온다.
	iterator it = begin();

	//반복한다. 큐 내용이 끝이 아닐때까지
	for(; it != end(); it++)
	{
		T lpItemData = *it;
		if(lpItemData == pItem)
		{
			SAFE_DELETE(lpItemData);
			erase(it);
			nErrorCode = OKAY;
			break;
		}
	}
	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
	return nErrorCode;
}

/*
*	Module name	:	m_fnResetItem
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	메시지 큐의 전체 내용을 지운다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
int CItemList<T>::m_fnResetItem()
{
	int				nErrorCode = OKAY;
	T				pItem = NULL;

	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

	//구간 반복 한다. Queue 가 empty 일때까지
	while(!empty())
	{
		//Queue 의 내용을 하나 가져온다.
		pItem = front();
		pop_front();

		//가져온 Queue 내용을 지운다.
		SAFE_DELETE(pItem);
	}

	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

	return nErrorCode;
}

/*
*	Module name	:	m_fnIsEmpty
*	Parameter	:	없음
*	Return		:	큐 내용이 존재하면 : true
*					큐 내용이 존재하지 않으면 : false
*	Function	:	메시지 큐가 비었는지 반환한다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
bool CItemList<T>::m_fnIsEmpty()
{
	bool bReturn;

	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

	//Queue 의 내용이 비었는지 대한 값을 가져 온다.
	bReturn = empty();

	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△
	//큐가 비었는지 반환한다.
	return bReturn;
}

/*
*	Module name	:	m_fnIsItem
*	Parameter	:	Item Pointer
*	Return		:	큐 내용이 존재하면 : true
*					큐 내용이 존재하지 않으면 : false
*	Function	:	메시지 큐의 내용중 Parameter 의 Item 이 존재하는지 찾는다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
template <class T>
bool CItemList<T>::m_fnIsItem(T pItem)
{
	bool bReturn = false;

	EnterCriticalSection(&m_csItemQueue);			//▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽

	//큐안의 내용중 처음 내용을 가져온다.
	iterator it = begin();

	//반복한다. 큐 내용이 끝이 아닐때까지
	for(; it != end(); it++)
	{
		T lpItemData = *it;
		//만일 파라메터와 같은 내용이 존재한다면 true 를 반환한다.
		if(lpItemData == pItem)
		{
			bReturn = true;
			break;
		}
	}

	LeaveCriticalSection(&m_csItemQueue);			//△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△△

	return bReturn;
}
