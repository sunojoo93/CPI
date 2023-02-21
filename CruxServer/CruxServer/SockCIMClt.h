#pragma once
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSockCIMClt 명령 대상입니다.

class CSockCIMClt : public CAsyncSocket
{
public:
	CSockCIMClt();
	CSockCIMClt(CAsyncSocket* pServerSock);
	virtual ~CSockCIMClt();

	CString m_strIPAddress;
	int		m_nPortNo;
	char m_chBuffer[SOCK_BUFF_LENGTH];
	int m_nBufferSize;

	CAsyncSocket* m_pSockCIMSvr;

	BOOL m_fnConnectToServer(CString strIPAddr, int nPort);

	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


