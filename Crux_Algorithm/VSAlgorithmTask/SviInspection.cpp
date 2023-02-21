#include "stdafx.h"
#include "SviInspection.h"
#include "SviDefineInspect.h"
#include "DllInterface.h"

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* strPath, ...);
void ImageAsyncSaveJPG(cv::Mat& MatSrcBuffer, const char* strPath);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace Concurrency;

IMPLEMENT_DYNCREATE(SviInspection, CWinThread)

SviInspection::SviInspection(void)
{
}

SviInspection::~SviInspection(void)
{
}

bool SviInspection::DrawDefectImage(CString strPanelID, cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData)
{
	// SVI �����ϴ� ���� ����
	// SVI �ҷ��� ��� MURA �̹Ƿ� Rect �ȱ׸�
	CCPUTimer timerDrawRect;
	timerDrawRect.Start();
	
	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// �̴ϸʿ� Rect �ȱ׸�
		resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
	}
	timerDrawRect.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Draw Rect tact time %.2f"), timerDrawRect.GetTime()/1000.);

	return true;
}

// �ܰ� �κ� ó��
long SviInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
{
	// ���� ó��
	if( dAlgPara == NULL )	return E_ERROR_CODE_EMPTY_PARA;

	wchar_t wstrID[MAX_PATH] = {0, };
	swprintf( wstrID, _T("%s"), LogicPara.tszPanelID );

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum) );

	// ���� �ڵ�
	long nErrorCode = Align_FindFillOutArea(LogicPara.MatOrgImage, LogicPara.MatBKG, theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nAlgImg], stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg], stThrdAlignInfo.tCHoleAlignData->bCHoleAD[nAlgImg],
		dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, strAlgLog, wstrID, stThrdAlignInfo.ptCorner);

	return nErrorCode;
}

