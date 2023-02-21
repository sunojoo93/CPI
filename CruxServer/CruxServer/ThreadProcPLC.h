#pragma once
#include "seqtimemng.h"
#include "DlgPLCSetup.h"
#include "DataManager.h"
#include "SockManager.h"

enum
{	
	SEQ_PROC_PLC_GRAB_RETRY_STG1 = 1,
	SEQ_PROC_PLC_GRAB_RETRY_STG2,
	SEQ_PROC_PLC_GRAB_RETRY_STG3,
	SEQ_PROC_PLC_FRONT_NG_FLAG,
	SEQ_PROC_PLC_RCP_CHECK,
	SEQ_PROC_PLC_INIT_RESET,
	SEQ_PROC_PLC_SIG_CHECK,
	SEQ_PROC_PLC_SAV_MODID,
	SEQ_PROC_PLC_REAR_NG_FLAG,
	SEQ_PROC_PLC_STOP,
	SEQ_PROC_PLC_ALARM_RESET,
	SEQ_PROC_PLC_MAX,
};

enum
{
	SEQ_STEP_FRONT_NG_FLAG_CHECK = SEQ_PROC_PLC_FRONT_NG_FLAG * 1000,
	SEQ_STEP_FRONT_GET_PANEL_COUNT_ONE,
	SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE,
	SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO,
	SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO,
	SEQ_STEP_FRONT_END,
};

enum
{
	SEQ_STEP_REAR_NG_FLAG_CHECK = SEQ_PROC_PLC_REAR_NG_FLAG * 1000,
	SEQ_STEP_REAR_GET_PANEL_COUNT_ONE,
	SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE,
	SEQ_STEP_REAR_GET_PANEL_COUNT_TWO,
	SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO,
	SEQ_STEP_REAR_END,
};

enum
{
	SEQ_STEP_GRAB_RETRY_INIT_REQ_CHECK = SEQ_PROC_PLC_GRAB_RETRY_STG1 * 1000,
	SEQ_STEP_GRAB_RETRY_PC_COMPLETE_ON,
	SEQ_STEP_GRAB_RETRY_PLC_INIT_REQ_OFF_CHK,
	SEQ_STEP_GRAB_RETRY_PC_COMPLETE_OFF,
	SEQ_STEP_GRAB_RETRY_END,
};
enum
{
	SEQ_STEP_PLC_RCP_CHECK = SEQ_PROC_PLC_RCP_CHECK * 1000,	
	EQ_STEP_PLC_RCP_END,
};
enum
{
	SEQ_STEP_PLC_INIT_RESET = SEQ_PROC_PLC_INIT_RESET * 1000,
	SEQ_STEP_PLC_INIT_RESET_END,
};
enum
{
	SEQ_STEP_PLC_SIG_CHECK = SEQ_PROC_PLC_SIG_CHECK * 1000,	
	SEQ_STEP_PLC_SIG_END,
};
enum
{
	SEQ_STEP_PLC_SAV_MODID = SEQ_PROC_PLC_SAV_MODID * 1000,
	SEQ_STEP_PLC_SAV_MODID_END,
};

enum
{
	SEQ_STEP_PLC_ALARM_RESET = SEQ_PROC_PLC_ALARM_RESET * 1000,
	SEQ_STEP_PLC_ALARM_RESET_END,
};


#define EQP_TYPE_MEDIUM		0
#define EQP_TYPE_SMALL		1

// CThreadProcPLC
class CThreadProcPLC : public CSeqTimeMng
{
private:
	int m_nSeqCnt;
	int		m_nRetryClassifyCnt;
	HANDLE	m_hThreadSaveModelId;		
public:
	CThreadProcPLC();
	virtual ~CThreadProcPLC();

	CDlgPLCSetup*		m_pDlgPLCSetup;
	CDataManager*		m_pDataManager;
	CThreadProcCIM *	m_pTreadCimProc;

	int m_nUsingFlag;
	int m_nUsingPlcAlarmResetFlag;	//Chung Du
	int m_nNgFlagCheck;
	int m_nRetryCount;
	CString m_strPanelID1;
	CString m_strPanelID2;
	BOOL m_bULDStage_R;
	BOOL m_bULDStage_F;

	int m_fnInitThread(int nSeqCnt, CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager);
	void m_fnDeInitThread();
	void m_fnResetSEQ(BOOL bDir = TRUE, BOOL bFlag = FALSE, BOOL bErrReset = FALSE);
	void m_fnCheck();
	BOOL m_fnRetryCheck(CString strMsg, int nTimeout);
	void m_fnPlcInitRest();
	void m_fnProcCheckFrontNgFlag();
	void m_fnProcCheckRearNgFlag();
	void m_fnSignalCheck();
	void m_fnPlcRecipeUpdateCheck();	//2015.11.05
	void m_fnPlcSaveModelID();
	static unsigned __stdcall	m_fnThreadSaveModel(LPVOID pArg);	 // 2015.10.15 add by ikm Packing ¿ë ÀÚµ¿ ·¹½ÃÇÇ º¯°æ	
	void m_fnWriteModelId(CString strModelID);	 // 2015.10.15 add by ikm Packing ¿ë ÀÚµ¿ ·¹½ÃÇÇ º¯°æ	
	void m_fnGrabRetryCheck();					 // 2015.12.31 add by ikm Grab Retry Cehck
	void m_fnPlcAlarmReset();	//Cheng Du
	int  m_fnNgFlagInfoCheck(int Ngflag);
};


