// CommThread.cpp: implementation of the CCommThread class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CommThread.h"
#include "stdio.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCommThread::CCommThread()
{
	m_CallBackFunc = NULL;
	m_CallBackFuncPort = NULL;
	m_hCommWnd = NULL;
	m_bConnected = FALSE;
	m_wPortID	 = 0;
	m_hThreadWatchComm = NULL;
}

CCommThread::~CCommThread()
{
	if(m_hThreadWatchComm) CloseHandle(m_hThreadWatchComm);
	if(m_bConnected) ClosePort();
	while(m_bConnected);
	m_CallBackFunc = NULL;
	m_CallBackFuncPort = NULL;
	m_hCommWnd = NULL;
}

BOOL CCommThread::OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,HWND hCommWnd)
{
	COMMTIMEOUTS timeouts;
	DCB		dcb;
	DWORD	dwThreadID;
	DWORD	dwbaud;

	if(wPortID>4) sprintf_s(pPortName,sizeof(pPortName),"\\\\.\\COM%d",wPortID);
	
	switch(nBaud)
	{
 		case BAUDRATE_2400:		dwbaud = 2400;	break;
 		case BAUDRATE_4800:		dwbaud = 4800;	break;
 		case BAUDRATE_9600:		dwbaud = 9600;	break;
 		case BAUDRATE_19200:	dwbaud = 19200;	break;
 		case BAUDRATE_38400:	dwbaud = 38400;	break;
 		case BAUDRATE_115200:	dwbaud = 115200;break;
	}

	m_hCommWnd = hCommWnd;
	m_wPortID = wPortID;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;

	if(!(m_osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if(!(m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	strcpy_s(m_pPortName,pPortName);
	m_hComm = CreateFile(m_pPortName,GENERIC_READ | GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if(m_hComm==(HANDLE)-1) return FALSE;
	SetCommMask(m_hComm,EV_RXCHAR);
	SetupComm(m_hComm,38400,38400);
	PurgeComm(m_hComm,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600/dwbaud;
	SetCommTimeouts(m_hComm,&timeouts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm,&dcb);
	dcb.BaudRate = dwbaud;
	dcb.ByteSize = 8;
	dcb.Parity = byParity;  //0:none 1: odd 2: even 3: mark 4: space
	dcb.StopBits = 0;		//1 Stop bit
	dcb.fInX = dcb.fOutX = FALSE;	//XON/OFF Control
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if(!SetCommState(m_hComm,&dcb)) return FALSE;
	m_bConnected = TRUE;
	m_hThreadWatchComm = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadWatchComm,this,0,&dwThreadID);

	if(!m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	return TRUE;
}

BOOL CCommThread::OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(int nPort))
{
	COMMTIMEOUTS timeouts;
	DCB		dcb;
	DWORD	dwThreadID;
	DWORD	dwbaud;

	if(wPortID>4) sprintf_s(pPortName,sizeof(pPortName),"\\\\.\\COM%d",wPortID);
	
	switch(nBaud)
	{
 		case BAUDRATE_2400:		dwbaud = 2400;	break;
 		case BAUDRATE_4800:		dwbaud = 4800;	break;
 		case BAUDRATE_9600:		dwbaud = 9600;	break;
 		case BAUDRATE_19200:	dwbaud = 19200;	break;
 		case BAUDRATE_38400:	dwbaud = 38400;	break;
 		case BAUDRATE_115200:	dwbaud = 115200;break;
	}

	m_CallBackFuncPort = callback;
	m_wPortID = wPortID;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;

	if(!(m_osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if(!(m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	strcpy_s(m_pPortName,pPortName);
	m_hComm = CreateFile(m_pPortName,GENERIC_READ | GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if(m_hComm==(HANDLE)-1) return FALSE;
	SetCommMask(m_hComm,EV_RXCHAR);
	SetupComm(m_hComm,38400,38400);
	PurgeComm(m_hComm,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600/dwbaud;
	SetCommTimeouts(m_hComm,&timeouts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm,&dcb);
	dcb.BaudRate = dwbaud;
	dcb.ByteSize = 8;
	dcb.Parity = byParity;  //0:none 1: odd 2: even 3: mark 4: space
	dcb.StopBits = 0;		//1 Stop bit
	dcb.fInX = dcb.fOutX = FALSE;	//XON/OFF Control
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if(!SetCommState(m_hComm,&dcb)) return FALSE;
	m_bConnected = TRUE;
	m_hThreadWatchComm = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadWatchComm,this,0,&dwThreadID);

	if(!m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	return TRUE;
}

BOOL CCommThread::OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(void))
{
	COMMTIMEOUTS timeouts;
	DCB		dcb;
	DWORD	dwThreadID;
	DWORD	dwbaud;

	if(wPortID>4) sprintf_s(pPortName,sizeof(pPortName),"\\\\.\\COM%d",wPortID);
	
	switch(nBaud)
	{
 		case BAUDRATE_2400:		dwbaud = 2400;	break;
 		case BAUDRATE_4800:		dwbaud = 4800;	break;
 		case BAUDRATE_9600:		dwbaud = 9600;	break;
 		case BAUDRATE_19200:	dwbaud = 19200;	break;
 		case BAUDRATE_38400:	dwbaud = 38400;	break;
 		case BAUDRATE_115200:	dwbaud = 115200;break;
	}

	m_CallBackFunc = callback;
	m_wPortID = wPortID;
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;

	if(!(m_osRead.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if(!(m_osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	strcpy_s(m_pPortName,pPortName);
	m_hComm = CreateFile(m_pPortName,GENERIC_READ | GENERIC_WRITE,0,NULL,
		OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if(m_hComm==(HANDLE)-1) return FALSE;
	SetCommMask(m_hComm,EV_RXCHAR);
	SetupComm(m_hComm,38400,38400);
	PurgeComm(m_hComm,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600/dwbaud;
	SetCommTimeouts(m_hComm,&timeouts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm,&dcb);
	dcb.BaudRate = dwbaud;
	dcb.ByteSize = 8;
	dcb.Parity = byParity;  //0:none 1: odd 2: even 3: mark 4: space
	dcb.StopBits = 0;		//1 Stop bit
	dcb.fInX = dcb.fOutX = FALSE;	//XON/OFF Control
	dcb.XonChar = ASCII_XON;
	dcb.XoffChar = ASCII_XOFF;
	dcb.XonLim = 100;
	dcb.XoffLim = 100;

	if(!SetCommState(m_hComm,&dcb)) return FALSE;
	m_bConnected = TRUE;
	m_hThreadWatchComm = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadWatchComm,this,0,&dwThreadID);

	if(!m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	return TRUE;
}

BOOL CCommThread::ClosePort()
{
	SetCommMask(m_hComm,0);
	PurgeComm(m_hComm,PURGE_TXABORT|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_RXCLEAR);
	CloseHandle(m_hComm);
	m_bConnected = FALSE;
	Sleep(2000);
	return FALSE;
}

DWORD CCommThread::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD dwWritten,dwError,dwErrorFlags;
	COMSTAT comstat;
	if(!WriteFile(m_hComm,pBuff,nToWrite,&dwWritten,&m_osWrite))
	{
		if(GetLastError() == ERROR_IO_PENDING)
		{
			while(!GetOverlappedResult(m_hComm,&m_osWrite,&dwWritten,TRUE))
			{
				dwError = GetLastError();
				if(dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(m_hComm,&dwErrorFlags,&comstat);
					break;
				}
			}
		}
		else
		{
			dwWritten = 0;
			ClearCommError(m_hComm,&dwErrorFlags,&comstat);
		}
	}
	return dwWritten;
}

DWORD CCommThread::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD dwRead,dwError,dwErrorFlags;
	COMSTAT comstat;
	ClearCommError(m_hComm,&dwErrorFlags,&comstat);
	dwRead = comstat.cbInQue;
	if(dwRead>0)
	{
		if(!ReadFile(m_hComm,pBuff,nToRead,&dwRead,&m_osRead))
		{
			if(GetLastError() == ERROR_IO_PENDING)
			{
				while(!GetOverlappedResult(m_hComm,&m_osRead,&dwRead,TRUE))
				{
					dwError = GetLastError();
					if(dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError(m_hComm,&dwErrorFlags,&comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError(m_hComm,&dwErrorFlags,&comstat);
			}
		}
	}
	return dwRead;
}

DWORD ThreadWatchComm(CCommThread* pComm)
{
	int rxByte;
	DWORD dwEvent;
	OVERLAPPED os;
	BOOL bOk = TRUE;
	BYTE buff[BUFF_SIZE];
	DWORD dwRead;
	memset(&os,0,sizeof(OVERLAPPED));
	if(!(os.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL))) bOk = FALSE;
	if(!SetCommMask(pComm->m_hComm,EV_RXCHAR)) bOk = FALSE;
	if(!bOk)
	{
		MessageBox(NULL,"Error while creating ThreadWatchComm",NULL,MB_OK);
		return FALSE;
	}
	while(pComm->m_bConnected)
	{
		dwEvent = 0;
		WaitCommEvent(pComm->m_hComm,&dwEvent,NULL);
		if((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			do
			{
				dwRead = pComm->ReadComm(buff,BUFF_SIZE);
				rxByte =pComm->m_QueueRead.GetSize();
				if(BUFF_SIZE - rxByte > (int)dwRead)
				{
					for(WORD i=0;i<dwRead;i++)
						pComm->m_QueueRead.PutByte(buff[i]);
				}
				else
					MessageBox(NULL,"m_QueueRead FULL!",NULL,MB_OK);
			} while(dwRead);
			if(pComm->m_QueueRead.GetSize() && pComm->m_hCommWnd) ::PostMessage(pComm->m_hCommWnd,WM_COMM_READ,pComm->m_wPortID,0);
			if(pComm->m_QueueRead.GetSize() && pComm->m_CallBackFuncPort) pComm->m_CallBackFuncPort(pComm->m_wPortID);
			if(pComm->m_QueueRead.GetSize() && pComm->m_CallBackFunc) pComm->m_CallBackFunc();
		}
	}
	CloseHandle(os.hEvent);
	pComm->m_hThreadWatchComm = NULL;
	return TRUE;
}
