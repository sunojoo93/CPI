
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "StdAfx.h"
#include "ThreadSeqPLC.h"
#include "MainFrm.h"


CThreadSeqPLC::CThreadSeqPLC(void)
{
	m_nEqpType = 0;
	m_bForwardScan = TRUE;
	m_nCheckAlive = 0;
	m_nPCAlive = 0;
	m_nRetryCount = 0;
	m_nPanelID1 = 0;
	m_nPanelID2 = 0;
	m_nRetryClassifyCnt = 0;
}

CThreadSeqPLC::~CThreadSeqPLC(void)
{

}

int CThreadSeqPLC::m_fnInitThread(int nEqpType, int nStageID, CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CSockManager* pSockManager, CServerWorkManager* m_pVsSeverIF )
{	
	m_nEqpType = nEqpType;
	m_nStageID = nStageID;

	m_pDlgPLCSetup = pDlgPLCSetup;
	m_pDataManager = pDataManager;
	//m_pServerWoarkManager = pSockManager;
	m_pServerWoarkManager = m_pVsSeverIF;
#if 1
	m_fnResetSEQ(TRUE, TRUE);
	m_bForwardScan = TRUE;
#endif

	return CSequenceManager::m_fnStartScenario(TRUE);
}

void CThreadSeqPLC::m_fnDeInitThread()
{
	CSequenceManager::m_fnDeInit();
}

void CThreadSeqPLC::m_fnResetSEQ(BOOL bDir, BOOL bFlag, BOOL bErrReset)
{	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	long lRet = -1;
	//2015.05.13 edit s-
	//m_pServerWoarkManager->m_fnResetFlag(m_nStageID + 1);
	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					m_pServerWoarkManager->m_fnResetFlag(j + 1);					
				}
			}
		}
	}
	//2015.05.13 edit e-


#if PLC_TASK_MODE
#else
	if (pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
	{
		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID == 0)	//2015.05.13 edit
		{
			pFrame->m_fnReportPCAlive();		//2015.10.31 Mr.Chu
			//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 0); //2015.10.31 Comment
			
			//Cheng Du Comment
			//if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 0);

			if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE || EQP_MONITOR_SIZE)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
			}
			if(bErrReset)
			{
				//Cheng Du Comment
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ERROR, 0);
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_PPIDMISMATCH, 0);
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_SEQ_ERROR, 0);
			}
		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID == 1)	//2015.05.13 edit
		{
			pFrame->m_fnReportPCAlive();		//2015.10.31 Mr.Chu
			//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 0); //2015.10.31 Comment
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_FORWARD, 0);
			if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE || EQP_MONITOR_SIZE)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 0);
			} 	
			if(bErrReset)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ERROR, 0);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_PPIDMISMATCH, 0);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_SEQ_ERROR, 0);
			}
		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID == 2)	//2015.05.13 edit
		{
			if(m_nEqpType == EQP_TYPE_ACS)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 0);
				if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_FORWARD, 0);
				if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE || EQP_MONITOR_SIZE)
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 0);
				} 	
				if(bErrReset)
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ERROR, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_PPIDMISMATCH, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_SEQ_ERROR, 0);
				}
			}
		}

	}
	else
	{
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 0); // Grab Ack 신호 초기화
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_END, 0);	// Grab End  신호를 초기화한다.
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 0);	// 결과 신호를 초기화한다.
	}

#endif
	//2016.10.10 add by mby 
	Sleep(200);	//delay time
	//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK_BWD, 0);	//Chung Du Comment

// 	if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE )
// 	{
// 		//2016.10.10 add by mby 
// 		int bCheck = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_BACKWARD_CHECK);	
// 		G_AddLog_STG_SEQ(3, 0, _T("[STAGE POSITION] %d."), bCheck);
// 		
// 		if(bCheck)
// 		{
// 			m_bForwardScan = FALSE;
// 			m_fnSetStep(SEQ_BACKWARD_START);
// 		}
// 		else 
// 		{
// 			m_bForwardScan = TRUE;
// 			m_fnSetStep(SEQ_FORWARD_START);
// 		}
// 		bFlag = FALSE;
// 	}

	if(bFlag)
	{
		if(bDir)
		{
			m_bForwardScan = TRUE;
			m_fnSetStep(SEQ_FORWARD_START);
		}
		else
		{
			m_bForwardScan = FALSE;
			m_fnSetStep(SEQ_BACKWARD_START);
		}
	}

	m_nRetryCount = 0;
}

void CThreadSeqPLC::m_fnCheck()
{
	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
#if 0
	BOOL bForward, bGrabReady;

	if(m_nStageID == 0)
	{
		bForward = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_FORWARD);
		bGrabReady = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_READY);

		G_AddLog_SEQ(m_nStageID, _T("[Stage ID = %d] Read PLC Forward = %d, Grab Ready = %d"), m_nStageID, bForward, bGrabReady);
		bForward = bForward ? 0 : 1;
		bGrabReady = bGrabReady ? 0 : 1;

		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, bForward);
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, bGrabReady);
	}
	else if(m_nStageID == 1)
	{
		bForward = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_FORWARD);
		bGrabReady = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_READY);

		G_AddLog_SEQ(m_nStageID, _T("[Stage ID = %d] Read PLC Forward = %d, Grab Ready = %d"), m_nStageID, bForward, bGrabReady);
		bForward = bForward ? 0 : 1;
		bGrabReady = bGrabReady ? 0 : 1;

		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_FORWARD, bForward);
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, bGrabReady);
	}
	m_fnRetryStep();
	return;
#endif
	m_nCheckAlive++;

	if(m_nCheckAlive > 20)		// 50 * 20 -> 1000 msec
	{
		m_nCheckAlive = 0;
		m_nPCAlive = m_nPCAlive ? 0 : 1;

		//		long lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_ALIVE, m_nPCAlive);

		//if(lRet != 0)
		{
			// 작업중... Error 처리...
		}
	}
#ifdef DEBUG_TEST
	if(m_bForwardScan)		m_fnScanSeqForward();
	else					m_fnScanSeqBackward();
#else
	//2015.05.13 edit s- todo
	//검사 Stage에 매칭되어 있는 VISION PC(컨트롤 PC)만 처리 하도록 수정
	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1)
		{
			if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[m_nStageID])	// Vision Client 접속 확인후 SEQ 진입
			{
				//Sleep(30);
				if (pFrame->m_stParamInfo.nEqpObject == B_PROJECT)	// B Project 의 경우
				{
					if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
					{						
						m_fnScanSeqLarge();
					}
					else if(pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
					{
						if(m_bForwardScan)		
							m_fnScanSeqForward_M();
						else					
							m_fnScanSeqBackward_M();
					}
					else
					{
						if(m_bForwardScan)		m_fnScanSeqForward();
						else					m_fnScanSeqBackward();
					}
				}
				else if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT) // packing 의 경우 
				{
					m_fnCheckAlivePLC();
					// Packing 용 시퀀스 
					m_fnScanSeqPacking();
				}

			}
			else
			{
				//G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] Waiting..."), m_nStageID);
				m_fnResetSEQ(TRUE, TRUE, TRUE);
				m_fnRetryStep();
				//Sleep(3000);
			}
		}
	}
	//if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[m_nStageID])	// Vision Client 접속 확인후 SEQ 진입
	//{
	//	//Sleep(500);
	//	if(m_bForwardScan)		m_fnScanSeqForward();
	//	else					m_fnScanSeqBackward();
	//}
	//else
	//{
	//	//G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] Waiting..."), m_nStageID);
	//	m_fnResetSEQ(TRUE, TRUE, TRUE);
	//	m_fnRetryStep();

	//	Sleep(3000);
	//}
	//2013.05.13 edit e-

#endif
}

BOOL CThreadSeqPLC::m_fnRetryCheck(CString strMsg, int nTimeout)
{
	m_nRetryCount++;

	if(m_nRetryCount * SLEEP_COUNT < nTimeout)		// Timeout 확인
		//if(m_nRetryCount * SLEEP_COUNT < 100000)
	{
		Sleep(20);
		m_fnRetryStep();
		return TRUE;
	}
	else
	{
		m_nRetryCount = 0;
		G_AddLog_STG_SEQ(3,m_nStageID, _T("[StageID = %d] Retry End %s."), m_nStageID, strMsg);
		m_fnSetStep(SEQ_RESET);
		m_fnResetSEQ();
		return FALSE;
	}
}

//2015.05.13 add
BOOL CThreadSeqPLC::m_fnVisionReadyResCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 체크
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					if(!m_pServerWoarkManager->m_bRecvReadyRes[j])	return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CThreadSeqPLC::m_fnVisionReadyResModelMismatchCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 체크
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					if(m_pServerWoarkManager->m_nGrabReadyRes[j] == 1)	return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CThreadSeqPLC::m_fnVisionReadyResTriggerSetErrorCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 체크
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					if(m_pServerWoarkManager->m_nGrabReadyRes[j] == 2)	return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CThreadSeqPLC::m_fnVisionGrabEndFlagCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 체크
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					if(!m_pServerWoarkManager->m_bGrabEndFlag[j])		return FALSE;
				}
			}
		}
	}

	return TRUE;
}

