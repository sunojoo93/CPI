// NLight.cpp: implementation of the CLight class.
// 닙스 조명 컨트롤용
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TriggerControl.h"
//#include "include.h"
//#include "extern.h"
#include "VSCameraTask.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CTriggerControl::CTriggerControl()
{
	m_bFirst = true;
	m_bTriFailFlag = FALSE;
	m_bChsetCheck = FALSE;	//2016.01.19 add by mby 
}
CTriggerControl::~CTriggerControl()
{
	m_CurrentMsg = _T("");
}


BOOL	CTriggerControl::Send_Trigger(LPCTSTR szFormatString, ...)
{
	BOOL bRet;
	va_list	args;

	//size_t i, j, pos;  
	size_t i;
	char Lens[10];

	va_start(args, szFormatString);

	int len = _vscprintf(CT2A(szFormatString), args);

	unsigned char *buffer = (unsigned char*)malloc((len + 5) * sizeof(unsigned char));
	unsigned char sum = 0;

	sprintf((char *)&Lens, "%02d", len);
	buffer[0] = PID_SET_STX;
	sprintf((char *)&buffer[1], "%c", Lens[0]);
	sprintf((char *)&buffer[2], "%c", Lens[1]);

	int k = 0;
	for (i = 0; i < len; i++)
		sprintf((char *)&buffer[3 + i], "%c", szFormatString[i]);

	sum = CalcChecksum((unsigned char*)szFormatString, len * 2);

	buffer[3 + len] = sum;
	buffer[4 + len] = PID_SET_ETX;

	bRet = WriteComm((UCHAR *)buffer, len + 5);

	//보낸 메시지에 대해 리턴을 받을때까지 대기한다. add by chlee 2016.02.24
	m_bReturned = false;
	//while(!m_bReturned) Sleep(1);

	free(buffer);
	return bRet;
}

unsigned char  CTriggerControl::CalcChecksum(unsigned char *data, int leng)
{
	unsigned char csum = 0;
	for (; leng > 0; leng--)
		csum += *data++;

	return csum = csum % 256;
}

BOOL CTriggerControl::ConnectTrigger(int nPortNumber, int nBaudRate)
{
	CString		strPortName;
	CString		strPanelId;
	DWORD		dwThreadID;

	if (this->m_Connected)		return TRUE;

	//2016.11.08 add by mby 
	//strPanelId = pSys->GetPanelID();
	strPanelId = _T("TEST_LCP");
	if (strPanelId == "") {
		strPanelId = "PANELID";
	}

	strPortName.Format(_T("COM%d"), nPortNumber);
	if (this->OpenPort(strPortName, DWORD(nBaudRate), nPortNumber - 1))
	{
		this->m_Connected = TRUE;

		//2016.11.08 add by mby 
		//Message PanelID,EventID,EventInfo		
		//strPortName.Format(_T("EVENT:ConnectTrigger PortNum(%d) %s,%d"), nPortNumber, strPanelId, EVT_CONNECT_TRIGGER);
		//pSys->EventLogFile(&strPortName, LOG_LEVEL_BASIC);
		if (theApp.m_pInternalServer != NULL)
			theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:ConnectTrigger PortNum(%d) %s,%d"), nPortNumber, strPanelId, EVT_CONNECT_TRIGGER);

		//	수신 포트 감시 스레드 시작 ...
		m_hThreadWatchComm = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadWatchTrigger, this, 0, &dwThreadID);
		if (!m_hThreadWatchComm)	// 2014.12.20 ljk
		{
			ClosePort();
			return FALSE;
		}
	}
	else {
		//Message PanelID,ErrorID,ErrorLevel,ErrorInfo		
		//strPortName.Format(_T("%s,%d,%d,ConnectTrigger: OpenPort Fail"), strPanelId, TRI_CONNECT_FAIL, 3);
		//pSys->AlarmLogFile(&strPortName, LOG_LEVEL_BASIC);
		if (theApp.m_pInternalServer != NULL)
			theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("%s,%d,%d,ConnectTrigger: OpenPort Fail"), strPanelId, TRI_CONNECT_FAIL, 3);
		AfxMessageBox(_T("Trigger Comport disconnection!! Please Check The Cable or Borad "));
		return FALSE;
	}

	return TRUE;
}


