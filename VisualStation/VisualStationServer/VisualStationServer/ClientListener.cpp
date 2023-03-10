#include "StdAfx.h"
#include "clientlistener.h"
#include "InternalServer.h"
#include <mstcpip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	Class 생성자
*	Parameter	:	호출한 Class(Internal Server)의 Pointer
*					Server IP
*					Server Port
*					호출한 Class에서 생성된 IOCP handle
*	Return		:	없음
*	Function	:	Class 생성자로서 Class 생성시 초기화 기능을 담당한다.
*	Create		:	2006.02.22
*	Author		:	김용태
*	Version		:	1.07
*/
CClientListener::CClientListener(CInternalServer *pInternalServer, char* pszServerIP, WORD wServerPort, HANDLE hIOCP)
{
	//전달받은 Internal Server Pointer, Server IP, Server Port, IOCP handle을 자신의 Attribute에 전개한다.
	m_pInternalServer	= pInternalServer;
	m_wServerPort		= wServerPort;
	m_hIOCP				= hIOCP;
	strcpy(m_szServerIP, pszServerIP);

	//Thread 수행 Flag를 true로 초기화 한다.
	m_bRun				=	true;
}

/*
*	Module name	:	Class 소멸자
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	Class 소멸자로서 Class 소멸시 호출 된다.
*	Create		:	2006.02.22
*	Author		:	김용태
*	Version		:	1.0
*/
CClientListener::~CClientListener(void)
{
	SOCKET		sock;
	SOCKADDR_IN ServerAddr;

	//Thread 수행 Flag를 false로 설정하여서 Thread를 종료할 수 있게 한다.
	m_bRun						= false;

	//자신의 socket에 한번 접속을 하고 종료한다. 
	//이유는 자신의 Thread 가 Client Accept를 위해 Blocking 되어 있음으로 Blocking을 풀기 위함이다.
	sock						= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	ServerAddr.sin_family		= AF_INET;
	ServerAddr.sin_port			= htons(m_wServerPort);
	ServerAddr.sin_addr.s_addr	= inet_addr(m_szServerIP);

	connect(sock, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr));
	SAFE_CLOSE_SOCKET(sock);

	m_fnWaitForTermination();
}

/*
*	Module name	:	m_fnStartServer
*	Parameter	:	없음
*	Return		:	정상 : 1
					이상 : Error Code
*	Function	:	Listen Socket을 생성하고 Accept Thread를 생성한다.
*	Create		:	2006.02.22
*	Author		:	김용태
*	Version		:	1.0
*/
int CClientListener::m_fnStartServer()
{	
	int			nErrorCode = OKAY;
	int         nRet;
	HANDLE		hThread;	//Accept Thread 의 Handle 을 저장한다.
	SOCKADDR_IN sa;
	memset(&sa,0,sizeof(sa));
	
	m_fnWriteLog("CClientListener::m_fnStartServer -> Start...");

	//서버소켓을 생성한다.
	m_hListenSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_hListenSocket)
	{
		nErrorCode	= WSAGetLastError();
		m_fnWriteLog("CClientListener::m_fnStartServer -> WSASocket Error : %d", nErrorCode);
		return nErrorCode;		
	}

	//서버소켓을 bind 한다.
	sa.sin_family		= AF_INET;
	sa.sin_port			= htons(m_wServerPort);
	sa.sin_addr.s_addr	= htonl(INADDR_ANY); 

	bool b = true;
	setsockopt(m_hListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&b, sizeof(b));
	
	nRet = bind(m_hListenSocket, (SOCKADDR*)&sa, sizeof(sa));
	if(SOCKET_ERROR == nRet)
	{
		nErrorCode	= WSAGetLastError();
		m_fnWriteLog("CClientListener::m_fnStartServer -> bind Error : %d", nErrorCode);
		return nErrorCode;
	}

	//서버소켓을 listen 한다.
	nRet = listen(m_hListenSocket, 5);
	if(SOCKET_ERROR == nRet)
	{
		nErrorCode	= WSAGetLastError();
		m_fnWriteLog("CClientListener::m_fnStartServer -> listen Error : %d", nErrorCode);
		return nErrorCode;
	}

	//Accept Thread를 생성한다.
	hThread	= m_fnStartAccept();
	if(NULL == hThread)
	{
		nErrorCode	= -1;	//Work item... Error Code 의 정의가 필요하다.
		m_fnWriteLog("CClientListener::m_fnStartServer -> Accept Thread create fail..");
		return nErrorCode;
	}

	m_fnWriteLog("CClientListener::m_fnStartServer -> End...");

	//만일 상기 과정 중 하나라도 Error 가 아니면 1 을 반환한다.
	return nErrorCode;	//Default = OK
}

