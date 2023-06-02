// 
// ../Lib/NFLib.lib
// System Hardware Control Define.h
#if !defined (_SYSHWCTRL_DEFINE)
#define _SYSHWCTRL_DEFINE

// Sap System
#define SAP_SYSTEM_CORECO			_T("Xcelera-HS_PX8_1")
#define SAP_SYSTEM_VGA				M_SYSTEM_VGA

enum E_SELECT_SCAN_DIR
{
	E_DIR_FORWARD = 0,
	E_DIR_BACK,
	E_SELECT_DIR_NUMBER
};

enum E_SELECT_SCAN_TYPE
{
	E_1_SCAN_TYPE = 1,
	E_2_SCAN_TYPE
};

enum E_INSPECTION_SURFACE
{
	E_CF_TYPE = 0,
	E_TFT_TYPE,
	E_POL_TYPE
};

// 2015.09.04 소형장비 Panel 별 Filter ROI 사용 / 비사용 추가
enum eFilterUsePanel
{
	eNoUse,
	eUsePanel1,
	eUsePanel2,
	eUseAllPanel
};

// 2015.08.06 Defect Filter ROI 추가
enum ROIDrawMode
{
	eBasicROI = 0,
	eFilterROI,
	ePolMarkROI,
	eMoniImgROI
};

// config path
#define HWCONFIG_ROOT_FILENAME		"D:\\CRUX\\INI\\HWConfig.INI"

// Camera Area & Line Camera define
#define AREACAM				0
#define	LINE2K				1
#define	LINE8K				2
#define	LINE4K				3

#define DEF_APPLICATION_VERSION			"15.02.09.Ver1"
#define DEF_ALGORITHM_VERSION			"15.02.09.Ver1"
// 
// #define DEF_FISRT_ACTIVE				1
// //#define DEF_SECOND_ACTIVE				2 

#define MAX_GRAB_BUFF_CNT  60// 20  // 15" 400mm/0.026

// system init states
#define SYS_INIT_NOT_STARTED	-1
#define SYS_INIT_ING			 0
#define SYS_INIT_FAIL			 1
#define SYS_INIT_SUCCESS		 2

#define INIT_STATE_WAIT		 0
#define INIT_STATE_SUCCESS	 1
#define INIT_STATE_FAIL		 2
#define INIT_STATE_RUNNING	 3

#define	INIT_STEP_3		3			// 3. Serial Port 열기 : Light, Trigger,
#define	INIT_STEP_4		4			// 4. 통신 소켓 생성
#define	INIT_STEP_5		5			// 5. Thread 생성
#define	INIT_STEP_6		6			// 6. 모델 설정
#define	INIT_STEP_7		7			// 7. 초기화 완료

// inspection 
//#define MAX_INSP_THREAD		4  //3	// number of max inspection treads( Area, Active Line, Si Line Camera)
#define MAX_INSP_THREAD	    30 //4//	1  //3	// number of max inspection treads( Area, Active Line, Si Line Camera)
#define NUM_INSP_STAGE		2 	// number of inspection stages : Area, Active / Si Line Camera 
//#define NUM_INSP_STAGE		1 	// number of inspection stages : Area, Active / Si Line Camera 
//#define NUM_INSP_CAMERA     4   //3
#define NUM_INSP_CAMERA			4//   1   //3
#define NUM_MAX_PANEL_IN_CELL     2//   1   //3
//#define NUM_FRAME_GRABBER	3
//#define NUM_FRAME_GRABBER    3//	1
#define NUM_SOCK_PEER		2	// Socket 통신할 상대방의 개수 - 현재[2013.7.31]는 AMT 두 대와 통신
#define NUM_MAX_LINE_CAMERA 3
#define CAM_MAX_NUM			12

#define DEF_AREA_ONE_CAMERA
//#undef	DEF_AREA_ONE_CAMERA

#define SILENT_RETRY_CNT	5		// silent retry :  display 불량 발생 시 사용자에게 확인 전 자동 재시도 회수

// lights
#define HW_BL_INTENSITY			200
#define HW_FL_INTENSITY			255

// Socket Status Msg
#define MSG_GRAB_START				300
#define MSG_GRAB_END				301
#define MSG_GRAB_ERR				302
#define MSG_RESULT_START			303
#define MSG_RESULT_END				304
#define MSG_OG_USE				    305
#define MSG_OG_NOT_USE				306
#define MSG_ABORT					307

//// MACHINE -> VISION
//// 추가 메시지 협의 사항.
////1. 각각의 Stage마다 소켓이 따로 있는데, 
////   전체 적용하는 메시지와 개별적으로 적용하는 메시지가 따로 있어야함
////2. 모델 변경시 해당 모델 정보. MODEL_CHANGE후 변경 모델 이름 
////3. 자동 검사중 메시지와 수동 검사 메시지 별도 관리하는 것이 좋을것 같음
////4. 
#define MODEL_CHANGE				"MOT.MODELCHE"                  // 모델 변경.		MOT.MODELCHE.[MODEL] 
#define REQUEST_VISION_STATE		"MOT.REQUEST.VISION.STATE"      // 비젼 상태 확인
#define GRAB_START					"MOT.GRAB.START"				// 촬상 시작 요청	MOT.GRAB.START[ID]
#define REQUEST_RESULT				"MOT.REQUEST.RESULT"			// 결과 요청		MOT.REQUEST.RESULT[ID]	
#define RESULT_RECEIVE_OK			"MOT.RESULT.RECEIVE.OK"			// 검사 결과 완료후 정보 지움 메시지 추가
#define CLASS_END					"MOT.CLASS.END"					// 검사 결과 완료후 정보 지움 메시지 추가
#define ABORT						"MOT.ABORT"	
#define POLTURN_OK					"MOT.POLTURN.OK"				// 폴 90도 회전 완료
#define POLZERO_OK					"MOT.POLZERO.OK"				// 폴 0도 복귀 완료 

#define	OG_NOT_USE		 			"MOT.OG.NOT.USE"				// 외관 사용 안함
#define	OG_USE		 				"MOT.OG.USE"					// 외관 사용 안함
#define TEACH_MODE_START			"MOT.TEACH.MODE.START"			// 티칭 모드 전환
#define TEACH_MODE_END				"MOT.TEACH.MODE.END"			// 티칭 모드 종료
#define OP_INFORM_OK				"MOT.OP.OK"						// 운영측 정보 생성 요청후 파일 생성 완료  

//// VISION -> MACHINE 
#define MODEL_CHANGE_OK				"FDB.MODELCHE.OK"					// 모델 변경 완료
#define REQUEST_VISION_AUTO			"FDB.REQUEST.VISION.AUTO"			// 비젼 현재 AUTO 상태 
#define REQUEST_VISION_TEACH		"FDB.REQUEST.VISION.TEACH"			// 비젼 현재 TEACH 상태 
#define GRAB_START_OK				"FDB.GRAB.START.OK"					// 촬상 시작 OK
#define GRAB_START_END_OK			"FDB.GRAB.END.OK"					// 촬상 완료
#define GRAB_START_END_AREA_OK		"FDB.GRAB.END.AREA.OK"					// 촬상 완료
#define GRAB_START_END_ERR			"FDB.GRAB.END.ERR"					// 촬상 ERR
#define ACT_CLASSIFICATION_END_OK	"FDB.ACT.CLASS.END.OK"				// ActiveEndOk
#define OG_CLASSIFICATION_END_OK	"FDB.OG.CLASS.END.OK"				// OgEndOk
#define CLASSIFICATION_END_OK		"FDB.CLASS.END.OK"					// Classification End
#define REQUEST_RESULT_OK			"FDB.REQUEST.RESULT.OK"				// 결과 요청에 대한 확인
#define RESULT_GOOD					"FDB.RESULT.GOOD"					// 검사 결과 양품
#define RESULT_BAD					"FDB.RESULT.BAD"					// 검사 결과 불량
#define RESULT_BIN2					"FDB.RESULT.BIN2"					// 검사 결과 BIN2
#define RESULT_BIN3					"FDB.RESULT.BIN3"					// 검사 결과 BIN3 (Active POL 불량)
#define RESULT_EXTERNAL_GOOD		"FDB.RESULT.EXTERNAL.GOOD"			// (내부 메시지)검사 결과 외관 불량 양품
#define RESULT_EXTERNAL_BIN2		"FDB.RESULT.EXTERNAL.BIN2"			// (내부 메시지)검사 결과 외관 불량 인데 BIN2로
#define RESULT_EXTERNAL_BIN3		"FDB.RESULT.EXTERNAL.BIN3"			// (내부 메시지)검사 결과 외관 불량 인데 BIN3로
#define RESULT_EXTERNAL				"FDB.RESULT.EXTERNAL"				// 검사 결과 외관 불량
#define TEACH_MODE_START_OK			"FDB.TEACH.MODE.START.OK"			// 비젼 티칭 모드로 전환 완료
#define TEACH_MODE_END_OK			"FDB.TEACH.MODE.END.OK"				// 비젼 티칭 모드 종료
#define OG_NOT_USE_OK				"FDB.OG.NOT.USE.OK"					// 외관 사용 안함 OK
#define OG_USE_OK					"FDB.OG.USE.OK"						// 외관 사용함 OK
#define REQUEST_OP_INFORM			"FDB.REQUEST.OP"					    // 파트너엠에서 결과 전송시 운영측 정보 생성 요청
#define ALARM						"FDB.ALARM"						  // 파트너엠에서 결과 전송시 운영측 정보 생성 요청
#define ALARM1						"FDB.ALARM1"					    // 파트너엠에서 결과 전송시 운영측 정보 생성 요청
#define ALARM2						"FDB.ALARM2"					    // 파트너엠에서 결과 전송시 운영측 정보 생성 요청
#define POLTURN						"FDB.POLTURN"						// 폴 90도 회전
#define POLZERO						"FDB.POLZERO"						// 폴 0도 복귀 

