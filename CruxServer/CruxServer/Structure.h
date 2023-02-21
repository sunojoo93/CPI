#if !defined(AFX_STRUCTURE_H_INCLUDED_)
#define AFX_STRUCTURE_H_INCLUDED_

#pragma pack(1)

struct STRU_COMM_INFO
{
	CString strIP_VisionServer;
	CString strIP_Vision_1;
	CString strIP_Vision_2;
	CString strIP_Vision_3;
	//2015.05.12 add s-
	CString strIP_Vision_4;
	CString strIP_Vision_5;
	CString strIP_Vision_6;
	CString strIP_Vision_7;
	CString strIP_Vision_8;
	CString strIP_Vision_9;
	//2015.05.12 add e-
	CString strIP_SignalFlag;
	CString strIP_CIM;
	CString strIP_PLC;
	CString strIP_ControlPC;
	CString strIP_PreAlignPC;	//2015.08.19

	int nPort_VisionServer;
	int nPort_Vision_1;
	int nPort_Vision_2;
	int nPort_Vision_3;
	//2015.05.12 add s-
	int nPort_Vision_4;
	int nPort_Vision_5;
	int nPort_Vision_6;
	int nPort_Vision_7;
	int nPort_Vision_8;
	int nPort_Vision_9;
	//2015.05.12 add e-
	int nPort_SignalFlag;
	int nPort_CIM;
	int nPort_PLC;
	int nPort_ControlPC;
	int nPort_PreAlignPC;	//2015.08.19

	STRU_COMM_INFO()
	{
		nPort_VisionServer = 0;
		nPort_Vision_1 = 0;
		nPort_Vision_2 = 0;
		nPort_Vision_3 = 0;
		//2015.05.12 add s-
		nPort_Vision_4 = 0;
		nPort_Vision_5 = 0;
		nPort_Vision_6 = 0;
		nPort_Vision_7 = 0;
		nPort_Vision_8 = 0;
		nPort_Vision_9 = 0;
		//2015.05.12 add e-
		nPort_CIM = 0;
		nPort_PLC = 0;
		nPort_ControlPC = 0;
		nPort_SignalFlag = 0;
		nPort_PreAlignPC = 0;	//2015.08.19
	}
};

struct STRU_PARAM_INFO
{
	BOOL bUseAutoDel;			// Log 자동 삭제 기능 사용
	int nLogDelDay;				// Log 파일 유지 기간
	int nSummaryBackupDay;		// Summary 파일 백업 주기
	int nCountVisionClient;		// Vision PC 갯수
	int nCountStage;			// Stage 갯수	//2015.05.13
	int nControlMode;			// 제어 모드 - 0 : PLC, 1 : PC
	BOOL bControlSockMode;		// 제어 모드 PC 일때 Socket 모드 - 0 : Passive, 1 : Active
	BOOL bUseCIM;				// CIM 사용 - 0 : 사용 않함, 1 : 사용함
	BOOL bUseCIM2;				// QMS 사용 - 0 : 사용 않함, 1 : 사용함
	BOOL bCIMSockMode;			// CIM 사용 할때 Socket 모드 - 0 : Passive, 1 : Active
	BOOL bUseSignalFlag;
	BOOL bSignalFlagSockMode;	// SignalFlag 사용 할때 Socket 모드 - 0 : Passive, 1 : Active
	int nEqpType;				// Equipment Type - 0 : ACC, 1 : ACS
	int nEqpObject;
	int nEqpSize;				//
	BOOL bUseManualJudge;	//2018.05.10
	BOOL bUseBubbleAreaAlarm;	//2020.02.28 add by mby
	BOOL bUseInspPc[9];			//2020.07.22 add by mby
	BOOL bUseAlignPc;			//2020.10.15 add by mby
	STRU_PARAM_INFO()
	{
		nLogDelDay = 0;
		nCountVisionClient = 0;
		nCountStage = 0;	//2015.05.13 add
		nControlMode = 0;
		bControlSockMode = FALSE;
		bUseCIM = FALSE;
		bUseCIM2 = FALSE;
		bCIMSockMode = FALSE;
		nEqpType = 0;
		nEqpObject = 0;
		bUseSignalFlag= FALSE;
		bUseManualJudge = FALSE;	//2018.05.10
		bUseBubbleAreaAlarm = FALSE;	//2020.02.28 add by mby
		for(int i=0; i<9; i++){
			bUseInspPc[i] = FALSE;
		}
	}
};

struct STRU_IPC_STATUS
{
	BOOL bVisionServer;
	BOOL bVisionClient[MAX_VISION_PC];
	BOOL bPLC;
	BOOL bControlPC;
	BOOL bCIM;
	BOOL bSignalFlag;
	BOOL bPreAlign;
	STRU_IPC_STATUS()
	{
		bVisionServer = FALSE;
		memset((void*)&bVisionClient, NULL, sizeof(BOOL) * MAX_VISION_PC);
		bPLC = FALSE;
		bControlPC = FALSE;
		bCIM = FALSE;
		bSignalFlag = FALSE;
		bPreAlign = FALSE;
	}
};

struct STRU_SOCK_STATUS
{
	BOOL bVisionServer;
	BOOL bVisionClient[MAX_VISION_PC];
	BOOL bPLC;
	BOOL bControlPC;
	BOOL bCIM;
	BOOL bSignalFlag;
	BOOL bPreAlign;
	STRU_SOCK_STATUS()
	{
		bVisionServer = FALSE;
		memset((void*)&bVisionClient, NULL, sizeof(BOOL) * MAX_VISION_PC);
		bPLC = FALSE;
		bControlPC = FALSE;
		bCIM = FALSE;
		bSignalFlag = FALSE;
		bPreAlign = FALSE;
	}
};

struct STRU_PANEL_INFO
{
	char chPanelID[LEN_PANEL_ID];
	char chModel[LEN_MODEL_ID];
	int nPanelSizeX;
	int nPanelSizeY;

	STRU_PANEL_INFO()
	{
		memset(chPanelID, NULL, LEN_PANEL_ID);
		memset(chModel, NULL, LEN_MODEL_ID);
		nPanelSizeX = 0;
		nPanelSizeY = 0;
	}
};

//2015.02.03 add by ikm for FTP - S

struct STRU_FTP_SEVER_INFO
{
	int iDirDelDay;		//2016.03.30 Mr.Chu
	CString strServer;
	CString strID;
	CString strPW;	

	CString strImgID;
	CString strImgPW;	
	CString strRoot;		
	int nFTP_Port;	

	void init()
	{
		strServer = _T("");
		strID = _T("");
		strPW = _T("");
		strImgID= _T("");
		strImgPW= _T("");
		strRoot = _T("");	
		nFTP_Port = 0;		
	}

	~STRU_FTP_SEVER_INFO()
	{		
		strServer = _T("");
		strID = _T("");
		strPW = _T("");
		strImgID= _T("");
		strImgPW= _T("");
		strRoot = _T("");	
		nFTP_Port = 0;		
	}
};

struct STRU_FTP_PATH
{
	CString strDir_Send;
	CString strDir_Wait;
	CString strDir_Error;
	CString strDir_Log;
	CString strDir_iLog;
	CString strDir_Default;
	CString strDir_iSend;
	CString strDir_iWait;
	//2018.05.14 temp
	CString strDir_Temp;
	CString strDir_iTemp;

	void init()
	{
		strDir_Send = _T("");
		strDir_Wait = _T("");
		strDir_Error = _T("");
		strDir_Log = _T("");
		strDir_iLog = _T("");
		strDir_Default = _T("");
		strDir_iSend = _T("");
		strDir_iWait = _T("");
		//2018.05.14 temp
		strDir_Temp = _T("");
		strDir_iTemp = _T("");
	}

	~STRU_FTP_PATH()
	{
		strDir_Send = _T("");
		strDir_Wait = _T("");
		strDir_Error = _T("");
		strDir_Log = _T("");
		strDir_iLog = _T("");
		strDir_Default = _T("");
		strDir_iSend = _T("");
		strDir_iWait = _T("");
		//2018.05.14 temp
		strDir_Temp = _T("");
		strDir_iTemp = _T("");
	}
};

struct STRU_FTP_CONDITION
{
	int iIP_Cnt;
	int iAns_Rename_Wait_cnt;
	int iAns_Rename_Chk_Timeout;

	void init()
	{
		iIP_Cnt = 0;
		iAns_Rename_Wait_cnt = 0;
		iAns_Rename_Chk_Timeout = 0;
	}

	~STRU_FTP_CONDITION()
	{
		iIP_Cnt = 0;
		iAns_Rename_Wait_cnt = 0;
		iAns_Rename_Chk_Timeout = 0;
	}
};

struct STRU_FTP_INFO
{
	STRU_FTP_SEVER_INFO *stInfo;
	STRU_FTP_PATH *stPath;	
};

//2015.02.03 add by ikm for FTP - E

//2015.05.13 add s-
struct STRU_PLC_STAGEINFO
{
	int nStageNo;
	int nVisionCnt;
	int nVisionNo[MAX_VISION_PC];

	STRU_PLC_STAGEINFO()
	{
		nStageNo = 0;
		nVisionCnt = 0;
	}

};
//2015.05.13 add e-

//2015.06.02 add s-
struct STRU_OVERLAP_COORD_INFO
{
	BOOL bAllOverlapDef; //2016.03.23 add by ikm
	BOOL bUseMode;
	BOOL bUseContactFail;
	BOOL bUsePolMiss;
	BOOL bUseAlignFail;
	DOUBLE dOffsetX;
	DOUBLE dOffsetY;
	BOOL bArrLuminanceChk[LUMINANCE_TYPE_MAX];
	int	nArrLuminanceCnt[LUMINANCE_TYPE_MAX];
	BOOL bArrBubbleStationChk[BUBBLESTATION_MAX];   //2020.02.28 add by mby 
	int	nArrBubbleStationCnt[BUBBLESTATION_MAX];	//2020.02.28 add by mby
	int nDefectCheck[MAX_DEFECT_KIND];
	int nDefectCount[MAX_DEFECT_KIND];
	int nContactFailDefectCount;
	int nPolMissDefectCount;
	int nPolMissStopCount;
	int nAlignFaliDefectCount;
	BOOL bUseBgradecnt;		  // 2019.03.21 -by ryu
	BOOL bUseCgradecnt;		  //           |
	int nBgradecntDefectCount; //		   |
	int nBgradecntStopCount;
	int nCgradecntDefectCount; // 2019.03.21 -by ryu
	int nCgradecntStopCount;
	int nPanelCnt;
	STRU_OVERLAP_COORD_INFO()
	{
		bAllOverlapDef = FALSE; //2016.03.23 add by ikm
		bUseMode = FALSE;
		bUseContactFail = FALSE;
		bUsePolMiss = FALSE;
		bUseAlignFail = FALSE;
		dOffsetX = 0;
		dOffsetY = 0;
		nContactFailDefectCount = 0;
		nPolMissDefectCount = 0;
		nPolMissStopCount = 0;
		nAlignFaliDefectCount = 0;
		nBgradecntDefectCount =0;
		nCgradecntDefectCount =0;
		nBgradecntStopCount =0;
		nCgradecntStopCount =0;
		nPanelCnt = 0;
		//memset(&nPanelCnt, 0 ,sizeof(nPanelCnt));
	}

};

struct STRU_OVERLAP_DEFTYPE_PARAM 
{
	STRU_OVERLAP_COORD_INFO PARAM;
	DOUBLE dHour[MAX_DEFECT_KIND];
	DOUBLE dContactHour;
	DOUBLE dAlignFaliHour;
	DOUBLE dPolMissHour;
	int	nAlarmCount[MAX_DEFECT_KIND];
	DOUBLE	dLumiHour[LUMINANCE_TYPE_MAX];
	int		nLumiAlarmCount[LUMINANCE_TYPE_MAX];

	DOUBLE	dGradecountHour[GRADECHECK_MAX];	    // 2019.03.20 by ryu
	int		nGradecountCount[GRADECHECK_MAX];		// 2019.03.20 by ryu
	DOUBLE	dBubbleStationHour[BUBBLESTATION_MAX];   //2020.02.28 add by mby
	int		nBubbleStationAlarmCount[BUBBLESTATION_MAX];	//2020.02.28 add by mby
	void init()
	{
		for(int i = 0; i < MAX_DEFECT_KIND; i++)
			dHour[i] = 0;			
		dContactHour	= 0;
		dAlignFaliHour	= 0;
		dPolMissHour	= 0;

		for(int i=0; i< LUMINANCE_TYPE_MAX; i++)
		{
			dLumiHour[i]		= 0;
			nLumiAlarmCount[i]	= 0;
		}

		for(int i=0; i< GRADECHECK_MAX; i++)    // 2019.03.20 by ryu
		{ 
			dGradecountHour[i]		= 0;
			nGradecountCount[i]	= 0;
		}
		for(int i=0; i< BUBBLESTATION_MAX; i++)   //2020.02.28 add by mby
		{
			dBubbleStationHour[i]		= 0;
			nBubbleStationAlarmCount[i]	= 0;
		}
	}
};

