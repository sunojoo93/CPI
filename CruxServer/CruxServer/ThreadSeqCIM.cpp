#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "StdAfx.h"
#include "ThreadSeqCIM.h"
#include "MainFrm.h"
#include "ViewAuto.h"

CThreadSeqCIM::CThreadSeqCIM(void)
{
	m_nRetryCount = 0;
	m_nRecipeCheck = 0;
	m_nProcCheck = 0;
	m_lPlcVal = 0;
	m_strPlcVal = _T("");
	m_nEqpModel = 0, m_nEqpSizeType = 0;
	m_nCimSeqCnt = 0;
	stCimRcpInfo = new CIM_RECIPEINFO();
	memset(stCimRcpInfo,0X00,sizeof(stCimRcpInfo));
	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());	
	m_nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	m_nEqpSizeType = pFrame->m_fnEqpSizeType();

//	m_nCntRecipeDn = 0;
	//m_nCntRecipeUp = 0;
	//m_nCntRecipeChg = 0;
}

CThreadSeqCIM::~CThreadSeqCIM(void)
{
	delete stCimRcpInfo;
}

int CThreadSeqCIM::m_fnInitThread(int nEqpType,int nSeqCnt , CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager, CSockManager* pSockManager)
{	
	m_pDlgPLCSetup = pDlgPLCSetup;
	m_pDataManager = pDataManager;
	m_pSockManager = pSockManager;

	m_nCimSeqCnt = m_fnSetCimStep(nSeqCnt);
	m_fnResetSEQ(TRUE, TRUE);
	return CSequenceManager::m_fnStartScenario(TRUE);
}

void CThreadSeqCIM::m_fnDeInitThread()
{
	CSequenceManager::m_fnDeInit();
}

void CThreadSeqCIM::m_fnResetSEQ(BOOL bDir, BOOL bFlag, BOOL bErrReset)
{	
	long lRet = -1;

	if(m_nEqpSizeType == EQP_MONITOR_SIZE){
		if(m_nCimSeqCnt == SEQ_RCP_EDIT_CHECK) 
		{
			m_fnSetStep(SEQ_RCP_EDIT_CHECK);
			m_nRetryCount = 0;
			return;
		}
	}
	else{
		if(m_nCimSeqCnt == SEQ_RCP_DN_CHECK) 
		{
			m_fnSetStep(SEQ_RCP_DN_CHECK);
			m_nRetryCount = 0;
			return;
		}
	}
	switch(m_nCimSeqCnt)
	{
	case SEQ_RCP_UP_CHECK:
		m_fnSetStep(SEQ_RCP_UP_CHECK);
		break;
	case SEQ_RCP_CHANGE_CHECK:
		m_fnSetStep(SEQ_RCP_CHANGE_CHECK);
		break;
	case SEQ_PROC_INFO_CHECK:
		m_fnSetStep(SEQ_PROC_INFO_CHECK);
		break;
	case SEQ_PROC_DATA_CHECK:
		//m_fnSetStep(SEQ_PROC_DATA_CHECK);
		break;	
	}
	/*m_nCntRecipeDn = SEQ_RCP_DN_CHECK;
	m_nCntRecipeUp = SEQ_RCP_UP_CHECK;
	m_nCntRecipeChg = SEQ_RCP_CHANGE_CHECK;*/
	//m_fnSetStep(SEQ_RCP_DN_CHECK);
	//m_fnSetStep(SEQ_RCP_UP_CHECK);
	//m_fnSetStep(SEQ_PROC_INFO_CHECK);
	//m_fnSetStep(SEQ_PROC_DATA_CHECK);

	m_nRetryCount = 0;
}

