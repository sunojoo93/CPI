
// VSSequenceTask.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "VSSequenceTask.h"
#include "VSSequenceTaskDlg.h"
#include <conio.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSSequenceTaskApp

BEGIN_MESSAGE_MAP(CVSSequenceTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSSequenceTaskApp ����

CVSSequenceTaskApp::CVSSequenceTaskApp()
	:m_bIPCConnect(FALSE)
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}

CVSSequenceTaskApp::~CVSSequenceTaskApp()
{
}

// ������ CVSSequenceTaskApp ��ü�Դϴ�.

CVSSequenceTaskApp theApp;


// CVSSequenceTaskApp �ʱ�ȭ

BOOL CVSSequenceTaskApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// Command Line ��ɾ� �޾ƿ���
	int iCnt = 0;
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW(GetCommandLine(), &iCnt);

	CString strRegKey;

	// ���� ���� Load
	m_Config.Load();

	// 19.12.03 yskim - Grab Limit Count
	m_nGrabLimitWaitTime = GetPrivateProfileInt(_T("Grab Limit"), _T("Grab Limit Wait Time"), 0, PATH_INIT_FILE);

	// Visual Station ����
	m_bExec = TRUE;
	m_hEventThreadAlive = NULL;
	m_hEventThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);
		
	strRegKey.Format(_T("YWDSP Sequence Task"));

	LocalFree(pStr);

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.	
	SetRegistryKey(strRegKey);

	// Tact ������ ���� Proc ����ü �ִ� ���� ��ŭ ����
	m_vPanelProcInfo.reserve(MAX_PROC_PANEL_COUNT);

	CVSSequenceTaskDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// IPC ���� ���
	m_bExec = FALSE;
	::WaitForSingleObject(m_hEventThreadAlive, INFINITE);
	ResetEvent(m_hEventThreadAlive);
	m_pVSThread = NULL;

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

ST_REPEAT_ALARM_INFO* CVSSequenceTaskApp::m_fnGetRepeatAlarmInfo(CString strJudgeCode)
{
	/*
	�˶� ���� ���Ϳ��� strJudgeCode�� ����ִ� �˶� ������ ã�� ��ȯ�Ѵ�.
	���������� ã�ٰ� ���ʷ� ã�� �˶� �������� ��ȯ�Ѵ�.
	*/
	ST_REPEAT_ALARM_INFO* info = NULL;

	for (int i = 0; i < (int) m_vRepeatAlarmInfo.size(); i++)
	{
		for (int k = 0; k < 10; k++)
		{
			if (m_vRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(_T("")) == 0)
				break;

			if (m_vRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(strJudgeCode) == 0)
			{
				info = m_vRepeatAlarmInfo[i];
				break;
			}
		}

		if (info != NULL)
			break;
	}

	return info;
}

ST_TIME_REPEAT_ALARM_INFO* CVSSequenceTaskApp::m_fnGetTimeRepeatAlarmInfo(CString strJudgeCode)
{
	/*
	�˶� ���� ���Ϳ��� strJudgeCode�� ����ִ� �˶� ������ ã�� ��ȯ�Ѵ�.
	���������� ã�ٰ� ���ʷ� ã�� �˶� �������� ��ȯ�Ѵ�.
	*/
	ST_TIME_REPEAT_ALARM_INFO* info = NULL;

	for (int i = 0; i < (int) m_vTimeRepeatAlarmInfo.size(); i++)
	{
		for (int k = 0; k < 10; k++)
		{
			if (m_vTimeRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(_T("")) == 0)
				break;

			if (m_vTimeRepeatAlarmInfo[i]->strJudge[k].CompareNoCase(strJudgeCode) == 0)
			{
				info = m_vTimeRepeatAlarmInfo[i];
				break;
			}
		}

		if (info != NULL)
			break;
	}

	return info;
}

