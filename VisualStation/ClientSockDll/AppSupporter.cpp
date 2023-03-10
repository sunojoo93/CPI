#include "StdAfx.h"
#include "appsupporter.h"

#include "MessageProcessor.h"
#include "MessageReceiver.h"
#include "snappy-c.h"

#include <mstcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning( disable : 4996 )	//컴파일 경고 메시지를 무시하게 한다. <~이(가) deprecated로 선언되었습니다.>메시지를 무시한다. .net2005 환경 이후에는 지원되지 않을 가능성이 있다.

/*
*	Module name	:	CAppSupporter
*	Parameter	:	Server IP, Port No, Send Message Queue Pointer, Receive Message Quque pointer
*					Socket 연결 유무 변수 Pointer
*	Return		:	없음
*	Function	:	생성자
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.1
*/
CAppSupporter::CAppSupporter()
{
	InitializeCriticalSection(&m_csAppSupporter);

	//멤버 변수 설정
	
	
	//멤버 변수 초기화
	m_uTaskNo					= 0;
	m_pMessageReceiver			= NULL;
	m_hClientSocket				= INVALID_SOCKET;
	m_hResponseEmergencyStop	= CreateEvent(NULL, TRUE, FALSE, NULL);
	m_nWaitResponseCount		= 0;

	m_lUniqueNo					= 0;
	m_bIsClientDLLStarted		= false;

	//메시지 처리자의 생성 개수를 설정 한다.
	/*{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
	}*/
}

