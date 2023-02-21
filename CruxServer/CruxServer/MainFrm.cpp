
// MainFrm.cpp : CMainFrame 클래스의 구현
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "CruxServer.h"

#include "MainFrm.h"

#include "ViewAuto.h"
#include "ViewManual.h"
#include "ViewReport.h"
#include "ViewSetup.h"
#include "ViewIOView.h"

#include "SelectEngineerModeDlg.h"	// 2015.07.09 ljk
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

#include "..\version.h"

CInterServerInterface* G_pVsServerIF = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_TIMER()

	ON_MESSAGE(UM_MAINMENU_CHANGE,	OnMainMenuChange)
	//ON_MESSAGE(UM_SELECT_ENGINEER_MODE, OnSelectEngineerMode)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


void G_AddLog(int nLogLevel, int nLogPriority, const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	
	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );
	
	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);

	//CString strLog;
	//strLog = buf;

	switch (nLogLevel)
	{
	case 1: //List Log
		if(nLogPriority <= pFrame->m_LogMng.m_nCurrentLogPriority)
			pFrame->m_fnAddLog((LPTSTR)buf);
		break;
	case 2: //File Log
		//pFrame->m_fnAddLog(buf, TRUE);
		//2015.07.03 Chu
		if(nLogPriority <= pFrame->m_LogMng.m_nCurrentLogPriority) // 2016.07.27 sgkim
			pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1);
		break;
	case 3: //List + File Log
		if(nLogPriority <= pFrame->m_LogMng.m_nCurrentLogPriority)
		{
			pFrame->m_fnAddLog(buf);//2015.07.03 Chu
			pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
		}
		break;
	case 4:
		if(nLogPriority <= pFrame->m_LogMng.m_nCurrentLogPriority)
		{
			pFrame->m_fnAddLog(buf, FALSE, 0, TRUE);
			pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, TRUE);
		}
		
		break;
	default:
		AfxMessageBox(_T("Log Level Error"), MB_TOPMOST);
		break;
	}
}

void G_AddLog_SEQ(int nStageID, const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);

	pFrame->m_fnAddLog(buf, TRUE, nStageID);
	//2015.07.03 Chu
	pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE, nStageID);
}
// CMainFrame 생성/소멸

void G_AddLog_ALARM(const wchar_t* chLog, ...) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);
	pFrame->m_AlarmLog.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
}
void G_AddLog_RESET(const wchar_t* chLog, ...) 
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);
	pFrame->m_ResetTimeLog.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
}
void G_AddLog_IMG_QMS( const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);
	pFrame->m_QMSGuestImgLog.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
}

//2017.06.27 성도Panda QMS 고객사 Log
void G_AddLog_QMS( const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);
	pFrame->m_QMSGuestLog.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
}

//2016.04.05 add by ikm - 온도값 전용 로그
void G_ADDLOG_TEMPERATURE(const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);
	pFrame->m_TemperatureLog.m_fnLogMngWriteLog(buf, FALSE, TRUE,-1, FALSE);
}

void G_AddLog_STG_SEQ(int nLogLevel, int nStageID, const wchar_t* chLog, ...)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;

	static TCHAR buf[1024] = {0};

	va_list marker;
	va_start( marker, chLog );

	int   result(-1), length(1024);

	if ((chLog == NULL) || (buf == NULL))
		return ;

	while (true)
	{
		//문자열을 포메팅한다.
#ifdef _UNICODE
		result = _vsnwprintf_s(buf, 1024, (const wchar_t*)chLog, marker);
#else
		result = _vsnprintf(buf, 1024, (const wchar_t*)chLog, marker);
#endif

		//문자열이 정상적으로 포메팅되었다면 종료한다.
		if (result != -1)
			break;

		//문자열을 포메팅하는데 필요한 공간이 부족했다면, 공간을 이전의 두배로 할당한다.
		length *= 2;
	}

	va_end(marker);


	switch (nLogLevel)
	{
	case 1: //List Log
		pFrame->m_fnAddLog(buf, TRUE, nStageID, TRUE);
		break;
	case 2: //File Log
		//pFrame->m_fnAddLog(buf, TRUE);
		//2015.07.03 Chu
		pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE, nStageID, TRUE);
		break;
	case 3: //List + File Log
		pFrame->m_fnAddLog(buf, TRUE, nStageID, TRUE);
		pFrame->m_LogMng.m_fnLogMngWriteLog(buf, FALSE, TRUE, nStageID, TRUE);
		break;
	default:
		AfxMessageBox(_T("Log Level Error"), MB_TOPMOST);
		break;
	}
}

// CMainFrame 생성/소멸
CMainFrame::CMainFrame()
{
	m_bOnlyOnce				= TRUE;		// 최초 1회 수행용
	m_bTerminateFlag		= FALSE;	// 프로그램을 종료해도 되는지 여부

	m_nShutdownSeq			= 0;			// 최후 종료시 동작 순서

	m_pViewAuto = NULL;
	//m_pViewManual = NULL;
	m_pViewReport = NULL;
	m_pViewSetup = NULL;
	m_pViewIOView = NULL;	
	for(int i = 0; i < CIM_MAX; i++)
		m_pThreadSeqCIM[i] = NULL;
	for(int i = 0; i < SEQ_PROC_PLC_MAX; i++)
		m_pThreadProcPLC[i] = NULL;

	m_pThreadProcAlign = NULL;	//2015.08.19
	m_pSockManager = NULL; 
	m_bPLCManual = FALSE;	// 2015.03.26 ljk

	m_strCellID = _T("");
	m_nStageNum = -1;		// 2015.05.02 ljk

	m_bIsGrabStartRequest = FALSE;	// 2015.07.02 ljk

	m_strJudge_1				 = _T("");	// 2015.07.09 ljk
	for(int i = 0; i < MAX_VISION_PC; i++)	// 2015.07.09 ljk
	{
		m_bIsEngineerModeMsg[i]	 = FALSE;	
		m_nSelectEngineerMode[i] = 0;
	}

	m_bMJudgeFlag = FALSE;	//2019.01.03 add by mby

	m_pVsSeverIF	= NULL;

	// memory init 
	m_stJudgeCount			 = NULL;
	m_stStageInfo			 = NULL;
	m_stOverlapCoordInfo	 = NULL;
	m_hDetectVisionClient    = NULL;
	m_hThreadInspAlive       = NULL;
	m_stOverlapLuminanceInfo		 = NULL;	//2016.03.02

	memset(m_bAutoRecipe, FALSE, sizeof(BOOL)*MAX_VISION_PC);
}

CMainFrame::~CMainFrame()
{

	delete m_stTdiOverlapCoordInfo;
	delete m_stJudgeCount;
	delete m_stStageInfo;
	delete[] m_stFtpInfo.stInfo;
	delete m_stFtpInfo.stPath;
	delete m_stOverlapCoordInfo;
	delete m_stOverlapLuminanceInfo;	//2016.03.02
	SAFE_DELETE(m_pVsSeverIF);
	if (m_stParamInfo.nEqpObject != P_PROJECT)
	{
		for(int i = 0; i < CIM_MAX; i++)
		{
			if(m_pThreadSeqCIM[i])
			{
				delete m_pThreadSeqCIM[i];
				m_pThreadSeqCIM[i] = NULL;
			}				
		}
		for(int i = 0; i < SEQ_PROC_PLC_MAX; i++)
		{
			if(m_pThreadProcPLC[i])
			{
				delete m_pThreadProcPLC[i];
				m_pThreadProcPLC[i] = NULL;
			}
		}		
	}
	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		if(m_pThreadSeqPLC[i])
		{
			delete m_pThreadSeqPLC[i];
			m_pThreadSeqPLC[i] = NULL;
		}
	}

	if(m_pSockManager)
	{
		delete m_pSockManager;
		m_pSockManager = NULL;
	}

	CloseHandle(m_hDetectVisionClient);	

	//2015.08.19
	if(m_pThreadProcAlign)
	{
		delete m_pThreadProcAlign;
		m_pThreadProcAlign = NULL;
	}

	if(m_hThreadInspAlive)
	{
		CloseHandle(m_hThreadInspAlive);
		m_hThreadInspAlive = NULL;
	}
}
	

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	// Set Frame size & Remove Menu
	cs.x		= 0;
	cs.y		= 0;
	cs.cx		= 1280;
	cs.cy		= 1040;
	cs.hMenu	= NULL;					// 메뉴 제거

	cs.lpszName = _T("YWdsp Crux Server GUI");

	cs.style = WS_OVERLAPPED | WS_CAPTION | /*FWS_ADDTOTITLE |*/ WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame 메시지 처리기
void CMainFrame::m_fnAddLog(CString strLog, BOOL bSeq /*= FALSE*/, int nStageID /*= 0*/, BOOL bNoUseTotalLog /*= FALSE*/)
{
	CString strCurrentTime;
	CTime ctCurrentTime;
	ctCurrentTime = CTime::GetCurrentTime();
	strCurrentTime = ctCurrentTime.Format(_T("%H시%M분%S초 : "));

	strLog = /*strCurrentTime +*/ strLog;
	m_fnUpdateGridLog(strLog, bSeq, nStageID, bNoUseTotalLog);
	strLog.ReleaseBuffer();
}

