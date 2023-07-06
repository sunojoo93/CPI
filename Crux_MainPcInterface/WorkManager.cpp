#include "stdafx.h"
#include "WorkManager.h" 
//#include "DefineSequenceTable.h"
#include "..\VisualStation\CommonHeader\Class\LogWriter.h"
#include "Crux_MainPcInterface.h"
#include "DefineStructure.h"
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define	SEQUENCE_TABLE(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME,ALWAYS_RUNMODE,SEQ_REST_POSSIBLE,CS_LOCK_NAME)					\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )										\
	{																														\
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			EnterCriticalSection(CS_LOCK_NAME);																				\
		}																													\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount++;																							\
			isFunctionRuned = true;																							\
			nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize, ALWAYS_RUNMODE, false, SEQ_REST_POSSIBLE);		\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount--;																							\
		if ((VOID*)CS_LOCK_NAME != NULL)																						\
		{																													\
			LeaveCriticalSection(CS_LOCK_NAME);																				\
		}																													\
	}



WorkManager::WorkManager()
{
	m_hWaitInspEndEvent		= CreateEvent( NULL, FALSE, FALSE, NULL );
	m_hSequenceResetEvent	= CreateEvent( NULL, TRUE,  FALSE,  NULL );
	m_bSeqResetFlag = 0;
	m_SeqenceCount = 0;
	m_skMainPcSocket = 0;

	// 2019.06.07 MDJ 재시작 시퀀스 대응
	m_nGrabCount = GetPrivateProfileInt(_T("Grab Limit"), _T("Grab Limit Count"), 0, INIT_FILE_PATH);

	//KYH 2021.04.01 AutoTheta
	m_ThetaCnt = 0;
	
	// 2019.06.11 MDJ Grab Count 사용여부
	if (m_nGrabCount > 0)
	{
		m_bGrabCount = true;
	}
	else
	{
		// 2019.07.02 MDJ Grab 중복 실행 오류 수정
		//m_nGrabCount = 1;
		m_bGrabCount = false;
	}

	// 2019.07.02 MDJ Grab 중복 실행 오류 수정
	//m_strGrabCellID = new CString[m_nGrabCount];
	m_strGrabCellID = new CString[m_nGrabCount + 1];

	//for (int nCnt = 0; nCnt < m_nGrabCount; nCnt++)
	for (int nCnt = 0; nCnt < m_nGrabCount + 1; nCnt++)
	{
		m_strGrabCellID[nCnt] = _T("");
	}

	// 2019.06.11 MDJ 세마포 갯수 Initialize.ini 파일에서 읽어온 값으로 설정
	// KYS 그랩 개수 제한 핸들 생성
	//m_hGrabSemaphore = CreateSemaphore(NULL, 3, 3, NULL);
	if (m_bGrabCount)
	{
		m_hGrabSemaphore = CreateSemaphore(NULL, m_nGrabCount, m_nGrabCount, NULL);
	}

	InitializeCriticalSection(&m_csMainPcInterface);

	InitializeCriticalSection(&m_csSequenceLock_1);
	InitializeCriticalSection(&m_csSequenceLock_2);
	InitializeCriticalSection(&m_csSequenceLock_3);
	InitializeCriticalSection(&m_csSequenceLock_4);
	InitializeCriticalSection(&m_csSequenceLock_5);
}


WorkManager::~WorkManager()
{
	DeleteCriticalSection(&m_csMainPcInterface);

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
		if ( handle == NULL || handle == (HANDLE)-1 )
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
		if ( nRet != APP_OK ) 
			throw nRet;

		EndWorkProcess(pMsg);

		pCmdMsg = NULL;
		pMsg = NULL;

		EXCEPTION_CATCH
			
		if ( nRet != APP_OK )
		{
			if ( pMsg != NULL )
			{
				EndWorkProcess(pMsg);
				pCmdMsg = NULL;
				pMsg = NULL;
			}
			m_fnPrintLog(_T("ERROR WorkManager::AnalyzeMsg. Error Code = %d \n"), nRet);
		}
	}


}

