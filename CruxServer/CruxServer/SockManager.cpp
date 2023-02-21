// SockManager.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SockManager.h"
#include "MainFrm.h"
#include "ViewAuto.h"
#include "ViewSetup.h"

// CSockManager

IMPLEMENT_DYNAMIC(CSockManager, CWnd)

CSockManager::CSockManager(CWnd* pParentWnd)
{
	m_pMainWnd = pParentWnd;
	m_pSockVisionSvr = NULL;
	m_pSockCIMSvr = NULL;
	m_pSockCtrlPCSvr = NULL;
	/*m_strSemaJudge = _T("");
	m_strSemaGrade = _T("");*/
	//m_pSockSignalFlagSvr = NULL;

	m_bRcpEditFlag = FALSE;
	//::InitializeCriticalSectionAndSpinCount(&cs, 2000);
	//::InitializeCriticalSectionAndSpinCount(&csSignalFlag, 2000);
	
	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		m_bGrabEndFlag[i] = FALSE;
		m_bInspectEndFlag[i] = FALSE;
		m_nVisionState[i] = STATE_IDLE;
		m_bRecvReadyRes[i] = FALSE;
		m_nGrabReadyRes[i] = 0;
		m_nGrabEndRes[i] = 0;
		m_bRecvRecipeRes[i] = FALSE;	//2015.11.05
	}
}

CSockManager::~CSockManager()
{
//	DeleteCriticalSection(&cs);
//	DeleteCriticalSection(&csSignalFlag);
	if(m_pSockVisionSvr)
	{
		m_pSockVisionSvr->Close();
		delete m_pSockVisionSvr;
		m_pSockVisionSvr = NULL;
	}

	if(m_pSockCIMSvr)
	{
		m_pSockCIMSvr->Close();

		delete m_pSockCIMSvr;
		m_pSockCIMSvr = NULL;
	}

	if(m_pSockCtrlPCSvr)
	{
		m_pSockCtrlPCSvr->Close();

		delete m_pSockCtrlPCSvr;
		m_pSockCtrlPCSvr = NULL;
	}

	//if (m_pSockSignalFlagSvr)
	//{
	//	m_pSockSignalFlagSvr->Close();

	//	delete m_pSockSignalFlagSvr;
	//	m_pSockSignalFlagSvr = NULL;
	//}
}

BEGIN_MESSAGE_MAP(CSockManager, CWnd)
	ON_MESSAGE(WM_SOCK_VISION_ACCEPT, OnSockVisionAccept)
	ON_MESSAGE(WM_SOCK_VISION_RECEIVE, OnSockVisionReceive)
	ON_MESSAGE(WM_SOCK_VISION_DISCONNECT, OnSockVisionClose)
	//ON_MESSAGE(WM_SOCK_SIGNALFLAG_ACCEPT, OnSockSignalFlagAccept)
	//ON_MESSAGE(WM_SOCK_SIGNALFLAG_RECEIVE, OnSockSignalFlagReceive)
	//ON_MESSAGE(WM_SOCK_SIGNALFLAG_DISCONNECT, OnSockSignalFlagClose)

	ON_MESSAGE(WM_SOCK_CIM_ACCEPT, OnSockCIMAccept)
	ON_MESSAGE(WM_SOCK_CIM_RECEIVE, OnSockCIMReceive)
	ON_MESSAGE(WM_SOCK_CIM_CONNECT,	OnSockCIMConnect)
	ON_MESSAGE(WM_SOCK_CIM_DISCONNECT, OnSockCIMClose)
	ON_MESSAGE(WM_SOCK_CONTROL_ACCEPT, OnSockCtrlPCAccept)
	ON_MESSAGE(WM_SOCK_CONTROL_RECEIVE, OnSockCtrlPCReceive)
	ON_MESSAGE(WM_SOCK_CONTROL_CONNECT, OnSockCtrlPCConnect)
	ON_MESSAGE(WM_SOCK_CONTROL_DISCONNECT, OnSockCtrlPCClose)
	//ON_MESSAGE(WM_SOCK_CONTROL_ALARM,m_fnAlarmSet )
	ON_WM_TIMER()
END_MESSAGE_MAP()



// CSockManager 메시지 처리기입니다.

