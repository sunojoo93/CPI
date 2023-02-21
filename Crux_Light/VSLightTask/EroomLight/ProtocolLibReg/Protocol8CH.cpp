// Protocol8CH.cpp: implementation of the Protocol8CH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol8CH.h"
#include "stdlib.h"
#include "stdio.h"
#include "./../Rs232CommSTD/Rs232CommSTD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void CallBackFunc(int nPort);

Protocol8CH::Protocol8CH()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_ulOnOffInfo = 0;
	m_pBuff		= new unsigned char[RS232_BUFF_SIZE];
	m_CmdBuff	= new unsigned char[RS232_BUFF_SIZE];
	m_pComm8CH  = new CRs232CommSTD;
	m_nCmdRxCnt = 0;
	memset(&m_Info,NULL,sizeof(m_Info));
}

Protocol8CH::~Protocol8CH()
{ 
	if(m_pComm8CH) delete ((CRs232CommSTD *)m_pComm8CH);
	if(m_pBuff) delete m_pBuff;
	if(m_CmdBuff) delete m_CmdBuff;
	m_pComm8CH= NULL;
	m_CallBack	= NULL;
	m_hWnd		= NULL;
}

bool Protocol8CH::CommandRx(void)
{
	int bRxComplete=false;
	int size=((CRs232CommSTD *)m_pComm8CH)->GetSize();
	for(int i=0;i<size;i++) 
	{
		((CRs232CommSTD *)m_pComm8CH)->GetByte(&m_pBuff[m_nCmdRxCnt]);
		switch(m_pBuff[m_nCmdRxCnt] & COMMAND_MASK)
		{
			case START_FRAME:
				m_nCmdRxCnt = 1;
				break;
			case END_FRAME:
				CommandDecoding(m_nCmdRxCnt,m_pBuff);
				m_nCmdRxCnt = 0;
				break;
			default:
				m_nCmdRxCnt++;
				if(m_nCmdRxCnt > 6) 
					m_nCmdRxCnt = 0;
				break;
		}
	}
	return true;
}

bool Protocol8CH::CommandDecoding(int cnt,unsigned char *pBuff)
{
	if(	((pBuff[0]&COMMAND_MASK)==START_FRAME) &&
		((pBuff[1]&COMMAND_MASK)==COMMAND_BYTE) &&
		((pBuff[2]&COMMAND_MASK)==DATA_BYTE) &&
		((pBuff[3]&COMMAND_MASK)==DATA_BYTE) &&
		((pBuff[4]&COMMAND_MASK)==DATA_BYTE) &&
		((pBuff[5]&COMMAND_MASK)==DATA_BYTE) &&
		((pBuff[6]&COMMAND_MASK)==END_FRAME))
	{
		switch(pBuff[1] & 0x0F)
		{
			case COMMAND_ONTIME:
				m_Info.triggerStatus.usOnTime[pBuff[2]&0x07] = 
					(USHORT)(pBuff[3]&0x1F)|((USHORT)(pBuff[4]&0x1F)<<5)|((USHORT)(pBuff[5]&0x1F)<<10);
				break;
			case COMMAND_DELAY:
				m_Info.triggerStatus.usDelayTime[pBuff[2]&0x07] = 
					(USHORT)(pBuff[3]&0x1F)|((USHORT)(pBuff[4]&0x1F)<<5)|((USHORT)(pBuff[5]&0x1F)<<10);
				break;
			case COMMAND_HOLD:
				m_Info.triggerStatus.usHoldTime[pBuff[2]&0x07] = 
					(USHORT)(pBuff[3]&0x1F)|((USHORT)(pBuff[4]&0x1F)<<5)|((USHORT)(pBuff[5]&0x1F)<<10);
				break;
			case COMMAND_TRGRF_EN:
				m_Info.triggerStatus.ucEdge = (pBuff[2]&0x0F);
				m_Info.triggerStatus.ucEnable = (pBuff[4]&0x0F);
				break;
			case COMMAND_PWMCTRL:
				m_Info.nRemotePwmStatus[pBuff[2]&0x07] = (pBuff[4]&0x0F) | ((pBuff[5]&0x0F)<<4);
				if(m_hWnd)  ::PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)(char *)&m_Info);
				if(m_CallBack)	m_CallBack(m_Info.nPort,(char *)&m_Info);
				break;
			case COMMAND_SWONOFF0:
				m_Info.nRemoteOnOffStatus[0] = (pBuff[2]&0x0F) | ((pBuff[3]&0x0F)<<4);
				m_Info.nRemoteOnOffStatus[1] = (pBuff[4]&0x0F) | ((pBuff[5]&0x0F)<<4);
				break;
			case COMMAND_SWONOFF1:
				m_Info.nRemoteOnOffStatus[2] = (pBuff[2]&0x0F) | ((pBuff[3]&0x0F)<<4);
				m_Info.nRemoteOnOffStatus[3] = (pBuff[4]&0x0F) | ((pBuff[5]&0x0F)<<4);
				if(m_hWnd)  ::PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)(char *)&m_Info);
				if(m_CallBack)	m_CallBack(m_Info.nPort,(char *)&m_Info);
				break;
			case COMMAND_DAC:
				m_Info.nDacValue[pBuff[2] & 0x03] = (pBuff[3]&0x0F) | ((pBuff[4]&0x0F)<<4);
				break;
			case COMMAND_ADC:
				m_Info.nManPwmStatus[pBuff[2]&0x07] = (pBuff[3]&0x0F) | ((pBuff[4]&0x0F)<<4);
				break;
			case COMMAND_PIO:
				m_Info.nPioPwmStatus[pBuff[2]&0x07] = (pBuff[3]&0x0F) | ((pBuff[4]&0x0F)<<4);
				break;
			case COMMAND_BAUD:
				if(m_hWnd)  ::PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)(char *)&m_Info);
				if(m_CallBack)	m_CallBack(m_Info.nPort,(char *)&m_Info);
				break;
			case COMMAND_READ:
				ReportBrightness();
				break;
			default:
				break;
		}
	}
	return true;
}

