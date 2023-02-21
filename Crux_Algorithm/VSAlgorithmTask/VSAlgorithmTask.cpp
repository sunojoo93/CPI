
// VSAlgorithmTask.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "VSAlgorithmTask.h"
#include <conio.h>
#include "Markup.h"
#include <omp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CVSAlgorithmTaskApp

BEGIN_MESSAGE_MAP(CVSAlgorithmTaskApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVSAlgorithmTaskApp ����

CVSAlgorithmTaskApp::CVSAlgorithmTaskApp()
	:m_bIPCConnect(FALSE), m_bExecDisk(FALSE)		// Disk Check Thread ���� �÷��� �ʱ�ȭ �߰�
{
	// �ٽ� ���� ������ ����
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	InitializeCriticalSection(&m_csCntFileSafe);
	InitializeCriticalSection(&m_csJudgeRepeatCount);
	InitializeCriticalSection(&theApp.m_SVICoordFile);
}

CVSAlgorithmTaskApp::~CVSAlgorithmTaskApp()
{
	DeleteCriticalSection(&m_csCntFileSafe);
	DeleteCriticalSection(&m_csJudgeRepeatCount);
	ExitVision();
	DeleteCriticalSection(&theApp.m_SVICoordFile);
}

// ������ CVSAlgorithmTaskApp ��ü�Դϴ�.

CVSAlgorithmTaskApp theApp;


// CVSAlgorithmTaskApp �ʱ�ȭ

BOOL CVSAlgorithmTaskApp::InitInstance()
{
	//���α׷� �ߺ� ���� ���� - S
	// �ߺ� ���� ���� ���� - B11 PC1:Stage2 ���� �۾�
// 	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("VsAlgorithmTask"));
// 	if(GetLastError() == ERROR_ALREADY_EXISTS)
// 	{
// 		ReleaseMutex(hMutex);
// 
// 		CWnd *pWndPrev, *pWndChild;
// 		pWndPrev = CWnd::FindWindow(NULL, _T("VsAlgorithmTask"));
// 		if(pWndPrev)
// 		{
// 			pWndChild = pWndPrev->GetLastActivePopup();
// 
// 			if(pWndChild->IsIconic())
// 				pWndPrev->ShowWindow(SW_RESTORE);
// 
// 			pWndChild->SetForegroundWindow();
// 		}	
// 		return FALSE;
// 	}
// 	ReleaseMutex(hMutex);
	//���α׷� �ߺ� ���� ���� - E

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;	
	
	//Alg Task�� �����Ⱓ�� ���� Log ���� �����ϵ��� �߰� 190716 YWS
	int uPeriodDay = GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, VS_ALGORITHM_TASK_INI_FILE);

	// Log ��ü ����
	for (int i=0; i<MAX_CAMERA_COUNT; i++)
	{
		CString strCam;
		strCam.Format(_T("CAM%02d"), i);
		m_pLogWriterCam[i] = new CAlgLogWriter(ALGORITHM_LOG_PATH, strCam.GetBuffer(0), uPeriodDay/*30*/, 0, true);
	}
	m_pLogWriterProc = new CAlgLogWriter(ALGORITHM_LOG_PATH, _T("PROCESS"), uPeriodDay/*30*/, 0, true);
	m_pLogWriterTact = new CAlgLogWriter(ALGORITHM_LOG_PATH, _T("TACT"), uPeriodDay/*30*/, 0, true);
	m_pLogWriterComm = new CAlgLogWriter(ALGORITHM_LOG_PATH, _T("COMMUNICATION"), uPeriodDay/*30*/, 0, true);

	// Command Line ��ɾ� �޾ƿ���
	int iCnt = 0;
	LPWSTR* pStr = NULL;
	pStr = CommandLineToArgvW(GetCommandLine(), &iCnt);

	CString strRegKey;

	// ���� ���� Load
	m_Config.Load();	

	// Grab Step �ִ� ���� ��ŭ ����
	m_pGrab_Step = new STRU_INFO_GRAB_STEP[MAX_GRAB_STEP_COUNT];	

	// Visual Station ����
	m_bExecIPC = TRUE;
	m_hEventIPCThreadAlive = NULL;
	m_hEventIPCThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );
	ResetEvent(m_hEventIPCThreadAlive);
	m_pVSThread = AfxBeginThread(ThreadConnectVSServer, this);

	// Disk Check	
	m_hEventDiskThreadAlive = NULL;
	m_hEventDiskThreadAlive = CreateEvent( NULL, TRUE,  FALSE,  NULL );

	m_pDiskInfo = new CDiskInfo();
	m_pDiskInfo->CreateDriveInfoVector();
	CString strDrive,strTemp;
	if( !theApp.m_Config.GetSimualtionMode() )
	{
		int nCount = 0;
		while(1)
		{
			if(!AfxExtractSubString(strDrive, m_Config.GetUseDrive(), nCount++, ','))
				break;
			m_pDiskInfo->SetUseDisk(strDrive);
		}

		m_bExecDisk = TRUE;
		//m_pDiskCheckThread = AfxBeginThread(ThreadDiskCheck, this);
	}
	else
	{
		strDrive = m_Config.GetSimulationDrive();
 		m_pDiskInfo->SetUseDisk(strDrive.Left(1));
		//strDrive = strDrive.Left(1) + _T("_Drive") + _T("\\");
 	}

 	

	strRegKey.Format(_T("YWDSP Sequence Task"));

	LocalFree(pStr);

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.	
	SetRegistryKey(strRegKey);

	// �ʱ�ȭ
	m_fnInitFunc();

	// YHS 18.03.12 - Merge Tool ��� ����
	m_nInspStep		= GetPrivateProfileInt(_T("MergeTool"),_T("USE_MERGE_TOOL"),1,INIT_FILE_PATH)? true : false;

	m_pDlg = new CVSAlgorithmTaskDlg();
	INT_PTR nResponse = m_pDlg->DoModal();

	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// IPC ���� ���
	m_bExecIPC = FALSE;
	::WaitForSingleObject(m_hEventIPCThreadAlive, INFINITE);
	ResetEvent(m_hEventIPCThreadAlive);
	if (m_bExecDisk)
	{
		m_bExecDisk = FALSE;
		::WaitForSingleObject(m_hEventDiskThreadAlive, INFINITE);
	}
	ResetEvent(m_hEventDiskThreadAlive);
	m_pVSThread = NULL;
	m_pDiskCheckThread = NULL;

	for (int i=0; i<MAX_CAMERA_COUNT; i++)
		SAFE_DELETE(m_pLogWriterCam[i]);
	SAFE_DELETE(m_pLogWriterProc);
	SAFE_DELETE(m_pLogWriterTact);
	SAFE_DELETE(m_pLogWriterComm);
	SAFE_DELETE(m_pDlg);
	SAFE_DELETE_ARR(m_pGrab_Step);
	SAFE_DELETE(m_pDiskInfo);

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

UINT CVSAlgorithmTaskApp::ThreadConnectVSServer(LPVOID pParam)
{
	BOOL bRet = FALSE;

	CVSAlgorithmTaskApp* pThis = (CVSAlgorithmTaskApp*)pParam;

	// Visual Station ����
	int					nRet;

	do 
	{
		nRet = pThis->m_fnConectVisualStation();
		if (APP_OK != nRet)		Sleep(1000);
	} while (APP_OK != nRet && pThis->m_bExecIPC);

	printf("Internal Sequence Start \n");

	//if (pThis->m_AlgorithmTask != NULL)
	//	pThis->m_AlgorithmTask->SendNotifyInitial();

	// VS ���� üũ
	while(pThis->m_bExecIPC)
	{
		// VS ���� ���� Get
		if (pThis->m_AlgorithmTask != NULL)
			// VS ���� ���� Get
			pThis->SetIPCState(pThis->m_AlgorithmTask->GetVSState());
		Sleep(1000);
	}

	pThis->SetIPCState(FALSE);
	//SAFE_DELETE(pInternalServer);

	SetEvent(pThis->m_hEventIPCThreadAlive);
	printf("Internal Sequence End \n");

	return 0;
}

