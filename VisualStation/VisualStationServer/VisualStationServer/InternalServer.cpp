#include "StdAfx.h"
#include "internalserver.h"
#include "ClientListener.h"
#include "ClientMessenger.h"
#include "LogWriter.h"
#include "SharedMemory.h"
#include "ItemList.h"		//Message List Template
#include "..\..\CommonHeader\Class\MemPooler.h"					//Memory Pool

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	CInternalServer
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	생성자
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
CInternalServer::CInternalServer(char* pszServerIp/* = "127.0.0.1" */, WORD wPortNo/* = 5000 */, UINT uStationNo/*= 0*/)
{
	//로그관리자를 생성한다.
	//work Item.. INI File 에서 파라메터를 가져오게 수정 필요
	m_pLogWriter		= new CLogWriter(VS_SERVER_LOG_PATH,"VS64",7,0,true);

	//공유메모리 관리자를 생성한다.
	m_pSharedMemory				= new CSharedMemory;

	//Client List, Client Messenger List, Timer List, Memory Pool 를 생성한다.
	m_pListClientIndex			= new CItemList<PER_SOCKET_CONTEXT*>;
	m_pListMessenger			= new CItemList<CClientMessenger*>;
	m_pListTimerClientRespons	= new CItemList<PER_TIMER_CLIENT_RESPONS*>;
	m_pMemoryPool				= new CMemPooler<PER_IO_CONTEXT>(MAX_MEMORY_POOL);

	//멤버 변수 초기화
	m_pClientListener	= NULL;
	m_hIOCP				= NULL;
	m_dwWorkerThread	= 0;
	m_wPortNo			= wPortNo;
	m_uStationNo		= uStationNo;

	strcpy(m_szServerIP, pszServerIp);

}

/*
*	Module name	:	~CInternalServer
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸자
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
CInternalServer::~CInternalServer(void)
{
	//Client 연결 처리 자를 소멸시킨다.
	SAFE_DELETE(m_pClientListener);

	//Client 메시지 처리 자의 Thread 종료를 위해 IOCP 에 Thread 종료 메시지를 전송한다.
	{
		DWORD dwBytesTransferred = 0;
		DWORD dwCompKey = 0;
		LPOVERLAPPED pOverlap = NULL;

		for(UINT n=0; n < m_dwWorkerThread; n++)
		{
			PostQueuedCompletionStatus(m_hIOCP, dwBytesTransferred,	dwCompKey, /*(LPOVERLAPPED)pOverlap*/NULL);
		}
	}

	//공유메모리 관리자를 소멸시킨다.
	SAFE_DELETE(m_pSharedMemory);

	//Client메시지 처리 자 리스트(m_pListMessenger)를 참고하여 소멸시킨다.
	SAFE_DELETE(m_pListMessenger);

	//Client Index 를 참고하여 Socket 을 모두 Close 하고 할당된 메모리를 소멸 시킨다.
	SAFE_DELETE(m_pListClientIndex);

	//Client Timer List를 참고하여 구조체를 모두 삭제한다.
	SAFE_DELETE(m_pListTimerClientRespons);

	//IOCP를 종료한다.
	CloseHandle(m_hIOCP);

	//Winsock 을 종료한다.
	WSACleanup();

	//메모리 풀을 해제한다.
	SAFE_DELETE(m_pMemoryPool);

	//로그관리자를 소멸시킨다.
	SAFE_DELETE(m_pLogWriter);
}

