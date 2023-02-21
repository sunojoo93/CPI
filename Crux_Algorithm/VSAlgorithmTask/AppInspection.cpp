
#include "StdAfx.h"
#include "AppInspection.h"

#include "AppDefineInspect.h"
#include "DllInterface.h"

void ImageSave(cv::Mat& MatSrcBuffer, TCHAR* strPath, ...);
void ImageAsyncSaveJPG(cv::Mat& MatSrcBuffer, const char* strPath);

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace Concurrency;

IMPLEMENT_DYNCREATE(AppInspection, CWinThread)

AppInspection::AppInspection(void)
{
}


AppInspection::~AppInspection(void)
{
}

int GetBitFromImageDepth(int nDepth)
{
	int nRet = -1;

	switch(nDepth)
	{
	case CV_8U :
	case CV_8S :
		nRet = 1;
		break;

	case CV_16U :
	case CV_16S :
		nRet = 2;
		break;

	case CV_32S :
	case CV_32F :
		nRet = 4;
		break;

	case CV_64F :
		nRet = 8;
		break;

	default:
		nRet = -1;
		break;
	}

	return nRet;

}

long GetHistogram(Mat& matSrcImage, Mat& matHisto)
{
	int nBit = GetBitFromImageDepth(matSrcImage.depth());
	if(nBit == -1)	return E_ERROR_CODE_APP_HISTO;

	int nHistSize = (int)pow((double)256,(double)nBit);
	float fHistRange[] = {0, (float)nHistSize-1};
	const float *ranges[] = { fHistRange };

	cv::calcHist(&matSrcImage,1, 0, Mat(), matHisto, 1, &nHistSize, ranges, true, false);

	return E_ERROR_CODE_TRUE;
}


bool GetAvgOfStdevRange(Mat matHisto, Scalar Avg, Scalar Stdev, double& dReturnAvg)
{
	int nMin = (int)(Avg[0] - Stdev[0]);
	int nMax = (int)(Avg[0] + Stdev[0]);

	if(nMin < 0)
		nMin = 0;
	if(nMax > 255)
		nMax = 255;

	double dSum = 0;
	int nSumCnt = 0;

	float* fHistData = (float*)matHisto.data + nMin;

	for(int n = nMin; n < nMax; n++, fHistData++)
	{
		// 		if((int)*fHistData == 0)
		// 			continue;
		dSum += *fHistData * n;
		nSumCnt += (int)*fHistData;
	}

	dReturnAvg = dSum / nSumCnt;

	return true;

}

bool AppInspection::DrawDefectImage(CString strPanelID, cv::Mat (*MatResult)[MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE], cv::Mat MatDrawBuffer[][MAX_CAMERA_COUNT], ResultPanelData& resultPanelData)
{
	CCPUTimer timerDrawRect;
	timerDrawRect.Start();

	// �ҷ� ���� ��ŭ
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if(resultPanelData.m_ListDefectInfo[i].Defect_Type != E_DEFECT_JUDGEMENT_APP_CURL)
			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;	// �̴ϸʿ� Rect �ȱ׸�
	}
	

//	// �ҷ� ���� ��ŭ
//	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
//	{
//		// [Draw & Save] �ҷ� ǥ��
//		cv::Scalar color;
//
//		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
//		{
//		case E_DEFECT_JUDGEMENT_POINT_DARK:
//		case E_DEFECT_JUDGEMENT_POINT_ADJOIN_DARK:
//		case E_DEFECT_JUDGEMENT_POINT_GROUP_DARK:
//		case E_DEFECT_JUDGEMENT_SPOT_DARK:
//			color = cv::Scalar(255,0,0);		// Red
//			break;
//
//		case E_DEFECT_JUDGEMENT_POINT_BRIGHT:
//		case E_DEFECT_JUDGEMENT_POINT_ADJOIN_BRIGHT:
//		case E_DEFECT_JUDGEMENT_POINT_GROUP_BRIGHT:
//		case E_DEFECT_JUDGEMENT_SPOT_BRIGHT:
//			color = cv::Scalar(0,255,0);		// Green
//			break;
//
//		case E_DEFECT_JUDGEMENT_POINT_PARTICLE:
//			color = cv::Scalar(0,0,255);		// Blue
//			break;
//
//		case E_DEFECT_JUDGEMENT_LINE_X_BRIGHT:
//		case E_DEFECT_JUDGEMENT_LINE_Y_BRIGHT:
//		case E_DEFECT_JUDGEMENT_LINE_X_DARK:	
//		case E_DEFECT_JUDGEMENT_LINE_Y_DARK:	
//		case E_DEFECT_JUDGEMENT_LINE_DGS:
//		case E_DEFECT_JUDGEMENT_LINE_X_OPEN:
//		case E_DEFECT_JUDGEMENT_LINE_Y_OPEN:
//			color = cv::Scalar(255,105,180);	// Hot Pink
//			break;
//
//		case E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL:
//		case E_DEFECT_JUDGEMENT_DISPLAY_BRIGHTENSS:
//			color = cv::Scalar(210,105,30);		// Chocolate
//			break;
//
//		case E_DEFECT_JUDGEMENT_MURA_LINE_X:	
//		case E_DEFECT_JUDGEMENT_MURA_LINE_Y:	
//		case E_DEFECT_JUDGEMENT_MURA_CD:		
//		case E_DEFECT_JUDGEMENT_MURA_WE:		
//		case E_DEFECT_JUDGEMENT_MURA_M2:		
//		case E_DEFECT_JUDGEMENT_MURA_EMD:
//		case E_DEFECT_JUDGEMENT_MURA_BLOB:			
//		case E_DEFECT_JUDGEMENT_MURA_AMORPH_BRIGHT:	
//		case E_DEFECT_JUDGEMENT_MURA_AMORPH_DARK:		
//		case E_DEFECT_JUDGEMENT_MURA_DIAGONAL:
//			color = cv::Scalar(135,206,250);	// Light SkyBlue
//			break;
//
//		default:
//			color = cv::Scalar(0,0,0);			// Black
//			break;
//		}
//
//		int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;
//		int nCamNum = resultPanelData.m_ListDefectInfo[i].Camera_No;
//
//		// 17.03.29 Mura & SVI �簢�� �׸��� �ʱ� ( Alg���� �ܰ��� ���� �׸� )
//		// ���� �߶󳻴� �κ��� �ؾ� ��
//		int nOffSet = 5, nThickness = 1;
//// 		// ���� Defect ����
//// 		int nDefStartX, nDefStartY, nDefWidth, nDefHeight;
//// 		
//// 		nDefStartX = resultPanelData.m_ListDefectInfo[i].Pixel_Start_X - nOffSet;
//// 		nDefStartY = resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y - nOffSet;
//// 		// ���밪 �ӽ� ��� (by CWH)
//// 		nDefWidth  = abs(resultPanelData.m_ListDefectInfo[i].Pixel_End_X - resultPanelData.m_ListDefectInfo[i].Pixel_Start_X) + (nOffSet * 2) + 1;
//// 		nDefHeight = abs(resultPanelData.m_ListDefectInfo[i].Pixel_End_Y - resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y) + (nOffSet * 2) + 1;
//		cv::Point ptDefectArea[4] = {(0, 0), };
//		ptDefectArea[E_CORNER_LEFT_TOP].x		= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].x		- nOffSet;
//		ptDefectArea[E_CORNER_LEFT_TOP].y		= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_TOP].y		- nOffSet;
//		ptDefectArea[E_CORNER_RIGHT_TOP].x		= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].x		+ nOffSet;
//		ptDefectArea[E_CORNER_RIGHT_TOP].y		= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_TOP].y		- nOffSet;
//		ptDefectArea[E_CORNER_RIGHT_BOTTOM].x	= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].x	+ nOffSet;
//		ptDefectArea[E_CORNER_RIGHT_BOTTOM].y	= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM].y	+ nOffSet;
//		ptDefectArea[E_CORNER_LEFT_BOTTOM].x	= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].x		- nOffSet;
//		ptDefectArea[E_CORNER_LEFT_BOTTOM].y	= resultPanelData.m_ListDefectInfo[i].Defect_Rect_Point[E_CORNER_LEFT_BOTTOM].y		+ nOffSet;
//		
//
//		if (resultPanelData.m_ListDefectInfo[i].Defect_Type >= E_DEFECT_JUDGEMENT_MURA_LINE_X	&&
//			resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_EDGE_HS		)
//		{
//			// �̴ϸʿ� Rect �ȱ׸�
//			resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
//		}
//		// ���� �̿��� �ҷ� / �簢�� �׸���
//		else
//		{
//			// �簢�� �׸���
//// 			cv::rectangle(MatDrawBuffer[nImgNum][nCamNum], cv::Rect(nDefStartX, nDefStartY, nDefWidth, nDefHeight), color);
//			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_TOP],		ptDefectArea[E_CORNER_RIGHT_TOP],		color, nThickness);
//			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_TOP],		ptDefectArea[E_CORNER_RIGHT_BOTTOM],	color, nThickness);
//			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_RIGHT_BOTTOM],	ptDefectArea[E_CORNER_LEFT_BOTTOM],		color, nThickness);
//			cv::line(MatDrawBuffer[nImgNum][nCamNum], ptDefectArea[E_CORNER_LEFT_BOTTOM],	ptDefectArea[E_CORNER_LEFT_TOP],		color, nThickness);
//		}
//	}
//	timerDrawRect.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Draw Rect tact time %.2f"), timerDrawRect.GetTime()/1000.);

	return true;
}

