#pragma once

#include "..\VisualStation\CommonHeader\Class\Thread.h"
#include "..\VisualStation\CommonHeader\Class\MessageQueue.h"
#include "..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "..\CommonHeaders\Structure.h"
#include "DefineSequenceTable.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

struct EVENT_INFO
{
	HANDLE	hEvent;
	BYTE	bParam[1000];
	bool	bUse;

	EVENT_INFO()
	{
		hEvent = CreateEvent( NULL, TRUE,  FALSE,  NULL );
		memset(bParam, 0, sizeof(bParam));
		bUse = false;
	}

	~EVENT_INFO()
	{
		CloseHandle( hEvent );
	}
};

struct MAIN_PC_REPLY_EVENT
{
	EVENT_INFO EI_CurrentRecipe;
	EVENT_INFO EI_MoveMotion_Motor_X;
	EVENT_INFO EI_MoveMotion_Motor_Y;
	EVENT_INFO EI_MoveMotion_Motor_T;
	EVENT_INFO EI_MoveMotion_Motor_Z;
	EVENT_INFO EI_MoveMotion_Focus_Z;
	EVENT_INFO EI_MoveMotion_Zoom_Z;

	EVENT_INFO EI_SetRecipePos;
	EVENT_INFO EI_SetVacuum;
	EVENT_INFO EI_Set_LineGrab;

	EVENT_INFO			EI_STAGEMOVE_FORWARD;
	EVENT_INFO			EI_STAGEMOVE_BACKWARD;

	EVENT_INFO EI_ReplyMotionStaus_Motor_X;
	EVENT_INFO EI_ReplyMotionStaus_Motor_Y;
	EVENT_INFO EI_ReplyMotionStaus_Motor_T;
	EVENT_INFO EI_ReplyMotionStaus_Motor_Z;
	EVENT_INFO EI_ReplyMotionStaus_Focus_Z;
	EVENT_INFO EI_ReplyMotionStaus_Zoom_Z;

};

struct INSP_PC_EVENT
{
	EVENT_INFO			EI_Grab_End[100];
	EVENT_INFO			EI_Classify_End[100];
	CRITICAL_SECTION	m_csLock;
	INSP_PC_EVENT()
	{
		InitializeCriticalSection(&m_csLock);
	}
	~INSP_PC_EVENT()
	{
		DeleteCriticalSection(&m_csLock);
	}
};


#define WAIT_BIT_ON_EVENT(EVENT_NAME, WAIT_TIME)																\
	{																											\
																												\
		{																										\
			HANDLE Event[2];																					\
			Event[0] = EVENT_NAME;																				\
			Event[1] = m_hSequenceResetEvent;																	\
																												\
			Sleep(10);																							\
			nRet = ::WaitForMultipleObjects(2, Event, FALSE, WAIT_TIME);										\
			if ( nRet == WAIT_OBJECT_0 + 1)																		\
				throw 9999;																						\
			if ( nRet == WAIT_TIMEOUT )																			\
				throw MAINPCINTERFACE_TASK_TIME_OUT;															\
																												\
		}																										\
	}

//다중 이벤틀 대기 매크로, 최대로 대기 할 수 있는 이벤트 개수는 10개다.
#define WAIT_MULTI_BITS_EVENT(EVENTS_ARRAY, WAIT_TIME)															\
	{																											\
		HANDLE Event[10] = {0,};																				\
		for ( int i = 0; i < WAIT_EVENT_CNT; i++)																\
		{																										\
			Event[i] = EVENTS_ARRAY[i];																			\
		}																										\
		Event[WAIT_EVENT_CNT] = m_hSequenceResetEvent;															\
																												\
		Sleep(10);																								\
		nRet = ::WaitForMultipleObjects(WAIT_EVENT_CNT+1, Event, FALSE, WAIT_TIME);								\
		if ( nRet == WAIT_OBJECT_0 + WAIT_EVENT_CNT)															\
			throw 9999;																							\
		if ( nRet == WAIT_TIMEOUT )																				\
			throw MAINPCINTERFACE_TASK_TIME_OUT;																\
	}


