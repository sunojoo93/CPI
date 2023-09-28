#include "stdafx.h"
#include "WorkManager.h" 
#include "DefineSequenceTable.h"
#include "..\..\VisualStation\CommonHeader\Class\LogWriter.h"
#include "VSSequenceTask.h"
#include <algorithm>
#include <omp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CLogWriter* GlassDataWriter;


#define	SEQUENCE_TABLE(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME,ALWAYS_RUNMODE,SEQ_REST_POSSIBLE,CS_LOCK_NAME)					\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )										\
	{																														\
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			EnterCriticalSection(CS_LOCK_NAME);																				\
		}																													\
			if (SEQ_REST_POSSIBLE)																							\
			m_SeqenceCount++;																								\
			isFunctionRuned = true;																							\
			nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize, ALWAYS_RUNMODE, false, SEQ_REST_POSSIBLE);		\
			if (SEQ_REST_POSSIBLE)																							\
			m_SeqenceCount--;																								\
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			LeaveCriticalSection(CS_LOCK_NAME);																				\
		}																													\
	}



WorkManager::WorkManager()
{
	m_hSequenceResetEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	// PC 별 Sequence 동기화 포인트
	for (int i = 0; i < MAX_PC_COUNT; i++)
	{
		for (int j = 0; j < MAX_AUTO_SEQ_ASNC_POINT; j++)
		{
			m_hAsycAutoSeq[i][j] = CreateEvent(NULL, TRUE, FALSE, NULL);
			ResetEvent(m_hAsycAutoSeq[i][j]);
		}
	}

	// Wait Dust End
	m_hDustEnd = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hSeqStageMoveDone = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_bNeedRetry = false;
	m_bIsNormalDust = false;
	ResetEvent(m_hDustEnd);
	ResetEvent(m_hSeqStageMoveDone);
	m_bSeqResetFlag = 0;
	m_SeqenceCount = 0;

	InitializeCriticalSection(&m_csSequenceLock_1);
	InitializeCriticalSection(&m_csSequenceLock_2);
	InitializeCriticalSection(&m_csSequenceLock_3);
	InitializeCriticalSection(&m_csSequenceLock_4);
	InitializeCriticalSection(&m_csSequenceLock_5);
}


WorkManager::~WorkManager()
{
	DeleteCriticalSection(&m_csSequenceLock_1);
	DeleteCriticalSection(&m_csSequenceLock_2);
	DeleteCriticalSection(&m_csSequenceLock_3);
	DeleteCriticalSection(&m_csSequenceLock_4);
	DeleteCriticalSection(&m_csSequenceLock_5);
}

int WorkManager::Start()
{
	HANDLE handle;
	//	WorkManagerQueueParam* param;

		// QParam = Alloc();
		// IndexerCommandQueue ã« Error Happen Report å¾E©EæŒE»¤ PUT. 


		//RestoreGlassData();		//Todo

	for (int i = 0; i < 30; i++)
	{
		handle = m_fnStartThread();
		if (handle == NULL || handle == (HANDLE)-1)
			return APP_NG;

	}
	return APP_OK;
}

void WorkManager::m_fnRunThread()
{
	int nRet;
	BYTE *pMsg = NULL;
	CMDMSG *pCmdMsg = NULL;

	//Start Status Refresh Thread. 처음 실행되는 1개 쓰레드가 처리를 전담한다.

	while (GetThreadRunFlag())
	{
		EXCEPTION_TRY

			pMsg = m_fnPeekMessage();
		pCmdMsg = (CMDMSG*)pMsg;


		nRet = AnalyzeMsg(pCmdMsg);
		if (nRet != APP_OK)
			throw nRet;

		EndWorkProcess(pMsg);

		pCmdMsg = NULL;
		pMsg = NULL;

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				if (pMsg != NULL)
				{
					EndWorkProcess(pMsg);
					pCmdMsg = NULL;
					pMsg = NULL;
				}
				m_fnPrintLog(FALSE, _T("ERROR WorkManager::AnalyzeMsg. Error Code = %d \n"), nRet);
			}
	}


}

