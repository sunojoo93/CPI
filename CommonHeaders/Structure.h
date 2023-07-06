#if !defined(AFX_VSSTRUCTURE_H_INCLUDED_)
#define AFX_VSSTRUCTURE_H_INCLUDED_

#define MAX_STAGE_COUNT				6
#define MAX_LIGHT_PORT_COUNT		4
#define MAX_LIGHT_CONTROLLER_COUNT	4
//#define MAX_LIGHT_CHANNEL_COUNT		24 //16 -> 24 
#define MAX_FRAME_GRABBER_COUNT		4
//#define MAX_CAMERA_COUNT			4
#define MAX_THREAD_PARAM_COUNT      10
#define MAX_GRAB_STEP_COUNT			150


#define  MAX_AREA_COUNT  5
#define  MAX_PATTERN_COUNT  5
#define  MAX_CAMERA_COUNT  4
#define  MAX_AUTOFOCUS_COUNT 4
#define  MAX_LIGHT_COUNT  4
#define  MAX_LIGHT_CHANNEL_COUNT  28

#define MAX_IMAGE_RATIO				1

#define TACT_GRAB					    _T("GRAB")
#define TACT_INSP					    _T("INSP")
#define TACT_START					    _T("START")
#define TACT_END					    _T("END")
#define TACT_TOTAL						_T("TOTAL")

#pragma pack(push)
#pragma pack(1)

struct ST_CAMERA_COND
{
	BOOL	bUseCamera;
	DOUBLE	dExpTime;
	double	dAnalogGain;
	UINT	nSeqMode;

	ST_CAMERA_COND()
	{
		bUseCamera = FALSE;
		dExpTime = 0.0;
		dAnalogGain = 0;
		nSeqMode = 0;
	}
};

struct STRU_SERIAL_INFO
{
	UINT  nChCnt;
	UINT  nLightVal[MAX_LIGHT_CHANNEL_COUNT];

	STRU_SERIAL_INFO()
	{
		nChCnt = 0;
		memset(nLightVal, 0, sizeof(UINT) * MAX_LIGHT_CHANNEL_COUNT);
	};
};
struct STRU_SERIAL_INFO_AOT
{
	UINT  nChCnt;
	UINT  nLightVal[MAX_LIGHT_CHANNEL_COUNT];

	STRU_SERIAL_INFO_AOT()
	{
		nChCnt = 0;
		memset(nLightVal, 0, sizeof(UINT) * MAX_LIGHT_CHANNEL_COUNT);
	};
};

struct STRU_LIGHT_INFO
{
	UINT nPortNo;
	UINT nControllerCount;
	STRU_SERIAL_INFO stLight[MAX_LIGHT_CONTROLLER_COUNT];

	STRU_LIGHT_INFO()
	{
		nPortNo = 0;
		nControllerCount = 0;
		memset(stLight, 0, sizeof(STRU_SERIAL_INFO) * MAX_LIGHT_CONTROLLER_COUNT);
	}
};


struct ST_PG_DATA
{
	TCHAR	strPatternName[100];
	BOOL	bJudge;
	WORD	wVoltage[3];
	double  dColor_X;
	double	dColor_Y;
	double	dLuminance;
	int     nPtnNum;
	ST_PG_DATA()
	{
		memset(strPatternName, 0, sizeof(strPatternName));
		memset(wVoltage, 0, sizeof(wVoltage));
		bJudge = FALSE;
		nPtnNum = 0;
	}
};

struct ST_LINE_DATA
{
	int nCOUNTF;
	int nCountB;
	int nStartF;
	int nStartB;
	int nStopF;
	int nStopB;
	int nPeriodF;
	int nPeriodB;
	double dExpose;
	double dGain;
	TCHAR strDirection[50];
	int nCurrentGrab;

