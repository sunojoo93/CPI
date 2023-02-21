// SockVisionClt.cpp : 구현 파일입니다.
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


// CSockVisionClt 멤버 함수


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
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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