int Protocol8CH::Create(int nPort,void (*callback)(int nPort,char *pBuff))
{
	int ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);

	ret = ((CRs232CommSTD *)m_pComm8CH)->Create(buff,RS232_BAUDRATE_19200,nPort,false,CallBackFunc);
	if(ret)
	{
/*		SetOnOff(0);
		for(int i=0;i<8;i++) SetBrightness(i,0);
		for(i=0;i<4;i++) 
		{
			TriggerDelay(i,0);
			TriggerHold(i,0);
			TriggerOnTime(i,0);
		}
*/
		SetOnOffEx(0xFFFFFFFF);
		TriggerStatus(0,0);
		m_Info.nPort = nPort;
		m_CallBack = callback;
	}
	return ret;
}

int Protocol8CH::Create(int nPort,HWND hWnd)
{
	int ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);

	ret = ((CRs232CommSTD *)m_pComm8CH)->Create(buff,RS232_BAUDRATE_19200,nPort,false,CallBackFunc);
	if(ret)
	{
/*		SetOnOff(0);
		for(int i=0;i<8;i++) SetBrightness(i,0);
		for(i=0;i<4;i++) 
		{
			TriggerDelay(i,0);
			TriggerHold(i,0);
			TriggerOnTime(i,0);
		}
*/
		SetOnOffEx(0xFFFFFFFF);
		TriggerStatus(0,0);
		m_Info.nPort = nPort;
		m_hWnd		   = hWnd;
	}
	return ret;
}

void Protocol8CH::SetOnOff(int channel,int onoff)
{
	PROTOCOL_8CH_PACKET protocol;
	UCHAR ucCommandSel	 = COMMAND_SWONOFF0;

	if(onoff)	m_ulOnOffInfo |= (0x01<<channel);
	else m_ulOnOffInfo &= (~(0x01<<channel));

	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | ucCommandSel;
	protocol.ucData[0] = DATA_BYTE | (UCHAR)(m_ulOnOffInfo & 0x0F);
	protocol.ucData[1] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>4) & 0x0F);
	protocol.ucData[2] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>8) & 0x0F);
	protocol.ucData[3] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>12) & 0x0F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);

	ucCommandSel		= COMMAND_SWONOFF1;
	protocol.ucCommand = COMMAND_BYTE | ucCommandSel;
	protocol.ucData[0] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>16) & 0x0F);
	protocol.ucData[1] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>20) & 0x0F);
	protocol.ucData[2] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>24) & 0x0F);
	protocol.ucData[3] = DATA_BYTE | (UCHAR)((m_ulOnOffInfo>>28) & 0x0F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);

	m_Info.nRemoteOnOffStatus[0] = (UCHAR)(m_ulOnOffInfo&0xFF);
	m_Info.nRemoteOnOffStatus[1] = (UCHAR)((m_ulOnOffInfo>>8)&0xFF);
	m_Info.nRemoteOnOffStatus[2] = (UCHAR)((m_ulOnOffInfo>>16)&0xFF);
	m_Info.nRemoteOnOffStatus[3] = (UCHAR)((m_ulOnOffInfo>>24)&0xFF);
}

