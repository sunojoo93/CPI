#pragma once
#include "actutltype.h"
#include "ActUtlType_i.h"
#include "resource.h"
#include "FtpClient.h"	

//2017.04.20	Mr.Chu		각 분산된 Tap 컨트롤에 있는 Def Arr 관련된 항목 갯수
#define		MAX_DEFECT_KIND_PARAM_UI_ONE	34
#define		MAX_DEFECT_KIND_PARAM_UI_TWO	2
//

enum {
	TIME_OVERLAP_CHECK	= 0,
	TIME_OVERLAP_TERM			,
	TIME_OVERLAP_ALARM_COUNT	,
	TIME_OVERLAP_STOP_COUNT		,
	TIME_OVERLAP_ITEM_MAX
};

struct STRU_PLC_MAP_PC
{
	BOOL bAlive;
	BOOL bAlarm;
	BOOL bGrabReady;
	BOOL bForward;
	BOOL bGrabAck;
	BOOL bGrabEnd;
	BOOL bGrabError;
	BOOL bPPIDMismatch;
	BOOL bSeqError;

	STRU_PLC_MAP_PC()
	{
		bAlive = FALSE;
		bAlarm = FALSE;
		bGrabReady = FALSE;
		bForward = FALSE;
		bGrabAck = FALSE;
		bGrabEnd = FALSE;
		bGrabError = FALSE;
		bPPIDMismatch = FALSE;
		bSeqError = FALSE;
	}
};

struct STRU_PLC_MAP_PLC
{
	BOOL bAlive;
	BOOL bGrabReq;
	BOOL bScanForward;
	BOOL bGrabEndOK;
	BOOL bReset;

	CString strModelID;
	CString strPanelID_1;
	CString strPanelID_2;

	STRU_PLC_MAP_PLC()
	{
		bAlive = FALSE;
		bGrabReq = FALSE;
		bScanForward = FALSE;
		bGrabEndOK = FALSE;
		bReset = FALSE;
	}
};

// CDlgPLCSetup 대화 상자입니다.

class CDlgPLCSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgPLCSetup)

private:
	
	//CRITICAL_SECTION csMXComponent;

	//CFtpClient *FtpDstClient;
	//CFtpClient *FtpImgClient;
public:
	CDlgPLCSetup(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPLCSetup();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_PLC_SETUP };

	//2015.06.02 
	void m_fnLoadParameter();
	void m_fnSaveParameter();
	void m_fnOverlapUseCheck(BOOL bUseCheck);
	//2015.06.02

private:
	CString m_strRoot;
	int		m_nCmdType;
	int		m_nEqpObject;
	int	m_fnGetDeviceFromPlcTask					(TCHAR *  strAddr, short* psData);
	int	m_fnGetDeviceDwordFromPlcTask				(TCHAR *  strAddr, long* psData);
	int m_fnGetDeviceBlockDwordFromPlcTask			(TCHAR *  strAddr, int nSize, LONG *lplData);
	int	m_fnSetDeviceFromPlcTask					(TCHAR *  strAddr, short psData);
	int	m_fnSetDeviceDwordFromPlcTask				(TCHAR *  strAddr, long psData);
	int m_fnSetDeviceBlockDwordFromPlcTask			(TCHAR *  strAddr, int nSize, LONG *lplData);
	int	m_fnSetDeviceAutoClearFromPlcTask			(TCHAR *  strAddr, short psData, ULONG lWaitTime = 1000);
	int	m_fnSetDeviceDwordAutoClearFromPlcTask		(TCHAR *  strAddr, long psData, ULONG lWaitTime = 1000);
	int	m_fnGetDeviceDwordFromPlcTaskModel			(TCHAR *  strAddr, long* psData);
	int	m_fnSetDeviceDwordFromPlcTaskModel			(TCHAR *  strAddr, long psData);
	CString m_fnConvertHexToDec						(TCHAR *  strAddr);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bOpenPLC;
	//HANDLE hMutexPlcDevice;
	long m_fnOpenPLC();
	long m_fnClosePLC();

	long m_fnGetDevice(TCHAR * strAddr);
	short m_fnGetDevice2(TCHAR * strAddr);
	long m_fnGetDeviceBlock(TCHAR * strAddr, int nSize, CString& strRet);
	long m_fnSetDevice(TCHAR * strAddr, long lVal);
	long m_fnSetDevice2(TCHAR * strAddr, short shVal);
	long m_fnSetDeviceAutoClear(TCHAR* strAddr, long lVal, ULONG lWaitTime = 3000);
	long m_fnSetDevice2AutoClear(TCHAR* strAddr, short IVal, ULONG lWaitTime = 3000);
	long m_fnSetDeviceBlock(TCHAR * strAddr, int nSize, CString strVal);
	long m_fnGetDeviceBlockModel(TCHAR * strAddr, int nSize, CString& strRet);
	long m_fnGetDeviceModel(TCHAR * strAddr);
	long m_fnSetDeviceModel(TCHAR * strAddr, long lVal);

