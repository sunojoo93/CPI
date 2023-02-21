// SockCtrlPCSvr.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SockCtrlPCSvr.h"


// CSockCtrlPCSvr

CSockCtrlPCSvr::CSockCtrlPCSvr(CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	m_pSockClient = NULL;
	m_bServerRun = FALSE;
}

CSockCtrlPCSvr::~CSockCtrlPCSvr()
{
}


// CSockCtrlPCSvr 멤버 함수


void CSockCtrlPCSvr::OnAccept(int nErrorCode)
{
	m_pSockClient = new CSockCtrlPCClt(this);

	if (CAsyncSocket::Accept(*m_pSockClient))
	{
		CString strAddress;
		UINT nPort;

		//ClientSock->GetPeerName(strAddress, nPort);
		m_pSockClient->GetSockName(strAddress, nPort);
		m_pSockClient->m_strIPAddress = strAddress;
		m_pSockClient->m_nPortNo = nPort;

		m_pParentWnd->SendMessage(WM_SOCK_CONTROL_ACCEPT, WPARAM(nErrorCode), LPARAM(m_pSockClient));
	}
	else
	{
		delete m_pSockClient;
		G_AddLog(3, 2, _T("Control PC Server Socket Error : Accept() - Error Code = %d"), GetLastError());
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

BOOL CSockCtrlPCSvr::m_fnCreateSock(CWnd *pParentWnd, UINT nPort)
{
	if (!CAsyncSocket::Create(nPort, SOCK_STREAM))
	{
#ifndef _DEBUG
		AfxMessageBox(_T("Error : Create() in socket"));
#endif
		return FALSE;
	}

	if (!CAsyncSocket::Listen())
	{
#ifndef _DEBUG
		AfxMessageBox(_T("Error : Listen() in socket"));
#endif
		return FALSE;
	}

	m_pParentWnd = pParentWnd;

	G_AddLog(3, 2, _T("Control PC Server Socket Create! Port = %d"), nPort);

	return TRUE;	
}

void CSockCtrlPCSvr::m_fnCloseClient()
{
	if(m_pSockClient)
	{
		m_pSockClient->Close();

		delete m_pSockClient;
		m_pSockClient = NULL;
	}
}

int CSockCtrlPCSvr::m_fnSendClient(const void* pBuf, int nBufLen, int nFlags)
{
	int nSend;

	nSend = m_pSockClient->Send(pBuf, nBufLen, nFlags);

	if(nSend == SOCKET_ERROR)
		G_AddLog(3, 2, _T("[Send CtrlPC] Error = %d"), GetLastError());
	else
		G_AddLog(3, 2, _T("[Send CtrlPC] %s"), pBuf);

	return nSend;
}