// SockCtrlPCClt.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SockCtrlPCClt.h"
#include "SockCtrlPCSvr.h"

// CSockCtrlPCClt

CSockCtrlPCClt::CSockCtrlPCClt()
{
}

CSockCtrlPCClt::CSockCtrlPCClt(CAsyncSocket* pServerSock)
{
	m_pSockCtrlSvr = pServerSock;

	memset(m_chBuffer, NULL, SOCK_BUFF_LENGTH);
	m_nBufferSize = 0;
}

CSockCtrlPCClt::~CSockCtrlPCClt()
{
}


// CSockCtrlPCClt 멤버 함수


void CSockCtrlPCClt::OnClose(int nErrorCode)
{
	CSockCtrlPCSvr* pServerSock = (CSockCtrlPCSvr*)m_pSockCtrlSvr;

	if(pServerSock != NULL)
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CONTROL_DISCONNECT, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnClose(nErrorCode);
}


void CSockCtrlPCClt::OnConnect(int nErrorCode)
{
	CSockCtrlPCSvr* pServerSock = (CSockCtrlPCSvr*)m_pSockCtrlSvr;

	if(pServerSock != NULL)
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CONTROL_CONNECT, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnConnect(nErrorCode);
}


void CSockCtrlPCClt::OnReceive(int nErrorCode)
{
	CSockCtrlPCSvr* pServerSock = (CSockCtrlPCSvr*)m_pSockCtrlSvr;

	if(pServerSock != NULL)
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_CONTROL_RECEIVE, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

BOOL CSockCtrlPCClt::m_fnConnectToServer(CString strIPAddr, int nPort)
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