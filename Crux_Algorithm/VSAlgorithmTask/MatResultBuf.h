
/************************************************************************/
// Mat 메모리 관련 헤더
// 수정일 : 17.08.02 - KSW
/************************************************************************/

#pragma once

#include "afxmt.h"
#include "Define.h"

// 메모리 반 나누기 : 상위, 하위 사용
enum ENUM_MEMORY_HALF
{
	E_MEMORY_UP		= 0,
	E_MEMORY_DOWN
};

// CMatBuf

class CMatResultBuf : public CWnd
{
	DECLARE_DYNAMIC(CMatResultBuf)

public:
	CMatResultBuf();
	virtual ~CMatResultBuf();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// 전체 메모리 할당 & 해제
	void				AllocMem_Result(__int64 nSize);
	void				DeleteMem_Result();
	
	// 고정 메모리 사용
	cv::Mat				GetMat_Result(int rows, int cols, int type, bool bSetFlag = true, ENUM_MEMORY_HALF memory = E_MEMORY_UP);
	cv::Mat				GetMat_Result(cv::Size size, int type, bool bSetFlag = true, ENUM_MEMORY_HALF  memory = E_MEMORY_UP);

	// Sub로 사용할때...
	void				SetMem_Result(CMatResultBuf* cMatBuf);

	// 현재 사용중 확인용
	bool				GetUse_Result();
	void				SetUse_Result(bool bUse);

protected:
	// Mat Type별 메모리 크기
	int					GetTypeSize_Result(int type);

	// 메모리 Index 초기화 & 설정
	void				MemIndexSet_Result();

protected:
	CCriticalSection	m_MemLock;				// 동시 접근 방지
	BYTE*				m_Data;					// 전체 메모리
	bool				m_bUse;					// 현재 사용 확인용
	bool				m_bMemAlloc;			// 메모리 할당했는지 확인용
	__int64				m_nSizeIndex;			// 메모리 Index
	__int64				m_nMaxSizeToTal;		// 할당한 전체 메모리
	__int64				m_nStartIndex;			// 메모리 시작 Index

	//int					m_nMemoryIndex;

	// NULL인 경우
	bool				m_bNULL;
};


