#include "stdafx.h"
#include "WorkManager.h" 
#include "..\..\VisualStation\CommonHeader\Class\LogWriter.h"
#include "VSAlgorithmTask.h"
#include <algorithm>
#include "DefineInterface.h"
#include "InspThrd.h"
//#include "AviDefineInspect.h"
#include "DllInterface.h"

// Image Save 관련 헤더
//#include "ImageSave.h"

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* strPath, ...);
void ImageAsyncSaveJPG(cv::Mat& MatSrcBuffer, const char* strPath);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CLogWriter* GlassDataWriter;


#define	SEQUENCE_TABLE(FUNCTION_NO,SEQUENCE_NO,FUNCTION_NAME,ALWAYS_RUNMODE,SEQ_REST_POSSIBLE,CS_LOCK_NAME)					\
	if ( pCmdMsg->uFunID_Dest == FUNCTION_NO && pCmdMsg->uSeqID_Dest == SEQUENCE_NO )										\
	{																														\
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			EnterCriticalSection(CS_LOCK_NAME);																				\
		}																													\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount++;																							\
			isFunctionRuned = true;																							\
			nRet = FUNCTION_NAME((BYTE*)pCmdMsg->cMsgBuf, pCmdMsg->uMsgSize, ALWAYS_RUNMODE, false, SEQ_REST_POSSIBLE);		\
			if (SEQ_REST_POSSIBLE)																							\
				m_SeqenceCount--;																							\
		if ((VOID*)CS_LOCK_NAME != NULL)																					\
		{																													\
			LeaveCriticalSection(CS_LOCK_NAME);																				\
		}																													\
	}



WorkManager::WorkManager()
{
	int nCameraNo = 0;
	TCHAR strVal[2];
	CString strSection, strKey;	

// 	for (int nGrabberCnt=0; nGrabberCnt < MAX_FRAME_GRABBER_COUNT; nGrabberCnt++)	//Frame Grabber Max : 4
// 	{
// 		strKey.Format(_T("Frame Grabber_%d"), nGrabberCnt);
// 		GetPrivateProfileString(_T("Grabber_Board"), strKey ,_T("F"), strVal, 2, DEVICE_FILE_PATH);
// 		if (!_tcscmp(strVal, _T("T")))
// 		{
			for (int nCameraCnt=0; nCameraCnt < MAX_CAMERA_COUNT; nCameraCnt++) //Frame Grabber당 Camera Max : 4
			{
				m_pSharedMemory[nCameraCnt] = NULL;
				strSection.Format(_T("Frame Grabber_%d"), 0);
				strKey.Format(_T("Insp Camera_%d"), nCameraCnt);
				GetPrivateProfileString(strSection, strKey ,_T("F"), strVal, 2, DEVICE_FILE_PATH);
				if (!_tcscmp(strVal, _T("T")))
				{					
					m_pSharedMemory[nCameraNo] = new CSharedMemWrapper();				// 메모리 할당
					CString strDrv_CamNo = _T("");
					strDrv_CamNo.Format(_T("%s_%d"), theApp.m_Config.GETDRV(), nCameraNo + 1);
					m_pSharedMemory[nCameraNo]->OpenSharedMem(strDrv_CamNo);			// CAMERA_NUM번 카메라 연결
					nCameraNo++;
				}
			}
// 		}
// 	}
	m_hSequenceResetEvent	= CreateEvent( NULL, TRUE,  FALSE,  NULL );
	m_bSeqResetFlag = 0;
	m_SeqenceCount = 0;

	InitializeCriticalSection(&m_csSequenceLock_1);
	InitializeCriticalSection(&m_csSequenceLock_2);
	InitializeCriticalSection(&m_csSequenceLock_3);
	InitializeCriticalSection(&m_csSequenceLock_4);
	InitializeCriticalSection(&m_csSequenceLock_5);
}


WorkManager::~WorkManager()
{
	for (int nCameraCnt=0; nCameraCnt < MAX_CAMERA_COUNT; nCameraCnt++) //Frame Grabber당 Camera Max : 4
		SAFE_DELETE(m_pSharedMemory[nCameraCnt]);
	DeleteCriticalSection(&m_csSequenceLock_1);
	DeleteCriticalSection(&m_csSequenceLock_2);
	DeleteCriticalSection(&m_csSequenceLock_3);
	DeleteCriticalSection(&m_csSequenceLock_4);
	DeleteCriticalSection(&m_csSequenceLock_5);
}

int WorkManager::Start()
{
	HANDLE handle;

// Message 수신부 단일 Thread
 	for (int i = 0; i < 30; i++)
 	{
	handle = m_fnStartThread();
	if ( handle == NULL || handle == (HANDLE)-1 )
		return APP_NG;
 	}
	
	return APP_OK;
}

void WorkManager::m_fnRunThread()
{
	int nRet;
	BYTE *pMsg = NULL;
	CMDMSG *pCmdMsg = NULL;

	//Start Status Refresh Thread. 처음 실행되는 1개 쓰레드가 처리를 전담한다.

	while (GetThreadRunFlag())
	{
		EXCEPTION_TRY

		pMsg = m_fnPeekMessage();
		pCmdMsg = (CMDMSG*)pMsg;

				
		nRet = AnalyzeMsg(pCmdMsg);
		if ( nRet != APP_OK ) 
			throw nRet;

		EndWorkProcess(pMsg);

		pCmdMsg = NULL;
		pMsg = NULL;

		EXCEPTION_CATCH
			
		if ( nRet != APP_OK )
		{
			if ( pMsg != NULL )
			{
				EndWorkProcess(pMsg);
				pCmdMsg = NULL;
				pMsg = NULL;
			}
			theApp.WriteLog(eLOGCOMM, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("ERROR WorkManager::AnalyzeMsg. Error Code = %d \n"), nRet);
		}
	}
}

int WorkManager::AnalyzeMsg(CMDMSG* pCmdMsg)
{
	int			nRet = APP_OK;
	bool		isFunctionRuned = false;	

	EXCEPTION_TRY

//	SEQUENCE_TABLE (	FUNNO,	SEQNO,	FUNCTION_NAME,					중복실행가능여부,	시퀀스 리셋 가능 여부	)
	// for test	
	SEQUENCE_TABLE (	10,		99	,	Seq_TEST						, false	,			true					,	&m_csSequenceLock_1	)
	// Alive
	SEQUENCE_TABLE (	10,		1	,	VS_TaskAlive					, false	,			true					,	&m_csSequenceLock_1	)
	// Inspect
	SEQUENCE_TABLE (	21,		1	,	Seq_StartInspection				, false	,			true					,	&m_csSequenceLock_2	)	// 검사 시작
	SEQUENCE_TABLE (	50,		10	,	Seq_ManualAlign					, false	,			true					,	&m_csSequenceLock_2	)	// Manual Align 실행
	SEQUENCE_TABLE (	50,		20	,	Seq_SetParam					, false	,			true					,	&m_csSequenceLock_2	)	// Parameter 설정
	SEQUENCE_TABLE (	50,		30	,	Seq_FocusValue					, false	,			true					,	&m_csSequenceLock_2	)	// Get Focus Value
	SEQUENCE_TABLE (	50,		40	,	Seq_WriteCCDIndex				, false	,			true					,	&m_csSequenceLock_2	)	// Write CCD Defect Index

	SEQUENCE_TABLE (	21,		10	,	Seq_GetAlignPatternNum			, false	,			true					,	&m_csSequenceLock_3	)	// Align Pattern Num 반환
	SEQUENCE_TABLE (	21,		50	,	Seq_StartAlign					, false	,			true					,	&m_csSequenceLock_3	)	// Align Camera

	SEQUENCE_TABLE (	21,		70	,	Seq_StartAutoCal_Mask			, false	,			true					,	&m_csSequenceLock_2)
	SEQUENCE_TABLE (	21,		71	,	Seq_StartAutoCal				, false	,			true					,	&m_csSequenceLock_2)

	if( m_SeqenceCount <= 0 )
	{
		m_bSeqResetFlag = 0;
		m_SeqenceCount = 0;
		ResetEvent(m_hSequenceResetEvent);
	}

	if ( isFunctionRuned  == false ) 
	{
		theApp.WriteLog(eLOGCOMM, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Function Table is nothing. FuncNo : %d, SeqNo : %d ,From %d Task "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest, pCmdMsg->uTask_Src);
		throw SEQUENCE_TASK_SEQUENCE_IS_NOTHING;
	}		

	EXCEPTION_CATCH

	if(pCmdMsg->uMsgType == CMD_TYPE_RES && pCmdMsg->uMsgOrigin == CMD_TYPE_CMD)
	{
		nRet = ResponseSend((USHORT)nRet, pCmdMsg);
		if ( nRet != APP_OK ) 
		{
			theApp.WriteLog(eLOGCOMM, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Response Send  Fail. FuncNo: %d, SeqNo : %d "), pCmdMsg->uFunID_Dest, pCmdMsg->uSeqID_Dest);
			return nRet;
		}
	}

	return nRet;
}

int	WorkManager::Seq_TEST(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible)
{
	int nRet = APP_OK;
	bool isRunSequence = true;
	int nStepNo = 0;
	static bool isSeqBusy = false;

	//USHORT	usSet;
	//INT		nVal;
	//byte*	tempParam = pParam;

	do
	{
		EXCEPTION_TRY

		if (nStepNo == 0 && isSeqBusy && bAlwaysRunMode == false )	// 시퀀스가 Busy 인경우 에러 리턴되게 하려면 bAlwaysRunMode가 false 이어야 한다.
			return SEQUENCE_TASK_SEQUENCE_IS_BUSY;
		else if (nStepNo == 0 && bBusyCheck == true && isSeqBusy == false)
		{
			return SEQUENCE_TASK_SEQUENCE_IS_NOT_BUSY;
		}
		
		isSeqBusy = true;

		if( m_bSeqResetFlag && bSeqResetPossible )
			throw 9999;

		// Sequence In LOG
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq1099_TEST StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

		nStepNo++;
		switch (nStepNo)
		{
		case 1:

			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("CASE 1"));
			break;

		case 2:

			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("CASE 2"));

			break;

		case 3:
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("CASE 3"));

			break;

		default :
			isRunSequence = false;
			break;
		}

		EXCEPTION_CATCH

		if ( nRet != APP_OK)
		{
			// Error Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq1099_TEST Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);
			isRunSequence = false;
			int nRetExcept =APP_OK;

			// EQP BIT ALL OFF

		}

	}while ( isRunSequence );

	// Sequence Out LOG
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq1099_TEST Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	isSeqBusy = false;

	return nRet;
}

int WorkManager::VS_TaskAlive( byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode /*= false*/, bool bBusyCheck /*= false*/, bool bSeqResetPossible /*= true*/ )
{
	int nRet = APP_OK;
	int nStepNo = 0;

	byte* tempParam	= pParam;

	EXCEPTION_TRY
		// Do nothing
	EXCEPTION_CATCH

	if ( nRet != APP_OK)
	{
		// Error Log
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq1001_Task_Alive Error Occured. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);			
		return nRet;
	}

	// Sequence Out LOG
	//theApp.WriteLog(eLOGCOMM, FALSE, TRUE, _T("Seq1001_Task_Alive Sequence END. StepNo=%d, RetVal=%d \n"), nStepNo, nRet);

	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Send Message
