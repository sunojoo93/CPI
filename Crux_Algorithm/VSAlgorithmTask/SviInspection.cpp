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
	// SVI 판정하는 구문 없음
	// SVI 불량은 모두 MURA 이므로 Rect 안그림
	CCPUTimer timerDrawRect;
	timerDrawRect.Start();
	
	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// 미니맵에 Rect 안그림
		resultPanelData.m_ListDefectInfo[i].Draw_Defect_Rect = false;
	}
	timerDrawRect.End();
	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Draw Rect tact time %.2f"), timerDrawRect.GetTime()/1000.);

	return true;
}

// 외곽 부분 처리
long SviInspection::makePolygonCellROI(const tLogicPara& LogicPara, tAlignInfo& stThrdAlignInfo, int nImageNum, int nCameraNum, double* dAlgPara, int nAlgImg, int nRatio)
{
	// 예외 처리
	if( dAlgPara == NULL )	return E_ERROR_CODE_EMPTY_PARA;

	wchar_t wstrID[MAX_PATH] = {0, };
	swprintf( wstrID, _T("%s"), LogicPara.tszPanelID );

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum) );

	// 에러 코드
	long nErrorCode = Align_FindFillOutArea(LogicPara.MatOrgImage, LogicPara.MatBKG, theApp.m_pGrab_Step[nImageNum].tRoundSet, theApp.m_pGrab_Step[nImageNum].tCHoleSet, stThrdAlignInfo.tCHoleAlignData->matCHoleROIBuf[nAlgImg], stThrdAlignInfo.tCHoleAlignData->rcCHoleROI[nAlgImg], stThrdAlignInfo.tCHoleAlignData->bCHoleAD[nAlgImg],
		dAlgPara, nAlgImg, nCameraNum, nRatio, nEqpType, strAlgLog, wstrID, stThrdAlignInfo.ptCorner);

	return nErrorCode;
}

// SVI 판정 함수
bool SviInspection::Judgement(CWriteResultInfo WrtResultInfo, ResultPanelData& resultPanelData, cv::Mat(*MatDrawBuffer)[MAX_CAMERA_COUNT], tCHoleAlignInfo& tCHoleAlignData,
								const CString strModelID, const CString strLotID, const CString strPanelID, const CString strDrive, int nRatio, const int nCamNum,
								ENUM_INSPECT_MODE eInspMode, cv::Mat MatOrgImage[][MAX_CAMERA_COUNT], cv::Mat MatColorImage[][MAX_CAMERA_COUNT], bool bUseInspect, int nStage)
{
	resultPanelData.m_ResultHeader.SetInspectEndTime();

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement Start. Panel ID : %s"), strPanelID);	

	// UI - 검사 실행 체크
	// 검사 동작한 경우만 실행
	// Grab만 한 경우 동작 안함 ( 불량 없음 )
	if( bUseInspect )
	{
		// 중복 좌표 제거
 		DeleteOverlapDefect(resultPanelData);
 		theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Delete Overlap Defect End. Panel ID : %s"), strPanelID);

		//	김형주 18.12.06
		//	MergeTool Falg 와 상관없이 무조건 동작
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

		// 외곽 정보 사용하는 경우, Judgement()에서 파일 저장 ( 중복 제거시, 해당 불량 외곽선 안그림 )
		// 사용 안하는 경우, Alg단에서 파일 저장 ( 중복 제거되도, 해당 불량 외곽선 그림 )
		if( USE_ALG_CONTOURS )
		{
			// KSW 18.02.05 - SVI 외곽 정보 저장 경로
			wchar_t strContoursPath[MAX_PATH] = {0};

			// Contours.Merge 파일 경로 설정 180322 YSS
			CString strResultPath = RESULT_PATH;
			//swprintf_s( strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)RESULT_PATH, (LPCWSTR)strPanelID );
			swprintf_s(strContoursPath, _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID);

			// KSW 17.11.29 - AVI & SVI 외곽 정보 저장 ( AVI & SVI 머지 툴 )
			JudgeSaveContours(resultPanelData, strContoursPath);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Contours End. Panel ID : %s"), strPanelID);

			// AutoRun 모드에서 MergeTool 기능 활성화인 경우 D:\MergeTool\ 경로내에 파일 복사 진행 
			if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
			{
				CString strDest;
				strDest.Format(_T("%s\\%s\\Contours.Merge"), MERGETOOL_PATH, strPanelID);
				CopyFile(strContoursPath, strDest, FALSE);
			}
		
			// KSW 18.02.05 - Mura 외곽선 정보 저장 경로
			swprintf_s( strContoursPath, _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, (LPCWSTR)strPanelID );

			// KSW 18.02.05 - Mura 외곽선 정보 저장
			JudgeSaveMuraContours(resultPanelData, strContoursPath);
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Save Mura Contours End. Panel ID : %s"), strPanelID);
		}
	}

	JudgementRepair(strPanelID, resultPanelData, WrtResultInfo);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Repair End. Panel ID : %s"), strPanelID);	

	// 17.07.12 Panel Grade 판정 추가
	JudgementPanelGrade(resultPanelData);

	// Work 좌표 계산 및 불량 순서대로 Numbering
	NumberingDefect(strModelID, strPanelID, strLotID, WrtResultInfo, resultPanelData, nRatio);

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_BASIC, TRUE, FALSE, _T("Judge - Numbering Defect End. Panel ID : %s"), strPanelID);

	// 상위 보고 룰 적용 (특정 불량 필터링, 대표 불량 선정)
	ApplyReportRule(resultPanelData);

	// 	// 결과에 따라서 CELL : NG / OK 판정
	// 	if ( resultPanelData.m_ListDefectInfo.GetCount() > 0)
	// 		resultPanelData.m_ResultPanel.Judge = _T("NG");
	// 	else
	// 		resultPanelData.m_ResultPanel.Judge = _T("OK");

	theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("Judgement End. Panel ID : %s (%s)"), strPanelID, resultPanelData.m_ResultPanel.Judge);

	return true;
}

