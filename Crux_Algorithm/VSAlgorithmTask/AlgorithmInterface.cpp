#include "stdafx.h"
#include "AlgorithmInterface.h"
#include "VSAlgorithmTask.h"

// 병렬 처리
#include <omp.h>
//#include <ppl.h>
//using namespace Concurrency;

// Align_RotateImage() 사용하기 위함
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
		int nThickness = 1;	// Rectangle 두께

		// Teaching Draw Rect
		if (nDrawMode == eBasicROI)
		{	color.val[0] = 0;	color.val[1] = 255;	 color.val[2] = 0;	}	// 설정 ROI
		else if (nDrawMode == eRndROI)
		{	color.val[0] = 255;	color.val[1] = 0;	color.val[2] = 0;	}	// Pol ROI
		else
		{	color.val[0] = 0;	color.val[1] = 255;	color.val[2] = 255;	}	// 비검사 영역 ROI

		// 직사각형 -> 사각형 변경
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
//	 1. 함 수 명 :	LogicStart
//	 2. 함수기능 :	하나의 셀 검사
//	 3. 인    수 :	MatOriginImage		: 원본 영상
//					MatDrawBuffer		: 결과 Draw 이미지
//					MatResultImg		: 알고리즘 결과 이미지
//					resultBlob_Total	: Pattern 별 모든 알고리즘 결과 취합 Buffer
//	 4. 반 환 값 :	완료 : true
//					실패 : false
//	 5. 작 성 일 :	2015/02/07
//	 6. 작 성 자 :	KSW
//	 7. 수정기록 :	2017/03/02 by CWH
//	 8. 참    고 :	
//----------------------------------------------------------------------------------------------------
long InspectAlgorithmInterface::StartLogic(CString strPanelID, CString strDrive, CString strLcp_Bubble_Path, tAlignInfo stThrdAlignInfo,
									cv::Mat MatOriginImage[][MAX_CAMERA_COUNT], cv::Mat& MatColorBuffer, cv::Mat& MatDrawBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
									ResultBlob_Total* pResultBlob_Total, const int nImageNum, const int nCameraNum, const int nThreadIndex, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	//////////////////////////////////////////////////////////////////////////
	// 파라미터 설정
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
		// RGB  패턴을 이미지 버퍼 마지막 3개에 임시로 복사한다.
		// makePolygonCellROI 하면 이미지가 망가짐으로 그전에 원본을 복사.
		//////////////////////////////////////////////////////////////////////////
	case EQP_AVI:
	case EQP_LCP:

//		switch (theApp.GetImageClassify(nImageNum))
//		{
//		case E_IMAGE_CLASSIFY_AVI_DUST:
//			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum]);		//메모리 할당 영상 전체
//			break;
//		case E_IMAGE_CLASSIFY_AVI_SIDE:
//			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum]);		//메모리 할당 영상 전체
//			break;
// 		case E_IMAGE_CLASSIFY_AVI_B:
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_B)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum]);		//메모리 할당 영상 전체
// 			break;
//		}