/*
*	Module name	:	m_fnStartInternalServer
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Internal Server를 시작한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int	CInternalServer::m_fnStartInternalServer()
{
	WSADATA		wsaData;
	int			nRet;
	int			nErrorCode = OKAY;
	SYSTEM_INFO systemInfo;

	m_fnWriteLog("CInternalServer::m_fnStartInternalServer -> Start..");

	//Worker Thread (Client 메시지 처리자)를 생성 할 개수를 알기위해 시스템 정보를 가지고온다. 
	GetSystemInfo(&systemInfo);

	//worker Thread 의 개수를 결정한다.
	m_dwWorkerThread = systemInfo.dwNumberOfProcessors*2;

	//Winsock 초기화
	nRet = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(0 != nRet)
	{
		nErrorCode	= WSAGetLastError();
		m_fnWriteLog("CInternalServer::m_fnStartInternalServer -> WSAStartup Error : %d", nErrorCode);
		return nErrorCode;
	}

	//공유메모리 관리자에서 Server Parameter를 가져온다.
	nRet = m_fnGetServerParameter();
	ERROR_RETURN("CInternalServer::m_fnStartInternalServer -> m_fnGetServerParameter Error : %d", nRet);

	//IOCP를 생성하고 IOCP 핸들을 멤버 변수로 가지고 있는다.
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0 );
	if (INVALID_HANDLE_VALUE == m_hIOCP )
	{
		nErrorCode = NG;
		m_fnWriteLog("CInternalServer::m_fnStartInternalServer -> CreateIoCompletionPort Error");
		return nErrorCode;
	}

	//Client 메시지 처리자를 결정된 생성 개수만큼 생성시킨다.
	for (DWORD n=0; n < m_dwWorkerThread; n++)
	{
		CClientMessenger* pMessenger = new CClientMessenger(this, m_hIOCP);
		pMessenger->m_fnStartMessenger();
		//생성시킨 Client Message 처리 자 Handle을 Client메시지 처리 자 리스트에 넣는다.
		m_pListMessenger->push_back(pMessenger);
	}

	//Client 연결 처리 자를 생성하고 Pointer를 멤버 변수로 가지고 있는다.
	m_pClientListener = new CClientListener(this, m_szServerIP, m_wPortNo, m_hIOCP);

	//Client 연결 처리 자의 수행을 시작한다.
	nRet = m_pClientListener->m_fnStartServer();
	ERROR_RETURN("CInternalServer::m_fnStartInternalServer -> m_pClientListener Start Fail~!!!: %d",nRet);

	m_fnWriteLog("CInternalServer::m_fnStartInternalServer -> End..");
	return nErrorCode;
}

/*
*	Module name	:	m_fnRegistClientRequest
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Server 등록을 위해, Client로 Client ID 와 name 전송을 요청한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int	CInternalServer::m_fnRegistClientRequest(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	int							nErrorCode = OKAY;
	PER_TIMER_CLIENT_RESPONS*	pPerTimerClientRespons;
	PER_IO_CONTEXT*				pPerSendIOContext;
	UINT						uTimerID;
	int							nRet;

	//PER_TIMER_CLIENT_RESPONS 구조체를 Setting 한다.
	pPerTimerClientRespons						= new PER_TIMER_CLIENT_RESPONS;
	pPerTimerClientRespons->VSCommand			= VS_COMMAND_CLIENT_ID_REQUEST;
	pPerTimerClientRespons->pPerSocketContext	= pPerSocketContext;
	pPerTimerClientRespons->nTimerID			= 0;
	pPerTimerClientRespons->pInternalServer		= this;

	//m_fnWriteLog("CInternalServer::m_fnRegistClientRequest -> PER_TIMER_CLIENT_RESPONS Created..");

	//PER_TIMER_CLIENT_RESPONS pointer를 반환 값으로 하는 5 초 Timer를 가동 시킨다.
	uTimerID = ::timeSetEvent(TIME_OUT_CLIENT_RESPONS, 5, (LPTIMECALLBACK)m_fnTimer, (DWORD_PTR)pPerTimerClientRespons, TIME_PERIODIC);
	if(NULL == uTimerID)
	{
		nErrorCode = NG;
		m_fnWriteLog("CInternalServer::m_fnRegistClientRequest -> timeSetEvent Error");
		return nErrorCode;
	}

	//PER_TIMER_CLIENT_RESPONS 구조체내에 생성된 Timer ID 를 설정하고 Timer 리스트로 삽입한다.
	pPerTimerClientRespons->nTimerID			= uTimerID;
	m_pListTimerClientRespons->m_fnPushItem(pPerTimerClientRespons);

	//Send 를 위한 구조체를 생성한다.
	//pPerSendIOContext = new	PER_IO_CONTEXT;
	pPerSendIOContext = m_pMemoryPool->Alloc();

	//Send Buffer에 Packet 정보와 Data를 설정 한다.
	nRet = m_fnMakeVSCommand(pPerSendIOContext, VS_COMMAND_CLIENT_ID_REQUEST, NULL);
	//해당 Socket으로 Client ID, name 전송 요청을 한다.						
	nRet = m_fnSendMessageClient(pPerSocketContext->socket, pPerSendIOContext);			//Clinet 로 전송 요청

	ERROR_RETURN("CInternalServer::m_fnRegistClientRequest -> m_fnSendMessageClient().. VS_COMMAND_CLIENT_ID_REQUEST Send Error : %d",nRet);

	return nErrorCode;
}

/*
*	Module name	:	m_fnProcessClientRegist
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client 에서 요청하는 Server 등록을 처리한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int	CInternalServer::m_fnProcessClientRegist(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	int	nErrorCode = OKAY;
	int nRet;
	PACKET_CLIENT_VS_REGIST*	pRecvPacketData;
	PER_IO_CONTEXT*				pSendIoContext;

	pRecvPacketData = (PACKET_CLIENT_VS_REGIST*)(pPerSocketContext->recvContext->pBuffer + sizeof(PACKET_HEADER));

	//Packet 정보 중 Client ID과 name을 PER_SOCKET_CONTEXT 내용 에 전개 한다.
	pPerSocketContext->uClientID = pRecvPacketData->uClientID;
	strcpy(pPerSocketContext->szClientName, pRecvPacketData->cClientName);

	//PER_SOCKET_CONTEXT pointer 를 참고하여  PER_TIMER_CLIENT_RESPONS 내의 Timer 를 종료하게 요청한다.
	nRet = m_fnDeleteTimerClientRespons(pPerSocketContext, VS_COMMAND_CLIENT_ID_REQUEST);
	ERROR_RETURN("CInternalServer::m_fnProcessClientRegist -> m_fnDeleteTimerClientRespons Error : %d",nRet);

	//Client Socket DLL 의 버전이 사용 가능한 버전인지 확인한다.
	if (pRecvPacketData->uClientSocketDLLVersion != (CLIENT_SOCKET_DLL_VERSION*1000))
	{
		//Client Socket DLL 의 버전이 맞지 않는다.
		m_fnWriteLog("CInternalServer::m_fnProcessClientRegist -> ClientSocketDLL Version is incorrect. Client ID : %d", pPerSocketContext->uClientID);

		//pSendIoContext = new PER_IO_CONTEXT;
		pSendIoContext = m_pMemoryPool->Alloc();

		nRet = m_fnMakeVSCommand(pSendIoContext, VS_COMMAND_REGIST_CLIENT_DLL_INCORRECT, NULL);
		nRet = m_fnSendMessageClient(pPerSocketContext->socket, pSendIoContext);
		ERROR_RETURN("CInternalServer::m_fnProcessClientRegist -> m_fnSendMessageClient().. VS_COMMAND_REGIST_CLIENT_DLL_INCORRECT Send Error : %d",nRet);

		//Client Socket Dll 버전이 맞지 않는다고 반환.
		nErrorCode = ERR_INSERVER_REGIST_CLIENT_SOCKET_DLL_INCORRECT;
		return nErrorCode;
	}
	 

	std::deque<PER_SOCKET_CONTEXT*>::iterator it = m_pListClientIndex->begin();

	//Client ID 가 PER_SOCKET_CONTEXT 리스트 내에 중복이 되는지를 찾는다.
	for(; it != m_pListClientIndex->end(); it++)
	{
		//만일 Client ID 가 PER_SOCKET_CONTEXT 리스트 내에 중복이 된다면 
		PER_SOCKET_CONTEXT* lpSocketContext = *it;
		if(lpSocketContext->uClientID == pPerSocketContext->uClientID)
		{
			//Client ID 가 중복이 되므로 Client 로 중복됨을 알린다.
			m_fnWriteLog("CInternalServer::m_fnProcessClientRegist -> Client ID is duplication : %d", pPerSocketContext->uClientID);

			//pSendIoContext = new PER_IO_CONTEXT;
			pSendIoContext = m_pMemoryPool->Alloc();

			nRet = m_fnMakeVSCommand(pSendIoContext, VS_COMMAND_REGIST_CLIENT_ID_DUPLICATE, NULL);
			nRet = m_fnSendMessageClient(pPerSocketContext->socket, pSendIoContext);
			ERROR_RETURN("CInternalServer::m_fnProcessClientRegist -> m_fnSendMessageClient().. VS_COMMAND_REGIST_CLIENT_ID_DUPLICATE Send Error : %d",nRet);

			//Client Id 가 중복임을 반환한다.
			nErrorCode = ERR_INSERVER_REGIST_CLIENT_ID_DUPLICATION;
			return nErrorCode;
		}
	}

	//만일 Client ID 가 PER_SOCKET_CONTEXT 리스트 내에 중복이 되지 않는다면
	//PER_SOCKET_CONTEXT 를 리스트의 마지막 노드로 삽입한다.
	m_pListClientIndex->m_fnPushItem(pPerSocketContext);

	//pSendIoContext = new PER_IO_CONTEXT;
	pSendIoContext = m_pMemoryPool->Alloc();

	//Client 로 정상 등록됨을 알린다.
	nErrorCode = m_fnMakeVSCommand(pSendIoContext, VS_COMMAND_REGIST_CLIENT_OK, NULL);
	nErrorCode = m_fnSendMessageClient(pPerSocketContext->socket, pSendIoContext);

	ERROR_RETURN("CInternalServer::m_fnProcessClientRegist -> m_fnSendMessageClient().. VS_COMMAND_REGIST_CLIENT_OK Send Error : %d",nErrorCode);

	m_fnWriteLog("Client Regist is OK! : %d", pPerSocketContext->uClientID);
	return nErrorCode;
}

/*
*	Module name	:	m_fnDeleteTimerClientRespons
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Parameter 정보와 일치하는 PER_TIMER_CLIENT_RESPONS list 를 지운다
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0, PPER_SOCKET_CONTEXT pPerSocketContext, VS_COMMAND nCommand
*/
int	CInternalServer::m_fnDeleteTimerClientRespons(PER_TIMER_CLIENT_RESPONS* pPerTimerClientRespons)
{
	int nRet;
	int	nErrorCode = OKAY;

	//Parameter 정보와 일치하는 PER_TIMER_CLIENT_RESPONS list 를 지운다
	nRet = m_pListTimerClientRespons->m_fnDeleteItem(pPerTimerClientRespons);

	ERROR_RETURN("CInternalServer::m_fnDeleteTimerClientRespons -> PER_TIMER_CLIENT_RESPONS Deleted Error : %d",nRet);

	return nErrorCode;
}