BOOL CThreadSeqPLC::m_fnVisionGrabEndResCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
	{
		if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
		{
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 체크
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					if(m_pServerWoarkManager->m_nGrabEndRes[j] == 1)		return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CThreadSeqPLC::m_fnScanSeqLarge()
{
	G_AddLog(3, 3, _T("m_fnScanSeqLarge() - Start"));
	long lRet = -1;
	CString strModelID, strPanelID_1, strPanelID_2;

	static CString s_strPanelID_1 = _T(""), s_strPanelID_2 = _T(""); 
	CString strReqName;
	BOOL bScanForward = FALSE;
	long lGrabReq = FALSE;
	long lGrabEnd = FALSE;
	long lArkResult = FALSE;	//2015.10.10 ark time out test
	long lGrabEndOK = FALSE;
	//2015.09.17
	//BOOL bByPassReq1 = FALSE;
	//BOOL bByPassReq2 = FALSE;
	long lByPassPanel1 = 0;
	long lByPassPanel2 = 0;
	int nEqpSizeType = 0;
	CString strByPassPanel1 = _T("");
	CString strByPassPanel2 = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	
	//2015.09.17


	//2016.01.15
	long lBypassDefCode_Pnl1 = 0;
	long lBypassDefCode_Pnl2 = 0;
	//2016.01.15

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_fnGetStep())
	{
	case SEQ_FORWARD_START:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}

		G_AddLog_STG_SEQ(3,m_nStageID, _T("[StageID = %d] SEQ_FORWARD_START."), m_nStageID);
		m_fnSetStep(SEQ_F_GRAB_READY);
		break;

	case SEQ_F_GRAB_READY:			// PLC Map Write Forward & Grab Ready

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
		{
			//2018.01.20 Comment
			//if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 1);
			
			//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
			//2015.10.30
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[0].nVisionCnt; j++)
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					switch (j)
					{
					case 0:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
						break;
					case 1:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
						break;
					case 2:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
						break;
					case 3:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S4_GRAB_READY, 1);
						break;
					case 4:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S5_GRAB_READY, 1);
						break;
					case 5:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S6_GRAB_READY, 1);
						break;
					case 6:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S7_GRAB_READY, 1);
						break;
					case 7:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S8_GRAB_READY, 1);
						break;
					case 8:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S9_GRAB_READY, 1);
						break;
					default:
						break;
					}
				}else{//2020.08.17 add by mby S-
					if(!pFrame->m_stParamInfo.bUseInspPc[j]){
						switch (j)
						{
						case 0:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
							break;
						case 1:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
							break;
						case 2:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
							break;
						case 3:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S4_GRAB_READY, 1);
							break;
						case 4:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S5_GRAB_READY, 1);
							break;
						case 5:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S6_GRAB_READY, 1);
							break;
						case 6:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S7_GRAB_READY, 1);
							break;
						case 7:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S8_GRAB_READY, 1);
							break;
						case 8:
							lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S9_GRAB_READY, 1);
							break;
						default:
							break;
						}
					}
				}//2020.08.17 add by mby E-
			}
			//2015.10.30
		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
		{
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_FORWARD, 1);
			//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
			//2015.11.03 Mr.Chu
			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[0].nVisionCnt; j++)
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					switch (j)
					{
					case 6:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S7_GRAB_READY, 1);
						break;
					case 7:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S8_GRAB_READY, 1);
						break;
					case 8:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S9_GRAB_READY, 1);
						break;
					default:
						break;
					}
				}
			}

		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
		{
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_FORWARD, 1);
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
		}

		if(lRet == -1)	
		{
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_nVisionState[m_nStageID] = STATE_READY;
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}

				}
			}

		}
		//2015.05.13 edit e-

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_F_GRAB_REQ_CHECK);

		break;

	case SEQ_F_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0]){	    lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);    strReqName	= ADDR_PLC_S1_REQ;	}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0]){		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_REQ);	  strReqName	= ADDR_PLC_S2_REQ;	}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0]){		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_REQ);	  strReqName	= ADDR_PLC_S3_REQ;	}

		if(lGrabReq == OK)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [Area No = %s]  PLC READ Get PLC REQ : %d"),
				m_nStageID, strReqName, lGrabReq); 

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				//2016.03.07 Mr.Chu		측정 Sequence 변경 ( Grab 할 때 측정 함 )
				
				//Cheng Du Comment
				//m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_PANEL_ID, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				strPanelID_1.TrimRight();
				//Sleep(200); //2018.03.21 add by mby for align image test
				if(pFrame->m_stParamInfo.bUseAlignPc){	//2020.10.15 add mby Align PC Skip true:검사, false:미검사
					m_pServerWoarkManager->m_fnRequestAlign( strPanelID_1);
				}
				//Sleep(200);
				//				
				if (/*m_pDataManager->m_strPolPositionEQ_CV[0] == _T("") &&*/
					m_pDataManager->m_strPolPositionEQ_CV[1] == _T("") &&
					m_pDataManager->m_strPolPositionEQ_CV[2] == _T("") &&
					m_pDataManager->m_strPolPositionStage_CV[0] == _T("")&&
					m_pDataManager->m_strPolPositionStage_CV[1] == _T("")&&
					m_pDataManager->m_strPolPositionStage_CV[2] == _T(""))
				{
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_EQ_POSITION_1,  LEN_POL_POSITION, m_pDataManager->m_strPolPositionEQ_CV[0]);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_EQ_POSITION_2,  LEN_POL_POSITION, m_pDataManager->m_strPolPositionEQ_CV[1]);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_EQ_POSITION_3,  LEN_POL_POSITION, m_pDataManager->m_strPolPositionEQ_CV[2]);

					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_STAGE_POSITION_1, LEN_POL_POSITION, m_pDataManager->m_strPolPositionStage_CV[0]);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_STAGE_POSITION_2, LEN_POL_POSITION, m_pDataManager->m_strPolPositionStage_CV[1]);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_STAGE_POSITION_3, LEN_POL_POSITION, m_pDataManager->m_strPolPositionStage_CV[2]);
				}

				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_MODEL_ID, LEN_MODEL_ID, strModelID);
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			// 작업중.. Test 모드
			strModelID =  _T("");// _T("RCPID00000000001");
			//if (strPanelID_1 == _T(""))
			//{
			//	strPanelID_1.Format(_T("PNL000000000000%03d"), 1);// m_nPanelID1);
			//	//strPanelID_2.Format(_T(""));// m_nPanelID2);
			//	strPanelID_2.Format(_T("PNL000000000000%03d"), 2);// m_nPanelID2);
			//}

			strPanelID_1.TrimRight();
			strPanelID_2.TrimRight();

			nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				if ((strPanelID_1 == _T("") && strPanelID_2 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}
			else
			{
				if ((/*s_strPanelID_1 == */strPanelID_1 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}

			//2015.05.13 edit s-
			//m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strModelID = strModelID;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [PANEL ID1 = %s] [PANEL ID2 = %s] PLC READ Get PLC REQ : %d"),
				m_nStageID, strPanelID_1, strPanelID_2, m_pDlgPLCSetup->m_fnGetDevice((TCHAR *)(LPCTSTR)strReqName));

			//2015.11.25 Mr.Chu
			m_pDataManager->m_fnCurPanelDataInit(strPanelID_1,strPanelID_2,m_nStageID);

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = strPanelID_1;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = strPanelID_2;
				}
			}


			if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] NO PANEL ID : %s, %s, %s - READY REQ Error."), m_nStageID, strModelID, strPanelID_1, strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			//Chung Du Comment
			lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);	//2017.11.07 Hold Panel	1:Normal 2:Bypass
			//lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);


			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_fnRequestGrabReady(m_nStageID + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{

				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
					{
						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
							m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
						}
					}

					m_fnSetStep(SEQ_F_GRAB_READY_RES);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_REQ_CHECK."), m_nStageID);
					break;

					//Chung Du Comment
					////2015.09.17
					//if (m_nStageID  == 0)
					//{
					//	if (lByPassPanel1 == NG_FLAG_BYPASS)
					//	{
					//		stNgFlagInfo_Panel1.init_info();
					//		lBypassDefCode_Pnl1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL1);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_1, lBypassDefCode_Pnl1);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_1, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
					//		//stNgFlagInfo_Panel1.init_info();
					//		strPanelID_1 = _T("");	
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (lByPassPanel2 == NG_FLAG_BYPASS)
					//	{	
					//		stNgFlagInfo_Panel2.init_info();
					//		lBypassDefCode_Pnl2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL2);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_2, lBypassDefCode_Pnl2);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_2, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel2);
					//		//stNgFlagInfo_Panel2.init_info();
					//		strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
					//	}
					//}
					//else
					//{
					//	stNgFlagInfo_Panel1 = m_pDataManager->m_fnReadNgFlagList(strPanelID_1);
					//	stNgFlagInfo_Panel2 = m_pDataManager->m_fnReadNgFlagList(strPanelID_2);

					//	if (stNgFlagInfo_Panel1.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_1 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (stNgFlagInfo_Panel2.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
					//	}

					//}

					////2015.09.24 edit by ikm - s
					//if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
					//{
					//	if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
					//		lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S1_GRAB_ACK, 1);
					//	else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
					//		lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S2_GRAB_ACK, 1);
					//	else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
					//		lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S3_GRAB_ACK, 1);

					//	Sleep(2000);	//09.28 add by choo

					//	m_fnResetSEQ(TRUE, TRUE);

					//	G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BY PASS!!!! SEQ_END."), m_nStageID);
					//	m_fnSetStep(SEQ_FORWARD_START);
					//}
					//else
					//{
					//	for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
					//	{
					//		if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					//			//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
					//			m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
					//		}
					//	}

					//	m_fnSetStep(SEQ_F_GRAB_READY_RES);
					//	G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_REQ_CHECK."), m_nStageID);
					//	break;
					//}
				}

			}
			break;
		}
		else
		{
			m_fnRetryStep();
		}

		break;


	case SEQ_F_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())		//if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID])	//2015.05.13 edit
		{
			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		// 수신 flag 리셋
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;
						}

					}
				}
			}
			//2015.05.13 edit e-

			m_nRetryCount = 0;

			{
				//Sleep(100);
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					//2019.06.20 by ryu
					
						if (/*m_pDataManager->m_strPolPositionEQ_CV[0] != _T("") && */
							m_pDataManager->m_strPolPositionEQ_CV[1] != _T("") && 
							m_pDataManager->m_strPolPositionEQ_CV[2] != _T("") && 
							m_pDataManager->m_strPolPositionStage_CV[0] != _T("")&&
							m_pDataManager->m_strPolPositionStage_CV[1] != _T("")&&
							m_pDataManager->m_strPolPositionStage_CV[2] != _T(""))
						{
							if (m_pDataManager->m_strPolPositionEQ_CV2 == _T("") &&	m_pDataManager->m_strPolPositionStage_CV2 == _T(""))
							{
								m_pDataManager->m_strPolPositionEQ_CV2  = (m_pDataManager->m_strPolPositionEQ_CV[0] + m_pDataManager->m_strPolPositionEQ_CV[1]+ m_pDataManager->m_strPolPositionEQ_CV[2]);
								m_pDataManager->m_strPolPositionStage_CV2 = (m_pDataManager->m_strPolPositionStage_CV[0]+m_pDataManager->m_strPolPositionStage_CV[1]+m_pDataManager->m_strPolPositionStage_CV[2]);
							}						

							for(int i =0; i<3; i++)
							{
								m_pDataManager->m_strPolPositionEQ_CV[i]  = _T("");
								m_pDataManager->m_strPolPositionStage_CV[i] = _T("");
							}
							
						}
						
					

					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					}
				}
				//2015.10.10 ark time out test add e-

				if(lRet == -1)	
				{
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}

				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_ACK."), m_nStageID);

				m_fnSetStep(SEQ_F_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip

			}

			m_fnSetStep(SEQ_F_GRAB_END_CHECK);
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY_RES."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_READY_RES"), 10000))
			{
				G_AddLog_ALARM(_T("m_fnScanSeqLarge - Vision PC No Ready StageID = %d : %d"),m_nStageID, ERROR_VISION_NOREADY);
				//G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_VISION_NOREADY"), m_nStageID);

				/*if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}*/
			}
		}
		break;

	case SEQ_F_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())	//if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID])	//2015.05.13 edit
		{
			m_nRetryCount = 0;
			Sleep(100);	//2017.05.25 add by mby

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//Sleep(300);
				
				//2018.03.14 edit s-
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);
				lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_CON1_BUSY);
				if(lRet == OK)
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
				else
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
					m_fnSetStep(SEQ_FORWARD_END);
				}
				//2018.03.14 edit e-
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			m_fnSetStep(SEQ_F_RESULT_PROCESS);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_END_CHECK"), 10000))
			{
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL, 1);	//2018.06.28 타이밍 체크 필요
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
				{
					//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 1);
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
				{
					//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 1);
				}
				if(lRet == -1)	
				{
					G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}
				m_fnSetStep(SEQ_F_RESULT_PROCESS);
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK TimeOut."), m_nStageID);
				G_AddLog_ALARM(_T("m_fnScanSeqLarge - SEQ_F_GRAB_END_CHECK, StageID = %d : %d"),m_nStageID, ERROR_VISION_GRAB_END);

			}
		}
		break;
	case SEQ_F_RESULT_PROCESS:

		////2018.01.17
		m_nRetryCount = 0;
		{
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_GRAB_END_OK);

			if(lGrabEndOK)
			{
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 0);

				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, FALSE);	//2019.06.22 add by mby
			}
			else
			{
				m_fnRetryStep();
				break;
			}
		}
		////

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_RESULT_PROCESS."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_END);
		break;
	case SEQ_FORWARD_END:
		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_fnResetFlag(m_nStageID + 1);
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{

					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}

				}
			}
		}

		m_fnResetSEQ(TRUE, TRUE);

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_FORWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_START);
		break;
	}
	G_AddLog(3, 3, _T("m_fnScanSeqLarge() - End"));
}

