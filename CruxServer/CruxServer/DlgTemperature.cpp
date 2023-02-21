// DlgTemperature.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CruxServer.h"
#include "DlgTemperature.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "ViewSetup.h"
#include "ViewAuto.h"
enum
{
	CONTROLLER_1 = 0,
	CONTROLLER_2,
	CONTROLLER_3,
	CONTROLLER_4,
	CONTROLLER_5,
	CONTROLLER_6,
	CONTROLLER_7,
	CONTROLLER_8,
	CONTROLLER_9
};

// Dlg_Temperature 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgTemperature, CDialogEx)

CDlgTemperature::CDlgTemperature(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgTemperature::IDD, pParent)
{
	m_pDlgErrorMsg = NULL;
	m_hWarnnigHandle = NULL;
}

CDlgTemperature::~CDlgTemperature()
{
	if (m_pDlgErrorMsg != NULL)
	{
		delete m_pDlgErrorMsg;
		m_pDlgErrorMsg = NULL;
	}
}

void CDlgTemperature::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	CString cConfigRoot, strValue;
	cConfigRoot.Append(ROOT_PATH);
	cConfigRoot.Append(CONFIG_PATH);
	int nChkTerm = GetPrivateProfileInt(EQP_STATE, KEY_TEMPERATURE_CHECK_TERM, 0, cConfigRoot);
	strValue.Format(_T("%d"), nChkTerm);
	GetDlgItem(IDC_EDIT_CHK_TERM)->SetWindowText(strValue);
	SetTimer(TIMER_TEMPERATURE_LOG, nChkTerm, NULL);
	SetTimer(TIMER_UPDATE_TEMPERATURE, TIMER_UPDATE_TEMPERATURE_INTERVAL, NULL);
	int nCountVisionClient	= GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_VPC_CNT, 0, cConfigRoot);

	for (int nPcCnt = 0; nPcCnt < MAX_VISION_PC - 3; nPcCnt++) // 20181206 -5 -> -3 by ryu 
	{

		for (int nCtrCnt = 0; nCtrCnt < LIGHT_CONTROL_MAX; nCtrCnt++)
		{
			if (nPcCnt >= nCountVisionClient)
			{				
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
				((CGxStaticEx*)GetDlgItem(IDC_STATIC_PC1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
				((CGxStaticEx*)GetDlgItem(IDC_STATIC_LIMIT1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
				((CGxStaticEx*)GetDlgItem(IDC_STATIC_CUR_TMP1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
			}
		}
	}

	double dOffset = nCountVisionClient * 3;
	((CGxStaticEx*)GetDlgItem(IDC_STATIC_CHK_TERM))->MoveWindow(20,(670  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70), 70, 25);
	((CGxStaticEx*)GetDlgItem(IDC_EDIT_CHK_TERM))->MoveWindow(104, (665  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70), 94, 25);
	((CGxStaticEx*)GetDlgItem(IDC_STATIC_TIME))->MoveWindow(199,   (670  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70), 30, 15);
	((CGxStaticEx*)GetDlgItem(IDSAVE))->MoveWindow(235, (665  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70), 70, 25);
	((CGxStaticEx*)GetDlgItem(IDCANCEL))->MoveWindow(235, (690  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70), 70, 25);
	this->MoveWindow(1280, 50, 330, (770  + dOffset) - ((MAX_VISION_PC - 2 - nCountVisionClient) * 70));
	
}


BEGIN_MESSAGE_MAP(CDlgTemperature, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CDlgTemperature::OnBnClickedCancel)
	ON_BN_CLICKED(IDSAVE, &CDlgTemperature::OnBnClickedSave)
END_MESSAGE_MAP()


// Dlg_Temperature 메시지 처리기입니다.

BOOL CDlgTemperature::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgTemperature::m_fnInitState()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString cCountRoot, strValue;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);
	int nCountVisionClient	= GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_VPC_CNT, 0, cCountRoot);
	int nLightCnt = 0;


	for (int nPcCnt = 0; nPcCnt < MAX_VISION_PC - 3; nPcCnt++) // -5 -> -3 change
	{
		
		for (int nCtrCnt = 0; nCtrCnt < LIGHT_CONTROL_MAX; nCtrCnt++)
		{
		//	pFrame->m_pVsSeverIF->m_bControllerFlag[nPcCnt][nCtrCnt] = TRUE;
			if (pFrame->m_pVsSeverIF->m_bControllerFlag[nPcCnt][nCtrCnt] == TRUE)
			{
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 + (nPcCnt) *2  + nCtrCnt))->EnableWindow(TRUE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 + (nPcCnt) *2  + nCtrCnt))->EnableWindow(TRUE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 + (nPcCnt) *2  + nCtrCnt))->EnableWindow(TRUE);

				if (nCtrCnt == 0)		nLightCnt++;			
			
			}
			else
			{
				/*((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 +  (nPcCnt) *2  + nCtrCnt))->EnableWindow(FALSE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 +  (nPcCnt) *2  + nCtrCnt))->EnableWindow(FALSE);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 +  (nPcCnt) *2  + nCtrCnt))->EnableWindow(FALSE);*/
			}
		}

	}

	//double dOffset = nLightCnt * 3;
	//int nDisCnt = 0;
	//
 //	for (int nPcCnt = 0; nPcCnt < MAX_VISION_PC - 2; nPcCnt++)
 //	{
 //
 //		for (int nCtrCnt = 0; nCtrCnt < LIGHT_CONTROL_MAX; nCtrCnt++)
 //		{
 //			
	//		if (pFrame->m_pVsSeverIF->m_bControllerFlag[nPcCnt][nCtrCnt] != TRUE)
	//		{				
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_HIDE);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_PC1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_LIMIT1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_CUR_TMP1 +  (nPcCnt)))->ShowWindow(SW_HIDE);
	//		}
	//		else
	//		{						
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 +  (nPcCnt) *2  + nCtrCnt))->MoveWindow(104 *(nCtrCnt + 1), (680 + dOffset + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt )) * 70), 94, 25);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 +  (nPcCnt) *2  + nCtrCnt))->MoveWindow(104 *(nCtrCnt + 1), (705  + dOffset  + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt ) ) * 70), 94, 25);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 +  (nPcCnt) *2  + nCtrCnt))->MoveWindow(104 *(nCtrCnt + 1), (730  + dOffset  + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt)) * 70), 94, 25);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_PC1 +  (nPcCnt)))->MoveWindow(20, (680  + dOffset + 5  + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt ) ) * 70), 70, 25);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_LIMIT1 +  (nPcCnt)))->MoveWindow(20, (705  + dOffset + 4  + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt ) ) * 70), 70, 25);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_CUR_TMP1 +  (nPcCnt)))->MoveWindow(20, (730  + dOffset + 4 + ((nDisCnt ) * 6)) - ((MAX_VISION_PC - 2 - (nDisCnt ) ) * 70), 80, 25);

	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_CONTROLLER1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_SHOW);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_SHOW);
	//			((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 +  (nPcCnt) *2  + nCtrCnt))->ShowWindow(SW_SHOW);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_PC1 +  (nPcCnt)))->ShowWindow(SW_SHOW);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_LIMIT1 +  (nPcCnt)))->ShowWindow(SW_SHOW);
	//			((CGxStaticEx*)GetDlgItem(IDC_STATIC_CUR_TMP1 +  (nPcCnt)))->ShowWindow(SW_SHOW);

	//		} 
	//	}
	//	if (nLightCnt > nDisCnt)
	//	{
	//		nDisCnt ++;
	//	}
	//	
 //	}
 //
	//((CGxStaticEx*)GetDlgItem(IDC_STATIC_CHK_TERM))->MoveWindow(20, (680 + dOffset) - ((MAX_VISION_PC - 2 - nLightCnt) * 70), 70, 25);
	//((CGxStaticEx*)GetDlgItem(IDC_EDIT_CHK_TERM))->MoveWindow(104, (675  + dOffset) - ((MAX_VISION_PC - 2 - nLightCnt) * 70), 94, 25);
	//((CGxStaticEx*)GetDlgItem(IDC_STATIC_TIME))->MoveWindow(199, (680  + dOffset) - ((MAX_VISION_PC - 2 - nLightCnt) * 70), 30, 15);
	//((CGxStaticEx*)GetDlgItem(IDSAVE))->MoveWindow(235, (675  + dOffset) - ((MAX_VISION_PC - 2 - nLightCnt) * 70), 70, 25);
	//((CGxStaticEx*)GetDlgItem(IDCANCEL))->MoveWindow(235, (700  + dOffset) - ((MAX_VISION_PC - 2 - nLightCnt) * 70), 70, 25);
	//this->MoveWindow(1280, 50, 330, (770  + dOffset ) - ((MAX_VISION_PC - 2 - nLightCnt) * 70));
}

