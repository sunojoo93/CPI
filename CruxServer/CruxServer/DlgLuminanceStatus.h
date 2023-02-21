#pragma once
#include "afxwin.h"
#include ".\Grid\GridCtrl.h"

// CDlgLuminanceStatus 대화 상자입니다.
const int CAM_COUNT = 6; //2015.05.11 edit 4 -> 10

class CDlgLuminanceState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLuminanceState)
	
	

public:
	CDlgLuminanceState(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgLuminanceState();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_LUMINANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CGridCtrl m_GridLuminanceState;
	int m_nMin[CAM_COUNT];
	int m_nMax[CAM_COUNT];

private:
	int m_fnSendAlarmToPLC(int nVisionID, int nCamNo);		//PLC 로 알람 전달 기능, 작업 필요 Todo..
	
public:
	virtual BOOL OnInitDialog();
	int m_fnGetLimitValue();
	int m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue);
};
