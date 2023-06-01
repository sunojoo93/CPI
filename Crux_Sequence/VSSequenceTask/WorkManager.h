#pragma once

#include "..\..\VisualStation\CommonHeader\Class\Thread.h"
#include "..\..\VisualStation\CommonHeader\Class\MessageQueue.h"
#include "..\..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "..\..\CommonHeaders\Structure.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 다른 Task No
#define VS_UI_TASK									10
#define VS_ALGORITHM_TASK							50  + (theApp.m_Config.GetPCNum() * 100)
#define VS_LIGHT_TASK								80  + (theApp.m_Config.GetPCNum() * 100)
#define VS_PG_TASK									40  + (theApp.m_Config.GetPCNum() * 100)
#define VS_CAMERA_TASK								90  + (theApp.m_Config.GetPCNum() * 100)
#define VS_MAIN_PC_TASK								60  + (theApp.m_Config.GetPCNum() * 100)
#define VS_OTHER_SEQ_TASK							21  + ((3 - theApp.m_Config.GetPCNum()) * 100)	/// 현재 PC 최대 2개로 가정하고 다른 PC 하나와 동기화. PC 개수 늘어날 경우 추가 작업 필요

// UI
#define SEND_CHANGE_MODEL							10, 1
#define SEND_GRAB_START								10, 2
#define SEND_UI_LOG									10, 3
#define SEND_CLASSIFY_END							10, 30
#define SEND_UI_NOTIFY_INIT							10, 90	
#define SEND_UI_TACT_TIME_DATA						10, 100	
#define SEND_UI_INSP_START							10, 10

// Algoithm
#define SEND_INSPECT_START							21, 1
#define SEND_GET_ALIGN_PATTERN_NUM					21, 10
#define SEND_START_ALIGN							21, 50

// Light
#define SEND_LIGHT_ON								80, 20
#define SEND_LIGHT_OFF								80, 30

// PG
#define SEND_PATTERN_ON_OFF							40, 11

#define SEND_CHANGE_PATTERN							40, 13
// #define SEND_CHANGE_VOLTAGE						40, 17
#define SEND_SEND_VOLTAGEINFO						40, 20
#define MAX_SEQUENCE_LCP							2	
#define SEND_MOVESTAGE								60, 60

// Camera
#define SEND_CAMERA_EXPOSE							90, 11
#define SEND_CAMERA_TRIGGER							90, 26

#define SEND_WAIT_CAMERA_GRAB_END					90, 12
#define SEND_WAIT_CAMERA_GRAB_END_SEQUENCE			90, 17
#define SEND_SET_CAMERA_COND						90, 30
#define SEND_SET_CAMERA_EXPOSE_TIME					90, 31	// 2021.12.15~ MDJ Modify Camera ExposeTime
#define SEND_SET_TRIGGER_MODE						90, 34
#define SEND_ALARM_OCCURRED							60, 52

// Main PC Interface
#define REQ_MOVE_MOTION								60, 53
#define REQ_MOVE_MOTION_PARAM1_SIZE					20			//Motor Type Name
#define REQ_GET_MOTION_STATUS						60, 56
#define REQ_GET_MOTION_STATUS_PARAM1_SIZE			20			//Motor Type Name
#define REQ_SWITCH_VACUUM							60, 55
#define REQ_SEND_LINE_GRAB_END						60, 50      //
#define REQ_CLASSIFY_END							60, 22
#define REQ_PG_CHANGE_PATTERN						60, 57		// PG Change Pattern	180801 YSS
#define REQ_PG_OFF									60, 58		// PG OFF				180801 YSS
#define REQ_PG_ON									60, 59		// PG OFF				180822 YSS

// Other PC Sequence Task
#define SEND_ASYNC_SEQUENCE							21, 88
#define SEND_SEQ_ALIVE								10, 1



#define MAX_GRAB_COUNT_LCP							2	
#define MAX_PC_COUNT								4			// 최대 PC 카운트 정의(시컨스 동기화 기능 적용시 사용)		181010 YSS
#define MAX_AUTO_SEQ_ASNC_POINT						30

#define SLEEP_TIME									500

//Share Img Num


