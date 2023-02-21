// Protocol16CH.cpp: implementation of the Protocol16CH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol16CH.h"
#include "stdlib.h"
#include "stdio.h"
#include "TsipProtocol.h"
#include "./../Rs232CommSTD/Rs232CommSTD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
extern void CallBackFunc(int nPort);

Protocol16CH::Protocol16CH()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_ulOnOffInfo = 0;
	m_pComm16CH	= new CRs232CommSTD;
	m_pTsipProtocol = new TsipProtocol;
	memset(&m_Info,0,sizeof(m_Info));
}

Protocol16CH::~Protocol16CH()
{ 
	if(m_pComm16CH) delete ((CRs232CommSTD *)m_pComm16CH);
	if(m_pTsipProtocol) delete ((TsipProtocol *)m_pTsipProtocol);
	m_pComm16CH= NULL;
	m_CallBack	= NULL;
	m_hWnd		= NULL;
}

bool Protocol16CH::CommandRx(void)
{
	BYTE buff[RS232_BUFF_SIZE];
	int size=((CRs232CommSTD *)m_pComm16CH)->GetSize();
	for(int i=0;i<size;i++)	((CRs232CommSTD *)m_pComm16CH)->GetByte(&buff[i]);
	if(((TsipProtocol *)m_pTsipProtocol)->DecodeData(size,(char *)buff))	CommandDecoding();
	return true;
}

bool Protocol16CH::CommandDecoding()
{
	int nOffset=0;
	int i;
	for(int j=0;j<((TsipProtocol *)m_pTsipProtocol)->m_nRxPacket;j++)
	{
		switch(((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset])
		{
			case ID_REMOTE_PWM_SETINFO:
				for(i=0;i<((((TsipProtocol *)m_pTsipProtocol)->m_pSizeTable[j]-1)/2);i++)
				{
					//					channal											data
					if(((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+1]>15) 	break;
					m_Info.nRemotePwmStatus[((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+1]] = ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+2];
				}
				if(m_hWnd)		PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)&m_Info);
				if(m_CallBack)	m_CallBack(m_Info.nPort,(char *)&m_Info);
				break;
			case ID_MAN_PWM_INFO:
				for(i=0;i<((((TsipProtocol *)m_pTsipProtocol)->m_pSizeTable[j]-1)/2);i++)
				{
					//					channal											data
					m_Info.nManPwmStatus[((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+1]] = ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+2];
				}
				break;
			case ID_PIO_PWM_INFO:
				for(i=0;i<((((TsipProtocol *)m_pTsipProtocol)->m_pSizeTable[j]-1)/2);i++)
				{
					//					channal											data
					m_Info.nPioPwmStatus[((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+1]] = ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i*2+2];
				}
				break;
			case ID_PIO_ONOFF_INFO:
				for(i=0;i<(((TsipProtocol *)m_pTsipProtocol)->m_pSizeTable[j]-1);i++)
				{
					m_Info.nPioOnOffStatus[i] = ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+i+1];
				}
				break;
			case ID_BAUDRATE_SETINFO:
				m_Info.nBaudStatus = ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1];
				break;
			case ID_MODE_INFO:
				m_Info.nMode		= ((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1];
				break;
			case ID_FIRMWARE_VER_INFO:
				m_Info.fFirmwareVersion = (float)(((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1]>>4) + (((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1]&0x0F)/10;
				break;
			case ID_FPGA_VER_INFO:
				m_Info.fFpgaVersion = (float)(((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1]>>4) + (((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1]&0x0F)/10;
				break;
			case ID_CURRENT_INFO:
				if(m_hWnd)		PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)&m_Info);
				if(m_CallBack)	m_CallBack(m_Info.nPort,(char *)&m_Info);
				break;
			case ID_REMOTE_PWM_REQUEST:
				ReportBrightness((int)(((TsipProtocol *)m_pTsipProtocol)->m_pRxBuffFix[nOffset+1]));
				break;
			case ID_CURRENT_REQUEST:
				ReportBrightness(0xFF);
				break;
			default :
				break;
		}
		nOffset +=((TsipProtocol *)m_pTsipProtocol)->m_pSizeTable[j]; //for process multi-Packet 
	}
	return false;
}

int Protocol16CH::Create(int nPort,void (*callback)(int nPort,char *pBuff))
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm16CH)->Create(buff,RS232_BAUDRATE_19200,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<16;i++)	SetBrightness(i,0);
		SetOnOffEx(0xFFFFFFFF);
		m_Info.nPort = nPort;
		m_CallBack = callback;
	}
	return ret;
}

int Protocol16CH::Create(int nPort,HWND hWnd)
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm16CH)->Create(buff,RS232_BAUDRATE_19200,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<16;i++)	SetBrightness(i,0);
		m_Info.nPort = nPort;
		m_hWnd		   = hWnd;
	}
	return ret;
}

void Protocol16CH::SetBrightness(int channel,int brightness)
{
	char tempBuff[2];
	TsipSendPacket Packet;
	tempBuff[0] = channel; tempBuff[1] = brightness;
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_SETINFO,2,tempBuff);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
	m_Info.nRemotePwmStatus[channel]=brightness;
}

void Protocol16CH::GetBrightness(int channel)
{
	char buff=channel;
	TsipSendPacket Packet;
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_REQUEST,1,&buff);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
}

void Protocol16CH::RequestCurrentStatus()
{
	TsipSendPacket Packet;
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_CURRENT_REQUEST);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
}

void Protocol16CH::SaveCurrentStatus()
{
	TsipSendPacket Packet;
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_CURRENT_SAVE);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
}

void Protocol16CH::ReportBrightness(int channel)
{
	char tempBuff[2];
	TsipSendPacket Packet;
	if(channel==0xFF)
	{
		for(int i=0;i<LIGHT_MODULE16_CHANNEL;i++)
		{
			tempBuff[0] = channel; tempBuff[1] = m_Info.nRemotePwmStatus[i];
			Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_SETINFO,2,tempBuff);
			((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
			Sleep(PROTOCOL_16CH_WAIT);
		}
	}
	else
	{
		tempBuff[0] = channel; tempBuff[1] = m_Info.nRemotePwmStatus[channel];
		Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_SETINFO,2,tempBuff);
		((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
		Sleep(PROTOCOL_16CH_WAIT);
	}

}

void Protocol16CH::SetOnOff(int channel,int onoff)
{
	char tempBuff[2];
	TsipSendPacket Packet;
	if(onoff)	m_ulOnOffInfo |= (0x01<<channel);
	else m_ulOnOffInfo &= (~(0x01<<channel));
	tempBuff[0] = (char)(m_ulOnOffInfo&0xFF); tempBuff[1] = (char)((m_ulOnOffInfo>>8)&0xFF);
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_SETONOFF,2,tempBuff);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
}


void Protocol16CH::SetOnOffEx(ULONG onoff)
{
	char tempBuff[2];
	TsipSendPacket Packet;
	m_ulOnOffInfo = onoff;
	tempBuff[0] = (char)(onoff&0xFF); tempBuff[1] = (char)((onoff>>8)&0xFF);
	Packet = ((TsipProtocol *)m_pTsipProtocol)->EncodeData(ID_REMOTE_PWM_SETONOFF,2,tempBuff);
	((CRs232CommSTD *)m_pComm16CH)->WriteData(Packet.SendData,Packet.dataSize);
	Sleep(PROTOCOL_16CH_WAIT);
}