int WorkManager::AnalyzeMsg(CMDMSG* pCmdMsg)
{
	int			nRet = APP_OK;
	bool		isFunctionRuned = false;	

	EXCEPTION_TRY

//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,						중복실행가능여부,	시퀀스 리셋 가능 여부	)
	SEQUENCE_TABLE (	99,		99	,	Seq_SeqeunceReset					, false	,			false					,	&m_csSequenceLock_1	)	
	SEQUENCE_TABLE (	10,		99	,	Seq_TEST							, false	,			true					,	NULL				)

	// Alive																												 
	SEQUENCE_TABLE (	10,		01	,	VS_TaskAlive						, false	,			true					,	NULL				)

	SEQUENCE_TABLE (	10,		10	,	Seq_SendMessageToMainPC				, false	,			false					,	NULL				)

	SEQUENCE_TABLE (	60,		01	,	Seq_StartGrab_FromMainPc			, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		23	,	Seq_AFReady_FromMainPc				, true ,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		04	,	Seq_Reply_StartGrabFromMainPc		, false, false, NULL)
	SEQUENCE_TABLE (	60,		02	,	Seq_ChangeRecipe_FromMainPc			, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		05	,	Seq_GrabEnd_FromMainPc				, false ,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		22	,	Seq_SendMsg_ClassifyEnd_FromInspPc	, true	,			false					,	NULL				)

	SEQUENCE_TABLE (    60,     60  ,   Seq_StageMove_FromInspPc            , true  ,           false                   ,   NULL                )
	SEQUENCE_TABLE(		60,     50  ,   Seq_ReqLineGrab_FromInspPc          , false ,           false                   ,   NULL                )
	SEQUENCE_TABLE (	60,		51	,	Seq_ReqCurrentRecipe_FromInspPc		, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		52	,	Seq_OccurAlarm_FromInspPc			, true	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		53	,	Seq_MoveMotion_FromInspPc			, true	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		54	,	Seq_SetRecipeMotion_FromInspPc		, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		55	,	Seq_ReqVacuum_FromInspPc			, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		56	,	Seq_ReqMotionStatus_FromInspPc		, true	,			false					,	NULL				)
	// Cell Align 결과 송부 : Theta, Distance (셀 중심과 카메라 중심 거리) X, Y
	SEQUENCE_TABLE (	60,		61	,	Seq_AlignResult_FromInspPc			, true	,			false					,	NULL				)
	SEQUENCE_TABLE (	60,		57	,	Seq_PGChangePatternCmd_FromInspPc	, true	,			false					,	NULL				)	// PG Change Pattern Command. AMT 설비에서 운영 프로그램을 통해서 PG 제어	180801 YSS
	SEQUENCE_TABLE (	60,		58	,	Seq_PGOffCmd_FromInspPc				, true	,			false					,	NULL				)	// PG Off Command. AMT 설비에서 운영 프로그램을 통해서 PG 제어	180801 YSS
	SEQUENCE_TABLE (	60,		59	,	Seq_PGOnCmd_FromInspPc				, true	,			false					,	NULL				)	// PG On Command. AMT 설비에서 운영 프로그램을 통해서 PG 제어	180822 YSS

/*******************************************************************************************************************/
/*****             Child Sequence, 이하는 다른 타스크에서 호출되지 않고, 내부적으로만 사용된다.               ******/
/*******************************************************************************************************************/
//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,						중복실행가능여부,	시퀀스 리셋 가능 여부	)
	SEQUENCE_TABLE (	90,		01	,	Wait_Event_ClassifyEnd				, true	,			false					,	NULL				)

/*******************************************************************************************************************/
/*****             Private Sequence, 이하는 다른 타스크에서 호출되지 않고, 내부적으로만 사용된다.             ******/
/*******************************************************************************************************************/
//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,						중복실행가능여부,	시퀀스 리셋 가능 여부	)
	SEQUENCE_TABLE (	20,		01	,	StartMainPcMsgReceiver				, false	,			false					,	NULL				)
	SEQUENCE_TABLE (	61,		01	,	Seq_StartSimulation					, false	,			true					,	NULL				)

																			
	if( m_SeqenceCount <= 0 )
	{
		m_bSeqResetFlag = 0;
		m_SeqenceCount = 0;
		ResetEvent(m_hSequenceResetEvent);
	}

	if ( isFunctionRuned  == false ) 
	{
		m_fnPrintLog(_T("Function Table is nothing. FuncNO : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		throw MAINPCINTERFACE_TASK_SEQUENCE_IS_NOTHING;
	}
		

	EXCEPTION_CATCH

	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
	{
		nRet = ResponseSend(nRet, pCmdMsg);
		if ( nRet != APP_OK ) 
		{
			m_fnPrintLog(_T("Response Send  Fail. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
			return nRet;
		}
	}

	return nRet;
}

int WorkManager::Seq_StageMove_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /* = false */, bool bBusyCheck /* = false */, bool bSeqResetPossible /* = true */)
{
	byte *pReceiveParam = pParam;

	int nGrabCount = *(int*)pReceiveParam; pReceiveParam += sizeof(int);
	int nKindDirection = *(int*)pReceiveParam; pReceiveParam += sizeof(int);

	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;
	int nReturnVal = 0;

	CStringA strSendMsg;
	CString strDirection = _T("");

	m_nGrabCnt = nGrabCount;
	m_nDirection = nKindDirection;

	// 2021.12.15~ MDJ Modify Motor Speed
	int nMotorSpeed = 0;
	if (m_nDirection == 0)
	{
		nMotorSpeed = *(int*)pReceiveParam; pReceiveParam += sizeof(int);
	}

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- StageMove_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			if (theApp.GetSimulationMode())
			{
				nRet = APP_OK;
				nStepNo = 9999;
				break;
			}

			break;

		case 2:
			// Main PC 로 전달한다.

			switch (nKindDirection)
			{
				// 2021.12.15~ MDJ Modify Motor Speed
			case 0: strSendMsg.Format("%s", MAIN_PC_PACKET_STAGEMOVE_FORWARD);
				strSendMsg.Format("%s%3d.", MAIN_PC_PACKET_STAGEMOVE_FORWARD, nMotorSpeed);
				strDirection.Format(_T("Forward"));
				break;
			case 1: strSendMsg.Format("%s", MAIN_PC_PACKET_STAGEMOVE_BACKWARD);
				strDirection.Format(_T("Backward"));
				break;
			default:
				break;
			}

			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			break;

		case 3:
			switch (nKindDirection)
			{
			case 0:
				ResetEvent(m_stMainPcReplyEvent.EI_STAGEMOVE_FORWARD.hEvent);
				WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_STAGEMOVE_FORWARD.hEvent, TIMEOUT_MAINPC_REPLY);
				nReturnVal = *(int*)&m_stMainPcReplyEvent.EI_STAGEMOVE_FORWARD.bParam[0];
				break;
			case 1:
				ResetEvent(m_stMainPcReplyEvent.EI_STAGEMOVE_BACKWARD.hEvent);
				WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_STAGEMOVE_BACKWARD.hEvent, TIMEOUT_MAINPC_REPLY);
				nReturnVal = *(int*)&m_stMainPcReplyEvent.EI_STAGEMOVE_BACKWARD.bParam[0];
				break;
			default:
				break;
			}
			m_fnPrintLog(_T("SEQLOG -- StageMove_FromInspPc. Set Status = %d."), nReturnVal);
			break;

		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(_T("SEQLOG -- StageMove_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- StageMove_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	/*switch (nKindDirection)
	{
	case 0 : strSendMsg.Format("%s", MAIN_PC_PACKET_STAGEMOVE_FORWARD);
	strDirection.Format(_T("Forward"));
	break;
	case 1 : strSendMsg.Format("%s", MAIN_PC_PACKET_STAGEMOVE_BACKWARD);
	strDirection.Format(_T("Backward"));
	break;
	default:
	break;
	}

	nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());

	if (nRet != APP_OK)
	{
	m_fnPrintLog(_T("SEQLOG -- SendMsgToMainPC STAGE.MOVE.%s.ERR"), (CStringA)strDirection);
	return nRet;
	}*/

	return nRet;
}

int	WorkManager::Seq_SeqeunceReset(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;


	do
	{
		EXCEPTION_TRY
			 
		//if( m_bSeqResetFlag && bSeqResetPossible )
		//	throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- Seq2599_SeqeunceReset StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			m_bSeqResetFlag = 1;
			SetEvent(m_hSequenceResetEvent);
			break;

		case 2:
			// 초기화 수행

			break;

		case 3:

			for (int nCnt = 0; nCnt < 200; nCnt++)
			{
				if( m_SeqenceCount <= 0 )
				{
					m_bSeqResetFlag = 0;
					m_SeqenceCount	= 0;
					ResetEvent(m_hSequenceResetEvent);
					break;
				}
				else
					Sleep(50);
			}

			m_bSeqResetFlag = 0;
			m_SeqenceCount	= 0;
			ResetEvent(m_hSequenceResetEvent);
			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2599_SeqeunceReset Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq2599_SeqeunceReset Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}


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

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			//usSet = *(USHORT*)tempParam;
			//tempParam += sizeof(USHORT);
			//nVal = *(int*)tempParam

			m_fnPrintLog(_T("CASE 1"));
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

			m_fnPrintLog(_T("CASE 2"));

			break;

		case 3:
			m_fnPrintLog(_T("CASE 3"));

			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

			// EQP BIT ALL OFF

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::VS_TaskAlive(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
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

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		//m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			
			//Sleep(500);
			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- VS_TaskAlive Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

			// EQP BIT ALL OFF

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	//m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

int	WorkManager::Seq_SendMessageToMainPC(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	int nSendSize = 0;
	byte *pTempParam = pParam;
	byte *pSendData = NULL;
	do
	{
		EXCEPTION_TRY

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- SendMessageToMainPC StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			nSendSize = *(int*)pTempParam;
			pTempParam += sizeof(int);
			pSendData = pTempParam;

			nRet = m_fnSendMessageToMainPc((char*)pSendData, nSendSize);
			if (nRet != APP_OK)
			{
				throw nRet;
			}
			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- SendMessageToMainPC Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

			// EQP BIT ALL OFF

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- SendMessageToMainPC Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::StartMainPcMsgReceiver(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	TCHAR szMainPcIP[100] = {0,};
	int nSocketMode = 0, nPort = 0;
	int nServerSocket = 0;
	char cBuffer[2000] = {0,};
	CString strIniFile;

	do
	{
		EXCEPTION_TRY

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		//m_fnPrintLog(_T("SEQLOG -- StartMainPcMsgReceiver StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			// Socket 연결 환경 설정 정보를 가져온다.

			

			if (theApp.GetSimulationProgramMode())
				strIniFile = VS_MAIN_PC_SIMULATOR_TASK_INI_FILE;
			else
				strIniFile = VS_MAIN_PC_INTERFACE_TASK_INI_FILE;

			nSocketMode		= GetPrivateProfileInt(_T("MainPCSettings"), _T("Socket Mode"), 0, strIniFile);
			nPort			= GetPrivateProfileInt(_T("MainPCSettings"), _T("Port"), 7000, strIniFile);
			GetPrivateProfileString(_T("MainPCSettings"), _T("MainPcIP") ,_T("127.0.0.1"), szMainPcIP, sizeof(szMainPcIP), strIniFile);
			break;

		case 2:
			if (m_skMainPcSocket)
			{
				closesocket(m_skMainPcSocket);
				m_skMainPcSocket = 0;
			}
			if (nServerSocket)
			{
				closesocket(nServerSocket);
				nServerSocket = 0;
			}

			memset(cBuffer, 0, sizeof(cBuffer));

			if (nSocketMode == SOCKET_SERVER_MODE)
				nStepNo = 2;
			else
				nStepNo = 3;

			break;

		case 3:
			// 서버 연결 모드로 클라이언트의 연결을 대기한다.
			m_fnPrintLog(_T("Start Server"));

			nRet = m_fnStartServer(nPort, nServerSocket);
			if (nRet != APP_OK)
			{
				nRet = APP_OK;
				nStepNo = 2;		// Retry
			}
			else
				nStepNo = 4;	// Goto Step 5
			break;

		case 4:
			// 클라이언트 연결 모드로 서버의 연결을 시도한다.
			m_fnPrintLog(_T("Connet to Main PC"));
			nRet = m_fnConnectServer(nPort, szMainPcIP, m_skMainPcSocket);
			if (nRet != APP_OK)
			{
				//m_fnPrintLog(_T("Server Connect fail."));
				nRet = APP_OK;
				nStepNo = 3;
				Sleep(1000);
			}
			break;

		case 5:
			// 메시지를 수신한 후, stx, etx 를 보고 메시지를 파싱한다.
			memset(cBuffer, 0, sizeof(cBuffer));
			nRet = m_fnReceiveMessage((byte*)cBuffer, sizeof(cBuffer));
			if (nRet != APP_OK)
			{
				nRet = APP_OK;

				nStepNo = 1;
			}
			break;

		case 6:
			// 서버 커맨드이면 다른 타스크에 필요한 메시지를 송신한다.
			nRet = m_fnAnalyzeMainPcCmd(cBuffer, sizeof(cBuffer));
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartMainPcMsgReceiver Error Occured. m_fnAnalyzeMainPcCmd, RetVal=%d \n"), nRet);
				nRet = APP_OK;

				nStepNo = 4;
			}
			break;

		case 7:
			// 검사 PC 메시지의 회신이면 대기 중인 이벤트를 찾아 대기이벤트를 깨운다.
			nRet = m_fnWakeUpSequence(cBuffer, sizeof(cBuffer));
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartMainPcMsgReceiver Error Occured. m_fnWakeUpSequence, RetVal=%d \n"), nRet);
				nRet = APP_OK;

				nStepNo = 4;
			}
			break;

		case 8:

			nStepNo = 4;
			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- StartMainPcMsgReceiver Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			//isRunSequence = false;
			int nRetExcept =APP_OK;
			nStepNo = 4;
			

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- StartMainPcMsgReceiver Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_StartGrab_FromMainPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = false;
	int nStepNo = 0, nAsyncFlag = 0, nTestMode = 0, nStartLine = 0, nTotalLine = 0;
	int			nStageNo = 0;
	static bool isSeqBusy = false;
	byte*		pParamTemp = pParam;


	CString		strCellID;
	CString     strDirection;
	CString     strPosition;
	bool		bReqAnalysis;
	CString		strLotID;
	CString		strVirtualID;
	CString		strCellIDTmp;	  // 2017.10.06 edit by ikm

	TCHAR		cTemp[500] = { 0, };
	TCHAR szTemp[100] = { 0, };
	CStringA	strSendMsg;
	CStringA	strClassEnd;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strVirtualID = cTemp;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strCellID = cTemp;

	nTotalLine = *(int*)pParamTemp;		pParamTemp += sizeof(int);

	//memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	//pParamTemp += SIZE_CELL_ID;
	//strDirection = cTemp;

	//memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	//pParamTemp += SIZE_CELL_ID;
	//strPosition = cTemp;


	/*CString strRecipeName;
	GetPrivateProfileString(_T("RECIPE_INFO"), _T("SELECT_RECIPE"), _T("No_Selected"), szTemp, sizeof(szTemp), INIT_FILE_PATH);
	strRecipeName = szTemp;*/

	EXCEPTION_TRY

	isSeqBusy = true;

	// Sequence In LOG
	m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc StepNo=%d, RetVal=%d \n"), nStepNo++, nRet);


	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;

	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualID);
	bpTempParam += SIZE_CELL_ID;
	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strCellID);
	bpTempParam += SIZE_CELL_ID;
	*(int*)bpTempParam = nTotalLine;
	bpTempParam += sizeof(int);
	//_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strDirection);
	//bpTempParam += SIZE_CELL_ID;
	//_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strPosition);
	//bpTempParam += SIZE_CELL_ID;


	//strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_START_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
	//nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());


	//nRet = 0;
	//if (nRet != APP_OK)
	//{
	//	strSendMsg.Format("%sERR.%s.%s.", MAIN_PC_PACKET_GRAB_START_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
	//	nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
	//	m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Message Send Error(FDB.GRAB.START.ERR)."));
	//	throw nRet;
	//}
	//
	//TEST

	//strSendMsg.Format("%s%d.OK.", MAIN_PC_PACKET_GRAB_END_REPLY, nTotalLine);
	//nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
	

	nRet = CmdEditSend(START_GRAB_REQ, 0, sizeof(bParam), VS_SEQUENCE_TASK, bParam, CMD_TYPE_RES, TIMEOUT_GRAB_END_OK);

	EXCEPTION_CATCH

		if (nRet == APP_OK)
		{
			
			strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());

			if (theApp.GetInspType() == 5)
			{
					if (nTotalLine == 3)
					{
						Sleep(2000);
						strClassEnd.Format("%sOK.%s.%s.", MAIN_PC_PACKET_CLASSIFY_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
						nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strClassEnd, strClassEnd.GetLength());
					}
			}
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc INSP End Err... Message Send Error."));
				throw nRet;
			}
		}
		else
		{
			m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc - Error return from sequence task. Error = %d"), nRet);
			strSendMsg.Format("%sERR.%s.%s.", MAIN_PC_PACKET_GRAB_END_REPLY, (CStringA)strVirtualID, strCellID);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());

			if (theApp.GetInspType() == 5)
			{
				if (nTotalLine == 3)
				{
					Sleep(2000);
					strClassEnd.Format("%sERR.%s.%s.", MAIN_PC_PACKET_CLASSIFY_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
					nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strClassEnd, strClassEnd.GetLength());
				}
			}
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Message Send Error."));
				throw nRet;
			}
			throw MAINPCINTERFACE_TASK_SEQUENCE_FAIL;
		}

	m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int	WorkManager::Seq_AFReady_FromMainPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = false;
	int nStepNo = 0, nAsyncFlag = 0, nTestMode = 0, nStartLine = 0, nPos = 0, nArea = 0;
	int			nStageNo = 0;
	static bool isSeqBusy = false;
	byte*		pParamTemp = pParam;


	CString		strCellID;

	bool		bReqAnalysis;
	CString		strLotID;
	CString		strVirtualID;
	CString		strCellIDTmp;	  // 2017.10.06 edit by ikm

	TCHAR		cTemp[500] = { 0, };
	TCHAR szTemp[100] = { 0, };
	CStringA	strSendMsg;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strVirtualID = cTemp;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strCellID = cTemp;

	nPos = *(int*)pParamTemp;		pParamTemp += sizeof(int);

	nArea = *(int*)pParamTemp;		pParamTemp += sizeof(int);
	EXCEPTION_TRY

		isSeqBusy = true;

	// Sequence In LOG
	m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc StepNo=%d, RetVal=%d \n"), nStepNo++, nRet);
	m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc Vir ID : %s, Panel ID : %s, PC Pos : %d, Area : %d \n"), strVirtualID,strCellID, nPos, nArea);



		byte bParam[1000] = { 0, };
		byte* bpTempParam = bParam;

		_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualID);
		bpTempParam += SIZE_CELL_ID;
		_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strCellID);
		bpTempParam += SIZE_CELL_ID;
		*(int*)bpTempParam = nPos;
		bpTempParam += sizeof(int);
		*(int*)bpTempParam = nArea;
		bpTempParam += sizeof(int);

		nRet = CmdEditSend(START_AF_READY, 0, sizeof(bParam), VS_SEQUENCE_TASK, bParam, CMD_TYPE_NORES, 60000);
	EXCEPTION_CATCH

		if (nRet == APP_OK)
		{
			strSendMsg.Format("%sOK.%s.%s.%d.%d.", MAIN_PC_PACKET_GRAB_READY_REPLY, (CStringA)strVirtualID, (CStringA)strCellID, nPos, nArea);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc  INSP End Err... Message Send Error."));
				throw nRet;
			}
		}
		else
		{
			m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc - Error return from sequence task. Error = %d"), nRet);
			strSendMsg.Format("%sERR.%s.%s.%d.%d.", MAIN_PC_PACKET_GRAB_READY_REPLY, (CStringA)strVirtualID, strCellID, nPos, nArea);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc Message Send Error."));
				throw nRet;
			}
			throw MAINPCINTERFACE_TASK_SEQUENCE_FAIL;
		}

	m_fnPrintLog(_T("SEQLOG -- Start AF Ready_FromMainPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int	WorkManager::Seq_GrabEnd_FromMainPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = false;
	int nStepNo = 0, nAsyncFlag = 0, nTestMode = 0, nStartLine = 0, nPos = 0;
	int			nStageNo = 0;
	static bool isSeqBusy = false;
	byte*		pParamTemp = pParam;


	CString		strCellID;

	bool		bReqAnalysis;
	CString		strLotID;
	CString		strVirtualID;
	CString		strCellIDTmp;	  // 2017.10.06 edit by ikm

	TCHAR		cTemp[500] = { 0, };
	TCHAR szTemp[100] = { 0, };
	CStringA	strSendMsg;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strVirtualID = cTemp;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strCellID = cTemp;

	EXCEPTION_TRY

		isSeqBusy = true;

	// Sequence In LOG
	m_fnPrintLog(_T("SEQLOG -- StartGrabReady_FromMainPc StepNo=%d, RetVal=%d \n"), nStepNo++, nRet);


	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;

	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualID);
	bpTempParam += SIZE_CELL_ID;
	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strCellID);
	bpTempParam += SIZE_CELL_ID;
	//*(int*)bpTempParam = 5;
	//bpTempParam += sizeof(int);


	nRet = CmdEditSend(START_GRAB_END, 0, sizeof(bParam), VS_SEQUENCE_TASK, bParam, CMD_TYPE_RES, 60000);

	//if (nRet == APP_OK)
	//{
	//	strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_START_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
	//	nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
	//}
	//else
	//{
	//	strSendMsg.Format("%sERR.%s.%s.", MAIN_PC_PACKET_GRAB_START_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
	//	nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
	//}



	EXCEPTION_CATCH

		if (nRet == APP_OK)
		{

			//strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
			//nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartGrabReady_FromMainPc  INSP End Err... Message Send Error."));
				throw nRet;
			}
		}
		else
		{
			m_fnPrintLog(_T("SEQLOG -- StartGrabReady_FromMainPc - Error return from sequence task. Error = %d"), nRet);
			strSendMsg.Format("%sERR.%s.%s.", MAIN_PC_PACKET_GRAB_END_REPLY, (CStringA)strVirtualID, strCellID);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			isSeqBusy = false;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- StartGrabReady_FromMainPc Message Send Error."));
				throw nRet;
			}
			throw MAINPCINTERFACE_TASK_SEQUENCE_FAIL;
		}

	m_fnPrintLog(_T("SEQLOG -- StartGrabReady_FromMainPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int	WorkManager::Seq_Reply_StartGrabFromMainPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{

	int nRet = APP_OK;
	bool isRunSequence = false;
	int nStepNo = 0, nAsyncFlag = 0, nTestMode = 0, nStartLine = 0, nPos = 0;
	int			nStageNo = 0;
	static bool isSeqBusy = false;
	byte*		pParamTemp = pParam;


	CString		strCellID;
	CString		strVirtualID;


	TCHAR		cTemp[500] = { 0, };
	TCHAR szTemp[100] = { 0, };
	CStringA	strSendMsg;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strVirtualID = cTemp;

	memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
	pParamTemp += SIZE_CELL_ID;
	strCellID = cTemp;

	EXCEPTION_TRY
		m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc StepNo=%d, RetVal=%d \n"), nStepNo++, nRet);
		isSeqBusy = true;

	// Sequence In LOG



	byte bParam[1000] = { 0, };
	byte* bpTempParam = bParam;

	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strVirtualID);
	bpTempParam += SIZE_CELL_ID;
	_tcscpy((TCHAR*)bpTempParam, (LPCTSTR)strCellID);
	bpTempParam += SIZE_CELL_ID;

	strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_START_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
	nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());

	EXCEPTION_CATCH

		if (nRet == APP_OK)
		{

			//strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_GRAB_READY_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
			//nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			isSeqBusy = false;
			m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc OK."));
			throw nRet;
			
		}
		else
		{
			isSeqBusy = false;
			m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Err."));
			throw MAINPCINTERFACE_TASK_SEQUENCE_FAIL;
		}

	m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}



