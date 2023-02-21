#pragma once
#include "afxcmn.h"

class CDlgJudgeUIParam1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgJudgeUIParam1)

private:

	//CRITICAL_SECTION csMXComponent;

	//CFtpClient *FtpDstClient;
	//CFtpClient *FtpImgClient;
public:
	CDlgJudgeUIParam1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgJudgeUIParam1();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_JUDGE_UI_PARAM_1 };

private:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nAryDefCount[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)]; //28 -> 34
	int m_nAryDefLuminance[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryDistanceD1[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryDistanceD2[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryLength[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryThinknessW1[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryThinknessW2[INSP_TYPE_MAX - (INSP_ETC_POINT-INSP_TFT_BURR + 1)];//28 -> 34

};


class CDlgJudgeUIParam2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgJudgeUIParam2)

private:

	//CRITICAL_SECTION csMXComponent;

	//CFtpClient *FtpDstClient;
	//CFtpClient *FtpImgClient;
public:
	CDlgJudgeUIParam2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgJudgeUIParam2();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_JUDGE_UI_PARAM_2 };

private:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nAryDefCount[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)]; //28 -> 34
	int m_nAryDefLuminance[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryDistanceD1[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryDistanceD2[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryLength[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryThinknessW1[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34
	DOUBLE m_dAryThinknessW2[INSP_TYPE_MAX - (INSP_TFT_BURR + 1)];//28 -> 34

};

// CDlgJudgeUI 대화 상자입니다.

class CDlgJudgeUI : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgJudgeUI)

public:
	CDlgJudgeUI(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgJudgeUI();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_JUDGE_UI };
	void m_fnLoadParameter();
	void m_fnDataTerminal();		//2016.10.10 저장하기 위해 기존 변수에 Tab 데이터를 옮긴다.
	void m_fnSaveJudgeCount();
	void m_fnJudgeCountUseCheck(BOOL bUseCheck);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


private:
	CDlgJudgeUIParam1 *m_DlgParam_One;
	CDlgJudgeUIParam2 *m_DlgParam_Two;

public:
	BOOL m_bUseJudgeCount;
	int m_nAryDefCount[INSP_TYPE_MAX - 1]; //28 -> 34
	int m_nAryDefLuminance[INSP_TYPE_MAX - 1];//28 -> 34
	DOUBLE m_dAryDistanceD1[INSP_TYPE_MAX - 1];//28 -> 34
	DOUBLE m_dAryDistanceD2[INSP_TYPE_MAX - 1];//28 -> 34
	DOUBLE m_dAryLength[INSP_TYPE_MAX - 1];//28 -> 34
	DOUBLE m_dAryThinknessW1[INSP_TYPE_MAX - 1];//28 -> 34
	DOUBLE m_dAryThinknessW2[INSP_TYPE_MAX - 1];//28 -> 34

	afx_msg void OnBnClickedCheckJudgeCountUse();
	CTabCtrl m_ctrlTabJudgeParam;
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeTabParam(NMHDR *pNMHDR, LRESULT *pResult);

};
