
	// 닙스 조명 컨트롤용
	//////////////////////////////////////////////////////////////////////
#pragma once
#if !defined(AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)
#define AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Serialport.h"
#include "Resource.h"
#include "LightInterface.h"
#include "VSLightTask.h"

#define DSMP_MIN_DATA_LEN				8
#define DSMP_NOERROR					0
#define DSMP_NOCOMPLETE					1
#define DSMP_CRCERROR					2
#define MAX_NTC_NUM						2


class CNeepsLight : public LightInterface //: public CSerialPort 
{
public:
	CNeepsLight();	// 표준 생성자입니다.
	virtual ~CNeepsLight();

public:
	CSerialPort		m_SPort;
	
	UINT			m_nPort;
	UINT			m_nBaudRate;
	UINT			m_nControllerCnt;
	UINT			m_nChannelCnt;

	// Virtual Function
	///////////////////////////////////////////////////////////////////////////
	BOOL			InitializeLight(CString strInitFilePath);

	void			MultiTurnOn(STRU_LIGHT_INFO stLightInfo);	
	void			MultiTurnOn(ST_LIGHT_COND_AOT stLightInfo);
	void			TurnOn(UINT nController, UINT nChannel, UINT nIntensity);

	void			MultiTurnOff();
	void			TurnOff(UINT nController, UINT nChannel);

	CString			GetControllerName()													{	return _T("Neeps Light")	;};

	void			CheckLight(int nControllerCount);
	///////////////////////////////////////////////////////////////////////////
	
	BOOL			ConnectLight(int nPortNumber, int nBaudRate);
	BOOL			IsConnected()		{	return m_SPort.IsOpen()	;};
	void			ClosePort()			{	m_SPort.Close()		;};

	WORD			GetCrc(BYTE *pBuff, WORD nByte);
	WORD			ConvertWord(WORD nValue);

	void			SetPWMSubChannel(BYTE Terminal, BYTE SubChannel, WORD Duty);
	void			SetPWMChannel(BYTE Terminal, BYTE Channel, WORD Duty);
	void			SetPWMAll(BYTE Terminal, STRU_SERIAL_INFO* stLightInfo = NULL);
	//void			SetPWMAll(BYTE Terminal, STRU_SERIAL_INFO_AOT* stLightInfo = NULL);
	void			SetPWMAll_Manual(BYTE Terminal, int nValue, int nCh);
	void			SetChannel16(BYTE Terminal, int* Duty, int nCh);

	void			ErrorReset(BYTE Terminal);
	int				GetChannelCount()											{	return m_nChannelCnt		;};
	int				GetControllerCount()										{	return m_nControllerCnt		;};

	static UINT		ThreadCheckLight(LPVOID lParam);	
	BYTE			CheckProtocol(BYTE *pBuff, WORD nByte);
private:
	CWinThread*     m_pThreadCheckLight;
};
//{{AFX_INSERT_LOCATION}}

#pragma pack()

#endif // !defined(AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)