// SVI ���� �Լ�
bool SviInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
								const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
								ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStage)
{
	resultPanelData.m_ResultHeader.SetInspectEndTime();

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement Start. Panel ID : %s"), strPanelID);	

	// UI - �˻� ���� üũ
	// �˻� ������ ��츸 ����
	// Grab�� �� ��� ���� ���� ( �ҷ� ���� )
	if( bUseInspect )
	{
		// �ߺ� ��ǥ ����
 		DeleteOverlapDefect(resultPanelData);
 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);

		//	������ 18.12.06
		//	MergeTool Falg �� ������� ������ ����
		{
			resultPanelData.m_ResultHeader.MERGE_nRatio						= nRatio;
			resultPanelData.m_ResultHeader.MERGE_rcAlignCellROI				= m_stThrdAlignInfo.rcAlignCellROI;
			WorkCoordCrt tmpWorkCoordCrt									= WrtResultInfo.GetWorkCoordCrt();	
			resultPanelData.m_ResultHeader.MERGE_dPanelSizeX				= (float)tmpWorkCoordCrt.dPanelSizeX;
			resultPanelData.m_ResultHeader.MERGE_dPanelSizeY				= (float)tmpWorkCoordCrt.dPanelSizeY;
			resultPanelData.m_ResultHeader.MERGE_nWorkDirection				= tmpWorkCoordCrt.nWorkDirection;
			resultPanelData.m_ResultHeader.MERGE_nWorkOriginPosition		= tmpWorkCoordCrt.nWorkOriginPosition;
			resultPanelData.m_ResultHeader.MERGE_nWorkOffsetX				= tmpWorkCoordCrt.nWorkOffsetX;
			resultPanelData.m_ResultHeader.MERGE_nWorkOffsetY				= tmpWorkCoordCrt.nWorkOffsetY;
			resultPanelData.m_ResultHeader.MERGE_nDataDirection				= tmpWorkCoordCrt.nDataDirection;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOriginPosition	= tmpWorkCoordCrt.nGateDataOriginPosition;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOffsetX			= tmpWorkCoordCrt.nGateDataOffsetX;
			resultPanelData.m_ResultHeader.MERGE_nGateDataOffsetY			= tmpWorkCoordCrt.nGateDataOffsetY;
			resultPanelData.m_ResultHeader.MERGE_dGatePitch					= (int)tmpWorkCoordCrt.dGatePitch;
			resultPanelData.m_ResultHeader.MERGE_dDataPitch					= (int)tmpWorkCoordCrt.dDataPitch;
		}

		// �ܰ� ���� ����ϴ� ���, Judgement()���� ���� ���� ( �ߺ� ���Ž�, �ش� �ҷ� �ܰ��� �ȱ׸� )
		// ��� ���ϴ� ���, Alg�ܿ��� ���� ���� ( �ߺ� ���ŵǵ�, �ش� �ҷ� �ܰ��� �׸� )
		if( USE_ALG_CONTOURS )
		{
			// KSW 18.02.05 - SVI �ܰ� ���� ���� ���
			wchar_t strContoursPath[MAX_PATH] = {0};

			// Contours.Merge ���� ��� ���� 180322 YSS
			CString strResultPath = RESULT_PATH;
			//swprintf_s( strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)RESULT_PATH, (LPCWSTR)strPanelID );
			swprintf_s(strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

			// KSW 17.11.29 - AVI & SVI �ܰ� ���� ���� ( AVI & SVI ���� �� )
			JudgeSaveContours(resultPanelData, strContoursPath);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours End. Panel ID : %s"), strPanelID);

			// AutoRun ��忡�� MergeTool ��� Ȱ��ȭ�� ��� D:\MergeTool\ ��γ��� ���� ���� ���� 
			if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
			{
				CString strDest;
				strDest.Format(_T("%s\\%s\\Contours.Merge"), MERGETOOL_PATH, strPanelID);
				CopyFile(strContoursPath, strDest, FALSE);
			}
		
			// KSW 18.02.05 - Mura �ܰ��� ���� ���� ���
			swprintf_s( strContoursPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID );

			// KSW 18.02.05 - Mura �ܰ��� ���� ����
			JudgeSaveMuraContours(resultPanelData, strContoursPath);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
		}
	}

	JudgementRepair(strPanelID, resultPanelData, WrtResultInfo);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Repair End. Panel ID : %s"), strPanelID);	

	// 17.07.12 Panel Grade ���� �߰�
	JudgementPanelGrade(resultPanelData);

	// Work ��ǥ ��� �� �ҷ� ������� Numbering
	NumberingDefect(strModelID, strPanelID, strLotID, WrtResultInfo, resultPanelData, nRatio);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Numbering Defect End. Panel ID : %s"), strPanelID);

	// ���� ���� �� ���� (Ư�� �ҷ� ���͸�, ��ǥ �ҷ� ����)
	ApplyReportRule(resultPanelData);

	// 	// ����� ���� CELL : NG / OK ����
	// 	if ( resultPanelData.m_ListDefectInfo.GetCount() > 0)
	// 		resultPanelData.m_ResultPanel.Judge = _T("NG");
	// 	else
	// 		resultPanelData.m_ResultPanel.Judge = _T("OK");

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement End. Panel ID : %s (%s)"), strPanelID, resultPanelData.m_ResultPanel.Judge);

	return true;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	StartLogicAlgorithm
//	 2. �Լ���� :	Algorithm DLL
//	 3. ��    �� :	
//	 4. �� ȯ �� :	�Ϸ� : true
//					���� : false
//	 5. �� �� �� :	17.12.04
//	 6. �� �� �� :	KSW
//	 7. ������� :	������ ��û ������.
//	 8. ��    �� :  �˰��� ����
//----------------------------------------------------------------------------------------------------
long SviInspection::StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// �Ķ���� ����
	cv::Mat				MatOriginImage_			= LogicPara.MatOrgImage;
	int					nAlgorithmNumber_		= nAlgorithmNumber;					// ���� �˰���	
	int					nCameraNum_				= LogicPara.nCameraNum;				// �˻� ī�޶�
	int					nThrdIndex_				= LogicPara.nThreadLog;
	cv::Mat				MatBKG_					= LogicPara.MatBKG;					// Cell �̿� ���� ( Dust, Black ������ ���� )

	ENUM_KIND_OF_LOG	eLogCamera				= (ENUM_KIND_OF_LOG)nCameraNum_;

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// ���� �Ķ���� ( �߰� ��� ���� �ʿ��� �Ķ���� )
	int nCommonPara[] = {
		MAX_DEFECT_COUNT								,	// 00 : �ҷ� �ִ� ���� ( MAX_DEFECT_COUNT ���� �۰� )
		theApp.GetCommonParameter()->bIFImageSaveFlag	,	// 01 : �˰��� �߰���� Image Save flag
		0												,	// 02 : ���� ���� ���� ī��Ʈ ( Point�� ��� �Լ��� �ΰ��� ���� �� )
		theApp.GetImageClassify(nImageNum)				,	// 03 : ���� Alg ���� ��ȣ
		nCameraNum_										,	// 04 : Cam Number ( ī�޶� ��ȣ / SVI )
		nROINumber										,	// 05 : ROI Number ( ROI ��ȣ )
		nAlgorithmNumber_								,	// 06 : �˰��� ��ȣ ( nAlgorithmNumber_ )
		nThrdIndex_										,	// 07 : Thread ID
		theApp.GetCommonParameter()->bDrawDefectNum		,	// 08 : Draw Defect Num ǥ��
		theApp.GetCommonParameter()->bDrawDust			,	// 09 : Draw Dust ǥ�� ( Point �˰����� ���ƾ� �� )
		nImageNum										,	// 10 : UI �� ���� ���� ���� ��ȣ
		(int)(stThrdAlignInfo.dAlignTheta * 1000)		,	// 11 : Cell ȸ���� ���� ( Align ��� ��, �Ҽ��� 3�ڸ� ������... )
		(int)stThrdAlignInfo.ptAlignCenter.x			,	// 12 : Cell ȸ���� �߽� x��ǥ
		(int)stThrdAlignInfo.ptAlignCenter.y			,	// 13 : Cell ȸ���� �߽� y��ǥ
		1													// 14 : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	};

	// �˻� ���(�ҷ�) ���� ����ü
	stDefectInfo* pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// �����Ͼ� ���� ����ü ��������
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// ���� ��� - ����
	wchar_t pathAlgorithms[4][1024] = {0, };

	CString strAlgResultPath = ALG_RESULT_PATH;
	CString strResultPath = RESULT_PATH;
	CString strMergeToolPath = MERGETOOL_PATH;

	// �����ڸ�� ���� ���� ���
	CreateDirectory(ALG_RESULT_PATH, NULL);
	swprintf( pathAlgorithms[0], _T("%s%s\\"), (LPCWSTR)strAlgResultPath, LogicPara.tszPanelID );
	CreateDirectory(pathAlgorithms[0], NULL);

	// �ܰ��� ���� ���� ���
	swprintf( pathAlgorithms[1], _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, LogicPara.tszPanelID );
	
	// Merge ���� ���� ���
	swprintf( pathAlgorithms[2], _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, LogicPara.tszPanelID );
	if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
	{
		swprintf( pathAlgorithms[2], _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strMergeToolPath, LogicPara.tszPanelID );
	}

	// �г� ID ���� ����
	swprintf( pathAlgorithms[3], _T("%s"), LogicPara.tszPanelID );

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum) );
	
	// SVI Main �˻� �˰���
	long lErrorCode = SVI_FindMuraDefect(MatOriginImage_, MatBKG_, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, pathAlgorithms, EngineerDefectJudgment, pResultBlob, strAlgLog);			

	// ������ ���, ���� �ڵ� & �α� ���
	if( lErrorCode != E_ERROR_CODE_TRUE )
	{
		// Alg DLL End Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End Fail.  PanelID: %s, CAM: %02d ROI: %02d, Img: %s Error Code:%03d."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), lErrorCode);

		// �޸� ����
		delete pResultBlob;
		pResultBlob = NULL;

		return true;
	}

	// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
	m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);

	// �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
	int nTotalblob = pResultBlob->nDefectCount;
	
	// �ʿ��� ��� ���
	if( theApp.GetCommonParameter()->bIFImageSaveFlag )
	{
		// Alg �߰� ��� ���
		// �˰��� �ҷ� ��� ���� ����
		// MatDrawBuffer ���� ���� ǥ���ؾ� �� ������.
		// m_fnDrawDefectImage() �Լ����� �������� �뵵�� ��� ��.
		// ���� ������ ǥ���ص� �Ǵ��� Ȯ�� �ʿ�
		// Judgement() ���� ��� ��...

		CString strResultROIFile = _T("");
		strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"), 
			INSP_PATH, LogicPara.strPanelID, nImageNum, nCameraNum_, nROINumber, 
			theApp.GetAlgorithmName(nAlgorithmNumber_));

		// �ҷ� ���� ����
		BlobFeatureSave(pResultBlob, strResultROIFile);
	}

	// �ҷ� ���� ����
	pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pResultBlob, NULL);

	// Alg DLL End Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( DefectNum: %d )"),
		theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

	return true;
}

// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
bool SviInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// SVI �� ������ ���ش����� Cell ������ Warping �Ͽ� ��ǥ ���� �ϹǷ� �״�� ���
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP]		= cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP]	= cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM]	= cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM]	= cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);

	pResultDefectInfo->Pixel_Start_X		= pResultBlob->ptLT[nBlobCnt].x;
	pResultDefectInfo->Pixel_Start_Y		= pResultBlob->ptLT[nBlobCnt].y;
	pResultDefectInfo->Pixel_End_X			= pResultBlob->ptRB[nBlobCnt].x;
	pResultDefectInfo->Pixel_End_Y			= pResultBlob->ptRB[nBlobCnt].y;

	pResultDefectInfo->Pixel_Center_X		= (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y		= (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// ���� �̹��� ���� Crop �� �ҷ� ���� ����
	pResultDefectInfo->Pixel_Crop_Start_X	= pResultBlob->ptLT[nBlobCnt].x;
	pResultDefectInfo->Pixel_Crop_Start_Y	= pResultBlob->ptLT[nBlobCnt].y;
	pResultDefectInfo->Pixel_Crop_End_X		= pResultBlob->ptRB[nBlobCnt].x;
	pResultDefectInfo->Pixel_Crop_End_Y		= pResultBlob->ptRB[nBlobCnt].y;

	// BOE Eng'r ��û Defect Size
	//pResultDefectInfo->Defect_Size			= (int)(pResultBlob->nArea[nBlobCnt] * WrtResultInfo.GetCamResolution(nCameraNum));
	pResultDefectInfo->Defect_Size		= (int)WrtResultInfo.CalcDistancePixelToUm(sqrt(pow((double)(pResultDefectInfo->Pixel_End_X - pResultDefectInfo->Pixel_Start_X), 2) +
																						pow((double)(pResultDefectInfo->Pixel_End_Y - pResultDefectInfo->Pixel_Start_Y), 2)), 
																						nCameraNum, nRatio);
	pResultDefectInfo->Defect_MeanGV		= pResultBlob->dMeanGV[nBlobCnt];
	pResultDefectInfo->Defect_Size_Pixel	= (int)pResultBlob->nArea[nBlobCnt];
	pResultDefectInfo->Img_Number			= nImageNum;
	pResultDefectInfo->Img_Size_X			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_Start_X - pResultDefectInfo->Pixel_Crop_Start_Y);	// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Img_Size_Y			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_X - pResultDefectInfo->Pixel_Crop_End_Y);		// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Defect_Type			= pResultBlob->nDefectJudge[nBlobCnt];
	pResultDefectInfo->Pattern_Type			= pResultBlob->nPatternClassify[nBlobCnt];
	pResultDefectInfo->Camera_No			= nCameraNum;
	pResultDefectInfo->nRatio				= nRatio;

#if USE_ALG_CONTOURS
	// KSW 18.02.05 - �ܰ� ���� ( ���� �� )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
#endif

	return true;
}

