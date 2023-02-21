#if !defined(AFX_SYSTEM_H__3BB91EA9_FA09_425E_89BF_F1300FD2FD74__INCLUDED_)
#define AFX_SYSTEM_H__3BB91EA9_FA09_425E_89BF_F1300FD2FD74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif
//#include "VsMessageProcessor.h"
#include "Supervisor.h"
#include "Config.h"
//#include "Light.h"
//#include "NLight.h"
#include "NeepsLightControl.h"
#include "Trigger.h"
#include "Stage.h"
#include "CamCtrl.h"
#include "OGModelInfo.h"

#include "FirActSvSocket.h"
#include "SndActSvSocket.h"
#include "ExtSvSocket.h"

#include "FirActClSocket.h"
#include "SndActClSocket.h"
#include "ExtClSocket.h"
#include "ClientSocket.h"
#include "ModelInfo.h"
#include "CriticFlag.h"
#include "ODSystem.h"
#include "Runpanel.h"
#include "ReportData.h"
#include "CheckWorkTable.h"
#include "Panel.h"

#include "LightOffset.h"

//#include "token.h"

struct tdefectDisplyParam
{
	int					Total_defect;
	int					Ng_defect;
	int					Defect_Type[MAX_DEFETYPE_COUNT];
	CString				Defect_Judge;
	void initParam()
	{		
		Total_defect = 0;
		Ng_defect = 0;	 
		Defect_Judge = _T("");
		for(int i =0; i < MAX_DEFETYPE_COUNT ; i++){
			Defect_Type[i] = 0;
		}		
	}
};
// struct tDriveInfoParam
// {
// 	int					Total_Drive;
// 	CString				Drive_Name;
// 	float				Free_PerCent;
// 	bool				bUseFlag;
// 	int					FormatFlag;
// 	int					nDriveLimitSize;
// 	//void initParam()
// 	tDriveInfoParam()
// 	{		
// 		Total_Drive = 0;
// 		Free_PerCent = 0.0;	 
// 		Drive_Name = _T("");	
// 		bUseFlag = false;
// 		FormatFlag = 0;
// 		nDriveLimitSize = 95;
// 	}
// };
struct Parameter
{
	int nWidth;
	int nHeight;
	int nArea;
	int nGradient;
	bool bUse;
};

struct Combination
{
	bool	bAnd;		// And 연산 시작점
	bool	bOr;		// Or 연산 시작점
	int		nFrature;	// 비교 대상
	int		nSign;		// 부등호
	double	dValue;		// 비교 값
};

struct InfoROI_AlignAfter
{
	CRect			rectROI;
	int				nThreadID;			// 멀티 쓰레딩 환경을 고려하여 ID 를 관리
	InfoROI_AlignAfter()
	{
		nThreadID = 0;
	}
};

const int MAX_INSPACT_THREAD_CNT = 100;

// 알고리즘 파라메터가 바뀔경우 대응하기 위해 레서피 메모리 공간을 미리 큰 사이즈로 확보해 놓는다. 이하는 레서피 구조체 크기를 위한 상수
const int MAX_MEM_SIZE_INSPECTION_SECTION = 8;		// 레서피 구조체로 메모리 공간으로 확보 할 크기, 향후 확장을 고려하여야한다.
const int MAX_MEM_SIZE_E_ALGORITHM_NUMBER = 16;
const int MAX_MEM_SIZE_E_DEFECT_NAME_COUNT = 65;	// KSW 16.01.26
const int MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT = 55;
//const int MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT = 50;	// DefineInspect.h 존재 ( Area, Box, Lengrh, .... )

enum INSPECTION_SECTION
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

struct InfoROI
{
	CRect				rectROI;												// 영역
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet 계산
	TCHAR				strROIName[300];										// 영역 이름
	CPoint				ptPoint[2];												// 영역 당시 Mark 2개
	int					nROICnt;												// 영역 개수
	bool				bAlgorithmUse[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER];			// 알고리즘 사용 여부
	double				dPara[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT];					// 알고리즘 파라메터
	tDefectJudgment		stDefectFilteringParam[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];	// 결함 필터링 파라메터
	BOOL				bUseSegmentation;																			// ROI를 분할 영역에 따라 나눌 것인지 여부
	InfoROI()
	{
		memset(strROIName, 0, sizeof(strROIName));
		memset(bAlgorithmUse, 0, sizeof(bAlgorithmUse));
		memset(dPara, 0, sizeof(dPara));
		memset(stDefectFilteringParam, 0, sizeof(stDefectFilteringParam));
		nROICnt = 0;
		bUseSegmentation = FALSE;
	}
};

