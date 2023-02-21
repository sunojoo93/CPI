// SockCIMSvr.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "SockCIMSvr.h"

// CSockCIMSvr

CSockCIMSvr::CSockCIMSvr(CWnd* pParentWnd)
{
	m_pParentWnd = pParentWnd;
	m_pSockClient = NULL;
	m_bServerRun = FALSE;
}

CSockCIMSvr::~CSockCIMSvr()
{
}


// CSockCIMSvr 멤버 함수


void CSockCIMSvr::OnAccept(int nErrorCode)
{
	m_pSockClient = new CSockCIMClt(this);

	if (CAsyncSocket::Accept(*m_pSockClient))
	{
		CString strAddress;
		UINT nPort;

		//ClientSock->GetPeerName(strAddress, nPort);
		m_pSockClient->GetSockName(strAddress, nPort);
		m_pSockClient->m_strIPAddress = strAddress;
		m_pSockClient->m_nPortNo = nPort;

		m_pParentWnd->SendMessage(WM_SOCK_CIM_ACCEPT, WPARAM(nErrorCode), LPARAM(m_pSockClient));
	}
	else
	{
		delete m_pSockClient;
		G_AddLog(3, 2, _T("CIM Server Socket Error : Accept() - Error Code = %d"), GetLastError());
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

BOOL CSockCIMSvr::m_fnCreateSock(CWnd *pParentWnd, UINT nPort)
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

	G_AddLog(3, 2, _T("CIM Server Socket Create! Port = %d"), nPort);

	return TRUE;	
}

void CSockCIMSvr::m_fnCloseClient()
{
	if(m_pSockClient)
	{
		m_pSockClient->Close();

		delete m_pSockClient;
		m_pSockClient = NULL;
	}
}