UINT	ThreadWatchTrigger(CTriggerControl* pTrigger)
{
	BYTE		Buff;
	DWORD		dwEvent;
	BOOL		bOk = TRUE;
	int			total = 0;
	int			nRetry = 0;
	CString		returnMsg;
	CString		logMsg;
	CString		strPanelId;

	while (pTrigger->m_Connected) {
		dwEvent = 0;  //	이벤트 클리어 
		WaitCommEvent(pTrigger->m_hComm, &dwEvent, NULL);  // 포트에 읽을 거리가 올때까지 기다린다.

		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)  // 포트에 이벤트가 발생하면 
		{
			// 512번 읽어도  데이터의 마지막이 아니면 다음 이벤트를 기다린다.  
			for (int i = 0; i<512; i++)
			{
				int ret = pTrigger->ReadComm(&Buff, 1);  // 먼저 하나를 읽는다.
				if (ret > 0)  // 길이를 체크하고
				{
					// 길이가 0보다 크면 시리얼 클래스에 마련된 버퍼에 읽은 값 하나를 넣는다. 
					pTrigger->m_InBuf[total] = Buff;
					total++;
					// 프로토콜상에서 데이터 마지막인지를 검사한다. 
					if (Buff == 0x03)
					{
						pTrigger->m_InBuf[total] = '\0';  // 데이터 마지막이면 버퍼의 마지막에 공백 문자를 넣고
						CString tmpStr(pTrigger->m_InBuf);
						returnMsg = tmpStr;

						memset(pTrigger->m_InBuf, 0, 512);
						total = 0;
						break;
					}
				}
			}

			logMsg = _T("[Trigger]Recv Msg : ") + returnMsg;
			//pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);
			if (theApp.m_pInternalServer != NULL)
				theApp.m_pInternalServer->m_fnGlobalPrintLog(logMsg);

			if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
			{
				if (returnMsg.Find(_T("error")) != -1) {
					if (nRetry > 2) { //세번 실패 시 변수 초기화. retry fail 후 대응은 아직 구현되지 않음
						nRetry = 0;
						pTrigger->m_bReturned = true;
						pTrigger->m_Connected = FALSE;	//Trigger 통신문제 상태
						//strPanelId = pSys->GetPanelID();
						strPanelId = _T("TEST_LCP");
						if (strPanelId == "") {
							strPanelId = _T("PANELID");
						}
						//Message PanelID,ErrorID,ErrorLevel,ErrorInfo
						//logMsg.Format(_T("%s,%d,%d,ThreadWatchTrigger: Command Fail"), strPanelId, TRI_COMMAND_FAIL, 3);
						//pSys->AlarmLogFile(&logMsg, LOG_LEVEL_BASIC);
						if (theApp.m_pInternalServer != NULL)
							theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("%s,%d,%d,ThreadWatchTrigger: Command Fail"), strPanelId, TRI_COMMAND_FAIL, 3);
					}
					else {
						logMsg = _T("[Trigger]Retry : ") + pTrigger->m_CurrentMsg;
						//pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);
						if (theApp.m_pInternalServer != NULL)
							theApp.m_pInternalServer->m_fnGlobalPrintLog(logMsg);
						pTrigger->Send_Trigger(pTrigger->m_CurrentMsg);
						nRetry++;
					}
				}
				else {
					nRetry = 0;
					pTrigger->m_bReturned = true;
				}
			}
		}
	}
	// 포트가 ClosePort에 의해 닫히면 m_bConnected 가 FALSE가 되어 종료.
	pTrigger->m_hThreadWatchComm = NULL;

	return TRUE;
}

//메시지 응답이 없을 경우 0.3초를 대기한다. 재시도는 2회
void    CTriggerControl::WaitRecvMsg() {
	int nLimit = 300;
	int nCount = 0;
	int nRetry = 0;

	while ((!m_bReturned)) {
		Sleep(1);
		nCount++;
		if (nCount >= nLimit) { //timeout retry
			if (nRetry >= 2) {
				break;
			}

			nCount = 0;
			Send_Trigger(m_CurrentMsg);
			CString logMsg = _T("[Trigger]WaitRecvMsg Retry : ") + m_CurrentMsg;
			//pSys->CommLogFile(&logMsg, LOG_LEVEL_BASIC);
			if (theApp.m_pInternalServer != NULL)
				theApp.m_pInternalServer->m_fnGlobalPrintLog(logMsg);
			nRetry++;
		}
	}
}

