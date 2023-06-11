
// AFTS_SimplifiedExample_MFCDlg.h: 헤더 파일
//

#pragma once
#include "Library/AFTSClass.h"
#define AFTSDEVICE_NUMBER 8 // 사용하려는 AF Z축 개수만큼 DEFINE해서 사용자가 쓰기. 
// CAFTSSimplifiedExampleMFCDlg 대화 상자
class CAFTSSimplifiedExampleMFCDlg : public CDialogEx
{
// 생성입니다.
public:
	CAFTSSimplifiedExampleMFCDlg(void* p_AFTSHandle, AFTSEventCallback Callback, int p_DeviceIndex , CWnd* pParent = nullptr );	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AFTS_SIMPLIFIEDEXAMPLE_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
	

	AFTSSingleTone* m_AFTSSingleTone;
	AFTSControlLibrary*  m_AFTSClass[AFTSDEVICE_NUMBER];
	AFTSControlLibrary*  m_pAFTSClass;
	IMAGEDATA			m_MainImageData; //ImageData Struct

	AFTSEventCallback* pEventCallback;
	static void ImageCallback(IMAGEDATA* _Data);
	void ImageProcessing(IMAGEDATA* _Data);

	static void EventCallback(EVENTDATA* _Data);
	void EventCallbackMain(EVENTDATA* _Data);

	static void AFTSParamCallback(AFTSParam* _Data);
	void AFTSParamCallbackMain(AFTSParam* _Data);

	bool m_bAFTSDiscovery;
	bool m_bAFCAMOpened;

	int nDeviceCount;
	void* _AFTSHandle;

	int m_AxisNum;
	HTREEITEM AFTSRoot, AFTSChild;

	
	void			Addlog_ListCtrlAFTS(CString Info);

	int				m_LogIndex;
	CString			AddString(char* msg);
	char*			AddChar(CString msg);
	CString			GetTimeInfo();
	CString			UpLogIndex();
	CCriticalSection cs;

	
	CString Unicode_char_to_CString_Convert(char *data);
	char* CString_to_uni_char_Convert(CString data);

	void RefreshAFParams();
protected:
	HICON m_hIcon;

	UINT m_radioTaskMode;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAftsdiscovery();
	afx_msg void OnBnClickedAftshandledestroy();
	afx_msg bool OnBnClickedAftsopen();
	afx_msg bool OnBnClickedAftsclose();
	afx_msg void OnBnClickedAfswitch();
	afx_msg void OnBnClickedZaxishoming();
	afx_msg void OnBnClickedZaxisup();
	afx_msg void OnBnClickedZaxisdown();
	afx_msg void OnBnClickedGetafcamexposure();
	afx_msg void OnBnClickedGetafcamzoffset();
	afx_msg void OnBnClickedSetafcamexposure();
	afx_msg void OnBnClickedSetafcamzoffset();
	afx_msg bool OnAfswitch();
	afx_msg bool OffAfswitch();
	//afx_msg void RadioCtrl(UINT ID);

	void			Addlog_ListCtrl(CString Date, CString Info);

	CListCtrl m_EventLogListctrl;
	//CIPAddressCtrl m_ACSIPAddressCtrl;
	//CTreeCtrl m_AFTSTreeCtrl;
	//afx_msg void OnTvnSelchangedAftstree(NMHDR *pNMHDR, LRESULT *pResult);
	//CListCtrl m_AFTSListControl;
	afx_msg void OnLvnItemchangedAftslist(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_AFModeBtn;
	CButton m_MeasureModeBtn;

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	CMFCButton m_AFSwitchLED;
	CMFCButton m_HWLeftLimitLED;
	CMFCButton m_HWRightLimitLED;
	afx_msg void OnDestroy();

	int m_nTaskMode;
	bool m_Dlginit_Flag;

	afx_msg int mfn_MoveZAxis( double zPos);
	afx_msg void mfn_DlgInit();

//	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