// 알람 ID 정의
enum ENUM_ALARM_ID
{
	eALARM_MTP_SETTING_ERROR = 1501,
	eALARM_CAMERA_TRIGGER_ERROR = 1502,
	eALARM_MUST_BE_FIRST_GRAB = 1503,
	eALARM_GET_ALIGN_NUM_ERROR = 1504,
	eALARM_MUST_BE_ALIGN_GRAB = 1505,
	eALARM_PG_ON_ERROR = 1506,
	eALARM_FIRST_PATTERN_ERROR = 1507,
	eALARM_FIRST_CAMERA_ERROR = 1508,
	eALARM_FIRST_LIGHT_ERROR = 1509,
	eALARM_SWITCH_VACUUM_ERROR = 1510,
	eALARM_EXPOSE_ERROR = 1511,
	eALARM_NEXT_PG_ERROR = 1512,
	eALARM_NEXT_LIGHT_ERROR = 1513,
	eALARM_NEXT_CAMERA_ERROR = 1514,
	eALARM_WAIT_GRAB_END_ERROR = 1515,
	eALARM_START_INSPECTION_ERROR = 1516,
	eALARM_PG_OFF_ERROR = 1517,
	eALARM_LIGHT_OFF_ERROR = 1518,
	eALARM_WAIT_DUST_RESULT_ERROR = 1519,
	eALARM_ALL_PATTERNS_SKIPED = 1520,
	
	eALARM_ALIGN_GET_PATTERN_ERROR = 1531,
	eALARM_ALIGN_PG_ON_ERROR = 1532,
	eALARM_ALIGN_LIGHT_ON_ERROR = 1533,
	eALARM_ALIGN_CAMERA_ERROR = 1534,
	eALARM_ALIGN_VACUUM_ERROR = 1535,
	eALARM_ALIGN_EXPOSE_ERROR = 1536,
	eALARM_ALIGN_PG_OFF_ERROR = 1537,
	eALARM_ALIGN_LIGHT_OFF_ERROR = 1538,
	eALARM_ALIGN_GRAB_END_ERROR = 1539,
	eALARM_ALIGN_ALGORITHME_ERROR = 1540,
	eALARM_ALIGN_MOTION_ERROR = 1541,

	eALARM_AD_REPEAT_OCCUR		= 1200,
	eALARM_READ_FAIL_MTP_RESULT	= 2000,
};

// 알람 Type
enum ENUM_ALARM_TYPE
{
	eALARMTYPE_LIGHT			= 1,		// 경알람
	eALARMTYPE_HEAVY						// 중알람
};
using namespace std;

class WorkManager 
	: protected CThread
{

public:
	WorkManager();
	~WorkManager();

	int Start();	
	int VS_Send_Log_To_UI(TCHAR* strLog, int len);
	int VS_Send_Notify_Init_To_UI();
	int VS_Send_Alarm_To_MainPC(int nAlarmID, int nAlarmType);
	

protected:
	virtual bool GetThreadRunFlag()
	{
		return false;
	}

	virtual int	CmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, ULONG PrmSize, USHORT TaskNo, byte* Buff, 
							USHORT uMsgType = CMD_TYPE_NORES, ULONG lMsgTimeout = CMD_TIMEOUT){return 0;}

	virtual int	ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg){return 0;}
	virtual void EndWorkProcess(BYTE* pMsg){return ;}					// Memory Pool の Memory 返納
	virtual int m_fnPrintLog(BOOL bSendUI, const wchar_t* pszLogData, ...){return 0;}
	virtual int m_fnPrintLog(const char* pszLogData, ...){return 0;}
	virtual BYTE* m_fnPeekMessage(){return 0;}

