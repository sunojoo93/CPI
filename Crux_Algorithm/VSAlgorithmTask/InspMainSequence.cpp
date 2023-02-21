#include "stdafx.h"
#include <ppl.h>
#include "InspMainSequence.h"

// Image Save ���� ���
#include "ImageSave.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace Concurrency;

InspMainSequence::InspMainSequence(void)
{
}

InspMainSequence::~InspMainSequence(void)
{
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	OnStartGrab
//	 2. �Լ���� :	�˻� ������ (Grab -> Inspection -> Result)
//	 3. ��    �� :	lParam : �˻� ����  E_DIR_FORWARD or E_DIR_BACK
//					
//					
//					
//					
//	 4. �� ȯ �� :	�Ϸ� : 0
//					���� : 0 �̿�
//	 5. �� �� �� :	2015/02/10
//	 6. �� �� �� :	KYT
//	 7. ������� :	
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
LRESULT InspMainSequence::OnStartInspection( WPARAM wParam, LPARAM lParam ) //2016.11.04 sgkim
{
	m_bBusy = true;	

	int nRet		= 0;
	
	tInspectThreadParam*	pInspectThreadParam =	(tInspectThreadParam*)	wParam;		// ���� ����
	STRU_IMAGE_INFO*		pStImageInfo =			(STRU_IMAGE_INFO*)		lParam;		// �̹����� ����

	// �˻� - ���� - ��� ���� �Լ� ȣ��
	nRet = m_fnMainSequence(theApp.GetCurModelID(), pStImageInfo->strPanelID, pStImageInfo->strLotID, pInspectThreadParam->strImagePath_LCP_Bubble ,pInspectThreadParam->strSaveDrive,
		pInspectThreadParam->stCamAlignInfo, pInspectThreadParam->WrtResultInfo, pInspectThreadParam->ResultPanelData,
		pInspectThreadParam->MatOrgImage, pInspectThreadParam->MatColorImage ,pInspectThreadParam->MatDraw ,pInspectThreadParam->MatResult, pInspectThreadParam->tCHoleAlignData,
		pInspectThreadParam->bInspectEnd, pInspectThreadParam->bStAlignEnd,pInspectThreadParam->bStBackAlignEnd, pInspectThreadParam->bStVacuumAlignEnd, pInspectThreadParam->bStLabelMaskMakeEnd,pInspectThreadParam->bStStageDustEnd, pInspectThreadParam->nError_Thread, pInspectThreadParam->nError_Image_Num, pInspectThreadParam->bAlign_Success,
		pInspectThreadParam->bAlignEnd ,pInspectThreadParam->bInspectAlgEnd,pInspectThreadParam->bChkDustEnd, pInspectThreadParam->bIsNormalDust, pInspectThreadParam->bUseDustRetry, pInspectThreadParam->nDustRetryCnt, pInspectThreadParam->bUseInspect, pInspectThreadParam->bHeavyAlarm,
		pInspectThreadParam->eInspMode, pStImageInfo );
	
	if (pStImageInfo->nImageNo == 0 && pStImageInfo->nCameraNo == 0)
		//SAFE_DELETE(pInspectThreadParam);
		pInspectThreadParam->clear();	// yskim 20.07.03

	SAFE_DELETE(pStImageInfo);

	Sleep(30);

	m_bBusy = false;

	return 0;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	OnStartSaveImage
//	 6. �� �� �� :	yskim
//   7. ��    �� :  ���� ���� ����
//----------------------------------------------------------------------------------------------------
LRESULT InspMainSequence::OnStartSaveImage(WPARAM wParam, LPARAM lParam) //2016.11.04 sgkim
{
	m_bBusy = true;

	cv::Mat*	MatOrgImage = (cv::Mat*)wParam;
	CString*	strImagePath = (CString*)lParam;


	ImageSave(*MatOrgImage, (TCHAR*)(LPCTSTR)*strImagePath);

	m_bBusy = false;

	return 0;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	WriteResultData
//	 2. �Լ���� :	������ �����ϴ� ��� ������ �ۼ��Ѵ�.
//	 3. ��    �� :	
//					
//					
//					
//					
//	 4. �� ȯ �� :	TRUE	: Cell �� ����
//					FALSE	: Cell �� ����
//	 5. �� �� �� :	2015/02/10
//	 6. �� �� �� :	KYT
//	 7. ������� :	
//	 8. ��    �� :	Vision Server ���� ������ �޾ƾ��Ѵ�.
//----------------------------------------------------------------------------------------------------
int InspMainSequence::WriteResultData(const CString strPanelID, const CString strDrive,
								cv::Mat MatResult[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT],
								CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, ENUM_INSPECT_MODE eInspMode)
{
	int					nRet = 0;

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("WriteResultData Start. PanelID : %s"), strPanelID);

// 2018.01.04 Judgement( ) �� �̵�
// 	NumberingDefect(_T("2"), strPanelID, _T("TEST"), WrtResultInfo, resultPanelData, 1);
// 
// 	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Numbering Defect End. Panel ID : %s"), strPanelID);

//  choi �������

// 	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
// 	M = getRotationMatrix2D(m_stThrdAlignInfo.ptAlignCenter,m_stThrdAlignInfo.dAlignTheta, 1.0);
// 	cv::warpAffine(MatDrawBuffer[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][0], MatDrawBuffer[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][0], M, MatDrawBuffer[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][0].size()); // ȸ��

	// ��� �̹��� �׸���
	DrawDefectImage(strPanelID, MatResult, MatDrawBuffer, resultPanelData);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Draw Defect Image End. Panel ID : %s"), strPanelID);

	SaveCropImage(strPanelID, strDrive, MatResult, MatDrawBuffer, resultPanelData, eInspMode);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Save Crop Image End. Panel ID : %s"), strPanelID);

	CCPUTimer timerSaveImage;
	timerSaveImage.Start();

	// �ʿ��Ѱ�? ( �ð� �ʹ� �ɸ�... )
	// �ʿ��ϸ�, �޴���� ������ Ȯ�� ( ���� �������� ����ǰ� ���� )
	// flag ó���� �ʿ��� ����...
/************************************************************************	
//#pragma omp parallel for
#pragma omp parallel for schedule(dynamic)
	// Save Result Image
	for (int nImageNum = 0; nImageNum < theApp.GetGrabStepCount(); nImageNum++)
	{
		if(theApp.m_pGrab_Step[nImageNum].bUse)
		{
//#pragma omp parallel for			// ���� Camera ������ 1~2���̹Ƿ� ��ȿ����
			for (int nCameraNum = 0; nCameraNum < MAX_CAMERA_COUNT; nCameraNum++)
			{
				if (theApp.m_pGrab_Step[nImageNum].stInfoCam[nCameraNum].bUse)
				{
					ImageSave(MatDrawBuffer[nImageNum][nCameraNum], _T("%s\\%s\\%d_%s_CAM%02d.jpg"), INSP_INFO_FILE_PATH, strPanelID, nImageNum, theApp.GetGrabStepName(nImageNum), nCameraNum);
				}
			}
		}
	}
************************************************************************/

	timerSaveImage.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Save Result Image tact time %.2f"), timerSaveImage.GetTime()/1000.);

	// ��� ���� ����
// ���� �ð� ���� - ������ ��û
// 	SYSTEMTIME time;
// 	::GetLocalTime(&time);
 	
// 	CString strTime;
// 	strTime.Format(_T("%02d%02d%02d_%02d%02d%02d"), time.wYear, time.wMonth, time.wDay,time.wHour,time.wMinute,time.wSecond);

	CString strMainPCFileName;
	//strMainPCFileName.Format(_T("D:\\%s_%02d\\%s_%s.txt"), theApp.m_Config.GetEqpTypeName(), theApp.m_Config.GetPCNum(), strPanelID, theApp.m_Config.GetEqpTypeName());

	CString strFinalResultDir, strFinalResultFileName;
	strFinalResultDir.Format(_T("%s:\\%s_%02d\\"), theApp.m_Config.GETDRV(), _T("FinalResult"), theApp.m_Config.GetPCNum());
	strFinalResultFileName.Format(_T("%s\\%s_%s_F.txt"), strFinalResultDir, strPanelID, theApp.m_Config.GetEqpTypeName());
	strMainPCFileName.Format(_T("%s\\%s_%s.txt"), strFinalResultDir, strPanelID, theApp.m_Config.GetEqpTypeName());

	CString strServerFileName;
	strServerFileName.Format(_T("%s\\%s\\%s.txt"), RESULT_PATH, strPanelID, strPanelID);
		
	//	������
	//	��������
	CreateDirectory(strFinalResultDir, NULL);
	

	// Merge Tool flag�� ��� ���� �� �˻� PC���� ������ ��� ���� ���⵵�� ���� ���� 180712 YSS
	nRet = WrtResultInfo.WriteResultPanelData_ToMainPC(strMainPCFileName, resultPanelData);//2016.12.29 sgkim H-Project
	CopyFile(strMainPCFileName, strFinalResultFileName, FALSE);

	//	������
	//	MergeTool ���� �ӽ� ó��
	// 180501 YSS - MergeTool ���� ����(AVI, SVI���� MergeTool ��� ����� ���, Alg �׽�ũ���� ���� ���� �� ��. MergeTool ���α׷����� ������ ���� ����)
// 	if(theApp.m_Config.GetEqpType() == EQP_AVI || theApp.m_Config.GetEqpType() == EQP_SVI)
// 	{
// 		if( !theApp.GetMergeToolUse())
// 		{
// 			nRet = WrtResultInfo.WriteResultPanelData_ToMainPC(strMainPCFileName, resultPanelData);//2016.12.29 sgkim H-Project
// 			CopyFile(strMainPCFileName, strFinalResultFileName, FALSE);
// 		}
// 	}
// 	else
// 	{
// 		nRet = WrtResultInfo.WriteResultPanelData_ToMainPC(strMainPCFileName, resultPanelData);//2016.12.29 sgkim H-Project
// 		CopyFile(strMainPCFileName, strFinalResultFileName, FALSE);
// 	}
	
	CString strTempPath;
	strTempPath.Format(_T("%s\\%s\\%s_F.txt"),RESULT_PATH, strPanelID );
	CopyFile(strMainPCFileName, strTempPath, FALSE);
	CFileFind find;
	BOOL bRtn = find.FindFile(strFinalResultFileName);
	if(bRtn)
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Created Final Result. PanelID : %s, Path : %s"), strPanelID,strFinalResultFileName);
	else
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Created Final Result Fail!!! PanelID : %s, Path : %s"), strPanelID,strFinalResultFileName);
	nRet = WrtResultInfo.WriteResultPanelData(strServerFileName, resultPanelData);


	SYSTEMTIME time;
	::GetLocalTime(&time);
	// Panel �� Defect Type ����
	if (GetPrivateProfileInt(_T("DEFECT"), _T("Use_Panel_Trend"), 1, INIT_FILE_PATH) == 1)
	{
		CString strPanelTrendFileName = _T("");
		strPanelTrendFileName.Format(_T("%s\\%02d%02d%02d_Trend.txt"), RESULT_PATH, time.wYear, time.wMonth, time.wDay);
		nRet = WrtResultInfo.WritePanelTrend(strPanelTrendFileName, resultPanelData.m_nDefectTrend, strPanelID, resultPanelData.m_ResultPanel.Judge);
	}

	CString strFinalDefectPath = _T("");
	strFinalDefectPath.Format(_T("%s\\%02d%02d%02d_FinalDefect.txt"),RESULT_PATH, time.wYear, time.wMonth, time.wDay);
	nRet = WrtResultInfo.WriteFinalDefect(strFinalDefectPath, resultPanelData.m_ResultPanel.nFinalDefectNum, strPanelID);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("WriteResultData End. PanelID : %s"), strPanelID);

	//	������
	//Merge_Tool Panel txt copy 
	//if(eInspMode == eAutoRun && theApp.GetMergeToolUse() )
	{
		CString strContourFileName;
		// ������Ʈ ���� ����̺� �߰�
		strContourFileName.Format(_T("%s:\\MergeTool\\%s\\%s.txt"), theApp.m_Config.GETDRV(), strPanelID, strPanelID);
		BOOL bRet = CopyFile(strServerFileName, strContourFileName, FALSE);
		if(bRet)
			theApp.WriteLog(eLOGTACT, eLOGLEVEL_SIMPLE, FALSE, FALSE, _T("Merge - Copy result.txt file. PanelID : %s, Path : %s"),strPanelID,strContourFileName);

	}

	return nRet;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	InspectImage
//	 2. �Լ���� :	
//	 3. ��    �� :	
//	 4. �� ȯ �� :	�Ϸ� : 0
//					���� : 0 �̿�
//	 5. �� �� �� :	2015/02/10
//	 6. �� �� �� :	KYT
//	 7. ������� :	
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
int InspMainSequence::InspectImage(const CString strModelID, const CString strPanelID, const CString strDrive, const CString strLcp_Bubble_Path,
	cv::Mat MatOriginImg[][MAX_CAMERA_COUNT], cv::Mat& MatColorBuffer,cv::Mat& MatDrawBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], tCHoleAlignInfo& tCHoleAlignData,
								ResultBlob_Total* pResultBlob_Total, const int nImageNum, const int nCameraNum, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	long nErrorCode = E_ERROR_CODE_TRUE;

	ENUM_KIND_OF_LOG eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum;

	theApp.WriteLog(eLogCamera, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Inspect Start. ModelID:%s, PanelID:%s, GrabStep:%s, CameraNum:%d"), strModelID, strPanelID, theApp.GetGrabStepName(nImageNum), nCameraNum);

	// CHole Align Data
	m_stThrdAlignInfo.tCHoleAlignData = &tCHoleAlignData;

	// �ش� �̹����� ���� Logic ���� (�ϳ��� ���� / �ϳ��� ī�޶� ���� ��� �˻� ����)
	nErrorCode = StartLogic(strPanelID, strDrive, strLcp_Bubble_Path ,m_stThrdAlignInfo, MatOriginImg, MatColorBuffer ,MatDrawBuffer, MatResultImg,
						pResultBlob_Total, nImageNum, nCameraNum, m_nThrdID, bpInspectEnd, bpInspectAlgEnd ,nRatio, eInspMode, WrtResultInfo, _mtp);	

	// ������ ���, ���� �ڵ� & �α� ���
	// KSW 18.05.21 - ���� ������ ���������� ���� ��� ��ȯ�� : E_ERROR_CODE_ALIGN_ROUND_SETTING
	if( nErrorCode != E_ERROR_CODE_TRUE						&&
		nErrorCode != E_ERROR_CODE_ALIGN_ROUND_SETTING		&&
		nErrorCode != E_ERROR_CODE_EMPTY_RGB				)	// KSW 18.05.30
	{
		// KSW 18.04.03 - Err�� ���, �˻� ���� ���� ����������... E�� ����
 		JudgeADDefect(nImageNum, nCameraNum, m_stThrdAlignInfo.nStageNo, MatOriginImg[nImageNum][nCameraNum].cols, MatOriginImg[nImageNum][nCameraNum].rows, pResultBlob_Total, E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL, eInspMode);

		theApp.WriteLog(eLogCamera, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Inspect Fail. ModelID:%s, PanelID:%s, GrabStep:%s, CameraNum:%d"), strModelID, strPanelID, theApp.GetGrabStepName(nImageNum), nCameraNum);
	}

	return E_ERROR_CODE_TRUE;
}

BOOL InspMainSequence::m_fnMainSequence(CString strModelID, CString strPanelID, CString strLotID, CString strLcp_Bubble_Path, TCHAR* strSaveDrive,
													tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], CWriteResultInfo& WrtResultInfo, ResultPanelData& ResultPanelData,
	cv::Mat MatOriginImg[][MAX_CAMERA_COUNT], cv::Mat MatColorBuffer[][MAX_CAMERA_COUNT] ,cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], tCHoleAlignInfo& tCHoleAlignData,
													bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bStAlignEnd[MAX_CAMERA_COUNT], bool bStBackAlignEnd[MAX_CAMERA_COUNT], bool bStVacuumAlignEnd[MAX_CAMERA_COUNT], bool bStLabelMaskMakeEnd[MAX_CAMERA_COUNT], bool bStStageDustEnd[MAX_CAMERA_COUNT],  long nError_Thread[MAX_CAMERA_COUNT], int nError_Image_Num[MAX_CAMERA_COUNT], bool bStSuccess[MAX_CAMERA_COUNT],
	bool bAlignEnd[MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER],bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool bUseInspect, bool& bIsHeavyAlarm,
													ENUM_INSPECT_MODE eInspMode,STRU_IMAGE_INFO* pStImageInfo )
{
	int	nCameraNum				= pStImageInfo->nCameraNo;
	int	nImageNum				= pStImageInfo->nImageNo;
	int nRatio					= pStImageInfo->nRatio;		// Pixel Shift ��� ���ο� ���� �̹����� ����
	int nStageNo				= pStImageInfo->nStageNo;
	
// E�� ������ ��� �ʹ� ���� ������ 2�� ��� 190314 YWS
	bool nSkipFlag				= false;

	m_stThrdAlignInfo.nStageNo	= nStageNo;

	CString strDrive = _T("");
	strDrive.Format(_T("%s"),strSaveDrive);
	ENUM_KIND_OF_LOG eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum;	// for Camera Log


	if(strPanelID != _T(""))
	{
		// ��� �׸� �̹��� ���� �Ҵ�
		// �÷��� ��� ( SVI )
		if( MatOriginImg[nImageNum][nCameraNum].channels() != 1 )
		{
			MatDrawBuffer[nImageNum][nCameraNum] = MatOriginImg[nImageNum][nCameraNum].clone();
			MatColorBuffer[nImageNum][nCameraNum] = MatOriginImg[nImageNum][nCameraNum].clone();
			cv::cvtColor(MatOriginImg[nImageNum][nCameraNum], MatOriginImg[nImageNum][nCameraNum], CV_RGB2GRAY);
			
		}
		// ����� ��� ( AVI, APP )
		else
		{
			// 12bit �̹����� ��� 8bit�� ��ȯ �� Color ����
			if (MatOriginImg[nImageNum][nCameraNum].type() != CV_8U)
			{
				cv::Mat matTemp;
				MatOriginImg[nImageNum][nCameraNum].convertTo(matTemp, CV_8U, 1./16.);
				cv::cvtColor(matTemp, MatDrawBuffer[nImageNum][nCameraNum], COLOR_GRAY2RGB);
				cv::cvtColor(matTemp, MatColorBuffer[nImageNum][nCameraNum], COLOR_GRAY2RGB);

				matTemp.release();
			}
			else
			{
				cv::cvtColor(MatOriginImg[nImageNum][nCameraNum], MatDrawBuffer[nImageNum][nCameraNum], COLOR_GRAY2RGB);	//�޸� �Ҵ� Pattern �� ��ü����
				cv::cvtColor(MatOriginImg[nImageNum][nCameraNum], MatColorBuffer[nImageNum][nCameraNum], COLOR_GRAY2RGB);	//�޸� �Ҵ� Pattern �� ��ü����
			}
		}

		if (bUseInspect)
		{

			ResultBlob_Total* pResultBlobTotal = new ResultBlob_Total();

			pResultBlobTotal->RemoveAll_ResultBlob();
			pResultBlobTotal->SetModelID(strModelID);
			pResultBlobTotal->SetPanelID(strPanelID);

			long nErrorCode = CheckImageIsNormal_LCP(strPanelID, strDrive, MatOriginImg[nImageNum][nCameraNum], MatColorBuffer[nImageNum][nCameraNum], MatDrawBuffer[nImageNum][nCameraNum], nRatio, nImageNum, nCameraNum, nStageNo, stCamAlignInfo,
				pResultBlobTotal, WrtResultInfo.GetCamResolution(0), WrtResultInfo.GetPanelSizeX(), WrtResultInfo.GetPanelSizeY(), bStAlignEnd, bStBackAlignEnd,bStVacuumAlignEnd ,bStLabelMaskMakeEnd, bStStageDustEnd, nError_Thread , nError_Image_Num , bStSuccess,
				bAlignEnd ,bChkDustEnd, bIsNormalDust, bUseDustRetry, nDustRetryCnt, bIsHeavyAlarm, eInspMode);
			

			// ���� ����� �̹����� ���������� ����� ���� ���� �˻� ������ ����
			if (bStSuccess[nCameraNum] == true && bIsHeavyAlarm == false && nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE)
			{

				// �˰��� ����
				InspectImage(strModelID, strPanelID, strDrive, strLcp_Bubble_Path ,MatOriginImg, MatColorBuffer[nImageNum][nCameraNum], MatDrawBuffer[nImageNum][nCameraNum], MatResultImg, tCHoleAlignData,
					pResultBlobTotal, nImageNum, nCameraNum, bpInspectEnd, bpInspectAlgEnd ,nRatio, eInspMode, WrtResultInfo, pStImageInfo->dPatternCIE);
				theApp.WriteLog(eLogCamera, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Inspect End. ModelID:%s, PanelID:%s, GrabStep:%s, CameraNum:%d"), strModelID, strPanelID, theApp.GetGrabStepName(nImageNum), nCameraNum);
			}
			else
			{
				theApp.WriteLog(eLogCamera, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Inspect Skip !!!. ModelID:%s, PanelID:%s, GrabStep:%s, CameraNum:%d"), strModelID, strPanelID, theApp.GetGrabStepName(nImageNum), nCameraNum);
				// E�� ������ ��� �ʹ� ���� ������ 2�� ��� 190314 YWS
				nSkipFlag = true;
			}

			// �˻� ��� ���� - ���� �� ��� -> �г� ���
			ConsolidateResult(strPanelID, strDrive, WrtResultInfo, pResultBlobTotal, ResultPanelData, nImageNum, nCameraNum, nRatio, eInspMode);

			SAFE_DELETE(pResultBlobTotal);
		}

		bpInspectEnd[nImageNum][nCameraNum] = true;

		// ��� ī�޶� ��� ������ �˻� ���� ���
		for (int i=0; i<MAX_GRAB_STEP_COUNT; i++)
		{
			for (int k=0; k<MAX_CAMERA_COUNT; k++)
			{
				while(!bpInspectEnd[i][k])
				{
					Sleep(10);
				}
			}
		}



		// ���� �����忡�� ���� �� ����� ����
		if (nImageNum == 0 && nCameraNum == 0)
		{
			if (bUseInspect && !bIsHeavyAlarm)
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Inspect Complete. ModelID : %s, PanelID : %s"), strModelID, strPanelID);
			else
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Inspect Skip - Complete!!!. ModelID : %s, PanelID : %s"), strModelID, strPanelID);

			if (!bIsHeavyAlarm)		// �߾˶� �߻� �� �ƹ��͵� ����
			{
				//18.09.18 Judgement Virture �Լ� ����(App Stage

				Judgement(WrtResultInfo, ResultPanelData, MatDrawBuffer, tCHoleAlignData, strModelID, strLotID, strPanelID, strDrive, nRatio, nCameraNum, eInspMode, MatOriginImg, MatColorBuffer, bUseInspect, nStageNo);

				WriteResultData(strPanelID, strDrive, MatResultImg, MatDrawBuffer, WrtResultInfo, ResultPanelData, eInspMode);

				// E�� ������ ��� �ʹ� ���� ������ 2�� ��� 190314 YWS
				if (nSkipFlag == true) Sleep(2000);


				if (eInspMode == eAutoRun) 
				{
					theApp.m_AlgorithmTask->VS_Send_InspEnd_To_UI(strPanelID);
				}
				else
				{
					theApp.m_AlgorithmTask->VS_Send_ClassifyEnd_To_Seq(strPanelID, strDrive, (UINT)ResultPanelData.m_ListDefectInfo.GetCount(),
						ResultPanelData.m_ResultPanel.Judge, ResultPanelData.m_ResultPanel.judge_code_1, ResultPanelData.m_ResultPanel.judge_code_2);
				}
			}
			else {
				if (eInspMode == eAutoRun)
				{
					theApp.m_AlgorithmTask->VS_Send_InspEnd_To_UI(strPanelID);
				}
			}
		}		
	}	

	return TRUE;
}

void InspMainSequence::SaveCropImage(CString strPanelID, CString strDrive, cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData, ENUM_INSPECT_MODE eInspMode)
{
	// GUI �� �ʿ��� ���� ũ�� - �������� �� �� 
	// - 2018.04.24 Width�� ���� ũ��, Height�� ī�޶� ������ ���� ����
	// - GUI ������ ���� �̹��� ���� ���� ���� Ȯ��/��Ҹ� �ؼ� �����ֵ��� ����	
	const int	nWantSizeX = 320;
	//const int	nWantSizeY = 240;	// ���� �̹��� ������ ���� ���� ũ��� ����
	const int	nOffSet = 2;

	// �������� �Ҵ��Ͽ� ��� - ���� ũ��� ����
	//cv::Mat				MatDefectImage = cv::Mat::zeros(nWantSizeY, nWantSizeX, CV_8UC3);

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
		int nCamNum = resultPanelData.m_ListDefectInfo[i].Camera_No;

		int	nTargetWidth = MatDrawBuffer[nImgNum][nCamNum].cols;
		int	nTargetHeight = MatDrawBuffer[nImgNum][nCamNum].rows;

		int nWantSizeY = (nTargetHeight * nWantSizeX) / nTargetWidth;	// ���� �̹��� ������ ���� ���� �̹��� Height ����

		cv::Rect rectCropDefect(0, 0, 0, 0);

		if (resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_X == 0 && resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_X == 0 &&
			resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_Y == 0 && resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_Y == 0)
		{
			rectCropDefect = cv::Rect(0, 0, nTargetWidth, nTargetHeight);
		}
		else
		{
			int nCenterX = (int)((resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_X) / 2);
			int nCenterY = (int)((resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_Y) / 2);

			// ���� Defect ����
			int nDefStartX, nDefStartY, nDefWidth, nDefHeight;

			nDefStartX = resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_X - nOffSet;
			nDefStartY = resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_Y - nOffSet;
			nDefWidth  = resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_X + (nOffSet * 2) + 1;
			nDefHeight = resultPanelData.m_ListDefectInfo[i].Pixel_Crop_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Crop_Start_Y + (nOffSet * 2) + 1;

			// ���� ���� �̹��� �ʺ� / ����
			int nCropWidth, nCropHeight;

			// �ҷ� ������ 320*240 ���� ���� ��� �߽���ǥ�� �������� 320*240 ���� �ڸ�
			if (nDefWidth <= nWantSizeX && nDefHeight <= nWantSizeY)
			{
				nCropWidth  = nWantSizeX;
				nCropHeight = nWantSizeY;
			}
			else
			{
				if (nDefWidth > nDefHeight)
				{
					nCropWidth  = nDefWidth;
					nCropHeight = (int)(nTargetHeight * (1.0 * nDefWidth / nTargetWidth));
				}
				else
				{
					nCropWidth  = (int)(nTargetWidth * (1.0 * nDefHeight / nTargetHeight));
					nCropHeight = nDefHeight;
				}
			}

			// Crop Image �ڸ��� ����
			rectCropDefect = cv::Rect(nCenterX - (nCropWidth / 2), nCenterY - (nCropHeight / 2), 
									nCropWidth, nCropHeight);
		}

		// �̹��� ��ǥ ����ó��
		if( rectCropDefect.width > nTargetWidth)	rectCropDefect.width = nTargetWidth;
		if( rectCropDefect.height > nTargetHeight)	rectCropDefect.height = nTargetHeight;
		if( rectCropDefect.x + rectCropDefect.width >= nTargetWidth )	rectCropDefect.x = nTargetWidth - 1 - rectCropDefect.width;
		if( rectCropDefect.y + rectCropDefect.height >= nTargetHeight )	rectCropDefect.y = nTargetHeight - 1 - rectCropDefect.height;
		if( rectCropDefect.x < 0 )		rectCropDefect.x = 0;
		if( rectCropDefect.y < 0 )		rectCropDefect.y = 0;

		// ��� �̹��� �ҷ� ��ǥ���� 320*240 Buffer Copy
		// ����� ���� �����͸� �״�� ���
		cv::Mat MatDefectImage = MatDrawBuffer[nImgNum][nCamNum](rectCropDefect);

		// �̹��� ũ�� Ʋ�� ���, ������ �޸𸮿� �ֱ�
		//if (matTemBuf.cols != nWantSizeX || matTemBuf.rows != nWantSizeY)
			cv::resize(MatDefectImage, MatDefectImage, cv::Size(nWantSizeX, nWantSizeY));

		// �̹��� ���� ���
		CString strImageName = _T("");
		strImageName.Format(_T("%d_%04d.jpg"), resultPanelData.m_ListDefectInfo[i].Defect_Type, i+1);
		COPY_CSTR2TCH(resultPanelData.m_ListDefectInfo[i].Defect_Img_Name, strImageName, sizeof(resultPanelData.m_ListDefectInfo[i].Defect_Img_Name));

		// �̹��� ����

		ImageSave(MatDefectImage, _T("%s\\%s\\%s"), RESULT_PATH, strPanelID, resultPanelData.m_ListDefectInfo[i].Defect_Img_Name);
	}

	//MatDefectImage.release();
}

/// AD �˻� -> Align Image -> AD GV �˻�
BOOL InspMainSequence::CheckImageIsNormal(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, int nRatio, int nImageNum, int nCameraNum, int nStageNo,
										tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], ResultBlob_Total* pResultBlobTotal, double dCamResolution,double dPannelSizeX,double dPannelSizeY,
										bool bAlignEnd[MAX_CAMERA_COUNT],bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, 
										ENUM_INSPECT_MODE eInspMode)
{
	// AD ����
	long nErrorCode = E_ERROR_CODE_TRUE;
	double* dAlignPara = theApp.GetAlignParameter(nCameraNum);

	int nAlignImageClassify = 0;
	int nEQType = theApp.m_Config.GetEqpType();
	switch(nEQType)
	{
	case EQP_AVI:
	case EQP_LCP:
		nAlignImageClassify  = (int)dAlignPara[E_PARA_AD_IMAGE_NUM];		// Align Parameter 0������ Align �� Image ID(Classify) 
		break;
	case EQP_SVI:
		nAlignImageClassify  = (int)dAlignPara[E_PARA_AD_IMAGE_NUM];		// Align Parameter 0������ Align �� Image ID(Classify) 
		break;
	case EQP_APP:
		nAlignImageClassify  = (int)dAlignPara[E_PARA_APP_AD_IMAGE_NUM];	// Align Parameter 0������ Align �� Image ID(Classify) 
		break;
	default:
		return E_ERROR_CODE_FALSE;
	}
	
	int nCurlImageClassify = (int)dAlignPara[E_PARA_APP_CURL_IMAGE_NUM];


	//if(MatOrgImage.empty())
	//	return E_ERROR_CODE_EMPTY_BUFFER;

	//Scalar m,s;
	//cv::meanStdDev(MatOrgImage, m, s);

	//if( m[0]==0 )
	//	return E_ERROR_CODE_EMPTY_BUFFER;

	int nPat = theApp.GetImageClassify(nImageNum);
	if(nPat == nAlignImageClassify)
	{
		// Alg DLL Start Log
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD Algorithm Start. PanelID: %s, CAM: %02d, Img: %s."),
			strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum));

		// 0 : ��ġ�� (Rate)
		// 1 : ��� ��� (Mean GV)
		// 2 : ǥ������ (Std)
		// 3 : ��� ������ ( DISPLAY_ABNORMAL, DISPLAY_DARK, DISPLAY_BRIGHT )
		double dResult[4] = {0, };

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// AD �˻�
		// AD �˰��� ��, ���� �����ؾ� ��...
		
		nErrorCode = CheckAD(strPanelID, strDrive, MatOrgImage, nImageNum, nCameraNum, dResult, nRatio);	
		
		if (nErrorCode != E_ERROR_CODE_TRUE)	// AD �ҷ��� ��� �˻� ����
		{
			// Alg DLL Stop Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, 
				_T("AD Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Rate : %.2f %% / Avg : %.2f GV / Std : %.2f ) ErrorCode : %d"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1], dResult[2], nErrorCode);

			// ���� �̻� / ��� �̻� / ���...
			if (nErrorCode == E_ERROR_CODE_ALIGN_DISPLAY)
			{
				// AD ���� ����� ����
				JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, (int)dResult[3], eInspMode);
			}
			// ���۰� ���� ��� / �Ķ���� �̻��� ��� / ���...
			else
			{
				// ���� �ҷ����� �����ϴ� ��� �ʿ� - ���� AD �ҷ����� �ϰ� ����
				JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL, eInspMode);
			}
			CString strOrgFileName = _T("");
			strOrgFileName.Format(_T("%s_CAM%02d_AD_Defect"), theApp.GetGrabStepName(nImageNum), nCameraNum);	
			strOrgFileName = strOrgFileName + _T(".bmp");


			ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
				ORIGIN_PATH, strPanelID, nImageNum, strOrgFileName);

			// Alg DLL End Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, 
				_T("AD Algorithm End Fail. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Rate : %.2f %% / Avg : %.2f GV / Std : %.2f )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1], dResult[2]);

			stCamAlignInfo[nCameraNum].bAlignSuccess = false;
		}
		
		else	// AD �ҷ��� �ƴ� ��츸 Align ����
		{
			// Alg DLL End Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, 
				_T("AD Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Rate : %.2f %% / Avg : %.2f GV / Std : %.2f )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1], dResult[2]);
			
			// Alg DLL Start Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/// Align Image

			// 0 :����� ���� ��������
			// pStCamAlignInfo->dAlignTheta �������� ������
			// Align ���н� �����͸� ���� �ʾƼ� 0�����θ� ���.
			// 1 : y�� Pixel ����
			double dResult[4] = {0, };		// SVI ���� ����

			// Image Align �� Auto ROI ����
			// AVI : ���� ���� �״�� ��� / SVI : ���� ���� Cell ���� Warping / APP : ���� ���� Rotate	
 			nErrorCode = AcquireAutoRoiAndAlignImage(strPanelID, strDrive, MatOrgImage, nRatio, nImageNum, nCameraNum, stCamAlignInfo[nCameraNum], dResult,dCamResolution , 
 													dPannelSizeX, dPannelSizeY);
 			
			

			// ���� �ۺο� ī�޶� �߽���ġ - �� �߽���ġ - ���� um
			int nDiffCenterX = (int)((stCamAlignInfo[nCameraNum].ptAlignCenter.x - stCamAlignInfo[nCameraNum].ptCellCenter.x) * dCamResolution);
			int nDiffCenterY = (int)((stCamAlignInfo[nCameraNum].ptAlignCenter.y - stCamAlignInfo[nCameraNum].ptCellCenter.y) * dCamResolution);

			// KSW 17.08.10 - ���� ��ġ �̻� Ʋ���� ��� ���ϰ� : E_ERROR_CODE_ALIGN_ANGLE_RANGE			// �ش� ��ġ�� Align �Ķ���� : AngleError ���� ����
			// ���� ��ġ �̻� Ʋ������ ��� ��˶� ����
			// ���� ������ ��� ��� Ȯ�� ������ �˶� ����� ��...
			// �������� �ٽ� ƼĪ�ϵ���... or ��� Ʋ�������� Ȯ�� ��/��
			if (nErrorCode == E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR || nErrorCode == E_ERROR_CODE_ALIGN_IMAGE_OVER ||  
				nErrorCode == E_ERROR_CODE_ALIGN_LENGTH_RANGE_ERROR)		// FOV ��� ��� �߰�// Curl�� ���� Align Fail �߰� - 2018.08.02 CJS - �ʿ� �� ���� ����
			{
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End - InspStop !!!. Abnormal Angle. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Theta : %.4f' / Diff Pixel : %.0f )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1]);

				// ����PC�� ����� �ۺ�
				theApp.m_AlgorithmTask->VS_Send_Align_Result_To_MainPC(eInspMode, (int)(dResult[0] * 1000), nDiffCenterX, nDiffCenterY, nStageNo, theApp.m_Config.GetPCNum());

				// �߾˶����� ����
				//SendAlarmToMain(eALARMID_ALIGN_WARNING, eALARMTYPE_LIGHT, bIsHeavyAlarm);
				theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_ALIGN_ANGLE_ERROR, eALARMTYPE_HEAVY, bIsHeavyAlarm);

				CString strOrgFileName = _T("");
				strOrgFileName.Format(_T("%s_CAM%02d_Abnormal_Angle"), theApp.GetGrabStepName(nImageNum), nCameraNum);	
				strOrgFileName = strOrgFileName + _T(".bmp");

				ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
					ORIGIN_PATH, strPanelID, nImageNum, strOrgFileName);
			

				stCamAlignInfo[nCameraNum].bAlignSuccess = false;				
			}
			else if (nErrorCode == E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING)	// Align �� �Ǿ����� �˻翡 ������ ���� ���ɼ��� �ִ� ���
			{
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End. Angle Warning !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Theta : %.4f' / Diff Pixel : %.0f )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1]);

				// ����PC�� ����� �ۺ�
				theApp.m_AlgorithmTask->VS_Send_Align_Result_To_MainPC(eInspMode, (int)(dResult[0] * 1000), nDiffCenterX, nDiffCenterY, nStageNo, theApp.m_Config.GetPCNum());

				// ��˶� �߻� - ��˶� ���� / �α׸� ���� (���μ�B ��û)
				//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_ALIGN_ANGLE_WARNING, eALARMTYPE_LIGHT, bIsHeavyAlarm);				

				CString strOrgFileName = _T("");
				strOrgFileName.Format(_T("%s_CAM%02d_Warning_Angle"), theApp.GetGrabStepName(nImageNum), nCameraNum);	
				strOrgFileName = strOrgFileName + _T(".bmp");

				ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
					ORIGIN_PATH, strPanelID, nImageNum, strOrgFileName);

				stCamAlignInfo[nCameraNum].bAlignSuccess = true;	// �켱 �˻�� ����				
			}
			// ����� ��ü�� ������ ���
			else if (nErrorCode != E_ERROR_CODE_TRUE)
			{
				// Error Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, 
					_T("Align Fail - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.. Error Code : %d"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), nErrorCode);

				// �߾˶����� ����
				theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_ALIGN_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);

				CString strOrgFileName = _T("");
				strOrgFileName.Format(_T("%s_CAM%02d_Align_Fail"), theApp.GetGrabStepName(nImageNum), nCameraNum);	
				strOrgFileName = strOrgFileName + _T(".bmp");


				ImageSave(MatOrgImage, _T("%s\\%s\\%02d_%s"),
					ORIGIN_PATH, strPanelID, nImageNum, strOrgFileName);

				// Align Error �ӽ÷� Display Abnormal ����
				// ���� �ҷ����� �����ϴ� ��� �ʿ� - ���� AD �ҷ����� �ϰ� ����
				//JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL, eInspMode);

				stCamAlignInfo[nCameraNum].bAlignSuccess = false;