int CVSAlgorithmTaskApp::m_fnConectVisualStation()
{
	if (m_AlgorithmTask != NULL)
	{
		return APP_OK;
	}
	TCHAR szAlgLogPath[100] = {0,}, szSeqFileName[100] = {0,}, szServerIP[100] = {0,};
	int nTaskNo, nServerPort, uPeriodDay, uWriteTerm, uWriteLogLevel;	

	// Sequence Task �� PC �� �ϳ��� �־�� ��
	nTaskNo				= (m_Config.GetPCNum() * 100)
		+ (GetPrivateProfileInt(_T("VSServerInfo"), _T("TaskNo"), 21, VS_ALGORITHM_TASK_INI_FILE));
	nServerPort			= GetPrivateProfileInt(_T("VSServerInfo"), _T("ServerPort"), 5000, VS_ALGORITHM_TASK_INI_FILE);
	uPeriodDay			= GetPrivateProfileInt(_T("LogInfo"), _T("PeriodDay"), 30, VS_ALGORITHM_TASK_INI_FILE);
	uWriteTerm			= GetPrivateProfileInt(_T("LogInfo"), _T("WriteTerm"), 0, VS_ALGORITHM_TASK_INI_FILE);
	uWriteLogLevel		= GetPrivateProfileInt(_T("LogInfo"), _T("WriteLogLevel"), 3, VS_ALGORITHM_TASK_INI_FILE);
	// ���� �ַ�� ���� ����̺� �Ǵ� �߰�
	GetPrivateProfileString(_T("LogInfo"), _T("Path") , theApp.m_Config.GETDRV() + _T(":\\VisualStation\\VSLOG\\VSPLC"), szAlgLogPath, sizeof(szAlgLogPath), VS_ALGORITHM_TASK_INI_FILE);
	GetPrivateProfileString(_T("LogInfo"), _T("LogName") ,_T("CRUX"), szSeqFileName, sizeof(szSeqFileName), VS_ALGORITHM_TASK_INI_FILE);
	GetPrivateProfileString(_T("VSServerInfo"), _T("ServerIP") ,_T("127.0.0.1"), szServerIP, sizeof(szServerIP), VS_ALGORITHM_TASK_INI_FILE);

	m_AlgorithmTask = new TaskInterfacer(nTaskNo, nServerPort, szServerIP, szAlgLogPath,
		szSeqFileName, false, uPeriodDay, uWriteTerm, uWriteLogLevel);
	int nRet = m_AlgorithmTask->Start();
	if (nRet != APP_OK)
	{
		SAFE_DELETE(m_AlgorithmTask);
	}
	return nRet;
}

// �� �� �ʿ� �� DLL ������ ����
double CVSAlgorithmTaskApp::CallFocusValue(Mat matSrc, CRect rect)
{
	// ���� ���� �� ���� ���
	//if (matSrc == NULL) return 0;
	if (matSrc.empty()) return 0;

	// ���� ���� ���� �Ѿ�� ���
	if (rect.left < 0) return 0;
	if (rect.top < 0) return 0;
	if (rect.right > matSrc.cols) return 0;
	if (rect.bottom > matSrc.rows) return 0;

	// Rect ���� �̻��� ���
	if (rect.left == rect.right) return 0;
	if (rect.top == rect.bottom) return 0;

	Mat matSobel;
	Mat temp = Mat::zeros(matSrc.rows, matSrc.cols, matSrc.type());

	// GaussianBlur
	//Cv2.GaussianBlur(matSrc, matSobel, new OpenCvSharp.Size(3, 3), 1.5);
	//cv::imwrite("D:\\imageTest1.bmp", matSrc);
	cv::GaussianBlur(matSrc, matSobel, cvSize(3, 3), 1.5);
	//cv::imwrite("D:\\imageTest2.bmp", matSobel);
	// Sobel edge
	//Cv2.Sobel(matSobel, matSobel, MatType.CV_8UC1, 1, 1);
	//cv::Sobel(matSobel,matSobel,CV_8UC1,1,1,5);
	//cv::imwrite("D:\\imageTest3.bmp", matSobel);

	//////////////////////////////////////////////////////////////////////////
	// KSW 17.11.01 - ǥ�������� ����

	// mean & stdDev ���ϱ�
	cv::Scalar m, s;
	cv::meanStdDev(matSobel, m, s);

	matSobel.release();

	return s[0];
	//////////////////////////////////////////////////////////////////////////

	long nSum = 0;

	for (int y = rect.top+1; y < rect.bottom-1; y++)
	{
		for (int x = rect.left+1; x < rect.right-1; x++)
		{
			long h1 =	-	matSobel.at<BYTE>(y-1, x-1)
				- 2 *	matSobel.at<BYTE>(y-1, x) 
				-		matSobel.at<BYTE>(y-1, x+1)
				+		matSobel.at<BYTE>(y+1, x-1)
				+ 2 *	matSobel.at<BYTE>(y+1, x)
				+		matSobel.at<BYTE>(y+1, x+1);

			long h2 =	-	matSobel.at<BYTE>(y-1, x-1)
				- 2 *	matSobel.at<BYTE>(y, x-1)
				-		matSobel.at<BYTE>(y+1, x-1)
				+		matSobel.at<BYTE>(y-1, x+1)
				+ 2 *	matSobel.at<BYTE>(y, x+1)
				+		matSobel.at<BYTE>(y+1, x+1);

			nSum += (long)sqrt( (double)(h1*h1 + h2*h2) );

			temp.at<BYTE>(y, x) = (BYTE)sqrt( (double)(h1*h1 + h2*h2) );
		}
	}

	//	cv::imwrite("D:\\imageTest2.bmp", temp);

	// 	������ Rect ��ŭ
	// 		for (int y = rect.top; y < rect.bottom; y++)
	// 		{
	// 			// ������ ù��° �迭 �ּ� ��������
	// 			// �̹� ������ �ּ� �� ( fixed ��� ���ص� �� )
	// 			byte* ptr = (byte*)matSobel.ptr(y) + rect.left;
	// 	
	// 			for (int x = rect.left; x < rect.right; x++)
	// 			{
	// 				// �ش� GV�� ����
	// 				nSum += *ptr++;
	// 			}
	// 		}


	// ���� ���� ����
	matSobel.release();

	// Rect ��, ��� ���	
	return nSum / (float)(rect.Width() * rect.Height());
}

int CVSAlgorithmTaskApp::GetInsTypeIndex(int nImageNum, int nCameraNum, int nROINum, CString strAlgName)
{
	for (int i=0; i<MAX_MEM_SIZE_E_ALGORITHM_NUMBER; i++)
	{
		if (strAlgName.CompareNoCase(m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].ROI[nROINum].AlgorithmList[i].strAlgName) == 0)
			return i;
	}
	return -1;
}

