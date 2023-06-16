#pragma once


#ifdef VTS_EXPORTS
#define VWVTS_API __declspec(dllexport)
#else
#define VWVTS_API __declspec(dllimport)
#endif

#include "VwSDK.Error.h"
#include "VwVTSDefine.h"

namespace VWSDK
{
	typedef void* AFM_HANDLE;
	typedef void* DEVICE_HANDLE;
};

#ifdef __cplusplus
extern "C"
{
#endif
	namespace VWSDK
	{
		//////////////////////////////////////////////////////////////////////////
		//	Common
		//////////////////////////////////////////////////////////////////////////

		VWVTS_API VWSDK::RESULT GetLibraryVersion(OUT VWSDK::TVTS_VERSION& tVersion);

		VWVTS_API VWSDK::RESULT OpenVTS(IN const VWSDK::TVTS_ACSC_CONNECTION_INFO& tInfo, OUT VWSDK::AFM_HANDLE* hVwHandle);
		VWVTS_API VWSDK::RESULT CloseVTS(IN VWSDK::AFM_HANDLE& hVwHandle, IN bool bDisableAxis = false);

		VWVTS_API VWSDK::RESULT Discovery(IN VWSDK::AFM_HANDLE hVwHandle);
		VWVTS_API VWSDK::RESULT GetDeviceCount(IN VWSDK::AFM_HANDLE hVwHandle, int& nNumDevice);

		VWVTS_API VWSDK::RESULT GetDeviceInfoByIndex(IN VWSDK::AFM_HANDLE hVwHandle, int nIndex, VWSDK::TVTS_DEVICE_INFO& tDeviceInfo);

		VWVTS_API VWSDK::RESULT SendCommand(IN VWSDK::DEVICE_HANDLE hDevice, IN const char* chCommand, OUT char* chArg, INOUT size_t* pszArg, IN bool isCameraCMD);

		VWVTS_API VWSDK::RESULT OpenDevice(IN VWSDK::AFM_HANDLE hVwHandle, IN const VWSDK::TVTS_DEVICE_INFO& tDeviceInfo, OUT VWSDK::DEVICE_HANDLE* hDevice);
		VWVTS_API VWSDK::RESULT CloseDevice(IN VWSDK::AFM_HANDLE hVwHandle, INOUT VWSDK::DEVICE_HANDLE& hDevice);

		VWVTS_API VWSDK::RESULT GetDeviceInfo(IN VWSDK::DEVICE_HANDLE hDevice, OUT VWSDK::TVTS_DEVICE_INFO& tDeviceInfo);

		//////////////////////////////////////////////////////////////////////////
		//	Camera
		//////////////////////////////////////////////////////////////////////////

		VWVTS_API VWSDK::RESULT Start(IN VWSDK::DEVICE_HANDLE hDevice);
		VWVTS_API VWSDK::RESULT Stop(IN VWSDK::DEVICE_HANDLE hDevice);

		VWVTS_API VWSDK::RESULT GetGrabCondition(IN VWSDK::DEVICE_HANDLE hDevice, OUT bool& bGrabbing);

		VWVTS_API VWSDK::RESULT GetLaserPos(IN VWSDK::DEVICE_HANDLE hDevice, OUT double& dPixelX);
		VWVTS_API VWSDK::RESULT GetLaserPosRaw(IN VWSDK::DEVICE_HANDLE hDevice, OUT int& nRawPos);
		VWVTS_API VWSDK::RESULT GetCamZPos(IN VWSDK::DEVICE_HANDLE hDevice, OUT double&	dPos);

		VWVTS_API VWSDK::RESULT SetGrabParam(IN VWSDK::DEVICE_HANDLE hDevice, IN const VWSDK::TVTS_CAM_PARAM& tParam);
		VWVTS_API VWSDK::RESULT GetGrabParam(IN VWSDK::DEVICE_HANDLE hDevice, OUT VWSDK::TVTS_CAM_PARAM& tParam);

		VWVTS_API VWSDK::RESULT SetAFParam(IN VWSDK::DEVICE_HANDLE hDevice, IN const VWSDK::TVTS_AF_PARAM& tParam);
		VWVTS_API VWSDK::RESULT GetAFParam(IN VWSDK::DEVICE_HANDLE hDevice, OUT VWSDK::TVTS_AF_PARAM& tParam);

		VWVTS_API VWSDK::RESULT SetCalibParam(IN VWSDK::DEVICE_HANDLE hDevice, IN const VWSDK::TVTS_CALIB_PARAM& tParam);
		VWVTS_API VWSDK::RESULT GetCalibParam(IN VWSDK::DEVICE_HANDLE hDevice, OUT VWSDK::TVTS_CALIB_PARAM& tParam);

		VWVTS_API VWSDK::RESULT	SetCommand(IN VWSDK::DEVICE_HANDLE hDevice, IN const char* chCommand, IN char* chArg, IN size_t szArg);
		VWVTS_API VWSDK::RESULT GetCommand(IN VWSDK::DEVICE_HANDLE hDevice, IN const char* chCommand, OUT char* chArg, INOUT size_t* pszArg);


		VWVTS_API void			SetImageGrabCallback(IN VWSDK::DEVICE_HANDLE hDevice, VWSDK::VTSImageGrabCallbackFn pCallback, void* pUser);
		VWVTS_API void			SetDisconnectCallback(IN VWSDK::DEVICE_HANDLE hDevice, VWSDK::VTSDisconnectCallbackFn pCallback, void* pUser);

		//////////////////////////////////////////////////////////////////////////
		//	Motion
		//////////////////////////////////////////////////////////////////////////

		VWVTS_API VWSDK::RESULT DoHoming(IN VWSDK::DEVICE_HANDLE hDevice);
		VWVTS_API VWSDK::RESULT DoMultiHoming(IN VWSDK::DEVICE_HANDLE* hDeviceList, IN int nNumDevice);

		VWVTS_API VWSDK::RESULT SetEnable(IN VWSDK::DEVICE_HANDLE hDevice, IN bool bEnable);
		VWVTS_API VWSDK::RESULT GetState(IN VWSDK::DEVICE_HANDLE hDevice, OUT int& nAxisState);
		VWVTS_API VWSDK::RESULT GetPosition(IN VWSDK::DEVICE_HANDLE hDevice, OUT double& dPos);

		VWVTS_API VWSDK::RESULT GetAFMParam(IN VWSDK::DEVICE_HANDLE hDevice, OUT VWSDK::TVTS_MOTION_AF_PARAM& tParam);
		VWVTS_API VWSDK::RESULT SetAFMParam(IN VWSDK::DEVICE_HANDLE hDevice, IN const VWSDK::TVTS_MOTION_AF_PARAM tParam);

		VWVTS_API VWSDK::RESULT RunP2P(IN VWSDK::DEVICE_HANDLE hDevice, IN double dPos, IN bool bRelative);
		VWVTS_API VWSDK::RESULT RunMultiP2P(IN VWSDK::DEVICE_HANDLE* hDeviceList, IN double* dPosList, IN int nNumDevice, IN bool bRelative);	


	}
#ifdef __cplusplus
}
#endif