// �ߺ� �α� ����. Theta �� �ǹ� ����
// 				// Alg DLL End Log
// 				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End Fail. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Theta : %.4f' / Diff Pixel : %.0f )"),
// 					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[0]);
			}
			// ������ ���
			else
			{				
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Theta : %.4f' / Diff Pixel : %.0f )"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1]);

				stCamAlignInfo[nCameraNum].bAlignSuccess = true;

				// ����PC�� ����� �ۺ�
				theApp.m_AlgorithmTask->VS_Send_Align_Result_To_MainPC(eInspMode, (int)(dResult[0] * 1000), nDiffCenterX, nDiffCenterY, nStageNo, theApp.m_Config.GetPCNum());
			}
		}

		if(nEQType == EQP_APP && nErrorCode == E_ERROR_CODE_TRUE)
		{
			if(nErrorCode == E_ERROR_CODE_TRUE)
			{
				tLogicPara LogicPara;
				LogicPara.MatOrgImage		= MatOrgImage;
				LogicPara.nCameraNum		= nCameraNum;

				makePolygonCellROI(LogicPara, stCamAlignInfo[nCameraNum], nImageNum, nCameraNum, dAlignPara, theApp.GetImageClassify(nImageNum), nRatio);
			}
		}
		bAlignEnd[nCameraNum] = true;
	}

	// Align ���� �ʴ� �̹������� ����� ��� ��ٸ�
	// ��� ī�޶� ����� �Ϸ�ɶ�����
	for (int i=0; i<MAX_CAMERA_COUNT; i++)
	{
		while(!bAlignEnd[i])
		{
			Sleep(10);
		}
	}
	
	bool bNeedRetry = false;

	// �� �����庰 ����� ���� ����
	// (���� Step �� Pixel Shift ���ο� ���� �⺻ �ػ� ���� Align Info �� ���� ����Ͽ� ����)
	m_stThrdAlignInfo.SetAdjustAlignInfoRatio(&stCamAlignInfo[nCameraNum], nRatio, stCamAlignInfo[0].bAlignSuccess && stCamAlignInfo[nCameraNum].bAlignSuccess);	// 0�� ī�޶� ����� ���и� ��� ���з� ����

	// AD �˻� �� Align ���� �ÿ��� GV �˻� ����
	// Align Warning �� ��쿡�� �˻�� ����
	if ((nErrorCode == E_ERROR_CODE_TRUE || nErrorCode == E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING) && m_stThrdAlignInfo.bAlignSuccess == true && !bIsHeavyAlarm)
	{
		// 0 : ��� ��� (Mean GV)
		// 1 : ��� ������ ( DISPLAY_ABNORMAL, DISPLAY_DARK, DISPLAY_BRIGHT )
		// ����� AD GV�� ��Ȯ��
		double dMeanResult[4] = {0 , };		// 2->4 SVI ���� ����

		/// AD GV �˻�		
		nErrorCode = CheckADGV(strPanelID, strDrive, MatOrgImage, nStageNo, nImageNum, nCameraNum, nRatio, m_stThrdAlignInfo.ptCorner, pResultBlobTotal, dMeanResult, 
								bChkDustEnd, bNeedRetry, bIsNormalDust, bUseDustRetry, nDustRetryCnt, bIsHeavyAlarm, eInspMode);

		// for SVI - ���� 0�� ī�޶� �̹��� �������� Warping �ؾ���.
		// 17.06.08 APP �߰� : Rotate
		AdjustOriginImage(MatOrgImage, MatDrawImage, &stCamAlignInfo[0]);
		//ImageSave(MatOrgImage, _T("D:\\Test\\Origin%d_CAM%d.jpg"), nImageNum, nCameraNum);

		//APP���� Curl ���� ����ϴ� �κ�, Dust ������ ����� by 2018.01.02
		if(nEQType == EQP_APP && nErrorCode == E_ERROR_CODE_TRUE)
		{
			if(theApp.GetImageClassify(nImageNum) == nCurlImageClassify)
			{
				stMeasureInfo* stCurlMeasure;
				stCurlMeasure = new stMeasureInfo(4);	//Top, Bottom, Corner �� Curl ���� ���� ��
				stCurlMeasure->strPanelID = strPanelID;
				PanelCurlJudge(MatOrgImage, dAlignPara, &stCamAlignInfo[nCameraNum], pResultBlobTotal, nImageNum, stCurlMeasure,ALG_RESULT_PATH);

				wchar_t fCurlPath[255] = {0};
				swprintf(fCurlPath, _T("%s\\Measure"), (LPCWSTR)theApp.m_Config.GetResultPath());

				CString strResult;
				strResult.Format(_T("%d,%lf,%lf,%lf,%lf"), stCurlMeasure->bJudge, stCurlMeasure->dMeasureValue[0], stCurlMeasure->dMeasureValue[1], stCurlMeasure->dMeasureValue[2], stCurlMeasure->dMeasureValue[3]);

				TCHAR* szResult = NULL;
				szResult = (TCHAR*)(LPCTSTR)strResult;
				theApp.WriteResultFile(strPanelID, fCurlPath, _T("Curl.csv"), _T("PANEL_ID,JUDGE,TOP_MAX,BOTTOM_MAX, TOP_CORNER_MAX, BOTTOM_CORNER_MAX"), szResult);
			}
		}
	}
	else
	{
		/// for AVI - Dust Retry �ʿ� ���� - Dust �˻� ���� - SVI / APP Align �� �ʿ� �� Ȯ���Ͽ� Ȱ��
		// Seq. Task �� Dust ���� �� ��� �ۺ� (AVI)
		bNeedRetry = false;
		bIsNormalDust = false;

		// �ӽ� ������ 4�� ä���ֱ� (�����)
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_LEFT_TOP]		= cv::Point(0, 0);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_RIGHT_TOP]		= cv::Point(MatOrgImage.cols-1, 0);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_RIGHT_BOTTOM]	= cv::Point(MatOrgImage.cols-1, MatOrgImage.rows-1);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_LEFT_BOTTOM]	= cv::Point(0, MatOrgImage.rows-1);
	}

	// Seq. Task �� Dust ���� �� ��� �ۺ� (����� AVI�� ���)
	if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_DUST)
		theApp.m_AlgorithmTask->VS_Send_Dust_Result_To_Seq(eInspMode, bNeedRetry, bIsNormalDust);

	return nErrorCode;
}
void InspMainSequence::Align_Test(cv::Mat MatOrgImage, cv::Mat& MatRotateImage ,float* fR_Degree ,RotatedRect& rRotate_Rect) {
	const double um = 300 * pow(10, -6); // 30um -> �ȳ��ͼ� �ϴ� 300um����..
										 //string path = "C:\\Users\\woojin\\Desktop\\YWDSP\\img";
	float fStan_x = 0.f, fStan_y = 0.f; // ù ������ ���� ������ ������ ������� ��ǥ
										// float fObj_x = 0.f, fObj_min = 0.f, fObj_max = 0.f, fObj_cal = 0.f, fObj_res = 0.f;


	Mat src ,dst;
	Mat img_draw; // �׽�Ʈ�Ҷ� �׸��� �뵵

	MatOrgImage.copyTo(src);
	MatOrgImage.copyTo(dst);
	//MatDrawImage.copyTo(img_draw);

	Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
	vector<Vec3f> circles; // ���� �� ã�� (�ȵǸ� Ÿ�� ã�� or PerspectiveTransform)
	vector<vector<Point>> contours; // ū Ÿ�� ã��
	vector<Vec4i> hierarchy;
	Mat thres;
	src.copyTo(thres);

	RotatedRect e; // ū Ÿ�� ����
	Vec3f find_circle; // ���� �� ����
	float fCircle_x = 0.f, fCircle_y = 0.f, fMin_dist = 0.f, fMax_dist = 0.f, fCircle_dist = 0.f, fDegree = 0.f, fCal_dist = 0.f;
	double dArea;

	cv::threshold(src, thres, 70, 255, 0);

	findContours(thres, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//cout << "Ÿ���� ���� : " << contours.size() << endl; // üũ

	for (int i = 0; i < contours.size(); i++) {
		dArea = contourArea(contours[i]);
		if (dArea > 1000000) // ǥ�� 3�� ���� ���� �� 112��
		{
			e = fitEllipse(contours[i]);
			// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
			fMax_dist = e.size.height / 2; // Ÿ���� ����/2
			fCal_dist = e.size.width / 2; // Ÿ���� ����/2 (�ѷ� ����)

			ellipse(img_draw, e, Scalar(255, 0, 0), 2); // �׸��� üũ (Ÿ�� �׵θ�)
			circle(img_draw, e.center, 2, Scalar(0, 0, 255), 3); // �׸��� üũ (Ÿ�� �߽�)
			line(img_draw, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (Ÿ�� ����)

			Size2f size(e.size.width / 1.3, e.size.height / 1.3); // ���� ��� ����� -> ���� �� ã�� ����
			e.size = size;
			fMin_dist = e.size.width / 2; // ���� Ÿ���� ����/2

			ellipse(img_draw, e, Scalar(255, 255, 0), 2); // �׸��� üũ (���� Ÿ�� �׵θ�)
			line(img_draw, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (���� Ÿ�� ����)
			break;
		}
	}

	//////////
	// ���� �� ã��
	medianBlur(src, src, 5); // blur�� ��������

	HoughCircles(src, circles, HOUGH_GRADIENT, 1, 20, 255.0, 35.0, 15, 150); // 1, 20, 255.0, 35.0, 15, 20
	//else if (ImageClassify == 9) HoughCircles(src, circles, HOUGH_GRADIENT, 1, 20, 50.0, 20.0, 15, 150); // ����Ʈ ���� ����

	cout << "���� ���� ���� : " << circles.size() << endl; // üũ



													 //////////
													 // ���� �� Ž��
	if (circles.size() > 0)
	{
		for (size_t i = 0; i < circles.size(); i++)
		{
			find_circle = circles[i];
			Point2f pCircle_center(find_circle[0], find_circle[1]);
			fCircle_dist = sqrt(pow(e.center.x - pCircle_center.x, 2) + pow(e.center.y - pCircle_center.y, 2));

			line(img_draw, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // �׸��� üũ (���� �� �߽���)

			if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // ���� �����ȿ� ������ �Ϸ�
			{
				circle(img_draw, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // �׸��� üũ (�� �׵θ�)
				circle(img_draw, pCircle_center, 1, Scalar(0, 0, 255), 2); // �׸��� üũ (�� �߽�)

				fCircle_x = find_circle[0]; // ��ǥ ���� �� x��ǥ
				fCircle_y = find_circle[1]; // ��ǥ ���� �� y��ǥ

				break; // break�� �ּ����� �ϸ� ��� ���� ���� ������ �׷���. ���� ������ ���� �� �׵θ��� �׷���.
			}
		}

		// ���� �����ȿ� ���� ���� 2���̻��̸�? -> ��� ��ƼĿ�� ��˻� �������� ����� or ���� �� �߽����� ��� ���� or ���� �߾� �ٻ簪���� �̱� ��.
		// ���� �� �߽����� ���� ���͸� �ϴ°� ���� ���� ����̶� ���������� �ش� �ȼ��� ���� ����� ���޾Ƽ�.. ����

		if (fCircle_x <= 0 || fCircle_y <= 0) // ���� �����ȿ� ���� ���� 1���� ������
		{
			cerr << "Circle is not in range." << endl;
			return;
		}
	}
	else if (circles.size() == 0) // ���� ���� ��ã����
	{
		cerr << "Circle not find." << endl;
		return;
	}



	//////////
	// ���� ���� �������� ȸ�� �� �̵�
	// cerr << "fCircle_x : " << fCircle_x << " fCircle_y : " << fCircle_y << endl;

	// ���� ���
	// fObj_x = e.center.x + fMin_dist + (fMax_dist - fMin_dist) / 2; // ��ǥ �߽��� x��ǥ
	// fObj_min = e.center.x - fMax_dist; // x��ǥ ȸ�� ��� min��
	// fObj_max = e.center.x + fMax_dist; // x��ǥ ȸ�� ��� max��
	// fObj_cal = fObj_max / 90; // ���� 1�� �ȼ� ���
	// fObj_res = (fCircle_x - fObj_min) / fObj_cal;
	//if (fCircle_y <= e.center.y) // ���� ���� �߽����̻� (���� ������)
	//{
	//	line(img_draw, e.center, Point(fObj_max, e.center.y), Scalar(255, 255, 255), 2, 8, 0); // ��ǥ �߽���
	//	circle(img_draw, Point(fObj_max, e.center.y), 1, Scalar(0, 0, 255), 2); // �׸���� üũ (�� �߽�)
	//}
	//else if (fCircle_y > e.center.y) // ���� ���� �߽����̸� (�Ʒ��� ������)
	//{
	//	line(img_draw, e.center, Point(fObj_max, e.center.y), Scalar(192, 192, 192), 2, 8, 0); // ��ǥ �߽���
	//	circle(img_draw, Point(fObj_max, e.center.y), 1, Scalar(0, 0, 255), 2); // �׸���� üũ (�� �߽�)
	//}
	// ���� ��ǥ�� ���� ����Ϸ� ������ �ȵ�. �ϴ� ����

	// ���� ���
	// ���ﰢ�Լ��� ū Ÿ���� �߽����� ���� ���� �߽��� ���� ��Ÿ�� ���Ͽ� 3�ù������� ȸ��.
	// ��ũź��Ʈ�� radian���� ��ȯ�ǹǷ� degree�� ��ȯ
	fDegree = atan2f(fCircle_y - e.center.y, fCircle_x - e.center.x) * 180.f / CV_PI;

	M = getRotationMatrix2D(e.center, fDegree, 1.0);
	cv::warpAffine(dst, dst, M, dst.size()); // ȸ��
	String Folder_Path = "C:\\Users\\chvx1\\OneDrive\\���� ȭ��\\�Ե��ɹ�Į\\���� - ���纻\\";
	CString strTem;
	strTem.Format(_T("ARESULT\\(%f)_Rotate.png"), fDegree);
	cv::imwrite(Folder_Path + CSTR2PCH(strTem), dst);
	
	dst.copyTo(MatRotateImage);

	*fR_Degree = fDegree;
	rRotate_Rect = e;
}

long InspMainSequence::Lotte_Align(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, cv::Mat& MatResultImage, int ImageClassify, bool bAlignEnd[MAX_CAMERA_COUNT], int nCameraNum, cv::Point &pAlign_Center, Rect& boundingRect, double& fDegree, double* dAlignPara) {

	if (dAlignPara == NULL)			return E_ERROR_CODE_ALIGN_WARNING_PARA;

	int nThreshold = (int)dAlignPara[E_PARA_LCP_ALIGN_TH];
	int nMedian = (int)dAlignPara[E_PARA_LCP_ALIGN_MEDIAN_BLUR];
	int nCircle_MinArea = (int)dAlignPara[E_PARA_LCP_ALIGN_BIGCIRCLE_MINAREA];
	double dHough_dp = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_DP];
	double dHough_minDist = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_MINDIST];
	double dHough_param1 = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_PARAM1];
	double dHough_param2 = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_PARAM2];
	int nHough_minRadius = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_MINRADIUD];
	int nHough_maxRadius = (int)dAlignPara[E_PARA_LCP_ALIGN_HOUGH_MAXRADIUD];

		const double um = 300 * pow(10, -6); // 30um -> �ȳ��ͼ� �ϴ� 300um����..
		//string path = "C:\\Users\\woojin\\Desktop\\YWDSP\\img";
		float fStan_x = 0.f, fStan_y = 0.f; // ù ������ ���� ������ ������ ������� ��ǥ
										// float fObj_x = 0.f, fObj_min = 0.f, fObj_max = 0.f, fObj_cal = 0.f, fObj_res = 0.f;

	
		Mat src;
		Mat img_draw; // �׽�Ʈ�Ҷ� �׸��� �뵵

		MatOrgImage.copyTo(src);
		MatDrawImage.copyTo(MatResultImage);
		MatDrawImage.copyTo(img_draw);

		Mat M(2, 3, CV_32FC1); // warpAffine ���꿡 ���Ǵ� 2*3 ���
		vector<Vec3f> circles; // ���� �� ã�� (�ȵǸ� Ÿ�� ã�� or PerspectiveTransform)
		vector<vector<Point>> contours; // ū Ÿ�� ã��
		vector<Vec4i> hierarchy;
		Mat thres;
		src.copyTo(thres);

		RotatedRect e; // ū Ÿ�� ����
		Vec3f find_circle; // ���� �� ����
		float fCircle_x = 0.f, fCircle_y = 0.f, fMin_dist = 0.f, fMax_dist = 0.f, fCircle_dist = 0.f, fCal_dist = 0.f;
		double dArea;

		if (src.empty())
		{
			return E_ERROR_CODE_ALIGN_IMAGE_OVER;
		}

		//////////
		// ū Ÿ�� ã��
		cv::threshold(src, thres, nThreshold, 255, 0);

		findContours(thres, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


		for (int i = 0; i < contours.size(); i++) {
			dArea = contourArea(contours[i]);
			if (dArea > nCircle_MinArea) // ǥ�� 3�� ���� ���� �� 112��
			{
				e = fitEllipse(contours[i]);
				// cout << "e.angle : " << e.angle << " e.center : " << e.center << " e.size : " << e.size << endl;
				fMax_dist = e.size.height / 2; // Ÿ���� ����/2
				fCal_dist = e.size.width / 2; // Ÿ���� ����/2 (�ѷ� ����)

				ellipse(img_draw, e, Scalar(255, 0, 0), 2); // �׸��� üũ (Ÿ�� �׵θ�)
				circle(img_draw, e.center, 2, Scalar(0, 0, 255), 3); // �׸��� üũ (Ÿ�� �߽�)
				line(img_draw, e.center, Point2f(e.center.x, e.center.y + e.size.height / 2), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (Ÿ�� ����)

				Size2f size(e.size.width / 1.3, e.size.height / 1.3); // ���� ��� ����� -> ���� �� ã�� ����
				e.size = size;
				fMin_dist = e.size.width / 2; // ���� Ÿ���� ����/2

				ellipse(img_draw, e, Scalar(255, 255, 0), 2); // �׸��� üũ (���� Ÿ�� �׵θ�)
				line(img_draw, e.center, Point2f(e.center.x + e.size.width / 2, e.center.y), Scalar(127, 0, 127), 2, 8, 0); // �׸��� üũ (���� Ÿ�� ����)

				if (ImageClassify == E_IMAGE_CLASSIFY_AVI_COAXIAL) {
					boundingRect = cv::boundingRect(contours[i]);
				}

				rectangle(img_draw, cv::boundingRect(contours[i]), Scalar(0, 0, 255), 2);

				break;
			}
		}




		//////////
		// ���� �� ã��
		medianBlur(src, src, nMedian); // blur�� ��������
								 // getPerspectiveTransform();
								 // warpPerspective();

		HoughCircles(src, circles, HOUGH_GRADIENT, dHough_dp, dHough_minDist, dHough_param1, dHough_param2, nHough_minRadius, nHough_maxRadius); // 1, 20, 255.0, 35.0, 15, 20

		//cout << "���� ���� ���� : " << circles.size() << endl; // üũ



														 //////////
														 // ���� �� Ž��
		if (circles.size() > 0)
		{
			for (size_t i = 0; i < circles.size(); i++)
			{
				find_circle = circles[i];
				Point2f pCircle_center(find_circle[0], find_circle[1]);
				fCircle_dist = sqrt(pow(e.center.x - pCircle_center.x, 2) + pow(e.center.y - pCircle_center.y, 2));

				line(img_draw, e.center, Point2f(pCircle_center.x, pCircle_center.y), Scalar(0, 255, 255), 2, 8, 0); // �׸��� üũ (���� �� �߽���)

				if (fCircle_dist > fMin_dist && fCircle_dist < fMax_dist) // ���� �����ȿ� ������ �Ϸ�
				{
					circle(img_draw, pCircle_center, find_circle[2], Scalar(0, 255, 0), 1); // �׸��� üũ (�� �׵θ�)
					circle(img_draw, pCircle_center, 1, Scalar(0, 0, 255), 2); // �׸��� üũ (�� �߽�)

					fCircle_x = find_circle[0]; // ��ǥ ���� �� x��ǥ
					fCircle_y = find_circle[1]; // ��ǥ ���� �� y��ǥ

					break; // break�� �ּ����� �ϸ� ��� ���� ���� ������ �׷���. ���� ������ ���� �� �׵θ��� �׷���.
				}
			}

			// ���� �����ȿ� ���� ���� 2���̻��̸�? -> ��� ��ƼĿ�� ��˻� �������� ����� or ���� �� �߽����� ��� ���� or ���� �߾� �ٻ簪���� �̱� ��.
			// ���� �� �߽����� ���� ���͸� �ϴ°� ���� ���� ����̶� ���������� �ش� �ȼ��� ���� ����� ���޾Ƽ�.. ����

			if (fCircle_x <= 0 || fCircle_y <= 0) // ���� �����ȿ� ���� ���� 1���� ������
			{
				return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
			}
		}
		else if (circles.size() == 0) // ���� ���� ��ã����
		{
			return E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR;
		}



		//////////
		// ���� ���� �������� ȸ�� �� �̵�
		// cerr << "fCircle_x : " << fCircle_x << " fCircle_y : " << fCircle_y << endl;

		// ���� ���
		// fObj_x = e.center.x + fMin_dist + (fMax_dist - fMin_dist) / 2; // ��ǥ �߽��� x��ǥ
		// fObj_min = e.center.x - fMax_dist; // x��ǥ ȸ�� ��� min��
		// fObj_max = e.center.x + fMax_dist; // x��ǥ ȸ�� ��� max��
		// fObj_cal = fObj_max / 90; // ���� 1�� �ȼ� ���
		// fObj_res = (fCircle_x - fObj_min) / fObj_cal;
		//if (fCircle_y <= e.center.y) // ���� ���� �߽����̻� (���� ������)
		//{
		//	line(img_draw, e.center, Point(fObj_max, e.center.y), Scalar(255, 255, 255), 2, 8, 0); // ��ǥ �߽���
		//	circle(img_draw, Point(fObj_max, e.center.y), 1, Scalar(0, 0, 255), 2); // �׸���� üũ (�� �߽�)
		//}
		//else if (fCircle_y > e.center.y) // ���� ���� �߽����̸� (�Ʒ��� ������)
		//{
		//	line(img_draw, e.center, Point(fObj_max, e.center.y), Scalar(192, 192, 192), 2, 8, 0); // ��ǥ �߽���
		//	circle(img_draw, Point(fObj_max, e.center.y), 1, Scalar(0, 0, 255), 2); // �׸���� üũ (�� �߽�)
		//}
		// ���� ��ǥ�� ���� ����Ϸ� ������ �ȵ�. �ϴ� ����

		// ���� ���
		// ���ﰢ�Լ��� ū Ÿ���� �߽����� ���� ���� �߽��� ���� ��Ÿ�� ���Ͽ� 3�ù������� ȸ��.
		// ��ũź��Ʈ�� radian���� ��ȯ�ǹǷ� degree�� ��ȯ
		fDegree = atan2f(fCircle_y - e.center.y, fCircle_x - e.center.x) * 180.f / CV_PI;
		//M = getRotationMatrix2D(e.center, 0.0, 1.0);

		if (ImageClassify  == E_IMAGE_CLASSIFY_AVI_COAXIAL)
		{
// 			cv::warpAffine(src, src, M, src.size()); // ȸ��
// 													 // ȸ���� ���� �� �ٽ� ã��. ���ܰ� ������ ����. �� ���� ����� �����Ű�����..
// 			HoughCircles(src, circles, HOUGH_GRADIENT, 1, 20, 255.0, 35.0, 15, 150);
// 			find_circle = circles[0];
// 			fMain_Stan[0] = find_circle[0];
// 			fMain_Stan[1] = find_circle[1]; // �ٽ� ã�� ���� ���� �߽��� ��ǥ ���� (������)
// 
// 			M.copyTo(MatRotate);

			pAlign_Center.x = fCircle_x;
			pAlign_Center.y = fCircle_y;

			bAlignEnd[nCameraNum] = true;
		}
		else if (ImageClassify != E_IMAGE_CLASSIFY_AVI_COAXIAL)
		{
			for (int i = 0; i < MAX_CAMERA_COUNT; i++)
			{
				while (!bAlignEnd[i])
				{
					Sleep(10);
				}
			}
// 
// 			cv::warpAffine(src, src, M, src.size()); // ȸ��
// 			if (ImageClassify  == 0) HoughCircles(src, circles, HOUGH_GRADIENT, 1, 20, 255.0, 35.0, 15, 150); // �ٽ� ã��
// 			else if (ImageClassify  == 9) HoughCircles(src, circles, HOUGH_GRADIENT, 1, 20, 50.0, 20.0, 15, 150); // �ٽ� ã��
// 			find_circle = circles[0];

		
// 
			Mat M2(2, 3, CV_32FC1);
			M2.at<float>(0, 0) = 1.f;
			M2.at<float>(1, 0) = 0.f;
			M2.at<float>(0, 1) = 0.f;
			M2.at<float>(1, 1) = 1.f; // ���� ��� -> ��, ������ �״��
			M2.at<float>(0, 2) = pAlign_Center.x - fCircle_x; // ù ������ ���� ���� �������� x�� �̵���
			M2.at<float>(1, 2) = pAlign_Center.y - fCircle_y; // ù ������ ���� ���� �������� y�� �̵���
			cv::warpAffine(MatOrgImage, MatOrgImage, M2, src.size()); // �̵�
//  		boundingRect.x += (fMain_Stan[0] - fCircle_x);
//  		boundingRect.y += (fMain_Stan[1] - fCircle_y);
		}

	
		// Ÿ���� ����
		double dRes_P = 2 * CV_PI * sqrt((pow(fMax_dist * 2, 2) + pow(fCal_dist * 2, 2)) / 2) * um; // 2�� * sqrt((����^2 + ����^2) / 2)
		double dRes_area = CV_PI * fMax_dist * fCal_dist * um; // �� * ����/2 * ����/2

		img_draw.copyTo(MatResultImage);
// 		cout << "Ÿ���� ����(m) : " << fMax_dist * 2.f * um << endl;
// 		cout << "Ÿ���� ����(m) : " << fCal_dist * 2.f * um << endl;
// 		cout << "Ÿ���� �ܰ��� ����(m) : " << dRes_P << endl;
// 		cout << "Ÿ���� ����(m) : " << dRes_area << endl;

		// �׽�Ʈ�� �� ���� â
		// �Ķ��� : Ÿ���� �׵θ�, �ʷϻ� : ���� ���� �׵θ�, ��Ʈ�� : ���� �� ������ �� ������ �����ϴ� �׵θ�
		// ����� : ��Ʈ�� ���� ����/2 �� �Ķ��� ���� ����/2, ����� : Ÿ���� �߽����� ���� ���� �߽������� �Ÿ�

// 		int nImgSave = 1;
// 
// 		if (nImgSave >= 1)
// 		{
// 			CString strTemp;
// 			strTemp.Format(_T("D:\\TEST\\Align\\%02d_%02d_LCP_Align.bmp"), ImageClassify, nImgSave++);
// 			cv::imwrite((cv::String)(CStringA)strTemp, MatResultImage);
// 		}
// 		if (nImgSave >= 1)
// 		{
// 			CString strTemp;
// 			strTemp.Format(_T("D:\\TEST\\Align\\%02d_%02d_LCP_Align_Draw.bmp"), ImageClassify, nImgSave++);
// 			cv::imwrite((cv::String)(CStringA)strTemp, img_draw);
// 		}

	

		return E_ERROR_CODE_TRUE;
}

long InspMainSequence::MakeMask(cv::Mat& matSrc, cv::Mat& matDstActive, cv::Mat& matDstLabel, cv::Mat& matDstHole, int nThreshActive, int nThreshLabel, int nThreshHole ,cv::Point pAlign_Center, int offset)
{
	cv::Mat matThreshold;

	// Active ������ ã�� ���� ����ȭ
	cv::threshold(matSrc, matThreshold, nThreshActive, 255, THRESH_BINARY);


	// Active Mask �����
	MakeActiveMask(matThreshold, matDstActive, nThreshActive);

// 	cv::imwrite("D:\\TEST\\Mask\\test_2_1.jpg", matThreshold);
// 
// 	cv::imwrite("D:\\TEST\\Mask\\Active_Mask.jpg", matDstActive);

	//////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////


	// ���⼭ ���Ͱ� Label ã��
	cv::Mat matGauss;
	cv::GaussianBlur(matSrc, matGauss, Size(3, 3), 3.0);

	
	cv::Mat Maximum = Mat::zeros(matGauss.size(), matGauss.type());
	cv::Mat Minimum = Mat::zeros(matGauss.size(), matGauss.type());;

	MinimumFilter(matGauss, Minimum, 5);

	MaximumFilter(matGauss, Maximum, 5);

	cv::subtract(Maximum, Minimum, Maximum);


	/*cv::imwrite("D:\\TEST\\Mask\\test_2_3.jpg", Maximum);*/

	cv::threshold(Maximum, Maximum, nThreshLabel, 255, THRESH_BINARY);

	////////////////////////////////// ������




	// Lable Mask �����
	MakeLabelMask(Maximum, matDstLabel);

	/*cv::imwrite("D:\\TEST\\Mask\\Label_Mask.jpg", matDstLabel);*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////


	// Hole Mask �����

	cv::Mat mHoleMask = Mat::zeros(matSrc.size(), matSrc.type());

	cv::threshold(matSrc, matDstHole, nThreshHole, 255, THRESH_BINARY_INV);

	cv::bitwise_and(matDstHole, matDstActive, matDstHole);

	cv::subtract(matDstHole, matDstLabel, matDstHole);

// 	MinimumFilter(matDstHole, matDstHole, 15);
// 	MaximumFilter(matDstHole, matDstHole, 15);

	

	cv::Point LT = cv::Point(pAlign_Center.x - offset, pAlign_Center.y - offset);
	cv::Point RB = cv::Point(pAlign_Center.x + offset, pAlign_Center.y + offset);

	if (LT.x > 0 && LT.y > 0 && RB.x < matSrc.cols && RB.y < matSrc.rows) {
		cv::Rect2i rHole = Rect2i(LT, RB);
		matDstHole(rHole).copyTo(mHoleMask(rHole));
		/*cv::imwrite("D:\\TEST\\Mask\\Hole_Mask.jpg", mHoleMask);*/
		mHoleMask.copyTo(matDstHole);
	}

	//cv::imwrite("D:\\TEST\\Mask\\Hole_Mask.jpg", matDstHole);
	
	// �޸� ����
	// �̸��̸� �صθ� ������ �ϴ� Test... 

	matThreshold.release();
	matGauss.release();
	Maximum.release();
	Maximum.release();

	return E_ERROR_CODE_TRUE;

}

void InspMainSequence::MakeLabelMask(cv::Mat& matSrc, cv::Mat& matDstActive)
{
	// �ι�°�� ū ������ ��Ƴ��� - ���ڵ� ���� (���� ����: ���ڵ�� �׻� ��γ� �Ϻ� ���� �ϳ��� �����ǿ����� ��Ÿ������)

	if (matDstActive.empty())
		matDstActive = Mat::zeros(matSrc.size(), matSrc.type());

	double dLargest_Area2 = 0;
	int nLargest_Contour_Index2 = 0;

	double dSecondLargest_Area2 = 0;
	int nSecondLargest_Contour_Index2 = 0;

	vector<vector<cv::Point> > vContours2; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy2;

	findContours(matSrc, vContours2, vHierarchy2, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
																						  //	findContours(matSrc, vContours, vHierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

	cv::Rect rtArea2 = cv::Rect(0, 0, 0, 0);
	for (int i = 0; i < (int)vContours2.size(); i++)
	{
		// iterate through each contour. 
		double dArea2 = contourArea(vContours2[i], false);		// Find the area of contour

		if (dArea2 > dLargest_Area2) // ���� ū��
		{
			dSecondLargest_Area2 = dLargest_Area2;
			dLargest_Area2 = (int)dArea2;

			nSecondLargest_Contour_Index2 = nLargest_Contour_Index2;
			nLargest_Contour_Index2 = i;							// Store the index of largest contour

			rtArea2 = boundingRect(vContours2[nSecondLargest_Contour_Index2]);				// Find the bounding rectangle for biggest contour
		}


	}

	drawContours(matDstActive, vContours2, nSecondLargest_Contour_Index2, Scalar(255), 2, 8, vHierarchy2);


	cv::floodFill(matDstActive, cv::Point(matDstActive.cols / 2, matDstActive.rows / 2), cv::Scalar(255));


	cv::bitwise_not(matDstActive, matDstActive);

	MaximumFilter(matDstActive, matDstActive, 11);
}



void InspMainSequence::MakeActiveMask(cv::Mat& matSrc, cv::Mat& matDstActive, int nThreshActive)
{

	if (matDstActive.empty())
		matDstActive = cv::Mat::zeros(matSrc.size(), matSrc.type());

	// ���� ū ����(Active ������ ã��)
	double dLargest_Area = 0;
	int nLargest_Contour_Index = 0;

	vector<vector<cv::Point> > vContours; // Vector for storing contour
	vector<cv::Vec4i> vHierarchy;

	findContours(matSrc, vContours, vHierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

	cv::Rect rtArea = cv::Rect(0, 0, 0, 0);
	for (int i = 0; i < (int)vContours.size(); i++)
	{
		// iterate through each contour. 
		double dArea = contourArea(vContours[i], false);		// Find the area of contour
		if (dArea > dLargest_Area)
		{
			dLargest_Area = (int)dArea;
			nLargest_Contour_Index = i;							// Store the index of largest contour

			rtArea = boundingRect(vContours[i]);				// Find the bounding rectangle for biggest contour
		}
	}

	drawContours(matDstActive, vContours, nLargest_Contour_Index, Scalar(255), CV_FILLED, 8, vHierarchy); // Draw the largest contour using previously stored index.
}

void InspMainSequence::MaximumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	int nMaskX_ST, nMaskY_ST, nMask_ST;

	nMask_ST = nMaskSize / 2 * -1;
	int nMax;

	uchar* ucSrcdata;
	uchar* ucDstdata;

	for (int nY = 0; nY < nRows; nY++)
	{
		if (nY + nMask_ST < 0 || nY + abs(nMask_ST) > nRows - 1)
			continue;
		ucDstdata = prc.ptr(nY);
		for (int nX = 0; nX < nCols; nX++)
		{
			if (nX + nMask_ST < 0 || nX + nMask_ST > nCols - 1)
				continue;

			nMax = 0;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] > nMax)
						nMax = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMax;
		}
	}

	prc.copyTo(dst);
}