//struct InfoROI
//{
//	CRect				rectROI;												// 영역
//	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet 계산
//	TCHAR				strROIName[300];										// 영역 이름
//	CPoint				ptPoint[2];												// 영역 당시 Mark 2개
//	int					nROICnt;												// 영역 개수
//	bool				bAlgorithmUse[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER];									// 알고리즘 사용 여부
//	double				dPara[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER][ALG_PARA_TOTAL_COUNT];					// 알고리즘 파라메터
//	tDefectJudgment		stDefectFilteringParam[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER][E_DEFECT_NAME_COUNT];	// 결함 필터링 파라메터
//	bool				bUseSegmentation;																			// ROI를 분할 영역에 따라 나눌 것인지 여부
//	InfoROI()
//	{
//		memset(strROIName, 0, sizeof(strROIName));
//		memset(bAlgorithmUse, 0, sizeof(bAlgorithmUse));
//		memset(dPara, 0, sizeof(dPara));
//		memset(stDefectFilteringParam, 0, sizeof(stDefectFilteringParam));
//		nROICnt = 0;
//	}
//};

struct InfoSEG
{
	int					nYCoordSEGLine;											// 분할 영역 Y 좌표
	int					nSEGCnt;												// 영역 개수	
	InfoSEG()
	{
		nSEGCnt = 0;
	}
};

// 2015.08.04 무효화 영역 ROI 추가
struct tRectInfo
{
	CRect				rectROI;												// 영역
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet 계산
	CPoint				ptPoint[2];												// 영역 당시 Mark 2개
	TCHAR				strROIName[300];										// 영역 이름
	int					nUsePanel;												// 2015.09.04 소형 Panel 별 검사 제외 영역 사용/비사용 선택
	tRectInfo()
	{
		memset(this, 0, sizeof(tRectInfo));
		nUsePanel = (int)eUseAllPanel;	// All Use Panel1, 2	// 2015.09.04 소형 Panel 별 검사 제외 영역 사용/비사용 선택
	}
};

struct InfoFilterROI
{
	int				nROICnt;		// 전체 영역 개수
	tRectInfo*		pRectInfo;
	InfoFilterROI()
	{
		nROICnt = 0;		
	}
};
//2016.04.06 add by mby
struct InfoMoniImgROI
{
	int				nROICnt;		// 전체 영역 개수
	tRectInfo*		pRectInfo;
	InfoMoniImgROI()
	{
		nROICnt = 0;		
	}
};

//2015.10.16 POL Mark 위치 등록 및 알람 기능 추가
struct InfoPolMarkROI
{
	int					nUseFlag;
	double				dbXsize;		//2015.10.16 POL Mark 위치 등록 및 알람 기능 추가
	double				dbYsize;		//2015.10.16 POL Mark 위치 등록 및 알람 기능 추가
	int					nMainUseFlag;	// Main 검사 기능
	tRectInfo			PolMarkRectInfo;
	InfoPolMarkROI()
	{
		memset(this, 0, sizeof(InfoPolMarkROI));
	}
};

struct InfoPolMark
{
	CRect				rectROIPol;												// 영역
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet 계산
	TCHAR				strROIPolName[300];										// 영역 이름
	TCHAR				strROIPolGroup[300];									// LDH 2017.03.25 POL Mark 
	CPoint				ptPoint[2];												// 영역 당시 Mark 2개
	int					nROIPolCnt;												// 영역 개수	
	int					nUseFlag;												// 사용 여부
	InfoPolMark()
	{
		memset(strROIPolName, 0, sizeof(strROIPolName));				
	}
};

//2015.06.05 add by mby for Measure Mark register S-
struct InfoMesMark
{
	CRect				rectROIMes;												// 영역
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet 계산
	TCHAR				strROIMesName[300];										// 영역 이름
	CPoint				ptPoint[2];												// 영역 당시 Mark 2개
	int					nROIMesCnt;												// 영역 개수	
	int					nUseFlag;												// 사용 여부
	InfoMesMark()
	{
		memset(strROIMesName, 0, sizeof(strROIMesName));				
	}
};