bool CVSAlgorithmTaskApp::ReadAlgorithmParameter(TCHAR* strModelPath, TCHAR* strCornerEdgePath)
{
	// ���� ���� ����
	CFileFind find;
	BOOL bFindFile = FALSE;
	CString strMsg = _T("");

	CCPUTimer timerReadParameter;
	timerReadParameter.Start();

	//////////////////////////////////// �˰��� ����Ʈ ���� �ε�
	CString strAlgListXMLPath;
	// ���� �ַ�� ���� ����̺� �Ǵ� �߰�
	strAlgListXMLPath.Format(_T("%s:\\CRUX\\DATA\\TXT\\ALGORITHM_LIST.xml"), theApp.m_Config.GETDRV());

	bFindFile = find.FindFile(strAlgListXMLPath);
	find.Close();

	if (!bFindFile)
	{
		strMsg.Format(_T("Not found algorithm list xml file. (%s)"), strAlgListXMLPath);
		AfxMessageBox(strMsg);
		return false;
	}	

	// XML ���� �ε�
	CMarkup xmlAlgList;
	if(!xmlAlgList.Load(strAlgListXMLPath))
	{
		strMsg.Format(_T("Model xml load fail. (%s)"), strAlgListXMLPath);
		AfxMessageBox(strMsg);
		return false;
	}

	xmlAlgList.FindElem();		// ALGORITHM_LIST
	xmlAlgList.IntoElem();		// inside ALGORITHM_LIST

	CString strAlgorithmName = _T("");
	CMarkup* xmlAlgorithmList = new CMarkup(xmlAlgList);
	//m_MapAlgList.clear();
	map<CString, UINT> MapAlgList;
	MapAlgList.clear();
	for (int nAlgListIndex=0; nAlgListIndex<MAX_MEM_SIZE_E_ALGORITHM_NUMBER; nAlgListIndex++)
	{		
		if (xmlAlgorithmList->FindElem(_T("ALGORITHM_%d"), nAlgListIndex))
		{
			strAlgorithmName = xmlAlgorithmList->GetAttrib(_T("Name"));
			MapAlgList[strAlgorithmName] = nAlgListIndex;
			xmlAlgorithmList->ResetMainPos();
		}
	}
	SetAlgoritmList(MapAlgList);
	SAFE_DELETE(xmlAlgorithmList);

	//////////////////////////////////// ���� ������(Ÿ��) ���� �ε�
	CString strDefItemListXMLPath;
	// ���� �ַ�� ���� ����̺� �Ǵ� �߰�
	strDefItemListXMLPath.Format(_T("%s:\\CRUX\\DATA\\TXT\\DEFITEM_LIST.xml"), theApp.m_Config.GETDRV());

	bFindFile = find.FindFile(strDefItemListXMLPath);
	find.Close();

	if (!bFindFile)
	{
		strMsg.Format(_T("Not found defect item list xml file. (%s)"), strDefItemListXMLPath);		
		AfxMessageBox(strMsg);		
		return false;
	}

	// XML ���� �ε�
	CMarkup xmlDefectItem;
	if(!xmlDefectItem.Load(strDefItemListXMLPath))
	{
		strMsg.Format(_T("Model xml load fail. (%s)"), strDefItemListXMLPath);
		AfxMessageBox(strMsg);
		return false;
	}

	xmlDefectItem.FindElem();		// DEF_ITEM
	xmlDefectItem.IntoElem();		// inside DEF_ITEM

	CString strDefItemName = _T("");
	CMarkup* xmlDefItemList = new CMarkup(xmlDefectItem);
	//m_MapDefItemList.clear();
	map<CString, UINT> MapDefItemList;
	MapDefItemList.clear();
	for (int nDefItemIndex=0; nDefItemIndex<MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; nDefItemIndex++)
	{		
		if (xmlDefItemList->FindElem(_T("DefType_%d"), nDefItemIndex))
		{
			strDefItemName = xmlDefItemList->GetAttrib(_T("Name"));
			MapDefItemList[strDefItemName] = nDefItemIndex;
			xmlDefItemList->ResetMainPos();
		}		
	}
	SetDefectItemList(MapDefItemList);
	SAFE_DELETE(xmlDefItemList);

	/////////////////////////////////// �� ���� �ε�
	CString strModelXMLPath;
	// RMS ���� ������ ��� ����
	//strModelXMLPath.Format(_T("%s\\%s\\Grab_List.xml"), MODEL_FILE_PATH, strModelID);
	strModelXMLPath.Format(_T("%s\\Grab_List.xml"), strModelPath);

	bFindFile = find.FindFile(strModelXMLPath);
	find.Close();
	if (!bFindFile)
	{
		strMsg.Format(_T("Not found model xml file. (%s)"), strModelXMLPath);
		AfxMessageBox(strMsg);		
		return false;
	}

	// XML ���� �ε�
	CMarkup xmlFile;
	if(!xmlFile.Load(strModelXMLPath))
	{
		strMsg.Format(_T("Model xml load fail. (%s)"), strModelXMLPath);
		AfxMessageBox(strMsg);
		return false;
	}

	xmlFile.FindElem();		// Recipe
	xmlFile.IntoElem();		// inside Recipe

	//LJH ������ ���� ����/////////////////////////////////////////////////////
	CMarkup* xmlList = new CMarkup(xmlFile);

	CString EQP = _T("");
	CString changeParamListDate = _T("");
	CString testAlgType = _T("");
	CString verMsg = _T("");

	if (xmlList->FindElem(_T("VERSION")))
	{
		CString verFullname;
		verFullname = xmlList->GetAttrib(_T("NAME"));		

		// ������ ������ �迭
		CString rcpVer[3] = {0,};

		for (int j = 0; j < 3; j++) 
			AfxExtractSubString(rcpVer[j], verFullname, j, '_');

		EQP.Format(_T("%s"), rcpVer[0]);
		changeParamListDate.Format(_T("%s"), rcpVer[1]);
		testAlgType.Format(_T("%s"), rcpVer[2]);
	}
	SAFE_DELETE(xmlList)

	if (testAlgType == "") 
		verMsg.Format((_T("%s"), EQP) + _T("_") + (_T("%s"), changeParamListDate));
	else
		verMsg.Format((_T("%s"), EQP) + _T("_") + (_T("%s"), changeParamListDate) + _T("_") + (_T("%s"), testAlgType));

//	theApp.m_AlgorithmTask->VS_Send_RcpVer_To_UI(verMsg);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	try{
		//memset(m_pGrab_Step, 0, sizeof(STRU_INFO_GRAB_STEP) * MAX_GRAB_STEP_COUNT);
		STRU_INFO_GRAB_STEP* pGrabStepInfo = new STRU_INFO_GRAB_STEP[MAX_GRAB_STEP_COUNT];

		// Grab Step ����
		int nGrabNum = 0;
		//#pragma omp parallel default(shared)
		{
#pragma omp parallel for reduction(+:nGrabNum) schedule(dynamic)
			for (int nGrabCnt=0; nGrabCnt<MAX_GRAB_STEP_COUNT; nGrabCnt++)
			{
				CMarkup* xmlList = new CMarkup(xmlFile);
				if (xmlList->FindElem(_T("GRAB_%d"), nGrabCnt))
				{
					nGrabNum++;
#pragma region >> Get Grab Step List
					STRU_INFO_GRAB_STEP* stInfoGrabStep = &pGrabStepInfo[nGrabCnt];
					stInfoGrabStep->bUse = xmlList->GetBoolAttrib(_T("USE"));
	// 17.07.03 Step ������� �ʾƵ� ������ ���� �ҷ������� ����
	// 				if (stInfoGrabStep->bUse)
	// 				{
						stInfoGrabStep->eImgClassify = _ttoi(xmlList->GetAttrib(_T("AlgNo")));
						_tcscpy(stInfoGrabStep->strGrabStepName, xmlList->GetAttrib(_T("NAME")));

						/////////////////////////////////// ���Ϻ� ���� �ε�
						CMarkup xmlPatternFile;

						CString strPatternXMLPath;
					// RMS ���� ������ ��� ����
					//strPatternXMLPath.Format(_T("%s\\%s\\Grab_List\\%s.xml"), MODEL_FILE_PATH, strModelID, stInfoGrabStep->strGrabStepName);
					strPatternXMLPath.Format(_T("%s\\Grab_List\\%s.xml"), strModelPath, stInfoGrabStep->strGrabStepName);

						CFileFind findPatternFile;
						BOOL bFindPatternFile = findPatternFile.FindFile(strPatternXMLPath);
						findPatternFile.Close();
						if (!bFindPatternFile)
						{					
							strMsg.Format(_T("Not found pattern xml file. (%s)"), strPatternXMLPath);
							AfxMessageBox(strMsg);					
						}
						else
						{
							if(!xmlPatternFile.Load(strPatternXMLPath)) 					
							{
								strMsg.Format(_T("Pattern xml load fail. (%s)"), strPatternXMLPath);
								AfxMessageBox(strMsg);
							}
							else
							{
								xmlPatternFile.FindElem();		// Recipe
								xmlPatternFile.IntoElem();		// inside Recipe

								//#pragma omp parallel shared(nGrabCnt)
								{
									// ī�޶� ����
									//#pragma omp parallel for schedule(dynamic)		// ���� Camera ������ 1~2���̹Ƿ� ��ȿ����
									for (int nCamCnt=0; nCamCnt<MAX_CAMERA_COUNT; nCamCnt++)
									{
#pragma region >> Get Parameter Each Camera
										CMarkup* xmlPattern = new CMarkup(xmlPatternFile);
										if (xmlPattern->FindElem(_T("CAM_%d"), nCamCnt))
										{
											stInfoGrabStep->nCamCnt++;
											STRU_INFO_CAMERA* stInfoCam = &stInfoGrabStep->stInfoCam[nCamCnt];
											stInfoCam->bUse = xmlPattern->GetBoolAttrib(_T("USE"));
											if (stInfoCam->bUse)
											{
												xmlPattern->IntoElem();

												if (xmlPattern->FindElem(_T("AD")))		/// 17.04.11 AD Parameter �߰�
												{
													stInfoCam->bUseAD = xmlPattern->GetBoolAttrib(_T("USE"));
													if (stInfoCam->bUseAD)
													{
														xmlPattern->IntoElem();
														for (int nADPrmCnt=0; nADPrmCnt<MAX_MEM_SIZE_AD_PARA_TOTAL_COUNT; nADPrmCnt++)
														{
															if (xmlPattern->FindElem(_T("PARAM_%d"), nADPrmCnt))
															{															
																stInfoCam->dADPara[nADPrmCnt] = _ttof(xmlPattern->GetAttrib(_T("VALUE")));
																xmlPattern->ResetMainPos();
															}
														}
														xmlPattern->OutOfElem();
													}
													xmlPattern->ResetMainPos();
												}
												for (int nROICnt=0; nROICnt<MAX_MEM_SIZE_ROI_COUNT; nROICnt++)
												{
													if (xmlPattern->FindElem(_T("ROI_%d"), nROICnt))
													{
														stInfoCam->nROICnt++;
														STRU_INFO_ROI* stInfoROI = &stInfoCam->ROI[nROICnt];
														stInfoROI->bUseROI = xmlPattern->GetBoolAttrib(_T("USE"));
														if (stInfoROI->bUseROI)
														{
															_tcscpy(stInfoROI->strROIName,	xmlPattern->GetAttrib(_T("NAME")));
															stInfoROI->rectROI.left =		_ttoi(xmlPattern->GetAttrib(_T("START_X")));
															stInfoROI->rectROI.top =			_ttoi(xmlPattern->GetAttrib(_T("START_Y")));
															stInfoROI->rectROI.right =		_ttoi(xmlPattern->GetAttrib(_T("END_X")));
															stInfoROI->rectROI.bottom =		_ttoi(xmlPattern->GetAttrib(_T("END_Y")));

															xmlPattern->IntoElem();
															// �˰��� ����Ʈ �� ����
															for (int nAlgCnt=0; nAlgCnt<MAX_MEM_SIZE_E_ALGORITHM_NUMBER; nAlgCnt++)
															{
																if (xmlPattern->FindElem(_T("ALGORITHM_%d"), nAlgCnt))
																{
																	// ���� - ī�޶� - �� �˰����� �׸��� ���� �ٸ� �� �����Ƿ� (ex:CAM1�������� POINT �˰��� XML�� ���� �� GUI ǥ��)
																	// ���������� �������� �ʰ� �ش� �˰��� �̸��� ���� �޾ƿͼ� �ε����� ã�Ƽ� �Ķ��Ÿ�� �������� (ALGORITHM_LIST.xml ����)
																	CString strAlgorithmName = xmlPattern->GetAttrib(_T("NAME"));
																	int nAlgorithmIndex = theApp.GetAlgorithmIndex(strAlgorithmName);
																	if (nAlgorithmIndex < 0)
																	{
																		AfxMessageBox(_T("Not Found Algorithm : ") + strAlgorithmName + _T("\r\n(") + strAlgListXMLPath + _T(")"));
																	}
																	STRU_PARAM_ALG* stInfoAlg = &stInfoROI->AlgorithmList[nAlgorithmIndex];
																	stInfoAlg->bAlgorithmUse = xmlPattern->GetBoolAttrib(_T("USE"));
																	if (stInfoAlg->bAlgorithmUse)
																	{
																		COPY_CSTR2TCH(stInfoAlg->strAlgName, strAlgorithmName, sizeof(stInfoAlg->strAlgName));
																		xmlPattern->IntoElem();

																		// �˰��� �Ķ����
																		for (int nAlgPrmCnt=0; nAlgPrmCnt<MAX_MEM_SIZE_ALG_PARA_TOTAL_COUNT; nAlgPrmCnt++)
																		{
																			if (xmlPattern->FindElem(_T("PARAM_%d"), nAlgPrmCnt))
																			{															
																				stInfoAlg->dPara[nAlgPrmCnt] = _ttof(xmlPattern->GetAttrib(_T("VALUE")));
																				xmlPattern->ResetMainPos();
																			}
																		}

																		// ���� ������ �� ����
																		for (int nDefItemCnt=0; nDefItemCnt<MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; nDefItemCnt++)
																		{												
																			if (xmlPattern->FindElem(_T("DEF_ITEM_%d"), nDefItemCnt))
																			{
																				stInfoAlg->nDefectItemCount++;
																				// ���� - ī�޶� - �˰��� �� DefectType�� �׸��� ���� �ٸ� �� �����Ƿ� (ex:POINT �˰��򿡴� POINT �ҷ��� XML�� ���� �� GUI ǥ��)
																				// ���������� �������� �ʰ� �ش� �ҷ� �̸��� ���� �޾ƿͼ� �ε����� ã�Ƽ� �Ķ��Ÿ�� �������� (DEFITEM_LIST.xml ����)
																				CString strDefItemName = xmlPattern->GetAttrib(_T("NAME"));
																				int nDefItemIndex = theApp.GetDefectTypeIndex(strDefItemName);
																				if (nDefItemIndex < 0)
																				{
																					AfxMessageBox(_T("Not Found Def Item : ") + strDefItemName + _T("\r\n(") + strDefItemListXMLPath + _T(")"));
																				}
																				STRU_DEFECT_ITEM* stInfoDefItem = &stInfoAlg->stDefectItem[nDefItemIndex];
																				stInfoDefItem->bDefectItemUse = xmlPattern->GetBoolAttrib(_T("USE"));
																				if(stInfoDefItem->bDefectItemUse)
																				{
																					COPY_CSTR2TCH(stInfoDefItem->strItemName, strDefItemName, sizeof(stInfoDefItem->strItemName));

																					xmlPattern->IntoElem();

																					// ���� ������ �� ���� ��
																					for (int nJudgeCnt=0; nJudgeCnt<MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; nJudgeCnt++)
																					{
																						if (xmlPattern->FindElem(_T("JUDGEMENT_%d"), nJudgeCnt))
																						{
																							STRU_JUDGEMENT* stInfoJudge = &stInfoDefItem->Judgment[nJudgeCnt];

																							stInfoJudge->bUse = xmlPattern->GetBoolAttrib(_T("USE"));
																							stInfoJudge->dValue = _ttof(xmlPattern->GetAttrib(_T("VALUE")));
																							CString strCond = xmlPattern->GetAttrib(_T("COND"));
																							if (strCond.CompareNoCase(_T("=")) == 0)			stInfoJudge->nSign = 0;
																							else if (strCond.CompareNoCase(_T("<>")) == 0)		stInfoJudge->nSign = 1;
																							else if (strCond.CompareNoCase(_T(">")) == 0)		stInfoJudge->nSign = 2;
																							else if (strCond.CompareNoCase(_T("<")) == 0)		stInfoJudge->nSign = 3;
																							else if (strCond.CompareNoCase(_T(">=")) == 0)		stInfoJudge->nSign = 4;
																							else if (strCond.CompareNoCase(_T("<=")) == 0)		stInfoJudge->nSign = 5;
																							xmlPattern->ResetMainPos();
																						}
																					}
																					xmlPattern->OutOfElem();
																				}
																				xmlPattern->ResetMainPos();
																			}
																		}
																		xmlPattern->OutOfElem();
																	}												
																	xmlPattern->ResetMainPos();
																}
															}
															xmlPattern->OutOfElem();
														}
														xmlPattern->ResetMainPos();
													}
												}
												for (int nNonROICnt=0; nNonROICnt<MAX_MEM_SIZE_E_INSPECT_AREA; nNonROICnt++)
												{
													if (xmlPattern->FindElem(_T("NON_%d"), nNonROICnt))
													{
														stInfoCam->nNonROICnt++;
														INSP_AREA* stInfoArea = &stInfoCam->NonROI[nNonROICnt];
														stInfoArea->bUseROI = xmlPattern->GetBoolAttrib(_T("USE"));
														if (stInfoArea->bUseROI)
														{
															COPY_CSTR2TCH(stInfoArea->strROIName, xmlPattern->GetAttrib(_T("NAME")), sizeof(stInfoArea->strROIName));
															stInfoArea->rectROI.left		=	_ttoi(xmlPattern->GetAttrib(_T("START_X")));
															stInfoArea->rectROI.top		=	_ttoi(xmlPattern->GetAttrib(_T("START_Y")));
															stInfoArea->rectROI.right	=	_ttoi(xmlPattern->GetAttrib(_T("END_X")));
															stInfoArea->rectROI.bottom	=	_ttoi(xmlPattern->GetAttrib(_T("END_Y")));
														}
														xmlPattern->ResetMainPos();
													}								
												}
												for (int nRndROICnt=0; nRndROICnt<MAX_MEM_SIZE_E_INSPECT_AREA; nRndROICnt++)
												{
													if (xmlPattern->FindElem(_T("RND_%d"), nRndROICnt))
													{
														stInfoCam->nRndROICnt++;
														INSP_AREA* stInfoArea = &stInfoCam->RndROI[nRndROICnt];
														stInfoArea->bUseROI = xmlPattern->GetBoolAttrib(_T("USE"));
														if (stInfoArea->bUseROI)
														{
															COPY_CSTR2TCH(stInfoArea->strROIName, xmlPattern->GetAttrib(_T("NAME")), sizeof(stInfoArea->strROIName));
															stInfoArea->rectROI.left		=	_ttoi(xmlPattern->GetAttrib(_T("START_X")));
															stInfoArea->rectROI.top		=	_ttoi(xmlPattern->GetAttrib(_T("START_Y")));
															stInfoArea->rectROI.right	=	_ttoi(xmlPattern->GetAttrib(_T("END_X")));
															stInfoArea->rectROI.bottom	=	_ttoi(xmlPattern->GetAttrib(_T("END_Y")));
														}
														xmlPattern->ResetMainPos();
													}								
												}

												//2019.02.20 for Hole ROI
												for (int nHoleROICnt = 0; nHoleROICnt < MAX_MEM_SIZE_E_INSPECT_AREA; nHoleROICnt++)
												{
													if (xmlPattern->FindElem(_T("HOLE_%d"), nHoleROICnt))
													{
														stInfoCam->nHoleROICnt++;
														INSP_AREA* stInfoArea = &stInfoCam->HoleROI[nHoleROICnt];
														stInfoArea->bUseROI = xmlPattern->GetBoolAttrib(_T("USE"));
														if (stInfoArea->bUseROI)
														{
															COPY_CSTR2TCH(stInfoArea->strROIName, xmlPattern->GetAttrib(_T("NAME")), sizeof(stInfoArea->strROIName));
															stInfoArea->rectROI.left = _ttoi(xmlPattern->GetAttrib(_T("START_X")));
															stInfoArea->rectROI.top = _ttoi(xmlPattern->GetAttrib(_T("START_Y")));
															stInfoArea->rectROI.right = _ttoi(xmlPattern->GetAttrib(_T("END_X")));
															stInfoArea->rectROI.bottom = _ttoi(xmlPattern->GetAttrib(_T("END_Y")));
														}
														xmlPattern->ResetMainPos();
													}
												}

												xmlPattern->OutOfElem();
											}
										}
#pragma endregion
										SAFE_DELETE(xmlPattern)
									}
								}
							}
						}

						// KSW 17.10.24 [Round] - ������ �ε� Start						
						{
							// � ���� �ִ� 30��
							for (int i=0; i<MAX_MEM_SIZE_E_INSPECT_AREA ; i++)
							{
								// �ӽ� ���� ���� ���
								CString strTemp;

								// RMS ���� ������ ��� ����
								
								// KSW 17.11.09 [Round] - Apply�� �ƴ� ��츸, ���� �б�
								strTemp.Format(_T("%s\\CornerEdge\\%s_%02d.EdgePT"), strCornerEdgePath, stInfoGrabStep->strGrabStepName, i);

								// ���� ���� ��/�� Ȯ�� �ʿ�
								CFileFind findEdgeFile;
								BOOL bFindEdgeFile = FALSE;

								bFindEdgeFile = findEdgeFile.FindFile( strTemp );
								findEdgeFile.Close();

								// ���� �ִ� ���
								if ( bFindEdgeFile )
								{

									FILE *out = NULL;
									fopen_s(&out, (CStringA)strTemp, ("r"));

									if( out != NULL)
									{

										cv::Point2i ptTemp;
										int j = 0;

										switch(theApp.m_Config.GetEqpType())
										{
										case EQP_AVI:
										case EQP_SVI:

											// ����� �ε��� �б�
											fscanf_s(out, "CornerIndex%d\n", &pGrabStepInfo[nGrabCnt].tRoundSet[i].nCornerMinLength);

											// �������� Cell ���� �ȿ� �����ϴ��� Ȯ��
											fscanf_s(out, "CornerInside%d,%d,%d,%d\n", 
												&pGrabStepInfo[nGrabCnt].tRoundSet[i].nCornerInside[0], 
												&pGrabStepInfo[nGrabCnt].tRoundSet[i].nCornerInside[1], 
												&pGrabStepInfo[nGrabCnt].tRoundSet[i].nCornerInside[2], 
												&pGrabStepInfo[nGrabCnt].tRoundSet[i].nCornerInside[3]);								




											while( true )
											{
												fscanf_s(out, "%d,%d\n", &ptTemp.x, &ptTemp.y);

												// fscanf_s ���� or ��ǥ ���� ��� ������
												if( pGrabStepInfo[nGrabCnt].tRoundSet[i].nContourCount > 0				&& 
													pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j-1].x == ptTemp.x	&&
													pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j-1].y == ptTemp.y	)
													break;

												// ��ǥ �߰�
												// ���ͷ� ����Ϸ� ������, SetGrabStepInfo memcopy �ϴ� �κЋ����� �迭�� ����
												pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j].x = ptTemp.x;
												pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j].y = ptTemp.y;
												pGrabStepInfo[nGrabCnt].tRoundSet[i].nContourCount++;

												j++;
											}
											break;
										case EQP_APP:
											// ����� �ε��� �б�
											fscanf_s(out, "CornerIndex%d\n", &pGrabStepInfo[nGrabCnt].tRoundSet[i].nContourCount);


											while(j != pGrabStepInfo[nGrabCnt].tRoundSet[i].nContourCount)
											{
												fscanf_s(out, "%d,%d\n", &ptTemp.x, &ptTemp.y);
												// ��ǥ �߰�
												// ���ͷ� ����Ϸ� ������, SetGrabStepInfo memcopy �ϴ� �κЋ����� �迭�� ����
												pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j].x = ptTemp.x;
												pGrabStepInfo[nGrabCnt].tRoundSet[i].ptContours[j].y = ptTemp.y;
												j++;
											}
											break;
										}
										fclose(out);
										out = NULL;
									}
								}						
							}
						}
						// KSW 17.10.24 [Round] - ������ �ε� End
						// [CHole] - ������ �ε� Start						
						{
							// � ���� �ִ� 30��
							for (int i = 0; i < MAX_MEM_SIZE_E_INSPECT_AREA; i++)
							{
								// �ӽ� ���� ���� ���
								CString strTemp2;

								// RMS ���� ������ ��� ����

								// KSW 17.11.09 [CameraHole] - Apply�� �ƴ� ��츸, ���� �б�
								strTemp2.Format(_T("%s\\CameraHole\\%s_%02d.EdgePT"), strCornerEdgePath, stInfoGrabStep->strGrabStepName, i);

								// ���� ���� ��/�� Ȯ�� �ʿ�
								CFileFind findEdgeFile2;
								BOOL bFindEdgeFile2 = FALSE;

								bFindEdgeFile2 = findEdgeFile2.FindFile(strTemp2);
								findEdgeFile2.Close();

								// ���� �ִ� ���
								if (bFindEdgeFile2)
								{

									FILE *out = NULL;
									fopen_s(&out, (CStringA)strTemp2, ("r"));

									if (out != NULL)
									{

										cv::Point2i ptTemp;
										int j = 0;

										switch (theApp.m_Config.GetEqpType())
										{
										case EQP_AVI:
										case EQP_SVI:

											// ����� �ε��� �б�
											fscanf_s(out, "CameraHoleIndex%d\n", &pGrabStepInfo[nGrabCnt].tCHoleSet[i].nCornerMinLength);

											// �������� Cell ���� �ȿ� �����ϴ��� Ȯ��
											fscanf_s(out, "CameraHoleInside%d,%d,%d,%d\n",
												&pGrabStepInfo[nGrabCnt].tCHoleSet[i].nCornerInside[0],
												&pGrabStepInfo[nGrabCnt].tCHoleSet[i].nCornerInside[1],
												&pGrabStepInfo[nGrabCnt].tCHoleSet[i].nCornerInside[2],
												&pGrabStepInfo[nGrabCnt].tCHoleSet[i].nCornerInside[3]);

											while (true)
											{
												fscanf_s(out, "%d,%d\n", &ptTemp.x, &ptTemp.y);

												// fscanf_s ���� or ��ǥ ���� ��� ������
												if (pGrabStepInfo[nGrabCnt].tCHoleSet[i].nContourCount > 0 &&
													pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j - 1].x == ptTemp.x	&&
													pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j - 1].y == ptTemp.y)
													break;

												// ��ǥ �߰�
												// ���ͷ� ����Ϸ� ������, SetGrabStepInfo memcopy �ϴ� �κЋ����� �迭�� ����
												pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j].x = ptTemp.x;
												pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j].y = ptTemp.y;
												pGrabStepInfo[nGrabCnt].tCHoleSet[i].nContourCount++;

												j++;
											}
											break;
										case EQP_APP:
											// ����� �ε��� �б�
											fscanf_s(out, "CornerIndex%d\n", &pGrabStepInfo[nGrabCnt].tCHoleSet[i].nContourCount);


											while (j != pGrabStepInfo[nGrabCnt].tCHoleSet[i].nContourCount)
											{
												fscanf_s(out, "%d,%d\n", &ptTemp.x, &ptTemp.y);
												// ��ǥ �߰�
												// ���ͷ� ����Ϸ� ������, SetGrabStepInfo memcopy �ϴ� �κЋ����� �迭�� ����
												pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j].x = ptTemp.x;
												pGrabStepInfo[nGrabCnt].tCHoleSet[i].ptContours[j].y = ptTemp.y;
												j++;
											}
											break;
										}
										fclose(out);
										out = NULL;
									}
								}
							}
						}
						// [CHole] - ������ �ε� End
	// 				}