void InspMainSequence::MinimumFilter(cv::Mat src, cv::Mat& dst, int nMaskSize)
{
	Mat prc;
	src.copyTo(prc);

	if (nMaskSize % 2 == 0)
		nMaskSize++;

	int nCols = src.cols;
	int nRows = src.rows;
	int nStep = src.step;

	int nMaskX_ST, nMaskY_ST, nMask_ST;

	nMask_ST = nMaskSize / 2 * -1;
	int nMin;

	uchar* ucSrcdata;
	uchar* ucDstdata;

	for (int nY = 0; nY < nRows; nY++)
	{
		if (nY + nMask_ST < 0 || nY + abs(nMask_ST) > nRows - 1)
			continue;
		ucDstdata = prc.ptr(nY);
		for (int nX = 0; nX < nCols; nX++)
		{
			if (nX + nMask_ST < 0 || nX + nMask_ST > nCols - 1)
				continue;

			nMin = 255;

			nMaskY_ST = nMask_ST;
			for (int nMaskY = 0; nMaskY < nMaskSize; nMaskY++)
			{
				int nY_ = nY + nMaskY_ST;
				ucSrcdata = src.ptr(nY_);

				nMaskX_ST = nMask_ST;
				for (int nMaskX = 0; nMaskX < nMaskSize; nMaskX++)
				{
					int nX_ = nX + nMaskX_ST;
					if ((int)ucSrcdata[nX_] < nMin)
						nMin = (int)ucSrcdata[nX_];
					nMaskX_ST++;
				}
				nMaskY_ST++;
			}
			ucDstdata[nX] = (uchar)nMin;
		}
	}

	prc.copyTo(dst);
}

