// LoadingDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "LoadingDlg.h"
#include "afxdialogex.h"


// CLoadingDlg ��ȭ �����Դϴ�.

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


// CLoadingDlg �޽��� ó�����Դϴ�.
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

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	if(m_pProgress == NULL)
	{
		m_pProgress = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
		m_pProgress->SetRange(0, 100);
		m_pProgress->SetPos(0);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


BOOL CLoadingDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
