

#include "CommThread.h"

#pragma once
#if !defined(AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)
#define AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Resource.h"
#include "PGControllerInterface.h"


class CPGController_EEC_C302B : public CPGControllerInterface //: public CSerialPort 
{
public:
	CPGController_EEC_C302B();	// 표준 생성자입니다.
	virtual ~CPGController_EEC_C302B();
	BOOL SendMsg(PBYTE Msg, int nSize = -1);
	

public:
	int	 ParsingData(PBYTE pData, const int nLen);
	BOOL Close();
	BOOL TurnOn();
	BOOL TurnOff();
	BOOL NextPattern();
	BOOL PrevPattern();
	BOOL ChangePattern(int nPatternIndex/*TCHAR strPattern[]*/)	;
		
	BOOL SwapVoltage(PARAM_PG* ParamPG);

	static UINT thrSchedulerELP(LPVOID pParam);
	USHORT m_fnCalcCRC(void* Data, int nSize);
	BOOL OpenPort(int nPort, DWORD dwBaud = CBR_38400, BYTE DataBits = 8, BYTE stopbits = 0, BYTE parity = NOPARITY, FlowControl fc = NoFlowControl);
	BOOL ClosePort(void);
	BOOL IsConnected() { return m_Comm.m_bConnected;}

	BOOL SendCMD(const int nCmd, USHORT uDataLength = 0, BYTE* pData = NULL);
	BYTE CheckSum(BYTE* p, int size);
	BOOL GetAcknowledge();
	void SetPortNo(int nPortNo){m_nPortNumber = nPortNo;}
	int	 GetProtNo(){return m_nPortNumber;}
	void GetCmdListFile();
	void SetCmdList(int nIndex, BYTE cCmd);
	int	 GetPatternIndex(TCHAR* strPattern);
	BYTE m_fnStringToHex(TCHAR* cCmd);
	void SetPatternInfo(TCHAR* strPattern);
	TCHAR* GetPatternInfo() { return m_strPatternInfo; };
	void SetPatternInfoSize(int nSize) { m_nPatternInfoSize = nSize; };
	int	 GetPatternInfoSize() { return m_nPatternInfoSize; };

private:
	USHORT Replace_7D(UINT mode, UINT Length, BYTE* DstBuf, BYTE* SrcBuf);

private:
	tagELPData			tFFUData[MAX_ELP_CNT];
	int					m_nPortNumber; 
	int					m_nBaudRate;
	int					m_eCurCMD;
	int					m_eOldCMD;
	CWinThread*			m_pScheduler;
	HANDLE				m_hScheduler;
	BOOL				m_bSchedulerRun;
	BOOL				m_bRecvAckForCommand;	
	CCommThread			m_Comm;
	TCHAR				m_strPatternInfo[256];
	int					m_nPatternInfoSize;

};
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LIGHT_H__6A26E2C5_520E_4374_8A8E_510115DF5E18__INCLUDED_)