int WorkManager::VS_Send_Log_To_UI(TCHAR* cBuffer, int len)
{
	byte* pParam				= new byte[len + sizeof(int)];
	byte* pSendParam			= pParam;
	int			nRet			= APP_OK;

	*(int *)pSendParam = theApp.m_Config.GetPCNum();	pSendParam += sizeof(int);
	memcpy(pSendParam, cBuffer, len);					pSendParam += len;

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_UI_LOG, 0, (ULONG)(pSendParam - pParam), VS_UI_TASK, pParam, CMD_TYPE_NORES);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("VS_Send_Log_To_UI Error Occured. RetVal=%d \n"), nRet);			
		}

		SAFE_DELETE_ARR(pParam);

		return nRet;
}

int WorkManager::VS_Send_Notify_Init_To_UI()
{
	byte*	pParam			= new byte[sizeof(int)];
	byte*	pSendParam		= pParam;
	int		nRet			= APP_OK;

	*(int *)pSendParam = theApp.m_Config.GetPCNum();	pSendParam += sizeof(int);

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_UI_NOTIFY_INIT, 0, (ULONG)(pSendParam - pParam), VS_UI_TASK, pParam, CMD_TYPE_NORES);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("VS_Send_Notify_Init_To_UI Error Occured. RetVal=%d \n"), nRet);			
		}

	SAFE_DELETE_ARR(pParam);

	return nRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Recieve Message
//2016.10.17
int WorkManager::Seq_StartInspection(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
 	byte* pReceiveParam = pParam;
 
 	PARAM_INSPECT_START* pStParamInsp = new PARAM_INSPECT_START;
 	memcpy(pStParamInsp, pReceiveParam, sizeof(PARAM_INSPECT_START)); 
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection Start. \
											  InspType:%d, StageNo:%d, PanelID:%s, VirtualID:%s, nImageNo:%d, LotID:%s"), 
											  pStParamInsp->nInspType, pStParamInsp->nStageNo, pStParamInsp->strPanelID, pStParamInsp->strVirtualID, pStParamInsp->nImageNum, pStParamInsp->strLotID);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
 	BOOL		bReturn = FALSE;
 
 	int			nWidth = 0;
 	int			nHeight = 0;
 	UINT32		nBufSize = 0;
	int			nBandWidth = 0;
	int			nBitRate = 0;
 
	// yskim 20.07.03 - 스레드 파라미터 구조체 사용방식 변경 - S
	// (Mat에 메모리를 계속 할당해줘야하는 불합리 수정건)
 	//static tInspectThreadParam *pStInspectThreadParam = NULL;
	static tInspectThreadParam *pStInspectThreadParam[MAX_THREAD_PARAM_COUNT];
	static int nThreadParamIndex = 0;	// yskim 20.07.03
	static bool bFirstFlag = true;
	if (bFirstFlag)	// 처음 한번만 생성해줌.
	{
		bFirstFlag = false;
		for (int i = 0; i < MAX_THREAD_PARAM_COUNT; i++)
			pStInspectThreadParam[i] = new tInspectThreadParam;
	}
	// yskim 20.07.03 - 스레드 파라미터 구조체 사용방식 변경 - E
 
 	cv::Mat			MatOrgImage;
	CString			strDrive = _T(""); // 2018.01.18 sgkim 현재 패널에서 사용되는 드라이브, Thread Parameter로 넘김.
	ST_MODEL_INFO* pStModelInfo = new ST_MODEL_INFO;

	pStModelInfo = theApp.GetModelInfo();
	// test
	CCPUTimer tact;
	tact.Start();	
 
 	for (int nCamIndex = 0; nCamIndex < MAX_CAMERA_COUNT; nCamIndex++)
 	{
		
		// Image Classify Num 이 음수일 경우 검사 진행 안함
 		if (pStParamInsp->bUseCamera[nCamIndex] &&theApp.m_pGrab_Step[pStParamInsp->nImageNum].eImgClassify >= 0 )
 		{

 			// Send //////////////////////////////////////////////////////////////////////////////////////////////////////
 			STRU_IMAGE_INFO* pStImageInfo = new STRU_IMAGE_INFO;	
 
 			pStImageInfo->strPanelID = pStParamInsp->strPanelID;
			if (pStImageInfo->strPanelID == _T(""))
				pStImageInfo->strPanelID = _T("Unknown");
			pStImageInfo->strLotID  = pStParamInsp->strLotID;
 			pStImageInfo->nCameraNo	= nCamIndex;
 			pStImageInfo->nImageNo	= pStParamInsp->nImageNum;
			pStImageInfo->nStageNo	= pStParamInsp->nStageNo;
 			pStImageInfo->nInspType	= pStParamInsp->nInspType;
			pStImageInfo->nRatio		= pStParamInsp->nSeqMode[nCamIndex] + 1;
			pStImageInfo->strPos    = pStParamInsp->strPos;                   //현재 방향 (F or B)
			pStImageInfo->nShareImgNum = pStParamInsp->nShareImgNum;         //현재 이미지가 저장된 공유메모리 Number
																			  // Test
			pStImageInfo->strPos = _T("B");
			//pStImageInfo->nImageNo = STANDARD_IMAGE;
			for (int i=0; i<3; i++)
				pStImageInfo->dPatternCIE[i] = pStParamInsp->dPatternCIE[i];		// MTP 보정 후 [0]:X, [1]:Y, [2]:L 검사 활용
 			///////////////////////////////////////////////////////////////////////////////////////////////////////////////// 	

			nWidth = m_pSharedMemory[nCamIndex]->GetImgWidth() * pStImageInfo->nRatio;
			nHeight = m_pSharedMemory[nCamIndex]->GetImgHeight() * pStImageInfo->nRatio;
			nBandWidth = m_pSharedMemory[nCamIndex]->GetImgBandWidth();
			nBitRate = m_pSharedMemory[nCamIndex]->GetImgBitrate();
  
			CString strManualImagePath = _T("");
			CString strOrgFileName = _T("");	// 원본 이미지 파일 기준 이름 (Grab Number 제외)
			// 파일 이름은 Alg.Task 에서 확정하도록 변경 - 다중 카메라 대응
			strOrgFileName.Format(_T("%s_CAM%02d"), theApp.GetGrabStepName(pStImageInfo->nImageNo), nCamIndex);			

  			if(pStParamInsp->nInspType == (int)eManualInspect)	// 메뉴얼 검사
  			{ // 경로에서 이미지 읽어옴				

				strDrive = theApp.m_Config.GetSimulationDrive();

				if (theApp.m_pGrab_Step[pStParamInsp->nImageNum].eImgClassify != AREA_CAM_IMG_CLASSIFY_NUM) { //AreaCamera Image 아닐경우

					strManualImagePath = theApp.GetCurStepFileName(pStParamInsp->strImagePath[nCamIndex], strOrgFileName.GetBuffer(0));

					char* pStr = NULL;
					pStr = CSTR2PCH(strManualImagePath);
					MatOrgImage = cv::imread(pStr, IMREAD_UNCHANGED);

					SAFE_DELETE_ARR(pStr);
				}
				else {
					strOrgFileName = "Area";

					strManualImagePath.Format(_T("%s\\%s.bmp"), pStParamInsp->strImagePath[nCamIndex], strOrgFileName);
					MatOrgImage = cv::imread((cv::String)(CStringA)strManualImagePath, IMREAD_UNCHANGED);

				}

				//choi 영상 반전
				//cv::flip(MatOrgImage, MatOrgImage, 1);

				theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(0) %.2f"), tact.Stop(false) / 1000.);
  			}
			else	// 자동 검사
			{		// 공유메모리에서 읽어옴  
				if (pStImageInfo->nImageNo == 0)
				{
					// Log 세분화 작업. 180503 YSS
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check Start."));
					theApp.CheckDrive();
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check End."));
				}
				strDrive = theApp.m_Config.GetINIDrive();

				if (theApp.m_pGrab_Step[pStParamInsp->nImageNum].eImgClassify == AREA_CAM_IMG_CLASSIFY_NUM) { //AreaCamera Image 일 경우 경로에서 불러옴

					strOrgFileName = "Area";

					CString strImagePath;
					strImagePath.Format(
						_T("%s\\%s\\%s.bmp"),
						ORIGIN_PATH,
						pStImageInfo->strPanelID,
						strOrgFileName);

					MatOrgImage = cv::imread((cv::String)(CStringA)strImagePath, IMREAD_UNCHANGED);

				}
				else {

					if (nBitRate == 8)
					{
						if (nBandWidth == 1)
						{
						
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC1, m_pSharedMemory[nCamIndex]->GetImgAddress(pStImageInfo->nShareImgNum));

						//imwrite("C:\\GrabTest-123.bmp", MatOrgImage);


							//cv::flip(MatOrgImage, MatOrgImage, 1);
						}
						else
						{
							// SVI 원본이미지 비율에 맞춰 여백 제거
	// 						double nCropRatio = *(double*)(theApp.GetAlignParameter(nCamIndex) + E_PARA_SVI_CROP_RATIO);
	// 						//double nCropRatio =0.45;
	// 						double StartX = nWidth * (1 - nCropRatio) * (0.5);
	// 						double StartY = nHeight * (1 - nCropRatio)* (0.5);
	// 						double EndX = nWidth*nCropRatio;
	// 						double EndY = nHeight*nCropRatio;
	// 
	// 						cv::Rect rect = cv::Rect((int)StartX, (int)StartY, (int)EndX, (int)EndY);
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC3, m_pSharedMemory[nCamIndex]->GetImgAddress(pStImageInfo->nShareImgNum));
							//cv::flip(MatOrgImage, MatOrgImage, 1);
							//MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC3, m_pSharedMemory[nCamIndex]->GetImgAddress(pStParamInsp->nImageNum));
						}
					}
					else
					{
						if (nBandWidth == 1)
						{
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC1, m_pSharedMemory[nCamIndex]->GetImgAddress(pStImageInfo->nShareImgNum));
							//cv::flip(MatOrgImage, MatOrgImage, 1);
						}
						else
						{
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC3, m_pSharedMemory[nCamIndex]->GetImgAddress(pStImageInfo->nShareImgNum));
							//cv::flip(MatOrgImage, MatOrgImage, 1);
						}
					}
				}
			}
			


			if (MatOrgImage.empty())
			{
				CString strMsg = _T("");
				if (pStParamInsp->nInspType == (int)eManualInspect)
					strMsg.Format(_T("!!! No Image !!! \r\n\t (%s)"), strManualImagePath);
				else
					strMsg.Format(_T("!!! Grab Error !!! \r\n\t (Step : %s)"), theApp.GetGrabStepName(pStImageInfo->nImageNo));
				//AfxMessageBox(strMsg);
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

				// 임시로 로그만 남기고 다른 검사는 진행 - AD 불량으로 판정 - Grab Error 시 보고하는 사양 필요
				//pStInspectThreadParam->bInspectEnd[pStImageInfo->nImageNo][nCamIndex] = true;
				// 빈 이미지 할당
				if (nBitRate == 8)
				{
					if (nBandWidth == 1)
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC1, cv::Scalar(0, 0, 0));
					else
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC3, cv::Scalar(0, 0, 0));
				}
				else
				{
					if (nBandWidth == 1)
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC1, cv::Scalar(0, 0, 0));
					else
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC3, cv::Scalar(0, 0, 0));
				}
			}
			
			// Pixel Shift 사용 / 비사용 정합성 검증
			// 이전에 찍은 이미지와 현재 설정값 상이할 수 있음
			// 설정값 상이할 경우 강제로 보정하여 검사 진행
			if (pStParamInsp->nInspType == (int)eManualInspect && theApp.m_pGrab_Step[pStParamInsp->nImageNum].eImgClassify != AREA_CAM_IMG_CLASSIFY_NUM)	// 메뉴얼 검사
			{
			
				// 임시 0 처리 - 수정할 것 !!!!
				int nRet = theApp.CheckImageRatio(pStImageInfo->nRatio, MatOrgImage.cols, MatOrgImage.rows, m_pSharedMemory[nCamIndex]->GetImgWidth(), m_pSharedMemory[nCamIndex]->GetImgHeight());

				// SVI Crop 시, nRatio 3으로 변환...  확인 필요...
				//if (nRet != 0)
				//{					
				//	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Load Image Ratio Incorrect !! Change Ratio %d -> %d \r\n\t (%s) !!!"), pStImageInfo->nRatio, pStImageInfo->nRatio + nRet, strManualImagePath);
				//	pStImageInfo->nRatio += nRet;
				//}
			}

			

			// KSW 임시
			// 원본 이미지 저장 - Grab Step 추가 (현재 모델 Grab Step 기준 순서대로 다시 정렬하여 저장)
			if (nBitRate == 8)
				strOrgFileName = strOrgFileName + _T(".bmp");
			else
				strOrgFileName = strOrgFileName + _T(".tiff");





		