void CDlgTemperature::m_fnUpdateState()
{
	int nLimitTemp = 0, nCurTemp = 0, nCtrNo = 0, nMsgFlg = 0;
	CString strLimitTemp, strCurTemp, strMsg;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for (int nPcCnt = 0; nPcCnt < pFrame->m_stParamInfo.nCountVisionClient; nPcCnt++)
	{
		for (int nCtrCnt = 0; nCtrCnt < LIGHT_CONTROL_MAX; nCtrCnt++)
		{
			if (pFrame->m_pVsSeverIF->m_bControllerFlag[nPcCnt][nCtrCnt] == TRUE)
			{
				nLimitTemp = pFrame->m_pVsSeverIF->m_nLimitTemperature[nPcCnt][nCtrCnt];
				strLimitTemp.Format(_T("%d"),nLimitTemp);
				nCurTemp = pFrame->m_pVsSeverIF->m_nTemperature[nPcCnt][nCtrCnt];
				strCurTemp.Format(_T("%d"),nCurTemp);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP_LIMIT1  + (nPcCnt) *2  + nCtrCnt))->SetCaption(strLimitTemp);
				((CGxStaticEx*)GetDlgItem(IDC_GXLBL_TEMP1 + (nPcCnt) *2  + nCtrCnt))->SetCaption(strCurTemp);
				
				if (_ttoi(strLimitTemp) <= _ttoi(strCurTemp) && nMsgFlg == FALSE)
				{
					CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
					//pSetup->m_pDlgPLC->m_fnSetDevice(ADDR_TEMPERATURE_WARNNING, 1); //2021.01.21 comment by mby
					
					KillTimer(TIMER_UPDATE_TEMPERATURE);
					SetTimer(TIMER_TEMPERATURE_CHECK_WARRNING_OFF, 100, NULL);
					if(m_hWarnnigHandle == NULL)
					{
						UINT uiThreadID;
						m_hWarnnigHandle = (HANDLE)::_beginthreadex(NULL, 0, m_fnCheckErrorClear, (LPVOID)this, 0, &uiThreadID);
					}	
					if (m_pDlgErrorMsg != NULL)
					{
						delete m_pDlgErrorMsg;
						m_pDlgErrorMsg = NULL;
					}
					else
					{
						m_pDlgErrorMsg = new CDlgMessage();			
						m_pDlgErrorMsg->Create(IDD_DLG_MESSAGE);
						m_pDlgErrorMsg->ShowWindow( SW_SHOW );
						m_pDlgErrorMsg->UpdateWindow();
						strMsg.Format(_T("LIGHT TEMPERATURE WARNNING !!"));
						m_pDlgErrorMsg->m_fnChangeCaption(strMsg);
						((CGxStaticEx*)m_pDlgErrorMsg->GetDlgItem(IDOK))->ShowWindow(SW_HIDE);
						//m_pDlgErrorMsg->MoveWindow(1260, 500, 648, 510);						
					}					
					nMsgFlg = TRUE;
				}
				//G_ADDLOG_TEMPERATURE(_T("[VISION PC][%d] [Contol No][%d] [Limit Temperature][%d] [temperature][%d]"), nPcCnt+ 1, nCtrCnt + 1, nLimitTemp, nCurTemp);
			}
		}
	}
}