int	WorkManager::Seq_ChangeRecipe_FromMainPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CString		strRecipeID;
	TCHAR		cTemp[500] = {0,};
	CStringA	strSendMsg;



	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- ChangeRecipe_FromMainPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				// 레서피 ID 취득
				{
					memcpy(cTemp, pParamTemp, SIZE_RECIPE_ID);

					strRecipeID = cTemp;

					m_fnPrintLog(_T("Change Recipe from Main PC. Recipe ID = %s"), strRecipeID);
				}
				
				break;

			case 2:
				// 레서피 변경 커맨드를 송신한다.
				// Todo.
				Sleep(1000);
				//변경결과에 따른 회신을 한다.
				strSendMsg.Format("%sOK.%s.", MAIN_PC_PACKET_RECIPE_CHANGE_REPLY, (CStringA)strRecipeID);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc Message Send Error."));
					throw nRet;
				}

				break;


			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- ChangeRecipe_FromMainPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- ChangeRecipe_FromMainPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_ReqCurrentRecipe_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	TCHAR szTemp[100] = { 0, };
	CString strRecipeName;
	GetPrivateProfileString(_T("RECIPE_INFO"), _T("SELECT_RECIPE"), _T("No_Selected"), szTemp, sizeof(szTemp), INIT_FILE_PATH);
	strRecipeName = szTemp;

	//USHORT	usSet;
	//INT		nVal;
	//byte*	tempParam = pParam;
	CStringA	strSendMsg;
	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- ReqCurrentRecipe_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				if (theApp.GetSimulationMode())
				{
					// 현재 레서피를 호출한 타스크에 응답한다.
					//TCHAR szParam[SIZE_RECIPE_ID] = {_T("aaaa")};
					//memcpy(pParam, szParam, SIZE_RECIPE_ID);
					memcpy(pParam, (LPCTSTR)strRecipeName, SIZE_RECIPE_ID);
					nRet = APP_OK;
					nStepNo = 9999;
					break;
				}

				// Main PC 로 현재 레서피를 요청한다.
				strSendMsg.Format("%s", MAIN_PC_PACKET_REQ_CURRENT_RECIPE);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- Seq_ReqCurrentRecipe_FromInspPc Message Send Error."));
					throw nRet;
				}
				break;

			case 2:
				// 회신을 대기한다.
				ResetEvent(m_stMainPcReplyEvent.EI_CurrentRecipe.hEvent);
				WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_CurrentRecipe.hEvent, TIMEOUT_MAINPC_REPLY);

				break;

			case 3:
				// 현재 레서피를 호출한 타스크에 응답한다.
				memcpy(pParam, m_stMainPcReplyEvent.EI_CurrentRecipe.bParam, SIZE_RECIPE_ID);
				nRet = APP_OK;
				break;


			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- ReqCurrentRecipe_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- ReqCurrentRecipe_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_OccurAlarm_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	//USHORT	usSet;
	//INT		nVal;
	//byte*	tempParam = pParam;
	byte*		pParamTemp = pParam;
	int			nAlarmID = 0, nAlarmType = 99, nRcvAlarmID = 0;
	CStringA	strSendMsg;



	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- OccurAlarm_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			CString AlarmName = _T("Test Alarm");
			CString AlarmMsg = _T("Test Message");
			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				// 알람ID을 취득한다. 알람 Type을 취득한다.
				{
					nRcvAlarmID = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
					nAlarmType = *(int*)pParamTemp;
					pParamTemp += sizeof(int);

				}

				{
					int const BASE_ALARM_ID		= 50000;
					int const PC_OFFSET			= 5000;
					int const INSP_ZONE_OFFSET	= 10000;
					// AlarmID 를 검사기 pc 별 기준 번호를 더한다.
					nAlarmID = theApp.GetInspType()*INSP_ZONE_OFFSET + (theApp.GetPCNum() -1)*PC_OFFSET + BASE_ALARM_ID + nRcvAlarmID;
				}

				m_fnPrintLog(_T("Alarm Occured. Alarm ID = %d, Alarm Type = %d"), nAlarmID, nAlarmType);
				break;

			case 2:
				// Main PC 로 전달한다.
				strSendMsg.Format("%s%05d.%02d.%s.%s.", MAIN_PC_PACKET_ALARM_OCCURED, nAlarmID, nAlarmType, AlarmName, AlarmMsg);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- StartGrab_FromMainPc <CASE 2> Message Send Error."));
					throw nRet;
				}


				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- OccurAlarm_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- OccurAlarm_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}




