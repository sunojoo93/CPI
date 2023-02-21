// Trigger.h: interface for the CTrigger class.
// Trigger Control
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIGGER_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)
#define AFX_TRIGGER_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"
#include "TriggerSerial.h"


#define PID_SET_STX					0x02
#define PID_SET_ETX					0x03

#define BOARD_TYPE_YOUNGWOO			0
#define BOARD_TYPE_DAEKON			1

class CTriggerControl : public CTriggerSerial
{

public:
	CTriggerControl();           // protected constructor used by dynamic creation
	virtual ~CTriggerControl();

	CString m_CurrentMsg;
	bool m_bReturned;
	//
private:
	BOOL m_bTrigerDirForword;

public:
	CString m_sPortName;
	UINT m_wPortID;
	UINT m_nBaudRate;
	bool m_bFirst;

	BOOL m_bTriFailFlag;		//2016.01.01 add by mby 
	BOOL m_bChsetCheck;		//2016.01.01 add by mby 

	BOOL	Send_Trigger(LPCTSTR szFormatString, ...);

	BOOL	ConnectTrigger(int nPortNumber, int nBaudRate);

	bool	IsConnected() { return this->m_Connected; };

	void	SetScanRange(int start, int end, int interval);

	void	SetTriggerDir(BOOL bForward);
	BOOL	GetTriggerDir(void);

	void	GetCurTrgCount();
	void	GetCurStart();
	void	GetCurEnd();
	void	GetCurInterval();
	void	GetEncErrCount();
	void	GetCurEnc();
	void	StopTriggerGen0();
	void	StopTriggerGen1();
	void	StartForwardScan();
	void	StartBackwardScan();

	void    TriggerInit0();
	void    TriggerInit1();

	void 	TriggerStartPosition8K();
	void 	TriggerStartPosition2K();
	void    TriggerOrigin();
	void    TriggerCurrentPosition();
	void    TriggerGenCount0();
	void    TriggerGenCount1();

	void	TriggerStartPositionGet_01();
	void	TriggerStartPositionGet_00();

	void    TriggerDebug();
	void    WaitRecvMsg();

	unsigned char CalcChecksum(unsigned char *data, int leng);

	BOOL StartTrigger(bool State); // Trigger
	BOOL TriggerInitSet(bool State); // TriggerInitSet //2016.08.19 add by mby for Tact time(BIG,MONITOR)
									 //BOOL StartTrigger(BOOL bState,  bool bIsDir); // Trigger 
	BOOL StartTrigger(bool bState, BOOL bIsDir);
	BOOL StartTrigger(BOOL bIsDir, int CamNo, int nStartPos, int nEndPos, int nPeriod);	//2015.01.31 for Camera Teaching

	void TriggerStartPositionChannel0(CString strStartPos, CString strEndPos, CString strPeriod);
	void TriggerStartPositionChannel1(CString strStartPos, CString strEndPos, CString strPeriod);

	void TriggerStartPositionChannel0(int startPos, int stopPos, int period);
	void TriggerStartPositionChannel1(int startPos, int stopPos, int period);
	//	DECLARE_MESSAGE_MAP()

	BOOL TriggerTimer(int time);

	void TriggerPositionChannelSet();

private:

	UINT			TriggerDelayCommand();					// Baudrate에 따른 커맨드간 딜레이를 얻는함수	// 2015.05.19 ljk
	void			DaekonSelectPitchMode(UINT uiSelectEncoder);	// 선택한 엔코더를 Pit모드로 설정				// 2015.05.19 ljk	
	void			DaekonSetUpperEncoderStart(double dStartPos, double dResolution = 1);	// 상부 엔코더의 시작 설정	// 2015.05.19 ljk
	void			DaekonSetDownEncoderStart(double dStartPos, double dResolution = 1);	// 하부 엔코더의 시작 설정	// 2015.05.19 ljk
	void			DaekonSetUpperEncoderEnd(double dEndPos, double dResolution = 1);		// 상부 엔코더의 종료 설정	// 2015.05.19 ljk
	void			DaekonSetDownEncoderEnd(double dEndPos, double dResolution = 1);		// 하부 엔코더의 종료 설정	// 2015.05.19 ljk
	void			DaekonSetUpperEncoderPeriod(double dPeriod, double dResolution = 1);	// 상부 엔코더의 주기 적용	// 2015.05.19 ljk
	void			DaekonSetDownEncoderPeriod(double dPeriod, double dResolution = 1);		// 하부 엔코더의 주기 적용	// 2015.05.19 ljk
	void			DaekonSetUpperEncoderPulseWidth(double dPulseWidth);				// 상부 엔코더에 펄스폭 적용// 2015.05.19 ljk
	void			DaekonSetDownEncoderPulseWidth(double dPulseWidth);					// 하부 엔코더에 펄스폭 적용// 2015.05.19 ljk
	void			DaekonSetUpperEncoderDelay();					// 상부 엔코더에 딜레이 적용					// 2015.05.19 ljk
	void			DaekonSetDownEncoderDelay();					// 하부 엔코더에 딜레이 적용					// 2015.05.19 ljk
	void			DaekonInitEncoder();							// 엔코더 초기화								// 2015.05.19 ljk
	void			DaekonInitTrigger();							// 트리거 초기화								// 2015.05.19 ljk
	void			DaekonOperatingOff();							// Operating Off								// 2015.05.19 ljk
	void			DaekonOperatingOn();							// Operating On									// 2015.05.19 ljk
	void			DaekonUpperDirection(int nDirection);			// 상부 엔코더 방향 지정						// 2015.05.19 ljk
	void			DaekonDownDirection(int nDirection);			// 하부 엔코더 방향 지정						// 2015.05.19 ljk

};

//{{AFX_INSERT_LOCATION}}
UINT	ThreadWatchTrigger(CTriggerControl* pTrigger);
#pragma pack()
#endif // !defined(AFX_TRIGGER_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)