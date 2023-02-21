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
	bool	bAnd;		// And ���� ������
	bool	bOr;		// Or ���� ������
	int		nFrature;	// �� ���
	int		nSign;		// �ε�ȣ
	double	dValue;		// �� ��
};

struct InfoROI_AlignAfter
{
	CRect			rectROI;
	int				nThreadID;			// ��Ƽ ������ ȯ���� ����Ͽ� ID �� ����
	InfoROI_AlignAfter()
	{
		nThreadID = 0;
	}
};

const int MAX_INSPACT_THREAD_CNT = 100;

// �˰��� �Ķ���Ͱ� �ٲ��� �����ϱ� ���� ������ �޸� ������ �̸� ū ������� Ȯ���� ���´�. ���ϴ� ������ ����ü ũ�⸦ ���� ���
const int MAX_MEM_SIZE_INSPECTION_SECTION = 8;		// ������ ����ü�� �޸� �������� Ȯ�� �� ũ��, ���� Ȯ���� ����Ͽ����Ѵ�.
const int MAX_MEM_SIZE_E_ALGORITHM_NUMBER = 16;
const int MAX_MEM_SIZE_E_DEFECT_NAME_COUNT = 65;	// KSW 16.01.26
const int MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT = 55;
//const int MAX_MEM_SIZE_E_DEFECT_JUDGMENT_COUNT = 50;	// DefineInspect.h ���� ( Area, Box, Lengrh, .... )

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
	CRect				rectROI;												// ����
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet ���
	TCHAR				strROIName[300];										// ���� �̸�
	CPoint				ptPoint[2];												// ���� ��� Mark 2��
	int					nROICnt;												// ���� ����
	bool				bAlgorithmUse[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER];			// �˰��� ��� ����
	double				dPara[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT];					// �˰��� �Ķ����
	tDefectJudgment		stDefectFilteringParam[MAX_MEM_SIZE_INSPECTION_SECTION][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];	// ���� ���͸� �Ķ����
	BOOL				bUseSegmentation;																			// ROI�� ���� ������ ���� ���� ������ ����
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
//	CRect				rectROI;												// ����
//	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet ���
//	TCHAR				strROIName[300];										// ���� �̸�
//	CPoint				ptPoint[2];												// ���� ��� Mark 2��
//	int					nROICnt;												// ���� ����
//	bool				bAlgorithmUse[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER];									// �˰��� ��� ����
//	double				dPara[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER][ALG_PARA_TOTAL_COUNT];					// �˰��� �Ķ����
//	tDefectJudgment		stDefectFilteringParam[MAX_INSPECTION_SECTION][E_ALGORITHM_NUMBER][E_DEFECT_NAME_COUNT];	// ���� ���͸� �Ķ����
//	bool				bUseSegmentation;																			// ROI�� ���� ������ ���� ���� ������ ����
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
	int					nYCoordSEGLine;											// ���� ���� Y ��ǥ
	int					nSEGCnt;												// ���� ����	
	InfoSEG()
	{
		nSEGCnt = 0;
	}
};

// 2015.08.04 ��ȿȭ ���� ROI �߰�
struct tRectInfo
{
	CRect				rectROI;												// ����
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet ���
	CPoint				ptPoint[2];												// ���� ��� Mark 2��
	TCHAR				strROIName[300];										// ���� �̸�
	int					nUsePanel;												// 2015.09.04 ���� Panel �� �˻� ���� ���� ���/���� ����
	tRectInfo()
	{
		memset(this, 0, sizeof(tRectInfo));
		nUsePanel = (int)eUseAllPanel;	// All Use Panel1, 2	// 2015.09.04 ���� Panel �� �˻� ���� ���� ���/���� ����
	}
};

struct InfoFilterROI
{
	int				nROICnt;		// ��ü ���� ����
	tRectInfo*		pRectInfo;
	InfoFilterROI()
	{
		nROICnt = 0;		
	}
};
//2016.04.06 add by mby
struct InfoMoniImgROI
{
	int				nROICnt;		// ��ü ���� ����
	tRectInfo*		pRectInfo;
	InfoMoniImgROI()
	{
		nROICnt = 0;		
	}
};

