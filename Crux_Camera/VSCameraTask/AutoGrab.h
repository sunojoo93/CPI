#pragma once
#include "resource.h"
#include "afxwin.h"
#include "CameraWorkManager.h"
#include "VSCameraTask.h"
#include "afxcmn.h"

#define MAX_PATTERN 9

// CAutoGrab 대화 상자입니다.

struct stAutoGrab
{
	TCHAR strPattern[50];
	int nRepeat;
	int nPeriod;
	int nExposureTime;
	int nGain;
	int nSaveNum;

	stAutoGrab()
	{
		memset(strPattern,0,50);
		nRepeat = 0;
		nPeriod = 0;
		nExposureTime = 0;
		nGain = 0;
		nSaveNum = 0;
	}
};
class CAutoGrab : public CDialogEx
{
	DECLARE_DYNAMIC(CAutoGrab)

public:
	CAutoGrab(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAutoGrab();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_AUTO_GRAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonStart();
	CButton m_Red;
	CButton m_Green;
	CButton m_Blue;
	CEdit m_GrabCount;
	CEdit m_GrabPeriod;
	
	CWinThread*     m_pThrAutoGrab;
	VSMessageProcessor* m_pVsMessage;

	afx_msg void OnBnClickedButtonPath();
	void m_fnSaveData();
	void m_fnReadData();
	void m_fnRenewalData();
	CEdit m_DirPath;
	CString m_strDirPath;
	BOOL m_bStopFlag;
	HANDLE m_hStopEvent;
private:
	int m_oldItem;
	int m_oldSubItem;
	
	

public:
	CListCtrl m_ListOption;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListOption(NMHDR *pNMHDR, LRESULT *pResult);
	stAutoGrab* m_stAutoGrabInfo;

	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonStop();
	CProgressCtrl m_Progress;
	CStatic m_State;
	CStatic m_CurrentPattern;
	void m_fnInitProgress(int nMin, int nMax) {m_Progress.SetRange32(nMin,nMax); }
	void m_fnChangeProgress(int nValue) {m_Progress.SetPos(nValue); }
};
static UINT ThreadAutoGrab(LPVOID lParam);
extern CVSCameraTaskApp theApp;