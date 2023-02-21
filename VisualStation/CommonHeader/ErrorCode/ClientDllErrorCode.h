#pragma once

//Client DLL Error Code
#define	ERR_CLIENT_REGIST_BAD_ID			5001		//Client 등록 오류, Client ID 가 1~60000 을 벗어난다.
#define	ERR_CLIENT_REGIST_CALL_BACK			5002		//Client 등록 오류, Application Call back funtion 이 NULL 이다.
#define ERR_CLIENT_REGIST_CONNREFUSED		5003		//Client 등록 오류, Server 가 준비 되지 않았다.

#define ERR_CLIENT_NOT_REGISTERED			5004		//Client 가 서버에 등록되어 있지 않다.
#define	ERR_CLIENT_REGISTERED				5005		//Client 가 서버에 이미 등록이 되어있다.

#define	ERR_CLIENT_CMDMSG_IS_NULL			5006		//Client 로 전달할 데이터 구조체가 NULL이다
#define	ERR_CLIENT_CMDMSG_TYPE_IS_NOT_RES	5007		//Client 로 전달할 메시지가 RESPONS Type 이 아니다.

#define ERR_CLIENT_SMP_TIME_OUT				5010		//SMP 처리에 Time out 이 발생되었다.
#define ERR_CLIENT_MESSAGE_RSP_TIMEOUT		5011		//Clinet Message 응답 타임 아웃이 발생되었다.
#define ERR_CLIENT_MESSAGE_RSP_RESET		5012		//Clinet Message 응답 대기가 강제 정지 되었다.

#define	ERR_CLIENT_INVALID_PARAMETER		5020		//Parameter 의 값이 잘 못 됬다.

#define ERR_CLIENT_PARAM_BUFFER_IS_NULL		5030		//메시지 응답을 받아서 넣을 메모리 공간이 부족하다.