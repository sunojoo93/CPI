
#pragma once

#define VS_PLC_TASK_INI_FILE			_T("D:\\CruxServer\\Source\\VisualStation\\Data\\VSPLC\\VSPLC.ini")


#ifndef TSTRING
#define TSTRING		basic_string<TCHAR> 
#endif


#if !defined(AFX_DEFINEVAR_H_INCLUDED_)
#define AFX_DEFINEVAR_H_INCLUDED_

#define AFXBEGINTHREAD(pfnThreadProc,pParam) AfxBeginThread(pfnThreadProc, pParam, THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED,NULL)

#define E_DEFECT_NAME_LIST						_T("DEFECT LIST	")				
#define E_DEFECT_BYPASS_CHECK					_T("BYPASS ABLE	")	

#define E_DEFECT_NAME_DUST						_T("DUST	")					// 00. 먼지 ( 확실한 먼지 제거 후
#define E_DEFECT_NAME_EDGE_CF_BROKEN			_T("CF EDGE BROKEN  	")	  // 01. CF 이지러짐 / 깨짐  ( 카케와레 )  
#define E_DEFECT_NAME_EDGE_TFT_BROKEN			_T("TFT EDGE BROKEN  	")	  // 05. TFT 이지러짐 / 깨짐 ( 카케와레 )  
#define E_DEFECT_NAME_EDGE_CF_CORNER			_T("CF EDGE CORNER")	  // 09. CF Edge 코너 LT지점
#define E_DEFECT_NAME_EDGE_TFT_CORNER			_T("TFT EDGE CORNER  	")	  // 13. TFT Edge 코너 LT지점
#define E_DEFECT_NAME_EDGE_CF_BURR				_T("CF EDGE BURR  	")	  // 17. CF 바리  
#define E_DEFECT_NAME_EDGE_TFT_BURR				_T("TFT EDGE BURR  	")	  // 21. TFT 바리  
#define E_DEFECT_NAME_CF_MEASURE				_T("CF MEASURE	")			// 25. CF 측정 불량
#define E_DEFECT_NAME_TFT_MEASURE				_T("TFT MEASURE	")			// 26. TFT 측정 불량
#define E_DEFECT_NAME_PAD_SCRATCH				_T("PAD SCRATCH	")			// 27. Pad 단자 상처 ( 기스 )
#define E_DEFECT_NAME_PAD_BURNT					_T("PAD BURNT	")			// 28. Pad 단자 탐 / 단자 변색
#define E_DEFECT_NAME_CF_SCRATCH				_T("CF SCRATCH	")	  // 29. 기판 기스 ( 흑색 )
#define E_DEFECT_NAME_TFT_SCRATCH				_T("TFT SCRATCH	")	  // 30. 기판 기스 ( 흰색 )
#define E_DEFECT_NAME_CF_ACTIVE_BROKEN				_T("CF ACTIVE BROKEN	")	  // 31. Active 깨짐
#define E_DEFECT_NAME_TFT_ACTIVE_BROKEN				_T("TFT ACTIVE BROKEN	")	  // 31. Active 깨짐
#define E_DEFECT_NAME_TFT_WHITE_TURBIDITY			_T("TFT WHITE TURBIDITY	")	  // 32. 백탁
#define E_DEFECT_NAME_CF_WHITE_TURBIDITY			_T("CF WHITE TURBIDITY	")	  // 32. 백탁
#define E_DEFECT_NAME_CF_ITO					_T("CF ITO	")			   // 33. ITO 벗겨짐 ( CF만 존재 )
#define E_DEFECT_NAME_CF_DIMPLE_POINT				_T("CF DIMPLE POINT	")			// 34. DIMPLE 점형
#define E_DEFECT_NAME_TFT_DIMPLE_POINT				_T("TFT DIMPLE POINT	")			// 34. DIMPLE 점형
#define E_DEFECT_NAME_CF_DIMPLE_LINE				_T("CF DIMPLE LINE	")			// 35. DIMPLE 선형 - 기스모양 딤플 (표면이 들어간 곳)
#define E_DEFECT_NAME_TFT_DIMPLE_LINE				_T("TFT DIMPLE LINE	")			// 35. DIMPLE 선형 - 기스모양 딤플 (표면이 들어간 곳)
#define E_DEFECT_NAME_CF_ETCHING_LINE				_T("CF ETCHING LINE	")			// 36. 엣칭 선형 불량 ( PIMPLE )
#define E_DEFECT_NAME_TFT_ETCHING_LINE				_T("TFT ETCHING LINE	")			// 36. 엣칭 선형 불량 ( PIMPLE )
#define E_DEFECT_NAME_CF_PIMPLE_POINT				_T("CF_PIMPLE POINT	")			// 37. PIMPLE 점형
#define E_DEFECT_NAME_TFT_PIMPLE_POINT				_T("TFT_PIMPLE POINT	")			// 37. PIMPLE 점형
#define E_DEFECT_NAME_PIMPLE_CIRCLE				_T("PIMPLE CIRCLE	")	  // 38. PIMPLE 원형
#define E_DEFECT_NAME_CF_PIMPLE_LINE				_T("CF PIMPLE LINE	")		    // 39. PIMPLE 선형
#define E_DEFECT_NAME_TFT_PIMPLE_LINE				_T("TFT PIMPLE LINE	")		    // 39. PIMPLE 선형
#define E_DEFECT_NAME_OTHER_POINT				_T("OTHER POINT	")		    // 40. 기타 (그밖에 다른) 점형
#define E_DEFECT_NAME_OTHER_LINE				_T("OTHER LINE	")		    // 41. 기타 (그밖에 다른) 선형
#define E_DEFECT_NAME_DIRT						_T("DIRT	")					// 42. 때 ( 기름
#define E_DEFECT_NAME_CF_POL_BUBBLE				_T("CF POL BUBBLE	")	// 43. 보호 필름 기포
#define E_DEFECT_NAME_TFT_POL_BUBBLE				_T("TFT POL BUBBLE	")	// 43. 보호 필름 기포
#define E_DEFECT_NAME_CF_POL_SCRATCH				_T("CF POL SCRATCH	")	// 47. 편광판 기스
#define E_DEFECT_NAME_TFT_POL_SCRATCH				_T("TFT POL SCRATCH	")	// 47. 편광판 기스
#define E_DEFECT_NAME_POL_BROKEN				_T("POL BROKEN	")	// 48. 편광판 깨짐
#define E_DEFECT_NAME_TDI_NO_POL				_T("TDI NO POL	")	// 49. POL 미부착
#define E_DEFECT_NAME_TDI_GAP_POL				_T("TDI GAP POL	")	// 50. POL 필름 뜸(틈새)
#define E_DEFECT_NAME_TDI_MURA					_T("TDI MURA	")	// 51. Mura
#define E_DEFECT_NAME_TDI_INNER					_T("TDI INNER	")	// 52. 내면 불량
#define E_DEFECT_NAME_TDI_MINOR					_T("TDI MINOR	")	// 53. 미소 결함
#define E_DEFECT_NAME_TDI_SUBSTANCE				_T("POL SUBSTANCE	")	// 54. 편광판 이물
#define E_DEFECT_NAME_CONTACT					_T("CONTACT	")	// 54. 편광판 이물
#define E_DEFECT_NAME_POL_MISS					_T("POL MISS	")	//2016.02.24
#define E_DEFECT_NAME_ALIGN_FAIL				_T("ALIGN FAIL	")	//2016.05.11

#define E_DEFECT_NAME_CFC_LUMINANCE				_T("CFC LUMINANCE	")	// 49. POL 미부착
#define E_DEFECT_NAME_CBC_LUMINANCE				_T("CBC LUMINANCE	")	// 50. POL 필름 뜸(틈새)
#define E_DEFECT_NAME_CFS_LUMINANCE				_T("CFS LUMINANCE	")	// 51. Mura
#define E_DEFECT_NAME_CBS_LUMINANCE				_T("CBS LUMINANCE	")	// 52. 내면 불량
#define E_DEFECT_NAME_TFC_LUMINANCE				_T("TFC LUMINANCE	")	// 53. 미소 결함
#define E_DEFECT_NAME_TBC_LUMINANCE				_T("TBC LUMINANCE	")	// 54. 편광판 이물
#define E_DEFECT_NAME_TFS_LUMINANCE				_T("TFS LUMINANCE	")	// 54. 편광판 이물
#define E_DEFECT_NAME_TBS_LUMINANCE				_T("TBS LUMINANCE	")	//2016.02.24
#define E_DEFECT_NAME_TDI_LUMINANCE				_T("TDI LUMINANCE	")	//2016.05.11

#define E_DEFECT_NAME_CF_PROTECTION				_T("CF PROTECTION	")	//2017.04.12
#define E_DEFECT_NAME_TFT_PROTECTION			_T("TFT PROTECTION	")	//2017.04.12
#define E_DEFECT_NAME_PANEL_CODE_TEST			_T("PANEL CODE TEST	")	//2017.04.12

#define E_DEFECT_NAME_BUBBLB_STATION1			_T("BUBBLE STATION 1")	//2019.05.22
#define E_DEFECT_NAME_BUBBLB_STATION2			_T("BUBBLE STATION 2")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION3			_T("BUBBLE STATION 3")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION4			_T("BUBBLE STATION 4")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION5			_T("BUBBLE STATION 5")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION6			_T("BUBBLE STATION 6")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION7			_T("BUBBLE STATION 7")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION8			_T("BUBBLE STATION 8")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION9			_T("BUBBLE STATION 9")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION10			_T("BUBBLE STATION 10")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION11			_T("BUBBLE STATION 11")	//    .
#define E_DEFECT_NAME_BUBBLB_STATION12			_T("BUBBLE STATION 12")	//2019.05.22
#define PLC_ADDR(x) #x

enum 
{
	GRID_STATS = 0,
//	GRID_DEFECT_SUMMARY,
	GRID_DEFECT_FILE_LIST,	// 2015.01.12 ljk
	GRID_DEFECT_TOTAL,
	GRID_DEFECT_1,
	GRID_DEFECT_2,
	GRID_DEFECT_3,
	GRID_DEFECT_STATS_MAX
};
enum
{
	FORWARD_CAM0_Panel_1,
	FORWARD_CAM0_Panel_2,
	FORWARD_CAM1_Panel_1,
	FORWARD_CAM1_Panel_2,
	BACKWARD_CAM0_Panel_1,
	BACKWARD_CAM0_Panel_2,
	BACKWARD_CAM1_Panel_1,
	BACKWARD_CAM1_Panel_2,
	MAX_INSPECTION_SECTION
};
enum
{
	VISION_STAGE_1,
	VISION_STAGE_2,
	VISION_STAGE_3,
	VISION_STAGE_MAX,
};

enum {
	CFSCRATCH			= 3393,
	TFTSCRATCH			= 3394,
	TFTWHITEBLUR		= 3380,
	CFWHITEBLUR			= 3381,
	ITOPEEL				= 3382,
	WANESBROKEN			= 3383,
	SOCKETSCRACH		= 3384,
	TFTDIMPLEPOINT		= 3385,
	CFDIMPLEPOINT		= 3386,	
	TFTEDGEILEGAL		= 3387,
	CFEDGEILEGAL		= 3388,	
	TFTDIMPLELINE		= 3389,
	CFDIMPLELINE		= 3390,
	SOCKETBURN			= 3391,
	BURR				= 3392,
	CFMEASUREFAIL		= 3395,
	TFTMEASUREFAIL		= 3396,
	TFTPOLBUBBLE		= 6190,
	CFPOLMEASUREFAIL	= 6092,
	TFTPOLMEASUREFAIL	= 6192,
	CFPOLBUBBLE			= 6090,
	TFTPOLSCRATCH		= 6191,
	CFPOLSCRATCH		= 6091,
	MURA				= 3791,
	POLSUBSTANCE		= 6290,
	CONTACTFAIL			= 3792,
	POLMINOR			= 3280,
	CFBURR				= 3374,	//2015.07.13
	TFTBURR				= 3375,
	CFBROKEN			= 3376,
	TFTBROKEN			= 3377,
	CFCORNERBROKEN		= 3378,
	TFTCORNERBROKEN		= 3379,
	CFCELLBROKEN		= 3372,	//2015.08.26
	TFTCELLBROKEN		= 3373,
	ITORESISTILEGAL		= 3782,
	CFPIMPLEPOINT		= 3364,
	TFTPIMPLEPOINT		= 3365,
	CFPIMPLELINE		= 3366,
	TFTPIMPLELINE		= 3367,
	CFPIMPLECIRCLE		= 3368,
	CFWAVEPATTERN	    = 3369,
	ETCLINE				= 3370,
	ETCPOINT			= 3371,
	POLMISS				= 6110,
	ALIGNFAIL			= 3397,	//2016,05,11 Mr.Chu		 Align Fail Defect Cord 변경할 것
	//2016.11.19		Mr.Chu		휘도값 추가
	CFCLUMINANCE		= 3355,
	CBCLUMINANCE		= 3356,
	CFSLUMINANCE		= 3357,
	CBSLUMINANCE		= 3358,
	TFCLUMINANCE		= 3359,
	TBCLUMINANCE		= 3360,
	TFSLUMINANCE		= 3361,
	TBSLUMINANCE		= 3362,
	TDILUMINANCE		= 3363,
	//2017.04.12
	CFPROTECTION		= 6093,
	TFTPROTECTION		= 6193
	//
};

