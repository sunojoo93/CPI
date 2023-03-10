#pragma once


/*Server 에서 사용하는 공용 상수 정의*/
#define CLIENT_SOCKET_DLL_VERSION	1.21							// 서버에서 허용하는 Client Socket DLL 버전, Version 은 소수점 3자리까지만 사용해야한다.

#define	MAX_CLIENT_NAME_LEN			20							//Client 등록시 사용되는 최대 name 길이
#define	MAX_LOG_BUFFER				1024						//Log 기록시 사용하는 버퍼크기
#define MAX_LOG_FILE_PATH			200							//Log 파일 저장 경로의 최대 길이
#define MAX_LOG_FILE_NAME			50							//Log 파일 이름의 최대 길이

#define	MAX_SMP_ACCESSOR_LEN		15							//Access id 를 통한 공유 메모리 접근시 접근자의 최대 길이

#define	TIME_OUT_CLIENT_RESPONS		2000						//Client 응답에 대한 Time out 시간[ms]

#define TIME_OUT_WAIT_OBJECT		5000						//WaitForSingleObject 에서 이벤트 발생 대기시간

#define MAX_MEMORY_POOL				1000						//Send Buffer 로 사용할 Memory POOL 생성 개수

#define ERROR_RETURN(log,errorcode) if(OKAY != errorcode){m_fnWriteLog(log,errorcode);return errorcode;}

#define SAFE_CLOSE_SOCKET(p) if(p){ closesocket(p); (p) = 0; }	///< closesocket 문 재정의, 메모리가 할당 되었을 경우에만 동작됨

#define VS_SERVER_LOG_PATH		"D:\\LOG\\VSSERVER"			//Visual Station Server 의 로그 기입 경로