// 			if (GetPrivateProfileInt(_T("Common"), _T("B11 AVI Ph3"), 0, INIT_FILE_PATH) && !(pStParamInsp->nInspType == (int)eManualInspect))
// 			{
// 				cv::rotate(MatOrgImage, MatOrgImage, cv::ROTATE_90_CLOCKWISE);
// 			}

			// 2020.07.22 yskim - StartSaveImage로 대체, theApp.InspPanel.StartInspection 위쪽으로 이동
			/*ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
				ORIGIN_PATH, pStImageInfo->strPanelID, 
				pStParamInsp->nImageNum, strOrgFileName);*/

			theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(1) %.2f"), tact.Stop(false)/1000.);			


			//int nThreadParamIndex = 0;	// yskim 20.07.03

			// 최초 촬상 이미지에서 Thread Parameter 생성
 			if(pStParamInsp->nImageNum == 0)
 			{

 				if (nCamIndex == 0)
 				{
					// Panel별 Thread Parameter 생성 (최초 Pattern / Camera 에서만)
					// pStInspectThreadParam = new tInspectThreadParam;	// yskim 20.07.03 - 기존 스레드파람 제거
					for (int i = 0; i < MAX_THREAD_PARAM_COUNT; i++)	// yskim 20.07.03 - 사용가능한 스레드파람 인덱스 획득
					{
						if (pStInspectThreadParam[i]->bParamUse == false)
						{
							nThreadParamIndex = i;
							break;
						}
					}

					pStInspectThreadParam[nThreadParamIndex]->bParamUse = true;	// yskim 20.07.03 - 스레드파람 사용중 처리
 					pStInspectThreadParam[nThreadParamIndex]->ResultPanelData.m_ResultHeader.SetInspectStartTime();
					if (pStParamInsp->nInspType == (int)eAutoRun) {		// AutoRun 진행시에만 검사 진행 여부 판단, 메뉴얼 동작시에는 무조건 검사 진행
						pStInspectThreadParam[nThreadParamIndex]->bUseInspect = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Inspect"), 1, INIT_FILE_PATH) ? true : false;
					}
					else {
						pStInspectThreadParam[nThreadParamIndex]->bUseInspect = true;
						pStInspectThreadParam[nThreadParamIndex]->eInspMode = (ENUM_INSPECT_MODE)pStParamInsp->nInspType;
						pStInspectThreadParam[nThreadParamIndex]->strImagePath_LCP_Bubble = pStParamInsp->strImagePath[nCamIndex];
					}
					// 결과값 저장할 폴더 미리 만들기.
					// 이미 폴더가 있으면 내용 모두 지움
					CreateResultDirectory(pStParamInsp->strPanelID, strDrive);

					


					// 현재 설정된 WorkCoord 관련 정보를 모든 스레드에 전달
					memcpy(&pStInspectThreadParam[nThreadParamIndex]->WrtResultInfo, theApp.GetCurWorkCoordInfo(), sizeof(CWriteResultInfo));

 					for(int nStepCnt = 0 ; nStepCnt < theApp.GetGrabStepCount() ; nStepCnt++)
 					{
						// Image Classify Num 이 음수일 경우 검사 진행 안함
 						if(theApp.m_pGrab_Step[nStepCnt].bUse && theApp.m_pGrab_Step[nStepCnt].eImgClassify >= 0 && theApp.m_pGrab_Step[nStepCnt].eImgClassify) //AreaCamera Image따로 설정
 						{
							for (int nCamCnt = 0; nCamCnt < MAX_CAMERA_COUNT; nCamCnt++)
							{
								if (theApp.m_pGrab_Step[nStepCnt].stInfoCam[nCamCnt].bUse)
								{
									pStInspectThreadParam[nThreadParamIndex]->bInspectEnd[nStepCnt][nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bAlignEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bStAlignEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bStBackAlignEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bStVacuumAlignEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bStLabelMaskMakeEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->bStStageDustEnd[nCamCnt] = false;
									pStInspectThreadParam[nThreadParamIndex]->nError_Thread[nCamCnt] = E_ERROR_CODE_TRUE;
									pStInspectThreadParam[nThreadParamIndex]->nError_Image_Num[nCamCnt] = -1;
									pStInspectThreadParam[nThreadParamIndex]->bAlign_Success[nCamCnt] = false;

									for (int nAlgCnt = 0; nAlgCnt < MAX_MEM_SIZE_E_ALGORITHM_NUMBER; nAlgCnt++) {
										if (theApp.GetUseAlgorithm(nStepCnt, nCamCnt, 0, nAlgCnt)) {
											pStInspectThreadParam[nThreadParamIndex]->bInspectAlgEnd[nStepCnt][nCamCnt][nAlgCnt] = false;
										}
										else {
											pStInspectThreadParam[nThreadParamIndex]->bInspectAlgEnd[nStepCnt][nCamCnt][nAlgCnt] = true;
										}
									}

								}
							}							
						}
					}

					pStInspectThreadParam[nThreadParamIndex]->bChkDustEnd = false;		// AVI 용 Dust Check 종료 플래그
					pStInspectThreadParam[nThreadParamIndex]->bUseDustRetry = GetPrivateProfileInt(_T("INSPECT"), _T("Use_Dust_Retry"), 1, INIT_FILE_PATH) ? true : false;
					pStInspectThreadParam[nThreadParamIndex]->nDustRetryCnt = pStParamInsp->nRetryCnt;
					COPY_CSTR2TCH(pStInspectThreadParam[nThreadParamIndex]->strSaveDrive,strDrive,sizeof(pStInspectThreadParam[nThreadParamIndex]->strSaveDrive));
					theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(2) %.2f"), tact.Stop(false)/1000.);					
				}
 			}
			
			

			if (pStInspectThreadParam != NULL)
			{

				// yskim 20.07.03 - clone에서 copyTo로 변경 (copyTo는 메모리할당이 필요할때만 메모리할당을 함)
				//pStInspectThreadParam->MatOrgImage[pStImageInfo->nImageNo][nCamIndex] = MatOrgImage.clone();	//메모리 할당 Pattern 당 전체영상

				MatOrgImage.copyTo(pStInspectThreadParam[nThreadParamIndex]->MatOrgImage[pStImageInfo->nImageNo][nCamIndex]);

				// 2020.07.22 yskim - 원본 영상 저장
				CString strImagePath;

				if (theApp.m_pGrab_Step[pStParamInsp->nImageNum].eImgClassify != AREA_CAM_IMG_CLASSIFY_NUM) {
					pStInspectThreadParam[nThreadParamIndex]->strImagePath[pStImageInfo->nImageNo][nCamIndex].Format(
						_T("%s\\%s\\%02d_%s"),
						ORIGIN_PATH,
						pStImageInfo->strPanelID,
						pStParamInsp->nImageNum,
						strOrgFileName);
				}
				else {
					pStInspectThreadParam[nThreadParamIndex]->strImagePath[pStImageInfo->nImageNo][nCamIndex].Format(
						_T("%s\\%s\\%s"),
						ORIGIN_PATH,
						pStImageInfo->strPanelID,
						strOrgFileName);

					//AreaCamera Image -> Main Insp Image 해상도로 변환 
				}
		
				// yskim 11.15
				theApp.InspPanel.StartSaveImage(
					(WPARAM)&pStInspectThreadParam[nThreadParamIndex]->MatOrgImage[pStImageInfo->nImageNo][nCamIndex], 
					(LPARAM)&pStInspectThreadParam[nThreadParamIndex]->strImagePath[pStImageInfo->nImageNo][nCamIndex]);
			}

 
			// GVO 요청사항 Auto일 경우 중간 이미지 안남도록 수정 - 190425 YWS
			if (pStParamInsp->nInspType == (int)eAutoRun)
			    theApp.SetCommonParameter(false);
			int nThreadCount = theApp.InspPanel.StartInspection((WPARAM)pStInspectThreadParam[nThreadParamIndex], (LPARAM)pStImageInfo);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartInspection End. RetVal:%d, ImageNo:%d, ThreadCount:%d"), bReturn, pStParamInsp->nImageNum, nThreadCount);

		
 		}
 	}
	SAFE_DELETE(pStParamInsp);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(3) %.2f"), tact.Stop(false)/1000.);
 
	return bReturn;
}

