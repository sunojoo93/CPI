
// 플러스텍 조명 컨트롤용
//////////////////////////////////////////////////////////////////////
#pragma once

#include "Resource.h"
#include "LightInterface.h"

class CLFineLight : public LightInterface //: public CSerialPort 
{
public:
	CLFineLight();	// 표준 생성자입니다.
	virtual ~CLFineLight();

public:
	UINT			m_nPort;
	UINT			m_nBaudRate;
	UINT			m_nControllerCnt;
	UINT			m_nChannelTotalCnt;									// 모든 컨트롤러의 채널 총 갯수
	UINT			m_nCtrlChannelCnt[MAX_LIGHT_CONTROLLER_COUNT];		// 최대 컨트롤러 갯수(컨트롤러 당 채널 갯수 저장)		180515 YSS
 	UINT			m_nChannelType;
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
	int				GetChannelCount()											{	return m_nChannelTotalCnt		;};
	int				GetControllerCount()										{	return m_nControllerCnt		;};
	CString			GetControllerName()											{	return _T("LFine Light");	}
	void			CheckLight(int nControllerCount);
	void			GetBrightness();
	void			GetC(UINT nChannel);
	///////////////////////////////////////////////////////////////////////////
};

