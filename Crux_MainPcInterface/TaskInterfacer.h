#pragma once

#include "..\VisualStation\CommonHeader\Class\interserverinterface.h"
#include "StatusManager.h"
#include "WorkManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class TaskInterfacer :
	private StatusManager, public WorkManager, protected CInterServerInterface
{
public:
	TaskInterfacer(int nTaskNo, int nServerPort, TSTRING szServerIP, 
		TSTRING szLogFilePath,			///< 로그 파일 경로
		TSTRING szLogFileName,			///< 로그 파일 이름
		bool bAutoExitFlag = true,		///< 서버가 종료 될때 Application 을 자동 종료 시킬지를 가지는 Flag, Windows handle 이 등록된경우는 자동종료안된다.
		int uPeriodDay = 30,			///< 로그 보존 기한일
		int uWriteTerm = 0,				///< 로그 파일을 몇시간 주기로 생성하는가..시간, 0 = 하루치의 로그가 파일 하나로 생성됨.
		USHORT uLogBlockLevel = LOG_LEVEL_4,
		HWND hPwnd = NULL				////< Dlg 핸들러
		);
	~TaskInterfacer();

	int Start();

	int		CmdEditSend(USHORT FunID, USHORT SeqNo, USHORT UnitNo, ULONG PrmSize, USHORT TaskNo, byte* Buff, USHORT uMsgType = CMD_TYPE_NORES, ULONG lMsgTimeout = CMD_TIMEOUT);

private:
	bool	GetThreadRunFlag();			// Threadを実行するか確認Flag を Get
	//int		VSMessageReceiver();		// Visua Station MessageをGetしてIndexerWorkManagerのMessageQueueにPUT


	int		ResponseSend(USHORT ReturnVal, CMDMSG* pCmdMsg);
	
	BYTE*	m_fnPeekMessage();
	void	EndWorkProcess(BYTE* pMsg);
	int		m_fnPrintLog( 
		const wchar_t* pszLogData,		///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		);

	int		m_fnPrintLog( 
		const char* pszLogData,			///< 실제 기록 할 Log 내용
		...								///< 가변 인자
		);
private:

	bool			m_bThreadRun;						// Threadを実行するか確認Flag
	int				m_nTaskNo;
	int				m_nServerPort;
	TCHAR			m_szServerIP[100];
	HWND			m_hParentWnd;
};