//2016.12.19 Mr.Chu
// Defect 좌표를 관리하는 구조체
struct STRU_DEFECT_COORD_RECIPE_DATA
{
	BOOL	bReportQMSCoordDat;
	BOOL	bMajorAxisType;
	int		nOriginType;
	double	dSetPanelSizeHeight;
	double	dSetPanelSizeWidth;
	int		nAlignPCNo;			//20201.07.07 add by mby

	void init_info()
	{
		bReportQMSCoordDat	= FALSE;
		bMajorAxisType		= FALSE;
		nOriginType			= RECIPE_DEF_MAP_ORIGIN_LT;
		nAlignPCNo			= 0;	//2021.07.07 add by mby
	}
};

struct STRU_DEFECT_COORD_DATA
{
	int nCount;
	int nPanelCnt;
	DOUBLE startX;
	DOUBLE startY;
	DOUBLE endX;
	DOUBLE endY;
	int nDefectType;
	int nFlag;
	BOOL bUpdateFlag;
	TCHAR strImage[LEN_IMAGE_PATH];

	STRU_DEFECT_COORD_DATA()
	{
		nCount = 0;
		nPanelCnt = 0;
		startX = 0;
		startY = 0;
		endX = 0;
		endY = 0;
		nDefectType = 0;
		nFlag = 0;
		bUpdateFlag = FALSE;
		memset(strImage, 0X00,sizeof(strImage));
	}
	void init_info()
	{
		nCount = 0;
		startX = 0;
		startY = 0;
		endX = 0;
		endY = 0;
		nDefectType = 0;
		nFlag = 0;
		bUpdateFlag = FALSE;
		memset(strImage, 0X00,sizeof(strImage));
	};
};

struct STRU_DEFECT_COORD_INFO
{
	STRU_DEFECT_COORD_DATA stPnlInfo[MAX_DEFECT_CNT];

};
//2015.06.02 add e-

//2015.06.11 s-
struct STRU_TDI_OVERLAP_COORD_INFO
{
	DOUBLE dOffsetX;
	DOUBLE dOffsetY;

	STRU_TDI_OVERLAP_COORD_INFO()
	{
		dOffsetX = 0;
		dOffsetY = 0;
	}

};
struct STRU_DEFECT_TYPE_UI
{
	// Edge Broken
	int CfBrokenA;
	int CfBrokenB;
	int CfBrokenC;
	int CfBrokenD;
	int TftBrokenA;
	int TftBrokenB;
	int TftBrokenC;
	int TftBrokenD;
	// Corner Broken
	int CfCornerBrokenA;
	int CfCornerBrokenB;
	int CfCornerBrokenC;
	int CfCornerBrokenD;
	int TftCornerBrokenA;
	int TftCornerBrokenB;
	int TftCornerBrokenC;
	int TftCornerBrokenD;
	void initDefType()
	{
		CfBrokenA = 0;
		CfBrokenB = 0;
		CfBrokenC = 0;
		CfBrokenD = 0;
		TftBrokenA = 0;
		TftBrokenB = 0;
		TftBrokenC = 0;
		TftBrokenD = 0;
		CfCornerBrokenA = 0;
		CfCornerBrokenB = 0;
		CfCornerBrokenC = 0;
		CfCornerBrokenD = 0;
		TftCornerBrokenA = 0;
		TftCornerBrokenB = 0;
		TftCornerBrokenC = 0;
		TftCornerBrokenD = 0;
	}
};
//2015.06.11 e-
struct STRU_DEFECT_TYPE
{
	CString strRoot, strValue, strItem;

	int PanelCodeTest;			// 고객사 요청으로 인한 Test Code		//2016.09.14	Mr.Chu

	int DustType;
	int CfScrach;			    // CF 깨짐
	int TftScrach;			
	int CFWhiteScratch;			//	CF 기판 흰색 기스
	int CFBlackScratch;			//	CF 기판 검은색 기스
	int ItoPeel;				//	ITO 벗겨짐		2
	//int PimplePoint;			//	PIMPLE 점형
	int CfDimplePoint;			//	DIMPLE 점형		5
	int TftDimplePoint;
	//int PimpleLine;				//	PIMPLE 선형
	int CfDimpleLine;				//	DIMPLE 선형		7
	int TftDimpleLine;
	int CfEdgeilegal;			//	CF 엣칭 불량	6
	int TftEdgeilegal;			
	int CfWhiteBlur;			//	CF 백탁        1
	int TftWhiteBlur;
	int SocketBurn;				//	단자 탐			8
	int WanesBroken;			//	이지러짐 / 깨짐	3
	int SocketScrach;
	int CfMeasureFail;
	int TftMeasureFail;
	int Burr;
	int CfPolBubble;
	int TftPolBubble;
	int CfPolScrach;
	int TftPolScrach;
	int mura;
	int CFpolSubstance;			// TFT,CF 이물 구분 Add by KYH01
	int TFTpolSubstance;		// TFT,CF 이물 구분 Add by KYH01
	int BlackScrach;
	int WhiteScrach;
	int PolMinor;
	int CfBurr;	//2015.07.13
	int TftBurr;
	int CfBroken;
	int TftBroken;
	int CfCornerBroken;
	int TftCornerBroken;
	//2018.05.15 -s
	int CfBurrA;
	int CfBurrB;
	int CfBurrC;
	int CfBurrD;
	int TftBurrA;
	int TftBurrB;
	int TftBurrC;
	int TftBurrD;
	// Edge Broken
	int CfBrokenA;
	int CfBrokenB;
	int CfBrokenC;
	int CfBrokenD;
	int TftBrokenA;
	int TftBrokenB;
	int TftBrokenC;
	int TftBrokenD;
	// Corner Broken
	int CfCornerBrokenA;
	int CfCornerBrokenB;
	int CfCornerBrokenC;
	int CfCornerBrokenD;
	int TftCornerBrokenA;
	int TftCornerBrokenB;
	int TftCornerBrokenC;
	int TftCornerBrokenD;
	//2018.05.15 -e
	// Active broken
	int CfCellBroken;	//2015,08,26
	int TftCellBroken;
	int Contact;
	//2016.01.25 add by ikm - 물류 코드 추가 - s
	int CfPimplePoint;
	int TftPimplePoint;
	int CfPimpleLine;
	int TftPimpleLine;
	int CfPimpleCircle;
	int CfWavePattern;
	int EtcLine;
	int EtcPoint;
	//2016.01.25 add by ikm - 물류 코드 추가 - e
	int PolMiss;	//2016.02.24
	int TotalCnt;
	int CfNgCnt;
	int TftNgCnt;
	int PenetNgCnt;
	int AlignFail;	//2016.05.11 Mr.Chu		Insp Align Fail Def cord 추가
	//2016.11.19		Mr.Chu				Luminance
	int CFCLuminance;
	int CBCLuminance;
	int CFSLuminance;
	int CBSLuminance;
	int TFCLuminance;
	int TBCLuminance;
	int TFSLuminance;
	int TBSLuminance;
	int TDILuminance;
	//2017.04.12
	int CfProtection;	
	int TftProtection;
	//2019.03.15, 우선순위에 따른 다음항목 불필요.
	int Bgradecheck;
	int Cgradecheck;
	//2020.02.28 add by mby
	int BubbleStation1;
	int BubbleStation2;
	int BubbleStation3;
	int BubbleStation4;
	int BubbleStation5;
	int BubbleStation6;
	int BubbleStation7;
	int BubbleStation8;
	int BubbleStation9;
	int BubbleStation10;
	int BubbleStation11;
	int BubbleStation12;

