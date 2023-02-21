#pragma once
#include "seqtimemng.h"
#include "DlgPLCSetup.h"
#include "DataManager.h"
#include "ServerWorkManager.h"

//2015.08.19 
enum
{
	SEQ_STEP_PLC_ALIGN_REQ_CHECK = 1000,
	SEQ_STEP_PC_ALIGN_REQ,
	SEQ_STEP_PC_ALIGN_RESULT,
	SEQ_STEP_ALIGN_END,

};

class CThreadProcAlign : public CSeqTimeMng
{
private:

public:
	CThreadProcAlign();
	virtual ~CThreadProcAlign();

	CDlgPLCSetup* m_pDlgPLCSetup;
	CDataManager* m_pDataManager;
	CServerWorkManager* m_pServerWoarkManager;
	int m_nRetryCount;
	CString m_strPanelID1;

	int m_fnInitThread(CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CServerWorkManager* m_pVsSeverIF);
	void m_fnDeInitThread();
	void m_fnResetSEQ(BOOL bDir = TRUE, BOOL bFlag = FALSE, BOOL bErrReset = FALSE);
	void m_fnCheck();
	void m_fnRetryCheck(CString strMsg, int nTimeout);
	void m_fnAlignProc();
};