void	CTriggerControl::SetScanRange(int start, int end, int interval)
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("ENCW,%d,%d,%d"), start, end, interval);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::SetTriggerDir(BOOL bForward)
{
	m_bTrigerDirForword = bForward;

	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("ENCW,0,0,0,%d"), int((bForward) ? 128 : 255));
		Send_Trigger(m_CurrentMsg);
		//CString msg;
		//msg.Format(_T("[Trigger]SetTriggerDir : %d"), bForward);
		//pSys->CommLogFile(&msg, LOG_LEVEL_BASIC);
		if (theApp.m_pInternalServer != NULL)
			theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("[Trigger]SetTriggerDir : %d"), bForward);

		//2016.11.08 add by mby 
		//Message PanelID,EventID,EventInfo
		//msg.Format(_T("EVENT:SetTriggerDir Direction(%d) ,%s,%d"), bForward, pSys->GetPanelID(), EVT_SET_TRIGGER_DIR);
		//pSys->EventLogFile(&msg, LOG_LEVEL_BASIC);
		if (theApp.m_pInternalServer != NULL)
			theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:SetTriggerDir Direction(%d) ,%s,%d"), bForward, _T("TEST_LCP"), EVT_SET_TRIGGER_DIR);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTriggerControl::GetCurTrgCount()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,5");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::GetCurStart()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,10");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::GetCurEnd()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,11");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::GetCurInterval()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,12");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::GetEncErrCount()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,13");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}
void	CTriggerControl::GetCurEnc()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("ENCR,14");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTriggerControl::StopTriggerGen0()	 //트리거 신호 발생정지			 
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,0,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonOperatingOff();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTriggerControl::StopTriggerGen1()	 //트리거 신호 발생정지		 
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonOperatingOff();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTriggerControl::StartForwardScan()  // 순방향 스켄
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	//CString traceMsg;
	//traceMsg.Format(_T("EVENT:StartForwardScan() ,%s,%d"), pSys->GetPanelID(), EVT_FORWARD_SCAN);
	//pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:StartForwardScan() ,%s,%d"), _T("TEST_LCP"), EVT_FORWARD_SCAN);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,2,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonDownDirection(3);		// 0b01 = 상부 스테이지 순방향, 0b10 = 하부 스테이지 순방향, 비트 조합으로 파라메터운용됨
		Sleep(TriggerDelayCommand());
		DaekonOperatingOn();
	}
	//Sleep(TriggerDelayCommand());
}

void	CTriggerControl::StartBackwardScan()  // 역방향 스켄
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	//CString traceMsg;
	//traceMsg.Format(_T("EVENT:StartBackwardScan() ,%s,%d"), pSys->GetPanelID(), EVT_BACKWARD_SCAN);
	//pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:StartBackwardScan() ,%s,%d"), _T("TEST_LCP"), EVT_BACKWARD_SCAN);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,0,3,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonDownDirection(0);		// 0b01 = 상부 스테이지 순방향, 0b10 = 하부 스테이지 순방향, 비트 조합으로 파라메터운용됨
		Sleep(TriggerDelayCommand());
		DaekonOperatingOn();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerInit0()  // 트리거 0 초기화
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,3,1,1,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonInitTrigger();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerInit1()  // 트리거 1 초기화
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,4,1,1,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonInitTrigger();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerOrigin0()  // 원점 설정
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,128,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonInitEncoder();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerOrigin1()  // 원점 설정
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,255,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonInitEncoder();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerStartPosition8K()  // 0번 발생주기, (AOI, 트리거번호, 시작위치, 끝위치, 주기)
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,1,50000,800000,26,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
}

void    CTriggerControl::TriggerStartPosition2K()  // 1번 발생주기, (AOI, 트리거번호, 시작위치, 끝위치, 주기)
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,2,50000,800000,42,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerCurrentPosition()  // 현재 위치값 읽기
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,15,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerGenCount0()  // 트리거 0 발생 카운터 갯수 읽기
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,20,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
}

void    CTriggerControl::TriggerGenCount1()  // 트리거 1 발생 카운터 갯수 읽기
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,21,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerDebug()
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,80,1,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerStartPositionGet_00()  // 0번 발생주기, (AOI, 트리거번호, 시작위치, 끝위치, 주기)
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,11,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void    CTriggerControl::TriggerStartPositionGet_01()  // 1번 발생주기, (AOI, 트리거번호, 시작위치, 끝위치, 주기) 
{
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg = _T("AOI,12,");
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	//Sleep(TriggerDelayCommand());
}