//2016.10.26
int WorkManager::Seq_ManualAlign(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{	
	byte*		tempParam			= pParam;	

	cv::Point	ptCorner[4];		// (공통)Cell 최 외곽 꼭지점
	cv::Rect	AlignCellROI;		// Edge ROI
	cv::Rect	AlignPadROI;		// Pad ROI
	cv::Rect	AlignActiveROI;		// Active ROI
	double		AlignTheta=0;		// 회전 각도
	cv::Point	AlignCenter;		// 회전 시, 중심

	// Receive Parameter ////////////////////////////////////////////////////////////////////////////////////////////////////
	double		dAlignPara[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT] = {0, };			// Align Parameter - Camera 당 최대 15개
	TCHAR		strOrgImagePath[1000];												// 원본 이미지 경로

	memcpy(dAlignPara, tempParam, sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT);
	tempParam += sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT;

	memcpy(strOrgImagePath, tempParam, sizeof(TCHAR) * 1000);				tempParam += sizeof(TCHAR) * 1000;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	char* pStr = NULL;
	pStr = CSTR2PCH(strOrgImagePath);  
	cv::Mat MatAlignImage = cv::imread(pStr, IMREAD_UNCHANGED);
	SAFE_DELETE_ARR(pStr);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_ManualAlign Start. ImagePath : %s"), strOrgImagePath);

	CRect rectROI;
	cv::Point ptTemp; 

	// Align 알고리즘 실행 ( 각도 결과 : dOriginTheta / 꼭지점 결과 : ptCorner )
	int nEQType = theApp.m_Config.GetEqpType();
	
	// KSW 18.02.12 - 수동 Align 카메라 0번 고정
	// SCJ 18.08.03 - 수동 Align Pannel Size 및 Pixel resolution 추가
	
	CWriteResultInfo* WrtResultInfo;
	WrtResultInfo = new CWriteResultInfo;
	long nErrorCode = Align_FindActive(MatAlignImage, dAlignPara, AlignTheta, ptCorner, 0, nEQType,theApp.GetCurWorkCoordInfo()->GetCamResolution(0), 
		                              theApp.GetCurWorkCoordInfo()->GetPanelSizeX(),theApp.GetCurWorkCoordInfo()->GetPanelSizeY());

	// 에러인 경우, 에러 코드 & 로그 출력
	if( nErrorCode != 0 )
	{
		// Error Log
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Error Occured In Seq_ManualAlign (1). ErrorCode : %d"), nErrorCode);
		return nErrorCode;
	}

	// 회전 중심
	AlignCenter.x = MatAlignImage.cols / 2;
	AlignCenter.y = MatAlignImage.rows / 2;

	//// 영상 회전
	//nErrorCode = Align_RotateImage(MatAlignImage, MatAlignImage, AlignTheta);

	//// 에러인 경우, 에러 코드 & 로그 출력
	//if( nErrorCode != 0 )
	//{
	//	// Error Log
	//	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Error Occured In Seq_ManualAlign (2). ErrorCode : %d"), nErrorCode);
	//	return nErrorCode;
	//}

	// 회전 좌표로 보정

	// LEFT - TOP
	Align_DoRotatePoint(ptCorner[0], ptTemp, AlignCenter, AlignTheta);
	rectROI.left	= ptTemp.x;
	rectROI.top		= ptTemp.y;

	// RIGHT - BOTTOM
	Align_DoRotatePoint(ptCorner[2], ptTemp, AlignCenter, AlignTheta);
	rectROI.right	= ptTemp.x;
	rectROI.bottom	= ptTemp.y;

	//AdjustOriginImage(MatOrgImage, MatDrawImage, &stCamAlignInfo[0]);

	// 회전된 임시 ROI 결과 넣기
	AlignCellROI	= cv::Rect(rectROI.left, rectROI.top, rectROI.Width(), rectROI.Height());
	AlignPadROI		= AlignCellROI;
	AlignActiveROI	= AlignCellROI;


	// test After Image Save
	//cv::imwrite("D:\\test_After.BMP", MatAlignedImg);

	// Response Parameter 에 결과값 적재하는 부분 - 사이즈 변경이 필요할 경우 Recive Parameter 부터 변경해야 함
	*(double*)pParam = AlignTheta;
	pParam += sizeof(double);
	*(cv::Rect*)pParam = AlignCellROI;
	pParam	+= sizeof(cv::Rect);
	*(cv::Rect*)pParam = AlignPadROI;
	pParam	+= sizeof(cv::Rect);
	*(cv::Rect*)pParam = AlignActiveROI;
	pParam	+= sizeof(cv::Rect);


	// 12 비트 모노 -> 8 비트 모노 변환
// 	int nChCnt = MatAlignImage.channels();  int imgDepth = MatAlignImage.depth();
// 	if(nChCnt == 1 && imgDepth == CV_16U) MatAlignImage.convertTo(MatAlignImage, CV_8UC1, 1./16.);
// 	ImageSave(MatAlignImage, ALIGN_IMAGE_PATH);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_ManualAlign End (T : %lf). Return AlignActiveROI(%d,%d,%d,%d)"), 
											AlignTheta, AlignActiveROI.x, AlignActiveROI.y, AlignActiveROI.width, AlignActiveROI.height);

	return APP_OK;
}

int WorkManager::Seq_SetParam(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
 	byte*  tempParam   = pParam;
	TCHAR strModelPath[200];
	TCHAR strModelID[50];
	ST_RECIPE_INFO* pStRecipeInfo = new ST_RECIPE_INFO();
	ST_MODEL_INFO* pStModelInfo = new ST_MODEL_INFO();
	CS::Rect rcPadArea[MAX_CAMERA_COUNT];
 
	// Receive ///////////////////////////////////////////////////////////////////////////////////////////////////////

	//APPLY Model 명을 가질 경우는 Recipe 경로 Path Update안함, APP Algorithm에서 경로를 사용 중이며
	//Reference 불러오기/저장하기 기능을 위해 사용한다. 저장하기 기능일 시 APPLY Model Recipe에 저장하게 되기때문에 이러한 기능을 구현해 놈 by LJM 2017.11.22
	memcpy(strModelPath, tempParam, sizeof(WCHAR) * 200);			tempParam += sizeof(WCHAR) * 200;
	TCHAR strTest[50];
	TCHAR* strApply = _tcsstr(strModelPath, _T("APPLY"));
	_stprintf_s(strTest,50,_T("%s"),strApply);
	if(_tcsncmp(strTest,_T("APPLY"), 5)!=0)	
		theApp.SetCurInspRecipePath(strModelPath);

	memcpy(strModelID, tempParam, sizeof(WCHAR) * 50);				tempParam += sizeof(WCHAR) * 50;
	theApp.SetCurModelID(strModelID);
 	
	memcpy(pStRecipeInfo, tempParam, sizeof(ST_RECIPE_INFO));		tempParam += sizeof(ST_RECIPE_INFO);

	// 카메라별 얼라인 파라메터 설정
	theApp.SetAlignParameter((double *)tempParam);
	tempParam += sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT;

	theApp.SetCommonParameter((ST_COMMON_PARA *)tempParam);
	tempParam += sizeof(ST_COMMON_PARA);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_SetParam Start. ModelID : %s, Path : %s"), strModelID, strModelPath);

	double dCamResolution[MAX_CAMERA_COUNT] = {0.0, };	
	for (int nCamIndex = 0; nCamIndex < theApp.m_Config.GetUseCamCount(); nCamIndex++)
	{
		// Camera별 Resolution
		dCamResolution[nCamIndex] = pStRecipeInfo->stCamInfo[nCamIndex].dResolution;
// 		// Camera별 Pad 영역
// 		rcPadArea[nCamIndex] = pStRecipeInfo->stCamInfo[nCamIndex].rcPad;
// 		// Camera별 Pad Reference Image 로딩
// 
// 		//2017.06.08 PAD ROI 받아오기
// 		theApp.SetPadRefROI(rcPadArea[nCamIndex], nCamIndex);
// 
// 		//17.06.20 Log 로 변경
// 				//CString strMsg = _T("");
// 				if (!theApp.SetPadRefImage(pStRecipeInfo->stCamInfo[nCamIndex].strPadRefPath, nCamIndex))
// 				{
// 					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Pad Reference Image Not Found [Path:%s]"), pStRecipeInfo->stCamInfo[nCamIndex].strPadRefPath);
// 					strMsg.Format(_T("Pad Reference Image Not Found [Path:%s]"), pStRecipeInfo->stCamInfo[nCamIndex].strPadRefPath);
// 					AfxMessageBox(strMsg);
// 				}
// 				// Camera별 Mura Reference Image 로딩
// 				if (!theApp.SetMuraRefImage(pStRecipeInfo->stCamInfo[nCamIndex].strMuraRefPath, nCamIndex))
// 				{
// 					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Mura Reference Image Not Found [Path:%s]"), pStRecipeInfo->stCamInfo[nCamIndex].strMuraRefPath);
// 					strMsg.Format(_T("Mura Reference Image Not Found [Path:%s]"), pStRecipeInfo->stCamInfo[nCamIndex].strMuraRefPath);
// 					AfxMessageBox(strMsg);
// 				}
	}
	/// 워크좌표 계산에 필요한 정보 설정
	/// 17.07.07 기존 Work 좌표 원점은 LT/RT/RB/LB 로부터의 Offset 으로 활용
	theApp.SetWorkCoordInfo(pStRecipeInfo->dPanelSizeX, pStRecipeInfo->dPanelSizeY, pStRecipeInfo->nWorkDirection, pStRecipeInfo->nWorkOriginPosition,
		pStRecipeInfo->nOriginOffsetX, pStRecipeInfo->nOriginOffsetY,
		pStRecipeInfo->nDataDirection, pStRecipeInfo->nGateDataOriginPosition, 
		pStRecipeInfo->nGateDataOriginOffsetX, pStRecipeInfo->nGateDataOriginOffsetY, pStRecipeInfo->dGatePitch, pStRecipeInfo->dDataPitch,
		dCamResolution);

	SAFE_DELETE(pStRecipeInfo);	

	// KSW 17.11.03 - [Round] 등록 저장 경로
	bool bApply = false;

	// APPLY 제외해야
	{
		CString strApply;
		strApply.Format(_T("%s"), strModelPath);
		int nRes = strApply.Find( _T("APPLY") );
		
		// 찾지 못한 경우 Apply
		if( nRes == -1 )
			theApp.SetRoundPath(strModelPath);
	}

	// 알고리즘 파라메터 (XML) 파일 로딩
	if ( theApp.ReadAlgorithmParameter(strModelPath, theApp.GetRoundPath()) )
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Read Algorithm Parameter Successfully. Model ID : %s"), strModelID);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Algorithm Parameter !!!. Model ID : %s"), strModelID);

	// 알고리즘 파라메터 (XML) 파일 로딩
	if ( theApp.ReadPadInspParameter(strModelPath) )
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Read Algorithm Parameter Successfully. Model ID : %s"), strModelID);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Algorithm Parameter !!!. Model ID : %s"), strModelID);

// for test
// 	CCPUTimer tact;
// 	tact.Start();

	// 판정 파라메터 파일 로딩
	if (theApp.ReadJudgeParameter(strModelPath))
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Read Judge Parameter Successfully. Model ID : %s"), strModelID);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Judge Parameter !!!. Model ID : %s"), strModelID);

	// 사용자 정의 필터
	if (theApp.ReadUserDefinedFilter(strModelPath))
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Read User Defined filter Successfully. Model ID : %s"), strModelID);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read User Defined filter Parameter !!!. Model ID : %s"), strModelID);

	// 대표 불량 선정 순위
	if (theApp.ReadDefectClassify(strModelPath))
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Read Defect Classify Successfully. Model ID : %s"), strModelID);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Failed Read Defect Classify Parameter !!!. Model ID : %s"), strModelID);