// wParam : ErrorCode, lParam : CSockVisionClt*
LRESULT CSockManager::OnSockVisionAccept(WPARAM wParam, LPARAM lParam)
{
	CSockVisionClt* pSockClient = (CSockVisionClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if( wParam != 0 )
	{
		G_AddLog(3, 2, _T("Accept Fail... Vision Client Socket ErrorCode [%d]"), 
			wParam, m_fnGetLastError_Desp((int)wParam));

		return 0;
	}

	m_fnSetVisionClientID(pSockClient);

	if (pSockClient->m_nVisionID == SIGNAL_PC)
	{
		G_AddLog(3, 2, _T("Accept Signal Flag Client [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
	}
	else
	{
		G_AddLog(3, 2, _T("Accept Vision Client [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);

	}
	
	int nClientID = m_fnGetVisionClientID(pSockClient);

	if(nClientID != VISION_PC_NONE)
	{
		if (nClientID == SIGNAL_PC)
			pFrame->m_pVsSeverIF->m_stSockStatus.bSignalFlag = TRUE;
		else
			pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[nClientID - 1] = TRUE;

		if (nClientID == PRE_ALIGN_PC)	
			pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign = TRUE;
		if (nClientID == 31)	
			pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign = TRUE;	//2021/09/09

		pFrame->m_fnUpdateSockStatus();
	}
	
	return 0;
}

LRESULT CSockManager::OnSockVisionReceive(WPARAM wParam, LPARAM lParam)
{
	CSockVisionClt* pSockClient = (CSockVisionClt*)lParam;

	if(lParam == NULL)	return 0;

	if( wParam != 0 )
	{
		
		if (pSockClient->m_nVisionID == SIGNAL_PC)
		{
			G_AddLog(3, 2, _T("Failed Receive Signal Flag [%s : %d] : [%d] : %s"), 
				pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));
		}
		else
		{
			G_AddLog(3, 2, _T("Failed Receive Vision [%s : %d] : [%d] : %s"), 
				pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));
		}
	}
	else
		m_fnRecvData_Vision(pSockClient);

	return 0;
}

LRESULT CSockManager::OnSockVisionClose(WPARAM wParam, LPARAM lParam)
{
	CSockVisionClt* pSockClient = (CSockVisionClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(lParam == NULL)	return 0;

	{
		int nClientID = m_fnGetVisionClientID(pSockClient);

		if(nClientID != VISION_PC_NONE)
		{
			if (nClientID == SIGNAL_PC)
				pFrame->m_pVsSeverIF->m_stSockStatus.bSignalFlag = FALSE;
			else
				pFrame->m_pVsSeverIF->m_stSockStatus.bVisionClient[nClientID - 1] = FALSE;

			if (nClientID == PRE_ALIGN_PC)
				pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign = FALSE;
			if (nClientID == 31)	
				pFrame->m_pVsSeverIF->m_stSockStatus.bPreAlign = FALSE;	//2021/09/09
			
			pFrame->m_fnUpdateSockStatus();
		}
		if (pSockClient->m_nVisionID == SIGNAL_PC)
		{
			G_AddLog(3, 2, _T("Accept Signal Flag Client  Socket [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
		}
		else
		{
			G_AddLog(3, 2, _T("Accept Vision Client  Socket [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
		}

		m_pSockVisionSvr->m_fnCloseClient(pSockClient);
	}

	return 0;
}

// CSockManager 메시지 처리기입니다.

// wParam : ErrorCode, lParam : CSockVisionClt*
//LRESULT CSockManager::OnSockSignalFlagAccept(WPARAM wParam, LPARAM lParam)
//{
//	CSockSignalFlagClt* pSockClient = (CSockSignalFlagClt*)lParam;
//
//	if( wParam != 0 )
//	{
//		G_AddLog(3, 2, _T("Accept Fail... SignalFlag Client Socket ErrorCode [%d]"), 
//			wParam, m_fnGetLastError_Desp((int)wParam));
//
//		return 0;
//	}
//
//	m_fnSetSignalFlagClientID(pSockClient);
//
//	G_AddLog(3, 2, _T("Accept SignalFlag Client [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
//
//	int nClientID = m_fnGetSignalFlagClientID(pSockClient);
//
//	if(nClientID == SIGNAL_PC)
//	{
//		m_stSockStatus.bSignalFlag = TRUE;
//
//		CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
//		pFrame->m_fnUpdateSockStatus();
//	}
//
//	return 0;
//}
//
//LRESULT CSockManager::OnSockSignalFlagReceive(WPARAM wParam, LPARAM lParam)
//{
//	CSockSignalFlagClt* pSockClient = (CSockSignalFlagClt*)lParam;
//
//	if(lParam == NULL)	return 0;
//
//	if( wParam != 0 )
//	{
//		G_AddLog(3, 2, _T("Failed Receive SignalFlag [%s : %d] : [%d] : %s"), 
//			pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));
//	}
//	else
//		m_fnRecvData_SignalFlag(pSockClient);
//
//	return 0;
//}
//
//LRESULT CSockManager::OnSockSignalFlagClose(WPARAM wParam, LPARAM lParam)
//{
//	CSockVisionClt* pSockClient = (CSockVisionClt*)lParam;
//
//	if(lParam == NULL)	return 0;
//	{
//		int nClientID = m_fnGetVisionClientID(pSockClient);
//
//		if(nClientID != VISION_PC_NONE)
//		{
//			m_stSockStatus.bVisionClient[nClientID - 1] = FALSE;
//
//			CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
//			pFrame->m_fnUpdateSockStatus();
//		}
//
//		G_AddLog(3, 2, _T("Disconnect SignalFlag Client Socket [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
//
//		m_pSockVisionSvr->m_fnCloseClient(pSockClient);
//	}
//
//	return 0;
//}




LRESULT CSockManager::OnSockCIMAccept(WPARAM wParam, LPARAM lParam)
{
	CSockCIMClt* pSockClient = (CSockCIMClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if( wParam != 0 )
	{
		G_AddLog(3, 2, _T("Accept Fail... CIM Client Socket ErrorCode [%d]"), wParam, m_fnGetLastError_Desp((int)wParam));

		return 0;
	}
	
	G_AddLog(3, 2, _T("Accept CIM Client [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);

	pFrame->m_pVsSeverIF->m_stSockStatus.bCIM = TRUE;

	
	pFrame->m_fnUpdateSockStatus();
	
	return 0;
}

LRESULT CSockManager::OnSockCIMReceive(WPARAM wParam, LPARAM lParam)
{
	CSockCIMClt* pSockClient = (CSockCIMClt*)lParam;

	if(lParam == NULL)	return 0;

	if( wParam != 0 )
		G_AddLog(3, 2, _T("Failed Receive CIM [%d] : %s"), wParam, m_fnGetLastError_Desp((int)wParam));
	else
		m_fnRecvData_CIM(pSockClient);

	return 0;
}

LRESULT CSockManager::OnSockCIMConnect(WPARAM wParam, LPARAM lParam)
{
	CSockCIMClt* pSockClient = (CSockCIMClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(lParam == NULL)	return 0;

	if((int)wParam)
	{
		G_AddLog(3, 2, _T("Failed ConnectToCIM [%s : %d] : [%d] : %s"), 
			pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));

		SetTimer(TIMER_RETRY_CIM_PC, TIMER_RETRY_INTERVAL, NULL);		// 재연결 시도...
	}
	else
	{
		pFrame->m_pVsSeverIF->m_stSockStatus.bCIM = TRUE;

		
		pFrame->m_fnUpdateSockStatus();

		G_AddLog(3, 2, _T("Success ConnectToCIM [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
	}

	return 0;
}

LRESULT CSockManager::OnSockCIMClose(WPARAM wParam, LPARAM lParam)
{
	CSockCIMClt* pSockClient = (CSockCIMClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(lParam == NULL)	return 0;

	pFrame->m_pVsSeverIF->m_stSockStatus.bCIM = FALSE;

	
	pFrame->m_fnUpdateSockStatus();

	G_AddLog(3, 2, _T("Disonnect CIM Client Socket [%s : %d] : [%d] : %s"), 
		pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));

	m_pSockCIMSvr->m_fnCloseClient();

	// Active 모드 일때는 재연결 시도
	if(pFrame->m_stParamInfo.bCIMSockMode == SOCK_MODE_ACTIVE)
		SetTimer(TIMER_RETRY_CIM_PC, TIMER_RETRY_INTERVAL, NULL);

	return 0;
}

LRESULT CSockManager::OnSockCtrlPCAccept(WPARAM wParam, LPARAM lParam)
{
	CSockCtrlPCClt* pSockClient = (CSockCtrlPCClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if( wParam != 0 )
	{
		G_AddLog(3, 2, _T("Accept Fail... Control PC Client Socket ErrorCode [%d]"), 
			wParam, m_fnGetLastError_Desp((int)wParam));

		return 0;
	}

	G_AddLog(3, 2, _T("Accept Vision Client [%s : %d]"), pSockClient->m_strIPAddress, pSockClient->m_nPortNo);

	pFrame->m_pVsSeverIF->m_stSockStatus.bControlPC = TRUE;

	
	pFrame->m_fnUpdateSockStatus();
	
	return 0;
}

LRESULT CSockManager::OnSockCtrlPCReceive(WPARAM wParam, LPARAM lParam)
{
	CSockCtrlPCClt* pSockClient = (CSockCtrlPCClt*)lParam;

	if(lParam == NULL)	return 0;

	if( wParam != 0 )
		G_AddLog(3, 2, _T("Failed Receive Control PC [%d] : %s"), wParam, m_fnGetLastError_Desp((int)wParam));
	else
		m_fnRecvData_CtrlPC(pSockClient);

	return 0;
}

LRESULT CSockManager::OnSockCtrlPCConnect(WPARAM wParam, LPARAM lParam)
{
	CSockCtrlPCClt* pSockClient = (CSockCtrlPCClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(lParam == NULL)	return 0;

	if((int)wParam)
	{
		G_AddLog(3, 2, _T("Failed ConnectToControlPC [%s : %d] : [%d] : %s"), 
			pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));

		SetTimer(TIMER_RETRY_CONTROL_PC, TIMER_RETRY_INTERVAL, NULL);		// 재연결 시도...
	}
	else
	{
		pFrame->m_pVsSeverIF->m_stSockStatus.bControlPC = TRUE;

		
		pFrame->m_fnUpdateSockStatus();

		G_AddLog(3, 2, _T("Success ConnectToControlPC [%s : %d] : [%d] : %s"), 
			pSockClient->m_strIPAddress, pSockClient->m_nPortNo);
	}

	return 0;
}

LRESULT CSockManager::OnSockCtrlPCClose(WPARAM wParam, LPARAM lParam)
{
	CSockCtrlPCClt* pSockClient = (CSockCtrlPCClt*)lParam;
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	if(lParam == NULL)	return 0;

	pFrame->m_pVsSeverIF->m_stSockStatus.bControlPC = FALSE;

	
	pFrame->m_fnUpdateSockStatus();

	G_AddLog(3, 2, _T("Disconnect Control PC Client Socket [%s : %d] : [%d] : %s"),
		pSockClient->m_strIPAddress, pSockClient->m_nPortNo, wParam, m_fnGetLastError_Desp((int)wParam));

	m_pSockCtrlPCSvr->m_fnCloseClient();

	// Active 모드 일때는 재연결 시도
	if(pFrame->m_stParamInfo.bControlSockMode == SOCK_MODE_ACTIVE)
		SetTimer(TIMER_RETRY_CONTROL_PC, TIMER_RETRY_INTERVAL, NULL);

	return 0;
}
//void CSockManager::m_fnSetSignalFlagClientID(CSockSignalFlagClt* pSockClient)
//{
//	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
//	if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_SignalFlag)
//		pSockClient->m_nSignalFlagID = SIGNAL_PC; //2015.04.27 add by IKM
//}

void CSockManager::m_fnSetVisionClientID(CSockVisionClt* pSockClient)
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

#if 1		// "Config.ini" 에 정의되어있는 IP 주소로 Vision Client ID 를 지정한다.
	if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_1)
		pSockClient->m_nVisionID = VISION_PC_1;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_2)
		pSockClient->m_nVisionID = VISION_PC_2;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_3)
		pSockClient->m_nVisionID = VISION_PC_3;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_4)	//2015.05.12 add s-
		pSockClient->m_nVisionID = VISION_PC_4;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_5)
		pSockClient->m_nVisionID = VISION_PC_5;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_6)
		pSockClient->m_nVisionID = VISION_PC_6;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_7)
		pSockClient->m_nVisionID = VISION_PC_7;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_8)
		pSockClient->m_nVisionID = VISION_PC_8;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_Vision_9)//2015.05.12 add e-
		pSockClient->m_nVisionID = VISION_PC_9;
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_SignalFlag)
		pSockClient->m_nVisionID = SIGNAL_PC; //2015.04.27 add by IKM
	else if(pSockClient->m_strIPAddress == pFrame->m_stCommInfo.strIP_PreAlignPC)
		pSockClient->m_nVisionID = PRE_ALIGN_PC; //2015.08.19
	else
		pSockClient->m_nVisionID = VISION_PC_NONE;
#else		// 접속되는 순서데로 Vision Client ID 를 지정한다.
	for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
	{
		if(m_stSockStatus.bVisionClient[i] == FALSE)
		{
			pSockClient->m_nVisionID = i + 1;
			break;
		}
	}
#endif
}
//int CSockManager::m_fnGetSignalFlagClientID(CSockSignalFlagClt* pSockClient)
//{
//	return pSockClient->m_nSignalFlagID;
//}

int CSockManager::m_fnGetVisionClientID(CSockVisionClt* pSockClient)
{
	return pSockClient->m_nVisionID;
}

BOOL CSockManager::m_fnGetVisionClientInfo(int nClientID, CString &strIPAddr, CString &strPort)
{
	if(m_pSockVisionSvr == NULL)
		return FALSE;

	CSockVisionClt* pSockClient = NULL;

	POSITION pos = m_pSockVisionSvr->m_ListClientSock.GetHeadPosition();

	while(pos)
	{
		pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_ListClientSock.GetNext(pos);

		if(pSockClient)
		{
			if(pSockClient->m_nVisionID == nClientID)
			{
				strIPAddr = pSockClient->m_strIPAddress;
				strPort.Format(_T("%d"), pSockClient->m_nPortNo);

				return TRUE;
			}
		}
	}

	return FALSE;
}

void CSockManager::m_fnInitSock()
{
	CString strLog;

	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	// Vision Socket
	m_pSockVisionSvr = new CSockVisionSvr;
	//m_pSockSignalFlagSvr = new CSockSignalFlagSvr;
	// Signal Flag Socket
	/*if(m_pSockSignalFlagSvr->m_fnCreateSock(this, pFrame->m_stCommInfo.nPort_SignalFlag) == FALSE)
	G_AddLog(3, 2, _T("Signal Flag Server Socket Create Error! Code = %d"), m_pSockSignalFlagSvr->GetLastError());
	else
	{
	m_pSockSignalFlagSvr->m_bSignalFlagRun = TRUE;
	m_stSockStatus.bSignalFlag = TRUE;
	}*/

	if(m_pSockVisionSvr->m_fnCreateSock(this, pFrame->m_stCommInfo.nPort_VisionServer) == FALSE)
		G_AddLog(3, 2, _T("Vision Server Socket Create Error! Code = %d"), m_pSockVisionSvr->GetLastError());
	else
	{
		m_pSockVisionSvr->m_bServerRun = TRUE;
		m_stSockStatus.bVisionServer = TRUE;
	}

	// CIM Socket
	if(pFrame->m_stParamInfo.bUseCIM)
	{
		m_pSockCIMSvr = new CSockCIMSvr(this);

		if(pFrame->m_stParamInfo.bCIMSockMode == SOCK_MODE_PASSIVE)		// Socket Passive Mode
		{
			if(m_pSockCIMSvr->m_fnCreateSock(this, pFrame->m_stCommInfo.nPort_CIM) == FALSE)
				G_AddLog(3, 2, _T("CIM Server Socket Create Error! Code = %d"), m_pSockCIMSvr->GetLastError());
			else
				m_pSockCIMSvr->m_bServerRun = TRUE;
		}
		else															// Socket Active Mode
		{
			m_fnConnectToCIM();
		}
	}

	// Control Socket
	if(pFrame->m_stParamInfo.nControlMode == CONTROL_MODE_PLC)			// Controller : PLC
	{
		// MxComponent 초기화
		pFrame->m_fnInitPLC();
	}
	else																// Controller : PC
	{
		m_pSockCtrlPCSvr = new CSockCtrlPCSvr(this);

		if(pFrame->m_stParamInfo.bControlSockMode == SOCK_MODE_PASSIVE)	// Socket Passive Mode
		{
			if(m_pSockCtrlPCSvr->m_fnCreateSock(this, pFrame->m_stCommInfo.nPort_ControlPC) == FALSE)
				G_AddLog(3, 2, _T("Control PC Server Socket Create Error! Code = %d"), m_pSockCtrlPCSvr->GetLastError());
			else
				m_pSockCtrlPCSvr->m_bServerRun = TRUE;
		}
		else															// Socket Active Mode
		{
			m_fnConnectToControlPC();
		}
	}


	pFrame->m_fnUpdateSockStatus();		// 현재 소켓 연결 상태 업데이트
}

void CSockManager::m_fnConnectToControlPC()
{
	if(m_pSockCtrlPCSvr == NULL)	return;

	if(m_pSockCtrlPCSvr->m_pSockClient)
	{
		delete m_pSockCtrlPCSvr->m_pSockClient;
		m_pSockCtrlPCSvr->m_pSockClient = NULL;
	}

	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	CString strIPAddr = pFrame->m_stCommInfo.strIP_ControlPC;
	int nPort = pFrame->m_stCommInfo.nPort_ControlPC;

	m_pSockCtrlPCSvr->m_pSockClient = new CSockCtrlPCClt(m_pSockCtrlPCSvr);

	if(m_pSockCtrlPCSvr->m_pSockClient->Create() == FALSE)
	{
		G_AddLog(3, 2, _T("Control PC Client Socket Create Error = %d"), GetLastError());
		return;
	}

	if(m_pSockCtrlPCSvr->m_pSockClient->m_fnConnectToServer(strIPAddr, nPort) == FALSE)
	{
		G_AddLog(3, 2, _T("Failed ConnectToControlPC [%s : %d] : %d"), 
			m_pSockCtrlPCSvr->m_pSockClient->m_strIPAddress, 
			m_pSockCtrlPCSvr->m_pSockClient->m_nPortNo,
			m_pSockCtrlPCSvr->m_pSockClient->GetLastError());

		// 작업중... 재연결 시도...
		SetTimer(TIMER_RETRY_CONTROL_PC, TIMER_RETRY_INTERVAL, NULL);
	}
}

void CSockManager::m_fnConnectToCIM()
{
	if(m_pSockCIMSvr == NULL)	return;

	if(m_pSockCIMSvr->m_pSockClient)
	{
		delete m_pSockCIMSvr->m_pSockClient;
		m_pSockCIMSvr->m_pSockClient = NULL;
	}

	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	CString strIPAddr = pFrame->m_stCommInfo.strIP_CIM;
	int nPort = pFrame->m_stCommInfo.nPort_CIM;

	m_pSockCIMSvr->m_pSockClient = new CSockCIMClt(m_pSockCIMSvr);

	if(m_pSockCIMSvr->m_pSockClient->Create() == FALSE)
	{
		G_AddLog(3, 2, _T("CIM Client Socket Create Error = %d"), GetLastError());
		return;
	}

	if(m_pSockCIMSvr->m_pSockClient->m_fnConnectToServer(strIPAddr, nPort) == FALSE)
	{
		G_AddLog(3, 2, _T("Failed ConnectToCIM [%s : %d] : %d"), 
			m_pSockCIMSvr->m_pSockClient->m_strIPAddress, 
			m_pSockCIMSvr->m_pSockClient->m_nPortNo,
			m_pSockCIMSvr->m_pSockClient->GetLastError());
	}
}

void CSockManager::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_RETRY_CONTROL_PC:
		KillTimer(TIMER_RETRY_CONTROL_PC);
		m_fnConnectToControlPC();
		break;
	case TIMER_RETRY_CIM_PC:
		KillTimer(TIMER_RETRY_CIM_PC);
		m_fnConnectToCIM();
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

void CSockManager::m_fnSockCloseAll()
{
	if(m_pSockVisionSvr)
	{
		m_pSockVisionSvr->Close();
		m_pSockVisionSvr->m_fnCloseClientAll();

		delete m_pSockVisionSvr;
		m_pSockVisionSvr = NULL;
	}

	if(m_pSockCIMSvr)
	{
		m_pSockCIMSvr->Close();
		m_pSockCIMSvr->m_fnCloseClient();

		delete m_pSockCIMSvr;
		m_pSockCIMSvr = NULL;
	}

	if(m_pSockCtrlPCSvr)
	{
		m_pSockCtrlPCSvr->Close();
		m_pSockCtrlPCSvr->m_fnCloseClient();

		delete m_pSockCtrlPCSvr;
		m_pSockCtrlPCSvr = NULL;
	}

	memset((void*)&m_stSockStatus, NULL, sizeof(STRU_SOCK_STATUS));
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	pFrame->m_fnUpdateSockStatus();
}

void CSockManager::m_fnAddLog(CString strLog)
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;

	pFrame->m_fnAddLog(strLog);
}

CString CSockManager::m_fnGetLastError_Desp(int nErrorCode)
{
	CString strErrorDesp;

	switch(nErrorCode)
	{
	case WSAEADDRINUSE:
		strErrorDesp = _T("지정된 주소가 이미 사용중입니다");
		break;
	case WSAEADDRNOTAVAIL:
		strErrorDesp = _T("지정된 주소는 사용이 불가합니다");
		break;
	case WSAEAFNOSUPPORT:
		strErrorDesp = _T("지정된 패밀리 주소는 이 소켓과 사용할 수 없습니다");
		break;
	case WSAECONNREFUSED:
		strErrorDesp = _T("연결시도가 거부되었습니다 - 서버가 다운되었을 가능성이 있습니다");
		break;
	case WSAEDESTADDRREQ:
		strErrorDesp = _T("목적지 주소가 필요합니다");
		break;
	case WSAEFAULT:
		strErrorDesp = _T("lpSocketAddrLen 인자가 잘못되었습니다");
		break;
	case WSAEINVAL:
		strErrorDesp = _T("이 소켓은 이미 다른 주소에 바인딩되어 있습니다");
		break;
	case WSAEISCONN:
		strErrorDesp = _T("이 소켓은 이미 연결되어 있습니다");
		break;
	case WSAEMFILE:
		strErrorDesp = _T("사용가능한 파일 기술자가 없습니다");
		break;
	case WSAEHOSTUNREACH:
	case WSAENETUNREACH:
		strErrorDesp = _T("목적지에 도달할 수 없습니다");
		break;
	case WSAENOBUFS:
		strErrorDesp = _T("사용가능한 버퍼공간이 없습니다");
		break;
	case WSAENOTCONN:
		strErrorDesp = _T("이 소켓은 연결된 소켓이 아닙니다");
		break;
	case WSAENOTSOCK:
		strErrorDesp = _T("이 기술자는 소켓이 아니라 파일입니다");
		break;
	case WSAETIMEDOUT:
		strErrorDesp =_T("연결시간이 초과되었습니다");
		break;
	}

	return strErrorDesp;
}

void CSockManager::m_fnRecvData_Vision(CSockVisionClt* pSockClient)
{
	bool bIsEnterCS = false;	//Lock 후 예외 처리 될 경우를 대비하여 Unlock 을 하기 위함
	try
	{
		if(pSockClient == NULL)
		{
			m_fnAddLog(_T("Receive Socket Error!"));
			return;
		}

		unsigned char chData[SOCK_BUFF_LENGTH];
		int nRecvLen;

		const char chSTX[2] = {0x02, 0};
		const char chETX[2] = {0x03, 0};

		memset(chData, NULL, SOCK_BUFF_LENGTH);

		bIsEnterCS = true;
		nRecvLen = pSockClient->Receive(chData, SOCK_BUFF_LENGTH - 1);
		
		if(nRecvLen <= 0)
		{
			G_AddLog(3, 2, _T("Receive Data Error! Vision = %s, Err = %d"), 
				pSockClient->m_strIPAddress, pSockClient->GetLastError());
			return;
		}

		// 내부 버퍼에 현재 수신된 버퍼를 더한다.
		memcpy(pSockClient->m_chBuffer + pSockClient->m_nBufferSize, chData, nRecvLen);
		pSockClient->m_nBufferSize += nRecvLen;

		WCHAR chWChar[SOCK_BUFF_LENGTH];
		memset(chWChar, NULL, SOCK_BUFF_LENGTH * 2);

		MultiByteToWideChar(CP_ACP, NULL, (LPCSTR)chData,  -1, chWChar, 1024);

		CString strPacket = chWChar;

		//	if(strPacket.Find(VTM_ALIVE) == -1)
		//		G_AddLog(2, 3, _T("[Recv] Vision Socket => %s"), strPacket);

		int nFindSTX, nFindETX;

		while(pSockClient->m_nBufferSize > 0)
		{
			nFindETX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chETX);

			if(nFindETX != -1)				// "ETX" 가 있으면
			{	
				nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

				if(nFindSTX == -1)			// 잘못된 메세지... 버린다.
				{
					memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
					pSockClient->m_nBufferSize = 0;

					return;
				}
			}
			else							// "ETX" 가 없으면
			{
				nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

				if(nFindSTX != -1)			// 미완성된 메세지... 다음 수신을 기다린다.
					return;
				else						// 잘못된 메세지... 버린다.
				{
					memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
					pSockClient->m_nBufferSize = 0;

					return;
				}
			}

			// "m_chSTX...ETX" 하나의 Packet 이 완성되면 메세지를 처리한다.
			m_fnParsingMsg_Vision(pSockClient->m_chBuffer, nFindETX + 1, pSockClient->m_nVisionID);

			if(pSockClient->m_nBufferSize > nFindETX + 1)	// 남는 패킷이 있을경우
			{
				char chTemp[SOCK_BUFF_LENGTH] = {0};
				memcpy(chTemp, pSockClient->m_chBuffer + (nFindETX + 1), pSockClient->m_nBufferSize - (nFindETX + 1));
				memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
				memcpy(pSockClient->m_chBuffer, chTemp, pSockClient->m_nBufferSize - (nFindETX + 1));
				pSockClient->m_nBufferSize = pSockClient->m_nBufferSize - (nFindETX + 1);
			}
			else
			{
				memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
				pSockClient->m_nBufferSize = 0;
			}
		}
	}
	catch(COleDispatchException *Exception){
		//if (bIsEnterCS)	LeaveCriticalSection(&cs);
		AfxMessageBox(_T("Socket Recive Error"), MB_ICONINFORMATION);
		Exception->Delete();
	}
}

void CSockManager::m_fnRecvData_CtrlPC(CSockCtrlPCClt* pSockClient)
{
	if(pSockClient == NULL)
	{
		m_fnAddLog(_T("Receive Socket Error!"));
		return;
	}

	char chData[SOCK_BUFF_LENGTH];
	int nRecvLen;

	const char chSTX[2] = {0x02, 0};
	const char chETX[2] = {0x03, 0};

	memset(chData, NULL, SOCK_BUFF_LENGTH);

	nRecvLen = pSockClient->Receive(chData, SOCK_BUFF_LENGTH - 1);

	if(nRecvLen <= 0)
	{
		G_AddLog(3, 2, _T("Receive Data Error! CtrlPC = %s, Err = %d"), 
			pSockClient->m_strIPAddress, pSockClient->GetLastError());
		return;
	}

	// 내부 버퍼에 현재 수신된 버퍼를 더한다.
	memcpy(pSockClient->m_chBuffer + pSockClient->m_nBufferSize, chData, nRecvLen);
	pSockClient->m_nBufferSize += nRecvLen;

	G_AddLog(3, 2, _T("[Recv] Control Socket => %s"), pSockClient->m_chBuffer);

	int nFindSTX, nFindETX;

	nFindETX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chETX);

	if(nFindETX != -1)				// "ETX" 가 있으면
	{	
		nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

		if(nFindSTX == -1)			// 잘못된 메세지... 버린다.
		{
			memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
			pSockClient->m_nBufferSize = 0;

			return;
		}
	}
	else							// "ETX" 가 없으면
	{
		nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

		if(nFindSTX != -1)			// 미완성된 메세지... 다음 수신을 기다린다.
			return;
		else						// 잘못된 메세지... 버린다.
		{
			memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
			pSockClient->m_nBufferSize = 0;

			return;
		}
	}

	// "STX...ETX" 하나의 Packet 이 완성되면 메세지를 처리한다.
	m_fnParsingMsg_CtrlPC(pSockClient->m_chBuffer, nFindETX + 1);

	if(pSockClient->m_nBufferSize > nFindETX + 1)	// 남는 패킷이 있을경우
	{
		char chTemp[SOCK_BUFF_LENGTH] = {0};
		memcpy(chTemp, pSockClient->m_chBuffer + (nFindETX + 1), pSockClient->m_nBufferSize - (nFindETX + 1));
		memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
		memcpy(pSockClient->m_chBuffer, chTemp, pSockClient->m_nBufferSize - (nFindETX + 1));
	}
	else
	{
		memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
		pSockClient->m_nBufferSize = 0;
	}
}