void CMainFrame::m_fnUpdateGridLog(CString strLog, BOOL bSeq /*= FALSE*/, int nStageID /*= 0*/, BOOL bNoUseTotalLog /*= FALSE*/)
{
	BOOL bLog = FALSE;
	CCruxData *pCruxData = CCruxData::m_fnGetInstance();

	if(pCruxData->m_fnGetCurrentViewCaption() == _T("Auto"))
	{
		m_pViewAuto = (CViewAuto*)GetActiveView();

		//2015.07.21 Chu
		for(int i=0;i<9;i++)
			((CViewAuto*)m_pViewAuto)->m_bLog_Sel[i] = FALSE;

		if(bSeq)
		{
			//2015.07.03 Chu
			((CViewAuto*)m_pViewAuto)->m_fnCheakLogFunc(nStageID);

			if(nStageID == 0)		bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[0];
			else if(nStageID == 1)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[1];
			else if(nStageID == 2)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[2];
			else if(nStageID == 3)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[3];
			else if(nStageID == 4)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[4];
			else if(nStageID == 5)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[5];
			else if(nStageID == 6)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[6];
			else if(nStageID == 7)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[7];
			else if(nStageID == 8)	bLog = ((CViewAuto*)m_pViewAuto)->m_bLog_Sel[8];

			if(bLog)	((CViewAuto*)m_pViewAuto)->SendMessage(WM_UI_ADD_LOG,bNoUseTotalLog,(LPARAM)(LPCTSTR)strLog );//m_fnAddLog(strLog, bNoUseTotalLog);
		}
		else
				((CViewAuto*)m_pViewAuto)->SendMessage(WM_UI_ADD_LOG,bNoUseTotalLog,(LPARAM)(LPCTSTR)strLog );//m_pViewAuto)->m_fnAddLog(strLog, bNoUseTotalLog);
		
	}
	/*else if (pCruxData->m_fnGetCurrentViewCaption() == _T("Manua_T("))
	{
		m_pViewManual = (CViewManual*)GetActiveView();
		((CViewManual*)m_pViewManual)->m_fnAddLog(strLog);
	}*/
}

void CMainFrame::m_fnUpdateGridDefectSummary(CString strPath)
{
	CCruxData *pCruxData = CCruxData::m_fnGetInstance();
	//CString strData;
	//BOOL bFirst = TRUE;	
	//m_pViewAuto = (CViewAuto*)GetActiveView();	

	m_pViewAuto = m_pCruxData->m_fnGetViewPtr(eFORM_Auto);
	((CViewAuto*)m_pViewAuto)->SendMessage(WM_UI_CONTROL_DEF_SUMMARY_LIST, (WPARAM)0, (LPARAM)(LPCTSTR)strPath);
	//((CViewAuto*)m_pViewAuto)->UpdateGridDetectSummaryRow();
	
	m_pViewAuto->Invalidate(FALSE);
}


int CMainFrame::m_fnCreateDialogBar(void)
{
	// DialogBar 생성
	// Top Menu -------------------------------------------------------------------------
	if( !m_dDlgTopBar.Create(this, IDD_DLG_TOPBAR, CBRS_TOP | CBRS_HIDE_INPLACE, IDD_DLG_TOPBAR) )
		{ return FALSE; }

	// Buttom Menu -------------------------------------------------------------------------
	if( !m_dDlgBottomBar.Create(this, IDD_DLG_BOTTOMBAR, CBRS_BOTTOM | CBRS_HIDE_INPLACE, IDD_DLG_BOTTOMBAR) )	
		{ return FALSE; }

	return TRUE;
}

// Delete Dialogbar
void CMainFrame::m_fnDestoryDialogbar()
{
	m_dDlgTopBar.DestroyWindow();
	m_dDlgBottomBar.DestroyWindow();
}

//Sub View 화면을 생성한다.
int CMainFrame::m_fnCreateSubForm(void)
{
	// Crux Data Class에 View 및 Caption을 저장한다.
	m_pCruxData->m_fnAddView(eFORM_Main,			GetActiveView(),			_T("Main"), FALSE); // Main View 할당, 기본 FormView이므로 새로이 할당하지 않는다.

	// 새로운 Form들을 생성한다.
	m_pCruxData->m_fnAddView(eFORM_Auto,	(CView*) new CViewAuto,		_T("Auto"));
//	m_pCruxData->m_fnAddView(eFORM_Manual,	(CView*) new CViewManual,	_T("Manua_T("));
	m_pCruxData->m_fnAddView(eFORM_Report,	(CView*) new CViewReport,	_T("Report"));
	m_pCruxData->m_fnAddView(eFORM_Setup,	(CView*) new CViewSetup,	_T("Setup"));
	m_pCruxData->m_fnAddView(eFORM_IOView,	(CView*) new CViewIOView,	_T("IOView"));

	return 0;
}

void CMainFrame::m_fnDestorySubForm(void)
{
}

// 지정한 화면으로 변경한다.
void CMainFrame::m_fnChangeForm(UINT nFrmID, BOOL bForce)
{
	if ( m_bLockFormChange ) {						// 화면 전환 금지에 걸려있다.
		if ( !bForce )		return;					// 강제로 화면을 전환하라는 지시가 아니라면 화면 전환 금지
	}

	m_nViewID = nFrmID;

	m_pCruxData->m_fnSetChangeView(nFrmID);			// 지정된 화면으로 변경한다.

	if(nFrmID == eFORM_IOView)
	{	
		if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
			m_ThreadStatePLC.m_bReadPLC = TRUE;

		CView* pView = m_fnGetViewPtr(nFrmID);
		pView->SetTimer(TIMER_UPDATE_MAP_DATA, TIMER_UPDATE_MAP_DATA_INTERVAL, NULL);
	}
	else
	{
		// 2015.06.25 edit by IKM for PLC 정보 사용을 위해 IO VIEW 탭에서만 사용 하던 IO 를 전체 공개로 바꿈
		if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
			m_ThreadStatePLC.m_bReadPLC = FALSE;
			//m_ThreadStatePLC.m_bReadPLC = TRUE; 

		CView* pView = m_fnGetViewPtr(nFrmID);
		pView->KillTimer(TIMER_UPDATE_MAP_DATA);
		//pView->SetTimer(TIMER_UPDATE_MAP_DATA, TIMER_UPDATE_MAP_DATA_INTERVAL, NULL);
	}

	// Top DialogBar에 메인 메뉴가 변경되었음을 표시해준다.
	::PostMessage(m_dDlgTopBar.m_hWnd, UM_MAINMENU_CHANGE, nFrmID, 0);		// 화면 변경을 알려준다.
}

void CMainFrame::m_fnChangeModelName()
{
	//Model 변경을 알려준다.
	::PostMessage(m_dDlgTopBar.m_hWnd, UM_MODEL_CHANGE, 0, 0); 
}

void CMainFrame::m_fnGetCurrentViewPointer()
{
	CCruxData *pCruxData = CCruxData::m_fnGetInstance();

	if (pCruxData->m_fnGetCurrentViewCaption() == "Auto")
		m_pViewAuto = (CViewAuto*)GetActiveView();
	/*if (pCruxData->m_fnGetCurrentViewCaption() == "Manua_T(")
		m_pViewManual = (CViewManual*)GetActiveView();*/
	if (pCruxData->m_fnGetCurrentViewCaption() == "Report")
		m_pViewReport = (CViewReport*)GetActiveView();
	if (pCruxData->m_fnGetCurrentViewCaption() == "Setup")
		m_pViewSetup = (CViewSetup*)GetActiveView();
	if (pCruxData->m_fnGetCurrentViewCaption() == "IOView")
		m_pViewIOView = (CViewIOView*)GetActiveView();
}

CView* CMainFrame::m_fnGetViewPtr(int nID)
{
	CCruxData *pCruxData = CCruxData::m_fnGetInstance();

	return pCruxData->m_fnGetViewPtr(nID);
}

void CMainFrame::m_fnInitEnv()
{
	m_fnInitLogMng();

	G_AddLog(3, 2, _T("MainFrame m_fnInitEnv()"));

 	m_pVsSeverIF = new CServerWorkManager(this);		// For PLC Task
 	m_pVsSeverIF->m_fnStartVsMsgReceiver();				// For PLC Task
	m_bLockFormChange = FALSE;				// 화면 변경 금지 해제
	m_fnChangeForm(eFORM_Auto, TRUE);		// 종료시에는 강제로 Main 화면으로 변경시킨다.
	m_fnChangeModelName();
	m_dDlgBottomBar.Init();					// 메뉴버튼을 활성화 시킨다.
	m_fnInitINIFile();
	m_DataManager.m_fnInitFtpClient();
	m_DataManager.m_fnClearSummary();		// 2015.01.12 ljk
	m_DataManager.m_fnLoadSummary();

	CView* pView = NULL;

	pView = m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	((CViewSetup*)pView)->m_fnLoadIPList(FALSE);
	((CViewSetup*)pView)->m_fnLoadParam(FALSE);

	pView = m_pCruxData->m_fnGetViewPtr(eFORM_IOView);
	((CViewIOView*)pView)->m_fnLoadMapConfig();

	m_DataManager.m_fnSetBackupDay(m_stParamInfo.nSummaryBackupDay);	// Summary 파일 백업 주기 설정

	m_fnInitSockMng();

	if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
	{
		m_fnInitThreadStatePLC();
		m_fnInitThreadSeqPLC();
		m_fnInitThreadProcPLC();
		if (m_stParamInfo.nEqpObject != P_PROJECT)
		{
			m_fnInitThreadSeqCIM();
			//2017.02.18 add by mby S-
			if( m_pVsSeverIF != NULL && m_fnEqpSizeType() == EQP_MONITOR_SIZE )	
			{
				m_pVsSeverIF->m_fnRequestMonitorInfo();
			}
			//2017.02.18 add by mby E-
			//m_fnInitThreadProcAlign();	//2015.08.19	2016.03.07 Mr.Chu		측정 Sequence 변경 ( Grab 할 때 측정 함 )
		}
	}
	m_fnInitBinaryData();

	m_fnInitTopBar();

	//2016.01.29 Mr.Chu		VisionClient 감시 Thread
	if(m_hDetectVisionClient == NULL)
	{
		UINT uiThreadID;
		m_hDetectVisionClient = (HANDLE)::_beginthreadex(NULL, 0, m_fnDetectVisionClient, (LPVOID)this, 0, &uiThreadID);
	}	
	m_fnInitState();

	m_DataManager.m_fnftnRun();	//2017.03.10

	m_fnSetVersion();	//2017.04.04
	
	m_fnSetSystemTime();	//2016.03.14	

	m_fnSetLastRecipeNo();		// ADD By KYH01
	//((CViewAuto*)m_pViewAuto)->m_fnInitGradeSet();	//2018.12.13 add by ryu
}