//----------------------------------------------------------------------------------------------------
//	 1. 함 수 명 :	StartLogicAlgorithm
//	 2. 함수기능 :	Algorithm DLL
//	 3. 인    수 :	
//	 4. 반 환 값 :	완료 : true
//					실패 : false
//	 5. 작 성 일 :	17.12.04
//	 6. 작 성 자 :	KSW
//	 7. 수정기록 :	김형주 엄청 수정중.
//	 8. 참    고 :  알고리즘 통합
//----------------------------------------------------------------------------------------------------
long SviInspection::StartLogicAlgorithm(const CString strDrive, const CString strLcp_Bubble_Path, const tLogicPara& LogicPara, cv::Mat& MatColorBuffer, cv::Mat MatResultImg[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER][MAX_MEM_SIZE_E_MAX_INSP_TYPE],
	cv::Mat& MatDrawBuffer, const int nImageNum, const int nROINumber, const int nAlgorithmNumber, tAlignInfo stThrdAlignInfo, ResultBlob_Total* pResultBlob_Total, bool bpInspectEnd[][MAX_CAMERA_COUNT], bool bpInspectAlgEnd[][MAX_CAMERA_COUNT][MAX_MEM_SIZE_E_ALGORITHM_NUMBER], int nRatio, ENUM_INSPECT_MODE eInspMode, CWriteResultInfo& WrtResultInfo, const double* _mtp)
{
	// 파라미터 설정
	cv::Mat				MatOriginImage_			= LogicPara.MatOrgImage;
	int					nAlgorithmNumber_		= nAlgorithmNumber;					// 현재 알고리즘	
	int					nCameraNum_				= LogicPara.nCameraNum;				// 검사 카메라
	int					nThrdIndex_				= LogicPara.nThreadLog;
	cv::Mat				MatBKG_					= LogicPara.MatBKG;					// Cell 이외 영역 ( Dust, Black 패턴은 없음 )

	ENUM_KIND_OF_LOG	eLogCamera				= (ENUM_KIND_OF_LOG)nCameraNum_;

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlgorithmParameter(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// 공통 파라미터 ( 중간 결과 영상 필요한 파라미터 )
	int nCommonPara[] = {
		MAX_DEFECT_COUNT								,	// 00 : 불량 최대 갯수 ( MAX_DEFECT_COUNT 보다 작게 )
		theApp.GetCommonParameter()->bIFImageSaveFlag	,	// 01 : 알고리즘 중간결과 Image Save flag
		0												,	// 02 : 영상 저장 순서 카운트 ( Point의 경우 함수가 두개로 나눠 짐 )
		theApp.GetImageClassify(nImageNum)				,	// 03 : 현재 Alg 영상 번호
		nCameraNum_										,	// 04 : Cam Number ( 카메라 번호 / SVI )
		nROINumber										,	// 05 : ROI Number ( ROI 번호 )
		nAlgorithmNumber_								,	// 06 : 알고리즘 번호 ( nAlgorithmNumber_ )
		nThrdIndex_										,	// 07 : Thread ID
		theApp.GetCommonParameter()->bDrawDefectNum		,	// 08 : Draw Defect Num 표시
		theApp.GetCommonParameter()->bDrawDust			,	// 09 : Draw Dust 표시 ( Point 알고리즘이 돌아야 함 )
		nImageNum										,	// 10 : UI 상 패턴 순서 영상 번호
		(int)(stThrdAlignInfo.dAlignTheta * 1000)		,	// 11 : Cell 회전된 각도 ( Align 계산 값, 소수점 3자리 까지만... )
		(int)stThrdAlignInfo.ptAlignCenter.x			,	// 12 : Cell 회전된 중심 x좌표
		(int)stThrdAlignInfo.ptAlignCenter.y			,	// 13 : Cell 회전된 중심 y좌표
		1													// 14 : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)
	};

	// 검사 결과(불량) 정보 구조체
	stDefectInfo* pResultBlob = new stDefectInfo(MAX_DEFECT_COUNT, nImageNum);

	// 엔지니어 판정 구조체 가져오기
	STRU_DEFECT_ITEM* EngineerDefectJudgment = theApp.GetDefectFilteringParam(nImageNum, nCameraNum_, nROINumber, nAlgorithmNumber_);

	// Alg DLL Start Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm Start. PanelID: %s, CAM: %02d, ROI: %02d, Img: %s."),
		theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum));

	// 저장 경로 - 통합
	wchar_t pathAlgorithms[4][1024] = {0, };

	CString strAlgResultPath = ALG_RESULT_PATH;
	CString strResultPath = RESULT_PATH;
	CString strMergeToolPath = MERGETOOL_PATH;

	// 개발자모드 영상 저장 경로
	CreateDirectory(ALG_RESULT_PATH, NULL);
	swprintf( pathAlgorithms[0], _T("%s%s\\"), (LPCWSTR)strAlgResultPath, LogicPara.tszPanelID );
	CreateDirectory(pathAlgorithms[0], NULL);

	// 외곽선 정보 저장 경로
	swprintf( pathAlgorithms[1], _T("%s\\%s\\mura.coord"), (LPCWSTR)strResultPath, LogicPara.tszPanelID );
	
	// Merge 정보 저장 경로
	swprintf( pathAlgorithms[2], _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strResultPath, LogicPara.tszPanelID );
	if (eInspMode == eAutoRun && theApp.GetMergeToolUse())
	{
		swprintf( pathAlgorithms[2], _T("%s\\%s\\Contours.Merge"), (LPCWSTR)strMergeToolPath, LogicPara.tszPanelID );
	}

	// 패널 ID 정보 저장
	swprintf( pathAlgorithms[3], _T("%s"), LogicPara.tszPanelID );

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), LogicPara.tszPanelID, theApp.GetGrabStepName(nImageNum) );
	
	// SVI Main 검사 알고리즘
	long lErrorCode = SVI_FindMuraDefect(MatOriginImage_, MatBKG_, MatDrawBuffer, stThrdAlignInfo.ptCorner, dAlgPara, nCommonPara, pathAlgorithms, EngineerDefectJudgment, pResultBlob, strAlgLog);			

	// 에러인 경우, 에러 코드 & 로그 출력
	if( lErrorCode != E_ERROR_CODE_TRUE )
	{
		// Alg DLL End Log
		theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End Fail.  PanelID: %s, CAM: %02d ROI: %02d, Img: %s Error Code:%03d."),
			theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), lErrorCode);

		// 메모리 해제
		delete pResultBlob;
		pResultBlob = NULL;

		return true;
	}

	// 비검사 영역 삭제 ( 원본 영상 그대로 계산하므로, Left-Top 보정 안해도 됨 : NULL )
	m_fnDefectFiltering(MatDrawBuffer, nImageNum, nCameraNum_, pResultBlob, stThrdAlignInfo, nRatio);

	// 불량 갯수 ( 어느 패턴, 어느 알고리즘에서 불량 몇개 검출했는지 로그 )
	int nTotalblob = pResultBlob->nDefectCount;
	
	// 필요할 경우 사용
	if( theApp.GetCommonParameter()->bIFImageSaveFlag )
	{
		// Alg 중간 결과 경로
		// 알고리즘 불량 결과 정보 저장
		// MatDrawBuffer 영상에 숫자 표시해야 알 수있음.
		// m_fnDrawDefectImage() 함수에서 상위보고 용도로 사용 중.
		// 숫자 같은거 표시해도 되는지 확인 필요
		// Judgement() 에서 사용 중...

		CString strResultROIFile = _T("");
		strResultROIFile.Format(_T("%s\\%s\\ROI\\IMG-%02d_CAM-%02d_ROI-%02d %s Result.csv"), 
			INSP_PATH, LogicPara.strPanelID, nImageNum, nCameraNum_, nROINumber, 
			theApp.GetAlgorithmName(nAlgorithmNumber_));

		// 불량 정보 저장
		BlobFeatureSave(pResultBlob, strResultROIFile);
	}

	// 불량 정보 병합
	pResultBlob_Total->AddTail_ResultBlobAndAddOffset(pResultBlob, NULL);

	// Alg DLL End Log
	theApp.WriteLog(eLogCamera, eLOGLEVEL_DETAIL, TRUE, FALSE, _T("%s Algorithm End. PanelID: %s, CAM: %02d ROI: %02d, Img: %s.\n\t\t\t\t( DefectNum: %d )"),
		theApp.GetAlgorithmName(nAlgorithmNumber_), LogicPara.strPanelID, nCameraNum_, nROINumber, theApp.GetGrabStepName(nImageNum), nTotalblob);

	return true;
}