void CSockManager::m_fnRecvData_CIM(CSockCIMClt* pSockClient)
{
	if(pSockClient == NULL)
	{
		m_fnAddLog(_T("Receive Socket Error!"));
		return;
	}

	unsigned char chData[SOCK_BUFF_LENGTH];
	int nRecvLen;

	const char chSTX[2] = {0x02, 0};
	const char chETX[2] = {0x03, 0};

	memset(chData, NULL, SOCK_BUFF_LENGTH);

	nRecvLen = pSockClient->Receive(chData, SOCK_BUFF_LENGTH - 1);

	if(nRecvLen <= 0)
	{
		G_AddLog(3, 2, _T("Receive Data Error! CIM = %s, Err = %d"), 
			pSockClient->m_strIPAddress, pSockClient->GetLastError());
		return;
	}

	// 내부 버퍼에 현재 수신된 버퍼를 더한다.
	memcpy(pSockClient->m_chBuffer + pSockClient->m_nBufferSize, chData, nRecvLen);
	pSockClient->m_nBufferSize += nRecvLen;

	G_AddLog(3, 2, _T("[Recv] Control Socket => %s"), pSockClient->m_chBuffer);

	int nFindSTX, nFindETX;

	nFindETX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chETX);

	if(nFindETX != -1)				// "ETX" 가 있으면
	{	
		nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

		if(nFindSTX == -1)			// 잘못된 메세지... 버린다.
		{
			memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
			pSockClient->m_nBufferSize = 0;

			return;
		}
	}
	else							// "ETX" 가 없으면
	{
		nFindSTX = m_fnFindStr(pSockClient->m_chBuffer, pSockClient->m_nBufferSize, chSTX);

		if(nFindSTX != -1)			// 미완성된 메세지... 다음 수신을 기다린다.
			return;
		else						// 잘못된 메세지... 버린다.
		{
			memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
			pSockClient->m_nBufferSize = 0;

			return;
		}
	}

	// "STX...ETX" 하나의 Packet 이 완성되면 메세지를 처리한다.
	m_fnParsingMsg_CIM(pSockClient->m_chBuffer, nFindETX + 1);

	if(pSockClient->m_nBufferSize > nFindETX + 1)	// 남는 패킷이 있을경우
	{
		char chTemp[SOCK_BUFF_LENGTH] = {0};
		memcpy(chTemp, pSockClient->m_chBuffer + (nFindETX + 1), pSockClient->m_nBufferSize - (nFindETX + 1));
		memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
		memcpy(pSockClient->m_chBuffer, chTemp, pSockClient->m_nBufferSize - (nFindETX + 1));
	}
	else
	{
		memset(pSockClient->m_chBuffer, NULL, SOCK_BUFF_LENGTH);
		pSockClient->m_nBufferSize = 0;
	}
}