void CThreadSeqPLC::m_fnScanSeqForward()
{
	G_AddLog(3, 3, _T("m_fnScanSeqForward() - Start"));
	long lRet = -1;
	CString strModelID, strPanelID_1, strPanelID_2;

	static CString s_strPanelID_1 = _T(""), s_strPanelID_2 = _T(""); 
	CString strReqName;
	BOOL bScanForward = FALSE;
	long lGrabReq = FALSE;
	long lGrabEnd = FALSE;
	long lArkResult = FALSE;	//2015.10.10 ark time out test
	long lGrabEndOK = FALSE;
	//2015.09.17
	//BOOL bByPassReq1 = FALSE;
	//BOOL bByPassReq2 = FALSE;
	long lByPassPanel1 = 0;
	long lByPassPanel2 = 0;
	int nEqpSizeType = 0;
	CString strByPassPanel1 = _T("");
	CString strByPassPanel2 = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	
	//2015.09.17


	//2016.01.15
	long lBypassDefCode_Pnl1 = 0;
	long lBypassDefCode_Pnl2 = 0;
	//2016.01.15

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_fnGetStep())
	{
	case SEQ_FORWARD_START:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}

		G_AddLog_STG_SEQ(3,m_nStageID, _T("[StageID = %d] SEQ_FORWARD_START."), m_nStageID);
		m_fnSetStep(SEQ_F_GRAB_READY);
		break;

	case SEQ_F_GRAB_READY:			// PLC Map Write Forward & Grab Ready

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
		{
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, s_strPanelID_1);
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, s_strPanelID_2);
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 1);
			lRet =m_pDlgPLCSetup-> m_fnSetDevice(ADDR_PC_S1_ALIVE, 1);
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
		{
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, s_strPanelID_1);
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_2, LEN_PANEL_ID, s_strPanelID_2);
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_FORWARD, 1);
			lRet =m_pDlgPLCSetup-> m_fnSetDevice(ADDR_PC_S2_ALIVE, 1);
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
		}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
		{
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, s_strPanelID_1);
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_2, LEN_PANEL_ID, s_strPanelID_2);
			if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_FORWARD, 1);
			lRet =m_pDlgPLCSetup-> m_fnSetDevice(ADDR_PC_S3_ALIVE, 1);
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
		}

		if(lRet == -1)	
		{
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_nVisionState[m_nStageID] = STATE_READY;
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}

				}
			}

		}
		//2015.05.13 edit e-

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_F_GRAB_REQ_CHECK);

		break;

	case SEQ_F_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0]){	    lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);    strReqName	= ADDR_PLC_S1_REQ;	}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0]){		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_REQ);	  strReqName	= ADDR_PLC_S2_REQ;	}
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0]){		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_REQ);	  strReqName	= ADDR_PLC_S3_REQ;	}

		if(lGrabReq == OK)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [Area No = %s]  PLC READ Get PLC REQ : %d"),
				m_nStageID, strReqName, lGrabReq); 


			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			// 작업중.. Test 모드
			strModelID =  _T("");// _T("RCPID00000000001");
			//if (strPanelID_1 == _T(""))
			//{
			//	strPanelID_1.Format(_T("PNL000000000000%03d"), 1);// m_nPanelID1);
			//	//strPanelID_2.Format(_T(""));// m_nPanelID2);
			//	strPanelID_2.Format(_T("PNL000000000000%03d"), 2);// m_nPanelID2);
			//}

			strPanelID_1.TrimRight();
			strPanelID_2.TrimRight();

			nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				if ((strPanelID_1 == _T("") && strPanelID_2 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}
			else
			{
				if ((/*s_strPanelID_1 == */strPanelID_1 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}

			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [PANEL ID1 = %s] [PANEL ID2 = %s] PLC READ Get PLC REQ : %d"),
				m_nStageID, strPanelID_1, strPanelID_2, m_pDlgPLCSetup->m_fnGetDevice((TCHAR *)(LPCTSTR)strReqName));

			//2015.11.25 Mr.Chu
			m_pDataManager->m_fnCurPanelDataInit(strPanelID_1,strPanelID_2,m_nStageID);

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = strPanelID_1;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = strPanelID_2;
				}
			}


			if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] NO PANEL ID : %s, %s, %s - READY REQ Error."), m_nStageID, strModelID, strPanelID_1, strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			//2016.09.10 Mr.Chu		소형 AVL Bypass 구분
			if ( nEqpSizeType == EQP_SMALL_SIZE && m_nEqpType == EQP_TYPE_AVL )
			{
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				{
					lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
					lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
				{
					lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ2_PANEL1);		
					lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ2_PANEL2);
				}
				G_AddLog_STG_SEQ(3, m_nStageID, _T("Panel Type - [Paenl 1 = %d]  [Panel 2 = %d] "), lByPassPanel1 , lByPassPanel2);
			}
			else
			{
				lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
				lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);
			}
			//


			//2015.09.28 Mr.Chu
			m_strPanelID_1 = strPanelID_1;
			m_strPanelID_2 = strPanelID_2;

			//2015.09.17
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					//2015.09.17
					if (m_nStageID  == 0)
					{
						if (lByPassPanel1 == NG_FLAG_BYPASS)
						{
							lBypassDefCode_Pnl1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL1);
							m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_1, lBypassDefCode_Pnl1);
							stNgFlagInfo_Panel1.DefaultData(strPanelID_1, NG_FLAG_BYPASS);
							m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
							stNgFlagInfo_Panel1.init_info();
							strPanelID_1 = _T("");	
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
						}
						if (lByPassPanel2 == NG_FLAG_BYPASS)
						{	
							lBypassDefCode_Pnl2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL2);
							m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_2, lBypassDefCode_Pnl2);
							stNgFlagInfo_Panel1.DefaultData(strPanelID_2, NG_FLAG_BYPASS);
							m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel2);
							stNgFlagInfo_Panel2.init_info();
							strPanelID_2 = _T("");
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
						}
					}
					else
					{
						stNgFlagInfo_Panel1 = m_pDataManager->m_fnReadNgFlagList(strPanelID_1);
						stNgFlagInfo_Panel2 = m_pDataManager->m_fnReadNgFlagList(strPanelID_2);

						if (stNgFlagInfo_Panel1.nNgFlag == 2)
						{
							//strPanelID_1 = _T("");
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
						}
						if (stNgFlagInfo_Panel2.nNgFlag == 2)
						{
							//strPanelID_2 = _T("");
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
						}

					}
					//2015.09.24 edit by ikm - s
					if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
					{
						if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
							lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S1_GRAB_ACK, 1);
						else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
							lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S2_GRAB_ACK, 1);
						else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
							lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S3_GRAB_ACK, 1);

						Sleep(2000);	//09.28 add by choo

						m_fnResetSEQ(TRUE, TRUE);

						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BY PASS!!!! SEQ_END."), m_nStageID);
						m_fnSetStep(SEQ_FORWARD_START);
					}
					else
					{
						//2015.09.17

						for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
						{
							if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
								//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
								m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
							}
						}

						m_fnSetStep(SEQ_F_GRAB_READY_RES);
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_REQ_CHECK."), m_nStageID);
						break;
					}		
					//2015.09.24 edit by ikm - e
				}

			}
			break;
		}
		else
		{
			m_fnRetryStep();
		}

		break;


	case SEQ_F_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())		//if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID])	//2015.05.13 edit
		{
			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		// 수신 flag 리셋
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;
						}

					}
				}
			}
			//2015.05.13 edit e-

			m_nRetryCount = 0;

			{
				//Sleep(100);
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					}
				}
				//2015.10.10 ark time out test add e-

				if(lRet == -1)	
				{
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}

				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_ACK."), m_nStageID);

				m_fnSetStep(SEQ_F_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip

			}

			m_fnSetStep(SEQ_F_GRAB_END_CHECK);
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY_RES."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_READY_RES"), 10000))
			{
				G_AddLog_ALARM(_T("m_fnScanSeqForward - Vision PC No Ready StageID = %d : %d"),m_nStageID, ERROR_VISION_NOREADY);
				//G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_VISION_NOREADY"), m_nStageID);

				/*if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}*/
			}
		}
		break;


	case SEQ_F_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())	//if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID])	//2015.05.13 edit

		{
			m_nRetryCount = 0;

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
				if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 0);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
				if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_FORWARD, 0);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
				if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_FORWARD, 0);
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			m_fnSetStep(SEQ_F_RESULT_PROCESS);
			break;
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_END_CHECK"), 20000))
			{
				G_AddLog_ALARM(_T("m_fnScanSeqForward - SEQ_F_GRAB_END_CHECK, StageID = %d : %d"),m_nStageID, ERROR_VISION_GRAB_END);
				break;
			}
				
		}
		break;
	case SEQ_F_RESULT_PROCESS:

		m_nRetryCount = 0;
		{
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_GRAB_END_OK);

			if(lGrabEndOK)
			{

				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 0);
			}
			else
			{
				m_fnRetryStep();
				break;
			}

			if(m_nEqpType == EQP_TYPE_AVL)			// ACC 이면 Backward Scan 시작
			{
				m_bForwardScan = FALSE;
				m_fnSetStep(SEQ_BACKWARD_START);	
			}
			else
			{
				m_fnSetStep(SEQ_FORWARD_END);
			}
		}
		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_RESULT_PROCESS."), m_nStageID);
		//m_fnSetStep(SEQ_FORWARD_END);
		break;

	case SEQ_FORWARD_END:
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{

					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}

				}
			}
		}

		m_fnResetSEQ(TRUE, TRUE);

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_FORWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_START);
		break;
	}
	G_AddLog(3, 3, _T("m_fnScanSeqForward() - End"));
}