//	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("(Read Judge Tact : %.2f)"), tact.Stop(false)/1000.);

	//////////////////////////////////////////////////////////////////////////

	const int nEqpType = theApp.m_Config.GetEqpType();
	long nErrorCode = E_ERROR_CODE_TRUE;

	switch(nEqpType)
	{
		case EQP_AVI:
			// Load CCD Defect Index
			nErrorCode = CCD_IndexLoad(CCD_DEFECT_FILE_PATH, CCD_DEFECT_FILE_PATH2);

			// 에러인 경우, 에러 코드 & 로그 출력
			if( nErrorCode != E_ERROR_CODE_TRUE )
			{
				// Error Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, TRUE, _T("Error Occured In Load CCD Defect Index. ErrorCode : %d"), nErrorCode);
			}
			else
			{
				// Successfully Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, TRUE, _T("Load CCD Defect Index Successfully."));
			}
			break;

		case EQP_SVI:
			// SVI 컬러 보정 관련
			nErrorCode = ColorCorrection_Load(COLOR_CORRECTION_FILE_PATH);

			// 에러인 경우, 에러 코드 & 로그 출력
			if( nErrorCode != E_ERROR_CODE_TRUE )
			{
				// Error Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, TRUE, _T("Error Occured In Load Color Correction. ErrorCode : %d"), nErrorCode);
			}
			else
			{
				// Successfully Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, TRUE, _T("Load Color Correction Successfully."));
			}
			break;

		case EQP_APP:
			break;

		default:
			break;
	}

	//////////////////////////////////////////////////////////////////////////

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_SetParam End. Model ID : %s"), strModelID);

	return APP_OK;
}

int WorkManager::Seq_FocusValue(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	byte*		tempParam			= pParam;
	UINT		nWidth, nHeight, nSize;
	
	nWidth	= *(int*)tempParam;	tempParam	+= sizeof(int);
	nHeight	= *(int*)tempParam;	tempParam	+= sizeof(int);
	nSize = nWidth * nHeight;

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_FocusValue Start. ImageWidth:%d, ImageHeight:%d"), nWidth, nHeight);

	byte* Imagebuf;
	Imagebuf = new byte[nSize];
	Imagebuf	= tempParam;			tempParam		+= nSize;

	Mat MatOrgImage = Mat(nHeight, nWidth, CV_8UC1, Imagebuf);
	CRect rtFocusRect(0,0,nWidth,nHeight);

	double dFoucsVal =  theApp.CallFocusValue(MatOrgImage,rtFocusRect);

	*(double*)pParam = dFoucsVal;
	pParam	+= sizeof(double);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_FocusValue End. Return FocusVal:%.3lf"), dFoucsVal);

	return APP_OK;

}

int WorkManager::VS_Send_ClassifyEnd_To_Seq(CString strPanelID, CString strDrive, UINT nDefectCount, CString strPanelGrade, CString strJudgeCode1, CString strJudgeCode2 )
{
	int nRet = APP_OK;
	int nParamSize = 0;
	byte bParam[1000] = {0,};
	byte* bpTemp = &bParam[0];

	PARAM_CLASSIFY_END* stClassifyEnd = new PARAM_CLASSIFY_END;

	COPY_CSTR2TCH(stClassifyEnd->strPanelID, strPanelID, sizeof(stClassifyEnd->strPanelID));
	stClassifyEnd->nPCNum = theApp.m_Config.GetPCNum();
	stClassifyEnd->nDefectCount = nDefectCount;			
// 패널 판정 적어주도록 변경
// 	if( stClassifyEnd->nDefectCount > 0)
// 		stClassifyEnd->bIsOK = FALSE;
// 	else
// 		stClassifyEnd->bIsOK = TRUE;
	COPY_CSTR2TCH(stClassifyEnd->strPanelJudge, strPanelGrade, sizeof(stClassifyEnd->strPanelJudge));
	COPY_CSTR2TCH(stClassifyEnd->strJudgeCode1, strJudgeCode1, sizeof(stClassifyEnd->strJudgeCode1));
	COPY_CSTR2TCH(stClassifyEnd->strJudgeCode2, strJudgeCode2, sizeof(stClassifyEnd->strJudgeCode2));
	COPY_CSTR2TCH(stClassifyEnd->strSavedDrive, strDrive.Left(7), sizeof(stClassifyEnd->strSavedDrive));

	*(PARAM_CLASSIFY_END*)bpTemp =*stClassifyEnd;
	bpTemp += sizeof(PARAM_CLASSIFY_END);
	SAFE_DELETE(stClassifyEnd);

	nParamSize = (int)(bpTemp - bParam);
	nRet = CmdEditSend(SEND_SEQ_CLASSIFY_END, 0, (USHORT)(bpTemp - bParam), VS_SEQUENCE_TASK, bParam, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Classify End Failed !!! Panel ID : %s (ErrorCode : %d)"), strPanelID, nRet);
		return APP_NG;		
	}
	
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Classify End. Panel ID : %s"), strPanelID);

	return nRet;
}