bool AppInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
								const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio,const int nCamNum,
								ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStageNo)
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
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Delete Overlap Defect End. Panel ID : %s"), strPanelID);
		
		//	18.04.16 ������
		//	MergeTool ����
		//	�Ӱ��K �� ���� �� �ӽ� ����
		if( theApp.GetMergeToolUse() )
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

		// KSW 17.11.30 - AVI & SVI �ܰ� ���� ���� ���	
		wchar_t strContoursPath[MAX_PATH] = {0};
		CString strResultPath = RESULT_PATH;

		// Contours.Merge ���� ��� ���� 180322 YSS
		swprintf( strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID );

		// KSW 17.11.29 - AVI & SVI & APP �ܰ� ���� ���� ( AVI & SVI & APP ���� �� )
		JudgeSaveContours(resultPanelData, strContoursPath);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours End. Panel ID : %s"), strPanelID);

		// AutoRun ��忡�� MergeTool ��� Ȱ��ȭ�� ��� D:\MergeTool\ ��γ��� ���� ���� ���� 
		if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
		{
			CString strDest;
			strDest.Format(_T("%s\\%s\\Contours.Merge"), MERGETOOL_PATH, strPanelID);	
			CopyFile(strContoursPath, strDest, FALSE);
		}

		// APP �ܰ��� ���� ���� ���
		swprintf( strContoursPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID );

		// APP �ܰ��� ���� ����
		JudgeSaveMuraContours(resultPanelData, strContoursPath);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save APP Contours End. Panel ID : %s"), strPanelID);	
	}

	JudgementRepair(strPanelID, resultPanelData, WrtResultInfo);
	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Repair End. Panel ID : %s"), strPanelID);	

	// 17.07.12 Panel Grade ���� �߰�
	JudgementPanelGrade(resultPanelData);

	// Work ��ǥ ��� �� �ҷ� ������� Numbering
	NumberingDefect(strModelID, strPanelID, strLotID, WrtResultInfo, resultPanelData, nRatio);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Numbering Defect End. Panel ID : %s"), strPanelID);

	// 18.09.18 Repeat Defect APP
	if (eInspMode == eAutoRun)
	{
		JudgementRepeatCount(strPanelID, resultPanelData, nStageNo);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judge - RepeatCount End. Panel ID : %s"), strPanelID);
	}

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

bool AppInspection::JudgementPointToLine(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, const int nImageWidth, const int nImageHeight)
{
	return true;
}

