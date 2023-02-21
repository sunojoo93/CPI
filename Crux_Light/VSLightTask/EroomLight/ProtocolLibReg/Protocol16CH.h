// Protocol16CH.h: interface for the Protocol16CH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL16CH_H__E799AAFA_0D45_4011_BCC4_66225E6B947E__INCLUDED_)
#define AFX_PROTOCOL16CH_H__E799AAFA_0D45_4011_BCC4_66225E6B947E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIGHT_MODULE16_CHANNEL	16
#define PROTOCOL_16CH_WAIT		20

class Protocol16CH  
{
public:
	typedef struct{
		int		nPort;
		int		nRemotePwmStatus[LIGHT_MODULE16_CHANNEL];
		int		nManPwmStatus[LIGHT_MODULE16_CHANNEL];
		int		nPioPwmStatus[LIGHT_MODULE16_CHANNEL];
		BYTE	nPioOnOffStatus[2];
		int		nBaudStatus;
		int		nMode;
		float	fFpgaVersion;
		float	fFirmwareVersion;
	} PROTOCOL_16CH_INFO;

	Protocol16CH();
	virtual ~Protocol16CH();
	void SetOnOff(int channel,int onoff);
	void SetOnOffEx(ULONG onoff);
	void SetBrightness(int channel,int brightness);
	void GetBrightness(int channel);
	void RequestCurrentStatus(void);
	void SaveCurrentStatus(void);
	int  Create(int nPort,void (*callback)(int nPort,char *pBuff));
	int  Create(int nPort,HWND hWnd);
	bool CommandRx(void);
protected:
	ULONG	m_ulOnOffInfo;
	void	*m_pComm16CH;
	void	*m_pTsipProtocol;
	bool	CommandDecoding();

	void	ReportBrightness(int channel);
	HWND m_hWnd;
	void (*m_CallBack)(int nPort,char *pBuff);
	PROTOCOL_16CH_INFO	m_Info;
};

#endif // !defined(AFX_PROTOCOL16CH_H__E799AAFA_0D45_4011_BCC4_66225E6B947E__INCLUDED_)