int WorkManager::AnalyzeMsg(CMDMSG* pCmdMsg)
{
	int			nRet = APP_OK;
	bool		isFunctionRuned = false;

	EXCEPTION_TRY

		//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					중복실행가능여부,	시퀀스 리셋 가능 여부	)
			// for test	
		SEQUENCE_TABLE(10, 99, Seq_TEST, false, true, &m_csSequenceLock_1)
		// Alive
		SEQUENCE_TABLE(10, 1, VS_TaskAlive, false, true, &m_csSequenceLock_1)
		// Manual
		SEQUENCE_TABLE(21, 10, Seq_ManualChangeModel, false, true, &m_csSequenceLock_2)
		//SEQUENCE_TABLE(21, 11, Seq_ManualGrabImage, false, true, &m_csSequenceLock_2)
		//SEQUENCE_TABLE(21, 12, Seq_ManualInspectImage, false, true, &m_csSequenceLock_2)
		// Auto
		SEQUENCE_TABLE(21, 20, Seq_AutoInspectGrabImage, false, true, &m_csSequenceLock_2)
		SEQUENCE_TABLE(21, 23, Seq_AFReady, true, true, NULL)
		SEQUENCE_TABLE(21, 24, Seq_GrabEnd_FromMainPC, false, true, &m_csSequenceLock_3)
		//SEQUENCE_TABLE(21, 21, Seq_AutoChangeModel, false, true, &m_csSequenceLock_2)
		// Sequence
		//SEQUENCE_TABLE(21, 30, Seq_ClassifyEnd, false, true, NULL)
		//SEQUENCE_TABLE(21, 31, Seq_GUI_ClassifyEnd, false, true, NULL)
		// Align Camera
		//SEQUENCE_TABLE(21, 50, Seq_AlignCamera, false, true, &m_csSequenceLock_2)
		// Async Sequence
		SEQUENCE_TABLE(21, 88, Seq_AsyncSequence, false, false, NULL)
		SEQUENCE_TABLE(21, 87, Seq_GrabSeqReset, false, false, NULL)
		// Receive Dust End
		//SEQUENCE_TABLE(21, 90, Seq_RcvCheckDustEnd, false, false, NULL)
		// Reset
		//SEQUENCE_TABLE(21, 99, Seq_SeqeunceReset, false, false, NULL)

		// For Test
		//SEQUENCE_TABLE(100, 1, Seq_MoveMotion_MortorX_ForTest, false, true, NULL)
		//	SEQUENCE_TABLE (	100,	2	,	Seq_MoveMotion_MortorY_ForTest	, false	,			true					)
		//	SEQUENCE_TABLE (	100,	3	,	Seq_MoveMotion_MortorT_ForTest	, false	,			true					)
		//	SEQUENCE_TABLE (	100,	4	,	Seq_MoveMotion_FocusZ_ForTest	, false	,			true					)
		//	SEQUENCE_TABLE (	100,	5	,	Seq_MoveMotion_ZoomZ_ForTest	, false	,			true					)

		if (m_SeqenceCount <= 0)
		{
			m_bSeqResetFlag = 0;
			m_SeqenceCount = 0;
			ResetEvent(m_hSequenceResetEvent);
		}

	if (isFunctionRuned == false)
	{
		m_fnPrintLog(FALSE, _T("Function Table is nothing. FuncNO : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		throw SEQUENCE_TASK_SEQUENCE_IS_NOTHING;
	}


	EXCEPTION_CATCH

		if (pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
		{
			nRet = ResponseSend(nRet, pCmdMsg);
			if (nRet != APP_OK)
			{
				m_fnPrintLog(FALSE, _T("Response Send  Fail. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
				return nRet;
			}
		}

	return nRet;
}



//int	WorkManager::Seq_SeqeunceReset(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//
//
//	do
//	{
//		EXCEPTION_TRY
//
//			//if( m_bSeqResetFlag && bSeqResetPossible )
//			//	throw 9999;
//
//			// Sequence In LOG
//			m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2199_SeqeunceReset StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			m_bSeqResetFlag = 1;
//			SetEvent(m_hSequenceResetEvent);
//			break;
//
//		case 2:
//			// 초기화 수행
//
//			break;
//
//		case 3:
//
//			for (int nCnt = 0; nCnt < 200; nCnt++)
//			{
//				if (m_SeqenceCount <= 0)
//				{
//					m_bSeqResetFlag = 0;
//					m_SeqenceCount = 0;
//					ResetEvent(m_hSequenceResetEvent);
//					break;
//				}
//				else
//					Sleep(50);
//			}
//
//			m_bSeqResetFlag = 0;
//			m_SeqenceCount = 0;
//			ResetEvent(m_hSequenceResetEvent);
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2199_SeqeunceReset Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2199_SeqeunceReset Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	return nRet;
//}


int	WorkManager::Seq_TEST(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	//USHORT	usSet;
	//INT		nVal;
	//byte*	tempParam = pParam;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2650_TEST StepNo=%d, RetVal=%d"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			//usSet = *(USHORT*)tempParam;
			//tempParam += sizeof(USHORT);
			//nVal = *(int*)tempParam

			m_fnPrintLog(FALSE, _T("CASE 1"));
			break;

		case 2:
			//tempParam = pParam;
			//*(USHORT*)tempParam = 1234;
			//tempParam += sizeof(USHORT);
			//*(int*)tempParam = 2345;
			//nPrmSize = sizeof(USHORT) + sizeof(int);

			//nPrmSizeData = 4;						
			//*(short *)&pParamData[0] = 1;					
			//*(short *)&pParamData[2] = PORT_STATUS_CANCELLED;		
			//nRet = CmdEditSend(SEND_INDX_FUNC_SET_PORT_STATUS, sizeof(int), LOADER_TASK, (UCHAR*)&pParamData, CMD_TYPE_RES);

			////Port Cancel Host로 Unload Request 보고
			//nRet = CmdEditSend(SEND_HOST_UNLOAD_REQUEST_PORT1, sizeof(int), ONLINE_TASK, (UCHAR*)&ndat, CMD_TYPE_RES);
			//if ( nRet != APP_OK )
			//	throw nRet;
			//else
			//{
			//	nStepNo = 999;
			//	nRet = CmdEditSend(1, 1, 0, sizeof(int), LOADER_TASK, 0, CMD_TYPE_NORES);
			//}

			m_fnPrintLog(FALSE, _T("CASE 2"));

			break;

		case 3:
			m_fnPrintLog(FALSE, _T("CASE 3"));

			break;

		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2650_TEST Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2650_TEST Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

int WorkManager::VS_TaskAlive(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;

	EXCEPTION_TRY
		// Do nothing
		EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(FALSE, _T("SEQLOG -- Seq1001_Task_Alive Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	//m_fnPrintLog(FALSE, _T("SEQLOG -- Seq1001_Task_Alive Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);

	return nRet;
}

int	WorkManager::Seq_ManualChangeModel(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReceiveParam = pParam;
	ST_RECIPE_INFO_AOT* pStModelInfo = new ST_RECIPE_INFO_AOT;
	ST_GRABIMAGE_LINK_LIST* pStGrabLinkInfo = new ST_GRABIMAGE_LINK_LIST;
	int nsize = sizeof(pStModelInfo);
	pStModelInfo = (ST_RECIPE_INFO_AOT *)pReceiveParam;
	pReceiveParam += sizeof(ST_RECIPE_INFO_AOT);
	pStGrabLinkInfo = (ST_GRABIMAGE_LINK_LIST*)pReceiveParam;

	ST_GRAB_AREA_INFO_AOT stCurGrabInfo;
	ST_GRABIMAGE_LINKDATA Temp;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2110_ManualChangeModel StepNo=%d, RetVal=%d"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			// Change Recipe
			//pStProcInfo = (ST_PROC_INFO *)pParamData;			// Not used
			//tempParam += sizeof(ST_PROC_INFO);
			theApp.m_Config.SetModelInfo(pStModelInfo);

			m_fnPrintLog(FALSE, _T("CASE %d : UI >> Seq : Set Model Info"), nStepNo);
			break;
		case 2:
			// Change Recipe
			//pStProcInfo = (ST_PROC_INFO *)pParamData;			// Not used
			//tempParam += sizeof(ST_PROC_INFO);
			theApp.m_Config.SetGrabLinkInfo(pStGrabLinkInfo);

			m_fnPrintLog(FALSE, _T("CASE %d : UI >> Seq : Set Link Info"), nStepNo);

			break;
		//	break;
		//case 3:
		//	// 조명 세팅
		//	switch (theApp.m_Config.GetType())
		//	{
		//	case TYPE_AOT_CHIPPING:
		//		stCurGrabInfo = theApp.m_Config.GetAreaInfo(_T("PAD"));
		//		nRet += CmdEditSend(SEND_APPLY_LIGHT_PROPERTY, 0, sizeof(ST_GRAB_AREA_INFO_AOT), VS_LIGHT_TASK, (byte *)&stCurGrabInfo, CMD_TYPE_RES);

		//		break;
		//	}
		//	break;
		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2110_ManualChangeModel Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);

	//SAFE_DELETE(pStModelInfo);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2110_ManualChangeModel Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


//int	WorkManager::Seq_ManualGrabImage(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//
//	int nRet = TRUE;
//
//
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage START. Panel ID=%s"), strPanelID);
//	switch (theApp.m_Config.GetType())
//	{
//	case TYPE_AVI:
//		nRet = Seq_ManualGrabImage_AVI(pParam, nPrmSize);
//		break;
//	case TYPE_SVI:
//		nRet = Seq_ManualGrabImage_SVI(pParam, nPrmSize);
//		break;
//	case TYPE_APP:
//		nRet = Seq_ManualGrabImage_APP(pParam, nPrmSize);
//		break;
//	case TYPE_AMT:
//		nRet = Seq_ManualGrabImage_AMT(pParam, nPrmSize);		// AMT 검사 시컨스 신규 추가 180801 YSS
//		break;
//	case TYPE_BUMP:
//		nRet = Seq_ManualGrabImage_DUMP(pParam, nPrmSize);		// AMT 검사 시컨스 신규 추가 180801 YSS
//		break;
//	default:
//		break;
//	}
//
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage END. Panel ID=%s, RetVal=%d\n"), strPanelID, nRet);
//	return nRet;
//}


//int	WorkManager::Seq_ManualGrabImage_APP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	CString strTemp = _T("");
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//
//	// APP - Camera Mode 강제로 Standard Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 1;		// Mode 0:FreeRun 1:Standard
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// 패턴 진행 여부 체크
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//				nGrabCnt++;
//				nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//				isSeqBusy = false;
//			}
//			else
//			{
//				if (nGrabCnt >= theApp.m_Config.GetGrabCount())
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skiped !!!"), nStepNo);
//					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//				}
//			}
//			nStepNo = 18;
//			break;
//
//		case 19:
//			// Light Task - 첫 번째 Image Light Turn On
//			m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Controll Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)	// Tact 관련 Parallel_for 적용 검토
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Controll End."), nStepNo);
//			else
//				throw nRet;
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 20:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 1;
//			break;
//
//		case 2:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 3:
//			// 			if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//			// 				nVacuumType = 1;
//			// 			else
//			// 				nVacuumType = 2;
//			// 
//			// 			nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//			nRet = APP_OK;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			else
//				throw nRet;
//			break;
//
//		case 4:
//			m_fnPrintLog(FALSE, _T("CASE %d : APP Light Delay Time Start "), nStepNo);
//			Sleep(300);
//			m_fnPrintLog(FALSE, _T("CASE %d : APP Light Delay Time End "), nStepNo);
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			else
//				throw nRet;
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 6;	// case 7 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 4;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 5:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Controll Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Controll End."), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 6:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 7:
//			// Camera Task - Wait Grab End -> Save Image File to Local
//			stWaitGrabEndParam.bUseFileSave = TRUE;
//			// GVO, APP Manual Grab시, 저장되는 파일명 수정 171026  YSS
//			strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt, theApp.m_Config.GetCurStepName(nGrabCnt), 0);
//			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp);
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 8:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 8 (Complete)				
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 2;
//			break;
//
//		case 9:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);			// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 10:
//			// Main PC Task - Vacuum Off
//// 			nVacuumType = 2;	// Todo. Type 정의 필요
//// 
//// 			nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//			nRet = APP_OK;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			else
//				throw nRet;
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


//int	WorkManager::Seq_ManualGrabImage_AVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	CString strTemp = _T("");
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// 패턴 진행 여부 체크
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//				nGrabCnt++;
//				nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//				isSeqBusy = false;
//			}
//			else
//			{
//				if (nGrabCnt >= theApp.m_Config.GetGrabCount())
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skiped !!!"), nStepNo);
//					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//				}
//			}
//			break;
//
//		case 2:
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//			nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s)"), nStepNo, nGrabCnt + 1, strCurStepName);
//				nStepNo = 49;		// goto case 50
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s) Error !!!"), nStepNo, nGrabCnt + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 50:
//			// Light Task - Change Light Condition
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);	// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 69;		// goto case 70
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 70:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 2;		// goto case 3
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 7;	// case 8 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 4;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 5:
//			// PG Task - Change PG Condition - INDEX 번째 image
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			// Dust Pattern 일 경우 PG Task 내부에서 PG Off로 처리
//			// 			nPGState = 0;
//			// 			if( !_tcsicmp(strCurStepName, _T("DUST")) )
//			// 				nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK , (byte *)&nPGState, CMD_TYPE_RES);
//			// 			else
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//			nRet += CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s)"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s) Error !!!"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 6:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//
//		case 7:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			// Camera Task - Wait Grab End -> Save Image File to Local
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);
//			stWaitGrabEndParam.bUseFileSave = TRUE;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt + 1, theApp.m_Config.GetCurStepName(nGrabCnt), 0);
//			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp);
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 8 (Complete)				
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 10:
//			nPGState = 0;
//			nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);		// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


//int	WorkManager::Seq_ManualGrabImage_SVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	CString strTemp = _T("");
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// 패턴 진행 여부 체크
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//				nGrabCnt++;
//				nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//				isSeqBusy = false;
//			}
//			else
//			{
//				if (nGrabCnt >= theApp.m_Config.GetGrabCount())
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skiped !!!"), nStepNo);
//					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//				}
//			}
//			nStepNo = 18;
//			break;
//
//		case 19:
//			// PG Task - 첫 번째 Pattern ON
//			nPGState = 1;
//			nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//			Sleep(150);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set PG ON"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 20:
//
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//			nRet += CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			Sleep(150);
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON"), nStepNo);
//			else
//				throw nRet;
//			nStepNo = 1;
//			break;
//
//		case 2:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, i);
//				if (stCurCamCond.bUseCamera)
//					nRet += CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK + i, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 3:
//			// 				if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//			// 					nVacuumType = 1;
//			// 				else
//			// 					nVacuumType = 2;
//			// 
//			// 				nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			else
//				throw nRet;
//			break;
//
//		case 4:
//			// Camera Task - Camera Expose
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				if (theApp.m_Config.GetCameraConditions(nGrabCnt, i).bUseCamera)
//					nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK + i, 0, CMD_TYPE_RES);
//			}
//
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			else
//				throw nRet;
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 6;	// case 7 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 4;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 5:
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//			nRet += CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Controll"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 6:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, i);
//				if (stCurCamCond.bUseCamera)
//					nRet += CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK + i, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 7:
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				stWaitGrabEndParam.bUseFileSave = TRUE;
//				strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt + 1, theApp.m_Config.GetCurStepName(nGrabCnt), i);
//				_tcscpy(stWaitGrabEndParam.strSavePath, strTemp);
//				stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, i);
//				if (stCurCamCond.bUseCamera)
//					nRet += CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK + i, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		case 8:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 8 (Complete)				
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 2;
//			break;
//
//		case 9:
//			nPGState = 0;
//			nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off"), nStepNo);
//			else
//				throw nRet;
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//int	WorkManager::Seq_ManualInspectImage(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2112_ManualInspectImage Sequence START. Panel ID=%s"), strPanelID);
//
//	// for Tact 
//	m_fnCreateProcInfo(strPanelID, _T("Manual"), strModelID, 1, TRUE);	//2019.07.10 nStageNo
//
//	//return 0;
//	// Send
//	PARAM_INSPECT_START* prmInspStart = new PARAM_INSPECT_START;
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	CString strTemp = _T("");
//
//	int nGrabCnt = 0;
//
//	int nAlignPatternNum = 0;
//
//	// Start Grab Time
//	nRet = Seq_TactTimeData(strPanelID, TACT_GRAB, TACT_START);
//	// Start Total Tact Time
//	nRet = Seq_TactTimeData(strPanelID, TACT_TOTAL, TACT_START);
//
//	BOOL bFirstInspFlg = TRUE;
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2112_ManualInspectImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			if (!theApp.m_Config.IsUseStep(0))
//			{
//				m_fnPrintLog(TRUE, _T("SEQLOG -- Must be Inspect First Pattern (00_%s) !!!!"), theApp.m_Config.GetCurStepName(0));
//				throw 999;
//			}
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				throw nRet;
//			}
//
//			if (!theApp.m_Config.IsUseStep(nAlignPatternNum))
//			{
//				m_fnPrintLog(TRUE, _T("SEQLOG -- Must be Inspect Align Pattern (%02d_%s) !!!!"), nAlignPatternNum, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//				throw 999;
//			}
//			Sleep(17); // 19.02.25 메뉴얼 검사에서도 실제 Grab 시간 반영
//			break;
//
//		case 2:
//			strTemp.Format(_T("%s"), strFileDirectory);
//			prmInspStart->nInspType = 2;
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			for (int nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				prmInspStart->bUseCamera[nCamIndex] = theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex);
//				_tcscpy(prmInspStart->strImagePath[nCamIndex], strTemp);		// CString To TCHAR *
//				prmInspStart->nSeqMode[nCamIndex] = theApp.m_Config.GetCamPSMode(nGrabCnt, nCamIndex);
//			}
//			prmInspStart->nImageNum = nGrabCnt;
//
//			// Edge Image 일 경우 Alg. Task 에서 Align 실행 후 응답
//// 			if (nGrabCnt == 0)
//// 				nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_RES);
//// 			else
//			// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(TRUE, _T("SEQLOG -- Next Step - Total Count : %d, Current Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			else
//				throw nRet;
//
//			break;
//
//		case 3:
//			nGrabCnt++;			// 다음 Grab
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//				nStepNo--;
//			break;
//
//		case 4:
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 1;
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2112_ManualInspectImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// End Grab Time
//	nRet = Seq_TactTimeData(strPanelID, TACT_GRAB, TACT_END);
//
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2112_ManualInspectImage Sequence END. Panel ID=%s, StepNo=%d, RetVal=%d\n"), strPanelID, nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


int	WorkManager::Seq_AutoInspectGrabImage(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReceiveParam = pParam;

	/*BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);*/

	TCHAR strPanelID[50] = { 0, };		// 100 byte
	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);

	//BOOL nStartLine = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStartLine);

	//BOOL nTotalLine = *(int *)pReceiveParam;						pReceiveParam += sizeof(nTotalLine);


	//TCHAR strDirection[50] = { 0, };		// 100 byte
	//memcpy(strDirection, pReceiveParam, sizeof(strDirection));	pReceiveParam += sizeof(strDirection);

	//TCHAR strModelID[50] = { 0, };		// 100 byte
	//memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);

	//pReceiveParam += sizeof(int) * 2;

	//TCHAR strLotID[50] = { 0, };		// 100 byte
	//memcpy(strLotID, pReceiveParam, sizeof(strLotID));		pReceiveParam += sizeof(strLotID);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage Sequence START. Panel ID=%s"), strPanelID);

	// Report Tact Time
	//Seq_TactTimeData(strPanelID, TACT_TOTAL, TACT_START);
	//Seq_TactTimeData(strPanelID, TACT_GRAB, TACT_START);

	// for Tact
	//m_fnCreateProcInfo(strPanelID, strVirtualID, strModelID, nStageNo, bIsManual);	//2019.07.10 nStageNo

	int nRet = APP_OK;
	int nGrabStartFlag = 0;

	//2021.02.24 Grab Time 수동조절
	clock_t start, end;
	int nResult;

	//if (!bIsManual)
	//	nRet = CmdEditSend(SEND_GRAB_START, 0, sizeof(int), VS_UI_TASK, (byte*)&nGrabStartFlag, CMD_TYPE_RES); //CMD_TYPE_NORES -> CMD_TYPE_RES


	switch (theApp.m_Config.GetType())
	{
	//case TYPE_APP:
	//	nRet = Seq_AutoInspectGrabImage_APP(pParam, nPrmSize);
	//	break;
	//case TYPE_AVI:
	//	nRet = Seq_AutoInspectGrabImage_AVI(pParam, nPrmSize);
	//	break;
	//case TYPE_SVI:
	//	nRet = Seq_AutoInspectGrabImage_SVI(pParam, nPrmSize);
	//	break;
	//case TYPE_AMT:
	//	nRet = Seq_AutoInspectGrabImage_AMT(pParam, nPrmSize);		// AMT 검사 시컨스 신규 추가 180801 YSS
	//	break;
	//case TYPE_BUMP:
	//	nRet = Seq_AutoInspectGrabImage_BUMP(pParam, nPrmSize);		// AMT 검사 시컨스 신규 추가 180801 YSS
	//	break;
	case TYPE_AOT_CHIPPING:
		nRet = Seq_AutoInspectGrabImage_AOT_CHIPPING(pParam, nPrmSize);		// AOT CHIPPING 검사 시퀀스 추가
		break;
	case TYPE_ALM:
		nRet = Seq_AutoInspectGrabImage_ALM(pParam, nPrmSize);		// ALM 검사 시퀀스 추가
		break;
	default:
		break;
	}

	// Report Tact Time
	Seq_TactTimeData(strPanelID, TACT_GRAB, TACT_END);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage Sequence END. Panel ID=%s, RetVal=%d\n"), strPanelID, nRet);

	return nRet;
}


//int	WorkManager::Seq_AutoInspectGrabImage_APP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));	pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	BOOL bUseAsync = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bUseAsync);	// int -> BOOL
//	int  nTestMode = *(int  *)pReceiveParam;					pReceiveParam += sizeof(nTestMode);	// int
//	TCHAR strLotID[50] = { 0, };																				// Add By KYH LOTID
//	memcpy(strLotID, pReceiveParam, sizeof(strLotID));			pReceiveParam += sizeof(strLotID);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nCamIndex = 0;
//	int nNextStepInterval = 1;
//
//	int nAlignPatternNum = 0;
//	int nAsyncCount = 0;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
//	bool bRet = false;
//
//	bool bDust = false; // APP 조명 Delay Time 추가 - JSO
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//	ST_CAMERA_COND stCurCamCond;
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumTypeTemp = 1; // Vacuum 상태
//	int						nVacuumType = 1;
//
//	// APP - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 1;		// Mode 0:FreeRun 1:Standard
//
//	BOOL bFirstInspFlg = TRUE;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// 제어 PC GRAB.START 호출 시 각 PC Sequence 동기화
//
//	if (!bIsManual && bUseAsync)
//		bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, theApp.GetGrabLimitWaitTime());
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_APP StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			//	nRet = Seq_AutoChangeModel(pParam, nPrmSize);
//			//	if (nRet == APP_OK)
//			//		m_fnPrintLog(FALSE, _T("CASE %d : Check Model Change"), nStepNo);
//			//	else
//			//		throw nRet;
//			nStepNo = 48;	// goto case 50
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 49:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_CAMERA_TRIGGER_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			nStepNo = 28;
//			break;
//
//		case 29:
//			// 예외처리
//			if (!theApp.m_Config.IsUseStep(0))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab First Pattern (00_%s) !!!!"), theApp.m_Config.GetCurStepName(0));
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_FIRST_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_GET_ALIGN_NUM_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//
//			if (!theApp.m_Config.IsUseStep(nAlignPatternNum))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab Align Pattern (%02d_%s) !!!!"), nAlignPatternNum, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_ALIGN_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//			nStepNo = 1;
//			break;
//
//			// 현재 무조건 최초 패턴 찍어야 함. 찍지 않아도 되도록 수정하려면 Alg. Task 수정 필요.
//			// 		// 패턴 진행 여부 체크
//			// 		case 30:
//			// 			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			// 			{
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);				
//			// 				nGrabCnt++;		nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//			// 			}
//			// 			else
//			// 			{
//			// 				if (nGrabCnt < theApp.m_Config.GetGrabCount())
//			// 				{
//			// 					nStepNo = 1;	// goto case 2
//			// 				}
//			// 				else
//			// 				{
//			// 					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skip !!!"), nStepNo);
//			// 					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//			// 				}
//			// 			}
//			// 			break;
//
//		case 2:
//			// 최초 패턴 촬상 시작 전 각 PC Sequence 동기화
//			if (!bIsManual && bUseAsync)	// Dust Retry 시에는 동기화 사용 안함
//				bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 10000);
//
//			// Light Task - 첫 번째 Image Light Turn On
//			m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Controll Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Controll End."), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_LIGHT_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//#pragma omp parallel for
//			for (nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				if (theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex))
//				{
//					nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(ST_CAMERA_COND), VS_CAMERA_TASK + nCamIndex, (byte *)&theApp.m_Config.GetCameraConditions(nGrabCnt, nCamIndex), CMD_TYPE_RES);
//				}
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//				nVacuumType = 1;
//			else
//				nVacuumType = 2;
//
//			if (nVacuumType != nVacuumTypeTemp)
//			{
//				nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//				nVacuumTypeTemp = nVacuumType;
//				nRet = APP_OK;
//				if (nRet == APP_OK)
//				{
//					Sleep(SLEEP_TIME);
//					m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d), Sleep : %d"), nStepNo, nVacuumType, SLEEP_TIME);
//				}
//				else
//				{
//					VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//					throw nRet;
//				}
//			}
//			else
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State [Pass], Current State : %d"), nStepNo, nVacuumTypeTemp);
//			break;
//
//		case 5:
//			m_fnPrintLog(FALSE, _T("CASE %d : APP Light Delay Time Start "), nStepNo);
//			if (bDust == true)
//				Sleep(350); // 조명 안정화 시간 추가 - JSO
//			m_fnPrintLog(FALSE, _T("CASE %d : APP Light Delay Time End "), nStepNo);
//			// Camera Task - Camera Expose
//#pragma omp parallel for
//			for (nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				if (theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex))
//				{
//					nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK + nCamIndex, 0, CMD_TYPE_RES);
//				}
//			}
//
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_EXPOSE_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 7;	// case 8 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 5;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 6:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Controll Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Controll End."), nStepNo);
//				bDust = true; // 면조명에만 딜레이 적용 - JSO				
//			}
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_LIGHT_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 7:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//#pragma omp parallel for
//			for (nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				if (theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex))
//				{
//					stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, nCamIndex);		// 현재 다중 카메라 대응 안됨 Todo
//
//					nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(ST_CAMERA_COND), VS_CAMERA_TASK + nCamIndex, (byte *)&stCurCamCond, CMD_TYPE_RES);
//				}
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = nGrabCnt;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//
//#pragma omp parallel for
//			for (nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				if (theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex))
//				{
//					nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK + nCamIndex, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//				}
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_WAIT_GRAB_END_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Alg Task - Inspection (NRS)
//			if (!bIsManual)
//				prmInspStart->nInspType = 0;
//			else
//				prmInspStart->nInspType = 1;
//			prmInspStart->nStageNo = nStageNo;
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualID, sizeof(prmInspStart->strVirtualID));
//			for (int nCamIndex = 0; nCamIndex < NUM_INSP_CAMERA; nCamIndex++)
//			{
//				prmInspStart->bUseCamera[nCamIndex] = theApp.m_Config.IsUseCamera(nGrabCnt, nCamIndex);
//				prmInspStart->nSeqMode[nCamIndex] = theApp.m_Config.GetCamPSMode(nGrabCnt, nCamIndex);
//			}
//			prmInspStart->nImageNum = nGrabCnt;
//			// Edge Image 일 경우 Alg. Task 에서 Align 실행 후 응답
//// 			if (nGrabCnt == 0)
//// 				nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_RES);
//// 			else
//			// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_START_INSPECTION_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 10:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 9 (Complete)			
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 11:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);		// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_LIGHT_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 12:
//			// Main PC Task - Vacuum Off
//			nVacuumType = 1;	// Todo. Type 정의 필요
//			if (nVacuumTypeTemp != 1)
//			{
//				nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//				nRet = APP_OK;
//				if (nRet == APP_OK)
//					m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//				else
//				{
//					VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//					throw nRet;
//				}
//			}
//			else
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State [Pass], Current State : %d"), nStepNo, nVacuumTypeTemp);
//
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_APP Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//
//				// 모든 Auto Sequence Event Reset
//				for (int i = 0; i < MAX_PC_COUNT; i++)
//					for (int j = 0; j < MAX_AUTO_SEQ_ASNC_POINT; j++)
//						ResetEvent(m_hAsycAutoSeq[i][j]);
//			}
//
//	} while (isRunSequence);
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_APP Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//int	WorkManager::Seq_AutoInspectGrabImage_AVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));	pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	BOOL bUseAsync = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bUseAsync);	// int -> BOOL
//	int  nTestMode = *(int  *)pReceiveParam;					pReceiveParam += sizeof(nTestMode);	// int
//	TCHAR strLotID[50] = { 0, };																				// Add By KYH LOTID
//	memcpy(strLotID, pReceiveParam, sizeof(strLotID));			pReceiveParam += sizeof(strLotID);
//
//	TCHAR tMtpDrv[50] = { 0, };
//	memset(tMtpDrv, 0, sizeof(tMtpDrv));
//	CString strMtpDrv = _T("");
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CString strOrgPanelID(strPanelID);
//	if (nTestMode == 2)		// Test Mode 2 일 경우 Panel ID 를 [Panel ID]_반복횟수 로 Casting 하여 사용하므로 반복횟수 제거
//		strOrgPanelID = strOrgPanelID.Left(strOrgPanelID.ReverseFind('_'));
//
//	// 검사 사용 유무 플래그
//	BOOL bUseInspect = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Inspect"), 0, PATH_INIT_FILE);
//
//	// Align Camera 기능 사용 여부 - Grab Sequence 시작 전에 매번 읽어와야 함 (GUI 에서 실시간 갱신)
//	int nUseAlignCamera = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Adjust_Theta"), 0, PATH_INIT_FILE) > 0 ? TRUE : nTestMode;
//	// Grab Sequence - Dust 결과 대기 기능 사용 여부
//	BOOL bUseDustRetry = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Dust_Retry"), 0, PATH_INIT_FILE);
//
//	GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
//	strMtpDrv.Format(_T("%s"), tMtpDrv);
//	ResetEvent(m_hDustEnd);
//
//	int nSiteTypeNo = GetPrivateProfileInt(_T("Common"), _T("SITE TYPE"), 0, PATH_INIT_FILE);
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	int nAlignPatternNum = 0;
//
//	int nAsyncCount = 0;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
//	bool bRet = false;
//	//KYH 2021.10.21 MTP DISPLAY_ABNORMAL
//	bool bAD = true;
//
//	int nDustRetryCount = 0;		// Dust 이상 시 반복 횟수
//	const int DUST_RETRY_LIMIT = 1;	// Dust 이상 시 반복 제한 (1==1회만 Retry)
//	m_bNeedRetry = false;
//	m_bIsNormalDust = false;
//
//	static bool bFirst = true;	// 임시 작업 - UI 속도 개선 후 지울 것
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//
//	PARAM_WAIT_GRAB_END		stWaitGrabEndParam;
//	ST_PG_DATA*				pData = NULL;
//	ST_PG_INFO				stPgInfo;
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//	CString					strOriginPGInfoPath = _T("");
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//	BOOL bFirstInspFlg = TRUE;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// 제어 PC GRAB.START 호출 시 각 PC Sequence 동기화
//	if (!bIsManual && bUseAsync)
//		bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, theApp.GetGrabLimitWaitTime());
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		nStepNo++;
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		switch (nStepNo)
//		{
//		case 1:
//			// CSOT 사이트가 아니면 수행, CSOT는 MTP 파일 사용하지 않음
//			// PG Voltage txt read
//			if (nSiteTypeNo != 4)
//			{
//				strOriginPGInfoPath.Format(_T("%s.ini"), strOrgPanelID);
//
//				if (!bIsManual)
//				{
//					if (theApp.m_Config.UpdatePGVoltInfo(strMtpDrv, strOriginPGInfoPath))
//					{
//						stPgInfo = theApp.m_Config.GetPgInfo();
//
//						for (int nCount = 0; nCount < MAX_GRAB_STEP_COUNT; nCount++)
//						{
//							m_fnPrintLog(FALSE, _T("AVI_PatternName : %s, R : %d, G : %d, B : %d"), stPgInfo.stPgData[nCount].strPatternName, stPgInfo.stPgData[nCount].wVoltage[0], stPgInfo.stPgData[nCount].wVoltage[1], stPgInfo.stPgData[nCount].wVoltage[2]);
//
//							if ((0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("WHITE"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("R"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("G"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("B"))))
//							{
//								m_fnPrintLog(FALSE, _T("PatternName2 : %s, R : %d, G : %d, B : %d"), stPgInfo.stPgData[nCount].strPatternName, stPgInfo.stPgData[nCount].wVoltage[0], stPgInfo.stPgData[nCount].wVoltage[1], stPgInfo.stPgData[nCount].wVoltage[2]);
//
//								if (stPgInfo.stPgData[nCount].wVoltage[0] == 7500 && stPgInfo.stPgData[nCount].wVoltage[1] == 7500 && stPgInfo.stPgData[nCount].wVoltage[2] == 7500)
//								{
//									bAD = false;
//									m_fnPrintLog(FALSE, _T("bAD Check(false)"));
//									break;
//								}
//							}
//						}
//
//
//						if (bAD)
//							nRet = CmdEditSend(SEND_SEND_VOLTAGEINFO, 0, (ULONG)sizeof(ST_PG_INFO), VS_PG_TASK, (byte *)&stPgInfo, CMD_TYPE_RES);
//
//						if (nRet != APP_OK)
//						{
//							VS_Send_Alarm_To_MainPC(eALARM_MTP_SETTING_ERROR, eALARMTYPE_HEAVY);
//							throw 999;
//						}
//
//					}
//					else
//					{
//						m_fnPrintLog(FALSE, _T("SetPgInfo fail - %s"), strOriginPGInfoPath);
//						VS_Send_Alarm_To_MainPC(eALARM_READ_FAIL_MTP_RESULT, eALARMTYPE_HEAVY);
//					}
//				}
//			}
//
//			nStepNo = 69;		// goto case 70
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 70:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_CAMERA_TRIGGER_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			// 2017.06.21 HW Align Sequence 추가	
//			if (nUseAlignCamera == 1 && bAD)
//				nStepNo = 19;	// goto case 20 - Align Camera		
//			else
//				nStepNo = 28;	// goto case 29 - Grab Sequence
//			break;
//
//			// 2017.06.21 HW Align Sequence 추가
//		case 20:
//			nRet = Seq_AlignCamera(pParam, nPrmSize);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Align Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Align Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 28;	// goto case 29 - 원래 Sequence 시작
//			break;
//
//		case 29:
//			// 예외처리
//			if (!theApp.m_Config.IsUseStep(0))
//			{
//				_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(0));
//				m_fnPrintLog(TRUE, _T("Must be Grab First Pattern (00_%s) !!!!"), strCurStepName);
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_FIRST_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_GET_ALIGN_NUM_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			if (!theApp.m_Config.IsUseStep(nAlignPatternNum))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab Align Pattern (%02d_%s) !!!!"), nAlignPatternNum, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_ALIGN_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//			nStepNo = 1;
//			break;
//
//			// 현재 무조건 최초 패턴 찍어야 함. 찍지 않아도 되도록 수정하려면 Alg. Task 수정 필요.
//			// 		// 패턴 진행 여부 체크
//			// 		case 30:
//			// 			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			// 			{
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);				
//			// 				nGrabCnt++;		nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//			// 			}
//			// 			else
//			// 			{
//			// 				if (nGrabCnt < theApp.m_Config.GetGrabCount())
//			// 				{
//			// 					nStepNo = 1;	// goto case 2
//			// 				}
//			// 				else
//			// 				{
//			// 					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skip !!!"), nStepNo);
//			// 					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//			// 				}
//			// 			}
//			// 			break;
//
//		case 2:
//			// 최초 패턴 촬상 시작 전 각 PC Sequence 동기화
//			// 임시 주석처리 - JSO
//// 			if (!bIsManual && bUseAsync && nDustRetryCount == 0)	// Dust Retry 시에는 동기화 사용 안함
//// 				bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 2000);
//			//Sleep(300);			// PG Off 후 안정화 시간 추가 - PG Protocol 검증 후 지울 것
//
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//
//			if (bAD)
//			{
//				nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CHANGE_PATTERN Skip(%s)!!!"), strCurStepName);
//				nRet = APP_OK;
//			}
//
//
//			if (nRet == APP_OK)
//			{
//				if (bAD)
//					m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s)"), nStepNo, nGrabCnt, strCurStepName);
//
//				nStepNo = 49;		// goto case 50
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s) Error !!!"), nStepNo, nGrabCnt, strCurStepName);
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_PATTERN_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//
//
//			//nStepNo = 2;
//			break;
//
//		case 50:
//			// 임시 동기화 포인트 - JSO
//			if (!bIsManual && bUseAsync && nDustRetryCount == 0)	// Dust Retry 시에는 동기화 사용 안함
//				bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 2000);
//			// Light Task - Change Light Condition
//			m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Control End."), nStepNo);
//				nStepNo = 2;		// goto case 3
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Light Control Error !!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_LIGHT_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//				nVacuumType = 1;
//			else
//				nVacuumType = 2;
//
//			//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d) Error !!!"), nStepNo, nVacuumType);
//				VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 5:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_EXPOSE_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 8;	// case 9 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Next Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 5;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 6:
//			// PG Task - Change PG Condition - INDEX 번째 image
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			// Dust Pattern 일 경우 PG Task 내부에서 PG Off로 처리
//			// 			nPGState = 0;
//			// 			if( !_tcsicmp(strCurStepName, _T("DUST")) )
//			// 				nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK , (byte *)&nPGState, CMD_TYPE_RES);
//			// 			else
//
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//
//			if (bAD)
//			{
//				nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CHANGE_PATTERN Skip(%s)!!!"), strCurStepName);
//				nRet = APP_OK;
//			}
//
//
//			if (nRet == APP_OK)
//			{
//				if (bAD)
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s)"), nStepNo, nGrabCnt + nNextStepInterval, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s) Error !!!"), nStepNo, nGrabCnt + nNextStepInterval, strCurStepName);
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_PG_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//
//
//			break;
//
//		case 7:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_LIGHT_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//
//		case 8:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = nGrabCnt;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_WAIT_GRAB_END_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 10:
//			// Alg Task - Inspection (NRS)
//			if (!bIsManual)
//				prmInspStart->nInspType = 0;
//			else
//				prmInspStart->nInspType = 1;
//			prmInspStart->nStageNo = nStageNo;
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualID, sizeof(prmInspStart->strVirtualID));
//			memcpy((TCHAR *)prmInspStart->strLotID, strLotID, sizeof(prmInspStart->strLotID));
//
//			prmInspStart->bUseCamera[0] = TRUE;						// Camera Num - 0 고정
//			prmInspStart->nImageNum = nGrabCnt;
//			prmInspStart->nSeqMode[0] = theApp.m_Config.GetCamPSMode(nGrabCnt, 0);
//			prmInspStart->nRetryCnt = nDustRetryCount;
//
//			// MTP 보정 후 색좌표 송부			
//// 			paramTemp = theApp.m_Config.GetPGParam(nGrabCnt);
//// 			prmInspStart->dPatternCIE[0] = paramTemp.pVoltageInfo.dColor_X;
//// 			prmInspStart->dPatternCIE[1] = paramTemp.pVoltageInfo.dColor_Y;
//// 			prmInspStart->dPatternCIE[2] = paramTemp.pVoltageInfo.dLuminance;
//			pData = theApp.m_Config.GetVoltageInfo(theApp.m_Config.GetCurStepName(nGrabCnt));
//			if (pData != NULL)
//			{
//				prmInspStart->dPatternCIE[0] = pData->dColor_X;
//				prmInspStart->dPatternCIE[1] = pData->dColor_Y;
//				prmInspStart->dPatternCIE[2] = pData->dLuminance;
//			}
//			// 첫번째 Image 일 경우 Alg. Task 에서 Align 실행 후 응답
//// 				if (nGrabCnt == 0)
//// 					nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_RES);
//// 				else
//			// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d Error !!!"), nStepNo, nGrabCnt);
//				VS_Send_Alarm_To_MainPC(eALARM_START_INSPECTION_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 9 (Complete)			
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 12:
//			nPGState = 0;
//			nRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_PG_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 13:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);			// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_LIGHT_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//			// Dust Retry 기능 - Auto Mode 에서만 동작
//		case 14:
//			if (bUseDustRetry && !bIsManual && bUseInspect)
//			{
//				bRet = WaitForCheckDustAtAlgTask(60000);						// Dust 결과 나올때까지 60초까지 대기				
//
//				if (bRet && !m_bIsNormalDust)	// Auto Mode 에서 정상적으로 결과 수신했는데 Dust Abnormal 일 경우
//				{
//					if (m_bNeedRetry && nDustRetryCount < DUST_RETRY_LIMIT)
//					{
//						m_fnPrintLog(FALSE, _T("CASE %d : Dust Error - Retry(%d) Grab Sequence !!!"), nStepNo, nDustRetryCount + 1);
//						nDustRetryCount++;
//						nGrabCnt = 0;
//						nNextStepInterval = 1;
//						//bUseAsync = FALSE;	// Dust Retry 시 동기화 기능 사용 안함
//						nStepNo = 28;	// Align Sequence 제외한 Grab Sequence 재시작
//					}
//					else
//					{
//						m_fnPrintLog(FALSE, _T("CASE %d : Dust Error !!!"), nStepNo);
//						//throw 6000;		// 중알람 띄우므로 GRAB.END.ERR 송부 안함
//					}
//				}
//				else if (!bRet)	// 결과 못받음
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Wait Dust Result Error !!!"), nStepNo);
//					VS_Send_Alarm_To_MainPC(eALARM_WAIT_DUST_RESULT_ERROR, eALARMTYPE_HEAVY);
//					throw 14000;
//				}
//				else	// 정상 종료
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Normal Dust"), nStepNo);
//				}
//			}
//			// 동기화 30초 대기
//			// PC 1, 2 모두 Dust Fail 이 났을 경우 현재는 서로 결과값을 체크하는 구문이 없으므로
//			// 끝나는 시점을 동기화하여 다음 촬상 시퀀스에 영향이 없도록 함.
//			// 필요 시 서로 결과값 받아와서 동기화 여부 결정하는 구문 작업 필요.
//			if (!bIsManual && bUseAsync)
//				WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 2000);
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				// 에러 발생 시 모든 동기화 포인트 클리어
//				int nAsyncClear = MAX_AUTO_SEQ_ASNC_POINT;
//				nErrRet = CmdEditSend(SEND_ASYNC_SEQUENCE, 0, sizeof(int), VS_OTHER_SEQ_TASK, (byte *)&nAsyncClear, CMD_TYPE_NORES);
//				// 모든 Auto Sequence Event Reset
//				for (int i = 0; i < MAX_PC_COUNT; i++)
//					for (int j = 0; j < MAX_AUTO_SEQ_ASNC_POINT; j++)
//						ResetEvent(m_hAsycAutoSeq[i][j]);
//				ResetEvent(m_hDustEnd);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	int Vcm = 1;
//	//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(int), VS_MAIN_PC_TASK, (byte*)&Vcm, CMD_TYPE_RES);
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


