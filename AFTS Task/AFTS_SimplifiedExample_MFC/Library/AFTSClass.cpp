#include "pch.h"
#include "AFTSClass.h"

AFTSSingleTone::AFTSSingleTone(AFTSEventCallback Callback)
{
	Create_AFTSSingleTone();
	AFTSEventCallbackPointer = Callback;
	Register_Callback_Event_AFTSHandle(AFTSEventCallbackPointer);	
	m_AFTSSingleTone_DeviceCount = 0;
	m_bDeviceDiscovered = false;
}

AFTSSingleTone::~AFTSSingleTone(void)
{
	CloseAFTS(false);
}

void AFTSSingleTone::Reg_EventCallback_ParentPointer(void *Pointer)
{
	Reg_Callback_Event_ParentPointer_AFTSHandle(Pointer);
}

bool AFTSSingleTone::AFTSDiscovery(IN char* ACS_IPADDRESS, OUT AFM_HANDLE& m_hAFMModuleSingleTone, OUT int& nDiscoveredDevices)
{
	try
	{
	
		if (m_bDeviceDiscovered == false)
		{
			DiscoveryAFTS(ACS_IPADDRESS, hAFMModule, m_AFTSSingleTone_DeviceCount);		
			m_hAFMModuleSingleTone = hAFMModule;
			nDiscoveredDevices = m_AFTSSingleTone_DeviceCount;
			return true;
		}
		else if (m_bDeviceDiscovered == true)
		{			
			return false;
		}		
	}
	catch (...)
	{
		return false;
	}
}

bool AFTSSingleTone::CloseAFTSHandle(bool bMotorEnable)
{
	try
	{
		if (m_bDeviceDiscovered == false)
		{
			return false;
		}
		else if (m_bDeviceDiscovered == true)
		{
			CloseAFTS(bMotorEnable);
			return true;
		}
	}
	catch (...)
	{
		return false;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
AFTSControlLibrary::AFTSControlLibrary(int AxisIndex, AFM_HANDLE pAFMHandle, AFTSImageCallback _ImageCallback, AFTSEventCallback _EventCallback, AFTSParamCallback _ParamCallback, void* pParent)
	:
	hAFMModule(pAFMHandle),
	m_nDeviceIndex(AxisIndex) // 모터 현재 축 번호
{
	Create_AFTSClassPointer(AxisIndex, hAFMModule);
	AFTSImageCallbackPointer = _ImageCallback; //AF 카메라 레이저 영상을 받기 위한 콜백 포인터.
	AFTSEventCallbackPointer = _EventCallback; //로그 및 관련 이벤트를 전달 받기 위한 콜백 포인터.
	AFTSParamCallbackPointer = _ParamCallback; //AFTS 구조체의 값을 전달 받기 위한 콜백 포인터. 
	Reg_ImageCallback_ParentPointer(m_nDeviceIndex, pParent);
	Reg_EventCallback_ParentPointer(m_nDeviceIndex, pParent);
	Reg_AFTSParam_ParentPointer(m_nDeviceIndex, pParent);
}	
//
AFTSControlLibrary::~AFTSControlLibrary(void)
{

}
//
void AFTSControlLibrary::OpenAFCAM()
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	RegCallback_Image(AFTSImageCallbackPointer);
	RegCallback_Event(AFTSEventCallbackPointer);
	RegCallback_AFTSParam(AFTSParamCallbackPointer);
	CheckResult(OpenAFCamera(m_nDeviceIndex));
	return;
}

void AFTSControlLibrary::RegCallback_Image(AFTSImageCallback *Pointer)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(Register_Callback_Image(m_nDeviceIndex, Pointer));
	return;
}

void AFTSControlLibrary::RegCallback_Event(AFTSEventCallback *Pointer)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(Register_Callback_Event(m_nDeviceIndex, Pointer));
	return;
	
}

void AFTSControlLibrary::RegCallback_AFTSParam(AFTSParamCallback *Pointer)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(Register_Callback_AFTSParam(m_nDeviceIndex, Pointer));
	return;
}


void AFTSControlLibrary::CloseAFCAM()
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(CloseAFCamera(m_nDeviceIndex));
}


void AFTSControlLibrary::SetStreamingStatus(bool bSwitch)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_StreamingSwitch(m_nDeviceIndex, bSwitch));
}

void AFTSControlLibrary::SetZOffset(float fValue)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_ZOffset(m_nDeviceIndex, fValue));
}
void AFTSControlLibrary::SetHaarFilterSize(int nValue)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_HaarFilterSize(m_nDeviceIndex, nValue));	
}
void AFTSControlLibrary::SetAFLineNumber(int nValue)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_AFLineNumber(m_nDeviceIndex, nValue));
}

void AFTSControlLibrary::SetAFLineSelectorIndex(int nValue)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_AFLineSelectorIndex(m_nDeviceIndex, nValue));
}

void AFTSControlLibrary::SetCamExposure(float fValue)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_CamExposure(m_nDeviceIndex, fValue));
}

void AFTSControlLibrary::SetMovingAvgFilterSwitch(bool bSwitch)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_MovingAvgFilterSwitch(m_nDeviceIndex, bSwitch));
}

void AFTSControlLibrary::SetMovingAvgFilterSample(int nValue)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_MovingAvgFilterSample(m_nDeviceIndex, nValue));
}

void AFTSControlLibrary::SetPeakThreshold(int nValue)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAFCamParam_PeakThreshold(m_nDeviceIndex, nValue));
}