void CThreadSeqPLC::m_fnScanSeqBackward()
{
	G_AddLog(3, 3, _T("m_fnScanSeqBackward() - Start"));
	long lRet = -1;
	CString strModelID, strPanelID_1, strPanelID_2;
	long lScanForward = FALSE;
	long lGrabReq = FALSE;
	long lGrabEnd = FALSE;
	long lGrabEndOK = FALSE;
	long lArkResult = FALSE;	//2015.10.10 ark time out test
	//2015.09.17
	//BOOL bByPassReq1 = FALSE;
	//BOOL bByPassReq2 = FALSE;
	long lByPassPanel1 = 0;
	long lByPassPanel2 = 0;
	CString strByPassPanel1 = _T("");
	CString strByPassPanel2 = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	
	//2015.09.17
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	int nEqpSizeType = 0;

	switch(m_fnGetStep())
	{
	case SEQ_BACKWARD_START: 
		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_fnResetFlag(m_nStageID + 1);
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}
		//2015.05.13 edit e-

		//2015.12.07 comment by ikm for Plc interface Tact time 
		//m_fnResetSEQ(FALSE, TRUE);

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_BACKWARD_START."), m_nStageID);

		m_fnSetStep(SEQ_B_GRAB_READY);
		break;
	case SEQ_B_GRAB_READY:			// PLC Map Write Grab Ready
		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);

		if(lRet == -1)	
		{
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_nVisionState[m_nStageID] = STATE_READY;
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}

				}
			}

		}
		//2015.05.13 edit e-

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_B_GRAB_REQ_CHECK);

		break;
	case SEQ_B_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ
		//2015.05.13 edit s-
		//if(m_nStageID == 0)			bGrabReq = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_S1_REQ);
		//else if(m_nStageID == 1)		bGrabReq = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_S2_REQ);
		//else if(m_nStageID == 2)		bGrabReq = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_S3_REQ);
		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])				lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_REQ);
		else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])		lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_REQ);
		//2015.05.13 edit e-

		if(lGrabReq == OK)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				//	lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				//	lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_MODEL_ID, LEN_MODEL_ID, strModelID);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			// 작업중.. Test 모드
			strModelID = _T("");// _T("RCPID00000000001");
			//if (strPanelID_1 == _T(""))
			//{
			//	strPanelID_1.Format(_T("PNL000000000000%03d"), 1);// m_nPanelID1);
			//	//strPanelID_2.Format(_T(""));// m_nPanelID2);
			//	strPanelID_2.Format(_T("PNL000000000000%03d"), 2);// m_nPanelID2);
			//}

			strPanelID_1.TrimRight();
			strPanelID_2.TrimRight();

			if(/*strModelID == "" || */(strPanelID_1 == _T("") && strPanelID_2 == _T("")))
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] NO PANEL ID : %s, %s, %s - READY REQ Error."), m_nStageID, strModelID, strPanelID_1, strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			//2016.09.10 Mr.Chu		소형 AVL Bypass 구분
			nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

			if ( nEqpSizeType == EQP_SMALL_SIZE && m_nEqpType == EQP_TYPE_AVL )
			{
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				{
					lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
					lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
				{
					lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ2_PANEL1);		
					lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ2_PANEL2);
				}
			}
			else
			{
				lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
				lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);
			}
			//

			//2015.09.28 Mr.Chu
			m_strPanelID_1 = strPanelID_1;
			m_strPanelID_2 = strPanelID_2;

			//2015.09.17
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					//2015.09.17
					if (m_nStageID  == 0)
					{
						if (lByPassPanel1 == NG_FLAG_BYPASS)
						{
							stNgFlagInfo_Panel1.DefaultData(strPanelID_1, NG_FLAG_BYPASS);							
							m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
							stNgFlagInfo_Panel1.init_info();	
							strPanelID_1 = _T("");	
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL1 BYPASS"), m_nStageID);
						}
						if (lByPassPanel2 == NG_FLAG_BYPASS)
						{	
							stNgFlagInfo_Panel1.DefaultData(strPanelID_2, NG_FLAG_BYPASS);
							m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel2);
							stNgFlagInfo_Panel2.init_info();
							strPanelID_2 = _T("");
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL2 BYPASS"), m_nStageID);
						}
					}
					else
					{
						stNgFlagInfo_Panel1 = m_pDataManager->m_fnReadNgFlagList(m_strPanelID_1);
						stNgFlagInfo_Panel2 = m_pDataManager->m_fnReadNgFlagList(m_strPanelID_2);

						if (stNgFlagInfo_Panel1.nNgFlag == 2)
						{
							strPanelID_1 = _T("");	
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL1 BYPASS"), m_nStageID);
						}
						if (stNgFlagInfo_Panel2.nNgFlag == 2)
						{
							strPanelID_2 = _T("");
							G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL2 BYPASS"), m_nStageID);
						}

					}
					//2015.09.17

					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
					{
						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_BACKWARD, strModelID, strPanelID_1, strPanelID_2);
							m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_BACKWARD, strModelID, strPanelID_1, strPanelID_2);
						}
					}

				}

			}
			//2015.05.13 edit e-


			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_REQ_CHECK."), m_nStageID);
			m_fnSetStep(SEQ_B_GRAB_READY_RES);

		}
		else
		{
			m_fnRetryStep();
		}

		break;
	case SEQ_B_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())		//if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID])	//2015.05.13 edit
		{
			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		// 수신 flag 리셋
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;
						}

					}
				}
			}
			//2015.05.13 edit e-

			m_nRetryCount = 0;
			{
				//Sleep(100);
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 1);
					}
				}
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 1);
					}
				}
				//2015.10.10 ark time out test add e-

				if(lRet == -1)	
				{
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}

				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_READY_RES."), m_nStageID);

				m_fnSetStep(SEQ_B_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip
			}
		}

		else
			if (!m_fnRetryCheck(_T("SEQ_B_GRAB_READY_RES"), 10000))
			{
				G_AddLog_ALARM(_T("m_fnScanSeqBackward - Vision PC No Ready StageID = %d : %d"),m_nStageID, ERROR_VISION_NOREADY);
				/*if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}*/
			}

			break;

	case SEQ_B_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())	//if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID])	//2015.05.13 edit
		{
			m_nRetryCount = 0;
			//Grab End OK

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}			

			m_nRetryCount = 0;

			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_END_CHECK."), m_nStageID);
			m_fnSetStep(SEQ_B_RESULT_PROCESS);
		}

		else
		{
			if (!m_fnRetryCheck(_T("SEQ_B_GRAB_END_CHECK"), 20000))
			{
				G_AddLog_ALARM(_T("m_fnScanSeqBackward - SEQ_B_GRAB_END_CHECK, StageID = %d : %d"),m_nStageID, ERROR_VISION_GRAB_END);
				break;
			}
		}

		break;
	case SEQ_B_RESULT_PROCESS:
		{
			lGrabEndOK = FALSE;

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_GRAB_END_OK);
			else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
				lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_GRAB_END_OK);

			if(lGrabEndOK)
			{

				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_END, 0);
				else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_END, 0);

			}
			else
			{
				m_fnRetryStep();
				break;
			}

			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_RESULT_PROCESS."), m_nStageID);
			m_fnSetStep(SEQ_BACKWARD_END);
			break;

		}
	case SEQ_BACKWARD_END:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{

					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}

				}
			}
		}


		m_fnResetSEQ(TRUE, TRUE);

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_BACKWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_START);
		//m_fnSetStep(SEQ_BACKWARD_START);
		break;
	}
	G_AddLog(3, 3, _T("m_fnScanSeqBackward() - End"));
}
////////////////////monitor -s

