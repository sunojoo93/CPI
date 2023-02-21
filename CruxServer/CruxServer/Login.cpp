// Login.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "Login.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"


// CLogin 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLogin, CDialog)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CLogin::IDD, pParent)
{
	m_bLoginCheck = FALSE;
	m_mod = FALSE;
}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogin, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT, &CLogin::OnBnClickedButtonAccept)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CLogin::OnBnClickedButtonExit)
END_MESSAGE_MAP()


// CLogin 메시지 처리기입니다.


void CLogin::OnBnClickedButtonAccept()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_strGetPW = _T("");
	GetDlgItem(IDC_EDIT_PW)->GetWindowText(m_strGetPW);

	m_fnCheckPassword();
}

void CLogin::m_fnCheckPassword(void)
{
	TCHAR strBuffer[255] = {0};

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	
	//Load Login Info
	CString strTemp;
	strTemp.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
	m_iniLoginInfo.SetFileName(strTemp);
	
	//2019.01.28 ryu
	if(m_mod)
	{
		m_iniLoginInfo.GetProfileStringW(_T("LOGIN_DATA"), _T("PW_SUB"), _T(""), (LPTSTR)strBuffer, 255);
		m_mod = FALSE;
	}
	else
	{
		m_iniLoginInfo.GetProfileStringW(_T("LOGIN_DATA"), _T("PW"), _T(""), (LPTSTR)strBuffer, 255);
	}

	if( m_strGetPW == strBuffer )
	{
		m_fnSetLoginCheck(TRUE);
		//DestroyWindow();
		CDialog::EndDialog(IDOK);
	}
	else
	{
		m_fnSetLoginCheck(FALSE);
		GetDlgItem(IDC_STATIC_LOGIN_CHECK)->SetWindowText(_T("Password mismatch !"));
		GetDlgItem(IDC_EDIT_PW)->SetWindowText(_T(""));
	}
}

void CLogin::m_fnSetLoginCheck(BOOL bCheck)
{
	m_bLoginCheck = bCheck;
}

BOOL CLogin::m_fnGetLoginCheck()
{
	return m_bLoginCheck;
}

void CLogin::OnBnClickedButtonExit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//DestroyWindow();
	m_fnSetLoginCheck(FALSE);

	CDialog::EndDialog(IDCANCEL);
	return ;
}

//2017.03.24
BOOL CLogin::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam || VK_ESCAPE == pMsg->wParam)
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