void CMainFrame::m_fnInitTopBar()
{
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_1)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_2)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_3)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_4)->EnableWindow(FALSE);
	//2015.05.12 add s-
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_5)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_6)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_7)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_8)->EnableWindow(FALSE);
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_9)->EnableWindow(FALSE);
	//2015.05.12 add e-
	m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_CIM)->EnableWindow(FALSE);

	for(int i = 0; i < m_stParamInfo.nCountVisionClient; i++)
	{
		if(i == 0)		
		{	
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_1)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_1)->ShowWindow(SW_SHOW);
		}
		else if(i == 1)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_2)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_2)->ShowWindow(SW_SHOW);
		}
		else if(i == 2)
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_3)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_3)->ShowWindow(SW_SHOW);
		}	
		else if(i == 3)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_4)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_4)->ShowWindow(SW_SHOW);
		}
		else if(i == 4)
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_5)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_5)->ShowWindow(SW_SHOW);
		}
		else if(i == 5)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_6)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_6)->ShowWindow(SW_SHOW);
		}
		else if(i == 6)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_7)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_7)->ShowWindow(SW_SHOW);
		}
		else if(i == 7)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_8)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_8)->ShowWindow(SW_SHOW);
		}
		else if(i == 8)	
		{
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_9)->EnableWindow(TRUE);
			m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_9)->ShowWindow(SW_SHOW);
		}
	}

	if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
		((CGxStaticEx*)m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_PLC))->SetCaption(_T("PLC"));
	else
		((CGxStaticEx*)m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_PLC))->SetCaption(_T("PC"));

	if(m_stParamInfo.bUseCIM)
		m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_CIM)->EnableWindow(TRUE);

	//2016.02.01 Packing UI
	//m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_QMS)->EnableWindow(TRUE);
	//m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_ALIGN)->EnableWindow(TRUE);
	if (m_stParamInfo.nEqpObject != P_PROJECT)
	{
		m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_QMS)->EnableWindow(TRUE);
		m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_QMS)->ShowWindow(SW_SHOW);

		m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_ALIGN)->EnableWindow(TRUE);
		m_dDlgTopBar.GetDlgItem(IDC_GXLBL_TOPBAR_STATE_ALIGN)->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::m_fnInitState()
{
	CView* pView = m_pCruxData->m_fnGetViewPtr(eFORM_Auto);
	//((CViewAuto*)pView)->m_pDlgVisionState->m_fnInitState();
	//((CViewAuto*)pView)->m_pDlgStageState->m_fnInitState();
	//2016.02.01 Packing UI
	if (m_stParamInfo.nEqpObject != P_PROJECT)
		((CViewAuto*)pView)->m_pDlgStageState->m_fnInitState();
	//2016.02.01 Packing UI
	((CViewAuto*)pView)->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
	//((CViewAuto*)pView)->m_pDlgPanelJudge->m_fnInitState();
	((CViewAuto*)pView)->m_pDlgDefInfo->m_fnInitDefectInfo();
	//((CViewAuto*)pView)->m_pDlgLuminanceState->m_fnGetLimitValue();

	//pView = m_pCruxData->m_fnGetViewPtr(eFORM_Manual);
	//((CViewManual*)pView)->m_pDlgVisionState->m_fnInitState();
	//((CViewManual*)pView)->m_pDlgStageState->m_fnInitState();
	if (m_hThreadInspAlive == NULL)
	{
		UINT uiThreadID;
		m_hThreadInspAlive = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadPcAliveChk, (LPVOID)this, 0, &uiThreadID);
	}	
}

void CMainFrame::m_fnInitSockMng()
{
	WSADATA wsaData;
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		G_AddLog(3, 2, _T("WSAStartup() Failure!!!"), WSAGetLastError());
		return ;
	}

	m_pSockManager = new CSockManager(this);
	CRect rc;
	m_pSockManager->Create(NULL, NULL, 0, rc, this, NULL);

	m_pSockManager->m_fnInitSock();
}

void CMainFrame::m_fnInitPLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	long lRet = pView->m_pDlgPLC->m_fnOpenPLC();
}

void CMainFrame::m_fnInitThreadSeqPLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	if(pView->m_pDlgPLC->m_bOpenPLC == FALSE)
	{
		G_AddLog(3, 2, _T("Not Open PLC!"));
		return;
	}
	//for(int i = 0; i < m_stParamInfo.nCountStage; i++)
	//	for(int i = 0; i < m_stParamInfo.nCountVisionClient; i++)
	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		m_pThreadSeqPLC[i] = NULL;
		m_pThreadSeqPLC[i] = new CThreadSeqPLC();
		m_pThreadSeqPLC[i]->m_fnInitThread(m_stParamInfo.nEqpType, i, pView->m_pDlgPLC, &m_DataManager, m_pSockManager, m_pVsSeverIF);

		long lRet = -1;
		
		//Cheng Du Comment
		//if(i == 0)			
		//	pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S1_ALIVE, 1);
		//else if(i == 1)		
		//	pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S2_ALIVE, 1);
		//else if(i == 2)		
		//	pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S3_ALIVE, 1);
	}

	//2017.07.17 add by yskim
	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_S1_REQ, 0);	//Chung Du Comment
	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_S2_REQ, 0);	//Chung Du Comment
	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_S3_REQ, 0);	//Chung Du Comment
	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_R_NG_FLAG_COMPLETE, 0);	//Chung Du Comment
	pView->m_pDlgPLC->m_fnSetDevice(ADDR_L_NG_FLAG_COMPLETE, 0);
	//2017.07.17 add by yskim
}

void CMainFrame::m_fnDeInitThreadSeqPLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	//for(int i = 0; i < m_stParamInfo.nCountVisionClient; i++)
	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		if(m_pThreadSeqPLC[i])
			m_pThreadSeqPLC[i]->m_fnDeInitThread();
	}
}

void CMainFrame::m_fnInitThreadStatePLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	if(pView->m_pDlgPLC->m_bOpenPLC == FALSE)
	{
		G_AddLog(3, 2, _T("Not Open PLC!"));
		return;
	}

	m_ThreadStatePLC.m_fnInitThread(m_stParamInfo.nEqpSize, m_stParamInfo.nEqpType, pView->m_pDlgPLC);
}

void CMainFrame::m_fnDeInitThreadStatePLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	m_ThreadStatePLC.m_fnDeInitThread();
}

void CMainFrame::m_fnInitThreadSeqCIM()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	if(pView->m_pDlgPLC->m_bOpenPLC == FALSE)
	{
		G_AddLog(3, 2, _T("Not Open PLC(CIM)!"));
		return;
	}

	for(int i = 0; i < CIM_MAX; i++)
	{
		m_pThreadSeqCIM[i] = new CThreadSeqCIM();
		m_pThreadSeqCIM[i]->m_fnInitThread(m_stParamInfo.nEqpType, (i+1), pView->m_pDlgPLC, &m_DataManager, m_pSockManager);
	}
}

void CMainFrame::m_fnDeInitThreadSeqCIM()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	int nSeqCnt = 0;
	for(int i = 0; i < CIM_MAX; i++)
	{
		if(m_pThreadSeqCIM[i])
			m_pThreadSeqCIM[i]->m_fnDeInitThread();
	}
}

void CMainFrame::m_fnInitThreadProcPLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	if(pView->m_pDlgPLC->m_bOpenPLC == FALSE)
	{
		G_AddLog(3, 2, _T("Not Open PLC(CIM)!"));
		return;
	}

	for(int i = 0; i < (SEQ_PROC_PLC_MAX ); i++)
	{
		m_pThreadProcPLC[i] = new CThreadProcPLC();
		m_pThreadProcPLC[i]->m_fnInitThread(i, pView->m_pDlgPLC, &m_DataManager);
	}	
}

//2015.08.19
void CMainFrame::m_fnInitThreadProcAlign()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	if(pView->m_pDlgPLC->m_bOpenPLC == FALSE)
	{
		G_AddLog(3, 2, _T("Not Open PLC(CIM)!"));
		return;
	}

	m_pThreadProcAlign = new CThreadProcAlign();
	m_pThreadProcAlign->m_fnInitThread( pView->m_pDlgPLC, &m_DataManager, m_pVsSeverIF);
}

void CMainFrame::m_fnDeInitThreadProcPLC()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	int nSeqCnt = 0;
	if(m_pThreadProcPLC)
	{
		for(int i = 0; i < SEQ_PROC_PLC_MAX ; i++)
		{
			m_pThreadProcPLC[i]->m_fnDeInitThread();
		}
	}

}

//2015.08.19
void CMainFrame::m_fnDeInitThreadProcAlign()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	int nSeqCnt = 0;
	if(m_pThreadProcAlign)
		m_pThreadProcAlign->m_fnDeInitThread();
}