void CThreadSeqCIM::m_fnCheck()
{
	m_nRecipeCheck++;
	m_nProcCheck++;

	if(m_nRecipeCheck * SLEEP_COUNT > 1000)		// 1초에 한번씩 Check
	{
		m_nRecipeCheck = 0;

		//Chung Du Comment
		//if(m_nEqpSizeType == EQP_MONITOR_SIZE)
		//	m_fnRecipeEditSeq();
		//else
		//	m_fnRecipeDnSeq();
		m_fnRecipeUpSeq();	//Recipe Upload 2018.01.20
		//m_fnRecipeChangeSeq();
	}

	if(m_nProcCheck * SLEEP_COUNT > 10)		// 1초에 한번씩 Check
	{
		m_nProcCheck = 0;
		
		//m_fnPlcInitRest();
		//m_fnProcessInfoSeq(); //Chung Du Comment
	}

	m_fnRetryStep();
}

void CThreadSeqCIM::m_fnRetryCheck(CString strMsg, int nTimeout)
{
	m_nRetryCount++;

	if(m_nRetryCount * SLEEP_COUNT < nTimeout * 10000000)		// Timeout 확인
		m_fnRetryStep();
	else
	{
		m_nRetryCount = 0;
		G_AddLog_SEQ(0, _T("CIM I/F Timeout Error %s."), strMsg);
		m_fnSetStep(SEQ_RESET);
		m_fnResetSEQ();
	}
}

void CThreadSeqCIM::m_fnRecipeEditSeq()
{
	long lRet = -1;
	int nPubNo = 0;
	CString strRecipeNo, strFSide, strFCoax;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	switch(m_fnGetStep())
	{
	case SEQ_RCP_EDIT_CHECK:
// 		if (pFrame->m_pSockManager->m_bRcpEditFlag)
// 		{
// 			m_fnSetStep(SEQ_RCP_EDIT_CHECK);
// 		}

		if (pFrame->m_pSockManager->m_bRcpEditFlag)
			m_fnSetStep(SEQ_RCP_EDIT_DATA_WRITE);		

		break;
	case SEQ_RCP_EDIT_DATA_WRITE:
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_CMD_NO, 101);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_RCP_NO, _ttoi(strRecipeNo));
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_0,  stCimRcpInfo->lEqData[0]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_1,  stCimRcpInfo->lEqData[1]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_2,  stCimRcpInfo->lEqData[2]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_3,  stCimRcpInfo->lEqData[3]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_4,  stCimRcpInfo->lEqData[4]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_5,  stCimRcpInfo->lEqData[5]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_6,  stCimRcpInfo->lEqData[6]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_7,  stCimRcpInfo->lEqData[7]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_8,  stCimRcpInfo->lEqData[8]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_DATA_9,  stCimRcpInfo->lEqData[9]);
		m_fnSetStep(SEQ_RCP_EDIT_PUB_PLUS);
		break;
	case SEQ_RCP_EDIT_PUB_PLUS:
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_EDIT_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
		nPubNo = lRet + 1;
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_EDIT_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set
		m_fnSetStep(SEQ_RCP_EDIT_END);
		break;
	case SEQ_RCP_EDIT_END:

		pFrame->m_pSockManager->m_bRcpEditFlag = FALSE;
		m_fnSetStep(SEQ_RCP_EDIT_CHECK);
		break;
	default:
		break;
	}
}

