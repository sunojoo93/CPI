#pragma once

#include "thread.h"

class CInternalServer;

/*
*	Module name	:	CClientMessenger
*	Function	:	Client 의 메시지를 처리하는 Thread
*	Create		:	2006.02.21
*	Author		:	김용태
*	Version		:	1.0
*/
class CClientMessenger :
	public CThread
{
public:		
																//생성자
	CClientMessenger(CInternalServer* pInternalServer, HANDLE hIOCP);
	~CClientMessenger(void);									//소멸자

	/*멤버 함수*/
public:
	HANDLE				m_fnStartMessenger();					//Client 의 Message를 처리할 Worker Thread를 생성한다.

private:
	void				m_fnRunThread();						//Client 의 Message를 처리할 Worker Thread
	void				m_fnWriteLog(const char* pszLog, ...);	//Log를 기록한다.

	/*멤버 변수*/
private:
	bool				m_bRun;									//Thread의 실행 및 정지 Flag
	CInternalServer*	m_pInternalServer;						//Internal Server의 pointer
	HANDLE				m_hIOCP;								//IOCP Handle
};
