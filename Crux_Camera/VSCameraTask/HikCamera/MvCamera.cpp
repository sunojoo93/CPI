#include "MvCamera.h"

CMvCamera::CMvCamera()
{
	m_hDevHandle = MV_NULL;
}

CMvCamera::~CMvCamera()
{
	if (m_hDevHandle)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}
}

// Get SDK Version
int CMvCamera::GetSDKVersion()
{
	return MV_CC_GetSDKVersion();
}

// Enumerate Device
int CMvCamera::EnumDevices(unsigned int nTLayerType, MV_CC_DEVICE_INFO_LIST* pstDevList)
{
	return MV_CC_EnumDevices(nTLayerType, pstDevList);
}

// Enum Interface Based On GenTL
int CMvCamera::EnumInterfacsByGenTL(MV_GENTL_IF_INFO_LIST* pstIFList, const char * pGenTLPath)
{
	return MV_CC_EnumInterfacesByGenTL(pstIFList, pGenTLPath);
}

// Enum Device Based On GenTL
int CMvCamera::EnumDevicesByGenTL(MV_GENTL_IF_INFO* pstIFInfo, MV_GENTL_DEV_INFO_LIST* pstDevList)
{
	return MV_CC_EnumDevicesByGenTL(pstIFInfo, pstDevList);
}

// Is the device accessible
bool CMvCamera::IsDeviceAccessible(MV_CC_DEVICE_INFO* pstDevInfo, unsigned int nAccessMode)
{
	return MV_CC_IsDeviceAccessible(pstDevInfo, nAccessMode);
}

// Open Device
int CMvCamera::Open(MV_CC_DEVICE_INFO* pstDeviceInfo)
{
	if (MV_NULL == pstDeviceInfo)
	{
		return MV_E_PARAMETER;
	}

	if (m_hDevHandle)
	{
		return MV_E_CALLORDER;
	}

	int nRet = MV_CC_CreateHandle(&m_hDevHandle, pstDeviceInfo);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	nRet = MV_CC_OpenDevice(m_hDevHandle);
	if (MV_OK != nRet)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}

	return nRet;
}

// Open Device Based On GenTL
int CMvCamera::OpenDeviceByGenTL(MV_GENTL_DEV_INFO* pstDeviceInfo)
{
	if (m_hDevHandle)
	{
		return MV_E_CALLORDER;
	}

	int nRet = MV_CC_CreateHandleByGenTL(&m_hDevHandle, pstDeviceInfo);
	if (MV_OK != nRet)
	{
		return nRet;
	}

	nRet = MV_CC_OpenDevice(m_hDevHandle);
	if (MV_OK != nRet)
	{
		MV_CC_DestroyHandle(m_hDevHandle);
		m_hDevHandle = MV_NULL;
	}

	return nRet;
}

// Close Device
int CMvCamera::Close()
{
	if (MV_NULL == m_hDevHandle)
	{
		return MV_E_HANDLE;
	}

	MV_CC_CloseDevice(m_hDevHandle);

	int nRet = MV_CC_DestroyHandle(m_hDevHandle);
	m_hDevHandle = MV_NULL;

	return nRet;
}

// Is The Device Connected
bool CMvCamera::IsDeviceConnected()
{
	return MV_CC_IsDeviceConnected(m_hDevHandle);
}

// Register Image Data CallBack
int CMvCamera::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser), void* pUser)
{
	return MV_CC_RegisterImageCallBackEx(m_hDevHandle, cbOutput, pUser);
}

// Start Grabbing
int CMvCamera::StartGrabbing()
{
	return MV_CC_StartGrabbing(m_hDevHandle);
}

// Stop Grabbing
int CMvCamera::StopGrabbing()
{
	return MV_CC_StopGrabbing(m_hDevHandle);
}

// Get one frame initiatively
int CMvCamera::GetImageBuffer(MV_FRAME_OUT* pFrame, int nMsec)
{
	return MV_CC_GetImageBuffer(m_hDevHandle, pFrame, nMsec);
}