//jskim 신규 윰검사 Message Define
//Machine -> Vision
#define MTV_REQUEST_STATE								"MTV.REQUEST_STATE"
#define MTV_STATE_AUTO									"MTV.AUTO.STATE"
#define MTV_STATE_TEACH									"MTV.TEACH.STATE"
#define MTV_GRAB_READY									"MTV.GRAB.READY"
#define MTV_GRAB_STRART									"MTV.GRAB.START"
#define MTV_VACUUM_SWITCH_FIRST_OK						"MTV.VACUUM.SWITCH.1"
#define MTV_VACUUM_SWITCH_SECOND_OK						"MTV.VACUUM.SWITCH.2"
#define MTV_VACUUM_SWITCH_THIRD_OK						"MTV.VACUUM.SWITCH.3"
#define MTV_VACUUM_SWITCH_FOURTH_OK						"MTV.VACUUM.SWITCH.4"
#define MTV_VACUUM_SWITCH_FIFTH_OK						"MTV.VACUUM.SWITCH.5"
//Machine->Vision Packing 용 추가
#define MTV_STATE_REQ						"MTV.STATE.REQ"
#define MTV_READY_REQ_F						"MTV.READY.REQ.F"
#define MTV_READY_REQ_B						"MTV.READY.REQ.B"
#define MTV_READY_REQ						"MTV.READY.REQ"
#define MTV_SELECT_RECIPE						"MTV.SELECT.RECIPE"			// 10.29 Recipe Recive
#define MTV_RECIPE_LIST_REQ						"MTV.RECIPE.LIST.REQ"		// 10.29 Recipe Recive
//Vision -> Machine
#define VTM_AUTO_STATE									"VTM.AUTO.STATE"//사용안함.
#define VTM_TEACH_STATE									"VTM.ERR.1"
#define VTM_GRAB_POSSIBLE								"VTM.GRAB.POSSIBLE"//사용안함.
#define VTM_GRAB_IMPOSSIBLE								"VTM.GRAB.IMPOSSIBLE"//사용안함.
#define VTM_GRAB_START_RECEIVE							"VTM.GRAB.START.RECEIVE"
#define VTM_VACUUM_SWITCH_FIRST_REQUEST					"VTM.VACUUM.SWITCH.1"
#define VTM_VACUUM_SWITCH_SECOND_REQUEST				"VTM.VACUUM.SWITCH.2"
#define VTM_VACUUM_SWITCH_THIRD_REQUEST					"VTM.VACUUM.SWITCH.3"
#define VTM_VACUUM_SWITCH_FOURTH_REQUEST				"VTM.VACUUM.SWITCH.4"//사용안함.
#define VTM_VACUUM_SWITCH_FIFTH_REQUEST					"VTM.VACUUM.SWITCH.5"//사용안함.
#define VTM_GRAB_END									"VTM.GRAB.END"
#define VTM_INSPECT_COMPLETE							"VTM.INSPECT.COMPLETE"
#define VTM_INSPECT_RESULT								"VTM.INSPECT.RESULT"
#define VTM_MESSAGE_ERROR								"VTM.ERR.2"
#define VTM_VACUUM_ERROR								"VTM.ERR.3"
#define VTM_DATA_WRITING_COMPLETE						"VTM.DATA.COMPLETE"
///Vision -> Machine Packing 용 추가
#define VTM_ALIVE								"VTM.ALIVE"
#define VTM_STATE								"VTM.STATE"
#define VTM_READY_RES							"VTM.READY.RES"
//#define VTM_GRAB_END							"VTM.GRAB.END"
#define VTM_CLASSIFY_END						"VTM.CLASSIFY.END"
#define VTM_LUMINANCE_REPORT					"VTM.LUMINANCE.REP"
#define VTM_JUDGE_END							"VTM.JUDGE.END"
#define VTM_PROCESS_INTERLOCK					"VTM.PROCESS.INTERLOCK"
#define VTM_RECIPE_SEND							"VTM.RECIPE.SEND"		// 10.29 Recipe List 전송
//jskim 신규 윰검사 내부 프로그램 사용의 Int형
//Machine -> Vision
#define MTV_INTERNAL_REQUEST_STATE						0
#define MTV_INTERNAL_STATE_AUTO							1
#define MTV_INTERNAL_STATE_TEACH						2
#define	MTV_INTERNAL_GRAB_READY							3
#define MTV_INTERNAL_GRAB_START							4
#define	MTV_INTERNAL_VACUUM_STWICH_FIRST_OK				5
#define	MTV_INTERNAL_VACUUM_STWICH_SECOND_OK			6
#define	MTV_INTERNAL_VACUUM_STWICH_THIRD_OK				7
#define	MTV_INTERNAL_VACUUM_STWICH_FOURTH_OK			8
#define	MTV_INTERNAL_VACUUM_STWICH_FIFTH_OK				9
//Machine->Vision Packing 용 추가
#define MTV_INTERNAL_STATE_REQ							10
#define MTV_INTERNAL_READY_REQ_F						11
#define MTV_INTERNAL_READY_REQ_B						12
#define MTV_INTERNAL_READY_REQ							13
// 2015.10.14 Recipe 자동 변경 추가
#define MTV_INTERNAL_CHANGE_RECIPE						14
#define MTV_INTERNAL_SELECT_RECIPE						14		// 10.29 Recipe Recive
#define MTV_INTERNAL_RECIPE_LIST_REQ					15		// 10.29 Recipe Recive

//Vision -> Machine
#define VTM_INTERNAL_AUTO_STATE							0
#define VTM_INTERNAL_TEACH_STATE						1
#define VTM_INTERNAL_GRAB_POSSIBLE						2
#define VTM_INTERNAL_GRAB_IMPOSSIBLE					3
#define VTM_INTERNAL_GRAB_START_RECEIVE					4
#define VTM_INTERNAL_VACUUM_SWITCH_FIRST_REQUEST		5
#define VTM_INTERNAL_VACUUM_SWITCH_SECOND_REQUEST		6
#define VTM_INTERNAL_VACUUM_SWITCH_THIRD_REQUEST		7
#define VTM_INTERNAL_VACUUM_SWITCH_FOURTH_REQUEST		8
#define VTM_INTERNAL_VACUUM_SWITCH_FIFTH_REQUEST		9
#define VTM_INTERNAL_GRAB_END							10
#define VTM_INTERNAL_INSPECT_COMPLETE					11
#define VTM_INTERNAL_INSPECT_RESULT						12
#define VTM_INTERNAL_MESSAGE_ERROR						13
#define VTM_INTERNAL_VACUUM_ERROR						14
#define VTM_INTERNAL_WRITING_COMPLETE					15
//Vision -> Machine Packing 용 추가
#define VTM_INTERNAL_ALIVE								16
#define VTM_INTERNAL_STATE								17
#define VTM_INTERNAL_READY_RES							18
//#define VTM_INTERNAL_GRAB_END							19
#define VTM_INTERNAL_CLASSIFY_END						20
#define VTM_INTERNAL_LUMINANCE_REPORT 					21
// 2015.06.17 NG Flag 전송 추가 - By CWH
#define VTM_INTERNAL_JUDGE_END							22
#define	VTM_INTERNAL_PROCESS_INTERLOCK					23
// 2015.10.29 대형 TFT 측정 값 전송 추가 - By LDH
#define VTM_INTERNAL_MEASURE_DATA						24
#define	VTM_INTERNAL_RECIPE_SEND						25		// 10.29 Recipe List 전송
////////////////////////////////////////////////////////////////////////////////////////////
// 참고 : 검사 없이 물류만 흘릴 경우, 이 정보는 system에서 확인할 것
// 패널 정보 
#define	STAGE_TYPE_ACTIVE	0
#define	STAGE_TYPE_OG		1

// 검사 진행 단계
#define STATE_STAND_BY		0
#define STATE_GRAB			1   // 진행 중
#define STATE_GRAB_DONE		2
#define STATE_PROC			3	// classify for inspection
#define STATE_PROC_DONE		4

//LOG LEVEL
#define LOG_LEVEL_DETAIL	1
#define LOG_LEVEL_BASIC		2
#define LOG_LEVEL_SIMPLE	3

// panel location
#define	POS_INSP_STAGE		1
#define POS_POST_INSP		0	// 다음 패널이 검사 stage에 들어왔을 때

// BIN class
#define BIN_CLASS_GOOD		0		// BIN1
#define BIN_CLASS_REJECT	1		// BIN2
#define BIN_CLASS_REVIEW	2
#define BIN_CLASS_OG_GOOD	3
#define BIN_CLASS_OG_REJECT	4		// LOOK
#define BIN_CLASS_OG_REVIEW	5		// (BIN2) 빛샘 불량

#define BIN_CLASS_BIN3		6		// BIN3 : active pol 불량
#define BIN_CLASS_OG_BIN3	7		// active pol 검사용 -> 별도 bin 관리함

#define KEY_STATE_VIS_CNT				_T("VISION PC CNT")
#define KEY_STATE_EQP_MODEL				_T("EQP_MODEL")
#define KEY_STATE_EQP_NO				_T("EQP_NO")
#define EXTENSION_DAT				    _T(".dat")
#define EXTENSION_TMP				    _T(".tmp")
#define EXTENSION_ANS				    _T(".ans")
#define EXTENSION_JPG					_T(".jpg")
#define EXT_COLON						_T(",")
#define EXT_SEMICOL						_T(";")
#define EQP_STATE						_T("EQP_STATE")

///////////////////////////////////////////////////////////////////////////////////////
//	USER MESSAGE DEFINE
//	각 모듈간 주고 받는 메시지 
//	Message format : WM_(ModuleName)_(Act)

// MainFrame
#define	WM_SYS_INIT_SUCCESS			WM_USER + 101
#define	WM_SYS_INIT_FAIL				WM_USER + 102	

