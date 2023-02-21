
// SpaceManagerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SpaceManager.h"
#include "SpaceManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpaceManagerDlg ��ȭ ����




CSpaceManagerDlg::CSpaceManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpaceManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nLanguage = 0;
}

void CSpaceManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USED_DRIVE, m_listCtrlDeleteDrive);
	DDX_Control(pDX, IDC_CHK_SUB_DIRECTORY, m_chkSubDir);
	DDX_Radio(pDX, IDC_RADIO_CNA, m_nLanguage);
	DDX_Control(pDX, IDC_LIST_USED_DRIVE_FORMAT, m_listCtrlFormatDrive);
}

BEGIN_MESSAGE_MAP(CSpaceManagerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USED_DRIVE, &CSpaceManagerDlg::OnItemchangedListUsedDeleteDrive)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USED_DRIVE_FORMAT, &CSpaceManagerDlg::OnItemchangedListUsedFormatDrive)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CSpaceManagerDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH1, &CSpaceManagerDlg::OnBnClickedBtnFindPath1)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH2, &CSpaceManagerDlg::OnBnClickedBtnFindPath2)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH3, &CSpaceManagerDlg::OnBnClickedBtnFindPath3)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH4, &CSpaceManagerDlg::OnBnClickedBtnFindPath4)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH5, &CSpaceManagerDlg::OnBnClickedBtnFindPath5)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH6, &CSpaceManagerDlg::OnBnClickedBtnFindPath6)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH7, &CSpaceManagerDlg::OnBnClickedBtnFindPath7)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH8, &CSpaceManagerDlg::OnBnClickedBtnFindPath8)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH9, &CSpaceManagerDlg::OnBnClickedBtnFindPath9)
	ON_BN_CLICKED(IDC_BTN_FIND_PATH10, &CSpaceManagerDlg::OnBnClickedBtnFindPath10)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DESTROY()
	ON_COMMAND_RANGE(IDC_RADIO_CNA, IDC_RADIO_KOR, OnLanRadioBtn)
END_MESSAGE_MAP()


// CSpaceManagerDlg �޽��� ó����

BOOL CSpaceManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	ShowWindow(SW_MINIMIZE);

	m_bTrayStatus = FALSE;
	TraySetting();	

	theApp.m_fnWriteLog(_T(">>>>>>>> SpaceManager Start <<<<<<<<"));	

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	 m_FileManager.m_fnGetDiskStatus(m_vDriveInfoDelete,0);
	 m_FileManager.m_fnGetDiskStatus(m_vDriveInfoFormat,1);

	BOOL bRet = m_fnLoadInitFile();		
	m_fnInitDriveList();

	//ChangeLanguage(m_nLanguage);

	m_nTimerCount = m_nCheckCycle;	// ���� ���� �� �� ����

	SetTimer(eTIMER_DriveCheck, 1000, NULL);	// 1�ʸ��� UI ����
	
	SendMessage(WM_TIMER, eTIMER_DriveCheck, 0);	// ���α׷� ���� �� ����

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CSpaceManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CSpaceManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSpaceManagerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch (nIDEvent) 
	{
	case eTIMER_DriveCheck :
		if (m_vDriveInfoDelete.size() == 0 || (m_vstrMonitoringPath.size() == 0 && m_vstrExMonitoringPath.size() == 0))
		{
			theApp.m_fnWriteLog(_T("== Timer Abnormal End =="));
			//break;
		}
		m_nTimerCount++;

		if((m_vstrMonitoringPath.size() != 0)){
			m_FileManager.m_fnUpdateDiskStatus(m_vDriveInfoDelete, m_vstrMonitoringPath);
		}
		if((m_vstrExMonitoringPath.size() != 0)){
			m_FileManager.m_fnUpdateDiskStatus(m_vDriveInfoDelete, m_vstrExMonitoringPath);
		}

		m_FileManager.m_fnUpdateDiskStatus(m_vDriveInfoFormat);
		
		if (m_nTimerCount > m_nCheckCycle)
		{
			m_FileManager.m_fnApplyDeleteRules(m_vDriveInfoDelete, m_vstrMonitoringPath, m_nTimeLate, m_nMaxSpace, m_nMinSpace, m_nStepTimeLate, m_nDeleteStep, m_chkSubDir.GetCheck(), m_nDeleteSleep, m_nExTimeLate, m_vstrExMonitoringPath);
			m_FileManager.m_fnApplyFormatRules(m_vDriveInfoFormat, m_nFormatMaxSpace);
			m_nTimerCount = 0;
		}
				
		m_fnUpdateDriveList();
		break;

	}
	CDialogEx::OnTimer(nIDEvent);
}

