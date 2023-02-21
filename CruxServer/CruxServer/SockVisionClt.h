#pragma once

// CSockVisionClt 명령 대상입니다.
#include "SockVisionSvr.h"

class CSockVisionClt : public CAsyncSocket
{
public:
	CSockVisionClt();
	CSockVisionClt(CAsyncSocket* pServerSock);
	virtual ~CSockVisionClt();

	CString m_strIPAddress;
	int		m_nPortNo;

	int m_nVisionID;
	int m_nSignalFlagID;
	char m_chBuffer[SOCK_BUFF_LENGTH];
	int m_nBufferSize;

	CAsyncSocket* m_pSockVisionSvr;

	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