// Casting - stDefectInfo에서 필요한 부분 추출하여 ResultPanelData에 적재
bool SviInspection::GetDefectInfo(CWriteResultInfo WrtResultInfo, ResultDefectInfo* pResultDefectInfo, stDefectInfo* pResultBlob, int nBlobCnt, int nImageNum, int nCameraNum, int nRatio)
{
	// SVI 는 동일한 분해능으로 Cell 영역을 Warping 하여 좌표 추출 하므로 그대로 사용
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

	// 원본 이미지 에서 Crop 할 불량 영역 지정
	pResultDefectInfo->Pixel_Crop_Start_X	= pResultBlob->ptLT[nBlobCnt].x;
	pResultDefectInfo->Pixel_Crop_Start_Y	= pResultBlob->ptLT[nBlobCnt].y;
	pResultDefectInfo->Pixel_Crop_End_X		= pResultBlob->ptRB[nBlobCnt].x;
	pResultDefectInfo->Pixel_Crop_End_Y		= pResultBlob->ptRB[nBlobCnt].y;

	// BOE Eng'r 요청 Defect Size
	//pResultDefectInfo->Defect_Size			= (int)(pResultBlob->nArea[nBlobCnt] * WrtResultInfo.GetCamResolution(nCameraNum));
	pResultDefectInfo->Defect_Size		= (int)WrtResultInfo.CalcDistancePixelToUm(sqrt(pow((double)(pResultDefectInfo->Pixel_End_X - pResultDefectInfo->Pixel_Start_X), 2) +
																						pow((double)(pResultDefectInfo->Pixel_End_Y - pResultDefectInfo->Pixel_Start_Y), 2)), 
																						nCameraNum, nRatio);
	pResultDefectInfo->Defect_MeanGV		= pResultBlob->dMeanGV[nBlobCnt];
	pResultDefectInfo->Defect_Size_Pixel	= (int)pResultBlob->nArea[nBlobCnt];
	pResultDefectInfo->Img_Number			= nImageNum;
	pResultDefectInfo->Img_Size_X			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_Start_X - pResultDefectInfo->Pixel_Crop_Start_Y);	// 임시 불량 영상 가로 크기
	pResultDefectInfo->Img_Size_Y			= (DOUBLE)(pResultDefectInfo->Pixel_Crop_End_X - pResultDefectInfo->Pixel_Crop_End_Y);		// 임시 불량 영상 세로 크기
	pResultDefectInfo->Defect_Type			= pResultBlob->nDefectJudge[nBlobCnt];
	pResultDefectInfo->Pattern_Type			= pResultBlob->nPatternClassify[nBlobCnt];
	pResultDefectInfo->Camera_No			= nCameraNum;
	pResultDefectInfo->nRatio				= nRatio;