BOOL CSpaceManagerDlg::m_fnLoadInitFile()
{
	TCHAR strBuffer[256];
	CString strEdit;

	// Timer Elapse
	GetPrivateProfileString(_T("Timer"), _T("CheckCycle"), _T("10"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_CYCLE, strBuffer);
	m_nCheckCycle = _ttoi(strBuffer) * 60;

	theApp.m_fnWriteLog(_T("Timer Cycle : %s "), strBuffer);

	// ���� ���� �� Sleep
	GetPrivateProfileString(_T("Timer"), _T("DeleteSleep"), _T("0"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_SLEEP, strBuffer);
	m_nDeleteSleep = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("Delete Sleep : %s "), strBuffer);

	// ����1 : ���� �ð� ���� ���� ����
	GetPrivateProfileString(_T("Codition"), _T("TimeLate"), _T("48"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_COND_TIME, strBuffer);
	m_nTimeLate = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("TimeLate : %s "), strBuffer);

	// ��ü ����̺� �հ� �뷮�� ���� ���� �����ϸ� ������ ���� �������� ����
	GetPrivateProfileString(_T("Codition"), _T("MaxSpace"), _T("90"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_COND_SPACE_MAX, strBuffer);
	m_nMaxSpace = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("MaxSpace : %s "), strBuffer);

	GetPrivateProfileString(_T("Codition"), _T("MinSpace"), _T("80"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_COND_SPACE_MIN, strBuffer);
	m_nMinSpace = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("MinSpace : %s "), strBuffer);

	// ����2 : �뷮 Ȯ�� �� ���� ������ ���� �ð� - 2015.10.13 �߰�
	GetPrivateProfileString(_T("Codition"), _T("StepTimeLate"), _T("48"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_COND_STEP_TIME, strBuffer);
	m_nStepTimeLate = _ttoi(strBuffer);

	m_fnCheckCondition();

	theApp.m_fnWriteLog(_T("StepTimeLate : %s "), strBuffer);

	// �ѹ��� ���� �� (���� : �ð�)
	GetPrivateProfileString(_T("Codition"), _T("DeleteStep"), _T("12"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_COND_SPACE_DELETE, strBuffer);
	m_nDeleteStep = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("DeleteStep : %s "), strBuffer);

	//���� ���� �߰� 2016.04.08 add by mby
	GetPrivateProfileString(_T("Codition"), _T("ExTimeLate"), _T("48"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_EXCON_TIME, strBuffer);
	m_nExTimeLate = _ttoi(strBuffer);

	theApp.m_fnWriteLog(_T("ExTimeLate : %s "), strBuffer);

	GetPrivateProfileString(_T("Codition"), _T("FormatMaxSpace"), _T("48"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_FORMAT_MAX_SPACE, strBuffer);
	m_nFormatMaxSpace = _ttoi(strBuffer);
	m_FileManager.SetFormatMaxSpace(m_nFormatMaxSpace);

	GetPrivateProfileString(_T("TimeCheckPath"), _T("Path"), _T("Defect Info\\Origin"), strBuffer, 256, INIT_FILE_PATH);
	SetDlgItemTextW(IDC_EDIT_PATH, strBuffer);
	m_FileManager.SetTimeCheckPath(strBuffer);

	theApp.m_fnWriteLog(_T("FormatMaxSpace : %s "), strBuffer);

	memset(strBuffer, 0, sizeof(strBuffer));
	// ����̺� ����͸� ��� ����
	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoDelete.size(); nVIndex++)
	{
		m_vDriveInfoDelete[nVIndex].bUseManagement = 
			(m_fnGetStr(_T("DeleteDrive"),m_vDriveInfoDelete[nVIndex].strDriveName,_T("F")) == _T("T"))?TRUE:FALSE;
		if (m_vDriveInfoDelete[nVIndex].bUseManagement)
		{
			_tcscat_s(strBuffer, m_vDriveInfoDelete[nVIndex].strDriveName);
			_tcscat_s(strBuffer, _T(" "));
		}
		//m_FileManager.m_fnSetDriveUseFlag(nVIndex, m_vDriveInfoDeleteUI[nVIndex].bUseManagement);
	}

	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoFormat.size(); nVIndex++)
	{
		m_vDriveInfoFormat[nVIndex].bUseManagement = 
			(m_fnGetStr(_T("FormatDrive"),m_vDriveInfoFormat[nVIndex].strDriveName,_T("F")) == _T("T"))?TRUE:FALSE;
		if (m_vDriveInfoFormat[nVIndex].bUseManagement)
		{
			_tcscat_s(strBuffer, m_vDriveInfoFormat[nVIndex].strDriveName);
			_tcscat_s(strBuffer, _T(" "));
		}
		//m_FileManager.m_fnSetDriveUseFlag(nVIndex, m_vDriveInfoFormatUI[nVIndex].bUseManagement);
	}

	theApp.m_fnWriteLog(_T("Drive : %s "), strBuffer);

	// ���� ���� ����
	m_chkSubDir.SetCheck((m_fnGetStr(_T("Path"),_T("Use Sub Folder"),_T("T")) == _T("T"))?TRUE:FALSE);

	theApp.m_fnWriteLog(_T("Use Sub Folder : %s "), (m_fnGetStr(_T("Path"),_T("Use Sub Folder"),_T("T")) == _T("T"))?_T("TRUE"):_T("FALSE"));

	GetPrivateProfileString(_T("LanguageSet"), _T("Language"), _T("0"), strBuffer, 256, INIT_FILE_PATH);
	m_nLanguage = _ttoi(strBuffer);
	if(m_nLanguage<0 || m_nLanguage>1)
		m_nLanguage = 0;
	UpdateData(FALSE);

	// ����͸��� ����
	CString strPathIndex;
	CString strFullPath;

	//����� ��ΰ� �ߺ� �����Ǵ� ������ ���� Clear�߰� 2016.04.08 add by mby 
	m_vstrMonitoringPath.clear();
	m_vstrExMonitoringPath.clear();
	for(int nPathIndex = 0; nPathIndex < MAX_PATH_NUM; nPathIndex++)
	{
		strPathIndex.Format(_T("%d"), nPathIndex + 1);
		GetPrivateProfileString(_T("Path"), _T("Path") + strPathIndex, NULL, strBuffer, 256, INIT_FILE_PATH);
		theApp.m_fnWriteLog(_T("Monitoring Folder%d : %s "), nPathIndex + 1, strBuffer);
		SetDlgItemText(IDC_EDIT_DEL_PATH1 + nPathIndex, strBuffer);
		strFullPath.Format(_T("%s"), strBuffer);
		strFullPath.Replace(_T(" "), NULL);
		// ����̺� ��Ʈ ���� ���Ͷ�
		//if (strFullPath.Compare(_T("\\")) == 0 || strFullPath.Compare(_T("\\\\")) == 0)
		//	strFullPath.Format(_T("%s"), _T(""));
		if(nPathIndex < 7){
			if (strFullPath.Compare(_T("")) != 0)
			m_vstrMonitoringPath.push_back(strBuffer);
		}else{
			if (strFullPath.Compare(_T("")) != 0)
			m_vstrExMonitoringPath.push_back(strBuffer);
		}
		
	}
	return TRUE;
}

// ����̺� ����Ʈ �ʱ�ȭ
void CSpaceManagerDlg::m_fnInitDriveList()
{
	m_listCtrlDeleteDrive.InsertColumn(0, _T("Drive"), LVCFMT_LEFT, 50);
	m_listCtrlDeleteDrive.SetExtendedStyle(m_listCtrlDeleteDrive.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER);
	m_listCtrlDeleteDrive.InsertColumn(1, _T("Free"), LVCFMT_LEFT, 70);
	m_listCtrlDeleteDrive.InsertColumn(2, _T("Last Write"), LVCFMT_LEFT, 150);

	m_listCtrlFormatDrive.InsertColumn(0, _T("Drive"), LVCFMT_LEFT, 50);
	m_listCtrlFormatDrive.SetExtendedStyle(m_listCtrlDeleteDrive.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_DOUBLEBUFFER);
	m_listCtrlFormatDrive.InsertColumn(1, _T("Free"), LVCFMT_LEFT, 70);
	m_listCtrlFormatDrive.InsertColumn(2, _T("Last Write"), LVCFMT_LEFT, 150);

	CString strSpace;

	int nHDDCount = 0;

	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoDelete.size(); nVIndex++)
	{		
		m_listCtrlDeleteDrive.InsertItem(nHDDCount, m_vDriveInfoDelete[nVIndex].strDriveName);
		if(m_vDriveInfoDelete[nVIndex].bUseManagement)
		{
			m_listCtrlDeleteDrive.SetCheck(nHDDCount, TRUE);
		}
		strSpace.Format(_T("%.2lf%%"), (float)(m_vDriveInfoDelete[nVIndex].nDriveRemainingSpace * 1.0 / m_vDriveInfoDelete[nVIndex].nDriveTotalSpace * 100));
		m_listCtrlDeleteDrive.SetItemText(nHDDCount, 1, strSpace);
		nHDDCount++;
	}
	
	nHDDCount = 0;
	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoFormat.size(); nVIndex++)
	{		
		if(m_vDriveInfoFormat[nVIndex].strDriveName != _T("C:\\")) // Format drive list�� C,D,E�� ǥ�� ����.
		{
			m_listCtrlFormatDrive.InsertItem(nHDDCount, m_vDriveInfoFormat[nVIndex].strDriveName);
			if(m_vDriveInfoFormat[nVIndex].bUseManagement)
			{
				m_listCtrlFormatDrive.SetCheck(nHDDCount, TRUE);
			}
			strSpace.Format(_T("%.2lf%%"), (float)(m_vDriveInfoFormat[nVIndex].nDriveRemainingSpace * 1.0 / m_vDriveInfoFormat[nVIndex].nDriveTotalSpace * 100));
			m_listCtrlFormatDrive.SetItemText(nHDDCount, 1, strSpace);
		}

		nHDDCount++;
	}


}