//2015.10.16 POL Mark ��ġ ��� �� �˶� ��� �߰�
struct InfoPolMarkROI
{
	int					nUseFlag;
	double				dbXsize;		//2015.10.16 POL Mark ��ġ ��� �� �˶� ��� �߰�
	double				dbYsize;		//2015.10.16 POL Mark ��ġ ��� �� �˶� ��� �߰�
	int					nMainUseFlag;	// Main �˻� ���
	tRectInfo			PolMarkRectInfo;
	InfoPolMarkROI()
	{
		memset(this, 0, sizeof(InfoPolMarkROI));
	}
};

struct InfoPolMark
{
	CRect				rectROIPol;												// ����
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet ���
	TCHAR				strROIPolName[300];										// ���� �̸�
	TCHAR				strROIPolGroup[300];									// LDH 2017.03.25 POL Mark 
	CPoint				ptPoint[2];												// ���� ��� Mark 2��
	int					nROIPolCnt;												// ���� ����	
	int					nUseFlag;												// ��� ����
	InfoPolMark()
	{
		memset(strROIPolName, 0, sizeof(strROIPolName));				
	}
};

//2015.06.05 add by mby for Measure Mark register S-
struct InfoMesMark
{
	CRect				rectROIMes;												// ����
	InfoROI_AlignAfter	rectROITemp[MAX_INSPACT_THREAD_CNT];					// OffSet ���
	TCHAR				strROIMesName[300];										// ���� �̸�
	CPoint				ptPoint[2];												// ���� ��� Mark 2��
	int					nROIMesCnt;												// ���� ����	
	int					nUseFlag;												// ��� ����
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

private:												// �ߺ� ���� ����
	CSystem(void);
	virtual ~CSystem(void);
	static CSystem* m_Instance;				// Singleton ��ü

	// LOG ���� ������ ���� Critical Section 
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
	static void ReleaseInstance();			// Singleton ����
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

// Attributes
public:

	// 2014-04-18, jhLee
	// SysHwCtrl���� �߻��� �� �ִ� Error Code
	enum {
		eERR_SYSHW_None			= 0,			// Error ����

		eERR_SYSHW_BASE			= 1000,		// CSystem�� Error ���� Code
		eERR_SYSHW_ConfigFile,					// �ʱ�ȭ ������ �������� �ʴ´�.
		eERR_SYSHW_ModelFile,					// Model file�� ����.
		eERR_SYSHW_InitStage,					// Stage �ʱ�ȭ ����
		eERR_SYSHW_InitLight,					// Serial, Light �ʱ�ȭ
		eERR_SYSHW_InitTrigger,					// Serial, Trigger �ʱ�ȭ
		eERR_SYSHW_InitSocket1,					// Socket, ù��° ���� �ʱ�ȭ ���� 
		eERR_SYSHW_InitSocket2,					// Socket, �ι�° ���� �ʱ�ȭ ���� 
		eERR_SYSHW_InitSocket3,					// Socket, ����° ���� �ʱ�ȭ ���� 
		eERR_SYSHW_BaseThread,					// Base Thread ���� ����
		eERR_SYSHW_ModuleThread,				// Module Thread ���� ����
		eERR_SYSHW_ModelSet,					// Model ���� ����

	};


	enum {			// Initial �������� �߻��ϴ� Event
		eEVT_InitStart			= 0,		// �ʱ�ȭ ���� 
		eEVT_InitPause,						// �Ͻø���
		eEVT_InitResume,					// �ʱ�ȭ �簳
		eEVT_InitAbort,						// �ʱ�ȭ �ߵ� ����
		eEVT_InitFinish,					// �ʱ�ȭ �Ϸ�

	};