/*
*	Module name	:	~CAppSupporter
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸자
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
CAppSupporter::~CAppSupporter(void)
{
	//Client 등록 해지 기능을 요청한다.
	m_fnUnRegisterTask();

	//
	CloseHandle(m_hResponseEmergencyStop);

	//임계영역 해지
	LeaveCriticalSection(&m_csAppSupporter);
}

/*
*	Module name	:	m_fnRegisterTask
*	Parameter	:	Client ID, Task name, Message receive function pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client 를 Visual Station 에 등록한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.1
*/
int CAppSupporter::m_fnRegisterTask(USHORT uTaskNo, char* pszTaskName, 
									CMessageQueue<BYTE*>* pAppPeekMessageQueue, char* pszServerIp, WORD wPortNo)
{
	int			nRet;
	int			nErrorCode = OKAY;

	EnterCriticalSection(&m_csAppSupporter);
	//맴버 변수 설정
	{
		m_uTaskNo = uTaskNo;
		strcpy(m_szClientName, pszTaskName);
		strcpy(m_szServerIP, pszServerIp);
		m_wPortNo = wPortNo;
		m_pAppPeekMessageQueue = pAppPeekMessageQueue;
	}

	//Socket Lib 초기화
	{
		WSADATA		wsaData;
		nRet = WSAStartup(MAKEWORD(2,2), &wsaData);
		if(0 != nRet)
		{
			nErrorCode	= WSAGetLastError();
			TRACE("CAppSupporter::m_fnRegisterTask -> WSAStartup Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
	}
	//Client Socket  생성 후 연결 한다.
	{

		//Client Socket을 생성한다.
		m_hClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//m_hClientSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		if (INVALID_SOCKET == m_hClientSocket)
		{
			nErrorCode	= WSAGetLastError();
			TRACE("CClientListener::m_fnStartServer -> WSASocket Error : %d", nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;		
		}


		SOCKADDR_IN ServerAddr;

		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(m_wPortNo);
		ServerAddr.sin_addr.s_addr = inet_addr(m_szServerIP);

		//Server에 연결한다.
		nRet = connect(m_hClientSocket, (SOCKADDR*) &ServerAddr, sizeof(ServerAddr));
		//nRet = WSAConnect(m_hClientSocket, (SOCKADDR*) &ServerAddr, sizeof(ServerAddr),NULL,NULL,NULL,NULL);
		if (SOCKET_ERROR == nRet)
		{
			nErrorCode = WSAGetLastError();
			TRACE("CAppSupporter::m_fnRegisterTask -> connect Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}

		int opt_val = TRUE;
		setsockopt(m_hClientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(opt_val));

		////Socket 의 유효성을 검사하게 한다. 20000ms 동안 아무런 메시지 전송이 없었다면 10000 ms 마다 socket 유효성 체크를 하게한다.
		//tcp_keepalive keepAlive = { TRUE, 20000, 10000};

		//DWORD nBytesReturned;

		//if(SOCKET_ERROR==WSAIoctl(m_hClientSocket, SIO_KEEPALIVE_VALS, &keepAlive, sizeof(keepAlive) ,0, 0, &nBytesReturned, NULL, NULL ))
		//{
		//	nErrorCode	= WSAGetLastError();
		//	TRACE("CClientListener::m_fnRunThread -> WSAIoctl Error : %d", nErrorCode);
		//	return nErrorCode;
		//}
	}

	//Server 의 Client 등록 요청 Packet 을 Receive 한다.
	{
		BYTE				buffer[1024];
		PACKET_HEADER*		pPacketHeader;
		PACKET_VS_COMMAND*	pPacketVsCommand;
		
		memset(buffer, 0, 1024);
		
		//Server 로 부터 Packet 을 받는다.
		nRet = m_fnReceiveMessage(buffer, sizeof(PACKET_HEADER)+sizeof(PACKET_VS_COMMAND));
		if(OKAY != nRet)
		{
			nErrorCode = nRet;
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage Error : %d\n",nRet);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}

		//받은 data 중 Header 부의 위치를 찾는다.
		pPacketHeader = (PACKET_HEADER*)buffer;
		//Header 의 내용중 Packet Data Type 이 Visual Station 에서 Client로 보내는 메시지 가 아니면 
		if(pPacketHeader->PacketDataType != VS_COMMAND_DATA)
		{
			//Error 반환한다.
			nErrorCode = NG;
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketHeader->PacketDataTyp)Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
		//받은 Data 중 Data 부분의 위치를 찾는다.
		pPacketVsCommand = (PACKET_VS_COMMAND*)(buffer + sizeof(PACKET_HEADER));
		//Data 의 내용중 VS command 의 종류가 Client 로 Client ID 전송 요청이 아니면
		if(pPacketVsCommand->VsCommand != VS_COMMAND_CLIENT_ID_REQUEST)
		{
			//Error 반환한다.
			nErrorCode = NG;
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketVsCommand->VsCommand) Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
	}

	//Client ID, name을 Server 로 전송한다.
	{
		BYTE						buffer[1024];
		PACKET_HEADER*				pPacketHeader;
		PACKET_CLIENT_VS_REGIST*	pPacketClientVsRegist;
		
		memset(buffer, 0, 1024);
		
		//Buffer 에서 Packet Header 와 body(PACKET_CLIENT_VS_REGIST) 의 시작 포인터를 가지고 온다.
		pPacketHeader			= (PACKET_HEADER*)buffer;
		pPacketClientVsRegist	= (PACKET_CLIENT_VS_REGIST*)(buffer + sizeof(PACKET_HEADER));
		
		//Header 정보를 설정 한다.
		pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_VS_REGIST);
		pPacketHeader->PacketDataType	= CLIENT_VS_REGIST_DATA;
		pPacketHeader->uDataSize		= sizeof(PACKET_CLIENT_VS_REGIST);
		
		//Body(PACKET_CLIENT_VS_REGIST) 정보를 설정한다.
		pPacketClientVsRegist->uClientID = m_uTaskNo;
		strcpy(pPacketClientVsRegist->cClientName, m_szClientName);
		pPacketClientVsRegist->uClientSocketDLLVersion = (UINT)(CLIENT_SOCKET_DLL_VERSION * 1000);

		//Packet 을 전송한다.
		nRet = m_fnSendMessage(buffer);
		if(OKAY != nRet)
		{
			nErrorCode = nRet;
			TRACE("CAppSupporter::m_fnRegisterTask -> WSAStartup Error : %d\n",nRet);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
	}

	//Client 등록 결과를 Receive 한다. 만일 다른 Packet 을 받았거나, 이상 등록이라면 결과를 반환 한다.
	{
		BYTE				buffer[1024];
		PACKET_HEADER*		pPacketHeader;
		PACKET_VS_COMMAND*	pPacketVsCommand;

		memset(buffer, 0, 1024);

		//Server 로 부터 Packet 을 받는다.
		nRet = m_fnReceiveMessage(buffer, sizeof(PACKET_HEADER)+sizeof(PACKET_VS_COMMAND));
		if(OKAY != nRet)
		{
			nErrorCode = nRet;
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage Error : %d\n",nRet);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}

		//받은 data 중 Header 부의 위치를 찾는다.
		pPacketHeader = (PACKET_HEADER*)buffer;
		//Header 의 내용중 Packet Data Type 이 Visual Station 에서 Client로 보내는 메시지 가 아니면 
		if(pPacketHeader->PacketDataType != VS_COMMAND_DATA)
		{
			//Error 반환한다.
			nErrorCode = NG;
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketHeader->PacketDataTyp)Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
		//받은 Data 중 Data 부분의 위치를 찾는다.
		pPacketVsCommand = (PACKET_VS_COMMAND*)(buffer + sizeof(PACKET_HEADER));
		//Data 의 내용중 VS command 의 종류가 Client 로 Client ID 정상등록 통보가 아니면
		if(pPacketVsCommand->VsCommand == VS_COMMAND_REGIST_CLIENT_ID_DUPLICATE)
		{
			//Error 반환한다.
			nErrorCode = ERR_INSERVER_REGIST_CLIENT_ID_DUPLICATION;
			closesocket(m_hClientSocket);
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketVsCommand->VsCommand) Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
		else if (pPacketVsCommand->VsCommand == VS_COMMAND_REGIST_CLIENT_DLL_INCORRECT)
		{
			//Error 반환한다.
			nErrorCode = ERR_INSERVER_REGIST_CLIENT_SOCKET_DLL_INCORRECT;
			closesocket(m_hClientSocket);
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketVsCommand->VsCommand) Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
		else if(pPacketVsCommand->VsCommand != VS_COMMAND_REGIST_CLIENT_OK)
		{
			//Error 반환한다.
			nErrorCode = ERR_INSERVER_REGIST_CLIENT_NG;
			closesocket(m_hClientSocket);
			TRACE("CAppSupporter::m_fnRegisterTask -> m_fnReceiveMessage data(pPacketVsCommand->VsCommand) Error : %d\n",nErrorCode);
			LeaveCriticalSection(&m_csAppSupporter);
			return nErrorCode;
		}
	}
	
	//Socket 연결 유무 Flag 를 true 로 설정한다.
	m_bSocketConnect = true;

	//Send Message Processor, Receive Message Processor를 각각 생성하고, Pointer 를 리스트로 가지고 있는다.
	//Client application Callback function 등록도 이루어 진다.
	{
		CMessageProcessor* pSMProcessor, *pRMProcessor;

		pSMProcessor = new CMessageProcessor(m_uTaskNo, &m_ReceiveMessageQueue, m_pAppPeekMessageQueue, PROCESSOR_MODE_RECEIVE, m_hClientSocket, &m_bSocketConnect);
		pRMProcessor = new CMessageProcessor(m_uTaskNo, &m_SendMessageQueue, m_pAppPeekMessageQueue, PROCESSOR_MODE_SEND, m_hClientSocket, &m_bSocketConnect);
			
		pSMProcessor->m_fnStartMessageProcessor();
		pRMProcessor->m_fnStartMessageProcessor();

		m_deqMessageProcessor.push_back(pSMProcessor);
		m_deqMessageProcessor.push_back(pRMProcessor);
	}	

	//Message Receiver 생성 및 메시지 수신자를 시작한다.
	{
		m_pMessageReceiver = new CMessageReceiver(&m_ReceiveMessageQueue, m_hClientSocket, m_pAppPeekMessageQueue, &m_bSocketConnect);
		m_pMessageReceiver->m_fnStartReceiver();
	}


	LeaveCriticalSection(&m_csAppSupporter);

	return nErrorCode;
}

/*
*	Module name	:	m_fnUnRegisterTask
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Visual Station에서 등록 해지 한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnUnRegisterTask()
{
	int nErrorCode = OKAY;

	EnterCriticalSection(&m_csAppSupporter);

	//메시지 수신자 Thread 를 종료하고 삭제한다.
	SAFE_DELETE(m_pMessageReceiver);

	//메시지 처리자 Thread 들을 종료하고 삭제한다.
	{
		for(;;)
		{
			CMessageProcessor* pProcessor = NULL;

			if(!m_deqMessageProcessor.empty())
			{
				pProcessor = m_deqMessageProcessor.front();
				m_deqMessageProcessor.pop_front();
			}
			else
				break;

			if(pProcessor)
			{
				pProcessor->m_fnStopMessageProcessor();
				SAFE_DELETE(pProcessor);
			}
			else
				break;
		}
	}

	//Client Socket 을 닫는다.
	if(INVALID_SOCKET != m_hClientSocket)
	{
		closesocket(m_hClientSocket);
	}

	//Socket 연결 유무 Flag 를 False 로 한다.
	m_bSocketConnect = false;

	LeaveCriticalSection(&m_csAppSupporter);

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendCommand
*	Parameter	:	CMDMSG Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Visual Station에 Client Message 전달을 하기 위한 기능을 담당한다.
*					메시지 큐에 Packet 을 만들어서 넣는다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnSendCommand(CMDMSG* pCmdMsg)
{
	int						nRet;
	int						nErrorCode = OKAY;
	BYTE*					pcBuffer;
	PACKET_HEADER*			pPacketHeader;
	PACKET_CLIENT_MESSAGE*	pPacketClientMessage;

	nRet = m_fnCompressMsg(pCmdMsg);
	if (nRet != APP_OK)
	{
		ERROR_RETURN("CAppSupporter::m_fnSendCommand -> m_fnCompressMsg Error\n",nRet);
	}
	
	EnterCriticalSection(&m_csAppSupporter);

	//응답이 필요한 메시지 이면 대기이벤트 핸들을 만든다. 
	if(pCmdMsg->uMsgType == CMD_TYPE_RES)
	{
		pCmdMsg->hRspEvent = (INT64)CreateEvent(NULL, TRUE, FALSE, NULL);
		if (m_lUniqueNo >= 0xFFFFFFFF) m_lUniqueNo = 0;
		pCmdMsg->lUniqueNo = ++m_lUniqueNo;
	}
	
	pCmdMsg->hSentEvent		= (INT64)CreateEvent(NULL, TRUE, FALSE, NULL);

	pcBuffer				= new BYTE[sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE)];			// 2016.09.28 가변 사이즈 파라메터 전달 작업
	
	//Buffer 에서 Packet 의 Header 와 Body 의 시작 Pointer 를 구한다.
	pPacketHeader			= (PACKET_HEADER*)pcBuffer;
	pPacketClientMessage	= (PACKET_CLIENT_MESSAGE*)(pcBuffer + sizeof(PACKET_HEADER));

	//Message Header 정보를 설정한다.

	pPacketHeader->PacketDataType	= CLIENT_MESSAGE_DATA;
	
	
	//Message Body 정보를 설정한다.
	pPacketClientMessage->uStationNo1	= 0;								//자신의 Station No.. 0으로 입력 되면 Server Receive 단에서 자동으로 자신의 Station No 로 바꿔준다.
	pPacketClientMessage->uStationNo2	= pCmdMsg->uStation_Dest;
	pPacketClientMessage->uTaskNo1		= pCmdMsg->uTask_Src;
	pPacketClientMessage->uTaskNo2		= pCmdMsg->uTask_Dest;
	pPacketClientMessage->dwTimeOut		= pCmdMsg->dwTimeOut;
	//pPacketClientMessage->uFuncNo		= pCmdMsg->uFunID;
	//pPacketClientMessage->uSeqNo		= pCmdMsg->uSeqID;

	pPacketHeader->uFullMemorySize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uMsgSize;

	if (pCmdMsg->uCompressedMsgSize > 0)
	{
		pPacketHeader->uFullMemorySize			= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uCompressedMsgSize;
		pPacketHeader->uPacketSize				= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uCompressedMsgSize;	
		pPacketHeader->uDataSize				= sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uCompressedMsgSize;							
		pPacketClientMessage->uParamBufSize		= (sizeof(CMDMSG) + pCmdMsg->uCompressedMsgSize);
	}
	else if (pCmdMsg->uSendMsgSize > 0)
	{
		pPacketHeader->uPacketSize				= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uSendMsgSize;	
		pPacketHeader->uDataSize				= sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uSendMsgSize;							
		pPacketClientMessage->uParamBufSize		= (sizeof(CMDMSG) + pCmdMsg->uSendMsgSize);
	}
	else
	{
		pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uMsgSize;	
		pPacketHeader->uDataSize		= sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + pCmdMsg->uMsgSize;							
		pPacketClientMessage->uParamBufSize = (sizeof(CMDMSG) + pCmdMsg->uMsgSize);
	}
	
	pPacketClientMessage->uSendOkFlag	= OKAY;
	pPacketClientMessage->pParamBuf		= (INT64)pCmdMsg;							// 2016.09.28 가변 사이즈 파라메터 전달 작업
	// 2016.09.28 가변 사이즈 파라메터 전달 작업  No use  S ----
	//memcpy(pPacketClientMessage->cParamBuf, pCmdMsg, sizeof(CMDMSG));		//CMDMSG 자체를 파라메터로쓰는 이유는 CMDMSG 구조체가 패킷구조체와 틀려서 
																			//body로 쓸수 없기 때문이다. body 자체는 Server 와 Client dll 에서만 참고 되어진다.
																			//현재 명령을 Receive 하는 Task 에서는 Application 으로 Callback 을 수행 되어 질때
																			//넘기는 구조체는 pPacketClientMessage->cParamBuf 의 pCmdMsg 이다.
																			//이후 실제 CMDMSG구조체에서 사용하는 Parameter 가 입력된다.
	//if(pCmdMsg->cMsgBuf != NULL)
	//	memcpy(pPacketClientMessage->cParamBuf + sizeof(CMDMSG), (void*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);	//Application 에서 사용하는 파라메터 최대 크기를 알아야 오류를 방지 할 수 있다..Work Item.. Todo
	//																									//uMsgSize 가 MAX_BUFFER-1204 보다 클경우 예외처리 필요 Todo..
	//	 // Paramerter buffer 가 NULL 인경우 사이즈는 0
	//else
	//	pCmdMsg->uMsgSize = 0;

	// 2016.09.28 가변 사이즈 파라메터 전달 작업 No use  E ----

	//TRACE("AppSupporter : %s\n",pCmdMsg->cMsgBuf);
	
	//Send Message Queue 에 메시지를 삽입 한다.
	nRet = m_SendMessageQueue.m_fnPushMessage(pcBuffer);

	//응답이 필요한 메시지 이면 대기 이벤트를 기다린다.
	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
	{
		HANDLE Event[2];
		Event[0] = (HANDLE)pCmdMsg->hRspEvent;
		Event[1] = m_hResponseEmergencyStop;

		m_nWaitResponseCount++;

		LeaveCriticalSection(&m_csAppSupporter);

		nRet = WaitForMultipleObjects(2, Event, FALSE, pCmdMsg->dwTimeOut);

		EnterCriticalSection(&m_csAppSupporter);

		m_nWaitResponseCount--;

		//만약 비상정지 이벤트가 활성화 되었으면 
		if(nRet == WAIT_OBJECT_0 + 1)
		{
			//pPacketClientMessage->uTaskNo2 = 0;		// 에러시 서버로 메시지를 전달하지 않도록 한다.
			//응답 대기 중인 커맨드가 없으면  비상정지용 이벤트를 Reset한다.
			if(m_nWaitResponseCount <= 0)
				ResetEvent(m_hResponseEmergencyStop);
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_RESET;
		}
		else if(nRet == WAIT_TIMEOUT)
		{
			//pPacketClientMessage->uTaskNo2 = 0;		// 에러시 서버로 메시지를 전달하지 않도록 한다.
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_TIMEOUT;
		}
		else
		{
			if ( pCmdMsg->uRetStatus == ERR_CLIENT_PARAM_BUFFER_IS_NULL)
			{
				nErrorCode = ERR_CLIENT_PARAM_BUFFER_IS_NULL;
			}
			else
				nErrorCode = OKAY;
		}
		CloseHandle((HANDLE)pCmdMsg->hRspEvent);
		pCmdMsg->lUniqueNo = 0;
		
	}
	else
	{
		HANDLE Event[2];
		Event[0] = (HANDLE)pCmdMsg->hSentEvent;
		Event[1] = m_hResponseEmergencyStop;

		m_nWaitResponseCount++;

		LeaveCriticalSection(&m_csAppSupporter);

		nRet = WaitForMultipleObjects(2, Event, FALSE, CMD_MESSAGE_SEND_WAIT_TIME);

		EnterCriticalSection(&m_csAppSupporter);

		m_nWaitResponseCount--;

		//만약 비상정지 이벤트가 활성화 되었으면% 
		if(nRet == WAIT_OBJECT_0 + 1)
		{
			//응답 대기 중인 커맨드가 없으면  비상정지용 이벤트를 Reset한다.
			if(m_nWaitResponseCount <= 0)
				ResetEvent(m_hResponseEmergencyStop);
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_RESET;
		}
		else if(nRet == WAIT_TIMEOUT)
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_TIMEOUT;
		else
		{
			if ( pCmdMsg->uRetStatus == ERR_CLIENT_PARAM_BUFFER_IS_NULL)
			{
				nErrorCode = ERR_CLIENT_PARAM_BUFFER_IS_NULL;
			}
			else
				nErrorCode = OKAY;
		}

	}

	CloseHandle((HANDLE)pCmdMsg->hSentEvent);

	LeaveCriticalSection(&m_csAppSupporter);

	if(pCmdMsg->uCompressedMsgSize > 0)
	{
		if (pCmdMsg->cCompressedMsgBuf)
		{
			delete[] (char*)pCmdMsg->cCompressedMsgBuf;
			pCmdMsg->cCompressedMsgBuf = 0;
		}
		pCmdMsg->uCompressedMsgSize = 0;
	}
	
	ERROR_RETURN("CAppSupporter::m_fnSendCommand -> m_pMessageQueue.AddMessage Error\n",nErrorCode);

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendCommandReset
*	Parameter	:	
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	현재 대기 중인 모든 커맨드를 정지시킨다.
*					비상정지용
*	Create		:	2014.08.29
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnSendCommandReset()
{
	EnterCriticalSection(&m_csAppSupporter);

	if(m_nWaitResponseCount >= 1)
		SetEvent(m_hResponseEmergencyStop);

	LeaveCriticalSection(&m_csAppSupporter);
	
	return OKAY;
}

// 다른 Task 에서 받은 CMDMSG 의 메모리를 소멸시킨다.
int CAppSupporter::m_fnFreeMemory(BYTE* rcvPacket)
{

	PACKET_CLIENT_MESSAGE* pPacketClientMessage = (PACKET_CLIENT_MESSAGE*)(rcvPacket + sizeof(PACKET_HEADER));
	CMDMSG* pCmdmsg = (CMDMSG*)pPacketClientMessage->pParamBuf;


	if (pCmdmsg->uCompressedMsgSize > 0)
	{
		// 받은 커맨드가 압축되서 받은 커맨드이므로, cMsgBuf 가 새로생성하여 만들어진 메모리 이다. 압축해제를 위해 사용한 버퍼를 반환한다.

		if (pCmdmsg->cMsgBuf)
		{
			delete[] (char*)pCmdmsg->cMsgBuf;
			pCmdmsg->cMsgBuf = 0;
		}
	}

	delete[] rcvPacket;
	return OKAY;
}

/*
*	Module name	:	m_fnReceiveMessage
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	서버로부터 메시지를 수신대기 하고, 수신 완료 된 Packet 을 Buffer 에 넣는다. 초기 VS64 등록 과정에만 사용 된다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnReceiveMessage(BYTE* pszBuffer, int buffSize)
{
	int						nRet;
	int						nErrorCode		= OKAY;
	ULONG						nReceiveOffset	= 0;
	PACKET_HEADER*			pPacketHeader;

	//구간 반복 한다.
	while(1)
	{
		//Server 로부터 Packet 을 임시 버퍼로 수신한다.
		nRet = recv(m_hClientSocket, (char*)pszBuffer + nReceiveOffset, buffSize - nReceiveOffset, 0);
		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			//이상 종료 한다.
			nErrorCode = WSAGetLastError();
			TRACE("CAppSupporter::m_fnReceiveMessage -> recv error : %d\n", nErrorCode);
			return nErrorCode;
		}
		//만일 수신한 Packet 이 Header 구성 Size 보다 작다면 
		if(sizeof(PACKET_HEADER) > nReceiveOffset + nRet)
		{ 
			//총 받은 크기를 저장한다.
			nReceiveOffset += nRet;
			//다시 받기 시도한다.
			continue;
		}
		
		//이하 최소 Header Size 만큼 받은 경우이다.
		pPacketHeader = (PACKET_HEADER*)pszBuffer;

		//만일 수신한 Packet 이 실제 필요한 전체 Packet 크기 보다 작다면
		if(pPacketHeader->uPacketSize > nReceiveOffset + nRet)
		{	
			//총 받은 크기를 저장한다.
			nReceiveOffset += nRet;
			//다시 받기 시도한다.
			continue;
		}
		//만일 수신한 Packet 이 실제 필요한 전체 Packet 크기 보다 크다면
		if(pPacketHeader->uPacketSize < nReceiveOffset + nRet)
		{
			//이상 종료한다.
			TRACE("CAppSupporter::m_fnReceiveMessage -> Recv Size is BIG.\n");
			return NG;
		}

		//만일 수신한 Pacekt 이 실제 필요한 만큼 받았다면
		//구간 반복을 빠져나온다.
		break;
	}	
	
	return nErrorCode;
}

/*
*	Module name	:	m_fnSendMessage
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Message를 Server 로 전송한다. 초기 VS64 등록 과정에만 사용 된다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnSendMessage(BYTE* pszBuffer)
{
	int						nRet;
	int						nErrorCode = OKAY;
	ULONG					nSendOffset	= 0;
	ULONG					uSendSize;
	PACKET_HEADER*			pPacketHeader;

	//총 패킷 사이즈를 구한다.
	pPacketHeader	= (PACKET_HEADER*)pszBuffer;
	uSendSize		= pPacketHeader->uPacketSize;

	//구간 반복 한다.
	while(1)
	{
		//Server 로 Packet 전송을 한다.
		nRet = send(m_hClientSocket, (char*)pszBuffer + nSendOffset, uSendSize - nSendOffset, 0);
		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			//이상 종료 한다.
			nErrorCode = WSAGetLastError();
			TRACE("CAppSupporter::m_fnSendMessage -> send error : %d\n", nErrorCode);
			return nErrorCode;
		}

		//만일 송신한 Packet 이 실제 필요한 전체 Packet 크기 보다 작다면
		if(uSendSize > nSendOffset + nRet)
		{	
			//총 송신 크기를 저장한다.
			nSendOffset += nRet;
			//다시 송신 시도한다.
			continue;
		}
		//만일 송신한 Pacekt 이 실제 필요한 만큼 전송 되었다면
		//구간 반복을 빠져나온다.
		break;
	}	

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendResponsCommand
*	Parameter	:	CMDMSG Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Visual Station에 Client Message 전달을 하기 위한 기능을 담당한다.
*					응답 회신을 위함.
*	Create		:	2014.08.29
*	Author		:	김용태
*	Version		:	1.0
*/
int CAppSupporter::m_fnSendResponseCommand(CMDMSG* pSrcCmdMsg)
{
	int						nRet;
	int						nErrorCode = OKAY;
	BYTE*					pcBuffer;
	PACKET_HEADER*			pPacketHeader;
	PACKET_CLIENT_MESSAGE*	pPacketClientMessage;
	CMDMSG					CopyCmdMsg;

	if(pSrcCmdMsg == NULL)
	{
		ERROR_RETURN("CAppSupporter::m_fnSendResponseCommand -> ERR_CLIENT_CMDMSG_IS_NULL", ERR_CLIENT_CMDMSG_IS_NULL);
	}
	else if(pSrcCmdMsg->uMsgType == CMD_TYPE_NORES)
	{
 		ERROR_RETURN("CAppSupporter::m_fnSendResponseCommand -> ERR_CLIENT_CMDMSG_IS_NULL", ERR_CLIENT_CMDMSG_TYPE_IS_NOT_RES);;
	}

	CopyCmdMsg = *pSrcCmdMsg;
	CopyCmdMsg.uCompressedMsgSize = 0;
	CopyCmdMsg.cCompressedMsgBuf = NULL;
	CopyCmdMsg.uSendMsgSize = 0;

	nRet = m_fnCompressMsg(&CopyCmdMsg);
	if (nRet != APP_OK)
	{
		ERROR_RETURN("CAppSupporter::m_fnSendResponseCommand -> m_fnCompressMsg Error\n",nRet);
	}

	EnterCriticalSection(&m_csAppSupporter);

	CopyCmdMsg.hSentEvent		= (INT64)CreateEvent(NULL, TRUE, FALSE, NULL);

	CopyCmdMsg.uMsgOrigin = CMD_TYPE_REPLY;

	pcBuffer				= new BYTE[sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE)];									

	//Buffer 에서 Packet 의 Header 와 Body 의 시작 Pointer 를 구한다.
	pPacketHeader			= (PACKET_HEADER*)pcBuffer;
	pPacketClientMessage	= (PACKET_CLIENT_MESSAGE*)(pcBuffer + sizeof(PACKET_HEADER));

	//Message Header 정보를 설정한다.
	pPacketHeader->PacketDataType	= CLIENT_MESSAGE_DATA;

	//Message Body 정보를 설정한다.
	pPacketClientMessage->uStationNo1	= 0;								//자신의 Station No.. 0으로 입력 되면 Server Receive 단에서 자동으로 자신의 Station No 로 바꿔준다.
	pPacketClientMessage->uStationNo2	= CopyCmdMsg.uStation_Dest;
	pPacketClientMessage->uTaskNo1		= CopyCmdMsg.uTask_Dest;
	pPacketClientMessage->uTaskNo2		= CopyCmdMsg.uTask_Src;
	pPacketClientMessage->dwTimeOut		= CopyCmdMsg.dwTimeOut;

	pPacketHeader->uFullMemorySize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uMsgSize;

	if (CopyCmdMsg.uCompressedMsgSize > 0)
	{
		pPacketHeader->uFullMemorySize			= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uCompressedMsgSize;
		pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uCompressedMsgSize;	
		pPacketHeader->uDataSize		= sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uCompressedMsgSize;							
		pPacketClientMessage->uParamBufSize = (sizeof(CMDMSG) + CopyCmdMsg.uCompressedMsgSize);
	}
	else
	{
		pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uMsgSize;	
		pPacketHeader->uDataSize		= sizeof(PACKET_CLIENT_MESSAGE) + sizeof(CMDMSG) + CopyCmdMsg.uMsgSize;							
		pPacketClientMessage->uParamBufSize = (sizeof(CMDMSG) + CopyCmdMsg.uMsgSize);
	}

	pPacketClientMessage->uSendOkFlag	= OKAY;
	pPacketClientMessage->pParamBuf		= (INT64)&CopyCmdMsg;
	//memcpy(pPacketClientMessage->cParamBuf, pCmdMsg, sizeof(CMDMSG));		//CMDMSG 자체를 파라메터로쓰는 이유는 CMDMSG 구조체가 패킷구조체와 틀려서 
																			//body로 쓸수 없기 때문이다. body 자체는 Server 와 Client dll 에서만 참고 되어진다.
																			//현재 명령을 Receive 하는 Task 에서는 Application 으로 Callback 을 수행 되어 질때
																			//넘기는 구조체는 pPacketClientMessage->cParamBuf 의 pCmdMsg 이다.
																			//이후 실제 CMDMSG구조체에서 사용하는 Parameter 가 입력된다.
	//if(pCmdMsg->cMsgBuf != NULL)
	//	memcpy(pPacketClientMessage->cParamBuf + sizeof(CMDMSG), (void*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);	//Application 에서 사용하는 파라메터 최대 크기를 알아야 오류를 방지 할 수 있다..Work Item.. 
																										//uMsgSize 가 MAX_BUFFER-1204 보다 클경우 예외처리 필요 Todo..
	// Paramerter buffer 가 NULL 인경우 사이즈는 0
	//else
	//	pCmdMsg->uMsgSize = 0;

	//TRACE("AppSupporter : %s\n",pCmdMsg->cMsgBuf);

	//Send Message
	nRet = m_SendMessageQueue.m_fnPushMessage(pcBuffer);

	//MemoryPool.Free((PER_PACKET_BUFFER*)pcBuffer);

	//nErrorCode = nRet;

	{
		HANDLE Event[2];
		Event[0] = (HANDLE)CopyCmdMsg.hSentEvent;
		Event[1] = m_hResponseEmergencyStop;

		m_nWaitResponseCount++;

		LeaveCriticalSection(&m_csAppSupporter);

		nRet = WaitForMultipleObjects(2, Event, FALSE, CMD_MESSAGE_SEND_WAIT_TIME);

		EnterCriticalSection(&m_csAppSupporter);

		m_nWaitResponseCount--;

		//만약 비상정지 이벤트가 활성화 되었으면% 
		if(nRet == WAIT_OBJECT_0 + 1)
		{
			//응답 대기 중인 커맨드가 없으면  비상정지용 이벤트를 Reset한다.
			if(m_nWaitResponseCount <= 0)
				ResetEvent(m_hResponseEmergencyStop);
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_RESET;
		}
		else if(nRet == WAIT_TIMEOUT)
			nErrorCode = ERR_CLIENT_MESSAGE_RSP_TIMEOUT;
		else
		{
			if ( pSrcCmdMsg->uRetStatus == ERR_CLIENT_PARAM_BUFFER_IS_NULL)
			{
				nErrorCode = ERR_CLIENT_PARAM_BUFFER_IS_NULL;
			}
			else
				nErrorCode = OKAY;
		}

	}

	CloseHandle((HANDLE)CopyCmdMsg.hSentEvent);

	LeaveCriticalSection(&m_csAppSupporter);

	if(CopyCmdMsg.uCompressedMsgSize > 0)
	{
		if (CopyCmdMsg.cCompressedMsgBuf)
		{
			delete[] (char*)CopyCmdMsg.cCompressedMsgBuf;
			CopyCmdMsg.cCompressedMsgBuf = 0;
		}
		CopyCmdMsg.uCompressedMsgSize = 0;
	}

	ERROR_RETURN("CAppSupporter::m_fnSendResponseCommand -> m_pMessageQueue.AddMessage Error\n",nErrorCode);

	return nErrorCode;
}


int	CAppSupporter::m_fnCompressMsg(CMDMSG* pCmdMsg)
{
	int nRet = APP_OK;

	if (pCmdMsg->uMsgCompressMode == 0 || (pCmdMsg->uMsgCompressMode == 2 && pCmdMsg->uMsgSize < AUTO_MSG_COMPRESS_SIZE))
	{
		return nRet;
	}
	else if (pCmdMsg->uMsgCompressMode == 1 || (pCmdMsg->uMsgCompressMode == 2 && pCmdMsg->uMsgSize >= AUTO_MSG_COMPRESS_SIZE))
	{ 
		// 실제 데이터가 들어있는 메모리 사이즈가 설정되어 있으면 압축을하지 않는다. 압축 해제 복잡성을 줄이기 위해 하지않는다.
		if (pCmdMsg->uSendMsgSize == 0)
		{
			size_t length = snappy_max_compressed_length(pCmdMsg->uMsgSize);
			char *compressed = new char[length];

			snappy_status status = snappy_compress((char*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize, compressed, &length);

			pCmdMsg->cCompressedMsgBuf = (INT64)compressed;
			pCmdMsg->uCompressedMsgSize = (ULONG)length;
		}
	}
	else
	{

		// 압축 옵션이 틀리다.
		return nRet;
	}


	return nRet;
}
