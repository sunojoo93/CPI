#pragma once

#include "AFTSGlobal.h"
#include "VwSDK.Error.h"
#define DLL_AFTS __declspec(dllexport)

extern "C"
{
#pragma region AFTS SingleTone Class API 
	//For SingleTone Class - The purpose of this class is to get AFTS handle from the DiscoveryAFTS funtion.
	DLL_AFTS VWSDK::RESULT Create_AFTSSingleTone();
	DLL_AFTS VWSDK::RESULT DiscoveryAFTS(IN char* ACS_IPADDRESS, OUT AFM_HANDLE& m_hAFMModuleSingleTone, OUT int& nDiscoveredDevices); //C#에서 AFM_HANDLE은 intptr로 넘겨 받을 것. ref Intptr AFMHandle.
	DLL_AFTS VWSDK::RESULT Destroy_AFTSSingleTone();
	DLL_AFTS VWSDK::RESULT Register_Callback_Event_AFTSHandle(AFTSEventCallback *_EventPointer);
	DLL_AFTS VWSDK::RESULT Reg_Callback_Event_ParentPointer_AFTSHandle(void *Pointer);
	DLL_AFTS VWSDK::RESULT CloseAFTS(bool bMotorEnable);
	// SingleTone Class end
#pragma endregion

#pragma region AFTS Control Class API
	//AFTS Control Functions
	DLL_AFTS VWSDK::RESULT Create_AFTSClassPointer(int nAxis, void* AFM_Handle);
	DLL_AFTS VWSDK::RESULT Destroy_AFTSClassPointer(int nAxis);
	DLL_AFTS VWSDK::RESULT Register_Callback_Image(int nAxis,AFTSImageCallback *_ImagePointer);
	DLL_AFTS VWSDK::RESULT Register_Callback_Event(int nAxis,AFTSEventCallback *_EventPointer);
	DLL_AFTS VWSDK::RESULT Register_Callback_AFTSParam(int nAxis,AFTSParamCallback *_EventPointer);

	DLL_AFTS VWSDK::RESULT Reg_ImageCallback_ParentPointer(int nAxis,void *Pointer);
	DLL_AFTS VWSDK::RESULT Reg_EventCallback_ParentPointer(int nAxis,void *Pointer);
	DLL_AFTS VWSDK::RESULT Reg_AFTSParam_ParentPointer(int nAxis,void *Pointer);

	DLL_AFTS VWSDK::RESULT OpenAFCamera(int nAxis);

	DLL_AFTS VWSDK::RESULT StartAFCam(int nAxis);
	DLL_AFTS VWSDK::RESULT StopAFCam(int nAxis);
	DLL_AFTS VWSDK::RESULT AFON(int nAxis);
	DLL_AFTS VWSDK::RESULT AFStop(int nAxis);
	DLL_AFTS VWSDK::RESULT CloseAFCamera(int nAxis);

	DLL_AFTS VWSDK::RESULT AFCalibration(int nAxis,float fRange, float fDelta);
	DLL_AFTS VWSDK::RESULT SendCustomCommand(int nAxis, char* Message, bool IsCameraCMD);

	DLL_AFTS VWSDK::RESULT SetAFCamParam_StreamingSwitch(int nAxis, bool bSwitch);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_ZOffset(int nAxis, float fValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_HaarFilterSize(int nAxis, int nValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_AFLineNumber(int nAxis, int nValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_AFLineSelectorIndex(int nAxis, int nValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_CamExposure(int nAxis, float fValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_MovingAvgFilterSwitch(int nAxis, bool bSwitch);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_MovingAvgFilterSample(int nAxis, int nValue);
	DLL_AFTS VWSDK::RESULT SetAFCamParam_PeakThreshold(int nAxis, int nValue);
	
	DLL_AFTS VWSDK::RESULT GetAFCamParam_AFSwitchStatus(int nAxis,OUT bool& bAFSwitch);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_StreamingStatus(int nAxis,OUT bool& bStreaming);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_AFCamExposure(int nAxis,OUT float& fExposure);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_AFCamZOffset(int nAxis,OUT float& fZOffset);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_PeakThreshold(int nAxis, OUT int& nPeakThreshold);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_HaarSize(int nAxis, OUT int& nHaarSize);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_AFLineNumber(int nAxis, OUT int& nLineNumber);
	DLL_AFTS VWSDK::RESULT GetAFCamParam_AFLineSelectorIndex(int nAxis, OUT int& nLineIndex);

	DLL_AFTS VWSDK::RESULT HomingAxis(int nAxis);
	DLL_AFTS VWSDK::RESULT SwitchAxis(int nAxis, bool _bEnable);
	DLL_AFTS VWSDK::RESULT MoveAxis(int nAxis, double dPoS, bool bRelative);
	DLL_AFTS VWSDK::RESULT GetAxisPoS(int nAxis, OUT double& PoS);

	DLL_AFTS VWSDK::RESULT GetMotorEnabledStatus(int nAxis, OUT bool & bAxisEnabled);
	DLL_AFTS VWSDK::RESULT SetSWLimitLeft(int nAxis, double Limit);
	DLL_AFTS VWSDK::RESULT SetSWLimitRight(int nAxis, double Limit);

	DLL_AFTS VWSDK::RESULT GetSWLimitLeftCurrentValue(int nAxis, OUT double & CurSWLimitLeft);
	DLL_AFTS VWSDK::RESULT GetSWLimitRightCurrentValue(int nAxis, OUT double & CurSWLimitRight);

	DLL_AFTS VWSDK::RESULT GetSWLimitLeftStatus(int nAxis, OUT bool & bSWLimitLeft);
	DLL_AFTS VWSDK::RESULT GetSWLimitRightStatus(int nAxis, OUT bool & bSWLimitRight);
	
	DLL_AFTS VWSDK::RESULT GetHWLimitLeftStatus(int nAxis, OUT bool & bHWLimitLeft);
	DLL_AFTS VWSDK::RESULT GetHWLimitRightStatus(int nAxis, OUT bool & bHWLimitRight);

	DLL_AFTS VWSDK::RESULT StopMotor(int nAxis);
	DLL_AFTS VWSDK::RESULT SetAxisSpeed(int nAxis, double dVelocity);
	DLL_AFTS VWSDK::RESULT GetAFDiff(int nAxis,OUT double& dAfDiff);
	DLL_AFTS VWSDK::RESULT GetAFDiff_Offset(int nAxis,OUT int& nDiffOffset);

	//AFTS Control Functions End
#pragma endregion

#pragma region X_AXIS Control API (Available when TDI SCAN Motor is connected to ACS Controller)
	//X_AXIS Control - Available when X_Axis motors are connected to ACS Controller.
	DLL_AFTS VWSDK::RESULT Homing_XAxis(int nAxis, int XAxis);
	DLL_AFTS VWSDK::RESULT Switch_XAxis(int nAxis, int XAxis, bool _bEnable);
	DLL_AFTS VWSDK::RESULT Get_XAxisPoS(int nAxis, int XAxis, OUT double& dPos);
	DLL_AFTS VWSDK::RESULT Move_XAxis(int nAxis, int XAxis, double dPoS, bool bRelative);
	DLL_AFTS VWSDK::RESULT Stop_XAxis(int nAxis, int XAxis);
	DLL_AFTS VWSDK::RESULT Set_XAxisSpeed(int nAxis, int XAxis, double dVelocity);
	DLL_AFTS VWSDK::RESULT Get_XAXISEnabledStatus(int nAxis, int XAxis, OUT bool & bAxisEnabled);
#pragma endregion

#pragma region AFTS Scope API (Currently Not available)
	//Scope API - Currently, it is not allowed for users to use. 
	DLL_AFTS VWSDK::RESULT GetAFDiff_Scope(int nAxis, OUT double& dAfDiff);
	DLL_AFTS VWSDK::RESULT GetFeedbackZPoS_Scope(int nAxis, OUT double& dFeedbackZPoS);
	DLL_AFTS VWSDK::RESULT GetStepResponseTime(int nAxis, int AfFactor, float StartPoS, float EndPoS, OUT double& dStepResponse);
	DLL_AFTS VWSDK::RESULT SetAFFactor(int nAxis,int AfFactor);
#pragma endregion
}
