#pragma once

#include "define.h"
#include "..\..\CommonHeaders\Structure.h"

// CameraInterface 명령 대상입니다.

class CameraInterface : public CObject
{
public:
	CameraInterface();
	virtual ~CameraInterface();
//protected:
	/************************************************************/
	/*							Common							*/
	/************************************************************/
	// Init
	virtual	BOOL	InitializeCamera(CString strInitFilePath)			{	return	FALSE	;};

	// Grab
	/*virtual	void	CameraExpose()										= 0;*/
	virtual	void	WaitGrabEnd()										= 0;
	virtual	void	GetGrabImage(byte* byteImgArr)						= 0;	
	virtual	void	GetGrabImage(int nImgCnt, BOOL bIsLiveImage, byte* byteImgArr, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, ULONG lWindowSizeX, ULONG lWindowSizeY)	
																		= 0;	
	virtual	void	DisplayLive(HWND hDispWnd = NULL)					{	return			;};
 	virtual	BOOL	StartLiveGrab()										= 0;
	virtual	BOOL	StopLiveGrab()										= 0;
	virtual void	SaveGrabImage(CString strFilePath)					{	return			;};	
	virtual void	SaveGrabImage(int nImgCnt, BOOL bIsLiveImage, BOOL bIsGrayImg, ULONG lOffsetX, ULONG lOffsetY, ULONG lCropSizeX, ULONG lCropSizeY, CString strFilePath)
																		{	return			;};
	virtual void	DisplayGrabImage(HWND hDispWnd = NULL)				{	return			;};

	
	virtual void	SaveFileCurBuffer(TCHAR* strSavePath)				{	return			;};	
// 	// Get
	virtual	int		GetImageWidth()										= 0;
	virtual	int		GetImageHeight()									= 0;
	virtual	int		GetImageBitrate()									= 0;
	virtual	int		GetImageBandwidth()									= 0;
	virtual	int		GetImageSize()										= 0;
 
 	virtual	double	GetExposureTime()									= 0;
 	virtual	double	GetAnalogGain()										= 0;
	virtual	int		GetTriggerMode()									= 0;
	virtual	int		GetSequenceMode()									= 0;	
	virtual	int		GetDataBit()										= 0;	
	virtual	int		GetExposureSource()									{	return 0		;};	

	virtual	bool	GetFanStatus()										{	return 0		;};
	virtual	int		GetFanRPM()											{	return 0		;};
	virtual	double	GetTemperature()									{	return 0		;};
	virtual	double	GetCCDTemperature()									{	return 0		;};
// 
// 	// Set
 	virtual	BOOL	SetExposureTime(double nExpTime)					= 0;		
	virtual BOOL	SetStitchMode(int nMode)							= 0;	// 180809 YSS
 	virtual	BOOL	SetAnalogGain(double nGain)							= 0;	
	virtual	BOOL	SetTriggerMode(int nTrigMode)						= 0;
	virtual	BOOL	SetSequenceMode(int nSeqMode)						= 0;
	virtual	BOOL	SetDataBit(int nDataBit)							= 0;
	virtual	BOOL	SetExposureSource(int nExpSrc)						{	return FALSE	;};

	virtual void	TestFunc( int nGV )									{	return			;};
 	virtual void 	m_fnSetBufImage(TCHAR*  strImgPath, int nImgCnt, int& nChcnt, int &nWidth, int &nHeight, double dAngle = 0.0F)	
																		{	return			;};

	virtual void	GetBrightnessVal( int nWantMean, CRect rect, int& nOutExp, int& nOutGain )
																		{	return			;};
	virtual CString	GetInitFilePath()									{	return	_T("")	;};

	virtual int		GetImageCallBackState()								{	return 0		;};
	virtual UINT	retryConnect()										{	return 0		;};

#ifdef _MATROXCAMERA
	/************************************************************/
	/*						 Matrox Camera						*/
	/************************************************************/
	virtual BOOL	InitializeGrabber(int nGrabberNo, eBoardModel eGrabberModel, int nDigCh, CString strDcfFilePath)	
	{
		return FALSE;
	};

	virtual	void	CameraExpose() = 0;
	virtual BOOL	OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
																		{	return FALSE	;};
	virtual int		GetMonitoringInterval()								{	return -1		;};
	virtual void	SetMonitoringInterval(int nInterval)				{	return			;};
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };

#elif _VISTEKCAMERA
	/************************************************************/
	/*						 Vistek Camera						*/
	/************************************************************/
	virtual void	CalcWhiteBalance(float* WhiteBalance)									{	return			;};
	virtual void	SetWhiteBalance(float* WhiteBalance)									{	return			;};
	virtual void	GetWhiteBalance(float* WhiteBalance)									{	return			;};
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#elif _IMICAMERA
	/************************************************************/
	/*						 Vistek Camera						*/
	/************************************************************/
	virtual void	CalcWhiteBalance(float* WhiteBalance) { return; };
	virtual void	SetWhiteBalance(float* WhiteBalance) { return; };
	virtual void	GetWhiteBalance(float* WhiteBalance) { return; };
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#elif _DALSACAMERA
	/************************************************************/
	/*						 Dalsa Camera						*/
	/************************************************************/
	virtual BOOL	InitializeGrabber(int nGrabberNo, eBoardModel eGrabberModel, int nDigCh, CString strDcfFilePath)
	{
		return FALSE;
	};
	virtual BOOL	OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
	{
		return FALSE;
	};
	virtual int		GetMonitoringInterval() { return -1; };
	virtual void	SetMonitoringInterval(int nInterval) { return; };
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#elif _HIKCAMERA
	/************************************************************/
	/*						 Hik Camera						*/
	/************************************************************/
	virtual void	CalcWhiteBalance(float* WhiteBalance) { return; };
	virtual void	SetWhiteBalance(float* WhiteBalance) { return; };
	virtual void	GetWhiteBalance(float* WhiteBalance) { return; };
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#elif _MATROXLINECAMERA
	/************************************************************/
	/*						 Matrox Camera						*/
	/************************************************************/
	virtual BOOL	InitializeGrabber(int nGrabberNo, eBoardModel eGrabberModel, int nDigCh, CString strDcfFilePath)
	{
		return FALSE;
	};
	virtual BOOL	OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
	{
		return FALSE;
	};
	virtual int		GetMonitoringInterval() { return -1; };
	virtual void	SetMonitoringInterval(int nInterval) { return; };
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#elif _NEDLINECAMERA
	/************************************************************/
	/*						 Ned Camera					*/
	/************************************************************/
	virtual bool	InitGrabber(int nGrabberNo, int nDigCh, CString strDcfFile) { return false; };
	virtual BOOL	OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
	{
		return FALSE;
	};
	virtual	void	CameraExpose() = 0;
	virtual void	SetSMemCurBuffer(UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };

#elif _DALSALINECAMERA
	/************************************************************/
	/*						 DalsaLine Camera					*/
	/************************************************************/
	virtual bool	InitGrabber(int nGrabberNo, int nDigCh, CString strDcfFile) { return false; };
	virtual BOOL	OpenCameraComPort(int nComPort, int nBaudrate, eCamModel eModel)
	{
		return FALSE;
	};
	virtual	void	CameraExpose(ST_LINE_INFO stLine) = 0;
	virtual void	SetSMemCurBuffer(int nTriggerCountF, int nTriggerCountB, UINT nGrabNum, TCHAR* strPanelID, TCHAR* strGrabStepName, int nSeqMode) { return; };
#endif
};


