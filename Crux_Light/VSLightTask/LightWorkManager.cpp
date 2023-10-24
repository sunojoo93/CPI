#include "stdafx.h"
#include "LightWorkManager.h"
#include "VSLightTask.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// gui 에서 호출할 경우, VS server 가 실행되어 있지 않으면 GUI 가 블록킹 된다. 다른 쓰레드에서 호출하든지, GUI 에서 직접호출하지 않도록 해야한다.
int VSMessageProcessor::m_fnStart(const CString strInitFilePath, int nTaskNo)
{
	int nRet = APP_OK;
	TCHAR szLogPath[100] = {0,}, szLogFileName[100] = {0,};
	int nLogPeriodDay = 0, nLogWriteTerm = 0, nLogLevel = 0;
	TCHAR szServerIP[100] = {0,};
	int nFinalTaskNo = 0, nServerPort = 0;

	HANDLE handle;

	m_bIsSeqThreadRun = true;

	GetPrivateProfileString(_T("LogInfo"), _T("Path") ,_T("C:\\VSLOG\\"), szLogPath, sizeof(szLogPath), strInitFilePath);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("VSTask"), szLogFileName, sizeof(szLogFileName), strInitFilePath);
	nLogPeriodDay		= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, strInitFilePath);
	nLogWriteTerm		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 4, strInitFilePath);
	nLogLevel			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 4, strInitFilePath);

	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), strInitFilePath);
	// Task No = [PC 번호]*100 + [Task 고유번호(10~90)] + [Task 순번(0~9)]
	// ex. 1번 PC 의 2번 Camera Task => 191
	nFinalTaskNo		= (GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH) * 100)
						+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 80, strInitFilePath))
						//+ (GetPrivateProfileInt(_T("Settings"), _T("Light Number"), 1, strInitFilePath) - 1);
						+ (nTaskNo - 1);		// 실행 시 Task No 받아오도록 수정 (INI - Light Number 삭제)
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, strInitFilePath);

	theApp.SetVsTaskNum(nFinalTaskNo);
	if(m_pVisualStationInterface == NULL)
	{

		// CInterServerInterface 초기화
		m_pVisualStationInterface = new CInterServerInterface(
			szLogPath,			// 로그 파일을 쓸 경로
			szLogFileName,		// 로그 파일 이름
			NULL,				// 윈도우 메시를 통해 VS 메시지를 수신할 경우, Receive Window Handle
			NULL,				// 윈도우 메시를 통해 VS 메시지를 수신할 경우, Message ID
			false,				// Visual station Server가 종료 할 경우, 같이 프로그램을 종료 할지 여부
			nLogPeriodDay,		// 로그 파일의 보존기간
			nLogWriteTerm,					// 로그 파일의 기입 시간 단위
			true,				// 로그 파일을 실제로 기입 할지의 여부
			nLogLevel			// 로그 파일을 실제로 쓸 경우, 차단될 로그레벨. 0은 상시 기입로그. 1로 입력하면 2이하 로그 기입은 차단된다.
			);		

		nRet = m_pVisualStationInterface->m_fnRegisterTask(
			nFinalTaskNo,		// 서버로 등록 할 현재 프로그램의 ID, Task No, 0~65535
			szServerIP,			// Visual station server 가 위치하는 PC 의 IP 주소
			nServerPort			// Server 가 사용할 TCP/IP Port 번호, 기본 5000번
			);

		if (nRet == APP_OK)
		{
			for (int i = 0; i < 5; i++)
			{
				handle = m_fnStartThread();
				if ( handle == NULL || handle == (HANDLE)-1 )
					return APP_NG;
			}
		}
		else
		{
			delete m_pVisualStationInterface;
			m_pVisualStationInterface = NULL;
		}
	}

	return nRet;
}

int VSMessageProcessor::m_fnCmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, USHORT PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType )
{

	int nRet = APP_OK;
	CMDMSG tempCmdMsg;

	if(m_pVisualStationInterface == NULL)
		return ERR_CLIENT_NOT_REGISTERED;

	tempCmdMsg.uTask_Src	= m_pVisualStationInterface->m_uTaskNo;
	tempCmdMsg.uTask_Dest	= TaskNo;
	tempCmdMsg.uFunID_Dest	= FunID;
	tempCmdMsg.uSeqID_Dest	= SeqNo;
	tempCmdMsg.uUnitID_Dest = UnitNo;
	tempCmdMsg.uMsgSize		= PrmSize;
	tempCmdMsg.cMsgBuf		= (INT64)Buff;
	tempCmdMsg.uMsgType		= uMsgType;

	nRet = m_pVisualStationInterface->m_fnSendCommand(&tempCmdMsg, LOG_LEVEL_NONE);


	return nRet;
}

