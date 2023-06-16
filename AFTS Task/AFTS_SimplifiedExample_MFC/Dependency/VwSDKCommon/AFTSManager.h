#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "memory.h"
#include "VTS_API.h"
#include <atlstr.h>
#include <thread>
#include "timecheck.h"
#include "AFTSGlobal.h"

using namespace VWSDK;
class AFTSSingleTone //AFM_HANDLE 받기 위한 SINGLETONE CLASS
{
public:
	AFTSSingleTone();
	virtual ~AFTSSingleTone(void);
	
	BOOL DiscoveryAFTS(IN char* ACS_IPADDRESS, OUT AFM_HANDLE& m_hAFMModuleSingleTone, OUT int& nDiscoveredDevices);
	void CloseAFTS(bool bMotorEnable); // 변경점
	void EventCallback(EVENTDATA* _Data);
	void RegCallback_Event(AFTSEventCallback *Pointer);

	//20220814
	void Reg_EventCallback_ParentPointer(void *Pointer);
	AFTSEventCallback *AFTSEventCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	EVENTDATA m_EvtData;

private:
	void Addlog(std::string Message, int LogIndex);
	char CharMessage[1024];

private:
	AFM_HANDLE hAFMModule;
	BOOL m_bDeviceDiscovered;
	RESULT eRet;
	int m_AFTSSingleTone_DeviceCount;
	TVTS_VERSION LibraryVersion;
};

class AFTSControlLibrary : public CTimeCheck
{
	struct TVTS_AF_RECIPE
	{
		TVTS_AF_PARAM			tParamAF;
		TVTS_CAM_PARAM			tParamCAM;
		TVTS_CALIB_PARAM		tParamCalib;
		TVTS_MOTION_AF_PARAM	tParamMotionAF;

		TVTS_AF_RECIPE() : tParamAF(), tParamCAM(), tParamCalib(), tParamMotionAF() {};

		TVTS_AF_RECIPE(const TVTS_AF_PARAM& paramAF, const TVTS_CAM_PARAM& paramCam, const TVTS_CALIB_PARAM& paramCalib, const TVTS_MOTION_AF_PARAM& paramMotionAF)
			: tParamAF(paramAF), tParamCAM(paramCam), tParamCalib(paramCalib), tParamMotionAF(paramMotionAF) {};

		TVTS_AF_PARAM			GetAFParam()		const { return tParamAF; };
		TVTS_CAM_PARAM			GetCamParam()		const { return tParamCAM; };
		TVTS_CALIB_PARAM		GetCalibParam()		const { return tParamCalib; };
		TVTS_MOTION_AF_PARAM	GetMotionParam()	const { return tParamMotionAF; };
	};

public:
	AFTSControlLibrary(int nIndex, AFM_HANDLE pAFMHandle);
	virtual ~AFTSControlLibrary(void);
private:
	AFM_HANDLE hAFMModule;
	DEVICE_HANDLE	m_DeviceHandle;
	RESULT eRet;

	TVTS_AF_RECIPE m_tAFRecipe;
	TVTS_ACSC_CONNECTION_INFO tConnectionInfo;
	TVTS_IMAGE_INFO  tImageInfo;
	TVTS_DEVICE_INFO tDeviceInfo;

public:

	// Motor Param Functions

	void HomingAxis(int nAxis);
	void SwitchAxis(int nAxis, bool _bEnable);
	void GetAxisPoS(int nAxis, OUT double& dPos);
	void MoveAxis(int nAxis, double dPoS, bool bRelative);
	
	void GetMotorEnabledStatus(int nAxis, OUT bool & bAxisEnabled);
	void GetSWLimitLeftCurrentValue(int nAxis, OUT double & CurSWLimitLeft);
	void GetSWLimitRightCurrentValue(int nAxis, OUT double & CurSWLimitRight);
	void GetSWLimitLeftStatus(int nAxis, OUT bool & bSWLimitLeft);
	void GetSWLimitRightStatus(int nAxis, OUT bool & bSWLimitRight);
	void GetHWLimitLeftStatus(int nAxis, OUT bool & bHWLimitLeft);
	void GetHWLimitRightStatus(int nAxis, OUT bool & bHWLimitRight);

	void SetSWLimitLeft(int nAxis, double Limit);
	void SetSWLimitRight(int nAxis, double Limit);
	void StopMotor(int nAxis); // 테스트용 함수
	void SetAxisSpeed(int nAxis, double dVelocity);
	
	double m_fStartPoS;
	double m_fEndPoS;

	void GetAFDiff(OUT double& dAfDiff);	
	void GetAFDiff_Offset(OUT int& nDiffOffset);
	double m_dAFDiff;

	// AF Cam Functions

	void OpenAFCamera();
	void CloseAFCamera();
	void StartAFCam();
	void AFON();
	void StopAFCam();
	void AFStop();
	void AFCalibration(float fRange, float fDelta);
	void SendCustomCommand(const char* Message, bool IsCameraCMD);