#if PLC_HANDLE_TYPE
	CActutltype m_ActUtlType;
#else
	IActUtlType* m_ActUtlType;
#endif
	CString m_strRetCode;

	STRU_PLC_MAP_PC m_stPlcMapPC[MAX_VISION_PC];
	STRU_PLC_MAP_PLC m_stPlcMapPLC[MAX_VISION_PC];
	//..TEST 2015.02.03 add by ikm - S
	CString m_strFileName;
	int m_nEqpModel;
	//..TEST 2015.02.03 add by ikm - E

	//2015.06.02
	BOOL m_bUseOverlap;
	BOOL m_bAllOverlapDef;//2016.03.23 add by ikm
	int m_nAryDefKind[MAX_DEFECT_KIND];
	int m_nAryDefCount[MAX_DEFECT_KIND];
	//2016.11.21
	BOOL m_bArrLumiKind[LUMINANCE_TYPE_MAX];
	int m_nArrLumiCount[LUMINANCE_TYPE_MAX];

	//2020.02.28 add by mby
	BOOL m_bArrBubbleStationKind[BUBBLESTATION_MAX];
	int m_nArrBubbleStationCount[BUBBLESTATION_MAX];
	DOUBLE m_dOffX;
	DOUBLE m_dOffY;
	BOOL m_bUseContactFail;
	int m_nContactFailCount;
	//2015.06.02

	int m_nPanelCount;

	//2016.08.22
	BOOL m_bUseAlignFail;
	int m_nAlignFailCount;
	//2016.08.22

	//2019.03.21 - - - S
	BOOL m_bUseBgrade;
	float m_fBgradeTerm;
	int m_nBgradeSetCount;
	int m_nBgradeStopCount;

	BOOL m_bUseCgrade;
	float m_fCgradeTerm;
	int m_nCgradeSetCount;
	int m_nCgradeStopCount;
	//2019.03.21 - - - E

	////2019.03.20
	//BOOL m_bUseBgrade;
	//int m_nBgradeCheckCount;
	//BOOL m_bUseCgrade;
	//int m_nCgradeCheckCount;
	////2019.03.20

	//2016.02.24
	BOOL m_bUsePolMiss;
	int m_nPolMissCount;
	//2016.02.24

	virtual BOOL OnInitDialog();

	DECLARE_EVENTSINK_MAP()
	void ClickGxbtnPlcOpen();
	void ClickGxbtnPlcClose();
	
	afx_msg void OnBnClickedBtnGetdevice();
	afx_msg void OnBnClickedBtnSetdevice();
	afx_msg void OnBnClickedBtnReadblock();
	afx_msg void OnBnClickedBtnWriteblock();
	afx_msg void OnBnClickedBtnSetPcalive();
	afx_msg void OnBnClickedBtnSetReadyS1();
	afx_msg void OnBnClickedBtnSetReadyS2();
	afx_msg void OnBnClickedBtnSetReadyS3();
	afx_msg void OnBnClickedBtnSetForwardS1();
	afx_msg void OnBnClickedBtnSetForwardS2();
	afx_msg void OnBnClickedBtnSetForwardS3();
	afx_msg void OnBnClickedBtnSetAckS1();
	afx_msg void OnBnClickedBtnSetAckS2();
	afx_msg void OnBnClickedBtnSetAckS3();
	afx_msg void OnBnClickedBtnSetEndS1();
	afx_msg void OnBnClickedBtnSetEndS2();
	afx_msg void OnBnClickedBtnSetEndS3();
	afx_msg void OnBnClickedBtnSetErrorS1();
	afx_msg void OnBnClickedBtnSetErrorS2();
	afx_msg void OnBnClickedBtnSetErrorS3();
	afx_msg void OnBnClickedBtnSetPpidMis1();
	afx_msg void OnBnClickedBtnSetPpidMis2();
	afx_msg void OnBnClickedBtnSetPpidMis3();
	afx_msg void OnBnClickedBtnSetResetS1();
	afx_msg void OnBnClickedBtnSetResetS2();
	afx_msg void OnBnClickedBtnSetResetS3();
	afx_msg void OnBnClickedBtnSetPcalarmS1();
	afx_msg void OnBnClickedBtnSetPcalarmS2();
	afx_msg void OnBnClickedBtnSetPcalarmS3();
	afx_msg void OnBnClickedBtnSetPcalive2();
	afx_msg void OnBnClickedBtnSetPcalive3();
	afx_msg void OnBnClickedBtnSetCycleS1();
	afx_msg void OnBnClickedBtnSetCycleS2();
	afx_msg void OnBnClickedBtnSetCycleS3();
	afx_msg void OnBnClickedButton2();

	virtual void OnOK();
	afx_msg void OnBnClickedBtnRcpEdit();
	afx_msg void OnBnClickedBtnSetRcp();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnRcpUp();
	afx_msg void OnBnClickedBtnRcpDown();
	afx_msg void OnBnClickedBtnProcInfo();
	afx_msg void OnBnClickedBtnProcData();
	afx_msg void OnBnClickedBtnRcpChg();
	afx_msg void OnBnClickedCheckOverlapUse();
	afx_msg void OnBnClickedCheckAllOverlapDef(); //2016.03.23 add by ikm
	//int m_nPanelCount;

};