void CSockManager::m_fnParsingMsg_CtrlPC(char* chBuf, int nRecvLen)
{
	CString strRecvMsg, strPacket, strTemp, strPanelID, strModel;
	char chRecvBuf[SOCK_BUFF_LENGTH] = {0};

	memcpy(chRecvBuf, chBuf, nRecvLen);

	strRecvMsg = strPacket = chRecvBuf;
	strPacket = strPacket.Mid(1, strPacket.GetLength() - 2);

#ifdef DEBUG_TEST
	G_AddLog(3, 2, _T("[Packet CtrlPC] %s"), strPacket);

	if(strPacket.Find(_T("PANELID")) != -1)
	{
		strPacket.Format(_T("%c.%s.%s.%s.%s.%c"), STX, MTV_READY_REQ_F, _T("ModelID"), _T("PanelID001"), _T("PanelID002"), ETX);
		m_pSockVisionSvr->m_fnSendClientAll(strPacket, strPacket.GetLength());
	}
	else if(strPacket.Find(_T("MOT.GRAB.START.001")) != -1)
	{
		strPacket.Format(_T("%c.%s.%c"), STX, MTV_GRAB_START, ETX);
		m_pSockVisionSvr->m_fnSendClientAll(strPacket, strPacket.GetLength());
	}
	
	return;
#else
#endif

	if(strPacket.Find(MTV_READY_REQ) != -1)
	{
		// Param -> Panel ID, Model Name
		strPacket.Replace(MTV_READY_REQ, _T(""));
		strPacket.Replace(_T("."), _T(" "));
		strPacket.Trim();
		
		int nFind = strPacket.Find(_T(" "));

		if(nFind == -1)
		{
			G_AddLog(3, 2, _T("[Packet CtrlPC] Data Format Error => %s"), strPacket);
			return;
		}

		strPanelID = strPacket.Mid(0, nFind);
		strModel = strPacket.Mid(nFind + 1, strPacket.GetLength() - (nFind + 1));

		G_AddLog(3, 2, _T("[Packet CtrlPC] %s : %s, %s"), MTV_READY_REQ, strPanelID, strModel);

		// Param 데이터 처리... 작업중...

		// Vision Client 에게 해당 메세지를 전달한다.
		m_pSockVisionSvr->m_fnSendClientAll(strRecvMsg, nRecvLen);
	}
	else if(strPacket.Find(MTV_GRAB_START) != -1)
	{
		G_AddLog(3, 2, _T("[Packet CtrlPC] %s"), MTV_GRAB_START);

		// Vision Client 에게 해당 메세지를 전달한다.
		m_pSockVisionSvr->m_fnSendClientAll(strRecvMsg, nRecvLen);
	}
	else if(strPacket.Find(MTV_GRAB_END) != -1)
	{
		G_AddLog(3, 2, _T("[Packet CtrlPC] %s"), MTV_GRAB_END);

		// Vision Client 에게 해당 메세지를 전달한다.
		m_pSockVisionSvr->m_fnSendClientAll(strRecvMsg, nRecvLen);
	}
	else
	{
		G_AddLog(3, 2, _T("[Packet CtrlPC] Undefined => %s"), strPacket);
		return;
	}
}