//int	WorkManager::Seq_AutoInspectGrabImage_SVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));	pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	TCHAR tMtpDrv[50] = { 0, };
//	memset(tMtpDrv, 0, sizeof(tMtpDrv));
//	CString strMtpDrv = _T("");
//
//	if (theApp.m_Config.GetPCNum() == 1 && theApp.m_Config.GetSiteNum() == 2)//사이트 번호  CSOT까지 고려해야 할듯
//		GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
//	else if (theApp.m_Config.GetPCNum() == 2 && theApp.m_Config.GetSiteNum() == 2)
//		GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT_SVI2"), _T("Y:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
//	else
//		GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
//
//	strMtpDrv.Format(_T("%s"), tMtpDrv);
//
//	int nSiteTypeNo = GetPrivateProfileInt(_T("Common"), _T("SITE TYPE"), 0, PATH_INIT_FILE);
//	int nGrabDelay = GetPrivateProfileInt(_T("INSPECT"), _T("Grab_Delay"), 0, PATH_INIT_FILE);
//
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	bool bAD_SVI = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//
//	int nAlignPatternNum = 0;
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	ST_PG_DATA*				pData = NULL;
//	ST_PG_INFO				stPgInfo;
//	CString					strOriginPGInfoPath = _T("");
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//	BOOL bFirstInspFlg = TRUE;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_SVI StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//
//		case 1:
//			// CSOT 사이트가 아니면 수행, CSOT는 MTP 파일 사용하지 않음
//			// PG Voltage txt read		
//			if (nSiteTypeNo != 4)
//			{
//				strOriginPGInfoPath.Format(_T("%s.ini"), strPanelID);
//
//				if (!bIsManual)
//				{
//					if (theApp.m_Config.UpdatePGVoltInfo(strMtpDrv, strOriginPGInfoPath))
//					{
//						stPgInfo = theApp.m_Config.GetPgInfo();
//
//						for (int nCount = 0; nCount < MAX_GRAB_STEP_COUNT; nCount++)
//						{
//							if ((0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("WHITE"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("R"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("G"))) || (0 == _tcscmp(stPgInfo.stPgData[nCount].strPatternName, _T("B"))))
//							{
//								m_fnPrintLog(FALSE, _T("SVI_PatternName : %s, R : %d, G : %d, B : %d"), stPgInfo.stPgData[nCount].strPatternName, stPgInfo.stPgData[nCount].wVoltage[0], stPgInfo.stPgData[nCount].wVoltage[1], stPgInfo.stPgData[nCount].wVoltage[2]);
//
//								if (stPgInfo.stPgData[nCount].wVoltage[0] == 7500 && stPgInfo.stPgData[nCount].wVoltage[1] == 7500 && stPgInfo.stPgData[nCount].wVoltage[2] == 7500)
//								{
//									bAD_SVI = false;
//									m_fnPrintLog(FALSE, _T("bAD_SVI Check(false)"));
//									break;
//								}
//							}
//				
//
//						}
//
//						m_fnPrintLog(FALSE, _T("bAD_SVI Check : %d"), bAD_SVI);
//
//						if (bAD_SVI)
//							nRet = CmdEditSend(SEND_SEND_VOLTAGEINFO, 0, (ULONG)sizeof(ST_PG_INFO), VS_PG_TASK, (byte *)&stPgInfo, CMD_TYPE_RES);
//
//						if (nRet != APP_OK)
//						{
//							VS_Send_Alarm_To_MainPC(eALARM_MTP_SETTING_ERROR, eALARMTYPE_HEAVY);
//							throw 999;
//						}
//					}
//					else
//					{
//						m_fnPrintLog(FALSE, _T("SetPgInfo fail - %s"), strOriginPGInfoPath);
//						VS_Send_Alarm_To_MainPC(eALARM_READ_FAIL_MTP_RESULT, eALARMTYPE_HEAVY);
//					}
//				}
//			}
//
//			nStepNo = 28;	// goto case 29
//			break;
//
//		case 29:
//			// 예외처리
//			if (!theApp.m_Config.IsUseStep(0))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab First Pattern (00_%s) !!!!"), theApp.m_Config.GetCurStepName(0));
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_FIRST_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_GET_ALIGN_NUM_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			if (!theApp.m_Config.IsUseStep(nAlignPatternNum))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab Align Pattern (%02d_%s) !!!!"), nAlignPatternNum, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//				VS_Send_Alarm_To_MainPC(eALARM_MUST_BE_ALIGN_GRAB, eALARMTYPE_HEAVY);
//				throw 999;
//			}
//			nStepNo = 1;
//			break;
//
//			// 현재 무조건 최초 패턴 찍어야 함. 찍지 않아도 되도록 수정하려면 Alg. Task 수정 필요.
//			// 			// 패턴 진행 여부 체크
//			// 			case 30:
//			// 				if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			// 				{
//			// 					m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);				
//			// 					nGrabCnt++;		nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//			// 				}
//			// 				else
//			// 				{
//			// 					if (nGrabCnt < theApp.m_Config.GetGrabCount())
//			// 					{
//			// 						nStepNo = 1;	// goto case 2
//			// 					}
//			// 					else
//			// 					{
//			// 						m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skip !!!"), nStepNo);
//			// 						throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//			// 					}
//			// 				}
//			// 				break;
//
//		case 2:
//			// PG Task - 첫 번째 Pattern ON
//
//			nPGState = 1;
//
//			if (bAD_SVI)
//			{
//				nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("PG_ON Skip!!!"));
//				nRet = APP_OK;
//			}
//
//			Sleep(150);
//
//			if (bAD_SVI)
//			{
//				if (nRet == APP_OK)
//					m_fnPrintLog(FALSE, _T("CASE %d : Set PG ON"), nStepNo);
//				else
//				{
//					VS_Send_Alarm_To_MainPC(eALARM_PG_ON_ERROR, eALARMTYPE_HEAVY);
//					throw nRet;
//				}
//			}
//
//
//			nStepNo = 20;
//			break;
//		case 21:
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//
//			if (bAD_SVI)
//			{
//				nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CHANGE_PATTERN Skip(%s)!!!"), theApp.m_Config.GetCurStepName(nGrabCnt));
//				nRet = APP_OK;
//			}
//
//
//			Sleep(150);
//
//
//			if (nRet == APP_OK)
//			{
//				nStepNo = 2;		// goto case 50
//				if (bAD_SVI)
//					m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON"), nStepNo);
//			}
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_PATTERN_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//
//
//
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, i);
//				if (stCurCamCond.bUseCamera)
//					nRet += CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK + i, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_FIRST_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//				nVacuumType = 1;
//			else
//				nVacuumType = 2;
//
//			//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 5:
//			// Camera Task - Camera Expose
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				if (theApp.m_Config.GetCameraConditions(nGrabCnt, i).bUseCamera)
//				{
//					Sleep(nGrabDelay);
//					m_fnPrintLog(FALSE, _T("Expose Delay = %d"), nGrabDelay);
//					nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK + i, 0, CMD_TYPE_RES);
//				}
//
//			}
//
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_EXPOSE_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 7;	// case 8 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 5;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 6:
//			// PG Task - Change PG Condition - INDEX 번째 image				
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//
//			if (bAD_SVI)
//			{
//				nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CHANGE_PATTERN Skip(%s)!!!"), theApp.m_Config.GetCurStepName(nGrabCnt));
//				nRet = APP_OK;
//			}
//
//
//			if (nRet == APP_OK)
//			{
//				nStepNo = 6;		// goto case 7
//				if (bAD_SVI)
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Controll"), nStepNo);
//			}
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_PG_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//
//
//			break;
//
//		case 7:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, i);
//				if (stCurCamCond.bUseCamera)
//				{
//
//					nRet += CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK + i, (byte *)&stCurCamCond, CMD_TYPE_RES);
//				}
//
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_NEXT_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = nGrabCnt;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				if (theApp.m_Config.GetCameraConditions(nGrabCnt, i).bUseCamera)
//					nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK + i, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			}
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_WAIT_GRAB_END_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Alg Task - Inspection (NRS)
//			if (!bIsManual)
//				prmInspStart->nInspType = 0;
//			else
//				prmInspStart->nInspType = 1;
//			prmInspStart->nStageNo = nStageNo;
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualID, sizeof(prmInspStart->strVirtualID));
//			for (int i = 0; i < NUM_INSP_CAMERA; i++)
//			{
//				prmInspStart->bUseCamera[i] = theApp.m_Config.GetCameraConditions(nGrabCnt, i).bUseCamera;
//				prmInspStart->nSeqMode[i] = theApp.m_Config.GetCamPSMode(nGrabCnt, i);
//			}
//
//			prmInspStart->nImageNum = nGrabCnt;
//
//			// MTP 보정 후 색좌표 송부
//// 				paramTemp = theApp.m_Config.GetPGParam(nGrabCnt);
//// 				prmInspStart->dPatternCIE[0] = paramTemp.pVoltageInfo.dColor_X;
//// 				prmInspStart->dPatternCIE[1] = paramTemp.pVoltageInfo.dColor_Y;
//// 				prmInspStart->dPatternCIE[2] = paramTemp.pVoltageInfo.dLuminance;
//			pData = theApp.m_Config.GetVoltageInfo(theApp.m_Config.GetCurStepName(nGrabCnt));
//			if (pData != NULL)
//			{
//				prmInspStart->dPatternCIE[0] = pData->dColor_X;
//				prmInspStart->dPatternCIE[1] = pData->dColor_Y;
//				prmInspStart->dPatternCIE[2] = pData->dLuminance;
//			}
//			// Edge Image 일 경우 Alg. Task 에서 Align 실행 후 응답
//// 				if (nGrabCnt == 0)
//// 					nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_RES);
//// 				else
//				// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_START_INSPECTION_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 10:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 9 (Complete)			
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 11:
//			// Pg Task - Pg Turn Off All
//			nPGState = 0;
//			nRet += CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			else
//			{
//				VS_Send_Alarm_To_MainPC(eALARM_PG_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_SVI Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// 2019.12.06 yskim
//	// =========== 그랩 모두 끝난 후 배큠 다시 잡도록 수정 =============
//	nVacuumType = 1;
//	//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//	if (nRet == APP_OK)
//		m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//	else
//		VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//	// =========== 그랩 모두 끝난 후 배큠 다시 잡도록 수정 =============
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_SVI Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

