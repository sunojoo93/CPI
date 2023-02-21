// Serial.cpp: implementation of the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "include.h"
//#include "extern.h"
#include "TriggerSerial.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


//#include "System.h"
//extern CSystem* pSys;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTriggerSerial::CTriggerSerial()
{
	m_Connected = FALSE;
	m_ArriveComplete = FALSE;
	m_Terminal = 0xD0;

	m_hThreadWatchComm = NULL;	// 2014.12.20 ljk

	m_hComm = (HANDLE)-1;
}

CTriggerSerial::~CTriggerSerial()
{
}

/////////////////////////////////////////////////////////////////////////////
// CSerial message handlers

BOOL CTriggerSerial::OpenPort(BYTE Terminal)
{
	// Local 변수.
	COMMTIMEOUTS	timeouts;

	// 변수 초기화
	if (m_Connected != false) return FALSE;
	m_wPortID = m_PortID - 1; // COM1-> 0, COM2->1,,,,,
	m_PortName.Format(_T("COM%d"), m_PortID);

	// overlapped structure 변수 초기화.
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	if (!(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if (!(m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))	return FALSE;

	// 포트 열기
	m_hComm = CreateFile(m_PortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (m_hComm == (HANDLE)-1) {
		DWORD dw;
		dw = GetLastError();
		return FALSE;
	}
	SetCommMask(m_hComm, EV_RXCHAR);
	// 포트 상태 설정.
	/*
	// 밑에 것은 동기화 시키기 위하여 필요함.
	BYTE InBuffer[BUFF_SIZE];
	DWORD ret;
	BYTE OutBuffer[100];
	DeviceIoControl(m_hComm,IOCTL_SERIAL_LSRMST_INSERT,
	(void *)InBuffer,BUFF_SIZE,OutBuffer,100,&ret,NULL);
	*/

	// InQueue, OutQueue 크기 설정.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);
	// 포트 비우기.
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	timeouts.ReadIntervalTimeout = 100;
	timeouts.ReadTotalTimeoutMultiplier = 100;
	timeouts.ReadTotalTimeoutConstant = 100;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts(m_hComm, &timeouts);

	// dcb 설정
	m_dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm, &m_dcb);	// 예전 값을 읽음.
	m_dcb.BaudRate = 38400;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.fInX = m_dcb.fOutX = 0;		// Xon, Xoff 사용.
										//	dcbb.XonChar	= ASCII_XON;
										//	dcb.XoffChar	= ASCII_XOFF;
	m_dcb.XonLim = 2048;
	m_dcb.XoffLim = 512;
	if (!SetCommState(m_hComm, &m_dcb))	return FALSE;
	PurgeComm(m_hComm, PURGE_RXCLEAR);
	GetCommState(m_hComm, &m_dcb);	// 예전 값을 읽음.
	PurgeComm(m_hComm, PURGE_TXCLEAR);
	// EV_RXCHAR event 설정...
	SetCommMask(m_hComm, EV_RXCHAR | EV_CTS | EV_DSR | EV_RLSD | EV_ERR | EV_RING | EV_BREAK | EV_RXFLAG | EV_TXEMPTY);
	// 포트 감시 쓰레드 생성...
	m_Connected = TRUE;
	CWinThread* Watch = new CWinThread;
	Watch = AfxBeginThread((AFX_THREADPROC)ThreadWatchComm, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	m_hThreadWatchComm = Watch->m_hThread;
	delete Watch;
	if (!m_hThreadWatchComm)
	{
		ClosePort();
		return FALSE;
	}
	PurgeComm(m_hComm, PURGE_RXCLEAR);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// 
//	포트를 연다
//	종료 문자 두개를 지정 할 수있다. 
//


/////////////////////////////////////////////////////////////////////////////
// 
//	포트를 연다
//	종료 문자 기본적으로 캐리지 리턴을 사용한다. 
//
BOOL CTriggerSerial::OpenPort(CString sPortName, DWORD dwBaud, WORD wPortID)
{
	// Local 변수.
	COMMTIMEOUTS	timeouts;
	//	DWORD			dwThreadID;

	// 변수 초기화
	//	Connected = FALSE;
	if (m_Connected != false) return FALSE;
	m_wPortID = wPortID; // COM1-> 0, COM2->1,,,,,

						 // overlapped structure 변수 초기화.
	m_osRead.Offset = 0;
	m_osRead.OffsetHigh = 0;
	if (!(m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;
	m_osWrite.Offset = 0;
	m_osWrite.OffsetHigh = 0;
	if (!(m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))		return FALSE;

	// 포트 열기
	m_sPortName = sPortName;
	m_hComm = CreateFile(sPortName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);
	if (m_hComm == (HANDLE)-1) return FALSE;
	SetCommMask(m_hComm, EV_RXCHAR);
	// InQueue, OutQueue 크기 설정.
	SetupComm(m_hComm, BUFF_SIZE, BUFF_SIZE);
	// 포트 비우기.
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);

	// timeout 설정.
	//	timeouts.ReadIntervalTimeout			=	3000;
	timeouts.ReadIntervalTimeout = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;
	SetCommTimeouts(m_hComm, &timeouts);

	// dcb 설정
	m_dcb.DCBlength = sizeof(DCB);
	GetCommState(m_hComm, &m_dcb);	// 예전 값을 읽음.
	m_dcb.BaudRate = dwBaud;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.fInX = m_dcb.fOutX = 0;		// Xon, Xoff 사용.
										//	dcb.XonChar = ASCII_XON;
										//	dcb.XoffChar = ASCII_XOFF;
	m_dcb.XonLim = 2048;
	m_dcb.XoffLim = 512;
	if (!SetCommState(m_hComm, &m_dcb))	return FALSE;

	PurgeComm(m_hComm, PURGE_RXCLEAR);

	GetCommState(m_hComm, &m_dcb);	// 예전 값을 읽음.

	PurgeComm(m_hComm, PURGE_TXCLEAR);
	PurgeComm(m_hComm, PURGE_RXCLEAR);

	PurgeComm(m_hComm, PURGE_TXCLEAR);
	PurgeComm(m_hComm, PURGE_RXCLEAR);

	return TRUE;
}

void CTriggerSerial::ClosePort()
{
	m_Connected = FALSE;
	if (m_hComm == (HANDLE)-1) return;
	SetCommMask(m_hComm, 0);
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
	CloseHandle(m_hComm);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
// 포트에 pBuff의 내용을 nToWrite만큼 쓴다.
// 실제로 쓰여진 Byte수를 리턴한다.

DWORD CTriggerSerial::WriteComm(BYTE *pBuff, DWORD nToWrite)
{
	DWORD	dwWritten, /*dwError,*/ dwErrorFlags;
	COMSTAT	comstat;

	dwWritten = TRUE;
	m_ArriveComplete = FALSE;
	if (!WriteFile(m_hComm, pBuff, nToWrite, &dwWritten, &m_osWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
			// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
			//timeouts에 정해준 시간만큼 기다려준다.
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
		else if (GetLastError() != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
		else
		{
			//dwWritten = 0;
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
		dwWritten = FALSE;
	}
	Sleep(10);

	return dwWritten;
}

///////////////////////////////////////////////////////////////////////////////////
//
// 포트로부터 pBuff에 nToWrite만큼 읽는다.
// 실제로 읽혀진 Byte수를 리턴한다.
DWORD CTriggerSerial::ReadComm(BYTE *pBuff, DWORD nToRead)
{
	DWORD	dwRead = 0, dwError, dwErrorFlags;
	COMSTAT	comstat;
	int nCnt = 0;

	//2016.04.07 edit by ikm - 포트의 버퍼를 모두 클리어 마지막 처리 데이터만 읽어 올수 있게 한다 - S
	ClearCommError(m_hComm, &dwErrorFlags, &comstat);

	if (!ReadFile(m_hComm, pBuff, nToRead, &dwRead, &m_osRead))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			//--------- timeouts에 정해준 시간만큼 기다려준다.
			while (!GetOverlappedResult(m_hComm, &m_osRead, &dwRead, TRUE))
			{
				dwError = GetLastError();

				//쓸데없는 스트링 주석
				//CString str;
				//str.Format("comm read error ... %d ",dwError);

				if (dwError != ERROR_IO_INCOMPLETE)
				{
					ClearCommError(m_hComm, &dwErrorFlags, &comstat);
					break;
				}
				if (nCnt == 10)
				{
					break;
				}
				nCnt++;
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
// 포트를 비운다.
void CTriggerSerial::ClearPort()
{
	PurgeComm(m_hComm, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR);
}

DWORD CTriggerSerial::Read(BYTE *pBuff, DWORD nToRead)
{
	//Validate our parameters
	//ASSERT(IsOpen());
	DWORD dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesRead = 0;
	ClearCommError(m_hComm, &dwErrorFlags, &comstat);
	if (!ReadFile(m_hComm, pBuff, nToRead, &dwBytesRead, NULL))
	{
		// 		if (GetLastError() != ERROR_IO_INCOMPLETE)
		// 		{
		ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		//	}
	}
	/*else
	{

	ClearCommError(m_hComm, &dwErrorFlags, &comstat);
	}*/

	return dwBytesRead;
}

DWORD CTriggerSerial::Read(void* lpBuf, DWORD dwCount)
{
	//Validate our parameters
	//  ASSERT(IsOpen());
	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesRead = 0;
	ClearCommError(m_hComm, &dwErrorFlags, &comstat);
	if (!ReadFile(m_hComm, lpBuf, dwCount, &dwBytesRead, &m_osRead))
	{
		dwError = GetLastError();

		CString str;
		str.Format(_T("comm read error ... %d "), dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwBytesRead;
}

void CTriggerSerial::Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesRead)
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
		str.Format(_T("comm read error ... %d "), dwError);
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

DWORD CTriggerSerial::Write(const void* lpBuf, DWORD dwCount)
{
	//Validate our parameters
	//  ASSERT(IsOpen());

	DWORD dwError, dwErrorFlags;
	COMSTAT	comstat;
	DWORD dwBytesWritten = 0;

	if (!WriteFile(m_hComm, lpBuf, dwCount, &dwBytesWritten, &m_osWrite))
	{
		dwError = GetLastError();
		CString str;
		str.Format(_T("comm read error ... %d "), dwError);
		if (dwError != ERROR_IO_INCOMPLETE)
		{
			ClearCommError(m_hComm, &dwErrorFlags, &comstat);
		}
	}

	return dwBytesWritten;
}

void CTriggerSerial::Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesWritten)
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
		str.Format(_T("comm read error ... %d "), dwError);
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
// 포트를 감시하고, 읽힌 내용이 있으면 
// m_ReadData에 저장한 뒤에 MainWnd에 메시지를 보내어 Buffer의 내용을
// 읽어가라고 신고한다.
// 읽어갈 데이터는 m_InBuf에 있다. 
DWORD ThreadWatchComm(CTriggerSerial* pComm)
{
	DWORD		dwEvent;
	BOOL		bOk = TRUE;
	int			total = 0;
	BYTE		bufbyte;

	memset(pComm->m_InBuf, '\0', 512);
	// 포트를 감시하는 루프.
	while (pComm->m_Connected)
	{
		dwEvent = 0;
		// 포트에 읽을 거리가 올때까지 기다린다.
		WaitCommEvent(pComm->m_hComm, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			// 512번 이벤트가 발생해도 데이터의 마지막이 아니면 무시한다. 
			for (int i = 0; i<512; i++)
			{
				// 먼저 하나를 읽는다. 
				int ret = pComm->ReadComm(&bufbyte, 1);
				if (ret>0)
				{
					// 길이가 0보다 크면 시리얼 클래스에 마련된 버퍼에 읽은 값 하나를 넣는다. 
					pComm->m_InBuf[total] = bufbyte;
					total++;
					if (bufbyte == pComm->m_Terminal)
					{
						// 데이터 마지막이면 시리얼 클래스에 마련된 버퍼의 마지막에 공백 문자를 넣고 메시지 날림
						pComm->m_InBuf[total] = '\0';
						SendMessage(pComm->m_hCommWnd, WM_COMM_ARRIVE, pComm->m_PortID, NULL);
						// 다음 통신을 위하여 시리얼 클래스에 마련된 버퍼의 인덱스를 0으로 한다. 
						total = 0;
						break;
					}
				}
				else
					break;
			}
		}

		Sleep(1);
	}
	// 포트가 ClosePort에 의해 닫히면 m_bConnected 가 FALSE가 되어 종료.
	pComm->m_hThreadWatchComm = NULL;
	return TRUE;
}

