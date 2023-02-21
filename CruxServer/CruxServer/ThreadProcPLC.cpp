
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "CruxServer.h"
#include "ThreadProcPLC.h"
#include "MainFrm.h"
#include "ViewAuto.h"
#include "ViewSetup.h"
#include "F_GRDAE_Dialog.h"


// CThreadProcPLC
CThreadProcPLC::CThreadProcPLC(void)
{
	m_nSeqCnt = 0;
	m_nNgFlagCheck = 0;
	m_nRetryCount = 0;
	m_nRetryClassifyCnt = 0;
	m_nUsingFlag = 1;
	m_nUsingPlcAlarmResetFlag = 1;
	m_strPanelID1 = _T("");
	m_strPanelID2 = _T("");
	m_bULDStage_R = FALSE;
	m_bULDStage_F = FALSE;
	m_hThreadSaveModelId = NULL;
	m_pTreadCimProc = NULL;
	m_pTreadCimProc = (CThreadProcCIM*)AfxBeginThread(RUNTIME_CLASS(CThreadProcCIM), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);	
	m_pTreadCimProc->ResumeThread();
}

CThreadProcPLC::~CThreadProcPLC(void)
{
	m_hThreadSaveModelId = NULL;	
	TerminateThread(m_pTreadCimProc->m_hThreadCim, 0);
	if(NULL != m_pTreadCimProc)
	{
		::WaitForSingleObject(m_pTreadCimProc->m_hThreadCim,INFINITE);
		delete (CWinThread *)m_pTreadCimProc;
		m_pTreadCimProc = NULL;
	}
}

int CThreadProcPLC::m_fnInitThread(int nSeqCnt, CDlgPLCSetup* pDlgPLCSetup, CDataManager* pDataManager)
{	
	m_pDlgPLCSetup = pDlgPLCSetup;
	m_pDataManager = pDataManager;	
	m_nSeqCnt = nSeqCnt;
	m_fnResetSEQ(TRUE, TRUE);
	return CSequenceManager::m_fnStartScenario(TRUE);
}

void CThreadProcPLC::m_fnDeInitThread()
{
	CSequenceManager::m_fnDeInit();
}

void CThreadProcPLC::m_fnResetSEQ(BOOL bDir, BOOL bFlag, BOOL bErrReset)
{	
	switch(m_nSeqCnt)
	{
	case SEQ_PROC_PLC_FRONT_NG_FLAG:
		m_fnSetStep(SEQ_STEP_FRONT_NG_FLAG_CHECK);
		break;
	case SEQ_PROC_PLC_GRAB_RETRY_STG1:	
	case SEQ_PROC_PLC_GRAB_RETRY_STG2:		
	case SEQ_PROC_PLC_GRAB_RETRY_STG3:
		m_fnSetStep(SEQ_STEP_GRAB_RETRY_INIT_REQ_CHECK);
		break;
	case SEQ_PROC_PLC_RCP_CHECK:
		m_fnSetStep(SEQ_STEP_PLC_RCP_CHECK);
		break;
	case SEQ_PROC_PLC_INIT_RESET:
		m_fnSetStep(SEQ_STEP_PLC_INIT_RESET);
		break;
	case SEQ_PROC_PLC_SIG_CHECK:
		m_fnSetStep(SEQ_STEP_PLC_SIG_CHECK);
		break;
	case SEQ_PROC_PLC_SAV_MODID:
		m_fnSetStep(SEQ_STEP_PLC_SAV_MODID);
		break;
	case SEQ_PROC_PLC_REAR_NG_FLAG:
		m_fnSetStep(SEQ_STEP_REAR_NG_FLAG_CHECK);
		break;
	case SEQ_PROC_PLC_ALARM_RESET:
		m_fnSetStep(SEQ_STEP_PLC_ALARM_RESET);
		break;
	}	

	m_nRetryCount = 0;
}

void CThreadProcPLC::m_fnCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_nSeqCnt)
	{
	case SEQ_PROC_PLC_FRONT_NG_FLAG:
		m_fnProcCheckFrontNgFlag();
		break;
	case SEQ_PROC_PLC_GRAB_RETRY_STG1:		
	case SEQ_PROC_PLC_GRAB_RETRY_STG2:		
	case SEQ_PROC_PLC_GRAB_RETRY_STG3:
		//m_fnGrabRetryCheck();	//Chung Du Comment
		break;
	case SEQ_PROC_PLC_RCP_CHECK:
		//m_fnPlcRecipeUpdateCheck();		//Chung Du Comment	//2015.11.05
		break;
	case SEQ_PROC_PLC_INIT_RESET:
		m_fnPlcInitRest();
		break;
	case SEQ_PROC_PLC_SIG_CHECK:
		m_fnSignalCheck();
		break;
	case SEQ_PROC_PLC_SAV_MODID:
		if(pFrame->m_stParamInfo.nEqpObject == P_PROJECT) m_fnPlcSaveModelID();
		break;
	case SEQ_PROC_PLC_REAR_NG_FLAG:
		//m_fnProcCheckRearNgFlag();	//Chung Du Comment
		break;
	case SEQ_PROC_PLC_ALARM_RESET:
		m_fnPlcAlarmReset();
		break;
	case SEQ_PROC_PLC_STOP:
		CViewAuto * pVeiw = (CViewAuto *)pFrame->m_fnGetViewPtr(eFORM_Setup);
		if (m_pDlgPLCSetup->m_fnGetDevice2(_T("M29")))
		{			
			((CGxStaticEx*)pVeiw->m_pDlgPanelJudge->GetDlgItem(IDC_GXBTN_RESET_ALARM2))->EnableWindow(FALSE);	
		}
		else
		{
			((CGxStaticEx*)pVeiw->m_pDlgPanelJudge->GetDlgItem(IDC_GXBTN_RESET_ALARM2))->EnableWindow(TRUE);	
		}
		break;
	}	
}
BOOL  CThreadProcPLC::m_fnRetryCheck(CString strMsg, int nTimeout)
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->GetMainWnd();

	m_nRetryCount++;
	m_fnRetryStep();

	if(m_nRetryCount * SLEEP_COUNT < nTimeout)
	{
		Sleep(20);
		
		return TRUE;
	}
	else
	{
		m_nRetryCount = 0;
		G_AddLog_SEQ(0, _T("SEQ_STEP Retry %s."), strMsg);



		return FALSE;
	}
}