void CTriggerControl::TriggerStartPositionChannel0(int startPos, int stopPos, int period)
{
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:TriggerStartPositionChannel0() ,%s,%d"), _T("TEST_LCP"), EVT_TRIGGER_CHANNEL0);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,1,%d,%d,%d,"), startPos, stopPos, period);
		Send_Trigger(m_CurrentMsg);
		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(0);
		Sleep(TriggerDelayCommand());
		// 엔코더 0의 시작지점을 설정한다.
		DaekonSetUpperEncoderStart(startPos);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderEnd(stopPos);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPeriod(period);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderDelay();
	}
}
void CTriggerControl::TriggerStartPositionChannel1(int startPos, int stopPos, int period)
{
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:TriggerStartPositionChannel1() ,%s,%d"), _T("TEST_LCP"), EVT_TRIGGER_CHANNEL1);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,2,%d,%d,%d,"), startPos, stopPos, period);
		Send_Trigger(m_CurrentMsg);
		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(1);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderStart(startPos);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderEnd(stopPos);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPeriod(period);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderDelay();
	}
}

//추가
void CTriggerControl::TriggerStartPositionChannel0(CString strStartPos, CString strEndPos, CString strPeriod)
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	//CString traceMsg;
	//traceMsg.Format(_T("EVENT:TriggerStartPositionChannel0() ,%s,%d"), pSys->GetPanelID(), EVT_TRIGGER_CHANNEL0);
	//pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:TriggerStartPositionChannel0() ,%s,%d"), _T("TEST_LCP"), EVT_TRIGGER_CHANNEL0);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,1,%s,%s,%s,"), strStartPos, strEndPos, strPeriod);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(0);
		Sleep(TriggerDelayCommand());
		// 엔코더 0의 시작지점을 설정한다.
		DaekonSetUpperEncoderStart(_wtof(strStartPos));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderEnd(_wtof(strEndPos));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPeriod(_wtof(strPeriod));
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetUpperEncoderDelay();
	}
	//Sleep(TriggerDelayCommand());
}