void CDlgTemperature::m_fnAddLog()
{
	int nLimitTemp = 0, nCurTemp = 0, nCtrNo = 0, nMsgFlg = 0;
	CString strLimitTemp, strCurTemp, strMsg;
	CString strTempFilePath;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	for (int nPcCnt = 0; nPcCnt < pFrame->m_stParamInfo.nCountVisionClient; nPcCnt++)
	{
		for (int nCtrCnt = 0; nCtrCnt < LIGHT_CONTROL_MAX; nCtrCnt++)
		{
			if (pFrame->m_pVsSeverIF->m_bControllerFlag[nPcCnt][nCtrCnt] == TRUE)
			{
				nLimitTemp = pFrame->m_pVsSeverIF->m_nLimitTemperature[nPcCnt][nCtrCnt];
				nCurTemp = pFrame->m_pVsSeverIF->m_nTemperature[nPcCnt][nCtrCnt];
				m_fnSaveFileTemperlInfo(strTempFilePath, nPcCnt+ 1, nCtrCnt + 1, nLimitTemp,  nCurTemp);
				//G_ADDLOG_TEMPERATURE(_T("[VISION PC][%d] [Contol No][%d] [Limit Temperature][%d] [temperature][%d]"), nPcCnt+ 1, nCtrCnt + 1, nLimitTemp, nCurTemp);
			}
		}
	}
}