void CThreadProcPLC::m_fnSignalCheck()
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	switch(m_fnGetStep())
	{
	case SEQ_STEP_PLC_SIG_CHECK:

		if ( pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )//|| pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
		{
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, _T("0"));
			pFrame->m_DataManager.m_bUseSemaphore = 0;
			m_fnSetStep(SEQ_STEP_PLC_SIG_END);
			break;
		}
		if ( pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0))
		{
			long lConFlag1,lConFlag2,lConFlag3 = 0; 
			lConFlag1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_CON1);
			lConFlag2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_CON2);
			lConFlag3 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_CON3);

		if (lConFlag1 == 0 || lConFlag2 == 0 || lConFlag3 == 0)
			{
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, _T("1"));
				pFrame->m_DataManager.m_bUseSemaphore = 1;
			}
			else
			{
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, _T("0"));
				pFrame->m_DataManager.m_bUseSemaphore = 0;
			}
		}
		else
		{
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, _T("0"));
			pFrame->m_DataManager.m_bUseSemaphore = 0;
		}
		m_fnSetStep(SEQ_STEP_PLC_SIG_END);
		break;
	case SEQ_STEP_PLC_SIG_END:
		m_fnSetStep(SEQ_STEP_PLC_SIG_CHECK);
		break;
	}
}

void CThreadProcPLC::m_fnPlcRecipeUpdateCheck()
{
	G_AddLog(3, 3, _T("m_fnPlcRecipeUpdateCheck() - Start"));
	CString strTempRecipe, strNo, strPLCRecipeADDr;
	TCHAR tszSection[255];

	long nRecipeUpdate  = -1, lRet = -1, nChgRecipeNo = -1;
	CString strModelName = _T(""), strPlcData = _T("");
	int nCurModel = 0, nCnt = 0;
	BOOL bRet = FALSE;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	switch(m_fnGetStep())
	{
	case SEQ_STEP_PLC_RCP_CHECK:
		{	
			nRecipeUpdate = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_REQ);

			if (nRecipeUpdate == OK)
			{
				nChgRecipeNo = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_MODEL_NO);

				int nEqpSize = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
				if(nEqpSize == EQP_MONITOR_SIZE || nEqpSize == EQP_LARGE_SIZE)
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlockModel(_T("ZR0000"), PLC_MODEL_READ_PARAM_SIZE, strPlcData);
				else
					lRet = m_pDlgPLCSetup->m_fnGetDeviceBlockModel(_T("ZR000100"), PLC_MODEL_READ_PARAM_SIZE, strPlcData);

				nCurModel = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_CURRENT_RECIPE_NO);
				strModelName =  pFrame->m_DataManager.m_fnGetModelName(nCurModel);
				strModelName.Trim();
				if(strModelName != _T("") && strPlcData != _T("") && nCurModel !=0)
				{
					bRet = pFrame->m_DataManager.m_fnWriteModel(nCurModel, strModelName, strPlcData);
					if(bRet)
					{
						nRecipeUpdate = m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_MODEL_SAVE_COMPLETE, TRUE);
						if(nChgRecipeNo != -1)
							m_pDlgPLCSetup->m_fnSetDevice(ADDR_PLC_MODEL_CHANGE_REQ_NO_INFO, nChgRecipeNo);
						Sleep(500);
						while(TRUE)
						{
							nRecipeUpdate = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_MODEL_CHANGE_REQ);
							if(nCnt >= 3 || nRecipeUpdate == FALSE)
							{
								m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_MODEL_SAVE_COMPLETE, FALSE);
								break;
							}
							nCnt++;
							Sleep(500);
						}

						//2017.07.07	PLCTeachingData Recipe Back Up
						CString strPLCModelBackUpPath = _T("");
						strPLCModelBackUpPath.Format(_T("%s%s"),ROOT_PATH,PLC_MODEL_PATH);
						pFrame->m_DataManager.m_fnBackUpNetworkDrive(strPLCModelBackUpPath,TRUE); 
						//
					}
					else
						AfxMessageBox(_T("Model Data Save Fail"));
				}
				else
					AfxMessageBox(_T("Model Data Save Fail"));
			}

			m_fnSetStep(EQ_STEP_PLC_RCP_END);

			break;
		}
	case EQ_STEP_PLC_RCP_END:
		m_fnSetStep(SEQ_STEP_PLC_RCP_CHECK);
		break;
	}
	G_AddLog(3, 3, _T("m_fnPlcRecipeUpdateCheck() - End"));
}

void CThreadProcPLC::m_fnPlcInitRest()
{
	switch(m_fnGetStep())
	{
	case SEQ_STEP_PLC_INIT_RESET:
		if (m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_S1_RESET) && m_nUsingFlag)
		{
			int iRet =0;
			CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
			CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

			int nEqpSizeType = pFrame->m_fnEqpSizeType();
			int nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

			m_nUsingFlag = 0;
			//pView->m_pDlgPLC->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, FALSE); //Chung Du Comment //16.05.03 sgkim
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, FALSE);

			if(nEqpSizeType == EQP_SMALL_SIZE || nEqpSizeType == EQP_MEDIUM_SIZE)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_VISION_CLIENT_FOLDER_ALARM, FALSE); //2017.06.26	 VisionClient 폴더 관련 Alarm 해지
	
			G_AddLog(3, 2, _T("Sequence Reset!!!"));
			
			if( nEqpSizeType == EQP_SMALL_SIZE || nEqpSizeType == EQP_MEDIUM_SIZE )
			{
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S1_GRAB_END, FALSE);
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S2_GRAB_END, FALSE);

				if( nEqpModel == EQP_TYPE_ABI )
					pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S3_GRAB_END, FALSE);
			}
			else if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE )
			{
				iRet = pFrame->m_pVsSeverIF->Seq_AlignProcReset();		//2016.11.05 Mr.Chu

				for (int i = 1; i <= pFrame->m_stParamInfo.nCountVisionClient; i++)
				{
					pFrame->m_pVsSeverIF->m_bRecvRecipeRes[i - 1] = FALSE;

					if(pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[i - 1])
						pFrame->m_pVsSeverIF->m_fnVisionInitial(i);			
				}
				//m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_R_CLASSIFY_END, 0);	//2017.07.12 add by mby PG complete bit L550 off
				//m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_S1_GRAB_END_BWD, 0);	//2017.07.15 add by mby Backward ok bit reset L535 off
			}

			for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
			{
				pFrame->m_pThreadSeqPLC[i]->m_fnResetSEQ(TRUE, TRUE, TRUE);		

			}
			//m_fnResetSEQ();
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 0);	//Overlap Alarm Off
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_JUDGE_SIGNAL, 0); //2018.12.14 add by mby
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PLC_S1_RESET, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_REGRADE_CHECK_ALARM, 0); //2019.02.02 add by mby
			//2020.08.17 add by mby S-
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_SCRATCH, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BURR, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BROKEN, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_MEASURE_FAIL, 0);
			//2020.08.17 add by mby E-

			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PLC_CIM_SKIP_END,0);		//2020.09.03 add by mby Cim 보고 완료 신호 OFF, 초기화작업
		}
		else if (m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_S1_RESET) == 0 )
		{
			m_nUsingFlag = 1;
		}
		m_fnSetStep(SEQ_STEP_PLC_INIT_RESET_END);
		break;

	case SEQ_STEP_PLC_INIT_RESET_END:
		m_fnSetStep(SEQ_STEP_PLC_INIT_RESET);
		break;
	}
}