void CTriggerControl::TriggerStartPositionChannel1(CString strStartPos, CString strEndPos, CString strPeriod)
{
	//2016.11.08 add by mby 
	//Message PanelID,EventID,EventInfo
	//CString traceMsg;
	//traceMsg.Format(_T("EVENT:TriggerStartPositionChannel1() ,%s,%d"), pSys->GetPanelID(), EVT_TRIGGER_CHANNEL1);
	//pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);
	if (theApp.m_pInternalServer != NULL)
		theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("EVENT:TriggerStartPositionChannel1() ,%s,%d"), _T("TEST_LCP"), EVT_TRIGGER_CHANNEL1);
	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		m_CurrentMsg.Format(_T("AOI,2,%s,%s,%s,"), strStartPos, strEndPos, strPeriod);
		Send_Trigger(m_CurrentMsg);

		WaitRecvMsg();
	}
	else if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	{
		DaekonSelectPitchMode(1);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderStart(_wtof(strStartPos));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderEnd(_wtof(strEndPos));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPeriod(_wtof(strPeriod));
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderPulseWidth(5);
		Sleep(TriggerDelayCommand());
		DaekonSetDownEncoderDelay();
	}
	//Sleep(TriggerDelayCommand());
}
BOOL CTriggerControl::StartTrigger(bool bState)
{
	return FALSE;

	//CSystem* pSys = CSystem::GetInstance();

	//if (bState)
	//{
	//	//2016.11.08 add by mby 
	//	//Message PanelID,EventID,EventInfo
	//	CString traceMsg;
	//	traceMsg.Format(_T("EVENT:StartTrigger() ,%s,%d"), pSys->GetPanelID(), EVT_START_TRIGGER);
	//	pSys->EventLogFile(&traceMsg, LOG_LEVEL_BASIC);

	//	//ABI는 처음 Grab 요청 시 한번만 설정을 하도록 한다. 2015.12.08 add by chlee
	//	if (!m_bFirst									&&
	//		pSys->m_Config.m_nModelType == EQP_TYPE_ABI	&&		// ABI 인 경우
	//		pSys->m_Config.m_nEqpType != EQP_TYPE_BIG	&&		// 대형이 아닌 경우 패스.
	//		pSys->m_Config.m_nEqpType != EQP_TYPE_MONITOR &&	// 모니터가 아닌 경우 패스
	//		pSys->m_Config.m_nMonitorLine != TRUE)				// 모니터라인이 아닌경우
	//	{
	//		StartForwardScan();
	//		return TRUE;
	//	}

	//	if (m_bTrigerDirForword)
	//	{
	//		if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	//		{
	//			StopTriggerGen0();
	//			StopTriggerGen1();
	//		}

	//		TriggerOrigin();
	//		if (pSys->m_Config.m_nMonitorLine)
	//			StopTriggerGen0();

	//		if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	//			TriggerInit0();
	//		//2016.04.18 add by mby 모니터라인구분
	//		if (pSys->m_Config.m_nMonitorLine) {
	//			if (pSys->m_Config.m_nInspactEqpNo == 1 || pSys->m_Config.m_nInspactEqpNo == 7) {
	//				TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
	//			}
	//			else if (pSys->m_Config.m_nInspactEqpNo == 3 || pSys->m_Config.m_nInspactEqpNo == 8)
	//			{
	//				TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos, pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
	//			}
	//		}
	//		else {
	//			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
	//			TriggerStartPositionChannel1(pSys->m_Config.stCam01_Forward.sStartPos, pSys->m_Config.stCam01_Forward.sEndPos, pSys->m_Config.stCam01_Forward.sPeriod);
	//		}
	//		StartForwardScan();
	//	}
	//	else
	//	{
	//		if (pSys->m_Config.m_nMonitorLine)
	//		{
	//			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	//			{
	//				StopTriggerGen0();
	//			}

	//			if (pSys->m_Config.m_nMonitorLine)
	//			{
	//				TriggerOrigin();
	//				StopTriggerGen0();
	//			}

	//			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos, pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);

	//			StartForwardScan();
	//		}
	//		else
	//		{
	//			if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	//			{
	//				StopTriggerGen0();
	//				StopTriggerGen1();
	//			}

	//			TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos, pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
	//			//pSys->CommLogFile(&CString("Backword TriggerStartPositionChannel1"), LOG_LEVEL_DETAIL);		
	//			TriggerStartPositionChannel1(pSys->m_Config.stCam01_Backward.sStartPos, pSys->m_Config.stCam01_Backward.sEndPos, pSys->m_Config.stCam01_Backward.sPeriod);

	//			StartBackwardScan();
	//		}
	//	}

	//	if (m_bFirst) m_bFirst = false; //2015.12.08 add by chlee
	//}
	//else
	//{

	//	if (pSys->m_Config.m_nTriggerBoardType == BOARD_TYPE_DAEKON)
	//	{
	//		StopTriggerGen0();
	//		// 2015.01.07 Comment by CWH - 불필요 구문
	//		//TriggerGenCount0();			
	//		//TriggerGenCount1();
	//		//TriggerCurrentPosition();		
	//	}
	//	else {
	//		TriggerGenCount0();
	//		TriggerGenCount1();
	//		TriggerCurrentPosition();
	//	}
	//}

	//return TRUE;
}
// TriggerInitSet //2016.08.19 add by mby for Tact time(BIG,MONITOR) S-
BOOL CTriggerControl::TriggerInitSet(bool bState)
{
	//CSystem* pSys = CSystem::GetInstance();

	//if (bState)
	//{
	//	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	//	{
	//		StopTriggerGen0();
	//		TriggerInit0();
	//	}

	//	TriggerOrigin();
	//	TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);
	//}
	//else
	//{
	//	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
	//	{
	//		StopTriggerGen0();
	//	}
	//	TriggerOrigin();
	//	TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos, pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);
	//}

	return TRUE;
}
// TriggerInitSet //2016.08.19 add by mby for Tact time(BIG,MONITOR) E-
BOOL CTriggerControl::StartTrigger(bool bState, BOOL bIsDir)
{

	//if (bState)
	//{
	//	if (bIsDir)
	//	{
	//		StopTriggerGen0();

	//		StopTriggerGen1();

	//		TriggerOrigin();

	//		TriggerInit0();

	//		//Forward Cam1 StartPos, EndPos, Period
	//		TriggerStartPositionChannel0(pSys->m_Config.stCam00_Forward.sStartPos, pSys->m_Config.stCam00_Forward.sEndPos, pSys->m_Config.stCam00_Forward.sPeriod);

	//		//Forward Cam2 StartPos, EndPos, Period
	//		TriggerStartPositionChannel1(pSys->m_Config.stCam01_Forward.sStartPos, pSys->m_Config.stCam01_Forward.sEndPos, pSys->m_Config.stCam01_Forward.sPeriod);

	//		StartForwardScan();

	//		TriggerGenCount0();
	//		TriggerGenCount1();
	//	}
	//	else
	//	{
	//		StopTriggerGen0();

	//		StopTriggerGen1();

	//		//TriggerOrigin();
	//		//
	//		//Backward Cam1 StartPos, EndPos, Period
	//		TriggerStartPositionChannel0(pSys->m_Config.stCam00_Backward.sStartPos, pSys->m_Config.stCam00_Backward.sEndPos, pSys->m_Config.stCam00_Backward.sPeriod);

	//		//Backward Cam2 StartPos, EndPos, Period
	//		TriggerStartPositionChannel1(pSys->m_Config.stCam01_Backward.sStartPos, pSys->m_Config.stCam01_Backward.sEndPos, pSys->m_Config.stCam01_Backward.sPeriod);

	//		StartBackwardScan();

	//		TriggerGenCount0();
	//		TriggerGenCount1();
	//	}
	//}
	//else
	//{
	//	StopTriggerGen0();
	//	TriggerGenCount0();
	//	TriggerGenCount1();
	//	TriggerCurrentPosition();
	//}

	return TRUE;
}