//2014-04-18, jhLee, Initial 동작과정을 알려준다.
#define	WM_SYS_INIT_PROCESS			(WM_USER + 103)				// 초기화 과정
#define	WM_SYS_INIT_EVENT				(WM_USER + 104)				// 초기화 과정중 Evnet 발생 (시작/정지/종료....)
#define	WM_SYS_ALARM_SET				(WM_USER + 300)				// Alarm 발생
#define WM_SYS_INIT_CAM				(WM_USER + 301)					//jskim Camera 초기화 Message




#define WM_SOCKET_VISION_INBOUND		WM_USER + 105
#define WM_SOCKET_VISION_OUTBOUND		WM_USER + 106

#define WM_INSPECT_GRAB_START			WM_USER + 107	
#define WM_INSPECT_GRAB_START_TEST		WM_USER + 155
#define	WM_INSPECT_GRAB_END				WM_USER + 108
#define	WM_INSPECT_GRAB_END_OK			WM_USER + 109
#define	WM_OG_NOT_USE					WM_USER + 110
#define	WM_OG_USE						WM_USER + 111
#define	WM_CLASS_END					WM_USER + 112
#define	WM_ABORT						WM_USER + 113
#define	WM_INSPECT_CLASSIFY_END			WM_USER + 114
#define WM_INSPECT_END_THREAD			WM_USER + 115
#define WM_INSPECT_GRAB_LINE_START		WM_USER + 116
#define WM_INSPECT_END_LINE_THREAD		WM_USER + 117

//TRIGER Setting
#define WM_TRIGGER_START_SET            WM_USER + 118
#define WM_TRIGGER_END_SET				WM_USER + 119

// client request message
#define WM_CL_REQUEST_RESULT			WM_USER + 120		// per panel id
#define WM_CL_RESULT_RECEIVE_OK			WM_USER + 121
#define WM_CL_REQUEST_VISION_STATE		WM_USER + 122		// per stage
#define WM_CL_TEACH_MODE_START			WM_USER + 123		// per system ? per stage?
#define WM_CL_TEACH_MODE_END			WM_USER + 124		// per system ? per stage?
#define WM_CL_MODEL_CHANGE				WM_USER + 125
#define WM_POLTURN_OK					WM_USER + 126
#define WM_POLZERO_OK					WM_USER + 127

// Grab thread
#define WM_USER_LIVE_START				WM_USER + 150		// Grab thread (live mode)
#define WM_USER_GRAB_START				WM_USER + 151		// Grab thread
#define WM_USER_LIVE_2K_START			WM_USER + 152		// Grab thread (live 2k mode)
#define WM_USER_LIVE_8K_START			WM_USER + 153		// Grab thread (live 8k mode)
#define WM_USER_LIVE_LINE_START			WM_USER + 154		// Grab thread (live 8k mode)

// GUI
#define	WM_USER_MOUSE_MOVE				WM_USER + 160
#define	WM_SET_PROGRESS_VALUE			WM_USER + 161
#define	WM_USER_OG_INSP_IMAGE			WM_USER + 162
#define	WM_USER_SET_ORIGIN				WM_USER + 163
#define	WM_USER_SET_CROP_TOPLEFT		WM_USER + 164
#define	WM_USER_SET_CROP_BOTTOMRIGHT	WM_USER + 165
#define WM_SET_GRAB_TIME_VALUE			WM_USER + 166
#define WM_SET_CLASS_TIME_VALUE         WM_USER + 167

#define WM_USER_MANUAL_INSP_IMAGE		WM_USER + 168
#define WM_SET_INSPECT_INFO_VALUE		WM_USER + 169

// 2015.10.15 Recipe 자동 변경 메세지 추가
#define WM_SET_INSPECT_RECIPE			WM_USER + 170

// Smith
#define WM_ERROR_MODULE				WM_USER + 401

//// SOCKET MESSAGE //////
#define WM_FIRACT_ACCEPT_SOCKET		WM_USER + 501
#define WM_SNDACT_ACCEPT_SOCKET		WM_USER + 502
#define WM_EXT_ACCEPT_SOCKET		WM_USER + 503

#define WM_FIRACT_RECEIVE_DATA		WM_USER + 601
#define WM_SNDACT_RECEIVE_DATA		WM_USER + 602
#define WM_EXT_RECEIVE_DATA			WM_USER + 603

// UI 
#define WM_SWITCH_VIEW				WM_USER + 701
#define WM_DRAW_DEFECT				WM_USER + 702

// 통신 상태 연결, 해지 표시
#define WM_FIRACT_STATE				WM_USER + 800
#define WM_SNDACT_STATE				WM_USER + 801
#define WM_EXT_STATE				WM_USER + 802

// 각 통신 메시지 상태 표시
#define WM_FIRACT_STATE_MSG				WM_USER + 901
#define WM_SNDACT_STATE_MSG				WM_USER + 902
#define WM_EXT_STATE_MSG				WM_USER + 903
#define WM_RESULT_STATE_MSG				WM_USER + 904
#define WM_OG_STATE_USE					WM_USER + 905
#define WM_ABORT_STATE_MSG				WM_USER + 906

// White Pattern 표시 업데이트
#define WM_USER_UPDATE_WHITE_MEAN				WM_USER + 951
#define WM_USER_MANUAL_UPDATE_WHITE_MEAN		WM_USER + 952

// Uf insp Thrd message
#define WM_SET_SPOT_BIN_IMAGE		WM_USER + 1000
#define WM_SET_GRAY_BIN_IMAGE		WM_USER + 1001
#define WM_SET_WHITE_BIN_IMAGE		WM_USER + 1002
#define WM_SET_BLACK_BIN_IMAGE		WM_USER + 1003
#define WM_SET_POLT_BIN_IMAGE		WM_USER + 1004
#define WM_SET_POLB_BIN_IMAGE		WM_USER + 1005
#define WM_SET_DUST_BIN_IMAGE		WM_USER + 1006
#define WM_SET_GRAY2_BIN_IMAGE		WM_USER + 1007

#define WM_INIT_INSPECTION			WM_USER + 1010
#define WM_EXIT_INSPECTION			WM_USER + 1011
#define WM_START_SPOT_INSPECTION	WM_USER + 1012
#define WM_START_GRAY_INSPECTION	WM_USER + 1013
#define WM_START_BLACK_INSPECTION	WM_USER + 1014
#define WM_START_WHITE_INSPECTION	WM_USER + 1015
#define WM_SET_SAVE_IMAGE			WM_USER + 1016
#define WM_START_INSPECTION			WM_USER + 1017
#define WM_START_GRAY2_INSPECTION	WM_USER + 1018

//jskim Inspection 종료후 Log저장 Message
#define WM_START_LOG_SAVE			WM_USER + 1020
//jskim
#define WM_LOG_AUTO_DELETE			WM_USER	+ 1021

#define WM_GET_VIEW_POINT			WM_USER	+ 1022

#define WM_SEND_MESSAGE				WM_USER	+ 1023

#define WM_SHOW_LOG_DATA			WM_USER	+ 1024
#define WM_SET_USER_CHANGE			WM_USER	+ 1025		//2015.07.17 add by mby

#define VIEW_INIT					100
#define VIEW_AUTO					101
#define VIEW_MANUAL					102
#define VIEW_TEACH					103
#define VIEW_SOCKET					104

#define EQUI_AVT					200
#define EQUI_AFT					201
#define EQUI_AMT					202

// CSystem -> MainFrm WM_COPYDATA Messages
#define SYS_MSG_LOG_SYSTEM				1000
#define SYS_MSG_LOG_TEST				2000
#define SYS_MSG_LOG_INSP_TOT			3000
#define SYS_MSG_LOG_INSP_USER			4000
#define SYS_MSG_SAME_DEFECTS			5000
#define SYS_MSG_CLEAR_LOG				9999

// enum ---------------------------------------------------------

enum
{
	GRID_LOG_SYSTEM = 0,
	GRID_LOG_TEST,
	GRID_LOG_INSP_TOT,
	GRID_LOG_INSP_USER,
	GRID_LOG_MAX
};

enum ENUM_ALGORITHM_NUM
{
	E_ALGORITHM_ACTIVE = 0,			// 00 Active
	E_ALGORITHM_EDGE,				// 01 Edge
	E_ALGORITHM_PAD,				// 02 Pad
	E_ALGORITHM_SCRATCH,			// 03 Scratch
	E_ALGORITHM_LUMINANCE,			// 04 Luminance
	E_ALGORITHM_MEASURE,			// 05 Measure
	E_ALGORITHM_EMPTY2,				// 06 Empty 2
	E_ALGORITHM_REFLECTION_ACTIVE,	// 07 Reflection Active
	E_ALGORITHM_ACTIVE_DIMPLE,		// 08 Active Dimple
	E_ALGORITHM_PAD_SCRATCH,		// 09 Pad Scratch
	E_ALGORITHM_SCRATCH2,			// 10 Scratch2
	E_ALGORITHM_TRANSMISSION,		// 11 Transmission
	E_ALGORITHM_PAD2,				// 12 PAD2
	E_ALGORITHM_CORNER,				// 13 TV Line CORNER
	E_ALGORITHM_DIAGONAL,			// 14 TV Line DIAGONAL
	E_ALGORITHM_VIBRATION,			// 15 TV Line VIBRATION
	E_ALGORITHM_NUMBER				// 16 Total
};

// Corner R/C Cut Alarm
#define CornerShapeError 3354	// R/C Cut Corner Shape Error
#define CornerSizeError  3355	// R/C Cut Corner Size Error

// 휘도값 알람 코드
#define LUMINANCE_ALRAM_CODE_CFC 3356	// CF Forward Coaxial
#define LUMINANCE_ALRAM_CODE_CBC 3357	// CF Backward Coaxial
#define LUMINANCE_ALRAM_CODE_CFS 3358	// CF Forward Side
#define LUMINANCE_ALRAM_CODE_CBS 3359	// CF Backward Side
#define LUMINANCE_ALRAM_CODE_TFC 3360	// TFT Forward Coaxial
#define LUMINANCE_ALRAM_CODE_TBC 3361	// TFT Backward Coaxial
#define LUMINANCE_ALRAM_CODE_TFS 3362	// TFT Forward Side
#define LUMINANCE_ALRAM_CODE_TBS 3363	// TFT Backward Side
#define LUMINANCE_ALRAM_CODE_TDI 6125	// TDI