//Cheng Du
void CThreadProcPLC::m_fnPlcAlarmReset()
{
	switch(m_fnGetStep())
	{
	case SEQ_STEP_PLC_ALARM_RESET:
		if (m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_ALARM_RESET) && m_nUsingPlcAlarmResetFlag)
		{
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_TEMPERATURE_WARNNING, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_NON_STOP, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_MEASURE_COOR_OCCUR, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_POL_MARK_ILEGAL_ALARM, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_INTERLOCK1, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC1, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC2, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC3, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC4, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC5, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC6, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC7, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC8, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC9, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_QMS_UPLOAD_FAIL, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_MODEL_CHANGE_NG, FALSE);	//2018.01.27 add by mby for model ng reset

			//2018.06.28
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC1, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC2, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC3, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC4, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC5, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC6, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC7, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC8, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC9, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL, 0);

			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_JUDGE_SIGNAL, 0);			//2018.12.14 add by mby
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_MANUAL_JUDGE_CHECK, 0);			//2019.01.07 add by ryu
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_CLASSIFY_END_TIME_OVER, 0);  //2019.01.09 add by ryu
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_REGRADE_CHECK_ALARM, 0);		//2019.02.02 add by mby
			//2019.03.13 add by mby
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_EDGE_MEASURE_FAIL, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_ALIGN_RCP_MIS, 0);		

			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_PACODE_ALARM, 0); //2020.01.13 add by mby

			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_MEASURE_VALUE_ZERO, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_PROCESS_CIM_NETWORK_FAIL, 0);		//2019.06.12 add by ryu

			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_SCRATCH, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BURR, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_BROKEN, 0);
			m_pDlgPLCSetup->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR_MEASURE_FAIL, 0);
		}
		else if (m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PLC_ALARM_RESET) == 0 )
		{
			m_nUsingPlcAlarmResetFlag = 1;
		}
		m_fnSetStep(SEQ_STEP_PLC_ALARM_RESET_END);
		break;

	case SEQ_STEP_PLC_ALARM_RESET_END:
		m_fnSetStep(SEQ_STEP_PLC_ALARM_RESET);
		break;
	}
}

