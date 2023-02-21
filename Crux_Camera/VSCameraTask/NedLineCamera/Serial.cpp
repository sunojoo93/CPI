// Serial.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Serial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSerial::CSerial()
{
	m_Connected		= FALSE;
	m_ArriveComplete	= FALSE;
	m_Terminal		= 0xD0;

	m_hThreadWatchComm = NULL;	// 2014.12.20 ljk

	m_hComm = (HANDLE) - 1;
}

CSerial::~CSerial()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSerial message handlers

 BOOL CSerial::OpenPort(BYTE Terminal)
 {
 	// Local ����.
 	COMMTIMEOUTS	timeouts;
 
 	// ���� �ʱ�ȭ
 	if(m_Connected != false) return FALSE;
 	m_wPortID = m_PortID - 1; // COM1-> 0, COM2->1,,,,,
 	m_PortName.Format(_T("COM%d"), m_PortID);
 
 	// overlapped structure ���� �ʱ�ȭ.
 	m_osRead.Offset		= 0;
 	m_osRead.OffsetHigh	= 0;
 	if (!(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;
 	m_osWrite.Offset		= 0;
 	m_osWrite.OffsetHigh	= 0;
 	if (!(m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))	return FALSE;
 	
 	// ��Ʈ ����
 	m_hComm				= CreateFile(m_PortName,
 									GENERIC_READ | GENERIC_WRITE,
 									0,
 									NULL,
 									OPEN_EXISTING,
 									FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
 									NULL);
 	if (m_hComm == (HANDLE) - 1) {
 		DWORD dw;
 		dw = GetLastError();
 		return FALSE;
 	}
 	SetCommMask(m_hComm, EV_RXCHAR);
 	// ��Ʈ ���� ����.
 	/*	
 	// �ؿ� ���� ����ȭ ��Ű�� ���Ͽ� �ʿ���.
 	BYTE InBuffer[BUFF_SIZE];
 	DWORD ret;
 	BYTE OutBuffer[100];
 	DeviceIoControl(m_hComm,IOCTL_SERIAL_LSRMST_INSERT,
  				(void *)InBuffer,BUFF_SIZE,OutBuffer,100,&ret,NULL);
 	*/
 
 	// InQueue, OutQueue ũ�� ����.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);
 	// ��Ʈ ����.
 	PurgeComm(m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
 
 	// timeout ����.
 	timeouts.ReadIntervalTimeout			=	3000;
 	timeouts.ReadTotalTimeoutMultiplier		=	0;
 	timeouts.ReadTotalTimeoutConstant		=	0;
 	timeouts.WriteTotalTimeoutMultiplier	=	0;
 	timeouts.WriteTotalTimeoutConstant		=	0;
 	SetCommTimeouts(m_hComm, &timeouts);
 
 	// dcb ����
 	m_dcb.DCBlength = sizeof(DCB);
 	GetCommState(m_hComm, &m_dcb);	// ���� ���� ����.
 	m_dcb.BaudRate	= 38400;
 	m_dcb.ByteSize	= 8;
 	m_dcb.Parity		= NOPARITY;
 	m_dcb.StopBits	= ONESTOPBIT;
 	m_dcb.fInX		= m_dcb.fOutX = 0;		// Xon, Xoff ���.
 //	dcbb.XonChar	= ASCII_XON;
 //	dcb.XoffChar	= ASCII_XOFF;
 	m_dcb.XonLim		= 2048;
 	m_dcb.XoffLim		= 512;
 	if (!SetCommState(m_hComm, &m_dcb))	return FALSE;	
 	PurgeComm(m_hComm,	PURGE_RXCLEAR);
 	GetCommState(m_hComm, &m_dcb);	// ���� ���� ����.
 	PurgeComm(m_hComm,	PURGE_TXCLEAR);
 	// EV_RXCHAR event ����...
 	SetCommMask(m_hComm, EV_RXCHAR|EV_CTS|EV_DSR|EV_RLSD|EV_ERR|EV_RING|EV_BREAK|EV_RXFLAG|EV_TXEMPTY);
 	// ��Ʈ ���� ������ ����...
 	m_Connected = TRUE;
 	CWinThread* Watch = new CWinThread;
 	Watch = AfxBeginThread((AFX_THREADPROC)ThreadWatchComm, this, THREAD_PRIORITY_NORMAL, 0, 0,NULL);
 	m_hThreadWatchComm = Watch->m_hThread;
 	delete Watch;
 	if(!m_hThreadWatchComm)
	{
 		ClosePort();
 		return FALSE;
 	}
 	PurgeComm(m_hComm,	PURGE_RXCLEAR);
 	return TRUE;
 }

/////////////////////////////////////////////////////////////////////////////
// 
//	��Ʈ�� ����
//	���� ���� �ΰ��� ���� �� ���ִ�. 
//


/////////////////////////////////////////////////////////////////////////////
// 
//	��Ʈ�� ����
//	���� ���� �⺻������ ĳ���� ������ ����Ѵ�. 
//
BOOL CSerial::OpenPort(CString sPortName, DWORD dwBaud, WORD wPortID)
{
	// Local ����.
	COMMTIMEOUTS	timeouts;
//	DWORD			dwThreadID;

	// ���� �ʱ�ȭ
//	Connected = FALSE;
	if(m_Connected != false) return FALSE;
	m_wPortID = wPortID; // COM1-> 0, COM2->1,,,,,

	// overlapped structure ���� �ʱ�ȭ.
	m_osRead.Offset		= 0;
	m_osRead.OffsetHigh = 0;
	if (! (m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;
	m_osWrite.Offset	= 0;
	m_osWrite.OffsetHigh= 0;
	if (! (m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;
	
	// ��Ʈ ����
	m_sPortName			= sPortName;
	m_hComm				= CreateFile(sPortName,
									GENERIC_READ | GENERIC_WRITE,
									0,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
									NULL);
	if (m_hComm == (HANDLE) - 1) return FALSE;
	SetCommMask( m_hComm, EV_RXCHAR);
	// InQueue, OutQueue ũ�� ����.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);
	// ��Ʈ ����.
	PurgeComm( m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout ����.
//	timeouts.ReadIntervalTimeout			=	3000;
	timeouts.ReadIntervalTimeout			=	0;
	timeouts.ReadTotalTimeoutMultiplier		=	0;
	timeouts.ReadTotalTimeoutConstant		=	0;
	timeouts.WriteTotalTimeoutMultiplier	=	0;
	timeouts.WriteTotalTimeoutConstant		=	0;
	SetCommTimeouts( m_hComm, &timeouts);

	// dcb ����
	m_dcb.DCBlength = sizeof(DCB);
	GetCommState( m_hComm, &m_dcb);	// ���� ���� ����.
	m_dcb.BaudRate	= dwBaud;
	m_dcb.ByteSize	= 8;
	m_dcb.Parity	= NOPARITY;
	m_dcb.StopBits	= ONESTOPBIT;
	m_dcb.fInX		= m_dcb.fOutX = 0;		// Xon, Xoff ���.
//	dcb.XonChar = ASCII_XON;
//	dcb.XoffChar = ASCII_XOFF;
	m_dcb.XonLim = 2048;
	m_dcb.XoffLim = 512;
	if(!SetCommState(m_hComm, &m_dcb))	return FALSE;
		
	PurgeComm(m_hComm,	PURGE_RXCLEAR);

	GetCommState(m_hComm, &m_dcb);	// ���� ���� ����.

	PurgeComm(m_hComm,	PURGE_TXCLEAR);
	PurgeComm(m_hComm,	PURGE_RXCLEAR);
	
	PurgeComm(m_hComm,	PURGE_TXCLEAR);
	PurgeComm(m_hComm,	PURGE_RXCLEAR);

	return TRUE;
}

void CSerial::ClosePort()
{
	m_Connected = FALSE;
	if (m_hComm == (HANDLE) - 1) return;
	SetCommMask(m_hComm, 0);
	PurgeComm(m_hComm,	PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	CloseHandle(m_hComm);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// ��Ʈ�� pBuff�� ������ nToWrite��ŭ ����.
// ������ ������ Byte���� �����Ѵ�.

DWORD CSerial::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, /*dwError,*/ dwErrorFlags;
	COMSTAT	comstat;

	m_ArriveComplete = FALSE;
	if (!WriteFile(m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// ���� ���ڰ� ���� �ְų� ������ ���ڰ� ���� ���� ��� Overapped IO��
			// Ư���� ���� ERROR_IO_PENDING ���� �޽����� ���޵ȴ�.
			//timeouts�� ������ �ð���ŭ ��ٷ��ش�.
			/*/
			while (!GetOverlappedResult(hComm, &osWrite, &dwWritten, TRUE))
			{
				dwError = GetLastError();
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
			/*/
		}
		else
		{
			dwWritten = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
	Sleep(10);

	return dwWritten;
}

///////////////////////////////////////////////////////////////////////////////////
//
// ��Ʈ�κ��� pBuff�� nToWrite��ŭ �д´�.
// ������ ������ Byte���� �����Ѵ�.
DWORD CSerial::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead=0, dwError, dwErrorFlags;
	COMSTAT	comstat;

	if (!ReadFile(m_hComm, pBuff, nToRead, &dwRead, &m_osRead))
	{

		if (GetLastError() == ERROR_IO_PENDING)
		{
			//--------- timeouts�� ������ �ð���ŭ ��ٷ��ش�.
			while (!GetOverlappedResult(m_hComm, &m_osRead, &dwRead, TRUE))
			{
				dwError = GetLastError();
				CString str;
				str.Format(_T("comm read error ... %d "),dwError);
				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(m_hComm, &dwErrorFlags, &comstat);
					break;
				}
			}
			
		}
		else
		{
			dwRead = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwRead;
}

//////////////////////////////////////////////////////////////////////////
//
// ��Ʈ�� ����.
void CSerial::ClearPort()
{
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
}

DWORD CSerial::Read(void* lpBuf, DWORD dwCount)
{
  //Validate our parameters
//  ASSERT(IsOpen());
	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesRead = 0;
	if (!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &m_osRead))
	{
		dwError = GetLastError();

		CString str;
		str.Format(_T("comm read error ... %d "),dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwBytesRead;
}

void CSerial::Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesRead)
{
  //Validate our parameters
//  ASSERT(IsOpen());
	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesRead = 0;
	BOOL bSuccess = ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &overlapped);
	if (!bSuccess)
	{
		dwError = GetLastError();
		CString str;
		str.Format(_T("comm read error ... %d "),dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
	else
	{
		if (pBytesRead)
			*pBytesRead = dwBytesRead;
	}
}

DWORD CSerial::Write(const void* lpBuf, DWORD dwCount)
{
  //Validate our parameters
//  ASSERT(IsOpen());

	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesWritten = 0;
	if(!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &m_osWrite))
	{
		dwError = GetLastError();
		CString str;
		str.Format(_T("comm read error ... %d "),dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwBytesWritten;
}

void CSerial::Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesWritten)
{
  //Validate our parameters
//  ASSERT(IsOpen());

	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesWritten = 0;
	BOOL bSuccess = WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &overlapped);
	if (!bSuccess)
	{
		dwError = GetLastError();
		CString str;
		str.Format(_T("comm read error ... %d "),dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}
	else
	{
		if (pBytesWritten)
			*pBytesWritten = dwBytesWritten;
	}
}
///////////////////////////////////////////////////////////////////////////////
//
// ��Ʈ�� �����ϰ�, ���� ������ ������ 
// m_ReadData�� ������ �ڿ� MainWnd�� �޽����� ������ Buffer�� ������
// �о��� �Ű��Ѵ�.
// �о �����ʹ� m_InBuf�� �ִ�. 
DWORD ThreadWatchComm(CSerial* pComm)
{
	DWORD		dwEvent;
	BOOL		bOk = TRUE;
	int			total=0;
	BYTE		bufbyte;

	memset(pComm->m_InBuf,'\0',512);
	// ��Ʈ�� �����ϴ� ����.
	while(pComm->m_Connected)
	{
		dwEvent = 0;
		// ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
		WaitCommEvent(pComm->m_hComm, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			// 512�� �̺�Ʈ�� �߻��ص� �������� �������� �ƴϸ� �����Ѵ�. 
			for(int i=0;i<512;i++)
			{
				// ���� �ϳ��� �д´�. 
				int ret=pComm->ReadComm(&bufbyte,1);
				if(ret>0)
				{
					// ���̰� 0���� ũ�� �ø��� Ŭ������ ���õ� ���ۿ� ���� �� �ϳ��� �ִ´�. 
					pComm->m_InBuf[total]=bufbyte;
					total++;
					if(bufbyte==pComm->m_Terminal)
					{
						// ������ �������̸� �ø��� Ŭ������ ���õ� ������ �������� ���� ���ڸ� �ְ� �޽��� ����
						pComm->m_InBuf[total]='\0';
						SendMessage(pComm->m_hCommWnd, WM_COMM_ARRIVE, pComm->m_PortID, NULL);
						// ���� ����� ���Ͽ� �ø��� Ŭ������ ���õ� ������ �ε����� 0���� �Ѵ�. 
						total=0;
						break;
					}
				}
				else 
					break;
			}
		}

		Sleep(1);
	}	
	// ��Ʈ�� ClosePort�� ���� ������ m_bConnected �� FALSE�� �Ǿ� ����.
	pComm->m_hThreadWatchComm = NULL;
	return TRUE;
}

