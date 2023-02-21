
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "stdafx.h"
#include "ServerWorkManager.h" 
#include "ErrorProcess.h" 
#include "MainFrm.h"
#include "ViewAuto.h"
#include "ViewSetup.h"
#include "..\..\..\..\..\VisualStation\CommonHeader\Class\Interserverinterface.h"

enum
{	CF =0,	TFT,	POL };
//extern CLogWriter* GlassDataWriter;


#define	SEQUENCE_TABLE(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME)										\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )				\
	{																								\
	isFunctionRuned = true;																			\
	nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);								\
	}

//
//#define WAIT_CRITICAL_EVENT( CRITICALSEC , WAIT_TIME)											\
//{																											\
//	unsigned int nData = 0, nRet = 0;																					\
//	HANDLE Event;																					\
//	Event = CreateEvent( NULL, TRUE,  FALSE,  NULL );												\
//	SetEvent(Event);																				\
//	\
//	Sleep(10);																							\
//	nRet = ::WaitForSingleObject(Event, WAIT_TIME);														\
//	if ( nRet == WAIT_OBJECT_0 + 1)																		\
//	throw 9999;																						\
//	if ( nRet == WAIT_TIMEOUT )	{																		\
//	LeaveCriticalSection(&CRITICALSEC);															\
//	ResetEvent(Event);}											\
//}


CServerWorkManager::CServerWorkManager(CMainFrame* pMainFrame)
	: m_pMainFrame(pMainFrame), m_bThreadRun(false), m_SeqenceCount(0)
{	
	for (int i = 0 ; i < E_CRITICAL_MAX; i++)
		::InitializeCriticalSection(&m_CriticalSectione[i]);	
	for (int i = 0; i < 20; i++)
		m_handle[i] = NULL;

	for(int i = 0; i < MAX_VISION_PC; i++)
	{
		for (int j = 0; j < LIGHT_CONTROL_MAX; j++)
		{
			m_bControllerFlag[i][j] = FALSE;
			m_nTemperature[i][j] = -1;
			m_nLimitTemperature[i][j] = 0;
		}
	}

	m_strPanelID_1	 = _T("");
	m_strPanelID_2	 = _T("");
	m_linspactEqpNo	 = 0;
	m_nRecvOriginType = RECIPE_DEF_MAP_ORIGIN_LT;
}


CServerWorkManager::~CServerWorkManager()
{
	m_bThreadRun = false;
	for (int i = 0 ; i < E_CRITICAL_MAX; i++)
	{
		//::WaitForSingleObject(&m_CriticalSectione[i],INFINITE);
		::DeleteCriticalSection(&m_CriticalSectione[i]);
	}
	SAFE_DELETE(G_pVsServerIF);

	Sleep(1000);
}

int CServerWorkManager::Seq_InspAliveChk(int nVisionID)
{

	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	int			nFlag			= FALSE;

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return FALSE;

	pTempParam				= 0;
	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_CLIENT_ALIVE_CHECK, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_RES);
	
	if (nRet != APP_OK)	
	{
		m_nVisionState[nVisionID - 1] = STATE_IDLE;
		nFlag = FALSE;
		G_AddLog_ALARM(_T("Seq_InspAliveChk - Vision PC No Ready_PC%d : %d"),nVisionID, ERROR_PC_ALIVE);
	}
	else
		nFlag = TRUE;
	m_stSockStatus.bVisionClient[nVisionID - 1] = nFlag;
	//m_stSockStatus.bVisionClient[nVisionID - 1] = TRUE;	//Chung Du Test
	
	return nFlag;
}

void CServerWorkManager::m_fnRequestGrabReady(int nVisionID, int nScanDir, CString strModelID, CString strPanelID_1, CString strPanelID_2)
{
	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	ULONG lScanDirection		= 0;
	BSTR UnistrModelID			= NULL;
	BSTR UniStrPanelID1		    = NULL;
	BSTR UniStrPanelID2			= NULL;
	CString MultiByteStr		=_T("");

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	lScanDirection				= nScanDir;
	*(ULONG*)pTempParam		=	lScanDirection;		//1: Foward, 2:Backward
	pTempParam				+=	sizeof(ULONG);
	UniStrPanelID1 = strPanelID_1.AllocSysString();
	strcpy(pTempParam,(CStringA) UniStrPanelID1);
	pTempParam				+= 40;
	UniStrPanelID2 = strPanelID_2.AllocSysString();
	strcpy(pTempParam,(CStringA) UniStrPanelID2);
	pTempParam				+= 40;

	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_PLC_SEQUENCE_SET_GRAB_READY, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);

	if (nRet != APP_OK)
	{
		// Write Error Log
		G_AddLog(2, 0, _T("m_fnRequestGrabReady()_For Ping Test"));						// Add By KYH01

	}
}

void CServerWorkManager::m_fnRequestVisionRecipe(int nVisionID)
{

	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	pTempParam				= 0;
	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_CRUX_CLIENT_VISION_RECIPE_REQ, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);	

	if (nRet != APP_OK)
	{
	
	}	
}
void CServerWorkManager::m_fnVisionInitial(int nVisionID)
{

	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	pTempParam				= 0;
	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_CLIENT_INITIAL, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);	

	if (nRet != APP_OK)
	{

	}	
}
int CServerWorkManager::m_fnSelectVisionRecipe(int nVisionID, CString strRecipe)
{
	char  cParam[1000]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	ULONG lScanDirection		= 0;
	BSTR UnistrRecipe			= NULL;	
	CString MultiByteStr		=_T("");

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return FALSE;

	UnistrRecipe = strRecipe.AllocSysString();
	strcpy(pTempParam,(CStringA) UnistrRecipe);
	pTempParam				+= 1000;

	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_CLIENT_SELECT_RECIPE_SET, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);

	if (nRet != APP_OK)
	{
		return FALSE;
	}

	return TRUE;
	//CString strMsg;
	//strMsg.Format(_T("%c.%s.%s.%c"), STX, MTV_SELECT_RECIPE, strRecipe, ETX);

	//int nLen = sizeof(MTV_SELECT_RECIPE) + + strRecipe.GetLength() + 4;

	//m_pSockVisionSvr->m_fnSendClientAt(pSockClient, strMsg, nLen);

}
//2017.02.18 add by mby S-
void CServerWorkManager::m_fnRequestMonitorInfo()
{

	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	pTempParam				= 0;
	nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK , VS_CLIENT_MONITOR_INFO_REQ, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);	

	if (nRet != APP_OK)
	{
	
	}	
}
//2017.02.18 add by mby E-

//2016.11.04 Mr.Chu
//기능 : PLC Reset 시킬 경우 Align Sequence를 함께 Reset 시킨다.
int CServerWorkManager::Seq_AlignProcReset()
{
	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	int		   nFlag			= FALSE;
	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return FALSE;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	pTempParam				= 0;

	//2021.06.08 add by mby 
	if(!pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK, VS_PLC_SEQUENCE_SET_ALIGN_RESET, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_RES);
	}else{
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK_TWO, VS_PLC_SEQUENCE_SET_ALIGN_RESET, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_RES);
	}
	if (nRet != APP_OK)		nFlag = FALSE;
	else					nFlag = TRUE;

	return nFlag;
}

void CServerWorkManager::Seq_AlignProcChk()
{
	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	int		   nFlag			= FALSE;
	if (G_pVsServerIF == NULL || m_pMainFrame == NULL) return;
		//return FALSE;

	pTempParam				= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	////2018.05.15 add by mby S-
	//CViewSetup* pView = (CViewSetup*)m_pMainFrame->m_fnGetViewPtr(eFORM_Setup);

	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_ALIGN_LIVE, 0);
	////2018.05.15 add by mby E-
	//pFrame->m_fnSetAlignLive(0);
	
	//nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK, VS_PLC_SEQUENCE_SET_ALIGN_CHECK, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_RES);
	//2021.06.08 add by mby 
	if(!pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK, VS_PLC_SEQUENCE_SET_ALIGN_CHECK, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
	}else{
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK_TWO, VS_PLC_SEQUENCE_SET_ALIGN_CHECK, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
	}
	if (nRet != APP_OK)
	{
		// Write Error Log

		G_AddLog(2, 0, _T("Seq_AlignProcChk()_For Ping Test"));						// Add By KYH01
	}

	//if (nRet != APP_OK)		nFlag = FALSE;
	//else					nFlag = TRUE;

	//m_stSockStatus.bPreAlign = nFlag;

	//return nFlag;
}
void CServerWorkManager::m_fnRequestAlign(CString strPanelID_1)
{
	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	BSTR UniStrPanelID1		    = NULL;
	CString MultiByteStr		=_T("");
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	UniStrPanelID1 = strPanelID_1.AllocSysString();
	strcpy(pTempParam,(CStringA) UniStrPanelID1);
	pTempParam				+= 40;

	//2021.06.08 add by mby 
	if(!pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK, VS_PLC_SEQUENCE_SET_ALIGN_REQ, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
	}else{
		nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK_TWO, VS_PLC_SEQUENCE_SET_ALIGN_REQ, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
	}
	if (nRet != APP_OK)
	{
		// Write Error Log
		G_AddLog_STG_SEQ(3, 0,  _T("m_fnRequestAlign()_For Ping Test"));				// Add By KYH01
	}
}
int CServerWorkManager::m_fnStartVsMsgReceiver()
{
	HANDLE handle;
	m_bThreadRun = true;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	if (m_bThreadRun)
	{


		if (G_pVsServerIF == NULL)
		{			
			Sleep(1000);

			TCHAR szPlcLogPath[100],szPlcFileName[100], szServerIP[100];
			int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;
			CString szVsIniFilePath;
			szVsIniFilePath.Format(_T("%s%s"), DATA_PATH, VS_CRUX_SERVER_TASK_INI_FILE);

			nTaskNo				= GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 10, szVsIniFilePath);
			nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, szVsIniFilePath);
			uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, szVsIniFilePath);
			uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, szVsIniFilePath);
			uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, szVsIniFilePath);
			//nTaskNo +=	10;/* m_pSystem->m_Config.m_nInspactEqpNo - 1;*/

			GetPrivateProfileString(_T("LogInfo"), _T("Path") ,_T("D:\\VisualStation\\VSLOG\\VSPLC"), szPlcLogPath, sizeof(szPlcLogPath), szVsIniFilePath);
			GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("VSPLC"), szPlcFileName, sizeof(szPlcFileName), szVsIniFilePath);
			GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), szVsIniFilePath);
			G_pVsServerIF		= new CInterServerInterface(szPlcLogPath, szPlcFileName, NULL, NULL, false, uPeriodDay, uWriteTerm, true, uWriteLogLevel);
			//G_pVsServerIF = (CInterServerInterface*)G_pVsServerIF;
			int nRet = G_pVsServerIF->m_fnRegisterTask(nTaskNo, szServerIP, nServerPort);
			if (nRet != APP_OK)
			{
				SAFE_DELETE(G_pVsServerIF);
			}
			m_stSockStatus.bVisionServer = TRUE;

		}
	}
	for (int i = 0; i < 20; i++)
	{
		//handle = m_fnStartThread();
		handle = m_fnStartThread();
		if ( handle == NULL || handle == (HANDLE)-1 )
			return APP_NG;
	}


	return APP_OK;
}

void CServerWorkManager::m_fnRunThread()
{
	int nRet = APP_OK;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	while (m_bThreadRun)
	{


		//if (G_pVsServerIF == NULL)
		//{			
		//	Sleep(1000);

		//	TCHAR szPlcLogPath[100],szPlcFileName[100], szServerIP[100];
		//	int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;
		//	CString szVsIniFilePath;
		//	szVsIniFilePath.Format(_T("%s%s"), DATA_PATH, VS_CRUX_SERVER_TASK_INI_FILE);

		//	nTaskNo				= GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 10, szVsIniFilePath);
		//	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, szVsIniFilePath);
		//	uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, szVsIniFilePath);
		//	uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, szVsIniFilePath);
		//	uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, szVsIniFilePath);
		//	//nTaskNo +=	10;/* m_pSystem->m_Config.m_nInspactEqpNo - 1;*/

		//	GetPrivateProfileString(_T("LogInfo"), _T("Path") ,_T("D:\\VisualStation\\VSLOG\\VSPLC"), szPlcLogPath, sizeof(szPlcLogPath), szVsIniFilePath);
		//	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("VSPLC"), szPlcFileName, sizeof(szPlcFileName), szVsIniFilePath);
		//	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), szVsIniFilePath);
		//	G_pVsServerIF		= new CInterServerInterface(szPlcLogPath, szPlcFileName, NULL, NULL, false, uPeriodDay, uWriteTerm, true, uWriteLogLevel);
		//	//G_pVsServerIF = (CInterServerInterface*)G_pVsServerIF;
		//	int nRet = G_pVsServerIF->m_fnRegisterTask(nTaskNo, szServerIP, nServerPort);
		//	if (nRet != APP_OK)
		//	{
		//		SAFE_DELETE(G_pVsServerIF);
		//	}
		//	m_stSockStatus.bVisionServer = TRUE;
		//	
		//}
		//else
		{
			if (G_pVsServerIF)
			{	
				BYTE* bpPeekMsg = NULL;
				bpPeekMsg = G_pVsServerIF->m_fnPeekMessage();

				if (bpPeekMsg)
				{
					CMDMSG *pCmdMsg = (CMDMSG*) bpPeekMsg;

					// 메세지 처리가 필요.

					//nRet = WAIT_ON_EVENT(m_handle[i], m_fnAnalyzeMsg(pCmdMsg), 10000);

					nRet = m_fnAnalyzeMsg(pCmdMsg);	

					if ( nRet != APP_OK ) 
						G_pVsServerIF->m_fnPrintLog(0, 100, 
						_T("VsMessageProcessor::m_fnRunThread AnalyzeMsg Error. nRet = %d"), nRet);

					G_pVsServerIF->m_fnFreeMemory(bpPeekMsg);
				}
			}else
			{
				Sleep(100);	//2018.10.29 add by mby
			}

		}
	}
}