int WorkManager::VS_Send_Alarm_Occurred_To_MainPC_LCP(ENUM_INSPECT_MODE eInspMode, CString cstrAlarmType, CString cstrAlarmName, CString cstrAlarmMsg,int nAlarmID, int nAlarmType, bool& bIsHeavyAlarm) //알람명 , 알람 메세지
{
	// Main PC 는 Auto Mode 일때만 연동
	if (eInspMode != eAutoRun)
		return APP_OK;

	int nRet = APP_OK;
	int nParamSize = 0;
	byte bParam[1000] = { 0, };
	byte* bpTemp = &bParam[0];

	//////////////////////////////////////////////////////////////////////////
	// Alarm Type
	const int nAlarmType_Size = 100;
	BYTE AlarmType[nAlarmType_Size] = { 0, };
	CString strAlarmType("");
	strAlarmType = cstrAlarmType;
	int nAlarmType_Lenght = strAlarmType.GetLength();
	memcpy(AlarmType, (VOID*)LPCTSTR(strAlarmType), strAlarmType.GetLength()*2);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Alarm Type
	const int nAlarmName_Size = 200;
	BYTE AlarmName[nAlarmName_Size] = { 0, };
	CString strAlarmName("");
	strAlarmName = cstrAlarmName;
	int nAlarmName_Lenght = strAlarmName.GetLength();
	memcpy(AlarmName, (VOID*)LPCTSTR(strAlarmName), strAlarmName.GetLength() * 2);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Alarm Type
	const int nAlarmMsg_Size = 400;
	BYTE AlarmMsg[nAlarmMsg_Size] = { 0, };
	CString strAlarmMsg("");
	strAlarmMsg = cstrAlarmMsg;
	int nAlarmMsg_Lenght = strAlarmMsg.GetLength();
	memcpy(AlarmMsg, (VOID*)LPCTSTR(strAlarmMsg), strAlarmMsg.GetLength() * 2);
	//////////////////////////////////////////////////////////////////////////

	// 중알람 발생 시 다른 스레드 컨트롤을 위해 중알람 발생 여부 플래그 ON
	if (nAlarmType == eALARMTYPE_HEAVY)
		bIsHeavyAlarm = true;

	//////////////////////////////////////////////////////////////////////////
	//Alarm ID
	*(int *)bpTemp = nAlarmID;			bpTemp += sizeof(int);

	//////////////////////////////////////////////////////////////////////////
	//Alarm Type
	 	for (int i = 0; i < nAlarmType_Lenght * 2; i++ ,bpTemp += sizeof(byte)) {
	 		*(byte *)bpTemp = AlarmType[i];
	 	}

	 	bpTemp += sizeof(byte)*(nAlarmType_Size -(nAlarmType_Lenght*2));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Alarm Type

		for (int i = 0; i < nAlarmName_Lenght * 2; i++, bpTemp += sizeof(byte)) {
			*(byte *)bpTemp = AlarmName[i];
		}

		bpTemp += sizeof(byte)*(nAlarmName_Size - (nAlarmName_Lenght * 2));
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//Alarm Msg

		for (int i = 0; i < nAlarmMsg_Lenght * 2; i++, bpTemp += sizeof(byte)) {
			*(byte *)bpTemp = AlarmMsg[i];
		}

		bpTemp += sizeof(byte)*(nAlarmMsg_Size - (nAlarmMsg_Lenght * 2));
	//////////////////////////////////////////////////////////////////////////

	nParamSize = (int)(bpTemp - bParam);
	nRet = CmdEditSend(SEND_MAINPC_ALARM_OCCURRED, 0, (USHORT)(bpTemp - bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{

		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Alarm Occurred Failed. (%d:%d)"), nAlarmID, nAlarmType);
		return APP_NG;
	}



	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Alarm Occurred. (%d:%d)"), nAlarmID, nAlarmType);

	return nRet;
}

int WorkManager::VS_Send_Alarm_Occurred_To_MainPC(ENUM_INSPECT_MODE eInspMode, int nAlarmID, int nAlarmType, bool& bIsHeavyAlarm) //알람명 , 알람 메세지
{
	// Main PC 는 Auto Mode 일때만 연동
 	if (eInspMode != eAutoRun)
 		return APP_OK;

	int nRet = APP_OK;
	int nParamSize = 0;
	byte bParam[1000] = {0,};
	byte* bpTemp = &bParam[0];

	// 중알람 발생 시 다른 스레드 컨트롤을 위해 중알람 발생 여부 플래그 ON
	if (nAlarmType == eALARMTYPE_HEAVY)
		bIsHeavyAlarm = true;

	*(int *)bpTemp = nAlarmID;			bpTemp += sizeof(int);
	*(int *)bpTemp = nAlarmType;		bpTemp += sizeof(int);

	nParamSize = (int)(bpTemp - bParam);
	nRet = CmdEditSend(SEND_MAINPC_ALARM_OCCURRED, 0, (USHORT)(bpTemp - bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Alarm Occurred Failed. (%d:%d)"), nAlarmID, nAlarmType);
		return APP_NG;		
	}

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Alarm Occurred. (%d:%d)"), nAlarmID, nAlarmType);

	return nRet;
}

int WorkManager::VS_Send_Align_Result_To_MainPC(ENUM_INSPECT_MODE eInspMode, int nTheta, int nDistX, int nDistY, int nStageNo, int nPCNo)
{
	// Main PC 는 Auto Mode 일때만 연동
 	if (eInspMode != eAutoRun)
 		return APP_OK;

	int nRet = APP_OK;
	int nParamSize = 0;
	byte bParam[1000] = {0,};
	byte* bpTemp = &bParam[0];

	*(int *)bpTemp = nTheta;			bpTemp += sizeof(int);
	*(int *)bpTemp = nDistX;			bpTemp += sizeof(int);
	*(int *)bpTemp = nDistY;			bpTemp += sizeof(int);
	*(int *)bpTemp = nStageNo;			bpTemp += sizeof(int);
	*(int *)bpTemp = nPCNo;				bpTemp += sizeof(int);

	nParamSize = (int)(bpTemp - bParam);
	nRet = CmdEditSend(SEND_MAINPC_ALIGN_RESULT, 0, (USHORT)(bpTemp - bParam), VS_MAIN_PC_TASK, bParam, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Send Align Result Failed. (%d:%d,%d)"), nTheta, nDistX, nDistY);
		return APP_NG;		
	}

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Send Align Result. (%d:%d,%d)"), nTheta, nDistX, nDistY);

	return nRet;
}

void WorkManager::CreateResultDirectory(TCHAR* strPanelID, CString strDrive)
{
	// 결과 폴더 생성
	CString strPathfolder = _T("");
	strPathfolder.Format(_T("%s\\%s"), INSP_PATH, strPanelID);
	if (GetFileAttributes(strPathfolder) == -1)
		SHCreateDirectoryEx(NULL, strPathfolder, NULL);
	else
		DeleteAllFilesWithDirectory(strPathfolder);

	strPathfolder.Format(_T("%s\\%s\\ROI"), INSP_PATH, strPanelID);
	if (GetFileAttributes(strPathfolder) == -1)
		SHCreateDirectoryEx(NULL, strPathfolder, NULL);
	else
		DeleteAllFilesWithDirectory(strPathfolder);

	strPathfolder.Format(_T("%s\\%s"), RESULT_PATH, strPanelID);
	if (GetFileAttributes(strPathfolder) == -1)
		SHCreateDirectoryEx(NULL, strPathfolder, NULL);
	else
		DeleteAllFilesWithDirectory(strPathfolder);
	
	//YHS 18.03.12 - Merge_Tool Cell_ID 폴더 생성 
	//	김형주 18.12.06
	//	MergeTool Falg 와 상관없이 무조건 동작
	//if(theApp.GetMergeToolUse())
	{
		strPathfolder.Format(_T("%s\\%s"), MERGETOOL_PATH, strPanelID);
		if (GetFileAttributes(strPathfolder) == -1)
			SHCreateDirectoryEx(NULL, strPathfolder, NULL);
		else
		{
			DeleteAllFilesWithDirectory(strPathfolder);
		}
	}	

	// 중간 결과값 저장 폴더 생성	
	if (theApp.GetCommonParameter()->bIFImageSaveFlag)
	{
		strPathfolder.Format(_T("%s\\%s"), ALG_RESULT_PATH, strPanelID);	// ARESULT - Panel ID 기입하도록 변경됨에 따라 Panel ID 추가하여 찾도록 수정 by CWH
		if (GetFileAttributes(strPathfolder) == -1)
			SHCreateDirectoryEx(NULL, strPathfolder, NULL);
		else
			DeleteAllFilesWithDirectory(strPathfolder);
	}
}

// Align 하는 Pattern Num 반환
int WorkManager::Seq_GetAlignPatternNum(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_GetAlignPatternNum Start."));
	int nEQPType = theApp.m_Config.GetEqpType();

	double* dAlignPara = theApp.GetAlignParameter(0);

	switch(nEQPType)
	{
	case EQP_AVI:
	case EQP_LCP:
		*(int *)pParam = theApp.GetImageNum((int)dAlignPara[E_PARA_LCP_AD_IMAGE_NUM]);
		break;
		
	case EQP_SVI:
		*(int *)pParam = theApp.GetImageNum((int)dAlignPara[E_PARA_AD_IMAGE_NUM]);
		break;

	case EQP_APP:
		*(int *)pParam = theApp.GetImageNum((int)dAlignPara[E_PARA_APP_AD_IMAGE_NUM]);
		break;
	}


	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_GetAlignPatternNum END. (RetVal : %d)"), *(int *)pParam);

	return APP_OK;
}

int WorkManager::Seq_StartAlign(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	// Receive //////////////////////////////////////////////////////////////////////////////////////////////////////
	byte* pReceiveParam = pParam;

	PARAM_ALIGN_START* pStParamAlign = (PARAM_ALIGN_START*)pReceiveParam;
	//memcpy(pStParamAlign, pReceiveParam, sizeof(PARAM_ALIGN_START));
	pReceiveParam += sizeof(PARAM_ALIGN_START);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartAlign Start. StageNo:%d, PanelID:%s, VirtualID:%s, nImageNo:%d"), 
																pStParamAlign->nStageNo, pStParamAlign->strPanelID, pStParamAlign->strVirtualID, pStParamAlign->nImageNum);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long		nErrorCode = E_ERROR_CODE_TRUE;

	int			nWidth = 0;
	int			nHeight = 0;
	UINT32		nBufSize = 0;
	int			nBandWidth = 0;
	int			nBitRate = 0;

	cv::Mat		MatOrgImage;
	CString		strDrive = _T("");

	double dAlignPara[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT] = {0.0, };
	memcpy(dAlignPara, theApp.GetAlignParameter(0), sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT);

	// Align Sequence 에서는 P/S Image 사용 안하므로 Parameter 에 Ratio 적용
	dAlignPara[E_PARA_CELL_SIZE_X] *= (1.0 / (pStParamAlign->nSeqMode[0] + 1));
	dAlignPara[E_PARA_CELL_SIZE_Y] *= (1.0 / (pStParamAlign->nSeqMode[0] + 1));

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// test
	CCPUTimer tact;
	tact.Start();	

	for (int nCamIndex = 0; nCamIndex < MAX_CAMERA_COUNT; nCamIndex++)
	{
		if (pStParamAlign->bUseCamera[nCamIndex])
		{
			if (_tcscmp(pStParamAlign->strPanelID, _T("")) == 0)
				_tcscpy(pStParamAlign->strPanelID, _T("Unknown"));

			// Align Sequence 는 무조건 Non-P/S Image
			nWidth = m_pSharedMemory[nCamIndex]->GetImgWidth();// * (pStParamAlign->nSeqMode[nCamIndex] + 1);
			nHeight = m_pSharedMemory[nCamIndex]->GetImgHeight();// * (pStParamAlign->nSeqMode[nCamIndex] + 1);
			nBandWidth = m_pSharedMemory[nCamIndex]->GetImgBandWidth();
			nBitRate = m_pSharedMemory[nCamIndex]->GetImgBitrate();

			CString strManualImagePath = _T("");
			CString strOrgFileName = _T("");	// 원본 이미지 파일 기준 이름 (Grab Number 제외)
			// 파일 이름은 Alg.Task 에서 확정하도록 변경 - 다중 카메라 대응
			strOrgFileName.Format(_T("ALIGN_CAM%02d"), nCamIndex);			

			{		// 공유메모리에서 읽어옴  
				// 12비트 대응 추가
				if (nBitRate == 8)
				{
					if (nBandWidth == 1)
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC1, m_pSharedMemory[nCamIndex]->GetImgAddress(pStParamAlign->nImageNum));
					else
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC3, m_pSharedMemory[nCamIndex]->GetImgAddress(pStParamAlign->nImageNum));
				}
				else
				{
					if (nBandWidth == 1)
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC1, m_pSharedMemory[nCamIndex]->GetImgAddress(pStParamAlign->nImageNum));
					else
						MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC3, m_pSharedMemory[nCamIndex]->GetImgAddress(pStParamAlign->nImageNum));
				}
			}

			if (MatOrgImage.empty())
			{
				CString strMsg = _T("");
				strMsg.Format(_T("!!! Grab Error !!! \r\n\t (Step : %s)"), theApp.GetGrabStepName(pStParamAlign->nImageNum));
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

				return APP_NG;
			}

			// KSW 임시
			// 원본 이미지 저장 - Grab Step 추가 (현재 모델 Grab Step 기준 순서대로 다시 정렬하여 저장)
			if (nBitRate == 8)
				strOrgFileName = strOrgFileName + _T(".bmp");
			else
				strOrgFileName = strOrgFileName + _T(".tiff");
			theApp.CheckDrive();
			strDrive = theApp.m_Config.GetINIDrive();
			ImageSave(MatOrgImage, _T("%s\\%s\\%s"),
				ORIGIN_PATH, pStParamAlign->strPanelID, 
				strOrgFileName);

			theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(1) %.2f"), tact.Stop(false)/1000.);

			double dResult[4] = {0.0, };

			// 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
			// PS 영상에따라, 1샷 좌표로 변경해야 하나, Align Sequence 에서는 P/S Image 사용 안함.
			// nRatio : 1

			nErrorCode = Align_FindDefectAD(MatOrgImage, dAlignPara, dResult, 1, nCamIndex, nEqpType);

			theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(2) %.2f"), tact.Stop(false)/1000.);

			if (nErrorCode != E_ERROR_CODE_TRUE)	// AD 불량인 경우 Align 안함 - Log 만 남기고 정상 Sequence 진행 (AD 불량 보고)
			{
				// Alg DLL Stop Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, 
					_T("AD Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Rate : %.2f %% / Avg : %.2f %% / Std : %.2f %% ) ErrorCode : %d"),
					pStParamAlign->strPanelID, pStParamAlign->nStageNo, nCamIndex, theApp.GetGrabStepName(pStParamAlign->nImageNum), dResult[0], dResult[1], dResult[2], nErrorCode);
				nErrorCode = E_ERROR_CODE_TRUE;		// Camera Align Sequence 에서 AD 불량일 경우 AD 불량 보고를 위해 TRUE 리턴
			}
			else
			{
				// KSW 17.07.10 Cell 중심 좌표 추가
				cv::Point ptCellCenter;
				nErrorCode = Align_FindTheta(MatOrgImage, dAlignPara, pStParamAlign->dAdjTheta[0], ptCellCenter, pStParamAlign->strPanelID);

				// 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
				// PS 영상에따라, 1샷 좌표로 변경해야 하나, Align Sequence 에서는 P/S Image 사용 안함.
				//ptCellCenter.x /= nRatio;
				//ptCellCenter.y /= nRatio;

				// 에러인 경우, 에러 코드 & 로그 출력
				if( nErrorCode != E_ERROR_CODE_TRUE )
				{
					// Error Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Error Occured In Seq_StartAlign. ErrorCode : %d"), nErrorCode);
					pStParamAlign->dAdjTheta[0] = 0.0;
				}
				else
				{
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, 
						_T("Success Seq_StartAlign. PanelID : %s, Stage : %d, Theta : %lf, Cell Center X : %d, Y : %d"), 
						pStParamAlign->strPanelID, pStParamAlign->nStageNo, pStParamAlign->dAdjTheta[0], ptCellCenter.x, ptCellCenter.y);
				}
			}								
		}			
	}
	//SAFE_DELETE(pStParamAlign);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(3) %.2f"), tact.Stop(false)/1000.);

	return E_ERROR_CODE_TRUE;	// 물류 진행을 위해 Align 실패하더라도 무조건 TRUE Return
}

