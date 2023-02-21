/*
Module : SerialPort.H
Purpose: Interface for an MFC wrapper class for serial ports

Copyright (c) 1999 - 2008 by PJ Naughter.  

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////// Macros / Structs etc ////////////////////////////////////

#pragma once

#ifndef __SERIALPORT_H__
#define __SERIALPORT_H__

#ifndef CSERIALPORT_EXT_CLASS
#define CSERIALPORT_EXT_CLASS
#endif


/////////////////////////// Classes ///////////////////////////////////////////

class CSerialException : public CException
{
public:
	
//Constructors / Destructors
	CSerialException(DWORD dwError);

//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(__out_ecount_z(nMaxError) LPTSTR lpszError, __in UINT nMaxError, __out_opt PUINT pnHelpContext = NULL);
	CString GetErrorMessage();

//Data members
	DWORD m_dwError;

protected:
	DECLARE_DYNAMIC(CSerialException)
};

class CSerialPort
{
public:
	BOOL	volatile 	m_Connected;				// 포트가 열렸는지 유무를 나타냄.
//Enums
  enum FlowControl
  {
    NoFlowControl,
    CtsRtsFlowControl,
    CtsDtrFlowControl,
    DsrRtsFlowControl,
    DsrDtrFlowControl,
    XonXoffFlowControl
  };

  enum Parity
  {    
    NoParity = 0,
    OddParity = 1,
    EvenParity = 2,
    MarkParity = 3,
    SpaceParity = 4
  };

  enum StopBits
  {
    OneStopBit,
    OnePointFiveStopBits,
    TwoStopBits
  };

//Constructors / Destructors
  CSerialPort();
  virtual ~CSerialPort();

//General Methods
  void Open(int nPort, DWORD dwBaud = 9600, Parity parity = NoParity, BYTE DataBits = 8, 
            StopBits stopBits = OneStopBit, FlowControl fc = NoFlowControl, BOOL bOverlapped = FALSE);
  BOOL Open(LPCTSTR pszPort, DWORD dwBaud = 9600, Parity parity = NoParity, BYTE DataBits = 8, 
            StopBits stopBits = OneStopBit, FlowControl fc = NoFlowControl, BOOL bOverlapped = FALSE);
  void Close();
  BOOL Attach(HANDLE hComm);
  HANDLE Detach();
  operator HANDLE() const { return m_hComm; };
  BOOL IsOpen() const { return m_hComm != INVALID_HANDLE_VALUE; };
#ifdef _DEBUG
  void Dump(CDumpContext& dc) const;
#endif

//Reading / Writing Methods
  DWORD Read(void* lpBuf, DWORD dwCount);
  BOOL  Read(void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesRead=NULL);
  BOOL  ReadEx(void* lpBuf, DWORD dwCount);
  DWORD Write(const void* lpBuf, DWORD dwCount);
  BOOL  Write(const void* lpBuf, DWORD dwCount, OVERLAPPED& overlapped, DWORD* pBytesWritten=NULL);
  BOOL  WriteEx(const void* lpBuf, DWORD dwCount);
  BOOL  TransmitChar(char cChar);
  BOOL  GetOverlappedResult(OVERLAPPED& overlapped, DWORD& dwBytesTransferred, BOOL bWait);
  BOOL  CancelIo();
  DWORD BytesWaiting();
  BOOL  DataWaiting(DWORD dwTimeout);
  BOOL  WaitEvent();
//Configuration Methods
  BOOL GetConfig(COMMCONFIG& config);
  static BOOL GetDefaultConfig(int nPort, COMMCONFIG& config);
  BOOL SetConfig(COMMCONFIG& Config);
  static BOOL SetDefaultConfig(int nPort, COMMCONFIG& config);

//Misc RS232 Methods
  BOOL ClearBreak();
  BOOL SetBreak();
  BOOL ClearError(DWORD& dwErrors);
  BOOL GetStatus(COMSTAT& stat);
  BOOL GetState(DCB& dcb);
  BOOL SetState(DCB& dcb);
  BOOL Escape(DWORD dwFunc);
  void ClearDTR();
  void ClearRTS();
  void SetDTR();
  void SetRTS();
  void SetXOFF();
  void SetXON();
  BOOL GetProperties(COMMPROP& properties);
  BOOL GetModemStatus(DWORD& dwModemStatus); 

//Timeouts
  BOOL SetTimeouts(COMMTIMEOUTS& timeouts);
  BOOL GetTimeouts(COMMTIMEOUTS& timeouts);
  void Set0Timeout();
  void Set0WriteTimeout();
  void Set0ReadTimeout();

//Event Methods
  BOOL SetMask(DWORD dwMask);
  BOOL GetMask(DWORD& dwMask);
  void WaitEvent(DWORD& dwMask);
  BOOL WaitEvent(DWORD& dwMask, OVERLAPPED& overlapped);
  
//Queue Methods
  BOOL Flush();
  BOOL Purge(DWORD dwFlags);
  void TerminateOutstandingWrites();
  void TerminateOutstandingReads();
  void ClearWriteBuffer();
  void ClearReadBuffer();
  BOOL Setup(DWORD dwInQueue, DWORD dwOutQueue);

//Overridables
 // virtual void OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped);

//Static methods
  static void ThrowSerialException(DWORD dwError = 0);

protected:
//Typedefs
  typedef BOOL (WINAPI CANCELIO)(HANDLE);
  typedef CANCELIO* LPCANCELIO;

//Static methods
  //static void WINAPI _OnCompletion(DWORD dwErrorCode, DWORD dwCount, LPOVERLAPPED lpOverlapped); 

//Member variables
  HANDLE     m_hComm;        //Handle to the comms port
  HANDLE     m_hEvent;       //A event handle we need for internal synchronisation
  HINSTANCE  m_hKernel32;    //Kernel32 handle
  LPCANCELIO m_lpfnCancelIo; //CancelIO function pointer
};

#endif //__SERIALPORT_H__
