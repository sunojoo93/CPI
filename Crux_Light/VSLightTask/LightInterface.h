#pragma once

#include "define.h"

// LightInterface 명령 대상입니다.

class LightInterface : public CObject
{
public:
	LightInterface();
	virtual ~LightInterface();
//protected:
	/************************************************************/
	/*							Common							*/
	/************************************************************/
	// Init
	virtual	BOOL	InitializeLight(CString strInitFilePath)							{	return	FALSE		;};

	// Turn On
	virtual	void	MultiTurnOn(STRU_LIGHT_INFO stLightInfo)							{	return				;};
	//virtual	void	MultiTurnOn(ST_LIGHT_COND_AOT stLightInfo) { return; };
	virtual	void	SingleTurnOn(UINT nController, UINT nChannel, UINT nIntensity)		{	return				;};	

	// Turn Off
	virtual	void	MultiTurnOff()														{	return				;};
	virtual	void	SingleTurnOff(UINT nController, UINT nChannel)						{	return				;};	

	virtual int		GetChannelCount()													{	return	0	;};
	virtual int		GetControllerCount()												{	return 	0	;};
	virtual void	GetBrightness()														{	return 		;};
	virtual CString	GetControllerName()													{	return _T("Light")	;};

	virtual void	GetC(UINT nChannel)																{	return				;};

	// Check
	virtual void	CheckLight(int nControllerCount)									{	return				;};


	/************************************************************/
	/*						 IPuls						*/
	/************************************************************/
		virtual UINT16	GetTriggerCount(BYTE SlaveID) {		return 0;	};
	virtual UINT16	SequenceIndexReset(UINT8 mode, BYTE SlabeID) { return 0; };
	virtual UINT32	GetAlarmCode	(UINT8 mode, BYTE SlabeID) { return 0; };
	virtual FLOAT Read_Func3_FLOAT(BYTE SlaveID, BYTE Func, WORD StartRegister, WORD Quantity) { return 0; };
	virtual UINT16 Read_Func3_UINT16(BYTE SlaveID, BYTE Func, WORD StartRegister, WORD Quantity) { return 0; };
	virtual UINT32 Read_Func3_UINT32(BYTE SlaveID, BYTE Func, WORD StartRegister, WORD Quantity) { return 0; };

	virtual FLOAT Write_Func6_FLOAT(BYTE SlaveID, BYTE Func, WORD StartRegister, FLOAT Value) { return 0; };
	virtual UINT16 Write_Func6_UINT16(BYTE SlaveID, BYTE Func, WORD StartRegister, WORD Value) { return 0; };
	virtual UINT32 Write_Func6_UINT32(BYTE SlaveID, BYTE Func, WORD StartRegister, UINT32 Value) { return 0; };

	virtual UINT16	ApplySequenceIndexer() { return 0; };
	virtual int	ApplyLightProperty(ST_GRAB_AREA_INFO_AOT* data) { return 0; };

};


