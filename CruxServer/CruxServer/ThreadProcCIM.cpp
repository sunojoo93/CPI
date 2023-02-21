// CThreadProcCIM.cpp : 구현 파일입니다.
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "stdafx.h"
#include "CruxServer.h"
#include "ThreadProcCIM.h"
#include "ViewSetup.h"
#include "MainFrm.h"
// CThreadProcCIM

IMPLEMENT_DYNCREATE(CThreadProcCIM, CWinThread)

CThreadProcCIM::CThreadProcCIM()
{
	m_hThreadCim = NULL;
	//m_hThreadCim = CreateEvent(NULL,TRUE, FALSE,NULL);
	::InitializeCriticalSection(&m_csThreadCim);
}

CThreadProcCIM::~CThreadProcCIM()
{
	m_hThreadCim =NULL;
	DeleteCriticalSection(&m_csThreadCim);
}

BOOL CThreadProcCIM::InitInstance()
{
	//return CThreadProcCIM::InitInstance();

	return TRUE;
}

int CThreadProcCIM::ExitInstance()
{
	// TODO: 여기에서 각 스레드에 대한 정리를 수행합니다.
	return CWinThread::ExitInstance();
}


unsigned __stdcall   CThreadProcCIM::m_fnResultJudgeProc(LPVOID pArg)
{
	//2016.01.15
	CThreadProcPLC *pCim = (CThreadProcPLC *)pArg;

	//EnterCriticalSection(& pCim->m_pTreadCimProc->m_csThreadCim);		//2018.12.17 comment by mby
	G_AddLog(3, 0, _T("m_fnResultJudgeProc() - Start"));

	long lRet = -1;
	int nPubNo = 0, nPubNoChk = 0, /* nVisionCnt = 0,*/ nChkList = 0;
	int curPos = 0,	nLoopCnt =0, nImgNameCnt = 0, nRetryCnt = 0;
	CString  resToken	= _T("");
	CString  strTemp	= _T(""), strTempComp = _T("");
	//2016.01.15
	int nMeasurePubNo = 0;
	int nCimMeasureData[MAX_CIM_MEASURE_DATA];	
	double nCimMeasureData_ABI[MAX_CIM_MEASURE_DATA];
	double nCimMeasureData_AVL[MAX_CIM_MEASURE_DATA];
	CString strCimMeasureTemp;
	int nStartIdx = 0;
	int	nCheckSize = 0;
	
	curPos = 0;	nLoopCnt =0; nImgNameCnt = 0;
	//CString strPanelID1, strPanelID2, strDefGrade1, strDefGrade2, strdefJudge1, strdefJudge2;

	CMainFrame* pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();	
	CViewSetup* pView = (CViewSetup*)pFrame->m_fnGetViewPtr(eFORM_Setup);
	
	CString strConfigFile = _T("");
	strConfigFile.Format(_T("%s%s"),ROOT_PATH, CONFIG_PATH);
	int nEqpModel = GetPrivateProfileInt(SECTION_PARAM, KEY_PARAM_EQP_TYPE, 0, strConfigFile);	//0:AVL 1:ABI

	CString strPanel[MAX_PANEL] ; 
	CString strDefGrade[MAX_PANEL] ; 
	CString strCimMeasureData[MAX_PANEL] ;
	CString strdefJudge[MAX_PANEL][MAX_JUDGE] ; 
	
	strPanel[PANEL_ONE]					= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE].cPanelID;		//pFrame->m_DataManager.m_fnGetItemData(0, strTemp);		
	strDefGrade[PANEL_ONE]				= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE].cGrade;			//pFrame->m_DataManager.m_fnGetItemData(1, strTemp);	
	strCimMeasureData[PANEL_ONE]		= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE].cCimMeasureData;
	strdefJudge[PANEL_ONE][JUDGE_ONE]	= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE].cJudge1;		//pFrame->m_DataManager.m_fnGetItemData(2, strTemp);
	strdefJudge[PANEL_ONE][JUDGE_TWO]	= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_ONE].cJudge2;		//pFrame->m_DataManager.m_fnGetItemData(3, strTemp);

	strPanel[PANEL_TWO]					= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO].cPanelID;		//pFrame->m_DataManager.m_fnGetItemData(4, strTemp);
	strDefGrade[PANEL_TWO]				= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO].cGrade;			//pFrame->m_DataManager.m_fnGetItemData(5, strTemp);	
	strCimMeasureData[PANEL_TWO]		= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO].cCimMeasureData;
	strdefJudge[PANEL_TWO][JUDGE_ONE]	= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO].cJudge1;		//pFrame->m_DataManager.m_fnGetItemData(6, strTemp);
	strdefJudge[PANEL_TWO][JUDGE_TWO]	= pCim->m_pTreadCimProc->m_stNgFlagTheadData[PANEL_TWO].cJudge2;		//pFrame->m_DataManager.m_fnGetItemData(7, strTemp);

	int nEqpSizeType = pFrame->m_fnEqpSizeType();
	
	int nCnt = 0;
	int nPanelCnt = 0;
	int nchk	=   0;		//test
	if (nEqpSizeType != EQP_LARGE_SIZE && nEqpModel == EQP_TYPE_AVL && nEqpSizeType != EQP_MONITOR_SIZE)
	{
		//lRet = pView->m_pDlgPLC->m_fnGetDevice(_T("L714")) ;
		while(pView->m_pDlgPLC->m_fnGetDevice(_T("L714"))  != 0)
		{
			Sleep(100);
		}
		lRet = pView->m_pDlgPLC->m_fnSetDevice(_T("L514"), 1);
		nPanelCnt = MAX_PANEL;
	}
	else
		nPanelCnt = 1;

	while(nCnt < MAX_PANEL)
	{
		if (nCnt == MAX_PANEL)
		{
			break;
		}
		if (strPanel[nCnt].IsEmpty()){ nCnt++;	continue;}
		
		if(nEqpSizeType == EQP_LARGE_SIZE)	// 대형  AVL
		{
			for(nchk=0; nchk < 3; nchk++){
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CMD_NO, 175);
				if (lRet == APP_OK) break;
				G_AddLog(3, 0, _T("ADDR_PROC_DATA_EQ_CMD_NO = 175"));
				Sleep(300);
			}
			strPanel[nCnt].TrimRight();
			for(nchk=0; nchk < 3; nchk++){
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]); //Panel ID 추가작업하여 진행할 것.
				if (lRet == APP_OK) break;
				G_AddLog(3, 0, _T("ADDR_PROC_DATA_EQ_PNL_ID_0 = 175"));
				Sleep(300);
			}
			if (strDefGrade[nCnt] == _T(""))
			{
				strDefGrade[nCnt] = JUDGE_A;
			}
			for(nchk=0; nchk < 3; nchk++){
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);
				if (lRet == APP_OK) break;
				G_AddLog(3, 0, _T("ADDR_PROC_DATA_EQ_INSPMODE "));
				Sleep(300);
			}			
			//2016.02.22 Mr.Chu
			if(nEqpModel == EQP_TYPE_AVL)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 1, strDefGrade[nCnt]);	//2018.06.14 2 -> 1

				//2018.05.19 change by mby ,wangjang
				//lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2_AVL, 4, JUDGE_A);
				//2019.01.22 by ryu
				//if(strDefGrade[nCnt] == JUDGE_B)
				//{
					if(pFrame->m_DataManager.m_str_rankCIMvalue != _T(""))
					{
						//strDefGrade[nCnt] = pFrame->m_DataManager.m_str_rankCIMvalue;
						lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2_AVL, 2, pFrame->m_DataManager.m_str_rankCIMvalue);
						pFrame->m_DataManager.m_str_rankCIMvalue = _T("");
					}
					else
					{
						//strDefGrade[nCnt] = JUDGE_B;  //재확인 필요.
						//2021.02.04 add by mby
						if(strDefGrade[nCnt] == JUDGE_A){
							lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2_AVL, 2, JUDGE_AN); 
						}else{
							lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2_AVL, 2, strDefGrade[nCnt]);
						}
						
					}	
					
				//}
				//else 
				//{
				//	lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2_AVL, 2, strDefGrade[nCnt]);					
				//}

					//2018.06.14 4 -> 2

				if (!strDefGrade[nCnt].Compare(JUDGE_A))
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE1, 2, _T("OK"));
				else		
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE1, 2, _T("NG"));

				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE2, 4, strdefJudge[nCnt][JUDGE_ONE]);
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE3_AVL, 4, strdefJudge[nCnt][JUDGE_TWO]);

				//lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE, 1);	// Glass Judge Bit(B82B) On  ==>  2015.10.01 현재 L530(중,소형 NG flag complete) 으로 Test 중...  
				nChkList = pFrame->m_DataManager.m_fnSetCimCheckList(strdefJudge[nCnt][JUDGE_ONE]);
				{
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_AVL_CHKLIST1, nChkList);
				}
			}
			else if(nEqpModel == EQP_TYPE_ABI)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_GRADE1_ABI, 2, strDefGrade[nCnt]);

				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, JUDGE_A);

				if (!strDefGrade[nCnt].Compare(JUDGE_A))
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE1_ABI, 2, _T("OK"));
				else		
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE1_ABI, 2, _T("NG"));

				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_BIG_JUDGE2_ABI, 4, strdefJudge[nCnt][JUDGE_ONE]);

				//lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE, 1);	// Glass Judge Bit(B82B) On  ==>  2015.10.01 현재 L530(중,소형 NG flag complete) 으로 Test 중...  
				nChkList = pFrame->m_DataManager.m_fnSetCimCheckList(strdefJudge[nCnt][JUDGE_ONE]);
				{
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_ABI_CHKLIST1, nChkList);
				}
			}
			//
		
			/*for(int i=0;i<300;i++)	//CIM 측 Glass Judge 수신 Bit 가 일정 시간(3초)동안 On 되지 않으면 Timeout 처리 필요.
			{
				Sleep(10);
				lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE_CHECK, nChkList);
				if(lRet != NULL)
					break;
			}
			if( lRet == NULL )
				pView->m_pDlgPLC->m_fnSetDevice(ADDR_OVERLAP_COOR_OCCUR, 1);
			else
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE, 0);	// Glass Judge Bit(B82B) On*/


			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_PUB_NO);	// CIM 의 Receive No 을 확인
			if (lRet == 0)	lRet = 1;
			nPubNo = lRet + 1;

			//2016.01.15
			if (nCnt == 0) nStartIdx = 8;
			else if (nCnt == 1) nStartIdx = 24;
			//2018.09.08 add by mby S-
			if(int(pFrame->m_DataManager.m_stDefCoordDatRcp.dSetPanelSizeHeight) == 631 ){	//수치확인필요(UI설정값) 이후 50,58인치외 생산시 추가필요
				nCheckSize = 0; //50인치
			}
			else{
				nCheckSize = 1; //58인치
			}
			//2018.09.08 add by mby E-
			for (int i = 0; i < MAX_CIM_MEASURE_DATA; i++)
			{
				strCimMeasureTemp = pFrame->m_DataManager.m_fnGetItemData(i, strCimMeasureData[nCnt]);
				if(nEqpModel == EQP_TYPE_AVL)
				{
					nCimMeasureData_AVL[i] = _tstoi((LPCTSTR)strCimMeasureTemp);
					nCimMeasureData[i] = pFrame->m_DataManager.MeasureValueLimitChk(nEqpModel, nCheckSize, i, int(nCimMeasureData_AVL[i]));//_tstoi((LPCTSTR)strCimMeasureTemp);		//2018.09.12  by hikim
				//	nCimMeasureData[i] = _tstoi((LPCTSTR)strCimMeasureTemp);  //2018.12.11 BY RYU 왕강요청.
				}else{
					nCimMeasureData_ABI[i] = _tstof((LPCTSTR)strCimMeasureTemp);
					nCimMeasureData[i] = pFrame->m_DataManager.MeasureValueLimitChk(nEqpModel, nCheckSize,i,int(nCimMeasureData_ABI[i] *100)); //_tstoi((LPCTSTR)strCimMeasureTemp);	//2018.09.08 add by mby
				}				
			}

			for(nchk=0; nchk < 3; nchk++){
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_CMD_NO, 170);
				if (lRet == APP_OK) break;
				G_AddLog(3, 0, _T("ADDR_MEASURE_DATA_EQ_CMD_NO = 170"));
				Sleep(300);
			}
			for(nchk=0; nchk < 3; nchk++){
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_MEASURE_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]);
				if (lRet == APP_OK) break;
				G_AddLog(3, 0, _T("ADDR_MEASURE_DATA_EQ_PNL_ID_0 = 170"));
				Sleep(300);
			}
			if (nEqpModel == EQP_TYPE_AVL)
			{
				//2018.03.12 LGU nCimMeasureData[8]....[15] <-> nCimMeasureData[0]...[7] //2018.04.10 WANGKANG 8...15 -<-> 0...7
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_0, nCimMeasureData[0]  );//+ MEASURE_CIM_DATA_EQ_OFFSET );			
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_1, nCimMeasureData[1]  );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_2, nCimMeasureData[2] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_3, nCimMeasureData[3] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_4, nCimMeasureData[4] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_5, nCimMeasureData[5] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_6, nCimMeasureData[6] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_7, nCimMeasureData[7] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_8, nCimMeasureData[8]  );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_9, nCimMeasureData[9]  );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_10, nCimMeasureData[10] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_11, nCimMeasureData[11] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_12, nCimMeasureData[12] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_13, nCimMeasureData[13] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_14, nCimMeasureData[14] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_15, nCimMeasureData[15] );//+ MEASURE_CIM_DATA_EQ_OFFSET );
			}
			else
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_0, nCimMeasureData[0] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_1, nCimMeasureData[1] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_2, nCimMeasureData[2] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_3, nCimMeasureData[3] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_4, nCimMeasureData[4] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_5, nCimMeasureData[5] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_6, nCimMeasureData[6] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_7, nCimMeasureData[7] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_8, nCimMeasureData[8] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_9, nCimMeasureData[9] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_10, nCimMeasureData[10] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_11, nCimMeasureData[11] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_12, nCimMeasureData[12] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_13, nCimMeasureData[13] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_14, nCimMeasureData[14] + MEASURE_CIM_DATA_EQ_OFFSET);
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_15, nCimMeasureData[15] + MEASURE_CIM_DATA_EQ_OFFSET);
			}
			
			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO);
			if (lRet == 0)	lRet = 1;
			nMeasurePubNo = lRet + 1;
			//2016.01.15

			//2018.11.28 by ryu
			G_AddLog(3, 0, _T("m_fnResultJudgeProc() - middle"));

			//2016.11.07
 			long lInspMode = 0;
 			lInspMode = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_INSPMODE);
 			if(lInspMode != 1)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);
 			}
 			Sleep(50);
 			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_PUB_NO, nPubNo);	
 			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO, nMeasurePubNo);

			Sleep(150);	//2018.03.07 1000 -> 500 ->200->150
 			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set
 			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO, nMeasurePubNo);	//2016.01.15

		}
		else if(nEqpSizeType == EQP_MONITOR_SIZE)
		{

			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CMD_NO, 175);
			strPanel[nCnt].TrimRight(); 
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]); //Panel ID 추가작업하여 진행할 것.

			if (strDefGrade[nCnt] == _T(""))
			{
				strDefGrade[nCnt] = JUDGE_A;
			}

			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);


			//2016.02.22 Mr.Chu			ABI & AVL 같은 주소값 공유 2016.11.17 확인
			//if(nEqpModel == EQP_TYPE_AVL)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 2, strDefGrade[nCnt]);

				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, JUDGE_A);

				if (!strDefGrade[nCnt].Compare(JUDGE_A))
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, _T("OK"));
				else		
					lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, _T("NG"));

				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE2, 4, strdefJudge[nCnt][JUDGE_ONE]);
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE3, 4, strdefJudge[nCnt][JUDGE_TWO]);

				//lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_BIG_GLASS_JUDGE, 1);	// Glass Judge Bit(B82B) On  ==>  2015.10.01 현재 L530(중,소형 NG flag complete) 으로 Test 중...  
				nChkList = pFrame->m_DataManager.m_fnSetCimCheckList(strdefJudge[nCnt][JUDGE_ONE]);
				{
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_MONITOR_EQ_AVL_CHKLIST1, nChkList);
				}
			}

			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_PUB_NO);	// CIM 의 Receive No 을 확인
			if (lRet == 0)	lRet = 1;
			nPubNo = lRet + 1;

			//2016.01.15
			if (nCnt == 0) nStartIdx = 8;
			else if (nCnt == 1) nStartIdx = 24;

			for (int i = 0; i < MAX_CIM_MEASURE_DATA; i++)
			{
				strCimMeasureTemp = pFrame->m_DataManager.m_fnGetItemData(i, strCimMeasureData[nCnt]);
				nCimMeasureData[i] = _tstoi((LPCTSTR)strCimMeasureTemp);
			}

			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_CMD_NO, 170);
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_MEASURE_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]);

			//ABI. AVL 공통(Vessel)
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_0, nCimMeasureData[0] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_1, nCimMeasureData[1] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_2, nCimMeasureData[2] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_3, nCimMeasureData[3] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_4, nCimMeasureData[4] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_5, nCimMeasureData[5] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_6, nCimMeasureData[6] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_7, nCimMeasureData[7] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_8, nCimMeasureData[8] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_9, nCimMeasureData[9] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_10, nCimMeasureData[10] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_11, nCimMeasureData[11] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_12, nCimMeasureData[12] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_13, nCimMeasureData[13] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_14, nCimMeasureData[14] + MEASURE_CIM_DATA_EQ_OFFSET);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_15, nCimMeasureData[15] + MEASURE_CIM_DATA_EQ_OFFSET);

			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO);
			if (lRet == 0)	lRet = 1;
			nMeasurePubNo = lRet + 1;
			//2016.01.15

			//2016.11.07
			long lInspMode = 0;
			lInspMode = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_INSPMODE);
			if(lInspMode != 1)
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);
			}

			Sleep(1000);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO, nMeasurePubNo);	//2016.01.15

			CString strPLCGrade1,strPLCGrade2,strPLCJudge1,strPLCJudge2,strPLCJudge3,strPanelID;

			lRet = pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 40, strPanelID);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 2, strPLCGrade1);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, strPLCGrade2);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, strPLCJudge1);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE2, 4, strPLCJudge2);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE3, 4, strPLCJudge3);

			G_AddLog(3, 3, _T("Panel Id : [%s] INSPMODE : %d  GRADE : %s  JUDGE1 : %s  JUDGE2 : %s PUB NO: %d Process Data GET"), 
				strPanelID ,lInspMode, strPLCGrade1,  strPLCJudge1, strPLCJudge2, nPubNo - 1 );
		}
		else if ( nEqpSizeType == EQP_SMALL_SIZE || nEqpSizeType ==  EQP_MEDIUM_SIZE )	// 중,소형  AVL
		{
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CMD_NO, 175);

			/*	if (strPanel[nCnt]  == _T(""))
			{
			strPanel[nCnt].Format(_T("PNL000000000000%03d"), 1);
			}*/

			//strPanel[nCnt] = m_fnGetPanelId();

			//2015.12.28 PLC LOG
			long lInspMode = 0;
			CString strPLCGrade1 = _T("");
			CString strPLCGrade2 = _T("");
			CString strPLCJudge1 = _T("");
			CString strPLCJudge2 = _T("");
			CString strPLCJudge3 = _T("");
			//2015.12.28 PLC LOG

			strPanel[nCnt].TrimRight();
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]);
		
			if (strDefGrade[nCnt] == _T(""))
			{
				strDefGrade[nCnt] = JUDGE_A;
			}
			if (strDefGrade[nCnt] != JUDGE_A )
			{
				if (nEqpModel == EQP_TYPE_AVL) strDefGrade[nCnt] = JUDGE_B;
				if (nEqpModel == EQP_TYPE_ABI) strDefGrade[nCnt] = JUDGE_D;
			}
		
			// PanelID #1 의 검사 결과값 Write
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_INSPMODE, 1);
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 1, strDefGrade[nCnt]);
			//2017.06.12  ABI 중 소형 설비 Grade A가 아닐경우 FD보고
			if( strDefGrade[nCnt] != JUDGE_A && nEqpModel == EQP_TYPE_ABI )
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, JUDGE_FD_1);
			else
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, JUDGE_A);

			// 			if (!strDefGrade[nCnt].Compare(JUDGE_A))
			// 				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 1, _T("OK"));
			// 			else		
			// 				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 1, _T("NG"));

			//2017.08.22	ABI305 Cim측 Judge1 OK,NG 간헐적 보고누락 개선을 위한 3번 Retry
			if (!strDefGrade[nCnt].Compare(JUDGE_A))	{ strTemp.Format(_T("OK")); }
			else										{ strTemp.Format(_T("NG")); }

			for( nRetryCnt = 0; nRetryCnt < RETRY_COUNT; nRetryCnt++ )
			{
				lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 1, strTemp);

				Sleep(10);
				pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, strTempComp);
				//G_AddLog( 3, 2, _T("Data : %s"), strTempComp);
				if( strTemp == strTempComp )
					break;
				else
					G_AddLog( 3, 2, _T("[PanelID : %s][Try : %d] DATA_EQ_JUDGE1 Not Changed  Process Data Report Error"), strPanel[nCnt], nRetryCnt);
			}
			//


			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE2, 2, strdefJudge[nCnt][JUDGE_ONE]);
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE3, 2, strdefJudge[nCnt][JUDGE_TWO]);

			nChkList = pFrame->m_DataManager.m_fnSetCimCheckList(strdefJudge[nCnt][JUDGE_ONE]);
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CHKLIST1, nChkList);
			}

		/*	lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CHKLIST2, 0);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CHKLIST3, 0);
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_CHKLIST4, 0);*/
	
			Sleep(200);	//ABI305 Cim측 Data 갱신시간으로 인한 보고 꼬임 방지 //500->200

			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_PUB_NO);	// CIM 의 Receive No 을 확인
			if (lRet == 0)	lRet = 1;
			nPubNoChk = lRet;
			nPubNo = lRet + 1;

			//2017.08.12	Publish No 바뀌지 않는 증상으로 인한 3번 Retry
			for( nRetryCnt = 0; nRetryCnt < RETRY_COUNT; nRetryCnt++ )
			{
				lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_PROC_DATA_EQ_PUB_NO, nPubNo);	// Recieve No 에 +1 하여 Publish No Set

				Sleep(10);
				if( nPubNoChk != pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_PUB_NO) )
					break;
				else
					G_AddLog( 3, 2, _T("[PanelID : %s][Try : %d] PUB NO Not Changed  Process Data Report Error"), strPanel[nCnt], nRetryCnt);
			}

			//2016.01.15
			if (nCnt == 0) nStartIdx = 8;
			else if (nCnt == 1) nStartIdx = 24;

			for (int i = 0; i < MAX_CIM_MEASURE_DATA; i++)
			{
				strCimMeasureTemp = pFrame->m_DataManager.m_fnGetItemData(i, strCimMeasureData[nCnt]);
				nCimMeasureData[i] = _tstoi((LPCTSTR)strCimMeasureTemp);
			}

			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_CMD_NO, 170);
			lRet = pView->m_pDlgPLC->m_fnSetDeviceBlock(ADDR_MEASURE_DATA_EQ_PNL_ID_0, 10, strPanel[nCnt]);

			if (pFrame->m_fnEqpSizeType() != EQP_LARGE_SIZE)
			{
				if (nEqpModel == EQP_TYPE_AVL)
				{
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_0, nCimMeasureData[0]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_1, nCimMeasureData[1]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_2, nCimMeasureData[2]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_3, nCimMeasureData[3]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_4, nCimMeasureData[4]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_5, nCimMeasureData[5]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_6, nCimMeasureData[6]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_7, nCimMeasureData[7]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_8, nCimMeasureData[8]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_9, nCimMeasureData[9]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_10, nCimMeasureData[10]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_11, nCimMeasureData[11]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_12, nCimMeasureData[12]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_13, nCimMeasureData[13]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_14, nCimMeasureData[14]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_AVL_MEASURE_EQ_DATA_15, nCimMeasureData[15]);
				}
				else
				{
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_0, nCimMeasureData[0]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_1, nCimMeasureData[1]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_2, nCimMeasureData[2]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_3, nCimMeasureData[3]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_4, nCimMeasureData[4]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_5, nCimMeasureData[5]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_6, nCimMeasureData[8]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_7, nCimMeasureData[9]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_8, nCimMeasureData[10]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_9, nCimMeasureData[11]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_10, nCimMeasureData[12]);
					lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_ABI_MEASURE_EQ_DATA_11, nCimMeasureData[13]);
				}
			}
			
			lRet = pView->m_pDlgPLC->m_fnGetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO);
			if (lRet == 0)	lRet = 1;
			nMeasurePubNo = lRet + 1;
			//2016.01.15
			Sleep(200);	//500->200
			lRet = pView->m_pDlgPLC->m_fnSetDevice(ADDR_MEASURE_DATA_EQ_PUB_NO, nMeasurePubNo);	//2016.01.15

			//2015.12.28 PLC LOG
			lInspMode = pView->m_pDlgPLC->m_fnGetDevice(ADDR_PROC_DATA_EQ_INSPMODE);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE1, 2, strPLCGrade1);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_GRADE2, 4, strPLCGrade2);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE1, 2, strPLCJudge1);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE2, 4, strPLCJudge2);
			pView->m_pDlgPLC->m_fnGetDeviceBlock(ADDR_PROC_DATA_EQ_JUDGE3, 4, strPLCJudge3);
			G_AddLog(3, 3, _T("PLC WRITE Panel Id : [%s]  INSPMODE : %d  GRADE1 : %s  GRADE2 : %s JUDGE1: %s JUDGE2: %s JUDGE3: %s "), 
				strPanel[nCnt] ,lInspMode,  strPLCGrade1, strPLCGrade2, strPLCJudge1, strPLCJudge2, strPLCJudge3);
	
		}


		//2018.01.20 Logpriority 2->0
		//G_AddLog(3, 2, _T("EqpSizeType : %d, %s"),nEqpSizeType, strPanel[nCnt]);
		G_AddLog(3, 0, _T("Panel Id : [%s]  GRADE : %s  JUDGE1 : %s  JUDGE2 : %s PUB NO: %d Process Data Send"), 
			strPanel[nCnt] ,strDefGrade[nCnt],  strdefJudge[nCnt][JUDGE_ONE], strdefJudge[nCnt][JUDGE_TWO], nPubNo - 1 );

		G_AddLog(3, 0, _T("Panel Id : [%s]  %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, PUB NO: %d Measure Data Send"), 
			strPanel[nCnt] ,nCimMeasureData[0], nCimMeasureData[1],nCimMeasureData[2], nCimMeasureData[3], nCimMeasureData[4],
			nCimMeasureData[5], nCimMeasureData[6], nCimMeasureData[7], nCimMeasureData[8], nCimMeasureData[9], nCimMeasureData[10],
			nCimMeasureData[11], nCimMeasureData[12], nCimMeasureData[13], nCimMeasureData[14], nCimMeasureData[15], nMeasurePubNo);

		nCnt++;
	}
	if (nEqpSizeType != EQP_LARGE_SIZE && nEqpModel == EQP_TYPE_AVL && nEqpSizeType != EQP_MONITOR_SIZE)
		lRet = pView->m_pDlgPLC->m_fnSetDevice(_T("L514"), 0);

	CloseHandle(pCim->m_pTreadCimProc->m_hThreadCim);
	pCim->m_pTreadCimProc->m_hThreadCim = NULL;
	G_AddLog(3, 0, _T("m_fnResultJudgeProc() - end"));
	//LeaveCriticalSection(& pCim->m_pTreadCimProc->m_csThreadCim);	//2018.12.17 comment by mby

	return 0;
}
