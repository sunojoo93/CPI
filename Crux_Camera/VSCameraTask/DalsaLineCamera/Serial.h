// Serial.h: interface for the CSerial class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIAL_H__4841B9A2_6296_4E83_A397_D9B3995F66AE__INCLUDED_)
#define AFX_SERIAL_H__4841B9A2_6296_4E83_A397_D9B3995F66AE__INCLUDED_


#define	BUFF_SIZE		4096
#define	WM_COMM_ARRIVE	WM_USER+2


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSerial  
{
public:
	CSerial();
	virtual ~CSerial();

	public:
	//--------- 환경 변수 -----------------------------------------//
	HANDLE				m_hComm;					// 통신 포트 파일 핸들
	HANDLE				m_hThreadWatchComm;		// Watch함수 Thread 핸들.
	HWND				m_hCommWnd;				// Thread 에서 메시지를 보낼 때 목적지의 윈도 핸들...
	CString				m_sPortName;				// 포트 이름 (COM1 ..)
	CString				m_PortName;				// 포트 이름 (COM1 ..)
	
	BOOL	volatile 	m_Connected;				// 포트가 열렸는지 유무를 나타냄.
	OVERLAPPED			m_osRead, m_osWrite;		// 포트 파일 Overlapped structure
	WORD				m_wPortID;				// 현재 열린 포트 번호 - 1, WM_COMM_READ와 함께 보내는 인수.
	DCB					m_dcb;
	BYTE				m_TerminalChar1;
	BYTE				m_TerminalChar2;
	BYTE				m_InBuf[512];
	unsigned int		m_PortID;
	unsigned int		m_BaudRate;
	BYTE				m_Terminal;
	BOOL	volatile	m_ArriveComplete;

	BOOL	OpenPort(CString sPortName, DWORD dwBaud, WORD wParam);
	BOOL	OpenPort(BYTE Terminal);
	void	ClosePort();

	DWORD	WriteComm(BYTE *pBuff, DWORD nToWrite);
	DWORD	ReadComm(BYTE *pBuff, DWORD nToRead);
	void	ClearPort();

	DWORD	Read(void* lpBuf, DWORD dwCount);
	void	Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesRead);
	DWORD	Write(const void* lpBuf, DWORD dwCount);
	void	Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesWritten);

};

#endif // !defined(AFX_SERIAL_H__4841B9A2_6296_4E83_A397_D9B3995F66AE__INCLUDED_)


// Thread로 사용할 함수 
DWORD	ThreadWatchComm(CSerial* pComm);