// ����̺� ����Ʈ ����
void CSpaceManagerDlg::m_fnUpdateDriveList()
{
	CString strSpace;
	CString strLastWriteTime;

	int nHDDCount = 0;
	CTime timeLastWrite;

	UpdateData(TRUE);

	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoDelete.size(); nVIndex++)
	{		
		strSpace.Format(_T("%.2lf%%"), (float)(m_vDriveInfoDelete[nVIndex].nDriveRemainingSpace * 1.0 / m_vDriveInfoDelete[nVIndex].nDriveTotalSpace * 100));
		m_listCtrlDeleteDrive.SetItemText(nHDDCount, 1, strSpace);

		if (m_vDriveInfoDelete[nVIndex].bUseManagement)
		{
			// LastWrite ���� - ���� ����̺� ���� ���� ���� ���� ���� ���� �ð�
			timeLastWrite = m_vDriveInfoDelete[nVIndex].timeLastWrite;
			if (timeLastWrite == 0)
				strLastWriteTime.Format(_T("%s"), _T("No Files"));
			else
				strLastWriteTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), 
					timeLastWrite.GetYear(), timeLastWrite.GetMonth(), timeLastWrite.GetDay(),
					timeLastWrite.GetHour(), timeLastWrite.GetMinute(), timeLastWrite.GetSecond());
			
			m_listCtrlDeleteDrive.SetItemText(nHDDCount, 2, strLastWriteTime);
		}
		else
		{
			m_listCtrlDeleteDrive.SetItemText(nHDDCount, 2, _T(""));
		}
		nHDDCount++;
	}


	nHDDCount = 0;
	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoFormat.size(); nVIndex++)
	{		
		strSpace.Format(_T("%.2lf%%"), (float)(m_vDriveInfoFormat[nVIndex].nDriveRemainingSpace * 1.0 / m_vDriveInfoFormat[nVIndex].nDriveTotalSpace * 100));
		m_listCtrlFormatDrive.SetItemText(nHDDCount, 1, strSpace);

		if (m_vDriveInfoFormat[nVIndex].bUseManagement)
		{
			// LastWrite ���� - ���� ����̺� ���� ���� ���� ���� ���� ���� �ð�
			timeLastWrite = m_vDriveInfoFormat[nVIndex].timeLastWrite;
			if (timeLastWrite == 0)
				strLastWriteTime.Format(_T("%s"), _T("No Files"));
			else
				strLastWriteTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), 
				timeLastWrite.GetYear(), timeLastWrite.GetMonth(), timeLastWrite.GetDay(),
				timeLastWrite.GetHour(), timeLastWrite.GetMinute(), timeLastWrite.GetSecond());

			m_listCtrlFormatDrive.SetItemText(nHDDCount, 2, strLastWriteTime);
		}
		else
		{
			m_listCtrlFormatDrive.SetItemText(nHDDCount, 2, _T(""));
		}
		nHDDCount++;
	}


	UpdateData(FALSE);
}