void CThreadProcPLC::m_fnProcCheckFrontNgFlag()
{
	G_AddLog(3, 3, _T("m_fnProcCheckFrontNgFlag() - Start"));
	long lRet = -1, lRet_Req_R = -1, lRet_Req_L = -1;
	int nPubNo = 0, nEqpSizeType = 0, nEqpModel = 0;
	CString strConfigFile, strMsg;
	CString strGrade = _T("");
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strNgFlagInfoPath;
	errno_t errFileOpen;

	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);
	strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile);
	nEqpModel = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0, strConfigFile);	//0:AVL 1:ABI
	long lNgFlagComplete = -1;	//2016.01.04 PLC LOG
	long lRet_Req_L_Off = -1;
	BOOL bNgFlag = FALSE;

	int nNgFlagCount = 0;

	switch(m_fnGetStep())
	{
	case SEQ_STEP_FRONT_NG_FLAG_CHECK:
		lRet_Req_L = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_NG_FLAG_L_REQ);	// ABI:ULD STAGE F

		if (lRet_Req_L == OK)
		{
			m_bULDStage_F = TRUE;

			if (nEqpSizeType == EQP_TYPE_MEDIUM || nEqpSizeType == EQP_LARGE_SIZE || nEqpSizeType == EQP_MONITOR_SIZE)
				m_fnSetStep(SEQ_STEP_FRONT_GET_PANEL_COUNT_ONE);
			else
				m_fnSetStep(SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO);
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_NG_FLAG_CHECK FLAG REQUEST"));
		}
		else
			m_fnSetStep(SEQ_STEP_FRONT_END);
		break;

	case SEQ_STEP_FRONT_GET_PANEL_COUNT_ONE:
		Sleep(100);	//2018.03.02 500 -> 250->100
		if (m_bULDStage_F == TRUE)
		{
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_ABI_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, m_strPanelID1);
		}
		//m_strPanelID1 = _T("PNL000000000000000");
		m_strPanelID1.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_GET_PANEL_COUNT_ONE PanelID1 : %s"), m_strPanelID1);

		if(m_strPanelID1 == "")
		{
			//m_bULDStage_F = FALSE;
			m_fnRetryCheck(_T("SEQ_STEP_FRONT_GET_PANEL_COUNT_ONE"), 10000);
			//m_fnSetStep(SEQ_STEP_FRONT_END);
			break;
		}
		m_nRetryClassifyCnt = 0;
		m_fnSetStep(SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE);
		break;

	case SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO:

		if (m_bULDStage_F == TRUE)
		{
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_ABI_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, m_strPanelID1);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_ABI_PLC_NG_FLAG_PANEL_ID_2, LEN_PANEL_ID, m_strPanelID2);
		}

		m_strPanelID1.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO PanelID1 : %s"), m_strPanelID1);
		m_strPanelID2.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO PanelID2 : %s"), m_strPanelID2);

		if(m_strPanelID1 == "" && m_strPanelID2 == "")
		{
			//m_bULDStage_F = FALSE;
			m_fnRetryCheck(_T("SEQ_STEP_FRONT_GET_PANEL_COUNT_TWO"), 10000);
			//m_fnSetStep(SEQ_STEP_FRONT_END);
			break;
		}
		m_nRetryClassifyCnt = 0;
		m_fnSetStep(SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO);
		break;

	case SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE:

		m_strPanelID1.TrimRight();
		stNgFlagInfo_Panel1 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID1);
		stNgFlagInfo_Panel2.init_info();
		if (stNgFlagInfo_Panel1.bComplete)
		{	
		
			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
			{
				strGrade.Format(_T("%s"),stNgFlagInfo_Panel1.cGrade);
				//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정
				if(stNgFlagInfo_Panel1.bLowerEqpBypass && strGrade != _T("A"))
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_NG_FLAG_LOWER_BYPASS, TRUE);
			}
			pFrame->m_bMJudgeFlag = FALSE;		//2019.01.03 add by mby
			m_nRetryCount = 0;
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				
			if (&stNgFlagInfo_Panel1.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE] = stNgFlagInfo_Panel1;
			if (&stNgFlagInfo_Panel2.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO] = stNgFlagInfo_Panel2;
			UINT uiThreadID;

			//2018.12.16 add by mby S-
			CloseHandle(m_pTreadCimProc->m_hThreadCim);
			m_pTreadCimProc->m_hThreadCim = NULL;

			//LeaveCriticalSection(&m_pTreadCimProc->m_csThreadCim);
			//2018.12.16 add by mby E-
			m_pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, m_pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);
			//G_AddLog(3, 2, _T("[Panel Id : %s] Process QMS Data End!!"), stNgFlagInfo_Panel1.cPanelID);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			if (m_bULDStage_F == TRUE)
			{	//2018.04.20 다시원복
				if (nEqpModel == EQP_TYPE_AVL)  //20181206 ABI, AVL 조건 구분 by ryu
				{						
					nNgFlagCount = m_fnNgFlagInfoCheck(stNgFlagInfo_Panel1.nNgFlag);

					//if(stNgFlagInfo_Panel1.nNgFlag == NG_FLAG_ABI_B_GRADE) stNgFlagInfo_Panel1.nNgFlag = NG_FLAG_AVL_B_GRADE;
					//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, nNgFlagCount);
				}
				else if(nEqpModel == EQP_TYPE_ABI)
				{
					if(stNgFlagInfo_Panel1.nNgFlag == NG_FLAG_AVL_B_GRADE) stNgFlagInfo_Panel1.nNgFlag = NG_FLAG_ABI_B_GRADE;
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
				}
				
				//2018.04.19 고객사 요청으로 NGflag ->0
				//lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, 0);
				Sleep(10);	//2018.03.02 1000 -> 500->10
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, TRUE);
			}

			nCnt = 0;
			while(nCnt < MAX_REV_PANEL * 2)
			{
				strVal = pFrame->m_DataManager.m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
				if (strVal == m_strPanelID1 && pFrame->m_DataManager.m_stNgFlagInfo[nCnt].bComplete)
				{				
					pFrame->m_DataManager.m_stNgFlagInfo[nCnt].init_info();
					break;
				}		
				nCnt ++;
			}

			errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
			size_t read_size = 0;
			if(errFileOpen == 0)
			{		
				fwrite(& pFrame->m_DataManager.m_stNgFlagInfo, sizeof(pFrame->m_DataManager.m_stNgFlagInfo) , 1, out);		
				fclose(out);
			}
			//2015.12.29 PLC LOG
			long lNgFlagValue = -1;
			if (m_bULDStage_F == TRUE)
			{
				lNgFlagValue = m_pDlgPLCSetup->m_fnGetDevice(ADDR_ABI_F_NG_FLAG1);
			}
			//2020.01.07 add by mby S-
			for(int i = 0 ; i<3 ; i++){
				strMsg.Format(_T("LIFE%d"), i+1);
				nCnt = pFrame->m_iniConfigSet.GetProfileIntW(_T("LIFE_COUNT"), strMsg, 0);
				strVal.Format(_T("%d"), nCnt+1);
				pFrame->m_iniConfigSet.WriteProfileStringW(_T("LIFE_COUNT"), strMsg, strVal);					
			}
			//2020.01.07 add by mby E-
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 PLC WRITE: [%s] || NG Flag = %d"), m_strPanelID1, lNgFlagValue);
			//2015.12.29 PLC LOG
			G_AddLog(3, 0, _T("NG FLAG - SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID1, nNgFlagCount, stNgFlagInfo_Panel1.bComplete);
			m_fnSetStep(SEQ_STEP_FRONT_END);
		}
		else
		{					
			CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
			//2019.01.03 add by mby		
			if(pFrame->m_bMJudgeFlag)
			{
				if(!m_fnRetryCheck(_T("SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE_RE_JUDGE"), 60000))			
				{
				}
			}
			else
			{							
				if(!m_fnRetryCheck(_T("SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE"), 20000))	//10->20 2018.11.17 by ryu			
				{
					/*if(!pFrame->m_stParamInfo.bUseManualJudge || !bNgFlag && pFrame->m_stParamInfo.bUseManualJudge){
						G_AddLog(2, 3, _T("SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_ONE : ManualJudge"));
					}else{	*/				
						if (nEqpModel == EQP_TYPE_ABI)
						{
							G_AddLog(2, 0, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_ONE PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
							G_AddLog_ALARM(_T("NG FLAG - Front Retry : %d"), ERROR_CHECK_FRONT_NGFLAG);
							stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
							pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
						}
						else if(nEqpModel == EQP_TYPE_AVL)
						{
							G_AddLog(3, 0, _T("Force judge PanelID1 : [%s] NG FLAG Complete = %d"),m_strPanelID1,stNgFlagInfo_Panel1.bComplete);	//2019.09.26 add by mby
							pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_JUDGE_SIGNAL, 1);  //
							//G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_ONE PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
							//G_AddLog_ALARM(_T("NG FLAG - Front Retry : %d"), ERROR_CHECK_FRONT_NGFLAG);
							//stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
							//pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1						
						}
					//}

					pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID1, 0);	//2016.07.14
					//m_nRetryClassifyCnt = 0;
				}
			}
			//m_nRetryClassifyCnt ++;
			
		}
		break;
	case SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO:
		long lNgFlag1, lNgFlag2;
		int nNgFlag1, nNgFlag2;
		m_strPanelID1.TrimRight();
		m_strPanelID2.TrimRight();
		stNgFlagInfo_Panel1 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID1);
		stNgFlagInfo_Panel2 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID2);
		if (stNgFlagInfo_Panel1.bComplete && stNgFlagInfo_Panel2.bComplete)
		{
			m_nRetryCount = 0;
			if (&stNgFlagInfo_Panel1.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE] = stNgFlagInfo_Panel1;
			if (&stNgFlagInfo_Panel2.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO] = stNgFlagInfo_Panel2;
			
			UINT uiThreadID;
			m_pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, m_pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);
			m_fnSetStep(SEQ_STEP_FRONT_END);
			if (m_bULDStage_F == TRUE)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_R_NG_FLAG2, stNgFlagInfo_Panel2.nNgFlag);
				
				if (nEqpModel == EQP_TYPE_ABI)	Sleep(300); // 2016.05.05 sgkim
				lNgFlag1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_ABI_F_NG_FLAG1);
				lNgFlag2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_ABI_R_NG_FLAG2);
				nNgFlag1 = lNgFlag1;
				nNgFlag2 = lNgFlag2;

				if(nNgFlag1 != stNgFlagInfo_Panel1.nNgFlag || nNgFlag2 != stNgFlagInfo_Panel2.nNgFlag)
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_ABI_R_NG_FLAG2, stNgFlagInfo_Panel2.nNgFlag);
				}
				if (nEqpModel == EQP_TYPE_ABI) 	Sleep(2000);
				else							Sleep(1000);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, TRUE);
			}
			nCnt = 0;
			while(nCnt < MAX_REV_PANEL * 2)
			{
				strVal = pFrame->m_DataManager.m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
				if ((strVal == m_strPanelID1 || strVal == m_strPanelID2) && pFrame->m_DataManager.m_stNgFlagInfo[nCnt].bComplete)
				{				
					pFrame->m_DataManager.m_stNgFlagInfo[nCnt].init_info();				
				}		
				nCnt ++;
			}
			errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
			size_t read_size = 0;
			if(errFileOpen == 0)
			{		
				fwrite(& pFrame->m_DataManager.m_stNgFlagInfo, sizeof(pFrame->m_DataManager.m_stNgFlagInfo) , 1, out);		
				fclose(out);
			}	
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO PanelID1 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID1, stNgFlagInfo_Panel1.nNgFlag, stNgFlagInfo_Panel1.bComplete);
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO PanelID2 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID2, stNgFlagInfo_Panel2.nNgFlag, stNgFlagInfo_Panel2.bComplete);
		}
		else
		{
			if(!m_fnRetryCheck(_T("SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO"), 20000))	
			{
				if (nEqpModel == EQP_TYPE_ABI)
				{
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_TWO PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_TWO PanelID2 : %s || NG Flag = %d"), m_strPanelID2, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Front Retry : %d"), ERROR_CHECK_FRONT_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					stNgFlagInfo_Panel2.DefaultData(m_strPanelID2, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel2);	//2015.09.17 change 1 -> 5 -> 1
				}
				else if(nEqpModel == EQP_TYPE_AVL)
				{
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_TWO PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_FRONT_RESULT_CHECK_COUNT_TWO PanelID2 : %s || NG Flag = %d"), m_strPanelID2, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Front Retry : %d"), ERROR_CHECK_FRONT_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					stNgFlagInfo_Panel2.DefaultData(m_strPanelID2, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel2);	//2015.09.17 change 1 -> 5 -> 1
				}
				pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID1, 0);	//2016.07.14
				pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID2, 0);	//2016.07.14
				//m_nRetryClassifyCnt = 0;
			}
			//m_nRetryClassifyCnt ++;
			//m_fnRetryCheck(_T("SEQ_STEP_FRONT_RESULT_CHECK_PANEL_COUNT_TWO"), 10000);
		}
		break;

	case SEQ_STEP_FRONT_END:
		Sleep(100);	//2018.03.02 500 -> 250 ->100
		lRet_Req_L_Off = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_NG_FLAG_L_REQ);	// ABI:ULD STAGE F

		if (lRet_Req_L_Off == 0 && m_bULDStage_F == TRUE)	
		{
			m_nRetryCount = 0;

			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, FALSE);
			//2016.01.04 PLC LOG
			
			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_NG_FLAG_LOWER_BYPASS, FALSE);

			Sleep(100);	//2018.03.02 500 -> 250 ->100
			lNgFlagComplete = m_pDlgPLCSetup->m_fnGetDevice(ADDR_L_NG_FLAG_COMPLETE);
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_FRONT_END PLC WRITE: NG Flag Complete Ul1(L510)= %d"), lNgFlagComplete);
			//2016.01.04 PLC LOG
			m_fnSetStep(SEQ_STEP_FRONT_NG_FLAG_CHECK);
			m_bULDStage_F = FALSE;
		}
		else if(m_bULDStage_F != FALSE && lRet_Req_L_Off != 0)
		{
			//m_fnRetryCheck(_T("SEQ_STEP_FRONT_END"), 20000)
			m_fnRetryStep();
		}
		else 
		{
			m_fnSetStep(SEQ_STEP_FRONT_NG_FLAG_CHECK);	
		}			
					
		break;

	default:
		break;
	}
	G_AddLog(3, 3, _T("m_fnProcCheckFrontNgFlag() - End"));
}