int	WorkManager::Seq_TactTimeData(TCHAR* strPanelId, CString strTactName, CString strTactState, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{

	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;
	TACT_TIME_DATA stTactTimeData;
	// Send /////////////////////////////////////////////////////////////////////////////////////////////////////////

	memcpy(stTactTimeData.strPanelID, strPanelId, sizeof(TCHAR) * 50);
	_tcscpy(stTactTimeData.strTactName, strTactName);
	_tcscpy(stTactTimeData.strTactState, strTactState);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Response /////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReturnParam = NULL;
	ST_PROC_INFO* pTempProcInfo = NULL;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	do
	{
		EXCEPTION_TRY
			// Grab / Classify End 에서 동시에 호출할 경우 SEQUENCE_TASK_SEQUENCE_IS_BUSY Return 하여 Error 발생
			// 			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			// 				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			// 			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			// 			{
			// 				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			// 			}

			isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq10100_stTactTimeData StepNo=%d, RetVal=%d"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:


			nRet = CmdEditSend(SEND_UI_TACT_TIME_DATA, 0, sizeof(TACT_TIME_DATA), VS_UI_TASK, (byte *)&stTactTimeData, CMD_TYPE_NORES);

			m_fnPrintLog(FALSE, _T("CASE %d : Send UI Tact Data"), nStepNo);
			break;

		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq10100_stTactTimeData Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq10100_stTactTimeData Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

// 2017.09.28 Edit by IKM - S
//int	WorkManager::Seq_AutoChangeModel(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));		pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	// Send /////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ULONG lSendParamSize = sizeof(int) + sizeof(TCHAR) * 50;
//
//	byte* pSendParam = new byte[lSendParamSize];
//	memset(pSendParam, 0, lSendParamSize);
//
//	//ST_PROC_INFO* pStProcInfo = new ST_PROC_INFO;
//	//memset(pStProcInfo, 0, sizeof(ST_PROC_INFO));
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// Response /////////////////////////////////////////////////////////////////////////////////////////////////////
//	//byte* pReturnParam = NULL;
//	//ST_PROC_INFO* pTempProcInfo = NULL;
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	////////////// PG Voltage Info///////////////////////////////////////////////////////////////////////////////////
//	//PARAM_PG *pStPgParam = new PARAM_PG;
//	//memset(pStPgParam, 0, sizeof(PARAM_PG));
//
//	//CString strPgInfoPath;
//
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2121_AutoChangeModel StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// UI Task - Change Recipe
//			//pStProcInfo->bFindModel = FALSE;
//			//pStProcInfo->nPcNum = theApp.m_Config.GetPCNum();
//			//_tcscpy(pStProcInfo->strModelID, strModelID);
//			*(int *)pSendParam = theApp.m_Config.GetPCNum();	pSendParam += sizeof(int);
//			memcpy(pSendParam, strModelID, sizeof(TCHAR) * 50);
//
//			nRet = CmdEditSend(SEND_CHANGE_MODEL, 0, lSendParamSize, VS_UI_TASK, pSendParam, CMD_TYPE_RES);
//			m_fnPrintLog(FALSE, _T("CASE %d : Seq >> UI : Find Model"), nStepNo);
//			break;
//
//			// 		case 2:
//			// 			// Change Recipe
//			// 			// Response Buffer
//			// 			pReturnParam = pSendParam;
//			// 			pTempProcInfo = (ST_PROC_INFO *)pReturnParam;		pReturnParam += sizeof(ST_PROC_INFO);
//			// 			if (pTempProcInfo->bFindModel)
//			// 			{
//			// 				theApp.m_Config.SetModelInfo((ST_MODEL_INFO *)pReturnParam);
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Change Model Info"), nStepNo);
//			// 			}
//			// 			else
//			// 			{				
//			// 				nStepNo = 999;
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Not Found Model :"), nStepNo);
//			// 			}
//			// 			
//			// 			break;
//
//			// 		case 3: 
//			// 			// PG Voltage txt read
//			// 			// PanelID(MTP_Result).txt 파일 읽어서 m_stPgParam에 넣어줌.
//			// 			
//			// 			//strPgInfoPath.Format(theApp.m_Config.GETDRV() + _T(":\\754A77E003C1BAQ06(MTP_Result).txt"));
//			// 			strPgInfoPath.Format(_T("x:\\%s.ini"), strPanelID);
//			// 			if(!theApp.m_Config.SetPgInfo(strPgInfoPath))
//			// 			{
//			// 				nStepNo = 999;
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : SetPgInfo fail - %s"), nStepNo, strPgInfoPath);
//			// 			}
//			// 
//			// 			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2121_AutoChangeModel Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	SAFE_DELETE_ARR(pSendParam);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2121_AutoChangeModel Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}
// 2017.09.28 Edit by IKM - E

//int	WorkManager::Seq_ClassifyEnd(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	BOOL bFindProcInfo = FALSE;
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//	PARAM_CLASSIFY_END* pPrmClassifyEnd = new PARAM_CLASSIFY_END;
//	memcpy((byte *)pPrmClassifyEnd, pReceiveParam, sizeof(PARAM_CLASSIFY_END));		pReceiveParam += sizeof(PARAM_CLASSIFY_END);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// for Tact ///////////////////////////////////////////////////////////////////////////////////////////////////////
//	std::vector<ST_PANEL_PROC_INFO>::iterator itr(find(theApp.m_vPanelProcInfo.begin(), theApp.m_vPanelProcInfo.end(), pPrmClassifyEnd->strPanelID));
//	if (itr != theApp.m_vPanelProcInfo.end())		// 동일한 Panel ID 찾았을 경우
//		itr->timeEnd = CTime::GetCurrentTime();
//	else
//		throw 800;
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// Send //////////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte bSendParam[1000];
//	byte* pSendParam = bSendParam;
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	CTimeSpan timeTact;
//
//	do
//	{
//		EXCEPTION_TRY
//
//			// 방금 들어왔는데, isSeqBusy가 True 일 경우 최대 5초간 대기 190305 YWS
//			if (nStepNo == 0 && isSeqBusy)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_Busy_Start"), nStepNo);
//				for (int nCheck = 0; nCheck <= 50; nCheck++)
//				{
//					Sleep(100);
//					if (!isSeqBusy)
//					{
//						m_fnPrintLog(FALSE, _T("Seq_Check_Not_Busy !"), nStepNo);
//						break;
//					}
//				}
//				m_fnPrintLog(FALSE, _T("Seq_Busy_End"), nStepNo);
//			}
//
//		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//			return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//		{
//			return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//		}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2130_ClassifyEnd StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// UI Task - Classify End
//			pPrmClassifyEnd->nPCNum = theApp.m_Config.GetPCNum();
//			pPrmClassifyEnd->bIsManual = itr->bIsManual;
//			memcpy(pPrmClassifyEnd->strModelID, itr->strModelID, sizeof(pPrmClassifyEnd->strModelID));
//			memcpy(pPrmClassifyEnd->strVirtualID, itr->strVirtualID, sizeof(pPrmClassifyEnd->strVirtualID));
//			_tcscpy(pPrmClassifyEnd->strStartTime, itr->timeStart.Format(_T("%Y-%m-%d %H:%M:%S")));
//			timeTact = itr->timeEnd - itr->timeStart;
//			_tcscpy(pPrmClassifyEnd->strTactTime, timeTact.Format(_T("%H:%M:%S")));
//			pPrmClassifyEnd->nStageNo = itr->nStageNo;	//2019.07.10 nStageNo
//
//			nRet = Seq_TactTimeData(pPrmClassifyEnd->strPanelID, TACT_INSP, TACT_END);
//
//			theApp.m_vPanelProcInfo.erase(itr);
//
//			nRet = CmdEditSend(SEND_CLASSIFY_END, 0, sizeof(PARAM_CLASSIFY_END), VS_UI_TASK, (byte *)pPrmClassifyEnd, CMD_TYPE_RES);
//			m_fnPrintLog(FALSE, _T("CASE %d : Seq >> UI : Classify End"), nStepNo);
//
//			nRet = Seq_TactTimeData(pPrmClassifyEnd->strPanelID, TACT_TOTAL, TACT_END);
//
//			break;
//
//			// 			case 2:
//			// 				// UI Task - Classify End
//			// 				memcpy(pSendParam, itr->strVirtualID, sizeof(pPrmClassifyEnd->strVirtualID));					pSendParam += sizeof(pPrmClassifyEnd->strVirtualID);
//			// 				memcpy(pSendParam, itr->strPanelID, sizeof(pPrmClassifyEnd->strPanelID));						pSendParam += sizeof(pPrmClassifyEnd->strPanelID);
//			// 				theApp.m_vPanelProcInfo.erase(itr);
//			// 				nRet = CmdEditSend(REQ_CLASSIFY_END, 0, pSendParam - bSendParam, VS_MAIN_PC_TASK, (byte *)bSendParam, CMD_TYPE_RES);
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Seq >> UI : Classify End"), nStepNo);
//			// 
//			// 				break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2130_ClassifyEnd Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	SAFE_DELETE(pPrmClassifyEnd);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2130_ClassifyEnd Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//int	WorkManager::Seq_GUI_ClassifyEnd(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	BOOL bFindProcInfo = FALSE;
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//	PARAM_CLASSIFY_END* pPrmClassifyEnd = new PARAM_CLASSIFY_END;
//	memcpy((byte *)pPrmClassifyEnd, pReceiveParam, sizeof(PARAM_CLASSIFY_END));		pReceiveParam += sizeof(PARAM_CLASSIFY_END);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// Send //////////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte bSendParam[1000];
//	byte* pSendParam = bSendParam;
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	CTimeSpan timeTact;
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2131_GUI_ClassifyEnd StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//
//		case 1:
//			// UI Task - Classify End
//			memcpy(pSendParam, pPrmClassifyEnd->strVirtualID, sizeof(pPrmClassifyEnd->strVirtualID));
//			pSendParam += sizeof(pPrmClassifyEnd->strVirtualID);
//
//			memcpy(pSendParam, pPrmClassifyEnd->strPanelID, sizeof(pPrmClassifyEnd->strPanelID));
//			pSendParam += sizeof(pPrmClassifyEnd->strPanelID);
//
//			// 추가 2019.04.12 오병종
//			memcpy(pSendParam, pPrmClassifyEnd->strPanelJudge, sizeof(pPrmClassifyEnd->strPanelJudge));
//			pSendParam += sizeof(pPrmClassifyEnd->strPanelJudge);
//
//			nRet = CmdEditSend(REQ_CLASSIFY_END, 0, pSendParam - bSendParam, VS_MAIN_PC_TASK, (byte *)bSendParam, CMD_TYPE_RES);
//			m_fnPrintLog(FALSE, _T("CASE %d : Seq >> UI : Classify End"), nStepNo);
//
//			break;
//
//		case 2:
//
//			m_fnPrintLog(FALSE, _T("CASE %d : Repeat Alarm Function Start (StageNo : %d, JudgeCode : %s)"), nStepNo, pPrmClassifyEnd->nStageNo, pPrmClassifyEnd->strJudgeCode1);
//
//			if (pPrmClassifyEnd->nStageNo >= 1 && pPrmClassifyEnd->nStageNo <= 6)
//			{
//				ST_REPEAT_ALARM_INFO* info = theApp.m_fnGetRepeatAlarmInfo(pPrmClassifyEnd->strJudgeCode1);
//				if (info)
//				{
//					if (info->bUse)
//					{
//						// 자신(=카운트할 알람 정보)을 뺀 나머지 알람 정보의 발생횟수 초기화
//						for (int i = 0; i < (int)theApp.m_vRepeatAlarmInfo.size(); i++)
//						{
//							if (theApp.m_vRepeatAlarmInfo[i] != info)
//								theApp.m_vRepeatAlarmInfo[i]->nCurrentCount[pPrmClassifyEnd->nStageNo - 1] = 0;
//						}
//
//						// 카운트할 알람 정보가 있으면 카운팅
//						info->nCurrentCount[pPrmClassifyEnd->nStageNo - 1]++;
//
//						// 알람 발생 조건 체크
//						if (info->nCurrentCount[pPrmClassifyEnd->nStageNo - 1] >= info->nRepeatCount)
//						{
//							// 알람 발생 후 발생 횟수 초기화
//							int nAlarmCode = eALARM_AD_REPEAT_OCCUR + pPrmClassifyEnd->nStageNo;
//							theApp.m_SequenceTask->VS_Send_Alarm_To_MainPC(nAlarmCode, eALARMTYPE_HEAVY);
//							info->nCurrentCount[pPrmClassifyEnd->nStageNo - 1] = 0;
//							m_fnPrintLog(FALSE, _T("CASE %d : Occur Repeat Alarm (Current Count : %d, JudgeCode : %s)"), nStepNo, info->nCurrentCount[pPrmClassifyEnd->nStageNo - 1], pPrmClassifyEnd->strJudgeCode1);
//						}
//					}
//				}
//				else
//				{
//					// 카운할 알람 정보가 없으면(=Repeat 조건이 깨졌으니까)
//					// 모든 알람 정보 발생 횟수 초기화
//					for (int i = 0; i < (int)theApp.m_vRepeatAlarmInfo.size(); i++)
//					{
//						theApp.m_vRepeatAlarmInfo[i]->nCurrentCount[pPrmClassifyEnd->nStageNo - 1] = 0;
//					}
//				}
//			}
//
//			m_fnPrintLog(FALSE, _T("CASE %d : Repeat Alarm Function End (StageNo : %d, JudgeCode : %s)"), nStepNo, pPrmClassifyEnd->nStageNo, pPrmClassifyEnd->strJudgeCode1);
//
//			break;
//
//		case 3:
//
//			m_fnPrintLog(FALSE, _T("CASE %d : Time Repeat Alarm Function Start (Cell id : %s, JudgeCode : %s)"), nStepNo, pPrmClassifyEnd->strPanelID, pPrmClassifyEnd->strJudgeCode1);
//
//			if (pPrmClassifyEnd->nStageNo >= 1 && pPrmClassifyEnd->nStageNo <= 6)
//			{
//				ST_TIME_REPEAT_ALARM_INFO* info = theApp.m_fnGetTimeRepeatAlarmInfo(pPrmClassifyEnd->strJudgeCode1);
//
//				if (info)
//				{
//					if (info->bUse)
//					{
//						// N분 지난 디펙 삭제
//						int cnt = 0;
//						for (int i = 0; i < (int)info->vCells.size(); i++)
//						{
//							// 클래스 앤드 시간과 현재 시간의 차이가 N분보다 크면 삭제
//							CTimeSpan divTime = CTime::GetCurrentTime() - info->vCells[i].tClassEnd;
//							if (divTime > info->tsAlarmTime)
//								info->vCells.erase(info->vCells.begin() + i);
//						}
//
//						// 벡터에 클래스 앤드 정보 추가 (셀아이디, 시간)
//						ST_TIME_REPEAT_CELL_INFO cell;
//						cell.strCellId = pPrmClassifyEnd->strPanelID;
//						cell.tClassEnd = CTime::GetCurrentTime();
//						info->vCells.push_back(cell);
//
//						// 알람 조건 비교하여 알람 처리
//						if ((int)info->vCells.size() >= info->nAlarmCount)
//						{
//							int nAlarmCode = eALARM_AD_REPEAT_OCCUR + pPrmClassifyEnd->nStageNo;
//							theApp.m_SequenceTask->VS_Send_Alarm_To_MainPC(nAlarmCode, eALARMTYPE_HEAVY);
//							info->vCells.clear();
//							m_fnPrintLog(FALSE, _T("CASE %d : Occur Time Repeat Alarm (Cell id : %s, JudgeCode : %s)"), nStepNo, pPrmClassifyEnd->strPanelID, pPrmClassifyEnd->strJudgeCode1);
//						}
//					}
//				}
//			}
//
//			m_fnPrintLog(FALSE, _T("CASE %d : Time Repeat Alarm Function End (Cell id : %s, JudgeCode : %s)"), nStepNo, pPrmClassifyEnd->strPanelID, pPrmClassifyEnd->strJudgeCode1);
//
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2131_GUI_ClassifyEnd Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	SAFE_DELETE(pPrmClassifyEnd);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2131_GUI_ClassifyEnd Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

// Light Info 정적 배열로 변경
// void WorkManager::m_fnLightInfoToByte(STRU_LIGHT_INFO stLightInfo, byte* tempParam)
// {
// 	*(STRU_LIGHT_INFO *)tempParam = stLightInfo; 
// 	tempParam += sizeof(STRU_LIGHT_INFO);
// 
// 	int nLightControllerCount = stLightInfo.nControllerCount;
// 
// 	for (int nLightCnt=0; nLightCnt<nLightControllerCount; nLightCnt++)
// 	{
// 		*(STRU_SERIAL_INFO *)tempParam = stLightInfo.stLight[nLightCnt]; 
// 		tempParam += sizeof(STRU_SERIAL_INFO); 
// 
// 		int nChannelCnt = stLightInfo.stLight[nLightCnt].nChCnt;
// 
// 		for (int nChCnt = 0; nChCnt < nChannelCnt; nChCnt ++)
// 		{				
// 			*(int *)tempParam = stLightInfo.stLight[nLightCnt].nLightVal[nChCnt];	tempParam += sizeof(int);	
// 		}
// 	}
// }

// Send Message
int WorkManager::VS_Send_Log_To_UI(TCHAR* cBuffer, int len)
{
	byte* pParam = new byte[len + sizeof(int)];
	byte* pSendParam = pParam;
	int			nRet = APP_OK;

	*(int *)pSendParam = theApp.m_Config.GetPCNum();	pSendParam += sizeof(int);
	memcpy(pSendParam, cBuffer, len);					pSendParam += len;

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_UI_LOG, 0, (ULONG)(pSendParam - pParam), VS_UI_TASK, pParam, CMD_TYPE_NORES);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			m_fnPrintLog(FALSE, _T("SEQLOG -- VS_Send_Log_To_UI Error Occured. RetVal=%d"), nRet);
		}

	SAFE_DELETE_ARR(pParam);

	return nRet;
}

int WorkManager::VS_Send_Notify_Init_To_UI()
{
	byte*	pParam = new byte[sizeof(int)];
	byte*	pSendParam = pParam;
	int		nRet = APP_OK;

	*(int *)pSendParam = theApp.m_Config.GetPCNum();	pSendParam += sizeof(int);

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_UI_NOTIFY_INIT, 0, (ULONG)(pSendParam - pParam), VS_UI_TASK, pParam, CMD_TYPE_RES); //CMD_TYPE_NORES -> CMD_TYPE_RES
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			m_fnPrintLog(FALSE, _T("SEQLOG -- VS_Send_Notify_Init_To_UI Error Occured. RetVal=%d"), nRet);
		}

	SAFE_DELETE_ARR(pParam);

	return nRet;
}

int WorkManager::VS_Send_Alarm_To_MainPC(int nAlarmID, int nAlarmType)
{
	byte*	pParam = new byte[1000];
	byte*	pSendParam = pParam;
	int		nRet = APP_OK;

	*(int *)pSendParam = nAlarmID;		pSendParam += sizeof(int);
	*(int *)pSendParam = nAlarmType;	pSendParam += sizeof(int);

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_ALARM_OCCURRED, 0, (ULONG)(pSendParam - pParam), VS_MAIN_PC_TASK, pParam, CMD_TYPE_RES);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			m_fnPrintLog(FALSE, _T("SEQLOG -- VS_Send_Alarm_To_MainPC Error Occured. RetVal=%d"), nRet);
		}

	SAFE_DELETE_ARR(pParam);

	return nRet;
}

//void WorkManager::m_fnCreateProcInfo(TCHAR* pStrPanelID, TCHAR* pStrVirtualID, TCHAR* pStrModelID, int nStageNo, BOOL bIsManual)
//{
//	ST_PANEL_PROC_INFO stPanelProcInfo;
//	stPanelProcInfo.bIsManual = bIsManual;
//	memcpy(stPanelProcInfo.strPanelID, pStrPanelID, sizeof(stPanelProcInfo.strPanelID));
//	memcpy(stPanelProcInfo.strVirtualID, pStrVirtualID, sizeof(stPanelProcInfo.strVirtualID));
//	memcpy(stPanelProcInfo.strModelID, pStrModelID, sizeof(stPanelProcInfo.strModelID));
//	stPanelProcInfo.timeStart = CTime::GetCurrentTime();
//	stPanelProcInfo.nStageNo = nStageNo;
//	// 중복 Panel ID 제거
//	for (std::vector<ST_PANEL_PROC_INFO>::iterator iter = theApp.m_vPanelProcInfo.begin(); iter != theApp.m_vPanelProcInfo.end(); )
//	{
//		if (_tcscmp((*iter).strPanelID, stPanelProcInfo.strPanelID) == 0)
//			iter = theApp.m_vPanelProcInfo.erase(iter);
//		else
//			++iter;
//	}
//	if (theApp.m_vPanelProcInfo.size() + 1 >= theApp.m_vPanelProcInfo.capacity())
//		theApp.m_vPanelProcInfo.erase(theApp.m_vPanelProcInfo.begin(), theApp.m_vPanelProcInfo.end() - 10);	// 임시 Test - 100개 배열이 꽉 차면 10개만 남기고 삭제
//	theApp.m_vPanelProcInfo.push_back(stPanelProcInfo);
//}

// -1000 + 2000 - 1500 + 1000 - 750 + 500 - 400 + 300 - 200 + 100 - 100 + 50 = 0
//int	WorkManager::Seq_MoveMotion_MortorX_ForTest(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	//USHORT	usSet;
//	//INT		nVal;
//	//byte*	tempParam = pParam;
//	CStringA	strMotorType;
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq_MoveMotion_MortorX_ForTest StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//			//Check Mortor Status
//		case 1:
//		{
//			strMotorType = "MOTOR_X";
//
//			nRet = m_fnSendMsgGetMotionStatus(strMotorType);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgGetMotionStatus Error = %d"), nRet);
//				throw nRet;
//			}
//
//		}
//
//		break;
//
//		//Move Motor -1000
//		case 2:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -1000);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//		break;
//
//		//Move Motor +2000
//		case 3:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 2000);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//		break;
//
//		//Move Motor -1500
//		case 4:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -1500);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor +1000
//		case 5:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 1000);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor -750
//		case 6:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -750);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor +500
//		case 7:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 500);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor -400
//		case 8:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -400);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor +300
//		case 9:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 300);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//		break;
//
//		//Move Motor -200
//		case 10:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -200);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor 100
//		case 11:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 100);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor -100
//		case 12:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, -100);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		//Move Motor 50
//		case 13:
//		{
//			nRet = m_fnSendMsgMoveMotion(strMotorType, 50);
//
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("Seq_MoveMotion_MortorX_ForTest: m_fnSendMsgMoveMotion Error = %d"), nRet);
//				throw nRet;
//			}
//		}
//
//		break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq_MoveMotion_MortorX_ForTest Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//
//				// EQP BIT ALL OFF
//
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq_MoveMotion_MortorX_ForTest Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


//int	WorkManager::m_fnSendMsgMoveMotion(CStringA strMotionType, int nRelativeMoveVal)
//{
//
//	m_fnPrintLog(FALSE, _T("m_fnSendMsgMoveMotion. %s Move %d"), strMotionType, nRelativeMoveVal);
//
//	byte	bParam[100] = { 0, };
//	byte*	bpTempParam = bParam;
//	int		nRet = APP_OK;
//
//	strcpy((char*)bpTempParam, strMotionType);
//	bpTempParam += REQ_MOVE_MOTION_PARAM1_SIZE;
//	*(int*)bpTempParam = nRelativeMoveVal;
//	bpTempParam += sizeof(int);
//
//	nRet = CmdEditSend(REQ_MOVE_MOTION, 0, (ULONG)(bpTempParam - bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES, 30000);
//
//	if (nRet != APP_OK)
//	{
//		m_fnPrintLog(FALSE, _T("m_fnSendMsgMoveMotion: REQ_MOVE_MOTION command send Error = %d"), nRet);
//		throw nRet;
//	}
//	m_fnPrintLog(FALSE, _T("m_fnSendMsgMoveMotion. MotorX Move %d. OK."), nRelativeMoveVal);
//	return APP_OK;
//}
//
//int	WorkManager::m_fnSendMsgGetMotionStatus(CStringA strMotionType)
//{
//	m_fnPrintLog(FALSE, _T("m_fnSendMsgGetMotionStatus. %s."), strMotionType);
//
//	byte	bParam[100] = { 0, };
//	byte*	bpTempParam = bParam;
//	int		nRet = APP_OK;
//
//	strcpy((char*)bpTempParam, strMotionType);
//
//	bpTempParam += REQ_GET_MOTION_STATUS_PARAM1_SIZE;
//
//
//	nRet = CmdEditSend(REQ_GET_MOTION_STATUS, 0, (ULONG)(bpTempParam - bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES);
//
//	if (nRet != APP_OK)
//	{
//		m_fnPrintLog(FALSE, _T("m_fnSendMsgGetMotionStatus: REQ_GET_MOTION_STATUS command send Error = %d"), nRet);
//		return nRet;
//	}
//	CStringA strReturnVal = (char*)bParam;
//	if (strReturnVal != "STOP")
//	{
//		m_fnPrintLog(FALSE, _T("m_fnSendMsgGetMotionStatus: REQ_GET_MOTION_STATUS Error. Return Val = %s"), (LPCSTR)strReturnVal);
//		return APP_NG;
//	}
//
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq_MoveMotion_MortorX_ForTest. Motor Status OK."));
//	return APP_OK;
//}

