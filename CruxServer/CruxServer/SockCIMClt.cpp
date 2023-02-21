
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// SockCIMClt.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "SockCIMClt.h"
#include "SockCIMSvr.h"

// CSockCIMClt

CSockCIMClt::CSockCIMClt()
{
}

CSockCIMClt::CSockCIMClt(CAsyncSocket* pServerSock)
{
	m_pSockCIMSvr = pServerSock;

	memset(m_chBuffer, NULL, SOCK_BUFF_LENGTH);
	m_nBufferSize = 0;
}

CSockCIMClt::~CSockCIMClt()
{
}


// CSockCIMClt 멤버 함수


void CSockCIMClt::OnClose(int nErrorCode)
{
	CSockCIMSvr* pServerSock = (CSockCIMSvr*)m_pSockCIMSvr;

	if( pServerSock != NULL )
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CIM_DISCONNECT, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnClose(nErrorCode);
}


void CSockCIMClt::OnConnect(int nErrorCode)
{
	CSockCIMSvr* pServerSock = (CSockCIMSvr*)m_pSockCIMSvr;

	if( pServerSock != NULL )
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CIM_CONNECT, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnConnect(nErrorCode);
}


void CSockCIMClt::OnReceive(int nErrorCode)
{
	CSockCIMSvr* pServerSock = (CSockCIMSvr*)m_pSockCIMSvr;

	if(pServerSock != NULL)
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CIM_RECEIVE, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CSockCIMClt::m_fnConnectToServer(CString strIPAddr, int nPort)
{
	m_strIPAddress = strIPAddr;
	m_nPortNo = nPort;

	if(m_strIPAddress == "" || m_nPortNo == 0) return FALSE;

	if(CAsyncSocket::Connect(m_strIPAddress, m_nPortNo) == FALSE)
	{
		if(GetLastError() != WSAEWOULDBLOCK)
			return FALSE;
	}

	return TRUE;
}