enum {
	INSP_DUST				= 0 ,
	INSP_WHITE_TURBIDITY	    ,
	INSP_ITO					,
	INSP_EDGE_BROKEN			,
	INSP_PAD_SCRATCH			,
	INSP_DIMPLE_POINT			,
	INSP_ETCHING_LINE			,
	INSP_DIMPLE_LINE			,
	INSP_PAD_BURNT				,	
	INSP_BURR					,
	INSP_POL_BUBBLE				,	
	INSP_POL_SCRATCH			,
	INSP_POL_ALIEN_SUBSTANCE    ,
	INSP_POL_MINOR 				,
	INSP_BLACK_SCRATCH			,
	INSP_WHITE_SCRATCH			,
	INSP_CF_MEASURE				,
	INSP_TFT_MEASURE			,
	INSP_MURA					,
	INSP_NO_USE					,
	INSP_CF_BURR				,//2015.07.13
	INSP_TFT_BURR				,
	INSP_CF_BROKEN				,
	INSP_TFT_BROKEN				,
	INSP_CF_CORNER_BROKEN		,
	INSP_TFT_CORNER_BROKEN		,
	INSP_CF_CELL_BROKEN			,//2015.08.26
	INSP_TFT_CELL_BROKEN		,
	INSP_PIMPLE_POINT			,
	//INSP_TFT_PIMPLE_POINT		,
	INSP_PIMPLE_LINE			,
	//INSP_TFT_PIMPLE_LINE		,
	INSP_CF_PIMPLE_CIRCLE		,
	INSP_CF_WAVE_PATTERN		, 
	INSP_ETC_LINE				,
	INSP_ETC_POINT				,
	INSP_CONTACT				,
	INSP_POL_MISS				,	//2016.02.24
	INSP_ALIGN_FAIL				,	//2016.05.11
	//2016.11.19 Mr.Chu		Luminance
	INSP_CFC_LUMINANCE			,
	INSP_CBC_LUMINANCE			,
	INSP_CFS_LUMINANCE			,
	INSP_CBS_LUMINANCE			,
	INSP_TFC_LUMINANCE			,
	INSP_TBC_LUMINANCE			,
	INSP_TFS_LUMINANCE			,
	INSP_TBS_LUMINANCE			,
	INSP_TDI_LUMINANCE			,
	INSP_CF_PROTECTION			,
	INSP_TFT_PROTECTION			,
	INSP_TYPE_MAX
};

enum {	PANEL_ONE,	PANEL_TWO,	MAX_PANEL};

enum {	GRADE_ONE,	GRADE_TWO,	MAX_GRADE};

enum {	JUDGE_ONE,	JUDGE_TWO,	MAX_JUDGE};

enum
{  
	LAYER_NO =1,						///		AA				0       0		고정 "30"
	DEFECT_NO,						///	   999				O		O		0 ~ 999
	DEFECT_CODE,					///	 AAAAA				O		O		K+4桁（ex：KSoRh）
	FLAG,							///	   	 9				O		O		0 : CF, 1 : TFT, 2 : POL　
	PIXEL_X,						///	 99999				O		O		Image 좌표 X
	PIXEL_Y,						///  99999				O		O		Image 좌표 Y
	GATE_START_NO,					///  99999				X		O		Gate Line 의 결함 시작 위치
	DATA_START_NO,					///  99999				X		O		Data Line 의 결함 시작 위치
	GATE_END_NO,					///  99999				X		O		Gate Line 의 결함 끝 위치
	DATA_END_NO,					///  99999				X		O		Data Line 의 결함 끝 위치
	COORD_START_X,					///±9999.999	um		O		O		결함 시작 위치 X
	COORD_START_Y,					///±9999.999	um		O		O		결함 시작 위치 Y
	COORD_END_X,					///±9999.999	um		O		O		결함 끝 위치 X
	COORD_END_Y,					///±9999.999	um		O		O		결함 끝 위치 Y
	DEFECT_SIZE,					///	   999		um		O		O		결함강도
	DEFECT_IMG_NO,					///9-12-9.AAA			O		O		결함 이미지 파일 이름
	IMG_SIZE_X,						///  9999.99	um		O		O		결함 이미지 크기 X
	IMG_SIZE_Y,						///  9999.99	um		O		O		결함 이미지 크기 Y
	DEFECT_TYPE,					///		99				O		O		결함 종류
	CAMERA_NO,
	LUMINANCE,						//2015.08.15
	IMG_PATH
};

enum {
	D_FLAG_CF_TFT_POL,	D_START_X,	D_START_Y,	D_END_X,	D_END_Y,
	D_TYPE			 ,	D_COUNT  ,	D_UPDATE
};

enum {
	DEF_MERGE_NG, DEF_MERGE_NO_DAT_FIR, DEF_MERGE_NO_DAT_SEC, DEF_MERGE_OK
};

//2016.03.02
enum {
	CF_FORWARD_SIDE		= 0 ,
	CF_FORWARD_COAXIAL	    ,
	CF_BACKWARD_SIDE		,
	CF_BACKWARD_COAXIAL		,
	TFT_FORWARD_SIDE		,
	TFT_FORWARD_COAXIAL		,
	TFT_BACKWARD_SIDE		,
	TFT_BACKWARD_COAXIAL	,
	POL1					,
	POL2					,
	POL3					
};

enum {
	CF_MEASURE_DST		= 1 ,
	CF_MEASURE_ANG	    ,
	TFT_MEASURE_DST		,
	TFT_MEASURE_ANG					
};

#define PLC_HANDLE_TYPE					1
#define EQP_TYPE_ACC					0
#define EQP_TYPE_ACS					1
#define EQP_TYPE_AVL					0
#define EQP_TYPE_ABI					1
#define EQP_SMALL_SIZE					1
#define EQP_MEDIUM_SIZE					0
#define EQP_LARGE_SIZE					2
#define EQP_MONITOR_SIZE				3

#define COLOR_ENABLE					RGB(255, 255, 224)
#define COLOR_DISABLE					RGB(160, 160, 160)
#define COLOR_MAP_EDGE					RGB(128, 128, 128)
#define COLOR_MAP_WHITE					RGB(255, 255, 255)
#define COLOR_MAP_RED					RGB(255, 0, 0)
#define COLOR_MAP_GREEN					RGB(0, 255, 0)
#define COLOR_MAP_BLUE					RGB(0, 0, 255)


#define QMS_IMAG_TEMP					_T("D:\\QMS_IMG_TMP\\%s\\")
#define JUDGE_A							_T("A")
#define JUDGE_B							_T("B")
#define JUDGE_C							_T("C")
#define JUDGE_D							_T("D")
#define JUDGE_FD_1						_T("FD1")

//2015.09.17
#define NG_FLAG_A_GRADE							0
#define NG_FLAG_BYPASS							2
#define NG_FLAG_ALARM							3
#define NG_FLAG_INSPECT							4
#define NG_FLAG_AVL_B_GRADE						5
#define NG_FLAG_ABI_B_GRADE						1
#define NG_FLAG_C_GRADE							6
#define NG_FLAG_D_GRADE							7
//2015.09.17

#define SECTION_IP						_T("IP")
#define SECTION_PORT					_T("PORT")
#define SECTION_PARAM					_T("PARAM")
#define SECTION_OVERLAP					_T("OVERLAP_COORD_INFO")
#define SECTION_OVERLAP_DEFTYPE			_T("OVERLAP_DEF_TYPE")
#define SECTION_TDI_OVERLAP				_T("TDI_OVERLAP_COORD_INFO")
#define SECTION_OVERLAP_LUMINANCE		_T("OVERLAP_LUMINANCE_INFO")	//2016.03.02

#define SECTION_MEASURE_SPEC			_T("MONITOR_MEASURE_SPEC")		//2017.04.29	모니터 측정 스펙 설정
//2015.02.03 add by ikm - S
#define SECTION_FTP						_T("FTP_")
#define FTP_LOCAL_STATE					_T("FTP_LOCAL_STATE")
#define FTP_LOG_STATE					_T("FTP_LOG_STATE")
#define FTP_CONDITION					_T("FTP_CONDITION")
#define EQP_STATE						_T("EQP_STATE")
#define PNL_DATA_PATH					_T("DATA_PATH")
#define PNL_COUNT_SET					_T("COUNT")
#define PNL_DEF_INFO					_T("DEFECT_INFO")
//2015.02.03 add by ikm - E
#define DEF_JUDGE_COUNT					_T("JUDGE_COUNT")	//2015.07.20
#define DEF_JUDGE_LUMINANCE				_T("JUDGE_LUMINANCE")	//2015.07.20
#define DEF_JUDGE_DISTANCE_D1				_T("JUDGE_DISTANCE_D1")	//2015.07.20
#define DEF_JUDGE_DISTANCE_D2				_T("JUDGE_DISTANCE_D2")	//2015.07.20
#define DEF_JUDGE_LENGTH				_T("JUDGE_LENGTH")	//2015.07.20
#define DEF_JUDGE_WIDTH_W1				_T("JUDGE_WIDTH_W1")	//2015.07.20
#define DEF_JUDGE_WIDTH_W2				_T("JUDGE_WIDTH_W2")	//2015.07.20
#define PNL_JUDGE_ITEM					_T("ITEM")
#define PNL_JUDGE_BYPASS_ITEM			_T("LOWER_EQP_BYPASS_INFO")		//2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정
#define PNL_JUDGE_PRIORITY				_T("PRIORITY")						//2016.05.28		Defect의 Judge 판단 유무 설정
//2015.11.05
#define RECIPE_INFO						_T("RECIPE_INFO")
#define RECIPE_LIST_PC_					_T("RECIPE_LIST_PC_")
#define RECIPE_COUNT					_T("RECIPE_COUNT")
#define RECIPE_							_T("RECIPE_")
#define RECIPE_COMMENT					_T("RECIPE_COMMENT")
#define PLC_MODEL						_T("PLC_MODEL")
#define RECIPE_PC_						_T("RECIPE_PC_")
#define RECIPE_NO						_T("RECIPE_NO")
//2015.11.05