int	WorkManager::Seq_MoveMotion_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	int			nRelativeMoveValue = 0, nRelativeMoveReplyValue = 0, nMovedOk = 0;
	CString		strMotionName;
	TCHAR		cTemp[100] = {0,};
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:

				if (theApp.GetSimulationMode())
				{
					nRet = APP_OK;
					nStepNo = 9999;
					break;
				}
				// 이동할 축 명을 취득한다, 상대 이동값을 취득한다.
				{

					memcpy(cTemp, pParamTemp, SIZE_MOTOR_TYPE_NAME);
					pParamTemp += SIZE_MOTOR_TYPE_NAME;
					
					strMotionName = cTemp;

					nRelativeMoveValue = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
				}

				if (strMotionName == _T("MOTOR_X") && m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_Y") && m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_T") && m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_Z") && m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("FOCUS_Z") && m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("ZOOM_Z") && m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName != _T("MOTOR_X") && strMotionName != _T("MOTOR_Y") && strMotionName != _T("MOTOR_T") &&
					strMotionName != _T("MOTOR_Z") && strMotionName != _T("FOCUS_Z") && strMotionName != _T("ZOOM_Z"))
					throw MAINPCINTERFACE_TASK_SEQUENCE_PARAMETER_IS_WRONG;


				break;

			case 2:
				// Main PC 로 전달한다.
				if (nRelativeMoveValue < 0)
				{
					nRelativeMoveValue = nRelativeMoveValue * -1;
					strSendMsg.Format("%s%s.-%d.", MAIN_PC_PACKET_MOTION_RELATIVE_MOVE, (CStringA)strMotionName, nRelativeMoveValue);
				}
				else
					strSendMsg.Format("%s%s.+%d.", MAIN_PC_PACKET_MOTION_RELATIVE_MOVE, (CStringA)strMotionName, nRelativeMoveValue);
				
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());

				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc <CASE 2> Message Send Error."));
					throw nRet;
				}
			break;

			case 3:
				// Main PC 로 부터의 이동 결과 회신을 대기한다.
				if (strMotionName == _T("MOTOR_X"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam[sizeof(int)];
				}
				else if (strMotionName == _T("MOTOR_Y"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam[sizeof(int)];
				}
				else if (strMotionName == _T("MOTOR_T"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam[sizeof(int)];
				}
				else if (strMotionName == _T("MOTOR_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam[sizeof(int)];
				}
				else if (strMotionName == _T("FOCUS_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam[sizeof(int)];
				}
				else if (strMotionName == _T("ZOOM_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.hEvent);
					m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.hEvent, TIMEOUT_MAINPC_MOTION_MOVE_REPLY);
					nRelativeMoveReplyValue = *(int*)m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam;
					nMovedOk				= *(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam[sizeof(int)];
				}

				break;

			case 4:
				if (strMotionName == _T("MOTOR_X"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam));
				}
				else if (strMotionName == _T("MOTOR_Y"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam));
				}
				else if (strMotionName == _T("MOTOR_T"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam));
				}
				else if (strMotionName == _T("MOTOR_Z"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam));
				}
				else if (strMotionName == _T("FOCUS_Z"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam));
				}
				else if (strMotionName == _T("ZOOM_Z"))
				{
					m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam));
				}
				break;

			case 5:
				// 이동량을 회신한다.
				// Todo.
				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

					if (strMotionName == _T("MOTOR_X"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam));
					}
					else if (strMotionName == _T("MOTOR_Y"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam));
					}
					else if (strMotionName == _T("MOTOR_T"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam));
					}
					else if (strMotionName == _T("MOTOR_Z"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam));
					}
					else if (strMotionName == _T("FOCUS_Z"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam));
					}
					else if (strMotionName == _T("ZOOM_Z"))
					{
						m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam));
					}
					return nRet;	// Exception 발생 시 Return
				}

	}while ( isRunSequence );

	// 17.06.21 Motor 동작 결과 Return 으로 변경
	// Sequence Out LOG
	//m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
	m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nMovedOk);

	isSeqBusy = false;

	return nMovedOk;
}


int	WorkManager::Seq_SetRecipeMotion_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CString		strRecipeID;
	TCHAR		cTemp[500] = {0,};
	CStringA	strSendMsg;
	int			nReturnVal = 0;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- SetRecipeMotion_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				if (theApp.GetSimulationMode())
				{
					nRet = APP_OK;
					nStepNo = 9999;
					break;
				}

				// 레서피 아이디 취득
				{
					memcpy(cTemp, pParamTemp, SIZE_RECIPE_ID);
					pParamTemp += SIZE_RECIPE_ID;

					strRecipeID = cTemp;
				}
				break;

			case 2:
				// Main PC 로 전달한다.
				strSendMsg.Format("%s%s.", MAIN_PC_PACKET_MOTION_SET_RECIPE_POS, (CStringA)strRecipeID);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- SetRecipeMotion_FromInspPc <CASE 2> Message Send Error."));
					throw nRet;
				}
				break;

			case 3:
				// Main PC 로 부터의 결과 회신을 대기한다.
				ResetEvent(m_stMainPcReplyEvent.EI_SetRecipePos.hEvent);
				WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_SetRecipePos.hEvent, TIMEOUT_MAINPC_REPLY);
				nReturnVal = *(int*)&m_stMainPcReplyEvent.EI_SetRecipePos.bParam[0];
				m_fnPrintLog(_T("SEQLOG -- SetRecipeMotion_FromInspPc. Set Status = %d."), nReturnVal);
				break;

			case 4:
				// 결과를 회신한다. Todo..
				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- SetRecipeMotion_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- SetRecipeMotion_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_ReqVacuum_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	int			nVacuumType = 0, nReturnVal = 0;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				if (theApp.GetSimulationMode())
				{
					nRet = APP_OK;
					nStepNo = 9999;
					break;
				}

				// 변경 할 Vacuum Type을 취득, 
				{
					nVacuumType = *(int*)pParamTemp;
				}
				break;

			case 2:
				// Main PC 로 전달한다.

				strSendMsg.Format("%s%d.", MAIN_PC_PACKET_VACUUM_SWITCH, nVacuumType);
				ResetEvent(m_stMainPcReplyEvent.EI_SetVacuum.hEvent);	// 원래 아래있던거 Send 하기 전으로 순서 변경
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc <CASE 2> Message Send Error."));
					throw nRet;
				}
				break;

			case 3:
				// Main PC 로 부터의 결과 회신을 대기한다. PAUSE, RESUME
				//ResetEvent(m_stMainPcReplyEvent.EI_SetVacuum.hEvent);	// 위로 옮김
				WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_SetVacuum.hEvent, TIMEOUT_MAINPC_REPLY);
				nReturnVal = *(int*)&m_stMainPcReplyEvent.EI_SetVacuum.bParam[0];
				m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc. Set Status = %d."), nReturnVal);
				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF

				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