#pragma endregion
				}
				SAFE_DELETE(xmlList)
			}
		}
		SetGrabStepInfo(pGrabStepInfo);
		SAFE_DELETE_ARR(pGrabStepInfo);
		SetGrabStepCount(nGrabNum);
	}
	catch(...){
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Algorithm Parameter!!!"));
		return false;
	}

	timerReadParameter.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Read algorithm parameter tact time %.2f"), timerReadParameter.GetTime()/1000.);

	return true;
}


//2018.04.13 add by ikm for pad area - S
bool CVSAlgorithmTaskApp::ReadPadInspParameter(TCHAR* strModelPath)
{
	// Delete Pad Area Info	
	DeletePadAreaInfo();

	CCPUTimer timerReadParameter;
	timerReadParameter.Start();

	try{
		
		CString strModelXMLPath;
		// ���� ���
		strModelXMLPath.Format(_T("%s\\PAD_INSP_INFO"), strModelPath);
		
		// Pad Info ���丮 ��		
		CString strFileName, strDirName ;
		CString strPadInspDir[E_PAD_AREA_COUNT]  = {_T("PadArea"), _T("fiducial") , _T("AlignMark"),  _T("PadRoi")	  ,  _T("PadRoi")	 };
		CString strPadInspFile[E_PAD_AREA_COUNT] = {_T("PadArea"), _T("fiducial") , _T("AlignMark"),  _T("PadRoiInsp"),  _T("PadRoiNone")};

		STRU_INFO_PAD** struInfoPad = new STRU_INFO_PAD*[E_PAD_AREA_COUNT];	
		for (int ncnt = 0 ; ncnt < E_PAD_AREA_COUNT; ncnt++) struInfoPad[ncnt] = new STRU_INFO_PAD[MAX_CAMERA_COUNT];
		
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif
		for (int nPadInfoCnt = 0; nPadInfoCnt <  (sizeof(strPadInspDir)/sizeof(*strPadInspDir)); nPadInfoCnt++)
		{			
#ifdef _DEBUG
#else
#pragma omp parallel for
#endif	
			for (int nCamCnt = 0; nCamCnt < MAX_CAMERA_COUNT; nCamCnt++)
			{				
				CString strFileName, strImgName;
				
				CString strPadInspPath;

				strFileName.Format(_T("%s_CAM%02d.xml"), strPadInspFile[nPadInfoCnt], nCamCnt);					

				strPadInspPath.Format(_T("%s\\%s\\%s"),strModelXMLPath, strPadInspDir[nPadInfoCnt], strFileName);
					
				struInfoPad[nPadInfoCnt][nCamCnt]._malloc(0);	

				CFileFind find;

				BOOL bFindFile = FALSE;

				bFindFile = find.FindFile(strPadInspPath); find.Close(); if (!bFindFile){continue;}
								
				CMarkup xmlFile; if(!xmlFile.Load(strPadInspPath)){continue;}
					
				CMarkup* xmlList = new CMarkup(xmlFile);		

				xmlList->FindElem(_T("AreaData"));		
			
				int nMaxCnt = _ttoi(xmlList->GetAttrib(_T("RoiCount")));
				
				xmlList->IntoElem();
								
				struInfoPad[nPadInfoCnt][nCamCnt]._malloc(nMaxCnt);				
 				
  				for (int nRoiCnt = 0 ; nRoiCnt < nMaxCnt; nRoiCnt++)
  				{					
  					if (xmlList->FindElem(_T("ROI_%d"), nRoiCnt + 1))
  					{		
  						STRU_PAD_AREA* tPadArea = (STRU_PAD_AREA*)&struInfoPad[nPadInfoCnt][nCamCnt].tPadInfo[nRoiCnt];
  
  						//Area Coord Load
  						cv::Rect rect = cv::Rect(Point(_ttoi(xmlList->GetAttrib(_T("START_X"))), _ttoi(xmlList->GetAttrib(_T("START_Y")))) 
  												,Point(_ttoi(xmlList->GetAttrib(_T("END_X")))  , _ttoi(xmlList->GetAttrib(_T("END_Y") ))));													
  						tPadArea->cvRect = rect;
						
   						//Image Load
   						int nAreaNo = _ttoi(xmlList->GetAttrib(_T("NO")));
   						strImgName.Format(_T("%s_img_%02d_CAM%02d.bmp"), strPadInspFile[nPadInfoCnt], nAreaNo, nCamCnt );
   						strPadInspPath.Format(_T("%s\\%s\\%s"), strModelXMLPath, strPadInspDir[nPadInfoCnt], strImgName);
   						char* pStr = NULL; pStr = CSTR2PCH(strPadInspPath);     						
   						tPadArea->ipImg  = cvLoadImage(pStr, IMREAD_UNCHANGED);	
						//Polygon Coord Load
						CString str = xmlList->GetAttrib(_T("POINT_X")) + xmlList->GetAttrib(_T("POINT_Y")) ;

						if (str != "")
						{
							tPadArea->Point_malloc(1);
							tPadArea->cvPoint[0].x = _ttoi(xmlList->GetAttrib(_T("POINT_X")));
							tPadArea->cvPoint[0].y = _ttoi(xmlList->GetAttrib(_T("POINT_Y")));
							xmlList->IntoElem();
						}
						else
						{							
  							int nPntNum = _ttoi(xmlList->GetAttrib(_T("PolygonCount")));
   							tPadArea->Point_malloc(nPntNum); 
  							xmlList->IntoElem();	
  						
  							for (int nPntCnt = 0 ; nPntCnt < nPntNum; nPntCnt++)
  							{
  								if (xmlList->FindElem(_T("POINT_%d"), nPntCnt + 1))
  								{
  									tPadArea->cvPoint[nPntCnt].x = _ttoi(xmlList->GetAttrib(_T("POINT_X")));		
  									tPadArea->cvPoint[nPntCnt].y = _ttoi(xmlList->GetAttrib(_T("POINT_Y")));		
  								}
  							}
						}
  						xmlList->ResetMainPos();						
  					}  					
  					xmlList->OutOfElem();					
  				}
				SAFE_DELETE(xmlList);
			}
		}
		SetPadAreaInfo(struInfoPad);		

	}
	catch(Exception ex){
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Pad Inspect Parameter!!! %s"), ex.msg);
		return false;
	}

	timerReadParameter.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Read Pad Inspect Parameter tact time %.2f"), timerReadParameter.GetTime()/1000.);

	return true;
}
//2018.04.13 add by ikm for pad area - E