#define NUM_OF_MAX_IMAGE		30
#define NUM_OF_MAX_CONTROLLER	4
#define MAX_CHANNEL_SIZE		12	//12->2
//#define MAX_CHANNEL_SIZE		16

//#define DEFECT_TOTAL_COUNT	E_ALGORITHM_NUMBER
#define ALG_PARA_TOTAL_COUNT	80		// 알고리즘 파라미터 Max 40개
#define ALG_PARA_ADD_COUNT		9		// 기타 알고리즘 파라미터 ( ROI Number, Image Save Flag, Thread ID 등등 )

// --------------------------------------------------------------

///////////// SECS 통신 SEND_ Message DEFINE //////////////////
/*
#define AU_READY		"S1F1"
#define SEND_ERROR		"S5F1"
#define SEND_OCR		"S6F11"
#define SEND_EQIP_ONOFF	"S6F11_2"
#define SEND_TOTAL_INFO	"S6F13"

#define I_HEAR			"S1F2"
#define RE_PROD_INFO	"S1F6"    // S1F5 올때
#define RE_EQIP_OFF		"S1F16"   // 설비 OFF요구시
#define RE_EQIP_ON		"S1F18"   // 설비 ON요구시
#define RE_LOT_INFO		"S2F42"   // 설비 LOT정보 요구시
*/

///////////// Error Code DEFINE //////////////////
// [TODO] 별도의 에러 관리 : 파일 혹은 리소스 사용
/*#define ERR_EX_VAC_TBL_S	"E0-00"
#define ERR_EX_VAC_TBL_D	"E0-01"
#define ERR_EX_VAC_POST_S	"E0-02"
#define ERR_EX_VAC_POST_D	"E0-03"
#define ERR_EX_VAC_PRE_S	"E0-11"
#define ERR_EX_VAC_PRE_D	"E0-12"
#define ERR_EX_TBL_HAND_UP	"E0-20"

#define ERR_SENS_POST		"E1-00"
#define ERR_SENS_PRE		"E1-10"

#define ERR_UNLD_SENS_POST	"E2-00"
#define ERR_UNLD_VAC_POST	"E2-01"
#define ERR_UNLD_VAC_CST	"E2-02"
#define ERR_LD_VAC_CST		"E2-03"
#define ERR_LD_AXIS_T		"E2-10"
#define ERR_LD_AXIS_X		"E2-11"
#define ERR_LD_AXIS_Z		"E2-12"

#define ERR_CST_UNLD		"E3-00"
#define ERR_CST_LD			"E3-01"
#define ERR_CST_UPDN		"E3-10"
#define ERR_CST_SIZE		"E3-11"
#define ERR_CST_MISS		"E3-12"
#define ERR_CST_POS			"E3-13"
#define ERR_CST_LOCK		"E3-14"

#define ERR_AUTO_VIEW		"E4-00"
#define ERR_AUTO_AXIS_FAULT	"E4-01"
#define ERR_AUTO_INIT_POS	"E4-02"
#define ERR_AUTO_CHNG		"E4-10"
#define ERR_DATA_NO_PNL		"E4-20"
#define ERR_INSP_FAIL		"E4-30"
#define ERR_INSP_DISP		"E4-31"
#define ERR_INSP_PATGEN		"E4-40"

#define ERR_OCR_NO_FILE		"E5-00"
#define ERR_OCR_FONT		"E5-01"
#define ERR_OCR_INPUT		"E5-02"
#define ERR_OCR_LENGTH		"E5-03"

#define ERR_INIT_CAM		"E6-00"
#define ERR_INIT_DETECT		"E6-10"

#define ERR_NMC_PHASE		"E7-00"
#define ERR_AXIS_FAIL		"E7-10"
#define ERR_AXIS_TIMEOUT	"E7-11"
#define ERR_AXIS_FAULT		"E7-12"
#define ERR_AXIS_RESET		"E7-13"
#define ERR_NMC_READ		"E7-10"

#define ERR_SECS_CON		"E8-00"
#define ERR_SECS_DISCON		"E8-01"
#define ERR_SECS_SELECT		"E8-02"
#define ERR_SECS_STATE		"E8-03"

#define ERR_USER_LOGIN		"E9-00"

#define ERR_MODEL_OPEN		"EA-00"
#define ERR_MODEL_DELETE	"EA-01"
*/
#define WM_SOCKDATA_CONNECT				(WM_USER + 52)
#define WM_SOCKDATA_DISCONNECT			(WM_USER + 53)
#define WM_SOCKDATA_RECEIVE				(WM_USER + 54)
#define WM_SOCKDATA_ACCEPT				(WM_USER + 55)
#define	WM_PLC_CONNECT_EVENT			(WM_USER + 56)
#define	WM_PLC_DISCONNECT_EVENT			(WM_USER + 57)
#define WM_PLC_LOG_EVENT				(WM_USER + 58)
#define WM_ASCII_SEND_EVENT				(WM_USER + 59)
#define WM_DIGITAL_SEND_EVENT			(WM_USER + 60)


// 다운로드 된 패턴 정보관련
#define MSG_NUM				1
#define MSG_KEYLOCKTIME		0
#define MSG_PATTERN_TITLE	1
#define MSG_PATTERN_SIZE	2

////////////////////////////////////////////////////////////////////////////
// 다운로드 된 패턴정보 저장
typedef struct _Shared
{
	DWORD Number;
	char Text[20][256];
} SharedData;

////////////////////////////////////////////
#define DEFECT_CODE_HIGH_MANY		"A401"
#define DEFECT_CODE_HIGH_NEAR		"A504"
#define DEFECT_CODE_OFF_NEAR		"A509"
#define DEFECT_CODE_PIXEL_MISC		"A514"
#define DEFECT_CODE_HIGH_OVER		"A518"
#define DEFECT_CODE_CD_SHORT		"B601"
#define DEFECT_CODE_CG_SHORT		"B602"
#define DEFECT_CODE_GATE_OPEN		"B606"
#define DEFECT_CODE_DATA_OPEN		"B607"
#define DEFECT_CODE_GATE_DEFECT		"B608"
#define DEFECT_CODE_DATA_DEFECT		"B609"
#define DEFECT_CODE_GFO				"B613"
#define DEFECT_CODE_DFO				"B614"

#define DEFECT_CODE_DISPLAY			"D714"
#define DEFECT_CODE_IMUL			"D711"
#define DEFECT_CODE_SPOT			"D716"
#define DEFECT_CODE_GBD				"D718"
#define DEFECT_CODE_DBD				"D719"
#define DEFECT_CODE_SPACER			"D723"
#define DEFECT_CODE_GAPS			"D722"
#define DEFECT_CODE_JUIPGU			"D709"
#define DEFECT_CODE_SEJUNG			"D704"
#define DEFECT_CODE_SCRATCH			"D749"
#define	DEFECT_CODE_HOR				"D725"
#define	DEFECT_CODE_14VLOW			"D769"
#define	DEFECT_CODE_UF				"D744"



#define SDEFECT_CODE_HIGH_MANY		"HIGH-다발"
#define SDEFECT_CODE_HIGH_NEAR		"H-인접"
#define SDEFECT_CODE_OFF_NEAR		"OFF-인접"
#define SDEFECT_CODE_OFF_OVER		"OFF"
#define SDEFECT_CODE_OFF_2OVER		"OFF 2 이상"
#define SDEFECT_CODE_PIXEL_MISC		"PIXEL 기타"
#define SDEFECT_CODE_HIGH_OVER		"HIGH OVER"
#define SDEFECT_CODE_HIGH_2OVER		"HIGH 2 이상"
#define SDEFECT_CODE_HIGH_MISC		"HIGH 기타"
#define SDEFECT_CODE_OFF_MISC		"OFF 기타"
#define SDEFECT_CODE_GATE_DEFECT	"G-DEFECT"
#define SDEFECT_CODE_DATA_DEFECT	"D-DEFECT"
#define SDEFECT_CODE_SCRATCH		"SCRATCH"
#define SDEFECT_CODE_IMUL			"이물"
#define SDEFECT_CODE_SPOT			"SPOT"
#define SDEFECT_CODE_GBD			"GATE BLOCK DEFECT"
#define SDEFECT_CODE_DBD			"DATA BLOCK DEFECT"
#define SDEFECT_CODE_BD				"BLOCK DEFECT"
#define SDEFECT_CODE_SPACER			"SPACER 눌림"
#define SDEFECT_CODE_GAPS			"GAP"
#define SDEFECT_CODE_JUIPGU			"주입구불량"
#define SDEFECT_CODE_SEJUNG			"세정액 침투"
#define SDEFECT_CODE_HOR			"가로줄얼룩"
#define SDEFECT_CODE_14VLOW			"14V 저온불량"
#define SDEFECT_CODE_FAIL			"Inspection Fail"
#define SDEFECT_CODE_DISPLAY		"Display불량"
#define SDEFECT_CODE_FAIL_MODEL		"Inspection Fail(Model Error)"
#define SDEFECT_CODE_FAIL_SYSTEM	"Inspection Fail(System Error)"
#define SDEFECT_CODE_MICRO_GDEFECT	"GATE DEFECT"
#define SDEFECT_CODE_MICRO_DDEFECT	"DATA DEFECT"
#define SDEFECT_CODE_UF				"UF 불량"
#define SDEFECT_CODE_IMUL_HIGH		"이물 HIGH"
#define SDEFECT_CODE_PTRN_ERR		"패턴불량"
#define SDEFECT_CODE_DIAG_ULUK		"사선얼룩"
#define SDEFECT_CODE_UF_IMUL		"UF이물"
#define SDEFECT_CODE_WHITE_POINT	"백점"