void CThreadSeqPLC::m_fnScanSeqForward_M()
{
	long lRet = -1;
	CString strModelID, strPanelID_1, strPanelID_2;

	static CString s_strPanelID_1 = _T(""), s_strPanelID_2 = _T(""); 
	CString strReqName;
	BOOL bScanForward = FALSE;
	long lGrabReq = FALSE;
	long lGrabEnd = FALSE;
	long lArkResult = FALSE;	//2015.10.10 ark time out test
	long lGrabEndOK = FALSE;
	//2015.09.17
	//BOOL bByPassReq1 = FALSE;
	//BOOL bByPassReq2 = FALSE;
	long lByPassPanel1 = 0;
	long lByPassPanel2 = 0;
	int nEqpSizeType = 0;
	CString strByPassPanel1 = _T("");
	CString strByPassPanel2 = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	
	//2015.09.17

	int nReadPnlCnt;

	//2016.01.15
	long lBypassDefCode_Pnl1 = 0;
	long lBypassDefCode_Pnl2 = 0;
	//2016.01.15
	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_fnGetStep())
	{
	case SEQ_FORWARD_START:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}

		G_AddLog_STG_SEQ(3,m_nStageID, _T("[StageID = %d] SEQ_FORWARD_START."), m_nStageID);
 		m_fnSetStep(SEQ_F_GRAB_READY);
		break;

	case SEQ_F_GRAB_READY:			// PLC Map Write Forward & Grab Ready

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
		{
			//if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 1);
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, s_strPanelID_1);

			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[0].nVisionCnt; j++)
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					switch (j)
					{
					case 0:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
						break;
					case 1:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
						break;
					case 2:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
						break;
					case 3:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S4_GRAB_READY, 1);
						break;
					case 4:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S5_GRAB_READY, 1);
						break;
					case 5:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S6_GRAB_READY, 1);
						break;	
					case 6:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S7_GRAB_READY, 1);
						break;
					case 7:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S8_GRAB_READY, 1);
						break;
					case 8:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S9_GRAB_READY, 1);
						break;
					default:
						break;
					}
				}
			}

		}

		if(lRet == -1)	
		{
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_nVisionState[m_nStageID] = STATE_READY;
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}
					
				}
			}

		}
		//2015.05.13 edit e-

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_F_GRAB_REQ_CHECK);

		break;

	case SEQ_F_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0]){	    lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);    strReqName	= ADDR_PLC_S1_REQ;	}

		if(lGrabReq == OK)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [Area No = %s]  PLC READ Get PLC REQ : %d"),
				 m_nStageID, strReqName, lGrabReq); 
			
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				nReadPnlCnt = 0;
				while(nReadPnlCnt < 5)
				{
					Sleep(100);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);

					if (strPanelID_1 != _T(""))
						break;
					nReadPnlCnt ++;
				}
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}
	
			strPanelID_1.TrimRight();
			strPanelID_2.TrimRight();

			nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				if ((strPanelID_1 == _T("") && strPanelID_2 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}
			else
			{
				if ((/*s_strPanelID_1 == */strPanelID_1 == _T("")))
				{
					m_fnRetryStep();
					break;
				}
			}

			//2015.05.13 edit s-
			//m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strModelID = strModelID;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [PANEL ID1 = %s] [PANEL ID2 = %s] PLC READ Get PLC REQ : %d"),
						 m_nStageID, strPanelID_1, strPanelID_2, m_pDlgPLCSetup->m_fnGetDevice((TCHAR *)(LPCTSTR)strReqName));

			//2015.11.25 Mr.Chu
			m_pDataManager->m_fnCurPanelDataInit(strPanelID_1,strPanelID_2,m_nStageID);

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = strPanelID_1;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = strPanelID_2;
				}
			}


			if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] NO PANEL ID : %s, %s, %s - READY REQ Error."), m_nStageID, strModelID, strPanelID_1, strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			//lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
			//lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);


			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_fnRequestGrabReady(m_nStageID + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{

				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					//2015.09.17
					//if (m_nStageID  == 0)
					//{
					//	if (lByPassPanel1 == NG_FLAG_BYPASS)
					//	{
					//		lBypassDefCode_Pnl1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL1);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_1, lBypassDefCode_Pnl1);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_1, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
					//		stNgFlagInfo_Panel1.init_info();
					//		strPanelID_1 = _T("");	
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (lByPassPanel2 == NG_FLAG_BYPASS)
					//	{	
					//		lBypassDefCode_Pnl2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL2);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_2, lBypassDefCode_Pnl2);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_2, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel2);
					//		stNgFlagInfo_Panel2.init_info();
					//		strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
					//	}
					//}
					//else
					//{
					//	stNgFlagInfo_Panel1 = m_pDataManager->m_fnReadNgFlagList(strPanelID_1);
					//	stNgFlagInfo_Panel2 = m_pDataManager->m_fnReadNgFlagList(strPanelID_2);

					//	if (stNgFlagInfo_Panel1.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_1 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (stNgFlagInfo_Panel2.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] FORWARD PANEL2 BYPASS"), m_nStageID);
					//	}

					//}
					//2015.09.24 edit by ikm - s
					if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
					{
						if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
							lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S1_GRAB_ACK, 1);

						Sleep(2000);	//09.28 add by choo

						m_fnResetSEQ(TRUE, TRUE);

						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BY PASS!!!! SEQ_END."), m_nStageID);
						m_fnSetStep(SEQ_FORWARD_START);
					}
					else
					{
						for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
						{
							if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
								//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
								m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
							}
			
						}
					}
				}

			}

			m_fnSetStep(SEQ_F_GRAB_READY_RES);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_REQ_CHECK."), m_nStageID);

		}
		else
		{
			m_fnRetryStep();
		}

		break;


	case SEQ_F_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())		//if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID])	//2015.05.13 edit
		{
			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		// 수신 flag 리셋
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;
						}
						
					}
				}
			}
			//2015.05.13 edit e-

			m_nRetryCount = 0;

			{
				//Sleep(100);
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 1);
					}
				}

				//2015.10.10 ark time out test add e-

				if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}

				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_ACK."), m_nStageID);

				m_fnSetStep(SEQ_F_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip

			}

			m_fnSetStep(SEQ_F_GRAB_END_CHECK);
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY_RES."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_READY_RES"), 30000))
			{
				//G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_VISION_NOREADY"), m_nStageID);
				
				/*if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}*/
			}
		}
		break;

	case SEQ_F_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())	//if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID])	//2015.05.13 edit
		{
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bGrabEndFlag[j] = FALSE;
						}
					}
				}
			}
			m_nRetryCount = 0;

			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//Sleep(300);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			m_fnSetStep(SEQ_F_RESULT_PROCESS);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_END_CHECK"), 20000))	//2017.02.16 10000->20000
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK TimeOut."), m_nStageID);
				G_AddLog_ALARM(_T("m_fnScanSeqForward - SEQ_F_GRAB_END_CHECK, StageID = %d : %d"),m_nStageID, ERROR_VISION_GRAB_END);
				break;
				//if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				//{
				//	//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 1);
				//}
				//if(lRet == -1)	
				//{
				//	G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				//	m_fnSetStep(SEQ_RESET);
				//	m_fnResetSEQ();
				//	break;
				//}
				//m_fnSetStep(SEQ_F_RESULT_PROCESS);
				//G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK TimeOut."), m_nStageID);
			}
		}
		break;
	case SEQ_F_RESULT_PROCESS:

		long lGrabEndOK;
		lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK);
		if(lGrabEndOK)
		{
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END, 0);
		}
		else
		{
			m_fnRetryStep();
			break;
		}

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_RESULT_PROCESS."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_END);
		break;
	case SEQ_FORWARD_END:
		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_fnResetFlag(m_nStageID + 1);
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{

					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				
				}
			}
		}


		m_fnResetSEQ(TRUE, TRUE);
		m_bForwardScan = FALSE;
		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_FORWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_BACKWARD_START);	
		break;
	}

}

