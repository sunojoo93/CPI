
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "StdAfx.h"
#include "ThreadStatePLC.h"
#include "MainFrm.h"
#include "ViewAuto.h"
DWORD dwTime = 0;
int nCount = 0;

CThreadStatePLC::CThreadStatePLC(void)
{
	m_bReadPLC = FALSE;
}

CThreadStatePLC::~CThreadStatePLC(void)
{
}

int CThreadStatePLC::m_fnInitThread(int nEqpSize, int nEqpType, CDlgPLCSetup* pDlgPLCSetup)
{
	//2015.10.30 Mr.Chu
	m_nEqpSize = nEqpSize;

	m_nEqpType = nEqpType;

	m_pDlgPLCSetup = pDlgPLCSetup;

	dwTime = GetTickCount();

	return CSequenceManager::m_fnStartScenario(TRUE, FALSE);
}

void CThreadStatePLC::m_fnDeInitThread()
{
	CSequenceManager::m_fnDeInit();
}

void CThreadStatePLC::m_fnCheck()
{
	nCount++;
	if(nCount == 1000 / SLEEP_COUNT)
	{
		DWORD dwTemp = GetTickCount() - dwTime;
//		G_AddLog(3, 2, _T("Read PLC Time : %.03f"), (dwTemp / (1000.0 * (1000 / SLEEP_COUNT))) - (SLEEP_COUNT / 1000.0));
		nCount = 0;
		dwTime = GetTickCount();
	}


	//if (m_pDlgPLCSetup->m_stPlcMapPLC[0].bReset)
	//{
	//	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	//	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->ClickGxbtnResetAlarm();
	//}


	if(m_bReadPLC)		// m_fnDeInitThread() 전에 종료시키지 않으면 Thread 종료 처리 않됨.
	{
	//	Sleep(100);
		m_fnReadPCIO();
		m_fnReadPLCIO();
	}
}

void CThreadStatePLC::m_fnReadPCIO()
{
	short shVal = 0;

	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		//2015.10.30 Mr.Chu
		if( m_nEqpSize == 0 || m_nEqpSize == 1 )	// 중 , 소형
		{
			if(i == 0)
			{
				m_pDlgPLCSetup->m_stPlcMapPC[0].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_STAGE_1);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_ALARM);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_READY);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_END);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ERROR);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_PPIDMISMATCH);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_SEQ_ERROR);
			}
			else if(i == 1)
			{
				m_pDlgPLCSetup->m_stPlcMapPC[1].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_STAGE_2);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_ALARM);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_READY);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ACK);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_END);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ERROR);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_PPIDMISMATCH);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_SEQ_ERROR);
			}
			else if(i == 2)
			{
				if(m_nEqpType == EQP_TYPE_ACS)
				{
					m_pDlgPLCSetup->m_stPlcMapPC[2].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_ALIVE);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_ALARM);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_READY);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_FORWARD);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ACK);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_END);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ERROR);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_PPIDMISMATCH);
					m_pDlgPLCSetup->m_stPlcMapPC[2].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_SEQ_ERROR);
				}
				else
					continue;
			}
		}
		else if( m_nEqpSize == 2 )		// 대형
		{
			if(i == 0)
			{
				m_pDlgPLCSetup->m_stPlcMapPC[0].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_STAGE_1);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_ALARM);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_READY);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ACK);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_END);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_GRAB_ERROR);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_PPIDMISMATCH);
				m_pDlgPLCSetup->m_stPlcMapPC[0].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S1_SEQ_ERROR);
			}
			else if(i == 1)
			{
				m_pDlgPLCSetup->m_stPlcMapPC[1].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_STAGE_2);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_ALARM);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_READY);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ACK);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_END);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_GRAB_ERROR);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_PPIDMISMATCH);
				m_pDlgPLCSetup->m_stPlcMapPC[1].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S2_SEQ_ERROR);
			}
			else if(i == 2)
			{
				m_pDlgPLCSetup->m_stPlcMapPC[2].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_STAGE_3);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bAlarm			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_ALARM);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_READY);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bForward		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabAck		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ACK);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabEnd		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_END);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_GRAB_ERROR);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bPPIDMismatch	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_PPIDMISMATCH);
				m_pDlgPLCSetup->m_stPlcMapPC[2].bSeqError		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S3_SEQ_ERROR);
			}
			else if(i == 3)
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S4_GRAB_READY);
			else if(i == 4)
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S5_GRAB_READY);
			else if(i == 5)
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S6_GRAB_READY);
			else if(i == 6)
				m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S7_GRAB_READY);

			if(m_nEqpType == EQP_TYPE_ACS)
			{
				if(i == 7)
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S8_GRAB_READY);
				else if(i == 8)
					m_pDlgPLCSetup->m_stPlcMapPC[2].bGrabReady		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PC_S9_GRAB_READY);
			}
			else
				continue;

		}

	}
}

