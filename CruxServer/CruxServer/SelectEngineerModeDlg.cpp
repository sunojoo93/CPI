// SelectEngineerModeDlg.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "SelectEngineerModeDlg.h"
#include "afxdialogex.h"


// CSelectEngineerModeDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSelectEngineerModeDlg, CDialogEx)

CSelectEngineerModeDlg::CSelectEngineerModeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSelectEngineerModeDlg::IDD, pParent)
{

	m_nSelectEngineerMode = 0;
}

CSelectEngineerModeDlg::~CSelectEngineerModeDlg()
{
}

void CSelectEngineerModeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_ENGINEER_CALL, m_nSelectEngineerMode);
}


BEGIN_MESSAGE_MAP(CSelectEngineerModeDlg, CDialogEx)
END_MESSAGE_MAP()


// CSelectEngineerModeDlg 메시지 처리기입니다.