struct tMesMarkInfo
{
	int			nTotalMesImageNumber;
	MIL_ID		*nImageBuf;					// Image info
	tMesMarkInfo(int nMaxImage)
	{
		nTotalMesImageNumber	= nMaxImage;		
		nImageBuf				= (MIL_ID *) malloc ( sizeof( MIL_ID ) * nMaxImage );
	}
};
//2015.06.05 add by mby for Measure Mark register E-

// System.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSystem thread

class CInterServerInterface;		// For PLC Task
class VsMessageProcessor;			// For PLC Task
//class AFX_EXT_CLASS CSystem : public CWinThread
class AFX_EXT_CLASS CSystem : public CWnd
{
//	DECLARE_DYNCREATE(CSystem)

private:												// 중복 선언 방지
	CSystem(void);
	virtual ~CSystem(void);
	static CSystem* m_Instance;				// Singleton 객체

	// LOG 파일 기입을 위한 Critical Section 
	CRITICAL_SECTION m_csTraceLog[NUM_INSP_CAMERA + 1][MAX_INSP_THREAD + 1];
	CRITICAL_SECTION m_csMemoryLog;
	CRITICAL_SECTION m_csComLog;			
	CRITICAL_SECTION m_csProgramLog;
	CRITICAL_SECTION m_csTactLog;
	CRITICAL_SECTION m_csFormatImg;	//2015.06.30 add by mby
	CRITICAL_SECTION m_csGuiBlocking;
	HANDLE			 m_hThreadFormatImg; //2015.06.30 add by mby
	static unsigned __stdcall  m_fnThreadFormatlmgProc(LPVOID pArg );
	

public:
	static CSystem* GetInstance();			// Singleton
	static void ReleaseInstance();			// Singleton 삭제
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

// Attributes
public:

	// 2014-04-18, jhLee
	// SysHwCtrl에서 발생할 수 있는 Error Code
	enum {
		eERR_SYSHW_None			= 0,			// Error 없음

		eERR_SYSHW_BASE			= 1000,		// CSystem의 Error 시작 Code
		eERR_SYSHW_ConfigFile,					// 초기화 파일이 존재하지 않는다.
		eERR_SYSHW_ModelFile,					// Model file이 없다.
		eERR_SYSHW_InitStage,					// Stage 초기화 실패
		eERR_SYSHW_InitLight,					// Serial, Light 초기화
		eERR_SYSHW_InitTrigger,					// Serial, Trigger 초기화
		eERR_SYSHW_InitSocket1,					// Socket, 첫번째 소켓 초기화 실패 
		eERR_SYSHW_InitSocket2,					// Socket, 두번째 소켓 초기화 실패 
		eERR_SYSHW_InitSocket3,					// Socket, 세번째 소켓 초기화 실패 
		eERR_SYSHW_BaseThread,					// Base Thread 생성 실패
		eERR_SYSHW_ModuleThread,				// Module Thread 생성 실패
		eERR_SYSHW_ModelSet,					// Model 설정 실패

	};


	enum {			// Initial 과정에서 발생하는 Event
		eEVT_InitStart			= 0,		// 초기화 시작 
		eEVT_InitPause,						// 일시멈춤
		eEVT_InitResume,					// 초기화 재개
		eEVT_InitAbort,						// 초기화 중도 포기
		eEVT_InitFinish,					// 초기화 완료

	};


	// 2014-04-18, jhLee
	// CSystem의 StartHW() 과 Initial 화면에서 사용하는 초기화 단계
	enum {
		eINIT_StepNone			= 0,	// 아직 초기화 진행하지 않는다.
		eINIT_StepFile,					// 1. 초기화 파일 점검 : path validity
		eINIT_StepModel,				// 2. 모델 파일 확인: path validity	
		eINIT_StepStage,				// 3. Stage 초기화
		eINIT_StepLight,				// 4. Serial, Light 초기화
		eINIT_StepTrigger,				// 5. Serial, Trigger 초기화
		eINIT_StepSocket1,				// 6. Socket, 첫번째 소켓 초기화
		eINIT_StepSocket2,				// 7. Socket, 두번째 소켓 초기화
		eINIT_StepSocket3,				// 8. Socket, 세번째 소켓 초기화
		eINIT_StepBaseThread,			// 9. Base Thread 생성
		eINIT_StepModuleThread,			// 10. Module Thread 생성
		eINIT_StepModelSet,				// 11. Model 설정 
		eINIT_StepFinish,				// 12. 초기화 마무리	
	};

