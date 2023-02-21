// Protocol32CH.h: interface for the Protocol32CH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL32CH_H__3A1B1527_A18B_4C4F_91D1_A56DA954B561__INCLUDED_)
#define AFX_PROTOCOL32CH_H__3A1B1527_A18B_4C4F_91D1_A56DA954B561__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIGHT_MODULE32_CHANNEL	32
#define PROTOCOL_32CH_WAIT		20

class Protocol32CH  
{
public:
	void UpdateFlip();
	void Restart();
	void FactoryReset();
	void LoadEx(int index);
	void SaveEx(int index);
	typedef struct{
		int nPort;
		int	nRemoteBrightness[LIGHT_MODULE32_CHANNEL];
		int	nManualBrightness[LIGHT_MODULE32_CHANNEL];
		int	nOnOffStatus[LIGHT_MODULE32_CHANNEL];
		ULONG nOnOffStatusEx;
		float nBandWidthMin;
		float nBandWidthMax;
		int nMemoryIndex;
	} PROTOCOL_32CH_INFO;

	Protocol32CH();
	virtual ~Protocol32CH();
	void SetBrightness(int channel,int brightness);
	void GetBrightness(int channel);
	void SetOnOff(int channel,int onoff);
	void SetOnOffEx(ULONG onoff);
	void GetOnOffEx(void);
	void SetBandWidth(float min, float max);
	void GetBandWidth(void);
	void GetMemIndex(void);

	void RequestCurrentStatus(void);
	void SaveCurrentStatus(void);
	int  Create(int nPort,HWND hWnd);
	int	 Create(int nPort,void (*callback)(int nPort,char *pBuff));
	bool CommandRx(void);
	void (*m_callback)(void);
protected:
	void *m_pComm32CH;
	void ReportBrightness(int channel);
	bool CommandDecoding(char *pBuff);
	int FindCommand(char *pBuff);
	char *m_pBuff;
	char *m_CmdBuff;
	int	m_nCmdRxCnt;
	int	m_nCmdLength;

	HWND m_hWnd;
	void (*m_CallBack)(int nPort,char *pBuff);
	PROTOCOL_32CH_INFO m_Info;
};

#endif // !defined(AFX_PROTOCOL32CH_H__3A1B1527_A18B_4C4F_91D1_A56DA954B561__INCLUDED_)
