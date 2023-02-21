
// Crux_MainPcInterface.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "Crux_MainPcInterface.h"
#include "Crux_MainPcInterfaceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCrux_MainPcInterfaceApp

BEGIN_MESSAGE_MAP(CCrux_MainPcInterfaceApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CCrux_MainPcInterfaceApp ����

CCrux_MainPcInterfaceApp::CCrux_MainPcInterfaceApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.

	CString strArgv = __targv[1];
	strArgv = strArgv.MakeLower();
	if (strArgv == _T("simul") || strArgv == _T("simulation") || strArgv == _T("1"))
	{
		m_bSimulationProgramMode = TRUE;
	}


	TCHAR szTemp[100] = {0,};
	CString strIsSimulMode;
	GetPrivateProfileString(_T("Common"), _T("SIMULATION Mode") ,_T("FALSE"), szTemp, sizeof(szTemp), INIT_FILE_PATH);
	strIsSimulMode = szTemp;
	strIsSimulMode = strIsSimulMode.MakeLower();
	if ( strIsSimulMode == _T("true"))
	{
		//ShellExecute(NULL, _T("open"), _T("Crux_MainPcInterface.exe"), NULL, NULL, SW_SHOW);
		m_bSimulationMode = TRUE;
	}	

	// ���� ���� ȯ�濡���� �ù������Ͱ� �ʿ� ����. �ù������ʹ� �����Ѵ�.
	if (m_bSimulationMode == FALSE && m_bSimulationProgramMode == TRUE)
	{
		PostQuitMessage(0);
	}
}


// ������ CCrux_MainPcInterfaceApp ��ü�Դϴ�.

CCrux_MainPcInterfaceApp theApp;


// CCrux_MainPcInterfaceApp �ʱ�ȭ

BOOL CCrux_MainPcInterfaceApp::InitInstance()
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

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	m_nPCNum		= GetPrivateProfileInt(_T("Common"), _T("VISION PC NUM"), 1, INIT_FILE_PATH);
	m_nInspType		= GetPrivateProfileInt(_T("Common"), _T("TYPE"), 1, INIT_FILE_PATH);
	
	// YHS 18.03.12 - Merge Tool ��� ����
	m_nInspStep		= GetPrivateProfileInt(_T("MergeTool"),_T("USE_MERGE_TOOL"),1,INIT_FILE_PATH)? true : false;

	m_nGrabLimitCount = GetPrivateProfileInt(_T("Grab Limit"), _T("Grab Limit Count"), 3, INIT_FILE_PATH);
	m_nGrabLimitWaitTime = GetPrivateProfileInt(_T("Grab Limit"), _T("Grab Limit Wait Time"), 30000, INIT_FILE_PATH);

	CCrux_MainPcInterfaceDlg dlg;
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

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