void CThreadProcPLC::m_fnProcCheckRearNgFlag()
{
	G_AddLog(3, 3, _T("m_fnProcCheckRearNgFlag() - Start"));

	long lRet = -1, lRet_Req_R = -1, lRet_Req_L = -1;
	int nPubNo = 0, nEqpSizeType = 0, nEqpModel = 0;
	CString strConfigFile, strMsg;
	CString strGrade = _T("");
	//CString strGrade1, strGrade2;	//2015.09.17
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();	
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strNgFlagInfoPath;
	errno_t errFileOpen;

	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);
	strConfigFile = _T("");
	strConfigFile.Append(ROOT_PATH);
	strConfigFile.Append(CONFIG_PATH);

	nEqpSizeType = GetPrivateProfileInt(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0, strConfigFile);
	nEqpModel = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0, strConfigFile);	//0:AVL 1:ABI
	long lNgFlagComplete = -1;	//2016.01.04 PLC LOG
	long lRet_Req_R_Off = -1;

	switch(m_fnGetStep())
	{
	case SEQ_STEP_REAR_NG_FLAG_CHECK:
		lRet_Req_R = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_NG_FLAG_R_REQ);	// ABI:ULD STAGE R OR AVL:ULD STAGE
		if (lRet_Req_R == OK)
		{
			m_bULDStage_R = TRUE;

			if (nEqpSizeType == EQP_TYPE_MEDIUM || nEqpSizeType == EQP_LARGE_SIZE)
				m_fnSetStep(SEQ_STEP_REAR_GET_PANEL_COUNT_ONE);
			else
				m_fnSetStep(SEQ_STEP_REAR_GET_PANEL_COUNT_TWO);
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_NG_FLAG_CHECK FLAG REQUEST"));
		}
		else
			m_fnSetStep(SEQ_STEP_REAR_END);
		break;

	case SEQ_STEP_REAR_GET_PANEL_COUNT_ONE:
		Sleep(200);	//500->200
		if (m_bULDStage_R == TRUE)
		{
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_AVL_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, m_strPanelID1);
		}

		m_strPanelID1.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_GET_PANEL_COUNT_ONE PanelID1 : %s"), m_strPanelID1);
		if(m_strPanelID1 == "")
		{
			//m_bULDStage_R = FALSE;
			m_fnRetryCheck(_T("SEQ_STEP_REAR_GET_PANEL_COUNT_ONE"), 3000);
			//m_fnSetStep(SEQ_STEP_REAR_END);
			break;
		}
		m_nRetryClassifyCnt = 0;
		m_fnSetStep(SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE);
		break;

	case SEQ_STEP_REAR_GET_PANEL_COUNT_TWO:

		if (m_bULDStage_R == TRUE)
		{
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_AVL_PLC_NG_FLAG_PANEL_ID_1, LEN_PANEL_ID, m_strPanelID1);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_AVL_PLC_NG_FLAG_PANEL_ID_2, LEN_PANEL_ID, m_strPanelID2);
		}

		m_strPanelID1.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_GET_PANEL_COUNT_TWO PanelID1 : %s"), m_strPanelID1);
		m_strPanelID2.TrimRight();
		G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_GET_PANEL_COUNT_TWO PanelID2 : %s"), m_strPanelID2);

		if(m_strPanelID1 == "" && m_strPanelID2 == "")
		{
			//m_bULDStage_R = FALSE;
			//m_fnSetStep(SEQ_STEP_REAR_END);
			m_fnRetryCheck(_T("SEQ_STEP_REAR_GET_PANEL_COUNT_TWO"), 10000);
			//m_fnSetStep(SEQ_STEP_REAR_END);
			break;
		}
		m_nRetryClassifyCnt = 0;
		m_fnSetStep(SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO);
		break;

	case SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE:
		
		m_strPanelID1.TrimRight();
		stNgFlagInfo_Panel1 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID1);
		stNgFlagInfo_Panel2.init_info();
		
		if (stNgFlagInfo_Panel1.bComplete)
		{	
			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
			{
				strGrade.Format(_T("%s"),stNgFlagInfo_Panel1.cGrade);
				//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정
				if(stNgFlagInfo_Panel1.bLowerEqpBypass && strGrade != _T("A"))
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_NG_FLAG_LOWER_BYPASS, TRUE);
			}

			m_nRetryCount = 0;
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				
			if (&stNgFlagInfo_Panel1.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE] = stNgFlagInfo_Panel1;
			if (&stNgFlagInfo_Panel2.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO] = stNgFlagInfo_Panel2;
			UINT uiThreadID;

			m_pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, m_pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
			if (m_bULDStage_R == TRUE)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_AVL_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
				Sleep(100);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, TRUE);
			}

			nCnt = 0;
			while(nCnt < MAX_REV_PANEL * 2)
			{
				strVal = pFrame->m_DataManager.m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
				if (strVal == m_strPanelID1 && pFrame->m_DataManager.m_stNgFlagInfo[nCnt].bComplete)
				{				
					pFrame->m_DataManager.m_stNgFlagInfo[nCnt].init_info();
					break;
				}		
				nCnt ++;
			}

			errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
			size_t read_size = 0;
			if(errFileOpen == 0)
			{		
				fwrite(& pFrame->m_DataManager.m_stNgFlagInfo, sizeof(pFrame->m_DataManager.m_stNgFlagInfo) , 1, out);		
				fclose(out);
			}
			//2015.12.29 PLC LOG
			long lNgFlagValue = -1;
			if (m_bULDStage_R == TRUE)
			{
				lNgFlagValue = m_pDlgPLCSetup->m_fnGetDevice(ADDR_AVL_F_NG_FLAG1);
			}
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 PLC WRITE: [%s] || NG Flag = %d"), m_strPanelID1, lNgFlagValue);
			//2015.12.29 PLC LOG
			G_AddLog(3, 0, _T("NG FLAG - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID1, stNgFlagInfo_Panel1.nNgFlag, stNgFlagInfo_Panel1.bComplete);
			m_fnSetStep(SEQ_STEP_REAR_END);
		}
		else
		{					
			if(!m_fnRetryCheck(_T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE"), 10000))
			{
				if (nEqpModel == EQP_TYPE_ABI)
				{
					G_AddLog(2, 0, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Rear Retry : %d"), ERROR_CHECK_REAR_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
				}
				else if(nEqpModel == EQP_TYPE_AVL)
				{
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Rear Retry : %d"), ERROR_CHECK_REAR_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
				}
				pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID1, 0);	//2016.07.14
				//m_nRetryClassifyCnt = 0;
			}
			//m_nRetryClassifyCnt ++;
			//m_fnRetryCheck(_T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_ONE"), 10000);
		}
		break;
	case SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO:
		long lNgFlag1, lNgFlag2;
		int nNgFlag1, nNgFlag2;
		m_strPanelID1.TrimRight();
		m_strPanelID2.TrimRight();
		stNgFlagInfo_Panel1 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID1);
		stNgFlagInfo_Panel2 = pFrame->m_DataManager.m_fnReadNgFlagList(m_strPanelID2);

		//G_AddLog(3, 2, _T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO - [ Panel Id 1: %s Com: %d] [ Panel Id 2: %s Com: %d]"), m_strPanelID1, m_strPanelID2,
		//	stNgFlagInfo_Panel1.bComplete, stNgFlagInfo_Panel2.bComplete);	//2017.03.29
		//G_AddLog(3, 2, _T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO - [ Panel 1 NGFlag Complete : %d ] [ Panel 2 NG Flag Complete: %d ]"),
		//	stNgFlagInfo_Panel1.bComplete, stNgFlagInfo_Panel2.bComplete);

		if (stNgFlagInfo_Panel1.bComplete && stNgFlagInfo_Panel2.bComplete)
		{
			m_nRetryCount = 0;
			if (&stNgFlagInfo_Panel1.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE] = stNgFlagInfo_Panel1;
			if (&stNgFlagInfo_Panel2.cPanelID[0] != NULL )			
				m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO] = stNgFlagInfo_Panel2;
			
			UINT uiThreadID;
			m_pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, m_pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);

			m_fnSetStep(SEQ_STEP_REAR_END);
			if (m_bULDStage_R == TRUE)
			{
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_AVL_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_AVL_R_NG_FLAG2, stNgFlagInfo_Panel2.nNgFlag);

				if (nEqpModel == EQP_TYPE_ABI)	Sleep(300); // 2016.05.05 sgkim
				lNgFlag1 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_AVL_F_NG_FLAG1);
				lNgFlag2 = m_pDlgPLCSetup->m_fnGetDevice(ADDR_AVL_R_NG_FLAG2);
				nNgFlag1 = lNgFlag1;
				nNgFlag2 = lNgFlag2;
				if(nNgFlag1 != stNgFlagInfo_Panel1.nNgFlag || nNgFlag2 != stNgFlagInfo_Panel2.nNgFlag)
				{
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_AVL_F_NG_FLAG1, stNgFlagInfo_Panel1.nNgFlag);
					lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_AVL_R_NG_FLAG2, stNgFlagInfo_Panel2.nNgFlag);
				}
				if (nEqpModel == EQP_TYPE_ABI)	Sleep(2000);
				else							Sleep(1000);
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, TRUE);
			}

			nCnt = 0;
			while(nCnt < MAX_REV_PANEL * 2)
			{
				strVal = pFrame->m_DataManager.m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
				if ((strVal == m_strPanelID1 || strVal == m_strPanelID2) && pFrame->m_DataManager.m_stNgFlagInfo[nCnt].bComplete)
				{				
					pFrame->m_DataManager.m_stNgFlagInfo[nCnt].init_info();				
				}		
				nCnt ++;
			}
			errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
			size_t read_size = 0;
			if(errFileOpen == 0)
			{		
				fwrite(& pFrame->m_DataManager.m_stNgFlagInfo, sizeof(pFrame->m_DataManager.m_stNgFlagInfo) , 1, out);		
				fclose(out);
			}	
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO PanelID1 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID1, stNgFlagInfo_Panel1.nNgFlag, stNgFlagInfo_Panel1.bComplete);
			G_AddLog(3, 2, _T("NG FLAG - SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO PanelID2 : [%s] || NG Flag = %d || Complete = %d"), m_strPanelID2, stNgFlagInfo_Panel2.nNgFlag, stNgFlagInfo_Panel2.bComplete);
		}
		else
		{
			if(!m_fnRetryCheck(_T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO"), 10000))
			{
				if (nEqpModel == EQP_TYPE_ABI)
				{
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_COUNT_TWO PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_COUNT_TWO PanelID2 : %s || NG Flag = %d"), m_strPanelID2, NG_FLAG_ABI_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Rear Retry : %d"), ERROR_CHECK_REAR_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					stNgFlagInfo_Panel2.DefaultData(m_strPanelID2, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel2);	//2015.09.17 change 1 -> 5 -> 1
				}
				else if(nEqpModel == EQP_TYPE_AVL)
				{
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_COUNT_TWO PanelID1 : %s || NG Flag = %d"), m_strPanelID1, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog(2, 3, _T("NG FLAG [RETRY] - SEQ_STEP_REAR_RESULT_CHECK_COUNT_TWO PanelID2 : %s || NG Flag = %d"), m_strPanelID2, NG_FLAG_AVL_B_GRADE);	//2015.09.17 change 1 -> 5 -> 1
					G_AddLog_ALARM(_T("NG FLAG - Rear Retry : %d"), ERROR_CHECK_REAR_NGFLAG);
					stNgFlagInfo_Panel1.DefaultData(m_strPanelID1, APP_NG);
					stNgFlagInfo_Panel2.DefaultData(m_strPanelID2, APP_NG);
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel1);	//2015.09.17 change 1 -> 5 -> 1
					pFrame->m_DataManager.m_fnWriteNgFlagList(stNgFlagInfo_Panel2);	//2015.09.17 change 1 -> 5 -> 1
				//m_nRetryClassifyCnt = 0;
				}
				pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID1, 0);	//2016.07.14
				pFrame->m_DataManager.m_fnDelLocalInspData(m_strPanelID2, 0);	//2016.07.14
			}
			//m_nRetryClassifyCnt ++;
			//m_fnRetryCheck(_T("SEQ_STEP_REAR_RESULT_CHECK_PANEL_COUNT_TWO"), 10000);
		}
		break;

	case SEQ_STEP_REAR_END:
		Sleep(200);	//500->200
		lRet_Req_R_Off = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_NG_FLAG_R_REQ);	// ABI:ULD STAGE R OR AVL:ULD STAGE

		if (m_bULDStage_R == TRUE && lRet_Req_R_Off == 0)	
		{
			m_nRetryCount = 0;
			lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, FALSE);

			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				lRet = m_pDlgPLCSetup->m_fnSetDevice(ADDR_NG_FLAG_LOWER_BYPASS, FALSE);

			//2016.01.04 PLC LOG
			Sleep(200);
			lNgFlagComplete = m_pDlgPLCSetup->m_fnGetDevice(ADDR_R_NG_FLAG_COMPLETE);
			G_AddLog(3, 3, _T("NG FLAG - SEQ_STEP_REAR_END PLC WRITE: NG Flag Complete Ul2(L530)= %d"), lNgFlagComplete);
			//2016.01.04 PLC LOG
			m_fnSetStep(SEQ_STEP_REAR_NG_FLAG_CHECK);					
			m_bULDStage_R = FALSE;
		}		
		else if(m_bULDStage_R != FALSE && lRet_Req_R_Off != 0)
		{
			//m_fnRetryCheck(_T("SEQ_STEP_REAR_END"), 20000);
			m_fnRetryStep();
		}
		else 
		{
			m_fnSetStep(SEQ_STEP_REAR_NG_FLAG_CHECK);	
		}			
		break;

	default:
		break;
	}
	G_AddLog(3, 3, _T("m_fnProcCheckRearNgFlag() - End"));
}

