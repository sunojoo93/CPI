// CThreadProcCIM.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ThreadProcData.h"
#include "ViewSetup.h"
#include "ViewAuto.h"
#include "MainFrm.h"
#include "DlgPanelJudge.h"

// CThreadProcCIM

IMPLEMENT_DYNCREATE(CThreadProcData, CWinThread)

CThreadProcData::CThreadProcData()
{
	m_hThreadData = NULL;
	//m_hThreadCim = CreateEvent(NULL,TRUE, FALSE,NULL);
	::InitializeCriticalSection(&m_csThreadData);
}

CThreadProcData::~CThreadProcData()
{
	m_hThreadData =NULL;
	DeleteCriticalSection(&m_csThreadData);
}

BOOL CThreadProcData::InitInstance()
{
	return TRUE;
}

int CThreadProcData::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}

unsigned __stdcall CThreadProcData::m_fnOverlapDefectData(LPVOID pArg)
{
    G_AddLog(3, 3, _T("m_fnOverlapDefectData() - Start"));
	CDataManager *pData = (CDataManager *)pArg;			
	CMainFrame* pFrame = (CMainFrame*)(AfxGetApp()->GetMainWnd());
	
	EnterCriticalSection(& pData->pTreadProcData->m_csThreadData); 
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	int nDefectCnt, nOverlapCnt,nstNextCnt, nImgCnt, nInspType;
	CString strLine, strItem, strNewLineDefect, strDefectCoorPath,strPnlDefectPath, strLineDefect, strAddDefect;
	CString strOverlapDefect, strLogOverlapDefect, strPanelData, strImgPath, strLineTemp, strPanelId, strFilePath;
	CStdioFile cstFile;
	CString strDefectType;	
	CString strMsg, strLineEdgeDistCF, strLineEdgeDistTFT, strLinePolDistCF, strLinePolDistTFT;

	strDefectCoorPath.Format(_T("%s%s"), ROOT_PATH, DEFECT_DATA_PATH);
	
	nDefectCnt =  pFrame->m_DataManager.m_fnGetSearchDefectCnt(strDefectCoorPath, 0);

	DOUBLE dOffsetX = pFrame->m_stOverlapCoordInfo->dOffsetX;
	DOUBLE dOffsetY = pFrame->m_stOverlapCoordInfo->dOffsetY;

	nDefectCnt = pFrame->m_stOverlapCoordInfo->nPanelCnt;
	STRU_DEFECT_COORD_INFO* stOverlapChkDef  = new STRU_DEFECT_COORD_INFO[nDefectCnt]();
	STRU_DEFECT_COORD_INFO* stOverlapCompDef = new STRU_DEFECT_COORD_INFO();
	STRU_DEFECT_COORD_DATA stTemp;
	STRU_CLASSIFY_INFO	stPcClassifyEndFlagTemp;

	FILE *in = NULL, *out = NULL;
	errno_t errFileOpen;		
	size_t read_size = 0;

	strOverlapDefect = _T("");
	errFileOpen = _tfopen_s(&in, strDefectCoorPath , _T("rb+")); // 구조체 바이너리 파일을 읽음
	if(errFileOpen == 0)
	{		
		read_size = fread(&stOverlapChkDef->stPnlInfo, sizeof(STRU_DEFECT_COORD_INFO) * nDefectCnt , 1, in);			
		fclose(in);
	}

	strOverlapDefect = _T("");
	nOverlapCnt = 0;
	nstNextCnt  = 0;

	strPnlDefectPath = pData->pTreadProcData->m_strTheadData;

	strPanelId = pFrame->m_DataManager.m_fnGetOverlapDefectData((STRU_DEFECT_COORD_INFO *) stOverlapCompDef, strPnlDefectPath);



	//////read contact result - s
	//int nVisionCnt = 0;
	//while(nVisionCnt < MAX_REV_PANEL)
	//{		
	//	CString cCompareVal = pFrame->m_DataManager.m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].cPanelID ; cCompareVal.TrimRight();

	//	if (!cCompareVal.IsEmpty() && cCompareVal == strPanelId)	
	//	{
	//		if( pFrame->m_DataManager.m_stPcClassifyEndFlag[nVisionCnt][PANEL_ONE].nPgGrade == 1 )
	//			int a;
	//	}

	//}
	//////read contact result - e

	for (int i = 0; i < nDefectCnt ; i++ )
	{
		if( i != 0 &&  stOverlapChkDef[i].stPnlInfo[0].startX == 0 && stOverlapChkDef[i].stPnlInfo[0].startY == 0 
			&& stOverlapChkDef[i].stPnlInfo[0].endX   == 0 && stOverlapChkDef[i].stPnlInfo[0].endY   == 0 
			&& stOverlapChkDef[i].stPnlInfo[0].nDefectType == 0 )	break;

		for (int j = 0; j < MAX_DEFECT_CNT ; j++ )
		{
			if(	 j != 0 && stOverlapChkDef[i].stPnlInfo[j].startX  == 0 && stOverlapChkDef[i].stPnlInfo[j].startY == 0 
				&& stOverlapChkDef[i].stPnlInfo[j].endX    == 0 && stOverlapChkDef[i].stPnlInfo[j].endY   == 0 
				&& stOverlapChkDef[i].stPnlInfo[j].nDefectType == 0 
				)	break;

			stOverlapChkDef[i].stPnlInfo[j].nPanelCnt++; // 셀의 갯수를 카운팅 한다.
			if (stOverlapChkDef[i].stPnlInfo[j].nPanelCnt >= nDefectCnt)
			{
				for (int nSortCnt = j; nSortCnt < MAX_DEFECT_CNT - 1 ;  nSortCnt++)
				{
					if (nSortCnt >= MAX_DEFECT_CNT) break;
					stTemp.init_info();
					stTemp = stOverlapChkDef[i].stPnlInfo[nSortCnt];
					stOverlapChkDef[i].stPnlInfo[nSortCnt] = stOverlapChkDef[i].stPnlInfo[nSortCnt+1];
					stOverlapChkDef[i].stPnlInfo[nSortCnt+1] = stTemp;
				}
			}
			for (int k = 0; k < MAX_DEFECT_CNT ; k++)
			{
				//2017.08.07   PROTECTION 검사번호 변경으로 이 후 추가 번호들에 대한 예외처리		//2017.07.29	Corner Err
				if( stOverlapCompDef->stPnlInfo[k].nDefectType == INSP_CF_PROTECTION || stOverlapCompDef->stPnlInfo[k].nDefectType == INSP_TFT_PROTECTION )
				{
					nInspType = INSP_ETC_POINT + ( stOverlapCompDef->stPnlInfo[k].nDefectType - INSP_TDI_LUMINANCE );
				}
				else
					nInspType = stOverlapCompDef->stPnlInfo[k].nDefectType;
				//

				if(	stOverlapCompDef->stPnlInfo[k].startX == 0 && stOverlapCompDef->stPnlInfo[k].startY == 0 
					&& stOverlapCompDef->stPnlInfo[k].endX   == 0 && stOverlapCompDef->stPnlInfo[k].endY   == 0 
					&& stOverlapCompDef->stPnlInfo[k].nDefectType == 0 
				) break;

				if( (pFrame->m_fnEqpSizeType() == EQP_LARGE_SIZE && pFrame->m_stOverlapCoordInfo->bAllOverlapDef 
					//&& pFrame->m_stOverlapCoordInfo->nDefectCheck[stOverlapCompDef->stPnlInfo[k].nDefectType] == TRUE) ||  // 2016.03.23 add by ikm
					&& pFrame->m_stOverlapCoordInfo->nDefectCheck[nInspType] == TRUE) ||  // 2016.03.23 add by ikm
					(stOverlapChkDef[i].stPnlInfo[j].startX	  -	 dOffsetX <= stOverlapCompDef->stPnlInfo[k].startX
					&& stOverlapCompDef->stPnlInfo[k].startX	    	  <= stOverlapChkDef[i].stPnlInfo[j].startX + dOffsetX
					&& stOverlapChkDef[i].stPnlInfo[j].startY -	 dOffsetY <= stOverlapCompDef->stPnlInfo[k].startY 
					&& stOverlapCompDef->stPnlInfo[k].startY		      <= stOverlapChkDef[i].stPnlInfo[j].startY + dOffsetY
					&& stOverlapChkDef[i].stPnlInfo[j].endX	  -	 dOffsetX <= stOverlapCompDef->stPnlInfo[k].endX 
					&& stOverlapCompDef->stPnlInfo[k].endX				  <= stOverlapChkDef[i].stPnlInfo[j].endX   + dOffsetX
					&& stOverlapChkDef[i].stPnlInfo[j].endY	  -	 dOffsetY <= stOverlapCompDef->stPnlInfo[k].endY 
					&& stOverlapCompDef->stPnlInfo[k].endY				  <= stOverlapChkDef[i].stPnlInfo[j].endY   + dOffsetY
					&& stOverlapChkDef[i].stPnlInfo[j].nDefectType		  == stOverlapCompDef->stPnlInfo[k].nDefectType 
					&& stOverlapChkDef[i].stPnlInfo[j].nFlag			  == stOverlapCompDef->stPnlInfo[k].nFlag 
					//&& pFrame->m_stOverlapCoordInfo->nDefectCheck[stOverlapCompDef->stPnlInfo[k].nDefectType] == TRUE
					&& pFrame->m_stOverlapCoordInfo->nDefectCheck[nInspType] == TRUE
					/*	&& stOverlapChkDef[i].stPnlInfo[j].startX  != 0 && stOverlapChkDef[i].stPnlInfo[j].startY != 0 
					&& stOverlapChkDef[i].stPnlInfo[j].endX    != 0 && stOverlapChkDef[i].stPnlInfo[j].endY   != 0 
					&& stOverlapChkDef[i].stPnlInfo[j].nDefectType != 0 */)
					)
				{
					stOverlapChkDef[i].stPnlInfo[j].nCount += 1;


					//if (pFrame->m_stOverlapCoordInfo->nDefectCount[stOverlapCompDef->stPnlInfo[k].nDefectType] 
					if (pFrame->m_stOverlapCoordInfo->nDefectCount[nInspType] 
						<= stOverlapChkDef[i].stPnlInfo[j].nCount)
					{
						//CString strTemp = stOverlapChkDef[i].stPnlInfo[j].strImage;
						strDefectType = pFrame->m_DataManager.m_fnGetOverlapDefectType(stOverlapCompDef->stPnlInfo[k].nDefectType, stOverlapCompDef->stPnlInfo[k].nFlag);
						strAddDefect.Format(_T("%0.3f,%0.3f,%0.3f,%0.3f,%s,%s;")
							,stOverlapChkDef[i].stPnlInfo[j].startX,stOverlapChkDef[i].stPnlInfo[j].startY,
							stOverlapChkDef[i].stPnlInfo[j].endX, stOverlapChkDef[i].stPnlInfo[j].endY, 
							strDefectType, stOverlapChkDef[i].stPnlInfo[j].strImage);
						//G_AddLog(2, 3, _T("Overlap Defect : %d"), strLogOverlapDefect, GetLastError());
						strOverlapDefect += strAddDefect;
						nOverlapCnt++;
						stOverlapChkDef[i].stPnlInfo[j].init_info();

						for (int z = j; z <  MAX_DEFECT_CNT - 1 ;  z++)
						{
							if (z>= MAX_DEFECT_CNT) break;
							stTemp.init_info();
							stTemp = stOverlapChkDef[i].stPnlInfo[z];
							stOverlapChkDef[i].stPnlInfo[z] = stOverlapChkDef[i].stPnlInfo[z+1];
							stOverlapChkDef[i].stPnlInfo[z+1] = stTemp;
						}
					}
					stOverlapCompDef->stPnlInfo[k].init_info();
				}

				//else if (j ==0 && pFrame->m_stOverlapCoordInfo->nDefectCount[stOverlapCompDef->stPnlInfo[k].nDefectType] == 0
				else if (j ==0 && pFrame->m_stOverlapCoordInfo->nDefectCount[nInspType] == 0
						//&& pFrame->m_stOverlapCoordInfo->nDefectCheck[stOverlapCompDef->stPnlInfo[k].nDefectType] == TRUE
						&& pFrame->m_stOverlapCoordInfo->nDefectCheck[nInspType] == TRUE
						&& stOverlapCompDef->stPnlInfo[k].startX  != 0 && stOverlapCompDef->stPnlInfo[k].startY != 0 
						&& stOverlapCompDef->stPnlInfo[k].endX    != 0 && stOverlapCompDef->stPnlInfo[k].endY   != 0 
					)
				{			
					strDefectType = pFrame->m_DataManager.m_fnGetOverlapDefectType(stOverlapCompDef->stPnlInfo[k].nDefectType, stOverlapCompDef->stPnlInfo[k].nFlag);
					strAddDefect.Format(_T("%0.3f,%0.3f,%0.3f,%0.3f,%s,%s;")
						,stOverlapCompDef->stPnlInfo[k].startX,stOverlapCompDef->stPnlInfo[k].startY,
						stOverlapCompDef->stPnlInfo[k].endX, stOverlapCompDef->stPnlInfo[k].endY, 
						strDefectType, stOverlapCompDef->stPnlInfo[k].strImage);
					//G_AddLog(2, 3, _T("Overlap Defect : %d"), strLogOverlapDefect, GetLastError());
					strOverlapDefect += strAddDefect;
					nOverlapCnt++;
					stOverlapCompDef->stPnlInfo[k].init_info();					
				}
				//else if(stOverlapCompDef->stPnlInfo[k].nDefectType == INSP_CONTACT && pFrame->m_stOverlapCoordInfo->bUseContactFail
				//	&& pFrame->m_stOverlapCoordInfo->nContactFailCount
				//	)
				//{
				//	strDefectType = pFrame->m_DataManager.m_fnGetOverlapDefectType(stOverlapCompDef->stPnlInfo[k].nDefectType, stOverlapCompDef->stPnlInfo[k].nFlag);
				//	strAddDefect.Format(_T("%0.3f,%0.3f,%0.3f,%0.3f,%s,%s;")
				//		,stOverlapCompDef->stPnlInfo[k].startX,stOverlapCompDef->stPnlInfo[k].startY,
				//		stOverlapCompDef->stPnlInfo[k].endX, stOverlapCompDef->stPnlInfo[k].endY, 
				//		strDefectType, stOverlapCompDef->stPnlInfo[k].strImage);
				//	//G_AddLog(2, 3, _T("Overlap Defect : %d"), strLogOverlapDefect, GetLastError());
				//	strOverlapDefect += strAddDefect;
				//	nOverlapCnt++;
				//	stOverlapCompDef->stPnlInfo[k].init_info();		
				//}
			}					
		}
		nstNextCnt++;
	}
	
	if (!strOverlapDefect.IsEmpty() && !strPanelId.IsEmpty())
	{		
		CViewAuto* pView = (CViewAuto*)pFrame->m_fnGetViewPtr(eFORM_Auto);
		CString strSendMsg = _T("");
		strSendMsg.Append(strOverlapDefect);
		strSendMsg.Append(_T("#"));
		strSendMsg.Append(strPanelId);
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
			pView->SendMessage(WM_UI_NEW_MSG_DIALOG, (WPARAM)nOverlapCnt, (LPARAM)cTempBuff ); 			 		
			delete[] cBuffer;
		}		
	}

	
	for (int k = MAX_DEFECT_CNT - 1 ; k >=  0 ;  k--)
	{
		if ( pFrame->m_stOverlapCoordInfo->nDefectCheck[stOverlapCompDef->stPnlInfo[k].nDefectType] != TRUE)
		{
			stOverlapCompDef->stPnlInfo[k].init_info();
		}

		if ( stOverlapCompDef->stPnlInfo[k].startX == 0 &&	stOverlapCompDef->stPnlInfo[k].startY == 0 
			&&	stOverlapCompDef->stPnlInfo[k].endX == 0 &&	stOverlapCompDef->stPnlInfo[k].endY == 0)
		{
			for (int z = k; z <  MAX_DEFECT_CNT ;  z++)
			{
				
				strImgPath = stOverlapCompDef->stPnlInfo[z].strImage;
				if (   /* stOverlapCompDef->stPnlInfo[k].startX != 0 &&	stOverlapCompDef->stPnlInfo[k].startY != 0 &&	*/ !strImgPath.IsEmpty()
					/*&&	stOverlapCompDef->stPnlInfo[k].endX != 0 &&	stOverlapCompDef->stPnlInfo[k].endY != 0 &&	stOverlapCompDef->stPnlInfo[k].nDefectType != 0*/)
				{
					stTemp.init_info();
					stTemp = stOverlapCompDef->stPnlInfo[z];
					stOverlapCompDef->stPnlInfo[z] = stOverlapCompDef->stPnlInfo[z-1];
					stOverlapCompDef->stPnlInfo[z-1] = stTemp;
				}								 
			}
		}
	}
	if(nstNextCnt != nDefectCnt)
	{

		*(STRU_DEFECT_COORD_INFO *)stOverlapChkDef[nstNextCnt].stPnlInfo = *(STRU_DEFECT_COORD_INFO*)&stOverlapCompDef->stPnlInfo;
	}

	errFileOpen = _tfopen_s(&out, strDefectCoorPath, _T("wb"));
	if(errFileOpen == 0)
	{				
		fwrite( &stOverlapChkDef[0].stPnlInfo,sizeof(STRU_DEFECT_COORD_INFO) * nDefectCnt , 1 ,  out);		
		fclose(out);
	}		

	delete[]  stOverlapChkDef;
	delete    stOverlapCompDef;

	CloseHandle(pData->pTreadProcData->m_hThreadData);
	pData->pTreadProcData->m_hThreadData = NULL;

	LeaveCriticalSection(& pData->pTreadProcData->m_csThreadData);
	 G_AddLog(3, 3, _T("m_fnOverlapDefectData() - End"));
	return 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