/*
*	Module name	:	m_fnDeleteTimerClientRespons
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Parameter 정보와 일치하는 PER_TIMER_CLIENT_RESPONS list 를 지운다
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0, 
*/
int	CInternalServer::m_fnDeleteTimerClientRespons(PER_SOCKET_CONTEXT* pPerSocketContext, VS_COMMAND nCommand)
{
	int	nRet;
	int	nErrorCode = OKAY;

	//Timer list(PER_TIMER_CLIENT_RESPONS) 중에 PER_SOCKET_CONTEXT 와 일치하는 정보를 찾는다.
	std::deque<PER_TIMER_CLIENT_RESPONS*>::iterator it = m_pListTimerClientRespons->begin();

	for(; it != m_pListTimerClientRespons->end(); it++)
	{
		PER_TIMER_CLIENT_RESPONS* lpTimerData = *it;
		//만일 일치하는 정보가 있으면 해당 timer 종료
		if(lpTimerData->pPerSocketContext == pPerSocketContext && lpTimerData->VSCommand == nCommand)
		{
			nRet = m_pListTimerClientRespons->m_fnDeleteItem(lpTimerData);
			ERROR_RETURN("CInternalServer::m_fnDeleteTimerClientRespons -> PER_TIMER_CLIENT_RESPONS Deleted Error : %d",nRet);
			break;
		}
	}
	return nErrorCode;
}