	void SetAFCamParam_StreamingSwitch(bool bSwitch);
	void SetAFCamParam_ZOffset(float fValue);
	void SetAFCamParam_HaarFilterSize(int nValue);
	void SetAFCamParam_AFLineNumber(int nValue);
	void SetAFCamParam_AFLineSelectorIndex(int nValue);
	void SetAFCamParam_CamExposure(float fValue);
	void SetAFCamParam_MovingAvgFilterSwitch(bool bSwitch);
	void SetAFCamParam_MovingAvgFilterSample(int nValue);
	void SetAFCamParam_PeakThreshold(int nValue);

	void GetAFCamParam_AFSwitchStatus(OUT bool& bAFSwitch);
	void GetAFCamParam_StreamingStatus(OUT bool& bStreaming);
	void GetAFCamParam_AFCamExposure(OUT float& fExposure);
	void GetAFCamParam_AFCamZOffset(OUT float& fZOffset);
	void GetAFCamParam_PeakThreshold(OUT int& nPeakThreshold);
	void GetAFCamParam_HaarSize(OUT int& nHaarSize);
	void GetAFCamParam_AFLineNumber(OUT int& nLineNumber);
	void GetAFCamParam_AFLineSelectorIndex(OUT int& nLineIndex);

	void GetAFTSParameters(PARAM_OPTION Option); // This function is not imported as DLL. Only Internal usage.

	//Legacy Codes
	//BOOL SetAFTSParameters(SETTING_OPTION Option);
	//BOOL SetAFTSParameters(SETTING_OPTION Option, int value);
	//BOOL SetAFTSParameters(SETTING_OPTION Option, float value);
	//BOOL SetAFTSParameters(SETTING_OPTION Option, char* value);
	//void GetAFSwitchStatus(OUT bool& bAFSwitch);
	//void GetStreamingStatus(OUT bool& bStreaming);
	//void GetAFCamExposure(OUT float& dExposure);
	//void GetAFCamZOffset(OUT float& dZOffset);
	//void GetPeakThreshold(OUT int& nPeakThreshold);
	//void GetHaarSize(OUT int& nHaarSize);
	//void GetAFLineNumber(OUT int& nLineNumber);
	//void GetAFLineSelectorIndex(OUT int& nLineIndex);

	//////////////////////////////////////////////////////////

	//Callback Function
	static void AFCAM_LaserImageCallback(const VWSDK::TVTS_IMAGE_INFO& tImageInfo, void* pUserPointer);
	static void AFCAM_DisconnectionCallback(const VWSDK::TVTS_DEVICE_INFO& tCamInfo, void* pUserPointer);

	AFTSImageCallback *AFTSImageCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	AFTSEventCallback *AFTSEventCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	AFTSParamCallback *AFTSParamCallbackPointer;
	IMAGEDATA m_IMGData;
	EVENTDATA m_EvtData;
	AFTSParam m_AFTSParam;

	void ImageCallback(IMAGEDATA* _Data);
	void EventCallback(EVENTDATA* _Data);
	void AFTSParameterCallback(AFTSParam* _Data);
	void RegCallback_Image(AFTSImageCallback *Pointer);
	void RegCallback_Event(AFTSEventCallback *Pointer);
	void RegCallback_AFTSParam(AFTSParamCallback *Pointer);

	void Reg_ImageCallback_ParentPointer(void *Pointer);
	void Reg_EventCallback_ParentPointer(void *Pointer);
	void Reg_AFTSParam_ParentPointer(void *Pointer);

	// Following functions are only for DEMO, not for USER! Do not use these functions.
	void Homing_XAxis(int XAxis);
	void Switch_XAxis(int XAxis, bool _bEnable);
	void Get_XAxisPoS(int XAxis, OUT double& dPos);
	void Move_XAxis(int XAxis, double dPoS, bool bRelative);
	void Stop_XAxis(int XAxis); // 테스트용 함수
	void Set_XAxisSpeed(int XAxis, double dVelocity);
	void Get_XAXISEnabledStatus(int nAxis, OUT bool & bAxisEnabled);

	void GetAFDiff_Scope(OUT double& dAfDiff);
	void GetFeedbackZPoS_Scope(OUT double& dFeedbackZPoS);
	void GetStepResponseTime(int AfFactor, float StartPoS, float EndPoS, OUT double& dStepResponse);
	void SetAFFactor(int AfFactor);

private:
	int m_nSPIndex;
	int m_nSPAxisIndex;
	int m_nZAxisIndex;

	int  m_nDeviceIndex;
	int  m_nDeviceCount;
	BOOL m_bDeviceDiscovered;
	std::thread* m_pMotorHomingThread;
	std::thread* m_pMotorResponseTimeCheckThread;

private:
	void Addlog(std::string Message, int LogIndex);
	char CharMessage[1024];

// AFTS Calibration Part

private:

	void StartCalibrationThread(float fRange, float fDelta);
	void StopCalibrationThread();

	std::vector<std::pair<int, float>> InitCalibSet(float fRange, float fDelta);
	BOOL RunCalibration(const std::vector<std::pair<int, float>>& vecCalibSet, VWSDK::TVTS_CALIB_PARAM& tResult);
	BOOL CollectCalibSet(std::vector<std::pair<int, float>>& vecCalibSet, BOOL& bRunCalib);
	BOOL SetCalibrationTarget();

	std::thread*								m_pthCalibration;
	BOOL										m_bRunCalibrationThread;
	int											m_nLaserPosInfocus;
	float										m_fAxisZPos0;
	double										m_daAxisPos;
};