// Free image buffer
int CMvCamera::FreeImageBuffer(MV_FRAME_OUT* pFrame)
{
	return MV_CC_FreeImageBuffer(m_hDevHandle, pFrame);
}

// Get one frame initiatively
int CMvCamera::GetOneFrameTimeout(unsigned char* pData, unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec)
{
	return MV_CC_GetOneFrameTimeout(m_hDevHandle, pData, nDataSize, pFrameInfo, nMsec);
}

// Set Display Window Handle
int CMvCamera::DisplayOneFrame(MV_DISPLAY_FRAME_INFO* pDisplayInfo)
{
	return MV_CC_DisplayOneFrame(m_hDevHandle, pDisplayInfo);
}

// Set the number of the internal image cache nodes in SDK
int CMvCamera::SetImageNodeNum(unsigned int nNum)
{
	return MV_CC_SetImageNodeNum(m_hDevHandle, nNum);
}

// Get device information
int CMvCamera::GetDeviceInfo(MV_CC_DEVICE_INFO* pstDevInfo)
{
	return MV_CC_GetDeviceInfo(m_hDevHandle, pstDevInfo);
}

// Get detect info of GEV camera
int CMvCamera::GetGevAllMatchInfo(MV_MATCH_INFO_NET_DETECT* pMatchInfoNetDetect)
{
	if (MV_NULL == pMatchInfoNetDetect)
	{
		return MV_E_PARAMETER;
	}

	MV_CC_DEVICE_INFO stDevInfo = { 0 };
	GetDeviceInfo(&stDevInfo);
	if (stDevInfo.nTLayerType != MV_GIGE_DEVICE)
	{
		return MV_E_SUPPORT;
	}

	MV_ALL_MATCH_INFO struMatchInfo = { 0 };

	struMatchInfo.nType = MV_MATCH_TYPE_NET_DETECT;
	struMatchInfo.pInfo = pMatchInfoNetDetect;
	struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_NET_DETECT);
	memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_NET_DETECT));

	return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// Get detect info of U3V camera
int CMvCamera::GetU3VAllMatchInfo(MV_MATCH_INFO_USB_DETECT* pMatchInfoUSBDetect)
{
	if (MV_NULL == pMatchInfoUSBDetect)
	{
		return MV_E_PARAMETER;
	}

	MV_CC_DEVICE_INFO stDevInfo = { 0 };
	GetDeviceInfo(&stDevInfo);
	if (stDevInfo.nTLayerType != MV_USB_DEVICE)
	{
		return MV_E_SUPPORT;
	}

	MV_ALL_MATCH_INFO struMatchInfo = { 0 };

	struMatchInfo.nType = MV_MATCH_TYPE_USB_DETECT;
	struMatchInfo.pInfo = pMatchInfoUSBDetect;
	struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_USB_DETECT);
	memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_USB_DETECT));

	return MV_CC_GetAllMatchInfo(m_hDevHandle, &struMatchInfo);
}

// Get Int type parameters, such as Width and Height, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetIntValue(IN const char* strKey, OUT MVCC_INTVALUE_EX *pIntValue)
{
	return MV_CC_GetIntValueEx(m_hDevHandle, strKey, pIntValue);
}

int CMvCamera::SetIntValue(IN const char* strKey, IN int64_t nValue)
{
	return MV_CC_SetIntValueEx(m_hDevHandle, strKey, nValue);
}

// Get Enum type parameters, such as PixelFormat, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetEnumValue(IN const char* strKey, OUT MVCC_ENUMVALUE *pEnumValue)
{
	return MV_CC_GetEnumValue(m_hDevHandle, strKey, pEnumValue);
}

int CMvCamera::SetEnumValue(IN const char* strKey, IN unsigned int nValue)
{
	return MV_CC_SetEnumValue(m_hDevHandle, strKey, nValue);
}

