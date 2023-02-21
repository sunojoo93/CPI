#pragma once


// CLoadingDlg 대화 상자입니다.

class CLoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadingDlg)
private:
	double m_dbTotalTime;
	double m_dbCurTime;

	CProgressCtrl *m_pProgress;
public:
	CLoadingDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadingDlg();

	void SetTotalTime(double dbTotalTime);
	void SetCurTime(double dbCurTime);
	void CloseDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_LOADING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