#define KEY_VISION_SERVER				_T("Vision Server")
#define KEY_VISION_PC_1					_T("Vision Client #1")
#define KEY_VISION_PC_2					_T("Vision Client #2")
#define KEY_VISION_PC_3					_T("Vision Client #3")
//2015.05.12 add s-
#define KEY_VISION_PC_4					_T("Vision Client #4")
#define KEY_VISION_PC_5					_T("Vision Client #5")
#define KEY_VISION_PC_6					_T("Vision Client #6")
#define KEY_VISION_PC_7					_T("Vision Client #7")
#define KEY_VISION_PC_8					_T("Vision Client #8")
#define KEY_VISION_PC_9					_T("Vision Client #9")
//2015.05.12 add e-
#define KEY_PRE_ALIGN_PC				_T("Pre Align PC")	//2015.08.19
#define KEY_SIGNAL_FLAG					_T("Signal Flag")
#define KEY_PLC							_T("PLC")
#define KEY_CONTROL_PC					_T("Control PC")
#define KEY_CIM							_T("CIM")
#define KEY_PARAM_USE_AUTODEL			_T("Use Auto Delete")
#define KEY_PARAM_LOGDEL				_T("Log_Delete")
#define KEY_PARAM_LOG_PRIORITY			_T("Log_Priority")	 // 2016.07.27 skgim
#define KEY_PARAM_SUMMARY_BACKUP		_T("Summary_Backup")
#define KEY_PARAM_VPC_CNT				_T("Vision PC Count")
#define KEY_PARAM_STAGE_CNT				_T("STAGE COUNT")	//2015.05.13 add
#define KEY_PARAM_CTRL_MODE				_T("Control Mode")
#define KEY_PARAM_CTRL_SOCK_MODE		_T("Control Sock Mode")
#define KEY_PARAM_USE_CIM				_T("Use CIM")
#define KEY_PARAM_CIM_SOCK_MODE			_T("CIM Sock Mode")
#define KEY_PARAM_EQP_TYPE				_T("EQP Type")
#define KEY_PARAM_EQ_SIZE_TYPE			_T("EQ_SIZE_TYPE")
#define KEY_PARAM_USE_SEMAPHORE			_T("Use Semaphore")
#define KEY_PARAM_USE_PLC_MODEL_READ	_T("Use PLC Model Read")
#define KEY_PARAM_LUMINANCE_CAM_1_MIN	_T("Luminance_Cam1_Min")
#define KEY_PARAM_LUMINANCE_CAM_2_MIN	_T("Luminance_Cam2_Min")
#define KEY_PARAM_LUMINANCE_CAM_3_MIN	_T("Luminance_Cam3_Min")
#define KEY_PARAM_LUMINANCE_CAM_4_MIN	_T("Luminance_Cam4_Min")
#define KEY_PARAM_LUMINANCE_CAM_1_MAX	_T("Luminance_Cam1_Max")
#define KEY_PARAM_LUMINANCE_CAM_2_MAX	_T("Luminance_Cam2_Max")
#define KEY_PARAM_LUMINANCE_CAM_3_MAX	_T("Luminance_Cam3_Max")
#define KEY_PARAM_LUMINANCE_CAM_4_MAX	_T("Luminance_Cam4_Max")
//2015.05.11 add s-
#define KEY_PARAM_LUMINANCE_CAM_5_MIN	_T("Luminance_Cam5_Min")
#define KEY_PARAM_LUMINANCE_CAM_6_MIN	_T("Luminance_Cam6_Min")
#define KEY_PARAM_LUMINANCE_CAM_7_MIN	_T("Luminance_Cam7_Min")
#define KEY_PARAM_LUMINANCE_CAM_8_MIN	_T("Luminance_Cam8_Min")
#define KEY_PARAM_LUMINANCE_CAM_9_MIN	_T("Luminance_Cam9_Min")
#define KEY_PARAM_LUMINANCE_CAM_10_MIN	_T("Luminance_Cam10_Min")
#define KEY_PARAM_LUMINANCE_CAM_5_MAX	_T("Luminance_Cam5_Max")
#define KEY_PARAM_LUMINANCE_CAM_6_MAX	_T("Luminance_Cam6_Max")
#define KEY_PARAM_LUMINANCE_CAM_7_MAX	_T("Luminance_Cam7_Max")
#define KEY_PARAM_LUMINANCE_CAM_8_MAX	_T("Luminance_Cam8_Max")
#define KEY_PARAM_LUMINANCE_CAM_9_MAX	_T("Luminance_Cam9_Max")
#define KEY_PARAM_LUMINANCE_CAM_10_MAX	_T("Luminance_Cam10_Max")
//2015.05.11 add e-
#define KEY_OVERLAP_ALIGN_FAIL_STOP_COUNT		 	_T("AlignFailStopCount")
#define KEY_OVERLAP_ALIGN_FAIL_DEFECT_COUNT		 	_T("AlignFailDefectCount")
#define KEY_OVERLAP_USEMODE		_T("UseMode")
#define KEY_OVERLAP_ALL_OVERLAP_DEF_MODE	_T("UseAllOverlapDef")
#define KEY_OVERLAP_OFFSET_X	_T("OffsetX")
#define KEY_OVERLAP_OFFSET_Y	_T("OffsetY")
#define KEY_OVERLAP_DEFECT_CHECK	_T("DefectCheck")
#define KEY_OVERLAP_DEFECT_COUNT	_T("DefectCount")
#define KEY_OVERLAP_CONTACT_CHECK	_T("ContactCheck")
#define KEY_OVERLAP_DEFECT_TERM		_T("DefectTerm")  
#define KEY_OVERLAP_DEFECT_STOP_COUNT		_T("DefectStopCount") 

#define KEY_OVERLAP_CONTACT_SETTING_COUNT	_T("ContactSetCount")
#define KEY_OVERLAP_CONTACT_CURRENT_COUNT	_T("ContactCurCount")
#define KEY_OVERLAP_CONTACT_TERM		 	_T("ContactTerm")
#define KEY_OVERLAP_CONTACT_PANEL_SETTING_COUNT	_T("ContactPanelSetCount")
#define KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT	_T("ContactPanelCurCount")
#define KEY_OVERLAP_CONTACT_DEFECT_COUNT		_T("ContactDefectCount")
#define KEY_OVERLAP_CONTACT_STOP_COUNT		_T("ContactStopCount")

#define KEY_OVERLAP_CF_CURRENT_COUNT	_T("CFMeasureFailCurCount")
#define KEY_OVERLAP_TFT_CURRENT_COUNT	_T("TFTMeasureFailCurCount")
#define KEY_OVERLAP_CF_CURRENT_COUNT_PANEL	_T("CFMeasureFailPanelCurCount")
#define KEY_OVERLAP_TFT_CURRENT_COUNT_PANEL	_T("TFTMeasureFailPanelCurCount")

//2016.02.24
#define KEY_OVERLAP_POL_MISS_CHECK	_T("PolMissCheck")
#define KEY_OVERLAP_POL_MISS_SETTING_COUNT	_T("PolMissSetCount")
#define KEY_OVERLAP_POL_MISS_CURRENT_COUNT	_T("PolMissCurCount")
#define KEY_OVERLAP_POL_MISS_TERM		 	_T("PolMissTerm")
#define KEY_OVERLAP_POL_MISS_PANEL_SETTING_COUNT	_T("PolMissPanelSetCount")
#define KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT	_T("PolMissPanelCurCount")
#define KEY_OVERLAP_POL_MISS_DEFECT_COUNT		_T("PolMissDefectCount")
#define KEY_OVERLAP_POL_MISS_STOP_COUNT		_T("PolMissStopCount")

//2016.08.22
#define KEY_OVERLAP_ALIGN_FAIL_CHECK	_T("AlignFailCheck")
#define KEY_OVERLAP_ALIGN_FAIL_SETTING_COUNT	_T("AlignFailSetCount")
#define KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT	_T("AlignFailCurCount")
#define KEY_OVERLAP_ALIGN_FAIL_TERM		 	_T("AlignFailTerm")
#define KEY_OVERLAP_ALIGN_FAIL_PANEL_SETTING_COUNT	_T("AlignFailPanelSetCount")
#define KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT	_T("AlignFailPanelCurCount")

//2016.11.21
#define KEY_OVERLAP_LUMINANCE_CHECK	_T("LuminanceCheck")		//2016.11.22
#define KEY_OVERLAP_LUMINANCE_SETTING_COUNT	_T("LuminanceSetCount")		//2016.11.22
#define KEY_OVERLAP_LUMINANCE_CURRENT_COUNT	_T("LuminanceCurCount")
#define KEY_OVERLAP_LUMINANCE_PANEL_CURRENT_COUNT	_T("LuminancePanelSetCount")
#define KEY_OVERLAP_LUMINANCE_TERM		 	_T("LuminanceTerm")
#define KEY_OVERLAP_LUMINANCE_PANEL_SETTING_COUNT	_T("LuminancePanelSetCount")
#define KEY_OVERLAP_LUMINANCE_STOP_COUNT		 	_T("LuminanceStopCount")
#define KEY_OVERLAP_LUMINANCE_DEFECT_COUNT		 	_T("LuminanceDefectCount")

//2016.03.02
#define KEY_LUMINANCE_USEMODE				_T("UseMode")
#define KEY_LUMINANCE_SET_COUNT				_T("OverlapCount")
#define KEY_LUMINANCE_SET_COUNT_PANEL		_T("PanelCount")
#define KEY_LUMINANCE_CURRENT_COUNT			_T("OverlapCurCount")
#define KEY_LUMINANCE_CURRENT_COUNT_PANEL	_T("PanelCurCount")
#define KEY_LUMINANCE_LIMIT					_T("Limit")
#define KEY_LUMINANCE_RANGE					_T("Range")
#define KEY_LUMINANCE_USE_TDI_MIN		_T("UseTdiMin")

//2016.08.17 Error Alarm ID add by sgkim - S
#define ERROR_CHECK_REAR_NGFLAG			800000
#define ERROR_CHECK_FRONT_NGFLAG		800001
#define ERROR_VISION_NOREADY			800002
#define ERROR_VISION_GRAB_END			800003
#define ERROR_PC_ALIVE					800004
#define ERROR_READ_DEFECT_DATA			800005
#define ERROR_FTP_CONNECT				800006
#define ERROR_QMS_FILE_UPLOAD			800007
#define ERROR_QMS_IMG_UPLOAD			800008
#define ERROR_INSP_INTERLOCK			800009
#define ERROR_VISOION_SELECT_RECIPE		800010
#define ERROR_INSP_AUTO_RECIPE			800011
#define ERROR_PLC_SELECT_RECIPE			800012
//2016.08.17 Error Alarm ID sgkim - E

//2015.09.16 Mr.Chu
#define KEY_OVERLAP_PANEL_COUNT	_T("PanelCount")

//2015.02.03 add by ikm - S
#define KEY_FTP_SERVER					_T("FTP Server")
#define KEY_FTP_ID						_T("FTP ID")
#define KEY_FTP_PW						_T("FTP PW")
#define KEY_FTP_IMG_ID					_T("FTP IMG_ID")
#define KEY_FTP_IMG_PW					_T("FTP IMG_PW")
#define KEY_FTP_PORT					_T("FTP Port")
#define KEY_FTP_ROOT					_T("FTP Root")
#define KEY_FTP_DIR_DEFAULT				_T("FTP DIR_DEFAULT")
#define KEY_FTP_DIR_SENT				_T("FTP DIR_SENT")
#define KEY_FTP_DIR_UNSENT				_T("FTP DIR_UNSENT")
#define KEY_FTP_DIR_IMG_SENT			_T("FTP DIR_IMG_SENT")
#define KEY_FTP_DIR_IMG_UNSENT			_T("FTP DIR_IMG_UNSENT")
#define KEY_FTP_DIR_ERROR				_T("FTP DIR_ERR")
#define KEY_FTP_DIR_DELETE				_T("FTP DIR_DELETE") 
#define KEY_FTP_DIR_LOG					_T("FTP DIR_LOG")
#define KEY_FTP_DIR_IMG_LOG				_T("FTP DIR_IMG_LOG")
#define KEY_FTP_DIR_LOG_NAME			_T("FTP LOG_NAME")
#define KEY_FTP_DIR_PERIOD_DAY			_T("FTP LOG_DELETE")
#define LOG_WRITE_TERM					_T("FTP LOG_WRITE_TERM")
#define ANS_RENAME_CHECK_TIMEOUT		_T("ANS_RENAME_CHECK_TIMEOUT")
#define ANS_RENAME_WAIT_CNT_MAX			_T("ANS_RENAME_WAIT_CNT_MAX")
#define FTP_IP_COUNT					_T("FTP IP_COUNT")
#define KEY_EQP_TYPE					_T("EQP_TYPE")
#define KEY_SEQ_NO 						_T("SEQ_NO")
#define KEY_EQP_OBJECT					_T("EQP_OBJECT")
#define KEY_DEFINFO_ROOT				_T("DEF_INFO_ROOT")
#define KEY_DEFINFO_PATH				_T("DEF_INFO_PATH")
#define KEY_DEFINFO_PANEL_PATH			_T("DEF_INFO_PANEL_PATH")	//2015.06.22
#define KEY_DEFINFO_DEFECT_PATH			_T("DEF_INFO_DEFECT_PATH")	//2015.06.22
#define KEY_DEFINFO_MEASURE_PATH		_T("DEF_INFO_MEASURE_PATH")	//2015.10.06
#define KEY_DEFINFO_OVERLAP_DEFECT_PATH		_T("DEF_INFO_OVERLAP_DEFECT_PATH")	//2015.10.07
#define KEY_DEFINFO_QMS_BACKUP_PATH		_T("DEF_INFO_QMS_BACKUP_PATH")	//2015.10.08
#define KEY_DEF_INFO_WRITE_TERM			_T("DEF_INFO_WRITE_TERM")
#define KEY_DEF_INFO_DELETE				_T("DEF_INFO_DELETE")
#define KEY_SAVE_DEFECT_PATH_1			_T("SAVE_DEFECT_PATH_1")
#define KEY_SAVE_DEFECT_PATH_2			_T("SAVE_DEFECT_PATH_2")
#define KEY_GRADE_OK					_T("GRADE_OK")
#define KEY_GRADE_NG					_T("GRADE_NG")
#define KEY_DEFINFO_DUST				_T("DustType")
#define KEY_EQP_USE_NG_FLAG				_T("EQP_USE_NG_FLAG") //2015.10.29 add by ikm for ng flag - PACKING 
#define KEY_TEMPERATURE_CHECK_TERM		_T("Tempermature_CheckTerm") //2016.04.06 add by ikm
#define KEY_ITEM_STATS_TIME1			_T("STATS_TIME1")
#define KEY_ITEM_STATS_TIME2			_T("STATS_TIME2")
#define KEY_ITEM_STATS_TIME3			_T("STATS_TIME3")