BOOL CVSAlgorithmTaskApp::m_fnInitFunc()
{
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("---------------Initialize Start!!-----------------"));

	if( !(InspPanel.InitVision()) )		// InspCam ������ �ʱ�ȭ (inspThrd ����)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("[Initialize] Fail (1)!!"));
		return FALSE;
	}

	// ���� �ҷ� �ݺ� Count �о����
	m_fnReadRepeatDefectInfo();

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("[Initialize] Init Complete!! Thread Create!!"));

	// ������ ����̺� ���� - ���� ��� ����
	//SearchDriveDisk();

	return TRUE;
}

// �α� ��� �߰�
void CVSAlgorithmTaskApp::WriteLog(const ENUM_KIND_OF_LOG eLogID, const ENUM_LOG_LEVEL eLogLevel, const BOOL bSendGUITask, const BOOL bTraceList, TCHAR* pszLog, ...)
{
	// �α� ���� 
	if (eLogLevel < ALG_LOG_LEVEL)
		return;

	//�α� ����� ���� �ӽ� ���ۿ� �Ű������� ������ �����Ͽ� �α� ������ ����Ѵ�.
	va_list vaList;
	va_start(vaList, pszLog);
	TCHAR* cBuffer = NULL;
	TCHAR* cBufferTemp = NULL;		// for UI
	int len = 0;
	int nBufSize = 0;

	if (pszLog != NULL)
	{
		len = _vscwprintf( pszLog, vaList ) + 1;
		nBufSize = len * sizeof(TCHAR);

		cBuffer = new TCHAR[nBufSize];		
		memset(cBuffer,0,nBufSize);

		if (cBuffer)
			vswprintf(cBuffer, pszLog, (va_list)vaList);
	}
	va_end(vaList);

	// GUI Task �� �α� �ۺ�
	if (bSendGUITask)
		if (GetIPCState())
			m_AlgorithmTask->VS_Send_Log_To_UI(cBuffer, nBufSize);

	// Alg Task UI �� Trace �α� ǥ��
	if (bTraceList)
	{
		if (m_pDlg != NULL)
		{
			cBufferTemp	= new TCHAR[nBufSize];
			memset(cBufferTemp,0,nBufSize);
			memcpy(cBufferTemp, cBuffer, nBufSize);
			::PostMessage(m_pDlg->m_hWnd, WM_PRINT_UI_LOG_MSG_UNICODE, nBufSize, (LPARAM)cBufferTemp);	// Print Log GUI
		}
	}

	switch(eLogID)
	{
	case eLOGCAM0:	case eLOGCAM1:	case eLOGCAM2:	case eLOGCAM3:
		m_pLogWriterCam[eLogID]->m_fnWriteLog(cBuffer);
		break;
	case eLOGPROC:
		m_pLogWriterProc->m_fnWriteLog(cBuffer);
		break;
	case eLOGTACT:
		m_pLogWriterTact->m_fnWriteLog(cBuffer);
		break;
	case eLOGCOMM:
		m_pLogWriterComm->m_fnWriteLog(cBuffer);
		break;
	default:
		break;
	}
	SAFE_DELETE(cBuffer);
}