// 2015.10.15 add by ikm Packing S
void CThreadProcPLC::m_fnPlcSaveModelID()
{	
	try
	{		
		switch(m_fnGetStep())
		{
			case SEQ_STEP_PLC_SAV_MODID:
			{
				if(m_pDlgPLCSetup->m_fnGetDevice2(ADDR_PACKING_PLC_SAVE_MODEL))
				{
					BOOL rc = FALSE;

					//CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

					CString strRootPath = _T("");

					/////////////////////////////////////////////////////////////////
					CStdioFile cstFile;
					int nPlcAdrModelNo;
					CString strPlcAdrModelID;
					CString strRetPlcModelID;
					/////////////////////////////////////////////////////////////////	

					strRootPath.Format(_T("%s%s"), ROOT_PATH, MODEL_LIST_PATH);


					if (!cstFile.Open(strRootPath, CFile::modeWrite | CFile::modeCreate|CFile::typeText, NULL))
					{
						G_AddLog(3, 2, _T("m_fnThreadSaveModel - Empty File Open Error %s : %d"), strRootPath, GetLastError());				
					}

					nPlcAdrModelNo = 11000;
					for (int nModelCnt = 0 ; nModelCnt <= MAX_PLC_MODEL_CNT  ; nModelCnt ++)
					{							
						strPlcAdrModelID.Format(_T("D%d"), nPlcAdrModelNo);		
						m_pDlgPLCSetup->m_fnGetDeviceBlock(strPlcAdrModelID.GetBuffer(0), LEN_MODEL_ID, strRetPlcModelID);		
						strPlcAdrModelID.ReleaseBuffer();
						strRetPlcModelID.Trim();						
						if (!strRetPlcModelID.IsEmpty() || strRetPlcModelID != _T("") )
						{
							cstFile.WriteString(strRetPlcModelID + _T("\n") );			
						}			
						nPlcAdrModelNo += 10;
					}
					cstFile.Close();
				}
				m_fnSetStep(SEQ_STEP_PLC_SAV_MODID_END);						
				break;
			}
			case SEQ_STEP_PLC_SAV_MODID_END:
				m_fnSetStep(SEQ_STEP_PLC_SAV_MODID);	
				break;	
	   }
			
	}

	catch (CFileException* e)
	{			
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadSaveModel Failed !! - CException "));
		e->Delete();
	}
	catch (CException* e)
	{		
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadSaveModel Failed !! - CException "));
		e->Delete();
	}
	/*CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if(m_hThreadSaveModelId == NULL)
	{
	UINT uiThreadID;
	m_hThreadSaveModelId = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadSaveModel, (LPVOID)this, 0, &uiThreadID);
	}	*/
}

