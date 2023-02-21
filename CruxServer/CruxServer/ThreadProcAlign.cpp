
#include "stdafx.h"
#include "CruxServer.h"
#include "ThreadProcAlign.h"
#include "MainFrm.h"
#include "ViewAuto.h"


// CThreadProcAlign

CThreadProcAlign::CThreadProcAlign(void)
{
	m_nRetryCount = 0;
}

CThreadProcAlign::~CThreadProcAlign(void)
{

}

int CThreadProcAlign::m_fnInitThread(CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CServerWorkManager* m_pVsSeverIF )
{	
	m_pDlgPLCSetup = pDlgPLCSetup;
	m_pDataManager = pDataManager;
	m_pServerWoarkManager = m_pVsSeverIF;
	m_fnResetSEQ(TRUE, TRUE);
	return CSequenceManager::m_fnStartScenario(TRUE);
}

void CThreadProcAlign::m_fnDeInitThread()
{
	CSequenceManager::m_fnDeInit();
}

void CThreadProcAlign::m_fnResetSEQ(BOOL bDir, BOOL bFlag, BOOL bErrReset)
{	
	m_fnSetStep(SEQ_STEP_PLC_ALIGN_REQ_CHECK);
}

void CThreadProcAlign::m_fnCheck()
{
	m_fnAlignProc();
}
void CThreadProcAlign::m_fnRetryCheck(CString strMsg, int nTimeout)
{
	long lRet = -1;
	m_nRetryCount++;

	if(m_nRetryCount * SLEEP_COUNT < nTimeout * 1000)
		m_fnRetryStep();
	else
	{
		m_nRetryCount = 0;
		G_AddLog_SEQ(0, _T("Align I/F Timeout Error %s."), strMsg);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_ALIGN_COMPLETE_OK, TRUE);	//2015.10.10 Req By Lim D.H
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_ALIGN_COMPLETE_NG, TRUE);
		m_fnResetSEQ();
	}
}

//2015.08.19
void CThreadProcAlign::m_fnAlignProc()
{
	long lRet = -1, lRet_Req_Align = -1;
	int nEqpSizeType = 0, nEqpModel = 0;
	CString strConfigFile;
	STRU_ALIGN_INFO stAlignInfo;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());	
	CString strAlignInfoPath;
	strAlignInfoPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_INFO_PATH);
	strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile); //0:Middle 1:Small 2:Large
	nEqpModel = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0, strConfigFile);	//0:AVL 1:ABI
	
	switch(m_fnGetStep())
	{
	case SEQ_STEP_PLC_ALIGN_REQ_CHECK: 

		//m_pServerWoarkManager->Seq_AlignProcChk(); // 얼라인 상태 표시

		lRet_Req_Align = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_ALIGN_REQUEST);

		if (m_pServerWoarkManager->m_stSockStatus.bPreAlign == TRUE && lRet_Req_Align == OK)
		{
			m_fnSetStep(SEQ_STEP_PC_ALIGN_REQ);
			//2021.06.08 add by mby 
			if(pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
				G_AddLog_SEQ(VS_ALIGN_TASK, _T("[StageID = %d] 1_SEQ_STEP_PLC_ALIGN_REQ_CHECK."), VS_ALIGN_TASK);
			}else{
				G_AddLog_SEQ(VS_ALIGN_TASK_TWO, _T("[StageID = %d] 1_SEQ_STEP_PLC_ALIGN_REQ_CHECK."), VS_ALIGN_TASK_TWO);
			}
		}
		else
			m_fnSetStep(SEQ_STEP_ALIGN_END);

		break;

	case SEQ_STEP_PC_ALIGN_REQ:
		//send message to align pc
		//G_AddLog_SEQ(49, _T("[StageID = %d] 2_SEQ_STEP_PC_ALIGN_REQ."), 49);
		m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_PANEL_ID, LEN_PANEL_ID, m_strPanelID1);
		m_strPanelID1.TrimRight();
		//2021.06.08 add by mby 
		if(pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
			G_AddLog_SEQ(VS_ALIGN_TASK, _T("[StageID = %d] 2_SEQ_STEP_PC_ALIGN_REQ. Panel : %s"), VS_ALIGN_TASK, m_strPanelID1);
		}else{
			G_AddLog_SEQ(VS_ALIGN_TASK_TWO, _T("[StageID = %d] 2_SEQ_STEP_PC_ALIGN_REQ. Panel : %s"), VS_ALIGN_TASK_TWO, m_strPanelID1);
		}
		//m_pVsSeverIF->m_fnRequestAlign(PRE_ALIGN_PC, m_strPanelID1);
		Sleep(200);
		m_pServerWoarkManager->m_fnRequestAlign( m_strPanelID1);
		m_fnSetStep(SEQ_STEP_PC_ALIGN_RESULT);
		break;

	case SEQ_STEP_PC_ALIGN_RESULT:
		//receive message from align pc
		m_strPanelID1.TrimRight();
		stAlignInfo = m_pDataManager->m_fnReadAlignData(m_strPanelID1);

		if (stAlignInfo.bAlignComplete)
		{
			//2021.06.08 add by mby 
			if(pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
				G_AddLog_SEQ(VS_ALIGN_TASK, _T("[StageID = %d] 3_SEQ_STEP_PC_ALIGN_RESULT."), VS_ALIGN_TASK);
			}else{
				G_AddLog_SEQ(VS_ALIGN_TASK_TWO, _T("[StageID = %d] 3_SEQ_STEP_PC_ALIGN_RESULT."), VS_ALIGN_TASK_TWO);
			}
			if (stAlignInfo.bAlignResult == TRUE)
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_ALIGN_COMPLETE_OK, TRUE);
			else
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_ALIGN_COMPLETE_OK, TRUE);	//ADDR_PC_ALIGN_COMPLETE_NG 를 보내면 PLC에서 Retry 기능을 수행함. PLC에서 Retry 기능 완성 X //2015.11.30 Mr.Chu

			m_fnSetStep(SEQ_STEP_ALIGN_END);
		}
		else
		{
			m_fnRetryCheck(_T("SEQ_STEP_PC_ALIGN_RESULT"), 3);
		}

		break;

	case SEQ_STEP_ALIGN_END:
		m_fnSetStep(SEQ_STEP_PLC_ALIGN_REQ_CHECK);
		break;
	default:
		break;
	}
}