	bool m_bVisionState;				//버튼 선택
	int	m_nInitSeq;						// 초기화가 진행중인 순번 
	int	m_nInitSeqSub;					// 하위 초기화 순번
	bool	m_bInitTerminateFlag;		// 초기화 Thread를 도중에 종료해야하는지 여부, false:수행, true:종료
	bool	m_bInitPauseFlag;			// 초기화 Thread를 도중에 일시멈춤을 해야 하는지 여부, false:수행, true:멈춤
	CWinThread* m_pInitThread;			// Initial 수행 Therad의 Handle


	void	StartInitialize();			// 초기화 시작, (처음부터 진행)
	void	PauseInitialize();			// 초기화를 일시적으로 정지시킨다.
	void	ResumeInitialize();			// 초기화 일시정지를 풀고 재개한다.
	void	AbortInitialize();			// 초기화를 완전 중지 시킨다.

	void    SetNetworkDrive();        // 네트워크 드라이브 체크용
 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	CInterServerInterface*		m_pVsInterServerIF;							// For PLC Task
	VsMessageProcessor*			m_pVsMessage;								// For PLC Task

	// states
	int  volatile	m_bNowInitializing;	// 초기화 상태
	CCriticFlag		SystemReady;
	

	int	m_iRejectDefectOption;          // 특정 불량 Reject 처리용 
	int m_nDefectCnt;
	int m_nTtcheckCnt;					//2019.11.26 add by mby
	CCriticFlag		ManualRun;			// 필요성 여부 확인
	CCriticFlag		AutoRun;			// 필요성 여부 확인할 것
	CCriticFlag		AutoStop;			// 필요성 여부 확인할 것

	// overall
	CSupervisor		Smith;
	CWnd* volatile  pMasterWnd;			// main 화면

	Parameter		**m_Parameter;

	int m_nImagTestImageNum;

	CConfig			m_Config;
	CModelInfo		ModelInfo;
	CString			m_strCurrentModelName;	// 현재 모델명
	
	COGModelInfo	OGModelInfo;

	// H/W controls (플러스테크 조명 사용시)
// 	CLight			FrontLight;			// 1개 controller로 3 stage 사용 (각 스테이지당 1 채널)
// 	CLight			BackLight;			// 1개 controller로 3 stage 사용 (각 스테이지당 1 채널)
//  CLight			CoaxialLight;		// 외관 스테이지용, 동축조명 제어

// H/W controls (플러스테크 조명 사용시)
 	CNipsLight			LineLight;          // Line8K용 조명(스테이지당 1 채널)
 	CNipsLight			LineCoaxialLight;   // Line2K용 조명(스테이지당 2 채널)

  	CNipsLight			FrontLight;			// Area용 조명 (스테이지당 4 채널)
 	CNipsLight			BackLight;			// Area용 조명 (스테이지당 5 채널)
	CNipsLight			FrontAngleLight;	// Area용 조명 (스테이지당 4 채널)
    CNipsLight			CoaxialLight;		// Area용 조명 (스테이지당 3 채널)

	CTrigger			Trigger;

	CString				PLCCellID;

//	CToken				Tokenizer;


//	CStage			Stage[NUM_INSP_STAGE];	// ODSystem, cam cotrol은 stage 마다 (per board)
	CStage			Stage[NUM_FRAME_GRABBER];	// ODSystem, cam cotrol은 stage 마다 (per board)

		// BM 빛샘 검사용 image buffer list
	CBMImageListCritic m_BMImageList;

	// 16.03.16 KSW
	// 측정 수치 넘어가면 수치 변경.
	bool			m_MeasureCutFlag;

	// 16.04.04 KSW
	// 물때 제거
	bool			m_DeleteWaterDirtFlag;

	// MIL : per system
 	MIL_ID			m_MilApp;
 	MIL_ID			m_MilHostSystem;
	MIL_ID			m_MilSystem;

// 	MAPPHOOKFCTPTR	HandlerPtr;
// 	void*			HandlerUserPtr;

	// 검사
	// 패널 정보 관리 (stage -> 다음 단계)
 	CRunPanel			RunPanel;		// 시스템에 진입하여, 아직 결과를 전송하지 않은 패널 pool
 	CReportData			Report;
// 	
 	CCheckWorkTable		CheckInsp0;
 	CCheckWorkTable		CheckInsp1;

	// Socket 
 	//CFirActSvSocket		m_FirActSvSocket;
 	//CSndActSvSocket		m_SndActSvSocket;
 	//CExtSvSocket		m_ExtSvSocket;
 	//CClientSocket		m_ClSock;

