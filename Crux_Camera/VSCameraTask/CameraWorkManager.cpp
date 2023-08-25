#include "stdafx.h"
#include "CameraWorkManager.h"
#include "VSCameraTask.h"
//#include "NedLineCamera.h"
#include "DalsaLineCamera.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// gui 에서 호출할 경우, VS server 가 실행되어 있지 않으면 GUI 가 블록킹 된다. 다른 쓰레드에서 호출하든지, GUI 에서 직접호출하지 않도록 해야한다.
int VSMessageProcessor::m_fnStart(const CString strInitFilePath, const int nTaskNo)
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
	nFinalTaskNo			= (GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH) * 100)
							+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 90, strInitFilePath))
							+ (nTaskNo - 1);
	
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, strInitFilePath);


	BOOL nSeqMode = GetPrivateProfileInt(_T("Settings"), _T("Pixel Shift Mode"), 4, strInitFilePath);
	

	m_oldCamCondition->dAnalogGain = theApp.m_pCamera->GetAnalogGain();
	m_oldCamCondition->dExpTime = theApp.m_pCamera->GetExposureTime();
	theApp.m_pCamera->SetSequenceMode(nSeqMode);
	m_oldCamCondition->nSeqMode = nSeqMode;

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
			nFinalTaskNo,			// 서버로 등록 할 현재 프로그램의 ID, Task No, 0~65535
			szServerIP,			// Visual station server 가 위치하는 PC 의 IP 주소
			nServerPort			// Server 가 사용할 TCP/IP Port 번호, 기본 5000번
			);

		if (nRet == APP_OK)
		{
			//for (int i = 0; i < 5; i++)
			//{
				handle = m_fnStartThread();
				if ( handle == NULL || handle == (HANDLE)-1 )
					return APP_NG;
			//}
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
	m_oldCamCondition					= new ST_CAMERA_COND();

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

	if (m_pVisualStationInterface != NULL)
	{
		m_pVisualStationInterface->m_fnUnRegisterTask();
		SAFE_DELETE(m_pVisualStationInterface);
	}
	delete m_oldCamCondition;	

 	DeleteCriticalSection(&m_csSequenceLock_1);
 	DeleteCriticalSection(&m_csSequenceLock_2);
 	DeleteCriticalSection(&m_csSequenceLock_3);
 	DeleteCriticalSection(&m_csSequenceLock_4);
 	DeleteCriticalSection(&m_csSequenceLock_5);
}