	ST_LINE_DATA()
	{
		nCOUNTF = 0;
		nCountB = 0;
		nStartF = 0;
		nStartB = 0;
		nStopF = 0;
		nStopB = 0;
		nPeriodF = 0;
		nPeriodB = 0;
		memset(strDirection, 0, sizeof(strDirection));
		nCurrentGrab = 0;
	}
};

struct PARAM_PG // PG에 전송하는 파라미터
{
	TCHAR	strChIndex[200]; // RGB_CH_INDEX
	int		nChIndexNum; // RGB_CH_INDEX 갯수

	ST_PG_DATA pVoltageInfo; // Voltage 정보

	PARAM_PG()
	{
		memset(strChIndex, 0, sizeof(strChIndex));
		memset(&pVoltageInfo, 0, sizeof(ST_PG_DATA));
		nChIndexNum = 0;
	}


};
struct ST_PG_INFO // 시퀀스가 PGController.ini와 MTP Result 파일을 읽어서 가지고 있는 구조체
{
	TCHAR	strChIndex[200]; // RGB_CH_INDEX
	int		nChIndexNum; // RGB_CH_INDEX 갯수
	ST_PG_DATA stPgData[MAX_GRAB_STEP_COUNT + 1]; // 패턴 넘버에 맞게 배열에 저장됨 ex) R=1일때 R은 배열의 1번에 저장됨. 0은 없음.

	ST_PG_INFO()
	{
		memset(strChIndex, 0, sizeof(strChIndex));
		memset(stPgData, 0, sizeof(ST_PG_DATA)*MAX_GRAB_STEP_COUNT);
		nChIndexNum = 0;
	}

	void Init()
	{
		memset(strChIndex, 0, sizeof(strChIndex));
		nChIndexNum = 0;
		for (int i = 0; i < MAX_GRAB_STEP_COUNT; i++)
		{
			stPgData[i].bJudge = FALSE;
			stPgData[i].wVoltage[0] = 7500;
			stPgData[i].wVoltage[1] = 7500;
			stPgData[i].wVoltage[2] = 7500;
		}
	}
};

struct ST_LINE_INFO // 시퀀스가 PGController.ini와 MTP Result 파일을 읽어서 가지고 있는 구조체
{
	ST_LINE_DATA stLineData[MAX_GRAB_STEP_COUNT]; // 패턴 넘버에 맞게 배열에 저장됨 ex) R=1일때 R은 배열의 1번에 저장됨. 0은 없음.

	ST_LINE_INFO()
	{
		memset(stLineData, 0, sizeof(ST_LINE_DATA)*MAX_GRAB_STEP_COUNT);
	}

};

struct ST_REQ_GRAB_START
{
	TCHAR	strCellID[100];
	TCHAR	strVirId[100];
	ST_REQ_GRAB_START()
	{
		memset(strCellID, 0, sizeof(strCellID));
		memset(strVirId, 0, sizeof(strVirId));
	}
};

struct ST_STEP_INFO
{
	BOOL			bUseStep;
	TCHAR			strGrabStepName[50];
	BOOL			bUseVacuum;
	ST_CAMERA_COND  stCamCond[MAX_CAMERA_COUNT];
	STRU_LIGHT_INFO stLightInfo[MAX_LIGHT_PORT_COUNT];

	ST_STEP_INFO()
	{
		bUseStep = FALSE;
		memset(strGrabStepName, 0, sizeof(strGrabStepName));
		bUseVacuum = FALSE;
		memset(stCamCond, 0, sizeof(ST_CAMERA_COND) * MAX_CAMERA_COUNT);
		memset(stLightInfo, 0, sizeof(STRU_LIGHT_INFO) * MAX_LIGHT_PORT_COUNT);
	}
};

struct ST_MODEL_INFO
{
	UINT			nGrabCount;
	ST_STEP_INFO	stStepInfo[MAX_GRAB_STEP_COUNT];
	ST_PG_INFO      stPgInfo;
	//ST_CAM_COND_AOT	stLineInfo;