//int	WorkManager::Seq_AlignCamera(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));	pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nAlignPatternNum = 0;
//	int nLightIndex = 0;
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_ALIGN_START*	prmAlignStart = new PARAM_ALIGN_START;
//	PARAM_MOVE_MOTION*	prmMoveMotion = new PARAM_MOVE_MOTION;
//	int					nVacuumType = 0;
//	int					nPGState = 0;
//	TCHAR				strCurStepName[50] = { 0, };
//	int					nPatternIndex = 0;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		nStepNo++;
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2150_Seq_AlignCamera StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		switch (nStepNo)
//		{
//		case 1:
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_GET_PATTERN_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 2:
//			// 얼라인 하는 패턴 점등
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nAlignPatternNum);
//			nRet = CmdEditSend(SEND_CHANGE_PATTERN, 0, sizeof(int), VS_PG_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Pattern ON (%02d_%s)"), nStepNo, nAlignPatternNum + 1, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Pattern ON (%02d_%s) Error !!!"), nStepNo, nAlignPatternNum + 1, strCurStepName);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_PG_ON_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			Sleep(200);		// 최초 WHITE Pattern 점등 시 안정화 시간 필요
//			//Sleep(1000);			// WHITE 점등 후 바로 끌 경우 PG Controller 무언정지 - PG Protocol 검증 후 지울 것
//			break;
//
//		case 3:
//			// Light Task - Change Light Condition
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nAlignPatternNum, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Light Control"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Light Control Error !!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_LIGHT_ON_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			// Camera Task - Change Camera Condition - Align Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nAlignPatternNum, 0);
//			stCurCamCond.nSeqMode = 0;
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Align Camera Conditions Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_CAMERA_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 5:
//			if (theApp.m_Config.IsUseVacuum(nAlignPatternNum))
//				nVacuumType = 1;
//			else
//				nVacuumType = 2;
//
//			//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d) Error !!!"), nStepNo, nVacuumType);
//				throw nRet;
//			}
//			break;
//
//		case 6:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_EXPOSE_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//			// PG Turn Off
//		case 7:
//			nPGState = 0;
//			nRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_PG_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//			// Light Turn Off
//		case 8:
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_LIGHT_OFF_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nAlignPatternNum, 0);
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = nAlignPatternNum;
//			// Align Sequence 일 경우 P/S Mode 사용 안함
//			//stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			stWaitGrabEndParam.nSeqMode = 0;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_GRAB_END_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 10:
//			// Alg Task - Inspection (NRS)
//			prmAlignStart->nStageNo = nStageNo;
//			memcpy((TCHAR *)prmAlignStart->strPanelID, strPanelID, sizeof(prmAlignStart->strPanelID));
//			memcpy((TCHAR *)prmAlignStart->strVirtualID, strVirtualID, sizeof(prmAlignStart->strVirtualID));
//			prmAlignStart->bUseCamera[0] = TRUE;						// Camera Num - 0 고정
//			prmAlignStart->nImageNum = nAlignPatternNum;
//			prmAlignStart->nSeqMode[0] = theApp.m_Config.GetCamPSMode(nAlignPatternNum, 0);
//			prmAlignStart->bIsManual = bIsManual;		// 메뉴얼 플래그 추가
//			nRet = CmdEditSend(SEND_START_ALIGN, 0, (ULONG)sizeof(PARAM_ALIGN_START), VS_ALGORITHM_TASK, (byte *)prmAlignStart, CMD_TYPE_RES, 10000);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Align - Image Count : %d"), nStepNo, nAlignPatternNum);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Align - Image Count : %d Error !!!"), nStepNo, nAlignPatternNum);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_ALGORITHME_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			_tcscpy(prmMoveMotion->strMotor, _T("MOTOR_T"));
//			prmMoveMotion->nMovePoint = (int)(prmAlignStart->dAdjTheta[0] * 1000 * -1);		// Theta 역방향 회전
//			nRet = CmdEditSend(REQ_MOVE_MOTION, 0, (ULONG)sizeof(PARAM_MOVE_MOTION), VS_MAIN_PC_TASK, (byte *)prmMoveMotion, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Move Motion (T : %d)"), nStepNo, prmMoveMotion->nMovePoint);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Move Motion (T : %d) Error !!!"), nStepNo, prmMoveMotion->nMovePoint);
//				VS_Send_Alarm_To_MainPC(eALARM_ALIGN_MOTION_ERROR, eALARMTYPE_HEAVY);
//				throw nRet;
//			}
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2150_Seq_AlignCamera Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;		// Exception 내부 Return 값
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	SAFE_DELETE(prmAlignStart);
//	SAFE_DELETE(prmMoveMotion);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2150_Seq_AlignCamera Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}
int	WorkManager::Seq_GrabSeqReset(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte*	pReceiveParam = pParam;

	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;


	CString		strCellID;
	CString		strVirtualID;


	TCHAR		cTemp[500] = { 0, };
	CStringA	strSendMsg;
	CStringA	strClassEnd;

	memcpy(cTemp, pReceiveParam, 100);
	pReceiveParam += 100;
	strVirtualID = cTemp;

	memcpy(cTemp, pReceiveParam, 100);
	pReceiveParam += 100;
	strCellID = cTemp;

	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;	
	PARAM_INSPECTOR_RESET*	Param_Reset = new PARAM_INSPECTOR_RESET;
	do
	{

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
		}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2187_Seq_GrabSeqReset Sequence Start StepNo=%d, RetVal=%d"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			memcpy(cTemp, pReceiveParam, 100);
			pReceiveParam += 100;
			strVirtualID = cTemp;

			memcpy(cTemp, pReceiveParam, 100);
			pReceiveParam += 100;
			strCellID = cTemp;
			break;

		case 2: 	
			_tcscpy(Param_Reset->strVirtualID, (LPCTSTR)strVirtualID);
			_tcscpy(Param_Reset->strPanelID, (LPCTSTR)strCellID);
		case 3:
			nRet = CmdEditSend(SEND_UI_INSP_RESET, 0, sizeof(PARAM_INSPECTOR_RESET), VS_UI_TASK, (byte*)Param_Reset, CMD_TYPE_RES, 6000);
			delete[] Param_Reset;

		default:
			isRunSequence = false;
			break;
		}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2187_Seq_GrabSeqReset Sequence End StepNo=%d, RetVal=%d"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

int	WorkManager::Seq_AsyncSequence(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte*	pReceiveParam = pParam;
	int		nAsyncCount = *(int *)pReceiveParam;		pReceiveParam += sizeof(int);
	int		nPcNum = *(int *)pReceiveParam;		pReceiveParam += sizeof(int);			// 데이터를 보낸 PC Num를 받아옴
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	do
	{
		// 임시 주석처리 - JSO
// 		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
// 			return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
// 		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
// 		{
// 			return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
// 		}
// 
// 		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2188_Seq_AsyncSequence StepNo=%d, RetVal=%d, RecievePcNum=%d, nAsyncCount=%d"), nStepNo, nRet, nPcNum, nAsyncCount);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			if (nAsyncCount < MAX_AUTO_SEQ_ASNC_POINT)
				SetEvent(m_hAsycAutoSeq[nPcNum][nAsyncCount]);
			else
			{
				// 모든 Event Clear
				for (int i = 0; i < MAX_PC_COUNT; i++)
				{
					for (int j = 0; j < MAX_AUTO_SEQ_ASNC_POINT; j++)
						SetEvent(m_hAsycAutoSeq[i][j]);
				}

			}
			break;

		default:
			isRunSequence = false;
			break;
		}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2188_Seq_AsyncSequence Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


/// 다른 Sequence Task 와 순서 동기화 - 17.07.05 by CWH
/// 현재 PC 최대 2개로 가정하고 다른 PC Sequence Task 하나와 동기화. PC 개수 늘어날 경우 추가 작업 필요
//  최대 PC 4개에 따른 시컨스 동기화 기능 수정			181010 YSS
//  다른 PC의 Seq Task 연결 상태 여부를 확인하여 연결되었을 경우 자신을 제외한 다른 Seq Task 에 SetEvent 설정 
//  자신을 제외한 다른 Seq Task의 SetEvent 여부를 확인하여 최대 (dwMilliseconds * pc 개수) 만큼 대기 후 루틴 빠져나옴(Time-Out).
bool WorkManager::WaitForOtherSequence(HANDLE(*pHandle)[MAX_AUTO_SEQ_ASNC_POINT], int& nAsyncCount, DWORD dwMilliseconds)
{
	int nRet = APP_OK;
	DWORD dwRet = 0;
	bool bRet = false;

	int nPcCnt = theApp.m_Config.GetPCCnt();
	int nPcNum = theApp.m_Config.GetPCNum() - 1;

	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;
	*(int*)bpTempParam = nAsyncCount;		bpTempParam += sizeof(int);
	*(int*)bpTempParam = nPcNum;			bpTempParam += sizeof(int);

	CCPUTimer tact;
	tact.Start();
	m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence START."));

	EXCEPTION_TRY

		for (int nTaskNo = 0; nTaskNo < nPcCnt; nTaskNo++)
		{
			// 자신을 제외한 다른 PC의 Seq Task로 Send
			if (nTaskNo != theApp.m_Config.GetPCNum() - 1)
			{
				// 다른 Seq Task 연결 여부 확인 후 데이터 Send 처리
				nRet = CmdEditSend(SEND_SEQ_ALIVE, 0, 0, 21 + ((nTaskNo + 1) * 100), 0, CMD_TYPE_RES, 3000);

				if (nRet == APP_OK)		// Seq Task 연결 되었을 경우
				{
					//nRet = CmdEditSend(SEND_ASYNC_SEQUENCE, 0, sizeof(int), 21 + ((nTaskNo + 1) * 100), (byte *)&nAsyncCount, CMD_TYPE_RES);
					nRet = CmdEditSend(SEND_ASYNC_SEQUENCE, 0, sizeof(bParam), 21 + ((nTaskNo + 1) * 100), bParam, CMD_TYPE_RES);

					if (nRet == APP_OK)
						m_fnPrintLog(FALSE, _T("Send Async Sequence. Task : %d, AsyncNo : %d"), 21 + ((nTaskNo + 1) * 100), nAsyncCount);
					else
						m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence Abnormal END. %d Task, RetVal=%d, Wait-Time:%.2f"), 21 + ((nTaskNo + 1) * 100), nRet, tact.Stop(false) / 1000.);
				}
				else
					m_fnPrintLog(FALSE, _T("SEQLOG --  %d Seq Task is abnormal end. RetVal = %d"), 21 + ((nTaskNo + 1) * 100), nRet);
			}
		}


	// 자신을 제외한 다른 PC의 Seq Task 의 Event Set 여부를 확인 
	for (int nTaskNo = 0; nTaskNo < nPcCnt; nTaskNo++)
	{
		if (nTaskNo != theApp.m_Config.GetPCNum() - 1)
		{
			dwRet = WaitForSingleObject(pHandle[nTaskNo][nAsyncCount], dwMilliseconds * nPcCnt);

			if (dwRet == WAIT_FAILED)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence Failed Occured. Task : %d, RetVal=%d"), 21 + ((nTaskNo + 1) * 100), (int)GetLastError());
			}
			else if (dwRet == WAIT_ABANDONED)	/// 1회만 Retry
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence Retry...Task : %d"), 21 + ((nTaskNo + 1) * 100));
				ResetEvent(pHandle[nTaskNo][nAsyncCount]);
				dwRet = WaitForSingleObject(pHandle[nTaskNo][nAsyncCount], dwMilliseconds * nPcCnt);
				if (dwRet != WAIT_OBJECT_0)
				{
					m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence Abandoned Occured."));
				}
			}
			else if (dwRet == WAIT_TIMEOUT)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence Time-Out Occured. Task : %d"), 21 + ((nTaskNo + 1) * 100));
			}

			ResetEvent(pHandle[nTaskNo][nAsyncCount]);
		}
	}

	EXCEPTION_CATCH

		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForOtherSequence END. nAsyncCount = %d, Wait-Time:%.2f"), nAsyncCount, tact.Stop(false) / 1000.);

	nAsyncCount++;		// AsyncCount 자동 증가
	bRet = APP_OK;		// 동기화 안되었다고 검사 시컨스 중단되면 안되서 우선 강제 APP_OK 처리 함.
	return bRet;
}

//int	WorkManager::Seq_RcvCheckDustEnd(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte*	pReceiveParam = pParam;
//	m_bNeedRetry = *(bool *)pReceiveParam;		pReceiveParam += sizeof(bool);		// Retry 필요 여부
//	m_bIsNormalDust = *(bool *)pReceiveParam;		pReceiveParam += sizeof(bool);		// Dust 결과를 멤버 변수에 설정 - Auto_AVI Sequence 에서 분기
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	do
//	{
//		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//			return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//		{
//			return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//		}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2190_Seq_RcvCheckDustEnd StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			SetEvent(m_hDustEnd);
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2190_Seq_RcvCheckDustEnd Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}


/// Alg. Task - Dust Image Check 대기 구문 작성
/// Dust Image 이상일 경우 1회만 Retry - 필요 시 반복횟수 지정 작업 해야함
//bool WorkManager::WaitForCheckDustAtAlgTask(DWORD dwMilliseconds)
//{
//	DWORD dwRet = 0;
//	bool bRet = false;
//	CCPUTimer tact;
//	tact.Start();
//
//	dwRet = WaitForSingleObject(m_hDustEnd, dwMilliseconds);
//
//	//////////////////////////////// 예외 처리
//	if (dwRet == WAIT_FAILED)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask Failed Occured. RetVal=%d"), (int)GetLastError());
//	}
//	else if (dwRet == WAIT_ABANDONED)	/// 1회만 Retry
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask Retry..."));
//		ResetEvent(m_hDustEnd);
//		dwRet = WaitForSingleObject(m_hDustEnd, dwMilliseconds);
//		if (dwRet != WAIT_OBJECT_0)
//		{
//			m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask Abandoned Occured."));
//		}
//	}
//	else if (dwRet == WAIT_TIMEOUT)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask Time-Out Occured."));
//	}
//
//	if (dwRet == WAIT_OBJECT_0)
//	{
//		bRet = true;
//		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask END. Wait-Time:%.2f"), tact.Stop(false) / 1000.);
//	}
//	else
//	{
//		bRet = false;
//		m_fnPrintLog(FALSE, _T("SEQLOG -- WaitForCheckDustAtAlgTask Abnormal END. WaitRetVal=%d, Wait-Time:%.2f"), (int)dwRet, tact.Stop(false) / 1000.);
//	}
//	ResetEvent(m_hDustEnd);
//
//	return bRet;
//}