unsigned __stdcall CThreadProcPLC::m_fnThreadSaveModel(LPVOID pArg )
{	
	BOOL rc = FALSE;
	CThreadProcPLC *pDplc = (CThreadProcPLC *)pArg;	
	//CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strRootPath = _T("");
	
	/////////////////////////////////////////////////////////////////
	CStdioFile cstFile;
	int nPlcAdrModelNo;
	CString strPlcAdrModelID;
	CString strRetPlcModelID;
	/////////////////////////////////////////////////////////////////	
	
	strRootPath.Format(_T("%s%s"), ROOT_PATH, MODEL_LIST_PATH);
		
	try
	{		
		if (!cstFile.Open(strRootPath, CFile::modeWrite | CFile::modeCreate|CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("m_fnThreadSaveModel - Empty File Open Error %s : %d"), strRootPath, GetLastError());
			return FALSE;
		}

		nPlcAdrModelNo = 11000;
		for (int nModelCnt = 0 ; nModelCnt <= MAX_PLC_MODEL_CNT  ; nModelCnt ++)
		{							
			strPlcAdrModelID.Format(_T("D%d"), nPlcAdrModelNo);		
			pDplc->m_pDlgPLCSetup->m_fnGetDeviceBlock(strPlcAdrModelID.GetBuffer(0), LEN_MODEL_ID, strRetPlcModelID);		
			strPlcAdrModelID.ReleaseBuffer();
			strRetPlcModelID.Trim();
			if (!strRetPlcModelID.IsEmpty() || strRetPlcModelID != _T("") )
			{
				cstFile.WriteString(strRetPlcModelID + _T("\n") );			
			}			
			nPlcAdrModelNo += 10;
		}

		cstFile.Close();		
	

		CloseHandle(pDplc->m_hThreadSaveModelId);
		pDplc->m_hThreadSaveModelId = NULL;

	}
	
	catch (CFileException* e)
	{
		CloseHandle(pDplc->m_hThreadSaveModelId);
		pDplc->m_hThreadSaveModelId = NULL;
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadSaveModel Failed !! - CException "));
		e->Delete();
	}
	catch (CException* e)
	{
		CloseHandle(pDplc->m_hThreadSaveModelId);
		pDplc->m_hThreadSaveModelId = NULL;
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadSaveModel Failed !! - CException "));
		e->Delete();
	}
	
	return TRUE;	
}
// 2015.10.15 add by ikm Packing  E

