// DlgMessage2.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgMessage2.h"
#include "afxdialogex.h"


// CDlgMessage2 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgMessage2, CDialogEx)

CDlgMessage2::CDlgMessage2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgMessage2::IDD, pParent)
{
	
}

CDlgMessage2::~CDlgMessage2()
{
}

void CDlgMessage2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgMessage2, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgMessage2::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMessage2 메시지 처리기입니다.


void CDlgMessage2::init(CString str)
{
	GetDlgItem(IDC_STATIC)->SetWindowText(str);
}

void CDlgMessage2::OnBnClickedOk()
{
	// TODO: ??? ??? ?? ??? ??? ?????.
	CDialogEx::OnOK();
}