bool SviInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
{
	// �ӽ÷� �����簢�� ����
// 	ptAdjCorner[E_CORNER_LEFT_TOP].x =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_LEFT_TOP].y =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);
// 	ptAdjCorner[E_CORNER_RIGHT_TOP].x =		max(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_RIGHT_TOP].y =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);
// 	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x =	max(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y =	max(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y);
// 	ptAdjCorner[E_CORNER_LEFT_BOTTOM].x =	min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_LEFT_BOTTOM].y =	max(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y);

	//////////////////////////////////////////////////////////////////////////
	// ���� ���
	//////////////////////////////////////////////////////////////////////////

	// Ư�� ��ġ�� Warping
	//const int TARGET_WIDTH = 912;
	//const int TARGET_HEIGHT = 513;

	//ptAdjCorner[E_CORNER_LEFT_TOP].x =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x);
	//ptAdjCorner[E_CORNER_LEFT_TOP].y =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);

	//ptAdjCorner[E_CORNER_RIGHT_TOP].x =		ptAdjCorner[E_CORNER_LEFT_TOP].x + TARGET_WIDTH;
	//ptAdjCorner[E_CORNER_RIGHT_TOP].y =		ptAdjCorner[E_CORNER_LEFT_TOP].y;

	//ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x =	ptAdjCorner[E_CORNER_LEFT_TOP].x + TARGET_WIDTH;
	//ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y =	ptAdjCorner[E_CORNER_LEFT_TOP].y + TARGET_HEIGHT;

	//ptAdjCorner[E_CORNER_LEFT_BOTTOM].x =	ptAdjCorner[E_CORNER_LEFT_TOP].x;
	//ptAdjCorner[E_CORNER_LEFT_BOTTOM].y =	ptAdjCorner[E_CORNER_LEFT_TOP].y + TARGET_HEIGHT;

	//////////////////////////////////////////////////////////////////////////
	// KSW 18.02.12 - ���� Cell ũ�� ����ϵ��� Warp �ϱ� ���� ����
	// Ȯ��� ��� ������, ��Ұ� �Ǹ� �ȵ� ( ���� ����� )
	//////////////////////////////////////////////////////////////////////////

	CRect rectTempROI = CRect(
		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x	),
		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y		),
		max(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
		max(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y	));

	ptAdjCorner[E_CORNER_LEFT_TOP].x		= rectTempROI.left;
	ptAdjCorner[E_CORNER_LEFT_TOP].y		= rectTempROI.top;
	ptAdjCorner[E_CORNER_RIGHT_TOP].x		= rectTempROI.right;
	ptAdjCorner[E_CORNER_RIGHT_TOP].y		= rectTempROI.top;
	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x	= rectTempROI.right;
	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y	= rectTempROI.bottom;
	ptAdjCorner[E_CORNER_LEFT_BOTTOM].x		= rectTempROI.left;
	ptAdjCorner[E_CORNER_LEFT_BOTTOM].y		= rectTempROI.bottom;

	return true;
}