void InspMainSequence::Contrast(cv::Mat& matSrcImage, double dContrast_Max, double dContrast_Min, int nBrightness_Max, int nBrightness_Min)
{

	double dAvg = cv::mean(matSrcImage, matSrcImage > 0)[0];

	int nMax = (int)(dAvg * (dContrast_Max));
	int nMin = (int)(dAvg * (dContrast_Min));
	if (nMax > 255)	nMax = 240;
	if (nMin < 0)	nMin = 0;

	float LUT[256] = { 0, };
	for (int i = 0; i < 256; i++)
	{
		if (i < nMin)		LUT[i] = nMin;
		else if (i > nMax)	LUT[i] = nMax;
		else				LUT[i] = i;
	}

	//Vec3f������ �����ϸ� ������?...��������..
	cv::MatIterator_<uchar> itSrc, endSrc;
	itSrc = matSrcImage.begin<uchar>();
	endSrc = matSrcImage.end<uchar>();

	for (; itSrc != endSrc; itSrc++)
	{
		(*itSrc) = LUT[((uchar)*itSrc)];
	}

	int nSize = 5;
	cv::blur(matSrcImage, matSrcImage, cv::Size(nSize, nSize));

	///////////////////////////////////////////////////////
	// Min, Max

	nMax += nBrightness_Max;
	nMin -= nBrightness_Min;

	double dVal = 255.0 / (nMax - nMin);
	cv::subtract(matSrcImage, nMin, matSrcImage);
	cv::multiply(matSrcImage, dVal, matSrcImage);


}

long InspMainSequence::RangeAvgThreshold_DUST(cv::Mat& matSrcImage, cv::Mat& matDstImage)
{
	long	nBlurLoop_5x5 = 2;
	long	nSegX = 200;
	long	nSegY = 200;
	long	nEdgeArea = 1;

	float	fActiveDarkRatio = 0.0;
	float	fActiveBrightRatio = 1.3;
	float	fEdgeDarkRatio = 0.0;
	float	fEdgeBrightRatio = 1.3;

	int		nMinThGV =5;
	int		nMedian = 11;

	// ���� ���� 10���� ���� ���.
	if (nSegX <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;
	if (nSegY <= 10)		return E_ERROR_CODE_POINT_WARNING_PARA;

	// �̹��� ũ��
	long	nWidth = (long)matSrcImage.cols;	// ���� ���� ũ��
	long	nHeight = (long)matSrcImage.rows;	// ���� ���� ũ��

	long nStart_Y, nEnd_Y;

	// Active ������ �˻�
	int nRangeX = matSrcImage.cols / nSegX + 1;
	int nRangeY = matSrcImage.rows / nSegY ;


	// Temp �޸� �Ҵ�
	cv::Mat matBlurBuf;
	cv::Mat matBlurBuf1;

	// Range Avg 
	cv::Mat matAvgBuf = Mat::zeros(cv::Size(nRangeX,nRangeY),matSrcImage.type());

	int nBlur = 5;
	//Insp_RectSet(rtInspROI, rectROI, matSrcImage.cols, matSrcImage.rows, nBlur);

	CRect rectROI = CRect(0,0,matSrcImage.cols-1,matSrcImage.rows-1);

	if (nBlurLoop_5x5 > 0)
 	{
 		cv::blur(matSrcImage, matBlurBuf, cv::Size(nBlur, nBlur));
 
 		if (nBlurLoop_5x5 > 1)
 		{
 			// Avg
 			for (int i = 1; i < nBlurLoop_5x5; i++)
 			{
 				cv::blur(matBlurBuf, matBlurBuf1, cv::Size(nBlur, nBlur));
 
 				matBlurBuf1.copyTo(matBlurBuf);
 			}
 		}
 	}
 	else
 	{
		matSrcImage.copyTo(matBlurBuf);
	}
	matBlurBuf1.release();

	// Range Rect
	cv::Rect *rectRange = new cv::Rect[nRangeX * nRangeY];
	memset(rectRange, 0, sizeof(cv::Rect) * nRangeX * nRangeY);

	// ���� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

		// y ���� ���
		if (y < nRangeY - nEdgeArea)
		{
			nStart_Y = y * nSegY + rectROI.top;

			if (y == nRangeY - 1 - nEdgeArea)		nEnd_Y = rectROI.bottom - nEdgeArea * nSegY;
			else							nEnd_Y = nStart_Y + nSegY;
		}
		else
		{
			nStart_Y = rectROI.bottom - (nRangeY - y) * nSegY;
			nEnd_Y = nStart_Y + nSegY;
		}

		for (long x = 0; x < nRangeX; x++)
		{
			long nStart_X, nEnd_X;

			// x ���� ���
			if (x < nRangeX - nEdgeArea)
			{
				nStart_X = x * nSegX + rectROI.left;

				if (x == nRangeX - 1 - nEdgeArea)		nEnd_X = rectROI.right - nEdgeArea * nSegX;
				else							nEnd_X = nStart_X + nSegX;
			}
			else
			{
				nStart_X = rectROI.right - (nRangeX - x) * nSegX;
				nEnd_X = nStart_X + nSegX;
			}

			// ���� ����
			rectPtr[x].x = nStart_X;
			rectPtr[x].y = nStart_Y;
			rectPtr[x].width = nEnd_X - nStart_X + 1;
			rectPtr[x].height = nEnd_Y - nStart_Y;
		}
	}

	// ��� ���
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// mean & stdDev ���ϱ�
			//	cv::Scalar m, s;
			//	cv::meanStdDev(matTempBuf, m, s);

			// ������׷�
			cv::Mat matHisto;
			GetHistogram(matTempBuf, matHisto);

			if (matHisto.empty()) {
				int a = 1 + 1;
			}
			double dblAverage;
			double dblStdev;
			GetMeanStdDev_From_Histo(matHisto, 0, 255, dblAverage, dblStdev);

			// ��� ���� ����
			//	int nMinGV = (int)(m[0] - s[0]);
			//	int nMaxGV = (int)(m[0] + s[0]);
			int nMinGV = (int)(dblAverage - dblStdev);
			int nMaxGV = (int)(dblAverage + dblStdev);
			if (nMinGV < 0)	nMinGV = 0;
			if (nMaxGV > 255)	nMaxGV = 255;

			// �ʱ�ȭ
			__int64 nPixelSum = 0;
			__int64 nPixelCount = 0;

			// ������ ��� ������ŭ�� �����
			float* pVal = (float*)matHisto.ptr(0) + nMinGV;

			for (int m = nMinGV; m <= nMaxGV; m++, pVal++)
			{
				nPixelSum += (m * *pVal);
				nPixelCount += *pVal;
			}

			// ������ �ϳ��� �־��...
			if (nPixelCount > 0)
				ptr[x] = (BYTE)(nPixelSum / (float)nPixelCount);
		}
	}

	// �ֺ� ��հ� -> �߰������� ����
	MedianFilter(matAvgBuf, matAvgBuf, nMedian);

	// ����ȭ
	for (long y = 0; y < nRangeY; y++)
	{
		// Range Avg
		BYTE *ptr = (BYTE *)matAvgBuf.ptr(y);

		// Range Rect
		cv::Rect *rectPtr = &rectRange[nRangeX * y];

#ifdef _DEBUG
#else
#pragma omp parallel for  num_threads(2)
#endif
		for (long x = 0; x < nRangeX; x++)
		{
			// ���� ROI
			cv::Mat matTempBuf = matBlurBuf(rectPtr[x]);

			// ��� * Ratio
			long nDarkTemp, nBrightTemp;

			// Edge ���� ���� �ϱ�
			if (x < nEdgeArea ||
				y < nEdgeArea ||
				x >= nRangeX - nEdgeArea ||
				y >= nRangeY - nEdgeArea)
			{
				nDarkTemp = (long)(ptr[x] * fEdgeDarkRatio);
				nBrightTemp = (long)(ptr[x] * fEdgeBrightRatio);
			}
			else
			{
				nDarkTemp = (long)(ptr[x] * fActiveDarkRatio);
				nBrightTemp = (long)(ptr[x] * fActiveBrightRatio);
			}

			// Gray �ʹ� ��ο� ��� ����.
			// ( ��� GV 2 ~ 3 * fBrightRatio -> ����ȭ : ������ �� �ö�� )
			// Dust���� ���� ���� ���� ã�� �� �� ������ 0�� �� �۵� �ȵǰ� ��
			if (nMinThGV != 0)
			{
				if (nBrightTemp < nMinThGV)	nBrightTemp = nMinThGV;
			}

			// ���� ó��
			if (nDarkTemp < 0)		nDarkTemp = 0;
			if (nBrightTemp > 255)	nBrightTemp = 255;

			// �Ķ���� 0�ΰ�� ���� ó��
			if (fActiveDarkRatio == 0)		nDarkTemp = -1;
			if (fActiveBrightRatio == 0)	nBrightTemp = 256;

			// E_DEFECT_COLOR_DARK Threshold


			// E_DEFECT_COLOR_BRIGHT Threshold
			cv::Mat matTempBufT = matDstImage(rectPtr[x]);
			cv::threshold(matTempBuf, matTempBufT, nBrightTemp, 255.0, THRESH_BINARY);
		}
	}

	// ����
	matAvgBuf.release();
	delete[] rectRange;
	rectRange = NULL;

	return E_ERROR_CODE_TRUE;
}

