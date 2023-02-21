// Protocol.cpp: implementation of the Protocol class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol.h"
#include "stdlib.h"
#include "stdio.h"
#include "./../Rs232CommSTD/Rs232CommSTD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern void CallBackFunc(int nPort);

Protocol::Protocol()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_pBuff		= new unsigned char[RS232_BUFF_SIZE];
	m_CmdBuff	= new unsigned char[RS232_BUFF_SIZE];
	m_pComm	= new CRs232CommSTD;
	m_nCmdRxCnt = 0;
	memset(m_pBuff,NULL,RS232_BUFF_SIZE);
	memset(m_CmdBuff,NULL,RS232_BUFF_SIZE);
	memset(&m_Info,NULL,sizeof(m_Info));
}

Protocol::~Protocol()
{ 
	if(m_pComm) delete ((CRs232CommSTD *)m_pComm);
	if(m_pBuff) delete m_pBuff;
	if(m_CmdBuff) delete m_CmdBuff;
	m_pComm	= NULL;
	m_hWnd	= NULL;
	m_CallBack	= NULL;
}

bool Protocol::CommandRx(void)
{
	int size=((CRs232CommSTD *)m_pComm)->GetSize();
	for(int i=0;i<size;i++) 
	{
		((CRs232CommSTD *)m_pComm)->GetByte(&m_pBuff[m_nCmdRxCnt]);
		TRACE("%c",m_pBuff[m_nCmdRxCnt]);
		m_nCmdRxCnt++;
		if(m_pBuff[m_nCmdRxCnt-1]==ASCII_CARRIGE_RETURN) 
		{
			m_pBuff[m_nCmdRxCnt-1] = NULL;
		}
		else if(m_pBuff[m_nCmdRxCnt-1]==ASCII_NEWLINE)	
		{
			m_pBuff[m_nCmdRxCnt-1] = NULL;
			m_nCmdRxCnt = 0;
			if(m_CallBack) m_CallBack(m_Info.nPort,(char *)m_pBuff);
			if(m_hWnd) PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)m_pBuff);			
		}
		else if(m_nCmdRxCnt>1000) m_nCmdRxCnt = 0;
	}
	return true;
}

bool Protocol::CommandDecoding(int cnt,unsigned char *pBuff)
{
	UCHAR i,j=0;
	UCHAR cmdbuff[1024];
	UCHAR argument_ch[10][5];
	CMD_PROTOCOL cmd_decode;

	memset(argument_ch,NULL,sizeof(argument_ch));
	memset(cmdbuff,NULL,1024);
	memset(&cmd_decode,NULL,sizeof(cmd_decode));
	memcpy(cmdbuff,pBuff,cnt);
	memcpy(cmd_decode.strcmd,pBuff,cnt);
	cmd_decode.strcmd[cnt-1]=NULL;
	cmd_decode.argnum = 0;

	for(i=0;i<cnt;i++)
	{
		if(cmdbuff[i] != ASCII_SPACE && cmdbuff[i] != ASCII_CARRIGE_RETURN)
		{
			switch(cmd_decode.argnum)
			{
				case 0:		cmd_decode.cmd[i] = tolower(cmdbuff[i]);break;
				case 1:		argument_ch[0][j] = cmdbuff[i];j++;break;
				case 2:		argument_ch[1][j] = cmdbuff[i];j++;break;
				case 3:		argument_ch[2][j] = cmdbuff[i];j++;break;
				case 4:		argument_ch[3][j] = cmdbuff[i];j++;break;
				case 5:		argument_ch[3][j] = cmdbuff[i];j++;break;
				case 6:		argument_ch[3][j] = cmdbuff[i];j++;break;
				case 7:		argument_ch[3][j] = cmdbuff[i];j++;break;
				case 8:		argument_ch[3][j] = cmdbuff[i];j++;break;
				case 9:		argument_ch[3][j] = cmdbuff[i];j++;break;
				default: 	break;
			}
		}
		else 
		{
			switch(cmd_decode.argnum)
			{
				case 0:		cmd_decode.cmd[i] = (UCHAR)NULL;break;
				case 1:		cmd_decode.argument[0] = (float)atof((const char *)argument_ch[0]);j=0;break;
				case 2:		cmd_decode.argument[1] = (float)atof((const char *)argument_ch[1]);j=0;break;
				case 3:		cmd_decode.argument[2] = (float)atof((const char *)argument_ch[2]);j=0;break;
				case 4:		cmd_decode.argument[3] = (float)atof((const char *)argument_ch[3]);j=0;break;
				case 5:		cmd_decode.argument[4] = (float)atof((const char *)argument_ch[4]);j=0;break;
				case 6:		cmd_decode.argument[5] = (float)atof((const char *)argument_ch[5]);j=0;break;
				case 7:		cmd_decode.argument[6] = (float)atof((const char *)argument_ch[6]);j=0;break;
				case 8:		cmd_decode.argument[7] = (float)atof((const char *)argument_ch[7]);j=0;break;
				case 9:		cmd_decode.argument[8] = (float)atof((const char *)argument_ch[8]);j=0;break;
				default: 	break;
			}
			cmd_decode.argnum++;
		}
	}
	if(!strcmp(cmd_decode.cmd,"brightness") && (cmd_decode.argument[0]==0 || cmd_decode.argument[0]==1))
	{
		m_Info.nRemotePwmStatus[(int)cmd_decode.argument[0]] = (int)cmd_decode.argument[1];
		if(m_CallBack) m_CallBack(m_Info.nPort,(char *)&m_Info);
		if(m_hWnd) PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)&m_Info);
	}
	else if(!strcmp(cmd_decode.cmd,"setbright") && (cmd_decode.argument[0]==0 || cmd_decode.argument[0]==1))
	{
		m_Info.nRemotePwmStatus[(int)cmd_decode.argument[0]] = (int)cmd_decode.argument[1];
		if(m_CallBack) m_CallBack(m_Info.nPort,(char *)&m_Info);
		if(m_hWnd) PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)&m_Info);
	}
	else if(!strcmp(cmd_decode.cmd,"getbright") && (cmd_decode.argument[0]==0 || cmd_decode.argument[0]==1))
	{
		//ReportBrightness((int)cmd_decode.argument[0]);
	}
	return true;
}

