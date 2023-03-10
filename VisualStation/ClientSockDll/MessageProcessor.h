#pragma once
#include "thread.h"

template <class T>
class CMessageQueue;

//template <class T>
//class CMemPooler;

//extern CMemPooler<PER_PACKET_BUFFER>	MemoryPool;							//Memory Pool


/*
*	Module name	:	CMessageProcessor
*	Function	:	메시지 큐의 내용을 확인하여 App Supporter 또는 Internal Server로의 메시지 중계기능을 한다.
*	Create		:	2006.03.08
*	Author		:	김용태
*	Version		:	1.0
*/
class CMessageProcessor :
	public CThread
{
public:
	CMessageProcessor(USHORT uTaskNo, CMessageQueue<BYTE*>* pMessageQueue, CMessageQueue<BYTE*>* pAppPeekMessageQueue, USHORT uProcessMode, 
		SOCKET hClientSocket, bool* pbSocketConnect = NULL );
	~CMessageProcessor(void);

	/*멤버 함수*/
public:
	HANDLE			m_fnStartMessageProcessor();							//메시지 처리자를 실행 시킨다. Thread 생성
	void			m_fnStopMessageProcessor();								//메시지 처리자 쓰레드를 종료 시킨다.
private:
	void			m_fnRunThread();										//메시지 수신자 Thread
	int				m_fnSendMessage(BYTE* pszBuffer);						//Buffer에 있는 Message를 Server 로 전송한다.
	int				m_fnSendMessage(BYTE* buffer, int len);					//Buffer에 있는 Message를 Server 로 전송한다.
	int				m_fnProcessClientMessageData(BYTE* pszBuffer);			//메시지 큐에서 꺼낸 내용인 ClientMessageData 를 처리한다.
	int				m_fnProcessMemoryMessageData(BYTE* pszBuffer);			//메시지 큐에서 꺼낸 내용인 MemoryMessageData 를 처리한다.
	int				m_fnProcessVsCommandData(BYTE* pszBuffer);				//메시지 큐에서 꺼낸 내용인 VsCommandData 를 처리한다.
	int				m_fnProcessClientVsRegistData(BYTE* pszBuffer);			//메시지 큐에서 꺼낸 내용인 ClientVsRegistData 를 처리한다.
	int				m_fnSendToAppSocketCloseMessage();						//Application 으로 Socket 이 끊겼음을 알린다.
	//int				m_fnSendResponsCommand(CMDMSG* pCmdMsg);				//근원 Cleint 로 응답 메시지를 전송한다.
	
	/*멤버 변수*/
private:
	CRITICAL_SECTION		m_csMessageSend;								//Message Send 에 대한 Ciritical Section
	USHORT					m_uTaskNo;										//자신의 Task No
	CMessageQueue<BYTE*>*	m_pMessageQueue;								//Send / Receive 메시지 큐의 Pointer
	USHORT					m_uProcessMode;									//Send / Receive 메시지 처리자 를 구분한다.
	bool					m_bRun;											//Thread 수행 Flag
	SOCKET					m_hClientSocket;								//Server 통신 하는 client Socket
	HANDLE					m_hThreadHandle;								//자신의 Thread Handle
	BYTE*					m_pszBuffer;									//메시지 큐에서 꺼내온 정보를 가진다.
	bool*					m_pbSocketConnect;								//Socket 연결 유무를 가지는 변수.. 생성자에서 초기화 되고 NULL 이 아닐경우 사용된다.
	HANDLE					m_hWaitStop;									//메시지 큐의 메시지 대기를 정지 시키는 이벤트
	CMessageQueue<BYTE*>*	m_pAppPeekMessageQueue;							//Client Application 에 전달하는 메시지

};