int CMvCamera::SetEnumValueByString(IN const char* strKey, IN const char* sValue)
{
	return MV_CC_SetEnumValueByString(m_hDevHandle, strKey, sValue);
}

// Get Float type parameters, such as ExposureTime and Gain, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetFloatValue(IN const char* strKey, OUT MVCC_FLOATVALUE *pFloatValue)
{
	return MV_CC_GetFloatValue(m_hDevHandle, strKey, pFloatValue);
}

int CMvCamera::SetFloatValue(IN const char* strKey, IN float fValue)
{
	return MV_CC_SetFloatValue(m_hDevHandle, strKey, fValue);
}

// Get Bool type parameters, such as ReverseX, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetBoolValue(IN const char* strKey, OUT bool *pbValue)
{
	return MV_CC_GetBoolValue(m_hDevHandle, strKey, pbValue);
}

int CMvCamera::SetBoolValue(IN const char* strKey, IN bool bValue)
{
	return MV_CC_SetBoolValue(m_hDevHandle, strKey, bValue);
}

// Get String type parameters, such as DeviceUserID, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::GetStringValue(IN const char* strKey, MVCC_STRINGVALUE *pStringValue)
{
	return MV_CC_GetStringValue(m_hDevHandle, strKey, pStringValue);
}

int CMvCamera::SetStringValue(IN const char* strKey, IN const char* strValue)
{
	return MV_CC_SetStringValue(m_hDevHandle, strKey, strValue);
}

// Execute Command once, such as UserSetSave, for details please refer to MvCameraNode.xlsx file under SDK installation directory
int CMvCamera::CommandExecute(IN const char* strKey)
{
	return MV_CC_SetCommandValue(m_hDevHandle, strKey);
}

// Detection network optimal package size(It only works for the GigE camera)
int CMvCamera::GetOptimalPacketSize(unsigned int* pOptimalPacketSize)
{
	if (MV_NULL == pOptimalPacketSize)
	{
		return MV_E_PARAMETER;
	}

	int nRet = MV_CC_GetOptimalPacketSize(m_hDevHandle);
	if (nRet < MV_OK)
	{
		return nRet;
	}

	*pOptimalPacketSize = (unsigned int)nRet;

	return MV_OK;
}

// Register Message Exception CallBack
int CMvCamera::RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser), void* pUser)
{
	return MV_CC_RegisterExceptionCallBack(m_hDevHandle, cbException, pUser);
}

// Register Event CallBack
int CMvCamera::RegisterEventCallBack(const char* pEventName, void(__stdcall* cbEvent)(MV_EVENT_OUT_INFO * pEventInfo, void* pUser), void* pUser)
{
	return MV_CC_RegisterEventCallBackEx(m_hDevHandle, pEventName, cbEvent, pUser);
}

// Force IP
int CMvCamera::ForceIp(unsigned int nIP, unsigned int nSubNetMask, unsigned int nDefaultGateWay)
{
	return MV_GIGE_ForceIpEx(m_hDevHandle, nIP, nSubNetMask, nDefaultGateWay);
}

// IP configuration method
int CMvCamera::SetIpConfig(unsigned int nType)
{
	return MV_GIGE_SetIpConfig(m_hDevHandle, nType);
}

// Set Net Transfer Mode
int CMvCamera::SetNetTransMode(unsigned int nType)
{
	return MV_GIGE_SetNetTransMode(m_hDevHandle, nType);
}

// Pixel format conversion
int CMvCamera::ConvertPixelType(MV_CC_PIXEL_CONVERT_PARAM* pstCvtParam)
{
	return MV_CC_ConvertPixelType(m_hDevHandle, pstCvtParam);
}

// save image
int CMvCamera::SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam)
{
	return MV_CC_SaveImageEx2(m_hDevHandle, pstParam);
}

// Save the image as a file
int CMvCamera::SaveImageToFile(MV_SAVE_IMG_TO_FILE_PARAM* pstSaveFileParam)
{
	return MV_CC_SaveImageToFile(m_hDevHandle, pstSaveFileParam);
}
