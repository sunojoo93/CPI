#pragma once
#include "Library/AFTSClass.h"
#include "AFTS_SimplifiedExample_MFCDlg.h"
#include "afxwin.h"

#define AFTSDEVICE_NUMBER 8 // 사용하려는 AF Z축 개수만큼 DEFINE해서 사용자가 쓰기. 
// CMainDlg 대화 상자

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)



public:
	CMainDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMainDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN };
#endif

protected:
	CTabCtrl m_ctlTab;
	CAFTSSimplifiedExampleMFCDlg* m_TapDlg1;

	CRect m_Image_Rect;
	CImage m_Image;
	void InitControls();

	//////////////////////////////////
	AFTSSingleTone* m_AFTSSingleTone;
	CAFTSSimplifiedExampleMFCDlg* m_AFTS_Dlg_Class[AFTSDEVICE_NUMBER];

	static void EventCallback(EVENTDATA* _Data);
	void EventCallbackMain(EVENTDATA* _Data);

	int nDeviceCount;
	void* _AFTSHandle;

	int m_AxisNum;
	int	m_LogIndex;

	bool m_bAFTSDiscovery;

	void	Addlog_ListCtrlAFTS(CString Info);
	void	AddDevlog_ListCtrlAFTS(CString Date, int Dev_Num, CString Info);

	char* CString_to_uni_char_Convert(CString data);
	CString		AddString(char* msg);

	CString Unicode_char_to_CString_Convert(char *data);

	CString			GetTimeInfo();

	int CListctr_clickindex ;
	//////////////////////////////////

protected:
	HICON m_hIcon;

	UINT m_radioTaskMode;

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:

	
	CIPAddressCtrl m_ACSIPAddressCtrl;
	CListCtrl m_AFTSListControl;

	CButton m_TaskMode_Auto; 
	CButton m_TaskMode_Setting;

	int m_nTaskMode;
	bool m_Dlginit_Flag;

	CString strTab_Name[AFTSDEVICE_NUMBER] = {_T("상면 Left"),_T("상면 Right"), _T("후면 Left"), _T("후면 Right"), _T(""), _T(""), _T(""), _T("") }; //AFTSDEVICE_NUMBER 만큼 초기화

	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAftsdiscovery();
	afx_msg void OnBnClickedAftsdiscovery_Auto();
	afx_msg void RadioCtrl(UINT ID);

	afx_msg void OnLvnItemchangedAftslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedAftshandledestroy();
	afx_msg void OnBnClickedAftsopen();
	afx_msg void OnNMClickAftslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int	 mfn_AutoFocus(int index , double zPos);
	afx_msg bool initDevice(int nDev_Count);
	afx_msg void OnBnClickedAftsclose();
	afx_msg void PingTest(int index);
	CStatic m_Picture_Ctrl;
	afx_msg void OnPaint();
};