void CDlgTemperature::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == TIMER_UPDATE_TEMPERATURE)
	{
		m_fnInitState();
		m_fnUpdateState();
	}
	if (nIDEvent == TIMER_TEMPERATURE_LOG)
	{
		m_fnAddLog();
	}
	if (nIDEvent == TIMER_TEMPERATURE_CHECK_WARRNING_OFF )
	{
		/*CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
		CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
		if (pSetup->m_pDlgPLC->m_fnGetDevice(ADDR_TEMPERATURE_WARNNING))
		{
		CString strVal;
		KillTimer(TIMER_TEMPERATURE_CHECK_WARRNIG_OFF);
		((CGxStaticEx*)GetDlgItem(IDC_EDIT_CHK_TERM))->GetWindowText(strVal);
		SetTimer(TIMER_UPDATE_TEMPERATURE, _ttoi(strVal), NULL);
		if (IsWindow(m_pDlgErrorMsg->m_hWnd))
		{
		m_pDlgErrorMsg->OnBnClickedOk();				
		}
		m_pDlgErrorMsg = NULL;
		}*/

		if(m_hWarnnigHandle == NULL)
		{
			if (IsWindow(m_pDlgErrorMsg->m_hWnd))
			{
				m_pDlgErrorMsg->OnBnClickedOk();
			}
			m_pDlgErrorMsg = NULL;
			SetTimer(TIMER_UPDATE_TEMPERATURE, TIMER_UPDATE_TEMPERATURE_INTERVAL, NULL);
			KillTimer(TIMER_TEMPERATURE_CHECK_WARRNING_OFF);			
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CDlgTemperature::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//CDialogEx::OnCancel();
	//DestroyWindow();
	this->ShowWindow(SW_HIDE);
}


void CDlgTemperature::OnBnClickedSave()
{
	CString cCountRoot;
	CString strValue;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_PATH);

	GetDlgItem(IDC_EDIT_CHK_TERM)->GetWindowText(strValue);
	WritePrivateProfileString(EQP_STATE, KEY_TEMPERATURE_CHECK_TERM, strValue, cCountRoot);
	
	KillTimer(TIMER_TEMPERATURE_LOG);
	SetTimer(TIMER_TEMPERATURE_LOG, _ttoi(strValue), NULL);
}