long InspMainSequence::GetHistogram(cv::Mat& matSrcImage, cv::Mat& matHisto)
{
	// ���۰� ���� ���.
	if (matSrcImage.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	int nHistSize = 0;
	if (matSrcImage.type() == CV_8U)
		nHistSize = 256;
	else
		nHistSize = 4096;

	float fHistRange[] = { (float)0, (float)nHistSize - 1 };
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage, 1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}

long InspMainSequence::GetMeanStdDev_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage, double &dblStdev)
{

	GetAverage_From_Histo(matHisto, nMin, nMax, dblAverage);

	double dblValSum = 0;
	double dblCountSum = 0;
	double dblTmp;
	float *pVal = (float*)matHisto.data;

	// �л��� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblTmp = (i - dblAverage);
		dblTmp = dblTmp*dblTmp;
		dblValSum += (*pVal * dblTmp);
		dblCountSum += *pVal;
		pVal++;
	}

	dblStdev = dblValSum / dblCountSum; ;
	dblStdev = sqrt(dblStdev);

	return E_ERROR_CODE_TRUE;
}

long InspMainSequence::GetAverage_From_Histo(cv::Mat matHisto, int nMin, int nMax, double &dblAverage)
{
	
	double dblValSum = 0;
	double dblCountSum = 0;
	float *pVal = (float*)matHisto.data;

	// ����� ���Ѵ�.
	for (int i = nMin; i < nMax; i++)
	{
		dblValSum += (*pVal * i);
		dblCountSum += *pVal;
		pVal++;
	}

	dblAverage = dblValSum / dblCountSum;

	return E_ERROR_CODE_TRUE;
}

long InspMainSequence::MedianFilter(cv::Mat& matSrcBuf, cv::Mat& matDstBuf, int nKSize)
{
	if (matSrcBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;
	if (matDstBuf.empty())		return E_ERROR_CODE_EMPTY_BUFFER;

	// ���� �ڵ�
	long	nErrorCode = E_ERROR_CODE_TRUE;

	if (matSrcBuf.type() == CV_8U)
	{
		cv::medianBlur(matSrcBuf, matDstBuf, nKSize);
	}
	else
	{
		cv::Mat matTempBuf = Mat::zeros(matSrcBuf.size(), matSrcBuf.type());

		int nHSize = nKSize / 2;
		ushort *nArr = new ushort[nKSize * nKSize];
		memset(nArr, 0, sizeof(ushort) * nKSize * nKSize);

		for (int yy = 0; yy < matSrcBuf.rows; yy++)
		{
			ushort *ptrDst = (ushort *)matTempBuf.ptr(yy);

			for (int xx = 0; xx < matSrcBuf.cols; xx++, ptrDst++)
			{
				// ���� Ȯ��
				int nSX = xx - nHSize;
				int nEX = xx + nHSize;
				int nSY = yy - nHSize;
				int nEY = yy + nHSize;

				// ���� ó��
				nSX = (nSX < 0) ? 0 : nSX;
				nSY = (nSY < 0) ? 0 : nSY;
				nEX = (nEX >= matSrcBuf.cols) ? matSrcBuf.cols - 1 : nEX;
				nEY = (nEY >= matSrcBuf.rows) ? matSrcBuf.rows - 1 : nEY;

				int nCount = 0;

				// ������ �ֱ�
				for (int nMy = nSY; nMy <= nEY; nMy++)
				{
					ushort *ptrSrc = (ushort *)matSrcBuf.ptr(nMy) + nSX;

					for (int nMx = nSX; nMx <= nEX; nMx++, ptrSrc++)
					{
						nArr[nCount++] = *ptrSrc;
					}
				}

				// ����
				for (int i = 0; i < nCount - 1; i++)
				{
					for (int j = i + 1; j < nCount; j++)
					{
						if (nArr[i] > nArr[j])
						{
							ushort nTemp = nArr[i];
							nArr[i] = nArr[j];
							nArr[j] = nTemp;
						}
					}
				}

				// ���
				*ptrDst = nArr[nCount / 2];
			}
		}

		delete[] nArr;
		nArr = NULL;

		// ��� ����
		matTempBuf.copyTo(matDstBuf);

		// �޸� ����
		matTempBuf.release();

	}

	return nErrorCode;
}

long InspMainSequence::Test()
{
	String Folder_Path = "C:\\Users\\chvx1\\OneDrive\\���� ȭ��\\�Ե��ɹ�Į\\���� - ���纻\\";

	cv::Mat matSide_Result = cv::imread(Folder_Path + "side_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matCoaxial_Result = cv::imread(Folder_Path + "co_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matDust_Result = cv::imread(Folder_Path + "Dust_blur_result.bmp", IMREAD_UNCHANGED);

	cv::Mat matGrayBuffer_Coaxial = cv::imread(Folder_Path + "����.bmp", IMREAD_UNCHANGED);
	cv::Mat matGrayBuffer_Side = cv::imread(Folder_Path + "���̵�.bmp", IMREAD_UNCHANGED);
	cv::Mat matGrayBuffer_Dust = cv::imread(Folder_Path + "����Ʈ.bmp", IMREAD_UNCHANGED);

	cv::Mat matDrawBuffer_Coaxia;
	cv::Mat matDrawBuffer_Side;
	cv::Mat matDrawBuffer_Dust;
	cv::Mat matDrawBuffer_Coaxia_Result;
	cv::Mat matDrawBuffer_Coaxia_Final_Result;

	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia);
	matGrayBuffer_Side.copyTo(matDrawBuffer_Side);
	matGrayBuffer_Dust.copyTo(matDrawBuffer_Dust);
	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia_Result);
	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia_Final_Result);

	//////////////////////////////////////////////////////////////////////////
	//Dust ��� ����
	//////////////////////////////////////////////////////////////////////////
	// distanceTransform�� �̿��Ͽ� mask �ܰ����� 0~ �߽ɱ����Ÿ��� ���� ��ȯ �� ��ȯ�� �ȼ� �� ��ŭ Threshold�Ͽ� �ܰ� ����
	int Dt_offset = 3;

	cv::bitwise_not(matDust_Result, matDust_Result);
	cv::distanceTransform(matDust_Result, matDust_Result, CV_DIST_L2, 3);
	
	cv::threshold(matDust_Result, matDust_Result, Dt_offset, 255, CV_THRESH_BINARY);
	
	matDust_Result.convertTo(matDust_Result, CV_8UC1);
	cv::bitwise_not(matDust_Result, matDust_Result);

	cv::imwrite(Folder_Path + "ARESULT\\Dust_Mask.png", matDust_Result);

	matCoaxial_Result -= matDust_Result;

	cv::imwrite(Folder_Path + "ARESULT\\Coaxia_Sub.png", matCoaxial_Result);
	//////////////////////////////////////////////////////////////////////////

	__int64 nTotalLabel_Coaxial = 0;
	cv::Mat matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial;
	matLabel_Coaxial = Mat().zeros(matCoaxial_Result.size(), CV_32SC1);

	nTotalLabel_Coaxial = cv::connectedComponentsWithStats(matCoaxial_Result, matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial, 8, CV_32S, CCL_GRANA) - 1;

	__int64 nTotalLabel_Side = 0;
	cv::Mat matLabel_Side, matStats_Side, matCentroid_Side;
	matLabel_Side = Mat().zeros(matSide_Result.size(), CV_32SC1);

	nTotalLabel_Side = cv::connectedComponentsWithStats(matSide_Result, matLabel_Side, matStats_Side, matCentroid_Side, 8, CV_32S, CCL_GRANA) - 1;

// 	__int64 nTotalLabel_Dust = 0;
// 	cv::Mat matLabel_Dust, matStats_Dust, matCentroid_Dust;
// 	matLabel_Dust = Mat().zeros(matDust_Result.size(), CV_32SC1);
// 
// 	nTotalLabel_Dust = cv::connectedComponentsWithStats(matDust_Result, matLabel_Dust, matStats_Dust, matCentroid_Dust, 8, CV_32S, CCL_GRANA) - 1;

	DoFeatureBasic_8bit(matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial, matGrayBuffer_Coaxial, (int)nTotalLabel_Coaxial, m_BlobResult_Coaxial);
	DoFeatureBasic_8bit(matLabel_Side, matStats_Side, matCentroid_Side, matGrayBuffer_Side, (int)nTotalLabel_Side, m_BlobResult_Side);
