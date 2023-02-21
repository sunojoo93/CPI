// Protocol32CH.cpp: implementation of the Protocol32CH class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Protocol32CH.h"
#include "stdlib.h"
#include "stdio.h"
#include "./../Rs232CommSTD/Rs232CommSTD.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern void CallBackFunc(int nPort);

typedef enum {
	CMD_LIST_RESTART,
	CMD_LIST_REBOOT,
	CMD_LIST_RESET,
	CMD_LIST_MODULEINFO,
	CMD_LIST_VERSIONINFO,
	CMD_LIST_SETEEPROM,		// EEPROM Write.
	CMD_LIST_GETEEPROM,		// EEPROM Read.
	CMD_LIST_SETEEDUMP,		// EEPROM Dump Write.
	CMD_LIST_GETEEDUMP,		// EEPROM Dump Read.

	CMD_LIST_SETBRIGHT,		// Bright Write.
	CMD_LIST_GETBRIGHT,		// Bright Rea.
	CMD_LIST_SETON,			// LED Channel Control.
	CMD_LIST_SETONEX,		// LED Hex Control.
	CMD_LIST_GETONEX,
	CMD_LIST_SETBANDWIDTH,	// Brightness BandWidth Set.
	CMD_LIST_GETBANDWIDTH,	// Brightness BandWidth Get.
	CMD_LIST_SETINDEX,		// Brightness Memory Index Set.
	CMD_LIST_GETINDEX,		// Brightness Memory Index Get.

	CMD_LIST_CMDSAVE,		// Remote Bright Save.
	CMD_LIST_CMDLOAD,		// Remote Bright Load.
	CMD_LIST_CMDUPDATE,		// Brightness Update.
	CMD_LIST_CMDRESTORE,	// CMD Bright Restore factory.

	CMD_LIST_SETBURNIN,		// Burning Mode Selection : 0->Off, 1->On.
	CMD_LIST_SETSPEED,		// Burning Speed.
	CMD_LIST_SETBAUD,		// BaudRate Set.	
	CMD_LIST_GETBAUD,		// BaudRate Get.
	CMD_LIST_SETMODE,

	CMD_LIST_GETMANUAL,		// Manual Table Load.
	CMD_LIST_GETRREMOTE,

	CMD_LIST_SETIIC,
	CMD_LIST_GETIID,
	CMD_LIST_SETDAC,
	CMD_LIST_GETDAC,
	CMD_LIST_SETHUB,
	CMD_LIST_GETHUB,

	CMD_LIST_SETGROUP,
	CMD_LIST_GETGROUP,
	CMD_LIST_SETMANUALTOMEMORY,
	CMD_LIST_SETREMOTETOMEMORY,

	CMD_LIST_GETADC,

	CMD_LIST_HELP,
	CMD_LIST_END
} CMD_LIST;

typedef struct {
	CMD_LIST list;
	UCHAR command[30];
} CMD_LIST_INFO;

CMD_LIST_INFO CMD_CommandList[] = 
{
	{CMD_LIST_RESTART,			"restart"},
	{CMD_LIST_REBOOT,			"reboot"},
	{CMD_LIST_RESET,			"reset"},
	{CMD_LIST_MODULEINFO,		"getmoduleinfo"},
	{CMD_LIST_VERSIONINFO,		"getversioninfo"},
	{CMD_LIST_SETEEPROM,		"seteeprom"},
	{CMD_LIST_GETEEPROM,		"geteeprom"},
	{CMD_LIST_SETEEDUMP,		"seteedump"},
	{CMD_LIST_GETEEDUMP,		"geteedump"},

	{CMD_LIST_SETBRIGHT,		"setbright"},
	{CMD_LIST_GETBRIGHT,		"getbright"},
	{CMD_LIST_SETON,			"seton"},
	{CMD_LIST_SETONEX,			"setonex"},
	{CMD_LIST_GETONEX,			"getonex"},
	{CMD_LIST_SETBANDWIDTH,		"setbandwidth"},
	{CMD_LIST_GETBANDWIDTH,		"getbandwidth"},
	{CMD_LIST_SETINDEX,			"setindex"},
	{CMD_LIST_GETINDEX,			"getindex"},

	{CMD_LIST_CMDSAVE,			"save"},
	{CMD_LIST_CMDLOAD,			"load"},
	{CMD_LIST_CMDUPDATE,		"update"},
	{CMD_LIST_CMDRESTORE,		"restore"},

	{CMD_LIST_SETBURNIN,		"setburnin"},
	{CMD_LIST_SETSPEED,			"setburninspeed"},
	{CMD_LIST_SETBAUD,			"setbaudrate"},
	{CMD_LIST_GETBAUD,			"getbaudrate"},
	{CMD_LIST_SETMODE,			"setmode"},

	{CMD_LIST_GETMANUAL,		"getmanual"},
	{CMD_LIST_GETRREMOTE,		"getremote"},

	{CMD_LIST_SETIIC,			"setiic"},
	{CMD_LIST_GETIID,			"getiic"},
	{CMD_LIST_SETDAC,			"setdac"},
	{CMD_LIST_GETDAC,			"getdac"},
	{CMD_LIST_SETHUB,			"sethub"},
	{CMD_LIST_GETHUB,			"gethub"},

	{CMD_LIST_SETGROUP,			"setgroup"},
	{CMD_LIST_GETGROUP,			"getgroup"},
	{CMD_LIST_SETMANUALTOMEMORY,"setmanualtomemory"},
	{CMD_LIST_SETREMOTETOMEMORY,"setremotetomemory"},

	{CMD_LIST_GETADC,			"getadc"},

	{CMD_LIST_HELP,				"help"},
	{CMD_LIST_END,NULL},
};