// AMT 검사 시컨스 신규 추가 180801 YSS
//int	WorkManager::Seq_ManualGrabImage_AMT(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	CString strTemp = _T("");
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// 패턴 진행 여부 체크
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//				nGrabCnt++;
//				nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//				isSeqBusy = false;
//			}
//			else
//			{
//				if (nGrabCnt >= theApp.m_Config.GetGrabCount())
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skiped !!!"), nStepNo);
//					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//				}
//			}
//			nStepNo = 59;
//			break;
//
//		case 60:
//			// AMT 설비 PG 패턴 점등 전 'PG On' 명령 추가			180822 YSS
//			nPGState = 0;
//			nRet += CmdEditSend(REQ_PG_ON, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 1;
//			break;
//
//		case 2:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//			nRet = CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s)"), nStepNo, nGrabCnt + 1, strCurStepName);
//				nStepNo = 49;		// goto case 50
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s) Error !!!"), nStepNo, nGrabCnt + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 50:
//			// Light Task - Change Light Condition
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);	// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 69;		// goto case 70
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 70:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 2;		// goto case 3
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 7;	// case 8 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 4;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 5:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//
//			// PG Task - Change PG Condition - INDEX 번째 image
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			// Dust Pattern 일 경우 PG Task 내부에서 PG Off로 처리
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//			nRet += CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s)"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s) Error !!!"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 6:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//
//		case 7:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			// Camera Task - Wait Grab End -> Save Image File to Local
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);
//			stWaitGrabEndParam.bUseFileSave = TRUE;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt + 1, theApp.m_Config.GetCurStepName(nGrabCnt), 0);
//			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp);
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 8 (Complete)				
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 10:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//			nPGState = 0;
//			nRet += CmdEditSend(REQ_PG_OFF, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);		// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//int	WorkManager::Seq_AutoInspectGrabImage_AMT(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	BOOL bIsManual = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bIsManual);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strVirtualID[50] = { 0, };		// 100 byte
//	memcpy(strVirtualID, pReceiveParam, sizeof(strVirtualID));	pReceiveParam += sizeof(strVirtualID);
//	TCHAR strModelID[50] = { 0, };		// 100 byte
//	memcpy(strModelID, pReceiveParam, sizeof(strModelID));		pReceiveParam += sizeof(strModelID);
//	BOOL bUseAsync = *(BOOL *)pReceiveParam;					pReceiveParam += sizeof(bUseAsync);	// int -> BOOL
//	int  nTestMode = *(int  *)pReceiveParam;					pReceiveParam += sizeof(nTestMode);	// int
//	TCHAR strLotID[50] = { 0, };																				// Add By KYH LOTID
//	memcpy(strLotID, pReceiveParam, sizeof(strLotID));			pReceiveParam += sizeof(strLotID);
//
//	TCHAR tMtpDrv[50] = { 0, };
//	memset(tMtpDrv, 0, sizeof(tMtpDrv));
//	CString strMtpDrv = _T("");
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CString strOrgPanelID(strPanelID);
//	if (nTestMode == 2)		// Test Mode 2 일 경우 Panel ID 를 [Panel ID]_반복횟수 로 Casting 하여 사용하므로 반복횟수 제거
//		strOrgPanelID = strOrgPanelID.Left(strOrgPanelID.ReverseFind('_'));
//
//	// 검사 사용 유무 플래그
//	BOOL bUseInspect = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Inspect"), 0, PATH_INIT_FILE);
//
//	// Align Camera 기능 사용 여부 - Grab Sequence 시작 전에 매번 읽어와야 함 (GUI 에서 실시간 갱신)
//	int nUseAlignCamera = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Adjust_Theta"), 0, PATH_INIT_FILE) > 0 ? TRUE : nTestMode;
//	// Grab Sequence - Dust 결과 대기 기능 사용 여부
//	BOOL bUseDustRetry = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Dust_Retry"), 0, PATH_INIT_FILE);
//
//	GetPrivateProfileString(_T("INSPECT"), _T("MTP_NET_ROOT"), _T("X:\\"), tMtpDrv, sizeof(tMtpDrv), PATH_INIT_FILE);
//	strMtpDrv.Format(_T("%s"), tMtpDrv);
//	ResetEvent(m_hDustEnd);
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	int nAlignPatternNum = 0;
//
//	int nAsyncCount = 0;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
//	bool bRet = false;
//
//	int nDustRetryCount = 0;		// Dust 이상 시 반복 횟수
//	const int DUST_RETRY_LIMIT = 1;	// Dust 이상 시 반복 제한 (1==1회만 Retry)
//	m_bNeedRetry = false;
//	m_bIsNormalDust = false;
//
//	static bool bFirst = true;	// 임시 작업 - UI 속도 개선 후 지울 것
//
//								// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//
//	PARAM_WAIT_GRAB_END		stWaitGrabEndParam;
//	ST_PG_DATA*				pData = NULL;
//	ST_PG_INFO				stPgInfo;
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//	CString					strOriginPGInfoPath = _T("");
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//	BOOL bFirstInspFlg = TRUE;
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	// 제어 PC GRAB.START 호출 시 각 PC Sequence 동기화
//	if (!bIsManual && bUseAsync)
//		bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 500);
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		nStepNo++;
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		switch (nStepNo)
//		{
//		case 1:
//			// AMT 설비 PG 패턴 점등 전 'PG On' 명령 추가			180822 YSS
//			nPGState = 0;
//			nRet += CmdEditSend(REQ_PG_ON, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON Error !!!"), nStepNo);
//				throw nRet;
//			}
//			// 			strOriginPGInfoPath.Format(_T("%s.ini"), strOrgPanelID);
//			// 
//			// 			if (!bIsManual)
//			// 			{
//			// 				if (theApp.m_Config.UpdatePGVoltInfo(strMtpDrv, strOriginPGInfoPath))
//			// 				{
//			// 					stPgInfo = theApp.m_Config.GetPgInfo();
//			// 					nRet = CmdEditSend(SEND_SEND_VOLTAGEINFO, 0, (ULONG)sizeof(ST_PG_INFO), VS_PG_TASK, (byte *)&stPgInfo, CMD_TYPE_RES);
//			// 					if (nRet != APP_OK)
//			// 						throw 999;
//			// 				}
//			// 				else
//			// 				{
//			// 					m_fnPrintLog(FALSE, _T("SetPgInfo fail - %s"), strOriginPGInfoPath);
//			// 					VS_Send_Alarm_To_MainPC(eALARM_READ_FAIL_MTP_RESULT, eALARMTYPE_HEAVY);
//			// 				}
//			// 			}
//			nStepNo = 69;		// goto case 70
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 70:
//			nRet = CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				throw nRet;
//			}
//			// 2017.06.21 HW Align Sequence 추가	
//			if (nUseAlignCamera == 1)
//				nStepNo = 19;	// goto case 20 - Align Camera		
//			else
//				nStepNo = 28;	// goto case 29 - Grab Sequence
//			break;
//
//			// 2017.06.21 HW Align Sequence 추가
//		case 20:
//			nRet = Seq_AlignCamera(pParam, nPrmSize);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Align Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Align Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 28;	// goto case 29 - 원래 Sequence 시작
//			break;
//
//		case 29:
//			// 예외처리
//			if (!theApp.m_Config.IsUseStep(0))
//			{
//				_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(0));
//				m_fnPrintLog(TRUE, _T("Must be Grab First Pattern (00_%s) !!!!"), strCurStepName);
//				throw 999;
//			}
//
//			// Alg. Task 에서 얼라인 하는 패턴 번호를 받아옴
//			nRet = CmdEditSend(SEND_GET_ALIGN_PATTERN_NUM, 0, (ULONG)sizeof(int), VS_ALGORITHM_TASK, (byte *)&nAlignPatternNum, CMD_TYPE_RES);
//			if (nRet != APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Get Align Pattern Num Error !!!"), nStepNo);
//				throw nRet;
//			}
//			if (!theApp.m_Config.IsUseStep(nAlignPatternNum))
//			{
//				m_fnPrintLog(TRUE, _T("Must be Grab Align Pattern (%02d_%s) !!!!"), nAlignPatternNum, theApp.m_Config.GetCurStepName(nAlignPatternNum));
//				throw 999;
//			}
//			nStepNo = 1;
//			break;
//
//			// 현재 무조건 최초 패턴 찍어야 함. 찍지 않아도 되도록 수정하려면 Alg. Task 수정 필요.
//			// 		// 패턴 진행 여부 체크
//			// 		case 30:
//			// 			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			// 			{
//			// 				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);				
//			// 				nGrabCnt++;		nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//			// 			}
//			// 			else
//			// 			{
//			// 				if (nGrabCnt < theApp.m_Config.GetGrabCount())
//			// 				{
//			// 					nStepNo = 1;	// goto case 2
//			// 				}
//			// 				else
//			// 				{
//			// 					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skip !!!"), nStepNo);
//			// 					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//			// 				}
//			// 			}
//			// 			break;
//
//		case 2:
//			// 최초 패턴 촬상 시작 전 각 PC Sequence 동기화
//			if (!bIsManual && bUseAsync && nDustRetryCount == 0)	// Dust Retry 시에는 동기화 사용 안함
//				bRet = WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 10000);
//
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180822 YSS
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//			nRet = CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s)"), nStepNo, nGrabCnt, strCurStepName);
//				nStepNo = 49;		// goto case 50
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s) Error !!!"), nStepNo, nGrabCnt, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 50:
//			// Light Task - Change Light Condition
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//				nStepNo = 2;		// goto case 3
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			if (theApp.m_Config.IsUseVacuum(nGrabCnt))
//				nVacuumType = 1;
//			else
//				nVacuumType = 2;
//
//			//nRet = CmdEditSend(REQ_SWITCH_VACUUM, 0, sizeof(nVacuumType), VS_MAIN_PC_TASK, (byte *)&nVacuumType, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d)"), nStepNo, nVacuumType);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Switch Vacuum State (Type : %d) Error !!!"), nStepNo, nVacuumType);
//				throw nRet;
//			}
//			break;
//
//		case 5:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES);
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 8;	// case 9 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Next Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 5;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 6:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180822 YSS
//
//			// PG Task - Change PG Condition - INDEX 번째 image
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			// Dust Pattern 일 경우 PG Task 내부에서 PG Off로 처리
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//			nRet += CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s)"), nStepNo, nGrabCnt + nNextStepInterval, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s) Error !!!"), nStepNo, nGrabCnt + nNextStepInterval, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 7:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//
//		case 8:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = nGrabCnt;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 10:
//			// Alg Task - Inspection (NRS)
//			if (!bIsManual)
//				prmInspStart->nInspType = 0;
//			else
//				prmInspStart->nInspType = 1;
//			prmInspStart->nStageNo = nStageNo;
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualID, sizeof(prmInspStart->strVirtualID));
//			memcpy((TCHAR *)prmInspStart->strLotID, strLotID, sizeof(prmInspStart->strLotID));
//
//			prmInspStart->bUseCamera[0] = TRUE;						// Camera Num - 0 고정
//			prmInspStart->nImageNum = nGrabCnt;
//			prmInspStart->nSeqMode[0] = theApp.m_Config.GetCamPSMode(nGrabCnt, 0);
//			prmInspStart->nRetryCnt = nDustRetryCount;
//
//			// MTP 보정 후 색좌표 송부			
//			// 			paramTemp = theApp.m_Config.GetPGParam(nGrabCnt);
//			// 			prmInspStart->dPatternCIE[0] = paramTemp.pVoltageInfo.dColor_X;
//			// 			prmInspStart->dPatternCIE[1] = paramTemp.pVoltageInfo.dColor_Y;
//			// 			prmInspStart->dPatternCIE[2] = paramTemp.pVoltageInfo.dLuminance;
//			pData = theApp.m_Config.GetVoltageInfo(theApp.m_Config.GetCurStepName(nGrabCnt));
//			if (pData != NULL)
//			{
//				prmInspStart->dPatternCIE[0] = pData->dColor_X;
//				prmInspStart->dPatternCIE[1] = pData->dColor_Y;
//				prmInspStart->dPatternCIE[2] = pData->dLuminance;
//			}
//			// 첫번째 Image 일 경우 Alg. Task 에서 Align 실행 후 응답
//			// 				if (nGrabCnt == 0)
//			// 					nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_RES);
//			// 				else
//			// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d Error !!!"), nStepNo, nGrabCnt);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 9 (Complete)			
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 12:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180822 YSS
//			nPGState = 0;
//			nRet += CmdEditSend(REQ_PG_OFF, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 13:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);			// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//			// Dust Retry 기능 - Auto Mode 에서만 동작
//		case 14:
//			if (bUseDustRetry && !bIsManual && bUseInspect)
//			{
//				bRet = WaitForCheckDustAtAlgTask(60000);						// Dust 결과 나올때까지 60초까지 대기				
//
//				if (bRet && !m_bIsNormalDust)	// Auto Mode 에서 정상적으로 결과 수신했는데 Dust Abnormal 일 경우
//				{
//					if (m_bNeedRetry && nDustRetryCount < DUST_RETRY_LIMIT)
//					{
//						m_fnPrintLog(FALSE, _T("CASE %d : Dust Error - Retry(%d) Grab Sequence !!!"), nStepNo, nDustRetryCount + 1);
//						nDustRetryCount++;
//						nGrabCnt = 0;
//						nNextStepInterval = 1;
//						//bUseAsync = FALSE;	// Dust Retry 시 동기화 기능 사용 안함
//						nStepNo = 28;	// Align Sequence 제외한 Grab Sequence 재시작
//					}
//					else
//					{
//						m_fnPrintLog(FALSE, _T("CASE %d : Dust Error !!!"), nStepNo);
//						//throw 6000;		// 중알람 띄우므로 GRAB.END.ERR 송부 안함
//					}
//				}
//				else if (!bRet)	// 결과 못받음
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Wait Dust Result Error !!!"), nStepNo);
//					throw 14000;
//				}
//				else	// 정상 종료
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Normal Dust"), nStepNo);
//				}
//			}
//			// 동기화 30초 대기
//			// PC 1, 2 모두 Dust Fail 이 났을 경우 현재는 서로 결과값을 체크하는 구문이 없으므로
//			// 끝나는 시점을 동기화하여 다음 촬상 시퀀스에 영향이 없도록 함.
//			// 필요 시 서로 결과값 받아와서 동기화 여부 결정하는 구문 작업 필요.
//			if (!bIsManual && bUseAsync)
//				WaitForOtherSequence(m_hAsycAutoSeq, nAsyncCount, 10000);
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				// 에러 발생 시 모든 동기화 포인트 클리어
//				int nAsyncClear = MAX_AUTO_SEQ_ASNC_POINT;
//				nErrRet = CmdEditSend(SEND_ASYNC_SEQUENCE, 0, sizeof(int), VS_OTHER_SEQ_TASK, (byte *)&nAsyncClear, CMD_TYPE_NORES);
//				// 모든 Auto Sequence Event Reset
//				for (int i = 0; i < MAX_PC_COUNT; i++)
//					for (int j = 0; j < MAX_AUTO_SEQ_ASNC_POINT; j++)
//						ResetEvent(m_hAsycAutoSeq[i][j]);
//				ResetEvent(m_hDustEnd);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_AVI Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//int	WorkManager::Seq_ManualGrabImage_DUMP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	BOOL nStageNo = *(int *)pReceiveParam;						pReceiveParam += sizeof(nStageNo);
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	TCHAR strFileDirectory[200] = { 0, };	// 400 byte
//	memcpy(strFileDirectory, pReceiveParam, sizeof(strFileDirectory));		pReceiveParam += sizeof(strFileDirectory);
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	CString strTemp = _T("");
//	CString strPosition = _T("FORWARD");
//
//	// Casting
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
//
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START*	prmInspStart = new PARAM_INSPECT_START;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//
//								////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage StepNo=%d, RetVal=%d"), nStepNo, nRet);
//		_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//		nStepNo++;
//		switch (nStepNo)
//		{
//		case 1:
//			// 패턴 진행 여부 체크
//			if (!theApp.m_Config.IsUseStep(nGrabCnt))
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//				nGrabCnt++;
//				nStepNo--;		// 사용하는 Step 나올때까지 Grab Count 증가				
//				isSeqBusy = false;
//			}
//			else
//			{
//				if (nGrabCnt >= theApp.m_Config.GetGrabCount())
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : All Patterns Skiped !!!"), nStepNo);
//					throw 999;		// 사용하는 Step 이 하나도 없으므로 비정상 종료
//				}
//			}
//			nStepNo = 59;
//			break;
//
//		case 60:
//			// AMT 설비 PG 패턴 점등 전 'PG On' 명령 추가			180822 YSS
//			nPGState = 0;
//			/*nRet += CmdEditSend(REQ_PG_ON, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);*/
//			nRet = 0;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG ON Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 1;
//			break;
//
//		case 2:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt);
//			nRet = 0/* CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES)*/;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s)"), nStepNo, nGrabCnt + 1, strCurStepName);
//				nStepNo = 49;		// goto case 50
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Pattern ON (%02d_%s) Error !!!"), nStepNo, nGrabCnt + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 50:
//			// Light Task - Change Light Condition
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);	// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, nLightIndex);
//				/*nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES)*/;
//				nRet = 0;
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 69;		// goto case 70
//			break;
//
//			// Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//		case 70:
//			nRet = 0;/*CmdEditSend(SEND_SET_TRIGGER_MODE, 0, sizeof(UINT), VS_CAMERA_TASK, (byte *)&nTriggerMode, CMD_TYPE_RES);*/
//		
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Change Trigger Mode Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 2;		// goto case 3
//			break;
//
//		case 3:
//			// Camera Task - Change Camera Condition - 첫 번째 Image 설정 값
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);		// 현재 다중 카메라 대응 안됨 Todo
//
//			nRet =CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set First Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 4:
//			// Camera Task - Camera Expose
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, 0, VS_CAMERA_TASK, 0, CMD_TYPE_RES,600000);
//
//			
//
//			nNextStepInterval = 1;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				throw nRet;
//			}
//			nStepNo = 50;	// goto case 51
//			break;
//
//		case 51:
//			// 최종 Grab 일 경우 다음 Light / Camera 설정 구문 Skip
//			if (nGrabCnt + nNextStepInterval >= theApp.m_Config.GetGrabCount())
//				nStepNo = 7;	// case 8 : Wait Grab End
//			break;
//
//		case 52:
//			// 현재 Grab Step 미사용 시 Grab Count 증가 후 재실행
//			if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			{
//				nNextStepInterval++;
//				m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Current Grab Count : %d - SKIP"), nStepNo, theApp.m_Config.GetGrabCount(), nGrabCnt + nNextStepInterval);
//				nStepNo = 50;
//			}
//			else
//			{
//				nStepNo = 4;	// case 6 : Set Next Grab Condition
//			}
//			break;
//
//		case 5:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//
//			// PG Task - Change PG Condition - INDEX 번째 image
//			_tcscpy(strCurStepName, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			// Dust Pattern 일 경우 PG Task 내부에서 PG Off로 처리
//			nPatternIndex = theApp.m_Config.GetCurPgIndex(nGrabCnt + nNextStepInterval);
//			/*nRet += CmdEditSend(REQ_PG_CHANGE_PATTERN, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPatternIndex, CMD_TYPE_RES);*/
//			nRet = 0;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s)"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next PG Control (%02d_%s) Error !!!"), nStepNo, nGrabCnt + nNextStepInterval + 1, strCurStepName);
//				throw nRet;
//			}
//			break;
//
//		case 6:
//			// Light Task - Change Light Condition - INDEX 번째 image
//			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start."), nStepNo);		// 조명 On 전에 로그 추가		180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//			{
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, nLightIndex);
//				/*nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + nLightIndex, (byte *)&stCurLightInfo, CMD_TYPE_RES);*/
//				nRet = 0;
//			}
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//
//		case 7:
//			// Camera Task - Change Camera Condition - INDEX 번째 image
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);		// 현재 다중 카메라 대응 안됨 Todo
//			nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(stCurCamCond), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Camera Conditions Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			// Camera Task - Wait Grab End -> Save Image File to Local
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt + nNextStepInterval, 0);
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.bUseFileSave = TRUE;
//			stWaitGrabEndParam.nGrabNum = nGrabCnt;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			strTemp.Format(_T("%s\\%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt + 1, theApp.m_Config.GetCurStepName(nGrabCnt), 0);
//			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp);
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			// Check Image INDEX - Move Case 4 (Continue) or Case 8 (Complete)				
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), theApp.m_Config.GetGrabCount(), nGrabCnt + 1);
//			nGrabCnt += nNextStepInterval;			// 다음 Grab
//
//			if (nGrabCnt < theApp.m_Config.GetGrabCount())
//				nStepNo = 3;
//			break;
//
//		case 10:
//			// AMT 설비에서는 PG 점등시 PG 테스크가 아닌 제어프로그램에서 점등 함 180801 YSS
//			nPGState = 0;
//			/*nRet += CmdEditSend(REQ_PG_OFF, 0, sizeof(int), VS_MAIN_PC_TASK, (byte *)&nPGState, CMD_TYPE_RES);*/
//			nRet = 0;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : PG Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 11:
//			// Light Task - Light Turn Off All
//			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start."), nStepNo);		// 180511 YSS
//
//			for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				/*nRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_RES);*/
//			nRet = 0;
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off End."), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		default:
//			isRunSequence = false;
//			break;
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//				nPGState = 0;
//				nErrRet = CmdEditSend(SEND_PATTERN_ON_OFF, 0, sizeof(int), VS_PG_TASK, (byte *)&nPGState, CMD_TYPE_NORES);
//				for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//					nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + nLightIndex, 0, CMD_TYPE_NORES);
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2111_ManualGrabImage Sequence END. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

//void WorkManager::SendStageMove(int nGrabCount, int nKindDirection, int nMotorSpeed)
//{
//	bool bRet = false;
//	int nRet = APP_OK;
//	CString strDirection = _T("");
//	byte bParam[1000] = { 0, };
//	byte* bpTempParam = bParam;
//
//	// 2021.12.15~ MDJ Modify Motor Speed
//	*(int*)bpTempParam = nGrabCount;		bpTempParam += sizeof(int);
//	*(int*)bpTempParam = nKindDirection;	bpTempParam += sizeof(int);
//
//	if (nKindDirection == 0)
//	{
//		strDirection.Format(_T("FORWARD"));
//		*(int*)bpTempParam = nMotorSpeed;	bpTempParam += sizeof(int);
//	}
//	else
//	{
//		strDirection.Format(_T("BACKWARD"));
//		*(int*)bpTempParam = nMotorSpeed;	bpTempParam += sizeof(int);
//	}
//
//	// 2021.12.15~ MDJ Modify Motor Speed
//	//*(int*)bpTempParam = nGrabCount;		bpTempParam += sizeof(int);
//	//*(int*)bpTempParam = nKindDirection;	bpTempParam += sizeof(int);
//
//	m_fnPrintLog(FALSE, _T("SEQLOG -- StageMove Req. Sequence : %d, Direction : %s, MotorSpeed : %d"), nGrabCount, strDirection, nMotorSpeed);
//    nRet = CmdEditSend(SEND_MOVESTAGE, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES, 10000);
//
//	if (nRet != APP_OK)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- StageMove Req Error Occur. Sequence : %d, Direction : %s"), nGrabCount, strDirection);
//	}
//	else
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- StageMove Res. Sequence : %d, Direction : %s"), nGrabCount, strDirection);
//	}
//}

//bool WorkManager::WaitForStageMove(HANDLE(*pHandle)[MAX_SEQUENCE_LCP], int& nGrabCount, int& nKindDirection, DWORD dwMilliseconds)
//{
//	int nRet = 0;
//	CString strDirection = _T("");
//
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Start"));
//
//	nRet = WaitForSingleObject(pHandle[nGrabCount][nKindDirection], dwMilliseconds);
//
//	if (nKindDirection == 0)
//	{
//		strDirection.Format(_T("FORWARD"));
//	}
//	else
//	{
//		strDirection.Format(_T("BACKWARD"));
//	}
//
//	if (nRet == WAIT_TIMEOUT)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Time Out Error : %d, Direction : %s"), nGrabCount, strDirection);
//	}
//	else if (nRet == WAIT_FAILED)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Wait Fail Error : %d, Direction : %s"), nGrabCount, strDirection);
//	}
//	else if (nRet == WAIT_ABANDONED)
//	{
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Error, Retry : %d, Direction : %s"), nGrabCount, strDirection);
//		ResetEvent(m_hSeqStageMove[nGrabCount][nKindDirection]);
//
//		nRet = WaitForSingleObject(m_hSeqStageMove[nGrabCount][nKindDirection], dwMilliseconds);
//
//		if (nRet != WAIT_OBJECT_0)
//		{
//			m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Retry Error : %d, Direction : %s"), nGrabCount, strDirection);
//		}
//	}
//
//	ResetEvent(m_hSeqStageMove[nGrabCount][nKindDirection]);
//
//	return nRet;
//
//}