void CMainFrame::m_fnInitINIFile()
{
	CString strTemp;
	CString strRecipePath;	//2015.11.05

	strTemp.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
	//2015.11.05
	strRecipePath.Format(_T("%s%s"), ROOT_PATH, RECIPE_INFO_PATH);
	m_iniRecipe.SetFileName(strRecipePath);
	//2015.11.05
	m_iniConfigSet.SetFileName(strTemp);
	//2015.02.03 add by ikm for FTP - S	
	m_iniConfigFtp.SetFileName(strTemp);
	m_fnSetFtpIniFile();
	//2015.02.03 add by ikm for FTP - E

	//2016.11.21 Mr.Chu
	strTemp.Format(_T("%s%s"), ROOT_PATH, OVERLAP_COORD_DATA_PATH);
	m_iniOverlapCurCoord.SetFileName(strTemp);
	
	m_fnSetStageInfo();	//2015.05.13 add
	m_fnSetOverlapCoordInfo();	//2015.06.02 add
	m_fnSetTdiOverlapCoordInfo();	//2015.06.11 add

	m_fnLoadJudgeCount();	//2015.07.20

	m_fnSetOverlapLuminanceInfo();	//2016.03.02
	strTemp.Format(_T("%s%s"), ROOT_PATH, PLC_MAP_PATH);
	m_iniConfigPLCMap.SetFileName(strTemp);
	m_LogMng.m_nCurrentLogPriority = m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_LOG_PRIORITY, 0);
	G_AddLog(3, 2, _T("MainFrame m_fnInitINIFile() - %s"), strTemp);
}

void CMainFrame::m_fnInitLogMng()
{
	CString strTemp;

	strTemp.Format(_T("%s%s"), ROOT_PATH, LOG_NAME_CRUXSERVER);

	m_LogMng.m_fnSetInitInfo(strTemp, _T("CruxServer"));
	//2016.04.05 add by ikm - 온도값 전용 로그
	m_TemperatureLog.m_fnSetInitInfo(strTemp, _T("Temperature"));
	m_AlarmLog.m_fnSetInitInfo(_T("D:\\CruxServer\\LOG\\Server_Alarm"), _T("CruxServer_Alarm"));
	m_ResetTimeLog.m_fnSetInitInfo(_T("D:\\CruxServer\\LOG\\ResetTime"), _T("CruxServer_ResetTime"));
	//m_stParamInfo.nLogDelDay = 30; //2015.07.09 chu

	if(m_stParamInfo.bUseAutoDel)
		m_LogMng.m_fnSetAutoDeleteInfo(strTemp, m_stParamInfo.nLogDelDay);

	G_AddLog(3, 2, _T("MainFrame m_fnInitLogMng() - %s"), strTemp);
}

void CMainFrame::m_fnUpdateSockStatus()
{
	CView* pView = GetActiveView();

	CCruxData *pCruxData = CCruxData::m_fnGetInstance();

	if (pCruxData->m_fnGetCurrentViewCaption() == "Setup")
		((CViewSetup*)pView)->m_fnUpdateStatus();
}

void CMainFrame::m_fnUpdateTopBarStatus()
{
	if(m_pVsSeverIF == NULL)	return;

	for(int i = 0; i < m_stParamInfo.nCountVisionClient; i++)
	{
		//m_pVsSeverIF->m_stSockStatus.bVisionClient[i] = m_pVsSeverIF->Seq_InspAliveChk(i + 1);
		m_dDlgTopBar.m_fnSetState(i + 1, m_pVsSeverIF->m_stSockStatus.bVisionClient[i]);
		//m_dDlgTopBar.m_fnSetState(i + 1, m_pVsSeverIF->m_stSockStatus.bVisionClient[i]);
	}		

	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	m_pVsSeverIF->m_stSockStatus.bPLC = pView->m_pDlgPLC->m_bOpenPLC;

	if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
		m_dDlgTopBar.m_fnSetState(STATUS_TOPBAR_PLC, m_pVsSeverIF->m_stSockStatus.bPLC);
	else
		m_dDlgTopBar.m_fnSetState(STATUS_TOPBAR_PLC, m_pVsSeverIF->m_stSockStatus.bControlPC);

	//2015.11.20
	m_dDlgTopBar.m_fnSetState(STATUS_TOPBAR_QMS, m_bQmsConnection);
	//2015.12.18 Mr.Chu
	m_dDlgTopBar.m_fnSetState(STATUS_TOPBAR_ALIGN, m_pVsSeverIF->m_stSockStatus.bPreAlign);

	m_fnReportPCAlive();	//2015.10.31 Mr.Chu
}

// 종료시 처리
void CMainFrame::m_fnShutdownProcess(void)
{
	KillTimer( eTIMER_Shutdown );						// StartupTimer를 중지시킨다.

	switch (m_nShutdownSeq )
	{
		case 0:
			m_bLockFormChange		= TRUE;				// 화면 변경 금지 설정
			m_dDlgBottomBar.MenuLock(TRUE);				// 메뉴버튼을 비활성화 시킨다.

			m_ThreadStatePLC.m_bReadPLC = FALSE;			// PLC Map Read 를 멈춘다.

			m_nShutdownSeq = 10;
			break;
		case 10:
			if(m_stParamInfo.nControlMode == CONTROL_MODE_PLC)
			{
				m_fnDeInitThreadSeqPLC();
				m_fnDeInitThreadStatePLC();
				m_fnDeInitThreadSeqCIM();
				m_fnDeInitThreadProcPLC();
				m_fnDeInitThreadProcAlign();	//2015.08.19
			}

			m_nShutdownSeq = 900;
			break;
		case 900:
			//KillTimer(TIMER_CHECK_SUMMARY);
			KillTimer(TIMER_CHECK_STATUS);
			KillTimer( eTIMER_Shutdown );					// StartupTimer를 중지시킨다.

			m_bTerminateFlag = TRUE;														// 이제 종료해도 좋다고 승인
			::PostMessage( this->m_hWnd, WM_CLOSE, (WPARAM)0, (LPARAM)0);		// 종료 메세지를 보낸다.
			return;
	}

	SetTimer(eTIMER_Shutdown, eINTERVAL_Shutdown, NULL);		// Timer 재기동
}

CString CMainFrame::m_fnGetPanelID_1(int nStageID)
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	CString strPanelID = pView->m_pDlgPLC->m_stPlcMapPLC[nStageID].strPanelID_1;

	return strPanelID;
}

CString CMainFrame::m_fnGetPanelID_2(int nStageID)
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	CString strPanelID = pView->m_pDlgPLC->m_stPlcMapPLC[nStageID].strPanelID_2;

	return strPanelID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 기본적인 변수들의 값을 초기화한다.
	m_bOnlyOnce				= TRUE;				// 최초 1회 수행용
	m_bLockFormChange		= TRUE;				// 화면 변경 금지 설정

	m_pCruxData		= CCruxData::m_fnGetInstance();		// GUI Data를 관리하는 Data Class 객체 사용
	
	m_fnCreateDialogBar();		// 각종 DialogBar를 생성하여준다.

	return 0;
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	if ( m_bOnlyOnce ) 
	{
		m_bOnlyOnce = FALSE;

		SetTimer(TIMER_CHECK_STATUS, TIMER_CHECK_STATUS_INTERVAL, NULL);	// TopBar Status Timer 가동
		//SetTimer(TIMER_CHECK_SUMMARY, TIMER_CHECK_SUMMARY_INTERVAL, NULL);

		m_fnCreateSubForm();		// 각종 Form 생성
		m_fnInitEnv();

		//SetTimer(TIMER_CHECK_STATUS, TIMER_CHECK_STATUS_INTERVAL, NULL);	// TopBar Status Timer 가동	//2015.05.12 move
		//SetTimer(TIMER_CHECK_SUMMARY, TIMER_CHECK_SUMMARY_INTERVAL, NULL);
	}
}

void CMainFrame::OnClose()
{
	if ( !m_bTerminateFlag ) {
		int nRet = AfxMessageBox(_T("Are you sure you want to exit ?"), MB_OKCANCEL);
		if ( nRet != IDOK ) return;		// 종료 취소
		G_AddLog(3, 2, _T("Program End"));
		SetTimer(eTIMER_Shutdown, eINTERVAL_Shutdown, NULL);				// 종료처리 Timer 구동
		return;
	}

	CFrameWnd::OnClose();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) 
	{
		case TIMER_CHECK_SUMMARY:
			//m_DataManager.m_fnBackupSummary();	// 하루에 한번확인. Summary 파일 백업 주기.
			break;
		case TIMER_CHECK_STATUS:
			m_fnUpdateTopBarStatus();
			break;
		case eTIMER_Shutdown:
			m_fnShutdownProcess();
			break;
	}

	CFrameWnd::OnTimer(nIDEvent);
}


//
// Message를 통해 화면 변경 요청이 들어왔다.
//
// wParam = MainMenu Select Button 
// lParam = SubMenu Select Button, 무시
//
LRESULT CMainFrame::OnMainMenuChange(WPARAM wParam, LPARAM lParam)
{
	m_fnChangeForm( (UINT)wParam, FALSE );

	return 0;
}


void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_pCruxData)
	{
		m_pCruxData->m_fnReleaseInstance();
		m_pCruxData = NULL;
	}
}

