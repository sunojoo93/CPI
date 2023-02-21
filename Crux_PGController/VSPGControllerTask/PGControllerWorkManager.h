
#pragma once

#include "..\..\VisualStation\CommonHeader\Class\Thread.h"
#include "..\..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "define.h"
#include "Structure.h"


#define	ON			1
#define	OFF			0

// ´Ù¸¥ Task No
#define VS_UI_TASK									10
#define VS_SEQ_TASK									21

// Send Message
#define VS_SEND_GRAB_IMAGE							80, 14

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

	int					m_fnStart(const CString strInitFilePath);
	void				m_fnSetSeqThreadRunFlag(bool bSet)	{ m_bIsSeqThreadRun = bSet; };
	int					m_fnCmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, USHORT PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType = CMD_TYPE_NORES);
	int					m_fnInitPGInfo(int nPGPortNo, int nBaudRate);
	
	BOOL				GetVSState();	//	{	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;	};
	
	int					m_fnPrintLog( const wchar_t* pszLogData, ... );
	int					m_fnPrintLog( const char* pszLogData, ... );

private:
	void				m_fnRunThread();
	int					AnalyzeMsg(CMDMSG* pCmdMsg);
	int					ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg);

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/

	// Receive Message
	// Alive
	int	VS_TaskAlive			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int Seq_TEST				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_Ptn_ON_Req			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_Ptn_OFF_Req			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_Sel_Ptn_Req			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	VS_ChangePattern		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_InitPgCtl			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_PGOnOff				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_PrevNextPattern		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_Open					(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_Close				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_IsConnected			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	VS_Swap_Voltage			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_Send_PatternInfo		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int VS_Send_VoltageInfo		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	
private:
	CInterServerInterface*	m_pVisualStationInterface;
	HANDLE					m_hSequenceResetEvent;
	bool					m_bSeqResetFlag;
	UINT					m_SeqenceCount;
	bool					m_bIsSeqThreadRun;
	STRU_PG_INFO			m_stPGInfo;
	bool					m_bSimulationMode;

	CRITICAL_SECTION	m_csSequenceLock_1;
	CRITICAL_SECTION	m_csSequenceLock_2;
	CRITICAL_SECTION	m_csSequenceLock_3;
	CRITICAL_SECTION	m_csSequenceLock_4;
	CRITICAL_SECTION	m_csSequenceLock_5;

	
};