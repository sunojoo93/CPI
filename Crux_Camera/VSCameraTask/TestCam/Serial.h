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
	//--------- ȯ�� ���� -----------------------------------------//
	HANDLE				m_hComm;					// ��� ��Ʈ ���� �ڵ�
	HANDLE				m_hThreadWatchComm;		// Watch�Լ� Thread �ڵ�.
	HWND				m_hCommWnd;				// Thread ���� �޽����� ���� �� �������� ���� �ڵ�...
	CString				m_sPortName;				// ��Ʈ �̸� (COM1 ..)
	CString				m_PortName;				// ��Ʈ �̸� (COM1 ..)
	
	BOOL	volatile 	m_Connected;				// ��Ʈ�� ���ȴ��� ������ ��Ÿ��.
	OVERLAPPED			m_osRead, m_osWrite;		// ��Ʈ ���� Overlapped structure
	WORD				m_wPortID;				// ���� ���� ��Ʈ ��ȣ - 1, WM_COMM_READ�� �Բ� ������ �μ�.
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


// Thread�� ����� �Լ� 
DWORD	ThreadWatchComm(CSerial* pComm);