void CThreadSeqCIM::m_fnRecipeUpSeq()
{
	long lRet = -1, lTmpVal = -1;
	CString strTemp;
	CString strRecipeNo, strFSide, strFCoax;
	int nPubNo = 0;

	switch(m_fnGetStep())
	{
	case SEQ_RCP_UP_CHECK:

		lTmpVal = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_UP_EQ_RECV_NO);
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_UP_CIM_PUB_NO);
	
		//if (lTmpVal > 0 && lTmpVal != lRet )	
		if (lTmpVal > -1 && lTmpVal != lRet )	
			m_fnSetStep(SEQ_RCP_UP_DATA_REQ);		
		break;
	case SEQ_RCP_UP_DATA_REQ:
		//todo.. 검사 PC로 부터 레시피 정보를 받아온다.
		
		m_fnSetStep(SEQ_RCP_UP_DATA_WRITE);
		break;
	case SEQ_RCP_UP_DATA_WRITE:
	
		stCimRcpInfo->lEqData[0] = 700;
		stCimRcpInfo->lEqData[1] = 700;
		stCimRcpInfo->lEqData[2] = 700;
		stCimRcpInfo->lEqData[3] = 300;
		stCimRcpInfo->lEqData[4] = 300;
		stCimRcpInfo->lEqData[5] = 200;
		stCimRcpInfo->lEqData[6] = 200;
		stCimRcpInfo->lEqData[7] = 200;
	
		// Recipe Body Upload 실시
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_CMD_NO, 100);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_0, stCimRcpInfo->lEqData[0]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_1, stCimRcpInfo->lEqData[1]);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_2, stCimRcpInfo->lEqData[2]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_3, stCimRcpInfo->lEqData[3]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_4, stCimRcpInfo->lEqData[4]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_5, stCimRcpInfo->lEqData[5]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_6, stCimRcpInfo->lEqData[6]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_7, stCimRcpInfo->lEqData[7]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_8, stCimRcpInfo->lEqData[8]);
		//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_DATA_9, stCimRcpInfo->lEqData[9]);
		//memset(stCimRcpInfo,0X00,sizeof(stCimRcpInfo));
		m_fnSetStep(SEQ_RCP_UP_END);
		break;
	case SEQ_RCP_UP_END:
		// ACK CODE 와 Receive No Set
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_UP_CIM_PUB_NO);
		nPubNo = lRet;
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_RECV_NO, nPubNo);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_UP_EQ_ACK_CODE, TRUE);
		m_fnSetStep(SEQ_RCP_UP_CHECK);
		break;
	default:
		break;
	}
}

void CThreadSeqCIM::m_fnRecipeDnSeq()
{
	long lRet = -1, lTmpVal = -1;
	int nPubNo = 0;
	CString strRecipeNo, strFSide, strFCoax, strTemp;
	long lFSide, lFCoax, lRSide, lRCoax, lTrans1, lTrans2, lPADPos, lROICount;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	switch(m_fnGetStep())
	{
	case SEQ_RCP_DN_CHECK:
		lTmpVal = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_EQ_RECV_NO);
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_PUB_NO);

		if (lTmpVal != -1 && lTmpVal != lRet ) 
			m_fnSetStep(SEQ_RCP_DN_DATA_READ);
		break;
	case SEQ_RCP_DN_DATA_READ:
		// Recipe Body Download 실시
		stCimRcpInfo->lEqData[0] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_0);
		stCimRcpInfo->lEqData[1] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_1);
		stCimRcpInfo->lEqData[2] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_2);
		stCimRcpInfo->lEqData[3]=  m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_3);
		stCimRcpInfo->lEqData[4] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_4);
		stCimRcpInfo->lEqData[5] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_5);
		stCimRcpInfo->lEqData[6] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_6);
		stCimRcpInfo->lEqData[7] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_7);
		stCimRcpInfo->lEqData[8] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_8);
		stCimRcpInfo->lEqData[9] = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_DATA_9);
		m_fnSetStep(SEQ_RCP_DN_DATA_SEND);
		break;
	case SEQ_RCP_DN_DATA_SEND:
		//todo.. 검사 PC에 레시피 정보를 전송한다.
		m_fnSetStep(SEQ_RCP_DN_END);
		break;
	case SEQ_RCP_DN_END:
		// ACK CODE 와 Receive No Set
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_DOWN_EQ_CMD_NO, 105);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_DOWN_EQ_ACK_CODE, TRUE);
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_RCP_DOWN_CIM_PUB_NO);
		nPubNo = lRet;
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_RCP_DOWN_EQ_RECV_NO, nPubNo);
		m_fnSetStep(SEQ_RCP_DN_CHECK);
		break;
	default:
		break;
	}
}