bool AppInspection::JudgeGroup(ResultPanelData& resultPanelData, cv::Mat (*MatDraw)[MAX_CAMERA_COUNT])
{
	return true;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	StartLogicAlgorithm
//	 2. �Լ���� :	Algorithm DLL
//	 3. ��    �� :	
//	 4. �� ȯ �� :	�Ϸ� : true
//					���� : false
//	 5. �� �� �� :	16.02.29
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :  �˰��� ����
//----------------------------------------------------------------------------------------------------
long AppInspection::StartLogicAlgorithm(const CString strDrive , const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// �Ķ���� ����
	cv::Mat				MatOriginImage_			= LogicPara.MatOrgImage;
	int					nAlgorithmNumber_		= nAlgorithmNumber;					// ���� �˰���	
	int					nCameraNum_				= LogicPara.nCameraNum;				// �˻� ī�޶�
	int					nThrdIndex_				= LogicPara.nThreadLog;
	double				dCameraResolution	    = WrtResultInfo.GetCamResolution(nCameraNum_);

	ENUM_KIND_OF_LOG	eLogCamera				= (ENUM_KIND_OF_LOG)nCameraNum_;
	
	cv::Point 			ptAlignedEdgeCorner[E_CORNER_END] = 
						{
							Point(stThrdAlignInfo.rcAlignCellROI.x, stThrdAlignInfo.rcAlignCellROI.y),		// LT
							Point(stThrdAlignInfo.rcAlignCellROI.x + stThrdAlignInfo.rcAlignCellROI.width, stThrdAlignInfo.rcAlignCellROI.y),
							Point(stThrdAlignInfo.rcAlignCellROI.x + stThrdAlignInfo.rcAlignCellROI.width, stThrdAlignInfo.rcAlignCellROI.y + stThrdAlignInfo.rcAlignCellROI.height),
							Point(stThrdAlignInfo.rcAlignCellROI.x, stThrdAlignInfo.rcAlignCellROI.y + stThrdAlignInfo.rcAlignCellROI.height)
						};


	// ���� �г� ID ����
	CString	strPanelID_ = _T("");
	strPanelID_.Format(_T("%s"), LogicPara.tszPanelID);

	long lErrorCode = E_ERROR_CODE_TRUE;

	// �˰��� ��� ���� ���� ����
	SYSTEMTIME time;
	::GetLocalTime(&time);

	// ���� �˰��� �˻� �Ķ���� ��������
	double * dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

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
		nRatio											,	// 14 : �̹��� ���� (Pixel Shift Mode - 0:None, 1:4-Shot, 2:9-Shot)
	};

	// �˻� ���(�ҷ�) ���� ����ü
	stDefectInfo * pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// �����Ͼ� ���� ����ü ��������
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."), theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// �ܰ��� ���� ���
	wchar_t fPath[255] = {0};
	CString strResultPath = RESULT_PATH;
	CString strAlgResultPath = ALG_RESULT_PATH;

	swprintf( fPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID_ );

	wchar_t fMeasurePath[255] = {0};
	swprintf(fMeasurePath, _T("%s\\Measure"), (LPCWSTR)strResultPath);

	// �߰� ���� ���� ��� & ���� ����
 	wchar_t fAlgPath[255] = {0, };
 	swprintf( fAlgPath, _T("%s\\%s\\"), (LPCWSTR)strAlgResultPath, (LPCWSTR)strPanelID_ );

	if (MatOriginImage_.type() != CV_8UC1)
	{
		cv::cvtColor(MatOriginImage_, MatOriginImage_, CV_BGR2GRAY);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �˰��� DLL ����
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CString strResultColumn		= _T("");		//���� ��� ���Ͽ� Column
	CString strCurRecipePath	= _T("");
	strCurRecipePath.Format(_T("%s"), theApp.GetCurInspRecipePath());

	//APP �˻� ROI ����ü
	STRU_INFO_PAD ** stPadInfo =  theApp.GetPadAreaInfo();
	
	switch (nAlgorithmNumber_)
	{
	case E_ALGORITHM_APP_CUTTING:
		{
			//��˻� ���� Blob ��� ���� �ѱ�� ���� ���������� ����Ϸ� ������ ���� ���� by LJM 2017.10.21
// 			CRect* rtFilterROI;
// 			int nFilterROICnt = theApp.GetFilterROICnt(nImageNum,nCameraNum_);
// 			rtFilterROI = new CRect[nFilterROICnt];
// 			for(int nFIlterROIInx =0; nFIlterROIInx < nFilterROICnt; nFIlterROIInx++)
// 			{
// 				if(theApp.GetUseFilterROI(nImageNum, nCameraNum_,nFIlterROIInx))
// 					rtFilterROI[nFIlterROIInx] = theApp.GetFilterROI(nImageNum, nCameraNum_, nFIlterROIInx, nRatio);
// 			}
			lErrorCode = APP_FindDefect_Cutting(MatOriginImage_, MatDrawBuffer, ptAlignedEdgeCorner,dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob);
			/*delete[] rtFilterROI;*/
		} break;		
	case E_ALGORITHM_APP_CUT_C:
		{
			//Reference Pattern ������ Model ������ �����Ͽ� ����ϱ� ������ Model��θ� �޾ƿ� ��� 2017.09.16 by LJM
			CString strCurInspRecipePath;
			strCurInspRecipePath.Format(_T("%s"), theApp.GetCurInspRecipePath());

			stMeasureInfo* pMeasureInfo = new stMeasureInfo(12);
			pMeasureInfo->strPanelID = strPanelID_;
			pMeasureInfo->dCameraResolution = dCameraResolution;

			if (stPadInfo == NULL) 
			{
				lErrorCode = E_ERROR_CODE_PAD_PARAM_IS_NULL	;				
				theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, PAD Parameter Is NULL!!!"), theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_);			
			}
			else
			{
				lErrorCode = APP_FindDefect_CCut(MatOriginImage_, MatDrawBuffer, ptAlignedEdgeCorner, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, strCurInspRecipePath, pResultBlob, pMeasureInfo, stPadInfo);
			}

			//C-Cut���� ��� ��� 2017.09.21 LJM
			strResultColumn.Format(_T("TIME,PANEL_ID,JUDGE,LT_MIN,LT_MAX,LT_AVG,RT_MIN,RT_MAX,RT_AVG,RB_MIN,RB_MAX,RB_AVG,LB_MIN,LB_MAX,LB_AVG"));
			WriteMeasureResultFile(fMeasurePath, _T("C_Cut.csv"), strResultColumn, pMeasureInfo);
		} break;
	case E_ALGORITHM_APP_PANEL_MEASURE:
		{
			//���� ���Data ����ü
			stMeasureInfo* pMeasureInfo = new stMeasureInfo(MAX_MEASURE_RESULT_DATA);
			pMeasureInfo->strPanelID = strPanelID_;
			pMeasureInfo->dCameraResolution = dCameraResolution;


			//���� ������ ��ǥ�� ������������ Ȯ���ϱ� ���� ���� 2017.09.21 LJM
			Mat mtMeasureImage;
			cv::cvtColor(MatOriginImage_, mtMeasureImage, CV_GRAY2RGB);

			vector<CRect> pFilterROI;
			int nFilterROICnt = theApp.GetFilterROICnt(nImageNum,nCameraNum_);
			for (int nFIlterROIInx = 0; nFIlterROIInx < nFilterROICnt; nFIlterROIInx++)
			{
				if (theApp.GetUseFilterROI(nImageNum, nCameraNum_,nFIlterROIInx) == TRUE)
				{
					pFilterROI.push_back(theApp.GetFilterROI(nImageNum, nCameraNum_, nFIlterROIInx, nRatio));
				}
			}

			if (stPadInfo == NULL) 
			{
				lErrorCode = E_ERROR_CODE_PAD_PARAM_IS_NULL;
				theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, PAD Parameter Is NULL!!!"), theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_);			
			}
			else
			{
				lErrorCode = APP_PanelMeasure(MatOriginImage_, MatDrawBuffer, ptAlignedEdgeCorner, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, pMeasureInfo, mtMeasureImage, strCurRecipePath, pFilterROI, stPadInfo);
			}
			
			//�ܰ� ���� ��� ��� 2017.09.20 LJM
			strResultColumn.Format(_T("TIME,PANEL_ID,JUDGE,LEFT_ST,LEFT_ED,LEFT_AVG,TOP_ST,TOP_ED,TOP_AVG,RIGHT_ST,RIGHT_ED,RIGHT_AVG,BOTTOM_ST,BOTTOM_ED,BOTTOM_AVG"));
			WriteMeasureResultFile(fMeasurePath, _T("Appearance.csv"), strResultColumn, pMeasureInfo);

		} break;
	case E_ALGORITHM_APP_PAD_SCRATCH:
		{
			if (stPadInfo == NULL)
			{
				lErrorCode = E_ERROR_CODE_PAD_PARAM_IS_NULL;
			}
			else if (stPadInfo[E_PAD_REF][nCameraNum_].GetRoiCount() <= 0)
			{
				lErrorCode = E_ERROR_CODE_PAD_TEACHING_ABNORMAL;
			}
			else
			{
				//Pad Reference ����
				Mat matRoiRefer = cvarrToMat(stPadInfo[E_PAD_REF][nCameraNum_].GetImage(0));

				//Work ��ǥ��
				Rect rtWorkReferROI = stPadInfo[E_PAD_REF][nCameraNum_].GetRectCoord(0);


				const int nROIArea = (int)dAlgPara[8];

				//���� ��ǥ��
				Rect rtReferROI;
				rtReferROI.x = ptAlignedEdgeCorner[E_CORNER_LEFT_TOP].x + rtWorkReferROI.x;
				rtReferROI.y = ptAlignedEdgeCorner[E_CORNER_LEFT_TOP].y + rtWorkReferROI.y;
				rtReferROI.width	=	 rtWorkReferROI.width;
				rtReferROI.height	=	 rtWorkReferROI.height;

				cv::Mat mtDust = LogicPara.MatDust;

				lErrorCode = APP_FindDefect_PadScartch(MatOriginImage_, matRoiRefer, mtDust, MatDrawBuffer, rtReferROI,  dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, strCurRecipePath, stPadInfo, fPath);
			}

			if (lErrorCode != E_ERROR_CODE_TRUE)
			{							
				theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, PAD Parameter Is ABNORMAL!!!"), theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_);			
			}
		} break;

	case E_ALGORITHM_APP_ACTIVE :
		{
			TCHAR strRoundContour[256];
			if (stThrdAlignInfo.ptActiveContour.size() == 0)
			{
				return E_ERROR_CODE_ACTIVE_MASK;
			}

			cv::Rect rtActiveInspArea = boundingRect(stThrdAlignInfo.ptActiveContour[0]);
			lErrorCode = APP_FindDefect_ActiveCorner(MatOriginImage_,  MatDrawBuffer, rtActiveInspArea, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, stThrdAlignInfo.ptActiveContour, strRoundContour);
		} break;
	case E_ALGORITHM_APP_ACTIVE_BURNT :
		{
			stActiveBurntROI * stROI = new stActiveBurntROI(3);
			for (int nROIInx = 0; nROIInx < stROI->nMaxROINum; nROIInx++)
			{
				CRect rtROI = theApp.GetRndROI(nImageNum, nCameraNum_, nROIInx, 1);
				stROI->rtROI[nROIInx] = cv::Rect(ptAlignedEdgeCorner[E_CORNER_LEFT_TOP].x + rtROI.left, ptAlignedEdgeCorner[E_CORNER_LEFT_TOP].y + rtROI.top, rtROI.Width(), rtROI.Height());
			}

			lErrorCode = APP_FindDefect_ActiveBurnt(MatOriginImage_,  MatDrawBuffer, stThrdAlignInfo.rcAlignCellROI, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, stROI, strCurRecipePath, fPath);
		} break;
	case E_ALGORITHM_APP_ACTIVE_DUST :
		{
			lErrorCode = APP_FindDefect_ActiveDust(MatOriginImage_,  MatDrawBuffer, stThrdAlignInfo.rcAlignActiveROI, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, fPath);
		} break;
	case E_ALGORITHM_APP_PAD_DUST :
		{
			if (stPadInfo == NULL)
			{
				lErrorCode = E_ERROR_CODE_PAD_PARAM_IS_NULL;
			}
			else if (stPadInfo[E_PAD_REF][nCameraNum_].GetRoiCount() <= 0)
			{
				lErrorCode = E_ERROR_CODE_PAD_TEACHING_ABNORMAL;
			}
			else
			{
				//Work ��ǥ��
				Rect rtWorkReferROI = stPadInfo[E_PAD_REF][nCameraNum_].GetRectCoord(0);

				//���� ��ǥ��
				Rect rtReferROI;
				rtReferROI.x = ptAlignedEdgeCorner[E_CORNER_RIGHT_TOP].x - rtWorkReferROI.width;
				rtReferROI.y = ptAlignedEdgeCorner[E_CORNER_RIGHT_TOP].y;
				rtReferROI.width	=	rtWorkReferROI.width;
				rtReferROI.height	=	ptAlignedEdgeCorner[E_CORNER_RIGHT_BOTTOM].y - ptAlignedEdgeCorner[E_CORNER_RIGHT_TOP].y; // 5.99 QHD �̻� Cell -> Dust Pattern������ Pad�� ��ü�κ� ���繰�� �˻�
				lErrorCode = APP_FindDefect_PadDust(MatOriginImage_, MatDrawBuffer, rtReferROI, dAlgPara, nCommonPara, fAlgPath, EngineerDefectJudgment, pResultBlob, fPath);
			}

			if (lErrorCode != E_ERROR_CODE_TRUE)
			{							
				theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, PAD Parameter Is ABNORMAL!!!"), theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_);			
			}
		} break;
	default:	
		break;
	}

	// ������ ���, ���� �ڵ� & �α� ���
	if (lErrorCode != E_ERROR_CODE_TRUE)
	{
		// Alg DLL Error Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Error (Code : %d). PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."), theApp.GetAlgorithmName(nAlgorithmNumber_), lErrorCode, strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

		return lErrorCode;
	}
	
	//////////////////////////////////////////////////////////////////////////

	// ��˻� ���� ���� ( ���� ���� �״�� ����ϹǷ�, Left-Top ���� ���ص� �� : NULL )
	m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);
	
	// �ҷ� ���� ( ��� ����, ��� �˰��򿡼� �ҷ� � �����ߴ��� �α� )
	int nTotalblob = pResultBlob->nDefectCount;	

	// KSW 17.09.07 - �ʿ��� ��� ���
	if( theApp.GetCommonParameter()->bIFImageSaveFlag )
	{
		// �˰��� �ҷ� ��� ���� ����
		// MatDrawBuffer ���� ���� ǥ���ؾ� �� �� ����.

		CString strResultROIFile = _T("");
		strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"), INSP_PATH, strPanelID_, nImageNum, nCameraNum_, nROINumber, theApp.GetAlgorithmName(nAlgorithmNumber_));

		// �ҷ� ���� ����
		BlobFeatureSave(pResultBlob, strResultROIFile);
	}

	// �ҷ� ���� ����
	pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pResultBlob, NULL);

	// Alg DLL End Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( DefectNum: %d )"), theApp.GetAlgorithmName(nAlgorithmNumber_), strPanelID_, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

	return true;
}