BOOL CTriggerControl::StartTrigger(BOOL bIsDir, int CamNo, int nStartPos, int nEndPos, int nPeriod)
{
	CString szStartPos, szEndPos, szPeriod;
	szPeriod.Format(_T("%d"), nPeriod);
	szStartPos.Format(_T("%d"), nStartPos);
	szEndPos.Format(_T("%d"), nEndPos);

	StopTriggerGen0();
	StopTriggerGen1();

	if (bIsDir == 1)
	{
		//pSys->CommLogFile(&CString("StartTrigger 1"), LOG_LEVEL_DETAIL);
		TriggerOrigin0();

		if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_DAEKON)
			TriggerInit0();

		/*if (pSys->m_Config.m_nMonitorLine)
		{
			TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
			TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);
		}
		else*/
		{
			if (CamNo == 0)
				TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);

			else
				TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);
		}

		StartForwardScan();
	}
	else
	{
		//pSys->CommLogFile(&CString("StartTrigger 0"), LOG_LEVEL_DETAIL); 
		/*if (pSys->m_Config.m_nMonitorLine)
			TriggerOrigin();*/

		if (nStartPos - nPeriod * 1000 < 0)
			nStartPos = 0;

		/*if (pSys->m_Config.m_nMonitorLine)
		{
			TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
			TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);

			StartForwardScan();
		}
		else*/
		{
			if (CamNo == 0)
				TriggerStartPositionChannel0(szStartPos, szEndPos, szPeriod);
			else
				TriggerStartPositionChannel1(szStartPos, szEndPos, szPeriod);

			StartBackwardScan();
		}
	}

	return TRUE;
}

BOOL CTriggerControl::GetTriggerDir(void)
{
	return m_bTrigerDirForword;
}