void Protocol8CH::SetOnOffEx(ULONG onoff)
{
	PROTOCOL_8CH_PACKET protocol;
	UCHAR ucCommandSel	 = COMMAND_SWONOFF0;

	m_ulOnOffInfo = onoff;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | ucCommandSel;
	protocol.ucData[0] = DATA_BYTE | (UCHAR)(onoff & 0x0F);
	protocol.ucData[1] = DATA_BYTE | (UCHAR)((onoff>>4) & 0x0F);
	protocol.ucData[2] = DATA_BYTE | (UCHAR)((onoff>>8) & 0x0F);
	protocol.ucData[3] = DATA_BYTE | (UCHAR)((onoff>>12) & 0x0F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);

	ucCommandSel		= COMMAND_SWONOFF1;
	protocol.ucCommand = COMMAND_BYTE | ucCommandSel;
	protocol.ucData[0] = DATA_BYTE | (UCHAR)((onoff>>16) & 0x0F);
	protocol.ucData[1] = DATA_BYTE | (UCHAR)((onoff>>20) & 0x0F);
	protocol.ucData[2] = DATA_BYTE | (UCHAR)((onoff>>24) & 0x0F);
	protocol.ucData[3] = DATA_BYTE | (UCHAR)((onoff>>28) & 0x0F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);

	m_Info.nRemoteOnOffStatus[0] = (UCHAR)(onoff&0xFF);
	m_Info.nRemoteOnOffStatus[1] = (UCHAR)((onoff>>8)&0xFF);
	m_Info.nRemoteOnOffStatus[2] = (UCHAR)((onoff>>16)&0xFF);
	m_Info.nRemoteOnOffStatus[3] = (UCHAR)((onoff>>24)&0xFF);
}

void Protocol8CH::SetBrightness(int channel,int brightness)
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | COMMAND_PWMCTRL;
	protocol.ucData[0] = DATA_BYTE | (channel&0x07);
	protocol.ucData[1] = DATA_BYTE;
	protocol.ucData[2] = DATA_BYTE | (brightness&0x0F);
	protocol.ucData[3] = DATA_BYTE | ((brightness>>4)&0x0F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
	m_Info.nRemotePwmStatus[channel] = brightness;
}


void Protocol8CH::TriggerStatus(int ucrf, int ucen)
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | COMMAND_TRGRF_EN;
	protocol.ucData[0] = DATA_BYTE | (ucrf & 0x0F);
	protocol.ucData[1] = DATA_BYTE;
	protocol.ucData[2] = DATA_BYTE | (ucen & 0x0F);
	protocol.ucData[3] = DATA_BYTE;
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
}

void Protocol8CH::TriggerTime(int channel, UCHAR command, UINT time)
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | command;
	protocol.ucData[0] = DATA_BYTE | (channel&0x07);
	protocol.ucData[1] = DATA_BYTE | (time & 0x1F);
	protocol.ucData[2] = DATA_BYTE | ((time>>5) & 0x1F);
	protocol.ucData[3] = DATA_BYTE | ((time>>10) & 0x1F);
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
}

void Protocol8CH::TriggerDelay(int channel, int time)
{
	TriggerTime(channel,COMMAND_DELAY,time);
}

void Protocol8CH::TriggerHold(int channel, int time)
{
	TriggerTime(channel,COMMAND_HOLD,time);
}

void Protocol8CH::TriggerOnTime(int channel, int time)
{
	TriggerTime(channel,COMMAND_ONTIME,time);
}

void Protocol8CH::GetBrightness(int channel)
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | COMMAND_READ;
	protocol.ucData[0] = DATA_BYTE;
	protocol.ucData[1] = DATA_BYTE;
	protocol.ucData[2] = DATA_BYTE;
	protocol.ucData[3] = DATA_BYTE;
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
}

void Protocol8CH::SaveCurrentStatus()
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | COMMAND_SAVE;
	protocol.ucData[0] = DATA_BYTE;
	protocol.ucData[1] = DATA_BYTE;
	protocol.ucData[2] = DATA_BYTE;
	protocol.ucData[3] = DATA_BYTE;
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
}

void Protocol8CH::RequestCurrentStatus()
{
	PROTOCOL_8CH_PACKET protocol;
	protocol.ucStartFrame = START_FRAME;
	protocol.ucEndFrame = END_FRAME;
	protocol.ucCommand = COMMAND_BYTE | COMMAND_READ;
	protocol.ucData[0] = DATA_BYTE;
	protocol.ucData[1] = DATA_BYTE;
	protocol.ucData[2] = DATA_BYTE;
	protocol.ucData[3] = DATA_BYTE;
	((CRs232CommSTD *)m_pComm8CH)->WriteData((unsigned char *)&protocol,sizeof(PROTOCOL_8CH_PACKET));
	Sleep(PROTOCOL_8CH_WAIT);
}

void Protocol8CH::ReportBrightness()
{
	for(int i=0;i<LIGHT_MODULE8_CHANNEL;i++)	SetBrightness(i,m_Info.nRemotePwmStatus[i]);
}
