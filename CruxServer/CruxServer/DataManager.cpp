#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "StdAfx.h"
#include "DataManager.h"
#include "MainFrm.h"
#include "ViewSetup.h"
#include "ViewAuto.h"

enum
{
	H_ITEM = 1,
	H_HEADERITEM,
	H_ROW_CNT,
	H_ITEM_END
};

enum 
{
	P_PANELITEM = 0,
	P_LAYER_NO, 
	P_PANEL_ID,
	P_PANEL_COUNT,
	P_JUDGEMENT,
	P_JUDGEMENT_CODE1,
	P_JUDGEMENT_CODE2,
	RECIPE_ID,
	PART_NAME,
	P_POLMARK_ILEGAL,
	P_ITEM_END
};

enum
{
	D_ITEM = 1,
	D_DEFITEM,
	D_LAYER_NO,
	D_DEFECT_NO,
	D_DEF_CODE,
	D_FLAG,
	D_PIXEL_X,
	D_PIXEL_Y,
	D_G_START_NO,
	D_D_START_NO,
	D_G_END_NO,
	D_D_END_NO,
	D_COORD_STARTR_X,
	D_COORD_START_Y,
	D_COORD_END_X,
	D_COORD_END_Y,
	D_DEF_SIZE,
	D_DEF_IMG_NO,
	D_IMG_SIZE_X,
	D_IMG_SIZE_Y,
	D_DEF_TYPE,
	D_CAM_NO,
	D_ITEM_END
};

//2016.02.01 Packing Defect
enum
{
	D_PACKING_CLASSIFY = 22,
	D_PACKING_AREA,
	D_PACKING_BOX_AREA,
	D_PACKING_BOX_X_MAX,
	D_PACKING_BOX_X_MIN,
	D_PACKING_BOX_Y_MAX,
	D_PACKING_BOX_Y_MIN,
	D_PACKING_CENTER_X,
	D_PACKING_CENTER_Y,
	D_PACKING_LENGTH,
	D_PACKING_BREADTH,
	D_PACKING_ELONGATION,
	D_PACKING_COMPACTNESS,
	D_PACKING_HOLECOUNT,
	D_PACKING_FERET_MIN_ANGLE,
	D_PACKING_FERET_MAX_ANGLE,
	D_PACKING_FERET_MIN,
	D_PACKING_FERET_MAX,
	D_PACKING_FERET_MEAN,
	D_PACKING_FERET_X,
	D_PACKING_FERET_Y,
	D_PACKING_FERET_ELONGATION,
	D_PACKING_PIXEL_MAX,
	D_PACKING_PIXEL_MEAN,
	D_PACKING_PIXEL_MIN,
	D_PACKING_PIXEL_SIGMA,
	D_PACKING_PIXEL_SUM,
	D_PACKING_ITEM_END
};

enum
{	CF =0,	TFT,	POL };

enum
{	D_TYPE_1 =0,	D_TYPE_2 };

enum
{	VS_1 =0,	VS_2,	VS_3 };

#define LEN_ROW_CNT					7
#define LEN_DEFECT_NO				1//10
#define INDEX_ITEM_NAME				1

CDataManager::CDataManager(void)
{
	m_nDefectCount = 0;
	m_hThreadDelInspData = NULL;

	//	m_fnLoadSummary();
	m_bInspEnd = FALSE;
	m_strPanelId = _T("");
	m_nDaySummaryBackup = 0;
	m_strDefGrade1 = _T("");
	m_strdefJudge1 = _T("");
	m_strDefGrade2 = _T("");
	m_strdefJudge2 = _T("");
	m_strReJudgeImgPath	= _T("");		//2019.01.18 add by rjb
	m_nSelectSummary    = 0;	// 2015.01.12 ljk
	m_strDefImgSavePath = _T("");
	FtpImgClient = NULL;
	FtpDstClient = NULL;

	m_nCfNgCnt		= 0;
	m_nTftNgCnt		= 0;
	m_nPenetNgCnt	= 0;
	m_nQmsFinalCnt	= 0;
	m_cCountRoot = _T("");
	m_cCountRoot.Append(ROOT_PATH);
	m_cCountRoot.Append(CONFIG_COUNT_PATH);
	m_strQmsImgName = _T("");
	//	memset(m_nPcClassifyEndFlag, 0 , sizeof(m_nPcClassifyEndFlag));	
	//pTreadCimProc = NULL;
	//pTreadCimProc = (CThreadProcCIM*)AfxBeginThread(RUNTIME_CLASS(CThreadProcCIM), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	pTreadProcData = (CThreadProcData*)AfxBeginThread(RUNTIME_CLASS(CThreadProcData), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	//pTreadCimProc->ResumeThread();
	pTreadProcData->ResumeThread();
	m_bUseSemaphore = FALSE;
	m_bUseAreaCheck = FALSE;	//2020.03.03 add by mby
	pTreadProcData->m_hThreadDelDir = NULL;
	ncnt = 0;
	m_strCimMeasureDataTemp = _T("");	//2016.01.15
	m_strQmsSendFileDate = _T("");
	m_str_rankCIMvalue = _T("");   //2019.01.28 ryu

	for (int i =0 ; i < MAX_REV_PANEL * 2 ;i ++)
	{
		m_stAlignMeasureInfo[i].init_info();
	}

	m_strPolPositionEQ_CV[3] = _T("");  //2019.06.20  by ryu
	m_strPolPositionEQ_CV2 = _T("");
	m_strPolPositionStage_CV[3] = _T("");
	m_strPolPositionStage_CV2 = _T("");
	m_fnReadPanelCodeData(&m_stPanelCodeTest);		// 2016.09.14	Mr.Chu	
	strRecipeNo = _T("");
	m_stMeasureLimitInfo.init_info();		//2019.02.18 add by mby
	//2019.12.16 add by mby
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		m_nArrayDccCount[i] = 0;
		m_nDccTotalPanel[i] = 0;
		m_nDccFlag[i]		= 0;
		m_nArrayPaCntPDCount[i] = 0;	//2020.08.05 add by mby
		m_bPaOverlapAlarmFlag[i] = 0;	//2021.01.13 add by mby for Edge,Corner Broken
	}
	for (int i = 0; i < DCC_COUNT; i++)
	{
		m_nAryDccChkSizeXData	[i] = 0;
		m_nAryDccChkSizeYData	[i] = 0;
	}
	for (int i = 0; i < DCC_SIZE_CHECK; i++)
	{
		m_strAryDccSizeXData[i] = _T("");
		m_strAryDccSizeYData[i] = _T("");
	}
	//2020.01.15 add by mby	
	for (int i = 0; i < 35; i++)
	{
		m_bPaCodeFlag[i] = FALSE;
		m_bPaGradeFlag[i] = FALSE;
	}
	//2020.01.15 add by mby	
	//2020.04.10 add by mby
	for (int i = 0; i < DCC_DIRECT_COUNT; i++)
	{
		m_nDccDirectFlag[i] = 0;		
	}
	//2020.08.26 add by mby
	for (int i = 0; i < 16; i++)
	{
		m_nArrayPaCntEdgeCount[i] = 0;		
	}
	
	m_bDcc_Alarm_flag = FALSE;		//2019.12.24 add by mby
	m_strDccAlarmMsg  = _T("");		 //2019.12.24 add by mby
	m_bPaCheckFlag		= FALSE;		//2020.04.23 add by mby
	m_bDcc_Check1_flag	= FALSE;		 //2020.04.10 add by mby
	m_bDcc_Check2_flag	= FALSE;		 //2020.04.10 add by mby

	m_nEdgeConnectFlag  = 0;			//2021.02.04 add by mby
	m_bMea_Fail_flag	= 0;			//2021.04.22 add by mby
	//2020.07.24 add by mby S-
	for(int i = 0 ; i < 16 ; i++)
	{
		m_nAryGradeRankChkA_X[i] = FALSE;
		m_nAryGradeRankChkA_Y[i] = FALSE;
		m_nAryGradeRankChkB_X [i] = FALSE;
		m_nAryGradeRankChkB_Y[i] = FALSE;
		m_nAryGradeRankChkB2_X [i] = FALSE;
		m_nAryGradeRankChkB2_Y[i] = FALSE;
		m_nAryGradeRankChkC_X [i] = FALSE;
		m_nAryGradeRankChkC_Y[i] = FALSE;

		m_strAryGradeRankA_X[i] = _T("");
		m_strAryGradeRankA_Y[i] = _T("");
		m_strAryGradeRankC_X[i] = _T("");
		m_strAryGradeRankC_Y[i] = _T("");

		m_strAryGradeB_Rank[i] = _T("");
		m_strAryGradeB2_Rank[i] = _T("");
	}
	for(int i = 0 ; i < 32 ; i++)
	{
		m_strAryGradeRankB_X[i] = _T("");
		m_strAryGradeRankB_Y[i] = _T("");
		m_strAryGradeRankB2_X[i] = _T("");
		m_strAryGradeRankB2_Y[i] = _T("");
	}
	//2020.07.24 add by mby E-
}

CDataManager::~CDataManager(void)
{
	m_fnClearSummary();		// Defect File List의 데이터 초기화		// 2015.01.12 ljk
	m_fnClearDefectList();	// Defect Summary List의 데이터 초기화 // 2015.01.12 ljk
	m_fnClearReviewDefectList();	//2015.06.22

	/*TerminateThread(pTreadCimProc->m_hThreadCim, 0);
	if(NULL != pTreadCimProc)
	{
	::WaitForSingleObject(pTreadCimProc->m_hThreadCim,INFINITE);
	delete (CWinThread *)pTreadCimProc;
	pTreadCimProc = NULL;
	}*/

	//if(NULL != pTreadProcData)
	//{
	//	WaitForSingleObject(pTreadProcData,INFINITE);
	/*CloseHandle(pTreadProcData->m_hThreadData);*/
	//CloseHandle(pTreadProcData->m_hThreadData);
	//CloseHandle(pTreadProcData->m_hThreadDelDir);

	//delete (CWinThread *)pTreadProcData;
	//pTreadProcData->m_hThreadDelDir = NULL;
	//pTreadProcData = NULL;
	//}
	/*if (FtpDstClient != NULL)
	{
	::WaitForSingleObject(FtpDstClient,INFINITE);
	delete FtpDstClient;
	FtpDstClient = NULL;

	}		
	if (FtpImgClient != NULL)
	{
	::WaitForSingleObject(FtpImgClient,INFINITE);
	delete FtpImgClient;
	FtpImgClient = NULL;

	}*/

	/*if(NULL != m_hThreadDelImg)
	{
	::WaitForSingleObject(FtpImgClient,INFINITE);
	CloseHandle(m_hThreadDelImg);
	m_hThreadDelImg = NULL;	
	}*/

}

BOOL CDataManager::m_fnCreateEmptyFile(CString strPathName)
{
	G_AddLog(3, 3, _T("m_fnCreateEmptyFile() - Start"));
	CStdioFile cstFile;

	if (!cstFile.Open(strPathName, CFile::modeWrite | CFile::modeCreate|CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("Empty File Open Error %s : %d"), strPathName, GetLastError());
		return FALSE;
	}

	cstFile.WriteString(_T("ITEM      ,HEADERITEM,ROW_CNT\n"));
	cstFile.WriteString(_T("DATA      ,HEADERDATA,       \n"));
	cstFile.WriteString(_T("ITEM      ,PANELITEM ,PANEL_ID    ,MODEL       ,USER      ,INSPECT_TIME  ,ALL_JUDGE ,JUDGE_01  ,JUDGE_02  ,JUDGE_03  ,GRAB_T_01 ,CLASS_T_01,GRAB_T_02 ,CLASS_T_02,GRAB_T_03 ,CLASS_T_03\n"));
	cstFile.WriteString(_T("DATA      ,PANELDATA ,            ,            ,          ,              ,          ,          ,          ,          ,          ,          ,          ,          ,          ,          \n"));
	cstFile.WriteString(_T("ITEM      ,DEFITEM   ,DEFECT_NO ,COORD_X ,COORD_Y ,SIZE_DX ,SIZE_DY ,SIZE    ,CODE    ,CAM_NO  \n"));

	cstFile.Close();

	G_AddLog(3, 3, _T("m_fnCreateEmptyFile() - End"));
	return TRUE;
}

BOOL CDataManager::m_fnAddDefectItem(CString strPathName, int nDefectNo, int nPosX, int nPosY, int nSizeX, int nSizeY, int nSize, int nCode, int nCamNo)
{
	G_AddLog(3, 3, _T("m_fnAddDefectItem() - Start"));
	CStdioFile cstFile;

	if (!cstFile.Open(strPathName, CFile::modeWrite | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("Defect File Open Error %s : %d")
			, strPathName, GetLastError());
		return FALSE;
	}

	cstFile.SeekToEnd();

	CString strData, strDefectNo, strPosX, strPosY, strSizeX, strSizeY, strSize, strCode, strCamNo;

	strDefectNo.Format(_T("%d"), nDefectNo);
	strPosX.Format(_T("%d"), nPosX);
	strPosY.Format(_T("%d"), nPosY);
	strSizeX.Format(_T("%d"), nSizeX);
	strSizeY.Format(_T("%d"), nSizeY);
	strSize.Format(_T("%d"), nSize);
	strCode.Format(_T("%d"), nCode);
	strCamNo.Format(_T("%d"), nCamNo);

	strDefectNo = m_fnFillSpaceStr(strDefectNo, 10);
	strPosX = m_fnFillSpaceStr(strPosX, 8);
	strPosY = m_fnFillSpaceStr(strPosY, 8);
	strSizeX = m_fnFillSpaceStr(strSizeX, 8);
	strSizeY = m_fnFillSpaceStr(strSizeY, 8);
	strSize = m_fnFillSpaceStr(strSize, 8);
	strCode = m_fnFillSpaceStr(strCode, 8);
	strCamNo = m_fnFillSpaceStr(strCamNo, 8);

	strData = _T("DATA      ,DEFECT    ,");
	strData += strDefectNo + EXT_COLON;
	strData += strPosX + EXT_COLON;
	strData += strPosY + EXT_COLON;
	strData += strSizeX + EXT_COLON;
	strData += strSizeY + EXT_COLON;
	strData += strSize + EXT_COLON;
	strData += strCode + EXT_COLON;
	strData += strCamNo + _T("\n");

	cstFile.WriteString(strData);

	cstFile.Close();

	G_AddLog(3, 3, _T("m_fnAddDefectItem() - End"));
	return TRUE;
}

// "HEADERDATA" 또는 "PANELDATA" 의 특정 항목을 값을 수정한다.
// Param 1 : 파일 경로
// Param 2 : ITEM NAME
// Param 3 : Item Index (1 부터 시작 : "1, 2, 3,...")
// Param 4 : 해당 Item 의 길이
// Param 5 : Item 의 총개수
// Param 6 : 수정할 데이터
BOOL CDataManager::m_fnModifyItemData(CString strPathName, CString strItemName, int nItemNo, int nItemLen, int nIndexEnd, CString strData)
{
	G_AddLog(3, 3, _T("m_fnModifyItemData() - Start"));
	CStdioFile cstFile;
	BOOL bRet = FALSE;

	if (!cstFile.Open(strPathName, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnModifyItemData1 File Open Error %s : %d"), strPathName, GetLastError());
		return FALSE;
	}

	CStringArray strArrayData;
	CString strLine, strItem, strNewLine;

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem == strItemName)
		{
			strNewLine = m_fnSetItemData(nItemNo, strLine, strData, nItemLen, nIndexEnd);

			if(strNewLine != "")
			{
				strArrayData.Add(strNewLine);
				bRet = TRUE;
				continue;
			}
		}

		strArrayData.Add(strLine);
	}

	cstFile.Close();

	if (!cstFile.Open(strPathName, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnModifyItemData2 File Open Error %s : %d"), strPathName, GetLastError());
		return FALSE;
	}

	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i) + _T("\n");
		cstFile.WriteString(strLine);
	}

	cstFile.Close();

	G_AddLog(3, 3, _T("m_fnModifyItemData() - End"));
	return bRet;
}

void CDataManager::m_fnParsingFile(CString strPathName)
{
	G_AddLog(3, 3, _T("m_fnParsingFile() - Start"));
	CStdioFile cstFile;
	int i = 0;
	int nColCount = 0;
	int nSerial = 0;    

	if (!cstFile.Open(strPathName, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("Parsing File Open Error %s : %d"), strPathName, GetLastError());
		return;
	}

	CString strLine = _T("");
	CString strItem, strData, strTemp;

	while(cstFile.ReadString(strLine))
	{
		strTemp = strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem == _T("HEADERDATA"))
		{
			for(nColCount = 1; nColCount < H_ITEM_END - 1; nColCount++)
			{
				strTemp = strTemp.Mid(strTemp.Find(EXT_COLON) + 1);

				if(nColCount == H_ITEM_END - 1)		// 마지막 이면...
					strData = strTemp;
				else	
					strData = strTemp.Left(strTemp.Find(EXT_COLON));

				strData.Trim();

				G_AddLog(3, 2, _T("HEADERDATA : %s"), strData);
			}
		}
		else if(strItem == _T("PANELDATA"))
		{
			for(nColCount = 1; nColCount < P_ITEM_END - 1; nColCount++)
			{
				strTemp = strTemp.Mid(strTemp.Find(EXT_COLON) + 1);

				if(nColCount == P_ITEM_END - 1)		// 마지막 이면...
					strData = strTemp;
				else	
					strData = strTemp.Left(strTemp.Find(EXT_COLON));

				strData.Trim();

				G_AddLog(3, 2, _T("PANELDATA : %s"), strData);
			}
		}
		else if(strItem == _T("DEFECT"))
		{
			for(nColCount = 1; nColCount < D_ITEM_END - 1; nColCount++)
			{
				strTemp = strTemp.Mid(strTemp.Find(EXT_COLON) + 1);

				if(nColCount == D_ITEM_END - 1)		// 마지막 이면...
					strData = strTemp;
				else	
					strData = strTemp.Left(strTemp.Find(EXT_COLON));

				strData.Trim();				

				G_AddLog(3, 2, _T("DEFECT : %s"), strData);
			}

			m_nDefectCount++;
		}		
	}

	G_AddLog(3, 3, _T("m_fnParsingFile() - Start"));
	cstFile.Close();
}

int CDataManager::m_fnDefectFileMerge(CString strPanelId, CString strReadFilePath1, CString strReadFilePath2, CString strNewName, int nVisionCount)
{
	G_AddLog(3, 3, _T("m_fnDefectFileMerge() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	int nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	CString strNewData, strValue, strDefNoTmp;
	CStdioFile cstFile;
	CString strRetVal,strStepId, strImgPanelID, strMesure;

	CString strtemp, strPCNum, strDefNo, resToken, cImgtemp, cFileName;	
	int nImgNameCnt =0 ,nLoopCnt = 0, nDefNo = 0, nEqpModel =0;

	int nDefectCount1 = 0;
	int nDefectCount2 = 0;
	BOOL nMergeFlag = FALSE, bMeasureFlag = FALSE;
	BOOL bWorking = FALSE;
	//CFileFind finder;
	CString strLine = _T("");
	CString strItem, strData, strOriginData;
	//CString strArray[4];
	CString strMeasureVal = _T("");
	m_strArrMerge.RemoveAll();
	m_strArrPanelData.RemoveAll();
	CFileFind finder;
	CString strAlignFailcode = _T("");		//2016.05.11 Mr.Chu
	CString strMeasureFailcodeCF = _T("");
	CString strMeasureFailcodeTFT = _T("");
	CString strFirstSaveDate = _T("");
	CString strFirstSaveTime = _T("");
	CString strOldFilePath = _T(""), strTmpFilePath = _T("");
	CString strOldFileName = _T(""), strTmpFileName = _T("");
	CString strDefType = _T("0"), strDefFlag = _T("0");
	//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능
	CString strPolMarkIlegal;
	CString strPolMarkTemp, strVisionNo, strVisionNoTmp;
	STRU_IMAGE_INFO struImgInfo;
	struImgInfo.init();
	struImgInfo.nQmsImgNoCnt.init();

	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
	stAlignMeasureInfo.init_info();
	CString cCompareVal;

	int nEqpType = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);	//0:AVL 1:ABI

	if (!finder.FindFile(strReadFilePath1) && !finder.FindFile(strReadFilePath2))
	{		
		return DEF_MERGE_NG;
	}

	if (!finder.FindFile(strReadFilePath1))
	{
		CopyFile(strReadFilePath2, strNewName, 0);
		return DEF_MERGE_NO_DAT_FIR;
	}

	if (!cstFile.Open(strReadFilePath1, CFile::modeRead|CFile::typeText, NULL))
	{	
		G_AddLog(3, 2, _T("Merge File1 Open Error %s : %d"), strReadFilePath1, GetLastError());

		return DEF_MERGE_NG;
	}

	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	if(pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS)
		strPolMarkIlegal = POLMARKILEGAL_ABI_OK;
	else
		strPolMarkIlegal = POLMARKILEGAL_OK;

	if (!nEqpModel)	bMeasureFlag = TRUE;
	else
	{
		/*if(nVisionCount != VISION_PC_2)*/ bMeasureFlag = TRUE;
	}

	_tcscpy(struImgInfo.tImgPnlID, strPanelId);
	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		if (strItem == _T("DEFITEM"))
		{
		}
		strOriginData = strData = strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.TrimRight();

		if(strItem == _T("PANELDATA"))
		{			
			if ( m_fnGetItemData(5, strLine) == ALIGN_FAIL)  strAlignFailcode   = ALIGN_FAIL;	//2016.05.11	Mr.Chu

			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE || pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
			{
				stAlignMeasureInfo = m_fnReadAlignMeasureData(strPanelId);
				cCompareVal = stAlignMeasureInfo.cCFMeasureFail; 
				if(cCompareVal == MEASUREFAIL_CF)
					strMeasureFailcodeCF   = MEASUREFAIL_CF;
				cCompareVal = stAlignMeasureInfo.cTFTMeasureFail; 
				if(cCompareVal == MEASUREFAIL_TFT)
					strMeasureFailcodeTFT   = MEASUREFAIL_TFT;
			}

			if ( m_fnGetItemData(7, strLine) == MEASUREFAIL_CF)  strMeasureFailcodeCF   = MEASUREFAIL_CF;
			if ( m_fnGetItemData(7, strLine) == MEASUREFAIL_TFT) strMeasureFailcodeTFT  = MEASUREFAIL_TFT;



			//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 S	
			if((pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS))
			{
				strPolMarkTemp = m_fnGetItemData(9, strLine);

				AfxExtractSubString(strVisionNo, strReadFilePath1, 1, '_');
				strVisionNoTmp.Format(_T("V%d.txt"), nVisionCount);
				//2018.03.14 VISION_PC_2 -> VISION_PC_3, VISION_PC_5 -> VISION_PC_6
				if((nVisionCount == VISION_PC_3 && strVisionNoTmp == strVisionNo)
				|| (nVisionCount == VISION_PC_6 && strVisionNoTmp == strVisionNo))
					m_fnWritePolMarkData(strPanelId, strPolMarkTemp, nVisionCount);
			}
			else 
			{
				//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 S			
				if ( m_fnGetItemData(9, strLine) == POLMARKILEGAL_NG) strPolMarkIlegal  = POLMARKILEGAL_NG;						
			}						
			//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 E
		}

		if(strItem == _T("DEFECT")){
			//nDefectCount1++;
			strValue.Format(_T("%d"), nDefectCount1);

			strNewData = m_fnGetItemData(D_DEF_IMG_NO - 1, strLine);
			strOldFileName = strNewData;
			strtemp = strLine;

			if (strNewData.Compare(_T("")))
			{
				strDefType = m_fnGetItemData(D_DEF_TYPE - 1, strLine);
				struImgInfo.nDefType = _ttoi(strDefType);
				
				//2018.01.22
				strDefFlag = m_fnGetItemData(FLAG + 1, strLine);
				struImgInfo.nDefFlag = _ttoi(strDefFlag);
				
				switch (_ttoi(strDefType))
				{			
				case INSP_POL_BUBBLE: case INSP_POL_SCRATCH: case INSP_POL_ALIEN_SUBSTANCE:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKhCntVal;
					break;
				case INSP_POL_MINOR: case INSP_MURA:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKsCntVal;
					break;
				default:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKvCntVal;
					break;
				}
				m_fnRenameQmsImgFile( struImgInfo, strNewData, nDefectCount1);

				/*cImgtemp.Format(_T("%s"),EXTENSION_JPG);
				strDefNo.Append(cImgtemp);*/
				strOldFilePath.Format(_T("%s%s\\VisionClient-%d\\%s"),  DATA_PATH, RESULT_PATH, nVisionCount, strOldFileName);
				//strOldFilePath =  strReadFilePath1;
				strTmpFileName = struImgInfo.tImgName;
				strNewData = m_fnSetItemData(D_DEF_IMG_NO , strtemp, strTmpFileName, strTmpFileName.GetLength() , D_ITEM_END - 1);


				//if (FileExist(strOldFilePath))
				//{	
				//	strtemp = struImgInfo.tImgPath;	
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	//strPanelID = m_fnGetItemData(3, strOldFilePath);
				//	strTmpFilePath.Format(_T("D:\\QMS_IMG_TMP\\%s"), strtemp);
				//	pFrame->m_fnCreateFolder(strTmpFilePath);
				//	MoveFile(strOldFilePath, strTmpFilePath + strTmpFileName); 
				//	
				//}
			}

			strDefNo.Format(_T("%d"), nDefectCount1);
			strNewData = m_fnSetItemData(D_DEFECT_NO, strNewData, strDefNo, LEN_DEFECT_NO, D_ITEM_END - 1);

			if(strNewData != "")
				m_strArrMerge.Add(strNewData);  
		}
		strMesure = m_fnGetItemData(LAYER_NO - 1, strLine);
		if (bMeasureFlag && !strMesure.Compare(_T("TFT73")) || !strMesure.Compare(_T("TFT74")) || !strMesure.Compare(_T("TFT75")) || !strMesure.Compare(_T("TFT77")) || !strMesure.Compare(_T("TFT78"))
			|| !strMesure.Compare(_T("CF73"))  || !strMesure.Compare(_T("CF74"))  || !strMesure.Compare(_T("CF75")) || !strMesure.Compare(_T("CF77")) || !strMesure.Compare(_T("POL78")) || !strMesure.Compare(_T("POL76"))  )
		{
			strMeasureVal.Append(strLine);
			strMeasureVal += _T("\n");
		}

		if (bMeasureFlag && !strMesure.Compare(_T("73")) || !strMesure.Compare(_T("74")) || !strMesure.Compare(_T("75"))
			|| strMesure == _T("76") || strMesure == _T("77") || strMesure == _T("78"))
		{
			if(nEqpNo/* && strMesure != (_T("75"))*/)
			{
				strMeasureVal.Append(_T("TFT"));
			}
			else if(!nEqpNo && strMesure != (_T("76")) && strMesure != (_T("78")))
			{
				strMeasureVal.Append(_T("CF") );
			}
			if(strMesure == _T("76") || strMesure == _T("78"))
			{
				strMeasureVal.Append(_T("POL"));
			}
			strMeasureVal.Append(strLine);
			strMeasureVal += _T("\n");
		}		
	}
	cstFile.Close();

	if (!finder.FindFile(strReadFilePath2))
	{
		CopyFile(strReadFilePath1, strNewName, 0);
		return DEF_MERGE_NO_DAT_SEC;
	}
	if (!cstFile.Open(strReadFilePath2, CFile::modeRead|CFile::typeText, NULL))
	{
		m_strArrMerge.RemoveAll();
		m_strArrPanelData.RemoveAll();
		G_AddLog(3, 2, _T("Merge File2 Open Error %s : %d"), strReadFilePath2, GetLastError());
		return DEF_MERGE_NG;
	}
	while(cstFile.ReadString(strLine))
	{		
		strOriginData = strData = strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON)+1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.TrimRight();

		if(strItem == _T("DEFECT")){
			nDefectCount2++;
			strValue.Format(_T("%d"), nDefectCount2);

			strNewData = m_fnGetItemData(D_DEF_IMG_NO - 1, strLine);
			strOldFileName = strNewData;
			strtemp = strLine;

			if (strNewData.Compare(_T("")))
			{
				strDefType = m_fnGetItemData(D_DEF_TYPE - 1, strLine);
				struImgInfo.nDefType = _ttoi(strDefType);

				//2018.01.22
				strDefFlag = m_fnGetItemData(FLAG + 1, strLine);
				struImgInfo.nDefFlag = _ttoi(strDefFlag);

				switch (_ttoi(strDefType))
				{			
				case INSP_POL_BUBBLE: case INSP_POL_SCRATCH: case INSP_POL_ALIEN_SUBSTANCE:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKhCntVal;
					break;
				case INSP_POL_MINOR: case INSP_MURA:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKsCntVal;
					break;
				default:
					nDefectCount1 = ++struImgInfo.nQmsImgNoCnt.nKvCntVal;
					break;
				}
				m_fnRenameQmsImgFile( struImgInfo, strNewData, nDefectCount1);

				/*cImgtemp.Format(_T("%s"),EXTENSION_JPG);
				strDefNo.Append(cImgtemp);*/
				strOldFilePath.Format(_T("%s%s\\VisionClient-%d\\%s"),  DATA_PATH, RESULT_PATH, nVisionCount, strOldFileName);
				//strOldFilePath =  strReadFilePath1;
				strTmpFileName = struImgInfo.tImgName;
				strNewData = m_fnSetItemData(D_DEF_IMG_NO , strtemp, strTmpFileName, strTmpFileName.GetLength() , D_ITEM_END - 1);


				//if (FileExist(strOldFilePath))
				//{	
				//	strtemp = struImgInfo.tImgPath;	
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
				//	//strPanelID = m_fnGetItemData(3, strOldFilePath);
				//	strTmpFilePath.Format(_T("D:\\QMS_IMG_TMP\\%s"), strtemp);
				//	pFrame->m_fnCreateFolder(strTmpFilePath);
				//	MoveFile(strOldFilePath, strTmpFilePath + strTmpFileName); 
				//	
				//}
			}

			strDefNo.Format(_T("%d"), nDefectCount1);
			strNewData = m_fnSetItemData(D_DEFECT_NO, strNewData, strDefNo, LEN_DEFECT_NO, D_ITEM_END - 1);

			if(strNewData != "")
				m_strArrMerge.Add(strNewData);  
		}

		strMesure = m_fnGetItemData(LAYER_NO - 1, strLine);

		if (bMeasureFlag && !strMesure.Compare(_T("TFT73")) || !strMesure.Compare(_T("TFT74")) || !strMesure.Compare(_T("TFT75")) || !strMesure.Compare(_T("TFT77")) || !strMesure.Compare(_T("TFT78"))
			|| !strMesure.Compare(_T("CF73"))  || !strMesure.Compare(_T("CF74"))  || !strMesure.Compare(_T("CF75")) || !strMesure.Compare(_T("CF77")) || !strMesure.Compare(_T("POL78")) || !strMesure.Compare(_T("POL76"))  )
		{
			strMeasureVal.Append(strLine);
			strMeasureVal += _T("\n");
		}
		if ( bMeasureFlag && !strMesure.Compare(_T("73")) || !strMesure.Compare(_T("74")) || !strMesure.Compare(_T("75")) 
			|| strMesure == _T("76") || strMesure == _T("77") || strMesure == _T("78"))
		{

			if(nEqpNo && strMesure != (_T("76")) && strMesure != (_T("78")))
			{
				strMeasureVal.Append(_T("CF"));
			}
			else if(!nEqpNo /*&& strMesure != (_T("75"))*/)
			{
				strMeasureVal.Append(_T("TFT"));
			}
			if( strMesure == _T("76") || strMesure == _T("78"))
			{
				strMeasureVal.Append(_T("POL"));
			}
			//m_strArrMerge.Add(strLine); 
			strMeasureVal.Append(strLine);
			strMeasureVal += _T("\n");
		}


		if(strItem == _T("DEFITEM")||strItem == _T("HEADERITEM") ||strItem == _T("HEADERDATA") 
			|| strItem == _T("PANELITEM") || strItem == _T("PANELDATA"))	
		{
			if(strItem == _T("PANELDATA")){
				//2016.05.11 Mr,Chu
				if (strAlignFailcode == ALIGN_FAIL)
				{
					strLine = m_fnSetItemData(6, strLine, strAlignFailcode, LEN_DEFECT_NO, P_ITEM_END + 1);
				}
				if (strMeasureFailcodeCF == MEASUREFAIL_CF)
				{
					strLine = m_fnSetItemData(7, strLine, strMeasureFailcodeCF, LEN_DEFECT_NO, P_ITEM_END + 1);
				}
				if (strMeasureFailcodeTFT == MEASUREFAIL_TFT)
				{
					strLine = m_fnSetItemData(8, strLine, strMeasureFailcodeTFT, LEN_DEFECT_NO, P_ITEM_END + 1);
				}
				//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능
				if((pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS))
				{
					strPolMarkTemp = m_fnGetItemData(9, strLine);

					AfxExtractSubString(strVisionNo, strReadFilePath2, 1, '_');
					strVisionNoTmp.Format(_T("V%d.txt"), nVisionCount);
					//2018.03.14 VISION_PC_2 -> VISION_PC_3, VISION_PC_5 -> VISION_PC_6
					if((nVisionCount == VISION_PC_3 && strVisionNoTmp == strVisionNo)
						|| (nVisionCount == VISION_PC_6 && strVisionNoTmp == strVisionNo))
						m_fnWritePolMarkData(strPanelId, strPolMarkTemp, nVisionCount);
				}

				if (strPolMarkIlegal == POLMARKILEGAL_NG)
				{					
					strLine = m_fnSetItemData(10,strLine , strPolMarkIlegal, LEN_DEFECT_NO, P_ITEM_END + 1);	
				}
				strNewData = strLine;
			}		
			else strNewData = strLine;

			if(strNewData != "") m_strArrPanelData.Add(strNewData);  			
		}
	}

	cstFile.Close();

	if (!cstFile.Open(strNewName, CFile::modeWrite | CFile::modeCreate|CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("Merge NewFile Open Error %s : %d"), strNewName, GetLastError());
		return DEF_MERGE_NG;
	}

	for(int i = 0; i < m_strArrPanelData.GetSize(); i++)
	{
		strLine = m_strArrPanelData.GetAt(i) + _T("\n");
		strNewData = strLine;
		cstFile.WriteString(strNewData);
	}
	for(int i = 0; i < m_strArrMerge.GetSize(); i++)
	{
		strLine = m_strArrMerge.GetAt(i) + _T("\n");
		strNewData = strLine;
		cstFile.WriteString(strNewData);
	}

	cstFile.WriteString(strMeasureVal);

	cstFile.Close();
	finder.Close();
	G_AddLog(3, 3, _T("m_fnDefectFileMerge() - End"));
	return DEF_MERGE_OK;
}

int CDataManager::m_fnMoveQMSimg(CString strFilePath) // 2016.03.15 add by sgkim - vision client에서 QMS_IMG_TMP로 이동
{
	G_AddLog(3, 3, _T("m_fnMoveQMSimg() - Start"));
	CString		strVisionNum, strtemp, strLine, strItem, strDefectType,strPanelID, strStepId, strOldVisionNum = 0;//, strGlassID = _T("");
	CString		strVisionPath, strQmsTempPath;
	CString		strOldFileName, strNewFileName;
	int			nEqpSizeType, nEqpModel, nImgCount = 0;
	CStringArray strRenameInfo;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	TCHAR chRead[255] = {0};
	BOOL		brc;

	nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	CStdioFile cstFile;
	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(2, 0, _T("m_fnMoveQMSimg Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("PANELDATA"))
		{
			strPanelID = m_fnGetItemData(3,strLine);
		}
		if(strItem ==  _T("DEFECT"))
		{
			strDefectType = m_fnGetItemData(DEFECT_TYPE + 1,strLine);
			
			if(nEqpSizeType != EQP_LARGE_SIZE && nEqpSizeType != EQP_MONITOR_SIZE && nEqpModel == EQP_TYPE_AVL)
			{
				strVisionNum = m_fnGetItemData(D_FLAG - 1,strLine);
				int nTempVisionNum = _ttoi(strVisionNum);
				nTempVisionNum++;
				strVisionNum.Format(_T("%d"),nTempVisionNum);
			}
			if(nEqpSizeType != EQP_LARGE_SIZE && nEqpSizeType != EQP_MONITOR_SIZE && nEqpModel == EQP_TYPE_ABI)
			{
				strVisionNum = m_fnGetItemData(D_FLAG - 1,strLine);
				if(strVisionNum == _T("0"))
					strVisionNum.Format(_T("%d"),3);
			}
			if(nEqpSizeType == EQP_LARGE_SIZE || nEqpSizeType == EQP_MONITOR_SIZE)
				strVisionNum = m_fnGetItemData(D_CAM_NO - 1,strLine);

			if(strVisionNum != strOldVisionNum)
				nImgCount = 0;
			strOldVisionNum = strVisionNum;
			nImgCount++;
			strNewFileName = m_fnGetItemData(DEFECT_IMG_NO +1 ,strLine);
			strOldFileName.Format(_T("%s_%s_%00004d"),strPanelID,strDefectType,nImgCount);
			strOldFileName.Append(EXTENSION_JPG);
			strVisionPath.Format(_T("%s%s\\VisionClient-%s\\%s"),  DATA_PATH, RESULT_PATH, strVisionNum , strOldFileName);

			memset(chRead,0X00,sizeof(chRead));	
			pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
			strStepId  = chRead;
			if (strStepId == _T(""))
			{
				if(pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0)==0)
					strStepId=_T("51400");					// AVL StepNo.
				else if(pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0)==1)
					strStepId=_T("61600");					// ABI StepNo.
			}


			//2018.01.17
			CString			strYear, strMonth, strDay, strHour, strMin, strSec;
			CString			strDate, strTime, strtemp;
			SYSTEMTIME		time;

			strtemp = m_strSaveDate;	//check time
			strYear = strtemp.Mid(0, 4);
			strMonth = strtemp.Mid(4, 2);
			strDay = strtemp.Mid(6, 2);
			strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

			strtemp = m_strSaveTime;	//check time
			strHour = strtemp.Mid(0, 2);
			strMin = strtemp.Mid(2, 2);
			strSec = strtemp.Mid(4, 2);
			strTime.Format(_T("%s_%s_%s"),strHour,strMin,strSec);
			G_AddLog(3, 3, _T("Check Time m_fnMoveQMSimg() Date:%s Time:%s"), m_strSaveDate, m_strSaveTime);

			//strTime.Format(_T("%02d_%02d_%02d"),time.wHour,time.wMinute,time.wMinute);

			//2018.01.17
			//strQmsTempPath.Format(_T("D:\\QMS_IMG_TMP\\%s\\%s\\%s\\%s\\"), m_strSaveDate, strStepId, strPanelID, m_strSaveTime);
			strQmsTempPath.Format(_T("D:\\QMS_IMG_TMP\\%s\\%s\\%s\\%s\\"), strDate, strStepId, strPanelID, strTime);

			m_strReJudgeImgPath.Format(_T("D:\\QMS\\i_temp\\%s\\%s\\%s\\%s\\"), strDate, strStepId, strPanelID, strTime); //2019.01.18 add by rjb

			//strQmsTempPath.Format(_T("D:\\QMS_IMG_TMP\\%s\\%s\\%s\\%s\\%s\\"), m_strSaveDate, strStepId, strGlassID, strPanelID, m_strSaveTime);	//2017.06.29	Qms Img 경로 내 GlassID 추가
			pFrame->m_fnCreateFolder(strQmsTempPath);
			if(FileExist(strVisionPath))
			{
				brc = MoveFile(strVisionPath, strQmsTempPath + strNewFileName);
				if(!brc)
					G_AddLog(2, 0,_T("m_fnMoveQMSimg : %s -> %s Failed"), strVisionPath, strQmsTempPath + strNewFileName);
			}
			else
				G_AddLog(2, 0,_T("m_fnMoveQMSimg : %s is not exist"), strVisionPath);
		}

	}


	cstFile.Close();







	//for(int i = 0 ; i < strArrMerge.GetSize() ; i ++)
	//{
	//	if(nEqpSizeType == EQP_MEDIUM_SIZE && nEqpModel == EQP_TYPE_AVL)
	//	{
	//		strVisionNum = m_fnGetItemData(D_FLAG - 1,strArrMerge.GetAt(i));
	//		strVisionNum.Format(_T("%d"),_ttoi(strVisionNum) + 1);
	//	}
	//	if(nEqpSizeType == EQP_MEDIUM_SIZE && nEqpModel == EQP_TYPE_ABI)
	//	{
	//		strVisionNum = m_fnGetItemData(D_FLAG - 1,strArrMerge.GetAt(i));
	//		if(strVisionNum == _T("0"))
	//			strVisionNum.Format(_T("%d"),3);
	//	}
	//	if(nEqpSizeType == EQP_LARGE_SIZE)
	//		strVisionNum = m_fnGetItemData(D_CAM_NO - 1,strArrMerge.GetAt(i));

	//	strOldFileName = m_fnGetItemData(1,strArrImgName.GetAt(i));
	//	strNewFileName = m_fnGetItemData(2,strArrImgName.GetAt(i));

	//	//strVisionNum.Format(_T("%d"),_ttoi(strVisionNum) + 1);
	//	strVisionPath.Format(_T("%s%s\\VisionClient-%s\\%s"),  DATA_PATH, RESULT_PATH, strVisionNum , strOldFileName);

	//	strtemp = struImgInfo.tImgPath;	
	//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
	//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
	//	strtemp = strtemp.Mid(strtemp.Find(_T("\\")) + 1);
	//	//strPanelID = m_fnGetItemData(3, strOldFilePath);
	//	strQmsTempPath.Format(_T("D:\\QMS_IMG_TMP\\%s"), strtemp);

	//	pFrame->m_fnCreateFolder(strQmsTempPath);
	//	MoveFile(strVisionPath, strQmsTempPath + strNewFileName); 
	//}
	G_AddLog(3, 3, _T("m_fnMoveQMSimg() - End"));
	return 0;
}

// nItemNo 는 0 부터 시작
CString CDataManager::m_fnGetItemData(int nItemNo, CString strData)
{
	CString strValue;

	AfxExtractSubString(strValue, strData, nItemNo, ',');
	strValue.Trim();

	return strValue;
}

// Param 1 : 수정할려고 하는 값의 인덱스, 1부터 시작
// Param 2 : 전체 문자열
// Param 3 : 수정할 데이터
// Param 4 : 수정할 데이터의 길이
// Param 5 : 전체 인덱스 크기, 1부터 시작
CString CDataManager::m_fnSetItemData(int nItemNo, CString strData, CString strValue, int nItemLen, int nIndexEnd)
{
	CString strDataLeft, strDataRight, strNewData;
	int nFind = 0;
	int nItemIndex = 0;

	strValue = m_fnFillSpaceStr(strValue, nItemLen);

	nFind = strData.Find(EXT_COLON);

	if(nItemNo == 1)								// 처음 데이터 이면...
	{
		strDataLeft = _T("");
		strDataRight = strData.Right(strData.GetLength() - nFind);	// "," 포함해서 Right 에 넣음.
		strNewData = strDataLeft + strValue + strDataRight;

		return strNewData;
	}

	while(nFind != -1)
	{
		nItemIndex++;

		if(nItemIndex == nItemNo - 1)
			break;

		nFind = strData.Find(EXT_COLON, nFind + 1);
	}

	if(nFind == -1)	return _T("");

	strDataLeft = strData.Left(nFind + 1);			// "," 포함해서 Left 에 넣음.

	if(nItemNo == nIndexEnd)						// 마지막 데이터 이면...
		return strDataLeft + strValue;

	nFind = strData.Find(EXT_COLON, nFind + 1);		

	if(nFind == -1) return _T("");

	strDataRight = strData.Right(strData.GetLength() - nFind);	// "," 포함해서 Right 에 넣음.

	strNewData = strDataLeft + strValue + strDataRight;

	return strNewData;
}

CString CDataManager::m_fnSetItemMutiData(int nItemNo, CString strData, CString strValue, int nItemLen, int nIndexEnd, CString strExt)
{
	CString strDataLeft, strDataRight, strNewData;
	int nFind = 0;
	int nItemIndex = 0;

	strValue = m_fnFillSpaceStr(strValue, nItemLen);

	nFind = strData.Find(strExt);

	if(nItemNo == 1)								// 처음 데이터 이면...
	{
		strDataLeft = _T("");
		strDataRight = strData.Right(strData.GetLength() - nFind);	// "," 포함해서 Right 에 넣음.
		strNewData = strDataLeft + strValue + strDataRight;

		return strNewData;
	}

	while(nFind != -1)
	{
		nItemIndex++;

		if(nItemIndex == nItemNo - 1)
			break;

		nFind = strData.Find(strExt, nFind + 1);
	}

	if(nFind == -1)	return _T("");

	strDataLeft = strData.Left(nFind + 1);			// "," 포함해서 Left 에 넣음.

	if(nItemNo == nIndexEnd)						// 마지막 데이터 이면...
		return strDataLeft + strValue;

	nFind = strData.Find(strExt, nFind + 1);		

	if(nFind == -1) return _T("");

	strDataRight = strData.Right(strData.GetLength() - nFind);	// "," 포함해서 Right 에 넣음.

	strNewData = strDataLeft + strValue + strDataRight;

	return strNewData;
}

CString CDataManager::m_fnFillSpaceStr(CString strData, int nItemLen)
{
	CString strSpace = _T(" ");

	for(int i = strData.GetLength(); i < nItemLen; i++)
		strData += strSpace;

	return strData;
}

void CDataManager::m_fnResetArray()
{
	m_strArrMerge.RemoveAll();

	for(int i = 0; i < MAX_VISION_PC; i++)
		m_strArrDefect[i].RemoveAll();
}

void CDataManager::m_fnFileToArray(CString strPath, int nIndex)
{
	CStdioFile cstFile;
	CString strLine;

	m_strArrDefect[nIndex].RemoveAll();

	if (!cstFile.Open(strPath, CFile::modeRead|CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("Defect File Open Error %s : %d"), strPath, GetLastError());
		return;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == _T(""))	continue;

		m_strArrDefect[nIndex].Add(strLine);
	}

	cstFile.Close();
}

int CDataManager::m_fnClassifyProcInfo(CString strPanelID_1, CString strPanelID_2, int nVisionID)
{
	G_AddLog(3, 3, _T("m_fnClassifyProcInfo() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Result Data 용 Panel 정보 처리 부*/
	int nVisionCnt = 0, nRet = NG;
	BOOL bPanelUseChk = FALSE;		
	FILE /**in = NULL,*/ *out = NULL;

	CString strtemp = _T(""), strClsEndInfoPath = _T("");
	CString cCompareVal ;
	size_t read_size = 0;
	errno_t errFileOpen;		
	int nEqpSizeType = 0;
	SYSTEMTIME	systime ;
	LONGLONG   llStartTime, llCmpTime;
	::GetLocalTime(&systime);
 	int nCnt = 0, ntimeInx = 0;
	llStartTime =  (systime.wYear% 2000)*100000000 +  systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;
	nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
	STRU_NG_FLAG_INFO stNgFlagInfo_Panel1, stNgFlagInfo_Panel2;	//2016.08.01
	//int nPcClassifyReadRetry = 0;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2015.06.12 add by IKM - S		

	//for (int nPanelCnt = 0; nPanelCnt < MAX_REV_PANEL; nPanelCnt++ )
	//{
	//	for (int nStageCnt = 0 ; nStageCnt < MAX_PANEL ; nStageCnt++)
	//	{
	//		m_stPcClassifyEndFlag[nPanelCnt][nStageCnt].init_info();
	//	}
	//}

	strClsEndInfoPath.Format(_T("%s%s"), ROOT_PATH , CLASSIFY_INFO_PATH);

	//while(nPcClassifyReadRetry < 3)
	//{
	//	errFileOpen = _tfopen_s(&in, strClsEndInfoPath , _T("rb+")); // 구조체 바이너리 파일을 읽음

	//	if(errFileOpen == 0)
	//	{		
	//		read_size = fread(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag), 1, in);			
	//		fclose(in);
	//		G_AddLog(2, 3, _T("ClassifyFile Read Success : m_fnClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
	//		break;
	//	}
	//	else
	//	{
	//		G_AddLog(2, 3, _T("ClassifyFile Read Error : m_fnClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
	//	//	fclose(in);
	//	}

	//	nPcClassifyReadRetry++;
	//}

	bPanelUseChk = FALSE;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2016.08.01
	if (nEqpSizeType == EQP_SMALL_SIZE && strPanelID_1.IsEmpty())
	{
		while(nVisionCnt < MAX_REV_PANEL)
		{		
			cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID ; cCompareVal.TrimRight();

			if (!cCompareVal.IsEmpty() && cCompareVal == strPanelID_2)				
			{				
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nTotalDefCnt[nVisionID] = 0;
				}
				if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
				{

					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID;
				}
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인						

				if (nEqpSizeType == EQP_SMALL_SIZE)
				{
					if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
					{
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nTotalDefCnt[nVisionID] = 0;
					}
					if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
					{

						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID;
					}				
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE;	
				}
				bPanelUseChk = TRUE;	
				break;

			}
			nVisionCnt++;
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			while(nVisionCnt < MAX_REV_PANEL)
			{

				cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID ; cCompareVal.TrimRight();

				if (cCompareVal.IsEmpty() && !strPanelID_2.IsEmpty())
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].init_info();
					G_AddLog(2,2, _T("m_fnClassifyProcInfo1 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID , strPanelID_2);
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].llStartTime = llStartTime;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID;

					if (nEqpSizeType == EQP_SMALL_SIZE)
					{
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].init_info();
						G_AddLog(2, 2, _T("m_fnClassifyProcInfo2 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
						_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID , strPanelID_1);
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime = llStartTime;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID ;
					}
					bPanelUseChk = TRUE;
					break;
				}
				nVisionCnt++;
			}
		}	
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			llCmpTime  = m_stPcClassifyEndFlag[0][PANEL_ONE].llStartTime ;
			while(nVisionCnt < MAX_REV_PANEL)
			{			
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime < llCmpTime )
				{
					llCmpTime = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime ;
					ntimeInx = nVisionCnt;
				}			
				nVisionCnt++;
			}
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].init_info();
			G_AddLog(2, 2, _T("m_fnClassifyProcInfo3 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
			_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID , strPanelID_1);
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].llStartTime = llStartTime;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nOrderCnt++;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nOrderCnt] = nVisionID;

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].init_info();
				G_AddLog(2, 2, _T("m_fnClassifyProcInfo4 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
				_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID , strPanelID_2);
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].llStartTime = llStartTime;
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nOrderCnt++;
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nOrderCnt] = nVisionID;

			}

			nVisionCnt = ntimeInx;
		}	

	}
	else
	{
		while(nVisionCnt < MAX_REV_PANEL)
		{		
			cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();

		if (!cCompareVal.IsEmpty() && cCompareVal == strPanelID_1)				
		{				
			if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
			{
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nTotalDefCnt[nVisionID] = 0;
			}
			if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
			{

				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID;
			}
			m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인						

			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nTotalDefCnt[nVisionID] = 0;
				}
				if (!m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
				{

					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID;
				}				
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE;	
			}
			bPanelUseChk = TRUE;	
			break;

		}
		nVisionCnt++;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!bPanelUseChk)
	{
		nVisionCnt = 0;
		while(nVisionCnt < MAX_REV_PANEL)
		{

			cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();

			if (cCompareVal.IsEmpty() && !strPanelID_1.IsEmpty())
			{
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].init_info();
				G_AddLog(2, 3, _T("m_fnClassifyProcInfo1 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
				_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID , strPanelID_1);
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt++;
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime = llStartTime;
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nOrderCnt] = nVisionID;

				if (nEqpSizeType == EQP_SMALL_SIZE)
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].init_info();
					G_AddLog(2, 3, _T("m_fnClassifyProcInfo2 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID , strPanelID_2);
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt++;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].llStartTime = llStartTime;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nOrderCnt] = nVisionID ;
				}
				bPanelUseChk = TRUE;
				break;
			}
			nVisionCnt++;
		}
	}	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (!bPanelUseChk)
	{
		nVisionCnt = 0;
		llCmpTime  = m_stPcClassifyEndFlag[0][PANEL_ONE].llStartTime ;
		while(nVisionCnt < MAX_REV_PANEL)
		{			
			if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime < llCmpTime )
			{
				llCmpTime = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime ;
				ntimeInx = nVisionCnt;
			}			
			nVisionCnt++;
		}
		m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].init_info();
		G_AddLog(2, 3, _T("m_fnClassifyProcInfo3 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
		_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID , strPanelID_1);
		m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].llStartTime = llStartTime;
		m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	
		m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nOrderCnt++;
		m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nVisionNo[m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].nOrderCnt] = nVisionID;

		if (nEqpSizeType == EQP_SMALL_SIZE)
		{
			m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].init_info();
			G_AddLog(2, 3, _T("m_fnClassifyProcInfo4 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
			_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID , strPanelID_2);
			m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].llStartTime = llStartTime;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

			m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nOrderCnt++;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nVisionNo[m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].nOrderCnt] = nVisionID;

		}

			nVisionCnt = ntimeInx;
		}	
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(pFrame->m_stParamInfo.nEqpObject == B_PROJECT)
	{
		nRet = m_fnResultProcess( m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE], m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO], nVisionID);
	}
	else if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
	{
		nRet = m_fnResultProcessPacking( m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE], m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO], nVisionID);
	}

	read_size = 0;	
	errFileOpen = _tfopen_s(&out, strClsEndInfoPath, _T("wb"));
	if(errFileOpen == 0)
	{
		fwrite(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag) , 1, out);		
		fclose(out);
	}
	else
	{
		G_AddLog(2, 3, _T("ClassifyFile Write Error : m_fnClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//2015.06.12 add by IKM - E
	//m_nVisionState[nVisionID - 1] = STATE_INSP_END;	//2015.07.05
	G_AddLog(3, 3, _T("m_fnClassifyProcInfo() - End"));
	return nRet;
}

int CDataManager::m_fnPgClassifyProcInfo(CString strPanelID_1, CString strPanelID_2, CString strContResult1, CString strPgResult1,
	CString strPgVal1,CString strContResult2, CString strPgResult2,	CString strPgVal2, CString strConFlag, int nVisionID)
{

	G_AddLog(3, 3, _T("m_fnPgClassifyProcInfo() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	/////////////////////////////////////////////
	/*Result Data 용 Panel 정보 처리 부*/
	int nVisionCnt = 0, nRet = NG;;
	BOOL bPanelUseChk = FALSE;
	FILE /**in = NULL,*/ *out = NULL;
	CString strtemp = _T(""), strClsEndInfoPath = _T("");
	CString cCompareVal ;
	size_t read_size = 0;
	errno_t errFileOpen;
	int nEqpSizeType = 0;
	nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);
	SYSTEMTIME	systime ;
	LONGLONG   llStartTime, llCmpTime;
	::GetLocalTime(&systime);
	int nCnt = 0, ntimeInx = 0;
	llStartTime =  (systime.wYear% 2000)*100000000 +  systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;
	/////////////////////////////////////////////
	CString strPgSocket;
	int nPgGrade1 = 0, nPgGrade2 = 0;


	// 서버로 결과를 전송한다.	OK : 0, NG : 1
	//strMsg.Format(_T("%c.%s.%s.%s.%s.%s.%s.%s.%d.%c"), STX, PTM_RESULT, strPanelID_1, strPanelID_2, abs(m_iContFlag1 - 1), abs(m_iResultFlag1 - 1), m_strPgVal1, abs(m_iContFlag2 - 1),abs(m_iResultFlag2 - 1), m_strPgVal2, E_SEL_PG1, ETX);

	if (_ttoi(strContResult1) == 1)		nPgGrade1 = CONTACT_NG;			

	if (_ttoi(strContResult2) == 1)		nPgGrade2 = CONTACT_NG;	

	if (_ttoi(strContResult1) == 0 && _ttoi(strPgResult1) == 1)		nPgGrade1 = ITO_NG;			

	if (_ttoi(strContResult2) == 0 && _ttoi(strPgResult2) == 1)		nPgGrade2 = ITO_NG;			

	strPanelID_1.TrimRight();	 strPanelID_2.TrimRight();

	BOOL  bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_USE_SEMAPHORE, 1);
	int nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	if (nEqpNo == 1 && bUseSemaphore)
	{			

		//for (int nPanelCnt = 0; nPanelCnt < MAX_REV_PANEL; nPanelCnt++ )
		//{
		//	for (int nStageCnt = 0 ; nStageCnt < MAX_PANEL ; nStageCnt++)
		//	{
		//		m_stPcClassifyEndFlag[nPanelCnt][nStageCnt].init_info();
		//	}
		//}

		strClsEndInfoPath.Format(_T("%s%s"), ROOT_PATH , CLASSIFY_INFO_PATH);
		//errFileOpen = _tfopen_s(&in, strClsEndInfoPath, _T("rb+"));

		//if(errFileOpen == 0)
		//{		
		//	read_size = fread(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag), 1, in);			
		//	fclose(in);
		//}
		//else
		//{
		//	G_AddLog(2, 3, _T("ClassifyFile Read Error : m_fnPgClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
		//}

		bPanelUseChk =FALSE;
		nVisionCnt = 0;
		// 순차적으로 저장할 Panel id의 정보를 셋팅한다.

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		while(nVisionCnt < MAX_REV_PANEL)
		{		
			cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();

			if (!cCompareVal.IsEmpty() && cCompareVal == strPanelID_1)				
			{				
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID])
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nTotalDefCnt[nVisionID] = 0;
				}

				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인

				//if (_ttoi(strPgResult1)  != 0)
				if (nPgGrade1 != 0)
				{
					//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nPgGrade  = nPgGrade1;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
					//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPgVal , strPgVal1);
				}	

				if (nEqpSizeType == EQP_SMALL_SIZE)
				{
					if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID])
					{
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nTotalDefCnt[nVisionID] = 0;
					}

					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE;		

					//if (_ttoi(strPgResult2)  != 0)
					if (nPgGrade2 != 0)
					{
						//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nPgGrade  = nPgGrade2;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
						//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
						_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPgVal , strPgVal2);
					}	
				}
				bPanelUseChk = TRUE;
				break;
			}
			nVisionCnt++;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			while(nVisionCnt < MAX_REV_PANEL)
			{

				cCompareVal = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();
				//if (!cCompareVal.Compare(_T(""))) cCompareVal = _T(" ");
				if (cCompareVal.IsEmpty() && !strPanelID_1.IsEmpty())
				{
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].init_info();
					G_AddLog(2, 3, _T("m_fnPgClassifyProcInfo1 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID , strPanelID_1);
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime = llStartTime;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

					//if (_ttoi(strPgResult1)  != 0)
					if (nPgGrade1 != 0)
					{
						//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nPgGrade  = nPgGrade1;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
						//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
						_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPgVal , strPgVal1);
					}	
					if (nEqpSizeType == EQP_SMALL_SIZE)
					{
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].init_info();
						G_AddLog(2, 3, _T("m_fnPgClassifyProcInfo2 init_info PanelID : %s"), m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
						_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPanelID , strPanelID_2);
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].llStartTime = llStartTime;
						m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

						//if (_ttoi(strPgResult2)  != 0)
						if (nPgGrade2 != 0)
						{
							//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
							m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nPgGrade  = nPgGrade2;
							m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
							//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
							_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPgVal , strPgVal2);
						}	
					}
					bPanelUseChk = TRUE;
					break;
				}
				nVisionCnt++;
			}
		}		

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (!bPanelUseChk)
		{
			nVisionCnt = 0;
			llCmpTime  = m_stPcClassifyEndFlag[0][PANEL_ONE].llStartTime ;
			while(nVisionCnt < MAX_REV_PANEL)
			{			
				if (m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime < llCmpTime )
				{
					llCmpTime = m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].llStartTime ;
					ntimeInx = nVisionCnt;
				}			
				nVisionCnt++;
			}

			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].init_info();
			G_AddLog(2, 3, _T("m_fnPgClassifyProcInfo3 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
			_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].cPanelID , strPanelID_1);
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].llStartTime = llStartTime;
			m_stPcClassifyEndFlag[ntimeInx][PANEL_ONE].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

			//if (_ttoi(strPgResult1)  != 0)
			if (nPgGrade1 != 0)
			{
				//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nPgGrade  = nPgGrade1;
				m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
				//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
				_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPgVal , strPgVal1);
			}	
			if (nEqpSizeType == EQP_SMALL_SIZE)
			{
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].init_info();
				G_AddLog(2, 3, _T("m_fnPgClassifyProcInfo4 init_info PanelID : %s"), m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID);	//2016.01.07 CHECK QMS COUNT LOG
				_tcscpy(m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].cPanelID , strPanelID_2);
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].llStartTime = llStartTime;
				m_stPcClassifyEndFlag[ntimeInx][PANEL_TWO].bComplete[nVisionID] = TRUE; // 각 PC의 데이터 취합 여부를 확인	

				//if (_ttoi(strPgResult2)  != 0)
				if (nPgGrade2 != 0)
				{
					//pFrame->m_DataManager.m_fnSetSemaGrade(_T("CONTACT_NG"));
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nPgGrade  = nPgGrade2;
					m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].nConFlag  = _tstoi((LPCTSTR)strConFlag) ;
					//pFrame->m_DataManager.m_fnSetPgValue(strPgVal);
					_tcscpy(m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO].cPgVal , strPgVal2);
				}	
			}

			nVisionCnt = ntimeInx;
		}	
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		//pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,1);
		//pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,2);
		//pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,3);

		BOOL bInspUse = TRUE;
		if( pView->m_pDlgPLC->m_fnGetDevice(_T("L352")) && pView->m_pDlgPLC->m_fnGetDevice(_T("L353"))&&  pView->m_pDlgPLC->m_fnGetDevice(_T("L354")) )
			bInspUse = FALSE;

		if( !bInspUse )
		{
			pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,1);
			pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,2);
		}
		nRet = pFrame->m_DataManager.m_fnResultProcess( m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE]
		,m_stPcClassifyEndFlag[nVisionCnt][PANEL_TWO], SIGNAL_PC);
		if( !bInspUse )
			pFrame->m_DataManager.m_fnClassifyProcInfo(strPanelID_1,strPanelID_2,3);
		
		read_size = 0;	
		errFileOpen = _tfopen_s(&out, strClsEndInfoPath, _T("wb"));
		if(errFileOpen == 0)
		{				
			fwrite(&m_stPcClassifyEndFlag, sizeof(m_stPcClassifyEndFlag) , 1, out);		
			fclose(out);
		}
		else
		{
			G_AddLog(2, 3, _T("ClassifyFile Write Error : m_fnPgClassifyProcInfo"));	//2016.01.07 CHECK QMS COUNT LOG
		}

	}
	G_AddLog(3, 3, _T("m_fnPgClassifyProcInfo() - End"));
	return nRet;
}
#if 0

int CDataManager::m_fnResultProcess(STRU_CLASSIFY_INFO &strPanelID1,STRU_CLASSIFY_INFO &strPanelID2, int nVisionCount)
	,STRU_CLASSIFY_INFO &cPanelInfo2, int nVisionCount)
{
	//CString strRootPath, strPathV1, strPathV2, strPathV3, strPath, strTime;

	//CTime time = CTime::GetCurrentTime();
	//strTime = time.Format(_T("%Y%m%d_%H%M%S"));

	//strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);
	//strPath.Format(_T("%s\\%s"), strRootPath, strPanelID);
	//CreateDirectory(strPath, NULL);

	//m_fnResetArray();

	//if(nVisionCount == 1)
	//{
	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.txt"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s.txt"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV1, strPath, 0);		// 데이터 파일 복사

	//	m_fnFileToArray(strPath, VISION_PC_1 - 1);	// Grid 표시용으로 Defect File -> String Array 

	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV1, strPath, 0);		// 이미지 파일 복사
	//}
	//else if(nVisionCount == 2)
	//{
	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.txt"), strRootPath, strPanelID);
	//	strPathV2.Format(_T("%sVisionClient-2\\%s_V2.txt"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s.txt"), strRootPath, strPanelID, strPanelID, strTime);

	//	m_fnDefectFileMerge(strPathV1, strPathV2, strPath);

	//	m_fnFileToArray(strPathV1, VISION_PC_1 - 1);
	//	m_fnFileToArray(strPathV2, VISION_PC_2 - 1);

	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s_V1.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV1, strPath, 0);		// 이미지 파일 복사

	//	strPathV2.Format(_T("%sVisionClient-2\\%s_V2.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s_V2.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV2, strPath, 0);		// 이미지 파일 복사
	//}
	//else if(nVisionCount == 3)
	//{
	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.txt"), strRootPath, strPanelID);
	//	strPathV2.Format(_T("%sVisionClient-2\\%s_V2.txt"), strRootPath, strPanelID);
	//	strPathV3.Format(_T("%sVisionClient-3\\%s_V3.txt"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s.txt"), strRootPath, strPanelID, strPanelID, strTime);

	//	m_fnDefectFileMerge(strPathV1, strPathV2, strPath);		// Merge V1 & V2

	//	m_fnDefectFileMerge(strPath, strPathV3, strPath);		// Merge V1 & V2 & V3

	//	m_fnFileToArray(strPathV1, VISION_PC_1 - 1);
	//	m_fnFileToArray(strPathV2, VISION_PC_2 - 1);
	//	m_fnFileToArray(strPathV3, VISION_PC_3 - 1);

	//	strPathV1.Format(_T("%sVisionClient-1\\%s_V1.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s_V1.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV1, strPath, 0);		// 이미지 파일 복사

	//	strPathV2.Format(_T("%sVisionClient-2\\%s_V2.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s_V2.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV2, strPath, 0);		// 이미지 파일 복사	

	//	strPathV3.Format(_T("%sVisionClient-3\\%s_V3.jpg"), strRootPath, strPanelID);
	//	strPath.Format(_T("%s%s\\%s_%s_V3.jpg"), strRootPath, strPanelID, strPanelID, strTime);

	//	CopyFile(strPathV3, strPath, 0);		// 이미지 파일 복사	
	//}

	////m_fnUpdateSummary();		// Summary 파일 생성 or 업데이트

	//m_fnClearDefectList();		// Defect Summary List의 데이터 초기화	// 2015.01.12 ljk
	//
	//m_fnUpdateDefectList();		// Defect List 에 데이터 업데이트
	int nVisionCnt = 0 ,nEqpNo = 0;
	int nTotalImgCnt = 0;
	CString strTemp = _T("");
	CString strQmsFilePath;
	CString cCompareVal;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	BOOL bQmsStartFlag =FALSE;
	CString strResutJudgeThreadVal;

	int nProcQms = 0, nQmsFinalCnt = 0;

	STRU_CLASSIFY_INFO strPanelInfo1;
	STRU_CLASSIFY_INFO strPanelInfo2;

	strPanelInfo1 = cPanelInfo1;
	strPanelInfo2 = cPanelInfo2;

	strResutJudgeThreadVal.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s"), 
		_T("PNL0000000000001"), _T("A"), _T("1111"), _T("2222"), 
		_T("PNL0000000000002") ,_T("B"), _T("4444"), _T("4444")) ;
	UINT uiThreadID;
	pFrame->m_DataManager.pTreadCimProc->m_strTheadData = strResutJudgeThreadVal;
	pFrame->m_DataManager.pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, pFrame->m_DataManager.pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);

}
#else

int CDataManager::m_fnResultProcess(STRU_CLASSIFY_INFO &cPanelInfo1,STRU_CLASSIFY_INFO &cPanelInfo2, int nVisionCount)
{ 
	G_AddLog(2, 3, _T("m_fnResultProcess() - Start"));
	int nVisionCnt = 0 ,nEqpNo = 0, nRet = NG;
	int nTotalImgCnt = 0;
	CString strTemp = _T("");
	CString strQmsFilePath;
	CString cCompareVal;
	CString		strStartDate, strStartTime, strSaveDate, strSaveTime;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	BOOL bQmsStartFlag =FALSE;
	CString strResutJudgeThreadVal;
	BOOL brc;
	BOOL bNgFlag = FALSE;
	SYSTEMTIME		time;		//2020.06.25 add by mby
	CString strTime = _T("");	//2020.06.25 add by mby	

	int nProcQms = 0;
	int nInspPcUseCnt = 0;

	STRU_CLASSIFY_INFO strPanelInfo1;
	STRU_CLASSIFY_INFO strPanelInfo2;

	strPanelInfo1 = cPanelInfo1;
	strPanelInfo2 = cPanelInfo2;

	//bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 1);

	CString cImgTmpPath, cImgQmsPath;
	nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	m_bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////
	//2015.06.12 add by IKM - S	
	for (int i = 1 ; i< MAX_VISION_PC ;i++) // 투입 된 Panel의 검사가 모두 끝났는지 체크
	{
		if (strPanelInfo1.bComplete[i] == TRUE)
		{
			if ( i != SIGNAL_PC ) 
				nProcQms++;
		}		
	}
	//2020.07.22 ad by mby S-
	for (int i = 0 ; i < pFrame->m_stParamInfo.nCountVisionClient ;i++) // 투입 된 Panel의 검사가 모두 끝났는지 체크
	{
		if (pFrame->m_stParamInfo.bUseInspPc[i] == TRUE)
		{
			nInspPcUseCnt++;
		}
	}
	//2020.07.22 ad by mby E-
	//2017.07.01 Signal PC 보고 시 검사 신호 초기화 타이밍문제에 대한 예외처리
	if( nProcQms == NULL && strPanelInfo1.bComplete[SIGNAL_PC] == TRUE )
		m_nQmsFinalCnt ++;
	else if( nProcQms != NULL && strPanelInfo1.bComplete[SIGNAL_PC] == FALSE )
		m_nQmsFinalCnt = nProcQms;
	else if( nProcQms != NULL && strPanelInfo1.bComplete[SIGNAL_PC] == TRUE )
		m_nQmsFinalCnt = nProcQms + 1;
	//

	G_AddLog(2, 3, _T("[Vision : %d] [Panel Id1 : %s] [Panel Id2 : %s] nProcQms : %d nQmsFinalCnt : %d"), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo2.cPanelID, nProcQms, m_nQmsFinalCnt);	//2016.01.07 CHECK QMS COUNT LOG

	if (nVisionCount <= pFrame->m_stParamInfo.nCountVisionClient)
	{	
		cCompareVal = strPanelInfo1.cPanelID;
		if (cCompareVal != (_T(""))) 
		{			 			
			_tcscpy(strPanelInfo1.cPanelPath  , m_fnReadDefectData(strPanelInfo1, nVisionCount));			
			//pFrame->m_DataManager.m_strPanelPath[PANEL_ONE] = strPanelInfo1.cPanelPath ;
			// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.
			/*if (nProcQms == 1)
			{
			m_fnGetDefStartDate(strPanelInfo1.cPanelPath);				
			_tcscpy(strPanelInfo1.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo1.cStartTime  , m_strInspTime);		
			_tcscpy(strPanelInfo1.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo1.cDataTime   , m_strSaveTime);		
			}*/
			//2020.06.25 add by mby S-
			::GetLocalTime(&time);
			strTime.Format(_T("%4d/%02d/%02d"),time.wYear,time.wMonth,time.wDay);
			_tcscpy(strPanelInfo1.cStartDate  , strTime);			
			strTime.Format(_T("%02d:%02d:%02d"),time.wHour,time.wMinute,time.wSecond);
			_tcscpy(strPanelInfo1.cStartTime  , strTime);
			strTime.Format(_T("%4d%02d%02d"),time.wYear,time.wMonth,time.wDay);
			_tcscpy(strPanelInfo1.cDataDate  , strTime);
			strTime.Format(_T("%02d%02d%02d"),time.wHour,time.wMinute,time.wSecond);
			_tcscpy(strPanelInfo1.cDataTime  , strTime);
			//2020.06.25 add by mby E-

			strStartDate = strPanelInfo1.cStartDate;	strStartTime = strPanelInfo1.cStartTime;
			strSaveDate = strPanelInfo1.cDataDate;		strSaveTime = strPanelInfo1.cDataTime;
			if (strStartDate == _T("") || strStartTime  == _T("") 
				|| strSaveDate == _T("") || strSaveTime  == _T(""))
			{
				m_fnGetDefStartDate(strPanelInfo1.cPanelPath);				
				_tcscpy(strPanelInfo1.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo1.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo1.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo1.cDataTime  , m_strSaveTime);	
			}
			m_strInspDate = strPanelInfo1.cStartDate;					m_strInspTime = strPanelInfo1.cStartTime;
			m_strSaveDate = strPanelInfo1.cDataDate ;					m_strSaveTime = strPanelInfo1.cDataTime;

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo1.cStartDate);

			m_fnSetDefStartDate(strPanelInfo1.cPanelPath);		

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Write Defect Data!!"), nVisionCount, strPanelInfo1.cPanelID);
		}
		cCompareVal = strPanelInfo2.cPanelID ;
		if (cCompareVal != _T(""))
		{			
			_tcscpy(strPanelInfo2.cPanelPath  , m_fnReadDefectData(strPanelInfo2, nVisionCount));
			//pFrame->m_DataManager.m_strPanelPath[PANEL_TWO] = strPanelInfo2.cPanelPath ;
			/*if (nProcQms == 1)
			{
			m_fnGetDefStartDate(strPanelInfo2.cPanelPath);				
			_tcscpy(strPanelInfo2.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo2.cStartTime  , m_strInspTime);		
			_tcscpy(strPanelInfo2.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo2.cDataTime  , m_strSaveTime);	
			}*/
			strStartDate = strPanelInfo2.cStartDate;	strStartTime = strPanelInfo2.cStartTime;
			strSaveDate = strPanelInfo2.cDataDate;		strSaveTime = strPanelInfo2.cDataTime;
			if (strStartDate == _T("") || strStartTime  == _T("") 
				|| strSaveDate == _T("") || strSaveTime  == _T(""))
			{
				m_fnGetDefStartDate(strPanelInfo2.cPanelPath);				
				_tcscpy(strPanelInfo2.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo2.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo2.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo2.cDataTime  , m_strSaveTime);	
			}

			m_strInspDate = strPanelInfo2.cStartDate;					m_strInspTime = strPanelInfo2.cStartTime;			
			m_strSaveDate = strPanelInfo2.cDataDate;					m_strSaveTime = strPanelInfo2.cDataTime;

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo2.cPanelID, strPanelInfo2.cStartDate);

			m_fnSetDefStartDate(strPanelInfo2.cPanelPath);			

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Write Defect Data!!"), nVisionCount, strPanelInfo2.cPanelID);
		}

	}
	//2015.06.16 by sjw - s
	int nContactOverlapCnt = 0;
	int nContactPanelOverlapCnt = 0;
	if (nVisionCount == SIGNAL_PC)
	{

		if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE && pFrame->m_stOverlapCoordInfo->bUseContactFail == TRUE)
		{
			nContactPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, 0);
			if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nContactPanelOverlapCnt + 1)
			{
				strTemp.Format(_T("%d"), 0);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
			}
			nContactPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, 0);
			strTemp.Format(_T("%d"), nContactPanelOverlapCnt + 1);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
			if (strPanelInfo1.nPgGrade == ITO_NG || strPanelInfo2.nPgGrade == ITO_NG)
			{
				nContactOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, 0);
				if (pFrame->m_stOverlapCoordInfo->nContactFailDefectCount <= nContactOverlapCnt + 1 && 
					pFrame->m_stOverlapCoordInfo->nPanelCnt >= nContactPanelOverlapCnt + 1)
				{
					if (strPanelInfo1.nPgGrade == ITO_NG)
						m_fnContactFailAlarmSet(_T("PG : ITO RESIST ILEGAL"), strPanelInfo1);
					if (strPanelInfo2.nPgGrade == ITO_NG)		
						m_fnContactFailAlarmSet(_T("PG : ITO RESIST ILEGAL"), strPanelInfo2);

					strTemp.Format(_T("%d"), 0);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				}
				else
				{
					strTemp.Format(_T("%d"), nContactOverlapCnt + 1);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
				}
			}

			if (strPanelInfo1.nPgGrade == CONTACT_NG || strPanelInfo2.nPgGrade == CONTACT_NG)
			{
				nContactOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, 0);
				if (pFrame->m_stOverlapCoordInfo->nContactFailDefectCount <= nContactOverlapCnt + 1 && 
					pFrame->m_stOverlapCoordInfo->nPanelCnt >= nContactPanelOverlapCnt + 1)
				{
					if (strPanelInfo1.nPgGrade == CONTACT_NG)
						m_fnContactFailAlarmSet(_T("PG : Contact ILEGAL"), strPanelInfo1);
					if (strPanelInfo2.nPgGrade == CONTACT_NG)	
						m_fnContactFailAlarmSet(_T("PG : Contact ILEGAL"), strPanelInfo2);

					strTemp.Format(_T("%d"), 0);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				}
				else
				{
					strTemp.Format(_T("%d"), nContactOverlapCnt + 1);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
				}
			}
		}

		G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] SIGNAL_PC!! :  %d"), nVisionCount, strPanelInfo1.cPanelID ,nProcQms);

	}
	//2015.06.16  by sjw - e

	//if (m_nQmsFinalCnt >= pFrame->m_stParamInfo.nCountVisionClient + m_bUseSemaphore)	//2020.07.22 comment by mby	
	if (m_nQmsFinalCnt >= nInspPcUseCnt + m_bUseSemaphore)		//2020.07.22 Use Live Pc Count - nInspPcUseCnt
	{			
		G_AddLog(3, 3, _T("m_stParamInfo.nCountVisionClient() - Start"));
		if (nProcQms == 1)	pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnInitState();

		int nTestMode = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_STATE_TEST_MODE, 0);
		if( !nTestMode)	// 모든 Vision Client 검사가 완료되었으면...
		{	
			G_AddLog(2, 3, _T("[Vision : %d] [PanelID_1 : %s][PanelID_2 : %s] Process QMS Data!! - Start Data Merge "), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo2.cPanelID);

			m_fnSetPanelId1(strPanelInfo1.cPanelID);
			m_fnSetPanelId2(strPanelInfo2.cPanelID);

			//CString strRootPath = _T("");
			//CString strPath = _T("");
			//strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

			cCompareVal = strPanelInfo1.cPanelID;
			if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_ONE] = strPanelInfo1.cPanelPath; // m_fnGetLastWriteFile(strPanelInfo1.cPanelPath);

			cCompareVal = strPanelInfo2.cPanelID;
			if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_TWO] = strPanelInfo2.cPanelPath;// m_fnGetLastWriteFile(strPanelInfo2.cPanelPath);

			//// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.

			if (m_strPanelPath[PANEL_ONE].Compare(_T("")))
			{

				m_strInspDate = strPanelInfo1.cStartDate;	m_strInspTime = strPanelInfo1.cStartTime;
				m_strSaveDate = strPanelInfo1.cDataDate;	m_strSaveTime = strPanelInfo1.cDataTime;

				//if (m_strSemaGrade.Find(_T("ITO_NG")) != -1)
				if (strPanelInfo1.nPgGrade == ITO_NG)	  m_strSemaGrade = _T("ITO_NG");
				if (strPanelInfo1.nPgGrade == CONTACT_NG) m_strSemaGrade = _T("CONTACT_NG");
				//m_fnUpLoadImgFile(strPanelInfo1.cPanelID); // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
				//Sleep(500);

				m_fnMoveQMSimg(m_strPanelPath[PANEL_ONE]); // 2016.03.15 add by sgkim - vision client에서 QMS_IMG_TMP로 이동
				bNgFlag = m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_ONE], strPanelInfo1.cPanelID, PANEL_ONE);	

				// 위치이동_180731_ryu(1)
				G_AddLog(3, 3, _T("WM_UI_CONTROL_DEF_TYPE_DIS_1() - Start"));
				pAuto->m_pDlgDefInfo->PostMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);
				G_AddLog(3, 3, _T("WM_UI_CONTROL_DEF_TYPE_DIS_1() - End"));

				if(strPanelInfo1.cPanelID != _T(""))				// Comment KYH01 디펙맵 실행 위치 변경
				{
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_PANEL_INFO, KEY_PANNELINFO_PANNEL_ID,strPanelInfo1.cPanelID);	
					pFrame->m_fnProcessMapSave();
				}

				//2018.01.22
				if(nEqpNo == EQP_TYPE_AVL)
				{
					m_fnMoveMeasureImg(strPanelInfo1.cPanelID);
					m_fnMoveDefectMapImg(strPanelInfo1.cPanelID);
					m_fnMoveMeasureImg_TFT(strPanelInfo1.cPanelID);	//2020.06.10 add by mby
				}
				else
				{					
					//m_fnMoveMeasureImg(strPanelInfo1.cPanelID);
					//m_fnMoveMeasureImg_TFT(strPanelInfo1.cPanelID);
				}

				//2018.01.22
				CString strtemp;
				CString strDate, strYear, strMonth, strDay;

				strtemp = m_strSaveDate;	//check time
				strYear = strtemp.Mid(0, 4);
				strMonth = strtemp.Mid(4, 2);
				strDay = strtemp.Mid(6, 2);
				strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);
				//2018.01.22

				G_AddLog(2, 3, _T("Check Time m_fnResultProcess() Date:%s"), m_strSaveDate);

				//cImgTmpPath.Format(_T("%s%s"), _T("D:\\QMS_IMG_TMP\\"), strPanelInfo1.cDataDate);
				cImgTmpPath.Format(_T("%s%s"), _T("D:\\QMS_IMG_TMP\\"), strDate);
				//2018.01.17


				if(!pFrame->m_stParamInfo.bUseManualJudge || !bNgFlag && pFrame->m_stParamInfo.bUseManualJudge || ((bNgFlag == 5) && pFrame->m_stParamInfo.bUseManualJudge))  //2019.01.16 by ryu
					cImgQmsPath = _T("D:\\QMS\\i_wait");
				else
					cImgQmsPath = _T("D:\\QMS\\i_temp");
				//if ((CString)strPanelInfo1.cDataDate != _T(""))
				if (strDate != _T(""))
				{
					m_fnExistImage(strPanelInfo1);
					brc = m_fnMoveFolder(cImgTmpPath, cImgQmsPath);
					if(!brc)
						G_AddLog(2, 0, _T("m_fnMoveFolder : %s -> %s failed!"),cImgTmpPath,cImgQmsPath);
				}
				else
					G_AddLog(2, 0, _T("Date is Null!"));

				m_fnDelLocalInspData(strPanelInfo1.cPanelID, nVisionCount) ;  // 이미지 삭제 스레드	

				//m_strDefGrade[PANEL_ONE]			= m_strDefGrade1;
				//m_strdefJudge[PANEL_ONE][JUDGE_ONE] = m_strdefJudge1; 
				//m_strdefJudge[PANEL_ONE][JUDGE_TWO] = m_strdefJudge2;
				//m_strCimMeasureData[PANEL_ONE] = m_strCimMeasureDataTemp;	//2016.01.15

			}
			if (m_strPanelPath[PANEL_TWO].Compare(_T("")))
			{

				m_strInspDate = strPanelInfo2.cStartDate;	m_strInspTime = strPanelInfo2.cStartTime;
				m_strSaveDate = strPanelInfo2.cDataDate;	m_strSaveTime = strPanelInfo2.cDataTime;

				if (strPanelInfo2.nPgGrade == ITO_NG)	  m_strSemaGrade = _T("ITO_NG");
				if (strPanelInfo2.nPgGrade == CONTACT_NG) m_strSemaGrade = _T("CONTACT_NG");

				//m_fnUpLoadImgFile(strPanelInfo2.cPanelID);  // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
				//Sleep(500);

				m_fnMoveQMSimg(m_strPanelPath[PANEL_TWO]); // 2016.03.15 add by sgkim - vision client에서 QMS_IMG_TMP로 이동
				m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_TWO], strPanelInfo2.cPanelID, PANEL_TWO);
				
				// 위치이동_180731_ryu(2)
				G_AddLog(3, 3, _T("WM_UI_CONTROL_DEF_TYPE_DIS_2() - Start"));
				pAuto->m_pDlgDefInfo->PostMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);
				G_AddLog(3, 3, _T("WM_UI_CONTROL_DEF_TYPE_DIS_2() - End"));

				cImgTmpPath.Format(_T("%s%s"), _T("D:\\QMS_IMG_TMP\\"), strPanelInfo2.cDataDate);
				cImgQmsPath = _T("D:\\QMS\\i_wait");
				if ((CString)strPanelInfo2.cDataDate != _T(""))
				{
					m_fnExistImage(strPanelInfo2);
					m_fnMoveFolder(cImgTmpPath, cImgQmsPath);
				}	
				m_fnDelLocalInspData(strPanelInfo2.cPanelID, nVisionCount) ;	


				//m_strDefGrade[PANEL_TWO]			= m_strDefGrade1;
				//m_strdefJudge[PANEL_TWO][JUDGE_ONE] = m_strdefJudge1; 
				//m_strdefJudge[PANEL_TWO][JUDGE_TWO] = m_strdefJudge2;
				//m_strCimMeasureData[PANEL_TWO] = m_strCimMeasureDataTemp;	//2016.01.15
			}

			m_bInspEnd = TRUE;


			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - End Data Merge "), nVisionCount, strPanelInfo1.cPanelID);

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - Start FTP Upload"), nVisionCount, strPanelInfo1.cPanelID);

			if(!pFrame->m_stParamInfo.bUseManualJudge || !bNgFlag && pFrame->m_stParamInfo.bUseManualJudge || ((bNgFlag == 5) && pFrame->m_stParamInfo.bUseManualJudge)) //2019.01.16 by ryu
			{
			/*	if (FtpDstClient == NULL)
			{
			delete FtpDstClient;
			FtpDstClient = new CFtpClient();

			}		*/
			FtpDstClient->m_fnFTPdUploadData();
			/*	if (FtpImgClient == NULL)
			{
			delete FtpImgClient;
			FtpImgClient = new CFtpClient();

			}*/
			FtpImgClient->m_fnFTPdUploadImg();		
			}
			nVisionCnt = 1;
			strTemp.Format(_T("%d"), nVisionCnt);

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
			//strResutJudgeThreadVal.Format(_T("%s,%s,%s,%s,%s,%s,%s,%s"), 
			//	strPanelInfo1.cPanelID, m_strDefGrade[PANEL_ONE], m_strdefJudge[PANEL_ONE][JUDGE_ONE], m_strdefJudge[PANEL_ONE][JUDGE_TWO], 
			//	strPanelInfo2.cPanelID ,m_strDefGrade[PANEL_TWO], m_strdefJudge[PANEL_TWO][JUDGE_ONE], m_strdefJudge[PANEL_TWO][JUDGE_TWO]) ;
			//
			//UINT uiThreadID;
			//pTreadCimProc->m_strTheadData = strResutJudgeThreadVal;
			//pTreadCimProc->m_hThreadCim = (HANDLE)::_beginthreadex(NULL, 0, pTreadCimProc->m_fnResultJudgeProc,(LPVOID) this, 0, &uiThreadID);
			//G_AddLog(3, 2, _T("[Vision : %d] [Panel Id : %s] Process QMS Data End!!"), nVisionCount, strPanelInfo1.cPanelID);
			strPanelInfo1.init_info();
			strPanelInfo2.init_info();
		}	
	}

	cPanelInfo1 = strPanelInfo1;
	cPanelInfo2 = strPanelInfo2;

	m_fnDeleteAllFolderThread(_T("D:\\QMS\\i_wait"));
	//2015.06.12 add by IKM - E
	///////////////////////////////////////////////////////////////////////////////////////////////
	G_AddLog(2, 3, _T("m_fnResultProcess() - End"));
	return nRet = OK;
}
#endif

int CDataManager::m_fnExistImage(STRU_CLASSIFY_INFO strPanelInfo) // 2016.03.12 add by sgkim
{
	G_AddLog(3, 3, _T("m_fnExistImage() - Start"));
	CFileFind		finder;
	BOOL			bWorking;
	int				curPos = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strDataPath;
	CString strDataDate;
	CString strQMSBackupPath = _T("D:\\DEFECT INFO\\QMS BACKUP SUM INFO");
	CString strQmsTmpPath = _T("D:\\QMS_IMG_TMP");
	CString strImgPath;
	CString token;
	FILE *out = NULL;
	TCHAR chRead[255] = {0};
	CArray<CString,CString&> strIMGName;


	CTime time = CTime::GetCurrentTime(); // 수정필요
	CString strTime = time.Format(_T("%m%d"));

	strDataPath.Format(_T("%s\\%s\\%s\\%s\\"),strQMSBackupPath,time.Format(_T("%Y")),time.Format(_T("%m")),time.Format(_T("%d")));
	bWorking = finder.FindFile(strDataPath +_T("\\*.*"));
	strDataPath = m_fnGetLastWriteFile(strPanelInfo.cPanelID, strDataPath); // 가장 최근에 쓰여진 tmp파일 읽어오기

	CStdioFile cstFile;
	BOOL bRet = FALSE;

	if (!cstFile.Open(strDataPath, CFile::modeRead | CFile::typeText, NULL)) // 파일 열기
	{
		G_AddLog(3, 2, _T("m_fnExistImage File Open Error %s : %d"), strDataPath, GetLastError());
		return FALSE;
	}


	CStringArray strArrayData;
	CString strLine, strItem, strNewLine;
	int nLineCount = 0;

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "" || nLineCount < 3)
		{
			nLineCount++;
			continue;
		}

		while((token = strLine.Tokenize(_T(","),curPos)) != "")
		{
			CString str(token);
			str.TrimRight();
			if(str.Right(1) == _T("g"))
				strIMGName.Add(str);

		}
		curPos = 0;

	}
	cstFile.Close(); // Data에 있는 QMS 이미지 이름 불러오기 끝

	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	CString strStepId = chRead;

	strImgPath.Format(_T("%s\\%s\\%s\\%s\\%s"),strQmsTmpPath,strPanelInfo.cDataDate,strStepId,strPanelInfo.cPanelID, strPanelInfo.cDataTime);
	//strImgPath.Format(_T("%s\\%s\\%s\\%s\\%s\\%s"),strQmsTmpPath,strPanelInfo.cDataDate,strStepId,strPanelInfo.cGlassID,strPanelInfo.cPanelID, strPanelInfo.cDataTime);//2017.06.29	Qms Img 경로 내 GlassID 추가

	bWorking = finder.FindFile(strImgPath +_T("\\*.*"));
	if( !bWorking )
	{
		return FALSE;
	}

	while(bWorking)
	{
		int nArrayCount = 0;
		bWorking = finder.FindNextFile();
		if(finder.IsDots())
		{
			continue;
		}
		CString a = finder.GetFileName();
		while(1)
		{
			if(nArrayCount < strIMGName.GetSize())
			{
				CString b = strIMGName.GetAt(nArrayCount);
				if( finder.GetFileName() != strIMGName.GetAt(nArrayCount))
					nArrayCount++;
				else
				{
					strIMGName.RemoveAt(nArrayCount);
					break;
				}
			}
			else
			{
				CString strDeleteFile = strImgPath;
				strDeleteFile += _T("\\");
				strDeleteFile += finder.GetFileName();
				DeleteFile(strDeleteFile);
				G_AddLog(3, 2, _T("m_fnExistimage : %s deleted , strDataPath : %s"), strDeleteFile,strDataPath);
				break;
			}
		}
		nArrayCount = 0;
	}
	G_AddLog(3, 3, _T("m_fnExistImage() - End"));
	return 0;
}

void CDataManager::m_fnftnRun()
{
	/*	if (FtpDstClient == NULL)
	{
	delete FtpDstClient;
	FtpDstClient = new CFtpClient();

	}		*/
	FtpDstClient->m_fnFTPdUploadData();
	FtpImgClient->m_fnFTPdUploadImg();
}
int CDataManager::m_fnResultProcessPacking(STRU_CLASSIFY_INFO &cPanelInfo1,STRU_CLASSIFY_INFO &cPanelInfo2,int nVisionCount)
{
	int nVisionCnt = 0 ,nEqpNo = 0, nRet = NG;
	int nTotalImgCnt = 0;
	CString strTemp = _T("");
	CString strQmsFilePath;
	CString cCompareVal;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	BOOL bQmsStartFlag =FALSE;
	CString strResutJudgeThreadVal;
	CString cImgTmpPath, cImgQmsPath;
	int nProcQms = 0, nQmsFinalCnt = 0;
	BOOL bNgFlag = FALSE;

	STRU_CLASSIFY_INFO strPanelInfo1;
	STRU_CLASSIFY_INFO strPanelInfo2;

	strPanelInfo1 = cPanelInfo1;
	strPanelInfo2 = cPanelInfo2;

	cCompareVal = strPanelInfo1.cPanelID;
	if (cCompareVal != (_T(""))) 
	{			 			
		_tcscpy(strPanelInfo1.cPanelPath  , m_fnReadDefectData(strPanelInfo1, nVisionCount));			
		pFrame->m_DataManager.m_strPanelPath[PANEL_ONE] = strPanelInfo1.cPanelPath ;
		// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.
		m_fnGetDefStartDate(strPanelInfo1.cPanelPath);				

		G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo1.cStartDate);

	}
	cCompareVal = strPanelInfo2.cPanelID ;
	if (cCompareVal != _T(""))
	{			
		_tcscpy(strPanelInfo2.cPanelPath  , m_fnReadDefectData(strPanelInfo2, nVisionCount));
		pFrame->m_DataManager.m_strPanelPath[PANEL_TWO] = strPanelInfo2.cPanelPath ;
		m_fnGetDefStartDate(strPanelInfo1.cPanelPath);	
		G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo2.cPanelID, strPanelInfo2.cStartDate);		
	}
	G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Write Defect Data!!"), nVisionCount, strPanelInfo1.cPanelID);

	pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);

	m_fnSetPanelId1(strPanelInfo1.cPanelID);
	m_fnSetPanelId2(strPanelInfo2.cPanelID);

	CString strRootPath = _T("");
	CString strPath = _T("");

	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	cCompareVal = strPanelInfo1.cPanelID;
	strRootPath.Format(_T("%s\\%s"),strRootPath,strPanelInfo1.cPanelID);
	if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_ONE] =  m_fnGetLastWriteFile(strPanelInfo1.cPanelID, strRootPath);

	cCompareVal = strPanelInfo2.cPanelID;
	strRootPath.Format(_T("%s\\%s"),strRootPath,strPanelInfo2.cPanelID);
	if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_TWO] =  m_fnGetLastWriteFile(strPanelInfo2.cPanelID, strRootPath);

	//// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.

	if (m_strPanelPath[PANEL_ONE].Compare(_T("")))
	{		
		cImgTmpPath.Format(_T("%s%s"), _T("D:\\QMS_IMG_TMP\\"), strPanelInfo1.cDataDate);
		cImgQmsPath = _T("D:\\DEFECT IMAGE\\i_send");
		//cImgQmsPath = _T("D:\\\DEFECT IMAGE");
		if ((CString)strPanelInfo1.cDataDate != _T(""))
		{
			m_fnMoveFolder(cImgTmpPath, cImgQmsPath);
		}

		m_strInspDate = strPanelInfo1.cStartDate;	m_strInspTime = strPanelInfo1.cStartTime;
		m_strSaveDate = strPanelInfo1.cDataDate;	m_strSaveTime = strPanelInfo1.cDataTime;


		//m_fnUpLoadImgFile(strPanelInfo1.cPanelID); // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
		//Sleep(500);
		//2016.02.01 Packing Defect
		//m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_ONE], strPanelInfo1.cPanelID, PANEL_ONE);
		m_fnMakePackingUpadateFile(m_strPanelPath[PANEL_ONE], strPanelInfo1.cPanelID, PANEL_ONE);
		//2016.02.01 Packing Defect					
		m_fnDelLocalInspData(strPanelInfo1.cPanelID, nVisionCount) ;  // 이미지 삭제 스레드			
		/*	m_strDefGrade[PANEL_ONE]			= m_strDefGrade1;
		m_strdefJudge[PANEL_ONE][JUDGE_ONE] = m_strdefJudge1; 
		m_strdefJudge[PANEL_ONE][JUDGE_TWO] = m_strdefJudge2;*/
	}
	if (m_strPanelPath[PANEL_TWO].Compare(_T("")))
	{
		cImgTmpPath.Format(_T("%s%s"), _T("D:\\QMS_IMG_TMP\\"), strPanelInfo2.cDataDate);
		cImgQmsPath = _T("D:\\DEFECT IMAGE\\i_send");
		//cImgQmsPath = _T("D:\\\DEFECT IMAGE");
		if ((CString)strPanelInfo2.cDataDate != _T(""))
		{
			m_fnMoveFolder(cImgTmpPath, cImgQmsPath);
		}
		m_strInspDate = strPanelInfo2.cStartDate;	m_strInspTime = strPanelInfo2.cStartTime;
		m_strSaveDate = strPanelInfo2.cDataDate;	m_strSaveTime = strPanelInfo2.cDataTime;


		//m_fnUpLoadImgFile(strPanelInfo2.cPanelID);  // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
		//Sleep(500);
		//2016.02.01 Packing Defect
		//m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_TWO], strPanelInfo2.cPanelID, PANEL_TWO);
		m_fnMakePackingUpadateFile(m_strPanelPath[PANEL_TWO], strPanelInfo2.cPanelID, PANEL_TWO);
		//2016.02.01 Packing Defect	
		m_fnDelLocalInspData(strPanelInfo2.cPanelID, nVisionCount) ;  // 이미지 삭제 스레드		
		/*	m_strDefGrade[PANEL_TWO]			= m_strDefGrade1;
		m_strdefJudge[PANEL_TWO][JUDGE_ONE] = m_strdefJudge1; 
		m_strdefJudge[PANEL_TWO][JUDGE_TWO] = m_strdefJudge2;*/
	}

	m_bInspEnd = TRUE;

	G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - End Data Merge "), nVisionCount, strPanelInfo1.cPanelID);

	strPanelInfo1.init_info();
	strPanelInfo2.init_info();

	cPanelInfo1 = strPanelInfo1;
	cPanelInfo2 = strPanelInfo2;

	//2015.06.12 add by IKM - E
	///////////////////////////////////////////////////////////////////////////////////////////////
	return nRet = OK;
}

int CDataManager::m_fnSetCimCheckList(CString strDefJudge)
{
	G_AddLog(3, 3, _T("m_fnSetCimCheckList() - Start"));
	int nRet, nChkNo = 0;
	int nDefJudge = 0;

	nDefJudge = _ttoi(strDefJudge);
	switch(nDefJudge)
	{
	case CONTACTFAIL: case ITORESISTILEGAL:
		nRet = (int)pow(double(2),double(32)); 
		break;
	case MURA:
		nRet = (int)pow(double(2),double(32)); 
		break;
	case CFPOLMEASUREFAIL:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case TFTPOLMEASUREFAIL:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFPOLSUBSTANCE:
		nChkNo = (int)pow(double(2),double(35)); 
		nRet = nChkNo;
		nChkNo = (int)pow(double(2),double(32)); 
		nRet += nChkNo;
		break;
	case TFTPOLBUBBLE:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case TFTPOLSCRATCH:
		nRet = (int)pow(double(2),double(33)); 
		break;
	case CFPOLBUBBLE:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFPOLSCRATCH:
		nRet = (int)pow(double(2),double(34)); 
		break;
	case CFSCRATCH:	case	TFTSCRATCH	:	case	TFTWHITEBLUR:	case	CFWHITEBLUR	:	
	case ITOPEEL :	case	WANESBROKEN	:	case	SOCKETSCRACH:	case	TFTDIMPLEPOINT:
	case CFDIMPLEPOINT:	case	TFTEDGEILEGAL:	case	CFEDGEILEGAL:	case	TFTDIMPLELINE:
	case CFDIMPLELINE : case	SOCKETBURN:	case	BURR: case	CFMEASUREFAIL:	case	TFTMEASUREFAIL:
	case CFBURR : case	TFTBURR:	case	CFBROKEN: case	TFTBROKEN:	case	CFCORNERBROKEN:		case	TFTCORNERBROKEN:	//2015.07.13
	case CFCELLBROKEN : case	TFTCELLBROKEN:	//2015.08.26
	case CFPIMPLEPOINT	: case TFTPIMPLEPOINT : case CFPIMPLELINE :	case TFTPIMPLELINE : case CFPIMPLECIRCLE :
	case CFWAVEPATTERN	: case ETCLINE		  : /*case ETCPOINT	  :		// Comment KYH01 정의 중복으로 빌드오류 임시 주석*/	
	case POLMISS	  :		case CFPROTECTION	: case TFTPROTECTION	:	//2017.04.12
		//2019.03.15
	case BGRADECHECK	:case CGRADECHECK	:
		nRet = (int)pow(double(2),double(1)); 
		break;
	default:
		nRet = 0;
	}
	G_AddLog(3, 3, _T("m_fnSetCimCheckList() - End"));
	return nRet;
}

BOOL CDataManager::m_fnDelLocalInspData(CString strPanelId, int nVisionCnt) 
{
	G_AddLog(3, 3, _T("m_fnDelLocalInspData() - Start"));

	m_treadDelPanelId = strPanelId;
	m_treadDelVisionId = nVisionCnt;

	UINT uiThreadID;
	m_hThreadDelInspData = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadDelInspDataProc, (LPVOID)this, 0, &uiThreadID);

	G_AddLog(3, 3, _T("m_fnDelLocalInspData() - End"));
	return 1;
}

#if THREAD_MODE
BOOL CDataManager::m_fnDelNoUseFile(CString strFilePath, CString strPanelID) 
{
	G_AddLog(3, 3, _T("m_fnDelNoUseFile() - Start"));

	m_treadDelPanelId  = strPanelID;
	m_treadDelPath     = strFilePath;

	UINT uiThreadID;
	m_hThreadDelNoUseDat = (HANDLE)::_beginthreadex(NULL, 0, m_fnThreadDelNoUseDatProc, (LPVOID)this, 0, &uiThreadID);
	//	}	
	G_AddLog(3, 3, _T("m_fnDelNoUseFile() - End"));
	return 1;
}

#else
CString CDataManager::m_fnDelNoUseFile(CString strFilePath, CString strPanelID)
{
	if (strFilePath.IsEmpty()) return FALSE;
	if (strPanelID.IsEmpty()) return FALSE;
	CFileFind finder;
	CString	 strTmp, strFileName, strVal, strPathTmp, strRetVal;
	CTime time;
	int curPos = 0;
	unsigned long size;
	BOOL bWorking;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	//2015.06.22
	SYSTEMTIME	systime;
	CString strDefectSummaryDataPath, strDate;
	int nDeleteTerm = 0;
	CString strQmsBackUpDataPath;	//2015.10.08

	::GetLocalTime(&systime);
	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);

	strDefectSummaryDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetDefectSummaryDataPath();
	strQmsBackUpDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();	//2015.10.08
	pFrame->m_fnCreateFolder(strDefectSummaryDataPath + strDate);

	strRetVal.Format(_T("%s%s%s.txt"),strDefectSummaryDataPath, strDate, strPanelID );
	CopyFile(strFilePath, strRetVal, 0);

	nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
	m_fnDeleteDefectFile(strDefectSummaryDataPath, nDeleteTerm);
	m_fnDeleteDefectFile(strQmsBackUpDataPath, nDeleteTerm);	//2015.10.08
	//2015.06.22

	strTmp = strFilePath.Left( strFilePath.ReverseFind( '\\' ));

	strFileName = strFilePath.Right( strFilePath.ReverseFind( '\\' ));
	strFileName.Mid(2, strFileName.GetLength());
	strFileName  = strFileName.Tokenize(_T("\\"), curPos);
	//strPathTmp = strFilePath;
	strPathTmp =  strTmp  + _T("\\*.*");
	strFilePath = strTmp + _T("\\");

	bWorking = finder.FindFile(strPathTmp);	// 모든화일

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();	// 화일하나를 찾음

		if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

		// 화일이름
		strVal = finder.GetFileName();	// 화일이름 얻기

		if (strVal.Find(strPanelID) != -1)
		{
			//2015.08.24 for TEST by IKM
#if 1
			DeleteFile(strFilePath + strVal);
#else
			MoveFile(strFilePath + strVal,  _T("D:\\TEMP_TEST\\") + strVal );
#endif
			//DeleteFile(strFilePath + strVal);

		}
	}

	int nVisionCnt = 0;

	//for (nVisionCnt = 1 ; nVisionCnt < pFrame->m_stParamInfo.nCountVisionClient + 1 ; nVisionCnt ++ )
	//{		
	//	strFilePath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);
	//	strPathTmp = strFilePath;
	//	strPathTmp.Format(_T("%sVisionClient-%d\\"),strPathTmp, nVisionCnt);
	//	strTmp.Format(_T("%sVisionClient-%d\\*.*"), strFilePath, nVisionCnt);
	//
	//	bWorking = finder.FindFile(strTmp);	// 모든화일

	//	while(bWorking == TRUE)
	//	{
	//		bWorking = finder.FindNextFile();	// 화일하나를 찾음

	//		if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감
	//		
	//		// 화일이름
	//		strVal = finder.GetFileName();	// 화일이름 얻기

	//		if (!strVal.Find(strPanelID) && !strVal.Find(_T(".txt")) )
	//		{
	//			DeleteFile(strPathTmp + strVal);
	//			
	//		}
	//		
	//	}
	//}
	//strPathTmp.Format(_T("%s\%s\\"),strFilePath, strPanelID);
	//RemoveDirectory(strPathTmp);

	return strRetVal;
}
#endif // THREAD_MODE

unsigned __stdcall  CDataManager::m_fnThreadDelNoUseDatProc(LPVOID pArg /*CString strFilePath, CString strPanelID*/)
{
	G_AddLog(3, 3, _T("m_fnThreadDelNoUseDatProc() - Start"));
	BOOL rc = FALSE;
	CDataManager *pDdat = (CDataManager *)pArg;	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CString strFilePath,strPanelID ;

	strFilePath = pDdat->m_treadDelPath;
	strPanelID = pDdat->m_treadDelPanelId;

	if (strFilePath.IsEmpty())
	{
		if (pDdat->m_hThreadDelNoUseDat)
		{
			CloseHandle(pDdat->m_hThreadDelNoUseDat);
			pDdat->m_hThreadDelNoUseDat = NULL;
		}
		G_AddLog(3, 3, _T("m_fnThreadDelNoUseDatProc() - End"));
		return FALSE;
	}
	if (strPanelID.IsEmpty())
	{
		if (pDdat->m_hThreadDelNoUseDat)
		{
			CloseHandle(pDdat->m_hThreadDelNoUseDat);
			pDdat->m_hThreadDelNoUseDat = NULL;
		}
		G_AddLog(3, 3, _T("m_fnThreadDelNoUseDatProc() - End"));
		return FALSE;
	}

	CFileFind finder;
	CString	 strTmp, strFileName, strVal, strPathTmp, strRetVal;
	CTime time;
	int curPos = 0;

	SYSTEMTIME	systime;
	CString strDefectSummaryDataPath, strDate;
	int nDeleteTerm = 0;
	CString strQmsBackUpDataPath;	//2015.10.08

	try
	{
		::GetLocalTime(&systime);
		strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);

		strDefectSummaryDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetDefectSummaryDataPath();
		strQmsBackUpDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();	//2015.10.08
		pFrame->m_fnCreateFolder(strDefectSummaryDataPath + strDate);

		strRetVal.Format(_T("%s%s%s.txt"),strDefectSummaryDataPath, strDate, strPanelID );
		CopyFile(strFilePath, strRetVal, 0);
		
		strRetVal = pDdat->m_fnLastToken(strFilePath, _T("\\"));
		strFilePath = strFilePath.Left(strFilePath.GetLength() - strRetVal.GetLength());
		RemoveDirectory(strFilePath);
		pDdat->m_fnDeleteFolder(strFilePath);

		nDeleteTerm = pFrame->m_iniConfigSet.GetProfileIntW(PNL_DATA_PATH, KEY_DEF_INFO_DELETE, 0);
		pDdat->m_fnDeleteDefectFile(strDefectSummaryDataPath, nDeleteTerm);
		pDdat->m_fnDeleteDefectFile(strQmsBackUpDataPath, nDeleteTerm);	//2015.10.08





	}	
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] [Path : %s] m_fnThreadDelNoUseDatProc Failed !! - CFileException "),pDdat->m_treadDelPanelId, pDdat->m_treadDelPath);
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] [Path : %s] m_fnThreadDelNoUseDatProc Failed !! - CException "),pDdat->m_treadDelPanelId, pDdat->m_treadDelPath);
		e->Delete();
	}
	if (pDdat->m_hThreadDelNoUseDat)
	{
		CloseHandle(pDdat->m_hThreadDelNoUseDat);
		pDdat->m_hThreadDelNoUseDat = NULL;
	}
	G_AddLog(3, 3, _T("m_fnThreadDelNoUseDatProc() - End"));
	return TRUE;
}


unsigned __stdcall  CDataManager::m_fnThreadDelInspDataProc(LPVOID pArg )
{	
	G_AddLog(3, 3, _T("m_fnThreadDelInspDataProc() - Start"));
	BOOL rc = FALSE;
	CDataManager *pDmg = (CDataManager *)pArg;	
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strPathName, strCmpFileName, strFileName;
	CString /*strImgFilePath = _T(""),*/ strRootPath = _T(""), strPath = _T("");
	BOOL bWorking;
	int nVisionCnt = 1;
	CFileFind finder;	

	CString strPanelId , strtemp, resToken;	
	int curPos;

	//strPanelId = pDmg->GetPanelId();
	strPanelId = pDmg->m_treadDelPanelId;
	strtemp = strPanelId;
	curPos = 0;

	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	try
	{
		while( nVisionCnt <= pFrame->m_stParamInfo.nCountVisionClient )
		{

			strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, nVisionCnt/*pDmg->m_nVisionCnt*/);

			bWorking = finder.FindFile(strPath + _T("*.*"));	// 모든 파일

			while(bWorking == TRUE)
			{
				bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

				if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

				strFileName = finder.GetFileName();	// 파일 이름 얻기			

				if ( (strFileName.Find(strPanelId) != -1 && strFileName.Find(_T(".txt")) != -1)
					|| (strFileName.Find(strPanelId) != -1 && strFileName.Find(_T(".jpg")) != -1))	//2016.07.14 DefectImageFile			
					DeleteFile(strPath + strFileName);				
				if (bWorking == FALSE) break;
			}
			nVisionCnt ++;
		}
		CloseHandle(pDmg->m_hThreadDelInspData);
		pDmg->m_hThreadDelInspData = NULL;
		finder.Close();

	}

	catch (CFileException* e)
	{
		CloseHandle(pDmg->m_hThreadDelInspData);
		pDmg->m_hThreadDelInspData = NULL;
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadDelmgProc Failed !! - CFileException "), pDmg->m_treadDelPanelId);
		e->Delete();
	}
	catch (CException* e)
	{
		CloseHandle(pDmg->m_hThreadDelInspData);
		pDmg->m_hThreadDelInspData = NULL;
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnThreadDelmgProc Failed !! - CException "), pDmg->m_treadDelPanelId);
		e->Delete();
	}
	G_AddLog(3, 3, _T("m_fnThreadDelInspDataProc() - End"));
	return TRUE;
	//return rc;
}



void CDataManager::SetPanelId(CString strPanelId)
{
	m_strPanelId = strPanelId;
}

CString CDataManager::GetPanelId(){

	return m_strPanelId;
}

CString CDataManager::m_fnGetDefStartDate(CString strFilePath)//(CString strFilePath, CString strQmsFilePath)
{
	G_AddLog(3, 3, _T("m_fnGetDefStartDate() - Start"));
	BOOL bRetVal = FALSE;
	CStdioFile cstFile;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\PanelID1_V1_00.txt"), CFile::modeRead | CFile::typeText, NULL))	
	{
		G_AddLog(3, 2, _T("m_fnGetDefStartDate QMS File Open Error %s : %d"), strFilePath , GetLastError());
		G_AddLog(3, 3, _T("m_fnGetDefStartDate() - End"));
		return FALSE;
	}

	m_stDefType.initDefType();
	m_stDefType_UI.initDefType();	//2019.01.17 add by mby

	CStringArray strArrayData;
	CString strLine, strItem, strNewLine ,strTemp;
	CString strLineHeader,strDate, strTime;
	CString strtemp, strDefNo, resToken, cImgtemp, cFileName;	
	int nImgNameCnt =0 ,nLoopCnt = 0, nDefNo = 0;

	CString check_date= _T("");
	CString strVal = _T("");	
	int nDefCnt = 0;
	int curPos = 0 , nAlignToMarkCnt = 0 , nLumitCnt;
	CString strCnt ;
	nAlignToMarkCnt = 1;
	nLumitCnt = 1;
	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("HEADERDATA"))
		{
			//strLineHeader = m_fnMakeQmsDataHead(strLine);
			m_strSaveDate = m_fnGetItemData(4, strLine);
			m_strSaveTime = m_fnGetItemData(5, strLine);

			m_strInspDate = m_strSaveDate;
			m_strInspTime = m_strSaveTime;

			curPos = 0;	nLoopCnt =0; nImgNameCnt = 0;
			resToken= m_strSaveDate.Tokenize(_T("/"),curPos);
			strDate = _T("");
			while (resToken != "")
			{
				strDate.Append(resToken);
				if (curPos >= m_strSaveDate.GetLength())	break;
				resToken= m_strSaveDate.Tokenize(_T("/"),curPos);
				//m_strSaveDate.Append(_T("_"));					// Add By KYH01 Merge 데이터 i_send경로 날짜 "_" 추가
				nLoopCnt++;
			}
			m_strSaveDate = strDate;
			curPos = 0;	nLoopCnt =0; nImgNameCnt = 0;
			resToken= m_strSaveTime.Tokenize(_T(":"),curPos);
			strTime = _T("");
			while (resToken != "")
			{
				strTime.Append(resToken);
				if (curPos >= m_strSaveTime.GetLength())	break;
				resToken= m_strSaveTime.Tokenize(_T(":"),curPos);
				//m_strSaveTime.Append(_T("_"));					// Add By KYH01 Merge 데이터 i_send경로 시간 "_" 추가
				nLoopCnt++;
			}

			m_strSaveTime = strTime;
		}	
	}
	cstFile.Close();
	G_AddLog(3, 3, _T("m_fnGetDefStartDate() - End"));
	return strTime;
}

CString CDataManager::m_fnSetDefStartDate(CString strFilePath)//(CString strFilePath, CString strQmsFilePath)
{
	G_AddLog(3, 3, _T("m_fnSetDefStartDate() - Start"));
	BOOL bRetVal = FALSE;
	CStdioFile cstFile;


	CStringArray strArrayData;
	CString strLine, strItem, strNewLine ,strTemp;
	CString strLineHeader,strDate, strTime;
	CString strtemp, strDefNo, resToken, cImgtemp, cFileName;	
	int nImgNameCnt =0 ,nLoopCnt = 0, nDefNo = 0;

	CString check_date= _T("");
	CString strVal = _T("");	
	int nDefCnt = 0;
	int curPos = 0 , nAlignToMarkCnt = 0 , nLumitCnt;
	CString strCnt ;
	nAlignToMarkCnt = 1;
	nLumitCnt = 1;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\PanelID1_V1_00.txt"), CFile::modeRead | CFile::typeText, NULL))	
	{
		G_AddLog(3, 2, _T("m_fnSetDefStartDate QMS File Open Error %s : %d"), strFilePath , GetLastError());
		G_AddLog(3, 3, _T("m_fnSetDefStartDate() - End"));
		return FALSE;
	}

	//m_stDefType.initDefType();

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("HEADERDATA"))
		{
			//cstFile.Remove(_T(""));
			//if(nClsEndCnt == 1){
			//	strLine = m_fnSetItemData(3, strLine, m_strInspDate, LEN_DEFECT_NO, D_ITEM_END - 1);
			//	strLine = m_fnSetItemData(4, strLine, m_strInspTime, LEN_DEFECT_NO, D_ITEM_END - 1);
			//}

			strLine = m_fnSetItemData(5, strLine, m_strInspDate, LEN_DEFECT_NO, D_ITEM_END - 1);
			strLine = m_fnSetItemData(6, strLine, m_strInspTime, LEN_DEFECT_NO, D_ITEM_END - 1);			
		}

		strArrayData.Add(strLine);		
	}
	cstFile.Close();

	if (!cstFile.Open(strFilePath, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnSetDefStartDate File Open Error %s : %d"), strFilePath, GetLastError());
		G_AddLog(3, 3, _T("m_fnSetDefStartDate() - End"));
		return FALSE;
	}

	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i) + _T("\n");
		cstFile.WriteString(strLine);
	}

	cstFile.Close();
	G_AddLog(3, 3, _T("m_fnSetDefStartDate() - End"));
	return strTime;
}
CString CDataManager::m_fnReadDefectData(STRU_CLASSIFY_INFO &stPanelInfo, int nVisionCnt)
{
	G_AddLog(3, 3, _T("m_fnReadDefectData() - Start"));
	CString		strRootPath, strPathV1, strPathV2, strPathV3, strPath, strTime, strQmsDate, strQmsTime;
	CFileFind   finder;
	CString		strRetVal,strTemp;
	CString		strDefectPath[MAX_VISION_PC + 1];
	CString		strPanelID = stPanelInfo.cPanelID;
	int			nRetryCnt = 0, nFirstCnt = 0;
	BOOL		bExistFlag, bResult;
	CString		strStartDate, strStartTime, strSaveDate, strSaveTime;

	bExistFlag = FALSE;

	if (!strPanelID.Compare(_T("")))
	{
		return strRetVal = _T("");
	}

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		

	CTime time = CTime::GetCurrentTime();
	strTime = time.Format(_T("%Y%m%d_%H%M%S"));

	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);
	strPath.Format(_T("%s\\%s"), strRootPath, strPanelID);
	CreateDirectory(strPath, NULL);

	m_fnResetArray();

	int nFileCnt = 0;
	int nExistCnt = 0;
	int bUseSemaphore = 0;
	CString strTmp = _T("");

	for (int i = 1 ; i< MAX_VISION_PC ;i++) // 투입 된 Panel의 검사가 모두 끝났는지 체크
	{
		if (stPanelInfo.bComplete[i] == TRUE && i != SIGNAL_PC) nFirstCnt++;
	}

	if(stPanelInfo.nOrderCnt <= pFrame->m_stParamInfo.nCountVisionClient)
	{
		strPath.Format(_T("%s%s"), strRootPath, strPanelID);

		if (GetFileAttributes(strPath) == -1)
		{
			CreateDirectory(strPath, NULL);
		}	

		strPath.Format(_T("%s%s\\%s_V%d.txt"), strRootPath, strPanelID, strPanelID, nVisionCnt);
		nFileCnt = 1;
		while(nFileCnt <= MAX_INSPECTION_SECTION + 1)
		{
			strDefectPath[0].Format(_T("%sVisionClient-%d\\%s_V%d_0%d.txt"), strRootPath, nVisionCnt, strPanelID, nVisionCnt, nFileCnt - 1);
			if (finder.FindFile(strDefectPath[0]))
			{	
				G_AddLog(3, 2, _T("m_fnReadDefectData Find Path : %s"), strDefectPath[0]);
				bExistFlag = TRUE;

				strTmp = strDefectPath[0];				
				nExistCnt ++;
				nRetryCnt = 0;
				break;
			}

			if (nRetryCnt < 3 && bExistFlag != TRUE)
			{				
				nRetryCnt++;				
				Sleep(10);
				if (nRetryCnt == 3) nRetryCnt = 0;
				else continue;
			}
			nFileCnt++;
		}

		if (nExistCnt)
		{
			//	DeleteFile(strPath);

			CopyFile(strTmp, strPath,0);		// 데이터 파일 복사
			if (nFirstCnt == 1)
			{
				m_fnGetDefStartDate(strPath);				
				_tcscpy(stPanelInfo.cStartDate  , m_strInspDate);		_tcscpy(stPanelInfo.cStartTime  , m_strInspTime);		
				_tcscpy(stPanelInfo.cDataDate  , m_strSaveDate);		_tcscpy(stPanelInfo.cDataTime   , m_strSaveTime);		
			}
			else
			{
				m_strInspDate = stPanelInfo.cStartDate;					m_strInspTime = stPanelInfo.cStartTime;
				m_strSaveDate = stPanelInfo.cDataDate;					m_strSaveTime = stPanelInfo.cDataTime;
			}
		}
		else
		{
			//2017.03.07
			if (nFirstCnt == 1)
			{
				strStartDate = stPanelInfo.cStartDate;	strStartTime = stPanelInfo.cStartTime;
				strSaveDate = stPanelInfo.cDataDate;	strSaveTime = stPanelInfo.cDataTime;
				if (strStartDate == _T("") || strStartTime  == _T("") 
					|| strSaveDate == _T("") || strSaveTime  == _T(""))
				{
					strQmsDate = time.Format(_T("%Y/%m/%d"));	m_strInspDate = strQmsDate;	m_strSaveDate = strQmsDate;
					strQmsTime = time.Format(_T("%H:%M:%S"));	m_strInspTime = strQmsTime;	m_strSaveTime = strQmsTime;
					_tcscpy(stPanelInfo.cStartDate  , m_strInspDate);		_tcscpy(stPanelInfo.cStartTime  , m_strInspTime);		
					_tcscpy(stPanelInfo.cDataDate  , m_strSaveDate);		_tcscpy(stPanelInfo.cDataTime   , m_strSaveTime);

					G_AddLog(3, 2, _T("m_fnReadDefectData Start Time Default Set : %s"), stPanelInfo.cPanelID);
				}
			}

			G_AddLog(2, 3, _T("m_fnReadDefectData do not Find Path : %s"), strDefectPath[0]);
			G_AddLog_ALARM(_T("m_fnReadDefectData : %d"), ERROR_READ_DEFECT_DATA);
		}

		for (int i = VISION_PC_1; i <= pFrame->m_stParamInfo.nCountVisionClient; i ++ )
		{			
			strDefectPath[i].Format(_T("%s%s\\%s_V%d.txt"), strRootPath, strPanelID, strPanelID, (i));			
		}

		strPath.Format(_T("%s%s\\%s_%s.txt"), strRootPath, strPanelID, strPanelID, _T("MERGE"));
		DeleteFile(strPath);	

		int nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

 		if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT && (nEqpNo == 1 && stPanelInfo.nOrderCnt >= pFrame->m_stParamInfo.nCountVisionClient ))
 			m_fnDeleteTdiOverlapDefect(strPanelID);  
		//add check tdi test ejw-


		int nRetDefVal = 0;
		for (int i = VISION_PC_1; i <= pFrame->m_stParamInfo.nCountVisionClient; i ++ )
		{
			bResult = finder.FindFile(strDefectPath[i]);

			if (finder.FindFile(strPath) == OK)	
				strDefectPath[i] = strPath;
			nRetDefVal = m_fnDefectFileMerge(stPanelInfo.cPanelID, strDefectPath[i], strDefectPath[i+1], strPath, nVisionCnt);
		}
	}
	G_AddLog(3, 3, _T("m_fnReadDefectData() - End"));
	return strPath;
}
//#endif

BOOL CDataManager::m_fnMakeQmsUpadateFile(CString strFilePath, CString strPanelID, int PanelCnt)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile() - Start"));
	BOOL bRetVal = FALSE, bNgFlag = APP_OK, bLowerEqpBypass = FALSE;
	CString strRetVal, strTime, strVal, strQmsWaitPath;
	long lRet = -1;
	int nStepCnt = 0;
	int nSeqNoCnt = 0;
	CTime time = CTime::GetCurrentTime();
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CString sTempTst, strPanelPath;
	TCHAR chRead[255] = {0};
	memset(chRead,0X00,sizeof(chRead));		
	//sprintf((char *)chRead, "%d",ipCnt);	
	//2015.10.08
	SYSTEMTIME	systime;
	
	//2015.10.08
	//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnInitState();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	STRU_NG_FLAG_INFO stNgDataTemp;
	m_strDefGrade1 = _T("");
	m_strdefJudge1 = _T("");
	m_strdefJudge2 = _T("");

	strVal.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
	m_iniConfig.SetFileName(strVal);
	//m_iniConfig.GetProfileStringW(EQP_STATE, KEY_EQP_TYPE, _T(""), (LPTSTR)chRead, 255);
	//strVal = _T("U_");
	//strVal += chRead;

	//strRetVal = strVal;

	//memset(chRead,0X00,sizeof(chRead));		
	//nSeqNoCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_SEQ_NO, 0, m_cCountRoot);

	//strVal.Format(_T("%.4d"), nSeqNoCnt);		// Nanjing
	////strVal.Format(_T("%s"),strPanelID);			// ChangDu

	//strVal = _T("_") + strVal;
	//strRetVal +=  strVal;

	//nSeqNoCnt ++;

	//if (nSeqNoCnt > 9999)	nSeqNoCnt = 1;

	//strVal.Format(_T("%04d"), nSeqNoCnt);
	//WritePrivateProfileString(PNL_COUNT_SET, KEY_SEQ_NO, strVal, m_cCountRoot);

	//strRetVal += EXTENSION_TMP;

	//	strQmsWaitPath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strRetVal;	
	m_fnDccInfoGet();	//2019.12.16 add by mby
	m_fnGradeRankInfoGet();	//2020.07.24 add by mby

	int nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	if (nEqpModel)
	{
		bNgFlag = m_fnMakeQmsData_ABI(strFilePath, strQmsWaitPath, bLowerEqpBypass, PanelCnt);	

		if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)	
		{
			if (bNgFlag)
			{
				m_fnNgJudgelAlarm(_T("NG Panel List!!"), strPanelID);
			}
			else
			{
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PLC_NG_JUDGE, 0);
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PC_NG_FLAG2, 0);
			}
			// 2015.12.14 Add by CWH for Packing wait sequence
			BOOL bUseNgFlag = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0);
			if (bUseNgFlag)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 1);
				G_AddLog(3, 3, _T("SEQ_F_RESULT_PROCESS - USE NG FLAG."));
			}
		}
		G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile STEP : %d Panel ID[%s] NG Flag Set = %d"), ++nStepCnt, strPanelID, bNgFlag);
	}
	else
	{			
		G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile STEP : %d Panel ID[%s] Make Qms Data AVL"),++nStepCnt, strPanelID);

		pAuto->m_fnInitGradeSet();	//2020.10.13 add by mby
		bNgFlag = m_fnMakeQmsData_AVL(strFilePath, strQmsWaitPath, bLowerEqpBypass);	

		if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)	
		{
			if (bNgFlag)
			{
				m_fnNgJudgelAlarm(_T("NG Panel List!!"), strPanelID);
			}
			else
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PLC_NG_JUDGE, 0);
			// 2015.12.14 Add by CWH for Packing wait sequence
			BOOL bUseNgFlag = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0);
			if (bUseNgFlag)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 1);
				G_AddLog(3, 2, _T("SEQ_F_RESULT_PROCESS - USE NG FLAG."));
			}
		}
		G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile STEP : %d Panel ID[%s] NG Flag Set = %d"),++nStepCnt, strPanelID, bNgFlag);

	}

	::GetLocalTime(&systime);

	CString strDate = _T("");
	CString strQmsBackUpDataPath = _T("");
	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);
	
#if THREAD_MODE
	bRetVal = m_fnDelNoUseFile(strFilePath, strPanelID);

	CString strDefectSummaryDataPath = _T("");
	int nDeleteTerm = 0;
// 	::GetLocalTime(&systime);
// 	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);

	strDefectSummaryDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetDefectSummaryDataPath();
	strQmsBackUpDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();	//2015.10.08
	pFrame->m_fnCreateFolder(strDefectSummaryDataPath + strDate);

	strRetVal.Format(_T("%s%s%s.txt"),strDefectSummaryDataPath, strDate, strPanelID );
	bRetVal = CopyFile(strFilePath, strRetVal, 0);


	if (bRetVal)
	{
		CopyFile(strRetVal, strRetVal + _T(".tmp"), 0);
		if(!pFrame->m_stParamInfo.bUseManualJudge || !bNgFlag && pFrame->m_stParamInfo.bUseManualJudge || ((bNgFlag == 5) && pFrame->m_stParamInfo.bUseManualJudge))  //2019.01.19 by ryu
			pFrame->m_fnUpdateGridDefectSummary(strRetVal);	
	}

	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE)
	{
		m_fnSearchOverlapCoord(strRetVal,strPanelID); 
	}

	//2016.03.02
	if(pFrame->m_stOverlapLuminanceInfo->bUseMode == TRUE)
	{
		m_fnSearchOverlapLuminance(strRetVal + _T(".tmp"), strPanelID); 
	}
	DeleteFile(strRetVal + _T(".tmp"));

	G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile STEP : %d Panel ID[%s] Update Defect List"),++nStepCnt, strPanelID);
#else
	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE)
	{
		strPanelPath = m_fnDelNoUseFile(strFilePath, strPanelID);
		m_fnSearchOverlapCoord(strPanelPath); 	// 미사용 파일 삭제
	}
	else
		m_fnDelNoUseFile(strFilePath, strPanelID);

	strRetVal = m_fnLastToken(strFilePath, _T("\\"));
	strFilePath = strFilePath.Left(strFilePath.GetLength() - strRetVal.GetLength());
	RemoveDirectory(strFilePath);

#endif // THREAD_MODE
	CString strQmsBackUpFile = _T("");
	//2015.10.08
// 	::GetLocalTime(&systime);
// 	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);
	strTime = time.Format(_T("%Y%m%d%H%M%S"));
	strQmsBackUpDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();
	pFrame->m_fnCreateFolder(strQmsBackUpDataPath + strDate);
	strQmsBackUpFile.Format(_T("%s_%s%s"),strTime, strPanelID, EXTENSION_TMP);
	CopyFile(strQmsWaitPath, strQmsBackUpDataPath + strDate + strQmsBackUpFile, 0);	
	//2015.10.08

	//2021.01.13 add by mby for A,B,C all check
	//if(!bNgFlag){ //2020.01.06 PA가 아닐경우만 사용
		m_fnDccCheckAlarm();//2019.12.16 add by mby
		m_fnDccInfoSave();	//2019.12.16 add by mby
	//}
	if(!pFrame->m_stParamInfo.bUseManualJudge || (!bNgFlag && pFrame->m_stParamInfo.bUseManualJudge) || ((bNgFlag == 5) && pFrame->m_stParamInfo.bUseManualJudge))
	{
		//////////////////////////////////////////////////////////////////////////////////
		///////////////////////// NG FLAG 처리 부 ////////////////////////////////////////
		stNgDataTemp.init_info();

		_tcscpy(stNgDataTemp.cPanelID, strPanelID);		
		_tcscpy(stNgDataTemp.cGrade,  m_strDefGrade1);	
		_tcscpy(stNgDataTemp.cJudge1, m_strdefJudge1);	
		_tcscpy(stNgDataTemp.cJudge2, m_strdefJudge2);	
		_tcscpy(stNgDataTemp.cCimMeasureData, m_strCimMeasureDataTemp);
		stNgDataTemp.nNgFlag = bNgFlag;	

		//2016.05.28	설정 Defect에 따라 Lower Eqp Bypass NGFlag Bit On
		if( bLowerEqpBypass )
		{
			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				stNgDataTemp.bLowerEqpBypass = TRUE;
		}

		if (m_strDefGrade1 == _T(""))	stNgDataTemp.DefaultData(strPanelID, APP_NG);

		m_fnWriteNgFlagList(stNgDataTemp);

		stNgDataTemp.init_info();

		m_strPolPositionEQ_CV2  = _T("");
		m_strPolPositionStage_CV2 = _T("");
		//////////////////////////////////////////////////////////////////////////////////
	}
	else
	{
		G_AddLog(2, 0,_T("ManualJudge PanelID : %s"), strPanelID);
		pFrame->m_bMJudgeFlag = TRUE;		//2019.01.03 add by mby
		//수동 판정 메세지창
		CString strSendMsg = _T("");
		strSendMsg.Append(_T("#"));
		strSendMsg.Append(strPanelID);
		strSendMsg.Append(_T("#"));
		strSendMsg.Append(strRetVal);		//내부용 Defect File
		strSendMsg.Append(_T("#"));
		strSendMsg.Append(strQmsWaitPath);	//임시 저장 Defect File
		strSendMsg.Append(_T("#"));
		va_list vaList;
		va_start(vaList, strSendMsg);
		int len = _vscwprintf( strSendMsg, vaList ) + 1;
		wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
		wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
		memset(cBuffer,0,sizeof(wchar_t)*len);
		memset(cTempBuff,0,sizeof(wchar_t)*len);

		if (cBuffer)
		{
			wcscpy((WCHAR*)cBuffer, strSendMsg);
			memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);		
			pAuto->SendMessage(WM_UI_NEW_MANUAL_JUDGE_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff);		
			delete[] cBuffer;
		}
	}

	G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile() - End"));
	bRetVal = TRUE;
	return bNgFlag;	//bRetVal -> bNgFlag
}

int CDataManager::m_fnUpLoadImgFile(CString strPanelId)
{
	G_AddLog(3, 3, _T("m_fnUpLoadImgFile() - Start"));
	CString strPathName, strCmpFileName, strFileName ,strQmsRootPath, strCntImgeName, strVal, strTmp;
	CString strImgFilePath = _T(""), strRootPath = _T(""), strPath = _T("");
	BOOL bWorking;
	int nVisionCnt = 1, nTotalImgCnt = 1, nImgCnt = 0;
	BOOL brc;
	CFileFind finder;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CString strCompVal;
	CString strtemp, resToken;	
	int curPos;
	curPos = 0;
	int nImgRetCnt = 0;
	CString sTempTst;
	CTime time = CTime::GetCurrentTime();
	STRU_IMAGE_INFO strQmsImgInfo;
	strQmsImgInfo.init();

	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	strImgFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iWait;
	nTotalImgCnt = 0;

	sTempTst.Format(QMS_IMAG_TEMP ,strPanelId);
	pFrame->m_fnCreateFolder(sTempTst);

	while( nVisionCnt <= pFrame->m_stParamInfo.nCountVisionClient )
	{
		//strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, stClsInfo.nVisionNo[nVisionCnt]);
		strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, nVisionCnt);
		bWorking = finder.FindFile(strPath + _T("*.*"));	// 모든 파일

		nImgCnt = 0;//nTotalImgCnt;
		while(bWorking == TRUE)
		{
			bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

			if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

			strFileName = finder.GetFileName();	// 파일 이름 얻기			

			if (strFileName.Find(EXTENSION_JPG) != -1)
			{
				strCompVal = strPanelId ;
				if (strCompVal == _T(""))
				{
					strPanelId =  _T(" ");						
				}
				strCompVal = strPanelId ;
				if ( strFileName.Find(strCompVal) != -1)
				{	
					m_fnRenameQmsImgFile( strQmsImgInfo, strFileName, nTotalImgCnt);
					//strQmsRootPath = (pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iUnsent);
					strImgFilePath = m_fnLastToken(strQmsImgInfo.tImgPath, _T("\\"));
					strQmsRootPath = strQmsImgInfo.tImgPath;
					strQmsRootPath = strQmsRootPath.Left(strQmsRootPath.GetLength() - strImgFilePath.GetLength());

					strCmpFileName = m_fnLastToken(strFileName, _T("_"));
					strCntImgeName = strCmpFileName;
					strTmp = m_fnLastToken(strCntImgeName, _T("."));
					strTmp = strCntImgeName.Left(strCntImgeName.GetLength() - strTmp.GetLength() - 1);

					strCmpFileName = strFileName.Left(strFileName.GetLength() - strCmpFileName.GetLength());
					strCntImgeName.Format(_T("%s%04d.jpg"),strCmpFileName, _ttoi(strTmp) + nTotalImgCnt) ;

					sTempTst.Format(QMS_IMAG_TEMP _T("%s"), strPanelId,strCntImgeName);
					brc = MoveFile(strPath + strFileName, sTempTst);
					nImgCnt++;
				}
			}
			if (bWorking == FALSE) break;
		}

		nTotalImgCnt += nImgCnt;
		nVisionCnt ++;
	}
	finder.Close();
	G_AddLog(3, 3, _T("m_fnUpLoadImgFile() - End"));
	return nImgCnt;
}

BOOL CDataManager::m_fnRenameQmsImgFile(STRU_IMAGE_INFO& struImgInfo, CString strLine, int nDefNo)
{
	G_AddLog(3, 3, _T("m_fnRenameQmsImgFile() - Start"));
	CString strRetVal,strStepId = _T(""), strPanelID = _T(""), strGlassID = _T("");
	CString strtemp, strDefNo, resToken, strRootPath = _T("");	

	int nImgCnt =0 ,nDefType = 0;

	TCHAR chRead[255] = {0};

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	strRootPath.Append(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iWait);

	//lRet = pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, strStepId);
	//lRet = pFrame->m_fnGetDeviceBlock(ADDR_PROC_INFO_CIM_STEPID, 20, stStepID);

	memset(chRead,0X00,sizeof(chRead));	
	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);

	strStepId  = chRead;

	if (strStepId == _T(""))
	{
		if(pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0)==0)
			strStepId=_T("51400");					// AVL StepNo.
		else if(pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0)==1)
			strStepId=_T("61600");					// ABI StepNo.
	}

	strPanelID =  struImgInfo.tImgPnlID;
	//strGlassID =	struImgInfo.tGlassID;	//2017.06.29	Qms Img 경로 내 GlassID 추가


	//Comment KYH01
	//2018.01.16

	CString			strYear, strMonth, strDay, strHour, strMin, strSec;
	CString			strDate, strTime;
	SYSTEMTIME		time;

	strtemp = m_strSaveDate;	//check time
	strYear = strtemp.Mid(0, 4);
	strMonth = strtemp.Mid(4, 2);
	strDay = strtemp.Mid(6, 2);
	strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

	strtemp = m_strSaveTime;	//check time
	strHour = strtemp.Mid(0, 2);
	strMin = strtemp.Mid(2, 2);
	strSec = strtemp.Mid(4, 2);
	strTime.Format(_T("%s_%s_%s"),strHour,strMin,strSec);

	G_AddLog(3, 3, _T("Check Time m_fnRenameQmsImgFile() Date:%s Time:%s"), m_strSaveDate, m_strSaveTime);

	strRetVal.Format(_T("%s%s\\%s\\%s\\%s"), strRootPath, strDate, strStepId, strPanelID.TrimRight(), strTime);
	//strRetVal.Format(_T("%s%s\\%s\\%s\\%s"), strRootPath, m_strSaveDate, strStepId, strPanelID.TrimRight(), m_strSaveTime);
	//2018.01.16

	//strRetVal.Format(_T("%s%s\\%s\\%s\\%s\\%s"), strRootPath, m_strSaveDate, strStepId, strGlassID.TrimRight(), strPanelID.TrimRight(), m_strSaveTime);	//2017.06.29	Qms Img 경로 내 GlassID 추가
	strRetVal.Append(_T("\\"));

	strtemp = _T("KV01");

	switch (struImgInfo.nDefType)
	{
	case INSP_DUST:
		break;

	case INSP_WHITE_TURBIDITY:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV15");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV16");
		break;

	case INSP_ITO:
		strtemp = _T("KV27");
		break;

	case INSP_EDGE_BROKEN:
		strtemp = _T("KV03");
		break;

	case INSP_PAD_SCRATCH: 
		strtemp = _T("KV28");
		break;

	case INSP_DIMPLE_POINT:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV17");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV18");
		break;

	case INSP_ETCHING_LINE:	case INSP_CF_PIMPLE_CIRCLE:
	case INSP_CF_WAVE_PATTERN:  case INSP_ETC_LINE:		case INSP_ETC_POINT:
		strtemp = _T("KV06");
		break;

	case INSP_DIMPLE_LINE: 
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV19");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV20");
		break;

	case INSP_PAD_BURNT:
		strtemp = _T("KV29");
		break;

	case INSP_BURR:
		strtemp = _T("KV09");
		break;

	case INSP_POL_ALIEN_SUBSTANCE:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KH01");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KH02");

		break;

	case INSP_POL_BUBBLE:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KH03");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KH04");
		break;

	case INSP_POL_SCRATCH:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KH05");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KH06");
		break;

	case INSP_POL_MISS:
		strtemp = _T("KH07");
		break;

	case INSP_CF_PROTECTION:	case INSP_TFT_PROTECTION:	//2017.04.12
		strtemp = _T("KH10");	//미지정
		break;
		//2019.03.16 by ryu TEST
	case INSP_B_GRADE_CHECK:	case INSP_C_GRADE_CHECK:	//2019.03.16
		strtemp = _T("KH00");	//
		break;

	case INSP_POL_MINOR:
		strtemp = _T("KS01");
		break;

	case INSP_BLACK_SCRATCH:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV11");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV12");
		break;

	case INSP_MURA:
		strtemp = _T("KS04");
		break;

	case INSP_CF_BURR :
		strtemp = _T("KV05");
		break;

	case INSP_TFT_BURR :
		strtemp = _T("KV06");
		break;

	case INSP_CF_BROKEN :
		strtemp = _T("KV09");
		break;

	case INSP_TFT_BROKEN :
		strtemp = _T("KV10");
		break;

	case INSP_CF_CORNER_BROKEN :
		strtemp = _T("KV13");
		break;

	case INSP_TFT_CORNER_BROKEN :
		strtemp = _T("KV14");
		break;

	case INSP_CF_CELL_BROKEN :
		strtemp = _T("KV03");
		break;

	case INSP_TFT_CELL_BROKEN :
		strtemp = _T("KV03");
		break;

	case INSP_PIMPLE_POINT:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV01");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV02");
		break;

	case INSP_PIMPLE_LINE:
		if(struImgInfo.nDefFlag == CF)
			strtemp = _T("KV03");
		else if(struImgInfo.nDefFlag == TFT)
			strtemp = _T("KV04");
	}

	//nDefNo += nDefNo;
	strtemp.Append(_T("-"));
	strDefNo.Format(_T("%0003d"),nDefNo/*nDefNo*/);
	strtemp.Append(strDefNo);
	strtemp.Append(_T("-1"));
	strtemp.Append(EXTENSION_JPG);
	_tcscpy(struImgInfo.tImgPath  , strRetVal);

	_tcscpy(struImgInfo.tImgName  , strtemp);
	strRetVal.Append(strtemp);
	G_AddLog(3, 3, _T("m_fnRenameQmsImgFile() - End"));
	return TRUE;
}


int   CDataManager::m_fnMakeQmsData_AVL(CString strFilePath, CString &strQmsFilePath, BOOL &bLowerEqpBypass)	 //2015.09.17 change BOOL -> int
{
	G_AddLog(3, 3, _T("m_fnMakeQmsData_AVL() - Start"));
	BOOL bRetVal = FALSE, bPanelCodeTest1 = FALSE, bPanelCodeTest2 = FALSE;
	BOOL bCreatDirFlg = FALSE;
	int nRetVal = 0;	//2015.09.17 add
	CStdioFile cstFile;
	int nKvCntVal = 0, nKhCntVal = 0;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\PanelID1_V1_00.txt"), CFile::modeRead | CFile::typeText, NULL))	
	{
		G_AddLog(3, 2, _T("m_fnMakeQmsData_AVL1 QMS File Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}

	m_nCfNgCnt		= 0;
	m_nTftNgCnt		= 0;
	m_nPenetNgCnt	= 0;

	m_stDefType.initDefType();
	m_stDefType_UI.initDefType();	//2019.01.17 add by mby

	CStringArray strArrayData;
	CString strLine, strItem, strNewLine ,strTemp;
	CString strLineHeader, strLinePanel, strLineSideInfo, strLineDefect, strAlignImage;
	CString strLineEdgeDist= _T(""), strLineLuminance = _T("");
	CString strLineLuminanceCF= _T(""), strLineLuminanceTFT= _T("");
	CString strLineEdgeDistCF= _T(""), strLineEdgeDistTFT= _T("");
	CString strSubSide, strIunsentPath = _T("");
	CString check_date= _T(""), strMesType = _T("");
	CString strVal = _T("");	
	CString strQmsFileName = _T("");
	CString strMeasureDist= _T("");	//2020.10.13 add by mby
	int nDefCnt = 0, nMesDstCnt =0 ;
	long imgX = 0, imgY = 0;
	int curPos = 0 , nAlignToMarkCnt = 0 , nLumitCnt;
	CString strCnt ;
	BOOL bRet = FALSE;
	nAlignToMarkCnt = 1;
	nLumitCnt = 1;
	CString cDefectVision = _T(""), CPanelId, strImgFilePath, strFileName, strImgName, strQmsImgName, strRetVal;
	BOOL bimgRetVal = FALSE;
	int nImgRetCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	CString strDefType;
	STRU_IMAGE_INFO stImgInf;
	//2015.08.19
	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
	//2015.08.19
	stAlignMeasureInfo.init_info();

	//2016.03.02
	STRU_LARGE_RECEIVE_LUMINANCE_DATA stLuminanceData;	stLuminanceData.init_info();
	int nLumiVal = 0;
	CString strLumiVal = _T("");
	//2016.03.02
	// 2016.03.22 add by ikm - 후보군 기능 추가 - S
	CString strDefFlag = _T(""), strDefSizeX = _T(""), strDefSizeY = _T(""), strDefLumi = _T("");
	DOUBLE dTempVal1, dTempVal2, dDefThinkness, dDefLength, dDefDistance;
	int nDefFlag = 0, nDefLumi = 0, nDefType = 0;
	
	//2018.06.03
	CString strDefSize = _T("");
	
	BOOL bIsGradeA  = FALSE;
	BOOL bIsGradeB  = FALSE;
	BOOL bIsGradeB2 = FALSE;	//2020.07.29 add by mby
	BOOL bIsGradeC  = FALSE;
	int  nGradeRankFlag = 0;

	m_bUseAreaCheck = FALSE;	//2020.03.03 add by mby

	CString strDefPos;
	
	for (int i = 0; i < MAX_DEFECT_KIND; i++)
		m_stPanelJudgeInfo[i].init_info();
	// 2016.03.22 add by ikm - 후보군 기능 추가 - E
	//2020.01.15 add by mby	
	for (int i = 0; i < 35; i++)
	{
		m_bPaCodeFlag[i] = FALSE;	
		m_bPaGradeFlag[i] = FALSE;
	}

	//2020.09.09 add by mby S-
	for (int i = 0; i < 16; i++)
	{
		m_nArrayPaCntEdgeCount[i] = 0;		
	}
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		m_nArrayPaCntPDCount[i] = 0;
		m_bPaOverlapAlarmFlag[i] = 0;	//2021.01.13 add by mby for Edge,Corner Broken
 
	}
	//2020.09.09 add by mby E-

	//2020.01.15 add by mby	
	m_bPaCheckFlag		= FALSE;		//2020.04.23 add by mby
	m_bDcc_Check1_flag	= FALSE;		 //2020.04.10 add by mby
	m_bDcc_Check2_flag	= FALSE;		 //2020.04.10 add by mby
	m_nEdgeConnectFlag  = 0;			//2021.02.04 add by mby
	m_bMea_Fail_flag	= 0;			//2021.04.22 add by mby
	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("HEADERDATA"))
		{
			strLineHeader = m_fnMakeQmsDataHead(strLine);

			//strQmsFilePath = d:\\QMS\\wait\\U_AVL205_0670

			strVal =  _T("_") + m_strQmsSendFileDate;
			strQmsFilePath += strVal;

			strQmsFilePath += EXTENSION_TMP;
			//strQmsFilePath = d:\\QMS\\wait\\U_AVL205_0670_endtime.tmp
		}

		if(strItem ==  _T("PANELDATA"))
		{
			strLinePanel = m_fnMakeQmsDataPanel(strLine);
			CPanelId = m_fnGetItemData(3, strLine);
			_tcscpy(stImgInf.tImgPnlID , CPanelId);
		}	

		if(strItem ==  _T("DEFECT"))
		{
			cDefectVision = m_fnGetItemData(FLAG + 1, strLine);
			//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(cDefectVision, CPanelId);
			pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)_ttoi(cDefectVision), (LPARAM)(LPCTSTR)CPanelId);
			if(_ttoi(cDefectVision) == POL)  m_fnSetItemData(D_FLAG - 1 , strLine, _T("0"), 1 , D_ITEM_END - 1);
			
			//if(nDefCnt <= 999)
			if (_ttoi( m_fnGetItemData(D_DEFECT_NO - 1, strLine)) <= 999)
			{
				if (_ttoi(cDefectVision) == CF)	  m_stDefType.CfNgCnt++;//m_fnSetCfNgCnt();
				else if (_ttoi(cDefectVision) == TFT)  m_stDefType.TftNgCnt++;//m_fnSetTftNgCnt();
				else							  m_stDefType.CfNgCnt++;

				// 2016.03.22 add by ikm - 후보군 기능 추가 - S
				strDefType  = m_fnGetItemData(DEFECT_TYPE + 1, strLine);
				strDefFlag  = m_fnGetItemData(FLAG + 1, strLine);
				strDefSizeX = m_fnGetItemData(IMG_SIZE_X + 1, strLine);
				strDefSizeY = m_fnGetItemData(IMG_SIZE_Y + 1, strLine);
				strDefLumi  = m_fnGetItemData(LUMINANCE + 1, strLine);
				dTempVal1   = _tstof((LPCTSTR)strDefSizeX);
				dTempVal2   = _tstof((LPCTSTR)strDefSizeY);
				strDefPos = m_fnGetItemData(DEFECT_CODE + 1, strLine);

				//2018.06.03 Defect Size(um) >= 1.5mm : 1500(um)
				strDefSize = m_fnGetItemData(DEFECT_SIZE + 1, strLine);
				/*if((_ttof(strDefType) == INSP_CF_BROKEN || _ttof(strDefType) == INSP_TFT_BROKEN ) && _ttof(strDefSize) >= 2.5)
				bIsGradeC = TRUE;
				else if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN || _ttof(strDefType) == INSP_TFT_CORNER_BROKEN) && _ttof(strDefSizeY) >= 2.5 && _ttof(strDefSizeX) >= 2.5)
				bIsGradeC = TRUE;*/

				if (dTempVal1 < dTempVal2)
				{
					dDefDistance = dTempVal2;
					dDefLength = dTempVal2;
					dDefThinkness = dTempVal1;
				}
				else
				{
					dDefDistance = dTempVal1;
					dDefLength = dTempVal1;
					dDefThinkness = dTempVal2;
				}

				nDefFlag = _tstoi((LPCTSTR)strDefFlag);
				nDefLumi = _tstoi((LPCTSTR)strDefLumi);
				nDefType = _tstoi((LPCTSTR)strDefType);
				m_fnPanelJudgeCheck(nDefType, nDefFlag, dDefDistance, dDefLength, dDefThinkness, nDefLumi);
				// 2016.03.22 add by ikm - 후보군 기능 추가 - E

				nKvCntVal ++;
				stImgInf.nDefType = _ttoi(m_fnGetItemData(DEFECT_TYPE + 1, strLine));
				stImgInf.nDefFlag = _ttoi(m_fnGetItemData(FLAG + 1, strLine));	//2018.01.22
				strImgName = m_fnGetItemData(DEFECT_IMG_NO + 1 , strLine);
				m_fnRenameQmsImgFile( stImgInf, strImgName, nKvCntVal);
				strImgFilePath = stImgInf.tImgPath;

				//strRetVal = m_fnLastToken(strImgFilePath, _T("\\"));

				//strImgFilePath = strImgFilePath.Left(strImgFilePath.GetLength() - strRetVal.GetLength());

				//strRetVal = strImgName;

				//CString sTempTst;
				////strImgName = strImgFilePath + strRetVal;
				//sTempTst.Format(QMS_IMAG_TEMP _T("%s"),CPanelId,strImgName);
				//strRetVal = stImgInf.tImgName;
				//strIunsentPath = strImgFilePath;
				//strImgFilePath = strImgFilePath + strRetVal;

				//bCreatDirFlg = pFrame->m_fnCreateFolder(strIunsentPath);
				//bimgRetVal = MoveFile(sTempTst/*strImgName*/, strImgFilePath);

				//if(bimgRetVal == FALSE)
				//{	
				//	while(nImgRetCnt < 3)
				//	{
				//		bCreatDirFlg = pFrame->m_fnCreateFolder(strIunsentPath);
				//		bimgRetVal = MoveFile(sTempTst, strImgFilePath);
				//		G_AddLog(3, 2, _T("m_fnMakeQmsData_AVL Retry image save %s to %s: %d"), sTempTst, strImgFilePath , GetLastError());
				//		nImgRetCnt ++; 
				//		if (bimgRetVal)	break;		
				//	}			
				//}	
				//nImgRetCnt = 0;

				strImgFilePath.Append(strImgName);
				strLineDefect += m_fnMakeQmsDataDefect_AVL(strLine, nKvCntVal);	

				//2020.07.27 Grade set S-
				nGradeRankFlag = m_fnGradeRankChk(strDefType, strDefSizeX, strDefSizeY, strDefPos);
				if(nGradeRankFlag == 1){	//Grade A
					bIsGradeA = TRUE ;
				}else if(nGradeRankFlag == 2){	//Grade B
					bIsGradeB = TRUE ;
				}else if(nGradeRankFlag == 3){	//Grade B2
					bIsGradeB2 = TRUE ;
				}else if(nGradeRankFlag == 4){	//Grade C
					bIsGradeC = TRUE ;
				}
				//2020.07.27 Grade set E-

				//2020.07.29 comment by mby
				//if((_ttof(strDefType) == INSP_TFT_BROKEN))
				//{
				//	if(m_stDefType.TftBrokenA && (strDefPos == _T("A")))
				//	{
				//		//if(_ttof(strDefSizeY) > 2.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[TFT_VALUE1]))
				//		{
				//			if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			 else if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}						 
				//	}	
				//	else if(m_stDefType.TftBrokenB && (strDefPos == _T("B"))) // 2018.11.21 삼성 사양 : 왕깡 요청 by ryu
				//	{
				//		//if(_ttof(strDefSizeX) > 1.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[TFT_VALUE2])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			 else if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}	
				//	}
				//	else if(m_stDefType.TftBrokenC && (strDefPos == _T("C")))
				//	{
				//		//if(_ttof(strDefSizeY) > 2.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[TFT_VALUE3])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}
				//	else if(m_stDefType.TftBrokenD && (strDefPos == _T("D")))
				//	{
				//		//if(_ttof(strDefSizeX) > 3.0) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[TFT_VALUE4])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[TFT_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}			
				//}
				//if((_ttof(strDefType) == INSP_CF_BROKEN))
				//{
				//	// 2018.11.21 삼성 사양 : 왕깡 요청 by ryu
				//	if(m_stDefType.CfBrokenA && (strDefPos == _T("A")))
				//	{
				//		//if(_ttof(strDefSizeY) > 1.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_VALUE1])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			 else if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}	
				//	else if(m_stDefType.CfBrokenB && (strDefPos == _T("B")))
				//	{
				//		//if(_ttof(strDefSizeX) > 0.9) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeX) >_ttof(pAuto->m_strValue_NumAuto[CF_VALUE2])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}
				//	else if(m_stDefType.CfBrokenC && (strDefPos == _T("C")))
				//	{
				//		//if(_ttof(strDefSizeY) > 1.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_VALUE3])) //bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}
				//	else if(m_stDefType.CfBrokenD && (strDefPos == _T("D")))
				//	{
				//		//if(_ttof(strDefSizeX) > 1.2) bIsGradeC = TRUE;
				//		if(_ttof(strDefSizeX) >_ttof(pAuto->m_strValue_NumAuto[CF_VALUE4]))// bIsGradeC = TRUE;
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[CF_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}	
				//}
				////2020.02.13 add by mby S-
				//if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN))
				//{
				//	if(m_stDefType.CfCornerBrokenA )
				//	{
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]) || _ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]))
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			 else if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}						 
				//	}	
				//	else if(m_stDefType.CfCornerBrokenB) 
				//	{
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]) || _ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]))
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			 else if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}	
				//	}
				//	else if(m_stDefType.CfCornerBrokenC)
				//	{
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]) || _ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]))
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}
				//	else if(m_stDefType.CfCornerBrokenD)
				//	{
				//		if(_ttof(strDefSizeY) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]) || _ttof(strDefSizeX) > _ttof(pAuto->m_strValue_NumAuto[CF_COR_VALUE]))
				//		{
				//			if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("A"))
				//			 {
				//				 bIsGradeA = TRUE ;
				//			 }
				//			else if(pAuto->m_strValue_GradeAuto[CF_COR_BROKEN_VAL] == _T("B"))
				//			 {
				//				 bIsGradeB = TRUE ;
				//			 }
				//			 else
				//			 {
				//				 bIsGradeC = TRUE ;
				//			 }
				//		}
				//	}			
				//}
				//2020.02.13 add by mby E-
				//strLine = m_fnSetItemData(D_DEF_IMG_NO , strLine, m_strQmsImgName, m_strQmsImgName.GetLength(), D_ITEM_END - 1);
				strImgFilePath = m_fnSetItemMutiData(3 , strImgFilePath, _T("i_send") , 6, 8 - 1, _T("\\"));	
				strLine.Append(EXT_COLON); strLine.Append(strImgFilePath); 
				nDefCnt++;
			}			
		}	

		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF73"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 8);
			strLineEdgeDistCF = (strItem);
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT73"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 8);
			strLineEdgeDistTFT = (strItem);
		}	

		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF75"))
		{
			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			{
				strItem = strLine;
				m_fnLuminanceSet(strItem, CPanelId);
			}
			else
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 4);
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);			
				strLineLuminanceCF = (strItem);

				//strLineLuminance = strItem + EXT_SEMICOL;
			}
		}	

		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT75"))
		{
			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			{
				strItem = strLine;
				m_fnLuminanceSet(strItem, CPanelId);
			}
			else
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 4);
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
				strItem = strItem.Mid(0, strItem.ReverseFind(_T(',')));
				strLineLuminanceTFT = strItem + EXT_SEMICOL;
			}
		}	
		//2016.11.21	Mr.Chu
		strItem = m_fnGetItemData(LAYER_NO - 1, strLine);
		if( !strItem.Compare(_T("TFT77")) || !strItem.Compare(_T("CF77")) || !strItem.Compare(_T("POL78")) )
		{
			CString strPanelData = _T("");
			int nStringCnt = 0,nDefectNum = 0;

			G_AddLog(3, 2, _T("Luminance Code [%s] : %s"),CPanelId, strLine);		//Data 표기

			strPanelData = strLine;
			strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);
			strPanelData.TrimRight();

			nStringCnt = strPanelData.Replace(EXT_COLON,EXT_COLON) + 1;

			for(int i=0;i<nStringCnt;i++)
			{
				strTemp = m_fnGetItemData(LAYER_NO - 1, strPanelData);
				strTemp.Trim();
				strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);

				if( strTemp == _T("0") )
					continue;

				nDefectNum = _ttoi(strTemp);
				switch(nDefectNum)
				{
				case CFCLUMINANCE:
					m_stDefType.CFCLuminance++;
					break;
				case CBCLUMINANCE:
					m_stDefType.CBCLuminance++;
					break;
				case CFSLUMINANCE:
					m_stDefType.CFSLuminance++;
					break;
				case CBSLUMINANCE:
					m_stDefType.CBSLuminance++;
					break;
				case TFCLUMINANCE:
					m_stDefType.TFCLuminance++;
					break;
				case TBCLUMINANCE:
					m_stDefType.TBCLuminance++;
					break;
				case TFSLUMINANCE:
					m_stDefType.TFSLuminance++;
					break;
				case TBSLUMINANCE:
					m_stDefType.TBSLuminance++;
					break;
				default:
					break;
				}
			}
		}
		//

		strArrayData.Add(strLine + _T("\n"));
	}
	cstFile.Close();

	if (!cstFile.Open(strFilePath, CFile::modeWrite | CFile::typeText, NULL))
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\PanelID1_V1_00.txt"), CFile::modeRead | CFile::typeText, NULL))	
	{
		G_AddLog(3, 2, _T("m_fnMakeQmsData_AVL2 QMS File Open Error %s : %d"), strFilePath , GetLastError());
		G_AddLog(3, 3, _T("m_fnMakeQmsData_AVL() - End"));
		return FALSE;
	}
	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	strArrayData.RemoveAll();
	cstFile.Close();

	//2021.01.14 add by mby for log
	G_AddLog(2, 0, _T("Alarm flag 1:%d, 2:%d, 3:%d, 4:%d"),m_bPaOverlapAlarmFlag[1],m_bPaOverlapAlarmFlag[2],m_bPaOverlapAlarmFlag[3],m_bPaOverlapAlarmFlag[4]);	
	// 중복 좌표 함수 기입 - 기존위치1
	m_fnCompareOverlapDefTypeCheck( CPanelId, strFilePath, m_stDefType);

	m_bUseAreaCheck = TRUE;	//2020.03.03 add by mby

	CString strPanelID1, strPanelID2, cCountRoot = _T("") , cCountVal = _T("");
	int nGradeCnt;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);


	if (!nDefCnt)
	{		
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, cCountVal, cCountRoot);

		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(_T("-1"), CPanelId);
		pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)-1, (LPARAM)(LPCTSTR)CPanelId);
	}
	else
	{
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, cCountVal, cCountRoot);
	}

	//////////////////////////////////////////////////////////////////
	///////////////////////패널 부가 정보/////////////////////////////  
	////"30"고정
	//strSubSide.Append(_T("30"));
	//strSubSide.Append(EXT_COLON);

	////KV01결함수
	//strSubSide.Append(IntToCstr(m_stDefType.CfWhiteBlur));
	//strSubSide.Append(EXT_COLON);

	////KV02결함수
	//strSubSide.Append(IntToCstr(m_stDefType.ItoPeel));
	//strSubSide.Append(EXT_COLON);

	////KV03결함수
	//strSubSide.Append(IntToCstr(m_stDefType.WanesBroken + m_stDefType.CfCornerBroken + m_stDefType.CfBroken + m_stDefType.TftBroken + m_stDefType.TftCornerBroken + m_stDefType.CfCellBroken + m_stDefType.TftCellBroken));	//2015.08.26
	//strSubSide.Append(EXT_COLON);

	////KV04결함수
	//strSubSide.Append(IntToCstr(m_stDefType.SocketScrach));
	//strSubSide.Append(EXT_COLON);

	////KV05결함수
	//strSubSide.Append(IntToCstr((m_stDefType.CfDimplePoint + m_stDefType.TftDimplePoint)));
	//strSubSide.Append(EXT_COLON);

	//strSubSide.Append(IntToCstr((m_stDefType.CfEdgeilegal + m_stDefType.TftEdgeilegal )));
	//strSubSide.Append(EXT_COLON);

	//strSubSide.Append(IntToCstr((m_stDefType.CfDimpleLine + m_stDefType.TftDimpleLine) + (m_stDefType.TftScrach + m_stDefType.CfScrach)));
	//strSubSide.Append(EXT_COLON);

	//strSubSide.Append(IntToCstr(m_stDefType.SocketBurn));
	//strSubSide.Append(EXT_COLON);

	//strSubSide.Append(IntToCstr(m_stDefType.Burr + m_stDefType.CfBurr + m_stDefType.TftBurr));
	//strSubSide.Append(EXT_COLON);

	//for(int i = 0 ; i < 42 ; i ++)	//2018.01.15 21-> 42
	//{
	//	strSubSide.Append(_T("0"));
	//	strSubSide.Append(EXT_COLON);
	//}

	m_fnMakeQmsDataDefCnt_AVL(strSubSide);	//2018.01.22

	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE || pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
	{
		stAlignMeasureInfo = m_fnReadAlignMeasureData(CPanelId);

		//2018.01.16 임시 Comment
		strLineEdgeDistCF = stAlignMeasureInfo.cCFMeasureDistanceData;
		if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())	strLineEdgeDistCF = _T("0,0,0,0,0,0,0,0,");

		strLineEdgeDistTFT = stAlignMeasureInfo.cTFTMeasureDistanceData;
		if (strLineEdgeDistTFT == _T("") || strLineEdgeDistTFT.IsEmpty())	strLineEdgeDistTFT = _T("0,0,0,0,0,0,0,0,");
		
		//strLineEdgeDistCF = _T("1,2,3,4,5,6,7,8,");
		//strLineEdgeDistTFT = _T("9,10,11,12,13,14,15,16,");
		//2018.01.16 임시 Comment


		// 2018.03.12 LGU strLineEdgeDistTFT <-> strLineEdgeDistCF
		strLineEdgeDist.Format(_T("%s%s"), strLineEdgeDistTFT,strLineEdgeDistCF);
		//2020.09.23 add by mby S-
		int nCimMeasureData[MAX_CIM_MEASURE_DATA];	
		double nCimMeasureData_AVL[MAX_CIM_MEASURE_DATA];
		CString strCimMeasureTemp;
		for (int i = 0; i < MAX_CIM_MEASURE_DATA; i++)
		{
				strCimMeasureTemp = pFrame->m_DataManager.m_fnGetItemData(i, strLineEdgeDist);
				nCimMeasureData_AVL[i] = _tstoi((LPCTSTR)strCimMeasureTemp);
				nCimMeasureData[i] = pFrame->m_DataManager.MeasureValueLimitChk(0, 2, i, int(nCimMeasureData_AVL[i] * 100));
		}
		//strLineEdgeDist= _T("");

		strMeasureDist.Format(_T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"),nCimMeasureData[0],nCimMeasureData[1],nCimMeasureData[2],nCimMeasureData[3],nCimMeasureData[4],
			nCimMeasureData[5],nCimMeasureData[6],nCimMeasureData[7],nCimMeasureData[8],nCimMeasureData[9],nCimMeasureData[10],nCimMeasureData[11],nCimMeasureData[12],nCimMeasureData[13],
			nCimMeasureData[14],nCimMeasureData[15]);
		/*for (int i = 0; i < MAX_CIM_MEASURE_DATA; i++)
		{
			if(i==0){
				strLineEdgeDist.Format(_T("%d,"), nCimMeasureData[i]);
			}else{
				strLineEdgeDist.Format(_T("%s%d,"), strLineEdgeDist,nCimMeasureData[i]);
			}
		}*/
		//2020.09.23 add by mby E-
		strSubSide.Append(strMeasureDist);		//2020.10.13 strLineEdgeDist-> strMeasureDist

		/*		if (strLineLuminanceCF == _T("") || strLineLuminanceCF.IsEmpty())
		{
		strLineLuminanceCF = (_T("0,0,0,0,"));
		}
		if (strLineLuminanceTFT == _T("") || strLineLuminanceTFT.IsEmpty())
		{
		strLineLuminanceTFT = (_T("0,0,0,0;"));
		}
		strLineLuminance.Format(_T("%s%s"),strLineLuminanceCF, strLineLuminanceTFT);*/	

		//2016.03.02
		stLuminanceData = m_fnReadLuminanceData(CPanelId);

		int nLumiBuf = NULL;
		for(int i = 0; i < MAX_LUMINANCE_DATA; i++)
		{
			if( i==6 ) 
			{	
				nLumiBuf = (stLuminanceData.nLuminanceLimitData[i] + stLuminanceData.nLuminanceLimitData[i+1]) / 2;	
				continue;	
			}

			if(	i == 7 )
				nLumiVal = nLumiBuf;
			else
				nLumiVal = stLuminanceData.nLuminanceLimitData[i];

			strLumiVal.Format(_T("%d"),nLumiVal);

			//2018.01.16
			//if(i == 8)
			//	strLineLuminance.Append(strLumiVal + EXT_SEMICOL);
			//else
			//	strLineLuminance.Append(strLumiVal + EXT_COLON);

			strLineLuminance.Append(strLumiVal + EXT_COLON);
		}

		strSubSide.Append(strLineLuminance);

		bLowerEqpBypass = m_fnGetLowerBypassNgFlag_AVL();	//2016.05.28 Mr.Chu		Defect 여부에 따라 Lower Bypass 여부 결정
	}
	else
	{

		if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())
		{
			strLineEdgeDistCF = (_T("0,0,0,0,0,0,0,0,"));
		}
		if (strLineEdgeDistTFT == _T("") || strLineEdgeDistTFT.IsEmpty())
		{
			strLineEdgeDistTFT = (_T("0,0,0,0,0,0,0,0,"));
		}
		if (strLineLuminanceCF == _T("") || strLineLuminanceCF.IsEmpty())
		{
			strLineLuminanceCF = (_T("0,0,0,0,"));
		}
		if (strLineLuminanceTFT == _T("") || strLineLuminanceTFT.IsEmpty())
		{
			strLineLuminanceTFT = (_T("0,0,0,0;"));
		}
		strLineEdgeDist.Format(_T("%s%s"),strLineEdgeDistCF, strLineEdgeDistTFT);
		strSubSide.Append(strLineEdgeDist);
		strLineLuminance.Format(_T("%s%s"),strLineLuminanceCF, strLineLuminanceTFT);	
		strSubSide.Append(strLineLuminance);		

	}

	for(int i = 0 ; i < 2; i ++)			// ADD KYH01  QMS 임시 테스트용
	{			
		strSubSide.Append(_T("0"));
		strSubSide.Append(EXT_COLON);		
	}
	//2019.06 20 by ryu EQ, Stage 추가
	strSubSide.Append(m_strPolPositionEQ_CV2);
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(m_strPolPositionStage_CV2);
	strSubSide.Append(EXT_COLON);
	// 2018.04.23 Data format 변경으로 인한 주석 처리

	//측정이미지 C1 파일 이름, Size X, Size Y
// 	strSubSide.Append(_T("C1_270.jpg"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("17136"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("14336"));
// 	strSubSide.Append(EXT_COLON);
// 
// 	//측정이미지 C2 파일 이름, Size X, Size Y
// 	strSubSide.Append(_T("C2_270.jpg"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("17136"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("14336"));
// 	strSubSide.Append(EXT_COLON);
// 
// 	//측정이미지 C3 파일 이름, Size X, Size Y
// 	strSubSide.Append(_T("C3_090.jpg"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("17136"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("14336"));
// 	strSubSide.Append(EXT_COLON);
// 
// 	//측정이미지 C4 파일 이름, Size X, Size Y
// 	strSubSide.Append(_T("C4_090.jpg"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("17136"));
// 	strSubSide.Append(EXT_COLON);
// 	strSubSide.Append(_T("14336"));
// 	strSubSide.Append(EXT_COLON);

	//결함 MAP 파일 이름, Size X, Size Y
	strSubSide.Append(_T("ALLMAP_000.jpg"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//결함1 파일 이름, Size X, Size Y
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//결함2 파일 이름, Size X, Size Y
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//결함3 파일 이름, Size X, Size Y
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//결함4 파일 이름, Size X, Size Y
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//결함5 파일 이름, Size X, Size Y
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	// Camera Image, 4 
	strSubSide.Append(_T("4"));
	strSubSide.Append(EXT_COLON);

	
	//2018.04.23 Align Image 추가

	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
		//측정이미지 C1 파일 이름, Size X, Size Y
	strAlignImage.Append(_T("C1_270.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("17136"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("14336"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));
	//측정이미지 C2 파일 이름, Size X, Size Y
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C2_270.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("17136"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("14336"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));
	//측정이미지 C3 파일 이름, Size X, Size Y
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C3_090.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("17136"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("14336"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));

	//측정이미지 C4 파일 이름, Size X, Size Y
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C4_090.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("17136"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("14336"));
	strAlignImage.Append(EXT_SEMICOL);
	//2020.06.18 add by mby TFT측정이미지 S-
	strAlignImage.Append(_T("\n"));
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C1_TFT.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C2_TFT.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C3_TFT.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_SEMICOL);
	strAlignImage.Append(_T("\n"));
	strAlignImage.Append(_T("50"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("C4_TFT.jpg"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_COLON);
	strAlignImage.Append(_T("600"));
	strAlignImage.Append(EXT_SEMICOL);
	//2020.06.18 add by mby TFT측정이미지 E-

	m_strCimMeasureDataTemp = strLineEdgeDist;	//2016.01.15
	G_AddLog(3, 3, _T("WM_UI_CONTROL_DEF_TYPE_DIS() - Start"));
	//2019.03.16 add by mby
	CString strCF, strTFT;
	strCF = stAlignMeasureInfo.cCFMeasureDistanceData;
	strTFT = stAlignMeasureInfo.cTFTMeasureDistanceData;
	if (strCF == _T("") && strTFT == _T("")){
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_MEASURE_VALUE_ZERO, 1);
	}
	//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgDefInfo->m_fnUpdateDefType(m_stDefType);	
	//pAuto->m_pDlgDefInfo->SendMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);
	//pAuto->m_pDlgDefInfo->PostMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);
	//2015.10.06
	CString strMeasureData = _T("");
	strMeasureData.Format(_T("%s"),strLineEdgeDist);
	((CViewAuto*)pFrame->m_pViewAuto)->SendMessage(WM_UI_CONTROL_SAV_MEASURE_DAT, (WPARAM)EQP_TYPE_AVL, (LPARAM)(LPCTSTR)strMeasureData);
	//((CViewAuto*)pFrame->m_pViewAuto)->PostMessage(WM_UI_CONTROL_SAV_MEASURE_DAT, (WPARAM)EQP_TYPE_AVL, (LPARAM)(LPCTSTR)strMeasureData);
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnSaveFileMeasureInfo(strMeasureData, EQP_TYPE_ACC);
	//2015.10.06

	/*while(m_fnLastTokenCnt(strLineEdgeDist, EXT_COLON) < 15)
	{
	strLineEdgeDist.Append(_T("0"));
	strLineEdgeDist.Append(EXT_COLON);
	}
	*/

	bRetVal = m_fnGetJudgeGrade_AVL(bPanelCodeTest1, m_strDefGrade1, m_strdefJudge1);
	G_AddLog(2, 0, _T("bIsGradeA : %d, bIsGradeB : %d, bIsGradeC : %d"),bIsGradeA,bIsGradeB,bIsGradeC);	//2020.04.06 add by mby
	//2020.08.14 add by mby S-
	if(m_strdefJudge1 == _T("AO78")/*TFTCORNERBROKEN*/||m_strdefJudge1 == _T("AO79")||m_strdefJudge1 == _T("AO80")||m_strdefJudge1 == _T("AO81") ||
	   m_strdefJudge1 == _T("AO74")/*CFCORNERBROKEN*/||m_strdefJudge1 == _T("AO75")||m_strdefJudge1 == _T("AO76")||m_strdefJudge1 == _T("AO77") ||
	   m_strdefJudge1 == _T("AO70")/*TFTBROKEN*/||m_strdefJudge1 == _T("AO71")||m_strdefJudge1 == _T("AO72")||m_strdefJudge1 == _T("AO73") ||
	   m_strdefJudge1 == _T("AO66")/*CFBROKEN*/||m_strdefJudge1 == _T("AO67")||m_strdefJudge1 == _T("AO68")||m_strdefJudge1 == _T("AO69"))
	{
		if(bIsGradeA)	m_strDefGrade1 = JUDGE_A;
		if(bIsGradeB)	m_strDefGrade1 = JUDGE_B;
		if(bIsGradeB2)	m_strDefGrade1 = JUDGE_B;	//2020.07.29 add by mby
		if(bIsGradeC)	m_strDefGrade1 = JUDGE_C;	//2018.06.03
	}
	//2020.08.14 add by mby E-

	//2015.09.17 add
	if (m_strDefGrade1 == JUDGE_A)	nRetVal = NG_FLAG_A_GRADE;					// 0 : A Grade
	else if (m_strDefGrade1 == JUDGE_B)
	{
		nRetVal = NG_FLAG_AVL_B_GRADE;		// 5 : B Grade
		m_stDefType.Bgradecheck  += 1;			// 2019.03.19 by ryu 
	}
	else if (m_strDefGrade1 == JUDGE_C)	
	{
		nRetVal = NG_FLAG_C_GRADE;				// 6 : C Grade
		m_stDefType.Cgradecheck  += 1;			// 2019.03.19 by ryu 
	}
	else if (m_strDefGrade1 == JUDGE_D)	nRetVal = NG_FLAG_D_GRADE;		// 7 : D Grade
	

	// 중복 좌표 함수 기입 - 기존위치2
	m_fnCompareOverlapDefTypeCheck( CPanelId, strFilePath, m_stDefType);
	m_bUseAreaCheck = FALSE;	//2020.03.03 add by mby

	//m_strDefGrade1 = _T("A");
	//2015.09.17 add -> 2018.07.24 by ryu 수정 및 Test
	m_fnGetJudgeGrade_AVL(bPanelCodeTest2, m_strDefGrade2, m_strdefJudge2);

	//2020.08.05 add by mby S-
	if(m_strDefGrade1 == _T("A")) 
	{				
		//PA Count Check			
		if(m_fnPACntPDCheck()){
			m_strDefGrade1 = JUDGE_B;	//Count over시 AVL: B, ABI: PD로 변경
			nRetVal = NG_FLAG_AVL_B_GRADE;
			G_AddLog(2, 0, _T("m_fnPACntPDCheck m_strDefGrade1 : %s"),m_strDefGrade1);	//2020.12.04 add by mby
		}
	}
	//2020.08.05 add by mby E-

	//if(pAuto->m_pDlgPanelCodeTest)	//2016.09.14	Mr.Chu		// 해당 UI 가 소멸시 실행되지 않도록 예외처리	( 왕빙 요청기능 )
	{

		int nPanelCode = 0;
		if(bPanelCodeTest1)
		{
			nPanelCode = m_fnPanelCodeTest(m_strdefJudge1);

			if(nPanelCode)
			{
				m_strDefGrade1 = JUDGE_B;
				m_strdefJudge1.Format(_T("%d"), nPanelCode);
				bRetVal = TRUE;
			}
			if( m_stPanelCodeTest.bTestActive )
				m_stJudgeItemInfo.PanelCodeTest = TRUE;
		}
		/*else if(bPanelCodeTest2)
		{
			nPanelCode = m_fnPanelCodeTest(m_strdefJudge2);

			if(nPanelCode)
			{
				m_strDefGrade2 = JUDGE_B;
				m_strdefJudge2.Format(_T("%d"), nPanelCode);
			}
			if( m_stPanelCodeTest.bTestActive )
				m_stJudgeItemInfo.PanelCodeTest = TRUE;
		}*/

	}
	//2018.12.14 add by mby S-
	if (m_strDefGrade1 == JUDGE_C){
		int nStopFlag = pFrame->m_iniConfigSet.GetProfileIntW(_T("CSTOP"), _T("STOP"), 0);
		if(nStopFlag != 0){
			//pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
			pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_REGRADE_CHECK_ALARM, 1);
		}
	}
	//2021.04.19 add by mby
	if(m_stDefType.CfMeasureFail){
		m_strdefJudge2 = _T("AO84");	/*CFMEASUREFAIL*/
		pFrame->m_DataManager.m_bMea_Fail_flag = 1;
	}else if(m_stDefType.TftMeasureFail){
		m_strdefJudge2 = _T("AO85");	/*TFTMEASUREFAIL*/
		pFrame->m_DataManager.m_bMea_Fail_flag = 2;
	}else{
		m_strdefJudge2 = _T("0");	/*TFTMEASUREFAIL*/
		pFrame->m_DataManager.m_bMea_Fail_flag = 0;
	}
	//2018.01.15
	//strLinePanel = m_fnSetItemData(3 , strLinePanel, m_strDefGrade1, m_strDefGrade1.GetLength() , P_ITEM_END - 1);
	//strLinePanel = m_fnSetItemData(4 , strLinePanel, m_strdefJudge1, m_strdefJudge1.GetLength() , P_ITEM_END - 1);
	//strLinePanel = m_fnSetItemData(5 , strLinePanel, m_strdefJudge2, m_strdefJudge2.GetLength() , P_ITEM_END - 1);
	strLinePanel = m_fnSetItemData(4 , strLinePanel, m_strDefGrade1, m_strDefGrade1.GetLength() , P_ITEM_END - 1);
	strLinePanel = m_fnSetItemData(5 , strLinePanel, m_strdefJudge1, m_strdefJudge1.GetLength() , P_ITEM_END - 1);
	////2020.04.20 add by mby S-
	//CString PaCodeFlag = _T("0");	
	//if(m_bDcc_Check1_flag){
	//	PaCodeFlag = _T("1");
	//}else if(m_bDcc_Check2_flag){
	//	PaCodeFlag = _T("2");
	//}

	//strLinePanel = m_fnSetItemData(6 , strLinePanel, PaCodeFlag, PaCodeFlag.GetLength() , P_ITEM_END - 1);
	strLinePanel = m_fnSetItemData(6 , strLinePanel, m_strdefJudge2, m_strdefJudge2.GetLength() , P_ITEM_END - 1);
	strQmsFileName = m_fnMakeQmsFileName();
	if(!pFrame->m_stParamInfo.bUseManualJudge || ((m_strDefGrade1 != JUDGE_C) && pFrame->m_stParamInfo.bUseManualJudge))
	{
		
		strQmsFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strQmsFileName;
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultGrade(m_strDefGrade1);
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultDefectCodeJudge(m_strdefJudge1);
		G_AddLog(3, 0, _T("NO strQmsFilePath:%s"),strQmsFilePath);
		pAuto->SendMessage(WM_UI_CONTROL_PNL_SUMMARY_LIST, (WPARAM)0, (LPARAM)(LPCTSTR)strQmsFilePath);
	}
	else
	{
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultDefectCodeJudge(m_strdefJudge1);
		strQmsFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Temp + strQmsFileName;
		G_AddLog(3, 0, _T("YES strQmsFilePath:%s"),strQmsFilePath);
	}
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnUpdatePanelSummary(strQmsFilePath);

	G_AddLog(2, 0, _T("Rank GRADE m_strDefGrade1 : %s"),m_strDefGrade1);	//2020.12.04 add by mby
	// 2018.06.20 99 Rank 추가 -> 2019.01.14 by ryu 허용화 요청
	//if(m_strDefGrade1 == JUDGE_B)//_T("B"))
	//{
	if(m_strdefJudge1 == _T("AO78")/*TFTCORNERBROKEN*/||m_strdefJudge1 == _T("AO79")||m_strdefJudge1 == _T("AO80")||m_strdefJudge1 == _T("AO81"))
	{
		if(m_strDefGrade1 == JUDGE_C || m_strDefGrade1 == JUDGE_B)
		{
			//2020.07.28 add by mby S-
			if(bIsGradeA || bIsGradeB || bIsGradeC || bIsGradeB2)	//2020.07.29 add bIsGradeB2 by mby
			{
				if(bIsGradeB){
					strSubSide.Append(m_strAryGradeB_Rank[12]);	
					m_str_rankCIMvalue =m_strAryGradeB_Rank[12];	
				}else if(bIsGradeB2){
					strSubSide.Append(m_strAryGradeB2_Rank[12]);	
					m_str_rankCIMvalue =m_strAryGradeB2_Rank[12];	
				}else{
					//2020.09.21 add by mby
					if (m_strDefGrade1 == JUDGE_B){
						strSubSide.Append(m_strAryGradeB_Rank[12]);	
						m_str_rankCIMvalue =m_strAryGradeB_Rank[12];
					}else if(m_strDefGrade1 == JUDGE_A){
						strSubSide.Append(JUDGE_AN); 
					}else{
						strSubSide.Append(m_strDefGrade1); 
					}
				}
				////2019.02.26 by ryu
				//if(pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK] != _T(""))
				//{
				//	strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK]);	
				//	m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK];		
				//}
				//else
				//{
				//	.Append(m_strDefGrade1); 
				//}

			}else
			{
				/*if(pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK]!= _T("")  )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK]);
					m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[TFT_CORNERBROKEN_RANK];
				}
				else
				{*/
					strSubSide.Append(m_strDefGrade1); 
				/*}*/
			}		
			
		}else
		{	
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
		//2020.07.28 add by mby E-
	}
	else if(m_strdefJudge1 == _T("AO74")/*CFCORNERBROKEN*/||m_strdefJudge1 == _T("AO75")||m_strdefJudge1 == _T("AO76")||m_strdefJudge1 == _T("AO77"))
	{
		//2020.02.28 add by mby S-
		if(m_strDefGrade1 == JUDGE_C || m_strDefGrade1 == JUDGE_B)
		{
			if(bIsGradeA || bIsGradeB || bIsGradeC || bIsGradeB2)	//2020.07.29 add bIsGradeB2 by mby
			{
				if(bIsGradeB){
					strSubSide.Append(m_strAryGradeB_Rank[8]);	
					m_str_rankCIMvalue =m_strAryGradeB_Rank[8];	
				}else if(bIsGradeB2){
					strSubSide.Append(m_strAryGradeB2_Rank[8]);	
					m_str_rankCIMvalue =m_strAryGradeB2_Rank[8];	
				}else{
					//2020.09.21 add by mby
					if (m_strDefGrade1 == JUDGE_B){
						strSubSide.Append(m_strAryGradeB_Rank[8]);	
						m_str_rankCIMvalue =m_strAryGradeB_Rank[8];
					}else if(m_strDefGrade1 == JUDGE_A){
						strSubSide.Append(JUDGE_AN); 
					}else{
						strSubSide.Append(m_strDefGrade1); 
					}
				}
				/*if(pAuto->m_strValue_RankAuto[CF_COR_BROKEN_VAL_RANK]!= _T("") )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[CF_COR_BROKEN_VAL_RANK]);	
					m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[CF_COR_BROKEN_VAL_RANK];
				}
				else
				{
					strSubSide.Append(m_strDefGrade1); 
				}*/
			}
			else
			{
				/*if(pAuto->m_strValue_RankAuto[CF_COR_BROKEN_VAL_RANK]!= _T("")  )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[CF_CORNERBROKEN_RANK]);
					m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[CF_CORNERBROKEN_RANK];
				}
				else
				{*/
					strSubSide.Append(m_strDefGrade1); 
				/*}*/
			}
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
		//2020.02.28 add by mby E-
	}
	else if(m_strdefJudge1 == _T("AO70")/*TFTBROKEN*/||m_strdefJudge1 == _T("AO71")||m_strdefJudge1 == _T("AO72")||m_strdefJudge1 == _T("AO73"))
	{
		if(m_strDefGrade1 == JUDGE_C || m_strDefGrade1 == JUDGE_B)
		{
			if(bIsGradeA || bIsGradeB || bIsGradeC || bIsGradeB2)	//2020.07.29 add bIsGradeB2 by mby
			{
				if(bIsGradeB){
					strSubSide.Append(m_strAryGradeB_Rank[4]);	
					m_str_rankCIMvalue =m_strAryGradeB_Rank[4];	
				}else if(bIsGradeB2){
					strSubSide.Append(m_strAryGradeB2_Rank[4]);	
					m_str_rankCIMvalue =m_strAryGradeB2_Rank[4];	
				}else{
					//2020.09.21 add by mby
					if (m_strDefGrade1 == JUDGE_B){
						strSubSide.Append(m_strAryGradeB_Rank[4]);	
						m_str_rankCIMvalue =m_strAryGradeB_Rank[4];
					}else if(m_strDefGrade1 == JUDGE_A){
						strSubSide.Append(JUDGE_AN); 
					}else{
						strSubSide.Append(m_strDefGrade1); 
					}
				}
				/*if(pAuto->m_strValue_RankAuto[TFT_RBROKEN_VAL_RANK]!= _T("") )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_RBROKEN_VAL_RANK]);	
					m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[TFT_RBROKEN_VAL_RANK];
				}
				else
				{
					strSubSide.Append(m_strDefGrade1); 
				}*/
			}
			else
			{
				/*if(pAuto->m_strValue_RankAuto[TFT_RBROKEN_RANK]!= _T("")  )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_RBROKEN_RANK]);
					m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[TFT_RBROKEN_RANK];
				}
				else
				{*/
					strSubSide.Append(m_strDefGrade1); 
				/*}*/
			}
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	}
	else if(m_strdefJudge1 == _T("AO66")/*CFBROKEN*/||m_strdefJudge1 == _T("AO67")||m_strdefJudge1 == _T("AO68")||m_strdefJudge1 == _T("AO69"))
	{
		if(m_strDefGrade1 == JUDGE_C || m_strDefGrade1 == JUDGE_B)
		{
			if(bIsGradeA || bIsGradeB || bIsGradeC || bIsGradeB2)	//2020.07.29 add bIsGradeB2 by mby
			{
				if(bIsGradeB){
					strSubSide.Append(m_strAryGradeB_Rank[0]);	
					m_str_rankCIMvalue =m_strAryGradeB_Rank[0];	
				}else if(bIsGradeB2){
					strSubSide.Append(m_strAryGradeB2_Rank[0]);	
					m_str_rankCIMvalue =m_strAryGradeB2_Rank[0];	
				}else{
					//2020.09.21 add by mby
					if (m_strDefGrade1 == JUDGE_B){
						strSubSide.Append(m_strAryGradeB_Rank[0]);	
						m_str_rankCIMvalue =m_strAryGradeB_Rank[0];
					}else if(m_strDefGrade1 == JUDGE_A){
						strSubSide.Append(JUDGE_AN); 
					}else{
						strSubSide.Append(m_strDefGrade1); 
					}
				}
				/*
				if(pAuto->m_strValue_RankAuto[CF_RBROKEN_VAL_RANK]!= _T("")  )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[CF_RBROKEN_VAL_RANK]);	
					m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[CF_RBROKEN_VAL_RANK];
				}
				else
				{
					strSubSide.Append(m_strDefGrade1); 
				}*/
			}
			else
			{
				/*if(pAuto->m_strValue_RankAuto[CF_RBROKEN_RANK] != _T("") )
				{
					strSubSide.Append(pAuto->m_strValue_RankAuto[CF_RBROKEN_RANK]);
					m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[CF_RBROKEN_RANK];
				}
				else
				{*/
					strSubSide.Append(m_strDefGrade1);
				/*}*/
			}
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	}
	else if(m_strdefJudge1 == _T("AO54")/*TFTBURR*/||m_strdefJudge1 == _T("AO55")||m_strdefJudge1 == _T("AO56")||m_strdefJudge1 == _T("AO57"))
	{
		if(pAuto->m_strValue_RankAuto[TFT_BURR_RANK] != _T("")  )
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_BURR_RANK]);
			m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[TFT_BURR_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
		
	}
	else if(m_strdefJudge1 == _T("AO50")/*CFBURR*/||m_strdefJudge1 == _T("AO51")||m_strdefJudge1 == _T("AO52")||m_strdefJudge1 == _T("AO53"))
	{
		if(pAuto->m_strValue_RankAuto[CF_BURR_RANK] != _T("") )
		{			
			strSubSide.Append(pAuto->m_strValue_RankAuto[CF_BURR_RANK]);
			m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[CF_BURR_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	}		
	else if(m_strdefJudge1 == _T("AO83")/*TFTSCRATCH*/) 	
	{
		if(pAuto->m_strValue_RankAuto[TFT_SCARCH_RANK] != _T("") )
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_SCARCH_RANK]);
			m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[TFT_SCARCH_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	}
	else if(m_strdefJudge1 == _T("AO82") /*CFSCRATCH*/)
	{
		if(pAuto->m_strValue_RankAuto[CF_SCARCH_RANK] != _T(""))
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[CF_SCARCH_RANK]);
			m_str_rankCIMvalue =pAuto->m_strValue_RankAuto[CF_SCARCH_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	
	}
	else if(m_strdefJudge1 == _T("AO87")/*TFTCELLBROKEN*/)
	{
		if(pAuto->m_strValue_RankAuto[TFT_CELL_BROKEN_RANK] != _T(""))
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[TFT_CELL_BROKEN_RANK]);
		    m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[TFT_CELL_BROKEN_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
		
	}
	else if(m_strdefJudge1 == _T("AO86")/*CFCELLBROKEN*/)
	{
		if(pAuto->m_strValue_RankAuto[CF_CELL_BROKEN_RANK]  != _T(""))
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[CF_CELL_BROKEN_RANK]);
			m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[CF_CELL_BROKEN_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	
	}
	else if(m_strdefJudge1 == _T("AO90")/*ETCPOINT*/)
	{
		if(pAuto->m_strValue_RankAuto[ETC_POINT_RANK] != _T(""))
		{
			strSubSide.Append(pAuto->m_strValue_RankAuto[ETC_POINT_RANK]);
			m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[ETC_POINT_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
		
	}
	else if(m_strdefJudge1 == _T("AO91")/*ALIGNFAIL*/)
	{
		if(pAuto->m_strValue_RankAuto[GRADE_ALIGN_FAIL_RANK] != _T(""))
		{ 
			strSubSide.Append(pAuto->m_strValue_RankAuto[GRADE_ALIGN_FAIL_RANK]);
			m_str_rankCIMvalue = pAuto->m_strValue_RankAuto[GRADE_ALIGN_FAIL_RANK];
		}
		else
		{
			//2021.02.04 add by mby
			if(m_strDefGrade1 == JUDGE_A){
				strSubSide.Append(JUDGE_AN); 
			}else{
				strSubSide.Append(m_strDefGrade1);
			}
		}
	}
	else  
	{
		//2021.02.04 add by mby
		if(m_strDefGrade1 == JUDGE_A){
			strSubSide.Append(JUDGE_AN); 
		}else{
			strSubSide.Append(m_strDefGrade1);
		}
	}	
	
	//2018.08.06 QMS LOT TYPE Info add by mby S-
	strSubSide.Append(EXT_COLON);	
	pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_LOT_TYPE1, 4, strVal);
	//strVal.Trim();
	strSubSide.Append(strVal);
	//strSubSide.Append(_T("  "));
	strSubSide.Append(EXT_SEMICOL);
	//2018.08.06 QMS LOT TYPE Info add by mby E-

	// 취득한 디펙 정보를 QMS Data 순서로 입력
	strArrayData.Add(strLineHeader + _T("\n"));
	strArrayData.Add(strLinePanel + _T("\n"));
	strArrayData.Add(strSubSide + _T("\n"));
	strArrayData.Add(strLineDefect);
	// 2018.04.23 Camera Image 추가
	strArrayData.Add(strAlignImage);

	//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\test.tmp"), CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
	{
		if (!cstFile.Open(strQmsFilePath, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
		{
			G_AddLog(3, 0, _T("QMS File Write Error %s : %d"), strQmsFilePath, GetLastError());
			return FALSE;
		}

		for(int i = 0; i < strArrayData.GetCount(); i++)
		{
			strLine = strArrayData.GetAt(i);
			cstFile.WriteString(strLine);
		}

		cstFile.Close();
		G_AddLog(3, 3, _T("QMS File Write %s"), strQmsFilePath);
	}
	
	//m_strPolPositionEQ_CV2  = _T("");
	//m_strPolPositionStage_CV2 = _T("");

	G_AddLog(3, 3, _T("m_fnMakeQmsData_AVL() - End"));

	bIsGradeA = FALSE;
	bIsGradeB = FALSE;
	bIsGradeC = FALSE;
	m_bPaCheckFlag		= FALSE;		//2020.04.23 add by mby
	m_bDcc_Check1_flag	= FALSE;		 //2020.04.10 add by mby
	m_bDcc_Check2_flag	= FALSE;		 //2020.04.10 add by mby

	return nRetVal;	//2015.09.17 change bRetVal -> nRetVal
}

BOOL CDataManager::m_fnMakeQmsData_ABI(CString strFilePath, CString &strQmsFilePath, BOOL &bLowerEqpBypass, int nPanelCnt)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsData_ABI() - Start"));
	BOOL bRetVal = FALSE, bPanelCodeTest1 = FALSE, bPanelCodeTest2 = FALSE;
	CStdioFile cstFile;
	int nKvCntVal = 0, nKhCntVal = 0, nKsCntVal = 0;
	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnMakeQmsData_ABI QMS File Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}

	m_nCfNgCnt		= 0;
	m_nTftNgCnt		= 0;
	m_nPenetNgCnt	= 0;

	m_stDefType.initDefType();
	m_stDefType_UI.initDefType();	//2019.01.17 add by mby

	CStringArray strArrayData;
	CString strLine, strItem, strNewLine ,strTemp;
	CString strLineHeader, strLinePanel, strLineSideInfo, strLineDefect;
	CString strLinePolDist= _T(""), strLineEdgeDist= _T(""), strLineLuminance= _T("");
	CString strLineLuminanceCF= _T(""), strLineLuminanceTFT= _T(""), strLineLuminancePOL = _T("");;
	CString strLinePolDistCF= _T(""), strLineEdgeDistCF= _T("");
	CString strLinePolDistTFT= _T(""), strLineEdgeDistTFT= _T("");
	CString strSubSide, strIunsentPath = _T("");
	CString check_date= _T("");
	CString strVal = _T(""), strDefType = _T("");	
	CString strQmsFileName = _T("");
	int nDefCnt = 0, nMesDstCnt = 0, nMesAngleCnt = 0;
	long imgX = 0, imgY = 0;
	int curPos = 0 , nAlignToMarkCnt = 0 , nLumitCnt;
	CString strCnt ;
	nAlignToMarkCnt = 1;
	nLumitCnt = 1;
	BOOL bimgRetVal = FALSE;
	int nImgRetCnt = 0;
	CString cDefectVision = _T(""), CPanelId, strImgFilePath, strFileName, strImgName, strQmsImgName;
	CString sTempTst;
	CString strRetVal;
	CString strStageCnt;
	//2015.08.15
	CString strDefFlag = _T(""), strDefSizeX = _T(""), strDefSizeY = _T(""), strDefLumi = _T("");
	DOUBLE dTempVal1, dTempVal2, dDefThinkness, dDefLength, dDefDistance;
	int nDefFlag = 0, nDefLumi = 0, nDefType = 0;
	int nEqpDirection = 0;
	for (int i = 0; i < MAX_DEFECT_KIND; i++)
		m_stPanelJudgeInfo[i].init_info();
	//2015.08.15

	//2016.03.02
	STRU_LARGE_RECEIVE_LUMINANCE_DATA stLuminanceData;	stLuminanceData.init_info();
	int nLumiVal = 0;
	CString strLumiVal = _T("");
	//2016.03.02

	//2017.01.21		compare to.... POL Substance   Same Defect    Cf, Tft coordination
	//CStringArray strArrayCoax;
	//CStringArray strArrayTdi;
	//CString strVisionNo;
	//int nVisionNo;
	//2017.01.21

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);  //180811

	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;   stAlignMeasureInfo.init_info();

	STRU_IMAGE_INFO			stImgInf;			  stImgInf.init(); stImgInf.nQmsImgNoCnt.init();

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("HEADERDATA"))
		{
			strLineHeader = m_fnMakeQmsDataHead(strLine);
		}	

		if(strItem ==  _T("PANELDATA"))
		{
			strLinePanel = m_fnMakeQmsDataPanel(strLine);
			CPanelId = m_fnGetItemData(3, strLine);
			_tcscpy(stImgInf.tImgPnlID , CPanelId);
		}	

		//2018.04.16 UI Measure NG/OK
		if (m_stDefType.CfMeasureFail)
			pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)0, (LPARAM)(LPCTSTR)CPanelId);
		if (m_stDefType.TftMeasureFail)
			pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)1, (LPARAM)(LPCTSTR)CPanelId);

		if(strItem ==  _T("DEFECT"))			//Defect 좌표 수정부 작업 예정
		{
			//bRetVal = TRUE;

			cDefectVision = m_fnGetItemData(FLAG + 1, strLine);
			//if (_ttoi(cDefectVision) == POL) m_stDefType.PenetNgCnt++;//m_fnSetPenetNgCnt();

			if(_ttoi(cDefectVision) == POL )  
			{
				strLine = m_fnSetItemData(D_FLAG, strLine, _T("0"), 1 , D_ITEM_END - 1);
				cDefectVision.Format(_T("%d"), CF);
			}	

			//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(_ttoi(cDefectVision), CPanelId);
			pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)_ttoi(cDefectVision), (LPARAM)(LPCTSTR)CPanelId);

			if (_ttoi( m_fnGetItemData(D_DEFECT_NO - 1, strLine)) <= 999)	//2015.05.27 edit 999 -> 9999
			{
				if		(_ttoi(cDefectVision) == CF)  m_stDefType.CfNgCnt++;//m_fnSetCfNgCnt();
				else if (_ttoi(cDefectVision) == TFT) m_stDefType.TftNgCnt++;//m_fnSetTftNgCnt();
				else								  m_stDefType.CfNgCnt++;
				strDefType = m_fnGetItemData(DEFECT_TYPE + 1, strLine);
				//2015.08.15
				strDefFlag = m_fnGetItemData(FLAG + 1, strLine);
				strDefSizeX = m_fnGetItemData(IMG_SIZE_X + 1, strLine);
				strDefSizeY = m_fnGetItemData(IMG_SIZE_Y + 1, strLine);
				strDefLumi = m_fnGetItemData(LUMINANCE + 1, strLine);
				dTempVal1 = _tstof((LPCTSTR)strDefSizeX);
				dTempVal2 = _tstof((LPCTSTR)strDefSizeY);

				if (dTempVal1 < dTempVal2)
				{
					dDefDistance = dTempVal2;
					dDefLength = dTempVal2;
					dDefThinkness = dTempVal1;
				}
				else
				{
					dDefDistance = dTempVal1;
					dDefLength = dTempVal1;
					dDefThinkness = dTempVal2;
				}

				nDefFlag = _tstoi((LPCTSTR)strDefFlag);
				nDefLumi = _tstoi((LPCTSTR)strDefLumi);
				nDefType = _tstoi((LPCTSTR)strDefType);
				m_fnPanelJudgeCheck(nDefType, nDefFlag, dDefDistance, dDefLength, dDefThinkness, nDefLumi);

				////2017.01.21	compare to.... POL Substance   Same Defect    Cf, Tft coordination
				//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
				//{
				//	strVisionNo = m_fnGetItemData(CAMERA_NO + 1, strLine);
				//	nVisionNo = _tstoi((LPCTSTR)strVisionNo);
				//
				//	if((nDefType == INSP_POL_ALIEN_SUBSTANCE) 
				//		&& (nVisionNo == VISION_PC_1 || nVisionNo == VISION_PC_2 || nVisionNo == VISION_PC_5 || nVisionNo == VISION_PC_6))
				//		strArrayCoax.Add(strLine + _T("\n"));
				//	else if((nDefType == INSP_POL_ALIEN_SUBSTANCE) 
				//		&& (nVisionNo == VISION_PC_7 || nVisionNo == VISION_PC_8 || nVisionNo == VISION_PC_9))
				//		strArrayTdi.Add(strLine + _T("\n"));
				//}
				////2017.01.21

				//2015.08.15
				/*if (strDefType)
				{*/
				switch (_ttoi(strDefType))
				{					
					//case INSP_DUST: case INSP_WHITE_TURBIDITY:	case INSP_ITO:	case INSP_EDGE_BROKEN: case INSP_PAD_SCRATCH: case INSP_DIMPLE_POINT:	
					//case INSP_ETCHING_LINE:	case INSP_DIMPLE_LINE:	case INSP_PAD_BURNT:	case INSP_BURR:
					//case INSP_CF_BURR:	case INSP_TFT_BURR:
					//case INSP_CF_BROKEN:	case INSP_TFT_BROKEN:	case INSP_CF_CORNER_BROKEN:	case INSP_TFT_CORNER_BROKEN:
					//case INSP_CF_CELL_BROKEN:	case INSP_TFT_CELL_BROKEN:	//2015.08.26
				case INSP_POL_BUBBLE: case INSP_POL_SCRATCH: case INSP_POL_ALIEN_SUBSTANCE:
					//KH 편광판 이물 결함 데이터
					nKhCntVal++;
					strImgName = m_fnGetItemData(DEFECT_IMG_NO + 1 , strLine);
					stImgInf.nDefType = _ttoi(m_fnGetItemData(DEFECT_TYPE + 1, strLine));
					stImgInf.nDefFlag = _ttoi(m_fnGetItemData(FLAG + 1, strLine));	//2018.01.22
					m_fnRenameQmsImgFile( stImgInf, strImgName, nKhCntVal);		
					strImgFilePath = stImgInf.tImgPath;
					strImgFilePath.Append(strImgName);
					strLineDefect += m_fnMakeQmsDataDefect_KH(strLine, nKhCntVal);
					//strLine = m_fnSetItemData(D_DEF_IMG_NO , strLine, m_strQmsImgName, m_strQmsImgName.GetLength(), D_ITEM_END - 1);
					strImgFilePath = m_fnSetItemMutiData(3 , strImgFilePath, _T("i_send") , 6, 8 - 1, _T("\\"));	
					strLine.Append(EXT_COLON); strLine.Append(strImgFilePath); 
					break;

					//2015.06.04 add by IKM - S 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
				case INSP_POL_MINOR: case INSP_MURA:
					nKsCntVal ++;
					strImgName = m_fnGetItemData(DEFECT_IMG_NO + 1 , strLine);
					stImgInf.nDefType = _ttoi(m_fnGetItemData(DEFECT_TYPE + 1, strLine));
					stImgInf.nDefFlag = _ttoi(m_fnGetItemData(FLAG + 1, strLine));	//2018.01.22
					m_fnRenameQmsImgFile( stImgInf, strImgName, nKsCntVal);
					strImgFilePath = stImgInf.tImgPath;
					strImgFilePath.Append(strImgName);
					strLineDefect += m_fnMakeQmsDataDefect_KS(strLine, nKsCntVal);
					//strLine = m_fnSetItemData(D_DEF_IMG_NO , strLine, m_strQmsImgName, m_strQmsImgName.GetLength(), D_ITEM_END - 1);
					strImgFilePath = m_fnSetItemMutiData(3 , strImgFilePath, _T("i_send") , 6, 8 - 1, _T("\\"));	
					strLine.Append(EXT_COLON); strLine.Append(strImgFilePath); 
					break;
				default:
					nKvCntVal ++;
					strImgName = m_fnGetItemData(DEFECT_IMG_NO + 1 , strLine);
					stImgInf.nDefType = _ttoi(m_fnGetItemData(DEFECT_TYPE + 1, strLine));
					stImgInf.nDefFlag = _ttoi(m_fnGetItemData(FLAG + 1, strLine));	//2018.01.22
					m_fnRenameQmsImgFile( stImgInf, strImgName, nKvCntVal);
					strImgFilePath = stImgInf.tImgPath;
					strImgFilePath.Append(strImgName);
					strLineDefect += m_fnMakeQmsDataDefect_KV(strLine, nKvCntVal);
					//strLine = m_fnSetItemData(D_DEF_IMG_NO , strLine, m_strQmsImgName, m_strQmsImgName.GetLength(), D_ITEM_END - 1);	
					strImgFilePath = m_fnSetItemMutiData(3 , strImgFilePath, _T("i_send") , 6, 8 - 1, _T("\\"));	
					strLine.Append(EXT_COLON); strLine.Append(strImgFilePath); 
					break;
					//2015.06.04 add by IKM - E
				}
				//}

				nDefCnt ++;
			}		
		}	

		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF73"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 8);
			strLineEdgeDistCF = (strItem);

		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT73"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 8);
			strLineEdgeDistTFT = (strItem);
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF74"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 8);
			strLinePolDistCF = (strItem);		
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT74"))
		{
			strItem = strLine;
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 8);
			strLinePolDistTFT = (strItem);		
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF75"))
		{
			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			{
				strItem = strLine;
				m_fnLuminanceSet(strItem, CPanelId);
			}
			else if (pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 4);
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
				strLineLuminanceCF = strItem;
			}
			else
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 4);
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
				strLineLuminanceCF = strItem;
			}
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT75"))
		{
			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			{
				strItem = strLine;
				m_fnLuminanceSet(strItem, CPanelId);
			}
			else if (pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 4);
				strItem = strItem.Mid(0, strItem.ReverseFind(_T(',')));
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
				strLineLuminanceTFT = strItem + EXT_SEMICOL;
			}
			else
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 4);
				//strItem = strItem.Mid(0, strItem.ReverseFind(_T(',')));
				//strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
				strLineLuminanceTFT = strItem ;//+ EXT_SEMICOL;
			}
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("POL76"))
		{
			if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
			{
				strItem = strLine;
				m_fnLuminanceSet(strItem, CPanelId);		// 대형 현재 아직 작업 안됨 .  2016.07.12
			}
			else
			{
				strItem = strLine;
				strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);

				strLineLuminancePOL = strItem + EXT_SEMICOL;
			}
		}	

		//2016.11.21	Mr.Chu
		strItem = m_fnGetItemData(LAYER_NO - 1, strLine);
		if( !strItem.Compare(_T("TFT77")) || !strItem.Compare(_T("CF77")) || !strItem.Compare(_T("POL78")) )
		{
			CString strPanelData = _T("");
			int nStringCnt = 0,nDefectNum = 0;

			G_AddLog(3, 2, _T("Luminance Code [%s] : %s"),CPanelId, strLine);		//Data 표기

			strPanelData = strLine;
			strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);
			strPanelData.TrimRight();

			nStringCnt = strPanelData.Replace(EXT_COLON,EXT_COLON) + 1;

			for(int i=0;i<nStringCnt;i++)
			{
				strTemp = m_fnGetItemData(LAYER_NO - 1, strPanelData);
				strTemp.Trim();
				strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);

				if( strTemp == _T("0") )
					continue;

				nDefectNum = _ttoi(strTemp);
				switch(nDefectNum)
				{
				case CFCLUMINANCE:
					m_stDefType.CFCLuminance++;
					break;
				case CBCLUMINANCE:
					m_stDefType.CBCLuminance++;
					break;
				case CFSLUMINANCE:
					m_stDefType.CFSLuminance++;
					break;
				case CBSLUMINANCE:
					m_stDefType.CBSLuminance++;
					break;
				case TFCLUMINANCE:
					m_stDefType.TFCLuminance++;
					break;
				case TBCLUMINANCE:
					m_stDefType.TBCLuminance++;
					break;
				case TFSLUMINANCE:
					m_stDefType.TFSLuminance++;
					break;
				case TBSLUMINANCE:
					m_stDefType.TBSLuminance++;
					break;
				case TDILUMINANCE:
					m_stDefType.TDILuminance++;
					break;
				default:
					break;
				}
			}
		}
		//

		strArrayData.Add(strLine + _T("\n"));

	}

	cstFile.Close();

	if (!cstFile.Open(strFilePath, CFile::modeWrite | CFile::typeText, NULL))
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\PanelID1_V1_00.txt"), CFile::modeRead | CFile::typeText, NULL))	
	{
		G_AddLog(3, 2, _T("m_fnMakeQmsData_ABI2 QMS File Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}
	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	strArrayData.RemoveAll();
	cstFile.Close();


	// 중복 좌표 함수 기입///////////////////////////////////////////////////////////////////////
	m_fnCompareOverlapDefTypeCheck( CPanelId,strFilePath,m_stDefType);
	/////////////////////////////////////////////////////////////////////////////////////////////

	CString strPanelID1, strPanelID2, cCountRoot = _T("") , cCountVal = _T("");
	int nGradeCnt;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);

	//2015.07.07 add by IKM for Display PG Judge - S
	if (m_strSemaGrade.Find(_T("ITO_NG")) != -1)
	{
		m_stDefType.CfNgCnt++;
		pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)2, (LPARAM)(LPCTSTR)CPanelId);
		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(_T("2"), CPanelId);
		//bRetVal = TRUE;
	}
	//2015.07.07 add by IKM for Display PG Judge - E

	if (!nDefCnt)
	{		
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, cCountVal, cCountRoot);
		pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)-1, (LPARAM)(LPCTSTR)CPanelId);
		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(_T("-1"), CPanelId);
	}
	else
	{
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, cCountVal, cCountRoot);
	}

	//// 패널 부가 정보 
	//strSubSide.Append(_T("30"));
	//strSubSide.Append(EXT_COLON);
	//for(int i = 0 ; i < 70; i ++)
	//{

	//	switch(i)
	//	{
	//	case 60:
	//		strSubSide.Append(IntToCstr(m_stDefType.polSubstance));
	//		strSubSide.Append(EXT_COLON);
	//		break;
	//	case 61:
	//		strSubSide.Append(IntToCstr(m_stDefType.CfPolBubble + m_stDefType.TftPolBubble));
	//		strSubSide.Append(EXT_COLON);
	//		break;
	//	case 62:
	//		strSubSide.Append(IntToCstr(m_stDefType.CfPolScrach + m_stDefType.TftPolScrach));
	//		strSubSide.Append(EXT_COLON);
	//		break;
	//	default:
	//		strSubSide.Append(_T("0"));
	//		strSubSide.Append(EXT_COLON);
	//		break;
	//	}
	//}
	//strSubSide.Append(IntToCstr(m_stDefType.CfWhiteBlur));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr(m_stDefType.ItoPeel));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr(m_stDefType.WanesBroken+ m_stDefType.CfCornerBroken + m_stDefType.CfBroken + m_stDefType.TftBroken + m_stDefType.TftCornerBroken + m_stDefType.CfCellBroken + m_stDefType.TftCellBroken));	//2015.08.26
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr(m_stDefType.SocketScrach));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr((m_stDefType.CfDimplePoint + m_stDefType.TftDimplePoint)));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr((m_stDefType.CfEdgeilegal + m_stDefType.TftEdgeilegal )));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr((m_stDefType.CfDimpleLine + m_stDefType.TftDimpleLine) + (m_stDefType.TftScrach + m_stDefType.CfScrach)));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr(m_stDefType.SocketBurn));
	//strSubSide.Append(EXT_COLON);
	//strSubSide.Append(IntToCstr(m_stDefType.Burr + m_stDefType.CfBurr + m_stDefType.TftBurr));
	//strSubSide.Append(EXT_COLON);
	//for(int i = 0 ; i < 20 ; i ++)
	//{
	//	strSubSide.Append(_T("0"));
	//	strSubSide.Append(EXT_COLON);
	//}

	//// 2016.08.27 add by sgkim - 소형 QMS 데이터에 스테이지 정보 기입. 1또는 2
	//
	//if(!nPanelCnt)
	//	strStageCnt.Format(_T("%d"),2);
	//else
	//	strStageCnt.Format(_T("%d"),1);
	//strSubSide.Append(strStageCnt); 
	//strSubSide.Append(EXT_COLON);
	m_fnMakeQmsDataDefCnt_ABI(strSubSide);	//2018.01.22
	
	//2015.08.19 s-
	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE || pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE)
	{
		stAlignMeasureInfo = m_fnReadAlignMeasureData(CPanelId);

		strLineEdgeDistCF = stAlignMeasureInfo.cCFMeasureDistanceData;
		if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())	strLineEdgeDistCF = _T("0,0,0,0,0,0,0,0,");
		strLineEdgeDistTFT = stAlignMeasureInfo.cTFTMeasureDistanceData;
		if (strLineEdgeDistTFT == _T("")|| strLineEdgeDistTFT.IsEmpty())	strLineEdgeDistTFT = _T("0,0,0,0,0,0,0,0");
		strLineEdgeDist.Format(_T("%s%s"),strLineEdgeDistCF, strLineEdgeDistTFT);
		strSubSide.Append(strLineEdgeDist);
		strSubSide.Append(EXT_COLON);


		strLinePolDistCF = stAlignMeasureInfo.cCFMeasureAngleData;
		if (strLinePolDistCF == _T("")|| strLinePolDistCF.IsEmpty())	strLinePolDistCF = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,");
		strLinePolDistTFT = stAlignMeasureInfo.cTFTMeasureAngleData;
		if (strLinePolDistTFT == _T("")|| strLinePolDistCF.IsEmpty())	strLinePolDistTFT = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");
		strLinePolDist.Format(_T("%s%s"),strLinePolDistCF, strLinePolDistTFT);
		strSubSide.Append(strLinePolDist);
		strSubSide.Append(EXT_COLON);
		//2015.11.09 ADD

		//if (strLineLuminanceCF == _T("") || strLineLuminanceCF.IsEmpty())
		//{
		//	strLineLuminanceCF = (_T("0,0,0,0,"));
		//}
		//if (strLineLuminanceTFT == _T("") || strLineLuminanceTFT.IsEmpty())
		//{
		//	strLineLuminanceTFT = (_T("0,0,0,0;"));
		//}
		//strLineLuminance.Format(_T("%s%s"),strLineLuminanceCF, strLineLuminanceTFT);

		//2016.03.02
		stLuminanceData = m_fnReadLuminanceData(CPanelId);

		int nLumiBuf = NULL;
		for(int i = 0; i < MAX_LUMINANCE_DATA; i++)
		{
			if( i==6 ) {	
				nLumiBuf = (stLuminanceData.nLuminanceLimitData[i] + stLuminanceData.nLuminanceLimitData[i+1]) / 2;	
				continue;	
			}

			if(	i == 7 )
				nLumiVal = nLumiBuf;
			else
				nLumiVal = stLuminanceData.nLuminanceLimitData[i];

			strLumiVal.Format(_T("%d"),nLumiVal);

			//if(i == 8)
			//	strLineLuminance.Append(strLumiVal + EXT_SEMICOL);
			//else
				strLineLuminance.Append(strLumiVal + EXT_COLON);
		}

		strSubSide.Append(strLineLuminance);		

		//bLowerEqpBypass = m_fnGetLowerBypassNgFlag_ABI();	//2016.05.28 Mr.Chu		Defect 여부에 따라 Lower Bypass 여부 결정
	}
	//else if(pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE){
	//	G_AddLog(3, 2, _T("m_fnMakeQmsData_ABI EQP_MONITOR_SIZE"));
	//	Sleep(1000);
	//	stAlignMeasureInfo = m_fnReadAlignMeasureData(CPanelId);

	//	strLineEdgeDistCF = stAlignMeasureInfo.cCFMeasureDistanceData;
	//	if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())	strLineEdgeDistCF = _T("0,0,0,0,0,0,0,0,");
	//	//strLineEdgeDistCF = _T("0,0,0,0,0,0,0,0,");
	//	strLinePolDistCF = stAlignMeasureInfo.cCFMeasureAngleData;
	//	if (strLinePolDistCF == _T("")|| strLinePolDistCF.IsEmpty())	strLinePolDistCF = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");	
	//	//strLinePolDistCF = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");

	//	strLineEdgeDist.Format(_T("%s%s"),strLineEdgeDistCF, strLinePolDistCF);
	//	strSubSide.Append(strLineEdgeDist);
	//	strSubSide.Append(EXT_COLON);

	//	strLineEdgeDistTFT = stAlignMeasureInfo.cTFTMeasureDistanceData;
	//	if (strLineEdgeDistTFT == _T("")|| strLineEdgeDistTFT.IsEmpty())	strLineEdgeDistTFT = _T("0,0,0,0,0,0,0,0,");
	//	//strLineEdgeDistTFT = _T("0,0,0,0,0,0,0,0,");
	//	strLinePolDistTFT = stAlignMeasureInfo.cTFTMeasureAngleData;
	//	if (strLinePolDistTFT == _T("")|| strLinePolDistTFT.IsEmpty())	strLinePolDistTFT = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");
	//	//strLinePolDistTFT = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");

	//	strLinePolDist.Format(_T("%s%s"),strLineEdgeDistTFT, strLinePolDistTFT);
	//	strSubSide.Append(strLinePolDist);
	//	strSubSide.Append(EXT_COLON);
	//	//2015.11.09 ADD
	//	
	//	//2016.03.02
	//	stLuminanceData = m_fnReadLuminanceData(CPanelId);

	//	for(int i = 0; i < MAX_LUMINANCE_DATA; i++)
	//	{
	//		nLumiVal = stLuminanceData.nLuminanceLimitData[i];

	//		if(i == 6 || i == 7)
	//			nLumiVal = (stLuminanceData.nLuminanceLimitData[i] + stLuminanceData.nLuminanceLimitData[i+1]) / 2;

	//		strLumiVal.Format(_T("%d"),nLumiVal);

	//		if(i == 7)
	//			strLineLuminance.Append(strLumiVal + EXT_SEMICOL);
	//		else
	//			strLineLuminance.Append(strLumiVal + EXT_COLON);
	//	}
	//	//

	//	strSubSide.Append(strLineLuminance);
	//}
	else
	{

		if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())
		{
			strLineEdgeDistCF = (_T("0,0,0,0,0,0,0,0,"));
		}
		if (strLineEdgeDistTFT == _T("") || strLineEdgeDistTFT.IsEmpty())
		{
			strLineEdgeDistTFT = (_T("0,0,0,0,0,0,0,0,"));
		}

		if (strLinePolDistCF == _T("") || strLinePolDistCF.IsEmpty())
		{
			strLinePolDistCF = (_T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,"));
		}
		if (strLinePolDistTFT == _T("") || strLinePolDistTFT.IsEmpty())
		{
			strLinePolDistTFT = (_T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,"));
		}
		if (strLineLuminanceCF == _T("") || strLineLuminanceCF.IsEmpty())
		{
			strLineLuminanceCF = (_T("0,0,0,0,"));
		}
		if (strLineLuminanceTFT == _T("") || strLineLuminanceTFT.IsEmpty())
		{
			strLineLuminanceTFT = (_T("0,0,0,"));
		}
		if (strLineLuminancePOL == _T("") || strLineLuminancePOL.IsEmpty())
		{
			strLineLuminancePOL = (_T("0;"));
		}
		strLineEdgeDist.Format(_T("%s%s"),strLineEdgeDistCF, strLineEdgeDistTFT);
		//strSubSide.Append(strLineEdgeDistCF);
		strSubSide.Append(strLineEdgeDist);
		strLinePolDist.Format(_T("%s%s"),strLinePolDistCF, strLinePolDistTFT);
		strSubSide.Append(strLinePolDist);
		strLineLuminance.Format(_T("%s%s%s"),strLineLuminanceCF, strLineLuminanceTFT,strLineLuminancePOL);
		strSubSide.Append(strLineLuminance);		
	}

	m_strCimMeasureDataTemp = strLineEdgeDist;	//2016.01.15

	//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgDefInfo->m_fnUpdateDefType(m_stDefType);
	//pAuto->m_pDlgDefInfo->SendMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnUpdatePanelSummary(strQmsFilePath);

	//2015.10.06
	CString strMeasureData = _T("");
	strMeasureData.Format(_T("%s,%s"),strLineEdgeDist, strLinePolDist);
	((CViewAuto*)pFrame->m_pViewAuto)->SendMessage(WM_UI_CONTROL_SAV_MEASURE_DAT, (WPARAM)EQP_TYPE_ABI, (LPARAM)(LPCTSTR)strMeasureData);	
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnSaveFileMeasureInfo(strMeasureData, EQP_TYPE_ACS);
	//2015.10.06

	//2017.01.21		compare to.... POL Substance   Same Defect    Cf, Tft coordination
	//if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	//{
	//	bRetVal = m_fnSerachPolAlienSubstance(strArrayCoax, strArrayTdi);
	//	if(bRetVal = TRUE)
	//		m_stDefType.polSubstance = TRUE;
	//	else
	//		m_stDefType.polSubstance = FALSE;
	//}
	//2017.01.21

	bRetVal = m_fnGetJudgeGrade_ABI(bPanelCodeTest1, m_strDefGrade1, m_strdefJudge1);
	m_fnGetJudgeGrade_ABI(bPanelCodeTest2, m_strDefGrade2, m_strdefJudge2);

	//if(pAuto->m_pDlgPanelCodeTest)	//2016.09.14	Mr.Chu		// 해당 UI 가 소멸시 실행되지 않도록 예외처리	( 왕빙 요청기능 )
	{

		int nPanelCode = 0;
		if(bPanelCodeTest1){
			nPanelCode = m_fnPanelCodeTest(m_strdefJudge1);

			if(nPanelCode)
			{
				m_strDefGrade1 = JUDGE_D;
				m_strdefJudge1.Format(_T("%d"), nPanelCode);
				bRetVal = TRUE;
			}
		}
		else if(bPanelCodeTest2){
			nPanelCode = m_fnPanelCodeTest(m_strdefJudge2);

			if(nPanelCode)
			{
				m_strDefGrade2 = JUDGE_D;
				m_strdefJudge2.Format(_T("%d"), nPanelCode);
			}
		}

	}

	strLinePanel = m_fnSetItemData(3 , strLinePanel, m_strDefGrade1, m_strDefGrade1.GetLength() , P_ITEM_END - 1);
	strLinePanel = m_fnSetItemData(4 , strLinePanel, m_strdefJudge1, m_strdefJudge1.GetLength() , P_ITEM_END - 1);
	strLinePanel = m_fnSetItemData(5 , strLinePanel, m_strdefJudge2, m_strdefJudge2.GetLength() , P_ITEM_END - 1);

	//2015.09.10 Mr.Chu
	strQmsFileName = m_fnMakeQmsFileName();
	if(!pFrame->m_stParamInfo.bUseManualJudge || !bRetVal && pFrame->m_stParamInfo.bUseManualJudge)  //2019.01.16 by ryu check
	{
		strQmsFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strQmsFileName;
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultGrade(m_strDefGrade1);
		((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultDefectCodeJudge(m_strdefJudge1);
		pAuto->SendMessage(WM_UI_CONTROL_PNL_SUMMARY_LIST, (WPARAM)0, (LPARAM)(LPCTSTR)strQmsFilePath);
	}
	else
		strQmsFilePath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Temp + strQmsFileName;
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnUpdatePanelSummary(strQmsFilePath);
	CString strRecipeCode;

	if(strRecipeNo == _T("2")) { strRecipeCode = _T("A1CC495PU1L01"); }
	else if(strRecipeNo == _T("4")) { strRecipeCode = _T("A1CC575PU1L01"); }
	else if(strRecipeNo == _T("6")) { strRecipeCode = _T("A1CC575PU2L01"); }
	else if(strRecipeNo == _T("8")) { strRecipeCode = _T("A1CC575PU3L01"); }

	strSubSide.Append(strRecipeCode);
	strSubSide.Append(EXT_COLON);
	// 2018.06.20 Rank 판정 추가
	strSubSide.Append(m_strDefGrade1);

	//2018.08.06 QMS LOT TYPE Info add by mby S-  RYU 180811
	strSubSide.Append(EXT_COLON);	
	pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_LOT_TYPE1, 4, strVal);
	//strVal.Trim();
	strSubSide.Append(strVal);
	//strSubSide.Append(_T("  "));
	strSubSide.Append(EXT_SEMICOL);

	// 취득한 디펙 정보를 QMS Data 순서로 입력
	strArrayData.Add(strLineHeader + _T("\n"));
	strArrayData.Add(strLinePanel + _T("\n"));
	strArrayData.Add(strSubSide + _T("\n"));
	strArrayData.Add(strLineDefect);

	if(pFrame->m_stParamInfo.nEqpObject != P_PROJECT)
	{
		//if (!cstFile.Open(_T("D:\\QMS TEST\\test\\test.tmp"), CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
		if (!cstFile.Open(strQmsFilePath, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Write Error %s : %d"), strQmsFilePath, GetLastError());
			return FALSE;
		}

		for(int i = 0; i < strArrayData.GetCount(); i++)
		{
			strLine = strArrayData.GetAt(i);
			cstFile.WriteString(strLine);
		}

		cstFile.Close();
	}

	G_AddLog(3, 3, _T("m_fnMakeQmsData_ABI() - End"));
	return bRetVal;
}

CString CDataManager::m_fnMakeQmsDataHead(CString strLine)		
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataHead() - Start"));
	CString strRetVal, strStepId, strOperId;
	CString strTemp , check_date, strVal;
	CString strDate, strTime;
	int curPos, nDefCnt, nEqpModel, nEqpNo;
	//TCHAR tPnlID[5];
	TCHAR chRead[255] = {0};
	//	strNewLine = //m_fnSetItemData(D_DEFECT_NO, strLine, strItem, LEN_DEFECT_NO, 33);
	check_date = strLine ;//
	curPos = 0;
	//char chValue[100];
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	int nn=0;
	strVal  = check_date.Tokenize(EXT_COLON, curPos);
	CTime time = CTime::GetCurrentTime();
	strTemp = _T("");
	nDefCnt = 0;
	m_strQmsSendFileDate = _T("");
	while (TRUE)
	{				
		strVal = check_date.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0: break;
		case 1:
			strTemp.Append(_T("00"));
			strTemp.Append(EXT_COLON);
			break;
		case 5:
			strDate = time.Format(_T("%Y/%m/%d"));	//2020.06.25 add by mby
			//strDate = strVal;						//2020.06.25 add by mby
			m_strQmsSendFileDate.Append(strDate.Mid(0,4));			
		    m_strQmsSendFileDate.Append(strDate.Mid(5,2));		 
			m_strQmsSendFileDate.Append(strDate.Mid(8,2));			

			strTemp.Append(strDate);
			strTemp.Append(EXT_COLON);
			break;
		case 6:
			strTime = time.Format(_T("%H:%M:%S"));	//2020.06.25 add by mby
			//strTime = strVal;						//2020.06.25 add by mby
			m_strQmsSendFileDate.Append(strTime.Mid(0,2));
			m_strQmsSendFileDate.Append(strTime.Mid(3,2));
			m_strQmsSendFileDate.Append(strTime.Mid(6,2));

			strTemp.Append(strTime);
			strTemp.Append(EXT_COLON);
			break;
		case 7:
			nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
			if (!nEqpModel)			
				strVal.Format(_T("%3s"), _T("401"));			
			else
				strVal.Format(_T("%3s"), _T("403"));		

			strTemp.Append(strVal + EXT_COLON);
			break;
		case 8:
			nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_STATE_EQP_NO, 0);
			strVal.Format(_T("%3d"), nEqpNo);
			strTemp.Append(strVal + EXT_COLON);
			break;
		case 9:
			memset(chRead,0X00,sizeof(chRead));	
			pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);

			strStepId  = chRead;
			strVal = strStepId;

			strTemp.Append(strVal + EXT_COLON);
			break;
		case 10:
			strOperId.Format(_T("%6s"), strVal);
			strTemp.Append(strVal + EXT_COLON);
			break;
			//strVal = (_T("AAAAAA"));			
		case 11://2018.12.10 QMS LOT TYPE Info add by mby
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_LOT_TYPE1, 4, strVal);			
			strTemp.Append(strVal.TrimRight() + EXT_SEMICOL);
			break;
		default:
			strTemp.Append(strVal);
			strTemp.Append(EXT_COLON);
			break;
		}
		if (nDefCnt == 11)	//if(strVal.IsEmpty())
		{
			break;
		}
		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataHead() - End"));
	return	strRetVal = strTemp;
}

CString CDataManager::m_fnMakeQmsDataPanel(CString strLine)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataPanel() - Start"));
	CString strRetVal, strPanelId, strJudge, strRcpId;
	CString strTemp , check_date, strVal;
	int curPos, nDefCnt ,nlength =0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	STRU_POL_MARK_INFO stPolMarkInfo;	//2017.03.21
	CString strCfResult, strTftResult, strCfPolMakerNo, strTftPolMakerNo;
	check_date = strLine ;//
	curPos = 0;
	int nn = 0;
	strVal  = check_date.Tokenize(EXT_COLON, curPos);

	int nEqpType = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);	//0:AVL 1:ABI

	//2018.03.09
	int nRecipeNo = pFrame->m_iniRecipe.GetProfileInt(RECIPE_INFO,RECIPE_CURRENT_NO,0);

	//TCHAR strPnlId[18];
	strTemp = _T("");
	nDefCnt = 0;
	while (TRUE)
	{				
		if (curPos == -1)
		{
			break;
		}
		strVal = check_date.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case P_PANELITEM: //case P_PANEL_COUNT:
			break;
		case P_LAYER_NO:
			strTemp.Append(_T("13"));
			strTemp.Append(EXT_COLON);
			break;
		case P_PANEL_ID:
			///CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());				
			//memset(strPnlId, 0x00, sizeof(strPnlId));
			//lstrcpy(strPnlId, strVal);
			//char chValue[100];
			//memset(chValue, 0x00, 100);
			//nn = 0;
			//for (int i = 0; i < 18 ; i++)
			//{
			//	chValue[nn++] = strPnlId[i];
			//	//chValue[nn++] = (strPnlId[i]);
			//}
			//strPanelId = chValue;
			//
			//nReqNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
			/*		if(nReqNo == 0 ) 
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_S2_PANEL_ID_1, LEN_PANEL_ID, strPanelId);
			if(nReqNo == 1 ) 
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_S3_PANEL_ID_1, LEN_PANEL_ID, strPanelId);
			strVal = pFrame->m_fnGetPanelID_1(0);*/
			strTemp.Append(strVal + EXT_COLON);
			strPanelId = strVal;
			break;
		case P_PANEL_COUNT:
			strTemp.Append(strVal+ EXT_COLON);
			break;
		case P_JUDGEMENT:
			//strVal.Format(_T("%1s"), strVal);
			//2016.05.11	Mr.Chu
			strJudge.Format(_T("%2s"), strVal);
			if (strJudge.Find(ALIGN_FAIL) != -1)
			{
				m_stDefType.AlignFail++;
			}
			//
			strVal = _T("C");
			strTemp.Append(strVal+ EXT_COLON);
			break;
		case P_JUDGEMENT_CODE1:
			strJudge.Format(_T("%4s"), strVal);
			//strVal = _T("AAAA");

			if (strJudge.Find(MEASUREFAIL_CF) != -1)
			{
				m_stDefType.CfMeasureFail++;
			}
			if (strJudge.Find(MEASUREFAIL_TFT) != -1)
			{
				m_stDefType.TftMeasureFail++;
			}
			strTemp.Append(strVal+ EXT_COLON);
			break;

		case P_JUDGEMENT_CODE2:
			strJudge.Format(_T("%4s"), strVal);
			//strVal = _T("AAAA");
			if (strJudge.Find(MEASUREFAIL_CF) != -1)
			{
				m_stDefType.CfMeasureFail++;
			}
			if (strJudge.Find(MEASUREFAIL_TFT) != -1)
			{
				m_stDefType.TftMeasureFail++;
			}
			strTemp.Append(strVal+ EXT_COLON);
			break;
		case RECIPE_ID:
			//strRcpId.Format(_T("%s"), strVal);


			//strVal = _T("RCPID00000000001");
			strVal.Format(_T("%03d"), nRecipeNo);
			strTemp.Append(strVal+ EXT_COLON);
			break;	

		//2019.01.23 QMS PartNameE Info add by mby
		case PART_NAME:
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_PARTNAME_TYPE1, 4, strVal);			
			strTemp.Append(strVal + EXT_SEMICOL);
			break;	

			//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 S		
		case P_POLMARK_ILEGAL:					
			if (pFrame->m_stParamInfo.nEqpObject != P_PROJECT)	
			{
				if(pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS)
				{
					stPolMarkInfo = m_fnReadPolMarkData(strPanelId);
					strCfResult = stPolMarkInfo.cCFResult;		strCfResult.Trim();
					strTftResult = stPolMarkInfo.cTFTResult;	strTftResult.Trim();

					AfxExtractSubString(strCfPolMakerNo, strCfResult, 1, '_');
					AfxExtractSubString(strTftPolMakerNo, strTftResult, 1, '_');

					G_AddLog(3, 2, _T("PolMark PanelID : %s, CF : %s, TFT : %s"),strPanelId, strCfResult, strTftResult);

					if(strCfResult == POLMARKILEGAL_ABI_NG || strTftResult == POLMARKILEGAL_ABI_NG)
						m_stDefType.PolMiss++;
					else if(strCfPolMakerNo != strTftPolMakerNo)
						m_stDefType.PolMiss++;
				}
				else
				{
					if (strVal.Find(POLMARKILEGAL_NG) != -1)
					{
						m_stDefType.PolMiss++;	//2016.02.24
						//m_fnPolMarkIlegalAlarm(_T("POL Mark ILEGAL !!"),strPanelId);	//2016.02.24 Comment
					}	
				}
			}					
			break;
		default:
			if (nDefCnt < PART_NAME)  //2019.01.23 RECIPE_ID-> PART_NAME change by mby
			{
				strTemp.Append(strVal);
				strTemp.Append(EXT_COLON);				
			}			
			break;
		}
		if (nDefCnt >= P_ITEM_END)	//if(strVal.IsEmpty())
		{
			break;
		}
		//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 E
		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataPanel() - End"));
	return	strRetVal = strTemp;
}

CString CDataManager::m_fnMakeQmsDataDefect_KV(CString &strLine, int nCntVal)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KV() - Start"));

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strRetVal;
	CString strTemp , check_data, strVal, strDefType, strDefNo, strDefCode, strDefFlag;

	CString cCoordStartX,cCoordStartY,cCoordEndX,cCoordEndY;
	int curPos, nDefCnt;
	long imgX = 0, imgY = 0;
	check_data = strLine ;//
	curPos = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	strTemp = _T("");
	nDefCnt = 0;

	m_fnGetDefectType(check_data);
	strLine = check_data;
	while (TRUE)
	{			
		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;

		strVal = check_data.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0:		break;
		case LAYER_NO:	strTemp.Append(_T("45,"));	 break;
		case DEFECT_NO:
			//strDefNo = strVal;
			strDefNo.Format(_T("%d"),nCntVal);
			strTemp.Append(strDefNo + EXT_COLON);
			break;
		case 5: imgX = _ttoi(strVal); break;
		case 6: imgY = _ttoi(strVal); break;
		case GATE_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case GATE_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);

			strTemp.Append(EXT_COLON); // Gate / Data 2
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // Gate / Data 2
			strTemp.Append(EXT_COLON);
			break;

		case COORD_START_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordStartX = strVal;
			break;
		case COORD_END_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordEndX = strVal;
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			//strTemp.Append(strVal + EXT_COLON);
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;

			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				m_fnSetDefCoord(cCoordStartX,cCoordEndX,cCoordStartY,cCoordEndY);

			strTemp.Append(cCoordStartX + EXT_COLON);
			strTemp.Append(cCoordStartY + EXT_COLON);

			strTemp.Append(cCoordEndX + EXT_COLON);
			strTemp.Append(cCoordEndY + EXT_COLON);

			strTemp.Append(EXT_COLON); // 좌표 2
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // 수정 결과
			break;
		case DEFECT_SIZE:
			//strVal.Format(_T("%3s"),strVal);
			strTemp.Append(strVal + EXT_COLON);

			strTemp.Append(EXT_COLON); // 결함강도 2
			strTemp.Append(EXT_COLON); // 결함강도 3
			break;
		case  DEFECT_IMG_NO:			
			//strVal = m_fnChangeDefectImageName(_T("KV"), strDefNo);
			//strVal = m_strQmsImgName;
			strTemp.Append(strVal + EXT_COLON);
			break;
		case IMG_SIZE_Y:
			strTemp.Append(strVal + EXT_COLON);  

			strTemp.Append(EXT_COLON); // 이미지 2
			strTemp.Append(EXT_COLON); 
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // 이미지 3
			strTemp.Append(EXT_COLON); 
			break;

		default:
			strTemp.Append(strVal + EXT_COLON);
			break;
		}
		if (nDefCnt == 18)	//if(strVal.IsEmpty())
		{
			strTemp.Append(EXT_SEMICOL);
			strTemp.Append(_T("\n"));
			break;	
		}
		nDefCnt ++;				
		//m_fnSetItemData(D_DEFECT_NO, strLine, strItem, LEN_DEFECT_NO, 33);
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KV() - End"));
	return	strRetVal = strTemp;
}

CString CDataManager::m_fnMakeQmsDataDefect_KH(CString &strLine, int nCntVal)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KH() - Start"));

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	

	CString strRetVal;
	CString strTemp , check_data, strVal, strDefType, strDefNo, strDefCode, strDefFlag;
	CString cCoordStartX,cCoordStartY,cCoordEndX,cCoordEndY;
	int curPos, nDefCnt;
	long imgX = 0, imgY = 0;
	check_data = strLine ;//
	curPos = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	strTemp = _T("");
	nDefCnt = 0;

	m_fnGetDefectType(check_data);
	strLine = check_data;
	while (TRUE)
	{				
		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0:		break;
		case LAYER_NO:	
			strTemp.Append(_T("44,"));	 break;
		case DEFECT_NO:
			//strDefNo = strVal;
			strDefNo.Format(_T("%d"),nCntVal);
			strTemp.Append(strDefNo + EXT_COLON);
			break;
		case DEFECT_CODE:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case FLAG:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case GATE_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case GATE_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
			/*case COORD_START_X:
			strTemp.Append(strVal + EXT_COLON);
			break;
			case COORD_END_X:
			strTemp.Append(strVal + EXT_COLON);
			break;
			case COORD_START_Y:
			strTemp.Append(strVal + EXT_COLON);
			break;*/
		case COORD_START_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordStartX = strVal;
			break;
		case COORD_END_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordEndX = strVal;
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			//strTemp.Append(strVal + EXT_COLON);
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;

			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				m_fnSetDefCoord(cCoordStartX,cCoordEndX,cCoordStartY,cCoordEndY);

			strTemp.Append(cCoordStartX + EXT_COLON);
			strTemp.Append(cCoordStartY + EXT_COLON);

			strTemp.Append(cCoordEndX + EXT_COLON);
			strTemp.Append(cCoordEndY + EXT_COLON);
			/*case COORD_END_Y:
			strTemp.Append(strVal + EXT_COLON);
			*/
			strTemp.Append(EXT_COLON); // 결함 수
			strTemp.Append(EXT_COLON); // 수정 결과
			break;

		case  DEFECT_IMG_NO:			
			//strVal = m_fnChangeDefectImageName(_T("KH"), strDefNo);
			//strVal = m_strQmsImgName;
			strTemp.Append(strVal + EXT_COLON);
			break;
		case IMG_SIZE_X:
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case IMG_SIZE_Y:
			strTemp.Append(strVal + EXT_COLON);  

			strTemp.Append(EXT_COLON); // 이미지 2
			strTemp.Append(EXT_COLON); 
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // 이미지 3
			strTemp.Append(EXT_COLON); 
			break;

		default:
			break;
		}

		if (nDefCnt == 18)	//if(strVal.IsEmpty())
		{
			strTemp.Append(EXT_SEMICOL);
			strTemp.Append(_T("\n"));
			break;	
		}
		nDefCnt ++;				
		//m_fnSetItemData(D_DEFECT_NO, strLine, strItem, LEN_DEFECT_NO, 33);
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KH() - End"));
	return	strRetVal = strTemp;
}

CString CDataManager::m_fnMakeQmsDataDefect_KS(CString &strLine, int nCntVal)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KS() - Start"));

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strRetVal;
	CString strTemp , check_data, strVal, strDefType, strDefNo, strDefCode, strDefFlag;
	CString cCoordStartX,cCoordStartY,cCoordEndX,cCoordEndY;
	int curPos, nDefCnt;
	long imgX = 0, imgY = 0;
	check_data = strLine ;//
	curPos = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	strTemp = _T("");
	nDefCnt = 0;

	m_fnGetDefectType(check_data);
	strLine = check_data;
	while (TRUE)
	{			
		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;

		strVal = check_data.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0:		
			break;
		case LAYER_NO:	
			strTemp.Append(_T("42,"));	 
			break;

		case DEFECT_NO:
			//strDefNo = strVal;
			strDefNo.Format(_T("%d"),nCntVal);
			strTemp.Append(strDefNo + EXT_COLON);
			break;

		case DEFECT_CODE:
			strTemp.Append(strVal + EXT_COLON);
			break;

		case GATE_START_NO: // G/D X 1
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;

		case DATA_START_NO: // G/D Y 1
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			strTemp.Append(EXT_COLON); // G/D X 2 
			strTemp.Append(EXT_COLON); // G/D Y 2 
			strTemp.Append(EXT_COLON); // G/D X 3 
			strTemp.Append(EXT_COLON); // G/D Y 3 
			break;
			/*case GATE_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
			case DATA_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;*/

		case COORD_START_X: //워크 좌표 X 1
			//strTemp.Append(strVal + EXT_COLON);
			cCoordStartX = strVal;
			break;

		case COORD_START_Y: //워크 좌표 Y 1
			//strTemp.Append(strVal + EXT_COLON);
			cCoordStartY = strVal;
			//	break;
			//case COORD_END_X:
			//	cCoordEndX = strVal;
			//	//strTemp.Append(strVal + EXT_COLON);
			//	break;
			//case COORD_END_Y:
			//	cCoordEndY = strVal;
			//	

			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				m_fnSetDefCoord(cCoordStartX,cCoordEndX,cCoordStartY,cCoordEndY);

			strTemp.Append(cCoordStartX + EXT_COLON);
			strTemp.Append(cCoordStartY + EXT_COLON);
			strTemp.Append(EXT_COLON); // 워크 좌표 X 2
			strTemp.Append(EXT_COLON); // 워크 좌표 Y 2 
			strTemp.Append(EXT_COLON); // 워크 좌표 X 3 
			strTemp.Append(EXT_COLON); // 워크 좌표 Y 3

			//	strTemp.Append(cCoordEndX + EXT_COLON);
			//	strTemp.Append(cCoordEndY + EXT_COLON);
			/*case COORD_END_Y:
			strTemp.Append(strVal + EXT_COLON);
			*/
			strTemp.Append(EXT_COLON); // 결함 결과
		case DEFECT_SIZE:
			strTemp.Append(strVal + EXT_COLON); // 결함 강도 1
			strTemp.Append(EXT_COLON); // 결함 강도 2
			//strTemp.Append(EXT_COLON); // 결함 강도 3
			break;		
			//case  DEFECT_IMG_NO:			
			//	//strVal = m_fnChangeDefectImageName(_T("KH"), strDefNo);
			//	strVal = m_strQmsImgName;
			//	strTemp.Append(strVal + EXT_COLON);
			//	break;
			//case IMG_SIZE_X:
			//	strTemp.Append(strVal + EXT_COLON);  
			//	break;
			//case IMG_SIZE_Y:
			//	strTemp.Append(strVal + EXT_COLON);  

			//	strTemp.Append(EXT_COLON); // 이미지 2
			//	strTemp.Append(EXT_COLON); 
			//	strTemp.Append(EXT_COLON);
			//	strTemp.Append(EXT_COLON); // 이미지 3
			//	strTemp.Append(EXT_COLON); 
			//	break;

		default:
			break;
		}

		if (nDefCnt == 18)	//if(strVal.IsEmpty())
		{
			strTemp.Append(EXT_SEMICOL);
			strTemp.Append(_T("\n"));
			break;	
		}

		nDefCnt ++;				
		//m_fnSetItemData(D_DEFECT_NO, strLine, strItem, LEN_DEFECT_NO, 33);
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_KS() - End"));
	return	strRetVal = strTemp;
}
//2015.06.04 add by IKM - E

// 2017.07.10 Mr,Chu
// 기능 : Client에서 받은 Origin Data 를 Recipe Origin Data 위치로 가공한다.
// 요청 : 대형설비 왕빙
void CDataManager::m_fnSetDefCoord(CString &strCoordStartX, CString &strCoordEndX, CString &strCoordStartY, CString &strCoordEndY)
{
	if( m_stDefCoordDatRcp.bReportQMSCoordDat == FALSE )	//2016.12.30 Mr.Chu  기능 Off 일 경우 기능이 동작하지 않는다.
		return;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strTempStart = _T(""), strTempEnd = _T("");
	double dStartComp = NULL, dEndComp = NULL;

	//Origin 위치 LT 위치로 변경
	switch( pFrame->m_pVsSeverIF->m_nRecvOriginType )
	{
	case RECIPE_DEF_MAP_ORIGIN_LT:
		break;

	case RECIPE_DEF_MAP_ORIGIN_LB:
		dStartComp	= _wtof(strCoordStartY);
		dEndComp	= _wtof(strCoordEndY);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dEndComp;
		strCoordStartY.Format(_T("%0.3f"),dStartComp);
		strCoordEndY.Format(_T("%0.3f"),dEndComp);
		break;

	case RECIPE_DEF_MAP_ORIGIN_RT:
		dStartComp	= _wtof(strCoordStartX);
		dEndComp	= _wtof(strCoordEndX);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dEndComp;
		strCoordStartX.Format(_T("%0.3f"),dStartComp);
		strCoordEndX.Format(_T("%0.3f"),dEndComp);
		break;

	case RECIPE_DEF_MAP_ORIGIN_RB:
		dStartComp	= _wtof(strCoordStartY);
		dEndComp	= _wtof(strCoordEndY);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dEndComp;
		strCoordStartY.Format(_T("%0.3f"),dStartComp);
		strCoordEndY.Format(_T("%0.3f"),dEndComp);

		strTempStart = _T(""), strTempEnd = _T("");
		dStartComp = NULL, dEndComp = NULL;
		dStartComp	= _wtof(strCoordStartX);
		dEndComp	= _wtof(strCoordEndX);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dEndComp;
		strCoordStartX.Format(_T("%0.3f"),dStartComp);
		strCoordEndX.Format(_T("%0.3f"),dEndComp);
		break;
	}


	// Defect Map Recipe 의 Origin Data 로 가공
	strTempStart = _T(""), strTempEnd = _T("");
	if( m_stDefCoordDatRcp.bMajorAxisType == TRUE )
	{
		strTempStart = strCoordStartX;
		strTempEnd = strCoordEndX;

		strCoordStartX	= strCoordStartY;
		strCoordEndX	= strCoordEndY;
		strCoordStartY	= strTempStart;
		strCoordEndY	= strTempEnd;
	}

	strTempStart = _T(""), strTempEnd = _T("");
	switch(m_stDefCoordDatRcp.nOriginType)
	{
	case RECIPE_DEF_MAP_ORIGIN_LT:
		break;

	case RECIPE_DEF_MAP_ORIGIN_LB:
		dStartComp	= _wtof(strCoordStartY);
		dEndComp	= _wtof(strCoordEndY);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dEndComp;
		strCoordStartY.Format(_T("%0.3f"),dStartComp);
		strCoordEndY.Format(_T("%0.3f"),dEndComp);
		break;

	case RECIPE_DEF_MAP_ORIGIN_RT:
		dStartComp	= _wtof(strCoordStartX);
		dEndComp	= _wtof(strCoordEndX);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dEndComp;
		strCoordStartX.Format(_T("%0.3f"),dStartComp);
		strCoordEndX.Format(_T("%0.3f"),dEndComp);
		break;

	case RECIPE_DEF_MAP_ORIGIN_RB:
		dStartComp	= _wtof(strCoordStartY);
		dEndComp	= _wtof(strCoordEndY);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeHeight - dEndComp;
		strCoordStartY.Format(_T("%0.3f"),dStartComp);
		strCoordEndY.Format(_T("%0.3f"),dEndComp);

		strTempStart = _T(""), strTempEnd = _T("");
		dStartComp = NULL, dEndComp = NULL;
		dStartComp	= _wtof(strCoordStartX);
		dEndComp	= _wtof(strCoordEndX);

		dStartComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dStartComp;
		dEndComp	= m_stDefCoordDatRcp.dSetPanelSizeWidth - dEndComp;
		strCoordStartX.Format(_T("%0.3f"),dStartComp);
		strCoordEndX.Format(_T("%0.3f"),dEndComp);

		break;
	}
	//2019.10.09 add by mby
	strTempStart = _T(""), strTempEnd = _T("");
	if( m_stDefCoordDatRcp.bMajorAxisType == TRUE )
	{
		strTempStart = strCoordStartX;
		strTempEnd = strCoordEndX;

		strCoordStartX	= strCoordStartY;
		strCoordEndX	= strCoordEndY;
		strCoordStartY	= strTempStart;
		strCoordEndY	= strTempEnd;
	}
}

CString CDataManager::m_fnMakeQmsDataDefect_AVL(CString &strLine, int KvCntVal)
{
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_AVL() - Start"));

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	CString strRetVal;
	CString strTemp , check_data, strVal, strDefType, strDefNo, strDefCode, strDefFlag;

	CString cCoordStartX,cCoordStartY,cCoordEndX,cCoordEndY;
	CString strDefSizeX,strDefSizeY;	//2020.10.23 add by mby
	DOUBLE	dDefSizeX = 0,dDefSizeY = 0;	//2020.10.23 add by mby
	int curPos, nDefCnt;
	long imgX = 0, imgY = 0;
	check_data = strLine ;
	curPos = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	strTemp = _T("");
	nDefCnt = 0;

	m_fnGetDefectType(check_data);
	strLine = check_data;
	while (TRUE)
	{				

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0:		break;
		case LAYER_NO:	strTemp.Append(_T("41,"));	 break;
		case DEFECT_NO:
			strDefNo = strVal;
			strTemp.Append(strVal + EXT_COLON);
			break;
		case 5: imgX = _ttoi(strVal); break;
		case 6: imgY = _ttoi(strVal); break;
		case GATE_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_START_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case GATE_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);
			break;
		case DATA_END_NO:
			if (_ttoi(strVal) == 0)	strVal = _T("");
			strTemp.Append(strVal + EXT_COLON);

			strTemp.Append(EXT_COLON); // Gate / Data 2
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // Gate / Data 2
			strTemp.Append(EXT_COLON);
			break;
		case COORD_START_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordStartX = strVal;
			break;
		case COORD_END_X:
			//strTemp.Append(strVal + EXT_COLON);
			cCoordEndX = strVal;
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			//strTemp.Append(strVal + EXT_COLON);
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;

			if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
				m_fnSetDefCoord(cCoordStartX,cCoordEndX,cCoordStartY,cCoordEndY);

			strTemp.Append(cCoordStartX + EXT_COLON);
			strTemp.Append(cCoordStartY + EXT_COLON);

			strTemp.Append(cCoordEndX + EXT_COLON);
			strTemp.Append(cCoordEndY + EXT_COLON);

			strTemp.Append(EXT_COLON); // 좌표 2
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON);
			//strTemp.Append(EXT_COLON); // 수정 결과  Delete KYH01 TEST 용 NG OK 표시 부분 삭제
			break;
		case DEFECT_SIZE:
			//strVal.Format(_T("%3s"),strVal);
			if (_ttoi(strVal) > 99999)	//2018.01.27 Req Wang Gang
			{
				strVal = _T("99999");
				strTemp.Append(strVal + EXT_COLON);
			}
			else
				strTemp.Append(strVal + EXT_COLON);

			strTemp.Append(EXT_COLON); // 결함강도 2
			strTemp.Append(EXT_COLON); // 결함강도 3
			break;
		case  DEFECT_IMG_NO:			
			//strVal = m_fnChangeDefectImageName(_T("KV"), strDefNo);
			//strVal = m_strQmsImgName;
			strTemp.Append(strVal + EXT_COLON);
			break;
		case IMG_SIZE_X:
			//2020.10.23 add by mby
			dDefSizeX = _ttof(strVal);		
			if(int(dDefSizeX *100) > 99998){	
				strDefSizeX.Format(_T("%d"),99998) ;
			}else{
				strDefSizeX.Format(_T("%d"),int(dDefSizeX *100)) ;
			}			
			strVal = _T("15360");	//2018.01.27 Req Wang Gang
			strTemp.Append(strVal + EXT_COLON);
			break;
		case IMG_SIZE_Y:
			//2020.10.23 add by mby
			dDefSizeY = _ttof(strVal);		
			if(int(dDefSizeY *100) > 99998){
				strDefSizeY.Format(_T("%d"),99998) ;
			}else{
				strDefSizeY.Format(_T("%d"),int(dDefSizeY *100)) ;
			}
			strVal = _T("11520");	//2018.01.27 Req Wang Gang
			strTemp.Append(strVal + EXT_COLON);  

			strTemp.Append(EXT_COLON); // 이미지 2
			strTemp.Append(strDefSizeX + EXT_COLON);	//2020.10.23 add by mby
			strTemp.Append(strDefSizeY + EXT_COLON);	//2020.10.23 add by mby
			//strTemp.Append(EXT_COLON); 
			//strTemp.Append(EXT_COLON);
			strTemp.Append(EXT_COLON); // 이미지 3
			strTemp.Append(EXT_COLON); 
			break;

		default:
			strTemp.Append(strVal + EXT_COLON);

			break;
		}
		if (nDefCnt == 18)	//if(strVal.IsEmpty())
		{
			strTemp.Append(EXT_SEMICOL);
			strTemp.Append(_T("\n"));
			break;	
		}
		nDefCnt ++;				
		//m_fnSetItemData(D_DEFECT_NO, strLine, strItem, LEN_DEFECT_NO, 33);
	};
	G_AddLog(3, 3, _T("m_fnMakeQmsDataDefect_AVL() - End"));
	return	strRetVal = strTemp;
}

BOOL CDataManager::m_fnGetDefectType(CString& check_data)
{
	G_AddLog(3, 3, _T("m_fnGetDefectType() - Start"));
	CString strDefType, strDefCode, strDefFlag;
	int strDefStartpos, strDefEndpos;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strDefSizeX, strDefSizeY;
	//2018.04.02
	CString strDefPos;
	strDefPos = m_fnGetItemData(DEFECT_CODE + 1, check_data);

	strDefType = m_fnGetItemData(DEFECT_TYPE + 1, check_data);
	strDefFlag = m_fnGetItemData(FLAG + 1, check_data);
	strDefSizeX = m_fnGetItemData(IMG_SIZE_X + 1, check_data);	//2020.01.02 add by mby
	strDefSizeY = m_fnGetItemData(IMG_SIZE_Y + 1, check_data);	//2020.01.02 add by mby
	
	
	strDefStartpos = _ttoi(m_fnGetItemData(COORD_START_X + 1 , check_data));
	strDefEndpos =  _ttoi(m_fnGetItemData(COORD_START_Y + 1 , check_data));

	switch (_ttoi(strDefType))
	{
	case INSP_DUST:
		m_stDefType.DustType += 1;
		break;
	case INSP_WHITE_TURBIDITY:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftWhiteBlur  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTWHITEBLUR);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			m_stDefType.CfWhiteBlur  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFWHITEBLUR);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_ITO:
		m_stDefType.ItoPeel += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), ITOPEEL);
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_EDGE_BROKEN:		
		m_stDefType.WanesBroken  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), WANESBROKEN);
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE  + 2, check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_PAD_SCRATCH:
		m_stDefType.SocketScrach  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), SOCKETSCRACH);
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE  + 2, check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);		
		break;
	case INSP_DIMPLE_POINT:	
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftDimplePoint  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTDIMPLEPOINT);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			m_stDefType.CfDimplePoint  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFDIMPLEPOINT);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_ETCHING_LINE:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftEdgeilegal += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTEDGEILEGAL);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			m_stDefType.CfEdgeilegal += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFEDGEILEGAL);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_DIMPLE_LINE:

		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftDimpleLine   += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTDIMPLELINE);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			m_stDefType.CfDimpleLine   += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFDIMPLELINE);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_PAD_BURNT:
		m_stDefType.SocketBurn  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), SOCKETBURN);
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_BURR:
		m_stDefType.Burr  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), BURR);
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_POL_BUBBLE:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftPolBubble  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLBUBBLE);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{			
			m_stDefType.CfPolBubble  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLBUBBLE);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_POL_SCRATCH:	
		if (_ttoi(strDefFlag) ==  TFT )
		{			
			m_stDefType.TftPolScrach  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLSCRATCH);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{			
			m_stDefType.CfPolScrach  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLSCRATCH);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	case INSP_POL_ALIEN_SUBSTANCE:
		
		if (_ttoi(strDefFlag) ==  TFT )
		{			
			m_stDefType.TFTpolSubstance  += 1;	
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLSUBSTANCE);			
		}
		if (_ttoi(strDefFlag) ==  CF )
		{			
			m_stDefType.CFpolSubstance  += 1;	
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLSUBSTANCE);			
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

		//2015.06.04 add by IKM - S 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
	case INSP_POL_MINOR: 
		m_stDefType.PolMinor  += 1;	
		strDefCode.Format(_T("%s%d"),_T("K"), POLMINOR);
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;	
		//2015.06.04 add by IKM - E 微小欠陥 미소결함, 猫爪ムラ 묘조무라 

	case INSP_BLACK_SCRATCH:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftScrach  += 1;
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTSCRATCH);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			m_stDefType.CfScrach  += 1;
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFSCRATCH);
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_MURA:
		m_stDefType.mura  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), MURA);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_CF_BURR:	//2015.07.13
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,0,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[0] == 0){
				m_nArrayDccCount[0] += 1;
				m_nDccFlag[0] = 1;
			}
			if(!m_bPaGradeFlag[INSP_CF_BURR]){
				m_bPaCodeFlag[INSP_CF_BURR] = TRUE;
			}
		}else{
			m_bPaGradeFlag[INSP_CF_BURR]= TRUE;
			m_bPaCodeFlag[INSP_CF_BURR] = FALSE;
		}
		m_stDefType.CfBurr  += 1;
		//m_stDefType.Burr  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), CFBURR);
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBURR_B);
			m_stDefType.CfBurrB += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBURR_C);
			m_stDefType.CfBurrC += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBURR_D);
			m_stDefType.CfBurrD += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBURR);
			m_stDefType.CfBurrA += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_TFT_BURR:
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,0,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[0] == 0){
				m_nArrayDccCount[0] += 1;
				m_nDccFlag[0] = 1;
			}
			if(!m_bPaGradeFlag[INSP_TFT_BURR]){
				m_bPaCodeFlag[INSP_TFT_BURR] = TRUE;
			}
		}else{
			m_bPaGradeFlag[INSP_TFT_BURR] = TRUE;
			m_bPaCodeFlag[INSP_TFT_BURR] = FALSE;
		}
		m_stDefType.TftBurr  += 1;
		//	m_stDefType.Burr  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), TFTBURR);	
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBURR_B);
			m_stDefType.TftBurrB  += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBURR_C);
			m_stDefType.TftBurrC  += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBURR_D);
			m_stDefType.TftBurrD  += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBURR);
			m_stDefType.TftBurrA  += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_CF_BROKEN:
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,1,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[1] == 0){
				m_nArrayDccCount[1] += 1;
				m_nDccFlag[1] = 1;
			}
			m_nArrayPaCntPDCount[0] += 1;	//2020.08.05 add by mby
			//2020.08.26 add by mby S-
			if(strDefPos == _T("A")){
				m_nArrayPaCntEdgeCount[0] += 1;
			}else if(strDefPos == _T("B")){
				m_nArrayPaCntEdgeCount[1] += 1;
			}else if(strDefPos == _T("C")){
				m_nArrayPaCntEdgeCount[2] += 1;
			}else{
				m_nArrayPaCntEdgeCount[3] += 1;
			}
			//2020.12.08 add by mby S-
			if(m_fnPACntPDCheck_TypeA( 0, 4,strDefPos)){
				m_bPaOverlapAlarmFlag[1] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
				m_bPaGradeFlag[INSP_CF_BROKEN] = TRUE;
				m_bPaCodeFlag[INSP_CF_BROKEN] = FALSE;
			}
			//2020.12.08 add by mby E-
			//2020.08.26 add by mby E-
			if(!m_bPaGradeFlag[INSP_CF_BROKEN]){
				m_bPaCodeFlag[INSP_CF_BROKEN] = TRUE;
			}
		}else{
			m_bPaOverlapAlarmFlag[1] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
			m_bPaGradeFlag[INSP_CF_BROKEN] = TRUE;
			m_bPaCodeFlag[INSP_CF_BROKEN] = FALSE;
		}
		m_stDefType.CfBroken  += 1;
		//	m_stDefType.WanesBroken  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), CFBROKEN);
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBROKEN_B);
			m_stDefType.CfBrokenB  += 1;
			m_stDefType_UI.CfBrokenB  += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBROKEN_C);
			m_stDefType.CfBrokenC  += 1;
			m_stDefType_UI.CfBrokenC  += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBROKEN_D);
			m_stDefType.CfBrokenD  += 1;
			m_stDefType_UI.CfBrokenD  += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFBROKEN);
			m_stDefType.CfBrokenA  += 1;
			m_stDefType_UI.CfBrokenA  += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_TFT_BROKEN:
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,2,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[2] == 0){
				m_nArrayDccCount[2] += 1;
				m_nDccFlag[2] = 1;
			}
			m_nArrayPaCntPDCount[1] += 1;	//2020.08.05 add by mby
			//2020.08.26 add by mby S-
			if(strDefPos == _T("A")){
				m_nArrayPaCntEdgeCount[4] += 1;
			}else if(strDefPos == _T("B")){
				m_nArrayPaCntEdgeCount[5] += 1;
			}else if(strDefPos == _T("C")){
				m_nArrayPaCntEdgeCount[6] += 1;
			}else{
				m_nArrayPaCntEdgeCount[7] += 1;
			}
			//2020.12.08 add by mby S-
			if(m_fnPACntPDCheck_TypeA( 1, 8,strDefPos)){
				m_bPaOverlapAlarmFlag[2] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
				m_bPaGradeFlag[INSP_TFT_BROKEN] = TRUE;
				m_bPaCodeFlag[INSP_TFT_BROKEN] = FALSE;
			}
			//2020.12.08 add by mby E-
			//2020.08.26 add by mby E-
			if(!m_bPaGradeFlag[INSP_TFT_BROKEN]){
				m_bPaCodeFlag[INSP_TFT_BROKEN] = TRUE;
			}
		}else{
			m_bPaOverlapAlarmFlag[2] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
			m_bPaGradeFlag[INSP_TFT_BROKEN] = TRUE;
			m_bPaCodeFlag[INSP_TFT_BROKEN] = FALSE;
		}
		m_stDefType.TftBroken  += 1;
		//m_stDefType.WanesBroken  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), TFTBROKEN);
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBROKEN_B);
			m_stDefType.TftBrokenB  += 1;
			m_stDefType_UI.TftBrokenB  += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBROKEN_C);
			m_stDefType.TftBrokenC  += 1;
			m_stDefType_UI.TftBrokenC  += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBROKEN_D);
			m_stDefType.TftBrokenD  += 1;
			m_stDefType_UI.TftBrokenD  += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTBROKEN);
			m_stDefType.TftBrokenA  += 1;
			m_stDefType_UI.TftBrokenA  += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_CF_CORNER_BROKEN:
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,3,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[3] == 0){
				m_nArrayDccCount[3] += 1;
				m_nDccFlag[3] = 1;
			}
			m_nArrayPaCntPDCount[2] += 1;	//2020.08.05 add by mby
			//2020.08.26 add by mby S-
			if(strDefPos == _T("A")){
				m_nArrayPaCntEdgeCount[8] += 1;
			}else if(strDefPos == _T("B")){
				m_nArrayPaCntEdgeCount[9] += 1;
			}else if(strDefPos == _T("C")){
				m_nArrayPaCntEdgeCount[10] += 1;
			}else{
				m_nArrayPaCntEdgeCount[11] += 1;
			}
			//2020.12.08 add by mby S-
			if(m_fnPACntPDCheck_TypeA( 2, 12,strDefPos)){
				m_bPaOverlapAlarmFlag[3] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
				m_bPaGradeFlag[INSP_CF_CORNER_BROKEN] = TRUE;
				m_bPaCodeFlag[INSP_CF_CORNER_BROKEN] = FALSE;
			}
			//2020.12.08 add by mby E-
			//2020.08.26 add by mby E-
			if(!m_bPaGradeFlag[INSP_CF_CORNER_BROKEN]){
				m_bPaCodeFlag[INSP_CF_CORNER_BROKEN] = TRUE;
			}
		}else{
			m_bPaOverlapAlarmFlag[3] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
			m_bPaGradeFlag[INSP_CF_CORNER_BROKEN] = TRUE;
			m_bPaCodeFlag[INSP_CF_CORNER_BROKEN] = FALSE;
		}
		m_stDefType.CfCornerBroken  += 1;
		//	m_stDefType.WanesBroken  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), CFCORNERBROKEN);
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFCORNERBROKEN_B);
			m_stDefType.CfCornerBrokenB  += 1;
			m_stDefType_UI.CfCornerBrokenB  += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFCORNERBROKEN_C);
			m_stDefType.CfCornerBrokenC  += 1;
			m_stDefType_UI.CfCornerBrokenC  += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFCORNERBROKEN_D);
			m_stDefType.CfCornerBrokenD  += 1;
			m_stDefType_UI.CfCornerBrokenD  += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), CFCORNERBROKEN);
			m_stDefType.CfCornerBrokenA  += 1;
			m_stDefType_UI.CfCornerBrokenA  += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_TFT_CORNER_BROKEN:
		if(m_fnDccSizeCheck(strDefSizeX,strDefSizeY,4,strDefPos)){	//2020.06.02 add by mby
			if(m_nDccFlag[4] == 0){
				m_nArrayDccCount[4] += 1;
				m_nDccFlag[4] = 1;
			}
			m_nArrayPaCntPDCount[3] += 1;	//2020.08.05 add by mby
			//2020.08.26 add by mby S-
			if(strDefPos == _T("A")){
				m_nArrayPaCntEdgeCount[12] += 1;
			}else if(strDefPos == _T("B")){
				m_nArrayPaCntEdgeCount[13] += 1;
			}else if(strDefPos == _T("C")){
				m_nArrayPaCntEdgeCount[14] += 1;
			}else{
				m_nArrayPaCntEdgeCount[15] += 1;
			}
			//2020.12.08 add by mby S-
			if(m_fnPACntPDCheck_TypeA( 3, 16,strDefPos)){
				m_bPaOverlapAlarmFlag[4] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
				m_bPaGradeFlag[INSP_TFT_CORNER_BROKEN] = TRUE;
				m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN] = FALSE;
			}
			//2020.12.08 add by mby E-
			//2020.08.26 add by mby E-
			if(!m_bPaGradeFlag[INSP_TFT_CORNER_BROKEN]){
				m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN] = TRUE;
			}
		}else{
			m_bPaOverlapAlarmFlag[4] = TRUE;	//2021.01.13 add by mby for Edge,Corner Broken
			m_bPaGradeFlag[INSP_TFT_CORNER_BROKEN] = TRUE;
			m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN] = FALSE;
		}
		m_stDefType.TftCornerBroken  += 1;
		//m_stDefType.WanesBroken  += 1;
		//2018.04.02
		//strDefCode.Format(_T("%s%d"),_T("K"), TFTCORNERBROKEN);
		if(strDefPos == _T("B"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTCORNERBROKEN_B);
			m_stDefType.TftCornerBrokenB  += 1;
			m_stDefType_UI.TftCornerBrokenB  += 1;
		}
		else if(strDefPos == _T("C"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTCORNERBROKEN_C);
			m_stDefType.TftCornerBrokenC  += 1;
			m_stDefType_UI.TftCornerBrokenC  += 1;
		}
		else if(strDefPos == _T("D"))
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTCORNERBROKEN_D);
			m_stDefType.TftCornerBrokenD  += 1;
			m_stDefType_UI.TftCornerBrokenD  += 1;
		}
		else
		{
			strDefCode.Format(_T("%s%d"),_T("KAO"), TFTCORNERBROKEN);
			m_stDefType.TftCornerBrokenA  += 1;
			m_stDefType_UI.TftCornerBrokenA  += 1;
		}
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_CF_CELL_BROKEN:	//2015.08.26
		m_stDefType.CfCellBroken  += 1;
		//m_stDefType.WanesBroken  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), CFCELLBROKEN);	
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_TFT_CELL_BROKEN:
		m_stDefType.TftCellBroken  += 1;
		//m_stDefType.WanesBroken  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), TFTCELLBROKEN);
		//2020.02.27 add by mby
		if(pFrame->m_stParamInfo.bUseBubbleAreaAlarm){
			m_fnGetBubble_ABI(strDefStartpos, strDefEndpos);
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//2016.01.25 add by ikm - 물류 코드 추가
	case INSP_PIMPLE_POINT:

		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftPimplePoint  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPIMPLEPOINT);	
		}
		if (_ttoi(strDefFlag) ==  CF )
		{
			m_stDefType.CfPimplePoint  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFPIMPLEPOINT);	
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

		//case INSP_TFT_PIMPLE_POINT:
		//	m_stDefType.CfCornerBroken  += 1;
		//	//	m_stDefType.WanesBroken  += 1;
		//	strDefCode.Format(_T("%s%d"),_T("K"), TFTPIMPLELINE);		
		//	check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		//	break;

	case INSP_PIMPLE_LINE:
		if (_ttoi(strDefFlag) ==  TFT )
		{
			m_stDefType.TftPimpleLine  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPIMPLELINE);	
		}
		if (_ttoi(strDefFlag) ==  CF )
		{
			m_stDefType.CfPimpleLine  += 1;
			strDefCode.Format(_T("%s%d"),_T("K"), CFPIMPLELINE);	
		}
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

		//case INSP_TFT_PIMPLE_LINE:	
		//	m_stDefType.CfCellBroken  += 1;
		//	//m_stDefType.WanesBroken  += 1;
		//	strDefCode.Format(_T("%s%d"),_T("K"), CFCELLBROKEN);		
		//	check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		//	break;

	case INSP_CF_PIMPLE_CIRCLE:
		m_stDefType.CfPimpleCircle  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), CFPIMPLECIRCLE);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_CF_WAVE_PATTERN:
		m_stDefType.CfWavePattern  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), CFWAVEPATTERN);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_ETC_LINE:	
		m_stDefType.EtcLine  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), ETCLINE);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_ETC_POINT:
		m_stDefType.EtcPoint  += 1;
		strDefCode.Format(_T("%s%d"),_T("KAO"), ETCPOINT);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_POL_MISS:
		m_stDefType.PolMiss  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), POLMISS);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;
	
		//2017.04.12
	case INSP_CF_PROTECTION:
		m_stDefType.CfProtection  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), CFPROTECTION);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

	case INSP_TFT_PROTECTION:
		m_stDefType.TftProtection  += 1;
		strDefCode.Format(_T("%s%d"),_T("K"), TFTPROTECTION);		
		check_data = m_fnSetItemData(DEFECT_CODE + 2 , check_data, strDefCode, strDefCode.GetLength() , D_ITEM_END - 1);
		break;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	G_AddLog(3, 3, _T("m_fnGetDefectType() - End"));
	return TRUE;
}
//2020.02.28 add by mby
BOOL CDataManager::m_fnGetBubble_ABI(int nDefX, int nDefY)
{	
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pViewSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);	
	CString strDefCode;
	int nDefIndexX, nDefIndexY, nDefCode;
	int nSperX,nSperY;
	int width = pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeWidth;
	int height = pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight;
	if(width > height){
		nSperX = width / 4;
		nSperY = height / 3;
	}
	else{
		nSperX = height / 4;
		nSperY = width / 3;
	}
	if(nSperX == 0 || nSperY == 0)
	{
		 nDefCode = 0;
	}
	else
	{
		if(pFrame->m_DataManager.m_stDefCoordDatRcp.bMajorAxisType){
			nDefIndexX = nDefY / nSperX;	// Y 3등분
			nDefIndexY = nDefX / nSperY;    // X 4등분	
		}else{
			nDefIndexX = nDefX / nSperX;	// X 4등분
			nDefIndexY = nDefY / nSperY;    // Y 3등분	
		}		
		if(nDefIndexX == 0 && nDefIndexY == 0) nDefCode = 0;
		else if(nDefIndexX == 1 && nDefIndexY == 0) nDefCode = 1;
		else if(nDefIndexX == 2 && nDefIndexY == 0) nDefCode = 2;
		else if(nDefIndexX == 3 && nDefIndexY == 0) nDefCode = 3;
		else if(nDefIndexX == 0 && nDefIndexY == 1) nDefCode = 4;
		else if(nDefIndexX == 1 && nDefIndexY == 1) nDefCode = 5;
		else if(nDefIndexX == 2 && nDefIndexY == 1) nDefCode = 6;
		else if(nDefIndexX == 3 && nDefIndexY == 1) nDefCode = 7;
		else if(nDefIndexX == 0 && nDefIndexY == 2) nDefCode = 8;
		else if(nDefIndexX == 1 && nDefIndexY == 2) nDefCode = 9;
		else if(nDefIndexX == 2 && nDefIndexY == 2) nDefCode = 10;
		else if(nDefIndexX == 3 && nDefIndexY == 2) nDefCode = 11;
		else nDefCode = 0;
	}
	int nBubbleStationNum = 0;
	switch (nDefCode)
	{			
		case 0: 
			m_stDefType.BubbleStation1 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 1: 
			m_stDefType.BubbleStation2 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 2:
			m_stDefType.BubbleStation3 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 3:
			m_stDefType.BubbleStation4 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 4:
			m_stDefType.BubbleStation5 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 5:
			m_stDefType.BubbleStation6 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 6:
			m_stDefType.BubbleStation7 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 7:
			m_stDefType.BubbleStation8 += 1;
			nDefCode = 0;
			return TRUE;
		case 8:
			m_stDefType.BubbleStation9 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 9:
			m_stDefType.BubbleStation10 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		case 10:
			m_stDefType.BubbleStation11 += 1;
			nDefCode = 0;
			return TRUE;
			break;
		default:
			m_stDefType.BubbleStation12 += 1;
			nDefCode = 0;
			return TRUE;
			break;
	}
	return FALSE;
}
//2020.02.28 add by mby
BOOL CDataManager::m_fnGetJudgeGrade_AVL(BOOL &bPanelCodeTest, CString &strGrade,CString &strJudge)
{
	//2015.07.20
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	BOOL bCheck = FALSE;
	int nIndex = 0;
	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_stJudgeItemInfo.initDefType();
	m_stJudgeItemInfo.setInfoPath(PNL_JUDGE_ITEM, strRoot);
	m_stJudgeItemInfo.GetDefInfo();

	m_stJudgeItemPriority.initDefType();
	m_stJudgeItemPriority.setInfoPath(PNL_JUDGE_PRIORITY, strRoot);
	m_stJudgeItemPriority.GetDefInfo();

	strGrade  = JUDGE_A;
	strJudge = _T("");	//2020.04.01 add by mby

	while(TRUE)
	{
		if( nIndex++ >= m_stJudgeItemInfo.TotalCnt )
			break;

		// PanelCodeTest
		if( m_stPanelCodeTest.bJudgeInfo && nIndex == m_stJudgeItemPriority.PanelCodeTest )
		{
			bPanelCodeTest = TRUE;
			m_stJudgeItemInfo.PanelCodeTest = FALSE;
		}
		///////////////////////////
		if(m_stJudgeItemInfo.CfCellBroken && nIndex == m_stJudgeItemPriority.CfCellBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_CELL_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_CELL_BROKEN) )
			{				
			    strGrade = pAuto->m_strValue_GradeAuto[CF_CELL_BROKEN];
				//strGrade = JUDGE_C;   // 2018.11.10 by ryu  왕깡요청
				strJudge.Format(_T("AO%d"), CFCELLBROKEN);
				m_stDefType.CfCellBroken= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfCellBroken)
			{
				 strGrade = pAuto->m_strValue_GradeAuto[CF_CELL_BROKEN];
				//strValue = JUDGE_C; //2018.11.10 by ryu  왕깡요청
				strJudge.Format(_T("AO%d"), CFCELLBROKEN);
				m_stDefType.CfCellBroken= 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.TftCellBroken && nIndex == m_stJudgeItemPriority.TftCellBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_CELL_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_CELL_BROKEN))
			{ 
				//strGrade = JUDGE_C;  //2018.11.10 by ryu  왕깡요청
				strGrade = pAuto->m_strValue_GradeAuto[TFT_CELL_BROKEN];
				strJudge.Format(_T("AO%d"), TFTCELLBROKEN);
				m_stDefType.TftCellBroken= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftCellBroken)
			{
				//strGrade = JUDGE_C;  //2018.11.10 by ryu  왕깡요청
				strGrade = pAuto->m_strValue_GradeAuto[TFT_CELL_BROKEN];
				strJudge.Format(_T("AO%d"), TFTCELLBROKEN);
				m_stDefType.TftCellBroken= 0;
				return TRUE;
			}
		}

		//
		if(m_stJudgeItemInfo.CFCLuminance && nIndex == m_stJudgeItemPriority.CFCLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CFC_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CFC_LUMINANCE))
			{
				strGrade = JUDGE_A;  // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFCLUMINANCE);
				m_stDefType.CFCLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CFCLuminance)
			{
				strGrade = JUDGE_A;	 // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFCLUMINANCE);
				m_stDefType.CFCLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CBCLuminance && nIndex == m_stJudgeItemPriority.CBCLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CBC_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CBC_LUMINANCE))
			{
				strGrade = JUDGE_A;  // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CBCLUMINANCE);
				m_stDefType.CBCLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.CBCLuminance)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CBCLUMINANCE);
				m_stDefType.CBCLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CFSLuminance && nIndex == m_stJudgeItemPriority.CFSLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CFS_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CFS_LUMINANCE))
			{
				strGrade = JUDGE_A;  // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFSLUMINANCE);
				m_stDefType.CFSLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.CFSLuminance )
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFSLUMINANCE);
				m_stDefType.CFSLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CBSLuminance && nIndex == m_stJudgeItemPriority.CBSLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CBS_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CBS_LUMINANCE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CBSLUMINANCE);
				m_stDefType.CBSLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CBSLuminance)
			{
				strGrade = JUDGE_A;// strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CBSLUMINANCE);
				m_stDefType.CBSLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TFCLuminance && nIndex == m_stJudgeItemPriority.TFCLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFC_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFC_LUMINANCE))
			{
				strGrade = JUDGE_A;  // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFCLUMINANCE);
				m_stDefType.TFCLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TFCLuminance)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFCLUMINANCE);
				m_stDefType.TFCLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TBCLuminance && nIndex == m_stJudgeItemPriority.TBCLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TBC_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TBC_LUMINANCE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TBCLUMINANCE);
				m_stDefType.TBCLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TBCLuminance )
			{
				strGrade = JUDGE_A;// strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TBCLUMINANCE);
				m_stDefType.TBCLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TFSLuminance && nIndex == m_stJudgeItemPriority.TFSLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFS_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFS_LUMINANCE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFSLUMINANCE);
				m_stDefType.TFSLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TFSLuminance )
			{
				strGrade = JUDGE_A;// strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFSLUMINANCE);
				m_stDefType.TFSLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TBSLuminance && nIndex == m_stJudgeItemPriority.TBSLuminance)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TBS_LUMINANCE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TBS_LUMINANCE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TBSLUMINANCE);
				m_stDefType.TBSLuminance= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TBSLuminance ) 
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TBSLUMINANCE);
				m_stDefType.TBSLuminance= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfBurr && nIndex == m_stJudgeItemPriority.CfBurr)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_BURR);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_BURR))
			{
				//strGrade = JUDGE_A;  // strGrade = JUDGE_B; 20181212 by ryu
				strGrade = pAuto->m_strValue_GradeAuto[CF_BURR];
				if(m_stDefType.CfBurrA)
				{
					strJudge.Format(_T("AO%d"), CFBURR);
					m_stDefType.CfBurrA = 0;
				}
				else if(m_stDefType.CfBurrB)
				{
					strJudge.Format(_T("AO%d"), CFBURR_B);
					m_stDefType.CfBurrB = 0;
				}
				else if(m_stDefType.CfBurrC)
				{
					strJudge.Format(_T("AO%d"), CFBURR_C);
					m_stDefType.CfBurrC = 0;
				}
				else if(m_stDefType.CfBurrD)
				{
					strJudge.Format(_T("AO%d"), CFBURR_D);
					m_stDefType.CfBurrD = 0;
				}
				m_stDefType.CfBurr= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.CfBurr && !m_bPaCodeFlag[INSP_CF_BURR])	
			{
				strGrade = JUDGE_A;	//2020.04.02 add by mby
				if(!m_bPaCodeFlag[INSP_CF_BURR]){		//2020.04.02 add by mby	
					strGrade = pAuto->m_strValue_GradeAuto[CF_BURR];
				}

				if(m_stDefType.CfBurrA)
				{
					strJudge.Format(_T("AO%d"), CFBURR);
					m_stDefType.CfBurrA = 0;
				}
				else if(m_stDefType.CfBurrB)
				{
					strJudge.Format(_T("AO%d"), CFBURR_B);
					m_stDefType.CfBurrB = 0;
				}
				else if(m_stDefType.CfBurrC)
				{
					strJudge.Format(_T("AO%d"), CFBURR_C);
					m_stDefType.CfBurrC = 0;
				}
				else if(m_stDefType.CfBurrD)
				{
					strJudge.Format(_T("AO%d"), CFBURR_D);
					m_stDefType.CfBurrD = 0;
				}
				m_stDefType.CfBurr= 0;
				return TRUE;
			}//2020.01.16 add by mby S-
			//else if ( bCheck == TRUE && m_bPaCodeFlag[INSP_CF_BURR])
			else if (m_bPaCodeFlag[INSP_CF_BURR] && !m_bPaCheckFlag)	//2020.01.10 add by mby PA일이지만 Defect있으면 Defecet Code표시 
			{
				if(m_stDefType.CfBurrA)
				{
					strJudge.Format(_T("AO%d"), CFBURR);
					m_stDefType.CfBurrA = 0;
				}
				else if(m_stDefType.CfBurrB)
				{
					strJudge.Format(_T("AO%d"), CFBURR_B);
					m_stDefType.CfBurrB = 0;
				}
				else if(m_stDefType.CfBurrC)
				{
					strJudge.Format(_T("AO%d"), CFBURR_C);
					m_stDefType.CfBurrC = 0;
				}
				else if(m_stDefType.CfBurrD)
				{
					strJudge.Format(_T("AO%d"), CFBURR_D);
					m_stDefType.CfBurrD = 0;
				}
				m_stDefType.CfBurr= 0;				
				m_bPaCodeFlag[INSP_CF_BURR] = FALSE;
				m_bPaCheckFlag = TRUE;
				
			}//2020.01.16 add by mby E-
		}

		if(m_stJudgeItemInfo.TftBurr && nIndex == m_stJudgeItemPriority.TftBurr)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_BURR);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_BURR))
			{
				//strGrade = JUDGE_B;
				strGrade = pAuto->m_strValue_GradeAuto[TFT_BURR];
				if(m_stDefType.TftBurrA)
				{
					strJudge.Format(_T("AO%d"), TFTBURR);
					m_stDefType.TftBurrA = 0;
				}
				else if(m_stDefType.TftBurrB)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_B);
					m_stDefType.TftBurrB = 0;
				}
				else if(m_stDefType.TftBurrC)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_C);
					m_stDefType.TftBurrC = 0;
				}
				else if(m_stDefType.TftBurrD)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_D);
					m_stDefType.TftBurrD= 0;
				}
				m_stDefType.TftBurr= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftBurr && !m_bPaCodeFlag[INSP_TFT_BURR])
			{
				strGrade = JUDGE_A;	//2020.04.02 add by mby
				if(!m_bPaCodeFlag[INSP_TFT_BURR]){		//2020.04.02 add by mby	
					strGrade = pAuto->m_strValue_GradeAuto[TFT_BURR];
				}
				if(m_stDefType.TftBurrA)
				{
					strJudge.Format(_T("AO%d"), TFTBURR);
					m_stDefType.TftBurrA = 0;
				}
				else if(m_stDefType.TftBurrB)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_B);
					m_stDefType.TftBurrB = 0;
				}
				else if(m_stDefType.TftBurrC)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_C);
					m_stDefType.TftBurrC = 0;
				}
				else if(m_stDefType.TftBurrD)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_D);
					m_stDefType.TftBurrD= 0;
				}
				m_stDefType.TftBurr= 0;
				return TRUE;
			}//2020.01.16 add by mby S-
			//else if( bCheck == TRUE && m_bPaCodeFlag[INSP_TFT_BURR])
			else if(m_bPaCodeFlag[INSP_TFT_BURR] && !m_bPaCheckFlag)
			{
				if(m_stDefType.TftBurrA)
				{
					strJudge.Format(_T("AO%d"), TFTBURR);
					m_stDefType.TftBurrA = 0;
				}
				else if(m_stDefType.TftBurrB)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_B);
					m_stDefType.TftBurrB = 0;
				}
				else if(m_stDefType.TftBurrC)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_C);
					m_stDefType.TftBurrC = 0;
				}
				else if(m_stDefType.TftBurrD)
				{
					strJudge.Format(_T("AO%d"), TFTBURR_D);
					m_stDefType.TftBurrD= 0;
				}
				m_stDefType.TftBurr= 0;				
				m_bPaCodeFlag[INSP_TFT_BURR] = FALSE;
				m_bPaCheckFlag = TRUE;
			}
			//2020.01.16 add by mby E-
		}

		if(m_stJudgeItemInfo.CfBroken && nIndex == m_stJudgeItemPriority.CfBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_BROKEN))
			{
				strGrade = pAuto->m_strValue_GradeAuto[CF_RBROKEN];
				//strGrade = JUDGE_A;	//2018.10.25 change by mby JUDGE_B->JUDGE_A
				
				//strJudge.Format(_T("%d"), CFBROKEN);
				if(m_stDefType.CfBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN);
					m_stDefType.CfBrokenA = 0;
				}
				else if(m_stDefType.CfBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_B);
					m_stDefType.CfBrokenB = 0;
				}
				else if(m_stDefType.CfBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_C);
					m_stDefType.CfBrokenC = 0;
				}
				else if(m_stDefType.CfBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_D);
					m_stDefType.CfBrokenD= 0;
				}
				m_stDefType.CfBroken= 0;
				return TRUE;
			}
			else if (  bCheck == FALSE && m_stDefType.CfBroken && !m_bPaCodeFlag[INSP_CF_BROKEN])
			{
				strGrade = JUDGE_A;	//2018.10.25 change by mby JUDGE_B->JUDGE_A
				if(!m_bPaCodeFlag[INSP_CF_BROKEN]){		//2020.04.02 add by mby
					strGrade = pAuto->m_strValue_GradeAuto[CF_RBROKEN];
				}
				if(m_stDefType.CfBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN);
					m_stDefType.CfBrokenA = 0;
				}
				else if(m_stDefType.CfBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_B);
					m_stDefType.CfBrokenB = 0;
				}
				else if(m_stDefType.CfBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_C);
					m_stDefType.CfBrokenC = 0;
				}
				else if(m_stDefType.CfBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_D);
					m_stDefType.CfBrokenD= 0;
				}
				m_stDefType.CfBroken= 0;
				return TRUE;
			}
			//2020.01.16 add by mby S-
			//else if( bCheck == TRUE && m_bPaCodeFlag[INSP_CF_BROKEN])
			else if(m_bPaCodeFlag[INSP_CF_BROKEN] && !m_bPaCheckFlag)
			{
				if(m_stDefType.CfBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN);
					m_stDefType.CfBrokenA = 0;
				}
				else if(m_stDefType.CfBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_B);
					m_stDefType.CfBrokenB = 0;
				}
				else if(m_stDefType.CfBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_C);
					m_stDefType.CfBrokenC = 0;
				}
				else if(m_stDefType.CfBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFBROKEN_D);
					m_stDefType.CfBrokenD= 0;
				}
				m_stDefType.CfBroken= 0;
				m_bPaCodeFlag[INSP_CF_BROKEN] = FALSE;
				m_bPaCheckFlag = TRUE;
			}
			//2020.01.16 add by mby E-
		}

		if(m_stJudgeItemInfo.TftBroken && nIndex == m_stJudgeItemPriority.TftBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_BROKEN))
			{
				//strGrade = JUDGE_A;	//2018.10.25 change by mby JUDGE_B->JUDGE_A
				strGrade = pAuto->m_strValue_GradeAuto[TFT_RBROKEN];
				//strJudge.Format(_T("%d"), TFTBROKEN);
				if(m_stDefType.TftBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN);
					m_stDefType.TftBrokenA = 0;
				}
				else if(m_stDefType.TftBrokenB)
				{
					//strGrade = JUDGE_C;	//2018.11.06 add by mby 
					strJudge.Format(_T("AO%d"), TFTBROKEN_B);
					m_stDefType.TftBrokenB = 0;
				}
				else if(m_stDefType.TftBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_C);
					m_stDefType.TftBrokenC = 0;
				}
				else if(m_stDefType.TftBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_D);
					m_stDefType.TftBrokenD= 0;
				}
				m_stDefType.TftBroken= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftBroken && !m_bPaCodeFlag[INSP_TFT_BROKEN])
			{
				strGrade = JUDGE_A;	//2018.10.25 change by mby JUDGE_B->JUDGE_A
				if(!m_bPaCodeFlag[INSP_TFT_BROKEN]){		//2020.04.02 add by mby		
					strGrade = pAuto->m_strValue_GradeAuto[TFT_RBROKEN];
				}
				//strJudge.Format(_T("%d"), TFTBROKEN);
				if(m_stDefType.TftBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN);
					m_stDefType.TftBrokenA = 0;
				}
				else if(m_stDefType.TftBrokenB)
				{
					//trGrade = JUDGE_C;	//2018.11.06 add by mby 
					strJudge.Format(_T("AO%d"), TFTBROKEN_B);
					m_stDefType.TftBrokenB = 0;
				}
				else if(m_stDefType.TftBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_C);
					m_stDefType.TftBrokenC = 0;
				}
				else if(m_stDefType.TftBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_D);
					m_stDefType.TftBrokenD= 0;
				}
				m_stDefType.TftBroken= 0;
				return TRUE;
			}
			//2020.01.16 add by mby S-
			//else if( bCheck == TRUE && m_bPaCodeFlag[INSP_TFT_BROKEN])
			else if(m_bPaCodeFlag[INSP_TFT_BROKEN] && !m_bPaCheckFlag)
			{
				if(m_stDefType.TftBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN);
					m_stDefType.TftBrokenA = 0;
				}
				else if(m_stDefType.TftBrokenB)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_B);
					m_stDefType.TftBrokenB = 0;
				}
				else if(m_stDefType.TftBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_C);
					m_stDefType.TftBrokenC = 0;
				}
				else if(m_stDefType.TftBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTBROKEN_D);
					m_stDefType.TftBrokenD= 0;
				}
				m_stDefType.TftBroken= 0;
				m_bPaCodeFlag[INSP_TFT_BROKEN] = FALSE;
				m_bPaCheckFlag = TRUE;
			}
			//2020.01.16 add by mby E-
		}

		if(m_stJudgeItemInfo.CfCornerBroken && nIndex == m_stJudgeItemPriority.CfCornerBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_CORNER_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_CORNER_BROKEN))
			{
				strGrade = pAuto->m_strValue_GradeAuto[CF_CORNERBROKEN];
				//strGrade = JUDGE_C;	//2018.10.25 change by mby JUDGE_B->JUDGE_A->JUDGE_C
				//strJudge.Format(_T("%d"), CFCORNERBROKEN);
				if(m_stDefType.CfCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN);
					m_stDefType.CfCornerBrokenA = 0;
				}
				else if(m_stDefType.CfCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_B);
					m_stDefType.CfCornerBrokenB = 0;
				}
				else if(m_stDefType.CfCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_C);
					m_stDefType.CfCornerBrokenC = 0;
				}
				else if(m_stDefType.CfCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_D);
					m_stDefType.CfCornerBrokenD= 0;
				}
				m_stDefType.CfCornerBroken= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfCornerBroken && !m_bPaCodeFlag[INSP_CF_CORNER_BROKEN])
			{
				strGrade = JUDGE_A;	//2020.04.02 add by mby
				if(!m_bPaCodeFlag[INSP_CF_CORNER_BROKEN]){		//2020.04.02 add by mby	
					strGrade = pAuto->m_strValue_GradeAuto[CF_CORNERBROKEN];
				}
				//strGrade = JUDGE_C;	//2018.10.25 change by mby JUDGE_B->JUDGE_A->JUDGE_C
				//strJudge.Format(_T("%d"), CFCORNERBROKEN);
				if(m_stDefType.CfCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN);
					m_stDefType.CfCornerBrokenA = 0;
				}
				else if(m_stDefType.CfCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_B);
					m_stDefType.CfCornerBrokenB = 0;
				}
				else if(m_stDefType.CfCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_C);
					m_stDefType.CfCornerBrokenC = 0;
				}
				else if(m_stDefType.CfCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_D);
					m_stDefType.CfCornerBrokenD= 0;
				}
				m_stDefType.CfCornerBroken= 0;
				return TRUE;
			}
			//2020.01.16 add by mby S-
			//else if( bCheck == TRUE && m_bPaCodeFlag[INSP_CF_CORNER_BROKEN])
			else if(m_bPaCodeFlag[INSP_CF_CORNER_BROKEN] && !m_bPaCheckFlag)
			{
				if(m_stDefType.CfCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN);
					m_stDefType.CfCornerBrokenA = 0;
				}
				else if(m_stDefType.CfCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_B);
					m_stDefType.CfCornerBrokenB = 0;
				}
				else if(m_stDefType.CfCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_C);
					m_stDefType.CfCornerBrokenC = 0;
				}
				else if(m_stDefType.CfCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), CFCORNERBROKEN_D);
					m_stDefType.CfCornerBrokenD= 0;
				}
				m_stDefType.CfCornerBroken= 0;
				m_bPaCodeFlag[INSP_CF_CORNER_BROKEN] = FALSE;
				m_bPaCheckFlag = TRUE;
			}
			//2020.01.16 add by mby E-
		}

		if(m_stJudgeItemInfo.TftCornerBroken && nIndex == m_stJudgeItemPriority.TftCornerBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_CORNER_BROKEN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_CORNER_BROKEN))
			{
				strGrade = pAuto->m_strValue_GradeAuto[TFT_CORNERBROKEN];
				//strGrade = JUDGE_C;	//2018.10.25 change by mby JUDGE_B->JUDGE_A->JUDGE_C
				//strJudge.Format(_T("%d"), TFTCORNERBROKEN);
				if(m_stDefType.TftCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN);
					m_stDefType.TftCornerBrokenA = 0;
				}
				else if(m_stDefType.TftCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_B);
					m_stDefType.TftCornerBrokenB = 0;
				}
				else if(m_stDefType.TftCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_C);
					m_stDefType.TftCornerBrokenC = 0;
				}
				else if(m_stDefType.TftCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_D);
					m_stDefType.TftCornerBrokenD= 0;
				}
				m_stDefType.TftCornerBroken= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftCornerBroken && !m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN])
			{
				strGrade = JUDGE_A;	//2020.04.02 add by mby
				if(!m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN]){		//2020.04.02 add by mby	
					strGrade = pAuto->m_strValue_GradeAuto[TFT_CORNERBROKEN];
				}
				//strGrade = JUDGE_C;	//2018.10.25 change by mby JUDGE_B->JUDGE_A->JUDGE_C
				//strJudge.Format(_T("%d"), TFTCORNERBROKEN);
				if(m_stDefType.TftCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN);
					m_stDefType.TftCornerBrokenA = 0;
				}
				else if(m_stDefType.TftCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_B);
					m_stDefType.TftCornerBrokenB = 0;
				}
				else if(m_stDefType.TftCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_C);
					m_stDefType.TftCornerBrokenC = 0;
				}
				else if(m_stDefType.TftCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_D);
					m_stDefType.TftCornerBrokenD= 0;
				}
				m_stDefType.TftCornerBroken= 0;
				return TRUE;
			}
			//2020.01.16 add by mby S-
			//else if( bCheck == TRUE && m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN])
			else if(m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN] && !m_bPaCheckFlag)
			{
				if(m_stDefType.TftCornerBrokenA)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN);
					m_stDefType.TftCornerBrokenA = 0;
				}
				else if(m_stDefType.TftCornerBrokenB)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_B);
					m_stDefType.TftCornerBrokenB = 0;
				}
				else if(m_stDefType.TftCornerBrokenC)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_C);
					m_stDefType.TftCornerBrokenC = 0;
				}
				else if(m_stDefType.TftCornerBrokenD)
				{
					strJudge.Format(_T("AO%d"), TFTCORNERBROKEN_D);
					m_stDefType.TftCornerBrokenD= 0;
				}
				m_stDefType.TftCornerBroken= 0;
				m_bPaCodeFlag[INSP_TFT_CORNER_BROKEN] = FALSE;
				m_bPaCheckFlag = TRUE;
			}
			//2020.01.16 add by mby E-
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////
		//if(m_fnJudgeResult())
		//{
		//	strGrade  = JUDGE_A;
		//	strJudge = _T("");
		//	return TRUE;
		//}

		//if(m_stJudgeItemInfo.TftCornerBroken && nIndex == m_stJudgeItemPriority.TftCornerBroken)
		//{
		//	if (pFrame->m_stJudgeCount->bUseMode != TRUE && m_stDefType.TftCornerBroken)
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), TFTCORNERBROKEN);
		//		m_stDefType.TftCornerBroken= 0;
		//		return TRUE;
		//	}
		//	else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
		//		m_stDefType.TftCornerBroken > pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_CORNER_BROKEN])
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), TFTCORNERBROKEN);
		//		m_stDefType.TftCornerBroken= 0;
		//		return TRUE;
		//	}
		//}

		//if(m_stJudgeItemInfo.CfCornerBroken && nIndex == m_stJudgeItemPriority.CfCornerBroken)
		//{
		//	if (pFrame->m_stJudgeCount->bUseMode != TRUE && m_stDefType.CfCornerBroken)
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), CFCORNERBROKEN);
		//		m_stDefType.CfCornerBroken= 0;
		//		return TRUE;
		//	}
		//	else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
		//		m_stDefType.CfCornerBroken > pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_CORNER_BROKEN])
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), CFCORNERBROKEN);
		//		m_stDefType.CfCornerBroken= 0;
		//		return TRUE;
		//	}
		//}

		//if(m_stJudgeItemInfo.TftBroken && nIndex == m_stJudgeItemPriority.TftBroken)
		//{
		//	if (pFrame->m_stJudgeCount->bUseMode != TRUE && m_stDefType.TftBroken)
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), TFTBROKEN);
		//		m_stDefType.TftBroken= 0;
		//		return TRUE;
		//	}
		//	else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
		//		m_stDefType.TftBroken > pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_BROKEN])
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), TFTBROKEN);
		//		m_stDefType.TftBroken= 0;
		//		return TRUE;
		//	}

		//}

		//if(m_stJudgeItemInfo.CfBroken && nIndex == m_stJudgeItemPriority.CfBroken)
		//{
		//	if (pFrame->m_stJudgeCount->bUseMode != TRUE && m_stDefType.CfBroken)
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), CFBROKEN);
		//		m_stDefType.CfBroken= 0;
		//		return TRUE;
		//	}
		//	else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
		//		m_stDefType.CfBroken > pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_BROKEN])
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), CFBROKEN);
		//		m_stDefType.CfBroken= 0;
		//		return TRUE;
		//	}
		//}

		//if(m_stJudgeItemInfo.WanesBroken && nIndex == m_stJudgeItemPriority.WanesBroken)
		//{
		//	if (pFrame->m_stJudgeCount->bUseMode != TRUE && m_stDefType.WanesBroken)
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), WANESBROKEN);
		//		m_stDefType.WanesBroken = 0;
		//		return TRUE;
		//	}
		//	else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
		//		m_stDefType.WanesBroken > pFrame->m_stJudgeCount->nJudgeCount[INSP_EDGE_BROKEN])
		//	{
		//		strGrade = JUDGE_B;
		//		strJudge.Format(_T("%d"), WANESBROKEN);
		//		m_stDefType.WanesBroken = 0;
		//		return TRUE;
		//	}
		//}


		if(m_stJudgeItemInfo.TftWhiteBlur && nIndex == m_stJudgeItemPriority.TftWhiteBlur)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_WHITE_TURBIDITY);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_WHITE_TURBIDITY))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTWHITEBLUR);
				m_stDefType.TftWhiteBlur = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftWhiteBlur)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTWHITEBLUR);
				m_stDefType.TftWhiteBlur = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.ItoPeel && nIndex == m_stJudgeItemPriority.ItoPeel)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ITO);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ITO))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), ITOPEEL);
				m_stDefType.ItoPeel = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.ItoPeel)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), ITOPEEL);
				m_stDefType.ItoPeel = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfWhiteBlur && nIndex == m_stJudgeItemPriority.CfWhiteBlur)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_WHITE_TURBIDITY);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_WHITE_TURBIDITY))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFWHITEBLUR);
				m_stDefType.CfWhiteBlur = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfWhiteBlur)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFWHITEBLUR);
				m_stDefType.CfWhiteBlur = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.SocketScrach && nIndex == m_stJudgeItemPriority.SocketScrach)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PAD_SCRATCH);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PAD_SCRATCH))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), SOCKETSCRACH);
				m_stDefType.SocketScrach =0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.SocketScrach)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), SOCKETSCRACH);
				m_stDefType.SocketScrach =0;
				return TRUE;
			}			
		}

		if(m_stJudgeItemInfo.SocketBurn && nIndex == m_stJudgeItemPriority.SocketBurn)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PAD_BURNT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PAD_BURNT))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), SOCKETBURN);
				m_stDefType.SocketBurn=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.SocketBurn)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), SOCKETBURN);
				m_stDefType.SocketBurn=0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TftDimpleLine && nIndex == m_stJudgeItemPriority.TftDimpleLine)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_DIMPLE_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_DIMPLE_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTDIMPLELINE);
				m_stDefType.TftDimpleLine= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftDimpleLine)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTDIMPLELINE);
				m_stDefType.TftDimpleLine= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfDimpleLine && nIndex == m_stJudgeItemPriority.CfDimpleLine)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_DIMPLE_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_DIMPLE_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFDIMPLELINE);
				m_stDefType.CfDimpleLine = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfDimpleLine)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFDIMPLELINE);
				m_stDefType.CfDimpleLine = 0;
				return TRUE;
			}
		}
		if(m_stJudgeItemInfo.TftPimpleLine && nIndex == m_stJudgeItemPriority.TftPimpleLine)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PIMPLE_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PIMPLE_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPIMPLELINE);
				m_stDefType.TftPimpleLine= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftPimpleLine)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPIMPLELINE);
				m_stDefType.TftPimpleLine= 0;
				return TRUE;
			}
		}
		if(m_stJudgeItemInfo.CfPimpleLine && nIndex == m_stJudgeItemPriority.CfPimpleLine)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PIMPLE_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PIMPLE_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLELINE);
				m_stDefType.CfPimpleLine= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfPimpleLine)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLELINE);
				m_stDefType.CfPimpleLine= 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TftScrach && nIndex == m_stJudgeItemPriority.TftScrach)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_BLACK_SCRATCH);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_BLACK_SCRATCH))
			{
				strGrade = pAuto->m_strValue_GradeAuto[TFT_SCARCH];
				//strGrade = JUDGE_B;
				strJudge.Format(_T("AO%d"), TFTSCRATCH);
				m_stDefType.TftScrach = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftScrach)
			{
				strGrade = pAuto->m_strValue_GradeAuto[TFT_SCARCH];
				//strGrade = JUDGE_B;
				strJudge.Format(_T("AO%d"), TFTSCRATCH);
				m_stDefType.TftScrach = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfScrach && nIndex == m_stJudgeItemPriority.CfScrach)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_BLACK_SCRATCH);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_BLACK_SCRATCH))
			{
				strGrade = pAuto->m_strValue_GradeAuto[CF_SCARCH];
				//strGrade = JUDGE_B;
				strJudge.Format(_T("AO%d"), CFSCRATCH);
				m_stDefType.CfScrach = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfScrach)
			{
				strGrade = pAuto->m_strValue_GradeAuto[CF_SCARCH];
				//strGrade = JUDGE_B;
				strJudge.Format(_T("AO%d"), CFSCRATCH);
				m_stDefType.CfScrach = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.TftDimplePoint && nIndex == m_stJudgeItemPriority.TftDimplePoint)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_DIMPLE_POINT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_DIMPLE_POINT))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTDIMPLEPOINT);
				m_stDefType.TftDimplePoint = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftDimplePoint)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTDIMPLEPOINT);
				m_stDefType.TftDimplePoint = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfDimplePoint && nIndex == m_stJudgeItemPriority.CfDimplePoint)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_DIMPLE_POINT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_DIMPLE_POINT))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFDIMPLEPOINT);
				m_stDefType.CfDimplePoint = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfDimplePoint)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFDIMPLEPOINT);
				m_stDefType.CfDimplePoint = 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.TftPimplePoint && nIndex == m_stJudgeItemPriority.TftPimplePoint)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PIMPLE_POINT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PIMPLE_POINT))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPIMPLEPOINT);
				m_stDefType.TftPimplePoint = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.TftPimplePoint)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPIMPLEPOINT);
				m_stDefType.TftPimplePoint = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfPimplePoint && nIndex == m_stJudgeItemPriority.CfPimplePoint)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PIMPLE_POINT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PIMPLE_POINT))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLEPOINT);
				m_stDefType.CfPimplePoint = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfPimplePoint)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLEPOINT);
				m_stDefType.CfPimplePoint = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfWavePattern && nIndex == m_stJudgeItemPriority.CfWavePattern)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_WAVE_PATTERN);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_WAVE_PATTERN))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFWAVEPATTERN);
				m_stDefType.CfWavePattern = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfWavePattern)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFWAVEPATTERN);
				m_stDefType.CfWavePattern = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfPimpleCircle && nIndex == m_stJudgeItemPriority.CfPimpleCircle)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_PIMPLE_CIRCLE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_PIMPLE_CIRCLE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLECIRCLE);
				m_stDefType.CfPimpleCircle = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfPimpleCircle)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPIMPLECIRCLE);
				m_stDefType.CfPimpleCircle = 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.EtcLine && nIndex == m_stJudgeItemPriority.EtcLine)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETC_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETC_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), ETCLINE);
				m_stDefType.EtcLine = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.EtcLine)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("AO%d"), ETCLINE);
				m_stDefType.EtcLine = 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.EtcPoint && nIndex == m_stJudgeItemPriority.EtcPoint)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETC_POINT);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETC_POINT))
			{
				//strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strGrade = pAuto->m_strValue_GradeAuto[ETC_POINT];
				strJudge.Format(_T("AO%d"), ETCPOINT);
				m_stDefType.EtcPoint = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.EtcPoint)
			{
				//strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strGrade = pAuto->m_strValue_GradeAuto[ETC_POINT];
				strJudge.Format(_T("AO%d"), ETCPOINT);
				m_stDefType.EtcPoint = 0;
				return TRUE;
			}			
		}


		if(m_stJudgeItemInfo.TftEdgeilegal && nIndex == m_stJudgeItemPriority.TftEdgeilegal)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETCHING_LINE);

			if(  bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETCHING_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTEDGEILEGAL);
				m_stDefType.TftEdgeilegal = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.TftEdgeilegal)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTEDGEILEGAL);
				m_stDefType.TftEdgeilegal = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfEdgeilegal && nIndex == m_stJudgeItemPriority.CfEdgeilegal)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETCHING_LINE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETCHING_LINE))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFEDGEILEGAL);
				m_stDefType.CfEdgeilegal = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE &&  m_stDefType.CfEdgeilegal)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFEDGEILEGAL);
				m_stDefType.CfEdgeilegal = 0;
				return TRUE;
			}
		}

		if(m_stJudgeItemInfo.CfMeasureFail && nIndex == m_stJudgeItemPriority.CfMeasureFail)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_MEASURE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_MEASURE))
			{
				strGrade = JUDGE_A;		//2018.10.25 change by mby JUDGE_B->JUDGE_A
				strJudge.Format(_T("AO%d"), CFMEASUREFAIL);	
				m_stDefType.CfMeasureFail = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfMeasureFail)
			{
				strGrade = JUDGE_A;		//2018.10.25 change by mby JUDGE_B->JUDGE_A
				strJudge.Format(_T("AO%d"), CFMEASUREFAIL);	
				m_stDefType.CfMeasureFail = 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.TftMeasureFail && nIndex == m_stJudgeItemPriority.TftMeasureFail)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_MEASURE);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_MEASURE))
			{
				strGrade = JUDGE_A;		//2018.10.25 change by mby JUDGE_B->JUDGE_A
				strJudge.Format(_T("AO%d"), TFTMEASUREFAIL);
				m_stDefType.TftMeasureFail= 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftMeasureFail)
			{
				strGrade = JUDGE_A;		//2018.10.25 change by mby JUDGE_B->JUDGE_A
				strJudge.Format(_T("AO%d"), TFTMEASUREFAIL);
				m_stDefType.TftMeasureFail= 0;
				return TRUE;
			}
		}


		if(m_stJudgeItemInfo.Burr && nIndex == m_stJudgeItemPriority.Burr)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_BURR);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_BURR))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), BURR);
				m_stDefType.Burr=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.Burr)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), BURR);
				m_stDefType.Burr=0;
				return TRUE;
			}
		}

		//2016.05.11 Mr.Chu		Align Fail Grade 판별
		if(m_stJudgeItemInfo.AlignFail && nIndex == m_stJudgeItemPriority.AlignFail)
		{
			if (m_stDefType.AlignFail)
			{
				//strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strGrade = pAuto->m_strValue_GradeAuto[GRADE_ALIGN_FAIL];				
				strJudge.Format(_T("AO%d"), ALIGNFAIL);
				m_stDefType.AlignFail = 0;

				return TRUE;
			}
		}

		//2016.02.24 우선 순위 마지막
		if(m_stJudgeItemInfo.PolMiss && nIndex == m_stJudgeItemPriority.PolMiss)
		{
			if (m_stDefType.PolMiss)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), POLMISS);
				m_stDefType.PolMiss = 0;
				return TRUE;
			}
		}

		//2017.04.12	Mr.Chu
		if(m_stJudgeItemInfo.CfProtection && nIndex == m_stJudgeItemPriority.CfProtection)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_PROTECTION);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_PROTECTION))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPROTECTION);
				m_stDefType.CfProtection=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfProtection)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), CFPROTECTION);
				m_stDefType.CfProtection=0;
				return TRUE;
			}
		}
		if(m_stJudgeItemInfo.TftProtection && nIndex == m_stJudgeItemPriority.TftProtection)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_PROTECTION);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_PROTECTION))
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPROTECTION);
				m_stDefType.TftProtection=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftProtection)
			{
				strGrade = JUDGE_A; // strGrade = JUDGE_B; 20181212 by ryu
				strJudge.Format(_T("%d"), TFTPROTECTION);
				m_stDefType.TftProtection=0;
				return TRUE;
			}
		}
		// 2019.03.15 by ryu check yo~ count?
		if(m_stJudgeItemInfo.Bgradecheck && nIndex == m_stJudgeItemPriority.Bgradecheck)
		{
			if(m_stDefType.Bgradecheck )
			{
				//strGrade = JUDGE_A; 
				//strJudge.Format(_T("%d"), BGRADECHECK);
				m_stDefType.Bgradecheck=0;
				return TRUE;
			}
		}
		if(m_stJudgeItemInfo.Cgradecheck && nIndex == m_stJudgeItemPriority.Cgradecheck)
		{
			if(m_stDefType.Cgradecheck)
			{
				//strGrade = JUDGE_A; 
				//strJudge.Format(_T("%d"), CGRADECHECK);
				m_stDefType.Cgradecheck=0;
				return TRUE;
			}
		}	
		///////////////////////////

	}
	///

	return FALSE;
}

BOOL CDataManager::m_fnGetJudgeGrade_ABI(BOOL &bPanelCodeTest, CString &strGrade,CString &strJudge)
{
	//2015.07.20
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	BOOL bCheck = FALSE;
	int nIndex = 0;		//2016.12.12 Mr.Chu
	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_stJudgeItemInfo.initDefType();
	m_stJudgeItemInfo.setInfoPath(PNL_JUDGE_ITEM, strRoot);
	m_stJudgeItemInfo.GetDefInfo();

	m_stJudgeItemPriority.initDefType();
	m_stJudgeItemPriority.setInfoPath(PNL_JUDGE_PRIORITY, strRoot);
	m_stJudgeItemPriority.GetDefInfo();

	while(TRUE)
	{
		if( nIndex++ >= m_stJudgeItemInfo.TotalCnt )
			break;


		// PanelCodeTest
		if( m_stPanelCodeTest.bJudgeInfo && nIndex == m_stJudgeItemPriority.PanelCodeTest )
		{
			bPanelCodeTest = TRUE;
			m_stJudgeItemInfo.PanelCodeTest = FALSE;
		}
		////////////////////////////////////////////////
 		
 		if(m_stJudgeItemInfo.CfCellBroken && nIndex == m_stJudgeItemPriority.CfCellBroken)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_CELL_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_CELL_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFCELLBROKEN);
 				m_stDefType.CfCellBroken= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE &&  m_stDefType.CfCellBroken)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFCELLBROKEN);
				m_stDefType.CfCellBroken= 0;
				return TRUE;
			}
 		}
 
 
 		if(m_stJudgeItemInfo.TftCellBroken && nIndex == m_stJudgeItemPriority.TftCellBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_CELL_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_CELL_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFTCELLBROKEN);
 				m_stDefType.TftCellBroken= 0;
 				return TRUE;
 			}
			if ( bCheck == FALSE && m_stDefType.TftCellBroken)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTCELLBROKEN);
				m_stDefType.TftCellBroken= 0;
				return TRUE;
			}
 		}
 
 		//
 		if(m_stJudgeItemInfo.CFCLuminance && nIndex == m_stJudgeItemPriority.CFCLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CFC_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CFC_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFCLUMINANCE);
 				m_stDefType.CFCLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CFCLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFCLUMINANCE);
				m_stDefType.CFCLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CBCLuminance && nIndex == m_stJudgeItemPriority.CBCLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CBC_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CBC_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CBCLUMINANCE);
 				m_stDefType.CBCLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CBCLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CBCLUMINANCE);
				m_stDefType.CBCLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CFSLuminance && nIndex == m_stJudgeItemPriority.CFSLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CFS_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CFS_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFSLUMINANCE);
 				m_stDefType.CFSLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CFSLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFSLUMINANCE);
				m_stDefType.CFSLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CBSLuminance && nIndex == m_stJudgeItemPriority.CBSLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CBS_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CBS_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CBSLUMINANCE);
 				m_stDefType.CBSLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CBSLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CBSLUMINANCE);
				m_stDefType.CBSLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TFCLuminance && nIndex == m_stJudgeItemPriority.TFCLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFC_LUMINANCE);

 			if( bCheck == TRUE && pFrame->m_stJudgeCount->bUseMode == TRUE && 
 				m_fnPanelJudgeResult(INSP_TFC_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFCLUMINANCE);
 				m_stDefType.TFCLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TFCLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFCLUMINANCE);
				m_stDefType.TFCLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TBCLuminance && nIndex == m_stJudgeItemPriority.TBCLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TBC_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TBC_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TBCLUMINANCE);
 				m_stDefType.TBCLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TBCLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TBCLUMINANCE);
				m_stDefType.TBCLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TFSLuminance && nIndex == m_stJudgeItemPriority.TFSLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFS_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFS_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFSLUMINANCE);
 				m_stDefType.TFSLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TFSLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFSLUMINANCE);
				m_stDefType.TFSLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TBSLuminance && nIndex == m_stJudgeItemPriority.TBSLuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TBS_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TBS_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TBSLUMINANCE);
 				m_stDefType.TBSLuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TBSLuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TBSLUMINANCE);
				m_stDefType.TBSLuminance= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TDILuminance && nIndex == m_stJudgeItemPriority.TDILuminance)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TDI_LUMINANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TDI_LUMINANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TDILUMINANCE);
 				m_stDefType.TDILuminance= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TDILuminance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TDILUMINANCE);
				m_stDefType.TDILuminance= 0;
				return TRUE;
			}
 		}
 		//
 
 		if(m_stJudgeItemInfo.CfBurr && nIndex == m_stJudgeItemPriority.CfBurr)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_BURR);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_BURR))
 			{
 				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), CFBURR);
				if(m_stDefType.CfBurrA)
				{
					strJudge.Format(_T("%d"), CFBURR);
					m_stDefType.CfBurrA = 0;
				}
				else if(m_stDefType.CfBurrB)
				{
					strJudge.Format(_T("%d"), CFBURR_B);
					m_stDefType.CfBurrB = 0;
				}
				else if(m_stDefType.CfBurrC)
				{
					strJudge.Format(_T("%d"), CFBURR_C);
					m_stDefType.CfBurrC = 0;
				}
				else if(m_stDefType.CfBurrD)
				{
					strJudge.Format(_T("%d"), CFBURR_D);
					m_stDefType.CfBurrD = 0;
				}
 				m_stDefType.CfBurr= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CfBurr)	
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), CFBURR);
				if(m_stDefType.CfBurrA)
				{
					strJudge.Format(_T("%d"), CFBURR);
					m_stDefType.CfBurrA = 0;
				}
				else if(m_stDefType.CfBurrB)
				{
					strJudge.Format(_T("%d"), CFBURR_B);
					m_stDefType.CfBurrB = 0;
				}
				else if(m_stDefType.CfBurrC)
				{
					strJudge.Format(_T("%d"), CFBURR_C);
					m_stDefType.CfBurrC = 0;
				}
				else if(m_stDefType.CfBurrD)
				{
					strJudge.Format(_T("%d"), CFBURR_D);
					m_stDefType.CfBurrD = 0;
				}
				m_stDefType.CfBurr= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TftBurr && nIndex == m_stJudgeItemPriority.TftBurr)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_BURR);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_BURR))
 			{
 				strGrade = JUDGE_D;
 				//strJudge.Format(_T("%d"), TFTBURR);
				if(m_stDefType.TftBurrA)
				{
					strJudge.Format(_T("%d"), TFTBURR);
					m_stDefType.TftBurrA = 0;
				}
				else if(m_stDefType.TftBurrB)
				{
					strJudge.Format(_T("%d"), TFTBURR_B);
					m_stDefType.TftBurrB = 0;
				}
				else if(m_stDefType.TftBurrC)
				{
					strJudge.Format(_T("%d"), TFTBURR_C);
					m_stDefType.TftBurrC = 0;
				}
				else if(m_stDefType.TftBurrD)
				{
					strJudge.Format(_T("%d"), TFTBURR_D);
					m_stDefType.TftBurrD= 0;
				}
 				m_stDefType.TftBurr= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TftBurr)
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), TFTBURR);
				if(m_stDefType.TftBurrA)
				{
					strJudge.Format(_T("%d"), TFTBURR);
					m_stDefType.TftBurrA = 0;
				}
				else if(m_stDefType.TftBurrB)
				{
					strJudge.Format(_T("%d"), TFTBURR_B);
					m_stDefType.TftBurrB = 0;
				}
				else if(m_stDefType.TftBurrC)
				{
					strJudge.Format(_T("%d"), TFTBURR_C);
					m_stDefType.TftBurrC = 0;
				}
				else if(m_stDefType.TftBurrD)
				{
					strJudge.Format(_T("%d"), TFTBURR_D);
					m_stDefType.TftBurrD= 0;
				}
				m_stDefType.TftBurr= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CfBroken && nIndex == m_stJudgeItemPriority.CfBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				//strJudge.Format(_T("%d"), CFBROKEN);
				if(m_stDefType.CfBrokenA)
				{
					strJudge.Format(_T("%d"), CFBROKEN);
					m_stDefType.CfBrokenA = 0;
				}
				else if(m_stDefType.CfBrokenB)
				{
					strJudge.Format(_T("%d"), CFBROKEN_B);
					m_stDefType.CfBrokenB = 0;
				}
				else if(m_stDefType.CfBrokenC)
				{
					strJudge.Format(_T("%d"), CFBROKEN_C);
					m_stDefType.CfBrokenC = 0;
				}
				else if(m_stDefType.CfBrokenD)
				{
					strJudge.Format(_T("%d"), CFBROKEN_D);
					m_stDefType.CfBrokenD= 0;
				}
 				m_stDefType.CfBroken= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CfBroken)
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), CFBROKEN);
				if(m_stDefType.CfBrokenA)
				{
					strJudge.Format(_T("%d"), CFBROKEN);
					m_stDefType.CfBrokenA = 0;
				}
				else if(m_stDefType.CfBrokenB)
				{
					strJudge.Format(_T("%d"), CFBROKEN_B);
					m_stDefType.CfBrokenB = 0;
				}
				else if(m_stDefType.CfBrokenC)
				{
					strJudge.Format(_T("%d"), CFBROKEN_C);
					m_stDefType.CfBrokenC = 0;
				}
				else if(m_stDefType.CfBrokenD)
				{
					strJudge.Format(_T("%d"), CFBROKEN_D);
					m_stDefType.CfBrokenD= 0;
				}
				m_stDefType.CfBroken= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TftBroken && nIndex == m_stJudgeItemPriority.TftBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				//strJudge.Format(_T("%d"), TFTBROKEN);
				if(m_stDefType.TftBrokenA)
				{
					strJudge.Format(_T("%d"), TFTBROKEN);
					m_stDefType.TftBrokenA = 0;
				}
				else if(m_stDefType.TftBrokenB)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_B);
					m_stDefType.TftBrokenB = 0;
				}
				else if(m_stDefType.TftBrokenC)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_C);
					m_stDefType.TftBrokenC = 0;
				}
				else if(m_stDefType.TftBrokenD)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_D);
					m_stDefType.TftBrokenD= 0;
				}
 				m_stDefType.TftBroken= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TftBroken)
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), TFTBROKEN);
				if(m_stDefType.TftBrokenA)
				{
					strJudge.Format(_T("%d"), TFTBROKEN);
					m_stDefType.TftBrokenA = 0;
				}
				else if(m_stDefType.TftBrokenB)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_B);
					m_stDefType.TftBrokenB = 0;
				}
				else if(m_stDefType.TftBrokenC)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_C);
					m_stDefType.TftBrokenC = 0;
				}
				else if(m_stDefType.TftBrokenD)
				{
					strJudge.Format(_T("%d"), TFTBROKEN_D);
					m_stDefType.TftBrokenD= 0;
				}
				m_stDefType.TftBroken= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CfCornerBroken && nIndex == m_stJudgeItemPriority.CfCornerBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_CORNER_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_CORNER_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				//strJudge.Format(_T("%d"), CFCORNERBROKEN);
				if(m_stDefType.CfCornerBrokenA)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN);
					m_stDefType.CfCornerBrokenA = 0;
				}
				else if(m_stDefType.CfCornerBrokenB)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_B);
					m_stDefType.CfCornerBrokenB = 0;
				}
				else if(m_stDefType.CfCornerBrokenC)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_C);
					m_stDefType.CfCornerBrokenC = 0;
				}
				else if(m_stDefType.CfCornerBrokenD)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_D);
					m_stDefType.CfCornerBrokenD= 0;
				}
 				m_stDefType.CfCornerBroken= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CfCornerBroken)
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), CFCORNERBROKEN);
				if(m_stDefType.CfCornerBrokenA)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN);
					m_stDefType.CfCornerBrokenA = 0;
				}
				else if(m_stDefType.CfCornerBrokenB)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_B);
					m_stDefType.CfCornerBrokenB = 0;
				}
				else if(m_stDefType.CfCornerBrokenC)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_C);
					m_stDefType.CfCornerBrokenC = 0;
				}
				else if(m_stDefType.CfCornerBrokenD)
				{
					strJudge.Format(_T("%d"), CFCORNERBROKEN_D);
					m_stDefType.CfCornerBrokenD= 0;
				}
				m_stDefType.CfCornerBroken= 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.TftCornerBroken && nIndex == m_stJudgeItemPriority.TftCornerBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_CORNER_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_CORNER_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				//strJudge.Format(_T("%d"), TFTCORNERBROKEN);
				if(m_stDefType.TftCornerBrokenA)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN);
					m_stDefType.TftCornerBrokenA = 0;
				}
				else if(m_stDefType.TftCornerBrokenB)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_B);
					m_stDefType.TftCornerBrokenB = 0;
				}
				else if(m_stDefType.TftCornerBrokenC)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_C);
					m_stDefType.TftCornerBrokenC = 0;
				}
				else if(m_stDefType.TftCornerBrokenD)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_D);
					m_stDefType.TftCornerBrokenD= 0;
				}
 				m_stDefType.TftCornerBroken= 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TftCornerBroken)
			{
				strGrade = JUDGE_D;
				//strJudge.Format(_T("%d"), TFTCORNERBROKEN);
				if(m_stDefType.TftCornerBrokenA)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN);
					m_stDefType.TftCornerBrokenA = 0;
				}
				else if(m_stDefType.TftCornerBrokenB)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_B);
					m_stDefType.TftCornerBrokenB = 0;
				}
				else if(m_stDefType.TftCornerBrokenC)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_C);
					m_stDefType.TftCornerBrokenC = 0;
				}
				else if(m_stDefType.TftCornerBrokenD)
				{
					strJudge.Format(_T("%d"), TFTCORNERBROKEN_D);
					m_stDefType.TftCornerBrokenD= 0;
				}
				m_stDefType.TftCornerBroken= 0;
				return TRUE;
			}
 		}
 
 		//
 		if(m_stJudgeItemInfo.ItoPeel && nIndex == m_stJudgeItemPriority.ItoPeel)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ITO);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ITO))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), ITOPEEL);
 				m_stDefType.ItoPeel = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.ItoPeel)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), ITOPEEL);
				m_stDefType.ItoPeel = 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.Contact && nIndex == m_stJudgeItemPriority.Contact)
 		{
 			if (m_strSemaGrade.Find(_T("ITO_NG")) != -1)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), ITORESISTILEGAL);
 				m_strSemaGrade = _T("");
 				return TRUE;
 			}
 			if (m_strSemaGrade.Find(_T("CONTACT_NG")) != -1)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CONTACTFAIL);
 				m_strSemaGrade = _T("");
 				return TRUE;
 			}
 		}
 
 		//
 		if(m_stJudgeItemInfo.mura && nIndex == m_stJudgeItemPriority.mura)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_MURA);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_MURA))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), MURA);
 				m_stDefType.mura = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.mura)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), MURA);
				m_stDefType.mura = 0;
				return TRUE;
			}
 		}
 
 
 		if(m_stJudgeItemInfo.CfMeasureFail && nIndex == m_stJudgeItemPriority.CfMeasureFail)
 		{
 			if (/*pFrame->m_stJudgeCount->bUseMode != TRUE &&*/ m_stDefType.CfMeasureFail)
 			{
 				strGrade = JUDGE_D;		
 				strJudge.Format(_T("%d"), CFPOLMEASUREFAIL);		
 				m_stDefType.CfMeasureFail = 0;
 				return TRUE;
 			}
 			//else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
 			//	m_fnPanelJudgeResult(INSP_CF_MEASURE))
 			//{
 			//	strGrade = JUDGE_D;		
 			//	strJudge.Format(_T("%d"), CFPOLMEASUREFAIL);		
 			//	m_stDefType.CfMeasureFail = 0;
 			//	return TRUE;
 			//}
 		}
 
 
 		if(m_stJudgeItemInfo.TftMeasureFail && nIndex == m_stJudgeItemPriority.TftMeasureFail)
 		{
 			if (/*pFrame->m_stJudgeCount->bUseMode != TRUE &&*/ m_stDefType.TftMeasureFail)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFTPOLMEASUREFAIL);
 				m_stDefType.TftMeasureFail = 0;
 				return TRUE;
 			}
 			/*else if(pFrame->m_stJudgeCount->bUseMode == TRUE && 
 			m_fnPanelJudgeResult(INSP_TFT_MEASURE))
 			{
 			strGrade = JUDGE_D;
 			strJudge.Format(_T("%d"), TFTPOLMEASUREFAIL);
 			m_stDefType.TftMeasureFail = 0;
 			return TRUE;
 			}*/
 
 		}
 
 
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S
 
 		if(m_stJudgeItemInfo.CFpolSubstance && nIndex == m_stJudgeItemPriority.CFpolSubstance)				// Add by KYH01	
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_ALIEN_SUBSTANCE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_ALIEN_SUBSTANCE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFPOLSUBSTANCE);
 				m_stDefType.CFpolSubstance = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CFpolSubstance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFPOLSUBSTANCE);
				m_stDefType.CFpolSubstance = 0;
				return TRUE;
			}
 		}

		if(m_stJudgeItemInfo.TFTpolSubstance && nIndex == m_stJudgeItemPriority.TFTpolSubstance)				// Add by KYH01			
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_ALIEN_SUBSTANCE+1);


			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_ALIEN_SUBSTANCE+1))
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPOLSUBSTANCE);
				m_stDefType.TFTpolSubstance = 0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TFTpolSubstance)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPOLSUBSTANCE);
				m_stDefType.TFTpolSubstance = 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.CfPolBubble && nIndex == m_stJudgeItemPriority.CfPolBubble)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_BUBBLE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_BUBBLE) && m_stDefType.CfPolBubble)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFPOLBUBBLE);
 				m_stDefType.CfPolBubble = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CfPolBubble)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFPOLBUBBLE);
 				m_stDefType.CfPolBubble = 0;
 				return TRUE;
 			}
 		}
 
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E
 
 		if(m_stJudgeItemInfo.CfPolScrach && nIndex == m_stJudgeItemPriority.CfPolScrach)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_SCRATCH);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_SCRATCH) && m_stDefType.CfPolScrach)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), CFPOLSCRATCH);
 				m_stDefType.CfPolScrach = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.CfPolScrach)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFPOLSCRATCH);
				m_stDefType.CfPolScrach = 0;
				return TRUE;
			}
 		}
 
 
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S
 		if(m_stJudgeItemInfo.TftPolBubble && nIndex == m_stJudgeItemPriority.TftPolBubble)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_BUBBLE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_BUBBLE) && m_stDefType.TftPolBubble)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFTPOLBUBBLE);
 				m_stDefType.TftPolBubble = 0 ;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TftPolBubble)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPOLBUBBLE);
				m_stDefType.TftPolBubble = 0 ;
				return TRUE;
			}
 		}
 
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E
 
 		if(m_stJudgeItemInfo.TftPolScrach && nIndex == m_stJudgeItemPriority.TftPolScrach)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_SCRATCH);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_SCRATCH) && m_stDefType.TftPolScrach)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), TFTPOLSCRATCH);
 				m_stDefType.TftPolScrach = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.TftPolScrach)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPOLSCRATCH);
				m_stDefType.TftPolScrach = 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.WanesBroken && nIndex == m_stJudgeItemPriority.WanesBroken)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_EDGE_BROKEN);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_EDGE_BROKEN))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), WANESBROKEN);
 				m_stDefType.WanesBroken = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.WanesBroken)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), WANESBROKEN);
				m_stDefType.WanesBroken = 0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.SocketScrach && nIndex == m_stJudgeItemPriority.SocketScrach)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PAD_SCRATCH);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PAD_SCRATCH))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), SOCKETSCRACH);
 				m_stDefType.SocketScrach =0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.SocketScrach)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), SOCKETSCRACH);
				m_stDefType.SocketScrach =0;
				return TRUE;
			}
 		}
 
 		if(m_stJudgeItemInfo.SocketBurn && nIndex == m_stJudgeItemPriority.SocketBurn)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_PAD_BURNT);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_PAD_BURNT))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), SOCKETBURN);
 				m_stDefType.SocketBurn=0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.SocketBurn)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), SOCKETBURN);
				m_stDefType.SocketBurn=0;
				return TRUE;
			}
 		}
 
 		//2016.09.23 Mr.Chu
 		if(m_stJudgeItemInfo.EtcLine && nIndex == m_stJudgeItemPriority.EtcLine)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETC_LINE);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETC_LINE))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), ETCLINE);
 				m_stDefType.EtcLine = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.EtcLine)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), ETCLINE);
				m_stDefType.EtcLine = 0;
				return TRUE;
			}
 		}
 
 
 		if(m_stJudgeItemInfo.EtcPoint && nIndex == m_stJudgeItemPriority.EtcPoint)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_ETC_POINT);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_ETC_POINT))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), ETCPOINT);
 				m_stDefType.EtcPoint = 0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.EtcPoint)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), ETCPOINT);
				m_stDefType.EtcPoint = 0;
				return TRUE;
			}
 		}
 		//
 
 
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S
 		//2015.06.04 add by IKM - S 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
 		if(m_stJudgeItemInfo.PolMinor && nIndex == m_stJudgeItemPriority.PolMinor)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_POL_MINOR);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_POL_MINOR))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), POLMINOR);
 				m_stDefType.PolMinor=0;
 				return TRUE;	
 			}
			else if ( bCheck == FALSE && m_stDefType.PolMinor)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), POLMINOR);
				m_stDefType.PolMinor=0;
				return TRUE;
			}
 		}
 
 		////2015.06.04 add by IKM - E 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
 		if(m_stJudgeItemInfo.Burr && nIndex == m_stJudgeItemPriority.Burr)
 		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_BURR);

 			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_BURR))
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), BURR);
 				m_stDefType.Burr=0;
 				return TRUE;
 			}
			else if ( bCheck == FALSE && m_stDefType.Burr)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), BURR);
				m_stDefType.Burr=0;
				return TRUE;
			}
 		}
 		//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E
 
 		//2016.05.11 Mr.Chu		Align Fail Grade 판별
 		if(m_stJudgeItemInfo.AlignFail && nIndex == m_stJudgeItemPriority.AlignFail)
 		{
 			if (m_stDefType.AlignFail)
 			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), ALIGNFAIL);
 				m_stDefType.AlignFail = 0;
 
 				return TRUE;
 			}
 		}
 
 		//2016.02.24 우선 순위 마지막
 		if(m_stJudgeItemInfo.PolMiss && nIndex == m_stJudgeItemPriority.PolMiss)
 		{
			if (m_stDefType.PolMiss)
			{
 				strGrade = JUDGE_D;
 				strJudge.Format(_T("%d"), POLMISS);
 				m_stDefType.PolMiss = 0;
 				return TRUE;
 			}
 		}

		//2017.04.12	Mr.Chu
		if(m_stJudgeItemInfo.CfProtection && nIndex == m_stJudgeItemPriority.CfProtection)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_CF_PROTECTION);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_CF_PROTECTION))
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFPROTECTION);
				m_stDefType.CfProtection=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.CfProtection)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), CFPROTECTION);
				m_stDefType.CfProtection=0;
				return TRUE;
			}
		}
		if(m_stJudgeItemInfo.TftProtection && nIndex == m_stJudgeItemPriority.TftProtection)
		{
			bCheck = m_fnPanelJudgeResultCheck(INSP_TFT_PROTECTION);

			if( bCheck == TRUE && m_fnPanelJudgeResult(INSP_TFT_PROTECTION))
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPROTECTION);
				m_stDefType.TftProtection=0;
				return TRUE;
			}
			else if ( bCheck == FALSE && m_stDefType.TftProtection)
			{
				strGrade = JUDGE_D;
				strJudge.Format(_T("%d"), TFTPROTECTION);
				m_stDefType.TftProtection=0;
				return TRUE;
			}
		}

		////////////////////////////////////////////////
	}


	strGrade  = JUDGE_A;
	strJudge = _T("");
	return FALSE;
}

CString CDataManager::m_fnChangeDefectImageName( CString strDefType, CString strDefNo)
{
	CFileFind finder;
	CString	cFileName, cImgtemp, cImgName, strVal = _T("");
	CString  resToken;	
	int curPos, nLoopCnt, nImgNameCnt = 0;
	BOOL bWorking;

	bWorking = finder.FindFile(m_strDefImgSavePath + _T("\\*.*"));	// 모든화일

	while(bWorking == TRUE)
	{	
		bWorking = finder.FindNextFile();	// 화일하나를 찾음

		if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		// 화일이름
		cFileName = finder.GetFileName();	// 화일이름 얻기

		if (cFileName.Find(EXTENSION_JPG))
		{
			curPos = 0;
			nLoopCnt =0;
			nImgNameCnt = 0;
			cImgtemp = cFileName;
			resToken= cImgtemp.Tokenize(_T("-"),curPos);

			while (resToken != "")
			{

				if (resToken.Find(strDefType)!= -1)
				{
					nImgNameCnt++;
				}							
				if (nLoopCnt == 1 && _ttoi(resToken)  == _ttoi(strDefNo))
				{
					nImgNameCnt++;
				}						

				if (curPos >= cImgtemp.GetLength())	break;
				resToken= cImgtemp.Tokenize(_T("-"),curPos);
				nLoopCnt++;
			}

			if (nImgNameCnt >=2)
			{		
				strVal = cFileName;
				break;
			}
		}
	}
	finder.Close();
	return strVal;
}


CString CDataManager::m_fnGetMeasureData( CString strval, int nvisionCnt, int nCutCnt)
{
	G_AddLog(3, 3, _T("m_fnGetMeasureData() - Start"));
	CString	cMestemp, cMesVal = _T(""), strVal = _T("");
	CString  resToken;	
	int curPos, nLoopCnt, nMeasureCnt = 0;
	curPos = 0;
	nLoopCnt =0;
	nMeasureCnt = 0;
	cMestemp = strval;
	resToken= cMestemp.Tokenize(_T(","),curPos);

	CMainFrame* pFrame =  (CMainFrame*)(AfxGetApp()->GetMainWnd());	
	int nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	while (resToken != "")
	{
		nMeasureCnt++;
		if ((nvisionCnt) == (VISION_STAGE_1) && nMeasureCnt <= nCutCnt)
		{
			cMesVal.Append(resToken);
			cMesVal.Append(EXT_COLON);
		}		
		if ((nvisionCnt) == (VISION_STAGE_2) && (nMeasureCnt > nCutCnt && nMeasureCnt <= (nCutCnt * 2)))	
		{
			cMesVal.Append(resToken);
			cMesVal.Append(EXT_COLON);
		}

		if (curPos >= cMestemp.GetLength())	break;
		resToken= cMestemp.Tokenize(_T(","),curPos);
	}

	if (nMeasureCnt <= nCutCnt )
	{
		for (int i = nMeasureCnt ; i  <= (nCutCnt * 2) ; i ++ )
		{
			cMesVal.Append(_T("0"));
			cMesVal.Append(EXT_COLON);
		}
	}

	G_AddLog(3, 3, _T("m_fnGetMeasureData() - End"));
	return cMesVal;
}
//
//CString CDataManager::m_fnMakeQmsDataEdgeDistance(CString strLine)
//{
//	CString strRetVal;
//	CString strTemp , check_date, strVal;
//	int curPos, nDefCnt, nAlignToMarkCnt;
//	check_date = strLine ;//
//	CString strCnt ;
//	curPos = 0;
//
//	nAlignToMarkCnt = 1;
//	strTemp = _T("");
//
//	strVal = check_date.Tokenize(EXT_COLON, curPos);
//	strCnt.Format(_T("%d"),nAlignToMarkCnt);
//
//	strTemp.Append(_T("75"));
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(strCnt);
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(_T("K4RH"));
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(_T("100"));
//	strTemp.Append(EXT_SEMICOL);
//
//	nAlignToMarkCnt ++;
//
//	return	strRetVal = strTemp;
//}
//
//CString CDataManager::m_fnMakeQmsDataLuminance(CString strLine)
//{
//	CString strRetVal;
//	CString strTemp , check_date, strVal;
//	int curPos, nDefCnt, nLumitCnt;
//	CString strCnt ;
//	nLumitCnt = 1;
//
//	check_date = strLine ;//
//	curPos = 0;
//
//	strTemp = _T("");
//
//	strVal = check_date.Tokenize(EXT_COLON, curPos);
//
//	strCnt.Format(_T("%d"),nLumitCnt);
//	strTemp.Append(_T("XX"));
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(strCnt);
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(_T("K4RH"));
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(_T("100"));
//	strTemp.Append(EXT_COLON);
//	strTemp.Append(_T("120"));
//	strTemp.Append(EXT_SEMICOL);
//	nLumitCnt ++;
//
//	return	strRetVal = strTemp;
//}


int CDataManager::m_fnLastTokenCnt(CString strSplitVal, CString strSplit){

	CString check_date = strSplitVal;
	CString strVal = _T("");	
	int curPos = 0, nstrCnt =0 , nParse = 0;

	strVal  = check_date.Tokenize(strSplit, curPos);

	while (strVal != "")
	{	
		strVal = check_date.Tokenize(strSplit, curPos);
		if(strVal.IsEmpty())
			return nstrCnt;	

		/*if (_ttoi(strVal) == 0)
		{*/
		nParse = _ttoi(strVal);
		strVal.Format(_T("%d"),nParse) ;
		//}

		nstrCnt ++/*= strVal.GetLength()*/;

	};

	return nstrCnt;
}

CString CDataManager::m_fnLastToken(CString strSplitVal, CString strSplit){

	CString check_date = strSplitVal;
	CString strVal = _T(""), strRtnVal = _T("");
	int curPos = 0;

	strVal  = check_date.Tokenize(strSplit, curPos);

	while (strVal != "")
	{		
		strVal   = check_date.Tokenize(strSplit, curPos);
		if(strVal.IsEmpty())
			return strRtnVal;
		strRtnVal = strVal;
	};

	return strRtnVal;
}

void CDataManager::m_fnResultProcess_Demo(CString strPanelID, int nStageID)
{
	CString strRootPath, strPath, strPath1, strPath2, strPath3, strPath4, strTime;

	CTime time = CTime::GetCurrentTime();
	strTime = time.Format(_T("%Y%m%d_%H%M%S"));

	strRootPath.Format(_T("%s%s"), ROOT_PATH, RESULT_PATH);
	strPath.Format(_T("%s\\%s"), strRootPath, strPanelID);
	CreateDirectory(strPath, NULL);

	m_fnResetArray();

	strPath1.Format(_T("%s\\VisionClient-%d\\%s\\%s_V%d_F-1.txt"), strRootPath, nStageID + 1, strPanelID, strPanelID, nStageID + 1);
	strPath2.Format(_T("%s\\VisionClient-%d\\%s\\%s_V%d_F-2.txt"), strRootPath, nStageID + 1, strPanelID, strPanelID, nStageID + 1);
	strPath3.Format(_T("%s\\VisionClient-%d\\%s\\%s_V%d_B-1.txt"), strRootPath, nStageID + 1, strPanelID, strPanelID, nStageID + 1);
	strPath4.Format(_T("%s\\VisionClient-%d\\%s\\%s_V%d_B-2.txt"), strRootPath, nStageID + 1, strPanelID, strPanelID, nStageID + 1);

	strPath.Format(_T("%s\\%s_%s.txt"), strRootPath, strPanelID, strTime);

	//m_fnDefectFileMerge(strPath1, strPath2, strPath, nVisionCount);
	//m_fnDefectFileMerge(strPath, strPath3, strPath, nVisionCount);
	//m_fnDefectFileMerge(strPath, strPath4, strPath, nVisionCount);

	//m_fnFileToArray(strPathV1, VISION_PC_1 - 1);
	//m_fnFileToArray(strPathV2, VISION_PC_2 - 1);
	//m_fnFileToArray(strPathV3, VISION_PC_3 - 1);

	m_fnUpdateSummary();		// Summary 파일 생성 or 업데이트
	m_fnUpdateDefectList();		// Defect List 에 데이터 업데이트
}

void CDataManager::m_fnUpdateDefectList()
{
	G_AddLog(3, 3, _T("m_fnUpdateDefectList() - Start"));
	////////////////////////////////////////// 2015.01.12 ljk /////////////////////////////////////////////
	if(m_nSelectSummary > (int)m_vectorDefectFile.size())
	{
		return;
	}

	if(m_nSelectSummary < 0)
	{
		return;
	}

	TCHAR tszFindPath[MAX_PATH] = {0,};
	//int nlength = _stprintf_s(tszFindPath, MAX_PATH, _T("%s\\%s\\"), ROOT_PATH, RESULT_PATH);
	_stprintf_s(tszFindPath, MAX_PATH, _T("%s%s\\%s"), DATA_PATH, RESULT_PATH, m_vectorDefectFile[m_nSelectSummary - 1]);

	//STRU_DEFECT_INFO *pNewDetectInfo;

	char szLine[1024] = {0,};
	char *pszLine	  = NULL;
	char *pszToken	  = NULL;
	char *pszTemp	  = NULL;
	FILE *pFile		  = NULL;

	int curPos = 0;
	CString strVal = _T("");
	_tfopen_s(&pFile, tszFindPath, _T("rb"));
	if(pFile)
	{
		int nCnt = 0;
		while(1)
		{
			pszLine = fgets(szLine, 1024, pFile);
			if(pszLine == NULL)
			{
				break;
			}

			if(m_vectorDefectInfo.size())
			{
				m_vectorDefectInfo.back();
			}

			strtok_s(szLine   , ",", &pszTemp);
			if(!strncmp(pszTemp, "DEFECT", strlen("DEFECT")))
			{

				pNewDetectInfo = NULL;
				pNewDetectInfo = new STRU_DEFECT_INFO;

				strtok_s(NULL   , ",", &pszTemp);

				/// DEFECT INFO START //////////////////////////////////	
				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.layer_No = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Code = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Flag = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Pixel_X = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Pixel_Y = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Gate_Start_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Data_Start_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Gate_End_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Data_End_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_Start_X = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_Start_Y = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_End_X = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Coord_End_Y = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Size = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Img_No = (pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Img_Size_X = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Img_Size_Y = atof(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Type = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Camera_No = atoi(pszToken);

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;

				pszToken = strtok_s(NULL   , ",", &pszTemp);
				if(pszToken == NULL)break;
				pNewDetectInfo->Dinfo.Defect_Img_Path = _T("");
				pNewDetectInfo->Dinfo.Defect_Img_Path.Empty();
				pNewDetectInfo->Dinfo.Defect_Img_Path = (pszToken);

				m_vectorDefectInfo.push_back(pNewDetectInfo);
			}
			nCnt++;
		}
		fclose(pFile);
		pFile = NULL;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	G_AddLog(3, 3, _T("m_fnUpdateDefectList() - End"));
}

void CDataManager::m_fnUpdateSelDefectList(CString strPath)
{

	////////////////////////////////////////// 2015.01.12 ljk /////////////////////////////////////////////

	G_AddLog(3, 3, _T("m_fnUpdateSelDefectList() - Start"));
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if(m_nSelectSummary > (int)m_vectorDefectFile.size())
	{
		return;
	}

	if(m_nSelectSummary < 0)
	{
		return;
	}

	TCHAR tszFindPath[MAX_PATH] = {0,};
	//int nlength = _stprintf_s(tszFindPath, MAX_PATH, _T("%s\\%s\\"), ROOT_PATH, RESULT_PATH);
	_stprintf_s(tszFindPath, MAX_PATH, _T("%s"), strPath);

	//STRU_DEFECT_INFO *pNewDetectInfo = NULL;

	char szLine[1024] = {0,};
	char *pszLine	  = NULL;
	char *pszToken	  = NULL;
	char *pszTemp	  = NULL;
	FILE *pFile		  = NULL;

	//2016.02.01 Packing Defect
	CString strInspDate;
	CString strInspTime;
	CString strCellID;
	CString strDevice;
	CString strPad;
	//2016.02.01 Packing Defect

	int curPos = 0;
	CString strVal = _T("");

	try
	{

		_tfopen_s(&pFile, tszFindPath, _T("rb"));
		if(pFile)
		{
			int nCnt = 0;
			while(1)
			{
				pszLine = fgets(szLine, 1024, pFile);
				if(pszLine == NULL)
				{
					break;
				}

				if(m_vectorDefectInfo.size())
				{
					m_vectorDefectInfo.back();
				}


				strtok_s(szLine   , ",", &pszTemp);

				//2016.02.01 Packing Defect
				if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
				{
					if(!strncmp(pszTemp, "HEADERDATA", strlen("HEADERDATA")))
					{
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Insp Date
						if(pszToken == NULL)break;
						strInspDate = pszToken;

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Insp Time
						if(pszToken == NULL)break;
						strInspTime = pszToken;

						strInspDate.Append(_T(" ") + strInspTime);
					}
					if(!strncmp(pszTemp, "PANELDATA", strlen("PANELDATA")))
					{
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Cell ID
						if(pszToken == NULL)break;
						strCellID = pszToken;

						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Device
						if(pszToken == NULL)break;
						strDevice = pszToken;

						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Left
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("L"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Top
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("T"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Right
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("R"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Bottom
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("B"));
						else	strPad.Append(_T("0"));

					}
				}
				//2016.02.01 Packing Defect
				if(!strncmp(pszTemp, "DEFECT", strlen("DEFECT")))
				{
					pSelDetectInfo = NULL;
					pSelDetectInfo = new STRU_DEFECT_INFO;

					//strtok_s(szLine   , ",", &pszTemp);
					strtok_s(NULL   , ",", &pszTemp);
					/// DEFECT INFO START //////////////////////////////////	

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.layer_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Defect_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Defect_Code = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Flag = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Pixel_X = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Pixel_Y = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Gate_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Data_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Gate_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Data_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Coord_Start_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Coord_Start_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Coord_End_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Coord_End_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Defect_Size = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Defect_Img_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Img_Size_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Img_Size_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Defect_Type = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pSelDetectInfo->Dinfo.Camera_No = atoi(pszToken);


					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;

					//2016.02.01 Packing Defect
					if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
					{

						pSelDetectInfo->Dinfo.InspTime = (strInspDate);
						pSelDetectInfo->Dinfo.Cell_ID = (strCellID);
						pSelDetectInfo->Dinfo.Device = (strDevice);
						pSelDetectInfo->Dinfo.Pad = (strPad);
						pSelDetectInfo->Dinfo.Judge = _T("DEFECT");

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.P_Defect_Type = (pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Area = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Box_Area = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Box_X_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Box_X_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Box_Y_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Box_Y_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Center_X = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Center_Y = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Length = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Breadth = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Elongation = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Compactness = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Holecount = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Min_Angle = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Max_Angle = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Mean = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_X = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Y = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Feret_Elongation = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Pixel_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Pixel_Mean = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Pixel_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Pixel_Sigma = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pSelDetectInfo->Dinfo.Pixel_Sum = atof(pszToken);
					}

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;

					pSelDetectInfo->Dinfo.Defect_Img_Path = _T("");
					pSelDetectInfo->Dinfo.Defect_Img_Path.Empty();

					pSelDetectInfo->Dinfo.Defect_Img_Path = (pszToken);

					m_vectorDefectInfo.push_back(pSelDetectInfo);
				}
				nCnt++;
			}
			fclose(pFile);
			pFile = NULL;

		}
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnUpdateSelDefectList Failed !! - Memory Exception "), GetPanelId());
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnUpdateSelDefectList Failed !! - File Exception "), GetPanelId());
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("[Panel Id : %s] m_fnUpdateSelDefectList Failed !! - Exception "), GetPanelId());
		e->Delete();
	}
	G_AddLog(3, 3, _T("m_fnUpdateSelDefectList() - End"));
}
void CDataManager::m_fnClearSummary()
{
	TCHAR *pDelDefectFile;
	if(m_vectorDefectFile.size()>0)
	{
		while(m_vectorDefectFile.size())
		{
			m_vectorDefectFile.back();
			pDelDefectFile = m_vectorDefectFile[m_vectorDefectFile.size()-1];
			if(pDelDefectFile)
			{
				delete [] pDelDefectFile;
				pDelDefectFile = NULL;
			}
			m_vectorDefectFile.pop_back();
		}
	}
}

void CDataManager::m_fnLoadSummary()
{
	//CString strPath, strLine;

	//strPath.Format(_T("%s%s\\SummaryData.txt"), ROOT_PATH, RESULT_PATH);

	//m_strArrSummary.RemoveAll();

	//CStdioFile cstFile;

	//if (!cstFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeRead | CFile::typeText, NULL))
	//{
	//	G_AddLog(3, 2, _T("Summary File Open Error %s : %d"), strPath, GetLastError());
	//	return ;
	//}

	//while(cstFile.ReadString(strLine))
	//{
	//	if(strLine == "")	continue;

	//	m_strArrSummary.Add(strLine);
	//}	

	//cstFile.Close();

	////////////////////////////////////// 2015.01.12 ljk ////////////////////////////////////
	// 직접 파일을 읽어들이는 방식으로 변경	
	HANDLE hFindFile = NULL;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	TCHAR tszAllFindPath[MAX_PATH] = {0,};
	_stprintf_s(tszAllFindPath, MAX_PATH, _T("%s%s\\*.*"), DATA_PATH, RESULT_PATH);

	int nFileNameSize = 0;

	hFindFile = ::FindFirstFile(tszAllFindPath, &wfd);
	if(hFindFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	int nlength = 0;
	TCHAR *ptszNewDetectFile;

	while(bResult)
	{
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)	// 파일을 찾았다면
		{
			if(_tcslen(wfd.cFileName) == _tcslen(_T("PID0000_00000000_000000.txt")))
			{
				if(_tcsncmp(wfd.cFileName, _T("PID"), _tcslen(_T("PID"))) == 0)
				{
					if(_tcsncmp(wfd.cFileName+_tcslen(_T("PID0000_00000000_000000")), _T(".txt"), _tcslen(_T(".txt"))) == 0)
					{
						// 벡터에 누적시킨다.
						if(m_vectorDefectFile.size())
						{
							m_vectorDefectFile.back();
						}
						ptszNewDetectFile = NULL;
						ptszNewDetectFile = new TCHAR[MAX_PATH];
						memset(ptszNewDetectFile, 0, sizeof(TCHAR) * MAX_PATH);
						nlength = _stprintf_s(ptszNewDetectFile, MAX_PATH, _T("%s"), wfd.cFileName);
						ptszNewDetectFile[nlength] = _T('\0');
						m_vectorDefectFile.push_back(ptszNewDetectFile);
					}
				}
			}
		}
		bResult = ::FindNextFile(hFindFile, &wfd);
	}
	if(hFindFile)
	{
		::FindClose(hFindFile);
		hFindFile = NULL;
	}
	//////////////////////////////////////////////////////////////////////////////////////////
}

//void CDataManager::m_fnSelLoadSummary(CString strLoadPath, CString strFileName)
//{
//
//	HANDLE hFindFile = NULL;
//	WIN32_FIND_DATA wfd;
//	BOOL bResult = TRUE;
//
//	TCHAR tszAllFindPath[MAX_PATH] = {0,};
//	_stprintf_s(tszAllFindPath, MAX_PATH, _T("%s\\*.*"), strLoadPath);
//
//	int nFileNameSize = 0;
//
//	hFindFile = ::FindFirstFile(tszAllFindPath, &wfd);
//	if(hFindFile == INVALID_HANDLE_VALUE)
//	{
//		return;
//	}
//
//	int nlength = 0;
//	TCHAR *ptszNewDetectFile;
//
//	while(bResult)
//	{
//		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)	
//		{
//			if(!strFileName.Compare(wfd.cFileName))
//			{
//				if (strFileName.ReverseFind(_T('.txt')))
//				{
//					if(m_vectorDefectFile.size())
//					{
//						m_vectorDefectFile.back();
//					}
//					ptszNewDetectFile = NULL;
//					ptszNewDetectFile = new TCHAR[MAX_PATH];
//					memset(ptszNewDetectFile, 0, sizeof(TCHAR) * MAX_PATH);
//					nlength = _stprintf_s(ptszNewDetectFile, MAX_PATH, _T("%s"), wfd.cFileName);
//					ptszNewDetectFile[nlength] = _T('\0');
//					m_vectorDefectFile.push_back(ptszNewDetectFile);
//				}
//				
//			}
//		}
//		bResult = ::FindNextFile(hFindFile, &wfd);
//	}
//
//	if(hFindFile)
//	{
//		::FindClose(hFindFile);
//		hFindFile = NULL;
//	}
//	
//}

void CDataManager::m_fnSetSummary(int nIndex)
{
	m_nSelectSummary = nIndex;
}

void CDataManager::m_fnWriteSummary()
{
	//CString strPath, strLine;		// 2015.01.12 ljk 임시적으로 주석처리

	//strPath.Format(_T("%s%s\\SummaryData.txt"), ROOT_PATH, RESULT_PATH);

	//CStdioFile cstFile;

	//if (!cstFile.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText, NULL))
	//{
	//	G_AddLog(3, 2, _T("Summary File Open Error %s : %d"), strPath, GetLastError());
	//	return ;
	//}

	//for(int i = 0; i < m_strArrSummary.GetCount(); i++)
	//{
	//	strLine = m_strArrSummary.GetAt(i) + _T("\n");
	//	cstFile.WriteString(strLine);
	//}

	//cstFile.Close();
}

void CDataManager::m_fnUpdateSummary()
{
	//CString strPath, strData;		// 2015.01.12 ljk 임시적으로 주석처리

	//if(m_strArrMerge.GetCount() <= INDEX_PANELDATA)	return ;

	//strData = m_strArrMerge.GetAt(INDEX_PANELDATA);		// PANELDATA 정보를 가져온다.
	//strData.Replace(_T("DATA      ,PANELDATA ,"), _T(""));

	//m_strArrSummary.Add(strData);	// Summary Data Array 업데이트

	//m_fnWriteSummary();				// Summary 파일 업데이트
}

void CDataManager::m_fnBackupSummary()
{
	CString		strPath, strNewPath;

	CTime		fileTime;
	CFileFind	fileFind;
	DWORD		dwFileDayAmount, dwNowDayAmount;

	strPath.Format(_T("%s%s\\SummaryData.txt"), DATA_PATH, RESULT_PATH);

	if(fileFind.FindFile(strPath.GetBuffer(strPath.GetLength())) == 0)
	{
		G_AddLog(3, 2, _T("File Find Error : %d"), GetLastError());
		strPath.ReleaseBuffer();
		return ;
	}

	fileFind.FindNextFile();
	fileFind.GetLastWriteTime(fileTime);

	dwFileDayAmount	= m_fnGetTimeAmount(fileTime);
	dwNowDayAmount	= m_fnGetTimeAmoutNow();

	if(dwNowDayAmount >= (dwFileDayAmount + m_nDaySummaryBackup))
	{
		// 기존 파일을 백업한다.
		CTime time = CTime::GetCurrentTime();
		CString strTime = time.Format(_T("%Y%m%d_%H%M%S"));

		strNewPath.Format(_T("%s%s\\SummaryData_%s.txt"), DATA_PATH, RESULT_PATH, strTime);

		if(_wrename(strPath, strNewPath))
			G_AddLog(3, 2, _T("Summary File Backup Error : %d"), GetLastError());
	}
	strPath.ReleaseBuffer();
}

void CDataManager::m_fnClearDefectList()
{
	STRU_DEFECT_INFO *pDelDefectInfo;
	while(m_vectorDefectInfo.size())
	{
		m_vectorDefectInfo.back();
		pDelDefectInfo = m_vectorDefectInfo[m_vectorDefectInfo.size()-1];
		if(pDelDefectInfo)
		{
			delete pDelDefectInfo;
			pDelDefectInfo = NULL;
		}
		m_vectorDefectInfo.pop_back(); 
	}
}

void CDataManager::m_fnSetBackupDay(int nDay)
{
	m_nDaySummaryBackup = nDay;
}

DWORD CDataManager::m_fnGetTimeAmount(CTime& time)
{
	int		nYear, nMonth, nDay;
	DWORD	dwAmount=0, dwTemp=0;
	int		i;

	nYear	= time.GetYear();
	nMonth	= time.GetMonth();
	nDay	= time.GetDay();

	for(i=2001;i<nYear;i++)
	{
		if(m_fnIsYunYear(i))
			dwTemp+=366;
		else
			dwTemp+=365;
	}

	dwAmount+=dwTemp;
	dwTemp=0;

	for(i=1; i<nMonth; i++)
	{
		switch(i)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			dwTemp+=31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			dwTemp+=30;
			break;
		case 2:
			if(m_fnIsYunYear(nYear))
				dwTemp+=29;
			else
				dwTemp+=28;
			break;
		default:
			dwTemp+=30;
			break;
		}
	}

	dwAmount+=dwTemp;
	dwTemp=0;

	dwTemp=nDay;
	dwAmount+=dwTemp;

	return dwAmount;
}

DWORD CDataManager::m_fnGetTimeAmoutNow()
{
	CTime		time;
	time=CTime::GetCurrentTime();
	return m_fnGetTimeAmount(time);
}

BOOL CDataManager::m_fnIsYunYear(int nYear)
{
	BOOL	fTemp;

	if(nYear%4==0)
	{
		if(nYear%100==0)	
		{	
			if(nYear%400==0)	{	fTemp=TRUE;		}
			else				{	fTemp=FALSE;	}
		}
		else					{	fTemp=FALSE;	}
	}
	else						{	fTemp=FALSE;	}

	return fTemp;
}

UINT CDataManager::GetSummaryCount()
{
	//	return m_strArrSummary.GetCount();
	return m_vectorDefectFile.size();
}

//CString CDataManager::GetSummaryText(int nIndex)
TCHAR *CDataManager::GetSummaryText(int nIndex)
{
	//	return m_strArrSummary.GetAt(nIndex);
	return m_vectorDefectFile[nIndex];
}

UINT CDataManager::GetMergeCount()
{
	return m_strArrMerge.GetCount();
}

CString CDataManager::GetMergeText(int nIndex)
{
	return m_strArrMerge.GetAt(nIndex);
}

UINT CDataManager::GetGridDefectCount(int nGrid)		
{
	return m_strArrDefect[nGrid].GetCount();
}

CString CDataManager::GetGridDefectText(int nGrid, int nIndex)
{
	return m_strArrDefect[nGrid].GetAt(nIndex);
}

UINT CDataManager::GetDefectCount()
{
	//return m_nDefectCount;
	return m_vectorDefectInfo.size();
}

STRU_DEFECT_INFO *CDataManager::GetDefectInfo(int nIndex)
{
	//	return &m_stDefectInfo[nIndex];
	return m_vectorDefectInfo[nIndex];
}

int CDataManager::m_fnVectorListClear()
{
	int nRet = 0;
	m_vectorDefectInfo.clear();
	return nRet;
}

CString CDataManager::IntToCstr(int nVal)
{
	CString strItem;
	strItem.Format(_T("%d"),nVal);
	return strItem;
}



BOOL CDataManager::FileExist(LPCTSTR lpPathName, BOOL bMsg)

{
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	if ((hFind = ::FindFirstFile (lpPathName, &fd)) != INVALID_HANDLE_VALUE) {
		FindClose (hFind);
		return TRUE;

	}

	if(bMsg && lstrlen(lpPathName) > 0)
	{
		return FALSE;
	}   

	return FALSE;
}

CString CDataManager::m_fnGetLastWriteFile(CString strPanelID1, CString strPath)
{
	/////////////////////////////////////////////////////////////////////////


	CFileFind finder;
	BOOL bWorking;
	CString strFileName, strFilePath;
	CTime tTime;
	LONGLONG lDateCntTemp=0, lDateCnt = 0;

	//strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);
	//strPath1.Format(_T("%s\\%s"), strPath, strName);

	//strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	//strPath.Format(_T("%s%s\\"), strPath, strPanelID1);

	strPath.TrimLeft();
	strPath.TrimRight();
	bWorking = finder.FindFile(strPath +_T("*.*"));	// 모든화일

	while(bWorking == TRUE)
	{


		bWorking = finder.FindNextFile();	// 화일하나를 찾음

		if(finder.IsDots()) continue;	// "."이나  ".." 이면 다음으로 넘어감

		strFileName = finder.GetFileName();
		if ( strFileName.Find(strPanelID1) != -1 )
		{
			if (strFileName != _T(".") )
			{
				finder.GetCreationTime(tTime); 

				lDateCntTemp = tTime.GetYear()*10000000000 + tTime.GetMonth()*100000000 + tTime.GetDay()*1000000 + tTime.GetHour()*10000 + tTime.GetMinute()*100 + tTime.GetSecond(); 

				if (lDateCnt < lDateCntTemp)
				{
					lDateCnt = lDateCntTemp;
					strFilePath = strFileName;
				}		
			}
		}


	}    
	//////////////////////////////////////////////////////////////////////////
	if (strFilePath == _T(""))
	{
		strPath  = _T("");
	}
	finder.Close();
	return  strPath + strFilePath;
}

void CDataManager::m_fnSearchOverlapMeasure(CString strFilePath, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnSearchOverlapMeasure() - Start"));
	CString strLine, strItem, strPanelData;
	CStdioFile cstFile;	
	CString strMsg, strLineEdgeDistCF, strLineEdgeDistTFT, strLinePolDistCF, strLinePolDistTFT;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int nCurOverlapPanelCnt = 0;
	CString strTemp = _T("");
	int nEqpType = pFrame->m_fnEqpSizeType();

	nCurOverlapPanelCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CF_CURRENT_COUNT_PANEL, 0);
	if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nCurOverlapPanelCnt)
	{
		strTemp.Format(_T("%d"), 0);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CF_CURRENT_COUNT, strTemp);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CF_CURRENT_COUNT_PANEL, strTemp);
	}
	else
	{
		strTemp.Format(_T("%d"), nCurOverlapPanelCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CF_CURRENT_COUNT_PANEL, strTemp);
	}

	nCurOverlapPanelCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_TFT_CURRENT_COUNT_PANEL, 0);
	if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nCurOverlapPanelCnt)
	{
		strTemp.Format(_T("%d"), 0);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_TFT_CURRENT_COUNT, strTemp);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_TFT_CURRENT_COUNT_PANEL, strTemp);
	}
	else
	{
		strTemp.Format(_T("%d"), nCurOverlapPanelCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_TFT_CURRENT_COUNT_PANEL, strTemp);
	}

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnSearchOverlapMeasure File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF73"))
		{
			strItem = strLine;
			//strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, 8);
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, 8);
			strLineEdgeDistCF.Append(strItem);
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT73"))
		{
			strItem = strLine;
			//strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, 8);
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, 8);
			strLineEdgeDistTFT.Append(strItem);
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF74"))
		{
			strItem = strLine;
			//strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, 8);
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, 8);
			strLinePolDistCF.Append(strItem);		
		}	
		if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT74"))
		{
			strItem = strLine;
			//strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, 8);
			strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, 8);
			strLinePolDistTFT.Append(strItem);		
		}	

		if(strItem == _T("PANELDATA"))
		{			
			strPanelData = strLine;
		}

	}
	cstFile.Close();

	//2016.07.09
	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	{
		STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
		stAlignMeasureInfo.init_info();

		stAlignMeasureInfo = m_fnReadAlignMeasureData(strPanelID);

		strLineEdgeDistCF = stAlignMeasureInfo.cCFMeasureDistanceData;
		if (strLineEdgeDistCF == _T("") || strLineEdgeDistCF.IsEmpty())	strLineEdgeDistCF = _T("0,0,0,0,0,0,0,0,");

		strLineEdgeDistTFT = stAlignMeasureInfo.cTFTMeasureDistanceData;
		if (strLineEdgeDistTFT == _T("") || strLineEdgeDistTFT.IsEmpty())	strLineEdgeDistTFT = _T("0,0,0,0,0,0,0,0,");

		strLinePolDistCF = stAlignMeasureInfo.cCFMeasureAngleData;
		if (strLinePolDistCF == _T("")|| strLinePolDistCF.IsEmpty())	strLinePolDistCF = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,");

		strLinePolDistTFT = stAlignMeasureInfo.cTFTMeasureAngleData;
		if (strLinePolDistTFT == _T("")|| strLinePolDistCF.IsEmpty())	strLinePolDistTFT = _T("0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00");
	}
	//
	if (!strPanelData.IsEmpty())
	{
		if (m_fnGetItemData(7, strPanelData) == MEASUREFAIL_CF) 
		{
			if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_CF_MEASURE] == TRUE)
			{
				strMsg.Format(_T("%d.%s%s%s%s;"),CFMEASUREFAIL, strLineEdgeDistCF,  strLineEdgeDistTFT,strLinePolDistCF, strLinePolDistTFT);
				m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strMsg, strPanelID);
			}
		}
		if ( m_fnGetItemData(7, strPanelData) == MEASUREFAIL_TFT) 
		{
			if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_TFT_MEASURE] == TRUE)
			{
				strMsg.Format(_T("%d.%s%s%s%s;"),TFTMEASUREFAIL, strLineEdgeDistCF,strLineEdgeDistTFT,  strLinePolDistCF, strLinePolDistTFT);
				m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strMsg, strPanelID);
			}
		}
	}
	G_AddLog(3, 3, _T("m_fnSearchOverlapMeasure() - End"));
}
#if 1
void CDataManager::m_fnSearchOverlapCoord(CString strFilePath, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnSearchOverlapCoord START"));
	m_fnSearchOverlapMeasure(strFilePath, strPanelID);
	m_fnSearchOverlapPolMiss(strFilePath, strPanelID);	//2016.02.24
	m_fnSearchOverlapAlignMark(strFilePath, strPanelID);	//2016.08.22
	m_fnSearchOverlapLuminanceCode(strFilePath, strPanelID);
	UINT uiThreadID;	
	// MsgWaitForMultipleObjects
	pTreadProcData->m_strTheadData = strFilePath;
	pTreadProcData->m_hThreadData = (HANDLE)::_beginthreadex(NULL, 0, pTreadProcData->m_fnOverlapDefectData,(LPVOID) this, 0, &uiThreadID);
	G_AddLog(3, 3, _T("m_fnSearchOverlapCoord END"));
}
#else if
void CDataManager::m_fnSearchOverlapCoord(CString strFilePath)
{
	int nDefectCnt, nOverlapCnt, nImgCnt;
	CString strLine, strItem, strNewLineDefect, strBeforeDefectPath, strLineDefect, strAddDefect;
	CString strOverlapDefect, strLogOverlapDefect, strPanelData, strImgPath, strLineTemp;
	CStdioFile cstFile;
	CString strDefectType;		//2015.06.12 add by mby
	CString strMsg, strLineEdgeDistCF, strLineEdgeDistTFT, strLinePolDistCF, strLinePolDistTFT;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	strBeforeDefectPath.Format(_T("%s%s"), ROOT_PATH, DEFECT_DATA_PATH);
	nDefectCnt = m_fnGetSearchDefectCnt(strBeforeDefectPath, D_TYPE_1);

	if (nDefectCnt == 0)
	{
		if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Open Error %s : %d"), strFilePath, GetLastError());
			return;
		}

		strOverlapDefect = _T("");
		nOverlapCnt = 0;

		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				strLineDefect += m_fnCreateCoord(strLine);
				strOverlapDefect += m_fnGetZeroOverlapCoord(nOverlapCnt, strLine);
			}
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF73"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, nCutCnt);
				strLineEdgeDistCF.Append(strItem);
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT73"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, nCutCnt);
				strLineEdgeDistTFT.Append(strItem);
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF74"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, nCutCnt);
				strLinePolDistCF.Append(strItem);		
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT74"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, nCutCnt);
				strLinePolDistTFT.Append(strItem);		
			}	

			if(strItem == _T("PANELDATA"))
			{			
				strPanelData = strLine;
			}

		}
		cstFile.Close();

		if (!strPanelData.IsEmpty())
		{
			if (m_fnGetItemData(7, strPanelData) == MEASUREFAIL_CF) 
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_CF_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d:%s%s%s%s;"),CFMEASUREFAIL, strLineEdgeDistCF,  strLineEdgeDistTFT,strLinePolDistCF, strLinePolDistTFT);
					m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strMsg, strPanelID_1);
				}
			}
			if ( m_fnGetItemData(8, strPanelData) == MEASUREFAIL_TFT) 
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_TFT_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d:%s%s%s%s;"),TFTMEASUREFAIL, strLineEdgeDistCF,strLineEdgeDistTFT,  strLinePolDistCF, strLinePolDistTFT);
					m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE,strMsg, strPanelID_1);
				}
			}
		}
		if (!strOverlapDefect.IsEmpty())
		{
			//m_fnAlarmSet(strOverlapDefect, nOverlapCnt);		
		}

		if (!cstFile.Open(strBeforeDefectPath, CFile::modeWrite | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Open Error %s : %d")
				, strBeforeDefectPath, GetLastError());
			return;
		}
		cstFile.WriteString(strLineDefect);
		cstFile.Close();
	}
	else
	{
		m_fnGetOverlapDefectData(strBeforeDefectPath, nDefectCnt);

		strOverlapDefect = _T("");
		strLogOverlapDefect = _T("");
		nOverlapCnt = 0;
		nImgCnt = 0;
		if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Open Error %s : %d"), strFilePath, GetLastError());
			return;
		}
		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				/*strLineTemp = _T("");
				AfxExtractSubString(strLineTemp, strLine , IMG_PATH + 1, ',');
				m_stBeforeDefect[nImgCnt].strImage = _T("");
				m_stBeforeDefect[nImgCnt].strImage = strLineTemp;
				nImgCnt++;*/
				strNewLineDefect += m_fnCompareOverlapCoord(strLine, nDefectCnt);
				strOverlapDefect += m_fnGetZeroOverlapCoord(nOverlapCnt, strLine);

			}

			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF73"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, nCutCnt);
				strLineEdgeDistCF.Append(strItem);
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT73"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, nCutCnt);
				strLineEdgeDistTFT.Append(strItem);
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF74"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 2, nCutCnt);
				strLinePolDistCF.Append(strItem);		
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT74"))
			{
				strItem = strLine;
				strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), 1, nCutCnt);
				strLinePolDistTFT.Append(strItem);		
			}	

			if(strItem == _T("PANELDATA"))
			{			
				strPanelData = strLine;
			}

		}
		cstFile.Close();

		if (!strPanelData.IsEmpty())
		{
			if (m_fnGetItemData(7, strLine) == MEASUREFAIL_CF) 
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_CF_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.%s%s%s%s;"),CFMEASUREFAIL, strLineEdgeDistCF, strLineEdgeDistTFT,strLinePolDistCF,  strLinePolDistTFT);
					m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strMsg, strPanelID_1);
				}
			}
			if ( m_fnGetItemData(8, strLine) == MEASUREFAIL_TFT) 
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_TFT_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.%s%s%s%s;"),TFTMEASUREFAIL, strLineEdgeDistCF,  strLineEdgeDistTFT, strLinePolDistCF,  strLinePolDistTFT);
					m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strMsg, strPanelID_1);
				}

			}
		}
		if (!cstFile.Open(strBeforeDefectPath, CFile::modeWrite | CFile::modeCreate|CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Open Error %s : %d"), strBeforeDefectPath, GetLastError());
			return;
		}

		for(int i = 0; i < nDefectCnt; i++)
		{
			if(m_stBeforeDefect[i].nCount != 0 && m_stBeforeDefect[i].bUpdateFlag == TRUE
				&& pFrame->m_stOverlapCoordInfo->nDefectCount[m_stBeforeDefect[i].nDefectType] != 0)
			{
				if ( pFrame->m_stOverlapCoordInfo->nDefectCount[m_stBeforeDefect[i].nDefectType] <= m_stBeforeDefect[i].nCount )
				{
					strLogOverlapDefect.Format(_T("Start X : %0.3f, Start Y : %0.3f, End X : %0.3f, End Y : %0.3f")
						,m_stBeforeDefect[i].startX, m_stBeforeDefect[i].startY, m_stBeforeDefect[i].endX, m_stBeforeDefect[i].endY);

					strDefectType = m_fnGetOverlapDefectType(m_stBeforeDefect[i].nDefectType);

					strAddDefect.Format(_T("%0.3f,%0.3f,%0.3f,%0.3f,%s,%s;")
						,m_stBeforeDefect[i].startX, m_stBeforeDefect[i].startY, m_stBeforeDefect[i].endX, m_stBeforeDefect[i].endY, strDefectType, m_stBeforeDefect[i].strImage);//2015.06.12 add  nDefectType

					G_AddLog(2, 3, _T("Overlap Defect : %s"), strLogOverlapDefect, GetLastError());

					strOverlapDefect += strAddDefect;
					nOverlapCnt ++;
				}
				else
				{
					strAddDefect.Format(_T("%d,%0.3f,%0.3f,%0.3f,%0.3f,%d,%d,0\n"),m_stBeforeDefect[i].nFlag
						, m_stBeforeDefect[i].startX, m_stBeforeDefect[i].startY, m_stBeforeDefect[i].endX
						, m_stBeforeDefect[i].endY, m_stBeforeDefect[i].nDefectType, m_stBeforeDefect[i].nCount);
					cstFile.WriteString(strAddDefect);
				}
			}
		}
		cstFile.Close();

		if (!strOverlapDefect.IsEmpty())
		{
			//m_fnAlarmSet(strOverlapDefect, nOverlapCnt);			
		}

		if (!cstFile.Open(strBeforeDefectPath, CFile::modeWrite | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("File Open Error %s : %d")
				, strBeforeDefectPath, GetLastError());
			return;
		}
		cstFile.SeekToEnd();
		cstFile.WriteString(strNewLineDefect);
		cstFile.Close();
	}
}
#endif

CString CDataManager::m_fnCompareOverlapCoord(CString strLine, int nCnt)
{
	G_AddLog(3, 3, _T("m_fnCompareOverlapCoord() - Start"));
	CString strRetVal, strTemp , check_data, strVal, strNewLine, strMsg, strLineTemp;
	CString strFlag, strCoordStartX, strCoordStartY, strCoordEndX, strCoordEndY, strDefectType;
	DOUBLE dCoordStartX, dCoordStartY, dCoordEndX, dCoordEndY, dOffsetX, dOffsetY;
	int curPos, nDefCnt, nOverlapCnt, nDefectType, nFlag;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	dOffsetX = pFrame->m_stOverlapCoordInfo->dOffsetX;
	dOffsetY = pFrame->m_stOverlapCoordInfo->dOffsetY;
	check_data = strLine;
	curPos = 0;
	nDefCnt = 0;
	nOverlapCnt = 0;
	strNewLine = _T("");
	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	while (TRUE)
	{			

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;

		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case FLAG:
			strFlag = strVal;
			strTemp.Append(strFlag + EXT_COLON);
			nFlag = _tstoi((LPCTSTR)strVal);
			break;
		case COORD_START_X:
			strCoordStartX = strVal;
			strTemp.Append(strCoordStartX + EXT_COLON);
			dCoordStartX = _tstof((LPCTSTR)strVal);
			break;
		case COORD_END_X:
			strCoordEndX = strVal;
			strTemp.Append(strCoordEndX + EXT_COLON);
			dCoordEndX = _tstof((LPCTSTR)strVal);
			break;
		case COORD_START_Y:
			strCoordStartY = strVal;
			strTemp.Append(strCoordStartY + EXT_COLON);
			dCoordStartY = _tstof((LPCTSTR)strVal);
			break;
		case COORD_END_Y:
			strCoordEndY = strVal;
			strTemp.Append(strCoordEndY + EXT_COLON);
			dCoordEndY = _tstof((LPCTSTR)strVal);
			break;
		case DEFECT_TYPE:
			strDefectType = strVal;
			strTemp.Append(strDefectType + EXT_COLON);
			nDefectType = _tstoi((LPCTSTR)strVal);
			break;
		default:
			break;
		}

		if (nDefCnt == DEFECT_TYPE)
		{
			strTemp.Append(_T("0,"));
			strTemp.Append(_T("0"));
			strTemp.Append(_T("\n"));

			for(int i = 0; i < nCnt; i++)
			{
				if(m_stBeforeDefect[i].startX - dOffsetX <= dCoordStartX && dCoordStartX <= m_stBeforeDefect[i].startX + dOffsetX
					&& m_stBeforeDefect[i].startY - dOffsetY <= dCoordStartY && dCoordStartY <= m_stBeforeDefect[i].startY + dOffsetY
					&& m_stBeforeDefect[i].endX - dOffsetX <= dCoordEndX && dCoordEndX <= m_stBeforeDefect[i].endX + dOffsetX
					&& m_stBeforeDefect[i].endY - dOffsetY <= dCoordEndY && dCoordEndY <= m_stBeforeDefect[i].endY + dOffsetY
					&& m_stBeforeDefect[i].nDefectType == nDefectType && m_stBeforeDefect[i].nFlag == nFlag 
					&& pFrame->m_stOverlapCoordInfo->nDefectCheck[nDefectType] == TRUE)
				{
					m_stBeforeDefect[i].nCount = m_stBeforeDefect[i].nCount + 1;
					m_stBeforeDefect[i].bUpdateFlag = TRUE;

					AfxExtractSubString(strLineTemp, strLine , IMG_PATH + 1, ',');
					_tcscpy(m_stBeforeDefect[i].strImage, strLineTemp);

					nOverlapCnt++;
				}
			}

			if (nOverlapCnt == 0 && pFrame->m_stOverlapCoordInfo->nDefectCheck[nDefectType] == TRUE
				&& pFrame->m_stOverlapCoordInfo->nDefectCount[nDefectType] != 0)
			{
				if (nDefectType != INSP_CF_MEASURE && nDefectType != INSP_TFT_MEASURE)
				{
					strNewLine += strTemp;
				}
			}

			if (nDefectType == INSP_CF_MEASURE)
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_CF_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;"),CFMEASUREFAIL);
					//m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strMsg, strPanelID);
				}
			}
			if (nDefectType == INSP_TFT_MEASURE)
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_TFT_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;"),TFTMEASUREFAIL);
					//m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strMsg, strPanelID);
				}
			}

			break;	
		}

		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnCompareOverlapCoord() - End"));
	return	strRetVal = strNewLine;
}

CString CDataManager::m_fnCreateCoord(CString strLine)
{
	G_AddLog(3, 3, _T("m_fnCreateCoord() - Start"));
	CString strRetVal, strTemp, check_data, strVal, strAddLine, strMsg;
	CString cFlag, cCoordStartX, cCoordStartY, cCoordEndX, cCoordEndY, cDefectType;
	int curPos, nDefCnt, nDefectType;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	check_data = strLine ;
	curPos = 0;
	nDefCnt = 0;
	strAddLine = _T("");
	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	while (TRUE)
	{				

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case FLAG:
			cFlag = strVal;
			strTemp.Append(cFlag + EXT_COLON);
			break;
		case COORD_START_X:
			cCoordStartX = strVal;
			strTemp.Append(cCoordStartX + EXT_COLON);
			break;
		case COORD_END_X:
			cCoordEndX = strVal;
			strTemp.Append(cCoordEndX + EXT_COLON);
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			strTemp.Append(cCoordStartY + EXT_COLON);
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;
			strTemp.Append(cCoordEndY + EXT_COLON);
			break;
		case DEFECT_TYPE:
			cDefectType = strVal;
			nDefectType = _tstoi((LPCTSTR)cDefectType);
			strTemp.Append(cDefectType + EXT_COLON);
			break;
		default:
			break;
		}

		if (nDefCnt == DEFECT_TYPE)
		{
			strTemp.Append(_T("0,"));	//Overlap Count
			strTemp.Append(_T("0"));	//Update Flag
			strTemp.Append(_T("\n"));
			if (pFrame->m_stOverlapCoordInfo->nDefectCheck[nDefectType] == TRUE
				&& pFrame->m_stOverlapCoordInfo->nDefectCount[nDefectType] != 0)
			{
				if (nDefectType != INSP_CF_MEASURE && nDefectType != INSP_TFT_MEASURE)
				{
					strAddLine = strTemp;
				}
			}

			if (nDefectType == INSP_CF_MEASURE)
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_CF_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;"),CFMEASUREFAIL);
					//m_fnCountMeasureFail(KEY_OVERLAP_CF_CURRENT_COUNT, INSP_CF_MEASURE, strMsg, strPanelID_1);
				}
			}
			if (nDefectType == INSP_TFT_MEASURE)
			{
				if (pFrame->m_stOverlapCoordInfo->nDefectCheck[INSP_TFT_MEASURE] == TRUE)
				{
					strMsg.Format(_T("%d.0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0;"),TFTMEASUREFAIL);
					//m_fnCountMeasureFail(KEY_OVERLAP_TFT_CURRENT_COUNT, INSP_TFT_MEASURE, strMsg, strPanelID_1);
				}
			}

			break;	
		}

		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnCreateCoord() - End"));
	return	strRetVal = strAddLine;
}

CString CDataManager::m_fnGetZeroOverlapCoord(int &nOverlapcnt, CString strLine)
{
	G_AddLog(3, 3, _T("m_fnGetZeroOverlapCoord() - Start"));
	CString strRetVal, strTemp, check_data, strVal, strOverlapLine, strImgPath;
	CString cCoordStartX, cCoordStartY, cCoordEndX, cCoordEndY, cDefectType, cFlag;
	int curPos, nDefCnt, nDefectType, nFlag;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	check_data = strLine ;
	curPos = 0;
	nDefCnt = 0;
	strOverlapLine = _T("");

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	while (TRUE)
	{				

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;

		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case FLAG:
			cFlag = strVal;
			nFlag = _tstoi((LPCTSTR)cFlag);
		case COORD_START_X:
			cCoordStartX = strVal;
			strTemp.Append(cCoordStartX + EXT_COLON);
			break;
		case COORD_END_X:
			cCoordEndX = strVal;
			strTemp.Append(cCoordEndX + EXT_COLON);
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			strTemp.Append(cCoordStartY + EXT_COLON);
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;
			strTemp.Append(cCoordEndY + EXT_COLON);
			break;
		case DEFECT_TYPE:
			cDefectType = strVal;
			nDefectType = _tstoi((LPCTSTR)cDefectType);
			cDefectType = m_fnGetOverlapDefectType(nDefectType, nFlag);
			strTemp.Append(cDefectType + EXT_COLON);
			break;
		case IMG_PATH:
			strImgPath = strVal;
			break;

		default:
			break;
		}

		if (nDefCnt == IMG_PATH)
		{
			strTemp.Append(strImgPath);
			strTemp.Append(_T(";"));
			if (pFrame->m_stOverlapCoordInfo->nDefectCheck[nDefectType] == TRUE
				&& pFrame->m_stOverlapCoordInfo->nDefectCount[nDefectType] == 0)
			{
				if (nDefectType != INSP_CF_MEASURE && nDefectType != INSP_TFT_MEASURE)
				{
					strOverlapLine = strTemp;
					nOverlapcnt++;
				}
			}
			break;	
		}
		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnGetZeroOverlapCoord() - End"));
	return	strRetVal = strOverlapLine;
}

void CDataManager::m_fnAddDefectStruct(CString strLine, int nCnt)
{
	G_AddLog(3, 3, _T("m_fnAddDefectStruct() - Start"));
	CString check_data, strVal;
	CString strFlag, strCoordStartX, strCoordStartY, strCoordEndX, strCoordEndY, strDefectType, strCount, strUpdate;
	int curPos, nDefCnt;

	check_data = strLine ;
	curPos = 0;
	nDefCnt = 0;

	while (TRUE)
	{			

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;

		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case D_FLAG_CF_TFT_POL:
			strFlag = strVal;
			m_stBeforeDefect[nCnt].nFlag  = _tstoi((LPCTSTR)strFlag);
			break;
		case D_START_X:
			strCoordStartX = strVal;
			m_stBeforeDefect[nCnt].startX  = _tstof((LPCTSTR)strCoordStartX);
			break;
		case D_START_Y:
			strCoordStartY = strVal;
			m_stBeforeDefect[nCnt].startY  = _tstof((LPCTSTR)strCoordStartY);
			break;
		case D_END_X:
			strCoordEndX = strVal;
			m_stBeforeDefect[nCnt].endX  = _tstof((LPCTSTR)strCoordEndX);
			break;
		case D_END_Y:
			strCoordEndY = strVal;
			m_stBeforeDefect[nCnt].endY  = _tstof((LPCTSTR)strCoordEndY);
			break;
		case D_TYPE:
			strDefectType = strVal;
			m_stBeforeDefect[nCnt].nDefectType  = _tstoi((LPCTSTR)strDefectType);
			break;
		case D_COUNT:
			strCount = strVal;
			m_stBeforeDefect[nCnt].nCount  = _tstoi((LPCTSTR)strCount);
			break;
		case D_UPDATE:
			strCount = strVal;
			m_stBeforeDefect[nCnt].bUpdateFlag  = _tstoi((LPCTSTR)strCount);
			break;
			/*case IMG_PATH:
			strCount = strVal;
			m_stBeforeDefect[nCnt].strImage  = ((LPCTSTR)strCount);
			break;*/
		default:
			break;
		}

		if (nDefCnt == D_UPDATE)
		{
			break;	
		}

		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnAddDefectStruct() - End"));
}

CString CDataManager::m_fnGetOverlapDefectData(STRU_DEFECT_COORD_INFO* &stOverlapChkDef,CString strFilePath)
{
	G_AddLog(3, 3, _T("m_fnGetOverlapDefectData() - Start"));
	CString check_data, strVal, strLine, strItem, strPanelId, strMesure;
	CStdioFile cstFile;
	int nDefCnt = 0 ,nCnt = 0, curPos = 0, nDefTotalCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());


	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnGetOverlapDefectData File Open Error %s : %d"), strFilePath, GetLastError());
		return strPanelId;
	}
	int nVisionCnt = 0;

	

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();
		if (strItem == _T("PANELDATA"))
		{
			strPanelId = m_fnGetItemData(DEFECT_NO + 1, strLine);
		}
		if(strItem == _T("DEFECT"))
		{
			check_data = strLine ;
			curPos = 0;
			nDefCnt = 0;
			strVal = check_data.Tokenize(EXT_COLON, curPos);
			while (TRUE)
			{		
				if (nDefTotalCnt >= 999)	break;	//2015.10.16

				int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
				if(nDefCnt >= ntokenCnt) break;
				strVal = check_data.Tokenize(EXT_COLON, curPos);

				switch (nDefCnt)
				{
				case FLAG:
					stOverlapChkDef->stPnlInfo[nCnt].nFlag  = _tstoi((LPCTSTR)strVal);
					break;
				case COORD_START_X:
					stOverlapChkDef->stPnlInfo[nCnt].startX  = _tstof((LPCTSTR)strVal);
					break;
				case COORD_START_Y:					
					stOverlapChkDef->stPnlInfo[nCnt].startY  = _tstof((LPCTSTR)strVal);
					break;
				case COORD_END_X:					
					stOverlapChkDef->stPnlInfo[nCnt].endX  = _tstof((LPCTSTR)strVal);
					break;
				case COORD_END_Y:					
					stOverlapChkDef->stPnlInfo[nCnt].endY  = _tstof((LPCTSTR)strVal);
					break;
				case DEFECT_TYPE:				
					stOverlapChkDef->stPnlInfo[nCnt].nDefectType  = _tstoi((LPCTSTR)strVal);
					break;
					/*case D_COUNT:					
					stOverlapChkDef.stPnlInfo[nCnt].nCount  = _tstoi((LPCTSTR)strVal);
					break;
					case D_UPDATE:
					stOverlapChkDef.stPnlInfo[nCnt].bUpdateFlag = _tstoi((LPCTSTR)strVal);
					break;*/
					//case IMG_PATH - 1:
				case IMG_PATH :
					_tcscpy(stOverlapChkDef->stPnlInfo[nCnt].strImage , (LPCTSTR)strVal);
					break;
				default:
					break;

				}
				if (nDefCnt > IMG_PATH)		break;

				nDefCnt++;
			};

			//nDefCnt++;
			nCnt++;
			nDefTotalCnt++;
		}

	}
	cstFile.Close();	
	G_AddLog(3, 3, _T("m_fnGetOverlapDefectData() - End"));
	return strPanelId;
}


void CDataManager::m_fnCreateDefectStruct(CString strFilePath, int nDefectCnt)
{
	G_AddLog(3, 3, _T("m_fnCreateDefectStruct() - Start"));
	CString strLine;
	CStdioFile cstFile;
	int nDCnt;

	m_stBeforeDefect = new STRU_DEFECT_COORD_DATA[nDefectCnt];

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	nDCnt = 0;

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		m_fnAddDefectStruct(strLine, nDCnt);
		nDCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnCreateDefectStruct() - End"));
	cstFile.Close();

}

int CDataManager::m_fnGetSearchDefectCnt(CString strFilePath, int nFlag)
{
	G_AddLog(3, 3, _T("m_fnGetSearchDefectCnt() - Start"));
	CString strLine, strItem;
	int nDefectCnt;
	CStdioFile cstFile;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnGetSearchDefectCnt File Open Error %s : %d"), strFilePath, GetLastError());
		G_AddLog(3, 3, _T("m_fnGetSearchDefectCnt() - End"));
		return FALSE;
	}

	nDefectCnt = 0;

	if (nFlag == D_TYPE_1)
	{
		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;
			nDefectCnt++;
		}
	}
	else if (nFlag == D_TYPE_2)
	{
		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				nDefectCnt++;
			}
		}
	}

	cstFile.Close();
	G_AddLog(3, 3, _T("m_fnGetSearchDefectCnt() - End"));
	return nDefectCnt;
}


void CDataManager::m_fnMeasureAlarmSet(CString strCaption, CString strMag, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnMeasureAlarmSet() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	CString strSendMsg = _T("");
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strCaption);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strMag);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strPanelID);
	strSendMsg.Append(_T("#"));
	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);
		//m_fnAlarmDialog(m_pDlgMessage);//(CDlgMessage*)CWnd::FromHandle( m_fnAlarmDialog(m_pDlgMessage) );
		pView->SendMessage(WM_UI_NEW_MEASURE_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff);
		delete[] cBuffer;
	}
	G_AddLog(3, 3, _T("m_fnMeasureAlarmSet() - End"));
}

void CDataManager::m_fnContactFailAlarmSet(CString strCaption, STRU_CLASSIFY_INFO &stPanelData)
{
	G_AddLog(3, 3, _T("m_fnContactFailAlarmSet() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strMsg = _T(""), strOverlapLine = _T(""), strGrade = _T("");
	CString strSendMsg = _T("");
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);


	if(stPanelData.nPgGrade == ITO_NG )		 strGrade.Format(_T("%d"),ITORESISTILEGAL);
	if(stPanelData.nPgGrade == CONTACT_NG )  strGrade.Format(_T("%d"),CONTACTFAIL);
	CString strTemp;

	strTemp = stPanelData.cPgVal;
	if(strTemp.IsEmpty())
		_tcscpy(stPanelData.cPgVal , _T("0"));
	strOverlapLine.Format(_T("%d,%s,%s;"), stPanelData.nConFlag + 1, strGrade, stPanelData.cPgVal);

	strMsg.Format(_T("%s [%s]"),strCaption, stPanelData.cPanelID);

	strSendMsg.Append(strMsg);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strOverlapLine);
	strSendMsg.Append(_T("#"));

	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);		 		
		pView->SendMessage(WM_UI_NEW_CONTACT_MSG_DIALOG,len, (LPARAM)cTempBuff);
		delete[] cBuffer;
	}
	G_AddLog(3, 3, _T("m_fnContactFailAlarmSet() - End"));
}


// 2015.10.15 add by ikm Packing 용 자동 레시피 변경 S
void CDataManager::m_fnNgJudgelAlarm(CString strCaption, CString strMsg)
{
	long lRet = 0;

	BOOL bUseNgFlag = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	CString strSendMsg = _T("");

	strSendMsg.Append(strCaption);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strMsg);
	strSendMsg.Append(_T("#"));

	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);		

		pView->SendMessage(WM_UI_NEW_NG_JUDGE_MSG_DIALOG,len, (LPARAM)cTempBuff);		
		delete[] cBuffer;
	}

	bUseNgFlag = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0); //2015.10.29 add by ikm for ng flag - PACKING 

	pSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PLC_NG_JUDGE, bUseNgFlag);
	pSetup->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PC_NG_FLAG2, bUseNgFlag);	
	G_AddLog(3, 2, _T("[ALARM] NG JUDGE !! STOPPED MACHINE - PANEL ID : %s"), strMsg);
}

//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 S
void CDataManager::m_fnPolMarkIlegalAlarm(CString strCaption, CString strMsg)
{
	G_AddLog(3, 3, _T("m_fnPolMarkIlegalAlarm() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pSetup = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CString strSendMsg = _T("");

	strSendMsg.Append(strCaption);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strMsg);
	strSendMsg.Append(_T("#"));

	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);
		pView->SendMessage(WM_UI_NEW_POL_MARK_MSG_DIALOG,len, (LPARAM)cTempBuff);
		delete[] cBuffer;
	}

	G_AddLog(3, 2, _T("[ALARM] POL MARK ILEGAL : %s"), strMsg);
}
//2015.10.12 add by IKM - 폴 마크 불량 관련 알람 처리 기능 E


void CDataManager::m_fnDeleteTdiOverlapDefect(CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnDeleteTdiOverlapDefect() - Start"));
	int nDefectCnt, nDCnt, nImageCnt;
	CString strDefectPath[MAX_VISION_PC];
	CString strLine, strItem, strRootPath, strPath, strNewLine;
	CString strTmp, strCntImgeName, strCmpFileName, strFileName, strNewImageName, strOldImageName;	//Image
	CStringArray strArrDefect;
	CStringArray strArrData;
	CStdioFile cstFile;

	strNewLine = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	for (int i = 0; i < pFrame->m_stParamInfo.nCountVisionClient; i ++ )
	{
		strDefectPath[i].Format(_T("%s%s\\%s_V%d.txt"), strRootPath, strPanelID, strPanelID, i + 1);
	}

	nDefectCnt = m_fnGetSearchDefectCnt(strDefectPath[VS_2], D_TYPE_2);
	m_stTdiDefect = new STRU_DEFECT_COORD_DATA[nDefectCnt];

	if (!cstFile.Open(strDefectPath[VS_2], CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnDeleteTdiOverlapDefect1 File Open Error %s : %d"), strDefectPath[VS_2], GetLastError());
		return;
	}
	try
	{	
		nDCnt = 0;
		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				m_fnAddTdiDefectStruct(strLine, nDCnt);
				nDCnt ++;
			}

		}
		cstFile.Close();

		m_fnCompareDefectCoord(strDefectPath[VS_1], nDefectCnt);
		m_fnCompareDefectCoord(strDefectPath[VS_3], nDefectCnt);

		if (!cstFile.Open(strDefectPath[VS_2], CFile::modeRead | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("m_fnDeleteTdiOverlapDefect2 File Open Error %s : %d"), strDefectPath[VS_2], GetLastError());
			return;
		}

		nDCnt = 0;
		nImageCnt = 0;
		strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, VISION_PC_2);
		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				if (m_stTdiDefect[nDCnt].bUpdateFlag == TRUE)
				{
					DeleteFile(strPath + m_stTdiDefect[nDCnt].strImage);
				}
				else
				{
					nImageCnt++;

					strFileName = m_stTdiDefect[nDCnt].strImage;
					strCmpFileName = m_fnLastToken(strFileName, _T("_"));
					strCntImgeName = strCmpFileName;
					strTmp = m_fnLastToken(strCntImgeName, _T("."));
					strTmp = strCntImgeName.Left(strCntImgeName.GetLength() - strTmp.GetLength() - 1);
					strCmpFileName = strFileName.Left(strFileName.GetLength() - strCmpFileName.GetLength());
					strCntImgeName.Format(_T("%s%04d.jpg"),strCmpFileName, nImageCnt) ;

					strNewImageName.Format(_T("%sVisionClient-%d\\%s"),strRootPath, VISION_PC_2,strCntImgeName);
					strOldImageName.Format(_T("%sVisionClient-%d\\%s"),strRootPath, VISION_PC_2,strFileName);
					//CopyFile(strOldImageName, strNewImageName, 0);
					CFile::Rename(strOldImageName , strNewImageName);
					//DeleteFile(strOldImageName);
					strArrDefect.Add(strLine);
				}
				nDCnt ++;
			}
			else
			{
				strArrData.Add(strLine);
			}

		}
		cstFile.Close();

		delete[] m_stTdiDefect;

		if (!cstFile.Open(strDefectPath[VS_2], CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
		{
			G_AddLog(3, 2, _T("m_fnDeleteTdiOverlapDefect3 File Open Error %s : %d"), strDefectPath[VS_2], GetLastError());
			return;
		}

		for(int i = 0; i < strArrData.GetSize(); i++)
		{
			strLine = strArrData.GetAt(i) + _T("\n");
			strNewLine = strLine;
			cstFile.WriteString(strNewLine);
		}
		for(int i = 0; i < strArrDefect.GetSize(); i++)
		{
			strLine = strArrDefect.GetAt(i) + _T("\n");
			strNewLine = strLine;
			cstFile.WriteString(strNewLine);
		}
		cstFile.Close();
	}
	catch (CFileException* e)
	{
		cstFile.Close();
		TCHAR szError[1024];
		e->GetErrorMessage(szError, 1024 );
		G_AddLog(3, 2, _T("m_fnDeleteTdiOverlapDefect File Open Error %s : %d"), szError, GetLastError());

		e->Delete();

	}
	catch (CException* e)
	{
		G_AddLog(3, 2, _T("m_fnDeleteTdiOverlapDefect CException Error %s : %d"), strDefectPath[VS_2], GetLastError());

		e->Delete();

	}
	G_AddLog(3, 3, _T("m_fnDeleteTdiOverlapDefect() - End"));
}

//2016.01.29 Mr.Chu
//기능 : 원하는 메시지를 Window 로 출력한다
void CDataManager::m_fnDisplayMSGSet(CString strMsgA, CString strMsgB, int nAutoRecipeFlag/*=1*/, int nSuccessFlag/*=1*/)
{
	G_AddLog(3, 3, _T("m_fnDisplayMSGSet() - Start"));
	//기존에 사용하던 DISPLAY_MSG 다이얼로그는 같은 UI로 사용되며, Auto Recipe 작동시 해당 UI로 변경하는 Flag 추가.
	//Auto Recipe 다이얼로그로 사용중일때 기존 다이얼로그 화면 생성 불가.
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CString strSendMsg = _T("");

	strSendMsg.Append(strMsgA);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strMsgB);

	//strSendMsg.Append(_T("#"));

	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);

		if(!nAutoRecipeFlag)
		{
			if ((CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto) !=NULL)
				pView->SendMessage(WM_UI_NEW_DISPLAY_MSG_DIALOG, nSuccessFlag, (LPARAM)cTempBuff);
		}
		else
		{
			if ((CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto) !=NULL)
				pView->SendMessage(WM_UI_NEW_DISPLAY_MSG_DIALOG, len, (LPARAM)cTempBuff);
		}
		delete[] cBuffer;
	}
	G_AddLog(3, 3, _T("m_fnDisplayMSGSet() - End"));
}

void CDataManager::m_fnDistroyMSGDialog()
{
	G_AddLog(3, 3, _T("m_fnDistroyMSGDialog() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	if(pView)
		pView->SendMessage(WM_UI_DISTROY_DIALOG,NULL,NULL);
	G_AddLog(3, 3, _T("m_fnDistroyMSGDialog() - End"));
}

void CDataManager::m_fnCompareDefectCoord(CString strPath, int nCnt)
{
	G_AddLog(3, 3, _T("m_fnCompareDefectCoord() - Start"));
	int nDefectCnt;
	CString strLine, strItem, strFilePath;
	CStdioFile cstFile;

	strFilePath = strPath;
	nDefectCnt = nCnt;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnCompareDefectCoord File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}
	try
	{

		while(cstFile.ReadString(strLine))
		{
			if(strLine == "")	continue;

			strItem = strLine;
			strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
			strItem = strItem.Left(strItem.Find(EXT_COLON));
			strItem.Trim();
			if(strItem == _T("DEFECT"))
			{
				m_fnCompareTdiCoord(strLine, nDefectCnt);
			}

		}
		cstFile.Close();

	}

	catch (CFileException* e)
	{
		cstFile.Close();
		G_AddLog(3, 2, _T("m_fnCompareDefectCoord File Open Error %s : %d"), strFilePath, GetLastError());
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(3, 2, _T("m_fnCompareDefectCoord CException Error %s : %d"), strFilePath, GetLastError());
		e->Delete();
	}
	G_AddLog(3, 3, _T("m_fnCompareDefectCoord() - End"));
}

void CDataManager::m_fnCompareTdiCoord(CString strLine, int nCnt)
{
	G_AddLog(3, 3, _T("m_fnCompareTdiCoord() - Start"));
	CString check_data, strVal;
	CString strFlag, strCoordStartX, strCoordStartY, strCoordEndX, strCoordEndY, strDefectType;
	DOUBLE dCoordStartX, dCoordStartY, dCoordEndX, dCoordEndY, dOffsetX, dOffsetY;
	int curPos, nDefCnt, nDefectType, nFlag;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	dOffsetX = pFrame->m_stTdiOverlapCoordInfo->dOffsetX;
	dOffsetY = pFrame->m_stTdiOverlapCoordInfo->dOffsetY;
	check_data = strLine;

	curPos = 0;
	nDefCnt = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	while (TRUE)
	{			

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case FLAG:
			strFlag = strVal;
			nFlag = _tstoi((LPCTSTR)strVal);
			break;
		case COORD_START_X:
			strCoordStartX = strVal;
			dCoordStartX = _tstof((LPCTSTR)strVal);
			break;
		case COORD_END_X:
			strCoordEndX = strVal;
			dCoordEndX = _tstof((LPCTSTR)strVal);
			break;
		case COORD_START_Y:
			strCoordStartY = strVal;
			dCoordStartY = _tstof((LPCTSTR)strVal);
			break;
		case COORD_END_Y:
			strCoordEndY = strVal;
			dCoordEndY = _tstof((LPCTSTR)strVal);
			break;
		case DEFECT_TYPE:
			strDefectType = strVal;
			nDefectType = _tstoi((LPCTSTR)strVal);
			break;
		default:
			break;
		}

		if (nDefCnt == DEFECT_TYPE)
		{
			for(int i = 0; i < nCnt; i++)
			{
				if(m_stTdiDefect[i].startX - dOffsetX <= dCoordStartX && dCoordStartX <= m_stTdiDefect[i].startX + dOffsetX
					&& m_stTdiDefect[i].startY - dOffsetY <= dCoordStartY && dCoordStartY <= m_stTdiDefect[i].startY + dOffsetY
					&& m_stTdiDefect[i].endX - dOffsetX <= dCoordEndX && dCoordEndX <= m_stTdiDefect[i].endX + dOffsetX
					&& m_stTdiDefect[i].endY - dOffsetY <= dCoordEndY && dCoordEndY <= m_stTdiDefect[i].endY + dOffsetY)
				{
					m_stTdiDefect[i].bUpdateFlag = TRUE;
				}
			}
			break;	
		}

		nDefCnt ++;
	}
	//
	G_AddLog(3, 3, _T("m_fnCompareTdiCoord() - End"));
}

void CDataManager::m_fnAddTdiDefectStruct(CString strLine, int nCnt)
{
	G_AddLog(3, 3, _T("m_fnAddTdiDefectStruct() - Start"));
	CString check_data, strVal;
	CString strFlag, strCoordStartX, strCoordStartY, strCoordEndX, strCoordEndY, strImage, strDefectType, strImgPath;
	int curPos, nDefCnt;

	check_data = strLine ;
	curPos = 0;
	nDefCnt = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	while (TRUE)
	{		
		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		switch (nDefCnt)
		{
		case FLAG:
			strFlag = strVal;
			m_stTdiDefect[nCnt].nFlag  = _tstoi((LPCTSTR)strFlag);
			break;
		case COORD_START_X:
			strCoordStartX = strVal;
			m_stTdiDefect[nCnt].startX  = _tstof((LPCTSTR)strCoordStartX);
			break;
		case COORD_START_Y:
			strCoordStartY = strVal;
			m_stTdiDefect[nCnt].startY  = _tstof((LPCTSTR)strCoordStartY);
			break;
		case COORD_END_X:
			strCoordEndX = strVal;
			m_stTdiDefect[nCnt].endX  = _tstof((LPCTSTR)strCoordEndX);
			break;
		case COORD_END_Y:
			strCoordEndY = strVal;
			m_stTdiDefect[nCnt].endY  = _tstof((LPCTSTR)strCoordEndY);
			break;
		case DEFECT_IMG_NO:
			strImage = strVal;
			_tcscpy(m_stTdiDefect[nCnt].strImage, strImage);

			break;
		case DEFECT_TYPE:
			strDefectType = strVal;
			m_stTdiDefect[nCnt].nDefectType  = _tstoi((LPCTSTR)strDefectType);
			break;
		default:
			break;
		}

		if (nDefCnt == DEFECT_TYPE)
		{
			break;	
		}

		nDefCnt ++;				
	};
	G_AddLog(3, 3, _T("m_fnAddTdiDefectStruct() - Start"));
}

CString CDataManager::m_fnGetOverlapDefectType(int nType, int nFlag)
{
	G_AddLog(3, 3, _T("m_fnGetOverlapDefectType() - Start"));
	int nDefType;
	CString strDefType;

	nDefType = nType;
	strDefType = _T("");

	switch(nDefType)
	{
	case INSP_DUST				  :    strDefType = _T("DUST");										break;
	case INSP_BLACK_SCRATCH		  :	   strDefType = _T("BLACK_SCRATCH");							break;
	case INSP_ITO				  :	   strDefType = _T("ITO");										break;
	case INSP_DIMPLE_POINT		  :	   if	   (nFlag ==  TFT )	strDefType = _T("TFT DIMPLE POINT");								
									   else if (nFlag ==  CF  )	strDefType = _T("CF DIMPLE POINT"); break;
	case INSP_DIMPLE_LINE		  :	   if	   (nFlag ==  TFT )	strDefType = _T("TFT DIMPLE LINE");	
									   else if (nFlag ==  CF  )	strDefType = _T("CF DIMPLE LINE");  break;
	case INSP_ETCHING_LINE		  :	   strDefType = _T("ETCHING_LINE");								break;
	case INSP_WHITE_TURBIDITY	  :	   strDefType = _T("WHITE_TURBIDITY");							break;
	case INSP_PAD_BURNT			  :	   strDefType = _T("PAD_BURNT");								break;
	case INSP_EDGE_BROKEN		  :	   strDefType = _T("EDGE_BROKEN");								break;
	case INSP_PAD_SCRATCH		  :	   strDefType = _T("PAD_SCRATCH");								break;
	case INSP_CF_MEASURE		  :	   strDefType = _T("CF_MEASURE");								break;
	case INSP_TFT_MEASURE		  :	   strDefType = _T("TFT_MEASURE");								break;
	case INSP_BURR				  :	   strDefType = _T("BURR");										break;
	case INSP_POL_BUBBLE		  :	   if	   (nFlag ==  TFT )	strDefType = _T("TFT POL BUBBLE");  
									   else if (nFlag ==  CF  )	strDefType = _T("CF POL BUBBLE");
									   else						strDefType = _T("POL BUBBLE");		break;							   
	case INSP_POL_SCRATCH		  :	   strDefType = _T("POL_SCRATCH");								break;	
	case INSP_MURA				  :	   strDefType = _T("MURA");										break;
	case INSP_POL_ALIEN_SUBSTANCE :	   if(nFlag  ==  TFT)  strDefType = _T("TFT_POL_SUBSTANCE");	
									   else if(nFlag == CF)strDefType = _T("CF_POL_SUBSTANCE");		break;
	case INSP_POL_MINOR			  :	   strDefType = _T("POL_MINOR");								break;
	case INSP_CF_BURR			  :	   strDefType = _T("CF_BURR");									break;
	case INSP_TFT_BURR			  :	   strDefType = _T("TFT_BURR");									break;
	case INSP_CF_BROKEN			  :	   strDefType = _T("CF_BROKEN");								break;
	case INSP_TFT_BROKEN		  :	   strDefType = _T("TFT_BROKEN");								break;
	case INSP_CF_CORNER_BROKEN	  :	   strDefType = _T("CF_CORNER_BROKEN");							break;
	case INSP_TFT_CORNER_BROKEN	  :	   strDefType = _T("TFT_CORNER_BROKEN");						break;
	case INSP_CF_CELL_BROKEN	  :	   strDefType = _T("CF_CELL_BROKEN");							break;
	case INSP_TFT_CELL_BROKEN	  :	   strDefType = _T("TFT_CELL_BROKEN");							break;
	case INSP_CF_PROTECTION		  :	   strDefType = _T("CF_PROTECTION");							break;		//2017.04.19
	case INSP_TFT_PROTECTION	  :	   strDefType = _T("TFT_PROTECTION");							break;		//2017.04.19

	case INSP_B_GRADE_CHECK		  :	   strDefType = _T("B_GRADE_CHECK");							break;		//2019.03.15
	case INSP_C_GRADE_CHECK		 :	   strDefType = _T("C_GRADE_CHECK");							break;		//2019.03.15

	case INSP_BUBBLB_STATION1	  :	   strDefType = _T("BUBBLB_STATION1");							break;	//2020.02.28 add by mby
	case INSP_BUBBLB_STATION2	  :	   strDefType = _T("BUBBLB_STATION2");							break;	
	case INSP_BUBBLB_STATION3	  :	   strDefType = _T("BUBBLB_STATION3");							break;	
	case INSP_BUBBLB_STATION4	  :	   strDefType = _T("BUBBLB_STATION4");							break;	
	case INSP_BUBBLB_STATION5	  :	   strDefType = _T("BUBBLB_STATION5");							break;	
	case INSP_BUBBLB_STATION6	  :	   strDefType = _T("BUBBLB_STATION6");							break;	
	case INSP_BUBBLB_STATION7	  :	   strDefType = _T("BUBBLB_STATION7");							break;	
	case INSP_BUBBLB_STATION8	  :	   strDefType = _T("BUBBLB_STATION8");							break;	
	case INSP_BUBBLB_STATION9	  :	   strDefType = _T("BUBBLB_STATION9");							break;	
	case INSP_BUBBLB_STATION10	  :	   strDefType = _T("BUBBLB_STATION10");							break;	
	case INSP_BUBBLB_STATION11	  :	   strDefType = _T("BUBBLB_STATION11");							break;	
	case INSP_BUBBLB_STATION12	  :	   strDefType = _T("BUBBLB_STATION12");							break;	//2020.02.28 add by mby

	case INSP_CONTACT				 :	   strDefType = _T("CONTACT_FAIL");								break;
	case INSP_PIMPLE_POINT		 :	   if	   (nFlag ==  TFT )	strDefType = _T("TFT PIMPLE POINT");								
									   else if (nFlag ==  CF  )	strDefType = _T("CF PIMPLE POINT");			break;
	case INSP_PIMPLE_LINE		     :	   if	   (nFlag ==  TFT )	strDefType = _T("TFT PIMPLE LINE");	
										   else if (nFlag ==  CF  )	strDefType = _T("CF PIMPLE LINE");			break;
	case INSP_CF_PIMPLE_CIRCLE	  :	   strDefType = _T("CF PIMPLE CIRCLE");									break;
	case INSP_CF_WAVE_PATTERN	  :	   strDefType = _T("CF WAVE PATTERN");							break;
	case INSP_ETC_LINE			  :	   strDefType = _T("ETC LINE");									break;
	case INSP_ETC_POINT			  :	   strDefType = _T("ETC POINT");								break;
	case INSP_POL_MISS			  :	   strDefType = _T("POL MISS");									break;	//2016.02.24
	}

	G_AddLog(3, 3, _T("m_fnGetOverlapDefectType() - End"));
	return strDefType;

}

CString CDataManager::m_fnGetDefectCode(CString strLine)
{
	G_AddLog(3, 3, _T("m_fnGetDefectCode() - Start"));
	CString strDefType, strDefCode, strDefFlag;

	strDefType = m_fnGetItemData(DEFECT_TYPE + 1, strLine);
	strDefFlag = m_fnGetItemData(FLAG + 1, strLine);
	switch (_ttoi(strDefType))
	{
	case 0:
		break;
	case 1:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTWHITEBLUR);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFWHITEBLUR);
		}
		break;
	case 2:
		strDefCode.Format(_T("%s%d"),_T("K"), ITOPEEL);
		break;
	case 3:
		strDefCode.Format(_T("%s%d"),_T("K"), WANESBROKEN);
		break;
	case 4:
		strDefCode.Format(_T("%s%d"),_T("K"), SOCKETSCRACH);		
		break;
	case 5:	
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTDIMPLEPOINT);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFDIMPLEPOINT);
		}
		break;
	case 6:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTEDGEILEGAL);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFEDGEILEGAL);
		}
		break;
	case 7:
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTDIMPLELINE);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFDIMPLELINE);
		}
		break;
	case 8:
		strDefCode.Format(_T("%s%d"),_T("K"), SOCKETBURN);
		break;
	case 9:
		strDefCode.Format(_T("%s%d"),_T("K"), BURR);
		break;
	case 10:
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLBUBBLE);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLBUBBLE);
		}
		break;
	case 11:		
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLSCRATCH);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLSCRATCH);
		}
		break;
	case 12:
		if(_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTPOLSUBSTANCE);
		}
		if(_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFPOLSUBSTANCE);
		}		
		break;
	case 14:
		if (_ttoi(strDefFlag) ==  TFT )
		{
			strDefCode.Format(_T("%s%d"),_T("K"), TFTSCRATCH);
		}
		if (_ttoi(strDefFlag) ==  CF)
		{
			strDefCode.Format(_T("%s%d"),_T("K"), CFSCRATCH);
		}
		break;
	case 18:
		strDefCode.Format(_T("%s%d"),_T("K"), MURA);		
		break;
	}
	G_AddLog(3, 3, _T("m_fnGetDefectCode() - End"));
	return strDefCode;
}

void CDataManager::m_fnCountMeasureFail(CString strKey, int nType, CString strMsg, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnCountMeasureFail() - Start"));
	CString strTemp;
	int nCurOverlapCnt;
	int nCurOverlapPanelCnt;

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	nCurOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, strKey, 0);
	nCurOverlapPanelCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, strKey + _T("_PANEL"), 0);
	if (pFrame->m_stOverlapCoordInfo->nDefectCount[nType] <= nCurOverlapCnt + 1 &&
		pFrame->m_stOverlapCoordInfo->nPanelCnt >= nCurOverlapPanelCnt)
	{
		//20180115
		//if (nType == INSP_CF_MEASURE)
		//	m_fnMeasureAlarmSet(_T("CF Measure Fail"), strMsg, strPanelID);
		//else
		//	m_fnMeasureAlarmSet(_T("TFT Measure Fail"), strMsg, strPanelID);

		strTemp.Format(_T("%d"), 0);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strKey, strTemp);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strKey + _T("_PANEL"), strTemp);
	}
	else
	{
		strTemp.Format(_T("%d"), nCurOverlapCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strKey, strTemp);
	}
	G_AddLog(3, 3, _T("m_fnCountMeasureFail() - End"));
}

//2015.06.22
void CDataManager::m_fnDeleteDefectFile(CString strFilePath, int nDayPeriod)
{
	//G_AddLog(3, 3, _T("m_fnDeleteDefectFile() - Start"));
	CString		strFindFolderName, strFindFileName, strFolderName, strExtension;
	CString		strDate, strYear, strMonth, strDay;
	CTime			fileTime;
	CFileFind	fileFind;
	BOOL			bWorking;
	DWORD		dwFileDayAmount, dwNowDayAmount;

	bWorking = fileFind.FindFile(strFilePath + _T("\\*.*"));

	while(bWorking)
	{
		bWorking = fileFind.FindNextFile();

		if(fileFind.IsDirectory() && !fileFind.IsDots())
		{
			strFindFolderName = fileFind.GetFilePath();
			m_fnDeleteDefectFile(strFindFolderName, nDayPeriod);
		}

		if(!fileFind.IsDots())
		{
			if(fileFind.IsDirectory()) continue;

			strFindFileName = fileFind.GetFilePath();
			strExtension = strFindFileName.Right(4);

			if(!strExtension.Compare(_T(".txt")))
			{
				fileFind.GetLastWriteTime(fileTime);
				dwFileDayAmount	= m_fnGetTimeAmount(fileTime);
				dwNowDayAmount	= m_fnGetTimeAmoutNow();

				if(dwNowDayAmount > (dwFileDayAmount + nDayPeriod))
				{
					strFolderName = fileFind.GetRoot();
					m_fnDeleteFolder(strFolderName);
				}

			}
		}
	}
	//G_AddLog(3, 3, _T("m_fnDeleteDefectFile() - End"));
}

BOOL CDataManager::m_fnDeleteFolder(const CString &strFolder)
{
	SHFILEOPSTRUCT FileOp = {0};
	TCHAR szTemp[2048 * 2];

	wcscpy_s(szTemp, strFolder.GetLength() + 1, strFolder);
	szTemp[strFolder.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.

	FileOp.hwnd						= NULL;
	FileOp.wFunc					= FO_DELETE;
	FileOp.pFrom					= (LPCWSTR)szTemp;
	FileOp.pTo						= NULL;
	FileOp.fFlags					= FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT ;	// 확인메시지가 안 뜨도록 설정
	FileOp.fAnyOperationsAborted	= false;
	FileOp.hNameMappings			= NULL;
	FileOp.lpszProgressTitle		= NULL;

	int n = SHFileOperation(&FileOp);

	return TRUE;
}

BOOL CDataManager::m_fnMoveFolder(CString strFrom, CString strTo)
{
	SHFILEOPSTRUCT FileOp = {0};
	TCHAR szTempFrom[2048 * 2];
	TCHAR szTempTo  [2048 * 2];
	wcscpy_s(szTempFrom, strFrom.GetLength() + 1, strFrom);
	szTempFrom[strFrom.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.
	wcscpy_s(szTempTo, strTo.GetLength() + 1, strTo);
	szTempTo[strTo.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.

	FileOp.hwnd						= NULL;
	FileOp.wFunc					= FO_MOVE;
	FileOp.pFrom					= (LPCWSTR)szTempFrom;
	FileOp.pTo						= (LPCWSTR)szTempTo;
	FileOp.fFlags					= FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT ;	// 확인메시지가 안 뜨도록 설정
	FileOp.fAnyOperationsAborted	= false;
	FileOp.hNameMappings			= NULL;
	FileOp.lpszProgressTitle		= NULL;

	int n = SHFileOperation(&FileOp);

	return TRUE;
}
void CDataManager::m_fnClearReviewDefectList()
{
	G_AddLog(3, 3, _T("m_fnClearReviewDefectList() - Start"));
	STRU_DEFECT_INFO *pDelDefectInfo;
	while(m_vectorReviewDefectInfo.size())
	{
		m_vectorReviewDefectInfo.back();
		pDelDefectInfo = m_vectorReviewDefectInfo[m_vectorReviewDefectInfo.size()-1];
		if(pDelDefectInfo)
		{
			delete pDelDefectInfo;
			pDelDefectInfo = NULL;
		}
		m_vectorReviewDefectInfo.pop_back(); 
	}
	G_AddLog(3, 3, _T("m_fnClearReviewDefectList() - End"));
}

void CDataManager::m_fnUpdateSelReviewDefectList(CString strPath)
{
	G_AddLog(3, 3, _T("m_fnUpdateSelReviewDefectList() - Start"));
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if(m_nSelectSummary > (int)m_vectorDefectFile.size())
	{
		return;
	}

	if(m_nSelectSummary < 0)
	{
		return;
	}

	TCHAR tszFindPath[MAX_PATH] = {0,};
	_stprintf_s(tszFindPath, MAX_PATH, _T("%s"), strPath);

	//STRU_DEFECT_INFO *pRevDetectInfo = NULL;

	char szLine[1024] = {0,};
	char *pszLine	  = NULL;
	char *pszToken	  = NULL;
	char *pszTemp	  = NULL;
	FILE *pFile		  = NULL;

	//2016.02.01 Packing Defect
	CString strInspDate;
	CString strInspTime;
	CString strCellID;
	CString strDevice;
	CString strPad;
	//2016.02.01 Packing Defect

	int curPos = 0;
	CString strVal = _T("");

	try
	{

		_tfopen_s(&pFile, tszFindPath, _T("rb"));
		if(pFile)
		{
			int nCnt = 0;
			while(1)
			{
				pszLine = fgets(szLine, 1024, pFile);
				if(pszLine == NULL)
				{
					break;
				}

				if(m_vectorReviewDefectInfo.size())
				{
					m_vectorReviewDefectInfo.back();
				}

				strtok_s(szLine   , ",", &pszTemp);

				//2016.02.01 Packing Defect
				if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
				{
					if(!strncmp(pszTemp, "HEADERDATA", strlen("HEADERDATA")))
					{
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Insp Date
						if(pszToken == NULL)break;
						strInspDate = pszToken;

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Insp Time
						if(pszToken == NULL)break;
						strInspTime = pszToken;

						strInspDate.Append(_T(" ") + strInspTime);
					}
					if(!strncmp(pszTemp, "PANELDATA", strlen("PANELDATA")))
					{
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Cell ID
						if(pszToken == NULL)break;
						strCellID = pszToken;

						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);
						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Device
						if(pszToken == NULL)break;
						strDevice = pszToken;

						strtok_s(NULL   , ",", &pszTemp);

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Left
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("L"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Top
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("T"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Right
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("R"));
						else	strPad.Append(_T("0"));

						pszToken = strtok_s(NULL   , ",", &pszTemp);	//Pad Bottom
						if(pszToken == NULL)break;
						if(!strcmp(pszToken, "Y"))		strPad.Append(_T("B"));
						else	strPad.Append(_T("0"));

					}
				}
				//2016.02.01 Packing Defect
				//2016.03.21 Mr.Chu				지난 Review Summary panel ID 저장	
				else if(!strncmp(pszTemp, "PANELDATA", strlen("PANELDATA")))		//Packing Project 가 아닐때
				{

					strtok_s(NULL   , ",", &pszTemp);

					pszToken = strtok_s(NULL   , ",", &pszTemp);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;

					m_cRevPanelInfo = NULL;
					m_cRevPanelInfo = new char[strlen(pszToken)];
					memcpy(m_cRevPanelInfo,pszToken,strlen(pszToken));
				}
				//

				if(!strncmp(pszTemp, "DEFECT", strlen("DEFECT")))
				{

					pRevDetectInfo = NULL;
					pRevDetectInfo = new STRU_DEFECT_INFO;

					//strtok_s(szLine   , ",", &pszTemp);
					strtok_s(NULL   , ",", &pszTemp);
					/// DEFECT INFO START //////////////////////////////////	

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.layer_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Defect_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Defect_Code = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Flag = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Pixel_X = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Pixel_Y = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Gate_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Data_Start_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Gate_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Data_End_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Coord_Start_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Coord_Start_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Coord_End_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Coord_End_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Defect_Size = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Defect_Img_No = (pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Img_Size_X = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Img_Size_Y = atof(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Defect_Type = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);
					if(pszToken == NULL)break;
					pRevDetectInfo->Dinfo.Camera_No = atoi(pszToken);

					pszToken = strtok_s(NULL   , ",", &pszTemp);	//Light
					if(pszToken == NULL)break;

					//2016.02.01 Packing Defect
					if (pFrame->m_stParamInfo.nEqpObject == P_PROJECT)
					{

						pRevDetectInfo->Dinfo.InspTime = (strInspDate);
						pRevDetectInfo->Dinfo.Cell_ID = (strCellID);
						pRevDetectInfo->Dinfo.Device = (strDevice);
						pRevDetectInfo->Dinfo.Pad = (strPad);
						pRevDetectInfo->Dinfo.Judge = _T("DEFECT");

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.P_Defect_Type = (pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Area = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Box_Area = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Box_X_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Box_X_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Box_Y_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Box_Y_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Center_X = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Center_Y = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Length = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Breadth = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Elongation = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Compactness = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Holecount = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Min_Angle = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Max_Angle = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Mean = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_X = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Y = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Feret_Elongation = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Pixel_Max = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Pixel_Mean = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Pixel_Min = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Pixel_Sigma = atof(pszToken);

						pszToken = strtok_s(NULL   , ",", &pszTemp);
						if(pszToken == NULL)break;
						pRevDetectInfo->Dinfo.Pixel_Sum = atof(pszToken);
					}

					pszToken = strtok_s(NULL   , ",", &pszTemp);	//Defect Image
					pRevDetectInfo->Dinfo.Defect_Img_Path = _T("");
					pRevDetectInfo->Dinfo.Defect_Img_Path.Empty();

					if(pszToken == NULL)

						pRevDetectInfo->Dinfo.Defect_Img_Path = _T("");
					else
						pRevDetectInfo->Dinfo.Defect_Img_Path = (pszToken);

					m_vectorReviewDefectInfo.push_back(pRevDetectInfo);

				}
				nCnt++;
			}
			fclose(pFile);
			pFile = NULL;
		}
	}
	catch (CMemoryException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelReviewDefectList Failed !! - Memory Exception "));
		e->Delete();
	}
	catch (CFileException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelReviewDefectList Failed !! - File Exception "));
		e->Delete();
	}
	catch (CException* e)
	{
		G_AddLog(2, 3, _T("m_fnUpdateSelReviewDefectList Failed !! - Exception "));
		e->Delete();
	}
	G_AddLog(3, 3, _T("m_fnUpdateSelReviewDefectList() - End"));
}

UINT CDataManager::GetReviewDefectCount()
{
	return m_vectorReviewDefectInfo.size();
}

STRU_DEFECT_INFO *CDataManager::GetReviewDefectInfo(int nIndex)
{
	return m_vectorReviewDefectInfo[nIndex];
}

//파일 ---> 구조체 
STRU_NG_FLAG_INFO CDataManager::m_fnReadNgFlagList(CString strPanelId)     
{ 
	G_AddLog(3, 3, _T("m_fnReadNgFlagList() - Start"));
	FILE *in = NULL;
	int nCnt = 0;
	CString strVal, strNgFlagInfoPath;

	//strVal = (_T("PNL000000000000000"));
	STRU_NG_FLAG_INFO stNgFlagInfo;
	errno_t errFileOpen;
	//FILE* hAgPramFileHandle = NULL;

	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strNgFlagInfoPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_stNgFlagInfo, sizeof(m_stNgFlagInfo), 1, in);			
		fclose(in);
	}	

	while(nCnt < MAX_REV_PANEL * 2)
	{
		strVal = m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelId /*&& m_stNgFlagInfo[nCnt].bComplete*/)
		{				
			stNgFlagInfo = m_stNgFlagInfo[nCnt];
			//m_stNgFlagInfo[nCnt].init_info();
			break;
		}		
		nCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnReadNgFlagList() - End"));
	return stNgFlagInfo;
} 

void CDataManager::m_fnReadPanelCodeData(STRU_PANEL_CODE_TEST* stData)
{
	FILE	*in = NULL;
	CString strDataPath = _T("");
	errno_t errFileOpen;

	strDataPath.Format(_T("%s%s"), ROOT_PATH, PANEL_CODE_TEST_DATA_PATH);

	errFileOpen = _tfopen_s(&in, strDataPath, _T("rb+"));

	if(errFileOpen == 0)
	{		
		fread(&*stData, sizeof(*stData), 1, in);			
		fclose(in);
	}	
	else
	{
		stData->init();
		m_fnWritePanelCodeData(*stData);
	}
}

void CDataManager::m_fnWritePanelCodeData(STRU_PANEL_CODE_TEST stData)
{
	FILE	*out = NULL;
	CString strDataPath = _T("");
	errno_t errFileOpen;

	strDataPath.Format(_T("%s%s"), ROOT_PATH, PANEL_CODE_TEST_DATA_PATH);

	errFileOpen = _tfopen_s(&out, strDataPath, _T("wb"));

	if(errFileOpen == 0)
	{		
		fwrite(&stData, sizeof(stData) , 1, out);		
		fclose(out);
	}	
}

//구조체 ---> 파일 
void CDataManager::m_fnWriteNgFlagList(STRU_NG_FLAG_INFO stData)
{ 
	G_AddLog(3, 3, _T("m_fnWriteNgFlagList() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strPanelid, strVal, strNgFlagInfoPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	strPanelid = stData.cPanelID; strVal.TrimRight();

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();		
		if (strVal == strPanelid)				
		{					
			/*_tcscpy(m_stNgFlagInfo[nCnt].cPanelID , strPanelid);			
			m_stNgFlagInfo[nCnt].nNgFlag = stData.nNgFlag;	
			_tcscpy(m_stNgFlagInfo[nCnt].cGrade , stData.cGrade);*/
			m_stNgFlagInfo[nCnt] = stData;
			m_stNgFlagInfo[nCnt].bComplete = TRUE; 
			bPanelUseChk = TRUE;
			G_AddLog(3, 2, _T("m_fnWriteNgFlagList - WRITE NG FLAG [%s] NG Flag = [%d] Complete = [%d] "), strPanelid, m_stNgFlagInfo[nCnt].nNgFlag, m_stNgFlagInfo[nCnt].bComplete);
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				m_stNgFlagInfo[nCnt].init_info();
				/*_tcscpy(m_stNgFlagInfo[nCnt].cPanelID , strPanelid);
				m_stNgFlagInfo[nCnt].nNgFlag = nNgFlag;
				_tcscpy(m_stNgFlagInfo[nCnt].cGrade , stData.cGrade);*/
				//m_stNgFlagInfo[nCnt].cGrade = stData.cGrade;
				m_stNgFlagInfo[nCnt] = stData;
				m_stNgFlagInfo[nCnt].bComplete = TRUE; 		
				m_stNgFlagInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				G_AddLog(3, 2, _T("m_fnWriteNgFlagList - WRITE NG FLAG [%s] NG Flag = [%d] Complete = [%d] "), strPanelid, m_stNgFlagInfo[nCnt].nNgFlag, m_stNgFlagInfo[nCnt].bComplete);
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{		
		nCnt = 0;
		llCmpTime  = m_stNgFlagInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stNgFlagInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stNgFlagInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
			}			
			nCnt++;
		}
		m_stNgFlagInfo[ntimeInx].init_info();
		/*_tcscpy(m_stNgFlagInfo[ntimeInx].cPanelID , strPanelid);
		m_stNgFlagInfo[ntimeInx].nNgFlag = nNgFlag;
		_tcscpy(m_stNgFlagInfo[nCnt].cGrade , stData.cGrade);*/		
		m_stNgFlagInfo[ntimeInx] = stData;
		m_stNgFlagInfo[ntimeInx].bComplete = TRUE; 		
		m_stNgFlagInfo[ntimeInx].llStartTime = llStartTime;
		G_AddLog(3, 2, _T("m_fnWriteNgFlagList - WRITE NG FLAG [%s] NG Flag = [%d] Complete = [%d] "), strPanelid, m_stNgFlagInfo[ntimeInx].nNgFlag, m_stNgFlagInfo[ntimeInx].bComplete);

	}	

	/////////////////////////////////////////////////////////////
	if(errFileOpen == 0)
	{		
		fwrite(&m_stNgFlagInfo, sizeof(m_stNgFlagInfo) , 1, out);		
		fclose(out);
	}	
	G_AddLog(3, 3, _T("m_fnWriteNgFlagList() - End"));
	//////////////////////////////////////////////////////////////////////////////////
}

//2015.08.19
//파일 ---> 구조체 
STRU_ALIGN_INFO CDataManager::m_fnReadAlignData(CString strPanelId)     
{ 
	G_AddLog(3, 3, _T("m_fnReadAlignData() - Start"));
	FILE *in = NULL;
	int nCnt = 0;
	CString strVal, strAlignDataPath;
	STRU_ALIGN_INFO stAlignInfo;
	errno_t errFileOpen;
	//	FILE* hAgPramFileHandle = NULL;

	strAlignDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strAlignDataPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_stAlignInfo, sizeof(m_stAlignInfo), 1, in);			
		fclose(in);
	}	

	while(nCnt < MAX_REV_PANEL * 2)
	{
		strVal = m_stAlignInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelId)
		{				
			stAlignInfo = m_stAlignInfo[nCnt];
			break;
		}		
		nCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnReadAlignData() - End"));
	return stAlignInfo;
} 

void CDataManager::m_fnWriteAlignData(CString strPanelid, BOOL bComplete, BOOL bAlignResult) 
{ 
	G_AddLog(3, 3, _T("m_fnWriteAlignData() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strAlignDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strAlignDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strAlignDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stAlignInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stAlignInfo[nCnt].cPanelID , strPanelid);
			m_stAlignInfo[nCnt].bAlignResult = bAlignResult;
			m_stAlignInfo[nCnt].bAlignComplete = bComplete; 
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stAlignInfo[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stAlignInfo[nCnt].cPanelID , strPanelid);
				m_stAlignInfo[nCnt].bAlignResult = bAlignResult;
				m_stAlignInfo[nCnt].bAlignComplete = bComplete;		
				m_stAlignInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stAlignInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stAlignInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stAlignInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
			}			
			nCnt++;
		}

		_tcscpy(m_stAlignInfo[ntimeInx].cPanelID , strPanelid);
		m_stAlignInfo[ntimeInx].bAlignResult = bAlignResult;
		m_stAlignInfo[ntimeInx].bAlignComplete = bComplete; 		
		m_stAlignInfo[ntimeInx].llStartTime = llStartTime;

	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stAlignInfo, sizeof(m_stAlignInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	
	G_AddLog(3, 3, _T("m_fnWriteAlignData() - End"));
}




//파일 ---> 구조체 
STRU_ALIGN_MEASURE_INFO CDataManager::m_fnReadAlignMeasureData(CString strPanelId)     
{ 
	G_AddLog(3, 3, _T("m_fnReadAlignMeasureData() - Start"));
	FILE *in = NULL;
	int nCnt = 0;
	CString strVal, strAlignMeasureDataPath;
	STRU_ALIGN_MEASURE_INFO stAlignMeasureInfo;
	errno_t errFileOpen;
	//FILE* hAgPramFileHandle = NULL;
	stAlignMeasureInfo.init_info();
	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strAlignMeasureDataPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_stAlignMeasureInfo, sizeof(m_stAlignMeasureInfo), 1, in);			
		fclose(in);
	}	

	while(nCnt < MAX_REV_PANEL * 2)
	{
		strVal = m_stAlignMeasureInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelId)
		{				
			stAlignMeasureInfo = m_stAlignMeasureInfo[nCnt];
			break;
		}		
		nCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnReadAlignMeasureData() - End"));
	return stAlignMeasureInfo;
} 

//구조체 ---> 파일 
void CDataManager::m_fnWriteAlignMeasureData(CString strPanelid, CString strMeasureData, int nDstAngFlag, CString strCFFail, CString strTFTFail) 
{ 
	G_AddLog(3, 3, _T("m_fnWriteAlignMeasureData() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strAlignMeasureDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strAlignMeasureDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stAlignMeasureInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);
			if(strCFFail == MEASUREFAIL_CF)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureFail , strCFFail);
			if(strTFTFail == MEASUREFAIL_TFT)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureFail , strTFTFail);
	        
			if (nDstAngFlag == CF_MEASURE_DST)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureDistanceData , strMeasureData);
			else if(nDstAngFlag == CF_MEASURE_ANG)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureAngleData , strMeasureData);
			else if(nDstAngFlag == TFT_MEASURE_DST)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureDistanceData , strMeasureData);
			else if(nDstAngFlag == TFT_MEASURE_ANG)
				_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureAngleData , strMeasureData);
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stAlignMeasureInfo[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);
				if(strCFFail == MEASUREFAIL_CF)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureFail , strCFFail);
				if(strTFTFail == MEASUREFAIL_TFT)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureFail , strTFTFail);

				if (nDstAngFlag == CF_MEASURE_DST)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureDistanceData , strMeasureData);
				else if(nDstAngFlag == CF_MEASURE_ANG)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cCFMeasureAngleData , strMeasureData);
				else if(nDstAngFlag == TFT_MEASURE_DST)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureDistanceData , strMeasureData);
				else if(nDstAngFlag == TFT_MEASURE_ANG)
					_tcscpy(m_stAlignMeasureInfo[nCnt].cTFTMeasureAngleData , strMeasureData);
				m_stAlignMeasureInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stAlignMeasureInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stAlignMeasureInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stAlignMeasureInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
				break;
			}			
			nCnt++;
		}

		m_stAlignMeasureInfo[ntimeInx].init_info();

		_tcscpy(m_stAlignMeasureInfo[ntimeInx].cPanelID , strPanelid);
		if(strCFFail == MEASUREFAIL_CF)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cCFMeasureFail , strCFFail);
		if(strTFTFail == MEASUREFAIL_TFT)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cTFTMeasureFail , strTFTFail);

		if (nDstAngFlag == CF_MEASURE_DST)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cCFMeasureDistanceData , strMeasureData);
		else if(nDstAngFlag == CF_MEASURE_ANG)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cCFMeasureAngleData , strMeasureData);
		else if(nDstAngFlag == TFT_MEASURE_DST)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cTFTMeasureDistanceData , strMeasureData);
		else if(nDstAngFlag == TFT_MEASURE_ANG)
			_tcscpy(m_stAlignMeasureInfo[ntimeInx].cTFTMeasureAngleData , strMeasureData);
		m_stAlignMeasureInfo[ntimeInx].llStartTime = llStartTime;
		
	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stAlignMeasureInfo, sizeof(m_stAlignMeasureInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	
	G_AddLog(3, 3, _T("m_fnWriteAlignMeasureData() - End"));
}

//2016.08.09	Mr.Chu	Monitor Measure
void CDataManager::m_fnWriteMonitorMeasureData(CString strPanelid, BOOL bComplete, DOUBLE dTemp_X1, DOUBLE dTemp_Y1, DOUBLE dTemp_X2, DOUBLE dTemp_Y2, DOUBLE dTemp_L1, DOUBLE dTemp_W1, int nVisionNo)
{
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;

	CString strVal, strAlignMeasureDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strAlignMeasureDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stAlignMeasureInfo[nCnt].cPanelID; 
		strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);

			if ( nVisionNo == VISION_PC_1)	//CF
			{
				m_stAlignMeasureInfo[nCnt].dCF_X3	= dTemp_X1;
				m_stAlignMeasureInfo[nCnt].dCF_Y3	= dTemp_Y1;
				m_stAlignMeasureInfo[nCnt].dCF_X4	= dTemp_X2;
				m_stAlignMeasureInfo[nCnt].dCF_Y4	= dTemp_Y2;
				m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;

				m_stAlignMeasureInfo[nCnt].bVision1Complete = TRUE;
			}
			else if(nVisionNo == VISION_PC_2)
			{
				m_stAlignMeasureInfo[nCnt].dCF_X1	= dTemp_X1;
				m_stAlignMeasureInfo[nCnt].dCF_Y1	= dTemp_Y1;
				m_stAlignMeasureInfo[nCnt].dCF_X2	= dTemp_X2;
				m_stAlignMeasureInfo[nCnt].dCF_Y2	= dTemp_Y2;
				m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W1;

				m_stAlignMeasureInfo[nCnt].bVision2Complete = TRUE;
			}
			else if ( nVisionNo == VISION_PC_5)	//TFT
			{
				m_stAlignMeasureInfo[nCnt].dTFT_X3	= dTemp_X1;
				m_stAlignMeasureInfo[nCnt].dTFT_Y3	= dTemp_Y1;
				m_stAlignMeasureInfo[nCnt].dTFT_X4	= dTemp_X2;
				m_stAlignMeasureInfo[nCnt].dTFT_Y4	= dTemp_Y2;
				m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;

				m_stAlignMeasureInfo[nCnt].bVision5Complete = TRUE;
			}
			else if (nVisionNo == VISION_PC_6 )
			{
				m_stAlignMeasureInfo[nCnt].dTFT_X1	= dTemp_X1;
				m_stAlignMeasureInfo[nCnt].dTFT_Y1	= dTemp_Y1;
				m_stAlignMeasureInfo[nCnt].dTFT_X2	= dTemp_X2;
				m_stAlignMeasureInfo[nCnt].dTFT_Y2	= dTemp_Y2;
				m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W1;

				m_stAlignMeasureInfo[nCnt].bVision6Complete = TRUE;
			}
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stAlignMeasureInfo[nCnt].cPanelID; 
			strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);

				if ( nVisionNo == VISION_PC_1)	//CF
				{
					m_stAlignMeasureInfo[nCnt].dCF_X3	= dTemp_X1;
					m_stAlignMeasureInfo[nCnt].dCF_Y3	= dTemp_Y1;
					m_stAlignMeasureInfo[nCnt].dCF_X4	= dTemp_X2;
					m_stAlignMeasureInfo[nCnt].dCF_Y4	= dTemp_Y2;
					m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;

					m_stAlignMeasureInfo[nCnt].bVision1Complete = TRUE;
				}
				else if (nVisionNo == VISION_PC_2 )
				{
					m_stAlignMeasureInfo[nCnt].dCF_X1	= dTemp_X1;
					m_stAlignMeasureInfo[nCnt].dCF_Y1	= dTemp_Y1;
					m_stAlignMeasureInfo[nCnt].dCF_X2	= dTemp_X2;
					m_stAlignMeasureInfo[nCnt].dCF_Y2	= dTemp_Y2;
					m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W1;

					m_stAlignMeasureInfo[nCnt].bVision2Complete = TRUE;
				}
				else if ( nVisionNo == VISION_PC_5)	//TFT
				{
					m_stAlignMeasureInfo[nCnt].dTFT_X3	= dTemp_X1;
					m_stAlignMeasureInfo[nCnt].dTFT_Y3	= dTemp_Y1;
					m_stAlignMeasureInfo[nCnt].dTFT_X4	= dTemp_X2;
					m_stAlignMeasureInfo[nCnt].dTFT_Y4	= dTemp_Y2;
					m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;

					m_stAlignMeasureInfo[nCnt].bVision5Complete = TRUE;
				}
				else if (nVisionNo == VISION_PC_6 )
				{
					m_stAlignMeasureInfo[nCnt].dTFT_X1	= dTemp_X1;
					m_stAlignMeasureInfo[nCnt].dTFT_Y1	= dTemp_Y1;
					m_stAlignMeasureInfo[nCnt].dTFT_X2	= dTemp_X2;
					m_stAlignMeasureInfo[nCnt].dTFT_Y2	= dTemp_Y2;
					m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W1;

					m_stAlignMeasureInfo[nCnt].bVision6Complete = TRUE;
				}
				m_stAlignMeasureInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stAlignMeasureInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stAlignMeasureInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stAlignMeasureInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
				break;
			}			
			nCnt++;
		}

		m_stAlignMeasureInfo[ntimeInx].init_info();		// TFT 측정데이터 꼬임 방지를 위한 초기화

		if ( nVisionNo == VISION_PC_1)	//CF
		{
			m_stAlignMeasureInfo[ntimeInx].dCF_X3	= dTemp_X1;
			m_stAlignMeasureInfo[ntimeInx].dCF_Y3	= dTemp_Y1;
			m_stAlignMeasureInfo[ntimeInx].dCF_X4	= dTemp_X2;
			m_stAlignMeasureInfo[ntimeInx].dCF_Y4	= dTemp_Y2;
			m_stAlignMeasureInfo[ntimeInx].dCF_L1	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dCF_W1	= dTemp_W1;

			m_stAlignMeasureInfo[ntimeInx].bVision1Complete = TRUE;
		}
		else if (nVisionNo == VISION_PC_2 )	//CF
		{
			m_stAlignMeasureInfo[ntimeInx].dCF_X1	= dTemp_X1;
			m_stAlignMeasureInfo[ntimeInx].dCF_Y1	= dTemp_Y1;
			m_stAlignMeasureInfo[ntimeInx].dCF_X2	= dTemp_X2;
			m_stAlignMeasureInfo[ntimeInx].dCF_Y2	= dTemp_Y2;
			m_stAlignMeasureInfo[ntimeInx].dCF_L2	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dCF_W2	= dTemp_W1;

			m_stAlignMeasureInfo[ntimeInx].bVision2Complete = TRUE;
		}
		else if ( nVisionNo == VISION_PC_5)	//TFT
		{
			m_stAlignMeasureInfo[ntimeInx].dTFT_X3	= dTemp_X1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_Y3	= dTemp_Y1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_X4	= dTemp_X2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_Y4	= dTemp_Y2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_L3	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W3	= dTemp_W1;

			m_stAlignMeasureInfo[ntimeInx].bVision5Complete = TRUE;
		}
		else if (nVisionNo == VISION_PC_6 )	//TFT
		{
			m_stAlignMeasureInfo[ntimeInx].dTFT_X1	= dTemp_X1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_Y1	= dTemp_Y1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_X2	= dTemp_X2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_Y2	= dTemp_Y2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_L4	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W4	= dTemp_W1;

			m_stAlignMeasureInfo[ntimeInx].bVision6Complete = TRUE;
		}
		m_stAlignMeasureInfo[ntimeInx].llStartTime = llStartTime;
		bPanelUseChk = TRUE;
	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stAlignMeasureInfo, sizeof(m_stAlignMeasureInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	

}
//2016.11.30 by mby	Monitor Measure L1-4,W1-4
void CDataManager::m_fnWriteMonitorMeasureData_temp(CString strPanelid, BOOL bComplete, DOUBLE dTemp_L1, DOUBLE dTemp_W1, DOUBLE dTemp_L2, DOUBLE dTemp_W2,int nVisionNo)
{
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;

	CString strVal, strAlignMeasureDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strAlignMeasureDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stAlignMeasureInfo[nCnt].cPanelID; 
		strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);

			if ( nVisionNo == VISION_PC_1)	//CF
			{				
				m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;
				m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L2;
				m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W2;
			}
			else if(nVisionNo == VISION_PC_2)
			{
				m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;
				m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L2;
				m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W2;
			}
			else if ( nVisionNo == VISION_PC_5)	//TFT
			{
				m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;
				m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L2;
				m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W2;
			}
			else if (nVisionNo == VISION_PC_6 )
			{
				m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
				m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;
				m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L2;
				m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W2;
			}
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stAlignMeasureInfo[nCnt].cPanelID; 
			strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);

				if ( nVisionNo == VISION_PC_1)	//CF
				{				
					m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;
					m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L2;
					m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W2;
				}
				else if(nVisionNo == VISION_PC_2)
				{
					m_stAlignMeasureInfo[nCnt].dCF_L1	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dCF_W1	= dTemp_W1;
					m_stAlignMeasureInfo[nCnt].dCF_L2	= dTemp_L2;
					m_stAlignMeasureInfo[nCnt].dCF_W2	= dTemp_W2;
				}
				else if ( nVisionNo == VISION_PC_5)	//TFT
				{
					m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;
					m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L2;
					m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W2;
				}
				else if (nVisionNo == VISION_PC_6 )
				{
					m_stAlignMeasureInfo[nCnt].dTFT_L3	= dTemp_L1;
					m_stAlignMeasureInfo[nCnt].dTFT_W3	= dTemp_W1;
					m_stAlignMeasureInfo[nCnt].dTFT_L4	= dTemp_L2;
					m_stAlignMeasureInfo[nCnt].dTFT_W4	= dTemp_W2;
				}
				m_stAlignMeasureInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stAlignMeasureInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stAlignMeasureInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stAlignMeasureInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
				break;
			}			
			nCnt++;
		}

		m_stAlignMeasureInfo[ntimeInx].init_info();		// TFT 측정데이터 꼬임 방지를 위한 초기화

		if ( nVisionNo == VISION_PC_1)	//CF
		{				
			m_stAlignMeasureInfo[ntimeInx].dCF_L1	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dCF_W1	= dTemp_W1;
			m_stAlignMeasureInfo[ntimeInx].dCF_L2	= dTemp_L2;
			m_stAlignMeasureInfo[ntimeInx].dCF_W2	= dTemp_W2;
		}
		else if(nVisionNo == VISION_PC_2)
		{
			m_stAlignMeasureInfo[ntimeInx].dCF_L1	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dCF_W1	= dTemp_W1;
			m_stAlignMeasureInfo[ntimeInx].dCF_L2	= dTemp_L2;
			m_stAlignMeasureInfo[ntimeInx].dCF_W2	= dTemp_W2;
		}
		else if ( nVisionNo == VISION_PC_5)	//TFT
		{
			m_stAlignMeasureInfo[ntimeInx].dTFT_L3	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W3	= dTemp_W1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_L4	= dTemp_L2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W4	= dTemp_W2;
		}
		else if (nVisionNo == VISION_PC_6 )
		{
			m_stAlignMeasureInfo[ntimeInx].dTFT_L3	= dTemp_L1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W3	= dTemp_W1;
			m_stAlignMeasureInfo[ntimeInx].dTFT_L4	= dTemp_L2;
			m_stAlignMeasureInfo[ntimeInx].dTFT_W4	= dTemp_W2;
		}
		m_stAlignMeasureInfo[ntimeInx].llStartTime = llStartTime;
		bPanelUseChk = TRUE;
	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stAlignMeasureInfo, sizeof(m_stAlignMeasureInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	

}
void CDataManager::m_fnWriteMonitorMeasureInfo(STRU_MONITOR_MEASURE_INFO stMonitorMeasureInfo)
{
	FILE	*out = NULL;
	CString strMonitorMeasureInfoPath;
	errno_t errFileOpen;

	strMonitorMeasureInfoPath.Format(_T("%s%s"), ROOT_PATH , MONITOR_MEASURE_INFO_PATH);
	errFileOpen = _tfopen_s(&out, strMonitorMeasureInfoPath, _T("wb"));

	if(errFileOpen == 0)
	{		
		fwrite(&stMonitorMeasureInfo, sizeof(stMonitorMeasureInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	

}

STRU_MONITOR_MEASURE_INFO CDataManager::m_fnReadMonitorMeasureInfo()     
{ 
	FILE *in = NULL;
	CString strMonitorMeasureInfoPath;
	STRU_MONITOR_MEASURE_INFO stMonitorMeasureInfo;
	stMonitorMeasureInfo.init_info();
	errno_t errFileOpen;

	stMonitorMeasureInfo.init_info();
	strMonitorMeasureInfoPath.Format(_T("%s%s"), ROOT_PATH , MONITOR_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strMonitorMeasureInfoPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&stMonitorMeasureInfo, sizeof(stMonitorMeasureInfo), 1, in);			
		fclose(in);
	}	

	return stMonitorMeasureInfo;
} 


void CDataManager::m_fnWriteMeasureData(CString strPanelid, BOOL bComplete, DOUBLE dX1, DOUBLE dY1, DOUBLE dX2, DOUBLE dY2, DOUBLE dAngle1, DOUBLE dAngle2, DOUBLE dDistance, int nVisionNo) 
{ 
	G_AddLog(3, 3, _T("m_fnWriteMeasureData() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strAlignMeasureDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;
	CString strX1, strY1, strX2, strY2, strAngle1, strAngle2, strDistance, strTemp;	//2015.08.19
	DOUBLE dTempX1;
	DOUBLE dTempY1;
	DOUBLE dTempX2;
	DOUBLE dTempY2;
	DOUBLE dTempAngle1;
	DOUBLE dTempAngle2;
	DOUBLE dTempDistance;	//2015.08.19

	dTempX1 = dX1;
	dTempY1 = dY1;
	dTempX2 = dX2;
	dTempY2 = dY2;
	dTempAngle1 = dAngle1;
	dTempAngle2 = dAngle2;
	dTempDistance = dDistance;

	strAlignMeasureDataPath.Format(_T("%s%s"), ROOT_PATH , ALIGN_MEASURE_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strAlignMeasureDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stAlignMeasureInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);
			if (nVisionNo == VISION_PC_5)
			{
				m_stAlignMeasureInfo[nCnt].bVision1Complete = TRUE;
				m_stAlignMeasureInfo[nCnt].dVision1_X1 = dTempX1;
				m_stAlignMeasureInfo[nCnt].dVision1_Y1 = dTempY1;
				m_stAlignMeasureInfo[nCnt].dVision1_X2 = dTempX2;
				m_stAlignMeasureInfo[nCnt].dVision1_Y2 = dTempY2;
				m_stAlignMeasureInfo[nCnt].dVision1_Angle1 = dTempAngle1;
				m_stAlignMeasureInfo[nCnt].dVision1_Angle2 = dTempAngle2;
				m_stAlignMeasureInfo[nCnt].dVision1_Distance = dTempDistance;

			}
			else if (nVisionNo == VISION_PC_6)
			{
				m_stAlignMeasureInfo[nCnt].bVision2Complete = TRUE;
				m_stAlignMeasureInfo[nCnt].dVision2_X1 = dTempX1;
				m_stAlignMeasureInfo[nCnt].dVision2_Y1 = dTempY1;
				m_stAlignMeasureInfo[nCnt].dVision2_X2 = dTempX2;
				m_stAlignMeasureInfo[nCnt].dVision2_Y2 = dTempY2;
				m_stAlignMeasureInfo[nCnt].dVision2_Angle1 = dTempAngle1;
				m_stAlignMeasureInfo[nCnt].dVision2_Angle2 = dTempAngle2;
				m_stAlignMeasureInfo[nCnt].dVision2_Distance = dTempDistance;
			}
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stAlignMeasureInfo[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stAlignMeasureInfo[nCnt].cPanelID , strPanelid);
				if (nVisionNo == VISION_PC_5)
				{
					m_stAlignMeasureInfo[nCnt].bVision1Complete = TRUE;
					m_stAlignMeasureInfo[nCnt].dVision1_X1 = dTempX1;
					m_stAlignMeasureInfo[nCnt].dVision1_Y1 = dTempY1;
					m_stAlignMeasureInfo[nCnt].dVision1_X2 = dTempX2;
					m_stAlignMeasureInfo[nCnt].dVision1_Y2 = dTempY2;
					m_stAlignMeasureInfo[nCnt].dVision1_Angle1 = dTempAngle1;
					m_stAlignMeasureInfo[nCnt].dVision1_Angle2 = dTempAngle2;
					m_stAlignMeasureInfo[nCnt].dVision1_Distance = dTempDistance;

				}
				else if (nVisionNo == VISION_PC_6)
				{
					m_stAlignMeasureInfo[nCnt].bVision2Complete = TRUE;
					m_stAlignMeasureInfo[nCnt].dVision2_X1 = dTempX1;
					m_stAlignMeasureInfo[nCnt].dVision2_Y1 = dTempY1;
					m_stAlignMeasureInfo[nCnt].dVision2_X2 = dTempX2;
					m_stAlignMeasureInfo[nCnt].dVision2_Y2 = dTempY2;
					m_stAlignMeasureInfo[nCnt].dVision2_Angle1 = dTempAngle1;
					m_stAlignMeasureInfo[nCnt].dVision2_Angle2 = dTempAngle2;
					m_stAlignMeasureInfo[nCnt].dVision2_Distance = dTempDistance;
				}
				m_stAlignMeasureInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stAlignMeasureInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stAlignMeasureInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stAlignMeasureInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
			}			
			nCnt++;
		}

		_tcscpy(m_stAlignMeasureInfo[ntimeInx].cPanelID , strPanelid);
		if (nVisionNo == VISION_PC_5)
		{
			m_stAlignMeasureInfo[ntimeInx].bVision1Complete = TRUE;
			m_stAlignMeasureInfo[ntimeInx].dVision1_X1 = dTempX1;
			m_stAlignMeasureInfo[ntimeInx].dVision1_Y1 = dTempY1;
			m_stAlignMeasureInfo[ntimeInx].dVision1_X2 = dTempX2;
			m_stAlignMeasureInfo[ntimeInx].dVision1_Y2 = dTempY2;
			m_stAlignMeasureInfo[ntimeInx].dVision1_Angle1 = dTempAngle1;
			m_stAlignMeasureInfo[ntimeInx].dVision1_Angle2 = dTempAngle2;
			m_stAlignMeasureInfo[ntimeInx].dVision1_Distance = dTempDistance;

		}
		else if (nVisionNo == VISION_PC_6)
		{
			m_stAlignMeasureInfo[ntimeInx].bVision2Complete = TRUE;
			m_stAlignMeasureInfo[ntimeInx].dVision2_X1 = dTempX1;
			m_stAlignMeasureInfo[ntimeInx].dVision2_Y1 = dTempY1;
			m_stAlignMeasureInfo[ntimeInx].dVision2_X2 = dTempX2;
			m_stAlignMeasureInfo[ntimeInx].dVision2_Y2 = dTempY2;
			m_stAlignMeasureInfo[ntimeInx].dVision2_Angle1 = dTempAngle1;
			m_stAlignMeasureInfo[ntimeInx].dVision2_Angle2 = dTempAngle2;
			m_stAlignMeasureInfo[ntimeInx].dVision2_Distance = dTempDistance;
		}
		m_stAlignMeasureInfo[ntimeInx].llStartTime = llStartTime;

	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stAlignMeasureInfo, sizeof(m_stAlignMeasureInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	
	G_AddLog(3, 3, _T("m_fnWriteMeasureData() - End"));
}
//
//STRU_NG_FLAG_INFO CDataManager::m_fnCompareNgTime(long long llNgTime) 
//{ 	
//	//////////////////////////////////////////////////////////////////////////////////
//	FILE	*out = NULL;
//	int		nCnt = 0;
//	BOOL	bPanelUseChk = FALSE;
//	CString strVal, strNgFlagInfoPath;
//	errno_t errFileOpen;
//	SYSTEMTIME	systime;
//
//	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);
//
//	errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
//	size_t read_size = 0;
//	/////////////////////////////////////////////////////////////
//
//	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
//	while(nCnt < MAX_REV_PANEL * 2)
//	{		
//		strVal = m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
//		if (strVal == strPanelid)				
//		{		
//			_tcscpy(m_stNgFlagInfo[nCnt].cPanelID , strPanelid);
//			if (m_stNgFlagInfo[nCnt].bComplete)	
//				m_stNgFlagInfo[nCnt].nNgFlag = nNgFlag;			
//			m_stNgFlagInfo[nCnt].bComplete = TRUE; 
//			bPanelUseChk = TRUE;
//			break;
//		}
//		nCnt++;
//	}
//
//	if (!bPanelUseChk)
//	{
//		nCnt = 0;
//		while(nCnt < MAX_REV_PANEL * 2)
//		{
//			strVal = m_stNgFlagInfo[nCnt].cPanelID; strVal.TrimRight();
//			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
//			{
//				_tcscpy(m_stNgFlagInfo[nCnt].cPanelID , strPanelid);
//				m_stNgFlagInfo[nCnt].nNgFlag = nNgFlag;
//				m_stNgFlagInfo[nCnt].bComplete = TRUE; 									
//				break;
//			}			
//			nCnt++;
//		}
//	}	
//
//	/////////////////////////////////////////////////////////////
//	if(errFileOpen == 0)
//	{		
//		fwrite(&m_stNgFlagInfo, sizeof(m_stNgFlagInfo) , 1, out);		
//		fclose(out);
//	}	
//	//////////////////////////////////////////////////////////////////////////////////
//}

//2015.07.20
BOOL CDataManager::m_fnJudgeResult() 
{
	G_AddLog(3, 3, _T("m_fnJudgeResult() - Start"));
	int nEqpModel;
	BOOL bResult = FALSE;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	//0:AVL 1:ABI
	nEqpModel = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);

	if (nEqpModel == 0)
	{
		if(pFrame->m_stJudgeCount->bUseMode == TRUE &&
			m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DUST] &&
			m_stDefType.CfWhiteBlur + m_stDefType.TftWhiteBlur <= pFrame->m_stJudgeCount->nJudgeCount[INSP_WHITE_TURBIDITY] &&
			m_stDefType.ItoPeel <= pFrame->m_stJudgeCount->nJudgeCount[INSP_ITO] &&
			m_stDefType.WanesBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_EDGE_BROKEN] &&
			m_stDefType.SocketScrach <= pFrame->m_stJudgeCount->nJudgeCount[INSP_PAD_SCRATCH] &&
			m_stDefType.CfDimplePoint + m_stDefType.TftDimplePoint <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DIMPLE_POINT] &&
			m_stDefType.CfEdgeilegal + m_stDefType.TftEdgeilegal <= pFrame->m_stJudgeCount->nJudgeCount[INSP_ETCHING_LINE] &&
			m_stDefType.CfDimpleLine + m_stDefType.TftDimpleLine <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DIMPLE_LINE] &&
			m_stDefType.SocketBurn <= pFrame->m_stJudgeCount->nJudgeCount[INSP_PAD_BURNT] &&
			m_stDefType.Burr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_BURR] &&
			//m_stDefType.CfPolBubble + m_stDefType.TftPolBubble <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_BUBBLE] &&
			//m_stDefType.CfPolScrach + m_stDefType.TftPolScrach <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_SCRATCH] &&
			//m_stDefType.polSubstance <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_ALIEN_SUBSTANCE] &&
			//m_stDefType.PolMinor  <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_MINOR] &&
			m_stDefType.CfScrach + m_stDefType.TftScrach <= pFrame->m_stJudgeCount->nJudgeCount[INSP_BLACK_SCRATCH] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_WHITE_SCRATCH] &&
			m_stDefType.CfMeasureFail <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_MEASURE] &&
			m_stDefType.TftMeasureFail <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_MEASURE] &&
			m_stDefType.mura <= pFrame->m_stJudgeCount->nJudgeCount[INSP_MURA] &&
			m_stDefType.CfBurr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_BURR] &&
			m_stDefType.TftBurr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_BURR] &&
			m_stDefType.CfBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_BROKEN] &&
			m_stDefType.TftBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_BROKEN] &&
			m_stDefType.CfCornerBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_CORNER_BROKEN] &&
			m_stDefType.TftCornerBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_CORNER_BROKEN])
		{
			bResult = TRUE;
		}
	}
	else if(nEqpModel == 1)
	{
		if(pFrame->m_stJudgeCount->bUseMode == TRUE &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DUST] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_WHITE_TURBIDITY] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_ITO] &&
			m_stDefType.WanesBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_EDGE_BROKEN] &&
			m_stDefType.SocketScrach <= pFrame->m_stJudgeCount->nJudgeCount[INSP_PAD_SCRATCH] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DIMPLE_POINT] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_ETCHING_LINE] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_DIMPLE_LINE] &&
			m_stDefType.SocketBurn <= pFrame->m_stJudgeCount->nJudgeCount[INSP_PAD_BURNT] &&
			m_stDefType.Burr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_BURR] &&
			m_stDefType.CfPolBubble + m_stDefType.TftPolBubble <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_BUBBLE] &&
			m_stDefType.CfPolScrach + m_stDefType.TftPolScrach <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_SCRATCH] &&
			m_stDefType.CFpolSubstance <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_ALIEN_SUBSTANCE] &&
			m_stDefType.TFTpolSubstance <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_ALIEN_SUBSTANCE+1] &&
			//m_stDefType.PolMinor  <= pFrame->m_stJudgeCount->nJudgeCount[INSP_POL_MINOR] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_BLACK_SCRATCH] &&
			//m_stDefType.DustType <= pFrame->m_stJudgeCount->nJudgeCount[INSP_WHITE_SCRATCH] &&
			m_stDefType.CfMeasureFail <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_MEASURE] &&
			m_stDefType.TftMeasureFail <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_MEASURE] &&
			m_stDefType.mura <= pFrame->m_stJudgeCount->nJudgeCount[INSP_MURA] &&
			m_stDefType.CfBurr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_BURR] &&
			m_stDefType.TftBurr <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_BURR] &&
			m_stDefType.CfBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_BROKEN] &&
			m_stDefType.TftBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_BROKEN] &&
			m_stDefType.CfCornerBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_CF_CORNER_BROKEN] &&
			m_stDefType.TftCornerBroken <= pFrame->m_stJudgeCount->nJudgeCount[INSP_TFT_CORNER_BROKEN])
		{
			bResult = TRUE;
		}

	}
	G_AddLog(3, 3, _T("m_fnJudgeResult() - End"));
	return bResult;
}

//2015.08.15
//기능 : 1. 모니터 : dDefLength 길이  //  dDefThinkness 브레스
void CDataManager::m_fnPanelJudgeCheck(int nDefType, int nDefFlag, DOUBLE dDefDistance, DOUBLE dDefLength, DOUBLE dDefThinkness, int nDefLumi) 
{ 
	G_AddLog(3, 3, _T("m_fnPanelJudgeCheck() - Start"));
	DOUBLE dSetDistance1, dSetDistance2, dSetThinkness1, dSetThinkness2, dSetLength;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	int nEqpSizeType = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_PARAM_EQ_SIZE_TYPE, 0);

	switch (nDefType)
	{
	case INSP_DUST:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_DUST];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_DUST];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_DUST].nJudgeCount = m_stPanelJudgeInfo[INSP_DUST].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_DUST].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_WHITE_TURBIDITY:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_WHITE_TURBIDITY].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_WHITE_TURBIDITY].bIsCF = TRUE;
		}

		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_WHITE_TURBIDITY];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_WHITE_TURBIDITY];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_WHITE_TURBIDITY].nJudgeCount = m_stPanelJudgeInfo[INSP_WHITE_TURBIDITY].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_WHITE_TURBIDITY].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_ITO:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_ITO];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_ITO];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_ITO].nJudgeCount = m_stPanelJudgeInfo[INSP_ITO].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_ITO].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_EDGE_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_EDGE_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_EDGE_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_EDGE_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_EDGE_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_EDGE_BROKEN].bJudgeGradeA = FALSE;			
		}
	
		break;

	case INSP_PAD_SCRATCH:
		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_PAD_SCRATCH];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_PAD_SCRATCH];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_PAD_SCRATCH];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_PAD_SCRATCH].nJudgeCount = m_stPanelJudgeInfo[INSP_PAD_SCRATCH].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_PAD_SCRATCH].bJudgeGradeA = FALSE;				

			}			
		}
		else
		{
			m_stPanelJudgeInfo[INSP_PAD_SCRATCH].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_DIMPLE_POINT:	
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_POINT].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_POINT].bIsCF = TRUE;
		}
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_DIMPLE_POINT];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_DIMPLE_POINT];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_POINT].nJudgeCount = m_stPanelJudgeInfo[INSP_DIMPLE_POINT].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_POINT].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_ETCHING_LINE:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_ETCHING_LINE].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_ETCHING_LINE].bIsCF = TRUE;
		}

		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_ETCHING_LINE];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_ETCHING_LINE];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_ETCHING_LINE];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_ETCHING_LINE].nJudgeCount = m_stPanelJudgeInfo[INSP_ETCHING_LINE].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_ETCHING_LINE].bJudgeGradeA = FALSE;
			}
		}
		else
		{
			m_stPanelJudgeInfo[INSP_ETCHING_LINE].bJudgeGradeA = FALSE;
		}

		break;

	case INSP_DIMPLE_LINE:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_LINE].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_LINE].bIsCF = TRUE;
		}

		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_DIMPLE_LINE];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_DIMPLE_LINE];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_DIMPLE_LINE];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_DIMPLE_LINE].nJudgeCount = m_stPanelJudgeInfo[INSP_DIMPLE_LINE].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_DIMPLE_LINE].bJudgeGradeA = FALSE;
			}
		}
		else
		{
			m_stPanelJudgeInfo[INSP_DIMPLE_LINE].bJudgeGradeA = FALSE;
		}

		break;

	case INSP_PAD_BURNT:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_PAD_BURNT];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_PAD_BURNT];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_PAD_BURNT].nJudgeCount = m_stPanelJudgeInfo[INSP_PAD_BURNT].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_PAD_BURNT].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_BURR:	//2017.01.07 Burr 작업
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_BURR].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_BURR].bIsCF = TRUE;
		}

		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_BURR];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_BURR];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_BURR].nJudgeCount = m_stPanelJudgeInfo[INSP_BURR].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_BURR].bJudgeGradeA = FALSE;		
		}	

		break;

	case INSP_POL_BUBBLE:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_POL_BUBBLE].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_POL_BUBBLE].bIsCF = TRUE;
		}

		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_POL_BUBBLE];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_BUBBLE];
		
		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_POL_BUBBLE].nJudgeCount = m_stPanelJudgeInfo[INSP_POL_BUBBLE].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_POL_BUBBLE].bJudgeGradeA = FALSE;
		}

		break;

	case INSP_POL_SCRATCH:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_POL_SCRATCH].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_POL_SCRATCH].bIsCF = TRUE;
		}

		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_POL_SCRATCH];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_POL_SCRATCH];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_POL_SCRATCH];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_POL_SCRATCH].nJudgeCount = m_stPanelJudgeInfo[INSP_POL_SCRATCH].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_POL_SCRATCH].bJudgeGradeA = FALSE;
			}
		}
		else
		{
			m_stPanelJudgeInfo[INSP_POL_SCRATCH].bJudgeGradeA = FALSE;
		}

		break;

	case INSP_POL_ALIEN_SUBSTANCE:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_POL_ALIEN_SUBSTANCE];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_ALIEN_SUBSTANCE];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE].nJudgeCount = m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE].bJudgeGradeA = FALSE;
		}
		break;	

	//case INSP_POL_MINOR:
	//	dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_POL_MINOR];
	//	dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_MINOR];

	//	if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
	//	{
	//		m_stPanelJudgeInfo[INSP_POL_MINOR].nJudgeCount = m_stPanelJudgeInfo[INSP_POL_MINOR].nJudgeCount + 1;
	//	}
	//	else if (dSetDistance2 < dDefDistance)
	//	{
	//		m_stPanelJudgeInfo[INSP_POL_MINOR].bJudgeGradeA = FALSE;
	//	}
	//	break;	

	case (INSP_POL_ALIEN_SUBSTANCE+1):
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_POL_ALIEN_SUBSTANCE+1];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_POL_ALIEN_SUBSTANCE+1];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE+1].nJudgeCount = m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE+1].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_POL_ALIEN_SUBSTANCE+1].bJudgeGradeA = FALSE;
		}
		break;	

	case INSP_BLACK_SCRATCH:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].bIsCF = TRUE;
		}
		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_BLACK_SCRATCH];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_BLACK_SCRATCH];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_BLACK_SCRATCH];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].nJudgeCount = m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].bJudgeGradeA = FALSE;			
			}			
		}
		else
		{
			m_stPanelJudgeInfo[INSP_BLACK_SCRATCH].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_MURA:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_MURA];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_MURA];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_MURA].nJudgeCount = m_stPanelJudgeInfo[INSP_MURA].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_MURA].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_CF_BURR://2017.01.07 Burr 작업
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_CF_BURR].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_CF_BURR].bIsCF = TRUE;
		}
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_BURR];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BURR];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_BURR].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_BURR].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_BURR].bJudgeGradeA = FALSE;			
		}		
		break;	

	case INSP_TFT_BURR://2017.01.07 Burr 작업
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_TFT_BURR].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_TFT_BURR].bIsCF = TRUE;
		}
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_BURR];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BURR];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_TFT_BURR].nJudgeCount = m_stPanelJudgeInfo[INSP_TFT_BURR].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_TFT_BURR].bJudgeGradeA = FALSE;			
		}		
		break;	

	case INSP_CF_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_BROKEN].bJudgeGradeA = FALSE;			
		}		
		break;

	case INSP_TFT_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_TFT_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_TFT_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_TFT_BROKEN].bJudgeGradeA = FALSE;			
		}		
		break;

	case INSP_CF_CORNER_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_CORNER_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_CORNER_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_CORNER_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_CORNER_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_CORNER_BROKEN].bJudgeGradeA = FALSE;			
		}		
		break;

	case INSP_TFT_CORNER_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_CORNER_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_CORNER_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_TFT_CORNER_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_TFT_CORNER_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_TFT_CORNER_BROKEN].bJudgeGradeA = FALSE;		
		}		
		break;
		//2015.08.26
	case INSP_CF_CELL_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_CELL_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_CELL_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_CELL_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_CELL_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_CELL_BROKEN].bJudgeGradeA = FALSE;		
		}		
		break;

	case INSP_TFT_CELL_BROKEN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_TFT_CELL_BROKEN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_TFT_CELL_BROKEN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_TFT_CELL_BROKEN].nJudgeCount = m_stPanelJudgeInfo[INSP_TFT_CELL_BROKEN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_TFT_CELL_BROKEN].bJudgeGradeA = FALSE;			
		}		
		break;

	case INSP_PIMPLE_LINE:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_LINE].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_LINE].bIsCF = TRUE;
		}
		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_PIMPLE_LINE];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_PIMPLE_LINE];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_PIMPLE_LINE];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_PIMPLE_LINE].nJudgeCount = m_stPanelJudgeInfo[INSP_PIMPLE_LINE].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_PIMPLE_LINE].bJudgeGradeA = FALSE;
			}
		}
		else
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_LINE].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_PIMPLE_POINT:
		if (nDefFlag ==  TFT )
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_POINT].bIsTFT = TRUE;
		}
		if (nDefFlag ==  CF)
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_POINT].bIsCF = TRUE;
		}

		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_PIMPLE_POINT];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_PIMPLE_POINT];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_POINT].nJudgeCount = m_stPanelJudgeInfo[INSP_PIMPLE_POINT].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_PIMPLE_POINT].bJudgeGradeA = FALSE;
		}
		break;

	case INSP_CF_PIMPLE_CIRCLE:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_PIMPLE_CIRCLE];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_PIMPLE_CIRCLE];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_PIMPLE_CIRCLE].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_PIMPLE_CIRCLE].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_PIMPLE_CIRCLE].bJudgeGradeA = FALSE;
		}
		break;
	case INSP_CF_WAVE_PATTERN:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_CF_WAVE_PATTERN];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_CF_WAVE_PATTERN];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_CF_WAVE_PATTERN].nJudgeCount = m_stPanelJudgeInfo[INSP_CF_WAVE_PATTERN].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_CF_WAVE_PATTERN].bJudgeGradeA = FALSE;
		}
		break;
	case INSP_ETC_LINE:
		dSetThinkness1 = pFrame->m_stJudgeCount->nJudgeThinknessW1[INSP_ETC_LINE];
		dSetThinkness2 = pFrame->m_stJudgeCount->nJudgeThinknessW2[INSP_ETC_LINE];
		dSetLength = pFrame->m_stJudgeCount->nJudgeLength[INSP_ETC_LINE];
		if (dDefLength <= dSetLength)
		{
			if (dSetThinkness1 < dDefThinkness && dDefThinkness <= dSetThinkness2)
			{
				m_stPanelJudgeInfo[INSP_ETC_LINE].nJudgeCount = m_stPanelJudgeInfo[INSP_ETC_LINE].nJudgeCount + 1;
			}			
			else if (dSetThinkness2 < dDefThinkness)
			{
				m_stPanelJudgeInfo[INSP_ETC_LINE].bJudgeGradeA = FALSE;
			}
		}
		else
		{
			m_stPanelJudgeInfo[INSP_ETC_LINE].bJudgeGradeA = FALSE;
		}
		break;
	case INSP_ETC_POINT:
		dSetDistance1 = pFrame->m_stJudgeCount->nJudgeDistanceD1[INSP_ETC_POINT];
		dSetDistance2 = pFrame->m_stJudgeCount->nJudgeDistanceD2[INSP_ETC_POINT];

		if (dSetDistance1 <= dDefDistance && dDefDistance <= dSetDistance2)
		{
			m_stPanelJudgeInfo[INSP_ETC_POINT].nJudgeCount = m_stPanelJudgeInfo[INSP_ETC_POINT].nJudgeCount + 1;
		}
		else if (dSetDistance2 < dDefDistance)
		{
			m_stPanelJudgeInfo[INSP_ETC_POINT].bJudgeGradeA = FALSE;		
		}	
		break;
	}
	G_AddLog(3, 3, _T("m_fnPanelJudgeCheck() - End"));
}
//2017.08.04
//기능 : m_fnCompareOverlapDefTypeCheck 함수의 정보값을 비교하여 UI 상에 결과를 나타낸다.
//Memo : 현재 정의되어있는 INSP_ - 에 대한 항목들이 뒤섞여 데이터 꼬임방지를 위한 모듈화 및 이용 ( 2017.08.04 ) ex) DefInfoTmp변수에 대한 INSP_CONTACT 배열 이후 데이터
CString CDataManager::m_fnCompareOverlapDefTypeResult(int& nOverlapCnt, CTime tTime, CString strPanelId, STRU_OVERLAP_DEFTYPE_PARAM stParam, int nCnt, int nDataJump)
{
	CString strTemp = _T(""), strMsg = _T("") ,strDefTypeName = _T(""), strPanelIdTemp = _T("");
	int nOrderCnt = 0, nDefSkipCnt = 0;
	CTimeSpan elapsedTime;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());	//2019.08.16 add by mby

	nDefSkipCnt = nCnt + nDataJump;		//PROTECTION 검사번호 변경으로 이 후 추가 번호들에 대한 예외처리

	elapsedTime = tTime - DefInfoTmp.StartDate[nDefSkipCnt];
	if (DefInfoTmp.StartDate[nDefSkipCnt] == 0) elapsedTime = 0;
	 
	for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[nDefSkipCnt][i]).IsEmpty()) {nOrderCnt = i; break;}
	strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[nDefSkipCnt][nOrderCnt -1]);
	 
	if (stParam.PARAM.nDefectCount[nCnt] != 0 
	 	&& stParam.PARAM.nDefectCheck[nCnt] && //현재 디펙 오버랩을 사용하고 있고,
	 	( stParam.PARAM.nDefectCount[nCnt] <= DefInfoTmp.DefTypeCnt[nDefSkipCnt] || // 오버랩 stop으로 설정한 갯수보다 발생한 갯수가 많거나
	 	(stParam.nAlarmCount[nCnt] <= DefInfoTmp.DefTypeCnt[nDefSkipCnt] && strPanelId == strPanelIdTemp) ) // 알람으로 설정한 갯수보다 발생한 갯수가 많고 현재 발생한 디펙일때
	 	&& (elapsedTime.GetMinutes() <= (stParam.dHour[nCnt]) || stParam.dHour[nCnt] == 0))
	{
	 	/* 알람을 출력할 데이터를 생성한다 */
	 	int nDefCnt = 0;
	 	while(nDefCnt < MAX_DEFECT_CNT)
	 	{
	 		//if (stOverlapCompDef->stPnlInfo[nDefCnt].nDefectType == nCnt)
	 		if( nDefSkipCnt == INSP_ALIGN_FAIL && nDefSkipCnt == INSP_CONTACT || nDefSkipCnt == INSP_TFT_MEASURE || nDefSkipCnt == INSP_POL_MISS || nDefSkipCnt == INSP_CF_MEASURE
				|| nDefSkipCnt == INSP_B_GRADE_CHECK || nDefSkipCnt == INSP_C_GRADE_CHECK  //2019.03.26
				) 				
				break;
			if(nDefSkipCnt >= INSP_BUBBLB_STATION1 && pFrame->m_stParamInfo.bUseBubbleAreaAlarm)	break;	//2020.02.28 add by mby
	 
	 		if (!((CString)DefInfoTmp.cPanelId[nDefSkipCnt][nDefCnt]).IsEmpty())
	 		{
	 
	 			if(stParam.PARAM.nDefectCount[nCnt] <= DefInfoTmp.DefTypeCnt[nDefSkipCnt])
	 				DefInfoTmp.bStopFlag[nDefSkipCnt] = TRUE;
			
				strDefTypeName = m_fnGetOverlapDefectType(DefInfoTmp.DefTypeCode[nDefSkipCnt], DefInfoTmp.DefTypeFlag[nDefSkipCnt][nDefCnt]);
				strTemp.Format(_T("%s,%s,%d,%0.1f,%0.3f,%0.3f,%0.3f,%0.3f,%s,%d;"),
					DefInfoTmp.cPanelId[nDefSkipCnt][nDefCnt], strDefTypeName, DefInfoTmp.DefTypeCnt[nDefSkipCnt], stParam.dHour[nCnt],
					DefInfoTmp.startX[nDefSkipCnt][nDefCnt], DefInfoTmp.startY[nDefSkipCnt][nDefCnt], DefInfoTmp.endX[nDefSkipCnt][nDefCnt], DefInfoTmp.endY[nDefSkipCnt][nDefCnt], 
					DefInfoTmp.cImgPath[nDefSkipCnt][nDefCnt], DefInfoTmp.bStopFlag[nDefSkipCnt]);
				
	 			strMsg += strTemp;
	 			nOverlapCnt++;
	 		}
	 
	 		nDefCnt++;
	 	}	
	 	if( (  nCnt != INSP_ALIGN_FAIL && nCnt != INSP_CONTACT && nCnt != INSP_TFT_MEASURE && nCnt != INSP_POL_MISS && nCnt != INSP_CF_MEASURE && nCnt != INSP_B_GRADE_CHECK && nCnt != INSP_C_GRADE_CHECK) && DefInfoTmp.bStopFlag[nDefSkipCnt] ){
	 		DefInfoTmp.init_DefType(nDefSkipCnt);
	 	}
	}
	else if (stParam.PARAM.nDefectCount[nCnt] == 0 || elapsedTime.GetMinutes() > (stParam.dHour[nCnt]))
	{
	 	DefInfoTmp.init_DefType(nDefSkipCnt);
	}
	return strMsg;
}

int CDataManager::m_fnCompareOverlapDefTypeCheck(CString strPanelId, CString strFilePath, STRU_DEFECT_TYPE& stDefType)
{
	G_AddLog(3, 3, _T("m_fnCompareOverlapDefTypeCheck() - Start"));
	int nRet = APP_OK;

	int nTypeJump = 0;		//PROTECTION 검사번호 변경으로 이 후 추가 번호들에 대한 예외처리
	size_t read_size = 0;
	errno_t errFileOpen;		
	int nCnt = 0;
	int nCnt_bubble = 0;	//2020.02.28 add by mby
	CString strTmpPath;
	CString strPanelIdTemp;
	strTmpPath.Format(_T("%s%s%s"), DATA_PATH, RESULT_PATH, _T("DATA_TMP.txt"));
	int nOrderCnt = 0;
	CopyFile(strFilePath, strTmpPath, 0);

	FILE *in = NULL, *out = NULL;

	int nRetryFileOpenCnt = 0, nOverlapCnt = 0;
	CString strDefTypePath, strTemp, strMsg ,strDefTypeName;

	DefInfoTmp.init();

	STRU_DEFECT_COORD_INFO *stOverlapCompDef = new STRU_DEFECT_COORD_INFO();
	m_fnGetOverlapDefectData((STRU_DEFECT_COORD_INFO *) stOverlapCompDef, strTmpPath);
	STRU_OVERLAP_DEFTYPE_PARAM stParam;
	stParam.init();
	BOOL bRet= m_fnGetOverlapDefTypeInfo(stParam);
	if (stParam.PARAM.bUseMode != OK)
	{
		delete stOverlapCompDef;
		return nRet = APP_OK;

	}

	strDefTypePath.Format(_T("%s%s"), ROOT_PATH , OVERLAP_DEF_TYPE_PATH);
	while(nRetryFileOpenCnt < 3)
	{
		errFileOpen = _tfopen_s(&in, strDefTypePath , _T("rb+")); // 구조체 바이너리 파일을 읽음

		if(errFileOpen == 0)
		{		
			read_size = fread(&DefInfoTmp, sizeof(DefInfoTmp), 1, in);			
			fclose(in);
			G_AddLog(2, 3, _T("Overlap DefType Read Success : DefInfoTmp"));	//2016.01.07 CHECK QMS COUNT LOG
			break;
		}
		else
		{
			G_AddLog(2, 3, _T("Overlap DefType Read Success : DefInfoTmp"));	//2016.01.07 CHECK QMS COUNT LOG
		}
		nRetryFileOpenCnt++;
	}

	//2020.03.05 add by mby S-
	if(m_bUseAreaCheck){
		if (stDefType.Bgradecheck)								m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_B_GRADE_CHECK, strPanelId);	//2019.03.15
		if (stDefType.Cgradecheck)								m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_C_GRADE_CHECK, strPanelId);	//2019.03.15
	}
	else{
		if (stDefType.DustType)  								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_DUST, strPanelId);
		if (stDefType.CfScrach || stDefType.TftScrach)			 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BLACK_SCRATCH, strPanelId);
		if (stDefType.ItoPeel) 									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_ITO, strPanelId);
		if (stDefType.CfDimplePoint || stDefType.TftDimplePoint) m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_DIMPLE_POINT, strPanelId);
		if (stDefType.CfDimpleLine || stDefType.TftDimpleLine) 	 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_DIMPLE_LINE, strPanelId);
		if (stDefType.CfEdgeilegal || stDefType.TftEdgeilegal) 	 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_ETCHING_LINE, strPanelId);
		if (stDefType.CfWhiteBlur || stDefType.TftWhiteBlur) 	 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_WHITE_TURBIDITY, strPanelId);
		if (stDefType.SocketBurn)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_PAD_BURNT, strPanelId);
		if (stDefType.WanesBroken) 								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_EDGE_BROKEN, strPanelId);
		if (stDefType.SocketScrach) 							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_PAD_SCRATCH, strPanelId);
		if (stDefType.CfMeasureFail) 							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_MEASURE, strPanelId);
		if (stDefType.TftMeasureFail) 							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_MEASURE, strPanelId);
		if (stDefType.Burr )									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BURR, strPanelId);	
		if (stDefType.CfPolBubble || stDefType.TftPolBubble) 	 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_BUBBLE, strPanelId);	
		if (stDefType.CfPolScrach) 								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_SCRATCH, strPanelId);	
		if (stDefType.mura )									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_MURA, strPanelId);
		if (stDefType.TFTpolSubstance)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_ALIEN_SUBSTANCE+1, strPanelId);	
		if (stDefType.CFpolSubstance)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_ALIEN_SUBSTANCE, strPanelId);	
		//if (stDefType.PolMinor)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_MINOR, strPanelId);
		if (stDefType.CfBurr)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_BURR, strPanelId);
		if (stDefType.TftBurr)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_BURR, strPanelId);
		if(m_bPaOverlapAlarmFlag[1]){					//2021.01.13 add by mby for Edge,Corner Broken			
			if (stDefType.CfBroken)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_BROKEN, strPanelId);
		}
		if(m_bPaOverlapAlarmFlag[2]){					//2021.01.13 add by mby for Edge,Corner Broken			
			if (stDefType.TftBroken)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_BROKEN, strPanelId);
		}
		if(m_bPaOverlapAlarmFlag[3]){					//2021.01.13 add by mby for Edge,Corner Broken	
			if (stDefType.CfCornerBroken)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_CORNER_BROKEN, strPanelId);
		}
		if(m_bPaOverlapAlarmFlag[4]){					//2021.01.13 add by mby for Edge,Corner Broken	
			if (stDefType.TftCornerBroken)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_CORNER_BROKEN, strPanelId);
		}
		
		if (stDefType.CfCellBroken)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_CELL_BROKEN, strPanelId);
		if (stDefType.TftCellBroken)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_CELL_BROKEN, strPanelId);	
		if (m_strSemaGrade == _T("ITO_NG") || m_strSemaGrade == _T("CONTACT_NG"))						 
			m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CONTACT, strPanelId);
		if (stDefType.CfPimplePoint || stDefType.TftPimplePoint) m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_PIMPLE_POINT, strPanelId);
		if (stDefType.CfPimpleLine || stDefType.TftPimpleLine)	 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_PIMPLE_LINE, strPanelId);
		if (stDefType.CfPimpleCircle)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_PIMPLE_CIRCLE, strPanelId);
		if (stDefType.CfWavePattern)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_WAVE_PATTERN, strPanelId);
		if (stDefType.EtcLine)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_ETC_LINE, strPanelId);
		if (stDefType.EtcPoint)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_ETC_POINT, strPanelId);
		if (stDefType.PolMiss)									 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_POL_MISS, strPanelId);	//2016.02.24
		if (stDefType.AlignFail)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_ALIGN_FAIL, strPanelId);	//2016.02.24
		if (stDefType.CFCLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CFC_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.CBCLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CBC_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.CFSLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CFS_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.CBSLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CBS_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.TFCLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFC_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.TBCLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TBC_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.TFSLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFS_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.TBSLuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TBS_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.TDILuminance)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TDI_LUMINANCE, strPanelId);	//2016.02.24
		if (stDefType.CfProtection)								 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_CF_PROTECTION, strPanelId);	//2016.02.24
		if (stDefType.TftProtection)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_TFT_PROTECTION, strPanelId);	//2016.02.24	

		if (stDefType.BubbleStation1)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION1, strPanelId);	// //2020.02.28 add by mby	
		if (stDefType.BubbleStation2)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION2, strPanelId);	//	  .
		if (stDefType.BubbleStation3)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION3, strPanelId);	//	  .
		if (stDefType.BubbleStation4)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION4, strPanelId);	//	  .
		if (stDefType.BubbleStation5)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION5, strPanelId);	//	  .
		if (stDefType.BubbleStation6)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION6, strPanelId);	//	  .
		if (stDefType.BubbleStation7)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION7, strPanelId);	//	  .
		if (stDefType.BubbleStation8)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION8, strPanelId);	//	  .
		if (stDefType.BubbleStation9)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION9, strPanelId);	//	  .
		if (stDefType.BubbleStation10)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION10, strPanelId);	//	  .
		if (stDefType.BubbleStation11)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION11, strPanelId);	//	  .
		if (stDefType.BubbleStation12)							 m_fnSetDefTypeCntAndStartDate(stOverlapCompDef,DefInfoTmp, stParam,INSP_BUBBLB_STATION12, strPanelId);	// //2020.02.28 add by mby
	}
	//2020.03.05 add by mby E-

	CTime tTime;

	CTimeSpan elapsedTime;
	tTime = CTime::GetCurrentTime();


	int nStopFlag = FALSE;
	//2020.03.05 add by mby S-
	if(!m_bUseAreaCheck){
		while(nCnt < MAX_DEFECT_KIND)
		{
			strMsg += m_fnCompareOverlapDefTypeResult(nOverlapCnt, tTime, strPanelId, stParam, nCnt, nTypeJump);
			///////////////////
	//  			elapsedTime = tTime - DefInfoTmp.StartDate[nCnt];
	//  			if (DefInfoTmp.StartDate[nCnt] == 0) elapsedTime = 0;
	//  
	//  			for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[nCnt][i]).IsEmpty()) {nOrderCnt = i; break;}
	//  			strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[nCnt][nOrderCnt -1]);
	//  
	//  			if (stParam.PARAM.nDefectCount[nCnt] != 0 
	//  				&& stParam.PARAM.nDefectCheck[nCnt] && //현재 디펙 오버랩을 사용하고 있고,
	//  				( stParam.PARAM.nDefectCount[nCnt] <= DefInfoTmp.DefTypeCnt[nCnt] || // 오버랩 stop으로 설정한 갯수보다 발생한 갯수가 많거나
	//  				(stParam.nAlarmCount[nCnt] <= DefInfoTmp.DefTypeCnt[nCnt] && strPanelId == strPanelIdTemp) ) // 알람으로 설정한 갯수보다 발생한 갯수가 많고 현재 발생한 디펙일때
	//  				&& (elapsedTime.GetMinutes() <= (stParam.dHour[nCnt]) || stParam.dHour[nCnt] == 0))
	//  			{
	//  				/* 알람을 출력할 데이터를 생성한다 */
	//  				int nDefCnt = 0;
	//  				while(nDefCnt < MAX_DEFECT_CNT)
	//  				{
	//  					//if (stOverlapCompDef->stPnlInfo[nDefCnt].nDefectType == nCnt)
	//  					if( nCnt == INSP_ALIGN_FAIL && nCnt == INSP_CONTACT || nCnt == INSP_TFT_MEASURE || nCnt == INSP_POL_MISS || nCnt == INSP_CF_MEASURE) break;
	//  
	//  					if (!((CString)DefInfoTmp.cPanelId[nCnt][nDefCnt]).IsEmpty())
	//  					{
	//  
	//  						if(stParam.PARAM.nDefectCount[nCnt] <= DefInfoTmp.DefTypeCnt[nCnt])
	//  							DefInfoTmp.bStopFlag[nCnt] = TRUE;
	//  
	//  						strDefTypeName = m_fnGetOverlapDefectType(DefInfoTmp.DefTypeCode[nCnt], DefInfoTmp.DefTypeFlag[nCnt][nDefCnt]);
	//  						strTemp.Format(_T("%s,%s,%d,%0.1f,%0.3f,%0.3f,%0.3f,%0.3f,%s,%d;"),
	//  							DefInfoTmp.cPanelId[nCnt][nDefCnt], strDefTypeName, DefInfoTmp.DefTypeCnt[nCnt], stParam.dHour[nCnt],
	//  							DefInfoTmp.startX[nCnt][nDefCnt], DefInfoTmp.startY[nCnt][nDefCnt], DefInfoTmp.endX[nCnt][nDefCnt], DefInfoTmp.endY[nCnt][nDefCnt], 
	//  							DefInfoTmp.cImgPath[nCnt][nDefCnt], DefInfoTmp.bStopFlag[nCnt]);
	//  
	//  						//G_AddLog(2, 3, _T("Overlap Defect : %d"), strLogOverlapDefect, GetLastError());
	//  						//_tcscpy(DefInfoTmp.cImgPath  , stOverlapCompDef->stPnlInfo[nDefCnt].strImage);						
	//  						strMsg += strTemp;
	//  						nOverlapCnt++;
	//  						//stOverlapCompDef->stPnlInfo[nDefCnt].init_info();
	//  					}
	//  
	//  					nDefCnt++;
	//  				}	
	//  				if( (  nCnt != INSP_ALIGN_FAIL && nCnt != INSP_CONTACT && nCnt != INSP_TFT_MEASURE && nCnt != INSP_POL_MISS && nCnt != INSP_CF_MEASURE) && DefInfoTmp.bStopFlag[nCnt] ){
	//  					DefInfoTmp.init_DefType(nCnt);
	//  				}
	//  			}
	//  			else if (stParam.PARAM.nDefectCount[nCnt] == 0 || elapsedTime.GetMinutes() > (stParam.dHour[nCnt]))
	//  			{
	//  				DefInfoTmp.init_DefType(nCnt);
	//  			}
			nCnt++;
			if( nCnt == INSP_CONTACT )		//PROTECTION 검사번호 변경으로 이 후 추가 번호들에 대한 예외처리
				nTypeJump = INSP_CF_PROTECTION - INSP_CONTACT;
		}

		///////////////////////////////////////////////////////////////////////////
		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_CONTACT];

		if (/*DefInfoTmp.StartDate[nCnt]!=0 && */stParam.PARAM.nDefectCount[INSP_CONTACT] != 0 
			&& stParam.PARAM.bUseContactFail && stParam.PARAM.nContactFailDefectCount <= DefInfoTmp.DefTypeCnt[INSP_CONTACT]
		&& (elapsedTime.GetMinutes() <= (stParam.dHour[INSP_CONTACT]) || stParam.dHour[INSP_CONTACT] == 0))//abs(llNow - DefInfoTmp.StartDate[nCnt])	<= (stParam.dContactHour * 60))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				//if (stOverlapCompDef->stPnlInfo[nDefCnt].nDefectType == nCnt)
				if (!((CString)DefInfoTmp.cPanelId[INSP_CONTACT][nDefCnt]).IsEmpty())
				{
					/* 알람을 출력할 데이터를 생성한다 */
					strDefTypeName = m_fnGetDefTypeName(INSP_CONTACT);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE;"),
						DefInfoTmp.cPanelId[INSP_CONTACT], strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_CONTACT], stParam.dHour[INSP_CONTACT]);
					strMsg += strTemp;
					nOverlapCnt++;
				}
				nDefCnt++;
			}
			DefInfoTmp.init_DefType(INSP_CONTACT);
		}
		else if (stParam.PARAM.nContactFailDefectCount == 0 || elapsedTime.GetMinutes()	> (stParam.dHour[INSP_CONTACT]))
			//(abs(llNow - DefInfoTmp.StartDate[INSP_CONTACT]) > (stParam.dContactHour * 60))
		{
			DefInfoTmp.init_DefType(INSP_CONTACT);
		}

		//2016.02.24
		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_POL_MISS];

		if (DefInfoTmp.StartDate[INSP_POL_MISS] == 0) elapsedTime = 0;

		if (stParam.PARAM.nPolMissStopCount != 0 
			&& stParam.PARAM.bUsePolMiss && //현재 디펙 오버랩을 사용하고 있고,
			(stParam.PARAM.nPolMissStopCount <= DefInfoTmp.DefTypeCnt[INSP_POL_MISS] ||// 오버랩 stop으로 설정한 갯수보다 발생한 갯수가 많거나
			(stParam.PARAM.nPolMissDefectCount <= DefInfoTmp.DefTypeCnt[INSP_POL_MISS]) ) 
			&& (elapsedTime.GetMinutes() <= stParam.dPolMissHour || stParam.dPolMissHour == 0))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				if (!((CString)DefInfoTmp.cPanelId[INSP_POL_MISS][nDefCnt]).IsEmpty())
				{
					if(stParam.PARAM.nPolMissStopCount <= DefInfoTmp.DefTypeCnt[INSP_POL_MISS])
						DefInfoTmp.bStopFlag[INSP_POL_MISS] = TRUE;

					/* 알람을 출력할 데이터를 생성한다 */
					strDefTypeName = m_fnGetDefTypeName(INSP_POL_MISS);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),
						DefInfoTmp.cPanelId[INSP_POL_MISS], strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_POL_MISS], stParam.dPolMissHour, DefInfoTmp.bStopFlag[INSP_POL_MISS]);
					strMsg += strTemp;
					nOverlapCnt++;
				}
				nDefCnt++;
			}
			DefInfoTmp.init_DefType(INSP_POL_MISS);
		}
		else if (stParam.PARAM.nPolMissDefectCount == 0 || elapsedTime.GetMinutes()	> (stParam.dHour[INSP_POL_MISS]))
		{
			DefInfoTmp.init_DefType(INSP_POL_MISS);
		}

		////////////////////////
		//2016.08.22	Mr.Chu
  		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_ALIGN_FAIL];
  
  		if (stDefType.AlignFail !=0 
  			&& stParam.PARAM.bUseAlignFail && stParam.PARAM.nAlignFaliDefectCount <= DefInfoTmp.DefTypeCnt[INSP_ALIGN_FAIL]
  			&& (elapsedTime.GetMinutes() <= stParam.dAlignFaliHour || stParam.dAlignFaliHour == 0))
  		{
  			int nDefCnt = 0;
  			while(nDefCnt < MAX_DEFECT_CNT)
  			{
  				if (!((CString)DefInfoTmp.cPanelId[INSP_ALIGN_FAIL][nDefCnt]).IsEmpty())
  				{
  					/* 알람을 출력할 데이터를 생성한다 */
  					strDefTypeName = m_fnGetDefTypeName(INSP_ALIGN_FAIL);
  					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),
  						DefInfoTmp.cPanelId[INSP_ALIGN_FAIL], strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_ALIGN_FAIL], stParam.dAlignFaliHour);
					strMsg += strTemp;
  					nOverlapCnt++;
  				}
  				nDefCnt++;
  			}
  			DefInfoTmp.init_DefType(INSP_ALIGN_FAIL);
  		}
  		else if (stParam.PARAM.nAlignFaliDefectCount == 0 || elapsedTime.GetMinutes() > stParam.dAlignFaliHour)
  		{
  			DefInfoTmp.init_DefType(INSP_ALIGN_FAIL);
  		}
		//////	
		////////////////////// tft measure 16.06.25 sgkim
		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_TFT_MEASURE];
		if (DefInfoTmp.StartDate[INSP_TFT_MEASURE] == 0) elapsedTime = 0;
		for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[INSP_TFT_MEASURE][i]).IsEmpty()) {nOrderCnt = i; break;}
		strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[INSP_TFT_MEASURE][nOrderCnt -1]);

		if (stParam.PARAM.nDefectCount[INSP_TFT_MEASURE] != 0 && 
			stParam.PARAM.nDefectCheck[INSP_TFT_MEASURE] && 
			(stParam.PARAM.nDefectCount[INSP_TFT_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_TFT_MEASURE] || 
			( stParam.nAlarmCount[INSP_TFT_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_TFT_MEASURE] && strPanelId == strPanelIdTemp) )
			&& (elapsedTime.GetMinutes() <= stParam.dHour[INSP_TFT_MEASURE]))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				if (!((CString)DefInfoTmp.cPanelId[INSP_TFT_MEASURE][nDefCnt]).IsEmpty())
				{
					/* 알람을 출력할 데이터를 생성한다 */
					if(stParam.PARAM.nDefectCount[INSP_TFT_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_TFT_MEASURE])
						DefInfoTmp.bStopFlag[INSP_TFT_MEASURE] = TRUE;

					strDefTypeName = m_fnGetDefTypeName(INSP_TFT_MEASURE);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),
						DefInfoTmp.cPanelId[INSP_TFT_MEASURE], strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_TFT_MEASURE], stParam.dHour[INSP_TFT_MEASURE],DefInfoTmp.bStopFlag[INSP_TFT_MEASURE]);
					strMsg += strTemp;
					//G_AddLog(3, 2,_T("%s"),strMsg);
					nOverlapCnt++;
				}
				nDefCnt++;
			}
			if(DefInfoTmp.bStopFlag[INSP_TFT_MEASURE])
				DefInfoTmp.init_DefType(INSP_TFT_MEASURE);
		}
		else if (stParam.PARAM.nDefectCheck[INSP_TFT_MEASURE] == 0 || elapsedTime.GetMinutes()	> (stParam.dHour[INSP_TFT_MEASURE]))
		{
			DefInfoTmp.init_DefType(INSP_TFT_MEASURE);
		}

		////////////////////////////
		for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[INSP_CF_MEASURE][i]).IsEmpty()) {nOrderCnt = i; break;}
		strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[INSP_CF_MEASURE][nOrderCnt -1]);
		//////////////////////cf measure 16.06.25 sgkim
		if (stParam.PARAM.nDefectCount[INSP_CF_MEASURE] != 0 
			&&stParam.PARAM.nDefectCheck[INSP_CF_MEASURE] &&
			(stParam.PARAM.nDefectCount[INSP_CF_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_CF_MEASURE] || 
			( stParam.nAlarmCount[INSP_CF_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_CF_MEASURE] && strPanelId == strPanelIdTemp) )
			&& (elapsedTime.GetMinutes() <= (stParam.dHour[INSP_CF_MEASURE]) || stParam.dHour[INSP_CF_MEASURE] == 0))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				if (!((CString)DefInfoTmp.cPanelId[INSP_CF_MEASURE][nDefCnt]).IsEmpty())
				{

					if(stParam.PARAM.nDefectCount[INSP_CF_MEASURE] <= DefInfoTmp.DefTypeCnt[INSP_CF_MEASURE])
						DefInfoTmp.bStopFlag[INSP_CF_MEASURE] = TRUE;

					strDefTypeName = m_fnGetDefTypeName(INSP_CF_MEASURE);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),
						DefInfoTmp.cPanelId[INSP_CF_MEASURE], strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_CF_MEASURE], stParam.dHour[INSP_CF_MEASURE],DefInfoTmp.bStopFlag[INSP_CF_MEASURE]);
					//strMsg += strTemp;
					strMsg.Append(strTemp);
					//G_AddLog(3, 2,_T("%s"),strMsg);

					nOverlapCnt++;
				}
				nDefCnt++;
			}
			if(DefInfoTmp.bStopFlag[INSP_CF_MEASURE])
				DefInfoTmp.init_DefType(INSP_CF_MEASURE);

		}
		else if (stParam.PARAM.nDefectCheck[INSP_CF_MEASURE] == 0 || elapsedTime.GetMinutes()	> (stParam.dHour[INSP_CF_MEASURE]))
		{
			DefInfoTmp.init_DefType(INSP_CF_MEASURE);
		}

		//////////2016.11.21 Mr.Chu
		nCnt = 0;
		int nINSP_OFFSET = 0;
		while(nCnt < LUMINANCE_TYPE_MAX)
		{
			nINSP_OFFSET = INSP_CFC_LUMINANCE + nCnt;

			elapsedTime = tTime - DefInfoTmp.StartDate[nINSP_OFFSET];

			//Type이 분명한 경우 마지막 Panel ID 데이터에 현재 Panel ID가 Setting이 되기 때문에 stDefType.CFCLuminance로 조건문을 걸어도 되지만
			//이 경우는 반복문으로써 Type이 바뀌기 때문에 해당 Panel ID가 Setting이 되었는지 확인하여 Option이 활성/비활성 인지 알아낸다.
			for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[nINSP_OFFSET][i]).IsEmpty()) {nOrderCnt = i; break;}
			strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[nINSP_OFFSET][nOrderCnt -1]);

			if ( strPanelId == strPanelIdTemp
				&& stParam.PARAM.bArrLuminanceChk[nCnt] && stParam.PARAM.nArrLuminanceCnt[nCnt] <= DefInfoTmp.DefTypeCnt[nINSP_OFFSET] 
			&& (elapsedTime.GetMinutes() <= stParam.dLumiHour[nCnt] || stParam.dLumiHour[nCnt] == 0))
			{
				int nDefCnt = 0;
				while(nDefCnt < MAX_DEFECT_CNT)
				{
					if (!((CString)DefInfoTmp.cPanelId[nINSP_OFFSET][nDefCnt]).IsEmpty())
					{
						/* 알람을 출력할 데이터를 생성한다 */
						strDefTypeName = m_fnGetDefTypeName(nINSP_OFFSET);
						strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),
							DefInfoTmp.cPanelId[nINSP_OFFSET], strDefTypeName, DefInfoTmp.DefTypeCnt[nINSP_OFFSET], stParam.dAlignFaliHour);
						//strMsg += strTemp;
						strMsg.Append(strTemp);
						nOverlapCnt++;
					}
					nDefCnt++;
				}
				DefInfoTmp.init_DefType(nINSP_OFFSET);
			}
			else if (stParam.PARAM.nArrLuminanceCnt[nCnt] == 0 || elapsedTime.GetMinutes() > stParam.dLumiHour[nCnt])
			{
				DefInfoTmp.init_DefType(nINSP_OFFSET);
			}

			nCnt++;
		}
		//////////////////////////////////////////////////////////////////////////
		//2020.03.03 add by mby
		nCnt_bubble = 0;
		int nINSP_OFFSET_Bubble = 0;
		CString strPnlTemp = _T(""),strTempType = _T("");//2019.09.09 ADD BY MBY

		while(nCnt_bubble < BUBBLESTATION_MAX)
		{
			nINSP_OFFSET_Bubble = INSP_BUBBLB_STATION1 + nCnt_bubble;

			elapsedTime = tTime - DefInfoTmp.StartDate[nINSP_OFFSET_Bubble];
			for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)DefInfoTmp.cPanelId[nINSP_OFFSET_Bubble][i]).IsEmpty()) {nOrderCnt = i; break;}
			strPanelIdTemp.Format(_T("%s"),DefInfoTmp.cPanelId[nINSP_OFFSET_Bubble][nOrderCnt -1]);
			if ( strPanelId == strPanelIdTemp &&  //o
					stParam.PARAM.bArrBubbleStationChk[nCnt_bubble] &&  //o
					stParam.nBubbleStationAlarmCount[nCnt_bubble] <= DefInfoTmp.DefTypeCnt[nINSP_OFFSET_Bubble] &&  // ???
					(elapsedTime.GetMinutes() <= (stParam.dBubbleStationHour[nCnt_bubble]) || stParam.dBubbleStationHour[nCnt_bubble] ==0))   //시간 조건 확인 1.	dBubbleStationHour	
			{
				int nDefCnt = 0;
				while(nDefCnt < MAX_DEFECT_CNT)
				{
					if(!((CString)DefInfoTmp.cPanelId[nINSP_OFFSET_Bubble][nDefCnt]).IsEmpty())
					{
						strDefTypeName = m_fnGetDefTypeName(nINSP_OFFSET_Bubble);
						if(stParam.PARAM.nArrBubbleStationCnt[nCnt_bubble] <= DefInfoTmp.DefTypeCnt[nINSP_OFFSET_Bubble]){   //중알람 조건 확인 2.		
							DefInfoTmp.bStopFlag[nINSP_OFFSET_Bubble] = TRUE;
							/*if((strPnlTemp != strPanelId) && (strDefTypeName != strTempType)){
								strPnlTemp = strPanelId;
								strTempType = strDefTypeName;
								m_fnSaveFileHeavyOverLapInfo(strPanelId, strDefTypeName,strTempTime);
							}	*/					
						}
						/* 알람을 출력할 데이터를 생성한다 */					
						strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),						
							strPanelId, strDefTypeName, DefInfoTmp.DefTypeCnt[nINSP_OFFSET_Bubble], stParam.dBubbleStationHour[nCnt_bubble], DefInfoTmp.bStopFlag[nINSP_OFFSET_Bubble]); 					
						strMsg.Append(strTemp);
						nOverlapCnt++;
					}
					nDefCnt++;
				}
				if(DefInfoTmp.bStopFlag[nINSP_OFFSET_Bubble])
				DefInfoTmp.init_DefType(nINSP_OFFSET_Bubble);    //초기화 조건 확인 3.		
			}		
			else if (stParam.PARAM.nArrBubbleStationCnt[nCnt_bubble] == 0 || elapsedTime.GetMinutes() > stParam.dBubbleStationHour[nCnt_bubble])
			{
				DefInfoTmp.init_DefType(nINSP_OFFSET_Bubble);
			}
			nCnt_bubble++;
		}
		//2020.02.28 add by mby		
	}
	else{
		////////////////////////
		//2019.03.20	BY. ryu B
		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_B_GRADE_CHECK];

		if (stParam.PARAM.nBgradecntStopCount !=0 
			&& stParam.PARAM.bUseBgradecnt &&  //현재 defect overlab를 사용하고 있고,
			(stParam.PARAM.nBgradecntStopCount <= DefInfoTmp.DefTypeCnt[INSP_B_GRADE_CHECK] ||  //over stop으로 설정한 갯수보다 발생한 갯수가 많거나,
			(stParam.PARAM.nBgradecntDefectCount <= DefInfoTmp.DefTypeCnt[INSP_B_GRADE_CHECK]))
		&& (elapsedTime.GetMinutes() <= stParam.dGradecountHour[BGRADECHECK_] || stParam.dGradecountHour[BGRADECHECK_] == 0))
		//	&& stParam.PARAM.nDefectCheck[36] && stParam.PARAM.nDefectCount[36]<= DefInfoTmp.DefTypeCnt[INSP_B_GRADE_CHECK]
		//&& (elapsedTime.GetMinutes() <= stParam.dHour[36] /*dGradecountHour[BGRADECHECK_]*/ || stParam.dHour[36] /*dGradecountHour[BGRADECHECK_]*/  == 0))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				if (!((CString)DefInfoTmp.cPanelId[INSP_B_GRADE_CHECK][nDefCnt]).IsEmpty())
				{
					if(stParam.PARAM.nBgradecntStopCount <= DefInfoTmp.DefTypeCnt[INSP_B_GRADE_CHECK] )
						DefInfoTmp.bStopFlag[INSP_B_GRADE_CHECK] = TRUE;

					/* 알람을 출력할 데이터를 생성한다 */
					strDefTypeName = m_fnGetDefTypeName(INSP_B_GRADE_CHECK);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"), 
					strPanelId/*DefInfoTmp.cPanelId[INSP_B_GRADE_CHECK]*/, strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_B_GRADE_CHECK], stParam.dGradecountHour[BGRADECHECK_], DefInfoTmp.bStopFlag[INSP_B_GRADE_CHECK]);				 
					strMsg += strTemp;
					nOverlapCnt++;
				}
				nDefCnt++;
			}
			if(DefInfoTmp.bStopFlag[INSP_B_GRADE_CHECK] == TRUE) DefInfoTmp.init_DefType(INSP_B_GRADE_CHECK);
		}
		else if (stParam.PARAM.nBgradecntDefectCount == 0 || elapsedTime.GetMinutes() > stParam.dGradecountHour[BGRADECHECK_])
		{
			DefInfoTmp.init_DefType(INSP_B_GRADE_CHECK);
		}
		//////
		////////////////////////
		//2019.03.20	BY. ryu C
		elapsedTime = tTime - DefInfoTmp.StartDate[INSP_C_GRADE_CHECK];

		if (stParam.PARAM.nCgradecntStopCount !=0 
			&& stParam.PARAM.bUseCgradecnt && //현재 defect overlab를 사용하고 있고,
			(stParam.PARAM.nCgradecntStopCount <= DefInfoTmp.DefTypeCnt[INSP_C_GRADE_CHECK] ||  //over stop으로 설정한 갯수보다 발생한 갯수가 많거나,
			(stParam.PARAM.nCgradecntDefectCount <= DefInfoTmp.DefTypeCnt[INSP_C_GRADE_CHECK]) )
		&& (elapsedTime.GetMinutes() <= stParam.dGradecountHour[CGRADECHECK_] || stParam.dGradecountHour[CGRADECHECK_] == 0))
		//&& stParam.PARAM.nDefectCheck[37] && stParam.PARAM.nDefectCount[37]<= DefInfoTmp.DefTypeCnt[INSP_C_GRADE_CHECK]
		//&& (elapsedTime.GetMinutes() <= stParam.dHour[37] /*dGradecountHour[BGRADECHECK_]*/ || stParam.dHour[37] /*dGradecountHour[BGRADECHECK_]*/  == 0))
		{
			int nDefCnt = 0;
			while(nDefCnt < MAX_DEFECT_CNT)
			{
				if (!((CString)DefInfoTmp.cPanelId[INSP_C_GRADE_CHECK][nDefCnt]).IsEmpty())
				{
					if(stParam.PARAM.nCgradecntStopCount <= DefInfoTmp.DefTypeCnt[INSP_C_GRADE_CHECK] )
						DefInfoTmp.bStopFlag[INSP_C_GRADE_CHECK] = TRUE;

					/* 알람을 출력할 데이터를 생성한다 */
					strDefTypeName = m_fnGetDefTypeName(INSP_C_GRADE_CHECK);
					strTemp.Format(_T("%s,%s,%d,%0.1f,-,-,-,-,NO IMAGE,%d;"),				
						strPanelId/*DefInfoTmp.cPanelId[INSP_C_GRADE_CHECK]*/, strDefTypeName, DefInfoTmp.DefTypeCnt[INSP_C_GRADE_CHECK], stParam.dGradecountHour[CGRADECHECK_], DefInfoTmp.bStopFlag[INSP_C_GRADE_CHECK]);
					strMsg += strTemp;
					nOverlapCnt++;
				}
				nDefCnt++;
			}
			if(DefInfoTmp.bStopFlag[INSP_C_GRADE_CHECK] == TRUE)DefInfoTmp.init_DefType(INSP_C_GRADE_CHECK);
		}
		else if (stParam.PARAM.nCgradecntDefectCount == 0 || elapsedTime.GetMinutes() > stParam.dGradecountHour[CGRADECHECK_])
		{
			DefInfoTmp.init_DefType(INSP_C_GRADE_CHECK);
		}
		//////
	}
	//2020.03.05 add by mby E-
	if (nOverlapCnt > 0)
	{
		//	알람을 발생하고 설비를 멈춘다	
		///////////////////////////////////////////////////////////////////////////////////
		CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
		CString strSendMsg = _T("");
		strSendMsg.Append(strMsg);
		strSendMsg.Append(_T("#"));

		va_list vaList;
		va_start(vaList, strSendMsg);
		int len = _vscwprintf( strSendMsg, vaList ) + 1;
		wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
		wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
		memset(cBuffer,0,sizeof(wchar_t)*len);
		memset(cTempBuff,0,sizeof(wchar_t)*len);

		if (cBuffer)
		{
			wcscpy((WCHAR*)cBuffer, strSendMsg);
			memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);									
			pView->SendMessage(WM_UI_NEW_DEF_TYPE_MSG_DIALOG, (WPARAM)nOverlapCnt, (LPARAM)cTempBuff ); 			 		
			delete[] cBuffer;
		}		
		//////////////////////////////////////////////////////////////////////////////////////
	}

	read_size = 0;	
	errFileOpen = _tfopen_s(&out, strDefTypePath, _T("wb"));
	if(errFileOpen == 0)
	{
		fwrite(&DefInfoTmp, sizeof(DefInfoTmp) , 1, out);		
		fclose(out);
	}
	DeleteFile(strTmpPath);
	delete stOverlapCompDef;
	G_AddLog(3, 3, _T("m_fnCompareOverlapDefTypeCheck() - End"));
	return nRet;
}
BOOL CDataManager::m_fnSetDefTypeCntAndStartDate(
	STRU_DEFECT_COORD_INFO *&stOverlapCompDef,
	STRU_OVERLAP_DEFTYPE_INFO& nDefType, 
	STRU_OVERLAP_DEFTYPE_PARAM &stParam, 
	int nInspType, 
	CString strPanelId
	)
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int nTypeJump = 0;

	//stParam 구조체의 AlignFail, Pol mark 등 사이 정보를 건너뛴다     //2017.08.04
	//if( nInspType >= INSP_TFT_PROTECTION )		
	//	nTypeJump = INSP_ETC_POINT + (nInspType - INSP_TDI_LUMINANCE);
	//else
	//	nTypeJump = nInspType;
	//2020.02.28 add by mby
	if(nInspType >= INSP_BUBBLB_STATION1)
		nTypeJump = INSP_POL_BUBBLE;
	else if( nInspType >= INSP_TFT_PROTECTION )		
		nTypeJump = INSP_ETC_POINT + (nInspType - INSP_TDI_LUMINANCE);
	else
		nTypeJump = nInspType;
	//

	G_AddLog(3, 3, _T("m_fnSetDefTypeCntAndStartDate() - Start"));
	if ( (stParam.PARAM.nDefectCount[nTypeJump] != 0 && stParam.PARAM.nDefectCheck[nTypeJump]) 
		|| (nInspType == INSP_ALIGN_FAIL) && stParam.PARAM.bUseAlignFail 
		|| (nInspType == INSP_POL_MISS) && stParam.PARAM.bUsePolMiss
		|| (nInspType == INSP_B_GRADE_CHECK) && stParam.PARAM.bUseBgradecnt
		|| (nInspType == INSP_C_GRADE_CHECK) && stParam.PARAM.bUseCgradecnt
		|| (nInspType >= INSP_BUBBLB_STATION1))   //2019.03.26 by ryu
	{
		int nDefCnt , nOrderCnt = 0;

		for (int i = 0 ;i < MAX_DEFECT_CNT; i ++)	if(((CString)nDefType.cPanelId[nInspType][i]).IsEmpty()) {nOrderCnt = i; break;}

		if (nInspType == INSP_CONTACT)
		{
			//if (stParam.PARAM.nDefectCount[INSP_CONTACT] != 0 && stParam.PARAM.nDefectCheck[INSP_CONTACT])
			if (stParam.PARAM.bUseContactFail != 0 && stParam.PARAM.nContactFailDefectCount)
			{
				if (m_strSemaGrade.Find(_T("ITO_NG")) != -1 || m_strSemaGrade.Find(_T("CONTACT_NG")) != -1)
				{
					_tcscpy(nDefType.cPanelId[INSP_CONTACT][nOrderCnt] , strPanelId);	
				}
			}
		}
		else if (nInspType == INSP_ALIGN_FAIL)
		{
			//if (stParam.PARAM.nDefectCount[INSP_ALIGN_FAIL] != 0 && stParam.PARAM.nDefectCheck[INSP_ALIGN_FAIL])
			if ( stParam.PARAM.nAlignFaliDefectCount != 0 && stParam.PARAM.bUseAlignFail )
			{
				_tcscpy(nDefType.cPanelId[INSP_ALIGN_FAIL][nOrderCnt] , strPanelId);	
			}
		}
		//2019.03.21 BY RYU START
		else if (nInspType == INSP_B_GRADE_CHECK)
		{			
			if ( stParam.PARAM.nBgradecntDefectCount != 0 && stParam.PARAM.bUseBgradecnt )
			{
				_tcscpy(nDefType.cPanelId[INSP_B_GRADE_CHECK][nOrderCnt] , strPanelId);	
			}
		}
		else if (nInspType == INSP_C_GRADE_CHECK)
		{		
			if ( stParam.PARAM.nCgradecntDefectCount != 0 && stParam.PARAM.bUseCgradecnt )
			{
				_tcscpy(nDefType.cPanelId[INSP_C_GRADE_CHECK][nOrderCnt] , strPanelId);	
			}
		}
		//2019.03.21 --->BY RYU END
		else if (nInspType == INSP_POL_MISS)
		{
			//if (stParam.PARAM.nDefectCount[INSP_POL_MISS] != 0 && stParam.PARAM.nDefectCheck[INSP_POL_MISS])
			if (stParam.PARAM.nPolMissDefectCount != 0 && stParam.PARAM.bUsePolMiss )
			{
				_tcscpy(nDefType.cPanelId[INSP_POL_MISS][nOrderCnt] , strPanelId);	
			}
		}
		else if (nInspType == INSP_TFT_MEASURE)
		{
			if (stParam.PARAM.nDefectCount[INSP_TFT_MEASURE] != 0 && stParam.PARAM.nDefectCheck[INSP_TFT_MEASURE])
			{
				_tcscpy(nDefType.cPanelId[INSP_TFT_MEASURE][nOrderCnt] , strPanelId);	
			}
		}
		else if (nInspType == INSP_CF_MEASURE)
		{
			if (stParam.PARAM.nDefectCount[INSP_CF_MEASURE] != 0 && stParam.PARAM.nDefectCheck[INSP_CF_MEASURE])
			{
				_tcscpy(nDefType.cPanelId[INSP_CF_MEASURE][nOrderCnt] , strPanelId);	
			}
		}
		else if ( nInspType >= INSP_CFC_LUMINANCE && nInspType <= INSP_TDI_LUMINANCE)	//2016.11.21	Mr.Chu		Luminance code
		{
			int nLumiNum = 0;
			
			switch(nInspType)
			{
			case INSP_CFC_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_CFCLUMINANCE;
				break;
			case INSP_CBC_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_CBCLUMINANCE;
				break;
			case INSP_CFS_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_CFSLUMINANCE;
				break;
			case INSP_CBS_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_CBSLUMINANCE;
				break;
			case INSP_TFC_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_TFCLUMINANCE;
				break;
			case INSP_TBC_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_TBCLUMINANCE;
				break;
			case INSP_TFS_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_TFSLUMINANCE;
				break;
			case INSP_TBS_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_TBSLUMINANCE;
				break;
			case INSP_TDI_LUMINANCE:
				nLumiNum = LUMINANCE_TYPE_TDILUMINANCE;
				break;
			default:
				break;
			}
			//if (stParam.PARAM.nDefectCount[nInspType] != 0 && stParam.PARAM.nDefectCheck[nInspType])
			if (stParam.PARAM.nArrLuminanceCnt[nLumiNum] != 0 && stParam.PARAM.bArrLuminanceChk[nLumiNum])
			{
				_tcscpy(nDefType.cPanelId[nInspType][nOrderCnt] , strPanelId);	
			}
		}
		else if ( nInspType >= INSP_BUBBLB_STATION1 && nInspType <= INSP_BUBBLB_STATION12)	//2020.02.28 add by mby
		{
			int nBubbleStationNum = 0;
			switch(nInspType) 
			{
				case INSP_BUBBLB_STATION1:
					nBubbleStationNum = BUBBLESTATION_1;
					break;
				case INSP_BUBBLB_STATION2:
					nBubbleStationNum = BUBBLESTATION_2;
					break;
				case INSP_BUBBLB_STATION3:
					nBubbleStationNum = BUBBLESTATION_3;
					break;
				case INSP_BUBBLB_STATION4:
					nBubbleStationNum = BUBBLESTATION_4;
					break;
				case INSP_BUBBLB_STATION5:
					nBubbleStationNum = BUBBLESTATION_5;
					break;
				case INSP_BUBBLB_STATION6:
					nBubbleStationNum = BUBBLESTATION_6;
					break;
				case INSP_BUBBLB_STATION7:
					nBubbleStationNum = BUBBLESTATION_7;
					break;
				case INSP_BUBBLB_STATION8:
					nBubbleStationNum = BUBBLESTATION_8;
					break;
				case INSP_BUBBLB_STATION9:
					nBubbleStationNum = BUBBLESTATION_9;
					break;
				case INSP_BUBBLB_STATION10:
					nBubbleStationNum = BUBBLESTATION_10;
					break;
				case INSP_BUBBLB_STATION11:
					nBubbleStationNum = BUBBLESTATION_11;
					break;
				case INSP_BUBBLB_STATION12:
					nBubbleStationNum = BUBBLESTATION_12;
					break;
				default:
					break;
			}
			if (stParam.PARAM.nArrBubbleStationCnt[nBubbleStationNum] != 0 && stParam.PARAM.bArrBubbleStationChk[nBubbleStationNum])
			{
				_tcscpy(nDefType.cPanelId[nInspType][nOrderCnt] , strPanelId);	
			}
		}
		else
		{
			nDefCnt = 0;
			int nlength = 0;
			while(nDefCnt < MAX_DEFECT_CNT+2)		//Align Fail과 Pol Miss 추가 2016.08.22
			{		
				if (stOverlapCompDef->stPnlInfo[nDefCnt].nDefectType == nInspType)
				{
					_tcscpy(nDefType.cPanelId[nInspType][nOrderCnt] , strPanelId);
					_tcscpy(nDefType.cImgPath[nInspType][nOrderCnt] , stOverlapCompDef->stPnlInfo[nDefCnt].strImage);
					nDefType.startX[nInspType][nOrderCnt] = stOverlapCompDef->stPnlInfo[nDefCnt].startX;
					nDefType.startY[nInspType][nOrderCnt] = stOverlapCompDef->stPnlInfo[nDefCnt].startY;
					nDefType.endX	 [nInspType][nOrderCnt] = stOverlapCompDef->stPnlInfo[nDefCnt].endX  ;
					nDefType.endY	 [nInspType][nOrderCnt] = stOverlapCompDef->stPnlInfo[nDefCnt].endY  ;	
					nDefType.DefTypeCode [nInspType] = stOverlapCompDef->stPnlInfo[nDefCnt].nDefectType  ;	
					nDefType.DefTypeFlag [nInspType][nOrderCnt] = stOverlapCompDef->stPnlInfo[nDefCnt].nFlag  ;
					break;
				}
				nDefCnt++;
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (++nDefType.DefTypeCnt[nInspType] == 1)
			nDefType.StartDate[nInspType] = CTime::GetCurrentTime();
		//nRetVal = (systime.wYear% 2000)*100000000 +  systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;
	}
	G_AddLog(3, 3, _T("m_fnSetDefTypeCntAndStartDate() - End"));
	return TRUE;
}

BOOL CDataManager::m_fnGetOverlapDefTypeInfo(STRU_OVERLAP_DEFTYPE_PARAM& stDefType)
{ 
	G_AddLog(3, 3, _T("m_fnGetOverlapDefTypeInfo() - Start"));
	TCHAR strBuffer[255];
	CString strLine, strDefectCheck;
	int nCurPos = 0;
	int nDefectCnt = 0;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	stDefType.PARAM.bUseMode = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_USEMODE, 0);

	////////////////////////////////////////////////////////////////
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	CString resTokenKind;
	resTokenKind = strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{		
		stDefType.PARAM.nDefectCheck[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	CString resTokenCount;
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{		
		stDefType.PARAM.nDefectCount[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_STOP_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	CString resTokenAlarmCount;
	resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenAlarmCount != "")
	{		
		stDefType.nAlarmCount[nDefectCnt] = _ttoi(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_DEFECT_TERM, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	CString resTokenTime;
	resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenTime != "")
	{		
		stDefType.dHour[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	////////////////////////////////////////////////////////////////
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	resTokenKind = _T("");
	nCurPos=0;
	nDefectCnt=0;
	resTokenKind = strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{		
		stDefType.PARAM.bArrLuminanceChk[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount = _T("");
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{		
		stDefType.PARAM.nArrLuminanceCnt[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_STOP_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenAlarmCount = _T("");
	resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenAlarmCount != "")
	{		
		stDefType.nLumiAlarmCount[nDefectCnt] = _ttoi(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_LUMINANCE_TERM, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenTime = _T("");
	resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenTime != "")
	{		
		stDefType.dLumiHour[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	///////////////////////////////////////
	//2020.02.28 add by mby
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_CHECK, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	resTokenKind = _T("");
	nCurPos=0;
	nDefectCnt=0;
	resTokenKind = strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{		
		stDefType.PARAM.bArrBubbleStationChk[nDefectCnt] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_DEFECT_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenCount = _T("");
	resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{		
		stDefType.PARAM.nArrBubbleStationCnt[nDefectCnt] = _ttoi(resTokenCount);
		resTokenCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_STOP_COUNT, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenAlarmCount = _T("");
	resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenAlarmCount != "")
	{		
		stDefType.nBubbleStationAlarmCount[nDefectCnt] = _ttoi(resTokenAlarmCount);
		resTokenAlarmCount= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_BUBBLESTATION_TERM, _T(""), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenTime = _T("");
	resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenTime != "")
	{		
		stDefType.dBubbleStationHour[nDefectCnt] = _ttof(resTokenTime);
		resTokenTime= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
		nDefectCnt++;
	}
	stDefType.PARAM.bUseContactFail   =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_CHECK, 0);
	stDefType.PARAM.nContactFailDefectCount =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_SETTING_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_CONTACT_TERM,_T(""), strBuffer, 255);
	stDefType.dContactHour	= _ttof(strBuffer);

	//2016.02.24
	stDefType.PARAM.bUsePolMiss  =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_CHECK, 0);
	stDefType.PARAM.nPolMissDefectCount =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_SETTING_COUNT, 0);
	stDefType.PARAM.nPolMissStopCount =  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_STOP_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_POL_MISS_TERM, _T(""), strBuffer,255);
	stDefType.dPolMissHour		= _ttof(strBuffer);
	//2016.02.24

	//2016.08.22
	stDefType.PARAM.bUseAlignFail			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_CHECK, 0);
	stDefType.PARAM.nAlignFaliDefectCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_DEFECT_COUNT, 0);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_ALIGN_FAIL_TERM, _T(""), strBuffer,255);
	stDefType.dAlignFaliHour		= _ttof(strBuffer);
	
	//2016.08.22

	//2019.03.21---------------start
	stDefType.PARAM.bUseBgradecnt			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_CHECK, 0);
	stDefType.PARAM.nBgradecntDefectCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_SETTING_COUNT, 0);   //2019.03.26 def ->set
	stDefType.PARAM.nBgradecntStopCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_STOP_COUNT, 0);  
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_B_GRADE_TERM, _T(""), strBuffer,255);
	stDefType.dGradecountHour[BGRADECHECK_]		= _ttof(strBuffer);	

	stDefType.PARAM.bUseCgradecnt			=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_CHECK, 0);
	stDefType.PARAM.nCgradecntDefectCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_SETTING_COUNT, 0);    //2019.03.26 def ->set
	stDefType.PARAM.nCgradecntStopCount	=  pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_STOP_COUNT, 0);   //2019.03.26
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_OVERLAP_DEFTYPE, KEY_OVERLAP_C_GRADE_TERM, _T(""), strBuffer,255);
	stDefType.dGradecountHour[CGRADECHECK_]		= _ttof(strBuffer);	
	//2019.03.21---------------end

	G_AddLog(3, 3, _T("m_fnGetOverlapDefTypeInfo() - End"));

	return TRUE;
}

// m_fnPanelJudgeResult가 Setting 안된경우 예외처리
BOOL CDataManager::m_fnPanelJudgeResultCheck(int nDefType) 
{
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	if( pFrame->m_stJudgeCount->bUseMode == FALSE )
		return FALSE;
	else if( pFrame->m_stJudgeCount->nJudgeCount[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeLuminance[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeDistanceD1[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeDistanceD2[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeLength[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeThinknessW1[nDefType] == NULL &&
		pFrame->m_stJudgeCount->nJudgeThinknessW1[nDefType] == NULL )
		return FALSE;
	//

	return TRUE;
}


BOOL CDataManager::m_fnPanelJudgeResult(int nDefType) 
{ 
	G_AddLog(3, 3, _T("m_fnPanelJudgeResult() - Start"));
	BOOL bJudgeGradeB;
	int nSetCount = 0, nCurCount = 0;

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	nSetCount = pFrame->m_stJudgeCount->nJudgeCount[nDefType];
	nCurCount = m_stPanelJudgeInfo[nDefType].nJudgeCount;

	if (nSetCount < nCurCount || m_stPanelJudgeInfo[nDefType].bJudgeGradeA == FALSE)
	{
		bJudgeGradeB = TRUE;
		m_stPanelJudgeInfo[nDefType].nJudgeCount = 0;
		m_bPaCodeFlag[nDefType] = FALSE;	//2020.01.16 add by mby
		if (m_stPanelJudgeInfo[nDefType].bIsCF == TRUE || m_stPanelJudgeInfo[nDefType].bIsTFT == TRUE)
			m_stPanelJudgeInfo[nDefType].bJudgeGradeA = FALSE;
		else
			m_stPanelJudgeInfo[nDefType].bJudgeGradeA = TRUE;
	}
	else
	{
		bJudgeGradeB = FALSE;
	}
	G_AddLog(3, 3, _T("m_fnPanelJudgeResult() - End"));
	return bJudgeGradeB;
}

//2019.01.15 
BOOL CDataManager::m_fnMakeQMSByPassData_AVL_Force(CString strPanelid,  CString Grade, CString opid_strValue, CString RankGrade)
{
	G_AddLog(3, 3, _T("m_fnMakeQMSForceGrade_AVL() - Start"));
	CString strQmsFileMakeTime, strQmsDate, strQmsTime, strCode;
	CString strRetVal, strVal, strLine, strEqpModel, strEqpNo, strStepId;
	CString strQmsDataHead, strQmsDataPanel, strSubSide;
	CStringArray strArrayData;
	CString strLotType,strRecipeNo, strPartname;
	CStdioFile cstFile;
	TCHAR chRead[255] = {0};
	int nSeqNoCnt = 0, nEqpModel, nEqpNo;
	CTime time = CTime::GetCurrentTime();

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_LOT_TYPE1, 4, strLotType);
	pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PLC_PARTNAME_TYPE1, 4, strPartname);  //2019.01.24 by ryu

	int nRecipeNo = pFrame->m_iniRecipe.GetProfileInt(RECIPE_INFO,RECIPE_CURRENT_NO,0);

	strVal.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
	m_iniConfig.SetFileName(strVal);
	m_iniConfig.GetProfileStringW(EQP_STATE, KEY_EQP_TYPE, _T(""), (LPTSTR)chRead, 255);
	strVal = _T("U_");
	strVal += chRead;

	strRetVal = strVal;

	memset(chRead,0X00,sizeof(chRead));		
	nSeqNoCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_SEQ_NO, 0, m_cCountRoot);

	strVal.Format(_T("%.4d"), nSeqNoCnt);

	strVal = _T("_") + strVal;
	strRetVal +=  strVal;

	nSeqNoCnt ++;

	if (nSeqNoCnt > 999)	nSeqNoCnt = 1;

	strVal.Format(_T("%d"), nSeqNoCnt);
	WritePrivateProfileString(PNL_COUNT_SET, KEY_SEQ_NO, strVal, m_cCountRoot);

	strQmsFileMakeTime = time.Format(_T("%Y%m%d%H%M%S"));
	strVal =  _T("_") + strQmsFileMakeTime;
	strRetVal += strVal;
	strRetVal += EXTENSION_TMP;

	strVal = _T("D:\\QMS\\wait\\") + strRetVal;

	strQmsDate = time.Format(_T("%Y/%m/%d"));
	strQmsTime = time.Format(_T("%H:%M:%S"));
	strQmsDataHead.Append(_T("00"));
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(_T("3"));
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsDate);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsTime);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsDate);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsTime);
	strQmsDataHead.Append(EXT_COLON);

	nEqpModel = m_iniConfig.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	if (!nEqpModel)			
		strEqpModel.Format(_T("%3s"), _T("401"));			
	else
		strEqpModel.Format(_T("%3s"), _T("403"));
	strQmsDataHead.Append(strEqpModel);
	strQmsDataHead.Append(EXT_COLON);

	nEqpNo = m_iniConfig.GetProfileIntW(EQP_STATE, KEY_STATE_EQP_NO, 0);
	strEqpNo.Format(_T("%3d"), nEqpNo);

	strQmsDataHead.Append(strEqpNo);
	strQmsDataHead.Append(EXT_COLON);

	memset(chRead,0X00,sizeof(chRead));	
	m_iniConfig.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	strStepId  = chRead;
	strQmsDataHead.Append(strStepId);
	strQmsDataHead.Append(EXT_COLON);
	
	strQmsDataHead.Append(opid_strValue);
	strQmsDataHead.Append(EXT_COLON);
	strQmsDataHead.Append(strLotType.TrimRight());
	strQmsDataHead.Append(EXT_SEMICOL);

	strQmsDataPanel.Append(_T("13"));
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(strPanelid);
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(_T("1"));
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(Grade);
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(strCode);
	strQmsDataPanel.Append(EXT_COLON);
	strQmsDataPanel.Append(EXT_COLON);
	strRecipeNo.Format(_T("%03d"), nRecipeNo);
	strQmsDataPanel.Append(strRecipeNo);

	strQmsDataPanel.Append(EXT_COLON);
	strQmsDataPanel.Append(strPartname.TrimRight());  //2019.01.24 by ryu
			
	strQmsDataPanel.Append(EXT_SEMICOL);

	strSubSide.Append(_T("30,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,"));
	strSubSide.Append(RankGrade);				//Rank등급 //2020.08.17 Grade->RankGrade by mby
	strSubSide.Append(EXT_COLON);	
	strSubSide.Append(strLotType);
	strSubSide.Append(EXT_SEMICOL);

	strArrayData.Add(strQmsDataHead + _T("\n"));
	strArrayData.Add(strQmsDataPanel + _T("\n"));
	strArrayData.Add(strSubSide + _T("\n"));

	if (!cstFile.Open(strVal, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("File Write Error %s : %d"), strVal, GetLastError());
		return FALSE;
	}

	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	cstFile.Close();
	m_str_rankCIMvalue = RankGrade;		//2020.08.17 by mby
	FtpDstClient->m_fnFTPdUploadData();
	G_AddLog(3, 3, _T("m_fnMakeQMSForceGrade_AVL() - End"));
	return TRUE;
}

BOOL CDataManager::m_fnMakeQMSByPassData_AVL(CString strPanelid, long lBypassCode)
{
	G_AddLog(3, 3, _T("m_fnMakeQMSByPassData_AVL() - Start"));
	CString strQmsFileMakeTime, strQmsDate, strQmsTime, strCode;
	CString strRetVal, strVal, strLine, strEqpModel, strEqpNo, strStepId;
	CString strQmsDataHead, strQmsDataPanel, strSubSide;
	CStringArray strArrayData;
	CStdioFile cstFile;
	TCHAR chRead[255] = {0};
	int nSeqNoCnt = 0, nEqpModel, nEqpNo;
	CTime time = CTime::GetCurrentTime();

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	strCode.Format(_T("%d"), lBypassCode);	//2016.01.15

	strVal.Format(_T("%s%s"), ROOT_PATH, CONFIG_PATH);
	m_iniConfig.SetFileName(strVal);
	m_iniConfig.GetProfileStringW(EQP_STATE, KEY_EQP_TYPE, _T(""), (LPTSTR)chRead, 255);
	strVal = _T("U_");
	strVal += chRead;

	strRetVal = strVal;

	memset(chRead,0X00,sizeof(chRead));		
	nSeqNoCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_SEQ_NO, 0, m_cCountRoot);

	strVal.Format(_T("%.4d"), nSeqNoCnt);

	strVal = _T("_") + strVal;
	strRetVal +=  strVal;

	nSeqNoCnt ++;

	if (nSeqNoCnt > 999)	nSeqNoCnt = 1;

	strVal.Format(_T("%d"), nSeqNoCnt);
	WritePrivateProfileString(PNL_COUNT_SET, KEY_SEQ_NO, strVal, m_cCountRoot);

	strQmsFileMakeTime = time.Format(_T("%Y%m%d%H%M%S"));
	strVal =  _T("_") + strQmsFileMakeTime;
	strRetVal += strVal;
	strRetVal += EXTENSION_TMP;

	strVal = _T("D:\\QMS\\wait\\") + strRetVal;

	strQmsDate = time.Format(_T("%Y/%m/%d"));  // Format(_T(“%Y/%m/%d”)); 
	strQmsTime = time.Format(_T("%H:%M:%S"));  // Format(_T(“%H:%M:%S”));
	strQmsDataHead.Append(_T("00"));
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(_T("3"));
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsDate);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsTime);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsDate);
	strQmsDataHead.Append(EXT_COLON);

	strQmsDataHead.Append(strQmsTime);
	strQmsDataHead.Append(EXT_COLON);

	nEqpModel = m_iniConfig.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	if (!nEqpModel)			
		strEqpModel.Format(_T("%3s"), _T("401"));			
	else
		strEqpModel.Format(_T("%3s"), _T("403"));
	strQmsDataHead.Append(strEqpModel);
	strQmsDataHead.Append(EXT_COLON);

	nEqpNo = m_iniConfig.GetProfileIntW(EQP_STATE, KEY_STATE_EQP_NO, 0);
	strEqpNo.Format(_T("%3d"), nEqpNo);

	strQmsDataHead.Append(strEqpNo);
	strQmsDataHead.Append(EXT_COLON);

	memset(chRead,0X00,sizeof(chRead));	
	m_iniConfig.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	

	strStepId  = chRead;
	strQmsDataHead.Append(strStepId);
	strQmsDataHead.Append(EXT_COLON);


	strQmsDataHead.Append(EXT_SEMICOL);



	strQmsDataPanel.Append(_T("13"));
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(strPanelid);
	strQmsDataPanel.Append(EXT_COLON);

	strQmsDataPanel.Append(_T("C"));
	strQmsDataPanel.Append(EXT_COLON);

	//strQmsDataPanel.Append(_T("2115"));
	strQmsDataPanel.Append(strCode);	//2016.01.15
	strQmsDataPanel.Append(EXT_COLON);
	strQmsDataPanel.Append(EXT_COLON);
	strQmsDataPanel.Append(EXT_SEMICOL);

	strSubSide.Append(_T("30,0,0,0,0,0,0,0,0,0,"));  // 10
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));   // 10
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));   // 10
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));   // 10
	strSubSide.Append(_T("0,0,0,0,0,0,0,0,0,0,"));   // 10
	strSubSide.Append(_T("0,0,0,0,0"));				 // 5
	strSubSide.Append(EXT_SEMICOL);

	strArrayData.Add(strQmsDataHead + _T("\n"));
	strArrayData.Add(strQmsDataPanel + _T("\n"));
	strArrayData.Add(strSubSide + _T("\n"));

	if (!cstFile.Open(strVal, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("File Write Error %s : %d"), strVal, GetLastError());
		return FALSE;
	}

	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	cstFile.Close();

	//2015.09.28 Mr.Chu
	//if (FtpDstClient == NULL)
	//{
	//	delete FtpDstClient;
	//	FtpDstClient = new CFtpClient();
	//}
	FtpDstClient->m_fnFTPdUploadData();
	G_AddLog(3, 3, _T("m_fnMakeQMSByPassData_AVL() - End"));
	return TRUE;
}

BOOL CDataManager::m_fnMakeQMSByPassData_ABI(CString strPanelid)
{
	return FALSE;
}

void CDataManager::m_fnOverlapArraySort(STRU_DEFECT_COORD_INFO &stOverlapChkDef, int nDefCnt)
{
	STRU_DEFECT_COORD_DATA stTemp;

	for (int z = nDefCnt; z <  MAX_DEFECT_CNT - 1 ;  z++)
	{
		if (z >= MAX_DEFECT_CNT) break;
		stTemp.init_info();
		stTemp = stOverlapChkDef.stPnlInfo[z];
		stOverlapChkDef.stPnlInfo[z] = stOverlapChkDef.stPnlInfo[z+1];
		stOverlapChkDef.stPnlInfo[z+1] = stTemp;
	}
}

//2015.11.05
void CDataManager::m_fnSetRecipeVision(int nVision, CString strRecipe, int nUpdateFlag)
{
	CString strRecipeVision;

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	strRecipeVision.Format(_T("%s%d"), RECIPE_LIST_PC_, nVision);

	if(nUpdateFlag == FALSE)
	{
		pFrame->m_iniRecipe.WriteProfileStringW(RECIPE_INFO, (LPCTSTR)strRecipeVision, strRecipe);
	}
}

void CDataManager::m_fnSetRecipeList(int nListNo, STRU_RECIPE_LIST_INFO &stRecipeInfo)
{
	CString strRecipeListNo;

	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	strRecipeListNo.Format(_T("%s%d"), RECIPE_, nListNo);

	//pFrame->m_iniRecipe.WriteProfileStringW((RECIPE_INFO, strRecipeListNo, stRecipeInfo.strRecipePLC);
}

// 2015.10.15 add by ikm Packing 용 자동 레시피 변경 E

//2015.11.25 Mr.Chu
//기능 : 이전에 있던 검사 Data 삭제
void CDataManager::m_fnCurPanelDataInit(CString strPanelID_1, CString strPanelID_2, int nStageNum)
{	
	G_AddLog(3, 3, _T("m_fnCurPanelDataInit() - Start"));
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());

	CString strFileName = _T(""), strPanelId = _T("");
	CString strRootPath = _T(""), strPath = _T(""), strResFolderPath = _T("");
	//CFileFind finder;	

	strPanelID_1.Trim();
	strPanelID_2.Trim();

	strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

	if( strPanelID_1 == _T("") )
		return;

	strPanelId = strPanelID_1;
	for(int nPanelIndex=0; nPanelIndex<2; nPanelIndex++)
	{
		//strPath.Format(_T("%sVisionClient-%d\\"),strRootPath, nStageNum+1/*pDmg->m_nVisionCnt*/);

		//bWorking = finder.FindFile(strPath + _T("*.*"));	// 모든 파일

		//while(bWorking == TRUE)
		//{
		//	bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

		//	if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		//	strFileName = finder.GetFileName();	// 파일 이름 얻기			

		//	if ( strFileName != _T("") && strFileName.Find(strPanelId) != -1)
		//		DeleteFile(strPath + strFileName);

		//	if (bWorking == FALSE) break;
		//}
		//finder.Close();
		if( nStageNum == VISION_STAGE_1 )	//첫번째 스테이지 일 경우에만 Result 폴더 내 이전 데이터 삭제
		{
			strResFolderPath.Format(_T("%s%s"),strRootPath,strPanelId);
			m_fnDeleteFolder(strResFolderPath);		// Delete about Panel_ID in the Result folder

			m_fnDelLastClsyfyInfo(strPanelId,nPanelIndex);	// Delete about Classyfy inforamtion data in ClayInfo.ini

			G_AddLog_STG_SEQ(3, nStageNum, _T("[StageID = %d] [PANEL ID1 = %s] m_fnDelLastClsyfyInfo_Finish"),nStageNum,strPanelId );//20151221

			m_fnDelLastNGFlagInfo(strPanelId);				// Delete about NGflag inforamtion data in NGflagInfo.ini

			G_AddLog_STG_SEQ(3, nStageNum, _T("[StageID = %d] [PANEL ID1 = %s] m_fnDelLastNGFlagInfo_Finish"),nStageNum,strPanelId );//20151221
		}

		if( strPanelID_2 != _T("") )
			strPanelId = strPanelID_2;
		else
			return;
	}
	G_AddLog(3, 3, _T("m_fnCurPanelDataInit() - End"));
}

void CDataManager::m_fnDelLastClsyfyInfo(CString strPanelID,int nPanelIndex)
{
	G_AddLog(3, 3, _T("m_fnDelLastClsyfyInfo() - Start"));
	FILE *in = NULL, *out = NULL;
	errno_t errFileOpen;
	CString strClsEndInfoPath = _T("") , cCompareVal = _T("")  ;
	size_t read_size = 0;

	unsigned int nPanelCnt = 0;
	STRU_CLASSIFY_INFO stPcClassifyEndFlag[MAX_REV_PANEL][MAX_PANEL];


	strClsEndInfoPath.Format(_T("%s%s"), ROOT_PATH , CLASSIFY_INFO_PATH);
	errFileOpen = _tfopen_s(&in, strClsEndInfoPath, _T("rb+"));

	if(errFileOpen == 0/*true*/)
	{		
		read_size = fread(&stPcClassifyEndFlag, sizeof(stPcClassifyEndFlag), 1, in);			
		fclose(in);
	}
	else
	{
		//fclose(in);
		G_AddLog(2, 0, _T("m_fnDelLastClsyfyInfo() - Read Fail"));
		return ;
	}


	//Search about Panel_ID
	while(nPanelCnt < MAX_REV_PANEL)
	{
		cCompareVal = stPcClassifyEndFlag[nPanelCnt][nPanelIndex].cPanelID ; cCompareVal.TrimRight();

		if (!cCompareVal.IsEmpty() && cCompareVal == strPanelID)
		{
			stPcClassifyEndFlag[nPanelCnt][nPanelIndex].init_info();
			G_AddLog(2, 0, _T("m_fnDelLastClsyfyInfo() - Delete Panel ID : %s"), strPanelID);
			break;
		}
		nPanelCnt++;
	}

	errFileOpen = _tfopen_s(&out, strClsEndInfoPath, _T("wb"));
	if(errFileOpen == 0)
	{				
		fwrite(&stPcClassifyEndFlag, sizeof(stPcClassifyEndFlag) , 1, out);		
		fclose(out);
		G_AddLog(2, 0, _T("m_fnDelLastClsyfyInfo() - Write OK"));
	}
	G_AddLog(3, 3, _T("m_fnDelLastClsyfyInfo() - End"));
	//else
	//	fclose(out);
}

void CDataManager::m_fnDelLastNGFlagInfo(CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnDelLastNGFlagInfo() - Start"));
	FILE *in = NULL, *out = NULL;
	unsigned int nPanelCnt = 0;
	CString strVal = _T(""), strNgFlagInfoPath = _T("");
	STRU_NG_FLAG_INFO   stNgFlagInfo[MAX_REV_PANEL * 2];
	errno_t errFileOpen;

	strNgFlagInfoPath.Format(_T("%s%s"), ROOT_PATH , NGFLAG_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strNgFlagInfoPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&stNgFlagInfo, sizeof(stNgFlagInfo), 1, in);			
		fclose(in);
	}	
	else
	{
		//fclose(in);
		return ;
	}

	while(nPanelCnt < MAX_REV_PANEL * 2)
	{
		strVal = stNgFlagInfo[nPanelCnt].cPanelID; 
		strVal.TrimRight();
		if (!strVal.IsEmpty() && strVal == strPanelID )
		{				
			stNgFlagInfo[nPanelCnt].init_info();
			break;
		}		
		nPanelCnt ++;
	}

	errFileOpen = _tfopen_s(&out, strNgFlagInfoPath, _T("wb"));
	if(errFileOpen == 0)
	{		
		fwrite(&stNgFlagInfo, sizeof(stNgFlagInfo) , 1, out);		
		fclose(out);
	}	
	//else
	//	fclose(out);
	G_AddLog(3, 3, _T("m_fnDelLastNGFlagInfo() - End"));
}

void CDataManager::m_fnDeleteAllFolderThread(CString strPath)
{
	m_strDelPath		= _T("");
	m_strDelOrignPath	= _T("");
	UINT uiThreadID;
	m_strDelPath = strPath;
	m_strDelOrignPath = strPath;

	if (pTreadProcData->m_hThreadDelDir == NULL)
	{
		pTreadProcData->m_hThreadDelDir = (HANDLE)::_beginthreadex(NULL, 0, pTreadProcData->m_fnDeleteAllFolder,(LPVOID) this, 0, &uiThreadID);
	}

}
//2015.12.10 Mr.Chu
//기능 : 내부 Judge 판정기준 동작을 위한 구조체 초기화
// TRUE : 불러오기  FALSE : 저장
void CDataManager::m_fnJudgeSetItem(bool bFunc)
{
	//m_stJudgeItemInfo.init_info(bFunc);

}

// 해당 결함 타입의 문자열 명을 반환한다
CString CDataManager::m_fnGetDefTypeName(int nDefType)
{
	G_AddLog(3, 3, _T("m_fnGetDefTypeName() - Start"));
	CString strDeftypeName;
	switch(nDefType)
	{
	case INSP_DUST				  :    strDeftypeName = _T("DUST");	break;
	case INSP_BLACK_SCRATCH		  :	   strDeftypeName = _T("BLACK_SCRATCH"); break;
	case INSP_ITO				  :	   strDeftypeName = _T("ITO"); break;
	case INSP_DIMPLE_POINT		  :	   strDeftypeName = _T("DIMPLE_POINT"); break;
	case INSP_DIMPLE_LINE		  :	   strDeftypeName = _T("DIMPLE_LINE"); break;
	case INSP_ETCHING_LINE		  :	   strDeftypeName = _T("ETCHING_LINE"); break;
	case INSP_WHITE_TURBIDITY	  :	   strDeftypeName = _T("WHITE_TURBIDITY"); break;
	case INSP_PAD_BURNT			  :	   strDeftypeName = _T("PAD_BURNT"); break;
	case INSP_EDGE_BROKEN		  :	   strDeftypeName = _T("EDGE_BROKEN"); break;
	case INSP_PAD_SCRATCH		  :	   strDeftypeName = _T("PAD_SCRATCH"); break;
	case INSP_CF_MEASURE		  :	   strDeftypeName = _T("CF_MEASURE"); break;
	case INSP_TFT_MEASURE		  :	   strDeftypeName = _T("TFT_MEASURE"); break;
	case INSP_BURR				  :	   strDeftypeName = _T("BURR");	break;
	case INSP_POL_BUBBLE		  :	   strDeftypeName = _T("POL_BUBBLE"); break;	
	case INSP_POL_SCRATCH		  :	   strDeftypeName = _T("POL_SCRATCH"); break;
	case INSP_MURA				  :	   strDeftypeName = _T("MURA"); break;
	case INSP_POL_ALIEN_SUBSTANCE :	   strDeftypeName = _T("CF_POL_SUBSTANCE"); break;
	case (INSP_POL_ALIEN_SUBSTANCE+1)	  :	   strDeftypeName = _T("TFT_POL_SUBSTANCE"); break;
	case INSP_CF_BURR			  :	   strDeftypeName = _T("CF_BURR"); break;
	case INSP_TFT_BURR			  :	   strDeftypeName = _T("TFT_BURR"); break;
	case INSP_CF_BROKEN			  :	   strDeftypeName = _T("CF_BROKEN"); break;
	case INSP_TFT_BROKEN		  :	   strDeftypeName = _T("TFT_BROKEN"); break;
	case INSP_CF_CORNER_BROKEN	  :	   strDeftypeName = _T("CF_CORNER_BROKEN"); break;
	case INSP_TFT_CORNER_BROKEN	  :	   strDeftypeName = _T("TFT_CORNER_BROKEN"); break;
	case INSP_CF_CELL_BROKEN	  :	   strDeftypeName = _T("CF_CELL_BROKEN"); break;
	case INSP_TFT_CELL_BROKEN	  :	   strDeftypeName = _T("TFT_CELL_BROKEN"); break;
	case INSP_CONTACT			  :	   strDeftypeName = _T("CONTACT_FAIL"); break;
	case INSP_POL_MISS			  :	   strDeftypeName = _T("POL MISS"); break;
	case INSP_ALIGN_FAIL		  :	   strDeftypeName = _T("ALIGN FAIL"); break;
	case INSP_CFC_LUMINANCE		  :	   strDeftypeName = _T("CFC_LUMINANCE"); break;
	case INSP_CBC_LUMINANCE		  :	   strDeftypeName = _T("CBC_LUMINANCE"); break;
	case INSP_CFS_LUMINANCE		  :	   strDeftypeName = _T("CFS_LUMINANCE"); break;
	case INSP_CBS_LUMINANCE		  :	   strDeftypeName = _T("CBS_LUMINANCE"); break;
	case INSP_TFC_LUMINANCE		  :	   strDeftypeName = _T("TFC_LUMINANCE"); break;
	case INSP_TBC_LUMINANCE		  :	   strDeftypeName = _T("TBC_LUMINANCE"); break;
	case INSP_TFS_LUMINANCE		  :	   strDeftypeName = _T("TFS_LUMINANCE"); break;
	case INSP_TBS_LUMINANCE		  :	   strDeftypeName = _T("TBS_LUMINANCE"); break;
	case INSP_TDI_LUMINANCE		  :	   strDeftypeName = _T("TDI_LUMINANCE"); break;
	case INSP_CF_PROTECTION		  :	   strDeftypeName = _T("CF_PROTECTION"); break;
	case INSP_TFT_PROTECTION	  :	   strDeftypeName = _T("TFT_PROTECTION"); break;
	// 2019.03.15 defecttype로 사용 안함.
	case INSP_B_GRADE_CHECK		  :	   strDeftypeName = _T("B_GRADE_CHECK"); break;
	case INSP_C_GRADE_CHECK	  :	   strDeftypeName = _T("C_GRADE_CHECK"); break;
	//2020.02.28 add by mby
	case INSP_BUBBLB_STATION1	  :	   strDeftypeName = _T("STATION1_DEFECT"); break;
	case INSP_BUBBLB_STATION2	  :	   strDeftypeName = _T("STATION2_DEFECT"); break;
	case INSP_BUBBLB_STATION3	  :	   strDeftypeName = _T("STATION3_DEFECT"); break;
	case INSP_BUBBLB_STATION4	  :	   strDeftypeName = _T("STATION4_DEFECT"); break;
	case INSP_BUBBLB_STATION5	  :	   strDeftypeName = _T("STATION5_DEFECT"); break;
	case INSP_BUBBLB_STATION6	  :	   strDeftypeName = _T("STATION6_DEFECT"); break;
	case INSP_BUBBLB_STATION7	  :	   strDeftypeName = _T("STATION7_DEFECT"); break;
	case INSP_BUBBLB_STATION8	  :	   strDeftypeName = _T("STATION8_DEFECT"); break;
	case INSP_BUBBLB_STATION9	  :	   strDeftypeName = _T("STATION9_DEFECT"); break;
	case INSP_BUBBLB_STATION10	  :	   strDeftypeName = _T("STATION10_DEFECT"); break;
	case INSP_BUBBLB_STATION11	  :	   strDeftypeName = _T("STATION11_DEFECT"); break;
	case INSP_BUBBLB_STATION12	  :	   strDeftypeName = _T("STATION12_DEFECT"); break;
	}
	G_AddLog(3, 3, _T("m_fnGetDefTypeName() - End"));
	return strDeftypeName;
}


int CDataManager::m_fnTestMemory(STRU_CLASSIFY_INFO& cPanelInfo1, STRU_CLASSIFY_INFO& cPanelInfo2, int nVisionCount)
{
	int nVisionCnt = 0 ,nEqpNo = 0, nRet = NG;
	int nTotalImgCnt = 0;
	CString strTemp = _T("");
	CString strQmsFilePath;
	CString cCompareVal;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	BOOL bQmsStartFlag =FALSE;
	CString strResutJudgeThreadVal;

	int nProcQms = 0, nQmsFinalCnt = 0;

	STRU_CLASSIFY_INFO strPanelInfo1;
	STRU_CLASSIFY_INFO strPanelInfo2;

	strPanelInfo1 = cPanelInfo1;
	strPanelInfo2 = cPanelInfo2;

	//bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 1);


	nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);
	m_bUseSemaphore = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_USE_SEMAPHORE, 0);
	///////////////////////////////////////////////////////////////////////////////////////////////
	//2015.06.12 add by IKM - S	
	for (int i = 1 ; i< MAX_VISION_PC ;i++) // 투입 된 Panel의 검사가 모두 끝났는지 체크
	{
		if (strPanelInfo1.bComplete[i] == TRUE)
		{
			if (i != SIGNAL_PC) 
			{
				nProcQms++;
				nTotalImgCnt += strPanelInfo1.nTotalDefCnt[i];
			}
			nQmsFinalCnt++;
		}		
	}
	G_AddLog(2, 3, _T("[Vision : %d] [Panel Id1 : %s] [Panel Id2 : %s] nProcQms : %d nQmsFinalCnt : %d"), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo2.cPanelID, nProcQms, nQmsFinalCnt);	//2016.01.07 CHECK QMS COUNT LOG
	m_nQmsFinalCnt = nProcQms;
	if (nVisionCount != SIGNAL_PC)
	{		
		cCompareVal = strPanelInfo1.cPanelID;
		if (cCompareVal != (_T(""))) 
		{			 			
			_tcscpy(strPanelInfo1.cPanelPath  , m_fnReadDefectData(strPanelInfo1, nVisionCount));			
			pFrame->m_DataManager.m_strPanelPath[PANEL_ONE] = strPanelInfo1.cPanelPath ;
			// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.
			if (nProcQms == 1)
			{
				m_fnGetDefStartDate(strPanelInfo1.cPanelPath);				
				_tcscpy(strPanelInfo1.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo1.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo1.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo1.cDataTime   , m_strSaveTime);		
			}

			if (strPanelInfo1.cStartDate == _T("") || strPanelInfo1.cStartTime  == _T("")
				|| strPanelInfo1.cDataDate == _T("") || strPanelInfo1.cDataTime  == _T(""))
			{
				m_fnGetDefStartDate(strPanelInfo1.cPanelPath);				
				_tcscpy(strPanelInfo1.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo1.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo1.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo1.cDataTime  , m_strSaveTime);	
			}
			m_strInspDate = strPanelInfo1.cStartDate;					m_strInspTime = strPanelInfo1.cStartTime;
			m_strSaveDate = strPanelInfo1.cDataDate ;					m_strSaveTime = strPanelInfo1.cDataTime;

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo1.cPanelID, strPanelInfo1.cStartDate);

			m_fnSetDefStartDate(strPanelInfo1.cPanelPath);		

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Write Defect Data!!"), nVisionCount, strPanelInfo1.cPanelID);
		}
		cCompareVal = strPanelInfo2.cPanelID ;
		if (cCompareVal != _T(""))
		{			
			_tcscpy(strPanelInfo2.cPanelPath  , m_fnReadDefectData(strPanelInfo2, nVisionCount));
			pFrame->m_DataManager.m_strPanelPath[PANEL_TWO] = strPanelInfo2.cPanelPath ;
			if (nProcQms == 1)
			{
				m_fnGetDefStartDate(strPanelInfo2.cPanelPath);				
				_tcscpy(strPanelInfo2.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo2.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo2.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo2.cDataTime  , m_strSaveTime);	
			}
			if (strPanelInfo2.cStartDate == _T("") || strPanelInfo2.cStartTime  == _T("") 
				|| strPanelInfo2.cDataDate == _T("") || strPanelInfo2.cDataTime  == _T(""))
			{
				m_fnGetDefStartDate(strPanelInfo2.cPanelPath);				
				_tcscpy(strPanelInfo2.cStartDate  , m_strInspDate);		_tcscpy(strPanelInfo2.cStartTime  , m_strInspTime);		
				_tcscpy(strPanelInfo2.cDataDate  , m_strSaveDate);		_tcscpy(strPanelInfo2.cDataTime  , m_strSaveTime);	
			}

			m_strInspDate = strPanelInfo2.cStartDate;					m_strInspTime = strPanelInfo2.cStartTime;			
			m_strSaveDate = strPanelInfo2.cDataDate;					m_strSaveTime = strPanelInfo2.cDataTime;

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Insp Start Time : %s"), nVisionCount, strPanelInfo2.cPanelID, strPanelInfo2.cStartDate);

			m_fnSetDefStartDate(strPanelInfo2.cPanelPath);			

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Write Defect Data!!"), nVisionCount, strPanelInfo2.cPanelID);
		}
	}
	//2015.06.16 by sjw - s
	int nContactOverlapCnt = 0;
	int nContactPanelOverlapCnt = 0;
	if (nVisionCount == SIGNAL_PC)
	{

		if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE && pFrame->m_stOverlapCoordInfo->bUseContactFail == TRUE)
		{
			nContactPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, 0);
			if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nContactPanelOverlapCnt + 1)
			{
				strTemp.Format(_T("%d"), 0);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
			}
			nContactPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, 0);
			strTemp.Format(_T("%d"), nContactPanelOverlapCnt + 1);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
			if (strPanelInfo1.nPgGrade == ITO_NG || strPanelInfo2.nPgGrade == ITO_NG)
			{
				nContactOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, 0);
				if (pFrame->m_stOverlapCoordInfo->nContactFailDefectCount <= nContactOverlapCnt + 1 && 
					pFrame->m_stOverlapCoordInfo->nPanelCnt >= nContactPanelOverlapCnt + 1)
				{
					if (strPanelInfo1.nPgGrade == ITO_NG)
						m_fnContactFailAlarmSet(_T("PG : ITO RESIST ILEGAL"), strPanelInfo1);
					if (strPanelInfo2.nPgGrade == ITO_NG)		
						m_fnContactFailAlarmSet(_T("PG : ITO RESIST ILEGAL"), strPanelInfo2);

					strTemp.Format(_T("%d"), 0);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				}
				else
				{
					strTemp.Format(_T("%d"), nContactOverlapCnt + 1);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
				}
			}

			if (strPanelInfo1.nPgGrade == CONTACT_NG || strPanelInfo2.nPgGrade == CONTACT_NG)
			{
				nContactOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, 0);
				if (pFrame->m_stOverlapCoordInfo->nContactFailDefectCount <= nContactOverlapCnt + 1 && 
					pFrame->m_stOverlapCoordInfo->nPanelCnt >= nContactPanelOverlapCnt + 1)
				{
					if (strPanelInfo1.nPgGrade == CONTACT_NG)
						m_fnContactFailAlarmSet(_T("PG : Contact ILEGAL"), strPanelInfo1);
					if (strPanelInfo2.nPgGrade == CONTACT_NG)	
						m_fnContactFailAlarmSet(_T("PG : Contact ILEGAL"), strPanelInfo2);

					strTemp.Format(_T("%d"), 0);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_PANEL_CURRENT_COUNT, strTemp);
				}
				else
				{
					strTemp.Format(_T("%d"), nContactOverlapCnt + 1);
					pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_CONTACT_CURRENT_COUNT, strTemp);
				}
			}
		}
		G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] SIGNAL_PC!! :  %d"), nVisionCount, strPanelInfo1.cPanelID ,nProcQms);

	}
	//2015.06.16  by sjw - e

	if (nQmsFinalCnt/* >= pFrame->m_stParamInfo.nCountVisionClient*/ )
	{			
		if (nProcQms == 1)	pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnInitState();

		int nTestMode = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_STATE_TEST_MODE, 0);
		if( !nTestMode)	// 모든 Vision Client 검사가 완료되었으면...
		{	
			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - Start Data Merge "), nVisionCount, strPanelInfo1.cPanelID);

			m_fnSetPanelId1(strPanelInfo1.cPanelID);
			m_fnSetPanelId2(strPanelInfo2.cPanelID);

			CString strRootPath = _T("");
			CString strPath = _T("");

			strRootPath.Format(_T("%s%s\\"), DATA_PATH, RESULT_PATH);

			cCompareVal = strPanelInfo1.cPanelID;
			if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_ONE] = strPanelInfo1.cPanelPath; // m_fnGetLastWriteFile(strPanelInfo1.cPanelPath);

			cCompareVal = strPanelInfo2.cPanelID;
			if(cCompareVal !=(_T("")))		m_strPanelPath[PANEL_TWO] = strPanelInfo2.cPanelPath;// m_fnGetLastWriteFile(strPanelInfo2.cPanelPath);

			//// 마지막 생성된 파일 외에는 모두 삭제 처리 한다.

			if (m_strPanelPath[PANEL_ONE].Compare(_T("")))
			{		
				m_strInspDate = strPanelInfo1.cStartDate;	m_strInspTime = strPanelInfo1.cStartTime;
				m_strSaveDate = strPanelInfo1.cDataDate;	m_strSaveTime = strPanelInfo1.cDataTime;

				//if (m_strSemaGrade.Find(_T("ITO_NG")) != -1)
				if (strPanelInfo1.nPgGrade == ITO_NG)	  m_strSemaGrade = _T("ITO_NG");
				if (strPanelInfo1.nPgGrade == CONTACT_NG) m_strSemaGrade = _T("CONTACT_NG");

				//m_fnUpLoadImgFile(strPanelInfo1.cPanelID); // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
				//Sleep(500);
				m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_ONE], strPanelInfo1.cPanelID, PANEL_ONE);					
				m_fnDelLocalInspData(strPanelInfo1.cPanelID, nVisionCount) ;  // 이미지 삭제 스레드			
				//m_strDefGrade[PANEL_ONE]			= m_strDefGrade1;
				//m_strdefJudge[PANEL_ONE][JUDGE_ONE] = m_strdefJudge1; 
				//m_strdefJudge[PANEL_ONE][JUDGE_TWO] = m_strdefJudge2;
				//m_strCimMeasureData[PANEL_ONE] = m_strCimMeasureDataTemp;	//2016.01.15

			}
			if (m_strPanelPath[PANEL_TWO].Compare(_T("")))
			{
				m_strInspDate = strPanelInfo2.cStartDate;	m_strInspTime = strPanelInfo2.cStartTime;
				m_strSaveDate = strPanelInfo2.cDataDate;	m_strSaveTime = strPanelInfo2.cDataTime;

				if (strPanelInfo2.nPgGrade == ITO_NG)	  m_strSemaGrade = _T("ITO_NG");
				if (strPanelInfo2.nPgGrade == CONTACT_NG) m_strSemaGrade = _T("CONTACT_NG");

				//m_fnUpLoadImgFile(strPanelInfo2.cPanelID);  // QMS 용 이미지 리네임 && QMS(i_unsent) 업로드
				//Sleep(500);
				m_fnMakeQmsUpadateFile(m_strPanelPath[PANEL_TWO], strPanelInfo2.cPanelID, PANEL_TWO);	
				m_fnDelLocalInspData(strPanelInfo2.cPanelID, nVisionCount) ;  // 이미지 삭제 스레드		
				//m_strDefGrade[PANEL_TWO]			= m_strDefGrade1;
				//m_strdefJudge[PANEL_TWO][JUDGE_ONE] = m_strdefJudge1; 
				//m_strdefJudge[PANEL_TWO][JUDGE_TWO] = m_strdefJudge2;
				//m_strCimMeasureData[PANEL_TWO] = m_strCimMeasureDataTemp;	//2016.01.15
			}

			m_bInspEnd = TRUE;


			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - End Data Merge "), nVisionCount, strPanelInfo1.cPanelID);

			G_AddLog(2, 3, _T("[Vision : %d] [Panel Id : %s] Process QMS Data!! - Start FTP Upload"), nVisionCount, strPanelInfo1.cPanelID);

			/*	if (FtpDstClient == NULL)
			{
			delete FtpDstClient;
			FtpDstClient = new CFtpClient();

			}		*/
			FtpDstClient->m_fnFTPdUploadData();
			//if (FtpImgClient == NULL)
			//{
			//	delete FtpImgClient;
			//	FtpImgClient = new CFtpClient();

			//}
			FtpImgClient->m_fnFTPdUploadImg();		
			nVisionCnt = 1;
			strTemp.Format(_T("%d"), nVisionCnt);

			strPanelInfo1.init_info();
			strPanelInfo2.init_info();
		}	
	}

	cPanelInfo1 = strPanelInfo1;
	cPanelInfo2 = strPanelInfo2;

	m_fnDeleteAllFolderThread(_T("D:\\QMS\\i_wait"));
	//2015.06.12 add by IKM - E
	///////////////////////////////////////////////////////////////////////////////////////////////
	return nRet = OK;
}


BOOL CDataManager::m_fnInitFtpClient(void)
{
	G_AddLog(3, 3, _T("m_fnInitFtpClient() - Start"));
	BOOL rc = FALSE;
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	int iLogWriteTerm = 0, iPeriodDay = 0;
	TSTRING strPath = _T(""), strLogName = _T(""), strLogNameTmp = _T("");
	wchar_t chLogName[255] = {0};

	iLogWriteTerm = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, LOG_WRITE_TERM, 0);	
	iPeriodDay = pFrame->m_iniConfigFtp.GetProfileIntW(FTP_LOG_STATE, KEY_FTP_DIR_PERIOD_DAY, 30);
	strPath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Log;
	pFrame->m_iniConfigFtp.GetProfileStringW(FTP_LOG_STATE, KEY_FTP_DIR_LOG_NAME, _T(""), (LPTSTR)chLogName, 255);
	strLogName = chLogName;
	strLogNameTmp = strLogName + _T("_INTERNAL");

	FtpDstClient = NULL;
	FtpDstClient = new CFtpClient(strPath, strLogNameTmp, iPeriodDay, iLogWriteTerm );

	//2017.06.27 성도Panda QMS 고객사 Log
	pFrame->m_QMSGuestLog.m_fnSetInitInfo(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Log, chLogName);
	//

	strPath = _T(""), strLogNameTmp = _T("");

	strPath = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iLog;
	strLogNameTmp = strLogName + _T("_INTERNAL");
	FtpImgClient = NULL;
	FtpImgClient = new CFtpClient(strPath, strLogNameTmp, iPeriodDay, iLogWriteTerm );

	//2017.06.27 성도Panda QMS 고객사 Log
 	pFrame->m_QMSGuestImgLog.m_fnSetInitInfo(pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_iLog, chLogName);
	//

	FtpDstClient->m_fnSetAutoDeleteInfo(pFrame->m_stFtpInfo.stPath->strDir_Default , pFrame->m_stFtpInfo.stInfo->iDirDelDay);
	G_AddLog(3, 3, _T("m_fnInitFtpClient() - End"));
	return TRUE;

}

//2016.11.21	Mr.Chu
void CDataManager::m_fnSearchOverlapLuminanceCode(CString strFilePath, CString strPanelID)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if (pFrame->m_stOverlapCoordInfo->bUseMode == FALSE)	// Overlap Defect 전체 Option 에 관한 예외처리
		return;

	int i=0, nCurPos = 0, nDefectCnt = 0;
	int nStringCnt = 0, nDefectNum = 0;
	CStdioFile cstFile;
	CString strTemp = _T("");
	TCHAR	strBuffer[255];
	CString strLine, strItem, strPanelData;
	CString strDefectCheck = _T(""), resTokenKind = _T("");

	//
	int** nArrLumiData = new int*[LUMINANCE_TYPE_MAX];		// [0]	: 존재 유무   ,    [1]	: Defect Code		
	for(i=0;i<LUMINANCE_TYPE_MAX;i++)
		nArrLumiData[i] = new int[2];

	int* nArrLumiPanelOverlapCnt = new int[LUMINANCE_TYPE_MAX];
	int* nArrLumiOverlapCnt = new int[LUMINANCE_TYPE_MAX];
	//

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3,2, _T("m_fnSearchOverlapPolMiss File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	for(i=0;i<LUMINANCE_TYPE_MAX;i++)
	{
		nArrLumiData[i][0] = FALSE;
		nArrLumiData[i][1] = 0;
	}

	//Find & Get Luminance Code Coordinate Data
	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

 		strItem = m_fnGetItemData(LAYER_NO - 1, strLine);

		if( !strItem.Compare(_T("TFT77")) || !strItem.Compare(_T("CF77")) || !strItem.Compare(_T("POL78")) )
		{
			strPanelData = strLine;
			strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);
			strPanelData.TrimRight();

			nStringCnt = strPanelData.Replace(EXT_COLON,EXT_COLON) + 1;

			for(i=0;i<nStringCnt;i++)
			{
				strTemp = m_fnGetItemData(LAYER_NO - 1, strPanelData);
				strTemp.Trim();
				strPanelData = strPanelData.Mid(strPanelData.Find(EXT_COLON)+1);
				
				if( strTemp == _T("0") )
					continue;

				nDefectNum = _ttoi(strTemp);
				switch(nDefectNum)
				{
				case CFCLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_CFCLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_CFCLUMINANCE][1] = CFCLUMINANCE;
					break;
				case CBCLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_CBCLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_CBCLUMINANCE][1] = CBCLUMINANCE;
					break;
				case CFSLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_CFSLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_CFSLUMINANCE][1] = CFSLUMINANCE;
					break;
				case CBSLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_CBSLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_CBSLUMINANCE][1] = CBSLUMINANCE;
					break;
				case TFCLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_TFCLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_TFCLUMINANCE][1] = TFCLUMINANCE;
					break;
				case TBCLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_TBCLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_TBCLUMINANCE][1] = TBCLUMINANCE;
					break;
				case TFSLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_TFSLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_TFSLUMINANCE][1] = TFSLUMINANCE;
					break;
				case TBSLUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_TBSLUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_TBSLUMINANCE][1] = TBSLUMINANCE;
					break;
				case TDILUMINANCE:
					nArrLumiData[LUMINANCE_TYPE_TDILUMINANCE][0] = TRUE;
					nArrLumiData[LUMINANCE_TYPE_TDILUMINANCE][1] = TDILUMINANCE;
					break;
				default:
					break;
				}
			}
		}
	}
	cstFile.Close();

	//	Get 
	resTokenKind = _T("");
	pFrame->m_iniOverlapCurCoord.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_PANEL_CURRENT_COUNT, _T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{
		nDefectCnt++;
		nArrLumiPanelOverlapCnt[nDefectCnt - 1] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}

	///////
	resTokenKind = _T("");
	pFrame->m_iniOverlapCurCoord.GetProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_LUMINANCE_CURRENT_COUNT, _T("0,0,0,0,0,0,0,0,0"), strBuffer, 255);
	strDefectCheck = strBuffer;
	nCurPos = 0;
	nDefectCnt = 0;
	resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	while (resTokenKind != "")
	{
		nDefectCnt++;
		nArrLumiOverlapCnt[nDefectCnt - 1] = _ttoi(resTokenKind);
		resTokenKind= strDefectCheck.Tokenize(EXT_COLON, nCurPos);
	}
	//

	for(i=0;i<LUMINANCE_TYPE_MAX;i++)
	{

		if ( pFrame->m_stOverlapCoordInfo->bArrLuminanceChk[i] == TRUE
			&& nArrLumiData[i][0] == TRUE)
		{

			if ( pFrame->m_stOverlapCoordInfo->nPanelCnt < nArrLumiPanelOverlapCnt[i] + 1 )	// if Max Panel Count Overment, Total Counting Parameter Reset
				nArrLumiPanelOverlapCnt[i] = NULL;


			if (pFrame->m_stOverlapCoordInfo->nArrLuminanceCnt[i] <= nArrLumiOverlapCnt[i] + 1  && 
				pFrame->m_stOverlapCoordInfo->nPanelCnt <= nArrLumiPanelOverlapCnt[i] + 1)
			{
				CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
				CString strMsgBuf  = _T("");
				CString strSendMsg = _T("");

				strMsgBuf.Format(_T("Luminance Code : %d"),nArrLumiData[i][1] );

				strSendMsg.Append(strMsgBuf);
				strSendMsg.Append(_T("#"));
				strSendMsg.Append(strPanelID);
				strSendMsg.Append(_T("#"));

				va_list vaList;
				va_start(vaList, strSendMsg);
				int len = _vscwprintf( strSendMsg, vaList ) + 1;
				wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
				wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
				memset(cBuffer,0,sizeof(wchar_t)*len);
				memset(cTempBuff,0,sizeof(wchar_t)*len);
				if (cBuffer)
				{
					wcscpy((WCHAR*)cBuffer, strSendMsg);
					memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);	

					// Pol Mark 와 Defect 보고 형태가 같아 Msg 공유	
					pView->SendMessage(WM_UI_NEW_POL_MARK_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff ); 	

					delete[] cBuffer;
				}		
				nArrLumiOverlapCnt[i]		= 0;
				nArrLumiPanelOverlapCnt[i]	= 0;
			}
			else
			{
				nArrLumiOverlapCnt[i]		++;		
				nArrLumiPanelOverlapCnt[i]	++;
			}
		}

	}
	//2016.11.21		
	///////////////////////////////////////// 
	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), nArrLumiPanelOverlapCnt[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniOverlapCurCoord.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_OVERLAP_LUMINANCE_PANEL_CURRENT_COUNT, strTemp);
	/////////////////////////////////////////
	strTemp = _T("");
	for (int i = 0; i < LUMINANCE_TYPE_MAX; i++)
	{
		strDefectCheck.Format(_T("%d"), nArrLumiOverlapCnt[i]);

		if (i == LUMINANCE_TYPE_MAX - 1)	strTemp.Append(strDefectCheck);
		else	strTemp.Append(strDefectCheck + EXT_COLON);
	}
	pFrame->m_iniOverlapCurCoord.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_OVERLAP_LUMINANCE_CURRENT_COUNT, strTemp);
	//


	//////
	for(i=0;i<LUMINANCE_TYPE_MAX;i++)
		delete [] nArrLumiData[i];
	delete [] nArrLumiData;

	delete [] nArrLumiOverlapCnt;
	delete [] nArrLumiPanelOverlapCnt;
}
//2017.08.08
//기능			:	Overlap Data에 관한 현재 카운트 값을 저장하고 현재값과 설정값을 비교하여 동작시킨다.
//Parameter		:	strPanelID		-	Panel ID
//					nDefCnt			-	Panel Def Count
//					bUserItem		-	Count Overlap 체크 유무
//					nUserDefCnt		-	Count Overlap Def 설정 갯수
//					strDefCur		-	현재 발생한 Def Count ( 파일 저장 항목 이름 )								ex) KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT
//					strPanelCur		-	현재 Def 발생한 Panel에 대한 Count	( 파일 저장 항목 이름 )					ex) KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT
//Memo			:	추 후 Align 및 Pol 관련 Count Overlap을 이 함수를 사용하여 쓰도록 변경할 것
//					현재 Config.ini에 저장하여 사용하는데 추 후 이 함수에 대한 파일을 만들어 사용할 것.
 void CDataManager::m_fnSearchOverlapData( CString strPanelID, int nDefCnt, BOOL bUserItem, int nUserDefCnt, CString strDefCur, CString strPanelCur, CString strAlarmMsg )
 {
     CString strTemp = _T("");
	 int nOverlapCnt = 0;
	 int nPanelOverlapCnt = 0;

	 CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	 if( bUserItem == TRUE )
	 {
		 nPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, strPanelCur, 0);

		 if ( pFrame->m_stOverlapCoordInfo->nPanelCnt < nPanelOverlapCnt + 1)
		 {
			 strTemp.Format(_T("%d"), 0);
			 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strPanelCur, strTemp);
			 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strDefCur, strTemp);
		 }

		 nPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, strPanelCur, 0);
		 strTemp.Format(_T("%d"), nPanelOverlapCnt + 1);
		 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strPanelCur, strTemp);

		 nOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, strDefCur, 0);

		 if (nUserDefCnt <= nOverlapCnt + 1  && pFrame->m_stOverlapCoordInfo->nPanelCnt <= nPanelOverlapCnt + 1)
		 {
			 CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
			 CString strSendMsg = _T("");
			 strSendMsg.Append(strAlarmMsg);
			 strSendMsg.Append(_T("#"));
			 strSendMsg.Append(strPanelID);
			 strSendMsg.Append(_T("#"));

			 va_list vaList;
			 va_start(vaList, strSendMsg);
			 int len = _vscwprintf( strSendMsg, vaList ) + 1;
			 wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
			 wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
			 memset(cBuffer,0,sizeof(wchar_t)*len);
			 memset(cTempBuff,0,sizeof(wchar_t)*len);
			 if (cBuffer)
			 {
				 wcscpy((WCHAR*)cBuffer, strSendMsg);
				 memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);	

				 // Pol Mark 와 Defect 보고 형태가 같아 Msg 공유	
				 pView->SendMessage(WM_UI_NEW_POL_MARK_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff ); 	

				 delete[] cBuffer;
			 }		

			 strTemp.Format(_T("%d"), 0);
			 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strDefCur, strTemp);
			 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strPanelCur, strTemp);
		 }
		 else
		 {
			 strTemp.Format( _T("%d"), nOverlapCnt + nDefCnt );
			 pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, strDefCur, strTemp);
		 }
	 }
 }
 
void CDataManager::m_fnSearchOverlapAlignMark(CString strFilePath, CString strPanelID)
{
	CStdioFile cstFile;
	CString strTemp = _T("");
	CString strLine, strItem, strPanelData;
	int nAlignFailOverlapCnt = 0;
	int nAlignFailPanelOverlapCnt = 0;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3,2, _T("m_fnSearchOverlapAlignMark File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem == _T("PANELDATA"))
		{			
			strPanelData = strLine;
		}

	}
	cstFile.Close();

	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE && pFrame->m_stOverlapCoordInfo->bUseAlignFail == TRUE && m_fnGetItemData(5, strPanelData) == ALIGN_FAIL)
	{
		nAlignFailPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT, 0);

		if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nAlignFailPanelOverlapCnt + 1)
		{
			strTemp.Format(_T("%d"), 0);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT, strTemp);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT, strTemp);
		}

		nAlignFailPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT, 0);
		strTemp.Format(_T("%d"), nAlignFailPanelOverlapCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT, strTemp);

		nAlignFailOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT, 0);

		if (pFrame->m_stOverlapCoordInfo->nAlignFaliDefectCount <= nAlignFailOverlapCnt + 1  && 
			pFrame->m_stOverlapCoordInfo->nPanelCnt <= nAlignFailPanelOverlapCnt + 1)
		{
			CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
			CString strSendMsg = _T("");
			strSendMsg.Append(_T("Inspection Align Fail"));
			strSendMsg.Append(_T("#"));
			strSendMsg.Append(strPanelID);
			strSendMsg.Append(_T("#"));

			va_list vaList;
			va_start(vaList, strSendMsg);
			int len = _vscwprintf( strSendMsg, vaList ) + 1;
			wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
			wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
			memset(cBuffer,0,sizeof(wchar_t)*len);
			memset(cTempBuff,0,sizeof(wchar_t)*len);
			if (cBuffer)
			{
				wcscpy((WCHAR*)cBuffer, strSendMsg);
				memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);	

				// Pol Mark 와 Defect 보고 형태가 같아 Msg 공유	
				pView->SendMessage(WM_UI_NEW_POL_MARK_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff ); 	

				delete[] cBuffer;
			}		


			strTemp.Format(_T("%d"), 0);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT, strTemp);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_PANEL_CURRENT_COUNT, strTemp);
		}
		else
		{
			strTemp.Format(_T("%d"), nAlignFailOverlapCnt + 1);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_ALIGN_FAIL_CURRENT_COUNT, strTemp);
		}
	}
}

void CDataManager::m_fnSearchOverlapPolMiss(CString strFilePath, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnSearchOverlapPolMiss() - Start"));
	CStdioFile cstFile;
	CString strTemp = _T("");
	CString strLine, strItem, strPanelData;
	int nPolMissOverlapCnt = 0;
	int nPolMissPanelOverlapCnt = 0;
	STRU_POL_MARK_INFO stPolMarkInfo;	//2017.03.21
	CString strCfResult, strTftResult, strCfPolMakerNo, strTftPolMakerNo;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	int nEqpType = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);	//0:AVL 1:ABI

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnSearchOverlapPolMiss File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem == _T("PANELDATA"))
		{			
			strPanelData = strLine;
		}

	}
	cstFile.Close();

	//if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE && pFrame->m_stOverlapCoordInfo->bUsePolMiss == TRUE && m_fnGetItemData(9, strPanelData) == POLMARKILEGAL_NG)

	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE && pFrame->m_stOverlapCoordInfo->bUsePolMiss == TRUE)
	{
		nPolMissPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, 0);

		if (pFrame->m_stOverlapCoordInfo->nPanelCnt < nPolMissPanelOverlapCnt + 1)
		{
			strTemp.Format(_T("%d"), 0);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, strTemp);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, strTemp);
		}

		nPolMissPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, 0);
		strTemp.Format(_T("%d"), nPolMissPanelOverlapCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, strTemp);

		if(pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && nEqpType == EQP_TYPE_ACS)
		{
			stPolMarkInfo = m_fnReadPolMarkData(strPanelID);
			strCfResult = stPolMarkInfo.cCFResult;		strCfResult.Trim();
			strTftResult = stPolMarkInfo.cTFTResult;	strTftResult.Trim();

			AfxExtractSubString(strCfPolMakerNo, strCfResult, 1, '_');
			AfxExtractSubString(strTftPolMakerNo, strTftResult, 1, '_');

			if(strCfResult == POLMARKILEGAL_ABI_NG || strTftResult == POLMARKILEGAL_ABI_NG)
			{
				nPolMissOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, 0);
				strTemp.Format(_T("%d"), nPolMissOverlapCnt + 1);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, strTemp);
			}
			else if(strCfPolMakerNo != strTftPolMakerNo)
			{
				nPolMissOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, 0);
				strTemp.Format(_T("%d"), nPolMissOverlapCnt + 1);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, strTemp);
			}
		}
		else
		{
			if(m_fnGetItemData(9, strPanelData) == POLMARKILEGAL_NG)
			{
				nPolMissOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, 0);
				strTemp.Format(_T("%d"), nPolMissOverlapCnt + 1);
				pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, strTemp);
			}

		}

		nPolMissPanelOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, 0);
		nPolMissOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, 0);

		if (pFrame->m_stOverlapCoordInfo->nPolMissDefectCount <= nPolMissOverlapCnt && 
			pFrame->m_stOverlapCoordInfo->nPanelCnt >= nPolMissPanelOverlapCnt)
		{
			m_fnPolMarkIlegalAlarm(_T("POL Mark ILEGAL !!"), strPanelID);

			strTemp.Format(_T("%d"), 0);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_CURRENT_COUNT, strTemp);
			pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP, KEY_OVERLAP_POL_MISS_PANEL_CURRENT_COUNT, strTemp);
		}
	}

}

//2016.03.02
void CDataManager::m_fnSearchOverlapLuminance(CString strFilePath, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnSearchOverlapLuminance() - Start"));
	CString strLine, strItem, strPanelData, strMsg, strLineLuminance, strPolLuminance;
	CStdioFile cstFile;	
	int nCurOverlapPanelCnt = 0, nCurOverlapCnt = 0, nCurPos = 0, nLimitCnt = 0;
	CString strTemp = _T("");
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	FILE *out = NULL;
	BOOL bLimitOver = FALSE, bPolLimitOver = FALSE;
	STRU_LARGE_RECEIVE_LUMINANCE_DATA stLuminanceData;	stLuminanceData.init_info();
	int nTempLuminance = 0;
	CString strVal = _T("");

	int nEqpType = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0);	//0:AVL 1:ABI

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnSearchOverlapLuminance File Open Error %s : %d"), strFilePath, GetLastError());
		return;
	}

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
		{
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("CF75"))
			{
				strItem = strLine;
				if(nEqpType == EQP_TYPE_AVL)
					strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 4);
				else
					strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_1, 4);
				strLineLuminance.Append(strItem);		
			}	
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("TFT75"))
			{
				strItem = strLine;
				if(nEqpType == EQP_TYPE_AVL)
					strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 4);
				else
					strItem = m_fnGetMeasureData(strItem.Mid(strItem.Find(EXT_COLON) + 1), VISION_STAGE_2, 4);
				strLineLuminance.Append(strItem);		
			}
			if(m_fnGetItemData(LAYER_NO - 1, strLine) == _T("POL76"))
			{
				strItem = strLine;
				strItem = strItem.Mid(strItem.Find(EXT_COLON)+1);
				strLineLuminance.Append(strItem+EXT_COLON);			
			}
		}	

	}
	cstFile.Close();

	if (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE)
	{
		stLuminanceData = m_fnReadLuminanceData(strPanelID);

		for(int i = 0; i < MAX_LUMINANCE_DATA; i++)
		{
			nTempLuminance = stLuminanceData.nLuminanceLimitData[i];

			if(i == 6 || i == 7)
				nTempLuminance = (stLuminanceData.nLuminanceLimitData[i] + stLuminanceData.nLuminanceLimitData[i+1]) / 2;

			strVal.Format(_T("%d"),nTempLuminance);
			strLineLuminance.Append(strVal + EXT_COLON);
		}

	}
	if(nEqpType == EQP_TYPE_ABI && pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
	{
		//		int nTmpLumi = pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[8];
		//		for(int i = 8 ; i >4 ; i--)
		//			pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[i] = pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[i-1];
		//		pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[4] = nTmpLumi;
		pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[2] = 0;
		pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[3] = 0;
		pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[6] = 0;
		pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[7] = 0;

		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[7], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[8]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[0], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[4]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[1], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[5]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[2], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[6]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[3], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[7]);
		//pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[8] = 0;

	}
	CString resTokenCount= strLineLuminance.Tokenize(EXT_COLON, nCurPos);
	while (resTokenCount != "")
	{
		if(nLimitCnt >= MAX_LUMINANCE_DATA)
			break;

		if(((nEqpType == EQP_TYPE_ABI) && (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE) && (nLimitCnt == 3)))
		{
			strPolLuminance = resTokenCount;
			if(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[nLimitCnt] - pFrame->m_stOverlapLuminanceInfo->nLuminanceRange > _ttoi(resTokenCount))
			{
				bPolLimitOver = TRUE;
			}
		}
		else
		{
			if((pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[nLimitCnt] - pFrame->m_stOverlapLuminanceInfo->nLuminanceRange > _ttoi(resTokenCount)) ||
				(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[nLimitCnt] + pFrame->m_stOverlapLuminanceInfo->nLuminanceRange < _ttoi(resTokenCount))) // 16.04.18 sgkim
			{
				bLimitOver = TRUE;
			}
		}		
		resTokenCount= strLineLuminance.Tokenize(EXT_COLON, nCurPos);
		nLimitCnt++;
	}

	nCurOverlapCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT, 0);
	nCurOverlapPanelCnt = pFrame->m_iniConfigSet.GetProfileIntW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT_PANEL, 0);


	strTemp.Format(_T("%d"), nCurOverlapPanelCnt + 1);
	pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT_PANEL, strTemp);
	nCurOverlapPanelCnt = nCurOverlapPanelCnt + 1;

	if(bLimitOver || bPolLimitOver)
	{
		strTemp.Format(_T("%d"), nCurOverlapCnt + 1);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT, strTemp);
		nCurOverlapCnt = nCurOverlapCnt + 1;
	}

	if (pFrame->m_stOverlapLuminanceInfo->nLuminanceAlarmSetCount <= nCurOverlapCnt &&
		pFrame->m_stOverlapLuminanceInfo->nLuminanceAlarmSetPanelCount >= nCurOverlapPanelCnt)
	{
		CString strTemp = _T("");
		strMsg.Format(_T("%s;"),strLineLuminance);
		strTemp = _T("Luminance Limit Over");

		if(bPolLimitOver)
		{
			strTemp = _T("POL Luminance Limit Over");
			strMsg.Format(_T("%s;"),strPolLuminance);
		}
		else if(nEqpType == EQP_TYPE_ABI && pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
		{
			int nDel = strMsg.Find(strPolLuminance);
			if(nDel > -1)
				strMsg.Delete(nDel, strPolLuminance.GetLength()+1);
		}
		m_fnLuminanceAlarmSet(strTemp, strMsg, strPanelID);

		strTemp.Format(_T("%d"), 0);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT, strTemp);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT_PANEL, strTemp);
	}
	else if(pFrame->m_stOverlapLuminanceInfo->nLuminanceAlarmSetPanelCount < nCurOverlapPanelCnt)
	{
		strTemp.Format(_T("%d"), 0);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT, strTemp);
		pFrame->m_iniConfigSet.WriteProfileStringW(SECTION_OVERLAP_LUMINANCE, KEY_LUMINANCE_CURRENT_COUNT_PANEL, strTemp);
	}
	if(nEqpType == EQP_TYPE_ABI && pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
	{

		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[0], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[4]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[1], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[5]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[2], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[6]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[3], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[7]);
		Swap(pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[7], pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[8]);
		//int nTmpLumi = pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[4];
		//for(int i = 5 ; i <8 ; i++)
		//	pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[i-1] = pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[i];
		//pFrame->m_stOverlapLuminanceInfo->nLuminanceLimitData[8] = nTmpLumi;
	}

	G_AddLog(3, 3, _T("m_fnSearchOverlapLuminance() - End"));
}

CString CDataManager::m_fnGetLuminanceTypeName(int nLumiType)
{
	G_AddLog(3, 3, _T("m_fnGetLuminanceTypeName() - Start"));
	CString strLuminanceTypeName;

	switch(nLumiType)
	{
	case CF_FORWARD_SIDE		  :    strLuminanceTypeName = _T("CF_FORWARD_SIDE");
	case CF_FORWARD_COAXIAL		  :	   strLuminanceTypeName = _T("CF_FORWARD_COAXIAL");
	case CF_BACKWARD_SIDE		  :	   strLuminanceTypeName = _T("CF_BACKWARD_SIDE");
	case CF_BACKWARD_COAXIAL	  :	   strLuminanceTypeName = _T("CF_BACKWARD_COAXIAL");
	case TFT_FORWARD_SIDE		  :	   strLuminanceTypeName = _T("TFT_FORWARD_SIDE");
	case TFT_FORWARD_COAXIAL	  :	   strLuminanceTypeName = _T("TFT_FORWARD_COAXIAL");
	case TFT_BACKWARD_SIDE		  :	   strLuminanceTypeName = _T("TFT_BACKWARD_SIDE");
	case TFT_BACKWARD_COAXIAL	  :	   strLuminanceTypeName = _T("TFT_BACKWARD_COAXIAL");
	}
	G_AddLog(3, 3, _T("m_fnGetLuminanceTypeName() - End"));
	return strLuminanceTypeName;
}

void CDataManager::m_fnLuminanceAlarmSet(CString strCaption, CString strMag, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnLuminanceAlarmSet() - Start"));
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewAuto*  pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	CString strSendMsg = _T("");
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strCaption);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strMag);
	strSendMsg.Append(_T("#"));
	strSendMsg.Append(strPanelID);
	strSendMsg.Append(_T("#"));
	va_list vaList;
	va_start(vaList, strSendMsg);
	int len = _vscwprintf( strSendMsg, vaList ) + 1;
	wchar_t *cBuffer	= new wchar_t[sizeof(wchar_t)*len];
	wchar_t *cTempBuff	= new wchar_t[sizeof(wchar_t)*len];
	memset(cBuffer,0,sizeof(wchar_t)*len);
	memset(cTempBuff,0,sizeof(wchar_t)*len);

	if (cBuffer)
	{
		wcscpy((WCHAR*)cBuffer, strSendMsg);
		memcpy(cTempBuff, cBuffer, sizeof(wchar_t)*len);
		pView->SendMessage(WM_UI_NEW_LUMINANCE_MSG_DIALOG, (WPARAM)len, (LPARAM)cTempBuff);
		delete[] cBuffer;
	}
	G_AddLog(3, 3, _T("m_fnLuminanceAlarmSet() - End"));
}

void CDataManager::m_fnLuminanceSet(CString strLine, CString strPanelID)
{
	G_AddLog(3, 3, _T("m_fnLuminanceSet() - Start"));
	CString strLumiLine = _T("");
	CString strLumiVal = _T("");
	int nLumiVal = 0, nLumiVision = 0, nCurPos = 0;

	strLumiLine = m_fnGetMeasureData(strLine.Mid(strLine.Find(EXT_COLON) + 1), 0, 2);
	strLumiVal = strLumiLine.Tokenize(EXT_COLON, nCurPos);
	nLumiVal = _ttoi(strLumiVal);
	strLumiVal = strLumiLine.Tokenize(EXT_COLON, nCurPos);
	nLumiVision = _ttoi(strLumiVal);

	m_fnWriteLuminanceData(strPanelID, nLumiVision - 1, nLumiVal);
	G_AddLog(3, 2, _T("Luminance Data : Vision[%d]  PanelID : %s Luminance : %d"), nLumiVision, strPanelID, nLumiVal);

}

STRU_LARGE_RECEIVE_LUMINANCE_DATA CDataManager::m_fnReadLuminanceData(CString strPanelId)     
{ 
	G_AddLog(3, 3, _T("m_fnReadLuminanceData() - Start"));
	FILE *in = NULL;
	int nCnt = 0;
	CString strVal, strLuminanceDataPath;
	STRU_LARGE_RECEIVE_LUMINANCE_DATA stLuminanceData;	stLuminanceData.init_info();
	errno_t errFileOpen;

	strLuminanceDataPath.Format(_T("%s%s"), ROOT_PATH , LUMINANCE_DATA_PATH);

	errFileOpen = _tfopen_s(&in, strLuminanceDataPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_stLuminanceData, sizeof(m_stLuminanceData), 1, in);			
		fclose(in);
	}	

	while(nCnt < MAX_REV_PANEL * 2)
	{
		strVal = m_stLuminanceData[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelId)
		{				
			stLuminanceData = m_stLuminanceData[nCnt];
			break;
		}		
		nCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnReadLuminanceData() - End"));
	return stLuminanceData;
} 

void CDataManager::m_fnWriteLuminanceData(CString strPanelid, int nVision, int nLuminance) 
{ 
	G_AddLog(3, 3, _T("m_fnWriteLuminanceData() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strLuminanceDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strLuminanceDataPath.Format(_T("%s%s"), ROOT_PATH , LUMINANCE_DATA_PATH);

	errFileOpen = _tfopen_s(&out, strLuminanceDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stLuminanceData[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stLuminanceData[nCnt].cPanelID , strPanelid);
			m_stLuminanceData[nCnt].nLuminanceLimitData[nVision] = nLuminance;
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stLuminanceData[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stLuminanceData[nCnt].cPanelID , strPanelid);
				m_stLuminanceData[nCnt].nLuminanceLimitData[nVision] = nLuminance;	
				m_stLuminanceData[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stLuminanceData[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stLuminanceData[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stLuminanceData[nCnt].llStartTime ;
				ntimeInx = nCnt;
			}			
			nCnt++;
		}

		_tcscpy(m_stLuminanceData[ntimeInx].cPanelID , strPanelid);
		m_stLuminanceData[ntimeInx].nLuminanceLimitData[nVision] = nLuminance;		
		m_stLuminanceData[ntimeInx].llStartTime = llStartTime;

	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stLuminanceData, sizeof(m_stLuminanceData) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	
	G_AddLog(3, 3, _T("m_fnWriteLuminanceData() - End"));
}

//2016.02.01 Packing Defect
BOOL CDataManager::m_fnMakePackingUpadateFile(CString strFilePath, CString strPanelID, int PanelCnt)
{
	BOOL bRetVal = FALSE, bNgFlag = APP_OK;
	CString strRetVal, strTime, strVal;
	long lRet = -1;
	int nStepCnt = 0;
	int nSeqNoCnt = 0;
	CTime time = CTime::GetCurrentTime();
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CString sTempTst, strPanelPath;
	TCHAR chRead[255] = {0};
	memset(chRead,0X00,sizeof(chRead));			
	//2015.10.08
	CString strQmsBackUpFile = _T("");
	SYSTEMTIME	systime;
	CString strQmsBackupDataPath, strDate;
	//2015.10.08
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	STRU_NG_FLAG_INFO stNgDataTemp;
	m_strDefGrade1 = _T("");
	m_strdefJudge1 = _T("");
	m_strdefJudge2 = _T("");

	strTime = time.Format(_T("%Y%m%d"));
	strRetVal = strTime;
	strRetVal += EXTENSION_TMP;
	strVal = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strRetVal;

	G_AddLog(3, 2, _T("m_fnMakePackingUpadateFile STEP : %d Panel ID[%s] Make Defect Data"),++nStepCnt, strPanelID);

	bNgFlag = m_fnMakePackingData(strFilePath, strVal);
	if (bNgFlag)
	{
		m_fnNgJudgelAlarm(_T("NG Panel List!!"), strPanelID);
	}
	else
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PLC_NG_JUDGE, 0);

	// 2015.12.14 Add by CWH for Packing wait sequence
	BOOL bUseNgFlag = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_EQP_USE_NG_FLAG, 0);
	if (bUseNgFlag)
	{
		lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PACKING_PC_GRAB_RESULT, 1);
		G_AddLog(3, 2, _T("m_fnMakePackingUpadateFile - USE NG FLAG."));
	}

	G_AddLog(3, 3, _T("m_fnMakePackingUpadateFile STEP : %d Panel ID[%s] NG Flag Set = %d"),++nStepCnt, strPanelID, bNgFlag);
	CString strDefectSummaryDataPath;
	int nDeleteTerm = 0;
	CString strQmsBackUpDataPath;
	::GetLocalTime(&systime);
	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);

	strDefectSummaryDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetDefectSummaryDataPath();
	strQmsBackUpDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();	//2015.10.08
	pFrame->m_fnCreateFolder(strDefectSummaryDataPath + strDate);

	strRetVal.Format(_T("%s%s%s.txt"),strDefectSummaryDataPath, strDate, strPanelID );
	bRetVal = CopyFile(strFilePath, strRetVal, 0);
	if (bRetVal)	pFrame->m_fnUpdateGridDefectSummary(strRetVal);	
	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE)
	{
		m_fnSearchOverlapCoord(strRetVal,strPanelID); 
	}
	G_AddLog(3, 3, _T("m_fnMakeQmsUpadateFile STEP : %d Panel ID[%s] Update Defect List"),++nStepCnt, strPanelID);

	//////////////////////////////////////////////////////////////////////////////////
	/////////////////////////NG FLAG 처리 부//////////////////////////////////////////

	stNgDataTemp.init_info();
	_tcscpy(stNgDataTemp.cPanelID, strPanelID);		
	_tcscpy(stNgDataTemp.cGrade,  m_strDefGrade1);	
	_tcscpy(stNgDataTemp.cJudge1, m_strdefJudge1);	
	_tcscpy(stNgDataTemp.cJudge2, m_strdefJudge2);	
	_tcscpy(stNgDataTemp.cCimMeasureData, m_strCimMeasureDataTemp);
	stNgDataTemp.nNgFlag = bNgFlag;	

	if (m_strDefGrade1 == _T(""))	stNgDataTemp.DefaultData(strPanelID, APP_NG);

	m_fnWriteNgFlagList(stNgDataTemp);

	stNgDataTemp.init_info();
	//////////////////////////////////////////////////////////////////////////////////

#if THREAD_MODE
	bRetVal = m_fnDelNoUseFile(strFilePath, strPanelID);		
#else
	if (pFrame->m_stOverlapCoordInfo->bUseMode == TRUE)
	{
		strPanelPath = m_fnDelNoUseFile(strFilePath, strPanelID);
		m_fnSearchOverlapCoord(strPanelPath); 	// 미사용 파일 삭제
	}
	else
		m_fnDelNoUseFile(strFilePath, strPanelID);

	strRetVal = m_fnLastToken(strFilePath, _T("\\"));
	strFilePath = strFilePath.Left(strFilePath.GetLength() - strRetVal.GetLength());
	RemoveDirectory(strFilePath);

#endif // THREAD_MODE

	//2015.10.08
	::GetLocalTime(&systime);
	strDate.Format(_T("%4d\\%02d\\%02d\\"), systime.wYear, systime.wMonth, systime.wDay);

	strQmsBackupDataPath = ((CViewAuto*)pFrame->m_pViewAuto)->m_fnGetQmsBackUpDataPath();
	pFrame->m_fnCreateFolder(strQmsBackupDataPath + strDate);
	strQmsBackUpFile.Format(_T("%s_%s%s"),strTime, strPanelID, EXTENSION_TMP);
	CopyFile(strVal, strQmsBackupDataPath +  + strDate + strQmsBackUpFile, 0);
	//2015.10.08

	sTempTst.Format(QMS_IMAG_TEMP,strPanelID);

	RemoveDirectory(sTempTst);

	bRetVal = TRUE;
	return bRetVal;
}

int   CDataManager::m_fnMakePackingData(CString strFilePath, CString strQmsFilePath)
{
	BOOL bRetVal = FALSE, bNoUse = FALSE;
	BOOL bCreatDirFlg = FALSE;
	int nRetVal = 0;	//2015.09.17 add
	CStdioFile cstFile;
	int nKvCntVal = 0, nKhCntVal = 0;

	if (!cstFile.Open(strFilePath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnMakePackingData File Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}

	m_nCfNgCnt		= 0;
	m_nTftNgCnt		= 0;
	m_nPenetNgCnt	= 0;

	m_stDefType.initDefType();

	CStringArray strArrayData;
	CString strLine, strItem, strNewLine ,strTemp;
	CString strLineHeader, strLinePanel, strLineSideInfo, strLineDefect;
	CString strLineEdgeDist= _T(""), strLineLuminance = _T("");
	CString strLineLuminanceCF= _T(""), strLineLuminanceTFT= _T("");
	CString strLineEdgeDistCF= _T(""), strLineEdgeDistTFT= _T("");
	CString strSubSide, strIunsentPath = _T("");
	CString check_date= _T(""), strMesType = _T("");
	CString strVal = _T("");	
	int nDefCnt = 0, nMesDstCnt =0 ;
	long imgX = 0, imgY = 0;
	int curPos = 0 , nAlignToMarkCnt = 0 , nLumitCnt;
	CString strCnt ;
	BOOL bRet = FALSE;
	nAlignToMarkCnt = 1;
	nLumitCnt = 1;
	CString cDefectVision = _T(""), CPanelId, strImgFilePath, strFileName, strImgName, strQmsImgName, strRetVal;
	BOOL bimgRetVal = FALSE;
	int nImgRetCnt = 0;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();		
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CString strDefType;
	CString strInspStartDate, strInspStartTime, strPad = _T(""), strPadLeft, strPadTop, strPadRight, strPadBottom, strDevice;
	STRU_IMAGE_INFO stImgInf;

	// 2016.03.22 add by ikm - 후보군 기능 추가 - S
	CString strDefFlag = _T(""), strDefSizeX = _T(""), strDefSizeY = _T(""), strDefLumi = _T("");
	DOUBLE dTempVal1, dTempVal2, dDefThinkness, dDefLength, dDefDistance;
	int nDefFlag = 0, nDefLumi = 0, nDefType = 0;
	for (int i = 0; i < 28; i++)
		m_stPanelJudgeInfo[i].init_info();
	// 2016.03.22 add by ikm - 후보군 기능 추가 - E

	while(cstFile.ReadString(strLine))
	{
		if(strLine == "")	continue;

		strItem = strLine;
		strItem = strItem.Mid(strItem.Find(EXT_COLON) + 1);
		strItem = strItem.Left(strItem.Find(EXT_COLON));
		strItem.Trim();

		if(strItem ==  _T("HEADERDATA"))
		{
			strLineHeader = m_fnMakeQmsDataHead(strLine);
			strInspStartDate = m_fnGetItemData(4, strLine);
			strInspStartTime = m_fnGetItemData(5, strLine);
			strInspStartDate.Append(_T(" ") + strInspStartTime);
		}	

		if(strItem ==  _T("PANELDATA"))
		{
			strLinePanel = m_fnMakeQmsDataPanel(strLine);
			CPanelId = m_fnGetItemData(3, strLine);
			_tcscpy(stImgInf.tImgPnlID , CPanelId);

			strDevice = m_fnGetItemData(8, strLine);
			strPadLeft = m_fnGetItemData(10, strLine);
			if (strPadLeft == _T("Y"))		strPad.Append(_T("L"));
			else	strPad.Append(_T("0"));

			strPadTop = m_fnGetItemData(11, strLine);
			if (strPadTop == _T("Y"))		strPad.Append(_T("T"));
			else	strPad.Append(_T("0"));

			strPadRight = m_fnGetItemData(12, strLine);
			if (strPadRight == _T("Y"))		strPad.Append(_T("R"));
			else	strPad.Append(_T("0"));

			strPadBottom = m_fnGetItemData(13, strLine);
			if (strPadBottom == _T("Y"))	strPad.Append(_T("B"));
			else	strPad.Append(_T("0"));
		}	

		if(strItem ==  _T("DEFECT"))
		{
			cDefectVision = m_fnGetItemData(FLAG + 1, strLine);
			pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)_ttoi(cDefectVision), (LPARAM)(LPCTSTR)CPanelId);

			if(_ttoi(cDefectVision) == POL)  m_fnSetItemData(D_FLAG - 1 , strLine, _T("0"), 1 , D_ITEM_END - 1);

			if (_ttoi( m_fnGetItemData(D_DEFECT_NO - 1, strLine)) <= 999)
			{
				if (_ttoi(cDefectVision) == CF)	  m_stDefType.CfNgCnt++;//m_fnSetCfNgCnt();
				if (_ttoi(cDefectVision) == TFT)  m_stDefType.TftNgCnt++;//m_fnSetTftNgCnt();
				else							  m_stDefType.CfNgCnt++;

				// 2016.03.22 add by ikm - 후보군 기능 추가 - S
				strDefType  = m_fnGetItemData(DEFECT_TYPE + 1, strLine);
				strDefFlag  = m_fnGetItemData(FLAG + 1, strLine);
				strDefSizeX = m_fnGetItemData(IMG_SIZE_X + 1, strLine);
				strDefSizeY = m_fnGetItemData(IMG_SIZE_Y + 1, strLine);
				strDefLumi  = m_fnGetItemData(LUMINANCE + 1, strLine);
				dTempVal1   = _tstof((LPCTSTR)strDefSizeX);
				dTempVal2   = _tstof((LPCTSTR)strDefSizeY);

				if (dTempVal1 < dTempVal2)
				{
					dDefDistance = dTempVal2;
					dDefLength = dTempVal2;
					dDefThinkness = dTempVal1;
				}
				else
				{
					dDefDistance = dTempVal1;
					dDefLength = dTempVal1;
					dDefThinkness = dTempVal2;
				}

				nDefFlag = _tstoi((LPCTSTR)strDefFlag);
				nDefLumi = _tstoi((LPCTSTR)strDefLumi);
				nDefType = _tstoi((LPCTSTR)strDefType);
				m_fnPanelJudgeCheck(nDefType, nDefFlag, dDefDistance, dDefLength, dDefThinkness, nDefLumi);
				// 2016.03.22 add by ikm - 후보군 기능 추가 - E


				nKvCntVal ++;
				stImgInf.nDefType = _ttoi(m_fnGetItemData(DEFECT_TYPE + 1, strLine));
				stImgInf.nDefFlag = _ttoi(m_fnGetItemData(FLAG + 1, strLine));	//2018.01.22
				strImgName = m_fnGetItemData(DEFECT_IMG_NO + 1 , strLine);
				m_fnRenameQmsImgFile( stImgInf, strImgName, nKvCntVal);
				strImgFilePath = stImgInf.tImgPath;
				/*
				strRetVal = m_fnLastToken(strImgFilePath, _T("\\"));
				strImgFilePath = strImgFilePath.Left(strImgFilePath.GetLength() - strRetVal.GetLength());
				strRetVal = strImgName;*/

				//CString sTempTst;
				////strImgName = strImgFilePath + strRetVal;
				//sTempTst.Format(QMS_IMAG_TEMP _T("%s"),CPanelId,strImgName);

				//strRetVal = m_strQmsImgName;
				//strIunsentPath = strImgFilePath;
				//strImgFilePath = strImgFilePath + strRetVal;

				//bCreatDirFlg = pFrame->m_fnCreateFolder(strIunsentPath);
				//bimgRetVal = MoveFile(sTempTst/*strImgName*/, strImgFilePath);

				//if(bimgRetVal == FALSE)
				//{	
				//	while(nImgRetCnt < 3)
				//	{
				//		bCreatDirFlg = pFrame->m_fnCreateFolder(strIunsentPath);
				//		bimgRetVal = MoveFile(sTempTst, strImgFilePath);
				//		G_AddLog(3, 2, _T("m_fnMakeQmsData_AVL Retry image save %s to %s: %d"), sTempTst, strImgFilePath , GetLastError());
				//		if(!bimgRetVal)
				//		{
				//			ShellExecute(0, _T("open"), _T("cmd"), _T("/C copy ") + sTempTst, strImgFilePath, SW_HIDE);
				//			G_AddLog(3, 2, _T("m_fnMakeQmsData_AVL Retry image save_cmd %s to %s: %d"), sTempTst, strImgFilePath , GetLastError());
				//			if(FileExist(sTempTst))
				//				G_AddLog(3, 2, _T("%s FileExist"), sTempTst);
				//			else {G_AddLog(3, 2, _T("%s Do not FileExist"), sTempTst); break;}
				//			if(FileExist(strImgFilePath)) break;
				//		}
				//		nImgRetCnt ++; 
				//		
				//		//Sleep(100);
				//		if (bimgRetVal)	break;		
				//	}			
				//}	
				//nImgRetCnt = 0;

				strImgFilePath.Append(strImgName);
				strLineDefect += m_fnMakePackingDataDefect(strLine, nKvCntVal, strInspStartDate, CPanelId, strDevice, strPad);
				//strLineDefect += m_fnMakeQmsDataDefect_AVL(strLine, nKvCntVal);			
				//strLine = m_fnSetItemData(D_DEF_IMG_NO , strLine, m_strQmsImgName, m_strQmsImgName.GetLength(), D_ITEM_END - 1);
				strImgFilePath = m_fnSetItemMutiData(3 , strImgFilePath, _T("i_send") , 6, 8 - 1, _T("\\"));	
				strLine.Append(EXT_COLON); strLine.Append(strImgFilePath); 
				nDefCnt++;
			}			
		}	

		strArrayData.Add(strLine + _T("\n"));
	}
	cstFile.Close();

	if (!cstFile.Open(strFilePath, CFile::modeWrite | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnMakePackingData File Open Error %s : %d"), strFilePath , GetLastError());
		return FALSE;
	}
	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	strArrayData.RemoveAll();
	cstFile.Close();

	// 중복 좌표 함수 기입///////////////////////////////////////////////////////////////////////

	m_fnCompareOverlapDefTypeCheck( CPanelId, strFilePath, m_stDefType);
	/////////////////////////////////////////////////////////////////////////////////////////////

	CString strPanelID1, strPanelID2, cCountRoot = _T("") , cCountVal = _T("");
	int nGradeCnt;
	cCountRoot.Append(ROOT_PATH);
	cCountRoot.Append(CONFIG_COUNT_PATH);


	if (!nDefCnt)
	{		
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_OK, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_OK, cCountVal, cCountRoot);

		//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->m_fnUpdateJudgeInfo(_T("-1"), CPanelId);
		pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)-1, (LPARAM)(LPCTSTR)CPanelId);
	}
	else
	{
		nGradeCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_GRADE_NG, 0, cCountRoot);
		cCountVal.Format(_T("%d"), ++nGradeCnt);
		WritePrivateProfileString(PNL_COUNT_SET, KEY_GRADE_NG, cCountVal, cCountRoot);
	}

	//((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgDefInfo->m_fnUpdateDefType(m_stDefType);	
	pAuto->m_pDlgDefInfo->SendMessage(WM_UI_CONTROL_DEF_TYPE_DIS, (WPARAM)0, (LPARAM)(STRU_DEFECT_TYPE*)&m_stDefType);

	bRetVal = m_fnGetJudgeGrade_AVL(bNoUse, m_strDefGrade1, m_strdefJudge1);
	//2015.09.17 add
	if (m_strDefGrade1 == JUDGE_A)	nRetVal = NG_FLAG_A_GRADE;		//0 : A Grade
	else if (m_strDefGrade1 == JUDGE_B)	nRetVal = NG_FLAG_AVL_B_GRADE;	//5 : B Grade
	else if (m_strDefGrade1 == JUDGE_C)	nRetVal = NG_FLAG_C_GRADE;		//6 : C Grade
	else if (m_strDefGrade1 == JUDGE_D)	nRetVal = NG_FLAG_D_GRADE;	//7 : D Grade
	//m_strDefGrade1 = _T("A");
	//2015.09.17 add
	m_fnGetJudgeGrade_AVL(bNoUse, m_strDefGrade2, m_strdefJudge2);

	////////strLinePanel = m_fnSetItemData(3 , strLinePanel, m_strDefGrade1, m_strDefGrade1.GetLength() , P_ITEM_END - 1);
	////////strLinePanel = m_fnSetItemData(4 , strLinePanel, m_strdefJudge1, m_strdefJudge1.GetLength() , P_ITEM_END - 1);
	////////strLinePanel = m_fnSetItemData(5 , strLinePanel, m_strdefJudge2, m_strdefJudge2.GetLength() , P_ITEM_END - 1);

	//2015.09.10 Mr.Chu
	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultGrade(m_strDefGrade1);
	((CViewAuto*)pFrame->m_pViewAuto)->m_pDlgPanelJudge->SetResultDefectCodeJudge(m_strdefJudge1);

	pAuto->SendMessage(WM_UI_CONTROL_PNL_SUMMARY_LIST, (WPARAM)0, (LPARAM)(LPCTSTR)strQmsFilePath);
	//((CViewAuto*)pFrame->m_pViewAuto)->m_fnUpdatePanelSummary(strQmsFilePath);

	// 취득한 디펙 정보를 QMS Data 순서로 입력
	////////strArrayData.Add(strLineHeader + _T("\n"));
	////////strArrayData.Add(strLinePanel + _T("\n"));
	////////strArrayData.Add(strSubSide + _T("\n"));
	if (!FileExist(strQmsFilePath))
	{
		strLine = _T("ITEM,TIME,DEVICE,CELL_ID,PAD,JUDGE,START_X,START_Y,END_X,END_Y,DEFECT_TYPE,AREA,BOX_AREA,BOX_X_MAX,BOX_X_MIN,BOX_Y_MAX,BOX_Y_MIN,");
		strLine.Append(_T("CENTER_X,CENTER_Y,LENGTH,BREADTH,ELONGATION,COMPACTNESS,HOLECOUNT,FERET_MIN_ANGLE,FERET_MAX_ANGLE,"));
		strLine.Append(_T("FERET_MIN,FERET_MAX,FERET_MEAN,FERET_X,FERET_Y,FERET_ELONGATION,PIXEL_MAX,PIXEL_MEAN,PIXEL_MIN,PIXEL_SIGMA,PIXEL_SUM\n"));

		strArrayData.Add(strLine);
	}

	strArrayData.Add(strLineDefect);

	if (!cstFile.Open(strQmsFilePath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate |CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnMakePackingData File Write Error %s : %d"), strQmsFilePath, GetLastError());
		return FALSE;
	}

	cstFile.SeekToEnd();

	for(int i = 0; i < strArrayData.GetCount(); i++)
	{
		strLine = strArrayData.GetAt(i);
		cstFile.WriteString(strLine);
	}

	cstFile.Close();

	return nRetVal;	//2015.09.17 change bRetVal -> nRetVal
}

CString CDataManager::m_fnMakePackingDataDefect(CString &strLine, int KvCntVal, CString strInspTime, CString strPanelID, CString strDevice, CString strPAD)
{
	CString strRetVal;
	CString strTemp, check_data, strVal, strDefType, strDefNo, strDefCode, strDefFlag;

	CString cCoordStartX, cCoordStartY, cCoordEndX, cCoordEndY;
	int curPos, nDefCnt;
	long imgX = 0, imgY = 0;
	check_data = strLine ;//
	curPos = 0;

	strVal  = check_data.Tokenize(EXT_COLON, curPos);

	strTemp = _T("");
	nDefCnt = 0;

	m_fnGetDefectType(check_data);
	strLine = check_data;
	while (TRUE)
	{				

		int ntokenCnt = m_fnLastTokenCnt(check_data, EXT_COLON);
		if(nDefCnt >= ntokenCnt) break;
		strVal = check_data.Tokenize(EXT_COLON, curPos);

		if (!strVal.Compare(_T(" ")))
		{
			strVal = _T("");
		}

		switch (nDefCnt)
		{
		case 0:
			strTemp.Append(_T("DATA,"));
			strTemp.Append(strInspTime + EXT_COLON);
			strTemp.Append(strDevice + EXT_COLON);
			strTemp.Append(strPanelID + EXT_COLON);
			strTemp.Append(strPAD + EXT_COLON);
			strTemp.Append(_T("DEFECT,"));
			break;
		case COORD_START_X:
			cCoordStartX = strVal;
			break;
		case COORD_END_X:
			cCoordEndX = strVal;
			break;
		case COORD_START_Y:
			cCoordStartY = strVal;
			break;
		case COORD_END_Y:
			cCoordEndY = strVal;	
			strTemp.Append(cCoordStartX + EXT_COLON);
			strTemp.Append(cCoordStartY + EXT_COLON);
			strTemp.Append(cCoordEndX + EXT_COLON);
			strTemp.Append(cCoordEndY + EXT_COLON);
			break;
		case D_PACKING_CLASSIFY:	
			strTemp.Append(strVal + EXT_COLON);	 
			break;
		case D_PACKING_AREA:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_BOX_AREA: 
			strTemp.Append(strVal + EXT_COLON); 
			break;
		case D_PACKING_BOX_X_MAX: 
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case D_PACKING_BOX_X_MIN:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_BOX_Y_MAX:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_BOX_Y_MIN:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_CENTER_X:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_CENTER_Y:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_LENGTH:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_BREADTH:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_ELONGATION:
			strTemp.Append(strVal + EXT_COLON);;
			break;
		case D_PACKING_COMPACTNESS:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_HOLECOUNT:			
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_FERET_MIN_ANGLE:
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case D_PACKING_FERET_MAX_ANGLE:
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case D_PACKING_FERET_MIN:
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case D_PACKING_FERET_MAX: 
			strTemp.Append(strVal + EXT_COLON); 
			break;
		case D_PACKING_FERET_MEAN: 
			strTemp.Append(strVal + EXT_COLON);  
			break;
		case D_PACKING_FERET_X:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_FERET_Y:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_FERET_ELONGATION:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_PIXEL_MAX:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_PIXEL_MEAN:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_PIXEL_MIN:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_PIXEL_SIGMA:
			strTemp.Append(strVal + EXT_COLON);
			break;
		case D_PACKING_PIXEL_SUM:
			strTemp.Append(strVal);
			break;
		default:
			break;
		}

		if (nDefCnt == D_PACKING_ITEM_END - 1)
		{
			strTemp.Append(EXT_SEMICOL);
			strTemp.Append(_T("\n"));
			break;	
		}
		nDefCnt ++;				
	};
	return	strRetVal = strTemp;
}

void CDataManager::Swap(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}

///2016.05.28		Defect에 따른 Lower Eqp Bypass 유무 설정 
//기능 : NGFlag로 보낼 Bypass 정보를 읽는다.
BOOL CDataManager::m_fnGetLowerBypassNgFlag_AVL()
{
	G_AddLog(3, 3, _T("m_fnGetLowerBypassNgFlag_AVL() - Start"));
	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_stJudgeItemBypassInfo.initDefType();
	m_stJudgeItemBypassInfo.setInfoPath(PNL_JUDGE_BYPASS_ITEM, strRoot);
	m_stJudgeItemBypassInfo.GetDefInfo();

	if(m_stJudgeItemBypassInfo.CfCellBroken)
		if (m_stDefType.CfCellBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftCellBroken)
		if (m_stDefType.TftCellBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfBurr)
		if (m_stDefType.CfBurr)	
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftBurr)
		if (m_stDefType.TftBurr)
			return TRUE;


	if(m_stJudgeItemBypassInfo.CfBroken)
		if (m_stDefType.CfBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftBroken)
		if (m_stDefType.TftBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfCornerBroken)
		if (m_stDefType.CfCornerBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftCornerBroken)
		if (m_stDefType.TftCornerBroken)
			return TRUE;

	///////////////////////////////////////////////////////////////////////////////////////////////////

	//if(m_stJudgeItemBypassInfo.TftCornerBroken)
	//	if (m_stDefType.TftCornerBroken)
	//		return TRUE;

	//if(m_stJudgeItemBypassInfo.CfCornerBroken)
	//	if (m_stDefType.CfCornerBroken)
	//		return TRUE;


	//if(m_stJudgeItemBypassInfo.TftBroken)
	//	if (m_stDefType.TftBroken)
	//		return TRUE;

	//if(m_stJudgeItemBypassInfo.CfBroken)
	//	if (m_stDefType.CfBroken)
	//		return TRUE;

	//if(m_stJudgeItemBypassInfo.WanesBroken)
	//	if (m_stDefType.WanesBroken)
	//		return TRUE;

	if(m_stJudgeItemBypassInfo.TftWhiteBlur)
		if (m_stDefType.TftWhiteBlur)
			return TRUE;

	if(m_stJudgeItemBypassInfo.ItoPeel)
		if (m_stDefType.ItoPeel)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfWhiteBlur)
		if (m_stDefType.CfWhiteBlur)
			return TRUE;

	if(m_stJudgeItemBypassInfo.SocketScrach)
		if (m_stDefType.SocketScrach)
			return TRUE;

	if(m_stJudgeItemBypassInfo.SocketBurn)
		if (m_stDefType.SocketBurn)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftDimpleLine)
		if (m_stDefType.TftDimpleLine)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfDimpleLine)
		if (m_stDefType.CfDimpleLine)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftPimpleLine)
		if (m_stDefType.TftPimpleLine)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfPimpleLine)
		if (m_stDefType.CfPimpleLine)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftScrach)
		if (m_stDefType.TftScrach)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfScrach)
		if (m_stDefType.CfScrach)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftDimplePoint)
		if (m_stDefType.TftDimplePoint)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfDimplePoint)
		if (m_stDefType.CfDimplePoint)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftPimplePoint)
		if (m_stDefType.TftPimplePoint)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfPimplePoint)
		if (m_stDefType.CfPimplePoint)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfWavePattern)
		if (m_stDefType.CfWavePattern)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfPimpleCircle)
		if (m_stDefType.CfPimpleCircle)
			return TRUE;

	if(m_stJudgeItemBypassInfo.EtcLine)
		if (m_stDefType.EtcLine)
			return TRUE;

	if(m_stJudgeItemBypassInfo.EtcPoint)
		if (m_stDefType.EtcPoint)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftEdgeilegal)
		if (m_stDefType.TftEdgeilegal)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfEdgeilegal)
		if ( m_stDefType.CfEdgeilegal)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfMeasureFail)
		if (m_stDefType.CfMeasureFail)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftMeasureFail)
		if (m_stDefType.TftMeasureFail)
			return TRUE;

	if(m_stJudgeItemBypassInfo.Burr)
		if ( m_stDefType.Burr)
			return TRUE;

	//2016.05.11 Mr.Chu		Align Fail Grade 판별
	if(m_stJudgeItemBypassInfo.AlignFail)
		if ( m_stDefType.AlignFail)
			return TRUE;

	//2016.02.24 우선 순위 마지막
	if(m_stJudgeItemInfo.PolMiss)
		if (m_stDefType.PolMiss)
			return TRUE;
	G_AddLog(3, 3, _T("m_fnGetLowerBypassNgFlag_AVL() - End"));
	return FALSE;
}

BOOL CDataManager::m_fnGetLowerBypassNgFlag_ABI()
{
	G_AddLog(3, 3, _T("m_fnGetLowerBypassNgFlag_ABI() - Start"));

	CString strRoot;
	strRoot.Format(_T("%s%s"), ROOT_PATH,JUDGE_ITEM_SET_PATH );
	m_stJudgeItemBypassInfo.initDefType();
	m_stJudgeItemBypassInfo.setInfoPath(PNL_JUDGE_BYPASS_ITEM, strRoot);
	m_stJudgeItemBypassInfo.GetDefInfo();

	if(m_stJudgeItemBypassInfo.CfCellBroken)
		if (m_stDefType.CfCellBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftCellBroken)
		if (m_stDefType.TftCellBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfBurr)
		if (m_stDefType.CfBurr)	
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftBurr)
		if (m_stDefType.TftBurr)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfBroken)
		if (m_stDefType.CfBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftBroken)
		if (m_stDefType.TftBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfCornerBroken)
		if (m_stDefType.CfCornerBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftCornerBroken)
		if ( m_stDefType.TftCornerBroken)
			return TRUE;
	//
	if(m_stJudgeItemBypassInfo.ItoPeel)
		if ( m_stDefType.ItoPeel)
			return TRUE;

	if(m_stJudgeItemBypassInfo.Contact)
		if (m_strSemaGrade.Find(_T("ITO_NG")) != -1)
			return TRUE;
	//
	if(m_stJudgeItemBypassInfo.mura)
		if (m_stDefType.mura)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfMeasureFail)
		if (m_stDefType.CfMeasureFail)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TftMeasureFail)
		if (m_stDefType.TftMeasureFail)
			return TRUE;



	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S

	if(m_stJudgeItemBypassInfo.CFpolSubstance)						// Modified by KYH01
		if (m_stDefType.CFpolSubstance)
			return TRUE;

	if(m_stJudgeItemBypassInfo.TFTpolSubstance)						// Add by KYH01
		if (m_stDefType.TFTpolSubstance)
			return TRUE;

	if(m_stJudgeItemBypassInfo.CfPolBubble)
		if (m_stDefType.CfPolBubble)
			return TRUE;


	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E

	if(m_stJudgeItemBypassInfo.CfPolScrach)
		if (m_stDefType.CfPolScrach)
			return TRUE;

	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S
	if(m_stJudgeItemBypassInfo.TftPolBubble)
		if (m_stDefType.TftPolBubble)
			return TRUE;

	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E

	if(m_stJudgeItemBypassInfo.TftPolScrach)
		if (m_stDefType.TftPolScrach)
			return TRUE;

	if(m_stJudgeItemBypassInfo.WanesBroken)
		if (m_stDefType.WanesBroken)
			return TRUE;

	if(m_stJudgeItemBypassInfo.SocketScrach)
		if (m_stDefType.SocketScrach)
			return TRUE;

	if(m_stJudgeItemBypassInfo.SocketBurn)
		if (m_stDefType.SocketBurn)
			return TRUE;

	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - S
	//2015.06.04 add by IKM - S 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
	// 	if(m_stJudgeItemBypassInfo.PolMinor)
	// 		if (m_stDefType.PolMinor)
	// 			return TRUE;

	////2015.06.04 add by IKM - E 微小欠陥 미소결함, 猫爪ムラ 묘조무라 
	if(m_stJudgeItemBypassInfo.Burr)
		if (m_stDefType.Burr)
			return TRUE;

	//2015.09.14 edit by IKM - 이물, 기포, 미소 결함은 판정에서 제외 - E

	//2016.05.11 Mr.Chu		Align Fail Grade 판별
	if(m_stJudgeItemBypassInfo.AlignFail)
		if (m_stDefType.AlignFail)
			return TRUE;

	//2016.02.24 우선 순위 마지막
	if(m_stJudgeItemBypassInfo.PolMiss)
		if (m_stDefType.PolMiss)
			return TRUE;

	G_AddLog(3, 3, _T("m_fnGetLowerBypassNgFlag_ABI() - End"));
	return FALSE;
}

//2016.09.14	Mr.Chu		m_fnPanelCodeTest 함수
// 기능 : 고객사 요청으로 인한 대형 ABI 설비일 경우 Panel N개( 최대 4개 ) 중 일정한 X번째 Panel을 사용자 설정 Judge ( 보통 3399 )로 처리한다.
int CDataManager::m_fnPanelCodeTest(CString strJudge)
{
	BOOL bTestJudge = FALSE;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	//CTime tTime = CTime::GetCurrentTime();	//시간기능 사용
	//CTimeSpan elapsedTime;

	pFrame->m_DataManager.m_fnReadPanelCodeData(&m_stPanelCodeTest);

	if( !m_stPanelCodeTest.bTestActive )
	{
		m_stPanelCodeTest.init();
		return FALSE;
	}

 	//elapsedTime = tTime - pFrame->m_stPanelCodeTest.tTimmer;	//시간기능 사용

	//Panel 갯수가 Total 갯수와 같을경우 Panel 갯수 Counting 초기화 후 진행.
	if( m_stPanelCodeTest.nTotalPanel == m_stPanelCodeTest.nDatCnt)
		m_stPanelCodeTest.nDatCnt = NULL;

	if( m_stPanelCodeTest.nTotalPanel > m_stPanelCodeTest.nDatCnt ) 
		// && pFrame->m_stPanelCodeTest.nTimeLimit >= elapsedTime.GetSeconds() )				//시간기능 사용
	{
		m_stPanelCodeTest.nDatCnt++;

		if(strJudge == _T(""))
			strJudge = _T("A");

		if( m_stPanelCodeTest.nDatCnt == m_stPanelCodeTest.nFirstDat ) 
		{
			if(pAuto->m_pDlgPanelCodeTest != NULL)
				pAuto->m_pDlgPanelCodeTest->GetDlgItem(IDC_STATIC_FIRST_DAT)->SetWindowText(strJudge);

			m_stPanelCodeTest.nFirstDatJudge = _ttoi(strJudge);
			bTestJudge = TRUE;
		}
		if( m_stPanelCodeTest.nDatCnt == m_stPanelCodeTest.nSecondDat )
		{
			if(pAuto->m_pDlgPanelCodeTest != NULL)
				pAuto->m_pDlgPanelCodeTest->GetDlgItem(IDC_STATIC_SECOND_DAT)->SetWindowText(strJudge);

			m_stPanelCodeTest.nSecondDatJudge = _ttoi(strJudge);
			bTestJudge = TRUE;
		}
		if( m_stPanelCodeTest.nDatCnt == m_stPanelCodeTest.nThirdDat )
		{
			if(pAuto->m_pDlgPanelCodeTest != NULL)
				pAuto->m_pDlgPanelCodeTest->GetDlgItem(IDC_STATIC_THIRD_DAT)->SetWindowText(strJudge);

			m_stPanelCodeTest.nThirdDatJudge = _ttoi(strJudge);
			bTestJudge = TRUE;
		}
		if( m_stPanelCodeTest.nDatCnt == m_stPanelCodeTest.nFourthDat )
		{
			if(pAuto->m_pDlgPanelCodeTest != NULL)
				pAuto->m_pDlgPanelCodeTest->GetDlgItem(IDC_STATIC_FOURTH_DAT)->SetWindowText(strJudge);

			m_stPanelCodeTest.nFourthDatJudge = _ttoi(strJudge);
			bTestJudge = TRUE;
		}

		// Set Item
		if( m_stPanelCodeTest.bTestActive )
			m_stJudgeItemInfo.PanelCodeTest = TRUE;
		else
			m_stJudgeItemInfo.PanelCodeTest = FALSE;

		m_fnWritePanelCodeData(m_stPanelCodeTest);
		//

		if(bTestJudge)
			return m_stPanelCodeTest.nJudgeCode;
	}	

	return FALSE;
}

//2017.03.21
STRU_POL_MARK_INFO CDataManager::m_fnReadPolMarkData(CString strPanelId)     
{ 
	G_AddLog(3, 3, _T("m_fnReadPolMarkData() - Start"));
	FILE *in = NULL;
	int nCnt = 0;
	CString strVal, strPolMarknDataPath;
	STRU_POL_MARK_INFO stPolMarkInfo;
	errno_t errFileOpen;
	//	FILE* hAgPramFileHandle = NULL;

	strPolMarknDataPath.Format(_T("%s%s"), ROOT_PATH , POL_MARK_INFO_PATH);

	errFileOpen = _tfopen_s(&in, strPolMarknDataPath, _T("rb+"));
	size_t read_size = 0;

	if(errFileOpen == 0)
	{		
		read_size = fread(&m_stPolMarkInfo, sizeof(m_stPolMarkInfo), 1, in);			
		fclose(in);
	}	

	while(nCnt < MAX_REV_PANEL * 2)
	{
		strVal = m_stPolMarkInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelId)
		{				
			stPolMarkInfo = m_stPolMarkInfo[nCnt];
			break;
		}		
		nCnt ++;
	}
	G_AddLog(3, 3, _T("m_fnReadPolMarkData() - End"));
	return stPolMarkInfo;
} 

void CDataManager::m_fnWritePolMarkData(CString strPanelid, CString strPolMark, int nVisionNo) 
{ 
	G_AddLog(3, 3, _T("m_fnWritePolMarkData() - Start"));
	//////////////////////////////////////////////////////////////////////////////////
	FILE	*out = NULL;
	int		nCnt = 0;
	BOOL	bPanelUseChk = FALSE;
	CString strVal, strPolMarkDataPath;
	errno_t errFileOpen;
	SYSTEMTIME	systime;

	strPolMarkDataPath.Format(_T("%s%s"), ROOT_PATH , POL_MARK_INFO_PATH);

	errFileOpen = _tfopen_s(&out, strPolMarkDataPath, _T("wb"));
	size_t read_size = 0;
	LONGLONG llStartTime, llCmpTime;
	int		 ntimeInx = 0; 
	/////////////////////////////////////////////////////////////
	::GetLocalTime(&systime);

	llStartTime =  (systime.wYear% 2000)*100000000 + systime.wMonth*1000000 + systime.wDay*10000 + systime.wHour*100 + systime.wMinute;

	// 순차적으로 저장할 Panel id의 정보를 셋팅한다.
	while(nCnt < MAX_REV_PANEL * 2)
	{		
		strVal = m_stPolMarkInfo[nCnt].cPanelID; strVal.TrimRight();
		if (strVal == strPanelid)				
		{		
			_tcscpy(m_stPolMarkInfo[nCnt].cPanelID , strPanelid);
			if(nVisionNo == VISION_PC_2)
				_tcscpy(m_stPolMarkInfo[nCnt].cCFResult , strPolMark);
			else if(nVisionNo == VISION_PC_5)
				_tcscpy(m_stPolMarkInfo[nCnt].cTFTResult , strPolMark);
			bPanelUseChk = TRUE;
			break;
		}
		nCnt++;
	}

	if (!bPanelUseChk)
	{
		nCnt = 0;
		while(nCnt < MAX_REV_PANEL * 2)
		{
			strVal = m_stPolMarkInfo[nCnt].cPanelID; strVal.TrimRight();
			if (strVal == _T("") || strVal == _T("0") || strVal.IsEmpty())
			{
				_tcscpy(m_stPolMarkInfo[nCnt].cPanelID , strPanelid);
				if(nVisionNo == VISION_PC_2)
					_tcscpy(m_stPolMarkInfo[nCnt].cCFResult , strPolMark);
				else if(nVisionNo == VISION_PC_5)
					_tcscpy(m_stPolMarkInfo[nCnt].cTFTResult , strPolMark);
				m_stPolMarkInfo[nCnt].llStartTime = llStartTime;
				bPanelUseChk = TRUE;
				break;
			}			
			nCnt++;
		}
	}	

	if (!bPanelUseChk)
	{
		nCnt = 0;
		llCmpTime  = m_stPolMarkInfo[0].llStartTime ;
		while(nCnt < MAX_REV_PANEL * 2)
		{			
			if (m_stPolMarkInfo[nCnt].llStartTime < llCmpTime )
			{
				llCmpTime = m_stPolMarkInfo[nCnt].llStartTime ;
				ntimeInx = nCnt;
			}			
			nCnt++;
		}

		_tcscpy(m_stPolMarkInfo[ntimeInx].cPanelID , strPanelid);
		if(nVisionNo == VISION_PC_2)
			_tcscpy(m_stPolMarkInfo[ntimeInx].cCFResult , strPolMark);
		else if(nVisionNo == VISION_PC_5)
			_tcscpy(m_stPolMarkInfo[ntimeInx].cTFTResult , strPolMark);
		m_stPolMarkInfo[ntimeInx].llStartTime = llStartTime;

	}	

	if(errFileOpen == 0)
	{		
		fwrite(&m_stPolMarkInfo, sizeof(m_stPolMarkInfo) , 1, out);		
		fclose(out);
		Sleep(50);		//100->50
	}	
	G_AddLog(3, 3, _T("m_fnWritePolMarkData() - End"));
}

BOOL CDataManager::m_fnWriteModel(long lModelNo, CString strModelName, CString strPlcData)
{
	CStdioFile cstFile;
	CFileFind	fileFind;
	CString strPlcModelFileName, strPlcModelDirPath, strPlcModelFullPath;
	BOOL bRet = TRUE;

	strPlcModelFileName.Format(_T("%d_%s%s"), lModelNo, strModelName, EXTENSION_DAT);
	strPlcModelDirPath.Format(_T("%s%s"), ROOT_PATH , PLC_MODEL_PATH);
	strPlcModelFullPath.Format(_T("%s%s"), strPlcModelDirPath, strPlcModelFileName);

	CreateDirectory(strPlcModelDirPath, NULL);

	if (!cstFile.Open(strPlcModelFullPath, CFile::modeWrite | CFile::modeCreate | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnWriteModel File Open Error %s : %d"), strPlcModelFullPath, GetLastError());
		return FALSE;
	}

	cstFile.WriteString(strPlcData);
	cstFile.Close();

	return bRet;
}

CString CDataManager::m_fnReadModel(long lModelNo, CString strModelName)
{
	CStdioFile cstFile;
	CString strPlcModelFullPath, strPlcModelFilePath, strLocalData = _T(""), strTemp = _T("");

	strPlcModelFilePath.Format(_T("%d_%s%s"), lModelNo, strModelName, EXTENSION_DAT);
	strPlcModelFullPath.Format(_T("%s%s"), ROOT_PATH , PLC_MODEL_PATH + strPlcModelFilePath);
	

	if (!cstFile.Open(strPlcModelFullPath, CFile::modeRead | CFile::typeText, NULL))
	{
		G_AddLog(3, 2, _T("m_fnReadModel File Open Error %s : %d"), strPlcModelFullPath, GetLastError());
		return _T("");
	}

	while(cstFile.ReadString(strTemp)) 
	{
		if(strTemp == "")	continue;
			
		strLocalData = strTemp;
	}
	

	cstFile.Close();

	return strLocalData;
}

BOOL CDataManager::m_fnDeleteModel(long lModelNo)
{
	CString strPlcModelFullPath, strPlcModelFilePath;
	CString strModelName;
	CFileFind fileFind;
	BOOL bFind = FALSE, bRetVal = FALSE;

	strModelName = m_fnGetModelName(lModelNo);

	strPlcModelFilePath.Format(_T("%d_%s%s"), lModelNo, strModelName, EXTENSION_DAT);
	strPlcModelFullPath.Format(_T("%s%s"), ROOT_PATH , PLC_MODEL_PATH + strPlcModelFilePath);

	bRetVal = DeleteFile(strPlcModelFullPath);

	return bRetVal;
}

BOOL CDataManager::m_fnCompareData(long lModelNo, CString strModelName, CString strAddr, int nSize)
{
	CString strLocalData, strPlcData, strTemp;
	TCHAR tszAddr[255];
	BOOL bRet = FALSE;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	strLocalData = _T("");	strPlcData = _T("");  strTemp = _T("");
	int m_Count = 0;
	long lRet;
	int nStart = 0;

	//1:PC 2:PLC
	if (nSize == PC_MODEL_BUFFER)
		nStart = 5376;
	else
		nStart = 6656;

	strLocalData = m_fnReadModel(lModelNo, strModelName);

	if(strLocalData != _T(""))
	{
		int nTemp = 0;
		for(int nParamCnt = 0; nParamCnt < PLC_MODEL_PARAM_COUNT; nParamCnt++)
		{
			lRet -1;
			nTemp = nStart + nParamCnt;
			memset(tszAddr, 0, sizeof(tszAddr));
			_stprintf_s(tszAddr, 255, _T("W%d"), nTemp);
			lRet = pView->m_pDlgPLC->m_fnGetDeviceModel(tszAddr);
			if(lRet != -1)
			{
				strTemp.Format(_T("%ld,"),lRet);
				strPlcData.Append(strTemp);
			}
		}

		if(strLocalData == strPlcData)
			bRet = TRUE;
	}

	return bRet;
}

BOOL CDataManager::m_fnCopyModel(long lSourModelNo, long lDestModelNo)
{
	CString strPlcModelSourFullPath, strPlcModelDestFullPath, strPlcModelFilePath, strLocalData, strToken, strTemp;
	CString strModelName;
	BOOL bRetVal = FALSE;
	long lPlcData[PLC_MODEL_PARAM_COUNT];
	int nCurPos, nPlcDataCnt;

	strModelName = m_fnGetModelName(lSourModelNo);

	strPlcModelFilePath.Format(_T("%d_%s%s"),lSourModelNo, strModelName, EXTENSION_DAT);
	strPlcModelSourFullPath.Format(_T("%s%s"), ROOT_PATH , PLC_MODEL_PATH + strPlcModelFilePath);
	strPlcModelFilePath.Format(_T("%d_%s%s"),lDestModelNo, strModelName, EXTENSION_DAT);
	strPlcModelDestFullPath.Format(_T("%s%s"), ROOT_PATH , PLC_MODEL_PATH + strPlcModelFilePath);

	bRetVal = CopyFile(strPlcModelSourFullPath, strPlcModelDestFullPath, 0);
	if(!bRetVal)	return bRetVal;

	//Cheng Du
	strLocalData = m_fnReadModel(lDestModelNo, strModelName);
	
	nCurPos = 0;	nPlcDataCnt = 0;
	strToken= strLocalData.Tokenize(EXT_COLON, nCurPos);
	while (strToken != "")
	{
		lPlcData[nPlcDataCnt] = _ttol(strToken);
		strToken= strLocalData.Tokenize(EXT_COLON, nCurPos);
		nPlcDataCnt++;
	}

	if(lPlcData[PLC_MODEL_PARAM_COUNT - 1] == lSourModelNo)
		lPlcData[PLC_MODEL_PARAM_COUNT - 1] = lDestModelNo;

	strLocalData = _T("");

	for(int nParamCnt = 0; nParamCnt < PLC_MODEL_PARAM_COUNT; nParamCnt++)
	{
		strTemp.Format(_T("%ld,"),lPlcData[nParamCnt]);
		strLocalData.Append(strTemp);
	}

	bRetVal = m_fnWriteModel(lDestModelNo, strModelName, strLocalData);

	return bRetVal;
}

CString CDataManager::m_fnGetModelName(long lModelNo)
{
	CString strPlcModelDataPath;
	CString strRetName, strFileName, strFileFullNameTmp, strFileNameTmp, strModelNo, strModelName;
	CFileFind finder;
	BOOL bWorking;
	TCHAR szTmp[256];

	ZeroMemory(szTmp, 256);
	strRetName = _T("");
	strPlcModelDataPath.Format(_T("%s%s"),ROOT_PATH , PLC_MODEL_PATH);
	bWorking = finder.FindFile(strPlcModelDataPath + _T("*.*"));

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots()) continue;

		strFileName = finder.GetFileName();
		if (strFileName.Find(EXTENSION_DAT))
		{
			strFileFullNameTmp = strFileName;

			//ZeroMemory(szTmp, 256);
			StrCpy(szTmp, strFileFullNameTmp);
			PathRemoveExtension(szTmp); 
			strFileNameTmp = szTmp;

			AfxExtractSubString(strModelNo, strFileNameTmp, 0, '_');
			AfxExtractSubString(strModelName, strFileNameTmp, 1, '_');

			if(lModelNo == _tstoi(strModelNo))
			{
				strRetName = strModelName;
				break;
			}

		}

	}
	
	return strRetName;
}

BOOL CDataManager::m_fnModelExist(long lModelNo)
{
	CString strPlcModelDataPath;
	CString strFileName, strFileFullNameTmp, strFileNameTmp, strModelNo;
	BOOL bRetVal = FALSE;
	CFileFind finder;
	TCHAR szTmp[256];
	BOOL bWorking;

	ZeroMemory(szTmp, 256);

	strPlcModelDataPath.Format(_T("%s%s"),ROOT_PATH , PLC_MODEL_PATH);
	bWorking = finder.FindFile(strPlcModelDataPath + _T("*.*"));

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots()) continue;

		strFileName = finder.GetFileName();
		if (strFileName.Find(EXTENSION_DAT))
		{
			strFileFullNameTmp = strFileName;

			//ZeroMemory(szTmp, 256);
			StrCpy(szTmp, strFileFullNameTmp);
			PathRemoveExtension(szTmp); 
			strFileNameTmp = szTmp;

			AfxExtractSubString(strModelNo, strFileNameTmp, 0, '_');

			if(lModelNo == _tstoi(strModelNo))
			{
				bRetVal = TRUE;
				break;
			}
		}

	}

	return bRetVal;
}

//2017.04.26	Mr.Chu
//기능 : Data Back Up To Network Drive
//변수 : strModelPath	- Original File
//		 nMode			- 0 (File Copy)		1 (Folder Copy)
BOOL CDataManager::m_fnBackUpNetworkDrive(CString strBackUpData, int nMode)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CString strPath_BackUp = _T(""), strPath = _T("");
	BOOL bRetVal = FALSE;
	TCHAR strBuffer[512];
	DWORD BufSize = 512;
	DWORD Result = NULL;
	NETRESOURCE nr;

	SYSTEMTIME	systime ;
	::GetLocalTime(&systime);

	memset(&nr, NULL, sizeof(NETRESOURCE));

	//Using NetWorkDrive Setting
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_IP, KEY_VISION_PC_1, _T(""), strBuffer, 255);	//PC 1

	strPath.Format(_T("\\\\%s\\shared"),strBuffer);
	strPath_BackUp.Format(_T("%s\\PLC_TeachingBack_%04d%02d%02d_%02d%02d%02d"),
		strPath,systime.wYear,systime.wMonth,systime.wDay,systime.wHour,systime.wMinute,systime.wSecond);

	wcscpy(strBuffer, (LPCTSTR)strPath);
	//


	//Using Login Type
	//Not Use... Login time So Long..

// 	nr.lpRemoteName = strBuffer;
// 	nr.lpLocalName  = _T("M:");               
// 	nr.dwType = RESOURCETYPE_DISK;
// 	nr.dwDisplayType = RESOURCEDISPLAYTYPE_SHARE;
// 	nr.dwScope = RESOURCE_REMEMBERED;
// 	nr.dwUsage = RESOURCEUSAGE_CONNECTABLE;
// 	nr.lpComment = NULL;
// 
//   	WNetCancelConnection2(nr.lpLocalName, CONNECT_UPDATE_PROFILE, FALSE);
//   
//   	if(WNetGetConnection(nr.lpLocalName, strBuffer, &BufSize) == ERROR_NOT_CONNECTED){
//   		// TRY Connection
//   		//Result = WNetAddConnection2(&nr, _T("1234"), _T("admin"), CONNECT_UPDATE_PROFILE); 
//  		Result = WNetAddConnection2(&nr, _T("ehdgus08"), _T("dhnam"), CONNECT_UPDATE_PROFILE); 
//   		if(Result == NO_ERROR){	// Connection TRUE
//   				
//   		}else{					// Connection FALSE
//   				
//   			return FALSE;
//   		}
//   	}else{						
//   		// Connected
//   	}
	//

	if (GetFileAttributes(strPath_BackUp) == -1)
	{
		CreateDirectory(strPath_BackUp, NULL);
	}	
	
	switch(nMode)
	{
	case 0:		//File Copy
		bRetVal = CopyFile(strBackUpData,strPath_BackUp,FALSE);
		break;
	case 1:		//Folder Copy
		bRetVal = m_fnCopyFolder(strBackUpData,strPath_BackUp);
		break;
	default:
		break;
	}
	

	return bRetVal;
}

//2017.04.26
//기능 : m_fnCopyFolder
//변수 : strFrom	- Original Folder
//		 strTo		- Copy Folder
BOOL CDataManager::m_fnCopyFolder(CString strFrom, CString strTo)
{
	SHFILEOPSTRUCT FileOp	= {0};
	int nRetVal				= 0;
	TCHAR* szTempFrom			= new TCHAR[strFrom.GetLength()+1];
	TCHAR* szTempTo				= new TCHAR[strTo.GetLength()+1];

	wcscpy_s(szTempFrom, strFrom.GetLength() + 1, strFrom);
	szTempFrom[strFrom.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.
	wcscpy_s(szTempTo, strTo.GetLength() + 1, strTo);
	szTempTo[strTo.GetLength() + 1] = NULL;				// NULL문자가 두개 들어가야 한다.

	FileOp.hwnd = AfxGetMainWnd()->m_hWnd;
	FileOp.wFunc = FO_COPY;
	FileOp.pTo = szTempTo;
	FileOp.pFrom = szTempFrom;
	FileOp.fFlags = FOF_SIMPLEPROGRESS	|	FOF_MULTIDESTFILES	|	FOF_SILENT	|	FOF_NOCONFIRMATION;
	FileOp.fAnyOperationsAborted	= false;
	FileOp.hNameMappings			= NULL;
	FileOp.lpszProgressTitle		= NULL;

	nRetVal = SHFileOperation(&FileOp);

	szTempFrom	= NULL;
	szTempTo	= NULL;
	delete[] szTempFrom;
	delete[] szTempTo;

	if(nRetVal == NULL)
		return TRUE;
	return nRetVal;
}

//2017.01.21
BOOL CDataManager::m_fnSerachPolAlienSubstance(CStringArray &strAryCoax, CStringArray &strAryTdi)
{
	CString strLineCoax, strLineTdi;
	CString strCoaxStartX, strCoaxEndX, strCoaxStartY, strCoaxEndY;
	CString strTdiStartX, strTdiEndX, strTdiStartY, strTdiEndY;
	DOUBLE dblCoaxStartX, dblCoaxEndX, dblCoaxStartY, dblCoaxEndY;
	DOUBLE dblTdiStartX, dblTdiEndX, dblTdiStartY, dblTdiEndY;
	BOOL bRetVal = FALSE;

	for (int nCoax = 0 ; nCoax < strAryCoax.GetCount() ; nCoax++)
	{
		strLineCoax = strAryCoax.GetAt(nCoax);
		strCoaxStartX = m_fnGetItemData(COORD_START_X + 1, strLineCoax);	dblCoaxStartX = _tstof((LPCTSTR)strCoaxStartX);
		strCoaxEndX = m_fnGetItemData(COORD_END_X + 1, strLineCoax);		dblCoaxEndX = _tstof((LPCTSTR)strCoaxEndX);
		strCoaxStartY = m_fnGetItemData(COORD_START_Y + 1, strLineCoax);	dblCoaxStartY = _tstof((LPCTSTR)strCoaxStartY);
		strCoaxEndY = m_fnGetItemData(COORD_END_Y + 1, strLineCoax);		dblCoaxEndY = _tstof((LPCTSTR)strCoaxEndY);

		for (int nTdi = 0 ; nTdi < strAryTdi.GetCount() ; nTdi++)
		{
			strLineTdi = strAryTdi.GetAt(nTdi);
			strTdiStartX = m_fnGetItemData(COORD_START_X + 1, strLineTdi);	dblTdiStartX = _tstof((LPCTSTR)strTdiStartX);
			strTdiEndX = m_fnGetItemData(COORD_END_X + 1, strLineTdi);		dblTdiEndX = _tstof((LPCTSTR)strTdiEndX);
			strTdiStartY = m_fnGetItemData(COORD_START_Y + 1, strLineTdi);	dblTdiStartY = _tstof((LPCTSTR)strTdiStartY);
			strTdiEndY = m_fnGetItemData(COORD_END_Y + 1, strLineTdi);		dblTdiEndY = _tstof((LPCTSTR)strTdiEndY);

			if(dblCoaxStartX < dblTdiStartX && dblCoaxEndX > dblTdiEndX && 
				dblCoaxStartY < dblTdiStartY && dblCoaxEndY > dblTdiEndY)
				return TRUE;
		}

	}

	return bRetVal;
}

//2018.01.18
void CDataManager::m_fnMakeQmsDataDefCnt_AVL(CString &strSubSide)
{
	int m_nCntTotal = 0;	//2020.09.01 add by mby

	m_nCntTotal = m_nArrayPaCntPDCount[0] + m_nArrayPaCntPDCount[1] + m_nArrayPaCntPDCount[2] + m_nArrayPaCntPDCount[3];	//All Count	//2020.09.01 add by mby

	//2021.04.21 add by mby
	CString PaCodeFlag = _T("0");	
	if(m_bDcc_Check1_flag){
		PaCodeFlag = _T("1");
	}else if(m_bDcc_Check2_flag){
		PaCodeFlag = _T("2");
	}


	//1. "30"고정
	strSubSide.Append(_T("30"));
	strSubSide.Append(EXT_COLON);

	//2. KV01결함수
	//strSubSide.Append(IntToCstr(m_stDefType.CfPimplePoint));
	strSubSide.Append(PaCodeFlag);	//2021.04.21 add by mby
	strSubSide.Append(EXT_COLON);

	//3. KV02결함수
	strSubSide.Append(IntToCstr(m_stDefType.TftPimplePoint));
	strSubSide.Append(EXT_COLON);

	//4. KV03결함수
	strSubSide.Append(IntToCstr(m_stDefType.CfPimpleLine));
	strSubSide.Append(EXT_COLON);

	//2020.09.01 add by mby S-
	//5. KV04결함수 - CF_A
	//strSubSide.Append(IntToCstr(m_stDefType.TftPimpleLine));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[0]));
	strSubSide.Append(EXT_COLON);

	//6. KV05결함수 - CF_B
	//strSubSide.Append(IntToCstr(m_stDefType.CfBurr));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[1]));
	strSubSide.Append(EXT_COLON);

	//7. KV06결함수 - CF_C
	//strSubSide.Append(IntToCstr(m_stDefType.TftBurr));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[2]));
	strSubSide.Append(EXT_COLON);

	//8. KV07결함수 - CF_D
	//strSubSide.Append(_T("0"));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[3]));
	strSubSide.Append(EXT_COLON);

	//9. KV08결함수 - TFT_A
	//strSubSide.Append(_T("0"));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[4]));
	strSubSide.Append(EXT_COLON);

	//10. KV09결함수 - TFT_B
	//strSubSide.Append(IntToCstr(m_stDefType.CfBroken));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[5]));
	strSubSide.Append(EXT_COLON);

	//11. KV10결함수 - TFT_C
	//strSubSide.Append(IntToCstr(m_stDefType.TftBroken));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[6]));
	strSubSide.Append(EXT_COLON);

	//12. KV11결함수 - TFT_D
	//strSubSide.Append(IntToCstr(m_stDefType.CfScrach));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[7]));
	strSubSide.Append(EXT_COLON);

	//13. KV12결함수 - CF_COR_A
	//strSubSide.Append(IntToCstr(m_stDefType.TftScrach));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[8]));
	strSubSide.Append(EXT_COLON);

	//14. KV13결함수 - CF_COR_B
	//strSubSide.Append(IntToCstr(m_stDefType.CfCornerBroken));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[9]));
	strSubSide.Append(EXT_COLON);

	//15. KV14결함수 - CF_COR_C
	//strSubSide.Append(IntToCstr(m_stDefType.TftCornerBroken));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[10]));
	strSubSide.Append(EXT_COLON);

	//16. KV15결함수 - CF_COR_D
	//strSubSide.Append(IntToCstr(m_stDefType.CfWhiteBlur));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[11]));
	strSubSide.Append(EXT_COLON);

	//17. KV16결함수 - TFT_COR_A
	//strSubSide.Append(IntToCstr(m_stDefType.TftWhiteBlur));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[12]));
	strSubSide.Append(EXT_COLON);

	//18. KV17결함수 - TFT_COR_B
	//strSubSide.Append(IntToCstr(m_stDefType.CfDimplePoint));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[13]));
	strSubSide.Append(EXT_COLON);

	//19. KV18결함수 - TFT_COR_C
	//strSubSide.Append(IntToCstr(m_stDefType.TftDimplePoint));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[14]));
	strSubSide.Append(EXT_COLON);

	//20. KV19결함수 - TFT_COR_D
	//strSubSide.Append(IntToCstr(m_stDefType.CfDimpleLine));
	strSubSide.Append(IntToCstr(m_nArrayPaCntEdgeCount[15]));
	strSubSide.Append(EXT_COLON);

	//21. KV20결함수 - CF_Total
	//strSubSide.Append(IntToCstr(m_stDefType.TftDimpleLine));
	strSubSide.Append(IntToCstr(m_nArrayPaCntPDCount[0]));
	strSubSide.Append(EXT_COLON);

	//22. KV21결함수 - TFT_Total
	//strSubSide.Append(_T("0"));
	strSubSide.Append(IntToCstr(m_nArrayPaCntPDCount[1]));
	strSubSide.Append(EXT_COLON);

	//23. KV22결함수 - CF_COR_Total
	//strSubSide.Append(_T("0"));
	strSubSide.Append(IntToCstr(m_nArrayPaCntPDCount[2]));
	strSubSide.Append(EXT_COLON);

	//24. KV23결함수 - TFT_COR_Total
	//strSubSide.Append(IntToCstr(m_stDefType.CfMeasureFail));
	strSubSide.Append(IntToCstr(m_nArrayPaCntPDCount[3]));
	strSubSide.Append(EXT_COLON);

	//25. KV24결함수 - ALL_Total
	//strSubSide.Append(IntToCstr(m_stDefType.TftMeasureFail));
	strSubSide.Append(IntToCstr(m_nCntTotal));
	strSubSide.Append(EXT_COLON);
	//2020.09.01 add by mby E-

	//26. KV25결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//27. KV26결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//28. KV27결함수
	strSubSide.Append(IntToCstr(m_stDefType.ItoPeel));
	strSubSide.Append(EXT_COLON);

	//29. KV28결함수
	strSubSide.Append(IntToCstr(m_stDefType.SocketScrach));
	strSubSide.Append(EXT_COLON);

	//30. KV29결함수
	strSubSide.Append(IntToCstr(m_stDefType.SocketBurn));
	strSubSide.Append(EXT_COLON);

	//31. KV30결함수
	strSubSide.Append(IntToCstr(m_stDefType.AlignFail));
	strSubSide.Append(EXT_COLON);

	//32. KV31결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//33. KV32결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//34. KV33결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//35. KV34결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//36. KV35결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//37. KV36결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//38. KV37결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//39. KV38결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//40. KV39결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//41. KV40결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//42. KV41결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//43. KV42결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//44. KV43결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//45. KV44결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//46. KV45결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//47. KO01결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//48. KO02결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//49. KO03결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//50. KO04결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//51. KO05결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);
}

void CDataManager::m_fnMakeQmsDataDefCnt_ABI(CString &strSubSide)
{
	//1. "30"고정
	strSubSide.Append(_T("30"));
	strSubSide.Append(EXT_COLON);

	//2. KP01결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//3. KP02결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//4. KP03결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//5. KP04결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//6. KP05결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//7. KP06결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//8. KP07결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//9. KP08결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//10. KP09결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//11. KP10결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//12. KP11결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//13. KP12결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//14. KP13결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//15. KP14결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//16. KP15결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//17. KP16결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//18. KP17결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//19. KP18결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//20. KP19결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//21. KP20결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//22. KP21결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//23. KP22결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//24. KP23결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//25. KP24결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//26. KP25결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//27. KP26결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//28. KP27결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//29. KP28결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//30. KP29결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//31. KP30결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//32. KS01결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//33. KS02결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//34. KS03결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//35. KS04결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//36. KS05결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//37. KS06결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//38. KS07결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//39. KS08결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//40. KS09결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//41. KS10결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//42. KS11결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//43. KS12결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//44. KS13결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//45. KS14결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//46. KS15결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//47. KS16결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//48. KS17결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//49. KS18결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//50. KS19결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//51. KS20결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//52. KC01결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//53. KC02결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//54. KC03결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//55. KC04결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//56. KC05결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//57. KC06결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//58. KC07결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//59. KC08결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//60. KC09결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//61. KC10결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//62. KH01결함수(Cf Pol Substance)
	if(m_stDefType.CFpolSubstance >=999)
		m_stDefType.CFpolSubstance = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CFpolSubstance));
	strSubSide.Append(EXT_COLON);

	//63. KH02결함수(Tft Pol Substance)
	strSubSide.Append(IntToCstr(m_stDefType.TFTpolSubstance));
	strSubSide.Append(EXT_COLON);

	//64. KH03결함수
	if(m_stDefType.CfPolBubble >= 999)
		m_stDefType.CfPolBubble = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfPolBubble));
	strSubSide.Append(EXT_COLON);

	//65. KH04결함수
	if(m_stDefType.TftPolBubble >= 999)
		m_stDefType.TftPolBubble = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftPolBubble));
	strSubSide.Append(EXT_COLON);

	//66. KH05결함수
	if(m_stDefType.CfPolScrach >= 999)
		m_stDefType.CfPolScrach = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfPolScrach));
	strSubSide.Append(EXT_COLON);

	//67. KH06결함수
	if(m_stDefType.TftPolScrach >= 999)
		m_stDefType.TftPolScrach = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftPolScrach));
	strSubSide.Append(EXT_COLON);

	//68. KH07결함수
	if(m_stDefType.PolMiss >= 999)
		m_stDefType.PolMiss = 999;

	strSubSide.Append(IntToCstr(m_stDefType.PolMiss));
	strSubSide.Append(EXT_COLON);

	//69. KH08결함수(KV23 동일 항목)
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//70. KH09결함수(KV24 동일 항목)
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//71. KH10결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//72. KV01결함수
	if(m_stDefType.CfPimplePoint >= 999)
		m_stDefType.CfPimplePoint = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfPimplePoint));
	strSubSide.Append(EXT_COLON);

	//73. KV02결함수
	if(m_stDefType.TftPimplePoint >= 999)
		m_stDefType.TftPimplePoint = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftPimplePoint));
	strSubSide.Append(EXT_COLON);

	//74. KV03결함수
	if(m_stDefType.CfPimpleLine >= 999)
		m_stDefType.CfPimpleLine = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfPimpleLine));
	strSubSide.Append(EXT_COLON);

	//75. KV04결함수
	if(m_stDefType.TftPimpleLine >= 999)
		m_stDefType.TftPimpleLine = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftPimpleLine));
	strSubSide.Append(EXT_COLON);

	//76. KV05결함수
	if(m_stDefType.CfBurr >=999 )
		m_stDefType.CfBurr = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfBurr));
	strSubSide.Append(EXT_COLON);

	//77. KV06결함수
	if(m_stDefType.TftBurr >= 999)
		m_stDefType.TftBurr = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftBurr));
	strSubSide.Append(EXT_COLON);

	//78. KV07결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//79. KV08결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//80. KV09결함수
	if(m_stDefType.CfBroken >= 999)
		m_stDefType.CfBroken = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfBroken));
	strSubSide.Append(EXT_COLON);

	//81. KV10결함수
	if(m_stDefType.TftBroken >= 999)
		m_stDefType.TftBroken = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftBroken));
	strSubSide.Append(EXT_COLON);

	//82. KV11결함수
	if(m_stDefType.CfScrach >= 999)
		m_stDefType.CfScrach = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfScrach));
	strSubSide.Append(EXT_COLON);

	//83. KV12결함수
	if(m_stDefType.TftScrach >= 999)
		m_stDefType.TftScrach = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftScrach));
	strSubSide.Append(EXT_COLON);

	//84. KV13결함수
	if(m_stDefType.CfCornerBroken >= 999)
		m_stDefType.CfCornerBroken = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfCornerBroken));
	strSubSide.Append(EXT_COLON);

	//85. KV14결함수
	if(m_stDefType.TftCornerBroken >= 999)
		m_stDefType.TftCornerBroken = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftCornerBroken));
	strSubSide.Append(EXT_COLON);

	//86. KV15결함수
	if(m_stDefType.CfWhiteBlur >= 999)
		m_stDefType.CfWhiteBlur = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfWhiteBlur));
	strSubSide.Append(EXT_COLON);

	//87. KV16결함수
	if(m_stDefType.TftWhiteBlur >= 999)
		m_stDefType.TftWhiteBlur = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftWhiteBlur));
	strSubSide.Append(EXT_COLON);

	//88. KV17결함수
	if(m_stDefType.CfDimplePoint >= 999)
		m_stDefType.CfDimplePoint = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfDimplePoint));
	strSubSide.Append(EXT_COLON);

	//89. KV18결함수
	if(m_stDefType.TftDimplePoint >= 999)
		m_stDefType.TftDimplePoint = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftDimplePoint));
	strSubSide.Append(EXT_COLON);

	//90. KV19결함수
	if(m_stDefType.CfDimpleLine >= 999)
		m_stDefType.CfDimpleLine = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfDimpleLine));
	strSubSide.Append(EXT_COLON);

	//91. KV20결함수
	if(m_stDefType.TftDimpleLine >= 999)
		m_stDefType.TftDimpleLine = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftDimpleLine));
	strSubSide.Append(EXT_COLON);

	//92. KV21결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//93. KV22결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//94. KV23결함수
	if(m_stDefType.CfMeasureFail >= 999)
		m_stDefType.CfMeasureFail = 999;

	strSubSide.Append(IntToCstr(m_stDefType.CfMeasureFail));
	strSubSide.Append(EXT_COLON);

	//95. KV24결함수
	if(m_stDefType.TftMeasureFail >= 999)
		m_stDefType.TftMeasureFail = 999;

	strSubSide.Append(IntToCstr(m_stDefType.TftMeasureFail));
	strSubSide.Append(EXT_COLON);

	//96. KV25결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//97. KV26결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//98. KV27결함수
	if(m_stDefType.ItoPeel >= 999)
		m_stDefType.ItoPeel = 999;

	strSubSide.Append(IntToCstr(m_stDefType.ItoPeel));
	strSubSide.Append(EXT_COLON);

	//99. KV28결함수
	if(m_stDefType.SocketScrach >= 999)
		m_stDefType.SocketScrach = 999;

	strSubSide.Append(IntToCstr(m_stDefType.SocketScrach));
	strSubSide.Append(EXT_COLON);

	//100. KV29결함수
	if(m_stDefType.SocketBurn >= 999)
		m_stDefType.SocketBurn = 999;

	strSubSide.Append(IntToCstr(m_stDefType.SocketBurn));
	strSubSide.Append(EXT_COLON);

	//101. KV30결함수
	if(m_stDefType.AlignFail>= 999)
		m_stDefType.AlignFail = 999;

	strSubSide.Append(IntToCstr(m_stDefType.AlignFail));
	strSubSide.Append(EXT_COLON);

	//102. KV31결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//103. KV32결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//104. KV33결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//105. KV34결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//106. KV35결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//107. KV36결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//108. KV37결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//109. KV38결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//110. KV39결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

	//111. KV40결함수
	strSubSide.Append(_T("0"));
	strSubSide.Append(EXT_COLON);

}

void CDataManager::m_fnMoveMeasureImg(CString strPanelID)
{
	//2018.01.22 Measure Image Move
	//D:\CruxServer\Result\Measure\PanelID\C1_270.jpg C2_270.jpg C3_090.jpg C4_090.jpg	
	//D:\QMS_IMG_TMP\YYYY_MM_DD\51400\PanelID\HH_MM_SS\ StartTime
	CString strQmsImgTmpPath, strMeasureImgPath, strtemp, strFileName, strStepId;
	CString strTime, strHour, strMin, strSec;
	CString strDate, strYear, strMonth, strDay;
	TCHAR chRead[255] = {0};
	CFileFind finder;
	BOOL bWorking;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();


	strtemp = m_strSaveDate;	//check time
	strYear = strtemp.Mid(0, 4);
	strMonth = strtemp.Mid(4, 2);
	strDay = strtemp.Mid(6, 2);
	strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

	strtemp = m_strSaveTime;	//check time
	strHour = strtemp.Mid(0, 2);
	strMin = strtemp.Mid(2, 2);
	strSec = strtemp.Mid(4, 2);
	strTime.Format(_T("%s_%s_%s"),strHour,strMin,strSec);

	G_AddLog(3, 3, _T("Check Time m_fnMoveMeasureimg() Date:%s Time:%s"), m_strSaveDate, m_strSaveTime);

	memset(chRead,0X00,sizeof(chRead));	
	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	strStepId  = chRead;

	strMeasureImgPath.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\Measure\\"), strPanelID);
	strQmsImgTmpPath.Format(_T("%s%s\\%s\\%s\\%s\\"), _T("D:\\QMS_IMG_TMP\\"), strDate, strStepId, strPanelID, strTime);
	pFrame->m_fnCreateFolder(strQmsImgTmpPath);	//2018.03.10
	bWorking = finder.FindFile(strMeasureImgPath + _T("*.*"));

	if(bWorking != TRUE)
	{
		strMeasureImgPath = _T("D:\\CruxServer\\Result\\Measure\\Default\\");
		bWorking = finder.FindFile(strMeasureImgPath + _T("*.*"));
	}

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

		if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		strFileName = finder.GetFileName();	// 파일 이름 얻기			

		if (strFileName.Find(EXTENSION_JPG) != -1)
		{
			MoveFile(strMeasureImgPath + strFileName,  strQmsImgTmpPath + strFileName );	

		}
	}

	m_fnDeleteFolder(strMeasureImgPath);
	//2018.01.22
}
//2020.06.10 add by mby S-
void CDataManager::m_fnMoveMeasureImg_TFT(CString strPanelID)
{
	CString strQmsImgTmpPath, strMeasureImgPath1,strMeasureImgPath2,strMeasureImgPath3, strtemp, strFileName, strStepId;
	CString strTime, strHour, strMin, strSec;
	CString strDate, strYear, strMonth, strDay;
	TCHAR chRead[255] = {0};
	CFileFind finder;
	BOOL bWorking;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();

	strtemp = m_strSaveDate;	//check time
	strYear = strtemp.Mid(0, 4);
	strMonth = strtemp.Mid(4, 2);
	strDay = strtemp.Mid(6, 2);
	strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

	strtemp = m_strSaveTime;	//check time
	strHour = strtemp.Mid(0, 2);
	strMin = strtemp.Mid(2, 2);
	strSec = strtemp.Mid(4, 2);
	strTime.Format(_T("%s_%s_%s"),strHour,strMin,strSec);

	G_AddLog(3, 3, _T("Check Time m_fnMoveMeasureimg() Date:%s Time:%s"), m_strSaveDate, m_strSaveTime);

	memset(chRead,0X00,sizeof(chRead));	
	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	strStepId  = chRead;

	if(pFrame->m_stParamInfo.nCountVisionClient > VISION_PC_6){
		strMeasureImgPath1.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\visionclient-7\\"), strPanelID);
		strMeasureImgPath2.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\visionclient-9\\"), strPanelID);
	}else{
		strMeasureImgPath1.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\visionclient-5\\"), strPanelID);
		strMeasureImgPath2.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\visionclient-6\\"), strPanelID);
	}
	strMeasureImgPath3.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\CF_TFT_Measure\\"), strPanelID);
	pFrame->m_fnCreateFolder(strMeasureImgPath3);
	strQmsImgTmpPath.Format(_T("%s%s\\%s\\%s\\%s\\"), _T("D:\\QMS_IMG_TMP\\"), strDate, strStepId, strPanelID, strTime);
	pFrame->m_fnCreateFolder(strQmsImgTmpPath);
	bWorking = finder.FindFile(strMeasureImgPath1 + _T("*.*"));

	if(bWorking != TRUE)
	{
		strMeasureImgPath1 = _T("D:\\CruxServer\\Result\\Measure\\Default\\");
		bWorking = finder.FindFile(strMeasureImgPath1 + _T("*.*"));
	}

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

		if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		strFileName = finder.GetFileName();	// 파일 이름 얻기			

		if (strFileName.Find(EXTENSION_JPG) != -1)
		{
			CopyFile(strMeasureImgPath1 + strFileName,	strQmsImgTmpPath + strFileName,0);
			MoveFile(strMeasureImgPath1 + strFileName,  strMeasureImgPath3 + strFileName );	

		}
	}

	m_fnDeleteFolder(strMeasureImgPath1);
	bWorking = finder.FindFile(strMeasureImgPath2 + _T("*.*"));

	if(bWorking != TRUE)
	{
		strMeasureImgPath2 = _T("D:\\CruxServer\\Result\\Measure\\Default\\");
		bWorking = finder.FindFile(strMeasureImgPath2 + _T("*.*"));
	}

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

		if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		strFileName = finder.GetFileName();	// 파일 이름 얻기			

		if (strFileName.Find(EXTENSION_JPG) != -1)
		{
			CopyFile(strMeasureImgPath2 + strFileName,	strQmsImgTmpPath + strFileName,0);
			MoveFile(strMeasureImgPath2 + strFileName,  strMeasureImgPath3 + strFileName );	

		}
	}

	m_fnDeleteFolder(strMeasureImgPath2);

}
//2020.06.10  add by mby E-
void CDataManager::m_fnMoveDefectMapImg(CString strPanelID)
{
	//2018.01.22 DefectMap Image Move
	//D:\CruxServer\Result\DefectMap\PanelID\ALLMAP_000.jpg
	//D:\QMS_IMG_TMP\YYYY_MM_DD\51400\PanelID\HH_MM_SS\ StartTime
	CString strQmsImgTmpPath, strDefectMapImgPath, strtemp, strFileName, strStepId;
	CString strTime, strHour, strMin, strSec;
	CString strDate, strYear, strMonth, strDay;
	TCHAR chRead[255] = {0};
	CFileFind finder;
	BOOL bWorking;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();


	strtemp = m_strSaveDate;	//check time
	strYear = strtemp.Mid(0, 4);
	strMonth = strtemp.Mid(4, 2);
	strDay = strtemp.Mid(6, 2);
	strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

	strtemp = m_strSaveTime;	//check time
	strHour = strtemp.Mid(0, 2);
	strMin = strtemp.Mid(2, 2);
	strSec = strtemp.Mid(4, 2);
	strTime.Format(_T("%s_%s_%s"),strHour,strMin,strSec);

	G_AddLog(3, 3, _T("Check Time m_fnMoveMeasureimg() Date:%s Time:%s"), m_strSaveDate, m_strSaveTime);

	memset(chRead,0X00,sizeof(chRead));	
	pFrame->m_iniConfigSet.GetProfileStringW(EQP_STATE, KEY_STATE_STEP_ID, _T("51400"), (LPTSTR)chRead, 255);
	strStepId  = chRead;

	strDefectMapImgPath.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\DefectMap\\"), strPanelID);
	//strDefectMapImgPath.Format(_T("%s%s\\"), _T("D:\\CruxServer\\Result\\DefectMap\\"), _T("Default"));
	strQmsImgTmpPath.Format(_T("%s%s\\%s\\%s\\%s\\"), _T("D:\\QMS_IMG_TMP\\"), strDate, strStepId, strPanelID, strTime);
	pFrame->m_fnCreateFolder(strQmsImgTmpPath);	//2018.03.10
	bWorking = finder.FindFile(strDefectMapImgPath + _T("*.*"));

	if(bWorking != TRUE)
	{
		strDefectMapImgPath = _T("D:\\CruxServer\\Result\\DefectMap\\Default\\");
		bWorking = finder.FindFile(strDefectMapImgPath + _T("*.*"));
	}

	while(bWorking == TRUE)
	{
		bWorking = finder.FindNextFile();	// 파일 하나를 찾음	

		if(finder.IsDots()) continue;		// "."이나  ".." 이면 다음으로 넘어감

		strFileName = finder.GetFileName();	// 파일 이름 얻기			

		if (strFileName.Find(EXTENSION_JPG) != -1)
		{
			CopyFile(strDefectMapImgPath + strFileName,  strQmsImgTmpPath + strFileName, 0);	

		}
	}

	m_fnDeleteFolder(strDefectMapImgPath);
	//2018.01.22
}

void CDataManager::m_fnSetManualJudge(int bJudge, CString strPanelID, CString strManualJudgeDefectFile, CString strQmsDefectFile, CString stropid)
{
	CString strFileName, strQmsWaitDefectFile, strQmsImgTempPath, strQmsImgWaitPath;
	BOOL bRet = FALSE, bNgFlag = FALSE;
	STRU_NG_FLAG_INFO stNgDataTemp;
	CStdioFile cstFile;
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	//2018.01.22
	CString strtemp;
	CString strDate, strYear, strMonth, strDay;

	strtemp = m_strSaveDate;	//check time
	strYear = strtemp.Mid(0, 4);
	strMonth = strtemp.Mid(4, 2);
	strDay = strtemp.Mid(6, 2);
	strDate.Format(_T("%s_%s_%s"),strYear,strMonth,strDay);

	if(bJudge == 1)
	{
		//1. Defect  File Modify NG -> OK
		m_strDefGrade1 = _T("A");
		m_strdefJudge1 = _T("");
		m_strdefJudge2 = _T("");  //null 2019.02.16 by ryu
		m_fnModifyItemData(strQmsDefectFile, strPanelID, P_JUDGEMENT, m_strDefGrade1.GetLength(), P_ITEM_END - 1, m_strDefGrade1);
		//m_fnModifyItemData(strQmsDefectFile, strPanelID, P_JUDGEMENT_CODE1, m_strdefJudge1.GetLength(), P_ITEM_END - 1, m_strdefJudge1);
		//m_fnModifyItemData(strQmsDefectFile, strPanelID, P_JUDGEMENT_CODE2, m_strdefJudge2.GetLength(), P_ITEM_END - 1, m_strdefJudge2);

		m_fnModifyItemData(strQmsDefectFile, _T("0"), 157, m_strDefGrade1.GetLength(), 158, m_strDefGrade1);
		m_fnModifyItemData(strQmsDefectFile, _T("3"), 10, stropid.GetLength(), 11, stropid);

		//2019.01.26 by ryu
		//m_fnModifyItemData(strQmsDefectFile, _T("0"), 99, m_strDefGrade1.GetLength(), 100, m_strDefGrade1);
		m_fnModifyItemData(strQmsDefectFile, _T("0"), 99, m_strDefGrade1.GetLength(), 100, JUDGE_AN);	//2021.02.04
		//m_str_rankCIMvalue = m_strDefGrade1;
		m_str_rankCIMvalue = JUDGE_AN;	//2021.02.07 add by mby
		G_AddLog(2, 0, _T("jaebok_test->%s"),m_str_rankCIMvalue);
		//2. Panel Judge UI Update
		pAuto->m_pDlgPanelJudge->SetResultCfJudge(_T("OK"));
		pAuto->m_pDlgPanelJudge->SetResultTftJudge(_T("OK"));
		pAuto->m_pDlgPanelJudge->PostMessage(WM_UI_CONTROL_JUDGE_DIS, (WPARAM)0, (LPARAM)0);
		pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)-1, (LPARAM)(LPCTSTR)strPanelID);
		
		//3. CIM Modify
		bNgFlag = m_fnNgFlagInfoCheck(NG_FLAG_A_GRADE);
		G_AddLog(2, 0,_T("ManualJudge PanelID : %s NG -> OK"), strPanelID);
	}
	//2019.12.18 add by mby
	else if(bJudge == 2)
	{
		//1. Defect  File Modify C -> B
		m_strDefGrade1 = _T("B");
		m_fnModifyItemData(strQmsDefectFile, strPanelID, P_JUDGEMENT, m_strDefGrade1.GetLength(), P_ITEM_END - 1, m_strDefGrade1);
		m_fnModifyItemData(strQmsDefectFile, _T("0"), 157, m_strDefGrade1.GetLength(), 158, m_strDefGrade1);
		m_fnModifyItemData(strQmsDefectFile, _T("3"), 10, stropid.GetLength(), 11, stropid);

		//2019.01.26 by ryu
		m_fnModifyItemData(strQmsDefectFile, _T("0"), 99, m_strDefGrade1.GetLength(), 100, JUDGE_FB3);	//2020.04.02 m_strDefGrade1->JUDGE_FB3 by mby
		m_str_rankCIMvalue = JUDGE_FB3;
		G_AddLog(2, 0, _T("jaebok_test->%s"),m_str_rankCIMvalue);

		//3. CIM Modify
		bNgFlag = m_fnNgFlagInfoCheck(NG_FLAG_AVL_B_GRADE);
		G_AddLog(2, 0,_T("ManualJudge PanelID : %s NG -> B"), strPanelID);
	}
	else
	{
		//1. Panel Judge UI Update
		//pAuto->m_pDlgPanelJudge->SendMessage(WM_UI_CONTROL_JUDGE_INFO_DIS, (WPARAM)-1, (LPARAM)(LPCTSTR)strPanelID);
		m_fnModifyItemData(strQmsDefectFile, _T("3"), 10, stropid.GetLength(), 11, stropid);
		//2. CIM Modify
		m_strDefGrade1 = _T("NG"); //2021.02.04 C->NG
		bNgFlag = m_fnNgFlagInfoCheck(NG_FLAG_C_GRADE);
		G_AddLog(2, 0,_T("ManualJudge PanelID : %s NG -> NG"), strPanelID);
	}

	//1. Defect File, Image Move Temp -> Wait
	strFileName = strQmsDefectFile.Right(strQmsDefectFile.GetLength() - strQmsDefectFile.ReverseFind('\\') - 1);
	strQmsWaitDefectFile = pFrame->m_stFtpInfo.stPath->strDir_Default + pFrame->m_stFtpInfo.stPath->strDir_Wait + strFileName;
	if(FileExist(strQmsDefectFile))
	{
		bRet = MoveFile(strQmsDefectFile, strQmsWaitDefectFile);
		if(!bRet)
			G_AddLog(2, 0,_T("m_fnSetManualJudge MoveFile() : %s -> %s Failed"), strQmsDefectFile, strQmsWaitDefectFile);
	}
	else
		G_AddLog(2, 0,_T("m_fnSetManualJudge : %s is not exist"), strQmsDefectFile);
	
	strQmsImgTempPath.Format(_T("%s%s"), _T("D:\\QMS\\i_temp\\"), strDate);
	strQmsImgWaitPath = _T("D:\\QMS\\i_wait");
	bRet = m_fnMoveFolder(strQmsImgTempPath, strQmsImgWaitPath);
	if(!bRet)
		G_AddLog(2, 0, _T("m_fnSetManualJudge m_fnMoveFolder() : %s -> %s Failed"),strQmsImgTempPath, strQmsImgWaitPath);

	//2. Panel Summary Data Modify And UI Update
	pAuto->m_pDlgPanelJudge->SetResultGrade(m_strDefGrade1);
	//pAuto->m_pDlgPanelJudge->SetResultDefectCodeJudge(m_strdefJudge1);
	pAuto->SendMessage(WM_UI_CONTROL_PNL_SUMMARY_LIST, (WPARAM)0, (LPARAM)(LPCTSTR)strQmsWaitDefectFile);
	pFrame->m_fnUpdateGridDefectSummary(strManualJudgeDefectFile);	

	//3. NG Flag Set
	stNgDataTemp.init_info();

	_tcscpy(stNgDataTemp.cPanelID, strPanelID);		
	_tcscpy(stNgDataTemp.cGrade,  m_strDefGrade1);	
	_tcscpy(stNgDataTemp.cJudge1, m_strdefJudge1);	
	_tcscpy(stNgDataTemp.cJudge2, m_strdefJudge2);	
	_tcscpy(stNgDataTemp.cCimMeasureData, m_strCimMeasureDataTemp);
	stNgDataTemp.nNgFlag = bNgFlag;	

	//2016.05.28		설정 Defect에 따라 Lower Eqp Bypass NGFlag Bit On
	//if( bLowerEqpBypass )
	//{
	//	if( pFrame->m_fnEqpSizeType() == EQP_MONITOR_SIZE || pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE )
	//		stNgDataTemp.bLowerEqpBypass = TRUE;
	//}

	if (m_strDefGrade1 == _T(""))	stNgDataTemp.DefaultData(strPanelID, APP_NG);

	m_fnWriteNgFlagList(stNgDataTemp);
	m_strPolPositionEQ_CV2  = _T("");
	m_strPolPositionStage_CV2 = _T("");

	//4. QMS File Upload
	FtpDstClient->m_fnFTPdUploadData();
	FtpImgClient->m_fnFTPdUploadImg();
}

CString CDataManager::m_fnMakeQmsFileName()
{
	CString strVal, strRetVal;
	int nSeqNoCnt = 0;
	TCHAR chRead[255] = {0};
	memset(chRead,0X00,sizeof(chRead));	

	m_iniConfig.GetProfileStringW(EQP_STATE, KEY_EQP_TYPE, _T(""), (LPTSTR)chRead, 255);
	strVal = _T("U_");
	strVal += chRead;

	strRetVal = strVal;

	memset(chRead,0X00,sizeof(chRead));		
	nSeqNoCnt = GetPrivateProfileInt(PNL_COUNT_SET, KEY_SEQ_NO, 0, m_cCountRoot);

	strVal.Format(_T("%.4d"), nSeqNoCnt);		// Nanjing
	//strVal.Format(_T("%s"),strPanelID);		// ChangDu

	strVal = _T("_") + strVal;
	strRetVal +=  strVal;

	nSeqNoCnt ++;

	if (nSeqNoCnt > 9999)	nSeqNoCnt = 1;

	strVal.Format(_T("%04d"), nSeqNoCnt);
	WritePrivateProfileString(PNL_COUNT_SET, KEY_SEQ_NO, strVal, m_cCountRoot);

	strVal =  _T("_") + m_strQmsSendFileDate;
	strRetVal += strVal;

	strRetVal += EXTENSION_TMP;

	return strRetVal;
}
//2018.09.08 add by mby ntype = Panel 모델(50인치= 0, 58인치=1, nStep=측정data순서, nvalue= 실제값)
int CDataManager::MeasureValueLimitChk(int nEqp, int ntype, int nStep , int value)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int nEqpNo = pFrame->m_iniConfigSet.GetProfileIntW(EQP_STATE, KEY_STATE_EQP_NO, 0);
	//AVL 추가 180912 by hikim
	if(nEqp == EQP_TYPE_AVL)
	{
		if(ntype == 2){
			switch (nStep)	
				{// CF
					case 0: if(m_stMeasureLimitInfo.nCFValue[0] < value || value < m_stMeasureLimitInfo.nCFValue[1])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[0]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[0]+m_stMeasureLimitInfo.nRValue);	break;//1200~200		//c1x
					case 1:	if(m_stMeasureLimitInfo.nCFValue[2] < value || value < m_stMeasureLimitInfo.nCFValue[3])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[1]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[1]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c1y
					case 2: if(m_stMeasureLimitInfo.nCFValue[4] < value || value < m_stMeasureLimitInfo.nCFValue[5])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[2]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[2]+m_stMeasureLimitInfo.nRValue);	break;//6200~5200	//c2x
					case 3:	if(m_stMeasureLimitInfo.nCFValue[6] < value || value < m_stMeasureLimitInfo.nCFValue[7])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[3]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[3]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c2y
					case 4: if(m_stMeasureLimitInfo.nCFValue[8] < value || value < m_stMeasureLimitInfo.nCFValue[9])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[4]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[4]+m_stMeasureLimitInfo.nRValue);	break;//6200~5200	//c3x
					case 5:	if(m_stMeasureLimitInfo.nCFValue[10] < value || value < m_stMeasureLimitInfo.nCFValue[11])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[5]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[5]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c3y
					case 6: if(m_stMeasureLimitInfo.nCFValue[12] < value || value < m_stMeasureLimitInfo.nCFValue[13])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[6]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[6]+m_stMeasureLimitInfo.nRValue);	break;//1200~200	//c4x
					case 7:	if(m_stMeasureLimitInfo.nCFValue[14] < value || value < m_stMeasureLimitInfo.nCFValue[15])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[7]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[7]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c4y
				// TFT
					case 8: if(m_stMeasureLimitInfo.nTFValue[0] < value || value < m_stMeasureLimitInfo.nTFValue[1])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[0]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[0]+m_stMeasureLimitInfo.nRValue);	break;//1390~390		//c1x
					case 9:	if(m_stMeasureLimitInfo.nTFValue[2] < value || value < m_stMeasureLimitInfo.nTFValue[3])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[1]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[1]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c1y
					case 10:if(m_stMeasureLimitInfo.nTFValue[4] < value || value < m_stMeasureLimitInfo.nTFValue[5])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[2]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[2]+m_stMeasureLimitInfo.nRValue);	break;//6340~5340	//c2x
					case 11:if(m_stMeasureLimitInfo.nTFValue[6] < value || value < m_stMeasureLimitInfo.nTFValue[7])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[3]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[3]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c2y
					case 12:if(m_stMeasureLimitInfo.nTFValue[8] < value || value < m_stMeasureLimitInfo.nTFValue[9])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[4]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[4]+m_stMeasureLimitInfo.nRValue);	break;//6340~5340	//c3x
					case 13:if(m_stMeasureLimitInfo.nTFValue[10] < value || value < m_stMeasureLimitInfo.nTFValue[11])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[5]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[5]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c3y
					case 14:if(m_stMeasureLimitInfo.nTFValue[12] < value || value < m_stMeasureLimitInfo.nTFValue[13])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[6]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[6]+m_stMeasureLimitInfo.nRValue);	break;//1390~390		//c4x
					default:
							if(m_stMeasureLimitInfo.nTFValue[14] < value || value < m_stMeasureLimitInfo.nTFValue[15])
							value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[7]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[7]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c4y				
				}
			//2020.10.16 add by mby S-
			if(value <= 20 && value >= -20){
				if(value > 0){
					value = (rand() % 79) + 20; //pFrame->m_RandomVal.m_fnGetRandomVal(21,100);
				}else{
					value = -((rand() % 79) + 20); //pFrame->m_RandomVal.m_fnGetRandomVal(-21,-100);
				}
			}
			//2020.10.16 add by mby E-
		}else{
			if(30 < value || value < -30) value = pFrame->m_RandomVal.m_fnGetRandomVal(-30,  30);
		}
	}
	else	//ABI
	{
		switch (nStep)	
			{// CF
				case 0: if(m_stMeasureLimitInfo.nCFValue[0] < value || value < m_stMeasureLimitInfo.nCFValue[1])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[0]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[0]+m_stMeasureLimitInfo.nRValue);	break;//1200~200		//c1x
				case 1:	if(m_stMeasureLimitInfo.nCFValue[2] < value || value < m_stMeasureLimitInfo.nCFValue[3])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[1]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[1]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c1y
				case 2: if(m_stMeasureLimitInfo.nCFValue[4] < value || value < m_stMeasureLimitInfo.nCFValue[5])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[2]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[2]+m_stMeasureLimitInfo.nRValue);	break;//6200~5200	//c2x
				case 3:	if(m_stMeasureLimitInfo.nCFValue[6] < value || value < m_stMeasureLimitInfo.nCFValue[7])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[3]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[3]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c2y
				case 4: if(m_stMeasureLimitInfo.nCFValue[8] < value || value < m_stMeasureLimitInfo.nCFValue[9])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[4]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[4]+m_stMeasureLimitInfo.nRValue);	break;//6200~5200	//c3x
				case 5:	if(m_stMeasureLimitInfo.nCFValue[10] < value || value < m_stMeasureLimitInfo.nCFValue[11])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[5]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[5]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c3y
				case 6: if(m_stMeasureLimitInfo.nCFValue[12] < value || value < m_stMeasureLimitInfo.nCFValue[13])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[6]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[6]+m_stMeasureLimitInfo.nRValue);	break;//1200~200	//c4x
				case 7:	if(m_stMeasureLimitInfo.nCFValue[14] < value || value < m_stMeasureLimitInfo.nCFValue[15])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nCFRStValue[7]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nCFRStValue[7]+m_stMeasureLimitInfo.nRValue);	break;//4450~3450	//c4y
			// TFT
				case 8: if(m_stMeasureLimitInfo.nTFValue[0] < value || value < m_stMeasureLimitInfo.nTFValue[1])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[0]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[0]+m_stMeasureLimitInfo.nRValue);	break;//1390~390		//c1x
				case 9:	if(m_stMeasureLimitInfo.nTFValue[2] < value || value < m_stMeasureLimitInfo.nTFValue[3])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[1]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[1]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c1y
				case 10:if(m_stMeasureLimitInfo.nTFValue[4] < value || value < m_stMeasureLimitInfo.nTFValue[5])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[2]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[2]+m_stMeasureLimitInfo.nRValue);	break;//6340~5340	//c2x
				case 11:if(m_stMeasureLimitInfo.nTFValue[6] < value || value < m_stMeasureLimitInfo.nTFValue[7])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[3]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[3]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c2y
				case 12:if(m_stMeasureLimitInfo.nTFValue[8] < value || value < m_stMeasureLimitInfo.nTFValue[9])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[4]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[4]+m_stMeasureLimitInfo.nRValue);	break;//6340~5340	//c3x
				case 13:if(m_stMeasureLimitInfo.nTFValue[10] < value || value < m_stMeasureLimitInfo.nTFValue[11])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[5]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[5]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c3y
				case 14:if(m_stMeasureLimitInfo.nTFValue[12] < value || value < m_stMeasureLimitInfo.nTFValue[13])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[6]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[6]+m_stMeasureLimitInfo.nRValue);	break;//1390~390		//c4x
				default:
						if(m_stMeasureLimitInfo.nTFValue[14] < value || value < m_stMeasureLimitInfo.nTFValue[15])
					    value = pFrame->m_RandomVal.m_fnGetRandomVal( m_stMeasureLimitInfo.nTFRStValue[7]-m_stMeasureLimitInfo.nRValue, m_stMeasureLimitInfo.nTFRStValue[7]+m_stMeasureLimitInfo.nRValue);	break;//4800~3800	//c4y				
			}
		//if(ntype == 0)
		//{
		//	switch (nStep)	
		//	{//50인치 CF
		//		case 0: if(120 < value || value < 20)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 60, 80);	break;//1200~200	//c1x
		//		case 1:	if(445 < value || value < 345) value = pFrame->m_RandomVal.m_fnGetRandomVal(385, 405);	break;//4450~3450	//c1y
		//		case 2: if(620 < value || value < 520) value = pFrame->m_RandomVal.m_fnGetRandomVal(560, 580);	break;//6200~5200	//c2x
		//		case 3:	if(445 < value || value < 345) value = pFrame->m_RandomVal.m_fnGetRandomVal(385, 405);	break;//4450~3450	//c2y
		//		case 4: if(620 < value || value < 520) value = pFrame->m_RandomVal.m_fnGetRandomVal(560, 580);	break;//6200~5200	//c3x
		//		case 5:	if(445 < value || value < 345) value = pFrame->m_RandomVal.m_fnGetRandomVal(385, 405);	break;//4450~3450	//c3y
		//		case 6: if(120 < value || value < 20)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 60, 80);	break;//1200~200	//c4x
		//		case 7:	if(445 < value || value < 345) value = pFrame->m_RandomVal.m_fnGetRandomVal(385, 405);	break;//4450~3450	//c4y
		//	//50인치 TFT
		//		case 8: if(139 < value || value < 39)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 79, 99);	break;//1390~390		//c1x
		//		case 9:	if(480 < value || value < 380) value = pFrame->m_RandomVal.m_fnGetRandomVal(420, 440);	break;//4800~3800	//c1y
		//		case 10:if(634 < value || value < 534) value = pFrame->m_RandomVal.m_fnGetRandomVal(574, 594);	break;//6340~5340	//c2x
		//		case 11:if(480 < value || value < 380) value = pFrame->m_RandomVal.m_fnGetRandomVal(420, 440);	break;//4800~3800	//c2y
		//		case 12:if(634 < value || value < 534) value = pFrame->m_RandomVal.m_fnGetRandomVal(574, 594);	break;//6340~5340	//c3x
		//		case 13:if(480 < value || value < 380) value = pFrame->m_RandomVal.m_fnGetRandomVal(420, 440);	break;//4800~3800	//c3y
		//		case 14:if(139 < value || value < 39)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 79, 99);	break;//1390~390		//c4x
		//		default:
		//				if(480 < value || value < 380) value = pFrame->m_RandomVal.m_fnGetRandomVal(420, 440);	break;//4800~3800	//c4y				
		//	}
		//}
		//else if(ntype == 1)
		//{
		//	switch (nStep)
		//	{//58인치 CF
		//		case 0: if(125 < value || value < 25)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 65,  85);	break;//1250~250			//c1x
		//		case 1:	if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c1y
		//		case 2: if(625 < value || value < 525) value = pFrame->m_RandomVal.m_fnGetRandomVal(565, 585);	break;//6250~5250		//c2x
		//		case 3:	if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c2y
		//		case 4: if(625 < value || value < 525) value = pFrame->m_RandomVal.m_fnGetRandomVal(565, 585);	break;//6250~5250		//c3x
		//		case 5:	if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c3y
		//		case 6: if(125 < value || value < 25)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 65, 85);	break;//1250~250			//c4x
		//		case 7:	if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c4y
		//	//58인치 TFT
		//		case 8: if(125 < value || value < 25)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 65, 85);	break;//1250~250			//c1x
		//		case 9:	if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c1y
		//		case 10:if(625 < value || value < 525) value = pFrame->m_RandomVal.m_fnGetRandomVal(565, 585);	break;//6250~5250		//c2x
		//		case 11:if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c2y
		//		case 12:if(625 < value || value < 525) value = pFrame->m_RandomVal.m_fnGetRandomVal(565, 585);	break;//6250~5250		//c3x
		//		case 13:if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c3y
		//		case 14:if(125 < value || value < 25)  value = pFrame->m_RandomVal.m_fnGetRandomVal( 65,  85);	break;//1250~250			//c4x
		//		default:
		//				if(475 < value || value < 375) value = pFrame->m_RandomVal.m_fnGetRandomVal(415, 435);	break;//4750~3750		//c4y				
		//	}
		//}	
	}
	return value;
}

int CDataManager::m_fnNgFlagInfoCheck(int Ngflag)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	int nNgFlag;
	if(Ngflag == 0) nNgFlag = 1;
	else if(Ngflag == 1) nNgFlag = 1;	//2021.02.10 add by mby
	else if(Ngflag == 5) nNgFlag = 20;
	else if(Ngflag == 6) nNgFlag = 30;

	//A
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_A)			nNgFlag = 1;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_AN)	nNgFlag = 1;	//2021.02.04 add by mby
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA1)	nNgFlag = NG_FLAG_FA1_GRADE;//10
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA2)	nNgFlag = NG_FLAG_FA2_GRADE;//11
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA3)	nNgFlag = NG_FLAG_FA3_GRADE;//12
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA4)	nNgFlag = NG_FLAG_FA4_GRADE;//13
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA5)	nNgFlag = NG_FLAG_FA5_GRADE;//14
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA6)	nNgFlag = NG_FLAG_FA6_GRADE;//15
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FA8)	nNgFlag = NG_FLAG_FA8_GRADE;//17
	//B
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_B)			nNgFlag = 20;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB1)	nNgFlag = NG_FLAG_FB1_GRADE;//21
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB2)	nNgFlag = NG_FLAG_FB2_GRADE;//22
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB3)	nNgFlag = NG_FLAG_FB3_GRADE;//23
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB4)	nNgFlag = NG_FLAG_FB4_GRADE;//24
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB5)	nNgFlag = NG_FLAG_FB5_GRADE;//25
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FB6)	nNgFlag = NG_FLAG_FB6_GRADE;//26
	//C
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_C)			nNgFlag = 30;
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC1)	nNgFlag = NG_FLAG_FC1_GRADE;//31
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC2)	nNgFlag = NG_FLAG_FC2_GRADE;//32
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC3)	nNgFlag = NG_FLAG_FC3_GRADE;//33
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC4)	nNgFlag = NG_FLAG_FC4_GRADE;//34
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC5)	nNgFlag = NG_FLAG_FC5_GRADE;//35
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_FC6)	nNgFlag = NG_FLAG_FC6_GRADE;//36

	//S,S1,A1
	if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_S)			nNgFlag = NG_FLAG_S_GRADE;//80
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_S1)	nNgFlag = NG_FLAG_S1_GRADE;//81
	else if(pFrame->m_DataManager.m_str_rankCIMvalue.Trim() == JUDGE_A1)	nNgFlag = NG_FLAG_A1_GRADE;//100	

	return nNgFlag;
}
void   CDataManager::m_fnDccInfoSave()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	int nRead = 0;
	TCHAR strBuffer[255];
	CString cCountRoot;
	CString strTemp,strTemp1;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}

	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("%d"), m_nArrayDccCount[i]);
		strTemp1.Format(_T("TCHECK%d"), i+1);
		WritePrivateProfileString(strRecipeNo, strTemp1, strTemp, cCountRoot);
		//WritePrivateProfileString(_T("DCC_COUNT"), strTemp1, strTemp, cCountRoot);

		strTemp.Format(_T("TSET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		nRead = _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);

		m_nDccTotalPanel[i] += 1;	//Save 할때 Paenl Count 증가
		if(m_nDccTotalPanel[i] > nRead){
			strTemp.Format(_T("%d"),0);
			WritePrivateProfileString(strRecipeNo, strTemp1, strTemp, cCountRoot); //CHECK DATA RESET
			//WritePrivateProfileString(_T("DCC_COUNT"), strTemp1, strTemp, cCountRoot); //CHECK DATA RESET
		}else{
			strTemp.Format(_T("%d"),m_nDccTotalPanel[i]);
		}
		strTemp1.Format(_T("TOTAL%d"), i+1);
		WritePrivateProfileString(strRecipeNo, strTemp1, strTemp, cCountRoot);
		//WritePrivateProfileString(_T("DCC_COUNT"), strTemp1, strTemp, cCountRoot);
	}
	
	
}
void   CDataManager::m_fnDccInfoGet()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString cCountRoot;
	TCHAR strBuffer[255];
	CString strTemp,strTemp1;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp1.Format(_T("TCHECK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nArrayDccCount[i] = _ttoi(strTemp);
		//GetPrivateProfileString(_T("DCC_COUNT"), strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp1.Format(_T("TOTAL%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nDccTotalPanel[i] = _ttoi(strTemp);
		//GetPrivateProfileString(_T("DCC_COUNT"), strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		//Defect Type Flag 초기화
		m_nDccFlag[i]		= 0;
	}
	for (int i = 0; i < DCC_COUNT; i++)
	{
		strTemp1.Format(_T("CHECKSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		m_nAryDccChkSizeXData[i] = _ttoi(strTemp);
		//m_nAryDccChkSizeXData[i] = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp1, 0);
		strTemp1.Format(_T("CHECKSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		m_nAryDccChkSizeYData[i] = _ttoi(strTemp);
		//m_nAryDccChkSizeYData[i] = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp1, 0);
	}
	for (int i = 0; i < DCC_SIZE_CHECK; i++)
	{
		strTemp1.Format(_T("SETSIZEX%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		m_strAryDccSizeXData[i] = strBuffer;
		//pFrame->m_iniConfigSet.GetProfileStringW(_T("DCC_INFO"), strTemp1, _T("0"), (LPTSTR)strBuffer, 255);

		strTemp1.Format(_T("SETSIZEY%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp1, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		//pFrame->m_iniConfigSet.GetProfileStringW(_T("DCC_INFO"), strTemp1, _T("0"), (LPTSTR)strBuffer, 255);
		m_strAryDccSizeYData[i] = strBuffer;
	}
	//2020.04.10 add by mby S-
	for (int i = 0; i < DCC_DIRECT_COUNT; i++)
	{
		strTemp.Format(_T("CHECKDIRECT%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strTemp = strBuffer;
		m_nDccDirectFlag[i] = _ttoi(strTemp);
		//m_nDccDirectFlag[i] = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
	}	
}
void   CDataManager::m_fnDccCheckAlarm()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);

	int m_nAryDccCheckTemp[DCC_CHK_COUNT];
	int m_nAryDccInfoOneTemp[DCC_CHK_COUNT];
	int m_nAryDccInfoTwoTemp[DCC_CHK_COUNT];

	int nRead = 0;
	TCHAR strBuffer[255];
	m_bDcc_Alarm_flag = FALSE;
	CString strMsg = _T("PA+ CODE DEFECT:");
	CString strTemp,cCountRoot;

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}

	//설정값 불러오기
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("CHECK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccCheckTemp[i]	= _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccCheckTemp[i] = nRead;
		strTemp.Format(_T("SET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoOneTemp[i]	= _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccInfoOneTemp[i] = nRead;
		strTemp.Format(_T("TSET%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccInfoTwoTemp[i]	= _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccInfoTwoTemp[i] = nRead;
	}
	//현재couunt와 설정값비교하여 알람발생
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		if(m_nAryDccCheckTemp[i]){
			if((m_nDccTotalPanel[i] <  m_nAryDccInfoTwoTemp[i]) && (m_nAryDccInfoOneTemp[i] != 0)){
				if(m_nArrayDccCount[i] >= m_nAryDccInfoOneTemp[i]){
					switch(i){
						case 0:		strMsg += "/CF-TFT_BURR/";
									m_nArrayDccCount[i] = 0;
							break;	
						case 1:		strMsg += "/CF_BROKEN/";
									m_nArrayDccCount[i] = 0;
							break;
						case 2:		strMsg += "/TFT_BROKEN/ ";
									m_nArrayDccCount[i] = 0;
							break;
						case 3:		strMsg += "/CF_CORNER_BROKEN/";
									m_nArrayDccCount[i] = 0;
							break;
						case 4:		strMsg += "/TFT_CORNER_BROKEN/";
									m_nArrayDccCount[i] = 0;
							break;
						/*case 5:		strMsg += "/TFT_CORNER_BROKEN/";
									m_nArrayDccCount[i] = 0;
							break;
						case 6:		strMsg += "/CF_POL_BUBBLE/";
									m_nArrayDccCount[i] = 0;
							break;
						case 7:		strMsg += "/TFT_POL_BUBBLE/";
									m_nArrayDccCount[i] = 0;
							break;
						case 8:		strMsg += "/EDGE_BROKEN/";
									m_nArrayDccCount[i] = 0;
							break;
						case 9:		strMsg += "/PAD_SCRATCH/";
									m_nArrayDccCount[i] = 0;
							break;						
						case 10:	strMsg += "/CF_POL_SCRATCH/";
									m_nArrayDccCount[i] = 0;
							break;
						case 11:	strMsg += "/TFT_POL_SCRATCH/";
									m_nArrayDccCount[i] = 0;
							break;
						case 12:	strMsg += "/CF_POL_SUBSTANCE/";
									m_nArrayDccCount[i] = 0;
							break;
						case 13:	strMsg += "/TFT_POL_SUBSTANCE/";
									m_nArrayDccCount[i] = 0;
							break;
						case 14:	strMsg += "/BLACK_SCRATCH/";
									m_nArrayDccCount[i] = 0;
							break;*/

					}
					m_bDcc_Alarm_flag = TRUE;
				}
			}			
		}
	}
	if(m_bDcc_Alarm_flag){
		pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROCESS_PACODE_ALARM, 1);
		pAuto->m_pDlgStageState->SetChangePACodeMsg(strMsg);
		//m_strDccAlarmMsg = strMsg;
		G_AddLog(2, 0, _T("PA+ Code HeavyAlarm"));
		/*pAuto->m_pDlgMeaErrorMsg = NULL;
		pAuto->m_pDlgMeaErrorMsg = new CDlgMessage2();
		pAuto->m_pDlgMeaErrorMsg->Create(IDD_DIALOG2);
		pAuto->m_pDlgMeaErrorMsg->init(strMsg);
		pAuto->m_pDlgMeaErrorMsg->ShowWindow( SW_SHOW );*/

		//pAuto->m_pDlgMeaErrorMsg->MoveWindow(500, 350, 500, 150);
	}
}
//2020.06.02 add by mby
BOOL  CDataManager::m_fnDccSizeCheck(CString strSizeX, CString strSizeY, int DeftypeNo, CString strDefPos)
{ 
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);

	int m_nAryDccCheckTemp[DCC_CHK_COUNT];
	int nRead = 0;
	CString strTemp,cCountRoot;
	bool Ret = FALSE;
	TCHAR strBuffer[255];

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("CHECK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryDccCheckTemp[i]	= _ttoi(strTemp);
		//nRead = pFrame->m_iniConfigSet.GetProfileIntW(_T("DCC_INFO"), strTemp, 0);
		//m_nAryDccCheckTemp[i] = nRead;		
	}
	if(m_nAryDccCheckTemp[DeftypeNo]){	//Defect Check flag
		switch (DeftypeNo)
		{
			case 0:	//CF/TFT BURR
				if(m_nDccDirectFlag[0] && !m_nDccDirectFlag[1]){	//X : A, C - Y: B,D
					//조건1번
					if(strDefPos == _T("B")){
						if(m_nAryDccChkSizeYData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}						
					}else if(strDefPos == _T("C")){
						if(m_nAryDccChkSizeXData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}						
					}else if(strDefPos == _T("D")){
						if(m_nAryDccChkSizeYData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}else{
						if(m_nAryDccChkSizeXData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}
					//////////////////////////
				}else if(!m_nDccDirectFlag[0] && m_nDccDirectFlag[1]){	//X: B, D - Y: A,C
					if(strDefPos == _T("B")){
						if(m_nAryDccChkSizeXData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}else if(strDefPos == _T("C")){
						if(m_nAryDccChkSizeYData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}else if(strDefPos == _T("D")){
						if(m_nAryDccChkSizeXData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}else{
						if(m_nAryDccChkSizeYData[DeftypeNo*2]){
							if((_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건1부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check1_flag = TRUE;
							}
						}
						if(m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){
							if((_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
								Ret = TRUE;
								//조건2부합Flag사용하여 Panelsum에 표시
								m_bDcc_Check2_flag = TRUE;
							}
						}
					}
				}
				break;

			//A,B,C,D 변 각각 Check 확인하여 비교한다.
			case 1:	//CF BROKEN				
				//조건1번
				if(strDefPos == _T("B")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 2] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*8)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*8)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*8]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*8)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*8]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*8)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 2]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*8]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*8)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 3] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 3]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*8) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*8)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*8) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*8)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 3]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*8) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*8)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 3]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*8) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*8)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else if(strDefPos == _T("C")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 4] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 4]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 4]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*12]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 4]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 5] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 5]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 5]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 5]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}						
				}else if(strDefPos == _T("D")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 6] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 6]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 6]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*16]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 6]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 7] && m_nAryDccChkSizeYData[(DeftypeNo*2) + 7]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) + 7]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) + 7]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else{//A변
					//조건1번
					if(m_nAryDccChkSizeXData[DeftypeNo*2] && m_nAryDccChkSizeYData[DeftypeNo*2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[DeftypeNo*2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*4]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[DeftypeNo*2]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*4]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1] && m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*2) +1]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*4) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*4)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*2) +1]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*4) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*4)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}			
				break;
			case 2:	//TFT BROKEN
				//조건1번
				if(strDefPos == _T("B")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 2] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*12]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 2]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 3] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 3]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 3]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 3]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else if(strDefPos == _T("C")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 4] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 4]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*14]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 4]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*14]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 4]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*14]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 5] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 5]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*14) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 5]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 5]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*14) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}						
				}else if(strDefPos == _T("D")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 6] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 6]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 6]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*16]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 6]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 7] && m_nAryDccChkSizeYData[(DeftypeNo*5) + 7]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) + 7]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) + 7]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else{	//A변
					//조건1번
					if(m_nAryDccChkSizeXData[DeftypeNo*5] && m_nAryDccChkSizeYData[DeftypeNo*5]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*10]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*10)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*10]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*10)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[DeftypeNo*5]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*10]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*10)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[DeftypeNo*5]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*10]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*10)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*5) +1] && m_nAryDccChkSizeYData[(DeftypeNo*5) +1]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*10) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*10)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*10) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*10)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*5) +1]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*10) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*10)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*5) +1]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*10) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*10)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}			
				break;
			case 3:	//CF COR BROEKN
				//조건1번
				if(strDefPos == _T("B")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 2] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+2]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+2]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+2]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+2]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 3] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 3]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 3]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+4]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 3]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+4]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 3]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 3]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+4]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 3]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 3]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+4]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else if(strDefPos == _T("C")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 4] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 4]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) - 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*15) - 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 4]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) - 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 4]){
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*15) - 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 5] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 5]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) + 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)+2]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)+2]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 5]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) + 1]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)+2]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 5]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 1]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)+2]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}						
				}else if(strDefPos == _T("D")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 6] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 6]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 6]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*16]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 6]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 7] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 7]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 7]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 7]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else{	//A변
					//조건1번
					if(m_nAryDccChkSizeXData[DeftypeNo*6] && m_nAryDccChkSizeYData[DeftypeNo*6]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*12]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[DeftypeNo*6]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*12]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[DeftypeNo*6]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*12]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) +1] && m_nAryDccChkSizeYData[(DeftypeNo*6) +1]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) +1]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*12) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*12)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) +1]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*12) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*12)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}			
				break;
			case 4:	//TFT COR BROEKN
				//조건1번
				if(strDefPos == _T("B")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*7)] && m_nAryDccChkSizeYData[(DeftypeNo*7)]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*14)]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*7)]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*7)]){
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*14)]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 1] && m_nAryDccChkSizeYData[(DeftypeNo*7) + 1]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*14) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 1]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*14) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*14)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*7) + 1]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*14) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*14)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else if(strDefPos == _T("C")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 2] && m_nAryDccChkSizeYData[(DeftypeNo*7) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15) + 1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*15)]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 2]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15 + 1)]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*7) + 2]){
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*15)]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 3] && m_nAryDccChkSizeYData[(DeftypeNo*7) + 3]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*7) + 3]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*15) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*15)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*7) + 3]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*15) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*15)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}						
				}else if(strDefPos == _T("D")){
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*8)] && m_nAryDccChkSizeYData[(DeftypeNo*8)]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16)]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*8)]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*16]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*8)]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*16]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*8) + 1] && m_nAryDccChkSizeYData[(DeftypeNo*8) + 1]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*8) + 1]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*16) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*16)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*8) + 1]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*16) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*16)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}else{	//A변
					//조건1번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 2] && m_nAryDccChkSizeYData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*13]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+1]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[DeftypeNo*13]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+1]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건1부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check1_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeXData[DeftypeNo*13]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+1]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) + 2]){
						if((_ttof(m_strAryDccSizeYData[DeftypeNo*13]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+1]) >= _ttof(strSizeY))){
							Ret = TRUE;						
							//조건1부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check1_flag = TRUE;
						}					
					}
					//조건2번
					if(m_nAryDccChkSizeXData[(DeftypeNo*6) +3] && m_nAryDccChkSizeYData[(DeftypeNo*6) +3]){
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+3]) >= _ttof(strSizeX)) &&
								(_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+3]) >= _ttof(strSizeY))){
									Ret = TRUE;
									//조건2부합Flag사용하여 Panelsum에 표시
									m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeXData[(DeftypeNo*6) +3]){					
						if((_ttof(m_strAryDccSizeXData[(DeftypeNo*13) + 2]) <= _ttof(strSizeX) && _ttof(m_strAryDccSizeXData[(DeftypeNo*13)+3]) >= _ttof(strSizeX))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}else if(m_nAryDccChkSizeYData[(DeftypeNo*6) +3]){					
						if((_ttof(m_strAryDccSizeYData[(DeftypeNo*13) + 2]) <= _ttof(strSizeY) && _ttof(m_strAryDccSizeYData[(DeftypeNo*13)+3]) >= _ttof(strSizeY))){
							Ret = TRUE;
							//조건2부합Flag사용하여 Panelsum에 표시
							m_bDcc_Check2_flag = TRUE;
						}
					}
				}			
				break;			
		}		
	}	
	return Ret;
}

void   CDataManager::m_fnGradeRankInfoGet()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	
	CString cCountRoot;
	TCHAR strBuffer[255];
	CString strTemp,strTemp1,strFilePath;
	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);

	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}
	strFilePath = GRADE_SETTING_PATH + strRecipeNo + _T(".ini");
	//Grade Rank A 
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank A  - Check X
		strTemp.Format(_T("CHKA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkA_X[i] = _ttoi(strTemp);
		//Grade Rank A  - Check Y
		strTemp.Format(_T("CHKA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkA_Y[i] = _ttoi(strTemp);

		//Grade Rank A  - Size X
		strTemp.Format(_T("SIZEA_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankA_X[i] = strTemp;
		//Grade Rank A  - Size Y
		strTemp.Format(_T("SIZEA_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_A"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankA_Y[i] = strTemp;
	}
	//Grade Rank C
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank C  - Check X
		strTemp.Format(_T("CHKC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkC_X[i] = _ttoi(strTemp);
		//Grade Rank C  - Check Y
		strTemp.Format(_T("CHKC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkC_Y[i] = _ttoi(strTemp);

		//Grade Rank C  - Size X
		strTemp.Format(_T("SIZEC_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankC_X[i] = strTemp;
		//Grade Rank C  - Size Y
		strTemp.Format(_T("SIZEC_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_C"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankC_Y[i] = strTemp;
	}
	//Grade Rank B,B2
	for (int i = 0; i < 16; i++)
	{
		//Grade Rank B  - Check X
		strTemp.Format(_T("CHKB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB_X[i] = _ttoi(strTemp);
		//Grade Rank B  - Check Y
		strTemp.Format(_T("CHKB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB_Y[i] = _ttoi(strTemp);
		//Grade Rank B  - RANK
		strTemp.Format(_T("RANKB%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB_Rank[i] = strTemp;

		//Grade Rank B2  - Check X
		strTemp.Format(_T("CHKB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_nAryGradeRankChkB2_X [i] = _ttoi(strTemp);
		//Grade Rank B2  - Check Y
		strTemp.Format(_T("CHKB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_nAryGradeRankChkB2_Y[i] = _ttoi(strTemp);
		//Grade Rank B2  - RANK
		strTemp.Format(_T("RANKB2%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeB2_Rank[i] = strTemp;

	}
	//Grade Rank B,B2 Size
	for (int i = 0; i < 32; i++)
	{
		//Grade Rank B  - Size X
		strTemp.Format(_T("SIZEB_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB_X[i] = strTemp;
		//Grade Rank B  - Size Y
		strTemp.Format(_T("SIZEB_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB_Y[i] = strTemp;

		//Grade Rank B2  - Size X
		strTemp.Format(_T("SIZEB2_X%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;
		m_strAryGradeRankB2_X[i] = strTemp;
		//Grade Rank B2  - Size Y
		strTemp.Format(_T("SIZEB2_Y%d"), i+1);
		GetPrivateProfileString(_T("GRADERANK_B2"), strTemp, _T(""), strBuffer, MAX_PATH, strFilePath);
		strTemp = strBuffer;		
		m_strAryGradeRankB2_Y[i] = strTemp;
	}
}
int   CDataManager::m_fnGradeRankChk(CString strDefType,CString strDefSizeX,CString strDefSizeY,CString strDefPos)
{
	int Ret = 0;
	//Grade A
	//int m_nAryGradeRankChkA_X[16];
	//int m_nAryGradeRankChkA_Y[16];
	//CString	m_strAryGradeRankA_X[16];
	//CString	m_strAryGradeRankA_Y[16];

	////Grade B1
	//int m_nAryGradeRankChkB_X[16];
	//int m_nAryGradeRankChkB_Y[16];
	//CString	m_strAryGradeRankB_X[32];
	//CString	m_strAryGradeRankB_Y[32];

	//CString	m_strAryGradeB_Rank[16];

	////Grade B2
	//int m_nAryGradeRankChkB2_X[16];
	//int m_nAryGradeRankChkB2_Y[16];
	//CString	m_strAryGradeRankB2_X[32];
	//CString	m_strAryGradeRankB2_Y[32];

	//CString	m_strAryGradeB2_Rank[16];

	////Grade C
	//int m_nAryGradeRankChkC_X[16];
	//int m_nAryGradeRankChkC_Y[16];
	//CString	m_strAryGradeRankC_X[16];
	//CString	m_strAryGradeRankC_Y[16];

	//A Grade Rank Check S-
	if((_ttof(strDefType) == INSP_CF_BROKEN))
	{
		if(m_stDefType.CfBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkA_X[0]
		{
			if(m_nAryGradeRankChkA_X[0] && m_nAryGradeRankChkA_Y[0]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[0])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[0]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[0]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[0]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[0]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[0])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkA_X[1]
		{
			if(m_nAryGradeRankChkA_X[1] && m_nAryGradeRankChkA_Y[1]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[1])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[1]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[1]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[1]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[1]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[1])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkA_X[2]
		{
			if(m_nAryGradeRankChkA_X[2] && m_nAryGradeRankChkA_Y[2]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[2])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[2]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[2]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[2]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[2]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[2])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkA_X[3]
		{
			if(m_nAryGradeRankChkA_X[3] && m_nAryGradeRankChkA_Y[3]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[3])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[3]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[3]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[3]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[3]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[3])) 
				{
					Ret = 1;
				}
			}
		}		
	}

	if((_ttof(strDefType) == INSP_TFT_BROKEN))
	{		
		if(m_stDefType.TftBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkA_X[4]
		{
			if(m_nAryGradeRankChkA_X[4] && m_nAryGradeRankChkA_Y[4]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[4])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[4]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[4]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[4]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[4]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[4])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkA_X[5]
		{
			if(m_nAryGradeRankChkA_X[5] && m_nAryGradeRankChkA_Y[5]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[5])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[5]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[5]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[5]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[5]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[5])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkA_X[6]
		{
			if(m_nAryGradeRankChkA_X[6] && m_nAryGradeRankChkA_Y[6]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[6])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[6]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[6]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[6]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[6]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[6])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkA_X[7]
		{
			if(m_nAryGradeRankChkA_X[7] && m_nAryGradeRankChkA_Y[7]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[7])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[7]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[7]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[7]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[7]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[7])) 
				{
					Ret = 1;
				}
			}
		}			
	}
	
	if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN))
	{
		if(m_stDefType.CfCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkA_X[8]
		{
			if(m_nAryGradeRankChkA_X[8] && m_nAryGradeRankChkA_Y[8]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[8])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[8]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[8]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[8]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[8]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[8])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfCornerBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkA_X[9]
		{
			if(m_nAryGradeRankChkA_X[9] && m_nAryGradeRankChkA_Y[9]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[9])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[9]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[9]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[9]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[9]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[9])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfCornerBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkA_X[10]
		{
			if(m_nAryGradeRankChkA_X[10] && m_nAryGradeRankChkA_Y[10]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[10])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[10]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[10]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[10]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[10]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[10])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.CfCornerBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkA_X[11]
		{
			if(m_nAryGradeRankChkA_X[11] && m_nAryGradeRankChkA_Y[11]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[11])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[11]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[11]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[11]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[11]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[11])) 
				{
					Ret = 1;
				}
			}
		}				
	}
	if((_ttof(strDefType) == INSP_TFT_CORNER_BROKEN))
	{
		if(m_stDefType.TftCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkA_X[12]
		{
			if(m_nAryGradeRankChkA_X[12] && m_nAryGradeRankChkA_Y[12]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[12])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[12]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[12]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[12]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[12]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[12])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftCornerBrokenB && (strDefPos == _T("B")))	//m_nAryGradeRankChkA_X[13]
		{
			if(m_nAryGradeRankChkA_X[13] && m_nAryGradeRankChkA_Y[13]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[13])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[13]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[13]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[13]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[13]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[13])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftCornerBrokenC && (strDefPos == _T("C")))	//m_nAryGradeRankChkA_X[14]
		{
			if(m_nAryGradeRankChkA_X[14] && m_nAryGradeRankChkA_Y[14]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[14])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[14]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[14]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[14]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[14]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[14])) 
				{
					Ret = 1;
				}
			}
		}else if(m_stDefType.TftCornerBrokenD && (strDefPos == _T("D")))	//m_nAryGradeRankChkA_X[15]
		{
			if(m_nAryGradeRankChkA_X[15] && m_nAryGradeRankChkA_Y[15]){
				if((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[15])) || (_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[15]))) 
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_X[15]){
				if(_ttof(strDefSizeX) < _ttof(m_strAryGradeRankA_X[15]))
				{
					Ret = 1;
				}
			}else if(m_nAryGradeRankChkA_Y[15]){
				if(_ttof(strDefSizeY) < _ttof(m_strAryGradeRankA_Y[15])) 
				{
					Ret = 1;
				}
			}
		}				
	}
	//A Grade Rank Check E-

	//B1 Grade Rank Check S-
	if((_ttof(strDefType) == INSP_CF_BROKEN))
	{
		if(m_stDefType.CfBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB_X[0]
		{
			if(m_nAryGradeRankChkB_X[0] && m_nAryGradeRankChkB_Y[0]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[1])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[0]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[1])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[0])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[0]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[1])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[0]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[0]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[1])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[0])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB_X[1]
		{
			if(m_nAryGradeRankChkB_X[1] && m_nAryGradeRankChkB_Y[1]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[3])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[2]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[3])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[2])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[1]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[3])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[2]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[1]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[3])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[2])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB_X[2]
		{
			if(m_nAryGradeRankChkB_X[2] && m_nAryGradeRankChkB_Y[2]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[5])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[4]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[5])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[4])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[2]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[5])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[4]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[2]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[5])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[4])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB_X[3]
		{
			if(m_nAryGradeRankChkB_X[3] && m_nAryGradeRankChkB_Y[3]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[7])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[6]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[7])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[6])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[3]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[7])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[6]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[3]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[7])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[6])))) 
				{
					Ret = 2;
				}
			}
		}		
	}

	if((_ttof(strDefType) == INSP_TFT_BROKEN))
	{		
		if(m_stDefType.TftBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB_X[4]
		{
			if(m_nAryGradeRankChkB_X[4] && m_nAryGradeRankChkB_Y[4]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[9])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[8]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[9])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[8])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[4]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[9])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[8]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[4]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[9])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[8])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB_X[5]
		{
			if(m_nAryGradeRankChkB_X[5] && m_nAryGradeRankChkB_Y[5]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[11])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[10]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[11])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[10])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[5]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[11])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[10]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[5]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[11])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[10])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB_X[6]
		{
			if(m_nAryGradeRankChkB_X[6] && m_nAryGradeRankChkB_Y[6]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[13])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[12]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[13])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[12])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[6]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[13])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[12]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[6]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[13])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[12])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB_X[7]
		{
			if(m_nAryGradeRankChkB_X[7] && m_nAryGradeRankChkB_Y[7]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[15])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[14]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[15])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[14])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[7]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[15])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[14]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[7]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[15])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[14])))) 
				{
					Ret = 2;
				}
			}
		}			
	}
	
	if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN))
	{
		if(m_stDefType.CfCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB_X[8]
		{
			if(m_nAryGradeRankChkB_X[8] && m_nAryGradeRankChkB_Y[8]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[17])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[16]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[17])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[16])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[8]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[17])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[16]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[8]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[17])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[16])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfCornerBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB_X[9]
		{
			if(m_nAryGradeRankChkB_X[9] && m_nAryGradeRankChkB_Y[9]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[19])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[18]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[19])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[18])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[9]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[19])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[18]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[9]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[19])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[18])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfCornerBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB_X[10]
		{
			if(m_nAryGradeRankChkB_X[10] && m_nAryGradeRankChkB_Y[10]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[21])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[20]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[21])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[20])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[10]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[21])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[20]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[10]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[21])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[20])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.CfCornerBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB_X[11]
		{
			if(m_nAryGradeRankChkB_X[11] && m_nAryGradeRankChkB_Y[11]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[23])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[22]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[23])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[22])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[11]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[23])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[22]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[11]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[23])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[22])))) 
				{
					Ret = 2;
				}
			}
		}				
	}
	if((_ttof(strDefType) == INSP_TFT_CORNER_BROKEN))
	{
		if(m_stDefType.TftCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB_X[12]
		{
			if(m_nAryGradeRankChkB_X[12] && m_nAryGradeRankChkB_Y[12]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[25])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[24]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[25])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[24])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[12]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[25])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[24]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[12]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[25])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[24])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftCornerBrokenB && (strDefPos == _T("B")))	//m_nAryGradeRankChkB_X[13]
		{
			if(m_nAryGradeRankChkB_X[13] && m_nAryGradeRankChkB_Y[13]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[27])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[26]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[27])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[26])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[13]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[27])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[26]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[13]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[27])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[26])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftCornerBrokenC && (strDefPos == _T("C")))	//m_nAryGradeRankChkB_X[14]
		{
			if(m_nAryGradeRankChkB_X[14] && m_nAryGradeRankChkB_Y[14]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[29])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[28]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[29])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[28])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[14]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[29])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[28]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[14]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[29])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[28])))) 
				{
					Ret = 2;
				}
			}
		}else if(m_stDefType.TftCornerBrokenD && (strDefPos == _T("D")))	//m_nAryGradeRankChkB_X[15]
		{
			if(m_nAryGradeRankChkB_X[15] && m_nAryGradeRankChkB_Y[15]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[31])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[30]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[31])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[30])))) 
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_X[15]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB_X[31])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB_X[30]))))
				{
					Ret = 2;
				}
			}else if(m_nAryGradeRankChkB_Y[15]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB_Y[31])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[30])))) 
				{
					Ret = 2;
				}
			}
		}				
	}
	//B1 Grade Rank Check E-

	//B2 Grade Rank Check S-
	if((_ttof(strDefType) == INSP_CF_BROKEN))
	{
		if(m_stDefType.CfBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB2_X[0]
		{
			if(m_nAryGradeRankChkB2_X[0] && m_nAryGradeRankChkB2_Y[0]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[1])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[0]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[1])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[0])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[0]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[1])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[0]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[0]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[1])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[0])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB2_X[1]
		{
			if(m_nAryGradeRankChkB2_X[1] && m_nAryGradeRankChkB2_Y[1]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[3])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[2]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[3])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[2])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[1]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[3])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[2]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[1]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[3])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[2])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB2_X[2]
		{
			if(m_nAryGradeRankChkB2_X[2] && m_nAryGradeRankChkB2_Y[2]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[5])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[4]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[5])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[4])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[2]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[5])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[4]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[2]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[5])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[4])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB2_X[3]
		{
			if(m_nAryGradeRankChkB2_X[3] && m_nAryGradeRankChkB2_Y[3]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[7])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[6]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[7])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[6])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[3]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[7])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[6]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[3]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[7])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[6])))) 
				{
					Ret = 3;
				}
			}
		}		
	}

	if((_ttof(strDefType) == INSP_TFT_BROKEN))
	{		
		if(m_stDefType.TftBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB2_X[4]
		{
			if(m_nAryGradeRankChkB2_X[4] && m_nAryGradeRankChkB2_Y[4]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[9])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[8]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[9])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[8])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[4]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[9])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[8]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[4]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[9])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[8])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB2_X[5]
		{
			if(m_nAryGradeRankChkB2_X[5] && m_nAryGradeRankChkB2_Y[5]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[11])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[10]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[11])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[10])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[5]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[11])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[10]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[5]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[11])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[10])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB2_X[6]
		{
			if(m_nAryGradeRankChkB2_X[6] && m_nAryGradeRankChkB2_Y[6]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[13])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[12]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[13])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[12])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[6]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[13])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[12]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[6]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[13])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[12])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB2_X[7]
		{
			if(m_nAryGradeRankChkB2_X[7] && m_nAryGradeRankChkB2_Y[7]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[15])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[14]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[15])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[14])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[7]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[15])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[14]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[7]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[15])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[14])))) 
				{
					Ret = 3;
				}
			}
		}			
	}
	
	if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN))
	{
		if(m_stDefType.CfCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB2_X[8]
		{
			if(m_nAryGradeRankChkB2_X[8] && m_nAryGradeRankChkB2_Y[8]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[17])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[16]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[17])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[16])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[8]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[17])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[16]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[8]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[17])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[16])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfCornerBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkB2_X[9]
		{
			if(m_nAryGradeRankChkB2_X[9] && m_nAryGradeRankChkB2_Y[9]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[19])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[18]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[19])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[18])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[9]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[19])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[18]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[9]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[19])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[18])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfCornerBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkB2_X[10]
		{
			if(m_nAryGradeRankChkB2_X[10] && m_nAryGradeRankChkB2_Y[10]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[21])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[20]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[21])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[20])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[10]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[21])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[20]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[10]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[21])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[20])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.CfCornerBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkB2_X[11]
		{
			if(m_nAryGradeRankChkB2_X[11] && m_nAryGradeRankChkB2_Y[11]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[23])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[22]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[23])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[22])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[11]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[23])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[22]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[11]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[23])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[22])))) 
				{
					Ret = 3;
				}
			}
		}				
	}
	if((_ttof(strDefType) == INSP_TFT_CORNER_BROKEN))
	{
		if(m_stDefType.TftCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkB2_X[12]
		{
			if(m_nAryGradeRankChkB2_X[12] && m_nAryGradeRankChkB2_Y[12]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[25])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[24]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[25])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[24])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[12]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[25])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[24]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[12]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[25])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[24])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftCornerBrokenB && (strDefPos == _T("B")))	//m_nAryGradeRankChkB2_X[13]
		{
			if(m_nAryGradeRankChkB2_X[13] && m_nAryGradeRankChkB2_Y[13]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[27])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[26]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[27])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[26])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[13]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[27])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[26]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[13]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[27])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[26])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftCornerBrokenC && (strDefPos == _T("C")))	//m_nAryGradeRankChkB2_X[14]
		{
			if(m_nAryGradeRankChkB2_X[14] && m_nAryGradeRankChkB2_Y[14]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[29])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[28]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[29])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[28])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[14]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[29])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[28]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[14]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[29])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[28])))) 
				{
					Ret = 3;
				}
			}
		}else if(m_stDefType.TftCornerBrokenD && (strDefPos == _T("D")))	//m_nAryGradeRankChkB2_X[15]
		{
			if(m_nAryGradeRankChkB2_X[15] && m_nAryGradeRankChkB2_Y[15]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[31])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[30]))) 
					|| ((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[31])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB2_Y[30])))) 
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_X[15]){
				if(((_ttof(strDefSizeX) < _ttof(m_strAryGradeRankB2_X[31])) && (_ttof(strDefSizeX) > _ttof(m_strAryGradeRankB2_X[30]))))
				{
					Ret = 3;
				}
			}else if(m_nAryGradeRankChkB2_Y[15]){
				if(((_ttof(strDefSizeY) < _ttof(m_strAryGradeRankB2_Y[31])) && (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankB_Y[30])))) 
				{
					Ret = 3;
				}
			}
		}				
	}
	//B2 Grade Rank Check E-

	//C Grade Rank Check S-
	if((_ttof(strDefType) == INSP_CF_BROKEN))
	{
		if(m_stDefType.CfBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkC_X[0]
		{
			if(m_nAryGradeRankChkC_X[0] && m_nAryGradeRankChkC_Y[0]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[0])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[0]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[0]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[0]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[0]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[0])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkC_X[1]
		{
			if(m_nAryGradeRankChkC_X[1] && m_nAryGradeRankChkC_Y[1]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[1])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[1]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[1]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[1]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[1]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[1])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkC_X[2]
		{
			if(m_nAryGradeRankChkC_X[2] && m_nAryGradeRankChkC_Y[2]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[2])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[2]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[2]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[2]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[2]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[2])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkC_X[3]
		{
			if(m_nAryGradeRankChkC_X[3] && m_nAryGradeRankChkC_Y[3]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[3])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[3]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[3]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[3]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[3]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[3])) 
				{
					Ret = 4;
				}
			}
		}		
	}

	if((_ttof(strDefType) == INSP_TFT_BROKEN))
	{		
		if(m_stDefType.TftBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkC_X[4]
		{
			if(m_nAryGradeRankChkC_X[4] && m_nAryGradeRankChkC_Y[4]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[4])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[4]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[4]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[4]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[4]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[4])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkC_X[5]
		{
			if(m_nAryGradeRankChkC_X[5] && m_nAryGradeRankChkC_Y[5]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[5])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[5]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[5]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[5]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[5]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[5])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkC_X[6]
		{
			if(m_nAryGradeRankChkC_X[6] && m_nAryGradeRankChkC_Y[6]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[6])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[6]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[6]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[6]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[6]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[6])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkC_X[7]
		{
			if(m_nAryGradeRankChkC_X[7] && m_nAryGradeRankChkC_Y[7]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[7])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[7]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[7]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[7]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[7]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[7])) 
				{
					Ret = 4;
				}
			}
		}			
	}
	
	if((_ttof(strDefType) == INSP_CF_CORNER_BROKEN))
	{
		if(m_stDefType.CfCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkC_X[8]
		{
			if(m_nAryGradeRankChkC_X[8] && m_nAryGradeRankChkC_Y[8]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[8])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[8]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[8]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[8]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[8]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[8])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfCornerBrokenB && (strDefPos == _T("B")))		//m_nAryGradeRankChkC_X[9]
		{
			if(m_nAryGradeRankChkC_X[9] && m_nAryGradeRankChkC_Y[9]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[9])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[9]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[9]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[9]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[9]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[9])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfCornerBrokenC && (strDefPos == _T("C")))		//m_nAryGradeRankChkC_X[10]
		{
			if(m_nAryGradeRankChkC_X[10] && m_nAryGradeRankChkC_Y[10]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[10])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[10]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[10]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[10]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[10]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[10])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.CfCornerBrokenD && (strDefPos == _T("D")))		//m_nAryGradeRankChkC_X[11]
		{
			if(m_nAryGradeRankChkC_X[11] && m_nAryGradeRankChkC_Y[11]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[11])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[11]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[11]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[11]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[11]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[11])) 
				{
					Ret = 4;
				}
			}
		}				
	}
	if((_ttof(strDefType) == INSP_TFT_CORNER_BROKEN))
	{
		if(m_stDefType.TftCornerBrokenA && (strDefPos == _T("A")))			//m_nAryGradeRankChkC_X[12]
		{
			if(m_nAryGradeRankChkC_X[12] && m_nAryGradeRankChkC_Y[12]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[12])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[12]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[12]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[12]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[12]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[12])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftCornerBrokenB && (strDefPos == _T("B")))	//m_nAryGradeRankChkC_X[13]
		{
			if(m_nAryGradeRankChkC_X[13] && m_nAryGradeRankChkC_Y[13]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[13])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[13]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[13]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[13]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[13]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[13])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftCornerBrokenC && (strDefPos == _T("C")))	//m_nAryGradeRankChkC_X[14]
		{
			if(m_nAryGradeRankChkC_X[14] && m_nAryGradeRankChkC_Y[14]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[14])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[14]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[14]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[14]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[14]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[14])) 
				{
					Ret = 4;
				}
			}
		}else if(m_stDefType.TftCornerBrokenD && (strDefPos == _T("D")))	//m_nAryGradeRankChkC_X[15]
		{
			if(m_nAryGradeRankChkC_X[15] && m_nAryGradeRankChkC_Y[15]){
				if((_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[15])) || (_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[15]))) 
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_X[15]){
				if(_ttof(strDefSizeX) > _ttof(m_strAryGradeRankC_X[15]))
				{
					Ret = 4;
				}
			}else if(m_nAryGradeRankChkC_Y[15]){
				if(_ttof(strDefSizeY) > _ttof(m_strAryGradeRankC_Y[15])) 
				{
					Ret = 4;
				}
			}
		}				
	}
	//C Grade Rank Check E-

	return Ret;
}
//2020.08.05 add by mby S-
int   CDataManager::m_fnPACntPDCheck()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	int Ret = 0;

	int m_nAryPaCntPDChkTemp[DCC_CHK_COUNT];
	int m_nAryPaCntPDTemp[DCC_CHK_COUNT];
	int m_nAryPaCntEDGEChkTemp[16];	//2020.08.26 add by mby
	int m_nAryPaCntEDGETemp[16];		//2020.08.26 add by mby
	int m_nCntTotal;

	int nRead = 0;
	TCHAR strBuffer[255];
	CString strTemp,cCountRoot;

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}

	//설정값 불러오기
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("PACNTPD%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDTemp[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTPDCHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDChkTemp[i] = _ttoi(strTemp);
	}
	m_nCntTotal = m_nArrayPaCntPDCount[0] + m_nArrayPaCntPDCount[1] + m_nArrayPaCntPDCount[2] + m_nArrayPaCntPDCount[3];	//All Count
	//2020.08.26 add by mby S-
	for (int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("PACNTEDGE%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEDGETemp[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTEDGECHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEDGEChkTemp[i] = _ttoi(strTemp);
	}
	for (int i = 0; i < 16; i++)
	{
		if(m_nAryPaCntEDGEChkTemp[i] && m_nAryPaCntEDGETemp[i]){	//check되어있고 Count가 0이 아닐때
			if(m_nArrayPaCntEdgeCount[i] >= m_nAryPaCntEDGETemp[i])
			{
				Ret = 1;
			}
		}
		m_nArrayPaCntEdgeCount[i] = 0;
	}
	if(!Ret){	
		//2020.08.26 add by mby E-
		for (int i = 0; i < DCC_CHK_COUNT; i++)
		{
			if(m_nAryPaCntPDChkTemp[i] && m_nAryPaCntPDTemp[i]){	//check되어있고 Count가 0이 아닐때
				if(i == 4){	//All Count				
					if(m_nCntTotal >= m_nAryPaCntPDTemp[i]){
						Ret = 1;
					}
				}//4개항목별 비교
				else if(m_nArrayPaCntPDCount[i] >= m_nAryPaCntPDTemp[i])
				{
					Ret = 1;
				}
			}
			m_nArrayPaCntPDCount[i] = 0;
		}
	}else{
		for (int i = 0; i < DCC_CHK_COUNT; i++)
		{			
			m_nArrayPaCntPDCount[i] = 0;
		}

	}
	return Ret;
}
//2020.08.05 add by mby E-
//2020.12.08 add by mby S-
int   CDataManager::m_fnPACntPDCheck_TypeA(int TNum,int Num,CString strPos)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	CViewAuto* pAuto = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	int Ret = 0;

	int m_nAryPaCntPDChkTemp[DCC_CHK_COUNT];
	int m_nAryPaCntPDTemp[DCC_CHK_COUNT];
	int m_nAryPaCntEDGEChkTemp[16];
	int m_nAryPaCntEDGETemp[16];
	int m_nCntTotal;

	int nRead = 0;
	TCHAR strBuffer[255];
	CString strTemp,cCountRoot;

	cCountRoot.Format(_T("%s%s"), ROOT_PATH, DCC_INFO_DIS_PATH);
	pFrame->m_iniConfigSet.GetProfileStringW(SECTION_RECIPE_NO, KEY_LAST_NO, _T("0"), strBuffer, 255);

	strRecipeNo = strBuffer;
	if(strRecipeNo == _T("")){
		GetPrivateProfileString(_T("LAST"), _T("INCH"), _T(""), strBuffer, sizeof(strBuffer), cCountRoot);
		strRecipeNo = strBuffer;
	}

	//설정값 불러오기
	for (int i = 0; i < DCC_CHK_COUNT; i++)
	{
		strTemp.Format(_T("PACNTPD%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDTemp[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTPDCHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntPDChkTemp[i] = _ttoi(strTemp);
	}
	for (int i = 0; i < 16; i++)
	{
		strTemp.Format(_T("PACNTEDGE%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEDGETemp[i] = _ttoi(strTemp);

		strTemp.Format(_T("PACNTEDGECHK%d"), i+1);
		GetPrivateProfileString(strRecipeNo, strTemp, _T(""), strBuffer, sizeof(strBuffer), cCountRoot);		
		strTemp = strBuffer;
		m_nAryPaCntEDGEChkTemp[i] = _ttoi(strTemp);
	}
	
	int k = Num - 5;	//2021.04.26 add by mby
	for ( k ; k < Num; k++)
	{
		//G_AddLog(3, 3, _T("m_fnPACntPDCheck_TypeA() k= %d - "),k);
		if(m_nAryPaCntEDGEChkTemp[k] && m_nAryPaCntEDGETemp[k]){	//check되어있고 Count가 0이 아닐때
			if(m_nArrayPaCntEdgeCount[k] >= m_nAryPaCntEDGETemp[k])
			{
				Ret = 1;
			}
		}
		//2021.04.23 add by mby S-
		if(!m_nAryPaCntEDGEChkTemp[k]){
			if(strPos == _T("A") && (k % 4 == 0)){
				if(TNum == 0){
					m_nArrayPaCntPDCount[0] -= 1;	
				}else if(TNum == 1){
					m_nArrayPaCntPDCount[1] -= 1;	
				}else if(TNum == 2){
					m_nArrayPaCntPDCount[2] -= 1;	
				}else{
					m_nArrayPaCntPDCount[3] -= 1;	
				}				
			}else if(strPos == _T("B") && (k % 4 == 1)){
				if(TNum == 0){
					m_nArrayPaCntPDCount[0] -= 1;	
				}else if(TNum == 1){
					m_nArrayPaCntPDCount[1] -= 1;	
				}else if(TNum == 2){
					m_nArrayPaCntPDCount[2] -= 1;	
				}else{
					m_nArrayPaCntPDCount[3] -= 1;	
				}
			}else if(strPos == _T("C") && (k % 4 == 2)){
				if(TNum == 0){
					m_nArrayPaCntPDCount[0] -= 1;	
				}else if(TNum == 1){
					m_nArrayPaCntPDCount[1] -= 1;	
				}else if(TNum == 2){
					m_nArrayPaCntPDCount[2] -= 1;	
				}else{
					m_nArrayPaCntPDCount[3] -= 1;	
				}
			}else if(strPos == _T("D") && (k % 4 == 3)){
				if(TNum == 0){
					m_nArrayPaCntPDCount[0] -= 1;	
				}else if(TNum == 1){
					m_nArrayPaCntPDCount[1] -= 1;	
				}else if(TNum == 2){
					m_nArrayPaCntPDCount[2] -= 1;	
				}else{
					m_nArrayPaCntPDCount[3] -= 1;	
				}
			}
		}
		//2021.04.23 add by mby E-
	}

	if(!Ret){			
		if(m_nAryPaCntPDChkTemp[TNum] && m_nAryPaCntPDTemp[TNum]){	//check되어있고 Count가 0이 아닐때
			if(m_nArrayPaCntPDCount[TNum] >= m_nAryPaCntPDTemp[TNum])
			{
				Ret = 1;
			}
		}		
	}
	return Ret;
}
//2020.12.08 add by mby E-