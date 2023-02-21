#if !defined(AFX_PROTOCOLLLight_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_)
#define AFX_PROTOCOLLLight_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_

#include "VSLightTask.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#define PROTOCOL_LLight_WAIT			20
#define PROTOCOL_LLight_LENGTH		512

#define ERR_CODE_COMMAND		10
#define ERR_CODE_ADDRESS		20
#define ERR_CODE_PARAMETER		30
#define ERR_CODE_DATA_RANGE		40
#define ERR_CODE_BYTES_NUMBER	50

struct LIGHT_LLight_STATUS {
	//int		nChannelCount;
	//int		nCtrID;
	bool	bChannelOn	[MAX_LIGHT_CHANNEL_COUNT];
	int		nBrightness	[MAX_LIGHT_CHANNEL_COUNT];
	int		nCurrent	[MAX_LIGHT_CHANNEL_COUNT];
	int		nTemperature[MAX_LIGHT_CHANNEL_COUNT];

	LIGHT_LLight_STATUS()
	{
		//nChannelCount = 0;
		//nCtrID = 0;
		memset(bChannelOn, NULL, sizeof(bChannelOn));
		memset(nBrightness, -1, sizeof(nBrightness)); 
		memset(nCurrent, NULL, sizeof(nCurrent));
		memset(nTemperature, NULL, sizeof(nTemperature));
	}
};

class LLightController  
{
public:
	LLightController();
	virtual ~LLightController();

	// Module Set/Get
	void SetOnOff(LIGHT_LLight_STATUS stLightInfo);
	void GetOnOffAll();
	
	// Brightness Set/Get
	void SetBrightness(LIGHT_LLight_STATUS stLightInfo);
	void GetBrightnessAll();

	void SetCtrID(int nID) { m_nCtrID = nID; };
	int  GetCtrID()		   { return m_nCtrID; };

	// Current Get
	void GetCurrentAll();

	// Temperature Get
	void GetTemperatureAll();


	void GetError();

	// Open Port
	int Create(int nPort,int nBaudRate,void (*callback)(int nPort,char *pBuff));
	int Create(int nPort,int nBaudRate,HWND hWnd);
	int	Create(int nPort,int nBaudRate);

	// Close Port : 이 객체(자신 객체)를 delete 해주면 된다.

	// 프로토콜 구조체 생성
	LIGHT_LLight_STATUS m_stLightStatus;

	HWND	m_hWnd;
	void	*m_pCommLLight;
	void	(*m_CallBack)(int nPort,char *pBuff);
	int		m_nCtrID;
};

#endif // !defined(AFX_PROTOCOL8CH_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_)
