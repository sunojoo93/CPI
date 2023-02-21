
/************************************************************************/
// Mat �޸� ���� ���
// ������ : 17.08.02 - KSW
/************************************************************************/

#pragma once

#include "afxmt.h"
#include "Define.h"


// �޸� �� ������ : ����, ���� ���
enum ENUM_MEMORY_HALF
{
	E_MEMORY_UP		= 0,
	E_MEMORY_DOWN
};

// CMatBuf

class CMatBuf : public CWnd
{
	DECLARE_DYNAMIC(CMatBuf)

public:
	CMatBuf();
	virtual ~CMatBuf();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// ��ü �޸� �Ҵ� & ����
	void				AllocMem(__int64 nSize);
	void				DeleteMem();
	
	// ���� �޸� ���
	cv::Mat				GetMat(int rows, int cols, int type, bool bSetFlag = true, ENUM_MEMORY_HALF memory = E_MEMORY_UP);
	cv::Mat				GetMat(cv::Size size, int type, bool bSetFlag = true, ENUM_MEMORY_HALF  memory = E_MEMORY_UP);

	// Sub�� ����Ҷ�...
	void				SetMem(CMatBuf* cMatBuf);

	// ���� ����� Ȯ�ο�
	bool				GetUse();
	void				SetUse(bool bUse);
	__int64				Get_AutoMemory();
	__int64				Get_FixMemory();

protected:
	// Mat Type�� �޸� ũ��
	int					GetTypeSize(int type);

	// �޸� Index �ʱ�ȭ & ����
	void				MemIndexSet();

protected:
	CCriticalSection	m_MemLock;				// ���� ���� ����
	BYTE*				m_Data;					// ��ü �޸�
	bool				m_bUse;					// ���� ��� Ȯ�ο�
	bool				m_bMemAlloc;			// �޸� �Ҵ��ߴ��� Ȯ�ο�
	__int64				m_nSizeIndex;			// �޸� Index
	__int64				m_nMaxSizeToTal;		// �Ҵ��� ��ü �޸�
	__int64				m_nStartIndex;			// �޸� ���� Index

	__int64				m_nMem_Fix;				// ���� �޸� ��뷮
	__int64				m_nMem_Auto;			// �ڵ��Ҵ� �޸� ��뷮
	//int					m_nMemoryIndex;

	// NULL�� ���
	bool				m_bNULL;
};