CString CVSAlgorithmTaskApp::GetCurStepFileName(TCHAR* strFindFolder, TCHAR* strFindFileName)
{
	CFileFind finder;

	CString strWildcard = _T("");
	CString strRet = _T("");

	strWildcard.Format(_T("%s\\*%s.*"), strFindFolder ,strFindFileName);

	BOOL bWorking = finder.FindFile(strWildcard);

	if (bWorking)
	{
		finder.FindNextFile();

		if (finder.IsDots())
			return _T("");

		strRet = finder.GetFilePath();
	}
	else
	{
		strRet = strWildcard;
	}
	finder.Close();

	return strRet;
}

/// ��ȯ�� : ���� ������ Ratio ������ ����Ǿ�� �ϴ� �� (-2 ~ +2)
int CVSAlgorithmTaskApp::CheckImageRatio(UINT nCurRatio, int nDstWidth, int nDstHeight, int nSrcWidth, int nSrcHeight)
{
	int nRet = 0;

	for (int nSeqMode = 2; nSeqMode >= 0; nSeqMode--)	// 0 : Non / 1 : 4-Shot / 2 : 9-Shot
	{
		if (nDstWidth >= nSrcWidth * (nSeqMode + 1) && nDstHeight >= nSrcHeight * (nSeqMode + 1))	// ���� Ȯ��
		{
			// ���� ������ �������� �̹��� ������ ������ ������ �ִ� �̹��� ������ ������ �ٸ� ���
			if (nCurRatio != (nSeqMode + 1))
			{
				nRet = (nSeqMode + 1) - nCurRatio;				
			}
			break;
		}
	}
	return nRet;
}