	ST_MODEL_INFO()
	{
		nGrabCount = 0;
		memset(stStepInfo, 0, sizeof(ST_STEP_INFO) * MAX_GRAB_STEP_COUNT);
	}
};

/////////////////////////////// AOT ///////////////////////////////
struct ST_CAM_COND_AOT
{
	TCHAR Type[50];
	TCHAR Name[50];
	BOOL Use;
	double Expose;
	double Gain;
	double PS;
	double Delay;
	double nCountF;
	double nCountB;
	double nStartF;
	double nStartB;
	double nStopF;
	double nStopB;
	double nPeriodF;
	double nPeriodB;
	ST_CAM_COND_AOT()
	{
		Use = false;
		Expose = 100;
		Gain = 1;
		PS = 0;
		Delay = 0;
		nCountF = 0;
		nCountB = 0;
		nStartF = 0;
		nStartB = 0;
		nStopF = 0;
		nStopB = 0;
		nPeriodF = 0;
		nPeriodB = 0;
	}
};

struct ST_LIGHT_COND_AOT
{
	BOOL Use;
	int Port_No;
	int Controller_No;
	STRU_SERIAL_INFO_AOT LightModule[MAX_LIGHT_COUNT];

	ST_LIGHT_COND_AOT()
	{
		Use = true;
		Port_No = 0;
		Controller_No = 0;
		memset(LightModule, 0, sizeof(STRU_SERIAL_INFO_AOT) * MAX_LIGHT_COUNT);
	}
};
struct ST_AUTOFOCUS_AOT
{
	TCHAR Name[30];
	BOOL Use;
	double AxisZ;
	ST_AUTOFOCUS_AOT()
	{
		Use = true;
		AxisZ = 0;
	}
};

struct ST_PATTERN_INFO_AOT
{
	TCHAR PatternName[50];
	BOOL Grab;
	BOOL Vacuum;	
	BOOL Insp;
	int CamCondCount;
	int LightCondCount;
	ST_CAM_COND_AOT Cam_Condition[MAX_CAMERA_COUNT];
	ST_LIGHT_COND_AOT Light_Condition[MAX_LIGHT_COUNT];
	ST_AUTOFOCUS_AOT AutoFocus_Condition[MAX_AUTOFOCUS_COUNT];
	ST_PATTERN_INFO_AOT()
	{
		Grab = false;
		Vacuum = false;	
		Insp = false;
		CamCondCount = 0;
		LightCondCount = 0;
		memset(Cam_Condition, 0, sizeof(ST_CAM_COND_AOT) * MAX_CAMERA_COUNT);
		memset(Light_Condition, 0, sizeof(ST_LIGHT_COND_AOT) * MAX_LIGHT_COUNT);
		memset(AutoFocus_Condition, 0, sizeof(ST_AUTOFOCUS_AOT) * MAX_AUTOFOCUS_COUNT);
	}
};
struct ST_GRAB_AREA_INFO_AOT
{
	TCHAR Name[50];
	int PtnCount;
	ST_PATTERN_INFO_AOT PatternList[MAX_PATTERN_COUNT];
	ST_GRAB_AREA_INFO_AOT()
	{
		PtnCount = 0;
		memset(PatternList, 0, sizeof(ST_PATTERN_INFO_AOT) * MAX_PATTERN_COUNT);
	}
};
struct ST_RECIPE_INFO_AOT
{
	TCHAR RecipeName[100];
	TCHAR RecipePath[100];
	int GrabCount;
	ST_GRAB_AREA_INFO_AOT GrabArea[MAX_AREA_COUNT];
	ST_RECIPE_INFO_AOT()
	{
		memset(GrabArea, 0, sizeof(ST_GRAB_AREA_INFO_AOT) * MAX_AREA_COUNT);
		GrabCount = 0;
	}
};
struct IMAGE_SET_AOT
{
	TCHAR PatternName[50];
	int SharedMemStartIdx;
	int SharedMemEndIdx;
	int ParticleImageCount;
	TCHAR Direction[50];
	int GrabLine;