void CThreadSeqCIM::m_fnProcessInfoSeq()
{
	long lRet = -1;
	int nPubNo = 0;
	CString strTemp;
	CString stStepID, strFlagArray, strFlagCF, strFlagCell, strFlagSLMOD;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	if (m_nEqpSizeType != EQP_LARGE_SIZE && m_nEqpModel ==  EQP_TYPE_AVL)
	{
		return;
	}

	//lRet = m_pDlgPLCSetup->m_fnSetDeviceBlock(ADDR_PROC_INFO_EQ_PNL_ID, 20, _T("PANELID_001"));
	switch(m_fnGetStep())
	{
	case SEQ_PROC_INFO_CHECK:
		 // 모니터에서 forward 일때만 process info data 송신하도록.
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);
// 		if(nEqpSizeType == EQP_MONITOR_SIZE)
// 		{
// 			BOOL bCheck = m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_BACKWARD_CHECK);
// 			
// 			if (lRet != -1 && lRet && !bCheck)
// 				m_fnSetStep(SEQ_PROC_INFO_REQ_1);
// 		}
// 		else
		{
			if (lRet != -1 && lRet) 
				m_fnSetStep(SEQ_PROC_INFO_REQ_1);
		}		
		break;
	case SEQ_PROC_INFO_REQ_1:
		lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, m_strPanelID1);

		if(m_strPanelID1 == "")
		{
			//G_AddLog(3, 2, _T("Panel ID #1 Empty!"));
			//m_fnSetStep(SEQ_PROC_INFO_END);
			m_fnRetryCheck(_T("SEQ_PROC_INFO_REQ_1"), 10);
			break;
		}

		lRet = m_pDlgPLCSetup->m_fnSetDeviceBlock(ADDR_PROC_INFO_EQ_PNL_ID_0, 20, m_strPanelID1);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROC_INFO_EQ_CMD_NO, 110);
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_EQ_PUB_NO);	// CIM 의 Receive No 을 확인
		nPubNo = lRet + 1;
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROC_INFO_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

		m_fnSetStep(SEQ_PROC_INFO_RECV_1);
		break;
	case SEQ_PROC_INFO_RECV_1:
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
		nPubNo = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_EQ_PUB_NO);
		if(lRet == nPubNo)
		{
			// Ack Code 확인 필요???

			// Step ID, STFLAGARRAY, STFLAGCF, STFLAGCELL, STFLAGSLMOD
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, stStepID);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGARRAY, 26, strFlagArray);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGCF, 26, strFlagCF);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGCELL, 26, strFlagCell);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGSLMOD, 26, strFlagSLMOD);

			if (m_nEqpSizeType == EQP_SMALL_SIZE)
				m_fnSetStep(SEQ_PROC_INFO_REQ_2);
			else
				m_fnSetStep(SEQ_PROC_INFO_END);

 		}
 		else
 			m_fnRetryCheck(_T("SEQ_PROC_INFO_RECV_1"), 20);

		break;
	case SEQ_PROC_INFO_REQ_2:
		lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, m_strPanelID2);

		if(m_strPanelID2 == "")
		{
			//G_AddLog(3, 2, _T("Panel ID #2 Empty!"));
			//m_fnSetStep(SEQ_PROC_INFO_END);
			m_fnRetryCheck(_T("SEQ_PROC_INFO_REQ_2"), 10);

			break;
		}

