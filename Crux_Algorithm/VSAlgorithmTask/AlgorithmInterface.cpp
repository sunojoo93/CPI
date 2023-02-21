#include "stdafx.h"
#include "AlgorithmInterface.h"
#include "VSAlgorithmTask.h"

// ���� ó��
#include <omp.h>
//#include <ppl.h>
//using namespace Concurrency;

// Align_RotateImage() ����ϱ� ����
#include "DllInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using namespace Concurrency;

InspectAlgorithmInterface::InspectAlgorithmInterface()
{
}

InspectAlgorithmInterface::~InspectAlgorithmInterface(void)
{
}

void InspectAlgorithmInterface::DrawAdjustROI(cv::Mat& MatDrawBuffer, cv::Point* pPtCorner, LPCTSTR strROIName, int nCurCount, int nDrawMode)
{
	CString strNumber;

	if(!MatDrawBuffer.empty())
	{
		cv::Scalar color;
		int nThickness = 1;	// Rectangle �β�

		// Teaching Draw Rect
		if (nDrawMode == eBasicROI)
		{	color.val[0] = 0;	color.val[1] = 255;	 color.val[2] = 0;	}	// ���� ROI
		else if (nDrawMode == eRndROI)
		{	color.val[0] = 255;	color.val[1] = 0;	color.val[2] = 0;	}	// Pol ROI
		else
		{	color.val[0] = 0;	color.val[1] = 255;	color.val[2] = 255;	}	// ��˻� ���� ROI

		// ���簢�� -> �簢�� ����
		//cv::rectangle(MatDrawBuffer, cv::Rect(rectROI.left, rectROI.top, rectROI.right - rectROI.left, rectROI.bottom - rectROI.top), color, nThickness);
		cv::line(MatDrawBuffer, pPtCorner[E_CORNER_LEFT_TOP],		pPtCorner[E_CORNER_RIGHT_TOP],		color, nThickness);
		cv::line(MatDrawBuffer, pPtCorner[E_CORNER_RIGHT_TOP],		pPtCorner[E_CORNER_RIGHT_BOTTOM],	color, nThickness);
		cv::line(MatDrawBuffer, pPtCorner[E_CORNER_RIGHT_BOTTOM],	pPtCorner[E_CORNER_LEFT_BOTTOM],	color, nThickness);
		cv::line(MatDrawBuffer, pPtCorner[E_CORNER_LEFT_BOTTOM],	pPtCorner[E_CORNER_LEFT_TOP],		color, nThickness);

		// Text
		strNumber.Format( _T("%d,%s,(%d,%d),(%d,%d),(%d,%d),(%d,%d)"),  nCurCount+1, strROIName, 
			pPtCorner[E_CORNER_LEFT_TOP].x,		pPtCorner[E_CORNER_LEFT_TOP].y, 
			pPtCorner[E_CORNER_RIGHT_TOP].x,	pPtCorner[E_CORNER_RIGHT_TOP].y,
			pPtCorner[E_CORNER_RIGHT_BOTTOM].x, pPtCorner[E_CORNER_RIGHT_BOTTOM].y,
			pPtCorner[E_CORNER_LEFT_BOTTOM].x,	pPtCorner[E_CORNER_LEFT_BOTTOM].y);
		char* pTemp = CSTR2PCH(strNumber);
		cv::putText(MatDrawBuffer, pTemp, cv::Point(pPtCorner[E_CORNER_LEFT_TOP].x - 5, pPtCorner[E_CORNER_LEFT_TOP].y - 5), FONT_HERSHEY_SIMPLEX, 1., color, 1);
		SAFE_DELETE_ARR(pTemp);
	}
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	LogicStart
//	 2. �Լ���� :	�ϳ��� �� �˻�
//	 3. ��    �� :	MatOriginImage		: ���� ����
//					MatDrawBuffer		: ��� Draw �̹���
//					MatResultImg		: �˰��� ��� �̹���
//					resultBlob_Total	: Pattern �� ��� �˰��� ��� ���� Buffer
//	 4. �� ȯ �� :	�Ϸ� : true
//					���� : false
//	 5. �� �� �� :	2015/02/07
//	 6. �� �� �� :	KSW
//	 7. ������� :	2017/03/02 by CWH
//	 8. ��    �� :	
//----------------------------------------------------------------------------------------------------
long InspectAlgorithmInterface::StartLogic(CString strPanelID, CString strDrive, CString strLcp_Bubble_Path, tAlignInfo stThrdAlignInfo,
									cv::Mat MatOriginImage[][MAX_CAMERA_COUNT], cv::Mat& MatColorBuffer, cv::Mat& MatDrawBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
									ResultBlob_Total* pResultBlob_Total, const int nImageNum, const int nCameraNum, const int nThreadIndex, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	//////////////////////////////////////////////////////////////////////////
	// �Ķ���� ����
	//////////////////////////////////////////////////////////////////////////
	tLogicPara LogicPara;

	LogicPara.MatOrgImage		= MatOriginImage[nImageNum][nCameraNum];
	LogicPara.nCameraNum		= nCameraNum;
	LogicPara.nThreadLog		= nThreadIndex;

	const int nEqpType = theApp.m_Config.GetEqpType();
	int TimeCount = 0;
	
	switch(nEqpType)
	{
		//////////////////////////////////////////////////////////////////////////
		// RGB  ������ �̹��� ���� ������ 3���� �ӽ÷� �����Ѵ�.
		// makePolygonCellROI �ϸ� �̹����� ���������� ������ ������ ����.
		//////////////////////////////////////////////////////////////////////////
	case EQP_AVI:
	case EQP_LCP:

//		switch (theApp.GetImageClassify(nImageNum))
//		{
//		case E_IMAGE_CLASSIFY_AVI_DUST:
//			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
//			break;
//		case E_IMAGE_CLASSIFY_AVI_SIDE:
//			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
//			break;
// 		case E_IMAGE_CLASSIFY_AVI_B:
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_B)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
// 			break;
//		}


// 		LogicPara.MatOrgRGBAdd[2] = &MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum];

		// RGB �� �Կ� �ɶ����� ���
// 		TimeCount = 0;
// 		while(	MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum].empty()	||
// 				MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum].empty()	||
// 				MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum].empty()	)
// 		{
// 			TimeCount++;
// 			Sleep(10);
// 
// 			if (TimeCount >= 1000)
// 				return E_ERROR_CODE_FALSE;
// 		}
// 
// 		switch(theApp.GetImageClassify(nImageNum))
// 		{
// 		case E_IMAGE_CLASSIFY_AVI_DUST: 
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
// 			break;
// 		case E_IMAGE_CLASSIFY_AVI_SIDE: 
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
// 			break;
//  		case E_IMAGE_CLASSIFY_AVI_COAXIAL:
//  			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum]);		//�޸� �Ҵ� ���� ��ü
//  			break;
// 		}
// 
// // 		// RGB �����Ҷ�����
// // 		TimeCount = 0;
// 		while(	MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum].empty()	||
// 				MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum].empty()	||
// 				MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum].empty()	)
// 		{
// 			TimeCount++;
// 			Sleep(10);
// 
// 			// KSW 18.05.30
// 			if (TimeCount >= 1000)
// 				return E_ERROR_CODE_EMPTY_RGB;
// 		}
// 
// // 		//RGB ������ �˰��� �Ѱ��� �غ� �Ѵ�. - �ε����� �������� ���� ����Ѵ�. => ���߿� Ʋ������ �����ʿ�
// // 		LogicPara.MatOrgRGB[0]		= MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum];
// // 		LogicPara.MatOrgRGB[1]		= MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum];
// // 		LogicPara.MatOrgRGB[2]		= MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum];
// 
// 		LogicPara.MatOrgRGBAdd[0] = &MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum];
// 		LogicPara.MatOrgRGBAdd[1] = &MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum];
//  		LogicPara.MatOrgRGBAdd[2] = &MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum];
		break;

	case EQP_SVI:
		break;

	case EQP_APP:
		TimeCount = 0;
		while(	MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_APP_DUST)][nCameraNum].empty()	)
		{
			TimeCount++;
			Sleep(10);

			if (TimeCount >= 1000)
				return E_ERROR_CODE_FALSE;
		}
		LogicPara.MatDust		= MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_APP_DUST)][nCameraNum];
		break;

	default:
		break;
	}

	// �ǳ� ID
	int nLength = 0;
	nLength = _stprintf_s(LogicPara.tszPanelID, MAX_PATH, _T("%s"), (LPCWSTR)strPanelID);
	LogicPara.tszPanelID[nLength] = _T('\0');
	LogicPara.strPanelID.Format(_T("%s"), strPanelID);

	//////////////////////////////////////////////////////////////////////////
	// ���� �� ROI ����
	//////////////////////////////////////////////////////////////////////////

	// ���� �ܰ迡�� Check
	// 	// ���۰� ���� ���. / �˻� �Ұ� ���� �ڵ� & �α� ���
	// 	if( MatOriginImage[nImageNum][nCameraNum].empty() )
	// 	{
	// 		// Error Log
	// 		return E_ERROR_CODE_EMPTY_BUFFER;
	// 	}	

	// Teaching���� ���� ROI ������ ���� ��������
	//int nROICnt = theApp.GetROICnt(nImageNum, nCameraNum);

	// ���� Image �� ROI �ϳ��� ����
	int nROICnt = 1;

	// ������ ROI ������ ���� ��� �˻� ����.
	// ������ ���, ���� �ڵ� & �α� ���
	if( nROICnt <= 0 )
	{
		return E_ERROR_CODE_EMPTY_SET_ROI;
	}


	//////////////////////////////////////////////////////////////////////////
	// Cell Edge �κ� ó���� ( APP ��� ���� )
	//////////////////////////////////////////////////////////////////////////
	double* dAlgPara;
	ENUM_KIND_OF_LOG eLogCamera;
	long nRoundErrorCode = E_ERROR_CODE_TRUE;

	switch(nEqpType)
	{
	case EQP_AVI:
		{
			//LCP Chole �̻��
 		}		
		break;

	case EQP_SVI:
		{
			// SVI �޸� �Ҵ� ( 8bit )
			LogicPara.MatBKG = cv::Mat::zeros( LogicPara.MatOrgImage.size(), CV_8UC1 );

			// Align �˰��� �˻� �Ķ���� ��������
			dAlgPara = theApp.GetAlignParameter(nCameraNum);

			// �ܰ� �κ� ó��
			nRoundErrorCode = makePolygonCellROI( LogicPara, stThrdAlignInfo, nImageNum, nCameraNum, dAlgPara, theApp.GetImageClassify(nImageNum), nRatio );
		}		
		break;

	case EQP_APP:
		{
 			// Align �˰��� �˻� �Ķ���� ��������
 			dAlgPara = theApp.GetAlignParameter(nCameraNum);

			if (dAlgPara[E_PARA_APP_MAKE_ACTIVE_MASK])
			{
				nRoundErrorCode = E_ERROR_CODE_ALIGN_ROUND_SETTING;
			}
			else
			{
				nRoundErrorCode = E_ERROR_CODE_TRUE;
			}
		} break;
	}

	// Alg DLL Success Log
	eLogCamera = (ENUM_KIND_OF_LOG)nCameraNum;

	// Round ���� �÷��� On�� ��� �˻� ����
	if (nRoundErrorCode == E_ERROR_CODE_ALIGN_ROUND_SETTING)
	{
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Setting Success. Algorithm Skip. PanelID: %s, CAM: %02d, Img: %s."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum));

		// �˻� ���� �̽���
		return E_ERROR_CODE_TRUE;
	}

	// �����ڵ尡 �߻��� ��� Log
	else if (nRoundErrorCode != E_ERROR_CODE_TRUE)
	{
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Fail. PanelID: %s, CAM: %02d, Img: %s Error Code:%d."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum), nRoundErrorCode);

		// �˻� ���� �̽���
		return E_ERROR_CODE_TRUE;
	}

	// �������� ������ ���
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Success. PanelID: %s, CAM: %02d, Img: %s."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum));



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// �˰��� �Լ� ����
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Teaching���� ������ Algorithm List ���� ��������
	int nAlgCnt = MAX_MEM_SIZE_E_ALGORITHM_NUMBER;

	//////////////////////////////////////////////////////////////////////////
	//�̹��˻� �ٲ��������� ok
	//�Ѱ��� ������ ������ ����
	if (!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) &&
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) &&
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) &&
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE)) {

		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("GetUseAlgorithm NON. PanelID: %s, CAM: %02d, Img: %s Error Code:%d."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum), nRoundErrorCode);

		return E_ERROR_CODE_TRUE;
	}
	else if (!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) ||
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) ||
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE) ||
		!theApp.GetUseAlgorithm(theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_BACK), LogicPara.nCameraNum, 0, E_ALGORITHM_LCP_SUBSTANCE)) {

		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("GetUseAlgorithm Fail. PanelID: %s, CAM: %02d, Img: %s Error Code:%d."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum), nRoundErrorCode);

		return E_ERROR_CODE_NON_USE;
	}
	//////////////////////////////////////////////////////////////////////////

	// Teaching ���� ������ ROI ������ŭ ( AVI 1����.... )
