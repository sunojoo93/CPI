
// VSAlgorithmTaskDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "VSAlgorithmTask.h"
#include "VSAlgorithmTaskDlg.h"
#include "afxdialogex.h"
#include "Define.h"
#include "version.h"
//#include "ImageSave.h"
#include "DllInterface.h"	// for test
#include "MssqlODBC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVSAlgorithmTaskDlg 대화 상자

CVSAlgorithmTaskDlg::CVSAlgorithmTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVSAlgorithmTaskDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVSAlgorithmTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlListLog);
}

BEGIN_MESSAGE_MAP(CVSAlgorithmTaskDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_TRAYICON_MSG,	TrayIconMessage)
	ON_WM_DRAWITEM()
	ON_BN_CLICKED(IDC_BTN_TEST, &CVSAlgorithmTaskDlg::OnBnClickedBtnTest)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_UNICODE,		m_fnPrintUiMsgUnicode)
	ON_MESSAGE(WM_PRINT_UI_LOG_MSG_MULTI_BYTE,	m_fnPrintUiMsgMultiByte)
	ON_BN_CLICKED(IDC_BTN_TEST2, &CVSAlgorithmTaskDlg::OnBnClickedBtnTest2)
	ON_BN_CLICKED(IDC_BTN_CNT_CLEAR, &CVSAlgorithmTaskDlg::OnBnClickedBtnCntClear)
END_MESSAGE_MAP()


// CVSAlgorithmTaskDlg 메시지 처리기

BOOL CVSAlgorithmTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// Auto Mode 시 최소화 시작
	ShowWindow(SW_MINIMIZE);

	// Icon 변경
	m_fnTrayIconRefresh();
	m_bTrayStatus = FALSE;
	TraySetting();	

	// Task 상태 갱신 Timer
	SetTimer(eTIMER_UpdateTaskStatus, 1000, NULL);

	SetWindowText(VER_INFO);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CVSAlgorithmTaskDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CVSAlgorithmTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVSAlgorithmTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	BOOL bHide = FALSE;
	BOOL bClose = FALSE;

	if(nID == SC_CLOSE	)
	{
		bHide = TRUE;
//#if defined(_DEBUG)
		if ( (GetKeyState(VK_SHIFT) < 0) && (AfxMessageBox(_T("정말 종료하시겠습니까?"), MB_YESNO|MB_ICONQUESTION)==IDYES) ) 
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
		AfxGetApp()->LoadIcon(IDI_ICON1);	

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

// 트레이 아이콘 추가
void CVSAlgorithmTaskDlg::TraySetting(void)
{
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDR_MAINFRAME;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON_MSG;
	nid.hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);

	TCHAR strTitle[256];
	GetWindowText(strTitle, sizeof(strTitle));
	lstrcpy(nid.szTip, strTitle);
	Shell_NotifyIcon(NIM_ADD, &nid);
	SendMessage(WM_SETICON, (WPARAM)TRUE, (LPARAM)nid.hIcon);
	SendMessage(WM_SETICON, (WPARAM)FALSE, (LPARAM)nid.hIcon);
	m_bTrayStatus = TRUE;
}


BOOL CVSAlgorithmTaskDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (pMsg->message == WM_COMMAND && pMsg->wParam == 2)
	{
		NOTIFYICONDATA nid;
		nid.cbSize = sizeof(nid);
		nid.hWnd = m_hWnd;
		nid.uID = IDR_MAINFRAME;
		// Delete Icon in Status Bar
		Shell_NotifyIcon(NIM_DELETE, &nid);

		AfxGetApp()->LoadIcon(IDI_ICON1);
	}


	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);

}

// 트레이 아이콘 추가
LRESULT CVSAlgorithmTaskDlg::TrayIconMessage(WPARAM wParam, LPARAM lParam)
{
	// Tray Icon Click -> Dialog pop up
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	return 0L;
}


void CVSAlgorithmTaskDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//리소스 해제
	NOTIFYICONDATA nid;
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd;
	nid.uID = IDI_ICON1;

	Shell_NotifyIcon(NIM_DELETE,&nid);
}

void CVSAlgorithmTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	HWND hButton = NULL;

	switch (nIDEvent)
	{
	case eTIMER_UpdateTaskStatus:
		if (theApp.GetIPCState())
			theApp.m_AlgorithmTask->SetParentWnd(m_hWnd);
		hButton	= ::GetDlgItem(m_hWnd, IDC_BTN_IPC_CONNECT);
		::InvalidateRect(hButton, NULL, FALSE);
		break;
	}	

	CDialogEx::OnTimer(nIDEvent);
}


void CVSAlgorithmTaskDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// nIDCtl = IDC값
	switch(nIDCtl)
	{
		case IDC_BTN_IPC_CONNECT:
		{
			if (theApp.GetIPCState())
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_GREEN, DARK_GREEN);
			}
			else
			{
				m_fnChangeBtnColor(lpDrawItemStruct, LIGHT_RED, DARK_RED);
			}
		}
		break;
	}

	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CVSAlgorithmTaskDlg::m_fnChangeBtnColor(LPDRAWITEMSTRUCT lpDrawItemStruct, COLORREF colorBtn, COLORREF colorText)
{
	CDC dc;
	RECT rect;
	dc.Attach(lpDrawItemStruct->hDC);					//버튼의 dc구하기
	rect = lpDrawItemStruct->rcItem;					//버튼영역 구하기
	dc.Draw3dRect(&rect,WHITE,BLACK);					//버튼의 외곽선 그리기

	dc.FillSolidRect(&rect, colorBtn);					//버튼색상
	dc.SetBkColor(colorBtn);							//text의 백그라운드 색상
	dc.SetTextColor(colorText);							//texttort

	UINT state = lpDrawItemStruct->itemState;			//버튼상태구하기
	if((state &ODS_SELECTED))
	{
		dc.DrawEdge(&rect,EDGE_SUNKEN,BF_RECT);
	}
	else
	{
		dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
	}

	TCHAR buffer[MAX_PATH];											//버튼의 text를 얻기위한 임시버퍼
	ZeroMemory(buffer,MAX_PATH);									//버퍼초기화
	::GetWindowText(lpDrawItemStruct->hwndItem,buffer,MAX_PATH);	//버튼의 text얻기
	dc.DrawText(buffer,&rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);	//버튼의 text넣기
	dc.Detach();													//버튼의 dc 풀어주기
}

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* strPath, ...);