#define KEY_STATE_TEST_MODE				_T("TEST_MODE")
#define KEY_STATE_VIS_CNT				_T("VISION PC CNT")
#define KEY_STATE_EQP_MODEL				_T("EQP_MODEL")
#define KEY_STATE_EQP_NO				_T("EQP_NO")
#define KEY_STATE_STEP_ID				_T("STEP_ID")
#define EXTENSION_DAT				    _T(".dat")
#define EXTENSION_TMP				    _T(".tmp")
#define EXTENSION_ANS				    _T(".ans")
#define EXTENSION_JPG					_T(".jpg")
#define EXT_COLON						_T(",")
#define EXT_SEMICOL						_T(";")

#define QMS_DATA_BACK					_T("sent_bak");

#define ANS_WAIT_CNT_MAX		     			6
#define ANS_WAIT_ERR_EXIST_FILE					80
#define QMS_IP_CNT_MAX							3
#define QMS_UPLOAD_CNT_MAX						1			//Originally Using 2 Qms Ip
#define QMS_DOWNLOAD_CNT_MAX					1			//Originally Using 2 Qms Ip
#define OK										1
//#define NG										0
#define CONTACT_NG								1
#define ITO_NG									2

//2015.02.03 add by ikm - E

#define SECTION_CONFIG					_T("CONFIG")
#define SECTION_PC_SIDE					_T("PC_SIDE")
#define SECTION_PLC_SIDE				_T("PLC_SIDE")
#define SECTION_CIM_SIDE				_T("CIM_SIDE")
#define KEY_ADDR_START_PC				_T("Addr Start PC")
#define KEY_ADDR_END_PC					_T("Addr End PC")
#define KEY_ADDR_START_PLC				_T("Addr Start PLC")
#define KEY_ADDR_END_PLC				_T("Addr End PLC")
#define KEY_ITEM_COUNT_PC				_T("Item Count PC")
#define KEY_ITEM_COUNT_PLC				_T("Item Count PLC")
#define KEY_ADDR_START_CIM				_T("Addr Start CIM")
#define KEY_ADDR_END_CIM				_T("Addr End CIM")
#define KEY_ITEM_COUNT_CIM				_T("Item Count CIM")
#define MEASUREFAIL_CF					_T("101")
#define MEASUREFAIL_TFT					_T("102")
#define MEASUREFAIL_DEFAULT				_T("999")

#define POLMARKILEGAL_OK					_T("Y")
#define POLMARKILEGAL_NG					_T("N")

#define POLMARKILEGAL_ABI_OK				_T("Y_0")
#define POLMARKILEGAL_ABI_NG				_T("N_0")

#define SOCK_MODE_PASSIVE				0
#define SOCK_MODE_ACTIVE				1

#define CONTROL_MODE_PLC				0
#define CONTROL_MODE_PC					1

#define STATUS_NONE						_T("None")
#define STATUS_LISTENING				_T("Listening")
#define STATUS_CONNECTED				_T("Connected")
#define STATUS_DISCONNECTED				_T("Disconnected")
#define STATUS_CLOSED					_T("Closed")

#define STATUS_TOPBAR_V1				1
#define STATUS_TOPBAR_V2				2
#define STATUS_TOPBAR_V3				3
#define STATUS_TOPBAR_V4				4
#define STATUS_TOPBAR_PLC				10	//2015.05.12 edit 5 -> 10
#define STATUS_TOPBAR_PC				6

#define STATUS_TOPBAR_QMS				12	//2015.11.20
#define STATUS_TOPBAR_ALIGN				30	//20151218 Mr.Chu

//2015.06.22
#define DEFECT_REVIEW					1
#define DEFECT_CURRENT					2

#define ROOT_PATH						_T("D:\\CruxServer")
#define CONFIG_PATH						_T("\\Config\\ConfigSet.ini")
#define CONFIG_COUNT_PATH				_T("\\Config\\CountSet.ini")
#define CONFIG_PNLCNT_INFO_PATH			_T("\\Config\\PanelCntInfo.txt")
#define PLC_MAP_PATH					_T("\\Config\\PLC_Map.ini")
#define LOG_NAME_CRUXSERVER				_T("\\LOG\\CruxServer")
#define RESULT_PATH						_T("\\Result")
#define DEFECT_DATA_PATH				_T("\\Config\\DefectCoord.dat")
#define OVERLAP_COORD_DATA_PATH				_T("\\Config\\OverlapCurCoord.ini") //2016.11.21
#define CLASSIFY_INFO_PATH				_T("\\Config\\ClsyPnlInfo.inf")
#define OVERLAP_DEF_TYPE_PATH			_T("\\Config\\OverlapDefType.inf")
#define NGFLAG_INFO_PATH				_T("\\Config\\NgFlgInfo.inf")
#define PANEL_CODE_TEST_DATA_PATH		_T("\\Config\\PanelCodeTest.inf")		//2016.09.14
#define ALIGN_MEASURE_INFO_PATH			_T("\\Config\\AlignMeasureInfo.inf")	//2015.08.19
#define RECIPE_INFO_PATH				_T("\\Config\\Recipe.ini")	//2015.11.05
#define ALIGN_INFO_PATH					_T("\\Config\\AlignInfo.inf")			//2015.08.19
#define MONITOR_MEASURE_INFO_PATH		_T("\\Config\\MonitorMeasureInfo.inf")	//2016.08.18
#define POL_MARK_INFO_PATH				_T("\\Config\\PolMarkInfo.inf")			//2017.03.21
//2015.12.10 Mr.Chu
#define JUDGE_ITEM_SET_PATH				_T("\\Config\\JudgeItemSet.ini")
#define DATA_PATH						_T("D:\\CruxServer") //_T("C:\\CruxData")
#define MODEL_LIST_PATH					_T("\\Config\\Packing\\ModelList.txt")

//2015.11.17 VS Interface S---
#define VS_CRUX_SERVER_TASK_INI_FILE	_T("\\VisualStation\\Data\\VSCruxServer.ini")

#define LUMINANCE_DATA_PATH				_T("\\Config\\LuminanceData.inf")			//2016.03.02
#define PLC_MODEL_PATH					_T("\\Config\\PlcModel\\")

#define VS_PLC_TASK						7

#define VS_PLC_SEQUENCE_SET_DEVICE							10, 1
#define VS_PLC_SEQUENCE_SET_DEVICE_DWORD					10, 2
#define VS_PLC_SEQUENCE_GET_DEVICE							10, 3
#define VS_PLC_SEQUENCE_GET_DEVICE_DWORD					10, 4
#define VS_PLC_SEQUENCE_SET_DEVICE_BLOCK					10, 5
#define VS_PLC_SEQUENCE_SET_DEVICE_BLOCK_DWORD				10, 6
#define VS_PLC_SEQUENCE_GET_DEVICE_BLOCK					10, 7
#define VS_PLC_SEQUENCE_GET_DEVICE_BLOCK_DWORD				10, 8
#define VS_PLC_SEQUENCE_SET_DEVICE_AUTOCLEAR				10, 9
#define VS_PLC_SEQUENCE_SET_DEVICE_DWORD_AUTOCLEAR			10, 10
//2015.11.17 VS Interface  E---
#define MAX_GRID_LOG					100
#define MAX_VISION_PC					11	//2015.05.12 edit 3 -> 11
#define MAX_DEFECT_KIND					36	//2015.08.27 19->28->34
#define MAX_REV_PANEL					100
#define VISION_PC_NONE					0
#define VISION_PC_1						1
#define VISION_PC_2						2
#define VISION_PC_3						3
//2015.05.12 add s-
#define VISION_PC_4						4
#define VISION_PC_5						5
#define VISION_PC_6						6
#define VISION_PC_7						7
#define VISION_PC_8						8
#define VISION_PC_9						9
//2015.05.12 add e-

#define B_PROJECT						1
#define P_PROJECT						0

//2016.08.18
#define MEASURE_CAL_INSP				0
#define MEASURE_CAL_SERVER				1

//2016.11.28
#define CAM1							0
#define CAM2							1

#define SIGNAL_PC						10

#define PRE_ALIGN_PC					30	//2015.08.19

#define STATE_IDLE						0
#define STATE_READY						1
#define STATE_GRAB						2
#define STATE_PROCESS					3
#define STATE_ERROR						4
#define STATE_INSP_START				5	//2015.07.05
#define STATE_INSP_END					6

#define SOCK_BUFF_LENGTH				8192

#define WM_SOCK_VISION_ACCEPT			2000
#define WM_SOCK_VISION_CONNECT			2001
#define WM_SOCK_VISION_RECEIVE			2002
#define WM_SOCK_VISION_DISCONNECT		2003
#define WM_SOCK_SIGNALFLAG_ACCEPT		2010
#define WM_SOCK_SIGNALFLAG_CONNECT		2011
#define WM_SOCK_SIGNALFLAG_RECEIVE		2012
#define WM_SOCK_SIGNALFLAG_DISCONNECT	2013
#define WM_SOCK_CIM_ACCEPT				2100
#define WM_SOCK_CIM_CONNECT				2101
#define WM_SOCK_CIM_RECEIVE				2102
#define WM_SOCK_CIM_DISCONNECT			2103
#define WM_SOCK_CONTROL_ACCEPT			2200
#define WM_SOCK_CONTROL_CONNECT			2201
#define WM_SOCK_CONTROL_RECEIVE			2202
#define WM_SOCK_CONTROL_DISCONNECT		2203
#define WM_UI_NEW_MSG_DIALOG			2210
#define WM_UI_NEW_MEASURE_MSG_DIALOG	2211
#define WM_UI_NEW_CONTACT_MSG_DIALOG	2212
#define WM_UI_NEW_NG_JUDGE_MSG_DIALOG	2213
#define WM_UI_NEW_POL_MARK_MSG_DIALOG	2214
#define WM_UI_NEW_DEF_TYPE_MSG_DIALOG	2215
//2016.01.29 Mr.Chu			Message windows Output
#define WM_UI_NEW_DISPLAY_MSG_DIALOG	2216
#define WM_UI_ADD_LOG					2217
#define WM_UI_NEW_LUMINANCE_MSG_DIALOG	2218	//2016.03.02
#define WM_UI_DISTROY_DIALOG            2219

