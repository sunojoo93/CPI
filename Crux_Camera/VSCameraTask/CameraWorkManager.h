
#pragma once


#include "..\..\VisualStation\CommonHeader\Class\Thread.h"
#include "..\..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "..\..\CommonHeaders\Structure.h"


#define	ON			1
#define	OFF			0

// ´Ù¸¥ Task No
#define VS_UI_TASK									20
#define VS_SEQ_TASK									21
#define VS_PG_TASK									40  + (theApp.GetPcNum() * 100)

// Send Message
#define VS_SEND_GRAB_IMAGE							90, 14
#define VS_SEND_PATTERN_ONOFF						40, 11
#define VS_SEND_PATTERN_CHANGE						40, 13

#define TASK_EXCEPTION_UNKNOWN_EXCEPTION			1100

#define TASK_SEQUENCE_IS_NOTHING					1500
#define TASK_SEQUENCE_IS_BUSY						1501
#define TASK_SEQUENCE_IS_NOT_BUSY					1502
#define TASK_SEQUENCE_IS_NOT_EXECUTABLE				1505		// Indexer Initial All¡¡ñé

#define EXCEPTION_TRY	try{
#define EXCEPTION_CATCH	}catch(int nErrCode){nRet = nErrCode;}	catch(...){nRet = TASK_EXCEPTION_UNKNOWN_EXCEPTION;}

#define	SEQUENCE_TABLE(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME,ALWAYS_RUNMODE,SEQ_REST_POSSIBLE)								\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )										\
	{																														\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount++;																							\
			isFunctionRuned = true;																							\
			nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize, ALWAYS_RUNMODE, false, SEQ_REST_POSSIBLE);		\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount--;																							\
	}																														\




class VSMessageProcessor 
	: protected CThread
{
public:
	VSMessageProcessor();
	~VSMessageProcessor();

	int		m_fnStart(const CString strInitFilePath, const int nTaskNo);
	void	m_fnSetSeqThreadRunFlag(bool bSet)	{ m_bIsSeqThreadRun = bSet; };
	int		m_fnCmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, USHORT PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType = CMD_TYPE_NORES);
	BOOL	GetVSState();	//	{	return m_pVisualStationInterface->m_fnGetServerConnected()?TRUE:FALSE;	};

	int		VS_Send_Pattern_OnOff_To_PG(int nState);
	int		VS_Send_Pattern_Change_To_PG(TCHAR* strPattern);

	int		m_fnGlobalPrintLog(const wchar_t* pszLogData, ...);

private:
	void	m_fnRunThread();
	int		AnalyzeMsg(CMDMSG* pCmdMsg);
	int		ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg);
	int		m_fnPrintLog( const wchar_t* pszLogData, ... );
	int		m_fnPrintLog( const char* pszLogData, ... );

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/

	// Receive Message
	// Alive
	int		VS_TaskAlive			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Init
	int		VS_InitCamera			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Grab
	int		VS_CameraExpose			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_WaitGrabEnd			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_WaitGrabEndSequence	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetDimmingData       (byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetGrabBuffer		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetGrabBufferNoRes	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_LiveGrab				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int     VS_ManualLoadImage		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SaveGrabImage		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Get
	int		VS_GetImageWidth		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetImageHeight		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetImageBitrate		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetImageBandwidth	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetExposureTime		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_GetAnalogGain		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Set
	int		VS_SetTirgger           (byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SetCameraConditions	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SetExposureTime		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SetAnalogGain		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SetSequenceMode		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int		VS_SetTriggerMode		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

private:
	CInterServerInterface*	m_pVisualStationInterface;
	HANDLE					m_hSequenceResetEvent;
	bool					m_bSeqResetFlag;
	UINT					m_SeqenceCount;
	bool					m_bIsSeqThreadRun;
	int						m_nSimulationGrabTime;
	ST_CAMERA_COND*			m_oldCamCondition;

// 	CRITICAL_SECTION	m_csSequenceLock_1;
// 	CRITICAL_SECTION	m_csSequenceLock_2;
// 	CRITICAL_SECTION	m_csSequenceLock_3;
// 	CRITICAL_SECTION	m_csSequenceLock_4;
// 	CRITICAL_SECTION	m_csSequenceLock_5; 
};