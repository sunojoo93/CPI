#pragma once


// CDlgMessage2 대화 상자입니다.

class CDlgMessage2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMessage2)

public:
	CDlgMessage2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	void init(CString str);
	virtual ~CDlgMessage2();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MESSAGE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