int	WorkManager::Seq_ReqLineGrab_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	int			nGrabNum = 0, nReturnVal = 0;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- ReqLINE_GRAB_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			if (theApp.GetSimulationMode())
			{
				nRet = APP_OK;
				nStepNo = 9999;
				break;
			}
			else
			{
				nGrabNum = *(int*)pParamTemp;
			}
			break;

		case 2:
			// Main PC 로 전달한다.

			//strSendMsg.Format("%s%d.OK", MAIN_PC_PACKET_GRAB_END_REPLY,nGrabNum);
			
			
			//nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			nRet = APP_OK;
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG -- ReqLINE_GRAB_FromInspPc <CASE 2> Message Send Error."));
				throw nRet;
			}
			break;

		case 3:
			// Main PC 로 부터의 결과 회신을 대기한다. PAUSE, RESUME
			//ResetEvent(m_stMainPcReplyEvent.EI_SetVacuum.hEvent);	// 위로 옮김
			/*WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_Set_LineGrab.hEvent, TIMEOUT_MAINPC_REPLY);
			nReturnVal = *(int*)&m_stMainPcReplyEvent.EI_Set_LineGrab.bParam[0];*/
			m_fnPrintLog(_T("SEQLOG -- ReqLINE_GRAB_FromInspPc <CASE 2> OK."));
			break;

		default:
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

			if (nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(_T("SEQLOG -- Req_Line_Grab_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- ReqVacuum_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_ReqMotionStatus_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CStringA	strMotionStatus;
	CString		strMotionName;
	TCHAR		cTemp[500] = {0,};
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- ReqMotionStatus_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				if (theApp.GetSimulationMode())
				{
					nRet = APP_OK;
					nStepNo = 9999;
					break;
				}

				// 상태값을 가져올 축명을 취득
				{
					memcpy(cTemp, pParamTemp, SIZE_MOTOR_TYPE_NAME);
					pParamTemp += SIZE_MOTOR_TYPE_NAME;

					strMotionName = cTemp;
				}
				if (strMotionName == _T("MOTOR_X") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_Y") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_T") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("MOTOR_Z") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("FOCUS_Z") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName == _T("ZOOM_Z") && m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bUse)
					throw MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
				else if (strMotionName != _T("MOTOR_X") && strMotionName != _T("MOTOR_Y") && strMotionName != _T("MOTOR_T") &&
					strMotionName != _T("MOTOR_Z") && strMotionName != _T("FOCUS_Z") && strMotionName != _T("ZOOM_Z"))
					throw MAINPCINTERFACE_TASK_SEQUENCE_PARAMETER_IS_WRONG;
				break;

			case 2:
				strSendMsg.Format("%s%s.", MAIN_PC_PACKET_MOTION_STATUS, (CStringA)strMotionName);

				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- MoveMotion_FromInspPc <CASE 2> Message Send Error."));
					throw nRet;
				}
				break;

			case 3:
				// Main PC 로 부터의 결과 회신을 대기한다. 축명, 상태정보. 회신
				if (strMotionName == _T("MOTOR_X"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam, 10);
				}
				else if (strMotionName == _T("MOTOR_Y"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam, 10);
				}
				else if (strMotionName == _T("MOTOR_T"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam, 10);
				}
				else if (strMotionName == _T("MOTOR_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam, 10);
				}
				else if (strMotionName == _T("FOCUS_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam, 10);
				}
				else if (strMotionName == _T("ZOOM_Z"))
				{
					ResetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.hEvent);
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bUse = true;
					WAIT_BIT_ON_EVENT(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.hEvent, TIMEOUT_MAINPC_REPLY);
					memcpy(pParam, m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam, 10);
				}
				break;

			case 4:
				if (strMotionName == _T("MOTOR_X"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam));
				}
				else if (strMotionName == _T("MOTOR_Y"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bParam));
				}
				else if (strMotionName == _T("MOTOR_T"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam));
				}
				else if (strMotionName == _T("MOTOR_Z"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam));
				}
				else if (strMotionName == _T("FOCUS_Z"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam));
				}
				else if (strMotionName == _T("ZOOM_Z"))
				{
					m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bUse = false;
					memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam));
				}
				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- ReqMotionStatus_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;

					// EQP BIT ALL OFF
					if (strMotionName == _T("MOTOR_X"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam));
					}
					else if (strMotionName == _T("MOTOR_Y"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bParam));
					}
					else if (strMotionName == _T("MOTOR_T"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam));				}
					else if (strMotionName == _T("MOTOR_Z"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam));
					}
					else if (strMotionName == _T("FOCUS_Z"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam));
					}
					else if (strMotionName == _T("ZOOM_Z"))
					{
						m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bUse = false;
						memset(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam, 0, sizeof(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam));
					}
				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- ReqMotionStatus_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Seq_SendMsg_ClassifyEnd_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CString		strVirtualID;
	CString		strCellID;
	TCHAR		cTemp[100] = {0,};
	CStringA	strSendMsg;
	
	CString		strTemp;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- SendMsg_ClassifyEnd_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				// Classify End 를 보낼 가상 ID, 패널 ID를 취득한다.
				{
					memcpy(cTemp, pParamTemp, SIZE_VIRTUAL_ID);
					pParamTemp += SIZE_VIRTUAL_ID;

					strVirtualID = cTemp;

					memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
					pParamTemp += SIZE_CELL_ID;
					strCellID = cTemp;

					//---------------------------------------------
					// 추가 2019.04.12 오병종
					//---------------------------------------------
					memcpy(cTemp, pParamTemp, SIZE_JUDGE);
					pParamTemp += SIZE_JUDGE;
					m_strJudge = cTemp;
					//---------------------------------------------

				}
				m_fnPrintLog(_T("CLASSIFY END - 1 : Virtual ID = %s, Cell ID = %s, Judge = %s"), strVirtualID, strCellID, m_strJudge);
				break;

			case 2:

				// 2019.06.07 MDJ 재시작 시퀀스 대응
				for (int nCnt = 0; nCnt < m_nGrabCount + 1; nCnt++)
				{
					if (m_strGrabCellID[nCnt].Compare(strCellID) == 0)
					{
						m_strGrabCellID[nCnt] = _T("");
						break;
					}
				}

				int i;
				int nLoopCount;
				EnterCriticalSection(&m_stInspPcEvent.m_csLock);

				for (i = 0; i < 100; i++)
				{
					// Grab End Flag 추가 21.12.22 YWS
					strTemp = (TCHAR*)(m_stInspPcEvent.EI_Grab_End[i].bParam);
					if (strCellID.Find(strTemp) >= 0)	// 현재 ClassifyEnd 들어온 Cell과 같은 Cell이면
					{
						nLoopCount = 0;
						while (m_stInspPcEvent.EI_Grab_End[i].bUse)	// 해당 Cell Grab_End Flag가 false가 될 때까지 대기한다.
						{
							nLoopCount++;
							Sleep(1);
							if (nLoopCount > 30000)	// 최대 30초 대기, 무한루프 방지
							{
								m_stInspPcEvent.EI_Grab_End[i].bUse = false;
								break;
							}
						}
					}

					if (nLoopCount > 30000)
					{
						m_fnPrintLog(_T("Grab End Flag Count Over !!! = %d "), nLoopCount);
					}

					// Panel ID ""도 포함된 것으로 판단하여 CLASS.END.ERR 발생 - bUse Flag 사용하도록 수정
					if (m_stInspPcEvent.EI_Classify_End[i].bUse)
					{
						strTemp = (TCHAR*)(m_stInspPcEvent.EI_Classify_End[i].bParam);
						if (strCellID.Find(strTemp) >= 0) // strTemp.Find(strCellID) >= 0 -> strCellID.Find(strTemp) >= 0 - 2017.10.06 edit by ikm 
						{
							m_stInspPcEvent.EI_Classify_End[i].bUse = false;
							memset(m_stInspPcEvent.EI_Classify_End[i].bParam, 0, sizeof(m_stInspPcEvent.EI_Classify_End[i].bParam));
							SetEvent(m_stInspPcEvent.EI_Classify_End[i].hEvent);
							m_fnPrintLog(_T("CLASSIFY END - 2 : Cell ID = %s, EventIndex = %d "), strCellID, i);
							break;
						}
					}

				}
				LeaveCriticalSection(&m_stInspPcEvent.m_csLock);

				if (i >= 100)
				{
					m_fnPrintLog(_T("SEQLOG -- SendMsg_ClassifyEnd_FromInspPc - No Wait Event. Cell ID = \n"), strCellID);
				}
				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

			if ( nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(_T("SEQLOG -- SendMsg_ClassifyEnd_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept =APP_OK;

				// EQP BIT ALL OFF

			}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- SendMsg_ClassifyEnd_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::Wait_Event_ClassifyEnd(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CString		strVirtualID;
	CString		strCellID;
	TCHAR		cTemp[100] = {0,};
	CStringA	strSendMsg;
	int			stEventIndex = 0;	

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- Wait_Event_ClassifyEnd StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				// Classify End 를 대기할 가상 ID, 패널 ID를 취득한다.
				{
// 					memcpy(cTemp, pParamTemp, SIZE_VIRTUAL_ID);
// 					pParamTemp += SIZE_VIRTUAL_ID;
// 
// 					strVirtualID = cTemp;
// 
// 					memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
// 					strCellID = cTemp;
					memcpy(cTemp, pParamTemp, SIZE_CELL_ID);
					pParamTemp += SIZE_CELL_ID;
					strCellID = cTemp;

					memcpy(cTemp, pParamTemp, SIZE_VIRTUAL_ID);
					pParamTemp += SIZE_VIRTUAL_ID;
					strVirtualID = cTemp;
				}

				m_fnPrintLog(_T("CASE 1"));
				break;

			case 2:
				// m_stInspPcEvent 의 파라메터에 가상ID, 패널ID를 설정하고 대기한다. 
			{
				EnterCriticalSection(&m_stInspPcEvent.m_csLock);

				for (stEventIndex = 0; stEventIndex < 100; stEventIndex++)
				{
					if (m_stInspPcEvent.EI_Classify_End[stEventIndex].bUse == false)
					{
						m_stInspPcEvent.EI_Classify_End[stEventIndex].bUse = true;
						memset(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam, 0, sizeof(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam));
						_tcscpy((TCHAR*)m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam, strCellID);

						m_stInspPcEvent.EI_Grab_End[stEventIndex].bUse = false;	// Grab End Flag 추가 21.12.22 YWS

						m_fnPrintLog(_T("SEQLOG -- Wait_Event_Classify - Cell ID = %s, EventIndex = %d"), strCellID, stEventIndex);
						break;
					}
				}
				LeaveCriticalSection(&m_stInspPcEvent.m_csLock);
				WAIT_BIT_ON_EVENT(m_stInspPcEvent.EI_Classify_End[stEventIndex].hEvent, TIMEOUT_CLASSIFY_END);
				ResetEvent(m_stInspPcEvent.EI_Classify_End[stEventIndex].hEvent);

				// 2019.06.11 MDJ Grab Count 사용여부 추가
				// KYS 그랩 개수 제한 세마포어 1개 해제
				if (m_bGrabCount)
				{
					ReleaseSemaphore(m_hGrabSemaphore, 1, NULL);
				}
			}
				break;

			case 3:
				// Main PC로 메시지 전송.
				strSendMsg.Format("%sOK.%s.%s.", MAIN_PC_PACKET_CLASSIFY_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- MAIN_PC_PACKET_CLASSIFY_END_REPLY Message Send Error."));
					throw nRet;
				}

				break;

			case 4: // 추가 2019.04.12 오병종
				// Main PC로 메시지 전송.
				strSendMsg.Format("%sOK.%s.%s.%s.", MAIN_PC_PACKET_INSPECT_JUDGED_REPLY, (CStringA)m_strJudge, (CStringA)strVirtualID, (CStringA)strCellID);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- MAIN_PC_PACKET_INSPECT_JUDGED_REPLY Message Send Error."));
					throw nRet;
				}

				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

			if ( nRet != APP_OK)
			{
				// MAIN_PC_PACKET_CLASSIFY_END_REPLY 일 경우
				if (nStepNo == 3) // 추가 2019.04.12 오병종
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- Wait_Event_ClassifyEnd Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept = APP_OK;

					// 
					EnterCriticalSection(&m_stInspPcEvent.m_csLock);
					m_stInspPcEvent.EI_Classify_End[stEventIndex].bUse = false;
					memset(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam, 0, sizeof(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam));
					LeaveCriticalSection(&m_stInspPcEvent.m_csLock);

					strSendMsg.Format("%sERR.%s.%s.", MAIN_PC_PACKET_CLASSIFY_END_REPLY, (CStringA)strVirtualID, (CStringA)strCellID);
					nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
					if (nRet != APP_OK)
					{
						m_fnPrintLog(_T("SEQLOG -- MAIN_PC_PACKET_CLASSIFY_END_REPLY Message Send Error."));
						throw nRet;
					}
				}
				else
				{
					// MAIN_PC_PACKET_INSPECT_JUDGED_REPLY 일 경우
					// 추가 2019.04.12 오병종
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- Wait_Event_ClassifyEnd Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept = APP_OK;

					// 2020.07.31 MDJ Classify End Time Out 발생시 Cell 이름 지우는 루틴 추가
					for (int nCnt = 0; nCnt < m_nGrabCount + 1; nCnt++)
					{
						if (m_strGrabCellID[nCnt].Compare(strCellID) == 0)
						{
							m_strGrabCellID[nCnt] = _T("");
							//////////////////// 이거 추가해야하는거 아님? 
							if (m_bGrabCount)
							{
								ReleaseSemaphore(m_hGrabSemaphore, 1, NULL);
							}
							////////////////////
							break;
						}
					}

					// 
					EnterCriticalSection(&m_stInspPcEvent.m_csLock);
					m_stInspPcEvent.EI_Classify_End[stEventIndex].bUse = false;
					memset(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam, 0, sizeof(m_stInspPcEvent.EI_Classify_End[stEventIndex].bParam));
					// Grab End Flag 추가 21.12.22 YWS
					m_stInspPcEvent.EI_Grab_End[stEventIndex].bUse = false;
					memset(m_stInspPcEvent.EI_Grab_End[stEventIndex].bParam, 0, sizeof(m_stInspPcEvent.EI_Grab_End[stEventIndex].bParam));

					LeaveCriticalSection(&m_stInspPcEvent.m_csLock);

					strSendMsg.Format("%sERR.%s.%s.%s.", MAIN_PC_PACKET_INSPECT_JUDGED_REPLY, (CStringA)m_strJudge, (CStringA)strVirtualID, (CStringA)strCellID);
					nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
					if (nRet != APP_OK)
					{
						m_fnPrintLog(_T("SEQLOG -- MAIN_PC_PACKET_INSPECT_JUDGED_REPLY Message Send Error."));
						throw nRet;
					}
				}


			}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Wait_Event_ClassifyEnd Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}


