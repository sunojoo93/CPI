#pragma once

#include "define.h"
#include "CommThread.h"
// CPGControllerInterface 명령 대상입니다.

struct tagELPData
{
	short sID;
	short sPV;
	short sAL;
	short sSV;
};
enum E_ELP_CMD_TYPE
{
	CMD_NONE				= 0x00,
	CMD_TURN_ON				=1,
	CMD_TURN_OFF			=2,
	CMD_PATTERN_NEXT		=3,
	CMD_PATTERN_PREV		=4,
	CMD_PATTERN_INDEX		=5,
	CMD_VOLTAGE_SWAP		=6,
	CMD_VOLTAGE_SWAP_R		=7,
	CMD_VOLTAGE_SWAP_G		=8,
	CMD_VOLTAGE_SWAP_B		=9,
};

enum E_ELP_PATTERN_INDEX
{		
	RED_PATTERN = 0,
	GREEN_PATTERN			,
	BLUE_PATTERN			,
	BLACK_PATTERN			,
	WHITE_PATTERN			,
	GRAY_8_PATTERN			,	
	GRAY_40_PATTERN			,
	GRAY_180_PATTERN			,
	GRAY_16_PATTERN			,
	GRAY_32_PATTERN			,
	GRAY_64_PATTERN			,
	GRAY_128_PATTERN			,
	LAST_PATTERN_NUM,
};
class CPGControllerInterface : public CObject
{
public:
	CPGControllerInterface();
	virtual ~CPGControllerInterface();

	virtual void SetPGInfo(int nPortNo, int nBaudRate){};
	virtual int	 GetPortNo() {return 0;};	
	virtual int	 ParsingData(PBYTE pData, const int nLen) {return 0;};	
	virtual BYTE CheckSum(BYTE* p, int size) {return 0;};	

	virtual BOOL SendMsg(PBYTE Msg, int nSize = -1) {return 0;};	
	virtual BOOL Open(int nPort) {return 0;};	
	virtual BOOL Close() {return 0;};	
	virtual BOOL TurnOn() {return 0;};	
	virtual BOOL TurnOff() {return 0;};	
	virtual BOOL NextPattern() {return 0;};	
	virtual BOOL PrevPattern() {return 0;};	
	virtual BOOL ChangePattern(int nPatternIndex/*TCHAR strPattern[]*/)	{return 0;};	
		
	virtual BOOL SwapVoltage(PARAM_PG* pParamPG) {return 0;};	
	virtual BOOL OpenPort(int nPort, DWORD dwBaud = CBR_38400, BYTE DataBits = 8, 
		BYTE stopbits = 0, BYTE parity = NOPARITY, FlowControl fc = NoFlowControl)  {return 0;};	
	virtual BOOL ClosePort(void) {return 0;};	
	virtual BOOL IsConnected() { return 0;}	
	virtual BOOL GetAcknowledge() {return 0;};		

	virtual BOOL ChangeVoltage(const int Rval,const int Gval,const int Bval) {return 0;};	
	virtual BOOL ChangeVoltage(int nIndex,const float fRval, const float fGval, const float fBval){return 0;};		
	virtual BOOL SendCMD(const int nCmd, USHORT uDataLength, BYTE* pData) {return 0;};
	virtual BOOL SendCMD(const int nCmd, const int nIndex = 1,const int val=0,const int val2=0,const int val3=0) {return 0;};	
	virtual BOOL SendCMD(const int nCmd, const int nIndex = 1,const float fRval=0.0, const float fGval=0.0, const float fBval=0.0){return 0;};
	virtual void SetPatternInfo(TCHAR* strPattern) { };
	virtual void SetPatternInfoSize(int nSize) { m_nPatternInfoSize = nSize; };
	virtual int	 GetPatternInfoSize() { return m_nPatternInfoSize; };
	virtual TCHAR* GetPatternInfo() { return m_strPatternInfo; };
	virtual int GetPatternIndex() { return 0; };

	virtual BOOL IsTurnOn() {return FALSE;};


private:
	TCHAR m_strPatternInfo[256];
	int   m_nPatternInfoSize;
//protected:
	/************************************************************/
	/*							Common							*/
	/************************************************************/


};


