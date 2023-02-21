// Protocol.h: interface for the Protocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL12CH_H__47952BAB_2AC3_47F2_95F4_F54BF1EA50C5__INCLUDED_)
#define AFX_PROTOCOL12CH_H__47952BAB_2AC3_47F2_95F4_F54BF1EA50C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_LIGHT_CHANNEL		36		// 최대 LIGHT 개수
#define PROTOCOL_EROOM_WAIT		0

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
	void SetBrightness(int channel,int brightness);
	void SetOnOff(int channel,int onoff);
	void GetBrightness(int channel);
	void SaveCurrentStatus(void);
	int  Create(int nPort, int nType, HWND hWnd);	// Type (채널 개수) 추가
	int	 Create(int nPort, int nType, void (*callback)(int nPort,char *pBuff));	// Type (채널 개수) 추가
	bool CommandRx(void);
	void (*m_callback)(void);
	void GetError();
	void GetC(int channel);
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
