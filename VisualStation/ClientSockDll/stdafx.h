
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//
#pragma once


#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.
#include <afx.h>
#include <WinSock2.h>			//Winsock 2 정의 파일
#include <process.h>			//Thread 사용을 위함
#include <MMSystem.h>			//Multi Media Timer 사용을 위함

#include "..\CommonHeader\Constant\CommonConstant.h"				//Client/Server 공용 상수
#include "..\CommonHeader\Structure\CommonStructure.h"				//Client/Server 공용 구조체
#include "..\CommonHeader\Structure\Define_Message.h"				//Client, Application 에서 사용되어지는 메시지구조체
#include "..\CommonHeader\ErrorCode\InternalServerErrorCode.h"		//Internal Server Error Code 정의
#include "..\CommonHeader\ErrorCode\ClientDllErrorCode.h"								//Client DLL Error Code 정의
#include "ClientConstant.h"									//Client 에서 사용되어지는 상수
#include "ClientStructure.h"								//CLient 에서 사용되는 구조체
#include "..\CommonHeader\Class\MessageQueue.h"				//Message Queue
#include "..\CommonHeader\Class\MemPooler.h"					//Memory Pool