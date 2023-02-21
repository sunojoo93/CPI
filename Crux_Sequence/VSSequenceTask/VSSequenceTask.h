
// VSSequenceTask.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

#include "TaskInterfacer.h"
#include "Config.h"
#include "CPUTimer.h"
#include <vector>

#define MAX_PROC_PANEL_COUNT	100
#define MAX_STAGE_COUNT			6
 
// CVSSequenceTaskApp:
// 이 클래스의 구현에 대해서는 VSSequenceTask.cpp을 참조하십시오.
//

struct ST_PANEL_PROC_INFO
{
	BOOL  bIsManual;
	TCHAR strPanelID[50];
	TCHAR strVirtualID[50];
	TCHAR strModelID[50];
	CTime timeStart;
	CTime timeEnd;
	int   nStageNo;	//2019.07.10 nStageNo

	ST_PANEL_PROC_INFO()
	{
		bIsManual = FALSE;
		memset(strPanelID, 0, sizeof(strPanelID));
		memset(strModelID, 0, sizeof(strModelID));
		nStageNo = 0;	//2019.07.10 nStageNo
	}

	// Panel ID 검색을 위한 연산자 오버로딩
	bool operator==(const TCHAR* strCompPanelID)
	{
		return _tcscmp(strPanelID, strCompPanelID) == 0;
	}
};

// Repeat Alarm 구조체
struct ST_REPEAT_ALARM_INFO
{
	bool	bUse;
	int		nRepeatCount;
	int		nCurrentCount[MAX_STAGE_COUNT];
	CString	strJudge[10];
};

struct ST_TIME_REPEAT_CELL_INFO
{
	CString		strCellId;
	CTime		tClassEnd;
};

// Time Repeat Alarm 구조체
struct ST_TIME_REPEAT_ALARM_INFO
{
	bool		bUse;			// 사용 여부
	CTimeSpan	tsAlarmTime;	// 이 시간안에
	int			nAlarmCount;	// 이 횟수만큼 strJudge가 발생하면 알람 발생
	CString		strJudge[10];
	std::vector<ST_TIME_REPEAT_CELL_INFO>	vCells;	// 클래스앤드 시점 저장
};


class CVSSequenceTaskApp : public CWinApp
{
public:
	CVSSequenceTaskApp();
	~CVSSequenceTaskApp();
// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	// Get
	BOOL				GetIPCState()				{	return m_bIPCConnect		;};
	int					GetGrabLimitWaitTime()		{	return m_nGrabLimitWaitTime	;};
	
	// Set
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead 실행 Flag

	// 전역으로 접근할 필요가 있는 변수 및 함수는 여기에 선언 후 theApp. 사용
	// Variable
	CConfig						m_Config;
	TaskInterfacer*				m_SequenceTask;
	vector<ST_PANEL_PROC_INFO>	m_vPanelProcInfo;
	vector<ST_REPEAT_ALARM_INFO*>	m_vRepeatAlarmInfo;	// Repeat Alarm
	vector<ST_TIME_REPEAT_ALARM_INFO*>	m_vTimeRepeatAlarmInfo;	// Time Repeat Alarm
	ST_REPEAT_ALARM_INFO*		m_fnGetRepeatAlarmInfo(CString strJudgeCode);
	ST_TIME_REPEAT_ALARM_INFO*		m_fnGetTimeRepeatAlarmInfo(CString strJudgeCode);

 	// Function
	void						m_fnInitializeRepeatAlarmInfo();	// 2019.09.26 Repeat Alarm
	void						m_fnInitializeTimeRepeatAlarmInfo();	// 2019.09.26 Repeat Alarm

	DECLARE_MESSAGE_MAP()

private:
	CWinThread*			m_pVSThread;

	BOOL				m_bIPCConnect;

	HANDLE				m_hEventThreadAlive;	

	int					m_nGrabLimitWaitTime;

	int					m_fnConectVisualStation();
};

extern CVSSequenceTaskApp theApp;