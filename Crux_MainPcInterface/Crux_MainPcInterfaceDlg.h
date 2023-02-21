
// Crux_MainPcInterfaceDlg.h : 헤더 파일
//

#pragma once

#include "TaskInterfacer.h"
#include "MainPcSimulator.h"

#define MAX_GRID_LOG					1000

// CCrux_MainPcInterfaceDlg 대화 상자
class CCrux_MainPcInterfaceDlg : public CDialogEx
{
// 생성입니다.
public:
	CCrux_MainPcInterfaceDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CCrux_MainPcInterfaceDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CRUX_MAINPCINTERFACE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

private:
	int	m_fnConectVisualStation();
	int m_fnTrayIconRefresh();
	double ReadFloat(char* szSection, char* szKey, float fltDefaultValue);
	HANDLE m_hProcessLockMutex;		// 중복 실행 방지 뮤텍스
	TaskInterfacer* MainPcInterface;
	CListBox m_ctrlListLog;
	CMainPcSimulator* m_pMainPcSimulator;

	//BOOL m_bSimulationProgramMode;		//Program 이 Simultor로 구동 되어야 하는지 Flag 
	BOOL m_bSimulationMode;				//Crux 가 Simulation Mode 로 구동되어야 하는지 Flag

// 구현입니다.
protected:
	HICON m_hIcon;
	BOOL m_bTrayStatus;

	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();

	LRESULT m_fnPrintUiMsgUnicode(WPARAM wParam, LPARAM lParam);	
	LRESULT m_fnPrintUiMsgMultiByte(WPARAM wParam, LPARAM lParam);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnStartSimulator();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
};
