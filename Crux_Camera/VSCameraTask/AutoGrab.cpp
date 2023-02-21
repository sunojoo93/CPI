// AutoGrab.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoGrab.h"
#include "afxdialogex.h"

// CAutoGrab 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAutoGrab, CDialogEx)


//2022.05.31 주석
//static UINT ThreadAutoGrab(LPVOID lParam)
//{
//	CAutoGrab* pAutoGrab = (CAutoGrab*)lParam;
//	CString strTemp;
//	CString strFilePath;
//	TCHAR strPattern[50];
//	int nTotalGrab = 0, nCurrentGrab = 0;
//	for(int i = 0 ; i < MAX_PATTERN ; i++)
//		nTotalGrab += _ttoi( pAutoGrab->m_ListOption.GetItemText(i,1) );
//
//	pAutoGrab->m_ListOption.EnableWindow(FALSE);
//	CString strState;
//	strState.Format(_T("%d / %d"), 0, nTotalGrab);
//	pAutoGrab->m_State.SetWindowTextW(strState);
//	pAutoGrab->m_fnInitProgress(0,nTotalGrab);
//
//	for(int i = 0 ; i < pAutoGrab->m_ListOption.GetItemCount() ; i++)
//	{
//		CString strTemp = pAutoGrab->m_ListOption.GetItemText(i,0);
//		_tcscpy(strPattern,strTemp);
//		strTemp.Format(_T("Current Pattern : %s"), strPattern);
//		pAutoGrab->m_CurrentPattern.SetWindowText(strTemp);
//
//		theApp.m_pInternalServer->VS_Send_Pattern_Change_To_PG(strPattern);
//		theApp.m_pCamera->SetExposureTime( _ttoi(pAutoGrab->m_ListOption.GetItemText(i,3)) );
//		theApp.m_pCamera->SetAnalogGain( _ttoi(pAutoGrab->m_ListOption.GetItemText(i,4)) );
//		Sleep(200);
//		int nRepeat = 0;
//		int nSaveNum = 1;
//		while(nRepeat < _ttoi( pAutoGrab->m_ListOption.GetItemText(i,1)) )
//		{
//			if(pAutoGrab->m_bStopFlag)
//			{
//				AfxMessageBox(_T("Grab 종료"));
//				return 0;
//			}
//
//
//			if( nSaveNum == _ttoi( pAutoGrab->m_ListOption.GetItemText(i,5)) )
//			{
//				strFilePath.Format(_T("%s\\%s_%d"),pAutoGrab->m_strDirPath,strPattern,nRepeat +1);
//				// 확장자 결정
//				if (theApp.m_pCamera->GetImageBitrate() == 8)
//					strFilePath = strFilePath + _T(".bmp");
//				else
//					strFilePath = strFilePath + _T(".tiff");
//				theApp.m_pCamera->SaveGrabImage(strFilePath);
//				nSaveNum = 1;
//			}
//			else
//			{
//				theApp.m_pCamera->CameraExpose();
//				theApp.m_pCamera->WaitGrabEnd();
//				nSaveNum++;
//			}
//			nCurrentGrab++;
//			nRepeat++;
//			pAutoGrab->m_fnChangeProgress(nCurrentGrab);
//
//
//			strState.Format(_T("%d / %d"), nCurrentGrab, nTotalGrab);
//			pAutoGrab->m_State.SetWindowTextW(strState);
//			int a = _ttoi(pAutoGrab->m_ListOption.GetItemText(i,2));
//			Sleep(_ttoi(pAutoGrab->m_ListOption.GetItemText(i,2)));
//		}
//	}
//	AfxMessageBox(_T("Grab 완료"));
//	pAutoGrab->m_ListOption.EnableWindow(TRUE);
//	pAutoGrab->m_pThrAutoGrab = NULL;
//	return 0;
//}

CAutoGrab::CAutoGrab(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAutoGrab::IDD, pParent)
{
	m_oldItem = -1;
	m_oldSubItem = -1;

	m_pThrAutoGrab = NULL;
	m_stAutoGrabInfo = new stAutoGrab[MAX_PATTERN];
	m_hStopEvent = NULL;
	m_hStopEvent = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	m_bStopFlag = FALSE;
}

CAutoGrab::~CAutoGrab()
{
}

void CAutoGrab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_DirPath);
	DDX_Control(pDX, IDC_LIST_OPTION, m_ListOption);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_STATIC_STATE, m_State);
	DDX_Control(pDX, IDC_STATIC_PATTERN, m_CurrentPattern);
}