	void TotalDefType()
	{
// 		TotalCnt = DustType + CfScrach + TftScrach + CFWhiteScratch + CFBlackScratch + ItoPeel  
// 			+ CfDimplePoint + TftDimplePoint  + CfDimpleLine + TftDimpleLine + CfEdgeilegal
// 			+ TftEdgeilegal + CfWhiteBlur + TftWhiteBlur + SocketBurn +	WanesBroken
// 			+ SocketScrach + CfMeasureFail + TftMeasureFail + Burr + CfPolBubble
// 		    + TftPolBubble + CfPolScrach + TftPolScrach + mura + CFpolSubstance + TFTpolSubstance
// 			+ BlackScrach + WhiteScrach + PolMinor + Contact
// 			+ CfBurr + TftBurr + CfBroken + TftBroken + CfCornerBroken + TftCornerBroken	//2015.07.13
// 			+ CfCellBroken + TftCellBroken //2015.08.26
// 			+ CfPimplePoint + TftPimplePoint + CfPimpleLine + TftPimpleLine
// 			+ CfPimpleCircle + CfWavePattern + EtcLine + EtcPoint + PolMiss	//2016.01.25 add by ikm - 물류 코드 추가
// 			+ AlignFail + CfProtection + TftProtection
// 			;
		TotalCnt = CfScrach + TftScrach + SocketBurn + SocketScrach
			+ CfMeasureFail + TftMeasureFail
			+ CfBurr + TftBurr + CfBroken + TftBroken + CfCornerBroken + TftCornerBroken	//2015.07.13
			+ CfCellBroken + TftCellBroken //2015.08.26
			+ EtcLine + EtcPoint	//2016.01.25 add by ikm - 물류 코드 추가
			+ AlignFail +Bgradecheck+Cgradecheck   //2019.03.26 by ryu
			+BubbleStation1+BubbleStation2+BubbleStation3+BubbleStation4  //2020.02.28 add by mby
			+BubbleStation5+BubbleStation6+BubbleStation7+BubbleStation8
			+BubbleStation9+BubbleStation10+BubbleStation11+BubbleStation12
 			;

	}
	void AddDefCnt(STRU_DEFECT_TYPE DefInfoTmp)
	{
		DustType			+= DefInfoTmp.DustType;
		CfScrach			+= DefInfoTmp.CfScrach;
		TftScrach			+= DefInfoTmp.TftScrach;
		CFWhiteScratch		+= DefInfoTmp.CFWhiteScratch;
		CFBlackScratch		+= DefInfoTmp.CFBlackScratch;
		ItoPeel				+= DefInfoTmp.ItoPeel; 
		CfDimplePoint		+= DefInfoTmp.CfDimplePoint; 
		TftDimplePoint		+= DefInfoTmp.TftDimplePoint; 
		CfDimpleLine		+= DefInfoTmp.CfDimpleLine; 
		TftDimpleLine		+= DefInfoTmp.TftDimpleLine; 
		CfEdgeilegal		+= DefInfoTmp.CfEdgeilegal; 
		TftEdgeilegal		+= DefInfoTmp.TftEdgeilegal; 
		CfWhiteBlur			+= DefInfoTmp.CfWhiteBlur; 
		TftWhiteBlur		+= DefInfoTmp.TftWhiteBlur; 
		SocketBurn			+= DefInfoTmp.SocketBurn;	
		WanesBroken			+= DefInfoTmp.WanesBroken; 
		SocketScrach		+= DefInfoTmp.SocketScrach; 
		CfMeasureFail		+= DefInfoTmp.CfMeasureFail; 
		TftMeasureFail		+= DefInfoTmp.TftMeasureFail; 
		Burr				+= DefInfoTmp.Burr ;
		CfPolBubble			+= DefInfoTmp.CfPolBubble; 
		TftPolBubble		+= DefInfoTmp.TftPolBubble; 
		CfPolScrach			+= DefInfoTmp.CfPolScrach; 
		TftPolScrach		+= DefInfoTmp.TftPolScrach; 
		mura				+= DefInfoTmp.mura ;
		CFpolSubstance		+= DefInfoTmp.CFpolSubstance;
		TFTpolSubstance		+= DefInfoTmp.TFTpolSubstance;
		BlackScrach			+= DefInfoTmp.BlackScrach ;
		WhiteScrach			+= DefInfoTmp.WhiteScrach ;
		PolMinor			+= DefInfoTmp.PolMinor;
		CfBurr 				+= DefInfoTmp.CfBurr;	//2015.07.13
		TftBurr 			+= DefInfoTmp.TftBurr;
		CfBroken 			+= DefInfoTmp.CfBroken;
		TftBroken 			+= DefInfoTmp.TftBroken;
		CfCornerBroken 		+= DefInfoTmp.CfCornerBroken;
		TftCornerBroken		+= DefInfoTmp.TftCornerBroken;
		// 2018.05.31 add by yhs -s
		CfBurrA				+= DefInfoTmp.CfBurrA;
		CfBurrB				+= DefInfoTmp.CfBurrB;
		CfBurrC				+= DefInfoTmp.CfBurrC;
		CfBurrD				+= DefInfoTmp.CfBurrD;
		TftBurrA			+= DefInfoTmp.TftBurrA;
		TftBurrB			+= DefInfoTmp.TftBurrB;
		TftBurrC			+= DefInfoTmp.TftBurrC;
		TftBurrD			+= DefInfoTmp.TftBurrD;
		CfBrokenA			+= DefInfoTmp.CfBrokenA;
		CfBrokenB			+= DefInfoTmp.CfBrokenB;
		CfBrokenC			+= DefInfoTmp.CfBrokenC;
		CfBrokenD			+= DefInfoTmp.CfBrokenD;
		TftBrokenA			+= DefInfoTmp.TftBrokenA;
		TftBrokenB			+= DefInfoTmp.TftBrokenB;
		TftBrokenC			+= DefInfoTmp.TftBrokenC;
		TftBrokenD			+= DefInfoTmp.TftBrokenD;
		CfCornerBrokenA		+= DefInfoTmp.CfCornerBrokenA;
		CfCornerBrokenB		+= DefInfoTmp.CfCornerBrokenB;
		CfCornerBrokenC		+= DefInfoTmp.CfCornerBrokenC;
		CfCornerBrokenD		+= DefInfoTmp.CfCornerBrokenD;
		TftCornerBrokenA	+= DefInfoTmp.TftCornerBrokenA;
		TftCornerBrokenB	+= DefInfoTmp.TftCornerBrokenB;
		TftCornerBrokenC	+= DefInfoTmp.TftCornerBrokenC;
		TftCornerBrokenD	+= DefInfoTmp.TftCornerBrokenD;
		// 2018.05.31 add by yhs -s
		CfCellBroken 		+= DefInfoTmp.CfCellBroken;	//2015.08.26
		TftCellBroken		+= DefInfoTmp.TftCellBroken;
		Contact				*= DefInfoTmp.Contact;

		CfPimplePoint		+= DefInfoTmp.CfPimplePoint; //2016.01.25 add by ikm - 물류 코드 추가
		TftPimplePoint		+= DefInfoTmp.TftPimplePoint;
		CfPimpleLine		+= DefInfoTmp.CfPimpleLine;
		TftPimpleLine		+= DefInfoTmp.TftPimpleLine;
		CfPimpleCircle		+= DefInfoTmp.CfPimpleCircle;
		CfWavePattern		+= DefInfoTmp.CfWavePattern;
		EtcLine				+= DefInfoTmp.EtcLine;
		EtcPoint			+= DefInfoTmp.EtcPoint;

		PolMiss				+= DefInfoTmp.PolMiss;	//2016.02.24

		CfNgCnt				+= DefInfoTmp.CfNgCnt ;
		TftNgCnt			+= DefInfoTmp.TftNgCnt ;
		PenetNgCnt			+= DefInfoTmp.PenetNgCnt;
		AlignFail			+= DefInfoTmp.AlignFail;	//2016.05.11
		CfProtection		+= DefInfoTmp.CfProtection;
		TftProtection		+= DefInfoTmp.TftProtection;
		// 2019.03.15 by ryu
		Bgradecheck		+= DefInfoTmp.Bgradecheck;
		Cgradecheck		+= DefInfoTmp.Cgradecheck;
		//2020.02.28 add by mby
		BubbleStation1   += DefInfoTmp.BubbleStation1;
		BubbleStation2   += DefInfoTmp.BubbleStation2;
		BubbleStation3   += DefInfoTmp.BubbleStation3;
		BubbleStation4   += DefInfoTmp.BubbleStation4;
		BubbleStation5   += DefInfoTmp.BubbleStation5;
		BubbleStation6   += DefInfoTmp.BubbleStation6;
		BubbleStation7   += DefInfoTmp.BubbleStation7;
		BubbleStation8   += DefInfoTmp.BubbleStation8;
		BubbleStation9   += DefInfoTmp.BubbleStation9;
		BubbleStation10   += DefInfoTmp.BubbleStation10;
		BubbleStation11   += DefInfoTmp.BubbleStation11;
		BubbleStation12   += DefInfoTmp.BubbleStation12;
	}
	void setInfoPath(CString cItem, CString cInfoPath)
	{
		strItem = cItem;
		strRoot = cInfoPath;
	}
	void GetDefInfo(){

		//strRoot.Format(_T("%s%s"), ROOT_PATH,CONFIG_COUNT_PATH );
		PanelCodeTest		= GetPrivateProfileInt(strItem, _T("PanelCodeTest"), 0,strRoot);
		
		DustType =		 GetPrivateProfileInt(strItem, _T("DustType"), 0, strRoot);
		CfScrach =		 GetPrivateProfileInt(strItem, _T("CfScrach"), 0, strRoot);
		TftScrach =		 GetPrivateProfileInt(strItem, _T("TftScrach"), 0, strRoot);
		CFWhiteScratch = GetPrivateProfileInt(strItem, _T("CFWhiteScratch"), 0, strRoot);
		CFBlackScratch = GetPrivateProfileInt(strItem, _T("CFBlackScratch"), 0, strRoot);
		ItoPeel =		 GetPrivateProfileInt(strItem, _T("ItoPeel"), 0, strRoot);
		CfDimplePoint =  GetPrivateProfileInt(strItem, _T("CfDimplePoint"), 0, strRoot);
		TftDimplePoint = GetPrivateProfileInt(strItem, _T("TftDimplePoint"), 0, strRoot);
		CfDimpleLine =   GetPrivateProfileInt(strItem, _T("CfDimpleLine"), 0, strRoot);
		TftDimpleLine =  GetPrivateProfileInt(strItem, _T("TftDimpleLine"), 0, strRoot);
		CfEdgeilegal =   GetPrivateProfileInt(strItem, _T("CfEdgeilegal"), 0, strRoot);
		TftEdgeilegal =  GetPrivateProfileInt(strItem, _T("TftEdgeilegal"), 0, strRoot);
		CfWhiteBlur =    GetPrivateProfileInt(strItem, _T("CfWhiteBlur"), 0, strRoot);
		TftWhiteBlur =	 GetPrivateProfileInt(strItem, _T("TftWhiteBlur"), 0, strRoot);
		SocketBurn =	 GetPrivateProfileInt(strItem, _T("SocketBurn"), 0, strRoot);
		WanesBroken =	 GetPrivateProfileInt(strItem, _T("WanesBroken"), 0, strRoot);
		SocketScrach =	 GetPrivateProfileInt(strItem, _T("SocketScrach"), 0, strRoot);
		CfMeasureFail =  GetPrivateProfileInt(strItem, _T("CfMeasureFail"), 0, strRoot);
		TftMeasureFail = GetPrivateProfileInt(strItem, _T("TftMeasureFail"), 0, strRoot);
		Burr =			 GetPrivateProfileInt(strItem, _T("Burr"), 0, strRoot);
		CfPolBubble =	 GetPrivateProfileInt(strItem, _T("CfPolBubble"), 0, strRoot);
		TftPolBubble =	 GetPrivateProfileInt(strItem, _T("TftPolBubble"), 0, strRoot);
		CfPolScrach =	 GetPrivateProfileInt(strItem, _T("CfPolScrach"), 0, strRoot);
		TftPolScrach =	 GetPrivateProfileInt(strItem, _T("TftPolScrach"), 0, strRoot);
		mura =			 GetPrivateProfileInt(strItem, _T("mura"), 0, strRoot);
		CFpolSubstance = GetPrivateProfileInt(strItem, _T("CFpolSubstance"), 0, strRoot);
		TFTpolSubstance =GetPrivateProfileInt(strItem, _T("TFTpolSubstance"), 0, strRoot);				
		BlackScrach =	 GetPrivateProfileInt(strItem, _T("BlackScrach"), 0, strRoot);
		WhiteScrach =	 GetPrivateProfileInt(strItem, _T("WhiteScrach"), 0, strRoot);
		PolMinor =		 GetPrivateProfileInt(strItem, _T("PolMinor"), 0, strRoot);
		CfBurr =		 GetPrivateProfileInt(strItem, _T("CfBurr"), 0, strRoot);	//2015.07.13
		TftBurr =		 GetPrivateProfileInt(strItem, _T("TftBurr"), 0, strRoot);
		CfBroken =		 GetPrivateProfileInt(strItem, _T("CfBroken"), 0, strRoot);
		TftBroken =		 GetPrivateProfileInt(strItem, _T("TftBroken"), 0, strRoot);
		CfCornerBroken = GetPrivateProfileInt(strItem, _T("CfCornerBroken"), 0, strRoot);
		TftCornerBroken =GetPrivateProfileInt(strItem, _T("TftCornerBroken"), 0, strRoot);
		
		//2018.05.31 add by yhs -s
		CfBurrA			=GetPrivateProfileInt(strItem, _T("CfBurrA"), 0, strRoot);
		CfBurrB			=GetPrivateProfileInt(strItem, _T("CfBurrB"), 0, strRoot);
		CfBurrC			=GetPrivateProfileInt(strItem, _T("CfBurrC"), 0, strRoot);
		CfBurrD			=GetPrivateProfileInt(strItem, _T("CfBurrD"), 0, strRoot);
		TftBurrA		=GetPrivateProfileInt(strItem, _T("TftBurrA"), 0, strRoot);
		TftBurrB		=GetPrivateProfileInt(strItem, _T("TftBurrB"), 0, strRoot);
		TftBurrC		=GetPrivateProfileInt(strItem, _T("TftBurrC"), 0, strRoot);
		TftBurrD		=GetPrivateProfileInt(strItem, _T("TftBurrD"), 0, strRoot);
		CfBrokenA		=GetPrivateProfileInt(strItem, _T("CfBrokenA"), 0, strRoot);
		CfBrokenB		=GetPrivateProfileInt(strItem, _T("CfBrokenB"), 0, strRoot);
		CfBrokenC		=GetPrivateProfileInt(strItem, _T("CfBrokenC"), 0, strRoot);
		CfBrokenD		=GetPrivateProfileInt(strItem, _T("CfBrokenD"), 0, strRoot);
		TftBrokenA		=GetPrivateProfileInt(strItem, _T("TftBrokenA"), 0, strRoot);
		TftBrokenB		=GetPrivateProfileInt(strItem, _T("TftBrokenB"), 0, strRoot);
		TftBrokenC		=GetPrivateProfileInt(strItem, _T("TftBrokenC"), 0, strRoot);
		TftBrokenD		=GetPrivateProfileInt(strItem, _T("TftBrokenD"), 0, strRoot);
		CfCornerBrokenA	=GetPrivateProfileInt(strItem, _T("CfCornerBrokenA"), 0, strRoot);
		CfCornerBrokenB	=GetPrivateProfileInt(strItem, _T("CfCornerBrokenB"), 0, strRoot);
		CfCornerBrokenC	=GetPrivateProfileInt(strItem, _T("CfCornerBrokenC"), 0, strRoot);
		CfCornerBrokenD	=GetPrivateProfileInt(strItem, _T("CfCornerBrokenD"), 0, strRoot);
		TftCornerBrokenA=GetPrivateProfileInt(strItem, _T("TftCornerBrokenA"), 0, strRoot);
		TftCornerBrokenB=GetPrivateProfileInt(strItem, _T("TftCornerBrokenB"), 0, strRoot);
		TftCornerBrokenC=GetPrivateProfileInt(strItem, _T("TftCornerBrokenC"), 0, strRoot);
		TftCornerBrokenD=GetPrivateProfileInt(strItem, _T("TftCornerBrokenD"), 0, strRoot);
		//2018.05.31 add by yhs -e

		CfCellBroken	=GetPrivateProfileInt(strItem, _T("CfCellBroken"), 0, strRoot);	//2015.08.26
		TftCellBroken	=GetPrivateProfileInt(strItem, _T("TftCellBroken"), 0, strRoot);
		Contact			=GetPrivateProfileInt(strItem, _T("Contact"), 0,strRoot);

		CfPimplePoint  = GetPrivateProfileInt(strItem, _T("CfPimplePoint"), 0,strRoot);
		TftPimplePoint = GetPrivateProfileInt(strItem, _T("TftPimplePoint"), 0,strRoot);
		CfPimpleLine   = GetPrivateProfileInt(strItem, _T("CfPimpleLine"), 0,strRoot);
		TftPimpleLine  = GetPrivateProfileInt(strItem, _T("TftPimpleLine"), 0,strRoot);
		CfPimpleCircle = GetPrivateProfileInt(strItem, _T("CfPimpleCircle"), 0,strRoot);
		CfWavePattern  = GetPrivateProfileInt(strItem, _T("CfWavePattern"), 0,strRoot);
		EtcLine		   = GetPrivateProfileInt(strItem, _T("EtsLine"), 0,strRoot);
		EtcPoint	   = GetPrivateProfileInt(strItem, _T("EtsPoint"), 0,strRoot);

		PolMiss		   = GetPrivateProfileInt(strItem, _T("PolMiss"), 0,strRoot);	//2016.02.24

		AlignFail	   = GetPrivateProfileInt(strItem, _T("AlignFail"), 0, strRoot);	//2016.05.11
		CfNgCnt		   = GetPrivateProfileInt(PNL_COUNT_SET, _T("CF_NG_CNT"), 0, strRoot);
		TftNgCnt	   = GetPrivateProfileInt(PNL_COUNT_SET, _T("TFT_NG_CNT"), 0, strRoot);
		PenetNgCnt	   = GetPrivateProfileInt(PNL_COUNT_SET, _T("PENETRATE_NG_CNT"), 0, strRoot);

		//2016.11.19		Mr.Chu				Luminance
		CFCLuminance		= GetPrivateProfileInt(strItem, _T("CFCLuminance"), 0,strRoot);
		CBCLuminance		= GetPrivateProfileInt(strItem, _T("CBCLuminance"), 0,strRoot);
		CFSLuminance		= GetPrivateProfileInt(strItem, _T("CFSLuminance"), 0,strRoot);
		CBSLuminance		= GetPrivateProfileInt(strItem, _T("CBSLuminance"), 0,strRoot);
		TFCLuminance		= GetPrivateProfileInt(strItem, _T("TFCLuminance"), 0,strRoot);
		TBCLuminance		= GetPrivateProfileInt(strItem, _T("TBCLuminance"), 0,strRoot);
		TFSLuminance		= GetPrivateProfileInt(strItem, _T("TFSLuminance"), 0,strRoot);
		TBSLuminance		= GetPrivateProfileInt(strItem, _T("TBSLuminance"), 0,strRoot);
		TDILuminance		= GetPrivateProfileInt(strItem, _T("TDILuminance"), 0,strRoot);
		//
		CfProtection		= GetPrivateProfileInt(strItem, _T("CfProtection"), 0,strRoot);
		TftProtection		= GetPrivateProfileInt(strItem, _T("TftProtection"), 0,strRoot);

		Bgradecheck			= GetPrivateProfileInt(strItem, _T("Bgradecheck"), 0,strRoot);
		Cgradecheck			= GetPrivateProfileInt(strItem, _T("Cgradecheck"), 0,strRoot);
		BubbleStation1     = GetPrivateProfileInt(strItem, _T("BubbleStation1"), 0,strRoot);  //2020.02.28 add by mby bubble Station
		BubbleStation2     = GetPrivateProfileInt(strItem, _T("BubbleStation2"), 0,strRoot);
		BubbleStation3     = GetPrivateProfileInt(strItem, _T("BubbleStation3"), 0,strRoot);
	    BubbleStation4     = GetPrivateProfileInt(strItem, _T("BubbleStation4"), 0,strRoot);
		BubbleStation5     = GetPrivateProfileInt(strItem, _T("BubbleStation5"), 0,strRoot);
		BubbleStation6     = GetPrivateProfileInt(strItem, _T("BubbleStation6"), 0,strRoot);
		BubbleStation7     = GetPrivateProfileInt(strItem, _T("BubbleStation7"), 0,strRoot);
	    BubbleStation8     = GetPrivateProfileInt(strItem, _T("BubbleStation8"), 0,strRoot);
		BubbleStation9     = GetPrivateProfileInt(strItem, _T("BubbleStation9"), 0,strRoot);
		BubbleStation10    = GetPrivateProfileInt(strItem, _T("BubbleStation10"), 0,strRoot);
		BubbleStation11    = GetPrivateProfileInt(strItem, _T("BubbleStation11"), 0,strRoot);
	    BubbleStation12    = GetPrivateProfileInt(strItem, _T("BubbleStation12"), 0,strRoot);

		//2016.12.12
		TotalCnt			= GetPrivateProfileInt(strItem, _T("TotalCnt"), 0,strRoot);

		if(TotalCnt <=0)	//만약 값이 없는 경우 프로그램 꼬임 방지를 위한 예외처리
			TotalCnt = INSP_TYPE_MAX*2;
		//
	}