//////////////////////////// 이하는 생성자 및 Private 멤버이므로 필요한 경우에만 확인 하면 된다 /////////////////////////////////////

VSMessageProcessor::VSMessageProcessor()
{
	m_pVisualStationInterface			= NULL;
	m_hSequenceResetEvent				= CreateEvent( NULL, TRUE,  FALSE,  NULL );
	m_bSeqResetFlag						= 0;
	m_SeqenceCount						= 0;
	m_bIsSeqThreadRun					= false;
	//m_OldLightInfo						= new STRU_LIGHT_INFO();

	InitializeCriticalSection(&m_csSequenceLock_1);
	InitializeCriticalSection(&m_csSequenceLock_2);
	InitializeCriticalSection(&m_csSequenceLock_3);
	InitializeCriticalSection(&m_csSequenceLock_4);
	InitializeCriticalSection(&m_csSequenceLock_5);
}


VSMessageProcessor::~VSMessageProcessor()
{
	m_bIsSeqThreadRun = false;
	Sleep(100);

	m_pVisualStationInterface->m_fnUnRegisterTask();

	delete m_pVisualStationInterface;

	DeleteCriticalSection(&m_csSequenceLock_1);
	DeleteCriticalSection(&m_csSequenceLock_2);
	DeleteCriticalSection(&m_csSequenceLock_3);
	DeleteCriticalSection(&m_csSequenceLock_4);
	DeleteCriticalSection(&m_csSequenceLock_5);
}



int	 VSMessageProcessor::m_fnPrintLog(const wchar_t* pszLogData, ...)
{

	va_list vaList;
	va_start(vaList, pszLogData);
	int len = _vscwprintf( pszLogData, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		vswprintf(cBuffer, pszLogData, (va_list)vaList);

		if (m_pVisualStationInterface != NULL)
			m_pVisualStationInterface->m_fnPrintLog(LOG_LEVEL_4, cBuffer );

		delete [] cBuffer;
	}

	va_end(vaList);

	return APP_OK;
}

int	 VSMessageProcessor::m_fnPrintLog(const char* pszLogData, ...)
{
	va_list vaList;
	va_start(vaList, pszLogData);
	int len = _vscprintf( pszLogData, vaList ) + 1;
	char	*cBuffer	= new char[sizeof(char)*len];
	memset(cBuffer,0,sizeof(char)*len);

	if (cBuffer)
	{
		vsprintf(cBuffer, pszLogData, (va_list)vaList);

		if (m_pVisualStationInterface != NULL)
			m_pVisualStationInterface->m_fnPrintLog(LOG_LEVEL_4, cBuffer );

		delete [] cBuffer;
	}

	va_end(vaList);

	return APP_OK;
}