 	//CFirActClSocket		m_FirActClSocket;
 	//CSndActClSocket		m_SndActClSocket;
 	//CExtClSocket		m_ExtClSocket;

	CString				m_ReceiveMsg;

	int		volatile	SocketSendDefect[1000];  // 통신 보내기 위한 결과 값 저장 장소.
	bool	volatile	actend[1000];
	bool	volatile	ogend[1000];
	bool	volatile	OgNotUse;
	bool	volatile	resend[1000];
	short	int			nClassState[1000];		// 검사결과 파일 쓰기 관련

	bool	m_bCameraStatus;
	bool	GetCameraStatus();
	void	SetCameraStatus(bool bStatus);

	CPoint m_ptPMIndex;

	//vision Align 좌표
	CPoint m_ptAlignIndex[2];

	//2015.05.10 add by mby 
	//int					Total_defect[2];
	//int					Ng_defect[2];
	//int					Defect_Type[2][20];
	//CString				Defect_Judge1;
	//CString				Defect_Judge2;
	int					m_defIndex;

	int					m_nMainUINo;		//2021.02.08 add by mby
	int					m_nTeachUINo;		//2021.02.08 add by mby
	int					m_nAutoRecipeFlag;	//2016.02.01 add by mby
	//2015.05.10 add by mby 
	int					m_nSection;			//2015.07.20 add by mby
	int					m_nManual;			//2015.07.20 add by mby
	int					m_nUserParamUse;	//2015.07.20 add by mby
	int					m_nALLInspUse;		//2015.07.27 add by mby

	CString*			strArrayDrive;		//2015.06.07 add by mby 
	// 2015.09.01 Comment by CWH
	//CString				strImageDrviePath;		//2015.06.30 add by mby 
	BOOL				m_bInterLockProceeFlag;		//2015.10.19 add by mby 

	int					m_iVisionAlignX;
	int					m_iVisionAlignY;
	int					m_iVisionAlignT;

	BOOL				m_bPanelAllGood;

	BOOL				m_bSysInitComplete;

	BOOL				m_bFlgVacMessage[4];

	int					m_nStageID;
	CString				m_strInnerID;
	CString				m_strOldInnerID;
	CString				m_strModelName;
	CString				m_strDefectPathName;		//2015.05.16 add mby
	BOOL				m_flgInterfaceAuto;
	CString				m_strPanelID;
	CString				m_strPanelID1;				//2015.07.09 for abi_tdi

	float				m_fGrabTime;
	float				m_fClassTime;
	float				m_fTotalTactTime;

	CPanel*				m_pPanel;

	CLightOffset*		m_pLightOffset;

	BOOL				m_bUseDefectFilter;		// 검사 무효화 ROI 사용 / 비사용
	BOOL				m_bUseDefectMoni;		// 미검 영역 ROI 사용/비사용

	CString				m_strServerModelName;				//2015.10.29 서버에서 선택한 모델명

	//BOOL				m_bThreadFileBusy[MAX_INSP_THREAD + 1];			// 2014.08.24 스레드 별 Log 파일 기입 구분

	//2016.08.31 NDH : Cache 메모리 증가로 인한 그랩 횟수 체크 변수
	int					m_nInitMemGrabAccumCount;
// Operations
public:
	BOOL SendCopyMsg(ULONG nMsgId, CString strMsg);

	BOOL CheckManualOcr(CString strOcr);
	void TraceRunTimeDB(BOOL rCode);
	void TraceSystemLogDB(int sCode);
	void TraceErrorHistoryDB(int errorCode);
//	void TraceResultDB(CPanel *pPanel);
	void TraceLogDB(int logID);
	int GetEquipInfo();
	UINT SetEquipInfo(LPCTSTR strPath);
	UINT SetCurrentMode(UINT nMode);
	UINT GetCurrentMode();
//위로 이동	void	StartInitialize();
	bool	InitializeVision();
	void	ExitVision();
	void	TraceLogFile(CString *pmsg, int nCameraID = NUM_INSP_CAMERA, int nThreadID = MAX_INSP_THREAD);
	void	CommLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	MemoryLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	InspectInfoLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	//jskim
	void	StartLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	TactLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	AlarmLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);	//Alarm Display용 LOG
	void	EventLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);	//EVENT용 LOG