int	WorkManager::m_fnStartServer(int nServerPort, int &nServerSocket)
{
	TCHAR szTemp[100] = {0,};
	struct sockaddr_in server_addr,client_addr;
	int nLen = sizeof(client_addr);

	if (nServerSocket)
	{
		closesocket(nServerSocket);
		nServerSocket = 0;
	}

	if((nServerSocket = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		m_fnPrintLog(_T("Server: can not Open Socket\n"));
		exit(0);
	}

	memset(&server_addr,0x00,sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(nServerPort);

	if(bind(nServerSocket, (struct sockaddr *)&server_addr, sizeof(server_addr))< 0)
	{
		m_fnPrintLog(_T("Server: cat not bind local addrss\n"));
		exit(0);
	}
	if(listen(nServerSocket,5) < 0)
	{
		m_fnPrintLog(_T("Server: cat not listen connnect.\n"));
		exit(0);
	}

	m_skMainPcSocket = accept(nServerSocket,(struct sockaddr *)&client_addr,(socklen_t *)&nLen);

	if(m_skMainPcSocket < 0)
	{
		m_fnPrintLog(_T("Server: accept failed. Error = %d\n"),m_skMainPcSocket);

		closesocket(nServerSocket);
		nServerSocket = 0;
		m_skMainPcSocket = 0;
		return APP_NG;
	}

	InetNtop(AF_INET,&client_addr.sin_addr.s_addr,szTemp,sizeof(szTemp));
	m_fnPrintLog(_T("Server: %s client connected.\n"),szTemp);

	return APP_OK;
}


int	WorkManager::m_fnConnectServer(int nServerPort, TCHAR* szServerIP, int &nMainPcSocket)
{
	struct sockaddr_in client_addr;

	if (nMainPcSocket)
	{
		closesocket(nMainPcSocket);
		nMainPcSocket = 0;
	}
	
	nMainPcSocket = socket(PF_INET,SOCK_STREAM,0);

#ifdef UNICODE
	char IP[16];
	int Ret = 0;
	Ret = WideCharToMultiByte(CP_ACP, 0, szServerIP, _tcslen(szServerIP), IP, 15, NULL, NULL);
	IP[Ret] = 0;

	client_addr.sin_addr.s_addr = inet_addr(IP);
#endif

#ifndef UNICODE
	client_addr.sin_addr.s_addr = inet_addr(szServerIP);
#endif

	
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(nServerPort);

	if(connect(nMainPcSocket,(struct sockaddr *)&client_addr,sizeof(client_addr))== -1)
	{
		//m_fnPrintLog(_T("Can't connect..\n"));
		closesocket(nMainPcSocket);
		nMainPcSocket = 0;
		return APP_NG;
	}

	return APP_OK;
}


int	WorkManager::m_fnSendMessageToMainPc(char* szMessage, int nMsgSize)
{
	EnterCriticalSection(&m_csMainPcInterface);

	if (m_skMainPcSocket == NULL)
	{
		LeaveCriticalSection(&m_csMainPcInterface);
		return MAINPCINTERFACE_TASK_MAIN_PC_NOT_CONNECTED;
	}

	char *bpSendBuffer	= new char[nMsgSize + 100];
	char *szSendData	= new char[nMsgSize + 1];			// +1을 한 이유는 혹시 모를 문자열 복사시 메모리 참조 에러 때문임
	char *pBuffer		= NULL;

	memset(bpSendBuffer, 0, nMsgSize + 100);
	memset(szSendData, 0, nMsgSize+1);
	memcpy_s(szSendData, nMsgSize, szMessage, nMsgSize);

	pBuffer = bpSendBuffer;
	
	*(byte*)pBuffer = MAIN_PC_PACKET_STX;
	pBuffer += MAIN_PC_PACKET_STX_SIZE;

	*(byte*)pBuffer = MAIN_PC_PACKET_HEADER;
	pBuffer += MAIN_PC_PACKET_HEADER_SIZE;
	
	sprintf(pBuffer,"%04d",nMsgSize);
	pBuffer += MAIN_PC_PACKET_DATA_SIZE_INFO_SIZE;

	sprintf(pBuffer,"%s",szSendData);
	pBuffer += nMsgSize;

	*(byte*)pBuffer = MAIN_PC_PACKET_ETX;
	pBuffer += MAIN_PC_PACKET_ETX_SIZE;

	int						nRet			= 0;
	int						nSendOffset		= 0;
	int						nErrorCode		= OKAY;
	int						nLen			= pBuffer - bpSendBuffer;

	while(1)
	{
		//Server 로 Packet 전송을 한다.
		nRet = send(m_skMainPcSocket, bpSendBuffer + nSendOffset, nLen - nSendOffset, 0);

		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			m_fnPrintLog(("Packet Send error : %s\n"), bpSendBuffer);

			nErrorCode = WSAGetLastError();

			//이상 종료 한다.
			m_fnPrintLog("m_fnSendMessageToMainPc -> send error : %d\n", nErrorCode);

			if (m_skMainPcSocket)
			{
				closesocket(m_skMainPcSocket);
				m_skMainPcSocket = NULL;
			}

			SAFE_DELETE_ARR(bpSendBuffer);
			SAFE_DELETE_ARR(szSendData);
			LeaveCriticalSection(&m_csMainPcInterface);
			return MAINPCINTERFACE_TASK_SEND_ERROR_TO_MAIN_PC;
		}	

		//만일 송신한 Packet 이 실제 필요한 전체 Packet 크기 보다 작다면
		if(nLen > nSendOffset + nRet)
		{	
			TRACE("m_fnSendMessageToMainPc -> uSendSize > nSendOffset + nRet");

			//총 송신 크기를 저장한다.
			nSendOffset += nRet;
			//다시 송신 시도한다.
			continue;
		}
		//만일 송신한 Pacekt 이 실제 필요한 만큼 전송 되었다면
		//구간 반복을 빠져나온다.
		break;
	}	

	m_fnPrintLog(("Packet Sended : %s\n"), bpSendBuffer);

	SAFE_DELETE_ARR(bpSendBuffer);
	SAFE_DELETE_ARR(szSendData);

	LeaveCriticalSection(&m_csMainPcInterface);
	return APP_OK;
}


int WorkManager::m_fnReceiveMessage(BYTE* buffer, int nBuffSize)
{
	const int				RECEIVE_BUFFER_SIZE = 1024;
	int						nRet			= 0;
	static int				nReceiveOffset	= 0;
	static char				bRcvBuffer[RECEIVE_BUFFER_SIZE] = {0,};
	int						nErrorCode		= OKAY;
	int						nRcvBufferSize	= sizeof(bRcvBuffer);

	while(1)
	{

		if(bRcvBuffer[MAIN_PC_PACKET_STX_POS] == MAIN_PC_PACKET_STX && bRcvBuffer[MAIN_PC_PACKET_HEADER_POS] == MAIN_PC_PACKET_HEADER 
			&& nReceiveOffset > MAIN_PC_PACKET_TOTAL )
		{
			//STX 가 존재하고 전체 사이즈 정보를 받았다.
			char	szDataSize[MAIN_PC_PACKET_DATA_SIZE_INFO_SIZE + 1]	= {0,};
			int		nDataSize		= 0;

			memcpy(szDataSize, &bRcvBuffer[MAIN_PC_PACKET_DATA_SIZE_INFO_POS], MAIN_PC_PACKET_DATA_SIZE_INFO_SIZE);
			nDataSize = atoi(szDataSize);
			
			
			if (nReceiveOffset >= nDataSize + MAIN_PC_PACKET_TOTAL)
			{
				//받은 패킷이 1개의 커맨드 만큼 받았다.

				if (bRcvBuffer[nDataSize + MAIN_PC_PACKET_TOTAL - 1] == MAIN_PC_PACKET_ETX)
				{
					// 받은 패킷의 끝이 ETX 로 정상적으로 끝났다.

					// 1개의 패킷을 완성한다.
					memcpy_s(buffer, nBuffSize, bRcvBuffer, nDataSize + MAIN_PC_PACKET_TOTAL);

					// 뒤에 남는 패킷은 앞으로 끌어 당긴다.
					memcpy_s(bRcvBuffer, RECEIVE_BUFFER_SIZE, &bRcvBuffer[nDataSize + MAIN_PC_PACKET_TOTAL], nReceiveOffset - nDataSize - MAIN_PC_PACKET_TOTAL);

					// 앞으로 당긴 후 뒷부분은 0으로 초기화 한다.
					memset(&bRcvBuffer[nReceiveOffset - nDataSize - MAIN_PC_PACKET_TOTAL], 0, RECEIVE_BUFFER_SIZE - (nReceiveOffset - nDataSize - MAIN_PC_PACKET_TOTAL));

					nReceiveOffset = nReceiveOffset - nDataSize - MAIN_PC_PACKET_TOTAL;

					m_fnPrintLog(("Packet Received : %s\n"), buffer);

					return APP_OK;
				}
				else
				{
					// 끝이 ETX 로 끝나지 않는다. 이상 패킷.. 패킷을 지우고 소켓을 끊는다.
					nReceiveOffset = 0;
					memset(bRcvBuffer, 0, RECEIVE_BUFFER_SIZE);

					if (m_skMainPcSocket != NULL)
					{
						closesocket(m_skMainPcSocket);
						m_skMainPcSocket = NULL;
					}
					m_fnPrintLog(_T("m_fnReceiveMessage -> ETX error\n"));

					return APP_NG;
				}
			}

		}


		//Server 로부터 Packet 을 버퍼로 수신한다.
		nRet = recv(m_skMainPcSocket, &bRcvBuffer[nReceiveOffset], nRcvBufferSize - nReceiveOffset, 0);

		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			nErrorCode = WSAGetLastError();
			//이상 종료 한다.
			m_fnPrintLog(_T("m_fnReceiveMessage -> send error : %d\n"), nErrorCode);

			if (m_skMainPcSocket != NULL)
			{
				closesocket(m_skMainPcSocket);
				m_skMainPcSocket = NULL;
			}

			return MAINPCINTERFACE_TASK_RCV_ERROR_TO_MAIN_PC;
		}
		else if(0 == nRet)
		{
			nErrorCode = WSAGetLastError();
			m_fnPrintLog(_T("m_fnReceiveMessage -> recv size is 0.. Server disconnect.. error : %d \n"), nErrorCode);

			if (m_skMainPcSocket != NULL)
			{
				closesocket(m_skMainPcSocket);
				m_skMainPcSocket = NULL;
			}

			return MAINPCINTERFACE_TASK_RCV_ERROR_TO_MAIN_PC;
		}

		nReceiveOffset += nRet;
		
	}

	return nErrorCode;
}