int CServerWorkManager::m_fnResponseSend(USHORT ReturnVal, void* pRcvMsg)
{
	int nRet = APP_OK;
	CMDMSG* pCmdMsg = (CMDMSG*)pRcvMsg;
	CMDMSG tempCmdMsg;
	byte* pBuff;

	pBuff = new byte[pCmdMsg->uMsgSize];

	memcpy(pBuff, (void*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize);
	memcpy(&tempCmdMsg, pCmdMsg, sizeof(CMDMSG));
	tempCmdMsg.cMsgBuf = (INT64)pBuff;

	tempCmdMsg.uRetStatus = ReturnVal;

	nRet = G_pVsServerIF->m_fnSendResponseCommand(&tempCmdMsg);

	delete pBuff;

	return nRet;
}

int CServerWorkManager::m_fnAnalyzeMsg(void* pRcvMsg)
{
	int			nRet = APP_OK;
	bool		isFunctionRuned = false;	
	CMDMSG*		pCmdMsg = (CMDMSG*)pRcvMsg;
	m_SeqenceCount++;
	//hSeqEvent = CreateEvent( NULL, TRUE,  FALSE,  NULL );

	EXCEPTION_TRY

//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					중복실행가능여부,	시퀀스 리셋 가능 여부	)
//	SEQUENCE_TABLE (	20			,		1	,	Seq_GrabStart			 )
	SEQUENCE_TABLE (	20			,		11	,	Seq_SetGrabAck			 )
	SEQUENCE_TABLE (	20			,		12	,	Seq_SetGrabEnd			 )
	SEQUENCE_TABLE (	20			,		21	,	Seq_ClassifyEnd			 )
	SEQUENCE_TABLE (	20			,		51	,	Seq_PgResult			 )
	SEQUENCE_TABLE (    20			,		22	,	Seq_Insp_JudgeEnd		 )
	SEQUENCE_TABLE (    20			,		23	,	Seq_Insp_Interlock		 )
	SEQUENCE_TABLE (    20			,		24	,	Seq_Insp_RecipeSet		 )
	SEQUENCE_TABLE (    20			,		25	,	Seq_Insp_temperature	 ) //2016.04.05 add by ikm - 온도값
	SEQUENCE_TABLE (    20			,		26	,	Seq_Insp_Ch_Luminance    ) //2016.08.08 채널별 라이트 알람 처리
	SEQUENCE_TABLE (    20			,		27	,	Seq_Insp_LightController ) //2017.02.08 조명 컨트롤러 알람
	SEQUENCE_TABLE (    20			,		28	,	Seq_Insp_CheckFile		 ) //2017.02.28 파일 알람
	SEQUENCE_TABLE (    20			,		20	,	Seq_Insp_OriginCoord	 ) //2017.07.10
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		10	,	Seq_SetSystemTime		 )
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		1	,	Seq_AlignRes			 )
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		2	,	Seq_AlignCfMeasuredstRes )
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		3	,	Seq_AlignCfMeasureAngRes )
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		4	,	Seq_AlignTftMeasuredstRes)
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		5	,	Seq_AlignTftMeasureRes   )
	SEQUENCE_TABLE (	VS_ALIGN_TASK,      6   ,   Seq_Insp_AutoRecipe      ) //Auto Recipe 관련
	SEQUENCE_TABLE (	VS_ALIGN_TASK,		7	,	Seq_Insp_MonitorInfo)
	SEQUENCE_TABLE (	VS_ALIGN_TASK,      8   ,   Seq_Insp_MonitorMeasureDataAVL) // Monitor Measure 관련
	SEQUENCE_TABLE (	VS_ALIGN_TASK,      9   ,   Seq_Insp_MonitorMeasureDataABI) // Monitor Measure 관련
	SEQUENCE_TABLE (	VS_ALIGN_TASK,      50   ,  Seq_Alignlive) 					//Align PC live check 2018.03.24 add by mby 

	if( m_SeqenceCount <= 0 )
	{
		m_SeqenceCount = 0;
	}

	if ( isFunctionRuned  == false ) 
	{
		G_AddLog_STG_SEQ(3, 0,  _T("m_fnAnalyzeMsg FuncNo : %d, SeqNo : %d ,From %d Task"),pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		G_pVsServerIF->m_fnPrintLog(0, 100,
			_T("Function Table is nothing. FuncNo : %d, SeqNo : %d ,From %d Task "), 
			pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		throw PLC_TASK_SEQUENCE_IS_NOTHING;
	}
	EXCEPTION_CATCH

	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
	{
		G_AddLog_STG_SEQ(3, 0,  _T("m_fnResponseSend. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
		nRet = m_fnResponseSend(nRet, pCmdMsg);
		if ( nRet != APP_OK ) 
		{
			G_AddLog_STG_SEQ(3, 0,  _T("Response Send  Fail. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
			G_pVsServerIF->m_fnPrintLog(0, 100,
				_T("Response Send  Fail. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
			return nRet;
		}
	}

	return nRet;
}

//2017.07.10
//기능		: Client 에서 Origin Type를 보내줌.
//실행 범위	: Initialize 실행시		,		Recipe Setting 시 ( 두가지 )
int CServerWorkManager::Seq_Insp_OriginCoord(byte* pParam, ULONG& nPrmSize)
{
	CString strTemp = _T("");
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	//
	linspactEqpNo		= *(ULONG*)tempParam;
	tempParam			+= sizeof(ULONG);
	//

	strTemp	= tempParam;
	strTemp = strTemp.Right(strTemp.Find(EXT_COLON) +2 );		//	Data는 4 자리 수
	m_nRecvOriginType = _ttoi(strTemp);							//	Get Data

	G_AddLog_STG_SEQ(3, linspactEqpNo - 1, _T("[Packet Vision][%d] OriginType : %d"), linspactEqpNo, m_nRecvOriginType);

	return APP_OK;
}

int CServerWorkManager::Seq_SetGrabAck(byte* pParam, ULONG& nPrmSize)
{
	//Parameter
	//EnterCriticalSection(&m_CriticalSectione[E_INSP_GRAB_SEQ]);	//2016.03.29 Mr.Chu
	byte*		tempParam			= pParam;
	ULONG		lGrabAckFlag		= 0;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;


	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam		+= sizeof(ULONG);
	lGrabAckFlag	= *(ULONG*)tempParam;
	G_AddLog_STG_SEQ(3, linspactEqpNo - 1, _T("[Packet Vision][%d] %s : %d"), linspactEqpNo, VTM_READY_RES, lGrabAckFlag);

	//CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	m_bRecvReadyRes[linspactEqpNo - 1] = TRUE;
	m_nGrabReadyRes[linspactEqpNo - 1] = lGrabAckFlag;
	if (lGrabAckFlag == OK)	
	{
		m_nVisionState[linspactEqpNo - 1] = STATE_GRAB;
	}

	//SAFE_DELETE_ARR(sArray);
	//sArray = new CString[20];	
	//LeaveCriticalSection(&m_CriticalSectione[E_INSP_GRAB_SEQ]);		//2016.03.29 Mr.Chu
	return APP_OK;
}

int CServerWorkManager::Seq_SetGrabEnd(byte* pParam, ULONG& nPrmSize)
{
	//EnterCriticalSection(&m_CriticalSectione[E_INSP_GRAB_SEQ]);		//2016.03.29 Mr.Chu
	int			nRet			= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		lGrabEndFlag		= 0;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;


	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam		+= sizeof(ULONG);
	lGrabEndFlag	= *(ULONG*)tempParam;

	G_AddLog_STG_SEQ(3, linspactEqpNo - 1,  _T("[Packet Vision][%d] %s : %d"), linspactEqpNo, VTM_GRAB_END, lGrabEndFlag);
	//G_AddLog_SEQ(nVisionID - 1, _T("[Packet Vision][%d] %s : %s"), nVisionID, VTM_GRAB_END, strPacket);

	// 작업중...Param 데이터 처리... 
	m_fnSetGrabEndFlag(linspactEqpNo, TRUE);
	m_fnSetGrabEndRes (linspactEqpNo, lGrabEndFlag);
	//LeaveCriticalSection(&m_CriticalSectione[E_INSP_GRAB_SEQ]);		//2016.03.29 Mr.Chu
	return nRet;
}

int CServerWorkManager::Seq_PgResult(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_PG_DATA]);	
	CString strPanelID_1, strPanelID_2, strJudge_1;
	
	CString strInspType;
	CString strConFlag, strContResult1, strContResult2, strPgResult1, strPgResult2, strPgVal1, strPgVal2;

	int			nRet			= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	/////////////////////////////////////////////////////////////////////////////////
	linspactEqpNo	 = *(ULONG*)tempParam;
	tempParam		+= sizeof(ULONG);				strPanelID_1	= tempParam;
	tempParam		+= 40;							strPanelID_2	= tempParam;
	tempParam		+= 10;							strContResult1	= tempParam;
	tempParam		+= 10;							strPgResult1	= tempParam;
	tempParam		+= 10;							strPgVal1		= tempParam;
	tempParam		+= 10;							strContResult2	= tempParam;
	tempParam		+= 10;							strPgResult2	= tempParam;
	tempParam		+= 10;							strPgVal2		= tempParam;
	tempParam		+= 10;							strConFlag		= tempParam;
	/////////////////////////////////////////////////////////////////////////////////
	/*Result Data 용 Panel 정보 처리 부*/

	G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Signal Flag][%d]"), SIGNAL_PC);
	
	int m_bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 0);

	if(m_bUseSemaphore)
		nRet = pFrame->m_DataManager.m_fnPgClassifyProcInfo(strPanelID_1, strPanelID_2, strContResult1, strPgResult1,
		strPgVal1,strContResult2, strPgResult2,	strPgVal2, strConFlag, linspactEqpNo);
	LeaveCriticalSection(&m_CriticalSectione[E_PG_DATA]);
	return nRet;
}


int CServerWorkManager::Seq_ClassifyEnd(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_CLASSIFY]);
	CString strPanelID_1, strPanelID_2/*, strJudge_1, strJudge_2, strCode_1, strCode_2*/;
	int nVisionID = 0;
	int			nRet			= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	static CString		sArray[20];

	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam   += sizeof(ULONG);

	strPanelID_1	= tempParam;
	 tempParam		+= 40;
	//tempParam		+= 10;	strJudge_1		= tempParam;
	//tempParam		+= 10;	strCode_1		= tempParam;
	strPanelID_2	= tempParam;	
	/*tempParam		+= 10;	strJudge_2		= tempParam;
	tempParam		+= 10;	strCode_2		= tempParam;
	*/
	strPanelID_1.TrimRight();
	strPanelID_2.TrimRight();
	/*G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] %s : %s, %s, %s, %s, %s, %s"), 
		linspactEqpNo, VTM_CLASSIFY_END, strPanelID_1, strJudge_1, strCode_1, strPanelID_2, strJudge_2, strCode_2);*/

	G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		linspactEqpNo, VTM_CLASSIFY_END, strPanelID_1, strPanelID_2);
	DWORD dwThreadID;
	HANDLE m_hThreadClassify;

	m_strPanelID_1	 = strPanelID_1	   ;
	m_strPanelID_2	 = strPanelID_2	   ;
	m_linspactEqpNo	 = linspactEqpNo   ;

	m_hThreadClassify = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)m_fnClassifyProcInfo, (LPVOID)m_pMainFrame, CREATE_SUSPENDED, &dwThreadID);
	ResumeThread(m_hThreadClassify);
	
	//m_pMainFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1, strPanelID_2, linspactEqpNo);
//	nRet = pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1, strPanelID_2, linspactEqpNo);
	WaitForSingleObject(m_hThreadClassify, INFINITE);
	TRACE(_T("Classify End"));
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_CLASSIFY]);
	return nRet;
}

void CServerWorkManager::m_fnClassifyProcInfo(LPVOID pWnd)
{
	CMainFrame* pFrame = (CMainFrame*)pWnd;
	
	pFrame->m_DataManager.m_fnClassifyProcInfo
		(pFrame->m_pVsSeverIF->m_strPanelID_1
		, pFrame->m_pVsSeverIF->m_strPanelID_2
		, pFrame->m_pVsSeverIF->m_linspactEqpNo);
}
int CServerWorkManager::Seq_AlignRes(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	CString strPanelID_1;
	CString strAlign_Result, strVal;
	int nVisionID = 0;
	int nResult	  = 0;
	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;	//2015.08.19
	STRU_ALIGN_INFO stAlignInfo;	//2015.08.19
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	stAlignMeasureInfo.init_info();
	strPanelID_1	= tempParam;
	 tempParam		+= 40;
	strAlign_Result	= tempParam;	

	strPanelID_1.TrimRight();
	strAlign_Result.TrimRight();
	
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s, %s"), PRE_ALIGN_PC, ATM_ALIGN_RES, strPanelID_1, strAlign_Result);
	//Align Result Save
	nResult = _ttoi(strAlign_Result);
	strPanelID_1.TrimRight();
	stAlignInfo = pFrame->m_DataManager.m_fnReadAlignData(strPanelID_1);
	strVal = stAlignMeasureInfo.cPanelID; strVal.TrimRight();
	if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
	{
		m_pMainFrame->m_DataManager.m_fnWriteAlignData(strPanelID_1, TRUE, nResult);
	}
	LeaveCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	return nRet;
}

int CServerWorkManager::Seq_AlignCfMeasuredstRes(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	CString strPanelID_1;
	CString strMeasureData, strMeasureFail, strVal;
	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	strPanelID_1	= tempParam;
	tempParam		+= 40;
	strMeasureData	= tempParam;	
	tempParam		+= 60;
	strMeasureFail  = tempParam;


	strPanelID_1.TrimRight();
	strMeasureData.TrimRight();
	strMeasureFail.TrimRight();

	CString strTempMeasure = _T("");
	strTempMeasure.Format(_T("%s.%s"),strMeasureFail, strMeasureData);
	//strMeasureData = _T("0,0,0,0,0,0,0,0");
	if (strMeasureFail == MEASUREFAIL_CF)
		m_pMainFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strTempMeasure , strPanelID_1);

	strTempMeasure = strMeasureData;
	strTempMeasure.Replace(_T("#"), _T("."));
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s, %s FailCode:%s"), 
		PRE_ALIGN_PC, ATM_CFMEASUREDST_RES, strPanelID_1, strTempMeasure, strMeasureFail);
	//Measure Data Save
	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);
	LeaveCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	return nRet;
}

int CServerWorkManager::Seq_AlignCfMeasureAngRes	(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	CString strPanelID_1;
	CString strMeasureData, strVal;
	CString strTempMeasure = _T("");

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	strPanelID_1	= tempParam;
	tempParam		+= 40;
	strMeasureData	= tempParam;	
	
	strPanelID_1.TrimRight();
	strMeasureData.TrimRight();
	
	strTempMeasure = strMeasureData;
	strTempMeasure.Replace(_T("#"), _T("."));
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
		PRE_ALIGN_PC, ATM_CFMEASUREANG_RES, strPanelID_1, strTempMeasure);
	//Measure Data Save
	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, CF_MEASURE_ANG, MEASUREFAIL_DEFAULT, MEASUREFAIL_DEFAULT);
	LeaveCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	return nRet;
}
int CServerWorkManager::Seq_AlignTftMeasuredstRes	(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	CString strPanelID_1;
	CString strMeasureData, strMeasureFail, strVal;

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	strPanelID_1	= tempParam;
	tempParam		+= 40;
	strMeasureData	= tempParam;	
	tempParam		+= 60;
	strMeasureFail  = tempParam;
	
	strPanelID_1.TrimRight();
	strMeasureData.TrimRight();
	strMeasureFail.TrimRight();

	CString strTempMeasure = _T("");
	strTempMeasure.Format(_T("%s.%s"),strMeasureFail, strMeasureData);
	//strMeasureData = _T("0,0,0,0,0,0,0,0");
	if (strMeasureFail == MEASUREFAIL_TFT)
		m_pMainFrame->m_DataManager.m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strTempMeasure , strPanelID_1);

	strTempMeasure = strMeasureData;
	strTempMeasure.Replace(_T("#"), _T("."));
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s, %s FailCode:%s"), 
		PRE_ALIGN_PC, ATM_TFTMEASUREDST_RES, strPanelID_1, strTempMeasure, strMeasureFail);
	//Measure Data Save
	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTempMeasure, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);
	LeaveCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	return nRet;
}

int CServerWorkManager::Seq_AlignTftMeasureRes		(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
	ULONG		linspactEqpNo		= 0;
	CString strPanelID_1;
	CString strMeasureData, strMeasureFail, strVal, strTemp;
	CString strX1, strY1, strX2, strY2, strAngle1, strAngle2, strDistance;	//2015.08.19
	DOUBLE dX1, dY1, dX2, dY2, dAngle1, dAngle2, dDistance;	//2015.08.19
	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;	//2015.08.19
	stAlignMeasureInfo.init_info();
	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	int nMonitorScanDIR = 0;
	int nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

	CString strTempMeasure = _T("");
	CString strTFTMeasureDST = _T("");
	CString strTFTMeasureANG = _T("");
	double DSTX[4] = {0}, DSTY[4]= {0},DSTW[2]= {0};
	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam   += sizeof(ULONG);	
	strPanelID_1	= tempParam; strPanelID_1.TrimRight();
	tempParam		+= 40;
	strTempMeasure	= tempParam; strTempMeasure.TrimRight();	
	if (strTempMeasure.IsEmpty()) strTempMeasure = _T("0,0,0,0,0,0,0,0");
			
	int i = 0;
	while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
	{
		if(i == 0)		strX1		 = strTemp;
		else if(i == 1)	strY1		 = strTemp;	
		else if(i == 2)	strX2		 = strTemp;
		else if(i == 3)	strY2		 = strTemp;
		else if(i == 4)	strAngle1	 = strTemp;
		else if(i == 5)	strAngle2	 = strTemp;
		else if(i == 6)	strDistance  = strTemp;
		i++;
	}

	strX1.Replace(_T("#"), _T("."));
	strY1.Replace(_T("#"), _T("."));
	strX2.Replace(_T("#"), _T("."));
	strY2.Replace(_T("#"), _T("."));
	strAngle1.Replace(_T("#"), _T("."));
	strAngle2.Replace(_T("#"), _T("."));
	strDistance.Replace(_T("#"), _T("."));

	//		DataInput
	dX1 = _ttof(strX1);
	dY1 = _ttof(strY1);
	if(nEqpSizeType == EQP_MONITOR_SIZE)			//EQP_MONITOR_SIZE
		nMonitorScanDIR = _ttoi(strX2);
	else
	{
		dX2 = _ttof(strX2);
		dY2 = _ttof(strY2);
	}
	dAngle1 = _ttof(strAngle1);
	dAngle2 = _ttof(strAngle2);
	dDistance = _ttof(strDistance);

	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s, %s, %s, %s, %s, %s, %s, %s"), 
		linspactEqpNo, VTM_TFTMEASURE_RES, strPanelID_1, strX1, strY1, strX2, strY2, strAngle1, strAngle2, strDistance);
	//Measure Data Save
	stAlignMeasureInfo = pFrame->m_DataManager.m_fnReadAlignMeasureData(strPanelID_1);
	//if (stAlignMeasureInfo.bVision1Complete) //PC5
	if( linspactEqpNo == VISION_PC_7 )			// PC7			By ChungDu
	{
		DSTX[0] = stAlignMeasureInfo.dVision1_X1;	//PC5
		DSTY[0] = stAlignMeasureInfo.dVision1_Y1;	//PC5
		DSTX[1] = stAlignMeasureInfo.dVision1_X2;	//PC5
		DSTY[1] = stAlignMeasureInfo.dVision1_Y2;	//PC5
		DSTX[2]	= dX1;								//PC6
		DSTY[2]	= dY1;								//PC6
		DSTX[3] = dX2;								//PC6
		DSTY[3] = dY2;								//PC6
		strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")	//2015.10.31 .2lf->.0lf
									,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);

		DSTW[0] =(stAlignMeasureInfo.dVision1_Y1 + dY1);//W3
		DSTW[1] =(stAlignMeasureInfo.dVision1_Y2 + dY2);//W4
		strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
								, dDistance, stAlignMeasureInfo.dVision1_Distance
								, DSTW[0], DSTW[1]
								, dAngle1, dAngle2
								, stAlignMeasureInfo.dVision1_Angle1, stAlignMeasureInfo.dVision1_Angle2);

		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, MEASUREFAIL_DEFAULT);
		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT, MEASUREFAIL_DEFAULT);
		G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d]%s : %s DST - %s, ANG - %s"), 
			linspactEqpNo,VTM_TFTMEASURE_RES, strPanelID_1, strTFTMeasureDST, strTFTMeasureANG);
	}
	//else if (stAlignMeasureInfo.bVision2Complete) //PC6
	else if( linspactEqpNo == VISION_PC_9 )			//PC9		By ChungDu
	{
		if(nEqpSizeType == EQP_MONITOR_SIZE)			//EQP_MONITOR_SIZE
		{
			// 모니터에서는 Grapper의 존재로 인해 TFT측정을 Grapper가 영상에 안찍힌 상태로 두번 수행한다..[ 3번째 데이터에 스캔 방향 정보값 ] 
			if( nMonitorScanDIR == SCAN_DIR_FORWARD )
			{
				DSTX[0] = dX1;								//PC5
				DSTY[0] = dY1;								//PC5
				DSTX[1] = stAlignMeasureInfo.dVision1_X2;	//PC5
				DSTY[1] = stAlignMeasureInfo.dVision1_Y2;	//PC5
			}
			else if( nMonitorScanDIR ==  SCAN_DIR_BACKWARD )			
			{
				DSTX[0] = stAlignMeasureInfo.dVision1_X1;	//PC5
				DSTY[0] = stAlignMeasureInfo.dVision1_Y1;	//PC5
				DSTX[1] = dX1;								//PC5
				DSTY[1] = dY1;								//PC5
			}
		}
		else											//	EQP_LARGE_SIZE 
		{
			DSTX[0] = dX1;								//PC5
			DSTY[0] = dY1;								//PC5
			DSTX[1] = dX2;								//PC5
			DSTY[1] = dY2;								//PC5
		}
		DSTX[2]	= stAlignMeasureInfo.dVision2_X1;	//PC6
		DSTY[2]	= stAlignMeasureInfo.dVision2_Y1;	//PC6
		DSTX[3]	= stAlignMeasureInfo.dVision2_X2;	//PC6
		DSTY[3]	= stAlignMeasureInfo.dVision2_Y2;	//PC6

		strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
									,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);

		DSTW[0] =(stAlignMeasureInfo.dVision2_Y1 + dY1); //W3
		DSTW[1] =(stAlignMeasureInfo.dVision2_Y2 + dY2); //W4
		strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
								,stAlignMeasureInfo.dVision2_Distance, dDistance
								,DSTW[0], DSTW[1]
								,stAlignMeasureInfo.dVision2_Angle1, stAlignMeasureInfo.dVision2_Angle2
								,dAngle1, dAngle2);

		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, MEASUREFAIL_DEFAULT);
		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT, MEASUREFAIL_DEFAULT);
		G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s DST - %s, ANG - %s"), 
			linspactEqpNo,VTM_TFTMEASURE_RES, strPanelID_1, strTFTMeasureDST, strTFTMeasureANG);
	}
	else
		pFrame->m_DataManager.m_fnWriteMeasureData(strPanelID_1, TRUE,  dX1, dY1, dX2, dY2, dAngle1, dAngle2, dDistance, linspactEqpNo);
	LeaveCriticalSection(&m_CriticalSectione[E_ALIGN_DATA]);
		return nRet;
}