BEGIN_MESSAGE_MAP(CAutoGrab, CDialogEx)

	ON_BN_CLICKED(IDC_BUTTON_START, &CAutoGrab::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_PATH, &CAutoGrab::OnBnClickedButtonPath)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OPTION, &CAutoGrab::OnNMClickListOption)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CAutoGrab::OnBnClickedButtonStop)
END_MESSAGE_MAP()


// CAutoGrab 메시지 처리기입니다.



void CAutoGrab::OnBnClickedButtonStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	

	if(m_pThrAutoGrab == NULL)
	{
		theApp.m_pInternalServer->VS_Send_Pattern_OnOff_To_PG(1);
		theApp.m_pCamera->SetSequenceMode(1);
		m_bStopFlag = FALSE;
		m_fnSaveData();
		//2022.05.31 주석
		//m_pThrAutoGrab = AfxBeginThread(ThreadAutoGrab, (LPVOID)this);
	}


	

}


void CAutoGrab::m_fnReadData()
{

	FILE* fp;
	_tfopen_s(&fp, theApp.GETDRV() + _T(":\\Crux\\Data\\ini\\AutoGrab.Dat"), _T("rb")); 
	if(NULL != fp){ 
		fread(m_stAutoGrabInfo, sizeof(stAutoGrab), MAX_PATTERN, fp); 
		fclose(fp); 
	} 

}

void CAutoGrab::m_fnSaveData()
{
	if(GetDlgItem(IDC_EDIT_MOD)->GetSafeHwnd())
	{
		CString str;
		GetDlgItemText(IDC_EDIT_MOD, str);
		m_ListOption.SetItemText(m_oldItem, m_oldSubItem, str);

		GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW );
	}

	for(int i = 0 ; i < MAX_PATTERN ; i++)
	{
		_tcscpy(m_stAutoGrabInfo[i].strPattern, m_ListOption.GetItemText(i,0) );
		m_stAutoGrabInfo[i].nRepeat = _ttoi( m_ListOption.GetItemText(i,1) );
		m_stAutoGrabInfo[i].nPeriod = _ttoi( m_ListOption.GetItemText(i,2) );
		m_stAutoGrabInfo[i].nExposureTime = _ttoi( m_ListOption.GetItemText(i,3) );
		m_stAutoGrabInfo[i].nGain = _ttoi( m_ListOption.GetItemText(i,4) );
		m_stAutoGrabInfo[i].nSaveNum = _ttoi( m_ListOption.GetItemText(i,5) );
	}
	
	FILE* fp;
	_tfopen_s(&fp, theApp.GETDRV() + _T(":\\Crux\\Data\\ini\\AutoGrab.Dat"), _T("wb")); 
	if(NULL != fp){ 
		fwrite(m_stAutoGrabInfo, sizeof(stAutoGrab), MAX_PATTERN, fp); 
		fclose(fp); 
	} 


}

void CAutoGrab::m_fnRenewalData()
{
	CString strTemp;
	for(int i = 0 ; i < MAX_PATTERN ; i++)
	{
		strTemp.Format(_T("%d"),m_stAutoGrabInfo[i].nRepeat);			m_ListOption.SetItemText(i, 1, strTemp);
		strTemp.Format(_T("%d"),m_stAutoGrabInfo[i].nPeriod);			m_ListOption.SetItemText(i, 2, strTemp);
		strTemp.Format(_T("%d"),m_stAutoGrabInfo[i].nExposureTime);		m_ListOption.SetItemText(i, 3, strTemp);
		strTemp.Format(_T("%d"),m_stAutoGrabInfo[i].nGain);				m_ListOption.SetItemText(i, 4, strTemp);
		strTemp.Format(_T("%d"),m_stAutoGrabInfo[i].nSaveNum);			m_ListOption.SetItemText(i, 5, strTemp);
	}		
}

void CAutoGrab::OnBnClickedButtonPath()
{
	__unaligned ITEMIDLIST *pidlBrowse;
	CString strPath;

	BROWSEINFO BrInfo;

	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;

	memset( &BrInfo, 0, sizeof(BrInfo) );
	BrInfo.pszDisplayName = (LPWSTR)(LPCTSTR)strPath;
	m_strDirPath = strPath;
	BrInfo.lpszTitle = _T("이미지를 저장할 폴더를 선택해 주십시오.");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;

	// 다이얼로그 띄우기
	pidlBrowse = SHBrowseForFolder(&BrInfo);

	if( pidlBrowse != NULL )
	{
		BOOL bSuccess = ::SHGetPathFromIDList(pidlBrowse, (LPWSTR)(LPCTSTR)strPath);

		m_DirPath.SetWindowTextW(strPath);
		if(!bSuccess)
		{
			MessageBox( _T("잘못된 폴더명 입니다."), _T(""), MB_OKCANCEL|MB_ICONASTERISK );
			return;
		}
	}

}