// 		LogicPara.MatOrgRGBAdd[2] = &MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum];

		// RGB 다 촬영 될때까지 대기
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
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_DUST)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 3][nCameraNum]);		//메모리 할당 영상 전체
// 			break;
// 		case E_IMAGE_CLASSIFY_AVI_SIDE: 
// 			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_SIDE)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 2][nCameraNum]);		//메모리 할당 영상 전체
// 			break;
//  		case E_IMAGE_CLASSIFY_AVI_COAXIAL:
//  			MatOriginImage[theApp.GetImageNum(E_IMAGE_CLASSIFY_AVI_COAXIAL)][nCameraNum].copyTo(MatOriginImage[MAX_GRAB_STEP_COUNT - 1][nCameraNum]);		//메모리 할당 영상 전체
//  			break;
// 		}
// 
// // 		// RGB 복사할때까지
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
// // 		//RGB 패턴을 알고리즘에 넘겨줄 준비를 한다. - 인덱스가 같음으로 같이 사용한다. => 나중에 틀려지면 변경필요
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

	// 판넬 ID
	int nLength = 0;
	nLength = _stprintf_s(LogicPara.tszPanelID, MAX_PATH, _T("%s"), (LPCWSTR)strPanelID);
	LogicPara.tszPanelID[nLength] = _T('\0');
	LogicPara.strPanelID.Format(_T("%s"), strPanelID);

	//////////////////////////////////////////////////////////////////////////
	// 영상 및 ROI 보정
	//////////////////////////////////////////////////////////////////////////

	// 이전 단계에서 Check
	// 	// 버퍼가 없는 경우. / 검사 불가 에러 코드 & 로그 출력
	// 	if( MatOriginImage[nImageNum][nCameraNum].empty() )
	// 	{
	// 		// Error Log
	// 		return E_ERROR_CODE_EMPTY_BUFFER;
	// 	}	

	// Teaching에서 현재 ROI 설정한 갯수 가져오기
	//int nROICnt = theApp.GetROICnt(nImageNum, nCameraNum);

	// 현재 Image 당 ROI 하나로 고정
	int nROICnt = 1;

	// 설정한 ROI 갯수가 없는 경우 검사 안함.
	// 에러인 경우, 에러 코드 & 로그 출력
	if( nROICnt <= 0 )
	{
		return E_ERROR_CODE_EMPTY_SET_ROI;
	}


	//////////////////////////////////////////////////////////////////////////
	// Cell Edge 부분 처리용 ( APP 사용 안함 )
	//////////////////////////////////////////////////////////////////////////
	double* dAlgPara;
	ENUM_KIND_OF_LOG eLogCamera;
	long nRoundErrorCode = E_ERROR_CODE_TRUE;

	switch(nEqpType)
	{
	case EQP_AVI:
		{
			//LCP Chole 미사용
 		}		
		break;

	case EQP_SVI:
		{
			// SVI 메모리 할당 ( 8bit )
			LogicPara.MatBKG = cv::Mat::zeros( LogicPara.MatOrgImage.size(), CV_8UC1 );

			// Align 알고리즘 검사 파라미터 가져오기
			dAlgPara = theApp.GetAlignParameter(nCameraNum);

			// 외곽 부분 처리
			nRoundErrorCode = makePolygonCellROI( LogicPara, stThrdAlignInfo, nImageNum, nCameraNum, dAlgPara, theApp.GetImageClassify(nImageNum), nRatio );
		}		
		break;

	case EQP_APP:
		{
 			// Align 알고리즘 검사 파라미터 가져오기
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

	// Round 셋팅 플래그 On인 경우 검사 안함
	if (nRoundErrorCode == E_ERROR_CODE_ALIGN_ROUND_SETTING)
	{
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Setting Success. Algorithm Skip. PanelID: %s, CAM: %02d, Img: %s."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum));

		// 검사 로직 미실행
		return E_ERROR_CODE_TRUE;
	}

	// 에러코드가 발생한 경우 Log
	else if (nRoundErrorCode != E_ERROR_CODE_TRUE)
	{
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Fail. PanelID: %s, CAM: %02d, Img: %s Error Code:%d."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum), nRoundErrorCode);

		// 검사 로직 미실행
		return E_ERROR_CODE_TRUE;
	}

	// 에러없이 동작한 경우
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("makePolygonCellROI Success. PanelID: %s, CAM: %02d, Img: %s."), strPanelID, nCameraNum, theApp.GetGrabStepName(nImageNum));



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 알고리즘 함수 동작
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Teaching에서 설정한 Algorithm List 갯수 가져오기
	int nAlgCnt = MAX_MEM_SIZE_E_ALGORITHM_NUMBER;

	//////////////////////////////////////////////////////////////////////////
	//이물검사 다꺼져잇으면 ok
	//한개라도 안켜져 있으면 오류
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

	// Teaching 에서 설정한 ROI 갯수만큼 ( AVI 1개임.... )
// #ifndef _DEBUG
// #pragma omp parallel for
// #endif
	for (int nROINum = 0; nROINum < nROICnt; nROINum++)
	{
		// ROI 이름 현재 사용 안함
		// 검사 ROI 그리기
		DrawAdjustROI(MatDrawBuffer, stThrdAlignInfo.ptCorner, /*theApp.GetROIName(nImageNum, nCameraNum, nROINum)*/_T("ROI"), nROINum, eBasicROI);

		// Teaching 에서 설정한 Algorithm List 갯수만큼.
		
#ifndef _DEBUG
			
			// ini에서 Data 읽기 임시 추가 190218 YWS
			int nNested_offset = GetPrivateProfileInt(_T("InspInfo"), _T("Nested offset"), 1, VS_ALGORITHM_TASK_INI_FILE);
			int nThread_Count = GetPrivateProfileInt(_T("InspInfo"), _T("Thread Count"), 1, VS_ALGORITHM_TASK_INI_FILE);

		omp_set_nested(nNested_offset); // 기존 1
#pragma omp parallel for schedule(dynamic) num_threads(nThread_Count) // 기존 4

#endif
		for (int nAlgNum=0; nAlgNum < nAlgCnt; nAlgNum++)
		{
			// Teaching에서 Algorithm List 선택 확인.
			if (theApp.GetUseAlgorithm(nImageNum, LogicPara.nCameraNum, nROINum, nAlgNum))
			{
				// 알고리즘 동작
				long nErrorCode = StartLogicAlgorithm(strDrive, strLcp_Bubble_Path ,LogicPara, MatColorBuffer, MatResultImg, MatDrawBuffer, nImageNum, nROINum, nAlgNum, stThrdAlignInfo, pResultBlob_Total, bpInspectEnd, bpInspectAlgEnd, nRatio, eInspMode, WrtResultInfo, _mtp );
			}
		}
	}

	return E_ERROR_CODE_TRUE;
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	BlobFeatureSave
//	 2. 함수기능 :	Defect 결과 파일 저장
//	 3. 인    수 :	resultBlob		: Blob 정보
//					strPath			: 저장 경로
//					nThreadID		: 쓰레드 ID
//	 4. 반 환 값 :	
//	 5. 작 성 일 :	2015/03/27
//	 6. 작 성 자 :	KSW
//	 7. 수정기록 :
//	 8. 참    고 :
//----------------------------------------------------------------------------------------------------
void InspectAlgorithmInterface::BlobFeatureSave(stDefectInfo *resultBlob, CString strPath, int* nImageDefectCount)
{
	if( resultBlob == NULL )	return;

 	int	nDefectNum		= 0;
 	
 	// 결과 Log 저장
 	FILE *out = NULL;
 
 	if (nImageDefectCount == NULL)
 		nImageDefectCount = &nDefectNum;
 
 	if (*nImageDefectCount == 0)
 		_wfopen_s(&out, strPath, _T("wt"));
 	else
 		_wfopen_s(&out, strPath, _T("at"));
 
 	// 예외 처리.
 	if(out == NULL)		return;
 
	// 여러개의 ROI 결과 기입 시 최초에만 Header 기입
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
		// KSW 17.06.24 객체 히스토그램
		for (int m=0 ; m<IMAGE_MAX_GV ; m++)
		{
			fprintf_s(out, "%d,", m);
		}
#endif
		fprintf_s(out, "\n");
 	}
 	
	// KSW 17.07.27 프로그램 다운 수정
	char szPath[MAX_PATH] = {0, };
 	for ( int nFori = 0 ; nFori<resultBlob->nDefectCount ; nFori++ )
	{
		if (resultBlob->bUseResult[nFori] == false) continue;
		// KSW 17.07.27 프로그램 다운 수정
		memset(szPath, 0, sizeof(char) * MAX_PATH);
		WideCharToMultiByte(CP_ACP, 0, theApp.GetDefectTypeName(resultBlob->nDefectJudge[nFori]), -1, szPath, sizeof(szPath), NULL, NULL);

		// KSW 17.07.27 불량 개수 많아지면 프로그램 다운 발생
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
		// KSW 17.06.24 객체 히스토그램
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
// AVI의 경우 - 회전 안함. ( 꼭지점 좌표만 받음 ptCorner )
// SVI의 경우 - 와핑 해야 함.
// APP의 경우 - 회전(Align_RotateImage) 후, Pad / Active / Edge ROI 변경해야 함.
//				최 외곽 라인으로부터 설정 거리 -> 검사 ROI 설정
//				나중에 Burr 불량이 크게 있으면, Auto 안될수 있음. ( 검토 필요 )
//////////////////////////////////////////////////////////////////////////	
long InspectAlgorithmInterface::AcquireAutoRoiAndAlignImage(CString strPanelID, CString strDrive, cv::Mat& MatOrgImage, int nRatio, int nImageNum, int nCameraNum, tAlignInfo& stCamAlignInfo, double* dResult, double dCamResolution, double dPannelSizeX,double dPannelSizeY)
{
	// test
	CCPUTimer tact;
	tact.Start();
	tAlignInfo* pStCamAlignInfo = new tAlignInfo();	

	// Align 알고리즘 검사 파라미터 가져오기
	// Align Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = {0, };
	swprintf( wstrID, _T("%s"), (LPCWSTR)strPanelID );

	// 0번 이미지만 얼라인 하므로 얼라인하는 위치 변경
	// 기존 APP 에서 얼라인 된 이미지를 검사 스레드에 넘기고 있었음
	// KSW 17.07.10 Cell 중심 좌표 추가
	// LJM 17.09.07 EQType분류 매개변수 추가
	// SCJ 18.08.03 Cam Resolution 및 Pannel 실측 사이즈 추가
	int nEQType = theApp.m_Config.GetEqpType();
	long nErrorCode = Align_FindActive(MatOrgImage, dAlgPara, pStCamAlignInfo->dAlignTheta, pStCamAlignInfo->ptCorner, nCameraNum, nEQType, dCamResolution, dPannelSizeX, dPannelSizeY, nRatio, pStCamAlignInfo->ptCellCenter, wstrID);

	// Align 한 영상 P/S 모드 유무
	pStCamAlignInfo->nRatio = nRatio;

	// 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	// PS 영상에따라, 1샷 좌표로 변경
	pStCamAlignInfo->ptCellCenter.x /= nRatio;
	pStCamAlignInfo->ptCellCenter.y /= nRatio;

	// 계산한 각도 가져오기
	dResult[0] = pStCamAlignInfo->dAlignTheta;

	// y축 Pixel 차이
	dResult[1] = abs(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y - pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Find Active : %.2f"), tact.Stop(false)/1000.);

	// 에러인 경우, 에러 코드 & 로그 출력
	//if( nErrorCode == E_ERROR_CODE_TRUE)	// Warning 인 경우에도 검사는 진행
	{
		// 회전 중심
		pStCamAlignInfo->ptAlignCenter.x = (int)(MatOrgImage.cols / 2);
		pStCamAlignInfo->ptAlignCenter.y = (int)(MatOrgImage.rows / 2);

		cv::Point ptAdjCorner[E_CORNER_END] = {(0, 0), };

		// ptCorner 보정 -> ptAdjCorner
		AdjustAlignInfo(pStCamAlignInfo, ptAdjCorner);

		pStCamAlignInfo->SetAdjustAlignInfo(ptAdjCorner, dAlgPara, MatOrgImage.cols - 1, MatOrgImage.rows - 1);
		
		theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Align Image : %.2f"), tact.Stop(false)/1000.);

		stCamAlignInfo.SetAdjustAlignInfoRatio(pStCamAlignInfo, 1.0 / nRatio);		// PixelShift 여부에 따라 '원본 크기 기준'으로 얼라인 정보 쓰기 (스레드 전역)
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

	// Align 알고리즘 검사 파라미터 가져오기
	// Align Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum,nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// 중간 영상 저장 경로 & 폴더 생성
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID,_T("Align"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP 중간이미지

	// 중간 영상 저장 경로 & 폴더 생성

	// 0번 이미지만 얼라인 하므로 얼라인하는 위치 변경
	// 기존 APP 에서 얼라인 된 이미지를 검사 스레드에 넘기고 있었음
	// KSW 17.07.10 Cell 중심 좌표 추가
	// LJM 17.09.07 EQType분류 매개변수 추가
	// SCJ 18.08.03 Cam Resolution 및 Pannel 실측 사이즈 추가
	
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

	// Align 알고리즘 검사 파라미터 가져오기
	// Align Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	wchar_t wstrMaskPath[MAX_PATH] = { 0, };
	swprintf(wstrMaskPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Label Mask"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrMaskPath, NULL);
	if (true) CreateDirectory(wstrMaskPath, NULL); //LCP 중간이미지
	// 0번 이미지만 얼라인 하므로 얼라인하는 위치 변경
	// 기존 APP 에서 얼라인 된 이미지를 검사 스레드에 넘기고 있었음
	// KSW 17.07.10 Cell 중심 좌표 추가
	// LJM 17.09.07 EQType분류 매개변수 추가
	// SCJ 18.08.03 Cam Resolution 및 Pannel 실측 사이즈 추가

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

	// Align 알고리즘 검사 파라미터 가져오기
	// Align Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// 중간 영상 저장 경로 & 폴더 생성
	wchar_t wstrAlignPath2[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath2, _T("%s%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID);
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath2, NULL); //LCP 중간이미지

	// 중간 영상 저장 경로 & 폴더 생성
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Back"));
	//if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL);
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP 중간이미지
	// 중간 영상 저장 경로 & 폴더 생성

	// 0번 이미지만 얼라인 하므로 얼라인하는 위치 변경
	// 기존 APP 에서 얼라인 된 이미지를 검사 스레드에 넘기고 있었음
	// KSW 17.07.10 Cell 중심 좌표 추가
	// LJM 17.09.07 EQType분류 매개변수 추가
	// SCJ 18.08.03 Cam Resolution 및 Pannel 실측 사이즈 추가

	
	stCamAlignInfo.matMask_Active = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_BackStage = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_Hole = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matMask_Label = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type());
	stCamAlignInfo.matSrc_DrawMain = cv::Mat::zeros(MatOrgImage.size(), MatOrgImage.type()); //임시

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

	// Align 알고리즘 검사 파라미터 가져오기
	// Align Parameter 별도로 정의할 것. ToDo.
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// Cell ID
	wchar_t wstrID[MAX_PATH] = { 0, };
	swprintf(wstrID, _T("%s"), (LPCWSTR)strPanelID);

	// 중간 영상 저장 경로 & 폴더 생성
	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Vaccum_Mask"));
	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP 중간이미지

	// 중간 영상 저장 경로 & 폴더 생성

	// 0번 이미지만 얼라인 하므로 얼라인하는 위치 변경
	// 기존 APP 에서 얼라인 된 이미지를 검사 스레드에 넘기고 있었음
	// KSW 17.07.10 Cell 중심 좌표 추가
	// LJM 17.09.07 EQType분류 매개변수 추가
	// SCJ 18.08.03 Cam Resolution 및 Pannel 실측 사이즈 추가


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


	// 중간 영상 저장 경로 & 폴더 생성

// 	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
// 	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Stage_Dust"));
// 	if (theApp.GetCommonParameter()->bIFImageSaveFlag > 0) CreateDirectory(wstrAlignPath, NULL); //LCP 중간이미지

	wchar_t wstrAlignPathOri[MAX_PATH] = { 0, };
	swprintf(wstrAlignPathOri, _T("%s%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID);

	if (true) CreateDirectory(wstrAlignPathOri, NULL); //LCP 중간이미지

	wchar_t wstrAlignPath[MAX_PATH] = { 0, };
	swprintf(wstrAlignPath, _T("%s%s\\%s"), (LPCWSTR)ALG_RESULT_PATH, (LPCWSTR)strPanelID, _T("Align_Stage_Dust"));

	if (true) CreateDirectory(wstrAlignPath, NULL); //LCP 중간이미지

	//빈스테이지 먼지 알람 알고리즘 추가 자리
	cv::Mat matStageBuf = matSrc_Stage.clone();

	// Stage 먼지 체크

	if (nImageSave > 0) {
		CString strPath;
		strPath.Format(_T("%s\\Stage_Dust.jpg"), wstrAlignPath);

		cv::imwrite((cv::String)(CStringA)strPath, matStageBuf);
	}

	cv::Mat dust_check;
	cv::blur(matStageBuf, dust_check, cv::Size(nBlur_Size, nBlur_Size));
	cv::subtract(dust_check, matStageBuf, dust_check);
	cv::threshold(dust_check, dust_check, nThreshold, 255, 0);
	// stage dust 알람조건

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