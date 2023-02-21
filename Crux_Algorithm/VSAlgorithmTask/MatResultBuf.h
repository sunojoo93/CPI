
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

class CMatResultBuf : public CWnd
{
	DECLARE_DYNAMIC(CMatResultBuf)

public:
	CMatResultBuf();
	virtual ~CMatResultBuf();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// ��ü �޸� �Ҵ� & ����
	void				AllocMem_Result(__int64 nSize);
	void				DeleteMem_Result();
	
	// ���� �޸� ���
	cv::Mat				GetMat_Result(int rows, int cols, int type, bool bSetFlag = true, ENUM_MEMORY_HALF memory = E_MEMORY_UP);
	cv::Mat				GetMat_Result(cv::Size size, int type, bool bSetFlag = true, ENUM_MEMORY_HALF  memory = E_MEMORY_UP);

	// Sub�� ����Ҷ�...
	void				SetMem_Result(CMatResultBuf* cMatBuf);

	// ���� ����� Ȯ�ο�
	bool				GetUse_Result();
	void				SetUse_Result(bool bUse);

protected:
	// Mat Type�� �޸� ũ��
	int					GetTypeSize_Result(int type);

	// �޸� Index �ʱ�ȭ & ����
	void				MemIndexSet_Result();

protected:
	CCriticalSection	m_MemLock;				// ���� ���� ����
	BYTE*				m_Data;					// ��ü �޸�
	bool				m_bUse;					// ���� ��� Ȯ�ο�
	bool				m_bMemAlloc;			// �޸� �Ҵ��ߴ��� Ȯ�ο�
	__int64				m_nSizeIndex;			// �޸� Index
	__int64				m_nMaxSizeToTal;		// �Ҵ��� ��ü �޸�
	__int64				m_nStartIndex;			// �޸� ���� Index

	//int					m_nMemoryIndex;

	// NULL�� ���
	bool				m_bNULL;
};