Protocol32CH::Protocol32CH()
{ 
	m_CallBack	= NULL;
	m_hWnd		= NULL;
	m_pBuff		= new char[RS232_BUFF_SIZE];
	m_CmdBuff	= new char[RS232_BUFF_SIZE];
	m_pComm32CH	= new CRs232CommSTD;
	m_nCmdRxCnt = 0;
	memset(m_pBuff,NULL,RS232_BUFF_SIZE);
	memset(m_CmdBuff,NULL,RS232_BUFF_SIZE);
	memset(&m_Info,NULL,sizeof(m_Info));
}

Protocol32CH::~Protocol32CH()
{ 
	if(m_pComm32CH) delete ((CRs232CommSTD *)m_pComm32CH);
	if(m_pBuff) delete m_pBuff;
	if(m_CmdBuff) delete m_CmdBuff;
	m_pComm32CH	= NULL;
	m_hWnd	= NULL;
	m_CallBack	= NULL;
}

int Protocol32CH::FindCommand(char *pBuff)
{
	int i;
	for(i=0; 1; i++)
	{
		if(CMD_CommandList[i].list==CMD_LIST_END) return 255;
		if(!strcmp(( char *)pBuff,(char *)CMD_CommandList[i].command)) return CMD_CommandList[i].list;
	}
}

bool Protocol32CH::CommandRx(void)
{
	int size=((CRs232CommSTD *)m_pComm32CH)->GetSize();
	for(int i=0;i<size;i++) 
	{
		((CRs232CommSTD *)m_pComm32CH)->GetByte((BYTE *)&m_pBuff[m_nCmdRxCnt]);
		TRACE("%c",m_pBuff[m_nCmdRxCnt]);
		m_nCmdRxCnt++;
		if(m_pBuff[m_nCmdRxCnt-1]==ASCII_CARRIGE_RETURN) 
		{
			m_pBuff[m_nCmdRxCnt-1] = NULL;
			TRACE("%s\n",m_pBuff);
		}
		else if(m_pBuff[m_nCmdRxCnt-1]==ASCII_NEWLINE)	
		{
			m_pBuff[m_nCmdRxCnt-1] = NULL;
			TRACE("%s\n",m_pBuff);
			m_nCmdRxCnt = 0;
			CommandDecoding(m_pBuff);
		}
		else if(m_nCmdRxCnt>1000) m_nCmdRxCnt = 0;
	}
	return true;
}

bool Protocol32CH::CommandDecoding(char *pBuff)
{
	char command[5][64];

	sscanf_s((char *)pBuff,"%s %s %s %s %s",command[0],command[1],command[2],command[3],command[4]);
	switch(FindCommand(command[0]))
	{
		case CMD_LIST_GETBRIGHT:
			if(atoi(command[1])<0 || atoi(command[1])>31) break;
			m_Info.nRemoteBrightness[atoi(command[1])] = atoi(command[2]);
			break;
		case CMD_LIST_GETBANDWIDTH:
			m_Info.nBandWidthMin = (float)atof(command[1]);
			m_Info.nBandWidthMax = (float)atof(command[2]);
			break;
		case CMD_LIST_GETINDEX:
			m_Info.nMemoryIndex = atoi(command[1]);
			break;
		case CMD_LIST_GETONEX:
			m_Info.nOnOffStatusEx = strtoul(command[1],NULL,16);
			break;
	}

	if(m_CallBack) 
	{
		m_CallBack(m_Info.nPort,pBuff);
		// m_CallBack(m_Info.nPort,(char *)&m_Info);
	}
	if(m_hWnd) PostMessage(m_hWnd,WM_COMM_READ,m_Info.nPort,(LPARAM)&m_Info);
	return true;
}

