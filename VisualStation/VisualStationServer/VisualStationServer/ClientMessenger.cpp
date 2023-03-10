#include "StdAfx.h"
#include "clientmessenger.h"
#include "InternalServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	CClientMessenger
*	Parameter	:	호출한 Class(Internal Server)의 Pointer
*	Return		:	없음
*	Function	:	생성자, 피 사용 Class로부터 수행되며 Class 의 생성을 담당한다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
CClientMessenger::CClientMessenger(CInternalServer* pInternalServer, HANDLE hIOCP)
{
	//전달받은 Internal Server Pointer를 자신의 Attribute에 전개한다.
	m_pInternalServer	= pInternalServer;
	m_hIOCP				= hIOCP;

	//Thread 수행 Flag를 true로 초기화 한다.
	m_bRun	= true;
}

/*
*	Module name	:	~CClientMessenger
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸자, 현재 수행되는 자신의 Thread를 종료시킨다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
CClientMessenger::~CClientMessenger(void)
{
	//Thread 수행 Flag를 false로 설정하여서 Thread를 종료할 수 있게 한다.
	m_bRun	= false;
	m_fnWaitForTermination();
}

/*
*	Module name	:	m_fnStartThread
*	Parameter	:	없음
*	Return		:	정상 종료 : Worker Thread Handle
					이상 종료 : Null
*	Function	:	Client 의 Message를 처리할 Worker Thread를 생성한다.
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
HANDLE CClientMessenger::m_fnStartMessenger()
{
	//Thread를 생성한다
	//만일 Thread 생성 핸들 값이 Null 이 아니라면 Thread 의 핸들 값을 반환한다.
	//그렇지 않고 만일 Null 이라면 Null을 반환한다.
	return m_fnStartThread();
}

/*
*	Module name	:	m_fnRunThread
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	Client 의 Message를 처리할 Worker Thread
*	Create		:	2006.02.24
*	Author		:	김용태
*	Version		:	1.0
*/
void CClientMessenger::m_fnRunThread()
{
	DWORD					dwIoSize = 0;
	PER_SOCKET_CONTEXT*		pPerSocketContext = NULL;
	LPOVERLAPPED			pOverlap = NULL;
	int						nRet = 0;

	m_fnWriteLog("CClientMessenger::m_fnRunThread -> Start..");

	//반복한다. Thread 수행 Flag 가 true 일때까지
	while(m_bRun)
	{
		//Thread를 IOCP Thread Pool로 등록한다.
		//Thread가 Pool 에서 활성화 될 때 Data Buffer 의 pointer 와 PER_SOCKET_CONTEXT pointer를 가져오게된다.
		//만일 Thread 대기 상태 해지 결과가 False로 반환되면
		if(FALSE == GetQueuedCompletionStatus( m_hIOCP, &dwIoSize, 
			(LPDWORD)&pPerSocketContext, (LPOVERLAPPED *)&pOverlap, INFINITE))
		{
			if(!m_bRun)
				break;
			
			if(ERROR_IO_PENDING == WSAGetLastError())
				continue;

			//Client Socket의 연결이 끊긴것임. Client 종료 처리
			m_fnWriteLog("CClientMessenger::m_fnRunThread -> GetQueuedCompletionStatus Error : %d, Client disconnect.",  WSAGetLastError());
			
			//만일 받은 Packet 이 Receive Data 이면 (구조체이므로 PER_SOCKET_CONTEXT 시작 지점은 OVERLAPPED 시작지점과 일치한다.)
			if(pOverlap == (LPOVERLAPPED)(pPerSocketContext->recvContext))
			{
				nRet = m_pInternalServer->m_fnCloseClient(pPerSocketContext);
				//if(pPerSocketContext->uClientID == 0)
				//{
				//	SAFE_DELETE(pPerSocketContext);
				//}
			}
			//그렇지 않고 만일 Packet 이 Send Data 이면
			else
			{
				//Internal Server 관리자의 Send Data 처리를 한다.
				m_pInternalServer->m_fnProcessSend(pPerSocketContext->socket, (PER_IO_CONTEXT*)pOverlap, -1L);
			}
			continue;
		}

		//Thread 수행 Flag 를 다시 체크한다. 대기중인 Thread pool 에서 활성화 되기 때문..
		if(!m_bRun)
			break;

		//IOCP 큐로 종료 메시지가 넘어오는 경우 Thread 를 종료 한다.
		if(NULL == pPerSocketContext)
		{
			m_fnWriteLog("CClientMessenger::m_fnRunThread -> IOCP thread terminate message received. Worker Thread end.",  WSAGetLastError());
			break;
		}		
		
		//만일 받은 Packet 이 Receive Data 이면 (구조체이므로 PER_SOCKET_CONTEXT 시작 지점은 OVERLAPPED 시작지점과 일치한다.)
		if(pOverlap == (LPOVERLAPPED)(pPerSocketContext->recvContext))
		{
			//만일 Client 소켓이 이상상태이면 소켓을 종료하고 메모리 할당을 지운다.
			if(-1 == dwIoSize)
			{
				//Receive Data 처리가 완료 될 때 까지 대기한다.

				m_fnWriteLog("CInternalServer::m_fnRunThread -> dwIoSize is -1 .. client disconnect!");
				nRet = m_pInternalServer->m_fnCloseClient(pPerSocketContext);
				if(OKAY != nRet)
					SAFE_DELETE(pPerSocketContext);
				continue;
			}

			//만일 Receive Size 가 0 이면 Client 를 종료한다.
			if(0 == dwIoSize)
			{						
		
				m_fnWriteLog("CClientMessenger::m_fnRunThread -> GetQueuedCompletionStatus Error. Receive data is 0. client disconnect! : Error no = %d",  WSAGetLastError());

				nRet = m_pInternalServer->m_fnCloseClient(pPerSocketContext);
				if(OKAY != nRet)
					SAFE_DELETE(pPerSocketContext);
				continue;

			}

			//이하 정상정인 Receive 구문이다.
			//Internal Server 관리자의 Receive message 처리를 한다.
			nRet = m_pInternalServer->m_fnProcessReceive(pPerSocketContext, (ULONG)dwIoSize);	
			if(nRet != OKAY)
			{
				m_fnWriteLog("CClientMessenger::m_fnRunThread -> m_fnProcessReceive Error. : Error no = %d",  nRet);

				if(nRet == ERR_INSERVER_RECEIVE_PACKET_DATA_IS_NULL)
				{
					nRet = m_pInternalServer->m_fnCloseClient(pPerSocketContext);
					if(OKAY != nRet)
						SAFE_DELETE(pPerSocketContext);
				}
				continue;
			}
		}
		//그렇지 않고 만일 Packet 이 Send Data 이면
		else
		{
			//Internal Server 관리자의 Send Data 처리를 한다.
			m_pInternalServer->m_fnProcessSend(pPerSocketContext->socket, (PER_IO_CONTEXT*)pOverlap, (ULONG)dwIoSize);
		}
	}
	m_fnWriteLog("CClientMessenger::m_fnRunThread -> End..");
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
void CClientMessenger::m_fnWriteLog(const char* pszLog, ...)
{
	char cBuffer[MAX_LOG_BUFFER];

	//로그 기록을 위한 임시 버퍼에 매개변수의 내용을 참고하여 로그 내용을 기록한다.
	va_list vaList; 
	va_start(vaList, pszLog); 
	vsprintf(cBuffer, pszLog, (va_list)vaList); 
	va_end(vaList);

	//CInternalServer의 Log 기록 Method를 호출한다.
	m_pInternalServer->m_fnWriteLog(cBuffer);
}