BOOL CAutoGrab::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ListOption.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListOption.InsertColumn(0, _T("Pattern"),LVCFMT_LEFT,60);
	m_ListOption.InsertColumn(1, _T("Repeat"),LVCFMT_LEFT,60);
	m_ListOption.InsertColumn(2, _T("Period (ms)"),LVCFMT_LEFT,70);
	m_ListOption.InsertColumn(3, _T("Exposure time"),LVCFMT_LEFT,100);
	m_ListOption.InsertColumn(4, _T("Gain"),LVCFMT_LEFT,60);
	m_ListOption.InsertColumn(5, _T("n장 중에 1장 저장"),LVCFMT_LEFT,150);

	m_ListOption.InsertItem(0, _T("Red"));
	m_ListOption.InsertItem(1, _T("Green"));
	m_ListOption.InsertItem(2, _T("Blue"));
	m_ListOption.InsertItem(3, _T("Black"));
	m_ListOption.InsertItem(4, _T("White"));
	m_ListOption.InsertItem(5, _T("L16"));
	m_ListOption.InsertItem(6, _T("L32"));
	m_ListOption.InsertItem(7, _T("L84"));
	m_ListOption.InsertItem(8, _T("L127"));
	m_ListOption.SetItem(0, 1, LVIF_TEXT, _T(""), 0, NULL, NULL, NULL);

	ResetEvent(m_hStopEvent);
	CString strTemp;
	strTemp.Format(_T("0 / 0"));
	m_State.SetWindowText(strTemp);
	strTemp.Format(_T("Current Pattern :"));
	m_CurrentPattern.SetWindowText(strTemp);
// 	for(int i = 0 ; i < MAX_PATTERN ; i++)
// 		m_stAutoGrabInfo[i] = new stAutoGrab;

	//memset(&m_stAutoGrabInfo,0,sizeof(m_stAutoGrabInfo)* MAX_PATTERN);
	m_fnReadData();
	m_fnRenewalData();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CAutoGrab::OnNMClickListOption(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSavedItem = pNMItemActivate->iItem;
	int iSavedSubitem = pNMItemActivate->iSubItem;


	if(GetDlgItem(IDC_EDIT_MOD)->GetSafeHwnd())
	{
		CString str;
		GetDlgItemText(IDC_EDIT_MOD, str);
		m_ListOption.SetItemText(m_oldItem, m_oldSubItem, str);

		GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW );
	}
	m_fnSaveData();
	m_oldItem = iSavedItem;
	m_oldSubItem = iSavedSubitem;

	if(pNMItemActivate->iItem != -1)
	{
		CRect rect;

		if(pNMItemActivate->iSubItem == 0)
		{
			m_ListOption.GetItemRect(pNMItemActivate->iItem, rect, LVIR_BOUNDS);
			rect.right = rect.left + m_ListOption.GetColumnWidth(0);
		}
		else
		{
			m_ListOption.GetSubItemRect(pNMItemActivate->iItem, pNMItemActivate->iSubItem, LVIR_BOUNDS, rect);
		}

		m_ListOption.ClientToScreen(rect);
		this->ScreenToClient(rect);

		GetDlgItem(IDC_EDIT_MOD)->SetWindowText(m_ListOption.GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem));
		GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW );
		GetDlgItem(IDC_EDIT_MOD)->SetFocus();
		

	}

}


BOOL CAutoGrab::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.


	m_bStopFlag = TRUE;

	m_pThrAutoGrab = NULL;
	m_fnSaveData();
	return CDialogEx::DestroyWindow();
}


BOOL CAutoGrab::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN){
		if(pMsg->wParam == VK_RETURN){

			if(pMsg->hwnd == GetDlgItem(IDC_EDIT_MOD)->GetSafeHwnd())
			{
				CString str;
				GetDlgItemText(IDC_EDIT_MOD, str);
				m_ListOption.SetItemText(m_oldItem, m_oldSubItem, str);

				GetDlgItem(IDC_EDIT_MOD)->SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW );
				m_fnSaveData();
			}

			return TRUE;
		}

		if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAutoGrab::OnBnClickedButtonStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bStopFlag = TRUE;

	//2022.05.31 주석
	//m_pThrAutoGrab = NULL;
	m_ListOption.EnableWindow(TRUE);
}