	void WriteDefInfo(){

		strValue.Format(_T("%d"), PanelCodeTest );
		WritePrivateProfileString(strItem, _T("PanelCodeTest"), strValue, strRoot);
		
		strValue.Format(_T("%d"), DustType );
		WritePrivateProfileString(strItem, _T("DustType"), strValue, strRoot);
		strValue.Format(_T("%d"), CfScrach );
		WritePrivateProfileString(strItem, _T("CfScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), TftScrach );
		WritePrivateProfileString(strItem, _T("TftScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), CFWhiteScratch );
		WritePrivateProfileString(strItem, _T("CFWhiteScratch"), strValue, strRoot);
		strValue.Format(_T("%d"), CFBlackScratch );
		WritePrivateProfileString(strItem, _T("CFBlackScratch"), strValue, strRoot);
		strValue.Format(_T("%d"), ItoPeel );
		WritePrivateProfileString(strItem, _T("ItoPeel"), strValue, strRoot);
		strValue.Format(_T("%d"), CfDimplePoint );
		WritePrivateProfileString(strItem, _T("CfDimplePoint"), strValue, strRoot);
		strValue.Format(_T("%d"), TftDimplePoint );
		WritePrivateProfileString(strItem, _T("TftDimplePoint"), strValue, strRoot);
		strValue.Format(_T("%d"), CfDimpleLine );
		WritePrivateProfileString(strItem, _T("CfDimpleLine"), strValue, strRoot);
		strValue.Format(_T("%d"), TftDimpleLine );
		WritePrivateProfileString(strItem, _T("TftDimpleLine"), strValue, strRoot);
		strValue.Format(_T("%d"), CfEdgeilegal );
		WritePrivateProfileString(strItem, _T("CfEdgeilegal"), strValue, strRoot);
		strValue.Format(_T("%d"), TftEdgeilegal );
		WritePrivateProfileString(strItem, _T("TftEdgeilegal"), strValue, strRoot);
		strValue.Format(_T("%d"), CfWhiteBlur );
		WritePrivateProfileString(strItem, _T("CfWhiteBlur"), strValue, strRoot);
		strValue.Format(_T("%d"), TftWhiteBlur );
		WritePrivateProfileString(strItem, _T("TftWhiteBlur"), strValue, strRoot);
		strValue.Format(_T("%d"), SocketBurn );
		WritePrivateProfileString(strItem, _T("SocketBurn"), strValue, strRoot);
		strValue.Format(_T("%d"), WanesBroken );
		WritePrivateProfileString(strItem, _T("WanesBroken"), strValue, strRoot);
		strValue.Format(_T("%d"), SocketScrach );
		WritePrivateProfileString(strItem, _T("SocketScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), CfMeasureFail );
		WritePrivateProfileString(strItem, _T("CfMeasureFail"), strValue, strRoot);
		strValue.Format(_T("%d"), TftMeasureFail );
		WritePrivateProfileString(strItem, _T("TftMeasureFail"), strValue, strRoot);
		strValue.Format(_T("%d"), Burr );
		WritePrivateProfileString(strItem, _T("Burr"), strValue, strRoot);
		strValue.Format(_T("%d"), CfPolBubble );
		WritePrivateProfileString(strItem, _T("CfPolBubble"), strValue, strRoot);
		strValue.Format(_T("%d"), TftPolBubble );
		WritePrivateProfileString(strItem, _T("TftPolBubble"), strValue, strRoot);
		strValue.Format(_T("%d"), CfPolScrach );
		WritePrivateProfileString(strItem, _T("CfPolScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), TftPolScrach );
		WritePrivateProfileString(strItem, _T("TftPolScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), mura );
		WritePrivateProfileString(strItem, _T("mura"), strValue, strRoot);
		strValue.Format(_T("%d"), CFpolSubstance );
		WritePrivateProfileString(strItem, _T("CFpolSubstance"), strValue, strRoot);
		strValue.Format(_T("%d"), TFTpolSubstance );
		WritePrivateProfileString(strItem, _T("TFTpolSubstance"), strValue, strRoot);
		strValue.Format(_T("%d"), BlackScrach );
		WritePrivateProfileString(strItem, _T("BlackScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), WhiteScrach );
		WritePrivateProfileString(strItem, _T("WhiteScrach"), strValue, strRoot);
		strValue.Format(_T("%d"), PolMinor );
		WritePrivateProfileString(strItem, _T("PolMinor"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBurr );	//2015.07.13
		WritePrivateProfileString(strItem, _T("CfBurr"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBurr );
		WritePrivateProfileString(strItem, _T("TftBurr"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBroken );
		WritePrivateProfileString(strItem, _T("CfBroken"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBroken );
		WritePrivateProfileString(strItem, _T("TftBroken"), strValue, strRoot);
		strValue.Format(_T("%d"), CfCornerBroken );
		WritePrivateProfileString(strItem, _T("CfCornerBroken"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCornerBroken );
		WritePrivateProfileString(strItem, _T("TftCornerBroken"), strValue, strRoot);
		
		//2018.05.31 add by yhs -s
		strValue.Format(_T("%d"), CfBurrA );
		WritePrivateProfileString(strItem, _T("CfBurrA"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBurrB );
		WritePrivateProfileString(strItem, _T("CfBurrB"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBurrC );
		WritePrivateProfileString(strItem, _T("CfBurrC"), strValue, strRoot);	
		strValue.Format(_T("%d"), CfBurrD );
		WritePrivateProfileString(strItem, _T("CfBurrD"), strValue, strRoot);	
		strValue.Format(_T("%d"), TftBurrA );
		WritePrivateProfileString(strItem, _T("TftBurrA"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBurrB );
		WritePrivateProfileString(strItem, _T("TftBurrB"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBurrC );
		WritePrivateProfileString(strItem, _T("TftBurrC"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBurrD );
		WritePrivateProfileString(strItem, _T("TftBurrD"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBrokenA );
		WritePrivateProfileString(strItem, _T("CfBrokenA"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBrokenB );
		WritePrivateProfileString(strItem, _T("CfBrokenB"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBrokenC );
		WritePrivateProfileString(strItem, _T("CfBrokenC"), strValue, strRoot);
		strValue.Format(_T("%d"), CfBrokenD );
		WritePrivateProfileString(strItem, _T("CfBrokenD"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBrokenA );
		WritePrivateProfileString(strItem, _T("TftBrokenA"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBrokenB );
		WritePrivateProfileString(strItem, _T("TftBrokenB"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBrokenC );
		WritePrivateProfileString(strItem, _T("TftBrokenC"), strValue, strRoot);
		strValue.Format(_T("%d"), TftBrokenD );
		WritePrivateProfileString(strItem, _T("TftBrokenD"), strValue, strRoot);
		strValue.Format(_T("%d"), CfCornerBrokenA );
		WritePrivateProfileString(strItem, _T("CfCornerBrokenA"), strValue, strRoot);
		strValue.Format(_T("%d"), CfCornerBrokenB );
		WritePrivateProfileString(strItem, _T("CfCornerBrokenB"), strValue, strRoot);
		strValue.Format(_T("%d"), CfCornerBrokenC );
		WritePrivateProfileString(strItem, _T("CfCornerBrokenC"), strValue, strRoot);
		strValue.Format(_T("%d"), CfCornerBrokenD );
		WritePrivateProfileString(strItem, _T("CfCornerBrokenD"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCornerBrokenA );
		WritePrivateProfileString(strItem, _T("TftCornerBrokenA"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCornerBrokenB );
		WritePrivateProfileString(strItem, _T("TftCornerBrokenB"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCornerBrokenC );
		WritePrivateProfileString(strItem, _T("TftCornerBrokenC"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCornerBrokenD );
		WritePrivateProfileString(strItem, _T("TftCornerBrokenD"), strValue, strRoot);	

		//2018.05.31 add by yhs -e
		
		strValue.Format(_T("%d"), CfCellBroken );	//2015.08.26
		WritePrivateProfileString(strItem, _T("CfCellBroken"), strValue, strRoot);
		strValue.Format(_T("%d"), TftCellBroken );
		WritePrivateProfileString(strItem, _T("TftCellBroken"), strValue, strRoot);
		strValue.Format(_T("%d"),  Contact );
		WritePrivateProfileString(strItem, _T("Contact"), strValue, strRoot);

		strValue.Format(_T("%d"), CfPimplePoint );
		WritePrivateProfileString(strItem, _T("CfPimplePoint"), strValue,strRoot);
		strValue.Format(_T("%d"), TftPimplePoint );
		WritePrivateProfileString(strItem, _T("TftPimplePoint"), strValue,strRoot);
		strValue.Format(_T("%d"), CfPimpleLine );
		WritePrivateProfileString(strItem, _T("CfPimpleLine"), strValue,strRoot);
		strValue.Format(_T("%d"), TftPimpleLine );
		WritePrivateProfileString(strItem, _T("TftPimpleLine"), strValue,strRoot);
		strValue.Format(_T("%d"), CfPimpleCircle );
		WritePrivateProfileString(strItem, _T("CfPimpleCircle"), strValue,strRoot);
		strValue.Format(_T("%d"), CfWavePattern );
		WritePrivateProfileString(strItem, _T("CfWavePattern"), strValue,strRoot);
		strValue.Format(_T("%d"), EtcLine );
		WritePrivateProfileString(strItem, _T("EtsLine"), strValue,strRoot);
		strValue.Format(_T("%d"), EtcPoint );
		WritePrivateProfileString(strItem, _T("EtsPoint"), strValue,strRoot);

		//2016.02.24
		strValue.Format(_T("%d"), PolMiss );	
		WritePrivateProfileString(strItem, _T("PolMiss"), strValue,strRoot);

		//2016.05.11
		strValue.Format(_T("%d"), AlignFail );	
		WritePrivateProfileString(strItem, _T("AlignFail"), strValue,strRoot);
		strValue.Format(_T("%d"), CfNgCnt );
		WritePrivateProfileString(PNL_COUNT_SET, _T("CF_NG_CNT"), strValue, strRoot);
		strValue.Format(_T("%d"), TftNgCnt );
		WritePrivateProfileString(PNL_COUNT_SET, _T("TFT_NG_CNT"), strValue, strRoot);
		strValue.Format(_T("%d"), PenetNgCnt );
		WritePrivateProfileString(PNL_COUNT_SET, _T("PENETRATE_NG_CNT"), strValue, strRoot);

		//2016.11.21
		strValue.Format(_T("%d"), CFCLuminance );	
		WritePrivateProfileString(strItem, _T("CFCLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), CBCLuminance );	
		WritePrivateProfileString(strItem, _T("CBCLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), CFSLuminance );	
		WritePrivateProfileString(strItem, _T("CFSLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), CBSLuminance );	
		WritePrivateProfileString(strItem, _T("CBSLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), TFCLuminance );	
		WritePrivateProfileString(strItem, _T("TFCLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), TBCLuminance );	
		WritePrivateProfileString(strItem, _T("TBCLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), TFSLuminance );	
		WritePrivateProfileString(strItem, _T("TFSLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), TBSLuminance );	
		WritePrivateProfileString(strItem, _T("TBSLuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), TDILuminance );	
		WritePrivateProfileString(strItem, _T("TDILuminance"), strValue,strRoot);
		strValue.Format(_T("%d"), CfProtection );	
		WritePrivateProfileString(strItem, _T("CfProtection"), strValue,strRoot);
		strValue.Format(_T("%d"), TftProtection );	
		WritePrivateProfileString(strItem, _T("TftProtection"), strValue,strRoot);
		// 2019.03.15 by ryu
		strValue.Format(_T("%d"), Bgradecheck );	
		WritePrivateProfileString(strItem, _T("Bgradecheck"), strValue,strRoot);
		strValue.Format(_T("%d"), Cgradecheck );	
		WritePrivateProfileString(strItem, _T("Cgradecheck"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation1 );									//2020.02.28 add by mby
		WritePrivateProfileString(strItem, _T("BubbleStation1"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation2 );	
		WritePrivateProfileString(strItem, _T("BubbleStation2"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation3 );	
		WritePrivateProfileString(strItem, _T("BubbleStation3"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation4 );	
		WritePrivateProfileString(strItem, _T("BubbleStation4"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation5 );	
		WritePrivateProfileString(strItem, _T("BubbleStation5"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation6 );	
		WritePrivateProfileString(strItem, _T("BubbleStation6"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation7 );	
		WritePrivateProfileString(strItem, _T("BubbleStation7"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation8 );	
		WritePrivateProfileString(strItem, _T("BubbleStation8"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation9 );	
		WritePrivateProfileString(strItem, _T("BubbleStation9"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation10 );	
		WritePrivateProfileString(strItem, _T("BubbleStation10"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation11 );	
		WritePrivateProfileString(strItem, _T("BubbleStation11"), strValue,strRoot);
		strValue.Format(_T("%d"), BubbleStation12 );	
		WritePrivateProfileString(strItem, _T("BubbleStation12"), strValue,strRoot);

		//2016.12.12
		strValue.Format(_T("%d"), TotalCnt );	
		WritePrivateProfileString(strItem, _T("TotalCnt"), strValue,strRoot);
	}
	
	void initDefType()
	{
		PanelCodeTest = 0;

		DustType = 0;
		CfScrach= 0;			    // CF 깨짐
		CFWhiteScratch= 0;			//	CF 기판 흰색 기스
		CFBlackScratch= 0;			//	CF 기판 검은색 기스
		ItoPeel= 0;				//	ITO 벗겨짐
		//PimplePoint= 0;			//	PIMPLE 점형
		CfDimplePoint= 0;			//	DIMPLE 점형
		//PimpleLine= 0;				//	PIMPLE 선형
		CfDimpleLine= 0;				//	DIMPLE 선형
		CfEdgeilegal= 0;			//	CF 엣칭 불량
		CfWhiteBlur= 0;			//	CF 백탁
		SocketBurn= 0;				//	단자 탐
		WanesBroken= 0;	
		CfMeasureFail =0;
		SocketScrach=0;
		Burr=0;
		CfPolBubble = 0;
		CfPolScrach = 0;
		mura = 0;
		CFpolSubstance = 0;
		TFTpolSubstance = 0;		// Add by KYH01 TFT POL 이물 추가
		TftDimpleLine = 0;
		TftEdgeilegal = 0;
		TftWhiteBlur = 0;
		TftMeasureFail = 0;
		TftDimplePoint = 0;
		TftPolBubble = 0;
		TftPolScrach = 0;
		TftScrach = 0;
		BlackScrach = 0;
		WhiteScrach = 0;
		PolMinor = 0;
		TotalCnt =0;
		CfBurr = 0;	//2015.07.13
		TftBurr = 0;
		CfBroken = 0;
		TftBroken = 0;
		CfCornerBroken = 0;
		TftCornerBroken = 0;
		//2018.05.15 -s
		CfBurrA = 0;
		CfBurrB = 0;
		CfBurrC = 0;
		CfBurrD = 0;
		TftBurrA = 0;
		TftBurrB = 0;
		TftBurrC = 0;
		TftBurrD = 0;
		CfBrokenA = 0;
		CfBrokenB = 0;
		CfBrokenC = 0;
		CfBrokenD = 0;
		TftBrokenA = 0;
		TftBrokenB = 0;
		TftBrokenC = 0;
		TftBrokenD = 0;
		CfCornerBrokenA = 0;
		CfCornerBrokenB = 0;
		CfCornerBrokenC = 0;
		CfCornerBrokenD = 0;
		TftCornerBrokenA = 0;
		TftCornerBrokenB = 0;
		TftCornerBrokenC = 0;
		TftCornerBrokenD = 0;
		//2018.05.15 -e
		CfCellBroken = 0;	//2015.08.25
		TftCellBroken = 0;

		Contact = 0;
		CfPimplePoint = 0;
		TftPimplePoint = 0;
		CfPimpleLine = 0;
		TftPimpleLine = 0;
		CfPimpleCircle = 0;
		CfWavePattern = 0;
		EtcLine = 0;
		EtcPoint = 0;

		PolMiss = 0;	//2016.02.24

		CfNgCnt = 0;
		TftNgCnt = 0;
		PenetNgCnt = 0;

		//2016.05.11 Mr.Chu		Align Fail 확인
		AlignFail = 0;

		//2016.11.19		Mr.Chu				Luminance
		CFCLuminance = 0;
		CBCLuminance = 0;
		CFSLuminance = 0;
		CBSLuminance = 0;
		TFCLuminance = 0;
		TBCLuminance = 0;
		TFSLuminance = 0;
		TBSLuminance = 0;
		TDILuminance = 0;
		CfProtection	= 0;
		TftProtection	= 0;

		Bgradecheck  = 0;
		Cgradecheck  = 0;
		//2020.02.28 add by mby
		BubbleStation1 = 0;
		BubbleStation2 = 0;
		BubbleStation3 = 0;
		BubbleStation4 = 0;
		BubbleStation5 = 0;
		BubbleStation6 = 0;
		BubbleStation7 = 0;
		BubbleStation8 = 0;
		BubbleStation9 = 0;
		BubbleStation10 = 0;
		BubbleStation11 = 0;
		BubbleStation12 = 0;  
	}
};

struct STRU_OVERLAP_DEFTYPE_INFO
{
	TCHAR cPanelId [INSP_TYPE_MAX][MAX_DEFECT_CNT][LEN_PANEL_ID + 2];
	TCHAR cImgPath [INSP_TYPE_MAX][MAX_DEFECT_CNT][LEN_IMAGE_PATH + 2];
	DOUBLE startX  [INSP_TYPE_MAX][MAX_DEFECT_CNT];
	DOUBLE startY  [INSP_TYPE_MAX][MAX_DEFECT_CNT];
	DOUBLE endX    [INSP_TYPE_MAX][MAX_DEFECT_CNT];
	DOUBLE endY    [INSP_TYPE_MAX][MAX_DEFECT_CNT];
	int DefTypeCnt [INSP_TYPE_MAX];
	int DefTypeCode		[INSP_TYPE_MAX];
	int DefTypeFlag		[INSP_TYPE_MAX][MAX_DEFECT_CNT];
	int nAlarmCount [INSP_TYPE_MAX];
	BOOL bStopFlag	[INSP_TYPE_MAX];
	CTime StartDate[INSP_TYPE_MAX];
	void init()
	{
		for (int i=0; i < INSP_TYPE_MAX; i++ )
		{
			DefTypeCnt[i]	= 0;
			StartDate[i]	= 0;
			DefTypeCode[i]		= 0;
			bStopFlag[i]		=FALSE;
		}

		memset(cPanelId, 0X00,sizeof(cPanelId));
		memset(cImgPath, 0X00,sizeof(cImgPath));
		memset(startX, 0, sizeof(startX		 ));
		memset(startY, 0, sizeof(startY		 ));
		memset(endX  , 0, sizeof(endX		 ));
		memset(endY  , 0, sizeof(endY		 ));
		memset(DefTypeFlag  , 0, sizeof(DefTypeFlag		 ));
		
	}
	void init_DefType(int nDefType)
	{
		DefTypeCnt[nDefType] = 0;
		StartDate[nDefType]=0;
		DefTypeCode[nDefType]=0;
		bStopFlag[nDefType]=0;
	
		memset(cPanelId[nDefType], 0X00,sizeof(cPanelId[nDefType]));
		memset(cImgPath[nDefType], 0X00,sizeof(cImgPath[nDefType]));
		memset(startX[nDefType], 0, sizeof(startX[nDefType]));
		memset(startY[nDefType], 0, sizeof(startY[nDefType]));
		memset(endX  [nDefType], 0, sizeof(endX  [nDefType]));
		memset(endY  [nDefType], 0, sizeof(endY  [nDefType]));
		memset(DefTypeFlag  [nDefType], 0, sizeof(DefTypeFlag  [nDefType]));

	}
};
//
//struct STRU_DEFECT_INFO
//{
//	int nPosX;
//	int nPosY;
//	int nWidth;
//	int nHeight;
//	int nSize;		// 면적
//	int nVisionID;
//	int nCamNo;
//	int nType;		// Defect 종류
//
//	STRU_DEFECT_INFO()
//	{
//		nPosX = 0;
//		nPosY = 0;
//		nWidth = 0;
//		nHeight = 0;
//		nSize = 0;
//		nVisionID = 0;
//		nCamNo = 0;
//		nType = 0;
//	}
//};

struct ResultHeaderInfo 
{   /*/////////////////////////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    출처			필수		COMMENT
	/////////////////////////////////////////////////////////////////////////////////////////////*/
	CString layer_No;						///		AA		EQP			Y			고정 "00"
	int     file_unit;						///		9		EQP			Y			0 - Lot Unit, 1 - Glass	Unit, 2 - Short glass Unit,	3 - Panel Unit
	CString Insp_start_date;				///	YYYY/MM/DD	EQP			Y			검사 시작 날짜
	CString Insp_start_time;				/// HH:MM:SS	EQP			Y			검사 시작 시간
	CString Insp_end_date;					/// YYYY/MM/DD	EQP			Y			검사 종료 날짜
	CString Insp_end_time;					/// HH:MM:SS	EQP			Y			검사 종료 시간
	int		EQP_ID;							/// 999			EQP			Y			Equipment ID (by CEC Panda)
	int		Insp_EQP_No;					/// AA			EQP			Y			Device No
	CString Oper_No;						/// AAAAA		EQP			Y			Step ID (by CIM)
	CString Oper_ID;						/// AAAAAA		EQP			N	
	
	ResultHeaderInfo()
	{
		layer_No = _T("");		Insp_start_date = _T("");		Insp_start_time = _T("");		
		Insp_end_date = _T("");	Insp_end_time = _T("");			Oper_No = _T("");					
		Oper_ID = _T("");		file_unit = 0;					EQP_ID = 0;							
		Insp_EQP_No = 0;					
	}

};


struct ResultPanelInfo 
{   /*/////////////////////////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    출처			필수		COMMENT
	/////////////////////////////////////////////////////////////////////////////////////////////*/
	CString layer_No;						///		AA		EQP			Y			고정 "13"
	CString Panel_ID;						///	A-18-A		EQP			Y			CIM 에서 제공											
	int		Panel_Count;					///	 99999		EQP			Y			
	CString Judge;							///		 A		EQP			Y	
	CString judge_code_1;					///	  AAAA		EQP			Y	
	CString judge_code_2;					///	  AAAA		EQP			Y	
	CString	Recipe_ID;						/// A-16-A		EQP			Y	
	ResultPanelInfo()
	{
		layer_No = _T("");		Panel_ID = _T("");		Judge = _T("");						
		judge_code_1 = _T("");	judge_code_2 = _T("");	Recipe_ID = _T("");		
		Panel_Count = 0;	
	}
};


struct ResultDefectInfo 
{   /*/////////////////////////////////////////////////////////////////////////////////////////////
	ITEM										FORTMAT    단위		AVL		ABI		설명
	/////////////////////////////////////////////////////////////////////////////////////////////*/
	CString layer_No;						///		AA				0       0		고정 "30"
	int		Defect_No;						///	   999				O		O		0 ~ 999
	CString	Defect_Code;					///	 AAAAA				O		O		K+4桁（ex：KSoRh）
	int		Flag;							///	   	 9				O		O		0 : CF, 1 : TFT, 2 : POL　
	int		Pixel_X;						///	 99999				O		O		Image 좌표 X
	int		Pixel_Y;						///  99999				O		O		Image 좌표 Y
	int		Gate_Start_No;					///  99999				X		O		Gate Line 의 결함 시작 위치
	int		Data_Start_No;					///  99999				X		O		Data Line 의 결함 시작 위치
	int		Gate_End_No;					///  99999				X		O		Gate Line 의 결함 끝 위치
	int		Data_End_No;					///  99999				X		O		Data Line 의 결함 끝 위치
	DOUBLE	Coord_Start_X;					///±9999.999	um		O		O		결함 시작 위치 X
	DOUBLE	Coord_Start_Y;					///±9999.999	um		O		O		결함 시작 위치 Y
	DOUBLE	Coord_End_X;					///±9999.999	um		O		O		결함 끝 위치 X
	DOUBLE	Coord_End_Y;					///±9999.999	um		O		O		결함 끝 위치 Y
	DOUBLE	Defect_Size;					///	   999		um		O		O		결함강도
	CString	Defect_Img_No;					///9-12-9.AAA			O		O		결함 이미지 파일 이름
	DOUBLE	Img_Size_X;						///  9999.99	um		O		O		결함 이미지 크기 X
	DOUBLE	Img_Size_Y;						///  9999.99	um		O		O		결함 이미지 크기 Y
	int		Defect_Type;					///		99				O		O		결함 종류
	int		Camera_No;						///		 9				O		O		1 : Side, 2 : Coaxial, 3 : Transmission
	CString	Defect_Img_Path;
	//2016.02.01 Packing Defect
	CString InspTime;
	CString Device;
	CString Cell_ID;
	CString Pad;
	CString Judge;
	CString P_Defect_Type;
	DOUBLE Area;
	DOUBLE Box_Area;
	DOUBLE Box_X_Max;
	DOUBLE Box_X_Min;
	DOUBLE Box_Y_Max;
	DOUBLE Box_Y_Min;
	DOUBLE Center_X;
	DOUBLE Center_Y;
	DOUBLE Length;
	DOUBLE Breadth;
	DOUBLE Elongation;
	DOUBLE Compactness;
	DOUBLE Holecount;
	DOUBLE Feret_Min_Angle;
	DOUBLE Feret_Max_Angle;
	DOUBLE Feret_Min;
	DOUBLE Feret_Max;
	DOUBLE Feret_Mean;
	DOUBLE Feret_X;
	DOUBLE Feret_Y;
	DOUBLE Feret_Elongation;
	DOUBLE Pixel_Max;
	DOUBLE Pixel_Mean;
	DOUBLE Pixel_Min;
	DOUBLE Pixel_Sigma;
	DOUBLE Pixel_Sum;



	ResultDefectInfo(){
		layer_No = _T("");
		Defect_No = 0;						///	   999				O		O		0 ~ 999
		Defect_Code = _T("");
		Flag = 0;							///	   	 9				O		O		0 : CF, 1 : TFT, 2 : POL　
		Pixel_X = 0;						///	 99999				O		O		Image 좌표 X
		Pixel_Y = 0;						///  99999				O		O		Image 좌표 Y
		Gate_Start_No = 0;					///  99999				X		O		Gate Line 의 결함 시작 위치
		Data_Start_No = 0;					///  99999				X		O		Data Line 의 결함 시작 위치
		Gate_End_No = 0;					///  99999				X		O		Gate Line 의 결함 끝 위치
		Data_End_No = 0;					///  99999				X		O		Data Line 의 결함 끝 위치
		Coord_Start_X = 0;					///±9999.999	um		O		O		결함 시작 위치 X
		Coord_Start_Y = 0;					///±9999.999	um		O		O		결함 시작 위치 Y
		Coord_End_X = 0;					///±9999.999	um		O		O		결함 끝 위치 X
		Coord_End_Y = 0;					///±9999.999	um		O		O		결함 끝 위치 Y
		Defect_Size = 0;					///	   999		um		O		O		결함강도
		Defect_Img_No = _T("");
		Img_Size_X = 0;						///  9999.99	um		O		O		결함 이미지 크기 X
		Img_Size_Y = 0;						///  9999.99	um		O		O		결함 이미지 크기 Y
		Defect_Type = 0;					///		99				O		O		결함 종류
		Camera_No = 0;						///		 9				O		O		1 : Side, 2 : Coaxial, 3 : Transmission
		Defect_Img_Path = _T("");
		//2016.02.01 Packing Defect
		InspTime = _T("");
		Device = _T("");
		Cell_ID = _T("");
		Pad = _T("");
		Judge = _T("");
		P_Defect_Type = _T("");
		Area = 0;
		Box_Area = 0;
		Box_X_Max = 0;
		Box_X_Min = 0;
		Box_Y_Max = 0;
		Box_Y_Min = 0;
		Center_X = 0;
		Center_Y = 0;
		Length = 0;
		Breadth = 0;
		Elongation = 0;
		Compactness = 0;
		Holecount = 0;
		Feret_Min_Angle = 0;
		Feret_Max_Angle = 0;
		Feret_Min = 0;
		Feret_Max = 0;
		Feret_Mean = 0;
		Feret_X = 0;
		Feret_Y = 0;
		Feret_Elongation = 0;
		Pixel_Max = 0;
		Pixel_Mean = 0;
		Pixel_Min = 0;
		Pixel_Sigma = 0;
		Pixel_Sum = 0;
	}
};

struct STRU_DEFECT_INFO
{	
	ResultHeaderInfo Hinfo;
	ResultPanelInfo  Pinfo;
	ResultDefectInfo Dinfo;	
};

struct STRU_CLASSIFY_INFO
{	
	TCHAR	cPanelID[40];
	//TCHAR	cGlassID[40];	//2017.06.29	Qms Img 경로 내 GlassID 추가
	TCHAR	cPanelPath[1024];
	TCHAR	cStartDate[128];
	TCHAR	cStartTime[128];
	TCHAR	cDataDate[128];
	TCHAR	cDataTime[128];
	TCHAR	cPgVal[128];
	int		nOrderCnt;
	int		nTotalDefCnt[MAX_VISION_PC];
	int		nVisionNo[MAX_VISION_PC];
	BOOL	bComplete[MAX_VISION_PC];
	int		nPgGrade;
	int     nConFlag;
	LONGLONG llStartTime;

	STRU_CLASSIFY_INFO(){		
		llStartTime = 0;
		nOrderCnt= 0;
		nPgGrade = 0;
		nConFlag = -1;
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(cPanelPath, 0, sizeof(cPanelPath));
		memset(cStartDate, 0, sizeof(cStartDate));
		memset(cStartTime, 0, sizeof(cStartTime));
		memset(cDataDate, 0, sizeof(cDataDate));
		memset(cDataTime, 0, sizeof(cDataTime));
		memset(nTotalDefCnt, 0, sizeof(nTotalDefCnt));
		memset(nVisionNo, 0, sizeof(nVisionNo));
		memset(bComplete, 0, sizeof(bComplete));
		memset(cPgVal, 0, sizeof(cPgVal));
	}

	void init_info(){
		llStartTime = 0;
		nOrderCnt= 0;
		nPgGrade = 0;
		nConFlag = -1;
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(cPanelPath, 0, sizeof(cPanelPath));
		memset(cStartDate, 0, sizeof(cStartDate));
		memset(cStartTime, 0, sizeof(cStartTime));
		memset(cDataDate, 0, sizeof(cDataDate));
		memset(cDataTime, 0, sizeof(cDataTime));
		memset(nTotalDefCnt, 0, sizeof(nTotalDefCnt));
		memset(nVisionNo, 0, sizeof(nVisionNo));
		memset(bComplete, 0, sizeof(bComplete));
		memset(cPgVal, 0, sizeof(cPgVal));
	}
};


struct STRU_NG_FLAG_INFO
{	
	TCHAR	cPanelID[20];
	int		nNgFlag;
	BOOL	bComplete;
	LONGLONG llStartTime;
	TCHAR	cGrade[2];
	TCHAR	cJudge1[8];
	TCHAR	cJudge2[8];
	TCHAR   cCimMeasureData[128];
	BOOL bLowerEqpBypass;
	STRU_NG_FLAG_INFO()
	{		
		nNgFlag		= 0;	bComplete	= FALSE;   llStartTime = 0;
		memset(cPanelID, 0, sizeof(cPanelID));	memset(cGrade, 0, sizeof(cGrade));	
		memset(cJudge1, 0, sizeof(cJudge1));	memset(cJudge2, 0, sizeof(cJudge2));	
		memset(cCimMeasureData, 0, sizeof(cCimMeasureData));

	}

	void init_info()
	{
		nNgFlag		= 0;	bComplete	= FALSE;   llStartTime = 0;		bLowerEqpBypass = FALSE;
		memset(cPanelID, 0, sizeof(cPanelID));	memset(cGrade, 0, sizeof(cGrade));	
		memset(cJudge1, 0, sizeof(cJudge1));	memset(cJudge2, 0, sizeof(cJudge2));	
		memset(cCimMeasureData, 0, sizeof(cCimMeasureData));
	}

	void DefaultData(CString strPanelID, int nNGflag)
	{
		CString strTemp =_T("");
		int nEqpType = 0;
		strTemp.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
		nEqpType = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0,strTemp);

		if(nEqpType == EQP_TYPE_AVL)
		{
			_tcscpy(cGrade, JUDGE_B);
			if (nNgFlag > 0) nNgFlag = NG_FLAG_AVL_B_GRADE;
			else			 nNgFlag = nNGflag;
		}
		if(nEqpType == EQP_TYPE_ABI)
		{
			_tcscpy(cGrade, JUDGE_D);	
			if (nNgFlag > 0) nNgFlag = NG_FLAG_ABI_B_GRADE;
			else			 nNgFlag = nNGflag;
		}
		_tcscpy(cCimMeasureData, _T("0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"));
		_tcscpy(cJudge1, _T(""));	
		_tcscpy(cJudge2, _T(""));
		_tcscpy(cPanelID, strPanelID);	
		
	}
	// 강제 판정 등급 구분._2018.10.15
	void ForceDefaultData(CString strPanelID, int nNGflag)
	{
		CString strTemp =_T("");
		int nEqpType = 0;
		strTemp.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
		nEqpType = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0,strTemp);

		if(nEqpType == EQP_TYPE_AVL)
		{
			//_tcscpy(cGrade, JUDGE_B);
			if (nNGflag == 0)
			{
				_tcscpy(cGrade, JUDGE_A);
				nNgFlag = NG_FLAG_A_GRADE;
			}
			else if(nNGflag == 5)
			{
				_tcscpy(cGrade, JUDGE_B);
				nNgFlag = NG_FLAG_AVL_B_GRADE;
			}
			else // if(nNGflag == 6)
			{
				_tcscpy(cGrade, JUDGE_C);
				nNgFlag = NG_FLAG_C_GRADE;
			}

		}
		//ABI 강제 판정 미적용_2018.10.15
		if(nEqpType == EQP_TYPE_ABI)
		{
			_tcscpy(cGrade, JUDGE_D);	
			if (nNgFlag > 0) nNgFlag = NG_FLAG_ABI_B_GRADE;
			else			 nNgFlag = nNGflag;
		}
		_tcscpy(cCimMeasureData, _T("0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"));
		_tcscpy(cJudge1, _T(""));	
		_tcscpy(cJudge2, _T(""));
		_tcscpy(cPanelID, strPanelID);	
		
	}
};
//2016.08.17
struct STRU_MONITOR_MEASURE_INFO
{
	BOOL	bCalMode;
	BOOL	bModelSize;
	BOOL	bReferencePoint;
	DOUBLE	dPanel_X;
	DOUBLE  dPanel_Y;
	DOUBLE	d_DX1;
	DOUBLE  d_DY1;
	DOUBLE	d_DX2;
	DOUBLE	d_DY2;
	DOUBLE	d_DX3;
	DOUBLE	d_DY3;
	DOUBLE	d_DX4;
	DOUBLE	d_DY4;
	DOUBLE	d_DX5;
	DOUBLE  d_DY5;
	DOUBLE	d_DX6;
	DOUBLE	d_DY6;
	DOUBLE	d_DX7;
	DOUBLE	d_DY7;
	DOUBLE	d_DX8;
	DOUBLE	d_DY8;
	DOUBLE	d_DL1;
	DOUBLE	d_DL2;
	DOUBLE	d_DL3;
	DOUBLE	d_DL4;
	DOUBLE	d_DW1;
	DOUBLE	d_DW2;
	DOUBLE	d_DW3;
	DOUBLE	d_DW4;

	STRU_MONITOR_MEASURE_INFO()
	{
		bCalMode = FALSE;		bModelSize = FALSE;		bReferencePoint = FALSE;
		dPanel_X = 0;			dPanel_Y = 0;			
		d_DX1 = 0;	d_DY1 = 0;	d_DX2 = 0;	d_DY2 = 0;	d_DX3 = 0;	d_DY3 = 0;	d_DX4 = 0;	d_DY4 = 0;	
		d_DX5 = 0;	d_DY5 = 0;	d_DX6 = 0;	d_DY6 = 0;	d_DX7 = 0;	d_DY7 = 0;	d_DX8 = 0;	d_DY8 = 0;	
		d_DL1 = 0;	d_DL2 = 0;	d_DL3 = 0;	d_DL4 = 0;
		d_DW1 = 0;	d_DW2 = 0;	d_DW3 = 0;	d_DW4 = 0;
	}

	void init_info()
	{
		bCalMode = FALSE;		bModelSize = FALSE;		bReferencePoint = FALSE;
		dPanel_X = 0;			dPanel_Y = 0;			
		d_DX1 = 0;	d_DY1 = 0;	d_DX2 = 0;	d_DY2 = 0;	d_DX3 = 0;	d_DY3 = 0;	d_DX4 = 0;	d_DY4 = 0;	
		d_DX5 = 0;	d_DY5 = 0;	d_DX6 = 0;	d_DY6 = 0;	d_DX7 = 0;	d_DY7 = 0;	d_DX8 = 0;	d_DY8 = 0;	
		d_DL1 = 0;	d_DL2 = 0;	d_DL3 = 0;	d_DL4 = 0;
		d_DW1 = 0;	d_DW2 = 0;	d_DW3 = 0;	d_DW4 = 0;
	}

};

//2015.08.19
struct STRU_ALIGN_MEASURE_INFO
{	
	TCHAR	cPanelID[20];
	TCHAR	cCFMeasureDistanceData[100];
	TCHAR	cCFMeasureAngleData[100];
	TCHAR	cTFTMeasureDistanceData[100];
	TCHAR	cTFTMeasureAngleData[100];
	TCHAR	cCFMeasureFail[10];
	TCHAR	cTFTMeasureFail[10];
	LONGLONG llStartTime;
	BOOL	bVision1Complete;	//vision PC5
	BOOL	bVision2Complete;	//vision PC6
	DOUBLE	dVision1_X1;
	DOUBLE  dVision1_Y1;
	DOUBLE  dVision1_X2;
	DOUBLE  dVision1_Y2;
	DOUBLE  dVision1_Angle1;
	DOUBLE  dVision1_Angle2;
	DOUBLE  dVision1_Distance;
	DOUBLE	dVision2_X1;
	DOUBLE  dVision2_Y1;
	DOUBLE  dVision2_X2;
	DOUBLE	dVision2_Y2;
	DOUBLE	dVision2_Angle1;
	DOUBLE	dVision2_Angle2;
	DOUBLE	dVision2_Distance;
	//2016.08.09 Mr.Chu		Monitor 측정 Dat.	추 후 구조체 변경 할 수 있음.
	//Monitor
	BOOL	bVision5Complete;	//vision PC5
	BOOL	bVision6Complete;	//vision PC6

	DOUBLE	dCF_X1;
	DOUBLE	dCF_Y1;
	DOUBLE	dCF_X2;
	DOUBLE	dCF_Y2;
	DOUBLE	dCF_X3;
	DOUBLE	dCF_Y3;
	DOUBLE	dCF_X4;
	DOUBLE	dCF_Y4;

	DOUBLE	dTFT_X1;
	DOUBLE	dTFT_Y1;
	DOUBLE	dTFT_X2;
	DOUBLE	dTFT_Y2;
	DOUBLE	dTFT_X3;
	DOUBLE	dTFT_Y3;
	DOUBLE	dTFT_X4;
	DOUBLE	dTFT_Y4;

	DOUBLE	dCF_L1;
	DOUBLE	dCF_L2;
	DOUBLE	dCF_W1;
	DOUBLE	dCF_W2;
	DOUBLE	dTFT_L3;
	DOUBLE	dTFT_L4;
	DOUBLE	dTFT_W3;
	DOUBLE	dTFT_W4;

	//

	STRU_ALIGN_MEASURE_INFO()
	{		
		bVision1Complete = FALSE;	bVision2Complete = FALSE;
		bVision5Complete = FALSE;	bVision6Complete = FALSE;
		llStartTime = 0;		memset(cPanelID, 0, sizeof(cPanelID));	
		dVision1_X1 = 0;	dVision1_Y1 = 0;	dVision1_X2 = 0;	dVision1_Y2 = 0;
		dVision1_Angle1 = 0;	dVision1_Angle2 = 0;	dVision1_Distance = 0;
		dVision2_X1 = 0;	dVision2_Y1 = 0;	dVision2_X2 = 0;	dVision2_Y2 = 0;
		dVision2_Angle1 = 0;	dVision2_Angle2 = 0;	dVision2_Distance = 0;
		memset(cCFMeasureDistanceData, 0, sizeof(cCFMeasureDistanceData));
		memset(cCFMeasureAngleData, 0, sizeof(cCFMeasureAngleData));
		memset(cTFTMeasureDistanceData, 0, sizeof(cTFTMeasureDistanceData));
		memset(cTFTMeasureAngleData, 0, sizeof(cTFTMeasureAngleData));
		memset(cCFMeasureFail, 0, sizeof(cCFMeasureFail));
		memset(cTFTMeasureFail, 0, sizeof(cTFTMeasureFail));
		//Monitor
		dCF_X1 = 0;	dCF_Y1 = 0;	dCF_X2 = 0;	dCF_Y2 = 0;
		dCF_X3 = 0;	dCF_Y3 = 0;	dCF_X4 = 0;	dCF_Y4 = 0;
		dTFT_X1 = 0;	dTFT_Y1 = 0;	dTFT_X2 = 0;	dTFT_Y2 = 0;
		dTFT_X3 = 0;	dTFT_Y3 = 0;	dTFT_X4 = 0;	dTFT_Y4 = 0;
		dCF_L1 = 0;		dCF_L2 = 0;		dCF_W1 = 0;		dCF_W2 = 0;
		dTFT_L3 = 0;	dTFT_L4 = 0;	dTFT_W3 = 0;	dTFT_W4 = 0;
		//
	}

	void init_info()
	{
		bVision1Complete = FALSE;	bVision2Complete = FALSE;
		bVision5Complete = FALSE;	bVision6Complete = FALSE;
		llStartTime = 0;		memset(cPanelID, 0, sizeof(cPanelID));
		dVision1_X1 = 0;	dVision1_Y1 = 0;	dVision1_X2 = 0;	dVision1_Y2 = 0;
		dVision1_Angle1 = 0;	dVision1_Angle2 = 0;	dVision1_Distance = 0;
		dVision2_X1 = 0;	dVision2_Y1 = 0;	dVision2_X2 = 0;	dVision2_Y2 = 0;
		dVision2_Angle1 = 0;	dVision2_Angle2 = 0;	dVision2_Distance = 0;	
		memset(cCFMeasureDistanceData, 0, sizeof(cCFMeasureDistanceData));
		memset(cCFMeasureAngleData, 0, sizeof(cCFMeasureAngleData));
		memset(cTFTMeasureDistanceData, 0, sizeof(cTFTMeasureDistanceData));
		memset(cTFTMeasureAngleData, 0, sizeof(cTFTMeasureAngleData));
		memset(cCFMeasureFail, 0, sizeof(cCFMeasureFail));
		memset(cTFTMeasureFail, 0, sizeof(cTFTMeasureFail));
		//Monitor
		dCF_X1 = 0;	dCF_Y1 = 0;	dCF_X2 = 0;	dCF_Y2 = 0;
		dCF_X3 = 0;	dCF_Y3 = 0;	dCF_X4 = 0;	dCF_Y4 = 0;
		dTFT_X1 = 0;	dTFT_Y1 = 0;	dTFT_X2 = 0;	dTFT_Y2 = 0;
		dTFT_X3 = 0;	dTFT_Y3 = 0;	dTFT_X4 = 0;	dTFT_Y4 = 0;
		dCF_L1 = 0;		dCF_L2 = 0;		dCF_W1 = 0;		dCF_W2 = 0;
		dTFT_L3 = 0;	dTFT_L4 = 0;	dTFT_W3 = 0;	dTFT_W4 = 0;
		//
	}
};

struct STRU_ALIGN_INFO
{	
	TCHAR	cPanelID[20];
	BOOL	bAlignComplete;
	BOOL	bAlignResult;	//Align Camera 1,2,3,4 Result
	LONGLONG llStartTime;

	STRU_ALIGN_INFO()
	{		
		bAlignComplete = FALSE;		bAlignResult = FALSE;
		llStartTime = 0;		memset(cPanelID, 0, sizeof(cPanelID));	
	}

	void init_info()
	{
		bAlignComplete = FALSE;		bAlignResult = FALSE;
		llStartTime = 0;		memset(cPanelID, 0, sizeof(cPanelID));
	}
};

//2015.07.20
struct STRU_JUDGE_COUNT
{	
	//TCHAR	szJudgeName[20];
	BOOL    bUseMode;
	int		nJudgeCount[MAX_DEFECT_KIND];
	int		nJudgeLuminance[MAX_DEFECT_KIND];
	DOUBLE	nJudgeDistanceD1[MAX_DEFECT_KIND];
	DOUBLE	nJudgeDistanceD2[MAX_DEFECT_KIND];
	DOUBLE	nJudgeLength[MAX_DEFECT_KIND];
	DOUBLE	nJudgeThinknessW1[MAX_DEFECT_KIND];
	DOUBLE	nJudgeThinknessW2[MAX_DEFECT_KIND];

	STRU_JUDGE_COUNT()
	{		
		//memset(szJudgeName, 0, sizeof(szJudgeName));
		bUseMode = FALSE;
	}
};

//2015.08.15
struct STRU_PANEL_JUDGE_INFO
{
	BOOL bJudgeGradeA;
	int nJudgeCount;
	BOOL bIsCF;
	BOOL bIsTFT;
	//add cf/tft	//2015.08.26
	STRU_PANEL_JUDGE_INFO()
	{
		bJudgeGradeA = TRUE;
		nJudgeCount = 0;
		bIsCF = FALSE;
		bIsTFT = FALSE;
	}

	void init_info()
	{
		bJudgeGradeA = TRUE;
		nJudgeCount = 0;
		bIsCF = FALSE;
		bIsTFT = FALSE;
	}

};

//2015.10.06
struct STRU_MEASURE_DATA
{
	int nDistance_X1;
	int nDistance_Y1;
	int nDistance_X2;
	int nDistance_Y2;
	int nDistance_X3;
	int nDistance_Y3;
	int nDistance_E1;
	int nDistance_E2;

	int nDistance_X4;
	int nDistance_Y4;
	int nDistance_X5;
	int nDistance_Y5;
	int nDistance_X6;
	int nDistance_Y6;
	int nDistance_E3;
	int nDistance_E4;

	int nDistance_L1;
	int nDistance_L2;
	int nDistance_W1;
	int nDistance_W2;

	double dAngle_T1;
	double dAngle_T2;
	double dAngle_T3;
	double dAngle_T4;

	int nDistance_L3;
	int nDistance_L4;
	int nDistance_W3;
	int nDistance_W4;

	double dAngle_T5;
	double dAngle_T6;
	double dAngle_T7;
	double dAngle_T8;

	STRU_MEASURE_DATA()
	{
		nDistance_X1 = 0;		nDistance_Y1 = 0;
		nDistance_X2 = 0;		nDistance_Y2 = 0;
		nDistance_X3 = 0;		nDistance_Y3 = 0;
		nDistance_E1 = 0;		nDistance_E2 = 0;

		nDistance_X4 = 0;		nDistance_Y4 = 0;
		nDistance_X5 = 0;		nDistance_Y5 = 0;
		nDistance_X6 = 0;		nDistance_Y6 = 0;
		nDistance_E3 = 0;		nDistance_E4 = 0;

		nDistance_L1 = 0;		nDistance_L2 = 0;
		nDistance_W1 = 0;		nDistance_W2 = 0;

		dAngle_T1 = 0;			dAngle_T2 = 0;
		dAngle_T3 = 0;			dAngle_T4 = 0;

		nDistance_L3 = 0;		nDistance_L4 = 0;
		nDistance_W3 = 0;		nDistance_W4 = 0;

		dAngle_T5 = 0;			dAngle_T6 = 0;
		dAngle_T7 = 0;			dAngle_T8 = 0;
	}
};

//2015.11.05
struct STRU_RECIPE_LIST_INFO
{
	int	 nRecipeListNo;
	int	 nRecipeDefMapType;		//2016.03.14 Mr.Chu			Defect Map 과 Recipe Change 연동
	int  nRecipeMajorAxisType;	//2016.04.14 Mr.Chu			Defect Recipe Major Type Setting   false : X가 장축, true : Y가 장축
	int  nRecipeDefMapCoordQms;
// 	TCHAR strRecipeDefeMapSizeX[128];
// 	TCHAR strRecipeDefeMapSizeY[128]; 
	CString strRecipeDefeMapSizeX;
	CString strRecipeDefeMapSizeY; 
	TCHAR strRecipePLC[128];
	TCHAR strRecipeComment[128];
	TCHAR strRecipePC[MAX_VISION_PC][128];
	CString strRecipAlignPcNo;		//2021.07.07 add by mby

	STRU_RECIPE_LIST_INFO()
	{
		nRecipeListNo = 0;
		memset(strRecipePLC, 0X00,sizeof(strRecipePLC));
		memset(strRecipeComment, 0X00,sizeof(strRecipeComment));
		memset(strRecipePC, 0X00, sizeof(strRecipePC));
	}

	void init_info()
	{
		nRecipeListNo = 0;
		memset(strRecipePLC, 0X00,sizeof(strRecipePLC));
		memset(strRecipeComment, 0X00,sizeof(strRecipeComment));
		memset(strRecipePC, 0X00, sizeof(strRecipePC));
	}

};

struct STRU_RECIPE_INFO
{
	int nRecipeListNo;
	CString strRecipePLC;
	TCHAR strRecipePC[MAX_VISION_PC];

	STRU_RECIPE_INFO()
	{
		nRecipeListNo = 0;
		strRecipePLC = _T("");
		memset(strRecipePC, 0X00, sizeof(strRecipePC));
	}

	void init_info()
	{
		nRecipeListNo = 0;
		strRecipePLC = _T("");
		memset(strRecipePC, 0X00, sizeof(strRecipePC));
	}

};

struct STRU_IMAGE_NO_INF
{
	int nKhCntVal;
	int nKsCntVal;
	int nKvCntVal;
	//int nTotalCnt;
	void init()
	{
		nKhCntVal = 0;
		nKsCntVal = 0;
		nKvCntVal = 0;
		//nTotalCnt = 0;
	}

	int GetTotalCnt()
	{
		return nKhCntVal + nKsCntVal + nKvCntVal;
	}
};

struct	STRU_IMAGE_INFO
{
	TCHAR tImgPnlID[40];
	//TCHAR tGlassID[40];	//2017.06.29	Qms Img 경로 내 GlassID 추가
	TCHAR tImgPath[1024];
	TCHAR tImgName[128];
	int	  nDefType;
	int	  nDefFlag;	//2018.01.22
	STRU_IMAGE_NO_INF   nQmsImgNoCnt;
	void init()
	{
		memset(tImgPnlID, 0X00, sizeof(tImgPnlID));
		memset(tImgPath, 0X00, sizeof(tImgPath));
		memset(tImgName, 0X00, sizeof(tImgName));
		nDefType = 0;
		nDefFlag = 0;
	}
};

//2016.03.02
struct STRU_OVERLAP_LUMINANCE_INFO
{	
	BOOL    bUseMode;
	int		nLuminanceAlarmSetCount;
	int		nLuminanceAlarmSetPanelCount;
	int		nLuminanceLimitData[MAX_LUMINANCE_DATA];
	int		nLuminanceLimitDataTemp[MAX_LUMINANCE_DATA];
	int		nLuminanceRange;
	BOOL	bUseTdiMin;
	
	STRU_OVERLAP_LUMINANCE_INFO()
	{		
		bUseMode = FALSE;
		nLuminanceAlarmSetCount = 0;
		nLuminanceAlarmSetPanelCount = 0;
		memset(nLuminanceLimitData, 0, sizeof(nLuminanceLimitData));			
	}
};

struct STRU_LARGE_RECEIVE_LUMINANCE_DATA
{	
	TCHAR	cPanelID[20];
	int		nLuminanceLimitData[MAX_LUMINANCE_DATA + 1];
	LONGLONG llStartTime;

	STRU_LARGE_RECEIVE_LUMINANCE_DATA()
	{		
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(nLuminanceLimitData, 0, sizeof(nLuminanceLimitData));
		llStartTime = 0;
	}

	void init_info()
	{
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(nLuminanceLimitData, 0, sizeof(nLuminanceLimitData));
		llStartTime = 0;
	}
};

struct STRU_DATE
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nWriteTerm;
	void init()
	{
		nYear = 0;	nMonth = 0;	 nDay = 0;  nHour = 0;  nWriteTerm = 0;
	}
};

struct STRU_REPORT_INFO
{
	STRU_DATE stDataTime;
	//STRU_DATE stEndTime;
	TCHAR tDataTime[MAX_PATH];
	//TCHAR tEndtTime[MAX_PATH];
	TCHAR tDataPath[MAX_PATH];
	int	  nPanelCnt;

	void init()
	{
		stDataTime.init();
		//stEndTime.init();
		memset(tDataTime, 0X00, sizeof(tDataTime));
		//memset(tEndtTime , 0X00, sizeof(tEndtTime));
		memset(tDataPath , 0X00, sizeof(tDataPath));
		nPanelCnt = 0;
	}
};

struct STRU_REPORT_PANEL_DATA
{
	TCHAR* tPnlData[MAX_PANEL_DATA];
	void init()	
	{
		
		memset(tPnlData, 0X00, sizeof(tPnlData));

	}

	void Delete()
	{
		for (int i = 0 ; i < MAX_PANEL_DATA; i ++)
		{
			delete[] tPnlData[i];
		}
	}
};

//2016.09.14	Mr.Chu
struct STRU_PANEL_CODE_TEST
{
	BOOL bTestActive;
	BOOL bJudgeInfo;
	int	 nDatCnt;
	CTime tTimmer;

	int nTotalPanel;
	int nJudgeCode;
	int nTimeLimit;
	int nFirstDat;
	int nFourthDat;
	int nSecondDat;
	int nThirdDat;

	int nFirstDatJudge;
	int nSecondDatJudge;
	int nThirdDatJudge;
	int nFourthDatJudge;

	void init()
	{
		bTestActive = FALSE;
		bJudgeInfo	= FALSE;
		nDatCnt		= NULL;

		nTotalPanel = NULL;
		nJudgeCode  = NULL;
		nTimeLimit	= NULL;
		nFirstDat   = NULL;
		nFourthDat	= NULL;
		nSecondDat	= NULL;
		nThirdDat	= NULL;
		
		nFirstDatJudge	= NULL;
		nSecondDatJudge	= NULL;
		nThirdDatJudge	= NULL;
		nFourthDatJudge	= NULL;
	}
};

//2017.03.21
struct STRU_POL_MARK_INFO
{	
	TCHAR	cPanelID[20];
	TCHAR	cCFResult[5];	//PC3		//2018.03.14 PC2 -> PC3
	TCHAR	cTFTResult[5];	//PC6		//2018.03.14 PC5 -> PC6
	LONGLONG llStartTime;

	STRU_POL_MARK_INFO()
	{		
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(cCFResult, 0, sizeof(cCFResult));
		memset(cTFTResult, 0, sizeof(cTFTResult));
		llStartTime = 0;
	}

	void init_info()
	{
		memset(cPanelID, 0, sizeof(cPanelID));
		memset(cCFResult, 0, sizeof(cCFResult));
		memset(cTFTResult, 0, sizeof(cTFTResult));
		llStartTime = 0;
	}
};

struct STRU_MEASURE_LIMIT_INFO
{	
	//Limit Value
	int		nCFValue[16];
	int		nTFValue[16];
	//Range Standard Value
	int		nCFRStValue[8];
	int		nTFRStValue[8];
	//Range Value
	int		nRValue;
	//Panel Size
	int		nPanelValue;

	CString strInch;	

	STRU_MEASURE_LIMIT_INFO()
	{		
		memset(nCFValue, 0, sizeof(nCFValue));
		memset(nTFValue, 0, sizeof(nTFValue));
		memset(nCFRStValue, 0, sizeof(nCFRStValue));
		memset(nTFRStValue, 0, sizeof(nTFRStValue));
		nRValue = 0;
		nPanelValue = 0;
		strInch = _T("");
	}

	void init_info()
	{
		memset(nCFValue, 0, sizeof(nCFValue));
		memset(nTFValue, 0, sizeof(nTFValue));
		memset(nCFRStValue, 0, sizeof(nCFRStValue));
		memset(nTFRStValue, 0, sizeof(nTFRStValue));
		nRValue = 0;
		nPanelValue = 0;
		strInch = _T("");
	}
};
#pragma pack()

#endif
