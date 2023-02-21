/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		SafeDeque.h
@brief		STL Deque를 사용하여 Thread Safe 기능 및 Smart Pointer 기능을 가지는 CSafeDeque 를 제공한다.

@remark		
-			Critical Section을 사용하여 Deque 자원에 대한 Thread 동기화를 구현하였다.
-			Smart Pointer를 사용하여 Deque 에 삽입되는 동적 생성 객체에 대해 Memory Leak 을 해결 하였다.
@author		김용태
@date		2006
*/

#pragma once

#include <deque>
#include <algorithm>

#include "cslock.h"
#include "SmartPointer.h"


/** 
@brief		STL Deque를 사용하여 Thread Safe 기능 및 Smart Pointer 기능을 가지는 CSafeDeque 를 제공한다.

@remark		STL deque 와 cslock Class 를 상속 받는다.
@author		김용태
@date		2006
*/
template<class T >
class CSafeDeque : public std::deque< _sp<T> >, private cslock
{
public:
	/** 
	@brief		Deque 에 Template 객체를 넣는다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006
	*/
	void m_fnPushBack(const _sp<T>& x)
	{ 
		lock(); 

		push_back(x); 

		unlock(); 
	}

	/** 
	@brief		Deque 에서 처음 Push 되었던 Template 객체를 가져온다.

	@return		Smart Pointer Reference
	@remark		
	@author		김용태
	@date		2006
	*/
	_sp<T>& m_fnPopFront()
	{ 
		_sp<T>&	spMessage;

		lock();

		//데크가 비었다면 NULL 로 초기화된 Smart Pointer 반환 한다.
		if(empty())
		{
			static _sp<T> spNull;
			unlock();
			return spNull;
		}

		//데크의 처음 내용을 꺼내온다
		spMessage = front();
		pop_front();

		unlock(); 
		return spMessage;
	}

	/** 
	@brief		인자로 받은 객체와 일치하는 Deque 내의 값을 찾아서 지운다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006
	*/
	void m_fnErase(const _sp<T>& x)
	{
		lock(); 

		iterator it = std::find(begin(), end(), x);

		if(it != end()) 
		{
			erase(it);
		}

		unlock();
	}  

	/** 
	@brief		인자로 받은 객체와 일치하는 Deque 내의 값을 찾아서 지운다.

	@return		없음
	@remark		
	@author		김용태
	@date		2006
	*/
	void m_fnErase(const T* x)
	{
		lock(); 

		for(iterator it = begin(); it != end(); ++(it))
		{
			if((*it).get() == x)
			{ 
				erase( it ); 
				break;
			}
		}

		unlock(); 
	}

	/** 
	@brief		인자로 받은 객체와 일치하는 Deque 내의 값을 찾아서 반환한다.

	@return		Smart Pointer Reference
	@remark		
	@author		김용태
	@date		2006
	*/
	template <class M> 
	_sp<T>& m_fnFind(bool (*compare)( T* x, M m), M _m)
	{
		lock(); 

		for(iterator it = begin(); it != end(); ++(it))
		{
			if(compare((*it).get(), _m))
			{
				unlock();
				return (*it);
			}
		}

		unlock(); 

		static _sp<T> spNull;

		return spNull;
	}

};

/** 
@}
*/ 