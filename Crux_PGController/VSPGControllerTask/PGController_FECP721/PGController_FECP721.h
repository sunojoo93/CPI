#pragma once

#include "CommThread.h"
//#define  MAX_ELP_CNT  1
#include "PGControllerInterface.h"
class CPGController_FECP721 : public CPGControllerInterface
{
	//DECLARE_DYNCREATE(CPGController_FECP721)
public:

	CPGController_FECP721(int nPortNumber,int nBaudRate);
	CPGController_FECP721();
	~CPGController_FECP721(void);
	BOOL SendMsg(PBYTE Msg, int nSize = -1);
	
public:
	//virtual void operator=(const CPGController_FECP721& CPGController);
	int	 ParsingData(PBYTE pData, const int nLen);
	BOOL Close();
	BOOL TurnOn();
	BOOL TurnOff();
	BOOL NextPattern();
	BOOL PrevPattern();
	BOOL ChangePattern(int nPatternIndex/*TCHAR strPattern[]*/)	;
	BOOL ChangeVoltage(int nIndex,const float fRval, const float fGval, const float fBval);
	BOOL OpenPort(int nPort, DWORD dwBaud = CBR_38400, BYTE DataBits = 8, BYTE stopbits = 0, BYTE parity = NOPARITY, FlowControl fc = NoFlowControl);
	BOOL ClosePort(void);
	BOOL IsConnected() { return m_Comm.m_bConnected;}
	BOOL SendCMD(const int nCmd, const int nIndex = 1,const float fRval=0.0, const float fGval=0.0, const float fBval=0.0);
	BYTE CheckSum(BYTE* p, int size);
	BOOL GetAcknowledge();
	void SetPortNo(int nPortNo){m_nPortNumber = nPortNo;}
	int	 GetPortNo(){return m_nPortNumber;}
	TCHAR* GetPatternInfo() { return m_strPatternInfo; };
	int	 GetPatternInfoSize() { return m_nPatternInfoSize; };
	int GetPatternIndex(TCHAR* strPattern);
	void SetPatternInfo(TCHAR* strPattern);
	void SetPatternInfoSize(int nSize) { m_nPatternInfoSize = nSize; };
	static UINT thrSchedulerELP(LPVOID pParam);
private:
	tagELPData tFFUData[MAX_ELP_CNT];
	int m_nPortNumber; 
	int m_nBaudRate;

	int					m_eCurCMD;
	int					m_eOldCMD;
	CWinThread*			m_pScheduler;
	HANDLE				m_hScheduler;
	BOOL				m_bSchedulerRun;

	BOOL				m_bRecvAckForCommand;
	TCHAR				m_strPatternInfo[256];
	CCommThread			m_Comm;
	int					m_nPatternInfoSize;
};