void CVSAlgorithmTaskDlg::OnBnClickedBtnTest()
{
	CString barcode = _T("20211109184947");
	CStringA barcode2 = CStringA(barcode);
	char cArrayBarcode[100] = { 0x00, };
	memcpy(cArrayBarcode, barcode2.GetBuffer(), barcode2.GetLength());

	MssqlODBC db;
	db.Connect();

	stPancakeInfo pancakeInfo = db.SelectLastPancake(_atoi64(cArrayBarcode));
	db.InsertImage(pancakeInfo.id, _T("D:\\DefectMapImage.jpg"));

	stPancakeDefect stDefect;
	stDefect.id = pancakeInfo.id;
	stDefect.exception = false;
	stDefect.ITEM_NO = 0;
	stDefect.defect_no = 0;
	stDefect.pixel_start_x = 100;
	stDefect.pixel_start_y = 100;
	stDefect.pixel_end_x = 200;
	stDefect.pixel_end_y = 200;
	stDefect.nm_start_x = 123.111;
	stDefect.nm_start_y = 123.111;
	stDefect.nm_end_x = 33.11;
	stDefect.nm_end_y = 33.11;
	stDefect.def_size = 99.1123;
	db.InsertPancakeDefect(stDefect);

	db.Disconnect();

	return;





	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double nRatio = *theApp.GetAlignParameter(0) + E_PARA_SVI_CROP_RATIO;
	int nRet = 0;
	byte  byteParam[1000]		= {0,};
	byte* pSendParam			= byteParam;

	BYTE* pByteOutput[4] = {NULL, };
	for (int i=0; i<4; i++)
		pByteOutput[i] = new BYTE[6576 * 4384];

	cv::Mat MatPSImage = cv::imread("D:\\Backup\\00_DUST_CAM00.bmp", IMREAD_UNCHANGED);
	ImageSave(MatPSImage, _T("D:\\PSTEST\\PS_IMAGE.bmp"));

	BYTE* pByteInput = MatPSImage.data;

	int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;

	for(UINT32 j=0;j<4384*2;j++)
	{
		int p = 0;
		for(UINT32 i=0; i<6576*2; i++)
		{
			if		(i%2 == 0 && j%2 == 0)
				pByteOutput[1][cnt1++] = pByteInput[j*6576*2+i];
			else if (i%2 == 1 && j%2 == 0)
				pByteOutput[0][cnt2++] = pByteInput[j*6576*2+i];
			else if (i%2 == 0 && j%2 == 1)
				pByteOutput[3][cnt3++] = pByteInput[j*6576*2+i];
			else if (i%2 == 1 && j%2 == 1)
				pByteOutput[2][cnt4++] = pByteInput[j*6576*2+i];
		}
	}
	
	cv::Mat MatOrg[4];
	for (int i=0; i<4; i++)
	{
		MatOrg[i] = cv::Mat(4384, 6576, CV_8UC1, pByteOutput[i]);
		ImageSave(MatOrg[i], _T("D:\\PSTEST\\%d.bmp"), i);
	}	

	for (int i=0; i<4; i++)
		SAFE_DELETE_ARR(pByteOutput[i]);

// 회전 테스트
// 	for (int nImageNo=0; nImageNo<10; nImageNo++)
// 	{
// 		CString strManualImagePath = _T("");
// 		CString strOrgFileName = _T("");	// 원본 이미지 파일 기준 이름 (Grab Number 제외)
// 		// 파일 이름은 Alg.Task 에서 확정하도록 변경 - 다중 카메라 대응
// 		strOrgFileName.Format(_T("%s_CAM%02d"), theApp.GetGrabStepName(nImageNo), 0);			
// 
// 		strManualImagePath = theApp.GetCurStepFileName(_T("D:\\Doc\\Image\\H-Project\\점등\\66_0.05\\"), strOrgFileName.GetBuffer(0));
// 
// 		char* pStr = NULL;
// 		pStr = CSTR2PCH(strManualImagePath);  
// 		Mat MatOrgImage = cv::imread(pStr, IMREAD_UNCHANGED);
// 		SAFE_DELETE_ARR(pStr);
// 
// 		Align_RotateImage(MatOrgImage, MatOrgImage, -0.15);
// 
// 		ImageSave(MatOrgImage, _T("%s\\%02d_%s.bmp"), _T("D:\\Doc\\Image\\H-Project\\점등\\66_"), nImageNo, strOrgFileName);
// 	}

	return;

// 	double dAlignPara[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT] = {4, 3800, 1900, 20, 9, 3, 5, 5.0, 75, 4, };
// 	memcpy(pSendParam, dAlignPara, sizeof(double) * 15);
// 	pSendParam += sizeof(double) * 15;
// 	_tcscpy((TCHAR *)pSendParam, _T("D:\\Doc\\Image\\H-Project\\PS Image\\019 ps\\01_R_CAM00.bmp"));				pSendParam += 2000;
// 	nRet = theApp.m_AlgorithmTask->CmdEditSend_TEST(50, 10, 0, (ULONG)(2000+(sizeof(double) * 15)), 150, (byte *)byteParam, CMD_TYPE_NORES);
// 	return;
// 
// 	CString strTemp = _T("");
// 	strTemp.Format(_T("%s\\%d_%s.bmp"), _T("D:\\TEST"),/*strFileDirectory,*/ 2, _T("TEST")/*theApp.m_Config.GetCurStepName(nGrabCnt)*/);
// 
// 	PARAM_INSPECT_START* prmInspStart = new PARAM_INSPECT_START;
// 	prmInspStart->nInspType = 1;
// 	memcpy((TCHAR *)prmInspStart->strPanelID, _T("TEST"), sizeof(prmInspStart->strPanelID));
// 	prmInspStart->nCameraNum = 1;						// Camera Num - 0 고정
// 	prmInspStart->nImageNum = 2;
// 	_tcscpy(prmInspStart->strImagePath, strTemp);		// CString To TCHAR *
// 
// 	nRet = theApp.m_AlgorithmTask->CmdEditSend_TEST(SEND_INSPECT_START, 0, (ULONG)sizeof(PARAM_INSPECT_START), VS_ALGORITHM_TASK, (byte *)prmInspStart, CMD_TYPE_NORES);
// 	return;
// 
// 	PARAM_CLASSIFY_END* prmClassifyEnd = new PARAM_CLASSIFY_END;
// 
// 	_tcscpy(prmClassifyEnd->strPanelID, _T("KIM SANG GOO"));
// 	prmClassifyEnd->bIsOK = TRUE;
// 	prmClassifyEnd->nDefectCount = 1234;
// 	//_tcscpy(prmClassifyEnd->strStartTime, _T("IM KYUNG MIN"));
// 	//_tcscpy(prmClassifyEnd->strTactTime, _T("KIM SANG GOO"));
// 
// 	nRet = theApp.m_AlgorithmTask->CmdEditSend_TEST(21, 30, 0, sizeof(PARAM_CLASSIFY_END), 121, (byte *)prmClassifyEnd, CMD_TYPE_RES);
// 	SAFE_DELETE(prmClassifyEnd);
	theApp.ReadAlgorithmParameter(_T("bbbb"));
	return;
}