//REJECT 
#define SDEFECT_CODE_CD_SHORT		"C/D SHORT"
#define SDEFECT_CODE_CG_SHORT		"C/G SHORT"
#define SDEFECT_CODE_GATE_OPEN		"G/OPEN"
#define SDEFECT_CODE_DATA_OPEN		"D/OPEN"
#define SDEFECT_CODE_GFO			"GFO"
#define SDEFECT_CODE_DFO			"DFO"


//////////////////////////////
// COSMETIC DEFECT CODE

// final defect type
#define INSP_RESULT_OG_AM_DAMAGE		700
#define INSP_RESULT_OG_AM_DAMAGE_ALL	701
#define INSP_RESULT_OG_PAD_BROKEN		702
#define INSP_RESULT_OG_PAD_CHIPPING		703
#define INSP_RESULT_OG_BME				704
#define INSP_RESULT_OG_BMC				705
#define	INSP_RESULT_OG_CUTTING			706
#define INSP_RESULT_OG_LIGHT_LEAK		707     // Crack 정보와 같이 사용.
#define	INSP_RESULT_OG_CHIPPING			708		// 위치 무관함 (F/L 조명하에서 검출) // not used
#define INSP_RESULT_OG_BM_BROKEN		709
#define INSP_RESULT_OG_UNCLASSIFIED		710
#define INSP_RESULT_OG_BM_AREA_FAIL		711		// BM area detection fail
#define INSP_RESULT_OG_ALIGN_FAIL		712		// align fail
#define INSP_RESULT_OG_POL_ALIGN_MISS	713
#define INSP_RESULT_OG_POL_DETACH		714		// POL 들뜸
#define INSP_RESULT_OG_PAD_CUTTING		715		// PG 잔존
#define INSP_RESULT_OG_ACTIVE_POL		716		// ACTIVE POL 불량
#define INSP_RESULT_OG_BM_CF			717		// BM 색이상
#define INSP_RESULT_OG_OVER_SI			718		// SI 과도포 불량
#define INSP_RESULT_OG_EMPTY_SI			719		// SI 미도포 불량.
#define INSP_RESULT_OG_GOOD				720
#define INSP_RESULT_OG_PCB			    730		//PCB BONDIGN 불량
#define INSP_RESULT_OG_TAB			    731		//PCB TAB IC 불량



// OG defect classification
#define DEFECT_TYPE_OG_UNCLASSIFIED				50
//#define DEFECT_TYPE_OG_ALIGN_MARK_ALL			51
#define DEFECT_TYPE_OG_ALIGN_MARK				52
#define DEFECT_TYPE_OG_PG_CHIPPING_OR_CRACK		53
#define DEFECT_TYPE_OG_PG_BROKEN				54
#define DEFECT_TYPE_OG_POL_ALIGN				55
#define DEFECT_TYPE_OG_BM_LIGHT_LEAK			56
#define DEFECT_TYPE_OG_BM_CHIPPING_OR_CRACK		57
#define DEFECT_TYPE_OG_BM_BROKEN				58
#define DEFECT_TYPE_OG_BM_CUTTING_MISS			59	
#define DEFECT_TYPE_OG_BM_THICKNESS				60	// not used yet
#define DEFECT_TYPE_OG_BM_THICKNESS_INJECTION	61	// not used yet
#define DEFECT_TYPE_OG_BM_LINE					62
#define DEFECT_TYPE_OG_BM_CORNER				63
#define DEFECT_TYPE_OG_CHIPPING					64	// 위치 무관함 (F/L 조명하에서 검출) // not used
#define DEFECT_TYPE_OG_BM_LIGHT_LEAK_SKIP		65	// BM area detection fail -> 빛샘 검사 못함
#define DEFECT_TYPE_OG_BM_POL_DETACH			66	// Pol 들뜸
#define DEFECT_TYPE_OG_PG_CUTTING				67	// PG 잔존
#define DEFECT_TYPE_OG_ACTIVE_POL				68	// Active POL 불량
#define DEFECT_TYPE_OG_BM_CF					69	// BM 색이상
#define DEFECT_TYPE_OG_OVER_SI					70	// 상부 SI 도포 불량
#define DEFECT_TYPE_OG_EMPTY_SI					71	// pcb 불량.
#define DEFECT_TYPE_OG_PCB						72	// TAB 불량
#define DEFECT_TYPE_OG_TAB						73	// 하부 SI 도포 불량.
#define DEFECT_TYPE_OG_GOOD                     74


//jskim 윰검사 불량 Define
#define MAIN_STATS_NUM							3
#define STATS_OF_TOTAL							0
#define STATS_OF_GOOD							1
#define STATS_OF_DEFECT							2

#define DEFECT_ALL_SORT							18
#define DEFECT_OF_INSPECTION_FAIL				0
#define DEFECT_OF_UPPER_FOREIGN					1
#define DEFECT_OF_UPPER_STAB					2
#define DEFECT_OF_LOWER_BUBBLE					3
#define DEFECT_OF_LOWER_ABNOMAL					4		
#define DEFECT_OF_SPOT							5
#define DEFECT_OF_CIRCULAR_SPOT					6
#define DEFECT_OF_MONOMER_BIND					7
#define DEFECT_OF_FILM_BREAK					8
#define DEFECT_OF_WAVE							9
#define DEFECT_OF_FILM_EXFOLIATION				10
#define DEFECT_OF_PAD_BURNT						11
#define DEFECT_OF_PAD_SCRATCH					12
#define DEFECT_OF_PAD_ABNOMAL					13
#define DEFECT_OF_PAD_STAB						14
#define DEFECT_OF_CUTTING						15
#define DEFECT_OF_PAD_TEAR						16
#define DEFECT_OF_C_CUT							17
//________________________________________________________________________________________

//jskim 불량 Name Define
#define MAX_OF_DEFECT_CODE							19
#define SDEFECT_CODE_OF_GOOD						"Good"
#define SDEFECT_CODE_OF_INSPECTION_FAIL				"FAIL"
#define SDEFECT_CODE_OF_UPPER_FOREIGN				"상부이물"
#define SDEFECT_CODE_OF_UPPER_STAB					"상부찍힘"
#define SDEFECT_CODE_OF_LOWER_BUBBLE				"하부기포"
#define SDEFECT_CODE_OF_LOWER_ABNOMAL				"하부이상"
#define SDEFECT_CODE_OF_SPOT						"얼룩"
#define SDEFECT_CODE_OF_CIRCULAR_SPOT				"원형얼룩"
#define SDEFECT_CODE_OF_MONOMER_BIND				"Monomer 뭉침"
#define SDEFECT_CODE_OF_FILM_BREAK					"Film 꺾임"
#define SDEFECT_CODE_OF_WAVE						"WAVE"
#define SDEFECT_CODE_OF_FILM_EXFOLIATION			"탈막"
#define SDEFECT_CODE_OF_PAD_BURNT					"Pad Burnt"
#define SDEFECT_CODE_OF_PAD_SCRATCH					"Pad Scratch"
#define SDEFECT_CODE_OF_PAD_ABNOMAL					"Pad 이상"
#define SDEFECT_CODE_OF_PAD_STAB					"Pad 찍힘"
#define SDEFECT_CODE_OF_CUTTING						"Cutting 불량"
#define SDEFECT_CODE_OF_PAD_TEAR					"Pad 뜯김"
#define SDEFECT_CODE_OF_C_CUT						"C-Cut 불량"

#define DEFECT_CODE_ALL_SORT						19
#define DEFECT_CODE_OF_GOOD							0
#define DEFECT_CODE_OF_INSPECTION_FAIL				1
#define DEFECT_CODE_OF_UPPER_FOREIGN				2
#define DEFECT_CODE_OF_UPPER_STAB					3
#define DEFECT_CODE_OF_LOWER_BUBBLE					4
#define DEFECT_CODE_OF_LOWER_ABNOMAL				5		
#define DEFECT_CODE_OF_SPOT							6
#define DEFECT_CODE_OF_CIRCULAR_SPOT				7
#define DEFECT_CODE_OF_MONOMER_BIND					8
#define DEFECT_CODE_OF_FILM_BREAK					9
#define DEFECT_CODE_OF_WAVE							10
#define DEFECT_CODE_OF_FILM_EXFOLIATION				11
#define DEFECT_CODE_OF_PAD_BURNT					12
#define DEFECT_CODE_OF_PAD_SCRATCH					13
#define DEFECT_CODE_OF_PAD_ABNOMAL					14
#define DEFECT_CODE_OF_PAD_STAB						15
#define DEFECT_CODE_OF_CUTTING						16
#define DEFECT_CODE_OF_PAD_TEAR						17
#define DEFECT_CODE_OF_C_CUT						18
//_________________________________________________________________________________________

//jskim Classifycation Result
#define MAX_CLASSIFYCATION_SORT						3
#define CLASSIFYCATION_OK							0
#define CLASSIFYCATION_REJECT						1
#define CLASSIFYCATION_BIN2							2

#define SCODE_CLASSIFYCATION_OK						"OK"
#define SCODE_CLASSIFYCATION_BIN2					"BIN2"
#define SCODE_CLASSIFYCATION_REJECT					"REJECT"
//____________________________________________________________________________
// 불량명
#define SDEFECT_CODE_OG_FAIL				"Inspection Fail"
#define SDEFECT_CODE_OG_FAIL_MODEL			"Inspection Fail(Model Error)"
#define SDEFECT_CODE_OG_FAIL_SYSTEM			"Inspection Fail(System Error)"
#define SDEFECT_CODE_OG_DISPLAY				"영상 이상"
#define SDEFECT_CODE_OG_UNCLASSIFIED		"이름없는불량"