	IMAGE_SET_AOT()
	{
		memset(PatternName, 0, sizeof(PatternName));
		SharedMemStartIdx = 0;
		SharedMemEndIdx = 0;
		ParticleImageCount = 0;
		memset(Direction, 0, sizeof(Direction));
		GrabLine = 0;

	}

};
struct PARAM_INSPECT_START_AOT_CHIPPING_ALM
{
	int PcNo;
	UINT nInspType;				// 0: Auto, 1: Manual Grab & Inspection, 2: Manual Inspection
	IMAGE_SET_AOT ImageSet[5];
	int PatternCount;
	wchar_t strPanelID[50];
	wchar_t strVirtualID[50];
	wchar_t strArea[50];
	int GrabLine;
	int CamNo;
	BOOL FirstPattern;



	PARAM_INSPECT_START_AOT_CHIPPING_ALM()
	{
		PcNo = 0;
		nInspType = 0;
		PatternCount = 0;
		memset(ImageSet, 0, sizeof(IMAGE_SET_AOT) * 5);
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strVirtualID, 0, sizeof(strVirtualID));
		memset(strArea, 0, sizeof(strArea));
		GrabLine = 0;
		CamNo = 0;
		FirstPattern = false;
	}
};
/////////////////////////////// AOT ///////////////////////////////
struct ST_PROC_INFO
{
	BOOL bFindModel;
	UINT nPcNum;
	TCHAR strModelID[50];
	TCHAR strPanelID[50];
	byte byteDummy[100];
};

struct PARAM_WAIT_GRAB_END
{
	BOOL bUseSMem;
	UINT nGrabNum;
	BOOL bUseFileSave;
	TCHAR strSavePath[200];
	TCHAR strPanelID[50];
	TCHAR strGrabStepName[50];
	int nSeqMode;
	int nTriCountF;
	int nTriCountB;
	int GrabCnt;

	PARAM_WAIT_GRAB_END()
	{
		bUseSMem = FALSE;
		nGrabNum = 0;
		bUseFileSave = FALSE;
		memset(strSavePath, 0, sizeof(strSavePath));
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strGrabStepName, 0, sizeof(strGrabStepName));
		nSeqMode = 0;
		nTriCountF = 0;
		nTriCountB = 0;
		GrabCnt = 0;
	}
};

struct PARAM_CLASSIFY_END
{
	// Algorithm
	int nPCNum;
	BOOL bIsManual;
	wchar_t strPanelID[50];
	wchar_t strVirtualID[50];
	wchar_t strModelID[50];
	UINT nDefectCount;
	// 패널 판정 적어주도록 변경
	//BOOL bIsOK;
	wchar_t strPanelJudge[2];
	// Sequence
	wchar_t strStartTime[50];
	wchar_t strTactTime[50];
	wchar_t strJudgeCode1[50];
	wchar_t strJudgeCode2[50];
	wchar_t strSavedDrive[10];
	int nStageNo;

	PARAM_CLASSIFY_END()
	{
		nPCNum = 0;
		bIsManual = FALSE;
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strVirtualID, 0, sizeof(strVirtualID));
		memset(strModelID, 0, sizeof(strModelID));
		nDefectCount = 0;
		//bIsOK = FALSE;
		memset(strPanelJudge, 0, sizeof(strPanelJudge));
		memset(strStartTime, 0, sizeof(strStartTime));
		memset(strTactTime, 0, sizeof(strTactTime));
		memset(strJudgeCode1, 0, sizeof(strJudgeCode1));
		memset(strJudgeCode2, 0, sizeof(strJudgeCode2));
		memset(strSavedDrive, 0, sizeof(strSavedDrive));
		nStageNo = 0;
	}
};