// #ifndef _DEBUG
// #pragma omp parallel for
// #endif
	for (int nROINum = 0; nROINum < nROICnt; nROINum++)
	{
		// ROI �̸� ���� ��� ����
		// �˻� ROI �׸���
		DrawAdjustROI(MatDrawBuffer, stThrdAlignInfo.ptCorner, /*theApp.GetROIName(nImageNum, nCameraNum, nROINum)*/_T("ROI"), nROINum, eBasicROI);

		// Teaching ���� ������ Algorithm List ������ŭ.
		
#ifndef _DEBUG
			
			// ini���� Data �б� �ӽ� �߰� 190218 YWS
			int nNested_offset = GetPrivateProfileInt(_T("InspInfo"), _T("Nested offset"), 1, VS_ALGORITHM_TASK_INI_FILE);
			int nThread_Count = GetPrivateProfileInt(_T("InspInfo"), _T("Thread Count"), 1, VS_ALGORITHM_TASK_INI_FILE);

		omp_set_nested(nNested_offset); // ���� 1
#pragma omp parallel for schedule(dynamic) num_threads(nThread_Count) // ���� 4

#endif
		for (int nAlgNum=0; nAlgNum < nAlgCnt; nAlgNum++)
		{
			// Teaching���� Algorithm List ���� Ȯ��.
			if (theApp.GetUseAlgorithm(nImageNum, LogicPara.nCameraNum, nROINum, nAlgNum))
			{
				// �˰��� ����
				long nErrorCode = StartLogicAlgorithm(strDrive, strLcp_Bubble_Path ,LogicPara, MatColorBuffer, MatResultImg, MatDrawBuffer, nImageNum, nROINum, nAlgNum, stThrdAlignInfo, pResultBlob_Total, bpInspectEnd, bpInspectAlgEnd, nRatio, eInspMode, WrtResultInfo, _mtp );
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

//----------------------------------------------------------------------------------------------------
//	 1. �� �� �� :	BlobFeatureSave
//	 2. �Լ���� :	Defect ��� ���� ����
//	 3. ��    �� :	resultBlob		: Blob ����
//					strPath			: ���� ���
//					nThreadID		: ������ ID
//	 4. �� ȯ �� :	
//	 5. �� �� �� :	2015/03/27
//	 6. �� �� �� :	KSW
//	 7. ������� :
//	 8. ��    �� :
//----------------------------------------------------------------------------------------------------
void InspectAlgorithmInterface::BlobFeatureSave(stDefectInfo *resultBlob, CString strPath, int* nImageDefectCount)
{
	if( resultBlob == NULL )	return;

 	int	nDefectNum		= 0;
 	
 	// ��� Log ����
 	FILE *out = NULL;
 
 	if (nImageDefectCount == NULL)
 		nImageDefectCount = &nDefectNum;
 
 	if (*nImageDefectCount == 0)
 		_wfopen_s(&out, strPath, _T("wt"));
 	else
 		_wfopen_s(&out, strPath, _T("at"));
 
 	// ���� ó��.
 	if(out == NULL)		return;
 
	// �������� ROI ��� ���� �� ���ʿ��� Header ����
 	if (*nImageDefectCount == 0)
 	{
 		fprintf_s(out, "No					,\
 						Defect_Judge		,\
 						Defect_Color		,\
 						Area				,\
						ptLT_X				,\
						ptLT_Y				,\
						ptRT_X				,\
 						ptRT_Y				,\
						ptRB_X				,\
						ptRB_Y				,\
						ptLB_X				,\
						ptLB_Y				,\
 						Mean_GV				,\
 						Sigma				,\
 						Min_GV				,\
 						Max_GV				,\
 						BackGround_GV		,\
						Center_X			,\
						Center_Y			,\
 						Breadth				,\
 						Compactness			,\
						Roundness			,\
 						F_Elongation		,\
 						F_Min				,\
 						F_Max				,\
						MuraObj				,\
						MuraBk				,\
						MuraActive			,\
						MuraBright			,\
						MeanAreaRatio		,\
						In_Count			,\
						Judge_GV			,\
						Area_Per			,\
 						Lab_Avg_L			,\
 						Lab_Avg_a			,\
 						Lab_Avg_b			,\
						Lab_Degree			,\
						Lab_Lenght			,\
 						Lab_diff_L			,\
						Lab_diff_a			,\
						Lab_diff_b			,\
						MinBox_ratio		,\
						Use_Report			,,");
#if USE_ALG_HIST
		// KSW 17.06.24 ��ü ������׷�
		for (int m=0 ; m<IMAGE_MAX_GV ; m++)
		{
			fprintf_s(out, "%d,", m);
		}
#endif
		fprintf_s(out, "\n");
 	}
 	
	// KSW 17.07.27 ���α׷� �ٿ� ����
	char szPath[MAX_PATH] = {0, };
 	for ( int nFori = 0 ; nFori<resultBlob->nDefectCount ; nFori++ )
	{
		if (resultBlob->bUseResult[nFori] == false) continue;
		// KSW 17.07.27 ���α׷� �ٿ� ����
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 �ҷ� ���� �������� ���α׷� �ٿ� �߻�
		//USES_CONVERSION;
		//char *cstrName = W2A( theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]) );

 		fprintf_s(out, "%d,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%d,%d,%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%s,%s,%f,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%s,,",
 			(*nImageDefectCount)++						,
			szPath										,
			resultBlob->nDefectColor			[nFori]	,
			resultBlob->nArea					[nFori]	,
			resultBlob->ptLT					[nFori].x	,
			resultBlob->ptLT					[nFori].y	,
			resultBlob->ptRT					[nFori].x	,
			resultBlob->ptRT					[nFori].y	,
			resultBlob->ptRB					[nFori].x	,
			resultBlob->ptRB					[nFori].y	,
			resultBlob->ptLB					[nFori].x	,
			resultBlob->ptLB					[nFori].y	,
			resultBlob->dMeanGV					[nFori]	,
			resultBlob->dSigma					[nFori]	,
			resultBlob->nMinGV					[nFori]	,
			resultBlob->nMaxGV					[nFori]	,
			resultBlob->dBackGroundGV			[nFori]	,
			resultBlob->nCenterx				[nFori]	,
			resultBlob->nCentery				[nFori]	,
			resultBlob->dBreadth				[nFori]	,
			resultBlob->dCompactness			[nFori]	,
			resultBlob->dRoundness				[nFori]	,
			resultBlob->dF_Elongation			[nFori]	,
			resultBlob->dF_Min					[nFori]	,
			resultBlob->dF_Max					[nFori]	,
			resultBlob->dMuraObj				[nFori]	,
			resultBlob->dMuraBk					[nFori]	,
			resultBlob->bMuraActive				[nFori] ? "Y" : "N",
			resultBlob->bMuraBright				[nFori] ? "Y" : "N",
			resultBlob->dF_MeanAreaRatio		[nFori] ,
			resultBlob->nIn_Count				[nFori] ,
			resultBlob->nJudge_GV				[nFori] ,
			resultBlob->dF_AreaPer				[nFori] ,
			resultBlob->Lab_avg_L				[nFori]	,
			resultBlob->Lab_avg_a				[nFori]	,
			resultBlob->Lab_avg_b				[nFori]	,
			resultBlob->Lab_Degree				[nFori],
			resultBlob->Lab_Lenght				[nFori],
			resultBlob->Lab_diff_L				[nFori]	,
			resultBlob->Lab_diff_a				[nFori]	,
			resultBlob->Lab_diff_b				[nFori]	,
			resultBlob->dF_MinBoxRatio			[nFori],
			resultBlob->strColor				[nFori].c_str()	,
			resultBlob->bUseResult				[nFori]?"Y":"N");

#if USE_ALG_HIST
		// KSW 17.06.24 ��ü ������׷�
		for (int m=0 ; m<IMAGE_MAX_GV ; m++)
		{
			fprintf_s(out, "%d,", resultBlob->nHist[nFori][m]);
		}
#endif

		fprintf_s(out, "\n");
	}

 	fclose(out);
	out = NULL;
}

//////////////////////////////////////////////////////////////////////////
// AVI�� ��� - ȸ�� ����. ( ������ ��ǥ�� ���� ptCorner )
// SVI�� ��� - ���� �ؾ� ��.
// APP�� ��� - ȸ��(Align_RotateImage) ��, Pad / Active / Edge ROI �����ؾ� ��.
//				�� �ܰ� �������κ��� ���� �Ÿ� -> �˻� ROI ����
//				���߿� Burr �ҷ��� ũ�� ������, Auto �ȵɼ� ����. ( ���� �ʿ� )
//////////////////////////////////////////////////////////////////////////	
long InspectAlgorithmInterface::AcquireAutoRoiAndAlignImage(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, int nRatio, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, double dPannelSizeX,double dPannelSizeY)
{
	// test
	CCPUTimer tact;
	tact.Start();
	tAlignInfo* pStCamAlignInfo = new tAlignInfo();	

	// Align �˰��� �˻� �Ķ���� ��������
	// Align Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = {0, };
	swprintf( wstrID, _T("%s"), (LPCWSTR)strPanelID );

	// 0�� �̹����� ����� �ϹǷ� ������ϴ� ��ġ ����
	// ���� APP ���� ����� �� �̹����� �˻� �����忡 �ѱ�� �־���
	// KSW 17.07.10 Cell �߽� ��ǥ �߰�
	// LJM 17.09.07 EQType�з� �Ű����� �߰�
	// SCJ 18.08.03 Cam Resolution �� Pannel ���� ������ �߰�
	int nEQType = theApp.m_Config.GetEqpType();
	long nErrorCode = Align_FindActive(MatOrgImage, dAlgPara, pStCamAlignInfo->dAlignTheta, pStCamAlignInfo->ptCorner, nCameraNum, nEQType, dCamResolution, dPannelSizeX, dPannelSizeY, nRatio, pStCamAlignInfo->ptCellCenter, wstrID);

	// Align �� ���� P/S ��� ����
	pStCamAlignInfo->nRatio = nRatio;

	// �̹��� ���� (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	// PS ���󿡵���, 1�� ��ǥ�� ����
	pStCamAlignInfo->ptCellCenter.x /= nRatio;
	pStCamAlignInfo->ptCellCenter.y /= nRatio;

	// ����� ���� ��������
	dResult[0] = pStCamAlignInfo->dAlignTheta;

	// y�� Pixel ����
	dResult[1] = abs(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y - pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Find Active : %.2f"), tact.Stop(false)/1000.);

	// ������ ���, ���� �ڵ� & �α� ���
	//if( nErrorCode == E_ERROR_CODE_TRUE)	// Warning �� ��쿡�� �˻�� ����
	{
		// ȸ�� �߽�
		pStCamAlignInfo->ptAlignCenter.x = (int)(MatOrgImage.cols / 2);
		pStCamAlignInfo->ptAlignCenter.y = (int)(MatOrgImage.rows / 2);

		cv::Point ptAdjCorner[E_CORNER_END] = {(0, 0), };

		// ptCorner ���� -> ptAdjCorner
		AdjustAlignInfo(pStCamAlignInfo, ptAdjCorner);

		pStCamAlignInfo->SetAdjustAlignInfo(ptAdjCorner, dAlgPara, MatOrgImage.cols - 1, MatOrgImage.rows - 1);
		
		theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false)/1000.);

		stCamAlignInfo.SetAdjustAlignInfoRatio(pStCamAlignInfo, 1.0 / nRatio);		// PixelShift ���ο� ���� '���� ũ�� ����'���� ����� ���� ���� (������ ����)
	}

	SAFE_DELETE(pStCamAlignInfo);

	return nErrorCode;
}

long InspectAlgorithmInterface::PanelCurlJudge(cv::Mat& matSrcBuf, double* dPara, tAlignInfo* stCamAlignInfo, ResultBlob_Total* pResultBlobTotal, int nImageNum, stMeasureInfo* stCurlMeasure,CString strPath)
{
	BOOL bCurl = FALSE;
	BOOL bSaveImage = theApp.GetCommonParameter()->bIFImageSaveFlag;
	CString strSavePath;
	strSavePath.Format(_T("%s%s"), strPath, pResultBlobTotal->GetPanelID());
	Panel_Curl_Judge(matSrcBuf, dPara, stCamAlignInfo->ptCorner, bCurl, stCurlMeasure,bSaveImage,strSavePath);
	
	if(bCurl)
	{
		stDefectInfo* pCurl = new stDefectInfo(4, nImageNum); 

		pCurl->ptLT			[0].x	= 0;
		pCurl->ptLT			[0].y	= 0;
		pCurl->ptRT			[0].x	= matSrcBuf.cols;	
		pCurl->ptRT			[0].y	= 0;
		pCurl->ptRB			[0].x	= matSrcBuf.cols;	
		pCurl->ptRB			[0].y	= matSrcBuf.rows;	
		pCurl->ptLB			[0].x	= 0;
		pCurl->ptLB			[0].y	= matSrcBuf.rows;	
		pCurl->nDefectColor	[0]		= E_DEFECT_COLOR_DARK;
		pCurl->nPatternClassify[0]	= nImageNum;
		pCurl->nDefectJudge	[0]		= E_DEFECT_JUDGEMENT_APP_CURL;
		
		pCurl->nDefectCount = 1;

		pResultBlobTotal->AddTail_ResultBlobAndAddOffset(pCurl, NULL);
	}

	return E_ERROR_CODE_TRUE;
}

long InspectAlgorithmInterface::AcquireAutoRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio,int nAlignImageClassify, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, int nColorCode ,double dPannelSizeX, double dPannelSizeY)
{
	// test
	CCPUTimer tact;
	tact.Start();
	//tAlignInfo* pStCamAlignInfo = new tAlignInfo();

	// Align �˰��� �˻� �Ķ���� ��������
	// Align Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum,nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID,_T("Align"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP �߰��̹���

	// �߰� ���� ���� ��� & ���� ����

	// 0�� �̹����� ����� �ϹǷ� ������ϴ� ��ġ ����
	// ���� APP ���� ����� �� �̹����� �˻� �����忡 �ѱ�� �־���
	// KSW 17.07.10 Cell �߽� ��ǥ �߰�
	// LJM 17.09.07 EQType�з� �Ű����� �߰�
	// SCJ 18.08.03 Cam Resolution �� Pannel ���� ������ �߰�
	
	int nEQType = theApp.m_Config.GetEqpType();

	long nErrorCode = Align_FindActive_LCP(MatOrgImage, MatDrawImage ,stCamAlignInfo.matMask_BackStage, stCamAlignInfo.matSrc_Stage,stCamAlignInfo.matMask_Active,stCamAlignInfo.matMask_Hole ,nColorCode ,dAlgPara, stCamAlignInfo.dAlignTheta, stCamAlignInfo.ptCorner, stCamAlignInfo.rcAlignCellROI, nAlignImageClassify ,nCameraNum, theApp.GetImageClassify(nImageNum) ,nEQType, dCamResolution, dPannelSizeX, dPannelSizeY, nRatio, stCamAlignInfo.ptCellCenter,stCamAlignInfo.ptAlignCenter ,wstrID, wstrAlignPath);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

long InspectAlgorithmInterface::AcquireAutoRoiAndAlignImage_LCP_Mask(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, int nRatio, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, double dPannelSizeX, double dPannelSizeY, int nColorCode)
{
	// test
	CCPUTimer tact;
	tact.Start();
	//tAlignInfo* pStCamAlignInfo = new tAlignInfo();

	// Align �˰��� �˻� �Ķ���� ��������
	// Align Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	wchar_t wstrMaskPath[MAX_PATH] = { 0, };
	swprintf(wstrMaskPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Label Mask"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrMaskPath, NULL);
	if (true) CreateDirectory(wstrMaskPath, NULL); //LCP �߰��̹���
	// 0�� �̹����� ����� �ϹǷ� ������ϴ� ��ġ ����
	// ���� APP ���� ����� �� �̹����� �˻� �����忡 �ѱ�� �־���
	// KSW 17.07.10 Cell �߽� ��ǥ �߰�
	// LJM 17.09.07 EQType�з� �Ű����� �߰�
	// SCJ 18.08.03 Cam Resolution �� Pannel ���� ������ �߰�

	int nEQType = theApp.m_Config.GetEqpType();

	long nErrorCode = Align_FindActive_LCP_Mask(MatOrgImage, stCamAlignInfo.matMask_Active, stCamAlignInfo.matMask_Label, stCamAlignInfo.matMask_Hole ,dAlgPara, nCameraNum, theApp.GetImageClassify(nImageNum), nEQType, dCamResolution, dPannelSizeX, dPannelSizeY,nColorCode, nRatio, stCamAlignInfo.ptCellCenter, wstrID , wstrMaskPath);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

long InspectAlgorithmInterface::AcquireAutoBackRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio, int nAlignImageClassify, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution,int nColorCode ,double dPannelSizeX, double dPannelSizeY)
{
	// test
	CCPUTimer tact;
	tact.Start();
	//tAlignInfo* pStCamAlignInfo = new tAlignInfo();

	// Align �˰��� �˻� �Ķ���� ��������
	// Align Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlignPath2[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath2, _T("%s%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID);
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath2, NULL); //LCP �߰��̹���

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Back"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP �߰��̹���
	// �߰� ���� ���� ��� & ���� ����

	// 0�� �̹����� ����� �ϹǷ� ������ϴ� ��ġ ����
	// ���� APP ���� ����� �� �̹����� �˻� �����忡 �ѱ�� �־���
	// KSW 17.07.10 Cell �߽� ��ǥ �߰�
	// LJM 17.09.07 EQType�з� �Ű����� �߰�
	// SCJ 18.08.03 Cam Resolution �� Pannel ���� ������ �߰�

	
	stCamAlignInfo.matMask_Active = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_BackStage = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_Hole = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_Label = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matSrc_DrawMain = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type()); //�ӽ�

	if (nColorCode != E_COLOR_CODE_MP && nColorCode != E_COLOR_CODE_TRANSPARENCY) {
		stCamAlignInfo.matMask_VacuumHole = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
		stCamAlignInfo.matSrc_Stage = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	}
	else {
		if (stCamAlignInfo.matMask_VacuumHole.empty() || stCamAlignInfo.matSrc_Stage.empty()) return E_ERROR_CODE_EMPTY_BUFFER;
	}
	

	int nEQType = theApp.m_Config.GetEqpType();

	long nErrorCode = Align_FindActive_LCP_BACK(MatOrgImage, stCamAlignInfo.matMask_BackStage , dAlgPara, stCamAlignInfo.rcAlignActiveROI, nAlignImageClassify, nCameraNum, theApp.GetImageClassify(nImageNum), nEQType, dCamResolution, dPannelSizeX, dPannelSizeY, nRatio, stCamAlignInfo.ptStandard,  wstrID, wstrAlignPath);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

long InspectAlgorithmInterface::AcquireAutoVaccumRoiAndAlignImage_LCP(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, cv::Mat MatDrawImage, int nRatio, int nAlignImageClassify, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, int nColorCode, double dPannelSizeX, double dPannelSizeY)
{
	// test
	CCPUTimer tact;
	tact.Start();
	//tAlignInfo* pStCamAlignInfo = new tAlignInfo();

	// Align �˰��� �˻� �Ķ���� ��������
	// Align Parameter ������ ������ ��. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// �߰� ���� ���� ��� & ���� ����
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Vaccum_Mask"));
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP �߰��̹���

	// �߰� ���� ���� ��� & ���� ����

	// 0�� �̹����� ����� �ϹǷ� ������ϴ� ��ġ ����
	// ���� APP ���� ����� �� �̹����� �˻� �����忡 �ѱ�� �־���
	// KSW 17.07.10 Cell �߽� ��ǥ �߰�
	// LJM 17.09.07 EQType�з� �Ű����� �߰�
	// SCJ 18.08.03 Cam Resolution �� Pannel ���� ������ �߰�


	int nEQType = theApp.m_Config.GetEqpType();

	long nErrorCode = Align_FindActive_LCP_Vaccum(MatOrgImage, stCamAlignInfo.matMask_VacuumHole, dAlgPara, stCamAlignInfo.rcAlignActiveROI, nAlignImageClassify, nCameraNum, theApp.GetImageClassify(nImageNum), nEQType, dCamResolution, dPannelSizeX, dPannelSizeY, nRatio, stCamAlignInfo.ptStandard, wstrID, wstrAlignPath);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false) / 1000.);

	return nErrorCode;
}

