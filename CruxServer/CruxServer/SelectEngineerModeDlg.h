#pragma once


// CSelectEngineerModeDlg ��ȭ �����Դϴ�.

class CSelectEngineerModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectEngineerModeDlg)

public:
	CSelectEngineerModeDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSelectEngineerModeDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECT_ENGINEER_MODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nSelectEngineerMode;
};