unsigned __stdcall  CDlgTemperature::m_fnCheckErrorClear(LPVOID pArg)
{	
	CString strVisionClientPath,strRootPath;
	int nVisionCount = 1;
	int nCompareVisCnt = 0;
	CMainFrame *pFrame = (CMainFrame *)AfxGetApp()->GetMainWnd();
	CDlgTemperature *pData = (CDlgTemperature *)pArg;
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	while (TRUE){
		Sleep(500);
		if(pSetup->m_pDlgPLC->m_fnGetDevice(ADDR_TEMPERATURE_WARNNING) == 0)
			break;
	}

	CloseHandle(pData->m_hWarnnigHandle);
	pData->m_hWarnnigHandle = NULL;
	return TRUE;
}

void CDlgTemperature::m_fnSaveFileTemperlInfo(CString strTempFilePath, int nPcCnt, int nCtrCnt, int nLimitTemp, int nCurTemp)
{
	CStringArray strArrayData;
	wchar_t chRead[255] = {0}, lpszPath[255] = {0};	
	CString strResultFile, cPath, strLine, cTemperinfo, strTemperatureDataPath;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	//cPath =_T("D:\\Defect Info\\PanelData\\paneldata1.csv");
	//strResultFile.Format(_T("%s_%d.csv"), strPath_, nThreadID_);

	int nWriteTerm = 0, nDeleteTerm = 0;
	CStdioFile cstFile;
	CFileFind finder;

	try
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CTime tTime;
		CString cDataRoot, strDate;
		CString cDataPath, strFileName;
		ULONG nDateCnt = 0, nDateCntTemp = 0, lDataUntilDay = 0;
		SYSTEMTIME	systime;
		BOOL bWorking;

		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_ROOT, _T(""), (LPTSTR)chRead, 255);
		cDataRoot = chRead;
		memset(chRead,0X00,sizeof(chRead));		
		pFrame->m_iniConfigFtp.GetProfileStringW(PNL_DATA_PATH, KEY_DEFINFO_PATH, _T(""), (LPTSTR)chRead, 255);
		cDataPath = chRead;

		nWriteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_WRITE_TERM, 0);		
		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		strTemperatureDataPath.Format(_T("%s%sTEMPERATURE INFO\\"),cDataRoot, cDataPath );

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		HANDLE				hSearch;
		WIN32_FIND_DATA		FileData;
		TSTRING				cLogFileName;
		TSTRING				cDeleteLogFileName;
		bool				bExistFIle;
		SYSTEMTIME			SystemTime;
		FILETIME			TempFileTime, SystemFileTime;
		UINT				uDifferenceDate;
		int					nErrorCode = APP_OK;

		//cLogFileName = cDataRoot + cDataPath + _T("*.csv");	//2015.06.26 edit by sjw for File Path Change
		cLogFileName = strTemperatureDataPath + _T("*.csv");

		hSearch = FindFirstFile(cLogFileName.c_str(), &FileData);

		if (INVALID_HANDLE_VALUE == hSearch) 
		{ 
			FindClose(hSearch);
		//	return nErrorCode;
		}

		bExistFIle = true;


		::GetLocalTime(&SystemTime);
		SystemTimeToFileTime(&SystemTime,&SystemFileTime);

		while(bExistFIle)
		{
			TempFileTime.dwHighDateTime = SystemFileTime.dwHighDateTime - FileData.ftLastWriteTime.dwHighDateTime;
			TempFileTime.dwLowDateTime	= SystemFileTime.dwLowDateTime - FileData.ftLastWriteTime.dwLowDateTime;

			uDifferenceDate				= int((TempFileTime.dwHighDateTime*4294967296 + TempFileTime.dwLowDateTime)/864000000000);

			if(uDifferenceDate >= (UINT)nDeleteTerm)
			{
				//cDeleteLogFileName = cDataRoot + cDataPath + FileData.cFileName;	//2015.06.26 edit by sjw for File Path Change
				cDeleteLogFileName = strTemperatureDataPath + FileData.cFileName;
				DeleteFile(cDeleteLogFileName.c_str());
			}

			if(!FindNextFile(hSearch, &FileData))
			{
				nErrorCode = GetLastError();
				if(ERROR_NO_MORE_FILES == nErrorCode)	
				{
					bExistFIle = false;
					nErrorCode = APP_OK;
				}
				else
				{
					_tprintf(_T("CLogWriter::m_fnDeletePeriodOverFile -> Couldn't find next file.\n"));
					bExistFIle = false;
				}
			}
		}

		FindClose(hSearch);
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		pFrame->m_fnCreateFolder(strTemperatureDataPath);
		
		::GetLocalTime(&systime);
		
		//bWorking = finder.FindFile(cDataRoot + cDataPath +_T("*.*"));	// 모든화일
		bWorking = finder.FindFile(strTemperatureDataPath +_T("*.*"));	// 모든화일

		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 화일하나를 찾음

			if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

			
			strFileName = finder.GetFileName();

			//if(finder.IsDots()) continue;

			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				nDateCntTemp = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100 + nWriteTerm; // + tTime.GetHour();

				if (nDateCnt < nDateCntTemp)
				{
					nDateCnt = nDateCntTemp;
					lDataUntilDay = tTime.GetYear()*1000000 + tTime.GetMonth()*10000 + tTime.GetDay()*100;
				}	
				
			}
			
		}    

		nDateCntTemp = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + systime.wHour;

		if (nDateCnt == 0)
		{
			nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;//systime.wHour;
		}
		else if (lDataUntilDay < systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100)
		{
			int ntDay = nDateCnt - nDateCntTemp;
			if ( abs(ntDay) >= 100)
			{
				nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;				
			}
			else
			{
				if (nWriteTerm < systime.wHour)
				{
					nDateCnt = nDateCntTemp;
				}
				if (nWriteTerm == 0)
				{
					nDateCnt = systime.wYear*1000000 + systime.wMonth*10000 + systime.wDay*100 + nWriteTerm;		
				}
			}
		}
		else
		{			
			if (nDateCnt < nDateCntTemp)
			{
				if (nWriteTerm <= systime.wHour)
				{
					nDateCntTemp = (nDateCntTemp - systime.wHour) + nWriteTerm;
					nDateCnt = nDateCntTemp;
				}
			}
		}
		
		finder.Close();
		cPath.Format(_T("%sTemperature_%d.csv"), strTemperatureDataPath ,nDateCnt);
		strDate.Format(_T("%04d/%02d/%02d  %02d:%02d:%02d"), systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
		if (!pFrame->m_DataManager.FileExist(cPath))
		{
			strLine = _T("Temperature Time			,\
						  VISION PC					,\
						  Contol No					,\
						  Limit Temperature			,\
						  temperature				 ");//2015.09.17 Mr.Chu 

		}

		if (!cstFile.Open(cPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
		{
			G_AddLog(2, 3, _T("m_fnSaveFileTemperlInfo File Write Error %s : %d"), cPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();
		
		cTemperinfo.Format(_T("\n%s,%d,%d,%d,%d"),	
			strDate,
			nPcCnt,
			nCtrCnt,
			nLimitTemp,
			nCurTemp			
			);	

		strLine.Append(cTemperinfo);
	
		cstFile.WriteString(strLine);

		cstFile.Close();
	}
	catch (CMemoryException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CFileException* e)
	{
		e->Delete();
		cstFile.Close();
	}
	catch (CException* e)
	{
		e->Delete();
		cstFile.Close();
	}
}