/*
*	Module name	:	m_fnGetListenSocket
*	Parameter	:	없음
*	Return		:	Listen Socket Handle
*	Function	:	Listen Socket의 핸들을 반환한다.
*	Create		:	2006.02.23
*	Author		:	김용태
*	Version		:	1.0
*/
SOCKET CClientListener::m_fnGetListenSocket()
{
	return m_hListenSocket;
}

/*
*	Module name	:	m_fnStartAccept
*	Parameter	:	없음
*	Return		:	정상  : Accept Thread Handle
					이상  : Null
*	Function	:	Accept Thread를 구동시킨다.
*	Create		:	2006.02.23
*	Author		:	김용태
*	Version		:	1.0
*/
HANDLE CClientListener::m_fnStartAccept()
{
	//Thread를 생성한다.
	//만일 Thread 생성 핸들 값이 Null 이 아니라면 Thread 의 핸들 값을 반환한다.
	//그렇지 않고 만일 Null 이라면 Null을 반환한다.
	return m_fnStartThread();
}

/*
*	Module name	:	m_fnRunThread
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	Accept Thread로서 Client Accept의 실질적인 기능을 담당한다.
*	Create		:	2006.02.23
*	Author		:	김용태
*	Version		:	1.0
*/
void CClientListener::m_fnRunThread()
{
	int					nClientSocketAddrLen;
	int					nRet;
	SOCKADDR_IN			ClientSocketAddr;
	int					nErrorCode			= OKAY;
	SOCKET				hAcceptSock			= INVALID_SOCKET;
	PER_SOCKET_CONTEXT*	pPerSocketContext	= NULL;
	DWORD				dwRecvNumBytes		= 0;
	DWORD				dwFlags				= 0;

	nClientSocketAddrLen	= sizeof(ClientSocketAddr);

	//만일 Thread 수행 Flag가 true 이면
	while(m_bRun)
	{
		//Client Socket 을 Accept 한다.
		hAcceptSock = WSAAccept(m_hListenSocket, ( SOCKADDR * )&ClientSocketAddr, &nClientSocketAddrLen, NULL, 0);
		
		//만일 Thread 종료 Flag 가 false 라면 종료한다.
		if(!m_bRun) break;

		//만일 Socket 이 ERROR 라면 Error LOG를 기록하고 Continue한다.
		if (SOCKET_ERROR == hAcceptSock)
		{	
			nErrorCode	= WSAGetLastError();
			m_fnWriteLog("CClientListener::m_fnRunThread -> WSAAccept Error : %d", nErrorCode);
		}
		//그렇지 않고 만일 Error 가 아니라면 정상 수행 한다.
		else
		{
			m_fnWriteLog("CClientListener::m_fnRunThread -> Someone Accept Start");
			
			//Socket 의 System Send/Receive Buffer를 0으로 한다.(필수 사항은 아니고 서버 성능 향상을 위한 옵션사항)
			int nZero=0;
			nRet = setsockopt(hAcceptSock,SOL_SOCKET,SO_RCVBUF,(const char*)&nZero,sizeof(int));
			if(SOCKET_ERROR == nRet)
			{
				nErrorCode	= WSAGetLastError();
				m_fnWriteLog("CClientListener::m_fnRunThread -> Change Receive Buffer Size Error : %d", nErrorCode);
				continue;
			}
			nZero=0;
			nRet = setsockopt(hAcceptSock,SOL_SOCKET,SO_SNDBUF,(const char*)&nZero,sizeof(int));
			if(SOCKET_ERROR == nRet)
			{
				nErrorCode	= WSAGetLastError();
				m_fnWriteLog("CClientListener::m_fnRunThread -> Change Send Buffer Size Error : %d", nErrorCode);
				continue;
			}

			//int opt_val = TRUE;
			//nRet = setsockopt(hAcceptSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(opt_val));
			//if(SOCKET_ERROR == nRet)
			//{
			//	nErrorCode	= WSAGetLastError();
			//	m_fnWriteLog("CClientListener::m_fnRunThread -> Change TCP_NODELAY Error : %d", nErrorCode);
			//	continue;
			//}
			
			//Socket 의 유효성을 검사하게 한다. 5000ms 마다 메시지를 보내고 3000ms 내에 응답이 없으면 에러처리한다.
			tcp_keepalive keepAlive = { TRUE, 5000, 3000};

			DWORD nBytesReturned;

			if(SOCKET_ERROR==WSAIoctl(hAcceptSock, SIO_KEEPALIVE_VALS, &keepAlive, sizeof(keepAlive) ,0, 0, &nBytesReturned, NULL, NULL ))
			{
				nErrorCode	= WSAGetLastError();
				m_fnWriteLog("CClientListener::m_fnRunThread -> WSAIoctl Error : %d", nErrorCode);
				continue;
			}
			
			
			//Socket을 IOCP 에 등록한다.
			pPerSocketContext = m_fnRegistIOCP(hAcceptSock);
			if(NULL == pPerSocketContext)
			{
				m_fnWriteLog("CClientListener::m_fnRunThread -> IOCP Regist Error");
				continue;
			}

			//Client를 등록요청한다.
			nRet = m_pInternalServer->m_fnRegistClientRequest(pPerSocketContext);
			if(OKAY != nRet)
				m_fnWriteLog("CClientListener::m_fnRunThread -> m_fnRegistClientRequest error : %d", nRet);

			//Socket을 Receive mode로 전환한다.
			nRet = WSARecv(hAcceptSock, &(pPerSocketContext->recvContext->wsaBuf), 1, 
				&dwRecvNumBytes, &dwFlags, &(pPerSocketContext->recvContext->overlapped), NULL);

			//만일 Receive mode 전환이 Error 라면 Client Socket을 종료한다.
			if(SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError()))
			{
				//Client 의 연결을 종료한다.
				m_fnWriteLog("CClientListener::m_fnRunThread -> IOCP Receive Mode Change Error.. Disconnect Someone Client!!");
				SAFE_DELETE(pPerSocketContext);
				//m_fnWriteLog("CClientListener::m_fnRunThread -> PPER_SOCKET_CONTEXT Deleted..");
				continue;
			}
		}
	}

	return;
}