// 		if(strLastPanelID2 == m_strPanelID2)
// 		{
// 			m_fnSetStep(SEQ_PROC_INFO_END);
// 			break;
// 		}

		lRet = m_pDlgPLCSetup->m_fnSetDeviceBlock(ADDR_PROC_INFO_EQ_PNL_ID_0, 20, m_strPanelID2);
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROC_INFO_EQ_CMD_NO, 110);
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_EQ_PUB_NO);	// CIM 의 Receive No 을 확인
		nPubNo = lRet + 1;
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROC_INFO_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

		m_fnSetStep(SEQ_PROC_INFO_RECV_2);
		break;
	case SEQ_PROC_INFO_RECV_2:
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_CIM_RECV_NO);	// CIM 의 Receive No 을 확인
		nPubNo = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PROC_INFO_EQ_PUB_NO);
		if(lRet == nPubNo)
		{
			// Ack Code 확인 필요???

			// Step ID, STFLAGARRAY, STFLAGCF, STFLAGCELL, STFLAGSLMOD
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, stStepID);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGARRAY, 26, strFlagArray);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGCF, 26, strFlagCF);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGCELL, 26, strFlagCell);
			lRet = m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STFLAGSLMOD, 26, strFlagSLMOD);

			m_fnSetStep(SEQ_PROC_INFO_END);
		}
		else
			m_fnRetryCheck(_T("SEQ_PROC_INFO_RECV_2"), 20);

		break;
	case SEQ_PROC_INFO_END:

		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);
		if (lRet == FALSE)
		{
			m_fnSetStep(SEQ_PROC_INFO_CHECK);
		}
		else 
			m_fnRetryCheck(_T("SEQ_PROC_INFO_END"), 5);
		break;
	default:
		break;
	}
}

void CThreadSeqCIM::m_fnRecipeChangeSeq()
{
	long lRet = -1;
	int RecipNo = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	switch(m_fnGetStep())
	{
	case SEQ_RCP_CHANGE_CHECK:
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_CUR_RCP_CHG_REQ);
		if (lRet != -1 && lRet) m_fnSetStep(SEQ_RCP_CHANGE_NO);		
		break;

	case SEQ_RCP_CHANGE_NO:
		 
			if (m_nEqpSizeType == EQP_LARGE_SIZE)
			{
				if(m_nEqpModel == 0)	RecipNo	= m_pDlgPLCSetup->m_fnGetDevice(_T("W414"));
				if(m_nEqpModel == 1)	RecipNo = m_pDlgPLCSetup->m_fnGetDevice(_T("W416"));
			}
			else if(m_nEqpSizeType == EQP_MONITOR_SIZE)
			{
				if(m_nEqpModel == 0)	RecipNo	= m_pDlgPLCSetup->m_fnGetDevice(_T("W200"));
				if(m_nEqpModel == 1)	RecipNo = m_pDlgPLCSetup->m_fnGetDevice(_T("W200"));
			}
			else
			{
				if(m_nEqpModel == 0)	RecipNo = m_pDlgPLCSetup->m_fnGetDevice(_T("W8014"));
				if(m_nEqpModel == 1)	RecipNo = m_pDlgPLCSetup->m_fnGetDevice(_T("W8016"));
			}

		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_CUR_EQP_RCP_NO, RecipNo);	
		m_fnSetStep(SEQ_RCP_CHANGE_SEND);	
		break;

	case SEQ_RCP_CHANGE_SEND:
		//todo.. 검사 PC에 레시피 체인지 넘버를 전송한다. (소켓 통신)
		m_fnSetStep(SEQ_RCP_CHANGE_RECV);	
		break;

	case SEQ_RCP_CHANGE_RECV:
		lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_CUR_RCP_CHG_ACK, TRUE);
		m_fnSetStep(SEQ_RCP_CHANGE_END);
		break;

	case SEQ_RCP_CHANGE_END:
		lRet = m_pDlgPLCSetup->m_fnGetDevice(ADDR_CUR_RCP_CHG_REQ);
		if (!lRet) 
		{
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_CUR_RCP_CHG_ACK, FALSE);
			m_fnSetStep(SEQ_RCP_CHANGE_CHECK);
		}
		break;
	default:
		break;
	}
}