int AFTSControlLibrary::AFSwitchOn()
{	
	int nRet = APP_OK;

	if (hAFMModule == NULL)
	{
		return APP_NG;	
	}
	
	nRet = CheckResult(StartAFCam(m_nDeviceIndex));
	nRet = CheckResult(AFON(m_nDeviceIndex));

	return nRet;
}
int AFTSControlLibrary::AFSwitchOff()
{
	int nRet = APP_OK;

	if (hAFMModule == NULL)
	{
		return APP_NG;
	}
	nRet =  CheckResult(StopAFCam(m_nDeviceIndex));
	nRet =  CheckResult(AFStop(m_nDeviceIndex));

	return nRet;
}
void AFTSControlLibrary::MeasureModeOn()
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(StartAFCam(m_nDeviceIndex));
}
void AFTSControlLibrary::MeasureModeOff()
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(StopAFCam(m_nDeviceIndex));	
}

void AFTSControlLibrary::Get_AFSwitch(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;	
	CheckResult(GetAFCamParam_AFSwitchStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_AFCamStreaming(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value = false;
	CheckResult(GetAFCamParam_StreamingStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_AFCamExposure( OUT float& fExposure)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	float _Value = 0;
	CheckResult(GetAFCamParam_AFCamExposure(m_nDeviceIndex, _Value));
	fExposure = _Value;
	return;
}
void AFTSControlLibrary::Get_AFCamZOffset(OUT float& fZOffset)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	float _Value = 0;
	CheckResult(GetAFCamParam_AFCamZOffset(m_nDeviceIndex, _Value));
	fZOffset = _Value;
	return;
}
void AFTSControlLibrary::Get_PeakThreshold(OUT int& nPeakThreshold)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	int _Value = 0;
	CheckResult(GetAFCamParam_PeakThreshold(m_nDeviceIndex, _Value));
	nPeakThreshold = _Value;
	return;
}
void AFTSControlLibrary::Get_HaarSize(OUT int& nHaarSize)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	int _Value = 0;
	CheckResult(GetAFCamParam_HaarSize(m_nDeviceIndex, _Value));
	nHaarSize = _Value;
	return;
}
void AFTSControlLibrary::Get_AFLineNumber(OUT int& nLineNumber)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	int _Value = 0;
	CheckResult(GetAFCamParam_AFLineNumber(m_nDeviceIndex, _Value));
	nLineNumber = _Value;
	return;
}
void AFTSControlLibrary::Get_AFLineSelectorIndex(OUT int& nLineIndex)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	int _Value = 0;
	CheckResult(GetAFCamParam_AFLineSelectorIndex(m_nDeviceIndex, _Value));
	nLineIndex = _Value;
	return;
}

void AFTSControlLibrary::Get_MotorEnabledStatus(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;
	CheckResult(GetMotorEnabledStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_SWLimitLeftStatus(OUT bool& _Status)
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;
	CheckResult(GetSWLimitLeftStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_SWLimitRightStatus(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;
	CheckResult(GetSWLimitRightStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_HWLimitLeftStatus(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;
	CheckResult(GetHWLimitLeftStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::Get_HWLimitRightStatus(OUT bool& _Status)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	bool _Value;
	CheckResult(GetHWLimitRightStatus(m_nDeviceIndex, _Value));
	_Status = _Value;
	return;
}

void AFTSControlLibrary::GetAFDiff_OffsetValue(OUT int& _Value)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	int Value;
	CheckResult(GetAFDiff_Offset(m_nDeviceIndex, Value));
	_Value = Value;
	return;
}

void AFTSControlLibrary::GetAFDiffValue(OUT double& _Value)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	double Value;
	CheckResult(GetAFDiff(m_nDeviceIndex, Value));
	_Value = Value;
	return;
}

void AFTSControlLibrary::GetAxisPoSValue(OUT double& _Value)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	double Value;
	CheckResult(GetAxisPoS(m_nDeviceIndex, Value));
	_Value = Value;
	return;
}

int AFTSControlLibrary::MoveZAxis(double dPoS, bool bRelative)
{
	
	if (hAFMModule == NULL)
	{		
		return APP_NG;
	}
	return CheckResult(MoveAxis(m_nDeviceIndex, dPoS, bRelative));

}
void AFTSControlLibrary::HomingZAxis()
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(HomingAxis(m_nDeviceIndex));
	return;
}
void AFTSControlLibrary::SwitchZAxis(bool _bEnable)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SwitchAxis(m_nDeviceIndex,_bEnable));
	return;
}
void AFTSControlLibrary::StopZMotor()
{	
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(StopMotor(m_nDeviceIndex));
	return;
}
void AFTSControlLibrary::SetZAxisSpeed(double dVelocity)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetAxisSpeed(m_nDeviceIndex, dVelocity));
	return;
}

void AFTSControlLibrary::SendTerminalCommand(char* Message, bool IsCameraCMD)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SendCustomCommand(m_nDeviceIndex,Message, IsCameraCMD));
	return;
}

void AFTSControlLibrary::DoAFCalibration(float fRange, float fDelta)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(AFCalibration(m_nDeviceIndex, fRange, fDelta));
	return;
}

void AFTSControlLibrary::Set_SWLimitLeft(double value)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetSWLimitLeft(m_nDeviceIndex, value));
	return;
}

void AFTSControlLibrary::Set_SWLimitRight(double value)
{
	if (hAFMModule == NULL)
	{
		return;
	}
	CheckResult(SetSWLimitRight(m_nDeviceIndex,value));
	return;
}

int AFTSControlLibrary::CheckResult(VWSDK::RESULT FunctionReturn)
{
	if (FunctionReturn == VWSDK::RESULT_SUCCESS)
	{
		return APP_OK;
	}
	else
	{
		OutputDebugString(_T("Error!"));
		return APP_NG;
	}
}