//	DoFeatureBasic_8bit(matLabel_Dust, matStats_Dust, matCentroid_Dust, matGrayBuffer_Dust, (int)nTotalLabel_Dust, m_BlobResult_Dust);
	
	// �ҷ� ��ȣ �׸���
	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Coaxial.size();  nDefectCount++) {
		cv::rectangle(matDrawBuffer_Coaxia, cv::Rect(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x - 2, m_BlobResult_Coaxial.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x , m_BlobResult_Coaxial.at(nDefectCount).rectBox.y-10);
		cv::Rect rect = cv::Rect(
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.width ) + (5 * 2),
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.height ) + (5 * 2));

		cv::rectangle(matDrawBuffer_Coaxia, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Coaxia, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Coaxia_Label.png", matDrawBuffer_Coaxia);

	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Side.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Side, cv::Rect(m_BlobResult_Side.at(nDefectCount).rectBox.x - 2, m_BlobResult_Side.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Side.at(nDefectCount).rectBox.x, m_BlobResult_Side.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Side.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Side.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Side.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Side.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Side, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Side, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Side_Label.png", matDrawBuffer_Side);

// 	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Dust.size(); nDefectCount++) {
// 		cv::rectangle(matDrawBuffer_Dust, cv::Rect(m_BlobResult_Dust.at(nDefectCount).rectBox.x - 2, m_BlobResult_Dust.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);
// 
// 		char str[256] = { 0, };
// 		CString strTxt = _T("");
// 
// 		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
// 		cv::Point ptRT(m_BlobResult_Dust.at(nDefectCount).rectBox.x, m_BlobResult_Dust.at(nDefectCount).rectBox.y - 10);
// 
// 		cv::Rect rect = cv::Rect(
// 			(m_BlobResult_Dust.at(nDefectCount).rectBox.x) - 5,
// 			(m_BlobResult_Dust.at(nDefectCount).rectBox.y) - 5,
// 			(m_BlobResult_Dust.at(nDefectCount).rectBox.width) + (5 * 2),
// 			(m_BlobResult_Dust.at(nDefectCount).rectBox.height) + (5 * 2));
// 
// 		cv::rectangle(matDrawBuffer_Dust, rect, Scalar(255, 255, 255), 1);
// 		cv::putText(matDrawBuffer_Dust, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
// 	}
// 
// 	cv::imwrite(Folder_Path + "ARESULT\\Dust_Label.png", matDrawBuffer_Dust);
	///////////////////////////////////////////////////////////////////////////
	//���� ���̵� Defect ��ġ ��
	//////////////////////////////////////////////////////////////////////////
	int nOffSet = 10;

	for (int i = 0; i < m_BlobResult_Coaxial.size(); i++)
	{
		CRect rectM = CRect(
			(m_BlobResult_Coaxial.at(i).rectBox.x) - nOffSet,
			(m_BlobResult_Coaxial.at(i).rectBox.y) - nOffSet,
			(m_BlobResult_Coaxial.at(i).rectBox.width + m_BlobResult_Coaxial.at(i).rectBox.x) + nOffSet,
			(m_BlobResult_Coaxial.at(i).rectBox.height + m_BlobResult_Coaxial.at(i).rectBox.y) + nOffSet);

		for (int j = 0; j <m_BlobResult_Side.size(); j++)
		{
			CRect rectS = CRect(
				(m_BlobResult_Side.at(j).rectBox.x) - nOffSet,
				(m_BlobResult_Side.at(j).rectBox.y) - nOffSet,
				(m_BlobResult_Side.at(j).rectBox.width + m_BlobResult_Side.at(j).rectBox.x) + nOffSet,
				(m_BlobResult_Side.at(j).rectBox.height + m_BlobResult_Side.at(j).rectBox.y) + nOffSet);

			// �ߺ� ���� Ȯ��
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// ���� �ȿ� ������ ���� ����
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				m_BlobResult_Result.push_back(m_BlobResult_Coaxial[i]);
				break;
			}
		}
	}

	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Coaxia_Result, cv::Rect(m_BlobResult_Result.at(nDefectCount).rectBox.x - 2, m_BlobResult_Result.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Result.at(nDefectCount).rectBox.x, m_BlobResult_Result.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Result.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Result.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Coaxia_Result, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Coaxia_Result, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Coaxia_Label_Result.png", matDrawBuffer_Coaxia_Result);

	//////////////////////////////////////////////////////////////////////////
	//���� ���̵� ��� + Dust ��
	//////////////////////////////////////////////////////////////////////////

// 	int nOffSet2 = 3;
// 
// 	for (int i = 0; i < m_BlobResult_Result.size(); i++)
// 	{
// 		CRect rectM = CRect(
// 			(m_BlobResult_Result.at(i).rectBox.x) - nOffSet2,
// 			(m_BlobResult_Result.at(i).rectBox.y) - nOffSet2,
// 			(m_BlobResult_Result.at(i).rectBox.width + m_BlobResult_Result.at(i).rectBox.x) + nOffSet2,
// 			(m_BlobResult_Result.at(i).rectBox.height + m_BlobResult_Result.at(i).rectBox.y) + nOffSet2);
// 
// 		for (int j = 0; j < m_BlobResult_Dust.size(); j++)
// 		{
// 			CRect rectS = CRect(
// 				(m_BlobResult_Dust.at(j).rectBox.x) - nOffSet2,
// 				(m_BlobResult_Dust.at(j).rectBox.y) - nOffSet2,
// 				(m_BlobResult_Dust.at(j).rectBox.width + m_BlobResult_Dust.at(j).rectBox.x) + nOffSet2,
// 				(m_BlobResult_Dust.at(j).rectBox.height + m_BlobResult_Dust.at(j).rectBox.y) + nOffSet2);
// 
// 			// �ߺ� ���� Ȯ��
// 			CRect rectRes;
// 			IntersectRect(rectRes, rectM, rectS);
// 
// 			// ���� �ȿ� ������ ���� ����
// 			if ((rectRes.Width() + rectRes.Height()) > 0)
// 			{
// 				m_BlobResult_Result.erase(m_BlobResult_Result.begin() + i);
// 				i--;
// 				break;
// 			}
// 		}
// 	}
// 
// 	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
// 		cv::rectangle(matDrawBuffer_Coaxia_Final_Result, cv::Rect(m_BlobResult_Result.at(nDefectCount).rectBox.x - 2, m_BlobResult_Result.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);
// 
// 		char str[256] = { 0, };
// 		CString strTxt = _T("");
// 
// 		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
// 		cv::Point ptRT(m_BlobResult_Result.at(nDefectCount).rectBox.x, m_BlobResult_Result.at(nDefectCount).rectBox.y - 10);
// 
// 		cv::Rect rect = cv::Rect(
// 			(m_BlobResult_Result.at(nDefectCount).rectBox.x) - 5,
// 			(m_BlobResult_Result.at(nDefectCount).rectBox.y) - 5,
// 			(m_BlobResult_Result.at(nDefectCount).rectBox.width) + (5 * 2),
// 			(m_BlobResult_Result.at(nDefectCount).rectBox.height) + (5 * 2));
// 
// 		cv::rectangle(matDrawBuffer_Coaxia_Final_Result, rect, Scalar(0, 0, 0), 1);
// 		cv::putText(matDrawBuffer_Coaxia_Final_Result, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
// 	}
// 
// 	cv::imwrite(Folder_Path + "ARESULT\\Coaxia_Label_Final_Result.png", matDrawBuffer_Coaxia_Final_Result);
	///////////////////////////////////////////////////////////////////////////
	//�÷� ���� ã��
	//////////////////////////////////////////////////////////////////////////

	float fDegree = 0.f;
	cv::Mat matRotate;
	matRotate = Mat().zeros(matGrayBuffer_Coaxial.size(), matGrayBuffer_Coaxial.type());

	RotatedRect rRotate;

	Align_Test(matGrayBuffer_Coaxial, matRotate ,&fDegree, rRotate);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//��ǥ ȸ��
	//////////////////////////////////////////////////////////////////////////
	double dTheta = -fDegree * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	int		nCx = rRotate.center.x;				// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy = rRotate.center.y;

	int X = 0;
	int Y = 0;

// 	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
// 		X = (int)(dCos * (m_BlobResult_Result[nDefectCount].rectBox.x - nCx) - dSin * (m_BlobResult_Result[nDefectCount].rectBox.y - nCy) + nCx);
// 		Y = (int)(dSin * (m_BlobResult_Result[nDefectCount].rectBox.x - nCx) + dCos * (m_BlobResult_Result[nDefectCount].rectBox.y - nCy) + nCy);
// 
// 		m_BlobResult_Result[nDefectCount].rectBox.x = X;
// 		m_BlobResult_Result[nDefectCount].rectBox.y = Y;
// 	}

	for (int i = 0; i < m_BlobResult_Result.size(); i++)
	{

		int nL, nT, nR, nB;

		// ȸ�� ��, ���� ��ǥ ���
		int X = (int)(dCos * (m_BlobResult_Result[i].ptContours[0].x - nCx) - dSin * (m_BlobResult_Result[i].ptContours[0].y - nCy) + nCx);
		int Y = (int)(dSin * (m_BlobResult_Result[i].ptContours[0].x - nCx) + dCos * (m_BlobResult_Result[i].ptContours[0].y - nCy) + nCy);

		// �ʱ� ����
		nL = nR = X;
		nT = nB = Y;

		// �ܰ��� ���� ��ŭ
		for (int k = 1; k < m_BlobResult_Result[i].ptContours.size(); k++)
		{
			// ȸ�� ��, ���� ��ǥ ���
			X = (int)(dCos * (m_BlobResult_Result[i].ptContours[k].x - nCx) - dSin * (m_BlobResult_Result[i].ptContours[k].y - nCy) + nCx);
			Y = (int)(dSin * (m_BlobResult_Result[i].ptContours[k].x - nCx) + dCos * (m_BlobResult_Result[i].ptContours[k].y - nCy) + nCy);

			// ����
			if (nL > X)	nL = X;
			if (nR < X)	nR = X;
			if (nT > Y)	nT = Y;
			if (nB < Y)	nB = Y;
		}

		cv::Point ptTemp;

		m_BlobResult_Result[i].rectBox.x = nL;
		m_BlobResult_Result[i].rectBox.y = nT;
	

		m_BlobResult_Result[i].rectBox.width = nR-nL;
		m_BlobResult_Result[i].rectBox.height = nB-nT;
		

	}


	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
		cv::rectangle(matRotate, cv::Rect(m_BlobResult_Result.at(nDefectCount).rectBox.x - 2, m_BlobResult_Result.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Result.at(nDefectCount).rectBox.x, m_BlobResult_Result.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Result.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Result.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matRotate, rect, Scalar(0, 0, 0), 1);
		cv::putText(matRotate, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Rotate_Label_Result.png", matRotate);
	//////////////////////////////////////////////////////////////////////////
	
	
	
	return true;
}

long InspMainSequence::Test2()
{
	String Folder_Path = "C:\\Users\\chvx1\\OneDrive\\���� ȭ��\\�Ե��ɹ�Į\\����2\\";

	cv::Mat matSide_Result = cv::imread(Folder_Path + "side_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matCoaxial_Result = cv::imread(Folder_Path + "co_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matDust_Result = cv::imread(Folder_Path + "Dust_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matBack_Result = cv::imread(Folder_Path + "back_blur_result.bmp", IMREAD_UNCHANGED);

	cv::Mat matGrayBuffer_Coaxial = cv::imread(Folder_Path + "����.bmp", IMREAD_GRAYSCALE);
// 	cv::Mat matGrayBuffer_Side = cv::imread(Folder_Path + "���̵�.bmp", IMREAD_UNCHANGED);
// 	cv::Mat matGrayBuffer_Dust = cv::imread(Folder_Path + "����Ʈ.bmp", IMREAD_UNCHANGED);
	cv::Mat matGrayBuffer_Back = cv::imread(Folder_Path + "�ĸ�.bmp", IMREAD_GRAYSCALE);

	cv::Mat matDrawBuffer_Coaxia;
// 	cv::Mat matDrawBuffer_Side;
// 	cv::Mat matDrawBuffer_Dust;
	cv::Mat matDrawBuffer_Back;

	cv::Mat matDrawBuffer_Coaxia_Result;
	cv::Mat matDrawBuffer_Coaxia_Final_Result;

	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia);
// 	matGrayBuffer_Side.copyTo(matDrawBuffer_Side);
// 	matGrayBuffer_Dust.copyTo(matDrawBuffer_Dust);
	matGrayBuffer_Back.copyTo(matDrawBuffer_Back);

	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia_Result);
//	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia_Final_Result);

	//////////////////////////////////////////////////////////////////////////
	//���� ���̵� or ����
	//////////////////////////////////////////////////////////////////////////
	cv::Mat matCS_Result;

	cv::bitwise_or(matCoaxial_Result, matSide_Result, matCS_Result);

	cv::imwrite(Folder_Path + "ARESULT\\CS_Src.png", matCS_Result);
	//////////////////////////////////////////////////////////////////////////
	//Dust ��� ����
	//////////////////////////////////////////////////////////////////////////
	// distanceTransform�� �̿��Ͽ� mask �ܰ����� 0~ �߽ɱ����Ÿ��� ���� ��ȯ �� ��ȯ�� �ȼ� �� ��ŭ Threshold�Ͽ� �ܰ� ����
	int Dt_offset = 3;

	cv::bitwise_not(matDust_Result, matDust_Result);
	cv::distanceTransform(matDust_Result, matDust_Result, CV_DIST_L2, 3);

	cv::threshold(matDust_Result, matDust_Result, Dt_offset, 255, CV_THRESH_BINARY);

	matDust_Result.convertTo(matDust_Result, CV_8UC1);
	cv::bitwise_not(matDust_Result, matDust_Result);

	cv::imwrite(Folder_Path + "ARESULT\\Dust_Mask.png", matDust_Result);

	matCS_Result -= matDust_Result;

	cv::imwrite(Folder_Path + "ARESULT\\CS_Sub.png", matCS_Result);
	//////////////////////////////////////////////////////////////////////////

	__int64 nTotalLabel_CS = 0;
	cv::Mat matLabel_CS, matStats_CS, matCentroid_CS;
	matLabel_CS = Mat().zeros(matCS_Result.size(), CV_32SC1);

	nTotalLabel_CS = cv::connectedComponentsWithStats(matCS_Result, matLabel_CS, matStats_CS, matCentroid_CS, 8, CV_32S, CCL_GRANA) - 1;

	__int64 nTotalLabel_Back = 0;
	cv::Mat matLabel_Back, matStats_Back, matCentroid_Back;
	matLabel_Back = Mat().zeros(matBack_Result.size(), CV_32SC1);

	nTotalLabel_Back = cv::connectedComponentsWithStats(matBack_Result, matLabel_Back, matStats_Back, matCentroid_Back, 8, CV_32S, CCL_GRANA) - 1;

	// 	__int64 nTotalLabel_Dust = 0;
	// 	cv::Mat matLabel_Dust, matStats_Dust, matCentroid_Dust;
	// 	matLabel_Dust = Mat().zeros(matDust_Result.size(), CV_32SC1);
	// 
	// 	nTotalLabel_Dust = cv::connectedComponentsWithStats(matDust_Result, matLabel_Dust, matStats_Dust, matCentroid_Dust, 8, CV_32S, CCL_GRANA) - 1;

	DoFeatureBasic_8bit(matLabel_CS, matStats_CS, matCentroid_CS, matGrayBuffer_Coaxial, (int)nTotalLabel_CS, m_BlobResult_CS);
	DoFeatureBasic_8bit(matLabel_Back, matStats_Back, matCentroid_Back, matGrayBuffer_Back, (int)nTotalLabel_Back, m_BlobResult_Back);
	//	DoFeatureBasic_8bit(matLabel_Dust, matStats_Dust, matCentroid_Dust, matGrayBuffer_Dust, (int)nTotalLabel_Dust, m_BlobResult_Dust);

	// �ҷ� ��ȣ �׸���
	for (int nDefectCount = 0; nDefectCount < m_BlobResult_CS.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Coaxia, cv::Rect(m_BlobResult_CS.at(nDefectCount).rectBox.x - 2, m_BlobResult_CS.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_CS.at(nDefectCount).rectBox.x, m_BlobResult_CS.at(nDefectCount).rectBox.y - 10);
		cv::Rect rect = cv::Rect(
			(m_BlobResult_CS.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_CS.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_CS.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_CS.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Coaxia, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Coaxia, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\CS_Label.png", matDrawBuffer_Coaxia);

	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Back.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Back, cv::Rect(m_BlobResult_Back.at(nDefectCount).rectBox.x - 2, m_BlobResult_Back.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Back.at(nDefectCount).rectBox.x, m_BlobResult_Back.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Back.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Back.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Back.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Back.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Back, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Back, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Back_Label.png", matDrawBuffer_Back);

	
	///////////////////////////////////////////////////////////////////////////
	//���� + ���̵� && �ĸ� Defect ��ġ ��
	//////////////////////////////////////////////////////////////////////////
	int nOffSet = 10;

	for (int i = 0; i < m_BlobResult_CS.size(); i++)
	{
		CRect rectM = CRect(
			(m_BlobResult_CS.at(i).rectBox.x) - nOffSet,
			(m_BlobResult_CS.at(i).rectBox.y) - nOffSet,
			(m_BlobResult_CS.at(i).rectBox.width + m_BlobResult_CS.at(i).rectBox.x) + nOffSet,
			(m_BlobResult_CS.at(i).rectBox.height + m_BlobResult_CS.at(i).rectBox.y) + nOffSet);

		for (int j = 0; j < m_BlobResult_Back.size(); j++)
		{
			CRect rectS = CRect(
				(m_BlobResult_Back.at(j).rectBox.x) - nOffSet,
				(m_BlobResult_Back.at(j).rectBox.y) - nOffSet,
				(m_BlobResult_Back.at(j).rectBox.width + m_BlobResult_Back.at(j).rectBox.x) + nOffSet,
				(m_BlobResult_Back.at(j).rectBox.height + m_BlobResult_Back.at(j).rectBox.y) + nOffSet);

			// �ߺ� ���� Ȯ��
			CRect rectRes;
			IntersectRect(rectRes, rectM, rectS);

			// ���� �ȿ� ������ ���� ����
			if ((rectRes.Width() + rectRes.Height()) > 0)
			{
				m_BlobResult_Result.push_back(m_BlobResult_CS[i]);
				break;
			}
		}
	}

	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Coaxia_Result, cv::Rect(m_BlobResult_Result.at(nDefectCount).rectBox.x - 2, m_BlobResult_Result.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Result.at(nDefectCount).rectBox.x, m_BlobResult_Result.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Result.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Result.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Coaxia_Result, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Coaxia_Result, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\CS_Label_Result.png", matDrawBuffer_Coaxia_Result);

	
	///////////////////////////////////////////////////////////////////////////
	//�÷� ���� ã��
	//////////////////////////////////////////////////////////////////////////

	float fDegree = 0.f;
	cv::Mat matRotate;
	matRotate = Mat().zeros(matGrayBuffer_Coaxial.size(), matGrayBuffer_Coaxial.type());

	RotatedRect rRotate;

	Align_Test(matGrayBuffer_Coaxial, matRotate, &fDegree, rRotate);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//��ǥ ȸ��
	//////////////////////////////////////////////////////////////////////////
	double dTheta = -fDegree * PI / 180.;
	double	dSin = sin(dTheta);
	double	dCos = cos(dTheta);
	double	dSin_ = sin(-dTheta);
	double	dCos_ = cos(-dTheta);

	int		nCx = rRotate.center.x;				// 12 : Cell ȸ���� �߽� x��ǥ
	int		nCy = rRotate.center.y;

	int X = 0;
	int Y = 0;

	// 	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
	// 		X = (int)(dCos * (m_BlobResult_Result[nDefectCount].rectBox.x - nCx) - dSin * (m_BlobResult_Result[nDefectCount].rectBox.y - nCy) + nCx);
	// 		Y = (int)(dSin * (m_BlobResult_Result[nDefectCount].rectBox.x - nCx) + dCos * (m_BlobResult_Result[nDefectCount].rectBox.y - nCy) + nCy);
	// 
	// 		m_BlobResult_Result[nDefectCount].rectBox.x = X;
	// 		m_BlobResult_Result[nDefectCount].rectBox.y = Y;
	// 	}

	for (int i = 0; i < m_BlobResult_Result.size(); i++)
	{

		int nL, nT, nR, nB;

		// ȸ�� ��, ���� ��ǥ ���
		int X = (int)(dCos * (m_BlobResult_Result[i].ptContours[0].x - nCx) - dSin * (m_BlobResult_Result[i].ptContours[0].y - nCy) + nCx);
		int Y = (int)(dSin * (m_BlobResult_Result[i].ptContours[0].x - nCx) + dCos * (m_BlobResult_Result[i].ptContours[0].y - nCy) + nCy);

		// �ʱ� ����
		nL = nR = X;
		nT = nB = Y;

		// �ܰ��� ���� ��ŭ
		for (int k = 1; k < m_BlobResult_Result[i].ptContours.size(); k++)
		{
			// ȸ�� ��, ���� ��ǥ ���
			X = (int)(dCos * (m_BlobResult_Result[i].ptContours[k].x - nCx) - dSin * (m_BlobResult_Result[i].ptContours[k].y - nCy) + nCx);
			Y = (int)(dSin * (m_BlobResult_Result[i].ptContours[k].x - nCx) + dCos * (m_BlobResult_Result[i].ptContours[k].y - nCy) + nCy);

			// ����
			if (nL > X)	nL = X;
			if (nR < X)	nR = X;
			if (nT > Y)	nT = Y;
			if (nB < Y)	nB = Y;
		}

		cv::Point ptTemp;

		m_BlobResult_Result[i].rectBox.x = nL;
		m_BlobResult_Result[i].rectBox.y = nT;


		m_BlobResult_Result[i].rectBox.width = nR - nL;
		m_BlobResult_Result[i].rectBox.height = nB - nT;


	}


	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Result.size(); nDefectCount++) {
		cv::rectangle(matRotate, cv::Rect(m_BlobResult_Result.at(nDefectCount).rectBox.x - 2, m_BlobResult_Result.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Result.at(nDefectCount).rectBox.x, m_BlobResult_Result.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Result.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Result.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Result.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matRotate, rect, Scalar(0, 0, 0), 1);
		cv::putText(matRotate, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));
	}

	cv::imwrite(Folder_Path + "ARESULT\\Rotate_Label_Result.png", matRotate);
	//////////////////////////////////////////////////////////////////////////



	return true;
}

long InspMainSequence::Test3()
{
	String Folder_Path = "C:\\Users\\chvx1\\OneDrive\\���� ȭ��\\�Ե��ɹ�Į\\�� ����\\";

	
	
	cv::Mat matCoaxial_Result = cv::imread(Folder_Path + "co_blur_result.bmp", IMREAD_UNCHANGED);
	cv::Mat matGrayBuffer_Coaxial = cv::imread(Folder_Path + "����.bmp", IMREAD_GRAYSCALE);
	cv::Mat matDrawBuffer_Coaxia;
	matGrayBuffer_Coaxial.copyTo(matDrawBuffer_Coaxia);
	
// 	cv::Mat Temp;
// 	matGrayBuffer_Coaxial.copyTo(Temp);
// 	Temp.setTo(0);

	cv::imwrite(Folder_Path + "ARESULT\\Src.png", matCoaxial_Result);

	__int64 nTotalLabel_Coaxial = 0;
	cv::Mat matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial;
	matLabel_Coaxial = Mat().zeros(matCoaxial_Result.size(), CV_32SC1);

	nTotalLabel_Coaxial = cv::connectedComponentsWithStats(matCoaxial_Result, matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial, 8, CV_32S, CCL_GRANA) - 1;
	DoFeatureBasic_8bit(matLabel_Coaxial, matStats_Coaxial, matCentroid_Coaxial, matGrayBuffer_Coaxial, (int)nTotalLabel_Coaxial, m_BlobResult_Coaxial);
	
// 	cv::Mat Temp;
// 	matGrayBuffer_Coaxial.copyTo(Temp);
// 	Temp.setTo(0);
// 
// 	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Coaxial.size(); nDefectCount++) {
// 		Scalar m, s;
// 
// 		cv::Mat matT;
// 		cv::equalizeHist(matGrayBuffer_Coaxial(m_BlobResult_Coaxial[nDefectCount].rectBox), matT);
// 		cv::meanStdDev(matT, m, s);
// 
// 		int th = m[0] * 0.5;
// 
// 		cv::threshold(matT, Temp(m_BlobResult_Coaxial[nDefectCount].rectBox), th, 255, CV_THRESH_BINARY_INV);
// 	}
// 
// 	cv::imwrite(Folder_Path + "ARESULT\\Temp.png", Temp);
	//�ҷ� ��ȣ �׸���
	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Coaxial.size(); nDefectCount++) {
		cv::rectangle(matDrawBuffer_Coaxia, cv::Rect(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x - 2, m_BlobResult_Coaxial.at(nDefectCount).rectBox.y - 20, 30, 12), cv::Scalar(0, 0, 0), -1);

		char str[256] = { 0, };
		CString strTxt = _T("");

		sprintf_s(str, sizeof(str), "%s%d", LPSTR(LPCTSTR(strTxt)), nDefectCount);
		cv::Point ptRT(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x, m_BlobResult_Coaxial.at(nDefectCount).rectBox.y - 10);

		cv::Rect rect = cv::Rect(
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.x) - 5,
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.y) - 5,
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.width) + (5 * 2),
			(m_BlobResult_Coaxial.at(nDefectCount).rectBox.height) + (5 * 2));

		cv::rectangle(matDrawBuffer_Coaxia, rect, Scalar(0, 0, 0), 1);
		cv::putText(matDrawBuffer_Coaxia, str, ptRT, cv::FONT_HERSHEY_SIMPLEX, 0.4f, cv::Scalar(255, 0, 0));

		
	}

	cv::imwrite(Folder_Path + "ARESULT\\C����Label.png", matDrawBuffer_Coaxia);

	for (int nDefectCount = 0, idx = 1; nDefectCount < m_BlobResult_Coaxial.size(); nDefectCount++, idx++) {
		//////////////////////////////////////////////////////////////////////////
		//CSV
		//////////////////////////////////////////////////////////////////////////
		CString strTemp;

		strTemp.Format(_T("C:\\Users\\chvx1\\OneDrive\\���� ȭ��\\�Ե��ɹ�Į\\�� ����\\ARESULT\\Defect Value.csv"));

		char szPath[MAX_PATH] = { 0, };
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, szPath, sizeof(szPath), NULL, NULL);

		FILE *out = NULL;
		fopen_s(&out, szPath, "a+t");

		if (out)
		{
			if (idx < 2)
			{
				fprintf_s(out, "Defect Info\n");

				fprintf_s(out, "No.,Area,AxisRatio,BoxRatio,Compactness,DiffGVRatio\n");
			}

			//if (nArea > 3)
			fprintf_s(out, "%d,%d,%f,%f,%f,%f\n",  nDefectCount, m_BlobResult_Coaxial[nDefectCount].nArea, m_BlobResult_Coaxial[nDefectCount].fAxisRatio, m_BlobResult_Coaxial[nDefectCount].fBoxRatio, m_BlobResult_Coaxial[nDefectCount].fCompactness, m_BlobResult_Coaxial[nDefectCount].fDiffGVRatio);

			fclose(out);
		}

	}

// 		string ANumber;
// 
// 		ANumber = format("Num : %d", nDefectCount);
	

	//////////////////////////////////////////////////////////////////////////
	//Judge
	//////////////////////////////////////////////////////////////////////////
	for (int nDefectCount = 0; nDefectCount < m_BlobResult_Coaxial.size(); nDefectCount++) {

		if (
			(m_BlobResult_Coaxial[nDefectCount].nArea >= 100 && m_BlobResult_Coaxial[nDefectCount].fBoxRatio < 0.31) ||
			(m_BlobResult_Coaxial[nDefectCount].fAxisRatio > 3.0 && m_BlobResult_Coaxial[nDefectCount].fCompactness > 5.0)||
			m_BlobResult_Coaxial[nDefectCount].nArea <= 1)
		{
			matCoaxial_Result(m_BlobResult_Coaxial[nDefectCount].rectBox).setTo(0);
		}
	}

	cv::imwrite(Folder_Path + "ARESULT\\Result.png", matCoaxial_Result);

	

// 	vector<Vec4i> linesP;
// 	cv::HoughLinesP(matCoaxial_Result, linesP, 1, (CV_PI / 180), 5, 50, 50);
// 
// 	for (size_t i = 0; i < linesP.size(); i++)
// 	{
// 		Vec4i l = linesP[i];
// 
// 			
// 		line(matCoaxial_Result, Point(l[0], l[1]), Point(l[2], l[3]), Scalar::all(255), 2, 8);
// 		
// 	}
// 
// 
// 	cv::imwrite(Folder_Path + "ARESULT\\Hough_Result.png", matCoaxial_Result);

	return true;
}

bool InspMainSequence::DoFeatureBasic_8bit(cv::Mat& matLabel, cv::Mat& matStats, cv::Mat& matCentroid, cv::Mat& GrayBuffer, int nTotalLabel, vector<tBLOB_FEATURE>& m_BlobResult)
{
	// ����� 1���� �ִ� ���
	if (nTotalLabel <= 0)	return true;

// 	CMatBuf cMatBufTemp;
// 	cMatBufTemp.SetMem(cMemSub);

	float fVal = 4.f * acos(-1.);

	//m_BlobResult = vector<tBLOB_FEATURE>(nTotalLabel);
	m_BlobResult.resize(nTotalLabel);

// #ifdef _DEBUG
// #else
// #pragma omp parallel for
// #endif
	for (int idx = 1; idx <= nTotalLabel; idx++)
	{
		// �ð� ���� Ȯ��
		//if (cTimeOut.GetTimeOutFlag())	continue;

		int nBlobNum = idx - 1;

		m_BlobResult.at(nBlobNum).rectBox.x = matStats.at<int>(idx, CC_STAT_LEFT);
		m_BlobResult.at(nBlobNum).rectBox.y = matStats.at<int>(idx, CC_STAT_TOP);
		m_BlobResult.at(nBlobNum).rectBox.width = matStats.at<int>(idx, CC_STAT_WIDTH);
		m_BlobResult.at(nBlobNum).rectBox.height = matStats.at<int>(idx, CC_STAT_HEIGHT);

		// ��ü �ֺ� ( ��� GV�뵵 )
		int nOffSet = 20;

		int nSX = m_BlobResult.at(nBlobNum).rectBox.x - nOffSet;
		int nSY = m_BlobResult.at(nBlobNum).rectBox.y - nOffSet;
		int nEX = m_BlobResult.at(nBlobNum).rectBox.x + m_BlobResult.at(nBlobNum).rectBox.width + nOffSet + nOffSet;
		int nEY = m_BlobResult.at(nBlobNum).rectBox.y + m_BlobResult.at(nBlobNum).rectBox.height + nOffSet + nOffSet;

		if (nSX < 0)	nSX = 0;
		if (nSY < 0)	nSY = 0;
		if (nEX >= GrayBuffer.cols)	nEX = GrayBuffer.cols - 1;
		if (nEY >= GrayBuffer.rows)	nEY = GrayBuffer.rows - 1;

		cv::Rect rectTemp(nSX, nSY, nEX - nSX + 1, nEY - nSY + 1);

		__int64 nCount_in = 0;
		__int64 nCount_out = 0;
		__int64 nSum_in = 0;	//	defect ����
		__int64 nSum_out = 0;	//	defect ���ܿ���

		cv::Mat matTmp_src = GrayBuffer(rectTemp);		//	������ ROI
		cv::Mat matTmp_label = matLabel(rectTemp);			//	Label �� ROI
		cv::Mat matTemp = Mat().zeros(rectTemp.height, rectTemp.width, CV_8UC1);

		for (int y = 0; y < rectTemp.height; y++)
		{
			int		*ptrLabel = (int *)matTmp_label.ptr(y);
			uchar	*ptrGray = (uchar *)matTmp_src.ptr(y);
			uchar	*ptrTemp = (uchar *)matTemp.ptr(y);

			for (int x = 0; x < rectTemp.width; x++, ptrLabel++, ptrGray++, ptrTemp++)
			{
				// �ش� ��ü
				if (*ptrLabel == idx)
				{
					nSum_in += *ptrGray;
					nCount_in++;

					// �ش� �� ���Ϳ� �ȼ� ��ǥ ����
					m_BlobResult.at(nBlobNum).ptIndexs.push_back(cv::Point(nSX + x, nSY + y));

					*ptrTemp = (uchar)255;

					m_BlobResult.at(nBlobNum).nHist[*ptrGray]++;
				}
				// �ٸ���� ���
				else
				{
					// �� ��ȣ�� 0�� ���...
					// �ٸ� ��ü �����ɼ� �����Ƿ�.... ���� �߰�
					if (*ptrLabel == 0)
					{
						nSum_out += *ptrGray;
						nCount_out++;
					}
				}
			}
		}

		// ��� ���� ��
		m_BlobResult.at(nBlobNum).nSumGV = nSum_in;

		// ��ü ����
		m_BlobResult.at(nBlobNum).nArea = nCount_in;	//matStats.at<int>(idx, CC_STAT_AREA);

														// Box Area
		m_BlobResult.at(nBlobNum).nBoxArea = m_BlobResult.at(nBlobNum).rectBox.width * m_BlobResult.at(nBlobNum).rectBox.height;

		// Bounding Box ���� ���� / ��ü ���� ( Rectangularity(= Extent) )
		m_BlobResult.at(nBlobNum).fBoxRatio = m_BlobResult.at(nBlobNum).nArea / (float)m_BlobResult.at(nBlobNum).nBoxArea;

		// Elongation ���ϱ�
		m_BlobResult.at(nBlobNum).fElongation = m_BlobResult.at(nBlobNum).rectBox.width / (float)m_BlobResult.at(nBlobNum).rectBox.height;

		// stdDev ���ϱ�
		cv::Scalar m, s;
		cv::meanStdDev(matTmp_src, m, s, matTemp);
		m_BlobResult.at(nBlobNum).fStdDev = float(s[0]);

		// Contours ���ϱ�
		vector<vector<cv::Point>>	ptContours;
		vector<vector<cv::Point>>().swap(ptContours);
		cv::findContours(matTemp, ptContours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		// Perimeter ���ϱ�
		if (ptContours.size() != 0)
		{
			// ROI �����̹Ƿ�, ���� �ʿ�
			// �ܰ� ��ǥ ��� ����
			for (int m = 0; m < ptContours.size(); m++)
			{
				for (int k = 0; k < ptContours.at(m).size(); k++)
					m_BlobResult.at(nBlobNum).ptContours.push_back(cv::Point(ptContours.at(m)[k].x + nSX, ptContours.at(m)[k].y + nSY));
			}
		}
		else
		{
			// ���� ��ǥ�̹Ƿ�, ���� ���ص� ��.
			// �ܰ� ��ǥ ��� ����
			m_BlobResult.at(nBlobNum).ptContours.resize((int)m_BlobResult.at(nBlobNum).ptIndexs.size());
			std::copy(m_BlobResult.at(nBlobNum).ptIndexs.begin(), m_BlobResult.at(nBlobNum).ptIndexs.end(), m_BlobResult.at(nBlobNum).ptContours.begin());
		}
		m_BlobResult.at(nBlobNum).fPerimeter = float(cv::arcLength(m_BlobResult.at(nBlobNum).ptContours, true));
		vector<vector<cv::Point>>().swap(ptContours);

		// Roundness ���ϱ�
		m_BlobResult.at(nBlobNum).fRoundness = (fVal * m_BlobResult.at(nBlobNum).nArea)
			/ (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter);

		// ��ü�� �� ��翡 �󸶳� ����? ( �ѷ�^2 / 4 * Pi * ���� )
		m_BlobResult.at(nBlobNum).fCompactness = (m_BlobResult.at(nBlobNum).fPerimeter * m_BlobResult.at(nBlobNum).fPerimeter)
			/ (fVal * float(m_BlobResult.at(nBlobNum).nArea));

		// Defect GV ���ϱ�
		m_BlobResult.at(nBlobNum).fMeanGV = nSum_in / (float)nCount_in;

		//	��� GV ���ϱ�
		m_BlobResult.at(nBlobNum).fBKGV = nSum_out / (float)nCount_out;

		// GV ���̰� ���ϱ� ( ��� - ��ü )
		m_BlobResult.at(nBlobNum).fDiffGV = m_BlobResult.at(nBlobNum).fBKGV - m_BlobResult.at(nBlobNum).fMeanGV;

		// min, max GV ���ϱ�
		double valMin, valMax;
		cv::minMaxLoc(matTmp_src, &valMin, &valMax, 0, 0, matTemp);
		m_BlobResult.at(nBlobNum).nMinGV = (long)valMin;
		m_BlobResult.at(nBlobNum).nMaxGV = (long)valMax;

		// ��ü �ּ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMinGVRatio = m_BlobResult.at(nBlobNum).nMinGV / m_BlobResult.at(nBlobNum).fBKGV;

		// ��ü �ִ� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).nMaxGVRatio = m_BlobResult.at(nBlobNum).nMaxGV / m_BlobResult.at(nBlobNum).fBKGV;

		//      ��� ��� / ��ü ��� ���
		m_BlobResult.at(nBlobNum).fDiffGVRatio = m_BlobResult.at(nBlobNum).fMeanGV / m_BlobResult.at(nBlobNum).fBKGV;

		// Center Point ���ϱ�
		m_BlobResult.at(nBlobNum).ptCenter.x = (int)matCentroid.at<double>(idx, 0);
		m_BlobResult.at(nBlobNum).ptCenter.y = (int)matCentroid.at<double>(idx, 1);

		// SEMU ���ϱ�
		if (m_BlobResult.at(nBlobNum).fDiffGV == 0.0)
		{
			if (m_BlobResult.at(nBlobNum).fBKGV == 0)
			{
				m_BlobResult.at(nBlobNum).fSEMU = 1.0
					/ (1.97f / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f));
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (0.000001 / m_BlobResult.at(nBlobNum).fBKGV)
					/ (1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f));
			}
		}
		else
		{
			if (m_BlobResult.at(nBlobNum).fBKGV == 0)
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / 0.000001)
					/ (1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f));
			}
			else
			{
				m_BlobResult.at(nBlobNum).fSEMU = (fabs(m_BlobResult.at(nBlobNum).fMeanGV - m_BlobResult.at(nBlobNum).fBKGV) / m_BlobResult.at(nBlobNum).fBKGV)
					/ (1.97 / (cv::pow((float)m_BlobResult.at(nBlobNum).nArea, 0.33f) + 0.72f));
			}
		}

		cv::RotatedRect BoundingBox = cv::minAreaRect(m_BlobResult.at(nBlobNum).ptIndexs);
		// ȸ���� �簢�� ������ 4��
		//cv::Point2f vertices[4];
		//BoundingBox.points(vertices);


		// Box width and length
		m_BlobResult.at(nBlobNum).BoxSize = BoundingBox.size;

		// Angle between the horizontal axis
		m_BlobResult.at(nBlobNum).fAngle = BoundingBox.angle;


		// minAreaRect�� ��� ������ ���� Width�� Height�� ����Ǵ� ������ �־�
		// ����� ��������� �����ϴ� ���� ����
		// ������ �̻��ϱ��ѵ�.. �̰� ���� ������� �ʾƼ� ���߿� ���濹��
		// ���� blog https://namkeenman.wordpress.com/tag/minarearect/
		// CIH 20200425 
		if (abs(m_BlobResult.at(nBlobNum).rectBox.width - BoundingBox.size.width) > abs(m_BlobResult.at(nBlobNum).rectBox.width - BoundingBox.size.height))
		{
			float fTemp = BoundingBox.size.width;
			BoundingBox.size.width = BoundingBox.size.height;
			BoundingBox.size.height = fTemp;
		}

		// Minor Axis & Major Axis
		if (BoundingBox.size.width > BoundingBox.size.height)
		{
			m_BlobResult.at(nBlobNum).fMinorAxis = BoundingBox.size.height;
			m_BlobResult.at(nBlobNum).fMajorAxis = BoundingBox.size.width;
		}
		else
		{
			m_BlobResult.at(nBlobNum).fMinorAxis = BoundingBox.size.width;
			m_BlobResult.at(nBlobNum).fMajorAxis = BoundingBox.size.height;
		}

		// Feret��s area
		m_BlobResult.at(nBlobNum).fMinBoxArea = m_BlobResult.at(nBlobNum).fMinorAxis * m_BlobResult.at(nBlobNum).fMajorAxis;

		// Axis Ratio
		if (m_BlobResult.at(nBlobNum).fMajorAxis > 0)
			m_BlobResult.at(nBlobNum).fAxisRatio = m_BlobResult.at(nBlobNum).fMinorAxis / m_BlobResult.at(nBlobNum).fMajorAxis;
		else
			m_BlobResult.at(nBlobNum).fAxisRatio = 0.f;

		// Min Bounding Box ���� ���� / ��ü ���� ( Area porosity )
		m_BlobResult.at(nBlobNum).fMinBoxRatio = m_BlobResult.at(nBlobNum).fMinBoxArea / (float)m_BlobResult.at(nBlobNum).nArea;
		//choikwangil
		m_BlobResult.at(nBlobNum).fMeanAreaRatio = m_BlobResult.at(nBlobNum).fMeanGV / (float)m_BlobResult.at(nBlobNum).nArea;
		// �Ҵ� ����
		matTmp_src.release();
		matTmp_label.release();
		matTemp.release();
	}

