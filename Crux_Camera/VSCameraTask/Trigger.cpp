// NLight.cpp: implementation of the CLight class.
// �ս� ���� ��Ʈ�ѿ�
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "include.h"
#include "extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CTrigger::CTrigger()
{
	m_bFirst = true;
	m_bTriFailFlag = FALSE;
	m_bChsetCheck = FALSE;	//2016.01.19 add by mby 
}
CTrigger::~CTrigger()
{
	m_CurrentMsg = _T("");
}


BOOL	CTrigger::Send_Trigger(LPCTSTR szFormatString, ...)
{
	BOOL bRet;
	va_list	args;

	//size_t i, j, pos;  
	size_t i;
	char Lens[10];
	
	va_start(args, szFormatString);

	int len = _vscprintf(szFormatString, args);

	unsigned char *buffer = (unsigned char*)malloc((len + 5) * sizeof(unsigned char));
	unsigned char sum= 0;

	sprintf((char *)&Lens, "%02d", len);
	buffer[0]	= PID_SET_STX;
	sprintf((char *)&buffer[1], "%c", Lens[0]);
	sprintf((char *)&buffer[2], "%c", Lens[1]);

	int k = 0;
	for ( i = 0; i < len; i++)
		sprintf((char *)&buffer[3 + i], "%c", szFormatString[i]);

	sum = CalcChecksum((unsigned char*)szFormatString, len);

    buffer[3+len] = sum;
	buffer[4+len] = PID_SET_ETX;

	bRet = WriteComm((UCHAR *)buffer, len+5);

	//���� �޽����� ���� ������ ���������� ����Ѵ�. add by chlee 2016.02.24
	m_bReturned = false;
	//while(!m_bReturned) Sleep(1);

 	free(buffer);
	return bRet;
}

 unsigned char  CTrigger::CalcChecksum(unsigned char *data, int leng)
 	{
 		unsigned char csum = 0;
 		for (;leng > 0;leng--) 
 			csum += *data++;

 		return csum = csum%256;
 	}

BOOL CTrigger::ConnectTrigger(int nPortNumber, int nBaudRate)
{
	CString		strPortName;
	CString		strPanelId;
	DWORD		dwThreadID;

	if(this->m_Connected)		return TRUE;

	//2016.11.08 add by mby 
	strPanelId = pSys->GetPanelID();
	if(strPanelId == ""){
		strPanelId = "PANELID";
	}

	strPortName.Format("COM%d", nPortNumber);
	if(this->OpenPort(strPortName, DWORD(nBaudRate), nPortNumber-1))
	{
		this->m_Connected = TRUE;

		//2016.11.08 add by mby 
		//Message PanelID,EventID,EventInfo		
		strPortName.Format(_T("EVENT:ConnectTrigger PortNum(%d) %s,%d"),nPortNumber, strPanelId, EVT_CONNECT_TRIGGER);
		pSys->EventLogFile(&strPortName, LOG_LEVEL_BASIC);

		//	���� ��Ʈ ���� ������ ���� ...
		m_hThreadWatchComm = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)ThreadWatchTrigger, this, 0,&dwThreadID);
		if(!m_hThreadWatchComm)	// 2014.12.20 ljk
		{
			ClosePort();			
			return FALSE;
		}
	}else{
		//Message PanelID,ErrorID,ErrorLevel,ErrorInfo		
		strPortName.Format(_T("%s,%d,%d,ConnectTrigger: OpenPort Fail"),strPanelId,TRI_CONNECT_FAIL,3);
		pSys->AlarmLogFile(&strPortName, LOG_LEVEL_BASIC);
		AfxMessageBox("Trigger Comport disconnection!! Please Check The Cable or Borad ");
		return FALSE;
	}
	
	return TRUE;	
}


