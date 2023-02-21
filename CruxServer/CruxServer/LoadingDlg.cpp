// LoadingDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "LoadingDlg.h"
#include "afxdialogex.h"


// CLoadingDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoadingDlg, CDialogEx)

CLoadingDlg::CLoadingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoadingDlg::IDD, pParent)
{
	m_dbTotalTime = 0;
	m_dbCurTime   = 0;

	m_pProgress  = NULL;
}

CLoadingDlg::~CLoadingDlg()
{
}

void CLoadingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoadingDlg, CDialogEx)
END_MESSAGE_MAP()


// CLoadingDlg 메시지 처리기입니다.
void CLoadingDlg::SetTotalTime(double dbTotalTime)
{
	m_dbTotalTime = dbTotalTime;
}
	
void CLoadingDlg::SetCurTime(double dbCurTime)
{
	m_dbCurTime = dbCurTime;
	m_pProgress->SetPos((int)(100. * m_dbCurTime / m_dbTotalTime));
}

void CLoadingDlg::CloseDialog()
{
	::PostMessage(GetSafeHwnd(), WM_CLOSE, NULL, NULL);
}

BOOL CLoadingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	if(m_pProgress == NULL)
	{
		m_pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
		m_pProgress->SetRange(0, 100);
		m_pProgress->SetPos(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CLoadingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		break;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
