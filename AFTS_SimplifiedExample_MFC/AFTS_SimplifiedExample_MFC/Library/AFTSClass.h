#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "memory.h"
#include "AFTSControlLibrary.h"
#include "VwSDK.Error.h"
#include <atlstr.h>
#include <thread>


class AFTSSingleTone 
{
public:
	AFTSSingleTone(AFTSEventCallback Callback);
	virtual ~AFTSSingleTone(void);

	bool AFTSDiscovery(IN char* ACS_IPADDRESS, OUT AFM_HANDLE& m_hAFMModuleSingleTone, OUT int& nDiscoveredDevices);
	void Reg_EventCallback_ParentPointer(void *Pointer);
	AFTSEventCallback *AFTSEventCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	AFM_HANDLE hAFMModule;
	VWSDK::RESULT eRet;
	bool CloseAFTSHandle(bool bMotorEnable);

private:
	
	bool m_bDeviceDiscovered;
	int m_AFTSSingleTone_DeviceCount;
};

class AFTSControlLibrary
{
public:
	AFTSControlLibrary(int nIndex, AFM_HANDLE pAFMHandle, AFTSImageCallback _ImageCallback, AFTSEventCallback _EventCallback, AFTSParamCallback _ParamCallback,void* pParent);
	virtual ~AFTSControlLibrary(void);

	private:
	AFM_HANDLE hAFMModule;
	VWSDK::RESULT eRet;
public:
	void OpenAFCAM();
	void CloseAFCAM();

	int AFSwitchOn();
	int AFSwitchOff();

	void MeasureModeOn();
	void MeasureModeOff();

	void SetStreamingStatus(bool bSwitch);
	void SetZOffset(float fValue);
	void SetHaarFilterSize(int nValue);
	void SetAFLineNumber(int nValue);
	void SetAFLineSelectorIndex(int nValue);
	void SetCamExposure(float fValue);
	void SetMovingAvgFilterSwitch(bool bSwitch);
	void SetMovingAvgFilterSample(int nValue);
	void SetPeakThreshold(int nValue);

	void Set_SWLimitLeft(double value);
	void Set_SWLimitRight(double value);

	void Get_AFSwitch(OUT bool& _Status);
	void Get_AFCamStreaming(OUT bool& _Status);
	void Get_AFCamExposure(OUT float& fExposure);
	void Get_AFCamZOffset(OUT float& fZOffset);
	void Get_PeakThreshold(OUT int& nPeakThreshold);
	void Get_HaarSize(OUT int& nHaarSize);
	void Get_AFLineNumber(OUT int& nLineNumber);
	void Get_AFLineSelectorIndex(OUT int& nLineIndex);

	void Get_MotorEnabledStatus(OUT bool& _Status);
	void Get_SWLimitLeftStatus(OUT bool& _Status);
	void Get_SWLimitRightStatus(OUT bool& _Status);
	void Get_HWLimitLeftStatus(OUT bool& _Status);
	void Get_HWLimitRightStatus(OUT bool& _Status);

	void GetAFDiff_OffsetValue(OUT int& _Value);
	void GetAFDiffValue(OUT double& _Value);
	void GetAxisPoSValue(OUT double& _Value);

	int MoveZAxis(double dPoS, bool bRelative);
	void HomingZAxis();
	void SwitchZAxis(bool _bEnable);
	void StopZMotor();
	void SetZAxisSpeed(double dVelocity);

	void DoAFCalibration(float fRange, float fDelta);
	void SendTerminalCommand(char* Message, bool IsCameraCMD);

private:
	void RegCallback_Image(AFTSImageCallback *Pointer);
	void RegCallback_Event(AFTSEventCallback *Pointer);
	void RegCallback_AFTSParam(AFTSParamCallback *Pointer);

	AFTSImageCallback *AFTSImageCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	AFTSEventCallback *AFTSEventCallbackPointer;   //C#의 콜백루틴 받아오는 포인터변수
	AFTSParamCallback *AFTSParamCallbackPointer;

private:

	int m_nSPIndex;
	int m_nSPAxisIndex;
	int m_nZAxisIndex;

	int  m_nDeviceIndex;
	int  m_nDeviceCount;
	bool m_bDeviceDiscovered;

	const int SEQ_OK = 0;			///< Sequence Task 에서 사용 되어지는 OK
	const int SEQ_NG = 0xFFFF;		///< Sequence Task 에서 사용 되어지는 NG
	const int APP_OK = 0;			///< Application 에서 사용하는 정상 응답
	const int APP_NG = 1;			///< Application 에서 사용하는 이상 응답


	int CheckResult(VWSDK::RESULT FunctionReturn);
};