// 	if (m_cInspectLibLog->Use_AVI_Memory_Log) {
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Fixed USE Memory"), cMatBufTemp.Get_FixMemory(), m_nAlgType);
// 		writeInspectLog_Memory(E_ALG_TYPE_AVI_MEMORY, __FUNCTION__, _T("Auto USE Memory"), cMatBufTemp.Get_AutoMemory(), m_nAlgType);
// 	}
// 
// 	// �ð� ���� Ȯ��
// 	if (cTimeOut.GetTimeOutFlag())	return false;

	return true;
}

BOOL InspMainSequence::CheckImageIsNormal_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat& MatColorImage, cv::Mat& MatDrawImage, int nRatio, int nImageNum, int nCameraNum, int nStageNo,
	tAlignInfo stCamAlignInfo[MAX_CAMERA_COUNT], ResultBlob_Total* pResultBlobTotal, double dCamResolution, double dPannelSizeX, double dPannelSizeY,
	bool bStAlignEnd[MAX_CAMERA_COUNT], bool bStBackAlignEnd[MAX_CAMERA_COUNT], bool bStVacuumAlignEnd[MAX_CAMERA_COUNT],bool bStLabelMaskMakeEnd[MAX_CAMERA_COUNT], bool bStStageDustEnd[MAX_CAMERA_COUNT], long nError_Thread[MAX_CAMERA_COUNT], int nError_Image_Num[MAX_CAMERA_COUNT], bool bStSuccess[MAX_CAMERA_COUNT], 
	bool bAlignEnd[MAX_CAMERA_COUNT] ,bool& bChkDustEnd, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm,
	ENUM_INSPECT_MODE eInspMode)
{
	// AD ����
	long nErrorCode = E_ERROR_CODE_TRUE;
	
	double* dAlignPara = theApp.GetAlignParameter(nCameraNum);

	int nAlignImageClassify = 0;
	int nAlignBackImageClassify = 0;
	int nAlignVacuumImageClassify = 0;
	int nAlignLabelImageClassify = 0;

	int nColorCode = (int)dAlignPara[E_PARA_LCP_COLORCODE_NUM]; //�ӽ� �÷� �ڵ�

	int nEQType = theApp.m_Config.GetEqpType();

	double dStage_Mean_Gv = 0.0; // Stage ��� Gv Log �� �˶���
	//LCP Area Camera �ػ� ���� Image ���� ���
	wchar_t wstrInspAreaImgPath[MAX_PATH] = { 0, };
	swprintf(wstrInspAreaImgPath, _T("%s\\%s\\"), ORIGIN_PATH, (LPCWSTR)strPanelID);

	switch (nEQType)
	{
	case EQP_AVI:
	case EQP_LCP:
		nAlignImageClassify = (int)dAlignPara[E_PARA_LCP_AD_IMAGE_NUM];		// Align Parameter 0������ Align �� Image ID(Classify) 
		nAlignBackImageClassify = (int)dAlignPara[E_PARA_LCP_BACK_IMAGE_NUM];		// Align Parameter 0������ Align �� Image ID(Classify) 
		nAlignVacuumImageClassify = (int)dAlignPara[E_PARA_LCP_MASK_VACUUM_IMAGE_NUM];		//12.14
		nAlignLabelImageClassify = (int)dAlignPara[E_PARA_LCP_MASK_LABEL_IMAGE_NUM];		//12.14
		
		//nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignImageClassify);

		break;
	default:
		return E_ERROR_CODE_FALSE;
	}


	

	// Alg DLL Start Log
	//if (theApp.GetImageClassify(nImageNum) == nAlignBackImageClassify || theApp.GetImageClassify(nImageNum) == nAlignVacuumImageClassify || theApp.GetImageClassify(nImageNum) == nAlignImageClassify || theApp.GetImageClassify(nImageNum) == nAlignLabelImageClassify) {

		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
			strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));
		double dResult[4] = { 0, };		// SVI ���� ����

		
										//////////////////////////////////////////////////////////////////////////
										//Image Buffer Check
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Image Buffer Check. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

			cv::Scalar m, s;
			cv::meanStdDev(MatOrgImage, m, s);

			if (m[0] == 0) {
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Image Buffer Check Fail!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

				nErrorCode = E_ERROR_CODE_LCP_ALIGN_IMAGE_NON_BUFFER;

				nError_Thread[nCameraNum] = nErrorCode;
				nError_Image_Num[nCameraNum] = nImageNum;
			}
			else {
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Image Buffer Check OK!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

			}
		}
		//////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		///Back Align Image

		//////////////////////////////////////////////////////////////////////////
		// ����, ���̺����� �÷� �������� �Կ� ��ŨȦ ����ũ ����
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
		if (nColorCode == E_COLOR_CODE_MP || nColorCode == E_COLOR_CODE_TRANSPARENCY) {
			
				if (theApp.GetImageClassify(nImageNum) == nAlignVacuumImageClassify) {



					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Vaccum Mask Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					nErrorCode = AcquireAutoVaccumRoiAndAlignImage_LCP(strPanelID, strDrive, MatOrgImage, MatColorImage, nRatio, nAlignImageClassify, nImageNum, nCameraNum, stCamAlignInfo[nCameraNum], dResult, dCamResolution,
						nColorCode, dPannelSizeX, dPannelSizeY);

					MatOrgImage.copyTo(stCamAlignInfo[nCameraNum].matSrc_Stage);

					if (nErrorCode == E_ERROR_CODE_TRUE) {

						bStVacuumAlignEnd[nCameraNum] = true;

					}
					else {
						theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Vaccum Mask Fail!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
							strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

						nError_Thread[nCameraNum] = nErrorCode;
						nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignVacuumImageClassify);

					}
				}
				else {

					for (int i = 0; i < MAX_CAMERA_COUNT; i++)
					{
						while (!bStVacuumAlignEnd[i])
						{
							Sleep(10);
						}
					}
				}

				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Vaccum Mask End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));
			}
		}
		//��Ũ Mask End
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// ���÷� �齺������ Mask ����
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
		if (theApp.GetImageClassify(nImageNum) == nAlignBackImageClassify) {
			

				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align BackStage Mask Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));



				nErrorCode = AcquireAutoBackRoiAndAlignImage_LCP(strPanelID, strDrive, MatOrgImage, MatColorImage, nRatio, nAlignImageClassify, nImageNum, nCameraNum, stCamAlignInfo[nCameraNum], dResult, dCamResolution,
					nColorCode, dPannelSizeX, dPannelSizeY);

				if (nErrorCode == E_ERROR_CODE_TRUE) {

					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align BackStage Mask End!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					bStBackAlignEnd[nCameraNum] = true;

				}
				else {
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align BackStage Mask Fail!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					nError_Thread[nCameraNum] = nErrorCode;
					nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignBackImageClassify);
				}
			}
		
		else {

			for (int i = 0; i < MAX_CAMERA_COUNT; i++)
			{
				while (!bStBackAlignEnd[i])
				{
					Sleep(10);
				}
			}
		}

		}

		// Make Back Mask End.
		//////////////////////////////////////////////////////////////////////////

		Mat matTempAlign;
		//////////////////////////////////////////////////////////////////////////
		//Active Align ��ó��
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {

			if (theApp.GetImageClassify(nImageNum) != AREA_CAM_IMG_CLASSIFY_NUM) { //AREA ������ �ڿ��� ó��

				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Matori And ActiveMask Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));



				switch (nColorCode)
				{
				case E_COLOR_CODE_WHITE:
					cv::bitwise_and(MatOrgImage, stCamAlignInfo->matMask_BackStage, matTempAlign);

					break;
				case E_COLOR_CODE_BLACK:

					cv::bitwise_not(MatOrgImage, matTempAlign);
					cv::bitwise_and(matTempAlign, stCamAlignInfo->matMask_BackStage, matTempAlign);

					break;
				case E_COLOR_CODE_MP:

					cv::bitwise_and(MatOrgImage, stCamAlignInfo->matMask_BackStage, matTempAlign);

					break;
				case E_COLOR_CODE_TRANSPARENCY:
					cv::bitwise_and(MatOrgImage, stCamAlignInfo->matMask_BackStage, matTempAlign);
					cv::bitwise_and(stCamAlignInfo->matMask_VacuumHole, stCamAlignInfo->matMask_BackStage, stCamAlignInfo->matMask_VacuumHole);
					cv::bitwise_and(stCamAlignInfo->matSrc_Stage, stCamAlignInfo->matMask_BackStage, stCamAlignInfo->matSrc_Stage);

					break;
				default:
					break;
				}

				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align MatSrc And ActiveMask End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));
			}
		}
		// ��ó�� End.
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// ���÷� Align(Get Theta,Roi) �� Mask ���� 
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
			if (theApp.GetImageClassify(nImageNum) == nAlignImageClassify) {


				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Theta Roi Align Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

				//////////////////////////////////////////////////////////////////////////�ӽ�
				MatOrgImage.copyTo(stCamAlignInfo[nCameraNum].matSrc_DrawMain);
				//////////////////////////////////////////////////////////////////////////

				nErrorCode = AcquireAutoRoiAndAlignImage_LCP(strPanelID, strDrive, matTempAlign, MatColorImage, nRatio, nAlignImageClassify, nImageNum, nCameraNum, stCamAlignInfo[nCameraNum], dResult, dCamResolution,
					nColorCode, dPannelSizeX, dPannelSizeY);


				if (nErrorCode == E_ERROR_CODE_TRUE) {
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Theta Roi Align End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					bStAlignEnd[nCameraNum] = true;

				}
				else {
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Theta Roi Align Fail!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					nError_Thread[nCameraNum] = nErrorCode;
					nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignImageClassify);

				}
			}

			else {

				for (int i = 0; i < MAX_CAMERA_COUNT; i++)
				{
					while (!bStAlignEnd[i])
					{
						Sleep(10);
					}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//Label Mask ����
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
			if (nColorCode == E_COLOR_CODE_MP || nColorCode == E_COLOR_CODE_TRANSPARENCY) {
				if (theApp.GetImageClassify(nImageNum) == nAlignLabelImageClassify) {

					cv::bitwise_and(matTempAlign, stCamAlignInfo[nCameraNum].matMask_Active, matTempAlign);

					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Make Label Mask Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					nErrorCode = AcquireAutoRoiAndAlignImage_LCP_Mask(strPanelID, strDrive, matTempAlign, nRatio, nImageNum, nCameraNum, stCamAlignInfo[nCameraNum], dResult, dCamResolution,
						dPannelSizeX, dPannelSizeY, nColorCode);

					if (nErrorCode == E_ERROR_CODE_TRUE) {

						bStLabelMaskMakeEnd[nCameraNum] = true;

					}
					else {
						theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Make Label Mask Fail!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
							strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

						nError_Thread[nCameraNum] = nErrorCode;
						nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignLabelImageClassify);
					}
				}

				else {

					for (int i = 0; i < MAX_CAMERA_COUNT; i++)
					{
						while (!bStLabelMaskMakeEnd[i])
						{
							Sleep(10);
						}
					}
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if (nError_Thread[nCameraNum] == E_ERROR_CODE_TRUE || nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
			if (nColorCode == E_COLOR_CODE_MP || nColorCode == E_COLOR_CODE_TRANSPARENCY) {
				if (theApp.GetImageClassify(nImageNum) == nAlignVacuumImageClassify) {



					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Stage Dust Check Start. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s."),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					nErrorCode = Empty_Stage_Dust_Judge(stCamAlignInfo->matSrc_Stage, stCamAlignInfo->matMask_VacuumHole, strPanelID, strDrive, dStage_Mean_Gv, dAlignPara);


					if (nErrorCode == E_ERROR_CODE_TRUE) {

						theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Stage Dust Check End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s [Stage Mean Gv: %f]."),
							strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dStage_Mean_Gv);

						bStStageDustEnd[nCameraNum] = true;

					}
					else {
						theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Stage Dust Warning!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s [Stage Mean Gv: %f]."),
							strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dStage_Mean_Gv);

						nError_Thread[nCameraNum] = nErrorCode;
						nError_Image_Num[nCameraNum] = theApp.GetImageNum(nAlignVacuumImageClassify);

					}

				}

				else {

					for (int i = 0; i < MAX_CAMERA_COUNT; i++)
					{
						while (!bStStageDustEnd[i])
						{
							Sleep(10);
						}
					}
				}
			}
		}
		//}
		//////////////////////////////////////////////////////////////////////////

		// ���� �ۺο� ī�޶� �߽���ġ - �� �߽���ġ - ���� um
		//int nDiffCenterX = (int)((stCamAlignInfo[nCameraNum].ptAlignCenter.x - stCamAlignInfo[nCameraNum].ptCellCenter.x) * dCamResolution);
		//int nDiffCenterY = (int)((stCamAlignInfo[nCameraNum].ptAlignCenter.y - stCamAlignInfo[nCameraNum].ptCellCenter.y) * dCamResolution);

			// KSW 17.08.10 - ���� ��ġ �̻� Ʋ���� ��� ���ϰ� : E_ERROR_CODE_ALIGN_ANGLE_RANGE			// �ش� ��ġ�� Align �Ķ���� : AngleError ���� ����
			// ���� ��ġ �̻� Ʋ������ ��� ��˶� ����
			// ���� ������ ��� ��� Ȯ�� ������ �˶� ����� ��...
			// �������� �ٽ� ƼĪ�ϵ���... or ��� Ʋ�������� Ȯ�� ��/��
		if (nError_Image_Num[nCameraNum] == -1) { //Error ���� �������
			if (theApp.GetImageClassify(nImageNum) == nAlignImageClassify) {
			
				// Alg DLL End Log
				theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n"),
					strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dResult[0], dResult[1]);

				stCamAlignInfo[nCameraNum].bAlignSuccess = true;
				bStSuccess[nCameraNum] = true;

				if (nColorCode == E_COLOR_CODE_WHITE) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_BLACK) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_MP) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true && bStVacuumAlignEnd[nCameraNum] == true && bStLabelMaskMakeEnd[nCameraNum] == true && bStStageDustEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_TRANSPARENCY) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true && bStVacuumAlignEnd[nCameraNum] == true && bStLabelMaskMakeEnd[nCameraNum] == true && bStStageDustEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
			}
		}else{//Error�� ������

			if (theApp.GetImageClassify(nImageNum) == theApp.GetImageClassify(nError_Image_Num[nCameraNum])) { //���� Thread���� ����

				if (nError_Thread[nCameraNum] == E_ERROR_CODE_ALIGN_ANGLE_RANGE_ERROR)		// 
				{
					// Alg DLL End Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End - InspStop !!!. Abnormal Angle. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));


					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Heavy"), _T("Hole Find Fail"), _T("�÷��� Hole�� ��ã�ҽ��ϴ�."), eALARMID_LCP_ALIGN_HOLE_FIND_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = false;
					bStSuccess[nCameraNum] = false;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
				else if (nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_ALIGN_IMAGE_NON_BUFFER)
				{
					// Alg DLL End Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End - InspStop !!!. Abnormal Angle. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));

					CString strMsg;

					strMsg.Format(_T("%s �̹����� ����� ������ �ʾҽ��ϴ�."), theApp.GetGrabStepName(nImageNum));

					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Heavy"), _T("Align Image Error"), strMsg, eALARMID_LCP_ALIGN_IMAGE_BUFFER_ERROR, eALARMTYPE_HEAVY, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = false;
					bStSuccess[nCameraNum] = false;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
				else if (nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_ALIGN_BACK_MASK_FAIL)
				{
					// Alg DLL End Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End - InspStop !!!. Abnormal Angle. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));


					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Heavy"), _T("Back Mask Error"), _T("Back Mask ����� ����."), eALARMID_LCP_ALIGN_BACK_MASK_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = false;
					bStSuccess[nCameraNum] = false;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
				else if (nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_ALIGN_ACTIVE_MASK_FAIL)
				{
					// Alg DLL End Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("Align Algorithm End - InspStop !!!. Abnormal Angle. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum));


					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Heavy"), _T("Active Mask Error"), _T("Active Mask ����� ����."), eALARMID_LCP_ALIGN_ACTIVE_MASK_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = false;
					bStSuccess[nCameraNum] = false;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
				else if (nError_Thread[nCameraNum] == E_ERROR_CODE_LCP_STAGE_DIRTY) {
					// Error Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE,
						_T("Stage Dirty - Insp Warning !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.. Error Code : %d"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), nErrorCode);

					CString strMsg;

					strMsg.Format(_T("Stage ������ �����ϴ�. [Stage Mean Gv: %f]"), dStage_Mean_Gv);
					// �߾˶����� ����
					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Warning"), _T("Stage Dust"), strMsg, eALARMID_LCP_STAGE_DUST, eALARMTYPE_LIGHT, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = true;
					bStSuccess[nCameraNum] = true;
					nError_Thread[nCameraNum] = E_ERROR_CODE_TRUE;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
				// ����� ��ü�� ������ ���
				else if (nError_Thread[nCameraNum] != E_ERROR_CODE_TRUE)
				{
					// Error Log
					theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE,
						_T("Align Fail - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.. Error Code : %d"),
						strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), nErrorCode);

					// �߾˶����� ����
					//theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eInspMode, eALARMID_ALIGN_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);
					theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC_LCP(eInspMode, _T("Heavy"), _T("Align Fail"), _T("Align ���� �߽��ϴ�."), eALARMID_LCP_ALIGN_FAIL, eALARMTYPE_HEAVY, bIsHeavyAlarm);

					nError_Image_Num[nCameraNum] = nImageNum;

					stCamAlignInfo[nCameraNum].bAlignSuccess = false;
					bStSuccess[nCameraNum] = false;
					//////////////////////////////////////////////////////////////////////////
					bStVacuumAlignEnd[nCameraNum] = true;
					bStBackAlignEnd[nCameraNum] = true;
					bStAlignEnd[nCameraNum] = true;
					bStLabelMaskMakeEnd[nCameraNum] = true;
					bStStageDustEnd[nCameraNum] = true;
					//////////////////////////////////////////////////////////////////////////

				}
			
				if (nColorCode == E_COLOR_CODE_WHITE) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_BLACK) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_MP) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true && bStVacuumAlignEnd[nCameraNum] == true && bStLabelMaskMakeEnd[nCameraNum] == true && bStStageDustEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
				else if (nColorCode == E_COLOR_CODE_TRANSPARENCY) {
					if (bStAlignEnd[nCameraNum] == true && bStBackAlignEnd[nCameraNum] == true && bStVacuumAlignEnd[nCameraNum] == true && bStLabelMaskMakeEnd[nCameraNum] == true && bStStageDustEnd[nCameraNum] == true) {
						bAlignEnd[nCameraNum] = true;
					}
				}
			}
		
		}

			

		
	//}

		// �� �����庰 ����� ���� ����
		// (���� Step �� Pixel Shift ���ο� ���� �⺻ �ػ� ���� Align Info �� ���� ����Ͽ� ����)

		
	
	// Align ���� �ʴ� �̹������� ����� ��� ��ٸ�
	// ��� ī�޶� ����� �Ϸ�ɶ�����
	for (int i = 0; i < MAX_CAMERA_COUNT; i++)
	{
		while (!bAlignEnd[i])
		{
			Sleep(10);
		}
	}

	bool bNeedRetry = false;


	
	//////////////////////////////////////////////////////////////////////////
	m_stThrdAlignInfo.SetAdjustAlignInfoRatio(&stCamAlignInfo[nCameraNum], nRatio, stCamAlignInfo[0].bAlignSuccess && stCamAlignInfo[nCameraNum].bAlignSuccess);	// 0�� ī�޶� ����� ���и� ��� ���з� ����

																																									//Area Camera
	if (theApp.GetImageClassify(nImageNum) == AREA_CAM_IMG_CLASSIFY_NUM) {

		Area_Cam_Img_Transform(MatOrgImage, &stCamAlignInfo[nCameraNum], wstrInspAreaImgPath , dAlignPara);
		cv::cvtColor(MatOrgImage, MatColorImage, COLOR_GRAY2RGB);
		cv::cvtColor(MatOrgImage, MatDrawImage, COLOR_GRAY2RGB);
	
	}

																																									// AD �˻� �� Align ���� �ÿ��� GV �˻� ����
																																									// Align Warning �� ��쿡�� �˻�� ����
	if ((nErrorCode == E_ERROR_CODE_TRUE || nErrorCode == E_ERROR_CODE_ALIGN_ANGLE_RANGE_WARNING) && m_stThrdAlignInfo.bAlignSuccess == true && !bIsHeavyAlarm)
	{
		// 0 : ��� ��� (Mean GV)
		// 1 : ��� ������ ( DISPLAY_ABNORMAL, DISPLAY_DARK, DISPLAY_BRIGHT )
		// ����� AD GV�� ��Ȯ��
		//double dMeanResult[4] = { 0 , };		// 2->4 SVI ���� ����

												/// AD GV �˻�		
// 		nErrorCode = CheckADGV(strPanelID, strDrive, MatOrgImage, nStageNo, nImageNum, nCameraNum, nRatio, m_stThrdAlignInfo.ptCorner, pResultBlobTotal, dMeanResult,
// 			bChkDustEnd, bNeedRetry, bIsNormalDust, bUseDustRetry, nDustRetryCnt, bIsHeavyAlarm, eInspMode);

		// for SVI - ���� 0�� ī�޶� �̹��� �������� Warping �ؾ���.
		// 17.06.08 APP �߰� : Rotate
		// AdjustOriginImage(MatOrgImage, MatDrawImage, &stCamAlignInfo[0]);
		// ImageSave(MatOrgImage, _T("D:\\Test\\Origin%d_CAM%d.jpg"), nImageNum, nCameraNum);

	}
	else
	{
		/// for AVI - Dust Retry �ʿ� ���� - Dust �˻� ���� - SVI / APP Align �� �ʿ� �� Ȯ���Ͽ� Ȱ��
		// Seq. Task �� Dust ���� �� ��� �ۺ� (AVI)
		bNeedRetry = false;
		bIsNormalDust = false;

		// �ӽ� ������ 4�� ä���ֱ� (�����)
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_LEFT_TOP] = cv::Point(0, 0);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_RIGHT_TOP] = cv::Point(MatOrgImage.cols - 1, 0);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_RIGHT_BOTTOM] = cv::Point(MatOrgImage.cols - 1, MatOrgImage.rows - 1);
		stCamAlignInfo[nCameraNum].ptCorner[E_CORNER_LEFT_BOTTOM] = cv::Point(0, MatOrgImage.rows - 1);
	}

	// Seq. Task �� Dust ���� �� ��� �ۺ� (����� AVI�� ���)