bool AppInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData)
{
	// �ҷ� ����Ʈ ���� ��� ������
	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
		return true;

	int nOffSet = 10;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// AD �ҷ��� �ƴ� ���
		if ( resultPanelData.m_ListDefectInfo[i].Defect_Type > E_DEFECT_JUDGEMENT_DUST_GROUP )
			 continue;

		// ù ���� AD �ҷ��� ���
		if( resultPanelData.m_ListDefectInfo[i].Img_Number == 0 )
		{
			// �ҷ� ù��° ����Ʈ�� ���� �����
			ResultDefectInfo tTempAD = resultPanelData.m_ListDefectInfo[i];
			resultPanelData.m_ListDefectInfo[0] = tTempAD;

			// ù��° ����Ʈ(AD �ҷ�) �����ϰ�, ��� ����.
			for (int j = 1; j < resultPanelData.m_ListDefectInfo.GetCount();)
				resultPanelData.m_ListDefectInfo.RemoveAt(j);

			// ������, ������
			return true;
		}
	}

	int Overlap_Offset = 5;

	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if(resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_APP_MEASURE || 
			resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_APP_CURL)
			continue;

		CRect rectTemp = CRect(
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	- nOffSet,
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	- nOffSet,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_X		+ nOffSet,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		+ nOffSet);

		for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
		{
			//Active �ߺ��ҷ��� ����
			//Active�� ���� ������ Skip
			if (i == j ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_APP_MEASURE || 
				resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_APP_CURL ||
				resultPanelData.m_ListDefectInfo[i].Defect_Type != resultPanelData.m_ListDefectInfo[j].Defect_Type)
			{
				j++;
				continue;
			}

			// �ҷ� �߽� ��ǥ
			CPoint ptSrc;
			ptSrc.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptSrc.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// �����ȿ� �ִٸ�
			if ( rectTemp.PtInRect(ptSrc) )
			{
				if (i > j)   i--;
				// �ش� �ҷ� ����
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// �ߺ� �ȵŸ�... ���� �ҷ�����...
			else j++;
		}

	}

	// Dust - Substance, Dust �̹������� ����� �ҷ� ���ܺκ�
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_APP_START)
			continue;

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_APP_ACTIVE_DUST)
		{
			CRect rectTemp = CRect(
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	- nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	- nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_End_X		+ nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		+ nOffSet);

			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ���� || �̹� �ҷ� Ȥ�� ��� ���� �ҷ����� ���� �ȉ��� ��
				if (i == j || resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE)
				{
					j++;
					continue;
				}

				// �ҷ� �߽� ��ǥ
				CPoint ptSrc;
				ptSrc.x = (LONG)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;
				ptSrc.y = (LONG)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y;

				// ���� �ȿ� �ҷ� ����?
				if ( rectTemp.PtInRect(ptSrc) )
				{
					if (i > j)   i--;

					// �ش� �ҷ� ����
					resultPanelData.m_ListDefectInfo.RemoveAt(j);
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;
			}
		}
	}

	//Dust ������ �����̹� �ҷ��ĺ��� ����
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_APP_ACTIVE_DUST)
		{
			// �ش� �ҷ� ����
			resultPanelData.m_ListDefectInfo.RemoveAt(i);
			i--;
		}
	}

	//Dust ���󿡼� ����� Cutting ���繰�� PAD Algorithm���� ����� Cutting ���繰 ���� ��ġ ���Ͽ� ó��
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (resultPanelData.m_ListDefectInfo[i].Defect_Type < E_DEFECT_JUDGEMENT_APP_START)
			continue;

		if (resultPanelData.m_ListDefectInfo[i].Defect_Type == E_DEFECT_JUDGEMENT_APP_PAD_DUST)
		{
			CRect rectTemp = CRect(
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	- nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	- nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_End_X		+ nOffSet,
				resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		+ nOffSet);

			CPoint	ptTemp = CPoint((LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_X, (LONG)resultPanelData.m_ListDefectInfo[i].Pixel_Center_Y);

			// ���� �ҷ�
			for (int j = 0; j < resultPanelData.m_ListDefectInfo.GetCount(); )
			{
				// ���� �ҷ� �׸� �� ���� || �̹� �ҷ� Ȥ�� ��� ���� �ҷ����� ���� �ȉ��� ��
				if (i == j || (resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_APP_PAD_BURNT &&
					resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_APP_PAD_SCRATCH && resultPanelData.m_ListDefectInfo[j].Defect_Type != E_DEFECT_JUDGEMENT_APP_PAD_DUST))
				{
					j++;
					continue;
				}

				CRect rectSrc = CRect(
					resultPanelData.m_ListDefectInfo[j].Pixel_Start_X	- nOffSet,
					resultPanelData.m_ListDefectInfo[j].Pixel_Start_Y	- nOffSet,
					resultPanelData.m_ListDefectInfo[j].Pixel_End_X		+ nOffSet,
					resultPanelData.m_ListDefectInfo[j].Pixel_End_Y		+ nOffSet);

				// �ҷ� �߽� ��ǥ
				CPoint ptSrc;
				ptSrc.x = (LONG)resultPanelData.m_ListDefectInfo[j].Pixel_Center_X;
				ptSrc.y = (LONG)resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y;

				// ���� �ȿ� �ҷ� ����?
				if ( rectTemp.PtInRect(ptSrc) )
				{
					if( rectTemp.Width() * rectTemp.Height() > rectSrc.Width() * rectSrc.Height())
					{
						// �ش� �ҷ� ����
						resultPanelData.m_ListDefectInfo.RemoveAt(j);
						if (i > j)   i--;
					}
					else
					{
						resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_APP_PAD_DUST;
						resultPanelData.m_ListDefectInfo.RemoveAt(i);
						i--;
						break;
					}			
				}
				else if(rectSrc.PtInRect(ptTemp))
				{
					resultPanelData.m_ListDefectInfo[j].Defect_Type = E_DEFECT_JUDGEMENT_APP_PAD_DUST;
					resultPanelData.m_ListDefectInfo.RemoveAt(i);
					i--;
					break;
				}
				// �ߺ� �ȵŸ�... ���� �ҷ�����...
				else j++;

			}
		}
	}

	return true;
}