void CThreadSeqPLC::m_fnScanSeqBackward_M()
{
	long lRet = -1;
	CString strModelID, strPanelID_1, strPanelID_2;

	static CString s_strPanelID_1 = _T(""), s_strPanelID_2 = _T(""); 
	CString strReqName;
	BOOL bScanForward = FALSE;
	long lGrabReq = FALSE;
	long lGrabEnd = FALSE;
	long lArkResult = FALSE;	//2015.10.10 ark time out test
	long lGrabEndOK = FALSE;
	//2015.09.17
	//BOOL bByPassReq1 = FALSE;
	//BOOL bByPassReq2 = FALSE;
	long lByPassPanel1 = 0;
	long lByPassPanel2 = 0;
	int nEqpSizeType = 0;
	CString strByPassPanel1 = _T("");
	CString strByPassPanel2 = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	
	//2015.09.17


	//2016.01.15
	long lBypassDefCode_Pnl1 = 0;
	long lBypassDefCode_Pnl2 = 0;
	//2016.01.15

	int nReadPnlCnt;
	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_fnGetStep())
	{
	case SEQ_BACKWARD_START:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}

		G_AddLog_STG_SEQ(3,m_nStageID, _T("[StageID = %d] SEQ_BACKWARD_START."), m_nStageID);
 		m_fnSetStep(SEQ_B_GRAB_READY);
		break;

	case SEQ_B_GRAB_READY:			// PLC Map Write Forward & Grab Ready

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
		{
			//if(m_nEqpType == EQP_TYPE_ACC)	lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_FORWARD, 1);
			//lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, s_strPanelID_1);

			for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[0].nVisionCnt; j++)
			{
				if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
					switch (j)
					{
					case 0:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_READY, 1);
						break;
					case 1:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S2_GRAB_READY, 1);
						break;
					case 2:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S3_GRAB_READY, 1);
						break;
					case 3:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S4_GRAB_READY, 1);
						break;
					case 4:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S5_GRAB_READY, 1);
						break;
					case 5:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S6_GRAB_READY, 1);
						break;	
					case 6:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S7_GRAB_READY, 1);
						break;
					case 7:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S8_GRAB_READY, 1);
						break;
					case 8:
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S9_GRAB_READY, 1);
						break;
					default:
						break;
					}
				}
			}

		}

		if(lRet == -1)	
		{
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		//2015.05.13 edit s-
		//m_pServerWoarkManager->m_nVisionState[m_nStageID] = STATE_READY;
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}
					
				}
			}

		}
		//2015.05.13 edit e-

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_B_GRAB_REQ_CHECK);

		break;

	case SEQ_B_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0]){	    lGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ_BWD);strReqName	= ADDR_PLC_S1_REQ_BWD;	}

		if(lGrabReq == OK)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [Area No = %s]  PLC READ Get PLC REQ : %d"),
				 m_nStageID, strReqName, lGrabReq); 
			
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				nReadPnlCnt = 0;
				while(nReadPnlCnt < 5)
				{
					Sleep(100);
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1_BWD, LEN_PANEL_ID, strPanelID_1);

					if (strPanelID_1 != _T(""))
						break;
					nReadPnlCnt ++;
				}
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			// 작업중.. Test 모드
			strModelID =  _T("");// _T("RCPID00000000001");
			//if (strPanelID_1 == _T(""))
			//{
			//	strPanelID_1.Format(_T("PNL000000000000%03d"), 1);// m_nPanelID1);
			//	//strPanelID_2.Format(_T(""));// m_nPanelID2);
			//	strPanelID_2.Format(_T("PNL000000000000%03d"), 2);// m_nPanelID2);
			//}
	
			strPanelID_1.TrimRight();
			strPanelID_2.TrimRight();

			nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

			if (strPanelID_1 == _T(""))
			{
				m_fnRetryStep();
				break;
			}

			//2015.05.13 edit s-
			//m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strModelID = strModelID;
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] [PANEL ID1 = %s] [PANEL ID2 = %s] PLC READ Get PLC REQ : %d"),
						 m_nStageID, strPanelID_1, strPanelID_2, m_pDlgPLCSetup->m_fnGetDevice((TCHAR *)(LPCTSTR)strReqName));

			//2015.11.25 Mr.Chu
			//m_pDataManager->m_fnCurPanelDataInit(strPanelID_1,strPanelID_2,m_nStageID);

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = strPanelID_1;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = strPanelID_2;
				}
			}


			if( strPanelID_1 == _T("") )
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] NO PANEL ID : %s, %s, %s - READY REQ Error."), m_nStageID, strModelID, strPanelID_1, strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			//lByPassPanel1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL1);		
			//lByPassPanel2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_REQ_PANEL2);


			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_fnRequestGrabReady(m_nStageID + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{

				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					//2015.09.17
					//if (m_nStageID  == 0)
					//{
					//	if (lByPassPanel1 == NG_FLAG_BYPASS)
					//	{
					//		lBypassDefCode_Pnl1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL1);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_1, lBypassDefCode_Pnl1);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_1, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel1);
					//		stNgFlagInfo_Panel1.init_info();
					//		strPanelID_1 = _T("");	
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (lByPassPanel2 == NG_FLAG_BYPASS)
					//	{	
					//		lBypassDefCode_Pnl2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_BYPASS_DEFECT_CODE_PANEL2);
					//		m_pDataManager->m_fnMakeQMSByPassData_AVL(strPanelID_2, lBypassDefCode_Pnl2);
					//		stNgFlagInfo_Panel1.DefaultData(strPanelID_2, NG_FLAG_BYPASS);
					//		m_pDataManager->m_fnWriteNgFlagList(stNgFlagInfo_Panel2);
					//		stNgFlagInfo_Panel2.init_info();
					//		strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL2 BYPASS"), m_nStageID);
					//	}
					//}
					//else
					//{
					//	stNgFlagInfo_Panel1 = m_pDataManager->m_fnReadNgFlagList(strPanelID_1);
					//	stNgFlagInfo_Panel2 = m_pDataManager->m_fnReadNgFlagList(strPanelID_2);

					//	if (stNgFlagInfo_Panel1.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_1 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL1 BYPASS"), m_nStageID);
					//	}
					//	if (stNgFlagInfo_Panel2.nNgFlag == NG_FLAG_BYPASS)
					//	{
					//		//strPanelID_2 = _T("");
					//		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BACKWARD PANEL2 BYPASS"), m_nStageID);
					//	}

					//}
					//2015.09.24 edit by ikm - s
					if(strPanelID_1 == _T("") && strPanelID_2 == _T(""))
					{
						if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
							lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S1_GRAB_ACK_BWD, 1);
						//else if(m_nStageID + 1 == pFrame->m_stStageInfo[1].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_2)	//2015.05.13 edit
							//Ret = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S2_GRAB_ACK, 1);
						//else if(m_nStageID + 1 == pFrame->m_stStageInfo[2].nVisionNo[0])	//else if(m_nStageID + 1 == VISION_PC_3)	//2015.05.13 edit
							//lRet = m_pDlgPLCSetup->m_fnSetDeviceAutoClear(ADDR_PC_S3_GRAB_ACK, 1);

						Sleep(2000);	//09.28 add by choo

						m_fnResetSEQ(TRUE, TRUE);

						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] BY PASS!!!! SEQ_END."), m_nStageID);
						m_fnSetStep(SEQ_FORWARD_START);
					}
					else
					{
						for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
						{
							if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
								//m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);
								m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_BACKWARD, strModelID, strPanelID_1, strPanelID_2);
							}
			
						}
					}
				}

			}

			m_fnSetStep(SEQ_B_GRAB_READY_RES);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_REQ_CHECK."), m_nStageID);

		}
		else
		{
			m_fnRetryStep();
		}

		break;


	case SEQ_B_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())		//if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID])	//2015.05.13 edit
		{
			//2015.05.13 edit s-
			//m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		// 수신 flag 리셋
			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;
						}
						
					}
				}
			}
			//2015.05.13 edit e-

			m_nRetryCount = 0;

			{
				//Sleep(100);
				if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				{
					//Sleep(10000);	//2016.05.30 test
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK_BWD, 1);
					lArkResult = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK_BWD);
					G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE"), m_nStageID);
					if (lArkResult == FALSE)
					{
						G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC ARK WRITE Error."), m_nStageID);
						lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK_BWD, 1);
					}
				}

				if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}

				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_ACK."), m_nStageID);

				m_fnSetStep(SEQ_B_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip

			}

			m_fnSetStep(SEQ_B_GRAB_END_CHECK);
			G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_READY_RES."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_B_GRAB_READY_RES"), 10000))
			{
				//G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_VISION_NOREADY"), m_nStageID);
				
				/*if(lRet == -1)	
				{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
				}*/
			}
		}
		break;

	case SEQ_B_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())	//if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID])	//2015.05.13 edit
		{
			m_nRetryCount = 0;

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{

						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_bGrabEndFlag[j] = FALSE;
						}
					}
				}
			}
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK_BWD, 0);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END_BWD, 1);
			}

			if(lRet == -1)	
			{
				G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			m_fnSetStep(SEQ_B_RESULT_PROCESS);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_END_CHECK."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_B_GRAB_END_CHECK"), 20000))	//2017.02.16 10000->20000
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_END_CHECK TimeOut."), m_nStageID);
				G_AddLog_ALARM(_T("m_fnScanSeqBackward - SEQ_B_GRAB_END_CHECK, StageID = %d : %d"),m_nStageID, ERROR_VISION_GRAB_END);
				break;
			
				//if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit
				//{
				//	//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK, 0);
				//	//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_ACK_BWD, 0);
				//	//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END_BWD, 1);
				//}
				//if(lRet == -1)	
				//{
				//	G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				//	m_fnSetStep(SEQ_RESET);
				//	m_fnResetSEQ();
				//	break;
				//}
				//m_fnSetStep(SEQ_B_RESULT_PROCESS);
				//G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_B_GRAB_END_CHECK TimeOut."), m_nStageID);
			}
		}
		break;
	case SEQ_B_RESULT_PROCESS:

		//G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_RESULT_PROCESS."), m_nStageID);

		long lGrabEndOK;
		lGrabEndOK = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK_BWD);
		if(lGrabEndOK)
		{
			if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END_BWD, 0);
		}
		else
		{
			m_fnRetryStep();
			break;
		}

		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_B_RESULT_PROCESS."), m_nStageID);
		m_fnSetStep(SEQ_BACKWARD_END);
		break;
	case SEQ_BACKWARD_END:
		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{

					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				
				}
			}
		}
		m_bForwardScan = TRUE;
		m_fnResetSEQ(TRUE, TRUE);
		G_AddLog_STG_SEQ(3, m_nStageID, _T("[StageID = %d] SEQ_BACKWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_START);
		break;
	}
}

