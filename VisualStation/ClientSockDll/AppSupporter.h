// Header File 의 중복 Include 를 막기 위한 Macro guard
#pragma once

template <class T>
class CMessageQueue;
class CMessageProcessor;
class CMessageReceiver;

/*
*	Module name	:	CAppSupporter
*	Function	:	내부 Application에 Visual Station 64 기능을 지원 한다.
*					내부 메시지 큐를 통하여 Message Processor로 메시지 처리를 요청한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
class CAppSupporter
{
public:
	CAppSupporter();
	~CAppSupporter(void);

	/*멤버 함수*/
public:
	bool							m_bIsClientDLLStarted;						//Cleint DLL 이 시작 되었는지 가진다. 
	bool							m_bSocketConnect;							//Server 로 Socket이 연결 되었는지를 가진다.

public:																			//Task를 Visual Station Internal Server 에 등록 한다.
	int				m_fnRegisterTask(USHORT uTaskNo, char* pszTaskName, CMessageQueue<BYTE*>* pAppPeekMessageQueue, 
									char* pszServerIp = "127.0.0.1", WORD wPortNo = 5000);

	int				m_fnUnRegisterTask();										//Visual Station에서 등록 해지 한다.
	int				m_fnSendCommand(CMDMSG* pCmdMsg);							//타 Cleint 로 메시지를 전송한다.
	int				m_fnSendResponseCommand(CMDMSG* pCmdMsg);					//근원 Cleint 로 응답 메시지를 전송한다.
	int				m_fnSMPRead(SMPMSG* pSmpMsg);								//공유메모리 읽기 기능을 지원한다.
	int				m_fnSMPWrite(SMPMSG* pSmpMsg);								//공유메모리 쓰기 기능을 지원한다.
	int				m_fnSendCommandReset();										//메인프로세스에서 리스폰스리셋, 시퀀스 리셋을 할 경우 응답 대기중인 커맨드를 강제 정지시킨다.
	int				m_fnFreeMemory(BYTE* rcvPacket);
private:
	int				m_fnReceiveMessage(BYTE* pszBuffer, int buffSize);			//Message를 Receive해서 Buffer에 넣는다.
	int				m_fnSendMessage(BYTE* pszBuffer);							//Buffer에 있는 Message를 Server 로 전송한다.
	int				m_fnCompressMsg(CMDMSG* pCmdMsg);							//MsgBuffer 의 내용을 압축옵션에 따라 압축한다.
	ULONG			m_lUniqueNo;

	/*멤버 변수*/
private:
	SHORT							m_uTaskNo;									//자신의 Client ID(TaskNO)
	char							m_szClientName[MAX_VS_REGIST_CLIENT_NAME];	//자신의 Client name
	//CMessageQueue<BYTE*>*			m_pSendMessageQueue;						//Send 메시지 큐의 Pointer
	//CMessageQueue<BYTE*>*			m_pReceiveMessageQueue;						//Receive 메시지 큐의 Pointer
	std::deque<CMessageProcessor*>	m_deqMessageProcessor;						//메시지 처리자의 List
	CMessageReceiver*				m_pMessageReceiver;							//메시지 수신자 객체 pointer
	SOCKET							m_hClientSocket;							//Client Socket
	char							m_szServerIP[MAX_IP_LENGTH];				//Server IP
	WORD							m_wPortNo;									//Server Port Number
	CRITICAL_SECTION				m_csAppSupporter;							//Critical Section
	HANDLE							m_hResponseEmergencyStop;					//리스폰스리셋, 시퀀스 리셋을 할 경우 발생 시키는 이벤트,
	int								m_nWaitResponseCount;						//커맨드 응답을 기다리고 있는 수(쓰레드수), 비상정지용 이벤트

	CMessageQueue<BYTE*>*			m_pAppPeekMessageQueue;						//Client Application 에 전달하는 메시지

	//#ifdef _DEBUG
	//	CMessageQueue<BYTE*>			SendMessageQueue(true);						//Send Message Queue
	//#else
	CMessageQueue<BYTE*>			m_SendMessageQueue;							//Receive Message Queue
	//#endif
	CMessageQueue<BYTE*>			m_ReceiveMessageQueue;						//Receive Message Queue
};