#define WM_SOCK_CONTROL_ALARM			2204
#define WM_SOCK_CONTROL_MEASURE_ALARM	2205
#define WM_SOCK_CONTROL_CONTACT_ALARM   2206
#define WM_SOCK_CONTROL_LIST_ALARM		2207
#define WM_SOCK_CONTROL_DEFTYPE_ALARM	2208
#define WM_SOCK_CONTROL_LUMINANCE_ALARM	2209	//2016.03.02
#define WM_UI_CONTROL_JUDGE_DIS			3000
#define WM_UI_CONTROL_JUDGE_INFO_DIS	3001
#define WM_UI_CONTROL_DEF_TYPE_DIS		3002
#define WM_UI_CONTROL_SAV_MEASURE_DAT	3003
#define WM_UI_CONTROL_PNL_SUMMARY_LIST	4000
#define WM_UI_CONTROL_DEF_SUMMARY_LIST	4001
#define WM_UI_CONTROL_POPUP_IMAGE		4002
#define WM_UI_CONTROL_DIS_STATS_LIST	4003
#define WM_UI_CONTROL_DIS_STATS_NAME	4004
#define RET_SOCK_OK						0
#define RET_SOCK_PARAM_ERR				-1		// IP or Port 오류

#define TIMER_RETRY_CONTROL_PC			1000
#define TIMER_RETRY_CIM_PC				2000
#define TIMER_RETRY_INTERVAL			1000 * 10	// 연결 접속 재시도 10초 마다 
#define TIMER_CHECK_SUMMARY				3000
#define TIMER_CHECK_SUMMARY_INTERVAL	1000 * 60 * 60 * 24		// 하루에 한번 체크
#define TIMER_CHECK_STATUS				4000
#define TIMER_CHECK_STATUS_INTERVAL		1000 * 3
#define TIMER_UPDATE_MAP_DATA			5000
#define TIMER_UPDATE_MAP_DATA_INTERVAL	1000
#define TIMER_UPDATE_STATE_VISION		6000
#define TIMER_UPDATE_STATE_V_INTERVAL	1000
#define TIMER_UPDATE_STATE_STAGE		7000
#define TIMER_UPDATE_STATE_S_INTERVAL	3000
#define TIMER_VS_CONNETCTION			8000
#define TIMER_VS_CONNETCTION_INTERVAL	1000

#define STX								0x02
#define ETX								0x03

#define MTV_STATE_REQ					_T("MTV.STATE.REQ")
#define MTV_READY_REQ					_T("MTV.READY.REQ")
#define MTV_READY_REQ_F					_T("MTV.READY.REQ.F")
#define MTV_READY_REQ_B					_T("MTV.READY.REQ.B")
#define MTV_GRAB_START					_T("MTV.GRAB.START")
#define MTV_GRAB_END					_T("MTV.GRAB.END")
#define MTA_ALIGN_REQ					_T("MTA.ALIGN.REQ")		//2015.08.19 Align
#define MTV_RECIPE_LIST_REQ				_T("MTV.RECIPE.LIST.REQ")	//2015.11.05
#define MTV_SELECT_RECIPE				_T("MTV.SELECT.RECIPE")		//2015.11.05

#define VTM_ALIVE						_T("VTM.ALIVE")			
#define VTM_STATE						_T("VTM.STATE")			// Prm - State
#define VTM_READY_RES					_T("VTM.READY.RES")		// Prm - Ret Code
#define VTM_GRAB_END					_T("VTM.GRAB.END")		// Prm - Ret Code
#define VTM_CLASSIFY_END				_T("VTM.CLASSIFY.END")	// Prm - Judge, Code
#define VTM_INSPECT_END					_T("VTM.INSPECT.END")	// Prm - PanelID, Inspect Type
#define VTM_LUMINANCE_REPORT			_T("VTM.LUMINANCE.REP")	// 휘도 값 보고, Prm - Camera No, Value
#define PTM_ITO_RESULT					_T("PTM.ITORESULT")			// Prm - Judge (OK = 0, NG = 1)
#define PTM_RESULT						_T("PTM.RESULT")			// Prm - Judge (OK = 0, NG = 1)
#define PTM_CONTACT_RESULT				_T("PTM.CONTACTRESULT")	
#define VTM_JUDGE_END					_T("VTM.JUDGE.END")		// NG FLAG SET //2015.06.17
#define ATM_CFMEASUREDST_RES			_T("ATM.CFMEASUREDST.RES")	//2015.08.19 Align
#define ATM_TFTMEASUREDST_RES			_T("ATM.TFTMEASUREDST.RES")	//2015.08.19 Align
#define ATM_CFMEASUREANG_RES			_T("ATM.CFMEASUREANG.RES")	//2015.08.19 Align
#define VTM_TFTMEASUREDST_RES			_T("VTM.TFTMEASUREDST.RES")	//2015.08.19 Align
#define VTM_TFTMEASUREANG_RES			_T("VTM.TFTMEASUREANG.RES")	//2015.08.19 Align
#define VTM_TFTMEASURE_RES				_T("VTM.TFTMEASURE.RES")	//2015.08.19 Align
#define ATM_ALIGN_RES					_T("ATM.ALIGN.RES")		//2015.08.19 Align
#define VTM_PROCESS_INTERLOCK			_T("VTM.PROCESS.INTERLOCK")	//2015.10.20
#define VTM_RECIPE_SEND					_T("VTM.RECIPE.SEND")	//2015.11.05
#define VTM_LUMINANCE_CHANNEL			_T("VTM.LUMINANCE.CHANNEL")	//2016.08.08

//2015.11.17
#define PTM_CONCON_RESULT				_T("PTM.CONCON.RESULT")
#define PTM_CONITO_RESULT				_T("PTM.CONITO.RESULT")
#define PTM_ITOCON_RESULT				_T("PTM.ITOCON.RESULT")
#define PTM_ITOITO_RESULT				_T("PTM.ITOITO.RESULT")
//2015.11.17

#define SCAN_DIR_FORWARD				1
#define SCAN_DIR_BACKWARD				0

#define RETRY_TIMEOUT					5000 * 10
#define RETRY_COUNT						3				//2017.08.26

// Result File 관련
#define INDEX_PANELDATA					3
#define MAX_DEFECT						5000

// Recipe 정보
#define LEN_PANEL_ID					20//18			//2017.08.22 Before Merge, AVL315 Value was 40. This is ABI Value
#define LEN_MODEL_ID					40 //20->40

#define MAP_PANEL_SIZE_X				1690
#define MAP_PANEL_SIZE_Y				1300

#define DEFAULT_PANEL_WIDTH_SIZE		308		
#define DEFAULT_PANEL_HEIGHT_SIZE		534

#define MAP_PANEL_OFFSET_X				10
#define MAP_PANEL_OFFSET_Y				10

#define MAX_DEFECT_CNT					999// *3
#define LEN_IMAGE_PATH					128
#define LEN_RECIPE_MODEL				40	//2015.11.05

#define PLC_MAX_RECIPE					200	//2015.11.05	30->20
//#define MAX_RECIPE						200

#define PLC_MODEL_READ_PARAM_SIZE			3960
#define PLC_MODEL_READ_PARAM_BUFFER_SIZE	3960
#define PLC_MODEL_CHANGE_PARAM_BUFFER		40000
#define PLC_MODEL_PARAM_COUNT				990
#define PLC_MODEL_CHANGE_SLEEP				10000
#define PLC_MODEL_NAME_SIZE					10

#define SYSTEM_TIME_MAX					6	//2016.03.14

#define MAX_CIM_MEASURE_DATA			16	//2016.01.15

#define MAX_LUMINANCE_DATA				8	//2016.03.02
//PC ALARM BIT
#define		ADDR_PREALIGN_PC_DISCONNECT			_T("L400")
#define 	ADDR_INSP_PC1_DISCONNECT			_T("L401")
#define 	ADDR_INSP_PC2_DISCONNECT			_T("L402")
#define 	ADDR_F_SIDE_SCAN_ERROR				_T("L403")
#define 	ADDR_F_COAXIAL_SCAN_ERROR			_T("L404")
#define 	ADDR_R_SIDE_SCAN_ERROR				_T("L405")
#define 	ADDR_R_COAXIAL_SCAN_ERROR			_T("L406")
#define 	ADDR_PPID_MISMATCH_ERROR			_T("L407")
#define 	ADDR_F_SIDE_LIGHT_ERROR				_T("L408")
#define 	ADDR_F_COAXIAL_LIGHT_ERROR			_T("L409")
#define 	ADDR_R_SIDE_LIGHT_ERROR				_T("L410")
#define 	ADDR_R_COAXIAL_LIGHT_ERROR			_T("L411")
#define		ADDR_OVERLAP_COOR_OCCUR				_T("L420")		//LO412->L420
#define		ADDR_TEMPERATURE_WARNNING			_T("L425")
#define		ADDR_OVERLAP_COOR_OCCUR_NON_STOP	_T("L435")
#define		ADDR_OVERLAP_MEASURE_COOR_OCCUR		_T("L423")		//2016.03.07 Mr.Chu 측정 Overlap Alarm 발생 시 Alarm을 발생하되, 물류 진행 (대형요청사항)

#define 	ADDR_POL_MARK_ILEGAL_ALARM	_T("L421")      //2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능
#define 	ADDR_PROCESS_INTERLOCK1		_T("L570")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK2		_T("L571")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK3		_T("L572")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK4		_T("L573")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK5		_T("L574")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK6		_T("L575")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK7		_T("L576")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK8		_T("L577")	//2017.02.18  by mby
#define 	ADDR_PROCESS_INTERLOCK9		_T("L578")	//2017.02.18  by mby

#define 	ADDR_PROCESS_ALIGN_FAIL_PC1		_T("L553")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC2		_T("L554")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC3		_T("L555")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC4		_T("L556")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC5		_T("L557")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC6		_T("L558")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC7		_T("L559")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC8		_T("L560")
#define 	ADDR_PROCESS_ALIGN_FAIL_PC9		_T("L561")

//2015.09.29 Mr.Chu	
//AVL
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_CRACK					_T("W1000")		//Crack(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_CRACK					_T("W1001")		//Crack(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_BURR						_T("W1002")		//Burr(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_BURR					_T("W1003")		//Burr(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_EDGE_BROKEN				_T("W1004")		//Edge Broken(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_EDGE_BROKEN				_T("W1005")		//Edge Broken(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_CONER_BROKEN				_T("W1006")		//Corner Broken(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_CONER_BROKEN			_T("W1007")		//Corner Broken(TFT)
//주소값 W1008 미사용
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_MEASUREMENT				_T("W1009")		//측정불량(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_MEASUREMENT				_T("W1010")		//측정불량(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_SCRATCH					_T("W1011")		//S/C(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_SCRATCH					_T("W1012")		//S/C(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_PAD_SCRATCH					_T("W1013")		//PAD S/C
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_PAD_BURN					_T("W1014")		//PAD 탐, 변색
//주소값 W1015 미사용
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_WHITE_TURBIDITY			_T("W1016")		//백탁(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_ITO						_T("W1017")		//ITO 결손
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_DIMPLE_POINT				_T("W1018")		//점형Dimple(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_PIMPLE					_T("W1019")		//Pimple(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_CF_DIMPLE_LINE				_T("W1020")		//선형Dimple(CF)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_WHITE_TURBIDITY			_T("W1021")		//백탁(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_DIMPLE_POINT			_T("W1022")		//점형Dimple(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_PIMPLE					_T("W1023")		//Pimple(TFT)
#define		ADDR_OVERLAP_COOR_OCCUR_AVL_TFT_DIMPLE_LINE				_T("W1024")		//선형Dimple(TFT)


//NG FLAG 2015.06.13 add s-
//#define		ADDR_REQ_NG_FLAG_PNL1		_T("NgFlag")	//REQ_NG_FLAG_PNL1 FROM PLC
//#define		ADDR_RES_NG_FLAG_PNL1		_T("OK/NG")		//RES_NG_FLAG_PNL1 TO PLC
//#define		ADDR_RES_COMPLETE_PNL1		_T("COMPLETE")	//RES_COMPLETE_PNL1 TO PLC
//#define		ADDR_REQ_NG_FLAG_PNLID1		_T("PanelIDSet")// REQ_NG_FLAG_PNLID1 FROM PLC
//#define		ADDR_REQ_NG_FLAG_PNL2		_T("NgFlag")	//REQ_NG_FLAG_PNL2 FROM PLC
//#define		ADDR_RES_NG_FLAG_PNL2		_T("OK/NG")		//RES_NG_FLAG_PNL2 TO PLC
//#define		ADDR_RES_COMPLETE_PNL2		_T("COMPLETE")	//RES_COMPLETE_PNL2 TO PLC
//#define		ADDR_REQ_NG_FLAG_PNLID2		_T("PanelIDSet")// REQ_NG_FLAG_PNLID2 FROM PLC
//NG FLAG 2015.06.13 add e-