CString CSpaceManagerDlg::m_fnGetItem(TCHAR *sec, CString key, TCHAR *def)
{
	TCHAR buf[_MAX_PATH];
	CString strRes;
	GetPrivateProfileString(sec,key,def,buf,_MAX_PATH, INIT_FILE_PATH);
	strRes.Format(_T("%s"), buf);
	return strRes;
}

CString CSpaceManagerDlg::m_fnGetStr(TCHAR* Sec, CString key, CString def)
{
	return m_fnGetItem(Sec,key,(TCHAR*)LPCTSTR(def));
}


void CSpaceManagerDlg::OnItemchangedListUsedDeleteDrive(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	if (pNMLV->uChanged & LVIF_STATE)
	{
		if ((pNMLV->uOldState & 0x1000) && (pNMLV->uNewState & 0x2000))
		{
			// checked
			m_vDriveInfoDelete[pNMLV->iItem].bUseManagement = TRUE;
		}
		else if((pNMLV->uOldState & 0x2000) && (pNMLV->uNewState & 0x1000))
		{
			// unchecked
			m_vDriveInfoDelete[pNMLV->iItem].bUseManagement = FALSE;
		}
	}
}

void CSpaceManagerDlg::OnItemchangedListUsedFormatDrive(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	if (pNMLV->uChanged & LVIF_STATE)
	{
		if ((pNMLV->uOldState & 0x1000) && (pNMLV->uNewState & 0x2000))
		{
			// checked
			m_vDriveInfoFormat[pNMLV->iItem].bUseManagement = TRUE;
		}
		else if((pNMLV->uOldState & 0x2000) && (pNMLV->uNewState & 0x1000))
		{
			// unchecked
			m_vDriveInfoFormat[pNMLV->iItem].bUseManagement = FALSE;
		}
	}
}


