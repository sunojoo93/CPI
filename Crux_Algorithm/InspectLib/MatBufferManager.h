
/************************************************************************/
// Mat 메모리 관리 관련 헤더
// 수정일 : 17.08.02 - KSW
/************************************************************************/

#pragma once

#include "afxmt.h"
#include "Define.h"
#include "MatBuf.h"


// CMatBufferManager

class CMatBufferManager : public CWnd
{
	DECLARE_DYNAMIC(CMatBufferManager)

public:
	CMatBufferManager();
	virtual ~CMatBufferManager();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CMatBuf*			FindFreeBuf();
	void				ReleaseFreeBuf(CMatBuf* data);

	void				FindFreeBuf_Multi(const int nCount, CMatBuf **cMatBuf_Multi);
	void				ReleaseFreeBuf_Multi(const int nCount, CMatBuf** data);

	void				AllocMem(__int64 nSizeMB);
	void				DeleteMem();
	void				SetLodeData();	// 190218 YWS
	CString				GETDRV();

	////////////////////////////////////////////////////////////////////////// Low

	CMatBuf*			FindFreeBuf_Low();
	void				ReleaseFreeBuf_Low(CMatBuf* data);

	void				FindFreeBuf_Multi_Low(const int nCount, CMatBuf **cMatBuf_Multi);
	void				ReleaseFreeBuf_Multi_Low(const int nCount, CMatBuf** data);

	void				AllocMem_Low(__int64 nSizeMB);
	void				DeleteMem_Low();
	void				SetLodeData_Low();	// 190218 YWS

	////////////////////////////////////////////////////////////////////////// High

	CMatBuf*			FindFreeBuf_High();
	void				ReleaseFreeBuf_High(CMatBuf* data);

	void				FindFreeBuf_Multi_High(const int nCount, CMatBuf **cMatBuf_Multi);
	void				ReleaseFreeBuf_Multi_High(const int nCount, CMatBuf** data);

	void				AllocMem_High(__int64 nSizeMB);
	void				DeleteMem_High();
	void				SetLodeData_High();	// 190218 YWS
	
protected:
	CCriticalSection	m_MemManager;
	CCriticalSection	m_MemManager_Low;
	CCriticalSection	m_MemManager_High;

	CMatBuf				*m_Data;
	CMatBuf				*m_Data_Low;
	CMatBuf				*m_Data_High;

	// 190218 YWS
	int m_nMax_Men_Count_ini = 0;
	int m_nMaxAllocCount = 0;
	int m_nMenAllocCount = 0;

	//////////////////////////////////////////////////////////////////////////Low

	int m_nMax_Men_Count_ini_Low = 0;
	int m_nMaxAllocCount_Low = 0;
	int m_nMenAllocCount_Low = 0;

	//////////////////////////////////////////////////////////////////////////High
	int m_nMax_Men_Count_ini_High = 0;
	int m_nMaxAllocCount_High = 0;
	int m_nMenAllocCount_High = 0;
};


