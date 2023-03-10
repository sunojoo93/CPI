// Header File 의 중복 Include 를 막기 위한 Macro guard
#pragma once

#include "thread.h"

class CInternalServer;

/*
*	Module name	:	CClientListener
*	Function	:	Client 접속을 처리하는 Thread
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.0
*/
class CClientListener :
	public CThread
{
public:										//생성자
	CClientListener(CInternalServer *pInternalServer, char* pszServerIP, WORD wServerPort, HANDLE hIOCP);
	~CClientListener(void);					//소멸자

	/*멤버 함수*/
public:
	int					m_fnStartServer();	//Listen Socket을 생성하고 Accept Thread를 생성한다.

											//Listen Socket의 핸들을 반환한다.
	SOCKET				m_fnGetListenSocket();
											

private:
	HANDLE				m_fnStartAccept();	//Accept Thread를 구동시킨다.
	virtual void		m_fnRunThread();	//Accept Thread로서 Client Accept의 실질적인 기능을 담당한다.
											
											//Accept 된 Client Socket을 IOCP 에 등록한다.
	PER_SOCKET_CONTEXT*	m_fnRegistIOCP(SOCKET hClientsocket);

											//Accept 만 되고 Internal Server에 등록 되지 않은 Client Socket 을 종료한다.
	void				m_fnCloseClient(PER_SOCKET_CONTEXT* pPerSocketContext);

											//Log를 기록한다.
	void				m_fnWriteLog(const char* pszLog, ...);


	/*멤버 변수*/
private:
	bool				m_bRun;				//Thread의 실행 및 정지 Flag
	SOCKET				m_hListenSocket;	//Listen Socket 의 Handle
	char				m_szServerIP[16];	//Server IP
	WORD				m_wServerPort;		//Server Port
	CInternalServer*	m_pInternalServer;	//Internal Server의 pointer, 필요에 따라 Internal Server로의 Function Call로 기능을 수행한다.
	HANDLE				m_hIOCP;			//IOCP Handle
};