bool SviInspection::AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)
{
	cv::Point2f ptSrc[4] = {cv::Point2f( (float)m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x,	(float)m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y),
							cv::Point2f( (float)m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x,	(float)m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y),
							cv::Point2f( (float)m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x,(float)m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y),
							cv::Point2f( (float)m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x,	(float)m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y) };

	cv::Point2f ptDst[4] = {cv::Point2f( (float)pStAlignInfo->rcAlignCellROI.x,	(float)pStAlignInfo->rcAlignCellROI.y),
							cv::Point2f( (float)pStAlignInfo->rcAlignCellROI.x + pStAlignInfo->rcAlignCellROI.width,	(float)pStAlignInfo->rcAlignCellROI.y),
							cv::Point2f( (float)pStAlignInfo->rcAlignCellROI.x + pStAlignInfo->rcAlignCellROI.width,	(float)pStAlignInfo->rcAlignCellROI.y + pStAlignInfo->rcAlignCellROI.height),
							cv::Point2f( (float)pStAlignInfo->rcAlignCellROI.x,	(float)pStAlignInfo->rcAlignCellROI.y + pStAlignInfo->rcAlignCellROI.height) };

	cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);		//	Warping ���
	cv::warpPerspective(MatOrgImage, MatOrgImage, mat2Warp, MatOrgImage.size(), CV_INTER_AREA);
	cv::warpPerspective(MatDrawImage, MatDrawImage, mat2Warp, MatDrawImage.size(), CV_INTER_AREA);		// Draw Buffer �� ���� Warping �ؾ���

	// SVI �� ���� ��ü�� ��ȯ�ϹǷ� Corner ���� ����
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x			= (int)ptDst[E_CORNER_LEFT_TOP].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y			= (int)ptDst[E_CORNER_LEFT_TOP].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x		= (int)ptDst[E_CORNER_RIGHT_TOP].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y		= (int)ptDst[E_CORNER_RIGHT_TOP].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x		= (int)ptDst[E_CORNER_RIGHT_BOTTOM].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y		= (int)ptDst[E_CORNER_RIGHT_BOTTOM].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x		= (int)ptDst[E_CORNER_LEFT_BOTTOM].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y		= (int)ptDst[E_CORNER_LEFT_BOTTOM].y;

	// ���� 0���� ���� ����
	m_stThrdAlignInfo.dAlignTheta = 0.0;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align �ϱ� ��, AVI & SVI ���� �̻� Ȯ��
//////////////////////////////////////////////////////////////////////////
long SviInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
{
	CCPUTimer tact;
	tact.Start();

	// ���� �˰��� �˻� �Ķ���� ��������
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();
	
	// ���� ���� Ʋ���ų�, ������ �ȵȰ�� AD �ҷ��̹Ƿ� �˻� ���ص� ��. ( Align ���� ���� )
	long nErrorCode = Align_FindDefectAD(MatOrgImage, dAlgPara, dResult, nRatio, nCameraNum, nEqpType);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Inspect AD : %.2f"), tact.Stop(false)/1000.);

	return nErrorCode;
}

// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
bool SviInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
{
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (true)	// POINT �� ���
		{
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
			resultPanelData.m_ListDefectInfo[i].bUseReport = true;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align ��,ROI ��� �̻� Ȯ��
//////////////////////////////////////////////////////////////////////////
long SviInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
							bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)
{ 	
	CCPUTimer tact;
	tact.Start();
	long nErrorCode = E_ERROR_CODE_TRUE;

	// ��� Ÿ��
	int nEqpType = theApp.m_Config.GetEqpType();

	// �˰��� ���� ��ȣ
	int nAlgImg = theApp.GetImageClassify(nImageNum);

	// Alg �α׿�
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID, theApp.GetGrabStepName(nImageNum) );

	// ����� AD GV�� Ȯ�� �ڵ�
	if( theApp.GetUseFindDefectAD(nImageNum, nCameraNum) )
	{
		// ���� �˰��� �˻� �Ķ���� ��������
		// AD Parameter ������ ������ ��. ToDo.
		double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum, nCameraNum);

		// AD - ��Ⱚ Ȯ��
		nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

		theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Check AD GV %.2f"), tact.Stop(false)/1000.);

		// ������ ���, ���� �ڵ� & �α� ���
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// Alg DLL Stop Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("AD GV Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Avg : %.2f GV )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);

			// ��� �̻�
			if (nErrorCode == E_ERROR_CODE_ALIGN_DISPLAY)
			{
				JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, (int)dMeanResult[1], eInspMode);
			}
			// ���۰� ���� ��� / �Ķ���� �̻��� ��� / ���...
			else
			{
				// ���� �ҷ����� �����ϴ� ��� �ʿ� - ���� AD GV �ҷ����� �ϰ� ����
				JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, E_DEFECT_JUDGEMENT_DISPLAY_BRIGHT, eInspMode);
			}

			// Alg DLL End Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End Fail. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);
		}
		else
		{
			// Alg DLL End Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("AD GV Algorithm End. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s.\n\t\t\t\t( Avg : %.2f GV )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);
		}			
	}

	return nErrorCode;
}

