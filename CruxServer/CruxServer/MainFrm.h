
// MainFrm.h : CMainFrame Ŭ������ �������̽�
//

#pragma once

#include "CruxDefine.h"
#include "DlgTopBar.h"			// Top Dialogbar
#include "DlgBottomBar.h"		// Bottom Dialogbar	
#include "Util\BIniFile.h"			// INI ���� ������
#include "LogMng\logmng.h"
#include "CruxData.h"
#include "SockManager.h"
#include "DataManager.h"
#include "ThreadSeqPLC.h"
#include "ThreadStatePLC.h"
#include "ThreadSeqCIM.h"
#include "ThreadProcPLC.h"
#include "ThreadProcAlign.h"	//2015.08.19
//#include "FtpClient.h"
#include "ServerWorkManager.h"	 //2015.12.18 add by ikm for PLC Task
#include "RamdomVal.h"			//180911 by hikim
class CInterServerInterface;

class CMainFrame : public CFrameWnd
{
	
protected: // serialization������ ��������ϴ�.
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

private:
	CDlgTopBar			m_dDlgTopBar;
	CDlgBottomBar		m_dDlgBottomBar;
	CCruxData*			m_pCruxData;		// GUI ���� Data�� �����ϴ� Data Class ����	

	BOOL m_bLockFormChange;			// ȭ�� ���� ���� ����
	BOOL m_bOnlyOnce;				// ���� 1ȸ �����
	BOOL m_bTerminateFlag;

	HANDLE m_hDetectVisionClient;	//2016.01.29 Mr.Chu			VisionClient ���� ����
	HANDLE m_hThreadInspAlive;	
	static unsigned __stdcall  m_fnDetectVisionClient(LPVOID pArg );		//2016.01.29 Mr.Chu			VisionClient ���� ����
	static unsigned __stdcall  m_fnThreadPcAliveChk(LPVOID pArg );
	int	m_nShutdownSeq;				// ���� ����� ���� ����

	void m_fnGetCurrentViewPointer();

	void m_fnInitEnv();
	void m_fnInitTopBar();
	void m_fnInitState();

	
// Ư���Դϴ�.
public:

	CRamdomVal	 m_RandomVal;	//180911 by hikim
	CServerWorkManager* m_pVsSeverIF;
	STRU_COMM_INFO m_stCommInfo;
	STRU_PARAM_INFO m_stParamInfo;
	CView* m_pViewAuto;
	//CView* m_pViewManual;
	CView* m_pViewReport;
	CView* m_pViewSetup;
	CView* m_pViewIOView;


	UINT m_nViewID;

	CBIniFile   m_iniConfigSet;
	CBIniFile   m_iniOverlapCurCoord;		//2016.11.21
	CBIniFile	m_iniConfigPLCMap;
	CBIniFile   m_iniRecipe;	//2015.11.05
	//2015.02.03 add by ikm for FTP - S
	STRU_FTP_INFO m_stFtpInfo;
	CBIniFile   m_iniConfigFtp;
	//CFtpClient  m_FtpClient;	
	//CFtpClient  m_FtpImgClient;
	CLogMng		m_QMSGuestLog;		//2017.06.27 ����Panda QMS ���� Log
	CLogMng		m_QMSGuestImgLog;		//2017.06.27 ����Panda QMS ���� Log
	CLogMng		m_TemperatureLog;	
	CLogMng		m_FtpPriodData;	
	CLogMng		m_AlarmLog;
	CLogMng		m_ResetTimeLog;
	STRU_FTP_CONDITION m_stCurFtpCond;
	//2015.02.03 add by ikm for FTP - E
	CLogMng m_LogMng;
	//CFtpClient m_FtpLogMng;
	STRU_PLC_STAGEINFO *m_stStageInfo;	//2015.05.13
	STRU_OVERLAP_COORD_INFO *m_stOverlapCoordInfo;	//2015.06.02
	STRU_TDI_OVERLAP_COORD_INFO *m_stTdiOverlapCoordInfo;	//2015.06.11
	STRU_JUDGE_COUNT   *m_stJudgeCount;	//2015.07.20
	STRU_OVERLAP_LUMINANCE_INFO *m_stOverlapLuminanceInfo;	//2016.03.02

	static CCriticalSection m_crtsecLog;

	CSockManager* m_pSockManager;
	CDataManager m_DataManager;

	CThreadSeqPLC* m_pThreadSeqPLC[MAX_VISION_PC];
	CThreadStatePLC m_ThreadStatePLC;
	CThreadSeqCIM* m_pThreadSeqCIM[CIM_MAX];
	CThreadProcPLC* m_pThreadProcPLC[SEQ_PROC_PLC_MAX];
	CThreadProcAlign* m_pThreadProcAlign;	//2015.08.19

	void m_fnAddLog(CString strLog, BOOL bSeq = FALSE, int nStageID = 0, BOOL bNoUseTotalLog = FALSE);

