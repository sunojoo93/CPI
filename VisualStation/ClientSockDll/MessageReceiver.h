#pragma once

#include "thread.h"

template <class T>
class CMessageQueue;

//template <class T>
//class CMemPooler;

//extern CMemPooler<PER_PACKET_BUFFER>	MemoryPool;								//Memory Pool

/*
*	Module name	:	CMessageReceiver
*	Function	:	메시지 수신자로서 Visual Station Server 에서 전송하는 Message 를 Receive Queue 에 전개 한다.
*	Create		:	2006.03.07
*	Author		:	김용태
*	Version		:	1.0
*/
class CMessageReceiver :
	public CThread
{
public:
	CMessageReceiver(CMessageQueue<BYTE*>* pReceiveMessageQueue, SOCKET hClientSocket, CMessageQueue<BYTE*>* pAppPeekMessageQueue, 
		bool* pbSocketConnect = NULL);
	~CMessageReceiver(void);

	/*멤버 함수*/
public:
	HANDLE					m_fnStartReceiver();								//메시지 수신자를 시작한다.

private:
	void					m_fnRunThread();									//메시지 수신자 Thread
	int						m_fnReceiveMessage();				//Message를 Receive해서 Buffer에 넣는다.
	int						m_fnReceiveMessage(BYTE* buffer, int len);			//Message를 Receive해서 Buffer에 넣는다.
	int						m_fnSendToAppSocketCloseMessage();					//Application 으로 Socket 이 끊겼음을 알린다.

	/*멤버 변수*/
private:
	CMessageQueue<BYTE*>*	m_pReceiveMessageQueue;								//Receive Message Queue Pointer
	bool					m_bRun;												//Thread의 실행 및 정지 Flag
	SOCKET					m_hClientSocket;									//Server 통신 하는 client Socket
	BYTE*					m_pszBuffer;										//Server 로부터 받는 메시지를 저장하는 포인터
	HANDLE					m_hThreadHandle;									//자신의 Thread Handle
	bool*					m_pbSocketConnect;									//Server 로 Socket이 연결 되었는지를 가진다.
	//char					m_szRemainBuffer[MAX_BUFFER];						//Server 로 부터 받는 packet 이 처리되고 남은 부분을 가지고 있는다.
	int						m_nRemainOffset;									//Packet Receive 후 처리되고 남은 Packet 이 얼마나 되는지를 가진다.
	CMessageQueue<BYTE*>*	m_pAppPeekMessageQueue;								//Client Application 에 전달하는 메시지
};