// Casting - stDefectInfo���� �ʿ��� �κ� �����Ͽ� ResultPanelData�� ����
bool AppInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// AVI�� ȸ������ ���� ���� ���� ��ǥ�� ����ϹǷ� ���� ���� �� ��ũ��ǥ ��� ���� ���� ��ǥ�� ȸ���ؼ� �Ѱ���� ��
	_tcscpy_s(pResultDefectInfo->Defect_Code, _T("CODE"));
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_TOP]		= cv::Point(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_TOP]		= cv::Point(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_RIGHT_BOTTOM]	= cv::Point(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
	pResultDefectInfo->Defect_Rect_Point[E_CORNER_LEFT_BOTTOM]	= cv::Point(pResultBlob->ptLB[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].y);


	cv::Point ptRotate(0, 0);

	// AD �ҷ� �ƴ�...
	if (pResultBlob->nDefectJudge[nBlobCnt] > E_DEFECT_JUDGEMENT_DUST_GROUP)
	{
		pResultDefectInfo->Pixel_Start_X		= pResultBlob->ptLT[nBlobCnt].x;
		pResultDefectInfo->Pixel_Start_Y		= pResultBlob->ptLT[nBlobCnt].y;
		pResultDefectInfo->Pixel_End_X			= pResultBlob->ptRB[nBlobCnt].x;
		pResultDefectInfo->Pixel_End_Y			= pResultBlob->ptRB[nBlobCnt].y;

		// 2017.05.12 NDH : APP�� ���� ��� ��ǥ�� �Ѱ���ߵǴ��� Ȯ�� ���� �ʾ� ȸ�� ���� �ʴ´�. 

		//cv::Point ptLT(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLT[nBlobCnt].y);
		//Align_DoRotatePoint(ptLT, ptRotate, m_stThrdAlignInfo.ptAlignCenter, m_stThrdAlignInfo.dAlignTheta);
		//pResultDefectInfo->Pixel_Start_X		= ptRotate.x;
		//pResultDefectInfo->Pixel_Start_Y		= ptRotate.y;

		//cv::Point ptRB(pResultBlob->ptRB[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].y);
		//Align_DoRotatePoint(ptRB, ptRotate, m_stThrdAlignInfo.ptAlignCenter, -_stThrdAlignInfo.dAlignTheta);
		//pResultDefectInfo->Pixel_End_X			= ptRotate.x;
		//pResultDefectInfo->Pixel_End_Y			= ptRotate.y;
	}
	// AD �ҷ��� ��� ȸ������ �ʰ� ��ü ��ǥ �״�� ��ȯ
	else	
	{
		pResultDefectInfo->Pixel_Start_X		= pResultBlob->ptLT[nBlobCnt].x;
		pResultDefectInfo->Pixel_Start_Y		= pResultBlob->ptLT[nBlobCnt].y;
		pResultDefectInfo->Pixel_End_X		= pResultBlob->ptRB[nBlobCnt].x;
		pResultDefectInfo->Pixel_End_Y		= pResultBlob->ptRB[nBlobCnt].y;
	}

	pResultDefectInfo->Pixel_Center_X		= (int)(pResultDefectInfo->Pixel_Start_X + pResultDefectInfo->Pixel_End_X) / 2;
	pResultDefectInfo->Pixel_Center_Y		= (int)(pResultDefectInfo->Pixel_Start_Y + pResultDefectInfo->Pixel_End_Y) / 2;

	// ������ ���� �̹��� ���� Crop �� �ҷ� ���� ����
	pResultDefectInfo->Pixel_Crop_Start_X	= min(pResultBlob->ptLT[nBlobCnt].x, pResultBlob->ptLB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_Start_Y	= min(pResultBlob->ptLT[nBlobCnt].y, pResultBlob->ptRT[nBlobCnt].y);
	pResultDefectInfo->Pixel_Crop_End_X		= max(pResultBlob->ptRT[nBlobCnt].x, pResultBlob->ptRB[nBlobCnt].x);
	pResultDefectInfo->Pixel_Crop_End_Y		= max(pResultBlob->ptLB[nBlobCnt].y, pResultBlob->ptRB[nBlobCnt].y);
	
	// BOE Eng'r ��û Defect Size
	//double dCamResolution = WrtResultInfo.GetCamResolution(nCameraNum);
	//pResultDefectInfo->Defect_Size		= pResultBlob->nArea[nBlobCnt];
	//pResultDefectInfo->Defect_Size		= (int)(pResultBlob->nArea[nBlobCnt] * WrtResultInfo.GetCamResolution(nCameraNum));
	pResultDefectInfo->Defect_Size			= (int)WrtResultInfo.CalcDistancePixelToUm(sqrt(pow((double)(pResultDefectInfo->Pixel_End_X - pResultDefectInfo->Pixel_Start_X), 2) +
																						pow((double)(pResultDefectInfo->Pixel_End_Y - pResultDefectInfo->Pixel_Start_Y), 2)), 
																						nCameraNum, nRatio);
	pResultDefectInfo->Defect_MeanGV		= pResultBlob->dMeanGV[nBlobCnt];
	pResultDefectInfo->Defect_Size_Pixel	= (int)pResultBlob->nArea[nBlobCnt];
	pResultDefectInfo->Img_Number			= nImageNum;
	pResultDefectInfo->Img_Size_X			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_X - pResultDefectInfo->Pixel_Crop_Start_X);		// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Img_Size_Y			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_Y - pResultDefectInfo->Pixel_Crop_Start_Y);		// �ӽ� �ҷ� ���� ���� ũ��
	pResultDefectInfo->Defect_Type			= pResultBlob->nDefectJudge[nBlobCnt];
	pResultDefectInfo->Pattern_Type			= pResultBlob->nPatternClassify[nBlobCnt];
	pResultDefectInfo->Camera_No			= nCameraNum;
	pResultDefectInfo->nRatio				= nRatio;

#if USE_ALG_CONTOURS
	// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
#endif

	return true;
}