/*
*	Module name	:	m_fnWriteLog
*	Parameter	:	Null로 끝나는 문자열 Pointer, 매개 변수 리스트
*	Return		:	없음
*	Function	:	Log를 기록한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
void CInternalServer::m_fnWriteLog(const char* pszLog, ...)
{
	char cBuffer[MAX_LOG_BUFFER];

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList;
	va_start(vaList, pszLog);
	vsprintf(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	//CInternalServer의 Log 기록 Method를 호출한다.
	m_pLogWriter->m_fnWriteLog(cBuffer);
}

/*
*	Module name	:	m_fnCloseClient
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client 연결 및 관련된 할당 메모리를 제거한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnCloseClient(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	int	nErrorCode = OKAY;
	int nRet;

	m_fnWriteLog("CInternalServer::m_fnCloseClientNoSendMessage -> PER_SOCKET_CONTEXT Deleted.. Client ID is %d", pPerSocketContext->uClientID);

	//Client index List 에서 제거한다.
	//만일 Index 에 존재 하였을 경우 할당된 메모리도 지워진다.
	nRet = m_pListClientIndex->m_fnDeleteItem(pPerSocketContext);

	nErrorCode = nRet;

	////만일 Index 에 존재하지 않는다면
	//if(OK != nRet)
	//{
	//	//할당된 메모리 공간을 삭제한다.
	//	SAFE_DELETE(pPerSocketContext);
	//}

	//결과 리턴한다.
	return nErrorCode;
}

/*
*	Module name	:	m_fnMakeVSCommand
*	Parameter	:	PER_SOCKET_CONTEXT pointer, VS_COMMAND 종류, Parameter 정보
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Packet VS_COMMAND 를 만든다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnMakeVSCommand(PER_IO_CONTEXT* pPerSendIOContext, VS_COMMAND VsCommand, char* pszData)
{
	int							nErrorCode = OKAY;
	PACKET_HEADER*				pPacketHeader;
	PACKET_VS_COMMAND*			pPacketVsCommand;

	pPerSendIOContext->pBuffer	= new BYTE[sizeof(PACKET_HEADER) + sizeof(PACKET_VS_COMMAND)];

	//Buffer 에서 Packet Header 와 Body 부분의 시작점을 가지고 온다.
	pPacketHeader					= (PACKET_HEADER*)(pPerSendIOContext->pBuffer);
	pPacketVsCommand				= (PACKET_VS_COMMAND*)(pPerSendIOContext->pBuffer + sizeof(PACKET_HEADER));

	//Packet Header 정보를 설정 한다.
	pPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_VS_COMMAND);
	pPacketHeader->PacketDataType	= VS_COMMAND_DATA;		//Visual Station 명령
	pPacketHeader->uDataSize		= sizeof(PACKET_VS_COMMAND);

	//Packet Body 정보를 설정한다.
	pPacketVsCommand->VsCommand		= VsCommand;

	//Packet 의 보낼 크기를 지정한다.
	pPerSendIOContext->wsaBuf.buf		= (CHAR*)pPerSendIOContext->pBuffer;
	pPerSendIOContext->wsaBuf.len		= sizeof(PACKET_HEADER) + sizeof(PACKET_VS_COMMAND);
	pPerSendIOContext->uBufferLength	= sizeof(PACKET_HEADER) + sizeof(PACKET_VS_COMMAND);
	pPerSendIOContext->uBufferOffset	= 0;
	return nErrorCode;
}

/*
*	Module name	:	m_fnSendMessageClient
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client로의 Message 전송 기능을 제공한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int	CInternalServer::m_fnSendMessageClient(SOCKET hSendSocket, PER_IO_CONTEXT* pPerSendIOContext)
{
	int		nErrorCode = OKAY;
	DWORD	dwSendBytes=0;
	DWORD	dwFlags=0;
	int		nRet;

	memset((void*)&pPerSendIOContext->overlapped, 0, sizeof(OVERLAPPED));

	//해당 Socket으로 PER_SOCKET_CONTEXT 안의 Send buffer 내용을 전송한다.
	nRet = WSASend(hSendSocket, &(pPerSendIOContext->wsaBuf), 1, 
		&dwSendBytes, dwFlags, &(pPerSendIOContext->overlapped), NULL);

	//만일 전송 요청에 대한 반환 값이 flase 이고, Socket Error 가 ERROR_IO_PENDING가 아니 라면 (Socket 이 Client 에서 강제로 끊어짐)
	if(SOCKET_ERROR == nRet)
	{
		nErrorCode = WSAGetLastError();
		if(nErrorCode != WSA_IO_PENDING && nErrorCode != ERROR_IO_PENDING)
		{
			//Error Log를 기록한다.
			m_fnWriteLog("CInternalServer::m_fnSendMessageClient -> WSASend Error.. Client will close : %d", nErrorCode);
			//해당 Client 의 Index를 삭제하고 socket 을 끊는다.
			//nRet = m_pListClientIndex->m_fnDeleteItem(pPerSocketContext);
			SAFE_CLOSE_SOCKET(hSendSocket);
			//SAFE_DELETE(pPerSocketContext);
			//m_fnWriteLog("CInternalServer::m_fnSendMessageClient -> PER_SOCKET_CONTEXT Deleted..");
			return nErrorCode;
		}
		nErrorCode = OKAY;
	}
	//printf("Send Message : %d \n", pPerSocketContext->uClientID);
	return nErrorCode;
}

/*
*	Module name	:	m_fnSendMessageStation
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	타 Station 으로 Message 전송 기능을 제공한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int	CInternalServer::m_fnSendMessageStation(PER_SOCKET_CONTEXT* pPerSocketContext, USHORT uStationNo)
{
	int		nErrorCode = OKAY;

	m_fnWriteLog("CInternalServer::m_fnSendMessageStation.. Enter..");

	return	nErrorCode;
}

/*
*	Module name	:	m_fnProcessSharedMemoryAccess
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client 에서 요청하는 공유메모리 접근 처리
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnProcessSharedMemoryAccess(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	return OKAY;
	//int		nErrorCode = OKAY;
	//int		nRet;

	//PACKET_HEADER*				pRecvPacketHeader/*,pSendPacketHeader*/;
	//PACKET_MEMORY_MESSAGE*		pRecvPacketMemoryMessage/*,pSendPacketMemoryMessage*/;

	////Send Buffer, Recv Buffer 에서 Packet Header 와 Body 부분의 시작점을 가지고 온다.
	//pRecvPacketHeader			= (PACKET_HEADER*)(pPerSocketContext->recvContext->Buffer);
	//pRecvPacketMemoryMessage	= (PACKET_MEMORY_MESSAGE*)(pPerSocketContext->recvContext->Buffer + sizeof(PACKET_HEADER));
	////	pSendPacketHeader			= (PPACKET_HEADER)(pPerSocketContext->sendContext->Buffer);
	////	pSendPacketMemoryMessage	= (PPACKET_MEMORY_MESSAGE)(pPerSocketContext->sendContext->Buffer + sizeof(PACKET_HEADER));

	////만일 근원지 Station No 가 0 이면 자신의 Station 번호를 넣는다.
	//if(0 == pRecvPacketMemoryMessage->uStationNo1)
	//	pRecvPacketMemoryMessage->uStationNo1 = m_uStationNo;
	////만일 목적지 Station No 가 0 이면 자신의 Station 번호를 넣는다.
	//if(0 == pRecvPacketMemoryMessage->uStationNo2)
	//	pRecvPacketMemoryMessage->uStationNo2 = m_uStationNo;

	////만일 근원지,목적지 Staion no 가 서로 틀리면 
	//if(pRecvPacketMemoryMessage->uStationNo1 != pRecvPacketMemoryMessage->uStationNo2)
	//{
	//	//만일 목적지 Station 이 자신의 Station 이면
	//	if(m_uStationNo == pRecvPacketMemoryMessage->uStationNo2)
	//	{
	//		//타 Station 의 메시지가 근원지인 자신의 Station 으로 되돌아 온것이다.
	//		//자신의 Client 로 중계한다. Work Item...
	//		m_fnWriteLog("CInternalServer::m_fnProcessReceive -> Return Message(SMP) Process error ");
	//	}
	//	//만일 목적지 Station 이 자신의 Station 이 아니면
	//	if(m_uStationNo != pRecvPacketMemoryMessage->uStationNo2 && 0 != pRecvPacketMemoryMessage->uStationNo2)
	//	{
	//		//타 Staion message 전송을 한다.
	//		nRet = m_fnSendMessageStation(pPerSocketContext, pRecvPacketMemoryMessage->uStationNo2);
	//		ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnSendMessageStation error : %d", nRet);
	//	}
	//}



	//switch(pRecvPacketMemoryMessage->uReadWriteFlag)
	//{
	//	//만일 메모리 Read 요청 이라면
	//case VS_MEMORY_READ:
	//	{
	//		switch(pRecvPacketMemoryMessage->uAccessType)
	//		{
	//			//ID 를 통한 접근이라면
	//		case VS_MEMORY_ACCESS_ID:
	//			{
	//				int nReadLength;
	//				//Access ID 통해 Memory Read 요청한다. Access ID 를 통해 Read하게 되면 얼만큼 읽었는지를 반환 받아야 한다.
	//				nRet = m_pSharedMemory->m_fnVSMemoryRead(pRecvPacketMemoryMessage->uFileNo, pRecvPacketMemoryMessage->cAccessID, 
	//					pRecvPacketMemoryMessage->cParamBuf, &nReadLength);
	//				//SMP Return 값과 얼만큼을 읽었는가를 설정한다.
	//				pRecvPacketMemoryMessage->uSMPReturnValue = nRet;
	//				pRecvPacketMemoryMessage->uSize = nReadLength;
	//				break;
	//			}
	//			//직접접근이라면
	//		case VS_MEMORY_ACCESS_DIRECT:
	//			{
	//				//직접 접근을 통해 Memory Read 요청한다.
	//				nRet = m_pSharedMemory->m_fnVSMemoryRead(pRecvPacketMemoryMessage->uFileNo, pRecvPacketMemoryMessage->uOffset, 
	//					pRecvPacketMemoryMessage->uSize, pRecvPacketMemoryMessage->cParamBuf);
	//				//SMP Return 값을 설정한다.
	//				pRecvPacketMemoryMessage->uSMPReturnValue = nRet;
	//				break;
	//			}
	//		}
	//		break;
	//	}
	//	//만일 메모리 Write 요청 이라면
	//case VS_MEMORY_WRITE:
	//	{
	//		int		nRet;

	//		switch(pRecvPacketMemoryMessage->uAccessType)
	//		{
	//			//ID 를 통한 접근이라면
	//		case VS_MEMORY_ACCESS_ID:
	//			{
	//				//Accssor의 기록을 하기 위해 정수형 값을 문자열 값으로 변환한다.
	//				char cAccessor[MAX_SMP_ACCESSOR_LEN];
	//				_itoa_s(pPerSocketContext->uClientID, cAccessor, MAX_SMP_ACCESSOR_LEN, 10);

	//				//Access ID 통해 Memory write 요청한다.
	//				nRet = m_pSharedMemory->m_fnVSMemoryWrite(pRecvPacketMemoryMessage->uFileNo, pRecvPacketMemoryMessage->cAccessID, 
	//					pRecvPacketMemoryMessage->cParamBuf, cAccessor);
	//				pRecvPacketMemoryMessage->uSMPReturnValue = nRet;
	//				break;
	//			}
	//			//직접접근이라면
	//		case VS_MEMORY_ACCESS_DIRECT:
	//			{
	//				//직접 접근 통해 Memory write 요청한다.
	//				nRet = m_pSharedMemory->m_fnVSMemoryWrite(pRecvPacketMemoryMessage->uFileNo, pRecvPacketMemoryMessage->uOffset, 
	//					pRecvPacketMemoryMessage->uSize, pRecvPacketMemoryMessage->cParamBuf);
	//				pRecvPacketMemoryMessage->uSMPReturnValue = nRet;
	//				break;
	//			}
	//		}
	//		break;
	//	}
	//}
	////결과값을 전송한다.
	//{
	//	PER_IO_CONTEXT* pPerSendIoContext;
	//	//pPerSendIoContext = new PER_IO_CONTEXT;
	//	pPerSendIoContext = m_pMemoryPool->Alloc();

	//	PACKET_HEADER*				pSendPacketHeader;
	//	PACKET_MEMORY_MESSAGE*		pSendPacketMemoryMessage;

	//	//Send Buffer, Recv Buffer 에서 Packet Header 와 Body 부분의 시작점을 가지고 온다.
	//	pSendPacketHeader			= (PACKET_HEADER*)(pPerSendIoContext->Buffer);
	//	pSendPacketMemoryMessage	= (PACKET_MEMORY_MESSAGE*)(pPerSendIoContext->Buffer + sizeof(PACKET_HEADER));


	//	//결과값 전송을 위한 Packet 을 Send Buffer 에 만든다.
	//	nRet = m_fnMakeMemoryMessage(pPerSocketContext, pPerSendIoContext);
	//	ERROR_RETURN("CInternalServer::m_fnSendMessageSharedMemory -> m_fnMakeMemoryMessage error : %d", nRet);

	//	//근원지로 되돌려 보낸다.
	//	//만일 SendData 내용 중에 목적지 Station 이 자신의 Station 이면 자신의 Client 로 결과값을 보낸다.
	//	if(m_uStationNo == pSendPacketMemoryMessage->uStationNo2 || 0 == pSendPacketMemoryMessage->uStationNo2)
	//	{
	//		nRet = m_fnSendMessageClient(pPerSocketContext->socket, pPerSendIoContext);
	//		ERROR_RETURN("CInternalServer::m_fnSendMessageSharedMemory -> m_fnSendMessageClient error : %d", nRet);
	//	}
	//	//그렇지 않고 만일 타 Station 이면 타 Station 메시지 전송 기능을 이용한다.
	//	else 
	//	{
	//		nRet = m_fnSendMessageStation(pPerSocketContext, pSendPacketMemoryMessage->uStationNo2);
	//		ERROR_RETURN("CInternalServer::m_fnSendMessageSharedMemory -> m_fnSendMessageStation error : %d", nRet);
	//	}
	//}