unsigned __stdcall CThreadProcData::m_fnDeleteAllFolder(LPVOID pArg)
{
	//G_AddLog(3, 3, _T("m_fnDeleteAllFolder() - Start"));
	CDataManager* pData = (CDataManager*)pArg;

	CString strPath, strOriPath;

	strPath = pData->m_strDelPath;
	strOriPath = pData->m_strDelOrignPath;

	CFileFind		finder;
	CString			strLowFolderPath, strHighFolderPath, strFileName;
	BOOL			bWorking;

	CTime ctCurrentTime;
	CTime ctCreated;
	CTimeSpan tsInterval;

	if (strPath.IsEmpty()) return FALSE;
	

	bWorking = finder.FindFile(strPath +_T("\\*.*"));
	if( !bWorking )
	{
		//CloseHandle(pData->pTreadProcData->m_hThreadDelDir);
		//pData->pTreadProcData->m_hThreadDelDir = NULL;
		return FALSE;
	}

	while(bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDots())
		{
			continue;
		}

		if(finder.IsDirectory())
		{
			strFileName = finder.GetFileName();
			pData->m_strDelPath = strPath;
			pData->m_strDelPath.Format(_T("%s\\%s"),pData->m_strDelPath,strFileName);

			pData->pTreadProcData->m_fnDeleteAllFolder((LPVOID)pData);
		}
		else
		{
			//CloseHandle(pData->pTreadProcData->m_hThreadDelDir);
			//pData->pTreadProcData->m_hThreadDelDir = NULL;
			return FALSE;
		}
	}
	if( !finder.FindNextFile() && strPath != strOriPath)
	{
		CTime ctCurrentTime = CTime::GetCurrentTime();
		finder.GetCreationTime(ctCreated);
		CTimeSpan elapsedTime = ctCreated - ctCurrentTime;
		tsInterval = ctCurrentTime - ctCreated;
		double seconds = tsInterval.GetTotalSeconds();

		if(seconds > 60)
			RemoveDirectory(strPath);
	}

	if (strPath == strOriPath)
	{
		CloseHandle(pData->pTreadProcData->m_hThreadDelDir);
		pData->pTreadProcData->m_hThreadDelDir = NULL;

	}
	

	Sleep(1000);
	//G_AddLog(3, 3, _T("m_fnDeleteAllFolder() - End"));
}