void CVSSequenceTaskApp::m_fnInitializeRepeatAlarmInfo()
{
	/*
	Repeat Alarm Info �ʱ�ȭ
	�� PC �� CRUX_Sequence.ini �� �����ϴ� ����Ʈ �˶� ������ �����´�.
	������ ���� ��� �⺻���� �������ش�. (ini���� �⺻�� �־��ش�.)
	�⺻��
	[Repeat Alarm Info]
	item1 = 0, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS
	; ù��° ���� 1(=���), (0=�̻��)
	; �ι�° ���� 3(=Repeat Count, ��, 3�� �̻�(>=) ���� �߻� �� �˶�)
	; ����° ���� DISPLAY_ABNORMAL(=ī��Ʈ�� �����)

	(ini �ۼ� ����)
	[Repeat Alarm Info]
	item1 = 1, 3, ABNORMAL_AD1+ABNORMAL_AD3
	item2 = 1, 3, WHITE_BRIGHTNESS

	���߿� UI �ʿ��� ���
	UI ȭ�� �����, UI ���� �� �� ���˴�� ini ���Ͽ� ���� ������ ��, Seq���� �� �Լ��� ȣ�����ָ� ��
	*/

	TCHAR szBuf[500] = { 0, };
	CString strItemKey;
	CString strItemValue;
	CString strArgu1, strArgu2, strArgu3;
	CString temp;

	m_vRepeatAlarmInfo.clear();

	for (int i = 1; i <= MAX_PROC_PANEL_COUNT; i++)
	{
		strItemKey.Format(_T("item%d"), i);
		GetPrivateProfileString(_T("Repeat Alarm Info"), strItemKey, _T(""), szBuf, sizeof(szBuf), VS_SEQUENCE_TASK_INI_FILE);
		strItemValue.Format(_T("%s"), szBuf);

		// �� ���̸� �����.
		if (strItemValue.CompareNoCase(_T("")) == 0)
			break;

		ST_REPEAT_ALARM_INFO* stInfo = new ST_REPEAT_ALARM_INFO;

		// ù��° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu1, strItemValue, 0, ',');
		strArgu1.Trim();
		stInfo->bUse = strArgu1.CompareNoCase(_T("0")) != 0;	// 0�� �ƴϸ� true

		// �ι�° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu2, strItemValue, 1, ',');
		strArgu2.Trim();
		stInfo->nRepeatCount = _ttoi(strArgu2);

		// ����° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu3, strItemValue, 2, ',');
		strArgu3.Trim();
		for (int k = 0; k < 10; k++)
		{
			AfxExtractSubString(temp, strArgu3, k, '+');
			stInfo->strJudge[k] = temp;
		}

		// ���� �߻� Ƚ�� �ʱ�ȭ
		for (int stage_index = 0; stage_index < MAX_STAGE_COUNT; stage_index++)
		{
			stInfo->nCurrentCount[stage_index] = 0;
		}

		// ���Ϳ� �ִ´�.
		m_vRepeatAlarmInfo.push_back(stInfo);
	}

	// ���Ͱ� ����ִٸ� �⺻ ���� �������ش�.
	if (m_vRepeatAlarmInfo.empty())
	{
		WritePrivateProfileString(_T("Repeat Alarm Info"), _T("item1"), _T("0, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS"), VS_SEQUENCE_TASK_INI_FILE);
		m_fnInitializeRepeatAlarmInfo();
	}
}

