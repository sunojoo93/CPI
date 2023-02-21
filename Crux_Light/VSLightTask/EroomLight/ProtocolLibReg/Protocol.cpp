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

int Protocol::Create(int nPort, int nType, HWND hWnd)
{
	int i,ret;
	char buff[20];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<nType;i++) SetOnOff(i,1);			// Manual -> Remote 변경 후 Channel On 해줘야 함
		for(i=0;i<nType;i++) SetBrightness(i,0);
		m_Info.nPort = nPort;
		m_hWnd		   = hWnd;
	}
	return ret;
}

int Protocol::Create(int nPort, int nType, void (*callback)(int nPort,char *pBuff))
{
	int i,ret;
	char buff[20];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<nType;i++) SetOnOff(i,1);			// Manual -> Remote 변경 후 Channel On 해줘야 함
		for(i=0;i<nType;i++) SetBrightness(i,0);
		m_Info.nPort = nPort;
		m_CallBack	   = callback;
	}
	return ret;
}

void Protocol::SetBrightness(int channel,int brightness)
{
	char pBuff[50];
	sprintf_s(pBuff,"setbright %d %d%c",channel,brightness,0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_EROOM_WAIT);
	m_Info.nRemotePwmStatus[channel] = brightness;
}

void Protocol::SetOnOff(int channel,int onoff)
{
	char pBuff[50];
	sprintf_s(pBuff,"setonoff %d %d%c",channel,onoff,0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_EROOM_WAIT);
	m_Info.nOnOffStatus[channel] = onoff;
}

void Protocol::GetBrightness(int channel)
{
	char pBuff[50];
	sprintf_s(pBuff,"getbright %d%c",channel,0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_EROOM_WAIT);
}

// void Protocol::GetC(int channel)
// {
// 	char pBuff[50];
// 	sprintf(pBuff,"getc %d%c",channel,0x0A);
// 	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
// 	Sleep(PROTOCOL_EROOM_WAIT);
// }

void Protocol::SaveCurrentStatus(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"savemem%c",0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_EROOM_WAIT);
}

void Protocol::GetError()
{
	char pBuff[50];
	sprintf_s(pBuff,"geterrstatus%c",0x0A);
	((CRs232CommSTD *)m_pComm)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_EROOM_WAIT);
}