int CServerWorkManager::Seq_Insp_JudgeEnd(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	int			nRet				= APP_OK;		
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;	
	BOOL		bNgFlag				= FALSE;
	CString strJudge, strPanelID;

	linspactEqpNo	   = *(ULONG*)tempParam;
	tempParam	      += sizeof(ULONG);			strJudge		   = tempParam;
	tempParam	      += 40;					strPanelID		   = tempParam;
	strJudge.TrimRight();
	strPanelID.TrimRight();
	
		 if (strJudge == _T("OK"))
		bNgFlag = FALSE;
	else if (strJudge == _T("NG"))
		bNgFlag = TRUE;

	//G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] %s : %s, %s"), 
	//	linspactEqpNo, VTM_JUDGE_END, strPanelID,strJudge);
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	return nRet;
}


int CServerWorkManager::Seq_Insp_Interlock			(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;	
	BOOL		bNgFlag				= FALSE;
	CString		strInterlockFlag;

	linspactEqpNo	   = *(ULONG*)tempParam;	tempParam  += sizeof(ULONG);
	strInterlockFlag   = tempParam;
	
	strInterlockFlag.TrimRight();
	///////////////////////////////////////////////////////////////////////////

	long lRet = -1;
	CString strTmp = _T("");
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	int nEqpSizeType = pFrame->m_fnEqpSizeType();
	
	if (strInterlockFlag == _T("ON"))
	{
		if (linspactEqpNo == VISION_PC_1)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK1, 1);
		else if (linspactEqpNo == VISION_PC_2)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK2, 1);
		else if (linspactEqpNo == VISION_PC_3)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK3, 1);
		else if (linspactEqpNo == VISION_PC_4)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK4, 1);
		else if (linspactEqpNo == VISION_PC_5)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK5, 1);
		else if (linspactEqpNo == VISION_PC_6)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK6, 1);
		else if (linspactEqpNo == VISION_PC_7)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK7, 1);
		else if (linspactEqpNo == VISION_PC_8)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK8, 1);
		else if (linspactEqpNo == VISION_PC_9)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK9, 1);

		strTmp.Format(_T("[Packet Vision][%d] %s : %s"),linspactEqpNo, VTM_PROCESS_INTERLOCK, strInterlockFlag);
		pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Message"),strTmp);
	}
	else if (strInterlockFlag == _T("OFF"))
	{
		if (linspactEqpNo == VISION_PC_1)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK1, 0);
		else if (linspactEqpNo == VISION_PC_2)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK2, 0);
		else if (linspactEqpNo == VISION_PC_3)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK3, 0);
		else if (linspactEqpNo == VISION_PC_4)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK4, 0);
		else if (linspactEqpNo == VISION_PC_5)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK5, 0);
		else if (linspactEqpNo == VISION_PC_6)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK6, 0);
		else if (linspactEqpNo == VISION_PC_7)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK7, 0);
		else if (linspactEqpNo == VISION_PC_8)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK8, 0);
		else if (linspactEqpNo == VISION_PC_9)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_INTERLOCK9, 0);
		pFrame->m_DataManager.m_fnDistroyMSGDialog();
	}
//#if LARGE_SIZE_MODE	
	else if ( strInterlockFlag == _T("ALIGNFAIL") ) // 16.05.06 sgkim
	{
		if ( nEqpSizeType == EQP_LARGE_SIZE || nEqpSizeType == EQP_MONITOR_SIZE )
		{
			if(linspactEqpNo == VISION_PC_1)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC1, 1);
			if(linspactEqpNo == VISION_PC_2)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC2, 1);
			if(linspactEqpNo == VISION_PC_3)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC3, 1);
			if(linspactEqpNo == VISION_PC_4)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC4, 1);
			if(linspactEqpNo == VISION_PC_5)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC5, 1);
			if(linspactEqpNo == VISION_PC_6)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC6, 1);
			if(linspactEqpNo == VISION_PC_7)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC7, 1);
			if(linspactEqpNo == VISION_PC_8)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC8, 1);
			if(linspactEqpNo == VISION_PC_9)
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_ALIGN_FAIL_PC9, 1);
		}
	}
	else if ( strInterlockFlag == _T("GRABFAIL") ) //2018.06.28
	{
		if(linspactEqpNo == VISION_PC_1)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC1, 1);
		if(linspactEqpNo == VISION_PC_2)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC2, 1);
		if(linspactEqpNo == VISION_PC_3)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC3, 1);
		if(linspactEqpNo == VISION_PC_4)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC4, 1);
		if(linspactEqpNo == VISION_PC_5)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC5, 1);
		if(linspactEqpNo == VISION_PC_6)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC6, 1);
		if(linspactEqpNo == VISION_PC_7)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC7, 1);
		if(linspactEqpNo == VISION_PC_8)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC8, 1);
		if(linspactEqpNo == VISION_PC_9)
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_GRAB_FAIL_PC9, 1);		
	}
//#endif
	G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] %s : %s"), 
		linspactEqpNo, VTM_PROCESS_INTERLOCK, strInterlockFlag);
	G_AddLog_ALARM(_T("Seq_Insp_Interlock - Vision PC%d: %d"),linspactEqpNo, ERROR_INSP_INTERLOCK);
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	return nRet;
}
int CServerWorkManager::Seq_Insp_Luminance			(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	int			nRet				= APP_OK;
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	return nRet;
}
int CServerWorkManager::Seq_Insp_RecipeSet			(byte* pParam, ULONG& nPrmSize)
{	
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	CString strTemp, strParam, strRecipeFlag, strRecipe;

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	ULONG		lRcpListSize		= 0;
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;


	linspactEqpNo	= *(ULONG*)tempParam;		tempParam += sizeof(ULONG);			
	strParam		= tempParam;
	strParam.TrimRight();
	////////////////////////////////////////////////////
	CString strTempRecipe = _T("");	
	int i = 0;
	while(AfxExtractSubString(strTemp, strParam, i, '.'))
	{
		if(i == 0)		strRecipeFlag = strTemp;
		else if(i == 1)	strRecipe	  = strTemp;
		i++;
	}

	m_bRecvRecipeRes[linspactEqpNo - 1] = TRUE;

	int nUpdateFlag = _tstoi(strRecipeFlag);
	
		strTempRecipe = strRecipe;
		strTempRecipe.Replace(_T("#"), _T("."));
	
		pFrame->m_DataManager.m_fnSetRecipeVision(linspactEqpNo, strTempRecipe, nUpdateFlag);
		G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] Seq_Insp_RecipeSet"), linspactEqpNo);
		LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
		return nRet;
}
//2016.04.05 add by ikm - 온도값
int CServerWorkManager::Seq_Insp_temperature		(byte* pParam, ULONG& nPrmSize)
{	
	//Sleep(300);
	//EnterCriticalSection(&m_CriticalSectione[E_TEMPERATURE]);
	CString strTemp, strTremperature, strRecipeFlag, strRecipe;
	
	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	int			nCamNo				= 0;
	WORD		wTemperature,	wErrorTempVal;
	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	
	linspactEqpNo	= *(ULONG*)tempParam;		tempParam += sizeof(ULONG);			
	nCamNo			= *(BYTE*)tempParam;		tempParam += sizeof(BYTE);	
	wTemperature	= *(WORD*)tempParam;		tempParam += sizeof(WORD);		
	wErrorTempVal	= *(WORD*)tempParam;		tempParam += sizeof(WORD);		

	//strTremperature	= tempParam;
	//strTremperature.TrimRight();
	////////////////////////////////////////////////////
	
	// PC NO, 카메라 NO, 온도 값을 보내 UI에 출력하고 로그에 남긴다.	
	//pAuto->SendMessage(, (WPARAM)nCamNo, (LPARAM)(LPCTSTR)strTremperature);
	//2016.04.04
	
	//int nCtrFlag = (linspactEqpNo - 1) *2 + (nCamNo - 1);
	m_bControllerFlag[linspactEqpNo - 1][nCamNo - 1] = TRUE;
	m_nTemperature[linspactEqpNo - 1][nCamNo - 1] = wTemperature;
	m_nLimitTemperature[linspactEqpNo - 1][nCamNo - 1] =  wErrorTempVal;
	//2016.04.04
	//G_ADDLOG_TEMPERATURE(_T("[Packet Vision][%d] [Camera No][%d] [Limit Temperature][%d] [temperature][%d]"), linspactEqpNo, nCamNo, wErrorTempVal, wTemperature);
	//G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] Seq_Insp_temperature"), linspactEqpNo);
	//LeaveCriticalSection(&m_CriticalSectione[E_TEMPERATURE]);
	return nRet;
}
int CServerWorkManager::Seq_Insp_State				(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	int			nRet				= APP_OK;
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);
	return nRet;
}

//2016.03.14
int CServerWorkManager::Seq_SetSystemTime(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_SET_SYSTIME]);
	int			nRet			= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		lGrabEndFlag		= 0;
	ULONG		linspactEqpNo		= 0;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;


	linspactEqpNo	= *(ULONG*)tempParam;
	//tempParam		+= sizeof(ULONG);
	//lGrabEndFlag	= *(ULONG*)tempParam;
	if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
		pFrame->m_fnSetSystemTime();
	G_AddLog_STG_SEQ(3, linspactEqpNo - 1,  _T("[Packet Vision][%d] %s"), linspactEqpNo, _T("SYSTEM TIME REQUEST"));
	m_fnResponseSystemTime(linspactEqpNo);
	LeaveCriticalSection(&m_CriticalSectione[E_SET_SYSTIME]);
	return nRet;
}
//2018.03.24 add by mby
int CServerWorkManager::Seq_Alignlive(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_SET_SYSTIME]);
	int			nRet			= APP_OK;
	byte*		tempParam			= pParam;
	ULONG		lGrabEndFlag		= 0;
	ULONG		linspactEqpNo		= 0;	
	ULONG		lLive				= 0;	//2018.05.15 add by mby

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	////2018.05.15 add by mby S-
	//CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	lLive	= *(ULONG*)tempParam;

	pFrame->m_fnSetAlignLive(lLive);
	//pView->m_pDlgPLC->m_fnSetDevice(ADDR_PLC_ALARM_ALIGN_LIVE, lLive);
	//2018.05.15 add by mby E-

	m_stSockStatus.bPreAlign = TRUE;
	LeaveCriticalSection(&m_CriticalSectione[E_SET_SYSTIME]);
	return nRet;
}

//2016.08.09
int CServerWorkManager::Seq_Insp_MonitorInfo(byte* pParam, ULONG& nPrmSize)
{
	ULONG		linspactEqpNo	= 0;
	ULONG		lCalMode		= 0;
	ULONG		lModelSize		= 0;
	ULONG		lReferencePoint = 0;

	CString		strPanel_X, strPanel_Y;
	DOUBLE		dDesignValue_X, dDesignValue_Y;

	CString		strDX1, strDY1, strDX2, strDY2, strDX3, strDY3, strDX4, strDY4, strDL1, strDL2, strDW1, strDW2;
	DOUBLE		dDesignValue_DX1, dDesignValue_DY1, dDesignValue_DX2, dDesignValue_DY2, dDesignValue_DX3, dDesignValue_DY3, dDesignValue_DX4, dDesignValue_DY4, dDesignValue_DL1, dDesignValue_DL2, dDesignValue_DW1, dDesignValue_DW2;

	CString		strDX5, strDY5, strDX6, strDY6, strDX7, strDY7, strDX8, strDY8, strDL3, strDL4, strDW3, strDW4;
	DOUBLE		dDesignValue_DX5, dDesignValue_DY5, dDesignValue_DX6, dDesignValue_DY6, dDesignValue_DX7, dDesignValue_DY7, dDesignValue_DX8, dDesignValue_DY8, dDesignValue_DL3, dDesignValue_DL4, dDesignValue_DW3, dDesignValue_DW4;

	CString		strTempMeasure, strTemp, strMeasureMonitorInfoPath;
	STRU_MONITOR_MEASURE_INFO	stMonitorMeasureInfo;
	stMonitorMeasureInfo.init_info();

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	linspactEqpNo	= *(ULONG*)tempParam;	tempParam   += sizeof(ULONG);
	lCalMode		= *(ULONG*)tempParam;	tempParam   += sizeof(ULONG);
	lModelSize		= *(ULONG*)tempParam;

	stMonitorMeasureInfo = pFrame->m_DataManager.m_fnReadMonitorMeasureInfo();

	stMonitorMeasureInfo.bCalMode =			lCalMode;	//0:INSP	1:SERVER
	stMonitorMeasureInfo.bModelSize =		lModelSize; //0:중		1:대

	if(lCalMode == MEASURE_CAL_SERVER)
	{
		tempParam   += sizeof(ULONG);
		lReferencePoint = *(ULONG*)tempParam;	tempParam   += sizeof(ULONG);

		strTempMeasure	= tempParam; strTempMeasure.TrimRight();	
		if (strTempMeasure.IsEmpty()) strTempMeasure = _T("0,0,0,0,0,0,0,0,0,0,0,0,0,0");

		int i = 0;
		if (linspactEqpNo == VISION_PC_1 || linspactEqpNo == VISION_PC_2)
		{
			while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
			{
				if(i == 0)			{strPanel_X	= strTemp;	strPanel_X.Replace(_T("#"), _T("."));	dDesignValue_X = _ttof(strPanel_X);}
				else if(i == 1)		{strPanel_Y	= strTemp;	strPanel_Y.Replace(_T("#"), _T("."));	dDesignValue_Y = _ttof(strPanel_Y);}
				else if(i == 2)		{strDX1	= strTemp;	strDX1.Replace(_T("#"), _T("."));		dDesignValue_DX1 = _ttof(strDX1);}
				else if(i == 3)		{strDY1	= strTemp;	strDY1.Replace(_T("#"), _T("."));		dDesignValue_DY1 = _ttof(strDY1);}
				else if(i == 4)		{strDX2	= strTemp;	strDX2.Replace(_T("#"), _T("."));		dDesignValue_DX2 = _ttof(strDX2);}
				else if(i == 5)		{strDY2	= strTemp;	strDY2.Replace(_T("#"), _T("."));		dDesignValue_DY2 = _ttof(strDY2);}
				else if(i == 6)		{strDX3	= strTemp;	strDX3.Replace(_T("#"), _T("."));		dDesignValue_DX3 = _ttof(strDX3);}
				else if(i == 7)		{strDY3	= strTemp;	strDY3.Replace(_T("#"), _T("."));		dDesignValue_DY3 = _ttof(strDY3);}
				else if(i == 8)		{strDX4	= strTemp;	strDX4.Replace(_T("#"), _T("."));		dDesignValue_DX4 = _ttof(strDX4);}
				else if(i == 9)		{strDY4	= strTemp;	strDY4.Replace(_T("#"), _T("."));		dDesignValue_DY4 = _ttof(strDY4);}
				else if(i == 10)	{strDL1	= strTemp;	strDL1.Replace(_T("#"), _T("."));		dDesignValue_DL1 = _ttof(strDL1);}
				else if(i == 11)	{strDL2	= strTemp;	strDL2.Replace(_T("#"), _T("."));		dDesignValue_DL2 = _ttof(strDL2);}
				else if(i == 12)	{strDW1	= strTemp;	strDW1.Replace(_T("#"), _T("."));		dDesignValue_DW1 = _ttof(strDW1);}
				else if(i == 13)	{strDW2	= strTemp;	strDW2.Replace(_T("#"), _T("."));		dDesignValue_DW2 = _ttof(strDW2);}

				i++;
			}

			stMonitorMeasureInfo.bReferencePoint =	lReferencePoint;	//0:CAM1 1:CAM2
			stMonitorMeasureInfo.dPanel_X =			dDesignValue_X;
			stMonitorMeasureInfo.dPanel_Y =			dDesignValue_Y;
			stMonitorMeasureInfo.d_DX1 =			dDesignValue_DX1;
			stMonitorMeasureInfo.d_DY1 =			dDesignValue_DY1;
			stMonitorMeasureInfo.d_DX2 =			dDesignValue_DX2;
			stMonitorMeasureInfo.d_DY2 =			dDesignValue_DY2;
			stMonitorMeasureInfo.d_DX3 =			dDesignValue_DX3;
			stMonitorMeasureInfo.d_DY3 =			dDesignValue_DY3;
			stMonitorMeasureInfo.d_DX4 =			dDesignValue_DX4;
			stMonitorMeasureInfo.d_DY4 =			dDesignValue_DY4;
			stMonitorMeasureInfo.d_DL1 =			dDesignValue_DL1;
			stMonitorMeasureInfo.d_DL2 =			dDesignValue_DL2;
			stMonitorMeasureInfo.d_DW1 =			dDesignValue_DW1;
			stMonitorMeasureInfo.d_DW2 =			dDesignValue_DW2;
		}
		else if(linspactEqpNo == VISION_PC_5 || linspactEqpNo == VISION_PC_6)
		{
			while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
			{
				if(i == 0)			{strPanel_X	= strTemp;	strPanel_X.Replace(_T("#"), _T("."));	dDesignValue_X = _ttof(strPanel_X);}
				else if(i == 1)		{strPanel_Y	= strTemp;	strPanel_Y.Replace(_T("#"), _T("."));	dDesignValue_Y = _ttof(strPanel_Y);}
				else if(i == 2)		{strDX5	= strTemp;	strDX5.Replace(_T("#"), _T("."));		dDesignValue_DX5 = _ttof(strDX5);}
				else if(i == 3)		{strDY5	= strTemp;	strDY5.Replace(_T("#"), _T("."));		dDesignValue_DY5 = _ttof(strDY5);}
				else if(i == 4)		{strDX6	= strTemp;	strDX6.Replace(_T("#"), _T("."));		dDesignValue_DX6 = _ttof(strDX6);}
				else if(i == 5)		{strDY6	= strTemp;	strDY6.Replace(_T("#"), _T("."));		dDesignValue_DY6 = _ttof(strDY6);}
				else if(i == 6)		{strDX7	= strTemp;	strDX7.Replace(_T("#"), _T("."));		dDesignValue_DX7 = _ttof(strDX7);}
				else if(i == 7)		{strDY7	= strTemp;	strDY7.Replace(_T("#"), _T("."));		dDesignValue_DY7 = _ttof(strDY7);}
				else if(i == 8)		{strDX8	= strTemp;	strDX8.Replace(_T("#"), _T("."));		dDesignValue_DX8 = _ttof(strDX8);}
				else if(i == 9)		{strDY8	= strTemp;	strDY8.Replace(_T("#"), _T("."));		dDesignValue_DY8 = _ttof(strDY8);}
				else if(i == 10)	{strDL3	= strTemp;	strDL3.Replace(_T("#"), _T("."));		dDesignValue_DL3 = _ttof(strDL3);}
				else if(i == 11)	{strDL4	= strTemp;	strDL4.Replace(_T("#"), _T("."));		dDesignValue_DL4 = _ttof(strDL4);}
				else if(i == 12)	{strDW3	= strTemp;	strDW3.Replace(_T("#"), _T("."));		dDesignValue_DW3 = _ttof(strDW3);}
				else if(i == 13)	{strDW4	= strTemp;	strDW4.Replace(_T("#"), _T("."));		dDesignValue_DW4 = _ttof(strDW4);}

				i++;
			}

			stMonitorMeasureInfo.bReferencePoint =	lReferencePoint;	//0:CAM1 1:CAM2
			stMonitorMeasureInfo.dPanel_X =			dDesignValue_X;
			stMonitorMeasureInfo.dPanel_Y =			dDesignValue_Y;
			stMonitorMeasureInfo.d_DX5 =			dDesignValue_DX5;
			stMonitorMeasureInfo.d_DY5 =			dDesignValue_DY5;
			stMonitorMeasureInfo.d_DX6 =			dDesignValue_DX6;
			stMonitorMeasureInfo.d_DY6 =			dDesignValue_DY6;
			stMonitorMeasureInfo.d_DX7 =			dDesignValue_DX7;
			stMonitorMeasureInfo.d_DY7 =			dDesignValue_DY7;
			stMonitorMeasureInfo.d_DX8 =			dDesignValue_DX8;
			stMonitorMeasureInfo.d_DY8 =			dDesignValue_DY8;
			stMonitorMeasureInfo.d_DL3 =			dDesignValue_DL3;
			stMonitorMeasureInfo.d_DL4 =			dDesignValue_DL4;
			stMonitorMeasureInfo.d_DW3 =			dDesignValue_DW3;
			stMonitorMeasureInfo.d_DW4 =			dDesignValue_DW4;
		}
	}
	pFrame->m_DataManager.m_fnWriteMonitorMeasureInfo(stMonitorMeasureInfo);

	return nRet;
}

