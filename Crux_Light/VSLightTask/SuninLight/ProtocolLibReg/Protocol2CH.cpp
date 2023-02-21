// Protocol2CH.cpp: implementation of the Protocol2CH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol2CH.h"
#include "stdlib.h"
#include "stdio.h"
#include "./../Rs232CommSTD/Rs232CommSTD.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern void CallBackFunc(int nPort);

Protocol2CH::Protocol2CH()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_pBuff		= new unsigned char[RS232_BUFF_SIZE];
	m_CmdBuff	= new unsigned char[RS232_BUFF_SIZE];
	m_pComm2CH	= new CRs232CommSTD;
	m_nCmdRxCnt = 0;
	memset(m_pBuff,NULL,RS232_BUFF_SIZE);
	memset(m_CmdBuff,NULL,RS232_BUFF_SIZE);
	memset(&m_Info,NULL,sizeof(m_Info));
}

Protocol2CH::~Protocol2CH()
{ 
	if(m_pComm2CH) delete ((CRs232CommSTD *)m_pComm2CH);
	if(m_pBuff) delete m_pBuff;
	if(m_CmdBuff) delete m_CmdBuff;
	m_pComm2CH	= NULL;
	m_hWnd	= NULL;
	m_CallBack	= NULL;
}

bool Protocol2CH::CommandRx(void)
{
	int size=((CRs232CommSTD *)m_pComm2CH)->GetSize();
	for(int i=0;i<size;i++) 
	{
		((CRs232CommSTD *)m_pComm2CH)->GetByte(&m_pBuff[m_nCmdRxCnt]);
		m_nCmdRxCnt++;
		if(m_pBuff[m_nCmdRxCnt-1]==ASCII_CARRIGE_RETURN) 
		{
			CommandDecoding(m_nCmdRxCnt,m_pBuff);
			m_nCmdRxCnt = 0;
		}
		else if(m_pBuff[m_nCmdRxCnt-1]==ASCII_NEWLINE)	
		{
			m_nCmdRxCnt = 0;
		}
		else if(m_nCmdRxCnt>1000) m_nCmdRxCnt = 0;
	}
	return true;
}

bool Protocol2CH::CommandDecoding(int cnt,unsigned char *pBuff)
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
		ReportBrightness((int)cmd_decode.argument[0]);
	}
	else if(!strcmp(cmd_decode.cmd,"geterror") && (cmd_decode.argument[0]==0 || cmd_decode.argument[0]==1))
	{
		ReportBrightness((int)cmd_decode.argument[0]);
	}
	return true;
}

int Protocol2CH::Create(int nPort,HWND hWnd)
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm2CH)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<2;i++) SetBrightness(i,0);
		for(i=0;i<2;i++) SetBrightness(i,0);
		m_Info.nPort = nPort;
		m_hWnd		   = hWnd;
	}
	return ret;
}

int Protocol2CH::Create(int nPort,void (*callback)(int nPort,char *pBuff))
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm2CH)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<2;i++) SetBrightness(i,0);
		for(i=0;i<2;i++) SetBrightness(i,0);
		m_Info.nPort = nPort;
		m_CallBack	   = callback;
	}
	return ret;
}

void Protocol2CH::SetBrightness(int channel,int brightness)
{
	char pBuff[50];
	sprintf_s(pBuff,"setbright %d %d%c",channel,brightness,0x0A);
	((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_2CH_WAIT);
	m_Info.nRemotePwmStatus[channel] = brightness;
}

void Protocol2CH::GetBrightness(int channel)
{
	char pBuff[50];
	sprintf_s(pBuff,"getbright %d%c",channel,0x0A);
	((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_2CH_WAIT);
}

void Protocol2CH::GetError()
{
	char pBuff[50];
	sprintf(pBuff,"geterror%c",0x0A);
	((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_2CH_WAIT);
}

void Protocol2CH::RequestCurrentStatus(void)
{
	char pBuff[50];
	for(int i=0;i<2;i++)
	{
		sprintf_s(pBuff,"getbright %d%c",i,0x0A);
		((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
		Sleep(PROTOCOL_2CH_WAIT);
	}
}

void Protocol2CH::SaveCurrentStatus(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"save%c",0x0A);
	((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_2CH_WAIT);
}


void Protocol2CH::ReportBrightness(int channel)
{
	char pBuff[50];
	sprintf_s(pBuff,"brightness %d %d%c",channel,m_Info.nRemotePwmStatus[channel],0x0A);
	((CRs232CommSTD *)m_pComm2CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_2CH_WAIT);
}