//	BOOL	ChangeModelData(CString ModelName);
	BOOL	ChangeModelData();

	// util functions
	CString	GetIdfromMsg( CString buf);
	CString GetStringFromFile(CString FileRootName, CString Section, CString Key);
	DWORD	SetStringToFile(CString FileRootName, CString Section, CString Key, CString strValue);
	CString	GetString(CString Source, int Index, char Divider);
	BOOL	SetString(CString FileRootName, CString Section, CString Key, CString Data);
	BOOL	SetString(CString FileRootName, CString Section, CString Key, CString Data, int Index);
	CString GetMemoryInformation();

	//jskim
	void AutoDeleteFile(CString Target, int Duration);

	//2016.08.31 NDH Start : Cache 메모리 증가로 인한 그랩 횟수 체크 변수
	void InitGrabCountforCacheMem();
	int	AccumulateIGrabCountforCacheMem();
	//2016.08.31 NDH End

	BOOL GetVisionState();
	void SetVisionState(BOOL bState);

	BOOL GetSystemInitComplete();
	void SetSystemInitComplete(BOOL bSucess);

	BOOL GetVacMessage(int nID);
	void SetVacMessage(int nID, BOOL bFlg);

	int	 GetStageID();
	void SetStageID(int nID);

	CString GetInnerID();
	void	SetInnerID(CString strID);

	CString GetOldInnerID();
	void	SetOldInnerID(CString strID);

	CString GetPanelID();
	void	SetPanelID(CString strID);

	CString GetPanelID1();				//2015.07.09 for abi_tdi
	void	SetPanelID1(CString strID);	//2015.07.09 for abi_tdi
	
	CString GetModelName();
	void	SetModelName(CString strName);

	CString GetDefectPathName();						//2015.05.16 add by mby
	void	SetDefectPathName(CString strName);		//2015.05.16 add by mby

	BOOL	GetInterfaceAuto();
	void	SetInterfaceAuto(BOOL bSet);

	int		GetVisionAlignT();
	int		GetVisionAlignX();
	int		GetVisionAlignY();
	void	SetVisionAlignCoodi(int CoordX, int CoordY, int CoordT);

	void	SetGrabTime(float dTime);
	float	GetGrabTime();
	void	SetClassTime(float dTime);
	float	GetClassTime();
	void	SetTotalTactTime(float dTime);
	float	GetTotalTactTime();

	CPanel*	GetPanelCom();
	void	SetPanelCom(CPanel* pPanel);

	CLightOffset*	GetLightOffsetCom();
	void			SetLightOffsetCom(CLightOffset* pLightOffset);

	//Recipe List 송신	2015.10.29 add by mby 
	void	SendRecipeListInfo();
	BOOL	ServerRecipeSelect(CString strRecipeName);

	void	SendContollerError(BOOL bShow, CString strMsg); //2017.02.10
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Teaching 정보. Todo. 다른 Class 로 빼야한다.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	int					m_nSectionCount;			// ROI Section 개수.. ACC 의 경우 8개임
	CStringArray		m_strarryStrSectionName;	// ROI Section 의 표시 이름
	CPoint				m_ptMark[2];				// Teaching Align 마크 두개 좌표
	CPoint				m_ptMarkRef[2];				// Teaching Align 마크 두개 좌표
	InfoROI				*m_pInfoROI;				// 설정한 Teaching 구조체 포인터
	InfoSEG				*m_pInfoSEG;				// 설정한 Segmentation 구조체 포인터
	int					m_nForwardScanPadPosition;	// 첫 그라스의 Pad부 위치,0:Left,1:Top,2:Right,3:Bottom	
	tdefectDisplyParam	*m_tdefectDisplyParam;
	int					m_nRoiModePosition;			// Roi Mode Set 0:Normal 1:Pol Mark
	InfoPolMark			*m_pInfoPolMark;			// 설정한 Teaching 구조체 포인터
	tPolMarkImageInfo	*m_ptPolMarkInfo;			// 알고리즘에 보내줄 Image Buffer
	InfoMesMark			*m_pInfoMesMark;			// 설정한 Teaching 구조체 포인터
	tMesMarkInfo		*m_ptMesMarkInfo;			// 알고리즘에 보내줄 Image Buffer
	InfoROI				*m_pInfoRefROI;				// Pad Scratch Reference ROI Backward 용
	MIL_ID				m_milRefBuffer[8];				// Pad Scratch Reference buffer
	// 2015.09.01 디스크별 Limit 용량 기능 추가
	//tDriveInfoParam		*m_tDriveInfoParam;		// Disk info 구조체			

	// 15.10.06 KSW
	bool			m_bAlignCameraIndex[8];		// Teaching 시 Image 마다 Align 달리 하기 위함.
	CPoint			m_ptAlignMarkPos[8][2];		// Image 마다 Align 두개
	
	InfoROI			*m_pInfoUserParam;			// 2015.06.26 User Parameter Teaching 구조체 포인터 by CWH

	InfoFilterROI	*m_pInfoFilterROI;
	InfoPolMarkROI	m_InfoPolMarkROI;			//2015.10.16 POL Mark 위치 등록 및 알람 기능 추가
	InfoMoniImgROI	*m_pInfoMoniImgROI;			//2016.04.06 add by mby

	int				m_nActiveRoiSel;
	int				m_nDirection;

	//18.03.13 Lyh 
	int				m_LineFittingTh; //Line Fitting Align용 Th

	//21.06.07 add by mby 
	int				m_TTtime; //TT Time Recipe화
	