UINT	ThreadWatchTrigger(CTrigger* pTrigger)
{
	BYTE		Buff;
	DWORD		dwEvent;
	BOOL		bOk		= TRUE;
	int			total	= 0;
	int			nRetry  = 0;
	CString		returnMsg;
	CString		logMsg;
	CString		strPanelId;

	while(pTrigger->m_Connected){
		dwEvent = 0;  //	�̺�Ʈ Ŭ���� 
		WaitCommEvent(pTrigger->m_hComm, &dwEvent, NULL);  // ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
		
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)  // ��Ʈ�� �̺�Ʈ�� �߻��ϸ� 
		{
			// 512�� �о  �������� �������� �ƴϸ� ���� �̺�Ʈ�� ��ٸ���.  
			for(int i=0; i<512; i++)
			{
				int ret = pTrigger->ReadComm(&Buff, 1);  // ���� �ϳ��� �д´�.
				if(ret > 0)  // ���̸� üũ�ϰ�
				{ 
					// ���̰� 0���� ũ�� �ø��� Ŭ������ ���õ� ���ۿ� ���� �� �ϳ��� �ִ´�. 
					pTrigger->m_InBuf[total] = Buff;
					total++;
					// �������ݻ󿡼� ������ ������������ �˻��Ѵ�. 
					if(Buff == 0x03)
					{
						pTrigger->m_InBuf[total]='\0';  // ������ �������̸� ������ �������� ���� ���ڸ� �ְ�
						CString tmpStr(pTrigger->m_InBuf);
						returnMsg = tmpStr;

						memset(pTrigger->m_InBuf, 0, 512);
						total=0;
						break;
					}
				}
			}

			logMsg = _T("[Trigger]Recv Msg : ") + returnMsg;
			pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);

			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
			{
				if(returnMsg.Find("error") != -1){
					if(nRetry > 2){ //���� ���� �� ���� �ʱ�ȭ. retry fail �� ������ ���� �������� ����
						nRetry = 0;
						pTrigger->m_bReturned = true;
						pTrigger->m_Connected = FALSE;	//Trigger ��Ź��� ����
						strPanelId = pSys->GetPanelID();
						if(strPanelId == ""){
							strPanelId = "PANELID";
						}
						//Message PanelID,ErrorID,ErrorLevel,ErrorInfo
						logMsg.Format(_T("%s,%d,%d,ThreadWatchTrigger: Command Fail"),strPanelId,TRI_COMMAND_FAIL,3);
						pSys->AlarmLogFile(&logMsg, LOG_LEVEL_BASIC);
					}
					else{
						logMsg = _T("[Trigger]Retry : ") + pTrigger->m_CurrentMsg;
						pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);
						pTrigger->Send_Trigger(pTrigger->m_CurrentMsg);
						nRetry++;
					}
				}
				else{
					nRetry = 0;
					pTrigger->m_bReturned = true;
				}
			}
		}
	}	
	// ��Ʈ�� ClosePort�� ���� ������ m_bConnected �� FALSE�� �Ǿ� ����.
	pTrigger->m_hThreadWatchComm = NULL;

	return TRUE;	
}

//�޽��� ������ ���� ��� 0.3�ʸ� ����Ѵ�. ��õ��� 2ȸ
void    CTrigger::WaitRecvMsg(){
	int nLimit = 300;
	int nCount = 0;
	int nRetry = 0;

	while( (!m_bReturned) ){
		Sleep(1);
		nCount++;
		if(nCount >= nLimit){ //timeout retry
			if(nRetry >= 2){
				break;
			}

			nCount =0;
			Send_Trigger(m_CurrentMsg);
			CString logMsg = _T("[Trigger]WaitRecvMsg Retry : ") + m_CurrentMsg;
			pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);
			nRetry++;
		}
	}
}