///////////////////monitor -e

void CThreadSeqPLC::m_fnScanSeqPacking()
{

#if 1
	long lRet = -1;
	CString strModelID;//, strPanelID_1, strPanelID_2;
	CString strReqName;
	BOOL bScanForward = FALSE;
	// 2015.12.15 by CWH 자료반환형 일치
	//BOOL bGrabReq = FALSE;
	long nGrabReq = 0;
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strNgFlagInfoPath;

	// 2015.12.15 by CWH 자료반환형 일치
	//BOOL bGrabEnd = FALSE;
	//BOOL bGrabResult = FALSE;
	long nGrabEnd = 0;
	long nGrabResult = 0;

	switch(m_fnGetStep())
	{
	case SEQ_FORWARD_START:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}

		}

		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 0);		// Grab Ack 신호 초기화
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_END, 0);		// Grab End  신호를 초기화한다.
		m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 0);	// 결과 신호를 초기화한다.

		G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ PC READY."), m_nStageID);
		m_fnSetStep(SEQ_F_GRAB_READY);
		break;
	case SEQ_F_GRAB_READY:			// PLC Map Write Forward & Grab Ready

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)		 //검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 STATE_READY
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
						m_pServerWoarkManager->m_nVisionState[j] = STATE_READY;
					}
				}
			}
		}

		if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
		{
			if( m_pDlgPLCSetup->m_fnGetDevice(ADDR_PACKING_PLC_ALIVE))
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_ALIVE_ACK, 1);			
			else
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_ALIVE_ACK, 0);		
				m_fnRetryStep();
				break;
			}
		}

		if(lRet == -1)	
		{
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
			m_fnSetStep(SEQ_RESET);
			m_fnResetSEQ();
			break;
		}

		G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY."), m_nStageID);

		m_fnSetStep(SEQ_F_GRAB_REQ_CHECK);

		break;
	case SEQ_F_GRAB_REQ_CHECK:		// PLC Map Read Grab REQ
		// 2015.12.15 by CWH - Get/Set 자료형 통일
		//bGrabReq = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PACKING_PLC_GRAB_REQUEST);
		nGrabReq = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PACKING_PLC_GRAB_REQUEST);
		if(nGrabReq == 1)		// PLC Grab REQ Check OK.
		{
			m_nRetryCount = 0;			
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PACKING_PLC_CUR_MODELID, LEN_MODEL_ID, strModelID);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PACKING_PANEL_ID, LEN_PANEL_ID, m_strPanelID_1);
			//m_pDlgPLCSetup->m_fnGetDeviceBlock(_T("D0720"), 5, m_strPanelID_1);
			m_fnCheckModelId(strModelID);  // 2015.10.15 add by ikm Packing 용 자동 레시피 변경 
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] [PANEL ID1 = %s] PLC GRAB REQUEST ON"),
				m_nStageID, m_strPanelID_1 );

			if(lRet == -1)	
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC READ Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			SYSTEMTIME systime;
			::GetLocalTime(&systime);
			if(m_strPanelID_1.IsEmpty())	// CellID가 없을경우
			{
				m_strPanelID_1.Format(_T("%04d%02d%02d"), systime.wHour, systime.wMinute, systime.wSecond);
			}
			//strModelID = _T("");
			//m_strPanelID_1 = _T("");
			strModelID.TrimRight();
			m_strPanelID_1.TrimRight();
			m_strPanelID_2.TrimRight();

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = m_strPanelID_1;
					m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = m_strPanelID_2;
				}

			}	

			if(m_strPanelID_1 == "" && m_strPanelID_2 == "")
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] READY REQ Error. %s, %s, %s"), m_nStageID, strModelID, m_strPanelID_1, m_strPanelID_2);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 RequestGrabReady
					{
						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j]){
							m_pServerWoarkManager->m_fnRequestGrabReady(j + 1, SCAN_DIR_FORWARD, strModelID, m_strPanelID_1, m_strPanelID_2);
						}

					}
				}
			}

			m_fnSetStep(SEQ_F_GRAB_READY_RES);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_REQ_CHECK."), m_nStageID);

		}
		else
		{
			m_fnRetryStep();
		}

		break;
	case SEQ_F_GRAB_READY_RES:		// Vision 의 Grab 준비요청에 대한 가능 여부 확인

		if(m_fnVisionReadyResCheck())	
		{

			for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
			{
				if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
				{
					for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 FALSE
					{
						if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j])
							m_pServerWoarkManager->m_bRecvReadyRes[j] = FALSE;		
					}
				}
			}

			m_nRetryCount = 0;		
			if(m_fnVisionReadyResTriggerSetErrorCheck())	
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] Trigger Set Error"), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}
			else
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 1);

				if(lRet == -1)	
				{
					G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}

				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_ACK."), m_nStageID);
				m_fnSetStep(SEQ_F_GRAB_END_CHECK);	// External Mode 라서 Grab Start 신호 Skip
			}			

			m_fnSetStep(SEQ_F_GRAB_END_CHECK);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY_RES."), m_nStageID);
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_READY_RES"), 10000))
			{

				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 1);
				m_fnSetStep(SEQ_F_GRAB_END_CHECK);
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_READY_RES."), m_nStageID);
				if(lRet == -1)	
				{
					G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}
			}
		}
		break;
	case SEQ_F_GRAB_END_CHECK:		// Vision Grab End Check

		if(m_fnVisionGrabEndFlagCheck())
		{
			m_nRetryCount = 0;
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 0);

			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_END, 1);		// 신호 Off 는 PLC 에서 해주다.

			if(lRet == -1)	
			{
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
				m_fnSetStep(SEQ_RESET);
				m_fnResetSEQ();
				break;
			}
			m_fnSetStep(SEQ_F_RESULT_PROCESS);
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK."), m_nStageID);		
		}
		else
		{
			if (!m_fnRetryCheck(_T("SEQ_F_GRAB_END_CHECK"), 10000))
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_ACK, 0);


				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_END, 1);
				m_fnSetStep(SEQ_F_RESULT_PROCESS);
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_GRAB_END_CHECK."), m_nStageID);
				if(lRet == -1)	
				{
					G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] PLC WRITE Error."), m_nStageID);
					m_fnSetStep(SEQ_RESET);
					m_fnResetSEQ();
					break;
				}	
			}
			else
			{
				m_fnRetryStep();
			}

		}
		break;

	case SEQ_F_RESULT_PROCESS:
		// 2015.12.15 by CWH - Get/Set 자료형 통일
		//bGrabEnd = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PACKING_PLC_GRAB_END);
		nGrabEnd = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PACKING_PLC_GRAB_END);
		if(nGrabEnd == 1)		// PLC Grab REQ Check OK.
		{
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_END, 0);			

			// 2015.12.14 Add by CWH for Packing wait sequence
			BOOL bUseNgFlag = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0);
			if (!bUseNgFlag)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 1);
				G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_RESULT_PROCESS."), m_nStageID);
			}
			m_fnSetStep(SEQ_F_RESULT_END_CHECK);
		}
		else
		{
			m_fnRetryStep();
		}
		break;

		// 2015.12.28 NG Flag 사용 시 Result Check Process 추가 - NG 일시정지 사용 시 NG Flag 기입 후 Result ON
	case SEQ_F_RESULT_END_CHECK:
		// 2015.12.15 by CWH - Get/Set 자료형 통일
		//bGrabResult =  m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PACKING_PLC_GRAB_RESULT);
		nGrabResult =  m_pDlgPLCSetup->m_fnGetDevice(ADDR_PACKING_PLC_GRAB_RESULT);
		if(nGrabResult == 1)		// PLC Grab Result OK.
		{
			G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_F_RESULT_END_CHECK."), m_nStageID);
			m_fnSetStep(SEQ_FORWARD_END);
		}
		else
		{
			m_fnRetryStep();
		}
		break;

	case SEQ_FORWARD_END:

		for(int i = 0; i < pFrame->m_stParamInfo.nCountStage; i++)	//검사 Stage 갯수만큼 검색
		{
			if (m_nStageID == pFrame->m_stStageInfo[i].nVisionNo[0] - 1) //현재 m_nStageID가 검사 Stage의 컨트롤 PC인가 확인
			{
				for(int j = m_nStageID; j < m_nStageID + pFrame->m_stStageInfo[i].nVisionCnt; j++)	//검사 Stage에 종속되어 있는 Vision PC 모두 ResetFlag
				{
					if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[j])
					{
						m_pServerWoarkManager->m_fnResetFlag(j + 1);
					}
				}
			}
		}

		m_fnResetSEQ(TRUE, TRUE);

		G_AddLog_SEQ(m_nStageID, _T("[StageID = %d] SEQ_FORWARD_END."), m_nStageID);
		m_fnSetStep(SEQ_FORWARD_START);
		break;
	}
