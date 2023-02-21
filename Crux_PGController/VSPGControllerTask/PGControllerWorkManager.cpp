#include "stdafx.h"
#include "PGControllerWorkManager.h"
#include "VSPGControllerTask.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// gui ���� ȣ���� ���, VS server �� ����Ǿ� ���� ������ GUI �� ���ŷ �ȴ�. �ٸ� �����忡�� ȣ���ϵ���, GUI ���� ����ȣ������ �ʵ��� �ؾ��Ѵ�.
int VSMessageProcessor::m_fnStart(const CString strInitFilePath)
{
	int nRet = APP_OK;
	TCHAR szLogPath[100] = {0,}, szLogFileName[100] = {0,};
	int nLogPeriodDay = 0, nLogWriteTerm = 0, nLogLevel = 0;
	TCHAR szServerIP[100] = {0,};
	int nTaskNo = 0, nServerPort = 0;
	int nPortNo, nBaudRate;

	HANDLE handle;

	m_bIsSeqThreadRun = true;
	
	TCHAR szSimulationFlag[6] = {0,};
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szSimulationFlag, sizeof(szSimulationFlag), INIT_FILE_PATH);
	if( !_tcscmp(szSimulationFlag, _T("TRUE")) )
		m_bSimulationMode = TRUE;
	GetPrivateProfileString(_T("LogInfo"), _T("Path") ,_T("C:\\VSLOG\\"), szLogPath, sizeof(szLogPath), strInitFilePath);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("VSTask"), szLogFileName, sizeof(szLogFileName), strInitFilePath);
	nLogPeriodDay		= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, strInitFilePath);
	nLogWriteTerm		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 4, strInitFilePath);
	nLogLevel			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 4, strInitFilePath);

	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), strInitFilePath);
	// Task No = [PC ��ȣ]*100 + [Task ������ȣ(10~90)] + [Task ����(0~9)]
	// ex. 1�� PC �� 2�� Camera Task => 191
	nTaskNo				= (GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH) * 100)
						+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 80, strInitFilePath))
						+ (GetPrivateProfileInt(_T("Settings"), _T("Light Number"), 1, strInitFilePath) - 1);
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, strInitFilePath);

	nPortNo				= (GetPrivateProfileInt(_T("PGInfo"), _T("Port"), 1, strInitFilePath));
	nBaudRate			= (GetPrivateProfileInt(_T("PGInfo"), _T("BaudRate"), 1, strInitFilePath));
	
	
	if(m_pVisualStationInterface == NULL)
	{

		// CInterServerInterface �ʱ�ȭ
		m_pVisualStationInterface = new CInterServerInterface(
			szLogPath,			// �α� ������ �� ���
			szLogFileName,		// �α� ���� �̸�
			NULL,				// ������ �޽ø� ���� VS �޽����� ������ ���, Receive Window Handle
			NULL,				// ������ �޽ø� ���� VS �޽����� ������ ���, Message ID
			false,				// Visual station Server�� ���� �� ���, ���� ���α׷��� ���� ���� ����
			nLogPeriodDay,		// �α� ������ �����Ⱓ
			nLogWriteTerm,					// �α� ������ ���� �ð� ����
			true,				// �α� ������ ������ ���� ������ ����
			nLogLevel				// �α� ������ ������ �� ���, ���ܵ� �α׷���. 0�� ��� ���Էα�. 1�� �Է��ϸ� 2���� �α� ������ ���ܵȴ�.
			);		

		nRet = m_pVisualStationInterface->m_fnRegisterTask(
			nTaskNo,			// ������ ��� �� ���� ���α׷��� ID, Task No, 0~65535
			szServerIP,			// Visual station server �� ��ġ�ϴ� PC �� IP �ּ�
			nServerPort			// Server �� ����� TCP/IP Port ��ȣ, �⺻ 5000��
			);

		
		if (nRet == APP_OK)
		{
			if(theApp.GetSimulationMode())
			{
				CString strLog;
				strLog.Format(_T("Simulation Mode Start"));
				theApp.m_fnWriteLog(strLog);
			}
			else
				m_fnInitPGInfo(nPortNo,nBaudRate);

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

//////////////////////////// ���ϴ� ������ �� Private ����̹Ƿ� �ʿ��� ��쿡�� Ȯ�� �ϸ� �ȴ� /////////////////////////////////////

VSMessageProcessor::VSMessageProcessor()
{
	m_pVisualStationInterface			= NULL;
	m_hSequenceResetEvent				= CreateEvent( NULL, TRUE,  FALSE,  NULL );
	m_bSeqResetFlag						= 0;
	m_SeqenceCount						= 0;
	m_bIsSeqThreadRun					= false;
	m_bSimulationMode					= FALSE;

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

	//Start Status Refresh Thread. ó�� ����Ǵ� 1�� �����尡 ó���� �����Ѵ�.

	while (m_bIsSeqThreadRun)
	{
		EXCEPTION_TRY

		pMsg = m_pVisualStationInterface->m_fnPeekMessage();
		pCmdMsg = (CMDMSG*)pMsg;


		nRet = AnalyzeMsg(pCmdMsg);
		if ( nRet != APP_OK ) 
			throw nRet;
		if(pMsg !=NULL)
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

	
	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD && theApp.GetSimulationMode() )
	{
		nRet = ResponseSend(nRet, pCmdMsg);
		//return nRet;
	}


	EXCEPTION_TRY

	//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					�ߺ����డ�ɿ���,	������ ���� ���� ����	)
	SEQUENCE_TABLE (	10,		 1	,	VS_TaskAlive					, false	,			false					,	&m_csSequenceLock_1	)	
	SEQUENCE_TABLE (	40,		10	,	VS_InitPgCtl					, false	,			false					,	&m_csSequenceLock_2	)	
	SEQUENCE_TABLE (	40,		11	,	VS_PGOnOff						, false	,			false					,	&m_csSequenceLock_2	)	
	SEQUENCE_TABLE (	40,		12	,	VS_PrevNextPattern				, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		13	,	VS_ChangePattern				, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		14	,	VS_Open							, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		15	,	VS_Close						, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		16	,	VS_IsConnected					, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		17	,	VS_Swap_Voltage					, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		18	,	VS_Send_PatternInfo				, false	,			false					,	&m_csSequenceLock_2	)
	SEQUENCE_TABLE (	40,		20	,	VS_Send_VoltageInfo				, false	,			false					,	&m_csSequenceLock_2	)

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

	if(m_bSimulationMode)
		return nRet;
	EXCEPTION_TRY
		if( theApp.m_pPgController->IsConnected() )
			nRet = APP_OK;
		else
			nRet = APP_NG;
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


int VSMessageProcessor::m_fnInitPGInfo(int nPGPortNo, int nBaudRate)
{
	m_stPGInfo.nPort	 = nPGPortNo;
	m_stPGInfo.nBaudRate = nBaudRate;
	TCHAR strPatternInfo[256];
	CString strINIFilePath = theApp.GetInitFilePath();
	int nSize = GetPrivateProfileSection(_T("Pattern_Info"), strPatternInfo, 4096, strINIFilePath);
	theApp.m_pPgController->SetPatternInfoSize(nSize);
	theApp.m_pPgController->SetPatternInfo(strPatternInfo);
	if(!m_bSimulationMode)
		theApp.m_pPgController->OpenPort(nPGPortNo, nBaudRate);

	//////////////////////////Test Ver.///////////////////  		
// 	if (theApp.m_pPgController == NULL)
// 	{
// 		theApp.m_pPgController->Close();
// 		delete theApp.m_pPgController;
// 		theApp.m_pPgController = NULL;
// 	}

	//INIT_PG_CONTROLLER(m_pPGControl, m_fnGetPGKind(), m_fnGetPGPort());  		

	return TRUE;	
}

int VSMessageProcessor::VS_InitPgCtl(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte*		tempParam = pParam;
	int			nPGKind	= 0, nPGPortNo = 0, nBaudRate = 0;
	
	nPGPortNo	= *(int*)tempParam;			tempParam += sizeof(int);
	nBaudRate	= *(int*)tempParam;			tempParam += sizeof(int);

	EXCEPTION_TRY


	nRet = m_fnInitPGInfo(nPGPortNo, nBaudRate);
	EXCEPTION_CATCH
		if ( nRet != APP_OK )
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2010_m_fnInitPgCtl Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			int nRetExcept =APP_OK;
		}

		// Sequence Out LOG
		//m_fnPrintLog(_T("SEQLOG -- Seq2010_m_fnInitPgCtl Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_ChangePattern(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;
		
	//((CVSPGControllerTaskApp *)GetParent())->get = value

	if(m_bSimulationMode)
		return nRet;

	byte*		tempParam			= pParam;
	int			nPatternNo			= 0;
	
	TCHAR strPattern[20] = {0,};
	
	//memcpy(strPattern,tempParam,sizeof(strPattern));	tempParam += sizeof(strPattern);
	nPatternNo = *(int*)tempParam;
		
	EXCEPTION_TRY
		// Dust Pattern �� ��� PG ��.
		// �� �� Dust Pattern �� PG Recipe �� �߰��� ��� ����
		//if( !_tcsicmp(strPattern, _T("DUST")) )

		m_fnPrintLog(_T("SEQLOG -- Change Pattern Start : INDEX : %d"), nPatternNo);

		if (nPatternNo < 0 || nPatternNo >= MAX_GRAB_STEP_COUNT) nPatternNo = -100;

		if( theApp.bIsDust(nPatternNo))				
		{	
			nRet = theApp.m_pPgController->TurnOff();
		}
		else 
		{	
			nRet = theApp.m_pPgController->ChangePattern(nPatternNo/*strPattern*/);
			
			// 17.08.09 PG ����ȭ �ð� �߰�
			Sleep(100);

			ST_PG_DATA VoltageInfo = theApp.GetCurPatternVoltageData(nPatternNo/*strPattern*/);
			PARAM_PG *pParamPg = new PARAM_PG;
			pParamPg->nChIndexNum = theApp.GetPgInfo().nChIndexNum;			
			memcpy(pParamPg->strChIndex, theApp.GetPgInfo().strChIndex, sizeof(theApp.GetPgInfo().strChIndex));
			memcpy(&pParamPg->pVoltageInfo, &VoltageInfo, sizeof(ST_PG_DATA));
			
			m_fnPrintLog(_T("SEQLOG -- Set Voltage Start. \n %s / %s(%d) / R:%d / G:%d / B:%d / %s(%d)\n"), pParamPg->pVoltageInfo.bJudge?_T("OK"):_T("SKIP"), pParamPg->pVoltageInfo.strPatternName, pParamPg->pVoltageInfo.nPtnNum,
																		(int)(short)pParamPg->pVoltageInfo.wVoltage[0], (int)(short)pParamPg->pVoltageInfo.wVoltage[1], (int)(short)pParamPg->pVoltageInfo.wVoltage[2]
																		, pParamPg->strChIndex, pParamPg->nChIndexNum);
						
			if(&pParamPg->pVoltageInfo != NULL) nRet = theApp.m_pPgController->SwapVoltage(pParamPg);

			SAFE_DELETE(pParamPg);
		}
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq2013_m_fnChangePattern Error Occured. PtnVal = %d, StepNo=%d, RetVal=%d \n"), nPatternNo, nStepNo, nRet);			
		int nRetExcept =APP_OK;
	}
		
	return nRet;
}

int VSMessageProcessor::VS_PGOnOff(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	
	int			nRet = APP_OK;
	int			nStepNo = 0;
	
	byte*		tempParam		= pParam;
	int			nOnOff			= 0;

	if(m_bSimulationMode)
		return nRet;

	nOnOff	= *(int*)tempParam;	tempParam += sizeof(int);		

	EXCEPTION_TRY

		char  cParam[200]			= {0,};
		char* pTempParam			= cParam;			
		*(int*)pTempParam			= nOnOff;		
		pTempParam				   += sizeof(int);			

		if(nOnOff)	//ON
			nRet = theApp.m_pPgController->TurnOn();
		else
			nRet = theApp.m_pPgController->TurnOff();

	EXCEPTION_CATCH
		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2011_VS_PGOnOff Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			int nRetExcept =APP_OK;
		}

		// Sequence Out LOG
		//m_fnPrintLog(_T("SEQLOG -- Seq2011_VS_PGOnOff Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

int VSMessageProcessor::VS_PrevNextPattern(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{	
	
	int			nRet = APP_OK;
	int			nStepNo = 0;

	byte*		tempParam		= pParam;
	int			nNext			= 0;

	if(m_bSimulationMode)
		return nRet;

	nNext	= *(int*)tempParam;	tempParam += sizeof(int);		

	EXCEPTION_TRY

		char  cParam[200]			= {0,};
		char* pTempParam			= cParam;			
		*(int*)pTempParam			= nNext;		
		pTempParam				   += sizeof(int);			
	

		if (nNext == TRUE)
		{
			nRet = theApp.m_pPgController->NextPattern();
		}
		else 
		{		
			nRet = theApp.m_pPgController->PrevPattern();
		}

		//nRet = m_fnCmdEditSend( VS_PG_PTN_ON, 0, (USHORT)(pTempParam - cParam), VS_G1_PG, (byte*)cParam, CMD_TYPE_RES);

		EXCEPTION_CATCH
			if ( nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(_T("SEQLOG -- Seq2012_VS_PrevNextPattern Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
				int nRetExcept =APP_OK;
			}

			// Sequence Out LOG
			//m_fnPrintLog(_T("SEQLOG -- Seq2012_VS_PrevNextPattern Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			
	return nRet;
}
int VSMessageProcessor::VS_Open(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte*		tempParam		= pParam;
	int			nPort			= 0;

	nPort	= *(int*)tempParam;	tempParam += sizeof(int);		


// 	EXCEPTION_TRY
// 		if(m_fnGetPGKind() != NUM_VS_PG)
// 			nRet = m_pPGControl->Open(nPort);
// 
// 	EXCEPTION_CATCH
// 		if ( nRet != APP_OK)
// 		{
// 			// Error Log
// 			m_fnPrintLog(_T("SEQLOG -- Seq2014_VS_Open Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
// 			int nRetExcept =APP_OK;
// 		}
// 
// 		// Sequence Out LOG
// 		m_fnPrintLog(_T("SEQLOG -- Seq2014_VS_Open Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);


		return nRet;
}


int VSMessageProcessor::VS_Close(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;

// 	EXCEPTION_TRY
// 		if(m_fnGetPGKind() != NUM_VS_PG)
// 			nRet = m_pPGControl->Close();
// 
// 	EXCEPTION_CATCH
// 		if ( nRet != APP_OK)
// 		{
// 			// Error Log
// 			m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
// 			int nRetExcept =APP_OK;
// 		}
// 
// 		// Sequence Out LOG
// 		m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
// 

		return nRet;
}

int VSMessageProcessor::VS_IsConnected(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	EXCEPTION_TRY
		if( theApp.m_pPgController->IsConnected() )
			nRet = APP_OK;
		else
			nRet = APP_NG;
		//*(int *)pParam = nRet;	nPrmSize = sizeof(int);
	EXCEPTION_CATCH
		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			int nRetExcept =APP_OK;
		}

		// Sequence Out LOG
		//m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);


		return nRet;
}

int VSMessageProcessor::VS_Swap_Voltage(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	PARAM_PG* pStParamPG = new PARAM_PG;

	EXCEPTION_TRY

	byte* pReceiveParam = pParam;
	memcpy(pStParamPG,pReceiveParam,sizeof(PARAM_PG));

	theApp.m_pPgController->SwapVoltage(pStParamPG);

	EXCEPTION_CATCH
	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		int nRetExcept =APP_OK;
	}

	// Sequence Out LOG
	//m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
	SAFE_DELETE(pStParamPG);

	return nRet;
}

int VSMessageProcessor::VS_Send_PatternInfo( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	TCHAR strPatternInfo[256] = {0};
	TCHAR SendData[256] = {0};
	int pos = 0;
	bool bEqul = false;
	bool bSection = false; 
	TCHAR s_key[100] = {0};

		memcpy(strPatternInfo,theApp.m_pPgController->GetPatternInfo(),256);
		//m_strPatternInfo

			for(int i = 0; i < theApp.m_pPgController->GetPatternInfoSize(); i++, pos++)
			{
				if(strPatternInfo[i] != '\0') //���ڰ� ���� ���
				{
					if(strPatternInfo[i] != '=' && bEqul == false)
					{
						if(bSection) pos = 0;
						memcpy(s_key + pos, strPatternInfo+i, sizeof(s_key+pos)-2); //=�������� Ű�� ����
						bSection = FALSE;
						continue;
					}
					else 
					{
						bEqul = true; // ���ڰ� =�� ���
						//s_key[i++] = ',';
						
					}

				}
				else //������� ( Ű�� �� / ������ ��)
				{
					if(SendData[0] != 0)
						_tcscat(SendData,_T(","));
					_tcscat(SendData,s_key);
					//_stprintf(s_key, _T(",%s"), s_key);
					memset(s_key, 0, sizeof(s_key));
					bSection = true;
					bEqul = false;
				}
			}
	memcpy(pParam,SendData,_tcslen(SendData) * sizeof(TCHAR));
	nPrmSize = _tcslen(SendData) * sizeof(TCHAR);
	memset(s_key, 0, sizeof(s_key));
	//EXCEPTION_CATCH
		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("SEQLOG -- Seq2015_VS_Close Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			int nRetExcept =APP_OK;
		}

		return nRet;
}


int VSMessageProcessor::VS_Send_VoltageInfo( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	byte* pReceiveParam = pParam;

	CVSPGControllerTaskApp *pDlg = (CVSPGControllerTaskApp*)AfxGetMainWnd();

	ST_PG_INFO * stVolInfo = new ST_PG_INFO;	
	memcpy(stVolInfo, pReceiveParam, sizeof(ST_PG_INFO)); //return TRUE;
	nRet = theApp.SetPgInfo(stVolInfo);
	if(nRet == TRUE)
		nRet = APP_OK;
	else
		nRet = APP_NG;
	//m_strPatternInfo//
	SAFE_DELETE(stVolInfo);
	//EXCEPTION_CATCH
	if ( nRet != APP_OK)
	{		// Error Log
		m_fnPrintLog(_T("SEQLOG -- Seq4020_VS_Send_VoltageInfo Error Occured. RetVal=%d \n"), nRet);			
		int nRetExcept =APP_NG;
	}

	return nRet;
}
	
int	VSMessageProcessor::Seq_TEST(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
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

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// �������� Busy �ΰ�� ���� ���ϵǰ� �Ϸ��� bAlwaysRunMode�� false �̾�� �Ѵ�.
			return TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return TASK_SEQUENCE_IS_NOT_BUSY;
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

			////Port Cancel Host�� Unload Request ����
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
	//m_fnPrintLog(_T("SEQLOG -- Seq2650_TEST Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

BOOL VSMessageProcessor::GetVSState()	
{	
	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;
}