void CSockManager::m_fnParsingMsg_Vision(char* chBuf, int nRecvLen, int nVisionID)
{
	CMainFrame* pFrame = (CMainFrame*)m_pMainWnd;
	
	CString strRecvMsg, strPacket, strPanelID_1, strPanelID_2, strJudge_1, strJudge_2, strCode_1, strCode_2, strState, strTemp, strVal, strInterlockFlag;
	CString strInspType;
	CString strConFlag, strContResult1, strContResult2, strPgResult1, strPgResult2, strPgVal1, strPgVal2;	//2015.11.17
	CString srtCameraNo, strLumValue;
	CString strAlign_Result, strMeasureData;	//2015.08.19
	CString strX1, strY1, strX2, strY2, strAngle1, strAngle2, strDistance;	//2015.08.19
	//DOUBLE dX1, dY1, dX2, dY2, dAngle1, dAngle2, dDistance;	//2015.08.19
	char chRecvBuf[SOCK_BUFF_LENGTH] = {0};
	STRU_NG_FLAG_INFO stNgFlagInfo;
	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;	//2015.08.19
	STRU_ALIGN_INFO stAlignInfo;	//2015.08.19
	CString strMeasureFail;	//2015.11.02
	BOOL bNgFlag = FALSE;
	CString strRecipe, strRecipeFlag;	//2015.11.05
	int nRet;
	memcpy(chRecvBuf, chBuf, nRecvLen);

	strRecvMsg = strPacket = chRecvBuf;

	strTemp = strPacket.Mid(2, nRecvLen - 4);
	strPacket = strTemp;

	//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision] %s : %s"), _T("Socket Data"), strPacket);

	if(strPacket.Find(VTM_ALIVE) != -1)
	{
		// 작업중... Alive Signal 처리. 상단 메뉴바 상태 처리...
		//G_AddLog(3, 2, _T("[Packet Vision] %s"), VTM_ALIVE);
	}
	else if(strPacket.Find(VTM_STATE) != -1)
	{
		// Param -> State : 1 (Ready), 2 (Grab), 3 (Proc), 4 (Error)
		strPacket.Replace(VTM_STATE, _T(""));
		strPacket.Replace(_T("."), _T(" "));
		strPacket.Trim();
		
		strState = strPacket;

		G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision] %s : %s"), VTM_STATE, strState);

		// 작업중...Param 데이터 처리... 
		/*if (m_nVisionState[nVisionID - 1] == STATE_INSP_END)*/
			pFrame->m_pVsSeverIF->m_nVisionState[nVisionID - 1] = _ttoi(strState);
	}
	else if(strPacket.Find(VTM_READY_RES) != -1)
	{
		//// Param -> Response : 0 (정상), 1 (Model Name 오류), 2 (Trigger Set 오류)
		//strPacket.Replace(VTM_READY_RES, _T(""));
		//strPacket.Replace(_T("."), _T(""));

		//G_AddLog_STG_SEQ(3, nVisionID - 1, _T("[Packet Vision][%d] %s : %s"), nVisionID, VTM_READY_RES, strPacket);

		//// 작업중...Param 데이터 처리... 
		//m_bRecvReadyRes[nVisionID - 1] = TRUE;
		//m_fnSetGrabReadyRes(nVisionID, _ttoi(strPacket));

		//if (_ttoi(strPacket) == 0)	//2015.07.05
		//{
		//	m_nVisionState[nVisionID - 1] = STATE_GRAB;
		//}
	}
	else if(strPacket.Find(VTM_GRAB_END) != -1)
	{
		//// Param -> Response : 0 (정상), 1 (오류)
		//strPacket.Replace(VTM_GRAB_END, _T(""));
		//strPacket.Replace(_T("."), _T(""));
		//G_AddLog_STG_SEQ(3, nVisionID - 1,  _T("[Packet Vision][%d] %s : %s"), nVisionID, VTM_GRAB_END, strPacket);
		////G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s"), nVisionID, VTM_GRAB_END, strPacket);

		//// 작업중...Param 데이터 처리... 
		//m_fnSetGrabEndFlag(nVisionID, TRUE);
		//m_fnSetGrabEndRes(nVisionID, _ttoi(strPacket));

		////if (_ttoi(strPacket) == 0)	//2015.07.05
		////{
		////	m_nVisionState[nVisionID - 1] = STATE_INSP_START;
		////}
	}
	else if (strPacket.Find(VTM_INSPECT_END) != -1) // 중간 데이터가 Client로 부터 생성 되면 서버에 보고한다.
	{
		G_AddLog_STG_SEQ(3, nVisionID - 1,  _T("[Packet Vision][%d] : %s : %s"), nVisionID, VTM_INSPECT_END, strPacket);
		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] : %s : %s"), nVisionID, VTM_INSPECT_END, strPacket);
		// Param 1 -> Panel ID 1
		// Param 2 -> Panel ID 2
		// Param 3 -> Inspect Type		
		strPacket.Replace(VTM_INSPECT_END, _T(""));
		strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);		// 앞/뒤의 "." 를 제거한다.

		int i = 0;
		while(AfxExtractSubString(strTemp, strPacket, i, '.'))
		{
			if(i == 0)		strPanelID_1 = strTemp;
			else if(i == 1)	strPanelID_2 = strTemp;		
			else if(i == 2)	strInspType  = strTemp;		
			i++;
		}

		m_fnSendInspectEnd(nVisionID, strPanelID_1, strPanelID_2);

		TRACE(_T("Inspect End"));
	}
	else if(strPacket.Find(VTM_CLASSIFY_END) != -1)
	{
		//
//		// Param 1 -> Panel ID
//		// Param 2 -> Judge
//		// Param 3 -> Defect Code
//		strPacket.Replace(VTM_CLASSIFY_END, _T(""));
//		strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);		// 앞/뒤의 "." 를 제거한다.
//
//		// PanelID_1.Judge_1.Code_1.PanelID_2.Judge_2.Code_2
//		int i = 0;
//		while(AfxExtractSubString(strTemp, strPacket, i, '.'))
//		{
//			if(i == 0)		strPanelID_1 = strTemp;
//			else if(i == 1)	strJudge_1   = strTemp;
//			else if(i == 2) strCode_1    = strTemp;
//			else if(i == 3)	strPanelID_2 = strTemp;
//			else if(i == 4)	strJudge_2   = strTemp;
//			else if(i == 5) strCode_2    = strTemp;
//			i++;
//		}		
//		//if (strPanelID_1.IsEmpty()) strPanelID_1.Format(_T("PNL000000000000%03d"), 1);
//		//if (strPanelID_2.IsEmpty()) strPanelID_2.Format(_T("PNL000000000000%03d"), 2);
//
//		strPanelID_1.TrimRight();
//		strPanelID_2.TrimRight();
//				
//		G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s, %s, %s, %s, %s"), 
//			nVisionID, VTM_CLASSIFY_END, strPanelID_1, strJudge_1, strCode_1, strPanelID_2, strJudge_2, strCode_2);
//
//		// 작업중... Forward / Backward Scan 구분 처리...
//	//	m_fnSetInspectEndFlag(nVisionID, TRUE);		
//#if 1
//		
//		nRet = pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1, strPanelID_2, nVisionID);
//
//#endif
//		TRACE(_T("Classify End"));
	}
	else if(strPacket.Find(VTM_LUMINANCE_REPORT) != -1)
	{
		// Param 1 -> Camera No
		// Param 2 -> 휘도값, Value
		strPacket.Replace(VTM_LUMINANCE_REPORT, _T(""));
		strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);		// 앞/뒤의 "." 를 제거한다.

		// Camera No.Value
		int i = 0;
		while(AfxExtractSubString(strTemp, strPacket, i, '.'))
		{
			if(i == 0)		srtCameraNo = strTemp;
			else if(i == 1)	strLumValue = strTemp;
			i++;
		}

		int nCamNo = _tstoi(srtCameraNo);
		int nLunVal = _tstoi(strLumValue);

		G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %d, %d"), 
			nVisionID, VTM_LUMINANCE_REPORT, nCamNo, nLunVal);

		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgLuminanceState->m_fnSetLuminanceValue(nVisionID, nCamNo,nLunVal);
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgStageState->m_fnSetLuminanceValue(nVisionID, nCamNo,nLunVal); 
	}

	else if (strPacket.Find(PTM_RESULT) != -1)
	{	
		/////////////////////////////////////////////
		/*Result Data 용 Panel 정보 처리 부*/
		
		CString strtemp = _T("");
		CString strPgSocket;
	
		G_AddLog_SEQ(nVisionID - 1, _T("[Packet Signal Flag][%d] %s : %s"), SIGNAL_PC, strPgSocket, strPacket);
		// Param 1 -> Panel ID
		// Param 2 -> Judge
		// Param 3 -> Defect Code
		strPacket.Replace(PTM_RESULT, _T(""));
		strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);		// 앞/뒤의 "." 를 제거한다.

		// PanelID_1.Judge_1.Code_1.PanelID_2.Judge_2.Code_2
		int i = 0;
		while(AfxExtractSubString(strTemp, strPacket, i, '.'))
		{
			if(i == 0)		strPanelID_1   = strTemp.TrimLeft();
			else if(i == 1)	strPanelID_2   = strTemp.TrimLeft();
			else if(i == 2) strContResult1   = strTemp.TrimLeft();
			else if(i == 3) strPgResult1   = strTemp.TrimLeft();
			else if(i == 4) strPgVal1      = strTemp.TrimLeft();
			else if(i == 5) strContResult2   = strTemp.TrimLeft();
			else if(i == 6) strPgResult2   = strTemp.TrimLeft();
			else if(i == 7) strPgVal2      = strTemp.TrimLeft();
			else if(i == 8) strConFlag     = strTemp.TrimLeft();
			i++;
		}		
		int m_bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 0);
		
		if(m_bUseSemaphore)
			nRet = pFrame->m_DataManager.m_fnPgClassifyProcInfo(strPanelID_1, strPanelID_2, strContResult1, strPgResult1,
															strPgVal1,strContResult2, strPgResult2,	strPgVal2, strConFlag, nVisionID);
	}
	else if(strPacket.Find(VTM_JUDGE_END) != -1)	//2015.06.17
	{
		strPacket.Replace(VTM_JUDGE_END, _T(""));
		strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		int i = 0;
		while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//VTM.JUDGE.END.PanelID.OK or NG
		{
			if(i == 0)		strPanelID_1 = strTemp;
			else if(i == 1)	strState = strTemp;				
			i++;
		}
		strPanelID_1.TrimRight();

		if (strState == _T("OK"))
			bNgFlag = FALSE;
		else if (strState == _T("NG"))
			bNgFlag = TRUE;

		G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
			nVisionID, VTM_JUDGE_END, strPanelID_1,strState);	
	}
	
		else if(strPacket.Find(VTM_PROCESS_INTERLOCK) != -1)		//2015.10.20
	{
		//long lRet = -1;
		//CString strTmp = _T("");
		//strPacket.Replace(VTM_PROCESS_INTERLOCK, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	
		//CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//VTM.PROCESS.INTERLOCK.ON or OFF
		//{
		//	if(i == 0)		strInterlockFlag = strTemp;				
		//	i++;
		//}
		//if (strInterlockFlag == _T("ON"))
		//{
		//	if (nVisionID == VISION_PC_1)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK1, 1);
		//	else if (nVisionID == VISION_PC_2)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK2, 1);
		//	else if (nVisionID == VISION_PC_3)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK3, 1);
		//}
		//else if (strInterlockFlag == _T("OFF"))
		//{
		//	if (nVisionID == VISION_PC_1)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK1, 0);
		//	else if (nVisionID == VISION_PC_2)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK2, 0);
		//	else if (nVisionID == VISION_PC_3)
		//		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK3, 0);
		//}

		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s"), 
		//	nVisionID, VTM_PROCESS_INTERLOCK, strInterlockFlag);
	}
	else if (strPacket.Find(VTM_RECIPE_SEND) != -1) //2015.11.05
	{
		//CString strTempRecipe = _T("");	
		//strPacket.Replace(VTM_RECIPE_SEND, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);		// 앞/뒤의 "." 를 제거한다.

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))
		//{
		//	if(i == 0)		strRecipeFlag = strTemp;
		//	else if(i == 1)	strRecipe = strTemp;
		//	i++;
		//}

		//m_bRecvRecipeRes[nVisionID - 1] = TRUE;

		//int nUpdateFlag = _tstoi(strRecipeFlag);

		//strTempRecipe = strRecipe;
		//strTempRecipe.Replace(_T("#"), _T("."));

		//pFrame->m_DataManager.m_fnSetRecipeVision(nVisionID, strTempRecipe, nUpdateFlag);

		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s"), 
		//	nVisionID, VTM_RECIPE_SEND);
	}
	else if(strPacket.Find(ATM_ALIGN_RES) != -1)	//2015.08.19
	{
		//int nResult = 0;
		//strPacket.Replace(ATM_ALIGN_RES, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.Align12Result.Align34Result
		//{
		//	if(i == 0)		strPanelID_1 = strTemp;
		//	else if(i == 1)	strAlign_Result = strTemp;  //1 = OK, 0 = NG
		//	i++;
		//}
		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		//	nVisionID, ATM_ALIGN_RES, strPanelID_1, strAlign_Result);
		////Align Result Save
		//nResult = _ttoi(strAlign_Result);
		//strPanelID_1.TrimRight();
		//stAlignInfo = pFrame->m_DataManager.m_fnReadAlignData(strPanelID_1);
		//strVal = stAlignMeasureInfo.cPanelID; strVal.TrimRight();
		//if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
		//{
		//	pFrame->m_DataManager.m_fnWriteAlignData(strPanelID_1, TRUE, nResult);
		//}
	}
	else if(strPacket.Find(ATM_CFMEASUREDST_RES) != -1)	//2015.08.19
	{
		//CString strTempMeasure = _T("");
		//strPacket.Replace(ATM_CFMEASUREDST_RES, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.MeasureData.MeasureFailFlag
		//{
		//	if(i == 0)		strPanelID_1 = strTemp;
		//	else if(i == 1)	strMeasureData = strTemp;
		//	else if(i == 2)	strMeasureFail = strTemp;
		//	i++;
		//}

		//if (strMeasureFail == _T("101"))
		//	pFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strPanelID_1, strPanelID_1);

		//strTempMeasure = strMeasureData;
		//strTempMeasure.Replace(_T("#"), _T("."));
		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		//	nVisionID, ATM_CFMEASUREDST_RES, strPanelID_1, strTempMeasure);
		////Measure Data Save
		//pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, 1);
	}
	else if(strPacket.Find(ATM_CFMEASUREANG_RES) != -1)	//2015.08.19
	{
		//CString strTempMeasure = _T("");
		//strPacket.Replace(ATM_CFMEASUREANG_RES, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.MeasureData
		//{
		//	if(i == 0)		strPanelID_1 = strTemp;
		//	else if(i == 1)	strMeasureData = strTemp;				
		//	i++;
		//}
		//strTempMeasure = strMeasureData;
		//strTempMeasure.Replace(_T("#"), _T("."));
		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		//	nVisionID, ATM_CFMEASUREANG_RES, strPanelID_1, strTempMeasure);
		////Measure Data Save
		//pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, 3);
	}
	else if(strPacket.Find(ATM_TFTMEASUREDST_RES) != -1)	//2015.08.19
	{
		//CString strTempMeasure = _T("");
		//strPacket.Replace(ATM_TFTMEASUREDST_RES, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.MeasureData.MeasureFailFlag
		//{
		//	if(i == 0)		strPanelID_1 = strTemp;
		//	else if(i == 1)	strMeasureData = strTemp;
		//	else if(i == 2)	strMeasureFail = strTemp;				
		//	i++;
		//}

		//if (strMeasureFail == _T("102"))
		//	pFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strPanelID_1, strPanelID_1);

		//strTempMeasure = strMeasureData;
		//strTempMeasure.Replace(_T("#"), _T("."));
		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		//	nVisionID, ATM_TFTMEASUREDST_RES, strPanelID_1, strTempMeasure);
		////Measure Data Save
		//pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, 3);
	}
	else if(strPacket.Find(VTM_TFTMEASUREANG_RES) != -1)	//2015.08.19
	{
	//	CString strTempMeasure = _T("");
	//	strPacket.Replace(VTM_TFTMEASUREANG_RES, _T(""));
	//	strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

	//	int i = 0;
	//	while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.MeasureData
	//	{
	//		if(i == 0)		strPanelID_1 = strTemp;
	//		else if(i == 1)	strMeasureData = strTemp;				
	//		i++;
	//	}
	//	strTempMeasure = strMeasureData;
	//	strTempMeasure.Replace(_T("#"), _T("."));
	//	G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
	//		nVisionID, VTM_TFTMEASUREANG_RES, strPanelID_1, strTempMeasure);
	//	//Measure Data Save
	//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, TRUE, TRUE, strTempMeasure, 4);
	}
	else if(strPacket.Find(VTM_TFTMEASURE_RES) != -1)	//2015.08.19
	{
		//CString strTempMeasure = _T("");
		//CString strTFTMeasureDST = _T("");
		//CString strTFTMeasureANG = _T("");
		//double DSTX[4] = {0}, DSTY[4]= {0},DSTW[2]= {0};
		//strPacket.Replace(VTM_TFTMEASURE_RES, _T(""));
		//strPacket = strPacket.Mid(1, strPacket.GetLength() - 1);	

		//int i = 0;
		//while(AfxExtractSubString(strTemp, strPacket, i, '.'))		//ATM.MEASURE.RES.PanelID.MeasureData
		//{
		//	if(i == 0)		strPanelID_1 = strTemp;
		//	else if(i == 1)	strTempMeasure = strTemp;	
		//	i++;
		//}

		//i = 0;
		//while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
		//{
		//	if(i == 0)		strX1 = strTemp;
		//	else if(i == 1)	strY1 = strTemp;	
		//	else if(i == 2)	strX2 = strTemp;
		//	else if(i == 3)	strY2 = strTemp;
		//	else if(i == 4)	strAngle1 = strTemp;
		//	else if(i == 5)	strAngle2 = strTemp;
		//	else if(i == 6)	strDistance = strTemp;
		//	i++;
		//}

		//strX1.Replace(_T("#"), _T("."));
		//strY1.Replace(_T("#"), _T("."));
		//strX2.Replace(_T("#"), _T("."));
		//strY2.Replace(_T("#"), _T("."));
		//strAngle1.Replace(_T("#"), _T("."));
		//strAngle2.Replace(_T("#"), _T("."));
		//strDistance.Replace(_T("#"), _T("."));

		//_stscanf(strX1, _T("%lf"), &dX1);
		//_stscanf(strY1, _T("%lf"), &dY1);
		//_stscanf(strX2, _T("%lf"), &dX2);
		//_stscanf(strY2, _T("%lf"), &dY2);
		//_stscanf(strAngle1, _T("%lf"), &dAngle1);
		//_stscanf(strAngle2, _T("%lf"), &dAngle2);
		//_stscanf(strDistance, _T("%lf"), &dDistance);

		//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s, %s, %s, %s, %s, %s, %s, %s"), 
		//	nVisionID, VTM_TFTMEASURE_RES, strPanelID_1, strX1, strY1, strX2, strY2, strAngle1, strAngle2, strDistance);
		////Measure Data Save
		//stAlignMeasureInfo = pFrame->m_DataManager.m_fnReadAlignMeasureData(strPanelID_1);
		//if (stAlignMeasureInfo.bVision1Complete) //PC5
		//{
		//	DSTX[0] = stAlignMeasureInfo.dVision1_X1;	//PC5
		//	DSTY[0] = stAlignMeasureInfo.dVision1_Y1;	//PC5
		//	DSTX[1] = stAlignMeasureInfo.dVision1_X2;	//PC5
		//	DSTY[1] = stAlignMeasureInfo.dVision1_Y2;	//PC5
		//	DSTX[2]	= dX1;								//PC6
		//	DSTY[2]	= dY1;								//PC6
		//	DSTX[3] = dX2;								//PC6
		//	DSTY[3] = dY2;								//PC6
		//	strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")	//2015.10.31 .2lf->.0lf
		//								,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);

		//	DSTW[0] =(stAlignMeasureInfo.dVision1_Y1 + dY1);//W3
		//	DSTW[1] =(stAlignMeasureInfo.dVision1_Y2 + dY2);//W4
		//	strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
		//							, dDistance, stAlignMeasureInfo.dVision1_Distance
		//							, DSTW[0], DSTW[1]
		//							, dAngle1, dAngle2
		//							, stAlignMeasureInfo.dVision1_Angle1, stAlignMeasureInfo.dVision1_Angle2);
		//	/*
		//	stAlignMeasureInfo.dVision1_X1
		//	stAlignMeasureInfo.dVision1_Y1
		//	stAlignMeasureInfo.dVision1_X2
		//	stAlignMeasureInfo.dVision1_Y2
		//	stAlignMeasureInfo.dVision1_Angle1
		//	stAlignMeasureInfo.dVision1_Angle2
		//	stAlignMeasureInfo.dVision1_Distance
		//	*/
		//	//2015.11.03
		//	//if ()
		//	//	pFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strPanelID_1);
		//	//2015.11.03
		//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, 2);
		//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, 4);
		//	G_AddLog_SEQ(49, _T("[Packet Vision] %s : %s DST - %s, ANG - %s"), 
		//		VTM_TFTMEASURE_RES, strPanelID_1, strTFTMeasureDST, strTFTMeasureANG);
		//}
		//else if (stAlignMeasureInfo.bVision2Complete) //PC6
		//{
		//	DSTX[0] = dX1;								//PC5
		//	DSTY[0] = dY1;								//PC5
		//	DSTX[1] = dX2;								//PC5
		//	DSTY[1] = dY2;								//PC5
		//	DSTX[2]	= stAlignMeasureInfo.dVision2_X1;	//PC6
		//	DSTY[2]	= stAlignMeasureInfo.dVision2_Y1;	//PC6
		//	DSTX[3]	= stAlignMeasureInfo.dVision2_X2;	//PC6
		//	DSTY[3]	= stAlignMeasureInfo.dVision2_Y2;	//PC6

		//	strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
		//								,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);

		//	DSTW[0] =(stAlignMeasureInfo.dVision2_Y1 + dY1); //W3
		//	DSTW[1] =(stAlignMeasureInfo.dVision2_Y2 + dY2); //W4
		//	strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
		//							,stAlignMeasureInfo.dVision2_Distance, dDistance
		//							,DSTW[0], DSTW[1]
		//							,stAlignMeasureInfo.dVision2_Angle1, stAlignMeasureInfo.dVision2_Angle2
		//							,dAngle1, dAngle2);
		//				/*
		//	stAlignMeasureInfo.dVision2_X1
		//	stAlignMeasureInfo.dVision2_Y1
		//	stAlignMeasureInfo.dVision2_X2
		//	stAlignMeasureInfo.dVision2_Y2
		//	stAlignMeasureInfo.dVision2_Angle1
		//	stAlignMeasureInfo.dVision2_Angle2
		//	stAlignMeasureInfo.dVision2_Distance
		//	*/
		//	//2015.11.03
		//	//if ()
		//	//	pFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strPanelID_1);
		//	//2015.11.03
		//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, 2);
		//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, 4);
		//	G_AddLog_SEQ(49, _T("[Packet Vision] %s : %s DST - %s, ANG - %s"), 
		//		VTM_TFTMEASURE_RES, strPanelID_1, strTFTMeasureDST, strTFTMeasureANG);
		//}
		//else
		//	pFrame->m_DataManager.m_fnWriteMeasureData(strPanelID_1, TRUE,  dX1, dY1, dX2, dY2, dAngle1, dAngle2, dDistance, nVisionID);
	}
	else
	{
		G_AddLog(3, 2, _T("[Packet Vision][%d] Undefined => %s"), nVisionID, strPacket);
		return;
	}