///2015.02.03 add by ikm for FTP - S	
BOOL CMainFrame::m_fnSetFtpIniFile()
{	
	int ipCnt =0;
	//int iDirDelDay=0;
	int iPeriodDay, iLogWriteTerm = 0;	
	CString strIniIpNo;
	TSTRING  strLogPath = _T(""), strLogName = _T(""), strImgLogPath = _T("");
	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());		//(CMainFrame*)AfxGetApp()->GetMainWnd();		
	wchar_t chRead[255] = {0};	
	
	m_stCurFtpCond.iIP_Cnt = m_iniConfigFtp.GetProfileIntW(FTP_CONDITION, FTP_IP_COUNT, 3);  
	
	m_stFtpInfo.stInfo = new STRU_FTP_SEVER_INFO[m_stCurFtpCond.iIP_Cnt];
	m_stFtpInfo.stPath = new STRU_FTP_PATH;


	while (ipCnt < m_stCurFtpCond.iIP_Cnt)
	{		

		ipCnt++;		
		memset(chRead,0X00,sizeof(chRead));		
		sprintf((char *)chRead, "%d",ipCnt);		
		strIniIpNo = chRead;
		
		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_SERVER, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strServer = chRead;
		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_ID, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strID  = chRead;
		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_PW, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strPW  = chRead;

		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_IMG_ID, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strImgID  = chRead;

		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_IMG_PW, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strImgPW  = chRead;


		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigFtp.GetProfileStringW(SECTION_FTP + strIniIpNo, KEY_FTP_ROOT, _T(""), (LPTSTR)chRead, 255);
		m_stFtpInfo.stInfo[ipCnt - 1].strRoot  = chRead;
		memset(chRead,0X00,sizeof(chRead));
		m_stFtpInfo.stInfo[ipCnt - 1].nFTP_Port = m_iniConfigFtp.GetProfileIntW(SECTION_FTP + strIniIpNo, KEY_FTP_PORT, 0);		
			
	}	

	ipCnt = 0;

	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_DEFAULT, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Default  = chRead;
	strLogPath= chRead;
	strImgLogPath= chRead;
	memset(chRead,0X00,sizeof(chRead));		
	m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_LOG, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Log = chRead;
	strLogPath +=chRead;
	memset(chRead,0X00,sizeof(chRead));		
	m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_IMG_LOG, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_iLog = chRead;
	strImgLogPath +=chRead;
	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_SENT, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Send  = chRead;
	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_UNSENT, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Wait = chRead;
	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_ERROR, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Error = chRead;	
	memset(chRead,0X00,sizeof(chRead));		
	m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_LOG_NAME, _T(""), (LPTSTR)chRead, 255);
	strLogName = chRead;	
	memset(chRead,0X00,sizeof(chRead));		
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_IMG_SENT, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_iSend = chRead;
	memset(chRead,0X00,sizeof(chRead));		
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_IMG_UNSENT, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_iWait = chRead;		
	//2018.12.28 add by mby
	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_TEMP, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_Temp  = chRead;
	memset(chRead,0X00,sizeof(chRead));
	m_iniConfigFtp.GetProfileStringW(FTP_LOCAL_STATE, KEY_FTP_DIR_IMG_TEMP, _T(""), (LPTSTR)chRead, 255);
	m_stFtpInfo.stPath->strDir_iTemp  = chRead;

	iLogWriteTerm = m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, LOG_WRITE_TERM, 0);	
	
	iPeriodDay = m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, KEY_FTP_DIR_PERIOD_DAY, 30);

	m_stFtpInfo.stInfo->iDirDelDay = m_iniConfigFtp.GetProfileIntW(FTP_LOCAL_STATE, KEY_FTP_DIR_DELETE, 10);  

	m_stCurFtpCond.iAns_Rename_Chk_Timeout = m_iniConfigFtp.GetProfileIntW(FTP_CONDITION, ANS_RENAME_CHECK_TIMEOUT, 3);

	m_stCurFtpCond.iAns_Rename_Wait_cnt = m_iniConfigFtp.GetProfileIntW(FTP_CONDITION, ANS_RENAME_WAIT_CNT_MAX, 6);  
		
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_Send);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_Wait);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_Error);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_Log);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_iWait);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_iSend);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_iLog);
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_Temp);	//2018.05.14 -> 2018.12.28 move
	m_fnCreateFolder(m_stFtpInfo.stPath->strDir_Default + m_stFtpInfo.stPath->strDir_iTemp);
	
	//m_FtpLogMng.m_fnSetAutoDeleteInfo(m_stFtpInfo.stPath->strDir_Default , iDirDelDay);
	m_bQmsConnection  = TRUE;
	//m_FtpClient.m_fnStartFTPConnect();	//2015.11.20 Comment Out
	return TRUE;
}

BOOL CMainFrame::m_fnCreateFolder(CString szPath)
{
	BOOL bRet = FALSE;
	CString csPrefix(_T("")), csToken(_T(""));
	int nStart = 0, nEnd;
	while( (nEnd = szPath.Find('\\', nStart)) >= 0)
	{
		CString csToken = szPath.Mid(nStart, nEnd-nStart);
		
		if (!m_fncheckPathExists(csPrefix + csToken))
		{
			bRet = CreateDirectory(csPrefix + csToken, NULL);
		}	
	
		csPrefix += csToken;
		csPrefix += _T("\\");
		nStart = nEnd+1;
	} 
	csToken = szPath.Mid(nStart);
	bRet = CreateDirectory(csPrefix + csToken, NULL);

	return bRet;
}

BOOL CMainFrame::m_fncheckPathExists(LPCTSTR szPath)
{ 
	return (GetFileAttributes(szPath) != INVALID_FILE_ATTRIBUTES);
}


//LRESULT CMainFrame::OnSelectEngineerMode(WPARAM wParam, LPARAM lParam)	// 2015.07.09 ljk
//{
//	int nStage = (int)wParam;
//	
//	if(m_strJudge_1.IsEmpty() || m_strJudge_1.CompareNoCase(_T("NG")) == 0)
//	{
//		UpdateData(TRUE);
//		// 모드 선택 창 발생
//		CSelectEngineerModeDlg dlg;
//		if(dlg.DoModal() == IDOK)
//		{
//			m_nSelectEngineerMode[nStage] =	dlg.m_nSelectEngineerMode;
//		}
//	}
//	else
//	{
//		m_nSelectEngineerMode[nStage] = 0;	// Eng'r Mode(default)
//	}
//
//	m_bIsEngineerModeMsg[nStage] = TRUE;
//	return 0;
//}

//2015.05.13 add
BOOL CMainFrame::m_fnSetStageInfo()
{ 
	int nStageCnt, nCnt = 0;
	CString strIniIpNo, strVisionPC, strVal;		
	wchar_t chRead[255] = {0};
	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());		//(CMainFrame*)AfxGetApp()->GetMainWnd();		
	int nCurPos = 0;
	int nVisionCnt = 0;

	nStageCnt = m_iniConfigSet.GetProfileIntW(_T("PARAM"), _T("STAGE COUNT"), 0);  

	m_stStageInfo = new STRU_PLC_STAGEINFO[nStageCnt];
	memset(m_stStageInfo,0X00,sizeof(m_stStageInfo));

	while (nCnt < nStageCnt)
	{	
		nCnt++;

		m_stStageInfo[nCnt - 1].nStageNo = nCnt;

		memset(chRead,0X00,sizeof(chRead));		
		sprintf((char *)chRead, "%d",nCnt);		
		strIniIpNo = chRead;
		
		memset(chRead,0X00,sizeof(chRead));
		m_iniConfigSet.GetProfileStringW(_T("STAGE_") + strIniIpNo, _T("Vision PC Num"), _T(""), (LPTSTR)chRead, 255);
		strVisionPC  = chRead;

		CString resToken= strVisionPC.Tokenize(EXT_COLON, nCurPos);
		while (resToken != "")
		{
			nVisionCnt++;
			m_stStageInfo[nCnt - 1].nVisionNo[nVisionCnt - 1] = _ttoi(resToken);
			resToken= strVisionPC.Tokenize(EXT_COLON, nCurPos);
		}
		m_stStageInfo[nCnt - 1].nVisionCnt = nVisionCnt;
		nCurPos = 0;
		nVisionCnt = 0;
	}

	return TRUE;
}

