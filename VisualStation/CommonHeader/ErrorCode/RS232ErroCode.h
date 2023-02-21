/** 
@addtogroup	RS232_TASK
@{
*/ 

/**
@file		RS232ErrorCode.h
@brief		RS232 Task 의 Error Code 를 정의 함

@remark		
@author		김용태
@date		2007.05.18
*/

#pragma once

const int ERR_RS232_BASE_ERROR_NO			= 23000;
const int ERR_RS232_CMD_BUSY				= ERR_RS232_BASE_ERROR_NO + 1;	///< 실행 할 Command 가 이미 수행 중입니다.
const int ERR_RS232_CMD_INVALID_PARAMETER	= ERR_RS232_BASE_ERROR_NO + 2;	///< 유효하지 않은 파라메터입니다.
const int ERR_RS232_CMD_SEND_FAIL			= ERR_RS232_BASE_ERROR_NO + 3;	///< 회선 문제로 메시지 전송에 실패 했습니다.
const int ERR_RS232_CMD_INVALID_FUCTIONID	= ERR_RS232_BASE_ERROR_NO + 4;	///< 유효 하지 않은 Funtion ID 입니다.
const int ERR_RS232_CMD_INVALID_FUCTIONNO	= ERR_RS232_BASE_ERROR_NO + 5;	///< 유효 하지 않은 Funtion NO 입니다.
const int ERR_RS232_CMD_RESPONSE_TIMEOUT	= ERR_RS232_BASE_ERROR_NO + 6;	///< 응답 대기하는 시간을 초과 했습니다.
const int ERR_RS232_CMD_RESPONSE_DUPLICATE	= ERR_RS232_BASE_ERROR_NO + 7;	///< 응답 대기하는 메시지가 중복 됩니다.

const int ERR_RS232_CMD_TIMER_CREATE_FAIL	= ERR_RS232_BASE_ERROR_NO + 8;	///< Timer Out을 발생하는 Timer의 생성에 실패했습니다.


const int ERR_RS232_PORT_OPEN_FAIL			= ERR_RS232_BASE_ERROR_NO + 100;///< Port Open 을 실패 했습니다.
const int ERR_RS232_READ_DEFINE				= ERR_RS232_BASE_ERROR_NO + 101;///< 환경 설정 파일을 읽지 못했습니다.


/** 
@}
*/ 