// SelectEngineerModeDlg.cpp : ���� �����Դϴ�.
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


// CSelectEngineerModeDlg ��ȭ �����Դϴ�.

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


// CSelectEngineerModeDlg �޽��� ó�����Դϴ�.
