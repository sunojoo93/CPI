#pragma once
#ifndef _H_VTS_DEFINE
#define _H_VTS_DEFINE

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif


#ifndef PURE
#define PURE	= 0
#endif

namespace VWSDK
{
	const int INVALID_AXIS_INDEX		= -1;			
	const int MAX_INVALID_AXIS_INDEX	= -999;			// Ver 2.0.4 

	const int MAX_WIDTH					= 1280;
	const int MAX_HEIGHT				= 1024;

	const int MAX_DEVICE_NUMBER			= 8;			// Ver 2.0.4, MAX ACS Device Number

	struct TVTS_DEVICE_INFO
	{
		char			chCamName[256];			// camera name
		int				nSPIndex;				// ACS SP number:		0, 1, 2, 3				카메라와 매칭되는 축을 못 찾을 시, INVALID_AXIS_INDEX 값 반환
		int				nSPAxisIndex;			// ACS SP Axis Index:	0, 1					카메라와 매칭되는 축을 못 찾을 시, INVALID_AXIS_INDEX 값 반환
		int				nAxisIndex;				// ACS Axis Index:		0, 1, 2, 3, ..., 7		카메라와 매칭되는 축을 못 찾을 시, INVALID_AXIS_INDEX 값 반환
	};

	struct TVTS_IMAGE_INFO
	{
		unsigned char*		pbImage;			// size = ( Width * Height ), BYTE
		unsigned int		width;
		unsigned int		height;
	};

	enum EVTS_FILTER_TYPE
	{
//		EVTS_FILTER_OFF = 0,
//		EVTS_FILTER_LPF = 1,
//		EVTS_FILTER_AVG = 2,
//		EVTS_FILTER_MOV_AVG = 3,

		EVTS_FILTER_OFF = 0,						// Ver 2.x.x 변경
		EVTS_FILTER_LPF = 1,
		EVTS_FILTER_AVG = 2,
		EVTS_FILTER_LPF_AVG = 3,					
		EVTS_FILTER_MOV_AVG = 4,
		EVTS_FILTER_LPF_MOV_AVG = 5,			
	};

	enum EVTS_PIXEL_DELTA
	{
//		EVTS_PIXEL_DELTA_1 = 0,
//		EVTS_PIXEL_DELTA_2 = 1,
//		EVTS_PIXEL_DELTA_3 = 2
		EVTS_PIXEL_DELTA_1 = 1,						// Ver 2.x.x 변경
		EVTS_PIXEL_DELTA_2 = 2,
		EVTS_PIXEL_DELTA_3 = 3

	};

	struct TVTS_CAM_PARAM
	{
		unsigned int	unWidth;
		unsigned int	unHeight;
		unsigned int	unOffsetX;
		unsigned int	unOffsetY;
		float			fFPS;
		float			fExposureTime;

		TVTS_CAM_PARAM()
		{
			unWidth = 1280;
			unHeight = 300;
			unOffsetX = 0;
			unOffsetY = 0;
			fFPS = 500;
			fExposureTime = 20;
		}
	};

	struct TVTS_AF_PARAM
	{
		bool				bRcvImage;				// 영상 전송 여부,															다수의 카메라를 사용하는 AF 모드에서는 false로 사용하는 것을 권장
		EVTS_PIXEL_DELTA	eCalcDelta;				// AF 연산 관련 변수,														기본값: EVTS_PIXEL_DELTA_1
		EVTS_FILTER_TYPE	eFilterType;			// AF 연산 시 필터 관련 옵션,												기본값: EVTS_FILTER_OFF
		unsigned int		unAvgCnt;				// Avg필터 사용 시 변수,													기본값: 1
		unsigned int		unDACZeroVal;			// AF Analog 출력 관련 영점 조절 변수,										카메라에 쓰여있는 값을 그대로 읽어와서 사용하는 것을 권장
		float				fOffsetZ;				// AF Calibration 이후, 별도로 출력값의 Offset을 조절하기 위한 변수,		기본값: 0.f [um]
		float				fGainZ;					// AF Calibration 이후, 별도로 출력값의 Gain을 조절하기 위한 변수,			기본값: 1.f
		float				fGainV;					// AF Analog 출력의 Gain을 조절하기 위한 변수								기본값: 1.f
		unsigned int		unPeakThresholdLine;	// Peak Threshold(자동계산) = Peak Threshold Line * ROI Length Y
													// Peak Threshold 이상의 값부터 밝기 인식,								기본값: 3

		unsigned int		unHaarSize;				// add Ver 2.0.0
		unsigned int		unDETLineNumber;		// add Ver 2.0.1
		unsigned int		unDETLineSelect;		// add Ver 2.0.0
		

		TVTS_AF_PARAM()
		{
			bRcvImage = true;
			eCalcDelta = EVTS_PIXEL_DELTA_1;
			eFilterType = EVTS_FILTER_OFF;
			unAvgCnt = 1;
			unDACZeroVal = 1;
			fOffsetZ = 0.f;
			fGainZ = 1.f;
			fGainV = 1.f;
			unPeakThresholdLine = 3;
			unHaarSize = 6;
			unDETLineNumber = 1;
			unDETLineSelect = 0;
			
		}
	};

	struct TVTS_CALIB_PARAM 						// 계산된 Calibration 관련 변수
	{
		int nCalibA;
		int nCalibB;
		int nCalibDet;								// Ver 2.0.0, Deprecated...
	};

	struct TVTS_MOTION_AF_PARAM						// ACS로부터 읽어오는 값
	{
		bool		bAFSwitch;						// AF_Switch
		float		fAFGain;						// AF_Gain
		int			nDiffOffset;					// Diff_Offset
		int			nAFFactor;						// AF_Factor
		int			nJudgeRange;					// Judge_Range
	};

	typedef void(*VTSDisconnectCallbackFn)(const TVTS_DEVICE_INFO&, void*);
	typedef void(*VTSImageGrabCallbackFn)(const TVTS_IMAGE_INFO&, void*);

	//////////////////////////////////////////////////////////////////////////

	struct TVTS_ACSC_CONNECTION_INFO
	{
		char	EthernetIP[100];
	};

	struct TVTS_VERSION
	{
		unsigned int			major;
		unsigned int			minor;
		unsigned int			revision;
	};
};
#endif