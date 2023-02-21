// Rs232CommSTD.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Rs232CommSTD.h"

// BOOL APIENTRY DllMain( HANDLE hModule, 
//                        DWORD  ul_reason_for_call, 
//                        LPVOID lpReserved
// 					 )
// {
//     switch (ul_reason_for_call)
// 	{
// 		case DLL_PROCESS_ATTACH:
// 		case DLL_THREAD_ATTACH:
// 		case DLL_THREAD_DETACH:
// 		case DLL_PROCESS_DETACH:
// 			break;
//     }
//     return TRUE;
// }

/*
// This is an example of an exported variable
RS232COMMSTD_API int nRs232CommSTD=0;

// This is an example of an exported function.
RS232COMMSTD_API int fnRs232CommSTD(void)
{
	return 42;
}*/

// This is the constructor of a class that has been exported.
// see Rs232CommSTD.h for the class definition
CRs232CommSTD::CRs232CommSTD()
{ 
	m_ptheCommThread = new CCommThread;
}
CRs232CommSTD::~CRs232CommSTD()
{
	if(m_ptheCommThread) delete m_ptheCommThread;
	m_ptheCommThread = NULL;
}

DWORD CRs232CommSTD::WriteData(unsigned char *pBuff,DWORD nToWrite)
{
	return m_ptheCommThread->WriteComm(pBuff,nToWrite);
}

BOOL CRs232CommSTD::IsOpen()
{
	return m_ptheCommThread->m_bConnected;
}

void CRs232CommSTD::GetByte(unsigned char *pBuff)
{
	m_ptheCommThread->m_QueueRead.GetByte(pBuff);
}

int CRs232CommSTD::GetSize()
{
	return m_ptheCommThread->m_QueueRead.GetSize();
}

BOOL CRs232CommSTD::Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(void))
{
	if(m_ptheCommThread->OpenPort(pPortName, nBaud, wPortID,byParity,callback)==FALSE)
	{
		delete m_ptheCommThread;
		m_ptheCommThread = NULL;
		return false;
	}
	return true;
}

BOOL CRs232CommSTD::Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(int nPort,char *pBuff))
{
	if(m_ptheCommThread->OpenPort(pPortName, nBaud, wPortID,byParity,callback)==FALSE)
	{
		delete m_ptheCommThread;
		m_ptheCommThread = NULL;
		return false;
	}
	return true;
}

BOOL CRs232CommSTD::Create(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,HWND hWnd)
{
	if(m_ptheCommThread->OpenPort(pPortName, nBaud, wPortID,byParity,hWnd)==FALSE)
	{
		delete m_ptheCommThread;
		m_ptheCommThread = NULL;
		return false;
	}
	return true;
}