	// 2014-04-18, jhLee
	// CSystem�� StartHW() �� Initial ȭ�鿡�� ����ϴ� �ʱ�ȭ �ܰ�
	enum {
		eINIT_StepNone			= 0,	// ���� �ʱ�ȭ �������� �ʴ´�.
		eINIT_StepFile,					// 1. �ʱ�ȭ ���� ���� : path validity
		eINIT_StepModel,				// 2. �� ���� Ȯ��: path validity	
		eINIT_StepStage,				// 3. Stage �ʱ�ȭ
		eINIT_StepLight,				// 4. Serial, Light �ʱ�ȭ
		eINIT_StepTrigger,				// 5. Serial, Trigger �ʱ�ȭ
		eINIT_StepSocket1,				// 6. Socket, ù��° ���� �ʱ�ȭ
		eINIT_StepSocket2,				// 7. Socket, �ι�° ���� �ʱ�ȭ
		eINIT_StepSocket3,				// 8. Socket, ����° ���� �ʱ�ȭ
		eINIT_StepBaseThread,			// 9. Base Thread ����
		eINIT_StepModuleThread,			// 10. Module Thread ����
		eINIT_StepModelSet,				// 11. Model ���� 
		eINIT_StepFinish,				// 12. �ʱ�ȭ ������	
	};

	bool m_bVisionState;				//��ư ����
	int	m_nInitSeq;						// �ʱ�ȭ�� �������� ���� 
	int	m_nInitSeqSub;					// ���� �ʱ�ȭ ����
	bool	m_bInitTerminateFlag;		// �ʱ�ȭ Thread�� ���߿� �����ؾ��ϴ��� ����, false:����, true:����
	bool	m_bInitPauseFlag;			// �ʱ�ȭ Thread�� ���߿� �Ͻø����� �ؾ� �ϴ��� ����, false:����, true:����
	CWinThread* m_pInitThread;			// Initial ���� Therad�� Handle


	void	StartInitialize();			// �ʱ�ȭ ����, (ó������ ����)
	void	PauseInitialize();			// �ʱ�ȭ�� �Ͻ������� ������Ų��.
	void	ResumeInitialize();			// �ʱ�ȭ �Ͻ������� Ǯ�� �簳�Ѵ�.
	void	AbortInitialize();			// �ʱ�ȭ�� ���� ���� ��Ų��.

	void    SetNetworkDrive();        // ��Ʈ��ũ ����̺� üũ��
 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
	CInterServerInterface*		m_pVsInterServerIF;							// For PLC Task
	VsMessageProcessor*			m_pVsMessage;								// For PLC Task

	// states
	int  volatile	m_bNowInitializing;	// �ʱ�ȭ ����
	CCriticFlag		SystemReady;
	

	int	m_iRejectDefectOption;          // Ư�� �ҷ� Reject ó���� 
	int m_nDefectCnt;
	int m_nTtcheckCnt;					//2019.11.26 add by mby
	CCriticFlag		ManualRun;			// �ʿ伺 ���� Ȯ��
	CCriticFlag		AutoRun;			// �ʿ伺 ���� Ȯ���� ��
	CCriticFlag		AutoStop;			// �ʿ伺 ���� Ȯ���� ��

	// overall
	CSupervisor		Smith;
	CWnd* volatile  pMasterWnd;			// main ȭ��

	Parameter		**m_Parameter;

	int m_nImagTestImageNum;

	CConfig			m_Config;
	CModelInfo		ModelInfo;
	CString			m_strCurrentModelName;	// ���� �𵨸�
	
	COGModelInfo	OGModelInfo;

	// H/W controls (�÷�����ũ ���� ����)
// 	CLight			FrontLight;			// 1�� controller�� 3 stage ��� (�� ���������� 1 ä��)
// 	CLight			BackLight;			// 1�� controller�� 3 stage ��� (�� ���������� 1 ä��)
//  CLight			CoaxialLight;		// �ܰ� ����������, �������� ����

// H/W controls (�÷�����ũ ���� ����)
 	CNipsLight			LineLight;          // Line8K�� ����(���������� 1 ä��)
 	CNipsLight			LineCoaxialLight;   // Line2K�� ����(���������� 2 ä��)

  	CNipsLight			FrontLight;			// Area�� ���� (���������� 4 ä��)
 	CNipsLight			BackLight;			// Area�� ���� (���������� 5 ä��)
	CNipsLight			FrontAngleLight;	// Area�� ���� (���������� 4 ä��)
    CNipsLight			CoaxialLight;		// Area�� ���� (���������� 3 ä��)

	CTrigger			Trigger;

	CString				PLCCellID;

//	CToken				Tokenizer;


//	CStage			Stage[NUM_INSP_STAGE];	// ODSystem, cam cotrol�� stage ���� (per board)
	CStage			Stage[NUM_FRAME_GRABBER];	// ODSystem, cam cotrol�� stage ���� (per board)