int	 VSMessageProcessor::m_fnGlobalPrintLog(const wchar_t* pszLogData, ...)
{
	return m_fnPrintLog(pszLogData);
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

// for test0627
// 		m_pVisualStationInterface->m_fnGetServerConnected();
// 		GetVSState();
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

	EXCEPTION_TRY

		//m_fnPrintLog(_T("Analyze Start. FuncNo : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);

		//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					중복실행가능여부,	시퀀스 리셋 가능 여부	)
		SEQUENCE_TABLE (	10,		 1	,	VS_TaskAlive					, false	,			false, &m_csSequenceLock_1)
																														
		SEQUENCE_TABLE (	90,		10	,	VS_InitCamera					, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		11	,	VS_CameraExpose					, true	,			true, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		12	,	VS_WaitGrabEnd					, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		13	,	VS_GetGrabBuffer				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		14	,	VS_GetGrabBufferNoRes			, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		15	,	VS_LiveGrab						, false	,			false, &m_csSequenceLock_3)
		SEQUENCE_TABLE (	90,		115	,	VS_ManualLoadImage				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		16	,	VS_SaveGrabImage				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		17	,	VS_WaitGrabEndSequence			, false	,			false, &m_csSequenceLock_4)
		SEQUENCE_TABLE (	90,		18	,	VS_GetDimmingData				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		20	,	VS_GetImageWidth				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		21	,	VS_GetImageHeight				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		22	,	VS_GetImageBitrate				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		23	,	VS_GetImageBandwidth			, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		24	,	VS_GetExposureTime				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		25	,	VS_GetAnalogGain				, false	,			false, &m_csSequenceLock_2)
		SEQUENCE_TABLE (	90,		50	,	VS_GrabStop						, true ,			true, &m_csSequenceLock_3)

		SEQUENCE_TABLE(     90,     26  ,   VS_SetTirgger                   , false ,           false, &m_csSequenceLock_5)
																														
		SEQUENCE_TABLE (	90,		30	,	VS_SetCameraConditions			, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		31	,	VS_SetExposureTime				, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		32	,	VS_SetAnalogGain				, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		33	,	VS_SetSequenceMode				, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		34	,	VS_SetTriggerMode				, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		99	,	VS_GetCamInfo					, false	,			false, &m_csSequenceLock_5)
		SEQUENCE_TABLE (	90,		98	,	VS_GetCameraTemperature			, false	,			false, &m_csSequenceLock_5)
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
				//m_fnPrintLog(_T("Response Start. FuncNo : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
				nRet = ResponseSend(nRet, pCmdMsg);
				//m_fnPrintLog(_T("Response End. FuncNo : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
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
	
	// Sequence In LOG
	//m_fnPrintLog(_T("CAMLOG -- Seq1001_Task_Alive Sequence Start. \n"));

	EXCEPTION_TRY
// 		if (theApp.GetCameraState()	||
// 			theApp.GetSMemState()		)
			nRet = APP_OK;
// 		else
// 			nRet = APP_NG;
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq1001_Task_Alive Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		//m_fnPrintLog(_T("CAMLOG -- Seq1001_Task_Alive Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}


int VSMessageProcessor::VS_InitCamera( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	CString strInitFilePath = _T("");

	byte* tempParam	= pParam;
	strInitFilePath	= tempParam;
	strInitFilePath.TrimRight();

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9010_Init_Camera Sequence Start. strInitFilePath=%s \n"), strInitFilePath);

	EXCEPTION_TRY
		theApp.m_pCamera->InitializeCamera(strInitFilePath);
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9010_Init_Camera Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq9010_Init_Camera Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

int VSMessageProcessor::VS_CameraExpose( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	

	//byte* tempParam	= pParam;
	//int dd = sizeof(int);

	//TCHAR strDirection[1] = { 0, };		// 100 byte
	//memcpy(strDirection, tempParam, sizeof(tempParam));	
	//tempParam += sizeof(int);
	//int nGrabLine = *(int  *)tempParam;			
	
	byte* tempParam = pParam;

	//ST_LINE_INFO stLineParam;
	//stLineParam = *(ST_LINE_INFO *)tempParam;

	/////// 아이디 추가 ////
	TCHAR strPanelID[50] = { 0, };		// 100 byte
	memcpy(strPanelID, tempParam, sizeof(strPanelID));
	tempParam += sizeof(strPanelID);

	TCHAR strVirtualID[50] = { 0, };		// 100 byte
	memcpy(strVirtualID, tempParam, sizeof(strVirtualID));
	tempParam += sizeof(strVirtualID);

	int nGrabLine = *(int*)tempParam;
	tempParam += sizeof(int);

	TCHAR strPosition[50] = { 0, };		// 100 byte
	memcpy(strPosition, tempParam, sizeof(strPosition));
	tempParam += sizeof(strPosition);

	/////// 아이디 추가 ////

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9011_Camera_Expose Sequence Start(), %d. \n"), nGrabLine);
	
//#ifdef _DALSALINECAMERA 1221
//	CDalsaLineCamera* temp = (CDalsaLineCamera*)theApp.m_pCamera;
//	temp->m_Trigger->TriggerInit1(); //트리거 초기화
//	if (strDirection == _T("F"))
//	{
//		temp->m_Trigger->TriggerOrigin();
//		temp->m_Trigger->TriggerStartPositionChannel0(temp->TriggerStartF, temp->TriggerStopF, temp->TriggerPeriodF);
//		temp->m_Trigger->StartForwardScan();
//	}
//	else
//	{
//		temp->m_Trigger->TriggerOrigin();
//		temp->m_Trigger->TriggerStartPositionChannel0(temp->TriggerStartB, temp->TriggerStopB, temp->TriggerPeriodB);
//		temp->m_Trigger->StartBackwardScan();
//	}
	
//#elif _NEDLINECAMERA
//	CNedLineCamera* temp = (CNedLineCamera*)theApp.m_pCamera;
//	temp->m_Trigger->TriggerOrigin();
//	temp->m_Trigger->TriggerStartPositionChannel0(temp->TriggerStartF, temp->TriggerStopF, temp->TriggerPeriodF);
//	temp->m_Trigger->StartForwardScan();
//#endif

	EXCEPTION_TRY
#ifdef _MATROXCAMERA
#elif _VISTEKCAMERA

	theApp.m_pCamera->CameraExpose();
#elif _IMICAMERA
#elif _HIKCAMERA
#elif _MATROXLINECAMERA
#elif _NEDLINECAMERA
#elif _DALSALINECAMERA
	CString VirID;
	CString PanelID;
	CString Position;
	VirID = (LPCTSTR)strVirtualID;
	PanelID = (LPCTSTR)strPanelID;
	Position = (LPCTSTR)strPosition;
	theApp.m_pCamera->CameraExpose(PanelID, VirID, Position, nGrabLine);
#elif _TestCam
		CString VirID;
	CString PanelID;
	CString Position;
	VirID = (LPCTSTR)strVirtualID;
	PanelID = (LPCTSTR)strPanelID;
	Position = (LPCTSTR)strPosition;
	theApp.m_pCamera->CameraExpose(PanelID, VirID, Position, nGrabLine);
#endif

	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9011_Camera_Expose Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9011_Camera_Expose Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}


int VSMessageProcessor::VS_SetTirgger(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	CString DcfFile;

	byte* tempParam = pParam;

	ST_LINE_INFO stLineParam;
	stLineParam = *(ST_LINE_INFO *)tempParam;

	
	YskimLib *lib = &YskimLib::Instance();
	DcfFile = lib->GetDataFromINI(_T("Settings"), _T("Init File Path"), lib->GetData(_T("inifile")));
	

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9026 Trigger Setting. Start(POS : %s, Current Grab Num: %d). \n"), stLineParam.stLineData[0].strDirection, stLineParam.stLineData[0].nCurrentGrab);

	CDalsaLineCamera* temp = (CDalsaLineCamera*)theApp.m_pCamera;

	//temp->InitGrabber(0, 0, DcfFile);

	//temp->m_Trigger->TriggerInit0(); //트리거 파형 S,W, H 값 적용 "AOI,3,1,1,1,"
	//temp->m_Trigger->TriggerInit1(); //트리거 파형 S,W, H 값 적용
	//temp->m_Trigger->StopTriggerGen0(); // 트리거 발생 정지 및 트리거 카운터 초기화 "AOI,0,0,"
	//temp->m_Trigger->StopTriggerGen1(); // 트리거 발생 정지 및 트리거 카운터 초기화

	CString df = stLineParam.stLineData[0].strDirection;

	//MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_DIRECTION, M_FORWARD);

	if (df == _T("F"))
	{
		//temp->m_Trigger->TriggerInit0(); //트리거 파형 S,W, H 값 적용 "AOI,3,1,1,1,"
		//temp->m_Trigger->TriggerInit1(); //트리거 파형 S,W, H 값 적용
		/////temp->m_Trigger->StopTriggerGen0(); // 트리거 발생 정지 및 트리거 카운터 초기화 "AOI,0,0,"
		/////temp->m_Trigger->StopTriggerGen1(); // 트리거 발생 정지 및 트리거 카운터 초기화

		//MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_DIRECTION, M_FORWARD);
		//MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_POSITION, 0);
		//MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_POSITION_TRIGGER, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodF);

		//temp->m_Trigger->TriggerOrigin1();
		/////temp->m_Trigger->TriggerStartPositionChannel1(stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStartF, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStopF, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodF);
		/////temp->m_Trigger->TriggerStartPositionChannel0(stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStartF, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStopF, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodF);
		//temp->m_Trigger->StartForwardScan();


		//temp->m_Trigger->TriggerCurrentPosition();
		//temp->m_Trigger->TriggerGenCount0();
		//temp->m_Trigger->TriggerGenCount1();
	}
	else
	{
		//temp->m_Trigger->TriggerOrigin0();
		//temp->m_Trigger->TriggerInit0(); //트리거 파형 S,W, H 값 적용 "AOI,3,1,1,1,"
		//temp->m_Trigger->TriggerInit1();
		//temp->m_Trigger->TriggerOrigin0();
	/*	temp->m_Trigger->StopTriggerGen0();
		temp->m_Trigger->StopTriggerGen1();*/

		/*MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_DIRECTION, M_BACKWARD);
		MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_POSITION, 0);
		MdigControl(temp->m_milDigitizer, M_ROTARY_ENCODER_POSITION_TRIGGER, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodB);*/


		//////temp->m_Trigger->TriggerStartPositionChannel0(stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStartB, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStopB, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodB);
		//////temp->m_Trigger->TriggerStartPositionChannel1(stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStartB, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nStopB, stLineParam.stLineData[stLineParam.stLineData[0].nCurrentGrab].nPeriodB);
		//////temp->m_Trigger->StartBackwardScan();


		//////temp->m_Trigger->TriggerCurrentPosition();
		//////temp->m_Trigger->TriggerGenCount0();
		//////temp->m_Trigger->TriggerGenCount1();


	
	}
	
	EXCEPTION_TRY
	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9026_Camera_Trigger END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
	
	EXCEPTION_CATCH
	m_fnPrintLog(_T("CAMLOG -- Seq9026_Camera_Trigger ERR. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
	
	return nRet;
}


int VSMessageProcessor::VS_WaitGrabEnd(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9012_Wait_Grab_End Sequence Start. \n"));

	EXCEPTION_TRY
		theApp.m_pCamera->WaitGrabEnd();
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9012_Wait_Grab_End Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq9012_Wait_Grab_End Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}


int VSMessageProcessor::VS_WaitGrabEndSequence( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;
	PARAM_WAIT_GRAB_END stWaitGrabEndParam;
	stWaitGrabEndParam = *(PARAM_WAIT_GRAB_END *)tempParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9017_Wait_Grab_End Sequence Start. bUseSMem=%s, strPanelID=%s, GrabCnt =%d \n"),
		stWaitGrabEndParam.bUseSMem ? _T("TRUE") : _T("FALSE"),
		//stWaitGrabEndParam.nGrabNum,
		//stWaitGrabEndParam.bUseFileSave?_T("TRUE"):_T("FALSE"),
		stWaitGrabEndParam.strPanelID,
		stWaitGrabEndParam.GrabCnt
		
						//stWaitGrabEndParam.strGrabStepName
						);

	EXCEPTION_TRY

		int nRetryCnt = 3;
	int	ProcessGrabCnt = 0;
		for (int i =0 ; i < nRetryCnt; i++)
		{
			m_fnPrintLog(_T("CAMLOG -- Seq9017_Wait_Grab_End Sequence Before WaitGrabEnd."));
			//theApp.m_pCamera->WaitGrabEnd();
			if (stWaitGrabEndParam.bUseSMem)
				ProcessGrabCnt = theApp.m_pCamera->SetSMemCurBuffer(stWaitGrabEndParam.GrabCnt, stWaitGrabEndParam.strPanelID);
			if (stWaitGrabEndParam.bUseFileSave)
			{
				// 확장자 결정
				if (theApp.m_pCamera->GetImageBitrate() == 8)
					_tcscat(stWaitGrabEndParam.strSavePath, _T(".bmp"));
				else
					_tcscat(stWaitGrabEndParam.strSavePath, _T(".tiff"));
				theApp.m_pCamera->SaveFileCurBuffer(stWaitGrabEndParam.strSavePath);
			}

			if(theApp.m_pCamera->GetImageCallBackState() == 0)  
				break;
			//else 
				//theApp.m_pCamera->retryConnect();
			//*(int *)pParam = theApp.m_pCamera->GetImageCallBackState();	pParam   += sizeof(int);
			//*(int *)pParam = ProcessGrabCnt;	pParam += sizeof(int);
			//stWaitGrabEndParam.GrabCnt = ProcessGrabCnt;
		}
			

		EXCEPTION_CATCH
			if ( nRet != APP_OK)
			{
				// Error Log
				m_fnPrintLog(_T("CAMLOG -- Seq9013_Wait_Grab_End Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
				return nRet;
			}

			// Sequence Out LOG
			m_fnPrintLog(_T("CAMLOG -- Seq9013_Wait_Grab_End Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}


int VSMessageProcessor::VS_GetDimmingData( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;
	
	int	  nBrightVal = *(int *)tempParam;  	tempParam   += sizeof(int);
	CRect crecGuideLine = *(CRect *)tempParam; 	tempParam   += sizeof(CRect);

	// Sequence In LOG
	//m_fnPrintLog(_T("CAMLOG -- Seq9018_VS_GetDimmingData Sequence Start. bUseSMem=%s, nGrabNum=%d, bUseFileSave=%s, strSavePath=%s \n"),

	EXCEPTION_TRY

		theApp.m_pCamera->WaitGrabEnd();

		 int nOutExp = 0, nOutGain = 0 ;
		theApp.m_pCamera->GetBrightnessVal(nBrightVal, crecGuideLine , nOutExp, nOutGain);
			
		*(int *)pParam = nOutExp; pParam   += sizeof(int);
		*(int *)pParam = nOutGain; pParam   += sizeof(int);
	EXCEPTION_CATCH
		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9018_VS_GetDimmingData Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq9018_VS_GetDimmingData Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}


int VSMessageProcessor::VS_GetGrabBuffer( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	int nImgCnt = 0;
	BOOL bIsLive = FALSE;
	BOOL bIsGrayImg = FALSE;
	ULONG lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, lWindowSizeX, lWindowSizeY;
	
	// for test0627
	//m_fnPrintLog(_T("CAMLOG -- Seq9013_Get_Grab_Buffer Sequence START. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	byte* tempParam	= pParam;
	nImgCnt  		= *(int*)tempParam;		    tempParam   += sizeof(int);
	bIsLive  		= *(BOOL*)tempParam;		tempParam   += sizeof(BOOL);	
	lOffsetX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lOffsetY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lWindowSizeX	= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lWindowSizeY	= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	
	if (lCropSizeX <= 0 || lCropSizeY <= 0 || lWindowSizeX <= 0 || lWindowSizeY <= 0 || lWindowSizeX * lWindowSizeY > nPrmSize)
	{
		CString str;
		str.Format(_T("%d,%d,%d,%d,%d,%d\n"), lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, lWindowSizeX, lWindowSizeY);
		OutputDebugString(str);
		return 0;
	}


	EXCEPTION_TRY
		Sleep(0);

		if (theApp.m_pCamera != NULL)
			theApp.m_pCamera->GetGrabImage(nImgCnt, bIsLive, pParam, lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, lWindowSizeX, lWindowSizeY);
	
	EXCEPTION_CATCH

	// for test0627
	//m_fnPrintLog(_T("CAMLOG -- Seq9013_Get_Grab_Buffer Sequence RESPONSE. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9013_Get_Grab_Buffer Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	//m_fnPrintLog(_T("CAMLOG -- Seq9013_Get_Grab_Buffer Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetGrabBufferNoRes( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	ULONG lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, lWindowSizeX, lWindowSizeY;

	byte* tempParam	= pParam;	

	lOffsetX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lOffsetY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lWindowSizeX	= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lWindowSizeY	= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);

	byte* pImgBuf = new byte[lWindowSizeX * lWindowSizeY];

	EXCEPTION_TRY
	//	theApp.m_pCamera->GetGrabImage(pImgBuf, lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, lWindowSizeX, lWindowSizeY);
		m_fnCmdEditSend(VS_SEND_GRAB_IMAGE, 0, (USHORT)(lWindowSizeX * lWindowSizeY), VS_SEQ_TASK, pImgBuf);
	EXCEPTION_CATCH

	SAFE_DELETE_ARR(pImgBuf);

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9014_Get_Grab_Buffer_NoRes Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9014_Get_Grab_Buffer_NoRes Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_LiveGrab( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	int nChCnt  = 0;
	BOOL bLive;

	byte* tempParam	= pParam;
	bLive = *(BOOL *)tempParam;		tempParam += sizeof(BOOL);

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9015_Live_Grab Sequence Start. bLive=%s \n"), bLive?_T("TRUE"):_T("FALSE"));	
	
	EXCEPTION_TRY
	int nRetryCnt = 3;
	for (int i =0 ; i < nRetryCnt; i++)
	{
		if (bLive)
			nRet = theApp.m_pCamera->StartLiveGrab() ? APP_OK : APP_NG;
		else
			nRet = theApp.m_pCamera->StopLiveGrab() ? APP_OK : APP_NG;

		if(theApp.m_pCamera->GetImageCallBackState() == 0)  break;
		else theApp.m_pCamera->retryConnect();
		//*(int *)pParam = theApp.m_pCamera->GetImageCallBackState();	pParam   += sizeof(int);
	}

	nChCnt = theApp.m_pCamera->GetImageBandwidth();
	*(int *)pParam = nChCnt;	pParam   += sizeof(int);
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9015_Live_Grab Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9015_Live_Grab Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_ManualLoadImage(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	int nImgCnt = 0;
	int nChCnt  = 0;
	double dAngle = 0.0F;
	byte*	tempParam = pParam;	
	int		nWidth, nHeight = 0;
	wchar_t strImagePath[1000];
	memcpy(strImagePath, tempParam, 1000);	tempParam += 1000;  	
	nImgCnt  		= *(int*)tempParam;		tempParam   += sizeof(int);
	dAngle  		= *(double*)tempParam;		tempParam   += sizeof(double);
	EXCEPTION_TRY	
		
		theApp.m_pCamera->m_fnSetBufImage(strImagePath, nImgCnt, nChCnt, nWidth, nHeight, dAngle);	  		
		*(int *)pParam = nChCnt;	pParam   += sizeof(int);
		*(int *)pParam = nWidth;	pParam   += sizeof(int);
		*(int *)pParam = nHeight;	pParam   += sizeof(int);
	
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq90115_ManualLoadImage Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq90115_ManualLoadImage Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

// 2017.05.11 add by ikm GUI -> Image SAVE 용 - S
int VSMessageProcessor::VS_SaveGrabImage( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	int nImgCnt = 0;
	BOOL bIsLive = FALSE;
	BOOL bIsGrayImg = FALSE;
	ULONG lOffsetX, lOffsetY, lCropSizeX, lCropSizeY;

	// for test0627
	//m_fnPrintLog(_T("CAMLOG -- Seq9013_Get_Grab_Buffer Sequence START. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	byte* tempParam	= pParam;
	nImgCnt  		= *(int*)tempParam;		    tempParam   += sizeof(int);
	bIsLive  		= *(BOOL*)tempParam;		tempParam   += sizeof(BOOL);
	//bIsGrayImg		= *(BOOL*)tempParam;		tempParam   += sizeof(BOOL);
	lOffsetX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lOffsetY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeX		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);
	lCropSizeY		= *(ULONG*)tempParam;		tempParam   += sizeof(ULONG);

	TCHAR strTmpPath[500];
	CString strSavePath = _T("");
	memcpy(strTmpPath, tempParam, sizeof(WCHAR) * 500);				tempParam += sizeof(WCHAR) * 500;
	strSavePath = strTmpPath;
	strSavePath.TrimRight();

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9016_Save_Grab_Image Sequence Start. strSavePath=%s \n"), strSavePath);

	EXCEPTION_TRY
		theApp.m_pCamera->SaveGrabImage(nImgCnt, bIsLive, bIsGrayImg, lOffsetX, lOffsetY, lCropSizeX, lCropSizeY, strSavePath);
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9016_Save_Grab_Image Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq9016_Save_Grab_Image Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}
// 2017.05.11 add by ikm GUI -> Image SAVE 용 - E

int VSMessageProcessor::VS_GetImageWidth( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nImageWidth = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9020_Get_Image_Width Sequence Start. \n"));

	EXCEPTION_TRY
		nImageWidth = theApp.m_pCamera->GetImageWidth();
		*(UINT *)pParam = nImageWidth;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9020_Get_Image_Width Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9020_Get_Image_Width Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetImageHeight( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nImageHeight = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9021_Get_Image_Height Sequence Start. \n"));

	EXCEPTION_TRY
		nImageHeight = theApp.m_pCamera->GetImageHeight();
		*(UINT *)pParam = nImageHeight;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9021_Get_Image_Height Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9021_Get_Image_Height Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetImageBitrate( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nImageBitrate = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9022_Get_Image_Bitrate Sequence Start. \n"));

	EXCEPTION_TRY
		nImageBitrate = theApp.m_pCamera->GetImageBitrate();
		*(UINT *)pParam = nImageBitrate;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9022_Get_Image_Bitrate Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9022_Get_Image_Bitrate Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetImageBandwidth( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nImageBandwidth = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9023_Get_Image_Bandwidth Sequence Start. \n"));

	EXCEPTION_TRY
		nImageBandwidth = theApp.m_pCamera->GetImageBandwidth();
		*(UINT *)pParam = nImageBandwidth;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9023_Get_Image_Bandwidth Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9023_Get_Image_Bandwidth Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetExposureTime( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	DOUBLE dExpTime = 0.0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9024_Get_Exp_Time Sequence Start. \n"));

	EXCEPTION_TRY
		dExpTime = theApp.m_pCamera->GetExposureTime();
		*(double *)pParam = dExpTime;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9024_Get_Exp_Time Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9024_Get_Exp_Time Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_GetAnalogGain( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	double dAnalogGain = 0;

	byte* tempParam	= pParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9025_Get_Analog_Gain Sequence Start. \n"));

	EXCEPTION_TRY
		dAnalogGain = theApp.m_pCamera->GetAnalogGain();
		*(double *)pParam = dAnalogGain;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9025_Get_Analog_Gain Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9025_Get_Analog_Gain Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_GrabStop(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	double dAnalogGain = 0;

	byte* tempParam = pParam;
	int ProcessNum = 0;
	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9050_GrabStop Sequence Start. \n"));

	EXCEPTION_TRY
#ifdef _MATROXCAMERA
#elif _VISTEKCAMERA
		//theApp.m_pCamera->StopGrab();
#elif _IMICAMERA
#elif _DALSACAMERA
#elif _HIKCAMERA
#elif _MATROXLINECAMERA
#elif _NEDLINECAMERA
#elif _DALSALINECAMERA
		ProcessNum = theApp.m_pCamera->StopGrab(50);
#elif _TestCam
		ProcessNum = theApp.m_pCamera->StopGrab(50);	
#endif
		*(int*)tempParam = ProcessNum;
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9050_GrabStop Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9050_GrabStop Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_SetCameraConditions( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	ST_CAM_COND_AOT stCurCameraCond;
	double dExpTime = 0.0;
	UINT nAnalogGain = 0, nSeqMode = 0;

	byte* tempParam	= pParam;

	stCurCameraCond = *(ST_CAM_COND_AOT *)tempParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Sequence Start. dExpTime=%lf, nAnalogGain=%lf, nSeqMode=%d \n"),
						stCurCameraCond.Expose, stCurCameraCond.Gain, stCurCameraCond.PS);

	EXCEPTION_TRY
		theApp.m_fnWriteTactLog(_T("\t\tExp. Start"));

	if (!theApp.m_pCamera->SetExposureTime(stCurCameraCond.Expose))	
		return APP_NG;	
	if (!theApp.m_pCamera->SetAnalogGain(stCurCameraCond.Gain))
		return APP_NG;	
	//if (!theApp.m_pCamera->SetSequenceMode(stCurCameraCond.nSeqMode))	
	//	return APP_NG;
	//if (!theApp.m_pCamera->SetStitchMode(stCurCameraCond.nCamTab))		// 180809 YSS
	//	return APP_NG;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_SetCameraConditions_AOT(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	ST_CAM_COND_AOT stCurCameraCond;
	double dExpTime = 0.0;
	UINT nAnalogGain = 0, nSeqMode = 0;

	byte* tempParam = pParam;

	stCurCameraCond = *(ST_CAM_COND_AOT *)tempParam;

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Sequence Start. dExpTime=%lf, nAnalogGain=%lf \n"),
		stCurCameraCond.Expose, stCurCameraCond.Gain);

	EXCEPTION_TRY
		theApp.m_fnWriteTactLog(_T("\t\tExp. Start"));

	if (!theApp.m_pCamera->SetExposureTime(stCurCameraCond.Expose))
		return APP_NG;
	if (!theApp.m_pCamera->SetAnalogGain(stCurCameraCond.Gain))
		return APP_NG;
	//if (!theApp.m_pCamera->SetSequenceMode(stCurCameraCond.nSeqMode))
	//	return APP_NG;
	//if (!theApp.m_pCamera->SetStitchMode(stCurCameraCond.nCamTab))		// 180809 YSS
	//	return APP_NG;
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9030_Set_Camera_Conditions Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_SetExposureTime( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	double dExpTime = 0.0;

	byte* tempParam	= pParam;

	dExpTime		= *(double*)tempParam;		tempParam   += sizeof(double);

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9031_Set_Exp_Time Sequence Start. dExpTime=%lf \n"), dExpTime);

	EXCEPTION_TRY		
		if (!theApp.m_pCamera->SetExposureTime(dExpTime))
			nRet = APP_NG;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9031_Set_Exp_Time Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9031_Set_Exp_Time Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_SetAnalogGain( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nAnalogGain = 0;

	byte* tempParam	= pParam;

	nAnalogGain		= *(UINT*)tempParam;		tempParam   += sizeof(UINT);

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9031_Set_Exp_Time Sequence Start. nAnalogGain=%d \n"), nAnalogGain);

	EXCEPTION_TRY
		if (!theApp.m_pCamera->SetAnalogGain(nAnalogGain))
			nRet = APP_NG;

	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9032_Set_Analog_Gain Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9032_Set_Analog_Gain Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_SetSequenceMode( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nSeqMode = 0;

	byte* tempParam	= pParam;

	nSeqMode		= *(UINT*)tempParam;		tempParam   += sizeof(UINT);

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9033_Set_Analog_Gain Sequence Start. nSeqMode=%d \n"), nSeqMode);

	EXCEPTION_TRY
		if (!theApp.m_pCamera->SetSequenceMode(nSeqMode))
			nRet = APP_NG;
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		m_fnPrintLog(_T("CAMLOG -- Seq9033_Set_Analog_Gain Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9033_Set_Analog_Gain Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

int VSMessageProcessor::VS_SetTriggerMode( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;
	UINT nTrigMode = 0;

	byte* tempParam	= pParam;

	nTrigMode		= *(UINT*)tempParam;		tempParam   += sizeof(UINT);

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9034_Set_Trigger_Mode Sequence Start. nTrigMode=%d \n"), nTrigMode);

	EXCEPTION_TRY
		if (!theApp.m_pCamera->SetTriggerMode(nTrigMode))
			nRet = APP_NG;
	EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9034_Set_Trigger_Mode Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
			return nRet;
		}

		// Sequence Out LOG
		m_fnPrintLog(_T("CAMLOG -- Seq9034_Set_Trigger_Mode Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		return nRet;
}

int VSMessageProcessor::VS_GetCamInfo(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam = pParam;
	ST_CAM_INFOMATION stCurCamInfo;
	stCurCamInfo = *(ST_CAM_INFOMATION *)tempParam;	

	// Sequence In LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9099_Get_Cam_Information Sequence Start.\n"));

	EXCEPTION_TRY
		CString BoardName = theApp.m_pCamera->GetBoardName();
	CString CamType = theApp.m_pCamera->GetCameraType();
		CString CamName = theApp.m_pCamera->GetCameraName();
	int CamWidth = theApp.m_pCamera->GetCameraWidth();
	int CamHeight = theApp.m_pCamera->GetCameraHeight();
	int CamDepth = theApp.m_pCamera->GetCameraDepth();
	double CamTemp = theApp.m_pCamera->GetCameraTemperature();

	memcpy((*(ST_CAM_INFOMATION *)tempParam).BoardName, BoardName, BoardName.GetLength() * 2);
	memcpy((*(ST_CAM_INFOMATION *)tempParam).Name, CamName, CamName.GetLength() * 2);
	memcpy((*(ST_CAM_INFOMATION *)tempParam).Type, CamType, CamType.GetLength() * 2);
	(*(ST_CAM_INFOMATION *)tempParam).Width = CamWidth;
	(*(ST_CAM_INFOMATION *)tempParam).Height = CamHeight;
	(*(ST_CAM_INFOMATION *)tempParam).Depth = CamDepth;
	(*(ST_CAM_INFOMATION *)tempParam).Temp = CamTemp;
	//if (!theApp.m_pCamera->SetTriggerMode(nTrigMode))
	//	nRet = APP_NG;
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9099_Get_Cam_Information Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9099_Get_Cam_Information Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}
int VSMessageProcessor::VS_GetCameraTemperature(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/)
{
	int nRet = APP_OK;
	int nStepNo = 0;
	EXCEPTION_TRY

	byte* tempParam = pParam;
	m_fnPrintLog(_T("CAMLOG -- Seq9098_Get_Cam_Temperature Sequence Start.\n"));
	double CamTemp = theApp.m_pCamera->GetCameraTemperature();

	*(double*)tempParam = CamTemp;
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			// Error Log
			m_fnPrintLog(_T("CAMLOG -- Seq9098_Get_Cam_Temperature Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			return nRet;
		}

	// Sequence Out LOG
	m_fnPrintLog(_T("CAMLOG -- Seq9098_Get_Cam_Temperature Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
	return nRet;
}
BOOL VSMessageProcessor::GetVSState()	
{	
	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;
}

int VSMessageProcessor::VS_Send_Pattern_OnOff_To_PG(int nState)
{
	byte*	pParam			= new byte[sizeof(int)];
	byte*	pSendParam		= pParam;
	int		nRet			= APP_OK;

	*(int *)pSendParam = nState;	pSendParam += sizeof(int);

	//m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;
	
	EXCEPTION_TRY
		nRet = m_fnCmdEditSend(VS_SEND_PATTERN_ONOFF, 0, sizeof(int) , VS_PG_TASK, pParam);


	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
		}

	SAFE_DELETE_ARR(pParam);

	return nRet;

}

int	VSMessageProcessor::VS_Send_Pattern_Change_To_PG(TCHAR* strPattern)
{
	byte*	pParam			= new byte[sizeof(TCHAR)];
	byte*	pSendParam		= pParam;
	int		nRet			= APP_OK;
	TCHAR					strPatternName[50] = {0,};
	memcpy(strPatternName,strPattern,sizeof(strPatternName));	pSendParam += sizeof(strPatternName);
	
	EXCEPTION_TRY
 		nRet = m_fnCmdEditSend(VS_SEND_PATTERN_CHANGE, 0, (USHORT)(pSendParam - pParam), VS_PG_TASK, (byte *)&strPatternName, CMD_TYPE_NORES);

	EXCEPTION_CATCH

		if (nRet != APP_OK)

		{
		}

		SAFE_DELETE_ARR(pParam);

		return nRet;
		
}