#else
	//////////////////////////// Alive와 Grab Start 루틴구현 //////////////////////////////////
	if(m_bGrabEndProcess == TRUE)	// 2015.03.31 ljk
	{
		return;
	}

	// 2015.03.12 ljk
	if(::GetTickCount() - m_dwAliveTimeCount >= 1000)
	{
		CString strRet;
		// PLC의 Alive 상태를 확인한다.
		if(m_bAliveStart == TRUE)	// 2015.03.26 ljk
		{
			//	m_pDlgPLCSetup->m_fnGetDeviceBlock(_T("0700"), 1, strRet);
			if(m_pDlgPLCSetup->m_fnGetDevice(_T("D0700")) == 1L)
			{
				// PLC의 Alive 상태가 확인되면 PLC로 Check확인을 보낸다.
				m_pDlgPLCSetup->m_fnSetDevice(_T("D0750"), 1);

				m_bIsAlive = TRUE;
				m_nAliveValue = 0;	// 바뀐 상태로 설정

			}		
			else
			{
				// PLC의 Alive 상태가 확인되면 PLC로 Check확인을 보낸다.
				m_pDlgPLCSetup->m_fnSetDevice(_T("D0750"), 0L);
				m_bIsAlive = TRUE;	//FALSE;

				m_nAliveValue = 1;	// 바뀐상태로 설정
			}

			// 상태 설정 플래그 초기화
			m_pDlgPLCSetup->m_fnSetDevice(_T("D0760"), 0);
			m_pDlgPLCSetup->m_fnSetDevice(_T("D0761"), 0);	// Grab End  신호를 초기화한다.
			m_pDlgPLCSetup->m_fnSetDevice(_T("D0762"), 0);	// 결과 신호를 초기화한다.
			m_bAliveStart = FALSE;
		}
		else
		{
			if(m_pDlgPLCSetup->m_fnGetDevice(_T("D0700")) == m_nAliveValue)
			{
				// PLC의 Alive 상태가 확인되면 PLC로 Check확인을 보낸다.
				m_pDlgPLCSetup->m_fnSetDevice(_T("D0750"), m_nAliveValue);

				m_nAliveValue = m_nAliveValue>0?0:1;
				m_bIsAlive = TRUE;		
			}	
			else
			{
				m_bIsAlive = FALSE;
			}
		}

		// 측정시간 초기화
		m_dwAliveTimeCount = ::GetTickCount();
	}

	////////////////////////////////////// 2015.06.29 ljk /////////////////////////////////////
	if(m_bIsAlive == FALSE)
	{
		return;
	}

	if(m_pServerWoarkManager == NULL)
	{
		return;
	}

	if(m_pServerWoarkManager->m_stSockStatus.bVisionClient[m_nStageID] == FALSE)
	{
		return;
	}

	CMainFrame *pFrm  = (CMainFrame *)(AfxGetApp()->m_pMainWnd);
	pFrm->m_nStageNum = m_pDlgPLCSetup->m_fnGetDevice(_T("D0713"));	// 현재 상태가 상부인지 하부인지를 얻어온다.(Vision Check상태만 가능)

	//////////// Grab 시작 요청이 있는지 확인한다. ////////////
	if(m_pDlgPLCSetup->m_fnGetDevice(_T("D0710")))	// Grab요청이 있을경우
	{
		if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] == FALSE)	// Grab 시작 신호가 초기화 상태일경우
		{
			///////////////////////// CellID //////////////////////////
			if(!pFrm->m_bPLCManual)
			{
				pFrm->m_strCellID = _T("");
				if(m_pDlgPLCSetup->m_fnGetDeviceBlock(_T("D0720"), 5, pFrm->m_strCellID))	// 셀ID추출(자동 모드 일때만 갱신됨)
				{}		
				SYSTEMTIME systime;
				::GetLocalTime(&systime);
				if(pFrm->m_strCellID.IsEmpty())	// CellID가 없을경우
				{
					pFrm->m_strCellID.Format(_T("%04d%02d%02d"), systime.wHour, systime.wMinute, systime.wSecond);
				}
			}
			///////////////////////////////////////////////////////////

			CString strModelID	 = _T("");
			CString strPanelID_1 = _T("");
			CString strPanelID_2 = _T("");
			strModelID = _T("RCP001");
			strPanelID_1.Format(_T("PNL%03d_1"), ++m_nPanelID1);
			strPanelID_2.Format(_T("PNL%03d_2"), ++m_nPanelID2);

			m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strModelID   = strModelID;
			m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strPanelID_1 = strPanelID_1;
			m_pDlgPLCSetup->m_stPlcMapPLC[m_nStageID].strPanelID_2 = strPanelID_2;

			// 클라이언트로 Grab 시작 신호를 보낸다.
			if(pFrm->m_bIsGrabStartRequest == FALSE)	// 중복실행방지
			{
				m_pServerWoarkManager->m_fnRequestGrabReady(m_nStageID + 1, SCAN_DIR_FORWARD, strModelID, strPanelID_1, strPanelID_2);			
				pFrm->m_bIsGrabStartRequest = TRUE;
			}
		}
		else
		{
			if(pFrm->m_bIsGrabStartRequest == TRUE)
			{
				Sleep(10);
				m_pDlgPLCSetup->m_fnSetDevice(_T("D0760"), 1);	// Grab 시작 신호를 PLC로 보낸다.			
			}
		}
	}
	else												
	{
		if(m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] == TRUE)	// Grab 시작 신호가 변경되었을 경우
		{
			if(!m_pDlgPLCSetup->m_fnGetDevice(_T("D0710")))	// Grab요청이 있을경우
			{
				if(pFrm->m_bIsGrabStartRequest == TRUE)
				{
					Sleep(10);
					m_pDlgPLCSetup->m_fnSetDevice(_T("D0760"), 0);	// Grab 시작 신호를 초기화 한다.
					m_pServerWoarkManager->m_bRecvReadyRes[m_nStageID] = FALSE;		
					pFrm->m_bIsGrabStartRequest				    = FALSE;
				}
			}
		}
	}
	///////////////////////////////////////////////////////////

	////////////////////////////////// Grab Result 관련 ///////////////////////////////////////////
	// 2015.07.02 ljk
	if(m_pDlgPLCSetup->m_fnGetDevice(_T("D0711")))	// plc로 부터 Grab End 응답을 확인한다.
	{
		if(m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID] == TRUE)
		{
			Sleep(10);
			m_pDlgPLCSetup->m_fnSetDevice(_T("D0761"), 0);	// Grab End  신호를 초기화한다.
			m_pServerWoarkManager->m_bGrabEndFlag[m_nStageID] = FALSE;
		}
	}

	if(m_pDlgPLCSetup->m_fnGetDevice(_T("D0712")))	// plc로 부터 결과 데이터 응답을 확인한다.
	{
		if(m_pServerWoarkManager->m_bInspectEndFlag[m_nStageID] == TRUE)
		{
			// 임시로 추가(ng skip)
			Sleep(10);
			m_pDlgPLCSetup->m_fnSetDevice(_T("D0762"), 0);	// 결과 신호를 초기화한다.
			// plc로 NG데이터를 보낸다.
			Sleep(10);
			m_pDlgPLCSetup->m_fnSetDevice(_T("D00763"), 0);	//good(NG code)
			Sleep(10);
			m_pDlgPLCSetup->m_fnSetDevice(_T("D00764"), 0);	//good(NG code)

			m_pServerWoarkManager->m_bInspectEndFlag[m_nStageID] = FALSE;
		}
		if(pFrm->m_bIsEngineerModeMsg[m_nStageID] == TRUE)
		{
			pFrm->m_bIsEngineerModeMsg[m_nStageID] = FALSE;
		}
	}	
	////////////////////////////////////////////////////////////////////////////////////////////////
#endif
}


void CThreadSeqPLC::m_fnCheckAlivePLC()
{
	long lRet = 0;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if(m_nStageID + 1 == pFrame->m_stStageInfo[0].nVisionNo[0])	//if(m_nStageID + 1 == VISION_PC_1)	//2015.05.13 edit 
	{
		if( m_pDlgPLCSetup->m_fnGetDevice(ADDR_PACKING_PLC_ALIVE))
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_ALIVE_ACK, 1);			
		else
		{
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PACKING_PC_ALIVE_ACK, 0);		
		}
	}
}

// 2015.10.15 add by ikm Packing 용 자동 레시피 변경  S
void CThreadSeqPLC::m_fnCheckModelId(CString strModelID)
{
	CStdioFile cstFile;
	CString strRootPath;
	BOOL bExist = FALSE;
	strRootPath.Format(_T("%s%s"), ROOT_PATH, MODEL_LIST_PATH);

	if (!cstFile.Open(strRootPath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnCheckModelId - File Open Error %s : %d"), strRootPath, GetLastError());
		return;
	}

	CString strLine, strItem, strNewLine;

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		/*	strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));*/
		strItem.Trim();

		if(strItem.Find(strModelID) != -1)
		{		
			bExist = TRUE;
			break;
		}
	}

	cstFile.Close();

	if (!bExist)
	{
		if (!cstFile.Open(strRootPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite |CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("m_fnCheckModelId - Empty File Open Error %s : %d"), strRootPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();				
		cstFile.WriteString(strModelID + _T("\n") );		
		cstFile.Close();
	}

}

// 2015.10.15 add by ikm Packing 용 자동 레시피 변경  E