//2015.06.02 add
BOOL CMainFrame::m_fnSetOverlapCoordInfo()
{ 
	DOUBLE dOffsetX, dOffsetY;
	TCHAR strBuffer[255];
	CString strLine, strDefectCheck;
	int nCurPos = 0;
	int nDefectCnt = 0;

	if(m_stOverlapCoordInfo != NULL) delete m_stOverlapCoordInfo; m_stOverlapCoordInfo = NULL;
	m_stOverlapCoordInfo = new STRU_OVERLAP_COORD_INFO;

	m_stOverlapCoordInfo->bUseMode = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_USEMODE, 0);

	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_OFFSET_X, _T(""), strBuffer, 255);		
	strLine = strBuffer;
	if(!strLine.IsEmpty())
	{
		_stscanf(strLine, _T("%lf"), &dOffsetX);
	}	
	m_stOverlapCoordInfo->dOffsetX = dOffsetX;

	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_OFFSET_Y, _T(""), strBuffer, 255);
	strLine = strBuffer;
	if(!strLine.IsEmpty())
	{
		_stscanf(strLine, _T("%lf"), &dOffsetY);
	}
	m_stOverlapCoordInfo->dOffsetY = dOffsetY;

	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_DEFECT_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	CString resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->nDefectCheck[nDefectCnt - 1] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}

	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	CString resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->nDefectCount[nDefectCnt - 1] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}

	m_stOverlapCoordInfo->bUseContactFail = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CHECK, 0);
	m_stOverlapCoordInfo->nContactFailDefectCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_SETTING_COUNT, 0);

	//2016.02.24
	m_stOverlapCoordInfo->bUsePolMiss = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CHECK, 0);
	m_stOverlapCoordInfo->nPolMissDefectCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_SETTING_COUNT, 0);

	//2016.08.22
	m_stOverlapCoordInfo->bUseAlignFail = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CHECK, 0);
	m_stOverlapCoordInfo->nAlignFaliDefectCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_SETTING_COUNT, 0);

	//2019.03.21
	m_stOverlapCoordInfo->bUseBgradecnt = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_B_GRADE_CHECK, 0);
	m_stOverlapCoordInfo->nBgradecntDefectCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_B_GRADE_SETTING_COUNT, 0);
	m_stOverlapCoordInfo->bUseCgradecnt = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_C_GRADE_CHECK, 0);
	m_stOverlapCoordInfo->nCgradecntDefectCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_C_GRADE_SETTING_COUNT, 0);

	//2016.11.21
	//KEY_OVERLAP_LUMINANCE_CHECK
	resTokenKind = _T("");
	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->bArrLuminanceChk[nDefectCnt - 1] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}

	//KEY_OVERLAP_LUMINANCE_COUNT
	resTokenCount = _T("");
	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_SETTING_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->nArrLuminanceCnt[nDefectCnt - 1] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}
	//2020.02.28 add by mby
	//         ********* KEY_OVERLAP_BUBBLESTATION_CHECK
	//resTokenKind = _T("");
	resTokenKind = _T("");	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_BUBBLESTATION_CHECK, _T(""), strBuffer, 255);  //255
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->bArrBubbleStationChk[nDefectCnt - 1] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}
	////         ********* KEY_OVERLAP_BUBBLESTATION_COUNT
	resTokenCount = _T("");
	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_BUBBLESTATION_SETTING_COUNT, _T(""), strBuffer, 255);  //255
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{
		nDefectCnt++;
		m_stOverlapCoordInfo->nArrBubbleStationCnt[nDefectCnt - 1] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}
	//}

	//2015.09.16 Mr.Chu
	m_stOverlapCoordInfo->nPanelCnt = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_PANEL_COUNT, 0);

	 //2016.03.23 add by ikm
	m_stOverlapCoordInfo->bAllOverlapDef =  m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALL_OVERLAP_DEF_MODE, 0);
	return TRUE;
}

//2015.06.11 add
BOOL CMainFrame::m_fnSetTdiOverlapCoordInfo()
{ 
	DOUBLE dOffsetX, dOffsetY;
	TCHAR strBuffer[255];
	CString strLine;

	m_stTdiOverlapCoordInfo = new STRU_TDI_OVERLAP_COORD_INFO;

	m_iniConfigSet.GetProfileStringW(SECTION_TDI_OVERLAP, KEY_OVERLAP_OFFSET_X, _T(""), strBuffer, 255);		
	strLine = strBuffer;
	if(!strLine.IsEmpty())
	{
		_stscanf(strLine, _T("%lf"), &dOffsetX);
	}	
	m_stTdiOverlapCoordInfo->dOffsetX = dOffsetX;

	m_iniConfigSet.GetProfileStringW(SECTION_TDI_OVERLAP, KEY_OVERLAP_OFFSET_Y, _T(""), strBuffer, 255);
	strLine = strBuffer;
	if(!strLine.IsEmpty())
	{
		_stscanf(strLine, _T("%lf"), &dOffsetY);
	}
	m_stTdiOverlapCoordInfo->dOffsetY = dOffsetY;

	return TRUE;
}

//2015.07.20
void CMainFrame::m_fnLoadJudgeCount()
{


	CString strJudgeCountPath;
	strJudgeCountPath.Format(_T("%s%s"), ROOT_PATH,CONFIG_COUNT_PATH );

	if (m_stJudgeCount != NULL)	delete m_stJudgeCount; 	m_stJudgeCount = NULL; 
	m_stJudgeCount = new STRU_JUDGE_COUNT;

	//Count
	m_stJudgeCount->bUseMode =								GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_DUST] =				GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_WHITE_TURBIDITY] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_ITO] =					GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_EDGE_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_PAD_SCRATCH] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_DIMPLE_POINT] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_ETCHING_LINE] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_DIMPLE_LINE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_PAD_BURNT] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_BURR] =				GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_POL_BUBBLE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_POL_SCRATCH] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_POL_ALIEN_SUBSTANCE] = GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	//m_stJudgeCount->nJudgeCount[INSP_POL_MINOR]=			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("POL_MINOR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_POL_ALIEN_SUBSTANCE+1]=GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_BLACK_SCRATCH] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_WHITE_SCRATCH] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_MEASURE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_TFT_MEASURE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_MURA] =				GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_NO_USE] =				GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_BURR] =				GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_TFT_BURR] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_TFT_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_CORNER_BROKEN] =	GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_TFT_CORNER_BROKEN] =	GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_CELL_BROKEN] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_TFT_CELL_BROKEN] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeCount[INSP_PIMPLE_POINT] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_PIMPLE_LINE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_PIMPLE_CIRCLE] =	GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_CF_WAVE_PATTERN] =		GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_ETC_LINE] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeCount[INSP_ETC_POINT] =			GetPrivateProfileInt(DEF_JUDGE_COUNT, _T("ETC_POINT"), 0, strJudgeCountPath);

	//Luminance
	m_stJudgeCount->nJudgeLuminance[INSP_DUST] =				GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_WHITE_TURBIDITY] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_ITO] =					GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_EDGE_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_PAD_SCRATCH] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_DIMPLE_POINT] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_ETCHING_LINE] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_DIMPLE_LINE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_PAD_BURNT] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_BURR] =				GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_POL_BUBBLE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_POL_SCRATCH] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_POL_ALIEN_SUBSTANCE] = GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_POL_ALIEN_SUBSTANCE+1]=GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_BLACK_SCRATCH] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_WHITE_SCRATCH] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_MEASURE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_TFT_MEASURE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_MURA] =				GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_NO_USE] =				GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_BURR] =				GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_TFT_BURR] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_TFT_BROKEN] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_CORNER_BROKEN] =	GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_TFT_CORNER_BROKEN] =	GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_CELL_BROKEN] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_TFT_CELL_BROKEN] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeLuminance[INSP_PIMPLE_POINT] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_PIMPLE_LINE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_PIMPLE_CIRCLE] =	GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_CF_WAVE_PATTERN] =		GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_ETC_LINE] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLuminance[INSP_ETC_POINT] =			GetPrivateProfileInt(DEF_JUDGE_LUMINANCE, _T("ETC_POINT"), 0, strJudgeCountPath);

	//Distance_D1
	m_stJudgeCount->nJudgeDistanceD1[INSP_DUST] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_WHITE_TURBIDITY] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_ITO] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_EDGE_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_PAD_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_DIMPLE_POINT] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_ETCHING_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_DIMPLE_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_PAD_BURNT] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_POL_BUBBLE] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_POL_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_POL_ALIEN_SUBSTANCE] =m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_POL_ALIEN_SUBSTANCE+1]=m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_BLACK_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_WHITE_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_MEASURE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_MURA] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_NO_USE] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_BURR] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_BURR] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_CORNER_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_CORNER_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_CELL_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeDistanceD1[INSP_PIMPLE_POINT] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_PIMPLE_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_PIMPLE_CIRCLE] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_CF_WAVE_PATTERN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_ETC_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD1[INSP_ETC_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D1, _T("ETC_POINT"), 0, strJudgeCountPath);

	//Distance_D2
	m_stJudgeCount->nJudgeDistanceD2[INSP_DUST] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_WHITE_TURBIDITY] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_ITO] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_EDGE_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_PAD_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_DIMPLE_POINT] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_ETCHING_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_DIMPLE_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_PAD_BURNT] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_POL_BUBBLE] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_POL_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_POL_ALIEN_SUBSTANCE] =m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_POL_ALIEN_SUBSTANCE+1]=m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_BLACK_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_WHITE_SCRATCH] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_MEASURE] =			GetPrivateProfileInt(DEF_JUDGE_DISTANCE_D2, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_MEASURE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_MURA] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_NO_USE] =				m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BURR] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BURR] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_CORNER_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_CORNER_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_CELL_BROKEN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeDistanceD2[INSP_PIMPLE_POINT] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_PIMPLE_LINE] =		m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_PIMPLE_CIRCLE] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_CF_WAVE_PATTERN] =	m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_ETC_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeDistanceD2[INSP_ETC_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_DISTANCE_D2, _T("ETC_POINT"), 0, strJudgeCountPath);

	//LENGTH
	m_stJudgeCount->nJudgeLength[INSP_DUST] =					m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_WHITE_TURBIDITY] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_ITO] =					m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_EDGE_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_PAD_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_DIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_ETCHING_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_DIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_PAD_BURNT] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_BURR] =					m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_POL_BUBBLE] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_POL_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_POL_ALIEN_SUBSTANCE] =	m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_POL_ALIEN_SUBSTANCE+1]=	m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_BLACK_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_WHITE_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_MEASURE] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_TFT_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_MURA] =					m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_NO_USE] =					m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_TFT_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_BROKEN] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_TFT_BROKEN] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_TFT_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_CELL_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_TFT_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeLength[INSP_PIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_PIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_PIMPLE_CIRCLE] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_CF_WAVE_PATTERN] =		m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_ETC_LINE] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeLength[INSP_ETC_POINT] =				m_fnGetProfileDouble(DEF_JUDGE_LENGTH, _T("ETC_POINT"), 0, strJudgeCountPath);

	//WIDTH_W1
	m_stJudgeCount->nJudgeThinknessW1[INSP_DUST] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_WHITE_TURBIDITY] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_ITO] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_EDGE_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_PAD_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_DIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_ETCHING_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_DIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_PAD_BURNT] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_BURR] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_POL_BUBBLE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_POL_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_POL_ALIEN_SUBSTANCE] =	m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_POL_ALIEN_SUBSTANCE+1]=	m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_BLACK_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_WHITE_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_TFT_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_MURA] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_NO_USE] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_TFT_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_BROKEN] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_TFT_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_TFT_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_TFT_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeThinknessW1[INSP_PIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_PIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_PIMPLE_CIRCLE] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_CF_WAVE_PATTERN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_ETC_LINE] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW1[INSP_ETC_POINT] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W1, _T("ETC_POINT"), 0, strJudgeCountPath);

	//WIDTH_W2
	m_stJudgeCount->nJudgeThinknessW2[INSP_DUST] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("DUST"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_WHITE_TURBIDITY] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("WHITE_TURBIDITY"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_ITO] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("ITO"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_EDGE_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("EDGE_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_PAD_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("PAD_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_DIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("DIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_ETCHING_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("ETCHING_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_DIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("DIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_PAD_BURNT] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("PAD_BURNT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_BURR] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_POL_BUBBLE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("POL_BUBBLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_POL_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("POL_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_POL_ALIEN_SUBSTANCE] =	m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_POL_ALIEN_SUBSTANCE+1]=	m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_POL_SUBSTANCE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_BLACK_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("BLACK_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_WHITE_SCRATCH] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("WHITE_SCRATCH"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_TFT_MEASURE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_MEASURE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_MURA] =					m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("MURA"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_NO_USE] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("NO_USE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_TFT_BURR] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_BURR"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_BROKEN] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_TFT_BROKEN] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_TFT_CORNER_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_CORNER_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("CF_CELL_BROKEN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_TFT_CELL_BROKEN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("TFT_CELL_BROKEN"), 0, strJudgeCountPath);

	m_stJudgeCount->nJudgeThinknessW2[INSP_PIMPLE_POINT] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_POINT"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_PIMPLE_LINE] =			m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_PIMPLE_CIRCLE] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("PIMPLE_CIRCLE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_CF_WAVE_PATTERN] =		m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("WAVE_PATTERN"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_ETC_LINE] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("ETC_LINE"), 0, strJudgeCountPath);
	m_stJudgeCount->nJudgeThinknessW2[INSP_ETC_POINT] =				m_fnGetProfileDouble(DEF_JUDGE_WIDTH_W2, _T("ETC_POINT"), 0, strJudgeCountPath);
}

