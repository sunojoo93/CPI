#pragma once


// IDD_DLG_MESSAGE2 대화 상자입니다.

class IDD_DLG_MESSAGE2 : public CDialogEx
{
	DECLARE_DYNAMIC(IDD_DLG_MESSAGE2)

public:
	IDD_DLG_MESSAGE2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~IDD_DLG_MESSAGE2();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
