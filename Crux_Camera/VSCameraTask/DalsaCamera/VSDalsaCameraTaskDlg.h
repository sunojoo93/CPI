
// VSCameraTaskDlg.h : 헤더 파일
//

#pragma once
//#include "AutoGrab.h"
#include "afxwin.h"
// for Tray Icon - User Message
#define	WM_TRAYICON_MSG	WM_USER + 1

// Timer
enum {
	eTIMER_UpdateTaskStatus	= 0,	// Task 상태 UI 표시 Timer
};

#define LIGHT_RED	RGB(255,153,153)
#define DARK_RED	RGB(102,0,0)
#define LIGHT_GREEN	RGB(204,255,153)
#define DARK_GREEN	RGB(0,51,0)
#define DARK_GRAY	RGB(51,51,51)
#define WHITE		RGB(255,255,255)
#define BLACK		RGB(0,0,0)

// CVSCameraTaskDlg 대화 상자
class CVSCameraTaskDlg : public CDialogEx
{
// 생성입니다.
public:
	CVSCameraTaskDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VSCameraTask_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.	


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
private:
	// 트레이 아이콘 추가
	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();
	BOOL m_bTrayStatus;
	//CAutoGrab*		m_pDlgAutoGrab;
	CWinThread*		m_pThreadMonitoring;					// Camera Monitoring Thread	
private:
	void m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText);	
	void m_fnInitUIControls();
	void m_fnSetDefaultUIControls();
	void m_fnSetAutoUIControls(CString strInitFilePath);
	void m_fnRefreshCmbChannel();
	static UINT	ThreadMonitoring(LPVOID pParam);		// Camera Monitoring Thread
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedBtnSaveImage();
	afx_msg void OnBnClickedBtnIniFilePath();
	afx_msg void OnBnClickedBtnDcfFilePath();
	CComboBox m_cmbCamModel;
	void m_fnAddCmbString(CComboBox& ComboBox, const CString* strItemArr, int nItemCount);
	CComboBox m_cmbCamColor;
	CComboBox m_cmbBaudrate;
	CComboBox m_cmbTrigMode;
	CComboBox m_cmbPSMode;
	afx_msg void OnBnClickedBtnApplyAll();
	afx_msg void OnBnClickedBtnOpenPort();
	afx_msg void OnBnClickedBtnDcfApply();
	afx_msg void OnBnClickedBtnSettingApply();
	CComboBox m_cmbGrabberNum;
	CComboBox m_cmbGrabberCH;
	afx_msg void OnCbnSelchangeCmbGrabberNumber();
	CButton m_chkVertical;
	CButton m_chkHorizontal;
	afx_msg void OnBnClickedBtnLive();
	afx_msg void OnBnClickedBtnGrab();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeCmbModel();
	afx_msg void OnBnClickedGetCameraCondition();
	afx_msg void OnBnClickedButtonAutoGrab();
	int		m_nGV;	
	bool	m_bMonitoring;							// Monitoring Thread 동작 여부
	int		m_nMonitoringInterval;					// Camera 상태 체크 간격
	CEdit	m_ctrlInterval;							// 카메라 상태 체크 주기
	CEdit	m_ctrlTemperature;
	CEdit	m_ctrlFanRPM;
	afx_msg void OnBnClickedBtnSettingApply2();
};