void CVSSequenceTaskApp::m_fnInitializeTimeRepeatAlarmInfo()
{
	/*
	Time Repeat Alarm Info �ʱ�ȭ
	�� PC �� CRUX_Sequence.ini �� �����ϴ� ����Ʈ �˶� ������ �����´�.
	������ ���� ��� �⺻���� �������ش�. (ini���� �⺻�� �־��ش�.)
	�⺻��
	[Repeat Alarm Info]
	item1 = 0, 20, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS
	; ù��° ���� 1(=���), (0=�̻��)
	; �ι�° ���� 20(=�ð� ���� (����:��), 20�� �̸����� �߻� Ƚ���� ī�����ϱ� ����)
	; ����° ���� 3(=�ݺ� ����, ������ �ð� �� �߻� Ƚ���� 3ȸ �̻��̸� �˶� �߻�)
	; �׹�° ���� DISPLAY_ABNORMAL(=ī��Ʈ�� �����)

	(ini �ۼ� ����)
	[Repeat Alarm Info]
	item1 = 1, 10, 3, ABNORMAL_AD1+ABNORMAL_AD3
	item2 = 1, 20, 5, WHITE_BRIGHTNESS

	���߿� UI �ʿ��� ���
	UI ȭ�� �����, UI ���� �� �� ���˴�� ini ���Ͽ� ���� ������ ��, Seq���� �� �Լ��� ȣ�����ָ� ��
	*/

	TCHAR szBuf[500] = { 0, };
	CString strItemKey;
	CString strItemValue;
	CString strArgu1, strArgu2, strArgu3, strArgu4;
	CString temp;

	// ���� �ʱ�ȭ
	for (int i = 0; i < (int) m_vTimeRepeatAlarmInfo.size(); i++)
		m_vTimeRepeatAlarmInfo[i]->vCells.clear();

	m_vTimeRepeatAlarmInfo.clear();

	for (int i = 1; i <= MAX_PROC_PANEL_COUNT; i++)
	{
		strItemKey.Format(_T("item%d"), i);
		GetPrivateProfileString(_T("Time Repeat Alarm Info"), strItemKey, _T(""), szBuf, sizeof(szBuf), VS_SEQUENCE_TASK_INI_FILE);
		strItemValue.Format(_T("%s"), szBuf);

		// �� ���̸� �����.
		if (strItemValue.CompareNoCase(_T("")) == 0)
			break;

		ST_TIME_REPEAT_ALARM_INFO* stInfo = new ST_TIME_REPEAT_ALARM_INFO;

		// ù��° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu1, strItemValue, 0, ',');
		strArgu1.Trim();
		stInfo->bUse = strArgu1.CompareNoCase(_T("0")) != 0;	// 0�� �ƴϸ� true

		// �ι�° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu2, strItemValue, 1, ',');
		strArgu2.Trim();
		stInfo->tsAlarmTime = CTimeSpan(0, 0, _ttoi(strArgu2), 0);

		// ����° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu3, strItemValue, 2, ',');
		strArgu3.Trim();
		stInfo->nAlarmCount = _ttoi(strArgu3);

		// �׹�° ���� �Ľ� �� ����
		AfxExtractSubString(strArgu4, strItemValue, 3, ',');
		strArgu4.Trim();
		for (int k = 0; k < 10; k++)
		{
			AfxExtractSubString(temp, strArgu4, k, '+');
			stInfo->strJudge[k] = temp;
		}

		// ���Ϳ� �ִ´�.
		m_vTimeRepeatAlarmInfo.push_back(stInfo);
	}

	// ���Ͱ� ����ִٸ� �⺻ ���� �������ش�.
	if (m_vTimeRepeatAlarmInfo.empty())
	{
		WritePrivateProfileString(_T("Time Repeat Alarm Info"), _T("item1"), _T("0, 20, 3, ABNORMAL_AD1+ABNORMAL_AD3+WHITE_BRIGHTNESS"), VS_SEQUENCE_TASK_INI_FILE);
		m_fnInitializeTimeRepeatAlarmInfo();
	}
}

UINT CVSSequenceTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSSequenceTaskApp* pThis = (CVSSequenceTaskApp*)pParam;

	// Visual Station ����
	int					nRet;

	do 
	{
		nRet = pThis->m_fnConectVisualStation();
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExec);

	printf("Internal Sequence Start \n");

	pThis->m_SequenceTask->SendNotifyInitial();

	// VS ���� üũ
	while(pThis->m_bExec)
	{
		// VS ���� ���� Get
		if (pThis->m_SequenceTask != NULL)
			// VS ���� ���� Get
			pThis->SetIPCState(pThis->m_SequenceTask->GetVSState());
		Sleep(1000);
	}
	
	pThis->SetIPCState(FALSE);
	//SAFE_DELETE(pInternalServer);

	SetEvent(pThis->m_hEventThreadAlive);
	printf("Internal Sequence End \n");

	return 0;
}

int CVSSequenceTaskApp::m_fnConectVisualStation()
{
	if (m_SequenceTask != NULL)
	{
		return APP_OK;
	}
	TCHAR szSeqLogPath[100] = {0,}, szSeqFileName[100] = {0,}, szServerIP[100] = {0,};
	int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;	

	// Sequence Task �� PC �� �ϳ��� �־�� ��	
	nTaskNo				= (m_Config.GetPCNum() * 100)
						+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 21, VS_SEQUENCE_TASK_INI_FILE));
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, VS_SEQUENCE_TASK_INI_FILE);
	uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, VS_SEQUENCE_TASK_INI_FILE);
	uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, VS_SEQUENCE_TASK_INI_FILE);
	uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, VS_SEQUENCE_TASK_INI_FILE);

	GetPrivateProfileString(_T("LogInfo"), _T("Path") ,theApp.m_Config.GETDRV() + _T(":\\VisualStation\\VSLOG\\VSPLC"), szSeqLogPath, sizeof(szSeqLogPath), VS_SEQUENCE_TASK_INI_FILE);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("CRUX"), szSeqFileName, sizeof(szSeqFileName), VS_SEQUENCE_TASK_INI_FILE);
	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), VS_SEQUENCE_TASK_INI_FILE);

	m_SequenceTask = new TaskInterfacer(nTaskNo, nServerPort, szServerIP, szSeqLogPath,
		szSeqFileName, false, uPeriodDay, uWriteTerm, uWriteLogLevel);
	int nRet = m_SequenceTask->Start();
	if (nRet != APP_OK)
	{
		SAFE_DELETE(m_SequenceTask);
	}
	return nRet;
}