UINT CVSAlgorithmTaskApp::ThreadDiskCheck(LPVOID pParam)
{
	CVSAlgorithmTaskApp* pThis = (CVSAlgorithmTaskApp*)pParam;	

	CString strDrive, strTemp;	
	CString strUseDrive;
	int		nSleepCnt = 0;
	const int nSleepInterval = 50;

	do
	{
		// Task ���� �� �ﰢ �����ϱ� ����..
		if (nSleepCnt * nSleepInterval >= DRIVE_CHECK_INTERVAL || nSleepCnt == 0)
		{
			nSleepCnt = 0;
			pThis->m_pDiskInfo->RenewalDiskInfo();
			strDrive = theApp.m_Config.GetINIDrive();
			strTemp = strDrive.Left(1) + _T(":\\");
			strUseDrive = pThis->m_pDiskInfo->GetAvailableDrivePath( strTemp );	
			if(strUseDrive != strTemp)
			{
				strTemp = strUseDrive.Left(1) + _T("_Drive") + _T("\\");
				theApp.m_Config.OpenFile(INIT_FILE_PATH);
				theApp.m_Config.Write(_T("Diskinformation"), _T("Last Used Drive"), strTemp);
				theApp.m_Config.SetINIDrive(strTemp);
			}
		}

		Sleep(10);
		nSleepCnt++;		
	}	while (pThis->m_bExecDisk);

	SetEvent(pThis->m_hEventDiskThreadAlive);
	printf("Disk Thread End \n");

	return 0;
}
void CVSAlgorithmTaskApp::CheckDrive()
{
	CString strDrive, strTemp;	
	CString strUseDrive;

	m_pDiskInfo->RenewalDiskInfo();
	strDrive = theApp.m_Config.GetINIDrive();
	strTemp = strDrive.Left(1) + _T(":\\");
	strUseDrive = m_pDiskInfo->GetAvailableDrivePath( strTemp );	
	if(strUseDrive != strTemp)
	{
		strTemp = strUseDrive.Left(1) + _T("_Drive") + _T("\\");
		//theApp.m_Config.SetCurrentDrive( strTemp );
		theApp.m_Config.OpenFile(INIT_FILE_PATH);
		theApp.m_Config.Write(_T("Diskinformation"), _T("Last Used Drive"), strTemp);
		theApp.m_Config.SetINIDrive(strTemp);
	}
}



bool CVSAlgorithmTaskApp::ReadJudgeParameter(TCHAR* strModelPath)
{
	//////////////////////////////////// ���� ������(Ÿ��) ���� �ε�
	CString strMsg = _T("");
	CFileFind find;
	BOOL bFindFile = FALSE;
	CString strDefItemListXMLPath;
	// ���� �ַ�� ���� ����̺� �Ǵ� �߰�
	strDefItemListXMLPath.Format(_T("%s:\\CRUX\\DATA\\TXT\\DEFITEM_LIST.xml"), theApp.m_Config.GETDRV());
	//strDefItemListXMLPath.Format(_T("%s\\ReportFiltering.rule"), strModelPath);
	bFindFile = find.FindFile(strDefItemListXMLPath);
	find.Close();

	if (!bFindFile)
	{
		strMsg.Format(_T("Not found defect item list xml file. (%s)"), strDefItemListXMLPath);		
		AfxMessageBox(strMsg);		
		return false;
	}

	// XML ���� �ε�
	CMarkup xmlDefectItem;
	if(!xmlDefectItem.Load(strDefItemListXMLPath))
	{
		strMsg.Format(_T("Model xml load fail. (%s)"), strDefItemListXMLPath);
		AfxMessageBox(strMsg);
		return false;
	}

	xmlDefectItem.FindElem();		// DEF_ITEM
	xmlDefectItem.IntoElem();		// inside DEF_ITEM

	CString strDefSysName = _T(""), strDefCode = _T("");
	CMarkup* xmlDefItemList = new CMarkup(xmlDefectItem);
	stDefClassification* stDefClass = new stDefClassification[MAX_MEM_SIZE_E_DEFECT_NAME_COUNT];

	for (int nDefItemIndex=0; nDefItemIndex<MAX_MEM_SIZE_E_DEFECT_NAME_COUNT; nDefItemIndex++)
	{		
		if (xmlDefItemList->FindElem(_T("DefType_%d"), nDefItemIndex))
		{
			strDefSysName = xmlDefItemList->GetAttrib(_T("SysName"));
			//memcpy(stDefClass[nDefItemIndex].strDefectName, strDefSysName.GetBuffer(0), sizeof(stDefClass[nDefItemIndex].strDefectName) - sizeof(TCHAR));
			//_tcscat(stDefClass[nDefItemIndex].strDefectName, _T("\0"));
			COPY_CSTR2TCH(stDefClass[nDefItemIndex].strDefectName,strDefSysName,sizeof(stDefClass[nDefItemIndex].strDefectName));
			strDefCode = xmlDefItemList->GetAttrib(_T("DefCode"));
// 			memcpy(stDefClass[nDefItemIndex].strDefectCode, strDefCode.GetBuffer(0), sizeof(stDefClass[nDefItemIndex].strDefectCode) - sizeof(TCHAR));
// 			_tcscat(stDefClass[nDefItemIndex].strDefectCode, _T("\0"));
			COPY_CSTR2TCH(stDefClass[nDefItemIndex].strDefectCode,strDefCode,sizeof(stDefClass[nDefItemIndex].strDefectCode));
			xmlDefItemList->ResetMainPos();
		}		
	}
	SetDefectClassify(stDefClass);
	SAFE_DELETE_ARR(stDefClass);
	SAFE_DELETE(xmlDefItemList);

	///////////////////////////////////////////// �г� ���� ���� Load
	CString strModelPanelJudgePath = _T("");
	// RMS ���� ������ ��� ����
	//strModelPanelJudgePath.Format(_T("%s\\%s\\PanelJudge.rule"), MODEL_FILE_PATH, strModelID);
	strModelPanelJudgePath.Format(_T("%s\\PanelJudge.rule"), strModelPath);

	BOOL bRet = FALSE;
	CString strLine = _T(""), strTemp = _T(""), strItem = _T("");
	CStdioFile fileReader;

	std::vector<stPanelJudgePriority> vPanelJudgeTemp;
	vPanelJudgeTemp.clear();

	try{
		bRet = fileReader.Open(strModelPanelJudgePath, CFile::modeRead | CFile::typeText);
		if(bRet)
		{
			int nCount = 0;
			fileReader.ReadString(strLine);		// Header ���� ����
			while(fileReader.ReadString(strLine))
			{
				stPanelJudgePriority stPanelJudge;
				memset(&stPanelJudge, 0, sizeof(stPanelJudgePriority));

				AfxExtractSubString(strTemp, strLine, 0, ',');
				COPY_CSTR2TCH(stPanelJudge.strGrade, strTemp, sizeof(stPanelJudge.strGrade));

				for (nCount = 1; nCount <= E_PANEL_DEFECT_TREND_COUNT; nCount++)
				{				
					int nIndex = 0;
					AfxExtractSubString(strTemp, strLine, nCount, ',');

					AfxExtractSubString(strItem, strTemp,nIndex++ , '/');
					m_fnGetJudgeInfo(stPanelJudge.stJudgeInfo, strItem, nCount);
					AfxExtractSubString(strItem, strTemp,nIndex++ , '/');
					m_fnGetJudgeInfo(stPanelJudge.stFilterInfo, strItem, nCount);
					
				}

				for (nCount = E_PANEL_DEFECT_TREND_COUNT + 1; nCount <= E_PANEL_DEFECT_TREND_COUNT; nCount++)
				{
					AfxExtractSubString(strTemp, strLine, nCount, ',');
					if(strTemp != _T(""))
						m_fnGetJudgeInfo(stPanelJudge.stJudgeInfo, strTemp, nCount);
					else
						m_fnGetJudgeInfo(stPanelJudge.stJudgeInfo, _T("0+"), nCount);
				}

				vPanelJudgeTemp.push_back(stPanelJudge);
			}			
			fileReader.Close();
		}
	}
	catch(...){
		bRet = FALSE;
	}
	theApp.SetPanelJudgeInfo(vPanelJudgeTemp);

	if (!bRet)
	{
		// Log ���
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Judge Parameter File!!!"));
		if (!fileReader)
			fileReader.Close();
		return false;
	}

	return true;
}


bool CVSAlgorithmTaskApp::ReadUserDefinedFilter(TCHAR* strModelPath)
{
	CString strModelUserFilterPath = _T("");
//	strModelReportFilterPath.Format(_T("%s\\%s\\ReportFiltering.rule"), MODEL_FILE_PATH, strModelID);
	strModelUserFilterPath.Format(_T("%s\\UserDefinedFilter.rule"), strModelPath);
	BOOL bRet = FALSE;
	CString strLine = _T(""), strTemp = _T(""), strItem = _T("");
	CStdioFile fileReader;

	std::vector<stUserDefinedFilter> vUserDefinedFilter;
	vUserDefinedFilter.clear();

	try{
		bRet = fileReader.Open(strModelUserFilterPath, CFile::modeRead | CFile::typeText);
		if(bRet)
		{
			
			fileReader.ReadString(strLine);
			while(fileReader.ReadString(strLine))
			{
				int nCount = 0;
				stUserDefinedFilter stUserFilter;
				memset(&stUserFilter, 0, sizeof(stUserDefinedFilter));
				stUserFilter.bUse = TRUE;
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				_tcscpy(stUserFilter.strGrade,strTemp);
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				stUserFilter.nFilterItemsCount = _ttoi(strTemp);
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				AfxExtractSubString(strItem, strTemp, 0, '+');
				for(int i = 0 ; i < stUserFilter.nFilterItemsCount ; i++)
				{
					AfxExtractSubString(strItem, strTemp, i, '+');
					stUserFilter.nFilterItems[i] = _ttoi(strItem);
				}
				AfxExtractSubString(strTemp, strLine, nCount++, ',');
				m_fnGetJudgeInfo(&stUserFilter.stFilterInfo, strTemp, 1);

				vUserDefinedFilter.push_back(stUserFilter);
			}			
			fileReader.Close();
		}
	}
	catch(...){
		bRet = FALSE;
	}
	theApp.SetUserDefinedFilter(vUserDefinedFilter);
	std::vector<stUserDefinedFilter> dd;
	dd =theApp.GetUserDefinedFilter();
	if (!bRet)
	{
		// Log ���
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Judge Parameter File!!!"));
		if (!fileReader)
			fileReader.Close();
		return false;
	}	
	return true;
}

