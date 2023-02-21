
// VSSequenceTask.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

#include "TaskInterfacer.h"
#include "Config.h"
#include "CPUTimer.h"
#include <vector>

#define MAX_PROC_PANEL_COUNT	100
#define MAX_STAGE_COUNT			6
 
// CVSSequenceTaskApp:
// �� Ŭ������ ������ ���ؼ��� VSSequenceTask.cpp�� �����Ͻʽÿ�.
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

	// Panel ID �˻��� ���� ������ �����ε�
	bool operator==(const TCHAR* strCompPanelID)
	{
		return _tcscmp(strPanelID, strCompPanelID) == 0;
	}
};

// Repeat Alarm ����ü
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

// Time Repeat Alarm ����ü
struct ST_TIME_REPEAT_ALARM_INFO
{
	bool		bUse;			// ��� ����
	CTimeSpan	tsAlarmTime;	// �� �ð��ȿ�
	int			nAlarmCount;	// �� Ƚ����ŭ strJudge�� �߻��ϸ� �˶� �߻�
	CString		strJudge[10];
	std::vector<ST_TIME_REPEAT_CELL_INFO>	vCells;	// Ŭ�����ص� ���� ����
};


class CVSSequenceTaskApp : public CWinApp
{
public:
	CVSSequenceTaskApp();
	~CVSSequenceTaskApp();
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	// Get
	BOOL				GetIPCState()				{	return m_bIPCConnect		;};
	int					GetGrabLimitWaitTime()		{	return m_nGrabLimitWaitTime	;};
	
	// Set
	void				SetIPCState(BOOL bState)	{	m_bIPCConnect = bState		;};

	// Visual Station Thread
	static UINT			ThreadConnectVSServer(LPVOID pParam);
	BOOL				m_bExec;				// IPC Trhead ���� Flag

	// �������� ������ �ʿ䰡 �ִ� ���� �� �Լ��� ���⿡ ���� �� theApp. ���
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