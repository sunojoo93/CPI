#pragma once

/*
*	Module name	:	CSharedMemory
*	Function	:	공유 메모리 접근 기능을 제공한다.
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.0
*/
class CSharedMemory
{
public:
	CSharedMemory(void);
	~CSharedMemory(void);

	/*멤버 함수*/
public:
										//직접 메모리 접근 기록
	int		m_fnVSMemoryWrite(USHORT uFileNo, USHORT uOffset, USHORT uSize, char* pszParameter);
										//Access ID 를 통한 메모리 접근 기록
	int		m_fnVSMemoryWrite(USHORT uFileNo, char* pszAccessID, char* pszParameter, char* pszAccessor);

										//직접 메모리 접근 읽기
	int		m_fnVSMemoryRead(USHORT uFileNo, USHORT uOffset, USHORT uSize, char* pszParameter);
										//Access ID 를 통한 메모리 접근 읽기
	int		m_fnVSMemoryRead(USHORT uFileNo, char* pszAccessID, char* pszParameter, int* nReadLength);
	
	/*멤버 변수*/
private:
	CRITICAL_SECTION	m_csVSAccess;					//공유메모리 관리자에 접근하기 위한 Critical Section
};