int CThreadSeqCIM::m_fnSetCimStep(int nStepVal)
{
	int nCnt = 0;
	switch(nStepVal)
	{
	case 1:
		nCnt = SEQ_RCP_EDIT_CHECK;
		break;
	case 2:
		nCnt = SEQ_RCP_DN_CHECK;
		break;
	case 3:
		nCnt = SEQ_RCP_UP_CHECK;
		break;
	case 4:
		nCnt = SEQ_RCP_CHANGE_CHECK;
		break;
	case 5:
		nCnt = SEQ_PROC_INFO_CHECK;
		break;
	case 6:
		nCnt = SEQ_PROC_DATA_CHECK;
		break;
	}

	return nCnt;
}

int CThreadSeqCIM::m_fnSetCimCheckList(CString strDefJudge)
{
	int nRet, nChkNo = 0;
	int nDefJudge = 0;

	nDefJudge = _ttoi(strDefJudge);
	switch(nDefJudge)
	{
	case CONTACTFAIL:	case ITORESISTILEGAL:
		nRet = (int)pow(double(2),double(32)); 
		break;
	case MURA:
		nRet = (int)pow(double(2),double(32)); 
		break;
	case CFPOLMEASUREFAIL:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case TFTPOLMEASUREFAIL:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFPOLSUBSTANCE:
		nChkNo = (int)pow(double(2),double(35)); 
		nRet = nChkNo;
		nChkNo = (int)pow(double(2),double(32)); 
		nRet += nChkNo;
		break;
	case TFTPOLBUBBLE:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case TFTPOLSCRATCH:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case CFPOLBUBBLE:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFPOLSCRATCH:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFSCRATCH:	case	TFTSCRATCH	:	case	TFTWHITEBLUR:	case	CFWHITEBLUR	:	
	case ITOPEEL :	case	WANESBROKEN	:	case	SOCKETSCRACH:	case	TFTDIMPLEPOINT:
	case CFDIMPLEPOINT:	case	TFTEDGEILEGAL:	case	CFEDGEILEGAL:	case	TFTDIMPLELINE:
	case CFDIMPLELINE : case	SOCKETBURN:	case	BURR: case	CFMEASUREFAIL:	case	TFTMEASUREFAIL:
	case CFBURR : case	TFTBURR:	case	CFBROKEN: case	TFTBROKEN:	case	CFCORNERBROKEN:		case	TFTCORNERBROKEN:	//2015.07.13
	case CFCELLBROKEN : case	TFTCELLBROKEN:	//2015.08.26
	case CFPIMPLEPOINT	: case TFTPIMPLEPOINT : case CFPIMPLELINE :	case TFTPIMPLELINE : case CFPIMPLECIRCLE :
	case CFWAVEPATTERN	: case ETCLINE		  : /*case ETCPOINT	  : // Comment KYH01 정의 중복으로 빌드오류 임시 주석*/	
	case CFPROTECTION	: case TFTPROTECTION	:
	case BGRADECHECK	: case CGRADECHECK	:
		nRet = (int)pow(double(2),double(1)); 
		break;
	default:
		nRet = 0;
	}
	
	return nRet;
}
//
//int CThreadSeqCIM::m_fnGetRcpDnStep()
//{
//	return m_nCntRecipeDn;
//}
//
//
//int CThreadSeqCIM::m_fnGetRcpUpStep()
//{
//	return m_nCntRecipeUp;
//}
//
//int CThreadSeqCIM::m_fnGetRcpChgStep()
//{
//	return m_nCntRecipeChg;
//}
//
//
//void CThreadSeqCIM::m_fnSetRcpDnStep(int nRcpCnt)
//{
//	m_nCntRecipeChg = nRcpCnt;
//}
//
//
//void CThreadSeqCIM::m_fnSetRcpUpStep(int nRcpCnt)
//{
//		m_nCntRecipeUp = nRcpCnt;
//}
//
//void CThreadSeqCIM::m_fnSetRcpChgStep(int nRcpCnt)
//{
//		m_nCntRecipeChg = nRcpCnt;
//}