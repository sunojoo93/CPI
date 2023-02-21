#pragma once
#include "afxwin.h"
#include ".\Grid\GridCtrl.h"

// CDlgLuminanceStatus ��ȭ �����Դϴ�.
const int CAM_COUNT = 6; //2015.05.11 edit 4 -> 10

class CDlgLuminanceState : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLuminanceState)
	
	

public:
	CDlgLuminanceState(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgLuminanceState();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_LUMINANCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
private:
	CGridCtrl m_GridLuminanceState;
	int m_nMin[CAM_COUNT];
	int m_nMax[CAM_COUNT];

private:
	int m_fnSendAlarmToPLC(int nVisionID, int nCamNo);		//PLC �� �˶� ���� ���, �۾� �ʿ� Todo..
	
public:
	virtual BOOL OnInitDialog();
	int m_fnGetLimitValue();
	int m_fnSetLuminanceValue(int nVisionID, int nCamNo, int nValue);
};
