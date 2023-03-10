#include "StdAfx.h"
#include ".\messagereceiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/*
*	Module name	:	CMessageReceiver
*	Parameter	:	수신 메시지 큐 포인터, Client Socket, Application Callback Function, Socket 연결 유무 Flag 의 pointer
*	Return		:	없음
*	Function	:	생성자
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
CMessageReceiver::CMessageReceiver(CMessageQueue<BYTE*>* pReceiveMessageQueue, SOCKET hClientSocket, 
								   CMessageQueue<BYTE*>* pAppPeekMessageQueue, bool* pbSocketConnect /* = NULL */)
{
	//메시지 수신자를 Pointer 로 가지고 있는다.
	m_pReceiveMessageQueue	= pReceiveMessageQueue;
	//멤버 변수 초기화
	m_bRun					= true;
	m_hClientSocket			= hClientSocket;
	m_pszBuffer				= NULL;
	m_pbSocketConnect		= pbSocketConnect;
	m_nRemainOffset			= 0;
	m_pAppPeekMessageQueue	= pAppPeekMessageQueue;
	//memset(m_szRemainBuffer, 0, MAX_BUFFER);
}

/*
*	Module name	:	~CMessageReceiver
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	소멸자
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
CMessageReceiver::~CMessageReceiver(void)
{
	//스레스 수행 Flag 를 false 로 해서 쓰레드를 종료 하게 한다.
	m_bRun = false; 
	Sleep(10);
	SuspendThread(m_hThreadHandle);
	/*if(m_pszBuffer)
	{
		delete (PPER_PACKET_BUFFER)m_pszBuffer;
		m_pszBuffer = NULL;
	}*/
	//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
	if(m_pszBuffer)
	{
		delete[] (BYTE*)m_pszBuffer;
		m_pszBuffer = NULL;
	}

	TerminateThread(m_hThreadHandle,NULL);

	//Socket 연결 유무 Flag 를 False 로 한다.
	if(NULL != m_pbSocketConnect)
		*m_pbSocketConnect = false;

	m_fnWaitForTermination();
}

/*
*	Module name	:	m_fnStartReceiver
*	Parameter	:	없음
*	Return		:	정상 종료 : 생성된 Thread 의 Handle
*					이상 종료 : NULL 
*	Function	:	메시지 수신자를 시작한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
HANDLE CMessageReceiver::m_fnStartReceiver()
{
	//Thread를 생성한다
	//만일 Thread 생성 핸들 값이 Null 이 아니라면 Thread 의 핸들 값을 반환한다.
	//그렇지 않고 만일 Null 이라면 Null을 반환한다.
	m_hThreadHandle = m_fnStartThread();
	return m_hThreadHandle;
}

/*
*	Module name	:	m_fnRunThread
*	Parameter	:	없음
*	Return		:	없음
*	Function	:	메시지 수신자 Thread 기능을 담당한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
void CMessageReceiver::m_fnRunThread()
{
	int		nRet;

	//구문을 반복 한다. 쓰레드 수행 플레그가 False 일때 까지.
	while(m_bRun)
	{
		//Message Buffer 를 동적으로 생성한다. 수행중 비정상 동작을 하면 바로 Delete 되어 지고, 정상적으로 큐에 넣어지면 큐에서 POP 된 후 삭제 된다.
		//m_pszBuffer = (char*) new PER_PACKET_BUFFER;
		//m_pszBuffer = (char*)MemoryPool.Alloc();
		m_pszBuffer = NULL;

		//memset(m_pszBuffer, 0, MAX_BUFFER);

		//서버로부터 메시지 버퍼에 메시지를 받는다.
		nRet = m_fnReceiveMessage();
		//만일 쓰레드 수행 Flag 가 Fals 라면 종료한다.
		if(!m_bRun) 
		{
			//Socket 연결 유무 Flag 를 False 로 한다.
			if(NULL != m_pbSocketConnect)
				*m_pbSocketConnect = false;

			/*if(m_pszBuffer)
			{
				delete (PPER_PACKET_BUFFER)m_pszBuffer;
				m_pszBuffer = NULL;
			}*/
			//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
			if (m_pszBuffer != NULL)
			{
				delete[] m_pszBuffer;
				//free(m_pszBuffer);
				m_pszBuffer = NULL;
			}
			break;
		}
		//만일 메시지 수신 결과가 이상 종료라면
		if(OKAY != nRet)
		{
			//동적 생성된 메모리를 삭제한다.
			/*if(m_pszBuffer)
			{
				delete (PPER_PACKET_BUFFER)m_pszBuffer;
				m_pszBuffer = NULL;
			}*/
			//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);

			if (m_pszBuffer != NULL)
			{
				delete[] m_pszBuffer;
				//free(m_pszBuffer);
				m_pszBuffer = NULL;
			}

			TRACE("CMessageReceiver::m_fnRunThread -> m_fnReceiveMessage error : %d\n", nRet);
			
			//만일 Socket Error 가 ERROR_IO_PENDING 이 아니라면  Application 에 전송을 할 필요가 있다..Work Item.. Blocking socket 이므로 IO Pending이 발생되지 않을것임. 
			if(ERROR_IO_PENDING != nRet)
			{
				//동적생성된 메모리를 삭제한다.
				/*if(m_pszBuffer)
				{
					delete (PPER_PACKET_BUFFER)m_pszBuffer;
					m_pszBuffer = NULL;
				}*/
				//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
				//Socket 을 종료 한다.
				closesocket(m_hClientSocket);
				TRACE("CMessageReceiver::m_fnRunThread -> m_fnReceiveMessage error : %d.. Disconnect to server\n", nRet);
				return;
			}
			//만일 그외 라면 메시지 수신을 다시 시도 한다.
			continue;
		}
		
		//메시지를 수신 메시지 버퍼에 넣는다.
		nRet = m_pReceiveMessageQueue->m_fnPushMessage(m_pszBuffer);
		if(OKAY != nRet)
		{	
			/*if(m_pszBuffer)
			{
				delete (PPER_PACKET_BUFFER)m_pszBuffer;
				m_pszBuffer = NULL;
			}*/
			//MemoryPool.Free((PER_PACKET_BUFFER*)m_pszBuffer);
			if (m_pszBuffer != NULL)
			{
				delete[] m_pszBuffer;
				//free(m_pszBuffer);
				m_pszBuffer = NULL;
			}

			TRACE("CMessageReceiver::m_fnRunThread -> m_pReceiveMessageQueue->m_fnPushMessage error : %d\n", nRet);
		}
		m_pszBuffer = NULL;
	}
}