//int	WorkManager::Seq_AutoInspectGrabImage_BUMP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
//{
//	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
//	byte* pReceiveParam = pParam;
//
//	byte bParam2[100] = { 0, };
//	byte* bpTempParam2 = bParam2;
//
//	TCHAR strVirtualPanelID[50] = { 0, };		// 100 byte
//	TCHAR strPanelID[50] = { 0, };		// 100 byte
//
//
//	TCHAR strDirection[50] = { 0, };		// 100 byte
//	TCHAR strPosition[50] = { 0, };		// 100 byte
//
//	//CString	strDirection, strPos;
//
//	memcpy(strVirtualPanelID, pReceiveParam, sizeof(strVirtualPanelID));		pReceiveParam += sizeof(strVirtualPanelID);
//	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
//	int nTotalLine = *(int *)pReceiveParam;						pReceiveParam += sizeof(nTotalLine);
//	memcpy(strDirection, pReceiveParam, sizeof(strDirection));		pReceiveParam += sizeof(strDirection);
//	memcpy(strPosition, pReceiveParam, sizeof(strPosition));		pReceiveParam += sizeof(strPosition);
//	//TCHAR strDirection[50] = { 0, };		// 100 b21,
//	//memcpy(strDirection, pReceiveParam, sizeof(strDirection));	pReceiveParam += sizeof(strDirection);
//	//CString strDirection = _T("F");
//
//
//
//
//	nTotalLine = 2;
//
//	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//	// 검사 사용 유무 플래그
//	//BOOL bUseInspect = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Inspect"), 0, PATH_INIT_FILE);
//
//	int nRet = APP_OK;
//	bool isRunSequence = true;
//	int nStepNo = 0;
//	static bool isSeqBusy = false;
//
//	int nGrabCnt = 0;
//	int nLightIndex = 0;
//	int nNextStepInterval = 1;
//	int nAlignPatternNum = 0;
//	int dfg = 0;
//	int nAsyncCount = 0;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
//	bool bRet = false;
//
//	// 2021.12.15~ MDJ Modify Camera Expose Time
//	double dExposeTime = 0.0;
//
//	m_bNeedRetry = false;
//	m_bIsNormalDust = false;
//
//	static bool bFirst = true;	// 임시 작업 - UI 속도 개선 후 지울 것
//
//					
//	STRU_LIGHT_INFO stCurLightInfo;
//	ST_CAMERA_COND stCurCamCond;
//	ST_LINE_INFO stLineInfo;
//
//	PARAM_WAIT_GRAB_END		stWaitGrabEndParam;
//	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	PARAM_INSPECT_START_ACI*	prmInspStart = new PARAM_INSPECT_START_ACI;
//	int						nVacuumType = 0;
//	int						nPGState = 0;
//	TCHAR					strCurStepName[50] = { 0, };
//	int						nPatternIndex = 0;
//	CString					strOriginPGInfoPath = _T("");
//	//CString					strPosition = _T("FORWARD");
//	//CString					strPosi = _T("");
//
//	CString					strDirection_Temp, strPosition_Temp;
//	CString strTemp2 = _T("");
//	CString strFileDirectory = _T("D:\\");
//	int						nPgDelay = 0;	//2019.10.23
//	int						nPgIndex = 0;
//
//	bool*					bUseInspEachStage;
//
//	int exposureCount = 0;
//
//	// 2022.02.16~ MDJ Recipe Auto Change에 따라서 Colorcode 추가
//	//stLimsPancake stLimsData;
//	//if (b_UseDB)
//	//{
//	//	MsODBC.Connect(Si);
//	//	stLimsData = MsODBC.SelectCurrentPancake(strPanCakeID);
//	//	MsODBC.Disconnect();
//	//}
//
//	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
//	//UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap
//
//	BOOL bFirstInspFlg = TRUE;
//
//	// 2021.10.05~ MDJ GrabCount, Forward[Backward] for LCP
//	int	nGrabCount = 0;
//	//int nKindDirection = 0;
//
//	stLineInfo = theApp.m_Config.GetLineInfo();
//
//
//	strDirection_Temp = strDirection;
//
//	strPosition_Temp = strPosition;;
//	do
//	{
//		EXCEPTION_TRY
//
//			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인dddd경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
//				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
//			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
//			{
//				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
//			}
//
//		isSeqBusy = true;
//
//		if (m_bSeqResetFlag && bSeqResetPossible)
//			throw 9999;
//
//		nStepNo++;
//		// Sequence In LOG
//		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_DUMP strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);
//		
//		switch (nStepNo)
//		{
//		case 1:
//		
//			if (nGrabCnt == 0)  // 처음 한번만 조명 on
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start. Pattern : %s"), nStepNo, theApp.m_Config.GetCurStepName(nGrabCnt));		// 조명 On 전에 로그 추가		180511 YSS
//
//				stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt, 1);
//
//				nRet += /*CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + 1, (byte *)&stCurLightInfo, CMD_TYPE_RES)*/0;
//
//				if (nRet == APP_OK)
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON End. Pattern : %s"), nStepNo, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//				}
//				else
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON Error !!!"), nStepNo);
//					throw nRet;
//				}
//			}
//		
//
//			break;
//
//		case 2:
//			
//			if (nGrabCnt >=1)
//			{
//				nStepNo = 9;
//				break;
//			}
//			dExposeTime = (/*theApp.m_Config.GetCamExposeVal(nGrabCnt, 0)*/stLineInfo.stLineData[nGrabCnt].dExpose / 1000);
//		
//			nRet = 0/*CmdEditSend(SEND_SET_CAMERA_EXPOSE_TIME, 0, sizeof(dExposeTime), VS_CAMERA_TASK, (byte *)&dExposeTime, CMD_TYPE_RES, 6000)*/;
//			
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Expose Time : %f"), nStepNo, dExposeTime);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Condition Error !!!"), nStepNo);
//				//throw nRet;
//			}
//			break;
//
//		case 3:
//
//			//if (nGrabCnt == 0)
//			//{
//			//	SendStageMove(nGrabCnt, 1, 0); // 백워드 위치로 이동
//			//	strPosi = _T("F");
//			//}
//			//else
//			//{
//			//	if (!nGrabCnt % 1)
//			//	{
//			//		strPosi = _T("B");
//			//		SendStageMove(nGrabCnt, 0, 0);
//			//	}
//			//}
//	
//			//stLineInfo = theApp.m_Config.GetLineInfo();
//		
//			_tcscpy((TCHAR*)stLineInfo.stLineData[0].strDirection, strDirection);
//
//			m_fnPrintLog(FALSE, _T("CASE %d : Scan Direction : %s, Scan Position : %s"), nStepNo, strDirection_Temp, strPosition_Temp);
//
//			if (stLineInfo.stLineData[nGrabCnt].nCOUNTF == 0)
//			{
//				stLineInfo.stLineData[0].nCurrentGrab = 0;
//			}
//			else
//			{
//				stLineInfo.stLineData[0].nCurrentGrab = nGrabCnt;
//			}
//			
//
//			//_tcscpy((TCHAR*)bpTempParam2, (LPCTSTR)strPosi);
//			//bpTempParam2 += sizeof(int);
//			//*(int*)bpTempParam2 = nGrabCnt;
//			//bpTempParam2 += sizeof(int);
//
//
//
//			// Camera Task - Camera Trigger
//			//nRet = CmdEditSend(SEND_CAMERA_TRIGGER, 0, sizeof(ST_LINE_INFO), VS_CAMERA_TASK, (byte *)&stLineInfo, CMD_TYPE_RES);
//			nRet = 0;
//			//nNextStepInterval = 1;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Trigger Setting Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Trigger Setting Camera Error !!!"), nStepNo);
//				//throw nRet;
//			}
//
//		
//			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, sizeof(ST_LINE_INFO), VS_CAMERA_TASK, (byte *)&stLineInfo, CMD_TYPE_RES, 60000);
//					
//			
// 			nNextStepInterval = 1;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!"), nStepNo);
//				//throw nRet;
//			}
//
//	
//			break;
//
//		case 4:
//
//			////nRet = CmdEditSend(REQ_SEND_LINE_GRAB, 0, sizeof(nGrabCnt), VS_MAIN_PC_TASK, (byte *)&nGrabCnt, CMD_TYPE_RES);
//
//			//if (nRet == APP_OK)
//			//	m_fnPrintLog(FALSE, _T("CASE %d : Send Line Grab Num(%d)"), nGrabCnt);
//			//else
//			//{
//			//	//VS_Send_Alarm_To_MainPC(eALARM_SWITCH_VACUUM_ERROR, eALARMTYPE_HEAVY);
//			//	throw nRet;
//			//}
//
//
//			//if (nGrabCnt + nNextStepInterval >= nTotalLine/*theApp.m_Config.GetGrabCount()*/)
//			//	nStepNo = 6;
//			
//			break;
//
//		case 5:
//
//			//if (!theApp.m_Config.IsUseStep(nGrabCnt + nNextStepInterval))
//			//{
//			//	nNextStepInterval++;
//			//	m_fnPrintLog(FALSE, _T("CASE %d : Next Step - Total Grab Count : %d, Next Grab Count : %d - SKIP"), nStepNo, nTotalLine/*theApp.m_Config.GetGrabCount()*/, nGrabCnt + nNextStepInterval);
//			//	nStepNo = 3;
//			//}
//			//else
//			//{
//			//	nStepNo = 5;	// case 6 : Set Next Grab Condition
//			//}
//
//			break;
//
//		
//			//nStepNo = 50;	// goto case 51
//
//		case 6:
//		
//			break;
//
//		case 7:
//			// Camera Task - Wait Grab End -> Set Shared Memory (Image)
//			/*stLineInfo = theApp.m_Config.GetLineInfo();*/
//			
//			strTemp2.Format(_T("%s%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt, theApp.m_Config.GetCurStepName(nGrabCnt), 0);
//			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp2);
//
//
//
//			stCurCamCond = theApp.m_Config.GetCameraConditions(nGrabCnt, 0);
//			stWaitGrabEndParam.bUseSMem = TRUE;
//			stWaitGrabEndParam.nGrabNum = /*m_ShareImgNum(*/nGrabCnt;
//			stWaitGrabEndParam.nSeqMode = stCurCamCond.nSeqMode;
//			stWaitGrabEndParam.nTriCountF = stLineInfo.stLineData[nGrabCnt].nCOUNTF;
//			stWaitGrabEndParam.nTriCountB = stLineInfo.stLineData[nGrabCnt].nCountB;
//			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
//			_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(nGrabCnt));
//			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES, 60000);
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
//				throw nRet;
//			}
//			break;
//
//		case 8:
//			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
//			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualPanelID, sizeof(prmInspStart->strVirtualID));
//			//memcpy((TCHAR *)prmInspStart->strLotID, strPanelID, sizeof(prmInspStart->strLotID));
//			//memcpy((TCHAR *)prmInspStart->strPos, strDirection, sizeof(prmInspStart->strPos));
//
//			prmInspStart->nImageNum = nGrabCnt;
//			prmInspStart->nShareImgNum = /*m_ShareImgNum(*/nGrabCnt;
//			prmInspStart->nGrabLine = nTotalLine;
//			memcpy((TCHAR *)prmInspStart->strDirection, strDirection, sizeof(prmInspStart->strDirection));
//			memcpy((TCHAR *)prmInspStart->strPosition, strPosition, sizeof(prmInspStart->strPosition));
//			// 검사 시작은 무조건 NoRes 로 변경
//			nRet = CmdEditSend(SEND_UI_INSP_START, 0, (ULONG)sizeof(PARAM_INSPECT_START_ACI), VS_UI_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
//
//			if (bFirstInspFlg)
//				nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
//			bFirstInspFlg = FALSE;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d Error !!!"), nStepNo, nGrabCnt);
//				throw nRet;
//			}
//			break;
//
//		case 9:
//			m_fnPrintLog(TRUE, _T("SEQLOG : Next Step - Total Grab Count : %d, Current Grab Count : %d"), nTotalLine, nGrabCnt);
//					
//
//			if (nGrabCnt < nTotalLine/*theApp.m_Config.GetGrabCount()*/)
//			{
//				nStepNo = 1;
//
//				nRet = CmdEditSend(REQ_SEND_LINE_GRAB_END, 0, sizeof(nGrabCnt), VS_MAIN_PC_TASK, (byte *)&nGrabCnt, CMD_TYPE_RES);
//
//				if (nRet == APP_OK)
//					m_fnPrintLog(FALSE, _T("CASE %d : Send Line Grab END OK. Num(%d)"), nGrabCnt);
//				else
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Send Line Grab END ERR. Num(%d)"), nGrabCnt);
//					throw nRet;
//				}
//			}
//			else
//				nStepNo = 9;
//
//			nGrabCnt++;  // 다음 Grab
//
//			break;
//
//		case 10:
//			stCurLightInfo = theApp.m_Config.GetLightInfo(nGrabCnt + nNextStepInterval, 1);
//
//			nRet += 0/*CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK + 1, (byte *)&stCurLightInfo, CMD_TYPE_RES)*/;
//
//			if (nRet == APP_OK)
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light off End. Pattern : %s"), nStepNo, theApp.m_Config.GetCurStepName(nGrabCnt + nNextStepInterval));
//			}
//			else
//			{
//				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light off Error !!!"), nStepNo);
//				throw nRet;
//			}
//
//		default:
//			isRunSequence = false;
//			break;
//	
//	
//		}
//
//		EXCEPTION_CATCH
//
//			if (nRet != APP_OK)
//			{
//				// Error Log
//				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_DUMP Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);
//
//				// Error 발생 시 PG / Light Off
//				int nErrRet = APP_OK;
//
//// 				if (theApp.m_Config.GetSiteType() == theApp.m_Config.JDI_AUTO_PH2)
//// 				{
//// 					nRet = VS_Send_Light_To_Seq_OFF(theApp.m_Config.GetPCNum());    //Light turn off
//// 				}
//// 				else
//// 				{
//					//for (nLightIndex = 0; nLightIndex < NUM_LIGHT_CONTROLLER; nLightIndex++)
//				//	{
//						//nErrRet = CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + 1, 0, CMD_TYPE_NORES);
//					//}
//				//}
//
//
//				if (nRet == APP_OK || nErrRet == APP_OK)
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH End"), nStepNo);
//				}
//				else
//				{
//					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH Error !!!"), nStepNo);
//				}
//
//				// 모든 Auto Sequence Event Reset
//// 				for (int i = 0; i < MAX_GRAB_COUNT_LCP; i++)
//// 					for (int j = 0; j < MAX_SEQUENCE_LCP; j++)
//// 						ResetEvent(m_hSeqStageMove[i][j]);
//
//				isRunSequence = false;
//				int nRetExcept = APP_OK;
//			}
//
//	} while (isRunSequence);
//	//CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + 1, 0, CMD_TYPE_NORES);
//
//// 	for (int i = 0; i < MAX_GRAB_COUNT_LCP; i++)
//// 		for (int j = 0; j < MAX_SEQUENCE_LCP; j++)
//// 			ResetEvent(m_hSeqStageMove[i][j]);
//
//	SAFE_DELETE(prmInspStart);
//
//	// Sequence Out LOG
//	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_DUMP Sequence END. Barcode = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);
//
//	isSeqBusy = false;
//
//	return nRet;
//}

int	WorkManager::Seq_AFReady(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	byte* pReceiveParam = pParam;


	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte bParam2[100] = { 0, };
	byte* bpTempParam2 = bParam2;

	byte bParam1[1000] = { 0, };
	byte* bpTempParam1 = bParam1;

	TCHAR strVirtualPanelID[50] = { 0, };		// 100 byte
	TCHAR strPanelID[50] = { 0, };		// 100 byte

	int nPos = 0;
	int nArea = 0;
	int nRepeatIndex = 0;

	int nRet = APP_OK;
	memcpy(strVirtualPanelID, pReceiveParam, sizeof(strVirtualPanelID));		pReceiveParam += sizeof(strVirtualPanelID);
	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
	nPos = *(int*)pReceiveParam;
	pReceiveParam += sizeof(nPos);
	nArea = *(int*)pReceiveParam;
	pReceiveParam += sizeof(nArea);
	nRepeatIndex = *(int*)pReceiveParam;
	pReceiveParam += sizeof(nRepeatIndex);


	CString PCPosition;

	TCHAR* AreaName;

	if (nPos == 0)
	{
		PCPosition = _T("배면1");
	}
	else if (nPos == 1)
	{
		PCPosition = _T("배면2");
	}
	else if (nPos == 2)
	{
		PCPosition = _T("상면1");
	}
	else if (nPos == 3)
	{
		PCPosition = _T("상면2");
	}
	else
	{
		// 지정되지 않은 영역
		throw 5002;
	}

	if (nArea == 0)
	{
		AreaName = (LPTSTR)(LPCTSTR)_T("Pad");
	}
	else if (nArea == 1)
	{
		AreaName = (LPTSTR)(LPCTSTR)_T("Right");
	}
	else if (nArea == 2)
	{
		AreaName = (LPTSTR)(LPCTSTR)_T("Bottom");
	}
	else if (nArea == 3)
	{
		AreaName = (LPTSTR)(LPCTSTR)_T("TOP");
	}
	else
	{
		// 지정되지 않은 영역
		throw 5001;
	}
	ST_GRABIMAGE_LINKDATA GrabLinkData;
	ST_AUTOFOCUS_AOT AutoFocusData;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인dddd경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.

			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT ERR BUSY strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			}
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT NOT BUSY strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		nStepNo++;
		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

		switch (nStepNo)
		{
		case 1:
			// 레시피 확인
			GrabLinkData = theApp.m_Config.AnalyzeInspStartPacket(nArea, nRepeatIndex);
			AutoFocusData = theApp.m_Config.GetAutoFocusData(GrabLinkData.AreaName, 0, nPos);

			*(int*)bpTempParam1 = nPos;
			bpTempParam1 += sizeof(nPos);
			*(double*)bpTempParam1 = AutoFocusData.AxisZ;
			bpTempParam1 += sizeof(AutoFocusData.AxisZ);
			_tcscpy((TCHAR*)bpTempParam1, (LPCTSTR)AreaName);
			bpTempParam1 += 100;
			break;
		case 2:
			// AF 전송
			nRet = CmdEditSend(SEND_AF_PROCESS, 0, sizeof(bParam1), VS_AF_TASK, (byte *)bParam1, CMD_TYPE_RES);
			m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT strPanelID = %s, StepNo=%d, RetVal=%d, AreaName = %s, PCNum = %d, Axis = %f"), strPanelID, nStepNo, nRet, AreaName, nPos, AutoFocusData.AxisZ);
			break;
		default:
			isRunSequence = false;
			break;
		}
		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);

				int nErrRet = APP_OK;

				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);

	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2123_GrabReay_AOT Sequence END. Barcode = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}
int	WorkManager::Seq_GrabEnd_FromMainPC(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	byte* pReceiveParam = pParam;

	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte bParam2[100] = { 0, };
	byte* bpTempParam2 = bParam2;

	TCHAR strVirtualPanelID[50] = { 0, };		// 100 byte
	TCHAR strPanelID[50] = { 0, };		// 100 byte

	int nRet = APP_OK;
	memcpy(strVirtualPanelID, pReceiveParam, sizeof(strVirtualPanelID));		pReceiveParam += sizeof(strVirtualPanelID);
	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
	int CellPos = *(int *)pReceiveParam;						pReceiveParam += sizeof(CellPos);
	int RepeatCnt = *(int *)pReceiveParam;						pReceiveParam += sizeof(RepeatCnt);

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인dddd경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		nStepNo++;
		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2124_GrabEnd_AOT strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

		switch (nStepNo)
		{
		case 1:
			// Object Set
			SetEvent(m_hSeqStageMoveDone);
			break;
		default:
			isRunSequence = false;
			break;
		}
		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2124_GrabEnd_AOT Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);

				int nErrRet = APP_OK;

				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);

	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2124_GrabEnd_AOT Sequence END. Barcode = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}
