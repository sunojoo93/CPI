// Protocol.h: interface for the Protocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL12CH_H__47952BAB_2AC3_47F2_95F4_F54BF1EA50C5__INCLUDED_)
#define AFX_PROTOCOL12CH_H__47952BAB_2AC3_47F2_95F4_F54BF1EA50C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LIGHT_CHANNEL			36		// 최대 LIGHT 개수
#define PROTOCOL_Trivision_WAIT			20
#define PROTOCOL_Trivision_ONOFF_WAIT	300
#define MAX_LIGHT_VALUE				1000
#define MIN_LIGHT_VALUE				0

class Protocol  
{
public:
	typedef struct{
		int		nPort;
		int		nRemotePwmStatus[MAX_LIGHT_CHANNEL];
		int		nOnOffStatus[MAX_LIGHT_CHANNEL];
	} PROTOCOL_INFO;

	typedef struct{
		char strcmd[1024];
		char argnum;
		char cmd[50];
		float argument[10];
	} CMD_PROTOCOL;

	Protocol();
	virtual ~Protocol();
	void SetIndivMode();
	void GetLimitBrightness();
	void GetOffset();
	void GetLimitTemperature();
	void GetCurrentTemperature();
	void SetBrightness(int ctrlID, UINT LightVal[], int channelCnt);
	void SetOnOff(int ctrlID, int onoff, int nchannelCnt);
	void GetOnOff(int ctrlNum);
	void GetBrightness(int channel);
	void SaveCurrentStatus(void);
	int  Create(int nPort, int CtrlCnt, UINT CtrlChannelCnt[], int nBaud, int nType, HWND hWnd);								// 180514 YSS
	int	 Create(int nPort, int CtrlCnt, UINT CtrlChannelCnt[], int nBaud, int nType, void (*callback)(int nPort,char *pBuff));	// 180514 YSS
	bool CommandRx(void);
	void (*m_callback)(void);
	void GetError();
	void GetC(int ctrlNum, int channel);
	void GetT(int ctrlNum, int channel);
protected:
	void *m_pComm;
// 	void ReportBrightness(int channel);
	bool CommandDecoding(int cnt,unsigned char *pBuff);
	unsigned char	*m_pBuff;
	unsigned char	*m_CmdBuff;
	int				m_nCmdRxCnt;
	int				m_nCmdLength;

	HWND m_hWnd;
	void (*m_CallBack)(int nPort,char *pBuff);
	PROTOCOL_INFO m_Info;
};

#endif // !defined(AFX_PROTOCOL12CH_H__47952BAB_2AC3_47F2_95F4_F54BF1EA50C5__INCLUDED_)