double CMainFrame::m_fnGetProfileDouble(CString strJudgeKind1, CString strJudgeKind2, CString strDefault, CString strFilePath )
{
	TCHAR strBuffer[255];
	CString strLine;
	DOUBLE dTempVal;

	GetPrivateProfileString(strJudgeKind1, strJudgeKind2, strDefault, (LPTSTR)strBuffer, 255, strFilePath);
	strLine = strBuffer;

	if(!strLine.IsEmpty())	
		_stscanf(strLine, _T("%lf"), &dTempVal);
	
	return dTempVal;
}

//2015.10.31 Mr.Chu
//기능: 상단 Client Alive 상태 표시등 (Active Control)을 보고 PLC에 각 PC Alive 상태 보고
void CMainFrame::m_fnReportPCAlive()
{
	long lRet = -1;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	static bool bComparePCStatus[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
	static bool bcompareAlignPCStatus = -1; 

	//2016.12.19 모니터 전용 분기 필요
	//Server PC Alive Condition  2016.09.23 Mr.Chu  PLC에서 Server Alive 상태 확인용. 모니터에서 사용.. 항시 True 상태로 켜준다.
	if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE ) 
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_SERVER_ALIVE,  TRUE);
	else if( pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )	//2018.01.24
	{
		lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PC_ALIVE);
		if(lRet != TRUE)
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_ALIVE,  TRUE);
	}

	//lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_ALIGN_LIVE,  0);	//2018.06.28

	//Inspection Client
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[0] != bComparePCStatus[0])
	{
		
		bComparePCStatus[0] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[0];
		//2020.07.29 add by mby	for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[0]){
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S1_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[0]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S1_GRAB_READY,  1);			
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[1] != bComparePCStatus[1])
	{
		bComparePCStatus[1] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[1];
		//2020.07.29 add by mby	for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[1]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S2_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[1]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S2_GRAB_READY,  1);		
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[2] != bComparePCStatus[2])
	{
		bComparePCStatus[2] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[2];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[2]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S3_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[2]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S3_GRAB_READY,  1);		
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[3] != bComparePCStatus[3])
	{
		bComparePCStatus[3] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[3];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[3]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S4_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[3]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S4_GRAB_READY,  1);			
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[4] != bComparePCStatus[4])
	{
		bComparePCStatus[4] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[4];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[4]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S5_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[4]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S5_GRAB_READY,  1);
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[5] != bComparePCStatus[5])
	{
		bComparePCStatus[5] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[5];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[5]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S6_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[5]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S6_GRAB_READY,  1);
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[6] != bComparePCStatus[6])
	{
		bComparePCStatus[6] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[6];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[6]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S7_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[6]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S7_GRAB_READY,  1);			
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[7] != bComparePCStatus[7])
	{
		bComparePCStatus[7] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[7];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[7]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S8_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[7]);
		}else{
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S8_GRAB_READY,  1);			
		}
	}
	if( m_pVsSeverIF->m_stSockStatus.bVisionClient[8] != bComparePCStatus[8])
	{
		bComparePCStatus[8] =  m_pVsSeverIF->m_stSockStatus.bVisionClient[8];
		//2020.07.29 add by mby for by pass of Insp PC
		if(pFrame->m_stParamInfo.bUseInspPc[8]){			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S9_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bVisionClient[8]);
		}else{			
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_S9_GRAB_READY,  1);
		}
	}
	//Align Client
	if( m_pVsSeverIF->m_stSockStatus.bPreAlign != bcompareAlignPCStatus )
	{
		bcompareAlignPCStatus =  m_pVsSeverIF->m_stSockStatus.bPreAlign;
		

		G_AddLog(2, 3, _T("m_fnReportPCAlive() : bPreAlign - %d, Retry..."),m_pVsSeverIF->m_stSockStatus.bPreAlign);
		m_pVsSeverIF->Seq_AlignProcChk();
		Sleep(300);

		G_AddLog(2, 3, _T("m_fnReportPCAlive() : bPreAlign - %d"),m_pVsSeverIF->m_stSockStatus.bPreAlign);
		//2020.10.15 add by mby	for by pass of Align PC
		if(pFrame->m_stParamInfo.bUseAlignPc){
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_ALIGN_GRAB_READY,  m_pVsSeverIF->m_stSockStatus.bPreAlign);
		}else{
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PC_ALIGN_GRAB_READY,  1);
		}
	}
}

void CMainFrame::m_fnInitBinaryData(void)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FILE *in = NULL;
	size_t read_size = 0;
	errno_t errFileOpen;
	int nPcClassifyReadRetry = 0;
	CString strClsEndInfoPath = _T(""),strAlignMeasureDataPath,strNgFlagInfoPath;
	for (int nPanelCnt = 0; nPanelCnt < MAX_REV_PANEL; nPanelCnt++ )
	{
		for (int nStageCnt = 0 ; nStageCnt < MAX_PANEL ; nStageCnt++)
		{
			m_DataManager.m_stPcClassifyEndFlag[nPanelCnt][nStageCnt].init_info();
		}
	}

	strClsEndInfoPath.Format(_T("%s%s"), ROOT_PATH , CLASSIFY_INFO_PATH);

	while(nPcClassifyReadRetry < 3)
	{
		errFileOpen = _tfopen_s(&in, strClsEndInfoPath , _T("rb+")); // 구조체 바이너리 파일을 읽음

		if(errFileOpen == 0)
		{		
			read_size = fread(&m_DataManager.m_stPcClassifyEndFlag, sizeof(m_DataManager.m_stPcClassifyEndFlag), 1, in);			
			fclose(in);
			G_AddLog(2, 3, _T("ClassifyFile Read Success : m_fnClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
			break;
		}
		else
		{
			G_AddLog(2, 3, _T("ClassifyFile Read Error : m_fnClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
			//	fclose(in);
		}

		nPcClassifyReadRetry++;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strNgFlagInfoPath, _T("rb+"));

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_DataManager.m_stNgFlagInfo, sizeof(m_DataManager.m_stNgFlagInfo), 1, in);			
		fclose(in);
	}


	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strAlignMeasureDataPath, _T("rb+"));
	if(errFileOpen == 0)
	{		
		read_size = fread(&m_DataManager.m_stAlignMeasureInfo, sizeof(m_DataManager.m_stAlignMeasureInfo), 1, in);			
		fclose(in);
	}	


	//CString strDefTypePath;


	//strDefTypePath.Format(_T("%s%s"), ROOT_PATH , OVERLAP_DEF_TYPE_PATH);
	//while(nRetryFileOpenCnt < 3)
	//{
	//	errFileOpen = _tfopen_s(&in, strDefTypePath , _T("rb+")); // 구조체 바이너리 파일을 읽음

	//	if(errFileOpen == 0)
	//	{		
	//		read_size = fread(&DefInfoTmp, sizeof(DefInfoTmp), 1, in);			
	//		fclose(in);
	//		G_AddLog(2, 3, _T("Overlap DefType Read Success : DefInfoTmp"));	//2016.01.07 CHECK QMS COUNT LOG
	//		break;
	//	}
	//	else
	//	{
	//		G_AddLog(2, 3, _T("Overlap DefType Read Success : DefInfoTmp"));	//2016.01.07 CHECK QMS COUNT LOG
	//	}
	//	nRetryFileOpenCnt++;
	//}
}
//2016.01.29 Mr.Chu
// 기능 : VisionClient 폴더 유무 감시
unsigned __stdcall  CMainFrame::m_fnDetectVisionClient(LPVOID pArg )
{	
	CString strVisionClientPath,strRootPath;
	int nEqpModel,nVisionCount = 1, nEqpSizeType;
	int nCompareVisCnt = 0;
	CMainFrame *pFrame = (CMainFrame *)pArg;
	CViewSetup* pView = (CViewSetup*)pFrame->m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	int * nNoExistDirCnt;
	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	nEqpSizeType = pFrame->m_fnEqpSizeType();

	if (nEqpModel == EQP_TYPE_AVL) nCompareVisCnt = VISION_PC_2;
	if (nEqpModel == EQP_TYPE_ABI) nCompareVisCnt = VISION_PC_3;
	
	nNoExistDirCnt = new int[nCompareVisCnt];
	while(TRUE)
	{
		if(nVisionCount <= nCompareVisCnt)
		{
			strVisionClientPath.Format(_T("%sVisionClient-%d"), strRootPath, nVisionCount);
			if( !pFrame->m_fncheckPathExists(strVisionClientPath) )
			{	
				*nNoExistDirCnt = nVisionCount;
				pFrame->m_DataManager.m_fnDisplayMSGSet(_T("[Warning!] Inspection saving data folder can't find"),strVisionClientPath);
				if(nEqpSizeType == EQP_SMALL_SIZE || nEqpSizeType == EQP_MEDIUM_SIZE)
					pView->m_pDlgPLC->m_fnSetDevice(ADDR_VISION_CLIENT_FOLDER_ALARM, TRUE);		//2017.06.26		PLC Alarm	중알람
				//G_AddLog(2, 3, _T("VisionClient-%d Folder can't find ! "), nVisionCount);
			}
			else
				if (*nNoExistDirCnt == nVisionCount)
				{
					pFrame->m_DataManager.m_fnDisplayMSGSet(_T(""),_T(""));
					memset(nNoExistDirCnt, 0x00, sizeof(nNoExistDirCnt));
				}
				
			nVisionCount++;
		}
		else	nVisionCount = VISION_PC_1;
		Sleep(2000);
	}
	delete[] nNoExistDirCnt;

	CloseHandle(pFrame->m_hDetectVisionClient);

	return TRUE;
}