void CThreadStatePLC::m_fnReadPLCIO()
{
	CString strModelID, strPanelID_1, strPanelID_2;
	short shVal = 0;

	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		if(i == 0)
		{
			m_pDlgPLCSetup->m_stPlcMapPLC[0].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_STAGE_1);
			m_pDlgPLCSetup->m_stPlcMapPLC[0].bGrabReq		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_REQ);
			m_pDlgPLCSetup->m_stPlcMapPLC[0].bScanForward	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_FORWARD);
			m_pDlgPLCSetup->m_stPlcMapPLC[0].bGrabEndOK		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_GRAB_END_OK);
			m_pDlgPLCSetup->m_stPlcMapPLC[0].bReset			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S1_RESET);

			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_MODEL_ID, LEN_MODEL_ID, strModelID);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S1_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
		}
		else if(i == 1)
		{
			m_pDlgPLCSetup->m_stPlcMapPLC[1].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_STAGE_1);
			m_pDlgPLCSetup->m_stPlcMapPLC[1].bGrabReq		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_REQ);
			m_pDlgPLCSetup->m_stPlcMapPLC[1].bScanForward	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_FORWARD);
			m_pDlgPLCSetup->m_stPlcMapPLC[1].bGrabEndOK		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_GRAB_END_OK);
			m_pDlgPLCSetup->m_stPlcMapPLC[1].bReset			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S2_RESET);

			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_MODEL_ID, LEN_MODEL_ID, strModelID);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
			m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
		}
		else if(i == 2)
		{
			if(m_nEqpType == EQP_TYPE_ACS)
			{
				m_pDlgPLCSetup->m_stPlcMapPLC[2].bAlive			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_STAGE_1);
				m_pDlgPLCSetup->m_stPlcMapPLC[2].bGrabReq		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_REQ);
				m_pDlgPLCSetup->m_stPlcMapPLC[2].bScanForward	= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_FORWARD);
				m_pDlgPLCSetup->m_stPlcMapPLC[2].bGrabEndOK		= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_GRAB_END_OK);
				m_pDlgPLCSetup->m_stPlcMapPLC[2].bReset			= m_pDlgPLCSetup->m_fnGetDevice(ADDR_PLC_S3_RESET);

				m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_MODEL_ID, LEN_MODEL_ID, strModelID);
				m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, strPanelID_1);
				m_pDlgPLCSetup->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_2, LEN_PANEL_ID, strPanelID_2);
			}
			else
				continue;
		}

		m_pDlgPLCSetup->m_stPlcMapPLC[i].strModelID = strModelID;
		m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_1 = strPanelID_1;
		m_pDlgPLCSetup->m_stPlcMapPLC[i].strPanelID_2 = strPanelID_2;
	}


//	if (m_pDlgPLCSetup->m_stPlcMapPLC[0].bReset)
//	{
//		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
//
//		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->ClickGxbtnResetAlarm();
//	}
}