int WorkManager::Seq_WriteCCDIndex(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{	
	byte*		tempParam			= pParam;	

	// Receive Parameter ////////////////////////////////////////////////////////////////////////////////////////////////////
	TCHAR		strOrgImagePath[1000];												// 원본 이미지 경로
	memcpy(strOrgImagePath, tempParam, sizeof(TCHAR) * 1000);				tempParam += sizeof(TCHAR) * 1000;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_WriteCCDIndex Start. ImagePath : %s"), strOrgImagePath);

	char* pStr = NULL;
	pStr = CSTR2PCH(strOrgImagePath);  
	cv::Mat MatCCDImage = cv::imread(pStr, IMREAD_UNCHANGED);
	SAFE_DELETE_ARR(pStr);

	int nRet = theApp.CheckImageRatio(1, MatCCDImage.cols, MatCCDImage.rows, m_pSharedMemory[0]->GetImgWidth(), m_pSharedMemory[0]->GetImgHeight());
	if (nRet != 0)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, _T("Load CCD Image Ratio Incorrect (Path:%s) !!!"), strOrgImagePath);
		return APP_NG;
	}


	long nErrorCode = CCD_IndexSave(MatCCDImage, CCD_DEFECT_FILE_PATH, CCD_DEFECT_FILE_PATH2);

	// 에러인 경우, 에러 코드 & 로그 출력
	if( nErrorCode != 0 )
	{
		// Error Log
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Error Occured In Seq_WriteCCDIndex. ErrorCode : %d"), nErrorCode);
		return nErrorCode;
	}

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, FALSE, TRUE, _T("Seq_WriteCCDIndex End. (Save File Path : %s"), CCD_DEFECT_FILE_PATH);

	return APP_OK;
}

int WorkManager::VS_Send_Dust_Result_To_Seq(ENUM_INSPECT_MODE eInspMode, bool bNeedRetry, bool bIsNormalDust)
{
	// Main PC 는 Auto Mode 일때만 연동
	if (eInspMode != eAutoRun)
		return APP_OK;

	int nRet = APP_OK;
	int nParamSize = 0;
	byte bParam[1000] = {0,};
	byte* bpTemp = &bParam[0];

	*(bool *)bpTemp = bNeedRetry;				bpTemp += sizeof(bool);
	*(bool *)bpTemp = bIsNormalDust;			bpTemp += sizeof(bool);

	nParamSize = (int)(bpTemp - bParam);
	nRet = CmdEditSend(SEND_SEQ_DUST_RESULT, 0, (USHORT)(bpTemp - bParam), VS_SEQUENCE_TASK, bParam, CMD_TYPE_RES);
	if (nRet != APP_OK)
	{
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Send Dust Result Failed !!! result : %s (ErrorCode : %d)"), bIsNormalDust?_T("OK"):_T("NG"), nRet);
		return APP_NG;		
	}

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Send Dust Result. result : %s"), bIsNormalDust?_T("OK"):_T("NG"));

	return nRet;
}

//LJH 레시피 버전 전달 함수
int WorkManager::VS_Send_RcpVer_To_UI(CString ver)
{
	byte* pParam				= new byte[200];
	byte* pSendParam			= pParam;
	int			nRet			= APP_OK;

	RCP_VERSION* rcpVer = new RCP_VERSION;

	COPY_CSTR2TCH(rcpVer->Ver, ver, sizeof(rcpVer->Ver));		
	
	*(RCP_VERSION *)pSendParam = *rcpVer;

	pSendParam += sizeof(RCP_VERSION);

	SAFE_DELETE(rcpVer);

	EXCEPTION_TRY
		nRet = CmdEditSend(SEND_UI_RCPVER, 0, (ULONG)(pSendParam - pParam), VS_UI_TASK, pParam, CMD_TYPE_NORES);
	EXCEPTION_CATCH

		if (nRet != APP_OK)
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("VS_Send_RcpVer_To_UI Error Occured. RetVal=%d \n"), nRet);			
		}

		SAFE_DELETE_ARR(pParam);

		return nRet;
}

int WorkManager::VS_Send_InspEnd_To_UI(CString strPanelID)
{
	byte bParam[1000] = { 0, };
	byte* pParamTemp = bParam;
	_tcsncpy((TCHAR*)pParamTemp, (LPCTSTR)strPanelID, strPanelID.GetLength());
	pParamTemp += 100;

	int nRet = CmdEditSend(SEND_UI_INSPEND, 0, sizeof(bParam), VS_UI_TASK, bParam, CMD_TYPE_NORES);
	if (nRet != APP_OK) {
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Failed to Send Inspect End Msg (%s)"), strPanelID);
	}
	return nRet;
}

int WorkManager::Seq_StartAutoCal_Mask(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	// Receive ////////////////////////////////////////////////////////////////////////////////////////////////////// 필요한가?
	byte* pReceiveParam = pParam;
	byte* pReceiveParam_Origin = pParam;

	ST_LIGHTCAL_ALIGN_INSP_START* pStParamAlign = (ST_LIGHTCAL_ALIGN_INSP_START*)pReceiveParam;
	//memcpy(pStParamAlign, pReceiveParam, sizeof(PARAM_ALIGN_START));
	pReceiveParam += sizeof(ST_LIGHTCAL_ALIGN_INSP_START);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartAutoCal_Mask Start. StageNo:%d, PanelID:%s, nImageNo:%d"),
		pStParamAlign->nStageNo, pStParamAlign->PanCakeID, pStParamAlign->PatternNumber);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long		nErrorCode = E_ERROR_CODE_TRUE;



	double dAlignPara[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT] = { 0.0, };
	memcpy(dAlignPara, theApp.GetAlignParameter(0), sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT);

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// test
	CCPUTimer tact;
	tact.Start();

	CString strMsg = _T("");

	cv::Mat		MatOrgImage[MAX_LIGHTCAL_PARAM_SIZE];
	CString		strDrive = _T("");
	
	int			nWidth = 0;
	int			nHeight = 0;
	UINT32		nBufSize = 0;
	int			nBandWidth = 0;
	int			nBitRate = 0;

	nWidth = m_pSharedMemory[0]->GetImgWidth() *(pStParamAlign->nSeqMode[0] + 1);
	nHeight = m_pSharedMemory[0]->GetImgHeight() * (pStParamAlign->nSeqMode[0] + 1);
	nBandWidth = m_pSharedMemory[0]->GetImgBandWidth();
	nBitRate = m_pSharedMemory[0]->GetImgBitrate();

	cv::Mat matBack;
	cv::Mat matEmpty;
	cv::Mat matSide; // NP , 투명 라벨 제거용

	for (int idx = 0; idx < MAX_LIGHTCAL_PARAM_SIZE; ++idx)
	{
		if (pStParamAlign->Priority[idx] > 0)
		{
																	   // Log 세분화 작업. 180503 YSS
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check Start."));
			theApp.CheckDrive();
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check End."));

			strDrive = theApp.m_Config.GetINIDrive();


			if (nBitRate == 8)
			{
				if (nBandWidth == 1)
				{
					MatOrgImage[idx] = cv::Mat(nHeight, nWidth, CV_8UC1, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber[idx]));

					//////////////////////////////////////////////////////////////////////////
					if ((_tcscmp(pStParamAlign->PatternName[idx], L"BACK")) == 0) {
						MatOrgImage[idx].copyTo(matBack);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"EMPTY")) == 0) {
						MatOrgImage[idx].copyTo(matEmpty);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"SIDE")) == 0) {
						MatOrgImage[idx].copyTo(matSide);
					}
					//////////////////////////////////////////////////////////////////////////
				}
				else
				{
					MatOrgImage[idx] = cv::Mat(nHeight, nWidth, CV_8UC3, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber[idx]));

					//////////////////////////////////////////////////////////////////////////
					if ((_tcscmp(pStParamAlign->PatternName[idx], L"BACK")) == 0) {
						cv::cvtColor(MatOrgImage[idx], matBack, CV_RGB2GRAY);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"EMPTY")) ==0 ) {
						cv::cvtColor(MatOrgImage[idx], matEmpty, CV_RGB2GRAY);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"SIDE")) == 0) {
						cv::cvtColor(MatOrgImage[idx], matSide, CV_RGB2GRAY);
					}
					//////////////////////////////////////////////////////////////////////////
				}
			}
			else
			{
				if (nBandWidth == 1)
				{
					MatOrgImage[idx] = cv::Mat(nHeight, nWidth, CV_16UC1, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber[idx]));

					//////////////////////////////////////////////////////////////////////////
					if ((_tcscmp(pStParamAlign->PatternName[idx], L"BACK")) == 0) {
						MatOrgImage[idx].copyTo(matBack);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"EMPTY")) == 0) {
						MatOrgImage[idx].copyTo(matEmpty);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"SIDE")) == 0) {
						MatOrgImage[idx].copyTo(matSide);
					}
					//////////////////////////////////////////////////////////////////////////
				}
				else
				{
					MatOrgImage[idx] = cv::Mat(nHeight, nWidth, CV_16UC3, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber[idx]));

					//////////////////////////////////////////////////////////////////////////
					if ((_tcscmp(pStParamAlign->PatternName[idx], L"BACK")) == 0) {						
						MatOrgImage[idx].copyTo(matBack);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx],  L"EMPTY")) == 0) {						
						MatOrgImage[idx].copyTo(matEmpty);
					}
					else if ((_tcscmp(pStParamAlign->PatternName[idx], L"SIDE")) == 0) {					
						MatOrgImage[idx].copyTo(matSide);
					}
					//////////////////////////////////////////////////////////////////////////
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//받을 파라미터
	CString PanCakeID = pStParamAlign->PanCakeID;
	CString ColorCode = pStParamAlign->ColorCode;

	int nJudge_T = pStParamAlign->Transparent;
	CString T_Judge_Code = _T(""); //투명인지 아닌지 판단

	if (nJudge_T > 0) {
		T_Judge_Code = _T("T");
	}
	else {
		T_Judge_Code = _T("N");
	}

	bool Judge_OKNG = false;
	//////////////////////////////////////////////////////////////////////////
	CString Path_Img = _T("E:\\AutoCal");
	CString Path_Mask_Result = _T("");
	Path_Mask_Result.Format(_T("%s\\%s"),Path_Img, PanCakeID);

	CreateDirectory(Path_Img, NULL); //LCP 중간이미지
	CreateDirectory(Path_Mask_Result, NULL); //LCP 중간이미지
	//////////////////////////////////////////////////////////////////////////


					 /// 영상은 공유메모리에서 읽어들어옴 , 공유메모리 사용 제한될 시 불가피하게 영상 저장 후 읽는 것으로... ///

	// Image Load
	if (ColorCode.MakeUpper() != "NP" && T_Judge_Code.MakeUpper() != "T") {
		
		if (matBack.empty() || matEmpty.empty()) {

			strMsg.Format(_T("Start Auto Cal Mask Image Load Fail!!!! \r\n\t"));
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

			//알람은 일단 보류
			CString strAlarmMsg;
			strAlarmMsg.Format(_T("자동 조명에 사용 할 이미지를 불러올수 없습니다."));
			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eAutoRun, _T("Heavy"), _T("Auto Cal Make Mask Non Buffer"), strAlarmMsg, eALARMID_LCP_AUTOCAL_EMPTY_BUFFER, eALARMTYPE_HEAVY);

			nErrorCode = E_ERROR_CODE_EMPTY_BUFFER;

			return nErrorCode;
		}
		else {
			matSide = cv::Mat::zeros(matBack.size(), CV_8U); // 투명 NP와 같은 함수 탈때 예외 대비
		}
	}
	else { // Image Load

		if (matBack.empty() || matEmpty.empty() || matSide.empty()) {

			strMsg.Format(_T("Start Auto Cal Mask Image Load Fail!!!! \r\n\t"));
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

			//알람은 일단 보류
			CString strAlarmMsg;
			strAlarmMsg.Format(_T("자동 조명에 사용 할 이미지를 불러올수 없습니다."));
			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eAutoRun, _T("Heavy"), _T("Auto Cal Make Mask Non Buffer"), strAlarmMsg, eALARMID_LCP_AUTOCAL_EMPTY_BUFFER, eALARMTYPE_HEAVY);

			nErrorCode = E_ERROR_CODE_EMPTY_BUFFER;

			return nErrorCode;
		}
	}


	strMsg.Format(_T("Start Auto Cal Mask Image Load OK \r\n\t"));
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));
	//////////////////////////////////////////////////////////////////////////
	// Make Mask Start
	strMsg.Format(_T("Start Auto Cal Make Mask Start!! \r\n\t"));
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

	nErrorCode = Align_AutoCal_Make_Mask(matBack, matEmpty, matSide, ColorCode, T_Judge_Code, Path_Mask_Result, dAlignPara, nEqpType);

	if (nErrorCode != E_ERROR_CODE_TRUE) {

		strMsg.Format(_T("Start Auto Cal Make Mask Fail!! \r\n\t"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

		//알람은 일단 보류
		CString strAlarmMsg;
		strAlarmMsg.Format(_T("자동 조명 시료 마스크 만들기 실패"));
		theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eAutoRun, _T("Heavy"), _T("Auto Cal Make Mask Fail"), strAlarmMsg, eALARMID_LCP_AUTOCAL_MASK_FAIL, eALARMTYPE_HEAVY);
		

		return nErrorCode;
	}
	else {
		strMsg.Format(_T("Start Auto Cal Mask Image Make Mask OK!! \r\n\t"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

		Judge_OKNG = true;
	}
	//////////////////////////////////////////////////////////////////////////

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(3) %.2f"), tact.Stop(false) / 1000.);

	// 판정 회신
	//*(BOOL*)pParam = AlignTheta;
	pParam += sizeof(BOOL);
	*(BOOL*)pParam = Judge_OKNG;
	//ok ng 판정 넣어줘야함
	return E_ERROR_CODE_TRUE;
}

int WorkManager::Seq_StartAutoCal(byte* pParam, ULONG& nPrmSize, bool bAlwaysRunMode, bool bBusyCheck, bool bSeqResetPossible)
{
	// Receive ////////////////////////////////////////////////////////////////////////////////////////////////////// 필요한가?
	byte* pReceiveParam = pParam;
	byte* pReceiveParam_OriginPos = pParam;
	ST_LIGHTCAL_AUTOCAL_INSP_START* pStParamAlign = (ST_LIGHTCAL_AUTOCAL_INSP_START*)pReceiveParam;
	//memcpy(pStParamAlign, pReceiveParam, sizeof(PARAM_ALIGN_START));
	pReceiveParam += sizeof(ST_LIGHTCAL_AUTOCAL_INSP_START);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Seq_StartAutoCal_Mask Start. StageNo:%d, PanelID:%s, nImageNo:%d"),
		pStParamAlign->nStageNo, pStParamAlign->PanCakeID, pStParamAlign->PatternNumber);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	long		nErrorCode = E_ERROR_CODE_TRUE;

	double dAlignPara[MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT] = { 0.0, };
	memcpy(dAlignPara, theApp.GetAlignParameter(0), sizeof(double) * MAX_MEM_SIZE_ALIGN_PARA_TOTAL_COUNT);

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// test
	CCPUTimer tact;
	tact.Start();

	CString strMsg = _T("");

	/// 영상받기

	cv::Mat		MatOrgImage;
	CString		strDrive = _T("");

	int			nWidth = 0;
	int			nHeight = 0;
	UINT32		nBufSize = 0;
	int			nBandWidth = 0;
	int			nBitRate = 0;

	nWidth = m_pSharedMemory[0]->GetImgWidth() * (pStParamAlign->nSeqMode[0] + 1);
	nHeight = m_pSharedMemory[0]->GetImgHeight() * (pStParamAlign->nSeqMode[0] + 1);
	nBandWidth = m_pSharedMemory[0]->GetImgBandWidth();
	nBitRate = m_pSharedMemory[0]->GetImgBitrate();


	cv::Mat matSrc; 
	cv::Mat matAcitveMask;
	cv::Mat matLabelMask;

	if (pStParamAlign->Priority > 0)
	{
		
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check Start."));
		theApp.CheckDrive();
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, FALSE, TRUE, _T("Drive check End."));

		strDrive = theApp.m_Config.GetINIDrive();


		if (nBitRate == 8)
		{
			if (nBandWidth == 1)
			{
				MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC1, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber));

				MatOrgImage.copyTo(matSrc);
			}
			else
			{
				MatOrgImage = cv::Mat(nHeight, nWidth, CV_8UC3, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber));
				
					cv::cvtColor(MatOrgImage, matSrc, CV_RGB2GRAY);
			}
		}
		else
		{
			if (nBandWidth == 1)
			{
				MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC1, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber));
			}
			else
			{
				MatOrgImage = cv::Mat(nHeight, nWidth, CV_16UC3, m_pSharedMemory[0]->GetImgAddress(pStParamAlign->PatternNumber));
			}
		}
	}