int WorkManager::m_fnAnalyzeMainPcCmd(char* buffer, int nBuffSize)
{
	int nRet = APP_OK;
	byte bParam[500] = {0,};
	CStringA stRcvPacket = &buffer[MAIN_PC_PACKET_DATA_POS];

	

	if(stRcvPacket.Find(MAIN_PC_PACKET_GRAB_START) != -1)
	{
		// 17.07.14 Async Flag 추가
		int nStartLine = 0, nStartPos = 0, nEndPos = 0, nTotalLine = 0;
		CString strCellId, strVirtualID, strLineCount, strDirection, strPosition;
		
		// MergeTool 촬영위치, 검사 Step 추가 
		CString strGrabPos, strInspStep;
		int nGrabPos = 0, nInspStep = 0;

		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_GRAB_START, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strVirtualID = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strCellId       = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);
		
		nStartPos = nEndPos + 1;;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strLineCount = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strDirection = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strPosition = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		//*(int*)pParamTemp	= nStartLine;
		//pParamTemp += sizeof(int);
		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strVirtualID, strVirtualID.GetLength());
		pParamTemp += SIZE_CELL_ID;

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strCellId, strCellId.GetLength());
		pParamTemp += SIZE_CELL_ID;

		*(int*)pParamTemp = _ttoi(strLineCount);
		pParamTemp += sizeof(int);

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strDirection, strDirection.GetLength());
		pParamTemp += SIZE_CELL_ID;

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strPosition, strPosition.GetLength());
		pParamTemp += SIZE_CELL_ID;


		nRet = CmdEditSend(START_GRAB_REQ_FROM_MAIN_PC, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_NORES);
		
		if (nRet != APP_OK)
		{
			m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <START_GRAB_REQ_FROM_MAIN_PC> command send Error = %d \n"), nRet);
			return nRet;
		}
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_AF_READY) != -1)
	{
		// 17.07.14 Async Flag 추가
		int nStartLine = 0, nStartPos = 0, nEndPos = 0, nTotalLine = 0;
		CString strCellId, strVirtualID, strPos, strArea;

		// MergeTool 촬영위치, 검사 Step 추가 
		CString strGrabPos, strInspStep;
		int nGrabPos = 0, nInspStep = 0;

		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_AF_READY, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strVirtualID = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strCellId = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strPos = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strArea = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strVirtualID, strVirtualID.GetLength());
		pParamTemp += SIZE_CELL_ID;

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strCellId, strCellId.GetLength());
		pParamTemp += SIZE_CELL_ID;

		*(int*)pParamTemp = _ttoi(strPos);
		pParamTemp += sizeof(int);

		*(int*)pParamTemp = _ttoi(strArea);
		pParamTemp += sizeof(int);

		nRet = CmdEditSend(AF_READY, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_NORES);

		if (nRet != APP_OK)
		{
			m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <START_GRAB_REQ_FROM_MAIN_PC> command send Error = %d \n"), nRet);
			return nRet;
		}
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_GRAB_READY) != -1)
	{
		// 17.07.14 Async Flag 추가
		int nStartLine = 0, nStartPos = 0, nEndPos = 0, nTotalLine = 0;
		CString strCellId, strVirtualID, strPos;

		// MergeTool 촬영위치, 검사 Step 추가 
		CString strGrabPos, strInspStep;
		int nGrabPos = 0, nInspStep = 0;

		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_GRAB_READY, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strVirtualID = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strCellId = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strVirtualID, strVirtualID.GetLength());
		pParamTemp += SIZE_CELL_ID;

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strCellId, strCellId.GetLength());
		pParamTemp += SIZE_CELL_ID;

		*(int*)pParamTemp = _ttoi(strPos);
		pParamTemp += sizeof(int);

		nRet = CmdEditSend(GRAB_END, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_NORES);

		if (nRet != APP_OK)
		{
			m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <START_GRAB_REQ_FROM_MAIN_PC> command send Error = %d \n"), nRet);
			return nRet;
		}
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_ISNP_GRAB_START) != -1)
	{
		int nStartPos = 0, nEndPos = 0;
				
		CString strCellId, strStartLine, strTotalLine, strDirection;
		byte* pParamTemp = bParam;
		
		stRcvPacket.Replace(MAIN_PC_PACKET_ISNP_GRAB_START, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strCellId = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strStartLine = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strTotalLine = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);


		m_nStartLine = _ttoi(strStartLine);
		m_nTotalLine = _ttoi(strTotalLine);

		m_strCellId = strCellId;

		*(int*)pParamTemp = m_nStartLine;
		pParamTemp += sizeof(int);

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strCellId, strCellId.GetLength());
		pParamTemp += SIZE_CELL_ID;

		*(int*)pParamTemp = m_nTotalLine;
		pParamTemp += sizeof(int);
	}

	else if (stRcvPacket.Find(MAIN_PC_PACKET_RECIPE_CHANGE) != -1)
	{
		int nStartPos = 0, nEndPos = 0;
		CString strRecipeID;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_RECIPE_CHANGE, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strRecipeID		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strRecipeID, strRecipeID.GetLength()); 
		//memcpy(pParamTemp, (LPCSTR)strRecipeID, strRecipeID.GetLength());
		pParamTemp += SIZE_RECIPE_ID;
		

		nRet = CmdEditSend(RECIPE_CHANGE_REQ_FROM_MAIN_PC, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_NORES);
		if (nRet != APP_OK)
		{
			m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <START_GRAB_REQ_FROM_MAIN_PC> command send Error = %d \n"), nRet);
			return nRet;
		}
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_SYSTEM_TIME_SET) != -1)
	{
		int nStartPos = 0, nEndPos = 0;
		CString strRecipeID;
		byte* pParamTemp = bParam;

		SYSTEMTIME localtime;
		CString strYear, strMonth, strDay, strHour, strMinute, strSecond, strMilliSecond;

		GetLocalTime(&localtime);

		stRcvPacket.Replace(MAIN_PC_PACKET_SYSTEM_TIME_SET, "");

		nStartPos = 0;			nEndPos = stRcvPacket.Find("-");
		strYear			= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find("-", nStartPos);
		strMonth		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find("-", nStartPos);
		strDay			= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find("-", nStartPos);
		strHour			= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find("-", nStartPos);
		strMinute		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find("-", nStartPos);
		strSecond		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;	nEndPos	= stRcvPacket.Find(".", nStartPos);
		strMilliSecond	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		localtime.wYear			= _ttoi(strYear);
		localtime.wMonth		= _ttoi(strMonth);
		localtime.wDay			= _ttoi(strDay);
		localtime.wHour			= _ttoi(strHour);
		localtime.wMinute		= _ttoi(strMinute);
		localtime.wSecond		= _ttoi(strSecond);
		localtime.wMilliseconds = _ttoi(strMilliSecond);

		SetLocalTime(&localtime);

		m_fnPrintLog(_T("Rcv Local Time = %04d-%02d-%02d %02d:%02d:%02d.%03d.\n"),
			localtime.wYear, localtime.wMonth, localtime.wDay, localtime.wHour, localtime.wMinute, localtime.wSecond, localtime.wMilliseconds);
		m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <MAIN_PC_PACKET_SYSTEM_TIME_SET> SetLocalTime OK.\n"));
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_PG_CHANGE_PARRERN) != -1)
	{
		// "MOT.PATTERN.CHANGE.OK" or "MOT.PATTERN.CHANGE.ERR"			180801 YSS
		m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <MAIN_PC_PACKET_PG_CHANGE_PARRERN> %s\n"), stRcvPacket);
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_PG_OFF) != -1)
	{
		// "MOT.PATTERN.OFF.OK" or "MOT.PATTERN.OFF.ERR"				180801 YSS
		m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <MAIN_PC_PACKET_PG_OFF> %s\n"), stRcvPacket);
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_PG_ON) != -1)
	{
		// "MOT.PATTERN.ON.OK" or "MOT.PATTERN.ON.ERR"				180822 YSS
		m_fnPrintLog(_T("m_fnAnalyzeMainPcCmd: <MAIN_PC_PACKET_PG_ON> %s\n"), stRcvPacket);
	}
	
	return nRet;
}

int WorkManager::m_fnWakeUpSequence(char* buffer, int nBuffSize)
{
	int nRet = APP_OK;
	byte bParam[500] = {0,};
	CStringA stRcvPacket = &buffer[MAIN_PC_PACKET_DATA_POS];



	if(stRcvPacket.Find(MAIN_PC_PACKET_RECIPE_CURRENT_ID_REPLY) != -1)
	{
		int nStartPos = 0, nEndPos = 0;
		CString strRecipeID;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_RECIPE_CURRENT_ID_REPLY, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strRecipeID		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);
		
		_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_CurrentRecipe.bParam, (LPCTSTR)strRecipeID, strRecipeID.GetLength()); 
		//memcpy(m_stMainPcReplyEvent.EI_CurrentRecipe.bParam, strRecipeID, strRecipeID.GetLength());
		SetEvent(m_stMainPcReplyEvent.EI_CurrentRecipe.hEvent);

	}
	else if(stRcvPacket.Find(MAIN_PC_PACKET_MOTION_RECIPE_POS_REPLY) != -1)
	{
		int nStartPos = 0, nEndPos = 0, nRetVal = 0;
		CString strRecipeID, strReturnValue;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_MOTION_RECIPE_POS_REPLY, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strRecipeID		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strReturnValue	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		if (strReturnValue == _T("OK"))
			nRetVal = APP_OK;
		else
			nRetVal = APP_NG;

		*(int*)m_stMainPcReplyEvent.EI_SetRecipePos.bParam = nRetVal;
		SetEvent(m_stMainPcReplyEvent.EI_SetRecipePos.hEvent);

	}
	else if(stRcvPacket.Find(MAIN_PC_PACKET_VACUUM_SWITCH_REPLY) != -1)
	{
		int nStartPos = 0, nEndPos = 0, nRetVal = 0;
		CString strVacuumType, strReturnValue;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_VACUUM_SWITCH_REPLY, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strVacuumType	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strReturnValue	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		if (strReturnValue == _T("OK"))
			nRetVal = 0;
		else if (strReturnValue == _T("PAUSE"))
			nRetVal = 1;
		else if (strReturnValue == _T("RESUME"))
			nRetVal = 2;

		*(int*)m_stMainPcReplyEvent.EI_SetVacuum.bParam = nRetVal;
		SetEvent(m_stMainPcReplyEvent.EI_SetVacuum.hEvent);

	}

	else if (stRcvPacket.Find(MAIN_PC_PACKET_STAGEMOVE_REPLY) != -1)
	{
		int nStartPos = 0, nEndPos = 0;
		int nRetVal = APP_OK;
		CString strResult, strDirection;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_STAGEMOVE_REPLY, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strResult = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strDirection = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		if (strResult == _T("OK"))
			nRetVal = APP_OK;
		else
			nRetVal = APP_NG;

		if (strDirection == _T("F"))
		{
			*(int*)m_stMainPcReplyEvent.EI_STAGEMOVE_FORWARD.bParam = nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_STAGEMOVE_FORWARD.hEvent);
		}
		else
		{
			*(int*)m_stMainPcReplyEvent.EI_STAGEMOVE_BACKWARD.bParam = nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_STAGEMOVE_BACKWARD.hEvent);
		}
	}

	else if (stRcvPacket.Find(MAIN_PC_PACKET_LINE_GRAB_REPLY) != -1)
	{
		int nStartPos = 0, nEndPos = 0, nRetVal = 0;
		CString strLineGrab, strReturnValue;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_LINE_GRAB_REPLY, "");

		nStartPos = 0;
		nEndPos = stRcvPacket.Find(".");
		strLineGrab = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos = nEndPos + 1;
		nEndPos = stRcvPacket.Find(".", nStartPos);
		strReturnValue = stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		if (strReturnValue == _T("OK"))
			nRetVal = APP_OK;
		else
			nRetVal = APP_NG;

		*(int*)m_stMainPcReplyEvent.EI_Set_LineGrab.bParam = nRetVal;
		*(int*)m_stMainPcReplyEvent.EI_Set_LineGrab.bParam = nRetVal;
		SetEvent(m_stMainPcReplyEvent.EI_Set_LineGrab.hEvent);

	}

	else if (stRcvPacket.Find(MAIN_PC_PACKET_MOTION_RELATIVE_MOVE_REPLY) != -1)
	{
		int nRelativeMovedValue = 0, nStartPos = 0, nEndPos = 0, nRetVal = 0;
		CString strMotionName, strSign, strMovedValue, strReturnValue;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_MOTION_RELATIVE_MOVE_REPLY, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strMotionName	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		//nStartPos		= nEndPos + 1;
		//nEndPos		= stRcvPacket.Find(".", nStartPos);
		//strSign		= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strMovedValue	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strReturnValue	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nRelativeMovedValue	= _ttoi(strMovedValue);
		//if (strSign == "-")
		//{
		//	nRelativeMovedValue = nRelativeMovedValue * -1;
		//}

		//KYH 2021.02.25 6 Zone의 THETA값을 모두 구한 후 UI Theta Off 
		if (strReturnValue == _T("OK"))
		{
			m_ThetaCnt++;

			if (m_ThetaCnt == 6)
			{
				nRetVal = APP_OK;

				byte bPCNO[8] = { 0, };
				int nOff = 0;
				byte* pPCNoTemp = bPCNO;

				*(int*)pPCNoTemp = theApp.GetPCNum() - 1;
				pPCNoTemp += sizeof(int);
				*(int*)pPCNoTemp = nOff;
				pPCNoTemp += sizeof(int);

				nRet = CmdEditSend(SEND_THETA_ON_OFF, 0, sizeof(bPCNO), VS_UI_TASK, bPCNO, CMD_TYPE_RES);
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("%s : <THETA OFF ERR> command send Error = %d \n"), __WFUNCTION__, nRet);
				}

				m_ThetaCnt = 0;
			}

		}
		else
			nRetVal = APP_NG;

		if (strMotionName == _T("MOTOR_X"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam				= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_X.hEvent);
		}
		else if (strMotionName == _T("MOTOR_Y"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam				= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Y.hEvent);
		}
		else if (strMotionName == _T("MOTOR_T"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam				= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_T.hEvent);
		}
		else if (strMotionName == _T("MOTOR_Z"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam				= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Motor_Z.hEvent);
		}
		else if (strMotionName == _T("FOCUS_Z"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam				= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Focus_Z.hEvent);
		}
		else if (strMotionName == _T("ZOOM_Z"))
		{
			*(int*)m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam					= nRelativeMovedValue;
			*(int*)&m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.bParam[sizeof(int)]	= nRetVal;
			SetEvent(m_stMainPcReplyEvent.EI_MoveMotion_Zoom_Z.hEvent);
		}

	
		
	}
	else if (stRcvPacket.Find(MAIN_PC_PACKET_MOTION_STATUS_REPLY) != -1)
	{
		int nRelativeMovedValue = 0, nStartPos = 0, nEndPos = 0, nRetVal = 0;
		CString strMotionName, strSign, strMovedValue, strReturnValue;
		byte* pParamTemp = bParam;

		stRcvPacket.Replace(MAIN_PC_PACKET_MOTION_STATUS_REPLY, "");

		nStartPos		= 0;
		nEndPos			= stRcvPacket.Find(".");
		strMotionName	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		nStartPos		= nEndPos + 1;
		nEndPos			= stRcvPacket.Find(".", nStartPos);
		strReturnValue	= stRcvPacket.Mid(nStartPos, nEndPos - nStartPos);

		if (strMotionName == _T("MOTOR_X"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_X.hEvent);
		}
		else if (strMotionName == _T("MOTOR_Y"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Y.hEvent);
		}
		else if (strMotionName == _T("MOTOR_T"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_T.hEvent);
		}
		else if (strMotionName == _T("MOTOR_Z"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Motor_Z.hEvent);
		}
		else if (strMotionName == _T("FOCUS_Z"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Focus_Z.hEvent);
		}
		else if (strMotionName == _T("ZOOM_Z"))
		{
			_tcsncpy((TCHAR*)m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.bParam, strReturnValue, strReturnValue.GetLength()); 
			SetEvent(m_stMainPcReplyEvent.EI_ReplyMotionStaus_Zoom_Z.hEvent);
		}

	}


	return nRet;
}







