#pragma once

#include "SockCtrlPCClt.h"

// CSockCtrlPCSvr 명령 대상입니다.

class CSockCtrlPCSvr : public CAsyncSocket
{
public:
	CSockCtrlPCSvr(CWnd* pParentWnd);
	virtual ~CSockCtrlPCSvr();
	virtual void OnAccept(int nErrorCode);

	BOOL m_fnCreateSock(CWnd* pParentWnd, UINT nPort);
	void m_fnCloseClient();
	int m_fnSendClient(const void* pBuf, int nBufLen, int nFlags = 0);

	CWnd* m_pParentWnd;
	CSockCtrlPCClt* m_pSockClient;
	
	BOOL m_bServerRun;
};