//	return	nErrorCode;
}

/*
*	Module name	:	m_fnMakeMemoryMessage
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Packet MEMORY_MESSAGE 를 만든다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnMakeMemoryMessage(PER_SOCKET_CONTEXT* pPerSocketContext, PER_IO_CONTEXT* pPerSendIOContext)
{
	return OKAY;

	//int							nErrorCode = OKAY;
	//USHORT						nTemp;
	//PACKET_HEADER*				pRecvPacketHeader,*pSendPacketHeader;
	//PACKET_MEMORY_MESSAGE*		pRecvPacketMemoryMessage,*pSendPacketMemoryMessage;

	////Send Buffer, Recv Buffer 에서 Packet Header 와 Body 부분의 시작점을 가지고 온다.
	//pRecvPacketHeader			= (PACKET_HEADER*)(pPerSocketContext->recvContext->Buffer);
	//pRecvPacketMemoryMessage	= (PACKET_MEMORY_MESSAGE*)(pPerSocketContext->recvContext->Buffer + sizeof(PACKET_HEADER));
	//pSendPacketHeader			= (PACKET_HEADER*)(pPerSendIOContext->Buffer);
	//pSendPacketMemoryMessage	= (PACKET_MEMORY_MESSAGE*)(pPerSendIOContext->Buffer + sizeof(PACKET_HEADER));

	////Header 정보를 설정한다.
	//pSendPacketHeader->uPacketSize		= sizeof(PACKET_HEADER) + sizeof(PACKET_MEMORY_MESSAGE);
	//pSendPacketHeader->PacketDataType	= MEMORY_MESSAGE_DATA;
	//pSendPacketHeader->uDataSize		= sizeof(PACKET_MEMORY_MESSAGE);

	////body 정보를 설정한다.
	////Receive 내용을 Send buffer 에 전개한다.
	//memcpy(pSendPacketMemoryMessage, pRecvPacketMemoryMessage, sizeof(PACKET_MEMORY_MESSAGE));

	////SendData 내용 중 Source Station 과 Destination Station 을 서로 바꾼다.
	//nTemp = pSendPacketMemoryMessage->uStationNo1;
	//pSendPacketMemoryMessage->uStationNo1 = pSendPacketMemoryMessage->uStationNo2;
	//pSendPacketMemoryMessage->uStationNo2 = nTemp;

	////Packet 의 보낼 크기를 지정한다.
	//pPerSendIOContext->wsaBuf.buf		= pPerSendIOContext->Buffer;
	//pPerSendIOContext->wsaBuf.len		= sizeof(PACKET_HEADER) + sizeof(PACKET_MEMORY_MESSAGE);
	//pPerSendIOContext->uBufferLength	= sizeof(PACKET_HEADER) + sizeof(PACKET_MEMORY_MESSAGE);
	//pPerSendIOContext->uBufferOffset	= 0;
	//return nErrorCode;
}

/*
*	Module name	:	m_fnSetSocketReceiveMode
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client Socket 을 Receive Mode로 전환한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnSetSocketReceiveMode(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	int			nErrorCode = OKAY;
	int			nRet;
	DWORD		dwRecvNumBytes = 0, dwFlags=0;

	memset(&(pPerSocketContext->recvContext->overlapped), 0, sizeof(OVERLAPPED));

	//Socket을 Receive mode로 전환한다.
	nRet = WSARecv(pPerSocketContext->socket, &(pPerSocketContext->recvContext->wsaBuf), 1, 
		&dwRecvNumBytes, &dwFlags, &(pPerSocketContext->recvContext->overlapped), NULL);

	//만일 Receive mode 전환이 Error 라면 Client Socket을 종료한다.
	if(SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError()))
	{
		//Client 의 연결을 종료한다.
		nErrorCode = nRet;
		m_fnWriteLog("CInternalServer::m_fnRunThread -> IOCP Receive Mode Change Error.. Disconnect Someone Client!!");
		nRet = m_fnCloseClient(pPerSocketContext);
		if(OKAY != nRet)
			SAFE_DELETE(pPerSocketContext);

	}

	return nErrorCode;
}

/*
*	Module name	:	m_fnProcessReceive
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client로부터 Receive 되어지는 Message를 처리한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnProcessReceive(PER_SOCKET_CONTEXT* pPerSocketContext, ULONG dwReceiveSize)
{
	int				nErrorCode = OKAY;
	int				nRet;
	PACKET_HEADER*	pRecvPacketHeader;

	//PER_SOCKET_CONTEXT 내용 중 Receive Buffer Offset에 현재 Offset과 Receive 된 Size를 합친다.
	pPerSocketContext->recvContext->uBufferOffset += dwReceiveSize;

	//해더를 완성할 때까지 다시 전송 요구한다.
	{
		//만일 Buffer Offset 이 Message Header 의 Size 보다 작다면 
		if(pPerSocketContext->recvContext->uBufferOffset < sizeof(PACKET_HEADER))
		{
			//Receive Buffer 의 시작 포인트를 현재 Buffer 의 마지막 받은 위치로 이동 후 IOCP 에 Receive 요청을 한다.
			pPerSocketContext->recvContext->wsaBuf.buf += dwReceiveSize;
			pPerSocketContext->recvContext->wsaBuf.len -= dwReceiveSize;
			nRet = m_fnSetSocketReceiveMode(pPerSocketContext);
			ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnSetSocketReceiveMode error : %d", nRet);

			//정상 종료(Header 크기 만큼 받지 못했으면 종료하여서 현재 함수로 재진입 하게 한다.)
			return nErrorCode;
		}
	}

	//해더가 완성 되었다.. Body 가 완성 될때까지 다시 전송 요구한다.
	{
		pRecvPacketHeader = &(pPerSocketContext->recvContext->PacketHeader);
		//만일 Receive Size 가 Header 의 전체 Packet Size 보다 작다면 
		if(pRecvPacketHeader->uPacketSize > pPerSocketContext->recvContext->uBufferOffset)
		{
			if (pPerSocketContext->recvContext->pBuffer == NULL)
			{
				pPerSocketContext->recvContext->pBuffer = new BYTE[pRecvPacketHeader->uPacketSize];
				memcpy(pPerSocketContext->recvContext->pBuffer, pRecvPacketHeader, sizeof(PACKET_HEADER));
				pPerSocketContext->recvContext->wsaBuf.buf = (CHAR*)pPerSocketContext->recvContext->pBuffer;
				pPerSocketContext->recvContext->wsaBuf.len = pRecvPacketHeader->uPacketSize;

				pPerSocketContext->recvContext->wsaBuf.buf += pPerSocketContext->recvContext->uBufferOffset;
				pPerSocketContext->recvContext->wsaBuf.len -= pPerSocketContext->recvContext->uBufferOffset;
			}
			else
			{
				//Receive Buffer 의 시작 포인트를 현재 Buffer 의 마지막 받은 위치로 이동 후 IOCP 에 Receive 요청을 한다.
				pPerSocketContext->recvContext->wsaBuf.buf += dwReceiveSize;
				pPerSocketContext->recvContext->wsaBuf.len -= dwReceiveSize;
			}
			nRet = m_fnSetSocketReceiveMode(pPerSocketContext);
			ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnSetSocketReceiveMode error : %d", nRet);

			//정상 종료(Data Size 만큼 받지 못했으면 종료하여 현재 함수로 재진입 시킨다.)
			return nRet;
		}
		else if(pRecvPacketHeader->uPacketSize == 0)
		{
			nRet = ERR_INSERVER_RECEIVE_PACKET_DATA_IS_NULL;
			ERROR_RETURN("CInternalServer::m_fnProcessReceive -> Receive Packet Data is 0. error : %d", nRet);
			return nRet;
		}
	}

	//Body 가 완성 되었다. 이하 메시지 타입에 맞는 처리한다.
	{
		switch(pRecvPacketHeader->PacketDataType)
		{
			//만일 Header 의 Message Type 이 Client Message 라면
		case CLIENT_MESSAGE_DATA:
			{
				//Client 에서 요청하는 타 Client로의 메시지 전송을 처리한다.
				nRet = m_fnProcessClientMessage(pPerSocketContext);
				if(OKAY != nRet)
				{	
					nErrorCode = nRet;
					//m_fnWriteLog("CInternalServer::m_fnProcessReceive -> m_fnProcessClientMessage error");
				}
				break;
			}
			//만일 Header 의 Message Type 이 공유메모리 참조 라면
		case MEMORY_MESSAGE_DATA:
			{
				//Client 에서 요청하는 공유메모리 접근 요청을 처리한다.
				nRet = m_fnProcessSharedMemoryAccess(pPerSocketContext);
				ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnProcessSharedMemoryAccess error : %d", nRet);

				if (pPerSocketContext->recvContext->pBuffer != NULL)
				{
					delete[] pPerSocketContext->recvContext->pBuffer;
					pPerSocketContext->recvContext->pBuffer = NULL;
				}
				break;
			}
			//만일 Header 의 Message Type 이 Client 의 Visual Station 등록 메시지
		case CLIENT_VS_REGIST_DATA:
			{
				//Client 에서 요청하는 Client 등록을 처리한다.
				nRet = m_fnProcessClientRegist(pPerSocketContext);
				if(OKAY != nRet)
				{
					nErrorCode = nRet;

					DWORD dwBytesTransferred;

					if(pPerSocketContext->recvContext->pBuffer != NULL)
					{
						delete[] pPerSocketContext->recvContext->pBuffer;
						pPerSocketContext->recvContext->pBuffer = NULL;
					}

					//시작 버퍼와 크기를 소켓 종료를 위해 이상 상태로 설정한다.
					pPerSocketContext->recvContext->wsaBuf.buf = NULL;
					pPerSocketContext->recvContext->wsaBuf.len = 0;
					//Receive Size 를 남은 패킷으로 설정 한다.
					dwBytesTransferred = -1;
					//Buffer Offset 을 초기화 한다.
					pPerSocketContext->recvContext->uBufferOffset = 0;
					//IOCP 큐에 넣는다.
					PostQueuedCompletionStatus(m_hIOCP, dwBytesTransferred,	(ULONG_PTR)pPerSocketContext, 
						(LPOVERLAPPED)&(pPerSocketContext->recvContext->overlapped));

					m_fnWriteLog("CInternalServer::m_fnProcessReceive -> m_fnProcessClientRegist error : %d", nErrorCode);
					return nErrorCode;
				}
				
				if(pPerSocketContext->recvContext->pBuffer != NULL)
				{
					delete[] pPerSocketContext->recvContext->pBuffer;
					pPerSocketContext->recvContext->pBuffer = NULL;
				}

				break;
			}
			//그외라면 Error 처리한다. 전송 요청한 곳으로 메시지를 보내야 하지만 추후 구현..Work item
		default:
			{
				nErrorCode = NG;
				m_fnWriteLog("CInternalServer::m_fnProcessReceive -> Client Packet data type error");
				break;
			}
		}
	}

	//delete[] pPerSocketContext->recvContext->pBuffer;
	pPerSocketContext->recvContext->pBuffer = NULL;
	pPerSocketContext->recvContext->DataReset();
	//Socket Receive 대기한다.
	nRet = m_fnSetSocketReceiveMode(pPerSocketContext);
	//printf("CInternalServer::m_fnProcessReceive -> Remaind packet.. retry recv.. remaind  %d \n", pPerSocketContext->recvContext->uBufferOffset);
	ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnSetSocketReceiveMode error.. 2 : %d", nRet);

	////Buffer 의 남는 Data 처리
	//{
	//	USHORT uPacketSize;
	//	//전체 Packet Size 를 가지고 온다.
	//	uPacketSize = pRecvPacketHeader->uPacketSize;										
	//	//처리된 Packet 보다 더 많이 받았 을 수 있으므로 처리 해야될 Packet 을 계산한다.
	//	//		pPerSocketContext->recvContext->uBufferOffset -= (USHORT)dwReceiveSize;
	//	pPerSocketContext->recvContext->uBufferOffset -= uPacketSize;
	//	//		m_fnWriteLog("total packet size : %d, Clientis : %d, Remaind is %d", uPacketSize, pPerSocketContext->uClientID, pPerSocketContext->recvContext->uBufferOffset);
	//	//처리되지 않은 남은 Data 를 앞으로 끌어 온다.
	//	CopyMemory(pPerSocketContext->recvContext->Buffer, pPerSocketContext->recvContext->Buffer + uPacketSize, 
	//		pPerSocketContext->recvContext->uBufferOffset);
	//	//Buffer 의 남는 공간을 초기화 한다.
	//	memset(pPerSocketContext->recvContext->Buffer + pPerSocketContext->recvContext->uBufferOffset, 
	//		0, DEFAULT_IO_BUFFER - pPerSocketContext->recvContext->uBufferOffset);

	//	////만일 남은 Data 크기가 헤더의 크기보다 작으면
	//	//if(pPerSocketContext->recvContext->uBufferOffset < sizeof(PACKET_HEADER))
	//	//{
	//	//시작 버퍼와 Receive 할 크기를 지정한다.
	//	pPerSocketContext->recvContext->wsaBuf.buf = pPerSocketContext->recvContext->Buffer + pPerSocketContext->recvContext->uBufferOffset;
	//	pPerSocketContext->recvContext->wsaBuf.len = DEFAULT_IO_BUFFER - pPerSocketContext->recvContext->uBufferOffset;

	//	//Socket Receive 대기한다.
	//	nRet = m_fnSetSocketReceiveMode(pPerSocketContext);
	//	//printf("CInternalServer::m_fnProcessReceive -> Remaind packet.. retry recv.. remaind  %d \n", pPerSocketContext->recvContext->uBufferOffset);
	//	ERROR_RETURN("CInternalServer::m_fnProcessReceive -> m_fnSetSocketReceiveMode error.. 2 : %d", nRet);
	//	//}
	//	////그렇지 않고 남은 Data 크기가 헤더의 크기보다 크면 (추가적으로 Receive 할 필요 없고 다시 워커쓰레드로 들어오면 된다.)
	//	//else
	//	//{
	//	//	DWORD dwBytesTransferred;
	//	//
	//	//	//시작 버퍼와 크기를 처음 상태로 한다.
	//	//	pPerSocketContext->recvContext->wsaBuf.buf = pPerSocketContext->recvContext->Buffer;
	//	//	pPerSocketContext->recvContext->wsaBuf.len = MAX_BUFFER;
	//	//	//Receive Size 를 남은 패킷으로 설정 한다.
	//	//	dwBytesTransferred = pPerSocketContext->recvContext->uBufferOffset;
	//	//	//Buffer Offset 을 초기화 한다.
	//	//	pPerSocketContext->recvContext->uBufferOffset = 0;
	//	//	//printf("CInternalServer::m_fnProcessReceive -> PostQueuedCompletionStatus() execute\n");
	//	//	//IOCP 큐에 넣는다.
	//	//	//m_fnWriteLog("CInternalServer::m_fnProcessReceive -> PostQueuedCompletionStatus() execute..");
	//	//	PostQueuedCompletionStatus(m_hIOCP, dwBytesTransferred,	(ULONG_PTR)pPerSocketContext, 
	//	//		(LPOVERLAPPED)&(pPerSocketContext->recvContext->overlapped));
	//	//}
	//}
	return nErrorCode;
}

/*
*	Module name	:	m_fnProcessSend
*	Parameter	:	PER_SOCKET_CONTEXT pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client로 Send 후 발생 되어지는 IOCP로 부터의 호출을 처리한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.1
*	Revision
*		- 보낸 size 가 0 보다 작다면 Socket 관련 에러이므로 Sendbuffer 를  삭제
*/
int CInternalServer::m_fnProcessSend(SOCKET hSendSocke,  PER_IO_CONTEXT* pPerSendIOContext, ULONG dwSendSize)
{
	int				nErrorCode = OKAY;
	int				nRet;

	//보낸 size 가 0 보다 작다면 Socket 관련 에러이므로 Sendbuffer 를  삭제한다.
	if(dwSendSize <= 0)
	{
		if (pPerSendIOContext->pBuffer != NULL)
		{
			delete[] pPerSendIOContext->pBuffer;
			pPerSendIOContext->pBuffer = NULL;
		}
		m_pMemoryPool->Free(pPerSendIOContext);
		return nErrorCode;
	}

	//PER_SOCKET_CONTEXT 내용 중 Send Buffer Offset에 현재 Offset과 Send 된 Size를 합친다.
	pPerSendIOContext->uBufferOffset += dwSendSize;

	//만일 전송된 Size가 전체 보내야 할 Packet 보다 작다면
	if(pPerSendIOContext->uBufferOffset < pPerSendIOContext->uBufferLength)
	{
		//Send Buffer 의 시작 Pointer를 Send Offset 만큼 증가 시킨다.
		//pPerSendIOContext->wsaBuf.buf	= pPerSendIOContext->Buffer + pPerSendIOContext->uBufferOffset;
		pPerSendIOContext->wsaBuf.buf	+= dwSendSize;
		//총 Send 할 Size 를 이전에 보낸 Size 만큼 뺀다.
		pPerSendIOContext->wsaBuf.len	-= dwSendSize;
		//해당 Socket으로 남은 Packet을 재 전송 한다.
		m_fnWriteLog("Send packet remained : %d \n", pPerSendIOContext->wsaBuf.len);
		nRet = m_fnSendMessageClient(hSendSocke, pPerSendIOContext);
		ERROR_RETURN("CInternalServer::m_fnProcessSend -> m_fnSendMessageClient error : %d", nRet);
	}
	//그렇지 않고 만일 그 외 라면
	else
	{	
		//m_fnWriteLog("Msg Send. Size = %d\n, ", pPerSendIOContext->uBufferOffset );

		//printf("Send Comp : %d\n", pPerSocketContext->uClientID);
		//Send Buffer를 초기화 한다.
		//SAFE_DELETE(pPerSendIOContext);
		if (pPerSendIOContext->pBuffer != NULL)
		{
			delete[] pPerSendIOContext->pBuffer;
			pPerSendIOContext->pBuffer = NULL;
		}
		m_pMemoryPool->Free(pPerSendIOContext);
	}
	return nErrorCode;
}