//	}

	//////////////////////////////////////////////////////////////////////////
	//받을 파라미터
	CString PanCakeID = pStParamAlign->PanCakeID;
	CString ColorCode = pStParamAlign->ColorCode;
	CString PattenName = pStParamAlign->PatternName;

	int nJudge_T = pStParamAlign->Transparent;
	CString T_Judge_Code = _T(""); //투명인지 아닌지 판단

	if (nJudge_T > 0) {
		T_Judge_Code = _T("T");
	}
	else {
		T_Judge_Code = _T("N");
	}
	
	int nPre_Light_Value = pStParamAlign->Pre_Light_Value; // 전에 사용한 조명 값
	double dPre_Mean_Gv = pStParamAlign->Pre_Mean_GV; // 전에 사용한 조명 값에 대한 시료 평균 Gv
	int nMin_Gv = pStParamAlign->MinRange; //시료 평균 밝기 범위 최소
	int nMax_Gv = pStParamAlign->MaxRange; //시료 평균 밝기 범위 최대
	int nReTry_Count = pStParamAlign->RetryCount; // 함수 ReTry 횟수
	int nCurrent_Light_Value = pStParamAlign->Cur_Light_Value;	// 현재 사용한 조명 값

	
	double dCurrent_Mean_Gv = 0.0; // 현재 사용한 조명 값에 대한 시료 평균 Gv
	bool Judge_OK_NG = false;
	int nLight_Offset = 0;
	bool Judge_Up_Down = false;
	//////////////////////////////////////////////////////////////////////////
	CString Path_Img = _T("E:\\AutoCal");
	CString Path_Mask_Result = _T("");
	Path_Mask_Result.Format(_T("%s\\%s"), Path_Img, PanCakeID);

	//////////////////////////////////////////////////////////////////////////


	/// 영상은 공유메모리에서 읽어들어옴 , 공유메모리 사용 제한될 시 불가피하게 영상 저장 후 읽는 것으로... ///

	//////////////////////////////////////////////////////////////////////////

	MatOrgImage.copyTo(matSrc);

	CString Path_Active;
	Path_Active.Format(_T("%s\\Active_Mask.bmp"), Path_Mask_Result);

	matAcitveMask = cv::imread((cv::String)(CStringA)Path_Active, IMREAD_GRAYSCALE);

	CString Path_Label;
	Path_Label.Format(_T("%s\\Label_Mask.bmp"), Path_Mask_Result);

	matLabelMask = cv::imread((cv::String)(CStringA)Path_Label, IMREAD_GRAYSCALE);

	if (matSrc.empty() || matAcitveMask.empty() || matLabelMask.empty()) {

		strMsg.Format(_T("Start Auto Cal Mask Image Load Fail!!!! \r\n\t"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

		//알람은 일단 보류
		CString strAlarmMsg;
		strAlarmMsg.Format(_T("자동 조명에 사용 할 이미지를 불러올수 없습니다."));
		theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eAutoRun, _T("Heavy"), _T("Auto Cal Make Mask Non Buffer"), strAlarmMsg, eALARMID_LCP_AUTOCAL_EMPTY_BUFFER, eALARMTYPE_HEAVY);

		nErrorCode = E_ERROR_CODE_EMPTY_BUFFER;

		return nErrorCode;
	}

	nErrorCode = Align_AutoCal(matSrc, matAcitveMask , matLabelMask, PattenName ,ColorCode, T_Judge_Code, Path_Mask_Result, nMin_Gv, nMax_Gv, dCurrent_Mean_Gv, Judge_OK_NG, dAlignPara, nEqpType);
	//////////////////////////////////////////////////////////////////////////
	

	if (nErrorCode != E_ERROR_CODE_TRUE) {

		strMsg.Format(_T("Start Auto Cal Fail!! \r\n\t"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));

		//알람은 일단 보류
		CString strAlarmMsg;
		strAlarmMsg.Format(_T("자동 조명 검사 실패"));
		theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eAutoRun, _T("Heavy"), _T("Auto Cal Fail"), strAlarmMsg, eALARMID_LCP_AUTOCAL_INSPECT_FAIL, eALARMTYPE_HEAVY);

		return nErrorCode;
	}
	else {

		if (Judge_OK_NG == false) {

			if (nReTry_Count > 1) {
				if (dCurrent_Mean_Gv < nMin_Gv)  Judge_Up_Down = true;
				else if (dCurrent_Mean_Gv > nMax_Gv)  Judge_Up_Down = false;


			}
		}


		strMsg.Format(_T("Start Auto Cal OK!! \r\n\t"));
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, TRUE, strMsg.GetBuffer(0));
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("\t\t(3) %.2f"), tact.Stop(false) / 1000.);

	// 판정 회신
	// 데이터 담아야 함
	pReceiveParam = pReceiveParam_OriginPos;

	ST_LIGHTCAL_INSP_RESULT* pStParam_Lightcal_Result = new ST_LIGHTCAL_INSP_RESULT();
	pStParam_Lightcal_Result->BrightnessAverage = dCurrent_Mean_Gv;
	pStParam_Lightcal_Result->Judge = Judge_OK_NG;
	pStParam_Lightcal_Result->Offset = nLight_Offset;
	pStParam_Lightcal_Result->LightValueJudge = Judge_Up_Down;
	memcpy(pStParam_Lightcal_Result->PanCakeID, PanCakeID, sizeof(pStParam_Lightcal_Result->PanCakeID));

	*(ST_LIGHTCAL_INSP_RESULT*)pReceiveParam = *pStParam_Lightcal_Result;

	return E_ERROR_CODE_TRUE;
}