/*
*	Module name	:	m_fnReceiveMessage
*	Parameter	:	
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	 
*	Create		:	
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageReceiver::m_fnReceiveMessage(BYTE* buffer, int len)
{
	int						nRet			= 0;
	int						nReceiveOffset	= 0;
	int						nErrorCode		= OKAY;

	while(1)
	{
		//Server 로부터 Packet 을 버퍼로 수신한다.
		nRet = recv(m_hClientSocket, (char*)buffer + nReceiveOffset, len - nReceiveOffset, 0);

		//만일 Socket Error 가 발생 되었다면
		if(SOCKET_ERROR == nRet)	
		{	
			nErrorCode = WSAGetLastError();
			//Application 으로 Socket 종료를 알린다.
			m_fnSendToAppSocketCloseMessage();
			//이상 종료 한다.
			TRACE("CMessageReceiver::m_fnReceiveMessage -> recv error : %d\n", nErrorCode);
			return nErrorCode;
		}
		else if(0 == nRet)
		{
			nErrorCode = WSAGetLastError();
			//Application 으로 Socket 종료를 알린다.
			m_fnSendToAppSocketCloseMessage();
			TRACE("CMessageReceiver::m_fnReceiveMessage -> recv size is 0.. Server disconnect.. \n");
			return nErrorCode;
		}
		//TRACE("Receive Size is %d \n",nRet);

		//만일 수신한 Packet 이 Header 구성 Size 보다 작다면 
		if(len > nReceiveOffset + nRet)
		{
			//TRACE("sizeof(PACKET_HEADER) > nReceiveOffset + nRet \n");
			//총 받은 크기를 저장한다.
			nReceiveOffset += nRet;
			//다시 받기 시도한다.
			continue;
		}
		break;
	}


	//SYSTEMTIME systime;						//System 시간을 알기 위한 변수
	//::GetLocalTime(&systime);
	//TRACE("[%02d:%02d:%02d.%03d] Msg Received. Size = %d \n", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, len);

	return nErrorCode;
}


/*
*	Module name	:	m_fnReceiveMessage
*	Parameter	:	Buffer Pointer
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	서버로부터 메시지를 수신대기 하고, 수신 완료 된 Packet 을 Buffer 에 넣는다. 
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageReceiver::m_fnReceiveMessage()
{
	int						nRet			= 0;
	int						nErrorCode		= OKAY;
	int						nReceiveOffset	= 0;
	PACKET_HEADER			PacketHeader;
	//BYTE*					pRcvBuffer		= NULL;

	// Packet header 를 받아서 총 패킷 사이즈를 구한다.
	nRet = m_fnReceiveMessage((BYTE*)&PacketHeader, sizeof(PACKET_HEADER));
	if (nRet != OKAY)
	{
		return nRet;
	}

	// 받아야 할 총 패킷 크기을 구한후 필요한 크기만큼 메모리를 동적 생성한다.
	m_pszBuffer = new BYTE[PacketHeader.uFullMemorySize];
	//pRcvBuffer = (BYTE*)malloc(PacketHeader.uPacketSize);
	memcpy(m_pszBuffer, &PacketHeader, sizeof(PACKET_HEADER));

	// 남은 패킷을 받는다.
	nRet = m_fnReceiveMessage(m_pszBuffer + sizeof(PACKET_HEADER), PacketHeader.uDataSize);
	if (nRet != OKAY)
	{
		return nRet;
	}

	return nErrorCode;
}

/*
*	Module name	:	m_fnSendToAppSocketCloseMessage
*	Parameter	:	없음
*	Return		:	정상 종료 : 1
*					이상 종료 : 1을 제외한 나머지 값
*	Function	:	Application 으로 Socket 이 끊겼음을 알린다.
*	Create		:	2006.04.05
*	Author		:	김용태
*	Version		:	1.0
*/
int CMessageReceiver::m_fnSendToAppSocketCloseMessage()
{
	int nErrorCode = OKAY;

	//Socket 연결 유무 Flag 를 False 로 한다.
	if(NULL != m_pbSocketConnect)
		*m_pbSocketConnect = false;

	//Application Calback Function 을 파라메터 NUL,NULL 로 호출한다. APP에서는 Socket Close 처리가 필요하다.
	//m_fnAppCallBackFunc(NULL, NULL);		
	nErrorCode = m_pAppPeekMessageQueue->m_fnPushMessage((BYTE*)-1);

	return nErrorCode;
}