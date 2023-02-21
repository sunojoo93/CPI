#pragma once

//Internal Server Error Code
#define	ERR_INSERVER_REGIST_CLIENT_ID_DUPLICATION			4001			//Client ID 가 중복이다.
#define ERR_INSERVER_REGIST_CLIENT_NG						4002			//Client 등록 실패
#define ERR_INSERVER_REGIST_CLIENT_SOCKET_DLL_INCORRECT		4003			//Client Socket DLL 의 버전이 맞지 않다.

#define ERR_INSERVER_RECEIVE_PACKET_DATA_IS_NULL	4010			//받은 패킷 내용이 없다.