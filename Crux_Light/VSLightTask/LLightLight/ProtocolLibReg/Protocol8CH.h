// Protocol8CH.h: interface for the Protocol8CH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL8CH_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_)
#define AFX_PROTOCOL8CH_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIGHT_MODULE8_CHANNEL 8

#define PROTOCOL_8CH_WAIT	20

#define COMMAND_MASK		0xE0
#define START_FRAME			0x20
#define END_FRAME			0xC0
#define COMMAND_BYTE		0x40
#define DATA_BYTE			0x60

#define COMMAND_SAVE		0x00
#define COMMAND_ONTIME		0x01
#define COMMAND_DELAY		0x02
#define COMMAND_HOLD		0x03
#define COMMAND_TRGRF_EN	0x04
#define COMMAND_PWMCTRL		0x05
#define COMMAND_SWONOFF0	0x06
#define COMMAND_SWONOFF1	0x07
#define COMMAND_DAC			0x08
#define COMMAND_ADC			0x09
#define COMMAND_READ		0x0A
#define COMMAND_ERROR		0x0B
#define COMMAND_PIO			0x0C
#define COMMAND_RESERVED1	0x0D
#define COMMAND_RESERVED2	0x0E
#define COMMAND_BAUD		0x0F

class Protocol8CH  
{
public:
	typedef struct{
		UCHAR ucStartFrame;
		UCHAR ucCommand;
		UCHAR ucData[4];
		UCHAR ucEndFrame;
	} PROTOCOL_8CH_PACKET;

	typedef struct{
		UCHAR 	ucEnable;			//Enable
		UCHAR	ucEdge;				//Edge
		UCHAR	ucTriggerIntr;
		USHORT	usDelayTime[4];
		USHORT	usHoldTime[4];
		USHORT	usOnTime[4];
	} TRG_STATUS;

	typedef struct{
		int		nPort;
		int		nRemotePwmStatus[LIGHT_MODULE8_CHANNEL];
		int		nManPwmStatus[LIGHT_MODULE8_CHANNEL];
		int		nPioPwmStatus[LIGHT_MODULE8_CHANNEL];
		int		nDacValue[3];
		UCHAR	nRemoteOnOffStatus[4];
		TRG_STATUS triggerStatus;
	} PROTOCOL_8CH_INFO;

	Protocol8CH();
	virtual ~Protocol8CH();
	void SetOnOff(int channel,int onoff);
	void SetOnOffEx(ULONG onoff);
	void SetBrightness(int channel,int brightness);
	void GetBrightness(int channel);
	void RequestCurrentStatus();
	void SaveCurrentStatus();
	int  Create(int nPort,void (*callback)(int nPort,char *pBuff));
	int  Create(int nPort,HWND hWnd);
	bool CommandRx(void);
protected:
	void ReportBrightness();
	void TriggerStatus(int ucrf, int ucen);
	void TriggerDelay(int channel, int time);
	void TriggerHold(int channel, int time);
	void TriggerOnTime(int channel, int time);
	void TriggerTime(int channel, UCHAR command, UINT time);
	void *m_pComm8CH;
	bool CommandDecoding(int cnt,unsigned char *pBuff);
	unsigned char	*m_pBuff;
	unsigned char	*m_CmdBuff;
	int				m_nCmdRxCnt;
	int				m_nCmdLength;
	ULONG			m_ulOnOffInfo;

	HWND			  m_hWnd;
	void (*m_CallBack)(int nPort,char *pBuff);
	PROTOCOL_8CH_INFO m_Info;
};

#endif // !defined(AFX_PROTOCOL8CH_H__4BFD59CF_5181_475A_9AB1_504A19CC2BF9__INCLUDED_)