int CServerWorkManager::Seq_Insp_MonitorMeasureDataAVL(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_MONITOR_MEASURE_INFO]);
	TCHAR strBuffer[255];
	ULONG		linspactEqpNo		= 0;
	CString strPanelID_1;
	CString strMeasureData, strMeasureFail, strVal, strTemp, strCFMeasureDST, strTFTMeasureDST;
	CString str_X1, str_Y1, str_X2, str_Y2, str_X3, str_Y3, str_X4, str_Y4;
	CString strOffSet_X1, strOffSet_Y1, strOffSet_X2, strOffSet_Y2, strOffSet_X3, strOffSet_Y3, strOffSet_X4, strOffSet_Y4;
	CString str_Ypos;
	DOUBLE d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4;
	DOUBLE dOffSet_X1, dOffSet_Y1, dOffSet_X2, dOffSet_Y2, dOffSet_X3, dOffSet_Y3, dOffSet_X4, dOffSet_Y4;
	CString strReferencePoint_X, strReferencePoint_Y;
	DOUBLE dReferencePoint_X, dReferencePoint_Y;
	DOUBLE dYpos,dYdiff;
	CString strMode, strSize; 

	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
	stAlignMeasureInfo.init_info();
	STRU_MONITOR_MEASURE_INFO stMonitorMeasureInfo;
	stMonitorMeasureInfo.init_info();

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	stMonitorMeasureInfo = pFrame->m_DataManager.m_fnReadMonitorMeasureInfo();

	int nEqpSizeType = pFrame->m_fnEqpSizeType();

	//2017.04.29	모니터 측정 스펙 설정
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_MEASURE_SPEC, _T("Spec"), _T(""), (LPTSTR)strBuffer, 255);
	LPTSTR endPtr;
	double dSpecAVL = _tcstod(strBuffer, &endPtr );
	//

	CString strTempMeasure = _T("");

	double DSTX[4] = {0}, DSTY[4]= {0};

	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam   += sizeof(ULONG);	
	strPanelID_1	= tempParam; strPanelID_1.TrimRight();
	tempParam		+= 40;
	if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
	{
		strTempMeasure	= tempParam; strTempMeasure.TrimRight();	
		if (strTempMeasure.IsEmpty()) 
			strTempMeasure = _T("0,0,0,0,0,0,0,0");

		int i = 0;
		while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
		{
			if(i == 0)		{str_X1	= strTemp;	str_X1.Replace(_T("#"), _T("."));	d_X1 = _ttof(str_X1);}
			else if(i == 1)	{str_Y1 = strTemp;	str_Y1.Replace(_T("#"), _T("."));	d_Y1 = _ttof(str_Y1);}
			else if(i == 2)	{str_X2	= strTemp;	str_X2.Replace(_T("#"), _T("."));	d_X2 = _ttof(str_X2);}
			else if(i == 3)	{str_Y2	= strTemp;	str_Y2.Replace(_T("#"), _T("."));	d_Y2 = _ttof(str_Y2);}
			else if(i == 4)	{str_X3	= strTemp;	str_X3.Replace(_T("#"), _T("."));	d_X3 = _ttof(str_X3);}
			else if(i == 5)	{str_Y3	= strTemp;	str_Y3.Replace(_T("#"), _T("."));	d_Y3 = _ttof(str_Y3);}
			else if(i == 6)	{str_X4	= strTemp;	str_X4.Replace(_T("#"), _T("."));	d_X4 = _ttof(str_X4);}
			else if(i == 7)	{str_Y4	= strTemp;	str_Y4.Replace(_T("#"), _T("."));	d_Y4 = _ttof(str_Y4);}

			i++;
		}

	}
	else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
	{
		strTempMeasure	= tempParam; strTempMeasure.TrimRight();

		int i = 0;
		while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))	
		{
			if(i == 0)			{str_X1					= strTemp;	str_X1.Replace(_T("#"), _T("."));				d_X1 = _ttof(str_X1);}
			else if(i == 1)		{str_Y1					= strTemp;	str_Y1.Replace(_T("#"), _T("."));				d_Y1 = _ttof(str_Y1);}
			else if(i == 2)		{str_X2					= strTemp;	str_X2.Replace(_T("#"), _T("."));				d_X2 = _ttof(str_X2);}
			else if(i == 3)		{str_Y2					= strTemp;	str_Y2.Replace(_T("#"), _T("."));				d_Y2 = _ttof(str_Y2);}
			else if(i == 4)		{str_X3					= strTemp;	str_X3.Replace(_T("#"), _T("."));				d_X3 = _ttof(str_X3);}
			else if(i == 5)		{str_Y3					= strTemp;	str_Y3.Replace(_T("#"), _T("."));				d_Y3 = _ttof(str_Y3);}
			else if(i == 6)		{str_X4					= strTemp;	str_X4.Replace(_T("#"), _T("."));				d_X4 = _ttof(str_X4);}
			else if(i == 7)		{str_Y4					= strTemp;	str_Y4.Replace(_T("#"), _T("."));				d_Y4 = _ttof(str_Y4);}
			else if(i == 8)		{strOffSet_X1			= strTemp;	strOffSet_X1.Replace(_T("#"), _T("."));			dOffSet_X1 = _ttof(strOffSet_X1);}
			else if(i == 9)		{strOffSet_Y1			= strTemp;	strOffSet_Y1.Replace(_T("#"), _T("."));			dOffSet_Y1 = _ttof(strOffSet_Y1);}
			else if(i == 10)	{strOffSet_X2			= strTemp;	strOffSet_X2.Replace(_T("#"), _T("."));			dOffSet_X2 = _ttof(strOffSet_X2);}
			else if(i == 11)	{strOffSet_Y2			= strTemp;	strOffSet_Y2.Replace(_T("#"), _T("."));			dOffSet_Y2 = _ttof(strOffSet_Y2);}
			else if(i == 12)	{strOffSet_X3			= strTemp;	strOffSet_X3.Replace(_T("#"), _T("."));			dOffSet_X3 = _ttof(strOffSet_X3);}
			else if(i == 13)	{strOffSet_Y3			= strTemp;	strOffSet_Y3.Replace(_T("#"), _T("."));			dOffSet_Y3 = _ttof(strOffSet_Y3);}
			else if(i == 14)	{strOffSet_X4			= strTemp;	strOffSet_X4.Replace(_T("#"), _T("."));			dOffSet_X4 = _ttof(strOffSet_X4);}
			else if(i == 15)	{strOffSet_Y4			= strTemp;	strOffSet_Y4.Replace(_T("#"), _T("."));			dOffSet_Y4 = _ttof(strOffSet_Y4);}
			else if(i == 24)	{strReferencePoint_X	= strTemp;	strReferencePoint_X.Replace(_T("#"), _T("."));	dReferencePoint_X = _ttof(strReferencePoint_X);}
			else if(i == 25)	{strReferencePoint_Y	= strTemp;	strReferencePoint_Y.Replace(_T("#"), _T("."));	dReferencePoint_Y = _ttof(strReferencePoint_Y);}
			else if(i == 30)	{str_Ypos				= strTemp;	str_Ypos.Replace(_T("#"), _T("."));				dYpos = _ttof(str_Ypos);}

			i++;
		}
	}

	tempParam		+= 600;
	strMeasureFail  = tempParam;
	strMeasureFail.TrimRight();

	if (stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
		strMode = _T("CLIENT");
	else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
		strMode = _T("SERVER");

	if (stMonitorMeasureInfo.bModelSize == 0)
		strSize = _T("MIDDLE");
	else if(stMonitorMeasureInfo.bModelSize == 1)
		strSize = _T("LARGE");

 	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s %s %s: %s, %s, %s, %s, %s, %s, %s, %s, %s"), 
 		linspactEqpNo, _T("AVL DATA"), strSize, strMode, strPanelID_1, str_X1, str_Y1, str_X2, str_Y2, str_X3, str_Y3, str_X4, str_Y4);

	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s %s %s: %s, %s, %s, %s, %s, %s, %s, %s, %s"), 
		linspactEqpNo, _T("AVL OFFSET"), strSize, strMode, strPanelID_1, strOffSet_X1, strOffSet_Y1, strOffSet_X2, strOffSet_Y2, strOffSet_X3, strOffSet_Y3, strOffSet_X4, strOffSet_Y4);

	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s %s %s: %s, %s, %s, %s"), 
		linspactEqpNo, _T("AVL REF"), strSize, strMode, strPanelID_1, strReferencePoint_X, strReferencePoint_Y, str_Ypos);

	//임시 원래는 레시피에서 받아온다.
	stAlignMeasureInfo = pFrame->m_DataManager.m_fnReadAlignMeasureData(strPanelID_1);

	//if(m_bEqpSizeType > TRUE || m_bEqpSizeType < FALSE)
	//{
	//	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Model Size Error] [%d] Check Vision Model"),m_bEqpSizeType );
	//	return FALSE;
	//}

	switch(stMonitorMeasureInfo.bModelSize)
	{
	case 0:		// 중형
		if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
		{
			////pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE,dLT_X,dLT_Y,dLB_X,dLB_Y,dRT_X,dRT_Y,dRB_X,dRB_Y,linspactEqpNo);
			//if(linspactEqpNo == VISION_PC_2)
			//{
			//	strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
			//		,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
			//	if(d_X1 < 100 || d_Y1 < 100 || d_X2 < 100 || d_Y2 < 100
			//		|| d_X3 < 100 || d_Y3 < 100 || d_X4 < 100 || d_Y4 < 100)
			//		strMeasureFail = MEASUREFAIL_CF;
			//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);
			//}
			//else if(linspactEqpNo == VISION_PC_6)
			//{
			//	strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
			//		,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
			//	if(d_X1 < 100 || d_Y1 < 100 || d_X2 < 100 || d_Y2 < 100
			//		|| d_X3 < 100 || d_Y3 < 100 || d_X4 < 100 || d_Y4 < 100)
			//		strMeasureFail = MEASUREFAIL_TFT;
			//	pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);
			//}

			if(linspactEqpNo == VISION_PC_2 || linspactEqpNo == VISION_PC_6)
			{
				if (stAlignMeasureInfo.cTFTMeasureDistanceData != _T(""))	//PC6 In -> PC2 In
				{
					strMeasureData = stAlignMeasureInfo.cTFTMeasureDistanceData;

					int nCurPos = 0;
					CString resToken= strMeasureData.Tokenize(EXT_COLON, nCurPos);
					while (resToken != "")
					{
						resToken= strMeasureData.Tokenize(EXT_COLON, nCurPos);
						if(nCurPos == 5)		d_X3 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 6)	d_Y3 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 7)	d_X4 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 8)	d_Y4 = _tstof((LPCTSTR)resToken);
					}

					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
					if(abs(d_X1) > dSpecAVL || abs(d_Y1) > dSpecAVL || abs(d_X2) > dSpecAVL || abs(d_Y2) > dSpecAVL
						|| abs(d_X3) > dSpecAVL || abs(d_Y3) > dSpecAVL || abs(d_X4) > dSpecAVL || abs(d_Y4) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_TFT;
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);

				}
				else if(stAlignMeasureInfo.cCFMeasureDistanceData != _T(""))	//PC2 In -> PC6 In
				{
					strMeasureData = stAlignMeasureInfo.cCFMeasureDistanceData;

					int nCurPos = 0;
					CString resToken= strMeasureData.Tokenize(EXT_COLON, nCurPos);
					while (resToken != "")
					{
						resToken= strMeasureData.Tokenize(EXT_COLON, nCurPos);
						if(nCurPos == 1)		d_X1 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 2)	d_Y1 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 3)	d_X2 = _tstof((LPCTSTR)resToken);
						else if(nCurPos == 4)	d_Y2 = _tstof((LPCTSTR)resToken);
					}

					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
					if(abs(d_X1) > dSpecAVL || abs(d_Y1) > dSpecAVL || abs(d_X2) > dSpecAVL || abs(d_Y2) > dSpecAVL
						|| abs(d_X3) > dSpecAVL || abs(d_Y3) > dSpecAVL || abs(d_X4) > dSpecAVL || abs(d_Y4) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_TFT;
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);

				}
				else
				{
					if(linspactEqpNo == VISION_PC_2)	//PC2 In
					{
						strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
							,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
						if(abs(d_X1) > 100 || abs(d_Y1) > dSpecAVL || abs(d_X2) > dSpecAVL || abs(d_Y2) > dSpecAVL
							|| abs(d_X3) > 100 || abs(d_Y3) > dSpecAVL || abs(d_X4) > dSpecAVL || abs(d_Y4) > dSpecAVL)
							strMeasureFail = MEASUREFAIL_CF;
						pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);
					}
					else if(linspactEqpNo == VISION_PC_6)	//PC6 In
					{
						strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
							,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
						if(abs(d_X1) > dSpecAVL || abs(d_Y1) > dSpecAVL || abs(d_X2) > dSpecAVL || abs(d_Y2) > dSpecAVL
							|| abs(d_X3) > dSpecAVL || abs(d_Y3) > dSpecAVL || abs(d_X4) > dSpecAVL || abs(d_Y4) > dSpecAVL)
							strMeasureFail = MEASUREFAIL_TFT;
						pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);
					}
				}
			}
		}
		else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
		{
			if(linspactEqpNo == VISION_PC_2)
			{
				DSTX[0] = stMonitorMeasureInfo.d_DX1 - abs(dReferencePoint_X - (d_X1 + dOffSet_X1));
				DSTY[0] = stMonitorMeasureInfo.d_DY1 - abs(dReferencePoint_Y - (d_Y1 + dOffSet_Y1));
				DSTX[1] = stMonitorMeasureInfo.d_DX2 - abs(dReferencePoint_X - (d_X2 + dOffSet_X2));
				DSTY[1] = stMonitorMeasureInfo.d_DY2 - abs(dReferencePoint_Y - (d_Y2 + dOffSet_Y2));
				DSTX[2] = stMonitorMeasureInfo.d_DX3 - abs(dReferencePoint_X - (d_X3 + dOffSet_X3));
				DSTY[2] = stMonitorMeasureInfo.d_DY3 - abs(dReferencePoint_Y - (d_Y3 + dOffSet_Y3));
				DSTX[3] = stMonitorMeasureInfo.d_DX4 - abs(dReferencePoint_X - (d_X4 + dOffSet_X4));
				DSTY[3] = stMonitorMeasureInfo.d_DY4 - abs(dReferencePoint_Y - (d_Y4 + dOffSet_Y4));
				strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
					,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
				if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) > dSpecAVL)
					strMeasureFail = MEASUREFAIL_CF;
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);
			}
			else if(linspactEqpNo == VISION_PC_6)
			{
				DSTX[0] = stMonitorMeasureInfo.d_DX5 - abs(dReferencePoint_X - (d_X1 + dOffSet_X1)); //X5
				DSTY[0] = stMonitorMeasureInfo.d_DY5 - abs(dReferencePoint_Y - (d_Y1 + dOffSet_Y1)); //Y5
				DSTX[1] = stMonitorMeasureInfo.d_DX6 - abs(dReferencePoint_X - (d_X2 + dOffSet_X2)); //X6
				DSTY[1] = stMonitorMeasureInfo.d_DY6 - abs(dReferencePoint_Y - (d_Y2 + dOffSet_Y2)); //Y6
				DSTX[2] = stMonitorMeasureInfo.d_DX7 - abs(dReferencePoint_X - (d_X3 + dOffSet_X3)); //X7
				DSTY[2] = stMonitorMeasureInfo.d_DY7 - abs(dReferencePoint_Y - (d_Y3 + dOffSet_Y3)); //Y7
				DSTX[3] = stMonitorMeasureInfo.d_DX8 - abs(dReferencePoint_X - (d_X4 + dOffSet_X4)); //X8
				DSTY[3] = stMonitorMeasureInfo.d_DY8 - abs(dReferencePoint_Y - (d_Y4 + dOffSet_Y4)); //Y8
				strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
					,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
				if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) > dSpecAVL
					|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) > dSpecAVL)
					strMeasureFail = MEASUREFAIL_TFT;
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);
			}
		}
		break;
	case 1:		// 모니터
		if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)	//검사에서 계산
		{
			//대형일 경우 4자리 씩 데이터를 끊어 받는다.
			if ( linspactEqpNo == VISION_PC_1 || linspactEqpNo == VISION_PC_2 )	//	CF		PC 1  /  PC 2
			{
				if (stAlignMeasureInfo.bVision1Complete)		// Insert PC1 Data		
				{
					DSTX[0] = ValueLimitChk1(d_X1);									//PC2
					DSTY[0] = ValueLimitChk1(d_Y1);									//PC2
					DSTX[1] = ValueLimitChk1(d_X2);									//PC2
					DSTY[1] = ValueLimitChk1(d_Y2);									//PC2
					DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X3);			//PC1
					DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y3);			//PC1
					DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X4);			//PC1
					DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y4);			//PC1
					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(DSTX[0]) > dSpecAVL || abs(DSTY[0]) > dSpecAVL || abs(DSTX[1]) > dSpecAVL || abs(DSTY[1]) > dSpecAVL
						|| abs(DSTX[2]) > dSpecAVL || abs(DSTY[2]) > dSpecAVL || abs(DSTX[3]) > dSpecAVL || abs(DSTY[3]) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_CF;
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s"), 
						linspactEqpNo, _T("AVL CF DATA"), strCFMeasureDST);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT); //계산된 값과 스펙 비교후 알람 여부 기입
				}
				else if(stAlignMeasureInfo.bVision2Complete)			// Insert PC2 Data
				{
					DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dCF_X1);			//PC2
					DSTY[0] = ValueLimitChk1(stAlignMeasureInfo.dCF_Y1);			//PC2
					DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dCF_X2);			//PC2
					DSTY[1] = ValueLimitChk1(stAlignMeasureInfo.dCF_Y2);			//PC2
					DSTX[2]	= ValueLimitChk1(d_X3);									//PC1
					DSTY[2]	= ValueLimitChk1(d_Y3);									//PC1
					DSTX[3] = ValueLimitChk1(d_X4);									//PC1
					DSTY[3] = ValueLimitChk1(d_Y4);									//PC1
					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(DSTX[0]) > dSpecAVL || abs(DSTY[0]) > dSpecAVL || abs(DSTX[1]) > dSpecAVL || abs(DSTY[1]) > dSpecAVL
						|| abs(DSTX[2]) > dSpecAVL || abs(DSTY[2]) > dSpecAVL || abs(DSTX[3]) > dSpecAVL || abs(DSTY[3]) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_CF;
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s"), 
						linspactEqpNo, _T("AVL CF DATA"), strCFMeasureDST);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT); //계산된 값과 스펙 비교후 알람 여부 기입
				}
				else
				{
					if(linspactEqpNo == VISION_PC_1)
					{
						DSTX[0] = d_X3;
						DSTY[0] = d_Y3;
						DSTX[1] = d_X4;
						DSTY[1]	= d_Y4;
					}
					else if(linspactEqpNo == VISION_PC_2)
					{
						DSTX[0] = d_X1;
						DSTY[0] = d_Y1;
						DSTX[1] = d_X2;
						DSTY[1]	= d_Y2;
					}

					pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
				}
			}
			if ( linspactEqpNo == VISION_PC_5 || linspactEqpNo == VISION_PC_2 )		//	TFT		PC 2  /  PC 6
			{	
				if (stAlignMeasureInfo.bVision5Complete)		// Insert PC5 Data		
				{
					DSTX[0] = ValueLimitChk1(d_X1);									//PC2
					DSTY[0] = ValueLimitChk1(d_Y1);									//PC2
					DSTX[1] = ValueLimitChk1(d_X2);									//PC2
					DSTY[1] = ValueLimitChk1(d_Y2);									//PC2
					DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X3);			//PC5
					DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y3);			//PC5
					DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X4);			//PC5
					DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y4);			//PC5
					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(DSTX[0]) > dSpecAVL || abs(DSTY[0]) > dSpecAVL || abs(DSTX[1]) > dSpecAVL || abs(DSTY[1]) > dSpecAVL
						|| abs(DSTX[2]) > dSpecAVL || abs(DSTY[2]) > dSpecAVL || abs(DSTX[3]) > dSpecAVL || abs(DSTY[3]) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_TFT;
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s"), 
						linspactEqpNo, _T("AVL TFT DATA"), strTFTMeasureDST);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else if(stAlignMeasureInfo.bVision6Complete) // Insert PC6 Data
				{
					DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X1);			//PC2
					DSTY[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y1);			//PC2
					DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X2);			//PC2
					DSTY[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y2);			//PC2
					DSTX[2]	= ValueLimitChk1(d_X3);									//PC5
					DSTY[2]	= ValueLimitChk1(d_Y3);									//PC5
					DSTX[3] = ValueLimitChk1(d_X4);									//PC5
					DSTY[3] = ValueLimitChk1(d_Y4);									//PC5
					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(DSTX[0]) > dSpecAVL || abs(DSTY[0]) > dSpecAVL || abs(DSTX[1]) > dSpecAVL || abs(DSTY[1]) > dSpecAVL
						|| abs(DSTX[2]) > dSpecAVL || abs(DSTY[2]) > dSpecAVL || abs(DSTX[3]) > dSpecAVL || abs(DSTY[3]) > dSpecAVL)
						strMeasureFail = MEASUREFAIL_TFT;
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s : %s"), 
						linspactEqpNo, _T("AVL TFT DATA"), strTFTMeasureDST);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else
				{
					if(linspactEqpNo == VISION_PC_5)
					{
						DSTX[0] = d_X3;
						DSTY[0] = d_Y3;
						DSTX[1] = d_X4;
						DSTY[1]	= d_Y4;

						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
					}
					else if(linspactEqpNo == VISION_PC_2)
					{
						DSTX[0] = d_X1;
						DSTY[0] = d_Y1;
						DSTX[1] = d_X2;
						DSTY[1] = d_Y2;

						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, VISION_PC_6);
					}
				}
			}
		}
		else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)	//서버에서 계산
		{
			if ( linspactEqpNo == VISION_PC_1 || linspactEqpNo == VISION_PC_2 )		//	CF		PC 1  /  PC 2
			{
				if (stAlignMeasureInfo.bVision1Complete)		// Insert PC1 Data
				{					
					dYdiff = pFrame->m_dYPos_L[CF] - dYpos;

					if(stMonitorMeasureInfo.bReferencePoint == 0)
					{
						dReferencePoint_X = pFrame->m_dReferencePoint_X[CF];
						dReferencePoint_Y = pFrame->m_dReferencePoint_Y[CF];
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX1 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1));	//PC2
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY1 - (abs(dReferencePoint_Y - (d_Y1+dYdiff)) + dOffSet_Y1));	//PC2
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX2 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2));	//PC2
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY2 - (abs(dReferencePoint_Y - (d_Y2+dYdiff)) + dOffSet_Y2));	//PC2						
						DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X3);					//PC1
						DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y3);					//PC1
						DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X4);					//PC1
						DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y4);					//PC1
					}
					else
					{
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX1 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1));	//PC2
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY1 - (abs(dReferencePoint_Y - (d_Y1+dYdiff)) + dOffSet_Y1));	//PC2
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX2 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2));	//PC2
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY2 - (abs(dReferencePoint_Y - (d_Y2+dYdiff)) + dOffSet_Y2));	//PC2	
						DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X3 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3));	//PC1
						DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y3 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3));	//PC1
						DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X4 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4));	//PC1
						DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y4 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4)); //PC1
					}

					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) > dSpecAVL)
					{
						strMeasureFail = MEASUREFAIL_CF;
					}
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else if(stAlignMeasureInfo.bVision2Complete)			// Insert PC2 Data
				{
					dYdiff = dYpos - pFrame->m_dYPos_R[CF];

					if(stMonitorMeasureInfo.bReferencePoint == 1)
					{
						dReferencePoint_X = pFrame->m_dReferencePoint_X[CF];
						dReferencePoint_Y = pFrame->m_dReferencePoint_Y[CF];
						DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dCF_X1);					//PC2
						//DSTY[0] = stAlignMeasureInfo.dCF_Y1;					//PC2
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY1 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[CF][0]+dYdiff)) + pFrame->m_dPosion_Y[CF][1]));	//PC2
						DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dCF_X2);					//PC2
						//DSTY[1] = stAlignMeasureInfo.dCF_Y2;					//PC2
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY2 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[CF][2]+dYdiff)) + pFrame->m_dPosion_Y[CF][3]));	//PC2
					}
					else
					{
						DSTX[0]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X1 - (abs(dReferencePoint_X - pFrame->m_dPosion_X[CF][0]) + pFrame->m_dPosion_X[CF][1]));	//PC1
						DSTY[0]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y1 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[CF][0]+dYdiff)) + pFrame->m_dPosion_Y[CF][1]));	//PC1
						DSTX[1]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X2 - (abs(dReferencePoint_X - pFrame->m_dPosion_X[CF][2]) + pFrame->m_dPosion_X[CF][3]));	//PC1
						DSTY[1]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y2 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[CF][2]+dYdiff)) + pFrame->m_dPosion_Y[CF][3])); //PC1
					}

					DSTX[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DX3 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3));	//PC1
					DSTY[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DY3 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3));	//PC1
					DSTX[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DX4 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4));	//PC1
					DSTY[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DY4 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4));    //PC1

					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) > dSpecAVL)
					{
						strMeasureFail = MEASUREFAIL_CF;
					}
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else
				{
					if(linspactEqpNo == VISION_PC_1)
					{
						if(stMonitorMeasureInfo.bReferencePoint == 0)
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX3 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3);
							DSTY[0] = stMonitorMeasureInfo.d_DY3 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3);
							DSTX[1] = stMonitorMeasureInfo.d_DX4 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4);
							DSTY[1] = stMonitorMeasureInfo.d_DY4 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4);
							pFrame->m_dReferencePoint_X[CF] = dReferencePoint_X;
							pFrame->m_dReferencePoint_Y[CF] = dReferencePoint_Y;							
						}
						else
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX3;
							DSTY[0] = stMonitorMeasureInfo.d_DY3;
							DSTX[1] = stMonitorMeasureInfo.d_DX4;
							DSTY[1] = stMonitorMeasureInfo.d_DY4;
						}
						pFrame->m_dYPos_L[CF] = dYpos;
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1, TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
					}
					else if(linspactEqpNo == VISION_PC_2)
					{
						if(stMonitorMeasureInfo.bReferencePoint == 1)
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX1 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1);
							//DSTY[0] = stMonitorMeasureInfo.d_DY1 - (abs(dReferencePoint_Y - d_Y1) + dOffSet_Y1);
							DSTX[1] = stMonitorMeasureInfo.d_DX2 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2);
							//DSTY[1] = stMonitorMeasureInfo.d_DY2 - (abs(dReferencePoint_Y - d_Y2) + dOffSet_Y2);
							pFrame->m_dReferencePoint_X[CF] = dReferencePoint_X;
							pFrame->m_dReferencePoint_Y[CF] = dReferencePoint_Y;							
						}
						else
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX1;
							DSTY[0] = stMonitorMeasureInfo.d_DY1;
							DSTX[1] = stMonitorMeasureInfo.d_DX2;
							DSTY[1] = stMonitorMeasureInfo.d_DY2;
						}
						pFrame->m_dPosion_X[CF][0] = d_X1;
						pFrame->m_dPosion_X[CF][1] = dOffSet_X1;
						pFrame->m_dPosion_X[CF][2] = d_X2;
						pFrame->m_dPosion_X[CF][3] = dOffSet_X2;
						pFrame->m_dPosion_Y[CF][0] = d_Y1;
						pFrame->m_dPosion_Y[CF][1] = dOffSet_Y1;
						pFrame->m_dPosion_Y[CF][2] = d_Y2;
						pFrame->m_dPosion_Y[CF][3] = dOffSet_Y2;
						pFrame->m_dYPos_R[CF] = dYpos;
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1, TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
					}
				}
			}

			else if ( linspactEqpNo == VISION_PC_5 || linspactEqpNo == VISION_PC_6 )		//	TFT		PC 5  /  PC 6
			{
				if (stAlignMeasureInfo.bVision5Complete)		// Insert PC5 Data		
				{
					dYdiff = pFrame->m_dYPos_L[TFT] - dYpos;

					if(stMonitorMeasureInfo.bReferencePoint == 0)
					{
						dReferencePoint_X = pFrame->m_dReferencePoint_X[TFT];
						dReferencePoint_Y = pFrame->m_dReferencePoint_Y[TFT];
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX5 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1));	//PC6
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY5 - (abs(dReferencePoint_Y - (d_Y1+dYdiff)) + dOffSet_Y1));	//PC6
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX6 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2));	//PC6
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY6 - (abs(dReferencePoint_Y - (d_Y2+dYdiff)) + dOffSet_Y2));	//PC6
						DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X3);					//PC5
						DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y3);					//PC5
						DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X4);					//PC5
						DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y4);					//PC5
					}
					else
					{
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX5 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1));	//PC6
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY5 - (abs(dReferencePoint_Y - (d_Y1+dYdiff)) + dOffSet_Y1));	//PC6
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX6 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2));	//PC6
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY6 - (abs(dReferencePoint_Y - (d_Y2+dYdiff)) + dOffSet_Y2));	//PC6
						DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X3 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3));					//PC5
						DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y3 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3));					//PC5
						DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X4 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4));					//PC5
						DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y4 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4));					//PC5
					}

					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) > dSpecAVL)
					{
						strMeasureFail = MEASUREFAIL_TFT;
					}
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else if(stAlignMeasureInfo.bVision6Complete) // Insert PC6 Data
				{
					dYdiff = dYpos - pFrame->m_dYPos_R[TFT];

					if(stMonitorMeasureInfo.bReferencePoint == 1)
					{
						dReferencePoint_X = pFrame->m_dReferencePoint_X[TFT];
						dReferencePoint_Y = pFrame->m_dReferencePoint_Y[TFT];
						DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X1);					//PC6
						//DSTY[0] = stAlignMeasureInfo.dTFT_Y2;					//PC6
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY5 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[TFT][0]+dYdiff)) + pFrame->m_dPosion_Y[TFT][1])); //Y5
						DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X2);					//PC6
						//DSTY[1] = stAlignMeasureInfo.dTFT_Y2;					//PC6
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY6 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[TFT][2]+dYdiff)) + pFrame->m_dPosion_Y[TFT][3])); //Y6
					}
					else
					{
						DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X1 - (abs(dReferencePoint_X - pFrame->m_dPosion_X[TFT][0]) + pFrame->m_dPosion_X[TFT][1])); //PC6
						DSTY[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y1 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[TFT][0]+dYdiff)) + pFrame->m_dPosion_Y[TFT][1])); //PC6
						DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X2 - (abs(dReferencePoint_X - pFrame->m_dPosion_X[TFT][2]) + pFrame->m_dPosion_X[TFT][3])); //PC6
						DSTY[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y2 - (abs(dReferencePoint_Y - (pFrame->m_dPosion_Y[TFT][2]+dYdiff)) + pFrame->m_dPosion_Y[TFT][3])); //PC6
					}

					DSTX[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DX7 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3));	//PC5
					DSTY[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DY7 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3));	//PC5
					DSTX[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DX8 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4));	//PC5
					DSTY[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DY8 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4));	//PC5

					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) > dSpecAVL
						|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) > dSpecAVL || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) > dSpecAVL)
					{
						strMeasureFail = MEASUREFAIL_TFT;
					}
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);	//계산된 값과 스펙 비교후 알람 여부 기입
				}
				else
				{
					if(linspactEqpNo == VISION_PC_5)
					{
						if(stMonitorMeasureInfo.bReferencePoint == 0)
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX7 - (abs(dReferencePoint_X - d_X3) + dOffSet_X3); //X7
							DSTY[0] = stMonitorMeasureInfo.d_DY7 - (abs(dReferencePoint_Y - d_Y3) + dOffSet_Y3); //Y7
							DSTX[1] = stMonitorMeasureInfo.d_DX8 - (abs(dReferencePoint_X - d_X4) + dOffSet_X4); //X8
							DSTY[1] = stMonitorMeasureInfo.d_DY8 - (abs(dReferencePoint_Y - d_Y4) + dOffSet_Y4); //Y8

							pFrame->m_dReferencePoint_X[TFT] = dReferencePoint_X;
							pFrame->m_dReferencePoint_Y[TFT] = dReferencePoint_Y;
						}
						else
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX7; //X7
							DSTY[0] = stMonitorMeasureInfo.d_DY7; //Y7
							DSTX[1] = stMonitorMeasureInfo.d_DX8; //X8
							DSTY[1] = stMonitorMeasureInfo.d_DY8; //Y8
						}
						pFrame->m_dYPos_L[TFT] = dYpos;
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1, TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
					}
					else if(linspactEqpNo == VISION_PC_6)
					{
						if(stMonitorMeasureInfo.bReferencePoint == 1)
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX5 - (abs(dReferencePoint_X - d_X1) + dOffSet_X1); //X5
							//DSTY[0] = stMonitorMeasureInfo.d_DY5 - (abs(dReferencePoint_Y - d_Y1) + dOffSet_Y1); //Y5
							DSTX[1] = stMonitorMeasureInfo.d_DX6 - (abs(dReferencePoint_X - d_X2) + dOffSet_X2); //X6
							//DSTY[1] = stMonitorMeasureInfo.d_DY6 - (abs(dReferencePoint_Y - d_Y2) + dOffSet_Y2); //Y6

							pFrame->m_dReferencePoint_X[TFT] = dReferencePoint_X;
							pFrame->m_dReferencePoint_Y[TFT] = dReferencePoint_Y;
						}
						else
						{
							DSTX[0] = stMonitorMeasureInfo.d_DX5; //X5
							DSTY[0] = stMonitorMeasureInfo.d_DY5; //Y5
							DSTX[1] = stMonitorMeasureInfo.d_DX6; //X6
							DSTY[1] = stMonitorMeasureInfo.d_DY6; //Y6
						}
						pFrame->m_dPosion_X[TFT][0] = d_X1;
						pFrame->m_dPosion_X[TFT][1] = dOffSet_X1;
						pFrame->m_dPosion_X[TFT][2] = d_X2;
						pFrame->m_dPosion_X[TFT][3] = dOffSet_X2;
						pFrame->m_dPosion_Y[TFT][0] = d_Y1;
						pFrame->m_dPosion_Y[TFT][1] = dOffSet_Y1;
						pFrame->m_dPosion_Y[TFT][2] = d_Y2;
						pFrame->m_dPosion_Y[TFT][3] = dOffSet_Y2;
						pFrame->m_dYPos_R[TFT] = dYpos;
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1, TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], 0, 0, linspactEqpNo);
					}
				}
			}
		}

		break;
	}

	LeaveCriticalSection(&m_CriticalSectione[E_INSP_MONITOR_MEASURE_INFO]);
	return nRet;
}

