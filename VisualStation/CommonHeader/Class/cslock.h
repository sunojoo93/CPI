/** 
@addtogroup	UTILITY
@{
*/ 

/**
@file		cslock.h
@brief		Thread 동기화를 위한 Critical Section 을 제공한다.

@remark		
@author		김용태
@date		2006.06.21
*/

#pragma once

/** 
@brief		Critical Section 의 초기화와 임계영역 설정 및 해지 기능을 제공한다.

@remark		
@author		김용태
@date		2006
*/
class cslock
{
public:
	CRITICAL_SECTION cs;				///< 임계영역

	/** 
	@brief		생성자

	@return		없음
	@remark		임계영역 자원을 초기화 한다.
	@author		김용태
	@date		2006
	*/
	cslock()
	{ 
		//InitializeCriticalSectionAndSpinCount(&cs,4000);
		InitializeCriticalSection(&cs); 
	}

	/** 
	@brief		소멸자

	@return		없음
	@remark		임계영역 자원을 반환 한다.
	@author		김용태
	@date		2006
	*/
	~cslock()
	{ 
		DeleteCriticalSection(&cs); 
	}

	/** 
	@brief		임계영역 설정

	@return		없음
	@remark		
	@author		김용태
	@date		2006
	*/
	void lock()
	{ 
		EnterCriticalSection(&cs); 
	}

	/** 
	@brief		임계영역 해지

	@return		없음
	@remark		
	@author		김용태
	@date		2006
	*/
	void unlock()
	{ 
		LeaveCriticalSection(&cs); 
	}
};

/** 
@}
*/ 