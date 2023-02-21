#pragma once


// CSockCtrlPCClt 명령 대상입니다.

class CSockCtrlPCClt : public CAsyncSocket
{
public:
	CSockCtrlPCClt();
	CSockCtrlPCClt(CAsyncSocket* pServerSock);
	virtual ~CSockCtrlPCClt();

	CString m_strIPAddress;
	int		m_nPortNo;
	char m_chBuffer[SOCK_BUFF_LENGTH];
	int m_nBufferSize;

	CAsyncSocket* m_pSockCtrlSvr;

	BOOL m_fnConnectToServer(CString strIPAddr, int nPort);

	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


