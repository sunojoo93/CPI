#pragma once

// CSockVisionSvr 명령 대상입니다.

#include "SockVisionClt.h"

class CSockVisionSvr : public CAsyncSocket
{
public:
	CSockVisionSvr();
	virtual ~CSockVisionSvr();
	virtual void OnAccept(int nErrorCode);

	BOOL m_fnCreateSock(CWnd* pParentWnd, UINT nPort);
	void m_fnCloseClient(CAsyncSocket* pSockClient);
	void m_fnCloseClientAll();

	int m_fnSendClientAt(CAsyncSocket* pSockClient, CString strBuf, int nBufLen, int nFlags = 0);
	void m_fnSendClientAll(CString strBuf, int nBufLen, int nFlags = 0);
	CAsyncSocket* m_fnGetClient(CString strAddress);
	CAsyncSocket* m_fnGetClient(int nVisionID);
	int m_fnGetClientCount();

	CPtrList m_ListClientSock;
	CWnd* m_pParentWnd;

	BOOL m_bServerRun;
};


