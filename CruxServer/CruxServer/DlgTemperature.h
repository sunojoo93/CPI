#pragma once
#include "DlgMessage.h"

// Dlg_Temperature 대화 상자입니다.

class CDlgTemperature : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgTemperature)

public:
	CDlgTemperature(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgTemperature();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_TEMPERATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	void m_fnInitState();
	void m_fnUpdateState();
	void m_fnAddLog();
	void m_fnSaveFileTemperlInfo(CString strTempFilePath, int nPcCnt, int nCtrCnt, int nLimitTemp, int nCurTemp);
	int m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue);
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSave();
	CDlgMessage * m_pDlgErrorMsg;
	HANDLE		  m_hWarnnigHandle;
	static unsigned __stdcall  m_fnCheckErrorClear(LPVOID pArg );	
};