// �ߺ� ��ǥ ����
bool SviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData)
{
	// nRatio : �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// �ҷ� ����Ʈ ���� ��� ������
	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
		return true;

	CRect	rcDeleteArea;
	CPoint	ptCenterSub;
	int		nAdjustRange	= 0;
	int		j				= 0;
	int		nOffSet			= 0;

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// �ҷ� ����
		int nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		// AD �ҷ� ����
		if ( nDefectTypeM <= E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_END ||
			 nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_CM )
			continue;

		CRect rcDeleteArea = CRect(
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_X		/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio);
					
		// ��ǥ�� ����ȭ
		rcDeleteArea.NormalizeRect();

		// ���� Ȯ��
		rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

		// �ҷ� ���� ��ŭ
		for (j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
		{
			// ���� �ҷ� �� X
			if( i == j )
			{
				j++;
				continue;
			}

			// �ҷ� ����
			int nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

			// AD �ҷ� ����
			if ( nDefectTypeS <= E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_END ||
				 nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_CM )
			{
				j++;
				continue;
			}

			// ���� �ҷ� ������ �ߺ� ����
			if ( nDefectTypeM != nDefectTypeS )
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptPointS;
			ptPointS.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptPointS.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// ���� �ȿ� �ҷ� ����?
			if ( rcDeleteArea.PtInRect(ptPointS) )
			{
				// �ҷ� ���� ����
				int nPat = resultPanelData.m_ListDefectInfo[j].Img_Number;

				// White ������ �ִ� ���
				if( theApp.GetImageClassify(nPat) == E_IMAGE_CLASSIFY_AVI_WHITE )
				{
					// White �������� ���� ����
					ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
					resultPanelData.m_ListDefectInfo[i] = tTemp;

					// White ���� ũ��� ���� ����
					rcDeleteArea = CRect(
						tTemp.Pixel_Start_X	/ tTemp.nRatio,
						tTemp.Pixel_Start_Y	/ tTemp.nRatio,
						tTemp.Pixel_End_X	/ tTemp.nRatio,
						tTemp.Pixel_End_Y	/ tTemp.nRatio);
				}

				// ���� ����Ʈ���� ���� ����Ʈ ���� ��...
				if( i > j )	i--;

				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}
	}

	return true;
}


// �ܰ� ���� ( ���� �� )
bool SviInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
#if USE_ALG_CONTOURS

	// �ܰ��� ���� ��� ���� ���
	if( strContourTxt == NULL )			return false;

	// �ҷ� ���ų� �˻� ���� ��� Skip
	//if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT ����
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// ���� ����
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		// ȸ�� ��ǥ ��� ��, ���
		double dTheta	= -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		int nCx			= m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy			= m_stThrdAlignInfo.ptAlignCenter.y;

		// ȸ���� ���� ����
		cv::Point ptCorner[E_CORNER_END];

		// ȸ�� ��, ���� ��ǥ ���
		for (int m=0 ; m<E_CORNER_END ; m++)
		{			
			ptCorner[m].x = (int)( dCos * (m_stThrdAlignInfo.ptCorner[m].x - nCx) - dSin * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCx );
			ptCorner[m].y = (int)( dSin * (m_stThrdAlignInfo.ptCorner[m].x - nCx) + dCos * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCy );
		}

		// ����� ����
		CRect rectROI = CRect(
			min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
			min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
			max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
			max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

		// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
		rectROI.left	/= m_stThrdAlignInfo.nRatio;
		rectROI.top		/= m_stThrdAlignInfo.nRatio;
		rectROI.right	/= m_stThrdAlignInfo.nRatio;
		rectROI.bottom	/= m_stThrdAlignInfo.nRatio;

		// �ֻ��� ��� �ۼ� ( Cell ũ�� )
		strContourIndex.Format(_T("Cell_X=%d, Cell_Y=%d\n"), rectROI.Width(), rectROI.Height());
		fileWriter.SeekToEnd();
		fileWriter.WriteString(strContourIndex);

		// �ҷ� ���� ��ŭ
		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++ )
		{
			// UI ���� ����
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// ��� ���� ���� ( No. || ���� ���� || ī�޶� ��ȣ || �ҷ��� )
			strContourIndex.Format(_T("No=%d, Pattern=%02d, Camera=%02d, Defect=%02d\n"), i+1, theApp.GetImageClassify(nImgNum), resultPanelData.m_ListDefectInfo[i].Camera_No, resultPanelData.m_ListDefectInfo[i].Defect_Type);
			fileWriter.SeekToEnd();
			fileWriter.WriteString(strContourIndex);

			// AD �ҷ��� ���
			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP )
			{
				// �簢�� E_CORNER_LEFT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), 0, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_TOP ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width()-1, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_RIGHT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width()-1, rectROI.Height()-1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// �簢�� E_CORNER_LEFT_BOTTOM ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), 0, rectROI.Height()-1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				continue;
			}

			// SVI�� ����ҷ��̹Ƿ�, ��� ǥ��
			// �ܰ��� ����
			for (int j=0 ; j<MAX_CONTOURS ; j++)
			{
				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				// CFeatureExtraction::calcContours() ���� �̹� 1�� ��ǥ�� ����Ǽ� ����
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// ���� ���� ��� ������
				if( x==0 && y==0 )	break;

				// E_CORNER_LEFT_TOP ��ǥ�� �������� ���
				x -= rectROI.left;
				y -= rectROI.top;

				// ����ó��
				if( x < 0 )						x = 0;
				if( y < 0 )						y = 0;
				if( x >= rectROI.Width() )		x = rectROI.Width()-1;
				if( y >= rectROI.Height() )		y = rectROI.Height()-1;

				// �ܰ��� ���� ���� ����
				strContourIndex.Format(_T("%d, %d\n"), x, y);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);
			}
		}

		// ���� ���� ��츸 �ݱ�
		fileWriter.Close();
	}