long InspectAlgorithmInterface::Empty_Stage_Dust_Judge(cv::Mat matSrc_Stage, cv::Mat matVaccumMask, CString strPanelID, CString strDrive, double& dStage_Mean_Gv,double* dAlignPara) {

	//////////////////////////////////////////////////////////////////////////
	int nUse_Flag = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_USE_FLAG];

	int nImageSave = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_TEXT];
	int nBlur_Size = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_BLUR_SIZE]; // 30
	int nThreshold = (int)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_THRESHOLD]; // 15

	double dJudge_Ratio = (double)dAlignPara[E_PARA_LCP_STAGE_SUBSTANCE_JUDGE_RATIO]; // 1.2

	//////////////////////////////////////////////////////////////////////////
	if (nUse_Flag <= 0) return E_ERROR_CODE_TRUE;
	//////////////////////////////////////////////////////////////////////////


	// �߰� ���� ���� ��� & ���� ����

// 	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
// 	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Stage_Dust"));
// 	if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL); //LCP �߰��̹���

	wchar_t wstrAlignPathOri[MAX_PATH] = { 0, };
	swprintf(wstrAlignPathOri, _T("%s%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID);

	if (true) CreateDirectory(wstrAlignPathOri, NULL); //LCP �߰��̹���

	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Stage_Dust"));

	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP �߰��̹���

	//�������� ���� �˶� �˰��� �߰� �ڸ�
	cv::Mat matStageBuf = matSrc_Stage.clone();

	// Stage ���� üũ

	if (nImageSave > 0) {
		CString strPath;
		strPath.Format(_T("%s\\Stage_Dust.jpg"), wstrAlignPath);

		cv::imwrite((cv::String)(CStringA)strPath, matStageBuf);
	}

	cv::Mat dust_check;
	cv::blur(matStageBuf, dust_check, cv::Size(nBlur_Size, nBlur_Size));
	cv::subtract(dust_check, matStageBuf, dust_check);
	cv::threshold(dust_check, dust_check, nThreshold, 255, 0);
	// stage dust �˶�����

	cv::Mat invert2 = matVaccumMask.clone();
	//::invert(invert2, invert2);
	for (int i = 0; i < dust_check.rows; i++)
	{
		for (int j = 0; j < dust_check.cols; j++)
		{


			invert2.data[i*invert2.cols + j] = 255 - invert2.data[i*invert2.cols + j];

			if (invert2.data[i*invert2.cols + j] == 0)
				dust_check.data[i*invert2.cols + j] = 0;


			if (j < 200)
				dust_check.data[i*invert2.cols + j] = 0;
			if (j > dust_check.cols - 250)
				dust_check.data[i*invert2.cols + j] = 0;

		}
	}

	double dMean_dust = cv::mean(dust_check)[0];

	dStage_Mean_Gv = dMean_dust;

	if (dMean_dust >= dJudge_Ratio) {
		if (nImageSave > 0) {
			CString strDust_Path;
			strDust_Path.Format(_T("%s\\Stage_dust_substarct_High_(%f).jpg"), wstrAlignPath ,dMean_dust);
			cv::imwrite((cv::String)(CStringA)strDust_Path, dust_check);
		}

		return E_ERROR_CODE_LCP_STAGE_DIRTY;
	}
	else {
		if (nImageSave > 0) {
			CString strDust_Path;
			strDust_Path.Format(_T("%s\\Stage_dust_substarct_Low_(%f).jpg"), wstrAlignPath,dMean_dust);
			cv::imwrite((cv::String)(CStringA)strDust_Path, dust_check);
		}

		return E_ERROR_CODE_TRUE;
	}

}