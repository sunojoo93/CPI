/**
@file		ClientConstant.h
@brief		Client DLL 내부에서 사용하는 상수를 정의한다.	
@remark
@author		김용태
@date		2006.
*/

#pragma once

#define CLIENT_SOCKET_DLL_VERSION	1.21			///< Version 은 소수점 3자리까지만 사용해야한다.

#define PROCESSOR_MODE_SEND			0			///< Message Processor 의 Thread Message 처리 모드, Send Message 만 처리함을 의미한다. Message Queue 도 각각 분리 된다.
#define PROCESSOR_MODE_RECEIVE		1			///< Message Processor 의 Thread Message 처리 모드, Receive Message 만 처리함을 의미한다.

#define	SMP_MESSAGE_WAIT_TIME		30000		///< SMP Message 의 Time out 시간[ms]
#define	CMD_MESSAGE_WAIT_TIME		0			///< SMP Message 의 Time out 시간[ms]

#define	CMD_MESSAGE_SEND_WAIT_TIME	100000		///< Message Send 의 Time out 시간[ms]

#define AUTO_MSG_COMPRESS_SIZE		10000000	///< Msgbuffer 자동 압축을 사용하는 경우 10MB 이상의 경우 압축을 한다.

#define	VS64_SERVER_IP				"127.0.0.1"	///< Server IP
#define	VS64_PORT					5000		///< Server Port

#define ERROR_RETURN(log,errorcode) if(OKAY != errorcode){TRACE(log,errorcode);return errorcode;}
