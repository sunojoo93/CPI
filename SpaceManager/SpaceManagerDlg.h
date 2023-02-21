
// SpaceManagerDlg.h : ��� ����
//

#pragma once

#define INIT_FILE_PATH	_T("D:\\Crux\\DATA\\INI\\SpaceManager.ini")

#define ORG_FILE_PATH			_T("D:\\Crux\\DATA\\INI\\Language\\SpaceManager")
#define ORG_RESOURCE_PATH		_T("D:\\Crux\\Data\\INI\\Language\\resource.h")
#define ORG_LANG_KOREAN_PATH	_T("D:\\Crux\\Data\\INI\\Language\\Korean.ini")
#define ORG_LANG_CHINESE_PATH	_T("D:\\Crux\\Data\\INI\\Language\\Chinese.ini")
#define RESOURCE_PATH			_T("D:\\Crux\\Data\\INI\\Language\\SpaceManager\\resource.h")
#define LANG_KOREAN_PATH		_T("D:\\Crux\\Data\\INI\\Language\\SpaceManager\\Korean.ini")
#define LANG_CHINESE_PATH		_T("D:\\Crux\\Data\\INI\\Language\\SpaceManager\\Chinese.ini")

#define	WM_TRAYICON_MSG	WM_USER + 1

#include "FileManagement.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>
#include "XMultiLang.h"

enum {		
	// Timer index
	eTIMER_DriveCheck = 0 
};

#define LANG(x)					Lang(_T(#x), _T("TEXT"))

// CSpaceManagerDlg ��ȭ ����
class CSpaceManagerDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CSpaceManagerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SPACEMANAGER_DIALOG };	

	CFileManagement m_FileManager;

private:
	
	XMultiLang g_multilang;
	LPCTSTR Lang(LPCTSTR szLangKey, LPCTSTR szSection=_T("TEXT") );

	BOOL		m_fnLoadInitFile();
	void		m_fnInitDriveList();
	void		m_fnUpdateDriveList();	
	CString		m_fnGetItem(TCHAR *sec, CString key, TCHAR *def);
	CString		m_fnGetStr(TCHAR* Sec, CString key, CString def);
	void		m_fnUpdateDeletePath(int nID);
	void		m_fnCheckCondition();
	void        ChangeLanguage(int nLan);
	
	int m_nCheckCycle;
	int m_nDeleteSleep;
	int m_nTimeLate;
	int m_nMaxSpace;
	int m_nMinSpace;
	int m_nStepTimeLate;	// ����2 ���� �ð� �߰�
	int m_nDeleteStep;
	int m_nExTimeLate;		// ���� ���� �ð� �߰�
	int m_nFormatMaxSpace;

	std::vector<CString>	m_vstrMonitoringPath;
	std::vector<CString>	m_vstrExMonitoringPath;	//���� ���� Path
	std::vector<DriveInfo>	m_vDriveInfoDelete;
	std::vector<DriveInfo>	m_vDriveInfoFormat;

	int						m_nTimerCount;		// UI ���� Timer ���� Count	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.	

	// Ʈ���� ������ �߰�
	LRESULT TrayIconMessage(WPARAM wParam, LPARAM lParam);
	void TraySetting();
	BOOL m_bTrayStatus;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_listCtrlDeleteDrive;
	afx_msg void OnItemchangedListUsedDeleteDrive(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemchangedListUsedFormatDrive(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnFindPath1();
	afx_msg void OnBnClickedBtnFindPath2();
	afx_msg void OnBnClickedBtnFindPath3();
	afx_msg void OnBnClickedBtnFindPath4();
	afx_msg void OnBnClickedBtnFindPath5();
	CButton m_chkSubDir;
	afx_msg void OnBnClickedBtnFindPath6();
	afx_msg void OnBnClickedBtnFindPath7();
	afx_msg void OnBnClickedBtnFindPath8();
	afx_msg void OnBnClickedBtnFindPath9();
	afx_msg void OnBnClickedBtnFindPath10();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnLanRadioBtn(UINT ID);
	int m_nLanguage;
	CListCtrl m_listCtrlFormatDrive;
};
