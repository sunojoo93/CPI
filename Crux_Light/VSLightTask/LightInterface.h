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
	/*						 NeepsLight							*/
	/************************************************************/
};


