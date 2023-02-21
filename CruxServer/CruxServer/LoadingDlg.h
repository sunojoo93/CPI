#pragma once


// CLoadingDlg ��ȭ �����Դϴ�.

class CLoadingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLoadingDlg)
private:
	double m_dbTotalTime;
	double m_dbCurTime;

	CProgressCtrl *m_pProgress;
public:
	CLoadingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoadingDlg();

	void SetTotalTime(double dbTotalTime);
	void SetCurTime(double dbCurTime);
	void CloseDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LOADING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