#define SDEFECT_CODE_OG_AM_DAMAGE			"Align Mark 손상"
#define SDEFECT_CODE_OG_AM_DAMAGE_ALL		"Align Mark 손상(ALL)"
#define SDEFECT_CODE_OG_PAD_BROKEN			"PG Broken"
#define SDEFECT_CODE_OG_PAD_CHIPPING		"PG Chipping"
#define SDEFECT_CODE_OG_BME					"BM Edge"
#define SDEFECT_CODE_OG_BM_BROKEN			"BM Broken"
#define SDEFECT_CODE_OG_BMC					"BM Corner"
#define	SDEFECT_CODE_OG_CUTTING				"BM Glass 잔존"
#define SDEFECT_CODE_OG_LIGHT_LEAK			"BM 빛샘"
#define	SDEFECT_CODE_OG_CHIPPING			"Chipping"				// 위치 무관함 (F/L 조명하에서 검출)
#define SDEFECT_CODE_OG_BM_AREA_FAIL		"BM 빛샘 검사 Fail"		// BM area detection fail
#define SDEFECT_CODE_OG_ALIGN_FAIL			"Align 이상"
#define SDEFECT_CODE_OG_POL_ALIGN_MISS		"POL Align Miss"
//#define SDEFECT_CODE_OG_POL_DETACH		"POL 들뜸"
#define SDEFECT_CODE_OG_POL_DETACH			"POL 불량"	 
#define SDEFECT_CODE_OG_PAD_CUTTING			"Pad Glass 잔존"
#define SDEFECT_CODE_OG_ACTIVE_POL			"Active POL 불량"
#define SDEFECT_CODE_OG_BM_CF				"BM 색이상"
#define SDEFECT_CODE_OG_OVER_SI				"실리콘 과도포불량"
#define SDEFECT_CODE_OG_EMPTY_SI			"실리콘 미도포불량"
#define SDEFECT_CODE_OG_PCB					"PCB 불량"
#define SDEFECT_CODE_OG_TAB					"TAB IC 불량"
#define SDEFECT_CODE_OG_GOOD				"Good"


///////// OG DEFECT CODE 
#define DEFECT_CODE_BROKEN				"GMBR" //FAB성 BROKEN
#define DEFECT_CODE_CHIPPING			"GMCH" //FAB성 CHIPPING
#define DEFECT_CODE_CRACK				"GMCR" //FAB성 CRACK
#define DEFECT_CODE_EDGE_BROKEN			"GMBR"
#define DEFECT_CODE_PAD_CHIPPING		"GMCH"
#define DEFECT_CODE_MC_BROKEN			"GMBR"
#define DEFECT_CODE_ALIGN_MISS  		"D715"
#define DEFECT_CODE_OG_UNCLASSIFY		"TTSI"
#define DEFECT_CODE_OG_DISPLAY			"FAIL"  

#define DEFECT_CODE_PCB_CRACK			"MCCR" // CONNECT CRACK
#define DEFECT_CODE_PCB_LABEL			"TSPA" // S.PBA 외관불량

#define DEFECT_CODE_UPPER_POL_BMUP		"PCUP" // 상판 POL BM 들뜸
#define DEFECT_CODE_UPPER_POL_SC		"PCSC" // 상판 POL SC
#define DEFECT_CODE_UPPER_POL_TILT		"PMIS" // 상판 POL MISALIGN 
#define DEFECT_CODE_UPPER_POL_UP		"PBUB" // 상판 POL 들뜸
#define DEFECT_CODE_UPPER_POL_UNATTACH	"PBUB" // 상판 POL 미부착
#define DEFECT_CODE_UPPER_POL_ULUK		"PTNT" // 상판 POL 얼룩
#define DEFECT_CODE_UPPER_POL_MISS		"PUDI" // 상판 POL 오부착
#define DEFECT_CODE_UPPER_POL_CHIPPING	"PCDN" // 상판 POL 찍힘
#define DEFECT_CODE_UPPER_POL_JUMP		"PTAM" // 상판 POL 올라탐

#define DEFECT_CODE_UNDER_POL_UP		"PBBU" // 하판 POL 들뜸
#define DEFECT_CODE_UNDER_POL_SC		"PCBC" // 하판 POL SC
#define DEFECT_CODE_UNDER_POL_TILT		"PBMI" // 하판 POL MISALIGN
#define DEFECT_CODE_UNDER_POL_UNATTACH	"PBMA" // 하판 POL 미부착
#define DEFECT_CODE_UNDER_POL_ULUK		"PBTN" // 하판 POL 얼룩
#define DEFECT_CODE_UNDER_POL_MISS		"PBDI" // 하판 POL 오부착
#define DEFECT_CODE_UNDER_POL_CHIPOING	"PCBN" // 하판 POL 찍힘
#define DEFECT_CODE_UNDER_POL_JUMP		"PTAM" // 하판 POL 올라탐

#define DEFECT_CODE_OG_OVER_SI			"TSIO"  //과도포
#define DEFECT_CODE_OG_EMPTY_SI			"TSIM"  //미도포

#define DEFECT_CODE_TAB_BONDING			"TPBN"  //PCB BONDIGN 불량
#define DEFECT_CODE_TAB_MISS			"TGDE"  //PCB MISSING
#define DEFECT_CODE_TAB_CRACK			"TSPA"  //S.PBA 외관불량
#define DEFECT_CODE_TAB_UNATTACH		"TGPB"  //TAB IC 미부착
#define DEFECT_CODE_TAB_CHIPPING		"TDNT"  //TAB 찍힘/접힘

// OG DEFECT CODE 
//#define DEFECT_CODE_BROKEN		"LG03"
//#define DEFECT_CODE_CHIPPING      "LG04"
//#define DEFECT_CODE_CRACK			"LG05"
//#define DEFECT_CODE_EDGE_BROKEN	"LG06"
//#define DEFECT_CODE_PAD_CHIPPING	"LG07"
//#define DEFECT_CODE_MC_BROKEN		"LM01"
//#define DEFECT_CODE_ALIGN_MISS  	"D715"
//#define DEFECT_CODE_OG_UNCLASSIFY	"LG02"
//#define DEFECT_CODE_OG_SI			"LS01"


//////// 픽셀 검사에서 사용.---> 외관에서는 사용안함.

///////// UF Defect Result 
#define INSP_RESULT_NONE		-99		// 검사 안 함(결과 없음)

#define INSP_RESULT_GOOD		0
#define INSP_RESULT_DISPLAY		-10
#define INSP_RESULT_FAIL		-1
#define INSP_RESULT_SYSTEM_ERR	-5
#define INSP_RESULT_MODEL_ERR	-7
#define INSP_RESULT_BD			-2
#define INSP_RESULT_14V			-3
#define INSP_RESULT_DD			-4
#define INSP_RESULT_PIXEL		1
#define INSP_RESULT_MANY_HIGH	2
#define INSP_RESULT_ONE_HIGH	11
#define INSP_RESULT_TWO_HIGH	12
#define INSP_RESULT_HIGH_NEAR	13
#define INSP_RESULT_ONE_OFF		14
#define INSP_RESULT_TWO_OFF		15
#define INSP_RESULT_OFF_NEAR	16
#define INSP_RESULT_HIGH_MISC	17
#define INSP_RESULT_OFF_MISC	18
#define INSP_RESULT_LINE		5
#define INSP_RESULT_DARK_LINE	6
#define INSP_RESULT_UF			7
#define INSP_RESULT_LC_DEFECT	21
#define INSP_RESULT_DFO			501
#define INSP_RESULT_CD_SHORT	502
#define INSP_RESULT_DATA_OPEN	503
#define INSP_RESULT_GFO			504
#define INSP_RESULT_CG_SHORT	505
#define INSP_RESULT_GATE_OPEN	506
#define INSP_RESULT_LEAK		-25
#define INSP_RESULT_GRAD_DEFECT	-26
#define INSP_RESULT_MICRO_DDEFECT	507
#define INSP_RESULT_MICRO_GDEFECT	508
#define INSP_RESULT_IMUL_HIGH		509

////////////////// U/F ////////////////////////

#define INSP_RESULT_SPOT			41
#define INSP_RESULT_GBD				-42
#define INSP_RESULT_DBD				-43
#define INSP_RESULT_SPACER			44
#define INSP_RESULT_GAPS			45
#define INSP_RESULT_JUIPGU			46
#define INSP_RESULT_SEJUNG			-47
#define INSP_RESULT_CROSSTALK		48
#define INSP_RESULT_HOR_DEFECT		49
#define INSP_RESULT_SCRATCH			60
#define INSP_RESULT_PATTERN_ERROR	61
#define INSP_RESULT_DATA_DEFECT		62
#define INSP_RESULT_GATE_DEFECT		63
#define INSP_RESULT_VER_ULUK		64
#define INSP_RESULT_HOR_ULUK		65
#define INSP_RESULT_DARK_SPOT		66
#define INSP_RESULT_GAPA			67
#define INSP_RESULT_DIAGO_ULUK		68
#define INSP_RESULT_RDC				69
#define INSP_RESULT_FINE_LINE_ULUK	70
#define INSP_RESULT_CAM_MODULE		71
#define INSP_RESULT_BL_IMUL			72
#define INSP_RESULT_WHITE_SPOT		73
#define INSP_RESULT_BL_OFF			74
#define INSP_RESULT_BRIGHTNESS		75
#define INSP_RESULT_4INVERT			76
#define INSP_RESULT_IMUL			77
#define INSP_RESULT_WHITE_POINT		78

#define INSP_RESULT_NA			999		// 검사 결과 없음

//////////////////////////////

#define DEFECT_LINE				1
#define DEFECT_DOT				2
#define DEFECT_LINE_VERTICAL	3
#define DEFECT_RECT				4
#define DEFECT_RECT_TR			5
#define DEFECT_ELLIPSE			6

