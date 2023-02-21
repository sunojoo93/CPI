// ProtocolLibRegLFine.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Protocol.h"

#define LIBRARY_VERSION	0x5000
#define DEBUG_EXPORT

#include "ProtocolLibRegLFine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#define DEBUGMSG(x) OutputDebugString(x)
#else
#define DEBUGMSG(x) NULL
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CProtocolLibRegLFineApp

void			*g_pLightModule[MAX_NUMBEROF_HANDLE];
MODULE_INFO		g_ModuleInfo[MAX_NUMBEROF_HANDLE];

extern "C" __declspec(dllexport) int Init(void);
extern "C" __declspec(dllexport) int OpenPort(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], void (*callback)(int nPort,char *pBuff));
extern "C" __declspec(dllexport) int OpenPortEx(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], HWND hWnd);
extern "C" __declspec(dllexport) int SetBrightness(int port, int ctrlID, UINT LightVal[], int channelCnt);		// All Channels light on 180514 YSS
extern "C" __declspec(dllexport) int GetBrightness(int port,int channel);
extern "C" __declspec(dllexport) int ClosePort(int port);
extern "C" __declspec(dllexport) int ClosePortEx(int port);
extern "C" __declspec(dllexport) int CloseAll(void);
extern "C" __declspec(dllexport) int SaveCurrentStatus(int port);
extern "C" __declspec(dllexport) int SetOnOff(int port,int ctrlID,int onoff, int channelCnt);			// All Channels setonoff 180514 YSS
extern "C" __declspec(dllexport) int SetOnOffEx(int port, int ctrlID, ULONG onoff, int channelCnt);		// All Channels setonoff 180514 YSS
extern "C" __declspec(dllexport) void GetLibraryVersion(USHORT *version);
extern "C" __declspec(dllexport) int GetError(int channel);
// extern "C" __declspec(dllexport) int GetC(int nPort, int nChannel);

int FindPort(int port)
{
	int i = 0;
	for(i=0;i<MAX_NUMBEROF_HANDLE;i++) if(g_ModuleInfo[i].port==port) break;
	return i;
}

void CallBackFunc(int nPort)
{
	int i= FindPort(nPort);
	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			((Protocol *)g_pLightModule[i])->CommandRx();
			break;
		default:
			break;
	}
}

int Init(void)
{
	for(int i=0;i<MAX_NUMBEROF_HANDLE;i++) 
	{
		g_ModuleInfo[i].moduleType	= -1;
		g_ModuleInfo[i].port		= -1;
		g_ModuleInfo[i].nBaudRate	= -1;
		g_pLightModule[i] = NULL;
	}
	return CloseAll();
}