		// BM ���� �˻�� image buffer list
	CBMImageListCritic m_BMImageList;

	// 16.03.16 KSW
	// ���� ��ġ �Ѿ�� ��ġ ����.
	bool			m_MeasureCutFlag;

	// 16.04.04 KSW
	// ���� ����
	bool			m_DeleteWaterDirtFlag;

	// MIL : per system
 	MIL_ID			m_MilApp;
 	MIL_ID			m_MilHostSystem;
	MIL_ID			m_MilSystem;

// 	MAPPHOOKFCTPTR	HandlerPtr;
// 	void*			HandlerUserPtr;

	// �˻�
	// �г� ���� ���� (stage -> ���� �ܰ�)
 	CRunPanel			RunPanel;		// �ý��ۿ� �����Ͽ�, ���� ����� �������� ���� �г� pool
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

	int		volatile	SocketSendDefect[1000];  // ��� ������ ���� ��� �� ���� ���.
	bool	volatile	actend[1000];
	bool	volatile	ogend[1000];
	bool	volatile	OgNotUse;
	bool	volatile	resend[1000];
	short	int			nClassState[1000];		// �˻��� ���� ���� ����

	bool	m_bCameraStatus;
	bool	GetCameraStatus();
	void	SetCameraStatus(bool bStatus);

	CPoint m_ptPMIndex;

	//vision Align ��ǥ
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

	BOOL				m_bUseDefectFilter;		// �˻� ��ȿȭ ROI ��� / ����
	BOOL				m_bUseDefectMoni;		// �̰� ���� ROI ���/����

	CString				m_strServerModelName;				//2015.10.29 �������� ������ �𵨸�

	//BOOL				m_bThreadFileBusy[MAX_INSP_THREAD + 1];			// 2014.08.24 ������ �� Log ���� ���� ����

	//2016.08.31 NDH : Cache �޸� ������ ���� �׷� Ƚ�� üũ ����
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
//���� �̵�	void	StartInitialize();
	bool	InitializeVision();
	void	ExitVision();
	void	TraceLogFile(CString *pmsg, int nCameraID = NUM_INSP_CAMERA, int nThreadID = MAX_INSP_THREAD);
	void	CommLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	MemoryLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	InspectInfoLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	//jskim
	void	StartLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	TactLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);
	void	AlarmLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);	//Alarm Display�� LOG
	void	EventLogFile(CString *pmsg, int Level = LOG_LEVEL_BASIC);	//EVENT�� LOG

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

	//2016.08.31 NDH Start : Cache �޸� ������ ���� �׷� Ƚ�� üũ ����
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

	//Recipe List �۽�	2015.10.29 add by mby 
	void	SendRecipeListInfo();
	BOOL	ServerRecipeSelect(CString strRecipeName);

	void	SendContollerError(BOOL bShow, CString strMsg); //2017.02.10
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Teaching ����. Todo. �ٸ� Class �� �����Ѵ�.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	int					m_nSectionCount;			// ROI Section ����.. ACC �� ��� 8����
	CStringArray		m_strarryStrSectionName;	// ROI Section �� ǥ�� �̸�
	CPoint				m_ptMark[2];				// Teaching Align ��ũ �ΰ� ��ǥ
	CPoint				m_ptMarkRef[2];				// Teaching Align ��ũ �ΰ� ��ǥ
	InfoROI				*m_pInfoROI;				// ������ Teaching ����ü ������
	InfoSEG				*m_pInfoSEG;				// ������ Segmentation ����ü ������
	int					m_nForwardScanPadPosition;	// ù �׶��� Pad�� ��ġ,0:Left,1:Top,2:Right,3:Bottom	
	tdefectDisplyParam	*m_tdefectDisplyParam;
	int					m_nRoiModePosition;			// Roi Mode Set 0:Normal 1:Pol Mark
	InfoPolMark			*m_pInfoPolMark;			// ������ Teaching ����ü ������
	tPolMarkImageInfo	*m_ptPolMarkInfo;			// �˰��� ������ Image Buffer
	InfoMesMark			*m_pInfoMesMark;			// ������ Teaching ����ü ������
	tMesMarkInfo		*m_ptMesMarkInfo;			// �˰��� ������ Image Buffer
	InfoROI				*m_pInfoRefROI;				// Pad Scratch Reference ROI Backward ��
	MIL_ID				m_milRefBuffer[8];				// Pad Scratch Reference buffer
	// 2015.09.01 ��ũ�� Limit �뷮 ��� �߰�
	//tDriveInfoParam		*m_tDriveInfoParam;		// Disk info ����ü			

	// 15.10.06 KSW
	bool			m_bAlignCameraIndex[8];		// Teaching �� Image ���� Align �޸� �ϱ� ����.
	CPoint			m_ptAlignMarkPos[8][2];		// Image ���� Align �ΰ�
	
	InfoROI			*m_pInfoUserParam;			// 2015.06.26 User Parameter Teaching ����ü ������ by CWH

	InfoFilterROI	*m_pInfoFilterROI;
	InfoPolMarkROI	m_InfoPolMarkROI;			//2015.10.16 POL Mark ��ġ ��� �� �˶� ��� �߰�
	InfoMoniImgROI	*m_pInfoMoniImgROI;			//2016.04.06 add by mby

	int				m_nActiveRoiSel;
	int				m_nDirection;

	//18.03.13 Lyh 
	int				m_LineFittingTh; //Line Fitting Align�� Th

	//21.06.07 add by mby 
	int				m_TTtime; //TT Time Recipeȭ
	