void	CTrigger::SetScanRange(int start, int end, int interval)
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("ENCW,%d,%d,%d"), start, end, interval);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::SetTriggerDir(BOOL bForward)
{
	m_bTrigerDirForword = bForward;
	
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("ENCW,0,0,0,%d"), int((bForward) ? 128 : 255));
		Send_Trigger(m_CurrentMsg);
		CString msg;
		msg.Format(_T("[Trigger]SetTriggerDir : %d"), bForward);
		pSys->CommLogFile(&msg, LOG_LEVEL_BASIC);

		//2016.11.08 add by mby 
		//Message PanelID,EventID,EventInfo
		msg.Format(_T("EVENT:SetTriggerDir Direction(%d) ,%s,%d"),bForward, pSys->GetPanelID(),EVT_SET_TRIGGER_DIR);
		pSys->EventLogFile(&msg, LOG_LEVEL_BASIC);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTrigger::GetCurTrgCount()	
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,5");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::GetCurStart()		
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,10");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::GetCurEnd()			
{	
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,11");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::GetCurInterval()	
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,12");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::GetEncErrCount()	
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,13");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTrigger::GetCurEnc()		
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,14");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTrigger::StopTriggerGen0()	 //Ʈ���� ��ȣ �߻�����			 
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,0,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonOperatingOff();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTrigger::StopTriggerGen1()	 //Ʈ���� ��ȣ �߻�����		 
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonOperatingOff();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTrigger::StartForwardScan()  // ������ ����
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:StartForwardScan() ,%s,%d"), pSys->GetPanelID(),EVT_FORWARD_SCAN);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,2,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonDownDirection(3);		// 0b01 = ��� �������� ������, 0b10 = �Ϻ� �������� ������, ��Ʈ �������� �Ķ���Ϳ���
		Sleep(TriggerDelayCommand());
		DaekonOperatingOn();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTrigger::StartBackwardScan()  // ������ ����
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:StartBackwardScan() ,%s,%d"), pSys->GetPanelID(),EVT_BACKWARD_SCAN);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,3,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonDownDirection(0);		// 0b01 = ��� �������� ������, 0b10 = �Ϻ� �������� ������, ��Ʈ �������� �Ķ���Ϳ���
		Sleep(TriggerDelayCommand());
		DaekonOperatingOn();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerInit0()  // Ʈ���� 0 �ʱ�ȭ
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,3,1,1,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonInitTrigger();
	}
	//Sleep(TriggerDelayCommand());
} 

void    CTrigger::TriggerInit1()  // Ʈ���� 1 �ʱ�ȭ
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,4,1,1,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonInitTrigger();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerOrigin()  // ���� ����
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,255,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonInitEncoder();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerStartPosition8K()  // 0�� �߻��ֱ�, (AOI, Ʈ���Ź�ȣ, ������ġ, ����ġ, �ֱ�)
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,1,50000,800000,26,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
}

void    CTrigger::TriggerStartPosition2K()  // 1�� �߻��ֱ�, (AOI, Ʈ���Ź�ȣ, ������ġ, ����ġ, �ֱ�)
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,2,50000,800000,42,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerCurrentPosition()  // ���� ��ġ�� �б�
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,15,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerGenCount0()  // Ʈ���� 0 �߻� ī���� ���� �б�
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,20,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
}

void    CTrigger::TriggerGenCount1()  // Ʈ���� 1 �߻� ī���� ���� �б�
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,21,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerDebug()				 
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,80,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}  

void    CTrigger::TriggerStartPositionGet_00()  // 0�� �߻��ֱ�, (AOI, Ʈ���Ź�ȣ, ������ġ, ����ġ, �ֱ�)
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,11,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTrigger::TriggerStartPositionGet_01()  // 1�� �߻��ֱ�, (AOI, Ʈ���Ź�ȣ, ������ġ, ����ġ, �ֱ�) 
{
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,12,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

//�߰�
void CTrigger::TriggerStartPositionChannel0(CString strStartPos, CString strEndPos, CString strPeriod)
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:TriggerStartPositionChannel0() ,%s,%d"), pSys->GetPanelID(),EVT_TRIGGER_CHANNEL0);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,1,%s,%s,%s,"), strStartPos, strEndPos, strPeriod);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(0);
		Sleep(TriggerDelayCommand());
		// ���ڴ� 0�� ���������� �����Ѵ�.
		DaekonSetUpperEncoderStart(atof(strStartPos));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderEnd(atof(strEndPos));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPeriod(atof(strPeriod));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderDelay();
	}
	//Sleep(TriggerDelayCommand());
}