#ifdef DEBUG_TEST
#else
	if(pFrame->m_stParamInfo.nControlMode)		// Control Mode PC
	{
		m_pSockCtrlPCSvr->m_fnSendClient(strRecvMsg, nRecvLen);
	}
	else										// Control Mode PLC
	{
		// 작업중...
	}
#endif
}

void CSockManager::m_fnParsingMsg_CIM(char* chBuf, int nRecvLen)
{
	CString strRecvMsg, strPacket;
	char chRecvBuf[SOCK_BUFF_LENGTH] = {0};

	memcpy(chRecvBuf, chBuf, nRecvLen);

	strRecvMsg = strPacket = chRecvBuf;
	strPacket = strPacket.Mid(1, strPacket.GetLength() - 2);

	// 작업중... 메세지 처리...
}

int CSockManager::m_fnFindStr(const char *str, int nSize, const char *what)
{ 
	int i, j, Ls, Lw;
	
	Ls = nSize;
	Lw = strlen(what);

	for(i = 0; i <= Ls - Lw; i++)
	{
		if(str[i] == what[0])
		{
			for(j = 1; j < Lw; j++)
			{
				if(str[i + j] != what[j])
					break;
			}

			if(j == Lw) return i;
		}
	}

	return -1;
}

BOOL CSockManager::m_fnIsInspectEnd()
{
	for(int i = 0; i < m_pSockVisionSvr->m_fnGetClientCount(); i++)
	{
		if(m_bInspectEndFlag[i] == FALSE)
			return FALSE;
	}

	return TRUE;
}

