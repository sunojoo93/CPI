#pragma once

// enum eTrigMode
// {
// 	eFreeRun = 0,
// 	eStandard,
// 	eFast,
// 	eDouble,
// 	eOverlap
// };

class CVieworksComControl
{
public:

	CVieworksComControl(void);
	~CVieworksComControl(void);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera Control Member
	HANDLE		m_hComm;
	CString m_strReturnMsg;
	bool comSerialRead(BYTE *pBuff, UINT32& Buffersize);
	bool comSerialWrite(BYTE *pBuff, UINT32& Buffersize);
	int SetCommand(char* Command,UINT32 TimeOut);
	int OpenComPort(UINT32 BaudRate, CString Portname);
	void  CloseComPort();
	// Camera Control Member
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};