/*
*	Module name	:	m_fnRegistIOCP
*	Parameter	:	Client Socket
*	Return		:	정상 종료 : PER_SOCKET_CONTEXT형 구조체의 포인터 반환
					이상 종료 : NULL
*	Function	:	Accept 된 Client Socket을 IOCP 에 등록한다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
PER_SOCKET_CONTEXT* CClientListener::m_fnRegistIOCP(SOCKET hClientsocket)
{
	PER_SOCKET_CONTEXT*		pPerSocketContext = NULL;
	
	//PER_SOCKET_CONTEXT 구조체를 동적으로 생성하고 초기화 한다.
	pPerSocketContext	= new PER_SOCKET_CONTEXT(hClientsocket);

	if(NULL == pPerSocketContext)
	{
		m_fnWriteLog("CClientListener::m_fnRegistIOCP -> new PER_SOCKET_CONTEXT Error");
		SAFE_DELETE(pPerSocketContext);
		return	NULL;
	}
	//PER_SOCKET_CONTEXT 구조체가 생성 되면서 생성자에 의해 Receive Buffer 가 생성되고 초기화 된다.
	if(NULL == pPerSocketContext->recvContext)
	{
		m_fnWriteLog("CClientListener::m_fnRegistIOCP -> new PER_IO_CONTEXT Error");
		SAFE_DELETE(pPerSocketContext);
		return	NULL;
	}

	//Client socket 과 PER_SOCKET_CONTEXT pointer를 IOCP 에 연결한다. 
	//추후 IOCP에서 작업Thread를 구동시킬 때 해당 socket 과 구조체 pointer 가 넘어온다.
	if (NULL == CreateIoCompletionPort((HANDLE)hClientsocket, m_hIOCP, (ULONG_PTR)pPerSocketContext, 0))
	{
		m_fnWriteLog("CClientListener::m_fnRegistIOCP -> CreateIoCompletionPortT Error");
		SAFE_DELETE(pPerSocketContext);
		return NULL;
	}
	
	//만일 상기 과정 중 Error 가 아니면 PER_SOCKET_CONTEXT 구조체의 Pointer를 반환한다.
	//그렇지 않고 만일 상기 과정 중 Error라면 Null 을 반환한다.
	return pPerSocketContext;
}

/*
*	Module name	:	m_fnCloseClient
*	Parameter	:	Client 의 PER_SOCKET_CONTEXT 구조체 포인터
*	Return		:	없음
*	Function	:	Accept 만 되고 Internal Server에 등록 되지 않은 Client Socket 을 종료한다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
void CClientListener::m_fnCloseClient(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	//해당 Socket을 종료한다.
	m_pInternalServer->m_fnCloseClient(pPerSocketContext);
}

/*
*	Module name	:	m_fnWriteLog
*	Parameter	:	Null로 끝나는 문자열 Pointer, 매개 변수 리스트
*	Return		:	없음
*	Function	:	Log를 기록한다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
void CClientListener::m_fnWriteLog(const char* pszLog, ...)
{
	char cBuffer[MAX_LOG_BUFFER];

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList; 
	va_start(vaList, pszLog); 
	vsprintf(cBuffer, pszLog, (va_list)vaList); 
	va_end(vaList);

	//CInternalServer의 Log 기록 Method를 호출한다.
	m_pInternalServer->m_fnWriteLog(cBuffer);	//Work item
}