#if USE_ALG_CONTOURS
	// KSW 18.02.05 - 외곽 정보 ( 통합 툴 )
	memcpy(pResultDefectInfo->nContoursX, pResultBlob->nContoursX[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
	memcpy(pResultDefectInfo->nContoursY, pResultBlob->nContoursY[nBlobCnt], sizeof( int ) * MAX_CONTOURS );
#endif

	return true;
}

bool SviInspection::AdjustAlignInfo(tAlignInfo* pStCamAlignInfo, cv::Point* ptAdjCorner)
{
	// 임시로 외접사각형 설정
// 	ptAdjCorner[E_CORNER_LEFT_TOP].x =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_LEFT_TOP].y =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);
// 	ptAdjCorner[E_CORNER_RIGHT_TOP].x =		max(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_RIGHT_TOP].y =		min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].y,		pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].y);
// 	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].x =	max(pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_TOP].x,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_RIGHT_BOTTOM].y =	max(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y);
// 	ptAdjCorner[E_CORNER_LEFT_BOTTOM].x =	min(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_TOP].x,		pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].x);
// 	ptAdjCorner[E_CORNER_LEFT_BOTTOM].y =	max(pStCamAlignInfo->ptCorner[E_CORNER_LEFT_BOTTOM].y,	pStCamAlignInfo->ptCorner[E_CORNER_RIGHT_BOTTOM].y);

	//////////////////////////////////////////////////////////////////////////
	// 기존 사용
	//////////////////////////////////////////////////////////////////////////

	// 특정 위치로 Warping
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
	// KSW 18.02.12 - 현재 Cell 크기 비슷하도록 Warp 하기 위해 수정
	// 확대는 상관 없으나, 축소가 되면 안됨 ( 정보 사라짐 )
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

	cv::Mat mat2Warp = cv::getPerspectiveTransform(ptSrc, ptDst);		//	Warping 계수
	cv::warpPerspective(MatOrgImage, MatOrgImage, mat2Warp, MatOrgImage.size(), CV_INTER_AREA);
	cv::warpPerspective(MatDrawImage, MatDrawImage, mat2Warp, MatDrawImage.size(), CV_INTER_AREA);		// Draw Buffer 도 같이 Warping 해야함

	// SVI 는 원본 자체를 변환하므로 Corner 정보 변경
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].x			= (int)ptDst[E_CORNER_LEFT_TOP].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_TOP].y			= (int)ptDst[E_CORNER_LEFT_TOP].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].x		= (int)ptDst[E_CORNER_RIGHT_TOP].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_TOP].y		= (int)ptDst[E_CORNER_RIGHT_TOP].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].x		= (int)ptDst[E_CORNER_RIGHT_BOTTOM].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_RIGHT_BOTTOM].y		= (int)ptDst[E_CORNER_RIGHT_BOTTOM].y;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].x		= (int)ptDst[E_CORNER_LEFT_BOTTOM].x;
	m_stThrdAlignInfo.ptCorner[E_CORNER_LEFT_BOTTOM].y		= (int)ptDst[E_CORNER_LEFT_BOTTOM].y;

	// 각도 0으로 정보 변경
	m_stThrdAlignInfo.dAlignTheta = 0.0;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align 하기 전, AVI & SVI 점등 이상 확인
