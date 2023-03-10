
#pragma once

/*
*	Structure name	:	PER_IO_CONTEXT
*	Function		:	IOCP 에서 사용하는 Packet 저장 IO Buffer
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PER_IO_CONTEXT
{
	WSAOVERLAPPED		overlapped;					//IOCP 에 연결 되는 구조체
	WSABUF				wsaBuf;						//Send/Receive 할 크기와 Buffer 의 시작 포인터를 가지는 구조체
	ULONG				uBufferOffset;				//Send/Receive Buffer 시작 포인터 관리를 위한 Offset
	ULONG				uBufferLength;				//총 Send/Receive 되어질 Size
	PACKET_HEADER		PacketHeader;				//Packet header를 받고 나머지 패킷을 동적생성한다.
	//char				Buffer[DEFAULT_IO_BUFFER];	//Buffer
	BYTE*				pBuffer;					//Buffer
	PER_IO_CONTEXT()
	{
		memset(&PacketHeader, 0, sizeof(PACKET_HEADER));
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		//memset(Buffer, 0, DEFAULT_IO_BUFFER);
		wsaBuf.buf		= (CHAR*)&PacketHeader;
		wsaBuf.len		= sizeof(PACKET_HEADER);
		uBufferOffset	= 0;
		uBufferLength	= 0;
		pBuffer	= NULL;
	}
	~PER_IO_CONTEXT()
	{
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}
	void DataReset()
	{
		memset(&PacketHeader, 0, sizeof(PACKET_HEADER));
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		//memset(Buffer, 0, DEFAULT_IO_BUFFER);
		wsaBuf.buf		= (CHAR*)&PacketHeader;
		wsaBuf.len		= sizeof(PACKET_HEADER);
		uBufferOffset	= 0;
		uBufferLength	= 0;
		if (pBuffer != NULL)
		{
			delete[] pBuffer;
			pBuffer = NULL;
		}
	}
};

/*
*	Structure name	:	PER_SOCKET_CONTEXT
*	Function		:	Client INDEX, Client Socket 관리, IOCP Message buffer 에 사용됨 
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PER_SOCKET_CONTEXT
{
	SOCKET				socket;								//Client Socket
	PER_IO_CONTEXT*		recvContext;						//Receive 시 사용되는 자료
	USHORT				uClientID;							//해당 socket 의 Client ID
	char				szClientName[MAX_CLIENT_NAME_LEN];	//해당 Socket 의 Client name
	PER_SOCKET_CONTEXT(SOCKET sc)
	{ 
		socket				= sc;
		recvContext			= new PER_IO_CONTEXT;
		uClientID			= 0;
		memset(szClientName, 0, MAX_CLIENT_NAME_LEN);
	}
	~PER_SOCKET_CONTEXT()
	{
		SAFE_DELETE(recvContext);
		SAFE_CLOSE_SOCKET(socket);
	}
};

/*
*	Structure name	:	PER_TIMER_CLIENT_RESPONS
*	Function		:	Client 응답에 대한 Timer 들을 관리 하기 위함
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
typedef struct _PER_TIMER_CLIENT_RESPONS
{
	VS_COMMAND			VSCommand;					//어떤 Visual Station 명령에 대한 것인가를 나타냄
	PER_SOCKET_CONTEXT* pPerSocketContext;			//해당 Client 의 정보
	UINT				nTimerID;					//Timer ID
	void*				pInternalServer;			//Internal Server 의 Pointer.
	_PER_TIMER_CLIENT_RESPONS()
	{
		pPerSocketContext = NULL;
	}

	~_PER_TIMER_CLIENT_RESPONS()
	{
		//SAFE_DELETE(pPerSocketContext);			//timer 소멸만 필요 할것으로 보임
		::timeKillEvent(nTimerID);
	}
}PER_TIMER_CLIENT_RESPONS;