int CVSAlgorithmTaskDlg::m_fnTrayIconRefresh()
{
	try
	{
		HWND  hWnd;
		CRect re;
		DWORD dwWidth, dwHeight, x, y;

		// find a handle of a tray
		hWnd = ::FindWindow( _T("Shell_TrayWnd"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("TrayNotifyWnd"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("SysPager"), NULL );

		if( hWnd != NULL )
			hWnd = ::FindWindowEx( hWnd, 0, _T("ToolbarWindow32"), NULL );
		// get client rectangle (needed for width and height of tray)
		if( hWnd!=NULL )
		{
			::GetClientRect( hWnd, &re );

			// get size of small icons
			dwWidth = (DWORD)GetSystemMetrics(SM_CXSMICON);   //  sm_cxsmicon);
			dwHeight = (DWORD)GetSystemMetrics(SM_CYSMICON);   //  sm_cysmicon);

			// initial y position of mouse - half of height of icon
			y = dwHeight >> 1;
			while( y < (DWORD)re.bottom )  // while y < height of tray
			{
				x = dwHeight>>1;              // initial x position of mouse - half of width of icon
				while( x < (DWORD)re.right ) // while x < width of tray
				{
					::SendMessage( hWnd, WM_MOUSEMOVE, 0, (y<<16)|x); // simulate moving mouse over an icon
					x = x + dwWidth; // add width of icon to x position
				}
				y = y + dwHeight; // add height of icon to y position
			}
		}

		return APP_OK;
	}
	catch (...)
	{
		return APP_NG;
	}
}



LRESULT CVSAlgorithmTaskDlg::m_fnPrintUiMsgUnicode(WPARAM wParam, LPARAM lParam)
{
	CString strMsg = _T("");
	SYSTEMTIME systime;
	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
		systime.wMilliseconds, (LPCTSTR)lParam);

	if(m_ctrlListLog.GetCount() > MAX_GRID_LOG	)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0,strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[] (wchar_t*)lParam;

	return 0;
}

LRESULT CVSAlgorithmTaskDlg::m_fnPrintUiMsgMultiByte(WPARAM wParam, LPARAM lParam)
{
	wchar_t* pStr;

	//멀티 바이트 크기 계산 길이 반환
	int strSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, -1, NULL, NULL);
	//wchar_t 메모리 할당

	pStr = new WCHAR[strSize];
	memset(pStr,0,sizeof(WCHAR) * strSize);

	//형 변환
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)lParam, (int)strlen((LPCSTR)lParam)+1, pStr, strSize);


	CString strMsg = _T("");
	SYSTEMTIME systime;	

	::GetLocalTime(&systime);

	strMsg.Format(L"%02d:%02d:%02d.%03d - %s\n", systime.wHour, systime.wMinute, systime.wSecond, 
		systime.wMilliseconds, pStr);

	if(m_ctrlListLog.GetCount() > MAX_GRID_LOG	)
		m_ctrlListLog.DeleteString(MAX_GRID_LOG - 1);

	m_ctrlListLog.InsertString(0,strMsg);
	m_ctrlListLog.SetCurSel(0);

	//free((VOID*)lParam);
	delete[] (char*)lParam;
	delete[] pStr;

	return 0;
}