public:
	int			GetTeachingToatalROICount();										// Teaching에서 설정한 ROI 총 갯수 얻기
	CRect		GetTeachingROI(int nROINum);										// Teaching에서 설정한 ROI 영역 얻기.
	LPCTSTR		GetTeachingROIName(int nROINum);										// 2015.09.16 설정한 ROI 이름 얻기
	CRect		GetTeachingRefROI(int nROINum);
	CPoint		GetTeachingMark1(int nROINum);										// Teaching에서 ROI 설정 당시 Align Mark 1 좌표 얻기.
	CPoint		GetTeachingMark2(int nROINum);										// Teaching에서 ROI 설정 당시 Align Mark 2 좌표 얻기.
	CPoint		GetTeachingMarkRef1(int nROINum);									// Teaching에서 ROI 설정 당시 Align Mark 2 좌표 얻기.
	CPoint		GetTeachingMarkRef2(int nROINum);									// Teaching에서 ROI 설정 당시 Align Mark 2 좌표 얻기.
	int			GetTeachingToatalDefectCount();										// Teaching에서 설정한 Defect List 총 갯수 얻기
	bool		GetTeachingDefectUse(int nROINum, int nDefectNum, int nInspSectionNum);					// Teaching에서 설정한 ROI에 Defect List 체크 확인.
	double		GetTeachingParameter(int nROINum, int nDefectNum, int nParaNum, int nInspSectionNum);	// Teaching에서 설정한 ROI -> Defect -> 파라미터 얻기. ( 0~ 39 )
	CRect		GetTeachingAlignROI(int nROINum, int nThreadID);					// Align Mark 찾은 후, 보정 ROI 영역 얻기.
	CRect		GetTeachingAlignRefROI(int nROINum, int nThreadID);					// Align Mark 찾은 후, 보정 ROI 영역 얻기.
	int			GetTeachingToatalSEGCount();										// Teaching에서 설정한 SEG 총 갯수 얻기
	int			GetTeachingSEG(int nSEGNum);										// Teaching에서 설정한 SEG 영역 얻기.

	int			GetTeachingScanPadPos();	
	void		SetTeachingAlignRefROI(int nROINum, CRect rect, int nThreadID);		// Align Mark 찾은 후, 보정 ROI 영역 Reference 데이터로 저장.

	int			GetRoiMode();														//ROI MODE 설정 Normal,Pol Mark 2015.05.20 add by mby
	//void		CopyTeachingParameter(double *dDstArr, int nROINum, int nDefectNum, int nInspSectionNum);
	void		CopyTeachingParameter(double *dDstArr, int nROINum, int nDefectNum, int nInspSectionNum, int Mode = 0);
	//void		GetDefectFilteringParam(tDefectJudgment* dDstArr, int nCnt, int nROINum, int nAlgorithmNumber, int nInspSectionNum);
	void		GetDefectFilteringParam(tDefectJudgment* dDstArr, int nCnt, int nROINum, int nAlgorithmNumber, int nInspSectionNum, int Mode = 0 );
	int			SetTeachingScanPadPos(int nPos);	
	int			SetRoiMode(int nPos);												//ROI MODE 설정 Normal,Pol Mark 2015.05.20 add by mby
	void		SetTeachingAlignROI(int nROINum, CRect rect, int nThreadID);		// Align Mark 찾은 후, 보정 ROI 영역 수정.

	//2015.06.07 add by mby Disk info-
	void		SearchDriveDisk();													// Local Drvie Disk Search
	void		SearchDiskInfo(LPCTSTR lpDriver , int count);						// 지정 Drive Disk용량 확인
	float		GetByte(ULARGE_INTEGER dwParam);									// 바이트 취득
	void		GetUseDrivePath();													// 사용하는 Drive 경로 설정
	int			ProcessRamDiskFile();												// RAM Drive 를 사용하는 경우 파일 처리
	// 2015.09.01 Comment by CWH
	//BOOL		m_fnFormatImg();
	//BOOL		DeleteDirectoryFile(LPCTSTR RootDir);								// 하위 폴더까지 삭제.
	//2015.06.07 add by mby Disk info-

	// 2015.08.04 by CWH 무효화 영역 ROI
	int			GetTeachingToatalFilterROICount(void);									// Filter ROI Count
	CRect		GetTeachingFilterROI(int nROINum);										// Filter ROI Rect
	LPCTSTR		GetTeachingFilterROIName(int nROINum);									// Filter ROI Name
	void		SetTeachingAlignFilterROI(int nROINum, CRect rect, int nThreadID);		// Filter ROI Thread 별 Align 정보 Setting
	CPoint		GetTeachingMarkFilter1(int nROINum);									// Filter ROI Setting 당시 Mark Position 1
	CPoint		GetTeachingMarkFilter2(int nROINum);									// Filter ROI Setting 당시 Mark Position 2
	CRect		GetTeachingAlignFilterROI(int nROINum, int nThreadID);					// Align Mark 찾은 후, 보정 ROI 영역 얻기.

	// 2015.09.04 무효화 영역 Panel 별 사용 여부 기능 추가
	int			GetUsePanelFilterROI(int nCnt);

	// 2016.04.06 by mby MONI IMGAE 영역 ROI S-
	int			GetTeachingToatalMoniROICount(void);									// Moni Image ROI Count
	CRect		GetTeachingMoniROI(int nROINum);										// Moni Image ROI Rect
	LPCTSTR		GetTeachingMoniROIName(int nROINum);									// Moni Image ROI Name
	void		SetTeachingAlignMoniROI(int nROINum, CRect rect, int nThreadID);		// Moni Image ROI Thread 별 Align 정보 Setting
	CPoint		GetTeachingMarkMoni1(int nROINum);										// Moni Image ROI Setting 당시 Mark Position 1
	CPoint		GetTeachingMarkMoni2(int nROINum);										// Moni Image ROI Setting 당시 Mark Position 2
	CRect		GetTeachingAlignMoniROI(int nROINum, int nThreadID);					// Moni Image Mark 찾은 후, 보정 ROI 영역 얻기.
	int			GetUsePanelMoniROI(int nCnt);
	void		SetUseDefectMoniROI(BOOL bUseMoni);
	BOOL		GetUseDefectMoniROI();
	// 2016.04.06 by mby MONI IMGAE 영역 ROI E-

	int         m_nManualLightValue;
	BOOL        m_bGrabLight;
	BOOL        m_bManualLight;

	void		SetUseDefectFilterROI(BOOL bUseFilter);
	BOOL		GetUseDefectFilterROI();