void CTrigger::TriggerStartPositionChannel1(CString strStartPos, CString strEndPos, CString strPeriod)
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	CString traceMsg;
	traceMsg.Format(_T("EVENT:TriggerStartPositionChannel1() ,%s,%d"), pSys->GetPanelID(),EVT_TRIGGER_CHANNEL1);
	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,2,%s,%s,%s,"), strStartPos, strEndPos, strPeriod);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(1);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderStart(atof(strStartPos));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderEnd(atof(strEndPos));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPeriod(atof(strPeriod));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderDelay();
	}
	//Sleep(TriggerDelayCommand());
}
BOOL CTrigger::StartTrigger(bool bState)
{
	CSystem* pSys = CSystem::GetInstance();

	if(bState)
	{
		//2016.11.08 add by mby 
		//Message PanelID,EventID,EventInfo
		CString traceMsg;
		traceMsg.Format(_T("EVENT:StartTrigger() ,%s,%d"), pSys->GetPanelID(),EVT_START_TRIGGER);
		pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);

		//ABI�� ó�� Grab ��û �� �ѹ��� ������ �ϵ��� �Ѵ�. 2015.12.08 add by chlee
		if(	!m_bFirst									&& 
			pSys->m_Config.m_nModelType == EQP_TYPE_ABI	&&		// ABI �� ���
			pSys->m_Config.m_nEqpType != EQP_TYPE_BIG	&&		// ABI �� ���
			pSys->m_Config.m_nEqpType != EQP_TYPE_BIG_D9)		// ������ �ƴ� ��� �н�.
		{
			StartForwardScan();
			return TRUE;
		}

		if(m_bTrigerDirForword)
		{
			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
			{
				StopTriggerGen0();
				StopTriggerGen1();		
			}

			//pSys->CommLogFile(&CString("Forword TriggerOrigin"), LOG_LEVEL_DETAIL);
			TriggerOrigin();
			
			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
				TriggerInit0();	
			
			if(pSys->m_Config.m_nInspactEqpNo == 1){
				//pSys->CommLogFile(&CString("Forword TriggerStartPositionChannel0"), LOG_LEVEL_DETAIL);
				TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
				TriggerStartPositionChannel1(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
			}else{
				//pSys->CommLogFile(&CString("Forword TriggerStartPositionChannel0"), LOG_LEVEL_DETAIL);
				TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
				//pSys->CommLogFile(&CString("Forword TriggerStartPositionChannel1"), LOG_LEVEL_DETAIL);
				TriggerStartPositionChannel1(pSys->m_Config.stCam01_Forward.sStartPos, pSys->m_Config.stCam01_Forward.sEndPos, pSys->m_Config.stCam01_Forward.sPeriod);
			}
			////pSys->CommLogFile(&CString("Forword TriggerStartPositionChannel0"), LOG_LEVEL_DETAIL);
			//TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
			////pSys->CommLogFile(&CString("Forword TriggerStartPositionChannel1"), LOG_LEVEL_DETAIL);
			//TriggerStartPositionChannel1(pSys->m_Config.stCam01_Forward.sStartPos, pSys->m_Config.stCam01_Forward.sEndPos, pSys->m_Config.stCam01_Forward.sPeriod);
			
			StartForwardScan();
		}
		else
		{
			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
			{
				StopTriggerGen0();	
				StopTriggerGen1();
			}

			//pSys->CommLogFile(&CString("Backword TriggerStartPositionChannel0"), LOG_LEVEL_DETAIL);
			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos,pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
			//pSys->CommLogFile(&CString("Backword TriggerStartPositionChannel1"), LOG_LEVEL_DETAIL);		
			TriggerStartPositionChannel1(pSys->m_Config.stCam01_Backward.sStartPos,pSys->m_Config.stCam01_Backward.sEndPos, pSys->m_Config.stCam01_Backward.sPeriod);
			
			StartBackwardScan();
		}

		if(m_bFirst) m_bFirst = false; //2015.12.08 add by chlee
	}
	else
	{

		if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
		{
			StopTriggerGen0();
			// 2015.01.07 Comment by CWH - ���ʿ� ����
			//TriggerGenCount0();			
			//TriggerGenCount1();
			//TriggerCurrentPosition();		
		}
		else{
			TriggerGenCount0();			
			TriggerGenCount1();
			TriggerCurrentPosition();
		}
	}

	return TRUE;
}
// TriggerInitSet //2016.08.19 add by mby for Tact time(BIG,MONITOR) S-
BOOL CTrigger::TriggerInitSet(bool bState)
{
	CSystem* pSys = CSystem::GetInstance();
	
	if(bState)
	{
		if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
		{
			StopTriggerGen0();
			TriggerInit0();
		}

		TriggerOrigin();			
		TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);		
	}
	else
	{
		if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
		{
			StopTriggerGen0();
		}	
		TriggerOrigin();				
		TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos,pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
	}		
		
	return TRUE;
}
// TriggerInitSet //2016.08.19 add by mby for Tact time(BIG,MONITOR) E-
BOOL CTrigger::StartTrigger(bool bState, BOOL bIsDir)
{

	if(bState)
	{
		if(bIsDir)
		{
			StopTriggerGen0();
			
			StopTriggerGen1();
			
			TriggerOrigin();
			
			TriggerInit0();
			
			//Forward Cam1 StartPos, EndPos, Period
			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos,pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
			
			//Forward Cam2 StartPos, EndPos, Period
			TriggerStartPositionChannel1(pSys->m_Config.stCam01_Forward.sStartPos,pSys->m_Config.stCam01_Forward.sEndPos, pSys->m_Config.stCam01_Forward.sPeriod);
			
			StartForwardScan();

			TriggerGenCount0();			
			TriggerGenCount1();
		}
		else
		{
			StopTriggerGen0();
			
			StopTriggerGen1();
			
			//TriggerOrigin();
			//
			//Backward Cam1 StartPos, EndPos, Period
			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos,pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
			
			//Backward Cam2 StartPos, EndPos, Period
			TriggerStartPositionChannel1(pSys->m_Config.stCam01_Backward.sStartPos,pSys->m_Config.stCam01_Backward.sEndPos, pSys->m_Config.stCam01_Backward.sPeriod);
			
			StartBackwardScan();

			TriggerGenCount0();			
			TriggerGenCount1();
		}
	}
	else
	{
		StopTriggerGen0();
		TriggerGenCount0();			
		TriggerGenCount1();
		TriggerCurrentPosition();		
	}

	return TRUE;
}

