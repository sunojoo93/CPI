// SockVisionClt.cpp : ���� �����Դϴ�.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SockVisionClt.h"

// CSockVisionClt

CSockVisionClt::CSockVisionClt()
{
}

CSockVisionClt::CSockVisionClt(CAsyncSocket* pServerSock)
{
	m_pSockVisionSvr = pServerSock;

	memset(m_chBuffer, NULL, SOCK_BUFF_LENGTH);
	m_nBufferSize = 0;
}

CSockVisionClt::~CSockVisionClt()
{
}


// CSockVisionClt ��� �Լ�


void CSockVisionClt::OnClose(int nErrorCode)
{
	CSockVisionSvr* pServerSock = (CSockVisionSvr*)m_pSockVisionSvr;

	if( pServerSock != NULL )
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_VISION_DISCONNECT, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnClose(nErrorCode);
}


void CSockVisionClt::OnConnect(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CAsyncSocket::OnConnect(nErrorCode);
}


void CSockVisionClt::OnReceive(int nErrorCode)
{
	CSockVisionSvr* pServerSock = (CSockVisionSvr*)m_pSockVisionSvr;

	if(pServerSock != NULL)
	{
		pServerSock->m_pParentWnd->SendMessage(WM_SOCK_VISION_RECEIVE, WPARAM(nErrorCode), LPARAM(this));
	}

	CAsyncSocket::OnReceive(nErrorCode);
}