bool AppInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
{
	// ȸ���� ��� ������ ��ǥ ����
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP],		ptAdjCorner[E_CORNER_LEFT_TOP],		pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP],		ptAdjCorner[E_CORNER_RIGHT_TOP],	pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM],	ptAdjCorner[E_CORNER_RIGHT_BOTTOM],	pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);
	Align_DoRotatePoint(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM],		ptAdjCorner[E_CORNER_LEFT_BOTTOM],	pStCamAlignInfo->ptAlignCenter, pStCamAlignInfo->dAlignTheta);	

	//APP�� Align ȸ�� ��ȯ�� ��ǥ��� 2017.09.22 LJM
	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
	{
		pStCamAlignInfo->ptCorner[nCornerInx] = ptAdjCorner[nCornerInx];
	}

	return true;
}

bool AppInspection::AdjustOriginImage(cv::Mat& MatOrgImage, cv::Mat& MatDrawImage, tAlignInfo* pStAlignInfo)
{
	long nErrorCode = E_ERROR_CODE_TRUE;

	nErrorCode = Align_RotateImage(MatOrgImage, MatOrgImage, pStAlignInfo->dAlignTheta);

	nErrorCode = Align_RotateImage(MatDrawImage, MatDrawImage, pStAlignInfo->dAlignTheta);

	return true;
}

// Repair ���񿡼� ����ϴ� ��ǥ�� �� �ڵ� ����
bool AppInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
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

bool AppInspection::WriteMeasureResultFile(CString strFilePath, CString strFileName, CString strColumn, stMeasureInfo* pMeasureInfo)
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

	CString strPanelID = pMeasureInfo->strPanelID;
	CString strJudge   = pMeasureInfo->bJudge ? _T("OK") : _T("NG");
	
	const int nDataNum = pMeasureInfo->nMeasureValueSize;

	CString* strValue = new CString[nDataNum];
	strResultData.Format(_T("\n%s,%s,%s"), strTime, strPanelID, strJudge);

	for(int nDataInx =0; nDataInx < nDataNum; nDataInx++)
	{
		strValue[nDataInx].Format(_T(",%.3lf"), pMeasureInfo->dMeasureValue[nDataInx]);
		strResultData.Append(strValue[nDataInx]);
	}

	strColumn.Append(strResultData);

	if(nFileCheck == 0)			//������ ���� ���
	{
		stdFile.WriteString(strResultData);
	}
	else						//������ ���� ���
		stdFile.WriteString(strColumn);

	delete[] strValue;

	stdFile.Close();

	return true;
}

bool AppInspection::WriteResultFile(CString strPanelID, CString strFilePath, CString strFileName, CString strColumn, TCHAR* strResult)
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

	strColumn.Append(strResultData);

	if(nFileCheck == 0)			//������ ���� ���
	{
		stdFile.WriteString(strResultData);
	}
	else						//������ ���� ���
		stdFile.WriteString(strColumn);

	stdFile.Close();

	return true;
}

long AppInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
{

	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	long nErrorCode = E_ERROR_CODE_FALSE;

	nErrorCode = Align_FindDefectAD(MatOrgImage, dAlgPara, dResult, nRatio, nCameraNum, theApp.m_Config.GetEqpType());

	return nErrorCode;
}


long AppInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
{
	// ���� ó��
	bool bImageSave = theApp.GetCommonParameter()->bIFImageSaveFlag > 0 ? true : false;

	if( dAlgPara == NULL )	return E_ERROR_CODE_EMPTY_PARA;

	long nErrorCode = E_ERROR_CODE_TRUE;

	bool	bUse				= dAlgPara[E_PARA_APP_MAKE_ACTIVE_MASK] > 0 ? true : false;		//Polygon Mask�� �������?
	int		nUseImageNum		= (int)dAlgPara[E_PARA_APP_POLYGON_IMAGE_NUM];						//Polygon Mask�� ����� ����
	
	CStringA strImageName = (CStringA)theApp.GetGrabStepName(nImageNum);

	wchar_t wstrID[MAX_PATH] = {0, };
	swprintf( wstrID, _T("%s"), LogicPara.tszPanelID );

// 	Point ptCorner[E_CORNER_END];
// 	for(int nCornerInx = E_CORNER_LEFT_TOP; nCornerInx < E_CORNER_END; nCornerInx++)
// 	{
// 		ptCorner[nCornerInx] = stThrdAlignInfo.ptCorner[nCornerInx];
// 		Align_DoRotatePoint(ptCorner[nCornerInx], ptCorner[nCornerInx], stThrdAlignInfo.ptAlignCenter, stThrdAlignInfo.dAlignTheta);
// 	}

	//Polygon Mask�� ����ٸ�
	if(bUse)
	{
		if(nUseImageNum != nAlgImg)
			return E_ERROR_CODE_ALIGN_ROUND_SETTING;		
		
		nErrorCode = Align_SetFindContour(LogicPara.MatOrgImage, theApp.GetRoundROI(nImageNum, nCameraNum), theApp.GetRndROICnt(nImageNum, nCameraNum), dAlgPara, 
			nAlgImg, nCameraNum, nRatio, theApp.m_Config.GetEqpType(), theApp.GetRoundPath(), stThrdAlignInfo.ptCorner, strImageName, stThrdAlignInfo.dAlignTheta, theApp.GetCommonParameter()->bIFImageSaveFlag);

		if(nErrorCode == E_ERROR_CODE_TRUE)
			nErrorCode = E_ERROR_CODE_ALIGN_ROUND_SETTING;
	}
	else
	{
		if(nUseImageNum != nAlgImg)
			return E_ERROR_CODE_TRUE;

		theApp.WriteLog(eLOGCAM0, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI FillOut Start.  CAM: %02d, Img: %s."),
			 nCameraNum, theApp.GetGrabStepName(nImageNum));

		// Alg �α׿�
		wchar_t strAlgLog[MAX_PATH] = {0, };
		swprintf( strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum) );

		nErrorCode = Align_FindFillOutArea(LogicPara.MatOrgImage, LogicPara.MatBKG, theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nAlgImg], stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg], stThrdAlignInfo.tCHoleAlignData->bCHoleAD[nAlgImg],
			dAlgPara, nAlgImg, nCameraNum, nRatio, EQP_APP, strAlgLog, wstrID, stThrdAlignInfo.ptCorner, stThrdAlignInfo.ptActiveContour, stThrdAlignInfo.dAlignTheta, theApp.GetRoundPath(), bImageSave);

		if(stThrdAlignInfo.ptActiveContour.size() < 1)
			return E_ERROR_CODE_FALSE;

		stThrdAlignInfo.rcActive = cv::boundingRect(stThrdAlignInfo.ptActiveContour[0]);

		theApp.WriteLog(eLOGCAM0, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI FillOut End.  CAM: %02d, Img: %s."),
			nCameraNum, theApp.GetGrabStepName(nImageNum));
	}
	return nErrorCode;
}


long AppInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
	bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)
{
	long nErrorCode = E_ERROR_CODE_TRUE;

	if(MatOrgImage.empty())
	{
		nErrorCode = E_ERROR_CODE_EMPTY_BUFFER;
		theApp.WriteLog(eLOGCAM0, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("CAM: %02d, Img: %s Don't be Image Buffer"),
			nCameraNum, theApp.GetGrabStepName(nImageNum));
	}

	if(nErrorCode == E_ERROR_CODE_TRUE)
	{
		double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

		float fMean = (float)dAlgPara[E_PARA_APP_ADGV_MEAN_GV];

		Scalar scMean, scStdev;
		cv::meanStdDev(MatOrgImage, scMean, scStdev);
		if(scMean[0] < fMean)
		{
			nErrorCode = E_ERROR_CODE_FALSE;
			theApp.WriteLog(eLOGCAM0, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("CAM: %02d, Img: %s Abnormal GV Value"),
				nCameraNum, theApp.GetGrabStepName(nImageNum));
		}
	}


	

	return nErrorCode;
}

// Mura �ܰ��� ���� ����
bool AppInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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
		//int nLTX = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x;
		//int nLTY = m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y;

		// �ҷ� ���� ��ŭ
		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++ )
		{
			// �ҷ� ����
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// �ܰ��� ����
			for (int j=0 ; j<MAX_CONTOURS ; j++)
			{
				// P/S ��忡 ���� ��ǥ ���� ( 1�� ��ǥ�� ���� )
				// CFeatureExtraction::calcContours() ���� �̹� 1�� ��ǥ�� ����Ǽ� ����
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j]/* + nLTX*/;
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j]/* + nLTY*/;

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

// KSW 17.11.29 - �ܰ� ���� ( AVI & SVI ���� �� )
bool AppInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
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

		CRect rtActiveROI = CRect(CPoint(m_stThrdAlignInfo.rcActive.tl().x, m_stThrdAlignInfo.rcActive.tl().y), 
								  CPoint(m_stThrdAlignInfo.rcActive.br().x, m_stThrdAlignInfo.rcActive.br().y));
		

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

			// APP ��� ǥ��
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


bool AppInspection::JudgementRepeatCount(CString strPanelID, ResultPanelData& resultPanelData, int nStageNo)
{
	
	// ������ �ߺ� ���� ����
	// ���� ��� ������ ������� �ݺ� Ƚ�� üũ - �г� ���� ���� ��� ����

	EnterCriticalSection(&theApp.m_csJudgeRepeatCount);

	ListCurDefect* pStCurDefList = new ListCurDefect();
// 	std::list<RepeatDefectInfo>* pList = theApp.GetRepeatDefectInfo();					// ���� �ҷ� ����Ʈ(Pixel ��ǥ)
// 	static bool bIsAlarm = false;														// ���� �˶� �߻� ����

	// 2018.10.01 MDJ File Read ADD
	std::list<RepeatDefectInfo>* pList;
	static bool bIsAlarm = false;														// ���� �˶� �߻� ����

	theApp.ReadRepeatDefectInfo();
	pList = theApp.GetRepeatDefectInfo();

	static int nCntCell = 0;															// 2018.10.09 MDJ Test�� ���� ����

	CCPUTimer ttRepeatCount;
	ttRepeatCount.Start();

	memcpy(pStCurDefList->bUseChkRptDefect, theApp.m_Config.GetUseCCDAlarm(), sizeof(pStCurDefList->bUseChkRptDefect));

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (0) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// 2018.10.02 MDJ Machine ��ǥ üũ ������� ���� ��� ���� List Clear�ϰ� ���� Return
	if (!pStCurDefList->bUseChkRptDefect[eMACHINE])
	{
		
		if (pList[eMACHINE].size() != 0)
			pList[eMACHINE].clear();

		m_fnSaveRepeatDefInfo(pList);
		
		bIsAlarm = false;
		LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Skip Repeat Defect !!! - Not Used"));
		return true;
	}

	const int DIGIT_PNLID = 17;		// Panel ID �ڸ��� ����
	CString strOrgPanelID = strPanelID;
	static CString strOldPanelID = _T("");

	// [PANELID]_1, [PANELID]_2 ... �� Panel ID �� ���� �ڸ������� Ŭ ��� Panel ID ��ŭ�� �߶� ��
	if (strOrgPanelID.GetLength() > DIGIT_PNLID)
		strOrgPanelID = strPanelID.Left(DIGIT_PNLID);

	// �˶� �߻� ���� ���¿��� ������ Panel ���� ���� �� üũ ����
	// �̹� �˶��� �߻��� ���¸� ������ Panel ID �������ϴ��� ������ �Ǵ� ���� �ذ�Ǳ� ������ ���������� �˶� �߻�
	if (strOrgPanelID.Compare(strOldPanelID) == 0 && !bIsAlarm)
	{
		LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);
		theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Skip Repeat Defect !!! - Overlap Panel ID (%s)"), strOrgPanelID);
		return true;
	}
	else
	{
		strOldPanelID = strOrgPanelID;
	}
	
	bool bRet = false;
	// ���� �ݺ� �ҷ� üũ �ִ� ���� 1000���� ����
	// �ִ� ���� ���� �ʿ��� ��� �ӵ� ���� ���
	const int MAX_REPEAT_DEFECT = 1000;
	// [0] : CCD / [1] : ����(Work��ǥ) �ҷ�
	// 2018.09.26 MDJ APP RepeatDefect
	// [0] : CCD / [1] : ����(Work��ǥ) �ҷ� / [2] : ���(Machine)
	int* nRptOffset = theApp.m_Config.GetCCDOffset();
	int* nRptLArmCnt = theApp.m_Config.GetCCDLightAlarmCount();
	int* nRptHArmCnt = theApp.m_Config.GetCCDHeavyAlarmCount();

	int nTotalCount = 0;
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// ���⿡ üũ �ʿ��� �׸� �߰� (Pixel / Work)
		switch (resultPanelData.m_ListDefectInfo[i].Defect_Type)
		{
		// 2018.09.26 MDJ APP Repeat Defect Defect���� ����
		case E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLE:
		case E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE:
		case E_DEFECT_JUDGEMENT_APP_ACTIVE_BUBBLESUBSTANCE:
			if(pStCurDefList->bUseChkRptDefect[eMACHINE])
				pStCurDefList->Add_Tail(eMACHINE, E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE, &resultPanelData.m_ListDefectInfo[i], nStageNo);	// 18.09.21 MDJ StageNo ADD
			nTotalCount++;
			break;

		default:
			break;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// ���� �ҷ� �ʹ� ���� ��� Skip -
		if (nTotalCount > MAX_REPEAT_DEFECT)
		{
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("Skip Repeat Defect !!! - Too Many Defect (Repeat Defect Count : Over %d)"), MAX_REPEAT_DEFECT);
			LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);
			return false;
		}
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (1) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	// ���� �ҷ� ����Ʈ�� ���� �ҷ� ����Ʈ ��
	// ��ġ�� �ʴ� �ҷ� ����
	// ��ġ�� �ҷ� ��հ� ��� �� ī��Ʈ ����	
	int nMaxRepeatCount[eCOORD_KIND] = { 0, };


	std::list<RepeatDefectInfo>::iterator iterDst, iterSrc;
	bool bRepeat = false;
	bool bSameStage = false;