int	WorkManager::Seq_AutoInspectGrabImage_AOT_CHIPPING(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReceiveParam = pParam;

	byte bParam2[100] = { 0, };


	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;

	byte bParam3[1000] = { 0, };
	byte* bpTempParam3 = bParam3;

	byte bParam4[1000] = { 0, };
	byte* bpTempParam4 = bParam4;

	byte bParam5[1000] = { 0, };
	byte* bpTempParam5 = bParam5;

	TCHAR strVirtualPanelID[50] = { 0, };		// 100 byte
	TCHAR strPanelID[50] = { 0, };		// 100 byte
	TCHAR strAreaTemp[50] = { 0, };		// 100 byte
	memset(strAreaTemp, 0, sizeof(strAreaTemp));
	// 영상 Buf Index 계산을 위해서 방향 필요할지도?
	//TCHAR strDirection[50] = { 0, };		// 100 byte
	//TCHAR strPosition[50] = { 0, };		// 100 byte

										//CString	strDirection, strPos;

	memcpy(strVirtualPanelID, pReceiveParam, sizeof(strVirtualPanelID));		pReceiveParam += sizeof(strVirtualPanelID);
	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
	int AreaIndex = *(int *)pReceiveParam;						pReceiveParam += sizeof(AreaIndex);
	int RepeatCnt = *(int *)pReceiveParam;						pReceiveParam += sizeof(RepeatCnt);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString					strDirection, strPosition;
	bool FirstPattern = false;
	int ProcessCnt = 0;

	memcpy(strAreaTemp, strPosition, strPosition.GetLength()*2);
	TCHAR* Temp1 = (LPTSTR)(LPCTSTR)strPosition;

	int ProcessGrabCount = 0;
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	int nGrabCnt = 0;
	int nNextStepInterval = 1;
	int nAsyncCount = 5;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
	bool bRet = false;
	byte* CameraRespons;
	// 2021.12.15~ MDJ Modify Camera Expose Time
	double dExposeTime = 0.0;

	ST_LIGHT_COND_AOT stCurLightInfo;

	ST_CAM_COND_AOT stCurCamCond;

	PARAM_WAIT_GRAB_END		stWaitGrabEndParam;
	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PARAM_INSPECT_START_AOT_CHIPPING_ALM*	prmInspStart = new PARAM_INSPECT_START_AOT_CHIPPING_ALM;
	int						nVacuumType = 0;
	//int						nPGState = 0;
	//CString					strPosition = _T("FORWARD");
	//CString					strPosi = _T("");

	IMAGE_SET_AOT ImageSetTemp;

	CString strFileDirectory = _T("D:\\");
	int						nPgDelay = 0;	//2019.10.23
	int						nPgIndex = 0;

	bool*					bUseInspEachStage;
	CString strTemp2 = _T("");
	int exposureCount = 0;	
	//int ProcessCnt = 0;
	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
	//UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap

	BOOL bFirstInspFlg = TRUE;

	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	CString StartGrabTime;

	StartGrabTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d.%03ld"),
		cur_time.wYear,
		cur_time.wMonth,
		cur_time.wDay,
		cur_time.wHour,
		cur_time.wMinute,
		cur_time.wSecond,
		cur_time.wMilliseconds);

	memcpy(prmInspStart->GrabStartTime, StartGrabTime, (StartGrabTime.GetLength()*2));

	ST_GRABIMAGE_LINKDATA  CurLinkData;

	int	nGrabCount = 0;
	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인dddd경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		nStepNo++;
		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_CHIPPING strPanelID = %s, , AreaIndex = %d, RepeatCnt = %d StepNo=%d, RetVal=%d"), strPanelID,AreaIndex, RepeatCnt, nStepNo, nRet);
		CString PatternName = theApp.m_Config.GetCurPatternName(strPosition, 0);
		switch (nStepNo)
		{
		case 1:
			CurLinkData = theApp.m_Config.AnalyzeInspStartPacket(AreaIndex, RepeatCnt);
			if (CurLinkData.AreaIndex == -99)
			{
				nRet = APP_NG;
			}
			else
			{
				nRet == APP_OK;
				strPosition.Format(_T("%s"), CurLinkData.AreaName);
				ProcessCnt = CurLinkData.TrgCount;
				FirstPattern = CurLinkData.First;
				prmInspStart->AreaIndex = AreaIndex;
				prmInspStart->RepeatIdx = RepeatCnt;
				memcpy((TCHAR *)prmInspStart->strArea, CurLinkData.AreaName, sizeof(CurLinkData.AreaName));
			}

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Get GrabImage LinkData OK !!!"), nStepNo);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Get GrabImage LinkData Error !!!"), nStepNo);
				throw nRet;
			}
			break;
		case 2:
			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start. Pattern : %s"), nStepNo, PatternName);		// 조명 On 전에 로그 추가		180511 YSS

			stCurLightInfo = theApp.m_Config.GetLightInfo(strPosition, nGrabCnt, 0);

			nRet += CmdEditSend(SEND_LIGHT_SEQUENCE_IDX_INIT, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK, (byte *)&stCurLightInfo, CMD_TYPE_RES);
			//nRet += CmdEditSend(SEND_APPLY_LIGHT_PROPERTY, 0, sizeof(ST_LIGHT_COND_AOT), VS_LIGHT_TASK, (byte *)&stCurLightInfo, CMD_TYPE_RES);

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON End. Pattern : %s"), nStepNo, PatternName/*theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt + nNextStepInterval)*/);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON Error !!!"), nStepNo);
				throw nRet;
			}
			break;

		case 3:
			//stCurCamCond = /*theApp.m_Config.GetCamExposeVal(nGrabCnt, 0)*/theApp.m_Config.GetCameraConditions(strPosition, nGrabCnt,0);

			nRet = 0/*CmdEditSend(SEND_SET_CAMERA_EXPOSE_TIME, 0, sizeof(dExposeTime), VS_CAMERA_TASK, (byte *)&dExposeTime, CMD_TYPE_RES, 6000)*/;

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Expose Time : %f"), nStepNo, dExposeTime);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Condition Error !!!"), nStepNo);
				//throw nRet;
			}
			break;

		case 4:
			// Vaccum 세팅
			// 통신 추가 필요
			nRet = 0;
			//nNextStepInterval = 1;

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Vaccum OK"), nStepNo);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Vaccum Error !!!"), nStepNo);
				//throw nRet;
			}
			break;
		


		case 5:
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)strPanelID);
			bpTempParam4 += 100;
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)strVirtualPanelID);
			bpTempParam4 += 100;


			*(int*)bpTempParam4 = ProcessCnt;
			bpTempParam4 += sizeof(int);
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)Temp1);
			bpTempParam4 += 100;
			//stCurCamCond = theApp.m_Config.GetLineInfo(strPosition, 0, 0);
			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, sizeof(bParam4), VS_CAMERA_TASK, (byte *)&bParam4, CMD_TYPE_RES);

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera, ProcessCnt : %d"), nStepNo, ProcessCnt);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!, ProcessCnt : %d"), nStepNo, ProcessCnt);
				//throw nRet;
			}
			break;
		case 6:
			// 스테이지 이동 요청

			_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualPanelID);
			bpTempParam += 100;
			_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strPanelID);
			bpTempParam += 100;
			*(int*)bpTempParam = AreaIndex;
			bpTempParam += sizeof(int);
			*(int*)bpTempParam = RepeatCnt;
			bpTempParam += sizeof(int);
			ResetEvent(m_hSeqStageMoveDone);

			nRet = CmdEditSend(REQ_GRAB_START, 0, sizeof(bParam), VS_MAIN_PC_TASK, (byte *)&bParam, CMD_TYPE_NORES);
			break;

		case 7:
			// 스테이지 이동 확인			
			nRet = WaitForSingleObject(m_hSeqStageMoveDone, 10000);


			*(int*)bpTempParam5 = ProcessCnt;
			bpTempParam5 += sizeof(int);
			if (nRet == WAIT_TIMEOUT)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Time Out Error : %d, Area : %s"), nGrabCount, strPosition);
				nRet = CmdEditSend(SEND_GRAB_STOP, 0, sizeof(int), VS_CAMERA_TASK, (byte *)&bParam5, CMD_TYPE_RES, 5000);
				//throw;
			}
			else if (nRet == WAIT_FAILED)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Wait Fail Error : %d, Direction : %s"), nGrabCount, strPosition);
				nRet = CmdEditSend(SEND_GRAB_STOP, 0, sizeof(int), VS_CAMERA_TASK, (byte *)&bParam5, CMD_TYPE_RES, 5000);
				//throw;
			}
			else if (nRet == WAIT_ABANDONED)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Error, Retry : %d, Direction : %s"), nGrabCount, strPosition);
				nRet = CmdEditSend(SEND_GRAB_STOP, 0, sizeof(int), VS_CAMERA_TASK, (byte *)&bParam5, CMD_TYPE_RES, 5000);
				//ResetEvent(m_hSeqStageMoveDone);

				//nRet = WaitForSingleObject(m_hSeqStageMoveDone, 5000);

				//if (nRet != WAIT_OBJECT_0)
				//{
				//	m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Retry Error : %d, Direction : %s"), nGrabCount, strPosition);
				//}
				//throw;
			}
			else
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Stage Move OK"), nStepNo);

			//ResetEvent(m_hSeqStageMoveDone);
			break;
		case 8:
			//Sleep(300);
			//// 스테이지 이동 완료 후 그랩 종료
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Check !!!"), nStepNo);
			//_tcscpy((TCHAR*)bpTempParam3, (LPCTSTR)strVirtualPanelID);
			//bpTempParam3 += 100;
			//_tcscpy((TCHAR*)bpTempParam3, (LPCTSTR)strPanelID);
			//bpTempParam3 += 100;
			//nRet = CmdEditSend(SEND_GRAB_STOP, 0, sizeof(int), VS_CAMERA_TASK, (byte *)&nAsyncCount, CMD_TYPE_RES, 10000);
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Done !!!"), nStepNo);
			//ProcessCnt = *(int*)bpTempParam3;
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Done !!!, ProcessCnt : %d"), nStepNo, ProcessCnt);
			break;
		case 9:
			// 카메라 버퍼 -> 공유메모리
			strTemp2.Format(_T("%s%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt, theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt), 0);
			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp2);			

			stCurCamCond = theApp.m_Config.GetCameraConditions(strPosition, nGrabCnt, 0);
			stWaitGrabEndParam.bUseSMem = TRUE;
			//stWaitGrabEndParam.nGrabNum = tWaitGrabEndParam.nSeqMode = stCurCamCond.PS;
			//stWaitGrabEndParam.nTriCountF = stCurCamCond.nCountF;
			//stWaitGrabEndParam.nTriCountB = stCurCamCond.nCountB;
			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
			stWaitGrabEndParam.GrabCnt = ProcessCnt;
			//_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(strPosition, nGrabCnt));
			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES, 60000);
			
			//CameraRespons = (byte *)&stWaitGrabEndParam;
			//CameraRespons += sizeof(ProcessGrabCount);
			//ProcessGrabCount = *(int*)CameraRespons;
			//CameraRespons += sizeof(CameraRespons);
			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
				throw nRet;
			}
			break;

		case 10:
			for (int i = 0; i < theApp.m_Config.GetGrabCount(strPosition); ++i)
			{			

				ImageSetTemp.ParticleImageCount = ProcessCnt;
				ImageSetTemp.SharedMemStartIdx = i;
				memcpy((TCHAR *)ImageSetTemp.PatternName, theApp.m_Config.GetCurPatternName(strPosition, i), sizeof(ImageSetTemp.PatternName));
				prmInspStart->ImageSet[i] = ImageSetTemp;
			}
			prmInspStart->PcNo = 0;
			prmInspStart->nInspType = 0;
			prmInspStart->PatternCount = theApp.m_Config.GetGrabCount(strPosition);
			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualPanelID, sizeof(prmInspStart->strVirtualID));
			//memcpy((TCHAR *)prmInspStart->strArea, strAreaTemp, sizeof(prmInspStart->strArea));
			prmInspStart->FirstPattern = FirstPattern;
			// 검사 시작은 무조건 NoRes 로 변경
			nRet = CmdEditSend(SEND_UI_INSP_START, 0, (ULONG)sizeof(PARAM_INSPECT_START_AOT_CHIPPING_ALM), VS_UI_TASK, (byte *)prmInspStart, CMD_TYPE_RES, 60000);

			//if (bFirstInspFlg)
			//	nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
			bFirstInspFlg = FALSE;

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d Error !!!"), nStepNo, nGrabCnt);
				throw nRet;
			}
			break;
		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_CHIPPING Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);

				// Error 발생 시 PG / Light Off
				int nErrRet = APP_OK;

				if (nRet == APP_OK || nErrRet == APP_OK)
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH End"), nStepNo);
				}
				else
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH Error !!!"), nStepNo);
				}

				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);
	//CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + 1, 0, CMD_TYPE_NORES);


	SAFE_DELETE(prmInspStart);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_CHIPPING Sequence END. Barcode = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}
int	WorkManager::Seq_AutoInspectGrabImage_ALM(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReceiveParam = pParam;

	byte bParam2[100] = { 0, };


	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;

	byte bParam3[1000] = { 0, };
	byte* bpTempParam3 = bParam3;

	byte bParam4[1000] = { 0, };
	byte* bpTempParam4 = bParam4;

	TCHAR strVirtualPanelID[50] = { 0, };		// 100 byte
	TCHAR strPanelID[50] = { 0, };		// 100 byte
	TCHAR strAreaTemp[50] = { 0, };		// 100 byte
	memset(strAreaTemp, 0, sizeof(strAreaTemp));
	// 영상 Buf Index 계산을 위해서 방향 필요할지도?
	//TCHAR strDirection[50] = { 0, };		// 100 byte
	//TCHAR strPosition[50] = { 0, };		// 100 byte

	//CString	strDirection, strPos;

	memcpy(strVirtualPanelID, pReceiveParam, sizeof(strVirtualPanelID));		pReceiveParam += sizeof(strVirtualPanelID);
	memcpy(strPanelID, pReceiveParam, sizeof(strPanelID));		pReceiveParam += sizeof(strPanelID);
	int CellPos = *(int *)pReceiveParam;						pReceiveParam += sizeof(CellPos);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString					strDirection, strPosition;


	int ProcessCnt = 0;

	CString RecipePath;
	RecipePath.Format(_T("%s\\GrabData.ini"), theApp.m_Config.GetRecipeFullPath());

	strPosition.Format(_T("ALL"));
	ProcessCnt = GetPrivateProfileInt(_T("Offset"), strPosition, 45, RecipePath);

	memcpy(strAreaTemp, strPosition, strPosition.GetLength() * 2);
	TCHAR* Temp1 = (LPTSTR)(LPCTSTR)strPosition;

	int ProcessGrabCount = 0;
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	int nGrabCnt = 0;
	int nNextStepInterval = 1;
	int nAsyncCount = 5;		// Sequence 동기화 Flag Index - 함수 내부에서 자동 증가
	bool bRet = false;
	byte* CameraRespons;
	// 2021.12.15~ MDJ Modify Camera Expose Time
	double dExposeTime = 0.0;

	ST_LIGHT_COND_AOT stCurLightInfo;

	ST_CAM_COND_AOT stCurCamCond;

	PARAM_WAIT_GRAB_END		stWaitGrabEndParam;
	// Send ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	PARAM_INSPECT_START_AOT_CHIPPING_ALM*	prmInspStart = new PARAM_INSPECT_START_AOT_CHIPPING_ALM;
	int						nVacuumType = 0;
	//int						nPGState = 0;
	//CString					strPosition = _T("FORWARD");
	//CString					strPosi = _T("");

	IMAGE_SET_AOT ImageSetTemp;

	CString strFileDirectory = _T("D:\\");
	int						nPgDelay = 0;	//2019.10.23
	int						nPgIndex = 0;

	bool*					bUseInspEachStage;
	CString strTemp2 = _T("");
	int exposureCount = 0;
	//int ProcessCnt = 0;
	// AVI - Camera Mode 강제로 Overlap Mode 로 변경 - 필요 시 설정 구문 추가 할 것
	//UINT nTriggerMode = 4;		// Mode 0:FreeRun 1:Standard 2:Fast 3:Double 4:Overlap

	BOOL bFirstInspFlg = TRUE;

	int	nGrabCount = 0;
	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인dddd경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		nStepNo++;
		// Sequence In LOG
		m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_CHIPPING strPanelID = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);
		CString PatternName = theApp.m_Config.GetCurPatternName(strPosition, 0);
		switch (nStepNo)
		{
		case 1:
			break;
		case 2:
			m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control Start. Pattern : %s"), nStepNo, "123");		// 조명 On 전에 로그 추가		180511 YSS
			if (theApp.m_Config.GetPcName() == _T("LEFT"))
			{
				stCurLightInfo = theApp.m_Config.GetLightInfo(strPosition, nGrabCnt, 0);

				nRet += CmdEditSend(SEND_LIGHT_ON, 0, sizeof(STRU_LIGHT_INFO), VS_LIGHT_TASK, (byte *)&stCurLightInfo, CMD_TYPE_RES);

				if (nRet == APP_OK)
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON End. Pattern : %s"), nStepNo, "123"/*theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt + nNextStepInterval)*/);
				}
				else
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light ON Err or !!!"), nStepNo);
					throw nRet;
				}
			}
			break;

		case 3:
			stCurCamCond = theApp.m_Config.GetCameraConditions(strPosition, nGrabCnt,0);

			//nRet = CmdEditSend(SEND_SET_CAMERA_COND, 0, sizeof(ST_CAM_COND_AOT), VS_CAMERA_TASK, (byte *)&stCurCamCond, CMD_TYPE_RES, 6000);

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Expose Time : %f"), nStepNo, dExposeTime);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Set Camera Condition Error !!!"), nStepNo);
				//throw nRet;
			}
			break;

		case 4:
			// Vaccum 세팅
			// 통신 추가 필요
			nRet = 0;
			//nNextStepInterval = 1;

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Trigger Setting Camera"), nStepNo);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Trigger Setting Camera Error !!!"), nStepNo);
				//throw nRet;
			}
			break;



		case 5:
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)strPanelID);
			bpTempParam4 += 100;
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)strVirtualPanelID);
			bpTempParam4 += 100;


			*(int*)bpTempParam4 = ProcessCnt;
			bpTempParam4 += sizeof(int);
			_tcscpy((TCHAR*)bpTempParam4, (LPCTSTR)Temp1);
			bpTempParam4 += 100;
			//stCurCamCond = theApp.m_Config.GetLineInfo(strPosition, 0, 0);
			nRet = CmdEditSend(SEND_CAMERA_EXPOSE, 0, sizeof(bParam4), VS_CAMERA_TASK, (byte *)&bParam4, CMD_TYPE_NORES);

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera, ProcessCnt : %d"), nStepNo, ProcessCnt);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Expose Camera Error !!!, ProcessCnt : %d"), nStepNo, ProcessCnt);
				//throw nRet;
			}
			break;
		case 6:
			// 스테이지 이동 요청

			_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualPanelID);
			bpTempParam += 100;
			_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strPanelID);
			bpTempParam += 100;
			ResetEvent(m_hSeqStageMoveDone);
			nRet = CmdEditSend(REQ_GRAB_START, 0, 200, VS_MAIN_PC_TASK, (byte *)&bParam, CMD_TYPE_NORES);
			break;

		case 7:
			// 스테이지 이동 확인			
			nRet = WaitForSingleObject(m_hSeqStageMoveDone, 60000);

			if (nRet == WAIT_TIMEOUT)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Time Out Error : %d, Area : %s"), nGrabCount, strPosition);
				//throw;
			}
			else if (nRet == WAIT_FAILED)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Wait Fail Error : %d, Direction : %s"), nGrabCount, strPosition);
				//throw;
			}
			else if (nRet == WAIT_ABANDONED)
			{
				m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Error, Retry : %d, Direction : %s"), nGrabCount, strPosition);
				//ResetEvent(m_hSeqStageMoveDone);

				//nRet = WaitForSingleObject(m_hSeqStageMoveDone, 5000);

				//if (nRet != WAIT_OBJECT_0)
				//{
				//	m_fnPrintLog(FALSE, _T("SEQLOG -- Wait Stage Move Retry Error : %d, Direction : %s"), nGrabCount, strPosition);
				//}
				//throw;
			}
			else
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Stage Move OK"), nStepNo);

			//ResetEvent(m_hSeqStageMoveDone);
			break;
		case 8:
			//Sleep(300);
			//// 스테이지 이동 완료 후 그랩 종료
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Check !!!"), nStepNo);
			//_tcscpy((TCHAR*)bpTempParam3, (LPCTSTR)strVirtualPanelID);
			//bpTempParam3 += 100;
			//_tcscpy((TCHAR*)bpTempParam3, (LPCTSTR)strPanelID);
			//bpTempParam3 += 100;
			//nRet = CmdEditSend(SEND_GRAB_STOP, 0, sizeof(int), VS_CAMERA_TASK, (byte *)&nAsyncCount, CMD_TYPE_RES, 10000);
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Done !!!"), nStepNo);
			//ProcessCnt = *(int*)bpTempParam3;
			//m_fnPrintLog(FALSE, _T("CASE %d : Stage Move Done !!!, ProcessCnt : %d"), nStepNo, ProcessCnt);
			break;
		case 9:
			// 카메라 버퍼 -> 공유메모리
			strTemp2.Format(_T("%s%s\\%02d_%s_CAM%02d"), strFileDirectory, strPanelID, nGrabCnt, theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt), 0);
			_tcscpy(stWaitGrabEndParam.strSavePath, strTemp2);

			stCurCamCond = theApp.m_Config.GetCameraConditions(strPosition, nGrabCnt, 0);
			stWaitGrabEndParam.bUseSMem = TRUE;
			//stWaitGrabEndParam.nGrabNum = tWaitGrabEndParam.nSeqMode = stCurCamCond.PS;
			//stWaitGrabEndParam.nTriCountF = stCurCamCond.nCountF;
			//stWaitGrabEndParam.nTriCountB = stCurCamCond.nCountB;
			_tcscpy(stWaitGrabEndParam.strPanelID, strPanelID);
			stWaitGrabEndParam.GrabCnt =nGrabCnt;
			//_tcscpy(stWaitGrabEndParam.strGrabStepName, theApp.m_Config.GetCurStepName(strPosition, nGrabCnt));
			nRet = CmdEditSend(SEND_WAIT_CAMERA_GRAB_END_SEQUENCE, 0, sizeof(PARAM_WAIT_GRAB_END), VS_CAMERA_TASK, (byte *)&stWaitGrabEndParam, CMD_TYPE_RES, 60000);

			ImageSetTemp.ParticleImageCount = 1;
			ImageSetTemp.SharedMemStartIdx = nGrabCnt;
			memcpy((TCHAR *)ImageSetTemp.PatternName, theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt), sizeof(ImageSetTemp.PatternName));
			prmInspStart->ImageSet[nGrabCnt] = ImageSetTemp;
			prmInspStart->FirstPattern = true;
			//CameraRespons = (byte *)&stWaitGrabEndParam;
			//CameraRespons += sizeof(ProcessGrabCount);
			//ProcessGrabCount = *(int*)CameraRespons;
			//CameraRespons += sizeof(CameraRespons);
			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End"), nStepNo);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Wait Grab End Error !!!"), nStepNo);
				throw nRet;
			}
			break;
		case 10:
			m_fnPrintLog(FALSE, _T("CASE %d : Light Off Start. Pattern : %s"), nStepNo, "123");		// 조명 On 전에 로그 추가		180511 YSS

			if (theApp.m_Config.GetPcName() == _T("LEFT"))
			{
				stCurLightInfo = theApp.m_Config.GetLightInfo(strPosition, nGrabCnt, 0);

				nRet += CmdEditSend(SEND_LIGHT_OFF, 0, sizeof(ST_LIGHT_COND_AOT), VS_LIGHT_TASK, (byte *)&stCurLightInfo, CMD_TYPE_NORES);

				if (nRet == APP_OK)
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Light Off End. Pattern : %s"), nStepNo, "123"/*theApp.m_Config.GetCurPatternName(strPosition, nGrabCnt + nNextStepInterval)*/);
				}
				else
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Light Off Error !!!"), nStepNo);
					throw nRet;
				}
			}
			break;
		case 11:



			prmInspStart->PcNo = 0;
			prmInspStart->nInspType = 0;
			prmInspStart->PatternCount = theApp.m_Config.GetGrabCount(strPosition);
			memcpy((TCHAR *)prmInspStart->strPanelID, strPanelID, sizeof(prmInspStart->strPanelID));
			memcpy((TCHAR *)prmInspStart->strVirtualID, strVirtualPanelID, sizeof(prmInspStart->strVirtualID));
			memcpy((TCHAR *)prmInspStart->strArea, strAreaTemp, sizeof(prmInspStart->strArea));

			// 검사 시작은 무조건 NoRes 로 변경
			nRet = CmdEditSend(SEND_UI_INSP_START, 0, (ULONG)sizeof(PARAM_INSPECT_START_AOT_CHIPPING_ALM), VS_UI_TASK, (byte *)prmInspStart, CMD_TYPE_RES, 60000);

			//if (bFirstInspFlg)
			//	nRet = Seq_TactTimeData(strPanelID, TACT_INSP, TACT_START);
			bFirstInspFlg = FALSE;

			if (nRet == APP_OK)
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d"), nStepNo, nGrabCnt);
			}
			else
			{
				m_fnPrintLog(FALSE, _T("CASE %d : Start Auto Inspection - Image Count : %d Error !!!"), nStepNo, nGrabCnt);
				throw nRet;
			}
			break;
		
		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_ALM Error Occured. StepNo=%d, RetVal=%d"), nStepNo, nRet);

				// Error 발생 시 PG / Light Off
				int nErrRet = APP_OK;

				if (nRet == APP_OK || nErrRet == APP_OK)
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH End"), nStepNo);
				}
				else
				{
					m_fnPrintLog(FALSE, _T("CASE %d : Set Next Light Control CATCH Error !!!"), nStepNo);
				}

				isRunSequence = false;
				int nRetExcept = APP_OK;
			}

	} while (isRunSequence);
	//CmdEditSend(SEND_LIGHT_OFF, 0, 0, VS_LIGHT_TASK + 1, 0, CMD_TYPE_NORES);


	SAFE_DELETE(prmInspStart);

	// Sequence Out LOG
	m_fnPrintLog(FALSE, _T("SEQLOG -- Seq2120_AutoInspectGrabImage_ALM Sequence END. Barcode = %s, StepNo=%d, RetVal=%d"), strPanelID, nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}
int	WorkManager::m_ShareImgNum(int nImgNum)
{
	if (nImgNum == 0)
	{
		k = 1;
		m_count = -1;
	}
	if (nImgNum == theApp.m_Config.GetShareImgCount() * k)
	{
		m_count++;

		if (m_count == theApp.m_Config.GetShareImgCount())
		{
			k++;
			m_count = nImgNum - theApp.m_Config.GetShareImgCount() * (k - 1);
			return m_count;
		}
	}
	else
		m_count++;

	return m_count;
}
