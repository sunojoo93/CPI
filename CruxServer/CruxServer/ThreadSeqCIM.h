#pragma once
#include "seqtimemng.h"
#include "DlgPLCSetup.h"
#include "DataManager.h"
#include "SockManager.h"

enum
{
	CIM_RCP_EDIT_START =0,
	CIM_RCP_DN_START,
	CIM_RCP_UP_START,
	CIM_RCP_CHG_START,
	CIM_PRC_INFO_START,
	CIM_PRC_DAT_START,
	CIM_MAX,
};

enum
{
	SEQ_RCP_EDIT_CHECK = 1000,
	SEQ_RCP_EDIT_DATA_WRITE,
	SEQ_RCP_EDIT_PUB_PLUS,
	SEQ_RCP_EDIT_RECV_NO_CHECK,
	SEQ_RCP_EDIT_END,
	SEQ_RCP_EDIT_RESET,
};

enum
{
	SEQ_RCP_UP_CHECK = 2000,
	SEQ_RCP_UP_DATA_REQ,
	SEQ_RCP_UP_DATA_WRITE,
	SEQ_RCP_UP_RECV_NO_PLUS,
	SEQ_RCP_UP_END,
	SEQ_RCP_UP_RESET,
};

enum
{
	SEQ_RCP_DN_CHECK = 3000,
	SEQ_RCP_DN_DATA_READ,
	SEQ_RCP_DN_DATA_SEND,
	SEQ_RCP_DN_RECV_NO_PLUS,
	SEQ_RCP_DN_END,
	SEQ_RCP_DN_RESET,
};

enum
{
	SEQ_PROC_INFO_CHECK = 4000,
	SEQ_PROC_INFO_REQ_1,
	SEQ_PROC_INFO_RECV_1,
	SEQ_PROC_INFO_REQ_2,
	SEQ_PROC_INFO_RECV_2,
	SEQ_PROC_INFO_END,
};

enum
{
	SEQ_PROC_DATA_CHECK = 5000,
	SEQ_PROC_DATA_SEND_1,
	SEQ_PROC_DATA_RECV_1,
	SEQ_PROC_DATA_SEND_2,
	SEQ_PROC_DATA_RECV_2,
	SEQ_PROC_DATA_END,
};
enum
{
	SEQ_RCP_CHANGE_CHECK = 6000,
	SEQ_RCP_CHANGE_NO,
	SEQ_RCP_CHANGE_SEND,
	SEQ_RCP_CHANGE_RECV,
	SEQ_RCP_CHANGE_END,
	SEQ_RCP_CHANGE_RESET,
};
typedef struct stRecipeInfo
{
	long lEqCmdNo;
	long lEqData[10];

}CIM_RECIPEINFO;


class CThreadSeqCIM : public CSeqTimeMng
{
public:
	CThreadSeqCIM(void);
	~CThreadSeqCIM(void);

	CDlgPLCSetup* m_pDlgPLCSetup;
	CDataManager* m_pDataManager;
	CSockManager* m_pSockManager;

	CString m_strPanelID1;
	CString m_strPanelID2;

	int m_nRetryCount;
	int m_nRecipeCheck;
	int m_nProcCheck;

	int m_nCommandNo;
	int m_nAckCode;
	int m_nRecipeNo;

	int m_nRecvNoRecipeUP;
	int m_nPubNoRecipeUP;
	int m_nRecvNoRecipeEdit;
	int m_nPubNoRecipeEdit;
	int m_nRecvNoRecipeDown;
	int m_nPubNoRecipeDown;

	int m_nFrontSide;
	int m_nFrontCoaxial;
	int m_nRearSide;
	int m_nRearCoaxial;
	int m_nTrans1;
	int m_nTrans2;
	int m_nPADPosition;
	int m_nROICount;

	int m_fnInitThread(int nEqpType, int nSeqCnt, CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CSockManager* pSockManager);
	void m_fnDeInitThread();
	void m_fnResetSEQ(BOOL bDir = TRUE, BOOL bFlag = FALSE, BOOL bErrReset = FALSE);

	void m_fnCheck();
	void m_fnRecipeEditSeq();
	void m_fnRecipeUpSeq();
	void m_fnRecipeDnSeq();
	void m_fnRecipeChangeSeq();
	void m_fnProcessInfoSeq();
	void m_fnRetryCheck(CString strMsg, int nTimeout);

private:
	CString m_strPlcVal;
	long m_lPlcVal;
	CIM_RECIPEINFO *stCimRcpInfo;
	int m_nEqpModel;
	int m_nEqpSizeType;
	int m_nUsingFlag;
	int m_nCimSeqCnt;
	/*	int m_nCntRecipeDn;
	int m_nCntRecipeUp;
	int m_nCntRecipeChg*/;

private:
	int m_fnSetCimStep(int nStepVal);
	int m_fnSetCimCheckList(CString strDefJudge);
	//int m_fnGetRcpDnStep();
	//int m_fnGetRcpUpStep();	
	//int m_fnGetRcpChgStep();
	//void m_fnSetRcpDnStep(int nRcpCnt);	
	//void m_fnSetRcpUpStep(int nRcpCnt);	
	//void m_fnSetRcpChgStep(int nRcpCnt);
	
};