// 	if (theApp.GetImageClassify(nImageNum) == E_IMAGE_CLASSIFY_AVI_DUST)
// 		theApp.m_AlgorithmTask->VS_Send_Dust_Result_To_Seq(eInspMode, bNeedRetry, bIsNormalDust);

	return nErrorCode;
}

void InspMainSequence::Area_Cam_Img_Transform(cv::Mat& matSrc, tAlignInfo* pStAlignInfo ,CString strSave_Path, double* dAlignPara) {
	
	int nOri_Width = (int)dAlignPara[E_PARA_LCP_AREA_IMG_TRANSFORM_ORI_WIDTH];
	int nOri_Height = (int)dAlignPara[E_PARA_LCP_AREA_IMG_TRANSFORM_ORI_HEIGHT];
	int nTrans_X = (int)dAlignPara[E_PARA_LCP_AREA_IMG_TRANSFORM_X];
	int nTrans_Y = (int)dAlignPara[E_PARA_LCP_AREA_IMG_TRANSFORM_Y];

	cv::Mat matSrc_Resize;
	cv::Mat matSrc_Temp;

	matSrc.copyTo(matSrc_Temp);


	//////////////////////////////////////////////////////////////////////////
// 	cv::Mat matTest = imread("D:\\[IMAGE]\\�Ե��ɹ�Į\\220110\\220110_Wihte\\AlignCamera\\000284583237\\Insp2.bmp", IMREAD_GRAYSCALE);
// 	//cv::Mat matTest = cv::Mat::zeros(matTest2.size(), matTest2.type());
// 	cv::Mat matOriTest = imread("D:\\[IMAGE]\\�Ե��ɹ�Į\\220110\\220110_Wihte\\000284583237\\01_Side_CAM00.bmp", IMREAD_GRAYSCALE);
// 	cv::Mat matTemp = cv::Mat::zeros(matOriTest.size(), matOriTest.type());
// 
// 	cv::flip(matTest, matTest, -1);

// 	 	double dcammat[] = { 136179.105,  0,  3281.719,  0,  135456.503,  2760.784,  0,  0,  1 };
// 	 	double dDstMat[] = { 26.812687, -18634.780770, -0.047060,  0.011914 };
// 	// 
// // 	double dcammat[] = { 156920.941,  0,  1105.991,  0,  154432.186,   3282.980,  0,  0,  1 };
// // 	double dDstMat[] = {  6.480132, 5,  0.019790,  -0.110667 };
// 	// 
// 	cv::Mat cameramat = cv::Mat(3, 3, CV_64FC1, (void*)dcammat);
// 	cv::Mat disto = cv::Mat(1, 4, CV_64FC1, (void*)dDstMat);
// 	 
// 	cv::undistort(matTest2, matTest, cameramat, disto);

	cv::resize(matSrc_Temp, matSrc_Temp, matSrc_Temp.size() * 2, INTER_CUBIC);
	

//	imwrite("D:\\Test.bmp", matTest);

	Mat M2(2, 3, CV_32FC1);
	M2.at<float>(0, 0) = 1.f;
	M2.at<float>(1, 0) = 0.f;
	M2.at<float>(0, 1) = 0.f;
	M2.at<float>(1, 1) = 1.f; // ���� ��� -> ��, ������ �״��
	M2.at<float>(0, 2) = nTrans_X; // ù ������ ���� ���� �������� x�� �̵���
	M2.at<float>(1, 2) = nTrans_Y; // ù ������ ���� ���� �������� y�� �̵���
	cv::warpAffine(matSrc_Temp, matSrc, M2, cv::Size(nOri_Width, nOri_Height)); // �̵�
// 
// 	imwrite("D:\\Test2.bmp", matTemp);
	//////////////////////////////////////////////////////////////////////////

// 	cv::resize(matSrc_Temp, matSrc_Resize, Size(matSrc.cols * 2, matSrc.rows * 2), INTER_CUBIC);
// 
// // 	double dcammat[] = { 136179.105,  0,  3281.719,  0,  135456.503,  2760.784,  0,  0,  1 };
// // 	double dDstMat[] = { 26.812687, -18634.780770, -0.047060,  0.011914 };
// 
// 	double dcammat[] = { 156920.941,  0,  1105.991,  0,  154432.186,   3282.980,  0,  0,  1 };
// 	double dDstMat[] = {  6.480132, 5,  0.019790,  -0.110667 };
// 
// 	cv::Mat cameramat = cv::Mat(3, 3, CV_64FC1, (void*)dcammat);
// 	cv::Mat disto = cv::Mat(1, 4, CV_64FC1, (void*)dDstMat);
// 	// 
// 	// 	cv::Mat tt2 = cv::imread("C:\\Users\\woojin\\Desktop\\22.01.14\\�� ����\\02_COAXIAL_CAM00.bmp", 0);
// 	// 	cv::Mat tt1 = cv::imread("C:\\Users\\woojin\\Desktop\\22.01.14\\�� ����\\Area_PerAffine.bmp", 0);
// 
// 	cv::undistort(matSrc_Resize, matSrc_Temp, cameramat, disto);
	
	//���3
	//////////////////////////////////////////////////////////////////////////
// 	cv::Point2f ptSrc[4] = { cv::Point2f(0, 0),
// 		cv::Point2f(matSrc_Temp.cols ,	0),
// 		cv::Point2f(matSrc_Temp.cols ,	matSrc_Temp.rows),
// 		cv::Point2f(0,	matSrc_Temp.rows) };
// 	
// 	cv::Point2f ptDst[4] = { cv::Point2f((float)pStAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,	(float)pStAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y),
// 		cv::Point2f((float)pStAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	(float)pStAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y),
// 		cv::Point2f((float)pStAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x,(float)pStAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y),
// 		cv::Point2f((float)pStAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x,	(float)pStAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y) };
// 
// 
// 
// 	cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);		//	Warping ���
// 	cv::warpPerspective(matSrc_Temp, matSrc, mat2Warp, cv::Size(7300,9000), CV_INTER_AREA);

	cv::imwrite((cv::String)(CStringA)strSave_Path + "Area_PerAffine.bmp", matSrc);
	//cv::imwrite((cv::String)(CStringA)strBubblePath + "Bubble_WarpAffine.bmp", matBubble_Temp);
}

// long InspMainSequence::Empty_Stage_Dust_Judge(cv::Mat matSrc_Stage, cv::Mat matVaccumMask, CString strPanelID, double* dAlignPara) {
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	int nUse_Flag = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_USE_FLAG];
// 
// 	int nImageSave = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_TEXT];
// 	int nBlur_Size = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_BLUR_SIZE]; // 30
// 	int nThreshold = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_THRESHOLD]; // 15
// 
// 	double dJudge_Ratio = (double)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_JUDGE_RATIO]; // 1.2
// 
// 	//////////////////////////////////////////////////////////////////////////
// 	if (nUse_Flag <= 0) return E_ERROR_CODE_TRUE;
// 	//////////////////////////////////////////////////////////////////////////
// 
// 	CString strPanelID_;
// 	strPanelID_.Format(_T("%s"), strPanelID);
// 
// 	CString strStageDustPath;
// 	strStageDustPath.Format(_T("%s\\%s"), ,strPanelID);
// 
// 	if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(strStageDustPath, NULL);
// 
// 	//�������� ���� �˶� �˰��� �߰� �ڸ�
// 	cv::Mat matStageBuf = matSrc_Stage.clone();
// 
// 	// Stage ���� üũ
// 
// 	if (nImageSave > 0) {
// 		cv::imwrite("D:\\Stage_dust.jpg", matStageBuf);
// 	}
// 
// 	cv::Mat dust_check;
// 	cv::blur(matStageBuf, dust_check, cv::Size(nBlur_Size, nBlur_Size));
// 	cv::subtract(dust_check, matStageBuf, dust_check);
// 	cv::threshold(dust_check, dust_check, nThreshold, 255, 0);
// 	// stage dust �˶�����
// 
// 	cv::Mat invert2 = matVaccumMask.clone();
// 	//::invert(invert2, invert2);
// 	for (int i = 0; i < dust_check.rows; i++)
// 	{
// 		for (int j = 0; j < dust_check.cols; j++)
// 		{
// 
// 
// 			invert2.data[i*invert2.cols + j] = 255 - invert2.data[i*invert2.cols + j];
// 
// 			if (invert2.data[i*invert2.cols + j] == 0)
// 				dust_check.data[i*invert2.cols + j] = 0;
// 
// 
// 			if (j < 200)
// 				dust_check.data[i*invert2.cols + j] = 0;
// 			if (j > dust_check.cols - 250)
// 				dust_check.data[i*invert2.cols + j] = 0;
// 
// 		}
// 	}
// 
// 	double dMean_dust = cv::mean(dust_check)[0];
// 
// 	if (dMean_dust >= dJudge_Ratio) {
// 		if (nImageSave > 0) {
// 			CString strDust_Path;
// 			strDust_Path.Format(_T("D:\\Stage_dust_substarct_High_(%f).jpg"), dMean_dust);
// 			cv::imwrite((cv::String)(CStringA)strDust_Path, dust_check);
// 		}
// 
// 		return E_ERROR_CODE_LCP_STAGE_DIRTY;
// 	}
// 	else {
// 		if (nImageSave > 0) {
// 			CString strDust_Path;
// 			strDust_Path.Format(_T("D:\\Stage_dust_substarct_Low_(%f).jpg"), dMean_dust);
// 			cv::imwrite((cv::String)(CStringA)strDust_Path, dust_check);
// 		}
// 
// 		return E_ERROR_CODE_TRUE;
// 	}
// 
// }