int Protocol::Create(int nPort, int CtrlCnt, UINT CtrlChannelCnt[], int nBaud, int nType, HWND hWnd)
{
	// 사용 X?
	int i,ret;
	char buff[20];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm)->Create(buff, nBaud ,nPort,false,CallBackFunc);
	if(ret)
	{
		int nTotalChCnt = 0;
		for (i = 0; i < CtrlCnt; i++)			nTotalChCnt += CtrlChannelCnt[i];

		UINT* LightVal;
		LightVal = (UINT*)malloc(sizeof(UINT) * nTotalChCnt);
		for(int nIndex = 0 ; nIndex < nTotalChCnt; nIndex++)		LightVal[nIndex] = 0;
			
		//Initialize.. All Channel SetOnOff & Setbrightness		180718 YSS
		SetOnOff(0, 1, nTotalChCnt);
		SetBrightness(0, LightVal, nTotalChCnt);

		m_Info.nPort = nPort;
		m_hWnd		   = hWnd;
		free(LightVal);
	}
	return ret;
}

int Protocol::Create(int nPort, int CtrlCnt, UINT CtrlChannelCnt[], int nBaud, int nType, void (*callback)(int nPort,char *pBuff))
{
	int i,ret;
	char buff[20];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm)->Create(buff, nBaud, nPort, false, CallBackFunc);
	if(ret)
	{
		int nTotalChCnt = 0;
		for (i = 0; i < CtrlCnt; i++)			nTotalChCnt += CtrlChannelCnt[i];

		UINT* LightVal;
		LightVal = (UINT*)malloc(sizeof(UINT) * nTotalChCnt);
		for(int nIndex = 0 ; nIndex < nTotalChCnt; nIndex++)		LightVal[nIndex] = 0;
		
		//Initialize.. All Channel SetOnOff & Setbrightness		180718 YSS
		SetOnOff(0, 1, nTotalChCnt);
		SetBrightness(0, LightVal, nTotalChCnt);

		m_Info.nPort = nPort;
		m_CallBack	   = callback;
		free(LightVal);
	}
	return ret;
}

void Protocol::SetBrightness(int ctrlID, UINT LightVal[], int channelCnt)
{
	char pBuff[500];
	
	// PC			->	Controller		: setbrightness 0 999 [Ch1 Value] [Ch2 Value] [Ch3 Value]
	// Controller	->	PC sample code	: setbrightness 0 999 [Ch1 Value] [Ch2 Value] [Ch3 Value]

	// cmd ex)
	// : PC			->	Controller		: "setbrightness 0 999 100 200 250"		Ch1 : 100,	Ch2 : 200,	Ch3 : 250		조명 값 설정
	// : PC			->	Controller		: "setbrightness 0 999 100 150 200"		Ch1 : 100,	Ch2 : 150,	Ch3 : 200		조명 값 설정

	sprintf_s(pBuff, "setbrightness 0 999");		// setbrightness 0 999 
	
	char Value[10];
	memset(Value, 0, sizeof(Value));
	for(int nCnt = 0; nCnt < channelCnt; nCnt++)
	{
		sprintf_s(Value, " %d", LightVal[nCnt]);
		strcat_s(pBuff, Value);
		if(nCnt == channelCnt - 1)
		{
			sprintf_s(Value, "%c", 0x0D);
			strcat_s(pBuff, Value);
		}
	}

	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_WAIT);
	//m_Info.nRemotePwmStatus[channel] = brightness;
