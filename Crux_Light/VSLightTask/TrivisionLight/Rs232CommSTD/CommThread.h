// CommThread.h: interface for the CCommThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMTHREAD_H__4FDECFF7_C27C_4442_B8DF_B13C6FED4D9E__INCLUDED_)
#define AFX_COMMTHREAD_H__4FDECFF7_C27C_4442_B8DF_B13C6FED4D9E__INCLUDED_

#include "Queue.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_COMM_READ (WM_USER+100)

#define ASCII_LF	0x0a
#define ASCII_CR	0x0d
#define ASCII_XON	0x11
#define ASCII_XOFF	0x13
#define CR			0x0D

#define BAUDRATE_2400		0x00
#define BAUDRATE_4800		0x01
#define BAUDRATE_9600		0x02
#define BAUDRATE_19200		0x03
#define BAUDRATE_38400		0x04
#define BAUDRATE_115200		0x05

class CCommThread  
{
public:
	HWND m_hCommWnd;
	void (*m_CallBackFuncPort)(int nPort);
	void (*m_CallBackFunc)(void);
	BOOL OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,HWND hCommWnd);
	BOOL OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(int nPort));
	BOOL OpenPort(char *pPortName, int nBaud, WORD wPortID,BYTE byParity,void (*callback)(void));
	DWORD ReadComm(BYTE *pBuff,DWORD nToRead);
	DWORD WriteComm(BYTE *pBuff,DWORD nToWrite);
	BOOL ClosePort();
	BOOL m_bConnected;
	HANDLE m_hComm;
	char m_pPortName[20];
	OVERLAPPED m_osRead,m_osWrite;
	CCommThread();
	HANDLE m_hThreadWatchComm;
	WORD m_wPortID;
	CQueue m_QueueRead;
	virtual ~CCommThread();
};

DWORD ThreadWatchComm(CCommThread* pComm);

#endif // !defined(AFX_COMMTHREAD_H__4FDECFF7_C27C_4442_B8DF_B13C6FED4D9E__INCLUDED_)