///////////////////////////////////////////////////////////////////////////////////////////
class CDlgDEFSetupParam1 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDEFSetupParam1)

private:

public:
	CDlgDEFSetupParam1(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDEFSetupParam1();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DEF_SETUP_PARAM_1 };

	int m_nAryDefKind[MAX_DEFECT_KIND_PARAM_UI_ONE];
	int m_nAryDefCount[MAX_DEFECT_KIND_PARAM_UI_ONE];
	int m_nAryDefAlarmCount[MAX_DEFECT_KIND_PARAM_UI_ONE];
	float m_nAryDefTerm[MAX_DEFECT_KIND_PARAM_UI_ONE];

	BOOL m_bUseContactFail;
	int m_nContactFailStopCount;
	float m_fContactFailTerm;
	int m_nContactFailSetCount;

	//2016.02.24
	BOOL m_bUsePolMiss;
	int m_nPolMissStopCount;
	float m_fPolMissTerm;
	int m_nPolMissSetCount;

	//2016.08.22
	BOOL m_bUseAlignFail;
	float m_fAlignFailTerm;
	int m_nAlignFailSetCount;
	int m_nAlignFailStopCount;

public:
	void m_fnOverlapUseCheck(BOOL bUseCheck, int nEqpModel);
	void m_fnLoadParameter( int *nDefArrCurPos );
	void m_fnSaveParameter( CString *strDefArr );

private:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//DECLARE_EVENTSINK_MAP()
};


///////////////////////////////////////////////////////////////////////////////////////////
class CDlgDEFSetupParam2 : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDEFSetupParam2)

private:

public:
	CDlgDEFSetupParam2(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDEFSetupParam2();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DEF_SETUP_PARAM_2 };

	BOOL m_bAryLumiKind[LUMINANCE_TYPE_MAX];
	int m_nAryLumiDefCount[LUMINANCE_TYPE_MAX];
	int m_nAryLumiAlarmCount[LUMINANCE_TYPE_MAX];
	float m_nAryLumiTerm[LUMINANCE_TYPE_MAX];

	//2017.04.20
	int m_nAryDefKind[MAX_DEFECT_KIND_PARAM_UI_TWO];
	int m_nAryDefCount[MAX_DEFECT_KIND_PARAM_UI_TWO];
	int m_nAryDefAlarmCount[MAX_DEFECT_KIND_PARAM_UI_TWO];
	float m_nAryDefTerm[MAX_DEFECT_KIND_PARAM_UI_TWO];
	//
	//2019.03.21 - - - S
	BOOL m_bUseBgrade;
	float m_fBgradeTerm;
	int m_nBgradeSetCount;
	int m_nBgradeStopCount;

	BOOL m_bUseCgrade;
	float m_fCgradeTerm;
	int m_nCgradeSetCount;
	int m_nCgradeStopCount;
	//2019.03.21 - - - E
	//2020.02.28 add by mby
	BOOL m_bAryBubbleStationKind[BUBBLESTATION_MAX];
	int m_nAryBubbleStationDefCount[BUBBLESTATION_MAX];
	int m_nAryBubbleStationAlarmCount[BUBBLESTATION_MAX];
	float m_nAryBubbleStationTerm[BUBBLESTATION_MAX];

public:
	void m_fnOverlapUseCheck(BOOL bUseCheck, int nEqpModel);
	void m_fnLoadParameter( int *nDefArrCurPos );
	void m_fnSaveParameter( CString *strDefArr );

private:

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//DECLARE_EVENTSINK_MAP()
};

/////////////////////////////////////////////////////////////////////////////////////


class CDlgDEFSetup : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDEFSetup)

public:
	CDlgDEFSetup(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDEFSetup();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DEF_SETUP };

	BOOL m_bUseOverlap;

	//2015.06.02 
	void m_fnLoadParameter();
	void m_fnSaveParameter();
	void m_fnOverlapUseCheck(BOOL bUseCheck);
	//2015.06.02

private:
	CDlgDEFSetupParam1 *m_DlgTimeParam_One;	//2016.11.21	Mr.Chu
	CDlgDEFSetupParam2 *m_DlgTimeParam_Two; //2016.11.21	Mr.Chu

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	
	afx_msg void OnBnClickedCheckOverlapUse();
	
	afx_msg void OnBnClickedBtnSave();
	CTabCtrl m_ctrlTabTimeOverlab;
	afx_msg void OnTcnSelchangeTabParam(NMHDR *pNMHDR, LRESULT *pResult);

public:
	CString *m_strArrDefItem;	// Maximun Count = TIME_OVERLAP_ITEM_MAX	2017.04.20 Mr.Chu
};


