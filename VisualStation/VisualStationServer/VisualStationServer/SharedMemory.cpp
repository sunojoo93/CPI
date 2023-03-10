#include "StdAfx.h"
#include "sharedmemory.h"
#include "ImportSmpDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	CSharedMemory
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	생성 자
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
CSharedMemory::CSharedMemory(void)
{
	//Critical Section 초기화, Multi CPU 를 고려한 Spin Count 를 둔다.
	::InitializeCriticalSectionAndSpinCount(&m_csVSAccess, 2000);
}

/*
*	Module name	:	~CSharedMemory
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸 자
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
CSharedMemory::~CSharedMemory(void)
{
	//Critical Section 삭제
	::DeleteCriticalSection(&m_csVSAccess);
}

/*
*	Module name	:	m_fnVSMemoryWrite
*	Parameter	:	File no, Offset, Size, Parameter Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	공유메모리 직접 접근을 통한 기록 기능을 제공한다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CSharedMemory::m_fnVSMemoryWrite(USHORT uFileNo, USHORT uOffset, USHORT uSize, char* pszParameter)
{
	int nRet;
	int nFileNo, nOffset, nSize;

	EnterCriticalSection(&m_csVSAccess);

	nFileNo = uFileNo;
	nOffset = uOffset;
	nSize	= uSize;

	nRet = SharedMemoryWrite((int*)&nFileNo, (int*)&nOffset, (int*)&nSize, pszParameter);

	LeaveCriticalSection(&m_csVSAccess);
	return nRet;
}

/*
*	Module name	:	m_fnVSMemoryWrite
*	Parameter	:	File no, Access ID, Parameter Pointer, 접근자(마지작 접근한 Task 를 알기 위해 SMP 에서 관리되는 사항)
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	공유메모리 Access ID를 통한 기록 기능을 제공한다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CSharedMemory::m_fnVSMemoryWrite(USHORT uFileNo, char* pszAccessID, char* pszParameter, char* pszAccessor)
{
	int nRet;
	int nFileNo;

	EnterCriticalSection(&m_csVSAccess);

	nFileNo = uFileNo;
	nRet = SharedMemoryWrite(&nFileNo, pszAccessID, pszParameter, pszAccessor);

	LeaveCriticalSection(&m_csVSAccess);
	return nRet;
}

/*
*	Module name	:	m_fnVSMemoryRead
*	Parameter	:	File no, Offset, Size, Parameter Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	공유메모리 직접 접근을 통한 읽기 기능을 제공한다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CSharedMemory::m_fnVSMemoryRead(USHORT uFileNo, USHORT uOffset, USHORT uSize, char* pszParameter)
{
	int nRet;
	int nFileNo, nOffset, nSize;

	EnterCriticalSection(&m_csVSAccess);
	
	nFileNo = uFileNo;
	nOffset = uOffset;
	nSize	= uSize;

	nRet = SharedMemoryRead(&nFileNo, &nOffset, &nSize, pszParameter);

	LeaveCriticalSection(&m_csVSAccess);
	return nRet;
}

/*
*	Module name	:	m_fnVSMemoryRead
*	Parameter	:	File no, Access ID, Parameter Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1 을 제외한 나머지 값
*	Function	:	공유메모리 Access ID를 통한 읽기기능을 제공한다.
*	Create		:	2006.02.28
*	Author		:	김용태
*	Version		:	1.0
*/
int CSharedMemory::m_fnVSMemoryRead(USHORT uFileNo, char* pszAccessID, char* pszParameter,  int* nReadLength)
{
	int nRet;
	int nFileNo;

	EnterCriticalSection(&m_csVSAccess);

	nFileNo = uFileNo;

	nRet = SharedMemoryRead(&nFileNo, pszAccessID, pszParameter, nReadLength);

	LeaveCriticalSection(&m_csVSAccess);
	return nRet;
}

