#pragma once
#include "Thread.h"
#pragma once

#include "Thread.h"

typedef enum e_Critical
{
	E_INSP_GRAB_SEQ = 0,
	E_INSP_CLASSIFY,
	E_INSP_STATE,
	E_PG_DATA,
	E_ALIGN_DATA,
	E_SET_SYSTIME,
	E_SET_TEMPERATURE,
	E_INSP_AUTO_RECIPE,
	E_INSP_MONITOR_MEASURE_INFO,
	E_CRITICAL_MAX
};
#define VS_PLC_TASK							7
#define VS_CLIENT_TASK						50
#define VS_ALIGN_TASK						30
#define VS_ALIGN_TASK_TWO					31

#define VS_CRUX_CLIENT_TASK_INI_FILE		_T("D:\\Crux_Data\\INI\\VSCruxClient.ini")		// For VS Interface
#define VS_PLC_SEQUENCE_SET_ALIGN_CHECK		10, 30
#define VS_PLC_SEQUENCE_SET_ALIGN_REQ 		10, 31
#define VS_PLC_SEQUENCE_SET_ALIGN_RESET		10, 32	//2016.11.05	Mr.Chu
#define VS_PLC_SEQUENCE_SET_SYSTEMTIME 		10, 60	//2016.03.14
#define VS_PLC_SEQUENCE_SET_GRAB_READY 		10, 1
#define VS_CLIENT_ALIVE_CHECK				10, 11
#define VS_CRUX_CLIENT_VISION_RECIPE_REQ	10, 50
#define VS_CLIENT_SELECT_RECIPE_SET			10, 51
#define VS_CLIENT_INITIAL					10, 70
#define VS_CLIENT_MONITOR_INFO_REQ			10, 80		//2017.02.18 add by mby

#define VS_PLC_SEQUENCE_SET_GRAB_ACK		20, 11
#define VS_PLC_SEQUENCE_SET_GRAB_END		20, 12


#define SEQUENCE_IS_NOTHING					1500
#define SEQUENCE_IS_BUSY					1501
#define SEQUENCE_IS_NOT_BUSY				1502
#define SEQUENCE_IS_NOT_EXECUTABLE			1505		

enum {
	LIGHT_CONTROL_1 = 0,
	LIGHT_CONTROL_2,
	LIGHT_CONTROL_3,
	LIGHT_CONTROL_MAX
};