//////////////////////////////////////////////////////////////////////////
long SviInspection::CheckAD(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nImageNum, int nCameraNum, double* dResult, int nRatio)
{
	CCPUTimer tact;
	tact.Start();

	// 개별 알고리즘 검사 파라미터 가져오기
	double* dAlgPara = theApp.GetAlignParameter(nCameraNum);

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();
	
	// 정상 밝기와 틀리거나, 점등이 안된경우 AD 불량이므로 검사 안해도 됨. ( Align 하지 못함 )
	long nErrorCode = Align_FindDefectAD(MatOrgImage, dAlgPara, dResult, nRatio, nCameraNum, nEqpType);

	theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Inspect AD : %.2f"), tact.Stop(false)/1000.);

	return nErrorCode;
}

// Repair 설비에서 사용하는 좌표값 및 코드 판정
bool SviInspection::JudgementRepair(const CString strPanelID, ResultPanelData& resultPanelData, CWriteResultInfo& WrtResultInfo)
{
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		if (true)	// POINT 인 경우
		{
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_X = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_X + resultPanelData.m_ListDefectInfo[i].Pixel_End_X) / 2.0;
			resultPanelData.m_ListDefectInfo[i].Pixel_Repair_Y = (resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y + resultPanelData.m_ListDefectInfo[i].Pixel_End_Y) / 2.0;
			resultPanelData.m_ListDefectInfo[i].bUseReport = true;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Align 후,ROI 밝기 이상 확인
//////////////////////////////////////////////////////////////////////////
long SviInspection::CheckADGV(CString strPanelID, CString strDrive, cv::Mat MatOrgImage, int nStageNo, int nImageNum, int nCameraNum, int nRatio, cv::Point* ptCorner, ResultBlob_Total* pResultBlobTotal, double* dMeanResult, 
							bool& bChkDustEnd, bool& bNeedRetry, bool& bIsNormalDust, bool bUseDustRetry, int nDustRetryCnt, bool& bIsHeavyAlarm, ENUM_INSPECT_MODE eInspMode)
{ 	
	CCPUTimer tact;
	tact.Start();
	long nErrorCode = E_ERROR_CODE_TRUE;

	// 장비 타입
	int nEqpType = theApp.m_Config.GetEqpType();

	// 알고리즘 영상 번호
	int nAlgImg = theApp.GetImageClassify(nImageNum);

	// Alg 로그용
	wchar_t strAlgLog[MAX_PATH] = {0, };
	swprintf( strAlgLog, _T("ID:%s\tPat:%s"), (LPCWSTR)strPanelID, theApp.GetGrabStepName(nImageNum) );

	// 점등영역 AD GV값 확인 코드
	if( theApp.GetUseFindDefectAD(nImageNum, nCameraNum) )
	{
		// 개별 알고리즘 검사 파라미터 가져오기
		// AD Parameter 별도로 정의할 것. ToDo.
		double* dAlgPara = theApp.GetFindDefectADParameter(nImageNum, nCameraNum);

		// AD - 밝기값 확인
		nErrorCode = Align_FindDefectAD_GV(MatOrgImage, dAlgPara, dMeanResult, ptCorner, nEqpType, nAlgImg, strAlgLog);

		theApp.WriteLog(eLOGTACT, eLOGLEVEL_DETAIL, FALSE, FALSE, _T("Check AD GV %.2f"), tact.Stop(false)/1000.);

		// 에러인 경우, 에러 코드 & 로그 출력
		if( nErrorCode != E_ERROR_CODE_TRUE )
		{
			// Alg DLL Stop Log
			theApp.WriteLog(eLOGPROC, eLOGLEVEL_SIMPLE, TRUE, FALSE, _T("AD GV Defect - InspStop !!!. PanelID: %s, Stage: %02d, CAM: %02d, Img: %s..\n\t\t\t\t( Avg : %.2f GV )"),
				strPanelID, nStageNo, nCameraNum, theApp.GetGrabStepName(nImageNum), dMeanResult[0]);

			// 밝기 이상
			if (nErrorCode == E_ERROR_CODE_ALIGN_DISPLAY)
			{
				JudgeADDefect(nImageNum, nCameraNum, nStageNo, MatOrgImage.cols, MatOrgImage.rows, pResultBlobTotal, (int)dMeanResult[1], eInspMode);
			}
			// 버퍼가 없는 경우 / 파라미터 이상한 경우 / 등등...
			else
			{
				// 별도 불량으로 보고하는 사양 필요 - 현재 AD GV 불량으로 일괄 보고
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

// 중복 좌표 제거
bool SviInspection::DeleteOverlapDefect(ResultPanelData& resultPanelData)
{
	// nRatio : 이미지 비율 (Pixel Shift Mode - 1:None, 2:4-Shot, 3:9-Shot)

	// 불량 리스트 없는 경우 나가기
	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )
		return true;

	CRect	rcDeleteArea;
	CPoint	ptCenterSub;
	int		nAdjustRange	= 0;
	int		j				= 0;
	int		nOffSet			= 0;

	// 불량 갯수 만큼
	for (int i = 0; i < resultPanelData.m_ListDefectInfo.GetCount(); i++)
	{
		// 불량 판정
		int nDefectTypeM = resultPanelData.m_ListDefectInfo[i].Defect_Type;

		// AD 불량 제외
		if ( nDefectTypeM <= E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_END ||
			 nDefectTypeM == E_DEFECT_JUDGEMENT_MURA_CM )
			continue;

		CRect rcDeleteArea = CRect(
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_X	/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_Start_Y	/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_X		/ resultPanelData.m_ListDefectInfo[i].nRatio,
			resultPanelData.m_ListDefectInfo[i].Pixel_End_Y		/ resultPanelData.m_ListDefectInfo[i].nRatio);
					
		// 좌표계 정규화
		rcDeleteArea.NormalizeRect();

		// 범위 확장
		rcDeleteArea.InflateRect(nAdjustRange, nAdjustRange);

		// 불량 갯수 만큼
		for (j = 0; j < resultPanelData.m_ListDefectInfo.GetCount();)
		{
			// 같은 불량 비교 X
			if( i == j )
			{
				j++;
				continue;
			}

			// 불량 판정
			int nDefectTypeS = resultPanelData.m_ListDefectInfo[j].Defect_Type;

			// AD 불량 제외
			if ( nDefectTypeS <= E_DEFECT_JUDGEMENT_DISPLAY_ABNORMAL_END ||
				 nDefectTypeS == E_DEFECT_JUDGEMENT_MURA_CM )
			{
				j++;
				continue;
			}

			// 같은 불량 끼리만 중복 제거
			if ( nDefectTypeM != nDefectTypeS )
			{
				j++;
				continue;
			}

			// 불량 중심 좌표
			CPoint ptPointS;
			ptPointS.x = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_X / resultPanelData.m_ListDefectInfo[j].nRatio);
			ptPointS.y = (LONG)(resultPanelData.m_ListDefectInfo[j].Pixel_Center_Y / resultPanelData.m_ListDefectInfo[j].nRatio);

			// 범위 안에 불량 존재?
			if ( rcDeleteArea.PtInRect(ptPointS) )
			{
				// 불량 검출 패턴
				int nPat = resultPanelData.m_ListDefectInfo[j].Img_Number;

				// White 패턴이 있는 경우
				if( theApp.GetImageClassify(nPat) == E_IMAGE_CLASSIFY_AVI_WHITE )
				{
					// White 패턴으로 덮어 쓰기
					ResultDefectInfo tTemp = resultPanelData.m_ListDefectInfo[j];
					resultPanelData.m_ListDefectInfo[i] = tTemp;

					// White 패턴 크기로 정보 갱신
					rcDeleteArea = CRect(
						tTemp.Pixel_Start_X	/ tTemp.nRatio,
						tTemp.Pixel_Start_Y	/ tTemp.nRatio,
						tTemp.Pixel_End_X	/ tTemp.nRatio,
						tTemp.Pixel_End_Y	/ tTemp.nRatio);
				}

				// 현재 리스트보다 낮은 리스트 삭제 시...
				if( i > j )	i--;

				// 해당 불량 삭제
				resultPanelData.m_ListDefectInfo.RemoveAt(j);
			}
			// 중복 안돼면... 다음 불량으로...
			else j++;
		}
	}

	return true;
}


// 외곽 정보 ( 통합 툴 )
bool SviInspection::JudgeSaveContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
#if USE_ALG_CONTOURS

	// 외곽선 저장 경로 없는 경우
	if( strContourTxt == NULL )			return false;

	// 불량 없거나 검사 안할 경우 Skip
	//if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT 저장
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// 파일 열기
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		// 회전 좌표 계산 시, 사용
		double dTheta	= -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		int nCx			= m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy			= m_stThrdAlignInfo.ptAlignCenter.y;

		// 회전된 점등 영역
		cv::Point ptCorner[E_CORNER_END];

		// 회전 시, 예측 좌표 계산
		for (int m=0 ; m<E_CORNER_END ; m++)
		{			
			ptCorner[m].x = (int)( dCos * (m_stThrdAlignInfo.ptCorner[m].x - nCx) - dSin * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCx );
			ptCorner[m].y = (int)( dSin * (m_stThrdAlignInfo.ptCorner[m].x - nCx) + dCos * (m_stThrdAlignInfo.ptCorner[m].y - nCy) + nCy );
		}

		// 점등된 범위
		CRect rectROI = CRect(
			min(ptCorner[E_CORNER_LEFT_TOP].x,		ptCorner[E_CORNER_LEFT_BOTTOM].x	),
			min(ptCorner[E_CORNER_LEFT_TOP].y,		ptCorner[E_CORNER_RIGHT_TOP].y		),
			max(ptCorner[E_CORNER_RIGHT_TOP].x,		ptCorner[E_CORNER_RIGHT_BOTTOM].x	),
			max(ptCorner[E_CORNER_LEFT_BOTTOM].y,	ptCorner[E_CORNER_RIGHT_BOTTOM].y	) );

		// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
		rectROI.left	/= m_stThrdAlignInfo.nRatio;
		rectROI.top		/= m_stThrdAlignInfo.nRatio;
		rectROI.right	/= m_stThrdAlignInfo.nRatio;
		rectROI.bottom	/= m_stThrdAlignInfo.nRatio;

		// 최상위 헤더 작성 ( Cell 크기 )
		strContourIndex.Format(_T("Cell_X=%d, Cell_Y=%d\n"), rectROI.Width(), rectROI.Height());
		fileWriter.SeekToEnd();
		fileWriter.WriteString(strContourIndex);

		// 불량 갯수 만큼
		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++ )
		{
			// UI 패턴 순서
			int nImgNum = resultPanelData.m_ListDefectInfo[i].Img_Number;

			// 헤더 파일 쓰기 ( No. || 패턴 정보 || 카메라 번호 || 불량명 )
			strContourIndex.Format(_T("No=%d, Pattern=%02d, Camera=%02d, Defect=%02d\n"), i+1, theApp.GetImageClassify(nImgNum), resultPanelData.m_ListDefectInfo[i].Camera_No, resultPanelData.m_ListDefectInfo[i].Defect_Type);
			fileWriter.SeekToEnd();
			fileWriter.WriteString(strContourIndex);

			// AD 불량인 경우
			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP )
			{
				// 사각형 E_CORNER_LEFT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), 0, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_TOP 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width()-1, 0);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_RIGHT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), rectROI.Width()-1, rectROI.Height()-1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				// 사각형 E_CORNER_LEFT_BOTTOM 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), 0, rectROI.Height()-1);
				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);

				continue;
			}

			// SVI는 무라불량이므로, 모두 표시
			// 외곽선 정보
			for (int j=0 ; j<MAX_CONTOURS ; j++)
			{
				// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
				// CFeatureExtraction::calcContours() 에서 이미 1샷 좌표로 변경되서 들어옴
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// 값이 없는 경우 나가기
				if( x==0 && y==0 )	break;

				// E_CORNER_LEFT_TOP 좌표를 원점으로 사용
				x -= rectROI.left;
				y -= rectROI.top;

				// 예외처리
				if( x < 0 )						x = 0;
				if( y < 0 )						y = 0;
				if( x >= rectROI.Width() )		x = rectROI.Width()-1;
				if( y >= rectROI.Height() )		y = rectROI.Height()-1;

				// 외곽선 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d\n"), x, y);

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);
			}
		}

		// 파일 열린 경우만 닫기
		fileWriter.Close();
	}