// PLC Map 정보 검사
#define ADDR_PC_STAGE_1				_T("L500")
#define ADDR_PC_STAGE_2				_T("L520")
#define ADDR_PC_STAGE_3				_T("L540")
#define ADDR_PLC_STAGE_1			_T("L700")
#define ADDR_PLC_STAGE_2			_T("L720")
#define ADDR_PLC_STAGE_3			_T("L740")

#define BIT_0						0x0001
#define BIT_1						0x0002
#define BIT_2						0x0004
#define BIT_3						0x0008
#define BIT_4						0x0010
#define BIT_5						0x0020
#define BIT_6						0x0040
#define BIT_7						0x0080
#define BIT_8						0x0100
#define BIT_9						0x0200
#define BIT_10						0x0400
#define BIT_11						0x0800
#define BIT_12						0x1000
#define BIT_13						0x2000
#define BIT_14						0x4000
#define BIT_15						0x8000

// PLC 각 검사기 사용 유무
#define ADDR_ALIGN					_T("L351")
#define ADDR_F_INSP					_T("L352")
#define ADDR_P_INSP					_T("L353")
#define ADDR_R_INSP					_T("L354")
// PLC 신호기 맵
#define ADDR_CON1					_T("L355")
#define ADDR_CON2					_T("L356")
#define ADDR_CON3					_T("L357")
// PLC Map 정보 검사
#define ADDR_PC_ALIVE				_T("L500")
#define ADDR_PLC_ALIVE				_T("L700")

// PLC Map 정보 검사 Stage #1
#define ADDR_PC_SERVER_ALIVE		_T("L450")
#define ADDR_PC_S1_ALIVE			_T("L500")
#define ADDR_PC_S1_ALARM			_T("L501")
#define ADDR_PC_S1_GRAB_READY		_T("L510")
#define ADDR_PC_S1_FORWARD			_T("L503")
#define ADDR_PC_S1_GRAB_ACK			_T("L504")
#define ADDR_PC_S1_GRAB_END			_T("L505")
#define ADDR_PC_S1_GRAB_ERROR		_T("L506")
#define ADDR_PC_S1_PPIDMISMATCH		_T("L507")
#define ADDR_PC_S1_SEQ_ERROR		_T("L508")
#define ADDR_PC_F_INSP_INIT_CMP		_T("L515")
#define ADDR_PC_F_INSP_GRAB_RETRY	_T("L516")

#define ADDR_PC_S1_JUDGE			_T("D3550")
#define ADDR_PC_S1_JUDGE_CODE		_T("D3552")

#define ADDR_PLC_S1_REQ				_T("L702")
#define ADDR_PLC_S1_FORWARD			_T("L703")
#define ADDR_PLC_S1_GRAB_END_OK		_T("L704")
#define ADDR_PLC_S1_RESET			_T("L705")
#define ADDR_PLC_F_INSP_INIT_REQ		_T("L715")
#define ADDR_PLC_F_INSP_GRAB_RETRY_ACK	_T("L716")

#define ADDR_PLC_S1_MODEL_ID		_T("D700")
#define ADDR_PLC_S1_PANEL_ID_1		_T("D1710")
#define ADDR_PLC_S1_PANEL_ID_2		_T("D1720")

//2016.09.24 Mr.Chu		Monitor Backward Seq PLC Adress  // PLC 요청사항으로 주소값분리.
//#define ADDR_PLC_BACKWARD_CHECK		_T("L708")

#define ADDR_PLC_S1_REQ_BWD				_T("L732")
#define ADDR_PLC_S1_GRAB_END_OK_BWD		_T("L734")

#define ADDR_PC_S1_GRAB_ACK_BWD			_T("L533")
#define ADDR_PC_S1_GRAB_END_BWD			_T("L535")

#define ADDR_PLC_S1_PANEL_ID_1_BWD		_T("D1720")
//

// PLC Map 정보 검사 Stage #2
#define ADDR_PC_S2_ALIVE			_T("L520")
#define ADDR_PC_S2_ALARM			_T("L521")
#define ADDR_PC_S2_GRAB_READY		_T("L511")
#define ADDR_PC_S2_FORWARD			_T("L523")
#define ADDR_PC_S2_GRAB_ACK			_T("L524")
#define ADDR_PC_S2_GRAB_END			_T("L525")
#define ADDR_PC_S2_GRAB_ERROR		_T("L526")
#define ADDR_PC_S2_PPIDMISMATCH		_T("L527")
#define ADDR_PC_S2_SEQ_ERROR		_T("L528")
#define ADDR_PC_P_INSP_INIT_CMP		_T("L535")
#define ADDR_PC_P_INSP_GRAB_RETRY	_T("L536")

#define ADDR_PC_S2_JUDGE			_T("D520")
#define ADDR_PC_S2_JUDGE_CODE		_T("D522")

#define ADDR_PLC_S2_REQ				_T("L722")
#define ADDR_PLC_S2_FORWARD			_T("L723")
#define ADDR_PLC_S2_GRAB_END_OK		_T("L724")
#define ADDR_PLC_S2_RESET			_T("L725")
#define ADDR_PLC_P_INSP_INIT_REQ		_T("L735")
#define ADDR_PLC_P_INSP_GRAB_RETRY_ACK	_T("L736")

#define ADDR_PLC_S2_MODEL_ID		_T("D750")
#define ADDR_PLC_S2_PANEL_ID_1		_T("D1780")
#define ADDR_PLC_S2_PANEL_ID_2		_T("D1790")

// PLC Map 정보 검사 Stage #3
#define ADDR_PC_S3_ALIVE			_T("L540")
#define ADDR_PC_S3_ALARM			_T("L541")
#define ADDR_PC_S3_GRAB_READY		_T("L512")
#define ADDR_PC_S3_FORWARD			_T("L543")
#define ADDR_PC_S3_GRAB_ACK			_T("L544")
#define ADDR_PC_S3_GRAB_END			_T("L545")
#define ADDR_PC_S3_GRAB_ERROR		_T("L546")
#define ADDR_PC_S3_PPIDMISMATCH		_T("L547")
#define ADDR_PC_S3_SEQ_ERROR		_T("L548")
#define ADDR_PC_R_INSP_INIT_CMP		_T("L555")
#define ADDR_PC_R_INSP_GRAB_RETRY	_T("L556")

//#define ADDR_PC_S3_JUDGE			_T("D540")
//#define ADDR_PC_S3_JUDGE_CODE		_T("D542")

#define ADDR_PLC_S3_REQ					_T("L742")
#define ADDR_PLC_S3_FORWARD				_T("L743")
#define ADDR_PLC_S3_GRAB_END_OK			_T("L744")
#define ADDR_PLC_S3_RESET				_T("L745")
#define ADDR_PLC_R_INSP_INIT_REQ		_T("L755")
#define ADDR_PLC_R_INSP_GRAB_RETRY_ACK	_T("L756")

#define ADDR_PLC_S3_MODEL_ID		_T("D800")
#define ADDR_PLC_S3_PANEL_ID_1		_T("D810")
#define ADDR_PLC_S3_PANEL_ID_2		_T("D820")

#define ADDR_PC_S1_CYCLE			_T("L515")
#define ADDR_PC_S2_CYCLE			_T("L535")
#define ADDR_PC_S3_CYCLE			_T("L555")

#define ADDR_ALARM_S1_SIDE			_T("L400")
#define ADDR_ALARM_S1_COAX			_T("L401")
#define ADDR_ALARM_S2_SIDE			_T("L402")
#define ADDR_ALARM_S2_COAX			_T("L403")
#define ADDR_ALARM_S3_TR1			_T("L404")
#define ADDR_ALARM_S3_TR2			_T("L405")

//2015.10.30
#define ADDR_PC_S4_GRAB_READY		_T("L513")
#define ADDR_PC_S5_GRAB_READY		_T("L514")
#define ADDR_PC_S6_GRAB_READY		_T("L515")
#define ADDR_PC_S7_GRAB_READY		_T("L516")
#define ADDR_PC_S8_GRAB_READY		_T("L517")
#define ADDR_PC_S9_GRAB_READY		_T("L518")
#define ADDR_PC_ALIGN_GRAB_READY	_T("L660")

//2015.11.03 Mr.Chu
#define ADDR_QMS_UPLOAD_FAIL		_T("L422")

#define  ADDR_UNLOAD_REQUEST    	_T("B902")
//2015.03.10 add by jee - CIM 관련
// Recipe Command - Upload 
#define ADDR_RCP_UP_EQ_RECV_NO		_T("W600")	//
#define ADDR_RCP_UP_EQ_CMD_NO		_T("W601")
#define ADDR_RCP_UP_EQ_ACK_CODE		_T("W602")
#define ADDR_RCP_UP_EQ_DATA_0		_T("W608")	// Light Front - Forward side 
#define ADDR_RCP_UP_EQ_DATA_1		_T("W609")	// Light Front - Backward side 
#define ADDR_RCP_UP_EQ_DATA_2		_T("W60A")	// Light Front - Forward coaxial
#define ADDR_RCP_UP_EQ_DATA_3		_T("W60B")	// Light Front - Backward coaxial 
#define ADDR_RCP_UP_EQ_DATA_4		_T("W60C")	// Light Rear - Forward side 
#define ADDR_RCP_UP_EQ_DATA_5		_T("W60D")	// Light Rear - Backward side 
#define ADDR_RCP_UP_EQ_DATA_6		_T("W60E")	// Light Rear - Forward coaxial 
#define ADDR_RCP_UP_EQ_DATA_7		_T("W60F")	// Light Rear - Backward coaxial 
#define ADDR_RCP_UP_EQ_DATA_8		_T("W610")	// Light Transmit - Forward side 
#define ADDR_RCP_UP_EQ_DATA_9		_T("W611")	// Light Transmit - Backward side 
#define ADDR_RCP_UP_CIM_PUB_NO		_T("WB00") //  
#define ADDR_RCP_UP_CIM_CMD_NO		_T("WB01") // 
#define ADDR_RCP_UP_CIM_RCP_NO		_T("WB02")
// Recipe Command - Edit
#define ADDR_RCP_EDIT_EQ_PUB_NO		_T("W680")
#define ADDR_RCP_EDIT_EQ_CMD_NO		_T("W681")
#define ADDR_RCP_EDIT_EQ_RCP_NO		_T("W682")
#define ADDR_RCP_EDIT_EQ_DATA_0		_T("W688")	// Light Front - Forward side 
#define ADDR_RCP_EDIT_EQ_DATA_1		_T("W689")	// Light Front - Backward side 
#define ADDR_RCP_EDIT_EQ_DATA_2		_T("W68A")	// Light Front - Forward coaxial
#define ADDR_RCP_EDIT_EQ_DATA_3		_T("W68B")	// Light Front - Backward coaxial 
#define ADDR_RCP_EDIT_EQ_DATA_4		_T("W68C")	// Light Rear - Forward side
#define ADDR_RCP_EDIT_EQ_DATA_5		_T("W68D")	// Light Rear - Backward side 
#define ADDR_RCP_EDIT_EQ_DATA_6		_T("W68E")	// Light Rear - Forward coaxial
#define ADDR_RCP_EDIT_EQ_DATA_7		_T("W68F")	// Light Rear - Backward coaxial 
#define ADDR_RCP_EDIT_EQ_DATA_8		_T("W690")	// Light Transmit - Forward side
#define ADDR_RCP_EDIT_EQ_DATA_9		_T("W691")	// Light Transmit - Backward side 
#define ADDR_RCP_EDIT_CIM_RECV_NO	_T("BA80")
#define ADDR_RCP_EDIT_CIM_CMD_NO	_T("WB81")
#define ADDR_RCP_EDIT_CIM_ACK_CODE	_T("WB82")
// Recipe Command - Download
#define ADDR_RCP_DOWN_EQ_RECV_NO	_T("W700")
#define ADDR_RCP_DOWN_EQ_CMD_NO		_T("W701")
#define ADDR_RCP_DOWN_EQ_ACK_CODE	_T("W702")
#define ADDR_RCP_DOWN_CIM_PUB_NO	_T("WC00")
#define ADDR_RCP_DOWN_CIM_CMD_NO	_T("WC01")
#define ADDR_RCP_DOWN_CIM_RCP_NO	_T("WC02")
// #define ADDR_RCP_DOWN_CIM_DATA_0	_T("WC08")	// Light Front - Forward side 
// #define ADDR_RCP_DOWN_CIM_DATA_1	_T("WC09")	// Light Front - Backward side
// #define ADDR_RCP_DOWN_CIM_DATA_2	_T("WC0A")	// Light Front - Forward coaxial
// #define ADDR_RCP_DOWN_CIM_DATA_3	_T("WC0B")	// Light Front - Backward coaxial
// #define ADDR_RCP_DOWN_CIM_DATA_4	_T("WC0C")	// Light Rear - Forward side
// #define ADDR_RCP_DOWN_CIM_DATA_5	_T("WC0D")	// Light Rear - Backward side
// #define ADDR_RCP_DOWN_CIM_DATA_6	_T("WC0E")	// Light Rear - Forward coaxial
// #define ADDR_RCP_DOWN_CIM_DATA_7	_T("WC0F")	// Light Rear - Backward coaxial
#define ADDR_RCP_DOWN_CIM_DATA_8	_T("WC10")	// Light Transmit - Forward side
#define ADDR_RCP_DOWN_CIM_DATA_9	_T("WC11")	// Light Transmit - Backward side
// Recipe Command - Download 2015.10.01 Mr.Chu
#define ADDR_RCP_DOWN_CIM_DATA_0	_T("W700")	// Light Front - Forward side 
#define ADDR_RCP_DOWN_CIM_DATA_1	_T("W701")	// Light Front - Backward side
#define ADDR_RCP_DOWN_CIM_DATA_2	_T("W702")	// Light Front - Forward coaxial
#define ADDR_RCP_DOWN_CIM_DATA_3	_T("W703")	// Light Front - Backward coaxial
#define ADDR_RCP_DOWN_CIM_DATA_4	_T("W704")	// Light Rear - Forward side
#define ADDR_RCP_DOWN_CIM_DATA_5	_T("W705")	// Light Rear - Backward side
#define ADDR_RCP_DOWN_CIM_DATA_6	_T("W706")	// Light Rear - Forward coaxial
#define ADDR_RCP_DOWN_CIM_DATA_7	_T("W707")	// Light Rear - Backward coaxial