void CVSAlgorithmTaskDlg::OnBnClickedBtnTest2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE* pByteOutput[4] = {NULL, };
	for (int i=0; i<4; i++)
		pByteOutput[i] = new BYTE[6576 * 4384];

	CFileDialog* pDlg = NULL;
// 	if (m_pSharedMemory[nCamIndex]->GetImgBitrate() == 8)
		pDlg = new CFileDialog( TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("BMP FILES (*.BMP)|*.bmp|All Files (*.*)|*.*||") );
// 	else
// 		pDlg = new CFileDialog( TRUE, _T("tiff"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("TIFF FILES (*.TIFF)|*.tiff|All Files (*.*)|*.*||") );

	CString strPath;
	if(pDlg->DoModal() == IDOK)
	{
		strPath.Format(_T("%s"), pDlg->GetPathName() );
	}
	else
	{
		SAFE_DELETE(pDlg);
		return;
	}
	
	cv::Mat MatPSImage = cv::imread(CSTR2PCH(strPath), IMREAD_UNCHANGED);
	ImageSave(MatPSImage, _T("D:\\PSTEST\\PS_IMAGE.bmp"));

	BYTE* pByteInput = MatPSImage.data;

	int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;

	for(UINT32 j=0;j<4384*2;j++)
	{
		int p = 0;
		for(UINT32 i=0; i<6576*2; i++)
		{
			if		(i%2 == 0 && j%2 == 0)
				pByteOutput[1][cnt1++] = pByteInput[j*6576*2+i];
			else if (i%2 == 1 && j%2 == 0)
				pByteOutput[0][cnt2++] = pByteInput[j*6576*2+i];
			else if (i%2 == 0 && j%2 == 1)
				pByteOutput[3][cnt3++] = pByteInput[j*6576*2+i];
			else if (i%2 == 1 && j%2 == 1)
				pByteOutput[2][cnt4++] = pByteInput[j*6576*2+i];
		}
	}

	cv::Mat MatOrg[4];
	for (int i=0; i<4; i++)
	{
		MatOrg[i] = cv::Mat(4384, 6576, CV_8UC1, pByteOutput[i]);
		ImageSave(MatOrg[i], _T("D:\\PSTEST\\%d.bmp"), i);
	}	

	for (int i=0; i<4; i++)
		SAFE_DELETE_ARR(pByteOutput[i]);
}


void CVSAlgorithmTaskDlg::OnBnClickedBtnCntClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strLogPath = _T(""), strSection = _T(""), strKey = _T("");
	strLogPath.Format(_T("%s\\CountingStageAD_PC%02d.INI"), DEFECT_INFO_PATH, theApp.m_Config.GetPCNum());

	TRY 
	{
		// 현재 Count 읽기 / 증가 후 쓰기
		EnterCriticalSection(&theApp.m_csCntFileSafe);		

		for (int nStageNo = 1; nStageNo <= MAX_STAGE_COUNT; nStageNo++)
		{
			strSection.Format(_T("Stage_%d_%d"), nStageNo, theApp.m_Config.GetPCNum());
			WritePrivateProfileString(strSection, _T("AD"), _T("0"), strLogPath);

			for (int nImageNum = 0; nImageNum < theApp.GetGrabStepCount(); nImageNum++)
			{
				if (theApp.GetImageClassify(nImageNum) != E_IMAGE_CLASSIFY_AVI_DUST)
				{	
					strKey.Format(_T("%s_GV"), theApp.GetGrabStepName(nImageNum));
					WritePrivateProfileString(strSection, strKey, _T("0"), strLogPath);
				}
			}
		}

		LeaveCriticalSection(&theApp.m_csCntFileSafe);
	}
	CATCH (CException, e)
	{
		e->Delete();
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Exception m_fnCountingStageAD()"));
	}
	END_CATCH
}