int Protocol32CH::Create(int nPort,HWND hWnd)
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm32CH)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		for(i=0;i<LIGHT_MODULE32_CHANNEL;i++) SetBrightness(i,0);
		m_Info.nPort	= nPort;
		m_hWnd			= hWnd;
	}
	return ret;
}

int Protocol32CH::Create(int nPort,void (*callback)(int nPort,char *pBuff))
{
	int i,ret;
	char buff[10];
	sprintf_s(buff,"COM%d",nPort);
	ret = ((CRs232CommSTD *)m_pComm32CH)->Create(buff,RS232_BAUDRATE_38400,nPort,false,CallBackFunc);
	if(ret)
	{
		m_Info.nPort	= nPort;
		m_CallBack		= callback;
		for(i=0;i<LIGHT_MODULE32_CHANNEL;i++) SetBrightness(i,0);
	}
	return ret;
}

void Protocol32CH::SetBrightness(int channel,int brightness)
{
	char pBuff[50];
	//sprintf_s(pBuff,"setbright %d %d%c",channel,brightness,0x0A);
	sprintf_s(pBuff,"# %d %d%c",channel,brightness,0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	Sleep(PROTOCOL_32CH_WAIT);
	m_Info.nRemoteBrightness[channel] = brightness;
}

void Protocol32CH::GetBrightness(int channel)
{
	char pBuff[50];
	//sprintf_s(pBuff,"getbright %d%c",channel,0x0A);
	sprintf_s(pBuff,"$ %d%c",channel,0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::SetOnOff(int channel,int onoff)
{
	char pBuff[50];
	sprintf_s(pBuff,"seton %d %d%c",channel,onoff,0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
	m_Info.nOnOffStatus[channel] = onoff;
}

void Protocol32CH::SetOnOffEx(ULONG onoff)
{
	char pBuff[50];
	sprintf_s(pBuff,"setonex 0x%08X%c",onoff,0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT*5);
	m_Info.nOnOffStatusEx = onoff;
}

void Protocol32CH::GetOnOffEx(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"getonex%c",0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::RequestCurrentStatus(void)
{
	char pBuff[50];
	for(int i=0;i<32;i++)
	{
		sprintf_s(pBuff,"getbright %d%c",i,0x0A);
		((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
		Sleep(PROTOCOL_32CH_WAIT);
		TRACE("%s\n",pBuff);
	}
}

void Protocol32CH::SaveCurrentStatus(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"save%c",0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}


void Protocol32CH::ReportBrightness(int channel)
{
	char pBuff[50];
	sprintf_s(pBuff,"brightness %d %d%c",channel,m_Info.nRemoteBrightness[channel],0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::SetBandWidth(float min, float max)
{
	char pBuff[50];
	sprintf_s(pBuff,"setbandwidth %f %f%c", min,max,0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::GetBandWidth(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"getbandwidth%c", 0x0D);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::GetMemIndex(void)
{
	char pBuff[50];
	sprintf_s(pBuff,"getindex%c", 0x0D);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::SaveEx(int index)
{
	char pBuff[50];
	sprintf_s(pBuff,"save %d%c",index, 0x0D);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::LoadEx(int index)
{
	char pBuff[50];
	sprintf_s(pBuff,"load %d%c",index, 0x0D);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::FactoryReset()
{
	char pBuff[50];
	sprintf_s(pBuff,"factoryreset%c",0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::Restart()
{
	char pBuff[50];
	sprintf_s(pBuff,"restart%c",0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}

void Protocol32CH::UpdateFlip()
{
	char pBuff[50];
	sprintf_s(pBuff,"updateflip%c",0x0A);
	((CRs232CommSTD *)m_pComm32CH)->WriteData((unsigned char *)pBuff,strlen(pBuff));
	TRACE("%s\n",pBuff);
	Sleep(PROTOCOL_32CH_WAIT);
}