int CServerWorkManager::Seq_Insp_MonitorMeasureDataABI(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_MONITOR_MEASURE_INFO]);
	TCHAR		strBuffer[255];
	ULONG		linspactEqpNo		= 0;
	CString strPanelID_1;
	CString strMeasureData, strMeasureFail, strVal, strTemp, strCFMeasureDST, strTFTMeasureDST, strCFMeasureANG, strTFTMeasureANG;
	CString str_X1, str_Y1, str_X2, str_Y2, str_X3, str_Y3, str_X4, str_Y4;
	CString str_L1, str_L2, str_W1, str_W2, str_FL1, str_FL2, str_FW1, str_FW2;
	CString strOffSet_X1, strOffSet_Y1, strOffSet_X2, strOffSet_Y2, strOffSet_X3, strOffSet_Y3, strOffSet_X4, strOffSet_Y4;
	//CString strOffSet_L1, strOffSet_L2, strOffSet_W1, strOffSet_W2, strOffSet_FL1, strOffSet_FL2, strOffSet_FW1, strOffSet_FW2;
	CString str_T1, str_T2, str_T3, str_T4;
	CString strReferencePoint_X, strReferencePoint_Y;
	CString str_Ypos;
	DOUBLE d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4;
	DOUBLE d_L1, d_L2, d_W1, d_W2, d_FL1, d_FL2, d_FW1, d_FW2;
	DOUBLE dOffSet_X1, dOffSet_Y1, dOffSet_X2, dOffSet_Y2, dOffSet_X3, dOffSet_Y3, dOffSet_X4, dOffSet_Y4;
	//DOUBLE dOffSet_L1, dOffSet_L2, dOffSet_W1, dOffSet_W2, dOffSet_FL1, dOffSet_FL2, dOffSet_FW1, dOffSet_FW2;
	DOUBLE d_T1, d_T2, d_T3, d_T4;
	DOUBLE dReferencePoint_X, dReferencePoint_Y;
	DOUBLE dYpos;
	CString strMode, strSize; 

	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
	stAlignMeasureInfo.init_info();
	STRU_MONITOR_MEASURE_INFO stMonitorMeasureInfo;
	stMonitorMeasureInfo.init_info();

	int			nRet				= APP_OK;
	byte*		tempParam			= pParam;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	stMonitorMeasureInfo = pFrame->m_DataManager.m_fnReadMonitorMeasureInfo();

	int nEqpSizeType = pFrame->m_fnEqpSizeType();

	CString strTempMeasure = _T("");

	//2017.04.29	모니터 측정 스펙 설정
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_MEASURE_SPEC, _T("Spec"), _T(""), (LPTSTR)strBuffer, 255);
	LPTSTR endPtr;
	double dSpecABI = _tcstod(strBuffer, &endPtr );
	//

	double DSTX[4] = {0}, DSTY[4]= {0}, DSTL[2]= {0}, DSTW[2]= {0};

	linspactEqpNo	= *(ULONG*)tempParam;
	tempParam   += sizeof(ULONG);	
	strPanelID_1	= tempParam; strPanelID_1.TrimRight();
	tempParam		+= 40;
	if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
	{
		strTempMeasure	= tempParam; strTempMeasure.TrimRight();	
		//if (strTempMeasure.IsEmpty()) strTempMeasure = _T("0,0,0,0,0,0,0,0,0,0,0,0,90,90,90,90");

		int i = 0;
		while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))		//ATM.MEASURE.RES.PanelID.MeasureData
		{
			if(i == 0)		{str_X1	= strTemp;	str_X1.Replace(_T("#"), _T("."));	d_X1 = _ttof(str_X1);}
			else if(i == 1)	{str_Y1	= strTemp;	str_Y1.Replace(_T("#"), _T("."));	d_Y1 = _ttof(str_Y1);}
			else if(i == 2)	{str_X2	= strTemp;	str_X2.Replace(_T("#"), _T("."));	d_X2 = _ttof(str_X2);}
			else if(i == 3)	{str_Y2	= strTemp;	str_Y2.Replace(_T("#"), _T("."));	d_Y2 = _ttof(str_Y2);}
			else if(i == 4)	{str_X3	= strTemp;	str_X3.Replace(_T("#"), _T("."));	d_X3 = _ttof(str_X3);}
			else if(i == 5)	{str_Y3	= strTemp;	str_Y3.Replace(_T("#"), _T("."));	d_Y3 = _ttof(str_Y3);}
			else if(i == 6)	{str_X4	= strTemp;	str_X4.Replace(_T("#"), _T("."));	d_X4 = _ttof(str_X4);}
			else if(i == 7)	{str_Y4	= strTemp;	str_Y4.Replace(_T("#"), _T("."));	d_Y4 = _ttof(str_Y4);}
			else if(i == 8)	{str_L1	= strTemp;	str_L1.Replace(_T("#"), _T("."));	d_L1 = _ttof(str_L1);}
			else if(i == 9)	{str_L2	= strTemp;	str_L2.Replace(_T("#"), _T("."));	d_L2 = _ttof(str_L2);}
			else if(i == 10){str_W1	= strTemp;	str_W1.Replace(_T("#"), _T("."));	d_W1 = _ttof(str_W1);}
			else if(i == 11){str_W2	= strTemp;	str_W2.Replace(_T("#"), _T("."));	d_W2 = _ttof(str_W2);}

			i++;
		}

	}
	else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
	{
		strTempMeasure	= tempParam; strTempMeasure.TrimRight();

		int i = 0;
		while(AfxExtractSubString(strTemp, strTempMeasure, i, ','))	
		{
			if(i == 0)			{str_X1					= strTemp;	str_X1.Replace(_T("#"), _T("."));				d_X1 = _ttof(str_X1);}
			else if(i == 1)		{str_Y1					= strTemp;	str_Y1.Replace(_T("#"), _T("."));				d_Y1 = _ttof(str_Y1);}
			else if(i == 2)		{str_X2					= strTemp;	str_X2.Replace(_T("#"), _T("."));				d_X2 = _ttof(str_X2);}
			else if(i == 3)		{str_Y2					= strTemp;	str_Y2.Replace(_T("#"), _T("."));				d_Y2 = _ttof(str_Y2);}
			else if(i == 4)		{str_X3					= strTemp;	str_X3.Replace(_T("#"), _T("."));				d_X3 = _ttof(str_X3);}
			else if(i == 5)		{str_Y3					= strTemp;	str_Y3.Replace(_T("#"), _T("."));				d_Y3 = _ttof(str_Y3);}
			else if(i == 6)		{str_X4					= strTemp;	str_X4.Replace(_T("#"), _T("."));				d_X4 = _ttof(str_X4);}
			else if(i == 7)		{str_Y4					= strTemp;	str_Y4.Replace(_T("#"), _T("."));				d_Y4 = _ttof(str_Y4);}
			else if(i == 8)		{strOffSet_X1			= strTemp;	strOffSet_X1.Replace(_T("#"), _T("."));			dOffSet_X1 = _ttof(strOffSet_X1);}
			else if(i == 9)		{strOffSet_Y1			= strTemp;	strOffSet_Y1.Replace(_T("#"), _T("."));			dOffSet_Y1 = _ttof(strOffSet_Y1);}
			else if(i == 10)	{strOffSet_X2			= strTemp;	strOffSet_X2.Replace(_T("#"), _T("."));			dOffSet_X2 = _ttof(strOffSet_X2);}
			else if(i == 11)	{strOffSet_Y2			= strTemp;	strOffSet_Y2.Replace(_T("#"), _T("."));			dOffSet_Y2 = _ttof(strOffSet_Y2);}
			else if(i == 12)	{strOffSet_X3			= strTemp;	strOffSet_X3.Replace(_T("#"), _T("."));			dOffSet_X3 = _ttof(strOffSet_X3);}
			else if(i == 13)	{strOffSet_Y3			= strTemp;	strOffSet_Y3.Replace(_T("#"), _T("."));			dOffSet_Y3 = _ttof(strOffSet_Y3);}
			else if(i == 14)	{strOffSet_X4			= strTemp;	strOffSet_X4.Replace(_T("#"), _T("."));			dOffSet_X4 = _ttof(strOffSet_X4);}
			else if(i == 15)	{strOffSet_Y4			= strTemp;	strOffSet_Y4.Replace(_T("#"), _T("."));			dOffSet_Y4 = _ttof(strOffSet_Y4);}
			else if(i == 16)	{str_L1					= strTemp;	str_L1.Replace(_T("#"), _T("."));				d_L1 = _ttof(str_L1);}
			else if(i == 17)	{str_L2					= strTemp;	str_L2.Replace(_T("#"), _T("."));				d_L2 = _ttof(str_L2);}
			else if(i == 18)	{str_W1					= strTemp;	str_W1.Replace(_T("#"), _T("."));				d_W1 = _ttof(str_W1);}
			else if(i == 19)	{str_W2					= strTemp;	str_W2.Replace(_T("#"), _T("."));				d_W2 = _ttof(str_W2);}
			else if(i == 20)	{str_FL1				= strTemp;	str_FL1.Replace(_T("#"), _T("."));				d_FL1 = _ttof(str_FL1);}
			else if(i == 21)	{str_FL2				= strTemp;	str_FL2.Replace(_T("#"), _T("."));				d_FL2 = _ttof(str_FL2);}
			else if(i == 22)	{str_FW1				= strTemp;	str_FW1.Replace(_T("#"), _T("."));				d_FW1 = _ttof(str_FW1);}
			else if(i == 23)	{str_FW2				= strTemp;	str_FW2.Replace(_T("#"), _T("."));				d_FW2 = _ttof(str_FW2);}
			else if(i == 26)	{str_T1					= strTemp;	str_T1.Replace(_T("#"), _T("."));				d_T1 = _ttof(str_T1);}
			else if(i == 27)	{str_T2					= strTemp;	str_T2.Replace(_T("#"), _T("."));				d_T2 = _ttof(str_T2);}
			else if(i == 28)	{str_T3					= strTemp;	str_T3.Replace(_T("#"), _T("."));				d_T3 = _ttof(str_T3);}
			else if(i == 29)	{str_T4					= strTemp;	str_T4.Replace(_T("#"), _T("."));				d_T4 = _ttof(str_T4);}
			else if(i == 30)	{str_Ypos				= strTemp;	str_Ypos.Replace(_T("#"), _T("."));				dYpos = _ttof(str_Ypos);}

			i++;
		}
	}

	tempParam		+= 200;
	strMeasureFail  = tempParam;
	strMeasureFail.TrimRight();

	if (stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
		strMode = _T("CLIENT");
	else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
		strMode = _T("SERVER");

	if (stMonitorMeasureInfo.bModelSize == 0)
		strSize = _T("MIDDLE");
	else if(stMonitorMeasureInfo.bModelSize == 1)
		strSize = _T("LARGE");

	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s %s %s: %s, X1:%s, Y1:%s, X2:%s, Y2:%s, X3:%s, Y3:%s, X4:%s, Y4:%s"), 
		linspactEqpNo, _T("ABI"), strSize, strMode, strPanelID_1, str_X1, str_Y1, str_X2, str_Y2, str_X3, str_Y3, str_X4, str_Y4);
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s      :    OX1:%s, OY1:%s, OX2:%s, OY2:%s, OX3:%s, OY3:%s, OX4:%s, OY4:%s"), 
		linspactEqpNo, _T("ABI"), strOffSet_X1, strOffSet_Y1, strOffSet_X2, strOffSet_Y2, strOffSet_X3, strOffSet_Y3, strOffSet_X4, strOffSet_Y4);
	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Packet Vision][%d] %s      :    L1:%s, L2:%s, W1:%s, W2:%s, FL1:%s, FL2:%s, FW1:%s, FW2:%s, Ypos:%s"), 
		linspactEqpNo, _T("ABI"), str_L1, str_L2, str_W1, str_W2, str_FL1, str_FL2, str_FW1, str_FW2, str_Ypos);

	//임시 원래는 레시피에서 받아온다.
	stAlignMeasureInfo = pFrame->m_DataManager.m_fnReadAlignMeasureData(strPanelID_1);

	//if(m_bEqpSizeType > TRUE || m_bEqpSizeType < FALSE)
	//{
	//	G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("[Model Size Error] [%d] Check Vision Model"),m_bEqpSizeType );
	//	return FALSE;
	//}

	switch(stMonitorMeasureInfo.bModelSize)
	{
	case 0:		// 중형
		if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
		{
			//pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE,dLT_X,dLT_Y,dLB_X,dLB_Y,dRT_X,dRT_Y,dRB_X,dRB_Y,linspactEqpNo);
			if(linspactEqpNo == VISION_PC_2)
			{
				strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
					,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);

				strCFMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf,")
					,str_L1, str_L2, str_W1, str_W2, 90, 90, 90, 90);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureANG, CF_MEASURE_ANG, strMeasureFail, MEASUREFAIL_DEFAULT);
			}
			else if(linspactEqpNo == VISION_PC_6)
			{
				strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
					,d_X1, d_Y1, d_X2, d_Y2, d_X3, d_Y3, d_X4, d_Y4);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);

				strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
					,str_L1, str_L2, str_W1, str_W2, 90, 90, 90, 90);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT, strMeasureFail);
			}
		}
		else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
		{
			//ABI 중형 사이즈는 측정 데이터를 검사에서 계산(서버에서 계산 안함)	//2016.11.28 서버에서 계산
			if(linspactEqpNo == VISION_PC_2)
			{
				DSTX[0] = d_X1 + dOffSet_X1;
				DSTY[0] = d_Y1 + dOffSet_Y1;
				DSTX[1] = d_X2 + dOffSet_X2;
				DSTY[1] = d_Y2 + dOffSet_Y2;
				DSTX[2] = d_X3 + dOffSet_X3;
				DSTY[2] = d_Y3 + dOffSet_Y3;
				DSTX[3] = d_X4 + dOffSet_X4;
				DSTY[3] = d_Y4 + dOffSet_Y4;
				strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
					,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, strMeasureFail, MEASUREFAIL_DEFAULT);

				DSTL[0] = stMonitorMeasureInfo.dPanel_Y - (d_Y1 + d_Y2) + d_FL1;
				DSTL[1] = stMonitorMeasureInfo.dPanel_Y - (d_Y3 + d_Y4) + d_FL2;
				DSTW[0] = stMonitorMeasureInfo.dPanel_X - (d_X1 + d_X2) + d_FW1;
				DSTW[1] = stMonitorMeasureInfo.dPanel_X - (d_X3 + d_X4) + d_FW2;
				strCFMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf,")
					,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);
				if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DL1 - DSTL[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DL2 - DSTL[1]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DW1 - DSTW[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DW2 - DSTW[1]) < dSpecABI)
					strMeasureFail = MEASUREFAIL_CF;
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureANG, CF_MEASURE_ANG, strMeasureFail, MEASUREFAIL_DEFAULT);
			}
			else if(linspactEqpNo == VISION_PC_6)
			{
				DSTX[0] = d_X1 + dOffSet_X1;
				DSTY[0] = d_Y1 + dOffSet_Y1;
				DSTX[1] = d_X2 + dOffSet_X2;
				DSTY[1] = d_Y2 + dOffSet_Y2;
				DSTX[2] = d_X3 + dOffSet_X3;
				DSTY[2] = d_Y3 + dOffSet_Y3;
				DSTX[3] = d_X4 + dOffSet_X4;
				DSTY[3] = d_Y4 + dOffSet_Y4;
				strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
					,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, strMeasureFail);

				DSTL[0] = stMonitorMeasureInfo.dPanel_Y - (d_Y3 + d_Y4) + d_FL1;
				DSTL[1] = stMonitorMeasureInfo.dPanel_Y - (d_Y1 + d_Y2) + d_FL2;
				DSTW[0] = stMonitorMeasureInfo.dPanel_X - (d_X3 + d_X4) + d_FW1;
				DSTW[1] = stMonitorMeasureInfo.dPanel_X - (d_X1 + d_X2) + d_FW2;
				strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
					,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);
				if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) < dSpecABI || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DL3 - DSTL[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DL4 - DSTL[1]) < dSpecABI
					|| abs(stMonitorMeasureInfo.d_DW3 - DSTW[0]) < dSpecABI || abs(stMonitorMeasureInfo.d_DW4 - DSTW[1]) < dSpecABI)
					strMeasureFail = MEASUREFAIL_TFT;
				pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT, strMeasureFail);
			}
		}
		break;
	case 1:		// 대형
		if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_INSP)
		{
			//ABI 대형 사이즈는 측정 데이터를 서버에서 계산(검사에서 계산 안함)
			////대형일 경우 4자리 씩 데이터를 끊어 받는다.
			//if ( linspactEqpNo == VISION_PC_1 || linspactEqpNo == VISION_PC_2 )		//	CF		PC 1  /  PC 2
			//	if (stAlignMeasureInfo.bVision1Complete)		// Insert PC1 Data		
			//	{
			//		DSTX[0] = stAlignMeasureInfo.dCF_X1;			//PC1
			//		DSTY[0] = stAlignMeasureInfo.dCF_Y1;			//PC1
			//		DSTX[1] = d_X1;									//PC2
			//		DSTY[1] = d_Y1;									//PC2
			//		DSTX[2]	= stAlignMeasureInfo.dCF_X3;			//PC1
			//		DSTY[2]	= stAlignMeasureInfo.dCF_Y3;			//PC1
			//		DSTX[3]	= d_X2;									//PC2
			//		DSTY[3]	= d_Y2;									//PC2
			//		//계산된 값과 스펙 비교식 필요
			//		strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
			//			,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
			//		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, _T(""), MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입
			//	}
			//	else if(stAlignMeasureInfo.bVision2Complete)			// Insert PC2 Data
			//	{
			//		DSTX[0] = d_X1;									//PC1
			//		DSTY[0] = d_Y1;									//PC1
			//		DSTX[1] = stAlignMeasureInfo.dCF_X2;		//PC2
			//		DSTY[1] = stAlignMeasureInfo.dCF_Y2;		//PC2
			//		DSTX[2]	= d_X2;									//PC1
			//		DSTY[2]	= d_Y2;									//PC1
			//		DSTX[3] = stAlignMeasureInfo.dCF_X4;		//PC2
			//		DSTY[3] = stAlignMeasureInfo.dCF_Y4;		//PC2
			//		//계산된 값과 스펙 비교식 필요
			//		strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
			//			,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
			//		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, _T(""), MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입
			//	}
			//	else
			//	{
			//		if(linspactEqpNo == VISION_PC_1)
			//		{
			//			DSTX[0] = d_X1;
			//			DSTY[0] = d_Y1;
			//			DSTX[1] = d_X3;
			//			DSTY[1]	= d_Y3;
			//		}
			//		else if(linspactEqpNo == VISION_PC_2)
			//		{
			//			DSTX[0] = d_X2;
			//			DSTY[0] = d_Y2;
			//			DSTX[1] = d_X4;
			//			DSTY[1]	= d_Y4;
			//		}
			//		pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], linspactEqpNo);
			//	}

			//else if ( linspactEqpNo == VISION_PC_5 || linspactEqpNo == VISION_PC_6 )		//	TFT		PC 5  /  PC 6
			//	if (stAlignMeasureInfo.bVision5Complete)		// Insert PC5 Data		
			//	{
			//		DSTX[0] = stAlignMeasureInfo.dTFT_X1;			//PC5
			//		DSTY[0] = stAlignMeasureInfo.dTFT_Y1;			//PC5
			//		DSTX[1] = d_X1;									//PC6
			//		DSTY[1] = d_Y1;									//PC6
			//		DSTX[2]	= stAlignMeasureInfo.dTFT_X3;		//PC5
			//		DSTY[2]	= stAlignMeasureInfo.dTFT_Y3;		//PC5
			//		DSTX[3]	= d_X2;									//PC6
			//		DSTY[3]	= d_Y2;									//PC6
			//		//계산된 값과 스펙 비교식 필요
			//		strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
			//			,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
			//		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, _T(""));	//계산된 값과 스펙 비교후 알람 여부 기입
			//	}
			//	else if(stAlignMeasureInfo.bVision6Complete) // Insert PC6 Data
			//	{
			//		DSTX[0] = d_X1;									//PC5
			//		DSTY[0] = d_Y1;									//PC5
			//		DSTX[1] = stAlignMeasureInfo.dTFT_X2;		//PC6
			//		DSTY[1] = stAlignMeasureInfo.dTFT_Y2;		//PC6
			//		DSTX[2]	= d_X2;									//PC5
			//		DSTY[2]	= d_Y2;									//PC5
			//		DSTX[3] = stAlignMeasureInfo.dTFT_X4;		//PC6
			//		DSTY[3] = stAlignMeasureInfo.dTFT_Y4;		//PC6
			//		//계산된 값과 스펙 비교식 필요
			//		strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
			//			,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
			//		pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, _T(""));	//계산된 값과 스펙 비교후 알람 여부 기입
			//	}
			//	else
			//	{
			//		if(linspactEqpNo == VISION_PC_5)
			//		{
			//			DSTX[0] = d_X1;
			//			DSTY[0] = d_Y1;
			//			DSTX[1] = d_X3;
			//			DSTY[1]	= d_Y3;
			//		}
			//		else if(linspactEqpNo == VISION_PC_6)
			//		{
			//			DSTX[0] = d_X2;
			//			DSTY[0] = d_Y2;
			//			DSTX[1] = d_X4;
			//			DSTY[1]	= d_Y4;
			//		}
			//		pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], linspactEqpNo);
			//	}
		}
		else if(stMonitorMeasureInfo.bCalMode == MEASURE_CAL_SERVER)
		{
			if ( linspactEqpNo == VISION_PC_1 || linspactEqpNo == VISION_PC_2 )		//	CF		PC 1  /  PC 2
				//if (stAlignMeasureInfo.bVision2Complete)		// Insert PC1 Data	//포맷 위치 통일
				if (stAlignMeasureInfo.bVision2Complete && linspactEqpNo == VISION_PC_2)		// Insert PC1 Data	//포맷 위치 통일		
				{
					DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dCF_X1);	//PC1
					DSTY[0] = ValueLimitChk1(stAlignMeasureInfo.dCF_Y1);	//PC1
					DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dCF_X2);	//PC1
					DSTY[1] = ValueLimitChk1(stAlignMeasureInfo.dCF_Y2);	//PC1
					DSTX[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DX3 - (d_X3 + dOffSet_X3));			//PC2
					DSTY[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DY3 - (d_Y3 + dOffSet_Y3));			//PC2
					DSTX[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DX4 - (d_X4 + dOffSet_X4));			//PC2
					DSTY[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DY4 - (d_Y4 + dOffSet_Y4));			//PC2

					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, _T(""), MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입

					/*DSTL[0] = stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);
					DSTL[1] = stAlignMeasureInfo.dCF_L2;
					DSTW[0] = stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW2);
					DSTW[1] = stAlignMeasureInfo.dCF_W2;*/
					//DSTL[0] = stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1);
					DSTL[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1));  //L1

					//2017.01.06
					DSTW[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2 + dOffSet_Y2) + (d_Y4+ dOffSet_Y4)) + d_FW1));
					//DSTW[0] = stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((pFrame->m_dPosion_Y[CF][1] + pFrame->m_dPosion_OffsetY[CF][1]) + (d_Y4+ dOffSet_Y4)) + d_FW1);   //W1
					//

					//2017.01.06
					//DSTL[0] = stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);
					DSTL[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2));
					//DSTL[1] = stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((pFrame->m_dPosion_X[CF][0]+ pFrame->m_dPosion_OffsetX[CF][0]) + (pFrame->m_dPosion_X[CF][1]+ pFrame->m_dPosion_OffsetX[CF][1])) + d_FL2);	//L2
					//

					//2017.01.06
					//DSTW[0] = stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW2);
					DSTW[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW2));
					//DSTW[1] = stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((pFrame->m_dPosion_Y[CF][0]+ pFrame->m_dPosion_OffsetY[CF][0]) + (d_Y3+ dOffSet_Y3)) + d_FW2);	//W2
					//

					pFrame->m_DataManager.m_fnWriteMonitorMeasureData_temp(strPanelID_1,TRUE, DSTL[0], DSTW[0],DSTL[1], DSTW[1], VISION_PC_1); //포맷 위치 통일

					strCFMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf,")
						,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);

					//17.06.01 Lyh 측정 부등호 방향 변환.
					if(abs(DSTX[0]) > dSpecABI || abs(DSTY[0]) > dSpecABI
					|| abs(DSTX[1]) > dSpecABI || abs(DSTY[1]) > dSpecABI
					|| abs(DSTX[2]) > dSpecABI || abs(DSTY[2]) > dSpecABI
					|| abs(DSTX[3]) > dSpecABI || abs(DSTY[3]) > dSpecABI
					  )
					{
						strMeasureFail = MEASUREFAIL_CF;
					}

					//임시 삭제 Lyh. 170606
					/*if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DL1 - DSTL[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DL2 - DSTL[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DW1 - DSTW[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DW2 - DSTW[1]) > dSpecABI
						)*/
					
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureANG, CF_MEASURE_ANG, strMeasureFail, MEASUREFAIL_DEFAULT);
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision2Complete end"));
				}
				//else if(stAlignMeasureInfo.bVision1Complete)	// Insert PC2 Data	//포맷 위치 통일
				else if(stAlignMeasureInfo.bVision1Complete && linspactEqpNo == VISION_PC_1)	// Insert PC2 Data	//포맷 위치 통일 //2017.07.17 조건문 추가add by mby
				{
					DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX1 - (d_X1 + dOffSet_X1));			//PC1
					DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY1 - (d_Y1 + dOffSet_Y1));			//PC1
					DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX2 - (d_X2 + dOffSet_X2));			//PC1
					DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY2 - (d_Y2 + dOffSet_Y2));			//PC1
					DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X3);	//PC2
					DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y3);	//PC2
					DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_X4);	//PC2
					DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dCF_Y4);	//PC2

					//계산된 값과 스펙 비교식 필요
					strCFMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureDST, CF_MEASURE_DST, _T(""), MEASUREFAIL_DEFAULT);	//계산된 값과 스펙 비교후 알람 여부 기입

					/*DSTL[0] = stAlignMeasureInfo.dCF_L1;
					DSTL[1] = stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1);
					DSTW[0] = stAlignMeasureInfo.dCF_W1;
					DSTW[1] = stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2 + dOffSet_Y2) + (d_Y4+ dOffSet_Y4)) + d_FW1);*/

					//DSTL[0] = stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1);
					DSTL[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((pFrame->m_dPosion_X[CF][2]+ pFrame->m_dPosion_OffsetX[CF][2]) + (pFrame->m_dPosion_X[CF][3]+ pFrame->m_dPosion_OffsetY[CF][3])) + d_FL1));  //L1
					//DSTW[0] = stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2 + dOffSet_Y2) + (d_Y4+ dOffSet_Y4)) + d_FW1);
					DSTW[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2 + dOffSet_Y2) + (pFrame->m_dPosion_Y[CF][3]+ pFrame->m_dPosion_OffsetY[CF][3])) + d_FW1));   //W1
					//DSTL[0] = stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);
					DSTL[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2));	//L2
					//DSTW[0] = stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW2);
					DSTW[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (pFrame->m_dPosion_Y[CF][2]+ pFrame->m_dPosion_OffsetY[CF][2])) + d_FW2));	//W2

					pFrame->m_DataManager.m_fnWriteMonitorMeasureData_temp(strPanelID_1,TRUE, DSTL[0], DSTW[0],DSTL[1], DSTW[1], VISION_PC_2); //포맷 위치 통일

					strCFMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf,")
						,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);

					//17.06.01 Lyh 측정 부등호 방향 변환.
					if(abs(DSTX[0]) > dSpecABI || abs(DSTY[0]) > dSpecABI
						|| abs(DSTX[1]) > dSpecABI || abs(DSTY[1]) > dSpecABI
						|| abs(DSTX[2]) > dSpecABI || abs(DSTY[2]) > dSpecABI
						|| abs(DSTX[3]) > dSpecABI || abs(DSTY[3]) > dSpecABI
					  )
					{
						strMeasureFail = MEASUREFAIL_CF;
					}

					//임시 삭제 Lyh. 170606
					/*if(abs(stMonitorMeasureInfo.d_DX1 - DSTX[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY1 - DSTY[0]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX2 - DSTX[1]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY2 - DSTY[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX3 - DSTX[2]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY3 - DSTY[2]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX4 - DSTX[3]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY4 - DSTY[3]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DL1 - DSTL[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DL2 - DSTL[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DW1 - DSTW[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DW2 - DSTW[1]) > dSpecABI
						)*/

					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strCFMeasureANG, CF_MEASURE_ANG, strMeasureFail, MEASUREFAIL_DEFAULT);
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision1Complete end"));
				}
				else
				{
					if(linspactEqpNo == VISION_PC_1)
					{
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX1 - (d_X1 + dOffSet_X1));				//2018.02.01 KYH01 99999 리미트 처리 이곳 루틴 타는지 확인 필요 (Measure Data)
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY1 - (d_Y1 + dOffSet_Y1));
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX2 - (d_X2 + dOffSet_X2));
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY2 - (d_Y2 + dOffSet_Y2));
						//position
						pFrame->m_dPosion_X[CF][0] = d_X1;
						pFrame->m_dPosion_Y[CF][0] = d_Y1;
						pFrame->m_dPosion_X[CF][1] = d_X2;
						pFrame->m_dPosion_Y[CF][1] = d_Y2;
						//offset
						pFrame->m_dPosion_OffsetX[CF][0] = dOffSet_X1;
						pFrame->m_dPosion_OffsetY[CF][0] = dOffSet_Y1;
						pFrame->m_dPosion_OffsetX[CF][1] = dOffSet_X2;
						pFrame->m_dPosion_OffsetY[CF][1] = dOffSet_Y2;

						//DSTL[0] = stMonitorMeasureInfo.d_DL1 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1);
						//DSTW[0] = stMonitorMeasureInfo.d_DW1 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2 + dOffSet_Y2) + (d_Y4+ dOffSet_Y4)) + d_FW1);						
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], DSTL[0], DSTW[0], VISION_PC_2); //포맷 위치 통일
					}
					else if(linspactEqpNo == VISION_PC_2)
					{
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX3 - (d_X3 + dOffSet_X3));			//2018.02.01 KYH01 99999 리미트 처리 이곳 루틴 타는지 확인 필요 (Measure Data)
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY3 - (d_Y3 + dOffSet_Y3));
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX4 - (d_X4 + dOffSet_X4));
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY4 - (d_Y4 + dOffSet_Y4));
						//position
						pFrame->m_dPosion_X[CF][2] = d_X3;
						pFrame->m_dPosion_Y[CF][2] = d_Y3;
						pFrame->m_dPosion_X[CF][3] = d_X4;
						pFrame->m_dPosion_Y[CF][3] = d_Y4;
						//offset
						pFrame->m_dPosion_OffsetX[CF][2] = dOffSet_X3;
						pFrame->m_dPosion_OffsetY[CF][2] = dOffSet_Y3;
						pFrame->m_dPosion_OffsetX[CF][3] = dOffSet_X4;
						pFrame->m_dPosion_OffsetY[CF][3] = dOffSet_Y4;
						//DSTL[0] = stMonitorMeasureInfo.d_DL2 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);
						//DSTW[0] = stMonitorMeasureInfo.d_DW2 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1+ dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW2);
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], DSTL[0], DSTW[0], VISION_PC_1);	//포맷 위치 통일
					}
				}

			else if ( linspactEqpNo == VISION_PC_5 || linspactEqpNo == VISION_PC_6 )		//	TFT		PC 5  /  PC 6
				//if (stAlignMeasureInfo.bVision6Complete)		// Insert PC5 Data	//포맷 위치 통일	
				if (stAlignMeasureInfo.bVision6Complete && linspactEqpNo == VISION_PC_6 )		// Insert PC5 Data	//포맷 위치 통일	//2017.07.17 조건문 추가add by mby
				{
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision5Complete start"));
					DSTX[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X1);	//PC5
					DSTY[0] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y1);	//PC5
					DSTX[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_X2);	//PC5
					DSTY[1] = ValueLimitChk1(stAlignMeasureInfo.dTFT_Y2);	//PC5
					DSTX[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DX7 - (d_X3 + dOffSet_X3));			//PC6
					DSTY[2]	= ValueLimitChk1(stMonitorMeasureInfo.d_DY7 - (d_Y3 + dOffSet_Y3));			//PC6
					DSTX[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DX8 - (d_X4 + dOffSet_X4));			//PC6
					DSTY[3] = ValueLimitChk1(stMonitorMeasureInfo.d_DY8 - (d_Y4 + dOffSet_Y4));			//PC6

					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);
					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, _T(""));	//계산된 값과 스펙 비교후 알람 여부 기입

					/*DSTL[0] = stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);
					DSTL[1] = stAlignMeasureInfo.dTFT_L4;
					DSTW[0] = stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (d_Y4+ dOffSet_X4)) + d_FW2);
					DSTW[1] = stAlignMeasureInfo.dTFT_W4;*/

					//DSTL[0] = stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1); //L3
					DSTL[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1)); //L3

					//2017.01.06
					DSTW[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1 + dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW1)); //W3
					//DSTW[0] = stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((pFrame->m_dPosion_Y[TFT][0] + pFrame->m_dPosion_OffsetY[TFT][0]) + (d_Y3+ dOffSet_Y3)) + d_FW1); //W3
					//

					//
					DSTL[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2));//L4
					//DSTL[0] = stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);//L4
					//DSTL[1] = stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((pFrame->m_dPosion_X[TFT][0]+ pFrame->m_dPosion_OffsetX[TFT][0]) + (pFrame->m_dPosion_X[TFT][1]+ pFrame->m_dPosion_OffsetX[TFT][1])) + d_FL2);//L4

					//
					DSTW[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (d_Y4+ dOffSet_X4)) + d_FW2));//W4
					//DSTW[0] = stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (d_Y4+ dOffSet_X4)) + d_FW2);//W4
					//DSTW[1] = stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((pFrame->m_dPosion_Y[TFT][1]+ pFrame->m_dPosion_OffsetY[TFT][1]) + (d_Y4+ dOffSet_X4)) + d_FW2);//W4
					//


					pFrame->m_DataManager.m_fnWriteMonitorMeasureData_temp(strPanelID_1,TRUE, DSTL[0], DSTW[0], DSTL[1], DSTW[1], VISION_PC_5); //포맷 위치 통일

					strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
						,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);

					//17.06.01 Lyh 측정 부등호 방향 변환.
					if(abs(DSTX[0]) > dSpecABI || abs(DSTY[0]) > dSpecABI
						|| abs(DSTX[1]) > dSpecABI || abs(DSTY[1]) > dSpecABI
						|| abs(DSTX[2]) > dSpecABI || abs(DSTY[2]) > dSpecABI
						|| abs(DSTX[3]) > dSpecABI || abs(DSTY[3]) > dSpecABI
					)
					{
						strMeasureFail = MEASUREFAIL_TFT;
					}

					/*if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DL3 - DSTL[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DL4 - DSTL[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DW3 - DSTW[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DW4 - DSTW[1]) > dSpecABI)*/

					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT,strMeasureFail);
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision6Complete end"));
				}
				//else if(stAlignMeasureInfo.bVision5Complete) // Insert PC6 Data //포맷 위치 통일
				else if(stAlignMeasureInfo.bVision5Complete && linspactEqpNo == VISION_PC_5) // Insert PC6 Data //포맷 위치 통일	//2017.07.17 조건문 추가add by mby
				{
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision5Complete start"));
					DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX5 - (d_X1 + dOffSet_X1));			//PC5
					DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY5 - (d_Y1 + dOffSet_Y1));			//PC5
					DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX6 - (d_X2 + dOffSet_X2));			//PC5
					DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY6 - (d_Y2 + dOffSet_Y2));			//PC5
					DSTX[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X3);	//PC6
					DSTY[2]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y3);	//PC6
					DSTX[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_X4);	//PC6
					DSTY[3]	= ValueLimitChk1(stAlignMeasureInfo.dTFT_Y4);	//PC6

					//계산된 값과 스펙 비교식 필요
					strTFTMeasureDST.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf,%.0lf")
						,DSTX[0], DSTY[0],DSTX[1] ,DSTY[1] ,DSTX[2] ,DSTY[2],DSTX[3], DSTY[3]);

					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureDST, TFT_MEASURE_DST, MEASUREFAIL_DEFAULT, _T(""));	//계산된 값과 스펙 비교후 알람 여부 기입

					/*DSTL[0] = stAlignMeasureInfo.dTFT_L3;
					DSTL[1] = stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1);
					DSTW[0] = stAlignMeasureInfo.dTFT_W3;
					DSTW[1] = stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1 + dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW1);*/

					//DSTL[0] = stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1); //L3
					DSTL[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((pFrame->m_dPosion_X[TFT][2]+ pFrame->m_dPosion_OffsetX[TFT][2]) + (pFrame->m_dPosion_X[TFT][3]+ pFrame->m_dPosion_OffsetX[TFT][3])) + d_FL1)); //L3
					//DSTW[0] = stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1 + dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW1); //W3
					DSTW[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1 + dOffSet_Y1) + (pFrame->m_dPosion_Y[TFT][2]+ pFrame->m_dPosion_OffsetY[TFT][2])) + d_FW1)); //W3
					//DSTL[0] = stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);//L4
					DSTL[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2));//L4
					//DSTW[0] = stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (d_Y4+ dOffSet_X4)) + d_FW2);//W4
					DSTW[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (pFrame->m_dPosion_Y[TFT][4]+ pFrame->m_dPosion_OffsetY[TFT][3])) + d_FW2));//W4

					pFrame->m_DataManager.m_fnWriteMonitorMeasureData_temp(strPanelID_1,TRUE, DSTL[0], DSTW[0], DSTL[1], DSTW[1], VISION_PC_6); //포맷 위치 통일

					strTFTMeasureANG.Format(_T("%.0lf,%.0lf,%.0lf,%.0lf,%.2lf,%.2lf,%.2lf,%.2lf")
						,DSTL[0], DSTL[1], DSTW[0], DSTW[1], d_T1, d_T2, d_T3, d_T4);

					//17.06.01 Lyh 측정 부등호 방향 변환.
					if(abs(DSTX[0]) > dSpecABI || abs(DSTY[0]) > dSpecABI
						|| abs(DSTX[1]) > dSpecABI || abs(DSTY[1]) > dSpecABI
						|| abs(DSTX[2]) > dSpecABI || abs(DSTY[2]) > dSpecABI
						|| abs(DSTX[3]) > dSpecABI || abs(DSTY[3]) > dSpecABI
						)
					{
						strMeasureFail = MEASUREFAIL_TFT;
					}

					/*if(abs(stMonitorMeasureInfo.d_DX5 - DSTX[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY5 - DSTY[0]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX6 - DSTX[1]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY6 - DSTY[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX7 - DSTX[2]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY7 - DSTY[2]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DX8 - DSTX[3]) > dSpecABI || abs(stMonitorMeasureInfo.d_DY8 - DSTY[3]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DL3 - DSTL[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DL4 - DSTL[1]) > dSpecABI
						|| abs(stMonitorMeasureInfo.d_DW3 - DSTW[0]) > dSpecABI || abs(stMonitorMeasureInfo.d_DW4 - DSTW[1]) > dSpecABI)*/


					pFrame->m_DataManager.m_fnWriteAlignMeasureData(strPanelID_1, strTFTMeasureANG, TFT_MEASURE_ANG, MEASUREFAIL_DEFAULT, strMeasureFail);		
					G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("bVision5Complete end"));
				}
				else
				{
					if(linspactEqpNo == VISION_PC_5)
					{
						G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("linspactEqpNo == VISION_PC_5"));
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX5 - (d_X1 + dOffSet_X1));	//X5				//2018.02.01 KYH01 99999 리미트 처리 이곳 루틴 타는지 확인 필요 (Measure Data)
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY5 - (d_Y1 + dOffSet_Y1));	//Y5
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX6 - (d_X2 + dOffSet_X2));	//X6
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY6 - (d_Y2 + dOffSet_Y2));	//Y6
						//position
						pFrame->m_dPosion_X[TFT][0] = d_X1;
						pFrame->m_dPosion_Y[TFT][0] = d_Y1;
						pFrame->m_dPosion_X[TFT][1] = d_X2;
						pFrame->m_dPosion_Y[TFT][1] = d_Y2;
						//offset
						pFrame->m_dPosion_OffsetX[TFT][0] = dOffSet_X1;
						pFrame->m_dPosion_OffsetY[TFT][0] = dOffSet_Y1;
						pFrame->m_dPosion_OffsetX[TFT][1] = dOffSet_X2;
						pFrame->m_dPosion_OffsetY[TFT][1] = dOffSet_Y2;
						//DSTL[0] = stMonitorMeasureInfo.d_DL3 - (stMonitorMeasureInfo.dPanel_X - ((d_X3+ dOffSet_X3) + (d_X4+ dOffSet_X4)) + d_FL1); //L3
						//DSTW[0] = stMonitorMeasureInfo.d_DW3 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y1 + dOffSet_Y1) + (d_Y3+ dOffSet_Y3)) + d_FW1); //W3
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], DSTL[0], DSTW[0], VISION_PC_6); //포맷 위치 통일
					}
					else if(linspactEqpNo == VISION_PC_6)
					{
						G_AddLog_SEQ(PRE_ALIGN_PC - 1, _T("linspactEqpNo == VISION_PC_6"));
						DSTX[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DX7 - (d_X3 + dOffSet_X3));	//X7				//2018.02.01 KYH01 99999 리미트 처리 이곳 루틴 타는지 확인 필요 (Measure Data)
						DSTY[0] = ValueLimitChk1(stMonitorMeasureInfo.d_DY7 - (d_Y3 + dOffSet_Y3));	//Y7	
						DSTX[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DX8 - (d_X4 + dOffSet_X4));	//X8
						DSTY[1] = ValueLimitChk1(stMonitorMeasureInfo.d_DY8 - (d_Y4 + dOffSet_Y4));	//Y8
						//position
						pFrame->m_dPosion_X[TFT][2] = d_X3;
						pFrame->m_dPosion_Y[TFT][2] = d_Y3;
						pFrame->m_dPosion_X[TFT][3] = d_X4;
						pFrame->m_dPosion_Y[TFT][3] = d_Y4;
						//offset
						pFrame->m_dPosion_OffsetX[TFT][2] = dOffSet_X3;
						pFrame->m_dPosion_OffsetY[TFT][2] = dOffSet_Y3;
						pFrame->m_dPosion_OffsetX[TFT][3] = dOffSet_X4;
						pFrame->m_dPosion_OffsetY[TFT][3] = dOffSet_Y4;
						//DSTL[0] = stMonitorMeasureInfo.d_DL4 - (stMonitorMeasureInfo.dPanel_X - ((d_X1+ dOffSet_X1) + (d_X2+ dOffSet_X2)) + d_FL2);//L4
						//DSTW[0] = stMonitorMeasureInfo.d_DW4 - (stMonitorMeasureInfo.dPanel_Y - ((d_Y2+ dOffSet_Y2) + (d_Y4+ dOffSet_X4)) + d_FW2);//W4
						pFrame->m_DataManager.m_fnWriteMonitorMeasureData(strPanelID_1,TRUE, DSTX[0], DSTY[0], DSTX[1], DSTY[1], DSTL[0], DSTW[0], VISION_PC_5); //포맷 위치 통일
					}
				}
		}

		break;
	}

	LeaveCriticalSection(&m_CriticalSectione[E_INSP_MONITOR_MEASURE_INFO]);
	return nRet;
}