#define DEFECT_TYPE_DOT			7
#define DEFECT_TYPE_HLINE		8
#define DEFECT_TYPE_VLINE		9
#define DEFECT_TYPE_CD_SHORT	10
#define DEFECT_TYPE_DATA_OPEN	11
#define DEFECT_TYPE_CG_SHORT	12
#define DEFECT_TYPE_GATE_OPEN	13
#define DEFECT_TYPE_DATA_DEFECT 14
#define DEFECT_TYPE_GATE_DEFECT 15
#define DEFECT_TYPE_MISC		16
#define DEFECT_TYPE_HNEAR		17
#define DEFECT_TYPE_VNEAR		18
#define DEFECT_TYPE_DNEAR		19
#define DEFECT_TYPE_SPOT		20
#define DEFECT_TYPE_POL			21
#define DEFECT_TYPE_DATA_LINE   22
#define DEFECT_TYPE_GATE_LINE	23
#define DEFECT_TYPE_IMUL		24

#define DEFECT_TYPE_14VLOW			25
#define DEFECT_TYPE_BLOCK_DEFECT	26
#define DEFECT_TYPE_DISPLAY_ERROR	27
#define DEFECT_TYPE_UF_DATA_DEFECT	28
#define DEFECT_TYPE_UF_RDC			29
#define DEFECT_TYPE_UF_SEJUNG		30
#define DEFECT_TYPE_UF_SPACER		31
#define DEFECT_TYPE_PTN_SCRATCH		32
#define DEFECT_TYPE_GAP_DARK		33
#define DEFECT_TYPE_GAP_HIGH		34
#define DEFECT_TYPE_UF_DARK			35
#define DEFECT_TYPE_UF_HIGH			36
#define DEFECT_TYPE_SPOT_DARK		37
#define DEFECT_TYPE_SPOT_HIGH		38
#define DEFECT_TYPE_GAP_A_HIGH		39
#define DEFECT_TYPE_GAP_A_DARK		40
#define DEFECT_TYPE_GAP_S_HIGH		41
#define DEFECT_TYPE_GAP_S_DARK		42
#define DEFECT_TYPE_UF_MISC_HIGH	43
#define DEFECT_TYPE_UF_MISC_DARK	44
#define DEFECT_TYPE_GAP_S			45
#define DEFECT_TYPE_INJ_HOLE		46
#define DEFECT_TYPE_PTN_ERROR		47
#define DEFECT_TYPE_DBD				48
#define DEFECT_TYPE_GBD				49
#define DEFECT_TYPE_BL_LAMP			50
#define DEFECT_TYPE_CROSSTALK		51
#define DEFECT_TYPE_BL_LEAK			52
#define DEFECT_TYPE_BL_LAMP2		53
#define DEFECT_TYPE_CROSSTALK_PTN	54
#define DEFECT_TYPE_BL_DARK			55
#define DEFECT_TYPE_PN_ULUK			56
#define DEFECT_TYPE_HOR_ULUK		57
#define DEFECT_TYPE_VER_ULUK		58
#define DEFECT_TYPE_UF_GATE_DEFECT	59

#define DEFECT_TYPE_MICRO_DDEFECT	60
#define DEFECT_TYPE_MICRO_GDEFECT	61

#define DEFECT_TYPE_IMUL_HIGH		62
#define DEFECT_TYPE_UF_IMUL			63
#define DEFECT_TYPE_WHITE_POINT		64


// display용으로 사용 : defect list 생성 시
#define DEFECT_TYPE_MANY_HIGH		70	
#define DEFECT_TYPE_INSP_ERROR		71
#define DEFECT_TYPE_DIAG_ULUK		72
#define DEFECT_TYPE_FINE_LINE_ULUK	73
#define DEFECT_TYPE_CAM_MODULE		74
#define DEFECT_TYPE_UF_BL_IMUL		75
#define DEFECT_TYPE_WHITE_SPOT		76
#define DEFECT_TYPE_BL_OFF			77
#define DEFECT_TYPE_BRIGHTNESS		78
#define DEFECT_TYPE_4INVERT			79
#define DEFECT_TYPE_BRIDGE			80
#define DEFECT_TYPE_THINNING		81
#define DEFECT_TYPE_PATTERNMISS		82

// 특정 불량 Reject 처리용 
#define REJECT_OPTION_SC			1
#define REJECT_OPTION_HIGH			2
#define REJECT_OPTION_HIGH_OVER     4
#define REJECT_OPTION_OFF			8
#define REJECT_OPTION_OFF_OVER		16    
#define REJECT_OPTION_IMUL          32
#define REJECT_OPTION_SPOT			64
#define REJECT_OPTION_GAP			128
#define REJECT_OPTION_JUIPGU		256
#define REJECT_OPTION_CD_SHORT      512
#define REJECT_OPTION_CG_SHORT      1024
#define REJECT_OPTION_GATE_OPEN     2048
#define REJECT_OPTION_DATA_OPEN     4096
#define REJECT_OPTION_GFO           8192
#define REJECT_OPTION_DFO			131072
#endif

///점등검사에서 사용... 끝
////////////////////////////////////////////////////////////////////////// 

// PLC_interface 사용 목적.. 
#define DEVICE_MAX_SIZE            10000

#define DEVICE_R                   0
#define DEVICE_D                   1
#define DEVICE_ZR                  2
#define DEVICE_B                   3
#define DEVICE_W                   4
#define DEVICE_M                   5
#define DEVICE_X                   6
#define DEVICE_Y                   7

#define BIT_READ                   0
#define WORD_READ                  1
#define WORD_RANDOM_READ           2
#define BLOCK_RANDOM_READ          3

#define BIT_WRITE                  0
#define BIT_RANDOM_WRITE           1
#define WORD_WRITE                 2
#define WORD_RANDOM_WRITE          3
#define BLOCK_RANDOM_WRITE         4

#define WORD_MODE                  1
#define BIT_MODE                   2

#define SERVER_MODE                1
#define CLIENT_MODE                2
#define MAX_RECEIVE_LEN		       8192
#define RECEIVE_DATA_MAX_LENGTH    10000

#define ONE_WORD                   16


#define	OK_RESPONSE	               0
#define	ERROR_RESPONSE      	   -10
#define NOT_CONNECT		    	   -11
#define NO_IP			    	   -12
#define DATA_ERROR		    	   -13
#define FILE_NO			    	   -14
#define FILE_READ_ERROR	    	   -15


#define COMMAND_UNKNOWN     'U'
#define COMMAND_QUERY       'Q'
#define COMMAND_RESULT      'R'
#define COMMAND_READY       'D'
#define COMMAND_FILETRANCE  'F'
#define COMMAND_FILEEND     'E'
// Image Classify
#define ACTIVE_ALG				0
#define PAD_ALG					1
#define DUST_ALG				2
#define ACTIVE_AND_PAD_ALG		3
#define ACTIVE_AND_DUST_ALG		4
#define PAD_AND_DUST_ALG		5
#define ALL_ALG					6
// Defect Feature
#define FEATURE_WIDTH					0
#define FEATURE_HEIGHT					1
#define FEATURE_AREA					2
#define FEATURE_BOX_X_MIN				3
#define FEATURE_BOX_X_MAX				4
#define FEATURE_BOX_Y_MIN				5
#define FEATURE_BOX_Y_MAX				6
#define FEATURE_PIXEL_MIN				7
#define FEATURE_PIXEL_MAX				8
#define FEATURE_PIXEL_MEAN				9
#define FEATURE_PIXEL_SIGMA				10
#define FEATURE_BREADTH					11
#define FEATURE_COMPACTNESS				12
#define FEATURE_ELONGATION				13
#define FEATURE_FERET_ELONGATION		14
#define FEATURE_FERET_DIAMETER_MIN		15
#define FEATURE_FERET_DIAMETER_MAX		16
#define FEATURE_FERET_DIAMETER_MEAN		17

