
#pragma once


#include "..\..\VisualStation\CommonHeader\Class\Thread.h"
#include "..\..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "define.h"


#define	ON			1
#define	OFF			0

// ´Ù¸¥ Task No
#define VS_UI_TASK									10
#define VS_SEQ_TASK									21

// Send Message
#define VS_SEND_GRAB_IMAGE							80, 14
#define VS_SEND_CHECK_LIGHT							80, 40

#define TASK_EXCEPTION_UNKNOWN_EXCEPTION			1100

#define TASK_SEQUENCE_IS_NOTHING					1500
#define TASK_SEQUENCE_IS_BUSY						1501
#define TASK_SEQUENCE_IS_NOT_BUSY					1502
#define TASK_SEQUENCE_IS_NOT_EXECUTABLE				1505		// Indexer Initial All¡¡ñé

#define EXCEPTION_TRY	try{
#define EXCEPTION_CATCH	}catch(int nErrCode){nRet = nErrCode;}	catch(...){nRet = TASK_EXCEPTION_UNKNOWN_EXCEPTION;}

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
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			LeaveCriticalSection(CS_LOCK_NAME);																				\
		}																													\
	}



class VSMessageProcessor 
	: protected CThread
{
public:
	VSMessageProcessor();
	~VSMessageProcessor();

	int					m_fnStart(const CString strInitFilePath, int nTaskNo);
	void				m_fnSetSeqThreadRunFlag(bool bSet)	{ m_bIsSeqThreadRun = bSet; };
	int					m_fnCmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, USHORT PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType = CMD_TYPE_NORES);
	BOOL				GetVSState();	//	{	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;	};

	STRU_LIGHT_INFO		m_stLightInfo;

	int					m_fnPrintLog( const wchar_t* pszLogData, ... );
	int					m_fnPrintLog( const char* pszLogData, ... );

private:
	void				m_fnRunThread();
	int					AnalyzeMsg(CMDMSG* pCmdMsg);
	int					ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg);
	//int					m_fnPrintLog( const wchar_t* pszLogData, ... );
	//int					m_fnPrintLog( const char* pszLogData, ... );

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/

	// Receive Message
	// Alive
	int					VS_TaskAlive			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Init
	int					VS_InitLight			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Turn On
	int					VS_MultiTurnOn			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int					VS_SingleTurnOn			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Turn Off
	int					VS_MultiTurnOff			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int					VS_SingleTurnOff		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Check
	int					VS_CheckLight			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int					VS_SequenceIndexReset	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int					VS_GetTriggerCount		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);



private:
	CInterServerInterface*	m_pVisualStationInterface;
	HANDLE					m_hSequenceResetEvent;
	bool					m_bSeqResetFlag;
	UINT					m_SeqenceCount;
	bool					m_bIsSeqThreadRun;
// 	STRU_LIGHT_INFO*		m_OldLightInfo;

	CRITICAL_SECTION	m_csSequenceLock_1;
	CRITICAL_SECTION	m_csSequenceLock_2;
	CRITICAL_SECTION	m_csSequenceLock_3;
	CRITICAL_SECTION	m_csSequenceLock_4;
	CRITICAL_SECTION	m_csSequenceLock_5;
};