int CServerWorkManager::Seq_Insp_AutoRecipe(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_AUTO_RECIPE]);
	int			    nRet			        = APP_OK;
	byte*		    tempParam			= pParam;
	ULONG		linspactEqpNo	= 0;
	BOOL        bRecive            = FALSE;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	linspactEqpNo	= *(ULONG*)tempParam;     tempParam += sizeof(ULONG);	
	bRecive			    = *(BOOL*)tempParam;	    tempParam += sizeof(BOOL);	

	pFrame->m_bAutoRecipe[linspactEqpNo-1] = bRecive;
	
	LeaveCriticalSection(&m_CriticalSectione[E_INSP_AUTO_RECIPE]);
	return nRet;
}

void CServerWorkManager::m_fnResponseSystemTime(int nVisionID)
{
	char  cParam[200]			= {0,};
	char* pTempParam			= cParam;
	int			nRet			= APP_OK;
	BSTR UniSystemTime		    = NULL;
	CString MultiByteStr		=_T("");
	SYSTEMTIME systime;
	CString strSystemTime       = _T("");

	::GetLocalTime(&systime);
	strSystemTime.Format(_T("%4d-%02d-%02d#%02d:%02d:%02d"),systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);

	if (G_pVsServerIF == NULL || m_pMainFrame == NULL)
		return ;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;
	UniSystemTime = strSystemTime.AllocSysString();
	strcpy(pTempParam,(CStringA) UniSystemTime);
	pTempParam				+= 40;

	if(nVisionID >= 11 && nVisionID <= 20)	// Align Device는 11번부터...(10개정도 여유를 둠)	// 2016.03.17 LJK
	{
		//2021.06.08 add by mby 
		if(!pFrame->m_DataManager.m_stDefCoordDatRcp.nAlignPCNo){
			nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK + (nVisionID - 11), VS_PLC_SEQUENCE_SET_SYSTEMTIME, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
		}else{
			nRet = G_pVsServerIF->m_fnSendCommand(VS_ALIGN_TASK_TWO + (nVisionID - 11), VS_PLC_SEQUENCE_SET_SYSTEMTIME, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
		}
	}
	else
	{
		nRet = G_pVsServerIF->m_fnSendCommand(VS_CLIENT_TASK + (nVisionID - 1), VS_PLC_SEQUENCE_SET_SYSTEMTIME, 0, (USHORT)(pTempParam - cParam), (byte*)cParam, 0, 0, 0, CMD_TYPE_NORES);
	}

	if (nRet != APP_OK)
	{
		// Write Error Log
	}
}

//2016.08.08
int CServerWorkManager::Seq_Insp_Ch_Luminance(byte* pParam, ULONG& nPrmSize)
{
	int			nRet			= APP_OK;
	byte*		tempParam		= pParam;
	ULONG		linspactEqpNo	= 0;

	int			nController		= 0;
	WORD		wChannel, wSetValue, wCurrentValue;
	CString		strMessage		= _T("");
	CString		strPanelID = _T(""), strPanelID_1 = _T(""), strPanelID_2 = _T("");

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	linspactEqpNo	= *(ULONG*)tempParam;	tempParam		+= sizeof(ULONG);
	nController		= *(BYTE*)tempParam;	tempParam		+= sizeof(BYTE);
	//wController 	= *(WORD*)tempParam;	tempParam		+= sizeof(WORD);
	wChannel 		= *(WORD*)tempParam;	tempParam		+= sizeof(WORD);
	wSetValue 		= *(WORD*)tempParam;	tempParam		+= sizeof(WORD);
	wCurrentValue 	= *(WORD*)tempParam;	tempParam		+= sizeof(WORD);
	strPanelID_1	= tempParam;			tempParam		+= 40;
	strPanelID_2	= tempParam;

	strPanelID_1.TrimRight();
	strPanelID_2.TrimRight();
	strPanelID.Format(_T("%s,%s"),strPanelID_1, strPanelID_2);

	G_AddLog_STG_SEQ(3, linspactEqpNo - 1,  _T("[Packet Vision][%d] %s : Controller : %d, Channel : %d, SetValue : %d, CurrentValue : %d"), 
		linspactEqpNo, VTM_LUMINANCE_CHANNEL, nController, wChannel, wSetValue, wCurrentValue);

	strMessage.Format(_T("[PC%d] Controller->%d Channel->%d SetValue->%d CurrentValue->%d"),
		linspactEqpNo, nController, wChannel, wSetValue, wCurrentValue);
	pFrame->m_DataManager.m_fnLuminanceAlarmSet(_T("Luminance Alarm"),strMessage, strPanelID);

	return APP_OK;
}

//2017.02.03 add by mby
double CServerWorkManager::ValueLimitChk(double value)
{
	if(value > 999.9){
		value = 999.0;
	}else if(value < -999.9){
		value = -999.0;
	}
	return value;
}
double CServerWorkManager::ValueLimitChk1(double value)
{
	if(value > 99998){
		value = 99998;
	}else if(value < -99998){
		value = -99998;
	}
	return value;
}

//2017.02.08
int CServerWorkManager::Seq_Insp_LightController(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);

	int			nRet				= APP_OK;		
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	WORD		wControllerNo, wErrorFlag;
	CString		strDisplayMsg = _T(""), strErrorMsg = _T("");

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	linspactEqpNo	= *(ULONG*)tempParam;	tempParam		+= sizeof(ULONG);
	wControllerNo 	= *(WORD*)tempParam;	tempParam		+= sizeof(WORD);
	wErrorFlag		= *(WORD*)tempParam;

	//Error Flag 
	//0 : Fan Error  
	//1 : Temperature Error
	//2 : CM Error

	if(wErrorFlag == 0)			strErrorMsg = _T("Fan Error");
	else if(wErrorFlag == 1)	strErrorMsg = _T("Temperature Error");
	else if(wErrorFlag == 2)	strErrorMsg = _T("CM Error");

	strDisplayMsg.Format(_T("PC:%d Controller:%d Error:%s"),
		linspactEqpNo, wControllerNo, strErrorMsg);
	pFrame->m_DataManager.m_fnDisplayMSGSet(_T("Light Controller Alarm"), strDisplayMsg);

	G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] Controller No[%d] %s : %s"),
		linspactEqpNo, wControllerNo, _T("Light Controller Alarm"), strErrorMsg);

	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);

	return nRet;
}

//2017.02.28
int CServerWorkManager::Seq_Insp_CheckFile(byte* pParam, ULONG& nPrmSize)
{
	EnterCriticalSection(&m_CriticalSectione[E_INSP_STATE]);

	int			nRet				= APP_OK;		
	byte*		tempParam			= pParam;
	ULONG		linspactEqpNo		= 0;
	CString		strDisplayMsg = _T(""), strPanelID;

	CMainFrame* pFrame = (CMainFrame*)m_pMainFrame;

	linspactEqpNo	= *(ULONG*)tempParam;	tempParam   += sizeof(ULONG);
	strPanelID	= tempParam;

	strDisplayMsg.Format(_T("PC:%d PanelID:%s"),
		linspactEqpNo, strPanelID);
	pFrame->m_DataManager.m_fnDisplayMSGSet(_T("File Alarm"), strDisplayMsg);

	G_AddLog_SEQ(linspactEqpNo - 1, _T("[Packet Vision][%d] %s PanelID : %s"),
		linspactEqpNo, _T("File Alarm"), strPanelID);

	LeaveCriticalSection(&m_CriticalSectione[E_INSP_STATE]);

	return nRet;
}