// 	for(int nCnt = 0; nCnt < channelCnt; nCnt++)
// 	{
// 		m_Info.nRemotePwmStatus[nCnt] = LightVal[nCnt];
// 	}
}

void Protocol::SetOnOff(int ctrlID, int onoff, int channelCnt)
{
	char pBuff[300];

	// PC			->	Controller		: setonex 0 [On/Off Value]
	// Controller	->	PC sample code	: setonex 0 [On/Off Value]

	// cmd ex)
	// : PC			->	Controller		: "setonex 0 100"						Ch1 : ON,	Ch2 : OFF,	Ch3 : OFF
	// : PC			->	Controller		: "setonex 0 111"						Ch1 : ON,	Ch2 : ON ,	Ch3 : ON

	sprintf_s(pBuff,"setonex 0 ");

	char Value[10];
	memset(Value, 0, sizeof(Value));
	for(int nCnt = 0; nCnt < channelCnt; nCnt++)
	{
		sprintf_s(Value, "%d", onoff);
		strcat_s(pBuff, Value);
		if(nCnt == channelCnt - 1)
		{
			sprintf_s(Value, "%c", 0x0D);
			strcat_s(pBuff, Value);
		}
	}
	
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_ONOFF_WAIT);		// ON_OFF 셋팅 후 Current 값 300ms 후에 안정 됨.
	//m_Info.nOnOffStatus[channel] = onoff;
}

void Protocol::GetOnOff(int ctrlNum)
{
	char pBuff[50];

	//  PC			->	Controller		: getonex 0

	// cmd ex)
	// : PC			->	Controller		: "getonex 0"
	// : Controller ->	PC				: "getonex 0 011"						전체 채널 일괄 응답 -> Ch1 : OFF, Ch2 : ON, Ch3 : ON
	
	sprintf_s(pBuff,"getonex 0%c", 0x0D);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_ONOFF_WAIT);		// ON_OFF 셋팅 후 Current 값 300ms 후에 안정 됨.
}

void Protocol::GetBrightness(int channel)
{
	char pBuff[50];

	//  PC			->	Controller		: getbrightness 0 '0' or '999'
	
	// cmd ex)
	// : PC			->	Controller		: "getbrightness 0 0"
	// : PC			->	Controller		: "getbrightness 0 999"
	// : Controller ->	PC				: "getbrightness 0 0 100 200 250"		전체 채널 일괄 응답 -> Ch1 : 100, Ch2 : 200, Ch3 : 250

	sprintf_s(pBuff,"getbrightness 0 999%c",0x0D);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_WAIT);
}

void Protocol::SaveCurrentStatus(void)
{
	// 사용 X
	char pBuff[50];
	sprintf_s(pBuff,"savemem%c",0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_WAIT);
}

void Protocol::GetError()
{
	char pBuff[50];

	// PC			->	Controller		: geterror 0
	// Controller	-> PC sample code	: geterror 0 [Error Code] [Error comment]
	// [Error Code & Error Comment]
	// '0' : no_error
	// '1' : overcurrnt_error
	// '2' : overtemperature_error
	// '3' : fanalarm_error

	// cmd ex)
	// : PC			->	Controller		: "geterror"
	// : Controller ->	PC				: "geterror 0 0 no_error"				No Error 응답
	// : Controller ->	PC				: "geterror 0 1 overcurrent_error"		Overcurrent Error 응답

	sprintf_s(pBuff,"geterror 0%c",0x0D);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_LFine_WAIT);
}

 void Protocol::GetC(int ctrlNum, int channel)
 {
 	char pBuff[50];

	// PC			->	Controller		: getc 0 999
	// Controller	-> PC sample code	: getc 0 999 [0~9999 mA] [0~9999 mA] [0~9999 mA]

	// cmd ex)
	// : PC			->	Controller		: "getc 0 999"
	// : Controller ->	PC				: "getc 0 99 1000 200 1900"				전체 채널 일괄 응답(mA) -> Ch1 : 1000 mA, Ch2 : 200 mA, Ch3 : 1900 mA

	sprintf_s(pBuff,"getc 0 999%c",0x0D);
 	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
 	Sleep(PROTOCOL_LFine_WAIT);
 }

 void Protocol::GetT(int ctrlNum, int channel)
 {
	 char pBuff[50];

	 // PC			->	Controller		: gett 0 999
	 // Controller	-> PC sample code	: gett 0 999 [0~999(999 ->99.9℃)] [0~999(999 ->99.9℃)] [0~999(999 ->99.9℃)]

	 // cmd ex)
	 // : PC			->	Controller	: "gett 0 999"
	 // : Controller ->	PC				: "gett 0 99 215 300 495"				전체 채널 일괄 응답(℃) -> Ch1 : 21.5 ℃, Ch2 : 30.0 ℃, Ch3 : 49.5 ℃

	 sprintf_s(pBuff,"gett 0 999%c", 0x0D);
	 ((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	 Sleep(PROTOCOL_LFine_WAIT);
 }