//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonTriggerDelayCommand
//	 2. 함수기능 :	Baudrate에 따른 트리거 커맨드간 딜레이를 얻는 함수(대곤 보드)
//	 3. 인    수 :	없음	
//	 4. 반 환 값 :	딜레이값
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
UINT CTriggerControl::TriggerDelayCommand()
{
	UINT uiDelay = 0;

	// 2015.12.14 Comment by CWH for Packing - Delay 삭제
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

	if (/*pSys->m_Config.m_nTriggerBoardType*/0 == BOARD_TYPE_YOUNGWOO)
	{
		uiDelay = 0;	// 0,1번 카메라가 둘중 하나만 트리거가 나오는 문제로 인해 0 -> 20으로 수정함.
	}

	return uiDelay;
}
//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	TriggerTimeout
//	 2. 함수기능 :	
//	 3. 인    수 :	
//	 4. 반 환 값 :	
//	 5. 작 성 일 :	2016/01/04
//	 6. 작 성 자 :	mby
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
BOOL CTriggerControl::TriggerTimer(int time)
{
	BOOL bRet = false;
	int TimeoutCnt = 0;

	while (1)
	{
		if (TimeoutCnt > time) {
			//pSys->CommLogFile(&CString("TriggerTimer out"), LOG_LEVEL_DETAIL);
			if (theApp.m_pInternalServer != NULL)
				theApp.m_pInternalServer->m_fnGlobalPrintLog(_T("TriggerTimer out"));
			m_bChsetCheck = FALSE;
			return 0;
			break;
		}
		else if (m_bChsetCheck == TRUE) {	//2016.01.19 add by mby 
			m_bChsetCheck = FALSE;
			break;
		}
		Sleep(1);
		TimeoutCnt++;
	}

	return bRet;
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	TriggerPositionChannelSet
//	 2. 함수기능 :	
//	 3. 인    수 :	
//	 4. 반 환 값 :	
//	 5. 작 성 일 :	2016/01/10
//	 6. 작 성 자 :	mby
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::TriggerPositionChannelSet()
{
	m_bFirst = true;
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSelectPitchMode
//	 2. 함수기능 :	선택한 엔코더를 Pitch 모드로 설정
//	 3. 인    수 :	엔코더
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSelectPitchMode(UINT uiSelectEncoder)
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };
	// 엔코더 0의 모드 1을 선택한다.
	nBufSize = sprintf_s(szBuf, 1024, "/tm%d1/", uiSelectEncoder);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetUpperEncoderStart
//	 2. 함수기능 :	상부 엔코더의 시작지점 설정
//	 3. 인    수 :	시작위치, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetUpperEncoderStart(double dStartPos, double dResolution)
{
	UINT uiTrgStart;
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 시작 지점 및 분해능 적용(엔코더 0)
	if (dStartPos<0.)
	{
		uiTrgStart = 0;
	}
	else
	{
		uiTrgStart = (UINT)(dStartPos*1000. / dResolution + 0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1b%d%08x/", 0, uiTrgStart + 2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetDownEncoderStart
//	 2. 함수기능 :	하부 엔코더의 시작지점 설정
//	 3. 인    수 :	시작위치, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetDownEncoderStart(double dStartPos, double dResolution)
{
	UINT uiTrgStart;
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 시작 지점 및 분해능 적용(엔코더 1)
	if (dStartPos<0.)
	{
		uiTrgStart = 0;
	}
	else
	{
		uiTrgStart = (UINT)(dStartPos*1000. / dResolution + 0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1b%d%08x/", 1, uiTrgStart + 2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetUpperEncoderEnd
//	 2. 함수기능 :	상부 엔코더의 종료지점 설정
//	 3. 인    수 :	종료위치, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetUpperEncoderEnd(double dEndPos, double dResolution)
{
	UINT uiTrgEnd;
	int  nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 종료 지점 및 분해능 적용(엔코더 0)
	if (dEndPos<0.)
	{
		uiTrgEnd = 0;
	}
	else
	{
		uiTrgEnd = (UINT)(dEndPos*1000. / dResolution + 0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1e%d%08x/", 0, uiTrgEnd + 2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetDownEncoderEnd
//	 2. 함수기능 :	하부 엔코더의 종료지점 설정
//	 3. 인    수 :	종료위치, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetDownEncoderEnd(double dEndPos, double dResolution)
{
	UINT uiTrgEnd;
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 종료 지점 및 분해능 적용(엔코더 1)
	if (dEndPos<0.)
	{
		uiTrgEnd = 0;
	}
	else
	{
		uiTrgEnd = (UINT)(dEndPos*1000. / dResolution + 0.5);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1e%d%08x/", 1, uiTrgEnd + 2147483648);	// Enc_basepos = 2147483648
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetUpperEncoderPeriod
//	 2. 함수기능 :	상부 엔코더에 주기적용
//	 3. 인    수 :	주기, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetUpperEncoderPeriod(double dPeriod, double dResolution)
{
	UINT uiTrgPitch = 0;
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 주기적용(엔코더 0)
	if (dPeriod < 0.)
	{
		uiTrgPitch = 0;
	}
	else
	{
		uiTrgPitch = (UINT)(dPeriod / dResolution + 0.05);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1p%d%08x/", 0, uiTrgPitch);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetDownEncoderPeriod
//	 2. 함수기능 :	하부 엔코더에 주기적용
//	 3. 인    수 :	주기, 분해능
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetDownEncoderPeriod(double dPeriod, double dResolution)
{
	UINT uiTrgPitch = 0;

	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 주기적용(엔코더 1)
	if (dPeriod < 0.)
	{
		uiTrgPitch = 0;
	}
	else
	{
		uiTrgPitch = (UINT)(dPeriod / dResolution + 0.05);
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1p%d%08x/", 1, uiTrgPitch);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetUpperEncoderPulseWidth
//	 2. 함수기능 :	상부 엔코더에 펄스폭적용
//	 3. 인    수 :	펄스폭
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetUpperEncoderPulseWidth(double dPulseWidth)
{
	UINT uiPulseWidth = 0;

	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 펄스폭 적용(엔코더 0)
	if (dPulseWidth < 0.)
	{
		uiPulseWidth = 0;
	}
	else
	{
		uiPulseWidth = (UINT)(dPulseWidth * 100);//.*0.005);	// comppulsewidth
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1w%d%08x/", 0, uiPulseWidth);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetDownEncoderPulseWidth
//	 2. 함수기능 :	하부 엔코더에 펄스폭적용
//	 3. 인    수 :	펄스폭
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetDownEncoderPulseWidth(double dPulseWidth)
{
	UINT uiPulseWidth = 0;

	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 펄스폭 적용(엔코더 1)
	if (dPulseWidth < 0.)
	{
		uiPulseWidth = 0;
	}
	else
	{
		uiPulseWidth = (UINT)(dPulseWidth * 100);//.*0.005);	// comppulsewidth
	}
	nBufSize = sprintf_s(szBuf, 1024, "/1w%d%08x/", 1, uiPulseWidth);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetUpperEncoderDelay
//	 2. 함수기능 :	상부 엔코더에 딜레이 적용
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetUpperEncoderDelay()
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 딜레이 적용(엔코더 0)
	nBufSize = sprintf_s(szBuf, 1024, "/1l%d%08x/", 0, 0);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonSetupDownEncoderDelay
//	 2. 함수기능 :	상부 엔코더에 딜레이 적용
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonSetDownEncoderDelay()
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	// 딜레이 적용(엔코더 1)
	nBufSize = sprintf_s(szBuf, 1024, "/1l%d%08x/", 1, 0);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonInitEncoder
//	 2. 함수기능 :	엔코더 초기화
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonInitEncoder()
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/re/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonInitTrigger
//	 2. 함수기능 :	트리거 초기화
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonInitTrigger()
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/gid5/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonOperatingOff
//	 2. 함수기능 :	엔코더 지점 초기화를 위해 Off조작
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonOperatingOff()
{
	int nBufSize = 0;	// 2015.05.18 ljk
	char szBuf[1024] = { 0, };

	nBufSize = sprintf_s(szBuf, 1024, "%s", "/of/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonOperatingOn
//	 2. 함수기능 :	엔코더 지점 초기화를 위해 On조작
//	 3. 인    수 :	없음
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonOperatingOn()
{
	int nBufSize = 0;	// 2015.05.18 ljk
	char szBuf[1024] = { 0, };
	nBufSize = sprintf_s(szBuf, 1024, "%s", "/on/");
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonUpperDirection
//	 2. 함수기능 :	엔코더 0 방향 지정
//	 3. 인    수 :	엔코더 방향
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonUpperDirection(int nDirection)
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	nBufSize = sprintf_s(szBuf, 1024, "/1i%d/", nDirection);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	DaekonDownDirection
//	 2. 함수기능 :	엔코더 1 방향 지정
//	 3. 인    수 :	엔코더 방향
//	 4. 반 환 값 :	없음
//	 5. 작 성 일 :	2015/05/19
//	 6. 작 성 자 :	ljk
//	 7. 수정기록 :
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
void CTriggerControl::DaekonDownDirection(int nDirection)
{
	int nBufSize = 0;
	char szBuf[1024] = { 0, };

	nBufSize = sprintf_s(szBuf, 1024, "/1i%d/", nDirection);
	szBuf[nBufSize] = '\0';
	WriteComm((byte *)szBuf, nBufSize);
}