class CInterServerInterface;
class CMainFrame;
extern CInterServerInterface*	G_pVsServerIF;
class CServerWorkManager
	:protected CThread
{
public:
	CServerWorkManager(CMainFrame* pMainFrame);
	~CServerWorkManager();

protected:
	virtual bool GetThreadRunFlag()
	{
		return false;
	}
	virtual BYTE* m_fnPeekMessage(){return 0;}
	

private:
	CRITICAL_SECTION		m_CriticalSectione[E_CRITICAL_MAX];		
	CInterServerInterface*	m_pInterServerInterface;
	CMainFrame*				m_pMainFrame;
	bool					m_bThreadRun;
	UINT					m_SeqenceCount;
	HANDLE					m_hSequenceResetEvent;
	HANDLE					m_handle[20]; 
	bool					m_bSeqResetFlag;
	void m_fnRunThread();
	int m_fnAnalyzeMsg(void* pRcvMsg);
	int m_fnResponseSend(USHORT ReturnVal, void* pRcvMsg);
	

	/************************************************************************/
	/*						Sequence Function                               */
	/************************************************************************/

	int Seq_PgResult				(byte* pParam, ULONG& nPrmSize);
	int Seq_SetGrabAck				(byte* pParam, ULONG& nPrmSize);
	int Seq_SetGrabEnd				(byte* pParam, ULONG& nPrmSize);
	int Seq_ClassifyEnd				(byte* pParam, ULONG& nPrmSize);
	int Seq_AlignRes				(byte* pParam, ULONG& nPrmSize);
	int Seq_AlignCfMeasuredstRes	(byte* pParam, ULONG& nPrmSize);
	int Seq_AlignCfMeasureAngRes	(byte* pParam, ULONG& nPrmSize);
	int Seq_AlignTftMeasuredstRes	(byte* pParam, ULONG& nPrmSize);
	int Seq_AlignTftMeasureRes		(byte* pParam, ULONG& nPrmSize);
	int Seq_Insp_JudgeEnd			(byte* pParam, ULONG& nPrmSize);
	int Seq_Insp_Interlock			(byte* pParam, ULONG& nPrmSize);
	int Seq_Insp_Luminance			(byte* pParam, ULONG& nPrmSize);
	int Seq_Insp_RecipeSet			(byte* pParam, ULONG& nPrmSize);
	int Seq_Insp_temperature		(byte* pParam, ULONG& nPrmSize); //2016.04.05 add by ikm - 온도값
	int Seq_Insp_Ch_Luminance		(byte* pParam, ULONG& nPrmSize); //2016.08.08 
	int Seq_Insp_State				(byte* pParam, ULONG& nPrmSize);
	int Seq_SetSystemTime			(byte* pParam, ULONG& nPrmSize);	//2016.03.14
	int Seq_Insp_AutoRecipe (byte* pParam, ULONG&	nPrmSize);
	int Seq_Insp_MonitorMeasureDataAVL (byte* pParam, ULONG& nPrmSize);		//2016.08.09
	int Seq_Insp_MonitorMeasureDataABI (byte* pParam, ULONG& nPrmSize);		//2016.08.09
	int Seq_Insp_MonitorInfo	(byte* pParam, ULONG& nPrmSize);		//2016.08.09
	int Seq_Insp_LightController	(byte* pParam, ULONG& nPrmSize);	//2017.02.08
	int Seq_Insp_CheckFile			(byte* pParam, ULONG& nPrmSize);	//2017.02.28
	int Seq_Insp_OriginCoord		(byte* pParam, ULONG& nPrmSize);	//2017.07.10
	int Seq_Alignlive				(byte* pParam, ULONG& nPrmSize);	//2018.03.24 add by mby
	
	static void m_fnClassifyProcInfo		(LPVOID pWnd);
	//2017.02.03
	double ValueLimitChk(double value);
	double ValueLimitChk1(double value);
public:
	CString m_strPanelID_1;
	CString m_strPanelID_2;
	int		m_linspactEqpNo;
	STRU_IPC_STATUS m_stSockStatus;
	int	m_nRecvOriginType;				//2017.07.10
	BOOL m_bGrabEndFlag[MAX_VISION_PC];
	int m_nGrabEndRes[MAX_VISION_PC];
	BOOL m_bInspectEndFlag[MAX_VISION_PC];
	BOOL m_bRecvReadyRes[MAX_VISION_PC];
	int m_nGrabReadyRes[MAX_VISION_PC];
	BOOL m_bRecvRecipeRes[MAX_VISION_PC];
	int m_nVisionState[MAX_VISION_PC];
	//2016.04.04
	BOOL m_bControllerFlag[MAX_VISION_PC][LIGHT_CONTROL_MAX];
	int m_nTemperature[MAX_VISION_PC][LIGHT_CONTROL_MAX];
	int m_nLimitTemperature[MAX_VISION_PC][LIGHT_CONTROL_MAX];
	//2016.04.04
	CPtrList m_ListClientSock;
	CString  m_AutoRecipeMSG;

	//2016.11.28
	DOUBLE m_dReferencePoint_X;
	DOUBLE m_dReferencePoint_Y;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int m_fnStartVsMsgReceiver();
	void m_fnRequestGrabReady(int nVisionID, int nScanDir, CString strModelID, CString strPanelID_1, CString strPanelID_2);
	void m_fnResponseSystemTime(int nVisionID);	//2016.03.14
	int  Seq_AlignProcReset();					//2016.11.04 Mr.Chu		//PLC Reset시 Contact Align 초기화를 위한 작업
	//int  Seq_AlignProcChk();
	void  Seq_AlignProcChk();
	int  Seq_InspAliveChk(int nVisionID);
	int m_fnSelectVisionRecipe(int nVisionID, CString strRecipe);
	void m_fnRequestAlign(CString strPanelID_1);
	void m_fnRequestVisionRecipe(int nVisionID);
	virtual int	CmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, USHORT PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType = CMD_TYPE_NORES){return 0;}
	void m_fnVisionInitial(int nVisionID);
	void m_fnRequestMonitorInfo();			//2017.02.18 add by mby

	BOOL m_fnIsInspectEnd()
	{
		for(int i = 0; i < MAX_VISION_PC; i++)
		{
			if(m_bInspectEndFlag[i] == FALSE)
				return FALSE;
		}

		return TRUE;
	}

	void m_fnSetGrabEndFlag(int nVisionID, BOOL bFlag)
	{
		m_bGrabEndFlag[nVisionID - 1] = TRUE;
	}

	void m_fnSetInspectEndFlag(int nVisionID, BOOL bFlag)
	{
		m_bInspectEndFlag[nVisionID - 1] = TRUE;
	}

	void m_fnSetGrabReadyRes(int nVisionID, int nRes)
	{
		m_nGrabReadyRes[nVisionID - 1] = nRes;
	}

	void m_fnSetGrabEndRes(int nVisionID, int nRes)
	{
		m_nGrabEndRes[nVisionID - 1] = nRes;
	}

	void m_fnResetFlag(int nVisionID)
	{
		m_bGrabEndFlag[nVisionID - 1]	 = FALSE;
		m_bInspectEndFlag[nVisionID - 1] = FALSE;
		m_nVisionState[nVisionID - 1]	 = STATE_IDLE;
		m_bRecvReadyRes[nVisionID - 1]	 = FALSE;
	}
	
};