BOOL CTrigger::StartTrigger(BOOL bIsDir, int CamNo, int nStartPos, int nEndPos, int nPeriod)
{
	CString szStartPos, szEndPos, szPeriod;
	szPeriod.Format(_T("%d"), nPeriod);
	szStartPos.Format(_T("%d"), nStartPos);
	szEndPos.Format(_T("%d"), nEndPos );

	StopTriggerGen0();
	StopTriggerGen1();

	if(bIsDir == 1)
	{
		//pSys->CommLogFile(&CString("StartTrigger 1"), LOG_LEVEL_DETAIL);
		TriggerOrigin();
		
		if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
			TriggerInit0();
		if(pSys->m_Config.m_nInspactEqpNo == 1){
			TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
			TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);
		}else{
			if(CamNo == 0) TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
			else TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);
		}	

		StartForwardScan();
	}
	else
	{
		//pSys->CommLogFile(&CString("StartTrigger 0"), LOG_LEVEL_DETAIL); 
		if (nStartPos - nPeriod * 1000 < 0)
			nStartPos = 0;

		if(CamNo == 0) TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
		else TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);

		StartBackwardScan();
	}

	return TRUE;
}

BOOL CTrigger::GetTriggerDir( void )
{
	return m_bTrigerDirForword;
}