// UI - System Interface 함수 추가
	void		CallCalculate_OffSetRef(MIL_ID milDrawBuffer, int nPosition, bool bClockWise, tInfoAlign InfoAlign, int nThreadID);

	//18.03.13 Lyh LineFitting Th 변수 추가.
	int			GetLineFittingTh();	
	void		SetLineFittingTh(int Value);	

	//21.06.07  add by mby T/T Time Reciep화
	int			GetTTIndfo();	
	void		SetTTIndfo(int Value);	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*	
	BOOL volatile occupied;
	int	 volatile OpGroup;
	BOOL volatile NW_Connection;

	SYSTEMTIME lastLogTime;

	CString		CurrentModelName;
	CCPUTimer	Timer;
	
	// flags
	CCrticFlag	SystemSuspended;
	CCrticFlag	SystemPausing;

	CSystemMsgBox MsgDlg;
*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSystem)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nCurMode;
	// Generated message map functions
	//{{AFX_MSG(CSystem)
		afx_msg void OnDestroy();
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	BOOL mIterate;
	BOOL mCInspStop;
	int  m_nContineCnt;	
};

UINT StartHW( LPVOID pParam );	// system H/W initialization

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEM_H__3BB91EA9_FA09_425E_89BF_F1300FD2FD74__INCLUDED_)