int CMainFrame::m_fnEqpSizeType(void)
{	
	int nEqpSizeType = 0;
	nEqpSizeType = m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
	return nEqpSizeType;
}

unsigned __stdcall  CMainFrame::m_fnThreadPcAliveChk(LPVOID pArg )
{	
	CMainFrame *pFrame = (CMainFrame *)pArg;
	CViewSetup* pView = (CViewSetup*)pFrame->m_pCruxData->m_fnGetViewPtr(eFORM_Setup);	//2020.08.17 add by mby
	BOOL rc = FALSE;

	while(TRUE)
	{
		if(pFrame->m_pVsSeverIF == NULL)	
		{
			CloseHandle(pFrame->m_hThreadInspAlive);
			pFrame->m_hThreadInspAlive = NULL;
			return rc;
		}

		for(int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i++)
		{			
			pFrame->m_pVsSeverIF->Seq_InspAliveChk(i + 1);
		}

		//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)	//2016.03.29 Mr.Chu
			pFrame->m_pVsSeverIF->Seq_AlignProcChk();
		//2020.08.17 add by mby for Server Live Check S-
		if( pView->m_pDlgPLC->m_fnGetDevice(ADDR_PLC_ALARM_SERVER_LIVE)){
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_SERVER_LIVE, 0);
		}else{
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_SERVER_LIVE, 1);
		}
		Sleep(1000);
	}	

	CloseHandle(pFrame->m_hThreadInspAlive);
	pFrame->m_hThreadInspAlive = NULL;
	
	return rc;
}

BOOL CMainFrame::m_fnSetOverlapLuminanceInfo()
{ 
	TCHAR strBuffer[255];
	CString strLimit;
	int nCurPos = 0;
	int nLimitCnt = 0;

	if(m_stOverlapLuminanceInfo != NULL) delete m_stOverlapLuminanceInfo; m_stOverlapLuminanceInfo = NULL;
	m_stOverlapLuminanceInfo = new STRU_OVERLAP_LUMINANCE_INFO;

	m_stOverlapLuminanceInfo->bUseMode = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_USEMODE, 0);
	m_stOverlapLuminanceInfo->nLuminanceAlarmSetCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_SET_COUNT, 0);
	m_stOverlapLuminanceInfo->nLuminanceAlarmSetPanelCount = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_SET_COUNT_PANEL, 0);
	m_stOverlapLuminanceInfo->nLuminanceRange = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_RANGE, 0);
	m_stOverlapLuminanceInfo->bUseTdiMin = m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_USE_TDI_MIN, 0);

	m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_LIMIT, _T(""), strBuffer, 255);
	strLimit = strBuffer;
	CString resTokenCount= strLimit.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{
		nLimitCnt++;
		m_stOverlapLuminanceInfo->nLuminanceLimitData[nLimitCnt - 1] = _ttoi(resTokenCount);
		resTokenCount= strLimit.Tokenize(EXT_COLON, nCurPos);
	}

	return TRUE;
}

//2016.03.14
void CMainFrame::m_fnSetSystemTime()
{
	//CString strYear = _T(""), strMonth = _T(""), strDay = _T(""), strHour = _T(""), strMinute = _T(""), strSecond = _T("");
	//int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0, nSecond = 0;
	BOOL bPlcDataRead = TRUE;
	CString strTime;
	TCHAR tszPlcAddr[10];
	int nSystemTime[SYSTEM_TIME_MAX];

	SYSTEMTIME	systime;
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	::GetLocalTime(&systime);
	memset(nSystemTime, 0, sizeof(nSystemTime));

	//Cheng Du Comment
	//for (int i = 0; i <  SYSTEM_TIME_MAX ;  i++)
	//{
	//	long lRet = -1;
	//	memset(tszPlcAddr, 0, sizeof(tszPlcAddr));

	//	if (m_fnEqpSizeType() == EQP_LARGE_SIZE)
	//		_stprintf_s(tszPlcAddr, 10, _T("D1003%d"), i);
	//	else
	//		_stprintf_s(tszPlcAddr, 10, _T("W800%d"), i);

	//	lRet = pView->m_pDlgPLC->m_fnGetDevice(tszPlcAddr);
	//	nSystemTime[i] = lRet;

	//	//lRet = pView->m_pDlgPLC->m_fnGetDeviceBlock(tszPlcAddr, 2, strTime);
	//	//nSystemTime[i] = _ttoi(strTime);
	//	if (lRet == -1)
	//	{
	//		bPlcDataRead = FALSE;
	//		break;
	//	}	
	//}

	long lRet = -1;

	nSystemTime[0] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFA"));
	nSystemTime[1] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFB"));
	nSystemTime[2] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFC"));
	nSystemTime[3] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFD"));
	nSystemTime[4] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFE"));
	nSystemTime[5] = pView->m_pDlgPLC->m_fnGetDevice(_T("W1FFF"));

	if (lRet == -1)
	{
		bPlcDataRead = FALSE;
		return ;
	}	

	strTime.Trim();

	//CIM 연결 후 Comment Out
	//if (bPlcDataRead)
	//{
	//	systime.wYear	= _ttoi(strTime.Mid(0,4));
	//	systime.wMonth	= _ttoi(strTime.Mid(4,2));
	//	systime.wDay	= _ttoi(strTime.Mid(6,2));
	//	systime.wHour	= _ttoi(strTime.Mid(8,2));
	//	systime.wMinute	= _ttoi(strTime.Mid(10,2));
	//	systime.wSecond = _ttoi(strTime.Mid(12,2));

	//	G_AddLog(3, 2, _T("m_fnSetSystemTime() %4d-%02d-%02d %02d:%02d:%02d"), nSystemTime[0], nSystemTime[1], nSystemTime[2], nSystemTime[3], nSystemTime[4], nSystemTime[5]);
	//	SetLocalTime(&systime);
	//}

}

//2017.04.04
void CMainFrame::m_fnSetVersion()
{	
	//wchar_t chRead[255] = {0};
	//CString srtVersionPath =_T(".\\Version.ini");	//Version.ini파일에 업데이트 시 업데이트 내용 기입
	//CString strVersion = _T("");

	//memset(chRead,0X00,sizeof(chRead));		
	//GetPrivateProfileString(_T("VERSION"), _T("VERSION_NO"), _T(""), (LPTSTR)chRead, 255, srtVersionPath);
	//strVersion = chRead;

	m_dDlgTopBar.m_fnSetProcState(STR_CURRENT_VERSION);
}

void CMainFrame::m_fnProcessMapSave()
{	
	((CViewAuto*)m_pViewAuto)->m_pDlgMap->TriggerDefectMap(TRUE);
	return;
}

void CMainFrame::m_fnSetLastRecipeNo()
{
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);
	TCHAR strBuffer[255] = {0,};
	int nRecipeNo = 0;

	m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);
	nRecipeNo = _tstoi(strBuffer);	
	pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_CHANGE_RECIPE_NO, nRecipeNo);
}

void CMainFrame::m_fnSetAlignLive(ULONG lLive)
{
	long lRet = -1;
	Sleep(100);	//2018.12.27 ADD BY MBY
	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)m_pCruxData->m_fnGetViewPtr(eFORM_Setup);

	lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_ALIGN_LIVE, lLive);
	
}