// Process Information
#define ADDR_PROC_INFO_EQ_PUB_NO		_T("W500")		// Publish No
#define ADDR_PROC_INFO_EQ_CMD_NO		_T("W501")		// Command No
#define ADDR_PROC_INFO_EQ_PNL_ID_0		_T("W502")		// Panel ID (W2302 ~ W230B) 10 Word
#define ADDR_PROC_INFO_EQ_PNL_ID_1		_T("W503")		// Panel ID (W2302 ~ W230B) 10 Word
#define ADDR_PROC_INFO_CIM_RECV_NO		_T("WA00")	// Receive No
#define ADDR_PROC_INFO_CIM_CMD_NO		_T("WA01")	// Command No 110
#define ADDR_PROC_INFO_CIM_STEPID		_T("WA08")		//_T("W11108")	// StepName		(W11108 ~ W11111) 10 Word
#define ADDR_PROC_INFO_CIM_STFLAGARRAY	_T("WA12")	// STFLAGARRAY	(W11112 ~ W1111E), 최상위 1Byte는 0x20
#define ADDR_PROC_INFO_CIM_STFLAGCF		_T("WA1F")	// STFLAGCF		(W1111F ~ W1112B), 최상위 1Byte는 0x20
#define ADDR_PROC_INFO_CIM_STFLAGCELL	_T("WA2C")	// STFLAGCELL	(W1112C ~ W11138), 최상위 1Byte는 0x20
#define ADDR_PROC_INFO_CIM_STFLAGSLMOD	_T("WA39")	// STFLAGSLMOD	(W11139 ~ W11145), 최상위 1Byte는 0x20
// Process Data
#define ADDR_PROC_DATA_EQ_PUB_NO		_T("W580")		// Publish No
#define ADDR_PROC_DATA_EQ_CMD_NO		_T("W581")		// Command No 170
#define ADDR_PROC_DATA_EQ_PNL_ID_0		_T("W582")		// Panel ID (W2382 ~ W238B) 10 Word
#define ADDR_PROC_DATA_EQ_PNL_ID_1		_T("W683")		// Panel ID (W2382 ~ W238B) 10 Word
#define ADDR_PROC_DATA_EQ_INSPMODE		_T("W590")		// 검사Mode 1 : 자동, 2 : 수동
//#define ADDR_PROC_DATA_EQ_GRADE1		_T("W691")		// "A", "B", "C", "D"

//2015.09.28 Mr.Chu
#define ADDR_PROC_DATA_EQ_GRADE1						_T("W591")		// "A", "B", "C", "D" 
#define ADDR_PROC_DATA_EQ_BIG_GRADE1_ABI				_T("W591")		// "A", "B", "C", "D"
#define ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE				_T("L530")		// Glass judge bit
#define ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE_CHECK			_T("B03F")		// Glass judge bit Check
#define ADDR_PROC_DATA_EQ_BIG_JUDGE1					_T("W9C3")		// "OK", "NG" 
#define ADDR_PROC_DATA_EQ_BIG_JUDGE1_ABI				_T("W594")		// "OK", "NG" 
#define ADDR_PROC_DATA_EQ_BIG_JUDGE2					_T("W9C4")
#define ADDR_PROC_DATA_EQ_BIG_JUDGE2_ABI				_T("W595")
#define ADDR_PROC_DATA_EQ_BIG_JUDGE3_AVL				_T("W9C6")
//#define ADDR_PROC_DATA_EQ_BIG_JUDGE2					_T("69C4")

#define ADDR_PROC_DATA_EQ_GRADE2		_T("W592")		// "AF1", "AF2", "AF3", ...
#define ADDR_PROC_DATA_EQ_GRADE2_AVL	_T("W9C1")		// "AF1", "AF2", "AF3", ...
#define ADDR_PROC_DATA_EQ_JUDGE1		_T("W594")		// "OK", "NG" 
#define ADDR_PROC_DATA_EQ_JUDGE2		_T("W595")		// 상위:GROUP, 하위:CODE
#define ADDR_PROC_DATA_EQ_JUDGE3		_T("W597")		// 상위:GROUP, 하위:CODE
#define ADDR_PROC_DATA_EQ_CHKLIST1		_T("W699")		// Check List
#define ADDR_PROC_DATA_EQ_CHKLIST2		_T("W69A")		// Check List
#define ADDR_PROC_DATA_EQ_CHKLIST3		_T("W69B")		// Check List
#define ADDR_PROC_DATA_EQ_CHKLIST4		_T("W69C")		// Check List
#define ADDR_PROC_DATA_CIM_RECV_NO		_T("WA80")	    // Receive No	
#define ADDR_PROC_DATA_CIM_CMD_NO		_T("WA81")	    // Command No 170	
#define ADDR_PROC_DATA_CIM_ACK_CODE		_T("WA82")	    // Ack Code	

//Measure Data 2016.01.15
#define MEASURE_CIM_DATA_EQ_OFFSET		9999

#define ADDR_MEASURE_DATA_EQ_PUB_NO		_T("W820")		// Publish No
#define ADDR_MEASURE_DATA_EQ_CMD_NO		_T("W821")		// Command No 170
#define ADDR_MEASURE_DATA_EQ_PNL_ID_0	_T("W822")		// Panel ID 0
#define ADDR_MEASURE_DATA_EQ_PNL_ID_1	_T("W823")		// Panel ID 1
//AVL
#define ADDR_AVL_MEASURE_EQ_DATA_0		_T("W2610")		// CF_X1
#define ADDR_AVL_MEASURE_EQ_DATA_1		_T("W2611")		// CF_Y1
#define ADDR_AVL_MEASURE_EQ_DATA_2		_T("W2612")		// CF_X2
#define ADDR_AVL_MEASURE_EQ_DATA_3		_T("W2613")		// CF_Y2
#define ADDR_AVL_MEASURE_EQ_DATA_4		_T("W2614")		// CF_X3
#define ADDR_AVL_MEASURE_EQ_DATA_5		_T("W2615")		// CF_Y3
#define ADDR_AVL_MEASURE_EQ_DATA_6		_T("W2616")		// CF_X4
#define ADDR_AVL_MEASURE_EQ_DATA_7		_T("W2617")		// CF_Y4
#define ADDR_AVL_MEASURE_EQ_DATA_8		_T("W2618")		// TFT_X5
#define ADDR_AVL_MEASURE_EQ_DATA_9		_T("W2619")		// TFT_Y5
#define ADDR_AVL_MEASURE_EQ_DATA_10		_T("W261A")		// TFT_X6
#define ADDR_AVL_MEASURE_EQ_DATA_11		_T("W261B")		// TFT_Y6
#define ADDR_AVL_MEASURE_EQ_DATA_12		_T("W261C")		// TFT_X7
#define ADDR_AVL_MEASURE_EQ_DATA_13		_T("W261D")		// TFT_Y7
#define ADDR_AVL_MEASURE_EQ_DATA_14		_T("W261E")		// TFT_X8
#define ADDR_AVL_MEASURE_EQ_DATA_15		_T("W261F")		// TFT_Y8
//ABI
#define ADDR_ABI_MEASURE_EQ_DATA_0		_T("W830")		// POL_CF_X1
#define ADDR_ABI_MEASURE_EQ_DATA_1		_T("W831")		// POL_CF_Y1
#define ADDR_ABI_MEASURE_EQ_DATA_2		_T("W832")		// POL_CF_X2
#define ADDR_ABI_MEASURE_EQ_DATA_3		_T("W833")		// POL_CF_Y2
#define ADDR_ABI_MEASURE_EQ_DATA_4		_T("W834")		// POL_CF_X3
#define ADDR_ABI_MEASURE_EQ_DATA_5		_T("W835")		// POL_CF_Y3
#define ADDR_ABI_MEASURE_EQ_DATA_6		_T("W836")		// POL_CF_X4
#define ADDR_ABI_MEASURE_EQ_DATA_7		_T("W837")		// POL_CF_Y4
#define ADDR_ABI_MEASURE_EQ_DATA_8		_T("W838")		// POL_TFT_X1
#define ADDR_ABI_MEASURE_EQ_DATA_9		_T("W839")		// POL_TFT_Y1
#define ADDR_ABI_MEASURE_EQ_DATA_10		_T("W83A")		// POL_TFT_X2
#define ADDR_ABI_MEASURE_EQ_DATA_11		_T("W83B")		// POL_TFT_Y2
#define ADDR_ABI_MEASURE_EQ_DATA_12		_T("W83C")		// POL_TFT_X3
#define ADDR_ABI_MEASURE_EQ_DATA_13		_T("W83D")		// POL_TFT_Y3
#define ADDR_ABI_MEASURE_EQ_DATA_14		_T("W83E")		// POL_TFT_X4
#define ADDR_ABI_MEASURE_EQ_DATA_15		_T("W83F")		// POL_TFT_Y4


#define ADDR_CUR_RCP_CHG_REQ			_T("B510")
#define ADDR_CUR_RCP_CHG_ACK			_T("B310")
#define ADDR_CUR_CIM_RCP_NO				_T("W8016")
#define ADDR_CUR_EQP_RCP_NO				_T("W200")


//////////////////////////////////////
#define ADDR_ABI_CON1_F_CLASSIFY_END		_T("L609")
#define ADDR_ABI_CON1_R_CLASSIFY_END		_T("L615")
#define ADDR_ABI_CON1_F_NG_FLAG				_T("D611")
#define ADDR_ABI_CON1_R_NG_FLAG				_T("D612")

#define ADDR_ABI_CON2_F_CLASSIFY_END		_T("L632")
#define ADDR_ABI_CON2_R_CLASSIFY_END		_T("L635")
#define ADDR_ABI_CON2_F_NG_FLAG				_T("D631")
#define ADDR_ABI_CON2_R_NG_FLAG				_T("D632")