struct PARAM_INSPECT_START
{
	UINT nInspType;				// 0: Auto, 1: Manual Grab & Inspection, 2: Manual Inspection
	int	nStageNo;
	wchar_t strPanelID[50];
	wchar_t strVirtualID[50];
	wchar_t	strLotID[50];
	BOOL bUseCamera[MAX_CAMERA_COUNT];
	UINT nSeqMode[MAX_CAMERA_COUNT];	// PixelShift - 0 : Non / 1 : 4-Shot / 2 : 9-Shot
	UINT nImageNum;
	UINT nShareImgNum;
	wchar_t strImagePath[MAX_CAMERA_COUNT][200];
	int nRetryCnt;
	double dPatternCIE[3];	// MTP 보정 후 [0]:X, [1]:Y, [2]:L 검사 활용
	wchar_t strPos[50];
	int nGrabLine;

	PARAM_INSPECT_START()
	{
		nInspType = 0;
		nStageNo = 0;
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strVirtualID, 0, sizeof(strVirtualID));
		memset(strLotID, 0, sizeof(strLotID));
		for (int i = 0; i < MAX_CAMERA_COUNT; i++)
		{
			bUseCamera[i] = FALSE;
			nSeqMode[i] = 0;
			memset(strImagePath[i], 0, sizeof(strImagePath[i]));
		}
		memset(strPos, 0, sizeof(strPos));
		nGrabLine = 0;
		nImageNum = 0;
		nShareImgNum = 0;
		nRetryCnt = 0;
		for (int i = 0; i < 3; i++)
			dPatternCIE[i] = 0.0;
	}
};

struct PARAM_INSPECT_START_ACI
{
	int PcNo;
	UINT nInspType;				// 0: Auto, 1: Manual Grab & Inspection, 2: Manual Inspection
	wchar_t strPanelID[50];
	wchar_t strVirtualID[50];


	UINT nImageNum;
	UINT nShareImgNum;
	wchar_t strDirection[50];
	wchar_t strPosition[50];
	int nGrabLine;
	int ParticleCount;

	PARAM_INSPECT_START_ACI()
	{
		PcNo = 0;
		nInspType = 0;

		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strVirtualID, 0, sizeof(strVirtualID));

		nGrabLine = 0;
		nImageNum = 0;
		memset(strDirection, 0, sizeof(strDirection));
		memset(strPosition, 0, sizeof(strPosition));
		nShareImgNum = 0;
		ParticleCount = 0;


	}
};

struct PARAM_ALIGN_START
{
	int	nStageNo;
	wchar_t strPanelID[50];
	wchar_t strVirtualID[50];
	BOOL bUseCamera[MAX_CAMERA_COUNT];
	UINT nSeqMode[MAX_CAMERA_COUNT];	// PixelShift - 0 : Non / 1 : 4-Shot / 2 : 9-Shot
	UINT nImageNum;
	wchar_t strImagePath[MAX_CAMERA_COUNT][200];
	double dAdjTheta[MAX_CAMERA_COUNT];
	BOOL bIsManual;

	PARAM_ALIGN_START()
	{
		nStageNo = 0;
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strVirtualID, 0, sizeof(strVirtualID));
		bIsManual = FALSE;
		for (int i = 0; i < MAX_CAMERA_COUNT; i++)
		{
			bUseCamera[i] = FALSE;
			nSeqMode[i] = 0;
			memset(strImagePath[i], 0, sizeof(strImagePath[i]));
			dAdjTheta[i] = 0.0;
		}
		nImageNum = 0;
	}
};

struct PARAM_MOVE_MOTION
{
	wchar_t strMotor[20];
	int		nMovePoint;

	PARAM_MOVE_MOTION()
	{
		memset(strMotor, 0, sizeof(strMotor));
		nMovePoint = 0;
	}
};


namespace CS
{
	struct Rect
	{
		int left;
		int top;
		int right;
		int bottom;