#endif

	return true;
}


// Mura 외곽선 정보 저장
bool SviInspection::JudgeSaveMuraContours(ResultPanelData& resultPanelData, wchar_t* strContourTxt)
{
	// 외곽선 저장 경로 없는 경우
	if( strContourTxt == NULL )			return false;

	// 불량 없거나 검사 안할 경우 Skip
	if( resultPanelData.m_ListDefectInfo.GetCount() <= 0 )	return true;

	// TXT 저장
	CStdioFile	fileWriter;
	CString		strContourIndex;

	// 파일 열기
	if( fileWriter.Open( strContourTxt, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite ) )
	{
		// 회전 좌표 계산 시, 사용
		double dTheta	= -m_stThrdAlignInfo.dAlignTheta * PI / 180.;
		double	dSin	= sin( dTheta );
		double	dCos	= cos( dTheta );
		int nCx			= m_stThrdAlignInfo.ptAlignCenter.x;
		int nCy			= m_stThrdAlignInfo.ptAlignCenter.y;

		// 불량 갯수 만큼
		for (int i=0 ; i<resultPanelData.m_ListDefectInfo.GetCount() ; i++ )
		{
			// AD 불량인 경우
			if ( resultPanelData.m_ListDefectInfo[i].Defect_Type <= E_DEFECT_JUDGEMENT_DUST_GROUP )
				continue;

			// 불량 판정
			int nDefectType = resultPanelData.m_ListDefectInfo[i].Defect_Type;

			// SVI는 무라불량이므로, 모두 표시

			// 외곽선 정보
			for (int j=0 ; j<MAX_CONTOURS ; j++)
			{
				// P/S 모드에 따라 좌표 수정 ( 1샷 좌표로 변경 )
				// CFeatureExtraction::calcContours() 에서 이미 1샷 좌표로 변경되서 들어옴
				int x = resultPanelData.m_ListDefectInfo[i].nContoursX[j];
				int y = resultPanelData.m_ListDefectInfo[i].nContoursY[j];

				// 값이 없는 경우 나가기
				if( x==0 && y==0 )	break;

				// 외곽선 정보 파일 쓰기
				strContourIndex.Format(_T("%d, %d, %s\n"), x, y, theApp.GetDefectTypeName(nDefectType));

				fileWriter.SeekToEnd();
				fileWriter.WriteString(strContourIndex);
			}
		}

		// 파일 열린 경우만 닫기
		fileWriter.Close();
	}

	return true;
}