//#pragma omp parallel for schedule(dynamic)		// ����ó�� �߰�

	// ���� �ҷ� ����Ʈ
	for (iterDst = pList[eMACHINE].begin(); iterDst != pList[eMACHINE].end(); )
	{
		bRepeat = false;
		bSameStage = false;

		// ���� �ҷ� ��ġ Area ����
		// CCD �ҷ� - ���� Pixel / ���� �ҷ� - ���� um
		CRect rcDst(iterDst->ptCenterPos.x - nRptOffset[eMACHINE] - 1, iterDst->ptCenterPos.y - nRptOffset[eMACHINE] - 1,			// LT
			iterDst->ptCenterPos.x + nRptOffset[eMACHINE] + 1, iterDst->ptCenterPos.y + nRptOffset[eMACHINE] + 1);					// RB

																																// ���� �ҷ� ����Ʈ
		for (iterSrc = pStCurDefList->listCurDefInfo[eMACHINE].begin(); iterSrc != pStCurDefList->listCurDefInfo[eMACHINE].end(); )
		{
			if (iterDst->nStageNo == iterSrc->nStageNo)
			{
				bSameStage = true;

				if (iterDst->eDefType == iterSrc->eDefType)
				{
					// �ݺ��� ��ġ�� ���ο� �ҷ��� ���� ���
					if (PtInRect(rcDst, iterSrc->ptCenterPos))
					{
						bRepeat = true;

						// �߽���ǥ ��� ���
						iterDst->ptCenterPos += iterSrc->ptCenterPos;
						iterDst->ptCenterPos.x /= 2;
						iterDst->ptCenterPos.y /= 2;
						iterDst->nRepeatCount++;

						if (iterDst->nRepeatCount >= nRptLArmCnt[eMACHINE])
						{
							nMaxRepeatCount[eMACHINE] = MAX(nMaxRepeatCount[eMACHINE], iterDst->nRepeatCount);				// �ִ� �ݺ� �߻� Ƚ��

							// Log �� ���ڿ�
							CString strErrMsg = _T("");
							CString strErrType = _T("");
							CString strDefType = _T("");

							if (iterDst->nRepeatCount >= nRptHArmCnt[eMACHINE])			strErrMsg = _T("Error");
							else														strErrMsg = _T("Warning");

							strErrType = _T("Machine");

							switch (iterDst->eDefType)
							{
							case E_DEFECT_JUDGEMENT_APP_ACTIVE_SUBSTANCE:
								strDefType = _T("ACTIVE_SUBSTANCE");
								break;
							default:
								strDefType.Format(_T("%d"), (int)iterDst->eDefType);
								break;
							}
							theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("%s !! APP Repeat Defect(%s) %d Times !!! (DefCode <%s> : %d, %d)"),
								strErrMsg,
								strErrType,
								iterDst->nRepeatCount,
								strDefType,
								iterDst->ptCenterPos.x, iterDst->ptCenterPos.y);
						}

						// 2018.10.09 MDJ Test�� ���� ����
						theApp.WriteLog(eLOGPROC, eLOGLEVEL_DETAIL, TRUE, TRUE, _T("APP Repeat Defect(Stage No : %d) ------- BeforeDef : %d, %d / AfterDef : %d, %d / Cnt : %d)"),
							nStageNo,
							iterDst->ptCenterPos.x,
							iterDst->ptCenterPos.y,
							iterSrc->ptCenterPos.x,
							iterSrc->ptCenterPos.y,
							iterDst->nRepeatCount);

						iterSrc = pStCurDefList->listCurDefInfo[eMACHINE].erase(iterSrc);

						break;
					}
					else
					{
						iterSrc++;
					}
				}
				else
				{
					iterSrc++;
				}
			}
			else
			{
				bSameStage = false;
				iterSrc++;
			}
		}

		// �ϳ��� ��ġ�°� ������ ���� ����Ʈ�� �ҷ� ����
		if (!bRepeat && bSameStage)
			iterDst = pList[eMACHINE].erase(iterDst);
		else
			iterDst++;
	}

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (2) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	if (pStCurDefList->bUseChkRptDefect[eMACHINE])
	{
		if (nMaxRepeatCount[eMACHINE] >= nRptHArmCnt[eMACHINE])
		{
			bIsAlarm = true;
			// nKind 0 : CCD �߾˶� (3000) / nKind 1 : �����ҷ� �߾˶� (3001)
			// 2018.09.25 MDJ	nKind 0 : CCD �߾˶� (3000) / nKind 1 : �����ҷ� �߾˶� (3001) / nKind 2 : ���ҷ� �߾˶� (3002)
			theApp.m_AlgorithmTask->VS_Send_Alarm_Occurred_To_MainPC(eAutoRun, eALARMID_CCD_DEFECT_ERROR + eMACHINE, eALARMTYPE_HEAVY);
		}
		else
		{
			bIsAlarm = false;
		}

		// ��ġ�� �׸� ���� ���� ����Ʈ�� ���� �߻��� �ҷ� �߰�
		// merge�ϱ� ���� �����ϴ� ���� �߰�
		pList[eMACHINE].sort();
		pStCurDefList->listCurDefInfo[eMACHINE].sort();
		pList[eMACHINE].merge(pStCurDefList->listCurDefInfo[eMACHINE]);
	}

	nCntCell++;															// 2018.10.09 MDJ Test�� ���� ����

	// ���ο� ����Ʈ ���Ϸ� ����
	bRet = m_fnSaveRepeatDefInfo(pList);
	
	SAFE_DELETE(pStCurDefList);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Judge Repeat (3) %.2f"), ttRepeatCount.Stop(false) / 1000.);

	LeaveCriticalSection(&theApp.m_csJudgeRepeatCount);

	return bRet;
}

//bool AppInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)
bool AppInspection::m_fnSaveRepeatDefInfo(std::list<RepeatDefectInfo>* pListRepeatInfo)	// 2018.10.09 MDJ Test�� ���� ����
{
	// List ���� ���
	if (pListRepeatInfo == NULL)			return false;

	// TXT ����
	std::list<RepeatDefectInfo>::iterator iterList;
	CStdioFile	fileWriter;
	CString		strRepeatInfoHeader, strRepeatInfo;
	BOOL bRet = FALSE;

	// 2018.09.26 MDJ APP Repeat Defect ��� ���ϴ� �˶� Skip
	BOOL* bRptUse = theApp.m_Config.GetUseCCDAlarm();

	bRet = fileWriter.Open(REPEAT_DEFECT_MACHINE_INFO_PATH, CFile::modeCreate | CFile::modeWrite);

	if (bRet)
	{
		// ��� ���� - for GUI
		// 2018.09.25 MDJ APP Repeat Defect
		strRepeatInfoHeader = _T("Type,DefectX,DefectY,RepeatCount,StageNo\r\n");;

		fileWriter.WriteString(strRepeatInfoHeader);

		// �ߺ� ��ǥ ���� ��ŭ
		for (iterList = pListRepeatInfo[eMACHINE].begin(); iterList != pListRepeatInfo[eMACHINE].end(); ++iterList)
		{
			strRepeatInfo.Format(_T("%d,%d,%d,%d,%d\r\n"), (int)iterList->eDefType,
				iterList->ptCenterPos.x, iterList->ptCenterPos.y, iterList->nRepeatCount, iterList->nStageNo);
				
			fileWriter.SeekToEnd();
			fileWriter.WriteString(strRepeatInfo);

		}

		// ���� ���� ��츸 �ݱ�
		fileWriter.Close();
	}

	return true;
}