void CSockManager::m_fnSetGrabEndFlag(int nVisionID, BOOL bFlag)
{
	if(nVisionID > m_pSockVisionSvr->m_fnGetClientCount())
	{
		G_AddLog(3, 2, _T("Set Grab End Flag Error = %d"), nVisionID);
		return;
	}

	m_bGrabEndFlag[nVisionID - 1] = TRUE;
}

void CSockManager::m_fnSetInspectEndFlag(int nVisionID, BOOL bFlag)
{
	if(nVisionID > m_pSockVisionSvr->m_fnGetClientCount())
	{
		G_AddLog(3, 2, _T("Set Inspect End Flag Error = %d"), nVisionID);
		return;
	}

	m_bInspectEndFlag[nVisionID - 1] = TRUE;
}

void CSockManager::m_fnSetGrabReadyRes(int nVisionID, int nRes)
{
	if(nVisionID > m_pSockVisionSvr->m_fnGetClientCount())
	{
		G_AddLog(3, 2, _T("Set Ready Res Error = %d"), nVisionID);
		return;
	}

	m_nGrabReadyRes[nVisionID - 1] = nRes;
}

void CSockManager::m_fnSetGrabEndRes(int nVisionID, int nRes)
{
	if(nVisionID > m_pSockVisionSvr->m_fnGetClientCount())
	{
		G_AddLog(3, 2, _T("Set Grab End Res Error = %d"), nVisionID);
		return;
	}

	m_nGrabEndRes[nVisionID - 1] = nRes;
}