void CSpaceManagerDlg::OnBnClickedBtnSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strValue;

	GetDlgItemText(IDC_EDIT_CYCLE, strValue);
	WritePrivateProfileString(_T("Timer"), _T("CheckCycle"), strValue, INIT_FILE_PATH);

	GetDlgItemText(IDC_EDIT_SLEEP, strValue);
	WritePrivateProfileString(_T("Timer"), _T("DeleteSleep"), strValue, INIT_FILE_PATH);

	GetDlgItemText(IDC_EDIT_COND_TIME, strValue);
	WritePrivateProfileString(_T("Codition"), _T("TimeLate"), strValue, INIT_FILE_PATH);	

	GetDlgItemText(IDC_EDIT_COND_SPACE_MAX, strValue);
	WritePrivateProfileString(_T("Codition"), _T("MaxSpace"), strValue, INIT_FILE_PATH);

	GetDlgItemText(IDC_EDIT_COND_SPACE_MIN, strValue);
	WritePrivateProfileString(_T("Codition"), _T("MinSpace"), strValue, INIT_FILE_PATH);

	// ����2 ���� �ð�
	GetDlgItemText(IDC_EDIT_COND_STEP_TIME, strValue);
	WritePrivateProfileString(_T("Codition"), _T("StepTimeLate"), strValue, INIT_FILE_PATH);

	m_fnCheckCondition();

	GetDlgItemText(IDC_EDIT_COND_SPACE_DELETE, strValue);
	WritePrivateProfileString(_T("Codition"), _T("DeleteStep"), strValue, INIT_FILE_PATH);
	//���� ���� ���� �ð� 2016.04.08 add by mby
	GetDlgItemText(IDC_EDIT_EXCON_TIME, strValue);
	WritePrivateProfileString(_T("Codition"), _T("ExTimeLate"), strValue, INIT_FILE_PATH);

	GetDlgItemText(IDC_EDIT_FORMAT_MAX_SPACE, strValue);
	WritePrivateProfileString(_T("Codition"), _T("FormatMaxSpace"), strValue, INIT_FILE_PATH);

	GetDlgItemText(IDC_EDIT_PATH, strValue);
	WritePrivateProfileString(_T("TimeCheckPath"), _T("Path"), strValue, INIT_FILE_PATH);


	CString strKey;

	for (int nVIndex = 0; nVIndex < (int)m_vDriveInfoDelete.size(); nVIndex++)
	{
		strKey = m_listCtrlDeleteDrive.GetItemText(nVIndex, 0);
		m_listCtrlDeleteDrive.GetCheck(nVIndex)?strValue = _T("T"):strValue = _T("F");
		WritePrivateProfileString(_T("DeleteDrive"), strKey, strValue, INIT_FILE_PATH);
	}

	for (int nVIndex = 0; nVIndex < (int)m_listCtrlFormatDrive.GetItemCount(); nVIndex++)
	{
		strKey = m_listCtrlFormatDrive.GetItemText(nVIndex, 0);
		m_listCtrlFormatDrive.GetCheck(nVIndex)?strValue = _T("T"):strValue = _T("F");
		WritePrivateProfileString(_T("FormatDrive"), strKey, strValue, INIT_FILE_PATH);
	}

	m_chkSubDir.GetCheck()?strValue = _T("T"):strValue = _T("F");
	WritePrivateProfileString(_T("Path"), _T("Use Sub Folder"), strValue, INIT_FILE_PATH);

	CString strPathIndex;

	for(int nPathIndex = 0; nPathIndex < MAX_PATH_NUM; nPathIndex++)
	{		
		GetDlgItemText(IDC_EDIT_DEL_PATH1 + nPathIndex, strValue);
		strPathIndex.Format(_T("%d"), nPathIndex + 1);
		WritePrivateProfileString(_T("Path"), _T("Path") + strPathIndex, strValue, INIT_FILE_PATH);
	}

	//AfxMessageBox(LANG(TEXT_MSG_SET));
	//AfxMessageBox(_T("����Ǿ����ϴ�."));

	// 2015.10.12 Log �߰�
	theApp.m_fnWriteLog(_T(">>>>>>>> Change Config <<<<<<<<"));

	BOOL bRet = m_fnLoadInitFile();

	KillTimer(eTIMER_DriveCheck);

	m_nTimerCount = m_nCheckCycle;	// ���� ���� �� �ٷ� ���� ����

	SetTimer(eTIMER_DriveCheck, 1000, NULL);
	SendMessage(WM_TIMER, eTIMER_DriveCheck, 0);
}

