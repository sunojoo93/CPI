// SockVisionSvr.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SockVisionSvr.h"


// CSockVisionSvr

CSockVisionSvr::CSockVisionSvr()
{
	m_pParentWnd = NULL;
	m_bServerRun = FALSE;
}

CSockVisionSvr::~CSockVisionSvr()
{
	
}


// CSockVisionSvr 멤버 함수


void CSockVisionSvr::OnAccept(int nErrorCode)
{
	CSockVisionClt* pSockClient = new CSockVisionClt(this);

	if (CAsyncSocket::Accept(*pSockClient))
	{
		CString strAddress;
		UINT nPort;

		pSockClient->GetPeerName(strAddress, nPort);
		//pSockClient->GetSockName(strAddress, nPort);
		pSockClient->m_strIPAddress = strAddress;
		pSockClient->m_nPortNo = nPort;

		m_ListClientSock.AddTail(pSockClient);
		m_pParentWnd->SendMessage(WM_SOCK_VISION_ACCEPT, WPARAM(nErrorCode), LPARAM(pSockClient));
	}
	else
	{
		delete pSockClient;
		G_AddLog(3, 2, _T("Vision Server Socket Error : Accept() - Error Code = %d"), GetLastError());
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

BOOL CSockVisionSvr::m_fnCreateSock(CWnd *pParentWnd, UINT nPort)
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

	G_AddLog(3, 2, _T("Vision Server Socket Create! Port = %d"), nPort);

	return TRUE;	
}

void CSockVisionSvr::m_fnCloseClient(CAsyncSocket* pSockClient)
{
	POSITION pos = m_ListClientSock.Find(pSockClient);

	if (pos != NULL)
	{
		pSockClient->Close();
		m_ListClientSock.RemoveAt(pos);

		delete pSockClient;
		pSockClient = NULL;
	}
}

void CSockVisionSvr::m_fnCloseClientAll()
{
	CSockVisionClt * pSockClient;
	POSITION pos = m_ListClientSock.GetHeadPosition();

	while(pos)
	{
		pSockClient = (CSockVisionClt*)m_ListClientSock.GetNext(pos);

		if(pSockClient)
		{
			pSockClient->Close();
			delete pSockClient;
			pSockClient = NULL;
		}
	}

	m_ListClientSock.RemoveAll();
}

int CSockVisionSvr::m_fnGetClientCount()
{
	return (int)m_ListClientSock.GetCount();	
}

CAsyncSocket * CSockVisionSvr::m_fnGetClient(CString strAddress/*int nIndex*/)
{
	CSockVisionClt * pSockClient;
	POSITION pos = m_ListClientSock.GetHeadPosition();

	while(pos)
	{
		pSockClient = (CSockVisionClt*)m_ListClientSock.GetNext(pos);

		if(pSockClient->m_strIPAddress == strAddress)
			return pSockClient;
	}

	return NULL;
}

CAsyncSocket * CSockVisionSvr::m_fnGetClient(int nVisionID)
{
	CSockVisionClt * pSockClient;
	POSITION pos = m_ListClientSock.GetHeadPosition();

	while(pos)
	{
		pSockClient = (CSockVisionClt*)m_ListClientSock.GetNext(pos);

		if(pSockClient->m_nVisionID == nVisionID)
			return pSockClient;
	}

	return NULL;
}

void CSockVisionSvr::m_fnSendClientAll(CString strBuf, int nBufLen, int nFlags)
{
	char chBuf[1024] = {0};
	WideCharToMultiByte(CP_ACP, NULL, strBuf, -1, chBuf,  1024, NULL, FALSE);

	nBufLen = strlen(chBuf);

	CSockVisionClt *pSockClient;

	POSITION pos = m_ListClientSock.GetHeadPosition();

	while (pos != NULL)
	{
		pSockClient = (CSockVisionClt*)m_ListClientSock.GetNext(pos);
		int nSend = pSockClient->Send(chBuf, nBufLen, nFlags);

		if(nSend == SOCKET_ERROR)
			G_AddLog(3, 2, _T("[Send Vision] ID = %d, Error = %d"), pSockClient->m_nVisionID, GetLastError());
		else
			G_AddLog(3, 2, _T("[Send Vision] ID = %d, %s"), pSockClient->m_nVisionID, strBuf);
	}
}

int CSockVisionSvr::m_fnSendClientAt(CAsyncSocket* pSockClient, CString strBuf, int nBufLen, int nFlags)
{
	int nSend = 0;

	char chBuf[1024] = {0};
	WideCharToMultiByte(CP_ACP, NULL, strBuf, -1, chBuf,  1024, NULL, FALSE);

	nBufLen = strlen(chBuf);

	if(pSockClient != NULL)
	{
		nSend = pSockClient->Send(chBuf, nBufLen, nFlags);

		if(nSend == SOCKET_ERROR)
			G_AddLog(3, 2, _T("[Send Vision] ID = %d, Error = %d"), ((CSockVisionClt*)pSockClient)->m_nVisionID, GetLastError());
		else
			G_AddLog(3, 2, _T("[Send Vision] ID = %d, %s"), ((CSockVisionClt*)pSockClient)->m_nVisionID, strBuf);

	}

	return nSend;
}
