
// 플러스텍 조명 컨트롤용
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Resource.h"
#include "LightInterface.h"

class CEroomLight : public LightInterface //: public CSerialPort 
{
public:
	CEroomLight();	// 표준 생성자입니다.
	virtual ~CEroomLight();

public:
	UINT			m_nPort;
	UINT			m_nBaudRate;
	UINT			m_nControllerCnt;
	UINT			m_nChannelCnt;
// 	UINT			m_nChannelType;
	BOOL			m_bIsPortOpen;

	void			CallBackFunc_FromUnit(int nPort,char *pBuff);
	static UINT		ThreadCheckLight(LPVOID lParam);
private:
	CWinThread*     m_pThreadCheckLight;
	

	// Virtual Function
	///////////////////////////////////////////////////////////////////////////
	BOOL			InitializeLight(CString strInitFilePath);

	void			MultiTurnOn(STRU_LIGHT_INFO stLightInfo);	
	void			TurnOn(UINT nController, UINT nChannel, UINT nIntensity);

	void			MultiTurnOff();
	void			TurnOff(UINT nController, UINT nChannel);
	int				GetChannelCount()											{	return m_nChannelCnt		;};
	int				GetControllerCount()										{	return m_nControllerCnt		;};
	CString			GetControllerName()											{	return _T("Eroom Light");	}
	void			CheckLight(int nControllerCount);
	void			GetBrightness();
	void			GetC(UINT nChannel);
	///////////////////////////////////////////////////////////////////////////
};

