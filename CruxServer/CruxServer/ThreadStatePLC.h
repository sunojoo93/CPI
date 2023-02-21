#pragma once
#include "seqtimemng.h"
#include "DlgPLCSetup.h"

class CThreadStatePLC :
	public CSeqTimeMng
{
public:
	CThreadStatePLC(void);
	~CThreadStatePLC(void);

	CDlgPLCSetup* m_pDlgPLCSetup;
	BOOL m_bReadPLC;
	int m_nEqpType;
	int m_nEqpSize;

	//2015.10.30 Mr.Chu
	//int m_fnInitThread(int nEqpType, CDlgPLCSetup* pDlgPLCSetup);
	int m_fnInitThread(int nEqpSize, int nEqpType, CDlgPLCSetup* pDlgPLCSetup);
	void m_fnDeInitThread();

	void m_fnCheck();
	void m_fnReadPCIO();
	void m_fnReadPLCIO();
};