int VSMessageProcessor::ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg)
{
	int nRet = APP_OK;
	CMDMSG tempCmdMsg;
	byte* pBuff;

	pBuff = new byte[pCmdMsg->uMsgSize];

	memcpy(pBuff, (void*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);
	memcpy(&tempCmdMsg, pCmdMsg, sizeof(CMDMSG));
	tempCmdMsg.cMsgBuf = (INT64)pBuff;

	tempCmdMsg.uRetStatus = ReturnVal;

	if (m_pVisualStationInterface != NULL)
		nRet = m_pVisualStationInterface->m_fnSendResponseCommand(&tempCmdMsg);

	delete pBuff;

	return nRet;
}


void VSMessageProcessor::m_fnRunThread()
{
	int nRet;
	BYTE *pMsg = NULL;
	CMDMSG *pCmdMsg = NULL;

	//Start Status Refresh Thread. 처음 실행되는 1개 쓰레드가 처리를 전담한다.

	while (m_bIsSeqThreadRun)
	{
		EXCEPTION_TRY

		pMsg = m_pVisualStationInterface->m_fnPeekMessage();
		pCmdMsg = (CMDMSG*)pMsg;


		nRet = AnalyzeMsg(pCmdMsg);
		if ( nRet != APP_OK ) 
			throw nRet;

		m_pVisualStationInterface->m_fnFreeMemory(pMsg);

		pCmdMsg = NULL;
		pMsg = NULL;

		EXCEPTION_CATCH

			if ( nRet != APP_OK )
			{
				if ( pMsg != NULL )
				{
					m_pVisualStationInterface->m_fnFreeMemory(pMsg);
					pCmdMsg = NULL;
					pMsg = NULL;
				}
				m_fnPrintLog(_T("ERROR PlcWorkManager::AnalyzeMsg. Error Code = %d \n"), nRet);
			}
	}
}





int VSMessageProcessor::AnalyzeMsg(CMDMSG* pCmdMsg)
{
	int			nRet = APP_OK;
	bool		isFunctionRuned = false;	

	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);

	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD && !_tcscmp(szSimulationFlag, _T("TRUE")) )
	{
		nRet = ResponseSend(nRet, pCmdMsg);
		return nRet;
	}


	EXCEPTION_TRY

	//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					중복실행가능여부,	시퀀스 리셋 가능 여부	)
	SEQUENCE_TABLE (	10,		 1	,	VS_TaskAlive					, false	,			false					,	&m_csSequenceLock_1	)	

	SEQUENCE_TABLE (	80,		10	,	VS_InitLight					, false	,			false					,	&m_csSequenceLock_2	)	

	SEQUENCE_TABLE (	80,		20	,	VS_MultiTurnOn					, false	,			false					,	&m_csSequenceLock_2	)	
	SEQUENCE_TABLE (	80,		21	,	VS_SingleTurnOn					, false	,			false					,	&m_csSequenceLock_2	)	

	SEQUENCE_TABLE (	80,		30	,	VS_MultiTurnOff					, false	,			false					,	&m_csSequenceLock_2	)	
	SEQUENCE_TABLE (	80,		31	,	VS_SingleTurnOff				, false	,			false					,	&m_csSequenceLock_2	)	

	SEQUENCE_TABLE (	80,		40	,	VS_CheckLight					, false	,			false					,	&m_csSequenceLock_2	)	
	SEQUENCE_TABLE (	80,		41	,	VS_SequenceIndexReset			, false	,			false					,	&m_csSequenceLock_3	)
	SEQUENCE_TABLE (	80,		42	,	VS_GetAlarmCode					, false	,			false					,	&m_csSequenceLock_4	)
	SEQUENCE_TABLE (	80,		15	,	VS_ApplyLightProperty			, false	,			false					,	&m_csSequenceLock_5	)
	if( m_SeqenceCount <= 0 )
	{
		m_bSeqResetFlag = 0;
		m_SeqenceCount = 0;
		ResetEvent(m_hSequenceResetEvent);
	}

	if ( isFunctionRuned  == false ) 
	{
		m_fnPrintLog(_T("Function Table is nothing. FuncNo : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		throw TASK_SEQUENCE_IS_NOTHING;
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Receive Message
int VSMessageProcessor::VS_TaskAlive( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;

	EXCEPTION_TRY
		if (!theApp.GetLightState())
			nRet = APP_NG;
		else
			nRet = APP_OK;
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq1001_Task_Alive Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		//m_fnPrintLog(_T("SEQLOG -- Seq1001_Task_Alive Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

int VSMessageProcessor::VS_InitLight( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	CString strInitFilePath = _T("");

	byte* tempParam	= pParam;
	strInitFilePath	= tempParam;
	strInitFilePath.TrimRight();

	EXCEPTION_TRY
		theApp.m_pLight->InitializeLight(strInitFilePath);	
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8010_Init_Light Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("SEQLOG -- Seq8010_Init_Light Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

int VSMessageProcessor::VS_ApplyLightProperty(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	CString strInitFilePath = _T("");

	byte* tempParam = pParam;

	ST_GRAB_AREA_INFO_AOT LightValue = *(ST_GRAB_AREA_INFO_AOT*)tempParam;
	//memcpy(&LightValue, &tempParam, sizeof(ST_GRAB_AREA_INFO_AOT));
	//LightValue = (ST_GRAB_AREA_INFO_AOT *)tempParam;

	EXCEPTION_TRY
		theApp.m_pLight->ApplyLightProperty(LightValue);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8010_Init_Light Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8010_Init_Light Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_MultiTurnOn( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;
	BOOL bChangeFlag = 0;
	STRU_LIGHT_INFO RcvLightCond;
	RcvLightCond = *(STRU_LIGHT_INFO *)tempParam;	// Light Info 정적 배열로 변경

	m_fnPrintLog(_T("SEQLOG -- Seq8020_Multi_Turn_On Sequence Start."));	// 조명 On 전에 로그 추가		180511 YSS

	EXCEPTION_TRY
// 		for (int i=0; i<theApp.m_pLight->GetControllerCount(); i++)
// 		{
// 			for (int j=0; j<theApp.m_pLight->GetChannelCount(); j++)
// 			{	
// 				if( m_OldLightInfo->stLight[i].nLightVal[j] != m_stLightInfo.stLight[i].nLightVal[j] )
// 					bChangeFlag++;
// 			}
// 		}
// 
// 		if(bChangeFlag > 0)
		theApp.m_pLight->MultiTurnOn(RcvLightCond);
// 		memcpy( m_OldLightInfo,&m_stLightInfo,sizeof(STRU_LIGHT_INFO) );
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq8020_Multi_Turn_On Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8020_Multi_Turn_On Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_SingleTurnOn( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nControllerNum = 0, nChannelNum = 0, nIntensity = 0;

	byte* tempParam	= pParam;
	nControllerNum		= *(UINT*)tempParam;		tempParam   += sizeof(UINT);
	nChannelNum			= *(UINT*)tempParam;		tempParam   += sizeof(UINT);
	nIntensity			= *(UINT*)tempParam;		tempParam   += sizeof(UINT);

	EXCEPTION_TRY
		theApp.m_pLight->SingleTurnOn(nControllerNum, nChannelNum, nIntensity);
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq8021_VS_Single_Turn_On Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8021_VS_Single_Turn_On Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_MultiTurnOff( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8030_Multi_Turn_Off Sequence Start. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	EXCEPTION_TRY
		theApp.m_pLight->MultiTurnOff();
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq8030_Multi_Turn_Off Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8030_Multi_Turn_Off Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_SingleTurnOff( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nControllerNum = 0, nChannelNum = 0;

	byte* tempParam	= pParam;	

	nControllerNum		= *(UINT*)tempParam;		tempParam   += sizeof(UINT);
	nChannelNum			= *(UINT*)tempParam;		tempParam   += sizeof(UINT);

	EXCEPTION_TRY
		theApp.m_pLight->SingleTurnOff(nControllerNum, nChannelNum);
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq8031_Single_Turn_Off Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8031_Single_Turn_Off Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_GetTriggerCount(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;
	int ModuleNum = *(int*)tempParam;

	EXCEPTION_TRY
		theApp.m_pLight->Write_Func6_UINT32(0, (BYTE)0x03, (WORD)0x0100, 1);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8042_GetTriggerCount Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8042_GetTriggerCount Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;

}
int VSMessageProcessor::VS_SequenceIndexReset(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;
	int ModuleNum = *(int*)tempParam;

	EXCEPTION_TRY
		theApp.m_pLight->Write_Func6_UINT16(0, (BYTE)0x06, (WORD)0x0008, (WORD)1);

	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8041_Sequence Index Count Reset Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8041_Sequence Index Count Reset Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;

}
int VSMessageProcessor::VS_GetAlarmCode(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;

	/**(UINT*)tempParam = */
	//UINT AlarmCode = theApp.m_pLight->GetAlarmCode();
	//	theApp.m_
	EXCEPTION_TRY
		/**(UINT*)tempParam = */
	*(UINT*)tempParam = theApp.OccuredAlaramCode;;
	tempParam += sizeof(UINT);
	*(UINT*)tempParam = theApp.Temperature;
	tempParam += sizeof(UINT);

	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8042_Get Alarm Code Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8042_Get Alarm Code Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;

}
int VSMessageProcessor::VS_GetTemperature(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;

	/**(UINT*)tempParam = */
	//UINT AlarmCode = theApp.m_pLight->GetAlarmCode();
	//	theApp.m_
	EXCEPTION_TRY
		/**(UINT*)tempParam = */
		UINT AlarmCode = theApp.OccuredAlaramCode;
	*(UINT*)tempParam = AlarmCode;

	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8042_Get Alarm Code Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("SEQLOG -- Seq8042_Get Alarm Code Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;

}
int VSMessageProcessor::VS_CheckLight( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;
	int nControllerNum		= *(int*)tempParam;		
	tempParam   += sizeof(int);

	EXCEPTION_TRY
		nControllerNum = APP_OK;
	    //theApp.m_pLight->CheckLight(nControllerNum);
			
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq8031_Single_Turn_Off Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("SEQLOG -- Seq8031_Single_Turn_Off Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

BOOL VSMessageProcessor::GetVSState()	
{	
	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;
}