void CSpaceManagerDlg::OnBnClickedBtnFindPath1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH1);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH2);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH3);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH4);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH5);
}

void CSpaceManagerDlg::OnBnClickedBtnFindPath6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH6);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath7()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH7);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath8()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH8);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath9()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH9);
}


void CSpaceManagerDlg::OnBnClickedBtnFindPath10()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_fnUpdateDeletePath(IDC_EDIT_DEL_PATH10);
}

// ����̺� ������ ���� ��θ� ��� - �Է� ���� ����
void CSpaceManagerDlg::m_fnUpdateDeletePath(int nID)
{
	BROWSEINFO BrInfo;
	TCHAR PathBuffer[512];

	::ZeroMemory(&BrInfo,sizeof(BROWSEINFO));
	::ZeroMemory(PathBuffer, 512);

	BrInfo.hwndOwner = GetSafeHwnd(); 

	BrInfo.lpszTitle = _T("���ϴ� ������ �����ϼ���.");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);

	::SHGetPathFromIDList(pItemIdList, PathBuffer);
	
	CString strDirectory;
	int nRetVal = 0;

	strDirectory.Format(_T("%s"), PathBuffer);
	nRetVal = strDirectory.Find(_T("\\"));

	strDirectory = strDirectory.Mid(nRetVal);
	
	SetDlgItemText(nID, strDirectory);
}

void CSpaceManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	BOOL bHide = FALSE;
	BOOL bClose = FALSE;

	if(nID == SC_CLOSE	)
	{
		bHide = TRUE;
//#if defined(_DEBUG)
		if ( (GetKeyState(VK_SHIFT) < 0) && (AfxMessageBox(_T("���� �����Ͻðڽ��ϱ�?"), MB_YESNO|MB_ICONQUESTION)==IDYES) ) 
		{
			bClose = TRUE;
		}
//#else
//		bClose = TRUE;
//#endif
	}
	else if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}

	if (bClose)
	{
		if (m_bTrayStatus)
		{
			NOTIFYICONDATA nid;
			nid.cbSize = sizeof(nid);
			nid.hWnd = m_hWnd;
			nid.uID = IDR_MAINFRAME;
			// Delete Icon in Status Bar
			Shell_NotifyIcon(NIM_DELETE, &nid);
		}
		AfxGetApp()->LoadIcon(IDI_TRAY);	

		CDialog::OnSysCommand(nID, lParam);
	}
	else if (bHide)
	{
		ShowWindow(SW_HIDE);
		return;

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// Ʈ���� ������ �߰�
void CSpaceManagerDlg::TraySetting(void)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON_MSG;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_TRAY);

	TCHAR strTitle[256];
	GetWindowText(strTitle, sizeof(strTitle));
	lstrcpy(nid.szTip, strTitle);
	Shell_NotifyIcon(NIM_ADD, &nid);
	SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
	SendMessage(WM_SETICON, (WPARAM)FALSE, (LPARAM)nid.hIcon);
	m_bTrayStatus = TRUE;
}

// Ʈ���� ������ �߰�
BOOL CSpaceManagerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	if (pMsg->message == WM_COMMAND && pMsg->wParam == 2)
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(nid);
		nid.hWnd = m_hWnd;
		nid.uID = IDR_MAINFRAME;
		// Delete Icon in Status Bar
		Shell_NotifyIcon(NIM_DELETE, &nid);

		AfxGetApp()->LoadIcon(IDI_TRAY);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// Ʈ���� ������ �߰�
LRESULT CSpaceManagerDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	return 0L;
}

void CSpaceManagerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	//���ҽ� ����
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_TRAY;

	Shell_NotifyIcon(NIM_DELETE,&nid);

	theApp.m_fnWriteLog(_T(">>>>>>>> SpaceManager End <<<<<<<<"));
}

void CSpaceManagerDlg::m_fnCheckCondition()
{
	int nTimeLate, nStepTimeLate;
	CString strValue;

	GetDlgItemText(IDC_EDIT_COND_TIME, strValue);	
	nTimeLate = _ttoi(strValue);

	GetDlgItemText(IDC_EDIT_COND_STEP_TIME, strValue);
	nStepTimeLate = _ttoi(strValue);

	if (nStepTimeLate > nTimeLate)
	{
		m_nStepTimeLate = nTimeLate;
		strValue.Format(_T("%d"), nTimeLate);
		SetDlgItemText(IDC_EDIT_COND_STEP_TIME, strValue);
		theApp.m_fnWriteLog(_T("== Change StepTimeLate Value %d -> %d =="), nStepTimeLate, nTimeLate);
		WritePrivateProfileString(_T("Codition"), _T("StepTimeLate"), strValue, INIT_FILE_PATH);	
	}
}

void CSpaceManagerDlg::OnLanRadioBtn(UINT ID)
{
	m_nLanguage = ID - IDC_RADIO_CNA;
	//ChangeLanguage(m_nLanguage);

	CString strTemp;
	strTemp.Format(_T("%d"), m_nLanguage);
	WritePrivateProfileString(_T("LanguageSet"), _T("Language"), strTemp, INIT_FILE_PATH);
}

void CSpaceManagerDlg::ChangeLanguage(int nLan)
{
	BOOL bSuccess = TRUE;
	switch(nLan)
	{
	case 1 :
		if(!g_multilang.Create(ORG_LANG_KOREAN_PATH, ORG_LANG_KOREAN_PATH, ORG_RESOURCE_PATH))
		{
			if(!g_multilang.Create(LANG_KOREAN_PATH, LANG_KOREAN_PATH, RESOURCE_PATH))
			{
				bSuccess = FALSE;
				AfxMessageBox(_T("Can Not Find Langue File."));
			}
		}
		break;
	default :
// 		if(!g_multilang.Create(ORG_LANG_CHINESE_PATH, ORG_LANG_KOREAN_PATH, ORG_RESOURCE_PATH))
// 		{
// 			if(!g_multilang.Create(LANG_CHINESE_PATH, LANG_KOREAN_PATH, RESOURCE_PATH))
// 			{
// 				bSuccess = FALSE;
// 				AfxMessageBox(_T("Can Not Find Langue File."));
// 			}
// 		}
		break;
	}
	if(bSuccess)
		g_multilang.SetDlgTexts(this->m_hWnd, IDD);
}

LPCTSTR CSpaceManagerDlg::Lang(LPCTSTR szLangKey, LPCTSTR szSection)
{
	return g_multilang.Lang(szLangKey, szSection);
}