		Rect()
		{

		}
		Rect(int nLeft, int nTop, int nRight, int nBottom)
		{
			left = nLeft;
			top = nTop;
			right = nRight;
			bottom = nBottom;
		}
	};
}
struct ST_CAMERA_INFO
{
	double		dResolution;
	CS::Rect		rcPad;					// Pad LT, RB
	wchar_t		strPadRefPath[500];
	wchar_t		strMuraRefPath[500];    // Au¿e ¿ⓒºI ¾ÆA÷ ¹IA¤

	ST_CAMERA_INFO()
	{
		dResolution = 0.0;
		rcPad = CS::Rect(0, 0, 0, 0);
		memset(strPadRefPath, 0, sizeof(strPadRefPath));
		memset(strMuraRefPath, 0, sizeof(strMuraRefPath));
	}
};

struct ST_RECIPE_INFO
{
	// Work Coordinate
	int		nWorkDirection;					// 0 : X = Width, 1 : Y = Width
	int		nWorkOriginPosition;			// 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int     nOriginOffsetX;					// Offset From Origin Position
	int     nOriginOffsetY;

	// Gate/Data Coordinate
	int		nDataDirection;             // 0 : Data = X, 1 : Data = Y
	int     nGateDataOriginPosition;    // 0 : LT, 1 : RT, 2 : RB, 3 : LB
	int     nGateDataOriginOffsetX;     // Offset From Origin Position
	int     nGateDataOriginOffsetY;
	double  dGatePitch;             // Gate Pitch (um)
	double  dDataPitch;             // Data Pitch (um)

	double	dPanelSizeX;
	double	dPanelSizeY;

	ST_CAMERA_INFO stCamInfo[MAX_CAMERA_COUNT];

	ST_RECIPE_INFO()
	{
		nWorkDirection = 0;
		nWorkOriginPosition = 0;
		nOriginOffsetX = 0;
		nOriginOffsetY = 0;
		nDataDirection = 0;
		nGateDataOriginPosition = 0;
		nGateDataOriginOffsetX = 0;
		nGateDataOriginOffsetY = 0;
		dGatePitch = 0.0;
		dDataPitch = 0.0;

		dPanelSizeX = 0.0;
		dPanelSizeY = 0.0;
	}
};

// Algorithm Common Parameter (User Setting 필요한 부분)
struct ST_COMMON_PARA
{
	BOOL	bIFImageSaveFlag;	/// 알고리즘 중간결과 Image Save flag (Interim Findings)
	BOOL	bDrawDefectNum;		/// Draw Defect Num 표시
	BOOL	bDrawDust;			/// Draw Dust 표시 ( Point 알고리즘이 돌아야 함 )
};

struct TACT_TIME_DATA
{
	TCHAR strPanelID[50];
	TCHAR strTactName[50];
	TCHAR strTactState[50];
	TACT_TIME_DATA()
	{
		memset(strPanelID, 0X00, sizeof(TCHAR) * 50);
		memset(strTactName, 0X00, sizeof(TCHAR) * 50);
		memset(strTactState, 0X00, sizeof(TCHAR) * 50);
	}
};

//LJH 레시피 버Version
struct RCP_VERSION
{
	wchar_t		Ver[100];				//설비
// 	wchar_t		EQP[10];				//설비
// 	wchar_t		ModelType[10];			//설비
// 	wchar_t		updateDate[10];			//파라미터 값 수정한 시간(MMDDHH)
// 	wchar_t		changeParamListDate[10];	//파라미터의 항목이 추가 / 삭제 / 수정 된 시간(MMDDHH)
// 	wchar_t		testAlgType[10];		//테스트 알고리즘 항목

	RCP_VERSION()
	{
		memset(Ver, 0, sizeof(Ver));
		// 		memset(EQP					, 0, sizeof(EQP));
		// 		memset(ModelType			, 0, sizeof(ModelType));
		// 		memset(updateDate			, 0, sizeof(updateDate));
		// 		memset(changeParamListDate	, 0, sizeof(changeParamListDate));
		// 		memset(testAlgType			, 0, sizeof(testAlgType));

	}
};

#pragma pack(pop)

#endif