#endif

	return true;
}


// Mura �ܰ��� ���� ����
bool SviInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
	// �ܰ��� ���� ��� ���� ���
	if( strContourTxt == NULL )			return false;

	// �ҷ� ���ų� �˻� ���� ��� Skip
	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT ����
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// ���� ����
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		// ȸ�� ��ǥ ��� ��, ���
		double dTheta	= -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		int nCx			= m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy			= m_stThrdAlignInfo.ptAlignCenter.y;

		// �ҷ� ���� ��ŭ
		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++ )
		{
			// AD �ҷ��� ���
			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP )
				continue;

			// �ҷ� ����
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// SVI�� ����ҷ��̹Ƿ�, ��� ǥ��

			// �ܰ��� ����
			for (int j=0 ; j<MAX_CONTOURS ; j++)
			{
				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				// CFeatureExtraction::calcContours() ���� �̹� 1�� ��ǥ�� ����Ǽ� ����
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// ���� ���� ��� ������
				if( x==0 && y==0 )	break;

				// �ܰ��� ���� ���� ����
				strContourIndex.Format(_T("%d, %d, %s\n"), x, y, theApp.GetDefectTypeName(nDefectType));

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);
			}
		}

		// ���� ���� ��츸 �ݱ�
		fileWriter.Close();
	}

	return true;
}
