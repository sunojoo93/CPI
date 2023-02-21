/** 
@addtogroup	VS64_COMMON
@{
*/ 

/**
@file		define_message.h
@brief		모든 Module 에서 사용하는 공용 구조체를 정의 한다.

@remark
@author		김용태
@date		2006.
*/

#pragma once

#pragma pack(push)
#pragma pack(8)
/** 
@brief		Client 간 메시지 전송에 사용되는 구조체이다.

@remark		
@author		김용태
@date		2006.
*/ 
struct CMDMSG
{

	///*********************************************************************************************************************************************************************************///
	INT64	pCmdMsg;			///< CMDMSG 가 초기 생성될때의 자기자신의 포인터, 메시지응답을 받은 후 원래 메지시에 회신 값을 넣어 주기 위함	**응용프로그램단에서는 사용하지 않는다.
	INT64	pRcvPacket;			///< 메시지가 리시브 되었을때 생성된 메모리 위치. 차후 삭제를 위해 사용한다.									**응용프로그램단에서는 사용하지 않는다.
	ULONG	uRcvPacketSize;		///< 받은 메시지의 총 패킷 사이즈. 차후 삭제를 위해 사용한다.													**응용프로그램단에서는 사용하지 않는다.
	INT64	hRspEvent;			///< 목적지 Task 에서 응답을 회신을 대기 할때 사용 하는 이벤트													**응용프로그램단에서는 사용하지 않는다.
	INT64	hSentEvent;			///< 목적지 Task 에 메시지 전송을 완료한 것을 대기할때 사용하는 이벤트											**응용프로그램단에서는 사용하지 않는다.
	ULONG	lUniqueNo;			///< 메시지에 대한 고유번호, 타임아웃 발생시 메시지가 존재하는지 판단하는데 사용								**응용프로그램단에서는 사용하지 않는다.
	ULONG	uCompressedMsgSize;	///< 압축된 Msg Buffer Size																						**응용프로그램단에서는 사용하지 않는다.
	INT64	cCompressedMsgBuf;	///< 압축된 Msg Buffer, 압축된 메모리의 포인터가 들어간다.														**응용프로그램단에서는 사용하지 않는다.
	///*********************************************************************************************************************************************************************************///


	USHORT	uStation_Src;		///< 근원지 Station No
	USHORT	uStation_Dest;		///< 목적지 Station No
	USHORT	uTask_Src;			///< 근원지 Task			
	USHORT	uTask_Dest;			///< 목적지 Task			
	USHORT	uFunID_Src;			///< 근원지 Function No		
	USHORT	uSeqID_Src;			///< 근원지 Sequence No	 
	USHORT	uUnitID_Src;		///< 근원지 Unit No			
	USHORT	uFunID_Dest;		///< 목적지 Function No		
	USHORT	uSeqID_Dest;		///< 목적지 Sequence No		
	USHORT	uUnitID_Dest;		///< 목적지 Unit No			
	USHORT	uRetStatus;			///< 반환시 수행 결과 값, 정상 : SEQ_OK, 비정상 : Error Code
	USHORT	uMsgType;			///< CMD_TYPE_RES, CMD_TYPE_NORES, 현재 메시지가 응답이 필요한지 유무를 나타낸다.
	USHORT	uMsgOrigin;			///< CMD_TYPE_REPLY, CMD_TYPE_CMD, 현재 메시지가 Response 인지 Command 인지를 나타낸다.
	UINT 	dwTimeOut;			///< 메시지 Time out, ms
	ULONG	uMsgSize;			///< 전송되는 메시지 Buffer Size, 2017.05.11 전체 메시지버퍼 사이즈를 입력한다.
	ULONG	uSendMsgSize;		///< 2017.05.11 전달할 메시지 버퍼 사이즈를 입력한다. response 대기 메시지의 경우 회신되는 메시지에서만 데이터를 전달받기 위해 사용. 패킷 사이즈를 줄일수 있다.
	USHORT	uMsgCompressMode;	///< 메시지 버퍼의 내용을 압축하여 전달할지 선택, 0:NoUse, 1:Compress, 2:Auto(10Mbyte 이상의 메시지버퍼는 자동 압축한다)
	INT64	cMsgBuf;			///< 메시지 Buffer


	CMDMSG()					///< 구조체 생성자, 초기 생성과 동시에 메모리를 초기화 한다.
	{
		memset(this, 0x00, sizeof(CMDMSG));
		pCmdMsg = (INT64)this;
		hRspEvent = NULL;
		hSentEvent = NULL;
		uMsgType = CMD_TYPE_RES;
		uRetStatus = SEQ_OK;
		uMsgOrigin = CMD_TYPE_CMD;
		dwTimeOut = CMD_TIMEOUT;
	}
	~CMDMSG()
	{
		/*if (cMsgBuf != NULL)
		{
		delete[] (BYTE*)cMsgBuf;
		}*/
	}
};

/** 
@brief		공유 메모리에 접근하기 위해 사용되는 구조체이다.

@remark	
@author		김용태
@date		2006.
*/ 
struct SMPMSG						
{
	USHORT	uStation;										///< 근원지 Station No (0 으로 설정하면 자동으로 자신의 Station No 로 입력된다.)
	USHORT	uTaskNo;										///< 근원지 Task No
	USHORT	uFuncNo;										///< 근원지 Function No, Server의 처리에는 필요하지 않다.. 참고 값으로 사용한다.
	USHORT	uFileNo;										///< 접근 할 File No
	USHORT	uAccessType;									///< 메모리 번지 직접 접근, ID를 통한 접근인가의 구분자
	USHORT	uStartPos;										///< 직접 접근시의 메모리 시작 지점
	USHORT	uLength;										///< 직접 접근시의 시작 지점으로 부터의 길이
	char	cAccessID[MAX_SHARED_MEMORY_ACCESS_ID_NAME];	///< ID 접근시의 ID name
	INT64	pcSMPBuf;										///< Parameter Buffer
	//BYTE*	cSMPBuf;										///< Parameter Buffer

	SMPMSG()												///< 구조체 생성자, 초기 생성과 동시에 메모리를 초기화 한다.
	{
		memset(this, 0x00, sizeof(SMPMSG));
	}
};

/** 
@brief		Client 에서 VS60LOGT로 LogData를 보내는 Log Level

@remark		Log Level 을 보고 Log 기록 Message 전송 전, Log Task 메시지 기록전 메시지의 처리를 차단 하게된다.
@author		최지형
@date		2006.10.23
*/ 
enum LOG_LEVEL
{
	LOG_LEVEL_ALWAYS = 0,	//MAX Log Level.
	LOG_LEVEL_1,
	LOG_LEVEL_2,
	LOG_LEVEL_3,
	LOG_LEVEL_4,			//MIN Log Level.
	LOG_LEVEL_NONE,
};

/** 
@brief		Log Task들의 List 를 가진다.

@remark		여러 Log Task 가 존재 하게 될 경우 필요하다.
@author		최지형
@date		2006.10.23
*/ 
enum LOG_TASK_LIST
{	
	DEFAULT_LOG_TASK = 60,
	APPEND_LOG_TASK_1,
	APPEND_LOG_TASK_2,
	APPEND_LOG_TASK_3,
	APPEND_LOG_TASK_4,
};

#pragma pack(pop)
/** 
@}
*/ 