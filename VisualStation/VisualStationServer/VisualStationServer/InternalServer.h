#pragma once

template<class T>
class CItemList;
template<class T>
class CMemPooler;
class CClientListener;
class CLogWriter;
class CSharedMemory;
class CClientMessenger;


/*
*	Module name	:	CInternalServer
*	Function	:	Visual Station 의 실제적인 기능을 담당하는 Internal Server
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.0
*/
class CInternalServer
{
public:
	CInternalServer(char* pszServerIp = "127.0.0.1", WORD wPortNo = 5000, UINT uStationNo = 0);
	~CInternalServer(void);

	/*멤버 함수*/
public:
	int		m_fnStartInternalServer();										//Internal Server를 시작한다.
	int		m_fnRegistClientRequest(PER_SOCKET_CONTEXT* pPerSocketContext);	//Server 등록을 위해, Client로 Client ID 와 name 전송을 요청한다.
	void	m_fnWriteLog(const char* pszLog, ...);							//Log를 기록한다.																
	int		m_fnCloseClient(PER_SOCKET_CONTEXT* pPerSocketContext);			//Client 연결 및 관련된 할당 메모리를 제거한다. Client 로 종료에대한 전송을 하지 않는다.

																			//Client로부터 Receive 되어지는 Message를 처리한다.
	int		m_fnProcessReceive(PER_SOCKET_CONTEXT* pPerSocketContext, ULONG dwReceiveSize);
																			//Client로 Send 후 발생 되어지는 IOCP로 부터의 호출을 처리한다.
	int		m_fnProcessSend(SOCKET hSendSocke, PER_IO_CONTEXT* pPerSendIOContext, ULONG dwReceiveSize);

private:
																			//Packet VS_COMMAND 를 만든다.
	int		m_fnMakeVSCommand(PER_IO_CONTEXT* pPerSendIOContext, VS_COMMAND VsCommand, char* pszData);
																			//Packet MEMORY_MESSAGE 를 만든다.
	int		m_fnMakeMemoryMessage(PER_SOCKET_CONTEXT* pPerSocketContext, PER_IO_CONTEXT* pPerSendIOContext);

																			//Client로의 Message 전송 기능을 제공한다.
	int		m_fnSendMessageClient(SOCKET hSendSocket, PER_IO_CONTEXT* pPerSendIOContext);

																			//타 Station 으로 Message 전송 기능을 제공한다.
	int		m_fnSendMessageStation(PER_SOCKET_CONTEXT* pPerSocketContext, USHORT uStationNo);	

																			//Client 에서 요청하는 공유메모리 접근을 처리한다.
	int		m_fnProcessSharedMemoryAccess(PER_SOCKET_CONTEXT* pPerSocketContext);
	int		m_fnProcessClientRegist(PER_SOCKET_CONTEXT* pPerSocketContext);	//Client 에서 요청하는 Server 등록을 처리한다.
	int		m_fnProcessClientMessage(PER_SOCKET_CONTEXT* pPerSocketContext);//Client 에서 요청하는 타 Client 로의 메시지 전송을 처리한다.

	int		m_fnSetSocketReceiveMode(PER_SOCKET_CONTEXT* pPerSocketContext);//Client Socket 을 Receive Mode로 전환한다.

	int		m_fnGetServerParameter();										//공유메모리로부터 Server 구동에 필요한 정보를 가지고 온다.
	PER_SOCKET_CONTEXT*	m_fnGetClientContext(USHORT uClientID);				//Client Index를 검색하여 ClientID 를 가지는 Client 정보를 반환한다.
																			//Parameter 정보와 일치하는 PER_TIMER_CLIENT_RESPONS 를 지운다.
	int		m_fnDeleteTimerClientRespons(PER_TIMER_CLIENT_RESPONS* pPerTimerClientRespons);
	int		m_fnDeleteTimerClientRespons(PER_SOCKET_CONTEXT* pPerSocketContext, VS_COMMAND nCommand);
																			//타이머 Callback 함수, Client 응답에 대한 time out 처리를 한다.
	static void CALLBACK	m_fnTimer(UINT uID,	UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

	/*멤버 변수*/
private:
	CClientListener*						m_pClientListener;				//Clinet 연결 관리자 객체
	CLogWriter*								m_pLogWriter;					//로그관리자 객체
	CSharedMemory*							m_pSharedMemory;				//공유메모리 관리자 객체
	char									m_szServerIP[MAX_IP_LENGTH];	//Server IP
	WORD									m_wPortNo;						//Server Port Number
	UINT									m_uStationNo;					//자신의 Station No
	HANDLE									m_hIOCP;						//생성된 IOCP Handle
	DWORD									m_dwWorkerThread;				//IOCP Worker Thread 등록 개수
	CItemList<PER_SOCKET_CONTEXT*>*			m_pListClientIndex;				//Client Index (PER_SOCKET_CONTEXT *)를 저장하는 Queue
	CItemList<CClientMessenger*>*			m_pListMessenger;				//Client 메시지 처리자(IOCP Worker Thread)의 Handle 을 저장하는 Queue
	CItemList<PER_TIMER_CLIENT_RESPONS*>*	m_pListTimerClientRespons;		//Visual Station 에서 Client로의 명령에 대해 응답을 기다리는 타이머 리스트
	CMemPooler<PER_IO_CONTEXT>*				m_pMemoryPool;					//Memory Pool
//public:
	//CRITICAL_SECTION						m_csCriticalSection;			//Interanal Server 에서 사용하는 임계 영역
};
