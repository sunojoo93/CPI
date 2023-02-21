
// SpaceManager.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "SpaceManager.h"
#include "SpaceManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceManagerApp

BEGIN_MESSAGE_MAP(CSpaceManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSpaceManagerApp ����

CSpaceManagerApp::CSpaceManagerApp()
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CSpaceManagerApp ��ü�Դϴ�.

CSpaceManagerApp theApp;

// CSpaceManagerApp �ʱ�ȭ

BOOL CSpaceManagerApp::InitInstance()
{

	// �ߺ� ���� ����
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("SpaceManager"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		ReleaseMutex(hMutex);

		CWnd *pWndPrev, *pWndChild;
		pWndPrev = CWnd::FindWindow(NULL, _T("SpaceManager"));
		if(pWndPrev)
		{
			pWndChild = pWndPrev->GetLastActivePopup();

			if(pWndChild->IsIconic())
				pWndPrev->ShowWindow(SW_RESTORE);

			pWndChild->SetForegroundWindow();
		}
		return FALSE;
	}
	ReleaseMutex(hMutex);

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

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	m_pLogWriter = new CLogWriter(SM_LOG_PATH, _T("SpaceManager"), 7, 0, true);
	//2016.03.15 ������� ������� ���� ��ȯ�ȵʹ��� �������� D:\Crux_Data\INI\Language\SpaceManager������ �̵���Ŵ
	//CreateDirectory(ORG_FILE_PATH, NULL);
	if(!PathIsDirectory(ORG_FILE_PATH))
		SHCreateDirectoryEx(NULL, ORG_FILE_PATH, NULL);
	CopyFile(ORG_RESOURCE_PATH, RESOURCE_PATH, 0);		        // ������ ���� ����
	CopyFile(ORG_LANG_KOREAN_PATH, LANG_KOREAN_PATH, 0);		// ������ ���� ����
	CopyFile(ORG_LANG_CHINESE_PATH, LANG_CHINESE_PATH, 0);		// ������ ���� ����
	//2016.03.15 ������� ������� ���� ��ȯ�ȵʹ��� �������� D:\Crux_Data\INI\Language\SpaceManager������ �̵���Ŵ

	CSpaceManagerDlg dlg;
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

	// �α� ��ü ����
	if(m_pLogWriter){ delete (m_pLogWriter); (m_pLogWriter)=0; }

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

// �α� ��� �߰�
void CSpaceManagerApp::m_fnWriteLog(const TCHAR* pszLog, ...)
{
	TCHAR cBuffer[1024] = {0, };

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	vswprintf_s(cBuffer, pszLog, (va_list)vaList);
	va_end(vaList);

	m_pLogWriter->m_fnWriteLog(cBuffer);
}