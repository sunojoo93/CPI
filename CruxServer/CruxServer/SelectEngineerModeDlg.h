#pragma once


// CSelectEngineerModeDlg 대화 상자입니다.

class CSelectEngineerModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectEngineerModeDlg)

public:
	CSelectEngineerModeDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectEngineerModeDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SELECT_ENGINEER_MODE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nSelectEngineerMode;
};