int OpenPort(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], void (*callback)(int nPort,char *pBuff))
{
	if(port<=0)
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	int i = 0;
	for(i=0;i<MAX_NUMBEROF_HANDLE;i++)	if(!g_pLightModule[i]) break;
	if(i==MAX_NUMBEROF_HANDLE)
	{
		DEBUGMSG("Invalid HANDLE,Overflow resource!!");
		return INVALID_HANDLE;
	}
	
	int nTotalChCnt = 0;
	int nMaxChCnt = 0;
	for (int nIndex = 0; nIndex < (int)controlCnt; nIndex++)
	{
		nTotalChCnt += (int)CtrlChannelCnt[nIndex];

		if (nIndex == 0)	nMaxChCnt = CtrlChannelCnt[0];
		else				nMaxChCnt = nMaxChCnt > (int)CtrlChannelCnt[nIndex] ? nMaxChCnt : CtrlChannelCnt[nIndex];
	}

	g_ModuleInfo[i].moduleType	= nMaxChCnt;		// 모듈 타입 정의(한 컨트롤러당 제어 가능한 최대 채널 개수)
	g_ModuleInfo[i].port		= port;
	g_ModuleInfo[i].nBaudRate	= nBaud;

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			g_pLightModule[i] = new Protocol;
			if(!((Protocol *)g_pLightModule[i])->Create(g_ModuleInfo[i].port, (int)controlCnt, CtrlChannelCnt, g_ModuleInfo[i].nBaudRate, g_ModuleInfo[i].moduleType, callback))	// 컨트롤러 갯수 추가	180515 YSS
			{
				delete (Protocol *)g_pLightModule[i];
				g_pLightModule[i]= NULL;
				g_ModuleInfo[i].moduleType	= -1;
				g_ModuleInfo[i].port		= -1;
				DEBUGMSG("Open failed!!,Check your number of port.");
				return OPEN_ERR;
			}
			break;
		// 3채널 외에 기타 타입 추가될 경우 여기에 추가해서 처리해야 함.


		default:
			g_ModuleInfo[i].moduleType	= -1;
			g_ModuleInfo[i].port		= -1;
			DEBUGMSG("Invalid module!!,Check your module option.");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int OpenPortEx(int port, UINT controlCnt, int nBaud, UINT CtrlChannelCnt[], HWND wnd)
{
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	int i = 0;
	for(i=0;i<MAX_NUMBEROF_HANDLE;i++)	if(!g_pLightModule[i]) break;
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Invalid HANDLE,Overflow resource!!");
		return INVALID_HANDLE;
	}

	int nTotalChCnt = 0;
	int nMaxChCnt = 0;
	for (int nIndex = 0; nIndex < (int)controlCnt; nIndex++)
	{
		nTotalChCnt += (int)CtrlChannelCnt[nIndex];

		if (nIndex == 0)	nMaxChCnt = CtrlChannelCnt[0];
		else				nMaxChCnt = nMaxChCnt > (int)CtrlChannelCnt[nIndex] ? nMaxChCnt : CtrlChannelCnt[nIndex];
	}

	g_ModuleInfo[i].moduleType	= nMaxChCnt;		// 모듈 타입 정의(한 컨트롤러당 제어 가능한 최대 채널 개수)
	g_ModuleInfo[i].port		= port;
	g_ModuleInfo[i].nBaudRate	= nBaud;

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			g_pLightModule[i] = new Protocol;
			if(!((Protocol *)g_pLightModule[i])->Create(g_ModuleInfo[i].port, (int)controlCnt, CtrlChannelCnt, g_ModuleInfo[i].nBaudRate, g_ModuleInfo[i].moduleType,wnd))
			{
				delete (Protocol *)g_pLightModule[i];
				g_pLightModule[i]= NULL;
				g_ModuleInfo[i].moduleType	= -1;
				g_ModuleInfo[i].port		= -1;
				DEBUGMSG("Open failed!!,Check your number of port.");
				return OPEN_ERR;
			}
			break;
		// 3채널 외에 기타 타입 추가될 경우 여기에 추가해서 처리해야 함.


		default:
			g_ModuleInfo[i].moduleType	= -1;
			g_ModuleInfo[i].port		= -1;
			DEBUGMSG("Invalid module!!,Check your module option.");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int ClosePortEx(int port)
{
	int i;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int ClosePort(int port)
{
	int i;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			delete (Protocol *)g_pLightModule[i];
			g_pLightModule[i] = NULL;
			g_ModuleInfo[i].port=-1;
			g_ModuleInfo[i].moduleType=-1;
			g_ModuleInfo[i].nBaudRate = -1;
			DEBUGMSG("Close complete!");
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int CloseAll(void)
{
	for(int i=0;i<MAX_NUMBEROF_HANDLE;i++)
	{
		if(g_pLightModule[i])
		{
			switch(g_ModuleInfo[i].moduleType)
			{
				case MODULE_3CH:
					delete (Protocol *)g_pLightModule[i];
					break;
				default:
					delete g_pLightModule[i];
					break;
			}
		}
		g_pLightModule[i]			= NULL;
		g_ModuleInfo[i].moduleType	= -1;
		g_ModuleInfo[i].port		= -1;
		g_ModuleInfo[i].nBaudRate	= -1;
	}
	return SUCCESS;
}

int SetBrightness(int port, int ctrlID, UINT LightVal[], int channelCnt)
{
	int i;
	if(port <= 0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i = FindPort(port);
	if(i == MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
// 			if(channel<=0 || channel >g_ModuleInfo[i].moduleType) 
// 			{
// 				DEBUGMSG("Invalid Parameter!!,channel<0 or channel>LIGHT_MODULE_CHANNEL");
// 				return INVALID_PARAMETER;
// 			}
// 			if(brightness<MIN_LIGHT_VALUE || brightness>MAX_LIGHT_VALUE) 
// 			{
// 				DEBUGMSG("Invalid Parameter!!,brightness < 0 or brightness > 1000");
// 				return INVALID_PARAMETER;
// 			}
			((Protocol *)g_pLightModule[i])->SetBrightness(ctrlID, LightVal, channelCnt);
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int GetBrightness(int port,int channel)
{
	int i;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			if(channel<=0 || channel >g_ModuleInfo[i].moduleType) 
			{
				DEBUGMSG("Invalid Parameter!!,channel<0 or channel>LIGHT_MODULE_CHANNEL");
				return INVALID_PARAMETER;
			}
			((Protocol *)g_pLightModule[i])->GetBrightness(channel-1);
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int GetError(int nPort)
{
	int i=FindPort(nPort);
	((Protocol *)g_pLightModule[i])->GetError();
	return SUCCESS;
}
// int GetC(int nPort, int nChannel)
// {
// 	int i=FindPort(nPort);
// 	((Protocol *)g_pLightModule[i])->GetC(nChannel);
// 	return SUCCESS;
// }

int SaveCurrentStatus(int port)
{
	int i;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			((Protocol *)g_pLightModule[i])->SaveCurrentStatus();
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int GetTypeinfo(int port,int *type)
{
	int i;
	if(port<0) return INVALID_PARAMETER;
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) return INVALID_PARAMETER;
	*type = g_ModuleInfo[i].moduleType;
	return SUCCESS;
}

int SetOnOff(int port,int ctrlID,int onoff, int channelCnt)
{
	int i;
	ULONG ulTemp=0;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	if(channelCnt<=0 || channelCnt>ONOFF_NUMBEROF_CHANNEL) 
	{
		DEBUGMSG("Invalid Parameter!!,channel<=0 or channel>49");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			((Protocol *)g_pLightModule[i])->SetOnOff(ctrlID-1,onoff, channelCnt);
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

int SetOnOffEx(int port,int ctrlID,int onoff, int channelCnt)
{
	// SetOnOff 함수와 무슨 차이?


	int i,j;
	if(port<=0) 
	{
		DEBUGMSG("Invalid Parameter!!,port<=0");
		return INVALID_PARAMETER;
	}
	i=FindPort(port);
	if(i==MAX_NUMBEROF_HANDLE) 
	{
		DEBUGMSG("Not found or aleady cloed!!,check your program");
		return INVALID_PARAMETER;
	}

	switch(g_ModuleInfo[i].moduleType)
	{
		case MODULE_3CH:
			for(j=0;j<g_ModuleInfo[i].moduleType;j++) ((Protocol *)g_pLightModule[i])->SetOnOff(j,((onoff>>j)&0x01), channelCnt);
			break;
		default:
			DEBUGMSG("Invalid module!!");
			return INVALID_TYPE;
			break;
	}
	return SUCCESS;
}

void GetLibraryVersion(USHORT *version)
{
	*version = LIBRARY_VERSION;
}