// 2015.12.31 add by ikm Grab Retry Check - S
void CThreadProcPLC::m_fnGrabRetryCheck()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	long lGrabInitReqFlag = OFF;
	int nStageNo = 0;
	switch(m_fnGetStep())
	{
	case SEQ_STEP_GRAB_RETRY_INIT_REQ_CHECK:	
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG1){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_F_INSP_INIT_REQ);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG2){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_P_INSP_INIT_REQ);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG3){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_R_INSP_INIT_REQ);}
	
		if (lGrabInitReqFlag == ON)
		{
			G_AddLog_STG_SEQ(3, m_nSeqCnt-1, _T("PLC Grab Initial Request ON - Sequence Reset!!!"));
			pFrame->m_pThreadSeqPLC[m_nSeqCnt-1]->m_fnResetSEQ(TRUE, TRUE, TRUE);
			m_fnSetStep(SEQ_STEP_GRAB_RETRY_PC_COMPLETE_ON);
		}
		else
			m_fnRetryStep();	
		break;

	case SEQ_STEP_GRAB_RETRY_PC_COMPLETE_ON:
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG1){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_F_INSP_INIT_CMP, 1);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG2){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_P_INSP_INIT_CMP, 1);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG3){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_R_INSP_INIT_CMP, 1);}
		G_AddLog_STG_SEQ(3, m_nSeqCnt-1, _T("PC Grab Initial Complete ON!!!"));
		m_fnSetStep(SEQ_STEP_GRAB_RETRY_PLC_INIT_REQ_OFF_CHK);		
		break;

	case SEQ_STEP_GRAB_RETRY_PLC_INIT_REQ_OFF_CHK:
		lGrabInitReqFlag = ON;
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG1){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_F_INSP_INIT_REQ);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG2){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_P_INSP_INIT_REQ);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG3){	lGrabInitReqFlag = m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_R_INSP_INIT_REQ);}
				
		if (lGrabInitReqFlag == OFF)
		{
			G_AddLog_STG_SEQ(3, m_nSeqCnt-1, _T("PLC Grab Initial Request OFF!!!"));		
			m_fnSetStep(SEQ_STEP_GRAB_RETRY_PC_COMPLETE_OFF);
		}
		else
			m_fnRetryStep();	
		break;

	case SEQ_STEP_GRAB_RETRY_PC_COMPLETE_OFF:
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG1){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_F_INSP_INIT_CMP, 0);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG2){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_P_INSP_INIT_CMP, 0);}
		if (m_nSeqCnt == SEQ_PROC_PLC_GRAB_RETRY_STG3){	m_pDlgPLCSetup->m_fnSetDevice(ADDR_PC_R_INSP_INIT_CMP, 0);}
		G_AddLog_STG_SEQ(3, m_nSeqCnt-1, _T("PC Grab Initial Complete OFF!!!"));
		m_fnSetStep(SEQ_STEP_GRAB_RETRY_END);				
		break;	

	case SEQ_STEP_GRAB_RETRY_END:
		m_fnSetStep(SEQ_STEP_GRAB_RETRY_INIT_REQ_CHECK);	
		break;	
	}	
}
// 2015.12.31 add by ikm Grab Retry Check - E

int CThreadProcPLC::m_fnNgFlagInfoCheck(int Ngflag)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int nNgFlag;
	if(Ngflag == 0) nNgFlag = 1;
	else if(Ngflag == 1) nNgFlag = 1;	//2021.02.10 add by mby
	else if(Ngflag == 5) nNgFlag = 20;
	else if(Ngflag == 6) nNgFlag = 30;

	//A
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_A)			nNgFlag = 1;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_AN)	nNgFlag = 1;	//2021.02.10 add by mby
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA1)	nNgFlag = NG_FLAG_FA1_GRADE;//10
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA2)	nNgFlag = NG_FLAG_FA2_GRADE;//11
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA3)	nNgFlag = NG_FLAG_FA3_GRADE;//12
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA4)	nNgFlag = NG_FLAG_FA4_GRADE;//13
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA5)	nNgFlag = NG_FLAG_FA5_GRADE;//14
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA6)	nNgFlag = NG_FLAG_FA6_GRADE;//15
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA8)	nNgFlag = NG_FLAG_FA8_GRADE;//17
	//B
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_B)			nNgFlag = 20;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB1)	nNgFlag = NG_FLAG_FB1_GRADE;//21
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB2)	nNgFlag = NG_FLAG_FB2_GRADE;//22
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB3)	nNgFlag = NG_FLAG_FB3_GRADE;//23
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB4)	nNgFlag = NG_FLAG_FB4_GRADE;//24
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB5)	nNgFlag = NG_FLAG_FB5_GRADE;//25
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB6)	nNgFlag = NG_FLAG_FB6_GRADE;//26
	//C
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_C)			nNgFlag = 30;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC1)	nNgFlag = NG_FLAG_FC1_GRADE;//31
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC2)	nNgFlag = NG_FLAG_FC2_GRADE;//32
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC3)	nNgFlag = NG_FLAG_FC3_GRADE;//33
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC4)	nNgFlag = NG_FLAG_FC4_GRADE;//34
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC5)	nNgFlag = NG_FLAG_FC5_GRADE;//35
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC6)	nNgFlag = NG_FLAG_FC6_GRADE;//36

	//S,S1,A1
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_S)			nNgFlag = NG_FLAG_S_GRADE;//80
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_S1)	nNgFlag = NG_FLAG_S1_GRADE;//81
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_A1)	nNgFlag = NG_FLAG_A1_GRADE;//100	
	
	return nNgFlag;
}