//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonTriggerDelayCommand
//	 2. �Լ���� :	Baudrate�� ���� Ʈ���� Ŀ�ǵ尣 �����̸� ��� �Լ�(��� ����)
//	 3. ��    �� :	����	
//	 4. �� ȯ �� :	�����̰�
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
UINT CTrigger::TriggerDelayCommand()
{
	UINT uiDelay = 0;

	// 2015.12.14 Comment by CWH for Packing - Delay ����
	/*if(pSys->m_Config.m_nTriggerBaud < 1)
	{
	uiDelay = 150;
	}
	else if(pSys->m_Config.m_nTriggerBaud == 2)
	{
	uiDelay = 75;
	}
	else if(pSys->m_Config.m_nTriggerBaud == 3)
	{
	uiDelay = 38;
	}
	else if(pSys->m_Config.m_nTriggerBaud == 4)
	{
	uiDelay = 25;
	}
	else if(pSys->m_Config.m_nTriggerBaud == 5)
	{
	uiDelay = 19;
	}
	else if(pSys->m_Config.m_nTriggerBaud >= 6 && pSys->m_Config.m_nTriggerBaud <= 7)
	{
	uiDelay = 10;
	}
	else if(pSys->m_Config.m_nTriggerBaud >= 8 && pSys->m_Config.m_nTriggerBaud <= 9)
	{
	uiDelay = 7;
	}
	else
	{
	uiDelay = 1;
	}*/
	
	if(pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_YOUNGWOO)
	{
		uiDelay = 0;	// 0,1�� ī�޶� ���� �ϳ��� Ʈ���Ű� ������ ������ ���� 0 -> 20���� ������.
	}

	return uiDelay;
}
//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	TriggerTimeout
//	 2. �Լ���� :	
//	 3. ��    �� :	
//	 4. �� ȯ �� :	
//	 5. �� �� �� :	2016/01/04
//	 6. �� �� �� :	mby
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
BOOL CTrigger::TriggerTimer(int time)
{
	BOOL bRet = false;
	int TimeoutCnt = 0;

	while (1)
	{	
		if (TimeoutCnt > time) {
			pSys->CommLogFile(&CString("TriggerTimer out"), LOG_LEVEL_DETAIL);
			m_bChsetCheck = FALSE;
			return 0;
			break;
		}else if(m_bChsetCheck == TRUE){	//2016.01.19 add by mby 
			m_bChsetCheck = FALSE;
			break;
		}
		Sleep(1);
		TimeoutCnt++;
	}

	return bRet;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	TriggerPositionChannelSet
//	 2. �Լ���� :	
//	 3. ��    �� :	
//	 4. �� ȯ �� :	
//	 5. �� �� �� :	2016/01/10
//	 6. �� �� �� :	mby
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::TriggerPositionChannelSet()
{
	m_bFirst = true;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSelectPitchMode
//	 2. �Լ���� :	������ ���ڴ��� Pitch ���� ����
//	 3. ��    �� :	���ڴ�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSelectPitchMode(UINT uiSelectEncoder)
{
	int nBufSize = 0;
	char szBuf[1024] = {0,};
	// ���ڴ� 0�� ��� 1�� �����Ѵ�.
	nBufSize = sprintf_s(szBuf, 1024, "/tm%d1/", uiSelectEncoder);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetUpperEncoderStart
//	 2. �Լ���� :	��� ���ڴ��� �������� ����
//	 3. ��    �� :	������ġ, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetUpperEncoderStart(double dStartPos, double dResolution)
{
	UINT uiTrgStart;
	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// ���� ���� �� ���ش� ����(���ڴ� 0)
	if(dStartPos<0.)
	{
		uiTrgStart = 0;
	}
	else
	{
		uiTrgStart = (UINT)(dStartPos*1000./dResolution+0.5);
	}							
	nBufSize = sprintf_s(szBuf, 1024, "/1b%d%08x/", 0, uiTrgStart+2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetDownEncoderStart
//	 2. �Լ���� :	�Ϻ� ���ڴ��� �������� ����
//	 3. ��    �� :	������ġ, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetDownEncoderStart(double dStartPos, double dResolution)
{
	UINT uiTrgStart;
	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// ���� ���� �� ���ش� ����(���ڴ� 1)
	if(dStartPos<0.)
	{
		uiTrgStart = 0;
	}
	else
	{
		uiTrgStart = (UINT)(dStartPos*1000./dResolution+0.5);
	}							
	nBufSize = sprintf_s(szBuf, 1024, "/1b%d%08x/", 1, uiTrgStart+2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetUpperEncoderEnd
//	 2. �Լ���� :	��� ���ڴ��� �������� ����
//	 3. ��    �� :	������ġ, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetUpperEncoderEnd(double dEndPos, double dResolution)
{
	UINT uiTrgEnd;
	int  nBufSize	 = 0;
	char szBuf[1024] = {0,};							

	// ���� ���� �� ���ش� ����(���ڴ� 0)
	if(dEndPos<0.)
	{
		uiTrgEnd   = 0;
	}
	else
	{
		uiTrgEnd   = (UINT)(dEndPos*1000./dResolution+0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1e%d%08x/", 0, uiTrgEnd+2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetDownEncoderEnd
//	 2. �Լ���� :	�Ϻ� ���ڴ��� �������� ����
//	 3. ��    �� :	������ġ, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetDownEncoderEnd(double dEndPos, double dResolution)
{
	UINT uiTrgEnd;
	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// ���� ���� �� ���ش� ����(���ڴ� 1)
	if(dEndPos<0.)
	{
		uiTrgEnd   = 0;
	}
	else
	{
		uiTrgEnd   = (UINT)(dEndPos*1000./dResolution+0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1e%d%08x/", 1, uiTrgEnd+2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetUpperEncoderPeriod
//	 2. �Լ���� :	��� ���ڴ��� �ֱ�����
//	 3. ��    �� :	�ֱ�, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetUpperEncoderPeriod(double dPeriod, double dResolution)
{
	UINT uiTrgPitch = 0;
	int nBufSize = 0;
	char szBuf[1024] = {0,};	

	// �ֱ�����(���ڴ� 0)
	if(dPeriod < 0.)
	{
		uiTrgPitch = 0;
	}
	else
	{
		uiTrgPitch = (UINT)(dPeriod/dResolution+0.05);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1p%d%08x/", 0, uiTrgPitch);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetDownEncoderPeriod
//	 2. �Լ���� :	�Ϻ� ���ڴ��� �ֱ�����
//	 3. ��    �� :	�ֱ�, ���ش�
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetDownEncoderPeriod(double dPeriod, double dResolution)
{
	UINT uiTrgPitch = 0;

	int nBufSize = 0;
	char szBuf[1024] = {0,};	

	// �ֱ�����(���ڴ� 1)
	if( dPeriod < 0.)
	{
		uiTrgPitch = 0;
	}
	else
	{
		uiTrgPitch = (UINT)(dPeriod/dResolution+0.05);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1p%d%08x/", 1, uiTrgPitch);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetUpperEncoderPulseWidth
//	 2. �Լ���� :	��� ���ڴ��� �޽�������
//	 3. ��    �� :	�޽���
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetUpperEncoderPulseWidth(double dPulseWidth)
{
	UINT uiPulseWidth = 0;

	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// �޽��� ����(���ڴ� 0)
	if(dPulseWidth < 0.)
	{
		uiPulseWidth = 0;
	}
	else
	{
		uiPulseWidth = (UINT)(dPulseWidth*100);//.*0.005);	// comppulsewidth
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1w%d%08x/", 0, uiPulseWidth);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetDownEncoderPulseWidth
//	 2. �Լ���� :	�Ϻ� ���ڴ��� �޽�������
//	 3. ��    �� :	�޽���
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetDownEncoderPulseWidth(double dPulseWidth)
{
	UINT uiPulseWidth = 0;

	int nBufSize = 0;
	char szBuf[1024] = {0,};							

	// �޽��� ����(���ڴ� 1)
	if(dPulseWidth < 0.)
	{
		uiPulseWidth = 0;
	}
	else
	{
		uiPulseWidth = (UINT)(dPulseWidth*100);//.*0.005);	// comppulsewidth
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1w%d%08x/", 1, uiPulseWidth);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetUpperEncoderDelay
//	 2. �Լ���� :	��� ���ڴ��� ������ ����
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetUpperEncoderDelay()
{
	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// ������ ����(���ڴ� 0)
	nBufSize = sprintf_s(szBuf, 1024, "/1l%d%08x/", 0, 0);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonSetupDownEncoderDelay
//	 2. �Լ���� :	��� ���ڴ��� ������ ����
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonSetDownEncoderDelay()
{
	int nBufSize = 0;
	char szBuf[1024] = {0,};

	// ������ ����(���ڴ� 1)
	nBufSize = sprintf_s(szBuf, 1024, "/1l%d%08x/", 1, 0);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonInitEncoder
//	 2. �Լ���� :	���ڴ� �ʱ�ȭ
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonInitEncoder()
{
	int nBufSize = 0;
	char szBuf[1024] = {0,};
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/re/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonInitTrigger
//	 2. �Լ���� :	Ʈ���� �ʱ�ȭ
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonInitTrigger()
{
	int nBufSize = 0;
	char szBuf[1024] = {0,};
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/gid5/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonOperatingOff
//	 2. �Լ���� :	���ڴ� ���� �ʱ�ȭ�� ���� Off����
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonOperatingOff()
{
	int nBufSize	 = 0;	// 2015.05.18 ljk
	char szBuf[1024] = {0,};

	nBufSize = sprintf_s(szBuf, 1024, "%s", "/of/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonOperatingOn
//	 2. �Լ���� :	���ڴ� ���� �ʱ�ȭ�� ���� On����
//	 3. ��    �� :	����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonOperatingOn()
{
	int nBufSize	 = 0;	// 2015.05.18 ljk
	char szBuf[1024] = {0,};
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/on/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonUpperDirection
//	 2. �Լ���� :	���ڴ� 0 ���� ����
//	 3. ��    �� :	���ڴ� ����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonUpperDirection(int nDirection)
{
	int nBufSize	 = 0;
	char szBuf[1024] = {0,};

	nBufSize = sprintf_s(szBuf, 1024, "/1i%d/", nDirection);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	DaekonDownDirection
//	 2. �Լ���� :	���ڴ� 1 ���� ����
//	 3. ��    �� :	���ڴ� ����
//	 4. �� ȯ �� :	����
//	 5. �� �� �� :	2015/05/19
//	 6. �� �� �� :	ljk
//	 7. ������� :
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
void CTrigger::DaekonDownDirection(int nDirection)
{
	int nBufSize	 = 0;
	char szBuf[1024] = {0,};

	nBufSize = sprintf_s(szBuf, 1024, "/1i%d/", nDirection);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}