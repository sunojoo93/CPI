#pragma once

#include <string>
#include <vector>

enum PARAM_OPTION
{
	PARAM_AF = 0x01,
	PARAM_CAM = 0x02,
	PARAM_CALIB = 0x03,
	PARAM_MOTIONAF = 0x04
};


enum LOGEVENT_OPTION
{
	MOTOR_AND_AFCAM_EVENT = 0x01,
	DISCOVERY_DEVICEINFO = 0x02,
	CUSTOM_COMMAND = 0x03,
	AF_CALIBRATION_DATA = 0x04,
	AF_CALIBRATION_RESULT = 0x05,
	AF_CAMERA_DISCONNECTED = 0x06,
};

typedef struct IMAGEDATA
{
	int _WIDTH;
	int _HEIGHT;
	void* _BUFFER;
	int _BUFFERINDEX;
	void* _pParent;
	IMAGEDATA()
	{
		_WIDTH = 0;
		_HEIGHT = 0;
		_BUFFER = NULL;
		_BUFFERINDEX = 0;
		_pParent = NULL;

	}
};

typedef struct EVENTDATA
{
	char* _MESSAGELOG;
	int	  _MESSAGESIZE;
	int	  _LOGINDEX;
	int   _AFTSAxisIndex;
	int   _AFTSCamIndex;
	void* _pParent;
	EVENTDATA()
	{
		_MESSAGELOG = "";
		_MESSAGESIZE = 0;
		_LOGINDEX = 0; //LOGINDEX = 0 -> F/G LOGINDEX = 1 -> MOTOR & AFCAM LOGINDEX = 2 -> Discovery & Getting Device Info
	}
};
#pragma pack(push, 1)
typedef struct AFTSParam
{
	double _ZPoS;
	double _XPoS;
	int   _AF_DiffOffset;
	int   _PeakThreshold;
	int   _HaarFilterSize;
	int   _AF_LineNumber;
	int   _AF_LineSelectorIndex;
	int   _Af_MovingAverageFilterSampleNumber;
	int   _Af_Factor;

	float _ZOffset;
	float _CAMExposure;
	float _AF_Diff;
	float _AF_SW_LeftLimitValue;
	float _AF_SW_RightLimitValue;
	
	bool  _CAMSTREAMING;
	bool  _AF_Switch;
	bool  _AF_MovingAverageFilterEnabled;
	bool  _AF_HW_RightLimit;
	bool  _AF_HW_LeftLimit;
	bool  _AF_SW_RightLimit;
	bool  _AF_SW_LeftLimit;
	void* _pParent;
	AFTSParam()
	{
		_ZPoS = 0;
		_XPoS = 0;
		_AF_DiffOffset = 0;
		_PeakThreshold = 10;
		_HaarFilterSize = 6;
		_AF_LineNumber = 1;
		_AF_LineSelectorIndex = 0;
		_Af_MovingAverageFilterSampleNumber = 0;

		_ZOffset = 0;
		_CAMExposure = 0;
		_AF_Diff = 0;
		_AF_SW_LeftLimitValue = 0;
		_AF_SW_RightLimitValue = 0;

		_CAMSTREAMING = false;
		_AF_Switch = false;
		_AF_MovingAverageFilterEnabled = false;
		_AF_HW_RightLimit = false;
		_AF_HW_LeftLimit = false;
		_AF_SW_RightLimit = false;
		_AF_SW_LeftLimit = false;
		_pParent = NULL;
	}
};
#pragma pack(pop)
void GetStringMessage(char* Message, int MessageSize, EVENTDATA* _Data);
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0' 
	if (size <= 0)
	{
		//throw std::runtime_error("Error during formatting.");
	}
	std::unique_ptr<char[]> buf(new char[size]);
	snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside }
}

typedef void* AFM_HANDLE;
typedef void* DEVICE_HANDLE;
typedef void __stdcall AFTSParamCallback(AFTSParam* _Data);
typedef void __stdcall AFTSEventCallback(EVENTDATA* _Data);
typedef void __stdcall AFTSImageCallback(IMAGEDATA* _Data);