#define ADDR_ABI_CON3_F_CLASSIFY_END		_T("L652")
#define ADDR_ABI_CON3_R_CLASSIFY_END		_T("L655")
#define ADDR_ABI_CON3_F_NG_FLAG				_T("D651")
#define ADDR_ABI_CON3_R_NG_FLAG				_T("D652")

#define ADDR_ABI_R_CLASSIFY_END				_T("L550")
//#define ADDR_AVL_CON3_R_CLASSIFY_END		_T("L655")
#define ADDR_ABI_R_NG_FLAG1					_T("D540")
//#define ADDR_ABI_R_NG_FLAG2				_T("D541")
#define ADDR_R_NG_FLAG_COMPLETE				_T("L530")  // AVL 언로드 R만 사용
#define ADDR_AVL_NG_FLAG_COMPLETE			_T("L530")
#define ADDR_AVL_F_NG_FLAG1					_T("D3650")	//_T("D520") 중형 NG Flag Word 영역 보고.	//2015.11.14 Mr.Chu
#define ADDR_AVL_R_NG_FLAG2					_T("D521")
//#define ADDR_AVL_CON3_R_NG_FLAG			_T("D652")
//////////////////////////////////////
//2015.06.18
#define ADDR_PLC_NG_FLAG_R_REQ				_T("L730")
#define ADDR_AVL_PLC_NG_FLAG_REQ			_T("L730")
#define ADDR_AVL_PLC_NG_FLAG_PANEL_ID_1		_T("D3900")	//_T("D780") 중형 NG flag check Panel ID Read Adress		//2015.11.14 Mr.Chu
#define ADDR_AVL_PLC_NG_FLAG_PANEL_ID_2		_T("D790")
//2015.06.18

//2015.06.25
#define ADDR_L_NG_FLAG_COMPLETE				_T("L509")  // 언로드 L 은 ABI만 사용
#define ADDR_ABI_NG_FLAG_COMPLETE			_T("L510")
#define ADDR_ABI_F_NG_FLAG1					_T("D1520")
#define ADDR_ABI_R_NG_FLAG2					_T("D501")
#define ADDR_PLC_NG_FLAG_L_REQ				_T("L709")
#define ADDR_ABI_PLC_NG_FLAG_REQ			_T("L710")
#define ADDR_ABI_PLC_NG_FLAG_PANEL_ID_1		_T("D1780")
#define ADDR_ABI_PLC_NG_FLAG_PANEL_ID_2		_T("D740")
//2015.06.25

// 2015.07.31 Packing 용 MAP
#define ADDR_PACKING_PLC_ALIVE				_T("D700")
#define ADDR_PACKING_PLC_GRAB_REQUEST		_T("D710")
#define ADDR_PACKING_PLC_GRAB_END			_T("D711")
#define ADDR_PACKING_PLC_GRAB_RESULT		_T("D712")
#define ADDR_PACKING_UPDOWN_CHECK			_T("D713")
#define ADDR_PACKING_PANEL_ID				_T("D720")
#define ADDR_PACKING_PC_ALIVE_ACK			_T("D750")
#define ADDR_PACKING_PC_GRAB_ACK			_T("D760")
#define ADDR_PACKING_PC_GRAB_END			_T("D761")
#define ADDR_PACKING_PC_GRAB_RESULT			_T("D762")
#define ADDR_PACKING_PC_NG_FLAG1			_T("D763")
#define ADDR_PACKING_PC_NG_FLAG2			_T("D764")


//2015.09.17
#define ADDR_PLC_BYPASS_REQ_PANEL1			_T("D3530")
#define ADDR_PLC_BYPASS_REQ_PANEL2			_T("D701")

#define ADDR_PLC_BYPASS_REQ2_PANEL1			_T("D750")
#define ADDR_PLC_BYPASS_REQ2_PANEL2			_T("D751")
//2015.09.17

//2016.01.15
#define ADDR_PLC_BYPASS_DEFECT_CODE_PANEL1			_T("D702")
#define ADDR_PLC_BYPASS_DEFECT_CODE_PANEL2			_T("D704")
//2016.01.15

#define ADDR_PC_MODEL_CHANGE_REQ			_T("L670")
#define ADDR_PC_MODEL_CHANGE_OK				_T("L671")
#define ADDR_PC_MODEL_CHANGE_NG				_T("L672")
#define ADDR_PLC_MODEL_CHANGE_CHECK			_T("L870")
#define ADDR_PLC_MODEL_CHANGE_REQ			_T("L875")
#define ADDR_PC_MODEL_SAVE_COMPLETE			_T("L675")
#define ADDR_PLC_MODEL_CHANGE_MODEL_NO		_T("D15995")
#define ADDR_PLC_CURRENT_MODEL_NAME			_T("ZR0990")
#define ADDR_PLC_CURRENT_MODEL_PARAM		_T("ZR0000")
#define ADDR_PLC_MODEL_CHANGE_PARAM_BUFFER	_T("ZR40000")
#define ADDR_PLC_MODEL_CHANGE_NAME_BUFFER	_T("ZR40990")

//2015.11.05 Auto Recipe Change
#define ADDR_PLC_AUTO_MODEL_CHANGE_REQ		_T("M40")
#define ADDR_PLC_MANUAL_MODEL_CHANGE_REQ	_T("M41")
#define ADDR_PLC_MODEL_CHANGE_INTERLOCK		_T("M42")
#define ADDR_PLC_MODEL_UPDATE_REQ			_T("M43")
#define ADDR_PLC_MODEL_CHANGE_REQ_NO_INFO	_T("D15996")
#define ADDR_PLC_MODEL_CHANGE_REQ_NO		_T("D16002")
#define ADDR_PLC_CURRENT_RECIPE_NO			_T("D5630")
#define ADDR_PLC_CURRENT_RECIPE_MODEL		_T("ZR10000")
//2017.09.05 Added
//#define ADDR_PLC_RECIPE_MODEL_1				_T("ZR20000")
//#define ADDR_PLC_RECIPE_MODEL_2				_T("ZR30000")
//#define ADDR_PLC_RECIPE_MODEL_3				_T("ZR40000")
//#define ADDR_PLC_RECIPE_MODEL_4				_T("ZR50000")
//#define ADDR_PLC_RECIPE_MODEL_5				_T("ZR60000")
//#define ADDR_PLC_RECIPE_MODEL_6				_T("ZR70000")
//#define ADDR_PLC_RECIPE_MODEL_7				_T("ZR80000")
//#define ADDR_PLC_RECIPE_MODEL_8				_T("ZR90000")
//#define ADDR_PLC_RECIPE_MODEL_9				_T("ZR100000")
//#define ADDR_PLC_RECIPE_MODEL_10			_T("ZR110000")
//#define ADDR_PLC_RECIPE_MODEL_11			_T("ZR120000")
//#define ADDR_PLC_RECIPE_MODEL_12			_T("ZR130000")
//#define ADDR_PLC_RECIPE_MODEL_13			_T("ZR140000")
//#define ADDR_PLC_RECIPE_MODEL_14			_T("ZR150000")
//#define ADDR_PLC_RECIPE_MODEL_15			_T("ZR160000")
//#define ADDR_PLC_RECIPE_MODEL_16			_T("ZR170000")
//#define ADDR_PLC_RECIPE_MODEL_17			_T("ZR180000")
//#define ADDR_PLC_RECIPE_MODEL_18			_T("ZR190000")
//#define ADDR_PLC_RECIPE_MODEL_19			_T("ZR200000")
//#define ADDR_PLC_RECIPE_MODEL_20			_T("ZR210000")
//#define ADDR_PLC_RECIPE_MODEL_21			_T("ZR220000")
//#define ADDR_PLC_RECIPE_MODEL_22			_T("ZR230000")
//#define ADDR_PLC_RECIPE_MODEL_23			_T("ZR240000")
//#define ADDR_PLC_RECIPE_MODEL_24			_T("ZR250000")
//#define ADDR_PLC_RECIPE_MODEL_25			_T("ZR260000")
//#define ADDR_PLC_RECIPE_MODEL_26			_T("ZR270000")
//#define ADDR_PLC_RECIPE_MODEL_27			_T("ZR280000")
//#define ADDR_PLC_RECIPE_MODEL_28			_T("ZR290000")
//#define ADDR_PLC_RECIPE_MODEL_29			_T("ZR300000")
//#define ADDR_PLC_RECIPE_MODEL_30			_T("ZR310000")
//
//2016.03.03 Mr.Chu
#define ADDR_PLC_CHANGE_RECIPE_NO			_T("D5860")
#define ADDR_PLC_CHANGE_RECIPE_NAME			_T("D5870")
//2015.11.05 Auto Recipe Change
#define ADDR_PACKING_PLC_CUR_MODELID		_T("D3000") // 2015.10.15 add by ikm Packing 용 자동 레시피 변경
#define ADDR_PACKING_PLC_CHG_MODELID		_T("M7300") // 2015.10.15 add by ikm Packing 용 자동 레시피 변경
#define ADDR_PACKING_PLC_SAVE_MODEL  		_T("M7302") // 2015.10.15 add by ikm Packing 용 자동 레시피 변경
#define ADDR_PACKING_PLC_DEL_MODEL  		_T("M7304") // 2015.10.15 add by ikm Packing 용 자동 레시피 변경
#define ADDR_PACKING_PLC_NG_JUDGE	  		_T("D763")  // 2015.10.15 add by ikm Packing 용 자동 레시피 변경
#define MAX_PLC_MODEL_CNT					100   
//2015.08.19
#define ADDR_PC_ALIGN_COMPLETE_NG		_T("L661")
#define ADDR_PC_ALIGN_COMPLETE_OK		_T("L662")
//#define ADDR_PC_ALIGN_3_4_COMPLETE_NG		_T("L663")
//#define ADDR_PC_ALIGN_3_4_COMPLETE_OK		_T("L664")
#define ADDR_PLC_ALIGN_READY				_T("L860")
#define ADDR_PLC_ALIGN_REQUEST				_T("L861")
#define ADDR_PLC_PANEL_ID					_T("D1710")

//2016.03.14
#define ADDR_PLC_CURRENT_YEAR				_T("D10030")
#define ADDR_PLC_CURRENT_MONTH				_T("D10031")
#define ADDR_PLC_CURRENT_DAY				_T("D10032")
#define ADDR_PLC_CURRENT_HOUR				_T("D10033")
#define ADDR_PLC_CURRENT_MINUTE				_T("D10034")
#define ADDR_PLC_CURRENT_SECOND				_T("D10035")

#define MAX_PANEL_LIST				700

//2015.0.03 add by ikm - S
//const int MAX_BUFFER							= (1024*15);	///< Send/Receive Buffer Size
//const int APP_OK								= 0;			///< Application 에서 사용하는 정상 응답
//const int APP_NG								= 1;			///< Application 에서 사용하는 이상 응답
//const int DEFAULT_LOG_SIZE						= 512;			//< 기본 로그 사이즈.
#ifndef TSTRING
#define TSTRING		basic_string<TCHAR> 
#endif

//enum LOG_LEVEL
//{
//	LOG_LEVEL_ALWAYS = 0,	//MAX Log Level.
//	LOG_LEVEL_1,
//	LOG_LEVEL_2,
//	LOG_LEVEL_3,
//	LOG_LEVEL_4,			//MIN Log Level.
//	LOG_LEVEL_NONE,
//};

//2015.0.03 add by ikm - E

#endif


//#define CSLOCK_AND_EXCEPTION_START	lock(); try{
//#define CSUNLOCK_AND_EXCEPTION_END	}catch(int nErrCode){nRet = nErrCode;}	catch(...){nRet = PLC_TASK_EXCEPTION_UNKNOWN_EXCEPTION;} unlock();



//一般定義
#define	ON			(1)
#define	OFF			(0)


#define MAX_WORD_SIZE			512		//한번에 읽어야 하는 최대 워드 크기

#define BUFFER_LENGTH (128)

#define PLC_TASK						7


#define	WM_TRAYICON_MSG					WM_USER + 1
#define	WM_PRINT_UI_LOG_MSG_UNICODE		WM_USER + 2
#define	WM_PRINT_UI_LOG_MSG_MULTI_BYTE	WM_USER + 3