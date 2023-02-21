/** 
@addtogroup	VS64_COMMON
@{
*/ 

/**
@file		CommonConstant.h
@brief		모든 Module 에서 사용하는 공용 상수를 정의한다.

@remark
@author		김용태
@date		2006.
*/

#pragma once



/** 
@defgroup	PACKET_CONST	Packet 및 CMD 및 SMP 구조체 생성 관련 상수
*/
/** 
@addtogroup	PACKET_CONST
@{
*/ 
//const int MAX_BUFFER							= (1024*15);	///< Send/Receive Buffer Size
const int MAX_BUFFER							= 419430400;	///< Send/Receive Buffer Size								// 2016.09.28 가변 사이즈 파라메터 전달 작업
const int MAX_CLIENT_MESSAGE_BUFFER				= (MAX_BUFFER - 1024);	///< Client Message 에서 사용하는 Parameter Buffer Size
const int MAX_SMP_BUFFER						= 1024;			///< SMP 에서 사용하는 Parameter Buffer Size				// 2016.09.28 가변 사이즈 파라메터 전달 작업
const int MAX_SHARED_MEMORY_FILE_NAME			= 20;			///< 공유메모리 접근시에 사용하는 최대 파일 이름 길이
const int MAX_SHARED_MEMORY_ACCESS_ID_NAME		= 30;			///< 공유메모리 접근시에 사용하는 최대 Access ID 길이
const int MAX_VS_COMMAND_PARAMETER_BUFFER		= 20;			///< Visual Station 명령에 대한 파라메터 버퍼 Size
const int MAX_VS_REGIST_CLIENT_NAME				= 20;			///< Visual Station 에 등록하는 Client 이름 길이
/** 
@}
*/ 

/** 
@defgroup	SMP_CONST	SMP 관련 상수
*/
/** 
@addtogroup	SMP_CONST
@{
*/ 
const int VS_MEMORY_READ						= 0;			///< 공유메모리 접근시 구분
const int VS_MEMORY_WRITE						= 1;			///< 공유메모리 접근시 구분
const int VS_MEMORY_ACCESS_ID					= 0;			///< 공유메모리 ID 를 통한 접근
const int VS_MEMORY_ACCESS_DIRECT				= 1;			///< 공유메모리 직접 접근
/** 
@}
*/ 

/** 
@defgroup	COMMAND_CONST	Command 전송 관련 상수
*/
/** 
@addtogroup	COMMAND_CONST
@{
*/ 
const int CMD_TYPE_RES							= 1;			///< Response
const int CMD_TYPE_NORES						= 2;			///< No Response
const int CMD_TYPE_REPLY						= 11;			///< Command Message 가 수행에 대한 결과를 응답 하는것임을 나타낸다.
const int CMD_TYPE_CMD							= 12;			///< Command Message 가 특정 Task로 보내서 수행을 필요로 하는 것임을 나타낸다.
const int CMD_TIMEOUT							= 6000;			///< Command 에 대한 Time Out, 
/** 
@}
*/ 

/** 
@defgroup	GENERAL_CONST	일반 상수
*/
/** 
@addtogroup	GENERAL_CONST
@{
*/  
const int MAX_IP_LENGTH							= 16;			///< IP 주소 최대 길이
const int DEFAULT_LOG_SIZE						= 512;			///< 기본 로그 사이즈.
const int READ_BUFFER							= 512;			///< 파일을 읽을때 사용하는 버퍼다. 한 라인에 대한 버퍼이다.

const int OKAY									= 1;			
const int NG									= 0;
const int SEQ_OK								= 0;			///< Sequence Task 에서 사용 되어지는 OK
const int SEQ_NG								= 0xFFFF;		///< Sequence Task 에서 사용 되어지는 NG
const int APP_OK								= 0;			///< Application 에서 사용하는 정상 응답
const int APP_NG								= 1;			///< Application 에서 사용하는 이상 응답
const int PROC_EXE_MODE_RUNTIME					= 1;			///< Client 실행 모드, Runtime mode 로 실행 한다
const int PROC_EXE_MODE_DEMO					= 2;			///< Demo Mode 로 실행한다.
/** 
@}
*/ 

/** 
@defgroup	GENERAL_MACRO	일반 MACRO
*/
/** 
@addtogroup	GENERAL_MACRO
@{
*/ 
//inline void SAFE_DELETE(void*p) 
//{if(p){ delete p; p =0; }}								
//inline void  SAFE_DELETE_ARR(void*p)
//{if(p){ delete []p; p=0;}}								
#define SAFE_DELETE(p)	if(p){ delete (p); (p)=0; }			///< delete 문 재정의, 메모리가 할당 되었을 경우에만 동작됨
#define SAFE_DELETE_ARR(p) if(p){ delete [](p); (p)=0;}		///< delete[] 문 재정의, 메모리가 할당 되었을 경우에만 동작됨

#ifndef TSTRING
#define TSTRING		basic_string<TCHAR> 
#endif
/** 
@}
*/

/** 
@}
*/ 