	//void m_fnUpdateGridLog(CString strLog, BOOL bSeq = FALSE, int nStageID = 0);
	void m_fnUpdateGridLog(CString strLog, BOOL bSeq = FALSE, int nStageID = 0, BOOL bNoUseTotalLog = FALSE); // 2015.11.27 comment by ikm for Log Delay
	void m_fnUpdateGridDefectSummary(CString strPath);

	void m_fnInitINIFile();
	void m_fnInitLogMng();
	void m_fnInitSockMng();

	void m_fnInitPLC();
	void m_fnInitThreadSeqPLC();
	void m_fnDeInitThreadSeqPLC();
	void m_fnInitThreadStatePLC();
	void m_fnDeInitThreadStatePLC();
	void m_fnInitThreadSeqCIM();
	void m_fnDeInitThreadSeqCIM();
	void m_fnInitThreadProcPLC();
	void m_fnDeInitThreadProcPLC();
	void m_fnInitThreadProcAlign();	//2015.08.19
	void m_fnDeInitThreadProcAlign();	//2015.08.19

	void m_fnUpdateSockStatus();
	void m_fnUpdateTopBarStatus();

	int m_fnCreateDialogBar(void);				// DialogBar ����
	void m_fnDestoryDialogbar(void);			// DialogBar ����

	int m_fnCreateSubForm(void);				// VIew ȭ���� �����Ѵ�.
	void m_fnDestorySubForm(void);				// View ȭ���� �����Ѵ�.

	void m_fnChangeForm(UINT nFrmID, BOOL bForce);
	void m_fnChangeModelName();

	BOOL m_fnCompareView(int nID);
	CView* m_fnGetViewPtr(int nID);

	void m_fnShutdownProcess(void);				// ����� ó��

	CString m_fnGetPanelID_1(int nStageID);
	CString m_fnGetPanelID_2(int nStageID);

	//2015.02.03 add by ikm for FTP - S
	BOOL m_fnSetFtpIniFile();
	BOOL m_fnCreateFolder(CString szPath);
	BOOL m_fncheckPathExists(LPCTSTR szPath);	
	//2015.02.03 add by ikm for FTP - E

	BOOL    m_bPLCManual;	// ���� PLC���°� �������� ���������� �Ǻ��ϴ� ����	// 2015.03.26 LJK
	CString m_strCellID;	// CellID�� �����ϴ� �Լ�	// 2015.03.26 ljk
	int		m_nStageNum;	// ���������� �����ϴ� ����	// 2015.05.02 ljk

	BOOL	m_bIsGrabStartRequest;	// 2015.07.02 ljk

	CString	m_strJudge_1;									// 2015.07.09 ljk
	BOOL    m_bIsEngineerModeMsg[MAX_VISION_PC];			// 2015.07.09 ljk
	int	    m_nSelectEngineerMode[MAX_VISION_PC];			// 2015.07.09 ljk
	BOOL m_fnSetStageInfo();	//2015.05.13
	BOOL m_fnSetOverlapCoordInfo();	//2015.06.02
	BOOL m_fnSetTdiOverlapCoordInfo();	//2015.06.11
	BOOL m_fnSetOverlapLuminanceInfo();	//2016.03.02

	void m_fnSetVersion();	//2017.04.04 Version

	void m_fnLoadJudgeCount();	//2015.07.20
	int m_fnEqpSizeType(void);
	BOOL	m_bQmsConnection;	//2015.11.20
	BOOL    m_bAutoRecipe[MAX_VISION_PC];

	DOUBLE  m_dReferencePoint_X[2], m_dReferencePoint_Y[2];
	DOUBLE  m_dYPos_L[2],m_dYPos_R[2];

	DOUBLE  m_dPosion_X[2][4], m_dPosion_Y[2][4];
	DOUBLE  m_dPosion_OffsetX[2][4], m_dPosion_OffsetY[2][4];
	void m_fnProcessMapSave();				// ADD By KYH01

	BOOL	m_bMJudgeFlag;	//2019.01.03 add by mby
// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �����Դϴ�.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
	//afx_msg LRESULT OnSelectEngineerMode(WPARAM wParam, LPARAM lParam);		// 2015.07.09 ljk
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg LRESULT OnMainMenuChange(WPARAM wParam, LPARAM lParam);						// ȭ�� ���� ��û
	afx_msg void OnDestroy();
	double m_fnGetProfileDouble(CString strJudgeKind1, CString strJudgeKind2, CString strDefault, CString strFilePath );
	void m_fnInitBinaryData(void);
	void m_fnReportPCAlive();	//2015.10.31 Mr.Chu
	void m_fnSetSystemTime();	//2016.03.14
	void m_fnSetLastRecipeNo();
	void m_fnSetAlignLive(ULONG lLive);
};