int	WorkManager::Seq_StartSimulation(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;
	//CString strFuncName = __WFUNCTION__;

	ST_SIMULATION_INFO stSimulationInfo;
	int	nCurrentRepeatCnt = 0;
	int nCurrentCellNo = 0;
	

	do
	{
		EXCEPTION_TRY

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG -- %s StepNo=%d, RetVal=%d \n"), __WFUNCTION__, nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			memcpy(&stSimulationInfo, pParam, sizeof(ST_SIMULATION_INFO));
			break;

		case 2:
			nCurrentRepeatCnt++;
			nCurrentCellNo = 0;
			

			if (stSimulationInfo.nRepeatCnt != 0 && nCurrentRepeatCnt > stSimulationInfo.nRepeatCnt)
			{
				nStepNo = 999;
				break;
			}
			m_fnPrintLog(_T("SEQLOG -- %s Simulation Repeat Count = %d \n"), __WFUNCTION__, nCurrentRepeatCnt);
			break;

		case 3:
			nCurrentCellNo++;

			if (nCurrentCellNo > stSimulationInfo.nTotalCellCount)
			{
				nStepNo = 1;	//Goto Step 2
				break;
			}

			m_fnPrintLog(_T("SEQLOG -- %s Current Cell No = %d, Cell ID = %s \n"), __WFUNCTION__, nCurrentCellNo, 
				stSimulationInfo.szCellId[nCurrentCellNo-1]);

			{
				byte bParam[500] = {0,};
				int nStageNo = 0, nStartPos = 0, nEndPos = 0;
				CString strVirtualId, strCellId;
				byte* pParamTemp = bParam;

				strVirtualId	= stSimulationInfo.szCellId[nCurrentCellNo-1];
				strCellId		= stSimulationInfo.szCellId[nCurrentCellNo-1];
				nStageNo		= 0;

				*(int*)pParamTemp	= nStageNo;
				pParamTemp += sizeof(int);
				_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strVirtualId, strVirtualId.GetLength()); 
				//memcpy(pParamTemp, (LPCSTR)strVirtualId, strVirtualId.GetLength());
				pParamTemp += SIZE_VIRTUAL_ID;
				_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strCellId, strCellId.GetLength()); 
				//memcpy(pParamTemp, (LPCSTR)strCellId, strCellId.GetLength());

				nRet = CmdEditSend(START_GRAB_REQ_FROM_MAIN_PC, 0, sizeof(bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES, 5000);
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("%s : <START_GRAB_REQ_FROM_MAIN_PC> command send Error = %d \n"), __WFUNCTION__, nRet);
					throw nRet;
				}
			}

			

			break;
		case 4:
			Sleep(stSimulationInfo.nDelay);
			nStepNo = 2;
			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- %s Error Occured. StepNo=%d, RetVal=%d \n"), __WFUNCTION__, nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

			// EQP BIT ALL OFF

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- %s Sequence END. StepNo=%d, RetVal=%d \n"), __WFUNCTION__, nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

// Cell Align 결과 송부 : Theta, Distance (셀 중심과 카메라 중심 거리) X, Y
int	WorkManager::Seq_AlignResult_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	int			nTheta = 0, nDistX = 0, nDistY = 0, nStageNo = 0, nPCNo = 0;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

			isSeqBusy = true;

			if( m_bSeqResetFlag && bSeqResetPossible )
				throw 9999;

			// Sequence In LOG
			m_fnPrintLog(_T("SEQLOG -- AlignResult_FromInspPc StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

			nStepNo++;
			switch (nStepNo)
			{
			case 1:
				// Theta, X, Y 값 취득
				{
					nTheta = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
					nDistX = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
					nDistY = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
					nStageNo = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
					nPCNo = *(int*)pParamTemp;
					pParamTemp += sizeof(int);
				}

				m_fnPrintLog(_T("Align Result. Theta = %d, Distance X = %d, Distance Y = %d, Stage No = %d, PC No = %d"), nTheta, nDistX, nDistY, nStageNo, nPCNo);
				break;

			case 2:
				// Main PC 로 전달한다.
				strSendMsg.Format("%s%d.%d.%d.%d.%d.", MAIN_PC_PACKET_ALIGN_RESULT, nTheta, nDistX, nDistY, nStageNo, nPCNo);
				nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
				if (nRet != APP_OK)
				{
					m_fnPrintLog(_T("SEQLOG -- AlignResult_FromInspPc <CASE 2> Message Send Error."));
					throw nRet;
				}


				break;

			default :
				isRunSequence = false;
				break;
			}

			EXCEPTION_CATCH

				if ( nRet != APP_OK)
				{
					// Error Log
					m_fnPrintLog(_T("SEQLOG -- AlignResult_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
					isRunSequence = false;
					int nRetExcept =APP_OK;
				}

	}while ( isRunSequence );

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- AlignResult_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

// "FDB.PATTERN.ON.패턴인덱스." 통신 인터페이스 추가	 180801 YSS
int	WorkManager::Seq_PGChangePatternCmd_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	int			nPatternIndex = -100;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG6057 -- Seq_PGChangePatternCmd_FromInspPc Sequence START.StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			// 패턴 인덱스를 취합한다.
			nPatternIndex = *(int*)pParamTemp;
			pParamTemp += sizeof(int);

			m_fnPrintLog(_T("SEQLOG6057 -- Seq_PGChangePatternCmd_FromInspPc StepNo=%d, nPatternIndex=%d \n"), nStepNo, nPatternIndex);

		break;

		case 2:
			// Main PC 로 전달한다.
			strSendMsg.Format("%s%d", MAIN_PC_PACKET_PG_CHANGE_PATTERN_REPLY, nPatternIndex);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG6057 -- Seq_PGChangePatternCmd_FromInspPc <CASE 2> Message Send Error."));
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
				m_fnPrintLog(_T("SEQLOG6057 -- Seq_PGChangePatternCmd_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG6057 -- Seq_PGChangePatternCmd_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}
// "FDB.PATTERN.OFF." 통신 인터페이스 추가	 180801 YSS
int	WorkManager::Seq_PGOffCmd_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG6058 -- Seq_PGOffCmd_FromInspPc Sequence START.StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			// Main PC 로 전달한다.
			strSendMsg.Format("%s", MAIN_PC_PACKET_PG_OFF_REPLY);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG6058 -- Seq_PGOffCmd_FromInspPc Message Send Error."));
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
				m_fnPrintLog(_T("SEQLOG6058 -- Seq_PGOffCmd_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG6058 -- Seq_PGOffCmd_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

// "FDB.PATTERN.ON." 통신 인터페이스 추가	 180822 YSS
int	WorkManager::Seq_PGOnCmd_FromInspPc(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	byte*		pParamTemp = pParam;
	CStringA	strSendMsg;

	do
	{
		EXCEPTION_TRY

			if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false)	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_BUSY;
			else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
			{
				return MAINPCINTERFACE_TASK_SEQUENCE_IS_NOT_BUSY;
			}

		isSeqBusy = true;

		if (m_bSeqResetFlag && bSeqResetPossible)
			throw 9999;

		// Sequence In LOG
		m_fnPrintLog(_T("SEQLOG6059 -- Seq_PGOnCmd_FromInspPc Sequence START.StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:
			// Main PC 로 전달한다.
			strSendMsg.Format("%s", MAIN_PC_PACKET_PG_ON_REPLY);
			nRet = m_fnSendMessageToMainPc((char*)(LPCSTR)strSendMsg, strSendMsg.GetLength());
			if (nRet != APP_OK)
			{
				m_fnPrintLog(_T("SEQLOG6059 -- Seq_PGOnCmd_FromInspPc Message Send Error."));
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
				m_fnPrintLog(_T("SEQLOG6059 -- Seq_PGOnCmd_FromInspPc Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
				isRunSequence = false;
				int nRetExcept = APP_OK;

				// EQP BIT ALL OFF

			}

	} while (isRunSequence);

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG6059 -- Seq_PGOnCmd_FromInspPc Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}