#pragma once

#include "SockCIMClt.h"

// CSockCIMSvr ��� ����Դϴ�.

class CSockCIMSvr : public CAsyncSocket
{
public:
	CSockCIMSvr(CWnd* pParentWnd);
	virtual ~CSockCIMSvr();
	virtual void OnAccept(int nErrorCode);

	BOOL m_fnCreateSock(CWnd* pParentWnd, UINT nPort);
	void m_fnCloseClient();

	CWnd* m_pParentWnd;
	CSockCIMClt* m_pSockClient;

	BOOL m_bServerRun;
};