bool CVSAlgorithmTaskApp::ReadDefectClassify(TCHAR* strModelPath)
{
	///////////////////////////////////////////// ���� ���� ���͸� ���� Load
	CString strDefectClassifyFilePath = _T("");
	//strDefectRankFilePath.Format(_T("%s\\%s\\DefectRank.rule"), MODEL_FILE_PATH, strModelID);
	strDefectClassifyFilePath.Format(_T("%s\\DefectClassify.rule"), strModelPath);
	BOOL bRet = FALSE;
	CString strLine = _T(""), strTemp = _T("");
	CStdioFile fileReader;
	int DefectRank[E_DEFECT_JUDGEMENT_COUNT] = {0,};
	int DefectGroup[E_DEFECT_JUDGEMENT_COUNT] = {0,};

	try{
		bRet = fileReader.Open(strDefectClassifyFilePath, CFile::modeRead | CFile::typeText);
		if(bRet)
		{
			int nCount = 0;
			fileReader.ReadString(strLine);		// Header ���� ����

			while(fileReader.ReadString(strLine))
			{
				AfxExtractSubString(strTemp, strLine, 0, ',');

				if (strTemp.CompareNoCase(_T("Rank")) == 0)
				{
					// Defect Rank ����
					for (nCount = 0; nCount < E_DEFECT_JUDGEMENT_COUNT; nCount++)
					{				
						AfxExtractSubString(strTemp, strLine, nCount + 1, ',');
						DefectRank[nCount] = _ttoi(strTemp);
					}
				}
				else if (strTemp.CompareNoCase(_T("Group")) == 0)
				{
					// Defect Group ����					
					for (nCount = 0; nCount < E_DEFECT_JUDGEMENT_COUNT; nCount++)
					{				
						AfxExtractSubString(strTemp, strLine, nCount + 1, ',');
						DefectGroup[nCount] = _ttoi(strTemp);
					}
				}
			}

			fileReader.Close();
		}
	}
	catch(...){
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Defect Classify Rule !!! Set Defect Rank / Group Default"));
		for(int i = 0 ; i < E_DEFECT_JUDGEMENT_COUNT ; i++)
			DefectRank[i] = 1;
	}
	theApp.SetDefectRank(DefectRank);
	theApp.SetDefectGroup(DefectGroup);

	if (!bRet)
	{
		if (!fileReader)
			fileReader.Close();
		return false;
	}	
	
	return true;

}
void CVSAlgorithmTaskApp::m_fnGetJudgeInfo(stPanelJudgeInfo* pStJudgeInfo, CString strVal, int nCount)
{
	if (strVal.GetLength() == 0)		strVal = _T("0-");		// Default

	// GUI ���� ��� Algorithm Parameter �� Format ����
	if		(strVal.Right(1).CompareNoCase(_T("=")) == 0)		pStJudgeInfo[nCount-1].nSign = 0;	// ����
	else if (strVal.Right(1).CompareNoCase(_T("!")) == 0)		pStJudgeInfo[nCount-1].nSign = 1;
	//else if (strVal.Right(1).CompareNoCase(_T(">")) == 0)		pStJudgeInfo[nCount-1].nSign = 2;	// ����
	//else if (strVal.Right(1).CompareNoCase(_T("<")) == 0)		pStJudgeInfo[nCount-1].nSign = 3;	// ����
	else if (strVal.Right(1).CompareNoCase(_T("+")) == 0)		pStJudgeInfo[nCount-1].nSign = 4;
	else if (strVal.Right(1).CompareNoCase(_T("-")) == 0)		pStJudgeInfo[nCount-1].nSign = 5;
	else
	{
		strVal += _T("-");
		pStJudgeInfo[nCount-1].nSign = 5;
	}
	pStJudgeInfo[nCount-1].nRefVal = _ttoi(strVal.Left(strVal.GetLength() - 1));
}

// 2018.10.01 MDJ File Read
void CVSAlgorithmTaskApp::ReadRepeatDefectInfo()
{
	m_fnReadRepeatDefectInfo();
}

bool CVSAlgorithmTaskApp::m_fnReadRepeatDefectInfo()
{
	m_listRepeatDefInfo[ePIXEL].clear();
	m_listRepeatDefInfo[eWORK].clear();

	m_fnReadRepeatFile(REPEAT_DEFECT_PIXEL_INFO_PATH, &m_listRepeatDefInfo[ePIXEL]);
	m_fnReadRepeatFile(REPEAT_DEFECT_WORK_INFO_PATH, &m_listRepeatDefInfo[eWORK]);

	return true;

}

bool CVSAlgorithmTaskApp::WriteResultFile(CString strPanelID, CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult)
{
	//��� Check
	DWORD result;
	if (((result=GetFileAttributes(strFilePath)) == -1 ) || !(result&FILE_ATTRIBUTE_DIRECTORY) ) {
		CreateDirectory(strFilePath, NULL);
	}

	CString strResultData;
	SYSTEMTIME	time;
	GetLocalTime(&time);

	CString strDate;
	CString strTime;
	strTime.Format(_T("%02u:%02u:%02u"), time.wHour, time.wMinute, time.wSecond);
	strDate.Format(_T("%04u%02u%02u_"), time.wYear, time.wMonth, time.wDay);

	strDate.Append(strFileName);

	CString strFullPath;
	strFullPath.Format(_T("%s\\%s"), strFilePath, strDate);

	CStdioFile stdFile;

	char cSavePath[256];
	memset(cSavePath, NULL, 256);
	WideCharToMultiByte(CP_ACP, NULL, strFullPath, -1, cSavePath, 256, NULL, NULL);

	int nFileCheck = access(cSavePath, 0);

	if(!stdFile.Open(strFullPath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate | CFile::typeText | CFile::shareDenyWrite, NULL))
	{
		return false;
	}

	stdFile.SeekToEnd(); //���� �������� ������ �̵�

	strResultData.Format(_T("\n%s,%s"), strTime, strPanelID);

	CString strValue;
	strValue.Format(_T(",%s"), strResult);
	strResultData.Append(strValue);

	CString strStandardColumn;
	strStandardColumn.Format(_T("TIME,"));
	strStandardColumn.Append(strColumn);

	if(nFileCheck == 0)			//������ ���� ���
	{
		stdFile.WriteString(strResultData);
	}
	else						//������ ���� ���
		stdFile.WriteString(strStandardColumn);

	stdFile.Close();

	return true;
}

BOOL CVSAlgorithmTaskApp::m_fnReadRepeatFile(CString strFilePath, std::list<RepeatDefectInfo>* pList)
{
	CString strLine = _T(""), strTemp = _T("");
	CStdioFile fileReader;
	RepeatDefectInfo stRepeatDefInfo;
	BOOL bRet = FALSE;

	try{
		bRet = fileReader.Open(strFilePath, CFile::modeRead | CFile::typeText);
		if(bRet)
		{
			int nCount = 0;
			fileReader.ReadString(strLine);		// Header ���� ����

			while(fileReader.ReadString(strLine))
			{
				/// Defect Code
				AfxExtractSubString(strTemp, strLine, 0, ',');
				stRepeatDefInfo.eDefType = (ENUM_DEFECT_JUDGEMENT)(_ttoi(strTemp));

				/// CCD �ҷ� ����
				AfxExtractSubString(strTemp, strLine, 1, ',');
				stRepeatDefInfo.ptCenterPos.x = _ttoi(strTemp);

				AfxExtractSubString(strTemp, strLine, 2, ',');
				stRepeatDefInfo.ptCenterPos.y = _ttoi(strTemp);

				AfxExtractSubString(strTemp, strLine, 3, ',');
				stRepeatDefInfo.nRepeatCount = _ttoi(strTemp);

				pList->push_back(stRepeatDefInfo);
			}

			fileReader.Close();
		}
	}
	catch(...){
		bRet = FALSE;
		pList->clear();
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, FALSE, _T("Failed Read Repeat Defect Info !!!"));
	}

	if (!bRet)		// ������ ���ų� ������ ���� ���
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, FALSE, _T("Not Found Repeat Defect Info !!! (%s)"), strFilePath);
		if (!fileReader)
			fileReader.Close();
		return false;
	}
	return bRet;
}

BOOL CVSAlgorithmTaskApp::GetMergeToolUse()		
{	
	m_nInspStep		= GetPrivateProfileInt(_T("MergeTool"),_T("USE_MERGE_TOOL"),1,INIT_FILE_PATH)? true : false; 
	return m_nInspStep; 
};