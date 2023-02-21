#pragma once

/*
*	Structure name	:	PACKET_DATA_TYPE
*	Function		:	Packet 의 Data 부분이 가지는 Packet 종류를 나타낸다.
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
enum PACKET_DATA_TYPE
{
	CLIENT_MESSAGE_DATA,											//Client 에서 보내는 메시지
	MEMORY_MESSAGE_DATA,											//Client 에서 보내는 메모리 메시지
	VS_COMMAND_DATA,												//Visual Station 에서 Client로 보내는 메시지
	CLIENT_VS_REGIST_DATA											//Client 의 Visual Station 등록 메시지
};

/*
*	Structure name	:	PACKET_HEADER
*	Function		:	Packet Header 
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PACKET_HEADER
{
	ULONG				uFullMemorySize;							//실제 전송하지 않는 패킷이 존재할경우, 포함된 전체 사이즈
	ULONG				uPacketSize;								//실제 전송할 총 패킷 Size,
	PACKET_DATA_TYPE	PacketDataType;								//Message Type
	ULONG				uDataSize;									//Data Packet Size
	PACKET_HEADER()
	{
		memset(this, 0x00, sizeof(PACKET_HEADER));
	}
};

/*
*	Structure name	:	PACKET_CLIENT_MESSAGE
*	Function		:	Client 에서 보내는 메시지 
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.1
*	- 1.1
*		- Function No, Sequence No 항목 삭제, Server 에서 메시지를 보낼때 정보가 필요하지 않음, ParamBuf 내에 CMDMSG 구조체정보가 들어가 있으므로 필요치 않다.
*/
struct PACKET_CLIENT_MESSAGE
{
	USHORT				uStationNo1;								//Source Station
	USHORT				uStationNo2;								//Destination Station
	USHORT				uTaskNo1;									//Source Task
	USHORT				uTaskNo2;									//Destination Task
	DWORD				dwTimeOut;									//Message 응답 Time out
	ULONG				uParamBufSize;								//Parameter Size
	USHORT				uSendOkFlag;								//목적지 Client 정상 전송이 되었는지 판단할 때 사용한다.(intenal server에서 설정, OK,NG 값을 갖는다.) Work Item
	INT64				pParamBuf;									//Parameter			// 2016.09.28 가변 사이즈 파라메터 전달 작업

	PACKET_CLIENT_MESSAGE()
	{
		memset(this, 0x00, sizeof(PACKET_CLIENT_MESSAGE));
	}
};

/*
*	Structure name	:	PACKET_MEMORY_MESSAGE
*	Function		:	Client 에서 보내는 Memory 참조 메시지 
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PACKET_MEMORY_MESSAGE
{
	USHORT				uStationNo1;								//Source Station
	USHORT				uStationNo2;								//Destination Station
	USHORT				uTaskNo;									//Source Task
	USHORT				uTimeout;									//Message 응답 Time out
	USHORT				uReadWriteFlag;								//Memory Read, Write Flag
	USHORT				uAccessType;								//메모리 번지 직접 접근, ID를 통한 접근
	USHORT				uFileNo;									//직접 접근 시 파일 No
	USHORT				uOffset ;									//직접 접근 시 Offset
	USHORT				uSize;										//직접 접근 시 Size
	HANDLE				hEvent;										//응답을 대기하고 있는 Client Thread 를 깨우기 위한 Event Handle
	USHORT				uSMPReturnValue;							//SMP 접근시 발생된 리턴 값. Client DLl 에서 이값으로 리턴을 한다. 
	INT64				pthis;										//자기 자신의 Pointer, Client 에서 요청한 응답이 돌아왔을때 Client 에서 사용하기 위함
//	PACKET_MEMORY_MESSAGE* pthis;									//자기 자신의 Pointer, Client 에서 요청한 응답이 돌아왔을때 Client 에서 사용하기 위함
	char				cAccessID[MAX_SHARED_MEMORY_ACCESS_ID_NAME];//ID 접근시의 ID name
	char				cParamBuf[MAX_SMP_BUFFER];					//파라메터		// 2016.09.28 가변 사이즈 파라메터 전달 작업

	PACKET_MEMORY_MESSAGE()
	{
		memset(this, 0x00, sizeof(PACKET_MEMORY_MESSAGE));
	}
};

/*
*	Structure name	:	VS_COMMAND
*	Function		:	Visual Station Command
*						Server 에서 Client 로 보내는 명령에 대한 enum 형
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
enum VS_COMMAND
{
	VS_COMMAND_LINK_TEST,											//Link Test 명령
	VS_COMMAND_CLIENT_ID_REQUEST,									//Client 로 Client ID 전송을 요청한다.
	VS_COMMAND_DISCONNECT_CLIENT,									//Client 연결 해지 통보
	VS_COMMAND_REGIST_CLIENT_OK,									//Client 정상 등록 통보
	VS_COMMAND_REGIST_CLIENT_ID_DUPLICATE,							//Client ID 중복 통보
	VS_COMMAND_REGIST_NG,											//Client 등록 실패
	VS_COMMAND_REGIST_CLIENT_DLL_INCORRECT							//Client DLL 버전이 맞지 않다.
};

/*
*	Structure name	:	PACKET_VS_COMMAND
*	Function		:	Server 에서 Client 로 보내는 Client 동작 명령  
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PACKET_VS_COMMAND
{
	VS_COMMAND			VsCommand;									//Visual Station 명령
	char				cParamBuf[MAX_VS_COMMAND_PARAMETER_BUFFER];	//파라메터 값이 필요한 경우 사용한다.

	PACKET_VS_COMMAND()
	{
		memset(this, 0x00, sizeof(PACKET_VS_COMMAND));
	}
};

/*
*	Structure name	:	PACKET_CLIENT_VS_REGIST
*	Function		:	Client 에서 보내는 Visual Station 등록 요청 메시지 
*	Create			:	2006.02.20
*	Author			:	김용태
*	Version			:	1.0
*/
struct PACKET_CLIENT_VS_REGIST
{
	USHORT				uClientID;									//등록 할 Client ID
	char				cClientName[MAX_VS_REGIST_CLIENT_NAME];		//등록 할 Client name
	UINT				uClientSocketDLLVersion;					//Client Socket DLL Version 를 VS Server 로 전달하여 버전을 맞추게한다. 맞지 않으면 Disconnect..

	PACKET_CLIENT_VS_REGIST()
	{
		memset(this, 0x00, sizeof(PACKET_CLIENT_VS_REGIST));
	}
};