class WorkManager 
	: protected CThread
{

public:
	WorkManager();
	~WorkManager();

	int Start();

protected:
	virtual bool GetThreadRunFlag()
	{
		return false;
	}

	virtual int	CmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, ULONG PrmSize, USHORT TaskNo, byte* Buff, 
		USHORT uMsgType = CMD_TYPE_NORES, ULONG lMsgTimeout = CMD_TIMEOUT){return 0;}
	virtual int	ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg){return 0;}
	virtual void EndWorkProcess(BYTE* pMsg){return ;}					// Memory Pool の Memory 返納
	virtual int m_fnPrintLog(const wchar_t* pszLogData, ...){return 0;}
	virtual int m_fnPrintLog(const char* pszLogData, ...){return 0;}
	virtual BYTE* m_fnPeekMessage(){return 0;}

private:	
	void				m_fnRunThread();
	int					AnalyzeMsg(CMDMSG* pCmdMsg);
	HANDLE				m_hSequenceResetEvent;
	HANDLE				m_hWaitInspEndEvent;
	bool				m_bSeqResetFlag;
	UINT				m_SeqenceCount;
	MAIN_PC_REPLY_EVENT	m_stMainPcReplyEvent;
	INSP_PC_EVENT		m_stInspPcEvent;
	int					m_skMainPcSocket;
	int					m_ThetaCnt;

	
	CString             m_strCellId;

	//시작 라인
	int                 m_nStartLine;
	//라인 총 수
	int                 m_nTotalLine;

	// 2019.06.11 MDJ Grab Count 사용여부
	bool				m_bGrabCount;

	// 2019.06.07 MDJ 재시작 시퀀스 대응
	int					m_nGrabCount;
	CString*			m_strGrabCellID;
	// 2021.10.05~ MDJ Grab Count & Forward[Backward] direction for LCP
	int					m_nGrabCnt;
	int					m_nDirection;
	// 그랩 개수 제한 핸들
	HANDLE				m_hGrabSemaphore;

	CRITICAL_SECTION	m_csMainPcInterface;	

	CRITICAL_SECTION	m_csSequenceLock_1;
	CRITICAL_SECTION	m_csSequenceLock_2;
	CRITICAL_SECTION	m_csSequenceLock_3;
	CRITICAL_SECTION	m_csSequenceLock_4;
	CRITICAL_SECTION	m_csSequenceLock_5;

	CString				m_strJudge;			// 추가 2019.04.12 오병종

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/
	int Seq_TEST							(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_SeqeunceReset					(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int VS_TaskAlive						(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int	Seq_SendMessageToMainPC				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int	StartMainPcMsgReceiver				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int	Seq_StartGrab_FromMainPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_StartReady_FromMainPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ChangeRecipe_FromMainPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int	Seq_SendMsg_ClassifyEnd_FromInspPc	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int Seq_StageMove_FromInspPc            (byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	int Seq_ReqLineGrab_FromInspPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ReqCurrentRecipe_FromInspPc		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_OccurAlarm_FromInspPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_MoveMotion_FromInspPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_SetRecipeMotion_FromInspPc		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ReqVacuum_FromInspPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ReqMotionStatus_FromInspPc		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Cell Align 결과 송부 : Theta, Distance (셀 중심과 카메라 중심 거리) X, Y
	int	Seq_AlignResult_FromInspPc			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_PGChangePatternCmd_FromInspPc	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// AMT 설비 PG 제어 Command 추가 180801 YSS
	int	Seq_PGOffCmd_FromInspPc				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// AMT 설비 PG 제어 Command 추가 180801 YSS
	int	Seq_PGOnCmd_FromInspPc				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// AMT 설비 PG 제어 Command 추가 180822 YSS

	int	Seq_StartSimulation					(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	/************************************************************************/
	/*						Child Function	                                */
	/************************************************************************/
	int	Wait_Event_ClassifyEnd				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);


	/************************************************************************/
	/*						     Use Function                               */
	/************************************************************************/
	int	m_fnStartServer						(int nServerPort, int &nServerSocket);
	int	m_fnConnectServer					(int nServerPort, TCHAR* szServerIP, int &nMainPcSocket);
	int	m_fnSendMessageToMainPc				(char* szMessage, int nMsgSize);
	int m_fnReceiveMessage					(BYTE* buffer, int nBuffSize);
	int m_fnAnalyzeMainPcCmd				(char* buffer, int nBuffSize);
	int m_fnWakeUpSequence					(char* buffer, int nBuffSize);
};