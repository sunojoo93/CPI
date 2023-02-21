#pragma once
#include "seqtimemng.h"
#include "DlgPLCSetup.h"
#include "DataManager.h"
#include "SockManager.h"
#include "ServerWorkManager.h"
enum
{
	SEQ_FORWARD_START = 1000,
	SEQ_F_PLC_ALIVE_CHECK,
	SEQ_F_PLC_FORWARD_CHECK,
	SEQ_F_VISION_STATE_REQ,
	SEQ_F_VISION_STATE_CHECK,
	SEQ_F_GRAB_READY,
	SEQ_F_GRAB_REQ_CHECK,
	SEQ_F_GRAB_READY_REQ,
	SEQ_F_GRAB_READY_RES,
	SEQ_F_GRAB_ACK,
	SEQ_F_GRAB_START,
	SEQ_F_GRAB_END_CHECK,
	SEQ_F_INSPECT_END_CHECK,
	SEQ_F_RESULT_PROCESS,
	SEQ_F_RESULT_END_CHECK,
	SEQ_FORWARD_END,
	SEQ_RESET
};

enum
{
	SEQ_BACKWARD_START = 2000,
	SEQ_B_PLC_ALIVE_CHECK,
	SEQ_B_PLC_BACKWARD_CHECK,
	SEQ_B_VISION_STATE_REQ,
	SEQ_B_VISION_STATE_CHECK,
	SEQ_B_GRAB_READY,
	SEQ_B_GRAB_REQ_CHECK,
	SEQ_B_GRAB_READY_REQ,
	SEQ_B_GRAB_READY_RES,
	SEQ_B_GRAB_ACK,
	SEQ_B_GRAB_START,
	SEQ_B_GRAB_END_CHECK,
	SEQ_B_INSPECT_END_CHECK,
	SEQ_B_RESULT_PROCESS,
	SEQ_BACKWARD_END
};

class CThreadSeqPLC : public CSeqTimeMng
{
public:
	CThreadSeqPLC(void);
	~CThreadSeqPLC(void);

	CDlgPLCSetup* m_pDlgPLCSetup;
	CDataManager* m_pDataManager;
	CSockManager* m_pSockManager;
	CServerWorkManager* m_pServerWoarkManager;

	BOOL m_bForwardScan;
	int m_nStageID;
	int m_nCheckAlive;
	int m_nPCAlive;
	int m_nEqpType;
	int m_nPanelID1;
	int m_nPanelID2;
	
	int m_nRetryCount;
	int m_nRetryClassifyCnt;

	CString m_strPanelID_1;
	CString m_strPanelID_2;

	int m_fnInitThread(int nEqpType, int nStageID, CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CSockManager* pSockManager, CServerWorkManager* m_pVsSeverIF);
	void m_fnDeInitThread();
	void m_fnResetSEQ(BOOL bDir = TRUE, BOOL bFlag = FALSE, BOOL bErrReset = FALSE);

	void m_fnCheck();
	void m_fnScanSeqLarge();
	void m_fnScanSeqForward();
	void m_fnScanSeqBackward();
	void m_fnScanSeqForward_M();
	void m_fnScanSeqBackward_M();
	void m_fnScanSeqPacking();
	void m_fnCheckAlivePLC();
	BOOL m_fnRetryCheck(CString strMsg, int nTimeout);

	//2015.05.13 add
	BOOL m_fnVisionReadyResCheck();
	BOOL m_fnVisionReadyResModelMismatchCheck();
	BOOL m_fnVisionReadyResTriggerSetErrorCheck();
	BOOL m_fnVisionGrabEndFlagCheck();
	BOOL m_fnVisionGrabEndResCheck();
	void m_fnCheckModelId(CString strModelID);  // 2015.10.15 add by ikm Packing ¿ë ÀÚµ¿ ·¹½ÃÇÇ º¯°æ
};