/*
*	Module name	:	m_fnGetServerParameter
*	Parameter	:	
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	공유메모리로부터 Server 구동에 필요한 정보를 가지고 온다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnGetServerParameter()
{


	return 1;
}

/*
*	Module name	:	m_fnGetClientContext
*	Parameter	:	Client ID
*	Return		:	정상 종료 : Client 의 PPER_SOCKET_CONTEXT
*					이상 종료 : NULL
*	Function	:	Client Index를 검색하여 ClientID 를 가지는 Client 정보를 반환한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
PER_SOCKET_CONTEXT* CInternalServer::m_fnGetClientContext(USHORT uClientID)
{
	PER_SOCKET_CONTEXT* lpSocketContext;

	std::deque<PER_SOCKET_CONTEXT*>::iterator it = m_pListClientIndex->begin();

	//Client ID가 PER_SOCKET_CONTEXT 리스트 내에 있는지를 찾는다.
	for(; it != m_pListClientIndex->end(); it++)
	{
		//만일 Client ID가 PER_SOCKET_CONTEXT 리스트 내 있다면 
		lpSocketContext = *it;
		if(lpSocketContext->uClientID == uClientID)
		{	//해당 Client 의 PER_SOCKET_CONTEXT를 반환한다.
			return lpSocketContext;
		}
	}
	return NULL;
}

/*
*	Module name	:	m_fnProcessClientMessage
*	Parameter	:	Client ID
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Client 에서 요청하는 타 Client 로의 메시지 전송을 처리한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
int CInternalServer::m_fnProcessClientMessage(PER_SOCKET_CONTEXT* pPerSocketContext)
{
	int						nErrorCode = OKAY;
	int						nRet;
	PACKET_CLIENT_MESSAGE*	pRecvPacketData;
	PER_SOCKET_CONTEXT*		pPerSocketContextDestination;	
	PACKET_HEADER*			pPacketHeader;

	pPacketHeader	= (PACKET_HEADER*)pPerSocketContext->recvContext->pBuffer;
	pRecvPacketData = (PACKET_CLIENT_MESSAGE*)(pPerSocketContext->recvContext->pBuffer + sizeof(PACKET_HEADER));

	//만일 근원지 Station No 가 0 이면 자신의 Station 번호를 넣는다.
	if(0 == pRecvPacketData->uStationNo1)
		pRecvPacketData->uStationNo1 = m_uStationNo;

	//만일 목적지 Station 이 자신의 Station 이 아니면
	if(m_uStationNo != pRecvPacketData->uStationNo2 && 0 != pRecvPacketData->uStationNo2)
	{
		//타 Staion message 전송을 한다.
		nRet = m_fnSendMessageStation(pPerSocketContext, pRecvPacketData->uStationNo2);
		ERROR_RETURN("CInternalServer::m_fnProcessClientMessage -> m_fnSendMessageStation error : %d", nRet);
		return nErrorCode;
	}

	//client ID 를 가지고 목적지 Client 의 PER_SOCKET_CONTEXT 정보를 가지고 온다.
	pPerSocketContextDestination = m_fnGetClientContext(pRecvPacketData->uTaskNo2);
	if(NULL == pPerSocketContextDestination)
	{

		if (pPerSocketContext->recvContext->pBuffer != NULL)
		{
			delete[] (pPerSocketContext->recvContext->pBuffer);
			pPerSocketContext->recvContext->pBuffer = NULL;
		}

		//없는 경우 응답 필요... Work item..
		//pRecvPacketData->uSendOkFlag = NG;
		nErrorCode = NG;
		//m_fnWriteLog("CInternalServer::m_fnProcessClientMessage -> m_fnGetClientContext error");
		return nErrorCode;
	}

	//m_fnWriteLog("Message Send %d -> %d", pPerSocketContext->uClientID, pPerSocketContextDestination->uClientID );

	//목적지 Client 로 전송한다.
	{
		int	nPacketSize;
		PER_IO_CONTEXT*	pPerSendIoContext;
		nPacketSize			= pPacketHeader->uPacketSize;
		//pPerSendIoContext	= new PER_IO_CONTEXT;
		pPerSendIoContext	= m_pMemoryPool->Alloc();


		//목적지 PER_SOCKET_CONTEXT 의 Send buffer 에 근원지 PER_SOCKET_CONTEXT 의 Receive buffer 의 내용을 전개 한다.
		//memcpy(pPerSendIoContext->Buffer, pPerSocketContext->recvContext->Buffer, nPacketSize);
		pPerSendIoContext->pBuffer			= pPerSocketContext->recvContext->pBuffer;
		//보낼 Data 양을 결정 한다.
		pPerSendIoContext->wsaBuf.buf		= (CHAR*)pPerSendIoContext->pBuffer;
		pPerSendIoContext->wsaBuf.len		= nPacketSize;
		pPerSendIoContext->uBufferLength	= nPacketSize;
		pPerSendIoContext->uBufferOffset	= 0;
		//목적지 Client 로 전송한다.
		nRet = m_fnSendMessageClient(pPerSocketContextDestination->socket, pPerSendIoContext);

		ERROR_RETURN("CInternalServer::m_fnProcessClientMessage -> m_fnSendMessageClient error : %d", nRet);
	}
	return nErrorCode;
}

/* 
*	Module name	:	m_fnTimer
*	Parameter	:	Timer ID, PER_SOCKET_CONTEXT Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	타이머 Callback 함수, Client 응답에 대한 time out 처리를 한다.
*	Create		:	2006.03.01
*	Author		:	김용태
*	Version		:	1.0
*/
void CALLBACK CInternalServer::m_fnTimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	int nRet;

	PER_TIMER_CLIENT_RESPONS*	pPerTimerData = (PER_TIMER_CLIENT_RESPONS*)(DWORD_PTR)dwUser;
	CInternalServer*			pInternalServer;
	PER_SOCKET_CONTEXT*			pPerSocketContext;

	//internal server 의 pointer 와 Client 의 PER_SOCKET_CONTEXT 포인터 정보를 가지고 온다. 
	pInternalServer		= (CInternalServer*)pPerTimerData->pInternalServer;	//Static 함수 내에서 nonStatic 함수에 접근하기 위함이다.
	pPerSocketContext	= pPerTimerData->pPerSocketContext;

	pInternalServer->m_fnWriteLog("CInternalServer::m_fnTimer -> Client Respons time out!! Client id is %d.",
		pPerSocketContext->uClientID);

	//PER_TIMER_CLIENT_RESPONS를 리스트 내에서 삭제한다. 이때 구조체 소멸자에서 Timer 도 소멸한다.
	nRet = pInternalServer->m_fnDeleteTimerClientRespons(pPerTimerData);
	if(OKAY != nRet)
	{
		pInternalServer->m_fnWriteLog("CInternalServer::m_fnTimer -> m_fnDeleteTimerClientRespons error");
	}

	//PER_SOCKET_CONTEXT 를 참고하여 Client 연결을 종료 처리한다.
	/*nRet = pInternalServer->m_fnCloseClient(pPerSocketContext);
	if(OK != nRet)
	{
	pInternalServer->m_fnWriteLog("CInternalServer::m_fnTimer -> m_fnCloseClient error");
	}*/
	SAFE_CLOSE_SOCKET(pPerSocketContext->socket);
	//SAFE_DELETE(pPerSocketContext);
}