public:
	int			GetTeachingToatalROICount();										// Teaching���� ������ ROI �� ���� ���
	CRect		GetTeachingROI(int nROINum);										// Teaching���� ������ ROI ���� ���.
	LPCTSTR		GetTeachingROIName(int nROINum);										// 2015.09.16 ������ ROI �̸� ���
	CRect		GetTeachingRefROI(int nROINum);
	CPoint		GetTeachingMark1(int nROINum);										// Teaching���� ROI ���� ��� Align Mark 1 ��ǥ ���.
	CPoint		GetTeachingMark2(int nROINum);										// Teaching���� ROI ���� ��� Align Mark 2 ��ǥ ���.
	CPoint		GetTeachingMarkRef1(int nROINum);									// Teaching���� ROI ���� ��� Align Mark 2 ��ǥ ���.
	CPoint		GetTeachingMarkRef2(int nROINum);									// Teaching���� ROI ���� ��� Align Mark 2 ��ǥ ���.
	int			GetTeachingToatalDefectCount();										// Teaching���� ������ Defect List �� ���� ���
	bool		GetTeachingDefectUse(int nROINum, int nDefectNum, int nInspSectionNum);					// Teaching���� ������ ROI�� Defect List üũ Ȯ��.
	double		GetTeachingParameter(int nROINum, int nDefectNum, int nParaNum, int nInspSectionNum);	// Teaching���� ������ ROI -> Defect -> �Ķ���� ���. ( 0~ 39 )
	CRect		GetTeachingAlignROI(int nROINum, int nThreadID);					// Align Mark ã�� ��, ���� ROI ���� ���.
	CRect		GetTeachingAlignRefROI(int nROINum, int nThreadID);					// Align Mark ã�� ��, ���� ROI ���� ���.
	int			GetTeachingToatalSEGCount();										// Teaching���� ������ SEG �� ���� ���
	int			GetTeachingSEG(int nSEGNum);										// Teaching���� ������ SEG ���� ���.

	int			GetTeachingScanPadPos();	
	void		SetTeachingAlignRefROI(int nROINum, CRect rect, int nThreadID);		// Align Mark ã�� ��, ���� ROI ���� Reference �����ͷ� ����.

	int			GetRoiMode();														//ROI MODE ���� Normal,Pol Mark 2015.05.20 add by mby
	//void		CopyTeachingParameter(double *dDstArr, int nROINum, int nDefectNum, int nInspSectionNum);
	void		CopyTeachingParameter(double *dDstArr, int nROINum, int nDefectNum, int nInspSectionNum, int Mode = 0);
	//void		GetDefectFilteringParam(tDefectJudgment* dDstArr, int nCnt, int nROINum, int nAlgorithmNumber, int nInspSectionNum);
	void		GetDefectFilteringParam(tDefectJudgment* dDstArr, int nCnt, int nROINum, int nAlgorithmNumber, int nInspSectionNum, int Mode = 0 );
	int			SetTeachingScanPadPos(int nPos);	
	int			SetRoiMode(int nPos);												//ROI MODE ���� Normal,Pol Mark 2015.05.20 add by mby
	void		SetTeachingAlignROI(int nROINum, CRect rect, int nThreadID);		// Align Mark ã�� ��, ���� ROI ���� ����.

	//2015.06.07 add by mby Disk info-
	void		SearchDriveDisk();													// Local Drvie Disk Search
	void		SearchDiskInfo(LPCTSTR lpDriver , int count);						// ���� Drive Disk�뷮 Ȯ��
	float		GetByte(ULARGE_INTEGER dwParam);									// ����Ʈ ���
	void		GetUseDrivePath();													// ����ϴ� Drive ��� ����
	int			ProcessRamDiskFile();												// RAM Drive �� ����ϴ� ��� ���� ó��
	// 2015.09.01 Comment by CWH
	//BOOL		m_fnFormatImg();
	//BOOL		DeleteDirectoryFile(LPCTSTR RootDir);								// ���� �������� ����.
	//2015.06.07 add by mby Disk info-

	// 2015.08.04 by CWH ��ȿȭ ���� ROI
	int			GetTeachingToatalFilterROICount(void);									// Filter ROI Count
	CRect		GetTeachingFilterROI(int nROINum);										// Filter ROI Rect
	LPCTSTR		GetTeachingFilterROIName(int nROINum);									// Filter ROI Name
	void		SetTeachingAlignFilterROI(int nROINum, CRect rect, int nThreadID);		// Filter ROI Thread �� Align ���� Setting
	CPoint		GetTeachingMarkFilter1(int nROINum);									// Filter ROI Setting ��� Mark Position 1
	CPoint		GetTeachingMarkFilter2(int nROINum);									// Filter ROI Setting ��� Mark Position 2
	CRect		GetTeachingAlignFilterROI(int nROINum, int nThreadID);					// Align Mark ã�� ��, ���� ROI ���� ���.

	// 2015.09.04 ��ȿȭ ���� Panel �� ��� ���� ��� �߰�
	int			GetUsePanelFilterROI(int nCnt);

	// 2016.04.06 by mby MONI IMGAE ���� ROI S-
	int			GetTeachingToatalMoniROICount(void);									// Moni Image ROI Count
	CRect		GetTeachingMoniROI(int nROINum);										// Moni Image ROI Rect
	LPCTSTR		GetTeachingMoniROIName(int nROINum);									// Moni Image ROI Name
	void		SetTeachingAlignMoniROI(int nROINum, CRect rect, int nThreadID);		// Moni Image ROI Thread �� Align ���� Setting
	CPoint		GetTeachingMarkMoni1(int nROINum);										// Moni Image ROI Setting ��� Mark Position 1
	CPoint		GetTeachingMarkMoni2(int nROINum);										// Moni Image ROI Setting ��� Mark Position 2
	CRect		GetTeachingAlignMoniROI(int nROINum, int nThreadID);					// Moni Image Mark ã�� ��, ���� ROI ���� ���.
	int			GetUsePanelMoniROI(int nCnt);
	void		SetUseDefectMoniROI(BOOL bUseMoni);
	BOOL		GetUseDefectMoniROI();
	// 2016.04.06 by mby MONI IMGAE ���� ROI E-

	int         m_nManualLightValue;
	BOOL        m_bGrabLight;
	BOOL        m_bManualLight;

	void		SetUseDefectFilterROI(BOOL bUseFilter);
	BOOL		GetUseDefectFilterROI();
// UI - System Interface �Լ� �߰�
	void		CallCalculate_OffSetRef(MIL_ID milDrawBuffer, int nPosition, bool bClockWise, tInfoAlign InfoAlign, int nThreadID);

	//18.03.13 Lyh LineFitting Th ���� �߰�.
	int			GetLineFittingTh();	
	void		SetLineFittingTh(int Value);	

	//21.06.07  add by mby T/T Time Reciepȭ
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
