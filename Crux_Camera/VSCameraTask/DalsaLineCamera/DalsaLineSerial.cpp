#include "stdafx.h"
#include "DalsaLineSerial.h"
#include "define.h"

CDalsaLineSerial::CDalsaLineSerial(void)
{
	m_hComm=NULL;
}

CDalsaLineSerial::~CDalsaLineSerial(void)
{
	CloseComPort();
}
bool CDalsaLineSerial::comSerialWrite(BYTE *pBuff, UINT32& Buffersize)
{
	unsigned long length;
	BOOL bRet = WriteFile( m_hComm, pBuff, Buffersize, &length, NULL);
	bool ecode =	(bRet != FALSE ? true:false);
	Buffersize=length;
	return ecode;
}

bool CDalsaLineSerial::comSerialRead(BYTE *pBuff, UINT32& Buffersize)
{
	unsigned long length;
	BOOL bRet = ReadFile(m_hComm, pBuff, Buffersize, &length, NULL);
	bool ecode = (bRet != 0 ? true:false);
	Buffersize=length;
	return ecode;
}

int CDalsaLineSerial::SetCommand(char* Command,UINT32 TimeOut)
{
	if(m_hComm==NULL)return -21000;

	// Send Command routine

	char *tempstr = NULL;
	UINT32 len  = (UINT32)(strlen(Command) + 1);
	tempstr		  = new char[len];

	memset(tempstr,0,len);
	memcpy(tempstr,Command,len-1);
	tempstr[len-1]   = 0x0d;
	UINT32 tsize = len;

	bool bRet = comSerialWrite((BYTE *)tempstr,len);
	if(bRet == false)	
	{
		//AfxMessageBox(_T("Error"));
		delete []  tempstr;
		return -22000;
	}

	delete []  tempstr;

	// read Ack message

	unsigned char buf;
	UINT32 numBytes=1;
	CString rxData;

	UINT32 dwStart = GetTickCount();

	while (GetTickCount() - dwStart < TimeOut)
	{
		numBytes=1;		

		while(1)
		{
			bool bRet = comSerialRead(&buf,numBytes);
			if(bRet == false || numBytes == 0)
				break;

			rxData+=buf;
			if(buf==0x4)
				break;

		}
		if(buf==0x4)
			break;
		Sleep(1);
	}
	
	m_strReturnMsg = rxData;

	if(rxData.GetLength()==0)
		return -22000; // no retuern string 
	else if(rxData.Find(_T("OK"),0)>=0 || rxData.Find(PCH2CSTR(Command),0)>=0 || (bRet == true && numBytes != 0))	
		return 0; // ok
	else if(rxData.Find(_T("Error"),0)>=0)
		return -1; // command Error 
	else 
		return -2;

}


int CDalsaLineSerial::OpenComPort(UINT32 BaudRate, CString Portname)
{
	if (m_hComm != NULL)
		CloseComPort();
	Portname.Insert(0,_T("\\\\.\\"));
	COMMTIMEOUTS	timeouts;
	DCB				dcb;

	DWORD	dwBaud		=	BaudRate;
	BYTE	byData		=	8;
	BYTE	byStop		=	ONESTOPBIT;
	BYTE	byParity	=	NOPARITY;

	m_hComm = CreateFile( Portname, 
		GENERIC_READ | GENERIC_WRITE, 0, NULL,
		OPEN_EXISTING,	0, 
		NULL);

	//--> 포트 열기에 실해하면..
	if (m_hComm == (HANDLE) -1)
	{
		m_hComm = NULL;
		return -21000;
	}

	SetupComm( m_hComm, 4096, 4096);	

	timeouts.ReadIntervalTimeout = 0xFFFFFFFF;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;

	timeouts.WriteTotalTimeoutMultiplier = 2*CBR_9600 / dwBaud;
	timeouts.WriteTotalTimeoutConstant = 0;

	SetCommTimeouts( m_hComm, &timeouts);

	// dcb 설정.... 포트의 실제적인..제어를 담당하는 DCB 구조체값 셋팅..
	dcb.DCBlength = sizeof(DCB);

	//--> 현재 설정된 값 중에서..
	GetCommState( m_hComm, &dcb);	

	//--> 보드레이트를 바꾸고..
	dcb.BaudRate = dwBaud;

	//--> Data 8 Bit
	dcb.ByteSize = byData;

	//--> Noparity
	dcb.Parity = byParity;

	//--> 1 Stop Bit
	dcb.StopBits = byStop;

	//--> 포트를 재..설정값으로.. 설정해보고..
	if( !SetCommState( m_hComm, &dcb) )	
	{
		m_hComm = NULL;
		return -21000;
	}

	return 0;
}


void  CDalsaLineSerial::CloseComPort()
{
	if(m_hComm!=NULL)CloseHandle(m_hComm);
	m_hComm=NULL;
}