///////////////////////////////////////////////////////////////////////////////////
//ALARM ID 정의
#define	TRI_CONNECT_FAIL					900000
#define	TRI_COMMAND_RETRY					900001
#define	TRI_COMMAND_FAIL					900002
#define	INTER_MSG_REV_FUNC_START_FAIL		900003
#define	INTER_SEND_RESPONSE_NG				900004
#define	INTER_SEND_INTERLOCK_NG				900005
#define	INTER_SEND_MEASURE_NG				900006
#define	INTER_SEND_TEMP_NG					900007
#define	INTER_SEND_RECIPESET_NG				900008
#define	INTER_SEND_JUDGEEND_NG				900009
#define	INTER_SEND_CLASSIFYEND_NG			900010
#define	INTER_SEND_TIMECHECK_NG				900011
#define	INTER_SEND_GRABEND_NG				900012
#define	INTER_SEND_GRABACK_NG				900013
#define	LIGHT_CONNECT_FAIL					900014
#define	LIGHT_SET_VALUE_OVER				900015
#define	LIGHT_SET_PWM_FAULT					900016
#define	LIGHT_TEMP_ERROR					900017
#define	LIGHT_FAN_ERROR						900018
#define	ERROR_GRAB_FAIL						900019
#define	ERROR_CMD_LIGHT_FAIL				900020
#define	ERROR_GRAB_TIME_OUT					900021
#define	ERROR_IMAGE_SAVE_TIME_OUT			900022
#define	ERROR_ALIGN_FAIL					900023
#define	ERROR_EMPTY_CELL					900024
#define	ERROR_NO_POL						900025
#define	ERROR_GET_LUMINACE					900026
#define	ERROR_ALGORITHM_MEASURE				900027
#define	ERROR_ALGORITHM_ACTIVE				900028
#define	ERROR_ALGORITHM_ACTIVE_DIMPLE		900029
#define	ERROR_ALGORITHM_EDGE				900030
#define	ERROR_ALGORITHM_PAD					900031
#define	ERROR_ALGORITHM_SCRATCH				900032
#define	ERROR_ALGORITHM_REFLECTION_ACTIVE	900033
#define	ERROR_ALGORITHM_TRANSMISSION		900034
#define	ERROR_ALGORITHM_PAD_SCRATCH			900035
#define	ERROR_ALGORITHM_SCRATCH2			900036
#define	ERROR_ALGORITHM_PAD2				900037
#define	ERROR_ALGORITHM_CORNER				900038
#define	ERROR_ALGORITHM_DIAGONAL			900039
#define	ERROR_MEASURE_TO_QMS				900040
#define	ERROR_DEF_COMPARE_CHECK				900041
#define	ERROR_DEF_DUST_REMOVE				900042
#define	ERROR_DEF_OVERLAP_DELETE			900043
#define	ERROR_DEF_MERGE_RENAME				900044
#define	ERROR_DEF_UPDATE_COPYIMAGE			900045
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//EVENT ID 정의
#define	EVT_CONNECT_TRIGGER					1000
#define	EVT_SET_TRIGGER_DIR					1001
#define	EVT_FORWARD_SCAN					1002
#define	EVT_BACKWARD_SCAN					1003
#define	EVT_TRIGGER_CHANNEL0				1004
#define	EVT_TRIGGER_CHANNEL1				1005
#define	EVT_START_TRIGGER					1006
#define	EVT_SEND_TRIGGER					1007
#define	EVT_CONNECT_PG						1008
#define	EVT_SET_PWALL						1009
#define	EVT_GET_LIGHT_STATUS				1010
#define	EVT_LIGHT_TURN_ON					1011
#define	EVT_LIGHT_TURN_OFF					1012
#define	EVT_SUPERVISOR_REV_MSG				1013
#define	EVT_SUPERVISOR_SEND_MSG				1014
#define	EVT_DIGISAP_CREATE					1015
#define	EVT_DIGISAP_CALLBACK_CAM1			1016
#define	EVT_DIGISAP_CALLBACK_CAM2			1017
#define	EVT_DIGISAP_INIT_SAPERA				1018
#define	EVT_DIGISAP_INIT_SAPERABUF			1019
#define	EVT_DIGISAP_INIT_SAPERABUF_INIT		1020
#define	EVT_DIGISAP_SCAN_START				1021
#define	EVT_DIGISAP_FREEZE					1022
#define	EVT_DIGISAP_CLEAR					1023
#define	EVT_DIGISAP_CLEAR_BUF				1024
#define	EVT_DIGISAP_SAVE_SAP				1025
#define	EVT_DIGISAP_ERROR_CALLBACK			1026
#define	EVT_STAGE_INIT						1027
#define	EVT_STAGE_INIT_VISION				1028
#define	EVT_SYS_START_INIT					1029
#define	EVT_SYS_INIT_VISION					1030
#define	EVT_SYS_TRACE_LOG					1031
#define	EVT_SYS_MEMORY_LOG					1032
#define	EVT_SYS_COMMON_LOG					1033
#define	EVT_SYS_INSP_LOG					1034
#define	EVT_SYS_CHANGE_MODEL				1035
#define	EVT_SYS_SEND_RCP_LIST				1036
#define	EVT_SYS_SERVER_RCP_SELECT			1037
#define	EVT_SYS_THREAD_INIT					1038
#define	EVT_SYS_START_LOG					1039
#define	EVT_SYS_TACT_LOG					1040
#define	EVT_AUTO_GRID_SUM_DBCLICK			1041
#define	EVT_AUTO_GRID_SUM_ROWCLICK			1042
#define	EVT_AUTO_GRID_DEFECT_LIST			1043
#define	EVT_AUTO_UPDATE_GRAB_TIME			1044
#define	EVT_AUTO_UPDATE_CLASS_TIME			1045
#define	EVT_AUTO_UPDATE_GRABIMAGE			1046
#define	EVT_AUTO_UPDATE_PROGRESS			1047
#define	EVT_LINECAM_START_INSP				1048
#define	EVT_LINECAM_START_GRAB				1049
#define	EVT_LINECAM_WRITE_RESULT_DATA		1050
#define	EVT_LINECAM_REMOVE_DUST				1051
#define	EVT_LINECAM_COMP_DEFECT_CHK			1052
#define	EVT_LINECAM_COMP_DRAW_RESULT		1053
#define	EVT_LINECAM_COMP_DRAW_ORI			1054
#define	EVT_LINECAM_COMP_DRAW_PART			1055
#define	EVT_LINECAM_COMP_MERGE_PIMPLE		1056
#define	EVT_LINECAM_RENAME_DATA_QMS			1057
#define	EVT_LINECAM_COMP_PIMPLE				1058
#define	EVT_LINECAM_COMP_INSERT_DATA		1059
#define	EVT_LINECAM_OVERLAP_DEF_DEL			1060
#define	EVT_LINECAM_INSP_IMAGE				1061
#define	EVT_LINECAM_START_GRABEX			1062
#define	EVT_LINECAM_PREPARE_GRAB_IMG		1063
#define	EVT_LINECAM_IMG_SAVE_PROC			1064
#define	EVT_LINECAM_RECALSSIFY_DEF_LIST		1065
#define	EVT_LINECAM_UPDATE_COPY_IMG			1066
#define	EVT_LINECAM_INSP_IMG_REPORT			1067
#define	EVT_LINECAM_GRAB_END_CHK			1068
#define	EVT_LINECAM_GRAB_END_PROC			1069
#define	EVT_LINECAM_FILTER_ROI				1070
#define	EVT_LINECAM_DEL_WATER_DIRT			1071
#define	EVT_ALG_INF_CAL_OFFSET				1072
#define	EVT_ALG_INF_ADJ_ALIGN_MARK			1073
#define	EVT_ALG_INF_CAL_OFFSET_REF			1074
#define	EVT_ALG_INF_CELL_CUT_BUF			1075
#define	EVT_ALG_INF_COLOR_BUF				1076
#define	EVT_ALG_INF_START_LOGIC				1077
#define	EVT_ALG_INF_GET_ALGNAME				1078
#define	EVT_ALG_INF_START_LOGIC_ALG			1079
#define	EVT_ALG_INF_START_LOGIC_ALIGN		1080
#define	EVT_ALG_INF_START_LOGIC_LUMI		1081
#define	EVT_ALG_INF_ADJ_ROI_ALIGN			1082
#define	EVT_ALG_INF_BLOB_FEA_SAVE			1083
#define	EVT_ALG_INF_MEA_QMS					1084
#define	EVT_ALG_INF_DEF_RESULT_USER_JDG		1085
#define	EVT_ALG_INF_EDGE_DEF_CLASSIFY		1086
#define	EVT_ALG_INF_EMPTY_CHK_CELL			1087
#define	EVT_ALG_INF_EMPTY_CHK_POL			1088
#define	EVT_ALG_INF_CAL_LUMI				1089
////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
//	매인 폼 로딩시 폼 크기를 결정함
//	SW_SHOW : 일반 
//	SW_MAXIMIZE : 최대화
/////////////////////////////////////////
#define  SW_MAIN_SHOW           SW_MAXIMIZE
// InspectLIB Class 수정여부를 나타내는 매크로
// TRUE : Crux Project(기존), FALSE : Packing Project
#define __INSPECTLIB_CRUX_PROJECT           FALSE
//////////////////////////////////////////////////////////////////////////////
//초기화에 필요한 파일의 경로
//PATH_CFG		: 운용에 필요한 파일의 경로를 관리하는 cfg 파일
//DEVICE_CFG	: Part Device 설정에 관련된 Parameter 를 관리하는cfg 파일
//INITIAL_INI   : Initialize 성 Parameter 를 관리하는 ini 파일
/////////////////////////////////////////////////////////////////////////////

#define PATH_CFG		"D:\\Crux_Data\\INI\\Path.cfg"
#define DEVICE_CFG		"D:\\Crux_Data\\INI\\Device.cfg"
#define INITIAL_INI		"D:\\Crux_Data\\INI\\Initialize.ini"
// 2015.10.15 Recipe 자동 변경 Matching Table
#define MODEL_MATCHING_INI	_T("D:\\Crux_Data\\INI\\Model_Recipe.ini")

#define USERACCOUNT_INI		"D:\\Crux_Data\\INI\\UserRegister.cfg"	//2015.07.18 add by mby


#define MM_TO_UM	1000.

// 측정 관련 에러
#define MEASURECELL_ERR			0
#define MEASURECELL_POL_CF_ERR	0.2
#define MEASURECELL_POL_TFT_ERR	0.2
#define MEASURECELL_POL_ERR		0

//2015.05.29 add by IKM for Grab 이미지 동일화를 위한 옵션 - S
#define EQP_TYPE_AVL		0
#define EQP_TYPE_ABI		1
#define EQP_TYPE_PACKING	2	// 2015.11.11 Packing 추가
#define EQP_PC_1  1
#define EQP_PC_2  2
#define EQP_PC_3  3
#define EQP_PC_4  4
#define EQP_PC_5  5
#define EQP_PC_6  6
#define EQP_PC_7  7
#define EQP_PC_8  8
#define EQP_PC_9  9

#define EQP_POS_R  0
#define EQP_POS_L  1
#define CF		   0
#define TFT		   1
#define POL		   2
#define EQP_TYPE_SML	 0
#define EQP_TYPE_MID	 1
#define EQP_TYPE_BIG	 2
// kyh modify 2016.05.18 moni
#define EQP_TYPE_MONITOR 3
// LDH 2017.09.05
#define BIG_TYPE_NINJING 0
#define BIG_TYPE_CHENGDU 1
//2015.05.29 add by IKM for Grab 이미지 동일화를 위한 옵션 - E

// 2015.06.11 Defect 재분류 관련 설정
#define MAX_PC_COUNT	10
#define MAX_RECLASSIFY_TIME	7200000

//2015.06.28 add by mby 
// 2016.02.16 KSW
#define MAX_DEFETYPE_COUNT	E_DEFECT_NAME_COUNT
//2015.06.30 add by mby for Drive path
#define DRIVE_PATH_COUNT	10	
#define DRIVE_LIMIT_SIZE	10		//(%)
#define EX_DRIVE1			_T("C:\\")
#define EX_DRIVE2			_T("D:\\")	

//2016.03.29 NDH : 데이터 저장 관련 옵션 첨부스트링
#define  MANAGE_OPTION_KEY				_T("_TYPE")

#define  IMAGE_EXT_NAME		_T("YWIMG")
#define IMAGE_HEADER_IDN	125