private:
	void				m_fnRunThread();
	int					AnalyzeMsg(CMDMSG* pCmdMsg);
	int                 m_count = -1;
	int                 k = 1;
	HANDLE				m_hSequenceResetEvent;
	bool				m_bSeqResetFlag;
	UINT				m_SeqenceCount;

	HANDLE				m_hAsycAutoSeq[MAX_PC_COUNT][MAX_AUTO_SEQ_ASNC_POINT];		// 시컨스 동기화 기능 1차원 배열 -> 2차원 배열로 수정(PC Num 추가)	181010 YSS
	HANDLE				m_hDustEnd;
	HANDLE				m_hSeqStageMove[MAX_GRAB_COUNT_LCP][MAX_SEQUENCE_LCP];
	bool				m_bNeedRetry;		// Dust Retry 필요 여부
	bool				m_bIsNormalDust;	// Dust Check 결과 - From Alg.Task

	CRITICAL_SECTION	m_csSequenceLock_1;
	CRITICAL_SECTION	m_csSequenceLock_2;
	CRITICAL_SECTION	m_csSequenceLock_3;
	CRITICAL_SECTION	m_csSequenceLock_4;
	CRITICAL_SECTION	m_csSequenceLock_5;

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/
	// Test
	int Seq_TEST					(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Alive
	int	VS_TaskAlive				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Manual
	int	Seq_ManualChangeModel		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	/*int	Seq_ManualGrabImage			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualGrabImage_AVI		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualGrabImage_SVI		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualGrabImage_APP		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualGrabImage_AMT		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualGrabImage_DUMP     (byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	int	Seq_ManualInspectImage		(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);*/
	// Auto
	int	Seq_AutoInspectGrabImage	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_AutoInspectGrabImage_APP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_AutoInspectGrabImage_AVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_AutoInspectGrabImage_SVI(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_AutoInspectGrabImage_AMT(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// AMT 검사 시컨스 신규 추가 180801 YSS
	//int	Seq_AutoInspectGrabImage_BUMP(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// 2021.10.05~ MDJ Inspection Sequence for LCP
	int	Seq_AutoInspectGrabImage_AOT_CHIPPING(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// 2023.05.26 AOT CHIPPING JSO
	//int	Seq_AutoInspectGrabImage_ALM(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	// 2023.05.26 ALM JSO
	//int	Seq_AutoChangeModel			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);	
	// Sequence
	//int	Seq_ClassifyEnd				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_GUI_ClassifyEnd			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Camera Align
	//int	Seq_AlignCamera				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
		
	// Reset
	//int	Seq_SeqeunceReset			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	//Tact Time
	int Seq_TactTimeData			(TCHAR* strPanelId, CString strTactName, CString strTactState, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	// Async Sequence
	int	Seq_AsyncSequence			(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	// Receive Dust End
	//int	Seq_RcvCheckDustEnd				(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	// For Test
	//int	Seq_MoveMotion_MortorX_ForTest	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_MoveMotion_MortorY_ForTest	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_MoveMotion_MortorT_ForTest	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_MoveMotion_FocusZ_ForTest	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);
	//int	Seq_MoveMotion_ZoomZ_ForTest	(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode = false, bool bBusyCheck = false, bool bSeqResetPossible = true);

	/************************************************************************/
	/*						     Use Function                               */
	/************************************************************************/
	//int	m_fnSendMsgMoveMotion(CStringA strMotionType, int nRelativeMoveVal);
	//int	m_fnSendMsgGetMotionStatus(CStringA strMotionType);
	int	m_ShareImgNum(int nImgNum);
	
	//void SendStageMove(int nGrabCount, int nKindDirection, int nMotorSpeed);
	//bool WaitForStageMove(HANDLE(*pHandle)[MAX_SEQUENCE_LCP], int& nGrabCount, int& nKindDirection, DWORD dwMilliseconds = INFINITE);


	//int	m_fnSendMsgGetMotionStatus	(CStringA strMotionType);

// Light Info 정적 배열로 변경
	//void m_fnCreateProcInfo(TCHAR* pStrPanelID, TCHAR* pStrVirtualID, TCHAR* pStrModelID, int nStageNo, BOOL bIsManual = FALSE);	//2019.07.10 nStageNo

	// 다른 Sequence Task 동기화 함수
	bool WaitForOtherSequence(HANDLE (*pHandle)[MAX_AUTO_SEQ_ASNC_POINT], int& nAsyncCount, DWORD dwMilliseconds = INFINITE);

	// Alg. Task - Dust Image Check 대기 구문 작성
	//bool WaitForCheckDustAtAlgTask(DWORD dwMilliseconds = INFINITE);

	

	
};