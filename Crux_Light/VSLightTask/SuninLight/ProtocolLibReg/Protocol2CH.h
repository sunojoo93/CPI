// Protocol2CH.h: interface for the Protocol2CH class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTOCOL2CH_H__9C797AE5_790F_4EED_83EF_98AD11F4D57A__INCLUDED_)
#define AFX_PROTOCOL2CH_H__9C797AE5_790F_4EED_83EF_98AD11F4D57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define LIGHT_MODULE2_CHANNEL	2
#define PROTOCOL_2CH_WAIT		20

class Protocol2CH  
{
public:
	typedef struct{
		int		nPort;
		int		nRemotePwmStatus[LIGHT_MODULE2_CHANNEL];
	} PROTOCOL_2CH_INFO;

	typedef struct{
		char strcmd[1024];
		char argnum;
		char cmd[50];
		float argument[10];
	} CMD_PROTOCOL;

	Protocol2CH();
	virtual ~Protocol2CH();
	void SetBrightness(int channel,int brightness);
	void GetBrightness(int channel);
	void RequestCurrentStatus(void);
	void SaveCurrentStatus(void);
	int  Create(int nPort,HWND hWnd);
	int	 Create(int nPort,void (*callback)(int nPort,char *pBuff));
	bool CommandRx(void);
	void (*m_callback)(void);
	void GetError();
protected:
	void *m_pComm2CH;
	void ReportBrightness(int channel);
	bool CommandDecoding(int cnt,unsigned char *pBuff);
	unsigned char	*m_pBuff;
	unsigned char	*m_CmdBuff;
	int				m_nCmdRxCnt;
	int				m_nCmdLength;

	HWND m_hWnd;
	void (*m_CallBack)(int nPort,char *pBuff);
	PROTOCOL_2CH_INFO m_Info;
};

#endif // !defined(AFX_PROTOCOL2CH_H__9C797AE5_790F_4EED_83EF_98AD11F4D57A__INCLUDED_)