void CSockManager::m_fnResetFlag(int nVisionID)
{
	if(nVisionID > m_pSockVisionSvr->m_fnGetClientCount())
	{
//		G_AddLog(3, 2, _T("Reset Flag Error = %d"), nVisionID);
		return;
	}

	m_bGrabEndFlag[nVisionID - 1] = FALSE;
	m_bInspectEndFlag[nVisionID - 1] = FALSE;
	m_nVisionState[nVisionID - 1] = STATE_IDLE;
	m_bRecvReadyRes[nVisionID - 1] = FALSE;
}

void CSockManager::m_fnRequestVisionState(int nVisionID)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	if(pSockClient)
	{
		CString strMsg;
		strMsg.Format(_T("%c.%s.%c"), STX, MTV_STATE_REQ, ETX);

		int nLen = sizeof(MTV_STATE_REQ) + 4;

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);
	}
}

void CSockManager::m_fnRequestGrabReady(int nVisionID, int nScanDir, CString strModelID, CString strPanelID_1, CString strPanelID_2)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	/*if(strPanelID_1 == "")	strPanelID_1 = _T("PNL000000000000001");
	if(strPanelID_2 == "")	strPanelID_2 = _T("PNL000000000000002");*/

	int nLen = 0;

	if(pSockClient)
	{
		CString strMsg;

		if(nScanDir == SCAN_DIR_FORWARD)
		{
			strMsg.Format(_T("%c.%s.%s.%s.%s.%c"), STX, MTV_READY_REQ_F, strModelID, strPanelID_1, strPanelID_2, ETX);
			nLen = sizeof(MTV_READY_REQ_F) + strModelID.GetLength() + strPanelID_1.GetLength() + strPanelID_2.GetLength() + 4;
		}
		else
		{
			strMsg.Format(_T("%c.%s.%s.%s.%s.%c"), STX, MTV_READY_REQ_B, strModelID, strPanelID_1, strPanelID_2, ETX);
			nLen = sizeof(MTV_READY_REQ_B) + strModelID.GetLength() + strPanelID_1.GetLength() + strPanelID_2.GetLength() + 4;
		}

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);
	}
}

void CSockManager::m_fnRequestGrabStart(int nVisionID)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	if(pSockClient)
	{
		CString strMsg;
		strMsg.Format(_T("%c.%s.%c"), STX, MTV_GRAB_START, ETX);

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, strMsg.GetLength());
	}
}

void CSockManager::m_fnSendInspectEnd(int nVisionID, CString strPanelID_1, CString strPanelID_2)
{
	CSockVisionClt* pSockClient = NULL;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	int nVisionCnt = 0;
		
	while(TRUE){
		if (!nVisionID)
		{			
			nVisionCnt ++;
			pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionCnt);

			int nLen = 0;

			if(pSockClient)
			{
				CString strMsg;

				strMsg.Format(_T("%c.%s.%s.%s.%d.%c"), STX, VTM_INSPECT_END, strPanelID_1, strPanelID_2, nVisionCnt, ETX);
				nLen = sizeof(VTM_INSPECT_END) + strPanelID_1.GetLength() + strPanelID_2.GetLength() + sizeof(nVisionCnt) + 4;

				m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);
			}

			if(nVisionCnt > pFrame->m_stParamInfo.nCountVisionClient)
			{
				break;
			}
		}
	}

	
}

//2015.11.05
void CSockManager::m_fnRequestVisionRecipe(int nVisionID)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	if(pSockClient)
	{
		CString strMsg;
		strMsg.Format(_T("%c.%s.%c"), STX, MTV_RECIPE_LIST_REQ, ETX);

		int nLen = sizeof(MTV_RECIPE_LIST_REQ) + 4;

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);
	}
}

void CSockManager::m_fnSelectVisionRecipe(int nVisionID, CString strRecipe)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	if(pSockClient)
	{
		CString strMsg;
		strMsg.Format(_T("%c.%s.%s.%c"), STX, MTV_SELECT_RECIPE, strRecipe, ETX);

		int nLen = sizeof(MTV_SELECT_RECIPE) + + strRecipe.GetLength() + 4;

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);
	}
}
void CSockManager::m_fnRequestAlign(int nVisionID, CString strPanelID)
{
	CSockVisionClt* pSockClient = NULL;

	pSockClient = (CSockVisionClt*)m_pSockVisionSvr->m_fnGetClient(nVisionID);

	if(pSockClient)
	{
		CString strMsg;
		strMsg.Format(_T("%c.%s.%s.%c"), STX, MTA_ALIGN_REQ, strPanelID, ETX);

		m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, strMsg.GetLength());
	}
}
